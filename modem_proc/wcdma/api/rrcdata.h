#ifndef RRCDATA_H
#define RRCDATA_H
/*===========================================================================
              R R C D A T A  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  RRC Data
  
Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE//source/qcom/qct/modem/wcdma/api/main/latest/rrcmnif.h

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rrcdata.h#1 $   

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
05/25/16   sp      Made changes for Mav debug snapshot
05/12/16   sas     Added api interface fun to provide acqdb cell list info to LTE.
02/09/15      sp      Made changes to add a new API is added which takes as_id as agrument and calls rrcdormancy_check_if_nw_support_fast_dormancy() 
06/09/14   sa      Made changes to support UL 16QAM
05/20/14   sr      Added prototype for "rrc_check_gsm_bands_supported"
05/14/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
18/04/14   ad      Made changes to adaptively calcualte T304 for GCF TCs
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
08/01/14   sp      Removed inclusion of l1rrcif.h
11/14/13   sa      DC feature support will be controlled thru UE Cat
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
07/03/13   sa      fixed the compilation error
06/27/13   sa      Added changes to support call type mask for AMR with PS0x0 config
05/23/13   geg     WSW decoupling (except for component of QCHAT)
04/29/13   sa      Made code changes for eAMR supprot
04/25/13   sg      Added code for REL 9 changes
03/22/13   mp      Removed tmc.h include file
02/23/13   sn      Changes to SIB bit mask order to be in agreement with rrc_SIB_type enum
12/07/12   sn      CSG feature changes
12/05/12   gv      Made changes to correctly calculate the remaining
                   free memory in DSM small item pool
11/08/12   pm      Made changed to remove rrcasn1_ie.h from rrcdata.h to avoid SU API
                   access violations.
03/28/12   zr      Adding feature definitions for Dime
02/23/12   sks     Added support for WTOL PSHO.
02/2/12    ad      Removed references to third party 
02/15/12   rl      Defeaturization changes 
01/19/12   ad      Added protoypes for rrc_save_mvs and rrc_restore_mvs
12/01/11   as      Fixed compilation error
11/30/11   md      Added prototype of function rrc_find_rab_for_rb_in_est_rabs.
07/29/11   ad      Made changes for CMI Phase 4
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/09/11   ad      Fixed compilation warnings
06/08/11   ad      Added log_wcdma_trch_type under FEATURE_WCDMA feature
06/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.99.50
05/02/11   ad      Added MMAL changes to return the trch type for the OTA
05/30/11   rl      CMI de-featurization
05/30/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.97.50
05/30/11   ad      Fixed compilation errors
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/21/11   ad      Fixed compilation warnings
04/20/11   ad      Fixed compilation issue
04/19/11   ad      Changes to add callback function for MMAL support
04/13/11   ad      Changes to address the rate change issue for voice call
                   Qchat media concurrency scenarios
04/11/11   ad      Added function which returns the message type,
                   transaction ID,etc. under FEATURE_MMAL macro
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/17/11   ad      RRC_CSP_GET_INT_MCC and RRC_CSP_GET_INT_MNC moved from 
                   rrcdata_v.h to rrcdata.h
03/17/11   su      Fixed Lint warning.
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/02/11   ad      Fixed compilation warnings on 3.0 PL
03/14/11   su      Added code changes to give UL/DL UE supported CAT and 
                   max. data rate possible to DS.
03/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/28/11   ad      Fixed compilation warnings on 3.0 PL
02/24/11   ad      Moved rrc_return_lc_info_for_rab protoype to rrcdata_v.h
02/19/11   sk      Created RRC_NV_REL_INDICATOR_MINIMUM_REL7 for GPS team. 
02/19/11   sk      Moved rrc_nv_rel_indicator to internal file. 
02/15/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.91.50
02/11/11   su      Added code to support setting UE HSDPA category 
                   through QXDM command.
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11   ad      Added changes to refeaturize the API files
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   ad      Moving RRC_NV_REL_INDICATOR_REL7 to API folder
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   rm      Fixed merging issue
10/29/10   su      Made changes to remove WCDMA_TASKLOCK() usage from RRC code base.
10/19/10   su      Fixed compiler warning.
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
07/29/10   su      Fixed compilation errors on MOB
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/18/10   prk     Fixed compiler warnings.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/30/10   dm      Reverted the changes made in the previous check-in.
03/29/10   dm      Made changes to allocate TMC sib heap if it is not already 
                   allocated.
03/15/10   gkg     Made changes to put changes for "Multiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
03/01/09   kp      Fixed compilation issue.
12/22/09   gkg     Made changes for CSoHS so that NV 6878 controls CSoHS operation.
12/05/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
11/02/09   rm      Added support for FEATURE_MODEM_STATISTICS
10/15/09   kp      Added code to set num SDU’s to be purged to 0xFF 
                   RLC will delete all the SDU’s which are in water mark.
06/24/09   gkg     Added support for API for getting the DC HSPDA Status
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
10/01/09   gkg     Made changes so that CS RAB Type is provided to MM in case
                   a RAB is released. For this, modified type rrcrb_rab_e_type.
08/03/09   kp      Changes to obsolete gs_alloc/gs_free under feature flag FEATURE MODEM HEAP
06/24/09   ps      Added support for API for getting the HSPA PLUS Status
06/22/09   kp      Added support to free memory from modem heap only for sib free 
                   when FEATURE MODEM HEAP is defined.
06/08/09   dm      Added support for freeing 100k sib heap when WCDMA stack is 
                   inactive as part of ZI reduction activity. code changes are under
                   FEATURE_WCDMA_RRC_ZI_OPT_PHASE_2.  
06/05/09   ps      Made changes to change MODEM_MEM_CL_WCDMA….to 
                   MODEM_MEM_CLIENT_WCDMA_.. accordinf to memory team requirement
05/18/09   ps      Changed MODEM_MEM_CL_WCDMA_RRC_CMD_CRITICAL
                     to MODEM_MEM_CL_WCDMA_RRC_CMD_CRIT
05/07/09   ps      Made changes for to allocate memory for RRC Heap and RRC Cmd
                   Pool from Modem Heap.
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
03/31/09   rm      Supressed Lint error "function 'rrc_free' #define'd, semantics may be lost"
02/27/09   kp      Added support code to free anoter 46k from sib heap under feature 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP.
02/02/09   kp      Added support for optimized SIB heap under feature flag 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
11/28/08   gkg     Made Changes for new MVS-RRC Interface to (de)configure vocoder
                   for AMR Voice Calls.
10/24/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
10/10/08   ps      Removal of feature FEATURE_GTOW_HO_TO_UTRAN_SPOOF_ENABLE
                    As the Spoof  is no longer required for Anritsu
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
06/30/08   ns      Made the necessary changes to enable the functionality of the feature 
                   FEATURE_NETWORK_SHARING only when the GCF NV is set
06/16/08   ps      Moved the define of INVALID_TRCH_ID out of HSDPA feature flag
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs
                   in Cell_DCH state.
06/13/08   gkg     Added signature for function rrc_get_idx_in_rrc_est_rabs_for_rab_id().
03/10/08   kp      Added support for SIB11bis. Code changes are under feature flag 
                   FEATURE_SIB11BIS_SUPPORT 
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
03/03/08   vk      In structure rrc_hsdpa_msg_struct_type, declared a bool that
                   indicates if MAC needs to restore the TSN's under 
                   FEATURE HSDPA MAC HS DL RESTORE TSN
22/02/08   ps/gkg  To support Spec CR 3160, DC 12 is removed for rel99 and rel5.
01/08/08   ps/gkg  Made changes to query the RAb type from Mn even for 
                   Handover To utran msg.
12/21/07   da      Support for FEATURE_MBMS
12/12/07   gkg     Added support for VT->Voice fallback and vice versa
                   when "RAB Info to Reconfig " IE is not present in the RB-Reconfig/RB-Release
                   msg  for SCUDIF under feature flag FEATURE_SCUDIF_ENHANCEMENT
26/11/07   gkg     Added support for VT->Voice fallback and vice versa
                   by RB-Reconfig msg for SCUDIF.Also added changes to 
                   support VT->Voice fallback using RB-Setup msg.
                   Changes under feature flag FEATURE_SCUDIF_ENHANCEMENT
11/23/07   pk      Added prototype for a new function rrc_extract_rac_from_sib_info().
11/21/07   rc/vm   Added support for FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY feature.
10/30/07   ps      Added support for DC 14
10/23/07   ps      Added support for DC 20 through Rb-Setup msg
10/23/07   pk      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE_NETWORK_SHARING.
09/07/07   vm      Added support for feature FEATURE_RRC_CALLOC
08/28/07   kp      Added support for capturing RACH transmission Statistic.
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
07/27/07   da      Put code changes for HSPA call status ind under feature 
                   flag FEATURE_HSPA_CALL_STATUS_IND
07/13/07   da      Added support for HSPA call status indication.
05/23/07   ps      Added support for "RB Info to Change" IE in RB-Setup msg.
                   Change are made under feature flag FEATURE_DEFAULT_CONFIG_IN_RB_SETUP.
04/24/07   kp      Added support for maintaining RACH transmission statistics
                   which is needed to evaluate performance in TCXO Shutdown.
                   This is currently TBD. Its values are set to Zero for now.
04/20/07   vm      Made RRC heap size a multiple of a kibibyte (KiB)
04/17/07   pk      Added support for maintaining intra frequency cell reselection,
                   WTOG cell reselection and OOS statistics which are needed to evaluate 
                   performance in TCXO Shutdown.
04/10/07   ps      Added support for default configs sent through Rb-Setup msg
                   The support for default config 6 has also been added.
03/09/07   vm      Added support for FEATURE_RRC_HEAP.
03/02/07   ps      Added support for WB-AMR .Also added support for  
                   default configs 13, 16 and 21
02/07/07   sm      Moved rrc_malloc to inline function from macro. Added a check for 
                   NULL pointer inside the function.
02/02/07   ps      Added support for default config 15,18and 19
12/11/06   segu    Added the hash definition of RRC_RSSI_VALID_REPORT_TIME_UNIT_OOS
10/25/06   sgk     Moved prototype function rrc_check_if_wcdma_can_be_stopped
                   under FEATURE_WCDMA_TRM. 
10/18/06   ps      Added support for default config 22
10/16/06   sgk     7600 support added prototype for rrc_check_if_wcdma_can_be_stopped. 
09/25/06   kp      Moved sib bitmask from rrccspi.h
09/15/06   vm      Added support for the feature FEATURE_UI_RF_ENABLED_BANDS_SUPPORT.
09/11/06   vk      Added prototype for RLC Size Change functions. Also declared
                   prototype for structure rlc_size_change_struct_type. Changes have
				           been featurized under FEATURE RLC ONE SIDE RE ESTABLISHMENT
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/23/06   ps      Externized default config 10 global variable.
08/21/06   tkk     Added SRB#5 support.
08/02/06   tkk     Externized "rrc_msg_failure_cause" global variable.
06/26/06   tkk     Added prototypes for functions
                   "rrc_find_if_voice_rab_exists_in_established_rabs",
                   "rrc_update_rb_info_to_existing_voice_rab"
06/15/06   vk      Added prototype for fn rrc_get_hsdpa_cell_capability_indicator()
                   and fn rrc_send_3g_cell_capability_information()
06/08/06   vm      1. Added new enum type w_uarfcn_source_e_type.
                   2. Added prototype of new function 
                      rrccsp_is_mcc_and_850_800_supported_band_comb_valid().
                   3. Added another prototype of the function validate_dl_arfcn()
                      under the feature flag FEATURE_MCC_AND_850_800_BAND_COMBINATION.
                   4. Updated prototype of the function validate_fdd_frequency().
                   5. Added prototype of new function is_freq_in_overlapping_850_and_800_bands().
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE_REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
05/08/06   segu    Added support for RSSI optimazation
05/01/06   vk      Added new parameter beta_table_present, hrnti_changed in 
                   hsdpa_message_param structure under FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
03/07/06   vr      Moved the #defines used for purging WM from rrccu.c to rrcdata.h
                   since it is now being used outside CU also
02/23/06   ss/tkk  Added prototype for "rrc_get_amr_mode_bitrate" to return the
                   AMR rate type (low bit rate or high bitrate) from AMR rate.
11/10/05   sm      Added prototype of function rrc_get_wcdma_rrc_version_nv(), macros 
                   RRC_NV_REL_INDICATOR_REL99, RRC_NV_REL_INDICATOR_REL5, and 
		               RRC_NV_REL_INDICATOR_REL6
11/04/05  da/vr    Made changes to detect UE inactivity and take appropriate actions.
10/19/05   vr      Added new procedure id(RRC_PROCEDURE_CHK) for Counter Check Procedure
10/03/05   tkk     Added new structure to handle Rel-5 RRC connection setup for 
                   default configuration.
09/14/05   tkk     Added prototype for updating N312 & N315 constants for Rel5.
08/08/05   ss      Added the function rrc_translate_establishment_cause to map the internal
                   establishment cause to its ASN equivalent
09/02/05   da      Added prototypes for the following funcs.  
                   1.rrc_get_downlink_rb_rate() 2. rrc_get_uplink_rb_rate()
                   3.rrc_is_wcdma_active()
08/24/05   ss      Externized default config variables 0,1,3 from rrciho.c and added
                   default configurations 4,7,8 & 9.
08/10/05   vk      Added hs_dpcch_info_present, in struct rrc_hsdpa_msg_struct_type
                   to indicate if Delta ACK/NACK has changed
07/20/05   vk      Made the function rrc_get_hsdpa_status() as Extern so that it could be called
                   by other entities
07/16/05   ss      Added prototype to support translating of N313 timer.
07/15/05   vk      Function rrc_translate_hrnti() now has a return type. Changed the
                   name of primitive dl_rlc_um_rlc_l1_size_present to dl_rlc_um_rlc_li_size_present
07/15/05   da      Added prototype for func rrc_get_lac_id()
07/07/05   vr      Flag to enable or diable Dynamic Power Class feature
04/06/05   da      Added prototype is_ctch_present_in_serving_cell()
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
03/09/05   kc      Added prototype to update connected mode timers from v3a0 ext
12/06/04   da      Added prototypes for validate_rb_id_in_reconfig_list() and 
                   validate_rb_id_in_release_list()  
11/23/04   sm      Added prototypes for utility functions to check/set est-rab params.
                   Introduced 'rab_type' in struct 'rrc_est_rab_info_type'.
10/08/04   sk      Added prototype fori function rrc_find_rab_for_rb_in_est_rabs() to 
                   get the rab_id associated with the rb id from established rab variable.
10/05/04   bu      Added prototype for validate_dl_arfcn().
08/30/04   sgk     Added prototype for get_rx_window_size defined in rrcllcrbie.c
08/25/04   bu      Added the prototype for validate_fdd_frequency().
08/19/04   sgk     Added the prototype for rrc_csp_send_interrat_cell_change_ind
08/05/04   jh      Exposed prototype for validate_frequency_info() function.
07/15/04   jh      Moved rrc_validate_frequency_info() to a different file.
06/18/04   kc      Increased the size of RRC_TOT_SCN to 128. It was originally
                   63 but more than 63 registrations are occuring currently
                   in the code.
06/16/04   ttl     Extern tmc_heap_sib, tmc_heap_sib_mem_buffer.
                   Define TMC_HEAP_SIB_MEM_BUFFER_SIZE.
06/07/04   vn      Added macro RRC_PRINT_SIB_HEAP to print SIB Heap usage.
06/07/04   svk     Added macro definitions for rrc_sib_malloc, rrc_sib_free 
                   under FEATURE_RRC_SIB_HEAP.
06/04/04   jh      Added constants for UARFCN validation.
05/06/04   jh      Added function rrc_validate_frequency_info() to check
                   range of UL and DL UARFCN parameters.
04/22/04   sk      Added rrc_ prefix to two structure types encode_info_type
                   and decode_info_type
04/21/04   sk      Added two structure types encode_info_type and
                   decode_info_type to get ASN.1 encoding/decoding failure
                   information.
04/20/04   sk      Added a new structure type
                   rrc_established_rabs_for_a_domain_type
                   Added prototype for new function
                   rrc_get_rabs_in_established_rabs_for_given_domain()
04/06/04   bu      Removed feature definition from rrc_proc_e_type.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
02/11/04   sk      prototype for urnti translation function.
                   Added ura-pch as valid state.
12/04/03   vn      Declaration for a global array rrc_cfn_before_decode
                   that holds timestamps for the various OTA RRC signalling messages
09/22/03   sk      Removed the reference to #defined values from rrc_malloc
                   rrc_realloc and rrc_free functions.
07/29/03   sk      Added a new function to find rb in established_rabs variable
05/23/03   bu      Added RRC_MODE_CHANGE to rrc_mode_e_type.
04/10/03   rj      Added new procedure enum type for Cell Change Order from
                   UTRAN.
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to 
                   INTERRAT_HANDOVER_GTOW
02/13/03   jh      Added prototype for rrc_translate_urnti_short() function.
12/12/02   jh      Updated NUM_OF_BITS_FOR_CCCH_MSG_TYPE and
                   NUM_OF_BITS_FOR_DCCH_MSG_TYPE to
                   NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE and
                   NUM_OF_BITS_FOR_DL_DCCH_MSG_TYPE.  Added
                   NUM_OF_BITS_FOR_UL_CCCH_MSG_TYPE and
                   NUM_OF_BITS_FOR_UL_DCCH_MSG_TYPE defines.
                   Added prototypes for rrc_get_ul_ccch_message_type() and
                   rrc_get_ul_dcch_message_type() functions.
10/24/02   rj      Added macros for rrc_malloc, rrc_free, and rrc_realloc. 
                   These macros use the suitable heap depending on size of the 
                   memory that is allocated.
10/03/02   jh      Added rrc_return_cs_data_call_parms() function to return
                   the SDU size and number of SDUs per TTI for circuit-switched
                   data calls.
08/09/02   vn      Added LLC in the list of procedures.
07/29/02   xfg     Added a procedure identifier for IHO for interrat handover 
                   support
06/13/02   rj      Removed external declarations for rrc_u_rnti_internal and 
                   rrc_u_rnti_short from this module since the same information
                   is available in the ORDERED_CONFIG.
05/17/02   ram     Increased the size of RRC_TOT_SCN. It was originally
                   31 but more than 31 registrations are occuring currently
                   in the code.
05/16/02   jh      Added AC info to global data.
05/13/02   ram     Moved RRC_TOT_SCN from rrcscmgr.c to here since it's
                   also used by rrcintif.h.
04/30/02   xfg     Added a procedure identifier for the Mode Control module
04/22/02   kc      Moved Security related const Definitions to rrcsmc.h
04/09/02   rj      Added prototype to check if RABs are established for
                   any particular CN Domain.
03/29/02   rj      Moved function prototype that appends START value to 
                   RRCSMC module.
03/05/02   rj      Renamed varible rrc_u_rnti to rrc_u_rnti_internal since 
                   VC++ debugger is confusing rrc_u_rnti variable with  
                   rrc_U_RNTI type declaration.
02/28/02   rj      Added additional field for re-estbalish timer in structure
                   rrc_est_rab_info_type. Added prototypes for functions
                   rrc_translate_state_indicator_to_state, rrc_convert_t305_to_ms,
                   rrc_release_rbs_associated_with_re_est_timer and  
                   rrc_check_rbs_associated_with_re_est_timer. Also moved
                   function prototype that appends START value from RRCRCE module.
02/14/02   rj      Deleted variable rrc_c_rnti from RRCDATA module since 
                   Ordered Config maintains the same.
02/05/02   kc      Added var and function to update/initialize 
                   ue_conn_timers_and_constants
01/30/02   xfg     Added a new procedure id for LB test
01/07/02   ram     Changed RRC_TOT_SIBS to 32 as per the June Spec. Updated
                   Copyright.
12/21/01   vn      Changed rrc_scr_code_type and rrc_freq_type to uint16 - 
                   needed for space saving in NV.
12/18/01   kc      Added functions to convert TMSI and PTMSI to integer.
12/12/01   vn      Convert IMSI, IMEI and Initial UE ID to a 64 bit value.
10/31/01   vn      Added prototypes for functions rrc_bcch_and_nas_plmns_match
                   and rrc_bcch_plmns_match. Removed RRC_SAME_PLMNS and RRC_NOT
                   SAME_PLMNS since these are replaced by the new functions.
08/31/01   xfg     Added function prototype for rrc_rssi_ber_timeout_handler
08/30/01   rj      Moved prototypes for functions rrc_get_dl_dcch_message_type
                   and rrc_get_dl_ccch_message_type from RRC dispatcher since
                   these are required by other RRC procedures. Added bit 
                   positions to peek into OTA message to identify message
                   type.
08/28/01   kiran   Added function rrc_log_protocol_error to support logging of
                   protocol errors.  Right now it supports only QXDM logging
08/13/01   ram     Added structure and function prototype to support getting
                   logical channel ids for user-plane RABs when requested by
                   user-plane entities. Right now it supports only CS data and
                   PS rabs.
07/27/01   rj      Removed formal parameter for function
                   rrc_convert_initial_ue_identity_into_int_value.
07/27/01   rj      Modified release cause type to internal format from the external
                   format. Modified IMSI, IMEI and Initial UE Id conversion
                   functions from internal types to integers.
07/25/01   rj      Added a prototype  for the function 
                   rrc_convert_gsm_map_imsi_into_int_value.
07/20/01   rj      Added a declaration for counter V304.
05/17/01   ram     Added function prototype rrc_remove_rb_from_established_rabs.
                   Added structures to hold the results of removing an RB
                   from the variable ESTABLISHED_RABS.
05/16/01   vn      Changed RRC_SAME_PLMNS and RRC_NOT_SAME_PLMNS due to changed
                   representation of rrc_plmn_identity_type. Added macros 
                   RRC_CSP_GET_INT_MCC and RRC_CSP_GET_INT_MNC to convert MCC 
                   and MNC from an array to an integer form for display.
05/02/01   rj      Renamed rrc_system_specific_cap_update_req_type to
                   rrc_system_specific_cap_update_req_e_type and typedefed to
                   external enum type rrc_SystemSpecificCapUpdateReq.
04/30/10   rj      Added declarations to represent variable 
                   rrc_capability_update_requirement in internal format.
04/29/01   rj      Modified declaration to C-RNTI and U-RNTI to internal format.
                   Added declarations for IMSI, TMSI and PTMSI. Added prototype
                   declarations for functions to translate external format of
                   C-RNTI and U-RNTI into internal format.
04/27/01   ram     Added a union type that is used in converting the ossBitString
                   type to a uint32 type. Added a edit history comment that was
                   taken out by mistake.
04/26/01   ram     Added support for the UE Variable ESTABLISHED_RABS. 
                   Provided access functions for RRC procedures to 
                   add RABs to or remove RABs from this variable.
04/23/01   ram     Moved rrc_state_e_type definition here from rrctask.h since
                   state change functionality has been removed from rrctask.h.
04/06/01   rj      Removed enum for Downlink outer loop control procedure and
                   renamed RRC_PROCEDURE_RR to RRC_PROCEDURE_UMI as per  the
                   December Spec release.
03/30/01   rj      Added prototypes to functions rrc_translate_imei and 
                   rrc_translate_gsm_map_imsi. Modified RRC Connection 
                   Establishment cause and Initial UE Identity from external
                   format to internal.
03/23/01   ram     Added external function prototypes rrc_translate_gsm_map_plmn_id
                   and rrc_translate_cell_id to translate external (ASN.1) formats
                   to internal formats used by RRC modules.
03/14/01   vn      Added rrc_cell_id since ASN1 now defines it as a link list.
                   Added rrc_translate_access_class_barred_list to translate 
                   access class barred list from ASN1 to RRC internal format.
02/12/01   rj      Added varibales rrc_protocol_error_information, rrc_v_308,
                   and rrc_release_cause for RRC Connection Release procedure.
01/26/01   vn      Added RRC_PROCEDURE_CCM for Cell Change Manager.
01/25/01   rj      Moved rrc_dpch_info_present to RRCRCE module since is is
                   local to the RRCRCE module.
01/17/01   vn      Added typedefs for rrc_scr_code_type and rrc_freq_type for
                   usage within RRC.
01/08/01   ram     Created a macro VALID_RRC_STATE to check if a given parameter
                   indicates a valid RRC state. RRC_STATE_WILDCARD is not considered
                   a valid RRC state by this macro. Updated copyright.
01/04/01   vn      Moved definitions for RRC_SAME_PLMNS and RRC_NOT_SAME_PLMNS 
                   from CSP file to here.
12/22/00   rj      Added EDIT HISTORY Header
12/22/00   rj      Added new variable declarations that are required by 
                   the RRC Connection Establishment procedure.
12/20/00   ram     Moved the definition RRC_TOT_SIBS to this file from 
                   rrcsibproci.h since this definition is used by multiple
                   files.
11/22/00   rj      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "comdef.h"

#include "queue.h"

#include "rex.h"

#include "rrcmmif.h"
#include "uecomdef.h"
#include "macrrcif.h"
#include "rlcrrcif.h"
#include <string.h>
#include <mvsamr.h>
#include <mvs.h>
#include "modem_mem.h"
/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/
/* Total number of SIBs (Including the MIB, Scheduling Blocks and 3 spares). 
This is as per the June '01 version of the RRC specification.
See the definition rrc_SIB_type in rrcsibproci.h for details. */
#define RRC_TOT_SIBS 16


