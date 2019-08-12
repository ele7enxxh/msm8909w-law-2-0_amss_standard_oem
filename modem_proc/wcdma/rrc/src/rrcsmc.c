/*===========================================================================
                  R R C  SECURITY MODE COMMAND

DESCRIPTION
  This  file implements functions for Security Mode Command
  procedure.  Ciphering and Integrity related IE's received
  in the SMC message are received and processed.


EXTERNALIZED FUNCTIONS

  rrcsmc_procedure_event_handler
    This function is the event handler for all events that are sent to the
    Security Mode Command procedure.

  rrcsmc_init_procedure
    This function initializes required parameters for Security Mode
    procedure.

  rrcsmc_append_hfns_to_rel_ind 
    This function appends PS and CS domain HFN's to RRC_REL_IND
    
  rrcsmc_append_ul_integrity_prot_act_info
    The function appends UL Integrity Activation Info IE to the client
    functions.
    
  rrcsmc_process_integrity_protection_mode_info
    This function processes the Integrity Protection Mode Info IE sent
    to it.
    
  rrcsmc_append_start_list
    This function appends the latest START_LIST.
    
  rrcsmc_append_uplink_integrity_check_info
    This function appends Integrity Check Info IE to UL messages.
    
  rrcsmc_check_downlink_integrity
    This function verifies Integrity on messages received on 
    downlink.
    
  rrcsmc_init_integrity_ciphering_config
    This function initializes Integrity Protection Information.

  rrcsmc_update_integrity_ciphering_config
    This function updates Integrity Protection Information.

  rrcsmc_is_ciphering_enabled
    This function checks to see if Ciphering is enabled or not.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcsmc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/16   sp      Modified the conditional logics used to detect SRNS relocation cases
05/25/16   sp      Made changes to convert ota metric msg_id to enum 
05/25/16   ad      Made changes to log important parms from OTAs
05/25/16   sp      Made changes for Mav debug snapshot
05/19/16   as      Made changes to reset the CS tm affected flag during PS RAB setup
05/09/16   nr      Made changes to remove irrelevant F3s
11/04/15   ad      Made changes not to apply the pending cs keys on wtog handover
10/16/15   as      Added code to support LTA durting activation time
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
12/31/14   sp      Made changes to reset stored security keys flag after DDS switch
12/15/14   sg      Made changes to copy a valid key to int key old data base before integrity is started
11/19/14   sg      Made changes to indicate cipher sync to GSM when going to IDLE after
                   applying new key                    
11/10/14   sp      Made changes to save procedure substates in crash debug info
10/13/14   sp      Made changes to not to update ciphering parameters during CCCH setup during DCH to FACH transition.
09/17/14   sg      Made changes to purge all SRBs except SRB2
                   when re-establishing during SRNS relocation
08/04/14   sp      Made changes to fix compilation errors
07/23/14   sp      Made changes to remove F3s as part of FR19999
07/21/14   ac      NV reorganization fixes in rrcsmc_get_nv_items
07/18/14   ad      Made changes to reduce F3s
07/15/14   ac      NV reorganization
12/30/13   sg      Made changes to add initialization of local variables and bound conditions 
                   to access array to avoid warnings and invalid memory access. 
06/19/14   sg      Made changes to revert the cipher config db if cipher config type is
                   updated to step1 but later message validation failed.
05/20/14   ad      Made changes to reject the security mode command if Network configures a integrity/ciphering 
                   configuration which UE hasnt indicated in its capabilities
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/14/14   sp      Fixed compiler warnings
02/13/14   ad      Made changes to remove redundant F3s
03/08/14   vg      Made changes to reduce the stack size usage of some RRC functions which has the usage more than 500 bytes.
02/03/14   as      Made changes to backup internal/external cmds for crash debugging
01/28/14   sp      Fixed KW errors.
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
11/07/13   as      Made changes to indicate security status ind as part of CU for Rel10 spe CR
10/30/13   sp      Fixed the bound checking related KW error
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/30/13   as      Made changes to condier successful DDT decode as implict L2 ACK for SMC complete
                   only if DDT integrity is passed with new security info
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/24/13   as      Made chagnes to send RRC_CIPHER_SYNC_IND to G whenever new keys are applied 
06/07/13   vi      Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
05/23/13   as      Made changes to consider the successful decode of DDT as implicit L2 ACK
                   for Security mode complete
05/10/13   sg      Made changes to fix the compilation errors
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
03/22/13   mp      Removed tmc.h include file
15/03/13   ad      Made changes to update the key indices for PS domain also when 
                   CS+PS SRVCC happens.
15/03/13   ad      Made changes to update the integrity keys for the domain id 
                   received in sim_update_req and not for both domains. 
                   Also changes to use old keys if new keys are pending to be 
                   applied for old cn domain. 
01/03/13   ad      Made changes to reduce the redundant F3s
12/13/12   sg      Made changes to initialize the LSB 12 bits of UL COUNTI
                   when SMC is received without new keys being received 
                   or new start value, which is not yet applied.
12/12/12   mp      Fixing KW errors
11/08/12   pm      Made changed to remove rrcasn1_ie.h from rrcdata.h to avoid SU API
                   access violations.
11/2/12   sg      Fixed KW errors
10/22/12   pm      Made changes to initialize the LSB 12 bits of COUNTI when SMC is received 
                   without new keys being received or new start value which is not yet applied
09/21/12   ad      Corrected featurization related to FEATURE_LTE_TO_WCDMA
09/07/12   db      Made the changes to detect the new keys based on the security_update_status flag for L2W
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/05/12   ad      Removed code which is initializing dl_latest_config_cn_id_old_1_valid
                   when its already in use
05/07/12   rl      Corrected to use dl_new_key_flag_per_rb and ul_new_key_flag_per_rb flag to use old keys only if 
                   new keys received have not been activated yet and activation time is not yet reached for the uplink and downlink message.
03/28/12   rl      Added the debug prints for keys used incase of integrity failure
03/14/12   ad      Made changes to suspend the radio bearers till L2 ack is 
                   received for security mode complete if only integrity is started
03/14/12   rl      Made changes to use the old keys only if new keys
                   received have not been activated yet and activation time 
                   is not yet reached in the uplink side too.
02/22/12   ad      Made changes to initialize the fresh_present variable after 
                   SRNS relocation is sucessfully completed
02/02/12   ad      Made changes to use the old keys only if new keys
                   received have not been activated yet and activation time 
                   is not yet reached.
01/16/12   rl      Made changes to saturate the start val at threshold
01/06/12   ad      Added code to handle SIM_UPDATE_REQ with
                   PS mapped keys after SRVCC handover
01/03/12   rl      Fixed the compilation warnings
12/13/11   sks     Added support for LTOW SRVCC.
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
11/20/11   db      Made changes to initiate cell selection when capability check failes
11/03/11   ad      Made changes to update the correct SN for rollover case for latest 
                   cn domain in DL Count I
10/27/11   mn      Removed the code to right shift the HFN by 5 bits for UM mode
09/28/11   ad      Added code changes for SMC to wait for till RRC comes back 
                   to active state if L2 ack for SMC is received when RRC is in 
                   STANDBY mode.
09/14/11   rl      W-RRC change to not reset START to Zero when NAS sends 
                   sim_update_req to W-RRC with cause RRC_SIM_UPDATE_VALUE_LTE_MAPPED 
                   & W-RRC is in connected mode.
08/26/11   rl      Fixed the KW error
08/26/11   ad      Made changes to initialise the nas_sync_ind flag
                   while closing the signalling connection
08/18/11   rl      Made changes to copy the DL ciph activation times sent in SMC only when
                   the rb id is present in the established rabs.
08/16/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.104.00
08/08/11   ad      Removed the changes for RRC HFN rollover changes depending on activation time
08/02/11   rl      Added changes to increment the num_rb inciph_rb_info in gtow HO case only
08/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.103.00
07/06/11   ad      Added changes to send SM failure with RRCSMC_INCOMPATIBLE_SIMULTANEOUS_RECONFIG
                   if OC is set and SMC cn domain is different from the
                   latest configured cn domain
07/08/11   gv      Fixed KW error
07/08/11   gv      Fixed KW errors
07/05/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.101.00
06/27/11   kp      Added code to track nas_sync_ind so that DDT messages acn be allowed 
                   if we have sent atleast one nas_sync_ind to that CN doamin.
05/03/11   rl      Fixed the lint error
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   as      Made changes to continue stopped RLC RB's during
                   srns relocation and moving to PCH state
04/25/11   rl      Fixed the KW error
04/18/11   kp      Added code to save mapped ck/ik for L2W PSHO no cipher case.
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/28/11   su      Made changes to set SRB2 suspend_offset based on 
                   rrc_set_srb2_act_time_nv item.
03/25/11   su      Made changes to initialize local_hfn_type
                   at the correct place. 
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/08/11   rl      Fixed the compilation warnings
01/31/11   ad      Fixed compilation warnings
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/17/11   ad      Made changes in RRC related to L2 SW Decoupling
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/06/10   ss      Added code to initialize correct cipher params in security recovery
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/02/10   su      Provided fix to process TFCC message while another 
                   Reconfiguration is on going.
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/19/10   ssg     Added changes for new WCDMA-L2 interface.
11/29/10   sks     Fixed bugs found during testing.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   su      Fixed Klocwork Warnings.
10/25/10   ss      Added code changes to support SRNS through URA Update message
09/27/10   ss      Fixes buffer overflow in Downlink activation time.
09/22/10   ss      Added code to use trans_id based on r3/later_than_r3 extensions in smc
                   for UE releases before R6
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/07/10   ss      Added code to get and apply latest start_value_to_transmit that still 
                   has not been udpated to security database.06/07/10   ss      Added code to reset SVTHHO after using in
08/24/10   ss      Added code to incorporate L2W Idle mode MAPPED security keys
09/08/10   ss      Added code changes to process R7 security mode command cipherring
                   and Integrity I.E's if NV is enabled for them.
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/07/10   ss      Added code to reset SVTHHO after using in
                   rrcsmc_extract_latest_start_list_per_cn_domain
05/21/10   ss      Added code to use trans_id based on r3/later_than_r3 extensions in smc
                   for UE releases before R6
05/20/10   ss      Fixed klockwork errors
05/07/10   ss      Fixed compiler warnings.
05/07/10   prk     Fixed compiler warnings.
04/26/10   ss      Added rrcsmc_is_rlc_re_est_for_srns_active for Fast Dormancy feature 
04/19/10   rmsd    Fixed compilation error
04/15/10   kp      Bug fix to pass correct doamin I.D for appending START value in START list for RBSC
04/06/10   ss      Added code to correct start value calculation for HHO step-2
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
03/03/10   kp      Bug fix to set new_key_for_start_value only if keys generated in WCDMA.
02/22/10   ss      Modified debug prints for Cipher Algo
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
01/27/10   ss      Fixed compilation errors.
01/22/10   ss      Made changes in Start-Value calculation for CU in DCH to FACH with RAB est
01/22/10   ss      Added code to take backup of MAX HFN for CS RB's before HHO Step-1 
                   and later use that backup to calculate START_VALUE_TO_TRANSMIT for CS domain.
01/11/10   ss      Bug fix to set START = 0 in IDT if new keys are received for that domain.
12/21/09   ss      Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
10/30/09   ss      Fixed lint and Klockwork errors.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
09/14/09   ss      Fixed lint warnings generated by Static Analysis tool
08/17/09   ss      Added code to update ciphering algorithm when SRNS relocation triggers change in ciphering algorithm.
08/17/09   ss      Added code to provide ciphering ON/OFF indication to UI 
                   under feature flag FEATURE_UMTS_VOICE_CIPHERING_IND.
08/17/09   ss      Bug Fix for correct structure decoding of RBRC message in SRNS Relocation.
05/29/09   kp      Added support for new SNOW3g security Algo & single cipher algo usage across 
                   access stratum layers.
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/21/09   kp      Added code to send SM failuer if SMC is received while UE is Transitioning 
                   from FACH to PCH or DCH to PCH
03/28/09   rm      CMI Featurisation
03/16/09   ss      Added support to use START value of 0x00 in IDT message if we have new key
                   generated for a cn doamin.
03/16/09   kp      Added code to handle dummy RRC_CIPHER_CONFIG_CNF which is sent by LLC 
                   if its not able to configure LL with cipher_config_req LLC in case of cell reselction. 
                   This new code will resume suspended RLC LC's.
03/05/09   kp      Added security related SRNS relocation support for R7 DCCH messages 
                   URA update cnf/UMI/RB reconfig based on ASN.1-7.9 version. 
03/04/09   kp      Added support for sending SM failuer if UE receives R7 SMC & rrc_nv_rel_indicator
                   is set to non R7 with protocol error cause as rrc_messageExtensionNotComprehended
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
12/01/08   ns      Made changes to send the correct cipher algo to RLC during RLC reconfigurations.
09/16/08   ns      Fixed issues to support the change in the enum type in Rel7.
                   The new type being uecomdef_wcdma_cipher_algo_e_type.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
09/15/08   ns      Made changes to calculate the activation time using the CFN of the
                   new cell, and thereby ensure that the activation time sent is at least 200
                   frames ahead of CFN. 
06/05/08   ns      Added code to store the maximum of Count C values of RBs to be released.
                   This max value will be used later to calculate START
05/26/08   ns      Added code to purge the SRBs before stopping them during SRNS
                   relocation
05/16/08   ns      Added code to stop DL AM/UM channels during SRNS relocation
04/21/08   ns      Fixed klockwork errors
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
02/25/08   ns      Added support for enabling the code under 
                   FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU only when GCF flag 
                   is enabled
12/21/07   da      Support for FEATURE_MBMS
09/11/07   kp      Added support to pass UL request type i.e.either suspend or stop 
                   in function rrcsmc_perform_stop_or_suspend_functionality since 
                   RLC needs type to perform either suspend action or to perform 
                   action related to stop.Also renamed function & local variable 
                   names to give appropriate meaning.
09/03/07   kp      Implementation of spec cr-2805. Its about inclussion of 
                   START value in CU.It states Include START value for each CN domain 
                   for which an entry exists in the variable ESTABLISHED_RABs or which 
                   is indicated in the variable "LATEST CONFIGURED CN DOMAIN".
                   This spe cr is correction on top of base spec cr-2541.Also Added
                   support for spec cr which say include START value for CS domain 
                   in cell update before security is enabled.
07/23/07   kp      Implementation of spec cr 2567-Handling of security key set 
                   generated in other RAT at G to W handover."At Inter-RAT handover 
                   to UTRAN, If ciphering was enabled in other RAT (ex.GSM).Then UE 
                   shall use the key set used in the other RAT for immediately starting 
                   ciphering on W for CS domain. The HANDOVER TO UTRAN COMPLETE will be 
                   ciphered with the key set used in the other RAT and  sent to N/W.
                   Intigrity protection will be started with the key set used in the other 
                   RAT after receiving first Seccurity mode command message from UTRAN(N/W)".    
                   Changes are under feature flag FEATURE SPEC CR 2567 GTOW SECURITY KEY HANDLING.
07/04/07   kp      Bug fix to take care of returning correct start vale in function 
                   rrcsmc_append_start_per_domain when integrity is not started for 
                   a CN domain.
05/23/07   kp      Added support to handle RB Release through RB Setup procedure.
                   This will happen with the support for "RB Info to Change" 
                   IE in RB-Setup msg.
04/09/07   kp      Added support for setting start_value_to_transmit_domain 
                   Correctly, when SRB4 is getting set up.
03/16/07   kp      Added support for setting START_VALUE_TO_TRANSMIT_FLAG to TRUE
                   for SRB set up only when LATEST_CONFIGURED_CN_DOMAIN has been 
                   initialized.This is as per spec section 8.6.4.1)Signalling RB 
                   information to setup list.
03/12/07   kp      Added support for enabling complete ciphering with UEA0 
                   Algo even when ciphering NV item is set to integrity Only.With 
                   these changes NV variable nv_ciphering_enabled will be enabled 
                   even when NV item is set to integrity Only, this was needed since
                   all the code  for ciphering is under nv_ciphering_enabled . New NV
                   variable nv_ciphering_enabled_status will keep track of exact 
                   status of Ciphering NV item.         
02/28/07   vr      Bug fix in implementation of Spec CR 2284R1. Do not increment
                   HFN by 1 at Count-C activation time for a FACH->DCH transition
                   using RBSetup which establishes TM bearers.
02/09/07   kp      Removed extra hop of command interface between SMC & LLC, 
                   for resuming uplink RB’s & updating uplink ciphering keys. 
                   Now SMC directly places these request to L2 task & L1 respectively 
                   bypassing LLC.
01/11/07   sm      Fixed 7200 compiler warning which expects a new line
                   at the end of file.
01/12/07   sm/vr   Implementation of Spec CR 2284R1 and added support for HHO with
                   AMR rb-setup message 
01/11/07   kp      Fixed 7200 compiler warning which expects a new line
                   at the end of file.
01/04/07   kp      Added support for differing update of UL Ciphering keys to L1
                   till L2 Ack for SMC complete is received.
01/04/07   kp      Added code to clear ciphering data base related to TM 
                   only if TM RB’s are really released while processing RB Release message
11/23/06   kp      SMC suspend request & confirmation will be through call back 
                   function with RLC Adding the required support & also 
                   cleaning up code for command interface. 
09/27/06   sgk     Fixed 7200 compiler warnings. 
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
09/11/06   vr      Added support for RLC re-establishment due to RLC Size change
09/11/06   kp      Added support to fill start list per active cn-domain which
                   will be used only by cell update procedure.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/21/06   tkk     Added SRB#5 support.
07/28/06   sm      Fixed the HFN initialization for SRB2 whenever pending
                   ciphering configuration exist for SRB2 during SRNS relocation.
07/27/06   sm      Move RRC-SN for SRB0 to one less than activation RRC-SN
                   on receipt of L2-Ack of SMComplete.
06/05/06   vr      Implementation of SRB2 suspension algorithm. SRB2 will 
                   now be suspended during SMC to avoid UE sending PDUs in
                   new ciphering configuration before network has 
                   successfully received the (re)transmitted PDUs of 
                   SecurityModeComplete. Provision has been made to allow 
                   transmission of a few PDUs in old configuration in order 
                   to avoid holding back measurement reports that might be 
                   critical for active set maintenance in bad RF conditions.  
                   Changes under FEATURE SMC SRB2 SUSPENSION
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
05/18/06   sm      Removed nv_gcf_test_flag from smc which was used to enable security
                   roll-back feature because this feature is risk-free to enable in all builds.
                   Changes to make sure that there is only one entry for each
                   configured rb's as well as srb's in "ciphering mode info"
05/08/06   vr      Fixed lint warnings
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
04/18/06   dvp     CR91152: Update rx. cn domain soon after SMC Integrity is
                   verified.
04/03/06   sm      Changes to reset HFN to 0 for new-keys whenever SMC-modify is 
                   received rather than resetting HFN when SIM_UPDATE_REQ is received
		        		   from NAS.
03/24/06   vr      Bug fix in FEATURE_CIPHERING_SUPPORT_FOR_CR2272
                   Fixed temporary voice garbling issues for the foll sceraios:
                   1. GtoW Handover with Ciphering turned ON
                   2. Ciphering config change due to SMC when TM bearers are 
                   already established and ciphered.
03/24/06   vr      Lint Fixes
03/15/06   vr      Before appending the HFN values to RRC_REL_IND,check to see
                   if there are new keys that are yet to be applied.If so,set the 
                   HFN to zero and append it to RRC_REL_IND.This is a spec
                   requirement.
03/13/06   vr      If CipheringModeInfo is included in SMC,check to see if 
                   all the SRBs are included in the IE.If not,declare the SMC to 
                   be invalid
03/10/06   sm      Added function rrcsmc_revert_back_to_old_fresh_for_srns_relocation() to
                   revert to old FRESH(in case of SRNS relocation scenario) when UE is
                   supposed to revert back to old configuration due to physical-Ch-Est-Failure.
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/23/06   sm      Added support for spec compliant processing of non-integrity
                   protected DDT message.                   
02/21/06   vr      Removed setting the flag NEW_TM_RB_IND_FLAG during Ciph
                   Config Request.This flag should be set only if ciphering
                   for CS domain is active and there are TM bearers getting 
                   established for the first time.This should not be done 
                   if TM bearers are already existing but getting ciphered for 
                   the first time.While the former is used and reset by the RBSetup 
                   procedure to include the Count-C Activation time in 
                   RBSetupCompelte,the latter is not used at all.The flag 
                   remains set and has other repurcussions later in the call. 
01/25/06   bd      Lint clean-up
11/30/05   vr      Fixed a bug in validating cipher contents in SMC.Using a local
                   pointer now to parse the list instead of using the higher level
                   structure
11/10/05   sm      Added changes to enable switching between Rel99 and Rel5 using nv_item
                   Fixed a lint error.
10/26/05   vr      Check to see if established PS Radio bearers are present 
                   in the Security Mode Command for PS domain.If not,send a 
                   Security Mode Failure message to the network.
                   Also set the nv_integrity and nv_ciphering variables
                   to 1 if there is an error reading the nv items.
10/20/05   vr      Removed dependency in the "act_incl" flag before releasing the 
                   TM bearers from cipher db.This check was present when CU was 
                   releasing the CS RAB due to T314 expiry after RL failure.This flag
                   is never set by CU/LLC procedure,hence SMC never deletes the TM bearers
                   from cipher db.This check is not needed because SMC is always 
                   going to act immediatley if the action is RB Release.
10/07/05   sm      Changes to handle third integrity configuration in DL
09/26/05   ss      added support for paramter mac-d-hfn in rrcsmc_process_cipher_config_for_hho
09/22/05   kc      Added support for handling SMC ciphering config when a ciph
                   config activation time is already pending.
09/12/05   tkk     Added support to IP check for Rel-5 UMI & URA update confirm.
08/17/05   vr      Spec5 CR 2149:While specifying the UL IP activation SN of RB0
                   in SecurityModeComplete,set the value to Curr_Seq_Num+N302+2 
                   instead of Curr_Seq_Num+N302+1.  
08/16/05   sm      Added support to check whether DL-configuration message contains
                   IE "integrityProtectionModeInfo" in later-than-r3 branch of DL_DCCH_Message
                   to determine SRNS relocation scenario with Rel-5 signalling message.
08/04/05   vr      Fixed a logical bug(looping condition)while starting ciphering
                   for new UM Data RBs.Also Cleaned up some redundant code.
06/21/05   vr      During a RBRelease,clear the TM RB info only if it has not been cleared
                   earlier.There is a possibility that the TM info would have been cleared
                   by a SignallingConnectionReleaseInd.Clearing it twice would cause havoc 
                   and possible Garbling.
06/09/05   sm/vr   1.Fixed corner scenario for feature FEATURE_ROLLBACK_SECURITY_CONFIGURATION.
                   When LLC is in LLC_SUSPENDED and SMC is in WAIT_FOR_CIPH_CONFIG_CNF state,
                   special care needs to be taken while reverting back to old security variables
                   because security variables are being updated when LLC processes
                   RRC_CIPH_CONFIG_REQ from LLC_SUSPENDED sub-state.
                   2.Do not suspend PS RAB when processing a SMC for CS domain
05/05/05   vr      Fixed lint error
05/03/05   sm      Added functions/changes to support roll-back to old security 
                   configuration when cell update is initiated during SMC procedure.
04/28/05   vr      If a NAS message is not integrity protected after security
                   has started,do not perform an Integrity check on that message.
                   Let NAS decide whether it needs to ignore it or not.This is done as an 
                   optimization to counter a race condition at the network side 
                   where Authentication and SMC happens parallely for 2 domains in
                   a MultiRAB call.
04/20/05   sm      Added variable 'backup_of_security_variables' to take 
                   back up of global security variables and added functions 
                   'rrcsmc_revert_to_old_cipher_config_db()' &
                   'rrcsmc_backup_cipher_config_db()'
04/07/05   vr      If we get an SMC in a state other than initial,send a Security
                   Mode Failure to the network.
                   Removed an unused function rrcsmc_process_disconnected_state()
                   Removed a redundant check(ASN1 decoding error) while handling SMC
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
04/06/05   vr      Send Sync_ind to MM if nv_integrity_enabled is false and 
                   SMC wihtout an IP info check
04/01/05   vr      Fix to notify MM about the integrity and ciphering status after SMC
03/18/05   vr      Do not reset ciph config status on RBRelease 
03/07/05   sm      Added check to increment RRC-HFN if activation SN > current RRC-SN
                   and rollover has happened because of RRC-SN jumping 
02/18/05   kc      Fix to clear up TM-Ciphering-Config while going to cell-fach due to
                   and also to when TM-Channels are released during CU-Cnf
02/17/05   sm      Added support to validate DL-PDU during SRNS relocation by decoding 
                   PDU before doing integrity check of config message during SRNS 
                   relocation scenario.
02/02/05   kc      Fix for voice-garbling issue during a G->W HO.
12/24/04   sm      Added support for config_type SMC_INITIATED_NON_HO_CIPHER to support
                   processing of IE cipher_mode_info in SMC when TM RBs are present.
12/14/04   kc      Fix to not cast byte-ptrs to uint32 ptrs.  Causing data-abort due to
                   unaligned memory access
12/10/04   sm      Fix for NOT incrementing DL-HFN by 1 when wrap-around of SN coincides
                   with security-reconfiguration activation for all SRBs and activation 
                   time is not 0.
10/19/04   kc      Fix for not incrementing HFN by 1 when wrap-around of SN coincides
                   with security-reconfiguration activation for RB0.
09/13/04   sgk     Assigned the right enum values to start_list->ciph_algo_srb
                   and start_list->ciph_algo_urb in function
                   rrcsmc_get_latest_transmitted_start_values in function
                   rrcsmc_get_latest_transmitted_start_values, assigned the
                   right enum values for cn domain id in function
                   rrcsmc_update_ciphering_config_info_for_reconfig, initialized
                   local_domain_id_list[i] to RRC_CS_DOMAIN_CN_ID instead of 0
                   in function rrcsmc_update_ciphering_config_info_for_reconfig,
                   assigned local_domain_id, domain_id to the right enum values
                   in function rrcsmc_update_ciphering_config_info, assigned
                   cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_algo
                   to the right cipering enum value, in function
                   rrcsmc_process_cipher_config_for_hho to fix lint error Type
                   mismatch (assignment) (enum/enum).
                   Typecast local_lc_id to uint8 in function 
                   rrcsmc_get_max_start_per_domain to fixl lint error
                   Loss of precision (arg. no. 1) (32 bits to 8 bits).
                   Included seq.h to fix lint error: Symbol 'seq_get_cfn'
                   undeclared, assumed to return int, call to function
                   'seq_get_cfn()' not made in the presence of a prototype.
                   Typecast srns_info->ciphering_mode_info->activationTimeForDPCH
                   to uint8 in function rrcsmc_validate_and_save_srns_info to
                   fix lint error Error (Info) 734: Loss of precision (assignment)
                   (32 bits to 8 bits).
                   Typecast return of rrclcm_get_dl_rb_id to uint16 in function
                   rrcsmc_check_integrity_for_srns_relocation to fox lint error 
                   of precision (assignment) (32 bits to 16 bits).
                   Typecast RRC_CS_DOMAIN_CN_ID to uint32 in function 
                   rrcsmc_append_start_list, Typecast RRC_PROCEDURE_SMC,
                   RRCLOG_PROTOCOL_ERROR, RRCLOG_ASN1_ENC_ERR to uint 8 in function 
                   rrcsmc_process_smc_initial_substate to fix lint error
                   Converting enum to int.                   
08/09/04   kc      Provided interface to check if TM ciphering is active.
07/26/04   kc      Fix to bypass IP-Check for BCCH-FACH messages.
07/09/04   kc      Set correct error cause in SMC-Failure message.
06/28/04   kc      Fix for GCF test case to check if RRC-SN received is > by half
                   the modulus value of SN to determine if act time is reached.
06/01/04   kc      API to indicate whether SMC is active and in the middle of
                   a security reconfiguration.
05/27/04   kc      Fix for using correct integrity key when there are 2 pending keys.
05/25/05   kc      Merged code to commit key-idx when new keys are received and no
                   SMC is received for this domain before a call release.
05/15/04   kc      Fix to not increment START between Conn-Setup and IDT.  Also
                   added fix to use correct START_VALUE_TO_TRANSMIT if CS and PS
                   calls are setup simultaneously.
04/20/04   kc      Resume RB's other than SRB-2 only after L2_ACK for SMC-Complete
                   is received.  This is to fix a race-condition seen when meas
                   reports reached RNC before ciphering configuration was completed
                   on UTRAN-side.
03/18/04   kc      Fix to use COUNT_I correctly for RB_0 when roll-over happens
                   and and the activation_time is not 0 after the roll-over.
03/05/04   kc      Corrected calculation of START values at call release after
                   authentication.
02/29/04   kc      Clearing Ciphering database correctly for back to back CS
                   voice calls without connection release.
02/25/04   kc      Do not update ciphering_info for RB_0 since CCCH is not
                   ciphered.  Also initialized cn_domain_list to default.
02/18/04   kc      Added code to correctly use last received FRESH value for SRNS 
                   relocation.  Added code to check and see if IK/CK was used 
                   during an RRC-conn and only then update START values.
02/10/04   kc      Added check to see if L2_ACK for Open_Session_Req is rcvd. or
                   not and to send OPEN_SESSION_CNF if not yet received by DT proc.
02/06/04   kc      Added feature_defn's around HO related fn. calls to fix
                   single mode build issues.
01/26/04   kc      Fix to send MAC Ciphering configuration only if a TM bearer
                   config has changed.
12/15/03   kc      Fix for CR38373.  Handling sending of ciphering parameters to
                   RLC/MAC when NV item for ciphering is turned on, but UTRAN sends
                   UEA_0 for ciphering algorithm.
12/12/03   kc      Fixed going to Initial_State in all SMC substates.  Also fixed
                   sending correct ciphering algorithm to RLC during a Multi-Rab
                   Hard Handover scenario.  Removed processing of New_Cell_Ind in
                   SMC as it will never get routed to SMC directly.
11/11/03   kc      Added Security support for SRNS relocation.
10/21/03   kc      Added code to query WM Queue for num of PDU's in queue and
                   then calculate RLC SN for RB2 ciphering activation.
10/14/03   kc      Added support for GtoW HO cipher related bugs found during
                   E IOT.  Also support for SPEC CR 2001.
10/07/03   kc      Added cause to RRC_CIPH_CONFIG_REQ to handle regular ciphering
                   and GtoW Step_1 Ciphering differently in LLC.
08/20/03   kc      Added code to peek into DL msgs to determine if IPModeInfo is 
                   present in non-SMC messages.
08/20/03   kc      Fix for CR34206.  DL IP check failure when SMC-Modify with
                   rollover happens.
08/11/03   kc      Added interface to send latest_transmitted_start_val to RLC
                   to support RLC-Re-establishment.
08/05/03   kc      Added fix to send SYNC_IND to MM when both Integrity/Ciphering
                   NV items are not turned on.
08/01/03   kc      Fix for CR33440, to handle race condition when L2 ACK for
                   IDT comes after SMC Complete is sent
07/21/03   kc      Fix for CR30817, to commit ciphering info to RLC/MAC/L1 before
                   receiving L2_ACK for security mode command
07/16/03   kc      Fix for CR32146.  Integrity fix for Multi-Rab issue to commit
                   START values only when SMC command is received.
07/11/03   kc      Fix for CR32422.  Clearing TM RB Ciphering database correctly
                   when Sig Conn. rel is received.  Also updating ciphering db
                   only for domain for which ciphering is configured.
07/07/03   kc      Fix for CR2560, to update integrity_protection_info for uplink
                   to latest START value sent in Initial Direct Transfer.
07/03/03   kc      Fix to use domain_id of latest SMC command when Security
                   Mode Command with modify is received.
06/27/03   kc      Fix for CR32146 and for Ciphering voice garbling during
                   emergency calls.
06/19/03   kc      Fix for Data_Abort.  Also changed RLC Ciphering activation
                   time to not add +1 as RLC provides vt(s) which is already + 1.
06/18/03   kc      Initialized CN_DOMAIN before using it in append_start_list.
06/13/03   kc      Changed RLC Ciph Activation times for RB_2 to +4.
06/04/03   kc      Added code to update START value when START values are
                   synchronized between UE-UTRAN during an Initial Direct Transfer.
05/30/03   rj      Fixed lint errors and modified interface to function
                   rrcueci_verify_inter_rat_security_capability.
05/28/03   kc      Added GTOW feature defn. around Inter-RAT Ciphering function 
                   to resolve single mode compilation issues.
04/30/03   kc      Added code for GtoW IRAT ciphering configuration.  Functions
                   to handle STEP_1 and STEP_2 Ciph Config states are added.
04/21/03   kc      Fix for CR29737, CR29536 uncovered during IOT.  Also added
                   is an interface to provide mac with an interface to query
                   start_value_to_transmit.
04/17/03   rj      Updated function rrcsmc_validate_security_mode_command 
                   to check presence of Cipheirng Mode Info IE and Integrity
                   Protection Mode Info IE only if NV items for ciphering
                   and Integrity protection are enabled.
04/11/03   kc      Fixed CR's 29530, 29531 uncovered during IOT.
                   Also implemented API for START value/domain to fix CR1282
04/02/03   kc      Fixed CR29309 to call UECI API's to verfiy UE Security
                   capabilities instead of checking against hard-coded
                   values in SMC procedure.
03/19/03   kc      Fixed CR28589 to remove incorrect check for SMC message
                   to Reject Security Modify.
03/04/03   kc      Fix for CR28219.  Added NV_CIPHERING_ENABLE flag check in
                   before sending ciphering configuration to lower layers if
                   ciphering is not enabled through NV RAM.
02/25/03   kc      Fix for CR27934.  Correct implementation of domain id's when
                   activation time was reached.
02/03/03   kc      Removed lint errors
02/02/03   kc      Corrected initialization of L1 ciphering keys. Also set the
                   default Activation time to current SQN +4 rather than +2.
01/24/03   kc      Removed START value initialization to 0's after every call. 
                   Only at Power-Up START value is init to 0 as UE maintains
                   the START value till Power-Down.
12/15/02   rj      Added  code to initialize the CN domain Ids and the 
                   respective HFN values.
12/06/02   kc      Fix for CR26212.  Added NV checks to not configure RLC/MAC
                   with ciphering config when NV_CIPHERING_ENABLED is off
12/06/02   kc      Removed outdated check for Integrity with Modify
11/15/02   kc      Fixed bugs found when SRB configuration is changed during
                   RB Setup or Reconfiguration.
11/01/02   kc      Fixed copy right info and made minor initialization changes
                   to improve code readability.
10/31/02   kc      Fixed resetting TM RB related flags when ciphering is
                   turned off.
10/29/02   kc      Fixed DPCH activation time validity check.
10/28/02   rj      Externalized NV items for Integrity Protection and 
                   Ciphering.
10/25/02   kc      Added more ciphering related changes to fix bugs found
                   during start_value_to_transmit calculation.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
10/09/02   kc      Added more ciphering related changes to fix bugs found
                   during PS and CS data integration.
09/30/02   kc      Added more interfaces to support PS data and generic 
                   interfaces between SMC and Reconfig procedures for 
                   ciphering when SRNS relocation is supported.
09/27/02   kc      Added code to fix CR24884  Also added additional ciphering
                   code fixes for bugs found during integration.
09/25/03   bu      Call rcllc_get_ordered_config_status() instead of
                   rrcllc_is_ordered_config_set().
09/12/02   kc      Added FEATURE defn. for Ciphering in additional parts of
                   code to not support ciphering IE's when feature is not
                   defined.
09/04/02   kc      Added code to support ciphering.
08/27/02   kc      Fix for CR24192. Removed the FEATURE defn. for Integrity 
                   Disabling as it is now controlled through NV RAM
08/26/02   kc      Mainlined CR23893.  Fixed CR24176.
08/08/02   kc      Added fn. to support SIM_INSERT_REQ and SIM_UPDATE_REQ
08/02/02   kc      Added fn. to read NV items INTEGRITY_ENABLED and 
                   CIPHERING_ENABLED.
06/17/02   kc      Added code to support Integrity Modify in Security Mode 
                   Command.
05/17/02   kc      Fixed more bugs discovered during regression testing.
05/16/02   kc      Added check to see if mem_malloc was successful
05/16/02   kc      Added more MSG_HIGH's to get additional debug info.
04/23/02   kc      Fixed more bugs discovered during adversarial testing.
04/22/02   kc      Fixed an ambigious spec. interpretation to Ericsson's
                   interpretation of the spec.
04/22/02   kc      Fixed bugs discovered during Integration testing.
04/17/02   kc      Fixed CN_DOMAIN initialization problems when INTEGRITY
                   FEATURE is disabled.
04/02/02   kc      Base build for Integrity Support.  Added lot of code to
                   support Integrity.
02/28/02   kc      Updated code to send a security mode command failure if an
                   invalid message is received. Added wait for l2_ack state.
12/09/01   rj      Updated code to be compatible for June release of 25.331 
                   RRC Protocol Specification.
10/23/01   kc      Initialized uninitialized vars to remove compile warnings
10/17/01   kc      Initial Check In for RRC SMC file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "uecomdef.h"
#include "comdef.h"
#include "rrccmd_v.h"              
#include "rrcsmc.h"
#include "msg.h"
#include "err.h"
#include "bit.h"
#include "rrcasn1.h"
#include "rrclcm.h"
#include "rrcdata_v.h"
#include "rrcsend.h"
#include "rrcf9driver.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrcscmgr.h"
#include "rrcnv.h"
#include "rrcrbcommon.h"
#include "rrccu.h"
#include "rrcrbe.h"
#include "rrcueci.h"
#include "rrcllcoc.h"
#include "rrcdt.h"
#include "seq.h"
#include "rrcasn1.h"
#include "rrcasn1util.h"
#include "rrclogging.h"
#include "macrrcif_v.h"
#include "rlcrrcif_v.h"
#include "rrcmcm.h"
#include "rrcllc.h"
#include "l2dltask.h"
#include "l2ultask.h"
#include "rrcmisc.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/*===================================================================
                            MACROS AND FUNCTIONS
=====================================================================*/

/*Macro used to indicate for printing integrity key*/
#define  RRC_PRINT_INTEGRITY_KEY    0

/*Macro used to indicate for printing ciphering key*/
#define  RRC_PRINT_CIPHERING_KEY    1

/*Macro indicates the max start val used to saturate at threshold*/
#define START_THRESHOLD 0x000FFFFF

/*===================================================================
MACRO       :   PRINT_INT_KEY 

DESCRIPTION :   Prints integrity key

DEPENDENCIES:   None

RETURN VALUE:   None.

SIDE EFFECTS:   None.

=====================================================================*/
#ifdef  __GNUC__
#define PRINT_INT_KEY(key_info)\
{\
  uint32 cnt=0;/*local variable use for counting in for loop*/\
  uint32 temp_key[4];/*local var used for printing keys*/\
  /* Print INTEGRITY KEY */ \
  for (cnt=0; cnt < (RRC_MAX_INTEGRITY_KEY_LENGTH /4); cnt++)\
  {\
    temp_key[cnt] = b_unpackd( &(key_info.integrity_key[cnt*4] ), 0, 32 );\
  }\
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"INT KEY 1: 0x%x 2: 0x%x 3: 0x%x 4: 0x%x", temp_key[0],temp_key[1],temp_key[2],temp_key[3]);\
}
#else /*__GNUC__*/
#define PRINT_INT_KEY(key_info)\
{\
  uint32 cnt=0;/*local variable use for counting in for loop*/\
  uint32 temp_key[4];/*local var used for printing keys*/\
  /* Print INTEGRITY KEY */ \
  for (cnt=0; cnt < (RRC_MAX_INTEGRITY_KEY_LENGTH /4); cnt++)\
  {\
    temp_key[cnt] = b_unpackd( &(key_info##.integrity_key[cnt*4] ), 0, 32 );\
  }\
    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"INT KEY 1: 0x%x 2: 0x%x 3: 0x%x 4: 0x%x", temp_key[0],temp_key[1],temp_key[2],temp_key[3]);\
}
#endif

/*===================================================================
MACRO       :   PRINT_CIPHER_INT_KEY 

DESCRIPTION :   Prints integrity or ciphering key

DEPENDENCIES:   None

RETURN VALUE:   None.

SIDE EFFECTS:   None.

=====================================================================*/
#ifdef  __GNUC__
#define     PRINT_CIPHER_INT_KEY(key_type,key_info)\
\
  {\
    uint32 cnt=0;/*local variable use for counting in for loop*/\
    uint32 temp_key[4];/*local var used for printing keys*/\
\
    /* Switch on key type for printing key */ \
    switch (key_type){ \
\
        /* Print INTEGRITY KEY */ \
        case RRC_PRINT_INTEGRITY_KEY:\
\
          for (cnt=0; cnt < (RRC_MAX_INTEGRITY_KEY_LENGTH /4); cnt++)\
          {\
            temp_key[cnt] = b_unpackd( &(key_info.integrity_key[cnt*4] ), 0, 32 );\
          }\
          MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"INT KEY 1: 0x%x 2: 0x%x 3: 0x%x 4: 0x%x", temp_key[0],temp_key[1],temp_key[2],temp_key[3]);\
          break;\
\
        /*  Print CIPHERING KEY */\
        case RRC_PRINT_CIPHERING_KEY:\
\
          for (cnt=0; cnt < (RRC_MAX_CIPHERING_KEY_LENGTH /4); cnt++)\
          {\
            temp_key[cnt] = b_unpackd( &(key_info.ciphering_key[cnt*4] ), 0, 32 );\
          }\
            MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"CIPHER KEY 1: 0x%x 2: 0x%x 3: 0x%x 4: 0x%x", temp_key[0],temp_key[1],temp_key[2],temp_key[3]);\
\
            break;\
\
        /* Invalid Key type  */ \
        default: \
\
            /* Log an error message */ \
            WRRC_MSG1_ERROR("Invalid TTI key type %d ",key_type); \
\
            break; \
    }\
  }
#else /*__GNUC__*/
#define     PRINT_CIPHER_INT_KEY(key_type,key_info)\
\
  {\
    uint32 cnt=0;/*local variable use for counting in for loop*/\
    uint32 temp_key[4];/*local var used for printing keys*/\
\
    /* Switch on key type for printing key */ \
    switch (key_type){ \
\
        /* Print INTEGRITY KEY */ \
        case RRC_PRINT_INTEGRITY_KEY:\
\
          for (cnt=0; cnt < (RRC_MAX_INTEGRITY_KEY_LENGTH /4); cnt++)\
          {\
            temp_key[cnt] = b_unpackd( &(key_info##.integrity_key[cnt*4] ), 0, 32 );\
          }\
          MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"INT KEY 1: 0x%x 2: 0x%x 3: 0x%x 4: 0x%x", temp_key[0],temp_key[1],temp_key[2],temp_key[3]);\
\
          break;\
\
        /*  Print CIPHERING KEY */\
        case RRC_PRINT_CIPHERING_KEY:\
\
          for (cnt=0; cnt < (RRC_MAX_CIPHERING_KEY_LENGTH /4); cnt++)\
          {\
            temp_key[cnt] = b_unpackd( &(key_info##.ciphering_key[cnt*4] ), 0, 32 );\
          }\
          MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"CIPHER KEY 1: 0x%x 2: 0x%x 3: 0x%x 4: 0x%x", temp_key[0],temp_key[1],temp_key[2],temp_key[3]);\
\
            break;\
\
        /* Invalid Key type  */ \
        default: \
\
            /* Log an error message */ \
            WRRC_MSG1_ERROR("Invalid TTI key type %d ",key_type); \
\
            break; \
    }\
  }
#endif /*__GNUC__*/
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* boolean flags to read NV items and check to see if Integrity 
   and Ciphering are enabled from QPST
*/
boolean    nv_integrity_enabled;     
/*nv_ciphering_enabled should be always set to TRUE 
  since we have to support Ciphering with UEA0 even if 
  the NV ciphering is set to FALSE through QPST.As 
  most of the code for ciphering is under this flag*/
boolean    nv_ciphering_enabled;     
/*nv_ciphering_enabled_status gives the exact
  Status of ciphering enabled through QPST.
  This NV item is used, when we have to look for 
  Security Capability's i.e.
  if nv_ciphering_enabled_status = TRUE, then UE supports both UEA0/UEA1
  if nv_ciphering_enabled_status = FALSE, then UE supports only UEA0*/
boolean    nv_ciphering_enabled_status;

extern boolean    nv_fake_security_enabled;    

#ifdef FEATURE_QSH_EVENT_METRIC
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern rrc_qsh_metric_db_type rrc_qsh_metric_db[WRRC_QSH_METRIC_MAX+1];
extern boolean rrc_qsh_dl_committed;
#endif
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
 * Global Data Declarations
 * ----------------------------------------------------------------------- */

boolean feature_rollback_security_config = TRUE;

rrcsmc_ciphering_config_info_type cipher_config_db;

extern boolean nv_gcf_test_flag;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
/*maintain state of SMC procedure*/
LOCAL rrcsmc_substate_e_type  rrcsmc_substate;  

/*maintain status of failure cause*/
LOCAL rrcsmc_config_status_e_type rrcsmc_failure_type;
LOCAL rrc_ProtocolErrorCause rrcsmc_prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;

/* file global var contains info about suspend status and info of rlc rb's*/
LOCAL rrcsmc_suspend_stop_rlc_type smc_rb_suspend_or_stop_info;

/*Temporary variable to store the max count c values*/
LOCAL rrcsmc_rel_rbs_max_count_c max_count_c;

/*struct to save Security Mode Command message till we have received
  Suspend_Cnf from LLC*/
LOCAL rrcsmc_security_mode_command_type saved_security_mode_command_msg;

/*struct to save SRNS Security Info till we have received
  Suspend_Cnf from LLC*/
LOCAL rrcsmc_srns_info saved_srns_info;

/*declare the struct integrity_protection_info*/
LOCAL rrcsmc_integrity_protection_info_type integrity_protection_info;

/*declare the struct integrity_protection_info*/
LOCAL rrcsmc_ciph_info_type ciphering_info;

/*static var to hold all kinds of security config which is not calculated
  by SMC procedure,which is essentially received from other procedures or
  OTA message */
LOCAL rrcsmc_security_config_info_type  rrcsmc_int_ciph_info;

/* contains info about ciphering status/radio bearer*/
LOCAL rrcsmc_rb_ciphering_config ciph_rb_info;

/* contains the ciphering activation time passed to SMC procedure by other
   procedures which are setting up TM radio bearers*/
LOCAL uint8 tm_ciph_act_cfn;

/*boolean to check is START_VALUE_TO_TRANSMIT is set or not*/
boolean START_VALUE_TO_TRANSMIT_FLAG;

/*uint32 value to hold START*/
uint32 start_value_to_transmit;

/*uint32 value to hold START_CS for HHO */
uint32 start_value_to_transmit_cs_hho;

/*Value to store start_value_to_transmit for CR1466 implementation*/
uint32 mac_start_value_to_transmit;

/*new tm radio bearer indication flag*/
boolean NEW_TM_RB_IND_FLAG;      

/*domain id of latest updated start_value_to_transmit */
rrc_cn_domain_identity_e_type start_value_to_transmit_domain;

boolean l2_ack_rcvd_for_smc_complete_flag=FALSE;
boolean rrcsmc_send_l2_ack_ind_to_ddt_flag=FALSE;
boolean extract_cause_rel_ind=FALSE;

/* This variable is supposed to be used to take back-up of all security related
   global variables. This will be used to revert back to old configuration when
   cell update is triggered during secruity reconfiguration or during physical
   channel establishment failure */
rrcsmc_security_variables_backup backup_of_security_variables;

/* This variable is used to determine whether RRC needs to revert back
  ciphering related variables or not. If it is set to FALSE then it indicates 
  that SMC dont have to roll back ciphering variables. If it is set to
  TRUE then it indicates that ONLY ciphering related global smc variables needs
  to be reverted back because integrity as well as structures common to
  ciphering and integrity are already rolled back
*/
boolean is_revert_to_old_cipher_variable_reqd = FALSE;

/* This variable is used to store security key set generated in other RAT
  for CS domain .This is done to support spec cr-2567, which say security
  key set to be used at Inter RAT handover to UTRAN will be the one which
  other RAT is being currently using i.e RR activated ciphering key Kc using 
  ciphering mode command message, corresponding UMTS security key set 
  is considered as key set used in other RAT. So to support this save the 
  security key set generated in G-RAT in this variable, Once RR receives 
  ciphering mod command message in G it will send a command to RRC, 
  then RRCSMC will move these saved security variables to its security database.*/
rrcsmc_security_keyset_generated_in_grat_type saved_security_key_set;
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
/*A variable used to track support of snow3g_security_algo support, by default set to FALSE*/
extern boolean    snow3g_security_algo_supported;
#endif


/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */

#ifdef FEATURE_WCDMA_REL9


/*===========================================================================
FUNCTION rrcsmc_get_srns_info_from_dl_sdu_ind_r9

DESCRIPTION
  This function extracts ip check info/integrity protection mode info/ciphering mode info from SRNS 
  relocation triggering messages for Release 9 version of OTAs. In summary, input srns_info has to be filled
  from OTA in cmd_ptr.

  NOTE:  There is no R9 UTRANMobilityInformation and URAUpdateConfirm messages in ASN 9.5.
  Right now, only 5 Reconfig OTAs + CU CNF can do SRNS relocation for R9 OTA.

DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if extraction was successful
  The function returns FAILURE if extraction was unsucceful.

SIDE EFFECTS
  None
===========================================================================*/
uecomdef_status_e_type rrcsmc_get_srns_info_from_dl_sdu_ind_r9
(
  rrcsmc_srns_relocation_security_info_r7_type * srns_info,
  rrc_cmd_type *cmd_ptr
)
{
  uecomdef_status_e_type status = SUCCESS;           /*By default, return status is SUCCEES*/
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;       /* Pointer to Downlink DCCH */
  uint8 msg_type;                                                    /*Local variable to store message type */             
  
  /* Get the decoded OTA message pointer */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /*By default, set integrity and ciphering info status as FALSE in srns_info*/
  srns_info->integrity_info_present = FALSE;
  srns_info->ciphering_mode_info_present = FALSE;          

  /*SRNS-Reloc is posssible through DCCH messages only, so check for DL SDU type.*/
  switch (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
  { 
    case rrc_DL_DCCH_Message_PDU :

      /*Extract actual OTA type from cmd_ptr*/
      msg_type = rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
          cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

      switch (msg_type)
      {
        case T_rrc_DL_DCCH_MessageType_cellUpdateConfirm:
        {
          rrc_CellUpdateConfirm_r9_IEs *cucnf_ptr = 
            RRC_EXTRACT_R9_MSG_POINTER(dcch_msg_ptr,cellUpdateConfirm,cellUpdateConfirm_r9);
      
			  if(RRCRB_R9_MSG_IE_PRESENT(cucnf_ptr,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(cucnf_ptr->integrityProtectionModeInfo);
          }
			  if(RRCRB_R9_MSG_IE_PRESENT(cucnf_ptr,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(cucnf_ptr->cipheringModeInfo);
          }
        }
        break;

        case T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration:
        {
          rrc_RadioBearerReconfiguration_r9_IEs *rbrc_ptr = 
            RRC_EXTRACT_R9_MSG_POINTER(dcch_msg_ptr,radioBearerReconfiguration,radioBearerReconfiguration_r9);

			  if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rbrc_ptr->integrityProtectionModeInfo);
          }
			  if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(rbrc_ptr->cipheringModeInfo);
          }
        }
        break;
 
        case T_rrc_DL_DCCH_MessageType_radioBearerRelease:
        {
          rrc_RadioBearerRelease_r9_IEs* rb_release_ptr = 
            RRC_EXTRACT_R9_MSG_POINTER(dcch_msg_ptr,radioBearerRelease,radioBearerRelease_r9);
 
			  if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rb_release_ptr->integrityProtectionModeInfo);
          }
			  if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info =  &(rb_release_ptr->cipheringModeInfo);
          }
        }
        break;

        case T_rrc_DL_DCCH_MessageType_radioBearerSetup:
        {
          rrc_RadioBearerSetup_r9_IEs* rb_setup_ptr = 
            RRC_EXTRACT_R9_MSG_POINTER(dcch_msg_ptr,radioBearerSetup,radioBearerSetup_r9);

			  if(RRCRB_R9_MSG_IE_PRESENT(rb_setup_ptr,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rb_setup_ptr->integrityProtectionModeInfo);
          }
			  if(RRCRB_R9_MSG_IE_PRESENT(rb_setup_ptr,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(rb_setup_ptr->cipheringModeInfo);
          }
        }
        break;
      
        case T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration:
        {
          rrc_TransportChannelReconfiguration_r9_IEs *tcreconfig_ptr =
            RRC_EXTRACT_R9_MSG_POINTER(dcch_msg_ptr,transportChannelReconfiguration,transportChannelReconfiguration_r9);

			  if(RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,integrityProtectionModeInfo) )
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info =   &(tcreconfig_ptr->integrityProtectionModeInfo);
          }
			  if(RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,cipheringModeInfo) )
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info =  &(tcreconfig_ptr->cipheringModeInfo);
          }
        }
        break;
      
        case T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration:
        {
          rrc_PhysicalChannelReconfiguration_r9_IEs *pcreconfig_ptr =
            RRC_EXTRACT_R9_MSG_POINTER(dcch_msg_ptr,physicalChannelReconfiguration,physicalChannelReconfiguration_r9);

			  if(RRCRB_R9_MSG_IE_PRESENT(pcreconfig_ptr,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(pcreconfig_ptr->integrityProtectionModeInfo);
          }
			  if(RRCRB_R9_MSG_IE_PRESENT(pcreconfig_ptr,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(pcreconfig_ptr->cipheringModeInfo);
          }
        }
        break;      
        case T_rrc_DL_DCCH_MessageType_uraUpdateConfirm:
        {
           /* For uraUpdateConfirm, I.E integrityProtectionModeInfo is supported in R5/R7 messages only. */
           if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
              uraUpdateConfirm_r7.m.integrityProtectionModeInfoPresent )
           {
             srns_info->integrity_info_present = TRUE;
             srns_info->integrity_protection_mode_info = 
               &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
                 uraUpdateConfirm_r7.integrityProtectionModeInfo);
           }
           if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
              uraUpdateConfirm_r7.m.cipheringModeInfoPresent )
           {
             srns_info->ciphering_mode_info_present = TRUE;
             srns_info->ciphering_mode_info = 
               &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
                 uraUpdateConfirm_r7.cipheringModeInfo);
           }
         }
         break;
            
        default:
          MSG_MED("DL DCCH MSG %d not supported for SRNS-reloc",msg_type,0,0);
          status = FAILURE;
          break;
      }
      break;

    default:
      WRRC_MSG1_HIGH("DL-DCCH msg only for SRNS-reloc But DL SDU Type is %d",cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
      status = FAILURE;
      break;
  }

  /*Set Integrity Check-info only if Integrity info is present in the OTA.*/
  if(srns_info->integrity_info_present == TRUE)
  {
    srns_info->integrity_check_info = &(dcch_msg_ptr->integrityCheckInfo);      
  }

  return status;
}
#endif /* FEATURE_WCDMA_REL9 */


#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */

/*===========================================================================

FUNCTION rrc_get_start_value_to_transmit

DESCRIPTION
    Temporary solution to fix for our implementatioin of CR1466 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/

uint32 rrc_get_start_value_to_transmit
(
void
)
{
  WRRC_MSG1_HIGH("HHO debug. mac_start_value_to_transmit is 0x%x", mac_start_value_to_transmit >> RRC_MAC_HFN_OFFSET);
  return (mac_start_value_to_transmit >> RRC_MAC_HFN_OFFSET);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/


/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/* This function sends a CPHY_CIPHERING_KEY_REQ to L1, Its defined in
  LLC.We use here to update UL cipher keys.This is done to bypass LLC & posts 
  command directly yo L1
*/

/* =======================================================================
**                            Function Implementation
** ======================================================================= */


/*===========================================================================

FUNCTION rrcsmc_get_int_cipher_algo_from_dl_sdu_ind

DESCRIPTION
  This function extracts Ciphering Algo from the OTA message to
  internal format of RRC.

DEPENDENCIES
  None.

RETURN VALUE
  Cipher Algo in RRC internal format
  
SIDE EFFECTS
  None

===========================================================================*/
uecomdef_wcdma_cipher_algo_e_type rrcsmc_get_int_cipher_algo_from_dl_sdu_ind
(
  void *cipher_algo,
  rrc_msg_ver_enum_type msg_version
)
{
  /*Internal format of ciphering Algo*/
  uecomdef_wcdma_cipher_algo_e_type cipher_algo_int = UE_WCDMA_CIPHER_ALGO_NONE;

  if((msg_version == MSG_REL7)
  || (msg_version == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
  || (msg_version == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
	#error code not present
#endif /* FEATURE_WCDMA_REL10 */

  )
  {
    rrc_CipheringAlgorithm_r7 *cipher_algo_r7;
    cipher_algo_r7 = (rrc_CipheringAlgorithm_r7 *)cipher_algo;
    switch(*cipher_algo_r7)
    {
      case rrc_CipheringAlgorithm_r7_uea0:
        cipher_algo_int = UE_WCDMA_CIPHER_ALGO_UEA0;
        break;
        
      case rrc_CipheringAlgorithm_r7_uea1:
        cipher_algo_int = UE_WCDMA_CIPHER_ALGO_UEA1;
        break;
        
      case rrc_CipheringAlgorithm_r7_uea2:
        cipher_algo_int = UE_WCDMA_CIPHER_ALGO_UEA2;
        break;
     
      default:
        WRRC_MSG1_ERROR("Cipher Algo command unknown =%d",*cipher_algo_r7);
        cipher_algo_int = UE_WCDMA_CIPHER_ALGO_UEA0;
      break;
    }
  }
  else
  {
    rrc_CipheringAlgorithm *cipher_algo_r3;
    cipher_algo_r3 = (rrc_CipheringAlgorithm *) cipher_algo;
    switch(*cipher_algo_r3)
    {
      case rrc_CipheringAlgorithm_uea0:
        cipher_algo_int = UE_WCDMA_CIPHER_ALGO_UEA0;
        break;
        
      case rrc_CipheringAlgorithm_uea1:
        cipher_algo_int = UE_WCDMA_CIPHER_ALGO_UEA1;
        break;
     
      default:
        WRRC_MSG1_ERROR("Cipher Algo command unknown =%d",*cipher_algo_r3);
        cipher_algo_int = UE_WCDMA_CIPHER_ALGO_UEA0;
      break;
    }
  }

  return cipher_algo_int;
}
/*===========================================================================

FUNCTION rrcsmc_get_int_integrity_algo_from_dl_sdu_ind

DESCRIPTION
  This function extracts Integrity Algo from the OTA message to
  internal format of RRC.

DEPENDENCIES
  None.

RETURN VALUE
  Integrity Algo in RRC internal format
  
SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_wcdma_integrity_protection_algorithm_e_type rrcsmc_get_int_integrity_algo_from_dl_sdu_ind
(
  void * integrity_algo,
  rrc_msg_ver_enum_type msg_version
)
{
  /*Internal format of Integrity Algo*/
  uecomdef_wcdma_integrity_protection_algorithm_e_type integrity_algo_int = UE_WCDMA_INTEGRITY_PROT_ALGO_UIA1;

  if(msg_version == MSG_REL7)
  {
    rrc_IntegrityProtectionAlgorithm_r7 *integrity_algo_r7;
    integrity_algo_r7 = (rrc_IntegrityProtectionAlgorithm_r7 *) integrity_algo;
    switch(*integrity_algo_r7)
    {
      case rrc_IntegrityProtectionAlgorithm_r7_uia1:
        integrity_algo_int = UE_WCDMA_INTEGRITY_PROT_ALGO_UIA1;
        break;
        
      case rrc_IntegrityProtectionAlgorithm_r7_uia2:
        integrity_algo_int = UE_WCDMA_INTEGRITY_PROT_ALGO_UIA2;
        break;
     
      default:
        WRRC_MSG1_ERROR("Integrity Algo command unknown =%d",*integrity_algo_r7);
        integrity_algo_int = UE_WCDMA_INTEGRITY_PROT_ALGO_UIA1;
      break;
    }
  }
  else
  {
    rrc_IntegrityProtectionAlgorithm *integrity_algo_r3;
    integrity_algo_r3 = (rrc_IntegrityProtectionAlgorithm *) integrity_algo;
    switch(*integrity_algo_r3)
    {
      case rrc_IntegrityProtectionAlgorithm_uia1:
        integrity_algo_int = UE_WCDMA_INTEGRITY_PROT_ALGO_UIA1;
        break;
     
      default:
        WRRC_MSG1_ERROR("Integrity Algo command unknown =%d",*integrity_algo_r3);
        integrity_algo_int = UE_WCDMA_INTEGRITY_PROT_ALGO_UIA1;
      break;
    }
  }

  return integrity_algo_int;
}
/*===========================================================================

FUNCTION rrcsmc_get_srns_info_from_dl_sdu_ind_r6

DESCRIPTION
  This function extracts ip check info/ integrity protection mode info and
  ciphering mode info from SRNS relocation triggering messages for Release 6 version
  of specs

  NOTE:  There is no R6 utranMobilityInformation message

DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if extraction was successful
  The function returns FAILURE if extraction was unsucceful.

SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_status_e_type rrcsmc_get_srns_info_from_dl_sdu_ind_r7
(
  rrcsmc_srns_relocation_security_info_r7_type * srns_info,
  rrc_cmd_type *cmd_ptr
)
{
  uecomdef_status_e_type status = SUCCESS;
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;       /* Pointer to Downlink DCCH */
  uint8 msg_type;                               /*Local variable to store message type */             
  
  /* Get the OTA message pointer */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
  
  srns_info->integrity_info_present = FALSE;
  srns_info->ciphering_mode_info_present = FALSE;          
  
  switch (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
  { 
    case rrc_DL_DCCH_Message_PDU :
      
      msg_type = 
        rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
        cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      switch (msg_type)
      {

        case T_rrc_DL_DCCH_MessageType_cellUpdateConfirm:
         {
          rrc_CellUpdateConfirm_r7_IEs *cucnf_ptr = 
            RRC_EXTRACT_R7_MSG_POINTER(dcch_msg_ptr, cellUpdateConfirm, cellUpdateConfirm_r7);
          
          if(RRCRBR_R7_MSG_IE_PRESENT(cucnf_ptr,rrc_CellUpdateConfirm_r7_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(cucnf_ptr->integrityProtectionModeInfo);
          }
          if(RRCRBR_R7_MSG_IE_PRESENT(cucnf_ptr,rrc_CellUpdateConfirm_r7_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(cucnf_ptr->cipheringModeInfo);
          }
        }
          break;

        case T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration:
         {
          rrc_RadioBearerReconfiguration_r7_IEs *rbrreconfig_ptr = 
            RRC_EXTRACT_R7_MSG_POINTER(dcch_msg_ptr,radioBearerReconfiguration,radioBearerReconfiguration_r7);

          if(RRCRBR_R7_MSG_IE_PRESENT(rbrreconfig_ptr,rrc_RadioBearerReconfiguration_r7_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rbrreconfig_ptr->integrityProtectionModeInfo);
          }
          if(RRCRBR_R7_MSG_IE_PRESENT(rbrreconfig_ptr,rrc_RadioBearerReconfiguration_r7_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(rbrreconfig_ptr->cipheringModeInfo);
          }
        }
          break;
     
        case T_rrc_DL_DCCH_MessageType_radioBearerRelease:
          {
          rrc_RadioBearerRelease_r7_IEs* rb_release_ptr = 
            RRC_EXTRACT_R7_MSG_POINTER(dcch_msg_ptr,radioBearerRelease,radioBearerRelease_r7);
            
          if( RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,integrityProtectionModeInfo))
            {
              srns_info->integrity_info_present = TRUE;
              srns_info->integrity_protection_mode_info = &(rb_release_ptr->integrityProtectionModeInfo);
            }
          if( RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,cipheringModeInfo))
            {
              srns_info->ciphering_mode_info_present = TRUE;
              srns_info->ciphering_mode_info =  &(rb_release_ptr->cipheringModeInfo);
            }
          }
          break;
        case T_rrc_DL_DCCH_MessageType_radioBearerSetup:
        {
          rrc_RadioBearerSetup_r7_IEs *rb_setup_ptr = 
            RRC_EXTRACT_R7_MSG_POINTER(dcch_msg_ptr,radioBearerSetup,radioBearerSetup_r7);

          if(RRCRBR_R7_MSG_IE_PRESENT(rb_setup_ptr,rrc_RadioBearerSetup_r7_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rb_setup_ptr->integrityProtectionModeInfo);
          }
          if(RRCRBR_R7_MSG_IE_PRESENT(rb_setup_ptr,rrc_RadioBearerSetup_r7_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info =  &(rb_setup_ptr->cipheringModeInfo);
          }
          }
          break;
          
        case T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration:
         {
          rrc_TransportChannelReconfiguration_r7_IEs *tcreconfig_ptr =
            RRC_EXTRACT_R7_MSG_POINTER(dcch_msg_ptr,transportChannelReconfiguration,transportChannelReconfiguration_r7);
          
          if(RRCRBR_R7_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,integrityProtectionModeInfo) )
            {
              srns_info->integrity_info_present = TRUE;
              srns_info->integrity_protection_mode_info =   &(tcreconfig_ptr->integrityProtectionModeInfo);
            }
          if(RRCRBR_R7_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,cipheringModeInfo) )
            {
              srns_info->ciphering_mode_info_present = TRUE;
              srns_info->ciphering_mode_info =  &(tcreconfig_ptr->cipheringModeInfo);
            }
         }
          break;
          
        case T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration:
        {
          rrc_PhysicalChannelReconfiguration_r7_IEs *pc_reconfig_ptr = 
            RRC_EXTRACT_R7_MSG_POINTER(dcch_msg_ptr,physicalChannelReconfiguration,physicalChannelReconfiguration_r7);

          if(RRCRBR_R7_MSG_IE_PRESENT(pc_reconfig_ptr,rrc_PhysicalChannelReconfiguration_r7_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(pc_reconfig_ptr->integrityProtectionModeInfo);
          }
          if(RRCRBR_R7_MSG_IE_PRESENT(pc_reconfig_ptr,rrc_PhysicalChannelReconfiguration_r7_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(pc_reconfig_ptr->cipheringModeInfo);
          }
        }
        break;      
                
        case T_rrc_DL_DCCH_MessageType_utranMobilityInformation:
        {
          rrc_UTRANMobilityInformation_r7_IEs *utran_mob_info = 
            RRC_EXTRACT_R7_MSG_POINTER_UMI_URA(dcch_msg_ptr,utranMobilityInformation,utranMobilityInformation_r7);
          
          if(RRCRBR_R7_MSG_IE_PRESENT(utran_mob_info,rrc_UTRANMobilityInformation_r7_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(utran_mob_info->integrityProtectionModeInfo);
          }
          if(RRCRBR_R7_MSG_IE_PRESENT(utran_mob_info,rrc_UTRANMobilityInformation_r7_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(utran_mob_info->cipheringModeInfo);
          }
        }
        break;
            
        case T_rrc_DL_DCCH_MessageType_uraUpdateConfirm:
        {
          rrc_URAUpdateConfirm_r7_IEs *ura_update_cfn =  
            RRC_EXTRACT_R7_MSG_POINTER_UMI_URA(dcch_msg_ptr,uraUpdateConfirm,uraUpdateConfirm_r7);
          
          if(RRCRBR_R7_MSG_IE_PRESENT(ura_update_cfn,rrc_URAUpdateConfirm_r7_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(ura_update_cfn->integrityProtectionModeInfo);
          }
          if(RRCRBR_R7_MSG_IE_PRESENT(ura_update_cfn,rrc_URAUpdateConfirm_r7_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(ura_update_cfn->cipheringModeInfo);
          }
        }
        break;

        default:
          MSG_MED("MSG not supported for SRNS-reloc",0,0,0);
          status = FAILURE;
          break;
      }
      break;

    default:
      WRRC_MSG0_HIGH("DL-DCCH msg only for SRNS-reloc");
      status = FAILURE;
      break;
  }
  
  if(srns_info->integrity_info_present == TRUE)
  {
    srns_info->integrity_check_info = &(dcch_msg_ptr->integrityCheckInfo);      
  }

  return status;
}
/*===========================================================================

FUNCTION rrcsmc_is_integrity_prot_mode_info_present

DESCRIPTION
  This function returns TRUE if IE "integrityProtectionModeInfo" is present in
  configuration message(byte stream), otherwise it returns FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_is_integrity_prot_mode_info_present
(
  uint8 *dl_sdu_ptr,
  uint8  msg_type
)
{
  /* Check whether Integrity Check info is present */
  if (0x00 != b_unpackb(dl_sdu_ptr, INTEGRITY_CHECK_INFO_PRESENT_POS,
                        NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT))
  {
    /* Check whether message contains 'r3' IE or not */
    if (0x00 == b_unpackb(dl_sdu_ptr, DL_DCCH_R3_INDICATOR_POS,
                        NUM_OF_BITS_FOR_DL_DCCH_R3_INDICATOR))
    {
      /* Check whether IE 'integrityProtectionModeInfo' is present or not */
      if (0x00 != b_unpackb(dl_sdu_ptr, 
                  DCCH_R3_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS,
                        NUM_OF_BITS_FOR_DCCH_R3_INTEGRITY_PROTECTION_MODE_INFO_PRESENT))
      {
        return(TRUE);
      }
    }
    else /* indicates later-than-r3 */
    {
      if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
      {
        if((msg_type == T_rrc_DL_DCCH_MessageType_utranMobilityInformation) || 
           (msg_type == T_rrc_DL_DCCH_MessageType_uraUpdateConfirm))
        {
          /* indicates later_than_r3_criticalExtensions_r5_chosen */
          if(0x00 == b_unpackb(dl_sdu_ptr, 
                DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_R5_POS,
                NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_R5))
          {
            if(0x00 != b_unpackb(dl_sdu_ptr, 
                DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_R5_POS + 2,
                NUM_OF_BITS_FOR_DL_DCCH_R5_INTEGRITY_PROTECTION_MODE_INFO_PRESENT))
            {
              return (TRUE);
            }
          }
          else
          {
            /* indicates later_than_r3_criticalExtensions_r5_chosen */
            if(0x00 == b_unpackb(dl_sdu_ptr, 
                  DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN_R7_POS,
                  NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_R5))
            {
              if(0x00 != b_unpackb(dl_sdu_ptr, 
                  DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN_R7_POS + 3,
                  NUM_OF_BITS_FOR_DL_DCCH_R5_INTEGRITY_PROTECTION_MODE_INFO_PRESENT))
              {
                return (TRUE);
              }
            }
          }
        }
        else
        {
          /* indicates later_than_r3_criticalExtensions_criticalExtensions_chosen */
          if (0x00 != b_unpackb(dl_sdu_ptr, 
          DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN_POS,
                NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN))
          {
            /* indicates later_than_r3_criticalExtensions_criticalExtensions_r5_chosen */
            if (0x00 == b_unpackb(dl_sdu_ptr, 
                DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN_R5_POS,
                NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRITICAL_EXTN_CRITICAL_EXTN_R5))
            { 
              if (0x00 != b_unpackb(dl_sdu_ptr, 
                  DL_DCCH_R5_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS,
                  NUM_OF_BITS_FOR_DL_DCCH_R5_INTEGRITY_PROTECTION_MODE_INFO_PRESENT))
              { 
                return(TRUE);
              } 
            } 
            else /* indicates  later_than_r3_criticalExtn_criticalExtn_criticalExtns_chosen */
            {
              /* indicates later_than_r3_criticalExtensions_criticalExtensions_criticalExtensions_r6_chosen */
              if (0x00 == b_unpackb(dl_sdu_ptr, 
                  DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R6_POS,
                  NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R6))
              {
                /* check the value of bit which whether integrity protection
                   mode info is included or not */
                if (0x00 != b_unpackb(dl_sdu_ptr, 
                    DL_DCCH_R6_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS,
                    NUM_OF_BITS_FOR_DL_DCCH_R6_INTEGRITY_PROTECTION_MODE_INFO_PRESENT))
                { 
                  WRRC_MSG0_HIGH("RRCEUL: SRNS relocation case detected - intProtModeInfo present");
                  return(TRUE);
                } 
              } 
              else /* indicates  later_than_r3_critExtn_critExtn_critExtn_critExtn_chosen */
              {
                /* indicates later_than_r3_criticalExtensions_criticalExtensions_criticalExtensions_criticalExtensions_r7_chosen */
                if (0x00 == b_unpackb(dl_sdu_ptr, 
                    DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R7_POS,
                    NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R7))
                {
                  /* check the value of bit which whether integrity protection
                     mode info is included or not */
                  WRRC_MSG0_HIGH("Rel-7");
                  if(msg_type == T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration)
                  {
                    WRRC_MSG0_HIGH("Check integrity_prot_mode_info for rrc_radioBearerReconfiguration_chosen");
                    if (0x00 != b_unpackb(dl_sdu_ptr, 
                       DL_DCCH_R7_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS,
                       NUM_OF_BITS_FOR_DL_DCCH_R7_INTEGRITY_PROTECTION_MODE_INFO_PRESENT))
                    { 
                      WRRC_MSG0_HIGH("RRCR7: SRNS relocation case detected - intProtModeInfo present");
                      return(TRUE);
                    }
                    else
                    {
                      WRRC_MSG0_HIGH("Integrity_prot_mode_info not present in radioBearerReconfiguration");
                    }
                  }
                  else 
                  {
                    if (0x00 != b_unpackb(dl_sdu_ptr, 
                       DL_DCCH_R7_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS + 1,
                       NUM_OF_BITS_FOR_DL_DCCH_R7_INTEGRITY_PROTECTION_MODE_INFO_PRESENT))
                    { 
                      WRRC_MSG0_HIGH("RRCR7: SRNS relocation case detected - intProtModeInfo present");
                      return(TRUE);
                    } 
                  } 
                } 
                else
                {
                   /* indicates later_than_r3_criticalExtensions_criticalExtensions_criticalExtensions_criticalExtensions_criticalExtensions_r8_chosen */
                  if (0x00 == b_unpackb(dl_sdu_ptr, 
                      DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R8_POS,
                      NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_R8))
                  {
                    /* check the value of bit which whether integrity protection
                                   mode info is included or not */
                    if (0x00 != b_unpackb(dl_sdu_ptr, 
                      DL_DCCH_R8_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS,
                      NUM_OF_BITS_FOR_DL_DCCH_R8_INTEGRITY_PROTECTION_MODE_INFO_PRESENT))
                    { 
                      WRRC_MSG0_HIGH("RRCR8: SRNS relocation case detected - intProtModeInfo present");
                      return(TRUE);
                    }
                    else
                    {
                      WRRC_MSG0_HIGH("RRCR8: Integrity_prot_mode_info not present");
                    }
                  }
                  else
                  {
#ifdef FEATURE_WCDMA_REL9
                     /* indicates later_than_r3_criticalExtensions_criticalExtensions_criticalExtensions_criticalExtensions_criticalExtensions_later_r8_criticalExtensions_r9_chosen */
                    if (0x00 == b_unpackb(dl_sdu_ptr, 
                        DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_LATER_THAN_R8_CRIT_EXTN_R9_POS,
                        NUM_OF_BITS_FOR_DL_DCCH_LATER_THAN_R3_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_CRIT_EXTN_LATER_THAN_R8_CRIT_EXTN_R9))
                    {
                      /* check the value of bit which whether integrity protection
                                     mode info is included or not */
                      if (0x00 != b_unpackb(dl_sdu_ptr, 
                        DL_DCCH_R9_INTEGRITY_PROTECTION_MODE_INFO_PRESENT_POS,
                        NUM_OF_BITS_FOR_DL_DCCH_R9_INTEGRITY_PROTECTION_MODE_INFO_PRESENT))
                      { 
                        WRRC_MSG0_HIGH("RRCR9: SRNS relocation case detected - intProtModeInfo present");
                        return(TRUE);
                      }
                    }

#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
                    #error code not present
#endif /* FEATURE_WCDMA_REL10 */


                  }
                }
              }
            } 
          } 
          else /* indicates later_than_r3_criticalExtensions_r4_chosen */
          { 
          } 
        }
      }
    } 
  }

  return (FALSE);
}

/*===========================================================================

FUNCTION rrcsmc_get_latest_configured_cn_domain

DESCRIPTION
  Returns the latest Configured Cn Domain value which is set based on the cnDomainIdentity IE that comes
  in last received Security Mode Command.

DEPENDENCIES
  None.

RETURN VALUE
  Cn Domain

SIDE EFFECTS
  None

===========================================================================*/
rrc_cn_domain_identity_e_type rrcsmc_get_latest_configured_cn_domain(void)
{
  if (FALSE == is_revert_to_old_cipher_variable_reqd)
  {
    return(rrcsmc_int_ciph_info.latest_config_cn_id);
  }
  else
  {
    /* use the latest_config_cn_domain from backup variable because
       structure "rrcsmc_int_ciph_info" is already reverted back to old config
    */
    return(backup_of_security_variables.rrcsmc_int_ciph_info.latest_config_cn_id);
  }
  
}

/*===========================================================================

FUNCTION rrcsmc_clear_cipher_config_db

DESCRIPTION
  Clears config information in the global ciphering database.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void rrcsmc_clear_cipher_config_db(void)
{
  /*reset contents of global ciphering database*/
  cipher_config_db.l1_cipher_info.cipher_key_req.req_mask = 0x00;

  /*reset MAC information in global database*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_enable = FALSE;
  cipher_config_db.mac_cipher_info[RRC_PS_DOMAIN_CN_ID].cipher_enable = FALSE;

  /*always set mac's cipher config type to NON_HO, only for brief periods of time
    when we are doing Step1 and Step2 ciphering config.. we need to change it*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_cfg_type = 
  NON_HO_CIPHER_CFG;  
  cipher_config_db.mac_cipher_info[RRC_PS_DOMAIN_CN_ID].cipher_cfg_type = 
  NON_HO_CIPHER_CFG;   
  cipher_config_db.mac_ciph_config_affected = FALSE;
    
  /*reset the number of Ciphered UL and DL RLC channels*/
  cipher_config_db.rlc_dl_cipher_info.nchan = 0;
  cipher_config_db.rlc_ul_cipher_info.nchan = 0;

  /*case initial start of ciphering due to reception of smc message*/
  /*checking in if just to suppress lint.  will remove once Ciphering
    feature defn. is removed*/
  if(ciph_rb_info.am_rb_ciph_config.num_rb != 0)
  {
    ciph_rb_info.am_rb_ciph_config.num_rb = 0;
  }

  ciph_rb_info.um_rb_ciph_config.num_rb=0;

  /*reset the number of ciphered TM radio bearers*/
  ciph_rb_info.tm_rb_ciph_config[RRC_CS_DOMAIN_CN_ID].num_rb=0;
  ciph_rb_info.tm_rb_ciph_config[RRC_PS_DOMAIN_CN_ID].num_rb=0;

  /*set the ciphering affected flag for all domains false*/
  ciph_rb_info.tm_rb_ciph_config[RRC_CS_DOMAIN_CN_ID].affected=FALSE;
  ciph_rb_info.tm_rb_ciph_config[RRC_PS_DOMAIN_CN_ID].affected=FALSE;
}
/*===========================================================================

FUNCTION rrcsmc_is_security_reconfig_active

DESCRIPTION
  checks to see if SMC status is in INITIAL.  

DEPENDENCIES
  None.

RETURN VALUE
  Return's TRUE is SMC status is not initial
  Return's FALSE is SMC status is initial

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_is_security_reconfig_active 
(
  void
)
{
  //return's true is SMC substate is not initial, else return's false..
  if(rrcsmc_substate == RRCSMC_INITIAL)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*===========================================================================

FUNCTION rrcsmc_get_ciph_key_idx

DESCRIPTION
  Gets the Key Index in L1 representation form.
  CS-KEY0 -> 0 L1 Key Index
  CS-KEY1 -> 1 L1 Key Index
  PS-KEY0 -> 2 L1 Key Index
  PS-KEY1 -> 3 L1 Key Index

DEPENDENCIES
  None.

RETURN VALUE
  Key Index in the form L1 needs

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
static uint8 rrcsmc_get_ciph_key_idx
(
rrc_cn_domain_identity_e_type domain_id
)
{
  /*convert CS or PS key index to L1 key index
    CS-KEY0 -> 0 L1 Key Index
    CS-KEY1 -> 1 L1 Key Index
    PS-KEY0 -> 2 L1 Key Index
    PS-KEY1 -> 3 L1 Key Index
  */
  if (domain_id == RRC_CS_DOMAIN_CN_ID)
  {
    if (rrcsmc_int_ciph_info.int_ciph_info[domain_id].
        ciph_keys_info.key_used == 0)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    if (rrcsmc_int_ciph_info.int_ciph_info[domain_id].
        ciph_keys_info.key_used == 0)
    {
      return 2;
    }
    else
    {
      return 3;
    }
  }
}
/*lint +e715*/
/*===========================================================================

FUNCTION rrcsmc_init_start_value_at_power_up

DESCRIPTION
  Resets START values upon powerup.  This is done in this function and not
  in the go_to_smc_initial_substate function because START values are
  statically stored by the UE from power-up till power-down, even though
  they are updated to the SIM at every connection release.
  
  So we don't want to reset START values at every connection release.  Hence
  this function is called only at power-up.  On a side note: the need to
  initialize START values outside of SIM_INSERT_REQ or SIM_UPDATE_REQ
  arose because an emergency call may be attempted, in this case the sim is
  not present.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void rrcsmc_init_start_value_at_power_up
(
void
)
{
  uint8 i;
  
  /*copy number of cn domains into int_ciph_info struct*/  
  rrcsmc_int_ciph_info.num_cn_domains = RRC_MAX_CN_DOMAINS;

  /*Initialize the domain Ids*/
  for (i=0; i<RRC_MAX_CN_DOMAINS; i++)
  {
    if( i == 0)
    {
      rrcsmc_int_ciph_info.int_ciph_info[i].domain_id = RRC_CS_DOMAIN_CN_ID;
      
    }
    else
    {
      rrcsmc_int_ciph_info.int_ciph_info[i].domain_id = RRC_PS_DOMAIN_CN_ID;
    }
    /* Set the HFNs to 0 for emergency calls when SIM or USIM is not inserted */
    rrcsmc_int_ciph_info.int_ciph_info[i].hfn[0] = 0x00;
    rrcsmc_int_ciph_info.int_ciph_info[i].hfn[1] = 0x00;
    rrcsmc_int_ciph_info.int_ciph_info[i].hfn[2] = 0x00;
  }
}
/*===========================================================================

FUNCTION rrcsmc_update_ciph_keys_used

DESCRIPTION
  This function updates the ciphering key indices when a Security
  Mode command with START/RESTART is used.  If authentication is
  done for the CN domain for which Ciphering is being START/RESTART
  then the key indices and keys for that cn domain are updated

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void rrcsmc_update_ciph_keys_used
(
rrc_cn_domain_identity_e_type domain_id
)
{
  /* update the global variable common to both ciphering and integrity 
     only when 'is_revert_to_old_cipher_variable_reqd' is set to FALSE i.e.
     only when we dont have to revert back to old configuration.
     Else no need to change any data structure because we have already
     reverted back to old data-structure */
  if (FALSE == is_revert_to_old_cipher_variable_reqd)
  {
    /*if a new key is received for this domain id then start using the new key,
      set the new key flag to false and start using the new key.  New keys should
      be only if ciphering is START/RESTART.  For a reconfig we should not move to
      a new key*/
    if (rrcsmc_int_ciph_info.int_ciph_info[domain_id].
        ciph_keys_info.new_key == TRUE)
    {
      rrcsmc_int_ciph_info.int_ciph_info[domain_id].
      ciph_keys_info.new_key = FALSE;
  
      if (rrcsmc_int_ciph_info.int_ciph_info[domain_id].
          ciph_keys_info.key_used == 0)
      {
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].
        ciph_keys_info.key_used = 1;
      }
      else
      {
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].
        ciph_keys_info.key_used = 0;  
      }
    }
  }
}

/*===========================================================================

FUNCTION rrcsmc_go_to_smc_initial_substate

DESCRIPTION
  Resets all stored info from previous SMC messages.  Called when going
  to IDLE state.  START values and keys are not reset, as these are
  statically stored by the UE from power-up to power-down, they can only
  be RESET as power-up, can be updated by SIM_INSERT_REQ or by 
  SIM_UPDATE_REQ only.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void rrcsmc_go_to_smc_initial_substate
(
void
)
{
  rrcsmc_substate = RRCSMC_INITIAL;

  /*set the reconfig and status flags to false and not started for integrity
    and ciphering global flags*/
  integrity_protection_info.reconfig = FALSE;
  integrity_protection_info.ip_status = RRCSMC_SECURITY_NOT_STARTED;        
  integrity_protection_info.modify_received = FALSE;


  /*set ciphering reconfig info to false*/
  ciphering_info.reconfig = FALSE;
  
  /*set ciphering status for both cn domains to false*/
  rrcsmc_int_ciph_info.ciph_status[RRC_CS_DOMAIN_CN_ID] = 
  RRCSMC_SECURITY_NOT_STARTED;
  rrcsmc_int_ciph_info.ciph_status[RRC_PS_DOMAIN_CN_ID] = 
  RRCSMC_SECURITY_NOT_STARTED;

  /*set is_sync_ind_sent_to_nas status for both cn domains to false*/
  rrcsmc_int_ciph_info.is_sync_ind_sent_to_nas[RRC_CS_DOMAIN_CN_ID] = FALSE;
  rrcsmc_int_ciph_info.is_sync_ind_sent_to_nas[RRC_PS_DOMAIN_CN_ID] = FALSE;

  /*Print ciphering key here*/
  PRINT_CIPHER_INT_KEY(RRC_PRINT_CIPHERING_KEY,saved_security_key_set)

  //if authentication was done during a call and SMC not received, then we should update
  //keys indices correctly right here.. when we go to initial substate, we reset all the new_key
  //info and such, hence we might lose the info that we got new keys during a call, and subsequent
  //call ciphering may be affected if auth not done in subsequent call or no power-down

  /* If new key flag is set for CS, before resetting it, indicate SYNC IND to RR*/
  if(rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.new_key == TRUE)
  {
    rrciho_send_cipher_sync_ind();
  }
  rrcsmc_update_ciph_keys_used(RRC_CS_DOMAIN_CN_ID);
  rrcsmc_update_ciph_keys_used(RRC_PS_DOMAIN_CN_ID);

  /*set the new keys flag to false initially*/
  rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.
  new_key = FALSE;
  rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].ciph_keys_info.
  new_key = FALSE;

  rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.
  new_key_for_start_value = FALSE;
  rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].ciph_keys_info.
  new_key_for_start_value = FALSE;

  /*set the previous calls ciphering flags to false*/
  NEW_TM_RB_IND_FLAG = FALSE;
  START_VALUE_TO_TRANSMIT_FLAG=FALSE;
  start_value_to_transmit=0;
  start_value_to_transmit_cs_hho=0;
  mac_start_value_to_transmit=0;
  rrcsmc_int_ciph_info.saved_ciph_algo[RRC_CS_DOMAIN_CN_ID] = UE_WCDMA_CIPHER_ALGO_UEA0;
  rrcsmc_int_ciph_info.saved_ciph_algo[RRC_PS_DOMAIN_CN_ID] = UE_WCDMA_CIPHER_ALGO_UEA0;

 rrcsmc_int_ciph_info.saved_integrity_algo[RRC_CS_DOMAIN_CN_ID] = UE_WCDMA_INTEGRITY_PROT_ALGO_UIA1;
 rrcsmc_int_ciph_info.saved_integrity_algo[RRC_PS_DOMAIN_CN_ID] = UE_WCDMA_INTEGRITY_PROT_ALGO_UIA1;

  /*flag to indicate that START value was updated due to Initial Direct Transfer
  message*/
  rrcsmc_int_ciph_info.start_value_update_flag[RRC_CS_DOMAIN_CN_ID] = FALSE;
  rrcsmc_int_ciph_info.start_value_update_flag[RRC_PS_DOMAIN_CN_ID] = FALSE;

  rrcsmc_int_ciph_info.int_start_status_for_conn[RRC_CS_DOMAIN_CN_ID] = FALSE;
  rrcsmc_int_ciph_info.int_start_status_for_conn[RRC_PS_DOMAIN_CN_ID] = FALSE;

  rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_CS_DOMAIN_CN_ID] = 0;
  rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_PS_DOMAIN_CN_ID] = 0;
  
  saved_security_mode_command_msg.dpch_ciph_act_time_present = FALSE;

  l2_ack_rcvd_for_smc_complete_flag = FALSE;
  rrcsmc_send_l2_ack_ind_to_ddt_flag= FALSE;

  /*clear cipher configuration database as we are going to idle state*/
  rrcsmc_clear_cipher_config_db();
}
/*===========================================================================

FUNCTION rrcsmc_is_cs_key_pending

DESCRIPTION
  returns TRUE if CS key is pending
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

static boolean rrcsmc_is_cs_key_pending(void)
{
  if(rrcsmc_int_ciph_info.ciph_status[RRC_CS_DOMAIN_CN_ID] == RRCSMC_SECURITY_STARTED)
  {
    return rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.new_key;
  }
  else if(integrity_protection_info.ip_status == RRCSMC_SECURITY_STARTED )
  {
    /*dl_new_key_flag_per_rb is being used for the case where integrity is started but not ciphering*/
    return integrity_protection_info.count_i_info[RRC_CS_DOMAIN_CN_ID].dl_new_key_flag_per_rb[DCCH_AM_RADIO_BEARER_ID];  
  }
  else
  {
    WRRC_MSG0_ERROR("Security is not yet started");     
    return FALSE;
  }
}
/*===========================================================================

FUNCTION rrcsmc_copy_pending_keys_to_saved_security_key_set

DESCRIPTION
  If there is a pending key, it is updated to saved security key set.
  For IRAT WtoG handover spec says not to apply the pending keys immediately.
  But we should reset the start on handover to G. pending_cs_key_to_be_activated
  flag is used to reset the start value to zero.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void rrcsmc_copy_pending_keys_to_saved_security_key_set(void)
{
  if(TRUE == rrcsmc_is_cs_key_pending())
  {
    WRRC_MSG0_HIGH("Save pending security keys generated in W-RAT on WtoG handover success for CS domain");
    /*Mark saved security keys as valid*/
    saved_security_key_set.grat_key_set_valid = TRUE;

    /*Save ciphering key here*/
    if(rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.key_used == 1)
    {
      WCDMA_MEMCPY(saved_security_key_set.ciphering_key,
                 sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                 rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.key[0],
                 RRC_MAX_CIPHERING_KEY_LENGTH);
    }
    else
    {
      WCDMA_MEMCPY(saved_security_key_set.ciphering_key,
           sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
           rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.key[1],
           RRC_MAX_CIPHERING_KEY_LENGTH);
    }

    /*Print ciphering key here*/
    PRINT_CIPHER_INT_KEY(RRC_PRINT_CIPHERING_KEY,saved_security_key_set)

    /*Save integrity key here*/
    WCDMA_MEMCPY(saved_security_key_set.integrity_key,
                 sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                 rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key,
                 RRC_MAX_INTEGRITY_KEY_LENGTH);

    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.new_key = FALSE;
    integrity_protection_info.count_i_info[RRC_CS_DOMAIN_CN_ID].dl_new_key_flag_per_rb[DCCH_AM_RADIO_BEARER_ID] = FALSE;
    saved_security_key_set.pending_cs_key_to_be_activated = TRUE;
  }
}
/*===========================================================================

FUNCTION rrcsmc_reset_pending_cs_key_to_be_activated

DESCRIPTION
  Resets the pending_cs_key_to_be_activated flag
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_reset_pending_cs_key_to_be_activated(void)
{
  saved_security_key_set.pending_cs_key_to_be_activated = FALSE;
}
/*===========================================================================

FUNCTION rrcsmc_reset_smc_ip_started_flag

DESCRIPTION
  Resets the STATUS flag to  RRCSMC_IP_NOT_STARTED.  This function will be
  called by rrcrce procedure on entering connected mode.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_reset_smc_ip_started_flag(void)
{
  integrity_protection_info.ip_status = RRCSMC_SECURITY_NOT_STARTED;
}
/*===========================================================================
FUNCTION rrcsmc_is_ciphering_enabled

DESCRIPTION
  This function indicates if ciphering is started or not.
  Right now we return SECURITY started if either cn domains is started,
  because we are not supporting concurrent services.  But when concurrent
  services is supported, this function will take cn domain as argument
  and only STATUS of the queried CN domain will be sent.
  
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS or FAILURE whether ciphering is started or not respectively.

SIDE EFFECTS
  None
===========================================================================*/
rrcsmc_security_status_e_type rrcsmc_is_ciphering_enabled
(
void
)
{
  if ((rrcsmc_int_ciph_info.ciph_status[RRC_CS_DOMAIN_CN_ID] == 
       RRCSMC_SECURITY_STARTED) ||
      (rrcsmc_int_ciph_info.ciph_status[RRC_PS_DOMAIN_CN_ID] == 
       RRCSMC_SECURITY_STARTED))
  {
    return RRCSMC_SECURITY_STARTED;
  }
  else
  {
    return RRCSMC_SECURITY_NOT_STARTED;
  }
}
/*===========================================================================

FUNCTION rrcsmc_tm_ciph_active

DESCRIPTION
  Checks to see if TM-Ciphering is active or not.

DEPENDENCIES
  None.

RETURN VALUE
  True:If TM ciphering is active
  False: If TM ciphering is inactive

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_tm_ciph_active()
{
  return cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_enable;
}
/*===========================================================================

FUNCTION rrcsmc_update_ciphering_activation_cfn

DESCRIPTION
  Update Ciphering Activation CFN global var.  This value is updated by
  other procedures if they receive a Ciphering Activation CFN from the
  N/W or with the current CFN which they query from L1

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_ciphering_activation_cfn
(
uint8 ciphering_activation_cfn
)
{
  /*update value of tm_ciph_act_cfn by RBE or other reconfig procedures*/
  tm_ciph_act_cfn = ciphering_activation_cfn;

  /*foll. 3 lines of code in here just to suppress lint error.
    will remove once Ciphering Feature defn. is removed */
  if(tm_ciph_act_cfn == 0)
  {
    MSG_LOW("Ciph Act Time set to 0",0,0,0);
  }
}
/*===========================================================================

FUNCTION rrcsmc_is_new_tm_rb_setup

DESCRIPTION
  If a new TM Radio Bearer is being added, then send TM RB ciphering act
  CFN to the procedure which configured the TM radio bearers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_is_new_tm_rb_setup
(
void
)
{
  /*if tm rb's are being setup for the first time return true and reset the
    flag else return false*/
  if (NEW_TM_RB_IND_FLAG == TRUE)
  {
    NEW_TM_RB_IND_FLAG = FALSE;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/*===========================================================================

FUNCTION rrcsmc_convert_START_to_count_c_i

DESCRIPTION
  This function converts a byte stream containing START to
  uint32 representation of count_c_i

DEPENDENCIES
  None.

RETURN VALUE
  Returns uint32 which is the count_c_i
===========================================================================*/
static uint32 rrcsmc_convert_START_to_count_c_i
(
byte      * start_byte_stream,
uint32    seq_num
)
{
  uint32 local_count_c_i=0;/*local var to store count_i*/

  /*unpack the 20bits of start to a uint32 local var*/
  local_count_c_i = b_unpackd(start_byte_stream, RRC_DEFAULT_BIT_POS, 
                              RRC_START_VALUE_LENGTH);

  /*move the 20bits left 12 times to generate count_c_i*/
  local_count_c_i <<= 12;

  /*mask the sequence number part of the seq_num var and set the rest 
    of the bits to 0's*/      
  seq_num &= 0xF;

  /*generate count_c_i by bitwise ORing masked HFN and SEQ_NUM*/
  local_count_c_i |= seq_num;

  return local_count_c_i;

}/*rrcsmc_convert_START_to_count_c_i*/
/*===========================================================================

FUNCTION rrcsmc_convert_count_c_i_to_START

DESCRIPTION
  This function extracts to a byte stream the START part of
  uint32 representation of count_c_i

DEPENDENCIES
  None.

RETURN VALUE
  None.
===========================================================================*/
static void rrcsmc_convert_count_c_i_to_START
(
byte      * start_byte_stream,
uint32    count_c_i
)
{
  uint32 local_count_c_i;

  /*copy the contents of count_c_i to a local variable and convert the
    count_c_i variable to an integer value of START by right shifting
    12 bits*/
  local_count_c_i = count_c_i;
  local_count_c_i >>=12;

  b_packd(local_count_c_i, start_byte_stream, RRC_DEFAULT_BIT_POS, 
          RRC_START_VALUE_LENGTH);

}/*rrcsmc_convert_count_c_i_to_START*/
/*===========================================================================

FUNCTION rrcsmc_get_latest_transmitted_start_values

DESCRIPTION
  This interface returns the START values that are updated latest to the
  N/W in any Uplink message.
  This function populates the start value for Signalling RB and for RAB's.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_get_latest_transmitted_start_values
(
rrcsmc_re_est_start_list * start_list
)
{
  /*get the PS domains' HFN to be sent to RLC*/
  if(rrcsmc_int_ciph_info.start_value_update_flag[RRC_PS_DOMAIN_CN_ID] == TRUE)
  {
    start_list->hfn_rab =
    rrcsmc_convert_START_to_count_c_i(
      rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].hfn_latest, 0);
  }
  else
  {
    start_list->hfn_rab =
    rrcsmc_convert_START_to_count_c_i(
      rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].hfn, 0);
  }

  /*if latest config cn domain is also PS domain, then use same HFN for
    SRB too.. else query the CS domain's HFN and initialize the structure*/
  if(rrcsmc_int_ciph_info.latest_config_cn_id == RRC_PS_DOMAIN_CN_ID)
  {
    start_list->hfn_srb = start_list->hfn_rab;

    //RAB is for PS domain, latest config cn domain is also PS domain, so copy
    //PS domain algo for RAB and SRB..

    start_list->ciph_algo_srb = rrcsmc_int_ciph_info.saved_ciph_algo[RRC_PS_DOMAIN_CN_ID];
    start_list->ciph_algo_urb = rrcsmc_int_ciph_info.saved_ciph_algo[RRC_PS_DOMAIN_CN_ID];
  }
  else
  {
    if(rrcsmc_int_ciph_info.start_value_update_flag[RRC_CS_DOMAIN_CN_ID] == TRUE)
    {
      start_list->hfn_srb =
        rrcsmc_convert_START_to_count_c_i(
          rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn_latest, 0);
    }
    else
    {
      start_list->hfn_srb =
        rrcsmc_convert_START_to_count_c_i(
          rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn, 0);
    }

    //RAB is for PS domain, but latest config cn domain is for CS domain, so copy
    //PS domain algo for RAB and CS algo for SRB..
    start_list->ciph_algo_srb = rrcsmc_int_ciph_info.saved_ciph_algo[RRC_CS_DOMAIN_CN_ID];
    start_list->ciph_algo_urb = rrcsmc_int_ciph_info.saved_ciph_algo[RRC_PS_DOMAIN_CN_ID];
  }

  //convert the count_c's to HFN's by right shifting 12 times for AM RB's
  start_list->hfn_srb >>= RRC_RLC_AM_HFN_OFFSET;
  start_list->hfn_rab >>= RRC_RLC_AM_HFN_OFFSET;
}
/*===========================================================================

FUNCTION rrcsmc_get_latest_transmitted_start_value_per_domain

DESCRIPTION
  This interface returns the START value per domain.

DEPENDENCIES
  None.

RETURN VALUE
  START value per domain

SIDE EFFECTS
  None

===========================================================================*/
static uint32 rrcsmc_get_latest_transmitted_start_value_per_domain
(
  rrc_cn_domain_identity_e_type domain_id
)
{
  uint32 local_start;

  /*get the PS domains' HFN to be sent to RLC*/
  if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id] == TRUE)
  {
    local_start =
    rrcsmc_convert_START_to_count_c_i(
      rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest, 0);
  }
  else
  {
    local_start =
    rrcsmc_convert_START_to_count_c_i(
      rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn, 0);
  }

  //convert the count_c's to START by right shifting 12 times
  local_start >>= RRC_RLC_AM_HFN_OFFSET;
  return local_start;
}
/*===========================================================================

FUNCTION rrcsmc_get_and_reset_start_value_to_transmit

DESCRIPTION
  If a new UM or AM Radio Bearer is being added, we need to send start
  values to the n/w side.  The reconfig procedure queries this start
  value to transmit and also this flag is reset here bcos once this
  start value is sent, the flag has to be reset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_and_reset_start_value_to_transmit
(
byte * start_value
)
{
  /*if start value to transmit is set, then sent the start value, else set
    flag to false*/
  if (START_VALUE_TO_TRANSMIT_FLAG == TRUE)
  {
    rrcsmc_convert_count_c_i_to_START(start_value, start_value_to_transmit);

    WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[start_value_to_transmit_domain].hfn_latest,
                 sizeof(byte) * RRC_MAX_HFN_LENGTH,
                 start_value,
                 RRC_MAX_START_LENGTH);

    rrcsmc_int_ciph_info.start_value_update_flag[start_value_to_transmit_domain] 
      = TRUE;
    
    START_VALUE_TO_TRANSMIT_FLAG = FALSE;
    return TRUE;    
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION rrcsmc_get_calc_start_value

DESCRIPTION
  Returns the calculated start_value_to_transmit

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_calc_start_value
(
byte * start_value
)
{
  boolean ret_val = FALSE;
 
  WRRC_MSG2_HIGH("GET SVTF:%d SVT:0x%x",
           START_VALUE_TO_TRANSMIT_FLAG,
           start_value_to_transmit);
  
  if (START_VALUE_TO_TRANSMIT_FLAG == TRUE)
  {
    rrcsmc_convert_count_c_i_to_START(start_value, start_value_to_transmit);
    ret_val = TRUE;    
  }
  return ret_val;
}

/*===========================================================================

FUNCTION rrcsmc_get_start_value_to_transmit

DESCRIPTION
  Returns the calculated start_value_to_transmit

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rrcsmc_get_start_value_to_transmit
(
  void
)
{
  WRRC_MSG2_HIGH("GET SVTF:%d SVT:0x%x",
           START_VALUE_TO_TRANSMIT_FLAG,
           start_value_to_transmit);
  
  if (START_VALUE_TO_TRANSMIT_FLAG == TRUE)
  {
    return start_value_to_transmit;    
  }

  return 0;
}

/*===========================================================================

FUNCTION rrcsmc_get_latest_transmitted_start_values

DESCRIPTION
  This interface returns the START values that are updated latest to the
  N/W in any Uplink message.
  This function populates the start value for Signalling RB and for RAB's.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_start_list_with_latest_transmitted_start_value
(
rrcsmc_re_est_start_list * start_list
)
{
  start_list->hfn_rab = start_value_to_transmit;    
  start_list->hfn_rab >>= RRC_RLC_AM_HFN_OFFSET;

  if(rrcsmc_int_ciph_info.latest_config_cn_id == rrcsmc_get_start_value_to_transmit_domain())
  {
    start_list->hfn_srb = start_value_to_transmit; 
    start_list->hfn_srb >>= RRC_RLC_AM_HFN_OFFSET;
  }
}

/*===========================================================================

FUNCTION rrcsmc_get_start_value_to_transmit_flag_status

DESCRIPTION
  Returns the status of START_VALUE_TO_TRANSMIT_FLAG

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_start_value_to_transmit_flag_status
(
  void
)
{
  if (START_VALUE_TO_TRANSMIT_FLAG == TRUE)
    return TRUE;    
  else
    return FALSE;
}

/*===========================================================================

FUNCTION rrcsmc_process_l2_ack_ind_to_dt_procedures

DESCRIPTION
If DT procedures have registered for L2_ACK IND of SMC, then send the IND
cmd to DDT procedure.

DEPENDENCIES
  TRUE/FALSE

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void rrcsmc_process_l2_ack_ind_to_dt_procedures
(
  void
)
{
  rrc_cmd_type *int_cmd_ptr;

  if(rrcsmc_send_l2_ack_ind_to_ddt_flag == TRUE)
  {

    /*this cmd indicates to ddt procedure that l2 ack has been received for smc
    command*/

    int_cmd_ptr =  rrc_get_int_cmd_buf();
    /* Fill in the command parameters for RRC_SMC_DDT_L2_ACK_IND */
    int_cmd_ptr->cmd_hdr.cmd_id = RRC_SMC_DDT_L2_ACK_IND;

    rrc_put_int_cmd(int_cmd_ptr);  

    rrcsmc_send_l2_ack_ind_to_ddt_flag=FALSE;

    WRRC_MSG0_HIGH("Send L2 Ack Ind to DDT Procedure");

  }
}
/*===========================================================================

FUNCTION rrcsmc_get_smc_procedure_complete_status

DESCRIPTION
  This status is queried by UDT procedure.  If procedure is waiting for L2_ACK
  for SMC complete or for lower layer ciphering_configuration, then return
  TRUE.  UDT procedure will wait for indication from SMC when L2_ACK is rcvd.
DEPENDENCIES
  TRUE/FALSE

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_smc_procedure_complete_status
(
  rrc_cn_domain_identity_e_type cn_id, rrc_RB_Identity rb_id
)
{
  WRRC_MSG1_HIGH("DT queried L2 ACK State for rb %d",rb_id);

  if((rrcsmc_substate == RRCSMC_WAIT_FOR_L2_ACK) ||
     (rrcsmc_substate == RRCSMC_WAIT_FOR_CIPH_CONFIG_CNF))
  {
    MSG_HIGH("CN ID=%d,sync_ind_sent status for CS=%d,PS=%d",
      cn_id,
    rrcsmc_int_ciph_info.is_sync_ind_sent_to_nas[RRC_CS_DOMAIN_CN_ID],
    rrcsmc_int_ciph_info.is_sync_ind_sent_to_nas[RRC_PS_DOMAIN_CN_ID]);
    
    /* RP-101213: CR 4427: Enhanced Security Mode procedure handling 
           Send L2 ACK only if no activation is pending for SRB3
       */
    if((rrcsmc_substate == RRCSMC_WAIT_FOR_L2_ACK) && (rb_id <=DCCH_DT_LOW_PRI_RADIO_BEARER_ID) &&
       (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
             dl_count_i_act_time_pending[rb_id] == FALSE))
    {
      /*Check: Can we hardcode DCCH_DT_HIGH_PRI_RADIO_BEARER_ID for DDT?? */
      rrclcm_internal_post_l2_ack_ind_for_proc(RRC_PROCEDURE_SMC, DCCH_AM_RADIO_BEARER_ID);
    }
    if(rrcsmc_int_ciph_info.is_sync_ind_sent_to_nas[cn_id] == TRUE)
    {
      WRRC_MSG1_HIGH("sync_ind for CN ID-%d sent once, so we can send DDT messages to NAS",cn_id);
      return FALSE;
    }
    else
    {
      rrcsmc_send_l2_ack_ind_to_ddt_flag=TRUE;
      return TRUE;
    }
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION rrcsmc_update_cphy_key_req

DESCRIPTION
  Copy the contents of locally stored keys into cipher global db.
  This function updates the keys stored locally into the keys to
  be used by L1.  This function is only called when Ciphering is
  START/RESTART, because keys are updated only at this time after
  an authentication is done.  This function is not to be called
  when Radio-Bearers are reconfigured.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void rrcsmc_update_cphy_key_req
(
  rrc_ciph_config_e_type config_cause
)
{
  uint32 key_id=0, i=0, j=0; /*local vars for count and key_id count*/

  /*set the L1 STRUCT's for UL and DL ciphering keys*/
  cipher_config_db.l1_cipher_info.cipher_key_req.req_mask |= L1_DL_CIPHER_KEY_REQ;
  cipher_config_db.l1_cipher_info.cipher_key_req.req_mask |= L1_UL_CIPHER_KEY_REQ;

  WRRC_MSG1_HIGH("Updating cphy_key_req with config_cause=%d",config_cause);
  if(config_cause == CIPH_CONFIG_UL_KEYS)
  {
    /*set the number of keys being given to l1 to 4 for UL,as we will be configuring 
      UL keys only.  Two keys per cn domain.2 current/old and 2 current/new
      (depending on the activation time)*/
    cipher_config_db.l1_cipher_info.cipher_key_req.ul_cipher_info.num_keys = 4;
    cipher_config_db.l1_cipher_info.cipher_key_req.dl_cipher_info.num_keys = 0;
  }
  else if(config_cause == CIPH_CONFIG_DL_KEYS)
  {
    /*set the number of keys being given to l1 to 4 for DL,as we will be configuring 
      DL keys only.  Two keys per cn domain.2 current/old and 2 current/new
      (depending on the activation time)*/
    cipher_config_db.l1_cipher_info.cipher_key_req.ul_cipher_info.num_keys = 0;
    cipher_config_db.l1_cipher_info.cipher_key_req.dl_cipher_info.num_keys = 4;
  }
  else
  {
    /*set the number of keys being given to l1 to 4.  Two keys per cn domain.
      2 current/old and 2 current/new(depending on the activation time)*/
    cipher_config_db.l1_cipher_info.cipher_key_req.ul_cipher_info.num_keys = 4;
    cipher_config_db.l1_cipher_info.cipher_key_req.dl_cipher_info.num_keys = 4;
  }
  /*Set the key_id's and key_algorithms and the key values in the global ciphering 
    database for uplink cipher keys. key_id values are from 0-3
    Note: UL and DL keys are the same, but L1 struct requires them in different
    struct variables     */

  /*ul key initialization*/
  if(config_cause != CIPH_CONFIG_DL_KEYS)
  {
    WRRC_MSG0_HIGH("Cipher ul key initialization");
    key_id=0; 
    for (i=0; i<RRC_MAX_CN_DOMAINS; i++)/*for cs and ps cn domain id's*/
    {
      for (j=0; j<MAX_CIPH_KEYS_PER_DOMAIN; j++)/*num of keys per domain*/
      {
        cipher_config_db.l1_cipher_info.cipher_key_req.ul_cipher_info.
        key_info[key_id].key_id = (uint8) key_id;
        cipher_config_db.l1_cipher_info.cipher_key_req.ul_cipher_info.
        key_info[key_id].algorithm = UE_WCDMA_CIPHER_ALGO_UEA1;

        WCDMA_MEMCPY(cipher_config_db.l1_cipher_info.cipher_key_req.ul_cipher_info.key_info[key_id].key_val, 
                     sizeof(uint8) * L1_NUM_BYTES_CIPHER_KEY,
                     rrcsmc_int_ciph_info.int_ciph_info[i].ciph_keys_info.key[j],
                     RRC_MAX_CIPHERING_KEY_LENGTH); 
        key_id++;      
      }
    }
  }
  /*dl key initialization*/
  if(config_cause != CIPH_CONFIG_UL_KEYS)
  {
    WRRC_MSG0_HIGH("Cipher dl key initialization");
    key_id=0; 
    for (i=0; i<RRC_MAX_CN_DOMAINS; i++)/*for cs and ps cn domain id's*/
    {
      for (j=0; j<MAX_CIPH_KEYS_PER_DOMAIN; j++)/*num of keys per domain*/
      {
        cipher_config_db.l1_cipher_info.cipher_key_req.dl_cipher_info.
        key_info[key_id].key_id = (uint8) key_id;
        cipher_config_db.l1_cipher_info.cipher_key_req.dl_cipher_info.
        key_info[key_id].algorithm = UE_WCDMA_CIPHER_ALGO_UEA1;
        WCDMA_MEMCPY(cipher_config_db.l1_cipher_info.cipher_key_req.dl_cipher_info.key_info[key_id].key_val,
                     sizeof(uint8) * L1_NUM_BYTES_CIPHER_KEY,
                     rrcsmc_int_ciph_info.int_ciph_info[i].ciph_keys_info.key[j],
                     RRC_MAX_CIPHERING_KEY_LENGTH);
        key_id++;
      }
    }
  }
}

/*===========================================================================

FUNCTION rrcsmc_revert_max_count_c

DESCRIPTION
  This function reverts the maximum count c stored in the temporary variables 
  to the older value, i.e, the value in permanent variable
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_revert_max_count_c
(
  void
)
{
  max_count_c.count_c_cs = rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_CS_DOMAIN_CN_ID];
  max_count_c.count_c_ps = rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_PS_DOMAIN_CN_ID];
}

/*===========================================================================

FUNCTION rrcsmc_update_count_c_released_rbs

DESCRIPTION
  This function updates the maximum count c stored in the temporary variables to the permanent one.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

void rrcsmc_update_count_c_released_rbs
(
void
)
{
  rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_CS_DOMAIN_CN_ID]=max_count_c.count_c_cs;
  rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_PS_DOMAIN_CN_ID]=max_count_c.count_c_ps;
}
/*===========================================================================

FUNCTION rrcsmc_get_max_start_per_domain

DESCRIPTION
  This function gets the max start value on a domain basis

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 rrcsmc_get_max_start_per_domain
(
rrc_cn_domain_identity_e_type domain_id
)
{
  uint32 i=0;                         /*local variables used for counting 
                                        in for loop*/

  uint32 max_start=0;
  
  /*rlc_hfn_info_type to calculate start_value_to_transmit*/
  rlc_hfn_req_type *local_hfn_type_ptr = (rlc_hfn_req_type *)rrc_malloc(sizeof(rlc_hfn_req_type));
  
  /*mac_hfn_info_type to calculate start_value_to_transmit*/
  mac_hfn_info_type mac_hfn;      
  
  /*local vars to store count_i, seq_num, START list used for 
    manipulations in this function only*/
  uint32 max_count_i_ul=0, max_count_i_dl=0, max_count_i = 0, max_start_i=0;  
  uint32 max_start_c_ul=0, max_start_c_dl=0, max_start_c=0, max_update=0;    
  uint32 local_lc_id;

  local_hfn_type_ptr->ul_nchan =0;
  local_hfn_type_ptr->dl_nchan =0;

  memset(local_hfn_type_ptr->hfn_per_rb_ul,0,((UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY) * sizeof(rlc_hfn_type)));

  memset(local_hfn_type_ptr->hfn_per_rb_dl,0,((UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY) * sizeof(rlc_hfn_type)));


  for ( i=0; i < RRC_MAX_SRB; i++)
  {
    if(integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
       dl_count_i_act_time_pending[i] != TRUE)
    {
      max_count_i_dl = 
        (max_count_i_dl < integrity_protection_info.count_i_info[domain_id].dl_count_i[i])?
        integrity_protection_info.count_i_info[domain_id].dl_count_i[i]:max_count_i_dl; 
    }
  }        

  /*get the max of count_i's for uplink by looping through ul count_i's*/    
  for ( i=0; i < RRC_MAX_SRB; i++)
  {
    if(integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
       ul_count_i_act_time_pending[i] != TRUE)
    {
      max_count_i_ul = 
        (max_count_i_ul < integrity_protection_info.count_i_info[domain_id].ul_count_i[i])?
        integrity_protection_info.count_i_info[domain_id].ul_count_i[i]:max_count_i_ul; 
    }
  }        

  /*get the max of ul or dl count_i for this domain*/    
  max_count_i = (max_count_i_ul > max_count_i_dl)? max_count_i_ul:max_count_i_dl;

  /*START is 20MSB of count_i which is 32bits.
  */
  max_start_i = max_count_i >> 12;
  
  if(rrcsmc_int_ciph_info.ciph_status[domain_id] == RRCSMC_SECURITY_STARTED)
  {
    /*get the latest RLC HFN's*/
    (void) rlc_rrc_get_hfn_req(local_hfn_type_ptr);

    for (i=0; i < local_hfn_type_ptr->ul_nchan; i++)
    {
      local_lc_id = local_hfn_type_ptr->hfn_per_rb_ul[i].rlc_id;
      /*check if lc_id of the Radio Bearer belongs to UM Mode*/           
      if (local_lc_id >= RRCLCM_UL_UM_START_IDX &&
          local_lc_id < RRCLCM_UL_AM_DATA_START_IDX)
      {
        /*since UM mode RB's HFN is 25 bits, remove the LSB 5 bits
          to get START value*/      
        local_hfn_type_ptr->hfn_per_rb_ul[i].hfn >>= 5;
      }
      
      if(domain_id == RRC_CS_DOMAIN_CN_ID)
      {
        if(rrcsmc_int_ciph_info.latest_config_cn_id == RRC_CS_DOMAIN_CN_ID)
        {
          switch(rrclcm_get_ul_rb_id((uint8)local_lc_id))
          {
          case DCCH_AM_RADIO_BEARER_ID:
          case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
          case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
          case DCCH_UM_RADIO_BEARER_ID:
            break;
            
          default:
                local_hfn_type_ptr->hfn_per_rb_ul[i].hfn = 0;
            break;
          }
        }
        else
        {
          local_hfn_type_ptr->hfn_per_rb_ul[i].hfn = 0;        
        }
      }
      else if(domain_id == RRC_PS_DOMAIN_CN_ID)
      {
        if(rrcsmc_int_ciph_info.latest_config_cn_id == RRC_CS_DOMAIN_CN_ID)
        {
          switch(rrclcm_get_ul_rb_id((uint8)local_lc_id))
          {
          case DCCH_AM_RADIO_BEARER_ID:
          case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
          case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
          case DCCH_UM_RADIO_BEARER_ID:
            local_hfn_type_ptr->hfn_per_rb_ul[i].hfn = 0;          
            break;
            
          default:
            break;
          }
        }
      }
    }
  
  
    for (i=0; i < local_hfn_type_ptr->dl_nchan; i++)
    {
      local_lc_id = local_hfn_type_ptr->hfn_per_rb_dl[i].rlc_id;
      /*check if lc_id of the Radio Bearer belongs to UM Mode*/           
      if (local_lc_id >= RRCLCM_DL_UM_START_IDX &&
          local_lc_id < RRCLCM_DL_AM_DATA_START_IDX)
      {
        /*since UM mode RB's HFN is 25 bits, remove the LSB 5 bits
          to get START value*/      
        local_hfn_type_ptr->hfn_per_rb_dl[i].hfn >>= 5;
      }
      
      if(domain_id == RRC_CS_DOMAIN_CN_ID)
      {
        if(rrcsmc_int_ciph_info.latest_config_cn_id == RRC_CS_DOMAIN_CN_ID)
        {
          switch(rrclcm_get_dl_rb_id((uint8)local_lc_id))

          {
          case DCCH_AM_RADIO_BEARER_ID:
          case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
          case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
          case DCCH_UM_RADIO_BEARER_ID:
            break;
          default:
                local_hfn_type_ptr->hfn_per_rb_dl[i].hfn = 0;
            break;
          }
        }
        else
        {
              local_hfn_type_ptr->hfn_per_rb_dl[i].hfn = 0;
        }
      }
      else if(domain_id == RRC_PS_DOMAIN_CN_ID)
      {
        if(rrcsmc_int_ciph_info.latest_config_cn_id == RRC_CS_DOMAIN_CN_ID)
        {
          switch(rrclcm_get_dl_rb_id((uint8)local_lc_id))
          {
          case DCCH_AM_RADIO_BEARER_ID:
          case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
          case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
          case DCCH_UM_RADIO_BEARER_ID:
            local_hfn_type_ptr->hfn_per_rb_dl[i].hfn = 0;          
            break;
            
          default:
            break;
          }
        }
      }      
    }

    if(local_hfn_type_ptr->ul_nchan > 0)
    {
      /*get the max of all the rlc UL HFN's*/
      max_start_c_ul = local_hfn_type_ptr->hfn_per_rb_ul[0].hfn;
    }
  
    for (i=1; ((i < local_hfn_type_ptr->ul_nchan) && (i < (UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY))); i++)
    {
      max_start_c_ul = (max_start_c_ul < local_hfn_type_ptr->hfn_per_rb_ul[i].hfn)?
                        local_hfn_type_ptr->hfn_per_rb_ul[i].hfn: max_start_c_ul;
    }
  
    if(local_hfn_type_ptr->dl_nchan > 0)
    {    
      /*get the max of all the rlc DL HFN's*/  
      max_start_c_dl = local_hfn_type_ptr->hfn_per_rb_dl[0].hfn;
    }

    for (i=1; ((i < local_hfn_type_ptr->dl_nchan) && (i < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY))); i++)
    {
      max_start_c_dl = (max_start_c_dl < local_hfn_type_ptr->hfn_per_rb_dl[i].hfn)?
                        local_hfn_type_ptr->hfn_per_rb_dl[i].hfn: max_start_c_dl;
    }
  
    /*select the greater and copy to max_start_c*/
    max_start_c = (max_start_c_dl > max_start_c_ul)? max_start_c_dl: max_start_c_ul;
  
    if((domain_id == RRC_CS_DOMAIN_CN_ID) && 
     (cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_enable == TRUE))
    {
      /*get the latest MAC HFN's*/
      (void) mac_rrc_get_hfn_info(&mac_hfn);

      /*since MAC HFN is 24 bit number, right shift by 4 to get START value
        from MAC COUNT-C*/
      mac_hfn.cs_hfn >>= 4;
      WRRC_MSG2_HIGH("MAC HFN CS : %x , MAC HFN PS : %x",mac_hfn.cs_hfn,mac_hfn.ps_hfn);
      
      max_start_c = (max_start_c < mac_hfn.cs_hfn)? mac_hfn.cs_hfn : max_start_c;

    }   
  }

  if(domain_id==RRC_CS_DOMAIN_CN_ID)
  {
    max_start_c = (max_start_c<rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_CS_DOMAIN_CN_ID])?
                 rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_CS_DOMAIN_CN_ID] : max_start_c;
    /*Reset the HFN value*/
    rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_CS_DOMAIN_CN_ID]=0;
    max_count_c.count_c_cs=0;
  }
  else
  {
    max_start_c = (max_start_c<rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_PS_DOMAIN_CN_ID])?
                 rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_PS_DOMAIN_CN_ID] : max_start_c;
    /*Reset the HFN value*/
    rrcsmc_int_ciph_info.max_count_c_rel_rbs[RRC_PS_DOMAIN_CN_ID]=0;
    max_count_c.count_c_ps=0;
  }
  
  max_start = (max_start_c > max_start_i)? max_start_c:max_start_i;
  
  if((extract_cause_rel_ind == TRUE) && 
     (rrcsmc_int_ciph_info.start_value_update_flag[domain_id] == TRUE))
  {
    max_update = rrcsmc_convert_START_to_count_c_i(
      rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest, 0);
    
    /*since COUNT-C is 32 bit number, right shift by 12 to get START value
      from COUNT-C*/
    max_update >>= 12;

    max_start = (max_start > max_update) ? max_start:max_update;
  }

    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"MAX START: 0x%x hfn_latest: 0x%x 0x%x 0x%x",
    max_start,
    rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest[0],
                                           rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest[1],
    rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest[2]
    );

  rrc_free(local_hfn_type_ptr);
  return max_start;
}
/*===========================================================================

FUNCTION rrcsmc_update_global_ciph_db_for_ciph_start

DESCRIPTION
  This function updates the global ciphering database when a reconfig
  message is received.  This function is called due to addition of
  RB's or deletion of RB's due to RBE or RBR procedures.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void rrcsmc_update_global_ciph_db_for_ciph_start
(
  rrc_cn_domain_identity_e_type latest_config_cn_domain,
  rrc_ciph_config_e_type config_cause
)
{
  uint32 i=0;

  /*copy the contents of locally stored keys into cipher global db*/
  rrcsmc_update_cphy_key_req(config_cause);

  /*update the key indices if incase authentication is done.*/
  rrcsmc_update_ciph_keys_used(latest_config_cn_domain);

#ifdef FEATURE_LTE_TO_WCDMA
  if(CIPH_CONFIG_CS_PLUS_PS == config_cause)
  {
    /*update the key indices to apply the PS mapped keys.
    latest_config_cn_domain will be CS for CS+PS SRVCC
    so PS mapped keys wont be updated. */
    rrcsmc_update_ciph_keys_used(RRC_PS_DOMAIN_CN_ID);
  }
#endif        

  /*reset nchan's for rlc to zero, as we only send incremental ciphering info
    for RLC RB's*/
  cipher_config_db.rlc_ul_cipher_info.nchan = 0;
  cipher_config_db.rlc_dl_cipher_info.nchan = 0;

  /* NOTE: there is no need to check for 'is_revert_to_old_cipher_variable_reqd' when we 
     use variables <like rrcsmc_int_ciph_info> which are already rolled back because
     immediately after this we are going to revert back ciphering related variable to old 
     configuration and whatever configuration we are going to send RLC will  be aborted, so 
     even if this configuration is slightly corrupted then also it is fine */

  /*loop through the number of AM radiobearers configured and update global
    database used by LLC to configure RLC with ciphering parameters*/
  for (i=0; i< ciph_rb_info.am_rb_ciph_config.num_rb; i++)
  {
    /*if domain id is same as domain id of latest configured cn domain only then
      update ciphering parameters*/
    if(
#ifdef FEATURE_LTE_TO_WCDMA
        (CIPH_CONFIG_CS_PLUS_PS == config_cause)||
#endif        
        (latest_config_cn_domain == 
        ciph_rb_info.am_rb_ciph_config.config_info[i].domain_id))
    {
      /*set HFN*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].hfn = 
      rrcsmc_convert_START_to_count_c_i(
          rrcsmc_int_ciph_info.int_ciph_info[ciph_rb_info.am_rb_ciph_config.config_info[i].domain_id].hfn, 0);

      /*change HFN offset for RLC AM*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].hfn >>= RRC_RLC_AM_HFN_OFFSET;

      /*set rb_id and lc_id*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].rb_id = 
      ciph_rb_info.am_rb_ciph_config.config_info[i].rb_id;

      cipher_config_db.rlc_ul_cipher_info.ciphering[i].lc_id = 
      rrclcm_get_ul_rlc_lc_id(ciph_rb_info.am_rb_ciph_config.config_info[i].rb_id);

      /*set ciphering activation time*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_act_sn = 
      ciphering_info.ul_ciph_act_info.sq_num[cipher_config_db.rlc_ul_cipher_info.
      ciphering[i].lc_id];

      /*set cipher mode to true*/
        cipher_config_db.rlc_ul_cipher_info.ciphering[i].cipher_mode = TRUE;
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_algo = 
          rrcsmc_int_ciph_info.saved_ciph_algo[ciph_rb_info.am_rb_ciph_config.config_info[i].domain_id];

      /*set key index*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].cipher_key_id = 
      rrcsmc_get_ciph_key_idx(ciph_rb_info.am_rb_ciph_config.config_info[i].domain_id);	

      /*set flag to ciph_started*/
      if (ciph_rb_info.am_rb_ciph_config.config_info[i].rb_status == CIPH_FIRST_TIME)
      {
        ciph_rb_info.am_rb_ciph_config.config_info[i].rb_status=CIPH_STARTED;
      }

      /*increment the number of channels by 1*/
      cipher_config_db.rlc_ul_cipher_info.nchan++;
    }
  }

  /*loop through the number of UM radiobearers configured and update global
    database used by LLC to configure RLC with ciphering parameters*/
  for (i=0; i < ciph_rb_info.um_rb_ciph_config.num_rb; i++)
  {
    /*if domain id is same as domain id of latest configured cn domain only then
      update ciphering parameters*/
    if (
#ifdef FEATURE_LTE_TO_WCDMA			
        (CIPH_CONFIG_CS_PLUS_PS == config_cause) ||
#endif        
        (latest_config_cn_domain == 
        ciph_rb_info.um_rb_ciph_config.config_info[i].domain_id))
    {
      /*set HFN*/
      if(cipher_config_db.rlc_ul_cipher_info.nchan < (UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY))
      {
        cipher_config_db.rlc_ul_cipher_info.ciphering[cipher_config_db.
        rlc_ul_cipher_info.nchan].hfn = 
        rrcsmc_convert_START_to_count_c_i(
          rrcsmc_int_ciph_info.int_ciph_info[ciph_rb_info.um_rb_ciph_config.config_info[i].domain_id].hfn, 0);

        /*change HFN offset for RLC UM rb*/
        cipher_config_db.rlc_ul_cipher_info.ciphering[cipher_config_db.
        rlc_ul_cipher_info.nchan].hfn >>= RRC_RLC_UM_HFN_OFFSET;

        /*set rb_id and lc_id*/
        cipher_config_db.rlc_ul_cipher_info.ciphering[cipher_config_db.
        rlc_ul_cipher_info.nchan].rb_id = 
        ciph_rb_info.um_rb_ciph_config.config_info[i].rb_id;

        cipher_config_db.rlc_ul_cipher_info.ciphering[cipher_config_db.rlc_ul_cipher_info.
        nchan].lc_id = 
        rrclcm_get_ul_rlc_lc_id(ciph_rb_info.um_rb_ciph_config.config_info[i].rb_id);

        /*set ciphering activation sequence number*/
        cipher_config_db.rlc_ul_cipher_info.ciphering[cipher_config_db.
        rlc_ul_cipher_info.nchan].ciph_act_sn = 
        ciphering_info.ul_ciph_act_info.sq_num[cipher_config_db.rlc_ul_cipher_info.
          ciphering[cipher_config_db.rlc_ul_cipher_info.nchan].lc_id];

        /*set cipher mode to true*/
        cipher_config_db.rlc_ul_cipher_info.ciphering[cipher_config_db.
        rlc_ul_cipher_info.nchan].cipher_mode = TRUE;
        cipher_config_db.rlc_ul_cipher_info.ciphering[cipher_config_db.
          rlc_ul_cipher_info.nchan].ciph_algo = 
            rrcsmc_int_ciph_info.saved_ciph_algo[ciph_rb_info.um_rb_ciph_config.config_info[i].domain_id];

        /*set key index*/
        cipher_config_db.rlc_ul_cipher_info.ciphering[cipher_config_db.
        rlc_ul_cipher_info.nchan].cipher_key_id = 
        rrcsmc_get_ciph_key_idx(ciph_rb_info.um_rb_ciph_config.config_info[i].domain_id);
      }
      else
      {
        ERR_FATAL("Invalid number of channels received",0,0,0);
      }

      /*set flag to ciph_started*/
      if (ciph_rb_info.um_rb_ciph_config.config_info[i].rb_status == CIPH_FIRST_TIME)
      {
        ciph_rb_info.um_rb_ciph_config.config_info[i].rb_status=CIPH_STARTED;
      }

      /*increment number of channels*/
      cipher_config_db.rlc_ul_cipher_info.nchan++;
    }
  }

  /*if affected status of TM RB's for the latest CN domain is true*/
  if (ciph_rb_info.tm_rb_ciph_config[latest_config_cn_domain].
      affected == TRUE)
  {
    /*set cipher mode to true*/
      cipher_config_db.mac_cipher_info[latest_config_cn_domain].cipher_algo = 
           rrcsmc_int_ciph_info.saved_ciph_algo[latest_config_cn_domain];
    
    /* The config type should be set to SMC_INITIATED_NON_HO_CIPHER_CFG only for the 
       following scenarios:
       1. Ciphering change for TM bearers due to SMC that would result in a second
       pending config for MAC.i.e MAC already has a pending configuration(due to 
       Step-2) and before that Act time is reached there is a SMC that would result 
       in a second pending config.To determine whether something is already pending in
       MAC,use the API provided by MAC   
       2. When we already have TM bearers established and this SMC is starting ciphering
       on TM bearers for the first time.

       The config type should NOT be set to SMC_INITIATED_NON_HO_CIPHER_CFG for the 
       following scenarios:For these scenarios,the config type is already set 
       appropriately before we reach this point
       1. Ciphering start during a GTOW Handover
       2. TM bearers are established and are already ciphered.There is no pending config
       in MAC.Now we get a SMC that modifies the ciphering configuration for TM bearers.
    */
    if((rrcsmc_substate == RRCSMC_WAIT_FOR_CIPH_CONFIG_CNF) &&
       (TRUE == mac_rrc_get_cipher_pending_status()))
    {
      WRRC_MSG0_HIGH("Sending SMC_INITIATED_NON_HO_CIPHER_CFG to MAC (Second Pending)");
      cipher_config_db.mac_cipher_info[latest_config_cn_domain].cipher_cfg_type = 
        SMC_INITIATED_NON_HO_CIPHER_CFG;
    }

    /* if 'cipher_enable' is FALSE then it means that ciphering(UEA0 or UEA1) is 
       going to start in TM due to this SMC i.e. during the on-going RRC connection
       this is first time UE received SMC with 'ciphering mode info'. Hence 
       indicate MAC with config_type set to 'SMC_INITIATED_NON_HO_CIPHER_CFG' 
    */ 
    if (FALSE == cipher_config_db.mac_cipher_info[latest_config_cn_domain].cipher_enable)
    {
       WRRC_MSG0_HIGH("Cipher Mode Info received in SMC for the first time during this rrc connection.");
       cipher_config_db.mac_cipher_info[latest_config_cn_domain].cipher_cfg_type = 
         SMC_INITIATED_NON_HO_CIPHER_CFG;
    }

    cipher_config_db.mac_cipher_info[latest_config_cn_domain].cipher_enable = TRUE;

    /*check if dpch ciph activation time is present or not and flag error*/
    if (saved_security_mode_command_msg.dpch_ciph_act_time_present)
    {
      cipher_config_db.mac_cipher_info[latest_config_cn_domain].
      ciph_act_time = saved_security_mode_command_msg.dpch_ciph_act_time; 
    }
    else
    {
       WRRC_MSG0_ERROR("SMC did not contain DPCH info");     
    }

    /*set HFN and offset with MAC requirement*/
    cipher_config_db.mac_cipher_info[latest_config_cn_domain].hfn = 
    rrcsmc_convert_START_to_count_c_i(
      rrcsmc_int_ciph_info.int_ciph_info[latest_config_cn_domain].hfn, 0);     

    cipher_config_db.mac_cipher_info[latest_config_cn_domain].hfn >>= 
    RRC_MAC_HFN_OFFSET;

    /*set key index*/
    cipher_config_db.mac_cipher_info[latest_config_cn_domain].key_idx =
    rrcsmc_get_ciph_key_idx(latest_config_cn_domain);

    //set this flag to indicate that mac's ciphering configuration is not affected.
    //configuration to mac should not be sent by LLC if this flag is FALSE.
    cipher_config_db.mac_ciph_config_affected = TRUE;

  }
  else
  {
    //set this flag to indicate that mac's ciphering configuration is not affected.
    //configuration to mac should not be sent by LLC if this flag is FALSE.
    cipher_config_db.mac_ciph_config_affected = FALSE;
  }

  /*copy the uplink information to dl information*/
  cipher_config_db.rlc_dl_cipher_info.nchan = 
  cipher_config_db.rlc_ul_cipher_info.nchan;

  for (i=0; i<cipher_config_db.rlc_ul_cipher_info.nchan; i++)
  {
    /*set key id, hfn, cipher_mode, rb_id and activation time for UM/AM*/
    cipher_config_db.rlc_dl_cipher_info.ciphering[i].cipher_key_id = 
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].cipher_key_id;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].cipher_mode =
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].cipher_mode;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].hfn =
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].hfn;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].rb_id = 
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].rb_id;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].ciph_algo = 
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_algo;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].lc_id =
    rrclcm_get_dl_rlc_lc_id(cipher_config_db.rlc_dl_cipher_info.
                            ciphering[i].rb_id);

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].ciph_act_sn =
    ciphering_info.dl_ciph_act_info.sq_num_for_complete
    [cipher_config_db.rlc_ul_cipher_info.ciphering[i].rb_id];
  }
}
/*===========================================================================

FUNCTION rrcsmc_update_global_ciph_db_for_reconfig

DESCRIPTION
  This function updates the global ciphering database when a reconfig
  message is received.  This function is called due to addition of
  RB's or deletion of RB's due to RBE or RBR procedures.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void rrcsmc_update_global_ciph_db_for_reconfig
(
void
)
{
  /*local vars to maintain count*/
  uint32 i;
  uint32 num_ul_rb_added;
  boolean is_start_val_reached_threshold = FALSE;

  /*reset nchan's for rlc to zero, as we only send incremental ciphering info
    for RLC RB's*/
  cipher_config_db.rlc_ul_cipher_info.nchan = 0;
  cipher_config_db.rlc_dl_cipher_info.nchan = 0;

  start_value_to_transmit = rrcsmc_get_max_start_per_domain(start_value_to_transmit_domain);
  if(START_THRESHOLD == start_value_to_transmit)
  {
    //don't increment the start value, ref TS25.331 ch 8.5.9
    WRRC_MSG1_HIGH("Max Start Value=0x%x reached the Threshold",start_value_to_transmit);
    is_start_val_reached_threshold = TRUE;
  }
  else
  {
    /*increment by 2*/
    start_value_to_transmit = start_value_to_transmit+2;    
  }
  start_value_to_transmit = (start_value_to_transmit << 12);

  //only if tm bearers are being setup, update mac_start_value_to_transmit, as MAC is interesed only
  //in TM bearer start value.. use regular start_value_to_transmit for all other SRB's or RAB's which
  //are committed by RRC itself and not used by MAC separately..
  if(start_value_to_transmit_domain == RRC_CS_DOMAIN_CN_ID)
  {
    mac_start_value_to_transmit = start_value_to_transmit;
    /* Implementation of Spec CR 2284R1. When TM bearers are getting established and if the same 
       message (RBSetup) is also used to perform a Timing Re-initialized HHO, then at activation time
       the HFN component of Count-C should be incremented by 1. In our implementation, for the first 
       time TM setup, MAC is given a NON_HO_CIPHER_CFG and at activation time, MAC queries RRC for 
       the new HFN. RRC will return 'mac_start_value_to_transmit' which is populated at this point.
       Hence, check for HHO condition and if it is true, increment the value by 1 here itself so that
       later the the correct value will be given to MAC in the query function. Make sure the increment
       is done for "Timing re-initialised HHO" cases only i.e. only for DCH->DCH scenarios */
    if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) && 
       (rrcllc_timing_init_in_progress()) &&
       (RRC_STATE_CELL_DCH == rrc_get_state()) &&
       (OC_SET_FOR_CELL_DCH == rrcllc_get_ordered_config_status()) &&
        !(is_start_val_reached_threshold))
    {
      mac_start_value_to_transmit = mac_start_value_to_transmit >> RRC_MAC_HFN_OFFSET;
      mac_start_value_to_transmit += 1;
      mac_start_value_to_transmit = mac_start_value_to_transmit << RRC_MAC_HFN_OFFSET;

      WRRC_MSG1_HIGH("Incremented mac_start_value_to_transmit by 1 for TI HHO condition. Mac Start Value 0x%x",mac_start_value_to_transmit);
    }
  }

  for (i=0; i< ciph_rb_info.am_rb_ciph_config.num_rb; i++)
  {
    /*only if starting ciphering for this RB, we should set ciphering parameters.
      for RLC we only give ciphering information for RB's for which ciphering is
      being STARTED or RESTARTED.*/
    if ((ciph_rb_info.am_rb_ciph_config.config_info[i].rb_status == CIPH_FIRST_TIME) &&
        (rrcsmc_int_ciph_info.ciph_status[ciph_rb_info.am_rb_ciph_config.config_info[i].domain_id] 
         == RRCSMC_SECURITY_STARTED))
    {
      /*set rb id and lc_id*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].rb_id = 
      ciph_rb_info.am_rb_ciph_config.config_info[i].rb_id;

      cipher_config_db.rlc_ul_cipher_info.ciphering[i].lc_id = 
      rrclcm_get_ul_rlc_lc_id(ciph_rb_info.am_rb_ciph_config.config_info[i].rb_id);

      /*we will start ciphering immediately for rlc rb's, so set act sequence
        number to zero*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_act_sn = 0;

      /*set cipher mode to true*/
        cipher_config_db.rlc_ul_cipher_info.ciphering[i].cipher_mode = TRUE;
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_algo = 
          rrcsmc_int_ciph_info.saved_ciph_algo[ciph_rb_info.am_rb_ciph_config.config_info[i].domain_id];

      /*set HFN and offset for AM RB's*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].hfn = 
      (start_value_to_transmit >> RRC_RLC_AM_HFN_OFFSET);

      /*if SRB get cipher key id of latest configured cn domain, otherwise
        set key id of the domain for which this RB belongs*/
      if (ciph_rb_info.am_rb_ciph_config.config_info[i].is_rb_srb == TRUE)
      {
        cipher_config_db.rlc_ul_cipher_info.ciphering[i].cipher_key_id = 
        rrcsmc_get_ciph_key_idx(rrcsmc_int_ciph_info.latest_config_cn_id);
      }
      else
      {
        cipher_config_db.rlc_ul_cipher_info.ciphering[i].cipher_key_id = 
        rrcsmc_get_ciph_key_idx(ciph_rb_info.am_rb_ciph_config.config_info[i].domain_id);
      }

      /*set ciphering status to STARTED and increment number of channels*/
      ciph_rb_info.am_rb_ciph_config.config_info[i].rb_status = CIPH_STARTED;
      cipher_config_db.rlc_ul_cipher_info.nchan++;
    }
  }

  num_ul_rb_added = cipher_config_db.rlc_ul_cipher_info.nchan;

  for (i=0; i<ciph_rb_info.um_rb_ciph_config.num_rb; i++,num_ul_rb_added++)
  {

    /*only if starting ciphering for this RB, we should set ciphering parameters.
      for RLC we only give ciphering information for RB's for which ciphering is
      being STARTED or RESTARTED.*/
    if ((ciph_rb_info.um_rb_ciph_config.config_info[i].rb_status == CIPH_FIRST_TIME) &&
        (rrcsmc_int_ciph_info.ciph_status[ciph_rb_info.um_rb_ciph_config.config_info[i].domain_id] 
         == RRCSMC_SECURITY_STARTED))    
    {
      /*set rb_id and lc_id*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[num_ul_rb_added].rb_id = 
      ciph_rb_info.um_rb_ciph_config.config_info[i].rb_id;
      cipher_config_db.rlc_ul_cipher_info.ciphering[num_ul_rb_added].lc_id = 
      rrclcm_get_ul_rlc_lc_id(ciph_rb_info.um_rb_ciph_config.config_info[i].rb_id);

      /*we will start ciphering immediately for rlc rb's, so set act sequence
            * number to zero*/
      cipher_config_db.rlc_ul_cipher_info.ciphering[num_ul_rb_added].ciph_act_sn = 0;

      /*set cipher mode to true*/
        cipher_config_db.rlc_ul_cipher_info.ciphering[num_ul_rb_added].cipher_mode = TRUE;
      cipher_config_db.rlc_ul_cipher_info.ciphering[num_ul_rb_added].ciph_algo = 
            rrcsmc_int_ciph_info.saved_ciph_algo[ciph_rb_info.um_rb_ciph_config.config_info[i].domain_id];

      /* Set HFN and offset form UM RB's*/

      /* Legacy Code */
      cipher_config_db.rlc_ul_cipher_info.ciphering[num_ul_rb_added].hfn = 
      (start_value_to_transmit >> RRC_RLC_UM_HFN_OFFSET);
 

      /*if SRB get cipher key id of latest configured cn domain, otherwise
        set key id of the domain for which this RB belongs*/
      if (ciph_rb_info.um_rb_ciph_config.config_info[i].is_rb_srb == TRUE)
      {
        cipher_config_db.rlc_ul_cipher_info.ciphering[num_ul_rb_added].cipher_key_id = 
        rrcsmc_get_ciph_key_idx(rrcsmc_int_ciph_info.latest_config_cn_id);
      }
      else
      {
        cipher_config_db.rlc_ul_cipher_info.ciphering[num_ul_rb_added].cipher_key_id = 
        rrcsmc_get_ciph_key_idx(ciph_rb_info.um_rb_ciph_config.config_info[i].domain_id);
      }

      /*set ciphering status to STARTED and increment number of channels*/
      ciph_rb_info.um_rb_ciph_config.config_info[i].rb_status=CIPH_STARTED;
      cipher_config_db.rlc_ul_cipher_info.nchan++;
    }
  }

  /*set the global database for TM radio bearers*/
  for (i=0; i < RRC_MAX_CN_DOMAINS; i++)
  {
    /*only if the ciphering status is affected*/
    if ((ciph_rb_info.tm_rb_ciph_config[i].affected == TRUE) &&
        (rrcsmc_int_ciph_info.ciph_status[i] == RRCSMC_SECURITY_STARTED))
    {
      /*set ciphering enable flag to true*/
        cipher_config_db.mac_cipher_info[i].cipher_enable = TRUE;
      cipher_config_db.mac_cipher_info[i].cipher_algo = 
               rrcsmc_int_ciph_info.saved_ciph_algo[i];

      /*set tm ciphering activation time*/
      cipher_config_db.mac_cipher_info[i].ciph_act_time = 
      tm_ciph_act_cfn;

      /*set hfn to latest HFN sent in the last OTA message*/
      //if start value is updated, then send start value from hfn_latest
      if(rrcsmc_int_ciph_info.start_value_update_flag[i] == TRUE)
      {
        
        WRRC_MSG0_ERROR("Start-increment btwn SMC and RBS???");

        /*copy the contents of hfn_latest into mac ciphering database*/
        cipher_config_db.mac_cipher_info[i].hfn = 
          rrcsmc_convert_START_to_count_c_i(
            rrcsmc_int_ciph_info.int_ciph_info[i].hfn_latest, 0);
      }
      else
      {
        /*copy the contents of hfn into mac ciphering database*/
        cipher_config_db.mac_cipher_info[i].hfn = 
          rrcsmc_convert_START_to_count_c_i(
            rrcsmc_int_ciph_info.int_ciph_info[i].hfn, 0);
      }

      /*offset HFN for TM rb*/
      cipher_config_db.mac_cipher_info[i].hfn = 
      (cipher_config_db.mac_cipher_info[i].hfn >> RRC_MAC_HFN_OFFSET);

      /*get key index*/
      cipher_config_db.mac_cipher_info[i].key_idx =
      rrcsmc_get_ciph_key_idx((rrc_cn_domain_identity_e_type) i);
    }
  }

  /*set the number of channels in downlink*/
  cipher_config_db.rlc_dl_cipher_info.nchan = 
  cipher_config_db.rlc_ul_cipher_info.nchan;

  /*populate ciphering config db for dl database too, convert UL ciphering
    config information to DL ciphering config information*/
  for (i=0; i<cipher_config_db.rlc_ul_cipher_info.nchan; i++)
  {
    cipher_config_db.rlc_dl_cipher_info.ciphering[i].cipher_key_id = 
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].cipher_key_id;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].cipher_mode =
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].cipher_mode;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].hfn =
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].hfn;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].rb_id = 
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].rb_id;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].ciph_algo = 
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_algo;

    cipher_config_db.rlc_dl_cipher_info.ciphering[i].lc_id =
    rrclcm_get_dl_rlc_lc_id(cipher_config_db.rlc_dl_cipher_info.
                            ciphering[i].rb_id);

    /*set ciph act sn for dl to be same as UL, as we do not receive
      any ciphering activation time for RLC radio bearers in a 
      reconfig message*/
    cipher_config_db.rlc_dl_cipher_info.ciphering[i].ciph_act_sn = 
    cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_act_sn;
  }
}
/*===========================================================================

FUNCTION rrcsmc_update_sync_ind_info_for_signalling_release

DESCRIPTION
  Function to update ciphering database if signalling connection release is
  received.  This cleans up the sync indication variable .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_sync_ind_info_for_signalling_release
( 
  rrc_cn_domain_identity_e_type domain_id 
)
{
    rrcsmc_int_ciph_info.is_sync_ind_sent_to_nas[domain_id] = FALSE;
    WRRC_MSG1_HIGH("SYNC IND being initialized for domain id %d ",domain_id);
}
/*===========================================================================

FUNCTION rrcsmc_update_ciphering_config_info_for_signalling_release

DESCRIPTION
  Function to update ciphering database if signalling connection release is
  received.  This cleans up the ciphering database, as RB-Release will/may
  not be released in this case.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_ciphering_config_info_for_signalling_release
( 
  rrc_cn_domain_identity_e_type domain_id 
)
{
  /*reset the tm ciphering info*/
  ciph_rb_info.tm_rb_ciph_config[domain_id].affected = FALSE;
  ciph_rb_info.tm_rb_ciph_config[domain_id].num_rb = 0;
  ciph_rb_info.tm_rb_ciph_config[domain_id].rb_status = CIPH_NOT_STARTED;
  
  /* set ciphering enabled flag to false */
  cipher_config_db.mac_cipher_info[domain_id].cipher_enable = FALSE;
  
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_cfg_type = 
  NON_HO_CIPHER_CFG;  
}

/*===========================================================================

FUNCTION rrcsmc_calc_max_count_c_released_rb

DESCRIPTION
  This function calculates and stores the maximum count_c values for all released rbs for both domains in a temporary variable.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_calc_max_count_c_released_rb
(
  rrcllc_rlc_config_req_list_type *rlc_req_type_ptr,
  rrc_cn_domain_identity_e_type  domain_id_list[MAX_RB],
  rrc_proc_e_type proc_id
)
{
  uint32 max_count_c_cs=0;
  uint32 max_count_c_ps=0;
  uint32 i=0;
  rrc_RB_Identity local_rb_id;
  uint32 local_hfn_per_ul_rb[MAX_RB],local_hfn_per_dl_rb[MAX_RB];

  uecomdef_logch_mode_e_type local_mode;
  rlc_lc_id_type local_lc_id;
  
  /*rlc_hfn_info_type to calculate start_value_to_transmit*/
  rlc_hfn_req_type *local_hfn_type_ptr = (rlc_hfn_req_type *)rrc_malloc(sizeof(rlc_hfn_req_type));
  
  /*mac_hfn_info_type to calculate start_value_to_transmit*/
  mac_hfn_info_type mac_hfn;      
  
  rrc_cn_domain_identity_e_type local_cn_domain_id;
  
  local_hfn_type_ptr->ul_nchan =0;
  local_hfn_type_ptr->dl_nchan =0;

  memset(local_hfn_type_ptr->hfn_per_rb_ul,0,((UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY) * sizeof(rlc_hfn_type)));

  memset(local_hfn_type_ptr->hfn_per_rb_dl,0,((UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY) * sizeof(rlc_hfn_type)));

  (void) rlc_rrc_get_hfn_req(local_hfn_type_ptr);
  
    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Max count c before releasing RB for cs domain = %x ps domain = %x  ul_nchan: %d, dl_nchan: %d",
    max_count_c.count_c_cs,max_count_c.count_c_ps,
    local_hfn_type_ptr->ul_nchan,local_hfn_type_ptr->dl_nchan
   );
   

  /*store the hfn values for each dl UM/AM RBs*/
  for(i=0;i<MAX_RB;i++)
  {
    local_hfn_per_dl_rb[i] = 0;
    local_hfn_per_ul_rb[i] = 0;
  }
  

  for(i=0;i<local_hfn_type_ptr->dl_nchan;i++)
  {
    local_lc_id=local_hfn_type_ptr->hfn_per_rb_dl[i].rlc_id;
    local_rb_id=rrclcm_get_dl_rb_id(local_lc_id);
    
    /* Processing SRB's first skipping SRB0 */
    switch(local_rb_id)
    {
      case CCCH_RADIO_BEARER_ID:
        local_hfn_per_dl_rb[local_rb_id]=0;
        break;
      case DCCH_AM_RADIO_BEARER_ID:
      case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
      case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
      case DCCH_UM_RADIO_BEARER_ID:
        local_hfn_per_dl_rb[local_rb_id]=local_hfn_type_ptr->hfn_per_rb_dl[i].hfn;
        break;
      default:
      {
        if(local_rb_id < MAX_RB)
        {
          /* Now processing RB's */
          if(rrc_find_cn_domain_id_for_rb_in_est_rabs(local_rb_id, &local_cn_domain_id) == TRUE)
          {
            if(rrcsmc_int_ciph_info.ciph_status[local_cn_domain_id] == RRCSMC_SECURITY_STARTED)
             {
               local_hfn_per_dl_rb[local_rb_id]=local_hfn_type_ptr->hfn_per_rb_dl[i].hfn;
             }
             else
             {
               local_hfn_per_dl_rb[local_rb_id]=0;
             }
          }
          else
          {
            local_hfn_per_dl_rb[local_rb_id]=0;
          }
          if(rrclcm_get_dl_rlc_mode(local_lc_id, &local_mode)==RRCLCM_SUCCESS)
          {
            if(local_mode==UE_MODE_UNACKNOWLEDGED)
            {
              local_hfn_per_dl_rb[local_rb_id]=local_hfn_per_dl_rb[local_rb_id]>>RRC_RLC_UM_START_OFFSET;
            }
          }
        }
      }
    }
  }

  /*store the hfn values for each ul UM/AM RBs*/
  for(i=0;i<local_hfn_type_ptr->ul_nchan;i++)
  {
    local_lc_id=local_hfn_type_ptr->hfn_per_rb_ul[i].rlc_id;
    local_rb_id=rrclcm_get_ul_rb_id(local_lc_id);
    
    /* Processing SRB's first */
    switch(local_rb_id)
    {
     case CCCH_RADIO_BEARER_ID:
         local_hfn_per_dl_rb[local_rb_id]=0;
         break;
      case DCCH_AM_RADIO_BEARER_ID:
      case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
      case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
      case DCCH_UM_RADIO_BEARER_ID:
        local_hfn_per_ul_rb[local_rb_id]=local_hfn_type_ptr->hfn_per_rb_ul[i].hfn;
        break;
      default:
      {
        if(local_rb_id < MAX_RB)
        {
             /* Now processing RB's */
          if(rrc_find_cn_domain_id_for_rb_in_est_rabs(local_rb_id, &local_cn_domain_id) == TRUE)
          {
            if(rrcsmc_int_ciph_info.ciph_status[local_cn_domain_id] == RRCSMC_SECURITY_STARTED)
            {
              local_hfn_per_ul_rb[local_rb_id]=local_hfn_type_ptr->hfn_per_rb_ul[i].hfn;
            }
            else
            {
              local_hfn_per_ul_rb[local_rb_id]=0;
            }
          }
          else
          {
            local_hfn_per_ul_rb[local_rb_id]=0;
          }
          if(rrclcm_get_ul_rlc_mode(local_lc_id, &local_mode)==RRCLCM_SUCCESS)
          {
            if(local_mode==UE_MODE_UNACKNOWLEDGED)
            {
              local_hfn_per_ul_rb[local_rb_id]=local_hfn_per_ul_rb[local_rb_id]>>RRC_RLC_UM_START_OFFSET;
            }
          }
        } 
      }
    }
  }

  /*Get the hfn values for TM RBs*/

  (void) mac_rrc_get_hfn_info(&mac_hfn);

  mac_hfn.cs_hfn=mac_hfn.cs_hfn>>RRC_RLC_TM_START_OFFSET;
  mac_hfn.ps_hfn=mac_hfn.ps_hfn>>RRC_RLC_TM_START_OFFSET;

  WRRC_MSG2_HIGH("MAC HFN CS : %x , MAC HFN PS : %x",mac_hfn.cs_hfn,mac_hfn.ps_hfn);


  if(proc_id == RRC_PROCEDURE_RBE || proc_id == RRC_PROCEDURE_RBR || proc_id == RRC_PROCEDURE_CU)
  {
    /*Find the maximum HFN for UL TM RBs to be released*/
    for(i=0;i<rlc_req_type_ptr->ul_tm.nchan;i++)
    {
      local_rb_id = rrclcm_get_ul_rb_id(rlc_req_type_ptr->ul_tm.chan_info[i].lc_id);
      /* Processing SRB's first skipping SRB0*/
      switch(local_rb_id)
      {
        case CCCH_RADIO_BEARER_ID:
          break;
        default:
        {
          if((local_rb_id < MAX_RB) && (rlc_req_type_ptr->ul_tm.ul_tm_act[i].action==RLC_RELEASE) &&
            (
              rlc_req_type_ptr->ul_tm.chan_info[i].lc_type == UE_LOGCHAN_DCCH ||
              rlc_req_type_ptr->ul_tm.chan_info[i].lc_type == UE_LOGCHAN_DTCH 
            ))
          {
            if((domain_id_list[local_rb_id]==RRC_CS_DOMAIN_CN_ID) &&
               (rrcsmc_int_ciph_info.ciph_status[RRC_CS_DOMAIN_CN_ID] == RRCSMC_SECURITY_STARTED))
            {
              max_count_c_cs=(max_count_c_cs<mac_hfn.cs_hfn)?mac_hfn.cs_hfn:max_count_c_cs;
            }
            else if((domain_id_list[local_rb_id]==RRC_PS_DOMAIN_CN_ID) &&
                    (rrcsmc_int_ciph_info.ciph_status[RRC_PS_DOMAIN_CN_ID] == RRCSMC_SECURITY_STARTED))
            {
              max_count_c_ps=(max_count_c_ps<mac_hfn.ps_hfn)?mac_hfn.ps_hfn:max_count_c_ps;
            }
          }
        }
      }
    }
    /*Find the maximum HFN for DL TM RBs to be released*/
    for(i=0;i<rlc_req_type_ptr->dl_tm.nchan;i++)
    {
      local_rb_id = rrclcm_get_dl_rb_id(rlc_req_type_ptr->dl_tm.chan_info[i].lc_id);
      /* Processing SRB's first skipping SRB0*/
      switch(local_rb_id)
      {
        case CCCH_RADIO_BEARER_ID:
        default:
        {
          if((local_rb_id < MAX_RB) && (rlc_req_type_ptr->dl_tm.dl_tm_act[i].action==RLC_RELEASE) &&
          (
              rlc_req_type_ptr->dl_tm.chan_info[i].lc_type == UE_LOGCHAN_DCCH ||
              rlc_req_type_ptr->dl_tm.chan_info[i].lc_type == UE_LOGCHAN_DTCH 
               ||rlc_req_type_ptr->dl_tm.chan_info[i].lc_type == UE_LOGCHAN_TM_DCCH_DL
          ))
          {
            if((domain_id_list[local_rb_id]==RRC_CS_DOMAIN_CN_ID) &&
                 (rrcsmc_int_ciph_info.ciph_status[RRC_CS_DOMAIN_CN_ID] == RRCSMC_SECURITY_STARTED))
            {
              max_count_c_cs=(max_count_c_cs<mac_hfn.cs_hfn)?mac_hfn.cs_hfn:max_count_c_cs;
            }
            else if((domain_id_list[local_rb_id]==RRC_PS_DOMAIN_CN_ID) &&
                      (rrcsmc_int_ciph_info.ciph_status[RRC_PS_DOMAIN_CN_ID] == RRCSMC_SECURITY_STARTED))
            {
              max_count_c_ps=(max_count_c_ps<mac_hfn.ps_hfn)?mac_hfn.ps_hfn:max_count_c_ps;
            }
          }
        }
      }
    }
    /*Find the maximum HFN for UL UM RBs to be released*/
    for(i=0;i<rlc_req_type_ptr->ul_um.nchan;i++)
    {
      local_rb_id=rrclcm_get_ul_rb_id(rlc_req_type_ptr->ul_um.chan_info[i].lc_id);
      if((local_rb_id < MAX_RB) && (rlc_req_type_ptr->ul_um.ul_um_act[i].action==RLC_RELEASE))
      {
        if((domain_id_list[local_rb_id]==RRC_CS_DOMAIN_CN_ID) &&
           (rrcsmc_int_ciph_info.ciph_status[RRC_CS_DOMAIN_CN_ID] == RRCSMC_SECURITY_STARTED))
        {
          max_count_c_cs=(max_count_c_cs<local_hfn_per_ul_rb[local_rb_id])?local_hfn_per_ul_rb[local_rb_id]:max_count_c_cs;
        }
        else if((domain_id_list[local_rb_id]==RRC_PS_DOMAIN_CN_ID) &&
                (rrcsmc_int_ciph_info.ciph_status[RRC_PS_DOMAIN_CN_ID] == RRCSMC_SECURITY_STARTED))
        {
          max_count_c_ps=(max_count_c_ps<local_hfn_per_ul_rb[local_rb_id])?local_hfn_per_ul_rb[local_rb_id]:max_count_c_ps;
        }
      }
    }
    /*Find the maximum HFN for DL UM RBs to be released*/
    for(i=0;i<rlc_req_type_ptr->dl_um.nchan;i++)
    {
      local_rb_id=rrclcm_get_dl_rb_id(rlc_req_type_ptr->dl_um.chan_info[i].lc_id);
      if((local_rb_id < MAX_RB) && (rlc_req_type_ptr->dl_um.dl_um_act[i].action==RLC_RELEASE))
      {
        if((domain_id_list[local_rb_id]==RRC_CS_DOMAIN_CN_ID) &&
           (rrcsmc_int_ciph_info.ciph_status[RRC_CS_DOMAIN_CN_ID] == RRCSMC_SECURITY_STARTED))
        {
          max_count_c_cs=(max_count_c_cs<local_hfn_per_dl_rb[local_rb_id])?local_hfn_per_dl_rb[local_rb_id]:max_count_c_cs;
        }
        else if((domain_id_list[local_rb_id]==RRC_PS_DOMAIN_CN_ID) &&
                (rrcsmc_int_ciph_info.ciph_status[RRC_PS_DOMAIN_CN_ID] == RRCSMC_SECURITY_STARTED))
        {
          max_count_c_ps=(max_count_c_ps<local_hfn_per_dl_rb[local_rb_id])?local_hfn_per_dl_rb[local_rb_id]:max_count_c_ps;
        }
      }
    }
    /*Find the maximum HFN for AM RBs to be released*/
    for(i=0;i<rlc_req_type_ptr->am.nentity;i++)
    {
      if(rlc_req_type_ptr->am.am_act[i].action==RLC_RELEASE)
      {
        local_rb_id=rlc_req_type_ptr->am.am_info[i]->common_config.rb_id;
        if(local_rb_id < MAX_RB)
        {
          if(domain_id_list[local_rb_id]==RRC_CS_DOMAIN_CN_ID)
          {
          /*Get the maximum hfn for DL AM for CS domain*/
            max_count_c_cs=(max_count_c_cs<local_hfn_per_dl_rb[local_rb_id])?local_hfn_per_dl_rb[local_rb_id]:max_count_c_cs;

          /*Get the maximum hfn for UL AM for CS domain*/
            max_count_c_cs=(max_count_c_cs<local_hfn_per_ul_rb[local_rb_id])?local_hfn_per_ul_rb[local_rb_id]:max_count_c_cs;
          }
          else
          {
          /*Get the maximum hfn for DL AM for PS domain*/
            max_count_c_ps=(max_count_c_ps<local_hfn_per_dl_rb[local_rb_id])?local_hfn_per_dl_rb[local_rb_id]:max_count_c_ps;

          /*Get the maximum hfn for UL AM for PS domain*/
            max_count_c_ps=(max_count_c_ps<local_hfn_per_ul_rb[local_rb_id])?local_hfn_per_ul_rb[local_rb_id]:max_count_c_ps;
          }
        }
      }
    }
  }

  /*Now store the max count c for released rbs in the temporary variable*/
  max_count_c.count_c_cs=(max_count_c.count_c_cs>max_count_c_cs)?
                          max_count_c.count_c_cs:max_count_c_cs;
  max_count_c.count_c_ps=(max_count_c.count_c_ps>max_count_c_ps)?
                          max_count_c.count_c_ps:max_count_c_ps;

  WRRC_MSG2_HIGH("Max count c for released RB for cs domain = %x ps domain = %x ",max_count_c.count_c_cs,max_count_c.count_c_ps);
  
  rrc_free(local_hfn_type_ptr);
  
}

/*===========================================================================

FUNCTION rrcsmc_update_ciphering_config_info_for_reconfig

DESCRIPTION
  This function is called by LLC procedure before it sends ciphering
  information to RLC, MAC and L1 layers.  When this function is called,
  SMC procedure updates the global DB for ciphering configuration with
  the latest ciphering information. 

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS: if UM or AM RB's are added
  FAILURE: if UM or AM RB's are not added

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
uecomdef_status_e_type rrcsmc_update_ciphering_config_info_for_reconfig
(
rrcllc_rlc_config_req_list_type *rlc_req_type_ptr,
rrc_proc_e_type proc_id
)
{
if(nv_ciphering_enabled)
{
  rrcrb_rab_info_per_msg_type *local_est_rab_ptr; /*store rab info*/

  /*local array to store domain id's for all rb's*/
  rrc_cn_domain_identity_e_type  local_domain_id_list[MAX_RB];

  /*local var to store domain id in for loop*/
  rrc_cn_domain_identity_e_type  local_domain_id;

  /*local vars to hold count and other misc info*/
  uint32 count,i, local_rb_id;

  /*status to indicate if am or um rb's are reconfigured*/
  uecomdef_status_e_type am_um_reconfig_status;

  /*This Variable will be set to TRUE when SRB-4 is established & 
    LATEST CONFIGURED CN DOMAIN is initialized */
  boolean is_srb4_est_and_security_started = FALSE;

  am_um_reconfig_status = FAILURE;

  /**************
    updation of ciphering configuration can be done only be RBE is new RB's are
    added or by RBR if RB's are deleted.  In future when SRNS relocation is
    supported, we will revisit this area
   **************/

  /*ciphering info for rlc is incremental.  so set UM and AM info
    to zero and only initialize the new radio bearers*/
  ciph_rb_info.am_rb_ciph_config.num_rb = 0;
  ciph_rb_info.um_rb_ciph_config.num_rb = 0;

  if(proc_id == RRC_PROCEDURE_RBE || proc_id == RRC_PROCEDURE_RBR || proc_id == RRC_PROCEDURE_CU)
  {
     MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"N_RB %d, S_V_T %d, N_TM %d CS_TM_AFF %d ",
              ciph_rb_info.tm_rb_ciph_config[0].num_rb, 
              START_VALUE_TO_TRANSMIT_FLAG,
      NEW_TM_RB_IND_FLAG,
      ciph_rb_info.tm_rb_ciph_config[0].affected
   );
    
  }
  /*First act on RB release info & then act on RB est. */ 
  if (proc_id == RRC_PROCEDURE_RBR || proc_id == RRC_PROCEDURE_CU || proc_id == RRC_PROCEDURE_RBE)
  {
    //reset the flags that checks to see if all SIB's are received  
    memset (local_domain_id_list, (int)RRC_CS_DOMAIN_CN_ID, MAX_RB * sizeof(rrc_cn_domain_identity_e_type));

    /*get the domain id of all the newly established radio bearers*/
    for (count = 0; count < MAX_RAB_TO_SETUP; count++)
    {
      if (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID)
      {
        for (i=0; i<rrc_est_rabs.rabs[count].num_rbs_for_rab; i++)
        {
          if(rrc_est_rabs.rabs[count].cn_domain == RRC_CS_DOMAIN_CN_ID)
          {
            local_domain_id_list[rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id] =
            RRC_CS_DOMAIN_CN_ID;
          }
          else
          {
            local_domain_id_list[rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id] =
            RRC_PS_DOMAIN_CN_ID;
          }
        }
      }
    }
    /*Calculate and store the max_count_c for released RBs */
    rrcsmc_calc_max_count_c_released_rb(rlc_req_type_ptr,local_domain_id_list,proc_id);
    
    /* Update struct for tm radio bearer id */
    for (i=0; i < rlc_req_type_ptr->ul_tm.nchan; i++)
    {
      local_rb_id = rrclcm_get_ul_rb_id(rlc_req_type_ptr->ul_tm.chan_info[i].lc_id);
      
      if(local_rb_id < MAX_RB)
      {
        local_domain_id = local_domain_id_list[local_rb_id];

        /* Update the TM RB info only if they exist.There is a possibility that these RBs
           could have been removed before the RadioBearerRelease during a 
           SignallingConnectionReleaseIndication */
        if(ciph_rb_info.tm_rb_ciph_config[local_domain_id].num_rb != 0)
        {
          //rb_id 0 is TM as well unciphered.. we don't do ciphering/deciphering on RB0 which is
          //a common channel, so in case RB0 is being released, don't change ciphering database
          if(local_rb_id != 0)
          {
            if((local_domain_id != RRC_CS_DOMAIN_CN_ID) && 
               (local_domain_id != RRC_PS_DOMAIN_CN_ID))
            {
              WRRC_MSG2_ERROR("Invalid Domain ID %d to delete RB_ID %d", local_domain_id, local_rb_id);
            }
            else
            {
              if(((proc_id == RRC_PROCEDURE_CU)||
                  (proc_id == RRC_PROCEDURE_RBR)||
                  (proc_id == RRC_PROCEDURE_RBE))&& 
                  (RLC_RELEASE == rlc_req_type_ptr->ul_tm.ul_tm_act[i].action)
                )
              {
                ciph_rb_info.tm_rb_ciph_config[local_domain_id].num_rb--;

                if (ciph_rb_info.tm_rb_ciph_config[local_domain_id].num_rb == 0)
                {
                  ciph_rb_info.tm_rb_ciph_config[local_domain_id].affected = FALSE;
                  ciph_rb_info.tm_rb_ciph_config[local_domain_id].rb_status = CIPH_NOT_STARTED;
                  cipher_config_db.mac_cipher_info[local_domain_id].cipher_enable = FALSE;
                }
              }
            }
          }
        }
      }
    }
  }

  /*check if this update is due to Radio Bearer Establishment*/
  if ( proc_id == RRC_PROCEDURE_RBE )
  {
    /*get the established rabs due to RBE from RRCRBE procedure*/
    local_est_rab_ptr = rrcrbe_get_est_rab_ptr();

    /*get the domain id of all the newly established radio bearers*/
    for (count = 0; count < local_est_rab_ptr->num_rabs_in_msg; count++)
    {
      if (local_est_rab_ptr->rabs_in_msg[count].rab_id != RRC_INVALID_RAB_ID)
      {
        for (i=0; i<local_est_rab_ptr->rabs_in_msg[count].num_rbs_for_rab; i++)
        {
          
          if(local_est_rab_ptr->rabs_in_msg[count].cn_domain == RRC_CS_DOMAIN_CN_ID)
          {
            local_domain_id_list[local_est_rab_ptr->rabs_in_msg[count].rb_for_rab[i].rb_id] = 
            RRC_CS_DOMAIN_CN_ID;
          }
          else
          {
            local_domain_id_list[local_est_rab_ptr->rabs_in_msg[count].rb_for_rab[i].rb_id] = 
            RRC_PS_DOMAIN_CN_ID;
            /*Reset TM affected status for CS domain if PS RAB is getting setup as this message can't setup TM RBs*/
            ciph_rb_info.tm_rb_ciph_config[RRC_CS_DOMAIN_CN_ID].affected = FALSE;
          }
        }/* for loop for num of rb id for each rab*/
      }/*if not INVALID_RAB_ID*/
    }/*for */


    /*update struct for am radio bearer id*/
    for (i=0; (i < rlc_req_type_ptr->am.nentity) && (ciph_rb_info.am_rb_ciph_config.num_rb < UE_MAX_AM_ENTITY); i++)
    {
      if(rlc_req_type_ptr->am.am_act[i].action != RLC_ESTABLISH)
      {
        continue;
      }

      local_rb_id = rlc_req_type_ptr->am.am_info[i]->common_config.rb_id;

      ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].rb_id =
      (uint8) rlc_req_type_ptr->am.am_info[i]->common_config.rb_id;

      /*check if the AM rb id is a SRB, and set SRB flag to TRUE*/
      if ((rlc_req_type_ptr->am.am_info[i]->common_config.rb_id == 
           DCCH_AM_RADIO_BEARER_ID) ||
          (rlc_req_type_ptr->am.am_info[i]->common_config.rb_id == 
           DCCH_DT_HIGH_PRI_RADIO_BEARER_ID) ||
          (rlc_req_type_ptr->am.am_info[i]->common_config.rb_id ==  
           DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
         )
      {
        ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
        is_rb_srb=TRUE;
        ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
          domain_id = rrcsmc_int_ciph_info.latest_config_cn_id;
      }
      /*if not a SRB, set is_rb_srb flag to false and get the domain id for the RB ID*/
      else
      {
        ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
        is_rb_srb = FALSE;
        /*lint -e644*/
        ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
        domain_id = local_domain_id_list[local_rb_id];
        /*lint +e644*/
      }
      /*If Signalling RB is getting set up, Set START_VALUE_TO_TRANSMIT_FLAG 
        only if the variable LATEST_CONFIGURED_CN_DOMAIN has been initialised.
        LATEST_CONFIGURED_CN_DOMAIN will be initialised when either integrity 
        and/or ciphering is started. So check security status here*/
      if((ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
          is_rb_srb == TRUE)&&
         (rrcsmc_get_security_start_status() == TRUE)
        )
      {
        /*this flag is queried by the reconfig procedure and indicates that new 
          START values need to be transmitted as AM/UM radio bearers are setup*/
        START_VALUE_TO_TRANSMIT_FLAG = TRUE;
        start_value_to_transmit_domain =  rrcsmc_int_ciph_info.latest_config_cn_id;
        /*Variable is_srb4_est_and_security_started is used to keep track 
          Of below section of the spec.
          8.2.2.2 Initiation
          - if signalling radio bearer RB4 is setup with this procedure and signalling radio bearers 
             RB1-RB3 were already established prior to the procedure:
          - if the variable "LATEST_CONFIGURED_CN_DOMAIN" has been initialised:
             connect any radio bearers setup by the same message as signalling radio bearer RB4
             to the CN domain indicated in the variable "LATEST CONFIGURED CN DOMAIN".*/
        if(ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].rb_id
           == DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
        {
          is_srb4_est_and_security_started = TRUE;
        }
      }
      
      /*If SRB-4 is getting set up through this RBsetup msg & LATEST CONFIGURED CN DOMAIN
        Is initialized then N/W should take care of associating CN domain of any RB getting set 
        up with this message with  LATEST CONFIGURED CN DOMAIN as per 8.2.2.2. 
        If so skip setting start_value_to_transmit_domain as its taken care in the handling 
        for SRB-4 above */
      if((ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
          is_rb_srb != TRUE)&&
          (is_srb4_est_and_security_started == FALSE))
      {
        /*this flag is queried by the reconfig procedure and indicates that new 
          START values need to be transmitted as AM/UM radio bearers are setup*/
        START_VALUE_TO_TRANSMIT_FLAG = TRUE;
        start_value_to_transmit_domain = RRC_PS_DOMAIN_CN_ID;
      }

      /*initialize ciph first time and increment the number of rb's counter*/
      ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.
      num_rb].rb_status = CIPH_FIRST_TIME;
      ciph_rb_info.am_rb_ciph_config.num_rb++;

      /*return STATUS is set to SUCCESS to indicate to LLC that we have to send
        RLC UL/DL cipher config requests*/
      am_um_reconfig_status = SUCCESS;
      if(ciph_rb_info.am_rb_ciph_config.num_rb == UE_MAX_AM_ENTITY)
      {
        WRRC_MSG0_ERROR("Reached the max value for num_rb");
        break;
      }
    }

    /*update struct for um radio bearer id*/
    for (i=0; (i< rlc_req_type_ptr->ul_um.nchan) && (ciph_rb_info.um_rb_ciph_config.num_rb < UE_MAX_UL_UM_CHANNEL); i++)
    {
      if(rlc_req_type_ptr->ul_um.ul_um_act[i].action != RLC_ESTABLISH)
      {
        continue;
      }

      local_rb_id = rrclcm_get_ul_rb_id(rlc_req_type_ptr->ul_um.chan_info[i].lc_id);

      ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].rb_id = 
      (uint8) rrclcm_get_ul_rb_id(rlc_req_type_ptr->ul_um.chan_info[i].lc_id);

      /*check if SRB or not and set the flag appropriately*/
      if (ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
          rb_id == DCCH_UM_RADIO_BEARER_ID )
      {
        ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
        is_rb_srb=TRUE;
        /*this will only be changing config. of SRB's.  Support for checking RLC size if
          not currently present.  Continue without changing SRB's ciphering information*/
        continue;
      }
      else
      {
        ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
        is_rb_srb=FALSE;
        if(local_rb_id < MAX_RB)
        {
          /*lint -e644*/
          ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
           domain_id = local_domain_id_list[local_rb_id];
          /*lint +e644*/
        }        
      }
      /*If Signalling RB is getting set up, Set START_VALUE_TO_TRANSMIT_FLAG 
        only if the variable LATEST_CONFIGURED_CN_DOMAIN has been initialised.
        LATEST_CONFIGURED_CN_DOMAIN will be initialised when either integrity 
        and/or ciphering is started. So check security status here*/
      if((ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
          is_rb_srb == TRUE)&&
         (rrcsmc_get_security_start_status() == TRUE)
        )
      {
        /*this flag is queried by the reconfig procedure and indicates that new 
          START values need to be transmitted as AM/UM radio bearers are setup*/
        START_VALUE_TO_TRANSMIT_FLAG = TRUE;
        start_value_to_transmit_domain =  rrcsmc_int_ciph_info.latest_config_cn_id;
      }
      
      /*If SRB-4 is getting set up through this RBsetup msg & LATEST CONFIGURED CN DOMAIN
        Is initialized then N/W should take care of associating CN domain of any RB getting set 
        up with this message with  LATEST CONFIGURED CN DOMAIN as per 8.2.2.2. 
        If so skip setting start_value_to_transmit_domain as its taken care in the handling 
        for SRB-4 above */
      if((ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
          is_rb_srb != TRUE)&&
          (is_srb4_est_and_security_started == FALSE))
      {
        /*this flag is queried by the reconfig procedure and indicates that new 
          START values need to be transmitted as AM/UM radio bearers are setup*/
        START_VALUE_TO_TRANSMIT_FLAG = TRUE;
        start_value_to_transmit_domain = RRC_PS_DOMAIN_CN_ID;
      }

      /*initialize ciph first time and increment the number of rb's counter*/
      ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
      rb_status = CIPH_FIRST_TIME;
      ciph_rb_info.um_rb_ciph_config.num_rb++;

      /*return STATUS is set to SUCCESS to indicate to LLC that we have to send
        RLC UL/DL cipher config requests*/
      am_um_reconfig_status = SUCCESS;
    }

    /*update struct for tm radio bearer id*/
    for (i=0; i< rlc_req_type_ptr->ul_tm.nchan; i++)
    {
      /* CCCH check is added for the following case.
         CR#738055 - RB setup taking UE from DCH to FACH without Redirection info. RBE is 
         setting up CCCH to send CU          
      */
      if((rlc_req_type_ptr->ul_tm.ul_tm_act[i].action != RLC_ESTABLISH) || (rlc_req_type_ptr->ul_tm.chan_info[i].lc_type == UE_LOGCHAN_CCCH))
      {
        continue;
      }

      local_rb_id=rrclcm_get_ul_rb_id(rlc_req_type_ptr->ul_tm.chan_info[i].lc_id);

      if(local_rb_id < MAX_RB)
      {
        local_domain_id = local_domain_id_list[local_rb_id];

          
        if(rrcsmc_int_ciph_info.ciph_status[local_domain_id] == RRCSMC_SECURITY_STARTED)
        {
          /*if TM rb's are being configured for the first time, set affected flag to
            TRUE*/
          if (ciph_rb_info.tm_rb_ciph_config[local_domain_id].affected != TRUE)
          {
            ciph_rb_info.tm_rb_ciph_config[local_domain_id].affected = TRUE;
            ciph_rb_info.tm_rb_ciph_config[local_domain_id].rb_status = CIPH_FIRST_TIME;
          }
    
          /*increment the number of rb's counter*/
          ciph_rb_info.tm_rb_ciph_config[local_domain_id].num_rb++;
    
          /*if this is the first RB for this CN domain, then set NEW flag to TRUE*/

          /*If SRB-4 is getting set up through this RBsetup msg & LATEST CONFIGURED CN DOMAIN
           Is initialized then N/W should take care of associating CN domain of any RB getting set 
           up with this message with  LATEST CONFIGURED CN DOMAIN as per 8.2.2.2. 
           If so skip setting start_value_to_transmit_domain as its taken care in the handling 
           for SRB-4 above */
          if ((ciph_rb_info.tm_rb_ciph_config[local_domain_id].num_rb == 1)&&
              (is_srb4_est_and_security_started == FALSE))
          {
            /*If a new TM Radio Bearer is being added, then set this flag for the
              reconfig procedure to query TM RB ciphering act CFN.
             */
            NEW_TM_RB_IND_FLAG = TRUE;      
    
            /*this flag is queried by the reconfig procedure and indicates that new 
              START values need to be transmitted as AM/UM radio bearers are setup*/
            START_VALUE_TO_TRANSMIT_FLAG = TRUE;
            start_value_to_transmit_domain = RRC_CS_DOMAIN_CN_ID;
          }
        }
        else
        {
          /*If prior to this procedure there exists no transparent mode radio bearer for the 
            CN domain included in the IE "CN domain identity" and at least one transparent
            mode radio bearer is included in the IE "RB information to setup" then,
            set START_VALUE_TO_TRANSMIT_FLAG */

          /*If SRB-4 is getting set up through this RBsetup msg & LATEST CONFIGURED CN DOMAIN
           Is initialized then N/W should take care of associating CN domain of any RB getting set 
           up with this message with  LATEST CONFIGURED CN DOMAIN as per 8.2.2.2. 
           If so skip setting start_value_to_transmit_domain as its taken care in the handling 
           for SRB-4 above */
          if((ciph_rb_info.tm_rb_ciph_config[local_domain_id].num_rb == 0)&&
             (is_srb4_est_and_security_started == FALSE))
          {
            /*this flag is queried by the reconfig procedure and indicates that new 
              START values need to be transmitted as first time TM radio bearers are setup*/
            START_VALUE_TO_TRANSMIT_FLAG = TRUE;
            start_value_to_transmit_domain = RRC_CS_DOMAIN_CN_ID;
          }
          ciph_rb_info.tm_rb_ciph_config[local_domain_id].num_rb++;
        }
      }
    }

    /*update the global ciphering database which will be used by LLC to configure
      RLC/MAC/L1*/
    rrcsmc_update_global_ciph_db_for_reconfig();

  }
  

  if(proc_id == RRC_PROCEDURE_RBE || proc_id == RRC_PROCEDURE_RBR || proc_id == RRC_PROCEDURE_CU)
  {
     MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"N_RB %d, S_V_T %d, N_TM %d CS_TM_AFF %d ",
              ciph_rb_info.tm_rb_ciph_config[0].num_rb, 
              START_VALUE_TO_TRANSMIT_FLAG,
      NEW_TM_RB_IND_FLAG,
      ciph_rb_info.tm_rb_ciph_config[0].affected
   );

    
  }

  return am_um_reconfig_status;
}
else
{
  return FAILURE;  /*if NV item is not turned on return failure,
                     LLC will not configure RLC with new ciph config*/
}
}/*rrcsmc_update_ciphering_config_info_for_reconfig*/
/*lint +e715*/
/*===========================================================================

FUNCTION rrcsmc_update_ciphering_config_info

DESCRIPTION
  Update ciphering config info for START of ciphering.  This function
  is called by RRCSMC procedure.  This is called to update ciphering
  parameters when ciphering is being first started for a particular
  CN domain.
  
DEPENDENCIES
  None

RETURN VALUE
  SUCCESS or FAILURE

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_update_ciphering_config_info
(
  rrc_ciph_config_e_type config_cause
)
{
if(nv_ciphering_enabled)
{

  /*local vars to store count and other misc local values*/
  uint32 count=0,i=0;

  /*local var to store cn domain id*/
  rrc_cn_domain_identity_e_type  local_domain_id, local_latest_config_cn_domain;

  /*local var to store logical channel ID and radio bearer ID*/
  uint32                 local_lc_id, local_rb_id;


  /***********BEGINNING OF CIPH CONFIGURATION FOR SRB's************/

  /*ciphering info for rlc is incremental.  so set UM and AM info
    to zero and only initialize the new radio bearers*/
  ciph_rb_info.am_rb_ciph_config.num_rb = 0;
  ciph_rb_info.um_rb_ciph_config.num_rb = 0;

  local_latest_config_cn_domain = rrcsmc_get_latest_configured_cn_domain();
  
  /*hardcode ciphering configuration for RB 2*/
  ciph_rb_info.am_rb_ciph_config.config_info[count].is_rb_srb = TRUE;
  ciph_rb_info.am_rb_ciph_config.config_info[count].domain_id = local_latest_config_cn_domain;

  ciph_rb_info.am_rb_ciph_config.config_info[count].rb_id = 
  DCCH_AM_RADIO_BEARER_ID; 

  ciph_rb_info.am_rb_ciph_config.config_info[count].rb_status = 
  CIPH_FIRST_TIME;

  ciph_rb_info.am_rb_ciph_config.num_rb++;
  count = ciph_rb_info.am_rb_ciph_config.num_rb;

  /*hardcode ciphering configuration for RB 3*/
  ciph_rb_info.am_rb_ciph_config.config_info[count].is_rb_srb = TRUE;
  ciph_rb_info.am_rb_ciph_config.config_info[count].domain_id = 
                                                            local_latest_config_cn_domain;

  ciph_rb_info.am_rb_ciph_config.config_info[count].rb_id = 
  DCCH_DT_HIGH_PRI_RADIO_BEARER_ID; 

  ciph_rb_info.am_rb_ciph_config.config_info[count].rb_status = 
  CIPH_FIRST_TIME;

  ciph_rb_info.am_rb_ciph_config.num_rb++;
  count = ciph_rb_info.am_rb_ciph_config.num_rb;

  /*check if RB4 is setup or not and then config ciphering config for RB4*/
  if (RRCLCM_RLC_LC_ID_NOT_FOUND != 
      rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                DCCH_DT_LOW_PRI_RADIO_BEARER_ID, 
                                UE_MODE_ACKNOWLEDGED_DATA))
  {

    ciph_rb_info.am_rb_ciph_config.config_info[count].is_rb_srb = TRUE;
    ciph_rb_info.am_rb_ciph_config.config_info[count].domain_id = 
                                                    local_latest_config_cn_domain;

    ciph_rb_info.am_rb_ciph_config.config_info[count].rb_id = 
    DCCH_DT_LOW_PRI_RADIO_BEARER_ID; 

    ciph_rb_info.am_rb_ciph_config.config_info[count].rb_status = 
    CIPH_FIRST_TIME;

    ciph_rb_info.am_rb_ciph_config.num_rb++;
    count = ciph_rb_info.am_rb_ciph_config.num_rb;
  }

  /*count is set to 0 as RB 1 is UM radiobearer, different array is
    used to store UM RB's*/
  count=0;

  /*hardcode ciphering configuration for RB 1*/
  ciph_rb_info.um_rb_ciph_config.config_info[count].is_rb_srb = TRUE;
  ciph_rb_info.um_rb_ciph_config.config_info[count].domain_id = 
                                                     local_latest_config_cn_domain;

  ciph_rb_info.um_rb_ciph_config.config_info[count].rb_id = 
  DCCH_UM_RADIO_BEARER_ID; 

  ciph_rb_info.um_rb_ciph_config.config_info[count].rb_status = 
  CIPH_FIRST_TIME;

  ciph_rb_info.um_rb_ciph_config.num_rb++;

  /***********END OF CIPH CONFIGURATION FOR SRB's************/

     MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"N_RB %d, S_V_T %d, N_TM %d CS_TM_AFF %d ",
            ciph_rb_info.tm_rb_ciph_config[0].num_rb, 
            START_VALUE_TO_TRANSMIT_FLAG,
      NEW_TM_RB_IND_FLAG,
      ciph_rb_info.tm_rb_ciph_config[0].affected
   );

  /***********BEGINNING OF CIPH CONFIGURATION FOR UM/AM/TM RB's************/

  /*loop through established_rabs struct and configure ciphering info. for
    non-SRB Radio Bearers*/
  for ( count=0; count<MAX_RAB_TO_SETUP; count++ )
  {
    if (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID)
    {
      for (i=0; i< rrc_est_rabs.rabs[count].num_rbs_for_rab; i++)
      {
        if (rrc_est_rabs.rabs[count].rb_for_rab[i].rb_status ==
            RRC_RB_STARTED)
        {

          /*copy to local vars to avoid multiple indirections in each loop and
                generally lengthy variable names and confusion */
          local_rb_id = rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id;
          local_lc_id = rrclcm_get_ul_rlc_lc_id(rrc_est_rabs.rabs[count].
                                                rb_for_rab[i].rb_id);
          if(rrc_est_rabs.rabs[count].cn_domain == RRC_CS_DOMAIN_CN_ID)
          {
            local_domain_id = RRC_CS_DOMAIN_CN_ID;
          }
          else
          {
            local_domain_id = RRC_PS_DOMAIN_CN_ID;
          }

          /*initialize value of TM radio bearers.  set affected status to TRUE
            cn domain and increment num_rb*/
          if (local_lc_id < RRCLCM_UL_UM_START_IDX)
          {
            if(local_domain_id ==  local_latest_config_cn_domain)
            {

              ciph_rb_info.tm_rb_ciph_config[local_domain_id].affected = TRUE;
  
              ciph_rb_info.tm_rb_ciph_config[local_domain_id].rb_status =
              CIPH_FIRST_TIME;  
              if((CIPH_CONFIG_GTOW == config_cause) || (CIPH_CONFIG_CS == config_cause) || 
                (CIPH_CONFIG_CS_PLUS_PS == config_cause))
              {
                ciph_rb_info.tm_rb_ciph_config[local_domain_id].num_rb++;
              }
            }
            else
            {
              ciph_rb_info.tm_rb_ciph_config[local_domain_id].affected = FALSE;
            }
          }
          /*check if lc_id of the Radio Bearer belongs to AM Mode*/
          else if (local_lc_id >= RRCLCM_UL_AM_DATA_START_IDX &&
                   local_lc_id < RRCLCM_UL_AM_DATA_START_IDX+UE_MAX_UL_AM_DATA_CHANNEL)
          {
            /*initialize value of AM radio bearers.  set SRB true to false.  set rb id
              cn domain and increment num_rb*/
            if(ciph_rb_info.am_rb_ciph_config.num_rb < UE_MAX_AM_ENTITY)
            {
              ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
              is_rb_srb = FALSE;

              ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
              rb_id = (uint8) local_rb_id;

              ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
              rb_status = CIPH_FIRST_TIME;      

              
            if(rrc_est_rabs.rabs[count].cn_domain == RRC_CS_DOMAIN_CN_ID)
              {
                ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
                        domain_id = RRC_CS_DOMAIN_CN_ID;
              }
              else
              {
                ciph_rb_info.am_rb_ciph_config.config_info[ciph_rb_info.am_rb_ciph_config.num_rb].
                        domain_id =  RRC_PS_DOMAIN_CN_ID;
              }  
              ciph_rb_info.am_rb_ciph_config.num_rb++;          
            }
            else
            {
              ERR_FATAL("AM num_rb is out or range: %d",ciph_rb_info.am_rb_ciph_config.num_rb,0,0);
            }
          }
          /*check if lc_id of the Radio Bearer belongs to UM Mode*/
          else if (local_lc_id >= RRCLCM_UL_UM_START_IDX &&
                   local_lc_id < RRCLCM_UL_AM_DATA_START_IDX)
          {

            /*initialize value of UM radio bearers.  set SRB true to false.  set rb id
              cn domain and increment num_rb*/
            if(ciph_rb_info.um_rb_ciph_config.num_rb < UE_MAX_UL_UM_CHANNEL)
            {
              ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
              is_rb_srb = FALSE;

              ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
              rb_id = (uint8) local_rb_id;

              ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
              rb_status= CIPH_FIRST_TIME;

            if(rrc_est_rabs.rabs[count].cn_domain == RRC_CS_DOMAIN_CN_ID)
              {
                ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
                  domain_id = RRC_CS_DOMAIN_CN_ID;
              }
              else
              {
                ciph_rb_info.um_rb_ciph_config.config_info[ciph_rb_info.um_rb_ciph_config.num_rb].
                  domain_id =  RRC_PS_DOMAIN_CN_ID;
              }  
              ciph_rb_info.um_rb_ciph_config.num_rb++;          
            }
            else
            {
              ERR_FATAL("UM num_rb is out or range: %d",ciph_rb_info.um_rb_ciph_config.num_rb,0,0);
            }
          }/*else if check for UM RB's*/
        }/*if check for RB STARTED*/
      }/*for loop for number of RB's in RAB*/
    }/*check for INVALID_RAB_ID*/
  }/*for loop for MAX_RAB_TO_SETUP*/

  /***********END OF CIPH CONFIGURATION FOR UM/AM/TM RB's************/

     MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"N_RB %d, S_V_T %d, N_TM %d CS_TM_AFF %d ",
            ciph_rb_info.tm_rb_ciph_config[0].num_rb, 
            START_VALUE_TO_TRANSMIT_FLAG,
    NEW_TM_RB_IND_FLAG,
    ciph_rb_info.tm_rb_ciph_config[0].affected
  );
       

  /*update the global ciphering database which will be used by LLC to configure
    RLC/MAC/L1*/
  rrcsmc_update_global_ciph_db_for_ciph_start(local_latest_config_cn_domain
                                              ,config_cause
  );

  return SUCCESS;
}
else
{
  return SUCCESS;  /*to ward off warnings*/
}
}
/*===========================================================================

FUNCTION rrcsmc_init_integrity_protection_info

DESCRIPTION
  Initialize the data structure integrity_protection_info with default
  values.  count_c and count_i values to default hfn's and 0's for seq_num
  also initialize status and reconfig flags to false.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
static void rrcsmc_init_integrity_protection_info
(
void
)
{
  uint32 i=0, j=0;             /*local variables used for counting in for loop*/

  /*local var to store domain_id*/  
  rrc_cn_domain_identity_e_type domain_id=RRC_CS_DOMAIN_CN_ID; 
  
  uint32 init_seq_num=0;

  /*initializer the status, modify and reconfig flag to false*/
  integrity_protection_info.reconfig = FALSE;
  integrity_protection_info.ip_status = RRCSMC_SECURITY_NOT_STARTED;        
  integrity_protection_info.modify_received = FALSE;


  /*cycle through both cn domains*/  
  for (j=0; j<RRC_MAX_CN_DOMAINS; j++)
  {
    if (j == 0)
    {
      domain_id = RRC_CS_DOMAIN_CN_ID;
    }
    else
    {
      domain_id = RRC_PS_DOMAIN_CN_ID;    
    }

    /*initialize count_i for cs and ps domain*/
    for ( i=0; i < RRC_MAX_SRB; i++)
    {
      /*initialize uplink count_i with seq_num = 0 and the default HFN received
        for this domain */
      integrity_protection_info.count_i_info[domain_id].dl_count_i[i] = 
      rrcsmc_convert_START_to_count_c_i(
                                       rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
                                       init_seq_num);

      /*initialize the downlink init_flag to false*/
      integrity_protection_info.count_i_info[domain_id].dl_count_i_init_flag[i]=FALSE;

      /*initialize downlink count_i with seq_num = 0 and the default HFN received
        for this domain */      
      integrity_protection_info.count_i_info[domain_id].ul_count_i[i] =       
      rrcsmc_convert_START_to_count_c_i(
                                       rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
                                       init_seq_num);

      /*init the UL/DL act_time_pending_flag to false*/
      integrity_protection_info.count_i_info[domain_id].
      dl_count_i_act_time_pending[i] = FALSE;
      integrity_protection_info.count_i_info[domain_id].
      ul_count_i_act_time_pending[i] = FALSE;

      /*init the UL/DL act_time_pending_flag to false*/
      integrity_protection_info.dl_act_seq_num[i] = 0;
      integrity_protection_info.ul_act_seq_num[i] = 0;

      /*init the new_key_flag to false*/
      integrity_protection_info.count_i_info[domain_id].
      dl_new_key_flag_per_rb[i]=FALSE;
      integrity_protection_info.count_i_info[domain_id].
      ul_new_key_flag_per_rb[i]=FALSE;

      integrity_protection_info.dl_latest_config_cn_id_old_1_valid[i] = FALSE;
    }/*for loop for number of SRB*/  
  }/*for loop for cn_domain_id*/

}/*rrcsmc_init_integrity_protection_info*/
/*===========================================================================

FUNCTION rrcsmc_init_integrity_ciphering_config

DESCRIPTION
  This function initializes the rrcsmc_integrity_and_ciphering_info with 
  the values received from MM in the RRC_SIM_INSERT_REQ. Also a copy is made 
  in the rrcsmc_integrity_and_ciphering_info_old.  MISC procedure will call 
  this function in SMC.


DEPENDENCIES
  None.

RETURN VALUE
  Returns SUCCESS if Initialization of Integrity and Ciphering 
  Configuration is successful.
  
  Returns FAILURE if Initialization of Integrity and Ciphering 
  Configuration is not successful.


SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_init_integrity_ciphering_config
(
rrc_sim_inserted_req_type  *sim_insert_req_ptr
)
{
  uint32 i=0, count;           /*local variable use for counting in for loop*/
  uint32 key[4];               /*local var to print keys received*/
  rrc_cn_domain_identity_e_type domain_id; /*local var to keep track of cn_domain*/

  /*copy number of cn domains into int_ciph_info struct*/  
  rrcsmc_int_ciph_info.num_cn_domains = RRC_MAX_CN_DOMAINS;

  /*copy the contents of cn_domain into int_ciph_info struct*/
  for (i=0; i<RRC_MAX_CN_DOMAINS; i++)
  {

    if (sim_insert_req_ptr->cipher_info[i].domain_id == RRC_CS_DOMAIN_CN_ID)
    {
      /* Domain Index should always be 0 for CS Domain and 1 for 
         PS Domain when using the structure rrcsmc_int_ciph_info */
      domain_id = RRC_CS_DOMAIN_CN_ID;
      /* Reset any keys that were received when RRC was inactive */     
      WRRC_MSG0_HIGH("Invalidate saved security key set");
      rrcsmc_reset_saved_security_key_set_status();
    }
    else
    {
      domain_id = RRC_PS_DOMAIN_CN_ID;    
    }

    /*copy cipher algo enum and domain_id*/    
    rrcsmc_int_ciph_info.int_ciph_info[domain_id].cipher_algo = 
    sim_insert_req_ptr->cipher_info[i].cipher_algo;

    rrcsmc_int_ciph_info.int_ciph_info[domain_id].domain_id = 
    sim_insert_req_ptr->cipher_info[i].domain_id;


    /*copy contents of ciphering_key to int_ciph_info struct
     *for this CN DOMAIN. Always copy to key location 0, then after
   *authentication use 1
     */  
    WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.key[0],
                 sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                 sim_insert_req_ptr->cipher_info[i].ciphering_key,
                 RRC_MAX_CIPHERING_KEY_LENGTH );

    rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.key_used=0;
    rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.new_key=FALSE;
    rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.new_key_for_start_value=FALSE;

    /*copy contents of integrity_key to int_ciph_info struct
     *for this CN DOMAIN
     */
    WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].integrity_key,
                 sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                 sim_insert_req_ptr->cipher_info[i].integrity_key,
                 RRC_MAX_INTEGRITY_KEY_LENGTH);

    for (count=0; count < (RRC_MAX_INTEGRITY_KEY_LENGTH /4); count++)
    {
      key[count] = b_unpackd( &(sim_insert_req_ptr->cipher_info[i].
                                integrity_key[count*4] ), 0, 32 );
    }

    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"INT KEY 1: 0x%x 2: 0x%x 3: 0x%x 4: 0x%x",
    key[0],key[1],key[2],key[3]);

    /*copy contents of HFN to int_ciph_info struct
     *for this CN DOMAIN
     */
    WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
                 sizeof(byte) * RRC_MAX_HFN_LENGTH,
                 sim_insert_req_ptr->cipher_info[i].hfn,
                 RRC_MAX_START_LENGTH);

    MSG_HIGH("EST START: 0x%x 0x%x 0x%x", sim_insert_req_ptr->cipher_info[i].hfn[0],
             sim_insert_req_ptr->cipher_info[i].hfn[1],
             sim_insert_req_ptr->cipher_info[i].hfn[2]);
  }

  /*initialize integrity protection info as we know the valid values of START
    and other information upon insertion of sim*/ 
  rrcsmc_init_integrity_protection_info();

  return SUCCESS;
}/*rrcsmc_init_integrity_ciphering_config*/

/*===========================================================================

FUNCTION rrcsmc_ul_purge_wm

DESCRIPTION
   This function sends RLC_UL_PURGE_WM_REQ with the required information so that RLC purges the WM

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_ul_purge_wm_except_srb2(void)
{
  uint8 count=0;
  rlc_lc_id_type srb4_lc_present = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                 DCCH_DT_LOW_PRI_RADIO_BEARER_ID, 
                                 UE_MODE_ACKNOWLEDGED_DATA);
  /* local cmd var to send to L2 */
  wcdma_l2_ul_cmd_type  *req_cmd_ptr;

  /* Get L2 UL command buffer */
  req_cmd_ptr= wcdma_l2_ul_get_cmd_buf();

  /*Set the command to RLC_UL_PURGE_WM_REQ*/
  req_cmd_ptr->cmd_hdr.cmd_id = RLC_UL_PURGE_WM_REQ;

  /*Add SRB1 Information to the LCs to be purged*/
  req_cmd_ptr->cmd_data.ul_purge_wm_ind.lc_id[count] = rrclcm_get_ul_rlc_lc_id(DCCH_UM_RADIO_BEARER_ID);
  req_cmd_ptr->cmd_data.ul_purge_wm_ind.num_sdus[count++] = MAX_SRB1_SDUS_TOBE_PURGED;

  /*Add SRB3 Information to the LCs to be purged*/
  req_cmd_ptr->cmd_data.ul_purge_wm_ind.lc_id[count] = rrclcm_get_ul_rlc_lc_id(DCCH_DT_HIGH_PRI_RADIO_BEARER_ID);
  req_cmd_ptr->cmd_data.ul_purge_wm_ind.num_sdus[count++] = MAX_SRB2_SDUS_TOBE_PURGED;

  if (RRCLCM_RLC_LC_ID_NOT_FOUND != srb4_lc_present)
  {
    /*Add SRB4 Information to the LCs to be purged*/
    req_cmd_ptr->cmd_data.ul_purge_wm_ind.lc_id[count] = rrclcm_get_ul_rlc_lc_id(DCCH_DT_LOW_PRI_RADIO_BEARER_ID);
    req_cmd_ptr->cmd_data.ul_purge_wm_ind.num_sdus[count++] = MAX_SRB2_SDUS_TOBE_PURGED;
  }

  req_cmd_ptr->cmd_data.ul_purge_wm_ind.num_lc_id = count;

  WRRC_MSG0_HIGH("Dispatching out L2 cmd / Sending RLC_UL_PURGE_WM_REQ except rb2 to L2");

  rrc_send_l2_ul_cmd(req_cmd_ptr);
  
}
/*===========================================================================

FUNCTION rrcsmc_ul_purge_wm_srb2

DESCRIPTION
   This function sends RLC_UL_PURGE_WM_REQ with the required information so that RLC purges the WM

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_ul_purge_wm_srb2(void)
{

  /* local cmd var to send to L2 */
  wcdma_l2_ul_cmd_type  *req_cmd_ptr;

  /* Get L2 UL command buffer */
  req_cmd_ptr= wcdma_l2_ul_get_cmd_buf();

  /*Set the command to RLC_UL_PURGE_WM_REQ*/
  req_cmd_ptr->cmd_hdr.cmd_id = RLC_UL_PURGE_WM_REQ;

  /*Add SRB2 Information to the LCs to be purged*/
  req_cmd_ptr->cmd_data.ul_purge_wm_ind.lc_id[0] = rrclcm_get_ul_rlc_lc_id(DCCH_AM_RADIO_BEARER_ID);
  req_cmd_ptr->cmd_data.ul_purge_wm_ind.num_sdus[0] = MAX_SRB2_SDUS_TOBE_PURGED;
  req_cmd_ptr->cmd_data.ul_purge_wm_ind.num_lc_id = 1;

  WRRC_MSG0_HIGH("Dispatching out L2 cmd / Sending RLC_UL_PURGE_WM_REQ for RB2 to L2");

  rrc_send_l2_ul_cmd(req_cmd_ptr);
  
}
/*===========================================================================

FUNCTION rrcsmc_perform_stop_or_suspend_functionality

DESCRIPTION
   This function suspend's or stops all RLC UM and AM radio bearers by calling
   RLC function crlc_ul_perform_suspend_or_stop_functionality().process suspend 
   cnf for SMC & send SMC complete or process stop cnf for 
   SRNS relocation based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_perform_stop_or_suspend_functionality
(
  rrcsmc_suspend_cmd_type suspend_cmd
)
{
  /* Local variables */
  uint32 i, count;  
  uint32 num_ch_to_suspend_ul = 0;
  uint32 num_ch_to_stop_dl = 0;
  uint32 local_lc_id;
  rlc_lc_id_type srb2_lc_id = 0xFF; //initialize to keep lint happy
  /*Structure to hold info abut suspend Request which will be filled 
      by rrcsmc & RLC will use it for updating its state variables*/
  rlc_ul_stop_suspend_request_type rrcsmc_ul_req;

  rlc_dl_stop_request_type rrcsmc_dl_stop_req;
  
  /*Structure to hold info abut suspend confirmation from RLC*/
  LOCAL  rlc_ul_stop_suspend_cnf_type rrcsmc_ul_cnf ;
  
  boolean status = FALSE;
  
  /* Suspend SRB1 */
  smc_rb_suspend_or_stop_info.suspend_or_stopped_info.suspend_stop[num_ch_to_suspend_ul].lc_id = 
    rrclcm_get_ul_rlc_lc_id(DCCH_UM_RADIO_BEARER_ID);
  num_ch_to_suspend_ul++;

  /* Suspend SRB2 if this suspension is due to SMC */
  if(RRCSMC_SMC_CMD == suspend_cmd)
  {
    srb2_lc_id = rrclcm_get_ul_rlc_lc_id(DCCH_AM_RADIO_BEARER_ID);
    smc_rb_suspend_or_stop_info.suspend_or_stopped_info.suspend_stop[num_ch_to_suspend_ul].lc_id = srb2_lc_id;
    num_ch_to_suspend_ul++;
  }

  /* Suspend SRB3 */
  smc_rb_suspend_or_stop_info.suspend_or_stopped_info.suspend_stop[num_ch_to_suspend_ul].lc_id = 
    rrclcm_get_ul_rlc_lc_id(DCCH_DT_HIGH_PRI_RADIO_BEARER_ID);
  num_ch_to_suspend_ul++;

  if(suspend_cmd == RRCSMC_SRNS_CMD)
  {
    smc_rb_suspend_or_stop_info.dl_stop_info.lc_id[num_ch_to_stop_dl] = 
      rrclcm_get_dl_rlc_lc_id(DCCH_UM_RADIO_BEARER_ID);
    num_ch_to_stop_dl++;
  
    smc_rb_suspend_or_stop_info.dl_stop_info.lc_id[num_ch_to_stop_dl] = 
      rrclcm_get_dl_rlc_lc_id(DCCH_DT_HIGH_PRI_RADIO_BEARER_ID);
    num_ch_to_stop_dl++;
  }

  /* Suspend SRB4 if established */
  if (RRCLCM_RLC_LC_ID_NOT_FOUND !=
      rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                 DCCH_DT_LOW_PRI_RADIO_BEARER_ID, 
                                 UE_MODE_ACKNOWLEDGED_DATA))
  {
    smc_rb_suspend_or_stop_info.suspend_or_stopped_info.suspend_stop[num_ch_to_suspend_ul].lc_id = 
      rrclcm_get_ul_rlc_lc_id(DCCH_DT_LOW_PRI_RADIO_BEARER_ID);
    num_ch_to_suspend_ul++;

    if(suspend_cmd == RRCSMC_SRNS_CMD)
    {
      smc_rb_suspend_or_stop_info.dl_stop_info.lc_id[num_ch_to_stop_dl] = 
        rrclcm_get_dl_rlc_lc_id(DCCH_DT_LOW_PRI_RADIO_BEARER_ID);
      num_ch_to_stop_dl++;
    }
  }

  for(count = 0; count < MAX_RAB_TO_SETUP; count++)
  {
    if (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID)
    {
      /* If the suspension is due to a Security Mode Command
         and the SMC was for CS domain, we do not have to suspend
         the PS RABs */
      
      if( (suspend_cmd == RRCSMC_SMC_CMD) && 
          (saved_security_mode_command_msg.domain_id == RRC_CS_DOMAIN_CN_ID) &&
          (rrc_est_rabs.rabs[count].cn_domain == RRC_PS_DOMAIN_CN_ID) )
      {
        // Do not suspend the radio bearers associated with this RAB
        WRRC_MSG0_HIGH("Not suspending PS RAB during CS SMC");
        break;
      }
        
      for (i=0; i< rrc_est_rabs.rabs[count].num_rbs_for_rab; i++)
      {
        if(suspend_cmd==RRCSMC_SRNS_CMD)
        {
          local_lc_id = rrclcm_get_dl_rlc_lc_id(rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id);
          if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
          {
            if ( local_lc_id >= RRCLCM_DL_UM_START_IDX && 
                 local_lc_id < (RRCLCM_DL_AM_DATA_START_IDX + UE_MAX_DL_AM_DATA_CHANNEL))
            {
              smc_rb_suspend_or_stop_info.dl_stop_info.lc_id[num_ch_to_stop_dl]= (uint8) local_lc_id;
              num_ch_to_stop_dl++;
            }
          }
        }

        local_lc_id = rrclcm_get_ul_rlc_lc_id(rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id);

        /*check if it is a AM or UM RLC RB only*/
        if ( local_lc_id >= RRCLCM_UL_UM_START_IDX && 
             local_lc_id < (RRCLCM_UL_AM_DATA_START_IDX + UE_MAX_UL_AM_DATA_CHANNEL) )
        {
          smc_rb_suspend_or_stop_info.suspend_or_stopped_info.suspend_stop[num_ch_to_suspend_ul].lc_id = (uint8) local_lc_id;
          num_ch_to_suspend_ul++;
        }
      }
    }
  }

  smc_rb_suspend_or_stop_info.suspend_or_stopped_info.nchan= (uint8) num_ch_to_suspend_ul;
  smc_rb_suspend_or_stop_info.dl_stop_info.nchan = (uint8) num_ch_to_stop_dl;

  /*this cmd initiates suspension of RLC radio bearers*/

  /* Fill in the command parameters for SUSPENDING RLC RB's */
  rrcsmc_ul_req.nchan = 
      smc_rb_suspend_or_stop_info.suspend_or_stopped_info.nchan;

  rrcsmc_dl_stop_req.nchan= 
      smc_rb_suspend_or_stop_info.dl_stop_info.nchan;

  /* In a loop set the contents of the suspend_req */
  for (i = 0; i < smc_rb_suspend_or_stop_info.suspend_or_stopped_info.nchan; i++)
  {
    rrcsmc_ul_req.suspend_stop[i].lc_id = 
     smc_rb_suspend_or_stop_info.suspend_or_stopped_info.suspend_stop[i].lc_id;

    /* For SRB2,the offset calculation is done based on the algorithm 
      *  1. Call RLC interface to get the following SRB2 values
      *     - Ciph pending status
      *     - VT_S
      *     - Act_SN if there is a pending config
      *     - WM size in terms of RLC PDUs
      *  2. If Ciph pending status is FALSE
      *       Set suspend_offset = 5
      *     Else
      *       If ((Act_SN - VT_S) >= WM RLC PDUs + RLC_CIPH_ACT_SN_RB_2)
      *         Set suspend_offset = Act_SN - VT_S - WM RLC PDUs 
      *       Else
      *         Set suspend_offset = 5
    */ 
    if((rrcsmc_ul_req.suspend_stop[i].lc_id == srb2_lc_id) &&
       (RRCSMC_SMC_CMD == suspend_cmd))
    {
      /* Local variables */
      uint8 suspend_offset;            
      uint32 vt_s = 0;
      uint32 act_sn = 0;
      uint32 wm_num_rlc_pdus = 0;
      uint32 act_pdu_buffer = 0;
      rlc_cipher_cfg_pending_e_type pending_status;        

      /* Query RLC for the srb2 parameters */
      pending_status = rlc_get_srb_ul_ciph_status
                       (
                           srb2_lc_id,
                           &vt_s,
                           &act_sn,
                           &wm_num_rlc_pdus
                         );
      
       MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"SRB2 Ciph Status: pending_status %d Ciph Status: vt_s %d, act_sn %d, wm_num_rlc_pdus %d",
       pending_status,vt_s,act_sn,wm_num_rlc_pdus
       );
     
      /* Check the Pending status and act accordingly */
      if(pending_status != RLC_CIPHER_CFG_PENDING)
      {
        suspend_offset = RLC_CIPH_SRB_2_ACT_SN_OFFSET;
        WRRC_MSG1_HIGH("No cipher cfg pending. Setting srb2 suspend offset to %d",
                    suspend_offset);
      }
      else // there is a pending config
      {
        /* Calculate the num pdus to activation time.Take care of rollovers */
        act_pdu_buffer = 
          (act_sn < vt_s) ? ((4096 - vt_s) + act_sn) : (act_sn - vt_s);

        WRRC_MSG1_HIGH("Pending status TRUE. Time to Activation %d", act_pdu_buffer);

        /* Check if the WM PDUs and SMComplete can be accomadated in the old 
           config itself i.e. before the act SN is reached.If so,then specify 
           the same pending activation SN in this SMComplete also.Else fall back 
           to the old algo and give an offset of RLC_CIPH_SRB_2_ACT_SN_OFFSET */

        if(rrc_set_srb2_act_time_nv == FALSE)
        {
          if(act_pdu_buffer >= (wm_num_rlc_pdus + RLC_NUM_PDUS_FOR_SMCOMPLETE))
          {
            status = TRUE;
          }
        }
        else
        {
          if(act_pdu_buffer > (wm_num_rlc_pdus + RLC_NUM_PDUS_FOR_SMCOMPLETE))
          {
            status = TRUE;
          }
        }
        
        if(status == TRUE)
        {
          suspend_offset = act_pdu_buffer;
          WRRC_MSG1_HIGH("WM & SMComplete can be sent before Act_SN.Setting srb2 suspend_offset to %d",
                        suspend_offset); 
        }
        else
        {
          suspend_offset = RLC_CIPH_SRB_2_ACT_SN_OFFSET;
          WRRC_MSG1_HIGH("WM & SMComplete cannot be sent before Act_SN.Setting srb2 suspend_offset to %d",
                      suspend_offset); 
        }
      }

      /* Set the offset in the Suspend request */
      rrcsmc_ul_req.suspend_stop[i].n = suspend_offset;
    }
    /* For other SRBs, set the offset to 0 - immediate suspension */
    else 
    {
      rrcsmc_ul_req.suspend_stop[i].n = RRCSMC_SUSPEND_UM_AM_0;
    } 
  }

  /*in a loop set the contents of the stop_req*/
  
  for (i=0; (i < smc_rb_suspend_or_stop_info.dl_stop_info.nchan) && (i < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY)); i++)
  {
    rrcsmc_dl_stop_req.lc_id[i] = 
    smc_rb_suspend_or_stop_info.dl_stop_info.lc_id[i];
  }
  

  if(suspend_cmd == RRCSMC_SRNS_CMD)
  {
    /*Purge the UL WM*/
    /* when performing STOP, we can purge only SRB2 as only SRB2 will re-established now */
    rrcsmc_ul_purge_wm_srb2();
  }
  
  if(suspend_cmd == RRCSMC_SMC_CMD)
  {
    /*process suspend req for SMC*/
    WRRC_MSG0_HIGH("Call RLC call back fun for processing SUSPEND_REQ");
    rrcsmc_ul_req.ul_cmd_type = RLC_SUSPEND_CMD;
  }
  else
  {
    /*process stop req for SRNS*/
    WRRC_MSG0_HIGH("Call RLC call back fun for processing STOP_REQ for SRNS");
    rrcsmc_ul_req.ul_cmd_type = RLC_STOP_CMD;
  }

  /*Call RLC fun here  for  suspending or stoping UL(AM & UM) RB's & SRB's*/
  crlc_ul_perform_suspend_or_stop_functionality(&rrcsmc_ul_req,
                                   &rrcsmc_ul_cnf);

  /*Call RLC fun here  for  stoping DL(AM & UM) RB's & SRB's*/
  if(suspend_cmd == RRCSMC_SRNS_CMD)
  {
    crlc_dl_perform_stop_functionality(&rrcsmc_dl_stop_req);
  }
  
  if(suspend_cmd == RRCSMC_SMC_CMD)
  {
    /*process suspend cnf for SMC & send SMC complete*/
    rrcsmc_process_suspend_cnf(&rrcsmc_ul_cnf);
  }
  else
  {
    /*process suspend cnf for SRNS relocation*/
    rrcsmc_process_suspend_srns_cnf();
  }  
}

/*===========================================================================

FUNCTION rrcsmc_update_integrity_ciphering_config

DESCRIPTION
  This function updates the Integrity and Ciphering Keys received from
  MM in RRC_SIM_UPDATE_REQ command

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_update_integrity_ciphering_config
(
rrc_sim_update_req_type *sim_update_req_ptr 
)
{
  uint32 j=0;       /*local variable use for counting in for loop*/

  rrc_state_e_type rrc_state = rrc_get_state(); /* local var to store rrc state*/

  /*local ciphering info type used to init global vars*/
  rrcsmc_security_config_info_type *local_int_ciph_info_ptr; 

  /*local var to keep track of cn_domain*/
  rrc_cn_domain_identity_e_type domain_id=RRC_CS_DOMAIN_CN_ID; 


  /* Update security information only if SIM_UPDATE_REQ contained any changes to
   security information*/
  if((sim_update_req_ptr->security_update_info.security_update_status 
      == RRC_SIM_UPDATE_VALUE_CHANGED)
#ifdef FEATURE_LTE_TO_WCDMA
     ||
     (sim_update_req_ptr->security_update_info.security_update_status 
        == RRC_SIM_UPDATE_VALUE_LTE_MAPPED)
#endif        
    )
  {
    WRRC_MSG1_HIGH("Received RRC_SIM_UPDATE_REQ for domain: %d", sim_update_req_ptr->security_update_info.ciphering_info.domain_id);
  
    domain_id = sim_update_req_ptr->security_update_info.ciphering_info.domain_id;
    if((rrcmcm_get_rrc_mode() == RRC_MODE_INACTIVE)&&
       (domain_id == RRC_CS_DOMAIN_CN_ID))
    {
      /*save the security key set generated in G-RAT in this variable, 
       *Once RR receives ciphering mod command message in G it will 
       *send a command RR_CIPHER_SYNC_IND to RRC, then RRCSMC 
       *will move these saved security variables to its security database.
       */
      WRRC_MSG0_HIGH("Save security keys generated in G-RAT for CS domain");
      /*Mark saved security keys as valid*/
      saved_security_key_set.grat_key_set_valid = TRUE;
      /*Save ciphering key here*/
      WCDMA_MEMCPY(saved_security_key_set.ciphering_key,
                   sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                   sim_update_req_ptr->security_update_info.ciphering_info.ciphering_key,
                   RRC_MAX_CIPHERING_KEY_LENGTH);
      /*Save integrity key here*/
      WCDMA_MEMCPY(saved_security_key_set.integrity_key,
                   sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                   sim_update_req_ptr->security_update_info.ciphering_info.integrity_key,
                   RRC_MAX_INTEGRITY_KEY_LENGTH);

      /*Print integrity key here*/
      PRINT_CIPHER_INT_KEY(RRC_PRINT_INTEGRITY_KEY,sim_update_req_ptr->security_update_info.
                                  ciphering_info)

      /*Print ciphering key here*/
      PRINT_CIPHER_INT_KEY(RRC_PRINT_CIPHERING_KEY,sim_update_req_ptr->security_update_info.
                                  ciphering_info)
    }
    else
    {
      /*If domain I.D is CS check whether we have any saved security keys which 
        where generated in other RAT, If so the mark them as invalid*/
      if(rrcsmc_return_saved_security_key_set_status(domain_id) == TRUE)
      {
        WRRC_MSG1_HIGH("New CS key rexed invalidating saved security key set=%d",
                                                                           saved_security_key_set.grat_key_set_valid);
        rrcsmc_reset_saved_security_key_set_status();
      }
       
  
      local_int_ciph_info_ptr = &(rrcsmc_int_ciph_info);
      /*Copy Integrity keys for the domain id in sim_update_req*/
        /*save the earlier keys in to a var.  We need this to calculate MAC for some
            SRB's whose activation time has not yet equalled the new activation times*/
      WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].integrity_key_old,
                   sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                   local_int_ciph_info_ptr->int_ciph_info[domain_id].integrity_key,
                   RRC_MAX_INTEGRITY_KEY_LENGTH);
  
      /*copy number of cn domains into int_ciph_info struct*/  
      local_int_ciph_info_ptr->num_cn_domains = RRC_MAX_CN_DOMAINS;
  
  
      /*if authentication is being done after we have started Integrity, then
          set this flag for use by other procedures*/
      if ( integrity_protection_info.ip_status == RRCSMC_SECURITY_STARTED )
      {
        /*When SIM_UPDATE_REQ always doesnt mean new authentication.
          Incase of CS only SRVCC handover NAS can send SIM_UPDATE_REQ with
          PS mapped keys after handover is done. In this case just store the keys 
          and apply them.*/
#ifdef FEATURE_LTE_TO_WCDMA
        if((sim_update_req_ptr->security_update_info.security_update_status 
            == RRC_SIM_UPDATE_VALUE_LTE_MAPPED) && (RRC_PS_DOMAIN_CN_ID == domain_id))
        {
            WRRC_MSG0_HIGH("L2W:Updating Security key sets from SIM_UPDATE_REQ to security DB");                                                  
            /*any previously sent start value is obliterated.. we should start using 0's from
              now onwards.. hence set start_value_update_flag to false*/
            rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;  
        }        
        else
#endif
        {
          for (j=0; j < RRC_MAX_SRB; j++)
          {
            integrity_protection_info.count_i_info[domain_id].
            dl_new_key_flag_per_rb[j]=TRUE;
    
            integrity_protection_info.count_i_info[domain_id].
            ul_new_key_flag_per_rb[j]=TRUE;
          }
        }
      }
      else
      {
        /*any previously sent start value is obliterated.. we should start using 0's from
          now onwards.. hence set start_value_update_flag to false*/
        rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;      
  
        /*Only if rrc state is dch/fach.. i.e. RAT is wcdma, update HFN's
          If in GSM rat do not update rat.. spec. CR 2001. Sep 02 */
        if((rrc_state == RRC_STATE_CELL_DCH) ||
           (rrc_state == RRC_STATE_CELL_FACH)
#ifdef FEATURE_LTE_TO_WCDMA
           ||
          ((sim_update_req_ptr->security_update_info.security_update_status 
              == RRC_SIM_UPDATE_VALUE_LTE_MAPPED) &&
                (rrc_state == RRC_STATE_DISCONNECTED))           
#endif        
          )
        {
          /* Reset the hfn values to 0's if AUTH is done and UE is in WCDMA mode in dch/fach
           * OR keys received from LTE */
          rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[0]=0x0;
          rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[1]=0x0;
          rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[2]=0x0;

          MSG_HIGH("Domain:%d RRC State:%d SIM Update Status:%d", 
            domain_id,
            rrc_state,
            sim_update_req_ptr->security_update_info.security_update_status);
        }
    
        MSG_HIGH("CIPH START VAL: 0x%x 0x%x 0x%x", 
               local_int_ciph_info_ptr->int_ciph_info[domain_id].hfn[0],
               local_int_ciph_info_ptr->int_ciph_info[domain_id].hfn[1],
               local_int_ciph_info_ptr->int_ciph_info[domain_id].hfn[2]);      
      }    
  
      /*copy cipher algo and domain_id*/
      local_int_ciph_info_ptr->int_ciph_info[domain_id].cipher_algo = 
      sim_update_req_ptr->security_update_info.ciphering_info.cipher_algo;
  
      local_int_ciph_info_ptr->int_ciph_info[domain_id].domain_id = 
      sim_update_req_ptr->security_update_info.ciphering_info.domain_id;
  
      /*copy contents of ciphering_key to int_ciph_info struct
       *for this CN DOMAIN.  If we are currently using ciph key 1 then copy
     *new keys to key0.  else copy to 1 and set new_key flag for this
     *domain id to true
      */
      if (rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.key_used == 1)
      {
        WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.key[0],
                     sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                     sim_update_req_ptr->security_update_info.ciphering_info.ciphering_key,
                     RRC_MAX_CIPHERING_KEY_LENGTH);
      }
      else
      {
        WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.key[1],
                     sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                     sim_update_req_ptr->security_update_info.ciphering_info.ciphering_key,
                     RRC_MAX_CIPHERING_KEY_LENGTH );
      } 
      rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.new_key=TRUE;
     /* NAS is sending SIM_UPDATE_REQ without new keys then also we treat them as new keys and send IDT start value as 0.
       Earlier we handled this scenario for SRVCC only but missed for new_key_for_start_value flag
    */
     if (((rrc_state == RRC_STATE_CELL_DCH) ||
          (rrc_state == RRC_STATE_CELL_FACH))
#ifdef FEATURE_LTE_TO_WCDMA
         && (!((sim_update_req_ptr->security_update_info.security_update_status 
            == RRC_SIM_UPDATE_VALUE_LTE_MAPPED) && (RRC_PS_DOMAIN_CN_ID == domain_id)
         && (integrity_protection_info.ip_status == RRCSMC_SECURITY_STARTED )))
#endif
      )
      {
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.
        new_key_for_start_value = TRUE;
      }
      /*copy contents of integrity_key to int_ciph_info struct for this CN DOMAIN
       */
      WCDMA_MEMCPY(local_int_ciph_info_ptr->int_ciph_info[domain_id].integrity_key,
                   sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                   sim_update_req_ptr->security_update_info.ciphering_info.integrity_key,
                   RRC_MAX_INTEGRITY_KEY_LENGTH);
  
	  /* If integrity is not started yet, make sure we copy the received key to key_old also so that key_old doesnt have stale keys*/
	  if ( integrity_protection_info.ip_status == RRCSMC_SECURITY_NOT_STARTED )
	  {
	    WCDMA_MEMCPY(local_int_ciph_info_ptr->int_ciph_info[domain_id].integrity_key_old,
                    sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                    sim_update_req_ptr->security_update_info.ciphering_info.integrity_key,
                    RRC_MAX_INTEGRITY_KEY_LENGTH);
	  }
  
      /*Print integrity key here*/
      PRINT_CIPHER_INT_KEY(RRC_PRINT_INTEGRITY_KEY,sim_update_req_ptr->security_update_info.
                                  ciphering_info)
  
      /*Print ciphering key here*/
      PRINT_CIPHER_INT_KEY(RRC_PRINT_CIPHERING_KEY,sim_update_req_ptr->security_update_info.
                                  ciphering_info)
  
    }/*if only security info has changed*/
  }
}/*rrcsmc_update_integrity_ciphering_info*/
/*===========================================================================

FUNCTION rrcsmc_extract_latest_start_list_per_cn_domain

DESCRIPTION
  Extract the latest_start_list from count_c_i for both CS and PS
  domains.  START value = 20 MSB of count_c_i for both domains

DEPENDENCIES
  None.

RETURN VALUE
  None.
===========================================================================*/
static void rrcsmc_extract_latest_start_list_per_cn_domain
(
rrc_cn_domain_identity_e_type domain_id,
rrcsmc_start_list_update_e_type start_update
)
{
  uint32 max_start=0, prev_max_start, init_seq_num=0;
  byte   local_start_value[RRC_MAX_START_LENGTH] = {0x00,0x00,0x00};
 

  //get the max start value and convert it to count_c_i type
  max_start = rrcsmc_get_max_start_per_domain(domain_id);

  if(domain_id == RRC_CS_DOMAIN_CN_ID)
  {
    if((start_value_to_transmit_cs_hho) &&
        (start_value_to_transmit_cs_hho >= max_start))
    {
      WRRC_MSG2_HIGH("Updating max_start:0x%x with SVTHHO:0x%x",
        max_start, start_value_to_transmit_cs_hho);
      max_start = start_value_to_transmit_cs_hho;
    }
    /* Reset SVTHHO */
    start_value_to_transmit_cs_hho = 0;
  }

  if(START_THRESHOLD == max_start)
  {
    //don't increment the start value, ref TS25.331 ch 8.5.9
    WRRC_MSG1_HIGH("Max Start Value=0x%x reached the Threshold",max_start);
  }  
  else
  {
    /*increment by 2*/
    max_start += 2;    
  }  
  max_start <<= 12;

  /*get START value stored previously for this CN domain and convert it to 
  count_i with seq_num 0*/
  if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id] == TRUE)
  {
    prev_max_start = 
    rrcsmc_convert_START_to_count_c_i(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest, 
                                      init_seq_num);
  }
  else
  {
    prev_max_start = 
    rrcsmc_convert_START_to_count_c_i(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn, 
                                      init_seq_num);
  }

  /*if the previously stored START value is greater than the currently 
  extracted START value then do not change the latest_start value.
  this can happen if all the count_i's in both uplink and downlink for
  this domain have rolled over*/
  if (prev_max_start <= max_start)
  {
    rrcsmc_convert_count_c_i_to_START(local_start_value, max_start);

    if(start_update == RRCSMC_UPDATE_START_LIST)
    {
      /*copy the contents of local_start_value to latest_start_list for this
        cn domain*/
      WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
                   sizeof(byte) * RRC_MAX_HFN_LENGTH,
                   local_start_value,        
                   RRC_MAX_START_LENGTH);
    }
    else
    {
      /*copy the contents of local_start_value to latest_start_list for this
        cn domain*/
      WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,
                   sizeof(byte) * RRC_MAX_HFN_LENGTH,
                   local_start_value,        
                   RRC_MAX_START_LENGTH);
    }
  }
  else
  {
    if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id] == FALSE)
    {
      /*sync up HFN_LATEST to HFN as start_value_update_flag is not set*/
      WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,
                   sizeof(byte) * RRC_MAX_HFN_LENGTH,
                   rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,        
                   RRC_MAX_START_LENGTH);
    }
  }
}/*rrcsmc_extract_latest_start_list_from_count_c_i*/
/*===========================================================================

FUNCTION rrcsmc_append_hfns_to_rel_ind

DESCRIPTION
  This procedure appends the CS and PS domain HFNs to RRC_REL_IND message
  ptr which will be sent to MM.

DEPENDENCIES
  None.

RETURN VALUE
  Returns SUCCESS if PS and CS HFNs are successfully appended to rrc_rel_ind_type.
  Returns SUCCESS if PS and CS HFNs are not appended to rrc_rel_ind_type.


SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_hfns_to_rel_ind 
(
byte *hfncs, 
byte *hfnps
)
{
  /*if Integrity has already started, then we have to extract the START values
    from COUNT_C and COUNT_I values according to 8.5.9 of 25.331 v3a0*/
  if (integrity_protection_info.ip_status != RRCSMC_SECURITY_STARTED)
  {
    /*if STATUS is not started then we never started Integrity Prot.
        we were in connected state only to do location update etc.
        so init integrity_protection_info with the recieved 
        integrity_ciphering_config and extract the latest start list*/
    rrcsmc_init_integrity_protection_info();
  }

  extract_cause_rel_ind=TRUE;
  if(rrcsmc_int_ciph_info.int_start_status_for_conn[RRC_CS_DOMAIN_CN_ID])
  {
    rrcsmc_extract_latest_start_list_per_cn_domain(RRC_CS_DOMAIN_CN_ID,
                                                   RRCSMC_UPDATE_START_LIST);  

  }

  if(rrcsmc_int_ciph_info.int_start_status_for_conn[RRC_PS_DOMAIN_CN_ID])
  {
    rrcsmc_extract_latest_start_list_per_cn_domain(RRC_PS_DOMAIN_CN_ID,
                                                   RRCSMC_UPDATE_START_LIST);     
  }

  extract_cause_rel_ind=FALSE;

  /* Before copying the calculated Start value to the REL_IND,check to see
     if there are any new keys pending.If new keys are pending,then we 
     is supposed to write 0 to the SIM.One way to check is to see the pending
     flag of SRB2 because this flag is set on SIM_UPDATE_REQ and is reset on 
     the receipt of SMC,since SMC always comes with the new Integrity config */

  if(integrity_protection_info.count_i_info[RRC_CS_DOMAIN_CN_ID].
     dl_new_key_flag_per_rb[DCCH_AM_RADIO_BEARER_ID] == TRUE)
  {
    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn[0]=0x0;
    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn[1]=0x0;
    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn[2]=0x0;
    WRRC_MSG0_HIGH("New Keys pending for CS domain.Setting HFN to zero to write to SIM");
  }

  if(integrity_protection_info.count_i_info[RRC_PS_DOMAIN_CN_ID].
     dl_new_key_flag_per_rb[DCCH_AM_RADIO_BEARER_ID] == TRUE)
  {
    rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].hfn[0]=0x0;
    rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].hfn[1]=0x0;
    rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].hfn[2]=0x0;
    WRRC_MSG0_HIGH("New Keys pending for PS domain.Setting HFN to zero to write to SIM");
  }

  if(saved_security_key_set.pending_cs_key_to_be_activated)
  {
    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn[0]=0x0;
    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn[1]=0x0;
    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn[2]=0x0;
    rrcsmc_reset_pending_cs_key_to_be_activated();
    MSG_HIGH("New Keys pending for CS domain during WtoG handover so resetting HFN to zero",0,0,0);
  }

  /*copy the contents of the latest_start_list just extracted into hfncs and
    hfnps byte ptrs received.*/
  WCDMA_MEMCPY(hfncs,
               RRC_MAX_START_LENGTH,
               rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn,
               RRC_MAX_START_LENGTH);

  WCDMA_MEMCPY(hfnps,
               RRC_MAX_START_LENGTH,
               rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].hfn,
               RRC_MAX_START_LENGTH);      

  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"REL CS START: 0x%x 0x%x 0x%x REL PS START: 0x%x 0x%x 0x%x",
  hfncs[0], hfncs[1], hfncs[2],hfnps[0], hfnps[1], hfnps[2]	);

  /*since we are going to idle mode, set the integrity started flag to
    NOT_STARTED*/
  integrity_protection_info.ip_status = RRCSMC_SECURITY_NOT_STARTED;        
  rrcsmc_init_integrity_protection_info();

  rrcsmc_go_to_smc_initial_substate();
  MSG_MED("SMC going to INITIAL_STATE",0,0,0);

  return SUCCESS;
}/*rrcsmc_append_hfns_to_rel_ind*/

/*===========================================================================

FUNCTION rrcsmc_check_new_keys_and_reset_append_start_per_domain

DESCRIPTION
  This function checks whether RRC received new keys from NAS and accordingly
  initializes the a byte *value with START values for the domain requested.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_check_new_keys_and_reset_append_start_per_domain 
(
byte  *value,
rrc_cn_domain_identity_e_type domain_id,
rrcsmc_append_start_e_type start_type
)
{

  if(TRUE == rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.new_key_for_start_value)
  {
    /* Consider the case of SMC being initiated and new keys are received from MM.
     * Before UE gets L2 ACK for SM complete, CU procedure started (because of cell reselction)
     * So SMC will be aborted as per spec section 8.1.12.4b and UE reverted to old configuration including IK
     * In this case UE has to sent START = 0x0 so that UTRAN uses this START to initialize HFN = 0x0 in Count_I 
     */
    WRRC_MSG1_HIGH("new_key_for_start_value is TRUE for domain:%d Set START = 0x00 0x00 0x00", domain_id);
    /* This flag will be set to FALSE when SMC arrives and rcvd_new_keys_flag == TRUE */

    memset(value, 0x0,RRC_MAX_START_LENGTH);

    if(domain_id == RRC_CS_DOMAIN_CN_ID)
    {
      MSG_HIGH("Appended CS-START: 0x%x 0x%x 0x%x", value[0], value[1], value[2]);
    }
    else
    {
      MSG_HIGH("Appended PS-START: 0x%x 0x%x 0x%x", value[0], value[1], value[2]);
    }  
  }
  else
  {
    rrcsmc_append_start_per_domain(value,
             domain_id,
             start_type);
  }
}/*rrcsmc_append_start_per_domain*/


/*===========================================================================

FUNCTION rrcsmc_append_start_per_domain

DESCRIPTION
  This function initializes the a byte ptr with START values for the domain
  requested.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_append_start_per_domain 
(
byte  *value,
rrc_cn_domain_identity_e_type domain_id,
rrcsmc_append_start_e_type start_type
)
{
  //if SMC is not received, start should never be re-calculated since 
  //count_c and count_i are not valid till a SMC is received..
  //so just return the START value read from the SIM
  if(rrcsmc_int_ciph_info.int_start_status_for_conn[domain_id] == FALSE)
  {
    if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id] == TRUE)
    {
      /*There might be a race condition in the N/W where it forgets to start ciphering 
       *and or modify integrity for a domain & its very much possible that we can have 
       *the call for this domain gets released, Start value gets updated here are part of 
       *RB establishment procedure. On the same RRC connection we may have another 
       *signaling connection establish. To have Start values in sync for IDT & the one used 
       *in processing for SMC we need to check & use updated start value if available.
       */
      WCDMA_MEMCPY(value,
                   RRC_MAX_START_LENGTH,
                   rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,
                   RRC_MAX_START_LENGTH);
    }
    else
    {
      WCDMA_MEMCPY(value,
                   RRC_MAX_START_LENGTH,
                   rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,  
                   RRC_MAX_START_LENGTH);         
    }
  }
  //otherwise, START needs to be re-calculated based on whether the caller
  //is requesting new START to be extracted and calculated or just the old
  //start needs to be sent..
  else
  {
    if(start_type == RRCSMC_EXTRACT_AND_APPEND_START)
    {
      rrcsmc_extract_latest_start_list_per_cn_domain(domain_id,
                                                     RRCSMC_UPDATE_START_LIST_LATEST);
  
      /*copy the contents of latest_start_list into start_list*/
      WCDMA_MEMCPY(value,
                   RRC_MAX_START_LENGTH,
                   rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,  
                   RRC_MAX_START_LENGTH);         
  
      rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = TRUE;
    }
    else
    {
      //if start value is updated, then send start value from hfn_latest
      if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id] == TRUE)
      {
        /*copy the contents of hfn_latest into start_list*/
        WCDMA_MEMCPY(value,
                     RRC_MAX_START_LENGTH,
                     rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,    
                     RRC_MAX_START_LENGTH);         
      }
      else
      {
        /*copy the contents of hfn into start_list*/
        WCDMA_MEMCPY(value,
                     RRC_MAX_START_LENGTH,
                     rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,   
                     RRC_MAX_START_LENGTH);         
      }
    }
  }

  if(domain_id == RRC_CS_DOMAIN_CN_ID)
  {
    MSG_HIGH("Appended CS-START: 0x%x 0x%x 0x%x", value[0], value[1], value[2]);
  }
  else
  {
    MSG_HIGH("Appended PS-START: 0x%x 0x%x 0x%x", value[0], value[1], value[2]);
  }
}/*rrcsmc_append_start_per_domain*/
/*===========================================================================

FUNCTION rrcsmc_append_start_list

DESCRIPTION
  This function initializes the START_LIST with the latest values to 
  be sent to the UTRAN by the RCE, CU  and ASU procedures. The START
  values for all CN domains for which we have received Integrity
  Protection Info will be sent.
  
  RCE, CU and ASU procedures will call this function in SMC.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns SUCCESS if STARTlist is appended.
  This function returns FAILURE if STARTlist is not appended.

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_start_list 
(
 rrc_STARTList *start_list_ext,
rrcsmc_append_start_e_type start_type
)
{
  uecomdef_status_e_type status;

  status =  rrcsmc_append_start_value_to_start_list (start_list_ext,
                                                     start_type
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
                                                     ,TRUE                                         
                                                     ,RRC_CS_DOMAIN_CN_ID            /*dummy value. Doesn't matter b'coz both 
                                                                                                      *domain are considered to be open anyways*/
#endif
                                                      );

  return status;
}/*rrcsmc_append_start_list*/
/*===========================================================================

FUNCTION rrcsmc_send_sync_ind_to_mm

DESCRIPTION
   This function processes SMC proecdure in wait for l2 ack state.  We can
   be waiting for an L2 ACK for a SMC complete command

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_send_sync_ind_to_mm
(
rrc_sync_cause_e_type sync_cause
)
{
  mm_cmd_type *mm_cmd_ptr;                      /* Pointer to MM command */

  /*set the reconfig flag to false as we got the l2_ack for Security Complete*/
  integrity_protection_info.reconfig        = FALSE;

  if(rrcidt_is_idt_waiting_for_l2ack_per_domain(rrcsmc_int_ciph_info.latest_config_cn_id))
  {
    WRRC_MSG1_HIGH("IDT L2_ACK after SYNC_IND for %d",
             rrcsmc_int_ciph_info.latest_config_cn_id);

    rrcidt_send_open_session_cnf(RRC_OPEN_SESS_SUCCESS,
                                 rrcsmc_int_ciph_info.latest_config_cn_id);

  }

  /* Allocate memory for RRC_SYNC_IND command */

    mm_cmd_ptr =  mm_rrc_get_cmd_buf(RRC_SYNC_IND);
    
#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND  
    #error code not present
#endif
    {
      /* No paramters to fill in for RRC_SYNC_IND command */

      WRRC_MSG1_HIGH("Dispatching out MM cmd / RRC_SYNC_IND is sent %d", rrcsmc_int_ciph_info.
                 latest_config_cn_id);

      mm_cmd_ptr->cmd.rrc_sync_ind.cn_domain_id = 
        rrcsmc_int_ciph_info.latest_config_cn_id;
    }
    
#ifdef FEATURE_DUAL_SIM
    mm_cmd_ptr->cmd.rrc_sync_ind.as_id = rrc_get_as_id();
#endif

    mm_cmd_ptr->cmd.rrc_sync_ind.rab_id_present = FALSE;
    mm_cmd_ptr->cmd.rrc_sync_ind.cause = sync_cause;

#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
    #error code not present
#endif

    rrc_send_rrc_message_to_mm(mm_cmd_ptr);            /* Sends the command to MM */

    rrcsmc_int_ciph_info.is_sync_ind_sent_to_nas[rrcsmc_int_ciph_info.latest_config_cn_id] = TRUE;
    MSG_HIGH("Latest CN ID=%d,sync_ind_sent status for CS=%d,PS=%d",
        rrcsmc_int_ciph_info.latest_config_cn_id,
        rrcsmc_int_ciph_info.is_sync_ind_sent_to_nas[RRC_CS_DOMAIN_CN_ID],
        rrcsmc_int_ciph_info.is_sync_ind_sent_to_nas[RRC_PS_DOMAIN_CN_ID]);

  }
/*===========================================================================

FUNCTION rrcsmc_process_dl_integrity_check_info

DESCRIPTION
  This function retrieves the MAC and SEQ_NO from the integrity_check_info
  Then populates MAC field to RB_ID on which the message was received and
  the SEQ_NO to zero.

  In downlink when we receive Integrity Check Info, we first extract the
  MAC and SEQ_NO from Integrity Check Info struct.  Then MAC is assigned
  to RB_ID of the received message and SEQ_NUM field is init to zero before
  we run it through the f9 engine.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns SUCCESS if Integrity Check Info is present and 
  int_check_info is populated with mac and seq. number
  Returns FAILURE if Integrity Check Info is not present.

SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_status_e_type rrcsmc_process_dl_integrity_check_info
(
byte* dl_sdu_ptr,                         /* encoded byte stream*/
uint32 dl_sdu_length,                     /* Length of SDU received */
rrcsmc_integrity_check_info_type * int_check_info,/*integrity check info struct*/
rrc_RB_Identity rb_id                     /*rb_id of the received message*/
)
{
  /* Local Variable to store mac type position, seq num position*/
  uint32 mac_type_pos, sn_type_pos;                     

  /*local var to init Message bit stream with sequence number set to 0*/
  uint32 seq_num_zero=0x0;

  /* Check whether Integrity Check info is present */
  if (0x00 == b_unpackb(dl_sdu_ptr, INTEGRITY_CHECK_INFO_PRESENT_POS,               
                        NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
  {
    WRRC_MSG0_HIGH("Integrity Check Info missing");
    return FAILURE;
  }
  /* The integrity Check Info is present, Skip 36 bits for message type */
  else if ( 0x01 == b_unpackb(dl_sdu_ptr, INTEGRITY_CHECK_INFO_PRESENT_POS, 
                              NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
  {
    /* Check if there are enough bits to peek into the message type */
    if ( (dl_sdu_length * 8) < 
         (NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
          NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO) )
    {
      WRRC_MSG0_HIGH("Invalid Msg Len.");
      return FAILURE;
    }

    /*peek into byte stream and get the MAC */
    mac_type_pos = INTEGRITY_CHECK_INFO_PRESENT_POS+
                   NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT;

    int_check_info->msg_auth_code =
    b_unpackd(dl_sdu_ptr, (uint16)mac_type_pos, RRC_MAC_LENGTH_IN_BITS);

    /*peek into byte stream and get the Sequence Number */
    sn_type_pos =  INTEGRITY_CHECK_INFO_PRESENT_POS +
                   NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
                   RRC_MAC_LENGTH_IN_BITS;

    int_check_info->msg_seq_num=b_unpackb(dl_sdu_ptr, (uint16)sn_type_pos, 
                                          RRC_SEQ_NUM_LENGTH_IN_BITS);


    /*pack mac with rb_id */
    mac_type_pos = INTEGRITY_CHECK_INFO_PRESENT_POS + 
                   NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT;

    b_packd(rb_id, dl_sdu_ptr, (uint16)mac_type_pos, RRC_MAC_LENGTH_IN_BITS);

    /*pack seq num with 0's */
    sn_type_pos =  INTEGRITY_CHECK_INFO_PRESENT_POS +
                   NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
                   RRC_MAC_LENGTH_IN_BITS;

    b_packb((uint8)seq_num_zero, dl_sdu_ptr, (uint16)sn_type_pos, RRC_SEQ_NUM_LENGTH_IN_BITS);

    return SUCCESS;
  }

  /*we should never come here.  for future use*/
  return FAILURE;
}/*rrcsmc_process_dl_integrity_check_info*/

/*===========================================================================

FUNCTION  rrcsmc_get_rlc_ul_info

DESCRIPTION
  This function extracts the hfn and ciph_act_time for lc id

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
static void rrcsmc_get_rlc_ul_info
(
  rlc_hfn_req_type  *local_hfn_type,
  rlc_lc_id_type    lc_id,
  uint32            *hfn,
  uint16            *ciph_sn,
  boolean           *hfn_valid
)
{
  uint32 i=0;
  *hfn = 0xFFFFFFFF;
  *ciph_sn = 0xFFFF;
  *hfn_valid = FALSE;
  
  for(i=0; i<local_hfn_type->ul_nchan; i++)
  {
    if(local_hfn_type->hfn_per_rb_ul[i].rlc_id == lc_id)
    {
      *hfn = local_hfn_type->hfn_per_rb_ul[i].hfn;  
      *ciph_sn = local_hfn_type->hfn_per_rb_ul[i].rlc_sn;
      *hfn_valid = local_hfn_type->hfn_per_rb_ul[i].hfn_valid;
      break;
    }
  }
}

/*===========================================================================

FUNCTION  rrcsmc_get_rlc_dl_info

DESCRIPTION
  This function extracts the hfn and ciph_act_time for lc id

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
static void rrcsmc_get_rlc_dl_info
(
  rlc_hfn_req_type  *local_hfn_type,
  rlc_lc_id_type    lc_id,
  uint32            *hfn,
  uint16            *ciph_sn,
  boolean           *hfn_valid
)
{
  uint32 i=0;
  *hfn = 0xFFFFFFFF;
  *ciph_sn = 0xFFFF;
  *hfn_valid = FALSE;
  
  for(i=0; i<local_hfn_type->dl_nchan; i++)
  {
    if(local_hfn_type->hfn_per_rb_dl[i].rlc_id == lc_id)
    {
      *hfn = local_hfn_type->hfn_per_rb_dl[i].hfn;  
      *ciph_sn = local_hfn_type->hfn_per_rb_dl[i].rlc_sn;
      *hfn_valid = local_hfn_type->hfn_per_rb_dl[i].hfn_valid;
      
      break;
    }
  }
}

/*===========================================================================

FUNCTION  rrcsmc_revert_to_old_cipher_config_db

DESCRIPTION
  This function reverts the value of global variable 'cipher_config_db' from its back-up value.
  This function is supposed to be called whenever RRC has to revert back to old configuration
  during HHO(timing initialized) or SRNS relocation. 

  NOTE: Make sure that "cipher_config_db" value is backed-up before this function is 
        called. By default backup variable will have junk value (its value is never cleared) 

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_revert_to_old_cipher_config_db
(
  void
)
{
  uint32 i=0, hfn=0;
  uint16  ciph_act_sn;  
  rlc_hfn_req_type *local_hfn_type_ptr;
  boolean hfn_valid = FALSE;
  local_hfn_type_ptr = (rlc_hfn_req_type *)rrc_malloc(sizeof(rlc_hfn_req_type));

  WRRC_MSG2_HIGH("Reverting to back-up of cipher_config_db. Before reverting MAC-config-type:(CS : %d, PS: %d) ",
    cipher_config_db.mac_cipher_info[CS_DOMAIN].cipher_cfg_type,
    cipher_config_db.mac_cipher_info[PS_DOMAIN].cipher_cfg_type);
  /* Taking back up of global variable which is used to send ciphering configuraiton to MAC */
  cipher_config_db = backup_of_security_variables.cipher_config_db;

  memset(local_hfn_type_ptr, 0, sizeof(rlc_hfn_req_type));
  (void) rlc_rrc_get_hfn_req(local_hfn_type_ptr);

  for(i=0; i<cipher_config_db.rlc_ul_cipher_info.nchan; i++)
  {
    hfn = 0;
    ciph_act_sn = 0;
    hfn_valid = FALSE;

    rrcsmc_get_rlc_ul_info(local_hfn_type_ptr, 
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].lc_id, 
      &hfn,
      &ciph_act_sn,
      &hfn_valid);

    if((hfn == 0xFFFFFFFF) && (hfn_valid == FALSE))
    {
      continue;
    }
    
    /* If sn at RLC > RRC then activation time already hit. Send act sn as Immediate. */
    if(cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_act_sn <= ciph_act_sn)
    {
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_act_sn = ciph_act_sn;
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].hfn = hfn;
    }
   /* If sn at RLC < RRC then two cases arrive
    *  1. Roll over happened. In this case HFN at RRC and RLC are NOT equal. Send act sn as immediate
    *     Change the HFN to the latest one that RLC has returned.
    *  2. Configuration is pending. In this case HFN at RRC and RLC are equal.
    *     No need to change anything. Backup of cipher_config_db will take care.
    */  
    else if(cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_act_sn > ciph_act_sn)
    {
      //if hfn at RLC and RRC are NOT equal then it is roll over case. Need to apply immediate config
      if((cipher_config_db.rlc_ul_cipher_info.ciphering[i].hfn != hfn) &&
         (hfn_valid == TRUE))
      {
        cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_act_sn = ciph_act_sn;
        cipher_config_db.rlc_ul_cipher_info.ciphering[i].hfn = hfn;
      }
    }

    MSG_HIGH("UL_DBG:Cipher DB: LC:%d HFN:0x%x SN:%d",cipher_config_db.rlc_ul_cipher_info.ciphering[i].lc_id,
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].hfn, 
      cipher_config_db.rlc_ul_cipher_info.ciphering[i].ciph_act_sn);
    
  }

  for(i=0; i<cipher_config_db.rlc_dl_cipher_info.nchan; i++)
  {
    hfn = 0;
    ciph_act_sn = 0;
    hfn_valid = FALSE;

    rrcsmc_get_rlc_dl_info(local_hfn_type_ptr, 
      cipher_config_db.rlc_dl_cipher_info.ciphering[i].lc_id,
      &hfn,
      &ciph_act_sn,
      &hfn_valid);
     
    if((hfn == 0xFFFFFFFF) && (hfn_valid == FALSE))
    {
      continue;
    }
    
    /* If sn at RLC > RRC then activation time already hit. Send act sn as Immediate. */
    if(cipher_config_db.rlc_dl_cipher_info.ciphering[i].ciph_act_sn <= ciph_act_sn)
    {
      cipher_config_db.rlc_dl_cipher_info.ciphering[i].ciph_act_sn = ciph_act_sn;
      cipher_config_db.rlc_dl_cipher_info.ciphering[i].hfn = hfn;
    }
    else if(cipher_config_db.rlc_dl_cipher_info.ciphering[i].ciph_act_sn > ciph_act_sn)
    {
      //if hfn at RLC and RRC are NOT equal then it is roll over case. Need to apply immediate config
      if((cipher_config_db.rlc_dl_cipher_info.ciphering[i].hfn != hfn) &&
         (hfn_valid == TRUE))
      {
        cipher_config_db.rlc_dl_cipher_info.ciphering[i].ciph_act_sn = ciph_act_sn;
        cipher_config_db.rlc_dl_cipher_info.ciphering[i].hfn = hfn;
      }
    }

    MSG_HIGH("DL_DBG:Cipher DB: LC:%d HFN:0x%x SN:%d",cipher_config_db.rlc_dl_cipher_info.ciphering[i].lc_id,
      cipher_config_db.rlc_dl_cipher_info.ciphering[i].hfn, 
      cipher_config_db.rlc_dl_cipher_info.ciphering[i].ciph_act_sn);    
  }
  rrc_free(local_hfn_type_ptr);
}

/*===========================================================================

FUNCTION  rrcsmc_backup_cipher_config_db

DESCRIPTION
  This function back-up the global variable 'cipher_config_db'. This function
  is supposed to be called immediately before updating this variable for
  STEP1 cipher config for HHO.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_backup_cipher_config_db
(
  void
)
{

  WRRC_MSG2_HIGH("Taking back-up of cipher_config_db. Old MAC-config-type:(CS : %d, PS: %d) ",
    cipher_config_db.mac_cipher_info[CS_DOMAIN].cipher_cfg_type,
    cipher_config_db.mac_cipher_info[PS_DOMAIN].cipher_cfg_type);
  /* Taking back up of global variable which is used to send ciphering configuraiton to MAC */
  backup_of_security_variables.cipher_config_db = cipher_config_db;

}

/*===========================================================================

FUNCTION  rrcsmc_backup_global_security_variables

DESCRIPTION
  This function back-up all security related global variables. This function
  is supposed to be called before processing any config/re-config request.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_backup_global_security_variables
(
  void
)
{
  /* Taking back up of global variable which is used to send ciphering configuraiton to MAC */
  backup_of_security_variables.ciphering_info = ciphering_info;
  backup_of_security_variables.cipher_config_db = cipher_config_db;
  backup_of_security_variables.ciph_rb_info = ciph_rb_info;
  backup_of_security_variables.integrity_protection_info = integrity_protection_info;
  backup_of_security_variables.new_tm_rb_ind_flag = NEW_TM_RB_IND_FLAG;
  backup_of_security_variables.rrcsmc_int_ciph_info = rrcsmc_int_ciph_info;
  backup_of_security_variables.start_value_to_transmit_flag = START_VALUE_TO_TRANSMIT_FLAG;
  /* reset the global variable */
  is_revert_to_old_cipher_variable_reqd = FALSE;
}

/*===========================================================================

FUNCTION  rrcsmc_revert_to_old_security_variables

DESCRIPTION
  This function reverts the security variables to their back-up values. This
  function will also take care of 

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_revert_to_old_security_variables
(
  void
)
{
  rrc_cn_domain_identity_e_type local_cn_domain;
  uint32 new_sn, old_sn, old_ul_count_i;
  rrcsmc_ciphering_config_info_type *temp_cipher_config_db_ptr = (rrcsmc_ciphering_config_info_type *)rrc_malloc(sizeof(rrcsmc_ciphering_config_info_type));
  rrcsmc_security_config_info_type *temp_rrcsmc_int_ciph_info_ptr = (rrcsmc_security_config_info_type *)rrc_malloc(sizeof(rrcsmc_security_config_info_type));
 
  /* LLC might have processed the RRC_CIPH_CONFIG_REQ, and RRC_CIPH_CONFIG_CNF
   * might be present in the cmd queue when cell update is triggered. Hence, 
   * check whether revert back to old ciphering variables is required or not. 
   * If LLC is not in suspended state then it indicates that LLC has already sent
   * RRC_CIPH_CONFIG_CNF and it is in queue if smc substate is still rrcsmc-wait-
   * for-cipher-cnf, in this case act as if all the global variables are updated
   * and safe to revert back to old configuration.
   */  
  if ((RRCSMC_WAIT_FOR_CIPH_CONFIG_CNF != rrcsmc_substate) || 
      (TRUE == is_revert_to_old_cipher_variable_reqd)
     )
  {
    /* is_revert_to_old_cipher_variable_reqd set to TRUE indicates that integrity
        variables are already reverted back, so no need to revert back the integrity
        variables else if it is set to FALSE then it means that all the gloabal 
        variables are safe to revert back to old config */
    if (FALSE == is_revert_to_old_cipher_variable_reqd)
    {
      /* There is a possibility that RRC might have already sent number of messages
         on SRB 2 (say measurement reports) before L2 ack, RRC should not send 
         messages with any SN for which RRC message is already been sent. Hence, 
         move the RRC-SN in backed-up security variable to current SN for SRB-2. 
         Special care needs to be taken for roll-over case.
    
         NOTE: SRBs (other than SRB2) are not taken care to avoid extra complexity
                   of handling corner scenarios like activation time expiry etc for those SRBs.
                   SRB2 always has activation time now, hence no need to look for activation
                   time expiry and possibility of messages going on SRB2 is more than any other SRBs.
      */
      local_cn_domain = rrcsmc_int_ciph_info.latest_config_cn_id;
    
      new_sn = integrity_protection_info.count_i_info[local_cn_domain].ul_count_i[2] & 0xF;
    
      local_cn_domain = backup_of_security_variables.rrcsmc_int_ciph_info.latest_config_cn_id;
    
      old_ul_count_i = backup_of_security_variables.integrity_protection_info.
                       count_i_info[local_cn_domain].ul_count_i[2];
      old_sn =  old_ul_count_i & 0xF;
    
      /* Assumption: RRC wont send more than 15 RRC Messages on on SRB2 before L2-Ack is 
          received for SM Complete message */
      old_ul_count_i = (((old_ul_count_i >> 4) + (uint8)(new_sn < old_sn))<<4) | new_sn;
    
      
      MSG_HIGH("Reverting to old Integrity variables. cnt-i-SRB2: prev: %d, cur: %d, after: %d", 
        backup_of_security_variables.integrity_protection_info.
                                       count_i_info[local_cn_domain].ul_count_i[2], 
           integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].ul_count_i[2], old_ul_count_i);
    
      /* updating count-I for SRB2 appropriately with new-SN */
      backup_of_security_variables.integrity_protection_info.
                                       count_i_info[local_cn_domain].ul_count_i[2] = old_ul_count_i;
  
      /* save rrcsmc_int_ciph_info received in last security mode command */
      *temp_rrcsmc_int_ciph_info_ptr = rrcsmc_int_ciph_info;
  
      rrcsmc_int_ciph_info = backup_of_security_variables.rrcsmc_int_ciph_info;

      /* copy the saved rrcsmc_int_ciph_info to backup variable */
      backup_of_security_variables.rrcsmc_int_ciph_info = *temp_rrcsmc_int_ciph_info_ptr;

      integrity_protection_info = backup_of_security_variables.integrity_protection_info;
    }

    WRRC_MSG0_HIGH("Reverting to old Ciphering variables");

    /* Save the value of current cipher_config_db because this will be put into backup_* 
       structure. Variable backup.cipher_config_db will be used while sending ABORT_CIPH
       to RLC */
    *temp_cipher_config_db_ptr = cipher_config_db;
  
    ciphering_info = backup_of_security_variables.ciphering_info;
    cipher_config_db =  backup_of_security_variables.cipher_config_db;
    ciph_rb_info =  backup_of_security_variables.ciph_rb_info;
    NEW_TM_RB_IND_FLAG = backup_of_security_variables.new_tm_rb_ind_flag;
    START_VALUE_TO_TRANSMIT_FLAG = backup_of_security_variables.start_value_to_transmit_flag;
  
    /* restoring the value of cipher_config_db in backup variable - this may not be optimal
       but seems easier way of doing it */
    backup_of_security_variables.cipher_config_db = *temp_cipher_config_db_ptr;
  }
  else
  {
    /* Reverting of integrity variable is requried because cell update goes out of RRC
       before this function is called....we can modularize following by putting into a function */

    /* There is a possibility that RRC might have already sent number of messages
       on SRB 2 (say measurement reports) before L2 ack, RRC should not send 
       messages with any SN for which RRC message is already been sent. Hence, 
       move the RRC-SN in backed-up security variable to current SN for SRB-2. 
       Special care needs to be taken for roll-over case.
  
       NOTE: SRBs (other than SRB2) are not taken care to avoid extra complexity
             of handling corner scenarios like activation time expiry etc for those SRBs.
             SRB2 always has activation time now, hence no need to look for activation
             time expiry and possibility of messages going on SRB2 is more than any other SRBs.
    */
    local_cn_domain = rrcsmc_int_ciph_info.latest_config_cn_id;
  
    new_sn = integrity_protection_info.count_i_info[local_cn_domain].ul_count_i[2] & 0xF;
  
    local_cn_domain = backup_of_security_variables.rrcsmc_int_ciph_info.latest_config_cn_id;
  
    old_ul_count_i = backup_of_security_variables.integrity_protection_info.
                     count_i_info[local_cn_domain].ul_count_i[2];
    old_sn =  old_ul_count_i & 0xF;
  
    /* Assumption: RRC wont send more than 15 RRC Messages on on SRB2 before L2-Ack is 
        received for SM Complete message */
    old_ul_count_i = (((old_ul_count_i >> 4) + (uint8)(new_sn < old_sn))<<4) | new_sn;
  
    
    MSG_HIGH("Reverting to old Integrity variables. cnt-i-SRB2: prev: %d, cur: %d, after: %d | Setting is_revert_to_old_cipher_variable_reqd to TRUE", 
        backup_of_security_variables.integrity_protection_info.
                                     count_i_info[local_cn_domain].ul_count_i[2], 
         integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].ul_count_i[2], old_ul_count_i);
  
    /* updating count-I for SRB2 appropriately with new-SN */
    backup_of_security_variables.integrity_protection_info.
                                     count_i_info[local_cn_domain].ul_count_i[2] = old_ul_count_i;
 
    /* save rrcsmc_int_ciph_info received in last security mode command */
    *temp_rrcsmc_int_ciph_info_ptr = rrcsmc_int_ciph_info;

    rrcsmc_int_ciph_info = backup_of_security_variables.rrcsmc_int_ciph_info;

    /* copy the saved rrcsmc_int_ciph_info to backup variable */
    backup_of_security_variables.rrcsmc_int_ciph_info = *temp_rrcsmc_int_ciph_info_ptr;

    integrity_protection_info = backup_of_security_variables.integrity_protection_info;

    /* Avoid reverting to old secrutiy global variables because there is a possiblity
       that all the global variable are not updated at this point. Hence wait till global
       variables are updated */
    is_revert_to_old_cipher_variable_reqd = TRUE;
  }
  rrc_free(temp_cipher_config_db_ptr);
  rrc_free(temp_rrcsmc_int_ciph_info_ptr);
  
}

/*===========================================================================

FUNCTION rrcsmc_step_2_cipher_config_for_first_tm_rb_setup

DESCRIPTION
  Process Ciph Config Cnf for first TM RBs being setup

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
uint32 rrcsmc_step_2_cipher_config_for_first_tm_rb_setup
( 
void
)
{
  uint32 step_2_start, step_2_ciph_act_time, step_2_hfn;
  
  /*256 is the CFN rollover length*/
  /*250 is used to activate ciphering after 250 10ms time interval,
    this should give sufficient time for n/w side and ue side to have
    configured ciphering on the rb's successfully*/
  step_2_ciph_act_time = (uint8)((seq_get_cfn() + 250) % 256);
  /*round it to 80ms boundary */
  step_2_ciph_act_time = (step_2_ciph_act_time/8)*8; 
  

  //set ul/dl rlc cipher info to 0's
  cipher_config_db.rlc_ul_cipher_info.nchan=0;
  cipher_config_db.rlc_dl_cipher_info.nchan=0;

  /*get the HFN for Step_2 HFN calculation*/
  step_2_start = rrcsmc_get_latest_transmitted_start_value_per_domain(RRC_CS_DOMAIN_CN_ID);

  //convert start to count_i
  step_2_hfn = step_2_start << 4;

  /*set the HFN value for MAC to the step_2 HFN value.. also set the config
    type enum to STEP_2.. and also copy the ciph_act_time for mac to the new
    activation time sent in HO_TO_UTRAN_COMPLETE message
    we don't need to increment HFN+1 for MAC, as MAC layer takes care of it
    once we hit activation time*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = NON_HO_STEP2_CIPHER_CFG;
    
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].hfn = 
    step_2_hfn;
    
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].ciph_act_time = 
    step_2_ciph_act_time;
      
  /*call llc function to update rlc/mac ciphering configuration for
    starting STEP_2 ciphering*/
  rrcllc_update_ciphering_config_info_for_first_tm_rb();

  /*reset the cipher config type to non-HO*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = NON_HO_CIPHER_CFG;

  WRRC_MSG2_HIGH("Ciph act time is %d HFN is 0x%x", step_2_ciph_act_time,step_2_hfn);
  
  return step_2_ciph_act_time;
}

/*===========================================================================
  FUNCTION rrcsmc_process_cipher_config_for_first_tm_setup

DESCRIPTION
  Process Step 1 ciphering for First TM RB Setup

DEPENDENCIES
  None.

RETURN VALUE
  uecomdef_status_e_type SUCCESS if Ciphering is started on TM RB's
                         FAILURE if Ciphering is not started on TM RB's  

===========================================================================*/
uecomdef_status_e_type rrcsmc_process_cipher_config_for_first_tm_setup
(
   uint32 mac_d_hfn
)
{

  if(rrcsmc_int_ciph_info.ciph_status[RRC_CS_DOMAIN_CN_ID] !=  RRCSMC_SECURITY_STARTED)
  {
    return FAILURE;
  }
  
  rrcsmc_backup_cipher_config_db();
  
  /*set cipher config type to HO_STEP_1*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = NON_HO_CIPHER_CFG;
      
  //update MAC cipher DB for ciph algo
    cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_algo = 
          rrcsmc_int_ciph_info.saved_ciph_algo[RRC_CS_DOMAIN_CN_ID];
  
  //left shift by 4 to make the START value to HFN for TM RB's

  if(mac_d_hfn == RRCSMC_INVALID_HFN_VALUE)
  {
    cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].hfn = 
    (rrcsmc_get_latest_transmitted_start_value_per_domain(RRC_CS_DOMAIN_CN_ID) << 4);
  }
  else
  {
    WRRC_MSG1_HIGH("mac d hfn presetn, updating cipher db, %d", mac_d_hfn);
    cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].hfn =mac_d_hfn;
  }
  WRRC_MSG0_HIGH("First TM RB setup");

  return SUCCESS;
}/*rrcsmc_process_cipher_config_for_hho*/

/*===========================================================================

FUNCTION rrcsmc_process_cipher_config_for_hho

DESCRIPTION
  Process Step 1 ciphering for Handover

DEPENDENCIES
  None.

RETURN VALUE
  uecomdef_status_e_type SUCCESS if Ciphering is started on TM RB's
                         FAILURE if Ciphering is not started on TM RB's  

===========================================================================*/
uecomdef_status_e_type rrcsmc_process_cipher_config_for_hho
(
   uint32 mac_d_hfn
)
{
  //if ciphering is not started for TM rb, HHO is being done in idle mode or
  //for PS RB.. hence do not add ciphering configuration..
  if(cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_enable != TRUE)
  {
    WRRC_MSG0_ERROR("Ciphering not enabled for CS domain return failure");
    return FAILURE;
  }

  rrcsmc_backup_cipher_config_db();
  
  /*set cipher config type to HO_STEP_1*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = HO_STEP1_CIPHER_CFG;
      
  //update MAC cipher DB for ciph algo
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_algo = 
    rrcsmc_int_ciph_info.saved_ciph_algo[RRC_CS_DOMAIN_CN_ID];

  //left shift by 4 to make the START value to HFN for TM RB's

  if(mac_d_hfn == RRCSMC_INVALID_HFN_VALUE)
  {
    cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].hfn = 
    (rrcsmc_get_latest_transmitted_start_value_per_domain(RRC_CS_DOMAIN_CN_ID) << 4);
  }
  else
  {
    WRRC_MSG1_HIGH("mac d hfn presetn, updating cipher db, %d", mac_d_hfn);
    cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].hfn =mac_d_hfn;
  }
  
  /* Calculate and save START based on current MAX HFN so that it can be used at Step-2 */
  (void)rrcsmc_update_start_value_to_transmit_cs_hho();

  WRRC_MSG1_HIGH("HHO Step1 HFN:0x%x", cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].hfn);

  return SUCCESS;
}/*rrcsmc_process_cipher_config_for_hho*/
/*===========================================================================

FUNCTION rrcsmc_step_2_cipher_config_for_hho

DESCRIPTION
  Process Ciph Config Cnf for Hard Handover

DEPENDENCIES
  None.

RETURN VALUE
  Returns Count_C Activation Time

===========================================================================*/
uint32 rrcsmc_step_2_cipher_config_for_hho
( 
void
)
{
  uint32 step_2_start, step_2_ciph_act_time, step_2_hfn;
  
  /*256 is the CFN rollover length*/
  /*250 is used to activate ciphering after 250 10ms time interval,
    this should give sufficient time for n/w side and ue side to have
    configured ciphering on the rb's successfully*/
  step_2_ciph_act_time = (uint8)((seq_get_cfn() + 250) % 256);
  /*round it to 80ms boundary */
  step_2_ciph_act_time = (step_2_ciph_act_time/8)*8; 
  

  //set ul/dl rlc cipher info to 0's
  cipher_config_db.rlc_ul_cipher_info.nchan=0;
  cipher_config_db.rlc_dl_cipher_info.nchan=0;

  /*get the HFN for Step_2 HFN calculation*/
  step_2_start = rrcsmc_get_latest_transmitted_start_value_per_domain(RRC_CS_DOMAIN_CN_ID);

  //convert start to count_i
  step_2_hfn = step_2_start << 4;

  /*set the HFN value for MAC to the step_2 HFN value.. also set the config
    type enum to STEP_2.. and also copy the ciph_act_time for mac to the new
    activation time sent in HO_TO_UTRAN_COMPLETE message
    we don't need to increment HFN+1 for MAC, as MAC layer takes care of it
    once we hit activation time*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = HO_STEP2_CIPHER_CFG;
    
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].hfn = 
    step_2_hfn;
    
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].ciph_act_time = 
    step_2_ciph_act_time;
      
  /*call llc function to update rlc/mac ciphering configuration for
    starting STEP_2 ciphering*/
  rrcllc_update_ciphering_config_info_for_handover();

  /*reset the cipher config type to non-HO*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = NON_HO_CIPHER_CFG;

  WRRC_MSG2_HIGH("Ciph act time is %d HFN is 0x%x", step_2_ciph_act_time,step_2_hfn);
  
  return step_2_ciph_act_time;
}

/*===========================================================================

FUNCTION rrcsmc_update_start_value_to_transmit_cs_hho

DESCRIPTION
  Updates start_value_to_transmit_cs_hho

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_update_start_value_to_transmit_cs_hho
(
void
)
{
  start_value_to_transmit_cs_hho = (rrcsmc_get_max_start_per_domain(RRC_CS_DOMAIN_CN_ID));
  WRRC_MSG1_HIGH("updating SVTHHO:0x%x",start_value_to_transmit_cs_hho);
}

/*===========================================================================

FUNCTION rrcsmc_validate_and_save_srns_info_r7

DESCRIPTION
  This function saves Security Info received in SRNS relocation
  message for later use.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static uecomdef_status_e_type rrcsmc_validate_and_save_srns_info_r7
(
  rrcsmc_srns_relocation_security_info_r7_type * srns_info,
  rrc_msg_ver_enum_type msg_version
)
{

  saved_srns_info.ciph_algo_present     = FALSE;
  saved_srns_info.fresh_present         = FALSE;
  saved_srns_info.dpch_act_time_present = FALSE;    
  
  if(srns_info->integrity_info_present)
  {
    if(srns_info->integrity_protection_mode_info->integrityProtectionModeCommand.
       t != T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection)
    {
      return FAILURE;
    }

    saved_srns_info.fresh_present = TRUE;
    
    /*get the fresh value*/
    saved_srns_info.fresh = b_unpackd(
      srns_info->integrity_protection_mode_info->integrityProtectionModeCommand.u.
      startIntegrityProtection->integrityProtInitNumber.data,
      RRC_DEFAULT_BIT_POS,
      RRC_FRESH_LENGTH_IN_BITS);  
    
    if(RRCRBR_R7_MSG_IE_PRESENT(srns_info->integrity_protection_mode_info,
      rrc_IntegrityProtectionModeInfo_r7_,integrityProtectionAlgorithm))
    {
      saved_srns_info.integrity_algo_present = TRUE;
      saved_srns_info.integrity_algo = rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
                &srns_info->integrity_protection_mode_info->integrityProtectionAlgorithm,
                MSG_REL7);
      WRRC_MSG1_HIGH("Integrity Algo present in SRNS info,Algo=%d[0:Invalid Algo,1:UIA1,2:UIA2]",saved_srns_info.integrity_algo);
    }
  }
    
  if (srns_info->ciphering_mode_info_present)
  {
    if(rrcsmc_get_int_cipher_algo_from_dl_sdu_ind
        (&(srns_info->ciphering_mode_info->cipheringModeCommand.startRestart),
             msg_version)
        == saved_security_mode_command_msg.ciph_algo)
    {
      WRRC_MSG0_ERROR("Ciph Algo same. Invalid Config");
      return FAILURE;
    }
    else
    {
      saved_srns_info.ciph_algo_present = TRUE;
      saved_srns_info.ciph_algo = rrcsmc_get_int_cipher_algo_from_dl_sdu_ind
        (&(srns_info->ciphering_mode_info->cipheringModeCommand.startRestart),
             msg_version);
    }
    
    if(srns_info->ciphering_mode_info->m.activationTimeForDPCHPresent)
    {
      saved_srns_info.dpch_act_time_present = TRUE;
      saved_srns_info.dpch_act_time_present = 
        (uint8)srns_info->ciphering_mode_info->activationTimeForDPCH;
    }
  }
  return SUCCESS;
}
/*===========================================================================

FUNCTION rrcsmc_save_srns_info

DESCRIPTION
  This function saves Security Info received in SRNS relocation
  message for later use.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static uecomdef_status_e_type rrcsmc_validate_and_save_srns_info
(
  rrcsmc_srns_relocation_security_info_type * srns_info,
  rrc_msg_ver_enum_type msg_version
)
{

  saved_srns_info.ciph_algo_present     = FALSE;
  saved_srns_info.fresh_present         = FALSE;
  saved_srns_info.dpch_act_time_present = FALSE;    
  
  if(srns_info->integrity_info_present)
  {
    if(srns_info->integrity_protection_mode_info->integrityProtectionModeCommand.
       t != T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection)
    {
      return FAILURE;
    }

    saved_srns_info.fresh_present = TRUE;
    
    /*get the fresh value*/
    saved_srns_info.fresh = b_unpackd(
      srns_info->integrity_protection_mode_info->integrityProtectionModeCommand.u.
      startIntegrityProtection->integrityProtInitNumber.data,
      RRC_DEFAULT_BIT_POS,
      RRC_FRESH_LENGTH_IN_BITS);  
    
    if(srns_info->integrity_protection_mode_info->m.integrityProtectionAlgorithmPresent)
    {
      saved_srns_info.integrity_algo_present = TRUE;
      saved_srns_info.integrity_algo = rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
                &srns_info->integrity_protection_mode_info->integrityProtectionAlgorithm,
                MSG_REL99);
      WRRC_MSG1_HIGH("Integrity Algo present in SRNS info,Algo=%d[0:Invalid Algo,1:UIA1]",saved_srns_info.integrity_algo);
    }
  }
    
  if (srns_info->ciphering_mode_info_present)
  {
    if(rrcsmc_get_int_cipher_algo_from_dl_sdu_ind
        (&(srns_info->ciphering_mode_info->cipheringModeCommand.u.startRestart),
             msg_version)
        == saved_security_mode_command_msg.ciph_algo)
    {
      WRRC_MSG0_ERROR("Ciph Algo same. Invalid Config");
      return FAILURE;
    }
    else
    {
      saved_srns_info.ciph_algo_present = TRUE;
      saved_srns_info.ciph_algo = 
        rrcsmc_get_int_cipher_algo_from_dl_sdu_ind
        (&(srns_info->ciphering_mode_info->cipheringModeCommand.u.startRestart),
             msg_version);
    }
    
    if(srns_info->ciphering_mode_info->m.activationTimeForDPCHPresent)
    {
      saved_srns_info.dpch_act_time_present = TRUE;
      saved_srns_info.dpch_activation_time = 
        (uint8)srns_info->ciphering_mode_info->activationTimeForDPCH;
    }
  }
  return SUCCESS;
}
/*===========================================================================

FUNCTION rrcsmc_get_srns_info_from_dl_sdu_ind

DESCRIPTION
  This function extracts ip check info/ integrity protection mode info and
  ciphering mode info from SRNS relocation triggering messages.


DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if extraction was successful
  The function returns FAILURE if extraction was unsucceful.

SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_status_e_type rrcsmc_get_srns_info_from_dl_sdu_ind
(
  rrcsmc_srns_relocation_security_info_type * srns_info,
  rrc_cmd_type *cmd_ptr
)
{

  uecomdef_status_e_type status = SUCCESS;
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;       /* Pointer to Downlink DCCH */
  uint8 msg_type;                               /*Local variable to store message type */             

  /* Get the OTA message pointer */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  srns_info->integrity_info_present = FALSE;
  srns_info->ciphering_mode_info_present = FALSE;          
  
  switch (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
  {

    case rrc_DL_DCCH_Message_PDU :

      msg_type = 
        rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                     cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      switch (msg_type)
      {
        case T_rrc_DL_DCCH_MessageType_cellUpdateConfirm:
        {
          rrc_CellUpdateConfirm_r3_IEs *cucnf_ptr = 
            RRC_EXTRACT_R3_MSG_POINTER(dcch_msg_ptr,cellUpdateConfirm,cellUpdateConfirm_r3);

          if(RRC_MSG_COMMON_BITMASK_IE_PTR(cucnf_ptr,rrc_CellUpdateConfirm_r3_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(cucnf_ptr->integrityProtectionModeInfo);
          }
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(cucnf_ptr,rrc_CellUpdateConfirm_r3_IEs,cipheringModeInfo))
            {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(cucnf_ptr->cipheringModeInfo);
          }
          }
          break;

        case T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration:
        {
          rrc_RadioBearerReconfiguration_r3_IEs *rbrreconfig_ptr = 
            RRC_EXTRACT_R3_MSG_POINTER(dcch_msg_ptr,radioBearerReconfiguration,radioBearerReconfiguration_r3);

          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrreconfig_ptr,rrc_RadioBearerReconfiguration_r3_IEs,integrityProtectionModeInfo))  
            {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rbrreconfig_ptr->integrityProtectionModeInfo);
          }
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrreconfig_ptr,rrc_RadioBearerReconfiguration_r3_IEs,cipheringModeInfo))  
            {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(rbrreconfig_ptr->cipheringModeInfo);
          }
          }
          break;
    
        case T_rrc_DL_DCCH_MessageType_radioBearerRelease:
        {
          rrc_RadioBearerRelease_r3_IEs *rb_release_ptr = 
            RRC_EXTRACT_R3_MSG_POINTER(dcch_msg_ptr,radioBearerRelease,radioBearerRelease_r3);

          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,rrc_RadioBearerRelease_r3_IEs,integrityProtectionModeInfo))            
            {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rb_release_ptr->integrityProtectionModeInfo);
          }
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,rrc_RadioBearerRelease_r3_IEs,cipheringModeInfo))            
            {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(rb_release_ptr->cipheringModeInfo);
          }
          }
          break;
    
        case T_rrc_DL_DCCH_MessageType_radioBearerSetup:
        {
          rrc_RadioBearerSetup_r3_IEs *rb_setup_ptr = 
            RRC_EXTRACT_R3_MSG_POINTER(dcch_msg_ptr,radioBearerSetup,radioBearerSetup_r3);

          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_setup_ptr,rrc_RadioBearerSetup_r3_IEs,integrityProtectionModeInfo))      
            {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rb_setup_ptr->integrityProtectionModeInfo);
          }
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_setup_ptr,rrc_RadioBearerSetup_r3_IEs,cipheringModeInfo))      
            {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(rb_setup_ptr->cipheringModeInfo);
          }
          }
          break;
    
        case T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration:
        {
          rrc_TransportChannelReconfiguration_r3_IEs *tcreconfig_ptr =
            RRC_EXTRACT_R3_MSG_POINTER(dcch_msg_ptr,transportChannelReconfiguration,transportChannelReconfiguration_r3);

          if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r3_IEs,integrityProtectionModeInfo))     
            {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(tcreconfig_ptr->integrityProtectionModeInfo);
          }
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r3_IEs,cipheringModeInfo))     
            {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(tcreconfig_ptr->cipheringModeInfo);
          }
          }
          break;

        case T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration:
        {
          rrc_PhysicalChannelReconfiguration_r3_IEs *pc_reconfig_ptr = 
            RRC_EXTRACT_R3_MSG_POINTER(dcch_msg_ptr,physicalChannelReconfiguration,physicalChannelReconfiguration_r3);

          if(RRC_MSG_COMMON_BITMASK_IE_PTR(pc_reconfig_ptr,rrc_PhysicalChannelReconfiguration_r3_IEs,integrityProtectionModeInfo))               
            {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(pc_reconfig_ptr->integrityProtectionModeInfo);
          }
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(pc_reconfig_ptr,rrc_PhysicalChannelReconfiguration_r3_IEs,cipheringModeInfo))               
            {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(pc_reconfig_ptr->cipheringModeInfo);
          }
          }
          break;      
      
        case T_rrc_DL_DCCH_MessageType_utranMobilityInformation:
        {
          
          rrc_UTRANMobilityInformation_r3_IEs *utran_mob_info = 
            RRC_EXTRACT_R3_MSG_POINTER(dcch_msg_ptr,utranMobilityInformation,utranMobilityInformation_r3);
          
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(utran_mob_info,rrc_UTRANMobilityInformation_r3_IEs,integrityProtectionModeInfo))               
            {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(utran_mob_info->integrityProtectionModeInfo);
          }
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(utran_mob_info,rrc_UTRANMobilityInformation_r3_IEs,cipheringModeInfo))               
            {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(utran_mob_info->cipheringModeInfo);
          }
          }
          break;
        case T_rrc_DL_DCCH_MessageType_uraUpdateConfirm:
          /* For uraUpdateConfirm, I.E integrityProtectionModeInfo is supported in R5/R7 messages only. */
          if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.r3.uraUpdateConfirm_r3.m.integrityProtectionModeInfoPresent)
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = 
              &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.r3.uraUpdateConfirm_r3.integrityProtectionModeInfo);
          }
          if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.r3.uraUpdateConfirm_r3.m.cipheringModeInfoPresent)
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = 
              &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.r3.uraUpdateConfirm_r3.cipheringModeInfo);
          }
        break; 

        default:
          MSG_MED("MSG not supported for SRNS-reloc",0,0,0);
          status = FAILURE;
          break;
      }
      break;

    default:
      WRRC_MSG0_HIGH("DL-DCCH msg only for SRNS-reloc");
      status = FAILURE;
      break;
  }
      
  if(srns_info->integrity_info_present == TRUE)
  {
    srns_info->integrity_check_info = &(dcch_msg_ptr->integrityCheckInfo);      
  }
  
  return status;
}
/*===========================================================================

FUNCTION rrcsmc_get_srns_info_from_dl_sdu_ind_r6

DESCRIPTION
  This function extracts ip check info/ integrity protection mode info and
  ciphering mode info from SRNS relocation triggering messages for Release 6 version
  of specs

  NOTE:  There is no R6 utranMobilityInformation message

DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if extraction was successful
  The function returns FAILURE if extraction was unsucceful.

SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_status_e_type rrcsmc_get_srns_info_from_dl_sdu_ind_r6
(
  rrcsmc_srns_relocation_security_info_type * srns_info,
  rrc_cmd_type *cmd_ptr
)
{
  uecomdef_status_e_type status = SUCCESS;
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;       /* Pointer to Downlink DCCH */
  uint8 msg_type;                               /*Local variable to store message type */             
  
  /* Get the OTA message pointer */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
  
  srns_info->integrity_info_present = FALSE;
  srns_info->ciphering_mode_info_present = FALSE;          
  
  switch (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
  { 
  case rrc_DL_DCCH_Message_PDU :
    
    msg_type = 
      rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
      cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
    switch (msg_type)
    {
    case T_rrc_DL_DCCH_MessageType_cellUpdateConfirm:
    {
      rrc_CellUpdateConfirm_r6_IEs *cucnf_ptr = 
        RRC_EXTRACT_R6_MSG_POINTER(dcch_msg_ptr,cellUpdateConfirm,cellUpdateConfirm_r6);

      if(RRC_MSG_COMMON_BITMASK_IE_PTR(cucnf_ptr,rrc_CellUpdateConfirm_r6_IEs,integrityProtectionModeInfo))
      {
        srns_info->integrity_info_present = TRUE;
        srns_info->integrity_protection_mode_info = &(cucnf_ptr->integrityProtectionModeInfo);
      }
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(cucnf_ptr,rrc_CellUpdateConfirm_r6_IEs,cipheringModeInfo))
      {
        srns_info->ciphering_mode_info_present = TRUE;
        srns_info->ciphering_mode_info = &(cucnf_ptr->cipheringModeInfo);
      }
      }
      break;
      
    case T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration:
    {
      rrc_RadioBearerReconfiguration_r6_IEs *rbrreconfig_ptr =  
        RRC_EXTRACT_R6_MSG_POINTER(dcch_msg_ptr,radioBearerReconfiguration,radioBearerReconfiguration_r6);

      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrreconfig_ptr,rrc_RadioBearerReconfiguration_r6_IEs,integrityProtectionModeInfo))      
      {
        srns_info->integrity_info_present = TRUE;
        srns_info->integrity_protection_mode_info = &(rbrreconfig_ptr->integrityProtectionModeInfo);
      }
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrreconfig_ptr,rrc_RadioBearerReconfiguration_r6_IEs,cipheringModeInfo))      
      {
        srns_info->ciphering_mode_info_present = TRUE;
        srns_info->ciphering_mode_info =  &(rbrreconfig_ptr->cipheringModeInfo);
      }
      }
      break;
      
    case T_rrc_DL_DCCH_MessageType_radioBearerRelease:
    {
      rrc_RadioBearerRelease_r6_IEs* rb_release_ptr =  
        RRC_EXTRACT_R6_MSG_POINTER(dcch_msg_ptr,radioBearerRelease,radioBearerRelease_r6);

      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,rrc_RadioBearerRelease_r6_IEs,integrityProtectionModeInfo))      
      {
        srns_info->integrity_info_present = TRUE;
        srns_info->integrity_protection_mode_info = &(rb_release_ptr->integrityProtectionModeInfo);
      }
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,rrc_RadioBearerRelease_r6_IEs,cipheringModeInfo))      
      {
        srns_info->ciphering_mode_info_present = TRUE;
        srns_info->ciphering_mode_info =  &(rb_release_ptr->cipheringModeInfo);
      }
      }
      break;
      
    case T_rrc_DL_DCCH_MessageType_radioBearerSetup:
    {
      rrc_RadioBearerSetup_r6_IEs *rb_setup_ptr = 
        RRC_EXTRACT_R6_MSG_POINTER(dcch_msg_ptr,radioBearerSetup,radioBearerSetup_r6);

      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_setup_ptr,rrc_RadioBearerSetup_r6_IEs,integrityProtectionModeInfo))               
      {
        srns_info->integrity_info_present = TRUE;
        srns_info->integrity_protection_mode_info = &(rb_setup_ptr->integrityProtectionModeInfo);
      }
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_setup_ptr,rrc_RadioBearerSetup_r6_IEs,cipheringModeInfo))               
      {
        srns_info->ciphering_mode_info_present = TRUE;
        srns_info->ciphering_mode_info = &(rb_setup_ptr->cipheringModeInfo);
      }
      }
      break;
      
    case T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration:
    {
      rrc_TransportChannelReconfiguration_r6_IEs *tcreconfig_ptr =
        RRC_EXTRACT_R6_MSG_POINTER(dcch_msg_ptr,transportChannelReconfiguration,transportChannelReconfiguration_r6);
            
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r6_IEs,integrityProtectionModeInfo))               
      {
        srns_info->integrity_info_present = TRUE;
        srns_info->integrity_protection_mode_info =  &(tcreconfig_ptr->integrityProtectionModeInfo);
      }
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r6_IEs,cipheringModeInfo))               
      {
        srns_info->ciphering_mode_info_present = TRUE;
        srns_info->ciphering_mode_info =  &(tcreconfig_ptr->cipheringModeInfo);
      }
      }
      break;
      
    case T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration:
    {
      
      rrc_PhysicalChannelReconfiguration_r6_IEs *pc_reconfig_ptr = 
        RRC_EXTRACT_R6_MSG_POINTER(dcch_msg_ptr,physicalChannelReconfiguration,physicalChannelReconfiguration_r6);
      
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(pc_reconfig_ptr,rrc_PhysicalChannelReconfiguration_r6_IEs,integrityProtectionModeInfo))               
      {
        srns_info->integrity_info_present = TRUE;
        srns_info->integrity_protection_mode_info = &(pc_reconfig_ptr->integrityProtectionModeInfo);
      }
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(pc_reconfig_ptr,rrc_PhysicalChannelReconfiguration_r6_IEs,cipheringModeInfo))               
      {
        srns_info->ciphering_mode_info_present = TRUE;
        srns_info->ciphering_mode_info =  &(pc_reconfig_ptr->cipheringModeInfo);
      }
      }
      break;      

    case T_rrc_DL_DCCH_MessageType_uraUpdateConfirm:
      /* For uraUpdateConfirm, I.E integrityProtectionModeInfo is supported in R5/R7 messages only. */
      if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.r5.uraUpdateConfirm_r5.m.integrityProtectionModeInfoPresent)
      {
        srns_info->integrity_info_present = TRUE;
        srns_info->integrity_protection_mode_info = 
          &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.
            r5.uraUpdateConfirm_r5.integrityProtectionModeInfo);
      }
      if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.r5.uraUpdateConfirm_r5.m.cipheringModeInfoPresent   )
      {
        srns_info->ciphering_mode_info_present = TRUE;
        srns_info->ciphering_mode_info = 
          &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.
             r5.uraUpdateConfirm_r5.cipheringModeInfo);
      }
      break;
            
    default:
      MSG_MED("MSG not supported for SRNS-reloc",0,0,0);
      status = FAILURE;
      break;
      }
      break;
      
  default:
    WRRC_MSG0_HIGH("DL-DCCH msg only for SRNS-reloc");
    status = FAILURE;
    break;
  }
  
  if(srns_info->integrity_info_present == TRUE)
  {
    srns_info->integrity_check_info = &(dcch_msg_ptr->integrityCheckInfo);      
  }
  
  return status;
}

/*===========================================================================

FUNCTION rrcsmc_get_srns_info_from_dl_sdu_ind_r5

DESCRIPTION
  This function extracts ip check info/ integrity protection mode info and
  ciphering mode info from SRNS relocation triggering messages for Release 5 version
  of specs


DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if extraction was successful
  The function returns FAILURE if extraction was unsucceful.

SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_status_e_type rrcsmc_get_srns_info_from_dl_sdu_ind_r5
(
  rrcsmc_srns_relocation_security_info_type * srns_info,
  rrc_cmd_type *cmd_ptr
)
{
  uecomdef_status_e_type status = SUCCESS;
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;       /* Pointer to Downlink DCCH */
  uint8 msg_type;                               /*Local variable to store message type */             
  
  /* Get the OTA message pointer */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
  
  srns_info->integrity_info_present = FALSE;
  srns_info->ciphering_mode_info_present = FALSE;          
  
  switch (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
  {
    
  case rrc_DL_DCCH_Message_PDU :
    
    msg_type = 
      rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
      cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
    switch (msg_type)
    {
      case T_rrc_DL_DCCH_MessageType_cellUpdateConfirm:
      {
        rrc_CellUpdateConfirm_r5_IEs *cucnf_ptr = 
          RRC_EXTRACT_R5_MSG_POINTER(dcch_msg_ptr,cellUpdateConfirm,cellUpdateConfirm_r5);
      
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(cucnf_ptr,rrc_CellUpdateConfirm_r5_IEs,integrityProtectionModeInfo))
        {
          srns_info->integrity_info_present = TRUE;
          srns_info->integrity_protection_mode_info = &(cucnf_ptr->integrityProtectionModeInfo);
        }
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(cucnf_ptr,rrc_CellUpdateConfirm_r5_IEs,cipheringModeInfo))
        {
          srns_info->ciphering_mode_info_present = TRUE;
          srns_info->ciphering_mode_info = &(cucnf_ptr->cipheringModeInfo);
        }
      }
      break;
      
      case T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration:
      {
        rrc_RadioBearerReconfiguration_r5_IEs *rbrreconfig_ptr = 
          RRC_EXTRACT_R5_MSG_POINTER(dcch_msg_ptr,radioBearerReconfiguration,radioBearerReconfiguration_r5); 

        if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrreconfig_ptr,rrc_RadioBearerReconfiguration_r5_IEs,integrityProtectionModeInfo))
        {
          srns_info->integrity_info_present = TRUE;
          srns_info->integrity_protection_mode_info = &(rbrreconfig_ptr->integrityProtectionModeInfo);
        }
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrreconfig_ptr,rrc_RadioBearerReconfiguration_r5_IEs,cipheringModeInfo))
        {
          srns_info->ciphering_mode_info_present = TRUE;
          srns_info->ciphering_mode_info = &(rbrreconfig_ptr->cipheringModeInfo);
        }
      }
      break;
      
      case T_rrc_DL_DCCH_MessageType_radioBearerRelease:
      {
        rrc_RadioBearerRelease_r5_IEs* rb_release_ptr = 
          RRC_EXTRACT_R5_MSG_POINTER(dcch_msg_ptr,radioBearerRelease,radioBearerRelease_r5);

        if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,rrc_RadioBearerRelease_r5_IEs,integrityProtectionModeInfo))
        {
          srns_info->integrity_info_present = TRUE;
          srns_info->integrity_protection_mode_info = &(rb_release_ptr->integrityProtectionModeInfo);
        }
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,rrc_RadioBearerRelease_r5_IEs,cipheringModeInfo))
        {
          srns_info->ciphering_mode_info_present = TRUE;
          srns_info->ciphering_mode_info =  &(rb_release_ptr->cipheringModeInfo);
        }
      }
      break;
      
      case T_rrc_DL_DCCH_MessageType_radioBearerSetup:
      {
        rrc_RadioBearerSetup_r5_IEs *rb_setup_ptr = 
          RRC_EXTRACT_R5_MSG_POINTER(dcch_msg_ptr,radioBearerSetup,radioBearerSetup_r5);
      
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_setup_ptr,rrc_RadioBearerSetup_r5_IEs,integrityProtectionModeInfo))
        {
          srns_info->integrity_info_present = TRUE;
          srns_info->integrity_protection_mode_info = &(rb_setup_ptr->integrityProtectionModeInfo);
        }
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_setup_ptr,rrc_RadioBearerSetup_r5_IEs,cipheringModeInfo))
        {
          srns_info->ciphering_mode_info_present = TRUE;
          srns_info->ciphering_mode_info = &(rb_setup_ptr->cipheringModeInfo);
        }
      }
      break;
      
      case T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration:
      {
        rrc_TransportChannelReconfiguration_r5_IEs *tcreconfig_ptr =
          RRC_EXTRACT_R5_MSG_POINTER(dcch_msg_ptr,transportChannelReconfiguration,transportChannelReconfiguration_r5);
      
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r5_IEs,integrityProtectionModeInfo))
        {
          srns_info->integrity_info_present = TRUE;
          srns_info->integrity_protection_mode_info = &(tcreconfig_ptr->integrityProtectionModeInfo);
        }
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r5_IEs,cipheringModeInfo))
        {
          srns_info->ciphering_mode_info_present = TRUE;
          srns_info->ciphering_mode_info = &(tcreconfig_ptr->cipheringModeInfo);
        }
      }
      break;
      
      case T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration:
      {
        rrc_PhysicalChannelReconfiguration_r5_IEs *pc_reconfig_ptr =  
          RRC_EXTRACT_R5_MSG_POINTER(dcch_msg_ptr,physicalChannelReconfiguration,physicalChannelReconfiguration_r5);

        if(RRC_MSG_COMMON_BITMASK_IE_PTR(pc_reconfig_ptr,rrc_PhysicalChannelReconfiguration_r5_IEs,integrityProtectionModeInfo))      
        {
          srns_info->integrity_info_present = TRUE;
          srns_info->integrity_protection_mode_info = &(pc_reconfig_ptr->integrityProtectionModeInfo);
        }
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(pc_reconfig_ptr,rrc_PhysicalChannelReconfiguration_r5_IEs,cipheringModeInfo))      
        {
          srns_info->ciphering_mode_info_present = TRUE;
          srns_info->ciphering_mode_info = &(pc_reconfig_ptr->cipheringModeInfo);
        }
      }
      break;      
      
      case T_rrc_DL_DCCH_MessageType_utranMobilityInformation:
      {
        rrc_UTRANMobilityInformation_r5_IEs *utran_mob_info = 
          RRC_EXTRACT_R5_MSG_POINTER_UMI(dcch_msg_ptr,utranMobilityInformation,utranMobilityInformation_r5);
      
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(utran_mob_info,rrc_UTRANMobilityInformation_r5_IEs,integrityProtectionModeInfo))      
        {
          srns_info->integrity_info_present = TRUE;
          srns_info->integrity_protection_mode_info = &(utran_mob_info->integrityProtectionModeInfo);
        }
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(utran_mob_info,rrc_UTRANMobilityInformation_r5_IEs,cipheringModeInfo))      
        {
          srns_info->ciphering_mode_info_present = TRUE;
          srns_info->ciphering_mode_info = &(utran_mob_info->cipheringModeInfo);
        }
      }
      
      break;
      case T_rrc_DL_DCCH_MessageType_uraUpdateConfirm:
        /* For uraUpdateConfirm, I.E integrityProtectionModeInfo is supported in R5/R7 messages only. */
        if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.r5.uraUpdateConfirm_r5.m.integrityProtectionModeInfoPresent)
        {
          srns_info->integrity_info_present = TRUE;
          srns_info->integrity_protection_mode_info = 
            &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.
               r5.uraUpdateConfirm_r5.integrityProtectionModeInfo);
        }
        if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.r5.uraUpdateConfirm_r5.m.cipheringModeInfoPresent)
        {
          srns_info->ciphering_mode_info_present = TRUE;
          srns_info->ciphering_mode_info = 
            &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.
              r5.uraUpdateConfirm_r5.cipheringModeInfo);
        }
      break;
      
    default:
      MSG_MED("MSG not supported for SRNS-reloc",0,0,0);
      status = FAILURE;
      break;
      }
      break;
      
    default:
      WRRC_MSG0_HIGH("DL-DCCH msg only for SRNS-reloc");
      status = FAILURE;
      break;
  }
  
  if(srns_info->integrity_info_present == TRUE)
  {
    srns_info->integrity_check_info = &(dcch_msg_ptr->integrityCheckInfo);      
  }
  
  return status;
}



/*===========================================================================

FUNCTION rrcsmc_commit_fresh_for_srns_relocation

DESCRIPTION
  This function commits the FRESH value received in SRNS Relocation
  triggering message.
 
DEPENDENCIES
  None.

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_commit_fresh_for_srns_relocation
(
  void
)
{
  //check if fresh flag is present and commit the fresh values
  if(saved_srns_info.fresh_present == TRUE)
  {
    //commit the new fresh value received and set ip reconfig to true
    rrcf9_init_fresh_per_connection(saved_srns_info.fresh);

    //save the comitted fresh value as old fresh value to be used in
    //the next connection setup
    saved_srns_info.fresh_old1 = saved_srns_info.fresh_old;    
    saved_srns_info.fresh_old = saved_srns_info.fresh;

  }
}

/*===========================================================================

FUNCTION rrcsmc_revert_back_to_old_fresh_for_srns_relocation

DESCRIPTION
  This function reverts back to old stored FRESH value which UE was using before
  receipt of SRNS relocation confiruation message(for which UE has to revert back
  to old configuration).
 
DEPENDENCIES
  None.

RETURN VALUE
  Void

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_revert_back_to_old_fresh_for_srns_relocation
(
  void
)
{
  //check if fresh flag is present and commit the fresh values
  if(saved_srns_info.fresh_present == TRUE)
  {
    WRRC_MSG0_HIGH("Revert back to old FRESH value (SRNS Relocation scenario)");
    //commit the new fresh value received and set ip reconfig to true
    rrcf9_init_fresh_per_connection(saved_srns_info.fresh_old1);

    //save the comitted fresh value as old fresh value to be used in
    //the next connection setup
    saved_srns_info.fresh_old = saved_srns_info.fresh_old1;

    saved_srns_info.fresh_present = FALSE;
  }
  else
  {
    WRRC_MSG0_HIGH("No need to revert back to old FRESH value");
  }
}

/*===========================================================================

FUNCTION rrcsmc_check_integrity_for_srns_relocation

DESCRIPTION
  This functions checks for integrity for SRNS relocation triggering
  messages.


DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if Integrity check was successful
  The function returns FAILURE if Integrity check was unsuccessful or config 
  sent was bad.

SIDE EFFECTS
  None

===========================================================================*/
rrcsmc_srns_ip_check_status_e_type rrcsmc_check_integrity_for_srns_relocation
(
  rrc_cmd_type *cmd_ptr
  ,rrc_msg_ver_enum_type msg_version
)
{
  rrc_RB_Identity rb_id;
  rrcsmc_srns_relocation_security_info_type srns_security_info;
  rrcsmc_srns_relocation_security_info_r7_type srns_security_info_7;
  boolean integrity_info_present = FALSE;
    
  /*local copy of integrity_check_info variable*/  
  rrcsmc_integrity_check_info_type int_check_info; 
  
  uint32 local_hfn = 0, local_seq_no = 0, local_count_i, local_mac;              

  memset(&srns_security_info,0,sizeof(rrcsmc_srns_relocation_security_info_type));
  memset(&srns_security_info_7,0,sizeof(rrcsmc_srns_relocation_security_info_r7_type));

  
#ifdef FEATURE_WCDMA_REL9
  if (msg_version == MSG_REL9)
  { 
    if(rrcsmc_get_srns_info_from_dl_sdu_ind_r9(&(srns_security_info_7), cmd_ptr) != SUCCESS)
    {
      return RRCSMC_SRNS_SECURITY_INFO_INVALID;  
    }
  }
  else
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */


  if (msg_version == MSG_REL8)
  { 
    if(rrcsmc_get_srns_info_from_dl_sdu_ind_r8(&(srns_security_info_7), cmd_ptr) != SUCCESS)
    {
      return RRCSMC_SRNS_SECURITY_INFO_INVALID;  
    }
  }
  else if (msg_version == MSG_REL7)
  { 
    if(rrcsmc_get_srns_info_from_dl_sdu_ind_r7(&(srns_security_info_7), cmd_ptr) != SUCCESS)
    {
      return RRCSMC_SRNS_SECURITY_INFO_INVALID;  
    }
  }
  else if (msg_version == MSG_REL6)
  { 
    if(rrcsmc_get_srns_info_from_dl_sdu_ind_r6(&(srns_security_info), cmd_ptr) != SUCCESS)
    {
      return RRCSMC_SRNS_SECURITY_INFO_INVALID;  
    }
  }
  else
  {
    if (msg_version == MSG_REL5)
    {    
      if(rrcsmc_get_srns_info_from_dl_sdu_ind_r5(&(srns_security_info), cmd_ptr) != SUCCESS)
      {
        return RRCSMC_SRNS_SECURITY_INFO_INVALID;  
      }
    }
    else
    {
      if(rrcsmc_get_srns_info_from_dl_sdu_ind(&(srns_security_info), cmd_ptr) != SUCCESS)
      {
        return RRCSMC_SRNS_SECURITY_INFO_INVALID;  
      }
    }
  }

  
  /*check if integrity check info and integrity protection mode info is 
    present, if it isn't then return invalid configuration*/
  if((msg_version == MSG_REL7)
  || (msg_version == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
|| (msg_version == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */

  
  )
  {
    if((rrcsmc_validate_and_save_srns_info_r7(&srns_security_info_7,msg_version)) != SUCCESS)
    {
      return RRCSMC_SRNS_SECURITY_INFO_INVALID;  
    }
  }
  else
  {
    if(rrcsmc_validate_and_save_srns_info(&(srns_security_info),MSG_REL99) != SUCCESS)
    {
      return RRCSMC_SRNS_SECURITY_INFO_INVALID;  
    }
  }
 
  if((msg_version == MSG_REL7)
  || (msg_version == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
|| (msg_version == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */

  )
  {
    if((srns_security_info_7.ciphering_mode_info_present == TRUE || 
        srns_security_info_7.integrity_info_present == TRUE) &&
       (integrity_protection_info.reconfig == TRUE ||
        ciphering_info.reconfig == TRUE))
    {
      return RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION;  
    }
  }
  else
  {
    if((srns_security_info.ciphering_mode_info_present == TRUE || 
       srns_security_info.integrity_info_present == TRUE) &&
      (integrity_protection_info.reconfig == TRUE ||
       ciphering_info.reconfig == TRUE))
    {
      return RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION;  
    }
  }
  
  /*only if integrity info is present, we need to do IP check again and
    sync up to new FRESH value*/

  if((msg_version == MSG_REL7)
  || (msg_version == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
  || (msg_version == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
	#error code not present
#endif /* FEATURE_WCDMA_REL10 */


  )
  {
    if(srns_security_info_7.integrity_info_present)
    {
      integrity_info_present = TRUE;
    }
  }
  else 
  {
    if(srns_security_info.integrity_info_present)  
    {
      integrity_info_present = TRUE;
    }
  }
    
  if(integrity_info_present)
  {
  
    rb_id =  rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id);

    /*so this message should be integrity protected. check if IntegrityCheckInfo IE
     is present in the message, also Integrity Check Info contents should be copied
    into the local variable int_check_info*/
    if ( rrcsmc_process_dl_integrity_check_info(
                                             cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                             cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length,
                                             &int_check_info,
                                             rb_id) == FAILURE)
    {
      WRRC_MSG0_ERROR("DL Integrity Info failed");
      return RRCSMC_SRNS_SECURITY_INFO_INVALID;
    }

      
    /*check if we received modify, then also check if activation time is pending
      for this rb_id, if yes for both, then sync to new configuration immediately,
      no need to wait until activation time*/
    if ( (rb_id < RRC_MAX_SRB) && ( integrity_protection_info.modify_received == TRUE) &&
         ( integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
           dl_count_i_act_time_pending[rb_id] == TRUE) )
    {
      /*reset the activation time pending flag*/
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      dl_count_i_act_time_pending[rb_id] = FALSE;
      integrity_protection_info.dl_latest_config_cn_id_old_1_valid[rb_id] = FALSE;

      /*set the default init flag*/
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      dl_count_i_init_flag[rb_id] = FALSE;

      /*if new keys have been received for this domain, then set the HFN part of
          count_i to 0, and continue using seq_num*/
      if (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
          dl_new_key_flag_per_rb[rb_id]==TRUE)
      {
        /*reset new keys flag*/
        integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
        dl_new_key_flag_per_rb[rb_id]=FALSE;
      }

      /*generate count_i with HFN part set to 0 and old domain SEQ_NUM*/
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      dl_count_i[rb_id] = 
        rrcsmc_convert_START_to_count_c_i(
          rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id].hfn,
          (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.
           latest_config_cn_id_old].dl_count_i[rb_id] & RRC_SEQ_NUM_BIT_MASK) );

    }
    if(rb_id < RRC_MAX_SRB)
    {    
              
      /*copy hfn to a local variable for further processing*/
      local_hfn= integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
                 dl_count_i[rb_id];

      /*this is not the first message on this rb_id, get the sequence num
        from the stored count_i*/
      local_seq_no = (local_hfn & RRC_SEQ_NUM_BIT_MASK);

    /*if the rcvd sequence number is the same as the previous message sequence
      number stored, then do not process this message further.  just return*/    
      if (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
          dl_count_i_init_flag[rb_id] == TRUE)
      {
        if((msg_version == MSG_REL7)
        || (msg_version == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
        || (msg_version == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
        #error code not present
#endif /* FEATURE_WCDMA_REL10 */

        )
        {
          if (local_seq_no == srns_security_info_7.integrity_check_info->rrc_MessageSequenceNumber)
          {
            WRRC_MSG2_HIGH("IP check failed,Duplicate SN %d for RB_ID %d",local_seq_no, rb_id);
            return RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;
          }
        }
        else
        {
          if (local_seq_no == srns_security_info.integrity_check_info->rrc_MessageSequenceNumber)
          {
            WRRC_MSG2_HIGH("IP check failed,Duplicate SN %d for RB_ID %d",local_seq_no, rb_id);
            return RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;
          }
        }
      }
      else
      {
        /*set the init_flag to TRUE*/  
        integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
          dl_count_i_init_flag[rb_id] = TRUE;
      }
    }

    /*update fresh value temporarily to calculate MAC-I for DL msg*/
    rrcf9_init_fresh_per_connection(saved_srns_info.fresh);
    
    /*if the stored seq number is greater than the one received in the msg
      then a rollover has occured.  Increment the hfn part of the number
      only*/
    if (local_seq_no > int_check_info.msg_seq_num)
    {
      /*increment hfn only*/
      local_hfn >>=4;
      local_hfn++;
      local_hfn <<=4;
    }

    /*generate the new count_i with the received sequence number and the hfn*/
    if((msg_version == MSG_REL7)
    || (msg_version == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
    || (msg_version == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */

    )
    {
      local_count_i = 
      (uint32)((local_hfn & RRC_HFN_BIT_MASK) | 
               (srns_security_info_7.integrity_check_info->rrc_MessageSequenceNumber & 
               RRC_SEQ_NUM_BIT_MASK));
    }
    else
    {
      local_count_i = 
      (uint32)((local_hfn & RRC_HFN_BIT_MASK) | 
               (srns_security_info.integrity_check_info->rrc_MessageSequenceNumber & 
               RRC_SEQ_NUM_BIT_MASK));
    }

    /*call the f9 engine to generate MAC for the dl message*/
    local_mac = rrcf9_generate_mac_for_message(
                  local_count_i,
                  RRC_INTEGRITY_DIR_DOWNLINK,
                  cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                  (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8),
                  rrcsmc_int_ciph_info.latest_config_cn_id,
                  saved_srns_info.integrity_algo
                  );

    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"maci gen = 0x%x int_check_info.msg_auth_code = 0x%x CN domain %d[0=CS,1=PS], Integrity Algo = %d[0=Invalid,1=UIA1,2=UIA2] ",
    local_mac,
    int_check_info.msg_auth_code,
    rrcsmc_int_ciph_info.latest_config_cn_id,
    saved_srns_info.integrity_algo
    );
        

    /*Restore the FRESH value to previously stored value*/
    rrcf9_init_fresh_per_connection(saved_srns_info.fresh_old);

    /*if the MAC's match then we have successfully checked the integrity of
      this message.  update the count_i for this rb_id*/
    if (local_mac == int_check_info.msg_auth_code)
    {
      /*update count_i stored in integrity_protection_info global var*/
      if(rb_id < RRC_MAX_SRB)
      {
        integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
        dl_count_i[rb_id] = local_count_i;
        
        WRRC_MSG0_HIGH("SRNS-DL IP check successful");
      }
      return RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS;
    }    
    else
    {
      WRRC_MSG0_ERROR("SRNS-DL IP check fail");  
      /*Logging the IK keys used for MAC-I generation*/
      PRINT_INT_KEY(rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id])
      return RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;
    }
  }
  
  return RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS;  
}
/*===========================================================================

FUNCTION rrcsmc_check_downlink_integrity

DESCRIPTION
  This function checks the Integrity of the DL message received and returns 
  the status.  It receives the pointer to the Command and runs Integrity Check 
  on the message.
  The dispatcher procedure will call this function.


DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if DL Integrity check was successful
  The function returns FAILURE if DL Integrity check was unsuccessful

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_check_downlink_integrity 
(
rrc_cmd_type *cmd_ptr 
)
{
  boolean is_dl_act_time_expired = FALSE;

  if (nv_integrity_enabled)
  {

    rrc_RB_Identity rb_id;           /*local var to store rb_id*/
    uint8 msg_type;                  /*Local variable to store message type */             

    /*local copy of integrity_check_info variable*/  
    rrcsmc_integrity_check_info_type int_check_info; 

    uint32 local_hfn = 0, local_seq_no = 0, local_count_i, local_mac;              
    /*local var to store hfn, count_i and
      sequence number*/

    rrc_cn_domain_identity_e_type  local_cn_domain;
    boolean                        using_old_keys_flag=FALSE;
    boolean                        using_old_config_flag=FALSE;
    boolean                        use_latest_config_cn_id_old_1_keys_flag = FALSE;

    /*check if integrity protection is started or not*/
    if (integrity_protection_info.ip_status == RRCSMC_SECURITY_NOT_STARTED)
    {
      MSG_MED("Integrity Protection Not Started",0,0,0);
      return SUCCESS;
    }

    /*check to see if the messages will be integrity protected.  SIBS, MIBS,
    PCCH messages and most CCCH messages are not integrity protected. Just
    return success if one of these messages.
    Also if Integrity is already started and we receive a SECURITY MODE COMMAND
    second time(with MODIFY) then return SUCCESS.  We will check IP for SMC later*/
/*lint -e527*/    
    switch (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
    {
      case rrc_BCCH_FACH_Message_PDU:
      case rrc_BCCH_BCH_Message_PDU:
      case rrc_PCCH_Message_PDU:    
        return SUCCESS;

      case rrc_DL_CCCH_Message_PDU:

        msg_type = 
        rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                     cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
        switch (msg_type )
        {
          /*Integrity Protection not reqd. for the foll. messages. Return success
            without doing any processing*/
          case T_rrc_DL_CCCH_MessageType_rrcConnectionReject:
          case T_rrc_DL_CCCH_MessageType_rrcConnectionRelease:
          case T_rrc_DL_CCCH_MessageType_rrcConnectionSetup:
            return SUCCESS;

          default:
            MSG_MED("Integrity Check Info Needed",0,0,0);
            break;
        }
        break;

      case RRCLCM_DL_DCCH_SRB5_SDU_TYPE:
          WRRC_MSG0_HIGH("Ignoring DL IP check for TFCC TM procedure");
          return SUCCESS;

      case rrc_DL_DCCH_Message_PDU :
        msg_type = 
        rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                     cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
        switch (msg_type)
        {
          /*Integrity Protection not checked here for SMC cmd. Will be checked later.
            Return success without doing any processing*/
          case T_rrc_DL_DCCH_MessageType_securityModeCommand:
            return SUCCESS;

          /* If NAS messages are not integrity protected,do not perform IP check on them
             NAS will decide whether this message needs to be honoured or not.This is an
             optimization to counter a race condition on the network side where Authentication  
             and SMC processes are going on parallely for 2 domains in a MultiRAB call
           */  
          case T_rrc_DL_DCCH_MessageType_downlinkDirectTransfer:
            if (0x00 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu, 
                        INTEGRITY_CHECK_INFO_PRESENT_POS,
                        NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
            {
              /* message not integrity protected */
              /* Check whether message contains 'r3' IE or not */
              if (0x00 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu, 
                  DL_DCCH_R3_INDICATOR_WITHOUT_INT_CHECK_INFO_POS,
                  NUM_OF_BITS_FOR_DL_DCCH_R3_INDICATOR))
              { /* r3 is selected */
                rrc_cn_domain_identity_e_type ddt_cn_domain_id = RRC_CS_DOMAIN_CN_ID;
                /* store the CN domain received in message in a temp variable
                   0 --> CS Domain, 1 --> PS Domain */
		            ddt_cn_domain_id = ((0x00 != b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu, 
                      DL_DCCH_R3_DDT_CN_DOMAIN_IE_POS,
                      NUM_OF_BITS_FOR_DL_DCCH_R3_DDT_CN_DOMAIN_IE))
                      ? RRC_PS_DOMAIN_CN_ID : RRC_CS_DOMAIN_CN_ID);

                WRRC_MSG1_HIGH("R3 DDT received with cnDomain: %d (0:CS, 1:PS)", ddt_cn_domain_id);

                /* if SMC was never received for this domain then return success */
                if (FALSE == rrcsmc_int_ciph_info.int_start_status_for_conn[ddt_cn_domain_id])
		            {
		              WRRC_MSG1_HIGH("Returning success to IP Check to forward msg to NAS as SMC is not recieved for cnDomain: %d", ddt_cn_domain_id);
                  return(SUCCESS);
                }
              }
             /* From 24.008:
                If any layer 3 signalling message is received, in either PS or CS domains, 
                as not integrity protected even though the integrity protection has been 
                activated in the MS by that domain in the network, then the lower layers 
                shall discard this message (see 3GPP TS 25.331). */
              WRRC_MSG0_HIGH("Ignoring NOT integrity protected DDT msg: either r3 not selected or SMC already recieved for cnDomain in DDT");
              return (FAILURE);
            }
            break;

          case T_rrc_DL_DCCH_MessageType_cellUpdateConfirm:
          case T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration:
          case T_rrc_DL_DCCH_MessageType_radioBearerRelease:
          case T_rrc_DL_DCCH_MessageType_radioBearerSetup:
          case T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration:
          case T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration:
          case T_rrc_DL_DCCH_MessageType_utranMobilityInformation:
          case T_rrc_DL_DCCH_MessageType_uraUpdateConfirm:

            /* Check whether Integrity Protection Mode  info is present for the messages
               that can carry SRNS relocation info.. if they are present, the don't check
               integrity for the message */

            if (TRUE == 
                         rrcsmc_is_integrity_prot_mode_info_present(
                                             cmd_ptr->cmd.downlink_sdu_ind.dl_sdu, msg_type))
            {
               /* Call the ASN.1 decoder to decode the message. */
              cmd_ptr->cmd.downlink_sdu_ind.decoded_msg =
                    rrcasn1_decode_pdu(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,(int )
                                       cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type,
                                       cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length
                          );

              if (NULL != cmd_ptr->cmd.downlink_sdu_ind.decoded_msg)
              {
                /* if it reaches here then it means that its a SRNS relocation scenario
                   and needs to be integrity checked later */
                /* No need to release the memory allocated to 'decode_msg' because it will be freed
                   at the end while returning control to dispatcher. */
                /* Special care required (check for 'decode_msg' not equal to NULL while decoding it
                   later. */
                if (((OC_NOT_SET == rrcllc_get_ordered_config_status()) 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
                     && (FALSE == rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
                     ) && (FALSE == rrcsmc_is_security_reconfig_active()))
                {
                  /* Take back-up of security variables only if security as well as reconfig 
                     is not in progress because this message will result in failure and backup would 
                     have already been taken */
                  rrcsmc_backup_global_security_variables();
                }

                return SUCCESS;
              }
              else
              {
                /* if 'decoded_msg' is NULL then it means its a ASN1 decode failure, so go ahead and
                  dont consider it a SRNS relocation scenario (this condition can happen because of
                  ciphering issue....integrity check for this message is expected to fail. */
                WRRC_MSG0_HIGH("ASN1 decode failure. Probable garbled message. Integrity check is expected to fail");
              }
            }
            else
            {
                /* save the security variables when its a configuration message, 
                   back needs to be restored while reverting back to old configuration */
              if (((OC_NOT_SET == rrcllc_get_ordered_config_status()) 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
                  &&(FALSE == rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
                   ) && (FALSE == rrcsmc_is_security_reconfig_active()))
              {
                /* NOTE: we are not taking back up of security variables when config message is received
                   in emergency call + security is not enabled. Since it looks like taking a backup will result
                   in some code duplication and unnecessary exectution of that code everytime when
                   message is not integrity protected. Since its very rare scenario <need to check?>, so 
                   we can avoid taking back up of security variables for this scenario. */
                rrcsmc_backup_global_security_variables();
              }
            }
            break;

          default:
            break;
        }
        break;

      default:
        MSG_MED("Integrity Check Info Needed",0,0,0);
        return FAILURE;
    } 
/*lint +e527*/    

    /*get the rb_id from lc_id*/
    rb_id = rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id);

    /*so this message should be integrity protected. check if IntegrityCheckInfo IE
     is present in the message, also Integrity Check Info contents should be copied
     into the local variable int_check_info*/
    if ( rrcsmc_process_dl_integrity_check_info(
                                               cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                               cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length,
                                               &int_check_info,
                                               rb_id) == FAILURE)
    {
      WRRC_MSG0_ERROR("Cant do integrity check as either integrity check info is missing or msg len is invalid");
      return FAILURE;
    }


    local_cn_domain = rrcsmc_int_ciph_info.latest_config_cn_id;

    /*check if we received modify, then also check if activation time is pending
      for this rb_id, if yes for both*/
    if(rb_id < RRC_MAX_SRB)
    {
      if (( integrity_protection_info.modify_received == TRUE) &&
           ( integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
             dl_count_i_act_time_pending[rb_id] == TRUE) )
      {
        is_dl_act_time_expired = RRC_CHECK_FOR_IP_ACT_TIME_EXPIRY(
                  (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.
                  latest_config_cn_id_old].dl_count_i[rb_id] & RRC_SEQ_NUM_BIT_MASK),
                         integrity_protection_info.dl_act_seq_num[rb_id], 
                         int_check_info.msg_seq_num);

        /*check to see if activation time has been reached, this is checked whether act time
          matches or if SN > Act-time by a value of 8 or less(half the SN range)*/
        if (TRUE == is_dl_act_time_expired)
        {
          /*reset the activation time pending flag*/
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
          dl_count_i_act_time_pending[rb_id] = FALSE;

          integrity_protection_info.dl_latest_config_cn_id_old_1_valid[rb_id] = FALSE;

          /*set the default init flag*/
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
          dl_count_i_init_flag[rb_id] = FALSE;

          /*if new keys have been received for this domain, then set the HFN part of
              count_i to 0, and continue using seq_num*/
          /* check if new keys has already been processed because of receipt of SMC on SRB2 or not, 
             if it is TRUE then it means that SMC is not received for last Authentication Request, 
             hence no need to touch new key flag for UL and
             also check whether new keys have been received and not activated for this SRB */
          if ((FALSE == integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
            dl_new_key_flag_per_rb[DCCH_AM_RADIO_BEARER_ID]) &&
            (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
              dl_new_key_flag_per_rb[rb_id]==TRUE))
          {
            /*reset new keys flag*/
            integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
            dl_new_key_flag_per_rb[rb_id]=FALSE;
          }
          
          /*generate count_i with HFN part set to 0 and old domain SEQ_NUM*/
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
          dl_count_i[rb_id] = 
            rrcsmc_convert_START_to_count_c_i(
              rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id].hfn,
              (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.
               latest_config_cn_id_old].dl_count_i[rb_id] & RRC_SEQ_NUM_BIT_MASK) );

          MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH," DL act exp lcn_old %d lcn %d CI lcn_old %d CI lcnd %d lcn HFN %d ActSN %d MSN %d RBID%d",
          rrcsmc_int_ciph_info.latest_config_cn_id_old,
          rrcsmc_int_ciph_info.latest_config_cn_id,
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].dl_count_i[rb_id],
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].dl_count_i[rb_id],
          rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id].hfn,
          integrity_protection_info.dl_act_seq_num[rb_id],
          int_check_info.msg_seq_num,
          rb_id);

        }
        else
        {
          using_old_config_flag = TRUE;
          /*if activation time has not been reached for this rb_id, continue using old
              cn domain */
          if (FALSE == integrity_protection_info.dl_latest_config_cn_id_old_1_valid[rb_id])
          {
            local_cn_domain     = rrcsmc_int_ciph_info.latest_config_cn_id_old;
            /* if old and new cn domain are the same, say due to authentication foll. by SMC,
               then we need to use old keys stored in backup.. else don't bother switching
               between old and new keys, because for a different domain, after auth, latest
              keys will be in current db and hence no need to set this flag
              For scenario where keys are received but no SMC to activate the keys  then old keys needs to be used
            eg - CS Keys1 are in use now auth is done which gives new CS keys2 and PS keys2. Now SMC for PS domain comes
            so before the activation time we should use CS keys1*/
            if(integrity_protection_info.count_i_info[local_cn_domain].dl_new_key_flag_per_rb[rb_id]==TRUE)
            {
              MSG_HIGH("Using old key local_cn_domain: %d rrcsmc_int_ciph_info.latest_config_cn_id: %d key: for local_cn_domain: %d ", 
                local_cn_domain, rrcsmc_int_ciph_info.latest_config_cn_id, integrity_protection_info.count_i_info[local_cn_domain].
                dl_new_key_flag_per_rb[rb_id]);
              using_old_keys_flag = TRUE;
  	          use_latest_config_cn_id_old_1_keys_flag = FALSE;
            }
            else
            {
              MSG_HIGH("Dont use old keys local_cn_domain: %d rrcsmc_int_ciph_info.latest_config_cn_id: %d key for local_cn_domain: %d ", 
                local_cn_domain, rrcsmc_int_ciph_info.latest_config_cn_id, integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
                dl_new_key_flag_per_rb[rb_id]);
            }
          }
          else
          {
            WRRC_MSG1_HIGH("Using latest_config_cn_id_old_1 and old1-IK-key: %d[0:CS, 1:PS]", rrcsmc_int_ciph_info.latest_config_cn_id_old_1);
            local_cn_domain = rrcsmc_int_ciph_info.latest_config_cn_id_old_1;
  	        use_latest_config_cn_id_old_1_keys_flag = TRUE;
          }      
        }
      }

      /*copy hfn to a local variable for further processing*/
      local_hfn= integrity_protection_info.count_i_info[local_cn_domain].
                 dl_count_i[rb_id];      

      /*this is not the first message on this rb_id, get the sequence num
        from the stored count_i*/
      local_seq_no = (local_hfn & RRC_SEQ_NUM_BIT_MASK);

      /*if this is the first time we are receiving a message on this SRB then
        generate count_i with the stored hfn and the received sequence number*/
      if (integrity_protection_info.count_i_info[local_cn_domain].
          dl_count_i_init_flag[rb_id] == FALSE)
      {
        /*set the init_flag to TRUE*/  
        integrity_protection_info.count_i_info[local_cn_domain].
        dl_count_i_init_flag[rb_id] = TRUE;
      }
      else
      {
        /*if the rcvd sequence number is the same as the previous message sequence
          number stored, then do not process this message further.  just return*/
        if (local_seq_no == int_check_info.msg_seq_num)
        {
          WRRC_MSG2_HIGH("Duplicate SN %d for RB_ID %d",local_seq_no, rb_id);
          return FAILURE;
        }
      }      
    }

    /*if the stored seq number is greater than the one received in the msg
      then a rollover has occured.  Increment the hfn part of the number
      only.
      HFN should NOT be incremented if 'is_dl_act_time_expired' is set to
      TRUE, AND activation time is NOT equal to zero AND activation time is 
      less than current RRC-SN
      */
    if (local_seq_no > int_check_info.msg_seq_num)
    {
      /*increment hfn only*/
      local_hfn >>=4;
      local_hfn++;
      local_hfn <<=4;
      MSG_HIGH("Local SN %d MSN %d HFN after rollover %d",local_seq_no, int_check_info.msg_seq_num,local_hfn);			
    }

    /*generate the new count_i with the received sequence number and the hfn*/
    local_count_i = 
    (uint32)((local_hfn & RRC_HFN_BIT_MASK) | 
             (int_check_info.msg_seq_num & RRC_SEQ_NUM_BIT_MASK) 
             );

    /*set the keys to old*/
    if (using_old_keys_flag == TRUE)
    {
      rrcf9_init_key_values(
        rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key_old,
        rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].integrity_key_old);
    }
    else if ((rb_id < RRC_MAX_SRB) && (TRUE == use_latest_config_cn_id_old_1_keys_flag))
    {
      
      /* write integrity keys associated with cn_id_old_1 in both the domains (to avoid unnecessary checks) */
      rrcf9_init_key_values(
        integrity_protection_info.integrity_key_for_cn_id_old_1[rb_id],
        integrity_protection_info.integrity_key_for_cn_id_old_1[rb_id]);
    }

    /*call the f9 engine to generate MAC for the dl message*/
    local_mac = rrcf9_generate_mac_for_message(
                  local_count_i,
                  RRC_INTEGRITY_DIR_DOWNLINK,
                  cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                  (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8),
                  local_cn_domain,
                  rrcsmc_int_ciph_info.saved_integrity_algo[local_cn_domain]
                  );

    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"maci gen = 0x%x int_check_info.msg_auth_code = 0x%x CN domain %d[0=CS,1=PS], Integrity Algo = %d[0=Invalid,1=UIA1,2=UIA2] ",
    local_mac,
    int_check_info.msg_auth_code,
    local_cn_domain,
    rrcsmc_int_ciph_info.saved_integrity_algo[local_cn_domain]
    );

    /*set the keys back to new*/
    if ((using_old_keys_flag == TRUE) || (TRUE == use_latest_config_cn_id_old_1_keys_flag))
    {
      rrcf9_init_key_values(
        rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key,
        rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].integrity_key);
    }

    /*if the MAC's match then we have successfully checked the integrity of
      this message.  update the count_i for this rb_id*/
    if (local_mac == int_check_info.msg_auth_code)
    {
      if(rb_id < RRC_MAX_SRB)
      {
        /*update count_i stored in integrity_protection_info global var*/
        integrity_protection_info.count_i_info[local_cn_domain].
        dl_count_i[rb_id] = local_count_i;

        /*since we r still using old domain.. update sn of the new domain too
               as SN is common to both domains.. only HFN part is different and will
         be taken care of when activation time is reached
         Made changes to store the received MSN instead of the local_seq_no
         in dl count-i because local_seq_no is not re-initialized incase of 
         rollover */
        if(using_old_config_flag == TRUE)
        {
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
            dl_count_i[rb_id] =
            ((integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
             dl_count_i[rb_id] & RRC_HFN_BIT_MASK) |
            (int_check_info.msg_seq_num & RRC_SEQ_NUM_BIT_MASK));
        }

        WRRC_MSG0_MED("DL IP check successful");
      }
      return SUCCESS;
    }
    else
    {
      ERR("DL IP check failed. FRESH used: 0x%x, COUNT-I used: 0x%x, old_keys_used: %d", 
        saved_srns_info.fresh_old, local_count_i, using_old_keys_flag);
      WRRC_MSG1_ERROR("use_latest_config_cn_id_old_1_keys_flag: %d", use_latest_config_cn_id_old_1_keys_flag);
      /*Logging the IK keys used for MAC-I generation*/
      PRINT_INT_KEY(rrcsmc_int_ciph_info.int_ciph_info[local_cn_domain])
      return FAILURE;
    }
  }
  else
  {
    return(SUCCESS);
  }
}/*rrcsmc_check_downlink_integrity*/
/*===========================================================================

FUNCTION rrcsmc_append_mac_seq_no_to_ul_sdu

DESCRIPTION
  RRCSMC receives MAC and seq_no to be appended to UL SDU, appends the same
  to the UL SDU 

DEPENDENCIES
  None.

RETURN VALUE
  None. 

SIDE EFFECTS
  None

===========================================================================*/
static void rrcsmc_append_mac_seq_no_to_ul_sdu
(
byte* ul_sdu, 
uint32 mac, 
uint32 seq_num
)
{
  uint32 sn_type_pos;                     /* Local Variable to store seq num
                                             type position*/                                 
  uint32 mac_type_pos;                    /* Local Variable to store mac 
                                             type position*/                                 

  /*peek into byte stream pack the MAC in appropriate position*/
  mac_type_pos = INTEGRITY_CHECK_INFO_PRESENT_POS + 
                 NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT;

  b_packd(mac, ul_sdu, (uint16)mac_type_pos, RRC_MAC_LENGTH_IN_BITS);

  /*peek into byte stream and pack seq num in appropriate position*/
  sn_type_pos =  INTEGRITY_CHECK_INFO_PRESENT_POS +
                 NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT+
                 RRC_MAC_LENGTH_IN_BITS;

  b_packb((uint8)seq_num, ul_sdu, (uint16)sn_type_pos, RRC_SEQ_NUM_LENGTH_IN_BITS);

}/*rrcsmc_append_mac_seq_no_to_ul_sdu*/
/*===========================================================================

FUNCTION rrcsmc_append_uplink_integrity_check_info

DESCRIPTION
  RRCSMC receives the encoded uplink SDU and adds Integrity Check Info to
  the UPLINK SDU.
  The send chain will call this function in SMC before sending a 
  message on a UL Signaling Radio Bearer to append the Integrity Check
  Info in the message.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns SUCCESS if IntegrityCheckInfo is appended successfully
  This function returns FAILURE if if it is failed to append IntegrityCheckInfo .


SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_uplink_integrity_check_info 
(
byte*  ul_sdu,
uint32 ul_sdu_length,
uint32 rb_id
)
{
  uint32 local_seq_num;              /*local vars for sdu length and seq num*/
  uint32 local_mac;                  /*local var to store mac*/
  rrc_cn_domain_identity_e_type  local_cn_domain;
  boolean                        using_old_keys_flag=FALSE;

  /*initialize local var it to latest cn domain*/
  local_cn_domain = rrcsmc_int_ciph_info.latest_config_cn_id;

  if(rb_id >= RRC_MAX_SRB)
  {
    WRRC_MSG1_ERROR(" rb_id is %d, greater that RRC_MAX_SRB",rb_id);
    return FAILURE;
  }

  /*if we have received a modify command and activation time is pending*/
  if ( ( integrity_protection_info.modify_received == TRUE) &&
       ( integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
         ul_count_i_act_time_pending[rb_id] == TRUE) )
  {
    /*check if next SEQ_NUM is equal to pending activation time*/
    if (((integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
          ul_count_i[rb_id]+1) & RRC_SEQ_NUM_BIT_MASK ) == 
        integrity_protection_info.ul_act_seq_num[rb_id])
    {
      /*reset pending flag*/
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      ul_count_i_act_time_pending[rb_id] = FALSE;

      /* check if new keys has already been processed because of receipt of SMC on SRB2 or not, 
         if it is TRUE then it means that SMC is not received for last Authentication Request, 
         hence no need to touch new key flag for UL and
         also check whether new keys have been received and not activated for this SRB */
      if ((FALSE == integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
          dl_new_key_flag_per_rb[DCCH_AM_RADIO_BEARER_ID]) && 
          (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
          ul_new_key_flag_per_rb[rb_id]==TRUE))
      {
        /*reset new keys flag*/
        integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
        ul_new_key_flag_per_rb[rb_id]=FALSE;

      }
      
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      ul_count_i[rb_id] = 
        rrcsmc_convert_START_to_count_c_i(
          rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id].hfn,
          (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.
           latest_config_cn_id_old].ul_count_i[rb_id] & RRC_SEQ_NUM_BIT_MASK) );

    }
    else
    {
      /*if we haven't reached activation time yet, set local_cn_domain 
        and flag to old*/
      local_cn_domain = rrcsmc_int_ciph_info.latest_config_cn_id_old;

      /*update new cn domain's seq num to take care of condition when act time
        is not reached for old.. but we get another modify.. hence old and new
        SN's are out of sync*/
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
        ul_count_i[rb_id]++;

      /* if old and new cn domain are the same, say due to authentication foll. by SMC,
         then we need to use old keys stored in backup.. else don't bother switching
         between old and new keys, because for a different domain, after auth, latest
        keys will be in current db and hence no need to set this flag
        For scenario where keys are received but no SMC to activate the keys  then old keys needs to be used
      eg - CS Keys1 are in use now auth is done which gives new CS keys2 and PS keys2. Now SMC for PS domain comes
      so before the activation time we should use CS keys1*/
      if(integrity_protection_info.count_i_info[local_cn_domain].ul_new_key_flag_per_rb[rb_id]==TRUE)
      {
        using_old_keys_flag = TRUE;
       
        MSG_HIGH("Using old key local_cn_domain: %d rrcsmc_int_ciph_info.latest_config_cn_id: %d key: for local_cn_domain: %d ", 
         local_cn_domain, rrcsmc_int_ciph_info.latest_config_cn_id, 
         integrity_protection_info.count_i_info[local_cn_domain].ul_new_key_flag_per_rb[rb_id]);
      }
      else
      {
         MSG_HIGH("Dont use old keys local_cn_domain: %d rrcsmc_int_ciph_info.latest_config_cn_id: %d key for local_cn_domain: %d ", 
          local_cn_domain, rrcsmc_int_ciph_info.latest_config_cn_id, integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
          ul_new_key_flag_per_rb[rb_id]);
      }
    }
  }


  /*increment the ul sn*/
  integrity_protection_info.count_i_info[local_cn_domain].
  ul_count_i[rb_id]++;

  /*set the keys to old*/
  if (using_old_keys_flag == TRUE)
  {
    rrcf9_init_key_values(
      rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key_old,
      rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].integrity_key_old);
  }



  /*call the f9 driver function to generate MAC for the received UL SDU*/
  local_mac= rrcf9_generate_mac_for_message(
               integrity_protection_info.count_i_info[local_cn_domain].
               ul_count_i[rb_id],
               RRC_INTEGRITY_DIR_UPLINK,
               ul_sdu,
               ul_sdu_length,
               local_cn_domain,
               rrcsmc_int_ciph_info.saved_integrity_algo[local_cn_domain]);

MSG_HIGH("maci gen = 0x%x  CN domain %d[0=CS,1=PS], Integrity Algo = %d[0=Invalid,1=UIA1,2=UIA2] ",
                  local_mac,local_cn_domain,
                  rrcsmc_int_ciph_info.saved_integrity_algo[local_cn_domain]);
   

  /*set the keys back to new*/
  if (using_old_keys_flag == TRUE)
  {
    rrcf9_init_key_values(
      rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key,
      rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].integrity_key);
  }

  /*get the seq_num for this message from count_i by doing a bitwise and with 0xF
    to get the seq_num.  count_i  = hfn[28bits]seq_num[4 bits]*/
  local_seq_num = integrity_protection_info.count_i_info[local_cn_domain].
                  ul_count_i[rb_id];
  local_seq_num &= 0xF;

  rrcsmc_append_mac_seq_no_to_ul_sdu(ul_sdu, local_mac, local_seq_num);

  return SUCCESS;
}/*rrcsmc_append_uplink_integrity_check_info*/
/*===========================================================================

FUNCTION rrcsmc_check_integrity_status

DESCRIPTION
  This function checks if Integrity Protection is already started or not.
  Other procedures may call this function to check the Start/NotStart
  Status of Integrity Protection.

DEPENDENCIES
  None.

RETURN VALUE
  Returns RRCSMC_IP_STARTED if Integrity Protection is started.
  Returns RRCSMC_IP_NOT_STARTED if Integrity Protection is not started.


SIDE EFFECTS
  None

===========================================================================*/
rrcsmc_ip_status_e_type rrcsmc_check_integrity_status
(
void
)
{
  if (nv_integrity_enabled)
  {
    /*check whether integrity is started or not*/
    if (integrity_protection_info.ip_status == RRCSMC_SECURITY_STARTED )
    {
      return RRCSMC_IP_STARTED;
    }
    else
    {
      return RRCSMC_IP_NOT_STARTED;
    }
  }
  else
  {
    return(RRCSMC_IP_NOT_STARTED);
  }
}/*rrcsmc_check_integrity_status*/
/*===========================================================================

FUNCTION rrcsmc_check_integrity_for_security_mode_command

DESCRIPTION
  This function validates Integrity Protection for Security Mode Command.
  It also updates the FRESH and KEYS if it is the START of Integrity.
  It also updates the KEY values if we have received new keys.
  
DEPENDENCIES
  None

RETURN VALUE
  uecomdef_status_e_type : 
  Returns SUCCESS if Integrity Check for Security Mode Command 
  is successful.
  Returns FAILURE if Integrity Check for Security Mode Command
  is not successful
  
SIDE EFFECTS
  None
===========================================================================*/
static uecomdef_status_e_type rrcsmc_check_integrity_for_security_mode_command
(
rrc_cmd_type *cmd_ptr                    /* Pointer to the RRC Command */
)
{  
  rrc_DL_DCCH_Message *dcch_msg_ptr;         /* Pointer to Downlink DCCH Msg */

  rrc_RB_Identity rb_id;/*local var to store rb_id*/
  rrc_IntegrityProtectionModeInfo* integrityProtModeInfo;

  /*flag to indicate whether new keys are being*/  
  boolean rcvd_new_keys_flag=FALSE;

  /*local vars to store bunch of values used for manipulation in this func.only*/
  uint32 local_fresh, local_seq_no, old_seq_no; 
  uint32 local_count_i = 0, local_hfn;
  uint32 received_mac, local_mac;
  rrcsmc_integrity_check_info_type int_check_info = {0};

  /*local copy of domain id. Set it to cs domain to suppress warning*/
  rrc_cn_domain_identity_e_type domain_id=RRC_CS_DOMAIN_CN_ID;  


  /*get the dcch_msg_ptr for SMC as this message is already decoded*/
  dcch_msg_ptr =  (rrc_DL_DCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /*copy integrityProtectionModeInfo * to a local copy to make easy readability*/
  integrityProtModeInfo = &(dcch_msg_ptr->message.u.securityModeCommand.u.r3.
                            securityModeCommand_r3.integrityProtectionModeInfo);

  /*get the domain id*/
  if (dcch_msg_ptr->message.u.securityModeCommand.u.r3.
      securityModeCommand_r3.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
  {
    domain_id = RRC_CS_DOMAIN_CN_ID;
  }
  else if (dcch_msg_ptr->message.u.securityModeCommand.u.r3.
           securityModeCommand_r3.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
  {
    domain_id = RRC_PS_DOMAIN_CN_ID;
  }

  /*get the rb_id on which the message was received*/
  rb_id = rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id);

  if(rb_id < RRC_MAX_SRB)
  {
    
    local_seq_no = dcch_msg_ptr->integrityCheckInfo.rrc_MessageSequenceNumber;

    if (integrityProtModeInfo->integrityProtectionModeCommand.t == 
        T_rrc_IntegrityProtectionModeCommand_modify)
    {
      if (integrity_protection_info.count_i_info[domain_id].
          dl_new_key_flag_per_rb[rb_id] == TRUE)
      {
        /*set this local flag to true*/
        rcvd_new_keys_flag=TRUE;
        /*set hfn to zero, as we have to reset HFN part of start value to 0*/
        local_hfn = 0x0;
      }
      else if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id]
              == TRUE)
      {
        /*if the start_value_update flag is set then we have updated the start value
          due to an IDT, this value needs to be used for COUNT_I calculation*/
        local_hfn =  
          rrcsmc_convert_START_to_count_c_i(
            rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,
            0);      
      }
      else
      {
        local_hfn = integrity_protection_info.count_i_info[domain_id].
                    dl_count_i[rb_id] & RRC_MSB_20BIT_HFN_BIT_MASK;
      }

      /*get the seq num of the old domain*/
      old_seq_no = (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.
                    latest_config_cn_id].dl_count_i[rb_id] & RRC_SEQ_NUM_BIT_MASK);


      /*check if old sequence number is greater than current seq. no.  if that is the
        case, then there has been a rollover.. so increment the HFN by 1, as per 1808*/
      if (old_seq_no > local_seq_no)
      {
        /*increment hfn only*/
        local_hfn >>=4;
        local_hfn++;
        local_hfn <<=4;
      }
    }
    else
    {
      local_hfn = integrity_protection_info.count_i_info[domain_id].dl_count_i[rb_id];
    }

    /*generate count_i from hfn and sequence number*/
    local_count_i = 
    (uint32)((local_hfn & RRC_HFN_BIT_MASK) | 
             (local_seq_no & RRC_SEQ_NUM_BIT_MASK) 
             );

    /*get the value of fresh from the received message and initialize F9 engine.
      We can receive the value of FRESH only if START chosen*/
    if (integrityProtModeInfo->integrityProtectionModeCommand.t == 
        T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection)
    {
      local_fresh = b_unpackd(
        integrityProtModeInfo->integrityProtectionModeCommand.u.startIntegrityProtection->
        integrityProtInitNumber.data, 
        RRC_DEFAULT_BIT_POS,
        RRC_FRESH_LENGTH_IN_BITS);  

      /*update fresh value for this connection in F9 engine*/
      rrcf9_init_fresh_per_connection(local_fresh);
    }

    /*init the IK values for CS and PS domains and the fresh value for
      this connection*/
    rrcf9_init_key_values(
      rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key,
      rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].integrity_key);


    (void) rrcsmc_process_dl_integrity_check_info(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                                  cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length,
                                                  &int_check_info,rb_id);
  }
  /*Update the received message*/
  received_mac = int_check_info.msg_auth_code;

  /*call the f9 engine to generate MAC for the dl message*/  
  local_mac = rrcf9_generate_mac_for_message(
                local_count_i,
                RRC_INTEGRITY_DIR_DOWNLINK,
                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length*8),
                domain_id,
                rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
                &integrityProtModeInfo->integrityProtectionAlgorithm,
                MSG_REL99));

   MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"maci gen = 0x%x received_mac = 0x%x CN domain %d[0=CS,1=PS], Integrity Algo = %d[0=Invalid,1=UIA1,2=UIA2] ",
   local_mac,
   received_mac,
   domain_id,
    rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
                &integrityProtModeInfo->integrityProtectionAlgorithm,
                MSG_REL99)
   );


  /*if the generated and received mac are the same, update our copy of count_i
    and set the init_flag to true*/
  if ( local_mac == received_mac)
  {
    if(rb_id < RRC_MAX_SRB)
    {
      /* new keys flag wont be set to TRUE is received type is integrity STARTED */
      if (rcvd_new_keys_flag == TRUE)
      {
        /*reset new keys flag.  we are changing it here to be sure that the SMC
            command was successfully integrity checked*/
        integrity_protection_info.count_i_info[domain_id].
        dl_new_key_flag_per_rb[rb_id] = FALSE;

        /*any previously sent start value is obliterated.. we should start using 0's from
          now onwards.. hence set start_value_update_flag to false*/
        rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;      

        /*reset the hfn values to 0's if authentication is done and UE is
         *in WCDMA mode in dch/fach
         */
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[0]=0x0;
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[1]=0x0;
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[2]=0x0;

        MSG_HIGH("Started using new keys, CIPH START VAL: 0x%x 0x%x 0x%x", 
           rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[0],
           rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[1],
           rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[2]);
        
      }
      if((domain_id == RRC_CS_DOMAIN_CN_ID) && (rrcsmc_int_ciph_info.int_ciph_info[domain_id].
      ciph_keys_info.new_key == TRUE))
      {
        rrciho_send_cipher_sync_ind();
      }

      integrity_protection_info.count_i_info[domain_id].dl_count_i[rb_id] = local_count_i;

      integrity_protection_info.count_i_info[domain_id].
      dl_count_i_init_flag[rb_id] = TRUE;

      /*reset the start_value_update flag as we already passed integrity check
        successfully for this domain, also copy the updated start value to the
        database and initialize rb2, as there is no act time for rb2..
        also start_value_update_flag will be true only for Integrity Modify,
        as START value will be updated for integrityStart in smc_initial substate
        function*/
      if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id] == TRUE &&
         !(rcvd_new_keys_flag))
      {
        /*copy the latest start value sent in IDT or CU message*/
        WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
                     sizeof(byte) * RRC_MAX_HFN_LENGTH,
                     rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,      
                     RRC_MAX_START_LENGTH);
        
        /*since there is no pending activation time for RB2, update COUNT_I info for new 
          CN domain here when successfully check for SMC is done*/
        integrity_protection_info.count_i_info[domain_id].ul_count_i[2] =         
          rrcsmc_convert_START_to_count_c_i(
            rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
            integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
            ul_count_i[2]);

        rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;
      }
      else if(!rcvd_new_keys_flag) 
      {
          /*since there is no pending activation time for RB2, update COUNT_I info for new 
          CN domain here when successfully check for SMC is done*/
        integrity_protection_info.count_i_info[domain_id].ul_count_i[2]= 
        (integrity_protection_info.count_i_info[domain_id].ul_count_i[2] & RRC_SEQ_NUM_BIT_MASK )| 
        (integrity_protection_info.count_i_info[domain_id].ul_count_i[2] & RRC_MSB_20BIT_HFN_BIT_MASK);
      }

      //set the Integrity START status for this domain ID to true.
      //only if this flag is set, then we will extract latest start values
      //at connection release time.
      rrcsmc_int_ciph_info.int_start_status_for_conn[domain_id] = TRUE;
    }
    return SUCCESS;
  }
  else
  {
    
    /*Logging the IK keys used for MAC-I generation*/
    PRINT_INT_KEY(rrcsmc_int_ciph_info.int_ciph_info[domain_id])
    
    /*if we have received new keys and this SMC message failed Integrity
        test, then we revert back to the old keys as we will not be using 
        the new keys or new Security Configuration*/
    if (rcvd_new_keys_flag == TRUE)
    {
      rrcf9_init_key_values(
       rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key_old,
       rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].integrity_key_old);
    }
    return FAILURE;
  }
}/*rrcsmc_check_integrity_for_security_mode_command*/
/*===========================================================================

FUNCTION rrcsmc_update_int_activation_time

DESCRIPTION
  This function processes a Security Mode Command with Modify. Copies the
  DL Activation Info time in count_i_new var and also UL activatioin time
  in count_i_new
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void rrcsmc_update_int_activation_time
(
void
)
{
  uint32 i;    /*counter in for loop*/

  /*initialize count_i for NEW latest CN domain*/
  for ( i=0; i < RRC_MAX_SRB; i++)
  {
    if ((TRUE == integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].dl_count_i_act_time_pending[i]) &&
		    (FALSE == integrity_protection_info.dl_latest_config_cn_id_old_1_valid[i]))
    {
      MSG_HIGH("dl_latest_config_cn_id_old_1_valid for SRB: %d, saving keys for domain latest_config_cn_id_old_1:%d(0:CS) ciph_keys_info.new_key: %d, if 1 using old IK keys else latest IK keys ", 
	  	         i, rrcsmc_int_ciph_info.latest_config_cn_id_old_1, rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id_old_1].ciph_keys_info.new_key);

      integrity_protection_info.dl_latest_config_cn_id_old_1_valid[i] = TRUE;

      if (TRUE ==
  	  	rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id_old_1].ciph_keys_info.new_key)
      {
        /* check whether new keys are received for this cn domain or not - CK and IK will be received at the same time for a given domain  
		       NOTE: we may need some other check above for new keys ????? */

        WCDMA_MEMCPY(integrity_protection_info.integrity_key_for_cn_id_old_1[i],
                     sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                     rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id_old_1].integrity_key_old,
                     RRC_MAX_INTEGRITY_KEY_LENGTH);      	
      }
      else
      {
        WCDMA_MEMCPY(integrity_protection_info.integrity_key_for_cn_id_old_1[i],
                     sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                     rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id_old_1].integrity_key,
                     RRC_MAX_INTEGRITY_KEY_LENGTH);
      }
    }

    /*initialize downlink count_i dl_activation_info time sent by utran
    for this domain */
    integrity_protection_info.dl_act_seq_num[i] = 
    saved_security_mode_command_msg.srb_int_dl_act_time[i].seq_num;

    /*initialize uplink count_i activation_info for this domain.  Add 
      RRC_UL_ACT_TIME_OFFSET to the current value of SEQ NUMBER for all rb's 
      other than RB_0.  for RB0 add (N302+2).  For RB2 add just 1 as
      we want to start Integrity Protection on the new configuration
      immediately.  RRC_UL_ACT_TIME_OFFSET is set to 1 currently..*/
    if (i == CCCH_RADIO_BEARER_ID)
    {
      integrity_protection_info.ul_act_seq_num[i] =  
      (( (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
          ul_count_i[i] & RRC_SEQ_NUM_BIT_MASK) + RRC_GET_N302() + 2) % 16);
    }
    else if( i == DCCH_AM_RADIO_BEARER_ID )
    {
      integrity_protection_info.ul_act_seq_num[i] =  
      (( (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
          ul_count_i[i] & RRC_SEQ_NUM_BIT_MASK) + 1) % 16);
    }
    else if (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.
             latest_config_cn_id_old].ul_count_i_act_time_pending[i] != TRUE)
    {
      integrity_protection_info.ul_act_seq_num[i] =  
      (( (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
          ul_count_i[i] & RRC_SEQ_NUM_BIT_MASK) + 1) % 16);
    }

    /*for rb_id 2 do not set the act time pending flag.  no activation time
      is required for rb_id 2*/
    if (i != DCCH_AM_RADIO_BEARER_ID)
    {
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      ul_count_i_act_time_pending[i] = TRUE; 
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      dl_count_i_act_time_pending[i] = TRUE; 
    }
    else
    {
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      ul_count_i_act_time_pending[i] = FALSE; 
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      dl_count_i_act_time_pending[i] = FALSE; 
    }
    
    /*copy old cn domain's Seq_Num to new domain's ul_count_i as seq_num is common across
      domains.. only HFN part differs.. the HFN will be adjusted when we hit activation
      time in the uplink/downlink integrity processing*/
    integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      ul_count_i[i] =
      ((integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
       ul_count_i[i] & RRC_HFN_BIT_MASK) |
       (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
       ul_count_i[i] & RRC_SEQ_NUM_BIT_MASK));
    
    //don't update this for rb_id = 2 on downlink, as this will already be done in smc.. 
    //rb_id 2 is handled differently, as count_i's are already calculated at reception
    // of SMC itself
    if(i != DCCH_AM_RADIO_BEARER_ID)
    {
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
        dl_count_i[i] =
        ((integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
         dl_count_i[i] & RRC_HFN_BIT_MASK) |
         (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
         dl_count_i[i] & RRC_SEQ_NUM_BIT_MASK));  
    }
  }
}/*rrcsmc_update_int_activation_time*/
/*===========================================================================

FUNCTION rrcsmc_validate_security_mode_command

DESCRIPTION
  This function validates security mode command for critical xtensions
  and also to check to see if either ciphering or integrity mode info
  is present in the security mode command
  
DEPENDENCIES
  None

RETURN VALUE
  rrcsmc_config_status_e_type : It returns INVALID_CONFIG if Critical 
  extensions or any other checks fail.  Else VALID_CONFIG is returned
  
SIDE EFFECTS
  None
===========================================================================*/
static rrcsmc_config_status_e_type rrcsmc_validate_security_mode_command
(
rrc_DL_DCCH_Message *msg_ptr              /* Pointer to Downlink DCCH Msg */
)
{
  /* Check if critical extensions are present */
  if (msg_ptr->message.u.securityModeCommand.t == 
      T_rrc_SecurityModeCommand_later_than_r3)
  {
    rrcsmc_failure_type = RRCSMC_PROTOCOL_ERROR;   
    rrcsmc_prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;   
    WRRC_MSG0_ERROR("Critical extensions are not supported");
    return RRCSMC_INVALID_CONFIGURATION;
  }

  /*if any security reconfiguration is currently in progress return failure
    indicating that this is a Simultaneous Reconfiguration*/
  if( (integrity_protection_info.reconfig == TRUE) || 
      (ciphering_info.reconfig == TRUE)
    )
  {
    WRRC_MSG0_ERROR("Incompatible Simultaneous Ciph Reconfig");
    rrcsmc_failure_type = RRCSMC_INCOMPATIBLE_SIMULTANEOUS_RECONFIG;
    return RRCSMC_INVALID_CONFIGURATION;
  }

  /*if both nv items are not enabled, then do not check any more IE's just
    return success*/
  if((!nv_ciphering_enabled) && (!nv_integrity_enabled))
  {
    return RRCSMC_VALID_CONFIG;
  }

  /*Either ciphering or integrity protection mode info ie's should be present
    in message received by UE*/
  if((msg_ptr->message.u.securityModeCommand.u.r3.securityModeCommand_r3.m.cipheringModeInfoPresent) == 0)
  {
    rrcsmc_int_ciph_info.ciphering_info_received = FALSE;
  }
  else
  {
    rrcsmc_int_ciph_info.ciphering_info_received = TRUE;  
  }
  
  if((msg_ptr->message.u.securityModeCommand.u.r3.securityModeCommand_r3.m.integrityProtectionModeInfoPresent) == 0)
  {  
    rrcsmc_int_ciph_info.integrity_info_received = FALSE;  
  }
  else
  {
    rrcsmc_int_ciph_info.integrity_info_received = TRUE;    
  }

  /* if Ciphering Mode IE's and Integrity Protection Mode IE's
     are not included, then send SMC Failure message */  
  if( (rrcsmc_int_ciph_info.integrity_info_received == FALSE ) &&
      (rrcsmc_int_ciph_info.ciphering_info_received == FALSE )
    )
  {
     WRRC_MSG0_ERROR("Incomplte Configuration"); 
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  return RRCSMC_VALID_CONFIG;
}/* rrcsmc_validate_security_mode_command */
/*===========================================================================

FUNCTION rrcsmc_validate_ciph_security_mode_command

DESCRIPTION
  This function validates ciphering part of SECURITY MODE COMMAND.
  
DEPENDENCIES
  None

RETURN VALUE
  rrcsmc_config_status_e_type : It returns INVALID_CONFIG if Critical 
  extensions or any other checks fail.  Else VALID_CONFIG is returned
  
SIDE EFFECTS
  None
===========================================================================*/
static rrcsmc_config_status_e_type rrcsmc_validate_ciph_security_mode_command
(
rrc_DL_DCCH_Message *msg_ptr              /* Pointer to Downlink DCCH Msg */
)
{
  uint32 count, i, local_lc_id;            /*vars used for count purposes*/
  boolean tm_rb_present;                   /*flag used to check if tm rb's are present*/
  rrc_CN_DomainIdentity local_domain_id;   /*domain id for which ciph config is being changed*/
  uint32 num_srbs_established = 3;         /*SRBs 1,2 and 3 are mandatory*/
  uint32 index=0,n_value=0;
  uint8 cipher_algo_suppported = 0;
  uecomdef_wcdma_cipher_algo_e_type cipher_algo_configured = UE_WCDMA_CIPHER_ALGO_NONE;

  /*local ptr to Ciph. Mode Info. */
  struct rrc_CipheringModeInfo* ciphModeInfo;

  /* local ptr to activation time list */
  struct rrc_RB_ActivationTimeInfoList  *local_rb_ciph_act_time_list = NULL;

  rrc_RB_Identity srb5_rb_id = RRCLLC_INVALID_RB_ID;

  /*set to FALSE*/
  tm_rb_present = FALSE;

  /* if Ciphering Mode IE's are not included, then send SMC Failure message */
  if ( (msg_ptr->message.u.securityModeCommand.u.r3.securityModeCommand_r3.m.cipheringModeInfoPresent) == 0 )
  {
     WRRC_MSG0_ERROR("Incomplte Configuration"); 
    rrcsmc_failure_type = RRCSMC_INCOMPLETE_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  /*check if activation times are present in do ciph activation.. they have to be present
    as SRB's are already started and have to be ciphered*/
  if ( (msg_ptr->message.u.securityModeCommand.u.r3.securityModeCommand_r3.cipheringModeInfo.m.
        rb_DL_CiphActivationTimeInfoPresent) == 0 )
  {
    /*lint -e793*/
     WRRC_MSG0_ERROR("rrc_CipheringModeInfo_rb_DL_CiphActivationTimeInfo_present not present");
    /*lint +e793*/
    rrcsmc_failure_type = RRCSMC_INCOMPLETE_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  /*get ptr to cipheringModeInfo. copy this to a local ptr to avoid 
    multiple indirections in later manipulations*/
  ciphModeInfo = &(msg_ptr->message.u.securityModeCommand.u.r3.
                   securityModeCommand_r3.cipheringModeInfo);

  /* Check to see if Ciph Activation time is included for all SRBs.Ignore SRB0
     since it is not ciphered.Check for SRBs 1,2 and 3 since they are mandatory.
     Check for RB4 only if it is already established */
  if(RRCLCM_RLC_LC_ID_NOT_FOUND != rrclcm_check_dl_rlc_lc_id(
                                   UE_LOGCHAN_DCCH,
                                   DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                   UE_MODE_ACKNOWLEDGED_DATA))
  {
    MSG_MED("SRB4 established",0,0,0);
    num_srbs_established = 4;
  }

  for(count = 1; count <= num_srbs_established; count++)
  {
    uint32 srb_found = 0;

    /* get the activation time list for RBs */
    local_rb_ciph_act_time_list = &(ciphModeInfo->rb_DL_CiphActivationTimeInfo);

    /* Check to see if this RB info is included in the ciph activation times list */
    n_value = local_rb_ciph_act_time_list->n;
    index = 0;
    while (n_value != 0)
    {
      if(count == local_rb_ciph_act_time_list->elem[index].rb_Identity)
      {
        MSG_MED("SRB id: %d included in ciph activation info in SMC",count,0,0);
        srb_found++;
      }
      index++;
      n_value--;
    }

    if(srb_found != 1)
    {
      WRRC_MSG2_ERROR("SRB id: %d is included %d times in ciph activation info in SMC",
        count, srb_found);
      rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
      return RRCSMC_INVALID_CONFIGURATION;
    }
  }
  
  index = 0;
  if(local_rb_ciph_act_time_list != NULL)
  {
    n_value = local_rb_ciph_act_time_list->n;
  }
  if(rrclcm_get_rb_id_if_dl_lc_type_exists(UE_LOGCHAN_TM_DCCH_DL, &srb5_rb_id) == RRCLCM_SUCCESS)
  {
    /* get the activation time list for RBs */
    local_rb_ciph_act_time_list = &(ciphModeInfo->rb_DL_CiphActivationTimeInfo);

    /* Check to see if this RB info is included in the ciph activation times list */
    while (n_value != 0)
    {
      if(srb5_rb_id == local_rb_ciph_act_time_list->elem[index].rb_Identity)
      {
        WRRC_MSG2_ERROR("SMC validation failed; SRB#5 Id = %d, RB ID: %d", srb5_rb_id, local_rb_ciph_act_time_list->elem[index].rb_Identity);
        rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
        return RRCSMC_INVALID_CONFIGURATION;
      }
      index++;
      n_value--;
    }
  }

  /*get the domain id of the latest configured CN domain for which we are
    configuring ciphering*/
  local_domain_id = msg_ptr->message.u.securityModeCommand.u.r3.
                    securityModeCommand_r3.cn_DomainIdentity;

  /*check to see if TM radio bearers are present.  only if present will we
    mandatorily check to see DPCH activation time is present.
    Also if the domain id is PS,check if SMC has 
    included all the PS RBs activation time in the message.If not
    UE should send a security mode failure and not honor the SMC */
  for ( count=0; count<MAX_RAB_TO_SETUP; count++ )
  {
    if (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID)
    {
      for (i=0; i< rrc_est_rabs.rabs[count].num_rbs_for_rab; i++)
      {
        if (rrc_est_rabs.rabs[count].rb_for_rab[i].rb_status ==
            RRC_RB_STARTED)
        {
          /*check if domain id is same as domain for which ciphering
            configuration is received for*/
          if(rrc_est_rabs.rabs[count].cn_domain == local_domain_id)
          {
            if(local_domain_id == rrc_CN_DomainIdentity_ps_domain) 
            {
              uint32 ps_rb_found = 0;

              /* get the activation time list for RBs */
              local_rb_ciph_act_time_list = &(ciphModeInfo->rb_DL_CiphActivationTimeInfo);

              /* Check to see if this RB info is included in the ciph activation times list */
              /* Initializing index and n_value to scan whole list */
              index = 0;
              n_value = local_rb_ciph_act_time_list->n;
              while (n_value != 0)
              {
                if(rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id ==
                    local_rb_ciph_act_time_list->elem[index].rb_Identity)
                {
                  ps_rb_found++;
                }
                index++;
                n_value--;
              }
              if(ps_rb_found != 1)
              {
                WRRC_MSG2_ERROR("RB id: %d is included %d times in ciph activation info in SMC",
                     rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id,
                     ps_rb_found);
                rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
                return RRCSMC_INVALID_CONFIGURATION;
              }
            }
            else /* CS domain */
            {
              /*copy to local vars to avoid multiple indirections*/
              local_lc_id = rrclcm_get_ul_rlc_lc_id(rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id);
  
              /*set TM radio bearers flag present to true*/
              if (local_lc_id < RRCLCM_UL_UM_START_IDX)
              {
                tm_rb_present=TRUE;
                break;
              }
            }
          }
        }/*if*/
      }/*for*/
    }/*if*/

    /*if TM radio bearers are present, then break right now as we don't have
      to loop through all rabs in est_rabs*/
    if (tm_rb_present == TRUE)
    {
      break;
    }
  }/*for loop till MAX_RAB_TO_SETUP*/

  /*if only TM radio bearers are present then mandatorily DPCH activation
    information should be present.*/
  if ( ((ciphModeInfo->m.activationTimeForDPCHPresent) == 0) && 
       (tm_rb_present == TRUE )
     )
  {
     WRRC_MSG0_ERROR("No DPCH Activation Time"); 
    rrcsmc_failure_type = RRCSMC_INCOMPLETE_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

    cipher_algo_suppported = UEA_1_SUPPORTED + UEA_0_SUPPORTED;
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
    if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && snow3g_security_algo_supported)
    {
      cipher_algo_suppported += UEA_2_SUPPORTED;
    }
#endif

  cipher_algo_configured = rrcsmc_get_int_cipher_algo_from_dl_sdu_ind(
              &(ciphModeInfo->cipheringModeCommand.u.startRestart),MSG_REL99);

  if(!(cipher_algo_suppported & cipher_algo_configured))
  {
    WRRC_MSG2_ERROR("Mismatch between cipher algo supported %d and configured %d", cipher_algo_suppported, cipher_algo_configured);  
    rrcsmc_failure_type = RRCSMC_UNSUPPORTED_CONFIG; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }
  
  return RRCSMC_VALID_CONFIG;
}/* rrcsmc_validate_ciph_security_mode_command */
/*===========================================================================

FUNCTION rrcsmc_validate_int_security_mode_command

DESCRIPTION
  This function validates integrity part of SECURITY MODE COMMAND.
  
DEPENDENCIES
  None

RETURN VALUE
  rrcsmc_config_status_e_type : It returns INVALID_CONFIG if Critical 
  extensions or any other checks fail.  Else VALID_CONFIG is returned
  
SIDE EFFECTS
  None
===========================================================================*/
static rrcsmc_config_status_e_type rrcsmc_validate_int_security_mode_command
(
rrc_DL_DCCH_Message *msg_ptr              /* Pointer to Downlink DCCH Msg */
)
{  
  /*local ptr to Int. Prot. Mode Info.  */
  struct rrc_IntegrityProtectionModeInfo* integrityProtModeInfo;
  uint8 int_algo_supported = 0;
  uecomdef_wcdma_integrity_protection_algorithm_e_type int_algo_configured = UE_WCDMA_INTEGRITY_PROT_ALGO_NONE;

  /* if Integrity Protection Mode Info is not included, then send
     SMC Failure message
   */
  if ( (msg_ptr->message.u.securityModeCommand.u.r3.securityModeCommand_r3.m.
        integrityProtectionModeInfoPresent) == 0 )
  {
     WRRC_MSG0_ERROR("Incomplete Configuration"); 
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  integrityProtModeInfo = &(msg_ptr->message.u.securityModeCommand.u.r3.
                            securityModeCommand_r3.integrityProtectionModeInfo);

  /*if we have already started integrity protection, then we should not
   *receive another start command*/
  if ( (integrityProtModeInfo->integrityProtectionModeCommand.t ==
        T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection) &&
       (integrity_protection_info.ip_status == RRCSMC_SECURITY_STARTED) )
  {
     WRRC_MSG0_ERROR("Invalid Start Cmd ");   
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION;  
    return RRCSMC_INVALID_CONFIGURATION;
  }

  /*if we receive a START cmd and do not receive the integrity 
    protection algorithm, return invalid config*/
  if ( (integrityProtModeInfo->integrityProtectionModeCommand.t == 
        T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection) &&  
       (integrityProtModeInfo->m.integrityProtectionAlgorithmPresent == 0 ) 
     )
  {
     WRRC_MSG0_ERROR("Incomplete Configuration");     
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION;  
    return RRCSMC_INVALID_CONFIGURATION;  
  }

  /*if we receive a MODIFY cmd and integrity is not yet started
    return invalid config*/
  if ( (integrityProtModeInfo->integrityProtectionModeCommand.t ==
        T_rrc_IntegrityProtectionModeCommand_modify) &&
       (integrity_protection_info.ip_status == RRCSMC_SECURITY_NOT_STARTED) )
  {
     WRRC_MSG0_ERROR("Invalid Configuration");     
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION;  
    return RRCSMC_INVALID_CONFIGURATION;    
  }


  if(integrityProtModeInfo->m.integrityProtectionAlgorithmPresent)
  {
    /* Right shift by 1 is added since the macro value starts from 0x2
       and to make it uniform to check against return type of rrcsmc_get_int_integrity_algo_from_dl_sdu_ind
       #define KASUMI_SUPPORTED         0x0002    */
      int_algo_supported = (KASUMI_SUPPORTED >> 1);
  #ifdef FEATURE_WCDMA_SNOW3G_SECURITY
      if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && snow3g_security_algo_supported)
      {
        int_algo_supported += (SNOW_3G_SUPPORTED >> 1);
      }
#endif

    int_algo_configured = rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
                &(integrityProtModeInfo->integrityProtectionAlgorithm),MSG_REL99);
  
    if(!(int_algo_supported & int_algo_configured))
    {
      WRRC_MSG2_ERROR("Mismatch between int algo supported %d and configured %d", int_algo_supported, int_algo_configured);  
      rrcsmc_failure_type = RRCSMC_UNSUPPORTED_CONFIG; 
      return RRCSMC_INVALID_CONFIGURATION;    
    }
  }
  
  return RRCSMC_VALID_CONFIG;          

}/*rrcsmc_validate_int_security_mode_command*/

/*===========================================================================

FUNCTION rrcsmc_send_security_mode_complete

DESCRIPTION
  This function sends the SECURITY MODE COMPLETE command on UL DCCH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcsmc_send_security_mode_complete
(
uint32 transaction_id
)
{
  rrc_UL_DCCH_Message *msg_ptr;         /* Pointer to uplink DCCH message */ 

  rlc_lc_id_type ul_lc_id;              /* Uplink logical channel ID */

  rrcsend_status_e_type send_status;    /* The status of sending the Initial
                                         * Security Mode Complete */

  uint32 i=0, num_srb=0, n_index=0;                  /*used for counter purposes*/

  struct rrc_RRC_MessageSequenceNumberList  *local_msg_seq_num_list=NULL;             
  /*local ptr to store msg seq num list
    used to populate SMC Complete msg*/

  rrcsmc_ciph_act_type  local_suspend_act_sn;
  struct rrc_RB_ActivationTimeInfoList *local_ciph_act_info;

  rrc_RB_Identity local_rb_id=0;            

  rrc_cmd_type *int_cmd_ptr;                    /*cmd ptr to send ciph config req*/

  memset(&local_suspend_act_sn, 0, sizeof(local_suspend_act_sn));

  l2_ack_rcvd_for_smc_complete_flag = FALSE;
  rrcsmc_send_l2_ack_ind_to_ddt_flag= FALSE;
  
  /* Determine the logical channel id for the Uplink DCCH
   */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                       DCCH_AM_RADIO_BEARER_ID, 
                                       UE_MODE_ACKNOWLEDGED_DATA);


  /*check if RB4 is setup or not and then config ciphering config for RB4*/
  if (RRCLCM_RLC_LC_ID_NOT_FOUND != 
      rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                DCCH_DT_LOW_PRI_RADIO_BEARER_ID, 
                                UE_MODE_ACKNOWLEDGED_DATA))
  {
    num_srb = RRC_MAX_SRB;
  }
  else
  {
    num_srb = (RRC_MAX_SRB-1);
  }

  /* Ensure that LCM found a valid logical channel id
   */
  if (ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    /* Found a valid RLC Logical Channel Id */
    msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc(sizeof(struct rrc_UL_DCCH_Message));

    memset((void *)msg_ptr, 0, sizeof(rrc_UL_DCCH_Message));

    /* Identify this message as a SECURITY MODE COMPLETE
     */
    msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_securityModeComplete;

    /* Initialize bit_mask to zero */
    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.securityModeComplete);

    if ( (nv_integrity_enabled) &&
         (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
    {
      /*set the bit_mask to indicate that we are sending the integrity
              protection activation info*/
      msg_ptr->message.u.securityModeComplete.m.ul_IntegProtActivationInfoPresent = 1;

      /*initialize the UL Integrity Protection Activation Info*/

      /*allocate memory for the local var local_msg_seq_num_list and then
              assign this address to the msg_ptr if alloc successful*/
      local_msg_seq_num_list = &(msg_ptr->message.u.securityModeComplete.
      ul_IntegProtActivationInfo.rrc_MessageSequenceNumberList);

      local_msg_seq_num_list->n = num_srb;
      ALLOC_ASN1ARRAY2(&enc_ctxt, local_msg_seq_num_list, rrc_RRC_MessageSequenceNumber);

      /*Initialize all the sequence number in the UL for all SRB's to 0*/
      for (i=0; i < num_srb; i++)
      {
        /* if we have received a modify command then copy the UL activation
                * time info instead of setting it to zero*/
        if (integrity_protection_info.modify_received == TRUE)
        {
          local_msg_seq_num_list->elem[i] = integrity_protection_info.ul_act_seq_num[i];
        }
        else
        {
          local_msg_seq_num_list->elem[i] = 0;
        }
      }
    }/*check if Integrity IE's present*/
    
    if ((nv_ciphering_enabled) &&
        (rrcsmc_int_ciph_info.ciphering_info_received == TRUE) )
    {
      /*set the bit_mask to indicate that we are sending the integrity
        protection activation info*/
      msg_ptr->message.u.securityModeComplete.m.rb_UL_CiphActivationTimeInfoPresent = 1;

      /*allocate memory for the local var local_msg_seq_num_list and then
        assign this address to the msg_ptr if alloc successful*/
      local_ciph_act_info = &(msg_ptr->message.u.securityModeComplete.rb_UL_CiphActivationTimeInfo);

      local_ciph_act_info->n = ciphering_info.dl_ciph_act_info.nchan;
      ALLOC_ASN1ARRAY2(&enc_ctxt, local_ciph_act_info, rrc_RB_ActivationTimeInfo);

      /*loop through all the channels which we suspended and have already
        calculated activation times for..  save them in the local struct
        indexed by Radio Bearer index.. This is done to sort the activation
        times based on Radio Bearers efficiently while sending Uplink 
        Activation Times..*/
      for (i=0; i<ciphering_info.ul_ciph_act_info.nchan; i++)
      {
        local_rb_id = rrclcm_get_ul_rb_id(ciphering_info.ul_ciph_act_info.
        lc_id_for_complete[i]);
        
        if(local_rb_id < MAX_RB)
        {
          local_suspend_act_sn.sq_num[local_rb_id] = 
          ciphering_info.ul_ciph_act_info.sq_num_for_complete[i];
        }
      }


      /*go thru the list of radio bearers.. append activation times only for
      the radio bearers for which we received activation times in DL.. we
      suspend all UM/AM Radio Bearers but may not need to send activation
      times for all UM/AM Radio Bearers.. we only send Activation Times for
      the UM/AM radio bearers for the ciphering changed domain*/

      n_index = 0;
      for (i=0; i<MAX_RB; i++)
      {
        if(ciphering_info.dl_ciph_act_info.act_time_present[i] == TRUE)
        {
           local_ciph_act_info->elem[n_index].rb_Identity = (rrc_RB_Identity)i;
          /*lint -e771*/
           local_ciph_act_info->elem[n_index].rlc_SequenceNumber =             
            local_suspend_act_sn.sq_num[i];
          /*lint +e771*/
          n_index++;
        }
      }/*check if Ciphering IE's present*/
    }
    /* Copy the transaction ID of SECURITY MODE COMMAND to COMPLETE Msg*/
    msg_ptr->message.u.securityModeComplete.rrc_TransactionIdentifier = 
    transaction_id; 
#ifdef FEATURE_QSH_EVENT_METRIC
   if(rrc_qsh_dl_committed == FALSE)
    {
      failure_ota.ul_ota_type = RRC_OTA_UL_CPLT;
      failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_SECURITYMODECOMPLETE;
      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
      {
        failure_ota.ul_fail_ota.ul_ota_parms.ul_integrity_info_present = 1;
      }
    }
    else if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
    {
      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
      {
     	  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.ul_integrity_info_present= 1;
      }
    }
#endif
    /* Send the SECURITY MODE COMPLETE on the Uplink DCCH
     */
    send_status = rrcsend_send_ul_sdu(RRC_PROCEDURE_SMC,
                                      msg_ptr,
                                      ul_lc_id,
                                      RRCSEND_L2ACK_REQUIRED);

    if (send_status == RRCSEND_SUCCESS)
    {
      WRRC_MSG0_HIGH("SECURITY MODE COMPLETE Message Sent");

        /*if feature is defined and nv item is turned off, then we need to just go
          to SMC idle without sending the config info to lower layers*/
      if((nv_ciphering_enabled) && 
         (rrcsmc_int_ciph_info.ciphering_info_received == TRUE) )
      {
        /*send cipher config req to llc, llc configures lower layers
          with ciphering information*/
          int_cmd_ptr =  rrc_get_int_cmd_buf();
          
          /* Fill in the command parameters for RRC_CIPH_CONFIG_REQ*/
          int_cmd_ptr->cmd_hdr.cmd_id = RRC_CIPH_CONFIG_REQ;
          /*Fill config_cause = CIPH_CONFIG_DL_KEYS, which will update 
            L1 with only DL keys.UL Keys will be updated to l1 after receiving
            L2 Ack of SMC message*/
          int_cmd_ptr->cmd.rrc_ciph_config_req.config_cause = CIPH_CONFIG_DL_KEYS;
  
          rrc_put_int_cmd(int_cmd_ptr);  
          rrcsmc_substate = RRCSMC_WAIT_FOR_CIPH_CONFIG_CNF;
  
          WRRC_MSG0_HIGH("Sending CIPH_CONFIG_REQ to LLC");
        }
        else
        {
        rrcsmc_substate=RRCSMC_WAIT_FOR_L2_ACK;  
      }
    }
    else
    {
      WRRC_MSG1_ERROR("Error %d sending SECURITY MODE COMPLETE",
          send_status);
      rrcsmc_substate = RRCSMC_INITIAL;
    }

  }/* Found a valid RLC Logical Channel Id */
  else
  {
    WRRC_MSG0_HIGH("Can't find RLC ID for RB 2");
  }
} /* rrcsmc_send_security_mode_complete */

/*===========================================================================

FUNCTION rrcsmc_send_security_mode_failure

DESCRIPTION
  This function sends the SECURITY MODE FAILURE command on UL DCCH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcsmc_send_security_mode_failure
(
uint32 transaction_id
)
{
  rrc_UL_DCCH_Message *msg_ptr;         /* Pointer to uplink DCCH message */ 

  rlc_lc_id_type ul_lc_id;              /* Uplink logical channel ID */

  rrcsend_status_e_type send_status;    /* The status of sending the Initial
                                         * Security Mode Failure */

  /* Determine the logical channel id for the Uplink DCCH
     Ensure that LCM found a valid logical channel id
   */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                       DCCH_AM_RADIO_BEARER_ID, 
                                       UE_MODE_ACKNOWLEDGED_DATA);

  if (ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    WRRC_MSG0_ERROR("Can't find RLC ID for RB 2");
    return;
  }

  /* Found a valid RLC Logical Channel Id */
  msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc(sizeof(struct rrc_UL_DCCH_Message));

  /*set message type to failure, non-critical errors to false and trans_id*/
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_securityModeFailure;
  msg_ptr->message.u.securityModeFailure.m.laterNonCriticalExtensionsPresent = 0;
  msg_ptr->message.u.securityModeFailure.rrc_TransactionIdentifier = 
  (OSUINT8)transaction_id;

  /*set the correct failure cause and fill in the failure struct with
    info if any*/
  switch (rrcsmc_failure_type)
  {

    /*set bitmask and failure causes to the appropriate types*/
    
    case RRCSMC_PROTOCOL_ERROR:
      msg_ptr->message.u.securityModeFailure.failureCause.t = 
      T_rrc_FailureCauseWithProtErr_protocolError;

      msg_ptr->message.u.securityModeFailure.failureCause.u.protocolError =
      	rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);
      
      msg_ptr->message.u.securityModeFailure.failureCause.u.protocolError->
        diagnosticsType.t = T_rrc_ProtocolErrorInformation_diagnosticsType_type1;
	  
      msg_ptr->message.u.securityModeFailure.failureCause.u.protocolError->
        diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);
      
      msg_ptr->message.u.securityModeFailure.failureCause.u.protocolError->
      diagnosticsType.u.type1->protocolErrorCause = rrcsmc_prot_err_cause;
      break;

    case RRCSMC_INCOMPATIBLE_SIMULTANEOUS_RECONFIG:
      msg_ptr->message.u.securityModeFailure.failureCause.t = 
      T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
      break;  

    case RRCSMC_INCOMPLETE_CONFIGURATION:
    case RRCSMC_INVALID_CONFIGURATION:
      msg_ptr->message.u.securityModeFailure.failureCause.t = 
      T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      break;  

    case RRCSMC_CELL_UPDATE_OCCURED:
      msg_ptr->message.u.securityModeFailure.failureCause.t =   
      T_rrc_FailureCauseWithProtErr_cellUpdateOccurred;
      break;

    case RRCSMC_UNSUPPORTED_CONFIG:
      msg_ptr->message.u.securityModeFailure.failureCause.t = 
      T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      break;
  
    default:
      msg_ptr->message.u.securityModeFailure.failureCause.t = 
      T_rrc_FailureCauseWithProtErr_spare1;  

      WRRC_MSG0_HIGH("Unknown SMC Failure Type");
      break;
  }  

  /*log the protocol error*/
  rrc_log_protocol_error( (uint8) (rrc_get_state()),
                          (uint8) RRC_PROCEDURE_SMC,
                          (uint8) msg_ptr->message.u.securityModeFailure.failureCause.t,
                          (uint8) rrcsmc_prot_err_cause 
                        );

  /*Send the SECURITY MODE COMPLETE on the Uplink DCCH
   */
  send_status = rrcsend_send_ul_sdu(RRC_PROCEDURE_SMC,
                                    msg_ptr,
                                    ul_lc_id,
                                    RRCSEND_L2ACK_NOT_REQUIRED);

  if (send_status == RRCSEND_SUCCESS)
  {
    WRRC_MSG0_HIGH("SECURITY MODE FAILURE Message Sent");      
  }
  else
  {
    WRRC_MSG1_ERROR("Error %d sending SECURITY MODE FAILURE",
        send_status);
  }
} /* rrcsmc_send_security_mode_failure */

/*===========================================================================

FUNCTION rrcsmc_fill_ul_resume_req

DESCRIPTION
   This function fills ul_resume_req with LC ids which were suspended while 
   processing security mode command.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE

===========================================================================*/
boolean rrcsmc_fill_ul_resume_req
(
  rlc_ul_resume_type *rrc_ul_resume_req_ptr
)
{
  uint32 i;
  rrc_ul_resume_req_ptr->nchan = 0;

  if (smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag == TRUE)
  {
    rrc_ul_resume_req_ptr->nchan = smc_rb_suspend_or_stop_info.suspend_or_stopped_info.nchan;

    /*in a loop set the contents of the suspend_req*/
    for (i=0; i < smc_rb_suspend_or_stop_info.suspend_or_stopped_info.nchan; i++)
    {
      rrc_ul_resume_req_ptr->channel[i] = 
         smc_rb_suspend_or_stop_info.suspend_or_stopped_info.suspend_stop[i].lc_id;
    }

    /* mark RBs as not in suspended state */
    smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag = FALSE;

    return(TRUE);
  }

  return(FALSE);
}
/*===========================================================================

FUNCTION rrcsmc_send_resume_req

DESCRIPTION
  This function constructs the CRLC_UL_RESUME_REQ primitive and
  sends it to L2 UL Task.
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_send_resume_req
(
void
)
{
  /*counter*/
  uint32 i;  
  wcdma_l2_ul_cmd_type  *rlc_ptr = NULL;

  WRRC_MSG0_HIGH("Dispatching out L2 cmd: Resume suspended RB's (CRLC_UL_RESUME_REQ)");

  if (smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag == TRUE )
  {
    /*Send RESUME Req to RLC */
    /* Get L2 UL command buffer */

    rlc_ptr = wcdma_l2_ul_get_cmd_buf();
  
    /* Fill in the Command Header */
    rlc_ptr->cmd_hdr.cmd_id = CRLC_UL_RESUME_REQ;
  
    /* Fill in the Command Data */
    rlc_ptr->cmd_data.ul_resume.nchan =
      smc_rb_suspend_or_stop_info.suspend_or_stopped_info.nchan;
  
    for(i=0; i < smc_rb_suspend_or_stop_info.suspend_or_stopped_info.nchan; i++)
    {
      rlc_ptr->cmd_data.ul_resume.channel[i] =
        smc_rb_suspend_or_stop_info.suspend_or_stopped_info.suspend_stop[i].lc_id;
    }
    /* Send command to L2 UL Task */
    rrc_send_l2_ul_cmd(rlc_ptr);

    smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag = FALSE;           
    ciphering_info.reconfig = FALSE;
  }
  else
  {
    WRRC_MSG0_HIGH("Error: No suspended RLC RB's to resume");
  }
}

/*===========================================================================

FUNCTION rrcsmc_process_wait_till_standby_ends_substate

DESCRIPTION
   This function processes SMC proecdure in wait till standby ends state.  We can
   be waiting for rrc mode to change from standby to active or inactive mode

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_wait_till_standby_ends_substate
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_MODE_CHANGE_IND:

      if(rrcmcm_get_rrc_mode() == RRC_MODE_ACTIVE)
      {	
        /* set "Uplink RRC Message sequence number" for signalling radio bearer
           RB0 in the variable INTEGRITY_PROTECTION_INFO to a value such that
           next RRC message to be sent on uplink RB0 will use the new integrity 
           protection configuration and the "RRC Message sequence number" in the IE
           "Integrity check info" in the next RRC message will equal to the activation
           time for RB0 as indicated in IE "Uplink integrity protection activation info"
           included in the transmitted SECURITY MODE COMPLETE;
           -- following IF check is to meet the above requirement */
        if ((TRUE == integrity_protection_info.modify_received) &&
            (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
             ul_count_i_act_time_pending[CCCH_RADIO_BEARER_ID] == TRUE))
        {
          uint32 rb_0_act_time_jump;         /*local var to store rb_0 jump act time*/

          //set the UL SN to one less than the act-time, as we will increment the count_i
          //after adjusting for HFN at the very end.  This will take care of N302+1 as well
          //as any roll-oer adjustment due to act-time becoming 0
          if(0 == integrity_protection_info.ul_act_seq_num[CCCH_RADIO_BEARER_ID])
          {
            rb_0_act_time_jump = 0xF;
          }
          else
          {
            rb_0_act_time_jump = 
              (integrity_protection_info.ul_act_seq_num[CCCH_RADIO_BEARER_ID] - 1);
          }
    
          //set the UL SN to one less than the act-time, as we will increment the count_i
          //after adjusting for HFN at the very end.  This will take care of N302+1 as well
          //as any roll-oer adjustment due to act-time becoming 0
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
            ul_count_i[CCCH_RADIO_BEARER_ID] =
            ((integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
             ul_count_i[CCCH_RADIO_BEARER_ID] & RRC_HFN_BIT_MASK) | 
            ( rb_0_act_time_jump & RRC_SEQ_NUM_BIT_MASK));
    
          MSG_HIGH("SMComp L2 Ack rcvd; SRB0 -> updated RRC-SN: 0x%x; latest-count-I: 0x%x; previousDomain-count-I: 0x%x)",
            rb_0_act_time_jump, integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
            ul_count_i[CCCH_RADIO_BEARER_ID], integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
            ul_count_i[CCCH_RADIO_BEARER_ID]);

          /* we need to copy the computed count-I<we need only SN> of _old domain
             to latest domain because if SMC is triggered before cell update 
             then activation SN for SRB0 need to be set using the SN of this latest
			 domain, which will become old domain with next SMC. */
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
            ul_count_i[CCCH_RADIO_BEARER_ID] =
            integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
              ul_count_i[CCCH_RADIO_BEARER_ID];
        }
        if(rrcsmc_int_ciph_info.ciphering_info_received == TRUE)
        {
          /*Lets Update L1 with UL cipher keys now.
            Note: We have already updated L1 with DL cipher keys*/
          rrcsmc_update_cphy_key_req(CIPH_CONFIG_UL_KEYS);
          rrcsmc_send_ul_ciphering_key_req();
        }
   
        /*Setting new_key = FALSE after receiving L2 ACK */
        WRRC_MSG1_HIGH("L2ACK for SMC received Set new_key_for_start_value to FALSE for CN doamin = %d",rrcsmc_int_ciph_info.latest_config_cn_id);
        rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id].ciph_keys_info.new_key_for_start_value= FALSE;
  
#ifdef FEATURE_SPEC_CR_2567_GTOW_SECURITY_KEY_HANDLING
        /*Update any pending security keys generated in other RAT right here */
        rrcsmc_check_and_update_security_key_set_gen_in_other_rat(RRC_CS_DOMAIN_CN_ID);
#endif
         
        //send resume_req to rlc after getting l2ack_ind
        rrcsmc_send_resume_req();
  
        /*check to see if integrity and ciphering are established.. if only ciphering
          is established, then send cause as ciphering only*/
        if((rrcsmc_int_ciph_info.ciphering_info_received == TRUE) &&
          (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
        {
          rrcsmc_send_sync_ind_to_mm(RRC_INTEGRITY_CIPHERING_ESTABLISHED);
  
        }
        else if((rrcsmc_int_ciph_info.ciphering_info_received != TRUE) &&
          (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
        {
          rrcsmc_send_sync_ind_to_mm(RRC_INTEGRITY_ESTABLISHED);
        }
        else if((rrcsmc_int_ciph_info.ciphering_info_received == TRUE) &&
         (rrcsmc_int_ciph_info.integrity_info_received != TRUE))
        {
          rrcsmc_send_sync_ind_to_mm(RRC_CIPHERING_ESTABLISHED);
        }
        else
        {
          if(nv_integrity_enabled)
          {
            WRRC_MSG0_ERROR("Integrity & ciphering both not started");
          }
          else
          {
            rrcsmc_send_sync_ind_to_mm(RRC_INTEGRITY_ESTABLISHED);
          }
        }
  
        /*received a l2ack, go to initial state*/
  
        rrcsmc_process_l2_ack_ind_to_dt_procedures();
   
       
        integrity_protection_info.reconfig = FALSE;
        rrcsmc_substate                    = RRCSMC_INITIAL;
      }
      else
      {
        WRRC_MSG0_HIGH("Ignoring MODE_CHANGE_IND for rrc mode other than ACTVE.");
      }

      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
               cmd_ptr->cmd_hdr.cmd_id,rrcsmc_substate);
      break;
  }          
}
			
				
/*===========================================================================

FUNCTION rrcsmc_process_wait_for_l2_ack_substate

DESCRIPTION
   This function processes SMC proecdure in wait for l2 ack state.  We can
   be waiting for an L2 ACK for a SMC complete command

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_wait_for_l2_ack_substate
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_L2ACK_IND:

      if (SUCCESS == cmd_ptr->cmd.downlink_l2ack_ind.status)
      {
        if(rrcmcm_get_rrc_mode() == RRC_MODE_STANDBY)
        {
          WRRC_MSG1_HIGH("SMC state changed to %d",RRCSMC_WAIT_TILL_STANDBY_ENDS);
          rrcmcm_register_for_mode_change(RRC_PROCEDURE_SMC,RRC_MODE_STANDBY,RRC_MODE_WILDCARD);
          rrcsmc_substate = RRCSMC_WAIT_TILL_STANDBY_ENDS;
        }
        else
        {  			
        /* set "Uplink RRC Message sequence number" for signalling radio bearer
           RB0 in the variable INTEGRITY_PROTECTION_INFO to a value such that
           next RRC message to be sent on uplink RB0 will use the new integrity 
           protection configuration and the "RRC Message sequence number" in the IE
           "Integrity check info" in the next RRC message will equal to the activation
           time for RB0 as indicated in IE "Uplink integrity protection activation info"
           included in the transmitted SECURITY MODE COMPLETE;
           -- following IF check is to meet the above requirement */
        if ((TRUE == integrity_protection_info.modify_received) &&
            (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
             ul_count_i_act_time_pending[CCCH_RADIO_BEARER_ID] == TRUE))
        {
          uint32 rb_0_act_time_jump;         /*local var to store rb_0 jump act time*/

          //set the UL SN to one less than the act-time, as we will increment the count_i
          //after adjusting for HFN at the very end.  This will take care of N302+1 as well
          //as any roll-oer adjustment due to act-time becoming 0
          if(0 == integrity_protection_info.ul_act_seq_num[CCCH_RADIO_BEARER_ID])
          {
            rb_0_act_time_jump = 0xF;
          }
          else
          {
            rb_0_act_time_jump = 
              (integrity_protection_info.ul_act_seq_num[CCCH_RADIO_BEARER_ID] - 1);
          }
    
          //set the UL SN to one less than the act-time, as we will increment the count_i
          //after adjusting for HFN at the very end.  This will take care of N302+1 as well
          //as any roll-oer adjustment due to act-time becoming 0
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
            ul_count_i[CCCH_RADIO_BEARER_ID] =
            ((integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
             ul_count_i[CCCH_RADIO_BEARER_ID] & RRC_HFN_BIT_MASK) | 
            ( rb_0_act_time_jump & RRC_SEQ_NUM_BIT_MASK));
    
          MSG_HIGH("SMComp L2 Ack rcvd; SRB0 -> updated RRC-SN: 0x%x; latest-count-I: 0x%x; previousDomain-count-I: 0x%x)",
            rb_0_act_time_jump, integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
            ul_count_i[CCCH_RADIO_BEARER_ID], integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
            ul_count_i[CCCH_RADIO_BEARER_ID]);

          /* we need to copy the computed count-I<we need only SN> of _old domain
             to latest domain because if SMC is triggered before cell update 
             then activation SN for SRB0 need to be set using the SN of this latest
			 domain, which will become old domain with next SMC. */
          integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
            ul_count_i[CCCH_RADIO_BEARER_ID] =
            integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
              ul_count_i[CCCH_RADIO_BEARER_ID];
        }
        if(rrcsmc_int_ciph_info.ciphering_info_received == TRUE)
        {
          /*Lets Update L1 with UL cipher keys now.
            Note: We have already updated L1 with DL cipher keys*/
          rrcsmc_update_cphy_key_req(CIPH_CONFIG_UL_KEYS);
          rrcsmc_send_ul_ciphering_key_req();
        }
   
        /*Setting new_key = FALSE after receiving L2 ACK */
        WRRC_MSG1_HIGH("L2ACK for SMC received Set new_key_for_start_value to FALSE for CN doamin = %d",rrcsmc_int_ciph_info.latest_config_cn_id);
        rrcsmc_int_ciph_info.int_ciph_info[rrcsmc_int_ciph_info.latest_config_cn_id].ciph_keys_info.new_key_for_start_value= FALSE;
  
        /*Update any pending security keys generated in other RAT right here */
        rrcsmc_check_and_update_security_key_set_gen_in_other_rat(RRC_CS_DOMAIN_CN_ID);
         
        //send resume_req to rlc after getting l2ack_ind
        rrcsmc_send_resume_req();
  
        /*check to see if integrity and ciphering are established.. if only ciphering
          is established, then send cause as ciphering only*/
        if((rrcsmc_int_ciph_info.ciphering_info_received == TRUE) &&
          (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
        {
          rrcsmc_send_sync_ind_to_mm(RRC_INTEGRITY_CIPHERING_ESTABLISHED);
  
        }
        else if((rrcsmc_int_ciph_info.ciphering_info_received != TRUE) &&
          (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
        {
          rrcsmc_send_sync_ind_to_mm(RRC_INTEGRITY_ESTABLISHED);
        }
        else if((rrcsmc_int_ciph_info.ciphering_info_received == TRUE) &&
         (rrcsmc_int_ciph_info.integrity_info_received != TRUE))
        {
          rrcsmc_send_sync_ind_to_mm(RRC_CIPHERING_ESTABLISHED);
        }
        else
        {
          if(nv_integrity_enabled)
          {
            WRRC_MSG0_ERROR("Integrity & ciphering both not started");
          }
          else
          {
            rrcsmc_send_sync_ind_to_mm(RRC_INTEGRITY_ESTABLISHED);
          }
        }
  
        /*received a l2ack, go to initial state*/
  
        rrcsmc_process_l2_ack_ind_to_dt_procedures();
   
       
        integrity_protection_info.reconfig = FALSE;
        rrcsmc_substate                    = RRCSMC_INITIAL;
      }
      }
      else
      {
        WRRC_MSG0_HIGH("Ignoring L2-Ack-Ind. Status in L2-Ack Indication set to FAILURE.");
      }

      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
               cmd_ptr->cmd_hdr.cmd_id,rrcsmc_substate);
      break;
  }          
}

/*===========================================================================

FUNCTION rrcsmc_process_wait_for_reconfig_cnf_substate

DESCRIPTION
   This function processes the all commands and events to SMC procedure
   in the WAIT_FOR_RECONFIG_CNF_SUBSTATE

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_wait_for_reconfig_cnf_substate
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_RECONFIG_COMPLETE_IND:
      WRRC_MSG0_HIGH("Rcvd RECONFIG CNF from LLC");

      if ((rrcllc_get_ordered_config_status() != OC_NOT_SET)
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME 
           || (rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
         )
      {
        /*register for a cnf when reconfiguration is completed*/
        rrcllc_register_for_reconfig_cnf();
        /* Wait until Reconfig procedure is completed */
        rrcsmc_substate = RRCSMC_WAIT_FOR_RECONFIG_CNF;  
      }
      else
      {
        rrcsmc_perform_stop_or_suspend_functionality(RRCSMC_SMC_CMD);
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for SMC in substate:%d", 
               cmd_ptr->cmd_hdr.cmd_id, rrcsmc_substate);
      break;
  }
}
/*===========================================================================

FUNCTION rrcsmc_process_ciph_security_mode_cmd_msg

DESCRIPTION
   This function processes the Integrity related IE's of security mode cmd

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_ciph_security_mode_cmd_msg
(
void
)
{
  uint32 i; /*counter in for loops*/

  /*set the latest_configured_cn_domain to the domain_id for which we 
    received security mode command*/
  rrcsmc_int_ciph_info.latest_config_cn_id =  
  saved_security_mode_command_msg.domain_id;

  /*ciphering on smc message passed.  so set reconfig to true
    and status to started.  send smc complete and wait for l2ack*/
  rrcsmc_int_ciph_info.ciph_status[rrcsmc_int_ciph_info.latest_config_cn_id] = 
  RRCSMC_SECURITY_STARTED;


  if (saved_security_mode_command_msg.dpch_ciph_act_time_present)
  {
    /*save dpch ciph act_time here*/
    tm_ciph_act_cfn = (uint8)saved_security_mode_command_msg.dpch_ciph_act_time;
  }

  if (saved_security_mode_command_msg.rb_ciph_dl_act_time_present)
  {

    ciphering_info.dl_ciph_act_info.nchan = saved_security_mode_command_msg.
      num_rb_ciph_dl_act_time;

    memset(ciphering_info.dl_ciph_act_info.act_time_present, FALSE, MAX_RB);

    for (i=0; i<saved_security_mode_command_msg.num_rb_ciph_dl_act_time; i++)
    {
      /*copy the saved security mode command's Activation times received for
        downlink.. also update the flag which indicates that activation time
        is present for this Radio Bearer*/
      ciphering_info.dl_ciph_act_info.sq_num_for_complete
        [saved_security_mode_command_msg.rb_ciph_dl_act_time[i].rb_id] =
      saved_security_mode_command_msg.rb_ciph_dl_act_time[i].seq_num;

      ciphering_info.dl_ciph_act_info.act_time_present
        [saved_security_mode_command_msg.rb_ciph_dl_act_time[i].rb_id] = TRUE;
    }
  }
}
/*===========================================================================

FUNCTION rrcsmc_process_int_security_mode_cmd_msg

DESCRIPTION
   This function processes the Integrity related IE's of security mode cmd

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_int_security_mode_cmd_msg
(
void
)
{

  /*set the activation time pending flag to true if we receive a modify*/
  if (saved_security_mode_command_msg.modify_chosen)
  {
    /*update activation time info structure*/
    rrcsmc_update_int_activation_time();

    /*set the modify flag to TRUE*/
    integrity_protection_info.modify_received = TRUE;

    /*Check to see if we got new keys or not and initialize count_i_info for
      RB2 only*/
    if (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
        ul_new_key_flag_per_rb[2] == TRUE)
    {
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      ul_count_i[2] = 
      (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
       ul_count_i[2] & RRC_SEQ_NUM_BIT_MASK);

      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      ul_new_key_flag_per_rb[2] = FALSE;
    }
    else
    {
      /*set count_i for the new cn domain by &ing HFN of new CN domain and 
      SEQ_NUM last used for the previous CN domain*/
      integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
      ul_count_i[2] =
      (uint32)((integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
        ul_count_i[2] & RRC_HFN_BIT_MASK) |
       (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id_old].
        ul_count_i[2] & RRC_SEQ_NUM_BIT_MASK));
    }
  }

  /* integrity check on smc message passed.  So, status to started.
   * Send smc complete and wait for l2ack*/
  integrity_protection_info.ip_status = RRCSMC_SECURITY_STARTED;        
}

/*===========================================================================

FUNCTION rrcsmc_process_suspend_cnf

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
/*lint -e715*/
void rrcsmc_process_suspend_cnf(rlc_ul_stop_suspend_cnf_type *rrcsmc_suspend_cnf)
{
  uint32 i; 
  if ( (nv_integrity_enabled) &&
       (rrcsmc_int_ciph_info.integrity_info_received == TRUE) )
  {
    rrcsmc_process_int_security_mode_cmd_msg();
  }

  /*check if ciphering IE's are sent process this only if present*/
  if(rrcsmc_int_ciph_info.ciphering_info_received == TRUE)
  {

    WRRC_MSG0_HIGH("Process SUSPEND RLC RB CNF ");

    smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag = TRUE;           

    rrcsmc_process_ciph_security_mode_cmd_msg();

    //get the suspend cnf information and set the activation time for all existing
    //rlc am and um channels
    ciphering_info.ul_ciph_act_info.nchan = rrcsmc_suspend_cnf->nchan;

    for (i=0; i< ciphering_info.ul_ciph_act_info.nchan; i++)
    {
      if(i < (UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY)) /* Max array size of cnf is UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY */
      {
        ciphering_info.ul_ciph_act_info.sq_num[rrcsmc_suspend_cnf->cnf[i].lc_id] = 
        rrcsmc_suspend_cnf->cnf[i].rlc_sn + RLC_CIPH_ACT_SN;
  
        ciphering_info.ul_ciph_act_info.sq_num_for_complete[i] = 
        rrcsmc_suspend_cnf->cnf[i].rlc_sn + RLC_CIPH_ACT_SN;
  
        ciphering_info.ul_ciph_act_info.lc_id_for_complete[i] = 
        rrcsmc_suspend_cnf->cnf[i].lc_id;
      }
      else
      {
        WRRC_MSG0_ERROR("Array size exceeded for cnf");
      }
    }

    /* Send SECURITY MODE COMPLETE command */
    rrcsmc_send_security_mode_complete(saved_security_mode_command_msg.transaction_id);

  }/*check for ciphering IE's are present*/
  else if ((nv_integrity_enabled) &&
       (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
  {
    smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag = TRUE;           
  
        /* Send SECURITY MODE COMPLETE command */
    rrcsmc_send_security_mode_complete(saved_security_mode_command_msg.transaction_id);
  }
}
/*lint +e715*/

/*===========================================================================

FUNCTION rrcsmc_process_wait_for_ciph_config_cnf_substate

DESCRIPTION
   This function processes the all commands and events to SMC procedure
   in the WAIT_FOR_CIPH_CONFIG_SUBSTATE

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_wait_for_ciph_config_cnf_substate
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  rrc_cmd_type *int_cmd_ptr = NULL;
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_L2ACK_IND:
      if (SUCCESS == cmd_ptr->cmd.downlink_l2ack_ind.status)
      {
        l2_ack_rcvd_for_smc_complete_flag = TRUE;
        /*received a l2ack, set l1_ack ind rcvd. flag to true.. this will be processed
        lates when ciph_config_cnf is received by rrc*/
        WRRC_MSG0_HIGH("L2ACK for SMC received");
      }
      else
      {
        WRRC_MSG0_HIGH("Ignoring L2-Ack Indication from RLC as l2-ack-status set to failure");
      }
      break;

    case RRC_CIPH_CONFIG_CNF:

      WRRC_MSG0_HIGH("Rcvd CIPH CNF from LLC");

      /* check whether we need to revert back to old ciphering variables or not */
      if (TRUE == is_revert_to_old_cipher_variable_reqd)
      {
        /* Revert the ciphering variables */
        WRRC_MSG0_HIGH("Reverting to old ciphering variables in ciph_config_cnf substate");
        rrcsmc_revert_to_old_security_variables();

        WRRC_MSG0_HIGH("Setting is_revert_to_old_cipher_variable_reqd to FALSE");
        is_revert_to_old_cipher_variable_reqd = FALSE;

        if(cmd_ptr->cmd.rrc_cipher_config_cnf.cause == RRC_CIPHER_CONFIG_FAIL_DIDNT_CONFIG_LL)
        {
          /*This case is cell reselction clash with smc, LLC would have not configured LL with 
          *cipher config req, So we need to simply resume suspended RB's
          */
          WRRC_MSG0_HIGH("Rcvd CIPH CNF with cause FAIL_DIDNT_CONFIG_LL from LLC");
          rrcsmc_send_resume_req();
        }
        else
        {
          /* Send Abort cipher config req to llc */
            int_cmd_ptr =  rrc_get_int_cmd_buf();
            /* Fill in the command parameters for RRC_CIPH_CONFIG_REQ*/
            int_cmd_ptr->cmd_hdr.cmd_id = RRC_ABORT_CIPH_CONFIG_REQ;
            int_cmd_ptr->cmd.rrc_abort_ciph_config_req.abort_action = RRC_ABORT_CIPH_ACTION_L1KEY_RLC_RESUME;
  
            WRRC_MSG1_HIGH("Sending RRC_ABORT_CIPH_CONFIG_REQ to LLC in Ciph_Config_Cnf substate. Abort-action: %d", 
                            int_cmd_ptr->cmd.rrc_abort_ciph_config_req.abort_action);
            rrc_put_int_cmd(int_cmd_ptr);
          }

        rrcsmc_substate = RRCSMC_INITIAL;
        break;
      }
      
      /*if l2_ack has already been received while in wait_for_ciph_config_cnf substate
        then take smc to initial and update dt procedures with l2_ack ind for smc*/
      if(l2_ack_rcvd_for_smc_complete_flag == TRUE)
      {
        if(rrcsmc_int_ciph_info.ciphering_info_received == TRUE)
        {
          /*Lets Update L1 with UL cipher keys now.
            Note: We have already updated L1 with DL cipher keys*/
          rrcsmc_update_cphy_key_req(CIPH_CONFIG_UL_KEYS);
          rrcsmc_send_ul_ciphering_key_req();
        }
        /*Update any pending security keys generated in other RAT right here */
        rrcsmc_check_and_update_security_key_set_gen_in_other_rat(RRC_CS_DOMAIN_CN_ID);
        //send resume req if already received l2_ack for smc-complete
        rrcsmc_send_resume_req();

        /*check to see if integrity and ciphering are established.. if only ciphering
          is established, then send cause as ciphering only*/
        if((rrcsmc_int_ciph_info.ciphering_info_received == TRUE) &&
        (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
        {
          rrcsmc_send_sync_ind_to_mm(RRC_INTEGRITY_CIPHERING_ESTABLISHED);
        }
        else if((rrcsmc_int_ciph_info.ciphering_info_received != TRUE) &&
          (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
        {
          rrcsmc_send_sync_ind_to_mm(RRC_INTEGRITY_ESTABLISHED);
        }
        else if((rrcsmc_int_ciph_info.ciphering_info_received == TRUE) &&
          (rrcsmc_int_ciph_info.integrity_info_received != TRUE))
        {
          rrcsmc_send_sync_ind_to_mm(RRC_CIPHERING_ESTABLISHED);
        }
        else
        {
          if(nv_integrity_enabled)
          {
            WRRC_MSG0_ERROR("Integrity & ciphering both not started");
          }
          else
          {
            rrcsmc_send_sync_ind_to_mm(RRC_INTEGRITY_ESTABLISHED);
          }
        }

        rrcsmc_substate = RRCSMC_INITIAL;
        
        /*set the reconfig flag to false as we got the l2_ack for 
          already in ciph_config_substate */
        integrity_protection_info.reconfig        = FALSE;
    
        /*send ind to dt procedures that l2 ack for smc has been rcvd. and dt
        procedures can send uplink messages*/
        rrcsmc_process_l2_ack_ind_to_dt_procedures();
      }
      else
      {
        rrcsmc_substate = RRCSMC_WAIT_FOR_L2_ACK;
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for SMC in substate:%d", 
               cmd_ptr->cmd_hdr.cmd_id, rrcsmc_substate);
      break;
  }
}
/*===========================================================================

FUNCTION rrcsmc_re_establish_rlc_rb_for_srns_relocation

DESCRIPTION
   This function re-establishes rlc radio bearers.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_re_establish_rlc_rb_for_srns_relocation
(
  rrc_rlc_re_est_for_srns_rb_e_type rb_type, 
  uint32 start_value_srb,
  uint32 start_value_urb
)
{
  rrc_cmd_type *cmd_ptr;
    
  switch(rb_type)
  {
    case RRC_RE_EST_RB_2:

      cmd_ptr =  rrc_get_int_cmd_buf();
      /* Fill in the command parameters  */
      cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_RE_EST_RLC_REQ;
      
      cmd_ptr->cmd.rrc_re_est_req.am_config.rb_type = RRC_RE_EST_RB_2;
      cmd_ptr->cmd.rrc_re_est_req.am_config.hfn_srb = start_value_srb;    
      
      //if latest domain is PS, then copy ciph algo for CS and PS to CS
      //domain
      if(rrcsmc_int_ciph_info.latest_config_cn_id == RRC_PS_DOMAIN_CN_ID)
      {
        cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_srb =
          rrcsmc_int_ciph_info.saved_ciph_algo[RRC_PS_DOMAIN_CN_ID];
      }
      //else copy URB to PS domain and SRB to CS domain
      else
      {
        cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_srb =
          rrcsmc_int_ciph_info.saved_ciph_algo[RRC_CS_DOMAIN_CN_ID];
      }

      rrc_put_int_cmd(cmd_ptr);  

      WRRC_MSG2_HIGH("Re-est for RB2 Algo SRB %d URB %d",
               cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_srb,
               cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_urb);
      break;
    
    case RRC_RE_EST_NON_RB_2_UM_AM:

      cmd_ptr =  rrc_get_int_cmd_buf();
      /* Fill in the command parameters  */
      cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_RE_EST_RLC_REQ;

      cmd_ptr->cmd.rrc_re_est_req.am_config.rb_type = RRC_RE_EST_NON_RB_2_UM_AM;
      
      cmd_ptr->cmd.rrc_re_est_req.am_config.hfn_srb = start_value_srb;    
      cmd_ptr->cmd.rrc_re_est_req.am_config.hfn_urb = start_value_urb;          
    
      cmd_ptr->cmd.rrc_re_est_req.ul_um_config.rb_type = RRC_RE_EST_NON_RB_2_UM_AM;
      
      cmd_ptr->cmd.rrc_re_est_req.ul_um_config.hfn_srb = start_value_srb<<5;    
      cmd_ptr->cmd.rrc_re_est_req.ul_um_config.hfn_urb = start_value_urb<<5;                
    
      cmd_ptr->cmd.rrc_re_est_req.dl_um_config.rb_type = RRC_RE_EST_NON_RB_2_UM_AM;
      
      cmd_ptr->cmd.rrc_re_est_req.dl_um_config.hfn_srb = start_value_srb<<5;    
      cmd_ptr->cmd.rrc_re_est_req.dl_um_config.hfn_urb = start_value_urb<<5;                    
      
      //if latest domain is PS, then copy ciph algo for CS and PS to CS
      //domain
      if(rrcsmc_int_ciph_info.latest_config_cn_id == RRC_PS_DOMAIN_CN_ID)
      {
        cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_urb =
        cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_srb =
        cmd_ptr->cmd.rrc_re_est_req.ul_um_config.ciph_algo_srb =
        cmd_ptr->cmd.rrc_re_est_req.dl_um_config.ciph_algo_srb =
        cmd_ptr->cmd.rrc_re_est_req.ul_um_config.ciph_algo_urb =
        cmd_ptr->cmd.rrc_re_est_req.dl_um_config.ciph_algo_urb =
          rrcsmc_int_ciph_info.saved_ciph_algo[RRC_PS_DOMAIN_CN_ID];
      }
      //else copy URB to PS domain and SRB to CS domain
      else
      {
        cmd_ptr->cmd.rrc_re_est_req.ul_um_config.ciph_algo_urb =
        cmd_ptr->cmd.rrc_re_est_req.dl_um_config.ciph_algo_urb =
        cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_urb =
          rrcsmc_int_ciph_info.saved_ciph_algo[RRC_PS_DOMAIN_CN_ID];

        cmd_ptr->cmd.rrc_re_est_req.ul_um_config.ciph_algo_srb =
        cmd_ptr->cmd.rrc_re_est_req.dl_um_config.ciph_algo_srb =
        cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_srb =
          rrcsmc_int_ciph_info.saved_ciph_algo[RRC_CS_DOMAIN_CN_ID];
      }

      rrc_put_int_cmd(cmd_ptr);  
      
      WRRC_MSG2_HIGH("Re-est for Non-RB2 Algo SRB %d URB %d",
               cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_srb,
               cmd_ptr->cmd.rrc_re_est_req.am_config.ciph_algo_urb);
        
      break;
    
    default:
    WRRC_MSG0_HIGH("Unsupported re-est cause");
  }
}
/*===========================================================================

FUNCTION rrcsmc_process_wait_srns_reloc_l2_ack_ind_substate    

DESCRIPTION
   This function processes the all commands and events to SMC procedure
   in the WAIT_SRNS_RELOC_L2_ACK_IND_SUBSTATE

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_wait_srns_reloc_l2_ack_ind_substate    
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  rrc_cmd_type *int_cmd_ptr;
  rrcsmc_re_est_start_list  start_list;

  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_RE_EST_RLC_FOR_SRNS_REQ:
    
        //check if case is for re-est rb2 only, in this state non-rb-2 
        //re-establishment is not supported.
        if(cmd_ptr->cmd.rrc_re_est_srns_req.rb_type != RRC_RE_EST_NON_RB_2_UM_AM)
        {
          //send cnf with failure
          int_cmd_ptr =  rrc_get_int_cmd_buf();

            /* Fill in the command parameters for RRC_RE_EST_RLC_FOR_SRNS_CNF */
            int_cmd_ptr->cmd_hdr.cmd_id = RRC_RE_EST_RLC_FOR_SRNS_CNF;
            int_cmd_ptr->cmd.rrc_re_est_srns_cnf.status = FAILURE;
            rrc_put_int_cmd(int_cmd_ptr);  
      
            WRRC_MSG0_HIGH("Send Re-Est Fail-RB2");
          }      
        else
        {
          saved_srns_info.rb_type = RRC_RE_EST_NON_RB_2_UM_AM;
        }
        
        //if re_est cnf was needed, set flag to true, else false
        if(cmd_ptr->cmd.rrc_re_est_srns_req.cnf_reqd == TRUE)
        {
          saved_srns_info.reconfig_cnf_req = TRUE;
          //save the procedure name to send cnf
          saved_srns_info.proc_type = cmd_ptr->cmd.rrc_re_est_srns_req.rrc_proc;
        }
        else
        {
          saved_srns_info.reconfig_cnf_req = FALSE;    
        }        
        
        rrcsmc_get_latest_transmitted_start_values(&start_list);
        
    WRRC_MSG2_HIGH("Re-EST Non-RB2 HFN_SRB %d, HFN_URB %d", 
         start_list.hfn_srb, start_list.hfn_rab);

        //START value and hfn are same for RB2      
        rrcsmc_re_establish_rlc_rb_for_srns_relocation(
          RRC_RE_EST_NON_RB_2_UM_AM, 
          start_list.hfn_srb,
          start_list.hfn_rab); 

        rrcsmc_substate = RRCSMC_WAIT_RE_EST_SRNS_CNF;
      break;


    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for SMC in substate:%d", 
               cmd_ptr->cmd_hdr.cmd_id, rrcsmc_substate);
      break;
  }
}

/*===========================================================================

FUNCTION rrcsmc_process_wait_re_est_srns_substate

DESCRIPTION
   This function processes the all commands and events to SMC procedure
   in the WAIT_RE_EST_SRNS_SUBSTATE

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_wait_re_est_srns_substate
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  rrc_cmd_type *int_cmd_ptr;
  
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_LLC_RE_EST_RLC_CNF:
      //if rsp was requested, then send the cnf to calling reconfig procedure.
      if(saved_srns_info.reconfig_cnf_req == TRUE)
      {
          int_cmd_ptr =  rrc_get_int_cmd_buf();
          /* Fill in the command parameters for RRC_RE_EST_RLC_FOR_SRNS_CNF */
          int_cmd_ptr->cmd_hdr.cmd_id = RRC_RE_EST_RLC_FOR_SRNS_CNF;
          int_cmd_ptr->cmd.rrc_re_est_srns_cnf.status = SUCCESS;
          int_cmd_ptr->cmd.rrc_re_est_srns_cnf.rrc_proc = saved_srns_info.proc_type;
          rrc_put_int_cmd(int_cmd_ptr);  
    
          WRRC_MSG0_HIGH("Send Re-Est Success");
          
          saved_srns_info.reconfig_cnf_req = FALSE;
        }      
      
      //if cause of doing re_est was for all rb's send resume cnf
      if(saved_srns_info.rb_type == RRC_RE_EST_RB_2)
      {
        WRRC_MSG0_HIGH("Re-EST of RB-2 done");
        //for rb2 procedure go to wait_for_srns_reloc_ind
        rrcsmc_substate = RRCSMC_WAIT_SRNS_RELOC_L2_ACK_IND;      
      }
      else
      {
        MSG_HIGH("Re-EST of other RB's done",0,0,0);
        /* Purge all SRBs except RB2 to make sure no SRB message is 
           transmitted with stale start values after continue of RLC */
	rrcsmc_ul_purge_wm_except_srb2();
        //for other rb's send continue_req and go to initial substate
        rrcsmc_send_continue_req();
        
#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
        #error code not present
#endif

        if(saved_srns_info.ciph_algo_present == TRUE)
        {
          WRRC_MSG2_HIGH("S_SMC_Algo:%d, S_SRNS_Algo:%d",
             saved_security_mode_command_msg.ciph_algo,
             saved_srns_info.ciph_algo);

          saved_security_mode_command_msg.ciph_algo = saved_srns_info.ciph_algo;
        }
        ciphering_info.reconfig = FALSE;
        /*Initialize the fresh present when SRNS is sucessfulyl completed. Without 
        this initialization RRC may false revert back to old fresh when HHO fails*/
        saved_srns_info.fresh_present= FALSE;
        rrcsmc_substate = RRCSMC_INITIAL;
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for SMC in substate:%d", 
               cmd_ptr->cmd_hdr.cmd_id, rrcsmc_substate);
      break;
  }
}
/*===========================================================================

FUNCTION rrcsmc_process_suspend_srns_cnf

DESCRIPTION
  This function performs all the functionality needed for SRNS relocation case 
  after suspending all AM & UM Uplink RLC RB's except-SRB-2/SRB-0.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_process_suspend_srns_cnf
(
void
)
{
  /*rlc_hfn_info_type to calculate start_value_to_transmit*/
  rlc_hfn_req_type *local_hfn_type_ptr= (rlc_hfn_req_type *)rrc_malloc(sizeof(rlc_hfn_req_type));
  rlc_pending_hfn_req_type local_pending_hfn_type;
  uint32 ul_rb2_hfn=0;
  uint32 dl_rb2_hfn=0;
  uint32 i, rb2_hfn;
  
  WRRC_MSG0_HIGH("Process STOP CNF from LLC");
  smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag = TRUE;           

  if (SUCCESS == rlc_rrc_get_latest_pending_hfn(
      rrclcm_get_ul_rlc_lc_id(DCCH_AM_RADIO_BEARER_ID),
      &local_pending_hfn_type))
  {
    /* inside this if check indicates that either UL or DL or both
       have a pending ciphering configuration */
    if (TRUE == local_pending_hfn_type.ul_pending_hfn_present)
    {
      ul_rb2_hfn = local_pending_hfn_type.ul_pending_hfn;
    }
    if (TRUE == local_pending_hfn_type.dl_pending_hfn_present)
    {
      dl_rb2_hfn = local_pending_hfn_type.dl_pending_hfn;
    }
  }

  /*get the latest RLC HFN's*/
  (void) rlc_rrc_get_hfn_req(local_hfn_type_ptr);

  /* update HFN only if there is no pending UL HFN */
  if (FALSE == local_pending_hfn_type.ul_pending_hfn_present)
  {
    //get ul HFN
    for (i=0; i < local_hfn_type_ptr->ul_nchan; i++)
    {
      if(local_hfn_type_ptr->hfn_per_rb_ul[i].rlc_id == 
          rrclcm_get_ul_rlc_lc_id(DCCH_AM_RADIO_BEARER_ID))
      {
        ul_rb2_hfn = local_hfn_type_ptr->hfn_per_rb_ul[i].hfn;
        break;          
      }
    }
  }

  /* update HFN only if there is no pending DL HFN */
  if (FALSE == local_pending_hfn_type.dl_pending_hfn_present)
  {
    //get dl HFN
    for (i=0; i < local_hfn_type_ptr->dl_nchan; i++)
    {
      if(local_hfn_type_ptr->hfn_per_rb_dl[i].rlc_id == 
         rrclcm_get_dl_rlc_lc_id(DCCH_AM_RADIO_BEARER_ID))
      {
        dl_rb2_hfn = local_hfn_type_ptr->hfn_per_rb_dl[i].hfn;
        break;
      }
    }
  }

  //get the max of ul/dl and add 1 to it.. HFN calculation
  //according to 25.331
  if(ul_rb2_hfn > dl_rb2_hfn)
  {
    rb2_hfn = ul_rb2_hfn+1;
  }
  else
  {
    rb2_hfn = dl_rb2_hfn+1;      
  }

  MSG_HIGH("RB2 re-est: SRB2-HFNs in UL 0x%x DL 0x%x Config HFN 0x%x", ul_rb2_hfn, dl_rb2_hfn, rb2_hfn);
      
  //START value and hfn are same for RB2      
  rrcsmc_re_establish_rlc_rb_for_srns_relocation(RRC_RE_EST_RB_2, rb2_hfn, rb2_hfn);
  rrcsmc_substate = RRCSMC_WAIT_RE_EST_SRNS_CNF;
  rrc_free(local_hfn_type_ptr);
  
}

/*===========================================================================

FUNCTION rrcsmc_is_rlc_re_est_for_srns_active

DESCRIPTION
  This function returns the status of SMC doing RLC re-establishment.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
boolean rrcsmc_is_rlc_re_est_for_srns_active()
{
  if((rrcsmc_substate == RRCSMC_WAIT_SRNS_RELOC_L2_ACK_IND) ||
     (rrcsmc_substate == RRCSMC_WAIT_RE_EST_SRNS_CNF))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION rrcsmc_save_contents_of_security_mode_command

DESCRIPTION
   This function processes the SECURITY MODE COMMAND received on DL DCCH
   and extracts the Transaction ID.  Sends the SYNC_IND cmd to MM and
   also calls the function to send SECURITY MODE COMPLETE

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_save_contents_of_security_mode_command
(
rrc_DL_DCCH_Message *dcch_msg_ptr
)
{

  uint32 i, n_index=0, n_value=0, rb_index = 0, count=0; /*used for counter purposes*/
  struct rrc_SecurityModeCommand_r3_IEs * local_smc_msg_ptr;

  /*local var to store act time linked list ptr*/
  struct rrc_RRC_MessageSequenceNumberList  *local_smc_act_list;

  struct rrc_RB_ActivationTimeInfoList  *local_rb_ciph_act_time;

  /*copy indirected link to a local ptr*/
  local_smc_msg_ptr = &(dcch_msg_ptr->message.u.securityModeCommand.u.r3.
                        securityModeCommand_r3);

  /*save transaction id*/
  saved_security_mode_command_msg.transaction_id = local_smc_msg_ptr->
                                                   rrc_TransactionIdentifier;

  /*copy domain id*/
  if(local_smc_msg_ptr->cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain) 
  {
    saved_security_mode_command_msg.domain_id = RRC_CS_DOMAIN_CN_ID;
  }
  else
  {
    saved_security_mode_command_msg.domain_id = RRC_PS_DOMAIN_CN_ID;
  }

  if(rrcsmc_int_ciph_info.integrity_info_received == TRUE)
  {

    /*check if start or modify is chosen and update accordingly*/
    if (local_smc_msg_ptr->integrityProtectionModeInfo.
        integrityProtectionModeCommand.t ==  T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection )
    {
      saved_security_mode_command_msg.start_chosen = TRUE;
      saved_security_mode_command_msg.modify_chosen = FALSE;
  
      /*get the fresh value*/
      saved_security_mode_command_msg.start = 
      b_unpackd(local_smc_msg_ptr->integrityProtectionModeInfo.
                integrityProtectionModeCommand.u.startIntegrityProtection->
                integrityProtInitNumber.data,
                RRC_DEFAULT_BIT_POS,
                RRC_FRESH_LENGTH_IN_BITS);  

      //save the fresh value for srns relocation purposes..
      saved_srns_info.fresh_old = saved_security_mode_command_msg.start;

      /* Since its the first time FRESH value is provided to the UE, hence
         there is no old1 fresh value. Hence, saving the received FRESH 
         value in fresh_old1 as well. This stored value is needed to revert back to old 
         configuration after SRNS relocation physChannelEstFailure */
      saved_srns_info.fresh_old1 = saved_security_mode_command_msg.start;
    }
    else
    {
      WRRC_MSG1_HIGH("Saved FRESH is 0x%x",saved_security_mode_command_msg.start);

      saved_security_mode_command_msg.start_chosen = FALSE;
      saved_security_mode_command_msg.modify_chosen = TRUE;
    
      /*initialize seqNumList*/
      local_smc_act_list = &(local_smc_msg_ptr->integrityProtectionModeInfo.
        integrityProtectionModeCommand.u.modify->dl_IntegrityProtActivationInfo.
        rrc_MessageSequenceNumberList);

      /*initialize count_i for NEW latest CN domain*/
      n_index = 0;
      n_value = local_smc_act_list->n;
      
      for ( i=0; i < RRC_MAX_SRB; i++)
      {
        if(n_value != 0)
        {
          /*initialize downlink count_i dl_activation_info time sent by utran
                 for this domain */
          saved_security_mode_command_msg.srb_int_dl_act_time[i].seq_num = 
            (uint8)(local_smc_act_list->elem[n_index]);
  
          n_value--;
          n_index++;
        }
      }
    }
    
    /*save the integrity algorithm sent*/
    saved_security_mode_command_msg.integrity_algo = 
          rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
            &local_smc_msg_ptr->integrityProtectionModeInfo.integrityProtectionAlgorithm,
             MSG_REL99);

    rrcsmc_int_ciph_info.saved_integrity_algo[saved_security_mode_command_msg.domain_id] 
       = rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
            &local_smc_msg_ptr->integrityProtectionModeInfo.integrityProtectionAlgorithm,
             MSG_REL99);

  }

  if((nv_ciphering_enabled) &&
     (rrcsmc_int_ciph_info.ciphering_info_received == TRUE))
   {
  
    /*save the ciphering algorithm sent*/
    saved_security_mode_command_msg.ciph_algo = 
          rrcsmc_get_int_cipher_algo_from_dl_sdu_ind(
              &(local_smc_msg_ptr->cipheringModeInfo.cipheringModeCommand.u.startRestart),
              MSG_REL99);
  
    /*save to temp var*/
    rrcsmc_int_ciph_info.saved_ciph_algo[saved_security_mode_command_msg.domain_id] = 
          rrcsmc_get_int_cipher_algo_from_dl_sdu_ind(
              &(local_smc_msg_ptr->cipheringModeInfo.cipheringModeCommand.u.startRestart),
              MSG_REL99);
  
    /*copy activation time for DPCH if sent*/
    if (local_smc_msg_ptr->cipheringModeInfo.m.activationTimeForDPCHPresent)
    {
      saved_security_mode_command_msg.dpch_ciph_act_time_present = TRUE;
      saved_security_mode_command_msg.dpch_ciph_act_time = 
        local_smc_msg_ptr->cipheringModeInfo.activationTimeForDPCH;
    }
    else
    {
      saved_security_mode_command_msg.dpch_ciph_act_time_present = FALSE;
    }
  
    if (local_smc_msg_ptr->cipheringModeInfo.m.rb_DL_CiphActivationTimeInfoPresent)
    {
      /*copy Radio Bearer DL activation time for all UM/AM RB's*/
      local_rb_ciph_act_time = &(local_smc_msg_ptr->cipheringModeInfo.
                               rb_DL_CiphActivationTimeInfo);
  
      /*set the flag to true*/
      saved_security_mode_command_msg.rb_ciph_dl_act_time_present = TRUE;
  
      /*init local vars used for counter purposes to zero*/
      saved_security_mode_command_msg.num_rb_ciph_dl_act_time = 0;

      n_index= 0;
  
      /*in a while loop, copy the number of DL activation time for each
        radio bearer*/
      while((local_rb_ciph_act_time->n > n_index) && 
           (n_index < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY)) && 
           (saved_security_mode_command_msg.num_rb_ciph_dl_act_time < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY)))
      {
        if((local_rb_ciph_act_time->elem[n_index].rb_Identity == DCCH_UM_RADIO_BEARER_ID) ||
           (local_rb_ciph_act_time->elem[n_index].rb_Identity == DCCH_AM_RADIO_BEARER_ID) ||
           (local_rb_ciph_act_time->elem[n_index].rb_Identity == DCCH_DT_HIGH_PRI_RADIO_BEARER_ID) ||
           (local_rb_ciph_act_time->elem[n_index].rb_Identity == DCCH_DT_LOW_PRI_RADIO_BEARER_ID))
        {
          saved_security_mode_command_msg.rb_ciph_dl_act_time[saved_security_mode_command_msg.num_rb_ciph_dl_act_time].rb_id = (uint8) 
            local_rb_ciph_act_time->elem[n_index].rb_Identity;
          saved_security_mode_command_msg.rb_ciph_dl_act_time[saved_security_mode_command_msg.num_rb_ciph_dl_act_time].seq_num = (uint16)
            local_rb_ciph_act_time->elem[n_index].rlc_SequenceNumber;           
          saved_security_mode_command_msg.num_rb_ciph_dl_act_time++;
        }
        else
        {
          for ( count=0; count<MAX_RAB_TO_SETUP; count++ )
          {
            if (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID)
            {
              for (rb_index = 0; (rb_index < rrc_est_rabs.rabs[count].num_rbs_for_rab) && (saved_security_mode_command_msg.num_rb_ciph_dl_act_time < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY)); rb_index++)
              {
                if (rrc_est_rabs.rabs[count].rb_for_rab[rb_index].rb_status ==
                    RRC_RB_STARTED)
                {
                  /* Check to see if this RB info is included in the ciph activation times list */
                  
                  if(rrc_est_rabs.rabs[count].rb_for_rab[rb_index].rb_id ==
                        local_rb_ciph_act_time->elem[n_index].rb_Identity)
                  {
                    saved_security_mode_command_msg.rb_ciph_dl_act_time[saved_security_mode_command_msg.num_rb_ciph_dl_act_time].rb_id = (uint8) 
                      local_rb_ciph_act_time->elem[n_index].rb_Identity;
                    saved_security_mode_command_msg.rb_ciph_dl_act_time[saved_security_mode_command_msg.num_rb_ciph_dl_act_time].seq_num = (uint16)
                      local_rb_ciph_act_time->elem[n_index].rlc_SequenceNumber;           
                    saved_security_mode_command_msg.num_rb_ciph_dl_act_time++;
                  }                
                }
              }
            }
          }           
        }
        n_index++;
      }

    }
  }
}
/*===========================================================================

FUNCTION rrcsmc_step_2_ho_ciph_req

DESCRIPTION
  Process Ciph Config Cnf in Step_2_Ciph_config_cnf substate

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE

===========================================================================*/
uecomdef_status_e_type rrcsmc_step_2_ho_ciph_req
( 
  uint16 step_2_ciph_act_time
)
{
  uint32 step_2_hfn;

  //set ul/dl rlc cipher info to 0's
  cipher_config_db.rlc_ul_cipher_info.nchan=0;
  cipher_config_db.rlc_dl_cipher_info.nchan=0;

  /*get the HFN for Step_2 HFN calculation*/
  step_2_hfn = rrcsmc_convert_START_to_count_c_i(
    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].hfn, 0);

  /*set the HFN value for MAC to the step_2 HFN value.. also set the config
    type enum to STEP_2.. and also copy the ciph_act_time for mac to the new
    activation time sent in HO_TO_UTRAN_COMPLETE message
  we don't need to increment HFN+1 for MAC, as MAC layer takes care of it
  once we hit activation time*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = HO_STEP2_CIPHER_CFG;
    
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].hfn = 
    step_2_hfn >> RRC_MAC_HFN_OFFSET;
    
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].ciph_act_time = 
    step_2_ciph_act_time;
      
  /*call llc function to update rlc/mac ciphering configuration for
    starting STEP_2 ciphering*/
  rrcllc_update_ciphering_config_info_for_handover();

  /*reset the cipher config type to non-HO*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = NON_HO_CIPHER_CFG;

  return SUCCESS;
}
/*===========================================================================

FUNCTION rrcsmc_send_ho_ciph_config_cnf

DESCRIPTION
  Process Ciph Config Cnf in Step_1_Ciph_config_cnf substate

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE

===========================================================================*/
static void rrcsmc_send_ho_ciph_config_cnf
(
  uecomdef_status_e_type status
)
{
  rrc_cmd_type *int_cmd_ptr;        /*cmd ptr to send ciph config req*/

  /*send cipher config req to llc, llc configures lower layers
    with ciphering information*/

    int_cmd_ptr =  rrc_get_int_cmd_buf();
    /* Fill in the command parameters for RRC_CIPH_CONFIG_REQ*/
    int_cmd_ptr->cmd_hdr.cmd_id = RRC_HO_CIPH_CNF;

    /*set the status to what was received*/
    int_cmd_ptr->cmd.rrc_ho_ciph_cnf.status = status;
    
    /*put it in internal queue*/
    rrc_put_int_cmd(int_cmd_ptr);  

    WRRC_MSG0_HIGH("Sending RRC_HO_CIPH_CNF to IHO");
  
  /*reset the cipher config info for CS domain..  for all cases we should always
    have NON_HO_CIPHER_CFG except for brief periods when we are doing Step1 and
    Step2 Ciphering Configuration for Handover*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = NON_HO_CIPHER_CFG;  
}/*rrcsmc_send_ho_ciph_config_cnf*/
/*===========================================================================

FUNCTION rrcsmc_process_wait_for_step_1_ciph_config_cnf_substate 

DESCRIPTION
  Process Ciph Config Cnf in Step_1_Ciph_config_cnf substate

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_wait_for_step_1_ciph_config_cnf_substate 
(
rrc_cmd_type * cmd_ptr
)
{
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_CIPH_CONFIG_CNF:
      WRRC_MSG0_HIGH("Rcvd CIPH CNF from LLC");
      rrcsmc_substate = RRCSMC_INITIAL;
      
      /*send ciphering config success to IHO procedure*/
      rrcsmc_send_ho_ciph_config_cnf(SUCCESS);      
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for SMC in substate:%d", 
               cmd_ptr->cmd_hdr.cmd_id, rrcsmc_substate);
      break;
  }
}
/*===========================================================================

FUNCTION rrcsmc_process_step_1_ho_ciph_req

DESCRIPTION
  Process Step 1 ciphering for Handover

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_step_1_ho_ciph_req
(
rrc_ho_ciph_req_type  ho_ciph_req
)
{
  rrc_cmd_type *int_cmd_ptr;        /*cmd ptr to send ciph config req*/

  /*reset any previous activation times to 0.. */
  memset(&ciphering_info.ul_ciph_act_info, 0, 
         sizeof(ciphering_info.ul_ciph_act_info));
  memset(&ciphering_info.dl_ciph_act_info, 0, 
         sizeof(ciphering_info.dl_ciph_act_info));  
      
#ifdef FEATURE_LTE_TO_WCDMA
if ((CIPH_CONFIG_LTOW == ho_ciph_req.config_cause) || (CIPH_CONFIG_CS_PLUS_PS == ho_ciph_req.config_cause))
  {
      //update latest domain to CS domain as it is a Handover, also update 
    //ciphering algorithm.
    rrcsmc_int_ciph_info.latest_config_cn_id = 
      ho_ciph_req.domain_id;
    rrcsmc_int_ciph_info.saved_ciph_algo[RRC_PS_DOMAIN_CN_ID] = 
      ho_ciph_req.ciph_algo;
    rrcsmc_int_ciph_info.ciph_status[RRC_PS_DOMAIN_CN_ID] = RRCSMC_SECURITY_STARTED;
    rrcsmc_save_security_key_set_from_lte(ho_ciph_req,RRC_PS_DOMAIN_CN_ID);
  }
#endif

  if((CIPH_CONFIG_GTOW == ho_ciph_req.config_cause)
#ifdef FEATURE_LTE_TO_WCDMA
    ||(CIPH_CONFIG_CS == ho_ciph_req.config_cause)||(CIPH_CONFIG_CS_PLUS_PS == ho_ciph_req.config_cause)
#endif
    )
  {
      
  /*set cipher config type to HO_STEP_1*/
  cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].
    cipher_cfg_type = HO_STEP1_CIPHER_CFG;
      
  //update latest domain to CS domain as it is a Handover, also update 
  //ciphering algorithm.
  rrcsmc_int_ciph_info.latest_config_cn_id = RRC_CS_DOMAIN_CN_ID;
    rrcsmc_int_ciph_info.saved_ciph_algo[RRC_CS_DOMAIN_CN_ID] = ho_ciph_req.ciph_algo;
  rrcsmc_int_ciph_info.ciph_status[RRC_CS_DOMAIN_CN_ID] = RRCSMC_SECURITY_STARTED;

  //set cipher_enable to mac to true, as for HO IHO will call this interface
  //only when ciphering is enabled
  cipher_config_db.mac_cipher_info[rrcsmc_int_ciph_info.latest_config_cn_id].
    cipher_enable = TRUE;

#ifdef FEATURE_LTE_TO_WCDMA
    if ((CIPH_CONFIG_CS == ho_ciph_req.config_cause) || (CIPH_CONFIG_CS_PLUS_PS == ho_ciph_req.config_cause))
    {
      rrcsmc_save_security_key_set_from_lte(ho_ciph_req,RRC_CS_DOMAIN_CN_ID);
    }
#endif

  }

  /*send cipher config req to llc, llc configures lower layers
  with ciphering information*/

  int_cmd_ptr =  rrc_get_int_cmd_buf();
  /* Fill in the command parameters for RRC_CIPH_CONFIG_REQ*/
  int_cmd_ptr->cmd_hdr.cmd_id = RRC_CIPH_CONFIG_REQ;
  int_cmd_ptr->cmd.rrc_ciph_config_req.config_cause = ho_ciph_req.config_cause;

  rrc_put_int_cmd(int_cmd_ptr);  

  WRRC_MSG0_HIGH("Sending CIPH_CONFIG_REQ to LLC");

  /*go to wait for ho ciph config cnf substate*/
  rrcsmc_substate = RRCSMC_WAIT_FOR_HO_STEP_1_CIPH_CONFIG_CNF;
  
}/*rrcsmc_process_step_1_ho_ciph_req*/

/*===========================================================================

FUNCTION rrcsmc_fill_crlc_dl_abort_cipher_req

DESCRIPTION
   This function fills the 'crlc_dl_abort_cipher_req' with the DL LC ids for which
   new ciphering configuraiton was given to RLC on receipt of SMC. 

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_fill_crlc_dl_abort_cipher_req
(
  rlc_abort_cipher_config_type *abort_cipher_info_ptr
)
{
  uint32 index;

  abort_cipher_info_ptr->nchan = 
    backup_of_security_variables.cipher_config_db.rlc_dl_cipher_info.nchan;

  for (index=0; index<abort_cipher_info_ptr->nchan; index++)
  {
    abort_cipher_info_ptr->lc_id[index] = 
      backup_of_security_variables.cipher_config_db.rlc_dl_cipher_info.ciphering[index].lc_id;
    WRRC_MSG2_HIGH("ABORT CIPHER REQ for DL-LC ID: %d <total channels: %d>",
        abort_cipher_info_ptr->lc_id[index], abort_cipher_info_ptr->nchan);
  }
}

/*===========================================================================

FUNCTION rrcsmc_fill_crlc_ul_abort_cipher_req

DESCRIPTION
   This function fills the 'crlc_ul_abort_cipher_req' with the UL LC ids for which
   new ciphering configuraiton was given to RLC on receipt of SMC. 

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_fill_crlc_ul_abort_cipher_req
(
  rlc_abort_cipher_config_type *abort_cipher_info_ptr
)
{
  uint32 index;

  abort_cipher_info_ptr->nchan = 
    backup_of_security_variables.cipher_config_db.rlc_ul_cipher_info.nchan;

  for (index=0; index<abort_cipher_info_ptr->nchan; index++)
  {
    abort_cipher_info_ptr->lc_id[index] = 
      backup_of_security_variables.cipher_config_db.rlc_ul_cipher_info.ciphering[index].lc_id;
    WRRC_MSG2_HIGH("ABORT CIPHER REQ for UL-LC ID: %d <total channels: %d>",
        abort_cipher_info_ptr->lc_id[index], abort_cipher_info_ptr->nchan);
  }
}

/*===========================================================================

FUNCTION rrcsmc_check_and_abort_ongoing_security_configuration

DESCRIPTION
   This function checks whether security configuration is in progress or not. 
   If yes, then it will abort the ongoing security configuration by indicating
   LLC to revert to old configuration. This will also indicate LLC as what action
   needs to be taken to abort the security configuration.   

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if security procedure is aborted
  FLASE if secuirty procedure is not active

===========================================================================*/
boolean rrcsmc_check_and_abort_ongoing_security_configuration
(
  void
)
{
  boolean is_ciphering_info_received = FALSE;
  /*local copy of domain id. Set it to cs domain to suppress warning*/
  rrc_cn_domain_identity_e_type domain_id=RRC_CS_DOMAIN_CN_ID;  
  rrc_cmd_type *int_cmd_ptr = NULL;                    /*cmd ptr to send ciph config req*/

  if (TRUE == feature_rollback_security_config)
  {
    if (TRUE == rrcsmc_is_security_reconfig_active())
    {
      WRRC_MSG2_HIGH("SecReconfig is active. smcSubstate: %d (waitL2Ack: %d )", 
          rrcsmc_substate, RRCSMC_WAIT_FOR_L2_ACK);
    
      if ((RRCSMC_WAIT_FOR_L2_ACK == rrcsmc_substate) || 
          (RRCSMC_WAIT_FOR_CIPH_CONFIG_CNF == rrcsmc_substate))
      {
        /* save the information from the ongoing security configuration, which are
           going to be used for rolling-back to old configuration */
        is_ciphering_info_received = rrcsmc_int_ciph_info.ciphering_info_received;
        domain_id = rrcsmc_int_ciph_info.latest_config_cn_id;
    
        /* revert to saved security variables */
        rrcsmc_revert_to_old_security_variables();

       MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"latest_config_cn_id new = %d, prev =%d dl_new_key_flag_per_rb= %d, ciph new_key =%d",
        domain_id,rrcsmc_int_ciph_info.latest_config_cn_id,
                       integrity_protection_info.count_i_info[domain_id].dl_new_key_flag_per_rb[2],
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.new_key
       );
       
        /* If required revert to old integrity keys */
        if ((TRUE == integrity_protection_info.count_i_info[domain_id].
            dl_new_key_flag_per_rb[2])
           /* In case of security keys received in other RAT dl_new_key_flag_per_rb flag for integrity keys    
              *will not be set to FALSE reason being our current design will set HFN part of count_i to zero 
              *by looking at this flag which is not desirable action as per spec section (8.3.6.3)Reception of HANDOVER TO UTRAN 
              *COMMAND. How ever ciph_keys_info.new_key flag will be set to TRUE for new keys generated in other RAT. With this 
              *spec cr implementation we may have the keys generated in other RAT not used at GtoW hand over.
              *that key may be used in this SMC, so we need make sure that we need to revert back to old I.K. CK is taken care while 
              *reverting back variable rrcsmc_int_ciph_info in above function rrcsmc_revert_to_old_security_variables().This spec cr is valid 
              *for CS domain, so revert I.K for CS domain is ciph_keys_info.new_key flag is TRUE only when previous 
              *rrcsmc_int_ciph_info.latest_config_cn_id & latest  rrcsmc_int_ciph_info.latest_config_cn_id CN domain I.D are CS.
              */
            ||((domain_id == rrcsmc_int_ciph_info.latest_config_cn_id)&&
                 (domain_id == RRC_CS_DOMAIN_CN_ID)&&
                 (rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].
            ciph_keys_info.new_key == TRUE))
        )
        {
          WRRC_MSG0_HIGH("Reverting to old integrity keys");
          rrcf9_init_key_values(
            rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key_old,
            rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].integrity_key_old);
        }
       
        /* Do not send the Abort_Cipher_Req if the SMC substate is wait_ciph_config_cnf
             It would be sent when SMC gets the ciph_config_cnf in that handler */
        if( RRCSMC_WAIT_FOR_CIPH_CONFIG_CNF != rrcsmc_substate)
        {
          if ((nv_ciphering_enabled) && (TRUE == is_ciphering_info_received))
          {
            /* Send Abort cipher config req to llc */
              int_cmd_ptr =  rrc_get_int_cmd_buf();
              /* Fill in the command parameters for RRC_CIPH_CONFIG_REQ*/
              int_cmd_ptr->cmd_hdr.cmd_id = RRC_ABORT_CIPH_CONFIG_REQ;
              int_cmd_ptr->cmd.rrc_abort_ciph_config_req.abort_action = 
                                                     RRC_ABORT_CIPH_ACTION_L1KEY_RLC_RESUME;
              WRRC_MSG1_HIGH("Sending RRC_ABORT_CIPH_CONFIG_REQ to LLC. Abort-action: %d", 
                     int_cmd_ptr->cmd.rrc_abort_ciph_config_req.abort_action);
    
              rrc_put_int_cmd(int_cmd_ptr);
            }
    
          rrcsmc_substate = RRCSMC_INITIAL;
        }
        return TRUE;
      }
      else
      {
        /* If we reach here that means SMC is in different sub-state NOT because
             of security mode procedure */
        WRRC_MSG1_HIGH("No need to revert to old configuration in SMC sub-state: %d", rrcsmc_substate);
      }
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION rrcsmc_update_latest_cn_id

DESCRIPTION
   This function updates the latest received CN domain id in the global.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_update_latest_cn_id(void)
{
  uint8 i = 0;
  /*save the previous CN_DOMAIN we were working with*/
  if (saved_security_mode_command_msg.modify_chosen)
  {
    /* dont change latest_config_cn_domain_old_1 when it is already in use */
    for (i=0; i<RRC_MAX_SRB;i++)
    {
      if (TRUE == integrity_protection_info.dl_latest_config_cn_id_old_1_valid[i])
      {
        WRRC_MSG1_ERROR("dl_latest_config_cn_id_old_1_valid already valid for SRB: %d", i);
        break;
      }
    }
    /* dont change latest_config_cn_domain_old_1 when it is already in use */
    if (i == RRC_MAX_SRB)
    {
      rrcsmc_int_ciph_info.latest_config_cn_id_old_1 = rrcsmc_int_ciph_info.latest_config_cn_id_old;
    }

    /*save the previous CN_DOMAIN we were working with*/
    rrcsmc_int_ciph_info.latest_config_cn_id_old=rrcsmc_int_ciph_info.latest_config_cn_id;
    MSG_HIGH("latest_cn_id_old_1: %d, latest_cn_id_old: %d, latest_cn_id: %d",
                rrcsmc_int_ciph_info.latest_config_cn_id_old_1,
                rrcsmc_int_ciph_info.latest_config_cn_id_old,
            rrcsmc_int_ciph_info.latest_config_cn_id);
  }

  /*set the rrcsmc_int_ciph_info.latest_config_cn_id to the domain_id for which we
    received security mode command*/
  rrcsmc_int_ciph_info.latest_config_cn_id =  saved_security_mode_command_msg.domain_id;
}
/*===========================================================================

FUNCTION rrcsmc_validate_security_mode_command_r7

DESCRIPTION
  This function validates security mode command for critical xtensions
  and also to check to see if either ciphering or integrity mode info
  is present in the security mode command
  
DEPENDENCIES
  None

RETURN VALUE
  rrcsmc_config_status_e_type : It returns INVALID_CONFIG if Critical 
  extensions or any other checks fail.  Else VALID_CONFIG is returned
  
SIDE EFFECTS
  None
===========================================================================*/
static rrcsmc_config_status_e_type rrcsmc_validate_security_mode_command_r7
(
rrc_DL_DCCH_Message *msg_ptr              /* Pointer to Downlink DCCH Msg */
)
{
  /*if any security reconfiguration is currently in progress return failure
    indicating that this is a Simultaneous Reconfiguration*/
  if( (integrity_protection_info.reconfig == TRUE) || 
      (ciphering_info.reconfig == TRUE)
    )
  {
    WRRC_MSG0_ERROR("Incompatible Simultaneous Ciph Reconfig");
    rrcsmc_failure_type = RRCSMC_INCOMPATIBLE_SIMULTANEOUS_RECONFIG;
    return RRCSMC_INVALID_CONFIGURATION;
  }

  /*if both nv items are not enabled, then do not check any more IE's just
    return success*/
  if((!nv_ciphering_enabled) && (!nv_integrity_enabled))
  {
    WRRC_MSG2_ERROR("NV cipher = %d & NV integrity = %d not enabled", nv_ciphering_enabled,
                       nv_integrity_enabled);
    return RRCSMC_VALID_CONFIG;
  }

  /*Either ciphering or integrity protection mode info ie's should be present
    in message received by UE*/
  if (msg_ptr->message.u.securityModeCommand.u.later_than_r3.criticalExtensions.
        u.r7.securityModeCommand_r7.m.cipheringModeInfoPresent)
  {
    rrcsmc_int_ciph_info.ciphering_info_received = TRUE;
  }
  else
  {
    rrcsmc_int_ciph_info.ciphering_info_received = FALSE;
  }

  if (msg_ptr->message.u.securityModeCommand.u.later_than_r3.criticalExtensions.
        u.r7.securityModeCommand_r7.m.integrityProtectionModeInfoPresent)
  {  
    rrcsmc_int_ciph_info.integrity_info_received = TRUE;
  }
  else
  {
    rrcsmc_int_ciph_info.integrity_info_received = FALSE;
  }

  /* if Ciphering Mode IE's and Integrity Protection Mode IE's
     are not included, then send SMC Failure message */  
  if( (rrcsmc_int_ciph_info.integrity_info_received == FALSE ) &&
      (rrcsmc_int_ciph_info.ciphering_info_received == FALSE )
    )
  {
     WRRC_MSG0_ERROR("Incomplte Configuration NO cipher & integrity info in OTA msg"); 
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  return RRCSMC_VALID_CONFIG;
}/* rrcsmc_validate_security_mode_command */
/*===========================================================================

FUNCTION rrcsmc_validate_ciph_security_mode_command_r7

DESCRIPTION
  This function validates ciphering part of SECURITY MODE COMMAND.
  
DEPENDENCIES
  None

RETURN VALUE
  rrcsmc_config_status_e_type : It returns INVALID_CONFIG if Critical 
  extensions or any other checks fail.  Else VALID_CONFIG is returned
  
SIDE EFFECTS
  None
===========================================================================*/
static rrcsmc_config_status_e_type rrcsmc_validate_ciph_security_mode_command_r7
(
rrc_DL_DCCH_Message *msg_ptr              /* Pointer to Downlink DCCH Msg */
)
{
  uint32 count, i, local_lc_id;            /*vars used for count purposes*/
  boolean tm_rb_present;                   /*flag used to check if tm rb's are present*/
  rrc_CN_DomainIdentity local_domain_id;   /*domain id for which ciph config is being changed*/
  uint32 num_srbs_established = 3;         /*SRBs 1,2 and 3 are mandatory*/
  uint32 index=0,n_value=0;
  uint8 cipher_algo_suppported = 0;
  uecomdef_wcdma_cipher_algo_e_type cipher_algo_configured = UE_WCDMA_CIPHER_ALGO_NONE;

  /*local ptr to Ciph. Mode Info. */
  struct rrc_CipheringModeInfo_r7* ciphModeInfo;

  /* local ptr to activation time list */
  struct rrc_RB_ActivationTimeInfoList  *local_rb_ciph_act_time_list;

  rrc_RB_Identity srb5_rb_id = RRCLLC_INVALID_RB_ID;

  /*set to FALSE*/
  tm_rb_present = FALSE;

  /* if Ciphering Mode IE's are not included, then send SMC Failure message */
  if ( (msg_ptr->message.u.securityModeCommand.u.later_than_r3.criticalExtensions.
        u.r7.securityModeCommand_r7.m.cipheringModeInfoPresent) == 0 )
  {
     WRRC_MSG0_ERROR("Incomplte Configuration"); 
    rrcsmc_failure_type = RRCSMC_INCOMPLETE_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  /*check if activation times are present in do ciph activation.. they have to be present
    as SRB's are already started and have to be ciphered*/
  if ( (msg_ptr->message.u.securityModeCommand.u.later_than_r3.criticalExtensions.
        u.r7.securityModeCommand_r7.cipheringModeInfo.m.rb_DL_CiphActivationTimeInfoPresent) == 0 )
  {
    /*lint -e793*/
     WRRC_MSG0_ERROR("rrc_CipheringModeInfo_r7_rb_DL_CiphActivationTimeInfo_present not present");
    /*lint +e793*/
    rrcsmc_failure_type = RRCSMC_INCOMPLETE_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  /*get ptr to cipheringModeInfo. copy this to a local ptr to avoid 
    multiple indirections in later manipulations*/
  ciphModeInfo = &(msg_ptr->message.u.securityModeCommand.u.
            later_than_r3.criticalExtensions.u.r7.securityModeCommand_r7.cipheringModeInfo);

  /* Check to see if Ciph Activation time is included for all SRBs.Ignore SRB0
     since it is not ciphered.Check for SRBs 1,2 and 3 since they are mandatory.
     Check for RB4 only if it is already established */
  if(RRCLCM_RLC_LC_ID_NOT_FOUND != rrclcm_check_dl_rlc_lc_id(
                                   UE_LOGCHAN_DCCH,
                                   DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                   UE_MODE_ACKNOWLEDGED_DATA))
  {
    MSG_MED("SRB4 established",0,0,0);
    num_srbs_established = 4;
  }

  for(count = 1; count <= num_srbs_established; count++)
  {
    uint32 srb_found = 0;

    /* get the activation time list for RBs */
    local_rb_ciph_act_time_list = &(ciphModeInfo->rb_DL_CiphActivationTimeInfo);

    index = 0;
    n_value = local_rb_ciph_act_time_list->n;

    /* Check to see if this RB info is included in the ciph activation times list */
    while (n_value != 0)
    {
      if(count == local_rb_ciph_act_time_list->elem[index].rb_Identity)
      {
        MSG_MED("SRB id: %d included in ciph activation info in SMC",count,0,0);
        srb_found++;
      }
      n_value--;
      index++;
    }

    if(srb_found != 1)
    {
      WRRC_MSG2_ERROR("SRB id: %d is included %d times in ciph activation info in SMC",
        count, srb_found);
      rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
      return RRCSMC_INVALID_CONFIGURATION;
    }
  }
  
  if(rrclcm_get_rb_id_if_dl_lc_type_exists(UE_LOGCHAN_TM_DCCH_DL, &srb5_rb_id) == RRCLCM_SUCCESS)
  {
    /* get the activation time list for RBs */
    local_rb_ciph_act_time_list = &(ciphModeInfo->rb_DL_CiphActivationTimeInfo);

    index = 0;
    n_value = local_rb_ciph_act_time_list->n;

    /* Check to see if this RB info is included in the ciph activation times list */
    while (n_value != 0)
    {
      if(srb5_rb_id == local_rb_ciph_act_time_list->elem[index].rb_Identity)
      {
        WRRC_MSG2_ERROR("SMC validation failed; SRB#5 Id = %d, RB ID: %d", srb5_rb_id, local_rb_ciph_act_time_list->elem[index].rb_Identity);
        rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
        return RRCSMC_INVALID_CONFIGURATION;
      }
      n_value--;
      index++;
    }
  }

  /*get the domain id of the latest configured CN domain for which we are
    configuring ciphering*/
  local_domain_id = msg_ptr->message.u.securityModeCommand.u.
            later_than_r3.criticalExtensions.u.r7.securityModeCommand_r7.cn_DomainIdentity;

  /*check to see if TM radio bearers are present.  only if present will we
    mandatorily check to see DPCH activation time is present.
    Also if the domain id is PS,check if SMC has 
    included all the PS RBs activation time in the message.If not
    UE should send a security mode failure and not honor the SMC */
  for ( count=0; count<MAX_RAB_TO_SETUP; count++ )
  {
    if (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID)
    {
      for (i=0; i< rrc_est_rabs.rabs[count].num_rbs_for_rab; i++)
      {
        if (rrc_est_rabs.rabs[count].rb_for_rab[i].rb_status ==
            RRC_RB_STARTED)
        {
          /*check if domain id is same as domain for which ciphering
            configuration is received for*/
          if(rrc_est_rabs.rabs[count].cn_domain == local_domain_id)
          {
            if(local_domain_id == rrc_CN_DomainIdentity_ps_domain) 
            {
              uint32 ps_rb_found = 0;

              /* get the activation time list for RBs */
              local_rb_ciph_act_time_list = &(ciphModeInfo->rb_DL_CiphActivationTimeInfo);

              index = 0;
              n_value = local_rb_ciph_act_time_list->n;

              /* Check to see if this RB info is included in the ciph activation times list */
              while (n_value != 0)
              {
                if(rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id ==
                    local_rb_ciph_act_time_list->elem[index].rb_Identity)
                {
                  ps_rb_found++;
                }
                n_value--;
                index++;
              }
              if(ps_rb_found != 1)
              {
                WRRC_MSG2_ERROR("RB id: %d is included %d times in ciph activation info in SMC",
                     rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id,
                     ps_rb_found);
                rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
                return RRCSMC_INVALID_CONFIGURATION;
              }
            }
            else /* CS domain */
            {
              /*copy to local vars to avoid multiple indirections*/
              local_lc_id = rrclcm_get_ul_rlc_lc_id(rrc_est_rabs.rabs[count].rb_for_rab[i].rb_id);
  
              /*set TM radio bearers flag present to true*/
              if (local_lc_id < RRCLCM_UL_UM_START_IDX)
              {
                tm_rb_present=TRUE;
                break;
              }
            }
          }
        }/*if*/
      }/*for*/
    }/*if*/

    /*if TM radio bearers are present, then break right now as we don't have
      to loop through all rabs in est_rabs*/
    if (tm_rb_present == TRUE)
    {
      break;
    }
  }/*for loop till MAX_RAB_TO_SETUP*/

  /*if only TM radio bearers are present then mandatorily DPCH activation
    information should be present.*/
  if ( ((ciphModeInfo->m.activationTimeForDPCHPresent) == 0) && 
       (tm_rb_present == TRUE )
     )
  {
     WRRC_MSG0_ERROR("No DPCH Activation Time"); 
    rrcsmc_failure_type = RRCSMC_INCOMPLETE_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

    cipher_algo_suppported = UEA_1_SUPPORTED + UEA_0_SUPPORTED;
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
    if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && snow3g_security_algo_supported)
    {
      cipher_algo_suppported += UEA_2_SUPPORTED;
    }
#endif

  cipher_algo_configured = rrcsmc_get_int_cipher_algo_from_dl_sdu_ind(
              &(ciphModeInfo->cipheringModeCommand.startRestart),MSG_REL7);

  if(!(cipher_algo_suppported & cipher_algo_configured))
  {
    WRRC_MSG2_ERROR("Mismatch between cipher algo supported %d and configured %d", cipher_algo_suppported, cipher_algo_configured);  
    rrcsmc_failure_type = RRCSMC_UNSUPPORTED_CONFIG; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  return RRCSMC_VALID_CONFIG;
}/* rrcsmc_validate_ciph_security_mode_command */
/*===========================================================================

FUNCTION rrcsmc_validate_int_security_mode_command_r7

DESCRIPTION
  This function validates integrity part of SECURITY MODE COMMAND.
  
DEPENDENCIES
  None

RETURN VALUE
  rrcsmc_config_status_e_type : It returns INVALID_CONFIG if Critical 
  extensions or any other checks fail.  Else VALID_CONFIG is returned
  
SIDE EFFECTS
  None
===========================================================================*/
static rrcsmc_config_status_e_type rrcsmc_validate_int_security_mode_command_r7
(
rrc_DL_DCCH_Message *msg_ptr              /* Pointer to Downlink DCCH Msg */
)
{  
  /*local ptr to Int. Prot. Mode Info.  */
  struct rrc_IntegrityProtectionModeInfo_r7* integrityProtModeInfo;
  uint8 int_algo_supported = 0;
  uecomdef_wcdma_integrity_protection_algorithm_e_type int_algo_configured = UE_WCDMA_INTEGRITY_PROT_ALGO_NONE;

  /* if Integrity Protection Mode Info is not included, then send
     SMC Failure message
   */
  if ((msg_ptr->message.u.securityModeCommand.u.later_than_r3.
        criticalExtensions.u.r7.securityModeCommand_r7.m.integrityProtectionModeInfoPresent) == 0)
  {
     WRRC_MSG0_ERROR("Incomplete Configuration"); 
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION; 
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  integrityProtModeInfo = &(msg_ptr->message.u.securityModeCommand.u.later_than_r3.
        criticalExtensions.u.r7.securityModeCommand_r7.integrityProtectionModeInfo);

  /*if we have already started integrity protection, then we should not
   *receive another start command*/
  if ( (integrityProtModeInfo->integrityProtectionModeCommand.t ==
        T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection) &&
       (integrity_protection_info.ip_status == RRCSMC_SECURITY_STARTED) )
  {
     WRRC_MSG0_ERROR("Invalid Start Cmd ");   
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION;  
    return RRCSMC_INVALID_CONFIGURATION;
  }

  /*if we receive a START cmd and do not receive the integrity 
    protection algorithm, return invalid config*/
  if ( (integrityProtModeInfo->integrityProtectionModeCommand.t == 
        T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection) &&  
       (integrityProtModeInfo->m.integrityProtectionAlgorithmPresent == 0 ) 
     )
  {
     WRRC_MSG0_ERROR("Incomplete Configuration");     
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION;  
    return RRCSMC_INVALID_CONFIGURATION;  
  }

  /*if we receive a MODIFY cmd and integrity is not yet started
    return invalid config*/
  if ( (integrityProtModeInfo->integrityProtectionModeCommand.t ==
        T_rrc_IntegrityProtectionModeCommand_modify) &&
       (integrity_protection_info.ip_status == RRCSMC_SECURITY_NOT_STARTED) )
  {
     WRRC_MSG0_ERROR("Invalid Configuration");     
    rrcsmc_failure_type = RRCSMC_INVALID_CONFIGURATION;  
    return RRCSMC_INVALID_CONFIGURATION;    
  }

  if(integrityProtModeInfo->m.integrityProtectionAlgorithmPresent)
  {
    /* Right shift by 1 is added since the macro value starts from 0x2
       and to make it uniform to check against return type of rrcsmc_get_int_integrity_algo_from_dl_sdu_ind
       #define KASUMI_SUPPORTED         0x0002    */
      int_algo_supported = (KASUMI_SUPPORTED >> 1);
  #ifdef FEATURE_WCDMA_SNOW3G_SECURITY
      if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && snow3g_security_algo_supported)
      {
        int_algo_supported += (SNOW_3G_SUPPORTED >> 1);
      }
#endif

    int_algo_configured = rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
                &(integrityProtModeInfo->integrityProtectionAlgorithm),MSG_REL7);
  
    if(!(int_algo_supported & int_algo_configured))
    {
      WRRC_MSG2_ERROR("Mismatch between int algo supported %d and configured %d", int_algo_supported, int_algo_configured);  
      rrcsmc_failure_type = RRCSMC_UNSUPPORTED_CONFIG; 
      return RRCSMC_INVALID_CONFIGURATION;    
    }
  }
  
  return RRCSMC_VALID_CONFIG;          

}/*rrcsmc_validate_int_security_mode_command*/
/*===========================================================================

FUNCTION rrcsmc_check_integrity_for_security_mode_command_r7

DESCRIPTION
  This function validates Integrity Protection for Security Mode Command.
  It also updates the FRESH and KEYS if it is the START of Integrity.
  It also updates the KEY values if we have received new keys.
  
DEPENDENCIES
  None

RETURN VALUE
  uecomdef_status_e_type : 
  Returns SUCCESS if Integrity Check for Security Mode Command 
  is successful.
  Returns FAILURE if Integrity Check for Security Mode Command
  is not successful
  
SIDE EFFECTS
  None
===========================================================================*/
static uecomdef_status_e_type rrcsmc_check_integrity_for_security_mode_command_r7
(
  rrc_cmd_type *cmd_ptr                    /* Pointer to the RRC Command */
)
{  
  rrc_DL_DCCH_Message *dcch_msg_ptr;         /* Pointer to Downlink DCCH Msg */

  rrc_RB_Identity rb_id;/*local var to store rb_id*/
  rrc_IntegrityProtectionModeInfo_r7* integrityProtModeInfo;

  /*flag to indicate whether new keys are being*/  
  boolean rcvd_new_keys_flag=FALSE;

  /*local vars to store bunch of values used for manipulation in this func.only*/
  uint32 local_fresh, local_seq_no, old_seq_no; 
  uint32 local_count_i = 0, local_hfn;
  uint32 received_mac, local_mac;
  rrcsmc_integrity_check_info_type int_check_info;

  /*local copy of domain id. Set it to cs domain to suppress warning*/
  rrc_cn_domain_identity_e_type domain_id=RRC_CS_DOMAIN_CN_ID;  


  /*get the dcch_msg_ptr for SMC as this message is already decoded*/
  dcch_msg_ptr =  (rrc_DL_DCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;


  /*copy integrityProtectionModeInfo * to a local copy to make easy readability*/
  integrityProtModeInfo = &(dcch_msg_ptr->message.u.securityModeCommand.u.
    later_than_r3.criticalExtensions.u.r7.securityModeCommand_r7.integrityProtectionModeInfo);


  /*get the domain id*/
  if (dcch_msg_ptr->message.u.securityModeCommand.u.
            later_than_r3.criticalExtensions.u.r7.securityModeCommand_r7.
            cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
  {
    domain_id = RRC_CS_DOMAIN_CN_ID;
  }
  else if (dcch_msg_ptr->message.u.securityModeCommand.u.
            later_than_r3.criticalExtensions.u.r7.securityModeCommand_r7.
            cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
  {
    domain_id = RRC_PS_DOMAIN_CN_ID;
  }

  /*get the rb_id on which the message was received*/
  rb_id = rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id);

  if(rb_id < RRC_MAX_SRB)
  {
    local_seq_no = dcch_msg_ptr->integrityCheckInfo.rrc_MessageSequenceNumber;

    if (integrityProtModeInfo->integrityProtectionModeCommand.t == 
        T_rrc_IntegrityProtectionModeCommand_modify)
    {
      if (integrity_protection_info.count_i_info[domain_id].
          dl_new_key_flag_per_rb[rb_id] == TRUE)
      {
        /*set this local flag to true*/
        rcvd_new_keys_flag=TRUE;
        /*set hfn to zero, as we have to reset HFN part of start value to 0*/
        local_hfn = 0x0;
      }
      else if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id]
              == TRUE)
      {
        /*if the start_value_update flag is set then we have updated the start value
          due to an IDT, this value needs to be used for COUNT_I calculation*/
        local_hfn =  
          rrcsmc_convert_START_to_count_c_i(
            rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,
            0);      
      }
      else
      {
        local_hfn = integrity_protection_info.count_i_info[domain_id].
                    dl_count_i[rb_id] & RRC_MSB_20BIT_HFN_BIT_MASK;
      }

      /*get the seq num of the old domain*/
      old_seq_no = (integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.
                    latest_config_cn_id].dl_count_i[rb_id] & RRC_SEQ_NUM_BIT_MASK);


      /*check if old sequence number is greater than current seq. no.  if that is the
        case, then there has been a rollover.. so increment the HFN by 1, as per 1808*/
      if (old_seq_no > local_seq_no)
      {
        /*increment hfn only*/
        local_hfn >>=4;
        local_hfn++;
        local_hfn <<=4;
      }
    }
    else
    {
      local_hfn = integrity_protection_info.count_i_info[domain_id].dl_count_i[rb_id];
    }

    /*generate count_i from hfn and sequence number*/
    local_count_i = 
    (uint32)((local_hfn & RRC_HFN_BIT_MASK) | 
             (local_seq_no & RRC_SEQ_NUM_BIT_MASK) 
             );
  }
  else
  {
    WRRC_MSG1_ERROR("rb-id %d is not SRB",rb_id);
    return FAILURE;    
  }
  /*get the value of fresh from the received message and initialize F9 engine.
    We can receive the value of FRESH only if START chosen*/
  if (integrityProtModeInfo->integrityProtectionModeCommand.t == 
      T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection)
  {
    local_fresh = b_unpackd(
      integrityProtModeInfo->integrityProtectionModeCommand.u.startIntegrityProtection->
      integrityProtInitNumber.data, 
      RRC_DEFAULT_BIT_POS,
      RRC_FRESH_LENGTH_IN_BITS);  

    /*update fresh value for this connection in F9 engine*/
    rrcf9_init_fresh_per_connection(local_fresh);
  }

  /*init the IK values for CS and PS domains and the fresh value for
    this connection*/
  rrcf9_init_key_values(
    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key,
    rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].integrity_key);


  (void) rrcsmc_process_dl_integrity_check_info(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length,
                                                &int_check_info,rb_id);

  /*Update the received message*/
  received_mac = int_check_info.msg_auth_code;

  /*call the f9 engine to generate MAC for the dl message*/  
  local_mac = rrcf9_generate_mac_for_message(
                local_count_i,
                RRC_INTEGRITY_DIR_DOWNLINK,
                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length*8),
                domain_id,
                rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
            &integrityProtModeInfo->integrityProtectionAlgorithm,
             MSG_REL7));


   MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"local_mac = 0x%x, received_mac = 0x%x CN domain %d[0=CS,1=PS], Integrity Algo = %d[0=Invalid,1=UIA1,2=UIA2]",
   local_mac,
   received_mac,
   domain_id,
    rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
            &integrityProtModeInfo->integrityProtectionAlgorithm,
             MSG_REL7));
   

  /*if the generated and received mac are the same, update our copy of count_i
    and set the init_flag to true*/
  if ( local_mac == received_mac)
  {
    if(rb_id < RRC_MAX_SRB)
    {
      /* new keys flag wont be set to TRUE is received type is integrity STARTED */
      if (rcvd_new_keys_flag == TRUE)
      {
        /*reset new keys flag.  we are changing it here to be sure that the SMC
            command was successfully integrity checked*/
        integrity_protection_info.count_i_info[domain_id].
        dl_new_key_flag_per_rb[rb_id] = FALSE;

        /*any previously sent start value is obliterated.. we should start using 0's from
          now onwards.. hence set start_value_update_flag to false*/
        rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;      

        /*reset the hfn values to 0's if authentication is done and UE is
         *in WCDMA mode in dch/fach
         */
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[0]=0x0;
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[1]=0x0;
        rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[2]=0x0;

        MSG_HIGH("Started using new keys, CIPH START VAL: 0x%x 0x%x 0x%x", 
           rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[0],
           rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[1],
           rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn[2]);
        
      }
      if((domain_id == RRC_CS_DOMAIN_CN_ID) && (rrcsmc_int_ciph_info.int_ciph_info[domain_id].
      ciph_keys_info.new_key == TRUE))
      {
        rrciho_send_cipher_sync_ind();
      }

      integrity_protection_info.count_i_info[domain_id].dl_count_i[rb_id] = local_count_i;

      integrity_protection_info.count_i_info[domain_id].
      dl_count_i_init_flag[rb_id] = TRUE;

      /*reset the start_value_update flag as we already passed integrity check
        successfully for this domain, also copy the updated start value to the
        database and initialize rb2, as there is no act time for rb2..
        also start_value_update_flag will be true only for Integrity Modify,
        as START value will be updated for integrityStart in smc_initial substate
        function*/
      if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id] == TRUE &&
         !(rcvd_new_keys_flag))
      {
        /*copy the latest start value sent in IDT or CU message*/
        WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
                     sizeof(byte) * RRC_MAX_HFN_LENGTH,
                     rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,    
                     RRC_MAX_START_LENGTH);
        
        /*since there is no pending activation time for RB2, update COUNT_I info for new 
          CN domain here when successfully check for SMC is done*/
        integrity_protection_info.count_i_info[domain_id].ul_count_i[2] =         
          rrcsmc_convert_START_to_count_c_i(
            rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
            integrity_protection_info.count_i_info[rrcsmc_int_ciph_info.latest_config_cn_id].
            ul_count_i[2]);

        rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;
      }
      else if(!rcvd_new_keys_flag) 
      {
          /*since there is no pending activation time for RB2, update COUNT_I info for new 
          CN domain here when successfully check for SMC is done*/
        integrity_protection_info.count_i_info[domain_id].ul_count_i[2]= 
        (integrity_protection_info.count_i_info[domain_id].ul_count_i[2] & RRC_SEQ_NUM_BIT_MASK )| 
        (integrity_protection_info.count_i_info[domain_id].ul_count_i[2] & RRC_MSB_20BIT_HFN_BIT_MASK);
      }

      //set the Integrity START status for this domain ID to true.
      //only if this flag is set, then we will extract latest start values
      //at connection release time.
      rrcsmc_int_ciph_info.int_start_status_for_conn[domain_id] = TRUE;
    }
    return SUCCESS;
  }
  else
  {
    /*Logging the IK keys used for MAC-I generation*/
    PRINT_INT_KEY(rrcsmc_int_ciph_info.int_ciph_info[domain_id])
      
    /*if we have received new keys and this SMC message failed Integrity
        test, then we revert back to the old keys as we will not be using 
        the new keys or new Security Configuration*/
    if (rcvd_new_keys_flag == TRUE)
    {
      rrcf9_init_key_values(
       rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key_old,
       rrcsmc_int_ciph_info.int_ciph_info[RRC_PS_DOMAIN_CN_ID].integrity_key_old);
    }
    return FAILURE;
  }
}/*rrcsmc_check_integrity_for_security_mode_command*/
/*===========================================================================

FUNCTION rrcsmc_save_contents_of_security_mode_command_r7

DESCRIPTION
   This function processes the SECURITY MODE COMMAND received on DL DCCH
   and extracts the Transaction ID.  Sends the SYNC_IND cmd to MM and
   also calls the function to send SECURITY MODE COMPLETE

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_save_contents_of_security_mode_command_r7
(
rrc_DL_DCCH_Message *dcch_msg_ptr
)
{

  uint32 i, index=0, n_value=0,rb_index=0, count=0; /*used for counter purposes*/
  struct rrc_SecurityModeCommand_r7_IEs * local_smc_msg_ptr;

  /*local var to store act time linked list ptr*/
  struct rrc_RRC_MessageSequenceNumberList *local_smc_act_list;

  struct rrc_RB_ActivationTimeInfoList    *local_rb_ciph_act_time;

  /*copy indirected link to a local ptr*/
  local_smc_msg_ptr = &(dcch_msg_ptr->message.u.securityModeCommand.u.
            later_than_r3.criticalExtensions.u.r7.securityModeCommand_r7);

  /*save transaction id*/
  saved_security_mode_command_msg.transaction_id = (OSUINT8)
            dcch_msg_ptr->message.u.securityModeCommand.u.
            later_than_r3.rrc_TransactionIdentifier;

  /*copy domain id*/
  if(local_smc_msg_ptr->cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain) 
  {
    saved_security_mode_command_msg.domain_id = RRC_CS_DOMAIN_CN_ID;
  }
  else
  {
    saved_security_mode_command_msg.domain_id = RRC_PS_DOMAIN_CN_ID;
  }

  if(rrcsmc_int_ciph_info.integrity_info_received == TRUE)
  {

    /*check if start or modify is chosen and update accordingly*/
    if (local_smc_msg_ptr->integrityProtectionModeInfo.
        integrityProtectionModeCommand.t ==  T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection)
    {
      saved_security_mode_command_msg.start_chosen = TRUE;
      saved_security_mode_command_msg.modify_chosen = FALSE;
  
      /*get the fresh value*/
      saved_security_mode_command_msg.start = 
      b_unpackd(local_smc_msg_ptr->integrityProtectionModeInfo.
                integrityProtectionModeCommand.u.startIntegrityProtection->
                integrityProtInitNumber.data,
                RRC_DEFAULT_BIT_POS,
                RRC_FRESH_LENGTH_IN_BITS);  

      //save the fresh value for srns relocation purposes..
      saved_srns_info.fresh_old = saved_security_mode_command_msg.start;

      /* Since its the first time FRESH value is provided to the UE, hence
         there is no old1 fresh value. Hence, saving the received FRESH 
         value in fresh_old1 as well. This stored value is needed to revert back to old 
         configuration after SRNS relocation physChannelEstFailure */
      saved_srns_info.fresh_old1 = saved_security_mode_command_msg.start;
    }
    else
    {
      WRRC_MSG1_HIGH("Saved FRESH is 0x%x",saved_security_mode_command_msg.start);

      saved_security_mode_command_msg.start_chosen = FALSE;
      saved_security_mode_command_msg.modify_chosen = TRUE;
    
      /*initialize seqNumList*/
      local_smc_act_list = &(local_smc_msg_ptr->integrityProtectionModeInfo.
        integrityProtectionModeCommand.u.modify->dl_IntegrityProtActivationInfo.
        rrc_MessageSequenceNumberList);

      /*initialize count_i for NEW latest CN domain*/
      index = 0;
      n_value = local_smc_act_list->n;
        
      for ( i=0; i < RRC_MAX_SRB; i++)
      {
        
        if(n_value == 0)
        {
          break;
        }
  
        /*initialize downlink count_i dl_activation_info time sent by utran
          for this domain */
        saved_security_mode_command_msg.srb_int_dl_act_time[i].seq_num = 
        (uint8)local_smc_act_list->elem[index];
        n_value--;
        index++;
      }
    }
    
    /*save the integrity algorithm sent*/
    saved_security_mode_command_msg.integrity_algo = 

    rrcsmc_int_ciph_info.saved_integrity_algo[saved_security_mode_command_msg.domain_id] 
       = rrcsmc_get_int_integrity_algo_from_dl_sdu_ind(
            &local_smc_msg_ptr->integrityProtectionModeInfo.integrityProtectionAlgorithm,
             MSG_REL7);

  }

  if(rrcsmc_int_ciph_info.ciphering_info_received == TRUE)
   {
  
    /*save the ciphering algorithm sent*/
    saved_security_mode_command_msg.ciph_algo = 
          rrcsmc_get_int_cipher_algo_from_dl_sdu_ind(
              &(local_smc_msg_ptr->cipheringModeInfo.cipheringModeCommand.startRestart),
              MSG_REL7);
  
    /*save to temp var*/
    rrcsmc_int_ciph_info.saved_ciph_algo[saved_security_mode_command_msg.domain_id] = 
          rrcsmc_get_int_cipher_algo_from_dl_sdu_ind(
              &(local_smc_msg_ptr->cipheringModeInfo.cipheringModeCommand.startRestart),
              MSG_REL7);
  
    /*copy activation time for DPCH if sent*/
    if (local_smc_msg_ptr->cipheringModeInfo.m.activationTimeForDPCHPresent)
    {
      saved_security_mode_command_msg.dpch_ciph_act_time_present = TRUE;
      saved_security_mode_command_msg.dpch_ciph_act_time = 
        local_smc_msg_ptr->cipheringModeInfo.activationTimeForDPCH;
    }
    else
    {
      saved_security_mode_command_msg.dpch_ciph_act_time_present = FALSE;
    }
  
    if (local_smc_msg_ptr->cipheringModeInfo.m.rb_DL_CiphActivationTimeInfoPresent)
    {
      /*copy Radio Bearer DL activation time for all UM/AM RB's*/
      local_rb_ciph_act_time = &(local_smc_msg_ptr->cipheringModeInfo.
                               rb_DL_CiphActivationTimeInfo);
  
      /*set the flag to true*/
      saved_security_mode_command_msg.rb_ciph_dl_act_time_present = TRUE;
  
      /*init local vars used for counter purposes to zero*/
      saved_security_mode_command_msg.num_rb_ciph_dl_act_time = 0;
      index= 0;
  
      /*in a while loop, copy the number of DL activation time for each
        radio bearer*/
      while((local_rb_ciph_act_time->n > index) && 
             (index < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY)) &&
             (saved_security_mode_command_msg.num_rb_ciph_dl_act_time < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY)))
      {
        if((local_rb_ciph_act_time->elem[index].rb_Identity == DCCH_UM_RADIO_BEARER_ID) ||
           (local_rb_ciph_act_time->elem[index].rb_Identity == DCCH_AM_RADIO_BEARER_ID) ||
           (local_rb_ciph_act_time->elem[index].rb_Identity == DCCH_DT_HIGH_PRI_RADIO_BEARER_ID) ||
           (local_rb_ciph_act_time->elem[index].rb_Identity == DCCH_DT_LOW_PRI_RADIO_BEARER_ID))
        {
          saved_security_mode_command_msg.rb_ciph_dl_act_time[saved_security_mode_command_msg.num_rb_ciph_dl_act_time].rb_id = (uint8) 
            local_rb_ciph_act_time->elem[index].rb_Identity;
          saved_security_mode_command_msg.rb_ciph_dl_act_time[saved_security_mode_command_msg.num_rb_ciph_dl_act_time].seq_num = (uint16)
            local_rb_ciph_act_time->elem[index].rlc_SequenceNumber;           
          saved_security_mode_command_msg.num_rb_ciph_dl_act_time++;
        }
        else
        {
          for ( count=0; count<MAX_RAB_TO_SETUP; count++ )
          {
            if (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID)
            {
              for (rb_index = 0; (rb_index < rrc_est_rabs.rabs[count].num_rbs_for_rab) && (saved_security_mode_command_msg.num_rb_ciph_dl_act_time < (UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY)); rb_index++)
              {
                if (rrc_est_rabs.rabs[count].rb_for_rab[rb_index].rb_status ==
                    RRC_RB_STARTED)
                {
                  /* Check to see if this RB info is included in the ciph activation times list */
                  
                  if(rrc_est_rabs.rabs[count].rb_for_rab[rb_index].rb_id ==
                        local_rb_ciph_act_time->elem[index].rb_Identity)
                  {
                    saved_security_mode_command_msg.rb_ciph_dl_act_time[saved_security_mode_command_msg.num_rb_ciph_dl_act_time].rb_id = (uint8) 
                      local_rb_ciph_act_time->elem[index].rb_Identity;
                    saved_security_mode_command_msg.rb_ciph_dl_act_time[saved_security_mode_command_msg.num_rb_ciph_dl_act_time].seq_num = (uint16)
                      local_rb_ciph_act_time->elem[index].rlc_SequenceNumber;           
                    saved_security_mode_command_msg.num_rb_ciph_dl_act_time++;
                  }                
                }
              }
            }
          }           
        }
        index++;
      }
    }
  }
}

/*===========================================================================

FUNCTION rrcsmc_process_smc_initial_substate

DESCRIPTION
   This function processes the SECURITY MODE COMMAND received on DL DCCH
   and extracts the Transaction ID.  Sends the SYNC_IND cmd to MM and
   also calls the function to send SECURITY MODE COMPLETE

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_security_mode_command_r7
(
  rrc_DL_DCCH_Message *dcch_msg_ptr,  /* Pointer to Downlink DCCH */
  rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  uint32 rrcsmc_trans_id = 0;                   /* local copy of trans id*/

  rrc_CN_DomainIdentity domain_id;              /* local copy of domain id*/


  /*Processing for R3 security mode command message*/
  
  /* Get the transaction identified from the SECURITY MODE COMMAND and
   store it in local copy*/
  rrcsmc_trans_id = dcch_msg_ptr->message.u.securityModeCommand.u.
         later_than_r3.rrc_TransactionIdentifier;  

  /*check to see that no critical extensions are added and no reconfig
     is currently being done*/
  if (RRCSMC_VALID_CONFIG != rrcsmc_validate_security_mode_command_r7(dcch_msg_ptr) )
  {
    WRRC_MSG0_ERROR("SMC MSG VALIDATION FAILED");
    rrcsmc_send_security_mode_failure(rrcsmc_trans_id);          
    return;
  }
  
  /*check if WCDMA security capablities are correct with UECI procedure*/
  if(rrcueci_verify_security_capability((&dcch_msg_ptr->message.u.
    securityModeCommand.u.later_than_r3.criticalExtensions.u.r7.
    securityModeCommand_r7.securityCapability)) == FALSE)
  {
    WRRC_MSG0_ERROR("SMC Sec. Cap. Check failed");
    //TBD use CSP to take UE to IDLE
    (void)rrc_transition_to_disconnected_state(RRC_PROCEDURE_SMC,RRC_TX_TO_DISCON_OTHER);
    return;
  }
  
  /*if interrat security capabilities are present in the message then check its's
    validity with UECI procedure*/
  if(dcch_msg_ptr->message.u.securityModeCommand.u.later_than_r3.
    criticalExtensions.u.r7.securityModeCommand_r7.m.ue_SystemSpecificSecurityCapPresent)
  {
    if(rrcueci_verify_inter_rat_security_capability(
	  ((struct rrc_InterRAT_UE_SecurityCapList *)&dcch_msg_ptr->message.u.securityModeCommand.u.
	   later_than_r3.criticalExtensions.u.r7.securityModeCommand_r7.ue_SystemSpecificSecurityCap)) == FALSE)
    {
      WRRC_MSG0_ERROR("SMC-IRAT Sec. Cap. Check failed");
        (void) rrc_transition_to_disconnected_state(RRC_PROCEDURE_SMC,RRC_TX_TO_DISCON_OTHER);
      return;
    }
  }  
  if((nv_ciphering_enabled) && (rrcsmc_int_ciph_info.ciphering_info_received == TRUE))
  {  
    /* Validate the SECURITY MODE COMMAND.  If not valid then return without 
        processing the message further. Send SMC failure*/
    if (RRCSMC_VALID_CONFIG != rrcsmc_validate_ciph_security_mode_command_r7(dcch_msg_ptr) )
    {
      WRRC_MSG0_ERROR("Ciphering SMC MSG VALIDATION FAILED");
      rrcsmc_send_security_mode_failure(rrcsmc_trans_id);          
      return;
    }
  }
  
  if ((nv_integrity_enabled)  &&
      (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
  {
    /* Validate the RRC SECURITY MODE COMMAND.  If validation not successful 
       then return without processing the message further. 
       Send the security mode failureis set to 0*/
    if (RRCSMC_VALID_CONFIG != rrcsmc_validate_int_security_mode_command_r7(dcch_msg_ptr) )
    {
      WRRC_MSG0_ERROR("Integrity SMC MSG VALIDATION FAILED");
      rrcsmc_send_security_mode_failure(rrcsmc_trans_id);          
      return;
    }
  
    if (dcch_msg_ptr->message.u.securityModeCommand.u.later_than_r3.
        criticalExtensions.u.r7.securityModeCommand_r7.integrityProtectionModeInfo.
        integrityProtectionModeCommand.t ==
          T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection)
    {
      domain_id = dcch_msg_ptr->message.u.securityModeCommand.u.
            later_than_r3.criticalExtensions.u.r7.securityModeCommand_r7.cn_DomainIdentity;
         
      /*if we have updated START value in IDT or CU and not have done authentication,
         then start_value_update_flag will be true. in this case, copy the start hfn_latest
         to the main database and initialize the integrity protection configuration*/
      if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id]
             == TRUE)
      {
        /*copy the latest start value sent in IDT or CU message*/
        WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
                     sizeof(byte) * RRC_MAX_HFN_LENGTH,
                     rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,   
                     RRC_MAX_START_LENGTH);
  
        rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;
      }
  
      /*initializes integrity_protection_info count_i values for ul and dl
         first time when we are starting integrity*/
      rrcsmc_init_integrity_protection_info();
    }
  
    /*check downlink integrity for the security mode command which 
       we just received. Only if integrity check passed for this message shall
       we continue further.  Otherwise just return and do not process this
      message*/
  
    if (rrcsmc_check_integrity_for_security_mode_command_r7(cmd_ptr) == FAILURE)
    {
      WRRC_MSG0_ERROR("Integrity Check for SMC cmd Failed");
      /*just return. Unsuccessful integrity check for security mode cmd*/
      return;
    }
    else
    {
      /* SMC will receive "RRC_DOWNLINK_SDU_IND" only for SecurityModeCommand */
      /*check if cell update is going on, if yes, then release the connection */
      /* if 'feature_rollback_security_config' is set to TRUE then UE will not
          reach this point */
      if (RRCCU_NOT_COMPLETED  == 
           rrccu_get_cell_update_complete_status(RRC_PROCEDURE_SMC))
      {
        WRRC_MSG0_HIGH("Cell Update is active. Ignoring SMC");
        return;          
      }
    }
  }/*if integrity protection info received and nv item is enabled*/
  
  /*save the contents of security mode command*/
  rrcsmc_save_contents_of_security_mode_command_r7(dcch_msg_ptr);
  if(nv_integrity_enabled &&
     rrcsmc_int_ciph_info.integrity_info_received)
  {
    /* Update all integrity related info. */
    rrcsmc_update_latest_cn_id();
    /* Integrity reconfiguration started */
    integrity_protection_info.reconfig = TRUE;
  }
  
  /*if feature is defined and nv item is turned on, then process the ciph
      parameters..*/
  if( (nv_ciphering_enabled) &&
      (rrcsmc_int_ciph_info.ciphering_info_received == TRUE))
  {
    /* Ciphering reconfig started. */
    ciphering_info.reconfig = TRUE;
  
    /*check if any reconfig is going on.  if it is go to wait state*/
    if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
         || (rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
       )
    {
      /*register for a cnf when reconfiguration is completed*/
      rrcllc_register_for_reconfig_cnf();
      WRRC_MSG0_HIGH("WAIT_FOR_RECONFIG_CNF state");
      /* Wait until Reconfig procedure is completed */
      rrcsmc_substate = RRCSMC_WAIT_FOR_RECONFIG_CNF;  
    }
    else
    {
      /*if cell update or reconfig is not currently in progress,
         send suspend req to rlc*/
      rrcsmc_perform_stop_or_suspend_functionality(RRCSMC_SMC_CMD);
    }
  }
  else if (nv_integrity_enabled &&
     rrcsmc_int_ciph_info.integrity_info_received)
  {
    /*check if any reconfig is going on.  if it is go to wait state*/
    if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
         || (rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
       )
    {
      /*register for a cnf when reconfiguration is completed*/
      rrcllc_register_for_reconfig_cnf();
      WRRC_MSG0_HIGH("WAIT_FOR_RECONFIG_CNF state");
      /* Wait until Reconfig procedure is completed */
      rrcsmc_substate = RRCSMC_WAIT_FOR_RECONFIG_CNF;  
    }
    else
    {
      /*if cell update or reconfig is not currently in progress,
         send suspend req to rlc*/
      rrcsmc_perform_stop_or_suspend_functionality(RRCSMC_SMC_CMD);
    }
  }
  else
  {
    /*ciphering is not present.. therefore integrity is present..
      just process the integrity message and return*/
    rrcsmc_process_int_security_mode_cmd_msg();
     
    /* Send SECURITY MODE COMPLETE command */
    rrcsmc_send_security_mode_complete(saved_security_mode_command_msg.
                                      transaction_id);
  }
}

/*===========================================================================

FUNCTION rrcsmc_process_smc_initial_substate

DESCRIPTION
   This function processes the SECURITY MODE COMMAND received on DL DCCH
   and extracts the Transaction ID.  Sends the SYNC_IND cmd to MM and
   also calls the function to send SECURITY MODE COMPLETE

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcsmc_process_smc_initial_substate
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;       /* Pointer to Downlink DCCH */

  uint8 message_choice;                         /* Local variable for message
                                                   choice */

  uint32 rrcsmc_trans_id = 0;                   /* local copy of trans id*/

  rrc_CN_DomainIdentity domain_id;              /* local copy of domain id*/

  rrc_cn_domain_identity_e_type cn_domain_id;   /*domain id                */
  boolean is_cn_domain_id_valid =FALSE;

  rrc_cmd_type *int_cmd_ptr;                    /* to send re-est srns failure */

  rrc_state_e_type rrc_state = rrc_get_state(); /* local var to store rrc state*/

  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    
    case RRC_HO_CIPH_REQ:
      /*we can do GtoW HO only in DCH state.. so if we r not in DCH state and
        receive this message.. flag error and return*/
      if(rrc_state != RRC_STATE_CELL_DCH)
      {
          WRRC_MSG0_ERROR("GtoW HO Ciph only in DCH");      
          rrcsmc_send_ho_ciph_config_cnf(FAILURE);
        return;
      }

        /*process the Ciphering request for GtoW Handover*/
      WRRC_MSG0_HIGH("Step 1 Ciph for GtoW HO");
      rrcsmc_process_step_1_ho_ciph_req(cmd_ptr->cmd.ho_ciph_req);

      break;

  case RRC_RE_EST_RLC_FOR_SRNS_REQ:
    //check if case is for re-est rb2 only, in this state non-rb-2 
    //re-establishment is not supported.
    if(cmd_ptr->cmd.rrc_re_est_srns_req.rb_type != RRC_RE_EST_RB_2)
    {
      //send cnf with failure
      int_cmd_ptr =  rrc_get_int_cmd_buf();
      /* Fill in the command parameters for RRC_RE_EST_RLC_FOR_SRNS_CNF */
      int_cmd_ptr->cmd_hdr.cmd_id = RRC_RE_EST_RLC_FOR_SRNS_CNF;
      int_cmd_ptr->cmd.rrc_re_est_srns_cnf.status = FAILURE;
      rrc_put_int_cmd(int_cmd_ptr);  

      WRRC_MSG0_HIGH("Send Re-Est Fail-Non-RB2");

    }
    else
    {
      saved_srns_info.rb_type = RRC_RE_EST_RB_2;
    }
    
    //set integrity reconfiguration to false
    integrity_protection_info.reconfig = FALSE;

    //commit the new ciphering algorithm
    if(saved_srns_info.ciph_algo_present == TRUE)
    {
      WRRC_MSG1_HIGH("At SRNS save Cipher Algo:%d[0=Invalid,1=UEA0,2=UEA1,4=UEA2] in rrcsmc_int_ciph_info",
                  saved_srns_info.ciph_algo);
      rrcsmc_int_ciph_info.saved_ciph_algo[RRC_CS_DOMAIN_CN_ID] = 
        rrcsmc_int_ciph_info.saved_ciph_algo[RRC_PS_DOMAIN_CN_ID] =
        saved_srns_info.ciph_algo;
    }

    //commit the new integrity algorithm
    if(saved_srns_info.integrity_algo_present == TRUE)
    {
      WRRC_MSG1_HIGH("AT SRNS save Integrity Algo = %x[0=Invalid,1=UIA1,2=UIA2] in rrcsmc_int_ciph_info",
                  saved_srns_info.integrity_algo);

      rrcsmc_int_ciph_info.saved_integrity_algo[RRC_CS_DOMAIN_CN_ID] = 
      rrcsmc_int_ciph_info.saved_integrity_algo[RRC_PS_DOMAIN_CN_ID] =
        saved_srns_info.integrity_algo;
    }

    /*set ciphering reconfig info to TRUE*/
    ciphering_info.reconfig = TRUE;
    
    //if re_est cnf was needed, set flag to true, else false
    if(cmd_ptr->cmd.rrc_re_est_srns_req.cnf_reqd == TRUE)
    {
      saved_srns_info.reconfig_cnf_req = TRUE;
      
      //save the procedure name to send cnf
      saved_srns_info.proc_type = cmd_ptr->cmd.rrc_re_est_srns_req.rrc_proc;
    }
    else
    {
      saved_srns_info.reconfig_cnf_req = FALSE;    
    }
    
    //send Stop req
    rrcsmc_perform_stop_or_suspend_functionality(RRCSMC_SRNS_CMD);
    break;

    case RRC_DOWNLINK_SDU_IND:
      /* SMC will receive "RRC_DOWNLINK_SDU_IND" only for SecurityModeCommand */
      /*check if cell update is going on, if yes, then ignore the Security Mode Procedure */
      if (RRCCU_NOT_COMPLETED  == 
             rrccu_get_cell_update_complete_status(RRC_PROCEDURE_SMC))
      {
        if (TRUE == feature_rollback_security_config)
        {
          WRRC_MSG0_HIGH("Ignoring Security Mode Command because cell update is in progress");
          return;
        }
      }

      /*if we get a downlink sdu ind in any other state then we should
       *report an error and not process it.
       */
      if ( (rrc_state != RRC_STATE_CELL_FACH )&&
           (rrc_state != RRC_STATE_CELL_DCH ) )
      {
        WRRC_MSG2_HIGH("Unsupported Event: %d for RRC in state: %d",
                 cmd_ptr->cmd_hdr.cmd_id, rrc_state); 
        return;
      }

      /* Set the protocol error reject to FALSE */
      rrc_protocol_error_reject = FALSE;

      /* Get the message type */    
      message_choice = 
      rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                   cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

      /* Check if the received message is Security Mode Command message */
      if ( message_choice != T_rrc_DL_DCCH_MessageType_securityModeCommand)
      {
/*lint -e793*/
        WRRC_MSG0_ERROR("Invalid msg rcvd SMC Init State");
/*lint +e793*/
        return;
      }

      WRRC_MSG0_HIGH("Received Security Mode Command Message");

      /* Get the OTA message pointer */
      dcch_msg_ptr = (rrc_DL_DCCH_Message *) 
                       cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

      if (dcch_msg_ptr == NULL) /* DCCH ASN1 decoding failed */
      {
        rrc_protocol_error_reject = TRUE ;

        /*send a security mode failure command if there is a protocol error
         *in the received security mode command
         */
        rrcsmc_failure_type = RRCSMC_PROTOCOL_ERROR; 
        rrcsmc_prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
        rrcsmc_send_security_mode_failure(rrcsmc_trans_id);
      }
      else /* ASN1 decoding is successful */
      {
        /* Get the transaction identified from the SECURITY MODE COMMAND and
           store it in local copy*/
        if(dcch_msg_ptr->message.u.securityModeCommand.t == 
                  T_rrc_SecurityModeCommand_r3)
        {           
          rrcsmc_trans_id = dcch_msg_ptr->message.u.securityModeCommand.u.
                          r3.securityModeCommand_r3.rrc_TransactionIdentifier;
        }
        else if(dcch_msg_ptr->message.u.securityModeCommand.t == 
                  T_rrc_SecurityModeCommand_later_than_r3)
        {
          rrcsmc_trans_id = dcch_msg_ptr->message.u.securityModeCommand.u.
                            later_than_r3.rrc_TransactionIdentifier;
        }
        else
        {
          rrcsmc_trans_id = 0;
        }
   
        /*check if any reconfig is going on.  if it is go to wait state*/
        if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
             || (rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
            )
        {
          if((rrcllc_get_ordered_config_status() == OC_SET_FOR_FACH_CELL_PCH_TRANS)||
            (rrcllc_get_ordered_config_status() == OC_SET_FOR_DCH_CELL_PCH_TRANS)||
            (rrcllc_get_ordered_config_status() == OC_SET_FOR_FACH_URA_PCH_TRANS)||
            (rrcllc_get_ordered_config_status() == OC_SET_FOR_DCH_URA_PCH_TRANS))
          {
            /*send a security mode failure command if there is a protocol error
             *in the received security mode command
             */
            WRRC_MSG0_HIGH("We are in middle of transition to PCH, send SM failure");
            rrcsmc_failure_type = RRCSMC_PROTOCOL_ERROR; 
            rrcsmc_prot_err_cause = rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState;
            rrcsmc_send_security_mode_failure(rrcsmc_trans_id);
            return;
          }

          if(dcch_msg_ptr->message.u.securityModeCommand.t == 
                       T_rrc_SecurityModeCommand_later_than_r3)
          {
            /*copy domain id*/
            if(dcch_msg_ptr->message.u.securityModeCommand.u.
                      later_than_r3.criticalExtensions.u.r7.securityModeCommand_r7.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain) 
            {
              cn_domain_id = RRC_CS_DOMAIN_CN_ID;
            }
            else
            {
              cn_domain_id = RRC_PS_DOMAIN_CN_ID;
            }
            is_cn_domain_id_valid = TRUE;
          }
          else
          {
            /*copy domain id*/
            if(dcch_msg_ptr->message.u.securityModeCommand.u.r3.securityModeCommand_r3.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain) 
            {
              cn_domain_id = RRC_CS_DOMAIN_CN_ID;
            }
            else
            {
              cn_domain_id = RRC_PS_DOMAIN_CN_ID;
            }
            is_cn_domain_id_valid = TRUE;
          }
          if((is_cn_domain_id_valid == TRUE)&&
            (rrcsmc_int_ciph_info.latest_config_cn_id != cn_domain_id)&&
              (rrcsmc_get_security_start_status() == TRUE))
          {
            /*send a security mode failure command if there is a protocol error
             *in the received security mode command
             */
            WRRC_MSG0_HIGH("We are in middle of Reconfig send SM failure, so that NW can retry");
            /* Set the failure cause and send the failure message */
            rrcsmc_failure_type = RRCSMC_INCOMPATIBLE_SIMULTANEOUS_RECONFIG; 
            rrcsmc_send_security_mode_failure(rrcsmc_trans_id);          
            return;
          }
        }

        /* ASN1 decoding passed, hence take a back up of security variables because 
           some of the variables are updated while validating the SMC message */
        rrcsmc_backup_global_security_variables();
        if(dcch_msg_ptr->message.u.securityModeCommand.t == 
                  T_rrc_SecurityModeCommand_later_than_r3)
        {
          if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
          {
            if(dcch_msg_ptr->message.u.securityModeCommand.u.later_than_r3.
              criticalExtensions.t == T_rrc_SecurityModeCommand_criticalExtensions_1_r7)
            {
              /*Processing for R7 security mode command message*/
              rrcsmc_process_security_mode_command_r7(dcch_msg_ptr,cmd_ptr);
              
            }
            else
            {
              rrc_print_supported_asn1_rel_version();
              WRRC_MSG0_ERROR("Invalid non critical ext for Security mode Command message");
              rrcsmc_failure_type = RRCSMC_PROTOCOL_ERROR;
              rrcsmc_prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
              rrcsmc_send_security_mode_failure(dcch_msg_ptr->message.u.
                securityModeCommand.u.later_than_r3.rrc_TransactionIdentifier);
            }
          }
          else
          {
            rrc_print_supported_asn1_rel_version();
            WRRC_MSG0_ERROR("nv_rel_ind is non Rel7 ,so cant support R7 SMC  Send SM failure");
            rrcsmc_failure_type = RRCSMC_PROTOCOL_ERROR;
            rrcsmc_prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
            rrcsmc_send_security_mode_failure(dcch_msg_ptr->message.u.
              securityModeCommand.u.later_than_r3.rrc_TransactionIdentifier);
          }
          return;
        }
        else
        {
          /*Processing for R3 security mode command message*/
      

        /*check to see that no critical extensions are added and no reconfig
          is currently being done*/
        if (RRCSMC_VALID_CONFIG != rrcsmc_validate_security_mode_command(dcch_msg_ptr) )
        {
          WRRC_MSG0_ERROR("SMC MSG VALIDATION FAILED");
          rrcsmc_send_security_mode_failure(rrcsmc_trans_id);          
          return;
        }

        /*check if WCDMA security capablities are correct with UECI procedure*/
        if(rrcueci_verify_security_capability((&dcch_msg_ptr->message.u.securityModeCommand.u.r3.
                                                securityModeCommand_r3.securityCapability)) == FALSE)
        {
          WRRC_MSG0_ERROR("SMC Sec. Cap. Check failed");
          (void)rrc_transition_to_disconnected_state(RRC_PROCEDURE_SMC,RRC_TX_TO_DISCON_OTHER);
          return;
        }

        /*if interrat security capabilities are present in the message then check its's
          validity with UECI procedure*/
        if(dcch_msg_ptr->message.u.securityModeCommand.u.r3.securityModeCommand_r3.
              m.ue_SystemSpecificSecurityCapPresent)
          {
            if(rrcueci_verify_inter_rat_security_capability(
              ( (struct rrc_InterRAT_UE_SecurityCapList *) 
                &(dcch_msg_ptr->message.u.securityModeCommand.u.r3.
                securityModeCommand_r3.ue_SystemSpecificSecurityCap))) == FALSE)
            {
              WRRC_MSG0_ERROR("SMC-IRAT Sec. Cap. Check failed");
              (void) rrc_transition_to_disconnected_state(RRC_PROCEDURE_SMC,RRC_TX_TO_DISCON_OTHER);
              return;
            }
          }

        if( (nv_ciphering_enabled) &&
            (rrcsmc_int_ciph_info.ciphering_info_received == TRUE))
        {
          /* Validate the SECURITY MODE COMMAND.  If not valid then return without 
             processing the message further. Send SMC failure*/
          if (RRCSMC_VALID_CONFIG != rrcsmc_validate_ciph_security_mode_command(dcch_msg_ptr) )
          {
            WRRC_MSG0_ERROR("Ciphering SMC MSG VALIDATION FAILED");
            rrcsmc_send_security_mode_failure(rrcsmc_trans_id);          
            return;
          }
        }

        if ((nv_integrity_enabled)  &&
            (rrcsmc_int_ciph_info.integrity_info_received == TRUE))
        {

          /* Validate the RRC SECURITY MODE COMMAND.  If validation not successful 
             then return without processing the message further. 
             Send the security mode failureis set to 0*/
          if (RRCSMC_VALID_CONFIG != rrcsmc_validate_int_security_mode_command(dcch_msg_ptr) )
          {
            WRRC_MSG0_ERROR("Integrity SMC MSG VALIDATION FAILED");
            rrcsmc_send_security_mode_failure(rrcsmc_trans_id);          
            return;
          }

          if (dcch_msg_ptr->message.u.securityModeCommand.u.r3.securityModeCommand_r3.
              integrityProtectionModeInfo.integrityProtectionModeCommand.t ==
              T_rrc_IntegrityProtectionModeCommand_startIntegrityProtection)
          {
            domain_id = dcch_msg_ptr->message.u.securityModeCommand.u.r3.
              securityModeCommand_r3.cn_DomainIdentity;
            
            /*if we have updated START value in IDT or CU and not have done authentication,
              then start_value_update_flag will be true. in this case, copy the start hfn_latest
              to the main database and initialize the integrity protection configuration*/
            if(rrcsmc_int_ciph_info.start_value_update_flag[domain_id]
               == TRUE)
            {
              /*copy the latest start value sent in IDT or CU message*/
              WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn,
                           RRC_MAX_HFN_LENGTH * sizeof(byte),
                           rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest,      
                           RRC_MAX_START_LENGTH);

              rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;
            }

            /*initializes integrity_protection_info count_i values for ul and dl
              first time when we are starting integrity*/
            rrcsmc_init_integrity_protection_info();
          }

          /*check downlink integrity for the security mode command which 
            we just received. Only if integrity check passed for this message shall
            we continue further.  Otherwise just return and do not process this
            message*/

          if (rrcsmc_check_integrity_for_security_mode_command(cmd_ptr) == FAILURE)
          {
            WRRC_MSG0_ERROR("Integrity Check for SMC cmd Failed");
            /*just return. Unsuccessful integrity check for security mode cmd*/
            return;
          }
          else
          {
            /* SMC will receive "RRC_DOWNLINK_SDU_IND" only for SecurityModeCommand */
            /*check if cell update is going on, if yes, then release the connection */
            /* if 'feature_rollback_security_config' is set to TRUE then UE will not
               reach this point */
            if (RRCCU_NOT_COMPLETED  == 
               rrccu_get_cell_update_complete_status(RRC_PROCEDURE_SMC))
            {
              WRRC_MSG0_HIGH("Cell Update is active. Ignoring SMC");
              return;          
            }
          }
        }/*if integrity protection info received and nv item is enabled*/

        if ((nv_ciphering_enabled) || (nv_integrity_enabled))
        {
          /*save the contents of security mode command*/
          rrcsmc_save_contents_of_security_mode_command(dcch_msg_ptr);
          if(nv_integrity_enabled &&
             rrcsmc_int_ciph_info.integrity_info_received)
          {
            /* Update all integrity related info. */
            rrcsmc_update_latest_cn_id();
            /* Integrity reconfiguration started */
            integrity_protection_info.reconfig = TRUE;
          }

          /*if feature is defined and nv item is turned on, then process the ciph
            parameters..*/
          if( (nv_ciphering_enabled) &&
              (rrcsmc_int_ciph_info.ciphering_info_received == TRUE))
          {
            /* Ciphering reconfig started. */
            ciphering_info.reconfig = TRUE;

            /*check if any reconfig is going on.  if it is go to wait state*/
            if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
                || (rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
               )
            {
              /*register for a cnf when reconfiguration is completed*/
              rrcllc_register_for_reconfig_cnf();
              WRRC_MSG0_HIGH("WAIT_FOR_RECONFIG_CNF state");
              /* Wait until Reconfig procedure is completed */
              rrcsmc_substate = RRCSMC_WAIT_FOR_RECONFIG_CNF;  
            }
            else
            {
              /*if cell update or reconfig is not currently in progress,
                send suspend req to rlc*/
              rrcsmc_perform_stop_or_suspend_functionality(RRCSMC_SMC_CMD);
            }
          }
          else if (nv_integrity_enabled &&
             rrcsmc_int_ciph_info.integrity_info_received)
          {
            /*check if any reconfig is going on.  if it is go to wait state*/
            if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
                || (rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
)
            {
              /*register for a cnf when reconfiguration is completed*/
              rrcllc_register_for_reconfig_cnf();
              WRRC_MSG0_HIGH("WAIT_FOR_RECONFIG_CNF state");
              /* Wait until Reconfig procedure is completed */
              rrcsmc_substate = RRCSMC_WAIT_FOR_RECONFIG_CNF;  
            }
            else
            {
              /*if cell update or reconfig is not currently in progress,
                 send suspend req to rlc*/
              rrcsmc_perform_stop_or_suspend_functionality(RRCSMC_SMC_CMD);
            }
          }
          else
          {
            /*ciphering is not present.. therefore integrity is present..
            just process the integrity message and return*/
            rrcsmc_process_int_security_mode_cmd_msg();
          
            /* Send SECURITY MODE COMPLETE command */
            rrcsmc_send_security_mode_complete(saved_security_mode_command_msg.
                                               transaction_id);
          }
        }

        if (!(nv_ciphering_enabled) && 
            !(nv_integrity_enabled))
        {
          /*set the rrcsmc_int_ciph_info.latest_config_cn_id to the domain_id for which we 
            received security mode command*/
          if (dcch_msg_ptr->message.u.securityModeCommand.
              u.r3.securityModeCommand_r3.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
          {
            rrcsmc_int_ciph_info.latest_config_cn_id =  RRC_CS_DOMAIN_CN_ID;
          }
          else if ( dcch_msg_ptr->message.u.securityModeCommand.u.r3.
                    securityModeCommand_r3.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
          {
            rrcsmc_int_ciph_info.latest_config_cn_id = RRC_PS_DOMAIN_CN_ID;
          }

          /* Send SECURITY MODE COMPLETE command */
          rrcsmc_send_security_mode_complete(rrcsmc_trans_id);

        }
        }
      }/*case DOWNLINK_SDU_IND*/
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for SMC in substate:%d", 
               cmd_ptr->cmd_hdr.cmd_id, rrcsmc_substate);
      break;
  }
} /* rrcsmc_process_smc_initial_substate */
/*===========================================================================

FUNCTION rrcsmc_process_cell_dch_state

DESCRIPTION
  This procedure handles smc procedure in CELL_DCH state

  Checks if SMC procedure is in Initial state or in Wait_for_l2_ack state
  and calls those functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcsmc_process_cell_dch_state
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  switch (rrcsmc_substate)
  {
    case RRCSMC_INITIAL: 
      rrcsmc_process_smc_initial_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_FOR_L2_ACK:
      rrcsmc_process_wait_for_l2_ack_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_FOR_RECONFIG_CNF:
      rrcsmc_process_wait_for_reconfig_cnf_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_FOR_CIPH_CONFIG_CNF:
      rrcsmc_process_wait_for_ciph_config_cnf_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_FOR_HO_STEP_1_CIPH_CONFIG_CNF:
      rrcsmc_process_wait_for_step_1_ciph_config_cnf_substate(cmd_ptr);
      break;    

    case RRCSMC_WAIT_RE_EST_SRNS_CNF:
      rrcsmc_process_wait_re_est_srns_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_SRNS_RELOC_L2_ACK_IND:
      rrcsmc_process_wait_srns_reloc_l2_ack_ind_substate(cmd_ptr);
      break;  
  
    default:     
      WRRC_MSG1_ERROR("Invalid substate in CELL_DCH state: %d",
               rrcsmc_substate);
      break;
  }  
}/*rrcsmc_process_cell_dch_state*/
/*===========================================================================

FUNCTION rrcsmc_process_cell_fach_state

DESCRIPTION
  This procedure handles smc procedure in CELL_FACH state

  Checks if SMC procedure is in Initial state or in Wait_for_l2_ack state
  and calls those functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcsmc_process_cell_fach_state
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  switch (rrcsmc_substate)
  {
    case RRCSMC_INITIAL: 
      rrcsmc_process_smc_initial_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_FOR_L2_ACK:
      rrcsmc_process_wait_for_l2_ack_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_FOR_RECONFIG_CNF:
      rrcsmc_process_wait_for_reconfig_cnf_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_FOR_CIPH_CONFIG_CNF:
      rrcsmc_process_wait_for_ciph_config_cnf_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_RE_EST_SRNS_CNF:
      rrcsmc_process_wait_re_est_srns_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_SRNS_RELOC_L2_ACK_IND:
      rrcsmc_process_wait_srns_reloc_l2_ack_ind_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_TILL_STANDBY_ENDS:
      rrcsmc_process_wait_till_standby_ends_substate(cmd_ptr);
      break;
			
    default:     
      WRRC_MSG1_ERROR("Invalid substate in CELL_FACH state: %d",
               rrcsmc_substate);
      break;
  }  
}/*rrcsmc_process_cell_fach_state*/
/*===========================================================================

FUNCTION rrcsmc_process_pch_state

DESCRIPTION
  This procedure handles smc procedure in CELL_PCH state

  Checks if SMC procedure is in Initial state and calls the appropriate
  function.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcsmc_process_pch_state
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  switch (rrcsmc_substate)
  {
    case RRCSMC_INITIAL: 
      rrcsmc_process_smc_initial_substate(cmd_ptr);
      break;

    case RRCSMC_WAIT_RE_EST_SRNS_CNF:
      rrcsmc_process_wait_re_est_srns_substate(cmd_ptr);
      break;

    default:     
      WRRC_MSG1_ERROR("Invalid substate in CELL_PCH state: %d",
               rrcsmc_substate);
      break;
  }  
}/*rrcsmc_process_pch_state*/

/*===========================================================================

FUNCTION rrcsmc_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for SECURITY MODE COMMAND
  The following events are handled by this procedure:

  RRC_DOWNLINK_SDU_IND Primitive (received from RLC) containing Security
      Mode Command message.
  RRC_STATE_CHANGE_IND Primitive informs transition from Connected mode to
    Idle Disconnected state.
  RRC_DOWNLINK_L2ACK_IND Primitive indicates success or failure of 
    transmission of Security Mode Complete Msg.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcsmc_procedure_event_handler
(
rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
  rrc_state_e_type rrc_state;                   /* Local variable to store the rrc state */
  rrc_DL_DCCH_Message *dcch_msg_ptr = NULL;     /* Pointer to Downlink DCCH */
  uint8 message_choice;                         /* Local variable for message choice */
  uint32 rrcsmc_trans_id = 0;                   /* Local copy of trans id */

  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_SMC,rrcsmc_substate,cmd_ptr->cmd_hdr.cmd_id);
  
  //process state_change_ind right here just to be sure that we don't
  //miss going to idle at any cost.. only for HO step 1 do additional
  //step, else for all other states, send SMC to initial substate
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_STATE_CHANGE_IND)
  {
    WRRC_MSG1_HIGH("Rexd State ChangeInd to %d RRC state", cmd_ptr->cmd.state_change_ind.new_state);
    switch(cmd_ptr->cmd.state_change_ind.new_state)
    {
      case RRC_STATE_DISCONNECTED:
        //if we r waiting for HO Step 1 cnf, send failure
        if(rrcsmc_substate == RRCSMC_WAIT_FOR_HO_STEP_1_CIPH_CONFIG_CNF)
        {
          rrcsmc_send_ho_ciph_config_cnf(FAILURE);
        }
    
        rrcsmc_go_to_smc_initial_substate();
        MSG_MED("SMC going to INITIAL_STATE",0,0,0);
      break;
   
      case RRC_STATE_CONNECTING:
        /*On receiving state change to connecting check & update any keys 
         *received in other-RAT right here
         */
        rrcsmc_check_and_update_security_key_set_gen_in_other_rat(RRC_CS_DOMAIN_CN_ID);
      break;
   
      default :
        WRRC_MSG1_ERROR("RRC_STATE_CHANGE_IND to RRC state %d not handled",
                      cmd_ptr->cmd.state_change_ind.new_state);
      break;
    }
  }
  //If we get SMC in a non-initial substate send a security
  //mode failure because we are in the process of configuring
  else if( (rrcsmc_substate != RRCSMC_INITIAL) &&
           (cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_SDU_IND) )
  {
    /* Get the message type */    
    message_choice = rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                                    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

    /* Check if the received message is Security Mode Command message */
    if ( message_choice != T_rrc_DL_DCCH_MessageType_securityModeCommand )
    {
      WRRC_MSG0_ERROR("Invalid msg rcvd in SMC in non-initial substate");
      return;
    }
    WRRC_MSG0_HIGH("Received Security Mode Command Message in non-initial substate");

    /* Get the OTA message pointer */
    dcch_msg_ptr = (rrc_DL_DCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

    if (dcch_msg_ptr == NULL) /* DCCH ASN1 decoding failed */
    {
      rrc_protocol_error_reject = TRUE ;

      /* Send a security mode failure command if there is a protocol error
       * in the received security mode command.Note that the trans id would be 
       * zero here which is fine because the network would ignore it for ASN1
       * decoding error cause
       */
      rrcsmc_failure_type = RRCSMC_PROTOCOL_ERROR; 
      rrcsmc_prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
      rrcsmc_send_security_mode_failure(rrcsmc_trans_id);
    }
    else /* ASN1 decoding is successful */
    {
      /* Extract the transaction id */
      if(dcch_msg_ptr->message.u.securityModeCommand.t == 
                T_rrc_SecurityModeCommand_r3)
      {           
        rrcsmc_trans_id = dcch_msg_ptr->message.u.securityModeCommand.u.r3.securityModeCommand_r3.rrc_TransactionIdentifier;
      }
      else if(dcch_msg_ptr->message.u.securityModeCommand.t == 
                T_rrc_SecurityModeCommand_later_than_r3)
      {
        rrcsmc_trans_id = dcch_msg_ptr->message.u.securityModeCommand.u.later_than_r3.rrc_TransactionIdentifier;
      }
      else
      {
        rrcsmc_trans_id = 0;
      }
      /* Set the failure cause and send the failure message */
      rrcsmc_failure_type = RRCSMC_INCOMPATIBLE_SIMULTANEOUS_RECONFIG; 
      rrcsmc_send_security_mode_failure(rrcsmc_trans_id);          
    }
  }
  else
  {
/*lint -e408*/
      if ((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RR_CIPHER_SYNC_IND)
/*lint +e408*/
        /*On receiving RR_CIPHER_SYNC_IND from GSM-RR check & update any keys 
         *received in other-RAT right here
         */
      {
        WRRC_MSG0_HIGH("Rexd RR_CIPHER_SYNC_IND");
        rrcsmc_check_and_update_security_key_set_gen_in_other_rat(RRC_CS_DOMAIN_CN_ID);
      }

      else
      {
        //get the rrc state
        rrc_state = rrc_get_state();
    
        switch (rrc_state)
        {
          case RRC_STATE_CELL_DCH:  
            /* CELL_FACH and CELL_DCH only valid states to receive 
               SECURITY MODE COMMAND message */
            rrcsmc_process_cell_dch_state(cmd_ptr); 
            break;
      
          case RRC_STATE_CELL_FACH:    
            /* CELL_FACH and CELL_DCH only valid states to receive 
               SECURITY MODE COMMAND message */
            rrcsmc_process_cell_fach_state(cmd_ptr); 
            break;
      
          case RRC_STATE_CELL_PCH:
          case RRC_STATE_URA_PCH:
            rrcsmc_process_pch_state(cmd_ptr); 
            break;
      
          case RRC_STATE_DISCONNECTED:
          case RRC_STATE_CONNECTING:
            /* SECURITY MODE COMMAND is valid only in CELL_DCH and CELL_FACH state */
            MSG_MED("RRCSMC Msg received in Invalid State: %d",
                     rrc_state,0,0); 
            break;
      
          default: 
            /* This should never happen */
            ERR_FATAL("RRCSMC Msg received in Invalid State: %d",rrc_state,0,0);
            break;
        }  
      }
  }
} /* rrcsmc_procedure_event_handler */
/*===========================================================================

FUNCTION rrcsmc_get_nv_items

DESCRIPTION
  This  function gets NV  item NV_RC_INTEGRITY_ENABLED  and 
  NV_RRC_CIPHERING_ENABLED and stores it in local variables
  used to determine if Integrity/Ciphering are supported or
  not

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcsmc_get_nv_items
(
  sys_modem_as_id_e_type wrrc_as_id
)
{

  /*read integrity and ciphering ENABLE/DISABLE flags from NV*/
  wnv_get_nv_msim(WNV_ID_RRC_INTEGRITY_ENABLED_I, &nv_integrity_enabled, sizeof(nv_integrity_enabled),wrrc_as_id);
  WRRC_MSG1_HIGH("NV_INTEGRITY_ENABLED is %d", nv_integrity_enabled);

  wnv_get_nv_msim(WNV_ID_RRC_CIPHERING_ENABLED_I, &nv_ciphering_enabled_status, sizeof(nv_ciphering_enabled_status),wrrc_as_id);
  WRRC_MSG1_HIGH("NV_CIPHERING_ENABLED is %d", nv_ciphering_enabled_status);

  /*set nv_ciphering_enabled to 1, as most of the code 
    is under this flag, We have to support UEA0 even if this Flag is set OFF */
  nv_ciphering_enabled = WNV_GET_DEFAULT(WNV_ID_RRC_CIPHERING_ENABLED_I);

}

/*===========================================================================

FUNCTION rrcsmc_init_procedure

DESCRIPTION
  This function initializes required parameters for SECURITY MODE COMMAND
  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcsmc_init_procedure
(
void
)
{
  rrcsmc_go_to_smc_initial_substate();
  
  /*reset START values to 0's at power-up, we may not receive a SIM insert
    REQ sometimes if SIM is not present, needed for an emergency call*/
  rrcsmc_init_start_value_at_power_up();

  /* register for state change ind when UE goes to idle disconnected*/
  rrcscmgr_register_for_scn(RRC_PROCEDURE_SMC,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_DISCONNECTED);     

  /* register for state change ind when UE goes to CONNECTING*/
  rrcscmgr_register_for_scn(RRC_PROCEDURE_SMC,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_CONNECTING);     
} /* rrcsmc_init_procedure */
/*===========================================================================

FUNCTION rrcsmc_process_integrity_protection_mode_info

DESCRIPTION
  This procedure process Integrity Protection Mode Info.  It is called by 
  the foll. procedures.  RRCSMC will copy the IntegrityProtectionModeInfo and
  processes them.If IntegrityProtectionModeInfo indicates to start Integrity
  Potection, it will update the necessary paramters and start the Integrity.
  Otherwise it will declares invalid configuration.
  
  UMI, Cell Update, Cell Update Confirm, URA Update Confirm, Physical Channel 
  Reconfiguration, Radio Bearer Setup, Radio Bearer Reconfiguration  
  
  The above procedures will call this function in SMC to notify the SMC
  procedure of the Integrity Protection Info and start Integrity Protection.


DEPENDENCIES
  None.

RETURN VALUE
  Returns RRCSMC_VALID_CONFIG if IntegrityProtectionModeInfo is 
  valid(Start only).
  Returns RRCSMC_INVALID_CONFIG if IntegrityProtectionModeInfo is invalid(Modify)

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
rrcsmc_config_status_e_type rrcsmc_process_integrity_protection_mode_info
(
struct rrc_IntegrityProtectionModeInfo* integrityProtModeInfo
)
{
  /*Template for future use when we support SRNS relocation*/
  return RRCSMC_INVALID_CONFIGURATION;
}/*rrcsmc_process_integrity_protection_mode_info*/
/*lint +e715*/
/*===========================================================================

FUNCTION rrcsmc_reset_smc_config_flag

DESCRIPTION
  This procedure resets the integrity_protection_info.reconfig FLAG.
  This is called by UMI, CU, RB, ASU procedures to indicate that they
  received an L2 Ack for their complete message which included the
  UL Activation Info Time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_reset_smc_config_flag
(
void
)
{
  /*Template for future use when we support SRNS relocation*/

}/*rrcsmc_reset_smc_config_flag*/
/*===========================================================================

FUNCTION rrcsmc_append_ul_integrity_prot_act_info

DESCRIPTION
  This procedure populates the UL Activation Info times into the ptr passed
  to it by the foll. procedures.
  
  UMI, Cell Update, Cell Update Confirm, URA Update Confirm, Physical Channel 
  Reconfiguration, Radio Bearer Setup, Radio Bearer Reconfiguration  
  
  The above procedures will call this function in SMC to get the UL Activation 
  Info while sending a Complete message.  The UL Activation Info will be used 
  by the UTRAN to start integrity protection on the Uplink.  



DEPENDENCIES
  None.

RETURN VALUE
  Returns SUCCESS if UL Activation Info is successfully populated.
  Returns FAILURE if UL Activation Info is not populated.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
uecomdef_status_e_type rrcsmc_append_ul_integrity_prot_act_info 
(
struct rrc_IntegrityProtActivationInfo * ulActivationInfo
)
{

  /*Template for future use when we support SRNS relocation*/
  return SUCCESS;
}/*rrcsmc_append_ul_integrity_prot_act_info*/
/*lint +e715*/
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU

/*===========================================================================

FUNCTION rrcsmc_is_cn_associated_with_any_est_rab

DESCRIPTION
  This function checks whether a given CN domain is 
  associated with any established RAB
DEPENDENCIES
  None

RETURN VALUE
  TRUE :If given CN comain I.D is associated with EST_RAB.
  FALSE:Otherwise

SIDE EFFECTS
  None
===========================================================================*/
static boolean rrcsmc_is_cn_associated_with_any_est_rab
(
  rrc_cn_domain_identity_e_type cn_domain_id
)
{
  uint32 rab_count;
  boolean status=FALSE;

  /*Get the Cn domain associated with first RAB which uses re_est_timer*/
  for (rab_count=0; rab_count<MAX_RAB_TO_SETUP; rab_count++)
  {
    if(rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0)
    {
      if(cn_domain_id == rrc_est_rabs.rabs[rab_count].cn_domain)
      {
        WRRC_MSG2_HIGH("RAB I.D = %d associated with CN I.D = %d",rrc_est_rabs.rabs[rab_count].rab_id,cn_domain_id);
        status = TRUE;
        break;
      }
    }
  }
  WRRC_MSG2_HIGH("Cn i.d=%d, is_cn_associated_with_any_est_rab = %d",cn_domain_id,status);
  return status;
}

/*===========================================================================

FUNCTION rrcsmc_append_start_list_per_active_domain

DESCRIPTION
  This function initializes the START_LIST per active cn domain
  with the latest values to be sent to the UTRAN by CU  procedure.
  Currently, functionality under flag FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
  gets enabled only if the GCF NV is set.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns SUCCESS if STARTlist is appended.
  This function returns FAILURE if STARTlist is not appended.

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_start_list_per_active_domain 
(
rrc_STARTList *start_list_ext,
rrcsmc_append_start_e_type start_type
)
{
  uint8 both_cn_domain_open = FALSE;
  rrc_cn_domain_identity_e_type active_cn_domain;
  uecomdef_status_e_type status;
  boolean is_cs_domain_open,is_ps_domain_open;
  rrc_proc_e_type                 proc_id;
  rrcllc_oc_process_state_e_type  process_state;  
  rrcllc_oc_set_status_e_type     oc_status;
  byte start_value[RRC_MAX_START_LENGTH];
  active_cn_domain = RRC_CS_DOMAIN_CN_ID;
  is_cs_domain_open = FALSE;
  is_ps_domain_open = FALSE;

  /*If the GCF flag is not set for this feature, we can simply consider both domains to be open*/
  
  oc_status = rrcllc_get_ordered_config_state_and_proc(&proc_id, &process_state);

  if(!nv_gcf_test_flag)
  {
    if((TRUE == rrcllc_reconfig_in_progress()) &&
       (oc_status != OC_NOT_SET) &&
       (proc_id == RRC_PROCEDURE_RBE) &&
       (RRCRB_WAIT_FOR_CELL_UPDATE_CNF == rrcrbe_get_current_sub_state()) &&
       (TRUE == rrcsmc_get_start_value_to_transmit_flag_status()))
    {
      if(TRUE == rrcsmc_get_calc_start_value(start_value))
      {
        status = rrcsmc_append_already_computed_start_value_to_start_list(start_list_ext,
                                                       start_value, 
                                                       start_type,
                                                       TRUE,
                                                       RRC_CS_DOMAIN_CN_ID);
      }
      else
      {
        status =  rrcsmc_append_start_value_to_start_list (start_list_ext,
                                                       start_type,
                                                       TRUE,
                                                       RRC_CS_DOMAIN_CN_ID);
      }
    }
    else
    {
      status =  rrcsmc_append_start_value_to_start_list (start_list_ext,
                                                       start_type,
                                                       TRUE,
                                                       RRC_CS_DOMAIN_CN_ID);
    }
    return status;
  }
  
  /*Check if the variable "LATEST_CONFIGURED_CN_DOMAIN" has been initialised*/
  if(rrcsmc_get_security_start_status() == TRUE)
  {
    /*Check if CS domains is in EST_RAB */
    if(rrcsmc_is_cn_associated_with_any_est_rab(RRC_CS_DOMAIN_CN_ID) == TRUE)
    {
      is_cs_domain_open = TRUE;
    }
    else
    {
      if(rrcsmc_int_ciph_info.latest_config_cn_id == RRC_CS_DOMAIN_CN_ID)
      {
        is_cs_domain_open = TRUE;
      }
      else
      {
        is_cs_domain_open = FALSE;
      }
    }
  
    /*Check if PS domains is in EST_RAB */
    if(rrcsmc_is_cn_associated_with_any_est_rab(RRC_PS_DOMAIN_CN_ID) == TRUE)
    {
      is_ps_domain_open = TRUE;
    }
    else
    {
      if(rrcsmc_int_ciph_info.latest_config_cn_id == RRC_PS_DOMAIN_CN_ID)
      {
        is_ps_domain_open = TRUE;
      }
      else
      {
        is_ps_domain_open = FALSE;
      }
    }
  }
  else
  {
    /*latest_config_cn_id is not init, so include START value for CS domain 
     *as spec say include the START value for either the CS domain or 
     *the PS domain
     */
    is_cs_domain_open = TRUE;
    is_ps_domain_open = FALSE;
  }
  MSG_HIGH("Domain status, CS=%d,PS=%d,Latest cn i.d=%d",is_cs_domain_open,
                          is_ps_domain_open,rrcsmc_int_ciph_info.latest_config_cn_id);

  if((is_cs_domain_open == FALSE)||(is_ps_domain_open == FALSE))
  {
    if(is_cs_domain_open == FALSE)
    {
      active_cn_domain = RRC_PS_DOMAIN_CN_ID;
    }
    else
    {
      active_cn_domain = RRC_CS_DOMAIN_CN_ID;
    }
  }
  else
  {
    both_cn_domain_open = TRUE;
  }

  if((TRUE == rrcllc_reconfig_in_progress()) &&
     (oc_status != OC_NOT_SET) &&
     (proc_id == RRC_PROCEDURE_RBE) &&
     (RRCRB_WAIT_FOR_CELL_UPDATE_CNF == rrcrbe_get_current_sub_state()) &&     
     (TRUE == rrcsmc_get_start_value_to_transmit_flag_status()))
  {
    if(TRUE == rrcsmc_get_calc_start_value(start_value))
    {
      status = rrcsmc_append_already_computed_start_value_to_start_list(start_list_ext,
                                                       start_value, 
                                                       start_type,
                                                       both_cn_domain_open,
                                                       active_cn_domain);
    }
    else
    {
      status =  rrcsmc_append_start_value_to_start_list (start_list_ext,
                                                       start_type,
                                                       both_cn_domain_open,
                                                       active_cn_domain);
    }
  }
  else
  {
    status =  rrcsmc_append_start_value_to_start_list (start_list_ext,
                                                       start_type,
                                                       both_cn_domain_open,
                                                       active_cn_domain);
  }

  return status;
}
#endif
/*===========================================================================

FUNCTION rrcsmc_append_start_value_to_start_list

DESCRIPTION
  This function initializes the START_LIST with the latest values to 
  be sent to the UTRAN by the RCE, CU  and ASU procedures. The START
  values for all CN domains for which we have received Integrity
  Protection Info will be sent.
  
  RCE, CU and ASU procedures will call this function in SMC.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns SUCCESS if STARTlist is appended.
  This function returns FAILURE if STARTlist is not appended.

SIDE EFFECTS
  None

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_start_value_to_start_list 
(
 rrc_STARTList *start_list,
rrcsmc_append_start_e_type start_type
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
,boolean both_cn_domain_open
,rrc_cn_domain_identity_e_type active_cn_domain
#endif
)
{
  uint32 i, index;                          /* local var to store count*/

  /*local var to keep track of cn_domain*/
  rrc_cn_domain_identity_e_type domain_id; 
  
  if(both_cn_domain_open == TRUE)
  {
    start_list->n = rrcsmc_int_ciph_info.num_cn_domains;
  }
  else
  {
    start_list->n = 1;
  }
  ALLOC_ASN1ARRAY2(&enc_ctxt, start_list, rrc_STARTSingle);

  WRRC_MSG2_HIGH("both_cn_domain_open:%d, active_cn_domain:%d",both_cn_domain_open,active_cn_domain);

  index = 0;

  /*copy the contents of cn_domain into int_ciph_info struct*/
  for (i=0; i<rrcsmc_int_ciph_info.num_cn_domains; i++)
  {
    domain_id = rrcsmc_int_ciph_info.int_ciph_info[i].domain_id;
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
    if(both_cn_domain_open == FALSE)
    {
      if(active_cn_domain != domain_id)
      {
        continue;
      }
    }
#endif
    if (domain_id == RRC_CS_DOMAIN_CN_ID)
    {
      /* Update START values for CS domain */
      start_list->elem[index].cn_DomainIdentity = rrc_CN_DomainIdentity_cs_domain;
    }
    else
    {
      /* Update START values for CS domain */
      start_list->elem[index].cn_DomainIdentity = rrc_CN_DomainIdentity_ps_domain;
    }

    start_list->elem[index].start_Value.numbits =  RRC_START_VALUE_LENGTH;

    //append start value for this domain
    rrcsmc_append_start_per_domain ((byte *) (start_list->elem[index].start_Value.data),
                                    domain_id,
                                    start_type);

    if(i == (uint32) RRC_CS_DOMAIN_CN_ID)
    {
      MSG_HIGH("Appended CS-START: 0x%x 0x%x 0x%x",start_list->elem[index].start_Value.data[0],
               start_list->elem[index].start_Value.data[1],
               start_list->elem[index].start_Value.data[2]);
    }
    else
    {
      MSG_HIGH("Appended PS-START: 0x%x 0x%x 0x%x", start_list->elem[index].start_Value.data[0],
               start_list->elem[index].start_Value.data[1],
               start_list->elem[index].start_Value.data[2]);
    }
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
    if(both_cn_domain_open == FALSE ) 
    {
      break;
    }
#endif

    index++;
  }
  return SUCCESS;
}

/*===========================================================================

FUNCTION RRCSMC_EXTRACT_RETURN_HFN_PER_DOMAIN_AND_UPDATE_START_FLAG

DESCRIPTION
  This procedure does start value calculation for a given domain and 
  set the start value update flag.
  
  DEPENDENCIES
  None.

RETURN VALUE
  Updated start value. Calling func should apply offset to this value to use it for AM/UM/TM

SIDE EFFECTS
  None

===========================================================================*/
uint32 rrcsmc_extract_return_hfn_per_domain_and_update_start_flag
(
  rrc_cn_domain_identity_e_type domain_id
)
{
  uint32 updated_hfn;

  //if SMC is not received, start should never be re-calculated since 
  //count_c and count_i are not valid till a SMC is received..
  //so just return the START value read from the SIM
  if(rrcsmc_int_ciph_info.int_start_status_for_conn[domain_id] == FALSE)
  {
    updated_hfn = 
    rrcsmc_convert_START_to_count_c_i(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn, 0);
  }
  //otherwise, START needs to be re-calculated
  else
  {
    // Do start value calculation for this domain
    rrcsmc_extract_latest_start_list_per_cn_domain(domain_id, RRCSMC_UPDATE_START_LIST_LATEST);
  
    // Set the start value update flag as the start value has been re-calculated
    rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = TRUE;
  
    // Convert Start value to HFN
    updated_hfn = 
    rrcsmc_convert_START_to_count_c_i(rrcsmc_int_ciph_info.int_ciph_info[domain_id].hfn_latest, 0);
  }

  WRRC_MSG2_HIGH("Extracted new HFN %d for domain %d",updated_hfn,domain_id);
  return updated_hfn;
}

/*===========================================================================

FUNCTION rrcsmc_update_hfn_for_rlc_rb

DESCRIPTION
  This procedure updates HFN for RLC size change feature
  
  DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcsmc_update_hfn_for_rlc_rb
(
  rrc_RB_Identity rb_id, 
  rlc_reestablish_e_type direction
)
{
  uint32 hfn_update;
  rlc_lc_id_type rlc_id;

  // Update Start value and get LC Id for this RB
  hfn_update = rrcsmc_extract_return_hfn_per_domain_and_update_start_flag(RRC_PS_DOMAIN_CN_ID);
  
  // Since this is for AM channel, shift bits to make it a AM HFN
  hfn_update >>= RRC_RLC_AM_HFN_OFFSET;

  if(RRCLCM_RLC_LC_ID_NOT_FOUND == (rlc_id = rrclcm_get_dl_rlc_lc_id(rb_id)))
  {
    ERR_FATAL("lc_id not found for rb_id %d",rb_id,0,0);
  }

  MSG_HIGH("Updated HFN is %d for LC_ID %d, direction %d", hfn_update, rlc_id, direction);

  // Call RLC interface function to overwrite HFN value
  if(FAILURE == rlc_rrc_overwrite_hfn(rlc_id, hfn_update, direction))
  {
    WRRC_MSG1_ERROR("RLC HFN Update failed for lc_id %d",rlc_id);
  }
}  
/*===========================================================================

FUNCTION rrcsmc_send_ul_ciphering_key_req

DESCRIPTION
   This function sends cmd to L1 for configuring UL cipher 
   keys.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_send_ul_ciphering_key_req
(
void
)
{
  if ((smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag == TRUE )&&
      (ciphering_info.reconfig == TRUE)
     )
  {
    /* Send commands to L1 */
    (void) rrcllc_cphy_ciphering_key_req();
  }
  else
  {
    WRRC_MSG2_ERROR("Error: cant config ul cipher key's,rlc_suspend=%d,cipher_reconfig=%d",
                            smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag,
                            ciphering_info.reconfig);
  }
}

/*===========================================================================

FUNCTION rrcsmc_get_start_value_to_transmit_domain

DESCRIPTION
  This function returns the value of global 'start_value_to_transmit_domain'
  
DEPENDENCIES
  None.

RETURN VALUE
  CN Domain Identity

SIDE EFFECTS
  None

===========================================================================*/
rrc_cn_domain_identity_e_type rrcsmc_get_start_value_to_transmit_domain(void)
{
  return(start_value_to_transmit_domain);
}
/*===========================================================================

FUNCTION rrcsmc_get_security_start_status

DESCRIPTION
  This function will look to see if security is started.
  This includes check for ciphering or integrity active 
  for any CN domain.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If security is already stated for any CN domain
  FALSE: other wise.

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsmc_get_security_start_status(void)
{
  uint8 cn_id;
  boolean status = FALSE;
  
  /*Check to see if I.P is started*/
  if(integrity_protection_info.ip_status == RRCSMC_SECURITY_STARTED)
  {
    status = TRUE;
  }
  
  if(status == FALSE)
  {
    /*loop through to see is ciphering is started for any CN domain*/
    for(cn_id = 0;cn_id <RRC_MAX_CN_DOMAINS;cn_id++ )
    {
      if(rrcsmc_int_ciph_info.ciph_status[cn_id] == RRCSMC_SECURITY_STARTED)
      {
        status = TRUE;
        break;
      }
    }
  }
  WRRC_MSG1_HIGH("Security started status=%d",status);
  return status;
}

/*===========================================================================

FUNCTION rrcsmc_return_saved_security_key_set_status

DESCRIPTION
  This function will return status of security key set 
  generated in other RAT & saved in a buffer for CS domain.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
boolean rrcsmc_return_saved_security_key_set_status
(
  rrc_cn_domain_identity_e_type domain_id
)
{
  boolean status = FALSE;

  if((domain_id == RRC_CS_DOMAIN_CN_ID)&&
    (saved_security_key_set.grat_key_set_valid == TRUE))
  {
    /*We have one key stored in buffer, so return status as TRUE*/
    status = TRUE;
  }
  return(status);
}
/*===========================================================================

FUNCTION rrcsmc_reset_saved_security_key_set_status

DESCRIPTION
  This function will reset status of security key set 
  generated in other RAT & saved in a buffer for CS domain.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_reset_saved_security_key_set_status
(
  void
)
{
  /*Reset saved security key set status flag*/
  saved_security_key_set.grat_key_set_valid = FALSE;
}
/*===========================================================================

FUNCTION rrcsmc_check_and_update_security_key_set_gen_in_other_rat

DESCRIPTION
  This function will move security key sets(generated in other RAT for CS domain ) from
  buffer to ciphering & integrity db if any.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_check_and_update_security_key_set_gen_in_other_rat
(
  rrc_cn_domain_identity_e_type domain_id
)
{
  if(rrcsmc_return_saved_security_key_set_status(
       domain_id) == TRUE)
  {
    rrc_state_e_type rrc_state = rrc_get_state(); /* local var to store rrc state*/
    /*local ciphering info type used to init global vars*/
    rrcsmc_security_config_info_type *local_int_ciph_info_ptr; 
  
    WRRC_MSG0_HIGH("Updating Security key sets from buffer to security DB");
    local_int_ciph_info_ptr = &(rrcsmc_int_ciph_info);
        
    /*save the earlier keys in to a var.  We need this to calculate MAC for some
        SRB's whose activation time has not yet equalled the new activation times*/
    WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key_old,
                 sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                 local_int_ciph_info_ptr->int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key,
                 RRC_MAX_INTEGRITY_KEY_LENGTH);
   
    /*copy contents of integrity_key to int_ciph_info struct for this CN DOMAIN
     */
    WCDMA_MEMCPY(local_int_ciph_info_ptr->int_ciph_info[RRC_CS_DOMAIN_CN_ID].integrity_key,
                 sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
                 saved_security_key_set.integrity_key,
                 RRC_MAX_INTEGRITY_KEY_LENGTH);
  
   
    /*copy contents of ciphering_key to int_ciph_info struct
     *for this CN DOMAIN.  If we are currently using ciph key 1 then copy
    new keys to key0.  else copy to 1 and set new_key flag for this
    domain id to true
    */
    if (rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.key_used == 1)
    {
      WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.key[0],
                   sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                   saved_security_key_set.ciphering_key,
                   RRC_MAX_CIPHERING_KEY_LENGTH);
    }
    else
    {
      WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.key[1],
                   sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                   saved_security_key_set.ciphering_key,
                   RRC_MAX_CIPHERING_KEY_LENGTH);
    } 
    rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.new_key=TRUE;
  
    if ((rrc_state == RRC_STATE_CELL_DCH) ||
        (rrc_state == RRC_STATE_CELL_FACH))
    {/*This fucntion should be called only in IDLE state, but good to have this code*/
      rrcsmc_int_ciph_info.int_ciph_info[RRC_CS_DOMAIN_CN_ID].ciph_keys_info.
      new_key_for_start_value = TRUE;
    }

    /*Print integrity key here*/
    PRINT_CIPHER_INT_KEY(RRC_PRINT_INTEGRITY_KEY,saved_security_key_set)
  
    /*Print ciphering key here*/
    PRINT_CIPHER_INT_KEY(RRC_PRINT_CIPHERING_KEY,saved_security_key_set)

    rrcsmc_reset_saved_security_key_set_status();
    /*any previously sent start value is obliterated.. we should start using 0's from
      now onwards.. hence set start_value_update_flag to false*/
    //  rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;      ??????
    /* Fix for CR 195046 */
    if((rrc_state != RRC_STATE_CELL_FACH) &&
        (rrc_state != RRC_STATE_CELL_DCH)
      )
    {
      /*any previously sent start value is obliterated.. we should start using 0's from
       now onwards.. hence set start_value_update_flag to false*/
      rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;  
    }
  }
  else
  {
    /*Do nothing here, as there are no security keys generated in other RAT */
  }
}


/*===========================================================================

FUNCTION rrcsmc_send_continue_req

DESCRIPTION
  This function constructs the CRLC_UL_CONTINUE_REQ primitive and
  sends it to L2 UL Task.
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_send_continue_req
(
void
)
{
  /*counter*/
  uint32 i;  
  wcdma_l2_ul_cmd_type  *rlc_ptr = NULL;
  wcdma_l2_dl_cmd_type  *rlc_ptr_dl = NULL;

  WRRC_MSG0_HIGH("Dispatching out L2 cmd : Continue STOPPED RLC RB's (CRLC_UL_CONTINUE_REQ)");

  if (smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag == TRUE )
  {
    /*Send CONTINUE Req to RLC */
    /* Get L2 UL command buffer */

    rlc_ptr = wcdma_l2_ul_get_cmd_buf();
  
    /* Fill in the Command Header */
    rlc_ptr->cmd_hdr.cmd_id = CRLC_UL_CONTINUE_REQ;
  
    /* Fill in the Command Data */
    rlc_ptr->cmd_data.ul_continue.nchan =
      smc_rb_suspend_or_stop_info.suspend_or_stopped_info.nchan;
  
    for(i=0; i < smc_rb_suspend_or_stop_info.suspend_or_stopped_info.nchan; i++)
    {
      rlc_ptr->cmd_data.ul_continue.channel[i] =
        smc_rb_suspend_or_stop_info.suspend_or_stopped_info.suspend_stop[i].lc_id;
    }
    /* Send command to L2 UL Task */
    rrc_send_l2_ul_cmd(rlc_ptr);


    rlc_ptr_dl = wcdma_l2_dl_get_cmd_buf();

    /*Now send the DL continue request*/
    
    /* Fill in the Command Header */
    rlc_ptr_dl->cmd_hdr.cmd_id = CRLC_DL_CONTINUE_REQ;
  
    /* Fill in the Command Data */
    rlc_ptr_dl->cmd_data.dl_continue.nchan =
      smc_rb_suspend_or_stop_info.dl_stop_info.nchan;

    for(i=0; i < smc_rb_suspend_or_stop_info.dl_stop_info.nchan; i++)
    {
      rlc_ptr_dl->cmd_data.dl_continue.channel[i] =
        smc_rb_suspend_or_stop_info.dl_stop_info.lc_id[i];
    }
	
    WRRC_MSG0_HIGH_OPT("Dispatching out L2 cmd CRLC_DL_CONTINUE_REQ");
	
    /* Send command to L2 UL Task */
    rrc_send_l2_dl_cmd(rlc_ptr_dl);

    /*Now set the flag to FALSE*/
    
    smc_rb_suspend_or_stop_info.rlc_rb_suspended_or_stopped_flag = FALSE;           
    ciphering_info.reconfig = FALSE;
  }
  else
  {
    WRRC_MSG0_HIGH("Error: No suspended RLC RB's to resume");
  }
}

/*===========================================================================

FUNCTION   rrcsmc_append_already_computed_start_value_to_start_list

DESCRIPTION

  This function appends start list and takes care of the already calculated start value of 
  a given domain which will be included in msg and hence it doesnt re-compute the 
  start value for that domain.

DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_already_computed_start_value_to_start_list
(
  rrc_STARTList *start_list_ext,
  byte start_value[RRC_MAX_START_LENGTH], /* start_value should always have a valid value */
  rrcsmc_append_start_e_type append_start_type
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
  ,boolean both_cn_domain_open
  ,rrc_cn_domain_identity_e_type active_cn_domain
#endif
)
{
  /* local var to store count*/
  uint32 i,index;              
  
  rrc_cn_domain_identity_e_type domain_id; 
  /* Update the pointer to the list and allocate memory */

#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
  if(both_cn_domain_open == FALSE ) 
  {
    start_list_ext->n = 1;
    ALLOC_ASN1ARRAY(&enc_ctxt,start_list_ext,rrc_STARTSingle);
  }
  else
#endif
  {
    start_list_ext->n = rrcsmc_int_ciph_info.num_cn_domains;
    ALLOC_ASN1ARRAY(&enc_ctxt,start_list_ext,rrc_STARTSingle);
  }
  index = 0;
  
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
  MSG_HIGH("start_type:%d, both_cn_domain_open:%d active_cn_domain:%d",
            append_start_type,
            both_cn_domain_open,
            active_cn_domain);
#endif

  /*copy the contents of cn_domain into int_ciph_info struct*/
  for (i=0; i<rrcsmc_int_ciph_info.num_cn_domains; i++)
  {
    domain_id = rrcsmc_int_ciph_info.int_ciph_info[i].domain_id;
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
    if(both_cn_domain_open == FALSE)
    {
      if(active_cn_domain != domain_id)
      {
        continue;
      }
    }
#endif
    if (domain_id == RRC_CS_DOMAIN_CN_ID)
    {
      /* Update START values for CS domain */
      start_list_ext->elem[index].cn_DomainIdentity = rrc_CN_DomainIdentity_cs_domain;
    }
    else
    {
      /* Update START values for CS domain */
      start_list_ext->elem[index].cn_DomainIdentity = rrc_CN_DomainIdentity_ps_domain;
    }

    start_list_ext->elem[index].start_Value.numbits =  RRC_START_VALUE_LENGTH;

    //append start value for this domain
    if ((domain_id == RRC_CS_DOMAIN_CN_ID) &&
        (RRC_CS_DOMAIN_CN_ID == rrcsmc_get_start_value_to_transmit_domain()))
    {
      /* Copy the contents of the start value that was calculated for ps domain */
      WCDMA_MEMCPY(start_list_ext->elem[index].start_Value.data,
                   sizeof(start_list_ext->elem[index].start_Value.data),
                   start_value,
                   RRC_MAX_START_LENGTH);
    }
    else if ((domain_id == RRC_PS_DOMAIN_CN_ID)&&
             (RRC_PS_DOMAIN_CN_ID == rrcsmc_get_start_value_to_transmit_domain()))
    {
      /* Copy the contents of the start value that was calculated for ps domain */
      WCDMA_MEMCPY(start_list_ext->elem[index].start_Value.data,
                   sizeof(start_list_ext->elem[index].start_Value.data),
                   start_value,
                   RRC_MAX_START_LENGTH);
    }
    else
    {
      rrcsmc_append_start_per_domain(
        (byte *)(start_list_ext->elem[index].start_Value.data),
        domain_id,
        append_start_type);
    }

    if(i == (uint32) RRC_CS_DOMAIN_CN_ID)
    {
      MSG_HIGH("Appended CS-START: 0x%x 0x%x 0x%x", start_list_ext->elem[index].start_Value.data[0],
               start_list_ext->elem[index].start_Value.data[1],
               start_list_ext->elem[index].start_Value.data[2]);
    }
    else
    {
      MSG_HIGH("Appended PS-START: 0x%x 0x%x 0x%x", start_list_ext->elem[index].start_Value.data[0],
               start_list_ext->elem[index].start_Value.data[1],
               start_list_ext->elem[index].start_Value.data[2]);
    }
    index++;
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
    if(both_cn_domain_open == FALSE ) 
    {
 
      break;
    }
#endif

  }
  return SUCCESS;
}

/*===========================================================================

FUNCTION   rrcsmc_append_start_list_with_already_computed_start

DESCRIPTION

  This function appends start list and takes care of the already calculated start value of 
  a given domain which will be included in msg and hence it doesnt re-compute the 
  start value for that domain.

DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrcsmc_append_start_list_with_already_computed_start
(
  rrc_STARTList *start_list_ext,
  byte start_value[RRC_MAX_START_LENGTH], /* start_value should always have a valid value */
  rrcsmc_append_start_e_type append_start_type
)
{
  uecomdef_status_e_type status;

  status =  rrcsmc_append_already_computed_start_value_to_start_list (start_list_ext,
                                                     start_value,
                                                     append_start_type
#ifdef FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
                                                     ,TRUE                                         
                                                     ,RRC_CS_DOMAIN_CN_ID    /*dummy value. Doesn't matter b'coz both 
                                                              *domain are considered to be open anyways*/
#endif
                                                      );
  return status;  
}

#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
#error code not present
#endif

/*===========================================================================
FUNCTION rrcsmc_get_srns_info_from_dl_sdu_ind_r8

DESCRIPTION
  This function extracts ip check info/integrity protection mode info/ciphering mode info from SRNS 
  relocation triggering messages for Release 8 version of OTAs. In summary, input srns_info has to be filled
  from OTA in cmd_ptr.

  NOTE:  There is no R8 UTRANMobilityInformation and URAUpdateConfirm messages in ASN 8.6.
  Right now, only 5 Reconfig OTAs + CU CNF can do SRNS relocation for R8 OTA.

DEPENDENCIES
  None.

RETURN VALUE
  The function returns SUCCESS if extraction was successful
  The function returns FAILURE if extraction was unsucceful.

SIDE EFFECTS
  None
===========================================================================*/
uecomdef_status_e_type rrcsmc_get_srns_info_from_dl_sdu_ind_r8
(
  rrcsmc_srns_relocation_security_info_r7_type * srns_info,
  rrc_cmd_type *cmd_ptr
)
{
  uecomdef_status_e_type status = SUCCESS;           /*By default, return status is SUCCEES*/
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;       /* Pointer to Downlink DCCH */
  uint8 msg_type;                                                    /*Local variable to store message type */             
  
  /* Get the decoded OTA message pointer */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /*By default, set integrity and ciphering info status as FALSE in srns_info*/
  srns_info->integrity_info_present = FALSE;
  srns_info->ciphering_mode_info_present = FALSE;          

  /*SRNS-Reloc is posssible through DCCH messages only, so check for DL SDU type.*/
  switch (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
  { 
    case rrc_DL_DCCH_Message_PDU :

      /*Extract actual OTA type from cmd_ptr*/
      msg_type = rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
          cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

      switch (msg_type)
      {
        case T_rrc_DL_DCCH_MessageType_cellUpdateConfirm:
        {
          rrc_CellUpdateConfirm_r8_IEs *cucnf_ptr = 
            RRC_EXTRACT_R8_MSG_POINTER(dcch_msg_ptr,cellUpdateConfirm,cellUpdateConfirm_r8);
      
          if(RRCRB_R8_MSG_IE_PRESENT(cucnf_ptr,rrc_CellUpdateConfirm_r8_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(cucnf_ptr->integrityProtectionModeInfo);
          }
          if(RRCRB_R8_MSG_IE_PRESENT(cucnf_ptr,rrc_CellUpdateConfirm_r8_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(cucnf_ptr->cipheringModeInfo);
          }
        }
        break;

        case T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration:
        {
          rrc_RadioBearerReconfiguration_r8_IEs *rbrc_ptr = 
            RRC_EXTRACT_R8_MSG_POINTER(dcch_msg_ptr,radioBearerReconfiguration,radioBearerReconfiguration_r8);

          if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rbrc_ptr->integrityProtectionModeInfo);
          }
          if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(rbrc_ptr->cipheringModeInfo);
          }
        }
        break;
 
        case T_rrc_DL_DCCH_MessageType_radioBearerRelease:
        {
          rrc_RadioBearerRelease_r8_IEs* rb_release_ptr = 
            RRC_EXTRACT_R8_MSG_POINTER(dcch_msg_ptr,radioBearerRelease,radioBearerRelease_r8);
 
          if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rb_release_ptr->integrityProtectionModeInfo);
          }
          if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info =  &(rb_release_ptr->cipheringModeInfo);
          }
        }
        break;

        case T_rrc_DL_DCCH_MessageType_radioBearerSetup:
        {
          rrc_RadioBearerSetup_r8_IEs* rb_setup_ptr = 
            RRC_EXTRACT_R8_MSG_POINTER(dcch_msg_ptr,radioBearerSetup,radioBearerSetup_r8);

          if(RRCRB_R8_MSG_IE_PRESENT(rb_setup_ptr,rrc_RadioBearerSetup_r8_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(rb_setup_ptr->integrityProtectionModeInfo);
          }
          if(RRCRB_R8_MSG_IE_PRESENT(rb_setup_ptr,rrc_RadioBearerSetup_r8_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(rb_setup_ptr->cipheringModeInfo);
          }
        }
        break;
      
        case T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration:
        {
          rrc_TransportChannelReconfiguration_r8_IEs *tcreconfig_ptr =
            RRC_EXTRACT_R8_MSG_POINTER(dcch_msg_ptr,transportChannelReconfiguration,transportChannelReconfiguration_r8);

          if(RRCRB_R8_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,integrityProtectionModeInfo) )
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info =   &(tcreconfig_ptr->integrityProtectionModeInfo);
          }
          if(RRCRB_R8_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,cipheringModeInfo) )
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info =  &(tcreconfig_ptr->cipheringModeInfo);
          }
        }
        break;
      
        case T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration:
        {
          rrc_PhysicalChannelReconfiguration_r8_IEs *pcreconfig_ptr =
            RRC_EXTRACT_R8_MSG_POINTER(dcch_msg_ptr,physicalChannelReconfiguration,physicalChannelReconfiguration_r8);

          if(RRCRB_R8_MSG_IE_PRESENT(pcreconfig_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,integrityProtectionModeInfo))
          {
            srns_info->integrity_info_present = TRUE;
            srns_info->integrity_protection_mode_info = &(pcreconfig_ptr->integrityProtectionModeInfo);
          }
          if(RRCRB_R8_MSG_IE_PRESENT(pcreconfig_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,cipheringModeInfo))
          {
            srns_info->ciphering_mode_info_present = TRUE;
            srns_info->ciphering_mode_info = &(pcreconfig_ptr->cipheringModeInfo);
          }
        }
        break;      
        case T_rrc_DL_DCCH_MessageType_uraUpdateConfirm:
        {
           /* For uraUpdateConfirm, I.E integrityProtectionModeInfo is supported in R5/R7 messages only. */
           if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
              uraUpdateConfirm_r7.m.integrityProtectionModeInfoPresent )
           {
             srns_info->integrity_info_present = TRUE;
             srns_info->integrity_protection_mode_info = 
               &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
                 uraUpdateConfirm_r7.integrityProtectionModeInfo);
           }
           if(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
              uraUpdateConfirm_r7.m.cipheringModeInfoPresent )
           {
             srns_info->ciphering_mode_info_present = TRUE;
             srns_info->ciphering_mode_info = 
               &(dcch_msg_ptr->message.u.uraUpdateConfirm.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
                 uraUpdateConfirm_r7.cipheringModeInfo);
           }
         }
         break;
            
        default:
          MSG_MED("DL DCCH MSG %d not supported for SRNS-reloc",msg_type,0,0);
          status = FAILURE;
          break;
      }
      break;

    default:
      WRRC_MSG1_HIGH("DL-DCCH msg only for SRNS-reloc But DL SDU Type is %d",cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
      status = FAILURE;
      break;
  }

  /*Set Integrity Check-info only if Integrity info is present in the OTA.*/
  if(srns_info->integrity_info_present == TRUE)
  {
    srns_info->integrity_check_info = &(dcch_msg_ptr->integrityCheckInfo);      
  }

  return status;
}


/*===========================================================================

FUNCTION  rrcsmc_is_reconfig_in_progress

DESCRIPTION

  This function will check if SMC Procedure substate is reconfig in progress
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE: if SMC procedure is wating for reconfig in progress
  FALSE: othewise.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcsmc_is_reconfig_in_progress
(
  void
)
{
  boolean status = FALSE;

  if(rrcsmc_substate == RRCSMC_WAIT_FOR_RECONFIG_CNF)
  {
    status = TRUE;
  }
  return status;
}

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION rrcsmc_save_security_key_set_from_lte

DESCRIPTION
  This function will move security key sets (generated in LTE for PS domain ) from
  buffer to ciphering & integrity db if any.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcsmc_save_security_key_set_from_lte
(
  rrc_ho_ciph_req_type  ho_ciph_req,
  rrc_cn_domain_identity_e_type domain_id
)
{
  rrcsmc_security_config_info_type *local_int_ciph_info_ptr = NULL; 

  WRRC_MSG0_HIGH("L2W:Updating Security key sets from buffer to security DB");
  local_int_ciph_info_ptr = &(rrcsmc_int_ciph_info);
  
  /* Take a backup of keys and update */
  WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].integrity_key_old,
               sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
               local_int_ciph_info_ptr->int_ciph_info[domain_id].integrity_key,
               RRC_MAX_INTEGRITY_KEY_LENGTH);
  
  WCDMA_MEMCPY(local_int_ciph_info_ptr->int_ciph_info[domain_id].integrity_key,
               sizeof(byte) * RRC_MAX_INTEGRITY_KEY_LENGTH,
               ho_ciph_req.security_keys[domain_id].integrity_key,
               RRC_MAX_INTEGRITY_KEY_LENGTH);
  
  /* Copy contents of ciphering_key to int_ciph_info struct for this CN DOMAIN.  
     * If we are currently using ciph key 1 then copy new keys to key0  
     * else copy to 1 and set new_key flag for this domain id to true
     */
  if (rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.key_used == 1)
  {
    WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.key[0],
                 sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                 ho_ciph_req.security_keys[domain_id].ciphering_key,
                 RRC_MAX_CIPHERING_KEY_LENGTH);
  }
  else
  {
    WCDMA_MEMCPY(rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.key[1],
                 sizeof(byte) * RRC_MAX_CIPHERING_KEY_LENGTH,
                 ho_ciph_req.security_keys[domain_id].ciphering_key,
                 RRC_MAX_CIPHERING_KEY_LENGTH);
    /*Print ciphering key here*/
  } 
  
  /* Need to check this flag in detail */
  rrcsmc_int_ciph_info.int_ciph_info[domain_id].ciph_keys_info.new_key = TRUE;
  
  /*Print integrity key here*/
  PRINT_CIPHER_INT_KEY(RRC_PRINT_INTEGRITY_KEY, ho_ciph_req.security_keys[domain_id])
  PRINT_CIPHER_INT_KEY(RRC_PRINT_CIPHERING_KEY, ho_ciph_req.security_keys[domain_id])

  /*any previously sent start value is obliterated.. we should start using 0's from
    now onwards.. hence set start_value_update_flag to false*/
  rrcsmc_int_ciph_info.start_value_update_flag[domain_id] = FALSE;  
}


#endif /* FEATURE_LTE_TO_WCDMA */

/*===========================================================================

FUNCTION rrcsmc_revert_to_old_config_for_failure_message

DESCRIPTION
This function is used  to revert the cipher config db.
Whenever we update cipher config type to step-1 HO and later if message validation fails, 
we need to revert to the backed up config.

We also reset start value to transmit to 0.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcsmc_revert_to_old_config_for_failure_message(boolean need_log)
{
  /* If cipher_cfg_type is updated due to timing re-initialized HHO, we need to revert on failure, 
     otherwise in the next reconfiguration to MAC, we will send step-1 resulting in MAC using the same HFN 
     and it will result in voice garbling */
  if(cipher_config_db.mac_cipher_info[RRC_CS_DOMAIN_CN_ID].cipher_cfg_type == HO_STEP1_CIPHER_CFG)
  {
    rrcsmc_revert_to_old_cipher_config_db();
  }
  if(need_log)
  {
    MSG_HIGH("Resetting SVTHHO from 0x%x -> 0",start_value_to_transmit_cs_hho,0,0);
  }  
  start_value_to_transmit_cs_hho = 0;
}
/*===========================================================================
FUNCTION rrcsmc_get_smc_sub_state()

DESCRIPTION
  This function will return current 
  procedure's sub state.
DEPENDENCIES
  None

RETURN VALUE
  return current smc sub state.
SIDE EFFECTS
none
===========================================================================*/
rrcsmc_substate_e_type  rrcsmc_get_smc_sub_state(void)
{
  return rrcsmc_substate;
} /*rrcsmc_get_smc_sub_state*/  
/*===========================================================================

FUNCTION rrcsmc_return_substate

DESCRIPTION
returns SMC substate

We also reset start value to transmit to 0.

DEPENDENCIES
  None.

RETURN VALUE
  rrcsmc_substate_e_type
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrcsmc_substate_e_type rrcsmc_return_substate(void)
{
  return rrcsmc_substate;
}
#ifdef FEATURE_QSH_EVENT_METRIC

/*===========================================================================

FUNCTION rrcsmc_return_substate

DESCRIPTION
returns SMC substate

We also reset start value to transmit to 0.

DEPENDENCIES
  None.

RETURN VALUE
  rrcsmc_substate_e_type
===========================================================================*/

void rrcqsh_log_smc_ota_params(void)
{
  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.smc_params.domain = saved_security_mode_command_msg.domain_id; 
  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.smc_params.int_algo = saved_security_mode_command_msg.integrity_algo; 
  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.smc_params.ciph_algo = saved_security_mode_command_msg.ciph_algo; 
  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.smc_params.sec_capability = 0; /* Can we choose some other info from saved global ??*/	
}
#endif 
