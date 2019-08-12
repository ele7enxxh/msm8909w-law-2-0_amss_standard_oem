/*===========================================================================
         R R C   I N T E R - R A T   H A N D O V E R   M O D U L E

DESCRIPTION

  This module initiates and coordinats inter-RAT handover procedure including
  both WCDMA to GSM and GSM to WCDMA CS handovrs.


EXTERNALIZED FUNCTIONS

  rrciho_event_handler            Main event handler for this module

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrciho.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   sn      Changes for LTE U feature
05/25/16   sp      Made changes to add call end metrics - FR#30856
05/25/16   sp      Made changes to convert ota metric msg_id to enum
05/25/16   sp      Made changes to log serving cell QSH metrics at metric config 
05/25/16   ad      Made changes to log important parms from OTAs
05/25/16   sp      Made changes for Mav debug snapshot
03/04/16   sas     Made Changes to fail HO from UTRAN to GSM, when the OTA has no proper padded bits for GSM command list.
11/04/15   ad      Made changes not to apply the pending cs keys on wtog handover
10/16/15   as      Added code to support LTA durting activation time
09/22/15   sp      Made changes to buffer RLC reset indicaiton in stand by mode.
07/13/15   sas     Made changes to honor MPLMN srch req when abort for WtoX irat procedure is active.
07/01/15   sp      Made changes to save procedure substates for LTE commands and to skip logging frequent internal commands
06/10/15   bc      Made changes to blindly change the TRM priority for L2W PS HO after HO is done when ciphering is not present.
04/06/15   bc      Changes to handle PSHO Abort req when failure response is already sent for PSHO.
03/27/15   sas     Changes to handle IHO_abort_REQ while waiting for Activation_Rsp from NAS.
11/21/14   bc      Made changes to not to change TRM priority after L2W PSHO if CSFB call status is set
11/10/14   sp      Made changes to save procedure substates in crash debug info
10/22/14   as      Changes are made to reset number_of_est_rabs before processing inter-RAT HO message
10/17/14   sr      CSFB call optimization changes
10/11/14   sg      Made changes to reset num_est_rab in gtow_ho global is HO to utran fails
09/16/14   rmn     Resolved compilation errors in WRRC for LTE removal flavor
08/08/14   rmn     Made changes to request lock on supported band indicated by NAS rather 
                   than deriving it from Japan PLMN and freq.
07/25/14   sg      Made changes to support band II for 
                   single carrier 3C
07/25/14   vi      Made changes to fix KW error.
07/23/14   sp      Made changes to remove F3s as part of FR19999
07/18/14   vi      Fixed KW error
07/15/14   ac      NV reorganization
12/30/13   sg      Made changes to add initialization of local variables and bound conditions 
                   to access array to avoid warnings and invalid memory access. 
07/14/14   rmn     Made changes to attempt trm_exchange for all XtoW IRAT scenarios 
06/24/13   ad      Made changes to send RRC_CIPHER_SYNC_IND for LTE to W handover scenario
06/20/14   sa      Made changes to update UL 16QAM
08/06/14   sg      Made changes to remove the reset of codec mode if HO message
                   includes a PS RAB
06/19/14   rmn     Made changes for DR-DSDS Feature
06/09/14   sp      Made changes to access correct structures for rel-9 Handover to UTRAN OTA
05/20/14   rmn     Made changes to support FRLTE when indicated GtoW HO Req and also indicate CSFB status in WtoG HO Req.
05/15/14   gv      Made changes to reduce the TRM priority to BG TRAFFIC post L2W PS HO
05/14/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/14/14   vi      Made changes to update CM with invalid cell id when UE camps back on same W cell after G2W handover
04/23/14   bc      Made changes to send cell info to LTE on receiving UE capability request
                   and also to send cell info WTOL PSHO request.
05/06/14   geg     Use safer versions of memcpy() and memmove()
05/02/14   gv      Made changes to attempt IRAT activity only after clean up of pending BPLMN
04/16/14   sp      Made changes to add save LTE_RRC_WCDMA_PSHO_REQ to crash debug info.
04/11/14   gv      Made changes to exchange trm before W2L PS HO to prevent GSM from sneaking 
                   in for low priority activity
04/10/14   db      Made changes to enable/disable feature support in DSDS/DSDA/TSTS modes through NV
04/10/14   sg      Made changes to correct the feature flag used for rel 10 spec CRs
04/02/14   db      Made changes to memset memory allocated for ue_RadioAccessCapabBandFDDList4 capability 
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/26/14   vi      Made changes to remove printing event for L to W handover.
03/20/14   gv      Made changes to save the abort request received while HO is in progress
                   and act on the same after channel config is done
03/13/14   sp      Made changes to include as_id in RRC_ESTABLISH_IND and RRC_SYNC_IND
03/13/14   gv      Made changes to do TRM exchange before sending the failure reponse for HO
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
03/13/14   sr      Made changes to not perform TRM exchange in single sim mode
02/13/14   ad      Made changes to remove redundant F3s
09/25/13   ad      Made changes to split NV control of FDPCH and CPC DTX/DRX features
01/31/14   sr      Made changes to set instance id for PSHO request to LTE
01/15/14   sp      Fixed compiler warnings
01/10/14   sa      Made changes to append the EDPCCH POWER BOOST in INTERAT HANDOVER INFO MESSAGE 
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
12/30/13   sr      WRRC support for new requirements of LWG DSDS
12/24/13   db      Introduced new NV to allow CPC,HSFACH,HSRACH,DC-HSDPA in DSDS and DSDA mode
11/08/13   ad      Made changes to indicate ho_type as part of codec info to CM
12/09/13   sp      Made changes for OSYS 6.6.5 migration
12/03/13   sg      Made changes to update review comments for 3C 
11/27/13   sp      Made changes to add nv support and to save time ticks for 
                   rr to/from rrc mm to/from rrc and to add msg_type for crash debug ota changes
03/08/14   vg      Made changes to reduce the stack size usage of some RRC functions which has the usage more than 500 bytes.
11/16/13   sr      WRRC support for LWG DSDS
11/14/13   sa      DC feature support will be controlled thru UE Cat
11/14/13   sa     Made the changes for Rel-10 spec CRCR4566 under FR16756
11/12/13   vi     Added AS ID in CYPHER_SYNC_IND command
10/30/13   as      Made changes to add feature code for FEATURE_WCDMA_3C_HSDPA
10/27/13   as      Made changes to backup OTAs for crash debugging
10/21/13   ad      Providing value of rrciho_mvs_control_flag to NAS if as_id matches with 
                   the NAS provided as_id through an API
10/07/13   rmn     Changes for Feature: RAT Search Order for FDD/TDD LTE
09/25/13   bc      Added code to support exchange of AS ID when LTE feature is enabled
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/15/13   bc      Made changes to crash the UE when gtow_info.rbs_ptr is NULL
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
07/08/13   sn      Changes for LTE CSG feature
06/28/13   ad      Made changes to send codec info to CM for SRVCC handover
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/26/13   sr      Made changes to trim the GSM message to be included in HO failure message 
06/24/13   as      Made changes to send RRC_CIPHER_SYNC_IND to G whenever new keys are applied 
06/04/13   sg      Fixed KW errors
05/29/13   vg      Made changes to have HSDPA category ext2 code under compiler flag FEATURE_WCDMA_DC_HSDPA in rrc_get_utran_classmark_change_handover_info.
05/31/13   vg      Added code for CS/PS capbability check before processing L->W PSHO
05/22/13   sa      Added code changes to support FEATURE_WCDMA_DC_HSUPA
05/15/13   db      Made changes to enhance NV 67333 to control FDPCH and CPC DTX/DRX features independently
05/13/13   vi      Fixed compiler and KW errors for Bolt
05/07/13   sr      Made changes to configure TX diversity for CLTD
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
04/22/13   as      Made changes to to send IHO failure if CU is active
04/16/13   db      Made changes to check the right RL database during validation in R5 XtoW HO case
04/19/13   sa      Made changes to update the call type mask for inter system handover from G to W with default config id present 
04/16/13   db      Made changes to check the right RL database during validation in R5 XtoW HO case
04/03/13   sr      Made changes to indicate the tx diversity mode received in handover 
                   message to L1 in ACQ request
03/14/13   md      Made changes to provide active subscription ID to NAS APIs for DSDA
03/06/13   vi      Made changes to sends eUTRA capabilities in UE cap info on W after L2W PSHO 
02/04/13   sn      Changes to fill handover cause in GtoW handover cipher request
01/03/13   ad      Made changes to reduce the redundant F3s
12/27/12   esr     Added boundary checks during memcpy to avoid buffer overflow
12/12/12   mp      Fixing KW errors
12/04/12   ad      Made changes to send start of SRVCC indication to CM 
11/30/12   ad      Made changes to remvoe the dependancy on  rfm_band_supports_dual_carrier
11/29/12   gv      Made changes to fill the length of RRC class mark info as zero 
                   when none of the WCDMA bands are enabled
11/27/12   gv      Fixed compiler warnings when DSDS is enabled
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
10/22/12   pm      Made the changes to update the RAB type for pre R8 handover messages also
10/11/12   sg      Made changes for Rel 9 migration, removing unsupported IEs and ZI reductions. 
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
10/04/12   pm      Removed unnecessary comments
09/28/12   pm     Added NV support for MAC_I
09/26/12   pm      Made changes to reset the ordered codec when procedure is cleared
08/31/12   rd      Transfer dedicated priority parameters to/from WCDMA and GERAN
                   during IRAT procedure
08/29/12   db      Additional changes to disable EFDPCH through NV
08/21/12   ad      Made changes to handle default config 1 for GtoW HO
                   These are needed because of recent optimiziations for SRVCC
07/06/12   sn      Changes to filter out bands properly
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/03/12   sks     Changes to indicate interrat HO capability to L-RRC
06/12/12   sa     Made the changes to keep the active codec info
05/10/12   md      Added code changes to support FEATURE_MAC_I
04/13/12   sks     Fixed compilation warning.
04/12/12   as      Added code to support feature NB/WB handover support
04/02/12   db      Made changes to disable CPC and FDPCH through NV
03/28/12   db      Leftover feature cleanup of FEATURE_MODEM_MBMS
03/27/12   sks     Added support for abort handling during WTOL PS handover.
03/19/12   sks     Fixed compilation error.
03/15/12   gv      Fixed compilation warning
03/14/12   gv      Merged changes to not wait for L2 Ack for HO complete and send rab_est_ind
                   to RABM immediately
03/09/12   as      Added changes to send start values only if l2w ps HO is enabled
03/07/12   sks     Changes were made to pass time-tx info from WL1 to LRRC.
02/23/12   sks     Added support for WTOL PSHO.
02/20/12   sks     Made changes to support default config for SRVCC.
02/20/12   sks     Added support for SRVCC failure indication.
12/28/11   sa      Mainlined the FEATURE_WCDMA_CHECK_NAS_ACQUIRED_VOC
12/14/11   md      Removed extern declaration for ghdi api funs
12/13/11   sks     Added support for LTOW SRVCC.
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
10/13/11   db      Made changes to restore/clear ordered codec when RB failure occurs
08/30/11   ad      Made changes to handle default configuration 23
08/12/11   sks     Made changes to use DSM pointer to transfer HO container from LRRC to WRRC.
08/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.103.00
08/01/11   ad      Fixed compilation error if FEATURE_IRAT_HO_INFO_COMPRESSED enabled
07/28/11   sks     Made changes to send correct STTD info to L1 for LTOW PSHO.
07/18/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.102.00
07/07/11   ad      Fixed compilation error under FEATURE_IRAT_HO_INFO_COMPRESSED 
07/08/11   sks     Made changes to handle the cmd PSHO_ABORT_REQ correctly from LRRC.
07/06/11   gv      Fixed KW errors
07/06/11   ad      Made changes to copy hanging rbmapping info from OC to CC
                   if channel config request is sucessful
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/08/11   as      Made changes to not report CPC capability if Tuneaway is active
06/14/11   sks     Made changes to use the timing info from LRRC for ACQ during LTOW PSHO.
05/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.97.50
05/06/11   ad      Fixed bug to change the check from rel7 to rel8 for R8 HO message
05/03/11   rl      Fixed the lint error
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
04/26/11   sks     Fixed compilation errors when FEATURE_VOC_AMR_WB is defined.
04/25/11   rl      Fixed the KW error
04/18/11   kp      Added code to save mapped ck/ik for L2W PSHO no cipher case.
04/12/11   sks     Added debug prints.
04/08/11   sks     Featurised code for checking RF band support for DC.
04/06/11   sks     Made changes to check for RF band support for dual carrier to
                   indicate dual carrier support in INTERRAT HANDOVER INFO.
03/28/11   rl      Merged the HS-FACH/PCH changes from mainline
02/15/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.91.50
02/09/11   su      Added code to free the gtow_info.sib16_ptr before calling 
                   mem_deinit_heap during G2W HO failure to avoid Error Fatal.
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/09/11   rl      Fixed compiler warning
02/08/11   rl      Fixed the compilation warnings
02/07/11   rl      Merged with the fix in the function rrc_get_utran_classmark_change_handover_info
                   to relsove the crash
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/22/10   rm      Made changes to indicate PCH UE pos capability
11/19/10   prk     Indicate e-fach capability if UE supports e-fach, irrespective
                   of network support.
11/29/10   sks     Fixed bugs found during testing.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/29/10   su      Made changes to remove WCDMA_TASKLOCK() usage from RRC code base.
10/22/10   prk     Acquire mvs vocoder only if nas has already acquired 
                   in case of W -> G HO failure scenarios.
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
09/20/10   prk/kp  Added code to support FEATURE_WCDMA_HS_FACH
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/22/10   kp      Fixed compilation errors
06/16/10   kp      Added code to indicate UE support of feature “Two DRX schemes in 
                   URA_PCH and CELL_PCH” and “E-DPDCH power interpolation formula”
                   for Rel-7 & above UE's.
05/17/05   gkg     Made changes to correct the advertisement of MIMO Tx Diversity 
                   for DL channels in InterRATHandoverInfo message.
05/13/10   sk      Re-Fix lint warnings.
05/12/10   gkg     Added support for sending Tx Diversity for DL Control Channels
                   for MIMO Devices i.e. IE supportofTxDivOnNonMIMOChannel.
05/10/10   gkg     Fixed Compile Warnings.
05/07/10   prk     Fixed compiler warnings.
05/06/10   gkg     Fixed KW Error.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
04/23/10   as      Fixed Klocwork issues
04/15/10   gkg     Fixed KW Errors in R8 Code.
04/08/10   gkg     Changed NAS API name from ghdi_has_nas_already_acquired_voc to 
                   mn_has_nas_already_acquired_voc()
03/29/10   dm      Made changes to free dynamic pointers if WtoG HO fails.
03/25/10   gkg     IE "hsdsch_physical_layer_category_ext2" should be included
                   in GSM classmark Info only if the HSDPA CAT is set to 21 to 24.
03/15/10   rm      Added code to support FEATURE_RAT_PRIORITY_LIST for W<->G IRAT
03/15/10   as      Updated RRC Code to support ASN.1 8.9 version
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
03/02/10   as      Fixed Klockwork issues
02/11/10   gkg     Made changes for Opt Timer for CSoHS.
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/05/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
11/25/09   ss      Added Release Independent Band (FDD2) support.
10/13/09   gkg     Made the changes such that Codec info is set to RRC_CODEC_NONE
                   if CS DATA TYPE is mentioned in RRC_SYNC_IND.
10/26/09   rm      Fixed Klockwork errors
10/07/09   gkg\kp  Changes for REL8 under FEATURE_WCDMA_REL8 and CPC-DTX
10/01/09   gkg     Made changes so that CS RAB Type is provided to MM in case
                   of G2W HO.
09/01/09   ps      Changed has_nas_already_acquired_voc() to 
                   ghdi_has_nas_already_acquired_voc() according to NAS requirement
08/17/09   ss      Added code to provide ciphering ON/OFF indication to UI 
                   under feature flag FEATURE_UMTS_VOICE_CIPHERING_IND.
08/17/10   ps      Made changes FEATURE WCDMA CHECK NAS ACQUIRED VOC to acquire
                   the vocoder during Inter-Rat HO only if NAS had acquired it before
                   i.e. the the NAS level signalling to connect a voice call is already
                   done 
05/08/09   ss      Updated Copyright Information
05/05/09   rm      Modified rrc_get_classmark_change_handover_info to wrapper function,
                   so that WCDMA_TASKLOCK/WCDMA_TASKFREE are used if RRC is active
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/25/09   rm      Made changes to go to INACTIVE from IDLE_DETACHEED on failure to extract
                   Ho to UTRAN message
04/13/09   gkg     In case of G->W Handover, if OC couldn't be set by IHO
                   procedure, wait for RRCIHO_WAIT_INACTIVE_CNF substate
                   instead of RRCIHO_WAIT_CHAN_CONFIG_GTOW_CNF substate so
                   that later attempts of G->W hanovers are not blocked.
03/31/09   rm      CMI Featurization
03/30/09   ss      Added support to prevent cipher_config step if ciphering Algo I.E is 
                   not included in the OTA message.
02/16/09   rm      Made changes to prevent pre-emption of GAN-task when GAN calls RRC
                   function to get the UTRAN classmark in its task context.
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
12/29/08   rm      Added support for 3G<->GAN Handover
12/08/08   ps      Made changes to set the MAC-HS(R5/R6 HS-DSCH )CAT 
                   according to global variable "rrc_machs_cat" when the 
                   HSDPA CAT is above 12 for R7
11/28/08   gkg     Made Changes for new MVS-RRC Interface to (de)configure vocoder
                   for AMR Voice Calls.
10/10/08  ps       Removal of feature FEATURE_GTOW_HO_TO_UTRAN_SPOOF_ENABLE
                    As the Spoof  is no longer required for Anritsu
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
09/08/08   gkg     In case of handover To UTRAN, first update the rrc_est_rabs, then
                   use it to configure MVS. Similarly, in case of handover From UTRAN
                   failure, use rrc_est_rab to restore MVS configuration.
04/18/08   gkg     Fixed Klockwork errors.
04/15/08   kp      Fixed lint errors.
04/14/08   gkg     Made changes for configuration of AMR Call so that no. of AMR
                   classes in UL to be registered with MVS is 3 whenever no. of RBs in
                   voice RAB is 3. This is done irrespective of amr Rate.
01/04/08   gkg     Added support for asymmetric AMR call by updating DL rlc ids, 
                   tr-ch ids and cctrch ids separate from UL. 
27/03/08   pk      Added support for feature FEATURE_IRAT_HO_INFO_COMPRESSED.
20/03/08   gkg     Added support for configuring MVS for AMR call in DL using DL CTFCs
                   instead of relying on UL CTFCs.
03/10/08   kp      Added support for SIB11bis. Code changes are under feature flag 
                   FEATURE SIB11BIS SUPPORT 
03/07/08   vr/da   Included FEATURE_MODEM_MBMS feature for SIB read abort in DCH state
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
22/02/08   ps/gkg  To support Spec CR 3160, DC 12 is removed for rel99 and rel5.
01/24/08   ps      Made changes to check IHO internal database for Voice RAB than
                   EST RAB to set the NAS Sync IND when FEATURE_VOC_AMR_WB is defined.
01/08/08   ps/gkg  Made changes to query the RAb type from MN even for 
                   Handover To utran msg.
12/21/07   vr      Support for FEATURE MBMS. Initialise MBMS information in RAB structures
26/11/07   gkg     Added support for VT->Voice fallback and vice versa
                   by RB-Reconfig msg for SCUDIF.Also added changes to 
                   support VT->Voice fallback using RB-Setup msg.
                   Changes under feature flag FEATURE_SCUDIF_ENHANCEMENT
10/30/07   ps      Added support for DC 14
10/23/07   ps      Added support for DC 20 through Rb-Setup msg
10/22/07   ps      Made correction for compiler error when WB-AMR is enabled
                   for REL6 targets.
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
07/23/07   kp      Added support for spec cr 2567-Handling of security key set 
                   generated in other RAT at G to W handover.Code changes are under 
                   feature flag FEATURE_SPEC CR 2567 GTOW SECURITY KEY HANDLING.
07/11/07   vr      Initialize bitmask of fddPhysicalChannelCapab_hspdsch_edch IE to
                   include dl_CapabilityWithSimultaneousHS_DSCHConfig and set the value
                   appropriately. If left uninitialized, it may lead to constraint violation
                   in ASN1 encoding of InterRATHandoverInfo message.
06/25/07   da      Corrected RAB and SRB info handling in R6 and R5 handover to utran msg.
04/10/07   ps      Added support for default config 6 sent through RB-Setup msg.
03/26/07   vr      Support for populating FDD2 bands in InterRAT Handover Info (Classmark)
                   message. In the earlier versions of ASN1, there was no provision 
                   to populate FDD2 bands in this message. In the recent versions container 
                   IE has been added in InterRAT Handover Info message that can carry FDD2 
                   information.
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB. Also added support for default config
                   13,16 and 21
02/05/07   ps      Added support for default config 15,18 and 19
02/05/07   ps      Made correction to handling of REL6 HandoverToUtran msg
12/22/06   ps      Made changes to handle postVerificationPeriod and the
                   timingMaintainedSynchInd IE received in OTA messages
11/30/06   sm      Added release 6 code under NV 'rrc_nv_rel_indicator'
11/16/06   da      Added initial support for R6 Handover from utran msg.
11/15/06   sm      Added support to indicate HS/EUL category in inter-RAT-
                   HO-info as part of v690 non-critical extensions
10/18/06   ps      Added support for default config 22
09/27/06   sgk     Fixed 7200 compiler warnings.
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/23/06   ps      Added support for default config 10
08/21/06   tkk     Added SRB#5 support.
07/25/06   vr      Set failure cause appropriately when extraction of Single GSM
                   message fails. Else, this will result in ASN encoding error
                   while sending HandoverFromUTRANFailure message
06/21/06   vr      Support to indicate rx_tx_TimeDifferenceType2 Capability
06/08/06   vm      RR will now also pass the plmn the UE is camped on the GSM side
                   in the GtoW handover request. And this PLMN information along with
                   the UARFCN will be used to determine whether the PLMN-UARFCN 
                   combination is correct or not.
05/25/06   vr      Assign default SRB Ids if the IDs are not included in the 
                   HandoverToUTRAN message
05/08/06   vr      Added check to process only R5 part of critical extensions in
                   HandoverToUTRAN message
03/15/06   vg      fixed lint warnings
03/07/06   vr      Modified the function call rrcueci_append_ue_radio_access_capability()
                   since the function now takes an additional paramter v650ext
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/21/06   vr      Fill Interrat reason in change mode request
01/25/06   bd      Lint clean-up
01/23/06   vr      Set Access stratum Indicator to R5 in handover classmark info
11/16/05   vr      Abstracted Rel5_ASN1 dependant IE(default config) to
                   rrcasn1i.h file.
11/11/05   tkk     Lint correction for Interrat HO config type & few MSG_HIGHs
                   for debugging purpose.
11/10/05   vr      Changes for switching between HEDGE and WEDGE using NV Item
                   All Rel 5 changes are now under the nv check.
11/10/05   tkk     Added support for Rel-5 Interrat HO To UTRAN.
10/19/05   vr      In case of a GTOW handover,turn on the vocoder only
                   if a CS Voice is getting established.For Signalling only
                   handovers,do not setup MVS
09/29/05   vr      Bug fix in Vocoder IHO optimization.Enable the vocoder
                   on Cell Update Confirm after Resume failure.
09/02/05   vr      Assign appropriate failure causes for GTOW HO failure
09/02/05   vm      Added support for sending release cause in RRC_ABORT_CNF
                   to MM.
08/24/05   tkk     Added support for default configurations 4,7,8 & 9.
08/17/05   vr      On reception of HandoverFromUTRAN message,check to see if
                   Ordered Config is set.If Set,reject the handover message
07/29/05   da      Made change to update var mac_rab_status
07/15/05   vr      Remove the references to the field band_pref in both WTOG
                   and GTOW cases.We will not be using this field anymore
                   since the BCCH_ARFCN type itself has the band information.
07/05/05   kc/vr   New interface to check if W->G IHO is active or not.
04/14/05   vr      Register for "any state"->idle state change indication instead
                   of just DCH->Idle.
04/04/05   vr      Release the WCDMA vocoder after a G->W handover
                   when a CS voice RAB is released
03/30/05   vr      Support default config 1 for GtoW handover
03/23/05   vr      Check if ASN1 decoding on HandoverFromUTRAN command is successful
                   before processing the command.Also changed an ERR_FATAL message
                   to MSG_ERROR
03/04/05   kc      Added support for Resumption failure on W after a WtoG HO failure
03/02/05   ttl     Support RRC ASN1 Release 5.
02/23/05   kc      Interrat HO optimization support
01/24/05   vr      Make the count-c activation time to be a multiple of 8
                   (spec)in the ho_to_utran_complete message
01/05/05   vr      In case of a G2W handover,first check if the frequency
                   specified in the handover command is supported by the MS.
                   If not,send handover failure with cause as
                   RR_RRC_FREQUENCY_NOT_IMPLEMENTED.Do not even start the acq.
12/27/04   vr      Fixed more lint errors
12/21/04   vr      Fixed lint errors
12/17/04   vr      Added support for WCDMA->1x handover feature
12/01/04   vr      Corrected powercontrolalgorithm stepsize to 1db from 2db
11/23/04   kc      Added support for dynamic power class.  Also added correction
                   to UL TPC feature during WCDMA resume after a failed HO to G
11/12/04   kc      Fixed lint errors
09/17/04   kc      Synced up to new interfaces to send UE-Capability in HO
                   Info Transfer message.
08/20/04   jh      Updated to use new APIs for enhanced AMR processing.
08/19/04   sgk     Included rrccsp.h to fix lint errorcall to function
                   'rrccsp_fill_info_for_rr()' not made in the presence of a
                   prototype and Symbol 'rrccsp_fill_info_for_rr' undeclared,
                   assumed to return int
                   Assigned the right enum values to gtow_ho_cnf.failure_cause
                   in function rrciho_send_handover_to_utran_failure to fix lint
                   error mismatch (assignment) (int/enum)
                   Assigned procedure_info.protocol_error =
                   rrc_messageNotCompatibleWithReceiverState; instead of
                   rrc_messageNotCompatibleWithReceiverState_chosen; in function
                   process_handover_from_utran_command_gsm to fix lint error
                   Type mismatch (assignment) (int/enum)
07/23/04   kc      Fixes for March 04 ASN1 notation.
07/19/04   kc      Changed position of incrementing SRB/RB counters to eliminate
                   un-necessary warning statement
06/01/04   bu      Added support for network_select_mode and band_pref in W<->G
                   handover requests.
05/19/04   kc      Code to err_fatal on diff lines for different encoding errors.
05/11/04   jh      Conditionally enabled new MVS interface for multi-rate AMR.
04/26/04   kc      Fixed a typo to check for to_mode instead of from_mode before
                   initializing IHO procedure internal data.
02/26/04   kc      Fix for mem_leak and mem_corruption causes when WtoG fails with
                   protocol_error_cause.
10/21/03   kc      Append START values to ABORT_CNF to write back latest START
                   for each domain during an abnormal call release.
10/21/03   kc      Added check to reject Pre-defined configuration in HO To
                   Utran Command.
10/14/03   kc      Merged fixes found during E IOT pertaining to GtoW HO, and
                   Handover Info Transfer.
09/23/03   kc      Fix for CR35240. Corrected Default Config IE for Trch-Id 3
08/07/03   kc      Added code for GtoW Handover Events
07/23/03   kc      Fix for CR33286. Cleared act_time_present flag when HO procedure
                   is completed during a WTOG handover.
07/01/03   kc      Added support for Non-Blind HO.  PN_POS and STTD info is
                   populated into ACQ_REQ and sent to LLC if measurement info
                   is received from RR in Handover_Req.
06/05/03   kc      Added Procedure_ID to rrcsmc_append_start_list function to
                   sync up to interface changes in SMC procedure.
05/30/03   kc      Initialized Re-establishment timer to useT314 in case of
                   Default configuration being used in GtoW Handover
05/29/03   kc      Rolled back some of the MVS interface changes to under a
                   feature defn.
05/27/03   kc      Lint fixes with GTOW HO feature turned on.
05/23/03   kc      Changes to support MODE_CHANGE_IND to inactive from mode_change
                   state.  This is needed as IHO procedure cleans up it's internal
                   state machine when we are directed to FTM mode.
05/08/03   kc      Merged MVS AMR related modifications
05/07/03   kc      Changed initialization of Default Config to be done when HO
                   is received.  This is required as the spoof flag can now change
                   dynamically at run time.
05/02/03   kc      Changed DEBUG_INTEGRATION_TEST_GTOW_HO_TO_UTRAN flag name to
                   FEATURE_GTOW_HO_TO_UTRAN_SPOOF_ENABLE to sync with RR.
05/01/03   kc      Added support for Inter-RAT GtoW Ciphering Handover.
04/30/03   kc      Added support for Single_GMS_Message in WtoG Handover.  This
                   will fix CR28114.  Also removed DEBUG feature for GtoW HO.
04/28/03   kc      Added support for Activation Time in WtoG Handover.
04/25/03   kc      Fix for CR29889.  Also added a check to return HO failure
                   if more than one Pri-SCR is received in GtoW HO command
04/11/03   kc      Added WTOG Handover Start event logging support.
03/24/03   kc      Added code to update ciphering status before sending WtoG
                   handover req to RR.
03/20/03   kc      Minor bug fix changed order of Sending HO to UTRAN after
                   Starting Vocoder and indicating MM with SYNC_IND
03/20/02   kc      Updated Default Config 3 and Default Config 0 to update the
                   Default Configurations for spec CR's1393, 1451, 1844.
03/07/03   xfg     Fixed a lint related issue that caused HO failure
03/06/03   kc      Corrected handling of Handover From UTRAN failure protocol
                   Error cause.
03/05/03   kc      Updated HandoverToUTRANCommand interface to read length
                   correctly as IEI field for Handover message is not sent
02/27/03   kc      Change interface to sync up with rce file to update rce
                   substate to final.
02/21/03   kc      Added fixes for Failure checking.  Also added classmark
                   updation dummy code.
02/18/03   kc      Added support for GtoW feature.
11/08/02   sk      Included "rrcrce.h" file to get the prototype of
                   rrcrce_procedure_event_handler() (detected by lint)
11/08/02   xfg     Get inter-RAT measurement data before sending out Suspend
10/29/02   xfg     Added changes from the code review
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
10/17/02   xfg     Fixed a procedure id bug
10/15/02   xfg     Added support for reverting back to UTRAN when inter-RAT HO
                   fails
10/10/02   xfg     Added free gsm_commands in rrciho_init_data.
10/10/02   xfg     Informed Direct Transfer procedure to forward NAS data to RR
                   for a WCDMA to GSM handover.
09/17/02   xfg     Added handling for RRC_ABORT_REQ and uncommented out
                   l1_get_interrat_measurement_data
08/16/02   xfg     Commented out l1_get_interrat_measurement_data for now
08/07/02   xfg     Added the original GSM message list to Handover from UTRAN
                   failure
07/10/02   xfg     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "bit.h"
#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "customer.h"
#include "memheap.h"
#include "rrcasn1util.h"
#include "rrcasn1.h"
#include "rrcdata_v.h"
#include "rrcsend.h"
#include "rrclcm.h"
#include "rrcmcm.h"
#include "rrcscmgr.h"
#include "rrccmd_v.h"
#include "rrcintif.h"
#include "rrcllc.h"
#include "rrcrbcommon.h"
#include "rrcdt.h"
#include "rrcrce.h"
#include "rrcsmc.h"
#include "rrccho.h"
#include "rrciho.h"
#include "rrcueci.h"
#include "rrccsp.h"
#include "mm_umts.h"
#include "sys_plmn.h"
#include "rrcnv.h"
#include "rrccui.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrc_rr_types.h"
#include "rrcrrif.h"
#include "l1rrcif.h"
#include "rr_rrc_if.h"
#include "rrcmmif.h"

#include "seq.h"
#include "rrclogging.h"
#include "event.h"

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif

#include "rrcllcoc.h"
#include "rrcsibproc.h"

/*Added to query RAb type.*/
#include "rrcmnif.h"

#include "rrccspi.h"
#include "rrcmisc.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif

#include "rrcdormancy.h"

#ifdef FEATURE_LTE_TO_WCDMA
#include "rabmtask.h"
#endif /* FEATURE_LTE_TO_WCDMA */

#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO
#include "rrcmeas.h"
#endif
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* UMTS->CDMA handover parameters.
*/
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif

#ifdef FEATURE_WCDMA_TO_LTE
typedef struct {
  /* Message to be sent to LRRC */
  lte_rrc_wcdma_psho_req_s   msg;

  /* DSM pointer to the EUTRA message */
  dsm_item_type   *dsm_ptr;

  /* Size of EUTRA container */
  uint32  eutra_container_size;
  /* A pointer to the container*/
  byte *eutra_container_ptr;

} rrciho_eutra_msg_type;
/* Handover success/failure macro */
#define RRCIHO_EVENT_FAILURE_CAUSE_NOT_VALID 255

#endif

/* Procedure status.
*/
typedef struct
{
  rrc_RRC_TransactionIdentifier transaction_id;

  boolean act_time_present;

  uint32 activation_time;

  uint8                         handover_type;

  union
  {
    rr_interrat_handover_req_type  gsm;

#ifdef FEATURE_WCDMA_TO_LTE
    rrciho_eutra_msg_type    eutra;
#endif

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#endif
  } parms;

  uint32 failure_cause;

  rrc_ProtocolErrorCause protocol_error;

} rrciho_procedure_info_type;

typedef enum
{
  IHO_GTOW_COMPLETE_CONFIG,
  IHO_GTOW_PREDEF_CONFIG,
  IHO_GTOW_DEFAULT_CONFIG
}rrciho_gtow_config_e_type;

/* Procedure status */
typedef struct
{
  uint16  count_c_act_time;                  /*count_c activation time sent
                                               in HO to UTRAN complete msg*/
  boolean gsm_ciph_status;                   /*flag to indicate if ciph is
                                               present in GSM RAT         */

  boolean ciph_algo_present;                 /*flag to indicate if ciph
                                               algo is present            */
  uecomdef_wcdma_cipher_algo_e_type cipheringAlgorithm; /*var to store ciph algo sent
                                               in HO to utran cmd         */

  rrciho_gtow_config_e_type config_type;     /*enum to indicate what config
                                               to use for Handover        */
  uint8               predef_config_id;      /*predefined configuration ID*/
  uint32              default_config_id;     /*default configuration ID   */

  struct rrc_SysInfoType16 * sib16_ptr;      /*ptr to sysInfoType16       */
  boolean             def_config_ptr_present;/*flag to indicate that def
                                               config ptr has been expanded
                                               to SIB16 type.             */

  uint32              maxAllowedUL_TX_Power; /*max allowed UL Tx Power sent
                                               in HOToUtran Command       */

  boolean             act_time_present;      /*flag to indicate if act
                                               time is present in the msg */
  uint32              activation_time;       /*activation time sent in
                                               HOToUtran Command          */

  rrc_HandoverToUTRANCommand *ho_ptr;        /* Pointer to the HO message */
  uint32              ho_msg_len;            /* length of the HO message  */
  boolean             ho_msg_present;        /* boolean to indicate that
                                                we have currently stored
                                                a hoToUtranMsg            */

  rrc_scr_code_type  scr_code;               /*scrambling code info from
                                               HO to UTRAN cmd            */
  rrc_freq_type      freq;                   /*UARFCN info from the HO to
                                               UTRAN cmd                  */

  boolean            rab_info_included;      /*boolean to indicate if rab
                                               info was included in msg   */

  uint8              num_srb;
  uint8              srb_id[UE_MAX_SRB_SETUP];

#ifndef FEATURE_LTE_TO_WCDMA
  rrc_est_rab_info_type  est_rabs;
#else
  uint8  number_of_est_rabs;
  rrc_est_rab_info_type  est_rabs[MAX_RAB_TO_SETUP];
  uint8 cs_rab_index;
  boolean is_cs_rab_present;
  boolean is_ps_rab_present;
#endif /* FEATURE_LTE_TO_WCDMA */

  boolean            class_mark_info_present;
  byte *             class_mark_ptr;

  boolean            pn_offset_present;      /*indicates pn_offset present */
  uint16             pn_offset;              /*actual pn_offset            */

  boolean            diversity_info_present; /*indicates if div present    */
  boolean            diversity_info;         /*indicates if known or not   */

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  rrc_msg_ver_enum_type  msg_rel_version;
  struct rrc_DL_DCCH_Message *rbs_ptr;

#ifdef FEATURE_LTE_TO_WCDMA
  lte_irat_umts_sec_info_s psho_security_keys;
  boolean cs_ho_is_supported;
  boolean ps_ho_is_supported;
#endif /* FEATURE_LTE_TO_WCDMA */
} rrciho_gtow_info_type;

/*declare a local object of type gtow_info*/
rrciho_gtow_info_type gtow_info;

#ifdef FEATURE_LTE_TO_WCDMA
typedef enum
{
  RRCIHO_INTERRAT_GSM,
#ifdef FEATURE_LTE_TO_WCDMA
  RRCIHO_INTERRAT_LTE,
#endif
  RRCIHO_INTERRAT_NONE
}rrciho_interrat_type;

rrciho_interrat_type iho_interrat_type;
#endif /* FEATURE_LTE_TO_WCDMA */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
PLMN_id_T irat_plmn_id;

/* following two variables are defined in rrcsend.c and used for debugging purpose.
   Whenever there is an encoding or decoding failure, these variables will get
   populated with error_status and other useful information. */
extern rrc_encode_info_type rrc_encode_info;

extern boolean rrcrce_send_abort_cnf;

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB 
extern rrcrce_csfb_status_e_type csfb_extended_status;
extern boolean rrc_cs_rab_established;
#endif

extern cell_info_type cell_id_dch_info;
extern uint32  prev_cell_id;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
/* MCM state */
rrciho_state_e_type rrciho_state = RRCIHO_INITIAL;

/* Abort indicator */
boolean rrciho_abort = FALSE;

rrc_abort_req_type *rrc_abort_req_ptr = NULL;

/* Procedure failure statue */
rrciho_procedure_info_type procedure_info;

/* GSM Command list */
rrc_GSM_MessageList gsm_commands;

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA) 
extern boolean rrc_ue_pos_cap_pch;
#endif
#ifdef FEATURE_QSH_EVENT_METRIC
extern boolean rrc_qsh_dl_committed;
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern rrc_qsh_metric_db_type rrc_qsh_metric_db[WRRC_QSH_METRIC_MAX+1];
#endif
boolean lte_rrc_lte_tdd_is_supported 
(
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type lte_band_preference /*!< LTE band preference */
#else
  sys_band_mask_type lte_band_preference /*!< LTE band preference */
#endif
);
boolean lte_rrc_lte_fdd_is_supported
(
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type lte_band_preference /*!< LTE band preference */
#else
  sys_band_mask_type lte_band_preference /*!< LTE band preference */
#endif
);
#ifdef FEATURE_INTER_RAT_HO_OPT

/* Flag to indicate that RRC is in charge of the Vocoder */
boolean rrciho_mvs_control_flag = FALSE;

/* Flag to indicate that mvs_acquire issued by RRC has succeeded */
boolean rrciho_mvs_acquired_flag = FALSE;

extern boolean mn_has_nas_already_acquired_voc(void);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
extern boolean mn_has_nas_already_acquired_voc_subs(sys_modem_as_id_e_type as_id);
#endif

#endif /* FEATURE_INTER_RAT_HO_OPT */

extern cell_info_type cell_id_dch_info;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================

FUNCTION  RRCIHO_INIT_DATA

DESCRIPTION

  This function clears all the module variables and puts the initial
  values wherever needed.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_init_data( void )
{
#ifdef FEATURE_VOC_AMR_WB
#ifdef FEATURE_LTE_TO_WCDMA
  uint8 idx;
#endif
#endif
  /* Reset variables */
  rrciho_state = RRCIHO_INITIAL;
  /* free GSM_commands if not do so */
  if(gsm_commands.n !=0)
  {
    rrc_free(gsm_commands.elem);
  }
  
  gsm_commands.n=0;
  
  /* Set Abort indication to False */
  rrciho_abort = FALSE;
  procedure_info.act_time_present=FALSE;
  procedure_info.handover_type = 0;
#ifdef FEATURE_VOC_AMR_WB
  /* initialise the nas_Synchronisation_Indicator to default value */
#ifdef FEATURE_LTE_TO_WCDMA
  for(idx =0 ; idx < MAX_RAB_TO_SETUP ; idx++)
  {
    gtow_info.est_rabs[idx].nas_Synchronisation_Indicator = MVS_MODE_NONE;
  }
#else
  gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_NONE;
#endif
#endif /*FEATURE_VOC_AMR_WB*/

#ifdef FEATURE_WCDMA_TO_LTE
  if(procedure_info.parms.eutra.eutra_container_size != 0)
  {
    rrc_free(procedure_info.parms.eutra.eutra_container_ptr);
    procedure_info.parms.eutra.eutra_container_ptr = NULL;
    procedure_info.parms.eutra.eutra_container_size = 0;
    WRRC_MSG0_HIGH("WTOL_PSHO: freed the EUTRA container ptr");
  }
#endif

  return;
}




/*===========================================================================

FUNCTION  SEND_NAS_DATA_TRANSFER_IND

DESCRIPTION

  This function sends RRC_NAS_DATA_TRANSFER_IND to request DT to forward all
  bufferred NAS signaling messages to the GSM RR.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void send_nas_data_transfer_ind(void)
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  if((cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    /* Fill in the command id  */
    cmd_ptr->cmd_hdr.cmd_id = RRC_NAS_DATA_TRANSFER_IND;

    /* Put the command on the internal queue */
    rrc_put_int_cmd( cmd_ptr );
    WRRC_MSG0_HIGH("RRC_NAS_DATA_TRANSFER_IND sent to DT");
  }

  return;
} /* end of send_nas_data_transfer_ind */
#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================

FUNCTION   wrrc_qsh_log_iho_call_end_stats

DESCRIPTION

  This function reports IHO failure stats to qsh
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void wrrc_qsh_log_iho_call_end_stats(sys_handover_failure_e_type fail_type)
{
  wrrc_call_end_data_log_type call_end_data;
  call_end_data.type = WRRC_CALL_END_TYPE_HO_FAIL;
  call_end_data.cause.ho_fail_type = fail_type;
  wrrc_qsh_log_conn_end_info(WRRC_METRIC_LOG,&call_end_data);
}
#endif
/*===========================================================================

FUNCTION   SEND_HANDOVER_FROM_UTRAN_FAILURE

DESCRIPTION

  This function sends a Handover From UTRAN Failure to UTRAN to indicate that
  inter-RAT WCDMA to GSM handover is failure.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void send_handover_from_utran_failure( void )
{
  rrc_UL_DCCH_Message *ptr;     /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id;      /* Logical Channel Id for the message  */
  rrcsend_status_e_type status; /* Status from send chain module       */

#ifdef FEATURE_WCDMA_TO_LTE
  rrc_HandoverFromUtranFailure_v860ext_IEs*  eutra_ho_msg_ptr;
#endif

  /* Allocate memory for this message. Note that this buffer is released
  by the SEND chain */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc(
                                          sizeof(rrc_UL_DCCH_Message));

  ptr->message.t = T_rrc_UL_DCCH_MessageType_handoverFromUTRANFailure;

  /* Initialize the bitmask */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.handoverFromUTRANFailure);

  /* Set failure cause present. Note: not include GSM messages for 8/1 */
 RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.handoverFromUTRANFailure,interRAT_HO_FailureCause);

  /* Fill the transaction identifier */
  ptr->message.u.handoverFromUTRANFailure.rrc_TransactionIdentifier =
    procedure_info.transaction_id;

  /* Fill the failure cause */
  ptr->message.u.handoverFromUTRANFailure.interRAT_HO_FailureCause.t =
    procedure_info.failure_cause;

  /* Check the failure cause for the protocol error */
  if (procedure_info.failure_cause == T_rrc_InterRAT_HO_FailureCause_protocolError)
  {

    ptr->message.u.handoverFromUTRANFailure.interRAT_HO_FailureCause.u.protocolError =
        rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);

    /* Set the specific protocol error */
    ptr->message.u.handoverFromUTRANFailure.interRAT_HO_FailureCause.u.protocolError->diagnosticsType.t =
      T_rrc_ProtocolErrorInformation_diagnosticsType_type1;

    ptr->message.u.handoverFromUTRANFailure.interRAT_HO_FailureCause.u.protocolError->
      diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);

    ptr->message.u.handoverFromUTRANFailure.interRAT_HO_FailureCause.u.protocolError->diagnosticsType.u.type1->protocolErrorCause =
      procedure_info.protocol_error;

  }

  if(procedure_info.failure_cause == T_rrc_InterRAT_HO_FailureCause_interRAT_ProtocolError)
  {
    /* Fill in the inter-RAT message for protocol error. */
#ifdef FEATURE_WCDMA_TO_LTE
    if((procedure_info.handover_type == T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA)
         && (procedure_info.parms.eutra.eutra_container_size != 0))
    {
    
       /* Set failure cause present. Note: not include GSM messages for 8/1 */
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.handoverFromUTRANFailure,laterNonCriticalExtensions);
    
      /* Initialize the bitmask */
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.handoverFromUTRANFailure.laterNonCriticalExtensions);
      
      /* Set failure cause present. Note: not include GSM messages for 8/1 */
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.handoverFromUTRANFailure.laterNonCriticalExtensions,v590NonCriticalExtensions);
    
      /* Initialize the bitmask */
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.handoverFromUTRANFailure.laterNonCriticalExtensions.v590NonCriticalExtensions);
      
      /* Set failure cause present. Note: not include GSM messages for 8/1 */
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.handoverFromUTRANFailure.laterNonCriticalExtensions.v590NonCriticalExtensions
           ,v860NonCriticalExtensions);
    
      eutra_ho_msg_ptr = &ptr->message.u.handoverFromUTRANFailure.laterNonCriticalExtensions.v590NonCriticalExtensions.
                           v860NonCriticalExtensions.handoverFromUTRANFailure_v860ext;
    
      eutra_ho_msg_ptr->m.eutra_MessagePresent = 1;
    
      eutra_ho_msg_ptr->eutra_Message.numocts = procedure_info.parms.eutra.eutra_container_size;
      eutra_ho_msg_ptr->eutra_Message.data = procedure_info.parms.eutra.eutra_container_ptr;
    
    }
    else
#endif
    /* Check if GSM message list is present */
    if (gsm_commands.n != 0)
    {
      /* Set inter-RAT message preset flag */
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.handoverFromUTRANFailure,interRATMessage);
      ptr->message.u.handoverFromUTRANFailure.interRATMessage.t = T_rrc_HandoverFromUTRANFailure_interRATMessage_gsm;
      ptr->message.u.handoverFromUTRANFailure.interRATMessage.u.gsm.gsm_MessageList = gsm_commands;
    }

  }

  /*log the error*/
  rrc_log_protocol_error( (uint8) (rrc_get_state()),
                          (uint8) RRC_PROCEDURE_IHO,
                          (uint8) (rrcho_map_inter_rat_cause_to_diag_cause(procedure_info.failure_cause)),                          
                          (uint8) procedure_info.protocol_error                   
                        );

  /* Get the RLC logical channel id for this msg from Logical Channel Manager */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);

  /* If the logical channel id is not invalid, send the message to the
  send chain module */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    WRRC_MSG0_HIGH("Sending HO Failure message");
    status = rrcsend_send_ul_sdu(RRC_PROCEDURE_IHO,  /* Procedure is RBE    */
                                 (void *)ptr,        /* Message pointer     */
                                 ul_lc_id,           /* RLC id              */
                                 RRCSEND_L2ACK_NOT_REQUIRED  /* ACK is not required */
                                );
    if( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Failed to send HO failure: err %d", status);
    }

    //set the gsm_commands to null as we would already have released the
    //asn1 gsm_commands by asn1 call irrespective of whether the send
    //was successful or failure
    gsm_commands.n = 0;

#ifdef FEATURE_WCDMA_TO_LTE
    procedure_info.parms.eutra.eutra_container_size = 0;
#endif
  }
  else
  {
    /* Free memory */
    rrc_free(ptr);

    /* Unexpected failure. ERR for now. May need to ERR_FATAL here */
    WRRC_MSG1_ERROR("Can't send DCCH msg! invalid lc: %d", ul_lc_id );
  }

  /* free memory */
  if(gsm_commands.n != 0)
  {

    rrc_free( gsm_commands.elem);
    gsm_commands.n =0;
  }

#ifdef FEATURE_WCDMA_TO_LTE
  if(procedure_info.parms.eutra.eutra_container_size != 0)
  {
    rrc_free(procedure_info.parms.eutra.eutra_container_ptr);
    procedure_info.parms.eutra.eutra_container_ptr = NULL;
    procedure_info.parms.eutra.eutra_container_size = 0;
    WRRC_MSG0_HIGH("WTOL_PSHO: freed the EUTRA container ptr");
  }
#endif

  /*clear up some static vars. as HO procedure is complete for this attempt*/
  procedure_info.act_time_present=FALSE;

#ifdef FEATURE_QSH_EVENT_METRIC
  if(rrc_qsh_dl_committed == FALSE)
  {
    failure_ota.ul_ota_type = RRC_OTA_IHO;
    failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_HANDOVERFROMUTRANFAILURE;
    failure_ota.ul_fail_ota.ul_ota_parms.failure_cause = procedure_info.failure_cause;
  }
  else
  {
    rrc_qsh_log_ho_from_utran_fail_params(procedure_info.failure_cause);
  }
  if(procedure_info.handover_type == T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA)
  {
    wrrc_qsh_log_iho_call_end_stats(SYS_HANDOVER_FAILURE_W_TO_L_FAILURE);
  }
  else if(procedure_info.handover_type == T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM)
  {
    wrrc_qsh_log_iho_call_end_stats(SYS_HANDOVER_FAILURE_W_TO_G_FAILURE);
  }
#endif

  return;
} /* end of send_handover_from_utran_failure */



/*===========================================================================

FUNCTION   send_rr_interrat_handover_req

DESCRIPTION

  This function sends a RR_INTERRAT_HANDOVER_REQ to GSM RR to initiate the
  WCDMA to GSM handover.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void send_rr_interrat_handover_req( void )
{

  ASSERT( procedure_info.handover_type ==
                               T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM );

  /*if ciphering is turned on then set cipher_enabled flag to true
    else set it to false*/
  if (rrcsmc_is_ciphering_enabled() == RRCSMC_SECURITY_STARTED)
  {
    procedure_info.parms.gsm.cipher_enabled = TRUE;
  }
  else
  {
    procedure_info.parms.gsm.cipher_enabled = FALSE;
  }

#ifdef FEATURE_INTER_RAT_HO_OPT
  /* Reset the mvs control flag because we are doing a W->G handover */
  rrciho_mvs_control_flag = FALSE;
#endif

  rrccsp_fill_info_for_rr(&(procedure_info.parms.gsm.network_select_mode));

#ifdef FEATURE_RAT_PRIORITY_LIST
  rrc_csp_get_rat_pri_list_info(&procedure_info.parms.gsm.rat_pri_list_info);
#endif

#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO
  /*copying the dedicated priority info from global structure(rrc_dedicated_priority_info) to rr_interrat_handover_req_type_req*/
  memset(&procedure_info.parms.gsm.dedicated_priority_info,0,sizeof(lte_irat_dedicated_priority_info_s));
  rrcmeas_populate_dedicated_priority_info_for_irat_req(&procedure_info.parms.gsm.dedicated_priority_info);
#endif

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB 
  if(csfb_extended_status != CSFB_INVALID)
  {
    WRRC_MSG1_HIGH("CSFB HACK: csfb_extended_status=%d",csfb_extended_status);
    procedure_info.parms.gsm.csfb_call_status=TRUE;
  }
  else
  {
    procedure_info.parms.gsm.csfb_call_status=FALSE;
  }
#endif
  /* Send the RR_INTERRAT_HANDOVER_REQ message to RR */
  rr_send_rrc_message_to_rr_internal((void *)(&(procedure_info.parms.gsm)), RR_INTERRAT_HANDOVER_REQ,
                sizeof(rr_interrat_handover_req_type));

  WRRC_MSG0_HIGH("RR_INTERRAT_HANDOVER_REQ sent to GSM RR");

  return;

} /* end of send_rr_interrat_handover_req */

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION   send_rr_interrat_handover_req

DESCRIPTION

  This function sends a RR_INTERRAT_HANDOVER_REQ to GSM RR to initiate the
  WCDMA to GSM handover.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_lte_ps_ho_req( void )
{
  msgr_attach_struct_type    *msgr_attach_ptr = NULL;

  ASSERT( procedure_info.handover_type ==
        T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA );

  // Build and send out the message request, including the HO Container as first attachment
  msgr_init_hdr_attach(&procedure_info.parms.eutra.msg.msg_hdr, MSGR_WCDMA_RRC, LTE_RRC_WCDMA_PSHO_REQ, 0, 1);

#ifdef FEATURE_DUAL_SIM
  msgr_set_hdr_inst(&procedure_info.parms.eutra.msg.msg_hdr,SYS_AS_ID_TO_INST_ID(rrc_get_as_id()));
#endif


  // Set the HandoverToUTRACommand as a DSM attachment
  msgr_attach_ptr = msgr_get_attach(&procedure_info.parms.eutra.msg.msg_hdr, 0);
  ASSERT(msgr_attach_ptr != NULL);
  msgr_set_dsm_attach(msgr_attach_ptr, procedure_info.parms.eutra.dsm_ptr);

  ASSERT(procedure_info.parms.eutra.dsm_ptr != NULL);

  rrccsp_fill_info_for_rr(&(procedure_info.parms.eutra.msg.irat_nas_info.network_select_mode));

  procedure_info.parms.eutra.msg.irat_nas_info.req_plmn = 
      mmumts_convert_rrc_plmn_id_to_nas_plmn_id(rrccsp_get_curr_nas_requested_plmn());

#ifdef FEATURE_RAT_PRIORITY_LIST
  rrc_csp_get_rat_pri_list_info(&procedure_info.parms.eutra.msg.irat_nas_info.rat_pri_list);
#endif

  /* Get EUTRA timing tx info from L1 */
  l1_get_eutra_measurement_data(&procedure_info.parms.eutra.msg.timing_tx_info);
  if(procedure_info.parms.eutra.msg.timing_tx_info.number_of_cells > 0)
  {
    procedure_info.parms.eutra.msg.timing_tx_info_present = TRUE;
  }
  else
  {
    procedure_info.parms.eutra.msg.timing_tx_info_present = FALSE;
  }

  procedure_info.parms.eutra.msg.wtol_fdd_supported = FALSE;
  procedure_info.parms.eutra.msg.wtol_tdd_supported = FALSE;
  if((lte_rrc_lte_fdd_is_supported(rrccsp_get_lte_band_pref()))&&(wcdma_rrc_wtol_ps_ho_support_nv))
  {
    procedure_info.parms.eutra.msg.wtol_fdd_supported = TRUE;
  }
  if((lte_rrc_lte_tdd_is_supported(rrccsp_get_lte_band_pref()))&&(wcdma_rrc_wtol_tdd_ps_ho_support_nv))
  {
    procedure_info.parms.eutra.msg.wtol_tdd_supported = TRUE;
  }

  procedure_info.parms.eutra.msg.utra_cell_info.rat = SYS_RAT_UMTS_RADIO_ACCESS;
  procedure_info.parms.eutra.msg.utra_cell_info.frequency = cell_id_dch_info.uarfcn_dl;
  procedure_info.parms.eutra.msg.utra_cell_info.phy_cell_id = cell_id_dch_info.psc;
  procedure_info.parms.eutra.msg.utra_cell_info.is_rat_active = TRUE;

  if(cell_id_dch_info.cell_id != WCDMA_INVALID_CELL_ID)
  {
    procedure_info.parms.eutra.msg.utra_cell_info.is_cell_global_id_present = TRUE;
    procedure_info.parms.eutra.msg.utra_cell_info.serving_cell_global_id.plmn_id = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(cell_id_dch_info.plmn_id);
    procedure_info.parms.eutra.msg.utra_cell_info.serving_cell_global_id.cell_id = cell_id_dch_info.cell_id;
    WRRC_MSG2_HIGH("Camped on PLMN: %d-%d ",RRC_CSP_GET_INT_MCC(cell_id_dch_info.plmn_id),RRC_CSP_GET_INT_MNC(cell_id_dch_info.plmn_id));
    WRRC_MSG1_HIGH("Current Camped CELL I.D# %d ",cell_id_dch_info.cell_id);
  }
  else
  {
    procedure_info.parms.eutra.msg.utra_cell_info.is_cell_global_id_present = FALSE;
  }

  /* Save the umid for debugging purposes */
  rrc_crash_debug_save_rrc_to_lte_cmds(LTE_RRC_WCDMA_PSHO_REQ);
  msgr_send(&procedure_info.parms.eutra.msg.msg_hdr, sizeof(lte_rrc_wcdma_psho_req_s));
  WRRC_MSG0_HIGH("WTOL: LTE_RRC_WCDMA_PSHO_REQ sent to LTE RRC");

  return;
}

/*===========================================================================

FUNCTION   rrciho_send_abort_handover_to_lrrc

DESCRIPTION

  This function sends a LTE_RRC_WCDMA_ABORT_PSHO_REQ to LTE-RRC.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_abort_handover_to_lte( void )
{

  lte_rrc_wcdma_abort_psho_req_s *abort_psho_req_ptr = (lte_rrc_wcdma_abort_psho_req_s *)rrc_malloc(sizeof(lte_rrc_wcdma_abort_psho_req_s));
  rrc_send_lte_cmd(&(abort_psho_req_ptr->msg_hdr),LTE_RRC_WCDMA_ABORT_PSHO_REQ, sizeof(lte_rrc_wcdma_abort_psho_req_s));
  WRRC_MSG0_HIGH("WTOL: LTE_RRC_WCDMA_ABORT_PSHO_REQ sent to LTE RRC");
  rrc_free(abort_psho_req_ptr);

}
#endif

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_UMTSMSM */




/*===========================================================================

FUNCTION   send_interrat_handover_req

DESCRIPTION

  This function sends Interrat handover request to either RR for WCDMA -> GSM
  or CM for WCDMA -> CDMA2000 handover.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void send_interrat_handover_req( void )
{

  switch ( procedure_info.handover_type )
  {
    case T_rrc_UL_DCCH_MessageType_handoverFromUTRANFailure:
      send_rr_interrat_handover_req();
      break;

#ifdef FEATURE_WCDMA_TO_LTE
    case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA:
      rrciho_send_lte_ps_ho_req();
      break;
#endif

    #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#endif

    default:
      ASSERT( 0 );
      break;

  } /* switch ( procedure_info.handover_type ) */

} /* end of send_interrat_handover_req */



/*===========================================================================

FUNCTION   SEND_ABORT_HANDOVER_REQ

DESCRIPTION

  This function sends a RR_INTERRAT_HANDOVER_ABORT_REQ to GSM RR to abort the
  WCDMA to GSM handover.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void send_abort_handover_req( void )
{
  rr_interrat_handover_abort_req_type abort_req;

  /* Send the RR_INTERRAT_HANDOVER_REQ message to RR */
  rr_send_rrc_message_to_rr_internal((void *)(&(abort_req)), RR_INTERRAT_HANDOVER_ABORT_REQ,
                sizeof(rr_interrat_handover_abort_req_type));

  WRRC_MSG0_HIGH("RR_INTERRAT_HANDOVER_ABORT_REQ sent to GSM RR");

  return;

} /* end of send_abort_handover_req */


/*===========================================================================

FUNCTION   SEND_CHANGE_MODE_REQ

DESCRIPTION

  This function sends a RRC_CHANGE_MODE_REQ to MCM to request to change the
  RRC mode to Standby.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void send_change_mode_req( rrc_mode_e_type mode )
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CHANGE_MODE_REQ;

  /* Fill in procedure id    */
  cmd_ptr->cmd.change_mode_req.procedure = RRC_PROCEDURE_IHO;

  /* Fill in the requested mode */
  cmd_ptr->cmd.change_mode_req.mode = mode;

  /* Fill the reason for the change mode request */
  cmd_ptr->cmd.change_mode_req.reason = INTERRAT_IHO;

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG0_HIGH("RRC_CHANGE_MODE_REQ sent to MCM");

  return;
} /* end of send_change_mode_req */


/*===========================================================================

FUNCTION   SEND_CHANNEL_CONFIG_REQ_TO_LLC

DESCRIPTION

  This function sends a RRC_CHANNEL_CONFIG_REQ to the other RRC LLC to revert
  physical channels to previous UTRAN configuration in WCDMA L1.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void send_channel_config_req_to_llc(void)
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* Fill in procedure id */
  cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_IHO;

  /* Indicate that state change is not required */
  cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;

  /* Indicate that no confirmation is required from LLC */
  cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG0_HIGH("RRC_CHANNEL_CONFIG_REQ sent to LLC");

  return;
} /* end of send_channel_config_req_to_llc */

/*===========================================================================

FUNCTION   rrciho_delete_stored_gtow_info

DESCRIPTION

  Clean up all allocated pointers, clear all variables and delete all the
  stuff stored in the static variable gtow_info.

  This may be called due to a successful HO or an unsuccessful HO or due
  to receiving a HandoverAbortReq


DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_delete_stored_gtow_info(void)
{
  /*delete the HO to Utran msg by calling rrcasn1_free_pdu*/
  if(gtow_info.ho_msg_present == TRUE)
  {

    /*free the msg_ptr previously allocated.. if failure, flag an error*/
    rrcasn1_free_pdu((int)rrc_HandoverToUTRANCommand_PDU, gtow_info.ho_ptr);
      /*clean the msg_ptr data structure*/
      gtow_info.ho_msg_len = 0;
      gtow_info.ho_msg_present = FALSE;
    gtow_info.ho_ptr = NULL;
  }

  /*delete default config info if it was expanded*/
  if(gtow_info.def_config_ptr_present == TRUE)
  {
    if(gtow_info.rbs_ptr != NULL)
    {
      rrcasn1_free_pdu((int)rrc_DL_DCCH_Message_PDU, gtow_info.rbs_ptr);
      /*clean the msg_ptr data structure*/
      gtow_info.def_config_ptr_present = FALSE;
      gtow_info.rbs_ptr =NULL;
    }
    else
    {
      /*free the msg_ptr previously allocated.. if failure, flag an error*/
      rrciho_free_sib16();
        /*clean the msg_ptr data structure*/
        gtow_info.def_config_ptr_present = FALSE;
      }
    }
  /*delete class mark information*/
  if(gtow_info.class_mark_info_present == TRUE)
  {
    WRRC_MSG0_ERROR("Memory leak.. Classmark info undeleted");

    // memory freeing for encoded_sdu should be done here using OSS compiler
    // functions.
    rrc_free(gtow_info.class_mark_ptr);

    /*reset the static variables*/
    gtow_info.class_mark_info_present = FALSE;
    gtow_info.class_mark_ptr = NULL;
  }

  /*set the other stored vars to false or default.  do not delete rab_info_included
    as it is needed to send SYNC indication later.  It will be deleted after sending
    SYNC_IND to MM*/
  gtow_info.act_time_present = FALSE;
  gtow_info.ciph_algo_present = FALSE;
  gtow_info.num_srb = 0;
  gtow_info.maxAllowedUL_TX_Power = 0;
  gtow_info.diversity_info_present = FALSE;
  gtow_info.diversity_info = FALSE;
  gtow_info.pn_offset_present = FALSE;
  gtow_info.pn_offset = 0;


}/*rrciho_delete_stored_gtow_info*/
/*===========================================================================

FUNCTION   rrciho_initialize_srb_list

DESCRIPTION

  Initializes GTOW information for Signalling Radio Bearer list


DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_initialize_srb_list(rrc_SRB_InformationSetupList *srb_ptr)
{
  uint8 srb_count=0;
  rrc_RB_Identity rb_id = CCCH_RADIO_BEARER_ID;

  while ((srb_ptr!=NULL )&& (srb_ptr->n > srb_count ))
  {
    /* Get the SRB ID from the message if present */
    if(srb_ptr->elem[srb_count].m.rb_IdentityPresent)
    {
      rb_id = srb_ptr->elem[srb_count].rb_Identity;
    }
    else
    {
      /* Else, increment from the previous SRB Id */
      rb_id++;
    }
    

    /* Do a safety check on the count. We can't do more than 32
    configs in a command */
    if ( srb_count >= UE_MAX_SRB_SETUP )
    {
      WRRC_MSG0_HIGH("MAX SRB count exceeded!");
      break;
    }

    gtow_info.srb_id[srb_count] = (uint8)rb_id;

    /* Go to the next RB */

    /* Increment count of RBs */
    srb_count++;

  }/* end while */

  /* Save the number of SRBs */
  gtow_info.num_srb = srb_count;
}/*rrciho_initialize_srb_list*/
/*===========================================================================

FUNCTION   rrciho_initialize_srb_list_r5

DESCRIPTION

  Initializes GTOW information for Signalling Radio Bearer list


DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_initialize_srb_list_r5(rrc_SRB_InformationSetupList_r5 *srb_ptr)
{
  uint8 srb_count=0;
  rrc_RB_Identity rb_id = CCCH_RADIO_BEARER_ID;

  while ((srb_ptr!=NULL )&& (srb_ptr->n > srb_count ))
  {
    /* Get the SRB ID from the message if present */
    if(srb_ptr->elem[srb_count].m.rb_IdentityPresent)
    {
      rb_id = srb_ptr->elem[srb_count].rb_Identity;
    }
    else
    {
      /* Else, increment from the previous SRB Id */
      rb_id++;
    }
    
    if ( srb_count >= UE_MAX_SRB_SETUP )
    {
      WRRC_MSG0_HIGH("MAX SRB count exceeded!");
      break;
    }
    gtow_info.srb_id[srb_count] = (uint8)rb_id;

    /* Go to the next RB */

    /* Do a safety check on the count. We can't do more than 32
    configs in a command */


    /* Increment count of RBs */
    srb_count++;

  }/* end while */

  /* Save the number of SRBs */
  gtow_info.num_srb = srb_count;
}/*rrciho_initialize_srb_list_r5*/
/*===========================================================================

FUNCTION   rrciho_initialize_srb_list_r6

DESCRIPTION

  Initializes GTOW information for Signalling Radio Bearer list


DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_initialize_srb_list_r6(rrc_SRB_InformationSetupList_r6 *srb_ptr)
{
  uint8 srb_count=0;
  rrc_RB_Identity rb_id = CCCH_RADIO_BEARER_ID;

  while ( (srb_ptr!=NULL )&& (srb_ptr->n > srb_count))
  {
    /* Get the SRB ID from the message if present */
    if(srb_ptr->elem[srb_count].m.rb_IdentityPresent)
    {
      rb_id = srb_ptr->elem[srb_count].rb_Identity;
    }
    else
    {
      /* Else, increment from the previous SRB Id */
      rb_id++;
    }
    
    if ( srb_count >= UE_MAX_SRB_SETUP )
    {
      WRRC_MSG0_HIGH("MAX SRB count exceeded!");
      break;
    }

    gtow_info.srb_id[srb_count] = (uint8)rb_id;

    /* Go to the next RB */

    /* Do a safety check on the count. We can't do more than 32
    configs in a command */


    /* Increment count of RBs */
    srb_count++;

  }/* end while */

  /* Save the number of SRBs */
  gtow_info.num_srb = srb_count;
}/*rrciho_initialize_srb_list_r6*/
/*===========================================================================

FUNCTION   rrciho_initialize_rb_list

DESCRIPTION

  Initializes GTOW information for Radio Bearer list

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_initialize_rb_list(rrc_RB_InformationSetupList *rb_ptr
                               #ifdef FEATURE_LTE_TO_WCDMA
                                    , uint8 index
                               #endif
                               )
{

  uint8 rb_count=0;

  while ((rb_ptr!=NULL)&& (rb_ptr->n > rb_count ))
  {
    if ( rb_count >= MAX_RB_PER_RAB )
    {
      WRRC_MSG0_HIGH("MAX RB count exceeded!");
      break;
    }

#ifndef FEATURE_LTE_TO_WCDMA
    gtow_info.est_rabs.rb_for_rab[rb_count].rb_id = rb_ptr->elem[rb_count].rb_Identity;
#else
    gtow_info.est_rabs[index].rb_for_rab[rb_count].rb_id = rb_ptr->elem[rb_count].rb_Identity;
#endif /* FEATURE_LTE_TO_WCDMA */    
    /* Increment count of RBs */
    rb_count++;
  }/* end while */

  /* Save the number of RBs for this RAB */
#ifndef FEATURE_LTE_TO_WCDMA
  gtow_info.est_rabs.num_rbs_for_rab = rb_count;
#else
  gtow_info.est_rabs[index].num_rbs_for_rab = rb_count;
#endif /* FEATURE_LTE_TO_WCDMA */
}/*rrciho_initialize_rb_list*/
/*===========================================================================

FUNCTION   rrciho_initialize_rb_list_r5

DESCRIPTION

  Initializes GTOW information for Radio Bearer list

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_initialize_rb_list_r5(rrc_RB_InformationSetupList_r5 *rb_ptr
                                  #ifdef FEATURE_LTE_TO_WCDMA
                                  , uint8 index
                                  #endif
                                  )
{

  uint8 rb_count=0;

  while ((rb_ptr!=NULL )&&( rb_ptr->n > rb_count ))
  {
    if ( rb_count >= MAX_RB_PER_RAB )
    {
      WRRC_MSG0_HIGH("MAX RB count exceeded!");
      break;
    }

#ifndef FEATURE_LTE_TO_WCDMA
    gtow_info.est_rabs.rb_for_rab[rb_count].rb_id = rb_ptr->elem[rb_count].rb_Identity;
#else
    gtow_info.est_rabs[index].rb_for_rab[rb_count].rb_id = rb_ptr->elem[rb_count].rb_Identity;
#endif /* FEATURE_LTE_TO_WCDMA */
    /* Increment count of RBs */
    rb_count++;

  }/* end while */

  /* Save the number of RBs for this RAB */
#ifndef FEATURE_LTE_TO_WCDMA
  gtow_info.est_rabs.num_rbs_for_rab = rb_count;
#else
  gtow_info.est_rabs[index].num_rbs_for_rab = rb_count;
#endif /* FEATURE_LTE_TO_WCDMA */

}/*rrciho_initialize_rb_list_r5*/

/*===========================================================================

FUNCTION   rrciho_initialize_rb_list

DESCRIPTION

  Initializes GTOW information for Radio Bearer list

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_initialize_rb_list_r6(rrc_RB_InformationSetupList_r6 *rb_ptr
                                  #ifdef FEATURE_LTE_TO_WCDMA
                                  , uint8 index
                                  #endif
                                  )
{

  uint8 rb_count=0;

  while ((rb_ptr!=NULL )&& (rb_ptr->n > rb_count ))
  {

    if ( rb_count >= MAX_RB_PER_RAB )
    {
      WRRC_MSG0_HIGH("MAX RB count exceeded!");
      break;
    }

#ifndef FEATURE_LTE_TO_WCDMA
    gtow_info.est_rabs.rb_for_rab[rb_count].rb_id = rb_ptr->elem[rb_count].rb_Identity;
#else
    gtow_info.est_rabs[index].rb_for_rab[rb_count].rb_id = rb_ptr->elem[rb_count].rb_Identity;
#endif /* FEATURE_LTE_TO_WCDMA */

    /* Increment count of RBs */
    rb_count++;

  }/* end while */

  /* Save the number of RBs for this RAB */
#ifndef FEATURE_LTE_TO_WCDMA
  gtow_info.est_rabs.num_rbs_for_rab = rb_count;
#else
  gtow_info.est_rabs[index].num_rbs_for_rab = rb_count;
#endif /* FEATURE_LTE_TO_WCDMA */
}/*rrciho_initialize_rb_list_r6*/

/*===========================================================================

FUNCTION   rrciho_initialize_srb_list_r7

DESCRIPTION

  Initializes GTOW information for Signalling Radio Bearer list


DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_initialize_srb_list_r7
(
   rrc_SRB_InformationSetupList_r7 *srb_ptr
)
{
  uint8 srb_count=0;
  rrc_RB_Identity rb_id = CCCH_RADIO_BEARER_ID;

  while ((srb_ptr!=NULL )&& (srb_ptr->n > srb_count))
  {
    /* Get the SRB ID from the message if present */
    if(srb_ptr->elem[srb_count].m.rb_IdentityPresent)
    {
      rb_id = srb_ptr->elem[srb_count].rb_Identity;
    }
    else
    {
      /* Else, increment from the previous SRB Id */
      rb_id++;
    }
    
    if ( srb_count >= UE_MAX_SRB_SETUP )
    {
      WRRC_MSG0_HIGH("MAX SRB count exceeded!");
      break;
    }

    gtow_info.srb_id[srb_count] = (uint8)rb_id;

    /* Increment count of RBs */
    srb_count++;

  }/* end while */

  /* Save the number of SRBs */
  gtow_info.num_srb = srb_count;
}
/*===========================================================================

FUNCTION   rrciho_initialize_rb_list

DESCRIPTION

  Initializes GTOW information for Radio Bearer list

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_initialize_rb_list_r7
(
  rrc_RB_InformationSetupList_r7 *rb_ptr
#ifdef FEATURE_LTE_TO_WCDMA
, uint8 index
#endif
)
{
  uint8 rb_count=0;

  while ((rb_ptr!=NULL )&&( rb_ptr->n > rb_count ) && (rb_count < MAX_RB_PER_RAB))
  {

#ifndef FEATURE_LTE_TO_WCDMA
    gtow_info.est_rabs.rb_for_rab[rb_count].rb_id = rb_ptr->elem[rb_count].rb_Identity;
#else
    gtow_info.est_rabs[index].rb_for_rab[rb_count].rb_id = (uint8)(rb_ptr->elem[rb_count].rb_Identity);
#endif
    /* Increment count of RBs */
    rb_count++;

  }/* end while */

  /* Save the number of RBs for this RAB */
#ifndef FEATURE_LTE_TO_WCDMA
  gtow_info.est_rabs.num_rbs_for_rab = rb_count;
#else
  gtow_info.est_rabs[index].num_rbs_for_rab = rb_count;
#endif
}/*rrciho_initialize_rb_list_r7*/

#ifndef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION   rrciho_update_rab_info_r7

DESCRIPTION

  updates GTOW information for Radio Bearer list

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static rrc_RAB_InformationSetupList_r7 *  rrciho_update_rab_info_r7
(
    uint32 * rab_identity_choice
)
{
  rrc_RB_InformationSetupList_r7  *rb_list_r7 = NULL;
  rrc_HandoverToUTRANCommand_r7_IEs * ho_utran_r7;
  rrc_RAB_InformationSetupList_r7 *rab_list_r7 = NULL;

      ho_utran_r7 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7
             .handoverToUTRANCommand_r7;
   if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == ho_utran_r7->specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.rab_Identity.t)
   {
     *rab_identity_choice = T_rrc_RAB_Identity_gsm_MAP_RAB_Identity;
   
     /* GSM_MAP RAB Id. Check if it's valid by calling the
        appropriate NAS entity. First convert it to internal
        format. */
   
     gtow_info.est_rabs.rab_id =
        rrcrb_translate_gsm_map_rab_id(ho_utran_r7->specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.
                                       rab_Identity.u.gsm_MAP_RAB_Identity);
   
     gtow_info.est_rabs.re_est_timer = ho_utran_r7->specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.re_EstablishmentTimer;
   
     rb_list_r7 =&ho_utran_r7->specificationMode.u.complete.rab_InformationSetupList.elem[0].rb_InformationSetupList;
   
     rrciho_initialize_rb_list_r7(rb_list_r7);
   
     rab_list_r7 = &ho_utran_r7->specificationMode.u.complete.rab_InformationSetupList;
   
#ifdef FEATURE_VOC_AMR_WB
     if(rab_list_r7->elem[0].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
     {
       gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_AMR;
        /* in the G-> W handover check the Nas-Sync-indicator for the codec type */        
       if(rab_list_r7->elem[0].rab_Info.m.nas_Synchronisation_IndicatorPresent)
       {
         gtow_info.est_rabs.nas_Synchronisation_Indicator = 
             rrc_translate_nas_sync_indicator(&(rab_list_r7->elem[0].rab_Info.nas_Synchronisation_Indicator));
         
         if( gtow_info.est_rabs.nas_Synchronisation_Indicator == MVS_MODE_NONE )
         {
           procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
           WRRC_MSG1_HIGH("Invalid configuration .. codec not supported %d",gtow_info.est_rabs.nas_Synchronisation_Indicator );
           return NULL;
         }
       }
       rrc_set_ordered_codec_mode(gtow_info.est_rabs.nas_Synchronisation_Indicator);
       WRRC_MSG1_HIGH("CSoHS:Codec in rrc_ota_codec_mode_status %d",rrc_ota_codec_mode_status);   
     }
   
#endif /*FEATURE_VOC_AMR_WB*/

   return rab_list_r7;
     /*lint +e740 */
   }
   return NULL;
}
#endif /* FEATURE_LTE_TO_WCDMA */

#ifdef FEATURE_VOC_AMR_WB
/*===========================================================================

FUNCTION   rrciho_update_nas_sync_ind

DESCRIPTION Updates the NAS Syncronisation Indicator field received in the handover to utran message.
            In case the NAS Syncronisation Indicator IE is not present , then a default value of 
            NB-AMR codec is used.

DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uecomdef_status_e_type rrciho_update_nas_sync_ind
(
  rrc_RAB_Info rab_info
#ifdef FEATURE_LTE_TO_WCDMA
  , uint8 index
#endif
)
{
  rrc_est_rab_info_type  *est_rabs;

#ifdef FEATURE_LTE_TO_WCDMA
  est_rabs = &(gtow_info.est_rabs[index]);
#else
  est_rabs = &(gtow_info.est_rabs);
#endif /* FEATURE_LTE_TO_WCDMA */

  if(rab_info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
  {
    est_rabs->nas_Synchronisation_Indicator = MVS_MODE_AMR;
     /* in the G-> W handover check the Nas-Sync-indicator for the codec type */        
    if(RRC_MSG_COMMON_BITMASK_IE(rab_info,rrc_RAB_Info,nas_Synchronisation_Indicator))
    {
      est_rabs->nas_Synchronisation_Indicator = 
          rrc_translate_nas_sync_indicator(&(rab_info.nas_Synchronisation_Indicator));
      
      if(est_rabs->nas_Synchronisation_Indicator == MVS_MODE_NONE )
      {
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
        WRRC_MSG1_HIGH("Invalid configuration .. codec not supported %d",est_rabs->nas_Synchronisation_Indicator );
        return FAILURE;
      }
    }
    rrc_set_ordered_codec_mode(est_rabs->nas_Synchronisation_Indicator);
  }

  return SUCCESS;
}

/*===========================================================================

FUNCTION   rrciho_update_def_config_nas_sync_ind

DESCRIPTION Updates the NAS Syncronisation Indicator field received in the handover to utran message for default
            configurations .
            In case the NAS Syncronisation Indicator IE is not present , then a default value of 
            NB-AMR codec is used.

DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uecomdef_status_e_type rrciho_update_def_config_nas_sync_ind
(
  rrc_RAB_Info_Post * rab_info
#ifdef FEATURE_LTE_TO_WCDMA
  , uint8 index
#endif /* FEATURE_LTE_TO_WCDMA */
)
{
  rrc_est_rab_info_type  *est_rabs;

#ifdef FEATURE_LTE_TO_WCDMA
  est_rabs = &(gtow_info.est_rabs[index]);
#else
  est_rabs = &(gtow_info.est_rabs);
#endif /* FEATURE_LTE_TO_WCDMA */

  if(rab_info->cn_DomainIdentity  == rrc_CN_DomainIdentity_cs_domain )
  {
    est_rabs->nas_Synchronisation_Indicator = MVS_MODE_AMR;
     /* in the G-> W handover check the Nas-Sync-indicator for the codec type */        
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rab_info,rrc_RAB_Info_Post,
		nas_Synchronisation_Indicator))
    {
      est_rabs->nas_Synchronisation_Indicator =  rrc_translate_nas_sync_indicator(
          &(rab_info->nas_Synchronisation_Indicator));
      
      if( est_rabs->nas_Synchronisation_Indicator == MVS_MODE_NONE )
      {
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
        WRRC_MSG1_HIGH("Invalid configuration .. codec not supported %d",est_rabs->nas_Synchronisation_Indicator );
        return FAILURE;
      }
    }
    rrc_set_ordered_codec_mode(est_rabs->nas_Synchronisation_Indicator);
  }

  return SUCCESS;
}

#endif /*FEATURE_VOC_AMR_WB*/

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION   rrciho_extract_and_fill_in_rab_rb_info

DESCRIPTION

  Extracts rab, rb info and updates est_rabs

DEPENDENCIES

  None.

RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type rrciho_extract_and_fill_in_rab_rb_info(
                                                              void *rab_info_list,
                                                              rrc_msg_ver_enum_type release
                                                              )
{
  rrc_RB_InformationSetupList  *rb_list = NULL;
  rrc_RAB_InformationSetupList *rab_list = NULL;
  rrc_RB_InformationSetupList_r5  *rb_list_r5 = NULL;
  rrc_RAB_InformationSetupList_r5 *rab_list_r5 = NULL;
  rrc_RB_InformationSetupList_r6  *rb_list_r6 = NULL;
  rrc_RAB_InformationSetupList_r6 *rab_list_r6 = NULL;
  rrc_RB_InformationSetupList_r7  *rb_list_r7 = NULL;
  rrc_RAB_InformationSetupList_r7 *rab_list_r7 = NULL;
  rrc_RB_InformationSetupList_r8  *rb_list_r8 = NULL;
  rrc_RAB_InformationSetupList_r8  *rab_list_r8 = NULL;

  uint8 number_of_rabs = 0;
  uint8 idx = 0;

#ifdef FEATURE_VOC_AMR_WB
  /* codec mode is initialized to an invalid value. 
  Only for the case of CS RAB in HO message, it will be determined and indicated to CM. 
  For PS only HO, initialized values remain unchanged. */
  rrc_set_ordered_codec_mode(MVS_MODE_NONE);
#endif /*FEATURE_VOC_AMR_WB*/

  if(release == MSG_REL99)
  {
    rab_list = (rrc_RAB_InformationSetupList *) rab_info_list;

    do 
    {
      /* Check if the RAB id is GSM_MAP type. We don't support any
         other type */
      if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == rab_list->elem[idx].rab_Info.rab_Identity.t)
      {

        gtow_info.est_rabs[number_of_rabs].rab_id =
           rrcrb_translate_gsm_map_rab_id(rab_list->elem[idx].rab_Info.
                                       rab_Identity.u.gsm_MAP_RAB_Identity);

        gtow_info.est_rabs[number_of_rabs].re_est_timer = rab_list->elem[idx].rab_Info.re_EstablishmentTimer;

        gtow_info.est_rabs[number_of_rabs].cn_domain = rab_list->elem[idx].rab_Info.cn_DomainIdentity;

        gtow_info.est_rabs[number_of_rabs].rab_id_is_gsm_map = TRUE;

        rb_list = &rab_list->elem[idx].rb_InformationSetupList;

        rrciho_initialize_rb_list(rb_list,number_of_rabs);

        (void)rrc_update_rab_type_in_rab_info(
                                                 &(rab_list->elem[idx]),
                                                 &(gtow_info.est_rabs[number_of_rabs]));

        MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
                                                     gtow_info.est_rabs[number_of_rabs].rab_id,
                                                     gtow_info.est_rabs[number_of_rabs].cn_domain,
                                                     gtow_info.est_rabs[number_of_rabs].re_est_timer);

        if(rab_list->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
        {
          gtow_info.cs_rab_index = number_of_rabs;
          gtow_info.is_cs_rab_present = TRUE;
          if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
          {
            gtow_info.est_rabs[number_of_rabs].rab_type = RRCRB_CS_VOICE_RAB;
          }
        }
        else if(rab_list->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
        {
          gtow_info.is_ps_rab_present = TRUE;
        }

#ifdef FEATURE_VOC_AMR_WB
         /* in the G-> W handover check the Nas-Sync-indicator for the codec type */
        if((rab_list->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain) && 
           (rrciho_update_nas_sync_ind(rab_list->elem[idx].rab_Info, number_of_rabs) == FAILURE))
        {
          return FAILURE;
        }
#endif /*FEATURE_VOC_AMR_WB*/
        number_of_rabs++;
      }
      else
      {
        return FAILURE;
      }
      idx++;
    }while((idx < rab_list->n) && (number_of_rabs < MAX_RAB_TO_SETUP));

  }
  else if(release == MSG_REL5)
  {
    rab_list_r5 = (rrc_RAB_InformationSetupList_r5 *) rab_info_list;

    do 
    {

      /* Check if the RAB id is GSM_MAP type. We don't support any
         other type */
      if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == rab_list_r5->elem[idx].rab_Info.rab_Identity.t)
      {

        gtow_info.est_rabs[number_of_rabs].rab_id =
           rrcrb_translate_gsm_map_rab_id(rab_list_r5->elem[idx].rab_Info.
                                       rab_Identity.u.gsm_MAP_RAB_Identity);

        gtow_info.est_rabs[number_of_rabs].re_est_timer = rab_list_r5->elem[idx].rab_Info.re_EstablishmentTimer;

        gtow_info.est_rabs[number_of_rabs].cn_domain = rab_list_r5->elem[idx].rab_Info.cn_DomainIdentity;

        gtow_info.est_rabs[number_of_rabs].rab_id_is_gsm_map = TRUE;

        rb_list_r5 = &rab_list_r5->elem[idx].rb_InformationSetupList;

        rrciho_initialize_rb_list_r5(rb_list_r5,number_of_rabs);

        (void)rrc_update_rab_type_in_rab_info_r5(
                                                 &(rab_list_r5->elem[idx]),
                                                 &(gtow_info.est_rabs[number_of_rabs]));

        MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
                                                     gtow_info.est_rabs[number_of_rabs].rab_id,
                                                     gtow_info.est_rabs[number_of_rabs].cn_domain,
                                                     gtow_info.est_rabs[number_of_rabs].re_est_timer);

        if(rab_list_r5->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
        {
          gtow_info.cs_rab_index = number_of_rabs;
          gtow_info.is_cs_rab_present = TRUE;
          if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
          {
            gtow_info.est_rabs[number_of_rabs].rab_type = RRCRB_CS_VOICE_RAB;
          }
        }
        else if(rab_list_r5->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
        {
          gtow_info.is_ps_rab_present = TRUE;
        }


#ifdef FEATURE_VOC_AMR_WB
         /* in the G-> W handover check the Nas-Sync-indicator for the codec type */
        if((rab_list_r5->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain) && 
           (rrciho_update_nas_sync_ind(rab_list_r5->elem[idx].rab_Info, number_of_rabs) == FAILURE))
        {
          return FAILURE;
        }
#endif /*FEATURE_VOC_AMR_WB*/
        number_of_rabs++;
      }
      else
      {
        return FAILURE;
      }
      idx++;
    }while((idx < rab_list_r5->n) && (number_of_rabs < MAX_RAB_TO_SETUP));

  }
  else if(release == MSG_REL6)
  {
    rab_list_r6 = (rrc_RAB_InformationSetupList_r6*) rab_info_list;

    do 
    {

      /* Check if the RAB id is GSM_MAP type. We don't support any
         other type */
      if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == rab_list_r6->elem[idx].rab_Info.rab_Identity.t)
      {

        gtow_info.est_rabs[number_of_rabs].rab_id =
           rrcrb_translate_gsm_map_rab_id(rab_list_r6->elem[idx].rab_Info.
                                       rab_Identity.u.gsm_MAP_RAB_Identity);

        gtow_info.est_rabs[number_of_rabs].re_est_timer = rab_list_r6->elem[idx].rab_Info.re_EstablishmentTimer;

        gtow_info.est_rabs[number_of_rabs].cn_domain = rab_list_r6->elem[idx].rab_Info.cn_DomainIdentity;

        gtow_info.est_rabs[number_of_rabs].rab_id_is_gsm_map = TRUE;

        rb_list_r6 = &rab_list_r6->elem[idx].rb_InformationSetupList;

        rrciho_initialize_rb_list_r6(rb_list_r6,number_of_rabs);

        (void)rrc_update_rab_type_in_rab_info_r6(
                                                 &(rab_list_r6->elem[idx]),
                                                 &(gtow_info.est_rabs[number_of_rabs]));

        MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
                                                     gtow_info.est_rabs[number_of_rabs].rab_id,
                                                     gtow_info.est_rabs[number_of_rabs].cn_domain,
                                                     gtow_info.est_rabs[number_of_rabs].re_est_timer);

        if(rab_list_r6->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
        {
          gtow_info.cs_rab_index = number_of_rabs;
          gtow_info.is_cs_rab_present = TRUE;
          if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
          {
            gtow_info.est_rabs[number_of_rabs].rab_type = RRCRB_CS_VOICE_RAB;
          }
        }
        if(rab_list_r6->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
        {
          gtow_info.is_ps_rab_present = TRUE;
        }

#ifdef FEATURE_VOC_AMR_WB
          if(rab_list_r6->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
          {
            gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = MVS_MODE_AMR;
         /* in the G-> W handover check the Nas-Sync-indicator for the codec type */
            if(rab_list_r6->elem[idx].rab_Info.m.nas_Synchronisation_IndicatorPresent)
            {
              gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = 
                  rrc_translate_nas_sync_indicator(&(rab_list_r6->elem[idx].rab_Info.nas_Synchronisation_Indicator));
              
              if( gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator == MVS_MODE_NONE )
        {
                procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
                WRRC_MSG1_HIGH("Invalid configuration .. codec not supported %d",gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator );
          return FAILURE;
        }
            }
            rrc_set_ordered_codec_mode(gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator);
          }
#endif /*FEATURE_VOC_AMR_WB*/

        number_of_rabs++;
      }
      else
      {
        return FAILURE;
      }
      idx++;
    }while((idx < rab_list_r6->n) && (number_of_rabs < MAX_RAB_TO_SETUP));

  }
  else if(release == MSG_REL7)
  {
    rab_list_r7 = (rrc_RAB_InformationSetupList_r7 *) rab_info_list;

    do 
    {

      if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == rab_list_r7->elem[idx].rab_Info.rab_Identity.t)
      {
        /* GSM_MAP RAB Id. Check if it's valid by calling the
           appropriate NAS entity. First convert it to internal
           format. */
      
        gtow_info.est_rabs[number_of_rabs].rab_id =
           rrcrb_translate_gsm_map_rab_id(rab_list_r7->elem[idx].rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity);
      
        gtow_info.est_rabs[number_of_rabs].re_est_timer = rab_list_r7->elem[idx].rab_Info.re_EstablishmentTimer;

        gtow_info.est_rabs[number_of_rabs].cn_domain = rab_list_r7->elem[idx].rab_Info.cn_DomainIdentity;

        gtow_info.est_rabs[number_of_rabs].rab_id_is_gsm_map = TRUE;

        rb_list_r7 = &rab_list_r7->elem[idx].rb_InformationSetupList;

        rrciho_initialize_rb_list_r7(rb_list_r7, number_of_rabs);

        (void)rrc_update_rab_type_in_rab_info_r7(
           &(rab_list_r7->elem[idx]),
           &(gtow_info.est_rabs[number_of_rabs]));

        MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
                          gtow_info.est_rabs[number_of_rabs].rab_id,
                          gtow_info.est_rabs[number_of_rabs].cn_domain,
                          gtow_info.est_rabs[number_of_rabs].re_est_timer);

        if(rab_list_r7->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
        {
          gtow_info.cs_rab_index = number_of_rabs;
          gtow_info.is_cs_rab_present = TRUE;
          if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
          {
            gtow_info.est_rabs[number_of_rabs].rab_type = RRCRB_CS_VOICE_RAB;
          }
        }
        else if(rab_list_r7->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
        {
          gtow_info.is_ps_rab_present = TRUE;
        }

   #ifdef FEATURE_VOC_AMR_WB
        if(rab_list_r7->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
        {
          gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = MVS_MODE_AMR;
           /* in the G-> W handover check the Nas-Sync-indicator for the codec type */        
          if(rab_list_r7->elem[idx].rab_Info.m.nas_Synchronisation_IndicatorPresent)
          {
            gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = 
                rrc_translate_nas_sync_indicator(&(rab_list_r7->elem[idx].rab_Info.nas_Synchronisation_Indicator));
            
            if( gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator == MVS_MODE_NONE )
            {
              procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
              WRRC_MSG1_HIGH("Invalid configuration .. codec not supported %d",gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator );
              return FAILURE;
            }
          }
          rrc_set_ordered_codec_mode(gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator);
          WRRC_MSG1_HIGH("CSoHS:Codec in rrc_ota_codec_mode_status %d",rrc_ota_codec_mode_status);   
        }
#endif

        number_of_rabs++;
      }
      idx++;
    }while((idx < rab_list_r7->n) && (number_of_rabs < MAX_RAB_TO_SETUP));
  }
  else if((release == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
         || (release == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
         #error code not present
#endif /* FEATURE_WCDMA_REL10 */
          )
  {
    rab_list_r8 = (rrc_RAB_InformationSetupList_r8 *) rab_info_list;

    do 
    {

      if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == rab_list_r8->elem[idx].rab_Info.rab_Identity.t)
      {
        /* GSM_MAP RAB Id. Check if it's valid by calling the
           appropriate NAS entity. First convert it to internal
           format. */
      
        gtow_info.est_rabs[number_of_rabs].rab_id =
           rrcrb_translate_gsm_map_rab_id(rab_list_r8->elem[idx].rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity);
      
        gtow_info.est_rabs[number_of_rabs].re_est_timer = rab_list_r8->elem[idx].rab_Info.re_EstablishmentTimer;
        // Nitin Change from rab_list_r6 to rab_list_r8
        gtow_info.est_rabs[number_of_rabs].cn_domain = rab_list_r8->elem[idx].rab_Info.cn_DomainIdentity;
    
        gtow_info.est_rabs[number_of_rabs].rab_id_is_gsm_map = TRUE;
  
        rb_list_r8 = &rab_list_r8->elem[idx].rb_InformationSetupList;
      
        rrciho_initialize_rb_list_r8(rb_list_r8, number_of_rabs);
  
        (void)rrc_update_rab_type_in_rab_info_r8(
           &(rab_list_r8->elem[idx]),
           &(gtow_info.est_rabs[number_of_rabs]));
  
        MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
                          gtow_info.est_rabs[number_of_rabs].rab_id,
                          gtow_info.est_rabs[number_of_rabs].cn_domain,
                          gtow_info.est_rabs[number_of_rabs].re_est_timer);
  
        if(rab_list_r8->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
        {
          gtow_info.cs_rab_index = number_of_rabs;
          gtow_info.is_cs_rab_present = TRUE;
          if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
          {
            gtow_info.est_rabs[number_of_rabs].rab_type = RRCRB_CS_VOICE_RAB;
          }
        }
        else if(rab_list_r8->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
        {
          gtow_info.is_ps_rab_present = TRUE;
        }

      #ifdef FEATURE_VOC_AMR_WB
        if(rab_list_r8->elem[idx].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
        {
          gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = MVS_MODE_AMR;
           /* in the G-> W handover check the Nas-Sync-indicator for the codec type */        
          if(rab_list_r8->elem[idx].rab_Info.m.nas_Synchronisation_IndicatorPresent)
          {
            gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = 
                rrc_translate_nas_sync_indicator(&(rab_list_r8->elem[idx].rab_Info.nas_Synchronisation_Indicator));
            
            if( gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator == MVS_MODE_NONE )
            {
              procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
              WRRC_MSG1_HIGH("Invalid configuration .. codec not supported %d",gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator );
              return NULL;
            }
          }
          rrc_set_ordered_codec_mode(gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator);
        }
#endif /*FEATURE_VOC_AMR_WB*/

        number_of_rabs++;
      }
      else
      {
        return FAILURE;
      }
      idx++;
    }while((idx < rab_list_r8->n) && (number_of_rabs < MAX_RAB_TO_SETUP));
  }
  else
  {
    WRRC_MSG0_ERROR(" Invalid Release Indicator");
    return FAILURE;
  }

  gtow_info.number_of_est_rabs = number_of_rabs;

  /*set rab included flag to true.  also domain id is always cn domain*/
  gtow_info.rab_info_included = TRUE;

  return SUCCESS;
}
#endif /* FEATURE_LTE_TO_WCDMA */

/*===========================================================================

FUNCTION   rrciho_validate_and_extract_srb_rab_info

DESCRIPTION

  Validates and calls functions to extract srb and rab info

DEPENDENCIES

  None.

RETURN VALUE

  Success or Failure.

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type rrciho_validate_and_extract_srb_rab_info(void)
{
  rrc_SRB_InformationSetupList *srb_list = NULL;
  rrc_RB_InformationSetupList  *rb_list = NULL;  
  rrc_SRB_InformationSetupList_r5 *srb_list_r5 = NULL;
  rrc_RB_InformationSetupList_r5  *rb_list_r5 = NULL;  
  rrc_SRB_InformationSetupList_r6 *srb_list_r6 = NULL;
  rrc_RB_InformationSetupList_r6  *rb_list_r6 = NULL;  

  rrc_SRB_InformationSetupList_r7 *srb_list_r7 = NULL;
  rrc_RB_InformationSetupList_r7  *rb_list_r7 = NULL;  

  rrc_SRB_InformationSetupList_r8 *srb_list_r8 = NULL;
  rrc_RB_InformationSetupList_r8  *rb_list_r8 = NULL;  

#ifndef FEATURE_LTE_TO_WCDMA
   rrc_RAB_InformationSetupList *rab_list = NULL;
   rrc_RAB_InformationSetupList_r5 *rab_list_r5 = NULL;
   rrc_RAB_InformationSetupList_r6 *rab_list_r6 = NULL;
   rrc_RAB_InformationSetupList_r7 *rab_list_r7 = NULL;
   rrc_RAB_InformationSetupList_r8 *rab_list_r8 = NULL;
  boolean rab_info_list_present = FALSE;
#endif /*FEATURE_LTE_TO_WCDMA*/

  uint32  rab_identity_choice = T_rrc_RAB_Identity_ansi_41_RAB_Identity;
  uint32 rab_bit_mask = 0;

#ifdef FEATURE_LTE_TO_WCDMA
  gtow_info.cs_rab_index = 0;
  gtow_info.is_cs_rab_present = FALSE;
  gtow_info.is_ps_rab_present = FALSE;
  gtow_info.number_of_est_rabs= 0;
#endif /* FEATURE_LTE_TO_WCDMA */

  /*if the configuration received is complete then process the struct for
    handover complete */
  if(gtow_info.config_type == IHO_GTOW_COMPLETE_CONFIG)
  {
    if(gtow_info.msg_rel_version == MSG_REL99)
    {
      /* Assign signalling radio bearers list to a local list */
      srb_list = &gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.
                               specificationMode.u.complete.srb_InformationSetupList;
    }
    else if((gtow_info.msg_rel_version == MSG_REL5) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
    {
      /* Assign signalling radio bearers list to a local list */
      srb_list_r5 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.
        specificationMode.u.complete.srb_InformationSetupList;
    }
    else if((gtow_info.msg_rel_version == MSG_REL6) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
    {
      srb_list_r6 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.r6.handoverToUTRANCommand_r6.specificationMode.u.complete.srb_InformationSetupList;
    }
    else if((gtow_info.msg_rel_version == MSG_REL7) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
    {
      srb_list_r7 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.criticalExtensions.u.r7.handoverToUTRANCommand_r7.specificationMode.u.complete.srb_InformationSetupList;
    }
    else if((gtow_info.msg_rel_version == MSG_REL8) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
    {
      srb_list_r8 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.handoverToUTRANCommand_r8.specificationMode.u.complete.srb_InformationSetupList;
    }
#ifdef FEATURE_WCDMA_REL9
    else if((gtow_info.msg_rel_version == MSG_REL9) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
    {
      srb_list_r8 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.handoverToUTRANCommand_r9.specificationMode.u.complete.srb_InformationSetupList;
    }
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */

    else
    {
      /* Shouldn't reach here - check should have already been made in the caller */
      procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
      return FAILURE;
    }

    if(gtow_info.msg_rel_version == MSG_REL99)
    {
      /*if rab information list is present, then initialize the established rabs */
      if(gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.complete.m.rab_InformationSetupListPresent)
      {

#ifdef FEATURE_LTE_TO_WCDMA
        if((rrciho_extract_and_fill_in_rab_rb_info((void *) &gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.
              specificationMode.u.complete.rab_InformationSetupList, MSG_REL99)) == FAILURE)
        {
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
          return FAILURE;
        }
#else
        rab_info_list_present = TRUE;

        /* Check if the RAB id is GSM_MAP type. We don't support any
           other type */
        if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.rab_Identity.t)
        {
          rab_identity_choice = T_rrc_RAB_Identity_gsm_MAP_RAB_Identity;

          gtow_info.est_rabs.rab_id =
             rrcrb_translate_gsm_map_rab_id( gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.
                                         specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.
                                         rab_Identity.u.gsm_MAP_RAB_Identity);

          gtow_info.est_rabs.re_est_timer = gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.
                 specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.re_EstablishmentTimer;

          gtow_info.est_rabs.cn_domain = gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.
                 specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.cn_DomainIdentity;

          rb_list = &gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.
                                specificationMode.u.complete.rab_InformationSetupList.elem[0].
                                rb_InformationSetupList;

          rrciho_initialize_rb_list(rb_list);

          rab_list = &gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.complete.
           rab_InformationSetupList;
#ifdef FEATURE_VOC_AMR_WB
           /* in the G-> W handover check the Nas-Sync-indicator for the codec type */
          if(rrciho_update_nas_sync_ind(rab_list->elem[0].rab_Info) == FAILURE)
          {
            procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
            return FAILURE;
          }
#endif /*FEATURE_VOC_AMR_WB*/
        }

#endif /* FEATURE_LTE_TO_WCDMA */
      }
    }
    else if((gtow_info.msg_rel_version == MSG_REL5) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
    {
      if(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.
	  	r5.handoverToUTRANCommand_r5.specificationMode.u.complete.m.rab_InformationSetupListPresent)
      {

#ifdef FEATURE_LTE_TO_WCDMA
        if((rrciho_extract_and_fill_in_rab_rb_info((void *) &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.
              handoverToUTRANCommand_r5.specificationMode.u.complete.rab_InformationSetupList, MSG_REL5)) == FAILURE)
        {
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
          return FAILURE;
        }
#else
        rab_info_list_present = TRUE;

        /* Check if the RAB id is GSM_MAP type. We don't support any
           other type */
        if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.rab_Identity.t)
        {
          rab_identity_choice = T_rrc_RAB_Identity_gsm_MAP_RAB_Identity;

          /* GSM_MAP RAB Id. Check if it's valid by calling the
             appropriate NAS entity. First convert it to internal
             format. */

          gtow_info.est_rabs.rab_id =
             rrcrb_translate_gsm_map_rab_id(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.
                                         specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.
                                         rab_Identity.u.gsm_MAP_RAB_Identity);

          gtow_info.est_rabs.re_est_timer = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.
                                         specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.re_EstablishmentTimer;

          gtow_info.est_rabs.cn_domain = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.
                                         specificationMode.u.complete.rab_InformationSetupList.
                                         elem[0].rab_Info.cn_DomainIdentity;

          rb_list_r5 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.
            specificationMode.u.complete.rab_InformationSetupList.elem[0].
            rb_InformationSetupList;

          rrciho_initialize_rb_list_r5(rb_list_r5);

          rab_list_r5 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.complete.
           rab_InformationSetupList;

#ifdef FEATURE_VOC_AMR_WB
          
          /* in the G-> W handover check the Nas-Sync-indicator for the codec type */
          if(rrciho_update_nas_sync_ind(rab_list_r5->elem[0].rab_Info) == FAILURE)
          {
            procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
            return FAILURE;
          }
         
#endif /*FEATURE_VOC_AMR_WB*/
        }

#endif /* FEATURE_LTE_TO_WCDMA */
      }
    }
    else if((gtow_info.msg_rel_version == MSG_REL6) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
    {
      if(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.
         specificationMode.u.complete.m.rab_InformationSetupListPresent)
      {

#ifdef FEATURE_LTE_TO_WCDMA
        if((rrciho_extract_and_fill_in_rab_rb_info((void *) &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
              handoverToUTRANCommand_r6.specificationMode.u.complete.rab_InformationSetupList, MSG_REL6)) == FAILURE)
        {
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
          return FAILURE;
        }
#else
        rab_info_list_present = TRUE;

        /* Check if the RAB id is GSM_MAP type. We don't support any
           other type */
        if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6
           .handoverToUTRANCommand_r6.specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.rab_Identity.t)
        {
          rab_identity_choice = T_rrc_RAB_Identity_gsm_MAP_RAB_Identity;

          /* GSM_MAP RAB Id. Check if it's valid by calling the
             appropriate NAS entity. First convert it to internal
             format. */

          gtow_info.est_rabs.rab_id =
             rrcrb_translate_gsm_map_rab_id(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
                                            handoverToUTRANCommand_r6.specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.
                                            rab_Identity.u.gsm_MAP_RAB_Identity);

          gtow_info.est_rabs.re_est_timer = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
            u.r6.handoverToUTRANCommand_r6.specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.re_EstablishmentTimer;

          gtow_info.est_rabs.cn_domain = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
            u.r6.handoverToUTRANCommand_r6.specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.cn_DomainIdentity;

          rb_list_r6 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
            u.r6.handoverToUTRANCommand_r6.specificationMode.u.complete.rab_InformationSetupList.elem[0].rb_InformationSetupList;

          rrciho_initialize_rb_list_r6(rb_list_r6);

          rab_list_r6 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.
            criticalExtensions.u.r6.handoverToUTRANCommand_r6.specificationMode.u.complete.rab_InformationSetupList;

#ifdef FEATURE_VOC_AMR_WB
          if(rab_list_r6->elem[0].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
          {
            gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_AMR;
             /* in the G-> W handover check the Nas-Sync-indicator for the codec type */        
            if(rab_list_r6->elem[0].rab_Info.m.nas_Synchronisation_IndicatorPresent)
            {
              gtow_info.est_rabs.nas_Synchronisation_Indicator = 
                  rrc_translate_nas_sync_indicator((ASN1BitStr32  *)&(rab_list_r6->elem[0].rab_Info.nas_Synchronisation_Indicator));
              
              if( gtow_info.est_rabs.nas_Synchronisation_Indicator == MVS_MODE_NONE )
              {
                procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
                WRRC_MSG1_HIGH("Invalid configuration .. codec not supported %d",gtow_info.est_rabs.nas_Synchronisation_Indicator );
                return FAILURE;
              }
            }
            rrc_set_ordered_codec_mode(gtow_info.est_rabs.nas_Synchronisation_Indicator);
          }
#endif /*FEATURE_VOC_AMR_WB*/

          /*lint +e740 */
        }
#endif /* FEATURE_LTE_TO_WCDMA */
      }
    }

    else if((gtow_info.msg_rel_version == MSG_REL7) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
    {
      if(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.handoverToUTRANCommand_r7.
         specificationMode.u.complete.m.rab_InformationSetupListPresent)
      {
#ifdef FEATURE_LTE_TO_WCDMA
        if((rrciho_extract_and_fill_in_rab_rb_info((void *) &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
              u.criticalExtensions.u.r7.handoverToUTRANCommand_r7.specificationMode.u.complete.rab_InformationSetupList, MSG_REL7)) == FAILURE)
        {
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
          return FAILURE;
        }
#else
        rab_info_list_present = TRUE;

        rab_list_r7 = rrciho_update_rab_info_r7(&rab_identity_choice);

#endif /* FEATURE_LTE_TO_WCDMA */
      }
    }
    else if((gtow_info.msg_rel_version == MSG_REL8) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
    {
      if(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.handoverToUTRANCommand_r8.
         specificationMode.u.complete.m.rab_InformationSetupListPresent)
      {
#ifdef FEATURE_LTE_TO_WCDMA
        if((rrciho_extract_and_fill_in_rab_rb_info((void *) &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
              u.criticalExtensions.u.r8.handoverToUTRANCommand_r8.specificationMode.u.complete.rab_InformationSetupList, MSG_REL8)) == FAILURE)
        {
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
          return FAILURE;
        }
#else
        rab_info_list_present = TRUE;

       rab_list_r8 = rrciho_update_rab_info_r8(&rab_identity_choice 
#ifdef FEATURE_WCDMA_REL9
                                               ,MSG_REL8
#endif /* FEATURE_WCDMA_REL9 */
                                               );
#endif /*FEATURE_LTE_TO_WCDMA */
      }
    }
#ifdef FEATURE_WCDMA_REL9
    else if((gtow_info.msg_rel_version == MSG_REL9) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
    {
      if(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.handoverToUTRANCommand_r9.
         specificationMode.u.complete.m.rab_InformationSetupListPresent)
      {
#ifdef FEATURE_LTE_TO_WCDMA
        if((rrciho_extract_and_fill_in_rab_rb_info((void *) &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
              u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.handoverToUTRANCommand_r9.specificationMode.u.complete.rab_InformationSetupList, MSG_REL9)) == FAILURE)
        {
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
          return FAILURE;
        }
#else
        rab_info_list_present = TRUE;

       rab_list_r8 = rrciho_update_rab_info_r8(&rab_identity_choice,MSG_REL9);
#endif /*FEATURE_LTE_TO_WCDMA */
      }
    }

#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */



#ifndef FEATURE_LTE_TO_WCDMA
    /* ASN1 structure 'rrc_RAB_InformationSetup' is validated at this point,
         hence update rab_type in structure 'rbe.rabs.rabs_in_msg[rab_index]'.
    */
    if((rab_list!=NULL) && (rab_list->n != 0))
    {
      (void)rrc_update_rab_type_in_rab_info(
         &(rab_list->elem[0]),
         &(gtow_info.est_rabs));
      }
    else if((rab_list_r5!=NULL) &&(rab_list_r5->n != 0))
    {
      (void)rrc_update_rab_type_in_rab_info_r5(
         &(rab_list_r5->elem[0]),
         &(gtow_info.est_rabs));
    }
    else if((rab_list_r6 !=NULL) &&(rab_list_r6->n != 0))
    {
      (void)rrc_update_rab_type_in_rab_info_r6(
         &(rab_list_r6->elem[0]),
         &(gtow_info.est_rabs));
    }
    else if((rab_list_r7 !=NULL) &&(rab_list_r7->n != 0))
    {
      (void)rrc_update_rab_type_in_rab_info_r7(
         &(rab_list_r7->elem[0]),
         &(gtow_info.est_rabs));
    }
    else if((rab_list_r8!=NULL) && (rab_list_r8->n != 0))
    {
      (void)rrc_update_rab_type_in_rab_info_r8(
         &(rab_list_r8->elem[0]),
         &(gtow_info.est_rabs));
    }

      MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
                        gtow_info.est_rabs.rab_id,
                        gtow_info.est_rabs.cn_domain,
                        gtow_info.est_rabs.re_est_timer);
#endif /* FEATURE_LTE_TO_WCDMA */

  }
  else
  {
    rrcrb_rab_e_type rab_type = RRCRB_UNKNOWN_RAB;      /* Type of RAB - PS, CS-DATA, CS-VOICE */ 
    rrc_rab_id_type rab_id;          /* GSM-MAP RAB Identity                 */
    rrc_Re_EstablishmentTimer re_est_timer;         /* Re-est timer associated with RAB    */

/* Processing for default configs received in HO msg*/
    gtow_info.rbs_ptr = NULL;
    gtow_info.sib16_ptr = NULL;

    switch(gtow_info.default_config_id)
    {
      /* Fall through cases that have to processed */
      case RRC_DEF_CONFIG_0:
      case RRC_DEF_CONFIG_1:
      case RRC_DEF_CONFIG_3:
      case RRC_DEF_CONFIG_4:
      case RRC_DEF_CONFIG_7:
      case RRC_DEF_CONFIG_8:
      case RRC_DEF_CONFIG_9:
      case RRC_DEF_CONFIG_10:  
      /* default config 22,15,18,19 is valid only for release 6  */
      case RRC_DEF_CONFIG_15:
      case RRC_DEF_CONFIG_18:
      case RRC_DEF_CONFIG_19:
      case RRC_DEF_CONFIG_22:
  #ifdef FEATURE_VOC_AMR_WB
      case RRC_DEF_CONFIG_21:
      case RRC_DEF_CONFIG_16:
  #endif
#ifdef FEATURE_WCDMA_REL10
      #error code not present
#endif
      { 

        if((gtow_info.default_config_id == RRC_DEF_CONFIG_0 ) ||
           (gtow_info.default_config_id == RRC_DEF_CONFIG_1 ) 
           ||(gtow_info.default_config_id == RRC_DEF_CONFIG_22 )
#ifdef FEATURE_WCDMA_REL10
           #error code not present
#endif
          )
        {
          WRRC_MSG1_HIGH("G->W Signalling only default config Id %d chosen",gtow_info.default_config_id);
        }
        /*first get the sib16 pointer.. currently only supporting default configuration.  will support
          real sib's soon*/
        gtow_info.sib16_ptr =
          (rrc_SysInfoType16 *)
        rrcasn1_decode_pdu( (void *)default_config_info[gtow_info.default_config_id].default_config_ptr,
                            rrc_SysInfoType16_PDU,
                            default_config_info[gtow_info.default_config_id].default_config_len
                            );
    
        gtow_info.def_config_ptr_present = TRUE;
    
        /*initialize signalling radio bearer list*/
        srb_list = &gtow_info.sib16_ptr->preDefinedRadioConfiguration.
                                   predefinedRB_Configuration.srb_InformationList;
      
        rb_list = &gtow_info.sib16_ptr->preDefinedRadioConfiguration.
                                  predefinedRB_Configuration.rb_InformationList;
      }
      break;
  
      case RRC_DEF_CONFIG_11:
      case RRC_DEF_CONFIG_12:
    #ifdef FEATURE_VOC_AMR_WB
      case  RRC_DEF_CONFIG_13:
    #endif/*FEATURE_VOC_AMR_WB*/
    case RRC_DEF_CONFIG_14:
      {
        if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
        {
          gtow_info.rbs_ptr =
          (rrc_DL_DCCH_Message *)
          rrcasn1_decode_pdu( (void *)default_config_info[gtow_info.default_config_id].default_config_ptr,
                            rrc_DL_DCCH_Message_PDU,
                            default_config_info[gtow_info.default_config_id].default_config_len
                            );
          if(gtow_info.rbs_ptr == NULL)
          {
            ERR_FATAL("No memory for RBS ptr",0,0,0);
          }   
          gtow_info.def_config_ptr_present = TRUE;
          WRRC_MSG1_HIGH("Decoded DEFAULT CONFIG %d Successfully from internal RB setup message", gtow_info.default_config_id );
    
          /*initialize signalling radio bearer list*/
          srb_list_r5 = &gtow_info.rbs_ptr->message.u.radioBearerSetup.u.later_than_r3.criticalExtensions.u.
            criticalExtensions.u.r5.radioBearerSetup_r5.srb_InformationSetupList;
        
          rb_list_r5 = &gtow_info.rbs_ptr->message.u.radioBearerSetup.u.later_than_r3.criticalExtensions.u.
            criticalExtensions.u.r5.radioBearerSetup_r5.rab_InformationSetupList.elem[0].rb_InformationSetupList;   
        }
      }
      break;
  
      default:
        WRRC_MSG1_ERROR("Unsupported Def Config ID %d", gtow_info.default_config_id);
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
        return FAILURE;
    }

    

    if(gtow_info.msg_rel_version == MSG_REL99)
    {
      rab_bit_mask =gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.
       preconfiguration.m.rab_InfoPresent;
    }
    else if((gtow_info.msg_rel_version == MSG_REL5) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
    {
      rab_bit_mask = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.
       preconfiguration.m.rab_InfoPresent;
    }
    else if((gtow_info.msg_rel_version == MSG_REL6) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
    {
      rab_bit_mask = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.
        specificationMode.u.preconfiguration.m.rab_InfoPresent;
    }
    else if((gtow_info.msg_rel_version == MSG_REL7) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
    {
      rab_bit_mask = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.handoverToUTRANCommand_r7.
        specificationMode.u.preconfiguration.m.rab_InfoPresent;
    }
    else if((gtow_info.msg_rel_version == MSG_REL8) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
    {
      rab_bit_mask = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.handoverToUTRANCommand_r8.
        specificationMode.u.preconfiguration.m.rab_InfoPresent;
    }
#ifdef FEATURE_WCDMA_REL9
    else if((gtow_info.msg_rel_version == MSG_REL9) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
    {
      rab_bit_mask = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
        later_r8_criticalExtensions.u.r9.handoverToUTRANCommand_r9.specificationMode.u.preconfiguration.m.rab_InfoPresent;
    }

#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */


    /*if rab information is present then initialize rab information*/
    if(rab_bit_mask)
    {
#ifndef FEATURE_LTE_TO_WCDMA
      rab_info_list_present = TRUE;
#endif

      /* Check if the RAB id is GSM_MAP type. We don't support any
         other type */

      if(gtow_info.msg_rel_version == MSG_REL99)
      {
        rab_identity_choice = gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.
           preconfiguration.rab_Info.rab_Identity.t;
#ifdef FEATURE_VOC_AMR_WB
        if(rrciho_update_def_config_nas_sync_ind(
           &(gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.preconfiguration.rab_Info)
#ifdef FEATURE_LTE_TO_WCDMA
           , gtow_info.cs_rab_index
#endif /* FEATURE_LTE_TO_WCDMA */
           ) == FAILURE)
        {
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
          return FAILURE;
        }
        
#endif /*FEATURE_VOC_AMR_WB*/

      }
      else if((gtow_info.msg_rel_version == MSG_REL5) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
      {
        rab_identity_choice = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.
           preconfiguration.rab_Info.rab_Identity.t;
#ifdef FEATURE_VOC_AMR_WB
   
        if( rrciho_update_def_config_nas_sync_ind(
            &(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.
                       specificationMode.u.preconfiguration.rab_Info)
#ifdef FEATURE_LTE_TO_WCDMA
            , gtow_info.cs_rab_index
#endif /* FEATURE_LTE_TO_WCDMA */
            ) == FAILURE)
        {
           procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
           return FAILURE;
        }
     
     
        if( gtow_info.default_config_id ==  RRC_DEF_CONFIG_13)
        {
#ifndef FEATURE_LTE_TO_WCDMA
          gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#else
          gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#endif 
          rrc_set_ordered_codec_mode(MVS_MODE_AMR_WB);
        }

#endif /*FEATURE_VOC_AMR_WB*/

      }

      else if((gtow_info.msg_rel_version == MSG_REL6) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
      {
        rab_identity_choice = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
          handoverToUTRANCommand_r6.specificationMode.u.preconfiguration.rab_Info.rab_Identity.t;
#ifdef FEATURE_VOC_AMR_WB

        if( rrciho_update_def_config_nas_sync_ind(
            &(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
            handoverToUTRANCommand_r6.specificationMode.u.preconfiguration.rab_Info)
#ifdef FEATURE_LTE_TO_WCDMA
            , gtow_info.cs_rab_index
#endif /* FEATURE_LTE_TO_WCDMA */
            ) == FAILURE)
        {
          return FAILURE;
        }
         
        if( (gtow_info.default_config_id ==  RRC_DEF_CONFIG_16) ||
            ( gtow_info.default_config_id ==  RRC_DEF_CONFIG_21)
        || ( gtow_info.default_config_id ==  RRC_DEF_CONFIG_13)
         )
        {
#ifndef FEATURE_LTE_TO_WCDMA
          gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#else
          gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#endif
          rrc_set_ordered_codec_mode(MVS_MODE_AMR_WB);
        }
#endif /*FEATURE_VOC_AMR_WB*/
      }
      else if((gtow_info.msg_rel_version == MSG_REL7) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
      {
        rab_identity_choice = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.
          handoverToUTRANCommand_r7.specificationMode.u.preconfiguration.rab_Info.rab_Identity.t;
#ifdef FEATURE_VOC_AMR_WB

        if( rrciho_update_def_config_nas_sync_ind(
            &(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.
            handoverToUTRANCommand_r7.specificationMode.u.preconfiguration.rab_Info)
#ifdef FEATURE_LTE_TO_WCDMA
            , gtow_info.cs_rab_index
#endif /* FEATURE_LTE_TO_WCDMA */
            ) == FAILURE)
        {
          return FAILURE;
        }
         
        if( (gtow_info.default_config_id ==  RRC_DEF_CONFIG_16) ||
            ( gtow_info.default_config_id ==  RRC_DEF_CONFIG_21)
        || ( gtow_info.default_config_id ==  RRC_DEF_CONFIG_13)
         )
        {
#ifndef FEATURE_LTE_TO_WCDMA
          gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#else
          gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#endif
          rrc_set_ordered_codec_mode(MVS_MODE_AMR_WB);
        }
#endif /*FEATURE_VOC_AMR_WB*/
      }

      else if((gtow_info.msg_rel_version == MSG_REL8) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
      {
        rab_identity_choice = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.
          handoverToUTRANCommand_r8.specificationMode.u.preconfiguration.rab_Info.rab_Identity.t;
#ifdef FEATURE_VOC_AMR_WB

        if( rrciho_update_def_config_nas_sync_ind(
            &(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.
            handoverToUTRANCommand_r8.specificationMode.u.preconfiguration.rab_Info)
#ifdef FEATURE_LTE_TO_WCDMA
            , gtow_info.cs_rab_index
#endif /* FEATURE_LTE_TO_WCDMA */
           ) == FAILURE)
        {
          return FAILURE;
        }
         
        if( (gtow_info.default_config_id ==  RRC_DEF_CONFIG_16) ||
            ( gtow_info.default_config_id ==  RRC_DEF_CONFIG_21)
        || ( gtow_info.default_config_id ==  RRC_DEF_CONFIG_13)
         )
        {
#ifndef FEATURE_LTE_TO_WCDMA
          gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#else
          gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#endif
          rrc_set_ordered_codec_mode(MVS_MODE_AMR_WB);
        }
#endif /*FEATURE_VOC_AMR_WB*/
      }
#ifdef FEATURE_WCDMA_REL9
      else if((gtow_info.msg_rel_version == MSG_REL9) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
      {
        rab_identity_choice = gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.
          handoverToUTRANCommand_r9.specificationMode.u.preconfiguration.rab_Info.rab_Identity.t;
#ifdef FEATURE_VOC_AMR_WB

        if( rrciho_update_def_config_nas_sync_ind(
            &(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
            later_r8_criticalExtensions.u.r9.handoverToUTRANCommand_r9.specificationMode.u.preconfiguration.rab_Info)
#ifdef FEATURE_LTE_TO_WCDMA
            , gtow_info.cs_rab_index
#endif /* FEATURE_LTE_TO_WCDMA */
           ) == FAILURE)
        {
          return FAILURE;
        }
         
        if( (gtow_info.default_config_id ==  RRC_DEF_CONFIG_16) ||
            ( gtow_info.default_config_id ==  RRC_DEF_CONFIG_21)
        || ( gtow_info.default_config_id ==  RRC_DEF_CONFIG_13)
         )
        {
#ifndef FEATURE_LTE_TO_WCDMA
          gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#else
          gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = MVS_MODE_AMR_WB;
#endif
          rrc_set_ordered_codec_mode(MVS_MODE_AMR_WB);
        }
#endif /*FEATURE_VOC_AMR_WB*/
      }
#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
      #error code not present
#endif /* FEATURE_WCDMA_REL10 */

      /* Check if the RAB id is GSM_MAP type. We don't support any
         other type */
      if (rab_identity_choice == T_rrc_RAB_Identity_gsm_MAP_RAB_Identity)
      {
        /* GSM_MAP RAB Id. Check if it's valid by calling the
         appropriate NAS entity. First convert it to internal
         format. */

        if(gtow_info.msg_rel_version == MSG_REL99)
        {
          rab_id = rrcrb_translate_gsm_map_rab_id(gtow_info.ho_ptr->u.r3.handoverToUTRANCommand_r3.
                                         specificationMode.u.preconfiguration.rab_Info.
                                         rab_Identity.u.gsm_MAP_RAB_Identity);
        }
        else if((gtow_info.msg_rel_version == MSG_REL5) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
        {
          rab_id = rrcrb_translate_gsm_map_rab_id(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.
                                         specificationMode.u.preconfiguration.rab_Info.
                                         rab_Identity.u.gsm_MAP_RAB_Identity);
        }
        else if((gtow_info.msg_rel_version == MSG_REL6) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
        {
          rab_id = rrcrb_translate_gsm_map_rab_id(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.
                                            criticalExtensions.u.r6.handoverToUTRANCommand_r6.specificationMode.u.
                                            preconfiguration.rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity);
        }
        else if((gtow_info.msg_rel_version == MSG_REL7) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
        {
          rab_id = rrcrb_translate_gsm_map_rab_id(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
                                            criticalExtensions.u.r7.handoverToUTRANCommand_r7.specificationMode.u.
                                            preconfiguration.rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity);
        }
        else if((gtow_info.msg_rel_version == MSG_REL8) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
        {
          rab_id = rrcrb_translate_gsm_map_rab_id(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
                                            criticalExtensions.u.r8.handoverToUTRANCommand_r8.specificationMode.u.
                                            preconfiguration.rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity);
        }
#ifdef FEATURE_WCDMA_REL9
       else if((gtow_info.msg_rel_version == MSG_REL9) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
       {
         rab_id = rrcrb_translate_gsm_map_rab_id(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
                                           criticalExtensions.u.later_r8_criticalExtensions.u.r9.handoverToUTRANCommand_r9.specificationMode.u.
                                           preconfiguration.rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity);
       }

#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
      #error code not present
#endif /* FEATURE_WCDMA_REL10 */

        else
        {
          /* Shouldn't reach here - check should have already been made in the caller */
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
          return FAILURE;
        }

        re_est_timer = rrc_Re_EstablishmentTimer_useT314;

        /* Currently we support def-config 0,1,3,4,7,8,9..  0 & 1 is for sig only, 3 for voice.. so update
           est_rabs info for 3 only..  this is done here on a hard-coded basis, since the
           rab info is not a composite struct as we get in rb_setup/ho_complete_config
           right now anything other than 3 is set to UNKNOWN_RAB, when we support CS-DATA,
           we need to update the rab_type based on def-config-info
        */
        switch(gtow_info.default_config_id)
        {
        case RRC_DEF_CONFIG_3:
        case RRC_DEF_CONFIG_11:
        case RRC_DEF_CONFIG_12:
        case RRC_DEF_CONFIG_14:
#ifdef FEATURE_VOC_AMR_WB
        case  RRC_DEF_CONFIG_13:
#endif/*FEATURE_VOC_AMR_WB*/            

#ifdef FEATURE_VOC_AMR_WB
        case RRC_DEF_CONFIG_21:
        case RRC_DEF_CONFIG_16:
#endif/*FEATURE_VOC_AMR_WB*/

        case RRC_DEF_CONFIG_10:
        case RRC_DEF_CONFIG_15:
        case RRC_DEF_CONFIG_18:
        case RRC_DEF_CONFIG_19:
#ifdef FEATURE_WCDMA_REL10			
        #error code not present
#endif
          rab_type = RRCRB_CS_VOICE_RAB;
          break;

        case RRC_DEF_CONFIG_0:
        case RRC_DEF_CONFIG_1:
        case RRC_DEF_CONFIG_22:

          rab_type = RRCRB_UNKNOWN_RAB;
          break;

        case RRC_DEF_CONFIG_4:
        case RRC_DEF_CONFIG_7:
        case RRC_DEF_CONFIG_8:
        case RRC_DEF_CONFIG_9:      
          rab_type = RRCRB_CS_DATA_RAB;
          break;

        default:
          WRRC_MSG1_ERROR("Unsupported Def Config ID %d", gtow_info.default_config_id);
          break;
        }

      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
            "Received default config -> %d: RAB type -> %d RAB_ID:%d,re_est_tmr:%d", 
            gtow_info.default_config_id, rab_type,rab_id, re_est_timer);		

#ifndef FEATURE_LTE_TO_WCDMA

        gtow_info.est_rabs.rab_id = rab_id;
        gtow_info.est_rabs.re_est_timer = re_est_timer;
        gtow_info.est_rabs.rab_type = rab_type;

        if (rb_list != NULL) 
        {
          rrciho_initialize_rb_list(rb_list);
        }
        else if (rb_list_r5!= NULL) 
        {
          rrciho_initialize_rb_list_r5(rb_list_r5);
        }
        else if (rb_list_r6 != NULL) 
        {
          rrciho_initialize_rb_list_r6(rb_list_r6);
        }
        else if (rb_list_r7 != NULL) 
        {
          rrciho_initialize_rb_list_r7(rb_list_r7);
        }
        else if (rb_list_r8 != NULL) 
        {
          rrciho_initialize_rb_list_r8(rb_list_r8);
        }
#else
        /* As per spec, for PS handover complete configuration is given by the UTRAN
         * In future this code needs to be extended to support PS Handover */

        gtow_info.is_cs_rab_present = TRUE;
        gtow_info.est_rabs[gtow_info.cs_rab_index].rab_id = rab_id;
        gtow_info.est_rabs[gtow_info.cs_rab_index].re_est_timer = re_est_timer;
        gtow_info.est_rabs[gtow_info.cs_rab_index].rab_type = rab_type;

        gtow_info.rab_info_included = TRUE;
        gtow_info.est_rabs[gtow_info.cs_rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;

        /* Store this info locally so that ESTABLISHED_RABS can
           be updated later, when these RABs are established */
        gtow_info.est_rabs[gtow_info.cs_rab_index].rab_id_is_gsm_map = TRUE;

        gtow_info.number_of_est_rabs = 1;

        if (rb_list != NULL) 
        {
          rrciho_initialize_rb_list(rb_list, gtow_info.cs_rab_index);
        }
        else if (rb_list_r5!= NULL) 
        {
          rrciho_initialize_rb_list_r5(rb_list_r5, gtow_info.cs_rab_index);
        }
        else if (rb_list_r6 != NULL) 
        {
          rrciho_initialize_rb_list_r6(rb_list_r6, gtow_info.cs_rab_index);
        }
        else if (rb_list_r7 != NULL) 
        {
          rrciho_initialize_rb_list_r7(rb_list_r7, gtow_info.cs_rab_index);
        }
        else if (rb_list_r8 != NULL) 
        {
          rrciho_initialize_rb_list_r8(rb_list_r8, gtow_info.cs_rab_index);
        }

#endif /* FEATURE_LTE_TO_WCDMA */
      }
      else
      {
        WRRC_MSG0_HIGH("RAB ID is not GSM_MAP!");
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
        return FAILURE;
      }
    }
  }

  if(NULL != srb_list)
  {
    rrciho_initialize_srb_list(srb_list);
  }
  else if (NULL != srb_list_r5) 
  {
    rrciho_initialize_srb_list_r5(srb_list_r5);
  }
  else if (NULL != srb_list_r6) 
  {
    rrciho_initialize_srb_list_r6(srb_list_r6);
  }
  else if (NULL != srb_list_r7) 
  {
    rrciho_initialize_srb_list_r7(srb_list_r7);
  }
  else if (NULL != srb_list_r8) 
  {
    rrciho_initialize_srb_list_r8(srb_list_r8);
  }

  else
  {
    procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
    return FAILURE;
  }

#ifndef FEATURE_LTE_TO_WCDMA
  if(rab_info_list_present == TRUE)
  {
    if (rab_identity_choice != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity)
    {
      WRRC_MSG0_HIGH("RAB ID is not GSM_MAP!");
      procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
      return FAILURE;
    }

    /*set rab included flag to true.  also domain id is always cn domain*/
    gtow_info.rab_info_included = TRUE;
    gtow_info.est_rabs.cn_domain = RRC_CS_DOMAIN_CN_ID;

    /* Store this info locally so that ESTABLISHED_RABS can
       be updated later, when these RABs are established */
    gtow_info.est_rabs.rab_id_is_gsm_map = TRUE;     
  }

#else
  MSG_HIGH("CS RAB Present: %d, CS RAB Index : %d, PS RAB Present: %d",gtow_info.is_cs_rab_present,
    gtow_info.cs_rab_index,gtow_info.is_ps_rab_present);

if(((gtow_info.is_cs_rab_present == TRUE) && (iho_interrat_type == RRCIHO_INTERRAT_LTE) && (gtow_info.cs_ho_is_supported == FALSE))||
    ((gtow_info.is_ps_rab_present == TRUE) && (iho_interrat_type == RRCIHO_INTERRAT_LTE) && (gtow_info.ps_ho_is_supported == FALSE)))
  {
    WRRC_MSG2_HIGH("CS supported: %d, PS supported: %d",gtow_info.cs_ho_is_supported,gtow_info.ps_ho_is_supported);
    procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
    return FAILURE;    
  }
#endif /* FEATURE_LTE_TO_WCDMA */

  return SUCCESS;
}/*rrciho_validate_and_extract_srb_rab_info*/


   /*===========================================================================
   
   FUNCTION   rrciho_get_tx_dix_mode_info
   
   DESCRIPTION
   
     This function checks for the tx diversity mode and sets the 
     gtow_info accordingly which is later used to fill ACQ REQ to L1
   
   DEPENDENCIES
   
	 None.
   
   RETURN VALUE
   
     None
   
   SIDE EFFECTS
   
	 None.
   
   ===========================================================================*/

void  rrciho_set_tx_dix_mode_info(rrc_TX_DiversityMode tx_div_mode)
{
  switch(tx_div_mode)
  {
    case rrc_TX_DiversityMode_sttd:
    case rrc_TX_DiversityMode_closedLoopMode1:
      /* If div_mode is set as STTD or CLMode in HO msg then set div_info to TRUE */
      gtow_info.diversity_info_present = TRUE;
      gtow_info.diversity_info = TRUE;
    break;
       
    case rrc_TX_DiversityMode_noDiversity:
    case rrc_TX_DiversityMode_dummy:
      /* If we come here then div_mode is set as NONE/CLOSED LOOP2 in HO msg 
         Set diversity_info_present to TRUE and diversity_info to FALSE */
      gtow_info.diversity_info_present = TRUE;
      gtow_info.diversity_info = FALSE;
    break;

     default:
       WRRC_MSG1_ERROR("Bad TX Diversity Mode %u", tx_div_mode);
  } /* end switch on Diversity Mode */
}


/*===========================================================================

FUNCTION   verify_and_extract_handover_to_utran_command

DESCRIPTION

  This function verifies the handover to UTRAN command.  If an error is
  detected sends a failure. If the command is valid, it extracts necessary
  information from the command.

DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type verify_and_extract_handover_to_utran_command
(
  rrc_HandoverToUTRANCommand *ho_ptr     /* Pointer to the HO message */
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
  , rrc_plmn_identity_type      plmn_id    /* GSM camped on PLMN_Id */
#endif
)
{
  boolean ciphering_algo_present = FALSE;
  rrc_CipheringAlgorithm ciphering_algo = rrc_CipheringAlgorithm_uea0;
  rrciho_gtow_config_e_type config_type = IHO_GTOW_PREDEF_CONFIG;
  rrc_DL_InformationPerRL_List *dl_info_per_rl = NULL;

  /* Legacy bug of typecasting to a wrong structure */
  rrc_DL_InformationPerRL_List_r5 *dl_info_per_rl_r5 = NULL;
  rrc_DL_InformationPerRL_List_r6 *dl_info_per_rl_r6 = NULL;
  rrc_DL_InformationPerRL_ListPostFDD *dl_info_per_rl_default_r6 = NULL;
  rrc_DL_InformationPerRL_List_r7 *dl_info_per_rl_r7 = NULL;


  rrc_DL_CommonInformation *dl_common_info = NULL;
  rrc_DL_CommonInformation_r4 *dl_common_info_r4 = NULL;
  rrc_DL_CommonInformation_r6 *dl_common_info_r6 = NULL;
  rrc_DL_CommonInformation_r7 *dl_common_info_r7 = NULL;
  rrc_DL_CommonInformation_r8 *dl_common_info_r8 = NULL;

  boolean closed_loop_info = FALSE;

  rrc_DefaultConfigMode default_config_mode = rrc_DefaultConfigMode_tdd;
  rrc_scr_code_type scrambling_code_type;
  rrc_FrequencyInfoFDD * freq_info =NULL;
  uint32 default_config_id = RRC_DEF_CONFIG_INVALID;
  rrc_CipheringAlgorithm_r7 ciphering_algo_r7 = rrc_CipheringAlgorithm_r7_uea0;
  gtow_info.msg_rel_version = MSG_INVALID;

  memset((void *)&scrambling_code_type, 0, sizeof(rrc_scr_code_type));

  if(ho_ptr->t == T_rrc_HandoverToUTRANCommand_r3)
  {
    gtow_info.msg_rel_version = MSG_REL99;

    WRRC_MSG0_HIGH("RRCIHO - Rel99 message received");

#ifdef FEATURE_LTE_TO_WCDMA
    /* In case of PSHO from EUTRA, only process complete config */
    if((ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.t ==
       T_rrc_HandoverToUTRANCommand_r3_IEs_specificationMode_preconfiguration )
       && (ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.preconfiguration.preConfigMode.t == 
            T_rrc_HandoverToUTRANCommand_r3_IEs_preConfigMode_predefinedConfigIdentity)
       &&  (iho_interrat_type == RRCIHO_INTERRAT_LTE))
    {
      WRRC_MSG0_HIGH("LTOW_PSHO:Predefined config ID present. Send failure to LRRC.");
      return FAILURE;
    }
#endif

    /*Check if Ciphering algo is present and validate the same*/
    if(RRC_MSG_COMMON_BITMASK_IE(ho_ptr->u.r3.handoverToUTRANCommand_r3,
		rrc_HandoverToUTRANCommand_r3_IEs,cipheringAlgorithm))
    {
      ciphering_algo_present = TRUE;
      /*Copy Ciphering Algo to local struct*/
      ciphering_algo = ho_ptr->u.r3.handoverToUTRANCommand_r3.cipheringAlgorithm;
      WRRC_MSG1_HIGH("RRCIHO - Rel99: ciphering algorithm -%d signaled", ciphering_algo);
    }

    /*Check if activation time is present and save the same*/
    if(RRC_MSG_COMMON_BITMASK_IE(ho_ptr->u.r3.handoverToUTRANCommand_r3,
     rrc_HandoverToUTRANCommand_r3_IEs,dummy))
    {
      gtow_info.activation_time =
      ho_ptr->u.r3.handoverToUTRANCommand_r3.dummy;
    }

    /*Check if Complete Configuration is sent and set that t in the
      procedure data*/
    if(RRC_CHECK_COMMON_MSG_TYPE(ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode,
       rrc_HandoverToUTRANCommand_r3_IEs_specificationMode_complete ))
    {
      config_type = IHO_GTOW_COMPLETE_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - Rel99: complete configuration received");

      dl_info_per_rl = &ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.
         complete.dl_InformationPerRL_List;

      if(dl_info_per_rl->elem[0].modeSpecificInfo.t == T_rrc_DL_InformationPerRL_modeSpecificInfo_fdd)
      {
        /* get the primary scrambling code */
        scrambling_code_type = (uint16)(ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.
         complete.dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
          primaryCPICH_Info.primaryScramblingCode);

        freq_info = ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.complete.frequencyInfo.modeSpecificInfo.u.fdd;

        default_config_mode = rrc_DefaultConfigMode_fdd;

        /* Checking if closed loop timing adjacent mode is present */ 
        if((dl_info_per_rl->elem[0].m.dl_DPCH_InfoPerRLPresent) &&
            (dl_info_per_rl->elem[0].dl_DPCH_InfoPerRL.t == T_rrc_DL_DPCH_InfoPerRL_fdd) &&
            (dl_info_per_rl->elem[0].dl_DPCH_InfoPerRL.u.fdd->m.closedLoopTimingAdjModePresent))
        {
          closed_loop_info = TRUE;  
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
      }

      /*Check if Tx Diversity mode is given */
      dl_common_info = &ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.complete.dl_CommonInformation;	  

      if(((dl_common_info->modeSpecificInfo.t == T_rrc_DL_CommonInformation_modeSpecificInfo_fdd) &&
          (dl_common_info->modeSpecificInfo.u.fdd->m.tx_DiversityModePresent)) && (closed_loop_info))
      {
        rrciho_set_tx_dix_mode_info(dl_common_info->modeSpecificInfo.u.fdd->tx_DiversityMode);
      }
      else
      {
        /* If the IE is absent then set diversity_info_present to FALSE so that meas data from other
           RAT can be checked */
        gtow_info.diversity_info_present = FALSE;
        gtow_info.diversity_info = FALSE;
      }
    }
    else if(ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.preconfiguration.
            preConfigMode.t ==  T_rrc_HandoverToUTRANCommand_r3_IEs_preConfigMode_defaultConfig)
    {
      config_type = IHO_GTOW_DEFAULT_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - Rel99: default configuration received");

      default_config_mode = ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.preconfiguration.
                            preConfigMode.u.defaultConfig.defaultConfigMode;

      if(default_config_mode == rrc_DefaultConfigMode_fdd)
      {
        default_config_id = ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.preconfiguration.
          preConfigMode.u.defaultConfig.defaultConfigIdentity;

        if(ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.preconfiguration.modeSpecificInfo.t ==
			T_rrc_HandoverToUTRANCommand_r3_IEs_modeSpecificInfo_1_fdd)
        {
          /*lint -e740 */
          /* type-casting to R3 type as we are interested to see if this list has only one RL */
          dl_info_per_rl = (rrc_DL_InformationPerRL_List *)&ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.
          preconfiguration.modeSpecificInfo.u.fdd.dl_InformationPerRL_List;
          /*lint +e740 */

          /* get the primary scrambling code */
          scrambling_code_type = (uint16)(ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.
           preconfiguration.modeSpecificInfo.u.fdd.dl_InformationPerRL_List.elem[0].primaryCPICH_Info.
            primaryScramblingCode);

          /* Check if the UARFCN is supported */
          freq_info = &ho_ptr->u.r3.handoverToUTRANCommand_r3.specificationMode.u.preconfiguration.modeSpecificInfo.u.fdd.frequencyInfo;
        }
        else
        {
          default_config_mode = rrc_DefaultConfigMode_tdd;
          WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Default config info is for TDD mode ... rejecting ...");
      }
    }
  }
  else if((ho_ptr->t == T_rrc_HandoverToUTRANCommand_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.t == 
            T_rrc_HandoverToUTRANCommand_criticalExtensions_4_criticalExtensions)&&
          (ho_ptr->u.criticalExtensions.u.criticalExtensions.t == 
            T_rrc_HandoverToUTRANCommand_criticalExtensions_3_r5) &&
          (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
  {
    gtow_info.msg_rel_version = MSG_REL5;

    WRRC_MSG0_HIGH("RRCIHO - Rel5 message received");

#ifdef FEATURE_LTE_TO_WCDMA
    /* In case of PSHO from EUTRA, only process complete config */
    if((gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.t ==
       T_rrc_HandoverToUTRANCommand_r5_IEs_specificationMode_preconfiguration )
      && (gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.preconfiguration.preConfigMode.t ==
          T_rrc_HandoverToUTRANCommand_r5_IEs_preConfigMode_predefinedConfigIdentity )
      && ( iho_interrat_type == RRCIHO_INTERRAT_LTE))
    {
      WRRC_MSG0_HIGH("LTOW_PSHO:Predefined config ID present. Send failure to LRRC.");
      return FAILURE;
    }
#endif

    if(RRC_MSG_COMMON_BITMASK_IE(ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5,
		rrc_HandoverToUTRANCommand_r5_IEs,cipheringAlgorithm))
    {
      ciphering_algo_present = TRUE;
      /*Copy Ciphering Algo to local struct*/
      ciphering_algo = ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.cipheringAlgorithm;
    }

    /* Check if Complete Configuration is sent and set that t in the
      procedure data*/
    if(RRC_CHECK_COMMON_MSG_TYPE(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode,
       rrc_HandoverToUTRANCommand_r5_IEs_specificationMode_complete ))
    {
      config_type = IHO_GTOW_COMPLETE_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - complete configuration received");


      dl_info_per_rl_r5 = ( rrc_DL_InformationPerRL_List_r5 *) &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.complete.dl_InformationPerRL_List;

      if(RRC_CHECK_COMMON_MSG_TYPE(dl_info_per_rl_r5->elem[0].modeSpecificInfo,
	  	rrc_DL_InformationPerRL_r5_modeSpecificInfo_fdd))
      {
        /*get the primary scrambling code*/
        scrambling_code_type = (uint16)(ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.
           complete.dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
           primaryCPICH_Info.primaryScramblingCode);

        freq_info = ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.
                              u.complete.frequencyInfo.modeSpecificInfo.u.fdd;

        default_config_mode = rrc_DefaultConfigMode_fdd;

        /* Checking if closed loop timing adjacent mode is present */ 
        if((dl_info_per_rl_r5->elem[0].m.dl_DPCH_InfoPerRLPresent) &&
            (dl_info_per_rl_r5->elem[0].dl_DPCH_InfoPerRL.t == T_rrc_DL_DPCH_InfoPerRL_r5_fdd) &&
            (dl_info_per_rl_r5->elem[0].dl_DPCH_InfoPerRL.u.fdd->m.closedLoopTimingAdjModePresent))
        {
          closed_loop_info = TRUE;
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
      }

      /*Check if Tx Diversity mode is given */
      dl_common_info_r4 = &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.complete.dl_CommonInformation;

      if(((dl_common_info_r4->modeSpecificInfo.t == T_rrc_DL_CommonInformation_modeSpecificInfo_fdd) &&
          (dl_common_info_r4->modeSpecificInfo.u.fdd->m.tx_DiversityModePresent)) && (closed_loop_info))
      {
        rrciho_set_tx_dix_mode_info(dl_common_info_r4->modeSpecificInfo.u.fdd->tx_DiversityMode);
      }
      else
      {
        /* If the IE is absent then set diversity_info_present to FALSE so that meas data from other
           RAT can be checked */
        gtow_info.diversity_info_present = FALSE;
        gtow_info.diversity_info = FALSE;
      }
    }
    else if(ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.preconfiguration.
            preConfigMode.t == T_rrc_HandoverToUTRANCommand_r5_IEs_preConfigMode_defaultConfig)
    {
      config_type = IHO_GTOW_DEFAULT_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - Rel5: default configuration received");

      default_config_mode = ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.preconfiguration.
                            preConfigMode.u.defaultConfig.defaultConfigMode;

      if(default_config_mode == rrc_DefaultConfigMode_fdd)
      {
        default_config_id = ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.preconfiguration.
          preConfigMode.u.defaultConfig.defaultConfigIdentity;

        if(ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.
          preconfiguration.modeSpecificInfo.t == T_rrc_HandoverToUTRANCommand_r5_IEs_modeSpecificInfo_1_fdd)
        {

          dl_info_per_rl_r5 = ( rrc_DL_InformationPerRL_List_r5 *) &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.
            preconfiguration.modeSpecificInfo.u.fdd.dl_InformationPerRL_List;

          /* get the primary scrambling code */
          scrambling_code_type = (uint16)(ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.
            preconfiguration.modeSpecificInfo.u.fdd.dl_InformationPerRL_List.elem[0].primaryCPICH_Info.
            primaryScramblingCode);

          /* Check if the UARFCN is supported */
          freq_info = &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.r5.handoverToUTRANCommand_r5.specificationMode.u.
                  preconfiguration.modeSpecificInfo.u.fdd.frequencyInfo;
        }
        else
        {
          default_config_mode = rrc_DefaultConfigMode_tdd;
          WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Default config info is for TDD mode ... rejecting ...");
      }
    }
  }
  else if((ho_ptr->t == T_rrc_HandoverToUTRANCommand_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_4_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_3_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_2_r6) &&
          (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
  {
    gtow_info.msg_rel_version = MSG_REL6;

    WRRC_MSG0_HIGH("RRCIHO - Rel6 message received");

#ifdef FEATURE_LTE_TO_WCDMA
    /* In case of PSHO from EUTRA, only process complete config */
    if((gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.specificationMode.t ==
       T_rrc_HandoverToUTRANCommand_r6_IEs_specificationMode_preconfiguration )
      && (gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.
           specificationMode.u.preconfiguration.preConfigMode.t == T_rrc_HandoverToUTRANCommand_r6_IEs_preConfigMode_predefinedConfigIdentity)
      && ( iho_interrat_type == RRCIHO_INTERRAT_LTE))
    {
      WRRC_MSG0_HIGH("LTOW_PSHO:Predefined Config ID present. Send failure to LRRC.");
      return FAILURE;
    }
#endif

    if(RRC_MSG_COMMON_BITMASK_IE(ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6,
       rrc_HandoverToUTRANCommand_r6_IEs,cipheringAlgorithm))
    {
      ciphering_algo_present = TRUE;
      /*Copy Ciphering Algo to local struct*/
      ciphering_algo = 
        ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.cipheringAlgorithm;
    }

    /* Check if Complete Configuration is sent and set that t in the
      procedure data*/
    if(RRC_CHECK_COMMON_MSG_TYPE(gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.specificationMode,
       rrc_HandoverToUTRANCommand_r6_IEs_specificationMode_complete ))
    {
      config_type = IHO_GTOW_COMPLETE_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - complete configuration received");

      dl_info_per_rl_r6 = &ho_ptr->u.criticalExtensions.u.criticalExtensions.
        u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.specificationMode.u.complete.dl_InformationPerRL_List;

      if(dl_info_per_rl_r6->elem[0].modeSpecificInfo.t == T_rrc_DL_InformationPerRL_r6_modeSpecificInfo_fdd)
      {
        /*get the primary scrambling code*/
        scrambling_code_type = (uint16)(dl_info_per_rl_r6->elem[0].modeSpecificInfo.u.fdd->
           primaryCPICH_Info.primaryScramblingCode);

        freq_info = ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.
          specificationMode.u.complete.frequencyInfo.modeSpecificInfo.u.fdd;

        default_config_mode = rrc_DefaultConfigMode_fdd;

        /* Checking if closed loop timing adjacent mode is present */ 
        if((dl_info_per_rl_r6->elem[0].m.dl_dpchInfoPresent) &&
            (dl_info_per_rl_r6->elem[0].dl_dpchInfo.t == T_rrc_DL_InformationPerRL_r6_dl_dpchInfo_dl_DPCH_InfoPerRL) && 
            (dl_info_per_rl_r6->elem[0].dl_dpchInfo.u.dl_DPCH_InfoPerRL->t == T_rrc_DL_DPCH_InfoPerRL_r6_fdd) && 
            (dl_info_per_rl_r6->elem[0].dl_dpchInfo.u.dl_DPCH_InfoPerRL->u.fdd->m.closedLoopTimingAdjModePresent))
        {
          closed_loop_info = TRUE;  
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
      }

      /*Check if Tx Diversity mode is given */
      dl_common_info_r6 = 
        &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.specificationMode.u.complete.dl_CommonInformation;

      if(((dl_common_info_r6->modeSpecificInfo.t == T_rrc_DL_CommonInformation_modeSpecificInfo_fdd) &&
          (dl_common_info_r6->modeSpecificInfo.u.fdd->m.tx_DiversityModePresent)) && (closed_loop_info))
      {
        rrciho_set_tx_dix_mode_info(dl_common_info_r6->modeSpecificInfo.u.fdd->tx_DiversityMode);
      }
      else
      {
        /* If the IE is absent then set diversity_info_present to FALSE so that meas data from other
           RAT can be checked */
        gtow_info.diversity_info_present = FALSE;
        gtow_info.diversity_info = FALSE;
      }
    }
    else if(ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.
		specificationMode.u.preconfiguration.preConfigMode.t == 
            T_rrc_HandoverToUTRANCommand_r6_IEs_preConfigMode_defaultConfig)
    {
      config_type = IHO_GTOW_DEFAULT_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - Rel6: default configuration received");

      default_config_mode = ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
        handoverToUTRANCommand_r6.specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigMode;

      if(default_config_mode == rrc_DefaultConfigMode_fdd)
      {
        default_config_id = ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
          handoverToUTRANCommand_r6.specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity;

        if(ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.specificationMode.u.
           preconfiguration.modeSpecificInfo.t == T_rrc_HandoverToUTRANCommand_r6_IEs_modeSpecificInfo_fdd)
        {
          dl_info_per_rl_default_r6 = &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
            handoverToUTRANCommand_r6.specificationMode.u.preconfiguration.modeSpecificInfo.u.fdd.dl_InformationPerRL_List;

          /* get the primary scrambling code */
          scrambling_code_type = (uint16)(ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
                                          handoverToUTRANCommand_r6.specificationMode.u.preconfiguration.modeSpecificInfo.
                                          u.fdd.dl_InformationPerRL_List.elem[0].primaryCPICH_Info.primaryScramblingCode);

          /* Check if the UARFCN is supported */
          freq_info = &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.handoverToUTRANCommand_r6.specificationMode.u.
            preconfiguration.modeSpecificInfo.u.fdd.frequencyInfo;
        }
        else
        {
          default_config_mode = rrc_DefaultConfigMode_tdd;
          WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Default config info is for TDD mode ... rejecting ...");
      }
    }
  }
  else if((ho_ptr->t == T_rrc_HandoverToUTRANCommand_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_4_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_3_criticalExtensions) &&
            (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_2_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_1_r7) &&
          (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
  {
    rrc_HandoverToUTRANCommand_r7_IEs * ho_to_utran_ptr_r7;
    gtow_info.msg_rel_version = MSG_REL7;

    WRRC_MSG0_HIGH("RRCIHO - Rel7 message received");
    ho_to_utran_ptr_r7 = &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
        r7.handoverToUTRANCommand_r7;

#ifdef FEATURE_LTE_TO_WCDMA
    /* In case of PSHO from EUTRA, only process complete config */
    if((ho_to_utran_ptr_r7->specificationMode.t ==
        T_rrc_HandoverToUTRANCommand_r7_IEs_specificationMode_preconfiguration )
       && (ho_to_utran_ptr_r7->specificationMode.u.preconfiguration.preConfigMode.t == 
            T_rrc_HandoverToUTRANCommand_r7_IEs_preConfigMode_predefinedConfigIdentity)
       && ( iho_interrat_type == RRCIHO_INTERRAT_LTE))
    {
      WRRC_MSG0_HIGH("LTOW_PSHO:Predefined Config ID present. Send failure to LRRC.");
      return FAILURE;
    }
#endif

    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ho_to_utran_ptr_r7,
       rrc_HandoverToUTRANCommand_r7_IEs,cipheringAlgorithm))
    {
      ciphering_algo_present = TRUE;
      /*Copy Ciphering Algo to local struct*/
      ciphering_algo_r7 = 
        ho_to_utran_ptr_r7->cipheringAlgorithm;
    }

#ifdef FEATURE_WCDMA_REL7_SPECRS 
	
   /*In R7 IHO, IE "supportForChangeOfUE_Capability" mandatory. We need to store it in
        "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
        "rrc_rnc_cap_change_support" can be updated accordingly. */  
    if(ho_to_utran_ptr_r7->supportForChangeOfUE_Capability == TRUE)
    {
      rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_SUPPORTED;
    }
    else
    {
      rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_UNSUPPORTED;
    }

#endif
    /* Check if Complete Configuration is sent and set that t in the
      procedure data*/
    if(ho_to_utran_ptr_r7->specificationMode.t == 
       T_rrc_HandoverToUTRANCommand_r7_IEs_specificationMode_complete)
    {
      config_type = IHO_GTOW_COMPLETE_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - complete configuration received");

      dl_info_per_rl_r7 =&ho_to_utran_ptr_r7->specificationMode.u.complete.dl_InformationPerRL_List;

      if(dl_info_per_rl_r7->elem[0].modeSpecificInfo.t == T_rrc_DL_InformationPerRL_r7_modeSpecificInfo_fdd)
      {
        /*get the primary scrambling code*/
        scrambling_code_type = (uint16)(dl_info_per_rl_r7->elem[0].modeSpecificInfo.u.fdd->
           primaryCPICH_Info.primaryScramblingCode);

        freq_info = ho_to_utran_ptr_r7->specificationMode.u.complete.frequencyInfo.modeSpecificInfo.u.fdd;

        default_config_mode = rrc_DefaultConfigMode_fdd;

        if((dl_info_per_rl_r7->elem[0].m.dl_dpchInfoPresent) &&
            (dl_info_per_rl_r7->elem[0].dl_dpchInfo.t == T_rrc_DL_InformationPerRL_r7_dl_dpchInfo_dl_DPCH_InfoPerRL) && 
            (dl_info_per_rl_r7->elem[0].dl_dpchInfo.u.dl_DPCH_InfoPerRL->t == T_rrc_DL_DPCH_InfoPerRL_r7_fdd) && 
            (dl_info_per_rl_r7->elem[0].dl_dpchInfo.u.dl_DPCH_InfoPerRL->u.fdd->m.closedLoopTimingAdjModePresent))
        {
          closed_loop_info = TRUE;  
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
      }

      /* Check if tx_diversity_mode is present */
     dl_common_info_r7 =&ho_to_utran_ptr_r7->specificationMode.u.complete.dl_CommonInformation;

     if(((dl_common_info_r7->modeSpecificInfo.t == T_rrc_DL_CommonInformation_modeSpecificInfo_fdd) &&
         (dl_common_info_r7->modeSpecificInfo.u.fdd->m.tx_DiversityModePresent)) && (closed_loop_info))
     {
       rrciho_set_tx_dix_mode_info(dl_common_info_r7->modeSpecificInfo.u.fdd->tx_DiversityMode);
     }
     else
     {
       /* If the IE is absent then set diversity_info_present to FALSE so that meas data from other
          RAT can be checked */
       gtow_info.diversity_info_present = FALSE;
       gtow_info.diversity_info = FALSE;
     }
    }
    else if(ho_to_utran_ptr_r7->specificationMode.u.preconfiguration.preConfigMode.t == 
            T_rrc_HandoverToUTRANCommand_r7_IEs_preConfigMode_defaultConfig)
    {
      config_type = IHO_GTOW_DEFAULT_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - Rel7: default configuration received");

      default_config_mode = ho_to_utran_ptr_r7->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigMode;

      if(default_config_mode == rrc_DefaultConfigMode_fdd)
      {
        default_config_id = ho_to_utran_ptr_r7->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity;

        if(ho_to_utran_ptr_r7->specificationMode.u.preconfiguration.modeSpecificInfo.t
			== T_rrc_HandoverToUTRANCommand_r7_IEs_modeSpecificInfo_fdd)
        {
          dl_info_per_rl_default_r6 = &ho_to_utran_ptr_r7->specificationMode.u.preconfiguration.modeSpecificInfo.u.fdd.dl_InformationPerRL_List;

          /* get the primary scrambling code */
          scrambling_code_type = (uint16)( ho_to_utran_ptr_r7->specificationMode.u.preconfiguration.modeSpecificInfo.
                                          u.fdd.dl_InformationPerRL_List.elem[0].primaryCPICH_Info.primaryScramblingCode);

          /* Check if the UARFCN is supported */
          freq_info =  &ho_to_utran_ptr_r7->specificationMode.u.
            preconfiguration.modeSpecificInfo.u.fdd.frequencyInfo;
        }
        else
        {
          default_config_mode = rrc_DefaultConfigMode_tdd;
          WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Default config info is for TDD mode ... rejecting ...");
      }
    }
  }

  else if((ho_ptr->t == T_rrc_HandoverToUTRANCommand_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_4_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_3_criticalExtensions) &&
            (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_2_criticalExtensions) &&
          (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_1_criticalExtensions) &&
           (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_HandoverToUTRANCommand_criticalExtensions_r8) &&
          (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
  {
    rrc_HandoverToUTRANCommand_r8_IEs * ho_to_utran_ptr_r8;
    gtow_info.msg_rel_version = MSG_REL8;

    WRRC_MSG0_HIGH("RRCIHO - Rel8 message received");
    ho_to_utran_ptr_r8 = &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
        r8.handoverToUTRANCommand_r8;


#ifdef FEATURE_LTE_TO_WCDMA
    /* In case of PSHO from EUTRA, only process complete config */
    if((ho_to_utran_ptr_r8->specificationMode.t ==
        T_rrc_HandoverToUTRANCommand_r8_IEs_specificationMode_preconfiguration)
       && (ho_to_utran_ptr_r8->specificationMode.u.preconfiguration.preConfigMode.t == 
           T_rrc_HandoverToUTRANCommand_r8_IEs_preConfigMode_predefinedConfigIdentity)
       && ( iho_interrat_type == RRCIHO_INTERRAT_LTE))
    {
      WRRC_MSG0_HIGH("LTOW_PSHO:Predefined Config ID present. Send failure to LRRC.");
      return FAILURE;
    }
#endif

    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ho_to_utran_ptr_r8,
       rrc_HandoverToUTRANCommand_r8_IEs,cipheringAlgorithm))
    {
      ciphering_algo_present = TRUE;
      /*Copy Ciphering Algo to local struct*/
      ciphering_algo_r7 = 
        ho_to_utran_ptr_r8->cipheringAlgorithm;
    }

#ifdef FEATURE_WCDMA_REL7_SPECRS 
	
   /*In R7 IHO, IE "supportForChangeOfUE_Capability" mandatory. We need to store it in
        "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
        "rrc_rnc_cap_change_support" can be updated accordingly. */  
    if(ho_to_utran_ptr_r8->supportForChangeOfUE_Capability == TRUE)
    {
      rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_SUPPORTED;
    }
    else
    {
      rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_UNSUPPORTED;
    }

#endif
    /* Check if Complete Configuration is sent and set that t in the
      procedure data*/
    if(RRC_CHECK_COMMON_MSG_TYPE(ho_to_utran_ptr_r8->specificationMode,
       rrc_HandoverToUTRANCommand_r8_IEs_specificationMode_complete))
    {
      config_type = IHO_GTOW_COMPLETE_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - complete configuration received");

      dl_info_per_rl_r7 =&ho_to_utran_ptr_r8->specificationMode.u.complete.dl_InformationPerRL_List;

      if(dl_info_per_rl_r7->elem[0].modeSpecificInfo.t == T_rrc_DL_InformationPerRL_r7_modeSpecificInfo_fdd)
      {
        /*get the primary scrambling code*/
        scrambling_code_type = (uint16)(dl_info_per_rl_r7->elem[0].modeSpecificInfo.u.fdd->
           primaryCPICH_Info.primaryScramblingCode);

        freq_info = ho_to_utran_ptr_r8->specificationMode.u.complete.frequencyInfo.modeSpecificInfo.u.fdd;

        default_config_mode = rrc_DefaultConfigMode_fdd;

        if((dl_info_per_rl_r7->elem[0].m.dl_dpchInfoPresent) &&
            (dl_info_per_rl_r7->elem[0].dl_dpchInfo.t == T_rrc_DL_InformationPerRL_r7_dl_dpchInfo_dl_DPCH_InfoPerRL) && 
            (dl_info_per_rl_r7->elem[0].dl_dpchInfo.u.dl_DPCH_InfoPerRL->t == T_rrc_DL_DPCH_InfoPerRL_r7_fdd) && 
            (dl_info_per_rl_r7->elem[0].dl_dpchInfo.u.dl_DPCH_InfoPerRL->u.fdd->m.closedLoopTimingAdjModePresent))
        {
          closed_loop_info = TRUE;  
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
      }

      /* Check if tx_diversity_mode is present */
      dl_common_info_r8 =&ho_to_utran_ptr_r8->specificationMode.u.complete.dl_CommonInformation;

      if(((dl_common_info_r8->modeSpecificInfo.t == T_rrc_DL_CommonInformation_modeSpecificInfo_fdd) &&
          (dl_common_info_r8->modeSpecificInfo.u.fdd->m.tx_DiversityModePresent)) && (closed_loop_info))
      {
        rrciho_set_tx_dix_mode_info(dl_common_info_r8->modeSpecificInfo.u.fdd->tx_DiversityMode);
      }
      else
      {
        /* If the IE is absent then set diversity_info_present to FALSE so that meas data from other
           RAT can be checked */
        gtow_info.diversity_info_present = FALSE;
        gtow_info.diversity_info = FALSE;
      }
    }
    else if(ho_to_utran_ptr_r8->specificationMode.u.preconfiguration.preConfigMode.t == 
            T_rrc_HandoverToUTRANCommand_r8_IEs_preConfigMode_defaultConfig)
    {
      config_type = IHO_GTOW_DEFAULT_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - Rel8: default configuration received");

      default_config_mode = ho_to_utran_ptr_r8->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigMode;

      if(default_config_mode == rrc_DefaultConfigMode_fdd)
      {
        default_config_id = ho_to_utran_ptr_r8->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity;

        if(ho_to_utran_ptr_r8->specificationMode.u.
			preconfiguration.modeSpecificInfo.t == T_rrc_HandoverToUTRANCommand_r8_IEs_modeSpecificInfo_fdd)
        {
          dl_info_per_rl_default_r6 = &ho_to_utran_ptr_r8->specificationMode.u.preconfiguration.modeSpecificInfo.u.fdd.dl_InformationPerRL_List;

          /* get the primary scrambling code */
          scrambling_code_type = (uint16)( ho_to_utran_ptr_r8->specificationMode.u.preconfiguration.modeSpecificInfo.
                                          u.fdd.dl_InformationPerRL_List.elem[0].primaryCPICH_Info.primaryScramblingCode);

          /* Check if the UARFCN is supported */
          freq_info =  &ho_to_utran_ptr_r8->specificationMode.u.
            preconfiguration.modeSpecificInfo.u.fdd.frequencyInfo;
        }
        else
        {
          default_config_mode = rrc_DefaultConfigMode_tdd;
          WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Default config info is for TDD mode ... rejecting ...");
      }
    }
  }
#ifdef FEATURE_WCDMA_REL9
  else if((ho_ptr->t == T_rrc_HandoverToUTRANCommand_criticalExtensions) &&
            (ho_ptr->u.criticalExtensions.t == 
             T_rrc_HandoverToUTRANCommand_criticalExtensions_4_criticalExtensions) &&
            (ho_ptr->u.criticalExtensions.u.criticalExtensions.t == 
             T_rrc_HandoverToUTRANCommand_criticalExtensions_3_criticalExtensions) &&
              (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == 
             T_rrc_HandoverToUTRANCommand_criticalExtensions_2_criticalExtensions) &&
            (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == 
             T_rrc_HandoverToUTRANCommand_criticalExtensions_1_criticalExtensions) &&
             (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == 
             T_rrc_HandoverToUTRANCommand_criticalExtensions_later_r8_criticalExtensions) &&
            (ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.t == 
             T_rrc_HandoverToUTRANCommand_later_r8_criticalExtensions_r9) &&
            (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
  {
    rrc_HandoverToUTRANCommand_r9_IEs * ho_to_utran_ptr_r9;
    gtow_info.msg_rel_version = MSG_REL9;

    WRRC_MSG0_HIGH("RRCIHO - Rel9 message received");
    ho_to_utran_ptr_r9 = &ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
        later_r8_criticalExtensions.u.r9.handoverToUTRANCommand_r9;


#ifdef FEATURE_LTE_TO_WCDMA
    /* In case of PSHO from EUTRA, only process complete config */
    if((ho_to_utran_ptr_r9->specificationMode.t ==
        T_rrc_HandoverToUTRANCommand_r9_IEs_specificationMode_preconfiguration)
       && (ho_to_utran_ptr_r9->specificationMode.u.preconfiguration.preConfigMode.t == 
           T_rrc_HandoverToUTRANCommand_r9_IEs_preConfigMode_predefinedConfigIdentity)
           && ( iho_interrat_type == RRCIHO_INTERRAT_LTE))
    {
      WRRC_MSG0_HIGH("LTOW_PSHO:Predefined Config ID present. Send failure to LRRC.");
      return FAILURE;
    }
#endif

    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ho_to_utran_ptr_r9,
       rrc_HandoverToUTRANCommand_r9_IEs,cipheringAlgorithm))
    {
      ciphering_algo_present = TRUE;
      /*Copy Ciphering Algo to local struct*/
      ciphering_algo_r7 = 
        ho_to_utran_ptr_r9->cipheringAlgorithm;
    }

#ifdef FEATURE_WCDMA_REL7_SPECRS 
  
   /*In R7 IHO, IE "supportForChangeOfUE_Capability" mandatory. We need to store it in
        "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
        "rrc_rnc_cap_change_support" can be updated accordingly. */  
    if(ho_to_utran_ptr_r9->supportForChangeOfUE_Capability == TRUE)
    {
      rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_SUPPORTED;
    }
    else
    {
      rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_UNSUPPORTED;
    }

#endif
    /* Check if Complete Configuration is sent and set that t in the
      procedure data*/
    if(RRC_CHECK_COMMON_MSG_TYPE(ho_to_utran_ptr_r9->specificationMode,
       rrc_HandoverToUTRANCommand_r9_IEs_specificationMode_complete))
    {
      config_type = IHO_GTOW_COMPLETE_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - complete configuration received");

      dl_info_per_rl_r7 =&ho_to_utran_ptr_r9->specificationMode.u.complete.dl_InformationPerRL_List;

      if(dl_info_per_rl_r7->elem[0].modeSpecificInfo.t == T_rrc_DL_InformationPerRL_r7_modeSpecificInfo_fdd)
      {
        /*get the primary scrambling code*/
        scrambling_code_type = (uint16)(dl_info_per_rl_r7->elem[0].modeSpecificInfo.u.fdd->
           primaryCPICH_Info.primaryScramblingCode);

        freq_info = ho_to_utran_ptr_r9->specificationMode.u.complete.frequencyInfo.modeSpecificInfo.u.fdd;

        default_config_mode = rrc_DefaultConfigMode_fdd;
      }
      else
      {
        WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
      }
    }
    else if(ho_to_utran_ptr_r9->specificationMode.u.preconfiguration.preConfigMode.t == 
            T_rrc_HandoverToUTRANCommand_r9_IEs_preConfigMode_defaultConfig)
    {
      config_type = IHO_GTOW_DEFAULT_CONFIG;

      WRRC_MSG0_HIGH("RRCIHO - Rel9: default configuration received");

      default_config_mode = ho_to_utran_ptr_r9->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigMode;

      if(default_config_mode == rrc_DefaultConfigMode_fdd)
      {
        default_config_id = ho_to_utran_ptr_r9->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity;

        if(ho_to_utran_ptr_r9->specificationMode.u.
      preconfiguration.modeSpecificInfo.t == T_rrc_HandoverToUTRANCommand_r9_IEs_modeSpecificInfo_fdd)
        {
          dl_info_per_rl_default_r6 = &ho_to_utran_ptr_r9->specificationMode.u.preconfiguration.modeSpecificInfo.u.fdd.dl_InformationPerRL_List;

          /* get the primary scrambling code */
          scrambling_code_type = (uint16)( ho_to_utran_ptr_r9->specificationMode.u.preconfiguration.modeSpecificInfo.
                                          u.fdd.dl_InformationPerRL_List.elem[0].primaryCPICH_Info.primaryScramblingCode);

          /* Check if the UARFCN is supported */
          freq_info =  &ho_to_utran_ptr_r9->specificationMode.u.
            preconfiguration.modeSpecificInfo.u.fdd.frequencyInfo;
        }
        else
        {
          default_config_mode = rrc_DefaultConfigMode_tdd;
          WRRC_MSG0_ERROR("Mode specific info is for TDD mode ... rejecting ...");
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Default config info is for TDD mode ... rejecting ...");
      }
    }
  }
#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */


  else
  {
    rrc_print_supported_asn1_rel_version();
    procedure_info.failure_cause = (uint32)RR_RRC_WCDMA_PROTOCOL_ERROR;
    WRRC_MSG0_HIGH("Unsupported message version ... rejecting ...");
    return FAILURE;
  }

  if(config_type == IHO_GTOW_PREDEF_CONFIG)
  {
    /* Preconfiguration mode - predefined not supported */
    procedure_info.failure_cause =   (uint32)RR_RRC_INVALID_CONFIGURATION;
    WRRC_MSG0_ERROR("Preconfiguration mode - predefined not supported");
    return FAILURE;
  }

  if(default_config_mode == rrc_DefaultConfigMode_fdd)
  {
    if(config_type == IHO_GTOW_DEFAULT_CONFIG)
    {
      switch(default_config_id)
      {
        /* Fall through cases that have to processed */
      case RRC_DEF_CONFIG_0:
      case RRC_DEF_CONFIG_1:
      case RRC_DEF_CONFIG_3:
      case RRC_DEF_CONFIG_4:
      case RRC_DEF_CONFIG_7:
      case RRC_DEF_CONFIG_8:
      case RRC_DEF_CONFIG_9:
      case RRC_DEF_CONFIG_11:
#ifdef FEATURE_VOC_AMR_WB
      case  RRC_DEF_CONFIG_13:
#endif/*FEATURE_VOC_AMR_WB*/
      case RRC_DEF_CONFIG_12:
      case RRC_DEF_CONFIG_14:
      case RRC_DEF_CONFIG_10:  
#ifdef FEATURE_WCDMA_REL10
      #error code not present
#endif
        gtow_info.default_config_id = default_config_id;
        WRRC_MSG1_HIGH("RRCIHO: Config ID is - %d", default_config_id);
        break;
      /* default config 22,15,18,19 is valid only for release 6  */
      case RRC_DEF_CONFIG_15:
      case RRC_DEF_CONFIG_18:
      case RRC_DEF_CONFIG_19:
      case RRC_DEF_CONFIG_22:
#ifdef FEATURE_VOC_AMR_WB
      case RRC_DEF_CONFIG_21:
      case RRC_DEF_CONFIG_16:
#endif/*FEATURE_VOC_AMR_WB*/
        if( rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6 )
        {
          gtow_info.default_config_id = default_config_id;
          WRRC_MSG1_HIGH("RRCIHO: Config ID is - %d", default_config_id);
        }
        else
        {
          WRRC_MSG1_ERROR("Unsupported Def Config ID %d", default_config_id);
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
          return FAILURE;
        }
        
        break;

      default:
        WRRC_MSG1_ERROR("Unsupported Def Config ID %d", default_config_id);
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
        return FAILURE;
      }
    }
  }
  else
  {
    procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
    return FAILURE;
  }

  /*we only support single cell handover from GtoW.. If Utran wants us to do
    Soft-Handover as we Handover from GtoW we error out and  return failure*/
  switch(gtow_info.msg_rel_version )
  {
    case MSG_REL5:
      if((dl_info_per_rl_r5 == NULL) || (dl_info_per_rl_r5->n != 1))
      {
        WRRC_MSG0_ERROR("Single Pri-SCR supported for GtoW HO");
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
        return FAILURE;
      }
      break;
      
    case MSG_REL6:
      if(((config_type == IHO_GTOW_COMPLETE_CONFIG) &&
       ((dl_info_per_rl_r6 == NULL) || (dl_info_per_rl_r6->n != 1)) )
        || ((config_type == IHO_GTOW_DEFAULT_CONFIG) &&
       ((dl_info_per_rl_default_r6 == NULL) || (dl_info_per_rl_default_r6->n != 1)) ))
      {
        WRRC_MSG0_ERROR("Single Pri-SCR supported for GtoW HO");
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
        return FAILURE;
      }
      break;
      
    case MSG_REL7:
    case MSG_REL8:
#ifdef FEATURE_WCDMA_REL9
    case MSG_REL9:
#endif
#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif

      if(((config_type == IHO_GTOW_COMPLETE_CONFIG) &&
       ((dl_info_per_rl_r7 == NULL) || (dl_info_per_rl_r7->n != 1)) )
        || ((config_type == IHO_GTOW_DEFAULT_CONFIG) &&
       ((dl_info_per_rl_default_r6 == NULL) || (dl_info_per_rl_default_r6->n != 1)) ))
      {
        WRRC_MSG0_ERROR("Single Pri-SCR supported for GtoW HO");
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
        return FAILURE;
      }
      break;
      
    case MSG_REL99:
      if((dl_info_per_rl == NULL) || (dl_info_per_rl->n != 1))
      {
        WRRC_MSG0_ERROR("Single Pri-SCR supported for GtoW HO");
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
        return FAILURE;
      }
      break;
      
    default:
      WRRC_MSG1_HIGH("Invalid message version, return FAILURE",gtow_info.msg_rel_version);
      return FAILURE;
  }

#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
  if(validate_fdd_frequency(freq_info, plmn_id) == FAILURE)
#else
  if(validate_fdd_frequency(freq_info) == FAILURE)
#endif
  {
    procedure_info.failure_cause = (uint32)RR_RRC_FREQUENCY_NOT_IMPLEMENTED;
    WRRC_MSG0_ERROR("Unsupported frequency band");
    return FAILURE;
  }
  else
  {
    /*get the UARFCN on which to do Acquisition*/
    gtow_info.freq = (uint16)freq_info->uarfcn_DL;
  }

  if(ciphering_algo_present)
  {
    gtow_info.ciph_algo_present  = ciphering_algo_present;
    if((gtow_info.msg_rel_version == MSG_REL7)
     || (gtow_info.msg_rel_version == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
     || (gtow_info.msg_rel_version == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
     #error code not present
#endif /* FEATURE_WCDMA_REL10 */
    )
    {
      gtow_info.cipheringAlgorithm = rrcsmc_get_int_cipher_algo_from_dl_sdu_ind(
                                              &ciphering_algo_r7,
                                              MSG_REL7);
    }
    else
    {
      gtow_info.cipheringAlgorithm = rrcsmc_get_int_cipher_algo_from_dl_sdu_ind(
                                              &ciphering_algo,
                                              MSG_REL99);
    }
  }
  else
  {
    WRRC_MSG0_HIGH("I.E ciph_algo not present");
    gtow_info.ciph_algo_present  = FALSE;
  }

  gtow_info.config_type = config_type;
  gtow_info.scr_code    = scrambling_code_type;

  /* check if we are successful in extracting the SRB and RB information */
  return rrciho_validate_and_extract_srb_rab_info();
} /* end of verify_and_extract_handover_to_utran_command */

/*===========================================================================

FUNCTION  process_meas_data_for_handover_to_utran_command

DESCRIPTION

  This function processes measurement data received in
  RRC_INTERRAT_HANDOVER_REQ sent by RR. Measurement data contains
  PN_OFFSET and DIVERSITY status of WCDMA cells as measured by
  GSM RAT

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
static void process_meas_data_for_handover_to_utran_command
(
  rr_rrc_interrat_measurement_data_type * meas_data
)
{
  uint32 count=0;                            /* for count in for loop*/

  for(count = 0;
      count < meas_data->number_of_cells;
      count ++)
  {
    /*if freq and scrambling code matches then measurement for the cell to which
      we have to handover is known.. copy pn_offset and diversity information*/
    if((meas_data->wcdma_cell_measurements[count].primary_scrambling_code == gtow_info.scr_code) &&
       (meas_data->wcdma_cell_measurements[count].uarfcn == gtow_info.freq))
    {
      /*check if pn offset is not unknown, if not then copy the same to gtow
        database*/
      if(meas_data->wcdma_cell_measurements[count].PN_Offset != PN_OFFSET_UNKNOWN)
      {
        gtow_info.pn_offset_present = TRUE;
        gtow_info.pn_offset = meas_data->wcdma_cell_measurements[count].PN_Offset;
      }
      else
      {
        gtow_info.pn_offset_present = FALSE;
      }

#ifdef FEATURE_LTE_TO_WCDMA
      if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
      {
        /* If diversity_info_present is FALSE then this IE is absent in HO message
         so follow meas data received from the other RAT */
        if(FALSE == gtow_info.diversity_info_present)
        {
          /*check to see if diversity info is not OFF*/
          if(meas_data->wcdma_cell_measurements[count].diversity != DIVERSITY_UNKNOWN)
          {
            /*check if diversity info is known or not.. if it is not then
              set it to false*/
            gtow_info.diversity_info_present = TRUE;
        
            /*check if diversity info is known or not.. if it is not then
                   set it to false*/
            if(meas_data->wcdma_cell_measurements[count].diversity == DIVERSITY_ON)
            {
              gtow_info.diversity_info = TRUE;
            }
            else
            {
              /*if diversity info is OFF then set the flag to false*/
              gtow_info.diversity_info = FALSE;
            }
          }
        }
      }
      else
#endif
      /* If diversity_info_present is FALSE then this IE is absent in HO message
       so follow meas data received from the other RAT */
      if(FALSE == gtow_info.diversity_info_present)
      {
        /*check to see if diversity info is not OFF*/
        if(meas_data->wcdma_cell_measurements[count].diversity != DIVERSITY_OFF)
        {
          /*check if diversity info is known or not.. if it is not then
            set it to false*/
          gtow_info.diversity_info_present = TRUE;
 
          /*check if diversity info is known or not.. if it is not then
            set it to false*/
          if(meas_data->wcdma_cell_measurements[count].diversity == DIVERSITY_ON)
          {
            gtow_info.diversity_info = TRUE;
          }
          else
          {
            /*if diversity info is OFF then set the flag to false*/
            gtow_info.diversity_info = FALSE;
          }
        }
      }
    }
  }
}

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION   rrciho_send_srvcc_handover_failure_ind

DESCRIPTION

  This function sends SRVCC HO failure indication to NAS. So that IMS can send re-INVITE to NW and resume
  IMS call on LTE.
  This SRVCC failure indication is a requirement from upper layers, and SRVCC success will be derived by
  NAS from RRC_ACTIVATION_IND and no seperate indication is needed.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_srvcc_handover_failure_ind(void)
{

  mm_cmd_type *mm_buf_ptr;

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_SRVCC_HANDOVER_FAILURE_IND);
#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_srvcc_handover_failure_ind.as_id = rrc_get_as_id();
#endif 
  /* Put the command on MM queue */
  WRRC_MSG0_HIGH("LTOW: Sending MM RRC_SRVCC_HANDOVER_FAILURE_IND to NAS / Dispatching out MM cmd");
  rrc_send_rrc_message_to_mm(mm_buf_ptr);
}

/*===========================================================================

FUNCTION   rrciho_send_ltow_ps_handover_to_utran_success

DESCRIPTION

  This function sends a HO success to UTRAN as well as a HO success indication
  to RR.  We should have successfully done Acquisition on WCDMA side and
  configured WCDMA lower layers to go to DCH state.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_ltow_ps_handover_to_utran_success(void)
{
  /*declare var of struct handover confirm type*/
  wcdma_rrc_lte_psho_rsp_type ltow_ho_cnf;

  /*delete all the stored handover to utran IE's as handover is successful*/
  rrciho_delete_stored_gtow_info();

  /*initialize the HO parameters to default*/
  rrciho_init_data();

  /*set HO status to success*/
  ltow_ho_cnf.handover_status = LTOW_PSHO_SUCCESS;

  rrc_send_lte_cmd(&ltow_ho_cnf.msg_hdr,WCDMA_RRC_LTE_PSHO_RSP,sizeof(wcdma_rrc_lte_psho_rsp_type));
  WRRC_MSG0_HIGH("LTOW: WCDMA_RRC_LTE_PSHO_RSP success sent to LTE RRC");

#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM    
    if(gtow_info.is_cs_rab_present)
    {
      rrc_send_hho_cmd_to_cm(SYS_HO_COMPLETE,SYS_HO_TYPE_LTE_UMTS_SRVCC);      
    }
#endif
  return;
}/*rrciho_send_ltow_ps_handover_to_utran_success*/

/*===========================================================================

FUNCTION   rrciho_send_handover_to_utran_failure

DESCRIPTION

  This function sends handover to UTRAN failure to RR.  We could have failed
  Handover due to ACQ failure, Channel Config failure or due to HO_ABORT_REQ
  from RR

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_ltow_ps_handover_to_utran_failure(void)
{
  /*declare var of struct handover confirm type*/
  wcdma_rrc_lte_psho_rsp_type ltow_ho_cnf;

  /*delete all the stored handover to utran IE's as handover is successful*/
  rrciho_delete_stored_gtow_info();

  /*initialize the HO parameters to default*/
  rrciho_init_data();

  /*set HO status to success*/
  ltow_ho_cnf.handover_status = LTOW_PSHO_FAILURE;

  switch( procedure_info.failure_cause)
  {
    case 0: ltow_ho_cnf.failure_cause = LTOW_PSHO_INVALID_STATE;
            break;
    case 1: ltow_ho_cnf.failure_cause = LTOW_PSHO_WCDMA_PROTOCOL_ERROR;
      break;
    case 2: ltow_ho_cnf.failure_cause = LTOW_PSHO_INVALID_CONFIGURATION;
      break;
    case 3: ltow_ho_cnf.failure_cause = LTOW_PSHO_PHYSICAL_CHANNEL_FAILURE;
      break;
    case 4: ltow_ho_cnf.failure_cause = LTOW_PSHO_INVALID_CIPHER_ALGORITHM;
      break;
    case 5: ltow_ho_cnf.failure_cause = LTOW_PSHO_FREQUENCY_NOT_IMPLEMENTED;
      break;
    case 6: ltow_ho_cnf.failure_cause = LTOW_PSHO_MAX_FAILURE_CAUSE;
      break;
    default: /* no action */
      break;
  }

#ifdef FEATURE_DUAL_SIM
  /* CSFB call is a failure hence the lock needs to be exchanged with IRAT client if TRM exchange
     was success earlier */
  rrc_check_and_perform_trm_exchange();

  /* Lock has already been exchanged so releasing the lock will not affect anything 
     This is just done to update WRM variables */
  rrc_release_lock();
#endif


  rrc_send_lte_cmd(&ltow_ho_cnf.msg_hdr,WCDMA_RRC_LTE_PSHO_RSP,sizeof(wcdma_rrc_lte_psho_rsp_type));
  WRRC_MSG0_HIGH("LTOW: WCDMA_RRC_LTE_PSHO_RSP failure sent to LTE RRC");

  if(gtow_info.is_cs_rab_present)
  {
    /* Send SRVCC failure indication to NAS, so that IMS call can resume on LTE
       This SRVCC failure indication is a requirement from upper layers, and SRVCC success will 
       be derived by NAS from RRC_ACTIVATION_IND and no seperate indication is needed. */
    rrciho_send_srvcc_handover_failure_ind();

#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
    rrc_send_hho_cmd_to_cm(SYS_HO_FAIL,SYS_HO_TYPE_LTE_UMTS_SRVCC);      
#endif

  }

  return;
}/*rrciho_send_handover_to_utran_failure*/

/*===========================================================================

FUNCTION   rrciho_send_ltow_ps_handover_to_utran_abort_cnf

DESCRIPTION

  This function sends handover to UTRAN Abort Cnf to RR.  This msg is in
  response  to HO_ABORT_REQ from RR

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_ltow_ps_handover_to_utran_abort_cnf(void)
{
  /*declare var of struct handover confirm type*/
  wcdma_rrc_lte_abort_psho_rsp_type ltow_abort_ho_cnf;

  /*delete all the stored handover to utran IE's as handover is successful*/
  rrciho_delete_stored_gtow_info();

  /*initialize the HO parameters to default*/
  rrciho_init_data();

#ifdef FEATURE_DUAL_SIM
  /* CSFB call is a failure hence the lock needs to be exchanged with IRAT client if TRM exchange
     was success earlier */
  rrc_check_and_perform_trm_exchange();
  
  /* Lock has already been exchanged so releasing the lock will not affect anything 
     This is just done to update WRM variables */
  rrc_release_lock();
#endif


  rrc_send_lte_cmd(&ltow_abort_ho_cnf.msg_hdr,WCDMA_RRC_LTE_ABORT_PSHO_RSP,sizeof(wcdma_rrc_lte_abort_psho_rsp_type));
  WRRC_MSG0_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_PSHO_RSP sent to LTE RRC");

  if(gtow_info.is_cs_rab_present)
  {
    /* Send SRVCC failure indication to NAS, so that IMS call can resume on LTE
       This SRVCC failure indication is a requirement from upper layers, and SRVCC success will 
       be derived by NAS from RRC_ACTIVATION_IND and no seperate indication is needed. */
    rrciho_send_srvcc_handover_failure_ind();
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
    rrc_send_hho_cmd_to_cm(SYS_HO_FAIL,SYS_HO_TYPE_LTE_UMTS_SRVCC);
#endif
    
  }

  return;
}/*rrciho_send_ltow_ps_handover_to_utran_abort_cnf*/
#endif

/*===========================================================================

FUNCTION   rrciho_process_channel_config_cnf

DESCRIPTION

  This function does post processing after Channel Config Cnf is received.
  Sets up the vocoder

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_VOC_AMR_WB
static void rrciho_process_channel_config_cnf(mvs_mode_type current_mode)
#else /*FEATURE_VOC_AMR_WB*/
static void rrciho_process_channel_config_cnf(void)
#endif /*FEATURE_VOC_AMR_WB*/
{

  rrcllc_amr_mapping_info_type amr_info = {0}; /* To store AMR mode
                                            info incase a voice RAB has
                                            been setup */
  uint8 voice_rab_idx = 0;

  for(voice_rab_idx=0; voice_rab_idx<MAX_RAB_TO_SETUP; voice_rab_idx++)
  {
    if(rrc_est_rabs.rabs[voice_rab_idx].rab_type == RRCRB_CS_VOICE_RAB)
    {
      break; /* Since a voice RAB has been found; no need to iterate */
    }
  }

#ifdef FEATURE_WCDMA_REL7_SPECRS 
  rrc_save_rnc_cap_change_support();
#endif
  /* We need to indicate to the higher layers that a voice RAB has been setup. */
#ifdef FEATURE_VOC_AMR_WB
  /* First check what is the current codec that needs to be set */
  rrc_set_current_codec_mode(current_mode);
  
  if( current_mode == MVS_MODE_AMR_WB)
  {
    /* Configure the codec for WB AMR */
    amr_info.mode_type = MVS_MODE_AMR_WB;
    (void) rrcllc_get_amr_wb_mapping(&(amr_info));
    rrc_configure_amr_wb_mode_param(&(amr_info));
  }
  else
  {
    amr_info.mode_type = MVS_MODE_AMR;
    /*Now MVS needs to be configured. The structure amr_info will be filled by this function. */
    (void)rrcllc_get_amr_mapping(&(amr_info));
    /*Configure MVS to register RLC queues for AMR Call.*/
    rrc_configure_mvs(&amr_info, voice_rab_idx);
  }
#else /* FEATURE_VOC_AMR_WB */

  /*Now MVS needs to be configured. The structure amr_info will be filled by this function. */
  (void)rrcllc_get_amr_mapping(&(amr_info));
  /*Configure MVS to register RLC queues for AMR Call.*/
  rrc_configure_mvs(&amr_info, voice_rab_idx);

#endif/*FEATURE_VOC_AMR_WB*/  

  return;
}
#ifdef FEATURE_INTER_RAT_HO_OPT

/*===========================================================================
FUNCTION   rrciho_cs_voice_rab_released()

DESCRIPTION

  This is to inform IHO procedure that a cs voice rab has been released

DEPENDENCIES

  None.

RETURN VALUE

  None

SIDE EFFECTS

  None.
===========================================================================*/
void rrciho_cs_voice_rab_released(void)
{
  if(rrciho_mvs_control_flag == TRUE)
  {
    WRRC_MSG0_HIGH("Releasing MVS Client(W) during a CS Voice RAB release. CS Voice RAB release notification.Setting mvs_control_flag to FALSE");
    ghdi_mvs_release(MVS_CLIENT_WCDMA);

    /* Reset the vocoder control flag */
    rrciho_mvs_control_flag = FALSE;
  }
}
#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION   rrc_vocoder_control_enabled_sub()

DESCRIPTION

  Interface to let other procedures know that rrc is controlling the vocoder if W has the as id 
  same as the as_id passed by NAS

DEPENDENCIES

  None.

RETURN VALUE

  TRUE if rrciho is controlling the vocoder if as id matches,FALSE otherwise.

SIDE EFFECTS

  None.
 ===========================================================================*/
boolean rrc_vocoder_control_enabled_sub(sys_modem_as_id_e_type as_id)
{
sys_modem_as_id_e_type w_as_id= rrc_get_as_id();

  if(w_as_id== as_id)
  {
    return rrciho_mvs_control_flag;
  }
  else
  {
    WRRC_MSG2_HIGH("w_as_id %d as_id passed by NAS %d ",w_as_id,as_id);
    return FALSE;
  }
}
#endif
/*===========================================================================
FUNCTION   rrc_vocoder_control_enabled()

DESCRIPTION

  Interface to let other procedures know that rrc is controlling the vocoder

DEPENDENCIES

  None.

RETURN VALUE

  TRUE if rrciho is controlling the vocoder,FALSE otherwise.

SIDE EFFECTS

  None.
 ===========================================================================*/
boolean rrc_vocoder_control_enabled(void)
{
  return rrciho_mvs_control_flag;
}

/*===========================================================================
FUNCTION   rrc_vocoder_acquired()

DESCRIPTION

  This is a callback function for NAS when the mvs_acquire commmand issued
  by RRC was succesful

DEPENDENCIES

  None.

RETURN VALUE

  None

SIDE EFFECTS
  The expectation is that while calling this function, the global variables "rrc_cs_over_hs_status" and 
  "rrc_active_codec" should have been updated properly. For FEATURE_CS_VOICE_OVER_HSPA.
  None.
 ===========================================================================*/
void rrc_vocoder_acquired(void)
{
  WRRC_MSG0_HIGH ("mvs_acquire success callback from NAS. Enabling MVS_CLIENT_WCDMA");

  /* Set the vocoder acquired flag to true */
  rrciho_mvs_acquired_flag = TRUE ;
  
  MSG_LOW ("rrciho_mvs_acquired_flag %d ",rrciho_mvs_acquired_flag,0,0);

  /* Enable the vocoder since we have got the acqurie confirm */
#ifdef FEATURE_VOC_AMR_WB
  if(rrc_get_current_codec_mode() == MVS_MODE_AMR_WB)
  {
    ghdi_mvs_enable(MVS_CLIENT_WCDMA,(mvs_mode_type)MVS_MODE_AMR_WB);
  }
  else
#endif /*FEATURE_VOC_AMR_WB*/
  {
    ghdi_mvs_enable(MVS_CLIENT_WCDMA,(mvs_mode_type)MVS_MODE_AMR);
  }

  /* Reset the vocoder acquired flag */
  rrciho_mvs_acquired_flag = FALSE ;
}

#endif /* FEATURE_INTER_RAT_HO_OPT */
/*===========================================================================

FUNCTION   rrciho_get_sib16_ptr

DESCRIPTION

  This function gets the Sib16 ptr either from the actual sib16 or the
  SIB16 version of Default Configuration

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
struct rrc_SysInfoType16 * rrciho_get_sib16_ptr()
{

  /*if the config type is complete, then we don't need SIB16 ptr*/
  if(gtow_info.config_type == IHO_GTOW_COMPLETE_CONFIG)
  {
    WRRC_MSG0_ERROR("No SIB16 Info reqd.");
  }

  /*return the ptr to the sib16*/
  return gtow_info.sib16_ptr;
}

/*===========================================================================

FUNCTION   rrciho_get_rbs_ptr

DESCRIPTION

  This function gets the RBS ptr either from the actual RBS or the
  RBS version of Default Configuration

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
void * rrciho_get_rbs_ptr
(  
  uint32 *df_config_id
)
{
  void *df_iho_ptr = NULL;
  
  /*if the config type is complete, then we don't need RBS ptr*/
  if(gtow_info.config_type == IHO_GTOW_COMPLETE_CONFIG)
  {
    WRRC_MSG0_ERROR("No Radio Bearer setup Info reqd.");
  }

  if((gtow_info.msg_rel_version == MSG_REL5)
     || (gtow_info.msg_rel_version == MSG_REL6)
     || (gtow_info.msg_rel_version == MSG_REL7)
     || (gtow_info.msg_rel_version == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
     || (gtow_info.msg_rel_version == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
     #error code not present
#endif /* FEATURE_WCDMA_REL10 */


)
  { 
    if(gtow_info.rbs_ptr != NULL)
    {
      df_iho_ptr = (void *)&gtow_info.rbs_ptr->message.u.radioBearerSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.radioBearerSetup_r5;
      *df_config_id = gtow_info.default_config_id;
    }
  }
  /*return the ptr to the sib16*/
  return df_iho_ptr;  
} /* rrciho_get_rbs_ptr */

/*===========================================================================

FUNCTION   rrciho_send_channel_config_for_gtow

DESCRIPTION

  This function first calls set_ordered_config.

  This function channel config req to LLC to perform transition to DCH state
  with the configuration in ordered config.

DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type rrciho_send_channel_config_for_handover_to_utran(void)
{
  /*ptr to internal command for chan config req*/
  rrc_cmd_type *cmd_ptr;

  uint32 i;

  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
#ifdef FEATURE_LTE_TO_WCDMA
  uint8 number_of_rbs = 0;
  uint8 j;
#endif /* FEATURE_LTE_TO_WCDMA */

  WRRC_MSG0_HIGH("Set Ordered Config Sent");

  /*set ordered config before sending channel config request*/
  oc_status =   rrcllc_set_ordered_config(RRC_PROCEDURE_IHO,
                                          RRC_STATE_CELL_DCH,
                                          rrc_HandoverToUTRANCommand_PDU,
                                          gtow_info.ho_ptr);
  if(oc_status != ORDERED_CONFIG_SET)
  {
    WRRC_MSG0_ERROR("OC Set failed for HO to UTRAN");
    return FAILURE;
  }
#ifdef FEATURE_QSH_EVENT_METRIC
  if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
  {
    ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->msg_id = WRRC_DL_DCCH_MESSAGETYPE_HANDOVERTOUTRAN;
    rrc_qsh_log_ota_params(RRC_PROCEDURE_IHO);
    rrc_qsh_commit_data(WRRC_QSH_METRIC_OTA, QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL);
  }
#endif
#ifdef FEATURE_VOC_AMR_WB
  /* This check is done , so that if the NAS Sync ind is not received
   * in the OTA msg, but the Trch blocks are configured for Wb-AMR rate.
   * Then in this case we can derive the codec used from the Transport 
   * Channel blocks size used
   */
#ifndef FEATURE_LTE_TO_WCDMA
  if(RRCRB_CS_VOICE_RAB == gtow_info.est_rabs.rab_type)
#else
  if(gtow_info.is_cs_rab_present)
#endif
  {
    mvs_mode_type codec_info =MVS_MODE_NONE;
    dch_id_mapping dch_mapping[MAX_DCH_VOICE_CHANNEL];
    (void)rrcllc_derive_amr_rate(&codec_info,dch_mapping);
    if(codec_info != MVS_MODE_NONE )
    {
      rrcllcoc_update_oc_orderd_codec_mode(codec_info);
#ifndef FEATURE_LTE_TO_WCDMA
      gtow_info.est_rabs.nas_Synchronisation_Indicator = codec_info;
      WRRC_MSG1_HIGH("Setting Codec Info as %d in gtow_info",gtow_info.est_rabs.nas_Synchronisation_Indicator);  
#else
      if(RRCRB_CS_VOICE_RAB == gtow_info.est_rabs[gtow_info.cs_rab_index].rab_type)
      {
        gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator = codec_info;
        WRRC_MSG1_HIGH("Setting Codec Info as %d in gtow_info",
          gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator);  
      }
#endif
    }
  }
#endif /*FEATURE_VOC_AMR_WB*/
  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* Fill in the next state    */
  cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_CELL_DCH;

  /*fill in the procedure name*/
  cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_IHO;

  /* Fill in the state change indication required flag */
  cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;

  /* Set the channel config cnf required to true */
  cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /*set the srb's that need to be initialized */

  for(i=0; (i < gtow_info.num_srb) && (i < UE_MAX_SRB_SETUP); i++)
  {
    cmd_ptr->cmd.chan_config_req.rb[i].rb_id = gtow_info.srb_id[i];
    cmd_ptr->cmd.chan_config_req.rb[i].rb_config = ESTABLISH_RB;
  }

#ifdef FEATURE_LTE_TO_WCDMA
  for(j = 0; (j < gtow_info.number_of_est_rabs) && (j < MAX_RAB_TO_SETUP); j++)
  {
    for(i=0;(i < gtow_info.est_rabs[j].num_rbs_for_rab) && (i < MAX_RB_PER_RAB); i++)
    {
      if((number_of_rbs + gtow_info.num_srb) < MAX_RB)
      {
        /*set the rb's that need to be initialized */
        cmd_ptr->cmd.chan_config_req.rb[(number_of_rbs + gtow_info.num_srb)].rb_id =
          (uint16)(gtow_info.est_rabs[j].rb_for_rab[i].rb_id);

        cmd_ptr->cmd.chan_config_req.rb[(number_of_rbs + gtow_info.num_srb)].rb_config =
          ESTABLISH_RB;

        number_of_rbs++;
      }
    }
  }

  if((number_of_rbs + gtow_info.num_srb) > MAX_RB)
  {
    WRRC_MSG0_HIGH("Ordered Config Set failed");
    send_change_mode_req(RRC_MODE_INACTIVE);
    procedure_info.failure_cause = (uint32)RR_RRC_PHYSICAL_CHANNEL_FAILURE;
    rrciho_state = RRCIHO_WAIT_INACTIVE_CNF;

    /* Return as SUCCESS otherwise it will be considered as chan_config failure */
    return SUCCESS;
  }

  /* Set the channel config cnf required to true */
  cmd_ptr->cmd.chan_config_req.num_rb_to_config =
    (int)(gtow_info.num_srb + number_of_rbs);

#else
  for(i=0;i < gtow_info.est_rabs.num_rbs_for_rab; i++)
  {
    /*set the rb's that need to be initialized */
    cmd_ptr->cmd.chan_config_req.rb[(i+gtow_info.num_srb)].rb_id =
      (gtow_info.est_rabs.rb_for_rab[i].rb_id);

    cmd_ptr->cmd.chan_config_req.rb[(i+gtow_info.num_srb)].rb_config =
      ESTABLISH_RB;
  }

  /* Set the channel config cnf required to true */
  cmd_ptr->cmd.chan_config_req.num_rb_to_config =
    (int)(gtow_info.num_srb + gtow_info.est_rabs.num_rbs_for_rab);
#endif /* FEATURE_LTE_TO_WCDMA */

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );

  WRRC_MSG0_HIGH("RRC_CHAN_CONFIG_REQ sent");

  /* Change the procedure state to wait for change mode confirmation */
  rrciho_state = RRCIHO_WAIT_CHAN_CONFIG_GTOW_CNF;

  return SUCCESS;

}/*rrciho_send_channel_config_for_gtow*/
/*===========================================================================

FUNCTION   rrciho_send_acq_req

DESCRIPTION

  This function sends Acquisition request to LLC which funnels it to L1.
  After we have successfully transitioned WCDMA mode to
  INTER_RAT_IN_PROGRESS we send this command to acquire on WCDMA cell
  based on the measurement reports we receive from GSM L1(MPH)

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_acq_req(void)
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CSP_ACQ_REQ;

  /* Fill in the Frequence on which we want to acquire    */
  cmd_ptr->cmd.rrc_csp_acq_req.freq = gtow_info.freq;

  /* Fill in the scrambling code */
  cmd_ptr->cmd.rrc_csp_acq_req.scr_code = (uint16)(gtow_info.scr_code * 16);

  /* Fill in the pn_offset information */
  cmd_ptr->cmd.rrc_csp_acq_req.pn_offset_present = gtow_info.pn_offset_present;
  cmd_ptr->cmd.rrc_csp_acq_req.pn_pos = gtow_info.pn_offset;

  cmd_ptr->cmd.rrc_csp_acq_req.network_select_mode = gtow_info.network_select_mode;

  WRRC_MSG2_HIGH("PN_OFFSET_PRESENT %d, PN_OFFSET %d", gtow_info.pn_offset_present, gtow_info.pn_offset);
  /* Fill in the diversity information */
  cmd_ptr->cmd.rrc_csp_acq_req.sttd_indicator_present =
    gtow_info.diversity_info_present;
  cmd_ptr->cmd.rrc_csp_acq_req.sttd_indicator =
    gtow_info.diversity_info;


  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );

  WRRC_MSG0_HIGH("RRC_CSP_ACQ_REQ sent to CSP");

  /* Change the procedure state to wait for change mode confirmation */
  rrciho_state = RRCIHO_WAIT_ACQ_CNF;

  return;
}/*rrciho_send_acq_req*/
/*===========================================================================

FUNCTION   rrciho_send_handover_to_utran_abort_cnf

DESCRIPTION

  This function sends handover to UTRAN Abort Cnf to RR.  This msg is in
  response  to HO_ABORT_REQ from RR

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_handover_to_utran_abort_cnf(void)
{
  /*declare var of struct abort confirm type*/
  rrc_interrat_handover_abort_cnf_type gtow_abort_cnf;

  /*delete all the stored handover to utran IE's as handover is successful*/
  rrciho_delete_stored_gtow_info();
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB 
  csfb_extended_status = CSFB_INVALID;
#endif
  /*initialize the HO parameters to default*/
  rrciho_init_data();

  /* Send the RR_INTERRAT_HANDOVER_ABORT_REQ message to RR */
  rr_send_rrc_message_to_rr_internal((void *)(&(gtow_abort_cnf)), RRC_INTERRAT_HANDOVER_ABORT_CNF,
                            sizeof(rrc_interrat_handover_abort_cnf_type));

#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
    /*Send  GtoW hand over failure   */
    rrc_send_hho_cmd_to_cm(SYS_HO_FAIL,SYS_HO_TYPE_GSM_UMTS);

#endif

  WRRC_MSG0_HIGH("INTERRAT_HANDOVER_CNF Failure sent to RR");

#ifdef FEATURE_DUAL_SIM
  rrc_check_and_perform_trm_exchange();
  rrc_release_lock();
#endif

  return;
}/*rrciho_send_handover_to_utran_abort_cnf*/
/*===========================================================================

FUNCTION   rrciho_send_handover_to_utran_failure

DESCRIPTION

  This function sends handover to UTRAN failure to RR.  We could have failed
  Handover due to ACQ failure, Channel Config failure or due to HO_ABORT_REQ
  from RR

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_handover_to_utran_failure(void)
{
  /*declare var of struct handover confirm type*/
  rrc_interrat_handover_cnf_type gtow_ho_cnf;

  /*delete all the stored handover to utran IE's as handover is successful*/
  rrciho_delete_stored_gtow_info();

  /*initialize the HO parameters to default*/
  rrciho_init_data();
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB   
  csfb_extended_status = CSFB_INVALID;
#endif
  /*set HO status to success*/
  gtow_ho_cnf.handover_status = RR_RRC_FAILURE;

  switch( procedure_info.failure_cause)
  {
    case 0: gtow_ho_cnf.failure_cause = RR_RRC_INVALID_STATE;
            break;
    case 1: gtow_ho_cnf.failure_cause = RR_RRC_WCDMA_PROTOCOL_ERROR;
      break;
    case 2: gtow_ho_cnf.failure_cause = RR_RRC_INVALID_CONFIGURATION;
      break;
    case 3: gtow_ho_cnf.failure_cause = RR_RRC_PHYSICAL_CHANNEL_FAILURE;
      break;
    case 4: gtow_ho_cnf.failure_cause = RR_RRC_INVALID_CIPHER_ALGORITHM;
      break;
    case 5: gtow_ho_cnf.failure_cause = RR_RRC_FREQUENCY_NOT_IMPLEMENTED;
      break;
#ifdef FEATURE_GAN_3G
    #error code not present
#else
    case 6: gtow_ho_cnf.failure_cause = RR_RRC_MAX_FAILURE_CAUSE;
      break;
#endif
    default: /* no action */
      break;
  }

  /* Send the RR_INTERRAT_HANDOVER_REQ message to RR */
  rr_send_rrc_message_to_rr_internal((void *)(&(gtow_ho_cnf)), RRC_INTERRAT_HANDOVER_CNF,
                            sizeof(rrc_interrat_handover_cnf_type));

#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
    /*Send  GtoW hand over failure   */
    rrc_send_hho_cmd_to_cm(SYS_HO_FAIL,SYS_HO_TYPE_GSM_UMTS);
#endif

  WRRC_MSG0_HIGH("INTERRAT_HANDOVER_CNF Failure sent to RR");

#ifdef FEATURE_DUAL_SIM
  rrc_check_and_perform_trm_exchange();
  rrc_release_lock();
#endif

#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(rrc_get_current_codec_mode());
#endif


  return;
}/*rrciho_send_handover_to_utran_failure*/
/*===========================================================================

FUNCTION   rrciho_send_handover_to_utran_complete

DESCRIPTION

  This function sends a HO success to UTRAN complete message to
  UTRAN.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_handover_to_utran_complete(void)
{
  rrc_UL_DCCH_Message *msg_ptr=NULL;    /* Pointer to uplink DCCH message */

  rlc_lc_id_type ul_lc_id;              /* Uplink logical channel ID */

  rrcsend_status_e_type send_status;    /* The status of sending the Initial
                                         * Direct Transfer message */


  /* Build a Handover To Utran Complete msg. Start by allocating an Uplink
   * DCCH message. This memory is released by rrcsend_send_ul_sdu().
   */

  /* Determine the logical channel id for the Uplink DCCH */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                       DCCH_AM_RADIO_BEARER_ID,
                                       UE_MODE_ACKNOWLEDGED_DATA);

  /* Ensure that LCM found a valid logical channel id  */
  if (ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    /* Found a valid RLC Logical Channel Id */
    msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc(
                sizeof(struct rrc_UL_DCCH_Message));

    /*Identify this message as HO to Utran Complete Message*/
    msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_handoverToUTRANComplete;

    /* Initialize bit_mask to zero */
    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.handoverToUTRANComplete);

    if((gtow_info.gsm_ciph_status == TRUE)&&
      (rrcsmc_tm_ciph_active() == TRUE))
    {
     RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.handoverToUTRANComplete,
        rrc_HandoverToUTRANComplete,count_C_ActivationTime);

      /*256 is the CFN rollover length*/
      /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
      /*Also make sure that the activation time is a multiple of 8 */
      gtow_info.count_c_act_time = (seq_get_cfn() + 250) & 0xF8;

      msg_ptr->message.u.handoverToUTRANComplete.count_C_ActivationTime =
        (uint8)gtow_info.count_c_act_time;
    }

    /* Set bit_mask to include START_list */
    RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.handoverToUTRANComplete,
     startList);

   
    /* Appends START list to the HO to UTRAN Complete message */
    (void)rrcsmc_append_start_list(
      &msg_ptr->message.u.handoverToUTRANComplete.startList,
      RRCSMC_APPEND_START);
#ifdef FEATURE_QSH_EVENT_METRIC
   /*
	   uint8 count_c_activation;
	   uint8 failure_cause;
	   uint8 cs_start_present:1;
	   uint8 ps_start_present:1;
	   uint8 ul_integrity_info_present:1;
	   uint8 ul_counter_sync_info:1;
   */
   if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
   {
      if(msg_ptr->message.u.handoverToUTRANComplete.m.count_C_ActivationTimePresent)
      {
	    ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.count_c_activation= msg_ptr->message.u.handoverToUTRANComplete.count_C_ActivationTime;
      }
    }
#endif
   /* Send the HO to UTRAN on the Uplink DCCH
    */
      send_status = rrcsend_send_ul_sdu(RRC_PROCEDURE_IHO,
                                        msg_ptr,
                                        ul_lc_id,
                                        RRCSEND_L2ACK_NOT_REQUIRED);

    if (send_status == RRCSEND_SUCCESS)
    {
      WRRC_MSG0_HIGH("HO to UTRAN complete Msg Sent");
    }
    else
    {
      WRRC_MSG1_ERROR("Error %d sending HO to UTRAN Complete",
                send_status);
    }
  }/* Found a valid RLC Logical Channel Id */
  else
  {
    WRRC_MSG0_HIGH("Can't find RLC ID for RB 2");
  }
} /* rrciho_send_handover_to_utran_complete */

/*===========================================================================

FUNCTION   rrciho_send_handover_to_utran_success

DESCRIPTION

  This function sends a HO success to UTRAN as well as a HO success indication
  to RR.  We should have successfully done Acquisition on WCDMA side and
  configured WCDMA lower layers to go to DCH state.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_send_handover_to_utran_success(void)
{
  /*declare var of struct handover confirm type*/
  rrc_interrat_handover_cnf_type gtow_ho_cnf;

  /*delete all the stored handover to utran IE's as handover is successful*/
  rrciho_delete_stored_gtow_info();

  /*initialize the HO parameters to default*/
  rrciho_init_data();

  /*set HO status to success*/
  gtow_ho_cnf.handover_status = RR_RRC_SUCCESS;

  /* Send the RR_INTERRAT_HANDOVER_REQ message to RR */
  rr_send_rrc_message_to_rr_internal((void *)(&(gtow_ho_cnf)), RRC_INTERRAT_HANDOVER_CNF,
                            sizeof(rrc_interrat_handover_cnf_type));

  WRRC_MSG0_HIGH("Successful INTERRAT_HANDOVER_CNF sent to RR");

#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
      /*Send G2W complete to CM*/
  rrc_send_hho_cmd_to_cm(SYS_HO_COMPLETE,SYS_HO_TYPE_GSM_UMTS);
#endif

  return;
}/*rrciho_send_handover_to_utran_success*/

/*===========================================================================

FUNCTION   RRCIHO_INIT_DEFAULT_CONFIGURATION

DESCRIPTION

  This function initializes the default configurations.  We see a lot of
  hard-coded values in the foll. functions.  All the values are copied
  straight from 13.7 of 25.331.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_init_default_configuration(void)
{
  /*copy the default config ptr and the number of bytes in the
    default config to the global var*/
  default_config_info[RRC_DEF_CONFIG_0].default_config_len = default_config_0_len;
  default_config_info[RRC_DEF_CONFIG_0].default_config_ptr = default_config_0;

  default_config_info[RRC_DEF_CONFIG_1].default_config_len = default_config_1_len;
  default_config_info[RRC_DEF_CONFIG_1].default_config_ptr = default_config_1;

  default_config_info[RRC_DEF_CONFIG_3].default_config_len = default_config_3_len;
  default_config_info[RRC_DEF_CONFIG_3].default_config_ptr = default_config_3;


  default_config_info[RRC_DEF_CONFIG_4].default_config_len = default_config_4_len;
  default_config_info[RRC_DEF_CONFIG_4].default_config_ptr = default_config_4;

  default_config_info[RRC_DEF_CONFIG_6].default_config_len = default_config_6_len;
  default_config_info[RRC_DEF_CONFIG_6].default_config_ptr = default_config_6;

  default_config_info[RRC_DEF_CONFIG_7].default_config_len = default_config_7_len;
  default_config_info[RRC_DEF_CONFIG_7].default_config_ptr = default_config_7;

  default_config_info[RRC_DEF_CONFIG_8].default_config_len = default_config_8_len;
  default_config_info[RRC_DEF_CONFIG_8].default_config_ptr = default_config_8;

  default_config_info[RRC_DEF_CONFIG_9].default_config_len = default_config_9_len;
  default_config_info[RRC_DEF_CONFIG_9].default_config_ptr = default_config_9;

  default_config_info[RRC_DEF_CONFIG_11].default_config_len = default_config_11_len;
  default_config_info[RRC_DEF_CONFIG_11].default_config_ptr = default_config_11;

  default_config_info[RRC_DEF_CONFIG_13].default_config_len = default_config_13_len;
  default_config_info[RRC_DEF_CONFIG_13].default_config_ptr = default_config_13;


  default_config_info[RRC_DEF_CONFIG_12].default_config_len = default_config_12_len;
  default_config_info[RRC_DEF_CONFIG_12].default_config_ptr = default_config_12;
  
  default_config_info[RRC_DEF_CONFIG_14].default_config_len = default_config_14_len;
  default_config_info[RRC_DEF_CONFIG_14].default_config_ptr = default_config_14;

  default_config_info[RRC_DEF_CONFIG_10].default_config_len = default_config_10_len;
  default_config_info[RRC_DEF_CONFIG_10].default_config_ptr = default_config_10;

  default_config_info[RRC_DEF_CONFIG_15].default_config_len = default_config_15_len;
  default_config_info[RRC_DEF_CONFIG_15].default_config_ptr = default_config_15;

  default_config_info[RRC_DEF_CONFIG_16].default_config_len = default_config_16_len;
  default_config_info[RRC_DEF_CONFIG_16].default_config_ptr = default_config_16;

  default_config_info[RRC_DEF_CONFIG_17].default_config_len = default_config_17_len;
  default_config_info[RRC_DEF_CONFIG_17].default_config_ptr = default_config_17;

  default_config_info[RRC_DEF_CONFIG_18].default_config_len = default_config_18_len;
  default_config_info[RRC_DEF_CONFIG_18].default_config_ptr = default_config_18;

  default_config_info[RRC_DEF_CONFIG_19].default_config_len = default_config_19_len;
  default_config_info[RRC_DEF_CONFIG_19].default_config_ptr = default_config_19;

  default_config_info[RRC_DEF_CONFIG_20].default_config_len = default_config_20_len;
  default_config_info[RRC_DEF_CONFIG_20].default_config_ptr = default_config_20;

  default_config_info[RRC_DEF_CONFIG_21].default_config_len = default_config_21_len;
  default_config_info[RRC_DEF_CONFIG_21].default_config_ptr = default_config_21;

  default_config_info[RRC_DEF_CONFIG_22].default_config_len = default_config_22_len;
  default_config_info[RRC_DEF_CONFIG_22].default_config_ptr = default_config_22;

  default_config_info[RRC_DEF_CONFIG_23].default_config_len = default_config_23_len;
  default_config_info[RRC_DEF_CONFIG_23].default_config_ptr = default_config_23;

#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif

}
/*===========================================================================

FUNCTION   rrciho_event_log_handover_to_utran

DESCRIPTION

    Event Logging for HO to UTRAN command.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_event_log_handover_to_utran(void)
{
  /* declare an event log type for handover start*/
  rrclog_wcdma_handover_to_utran_start_event_type start_event_log;

  /*update freq and scrambling code info*/
  start_event_log.uarfcn = gtow_info.freq;
  start_event_log.scr_code = gtow_info.scr_code;

  event_report_payload(EVENT_GSM_TO_WCDMA_HANDOVER_START,
                       sizeof(start_event_log),
                       (void *)&start_event_log);
}/*rrciho_event_log_handover_to_utran*/

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION   RRCIHO_GET_LOCK_FOR_IHO

DESCRIPTION

  This function tries to acquire lock for IHO. If lock is not acquired
  it sends a failure message to RR.

DEPENDENCIES

  None.

RETURN VALUE

    SUCCESS/FAILURE - if lock is acquired or not.

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type rrciho_get_lock_for_iho(wrm_freq_input_type lock_req_bands)
{
  uecomdef_status_e_type status = SUCCESS;
  rrc_out_of_lock_wait_e_type  lock_status;

  lock_status = rrc_acquire_lock_for_IHO_or_CCO(RRCTMR_WRM_LOCK_WAIT_TIMER_FOR_INTERRAT_IN_MS, lock_req_bands);
  rrc_stop_lock_wait_timer();

  if(lock_status == RRC_ABORT_FROM_RR)
  {
    WRRC_MSG0_HIGH("DSIM: Lock not acquired for IHO. Abort from RR.");
#ifdef FEATURE_LTE_TO_WCDMA
    if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
    {
      rrciho_send_ltow_ps_handover_to_utran_abort_cnf();
    }
    else
#endif
    {
      rrciho_send_handover_to_utran_abort_cnf();
    }
    status = FAILURE;
  }
  else if(lock_status != RRC_WRM_LOCK_ACQUIRED)
  {
    WRRC_MSG0_HIGH("DSIM: Lock not acquired for IHO.");

#ifdef FEATURE_LTE_TO_WCDMA
    if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
    {
      procedure_info.failure_cause = (uint32)LTOW_PSHO_INVALID_STATE;
      /*send a handover to utran failure and reset IHO procedure's state
        machine*/
      rrciho_send_ltow_ps_handover_to_utran_failure();
    }
    else
#endif
    {
      procedure_info.failure_cause = (uint32)RR_RRC_INVALID_STATE;
      /*send a handover to utran failure and reset IHO procedure's state
        machine*/
      rrciho_send_handover_to_utran_failure();
    }

    status = FAILURE;
  }
  else
  {
    WRRC_MSG0_HIGH("DSIM: Lock acquired for IHO.");
  }
  return status;
}
#endif /* FEATURE_DUAL_SIM */

/*===========================================================================

FUNCTION   PROCESS_HANDOVER_TO_UTRAN_COMMAND

DESCRIPTION

  This function processes the handover to UTRAN command. It
  decodes the HandoverToUTRANCommand and checks all the IE's
  and starts the process of acquiring a WCDMA cell.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_handover_to_utran_command(
  rrc_rr_cmd_type * cmd_ptr
)
{

  byte*  ho_msg_ptr;                     /* ptr to encoded HO msg     */
#ifdef FEATURE_DUAL_SIM
  wrm_freq_input_type lock_req_bands;
#endif
  WRRC_MSG0_HIGH("Rcvd HO To UTRAN from GSM");

#ifdef FEATURE_INTER_RAT_HO_OPT
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
  if(mn_has_nas_already_acquired_voc_subs(rrc_get_as_id()) == TRUE)
#else
  if(mn_has_nas_already_acquired_voc() == TRUE)
#endif
  {
    /* Release MVS client of GSM */
    WRRC_MSG0_HIGH("Releasing MVS Client(G) during G->W handover");
    ghdi_mvs_release(MVS_CLIENT_GSM);
  }
#endif /*FEATURE_INTER_RAT_HO_OPT*/

#ifdef FEATURE_LTE_TO_WCDMA
  iho_interrat_type = RRCIHO_INTERRAT_GSM;
#endif

  /*init the default config here.. we have to do this for every call
    because the spoof poke byte can be set at any time*/
  rrciho_init_default_configuration();

  /*the HO message is the second byte from the beginning of the message.
    So initialize the pointer to the second byte */
   ho_msg_ptr = &(cmd_ptr->cmd.interrat_handover_req.ho_cmd[1]);

#ifdef FEATURE_RAT_PRIORITY_LIST
  rrc_csp_update_rat_pri_list_info(&cmd_ptr->cmd.interrat_handover_req.rat_pri_list_info);
#endif

#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO
  /*updating the dedicated priority info received in rrc_interrat_handover_req into global structure (rrc_dedicated_priority_info)*/
  rrcmeas_update_dedicated_priority_info_from_irat_req(&cmd_ptr->cmd.interrat_handover_req.dedicated_priority_info,
                             mmumts_convert_nas_plmn_id_to_rrc_plmn_id(cmd_ptr->cmd.interrat_handover_req.plmn_id));
#endif

  /*the HO message is the 1st byte from the beginning of the message.
    So initialize the length of the message to the contents of the first
    byte*/
    gtow_info.ho_msg_len = cmd_ptr->cmd.interrat_handover_req.ho_cmd[0];

    gtow_info.network_select_mode = cmd_ptr->cmd.interrat_handover_req.network_select_mode;

  /*get the ptr to the byte stream of the HandoverToUTRANCommand*/
    gtow_info.ho_ptr =
      (rrc_HandoverToUTRANCommand *) rrcasn1_decode_pdu(  (void *)ho_msg_ptr,
                                                          rrc_HandoverToUTRANCommand_PDU,
                                                          gtow_info.ho_msg_len
                                                          );

     /* Save GtoW Utran message with type RRC_HO_TO_UTRAN  */
   rrc_crash_debug_save_ota(ho_msg_ptr, gtow_info.ho_msg_len,RRC_HO_TO_UTRAN,0xFF,RRC_PROCEDURE_IHO);

  if(gtow_info.ho_ptr == NULL)
  {
    /*unable to successfully decode a asn1 PDU, set failure cause to
      Protocol_Error
     */
    procedure_info.failure_cause = (uint32)RR_RRC_WCDMA_PROTOCOL_ERROR;

    /*set to flag that HO to UTRAN msg ptr is not present*/
    gtow_info.ho_msg_present = FALSE;

    WRRC_MSG0_ERROR("Unable to decode HO to Utran Msg");
#ifdef FEATURE_GAN_3G
    #error code not present
#endif
    {
      /* Send handover To UTRAN faliure to RR*/
      rrciho_send_handover_to_utran_failure();
    }
    return;
  }
  else
  {
    /*set the flag to indicate that HO to UTRAN msg is present*/
    gtow_info.ho_msg_present = TRUE;
  }

  /* Verify the handover from UTRAN command */
  if (verify_and_extract_handover_to_utran_command(gtow_info.ho_ptr
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
                           , mmumts_convert_nas_plmn_id_to_rrc_plmn_id(
                                  cmd_ptr->cmd.interrat_handover_req.plmn_id)
#endif
      ) == FAILURE)
  {
#ifdef FEATURE_GAN_3G
    #error code not present
#endif
    {
      /* Send handover To UTRAN faliure to RR*/
      rrciho_send_handover_to_utran_failure();
      rrc_free_dynamic_ptrs();
    }
  }
  else
  {

#ifdef FEATURE_DUAL_SIM
  /* Check if TRM exchange is successful */
  rrc_perform_trm_exchange();

  rrc_wrm_populate_bands_for_lock_req(&lock_req_bands, gtow_info.freq);

  /* Even after WCDMA has received the lock, call the below API to update WRRC client lock status 
     For non CSFB, acquire lock here */
  if(rrciho_get_lock_for_iho(lock_req_bands) == FAILURE)
  {
    return;
  }
#endif

#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
    rrc_send_codec_info_to_cm(rrc_get_ordered_codec_mode());
#endif
    /*ho cmd is kosher.. so log the event that HO is starting*/
    rrciho_event_log_handover_to_utran();

    /*process measurement data received in the handover to utran command*/
    process_meas_data_for_handover_to_utran_command(&cmd_ptr->cmd.interrat_handover_req.interrat_measurement_data);

#ifdef FEATURE_GAN_3G
    #error code not present
#endif

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB 
    WRRC_MSG1_HIGH("CSFB: CSFB Call status indicated by GRR %d ",cmd_ptr->cmd.interrat_handover_req.csfb_call_status);
    if((rrc_fast_return_to_lte_after_csfb_nv) && (cmd_ptr->cmd.interrat_handover_req.csfb_call_status))
    {
      csfb_extended_status = CSFB_STARTED;
      WRRC_MSG0_HIGH("CSFB_HACK: GRR triggered HO of a CSFB call");
    }
#endif
    /* Request MCM to change the RRC mode Inactive to Interrat in Progress */
    send_change_mode_req(RRC_MODE_INTERRAT_IN_PROGRESS);

    /* Change the procedure state to wait for change mode confirmation */
    rrciho_state = RRCIHO_WAIT_IRAT_IN_PROGRESS_CNF;
  }

  /*if verification of the Handover message is successful, then copy the ciphering
    status of GSM mode to the gtow information database.. also copy the ciphering
    status only if nv item for ciphering is set.. otherwise set it to false*/
  if((cmd_ptr->cmd.interrat_handover_req.cipher_enabled == TRUE) &&
     (nv_ciphering_enabled))
  {
    gtow_info.gsm_ciph_status = TRUE;
  }
  else
  {
    gtow_info.gsm_ciph_status = FALSE;
  }

  return;
} /* end of process_handover_to_utran_command*/


/*===========================================================================

FUNCTION   rrciho_get_activation_time_for_wtog_ho

DESCRIPTION

  This function populates activation time for WtoG handover into the received
  ptr and also returns success/failure if activation is present or not.

DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS/FAILURE depending if Activation Time was appended or not.

SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrciho_get_activation_time_for_wtog_ho
(
uint16 * act_time
)
{
  /*if activation time was present in the message then send the same*/
  if(procedure_info.act_time_present == TRUE)
  {
    *act_time = (uint16)(procedure_info.activation_time);
    return SUCCESS;
  }
  else
  {
    return FAILURE;
  }
}
/*===========================================================================

FUNCTION   RRCIHO_EXTRACT_SINGLE_GSM_MESSAGE

DESCRIPTION

  This function extracts single GSM message from the HandoverFromUtran
  command.  This message is part of the padding bits.

DEPENDENCIES

  None.

RETURN VALUE

  TRUE or FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrciho_extract_single_gsm_message(
  rrc_cmd_type *cmd_ptr                      /* Received command */
)
{

  uint32 total_pad_bits;              /* number of gsm-padding bits that make up gsm msg*/
  uint32 last_octet_pad_len;          /* number of wcdma-padding bits in last octet of msg*/
  uint32 num_bits_in_msg;             /* number of bits in ota msg without padding*/
  //rrc_DL_DCCH_Message *dcch_ptr;      /* Pointer to the DCCH message */
  uint32 local_encoded_sdu_len;       /* local var to hold encoded sdu length*/
  uint8 index=0;
  /* Get the downlink message from the SDU */
  //dcch_ptr=(rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  index =rrc_get_used_ctxt_idx_for_asn1_ptr(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg);
  
  if(index == RRC_MAX_NUM_ASN1_CTXT)
  {
    WRRC_MSG0_ERROR("Invalid ctxt index");
    return FALSE;
  }
  
  local_encoded_sdu_len = pu_getMsgLen(&dec_ctxt_list[index].dec_context);
  last_octet_pad_len = PU_GETPADBITS(&dec_ctxt_list[index].dec_context);
  /*total number of bits in the message is equal to the number of bytes in message
    multiplied by 8 and the total subtracted by the number of pad bits in the last
    octet*/
  num_bits_in_msg = ( (local_encoded_sdu_len*8) - last_octet_pad_len);
  /* can be cal used  
  num_bits_in_msg = PU_GETCTXTBITOFFSET (&dec_ctxt_list[index]);*/
   WRRC_MSG2_HIGH("Pad_Bit_Len %d, Decode_Msg_len %d", last_octet_pad_len,
                                                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
  /*total number of bits in the padding part of dl sdu received.. subtract new
    encoded length from the downlink sdu length and multiply by 8.  no need to do
    any kind of modulo 8 division to copy till last bit because GSM messages are
    byte aligned and cannot*/
  total_pad_bits = ((cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length - local_encoded_sdu_len)*8);
  if(total_pad_bits == 0)
  {
    WRRC_MSG0_HIGH("Incorrect OTA. No Padded bits, failed to extract single GSM element");
    procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_protocolError;
    procedure_info.protocol_error = rrc_ProtocolErrorCause_informationElementMissing;
    return FALSE;
  }
  /*copy the message length and payload into the existing gsm message command list..
    just copy it to the first buffer of the gsm message list.. we want to use
    the same existing interface between rrc-rr.. single message essentially means
    a list with length 1*/
  procedure_info.parms.gsm.gsm_command_list.number_of_commands = 1;
  procedure_info.parms.gsm.gsm_command_list.gsm_commands[0].message_length =
    (uint8)(total_pad_bits/8);

  /*total number of bytes*/
  b_copy( cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
          (uint16)(num_bits_in_msg),
          procedure_info.parms.gsm.gsm_command_list.gsm_commands[0].payload,
          0,
          (uint16)(total_pad_bits)
        );

  return TRUE;
}

/*===========================================================================

FUNCTION   VERIFY_AND_EXTRACT_HANDOVER_FROM_UTRAN_COMMAND_R6

DESCRIPTION

  This function verifies the handover from UTRAN command and sets the failure
  cause properly if an error is detected. If the command is valid, it extracts
  necessary information from the command.

DEPENDENCIES

  None.

RETURN VALUE

  TRUE or FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
static boolean verify_and_extract_handover_from_utran_command_r6(
  rrc_HandoverFromUTRANCommand_GSM *msg_ptr,
  rrc_cmd_type *cmd_ptr                      /* Received command */
)
{
  rrc_GSM_MessageList *gsm_cmd_ptr;
  uint8 gsm_cmd_ct, idx = 0;
  rrc_RAB_InformationList_r6 * rab_info_list = NULL;

#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_ho_gsm_ota ho_gsm_params;
  memset(&ho_gsm_params,0,sizeof(rrc_log_rrc_ho_gsm_ota)); 
  
  ho_gsm_params.freq_band = msg_ptr->u.later_than_r3.criticalExtensions.u.
    r6.handoverFromUTRANCommand_GSM_r6.frequency_band;
  
  ho_gsm_params.ho_msg_type = msg_ptr->u.later_than_r3.criticalExtensions.u.r6.
    handoverFromUTRANCommand_GSM_r6.gsm_message.t;

  rrc_qsh_log_ho_to_gsm_params(&ho_gsm_params);
#endif

  procedure_info.handover_type = T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM;

  /* Check IEs in the command */
  /* If non-critical extensions are present, ignore for now */
  if (msg_ptr->u.later_than_r3.criticalExtensions.u.r6.m.handoverFromUTRANCommand_GSM_r6_add_extPresent)
  {
    /* Ignore Non-critical extension */
    WRRC_MSG0_HIGH("Ignore R6 nonCriticalExtensions in CMD");
  }
  if(msg_ptr->u.later_than_r3.criticalExtensions.u.r6.m.v860NonCriticalExtensionsPresent)
  {
    WRRC_MSG0_HIGH("REL8: v860NonCriticalExtensions not supported");
  }


  /* For now reject any PS handover as it's not supported */

  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.r6.handoverFromUTRANCommand_GSM_r6,
      rrc_HandoverFromUTRANCommand_GSM_r6_IEs,toHandoverRAB_Info)) 
  {
    uint32 local_idx=0;
    rab_info_list = 
      &msg_ptr->u.later_than_r3.criticalExtensions.u.r6.handoverFromUTRANCommand_GSM_r6.toHandoverRAB_Info;
    while (rab_info_list->n > local_idx) 
    {
      if (rab_info_list->elem[local_idx].cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain) 
      {
        WRRC_MSG0_ERROR("PS handover not supported");
        procedure_info.transaction_id =  msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_configurationUnacceptable;
        break;
      }
      local_idx++;
    }
  }

  /*check if activation time is present and save the same in procedure_info*/
  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.r6.handoverFromUTRANCommand_GSM_r6,
      rrc_HandoverFromUTRANCommand_GSM_r6_IEs,activationTime))
  {
    /*copy the activation time and set the flag to true*/
    procedure_info.activation_time =
      msg_ptr->u.later_than_r3.criticalExtensions.u.r6.handoverFromUTRANCommand_GSM_r6.activationTime;
    procedure_info.act_time_present = TRUE;
  }
  else
  {
    /*else set activation time to false*/
    procedure_info.act_time_present = FALSE;
  }

  /* Extract the transaction identifier from the command */
  procedure_info.transaction_id =
    msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  /* Fail handover if Ordered Config is set */
  if(OC_NOT_SET != rrcllc_get_ordered_config_status() 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
     || rrcdata_check_if_ota_is_buffered()
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    )
  {
    procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;
    return (FALSE);
  }

  /* Extract frequency band */
  if (msg_ptr->u.later_than_r3.criticalExtensions.u.
  	r6.handoverFromUTRANCommand_GSM_r6.frequency_band == rrc_Frequency_Band_dcs1800BandUsed)
  {
    procedure_info.parms.gsm.frequency_band = RRC_RR_DCS_1800;
  }
  else
  {
    procedure_info.parms.gsm.frequency_band = RRC_RR_PCS_1900;
  }

  /* Extract GSM commands */
  if (msg_ptr->u.later_than_r3.criticalExtensions.u.r6.
  	handoverFromUTRANCommand_GSM_r6.gsm_message.t
       == T_rrc_HandoverFromUTRANCommand_GSM_r6_IEs_gsm_message_single_GSM_Message)
  {
    WRRC_MSG0_HIGH("Single GSM CMD received");

    /*if extracting single gsm message failed for any reason.. return failure*/
    if(!(rrciho_extract_single_gsm_message(cmd_ptr)))
    {
      return (FALSE);
    }
  }
  else
  {
    /* Extract the GSM command list: For handover, list should only contain 1 command */
    gsm_cmd_ct = 0;
    gsm_cmd_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.r6.handoverFromUTRANCommand_GSM_r6.gsm_message.u.gsm_MessageList.gsm_Messages;

    /* Check if these is no GSM command present */
    if ( (gsm_cmd_ptr == NULL ) || (gsm_cmd_ptr->n ==0))
    {
      /* Set failure cause and return failure */
      procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_protocolError;
      procedure_info.protocol_error = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
      WRRC_MSG0_HIGH("GSM Command list is empty!");
      return (FALSE);
    }

    while ((gsm_cmd_ptr->n >  idx) && (gsm_cmd_ct < RRC_RR_MAX_GSM_COMMANDS ))
    {

      gsm_cmd_ct++;
      
      /* Convert message length from the number of bits to number of bytes */
      procedure_info.parms.gsm.gsm_command_list.gsm_commands[gsm_cmd_ct-1].message_length =
        (uint8) (gsm_cmd_ptr->elem[idx].numbits/ 8);

      /* Check if the command is lined up with byte boundary */
      if ((gsm_cmd_ptr->elem[idx].numbits% 8) != 0)
      {
        /* not in byte boundary: indicate a warning and continue the process */
        procedure_info.parms.gsm.gsm_command_list.gsm_commands[gsm_cmd_ct-1].message_length++;
        WRRC_MSG0_HIGH("GSM CMD is not on byte boundary");
      }

      /* Extract the GSM command */
      WCDMA_MEMCPY((procedure_info.parms.gsm.gsm_command_list.gsm_commands[gsm_cmd_ct-1].payload),
                   sizeof(uint8) * RRC_RR_MAX_GSM_MESSAGE_LENGTH,
                   gsm_cmd_ptr->elem[idx].data,
                   MIN( procedure_info.parms.gsm.gsm_command_list.gsm_commands[gsm_cmd_ct-1].message_length,
                            sizeof(uint8)*RRC_RR_MAX_SINGLE_GSM_MESSAGE_LENGTH ));

      /* Move to the next command */
      idx++;

    }

    /* Check if the command list is more than RRC_RR_MAX_GSM_COMMANDS, indicate a warning */
    if ( gsm_cmd_ptr->n >  gsm_cmd_ct )
    {
       WRRC_MSG1_HIGH("More than %d GSM CMDs Received", RRC_RR_MAX_GSM_COMMANDS);
    }

    /* Set the command counter */
    procedure_info.parms.gsm.gsm_command_list.number_of_commands = gsm_cmd_ct;
  }

  return (TRUE);

} /* end of verify_and_extract_handover_from_utran_command_r6 */

/*===========================================================================

FUNCTION   VERIFY_AND_EXTRACT_HANDOVER_FROM_UTRAN_COMMAND

DESCRIPTION

  This function verifies the handover from UTRAN command and sets the failure
  cause properly if an error is detected. If the command is valid, it extracts
  necessary information from the command.

DEPENDENCIES

  None.

RETURN VALUE

  TRUE or FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
static boolean verify_and_extract_handover_from_utran_command(
  rrc_HandoverFromUTRANCommand_GSM *msg_ptr,
  rrc_cmd_type *cmd_ptr                      /* Received command */
)
{
  rrc_GSM_MessageList *gsm_cmd_ptr;
  uint8 gsm_cmd_ct;

#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_ho_gsm_ota ho_gsm_params;
  memset(&ho_gsm_params,0,sizeof(rrc_log_rrc_ho_gsm_ota));
#endif

  procedure_info.handover_type = T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM;


  /* Check the message if the critical extension is present */
  if (msg_ptr->t == T_rrc_HandoverFromUTRANCommand_GSM_later_than_r3)
  {
    /* Check if R6 Msg is received */
    if (msg_ptr->u.later_than_r3.criticalExtensions.t == 
        T_rrc_HandoverFromUTRANCommand_GSM_criticalExtensions_1_r6) 
    {
      WRRC_MSG0_HIGH("Rcvd R6 HO from Utran msg");
      return verify_and_extract_handover_from_utran_command_r6(msg_ptr, cmd_ptr);
    }
    else
    {
      /* Critical extension is not supported in this release, send handover from
         UTRAN failure to UTRAN */
      procedure_info.transaction_id = 0; /* transaction id unknown */
      procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_protocolError;
      procedure_info.protocol_error = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
      WRRC_MSG0_HIGH("CriticalExtensions not supported!");
      return (FALSE);
    }
  }

#ifdef FEATURE_QSH_EVENT_METRIC
  ho_gsm_params.freq_band = msg_ptr->u.r3.handoverFromUTRANCommand_GSM_r3.frequency_band;
  ho_gsm_params.ho_msg_type = msg_ptr->u.r3.handoverFromUTRANCommand_GSM_r3.gsm_message.t;
  rrc_qsh_log_ho_to_gsm_params(&ho_gsm_params);
#endif

  /*check if activation time is present and save the same in procedure_info*/
  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.handoverFromUTRANCommand_GSM_r3,
        rrc_HandoverFromUTRANCommand_GSM_r3_IEs,activationTime))
  {
    /*copy the activation time and set the flag to true*/
    procedure_info.activation_time =
      msg_ptr->u.r3.handoverFromUTRANCommand_GSM_r3.activationTime;
    procedure_info.act_time_present = TRUE;
  }
  else
  {
    /*else set activation time to false*/
    procedure_info.act_time_present = FALSE;
  }

  /* Check IEs in the command */
  /* If non-critical extensions are present, ignore for now */
  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3,
  	rrc_HandoverFromUTRANCommand_GSM_r3,laterNonCriticalExtensions))
  {
    /* Ignore Non-critical extension */
    WRRC_MSG0_HIGH("Ignore nonCriticalExtensions in CMD");
  }

  /* Extract the transaction identifier from the command */
  procedure_info.transaction_id =
    msg_ptr->u.r3.handoverFromUTRANCommand_GSM_r3.rrc_TransactionIdentifier;

  /* Fail handover if Ordered Config is set */
  if(OC_NOT_SET != rrcllc_get_ordered_config_status() 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
     || rrcdata_check_if_ota_is_buffered()
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    )
  {
    procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;
    return (FALSE);
  }

  /*-------------------------------------------------------------------------
   Ignore activation time: assume the activation time is NOW and ignore the
   RAB info for now
  -------------------------------------------------------------------------*/

  /* Extract frequency band */
  if (msg_ptr->u.r3.handoverFromUTRANCommand_GSM_r3.frequency_band == rrc_Frequency_Band_dcs1800BandUsed)
  {
    procedure_info.parms.gsm.frequency_band = RRC_RR_DCS_1800;
  }
  else
  {
    procedure_info.parms.gsm.frequency_band = RRC_RR_PCS_1900;
  }

  /* Extract GSM commands */
  if (msg_ptr->u.r3.handoverFromUTRANCommand_GSM_r3.gsm_message.t 
  	== T_rrc_HandoverFromUTRANCommand_GSM_r3_IEs_gsm_message_single_GSM_Message)
  {
    WRRC_MSG0_HIGH("Single GSM CMD received");

    /*if extracting single gsm message failed for any reason.. return failure*/
    if(!(rrciho_extract_single_gsm_message(cmd_ptr)))
    {
      return (FALSE);
    }
  }
  else
  {
    /* Extract the GSM command list: For handover, list should only contain 1 command */
    gsm_cmd_ct = 0;
    gsm_cmd_ptr = &msg_ptr->u.r3.handoverFromUTRANCommand_GSM_r3.gsm_message.u.gsm_MessageList.gsm_Messages;

    /* Check if these is no GSM command present */
    if ( (gsm_cmd_ptr == NULL ) ||gsm_cmd_ptr->n ==0 )
    {
      /* Set failure cause and return failure */
      procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_protocolError;
      procedure_info.protocol_error = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
      WRRC_MSG0_HIGH("GSM Command list is empty!");
      return (FALSE);
    }

    while( (gsm_cmd_ptr->n > gsm_cmd_ct) && (gsm_cmd_ct < RRC_RR_MAX_GSM_COMMANDS) )
    {
      /* Convert message length from the number of bits to number of bytes */
      procedure_info.parms.gsm.gsm_command_list.gsm_commands[gsm_cmd_ct].message_length =
        (uint8) (gsm_cmd_ptr->elem[gsm_cmd_ct].numbits / 8);

      /* Check if the command is lined up with byte boundary */
      if ((gsm_cmd_ptr->elem[gsm_cmd_ct].numbits % 8) != 0)
      {
        /* not in byte boundary: indicate a warning and continue the process */
        procedure_info.parms.gsm.gsm_command_list.gsm_commands[gsm_cmd_ct].message_length++;
        WRRC_MSG0_HIGH("GSM CMD is not on byte boundary");
      }

      /* Extract the GSM command */
      WCDMA_MEMCPY((procedure_info.parms.gsm.gsm_command_list.gsm_commands[gsm_cmd_ct].payload),
                   sizeof(uint8) * RRC_RR_MAX_GSM_MESSAGE_LENGTH,
                   gsm_cmd_ptr->elem[gsm_cmd_ct].data,
                   MIN( procedure_info.parms.gsm.gsm_command_list.gsm_commands[gsm_cmd_ct].message_length,
                            sizeof(uint8)*RRC_RR_MAX_SINGLE_GSM_MESSAGE_LENGTH ));

      /* Move to the next command */
      gsm_cmd_ct++;;

    }

    /* Set the command counter */
    WRRC_MSG2_HIGH("Received GSM CMDs: %d, MAX Allowed:%d", gsm_cmd_ct, RRC_RR_MAX_GSM_COMMANDS);
    procedure_info.parms.gsm.gsm_command_list.number_of_commands = gsm_cmd_ct;
  }

  return (TRUE);
} /* end of verify_and_extract_handover_from_utran_command */

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_UMTSMSM */

/*===========================================================================

FUNCTION   rrciho_event_log_handover_from_utran

DESCRIPTION

    Event Logging for HO from UTRAN command.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void          rrciho_event_log_handover_from_utran
(
       uint8         msg_type
         /* Handover message type.
         */
)
{
  /* declare an event log type for handover start*/
  rrclog_wcdma_inter_rat_handover_start_event_type start_event_log;

  /*update event type to wcdma_handover type to GSM.  GSM = 0, CDMA2000 = 1.
  */
  switch ( msg_type )
  {
    case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM:
      start_event_log.wcdma_handover_type = 0;
      break;

    case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_CDMA2000:
      start_event_log.wcdma_handover_type = 1;
      break;

    case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA:
      start_event_log.wcdma_handover_type = 2;
      break;

    default:
      WRRC_MSG1_ERROR("Unknown handover msg_type %d", msg_type);
      start_event_log.wcdma_handover_type = 0;
      break;
  }

  /* we do not support activation time.. will update this field when we support
   * activation time in Handover From Utran command
   */
  start_event_log.activation = 0x01;
  start_event_log.activation_cfn = 0x00;

  /*get current cfn from L1*/
  start_event_log.receive_cfn = seq_get_cfn();

  event_report_payload(EVENT_WCDMA_INTER_RAT_HANDOVER_START,
                       sizeof(start_event_log),
                       (void *)&start_event_log);
}/*rrciho_event_log_handover_from_utran*/


/*==============================================
FUNCTION rrciho_event_log_handover_from utran_end_event_indicator

DESCRIPTION

    End Event Logging for HO from UTRAN command.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

=========================================================
*/
static void  rrciho_event_log_handover_from_utran_end_event_indicator
(
         uint8 msg_type
         /* Handover message type.
         */,
         uint8 status,
         uint32 failure_cause
)
{
  /* declare an event log type for handover end*/
  rrclog_wcdma_inter_rat_handover_end_event_type end_event_log;
  end_event_log.handover_status = status;
  end_event_log.failure_cause = failure_cause;

  /*update event type to wcdma_handover type to GSM.  GSM = 0, CDMA2000 = 1.
  */
  switch ( msg_type )
  {
    case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM:
      end_event_log.handover_type = 0;
      break;

    case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_CDMA2000:
      end_event_log.handover_type = 1;
      break;

    case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA:
      end_event_log.handover_type = 2; 
      break;

    default:
      WRRC_MSG1_ERROR("Unknown handover msg_type %d", msg_type);
      end_event_log.handover_type = 0; 
      break;
  }
 
  event_report_payload(EVENT_WCDMA_INTER_RAT_HANDOVER_END,
                       sizeof(end_event_log),
                       (void *)&end_event_log);
}/*rrciho_event_log_handover_from_utran_failure*/



/*===========================================================================

FUNCTION   PROCESS_HANDOVER_FROM_UTRAN_COMMAND_GSM

DESCRIPTION

  This function verifies and processes the handover from UTRAN command. It
  initiates an inter-RAT handover procedure if it is required.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_handover_from_utran_command_gsm(
  rrc_DL_DCCH_Message *dcch_ptr,    /* Pointer to the DCCH message */
  rrc_cmd_type *cmd_ptr             /* Received command */
)
{
  rrc_HandoverFromUTRANCommand_GSM *msg_ptr;

  WRRC_MSG0_HIGH("Rvd Handover From UTRAN Command GSM");

  msg_ptr = &(dcch_ptr->message.u.handoverFromUTRANCommand_GSM);

  /* Verify the handover from UTRAN command */
  if (verify_and_extract_handover_from_utran_command(msg_ptr, cmd_ptr) == FALSE)
  {
    /* Send handover from UTRAN faliure */
    send_handover_from_utran_failure();
  }
  else
  {
    /* Check if RRC is in CELL_DCH state */
    if (rrc_get_state() != RRC_STATE_CELL_DCH)
    {
      /* RRC is not the correct state to process the command, send failure to UTRAN */
      procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_protocolError;
      procedure_info.protocol_error = rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState;
      send_handover_from_utran_failure();
      WRRC_MSG0_HIGH("UE is not in CELL_DCH");
      return;
    }

    /* Check if UE is dual-mode enabled */
    if (rrcmcm_is_dualmode_enabled() == FALSE)
    {
      /* UE is not enabled for dual-mode, send failure to UTRAN */
      procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_configurationUnacceptable;
      send_handover_from_utran_failure();
      WRRC_MSG0_HIGH("UE is not dual-mode enabled");
      return;
    }

    /* Check if GSM band is supported by UE */
    /* RR will provide a I/F function to verify this */

    /* Get inter-RAT measurement data from L1 */
    l1_get_interrat_measurement_data(&procedure_info.parms.gsm.interrat_measurement_data);

    /*Event logging for HO From UTRAN command*/
    rrciho_event_log_handover_from_utran( T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM );

   if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
   {
     rrccsp_cleanup_bplmn_state();
   }

    /* Request MCM to change the RRC mode from Active to Standby */
    send_change_mode_req(RRC_MODE_STANDBY);
    
    if(rrc_sib_is_current_event_sib_read_in_dch())
    {
      WRRC_MSG0_HIGH("Force SIB Init in DCH ");
      rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH_NO_PCCPCH_DROP);
    }

    /* Change the procedure state to wait for change mode confirmation */
    rrciho_state = RRCIHO_WAIT_STANDBY_CNF;
    return;
  }
} /* end of process_handover_from_utran_command_gsm*/

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION   rrciho_process_wtol_ps_handover_cnf

DESCRIPTION

  This function processes the WTOL PS handover confirm message from LRRC.

DEPENDENCIES

  None.

RETURN VALUE

  TRUE or FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
void static rrciho_process_wtol_ps_handover_cnf
(
  lte_rrc_wcdma_psho_rsp_s  *interrat_ho_cnf
)
{
  /* Check if the status is successful */
  if (interrat_ho_cnf->ho_status == LTE_RRC_IRAT_TO_LTE_PSHO_SUCCESS)
  {
    /* Abort Dormancy if IHO proceed to successful processing. */
    rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_IHO);
    rrciho_event_log_handover_from_utran_end_event_indicator(T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA,SUCCESS,RRCIHO_EVENT_FAILURE_CAUSE_NOT_VALID);

    /* Procedure ends */
    rrciho_init_data();
  }
  else
  {
#ifdef FEATURE_QSH_EVENT_METRIC
      wrrc_qsh_log_iho_stats(IRAT_MOB_FAIL, WRRC_METRIC_UPDATE);
#endif
    /* Inter-RAT handover failed: Resume the WCDMA mode and revert back to
       UTRAN configuration. */
    send_change_mode_req(RRC_MODE_ACTIVE);
  
    /* Set the failure cause */
    switch(interrat_ho_cnf->failure_cause)
    {
      /* Invalid Configuration */
      case LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_INVALID_CONFIG:
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_configurationUnacceptable;
        break;
  
      /* Invalid RR state or GSM protocol error */
      case LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_INVALID_STATE:
      case LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_PROTOCOL_ERROR:
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_interRAT_ProtocolError;
        break;
  
      /* Physical channel setup failure */
      case LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_ACQ_FAILURE:
      case LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_FREQ_NOT_IMPLEMENTED:
      case LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_RACH_FAILED:
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_physicalChannelFailure;
        break;

      default:
      /* Invalid failure cause */
        WRRC_MSG1_HIGH("Invalid Failure Cause for IHO:%d", interrat_ho_cnf->failure_cause);
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;
        break;
    }

    WRRC_MSG1_HIGH("WTOL: Failure Cause for IHO:%d", interrat_ho_cnf->failure_cause);
    rrciho_event_log_handover_from_utran_end_event_indicator(T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA,FAILURE,procedure_info.failure_cause);

    /* Set the procedure state to wait for change mode confirmation */
    rrciho_state = RRCIHO_WAIT_ACTIVE_CNF;
  }
  return;

}

/*===========================================================================

FUNCTION   VERIFY_AND_EXTRACT_HANDOVER_FROM_UTRAN_COMMAND_EUTRA

DESCRIPTION

  This function verifies the handover from UTRAN command and sets the failure
  cause properly if an error is detected. If the command is valid, it extracts
  necessary information from the command.

DEPENDENCIES

  None.

RETURN VALUE

  TRUE or FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
static boolean verify_and_extract_handover_from_utran_command_eutra
(
  rrc_HandoverFromUTRANCommand_EUTRA *msg_ptr
)
{
  uint16 container_size = 0, num_bytes = 0;

  procedure_info.handover_type = T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA;

  if(msg_ptr->criticalExtensions.t != T_rrc_HandoverFromUTRANCommand_EUTRA_criticalExtensions_1_r8)
  {
    /* Critical extension is not supported, send handover from
       UTRAN failure to UTRAN */
    procedure_info.transaction_id = 0; /* transaction id unknown */
    procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_protocolError;
    procedure_info.protocol_error = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    WRRC_MSG0_HIGH("CriticalExtensions not supported!");
    return (FALSE);
  }

  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->criticalExtensions.u.r8.handoverFromUTRANCommand_EUTRA_r8, activationTime))
  {
    /*copy the activation time and set the flag to true*/
    procedure_info.activation_time =
      (uint32)msg_ptr->criticalExtensions.u.r8.handoverFromUTRANCommand_EUTRA_r8.activationTime;
    procedure_info.act_time_present = TRUE;
  }
  else
  {
    /*else set activation time to false*/
    procedure_info.act_time_present = FALSE;
  }

  /* Extract the transaction identifier from the command */
  procedure_info.transaction_id = msg_ptr->rrc_TransactionIdentifier;

  /* Fail handover if Ordered Config is set */
  if(OC_NOT_SET != rrcllc_get_ordered_config_status() 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
     || rrcdata_check_if_ota_is_buffered()
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    )
  {
    procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;
    return (FALSE);
  }

  /* Store handoverFromUTRANCommand_EUTRA container as a DSM ptr */
  container_size = (uint16)msg_ptr->criticalExtensions.u.r8.handoverFromUTRANCommand_EUTRA_r8.eutra_Message.numocts;

  procedure_info.parms.eutra.dsm_ptr = NULL;
  num_bytes = dsm_pushdown (&procedure_info.parms.eutra.dsm_ptr, 
      (void *)msg_ptr->criticalExtensions.u.r8.handoverFromUTRANCommand_EUTRA_r8.eutra_Message.data, 
      container_size, 
      RRC_DSM_POOL_ID(container_size));

  if((num_bytes != container_size) || (procedure_info.parms.eutra.dsm_ptr == NULL))
  {
    WRRC_MSG1_HIGH("WTOL_PSHO: dsm_pushdown failed num_bytes: (%d), WTOL HO failed!", num_bytes);
    procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;
    return FALSE;
  }

  /* Store the container as it we need to sent it in HO failure message */
  procedure_info.parms.eutra.eutra_container_size = (uint32)container_size;
  procedure_info.parms.eutra.eutra_container_ptr = (byte *) rrc_malloc(container_size);
  WCDMA_MEMCPY(procedure_info.parms.eutra.eutra_container_ptr,
               container_size,
               msg_ptr->criticalExtensions.u.r8.handoverFromUTRANCommand_EUTRA_r8.eutra_Message.data,
               container_size );

  return TRUE;

}

/*===========================================================================

FUNCTION   RRCIHO_PROCESS_HANDOVER_FROM_UTRAN_COMMAND_EUTRA

DESCRIPTION

  This function verifies and processes the handover to EUTRAN from UTRAN
  command. It initiates an inter-RAT handover procedure if it is required.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_process_handover_from_utran_command_eutra
(
  rrc_DL_DCCH_Message    *dcch_ptr,
    /* Pointer to the DCCH message. */
  
  rrc_cmd_type		   *cmd_ptr
    /* Received command. */
)
{
  rrc_HandoverFromUTRANCommand_EUTRA *msg_ptr;
  
  WRRC_MSG0_HIGH("Rvd Handover From UTRAN Command EUTRA");
  
  msg_ptr = &(dcch_ptr->message.u.handoverFromUTRANCommand_EUTRA);
  
#ifdef FEATURE_QSH_EVENT_METRIC
  if(rrc_csfb_call_status)
  {
    wrrc_qsh_log_iho_stats(LTE_RAT_CSFB_MOB, WRRC_METRIC_UPDATE);
  }	   
#endif
  
  
  /* Verify the handover from UTRAN command */
  if (verify_and_extract_handover_from_utran_command_eutra(msg_ptr) == FALSE)
  {
    /* Send handover from UTRAN faliure */
    send_handover_from_utran_failure();
  }
  else
  {
    /* Check if RRC is in CELL_DCH state */
    if (rrc_get_state() != RRC_STATE_CELL_DCH)
    {
      /* RRC is not the correct state to process the command, send failure to UTRAN */
      procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_protocolError;
      procedure_info.protocol_error = rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState;
      send_handover_from_utran_failure();
      WRRC_MSG0_HIGH("UE is not in CELL_DCH");
      return;
    }
    
    /* Check if UE is dual-mode enabled */
    if (rrcmcm_is_lte_mode_enabled() == FALSE)
    {
      /* UE is not enabled for dual-mode, send failure to UTRAN */
      procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_configurationUnacceptable;
      send_handover_from_utran_failure();
      WRRC_MSG0_HIGH("UE is not dual-mode enabled");
      return;
    }

   if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
   {
     rrccsp_cleanup_bplmn_state();
   }
    /* Request MCM to change the RRC mode from Active to Standby */
    send_change_mode_req(RRC_MODE_STANDBY);

    if(rrc_sib_is_current_event_sib_read_in_dch())
    {
      WRRC_MSG0_HIGH("Force SIB Init in DCH ");
      rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH_NO_PCCPCH_DROP);
    }
 
    /* Change the procedure state to wait for change mode confirmation */
    rrciho_state = RRCIHO_WAIT_STANDBY_CNF;
    return;
  }
}
#endif

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif

/*===========================================================================

FUNCTION   PROCESS_INTERRAT_HANDOVER_CNF

DESCRIPTION

  This function processes the interrat handover confirmation received from the
  GSM RR and initiates the proper actions.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_interrat_handover_cnf(
  rr_interrat_handover_cnf_type *interrat_ho_cnf /* Received command */
)
{
  uint8 number_of_commands;
  uint8 idx =0;
  rrc_GSM_MessageList *gsm_command_ptr = NULL;

  /* Check if the status is successful */
  if (interrat_ho_cnf->handover_status == RRC_RR_SUCCESS)
  {
    rrcsmc_copy_pending_keys_to_saved_security_key_set();
    /* Abort Dormancy if IHO proceed to successful processing. */
    rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_IHO);
    /* Interrat handover succeeded: inform DT to forward bufferred NAS signaling
       messages to the GSM RR */
    send_nas_data_transfer_ind();
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB 
    csfb_extended_status = CSFB_INVALID;
#endif
    /* Procedure ends */
    rrciho_init_data();
  }
  else
  {
#ifdef FEATURE_QSH_EVENT_METRIC
      wrrc_qsh_log_iho_stats(IRAT_MOB_FAIL, WRRC_METRIC_UPDATE);
#endif
#ifdef FEATURE_INTER_RAT_HO_OPT
    if( rrc_is_voice_rab_present() 
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        &&  (mn_has_nas_already_acquired_voc_subs(rrc_get_as_id()) == TRUE)
#else
        &&  (mn_has_nas_already_acquired_voc() == TRUE)
#endif
      )
    {
      /* Release MVS client of GSM */
      WRRC_MSG0_HIGH("Releasing MVS Client(G) and (W) during W->G handover failure");
      ghdi_mvs_release(MVS_CLIENT_GSM);

      /* Release MVS client of WCDMA */
      ghdi_mvs_release(MVS_CLIENT_WCDMA);
    }
#endif

    /* Inter-RAT handover failed: Resume the WCDMA mode and revert back to
       UTRAN configuration. */
    send_change_mode_req(RRC_MODE_ACTIVE);

    /* Set the failure cause */
    switch(interrat_ho_cnf->failure_cause)
    {
      /* Invalid Configuration */
      case RRC_RR_INVALID_CONFIGURATION:
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_configurationUnacceptable;
        break;

    /* Invalid RR state or GSM protocol error */
    case RRC_RR_INVALID_STATE:
    case RRC_RR_GSM_PROTOCOL_ERROR:
      procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_interRAT_ProtocolError;
      /* Save the inter-RAT message */
      number_of_commands = interrat_ho_cnf->gsm_command_list.number_of_commands;
      idx = 0;
      if (number_of_commands > 0)
      {
        gsm_commands.elem = rrc_malloc(number_of_commands* sizeof(rrc_GSM_MessageList_element));
          /* Extract and save original GSM commands */
        gsm_command_ptr = &gsm_commands;

        /*if more than one command is present then scroll thru the list of messages and add it to the
               gsm message list*/
        while ((idx < number_of_commands) && (idx < RRC_RR_MAX_GSM_COMMANDS))
        {
          /*set the length in bits*/
          gsm_command_ptr->elem[idx].numbits = (MIN((interrat_ho_cnf->gsm_command_list.gsm_commands[idx].message_length),RRC_RR_MAX_SINGLE_GSM_MESSAGE_LENGTH))*8;

          WCDMA_MEMCPY(gsm_command_ptr->elem[idx].data,
                       sizeof(OSOCTET) * 64,
                       interrat_ho_cnf->gsm_command_list.gsm_commands[idx].payload,
                       gsm_command_ptr->elem[idx].numbits/8);
          idx++;
        }
        gsm_command_ptr->n = number_of_commands;
    }
    
    break;

     /* Physical channel setup failure */
      case RRC_RR_PHYSICAL_CHANNEL_FAILURE:
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_physicalChannelFailure;
        break;

      default:
        /* Invalid failure cause */
        WRRC_MSG1_HIGH("Invalid Failure Cause for IHO:%d", interrat_ho_cnf->failure_cause);
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;
        break;
    }

    /* Set the procedure state to wait for change mode confirmation */
    rrciho_state = RRCIHO_WAIT_ACTIVE_CNF;
  }
  return;

} /* end of process_interrat_handover_cnf */

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif


#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION   PROCESS_HANDOVER_TO_UTRAN_COMMAND

DESCRIPTION

  This function processes the handover to UTRAN command. It
  decodes the HandoverToUTRANCommand and checks all the IE's
  and starts the process of acquiring a WCDMA cell.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_handover_to_utran_command_from_eutran(
  rrc_cmd_type * cmd_ptr
)
{

  byte*  ho_msg_ptr = NULL;                     /* ptr to encoded HO msg     */
  wcdma_rrc_lte_psho_req_type *lte_psho_req = 
    &cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.wcdma_rrc_lte_psho_req;

  dsm_item_type *dsm_ptr;
  msgr_attach_struct_type *msgr_attach_ptr;
  uint16 dsm_pkt_len;
#ifdef FEATURE_DUAL_SIM
  wrm_freq_input_type lock_req_bands;
#endif
  WRRC_MSG0_HIGH("LTOW_PSHO:Rcvd HO To UTRAN from EUTRAN");

  iho_interrat_type = RRCIHO_INTERRAT_LTE;

  /*reset any previous saved security keys */
  memset(gtow_info.psho_security_keys.ck, 0, 
         LTE_IRAT_MAX_CK_LENGTH);
  memset(gtow_info.psho_security_keys.ik, 0, 
         LTE_IRAT_MAX_IK_LENGTH);  
  memset(gtow_info.psho_security_keys.ck_srvcc, 0, 
         LTE_IRAT_MAX_CK_LENGTH);
  memset(gtow_info.psho_security_keys.ik_srvcc, 0, 
         LTE_IRAT_MAX_IK_LENGTH);  

  /*init the default config here.. we have to do this for every call
    because the spoof poke byte can be set at any time*/
  rrciho_init_default_configuration();

#ifdef FEATURE_RAT_PRIORITY_LIST
  rrc_csp_update_rat_pri_list_info(&lte_psho_req->rat_pri_list_info);
#endif

  gtow_info.network_select_mode = lte_psho_req->network_select_mode;

  /* Extract HO continer from DSM pointer */
  msgr_attach_ptr = msgr_get_attach((msgr_hdr_struct_type *)&lte_psho_req->msg_hdr, 0);
  ASSERT(msgr_attach_ptr != NULL);
  msgr_get_dsm_attach(msgr_attach_ptr, &dsm_ptr);
  ASSERT(dsm_ptr != NULL);
    
  dsm_pkt_len = dsm_length_packet(dsm_ptr);
  WRRC_MSG1_HIGH("LTOW:HO container len %d ",dsm_pkt_len);
    
  gtow_info.ho_msg_len = dsm_pkt_len;
  ho_msg_ptr = (byte *) rrc_malloc(dsm_pkt_len);
  ASSERT(ho_msg_ptr != NULL);
  
  ASSERT(dsm_pullup(&dsm_ptr, ho_msg_ptr, dsm_pkt_len) == dsm_pkt_len);

  /*get the ptr to the byte stream of the HandoverToUTRANCommand*/
  gtow_info.ho_ptr =
      (rrc_HandoverToUTRANCommand *) rrcasn1_decode_pdu(  (void *)ho_msg_ptr,
                                                          rrc_HandoverToUTRANCommand_PDU,
                                                          gtow_info.ho_msg_len
                                                          );
/* Save GtoW Utran message with type RRC_HO_TO_UTRAN  */
  rrc_crash_debug_save_ota(ho_msg_ptr, gtow_info.ho_msg_len,RRC_HO_TO_UTRAN, 0xFF,RRC_PROCEDURE_IHO);
#ifdef FEATURE_LTE_TO_WCDMA
          gtow_info.cs_ho_is_supported = lte_psho_req->cs_ho_is_supported;
          gtow_info.ps_ho_is_supported = lte_psho_req->ps_ho_is_supported;
#endif

  if(ho_msg_ptr != NULL)
  {
    rrc_free(ho_msg_ptr);
  }

  if(gtow_info.ho_ptr == NULL)
  {
    /*unable to successfully decode a asn1 PDU, set failure cause to
      Protocol_Error
     */
    procedure_info.failure_cause = (uint32)RR_RRC_WCDMA_PROTOCOL_ERROR;

    /*set to flag that HO to UTRAN msg ptr is not present*/
    gtow_info.ho_msg_present = FALSE;

    WRRC_MSG0_ERROR("Unable to decode HO to Utran Msg");

    /*send a handover to utran failure and reset IHO procedure's state
      machine*/
    rrciho_send_ltow_ps_handover_to_utran_failure();
    return;
  }
  else
  {
    /*set the flag to indicate that HO to UTRAN msg is present*/
    gtow_info.ho_msg_present = TRUE;
  }

  /* Verify the handover from UTRAN command */
  if (verify_and_extract_handover_to_utran_command(gtow_info.ho_ptr
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
                           , mmumts_convert_nas_plmn_id_to_rrc_plmn_id(
                                  lte_psho_req->plmn_id)
#endif
      ) == FAILURE)
  {
    /*send a handover to utran failure and reset IHO procedure's state
      machine*/
    rrciho_send_ltow_ps_handover_to_utran_failure();

    rrc_free_dynamic_ptrs();
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
    rrc_perform_trm_exchange();
    rrc_wrm_populate_bands_for_lock_req(&lock_req_bands, gtow_info.freq);

  /* Even after WCDMA has received the lock, call the below API to update WRRC client lock status 
     For non CSFB, acquire lock here */
  if(rrciho_get_lock_for_iho(lock_req_bands) == FAILURE)
  {
    return;
  }
#endif

#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
    /*Send L2W SRVCC started to CM*/
    if(gtow_info.is_cs_rab_present)
    {
#ifdef FEATURE_QSH_EVENT_METRIC
      wrrc_qsh_log_iho_stats(LTE_RAT_SRVCC_MOB, WRRC_METRIC_UPDATE);				
#endif
      rrc_send_hho_cmd_to_cm(SYS_HO_START,SYS_HO_TYPE_LTE_UMTS_SRVCC);      
      rrc_send_codec_info_to_cm(rrc_get_ordered_codec_mode());
    }
#endif

    WCDMA_MEMCPY(gtow_info.psho_security_keys.ck,
                 sizeof(uint8) * LTE_IRAT_MAX_CK_LENGTH,
                 lte_psho_req->security_keys.ck,
                 LTE_IRAT_MAX_CK_LENGTH );
    WCDMA_MEMCPY(gtow_info.psho_security_keys.ik,
                 sizeof(uint8) * LTE_IRAT_MAX_IK_LENGTH,
                 lte_psho_req->security_keys.ik,
                 LTE_IRAT_MAX_IK_LENGTH );
    WCDMA_MEMCPY(gtow_info.psho_security_keys.ck_srvcc,
                 sizeof(uint8) * LTE_IRAT_MAX_CK_LENGTH,
                 lte_psho_req->security_keys.ck_srvcc,
                 LTE_IRAT_MAX_CK_LENGTH );
    WCDMA_MEMCPY(gtow_info.psho_security_keys.ik_srvcc,
                 sizeof(uint8) * LTE_IRAT_MAX_IK_LENGTH,
                 lte_psho_req->security_keys.ik_srvcc,
                 LTE_IRAT_MAX_IK_LENGTH );

    /*process measurement data received in the handover to utran command*/
    process_meas_data_for_handover_to_utran_command(&lte_psho_req->interrat_measurement_data);


    /* Request MCM to change the RRC mode Inactive to Interrat in Progress */
    send_change_mode_req(RRC_MODE_INTERRAT_IN_PROGRESS);

    /* Change the procedure state to wait for change mode confirmation */
    rrciho_state = RRCIHO_WAIT_IRAT_IN_PROGRESS_CNF;
  }

  /*Set cipher status depending on cipher algo present or not and whether cipher NV is set*/
  if(gtow_info.ciph_algo_present && nv_ciphering_enabled)
  {
    gtow_info.gsm_ciph_status = TRUE;
  }
  else
  {
    gtow_info.gsm_ciph_status = FALSE;
  }

#ifdef FEATURE_WCDMA_TO_LTE
  if(rrcmcm_is_lte_mode_enabled() &&
#ifdef LTE_BAND_NUM
     (rrccsp_check_if_lte_band_pref_is_set(rrccsp_get_lte_band_pref()) == TRUE))
#else
     (rrccsp_get_lte_band_pref() != 0))
#endif
  {
    rrcueci_send_eutra_capabilitites_req_to_lte();
  }
#endif

  return;
} /* end of process_handover_to_utran_command*/

#endif

/*===========================================================================

FUNCTION   RRCIHO_INITIAL_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the IHO is in the RRCIHO_INITIAL substate. This function
  looks at the received event and does the appropriate processing
  of the event.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_initial_substate_event_handler(
  rrc_cmd_type *cmd_ptr /* Received command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr;      /* Pointer to the DCCH message */
  rrc_CellUpdateCause       cu_cause;  /*To store CU cause from CU procedure*/

  /* Check the type of the command received */
  if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_INTERRAT_HANDOVER_REQ)
  {
     /* Received an Interrat Handover Request from RR to HO to WCDMA*/
      WRRC_MSG0_HIGH("Handover to WCDMA rcvd.");
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
      /*Send G2W started to CM*/
rrc_send_hho_cmd_to_cm(SYS_HO_START,SYS_HO_TYPE_GSM_UMTS);
#endif
#ifdef FEATURE_GAN_3G
      #error code not present
#endif
/*lint -e740*/
      /* Process the DCCH message: Handover From UTRAN Command */
      process_handover_to_utran_command( (rrc_rr_cmd_type *)cmd_ptr);
/*lint +e740*/
  }
  else if( ((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_INTERRAT_HANDOVER_ABORT_REQ) &&
           (rrcmcm_get_rrc_mode() == RRC_MODE_INTERRAT_IN_PROGRESS))
  {
    WRRC_MSG0_HIGH("Rx: RRC_INTERRAT_HANDOVER_ABORT_REQ after sending HO_SUCCESS_RSP, while waiting for the RRC_NAS_DATA_IND from GSM /RRC_ACTIVATION_RSP from NAS");
    rrciho_abort = TRUE; /* Set abort to true */
    send_change_mode_req(RRC_MODE_INACTIVE);
    procedure_info.failure_cause = (uint32)RR_RRC_INVALID_STATE;
    rrciho_state = RRCIHO_WAIT_INACTIVE_CNF;
  }
  else
  {
    switch( cmd_ptr->cmd_hdr.cmd_id )
    {
    case RRC_DOWNLINK_SDU_IND:
      /* Get the downlink message from the SDU */
      dcch_msg_ptr=(rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
      procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;
      /* Check if the ASN1 decoding was successful */
      if( NULL == dcch_msg_ptr )
      {
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_protocolError;
        procedure_info.protocol_error = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
        send_handover_from_utran_failure();
        WRRC_MSG0_HIGH("Handover to UTRAN failed because of ASN1 decoding error ");
        return;
      }
      else if (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED) 
      {
        if(rrc_CellUpdateCause_periodicalCellUpdate != cu_cause)
        {
          procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;
          procedure_info.protocol_error = rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState;
          send_handover_from_utran_failure();
          return;
        }
      }
      else if(rrcrce_send_abort_cnf)
      {
        WRRC_MSG0_HIGH("ABORT is in progress, send IHO failure to the NW");
        procedure_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;
        procedure_info.protocol_error = rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState;
        send_handover_from_utran_failure();
        return;
      }
      /* Process the DCCH message: Handover From UTRAN Command.
      */
      switch ( dcch_msg_ptr->message.t )
      {
        case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM:
#ifdef FEATURE_QSH_EVENT_METRIC
	      wrrc_qsh_log_iho_stats(GSM_RAT_MOB, WRRC_METRIC_UPDATE);
#endif
          process_handover_from_utran_command_gsm(dcch_msg_ptr, cmd_ptr);
          break;

        #ifdef FEATURE_WCDMA_TO_LTE
        case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA:
#ifdef FEATURE_QSH_EVENT_METRIC
	      wrrc_qsh_log_iho_stats(LTE_RAT_MOB, WRRC_METRIC_UPDATE);			
#endif
          rrciho_process_handover_from_utran_command_eutra(dcch_msg_ptr, cmd_ptr);
          break;
        #endif

        #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
        #error code not present
#endif

        default:
          WRRC_MSG1_ERROR("Invalid handover command %d", dcch_msg_ptr->message.t);
          break;

      } /* switch ( dcch_msg_ptr->message.t ) */
      break;

    case RRC_STATE_CHANGE_IND:

#ifdef FEATURE_INTER_RAT_HO_OPT
      /* Cleanup */
      rrciho_cs_voice_rab_released();
#endif

      /* Ignore the state change indication at the initial state */
      MSG_MED("Ignoring state change in IHO initial substate", 0, 0, 0);
      break;

    case RRC_ABORT_REQ:
      /* IHO is not active, forward the command to RCE for processing */
      rrcrce_procedure_event_handler( cmd_ptr );
      break;

/*lint -e408*/
#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_LTE_RRC_CMD:
      if(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == 
          WCDMA_RRC_LTE_PSHO_REQ)
      {
        process_handover_to_utran_command_from_eutran(cmd_ptr);
      }
      if( (cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == WCDMA_RRC_LTE_ABORT_PSHO_REQ) &&
          (rrcmcm_get_rrc_mode() == RRC_MODE_INTERRAT_IN_PROGRESS))
      {
        WRRC_MSG0_HIGH("Rx: WCDMA_RRC_LTE_ABORT_PSHO_REQ after sending HO_SUCCESS_RSP, while waiting for the RRC_ACTIVATION_RSP from NAS");
        rrciho_abort = TRUE; /* Set abort to true */
        send_change_mode_req(RRC_MODE_INACTIVE);
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_STATE;
        rrciho_state = RRCIHO_WAIT_INACTIVE_CNF;
      }
      else if((cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == WCDMA_RRC_LTE_ABORT_PSHO_REQ) &&
              (rrcmcm_get_rrc_mode() == RRC_MODE_INACTIVE))
      {
        rrciho_send_ltow_ps_handover_to_utran_abort_cnf();
      }
      break;
#endif

    default:
      /* umexpected enevts */
      WRRC_MSG2_ERROR("Unexpected IHO CMD %x in substate %d", cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
    }
  }
  return;
}



/*===========================================================================

FUNCTION   RRCIHO_WAIT_STANDBY_CNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the IHO procedure is in the RRCMCM_WAIT_STANDBY_CNF substate.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_standby_cnf_substate_event_handler(
  rrc_cmd_type *cmd_ptr /* Received command */
)
{
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    /* Change mode confirmation from the MCM */
    case RRC_CHANGE_MODE_CNF:
      /* Check if abort is received */
      if (rrciho_abort == TRUE)
      {
#ifdef FEATURE_QSH_EVENT_METRIC
         wrrc_qsh_log_iho_stats(IRAT_MOB_FAIL, WRRC_METRIC_UPDATE);
#endif
        /* Resume the WCDMA operation */
        send_change_mode_req(RRC_MODE_ACTIVE);

        /* Set the procedure state to wait for change mode confirmation */
        rrciho_state = RRCIHO_WAIT_ACTIVE_CNF;
      }
      else
      {
        /* Get inter-RAT measurement data from L1 */
        /* l1_get_interrat_measurement_data(&procedure_info.parms.gsm.interrat_measurement_data); */
        /* procedure_info.parms.gsm.interrat_measurement_data.number_of_cells = 0; */

        /* Initiate inter-RAT handover.
        */

#ifdef FEATURE_DUAL_SIM
        rrc_check_and_perform_trm_exchange();
        rrc_release_lock();
#endif

        send_interrat_handover_req();

        /* Change the procedure state to wait for handover confirmation */
        rrciho_state = RRCIHO_WAIT_INTERRAT_HO_CNF;
      }
      break;

    case RRC_STATE_CHANGE_IND:

#ifdef FEATURE_INTER_RAT_HO_OPT
      /* Cleanup */
      rrciho_cs_voice_rab_released();
#endif

      /* RRC state change out of DCH indication is received */
      WRRC_MSG1_HIGH("RRC State Change to IDLE in %d", rrciho_state);
      rrciho_init_data();

      break;

    case RRC_ABORT_REQ:
      /* Set abort to true */
      rrciho_abort = TRUE;
      if(rrc_abort_req_ptr != NULL)
      {
        rrc_free(rrc_abort_req_ptr);
      }
      rrc_abort_req_ptr = (rrc_abort_req_type *)rrc_malloc(sizeof(rrc_abort_req_type));
      WCDMA_MEMCPY(rrc_abort_req_ptr,
                   sizeof(rrc_abort_req_type),
                   &(cmd_ptr->cmd.rrc_abort_req),
                   sizeof(rrc_abort_req_type));
      break;

    default:
      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",
           cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
  }
  return;
}




/*===========================================================================

FUNCTION   RRCIHO_WAIT_INTERRAT_HO_CNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the procedure is in the Wait for inter-RAT handover Confirmation substate.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_interrat_ho_cnf_substate_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  /* Switch on the received command. */
   /* Inter-RAT handover confirmation from the GSM RR */
  if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RR_INTERRAT_HANDOVER_CNF)
  {
     /* Process the interrat handover confirmation command */
/*lint -e740*/
      WRRC_MSG0_HIGH("Inter-RAT HO CNF received from RR");
      process_interrat_handover_cnf(&(((rrc_rr_cmd_type *)cmd_ptr)->cmd.interrat_handover_cnf));
/*lint +e740*/
  }
  else
  {
   switch( cmd_ptr->cmd_hdr.cmd_id )
   {

#ifdef FEATURE_WCDMA_TO_LTE
    case RRC_LTE_RRC_CMD:
      if(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == LTE_RRC_WCDMA_PSHO_RSP)
      {
        WRRC_MSG1_HIGH("WTOL: Inter-RAT HO CNF received from LRRC with status: %d", 
                  cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.lte_rrc_wcdma_psho_rsp.ho_status);
        rrciho_process_wtol_ps_handover_cnf(&(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.lte_rrc_wcdma_psho_rsp));
      }
      break;
#endif

    #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#endif

    case RRC_STATE_CHANGE_IND:

#ifdef FEATURE_INTER_RAT_HO_OPT
      /* Cleanup */
      rrciho_cs_voice_rab_released();
#endif

      /* RRC state change to IDLE indication is received */
      WRRC_MSG1_HIGH("RRC State Change to IDLE in %d", rrciho_state);
      rrciho_init_data();

      break;

    case RRC_ABORT_REQ:
      /* Request RR to abort the inter-RAT handover.
      */
      rrciho_abort = TRUE;
      if(rrc_abort_req_ptr != NULL)
      {
        rrc_free(rrc_abort_req_ptr);
      }
      rrc_abort_req_ptr = (rrc_abort_req_type *)rrc_malloc(sizeof(rrc_abort_req_type));
      WCDMA_MEMCPY(rrc_abort_req_ptr,
                   sizeof(rrc_abort_req_type),
                   &(cmd_ptr->cmd.rrc_abort_req),
                   sizeof(rrc_abort_req_type));
      switch ( procedure_info.handover_type )
      {
        case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM:
          send_abort_handover_req();
          rrciho_state = RRCIHO_WAIT_ABORT_CNF;
          break;

#ifdef FEATURE_WCDMA_TO_LTE
        case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA:
          rrciho_send_abort_handover_to_lte();
          rrciho_state = RRCIHO_WAIT_ABORT_CNF;
          break;
#endif

        #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
        #error code not present
#endif
        default:
          /* Resume WCDMA operation.
          */
          send_change_mode_req(RRC_MODE_ACTIVE);

          /* Set the procedure state to wait for change mode confirmation.
          */
          rrciho_state = RRCIHO_WAIT_ACTIVE_CNF;
          break;
      } /* switch() */
      break;

    default:
      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",
           cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
   }
  }
  return;
}



/*===========================================================================

FUNCTION   RRCIHO_WAIT_ACTIVE_CNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the procedure is in the Wait for active Confirmation substate.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_active_cnf_substate_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  rrc_cmd_type *int_cmd_ptr;  //cmd ptr to send initiate_Cell_update_req

  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    /* Inter-RAT handover confirmation from the GSM RR */
    case RRC_CHANGE_MODE_CNF:
      MSG_MED("Change Mode CNF received from MCM", 0, 0, 0);

      if( cmd_ptr->cmd.change_mode_cnf.status == TRUE )
      {
        //if we are resuming, and voice rab's are present in est-rabs, then we need to indicate
        //to l1 of a change in power-class, if the feature is defined
        if(TRUE == rrc_is_voice_rab_present())
        {
          //call wrapper fn. that calls L1 to lower it's pwr class to 4 for voice
          rrc_indicate_layer1_next_config_is_for_voice();
        }
        /* Send Channel Config Request to LLC to revert back to UTRAN configuration */
        send_channel_config_req_to_llc();

        /* Set procedure state to wait for Channel Config Confirmation */
        rrciho_state = RRCIHO_WAIT_CHANNEL_CONFIG_CNF;
      }
      else /* change mode failure */
      {
        WRRC_MSG0_HIGH("Rx: RRC_CHANGE_MODE_CNF (Failure) ");

        if (rrciho_abort == TRUE)
        {
          rrc_cmd_type *abort_cmd_local_ptr = NULL;
          boolean hard_abort = FALSE;
          abort_cmd_local_ptr = (rrc_cmd_type *)rrc_malloc(sizeof(rrc_cmd_type));

          /* Invoke RCE to process the RRC_ABORT_REQ */
          abort_cmd_local_ptr->cmd_hdr.cmd_id = RRC_ABORT_REQ;
          WCDMA_MEMCPY(&abort_cmd_local_ptr->cmd.rrc_abort_req,
                       sizeof(rrc_abort_req_type),
                       rrc_abort_req_ptr,
                       sizeof(rrc_abort_req_type));
          hard_abort = rrc_abort_req_ptr->hard_abort;
          rrcrce_procedure_event_handler(abort_cmd_local_ptr);

          rrc_free(abort_cmd_local_ptr);
          rrc_free(rrc_abort_req_ptr);
          if(hard_abort)
          {
            WRRC_MSG0_HIGH("Hard abort is sent by NAS, so do not attempt recovery");
           /* Reset the procedure data, the procedure ends */
            rrciho_init_data();
            return;
          }
        }

        /* Initiate a cell update with Radio link failure */
        if ( (int_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
        {
          int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
          int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_RADIO_LINK_FAILURE;
          int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
          int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_IHO;

          WRRC_MSG0_HIGH("CELL_UPDATE_INITIATE_REQ sent by IHO on cphy resume failure");
          rrc_put_int_cmd(int_cmd_ptr);

          rrciho_state = RRCIHO_WAIT_CELL_UPDATE_CNF;
        }
      }
      break;

    case RRC_STATE_CHANGE_IND:

#ifdef FEATURE_INTER_RAT_HO_OPT
      /* Cleanup */
      rrciho_cs_voice_rab_released();
#endif

      /* RRC state change to IDLE indication is received */
      WRRC_MSG1_HIGH("RRC State Change to IDLE in %d", rrciho_state);
      rrciho_init_data();

      break;

    case RRC_ABORT_REQ:
      /* Set abort indicator to TRUE */
      rrciho_abort = TRUE;
      if(rrc_abort_req_ptr != NULL)
      {
        rrc_free(rrc_abort_req_ptr);
      }
      rrc_abort_req_ptr = (rrc_abort_req_type *)rrc_malloc(sizeof(rrc_abort_req_type));
      WCDMA_MEMCPY(rrc_abort_req_ptr,
                   sizeof(rrc_abort_req_type),
                   &(cmd_ptr->cmd.rrc_abort_req),
                   sizeof(rrc_abort_req_type));
      break;

    default:
      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",
           cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
  }
  return;
} /* end of rrciho_wait_active_cnf_substate_event_handler */



/*===========================================================================

FUNCTION   RRCIHO_WAIT_ABORT_CNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the procedure is in the Wait for abort Confirmation substate.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_abort_cnf_substate_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  /* Switch on the received command. */
  /* Inter-RAT handover confirmation from the GSM RR */
  if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RR_INTERRAT_HANDOVER_ABORT_CNF)
  {
     WRRC_MSG0_HIGH("ABORT_CNF Rvd from RR");
      /* Resume WCDMA operation */
      send_change_mode_req(RRC_MODE_ACTIVE);

      /* Set the procedure state to wait for change mode confirmation */
      rrciho_state = RRCIHO_WAIT_ACTIVE_CNF;
  }
  else
  {
   switch( cmd_ptr->cmd_hdr.cmd_id )
   {
#ifdef FEATURE_WCDMA_TO_LTE
    case RRC_LTE_RRC_CMD:
      if(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == LTE_RRC_WCDMA_ABORT_PSHO_RSP)
      {
        WRRC_MSG0_HIGH("WTOL: ABORT_CNF Rvd from LTE-RRC");
        /* Resume WCDMA operation */
        send_change_mode_req(RRC_MODE_ACTIVE);
        
        /* Set the procedure state to wait for change mode confirmation */
        rrciho_state = RRCIHO_WAIT_ACTIVE_CNF;
      }
      break;
#endif

    case RRC_STATE_CHANGE_IND:
#ifdef FEATURE_INTER_RAT_HO_OPT
      /* Cleanup */
      rrciho_cs_voice_rab_released();
#endif

      /* RRC state change to IDLE indication is received */
      WRRC_MSG1_HIGH("RRC State Change to IDLE in %d", rrciho_state);
      rrciho_init_data();

      break;

    default:
      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",
             cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
   }
  }
  return;
} /* end of rrciho_wait_abort_cnf_substate_event_handler */


/*===========================================================================

FUNCTION   RRCIHO_WAIT_CHANNEL_CONFIG_CNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the procedure is in the Wait for channel configuration confirmation substate.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_channel_config_cnf_substate_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  rrc_cmd_type *int_cmd_ptr;

  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    /* RRC_CHANNEL_CONFIG_CNF */
    case RRC_CHANNEL_CONFIG_CNF:
      WRRC_MSG0_HIGH("Rx: RRC_CHANNEL_CONFIG_CNF");
      rrcllcpcie_initialise_sync_a_post_veri_info();

#ifdef FEATURE_WCDMA_TO_LTE
      if(procedure_info.handover_type != T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA)
#endif
      {
/* We haven't checked whether Channel Config Req is sucessful or not yet RRC
is acquiring Vocoder and doing MVS configuration. Not sure whether it's as per design.
But same way, I'm also configuring MVS for CSoHSPA and sending PDCP_CSOHS_HANDOVER_END to PDCP.*/

#ifdef FEATURE_INTER_RAT_HO_OPT
      if( rrc_is_voice_rab_present() )
      {
        /* Set the amr parameters */
#ifdef FEATURE_VOC_AMR_WB
        rrciho_process_channel_config_cnf(rrc_active_codec);
#else /*FEATURE_VOC_AMR_WB*/
        rrciho_process_channel_config_cnf();
#endif /*FEATURE_VOC_AMR_WB*/
        WRRC_MSG0_HIGH("AMR settings complete after W->G handover failure");
        
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        if(mn_has_nas_already_acquired_voc_subs(rrc_get_as_id()) == TRUE)
#else
        if(mn_has_nas_already_acquired_voc() == TRUE)
#endif
        {
          /* Set the control flag to true to indicate that we are taking control of the vocoder */
          rrciho_mvs_control_flag = TRUE;
          /* Now try to acquire W */
          WRRC_MSG0_HIGH("Acquiring MVS_CLIENT(W) during W->G handover failure");
          ghdi_mvs_acquire(MVS_CLIENT_WCDMA);
        }
      }
#endif /*FEATURE_INTER_RAT_HO_OPT*/
      }

      /* Check the status of the channel config confirmation */
      if (cmd_ptr->cmd.chan_config_cnf.chan_cfg_status == RRCLLC_CHAN_CFG_SUCCESS)
      {
        WRRC_MSG0_HIGH("Rx: RRC_CHANNEL_CONFIG_CNF (Success) ");

        /* Copy the Ordered RB Mapping into the current RB-Mapping database*/
        rrcllc_copy_ordered_hanging_rb_mapping_to_current();

        /* Revert back to UTRAN is successful, send Handover from UTRAN failure */
        send_handover_from_utran_failure();
        
#if defined (FEATURE_DUAL_SIM) && defined (FEATURE_WCDMA_TO_LTE)
        if(procedure_info.handover_type == T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA)
        {
          wrm_retain_lock_for_primary();
        }
#endif

#if defined (FEATURE_GAN_3G) || defined (FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE)
        #error code not present
#endif

        /* Check if Abort is received */
        if (rrciho_abort == TRUE)
        {
          rrc_cmd_type *abort_cmd_local_ptr = NULL;
          abort_cmd_local_ptr = (rrc_cmd_type *)rrc_malloc(sizeof(rrc_cmd_type));

          /* Invoke RCE to process the RRC_ABORT_REQ */
          abort_cmd_local_ptr->cmd_hdr.cmd_id = RRC_ABORT_REQ;
          WCDMA_MEMCPY(&abort_cmd_local_ptr->cmd.rrc_abort_req,
                       sizeof(rrc_abort_req_type),
                       rrc_abort_req_ptr,
                       sizeof(rrc_abort_req_type));
          rrcrce_procedure_event_handler(abort_cmd_local_ptr);
          WRRC_MSG0_HIGH("Channel config success,ABORT Rvd,forwarding it to RCE");

          rrc_free(abort_cmd_local_ptr);
          rrc_free(rrc_abort_req_ptr);
        }

        /*check if any command is buffered and trigger the indication*/
        rrccu_post_buffered_commands();
        
        /* Reset the procedure data, the procedure ends */
        rrciho_init_data();
      }
      else // Reverting to UTRAN failure
      {
        WRRC_MSG0_HIGH("Rx: RRC_CHANNEL_CONFIG_CNF (Failure) ");
        /* If there is a pending abort,forward it to RCE and end the procedure */
        if (rrciho_abort == TRUE)
        {
          rrc_cmd_type *abort_cmd_local_ptr = NULL;
          boolean hard_abort = FALSE;
          abort_cmd_local_ptr = (rrc_cmd_type *)rrc_malloc(sizeof(rrc_cmd_type));

          /* Invoke RCE to process the RRC_ABORT_REQ */
          abort_cmd_local_ptr->cmd_hdr.cmd_id = RRC_ABORT_REQ;
          WCDMA_MEMCPY(&abort_cmd_local_ptr->cmd.rrc_abort_req,
                       sizeof(rrc_abort_req_type),
                       rrc_abort_req_ptr,
                       sizeof(rrc_abort_req_type));
          hard_abort = rrc_abort_req_ptr->hard_abort;
          rrcrce_procedure_event_handler(abort_cmd_local_ptr);

          rrc_free(abort_cmd_local_ptr);
          rrc_free(rrc_abort_req_ptr);
          if(hard_abort)
          {
          /* Reset the procedure data, the procedure ends */
            rrciho_init_data();
            return;
          }
        }

        /* Reverting to UTRAN fails, UE should perform cell update procedure */
        if ( (int_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
        {
          int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
          int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_RADIO_LINK_FAILURE;
          int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
          int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_IHO;

          WRRC_MSG0_HIGH("CELL_UPDATE_INITIATE_REQ sent by IHO on resume failure");
          rrc_put_int_cmd(int_cmd_ptr);

          rrciho_state = RRCIHO_WAIT_CELL_UPDATE_CNF;
        }
      }
      break;

    /* State change Indication */
    case RRC_STATE_CHANGE_IND:

#ifdef FEATURE_INTER_RAT_HO_OPT
      /* Cleanup */
      rrciho_cs_voice_rab_released();
#endif

      /* RRC state change to IDLE indication is received */
      WRRC_MSG1_HIGH("RRC State Change to IDLE in %d", rrciho_state);
      rrciho_init_data();

      break;

    /* RRC_ABORT_REQ */
    case RRC_ABORT_REQ:
      /* Set abort to true */
      rrciho_abort = TRUE;
      if(rrc_abort_req_ptr != NULL)
      {
        rrc_free(rrc_abort_req_ptr);
      }
      rrc_abort_req_ptr = (rrc_abort_req_type *)rrc_malloc(sizeof(rrc_abort_req_type));
      WCDMA_MEMCPY(rrc_abort_req_ptr,
                   sizeof(rrc_abort_req_type),
                   &(cmd_ptr->cmd.rrc_abort_req),
                   sizeof(rrc_abort_req_type));
      break;

    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected CMD %x in state %d", cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
  }
  return;
} /* end of rrciho_wait_channel_config_cnf_substate_event_handler */


/*===========================================================================

FUNCTION   rrciho_wait_acq_cnf_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Acquisition Confirmation from L1

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_acq_cnf_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  /* Switch on the received command. */
  /* RRC_INTERRAT_HANDOVER_ABORT_REQ */
  if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_INTERRAT_HANDOVER_ABORT_REQ)
  {
    WRRC_MSG0_HIGH("Rx: RRC_INTERRAT_HANDOVER_ABORT_REQ");

      /* Set abort to true */
      rrciho_abort = TRUE;
  }
  else
  {
    switch( cmd_ptr->cmd_hdr.cmd_id )
    {
    /* RRC_CSP_ACQ_CNF */
    case RRC_CSP_ACQ_CNF:
      WRRC_MSG0_HIGH("Rx: RRC_CSP_ACQ_CNF");

      /* if an abort command is received, send a handover failure stop
         the handover procedure and initialize all handover data*/
      if( ( rrciho_abort == TRUE ) ||
          ( cmd_ptr->cmd.rrc_csp_acq_cnf.acq_status == FAILURE) )
      {
        WRRC_MSG0_HIGH("CSP_ACQ_CNF FAILED");
        send_change_mode_req(RRC_MODE_INACTIVE);

        procedure_info.failure_cause = (uint32)RR_RRC_PHYSICAL_CHANNEL_FAILURE;
        rrciho_state = RRCIHO_WAIT_INACTIVE_CNF;
      }
      else
      {

        WRRC_MSG0_HIGH("CSP_ACQ_CNF SUCCESS");

#ifdef FEATURE_LTE_TO_WCDMA
        if(gtow_info.is_cs_rab_present)
          {
            if((RRCRB_CS_VOICE_RAB == gtow_info.est_rabs[gtow_info.cs_rab_index].rab_type))
            {
              //call wrapper fn. that calls L1 to lower it's pwr class to 4 for voice
              rrc_indicate_layer1_next_config_is_for_voice();
            }
            /* Update mac_rab_Status blindly to CS_RABS since IHO can set up only CS rabs  */
            rrcrb_update_mac_rab_status(RRC_PROCEDURE_IHO, RRC_CS_DOMAIN_CN_ID,
                                        NULL, FALSE);
          }

        /* In case of CS+PS handover, update MAC RAB status again for PS RAB */
        if(gtow_info.is_ps_rab_present)
        {
          /* Update mac_rab_Status based on the type of handover */
          rrcrb_update_mac_rab_status(RRC_PROCEDURE_IHO, RRC_PS_DOMAIN_CN_ID,
                                      NULL, FALSE);
        }
#else 
        /*By this time voice_config is updated.*/
        if((RRCRB_CS_VOICE_RAB == gtow_info.est_rabs.rab_type))
        {
          //call wrapper fn. that calls L1 to lower it's pwr class to 4 for voice
          rrc_indicate_layer1_next_config_is_for_voice();

        }

        if(RRCRB_CS_VOICE_RAB == gtow_info.est_rabs.rab_type ||
           RRCRB_CS_DATA_RAB == gtow_info.est_rabs.rab_type )
        {
          /* Update mac_rab_Status blindly to CS_RABS since IHO can set up only CS rabs  */
          rrcrb_update_mac_rab_status(RRC_PROCEDURE_IHO, RRC_CS_DOMAIN_CN_ID,
                                      NULL, FALSE);
        }

#endif /* FEATURE_LTE_TO_WCDMA */

        /*send channel config req to LLC to go to DCH state, if OC set
          is not successful for any reason, go to inactive and abandon
          the Handover */
        if(rrciho_send_channel_config_for_handover_to_utran() != SUCCESS)
        {
          WRRC_MSG0_HIGH("Ordered Config Set failed");
          send_change_mode_req(RRC_MODE_INACTIVE);
          procedure_info.failure_cause = (uint32)RR_RRC_PHYSICAL_CHANNEL_FAILURE;
          rrciho_state = RRCIHO_WAIT_INACTIVE_CNF;
        }

        /* By now, IHO procedure will be waiting in either RRCIHO_WAIT_CHAN_CONFIG_GTOW_CNF or 
        RRCIHO_WAIT_INACTIVE_CNF substate based on whether Channel Config Req is sent or not.*/
      }

      break;



#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_LTE_RRC_CMD:
      if(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == 
        WCDMA_RRC_LTE_ABORT_PSHO_REQ)
      {
        WRRC_MSG0_HIGH("Rx: RRC_INTERRAT_HANDOVER_ABORT_REQ");

        /* Set abort to true */
        rrciho_abort = TRUE;

        break;
      }
#endif
    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected CMD %x in state %d", cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
    }
  }
  return;
} /* end of rrciho_wait_acq_cnf_event_handler */


/*===========================================================================

FUNCTION   rrciho_wait_inactive_cnf_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Inactive Confirmation from MCM

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_inactive_cnf_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    /* RRC_CHANGE_MODE_CNF */
    case RRC_CHANGE_MODE_CNF:
      WRRC_MSG0_HIGH("Rx: RRC_CHANGE_MODE_CNF");

      if(rrciho_abort == TRUE)
      {
#ifdef FEATURE_LTE_TO_WCDMA
        if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
        {
          rrciho_send_ltow_ps_handover_to_utran_abort_cnf();
        }
        else
#endif
        {
          rrciho_send_handover_to_utran_abort_cnf();
        }
      }
      else
      {
#ifdef FEATURE_LTE_TO_WCDMA
        if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
        {
          /*send a handover to utran failure and reset IHO procedure's state
            machine*/
          rrciho_send_ltow_ps_handover_to_utran_failure();
        }
        else
#endif
        {
          /*send a handover to utran failure and reset IHO procedure's state
            machine*/
          rrciho_send_handover_to_utran_failure();
        }
      }
      break;

#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_LTE_RRC_CMD:
      if(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == 
        WCDMA_RRC_LTE_ABORT_PSHO_REQ)
      {
        WRRC_MSG0_HIGH("Rx: WCDMA_RRC_LTE_ABORT_PSHO_REQ");

        /* Set abort to true */
        rrciho_abort = TRUE;

        break;
      }
#endif

    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected CMD %x in state %d", cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
  }
  return;
} /* end of rrciho_wait_inactive_cnf_event_handler */


/*===========================================================================

FUNCTION  rrciho_wait_irat_in_progress_cnf_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Interrat in Progress Cnf from MCM

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_irat_in_progress_cnf_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  /* Switch on the received command. */
  if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_INTERRAT_HANDOVER_ABORT_REQ)
  {
    WRRC_MSG0_HIGH("Rx: RRC_INTERRAT_HANDOVER_ABORT_REQ");

      /* Set abort to true */
      rrciho_abort = TRUE;
  }
  else
  {
    switch( cmd_ptr->cmd_hdr.cmd_id )
    {
    /* RRC_CHANGE_MODE_CNF */
    case RRC_CHANGE_MODE_CNF:

      WRRC_MSG0_HIGH("Rx: RRC_CHANGE_MODE_CNF IRAT_PROG");

      /* if an abort command is received, send change mode request to
         inactivate L1 and set IHO state to wait for inactive state*/
      if(rrciho_abort == TRUE)
      {
        send_change_mode_req(RRC_MODE_INACTIVE);
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_STATE;
        rrciho_state = RRCIHO_WAIT_INACTIVE_CNF;
      }
      else
      {
        /*send acquisition request to LLC */
        rrciho_send_acq_req();

        /* Set the procedure state to wait for Acquisition confirmation */
        rrciho_state = RRCIHO_WAIT_ACQ_CNF;
      }

      break;

    /* RRC_INTERRAT_HANDOVER_ABORT_REQ */
/*lint -e408*/


#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_LTE_RRC_CMD:
      if(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == 
        WCDMA_RRC_LTE_ABORT_PSHO_REQ)
      {
        WRRC_MSG0_HIGH("Rx: RRC_INTERRAT_HANDOVER_ABORT_REQ");

        /* Set abort to true */
        rrciho_abort = TRUE;

        break;
      }
#endif
    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected CMD %x in state %d", cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
    }
  }
  return;
} /* end of rrciho_wait_irat_in_progress_cnf_event_handler */
/*===========================================================================

FUNCTION  rrciho_wait_ciph_config_gtow_cnf_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Ciph Cnf from SMC procedure

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_ciph_config_gtow_cnf_event_handler
(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{

#ifdef FEATURE_LTE_TO_WCDMA
  rrc_cmd_type *int_cmd_ptr;
#endif

  /* Switch on the received command. */
  /* RRC_INTERRAT_HANDOVER_ABORT_REQ */
  if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_INTERRAT_HANDOVER_ABORT_REQ)
  {
    WRRC_MSG0_HIGH("Rx: RRC_INTERRAT_HANDOVER_ABORT_REQ");
  
    /* Set abort to true */
    rrciho_abort = TRUE;
  }
  else
  {
    switch( cmd_ptr->cmd_hdr.cmd_id )
    {
    /* RRC_HO_CIPH_CNF */
    case RRC_HO_CIPH_CNF:

      WRRC_MSG0_HIGH("Rx: RRC_HO_CIPH_CNF in CIPH_CNF");

#ifdef FEATURE_INTER_RAT_HO_OPT
      if ((TRUE == gtow_info.rab_info_included) &&
#ifdef FEATURE_LTE_TO_WCDMA
          (gtow_info.is_cs_rab_present)
#else
          (RRCRB_CS_VOICE_RAB == gtow_info.est_rabs.rab_type)
#endif /* FEATURE_LTE_TO_WCDMA */
          )
      {
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        if(mn_has_nas_already_acquired_voc_subs(rrc_get_as_id()) == TRUE)
#else
        if(mn_has_nas_already_acquired_voc() == TRUE)
#endif
        {
          /* Set the control flag to true to indicate that we are taking control of the vocoder */
          rrciho_mvs_control_flag = TRUE;

          /* Now try to acquire W */
          WRRC_MSG0_HIGH("Acquiring MVS_CLIENT(W) during G->W handover");
          ghdi_mvs_acquire(MVS_CLIENT_WCDMA);
        }
      }
#endif

      /* if an abort command is received, send change mode request to
         inactivate L1 and set IHO state to wait for inactive state*/
      if(rrciho_abort == TRUE)
      {
        send_change_mode_req(RRC_MODE_INACTIVE);
        procedure_info.failure_cause = (uint32)RR_RRC_INVALID_STATE;
        rrciho_state = RRCIHO_WAIT_INACTIVE_CNF;
      }
      else
      {
        /*send handover to utran success command to RR layer*/
#ifdef FEATURE_LTE_TO_WCDMA
        if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
        {
          rrciho_send_ltow_ps_handover_to_utran_success();

          /* Send RRC_IHO_OPEN_DOMAIN_IND to DT */
          /* Get a RRC internal command buffer */
          int_cmd_ptr = rrc_get_int_cmd_buf();

          /* Fill in the command id  */
          int_cmd_ptr->cmd_hdr.cmd_id = RRC_IHO_OPEN_DOMAIN_IND;

          if(gtow_info.is_cs_rab_present && gtow_info.is_ps_rab_present)
          {
            int_cmd_ptr->cmd.rrc_open_domain_ind.domain = RRC_CS_PLUS_PS_DOMAIN;
          }
          else if(gtow_info.is_cs_rab_present)
          {
            int_cmd_ptr->cmd.rrc_open_domain_ind.domain = RRC_CS_DOMAIN;
          }
          else if(gtow_info.is_ps_rab_present)
          {
            int_cmd_ptr->cmd.rrc_open_domain_ind.domain = RRC_PS_DOMAIN;
          }

          /* Put the command on the internal queue */
          rrc_put_int_cmd( int_cmd_ptr );

          WRRC_MSG0_HIGH("RRC_IHO_OPEN_DOMAIN_IND sent to DT");
        }
        else
#endif
        {
          rrciho_send_handover_to_utran_success();
        }

        /*set Connection Setup status to true*/
        rrcrce_change_substate_to_rrcrce_final();

        /* Set the procedure state to Initial as all configuration for HO to
           UTRAN is complete.. we will wait for ACT RSP from MCM procedure
           and then send Uplink OTA msg HandoverToUTRANComplete */
        rrciho_state = RRCIHO_INITIAL;
      }

      break;

#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_LTE_RRC_CMD:
      if(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == 
        WCDMA_RRC_LTE_ABORT_PSHO_REQ)
      {
        WRRC_MSG0_HIGH("Rx: RRC_INTERRAT_HANDOVER_ABORT_REQ");

        /* Set abort to true */
        rrciho_abort = TRUE;

        break;
      }
#endif
    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected CMD %x in state %d", cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
    }
  }
  return;
}

/*===========================================================================

FUNCTION   rrciho_event_phychan_cfg_status_handover_to_utran

DESCRIPTION

    Event Logging for PHY_CHAN cnf for HO to UTRAN command.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_event_phychan_cfg_status_handover_to_utran
(
  boolean status
)
{
  /* declare an event log type for phychan cfg status*/
  rrclog_phychan_cfg_status_event_type phychan_cfg_event_log;

  phychan_cfg_event_log.phychan_cfg_status = status;

  /*update freq and scrambling code info*/

  event_report_payload(EVENT_WCDMA_PHYCHAN_CFG_CHANGED,
                       sizeof(phychan_cfg_event_log),
                       (void *)&phychan_cfg_event_log);
}/*rrciho_event_log_handover_to_utran*/

/*===========================================================================

FUNCTION   rrciho_wait_chan_config_gtow_cnf_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Chan Config confirmation from LLC

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_chan_config_gtow_cnf_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  rrc_cmd_type *int_cmd_ptr;  /*cmd ptr to send ciph config if ciphering
                                was enabled in GSM RAT*/

#ifdef FEATURE_LTE_TO_WCDMA
  uint8 j;
  rrc_ho_ciph_req_type ho_ciph_req;
#endif

  /* Switch on the received command. */
  if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_INTERRAT_HANDOVER_ABORT_REQ)
  {
    WRRC_MSG0_HIGH("Rx: RRC_INTERRAT_HANDOVER_ABORT_REQ");

      /* Set abort to true */
      rrciho_abort = TRUE;
  }
  else
  {
    switch( cmd_ptr->cmd_hdr.cmd_id )
    {
    /* RRC_CHANNEL_CONFIG_CNF */
    case RRC_CHANNEL_CONFIG_CNF:
      WRRC_MSG0_HIGH("Rx: RRC_CHANNEL_CONFIG_CNF");

      /* if an abort command is received, send change mode request to
         inactivate L1 and set IHO state to wait for inactive state or
         if the channel config confirmation is not successful then send a
         failure message and reset all the procedure data*/
      rrcllcpcie_initialise_sync_a_post_veri_info();

      if( ( rrciho_abort == TRUE ) ||
          ( cmd_ptr->cmd.chan_config_cnf.chan_cfg_status != RRCLLC_CHAN_CFG_SUCCESS) )
      {
        /* Why do we want to do the following. Anyway, rrc_active_codec wouldn't have been updated so far.*/      
 #ifdef FEATURE_VOC_AMR_WB
#ifndef FEATURE_LTE_TO_WCDMA
        gtow_info.est_rabs.nas_Synchronisation_Indicator= rrc_get_current_codec_mode();
        rrc_set_ordered_codec_mode(gtow_info.est_rabs.nas_Synchronisation_Indicator);
#else
        if((gtow_info.is_cs_rab_present) &&
           (RRCRB_CS_VOICE_RAB == gtow_info.est_rabs[gtow_info.cs_rab_index].rab_type))
        {
          gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator= rrc_get_current_codec_mode();
          rrc_set_ordered_codec_mode(gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator);
        }
#endif /* FEATURE_LTE_TO_WCDMA */
#endif/*FEATURE_VOC_AMR_WB*/
        send_change_mode_req(RRC_MODE_INACTIVE);
        rrciho_state = RRCIHO_WAIT_INACTIVE_CNF;

        /* Set the failure cause */
        if(rrciho_abort == TRUE)
        {
          procedure_info.failure_cause = (uint32)RR_RRC_INVALID_STATE;
        }
        else
        {
          procedure_info.failure_cause = (uint32)RR_RRC_PHYSICAL_CHANNEL_FAILURE;
          rrciho_event_phychan_cfg_status_handover_to_utran(FALSE);
        }
      }
      else
      {
        prev_cell_id = WCDMA_INVALID_CELL_ID;
        cell_id_dch_info.psc =0;
        cell_id_dch_info.uarfcn_dl = 0;
        /*Notify the Cell_Id to registered entities*/
        rrc_notify_cell_id_in_dch();

        /* Copy the Ordered RB Mapping into the current RB-Mapping database*/
        rrcllc_copy_ordered_hanging_rb_mapping_to_current();

        /*log an event in phy chan config is SUCCESSFUL*/
        rrciho_event_phychan_cfg_status_handover_to_utran(TRUE);

        if(TRUE == gtow_info.rab_info_included)
        {
#ifdef FEATURE_LTE_TO_WCDMA
          for(j = 0; j < gtow_info.number_of_est_rabs; j++)
          {
            /*add code here to update established rabs*/
            rrc_add_rab_to_established_rabs(&(gtow_info.est_rabs[j]));
          }
          if(gtow_info.is_cs_rab_present)
          {
              /*send activation command to vocoder*/
#ifdef FEATURE_VOC_AMR_WB
            rrciho_process_channel_config_cnf(gtow_info.est_rabs[gtow_info.cs_rab_index].nas_Synchronisation_Indicator);
#else /*FEATURE_VOC_AMR_WB*/
            rrciho_process_channel_config_cnf();
#endif /*FEATURE_VOC_AMR_WB*/
          }
#else
          /*add code here to update established rabs*/
          rrc_add_rab_to_established_rabs(&(gtow_info.est_rabs));

          if(RRCRB_CS_VOICE_RAB == gtow_info.est_rabs.rab_type)
          {
              /*send activation command to vocoder*/
#ifdef FEATURE_VOC_AMR_WB
            rrciho_process_channel_config_cnf(gtow_info.est_rabs.nas_Synchronisation_Indicator);
#else /*FEATURE_VOC_AMR_WB*/
            rrciho_process_channel_config_cnf();
#endif /*FEATURE_VOC_AMR_WB*/
          }
#endif /* FEATURE_LTE_TO_WCDMA */
        }

        /*clear the ordered config flag.. since we have successfully acquired and
          setup channels in WCDMA mode*/
        rrcllc_clear_ordered_config();

        /*if ciphering is started in GSM side then we have to start ciphering
          on WCDMA side too.. so send Ciphering Req to SMC and wait for ciph
          config.. if Ciphering is not started on GSM side then send handover
          to UTRAN success to RR and set rce's state to final*/
        WRRC_MSG2_HIGH("GSM Ciph Status: %d, Ciph Algo IE Present: %d",
           gtow_info.gsm_ciph_status,
           gtow_info.ciph_algo_present);
        
        if((gtow_info.gsm_ciph_status != TRUE)||
          (gtow_info.ciph_algo_present == FALSE ))
        {

#ifdef FEATURE_INTER_RAT_HO_OPT
          if ((TRUE == gtow_info.rab_info_included) &&
#ifndef FEATURE_LTE_TO_WCDMA
              (RRCRB_CS_VOICE_RAB == gtow_info.est_rabs.rab_type)
#else
              ((gtow_info.is_cs_rab_present) && 
               (RRCRB_CS_VOICE_RAB == gtow_info.est_rabs[gtow_info.cs_rab_index].rab_type))
#endif
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
              &&  (mn_has_nas_already_acquired_voc_subs(rrc_get_as_id()) == TRUE)
#else
              &&  (mn_has_nas_already_acquired_voc() == TRUE)
#endif
)
          {
            /* Set the control flag to true to indicate that we are taking control of the vocoder */
            rrciho_mvs_control_flag = TRUE;

            /* Now try to acquire W */
            WRRC_MSG0_HIGH("Acquiring MVS_CLIENT(W) during G->W handover");
            ghdi_mvs_acquire(MVS_CLIENT_WCDMA);
          }
#endif

          /*send handover to utran success command to RR layer*/
#ifdef FEATURE_LTE_TO_WCDMA
          if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
          {
            rrciho_send_ltow_ps_handover_to_utran_success();

            /* Send RRC_IHO_OPEN_DOMAIN_IND to DT */
            /* Get a RRC internal command buffer */
            int_cmd_ptr = rrc_get_int_cmd_buf();

            /* Fill in the command id  */
            int_cmd_ptr->cmd_hdr.cmd_id = RRC_IHO_OPEN_DOMAIN_IND;

            if(gtow_info.is_cs_rab_present && gtow_info.is_ps_rab_present)
            {
              int_cmd_ptr->cmd.rrc_open_domain_ind.domain = RRC_CS_PLUS_PS_DOMAIN;
            }
            else if(gtow_info.is_cs_rab_present)
            {
              int_cmd_ptr->cmd.rrc_open_domain_ind.domain = RRC_CS_DOMAIN;
            }
            else if(gtow_info.is_ps_rab_present)
            {
              int_cmd_ptr->cmd.rrc_open_domain_ind.domain = RRC_PS_DOMAIN;
            }

            /* Put the command on the internal queue */
            rrc_put_int_cmd( int_cmd_ptr );

            WRRC_MSG0_HIGH("RRC_IHO_OPEN_DOMAIN_IND sent to DT");
            /* Get a RRC internal command buffer */
            /*Store the ps keys only if PS rab is present and CS keys when CS rab is present*/
            if(gtow_info.is_ps_rab_present)
            {

              WCDMA_MEMCPY(ho_ciph_req.security_keys[RRC_PS_DOMAIN_CN_ID].ciphering_key,
                           sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                           gtow_info.psho_security_keys.ck,
                           LTE_IRAT_MAX_CK_LENGTH );
              WCDMA_MEMCPY(ho_ciph_req.security_keys[RRC_PS_DOMAIN_CN_ID].integrity_key,
                           sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                           gtow_info.psho_security_keys.ik,
                           LTE_IRAT_MAX_IK_LENGTH );

              rrcsmc_save_security_key_set_from_lte(ho_ciph_req, RRC_PS_DOMAIN_CN_ID);
            }

            if(gtow_info.is_cs_rab_present)
            {
              WCDMA_MEMCPY(ho_ciph_req.security_keys[RRC_CS_DOMAIN_CN_ID].ciphering_key,
                           sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                           gtow_info.psho_security_keys.ck_srvcc,
                           LTE_IRAT_MAX_CK_LENGTH );
              WCDMA_MEMCPY(ho_ciph_req.security_keys[RRC_CS_DOMAIN_CN_ID].integrity_key,
                           sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                           gtow_info.psho_security_keys.ik_srvcc,
                           LTE_IRAT_MAX_IK_LENGTH );

              rrcsmc_save_security_key_set_from_lte(ho_ciph_req, RRC_CS_DOMAIN_CN_ID);
            }
          }
          else
#endif
          {
            rrciho_send_handover_to_utran_success();
          }

          /*set Connection Setup status to true*/
          rrcrce_change_substate_to_rrcrce_final();

          /*If ciphering is not started in G then at GtoW HandOver check 
           *Check if any new keys got generated, if yes then update these
           *keys from buffer to ciphering data base, right here.
           */
          rrcsmc_check_and_update_security_key_set_gen_in_other_rat(RRC_CS_DOMAIN_CN_ID);
   
          /* Set the procedure state to wait for Acquisition confirmation */
          rrciho_state = RRCIHO_INITIAL;
        }
        else
        {
          /* Get a RRC internal command buffer */
          int_cmd_ptr = rrc_get_int_cmd_buf();

          /* Fill in the command id  and ciphering algorithm*/
          int_cmd_ptr->cmd_hdr.cmd_id = RRC_HO_CIPH_REQ;
          int_cmd_ptr->cmd.ho_ciph_req.ciph_algo = gtow_info.cipheringAlgorithm;
#ifndef FEATURE_LTE_TO_WCDMA
          int_cmd_ptr->cmd.ho_ciph_req.domain_id = RRC_CS_DOMAIN_CN_ID;
          int_cmd_ptr->cmd.ho_ciph_req.config_cause = CIPH_CONFIG_GTOW;

#else
         if((iho_interrat_type == RRCIHO_INTERRAT_LTE) && 
              ((gtow_info.is_ps_rab_present)&&(gtow_info.is_cs_rab_present)))
          {
            /*As per section 8.3.6.3 since both CS and PS rabs are present set the domain id to CS.
            set the variable LATEST_CONFIGURED_CN_DOMAIN to the value indicated in the IE "CN domain
            identity" of the IE "RAB info" of the IE "RAB information to setup" if all instances of the IE indicate the
            same CN domain, or to the CS domain when this IE is either not present or different instances indicate
            different CN domains */
            
            int_cmd_ptr->cmd.ho_ciph_req.domain_id = RRC_CS_DOMAIN_CN_ID;
            int_cmd_ptr->cmd.ho_ciph_req.config_cause = CIPH_CONFIG_CS_PLUS_PS;

            /*Copy the CS and PS keys*/
            WCDMA_MEMCPY(int_cmd_ptr->cmd.ho_ciph_req.security_keys[RRC_PS_DOMAIN_CN_ID].ciphering_key,
                         sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                         gtow_info.psho_security_keys.ck,
                         LTE_IRAT_MAX_CK_LENGTH );
            WCDMA_MEMCPY(int_cmd_ptr->cmd.ho_ciph_req.security_keys[RRC_PS_DOMAIN_CN_ID].integrity_key,
                         sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                         gtow_info.psho_security_keys.ik,
                         LTE_IRAT_MAX_IK_LENGTH );

            WCDMA_MEMCPY(int_cmd_ptr->cmd.ho_ciph_req.security_keys[RRC_CS_DOMAIN_CN_ID].ciphering_key,
                         sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                         gtow_info.psho_security_keys.ck_srvcc,
                         LTE_IRAT_MAX_CK_LENGTH );
            WCDMA_MEMCPY(int_cmd_ptr->cmd.ho_ciph_req.security_keys[RRC_CS_DOMAIN_CN_ID].integrity_key,
                         sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                         gtow_info.psho_security_keys.ik_srvcc,
                         LTE_IRAT_MAX_IK_LENGTH );
          }
          else if ((iho_interrat_type == RRCIHO_INTERRAT_LTE) && 
              (gtow_info.is_ps_rab_present))
          {
            int_cmd_ptr->cmd.ho_ciph_req.domain_id = RRC_PS_DOMAIN_CN_ID;
            int_cmd_ptr->cmd.ho_ciph_req.config_cause = CIPH_CONFIG_LTOW;
            WCDMA_MEMCPY(int_cmd_ptr->cmd.ho_ciph_req.security_keys[RRC_PS_DOMAIN_CN_ID].ciphering_key,
                         sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                         gtow_info.psho_security_keys.ck,
                         LTE_IRAT_MAX_CK_LENGTH );
            WCDMA_MEMCPY(int_cmd_ptr->cmd.ho_ciph_req.security_keys[RRC_PS_DOMAIN_CN_ID].integrity_key,
                         sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                         gtow_info.psho_security_keys.ik,
                         LTE_IRAT_MAX_IK_LENGTH );
          }
          else if ((iho_interrat_type == RRCIHO_INTERRAT_LTE) && 
              (gtow_info.is_cs_rab_present))
          {
            int_cmd_ptr->cmd.ho_ciph_req.domain_id = RRC_CS_DOMAIN_CN_ID;
            int_cmd_ptr->cmd.ho_ciph_req.config_cause = CIPH_CONFIG_CS;
            WCDMA_MEMCPY(int_cmd_ptr->cmd.ho_ciph_req.security_keys[RRC_CS_DOMAIN_CN_ID].ciphering_key,
                         sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                         gtow_info.psho_security_keys.ck_srvcc,
                         LTE_IRAT_MAX_CK_LENGTH );
            WCDMA_MEMCPY(int_cmd_ptr->cmd.ho_ciph_req.security_keys[RRC_CS_DOMAIN_CN_ID].integrity_key,
                         sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                         gtow_info.psho_security_keys.ik_srvcc,
                         LTE_IRAT_MAX_IK_LENGTH );
          }
          /* is_cs_rab_present is not true for default config 0 and 1
          so change the below condition to handle this scenario */
          else if (iho_interrat_type == RRCIHO_INTERRAT_GSM)
          {
            int_cmd_ptr->cmd.ho_ciph_req.domain_id = RRC_CS_DOMAIN_CN_ID;
            int_cmd_ptr->cmd.ho_ciph_req.config_cause = CIPH_CONFIG_GTOW;
          }
#endif

          /* Put the command on the internal queue */
          rrc_put_int_cmd( int_cmd_ptr );
          WRRC_MSG0_HIGH("RRC_HO_CIPH_REQ sent to SMC");

#ifdef FEATURE_GTOW_PS_HANDOVER
          /*Currently we dont have the domain I.D support in RRC_HO_CIPH_REQ, which will be introduced in PS hand over
            *So when we support PS-HandOver, we need to check whether the GtoW hand over is for PS domain, if so 
            *then check & update CS keys if any generated in other RAT here by calling below function
            *if(GtoW HandOver is for PS domain)
            *{
            *  rrcsmc_check_and_update_security_key_set_gen_in_other_rat(RRC_CS_DOMAIN_CN_ID);
            *}
            *else
            *{
            *  donothing here
            *}
            */
#endif

          rrciho_state = RRCIHO_WAIT_CIPH_CONFIG_GTOW_CNF;
        }
      }

      break;

#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_LTE_RRC_CMD:
      if(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == 
        WCDMA_RRC_LTE_ABORT_PSHO_REQ)
      {
        WRRC_MSG0_HIGH("Rx: RRC_INTERRAT_HANDOVER_ABORT_REQ");

        /* Set abort to true */
        rrciho_abort = TRUE;

        break;
      }
#endif
    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected CMD %x in state %d", cmd_ptr->cmd_hdr.cmd_id, rrciho_state);
      break;
    }
  }
  return;
} /* end of rrciho_wait_chan_config_gtow_cnf_event_handler */

/*===========================================================================

FUNCTION   rrciho_wait_cell_update_cnf_substate_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for cell update confirmation from CU

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrciho_wait_cell_update_cnf_substate_event_handler(rrc_cmd_type *cmd_ptr)
{
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:

      WRRC_MSG0_HIGH("Received RRC_CELL_UPDATE_CNF from CU after resume failure ");

#ifdef FEATURE_INTER_RAT_HO_OPT
      /* If the cell update was initiated because of resume failure
         the vocoder needs to be turned ON now.If the cell update was
         initiated due to Channel Config failure,it would have been acquired
         before.Hence check the mvs control flag and enabled the vocoder if
         needed */
      if( (rrciho_mvs_control_flag != TRUE) && 
          (rrc_is_voice_rab_present()) )
        {
          /* Set the amr parameters */
#ifdef FEATURE_VOC_AMR_WB
          rrciho_process_channel_config_cnf(rrc_active_codec);
#else /*FEATURE_VOC_AMR_WB*/
          rrciho_process_channel_config_cnf();
#endif   /*FEATURE_VOC_AMR_WB*/      
          WRRC_MSG0_HIGH("AMR settings complete after W->G handover failure in IHO CU_CNF substate");
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        if(mn_has_nas_already_acquired_voc_subs(rrc_get_as_id()) == TRUE)
#else
        if(mn_has_nas_already_acquired_voc() == TRUE)
#endif
        {
          /* Set the control flag to true to indicate that we are taking control of the vocoder */
          rrciho_mvs_control_flag = TRUE;

          /* Now try to acquire W */
          WRRC_MSG0_HIGH("Acquiring MVS_CLIENT(W) during W->G handover failure in IHO CU_CNF substate");
          ghdi_mvs_acquire(MVS_CLIENT_WCDMA);
        }
      }
#endif

      /* Send handover from utran failure to UTRAN */
      send_handover_from_utran_failure();

      /* Reset the procedure data, the procedure ends */
      rrciho_init_data();

#if defined (FEATURE_GAN_3G) || defined (FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE)
      #error code not present
#endif

      break;

    case RRC_ABORT_REQ:
      /* Option 1: Forward the req to RCE */
      rrcrce_procedure_event_handler( cmd_ptr );

      /* Reset the procedure data, the procedure ends */
      rrciho_init_data();
      break;

    case RRC_STATE_CHANGE_IND:

      /* RRC state change to IDLE indication is received */
      WRRC_MSG1_HIGH("RRC State Change to IDLE in %d", rrciho_state);

#ifdef FEATURE_INTER_RAT_HO_OPT
      /* Cleanup */
      rrciho_cs_voice_rab_released();
#endif

      /* Init procedure data */
      rrciho_init_data();
      break;

    default:

      WRRC_MSG0_HIGH("Received unexpected command in wait_cell_update_cnf_substate");
      break;
  }
} /* end of rrciho_wait_cell_update_cnf_substate_event_handler */

/*===========================================================================

FUNCTION          rrccsp_is_wcdma_band_enabled

DESCRIPTION       This function checks if WCDMA bands are supported 

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
static boolean rrccsp_is_wcdma_band_enabled( void)
{
  sd_ss_band_pref_e_type ui_rf_band_capability;

#ifdef FEATURE_DUAL_SIM
  ui_rf_band_capability = (sd_ss_band_pref_e_type) sd_misc_get_subs_supp_band_pref(rrc_get_as_id());
#else
  ui_rf_band_capability = (sd_ss_band_pref_e_type) sd_misc_get_supp_band_pref();
#endif
  if(ui_rf_band_capability &
      (SD_SS_BAND_PREF_WCDMA_II_PCS_1900 |
       SD_SS_BAND_PREF_WCDMA_I_IMT_2000  |
       SD_SS_BAND_PREF_WCDMA_VI_800      |
       SD_SS_BAND_PREF_WCDMA_V_850       |
       SD_SS_BAND_PREF_WCDMA_VIII_900    |
       SD_SS_BAND_PREF_WCDMA_III_1700    |
       SD_SS_BAND_PREF_WCDMA_IV_1700     |
       SD_SS_BAND_PREF_WCDMA_IX_1700     |
       SD_SS_BAND_PREF_WCDMA_XI_1500     |
       SD_SS_BAND_PREF_WCDMA_XIX_850)
    )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION RRC_GET_UTRAN_CLASSMARK_CHANGE_HANDOVER_INFO

DESCRIPTION

  This function builds a INTERRAT HANDOVER INFO message type which will be
  sent by GSM to UTRAN thru GERAN.  An ASN1 encoded message and message
  length is populated by RRC, the message and msg_length is used by RR to
  send this as part of Classmark Change to UTRAN via GERAN
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/
static void rrc_get_utran_classmark_change_handover_info
(
  byte ** msg_ptr,    /* The command to be sent to UTRAN */
  uint8 * msg_len    /* Length of the message           */
#ifdef FEATURE_IRAT_HO_INFO_COMPRESSED
  #error code not present
#endif /* FEATURE_IRAT_HO_INFO_COMPRESSED */
#if defined (FEATURE_LTE_TO_WCDMA) || defined (FEATURE_WCDMA_TO_LTE)
#ifdef LTE_BAND_NUM
  ,sys_lte_band_mask_e_type lte_band_pref
#else
  ,sys_band_mask_e_type lte_band_pref
#endif
  ,boolean l2w_psho_enabled
#endif
)
{
  pdu_buf_type encoded_sdu;          /* encoded SDU */
  rrc_InterRATHandoverInfo *ptr;     /* Pointer to Interrat HO Info PDU type */
  uint32 error_stat=0;
  rrc_UE_RadioAccessCapabilityInfo *cap_ptr;
  
  boolean use_compressed_irat_ho_info = FALSE;

  rrc_UE_RadioAccessCapability_v650ext local_v650ext;     /* Local copy of v650extensions */
#ifdef FEATURE_WCDMA_3C_HSDPA
  rrcueci_band_storage_type ueci_band = {0};	
  rrc_UE_RadioAccessCapabBandFDDList4 *temp_fdd_list4_ptr = NULL;
  uint8 num_of_sb_3c = 0; /* number of bands supporting single band 3C */
  rrc_RadioFrequencyBandFDD_Root sb_3c_band[2] = {0};
  uint8 idx = 0;
#endif
#ifdef FEATURE_WCDMA_TO_LTE
  rrc_UE_RadioAccessCapability_v860ext local_v860ext;
#endif

#ifdef FEATURE_WCDMA_MIMO
  boolean support_tx_diversity_mimo = FALSE;
#endif

#ifdef FEATURE_LTE_TO_WCDMA
  rrc_START_Value   *start_ps;
#endif

#ifdef FEATURE_IRAT_HO_INFO_COMPRESSED
  #error code not present
#endif /* FEATURE_IRAT_HO_INFO_COMPRESSED */

  WRRC_MSG0_HIGH("Get UTRAN Classmark Info");

  /* Allocate memory for this message. Note that this buffer is released
     by the delete command called from RR */
  ptr = (rrc_InterRATHandoverInfo *)rrc_malloc(sizeof(rrc_InterRATHandoverInfo));

  /* Currently set all the fields to absent.. We will support this when ciphering
     support is added */
  ptr->predefinedConfigStatusList.t = T_rrc_InterRATHandoverInfo_predefinedConfigStatusList_absent;

  ptr->uE_SecurityInformation.t = T_rrc_InterRATHandoverInfo_uE_SecurityInformation_present;
  /*append start value for CS domain */
  ptr->uE_SecurityInformation.u.present.start_CS.numbits = RRC_START_VALUE_LENGTH;

  
 

  rrcsmc_append_start_per_domain((byte *) (ptr->uE_SecurityInformation.u.present.
                                           start_CS.data),
                                 RRC_CS_DOMAIN_CN_ID,
                                 RRCSMC_APPEND_START);

  /* Initialize the v650 ext before sending it to the function.There is no provision yet to send this
     structure in Classmark info.Better to send a structure cos it would be risky to pass a NULL ptr */
  RRC_RESET_MSG_IE_PRESENT(local_v650ext);
  local_v650ext.ue_RadioAccessCapabBandFDDList2.n = 0;
  local_v650ext.ue_RadioAccessCapabBandFDDList_ext.n = 0;

#ifdef FEATURE_WCDMA_TO_LTE
  local_v860ext.ue_RadioAccessCapabBandFDDList3.n = 0;
#endif

  if(!use_compressed_irat_ho_info)
  {
    /* Allocate memory for this message. Note that this buffer is released
       by the delete command called from RR */
    cap_ptr = (rrc_UE_RadioAccessCapabilityInfo *)rrc_malloc(sizeof(rrc_UE_RadioAccessCapabilityInfo));

    ptr->ue_CapabilityContainer.t = T_rrc_InterRATHandoverInfo_ue_CapabilityContainer_present;

    (void)rrcueci_append_ue_radio_access_capability(&(cap_ptr->ue_RadioAccessCapability),
                        &(cap_ptr->ue_RadioAccessCapability_v370ext),
                        &local_v650ext,
                        RRCUECI_CAP_HO_INFO_TRANSFER);

    /* Encodes the OTA message using ASN1. The memory for msg_ptr is allocated
       by the RRC procedure and the memory for encoded_sdu will be allocated
       by the following function */
   
    if ((error_stat= rrc_enc_UE_RadioAccessCapabilityInfo_PDU(cap_ptr,&ptr->ue_CapabilityContainer.u.present))
        !=0)
        {
        ERR_FATAL("Error in ASN1 encoding %d  error %d",
                          rrc_UE_RadioAccessCapabilityInfo_PDU,
                          error_stat, 0);
        }
    

   
    WRRC_MSG1_HIGH("Class Mark Cap Len %d, Info",ptr->ue_CapabilityContainer.u.present.numocts);

    // The memory freeing for msg_ptr should be done here. using OSS compiler
    // functions. This function should free memory allocated for msg_ptr and as
    // well as linked lists in the message if they are any allocated.
   //if(rrcasn1_free_pdu( (int)rrc_UE_RadioAccessCapabilityInfo_PDU , cap_ptr))
    //{
      // This is a memory leak and it should never happen.
    //  WRRC_MSG0_ERROR("Memory freeing is unsuccessful");
    //}

    // memory freeing for encoded_sdu should be done here using OSS compiler
    // functions.
    //rrcasn1_free_buf(encoded_sdu.value);
    rrc_free(cap_ptr);
  }
#ifdef FEATURE_IRAT_HO_INFO_COMPRESSED
  #error code not present
#endif /* FEATURE_IRAT_HO_INFO_COMPRESSED */

  /* Set rx_tx_TimeDifferenceType2Capable to FALSE. Revisit when this is supported */
  ptr->v390NonCriticalExtensions.t = T_rrc_InterRATHandoverInfo_v390NonCriticalExtensions_present; 
  RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.interRATHandoverInfo_v390ext);
 RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.interRATHandoverInfo_v390ext,
   ue_RadioAccessCapability_v380ext);

  ptr->v390NonCriticalExtensions.u.present.interRATHandoverInfo_v390ext.ue_RadioAccessCapability_v380ext.
  ue_PositioningCapabilityExt_v380.rx_tx_TimeDifferenceType2Capable = FALSE;

  RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.interRATHandoverInfo_v390ext.dl_PhysChCapabilityFDD_v380ext);

  RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present);

  /* Append FDD2 list if present */
  WRRC_MSG2_HIGH("IHO FDD2 Status:%d, NV Release:%d",
              (local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0 ? 1:0),
              rrc_nv_rel_indicator);
  
  if((local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0) || 
     (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
    )
  {
    /* Set v3a0NonCriticalExtensions */
    ptr->v390NonCriticalExtensions.u.present.m.v3a0NonCriticalExtensionsPresent =1;
    RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.interRATHandoverInfo_v3a0ext);

    /* Set v3a0NonCriticalExtensions_laterNonCriticalExtensions */
    ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.m.
    laterNonCriticalExtensionsPresent =1;

    /* Set later non criticalExtensions bitmask to 0 initially */
   RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.
      laterNonCriticalExtensions);

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA) 
    if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) && (!use_compressed_irat_ho_info) && (rrc_ue_pos_cap_pch))
    {
      /*Set only if not compressed IRAT HO info, for compressed IRAT HO info, GPS capability is not set*/
       RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.interRATHandoverInfo_v3a0ext,
           ue_RadioAccessCapability_v3a0ext);
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.interRATHandoverInfo_v3a0ext.
        ue_RadioAccessCapability_v3a0ext.ue_PositioningCapabilityExt_v3a0.validity_CellPCH_UraPCH = rrc_UE_PositioningCapabilityExt_v3a0_validity_CellPCH_UraPCH_true;
      WRRC_MSG0_HIGH("RRCGPS: PCH validity set");
    }
#endif

    RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.
    laterNonCriticalExtensions.interRATHandoverInfo_v3d0ext);

    /* Append FDD2 list if present */
    if((local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0)
        ||(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
        )
    {

      WRRC_MSG0_HIGH("Populating Fdd2");

      /* Set bitmask to include r3_add_ext */
     RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions,
        interRATHandoverInfo_r3_add_ext);


      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.interRATHandoverInfo_r3_add_ext);

      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.interRATHandoverInfo_v690ext1);

      if(local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0)
      {
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.interRATHandoverInfo_v690ext1.m.ue_RadioAccessCapability_v650extPresent =1;
  
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.interRATHandoverInfo_v690ext1.ue_RadioAccessCapability_v650ext = 
          local_v650ext;
      }
    }
    if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.interRATHandoverInfo_r3_add_ext, v7e0NonCriticalExtensions);

      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions);

      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.interRATHandoverInfo_v7e0ext);

      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.interRATHandoverInfo_v7e0ext, ue_RadioAccessCapability);

      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.interRATHandoverInfo_v7e0ext.ue_RadioAccessCapability);
        
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.interRATHandoverInfo_v7e0ext.
      ue_RadioAccessCapability, supportForTwoDRXSchemesInPCH);

      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.interRATHandoverInfo_v7e0ext.
      ue_RadioAccessCapability.supportForTwoDRXSchemesInPCH = rrc_UE_RadioAccessCapability_v7e0ext_supportForTwoDRXSchemesInPCH_true;

      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.interRATHandoverInfo_v7e0ext.
      ue_RadioAccessCapability, supportEDPDCHPowerInterpolation);
      
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.interRATHandoverInfo_v7e0ext.
      ue_RadioAccessCapability.supportEDPDCHPowerInterpolation = rrc_UE_RadioAccessCapability_v7e0ext_supportEDPDCHPowerInterpolation_true;
#ifdef FEATURE_WCDMA_MIMO
      if (support_tx_diversity_mimo)
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions, v7f0NonCriticalExtensions);
    
        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.v7f0NonCriticalExtensions);
    
        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.v7f0NonCriticalExtensions.interRATHandoverInfo_v7f0ext);
    
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.v7f0NonCriticalExtensions.interRATHandoverInfo_v7f0ext, ue_RadioAccessCapabilityComp2);
    
        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.v7f0NonCriticalExtensions.interRATHandoverInfo_v7f0ext.
          ue_RadioAccessCapabilityComp2);
    
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.v7f0NonCriticalExtensions.interRATHandoverInfo_v7f0ext.
          ue_RadioAccessCapabilityComp2, supportofTxDivOnNonMIMOChannel);
    
    
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        interRATHandoverInfo_r3_add_ext.v7e0NonCriticalExtensions.v7f0NonCriticalExtensions.interRATHandoverInfo_v7f0ext.
          ue_RadioAccessCapabilityComp2.supportofTxDivOnNonMIMOChannel = rrc_UE_RadioAccessCapabilityComp2_v7f0ext_supportofTxDivOnNonMIMOChannel_true;
    
        WRRC_MSG0_HIGH("MIMO:Sending Tx Diversity for DL Control channels");
      }
#endif	  
    }
  }

  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    /* Set v3g0NonCriticalExtensions_present */
    ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v3g0NonCriticalExtensionsPresent =1;

    RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.
    laterNonCriticalExtensions.v3g0NonCriticalExtensions.interRATHandoverInfo_v3g0ext);

    /* Set v4b0NonCriticalExtensions */
    ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.
    laterNonCriticalExtensions.v3g0NonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent =1;


#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */

#ifdef FEATURE_WCDMA_REL9

    if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9)
    {
      /* Set the access stratum indicator */
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.interRATHandoverInfo_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_9;
      WRRC_MSG0_HIGH("AccessStratumIndicator indicator set to Rel-9 in Handover Classmark info");
    }
    else
#endif /* FEATURE_WCDMA_REL9 */

    if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
    {
      /* Set the access stratum indicator */
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.interRATHandoverInfo_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_8;
      WRRC_MSG0_HIGH("AccessStratumIndicator indicator set to Rel-8 in Handover Classmark info");
    }
    else
    if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
    {
      /* Set the access stratum indicator */
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.interRATHandoverInfo_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_7;
      WRRC_MSG0_HIGH("AccessStratumIndicator indicator set to Rel-7 in Handover Classmark info");
    }
    else
    {
      if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
      {
        /* Set the access stratum indicator */
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.interRATHandoverInfo_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_6;
        WRRC_MSG0_HIGH("AccessStratumIndicator indicator set to Rel-6 in Handover Classmark info");
      }
      else
      {
        /* Set the access stratum indicator */
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.interRATHandoverInfo_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_5;
        WRRC_MSG0_HIGH("AccessStratumIndicator indicator set to Rel-5 in Handover Classmark info");
      }
    }
    /* Set v4d0NonCriticalExtensions */
    ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.
    laterNonCriticalExtensions.v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.m.v4d0NonCriticalExtensionsPresent =1;

    RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
    v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.interRATHandoverInfo_v4d0ext);

	/* Set v590NonCriticalExtensions */
    ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.
    laterNonCriticalExtensions.v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.
    v4d0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent =1;

    RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
    v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions);

    RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
    v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
    interRATHandoverInfo_v590ext);

#ifdef FEATURE_IRAT_HO_INFO_COMPRESSED
    #error code not present
#endif /* FEATURE_IRAT_HO_INFO_COMPRESSED*/

    if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
    {
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.m.
        v690NonCriticalExtensionsPresent =1;
    
        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions);
    
        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext);
    
        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2);
    
    
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2,
          fddPhysicalChannelCapab_hspdsch_edch);

        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2
        .fddPhysicalChannelCapab_hspdsch_edch.m.dl_CapabilityWithSimultaneousHS_DSCHConfigPresent=1;

        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2
        .fddPhysicalChannelCapab_hspdsch_edch.dl_CapabilityWithSimultaneousHS_DSCHConfig = rrc_DL_CapabilityWithSimultaneousHS_DSCHConfig_kbps64;

        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2
        .fddPhysicalChannelCapab_hspdsch_edch.physicalChannelCapabComp_hspdsch_r6 = nv_hsdpa_category;
        if (nv_hsdpa_category >= 12)
        {
          ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
            v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2
            .fddPhysicalChannelCapab_hspdsch_edch.physicalChannelCapabComp_hspdsch_r6 = rrc_machs_cat;
           WRRC_MSG1_HIGH("In R6 ext put HSDPA category as %d",rrc_machs_cat);
        }
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2
        .fddPhysicalChannelCapab_hspdsch_edch.physicalChannelCapability_edch_r6.fdd_edch.t = T_rrc_PhysicalChannelCapability_edch_r6_fdd_edch_unsupported;
    
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2
        .fddPhysicalChannelCapab_hspdsch_edch.physicalChannelCapability_edch_r6.fdd_edch.t = T_rrc_PhysicalChannelCapability_edch_r6_fdd_edch_supported;

		ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2
        .fddPhysicalChannelCapab_hspdsch_edch.physicalChannelCapability_edch_r6.fdd_edch.u.supported =
	     rtxMemAllocTypeZ(&enc_ctxt,rrc_PhysicalChannelCapability_edch_r6_supported);
		
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_RadioAccessCapabilityComp2
        .fddPhysicalChannelCapab_hspdsch_edch.physicalChannelCapability_edch_r6.fdd_edch.u.supported->edch_PhysicalLayerCategory =
         MIN(wcdma_nv_hsupa_category,WNV_GET_DEFAULT(WNV_ID_WCDMA_HSUPA_DEFAULT_CATEGORY_I));
    
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.
        v590NonCriticalExtensions.v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent =1;
    
        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions);
    
        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.interRATHandoverInfo_v6b0ext);
   
        RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.interRATHandoverInfo_v6b0ext,
        rrc_InterRATHandoverInfo_v6b0ext_IEs,supportForSIB11bis);
    
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.interRATHandoverInfo_v6b0ext.supportForSIB11bis = 
        rrc_InterRATHandoverInfo_v6b0ext_IEs_supportForSIB11bis_true;

#ifdef FEATURE_IRAT_HO_INFO_COMPRESSED
        #error code not present
#endif /* FEATURE_IRAT_HO_INFO_COMPRESSED */

#ifdef FEATURE_LTE_TO_WCDMA
      if(l2w_psho_enabled)
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext, 
          ue_SecurityInformation2);
  
        start_ps = &(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.interRATHandoverInfo_v690ext.ue_SecurityInformation2.start_PS);
          
        start_ps->numbits= RRC_START_VALUE_LENGTH;      
          
        rrcsmc_append_start_per_domain((byte *) (start_ps->data),
                                         RRC_PS_DOMAIN_CN_ID,
                                         RRCSMC_APPEND_START);
      }
#endif /* FEATURE_LTE_TO_WCDMA */

    }
    if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
    {
      /*TBD PSR to be updated in 7.6*/
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent =1;
      
      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.interRATHandoverInfo_v6b0ext);

      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v6e0NonCriticalExtensionsPresent =1 ;

      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.interRATHandoverInfo_v6e0ext);

      if(!(rrc_disable_cpc_fdpch_nv & NV_FDPCH_DISABLE))
      {
        RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.interRATHandoverInfo_v6e0ext,
           rrc_InterRATHandoverInfo_v6e0ext_IEs,supportForFDPCH);
  
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.interRATHandoverInfo_v6e0ext.
          supportForFDPCH = rrc_InterRATHandoverInfo_v6e0ext_IEs_supportForFDPCH_true;
      }
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
       v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
       v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.m.v770NonCriticalExtensionsPresent = 1;

      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
       v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
       v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
       interRATHandoverInfo_v770ext);


      RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
       v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
       v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
       interRATHandoverInfo_v770ext,
       rrc_InterRATHandoverInfo_v770ext_IEs,ue_RadioAccessCapabilityInfo);

      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
       v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
       v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
       interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityInfo);

      RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
       v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
       v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
       interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityInfo,
        rrc_UE_RadioAccessCapabilityInfo_v770ext,mac_ehsSupport);

      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
       v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
       v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
       interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityInfo.mac_ehsSupport = rrc_UE_RadioAccessCapability_v770ext_IEs_mac_ehsSupport_true;

      RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
       v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
       v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
       interRATHandoverInfo_v770ext,
       rrc_InterRATHandoverInfo_v770ext_IEs,ue_RadioAccessCapabilityComp2);

      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
       v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
       v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
       interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2);
      if(nv_hsdpa_category > rrc_macehs_cat)
      {
         RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
         v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
         v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2,            
         rrc_UE_RadioAccessCapabilityComp2_v770ext,hsdsch_physical_layer_category_ext);

         ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
         v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
         v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2.hsdsch_physical_layer_category_ext =
               rrc_macehs_cat;
        WRRC_MSG1_HIGH("REL8: Update HSDPA CAT as %d",rrc_macehs_cat);
      }
      else
      if(nv_hsdpa_category > 8 )
      { 
         RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
         v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
         v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2,          
         rrc_UE_RadioAccessCapabilityComp2_v770ext,hsdsch_physical_layer_category_ext);

         ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
         v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
         v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2.hsdsch_physical_layer_category_ext =
               nv_hsdpa_category;
          
      }
      else
      {
          WRRC_MSG1_HIGH("RRCHSPA+: 64 QAM feature dfiened but NV returns %d , should be 13 or 14",nv_hsdpa_category);
      }

      if(!(rrc_disable_cpc_fdpch_nv & (NV_EFDPCH_DISABLE|NV_FDPCH_DISABLE)))
      {
        RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
             v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
             interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2,  
             rrc_UE_RadioAccessCapabilityComp2_v770ext,enhancedFdpch);
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
             v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
             interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2.enhancedFdpch =rrc_UE_RadioAccessCapabilityComp2_v770ext_enhancedFdpch_true;
      }

      if(rrcueci_nv_supports_cpc_dtx_on_dpcch() == TRUE)
      {
        WRRC_MSG0_HIGH("RRCCPC:Indicate UE support CPC-DTX operation on UL DPCCH");
        RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
             v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
             interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2,
             rrc_UE_RadioAccessCapabilityComp2_v770ext,discontinuousDpcchTransmission);
     
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
             v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
             interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2.discontinuousDpcchTransmission =
             rrc_UE_RadioAccessCapabilityComp2_v770ext_discontinuousDpcchTransmission_true;
      }

#ifdef FEATURE_WCDMA_HS_FACH
      if(rrcenhstate_nv_supports_e_fach() == TRUE)
      {
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
             v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
             interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2.m.hsdschReception_CellFachPresent = 1;
     
        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
             v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
             interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2.hsdschReception_CellFach =
             rrc_UE_RadioAccessCapabilityComp2_v770ext_hsdschReception_CellFach_true;
    
        WRRC_MSG0_HIGH("ENHSTATE:Sending Enh Cell_FACH capability");
  
  
      }
#endif /*FEATURE_WCDMA_HS_FACH*/
#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
    if (TRUE == rrcueci_nv_supports_edpcch_pwr_boost())
    {
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
      v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.m.v790nonCriticalExtensionsPresent =1;

      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
      v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v790nonCriticalExtensions.interRATHandoverInfo_v790ext.m.supportForEDPCCHPowerBoostingPresent = 1;

      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
      v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v790nonCriticalExtensions.interRATHandoverInfo_v790ext.supportForEDPCCHPowerBoosting = 
      rrc_InterRATHandoverInfo_v790ext_IEs_supportForEDPCCHPowerBoosting_true;
    }
#endif/*FEATURE_WCDMA_EDPCCH_POWER_BOOSTING*/
#ifdef FEATURE_WCDMA_16_QAM
      if(CHECK_WCDMA_OPTIONAL_FEATURE_16_QAM_SUPPORTED )
      {
         RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
         v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
         v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2,          
         rrc_UE_RadioAccessCapabilityComp2_v770ext,edch_PhysicalLayerCategory_extension);
         ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
         v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
         v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         interRATHandoverInfo_v770ext.ue_RadioAccessCapabilityComp2.edch_PhysicalLayerCategory_extension =
               MAX_UL_16QAM_HSUPA_CATEGORY;
      }
#endif
    }
    if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
    {
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.m.
        v790nonCriticalExtensionsPresent =1;

      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.m.v860NonCriticalExtensionsPresent = 1;

      RRC_MSG_COMMON_SET_BITMASK_IE(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.interRATHandoverInfo_v860ext,
        rrc_InterRATHandoverInfo_v860ext_IEs,ue_RadioAccessCapabilityComp2);

      RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.interRATHandoverInfo_v860ext.ue_RadioAccessCapabilityComp2);

#ifdef FEATURE_WCDMA_DC_HSDPA
      if((rrcueci_nv_supports_dc_hsdpa() == TRUE) && (!rrccsp_check_if_em_plmn()))
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790nonCriticalExtensions.v860NonCriticalExtensions.interRATHandoverInfo_v860ext.ue_RadioAccessCapabilityComp2, hsdsch_physical_layer_category_ext2);
 
           ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
            v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790nonCriticalExtensions.v860NonCriticalExtensions.interRATHandoverInfo_v860ext.ue_RadioAccessCapabilityComp2.
            hsdsch_physical_layer_category_ext2 = MIN(nv_hsdpa_category, RRC_DEFAULT_DC_HSDPA_CATEGORY);
      }
      else
#endif /*FEATURE_WCDMA_DC_HSDPA*/
      {
        WRRC_MSG0_HIGH("REL8:HSDPA Category isn't set from 21 to 35.");
      }
#ifdef FEATURE_WCDMA_TO_LTE
      if(rrcmcm_is_lte_mode_enabled())
      {
        /*Here fill the 860 extension, i.e RAC FDD List 3*/
        /*This is for compressed mode needed for LTE measurements, not needed for now*/
        if(rrcueci_append_ue_radio_access_capability_fdd3_list(&local_v860ext.ue_RadioAccessCapabBandFDDList3,lte_band_pref))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790nonCriticalExtensions.v860NonCriticalExtensions.interRATHandoverInfo_v860ext, ue_RadioAccessCapabilityInfo);
    
          ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
              v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790nonCriticalExtensions.v860NonCriticalExtensions.interRATHandoverInfo_v860ext.ue_RadioAccessCapabilityInfo =
            local_v860ext;
        }
      }
#endif
#ifdef FEATURE_MAC_I
      if(rrcueci_nv_supports_mac_i() == TRUE)
      {
      /*Add support of mac-i*/

        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790nonCriticalExtensions.v860NonCriticalExtensions.interRATHandoverInfo_v860ext.ue_RadioAccessCapabilityComp2, supportOfMACiis);

        ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790nonCriticalExtensions.v860NonCriticalExtensions.interRATHandoverInfo_v860ext.ue_RadioAccessCapabilityComp2.
          supportOfMACiis = rrc_UE_RadioAccessCapabilityComp2_v860ext_supportOfMACiis_true;

      }
#endif /* FEATURE_MAC_I */
    }
    
#ifdef FEATURE_WCDMA_DC_HSUPA
    if (rrcueci_nv_supports_dc_hsupa() == TRUE)
    {

      if(!RRC_CHECK_MSG_TYPE_IE (ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions,v880NonCriticalExtensionsPresent))
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions, v880NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions);

      }

      if(!RRC_CHECK_MSG_TYPE_IE (ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions,v920NonCriticalExtensionsPresent ))
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions,v920NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v920NonCriticalExtensions);

      }

      if(!RRC_CHECK_MSG_TYPE_IE (ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v920NonCriticalExtensions.interRATHandoverInfo_v920ext, ue_RadioAccessCapabilityComp2Present))
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v920NonCriticalExtensions.interRATHandoverInfo_v920ext, ue_RadioAccessCapabilityComp2);
        RRC_RESET_MSG_IE_PRESENT(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v920NonCriticalExtensions.interRATHandoverInfo_v920ext.ue_RadioAccessCapabilityComp2);
        
      }

      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v920NonCriticalExtensions.interRATHandoverInfo_v920ext.ue_RadioAccessCapabilityComp2,edch_PhysicalLayerCategory_extension2);
      ptr->v390NonCriticalExtensions.u.present.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
        v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v4d0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        v790nonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v920NonCriticalExtensions.interRATHandoverInfo_v920ext.ue_RadioAccessCapabilityComp2.edch_PhysicalLayerCategory_extension2 =
        MAX_DC_HSUPA_CATEGORY;
    }
#endif /* FEATURE_WCDMA_DC_HSUPA */
      

#ifdef FEATURE_WCDMA_REL10  
#error code not present
#endif /* FEATURE_WCDMA_REL10 */

  }
  else
  {
    ptr->v390NonCriticalExtensions.t = T_rrc_InterRATHandoverInfo_v390NonCriticalExtensions_absent;
  }

  WRRC_MSG2_HIGH("IHO_DEBUG: Unencoded msg= Preconfig - %d, start value - ",ptr->predefinedConfigStatusList.t,ptr->uE_SecurityInformation.t);

  /* Encodes the OTA message using ASN1. The memory for msg_ptr is allocated
     by the RRC procedure and the memory for encoded_sdu will be allocated
     by the following function */
  encoded_sdu = rrcasn1_encode_pdu(ptr, rrc_InterRATHandoverInfo_PDU);

  if(encoded_sdu.value == NULL)              /* Checks here whether encoding is*/
                                             /* ASN1 Encoding is failed        */
  {
    if (rrc_encode_info.encode_status == ASN1_OUT_MEMORY)
    {
      ERR_FATAL("No memory for encoding %d %d",
                rrc_encode_info.encode_status,
                rrc_encode_info.pdu_num, 0);
    }
    else if (rrc_encode_info.encode_status == ASN1_CONSTRAINT_VIOLATED)
    {
      ERR_FATAL("Constraint violation in ASN1 encoding %d %d",
                        rrc_encode_info.encode_status,
                        rrc_encode_info.pdu_num, 0);
    }
    else
    {
      ERR_FATAL("ASN1 encoding failed %d %d",
                        rrc_encode_info.encode_status,
                        rrc_encode_info.pdu_num, 0);
    }
  }

  /*check to see that the message was successfully encoded*/
  if(encoded_sdu.length != 0)
  {

    gtow_info.class_mark_ptr = rrc_malloc(encoded_sdu.length);
    WCDMA_MEMCPY(gtow_info.class_mark_ptr,
                 encoded_sdu.length,
                 encoded_sdu.value,
                 encoded_sdu.length);
    
    WRRC_MSG2_HIGH("IHO_DEBUG: Encoded msg 1st two bytes = %x, %x ",*(gtow_info.class_mark_ptr),*(gtow_info.class_mark_ptr+1));

    /*copy the message pointer and the message length to the RR parameters*/
    *(msg_ptr) = gtow_info.class_mark_ptr;
    *(msg_len) = (uint8)encoded_sdu.length;

    /*set the local flags to indicate that memory is allocated for the encoded buf
      and has to be deleted*/
    gtow_info.class_mark_info_present = TRUE;

    /* Allocate mem here , so that the buffer is not cleared when the grneral CnTX is freed.*/
  

  }
  else
  {
    WRRC_MSG0_ERROR("ASN1 encoding failed");
  }

  WRRC_MSG1_HIGH("Class Mark Len %d, Info",encoded_sdu.length);

  // The memory freeing for msg_ptr should be done here. using OSS compiler
  // functions. This function should free memory allocated for msg_ptr and as
  // well as linked lists in the message if they are any allocated.
  rrcasn1_free_pdu( (int) rrc_InterRATHandoverInfo_PDU, ptr);
  ptr = NULL;
  WRRC_MSG2_HIGH("IHO_DEBUG: Encoded msg 1st two bytes = %x, %x ",*(*(msg_ptr)),*((*msg_ptr)+1));

}

/*===========================================================================

FUNCTION RRC_GET_CLASSMARK_CHANGE_HANDOVER_INFO

DESCRIPTION

  This function builds a INTERRAT HANDOVER INFO message type which will be
  sent by GSM to UTRAN thru GERAN.  An ASN1 encoded message and message
  length is populated by RRC, the message and msg_length is used by RR to
  send this as part of Classmark Change to UTRAN via GERAN

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/
void rrc_get_classmark_change_handover_info
(
  byte ** msg_ptr,    /* The command to be sent to UTRAN */
  uint8 * msg_len,    /* Length of the message           */
  boolean irat_ho_info_compressed
)
{
#if defined (FEATURE_LTE_TO_WCDMA) || defined (FEATURE_WCDMA_TO_LTE)
  rrcmcm_update_lte_mode_enabled_status(FALSE);
#endif
#ifdef FEATURE_DUAL_SIM
  irat_plmn_id = rrc_get_rr_curr_camped_plmn_sub_id(rrc_get_as_id());
#else
  irat_plmn_id = rrc_get_rr_curr_camped_plmn();
#endif

  if(!rrccsp_is_wcdma_band_enabled())
  {
    WRRC_MSG0_ERROR("No W bands enabled. Sending msg_len = 0");
    *(msg_len) = 0;
    return;
  }

#ifdef FEATURE_GAN
  #error code not present
#endif
  {
  rrc_get_utran_classmark_change_handover_info(
      msg_ptr
      ,msg_len
#ifdef FEATURE_IRAT_HO_INFO_COMPRESSED
      #error code not present
#endif
#if defined (FEATURE_LTE_TO_WCDMA) || defined (FEATURE_WCDMA_TO_LTE)
#ifdef LTE_BAND_NUM
      ,SYS_LTE_BAND_MASK_CONST_NONE 
#else
       ,0
#endif
       ,0
#endif
     );
  }
}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION RRC_GET_CLASSMARK_CHANGE_HANDOVER_INFO_SUB_ID

DESCRIPTION

  This function builds a INTERRAT HANDOVER INFO message type which will be
  sent by GSM to UTRAN thru GERAN when DSDS is enabled. An ASN1 encoded message
  and message length is populated by RRC, the message and msg_length is used
  by RR to send this as part of Classmark Change to UTRAN via GERAN

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/

void rrc_get_classmark_change_handover_info_sub_id
(
  byte ** msg_ptr,    /* The command to be sent to UTRAN */
  uint8 * msg_len,    /* Length of the message           */
  boolean irat_ho_info_compressed,
  sys_modem_as_id_e_type as_id
)
{
  if (as_id != rrc_get_as_id())
  {
    WRRC_MSG2_ERROR("AS-id mismatch %d, internal %d",as_id, rrc_get_as_id());
  }
  rrc_get_classmark_change_handover_info
  (
    msg_ptr,/* The command to be sent to UTRAN */
    msg_len, /* Length of the message */
    irat_ho_info_compressed
  );
}
#endif
/*===========================================================================

FUNCTION RRCIHO_IS_WTOG_HO_ACTIVE


DESCRIPTION

  API to indicate if WTOG HO is active and ongoing.  Used by other procedures
  to query if WTOG HO is ongoing

DEPENDENCIES

  None

RETURN VALUE

TRUE: If WTOG HO is active
FALSE: IF WTOG is not active

SIDE EFFECTS

  None

===========================================================================*/
boolean rrciho_is_wtog_ho_active
(
 void
)
{
  /* Check the procedure substate and procedure the event accordingly */
  switch( rrciho_state )
  {
    case RRCIHO_WAIT_STANDBY_CNF:
    case RRCIHO_WAIT_INTERRAT_HO_CNF:
    case RRCIHO_WAIT_ACTIVE_CNF:
    case RRCIHO_WAIT_CHANNEL_CONFIG_CNF:
    case RRCIHO_WAIT_CELL_UPDATE_CNF:
      WRRC_MSG1_HIGH("IHO proc has ongoing WtoG HO iho substate %d", rrciho_state);
      return TRUE;

    default:
      WRRC_MSG1_HIGH("IHO proc has NO WtoG HO ongoing iho substate %d",rrciho_state);
      return FALSE;
  }
}
/*===========================================================================

FUNCTION RRC_DELETE_CLASSMARK_CHANGE_HANDOVER_INFO

DESCRIPTION

  This function deletes the CLASSMARK_CHANGE_HANDOVER_INFO asn1 type
  which was previously allocated to send to UTRAN via GERAN.
  RR calls this function after it has copied the contents of the
  Handover To Utran Info contents locally.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_delete_classmark_change_handover_info(void)
{
  if(gtow_info.class_mark_info_present == TRUE)
  {
    // memory freeing for encoded_sdu should be done here using OSS compiler
    // functions.
    rrc_free(gtow_info.class_mark_ptr);

    /*reset the static variables*/
    gtow_info.class_mark_info_present = FALSE;
    gtow_info.class_mark_ptr = NULL;
  }
  else
  {
    WRRC_MSG0_ERROR("No Classmark msg saved");
  }
}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION RRC_DELETE_CLASSMARK_CHANGE_HANDOVER_INFO_SUB_ID

DESCRIPTION

  This function deletes the CLASSMARK_CHANGE_HANDOVER_INFO asn1 type
  which was previously allocated to send to UTRAN via GERAN when DSDS is enabled.
  RR calls this function after it has copied the contents of the
  Handover To Utran Info contents locally.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrc_delete_classmark_change_handover_info_sub_id( sys_modem_as_id_e_type as_id)
{

  if (as_id != rrc_get_as_id())
  {
    WRRC_MSG2_ERROR("AS-id mismatch %d, internal %d",as_id, rrc_get_as_id());
  }

  rrc_delete_classmark_change_handover_info();
}
#endif
/*===========================================================================

FUNCTION   rrciho_send_sync_ind_for_gtow_ho

DESCRIPTION
  Sends SYNC Ind to MM after handover to utran is successful.
  Also sends HandoverToUTRANComplete to the network to indicate that we have
  successfully Handed Over to UTRAN from GSM and all GSM buffered messages
  have already been sent to the N/W.  This is also done after indication is
  sent to NAS to inform that HO is complete and Vocoder should be switched
  on for WCDMA

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrciho_send_sync_ind_for_gtow_ho(void)
{

 /* Reduce the TRM priority to BG TRAFFIC post successful L2W PS HO to facilitate lock
  * release for page read on GSM sub2. This has to be done after RCE substate is updated
  * to RRCRCE_FINAL
 */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE_TO_WCDMA)
  if((iho_interrat_type == RRCIHO_INTERRAT_LTE) 
#ifdef FEATURE_3GPP_CSFB
    &&(!rrc_csfb_call_status)
#endif
      )
  {
    rrc_check_send_change_priority();
  }
#endif
#ifdef FEATURE_LTE_TO_WCDMA
  /* is_cs_rab_present is not true for default config 0 and 1
    so change the below condition to handle this scenario */
  if((iho_interrat_type == RRCIHO_INTERRAT_GSM) || (gtow_info.is_cs_rab_present))
  {
#endif /* FEATURE_LTE_TO_WCDMA */
    mm_cmd_type *mm_cmd_ptr;                      /* Pointer to the MM Command */

  /* Allocate memory for SYNC_IND command */
    mm_cmd_ptr =  mm_rrc_get_cmd_buf(RRC_SYNC_IND);
#ifdef FEATURE_DUAL_SIM
    mm_cmd_ptr->cmd.rrc_sync_ind.as_id = rrc_get_as_id();
#endif

  /* Fill in parameters fro RRC_SYNC_IND command */
    mm_cmd_ptr->cmd.rrc_sync_ind.cn_domain_id= RRC_CS_DOMAIN_CN_ID;

    mm_cmd_ptr->cmd.rrc_sync_ind.cause = RRC_HANDOVER;

    /*set the rab info if it was included in the handover to utran
      message*/
    if(gtow_info.rab_info_included == TRUE)
    {
      mm_cmd_ptr->cmd.rrc_sync_ind.rab_id_present = TRUE;
#ifndef FEATURE_LTE_TO_WCDMA
      mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.rab_id = gtow_info.est_rabs.rab_id;

      if(gtow_info.est_rabs.rab_type == RRCRB_CS_VOICE_RAB)
      {
        mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_VOICE_CALL;
       /* Set the CS RAB established status to TRUE here to trigger FR2LTE in case UE moved to WCDMA for CSFB call */
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
        if(csfb_extended_status == CSFB_STARTED)
        {
          rrc_cs_rab_established = TRUE;
        }
#endif
      }
      else if(gtow_info.est_rabs.rab_type == RRCRB_CS_DATA_RAB)
      {
        /*HO of CS Data call from G to W is for futuristic use. Right now we don't use it.*/
        mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_DATA_CALL;        
      }
      else
      {
        WRRC_MSG1_ERROR("Something wrong with RAB Type for RAB-id %d",
              mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.rab_id);
        mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_INVALID_RAB_ID;
      }
#else
      mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.rab_id = gtow_info.est_rabs[gtow_info.cs_rab_index].rab_id;

      if(gtow_info.est_rabs[gtow_info.cs_rab_index].rab_type == RRCRB_CS_VOICE_RAB)
      {
        mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_VOICE_CALL;
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
        if(csfb_extended_status == CSFB_STARTED)
        {
          rrc_cs_rab_established = TRUE;
        }
#endif
      }
      else if(gtow_info.est_rabs[gtow_info.cs_rab_index].rab_type == RRCRB_CS_DATA_RAB)
      {
        /*HO of CS Data call from G to W is for futuristic use. Right now we don't use it.*/
        mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_DATA_CALL;        
      }
      else
      {
        WRRC_MSG1_ERROR("Something wrong with RAB Type for RAB-id %d",
              mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.rab_id);
        mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_INVALID_RAB_ID;
      }
#endif /* FEATURE_LTE_TO_WCDMA */

      mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.action = RAB_ESTABLISHED;

      /*reset the gtow_info static variable for rab info*/
      gtow_info.rab_info_included = FALSE;

#ifdef FEATURE_VOC_AMR_WB
      /* Fill in the codec type used in RRC_SYNC_IND*/
      if(mm_cmd_ptr->cmd.rrc_sync_ind.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
      {
        if(rrc_get_current_codec_mode() == MVS_MODE_AMR_WB )
        {
          mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_WB_AMR;
        }
        else
        {
          mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_NB_AMR;
        }
      }
      else
      {
        mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_NONE;
      }
      WRRC_MSG1_HIGH("Codec %d in RRC_SYNC_IND",mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.codec);    
#endif /*FEATURE_VOC_AMR_WB*/
    }
    /*else set it to false*/
    else
    {
      mm_cmd_ptr->cmd.rrc_sync_ind.rab_id_present = FALSE;
    }

#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
    #error code not present
#endif
    rrc_send_rrc_message_to_mm(mm_cmd_ptr);               /* Sends the command to MM */

    WRRC_MSG2_HIGH("Dispatching out MM cmd / Sending RRC_SYNC_IND to MM for RAB type %d RAB-id %d", 
            mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type, 
            mm_cmd_ptr->cmd.rrc_sync_ind.rab_info.rab_id);

#ifdef FEATURE_LTE_TO_WCDMA
  }
#endif /* FEATURE_LTE_TO_WCDMA */

#ifdef FEATURE_LTE_TO_WCDMA
  if(gtow_info.is_ps_rab_present)
  {
    uint16 i;
    for(i = 0; i < gtow_info.number_of_est_rabs; i++)
    {
      if(gtow_info.est_rabs[i].cn_domain == RRC_PS_DOMAIN_CN_ID)
      {
        rabm_cmd_type *rabm_ptr;
        /* It's a PS Handover. Send the RABMAS_RAB_ESTABLISH_IND primitive to RABM. */
        if ((rabm_ptr = rabm_get_cmd_buf()) == NULL)
        {
          ERR_FATAL("Can't send RABM cmd - out of memory!", 0, 0, 0);
        }
        rabm_ptr->header.cmd_id = RABMAS_RAB_ESTABLISH_IND;
        rabm_ptr->header.message_set = MS_RABM_RRC;
        rabm_ptr->cmd.rrc_establish_ind.rabid = (rabid_T)(gtow_info.est_rabs[i].rab_id);
#ifdef FEATURE_DUAL_SIM
        rabm_ptr->cmd.rrc_establish_ind.as_id = rrc_get_as_id();
#endif
    
        /* Put the command on RABM queue */
        WRRC_MSG1_HIGH("LTOW_PSHO: Sending RAB_ESTABLISH_IND to RABM RABid %d",
                                                              gtow_info.est_rabs[i].rab_id);
        rabm_put_cmd(rabm_ptr);
      }
    }
  }
#endif /* FEATURE_LTE_TO_WCDMA */

  /*send the OTA message to UTRAN*/
  rrciho_send_handover_to_utran_complete();

  /*only if ciphering is started on gsm side do we want to start ciphering
    on wcdma side*/
  if((gtow_info.gsm_ciph_status == TRUE)
#ifdef FEATURE_LTE_TO_WCDMA
    && ((iho_interrat_type == RRCIHO_INTERRAT_GSM)||
        ((iho_interrat_type == RRCIHO_INTERRAT_LTE) &&
        (gtow_info.is_cs_rab_present)))
#endif
    )
  {
    /*send step 2 ciphering configuration to SMC procedure*/
    WRRC_MSG0_HIGH("Processing step 2 HO request ");		
    (void)rrcsmc_step_2_ho_ciph_req(gtow_info.count_c_act_time);
  }


#ifdef FEATURE_LTE_TO_WCDMA
  /*Send Cipher sync indication so that W and G keys are in sync
    This is needed for LtoW scenario because NAS always provides 
    mapped keys when handover time and not at every authentication 
    in LTE. For GtoW handover NAS provides keys for every Auth in G*/
  if(iho_interrat_type == RRCIHO_INTERRAT_LTE)
  {
    rrciho_send_cipher_sync_ind();
  }
#endif

}

#if defined (FEATURE_LTE_TO_WCDMA) || defined (FEATURE_WCDMA_TO_LTE)
/*===========================================================================

FUNCTION RRCIHO_HANDLE_UTRA_CAP_REQ_FROM_LTE


DESCRIPTION

  This function handle the WCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ from LTE RRC

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrciho_handle_utra_cap_req_from_lte
(
  wcdma_rrc_lte_utra_capabilities_req_type *utra_cap_req
)
{
  wcdma_rrc_lte_utra_capabilities_rsp_type utra_cap_rsp;
  byte *cap_msg_ptr = NULL;
  uint8 cap_msg_len;
  uint16 idx=0;

  WRRC_MSG0_HIGH("LTOW: Recvd WCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ from LTE RRC");

  irat_plmn_id = utra_cap_req->camped_plmn;
  /*update the modes and band preferences*/
  rrc_update_band_preference(utra_cap_req->rat_capabilities.wcdma_band_preference);
  rrcmcm_update_lte_mode_enabled_status(TRUE);

  if(!utra_cap_req->rat_capabilities.geran_is_supported)
  {
    rrcmcm_update_dual_mode_enabled_status(FALSE);
  }
  else
  {
    rr_rrc_set_band_pref_internal(utra_cap_req->rat_capabilities.geran_band_preference);
    rrcmcm_update_dual_mode_enabled_status(TRUE);
  }

  /*
    Call the existing function
    In spec, predifined configurations are needed to be filled in different ways for E-UTRA and other RATS.
    As we dont fill the predefined configuration stuff, there is not need of change in arguments
  */
  rrc_get_utran_classmark_change_handover_info(
       &cap_msg_ptr,&cap_msg_len
#ifdef FEATURE_IRAT_HO_INFO_COMPRESSED
       #error code not present
#endif
#if defined (FEATURE_LTE_TO_WCDMA) || defined (FEATURE_WCDMA_TO_LTE)
       ,utra_cap_req->rat_capabilities.lte_band_preference
       ,utra_cap_req->l2w_psho_enabled
#endif
    );

  utra_cap_rsp.capabilities_container_length = 0;
  if(cap_msg_ptr != NULL)
  {
    for(idx=0;idx < cap_msg_len && idx < WCDMA_MAX_CAP_CONTAINER_LENGTH;idx++)
    {
      utra_cap_rsp.capabilities_container[idx] = *(cap_msg_ptr+idx);
    }
    utra_cap_rsp.capabilities_container_length = cap_msg_len;
  }

  rrc_send_lte_cmd(&utra_cap_rsp.msg_hdr,WCDMA_RRC_LTE_UTRA_CAPABILITIES_RSP,sizeof(wcdma_rrc_lte_utra_capabilities_rsp_type));
  WRRC_MSG0_HIGH("LTOW: WCDMA_RRC_LTE_UTRA_CAPABILITIES_RSP sent to LTE RRC");

  rrc_delete_classmark_change_handover_info();
}
#endif


/*===========================================================================

FUNCTION RRCIHO_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Inter-RAT Handover module. This function is called by the
  dispatcher whenever an event is received for this module.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrciho_event_handler(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  WRRC_MSG2_HIGH("function rrciho_event_handler is called in rrciho_state rrciho_state_e_type_value%d : Event is received by IHO: %d", rrciho_state,cmd_ptr->cmd_hdr.cmd_id);
  rrc_crash_debug_log_proc_substates(RRC_PROCEDURE_IHO,rrciho_state,cmd_ptr);

  /*we have added support to go from connected mode to inactive mode using
    MODE_CHANGE_REQ primitive..  When this happens MCM sends mode_change_ind,
    we honour this message before all other messages clear rrciho's initial
    database and go to RRCIHO_INITIAL substate, then process nothing else*/
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_MODE_CHANGE_IND) &&
     (cmd_ptr->cmd.mode_change_ind.from_mode == RRC_MODE_CHANGE) &&
     (cmd_ptr->cmd.mode_change_ind.to_mode == RRC_MODE_INACTIVE))
  {
    rrciho_init_data();
    return;
  }

#if defined (FEATURE_LTE_TO_WCDMA) || defined (FEATURE_WCDMA_TO_LTE)
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_LTE_RRC_CMD) &&
      (cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == WCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ))
  {
    rrciho_handle_utra_cap_req_from_lte(&cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.wcdma_rrc_lte_utra_capabilities_req);
    return;
  }
#endif

  /* Check the procedure substate and procedure the event accordingly */
  switch( rrciho_state )
  {
    case RRCIHO_INITIAL:
      /* Call the event handler for Initial substate */
      rrciho_initial_substate_event_handler( cmd_ptr );
      break;

    case RRCIHO_WAIT_STANDBY_CNF:
      /* Call the event handler for waiting for standby cnf substate */
      rrciho_wait_standby_cnf_substate_event_handler( cmd_ptr );
      break;

    case RRCIHO_WAIT_INTERRAT_HO_CNF:
      /* Call the event handler for wait for inter-RAT handover cnf substate */
      rrciho_wait_interrat_ho_cnf_substate_event_handler( cmd_ptr );
      break;

    case RRCIHO_WAIT_ACTIVE_CNF:
      /* Call the event handler for waiting for active confirmation substate */
      rrciho_wait_active_cnf_substate_event_handler(cmd_ptr);
      break;

    case RRCIHO_WAIT_CHANNEL_CONFIG_CNF:
      /* Call the event handler for waiting for wait for channel config confirmation substate */
      rrciho_wait_channel_config_cnf_substate_event_handler(cmd_ptr);
      break;

    case RRCIHO_WAIT_CELL_UPDATE_CNF:
      /* Call the event handler for waiting for wait for cell update confirmation substate */
      rrciho_wait_cell_update_cnf_substate_event_handler(cmd_ptr);
      break;

    case RRCIHO_WAIT_ABORT_CNF:
      /* Call the event handler for wait for abort cnf substate */
      rrciho_wait_abort_cnf_substate_event_handler( cmd_ptr );
      break;

    case RRCIHO_WAIT_ACQ_CNF:
      /* Call the event handler for wait for acq cnf substate */
      rrciho_wait_acq_cnf_event_handler( cmd_ptr );
      break;

    case RRCIHO_WAIT_IRAT_IN_PROGRESS_CNF:
      /* Call the event handler for wait for interrat in progress cnf substate */
      rrciho_wait_irat_in_progress_cnf_event_handler( cmd_ptr );
      break;

    case RRCIHO_WAIT_INACTIVE_CNF:
      /* Call the event handler for wait for interrat in progress cnf substate */
      rrciho_wait_inactive_cnf_event_handler( cmd_ptr );
      break;

    case RRCIHO_WAIT_CHAN_CONFIG_GTOW_CNF:
      /* Call the event handler for wait for channel config state to go to dch in
         case of a GSM to WCDMA transition */
      rrciho_wait_chan_config_gtow_cnf_event_handler( cmd_ptr );
      break;

    case RRCIHO_WAIT_CIPH_CONFIG_GTOW_CNF:
      /* Call the event handler for wait for ciph config state to go to dch in
         case of a GSM to WCDMA transition */
      rrciho_wait_ciph_config_gtow_cnf_event_handler( cmd_ptr );
      break;

    default:
      /* Invalid substate - should never happen. Clear
         all procedure vars */
      rrciho_init_data();
      ERR_FATAL("Invalid IHO Substate %d", rrciho_state, 0, 0);
      break;
  }/* end switch */
  return;
} /* end of rrciho_event_handler */


/*===========================================================================

FUNCTION   RRCIHO_INIT_PROCEDURE

DESCRIPTION

  This function initializes the inter-RAT handover procedure.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrciho_init_procedure(void)
{
  gsm_commands.n =0;

#ifdef FEATURE_WCDMA_TO_LTE
  procedure_info.parms.eutra.eutra_container_size = 0;
  procedure_info.parms.eutra.eutra_container_ptr = NULL;
#endif

  /* Initialize the procedure data */
  rrciho_init_data();

  //predefined config's are not supported, set it to zero for now
  gtow_info.predef_config_id = 0;
  gtow_info.config_type = IHO_GTOW_PREDEF_CONFIG;

  rrciho_init_default_configuration();
  /* Register the state change from any state to Idle */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_IHO,      /* Procedure name */
                             RRC_STATE_WILDCARD,     /* From State     */
                             RRC_STATE_DISCONNECTED  /* To State       */
                           );
}

/*===========================================================================

FUNCTION   rrciho_initialize_rb_list

DESCRIPTION

  Initializes GTOW information for Radio Bearer list

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrciho_initialize_rb_list_r8
(
   rrc_RB_InformationSetupList_r8 *rb_ptr
#ifdef FEATURE_LTE_TO_WCDMA
, uint8 index
#endif
)
{

  uint8 rb_count=0;

  while ((rb_ptr!=NULL )&& (rb_ptr->n > rb_count))
  {
    /* Do a safety check on the count. We can't do more than 32
    configs in a command */
    if ( rb_count >= MAX_RB_PER_RAB )
    {
      WRRC_MSG0_HIGH("MAX RB count exceeded!");
      break;
    }

#ifndef FEATURE_LTE_TO_WCDMA
    gtow_info.est_rabs.rb_for_rab[rb_count].rb_id = (uint8)(rb_ptr->elem[rb_count].rb_Identity);
#else
    gtow_info.est_rabs[index].rb_for_rab[rb_count].rb_id = (uint8)(rb_ptr->elem[rb_count].rb_Identity);
#endif
    rb_count++;

  }/* end while */

  /* Save the number of RBs for this RAB */
#ifndef FEATURE_LTE_TO_WCDMA
  gtow_info.est_rabs.num_rbs_for_rab = rb_count;
#else
  gtow_info.est_rabs[index].num_rbs_for_rab = rb_count;
#endif
}/*rrciho_initialize_rb_list_r7*/

/*===========================================================================

FUNCTION   rrciho_initialize_srb_list_r8

DESCRIPTION

  Initializes GTOW information for Signalling Radio Bearer list


DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrciho_initialize_srb_list_r8
(
  rrc_SRB_InformationSetupList_r8 *srb_ptr
)
{
  rrc_RB_Identity srb_count=0;
  rrc_RB_Identity rb_id = CCCH_RADIO_BEARER_ID;

  while ((srb_ptr!=NULL )&& (srb_ptr->n > srb_count ))
  {
    /* Get the SRB ID from the message if present */
    if(srb_ptr->elem[srb_count].m.rb_IdentityPresent)
    {
      rb_id = srb_ptr->elem[srb_count].rb_Identity;
    }
    else
    {
      /* Else, increment from the previous SRB Id */
      rb_id++;
    }        

    /* Do a safety check on the count. We can't do more than 32
    configs in a command */
    if ( srb_count >= UE_MAX_SRB_SETUP )
    {
      WRRC_MSG0_HIGH("MAX SRB count exceeded!");
      break;
    }

    gtow_info.srb_id[srb_count] = rb_id;
    srb_count++;

  }/* end while */

  /* Save the number of SRBs */
  gtow_info.num_srb = srb_count;
}

#ifndef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION   rrciho_update_rab_info_r8

DESCRIPTION

  updates GTOW information for Radio Bearer list

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_RAB_InformationSetupList_r8 *  rrciho_update_rab_info_r8
(
    uint32 * rab_identity_choice
#ifdef FEATURE_WCDMA_REL9
      ,rrc_msg_ver_enum_type release
#endif /* FEATURE_WCDMA_REL9 */

)
{
      rrc_RB_InformationSetupList_r8  *rb_list_r8 = NULL;
      rrc_HandoverToUTRANCommand_r8_IEs * ho_utran_r8;
         rrc_RAB_InformationSetupList_r8 *rab_list_r8 = NULL;
#ifdef FEATURE_WCDMA_REL9
    rrc_HandoverToUTRANCommand_r9_IEs * ho_utran_r9;
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */

#ifdef FEATURE_WCDMA_REL9
    if (release ==  MSG_REL8)
    {
#endif /* FEATURE_WCDMA_REL9 */


      ho_utran_r8 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8
             .handoverToUTRANCommand_r8;
   if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == ho_utran_r8->specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.rab_Identity.t)
   {
     *rab_identity_choice = T_rrc_RAB_Identity_gsm_MAP_RAB_Identity;
   
     /* GSM_MAP RAB Id. Check if it's valid by calling the
        appropriate NAS entity. First convert it to internal
        format. */
   
     gtow_info.est_rabs.rab_id =
        rrcrb_translate_gsm_map_rab_id(ho_utran_r8->specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.
                                       rab_Identity.u.gsm_MAP_RAB_Identity);
   
     gtow_info.est_rabs.re_est_timer = ho_utran_r8->specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.re_EstablishmentTimer;
   
     rb_list_r8 =&ho_utran_r8->specificationMode.u.complete.rab_InformationSetupList.elem[0].rb_InformationSetupList;
   
     rrciho_initialize_rb_list_r8(rb_list_r8);
   
     rab_list_r8 = &ho_utran_r8->specificationMode.u.complete.rab_InformationSetupList;
   
   #ifdef FEATURE_VOC_AMR_WB
     if(rab_list_r8->elem[0].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
     {
       gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_AMR;
        /* in the G-> W handover check the Nas-Sync-indicator for the codec type */        
       if(rab_list_r8->elem[0].rab_Info.m.nas_Synchronisation_IndicatorPresent)
       {
         gtow_info.est_rabs.nas_Synchronisation_Indicator = 
             rrc_translate_nas_sync_indicator(&(rab_list_r8->elem[0].rab_Info.nas_Synchronisation_Indicator));
         
         if( gtow_info.est_rabs.nas_Synchronisation_Indicator == MVS_MODE_NONE )
         {
           procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
           WRRC_MSG1_HIGH("Invalid configuration .. codec not supported %d",gtow_info.est_rabs.nas_Synchronisation_Indicator );
           return NULL;
         }
       }
       rrc_set_ordered_codec_mode(gtow_info.est_rabs.nas_Synchronisation_Indicator);
     }
   
   #endif /*FEATURE_VOC_AMR_WB*/

   return rab_list_r8;
     /*lint +e740 */
   }
#ifdef FEATURE_WCDMA_REL9
    }
    else if (release ==  MSG_REL9)
    {
      ho_utran_r9 = &gtow_info.ho_ptr->u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
        later_r8_criticalExtensions.u.r9.handoverToUTRANCommand_r9;
      if(T_rrc_RAB_Identity_gsm_MAP_RAB_Identity == ho_utran_r9->specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.rab_Identity.t)
      {
        *rab_identity_choice = T_rrc_RAB_Identity_gsm_MAP_RAB_Identity;
      
        /* GSM_MAP RAB Id. Check if it's valid by calling the
           appropriate NAS entity. First convert it to internal
           format. */
      
        gtow_info.est_rabs.rab_id =
           rrcrb_translate_gsm_map_rab_id(ho_utran_r9->specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.
                                          rab_Identity.u.gsm_MAP_RAB_Identity);
      
        gtow_info.est_rabs.re_est_timer = ho_utran_r9->specificationMode.u.complete.rab_InformationSetupList.elem[0].rab_Info.re_EstablishmentTimer;
      
        rb_list_r8 =&ho_utran_r9->specificationMode.u.complete.rab_InformationSetupList.elem[0].rb_InformationSetupList;
      
        rrciho_initialize_rb_list_r8(rb_list_r8);
      
        rab_list_r8 = &ho_utran_r9->specificationMode.u.complete.rab_InformationSetupList;
      
      #ifdef FEATURE_VOC_AMR_WB
        if(rab_list_r8->elem[0].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
        {
          gtow_info.est_rabs.nas_Synchronisation_Indicator = MVS_MODE_AMR;
           /* in the G-> W handover check the Nas-Sync-indicator for the codec type */        
          if(rab_list_r8->elem[0].rab_Info.m.nas_Synchronisation_IndicatorPresent)
          {
            gtow_info.est_rabs.nas_Synchronisation_Indicator = 
                rrc_translate_nas_sync_indicator(&(rab_list_r8->elem[0].rab_Info.nas_Synchronisation_Indicator));
            
            if( gtow_info.est_rabs.nas_Synchronisation_Indicator == MVS_MODE_NONE )
            {
              procedure_info.failure_cause = (uint32)RR_RRC_INVALID_CONFIGURATION;
              WRRC_MSG1_HIGH("Invalid configuration .. codec not supported %d",gtow_info.est_rabs.nas_Synchronisation_Indicator );
              return NULL;
            }
          }
          rrc_set_ordered_codec_mode(gtow_info.est_rabs.nas_Synchronisation_Indicator);
        }
      
      #endif /*FEATURE_VOC_AMR_WB*/
   
      return rab_list_r8;
        /*lint +e740 */
      }
    }
#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */

   return NULL;
}
#endif /*FEATURE_LTE_TO_WCDMA */

/*===========================================================================

FUNCTION   is_iho_w2g_active

DESCRIPTION

 Returns whether IRAT W2G is active or not

DEPENDENCIES

  None.

RETURN VALUE

  boolean

SIDE EFFECTS

  None.

===========================================================================*/
boolean is_iho_w2g_active()
{
  if((rrciho_state != RRCIHO_INITIAL) &&
     ((procedure_info.handover_type == T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM)
#ifdef FEATURE_LTE_TO_WCDMA
     || (procedure_info.handover_type == T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA)
#endif
     )
    )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }  
}



/*===========================================================================

FUNCTION   rrciho_free_sib16

DESCRIPTION

 Frees the ptr to sysInfoType16 previously allocated.

DEPENDENCIES

  None.

RETURN VALUE

   FAILURE if an error occurred while freeing else
   SUCCESS.


SIDE EFFECTS

  None.

===========================================================================*/

void rrciho_free_sib16 
(
  void
)
{
  if(gtow_info.sib16_ptr != NULL)
  {
    rrcasn1_free_pdu((int)rrc_SysInfoType16_PDU, gtow_info.sib16_ptr);
    gtow_info.sib16_ptr = NULL;
  }
  else
  {
    WRRC_MSG0_HIGH("RRCIHO FREE SIB16: gtow_info.sib16_ptr is NULL, no need to free");
  }
}


/*===========================================================================

FUNCTION rrciho_is_gtow_ho_active

DESCRIPTION 
    This function returns status of GTOW HO.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
boolean rrciho_is_gtow_ho_active(void)
{
  /* Check the procedure substate and procedure the event accordingly */
  switch( rrciho_state )
  {
    case RRCIHO_WAIT_ACQ_CNF:
    case RRCIHO_WAIT_IRAT_IN_PROGRESS_CNF:
    case RRCIHO_WAIT_INACTIVE_CNF:
    case RRCIHO_WAIT_CHAN_CONFIG_GTOW_CNF:
    case RRCIHO_WAIT_CIPH_CONFIG_GTOW_CNF:
      WRRC_MSG1_HIGH("IHO proc has ongoing GTOW HO iho substate %d", rrciho_state);
      return TRUE;

    default:
      return FALSE;
  }
}
#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
/*===========================================================================

FUNCTION rrciho_check_and_handle_mdsp_halt_during_gtow_handover

DESCRIPTION 
    This funcrion takes care of mDSP halt recovery when GTOW HO
    is in progress.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
boolean rrciho_check_and_handle_mdsp_halt_during_gtow_handover(void)
{
  boolean handled_mdsp_halt = TRUE;
  WRRC_MSG1_HIGH("IHO state %d",rrciho_state);
  switch(rrciho_state)
  {
    case RRCIHO_WAIT_CHAN_CONFIG_GTOW_CNF:
 #ifdef FEATURE_VOC_AMR_WB
      gtow_info.est_rabs.nas_Synchronisation_Indicator= rrc_get_current_codec_mode();
      rrc_set_ordered_codec_mode(gtow_info.est_rabs.nas_Synchronisation_Indicator);
#endif/*FEATURE_VOC_AMR_WB*/
      break;
    case RRCIHO_WAIT_CIPH_CONFIG_GTOW_CNF:
    case RRCIHO_WAIT_ACQ_CNF:
     break;
    default:
      handled_mdsp_halt = FALSE;
      break;
  }
  if(handled_mdsp_halt)
  {
    send_change_mode_req(RRC_MODE_INACTIVE);
    procedure_info.failure_cause = (uint32)RR_RRC_PHYSICAL_CHANNEL_FAILURE;
    rrciho_state = RRCIHO_WAIT_INACTIVE_CNF;
  }
  return handled_mdsp_halt;
}

/*===========================================================================

FUNCTION rrciho_check_and_handle_mdsp_halt_during_wtog_handover

DESCRIPTION 
    This function checks if WTOG HO is in progress, and then takes
    care of mDSP halt recovery.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
boolean rrciho_check_and_handle_mdsp_halt_during_wtog_handover(void)
{
  boolean handled_mdsp_halt = TRUE;
  WRRC_MSG1_HIGH("W2G IHO substate %d ",rrciho_state);
  switch(rrciho_state)
  {
    case RRCIHO_WAIT_INTERRAT_HO_CNF:
    case RRCIHO_WAIT_ABORT_CNF:
      WRRC_MSG0_HIGH("InterRAT is active, cannot handle MDSP halt");
      handled_mdsp_halt = FALSE;
      break;
    case RRCIHO_WAIT_STANDBY_CNF:
      WRRC_MSG0_HIGH("Can't handle when waiting for suspend CNF");
      handled_mdsp_halt = FALSE;
      break;
    case RRCIHO_WAIT_ACTIVE_CNF:
    case RRCIHO_WAIT_CHANNEL_CONFIG_CNF:
    case RRCIHO_WAIT_CELL_UPDATE_CNF:
      rrciho_init_data();
      break;
  }
  return handled_mdsp_halt;
}
#endif
/*===========================================================================

FUNCTION   rrciho_is_cs_data_rab_present

DESCRIPTION
  This function indicates whether any voice RAB is present or not in gtow est_rabs database.

      
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE: if CS DATA RAB exist in gtow est_rabs database
  FALSE: if CS DATA RAB doesnt exist gtow in est_rabs database


SIDE EFFECTS

  None.

===========================================================================*/ 


boolean rrciho_is_cs_data_rab_present (void)
{
  boolean status = FALSE;
#ifdef FEATURE_LTE_TO_WCDMA
  if(gtow_info.is_cs_rab_present)
  {
    if ( RRCRB_CS_DATA_RAB == gtow_info.est_rabs[gtow_info.cs_rab_index].rab_type )
    {
      status = TRUE;
    }
  }
#else
  if(gtow_info.est_rabs.rab_type == RRCRB_CS_DATA_RAB)
  { 
    status =  TRUE;
  }
#endif /* FEATURE_LTE_TO_WCDMA */  
  return status;
}

/*===========================================================================

FUNCTION rrciho_is_dl_ps_zero_zero_size_configured_for_all_rabs

DESCRIPTION

  This function fills in the transport block size per PS RAB

  
DEPENDENCIES


RETURN VALUE
  Returns FAILURE if PS RAB id doesn't find the size.  Otherwise returns SUCCESS.
 

SIDE EFFECTS

  None

===========================================================================*/
rrc_ps_data_rate_e_type rrciho_is_dl_ps_zero_zero_size_configured_for_all_rabs
(
void
)
{
  int i;
  rrc_RB_Identity rb_id;
  rrc_proc_e_type                 proc_id;
  rrcllc_oc_process_state_e_type  process_state; 
  rrcllc_oc_set_status_e_type     oc_status;
  rrc_ps_data_rate_e_type ps_zero_rb_found = RRC_PS_DATA_RATE_RAB_NOT_CONFIGURED;
  oc_status = rrcllc_get_ordered_config_state_and_proc(&proc_id,&process_state);

  if (!rrciho_is_gtow_ho_active()|| (RRC_PROCEDURE_IHO != proc_id))
  {
     return ps_zero_rb_found;
  }

#ifdef FEATURE_LTE_TO_WCDMA
  if(gtow_info.is_ps_rab_present)
  {
    for(i = 0; i < gtow_info.number_of_est_rabs; i++)
    {
      if(gtow_info.est_rabs[i].cn_domain == RRC_PS_DOMAIN_CN_ID)
      {
        ps_zero_rb_found = RRC_PS_DATA_RATE_ZERO_CONFIGURED;
        rb_id = gtow_info.est_rabs[i].rb_for_rab[0].rb_id;
        if(rrcllc_get_dl_ded_rate(RRC_STATE_CELL_DCH,rb_id) != 0)
        {
          ps_zero_rb_found = RRC_PS_DATA_RATE_NON_ZERO_CONFIGURED;
          break;
        }
      }
    }
  }
#endif /* FEATURE_LTE_TO_WCDMA */
 return ps_zero_rb_found;
}
/*===========================================================================

FUNCTION   rrciho_send_cipher_sync_ind

DESCRIPTION

  This function sends indication to RR that the new keys are activated

DEPENDENCIES

  None.

RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
void rrciho_send_cipher_sync_ind(void)
{
  /*declare var of struct abort confirm type*/
  rrc_cipher_sync_ind_type gtow_cipher_sync_ind;


  /* Send the RRC_CIPHER_SYNC_IND message to RR */
  rr_send_rrc_message_to_rr_internal((void *)(&(gtow_cipher_sync_ind)), RRC_CIPHER_SYNC_IND,
                            sizeof(rrc_cipher_sync_ind_type));
  WRRC_MSG0_HIGH("Sent the RRC_CIPHER_SYNC_IND to RR");
  return;
}
/*===========================================================================
FUNCTION rrciho_get_current_substate()

DESCRIPTION
  This function will return current RRC IHO procedure substate
DEPENDENCIES
  None

RETURN VALUE
  return current rrc_iho sub state.
SIDE EFFECTS
none
===========================================================================*/
uint8  rrciho_get_current_substate()
{
  return rrciho_state;
}

/*===========================================================================
FUNCTION   rrciho_return_interrat_type

DESCRIPTION
returns interrat ho type

DEPENDENCIES
  None.

RETURN VALUE
boolean

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
boolean rrciho_return_interrat_type(void)
{
#ifdef FEATURE_LTE_TO_WCDMA
  return ((iho_interrat_type == RRCIHO_INTERRAT_GSM)? TRUE:FALSE);
#else
  return TRUE;
#endif
}

/*===========================================================================
FUNCTION   rrciho_return_iho_abort

DESCRIPTION
returns iho abort

DEPENDENCIES
  None.

RETURN VALUE
uint8

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif

uint8 rrciho_return_iho_abort(void)
{
  return ((rrciho_abort == TRUE)?1:0);
}


/*===========================================================================
FUNCTION   rrciho_return_iho_substate

DESCRIPTION
returns iho substate

DEPENDENCIES
  None.

RETURN VALUE
rrciho_state_e_type


SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrciho_return_iho_substate(void)
{
  return (uint8)rrciho_state;
}

/*===========================================================================
FUNCTION   rrciho_return_default_config_id

DESCRIPTION
returns default config id

DEPENDENCIES
  None.

RETURN VALUE
uint32

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint32 rrciho_return_default_config_id(void)
{
  return gtow_info.default_config_id;
}

/*===========================================================================
FUNCTION   rrciho_return_config_type

DESCRIPTION
returns config type

DEPENDENCIES
  None.

RETURN VALUE
rrciho_gtow_config_e_type

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrciho_return_config_type(void)
{
  return (uint8)gtow_info.config_type;
}