#define RRC_TOT_SCN 128 /* Total number of state change notifications
                       supported my the module. This may be changed
                       as desired to support all RRC procedures. */
#ifdef FEATURE_WCDMA_DC_HSUPA
#define MAX_DC_HSUPA_CATEGORY  8
#endif
#ifdef FEATURE_WCDMA_16_QAM
#define MAX_UL_16QAM_HSUPA_CATEGORY  7
#endif
#define RRC_WCMDA_HSUPA_DEFAULT_CATEGORY 6

/* This is used between CSP,MEAS & CU functions for decoding SIB bit mask in 
     SIB_CHANGE_IND sent by sib*/
typedef  uint32  rrc_sib_change_mask_type;



/* The following define SIB bit masks */
#define MIB_BIT           0x00000001
#define SB_1_BIT          0x00000002
#define SB_2_BIT          0x00000004
#define SIB_1_BIT         0x00000008
#define SIB_2_BIT         0x00000010
#define SIB_3_BIT         0x00000020
#define SIB_4_BIT         0x00000040
#define SIB_5_BIT         0x00000080
#define SIB_6_BIT         0x00000100
#define SIB_7_BIT         0x00000200
#define SIB_11_BIT        0x00000400
#define SIB_12_BIT        0x00000800
#define SIB_18_BIT        0x00001000
#define SIB_5bis_BIT      0x00002000
#define SIB_11BIS_BIT     0x00004000
#define SIB_19_BIT        0x00008000

/*MAcro which indicates one time RACH attempt for a RRC message, 
 *This is needed for RACH transmission statistics to evaluate performance
 *in TCXO Shutdown */
#define RRC_RACH_TX_STATS_ONE_TIME_RACH 1
#define RRC_NV_REL_INDICATOR_REL7 0x4

/*If the NV is 3 or above, then UE supports Rel7*/
#define RRC_NV_REL_INDICATOR_MINIMUM_REL7 3

/*! @brief Macros for remaining free memory in DSM small item pool
*/
#define RRC_REM_SMALL_ITEM_POOL_SIZE (DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL) \
                                          * DSM_POOL_ITEM_SIZE(DSM_DS_SMALL_ITEM_POOL))

/*! @brief DSM Pool to use
*/
#define RRC_DSM_POOL_ID(size) ((size <= RRC_REM_SMALL_ITEM_POOL_SIZE) ? \
                                      (DSM_DS_SMALL_ITEM_POOL) : (DSM_DS_LARGE_ITEM_POOL))


typedef struct {
  uint8 channel_type;                       /* Variable for channel type */
  uint8 msg_type;                       /* Variable for message type */
  uint32 transactionid;                       /* Variable for transaction id */
  uint8 direction;
  uint8 *raw_sdu;
  uint32 raw_sdu_length;
  log_wcdma_trch_type transport_channel_type;
} log_message_type;

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/
typedef enum
{
  WCDMA_RAT_MOB,
  LTE_RAT_MOB,
  GSM_RAT_MOB,
  LTE_RAT_SRVCC_MOB,
  LTE_RAT_CSFB_MOB,
  IRAT_MOB_FAIL,
  CELL_RESEL_FAIL,
  CELL_SEL_FAIL,
  IRAT_INVALID
}wrrc_mobility_rat_enum_type;

typedef enum
{
  WRRC_RLF,
  WRRC_OOS,
  WRRC_CON_REJ,
  WRRC_UPDATE_INVALD
}wrrc_rlf_oos_con_reject_enum_type;

/* Enumerated type for the types of RABs that need to be setup */
typedef enum
{
  RRCRB_CS_VOICE_RAB,  /* Circuit Switched Voice */
  RRCRB_CS_DATA_RAB,   /* Circuit Switched Data  */
  RRCRB_PS_DATA_RAB,   /* Packet Switched Data   */
  RRCRB_UNKNOWN_RAB    /* Unknown (invalid) RAB  */
}rrcrb_rab_e_type;

typedef enum
{
  RRC_PROCEDURE_NONE,                      /* None RRC Procedure. */
  RRC_PROCEDURE_CSP,                       /* Cell Selection Procedure. */
  RRC_PROCEDURE_CCM,                       /* Cell Change Manager */
  RRC_PROCEDURE_SIB,                       /* SIB Processing Procedure. */
  RRC_PROCEDURE_PG1,                       /* Page Type 1 Procedure. */
  RRC_PROCEDURE_RCE,                       /* RRC Connection Establishment 
                                              Procedure. */
  RRC_PROCEDURE_RCR,                       /* RRC Connection Release 
                                              Procedure. */
  RRC_PROCEDURE_RCRE,                      /* RRC Connection Re-establishment
                                              Procedure. */
  RRC_PROCEDURE_UECI,                      /* UE Capability Information 
                                              Procedure. */
  RRC_PROCEDURE_UECE,                      /* UE Capability Enquiry 
                                              Procedure. */
  RRC_PROCEDURE_IDT,                       /* Initial Direct Transfer
                                              Procedure. */
  RRC_PROCEDURE_UDT,                       /* Uplink Direct Transfer
                                              Procedure. */
  RRC_PROCEDURE_DDT,                       /* Downlink Direct Transfer
                                              Procedure. */
  RRC_PROCEDURE_PG2,                       /* Paging Type 2 Procedure. */
  RRC_PROCEDURE_SMC,                       /* Security Mode Control
                                              Procedure. */
  RRC_PROCEDURE_SCR,                       /* Signalling Connection Release
                                              Procedure. */
  RRC_PROCEDURE_SCRR,                      /* Signalling Connection Release
                                              Request Procedure. */
  RRC_PROCEDURE_CC,                        /* Counter Check Procedure. */
  RRC_PROCEDURE_RBE,                       /* Radio Bearer Establishment
                                              Procedure. */
  RRC_PROCEDURE_RBRC,                      /* Radio Bearer Re-configuration
                                              Procedure. */
  RRC_PROCEDURE_RBR,                       /* Radio Bearer Release 
                                              Procedure. */
  RRC_PROCEDURE_TCR,                       /* Transport Channel
                                              Re-configuration Procedure. */
  RRC_PROCEDURE_TFC,                       /* Transport Format Combination
                                              Control Procedure. */
  RRC_PROCEDURE_PCR,                       /* Physical Channel 
                                              Re-configuration Procedure. */
  RRC_PROCEDURE_CU,                        /* Cell Update Procedure. */
  RRC_PROCEDURE_UU,                        /* URA Update Procedure. */
  RRC_PROCEDURE_UMI,                       /* UTRAN Mobility Information
                                              Procedure. */
  RRC_PROCEDURE_ASU,                       /* Active Set Update Procedure. */
  RRC_PROCEDURE_ISHTU,                     /* Inter-System Handover to 
                                              UTRAN. */
  RRC_PROCEDURE_ISHFU,                     /* Inter-System Handover from
                                              UTRAN. */
  RRC_PROCEDURE_ISCTU,                     /* Inter-System Cell Reselection
                                              to UTRAN. */
  RRC_PROCEDURE_ISCFU,                     /* Inter-System Cell Reselection
                                              from UTRAN. */
  RRC_PROCEDURE_MCMR,                      /* Measurement Control and 
                                              Measurement Report Procedure. */
  RRC_PROCEDURE_LBT,                       /* Loopback Test module */

  RRC_PROCEDURE_MCM,                       /* Mode Control module */

  RRC_PROCEDURE_IHO,                       /* Inter-RAT handover module */
  RRC_PROCEDURE_LLC,                       /* Lower Layer Controller */
  RRC_PROCEDURE_CHO,                       /* Cell Change Order */
  RRC_PROCEDURE_BMC,                       /* RRC support for BMC */
  RRC_PROCEDURE_CHK,                       /* RRC support for Counter Check procedure */ 
  RRC_PROCEDURE_MISC,                      /* RRC misc */
 RRC_PROCEDURE_GPS,
  RRC_PROCEDURE_COUEC,                    /*Change of UE Capability Procedure.*/ 
  RRC_PROCEDURE_DORMANCY,
    RRC_PROCEDURE_ENH_STATE,
  MAX_NUM_OF_PROCEDURES                    /* Total number of RRC Procedures. */
} rrc_proc_e_type;

/* Below #defines are added for logging purpose only */
 #define rrc_proc_e_type_value0 RRC_PROCEDURE_NONE,					  /* None RRC Procedure. */
 #define rrc_proc_e_type_value1 RRC_PROCEDURE_CSP, 					  /* Cell Selection Procedure. */
 #define rrc_proc_e_type_value2 RRC_PROCEDURE_CCM, 					  /* Cell Change Manager */
 #define rrc_proc_e_type_value3 RRC_PROCEDURE_SIB, 					  /* SIB Processing Procedure. */
 #define rrc_proc_e_type_value4 RRC_PROCEDURE_PG1, 					  /* Page Type 1 Procedure. */
 #define rrc_proc_e_type_value5 RRC_PROCEDURE_RCE, 					  /* RRC Connection Establishment 
											 Procedure. */
 #define rrc_proc_e_type_value6 RRC_PROCEDURE_RCR, 					  /* RRC Connection Release 
											 Procedure. */
 #define rrc_proc_e_type_value7 RRC_PROCEDURE_RCRE,					  /* RRC Connection Re-establishment
											 Procedure. */
 #define rrc_proc_e_type_value8 RRC_PROCEDURE_UECI,					  /* UE Capability Information 
											 Procedure. */
 #define rrc_proc_e_type_value9 RRC_PROCEDURE_UECE,					  /* UE Capability Enquiry 
											 Procedure. */
 #define rrc_proc_e_type_value10 RRC_PROCEDURE_IDT, 					  /* Initial Direct Transfer
											 Procedure. */
 #define rrc_proc_e_type_value11 RRC_PROCEDURE_UDT, 					  /* Uplink Direct Transfer
											 Procedure. */
 #define rrc_proc_e_type_value12 RRC_PROCEDURE_DDT, 					  /* Downlink Direct Transfer
											 Procedure. */
 #define rrc_proc_e_type_value13 RRC_PROCEDURE_PG2, 					  /* Paging Type 2 Procedure. */
 #define rrc_proc_e_type_value14 RRC_PROCEDURE_SMC, 					  /* Security Mode Control
											 Procedure. */
 #define rrc_proc_e_type_value15 RRC_PROCEDURE_SCR, 					  /* Signalling Connection Release
											 Procedure. */
 #define rrc_proc_e_type_value16 RRC_PROCEDURE_SCRR,					  /* Signalling Connection Release
											 Request Procedure. */
 #define rrc_proc_e_type_value17 RRC_PROCEDURE_CC,						  /* Counter Check Procedure. */
 #define rrc_proc_e_type_value18 RRC_PROCEDURE_RBE, 					  /* Radio Bearer Establishment
											 Procedure. */
 #define rrc_proc_e_type_value19 RRC_PROCEDURE_RBRC,					  /* Radio Bearer Re-configuration
											 Procedure. */
 #define rrc_proc_e_type_value20 RRC_PROCEDURE_RBR, 					  /* Radio Bearer Release 
											 Procedure. */
 #define rrc_proc_e_type_value21 RRC_PROCEDURE_TCR, 					  /* Transport Channel
											 Re-configuration Procedure. */
 #define rrc_proc_e_type_value22 RRC_PROCEDURE_TFC, 					  /* Transport Format Combination
											 Control Procedure. */
 #define rrc_proc_e_type_value23 RRC_PROCEDURE_PCR, 					  /* Physical Channel 
											 Re-configuration Procedure. */
 #define rrc_proc_e_type_value24 RRC_PROCEDURE_CU,						  /* Cell Update Procedure. */
 #define rrc_proc_e_type_value25 RRC_PROCEDURE_UU,						  /* URA Update Procedure. */
 #define rrc_proc_e_type_value26 RRC_PROCEDURE_UMI, 					  /* UTRAN Mobility Information
											 Procedure. */
 #define rrc_proc_e_type_value27 RRC_PROCEDURE_ASU, 					  /* Active Set Update Procedure. */
 #define rrc_proc_e_type_value28 RRC_PROCEDURE_ISHTU,					  /* Inter-System Handover to 
											 UTRAN. */
 #define rrc_proc_e_type_value29 RRC_PROCEDURE_ISHFU,					  /* Inter-System Handover from
											 UTRAN. */
 #define rrc_proc_e_type_value30 RRC_PROCEDURE_ISCTU,					  /* Inter-System Cell Reselection
											 to UTRAN. */
 #define rrc_proc_e_type_value31 RRC_PROCEDURE_ISCFU,					  /* Inter-System Cell Reselection
											 from UTRAN. */
 #define rrc_proc_e_type_value32 RRC_PROCEDURE_MCMR,					  /* Measurement Control and 
											 Measurement Report Procedure. */
 #define rrc_proc_e_type_value33 RRC_PROCEDURE_LBT, 					  /* Loopback Test module */

 #define rrc_proc_e_type_value34 RRC_PROCEDURE_MCM, 					  /* Mode Control module */
 #define rrc_proc_e_type_value35 RRC_PROCEDURE_IHO, 					  /* Inter-RAT handover module */
 #define rrc_proc_e_type_value36 RRC_PROCEDURE_LLC, 					  /* Lower Layer Controller */
 #define rrc_proc_e_type_value37 RRC_PROCEDURE_CHO, 					  /* Cell Change Order */
 #define rrc_proc_e_type_value38 RRC_PROCEDURE_BMC, 					  /* RRC support for BMC */
 #define rrc_proc_e_type_value39 RRC_PROCEDURE_CHK, 					  /* RRC support for Counter Check procedure */ 
 #define rrc_proc_e_type_value40 RRC_PROCEDURE_MISC,					  /* RRC misc */
 #define rrc_proc_e_type_value41 RRC_PROCEDURE_GPS,
 #define rrc_proc_e_type_value42 RRC_PROCEDURE_COUEC,					 /*Change of UE Capability Procedure.*/ 
 #define rrc_proc_e_type_value43 RRC_PROCEDURE_DORMANCY,
 #define rrc_proc_e_type_value44 RRC_PROCEDURE_ENH_STATE,
 #define rrc_proc_e_type_value45 MAX_NUM_OF_PROCEDURES	

/* This enumerates all Access Class indices used to access RRC Internal
 * Access Class barred list.
 */
typedef enum
{
  RRC_INT_ACCESS_CLASS_0_INDEX,
  RRC_INT_ACCESS_CLASS_1_INDEX,
  RRC_INT_ACCESS_CLASS_2_INDEX,
  RRC_INT_ACCESS_CLASS_3_INDEX,
  RRC_INT_ACCESS_CLASS_4_INDEX,
  RRC_INT_ACCESS_CLASS_5_INDEX,
  RRC_INT_ACCESS_CLASS_6_INDEX,
  RRC_INT_ACCESS_CLASS_7_INDEX,
  RRC_INT_ACCESS_CLASS_8_INDEX,
  RRC_INT_ACCESS_CLASS_9_INDEX,
  RRC_INT_ACCESS_CLASS_10_INDEX,
  RRC_INT_ACCESS_CLASS_11_INDEX,
  RRC_INT_ACCESS_CLASS_12_INDEX,
  RRC_INT_ACCESS_CLASS_13_INDEX,
  RRC_INT_ACCESS_CLASS_14_INDEX,
  RRC_INT_ACCESS_CLASS_15_INDEX,
  RRC_INT_ACCESS_CLASS_MAX_INDEX
}rrc_int_access_class_index_e_type;


/* -------------------------------------------------------------------------
** Enumerated type of RRC Mode
** ------------------------------------------------------------------------ */
typedef enum
{
    RRC_MODE_INACTIVE = 0,
    RRC_MODE_ACTIVE,
    RRC_MODE_STANDBY,
    RRC_MODE_INTERRAT_IN_PROGRESS,
    RRC_MODE_CHANGE,
    RRC_MODE_WILDCARD,
    RRC_MODE_IDLE_DETACH_IN_PROGRESS,  /*To handle the scenarios when going to IDLE detach is in progress*/
    RRC_MODE_IDLE_DETACHED,
    RRC_MODE_MAX
} rrc_mode_e_type;


/* -------------------------------------------------------------------------
** Enumerated type of RRC States
** ------------------------------------------------------------------------ */

typedef enum
{
  RRC_STATE_DISCONNECTED,
  RRC_STATE_CONNECTING,
  RRC_STATE_CELL_FACH,
  RRC_STATE_CELL_DCH,
  RRC_STATE_CELL_PCH,
  RRC_STATE_URA_PCH,
  RRC_STATE_WILDCARD,   /* This is used only for the purposes of state
                        change notification. When a procedure needs to
                        receive state change notification for a transition
                        from any state or to any state, it needs to use
                        this wild card state while registering with state
                        change manager. */
  RRC_STATE_MAX
}rrc_state_e_type;
/* Below #defines are just for logging purpose only */
#define rrc_state_e_type_value0 RRC_STATE_DISCONNECTED
#define rrc_state_e_type_value1 RRC_STATE_CONNECTING
#define rrc_state_e_type_value2 RRC_STATE_CELL_FACH
#define rrc_state_e_type_value3 RRC_STATE_CELL_DCH
#define rrc_state_e_type_value4 RRC_STATE_CELL_PCH
#define rrc_state_e_type_value5 RRC_STATE_URA_PCH
#define rrc_state_e_type_value6 RRC_STATE_WILDCARD
#define rrc_state_e_type_value7 RRC_STATE_MAX
typedef enum
{
  WRRC_METRIC_START,
  WRRC_METRIC_STOP,
  WRRC_METRIC_LOG,
  WRRC_METRIC_UPDATE,
  WRRC_METRIC_NONE
}wrrc_qsh_int_action_e_type;

/* Indicates whether we need to update UE Conn mode timers and consts */
typedef enum 
{
  RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_NOT_REQ,
  RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_REQ
} rrc_conn_timer_constants_update_status_e_type; 

/* Indicates type of RLC size list specification */
typedef enum
{
  RLC_SIZES_ALL,
  RLC_SIZES_CONFIGURED,
  RLC_SIZES_EXPLICIT_LIST,
  RLC_SIZES_UNCONFIGURED
} rrc_rlc_size_restriction_config_e_type;

/* Enumerated type of System Specific Capability Update Requirement */

typedef enum {
  RRC_GSM_CAP,
  RRC_GERANIU_CAP,
  RRC_EUTRA_CAP
} rrc_system_specific_cap_update_req_e_type;

/* Indicates generic success or failure from a function */
typedef enum
{
  RRC_STATUS_SUCCESS,
  RRC_STATUS_FAILURE
}rrc_status_e_type;


typedef enum
{
  RRC_MSG_VALID,
  RRC_MSG_UNSUPPORTED_CONFIG, /* Message has un-supported configuration  */
  RRC_MSG_PROTOCOL_ERROR,     /* Message has protocol error              */
  RRC_MSG_INVALID_CONFIG      /* Message has invalid configuration       */
} rrcllc_msg_failure_status_e_type;

/* Maximum number of Logical Channel Ids (in one direction) for a RAB.
This is used while informing a user-plane entity which logical channels
have been assigned to it's RAB. The number 3 is because AMR voice RABs
can have 3 logical channels in each direction */
#define RRC_MAX_LC_ID_PER_RAB 3

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/* This is Scrambling code representation in CSP */
typedef  uint16  rrc_scr_code_type;
/* This is frequency representation in CSP */
typedef  uint16  rrc_freq_type;

/* This defines the Cell ID as a binary number after decoding the 
 * link list format in ASN1
 */
typedef  uint32  rrc_int_cell_id_type;

typedef  uint32  rrc_int_csg_id_type;

typedef  uint32  rrc_int_c_rnti_type;

typedef  uint32  rrc_int_u_rnti_type;

typedef  uint32  rrc_int_u_rnti_short_type;
typedef  uint32  rrc_int_ura_id_type;

/* Data structure to store System Specific Capability Update 
   Requirement */ 
typedef struct  {
  uint8  count;                  /* Number of valid System Specific
                                     Capability Requirement Update Req */
  rrc_system_specific_cap_update_req_e_type value[16];
} rrc_system_specific_cap_update_req_list_type;

typedef struct
{
  boolean  barred[RRC_INT_ACCESS_CLASS_MAX_INDEX];
}rrc_int_access_class_barred_list_type;

typedef struct
{
  uint16 sdu_size;
  uint16 num_sdu_per_tti;
}rrc_cs_data_parms_type;


/* HSDPA Status Variable */
typedef enum
{
  HSDPA_INACTIVE,
  HSDPA_ACTIVE,
  HSDPA_SUSPEND
} rrc_hsdpa_status_enum_type;

/* MESSAGE VERSION. */
typedef enum
{
  MSG_REL99,
  MSG_REL5,
  MSG_REL6,
  MSG_REL7,
  MSG_REL8,
  MSG_REL9,
  MSG_REL10,
  MSG_INVALID
} rrc_msg_ver_enum_type;

#define RRC_DEBUG_CLEAR_ALL_STATS()    rrc_debug_clear_stats()

#define RRC_INVALID_RAB_ID (MAX_RAB_TO_SETUP+1) /* Invalid RAB id
                               used in ESTABLISHED_RABS to indicate
                               an unused RAB slot                    */ 

#define RRC_INVALID_RB_ID (MAX_RB+1) /* Invalid RAB id used in the
                               variable ESTABLISHED_RABS to indicate
                               an unused RB slot                     */

/* Enumerated type for status of a Radio Bearer                      */
typedef enum
{
  RRC_RB_STARTED,  /* Radio Bearer has been started (Default).       */
  RRC_RB_STOPPED   /* Radio Bearer has been stopped                  */
}rrc_rb_status_e_type;


/* Variable to hold a single RB that maps to a RAB                   */
typedef struct
{
  uint8 rb_id;            /* RB identity                             */
  uint32 subflow;         /* Subflow value for this RB               */
  rrc_rb_status_e_type rb_status; /* Status - started or stopped.    */

}rrc_rb_for_rab_type;

/* Variable to hold RAB Information. Note that this supports
only GSM_MAP RAB Ids now. When ASNI-41 RABs are supported
this needs to be expanded.                                           */
typedef struct
{
  boolean rab_id_is_gsm_map;       /* Set to TRUE if the RAB id is GSM_MAP */
  rrcrb_rab_e_type rab_type;      /* Type of RAB - PS, CS-DATA, CS-VOICE */ 
  rrc_rab_id_type rab_id;          /* GSM-MAP RAB Identity                 */
  rrc_cn_domain_identity_e_type cn_domain; /* CN domain for this RAB               */
  uint32 num_rbs_for_rab;          /* Number of RBs mapped to this RAB     */
  uint32 re_est_timer;  /* Re-est timer associated with RAB    */
  //NAS-Sync indicator may need to be stored later. It's not needed for now.
  rrc_rb_for_rab_type rb_for_rab[MAX_RB_PER_RAB];
  mvs_mode_type nas_Synchronisation_Indicator;  /* the codec type requested in OTA msg*/ 
  boolean is_cs_over_hs_rab;            /*To keep track whether the RAB in question is for CS over HSPA or not.*/
}rrc_est_rab_info_type;

/*following structure is used to return the rab info 
for a particular CN domain */
 typedef struct
{
  uint32 num_rabs;
  rrc_est_rab_info_type rabs[MAX_RAB_TO_SETUP]; /* Info for each RAB */
}rrc_established_rabs_for_a_domain_type;

/* Enumerated type to give results of searching for a RAB in
the variable ESTABLISHED_RABS                                        */
typedef enum 
{
  RRC_RAB_NOT_FOUND,
  RRC_RAB_FOUND
}rrc_rab_search_e_type;

/* Used in rlc_size_per_rb_struct_type to indicate DL RLC AM PDU size of DL RLC LI sIZE */
typedef enum 
{
  RRC_DL_AM_RLC,
  RRC_DL_UM_RLC,
  RRC_DL_INVALID_RLC
}rrc_rlc_dl_entity_e_type;

typedef enum 
{
  RRC_DL_UM_RLC_LI_SIZE_7, /* 7 bits */
  RRC_DL_UM_RLC_LI_SIZE_15, /* 15 Bits */
  RRC_DL_UM_INVALID_RLC
}rrc_rlc_dl_um_li_e_type;


/* Enumerated type to give results of searching for a RB in
the variable ESTABLISHED_RABS                                        */
typedef enum 
{
  RRC_RB_NOT_PRESENT,
  RRC_RB_PRESENT
}rrc_rb_search_e_type;

/* Enumerated type to give results of removal of an RB from 
the variable ESTABLISHED_RABS                                        */
typedef enum 
{
  RRC_RB_NOT_FOUND, /* The given RB was not found in ESTABLISHED_RABS */
  RRC_RB_REMOVED,   /* The given RB was removed. */
  RRC_RB_AND_RAB_REMOVED /* The RB was removed, and due to that a RAB
                         was also removed (it had no other RBs mapped
                         to it */
}rrc_rb_remove_e_type;

typedef enum
{
  RRC_CS_VOICE_OVER_DCH,
  RRC_CS_VOICE_OVER_HSPA,
  RRC_CS_VOICE_OVER_INVALID
} rrc_cs_voice_e_type;

/* Structure to hold results of removing an RB from ESTABLISHED_RABS */
typedef struct
{
  rrc_rb_remove_e_type result; /* Result of RB removal */
  rrc_rab_id_type rab_id;      /* GSM-MAP RAB Identity of the RAB
                               that was removed. Will be filled
                               only if the result is RRC_RB_AND_RAB_REMOVED */
  rrc_cn_domain_identity_e_type cn_domain; /* CN domain for this RAB. Will be filled
                               only if the result is RRC_RB_AND_RAB_REMOVED */
  rrcrb_rab_e_type rab_type; /*Voice, CS Data or PS RAB*/
  rrc_cs_voice_e_type voice_type;
}rrc_rb_remove_result_type;

/* A Structure to store the LC Ids for user-plane RABs.
When the user-plane entity needs to know which lc ids it
needs to register the watermarks it queries this module
*/
typedef struct
{
  uint32 rab_id;               /* RAB Id                                   */
  uint8 num_ul_lc_ids_for_rab; /* Number of uplink RLC ids for this RAB    */
  uint8 num_dl_lc_ids_for_rab; /* Number of downlink RLC ids for this RAB  */
  uint8 ul_lc_id[RRC_MAX_LC_ID_PER_RAB]; /* Uplink LC IDs                  */
  uint8 dl_lc_id[RRC_MAX_LC_ID_PER_RAB]; /* Uplink LC IDs                  */
}rrc_user_plane_lc_info_type;



/* Used to determine if the UARFCN passed to the function validate_dl_arfcn() is 
 * coming from WCDMA or GSM.
 */
typedef enum
{
  W_UARFCN_FROM_GSM_NBR_LIST,
  W_UARFCN_NOT_FROM_GSM_NBR_LIST
} w_uarfcn_source_e_type;


typedef struct
{
  uint8 dl_category;        /* max DL CAT supported by UE               */
  uint16 ul_category;        /* max UL CAT supported by UE               */
  uint32 dl_ded_rate;       /* max DL data rate supported by UE               */
  uint32 ul_ded_rate;       /* max UL data rate supported by UE               */
} rrc_data_rate_info_type;

typedef void RRC_MMAL_LOG_CB_TYPE(log_message_type *log_message);

/* Number of macro entries */
#define MAX_ACQ_DB_MACRO_ENTRIES 15

typedef struct
{
  uint16                 freq;
  sys_plmn_id_s_type     nas_plmn_id;
}acqdb_cell_info;

typedef struct
{
  acqdb_cell_info        cellInfoList[MAX_ACQ_DB_MACRO_ENTRIES];
  uint8                  num_valid_entries;
}wcdma_acqdb_cell_list;


/* This macro coverts the MCC from the array format in rrc_plmn_identity_type 
 * to the integer format.
 */
#define   RRC_CSP_GET_INT_MCC(plmn_id)  \
  ( plmn_id.mcc[0] * 100 +  plmn_id.mcc[1] * 10 + plmn_id.mcc[2] )


/* This macro coverts the MNC from the array format in rrc_plmn_identity_type
 * to the integer format.
 */
#define   RRC_CSP_GET_INT_MNC(plmn_id)  \
( \
  (plmn_id.num_mnc_digits == 2) ? \
    (plmn_id.mnc[0] * 10 + plmn_id.mnc[1]) : \
    (plmn_id.mnc[0] * 100 + plmn_id.mnc[1] * 10 + plmn_id.mnc[2]) \
)


/*===========================================================================
FUNCTION rrc_qxdm_set_hs_cat

DESCRIPTION
  This function sets the HSDPA category through QXDM command. 
  It also validates the CAT sent in QXDM command. 
  
DEPENDENCIES
  None
RETURN VALUE
  Boolean - TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/

boolean rrc_qxdm_set_hs_cat
(
  uint8 hs_cat
);

/*===========================================================================

FUNCTION  RRC_RETURN_LC_INFO_FOR_RAB

DESCRIPTION

  This function returns the logical channel ids for a given RAB.
  The calling function must initialize a structure of type
  rrc_user_plane_lc_info_type and fill in the RAB id before
  passing the pointer to this function. This function will fill
  in the LC ids for uplink and downlink for that RAB to the given
  pointer.
    
DEPENDENCIES

  Currently this is only supported for CS data RABs and PS RABs.
  This function must not be used for any other type of RABs at
  present.
 
RETURN VALUE

  A a value of type rrc_rab_status_e_type. RRC_RAB_NOT_FOUND is returned
  if the given RAB is not a valid RAB that has been setup by RRC. RRC_RAB_FOUND
  is returned if the RAB is valid.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_rab_search_e_type rrc_return_lc_info_for_rab
(
  rrc_user_plane_lc_info_type *ptr
);
/*====================================================================
FUNCTION: rrc_get_uplink_rab_rate

DESCRIPTION:
  This function returns the uplink rate of the RAB passed in

DEPENDENCIES:
  None

RETURN VALUE:
  uint32

SIDE EFFECTS:
====================================================================*/
extern uint32 rrc_get_uplink_rb_rate(uint8 lc_id);


/*====================================================================
FUNCTION: rrc_get_downlink_rab_rate

DESCRIPTION:
  This function returns the downlink rate of the RAB passed in

DEPENDENCIES:
  None

RETURN VALUE:
  uint32

SIDE EFFECTS:
====================================================================*/
extern uint32 rrc_get_downlink_rb_rate(uint8 lc_id);
/*===========================================================================

FUNCTION RRC_RETURN_CS_DATA_CALL_PARMS

DESCRIPTION

  This function fills in the SDU size and number of SDUs per TTI for the
  associated RAB id.
  
DEPENDENCIES


RETURN VALUE
  Returns FAILURE is the RAB id is not found.  Otherwise returns SUCCESS.
 

SIDE EFFECTS

  None

===========================================================================*/
rrc_status_e_type rrc_return_cs_data_call_parms
(    
  uint32                  rab_id,
  rrc_cs_data_parms_type *cs_data_parms_ptr
);

/*====================================================================
FUNCTION: is_wcdma_active

DESCRIPTION:
  This function returns true of W is active, meaning MCM rrc_mode
  is  returns FALSE otherwise

DEPENDENCIES:
  None

RETURN VALUE:
  uint32

SIDE EFFECTS:
====================================================================*/
extern boolean rrc_is_wcdma_active(void);
/*===========================================================================

FUNCTION RRC_GET_HSDPA_STATUS

DESCRIPTION

  This function returns the HSDPA Status Variable.
  
DEPENDENCIES


RETURN VALUE

  HSDPA Status 

SIDE EFFECTS

  None

===========================================================================*/

extern rrc_hsdpa_status_enum_type rrc_get_hsdpa_status
(
  void
);

/*===========================================================================
FUNCTION rrc_get_data_rate_for_ds

DESCRIPTION

  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern rrc_data_rate_info_type rrc_get_data_rate_for_ds
(
  void
);

/*===========================================================================

FUNCTION rrc_register_mmal_cb

DESCRIPTION
  This function is the API to register the function pointer for MMAL.

DEPENDENCIES


RETURN VALUE
  log_message_type

SIDE EFFECTS
  None
===========================================================================*/
void rrc_register_mmal_cb
(
  RRC_MMAL_LOG_CB_TYPE  *cb_ptr
);



/*===========================================================================

FUNCTION   RRC_FIND_RAB_FOR_RB_IN_EST_RABS

DESCRIPTION

  This function validates the rb_ids in the received RBR Message.
  For user plane RB ids it checks whether the rb
  is present in the established rbs. It returns the corresponding RAB id

DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_rab_id_type rab_id

SIDE EFFECTS

  None.

===========================================================================*/
rrc_rab_id_type rrc_find_rab_for_rb_in_est_rabs
(
  uint8 rb_id
);
/*============================================================================
FUNCTION: rrc_save_mvs()

DESCRIPTION:   
This function is called when WCDMA is no longer in control of MVS.
This function sets the vocoder is acquired by WCDMA flag.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void rrc_save_mvs
(
  void
);
/*============================================================================
FUNCTION: rrc_restore_mvs()

DESCRIPTION:   
This function will be called by gsvoice when WCDMA has got control over MVS. 
This function sets the mvs mapping info.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void rrc_restore_mvs
(
  void
);

/*===========================================================================
 FUNCTION     WCDMA_RRC_INIT_TCB

 DESCRIPTION
   This function populates the cached WCDMA RRC TCB pointer through rcinit. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
void wcdma_rrc_init_tcb(void);

/*===========================================================================
 FUNCTION:   WCDMA_RRC_GET_TCB

 DESCRIPTION:
   This function returns a pointer to the RRC TCB. 

 DEPENDENCIES:
   None.

 PARAMETERS:
   None.

 RETURN VALUE:
   A pointer to the RRC TCB.

 SIDE EFFECTS:
   An assertion will fail if the lookup fails.
===========================================================================*/
rex_tcb_type* wcdma_rrc_get_tcb(void);

/*===========================================================================

FUNCTION rrc_update_codec_info_to_cm

DESCRIPTION
Sends the codec information to CM


DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrc_update_codec_info_to_cm
(
  void
);

/*===========================================================================
 FUNCTION     rrc_check_gsm_bands_supported

 DESCRIPTION
   This function checks if device supports GSM bands

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   boolean

 SIDE EFFECTS
   None

===========================================================================*/

boolean rrc_check_gsm_bands_supported (void);

/*===========================================================================
 FUNCTION     rrc_get_minimum_t304_reqd

 DESCRIPTION
   This functions gets the minimum T304 required for successful transmission of UECI under good radio conditions

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   uint32.

 SIDE EFFECTS
   None

===========================================================================*/

uint32 rrc_get_minimum_t304_reqd(uint16 sdu_length);


/*===========================================================================
 FUNCTION     rrc_get_num_pdu

 DESCRIPTION
   This functions calculates the number of PDUs that will be transmitted by RLC for a single UECI SDU

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   uint32.

 SIDE EFFECTS
   None

===========================================================================*/

uint32 rrc_get_num_pdu(uint16 sdu_length);


/*===========================================================================
 FUNCTION     rrc_get_tti_configured

 DESCRIPTION
   This functions gets the tti configured in milli sec

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   uint32.

 SIDE EFFECTS
   None

===========================================================================*/

uint32 rrc_get_tti_configured (void);

/*===========================================================================
   FUNCTION     rrc_check_if_nw_support_fd
  
   DESCRIPTION
       This function takes as_id as argument and internally calls 
	   rrcdormancy_check_if_nw_support_fast_dormancy()
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     sys_modem_as_id_e_type
  
   RETURN VALUE
     boolean 
  
   SIDE EFFECTS
  
  ===========================================================================*/

boolean rrc_check_if_nw_support_fd( sys_modem_as_id_e_type wrrc_as_id);

/*===========================================================================

FUNCTION           WCDMA_RRC_GET_ACQDB_CELL_LIST

DESCRIPTION        Fills the passed LTE ptr with valid macro cell entries stored in W AcqDB.

DEPENDENCIES       None

RETURN VALUE       void

SIDE EFFECTS       None
===========================================================================*/
void wcdma_rrc_get_acqdb_cell_list
(
 wcdma_acqdb_cell_list  *wcdma_acqdb_cell_list_ptr
);
#endif /* RRCDATA_H */
