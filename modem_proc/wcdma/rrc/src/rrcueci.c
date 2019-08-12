/*===========================================================================
                   R R C  U E  C A P A B I L I T Y  I N F O R M A T I O N

DESCRIPTION
  This header file contains function prototypes for UE Capability Information
  procedure

EXTERNALIZED FUNCTIONS
  rrcueci_procedure_event_handler
    This function is the event handler for all events that are sent to the
    IUE Capability Information procedure.

  rrcueci_init_procedure
    This function initializes required parameters for UE Capability Information
    procedure.



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

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcueci.c_v   1.19   16 May 2002 13:27:16   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcueci.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   sn      Changes for LTE U feature
06/06/16   ad      Made changes to log important parms from OTAs
06/06/16   sp      Made changes for Mav debug snapshot
05/06/16   sg      Made changes to reduce F3s
05/11/16   vs      Changes to cleanup unnecessary f3s
12/22/15   sr      Changes to decouple TDD PSHO NV from TDD CM measurements
12/03/15   vi      Made changes to avoid sending fach meas ind to WL1 in multisim mode
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
07/08/15   bc      Made changes to control WTOL MFBI using LTE MFBI NV.
07/01/15   sp      Made changes to save procedure substates for LTE commands and to skip logging frequent internal commands
02/19/15   aa      Made changes to start T304 timer with 100ms if the timer expires for N304th timer during a legacy tuneaway
01/21/15   sa      Made changes for QTF with CRM
11/10/14   sp      Made changes to save procedure substates in crash debug info
11/03/14   ad      Made changes to control different DB DC band combinations through NV
11/06/14   vi      Made changes to avoid linker errors after disabling LTE
07/15/14   bc      Made changes to indicate support of EGSM and PGSM as mentioned in the spec.
10/22/14   gv      Made changes to have connected mode feature set independent of TA status 
09/29/14   sg      Made changes to check for upper layer band capability
                   also when indicating DBDC capability
08/13/14   vi      Made changes to send UECI if N/W requests for second time within the same connection
08/04/14   bc      Made changes to fix compiler warnings.
07/30/14   sa      Made changes to correct the Rel-10 AS Indicator in UE Cap info msg
07/25/14   sg      Made changes to support band II for 
                   single carrier 3C
07/21/14   as      Made changes to check-in F3 reduction chnages
07/15/14   ac      NV reorganization
07/07/14   bc      Made changes to indicate LTE MFBI support to network in connection setup message.
06/09/14   sa      Made changes to support 16QAM
05/20/14   rmn     Made changes to save DS Tuneaway Status Change indication in connected state based on the concurrency NV
03/13/14   bc      Made changes to trim the UE capability message  by indicating
                   the CM support only once instead of indicating for all the W bands
                   separately.
05/14/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/06/14   geg     Use safer versions of memcpy() and memmove()
18/04/14   ad      Made changes to adaptively calcualte T304 for GCF TCs
04/16/14   sp      Made changes to check Mac-I nv to enable DCHSUPA nv
04/11/14   vi      Made changes to disable FE_FACH capability in DSDS/DSDA mode.
04/10/14   db      Made changes to enable/disable feature support in DSDS/DSDA/TSTS modes through NV
04/02/14   db      Made changes to memset memory allocated for ue_RadioAccessCapabBandFDDList4 capability 
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/26/14   sr      Decouple Meas id extension capability from DC HSUPA
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
09/25/13   ad      Made changes to split NV control of FDPCH and CPC DTX/DRX features
01/37/14   sr      Fixed compilation errors
01/27/14   sr      Added NV control for MFBI support
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
01/03/14   sr      Added code changes for MFBI support
01/03/14   sa      Made changes to correct the edch_PhysicalLayerCategory print
12/30/13   bc      Made changes to optimize REL 10 Spec CRs code
12/24/13   db      Introduced new NV to allow CPC,HSFACH,HSRACH,DC-HSDPA in DSDS and DSDA mode
12/12/13   sr     Made Changes so that W extracts LTE band prefrences from message WCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ  
12/09/13   sp      Made changes for OSYS 6.6.5 migration
12/03/13   sg      Made changes to update review comments for 3C 
11/19/13  sg     Made changes to not disable DBDC if tuneaway is enabled.
11/18/13   bc      Checked in REL 10 spec crs changes
11/15/13   sp      Added Nv support for boost feature
11/14/13   sa      DC feature support will be controlled thru UE Cat
10/30/13   as      Made changes to add feature code for FEATURE_WCDMA_3C_HSDPA
10/29/13   sp      Made changes to fix compilation errors for BOOST feature 
10/17/13   ad      Changes for DC HSDPA feature with 3C interface
10/07/13   rmn     Changes for Feature: RAT Search Order for FDD/TDD LTE
09/24/13   vi      Added changes for FE-FACH
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
09/02/13   vi      Fixed Kw error
08/28/13   vi      Added nv check for GANSS feature
08/26/13   ad      Made changes to fix KW issue for boundary checks
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
08/08/13   sp      Made changes to correct the reference to rfm_wcdma_dual_band
08/05/13   sa      Made the changes to check prim and sec bands against the BAND1 and BAND8
07/29/13   sr      Adding missed changes for DSM interface
07/30/13   ad      Added code changes to support DB-DC feature
07/29/13   sr      Enhance X2L capability enquiry interface to use DSM for W-RRC
07/08/13   sn      Changes for LTE CSG feature
06/22/13   sr      Added code changes to manage More than 16 EUTRA Freqs in Pre-R11 WCDMA
06/20/13   vi      Made change to allocate memory from rtxMemHeapAlloc for embedded pointer in function rrcueci_append_lte_specific_capability()
06/07/13   vi      Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
05/20/13   sn      Changes to consider W CSG feature as disabled if there is 
                   no whitelist update from NAS
05/28/13   sr      Added code for measurement identity extension
05/22/13   sa      Added code changes to support FEATURE_WCDMA_DC_HSUPA
05/13/13   vi      Added code for GANSS under feature FEATURE_GANSS_SUPPORT
05/15/13   db      Made changes to enhance NV 67333 to control FDPCH and CPC DTX/DRX features independently
05/13/13   gv      Added code to support multi-SIM API extensions between WCDMA RRC and GSM RR
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
03/22/13   mp      Removed tmc.h include file
03/18/13   vi      Added check for WTOL_TDD_PSHO_SUPPORT_NV before setting compressedMode = TRUE for LTE TDD
03/13/13   sn      Changes to filter out LTE and W bands in FDD3 list
03/05/13   sn      Changes to filter out bands for all types of UE capability messages
01/03/13   ad      Made changes to reduce the redundant F3s
12/12/12   gv      Fixed compilation issue with HS RACH
12/07/12   sn      Changes to populate CSG capability in IDT and conn. setup complete
11/30/12   ad      Made changes to remvoe the dependancy on  rfm_band_supports_dual_carrier
10/23/12   pm      Made changes to correct enable HSRACH NV only when MACI and HSFACH
                   NVs are also ON
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
09/28/12   pm      Added NV support for MAC_I
08/25/12   sa      Made the changes for HSRACH NV support
08/22/12   db      Additional changes to disable EFDPCH through NV
07/06/12   sn      Changes to filter out bands properly
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/03/12   sks     Made changes to support NV item for wtol_TDD ps_ho,
                   and added support for LTE TDD CM measurements.
04/06/12   ad      Changes for HS-RACH feature
05/10/12   md      Added code changes to support FEATURE_MAC_I
05/02/12   rd      Made changes to support NV item for wtol_ps_ho
04/02/12   db      Made changes to disable CPC and FDPCH through NV
02/23/12   md      Made changes to set nv_hsdpa_category to valid value '24'
                   if it is set to invalid value with DC-HSDPA support .
02/23/12   sks     Added support for WTOL PSHO.
01/02/12   pm      Added rrc_wtol_cm_support_nv to put FEATURE_WCDMA_CM_LTE_SEARCH feature under nv check 
12/21/11   md      Made changes to set hsdpa category to default value 14 if modem deosn't support
                   UE CAT above 14.
11/09/11   md      Made changes to update cell-specific STTD support based on
                   UE HSDPA category and  DC support of the current band 
10/26/11   ad      Made changes to include BandFDD_extension_indicator in v370ext only if number of FDD bands
                   are zero and Number of FDD2 bands are non-zero.
10/10/11   ad      Made changes to include BandFDD_extension_indicator in v370ext in UE capabilies.
                   only if number of FDD bands is zero
09/08/11   as      Added E-DRX changes for R8
09/05/11   gv      Fixed KW errors
08/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.103.00
20/07/11   as      Made changes to fill correct L1 params for EFACH-DRX
7/04/11    sks      corrected FDD2 bitmask setting 
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/08/11   as      Made changes to not report CPC capability if Tuneaway is active
06/14/11   sks     Fixed compilation warning.
06/07/11   rl      Fixed compiler error
05/27/11   ad      Fixed bugs in OSYS code
05/25/11   sks     Made changes to fix bugs when for indicating TDD support.
05/23/11   sks     Made changes to update FGI for TDD only builds to indicate support for
                   PCH reselections.
05/20/11   sks     Fixed compilation warning.
05/18/11   rl      Fixed the compiler error
05/17/11   sks     Made changes to correctly populate LTE TDD capabilities in OTA messages to n/w.
05/11/11   sks     Fixed compilation error.
05/11/11   sks     Added a counter of 3 to delay crashing if LTE capability response is not received by RRC.
05/10/11   sks     Removed unnessary code when filling up capability info message.
05/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.97.50
05/05/11   vg      added channal locking feature support under rrccsp_channel_locking_nv_status
                   condition
05/03/11   rl      Fixed the lint error
05/03/11   rl      Moved the macro RRC_MAX_SYS_CAPABILITY to header file.
05/02/11   sb      Compilation error fix. Defined macro RRC_MAX_SYS_CAPABILITY
04/29/11   sks     Added code to send EUTRA capabilities when requested by n/w in 
                   UE capability enquiry message.
04/27/11   sks     Added source rat when requesing eutra capabilities from LRRC.
04/25/11   rl      Fixed the KW error
03/28/11   rl      Merged the HS-FACH/PCH changes from mainline
03/17/11   rl      Fixed the compiler errors/warning after enabling 
                   FEATURE_WCDMA_CM_LTE_SEARCH
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/07/11   su      Added code to support device type NV.
03/10/11   rl      Fixed the compiler warning
02/11/11   rl      Fixed the compiler warning
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/22/10   rm      Made changes to indicate PCH UE pos capability
11/19/10   prk     Indicate e-fach capability if UE supports e-fach, irrespective
                   of network support.
11/11/10   as      Fixed compiler errors for MIMO
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/04/10   su      Corrected the Typo from FEATURE_WCMDA_COMBINED_MIMO_AND_64QAM
                   to FEATURE_WCDMA_COMBINED_MIMO_AND_64QAM
11/02/10   sks     Fixed KW errors.
11/01/10   rm      Added code to support W2L measurements in DCH state under FEATURE_WCDMA_CM_LTE_SEARCH
10/28/10   su      Added prints to check support for DC-HSDPA.
10/13/10   su      Changed RRC_WCMDA_COMBINED_MIMO_AND_64QAM to 
                   FEATURE_WCMDA_COMBINED_MIMO_AND_64QAM
10/11/10   ss      ASN.1 migration to ASN1 9.3.0
10/08/10   su      Added code to support RRC_WCMDA_COMBINED_MIMO_AND_64QAM
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
09/27/10   kp      Made changes to not indicate I.E rrc_doesNotBenefitFromBatteryConsumptionOptimisation 
                   in UE cap if feature CPC-DTX is defined for thin UI builds, as some of the NW look at 
                   this I.E disiable CPC-DTX feature for this UE.
09/21/10   sks     Made changes to fix memory leak issue when filling ue radio access capability list.
09/20/10   prk/kp  Added code to support FEATURE_WCDMA_HS_FACH
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/10/10   rm      Made changes to enable RAT specific capability bit_mask only when it was requested by N/W
09/08/10   rm      Made changes to send the EUTRA specific capabilities to N/W
                   only when it was requested by N/W
08/24/10   rm      Fixed compiler warnings
08/11/10   ss      Added code to fill supportForFDPCH only in Rel7 and fixed compilation errors.
08/11/10   ss      Added code changes to fill supportForFDPCH only in Rel7
07/23/10   ss      Added support to fill dc-hsdpa IE after checking rf support on current camped band
07/05/10   rm      Fixed compiler warnings
07/02/10   rm      Made changes to report intra UTRA absolute priority reselection support
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/22/10   kp      Fixed compilation errors
06/16/10   kp      Added code to indicate UE support of feature “Two DRX schemes in 
                   URA_PCH and CELL_PCH” and “E-DPDCH power interpolation formula”
                   for Rel-7 & above UE's.   
06/07/10   rm      Made changes to not get GPS capability when FEATURE_WCDMA_BLACKBIRD_BRINGUP
                   is defined
06/03/10   gkg     Made changes to implement Spec CR3955 "Carrier-specific STTD configuration 
                   for DC-HSDPA". Changes are under feature FEATURE_WCDMA_DC_HSDPA_DIFF_TXD_MODE
05/18/10   prk     Hsdpa cat. nv item is forcibly set to default value when it 
                   is greater than 12 and FEATURE_HSPA_PLUS_CAT10 is defined.
05/12/10   gkg     Added support for sending Tx Diversity for DL Control Channels
                   for MIMO Devices i.e. IE supportofTxDivOnNonMIMOChannel.
04/07/10   gkg     ASN.1 migration to 8.a.0
03/17/10   as      Resolved Errors due to ASN.1 8.9 Migration
03/15/10   as      Updated RRC Code to support ASN.1 8.9 version
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
03/02/10   as      Fixed Klockwork issues, Added CLASSMARK3_DATA_MAX_LENGTH constant
02/26/10   gkg     Removed the dependency of advertisement of F-DPCH capability on GCF flag.
02/26/10   kp      Removed NV GCF dependency for Snow3g security algo feature 
02/01/10   gkg     Removed the reference to L1 function hs_is_supported(). Instead,
                   rely on feature HSDPA only while filling HS capabilities.
01/20/10   gkg     Made changes to correct the HSDPA CAT when both MIMO and 64QAM
                   features are defined. In that case, advertise CAT as 18.
01/11/10   gkg     Made changes so that HSDPA CAT 24 is not advertised if HSDPA CAT
                   isn't set to 21-24 thru NV.
12/22/09   gkg     Made changes for CSoHS so that NV 6878 controls CSoHS operation.
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/04/09   rm      Made changes to set the default WTOG NACC capability to TRUE when NVs
                   NV_GERAN_FEATURE_PACK_1_I, NV_INTERRAT_NACC_SUPPORT_I are not set
11/25/09   ss      Added Release Independent Band (FDD2) support.
11/20/09   dm      Fixed compiler warning
11/05/09   rm      Added code to derive the WTOG NACC capability by reading the NVs
                   NV_GERAN_FEATURE_PACK_1_I, NV_INTERRAT_NACC_SUPPORT_I
11/04/09   gkg     Made changes to send R8 in UECI OTA when UE release is set to R8.
10/30/09   as      Fixed klockwork warnings
10/07/09   gkg\kp  Changes for REL8 under FEATURE_WCDMA_REL8 and CPC-DTX
09/03/09   ps      Made changes for EDPCCH power Boost under 
                   FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
05/29/09   kp      Added support for new SNOW3g security Algo & single cipher algo usage across 
                   access stratum layers.Code changes are under FEATURE_WCDMA_SNOW3G_SECURITY
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/08/09   ps      Made changes such that by default UE advertises  
                   MIMO/64QAM +CM capbility as supported
                   i.e setting of dummy2 as FALSE
03/19/09   ps      Made changes to enable/disable advertising of MIMO/64QAM +CM
                   capbility through global variable rrc_mimo_64qam_cm_enabled.
                   by default MIMO/64QAM +CM is advertised as not capable with 
                   rrc_mimo_64qam_cm_enabled set as TRUE.
02/27/09   ps      Moved the code to enable FDPCH cability if 
                   nv_gcf_test_flag is TRUE to RCE
02/19/09   ps      Made changes that if FEATURE_THIN_UI is defined in builds for 
                   data cards, then advertise that UE does not benefit from battery
                   savings.Spec Cr 2677(RP-050484)
02/16/09   ps      Made changes to enable FDPCH cability if 
                   nv_gcf_test_flag is TRUE.
02/06/09   rm      Made changes to set supportOfHandoverToGAN to rrc_supportOfHandoverToGAN_present,
                   and supportOfUTRAN_ToGERAN_NACC to TRUE only if dual mode is enabled 
01/30/09   ps      Made changes to not fill 6b0NonCriticalExtensions if
                   FDPCH is not defined and NV is set as REL6.if FDPCH 
                   is not defined,it should be filled only if NV is REL7 
01/28/09   dm      Fixed lint errors
01/19/09   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
01/09/09   ps      Made changes to advertise FDPCH capbility if 
                   rrc_fdpch_capability_enable is set as TRUE
12/29/08   rm      Added support for 3G<->GAN Handover
12/11/08   ps      Made changes for EF : Constraint violation seen due to the
                   ue_RadioAccessCapability_v6e0ext bitmask not initialised
                   when FDPCH is not enabled
12/08/08   ps      Made changes to set the MAC-HS(R5/R6 HS-DSCH )CAT 
                   according to global variable "rrc_machs_cat" when the 
                   HSDPA CAT is above 12 for R7
11/21/08   ps      Made temp changes to send compressed capability as FALSE for the 
                   other WCDMA and GSM frequencies
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
08/29/08   kp      Getting code for spec CR 2880 out of FEATURE_WCDMA_REL6, since there 
                   is no backward compatibility issue with R5/R99.
07/04/08   pk      Aviod sending radio access capabilities of the UE twice in regular
                   and extension IEs.
05/26/08   ps      Made changes to send RLC Buffer size for AM as 500KB for REl6 UE
27/03/08   pk      Added support for feature FEATURE_IRAT_HO_INFO_COMPRESSED.
03/10/08   kp      Added support for SIB11bis. Code changes are under feature flag 
                   FEATURE.SIB11BIS.SUPPORT 
03/06/08   da      Made changes required to migrate to 09.2007 R6 version of
                   ASN.1
03/06/08   kp      Bugfix to not carry transation i.d in UECI message when 
                   UECI initiated due to UE Capability changed & accept any 
                   transaction I.D in UECI Confirmation message.
11/29/07   vk      Added F-DPCH capabilities in rrcueci_append_ue_radio_access_capability_later_than_r3()
                   under FEATURE_WCDMA_FDPCH
10/23/07   pk      Added support for sending radio access capability extensions in
                   connection setup complete and UECI messages, for UEs with only IMT2k
                   support apart from any of the GSM bands 850/900E/900P.
                   For details see R6 25.331 CR 2880.
10/15/07   vr      Added changes to limit the number of advertised bands in InterRAT Handover Info
                   message to RRC_UECI_MAX_FDD_BANDS_FOR_IHO. This is due to the ASN1 limitation 
                   on the container size of Radio Access Capability IE which is currently 64 
                   octets.Choosing more than RRC_UECI_MAX_FDD_BANDS_FOR_IHO will cause an 
                   ASN1 constraint violation during encoding of InterRAT Handover Info message.
                   This fixes CR 124001.
10/08/07   da      Added change to set dummy IE for HSUPA+CM indication based on
                   NV item.
07/11/07   vr      Added a feature flag for code that advertises support for 384k on 
                   20ms tti. The code is now under feature FEATURE_DISABLE_384K_ON_20MS_TTI_UE_CAPABILITY.
                   Only if the feature is not defined, UE will advertise the support for 
                   384k on 20 ms tti.
06/05/07   sm      Removed setting of dummy2 to TRUE for F-DPCH
05/25/07   vg      added support for FEATURE_CGPS and FEATURE_CGPS_UMTS_CP_WCDMA.
04/20/07   da      Made changes needed to update ASN.1 version to R6 sept R6 2006
03/26/07   vr      Support for populating FDD2 bands in InterRAT Handover Info (Classmark)
                   message. In the earlier versions of ASN1, there was no provision 
                   to populate FDD2 bands in this message, hence these fields were not populated
                   for IHO case alone. In the recent versions container IE has been added 
                   in InterRAT Handover Info message that can carry FDD2 information.
03/12/07   kp      Replacing NV variable nv_ciphering_enabled  with new NV variable 
                   nv_ciphering_enabled_status which will from now on keep track of 
                   exact status of Ciphering NV item.
02/26/07   sm      Added code to set dummy and dummy2 IEs in v590 non-critical extn
                   for early R6 UE to indicate un-supported features F-DPCH and CM+HSUPA
02/13/07   vr      Fixed memory leak issue for FDD2. Memory allocation for fdd2 IEs were 
                   happening outside REL-5 feature flag but the encoding (and freeing) was
                   done inside Rel-5 feature flag. Fix is to make sure the memory allocation
                   is done only if UE is REL-5 or higher. This fixes CR 110883
02/08/07   vr      Fixed lint error to ignore return value of gs_read_coded_classmark_3()
                   function. The function's prototype was changed in NAS and hence this 
                   lint error showed up. Ignoring the return value for the time being
                   as it doesnt seem very useful to RRC. 
01/31/07   vr      Set max bits per transport block to 8960 for 6280 and above targets.
                   Changes under FEATURE 6280 COMPILE. This is to advertise support
                   384k rate on 20ms tti
12/21/06   sm      Fixed 7200 lint error
12/07/06   sm      Changed code to indicate release-6 in access stratum indicator
                   IE of UE Capability information as per NV and feature definition
10/27/06   sgk     Initialized rlc_Capability_r5_ext bit mask to 0. 
10/24/06   da      Changed the max ROHC context sessions advertized in PDCP
                   capability from 1024 to 4.
10/20/06   sm      Added changes to remove padded number of bits from the length
                   included in CONTAINING IEs
10/13/06   sgk     Added 7600 support under feature
                   FEATURE_UMTS_1X_HANDOVER_UMTSMSM
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
08/29/06   sgk     Made changes to turn on HSDPA in UE capability onlf if l1 
                   returns hs supported. 
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
07/07/06   sgk     Fixed lint errors.
06/21/06   vr      Support to indicate rx_tx_TimeDifferenceType2 Capability
06/14/06   kc      Rolled back changes for honoring trans id from later than r3 release
                   struct in UECI-Confirm message.
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
05/12/06   vk      Added code to change HSDPA Capabilities to CAT8 under
                   FEATURE_6280_HSDPA_PHASE2_CAPABILITY
05/08/06   vr      Fixed lint warnings
05/02/06   kc      Added support to compare trans id from later than r3 release in Information
                   Confirm message
05/02/06   tkk     Added support to send UECI capabilities to UTRAN on mode change 
                   from single mode to dual mode.
05/01/06   vr      Perform security capability check even if NV Ciphering is turned off
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
03/17/06   vr      Fixed a bug while appending Extension Indicator to the FDD Band List
                   Even if no FDD bands are selected,allocate memory for the 
                   rrc_CompressedModeMeasCapabFDDList since it is a mandatory parameter  
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
03/07/06   vr      Added new functions to populate FDD2 bands and extensions appropriately
                   Added support to advertize FDD2 bands in UECI message
03/03/06   vk      Included rrcnv.h to remove compilation warning under FEATURE HSDPA
03/03/06   da      Removed the setting of accessStratumReleaseIndicator for saber builds
                   as accessStratumReleaseIndicator is removed from UECI 
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/22/06   vk      Implemented function rrcueci_get_nv_items() and defined a new
                   global variable nv_hsdpa_category. Also set the HSDPA
                   category as per NV variable nv_hsdpa_category in function
                   rrcueci_append_ue_radio_access_capability_r5(). Changes are 
                   under FEATURE HSDPA
02/01/06   vr      Set WTOG NACC supported flag to TRUE if NACC feature is defined
01/25/06   bd      Lint clean-up
12/01/05   vk      Updated UE Capabilities for higher data rates for HSDPA under
                   feature FEATURE_6280_HSDPA_PHASE1_CAPABILITY 
11/10/05   sm      Added changes to enable switching between Rel99 and Rel5 using nv_item
11/03/05   vk      Set RLC Buffer size to 300kb in rlc_Capability_r5_ext
08/02/05   kc      Lint fixes for GCF_NV flag and code within HSDPA feature.
07/07/05   vr      Do not support Dynamic Power Class feature if GCF flag is turned ON
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
03/02/05  ttl      Support RRC ASN1 Release 5.
12/22/04   sm      Fixed Lint errors
12/17/04   vr      Added support for WCDMA -> 1x handover feature - Append CDMA cap info
                   to the UECI message
12/03/04   kc      Added support for updating correct GSM-UE-Band groups.
12/03/04   bu      Added the MSG_HIGHs related to Dynamic power class feature under
                   appropriate feature definition.
12/02/04   sm      Added MSG_HIGHs
11/23/04   sm      Support for dynamic power class feature
11/16/04   vk      Use fn lsm_wcdma_ue_pos_capability_get() instead of lsm_ue_pos_capability_get()
                   since the original function has now been split into 2 fns[One for GSM
                   and one for WCDMA]
10/20/04   sm      Fix to advertise all the FDD bands correctly in compressed mode measurement
                   capability while sending ue capability extension to network.
09/17/04   kc      Added support for multi-band UE-capability advertisement
09/14/04   ttl     Given new default value of ueci_am_entity for March, 2004 Rel99.
09/13/04   ttl     Give maximumAM_EntityNumber the appropriate number.
08/30/04   sgk     Assigned the right enum values for network_assisted_gps_support
                   in function rrcueci_append_ue_radio_access_capability
                   to fix lint error Type mismatch (assignment) (enum/enum).
                   Typecast parameters to function rrc_log_protocol_error to
                   uint8 in function rrcueci_process_ueci_wait_for_t304_to_expire
                   to fix lint error Converting enum to int.
07/07/04   kc      Fix power_class to 3 correctly for both ue-cap and ue-cap-ext.
                   Correct check for Null-Ptr after mem-allocation.
06/22/04   kc      Changed maxbitsReceived to 6400 from 5120.  Also corrected
                   maxTFC for UL to 64 from 128
06/11/04   kc      Changed total-rlc-am-buffer-size to 150k
05/25/04   kc      Advertise maxDPDCH bits transmitted to 9600 from 2400 to 
                   support UL384kbits.
05/22/04   kc      Fix power_class integer value to 4 from 3, since the range is 
                   1..4 and not 0..3
03/22/04   kc      Added checks for Null_Ptr after allocation.  Also added flag
                   to clear measCapability bitmask before using it.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
03/11/04   kc      Modified UE capabilities to increase the UL data rate from 64k
                   to 384k.
01/26/04   vk      Modified UE Capabilities to indicate that positioning meas
                   is supported under FEATURE_MMGPS
11/26/03   kc      Changed maximum number of Uplink transmitted transport blocks 
                   to 8 from 16  for TTI's that start at same time.
11/06/03   vk      Modified UE Capabilities to indicate that FDD Measurements
                   using compressed mode in uplink and downlink are supported
07/22/03   vk      Modified UE Capabilities to indicate that FDD Measurements
                   using compressed mode in uplink and downlink are not supported
07/06/03   rj      Modified UE capabilities to limit data rates to 384K and 64K
                   respectively to  Downlink and Uplink.
07/03/03   kc      Set transaction id in RRC STATUS msg. correctly when Critical
                   extensions are received.
05/30/03   rj      Modified interface to function
                   rrcueci_verify_inter_rat_security_capability.
05/21/03   rj      Fixed a bug while restarting the timer T304 when invalid
                   UE Capability Informationn Confirm message is received.
04/03/03   kc      Added check for Integrity NV Flag.
04/02/03   rj      Changed F3 messages.
04/02/03   rj      Added code to provide APIs rrcueci_verify_security_capability
                   and rrcueci_verify_inter_rat_security_capability.
02/05/03   sk      March2002Rel specific changes 
                   Removed   ics_Version from UE_radioAccessCapability 
                   Added  accessStratumReleaseIndicator to UE_radioAccessCapability 
                   Changed  gps_ReferenceTimeCapable
                   to  supportForUE_GPS_TimingOfCellFrames
                   Changed  maxNumberOfTFC_InTFCS to  maxNumberOfTFC
                   Changed rrc_MaxNumberOfTFC_InTFCS_DL_tfc128
                   to  rrc_MaxNumberOfTFC_DL_tfc128
                   Changed turboDecodingSupport to turboEncodingSupport for
                   rrc_UL_TransChCapability 
                   Changed  rrc_fdd to   rrc_MultiModeCapability_fdd
02/03/03   kc      Removed lint errors
12/02/02   rj      Added code to support both UEA0 and UEA1 algorithms when
                   Ciphering is enabled.
11/26/02   xfg     Added support for 3GPP CR1714 regarding classmark 3. The 
                   changes were made under FEATURE_3GPPCR1714_CLASSMARK.
11/13/02   xfg     Added support for GSM multi-band feature
10/31/02   rj      Fixed supported Max. no of Transport Blocks in the uplink.
10/29/02   xfg     Fix a typo when calling rrcmcm_is_dualmode_enabled().
10/28/02   rj      Updated code to fix UE Capability Information message 
                   re-transmissions as per the latest Specification. Also 
                   Updated code to support UEA1 algorithm if Ciphering
                   NV Item is enabled.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
09/26/02   xfg     Set IEI and length for GSM classmark2 and classmark 3
09/17/02   rj      Added code to set that DCS1800 and GSM1900 measurements are
                   not supported.
09/12/02   rj      Added a check to know whether GSM is supported before 
                   appending System Specific Capabilities. Also renamed 
                   feature FEATURE DUALMODE BASELINE to 
                   FEATURE INTERRAT HANDOVER WTOG and modified Radio Access 
                   Capabilities to support GSM900 measurements.
08/16/02   bu      Removed void in the call to rrcmcm_is_dualmode_enabled().
08/16/02   rj      Updated function rrcueci_append_gsm_specific_capability for
                   latest APIs for Classmark information
08/15/02   xfg     Fixed a typo: libproto.h to libprot.h
08/07/02   rj      Added code to honor UE Capability Enquiry message with 
                   System Specific Capability Update Request for single
                   mode operation. And UE responds with UE Capability
                   Information message without IE "UE system specific capability".
08/06/02   rj      Added code to append GSM system Specific UE Capabilities
                   for the dual mode.
05/16/02   rj      Corrected mismatch while registering for State Change
                   Notification.
04/09/02   kc      Removed KASUMI_SUPPORTED defn. here and defined it in
                   rrcsmc.h file, as it is more appropriate in SMC proc.
02/14/02   rj      Updated code for new Cell Update procedure interface.
02/07/02   kc      Changed access functions for T304 and N304. Now we use
                   macros RRC_GET_N304 and RRC_GET_T304.
11/28/01   kc      Code changes for Dec->June Spec conversion.
10/25/01   rj      Modified code to add current state to RRC_CHANNEL_CONFIG_REQ
                   command.
10/23/01   kc      Initialized uninitialized vars to remove compile time warnings
10/17/01   rj      Updated rrcueci_append_ue_radio_access_capability function
                   with fixes for Security mode capability. Added more comments.
                   Added code to register for state transitions and removed code
                   that initializes RB Ids in RRC_CHANNEL_CONFIG_REQ while bringing
                   down RRC Connection.
10/09/01   rj      Updated functions rrcueci_get_t304_from_serv_cell_db and
                   rrcueci_get_n304_from_serv_cell_db to check for presence
                   of T304 and N304 parameters before reading them.
09/19/01   rj      Modified F3 message text to maximum length of 40 charecters.
09/15/01   rj      Updated UE Uplink and Downlink Transport channel Capabilities
                   in function rrcueci_append_ue_radio_access_capability. 
09/14/01   rj      Modified function rrcueci_send_ue_capability_info_msg to
                   check for RLC Id before allocating memory for uplink
                   message.Added define constants for KASUMI supported and 
                   Encryption not supported.
09/14/01   kc      Added code to handle protocol error logging
09/03/01   rj      Removed include file rrcasnutil.h since it is no longer 
                   required after moving send_rrc_status_message to SEND CHAIN.
08/30/01   rj      Added code to implement Peeking OTA message and update
                   function rrcueci_process_ueci_wait_for_t304_to_expire to 
                   address ASN1 decoding failure. Changed 
                   UE_MODE_ACKNOWLEDGED_DATA_PLANE to UE_MODE_ACKNOWLEDGED_DATA.
08/10/01   rj      Modified MSG_LOW to MSG_HIGH when OTA message is transmitted
                   or received.
08/06/01   rj      Updated code in function rrcueci_process_ueci_initial_substate
                   for latest command structure for RRC_CELL_UPDATE_INITIATE_REQ.
07/27/01   rj      Updated function rrcueci_process_ueci_wait_for_t304_to_expire
                   with uecomdef.h define constants and fixed a typos.
                   Added code to update the release cause in case of radio link 
                   failure.
06/05/01   rj      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "rex.h"
#include "rrccmd_v.h"              
#include "rrcuece.h"
#include "rrcueci.h"
#include "rrcsmc.h"
#include "msg.h"
#include "err.h"
#include "rrcasn1.h"
#include "rrcdata_v.h"
#include "rrclcm.h"
#include "rrcsend.h"
#include "rrcscmgr.h"
#include "rrctmr.h"
#include "rrccu.h"
#include "rrcsibdb.h"
#include "uecomdef.h"
#include "libprot.h"
#include "rrcmcm.h"
#include "sys_stru.h"
#include "rrccsp.h"
#include "rrcrcr.h"
#include "l1rrcif.h"
#include "rrcasn1util.h"
#include "mm_umts.h"
#include "sys_plmn.h"
#include "rr_rrc_if.h"
#ifdef FEATURE_WCDMA_BAND_SPEC_DC_HSDPA
#include "rfm_wcdma.h"
#include "rrcccm.h"
#endif

#ifdef FEATURE_GSM_MULTIBAND
#include "rr_multiband.h"
#endif

#include "rrcnv.h"

#include "rrclogging.h"

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif


#include "rrcmisc.h"


#include "bit.h"

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
#include <lte_rrc_ext_api.h>
#endif

#ifdef FEATURE_WCDMA_HS_RACH
#include "rrchsrachcfg.h"
#endif

#include "stringl.h"

//Added for mcs API fun prototypes
#include "trm.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define RRCUECI_INVALID_TRANSACTION_ID 0xFF

#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
/*A variable used to track support of snow3g_security_algo support, by default set to FALSE*/
boolean    snow3g_security_algo_supported = FALSE;
#endif

/*Boolean added to track if interfrequency and interrat measurements are to be processed or not*/
extern boolean process_interfreq_interrat_meas_rel7;

extern boolean process_interfreq_uei_meas_rel7;

extern boolean nv_gcf_test_flag;

/* Boolean that indicates measurement id extension is supported or not */
extern boolean rrc_meas_id_extn_support;

#ifdef FEATURE_UMTS_PDCP
extern boolean rrc_pdcp_enabled;
#endif /* FEATURE_UMTS_PDCP */

uint8 nv_hsdpa_category;

/*Setting the default MAC-HS cat as 8 when R7 is defined.*/
uint8 rrc_machs_cat = 10;

/*Various HSDPA Categories
13, 14 - 64QAM only
15, 16 - MIMO Only
17, 18 - Both 64QAM and MIMO supported but configured at a time
19, 20 - Both 64QAM and MIMO supported. And can be configured both
21, 22, 23, 24 - DC-HSDPA supported
*/
/*RRC_GKG: Will make changes for MIMO/64AQM etc validations.*/
#if defined(FEATURE_WCDMA_COMBINED_MIMO_AND_64QAM)
uint8 rrc_macehs_cat = 20;
#elif defined(FEATURE_WCDMA_MIMO)
uint8 rrc_macehs_cat = 18;
#else
uint8 rrc_macehs_cat = 14;
#endif


boolean nv_wtog_nacc_enabled = FALSE;
#define RRC_WTOG_NACC_SUPPORTED 0x01

#ifdef FEATURE_CGPS_UMTS_CP_WCDMA 
/* defined in rrcgpsmeas.c */
extern rrcgps_cgps_ue_pos_capability_cb_type  rrcgps_cgps_ue_pos_capability_cb;
#endif

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA) 
/*UE position measurement validity in PCH states*/
boolean rrc_ue_pos_cap_pch = FALSE;
#endif


boolean rrc_mimo_64qam_cm_enabled = FALSE;
#ifdef FEATURE_GANSS_SUPPORT
extern boolean rrc_ganss_supported;
extern boolean rrc_ganss_support_nv;
#endif
#define CLASSMARK3_DATA_MAX_LENGTH 14 /* Referenced from structure classmark_3_ie_T in sys_stru.h */

#ifdef FEATURE_MAC_I
extern uint16 rrc_feature_nv_item_value;
#endif /* FEATURE_MAC_I */

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
extern uint8 rrc_fe_fach_support_nv;
#endif

#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR
extern boolean rrc_mfbi_support_nv;
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/*==========================================================================
  Type for defining the substates of UE Capability Information Procedure.
===========================================================================*/

typedef enum 
{
  RRCUECI_INITIAL,               /* UECI initial substate      */
  RRCUECI_WAIT_FOR_CELL_UPDATE_COMP_CNF,
                                 /* UECI waits for CELL UPDATE
                                    procedure to complete */
  RRCUECI_WAIT_FOR_T304_TO_EXPIRE 
                                 /* UECI waits for T304 to expire */
} rrcueci_substate_e_type;
/* Below #defines are added for logging purpose only */
#define rrcueci_substate_e_type_value0 RRCUECI_INITIAL
#define rrcueci_substate_e_type_value1 RRCUECI_WAIT_FOR_CELL_UPDATE_COMP_CNF
#define rrcueci_substate_e_type_value2 RRCUECI_WAIT_FOR_T304_TO_EXPIRE 

#ifdef FEATURE_WCDMA_TO_LTE
typedef struct
{
  boolean lte_req_posted;
  boolean lte_rsp_received;

  uint32 cap_cont_length;
  byte *cap_cont_ptr;
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type  lte_camped_band_mask;  
  sys_lte_band_mask_e_type  lte_nghbr_band_mask;
#else
  sys_band_mask_e_type  lte_camped_band_mask;  
  sys_band_mask_e_type  lte_nghbr_band_mask;
#endif
} rrcueci_interrat_lte_capability_info_type;
#endif


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
uint16 ueci_message_length = 0;
extern PLMN_id_T irat_plmn_id;


#ifdef LTE_BAND_NUM
extern  sys_lte_band_mask_e_type rrcueci_sib19_band_mask;
#else
extern sys_band_mask_type rrcueci_sib19_band_mask;
#endif
/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* This variable holds substate for UE Capability Information Procedure
 */
LOCAL rrcueci_substate_e_type rrcueci_substate;

LOCAL rrc_RRC_TransactionIdentifier rrcueci_ue_cap_enquiry_trans_id;
                                      /* This stores the transaction Id 
                                         for UE Capability Enquiry message */

#ifdef FEATURE_WCDMA_REL7_SPECRS
LOCAL  rrc_ue_capability_originator_e_type rrcueci_ue_cap_originator;                                   
#endif

#ifdef FEATURE_HSPA_PLUS_CAT29

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 29

#elif defined FEATURE_HSPA_PLUS_CAT24

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 24

#elif defined FEATURE_HSPA_PLUS_CAT20

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 20

#elif defined FEATURE_HSPA_PLUS_CAT18

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 18

#elif defined (FEATURE_HSPA_PLUS_CAT16) 

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 16

#elif defined (FEATURE_HSPA_PLUS_CAT14)

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 14

#elif defined (FEATURE_HSPA_PLUS_CAT10)

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 10

#elif defined (FEATURE_HSPA_PLUS_CAT9)

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 9

#elif defined (FEATURE_HSPA_PLUS_CAT8)

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 8

#elif defined (FEATURE_HSPA_PLUS_CAT6)

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 6
#else

#define RRC_WCMDA_HSDPA_DEFAULT_CATEGORY 6
#endif

#ifdef FEATURE_WCDMA_TO_LTE
rrcueci_interrat_lte_capability_info_type rrcueci_interrat_lte_capability_info;
#endif


#ifdef FEATURE_WCDMA_DB_DC_HSDPA
/*Global structure to store the number of DB-DC band combinations supported*/
rrcueci_db_dc_band_storage_type db_dc_band_comb_list;
#endif
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================
FUNCTION rrcueci_reset_ueci_procedure

DESCRIPTION
  This resets the UECI procedure i.e. stops T304 timer, reset the value of variables
  used in this procedure to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_reset_ueci_procedure(void)
{
  /* Set conter V304 to zero */
  rrc_v_304 = 0;

  /* Stop T304 timer - no need to check whether this timer is running or not because its
       taken care inside this function 
  */
  rrctmr_stop_timer(RRCTMR_T_304_TIMER);

  /* Reset the UECI state machine */
  rrcueci_substate = RRCUECI_INITIAL;

#ifdef FEATURE_WCDMA_REL7_SPECRS
  rrcueci_ue_cap_originator = RRC_UE_CAPABILITY_ORIGINATOR_NONE;
#endif
#ifdef FEATURE_WCDMA_TO_LTE
  rrcueci_interrat_lte_capability_info.lte_req_posted = FALSE;

  /* Free the container and reset lte_rsp_received only on moving to idle */
  if(rrc_get_state() == RRC_STATE_DISCONNECTED)
  {
    rrcueci_interrat_lte_capability_info.lte_rsp_received = FALSE;
    /* Freeing the capability contianer received from LTE */
    if(rrcueci_interrat_lte_capability_info.cap_cont_ptr)
    {
      rrc_free(rrcueci_interrat_lte_capability_info.cap_cont_ptr);
      rrcueci_interrat_lte_capability_info.cap_cont_ptr = NULL;
    }
  }

#endif
}

/*===========================================================================
FUNCTION rrcueci_get_support_am_entity

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE                        h
  None

SIDE EFFECTS
  None
===========================================================================*/
static rrc_MaximumAM_EntityNumberRLC_Cap rrcueci_get_support_am_entity(void)
{
  rrc_MaximumAM_EntityNumberRLC_Cap ueci_am_entity = rrc_MaximumAM_EntityNumberRLC_Cap_dummy;
  uint8  support_am_entity = UE_MAX_AM_ENTITY;

  switch (support_am_entity)
  {
    case UE_MAX_NUM_OF_AM_ENTITIES_4:
      ueci_am_entity = rrc_MaximumAM_EntityNumberRLC_Cap_am4;
      break;
    case UE_MAX_NUM_OF_AM_ENTITIES_5: 
      ueci_am_entity = rrc_MaximumAM_EntityNumberRLC_Cap_am5;
      break;
    case UE_MAX_NUM_OF_AM_ENTITIES_6: 
      ueci_am_entity = rrc_MaximumAM_EntityNumberRLC_Cap_am6;
      break;
    case UE_MAX_NUM_OF_AM_ENTITIES_7:
    case UE_MAX_NUM_OF_AM_ENTITIES_8:
      ueci_am_entity = rrc_MaximumAM_EntityNumberRLC_Cap_am8;
      break;
    default:
      {
        ueci_am_entity = (support_am_entity <= UE_MAX_NUM_OF_AM_ENTITIES_16) ? rrc_MaximumAM_EntityNumberRLC_Cap_am16 : rrc_MaximumAM_EntityNumberRLC_Cap_am30;
        break;
      }
  }
  
  return ueci_am_entity;
}

/*===========================================================================
FUNCTION rrcueci_verify_security_capability

DESCRIPTION
  This function checks the Security Capability.
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if check is successful. Otherwise returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcueci_verify_security_capability
( 
  rrc_SecurityCapability * securityCapability
)
{
  boolean status = TRUE;  /* local varibale for status */
  uint32 int_algo_bit_mask = 0;
  uint32 cipher_algo_bit_mask = 0;
  
  /*lint -save -e778 */
  WRRC_MSG2_HIGH("nv_ciphering_enabled_status %d, nv_integrity_enabled %d", nv_ciphering_enabled_status, nv_integrity_enabled);
  
  if (nv_ciphering_enabled_status) 
  {
    cipher_algo_bit_mask = UEA_1_SUPPORTED + UEA_0_SUPPORTED;
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
    if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && snow3g_security_algo_supported)
    {
      cipher_algo_bit_mask += UEA_2_SUPPORTED;
    }
#endif
    /*lint -save -e572 */
    if ( (securityCapability->cipheringAlgorithmCap.numbits != 16) ||
         ( *(securityCapability->cipheringAlgorithmCap.data) !=
            (uint8)( (cipher_algo_bit_mask & 0xFF00) >> 8)) || 
         ( *(securityCapability->cipheringAlgorithmCap.data+1) != 
            (uint8)( cipher_algo_bit_mask & 0xFF) ) )
    /*lint -restore */
    {
      WRRC_MSG0_HIGH("NV Ciph ON.Security Cap. Check Failed");
      status = FALSE;
    }

  }
  else
  {
    /*lint -save -e572 */
    if ( (securityCapability->cipheringAlgorithmCap.numbits != 16) ||
         ( *(securityCapability->cipheringAlgorithmCap.data) !=
            ((UEA_0_SUPPORTED & 0xFF00) >> 8)) || 
         ( *(securityCapability->cipheringAlgorithmCap.data+1) != 
            (UEA_0_SUPPORTED & 0xFF)) )
    /*lint -restore */
    {
      WRRC_MSG0_HIGH("NV Ciph OFF. Security Cap. Check Failed");
      status = FALSE;
    }

  }
  if( (nv_integrity_enabled) && (status != FALSE) )
  { /*lint -save -e572 */
    int_algo_bit_mask = KASUMI_SUPPORTED;
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
    if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && snow3g_security_algo_supported)
    {
      int_algo_bit_mask += SNOW_3G_SUPPORTED;
    }
#endif
    if ( (securityCapability->integrityProtectionAlgorithmCap.numbits != 16) ||
         ( *(securityCapability->integrityProtectionAlgorithmCap.data) !=
            (uint8)( (int_algo_bit_mask & 0xFF00) >> 8)) || 
         ( *(securityCapability->integrityProtectionAlgorithmCap.data+1) != 
            (uint8)( int_algo_bit_mask & 0xFF) ) )
    /*lint -restore */
    {
      WRRC_MSG0_HIGH("Security Cap. Check Failed");
      status = FALSE;

    }

  }
  /*lint -restore */
  return (status);
}

/*===========================================================================
FUNCTION rrcueci_verify_inter_rat_security_capability

DESCRIPTION
  This function checks the GSM Specific Capability.
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if check is successful. Otherwise returns FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcueci_verify_inter_rat_security_capability
(
  rrc_InterRAT_UE_SecurityCapList * ue_SystemSpecificSecurityCap
)
/*lint -e715*/
{
  /* This dummy fucntion */
  return TRUE;
}

/*===========================================================================

FUNCTION        RR_RRC_IS_SUPPORTED_BAND_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
static boolean rr_rrc_is_supported_band_internal( rr_internal_band_T band)
{
#ifdef FEATURE_DUAL_SIM
  return  rr_ds_is_supported_band(band, rrc_get_as_id());
#else
  return  rr_is_supported_band(band);
#endif
}

/*lint +e715*/
/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_ext

DESCRIPTION
  This function appends the Radio Access Capability to the procedure which
  sends Inter-RAT HO Transfer
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_append_ue_radio_access_capability_ext
(
  rrc_UE_RadioAccessCapabBandFDD * fdd_info, /* Pointer to UE Radio Access
                                                  Capability Extension  */
  rrc_RadioFrequencyBandFDD freq_band,          /* freq band to populate */
  const rrcueci_band_storage_type *ueci_band_ptr     /* pointer to get supported FDD freq bands */
#ifdef FEATURE_LTE_TO_WCDMA
  ,rrcueci_internal_cap_update_cause_e_type update_cause  /*Remove the interRAT cell list only for HO*/
#endif
)
{
  /* temporary storage for gsm meas cap */
  rrc_CompressedModeMeasCapabGSMList * temp_gsm_list = NULL;

  rrc_CompressedModeMeasCapabFDDList  *temp_fdd_meas_cap_list_ptr = NULL;
  rrc_RadioFrequencyBandGSM rf_band_gsm[MAXNO_BANDS] = {rrc_RadioFrequencyBandGSM_gsm450};   /* Initialization required to satisfy LINT */

  uint8   num_gsm_bands = 0; //to store total number of gsm bands info that UE has to broadcast
  uint8   index = 0;  // for indexing purpose

#ifdef FEATURE_LTE_TO_WCDMA
uint8 num_gsm_bands_to_report = 0;
#endif

  RRC_RESET_MSG_IE_PRESENT_PTR(fdd_info);
  RRC_MSG_COMMON_SET_BITMASK_IE_PTR(fdd_info,rrc_UE_RadioAccessCapabBandFDD,fddRF_Capability);

  /* Suports UE Powerclass rrc_class3 (24dBm) */
  fdd_info->fddRF_Capability.ue_PowerClass = rrc_UE_PowerClassExt_class3;

  MSG_HIGH("Send UE power class: %d (%d->class1..%d->class4)",
      fdd_info->fddRF_Capability.ue_PowerClass ,rrc_UE_PowerClassExt_class1,rrc_UE_PowerClassExt_class4);

  /* TX/RX Frequency separation 190 Mhz */
  fdd_info->fddRF_Capability.txRxFrequencySeparation = rrc_TxRxFrequencySeparation_default_TxRx_separation;

  fdd_info->radioFrequencyBandFDD = freq_band;

  //set the bitmask to 0's, in case dualmode flag is set to false, we do not append
  //gsmMeasCapabGSMList, so ossEncoded should potentially not encode gsmCapabList
  RRC_RESET_MSG_IE_PRESENT(fdd_info->measurementCapability);

  /* Update FDD Measurement Capability.
   */  
  temp_fdd_meas_cap_list_ptr = &fdd_info->measurementCapability.compressedModeMeasCapabFDDList;
  temp_fdd_meas_cap_list_ptr->n = ueci_band_ptr->num_supported_bands;

  ALLOC_ASN1ARRAY2(&enc_ctxt,temp_fdd_meas_cap_list_ptr,rrc_CompressedModeMeasCapabFDD);

  for(index=0; index<ueci_band_ptr->num_supported_bands; index++)
  {
    
    RRC_MSG_COMMON_SET_BITMASK_IE(temp_fdd_meas_cap_list_ptr->elem[index],
    rrc_CompressedModeMeasCapabFDD,radioFrequencyBandFDD);

    //set the FDD Band appropriately
    temp_fdd_meas_cap_list_ptr->elem[index].radioFrequencyBandFDD =  
                   (rrc_RadioFrequencyBandFDD) ueci_band_ptr->band_type[index];
    if(((process_interfreq_interrat_meas_rel7 == FALSE) && 
         (process_interfreq_uei_meas_rel7 == FALSE)) ||
         ((rrccsp_channel_locking_nv_status()) && !rrccsp_is_ue_in_true_limited_service()))
    {
      /*Temp: L1 doesnt support Compressed mode at present */
      temp_fdd_meas_cap_list_ptr->elem[index].dl_MeasurementsFDD = FALSE;
      temp_fdd_meas_cap_list_ptr->elem[index].ul_MeasurementsFDD = FALSE;
    }
    else
    {
      temp_fdd_meas_cap_list_ptr->elem[index].dl_MeasurementsFDD = TRUE;
      temp_fdd_meas_cap_list_ptr->elem[index].ul_MeasurementsFDD = TRUE;
    }
  }

  //Check if UE is dualmode enabled and indicate GSM Measurement capabilities.
  if (rrcmcm_is_dualmode_enabled_no_f3())
  {
    // Initialize number of GSM bands to 0
    num_gsm_bands = 0;

    //query rr for 900E band support
    if(rr_rrc_is_supported_band_internal(BAND_EGSM))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm900E;
    }

    //query rr for 900P band support
    else if(rr_rrc_is_supported_band_internal(BAND_PGSM))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm900P;
    }

    //query rr for DCS band support
    if(rr_rrc_is_supported_band_internal(BAND_DCS))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm1800;
    }

    //query rr for PCS band support
    if(rr_rrc_is_supported_band_internal(BAND_PCS))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm1900;
    }

    //query rr for GSM 850
    if(rr_rrc_is_supported_band_internal(BAND_GSM_850 ))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm850;
    }

#ifdef FEATURE_LTE_TO_WCDMA
    /*If for HO INFO, try removing some or all interRAT bands, arrived at these values based on trail and error */
    if(update_cause ==RRCUECI_INT_CAP_UPDATE_1900_MULTI_BAND)
    {
      switch(ueci_band_ptr->num_supported_bands)
      {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
          num_gsm_bands_to_report = num_gsm_bands;
          break;
        case 5:
          num_gsm_bands_to_report = 2;
          break;
        default:
          num_gsm_bands_to_report = 0;
          break;
      }
      if(num_gsm_bands_to_report < num_gsm_bands)
      {
        num_gsm_bands = num_gsm_bands_to_report;
        WRRC_MSG1_HIGH("Set number of GSM bands to report to %d",num_gsm_bands);
      }
    }
#endif

    if(num_gsm_bands !=0)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE(fdd_info->measurementCapability,rrc_MeasurementCapabilityExt,compressedModeMeasCapabGSMList);

      temp_gsm_list = &fdd_info->measurementCapability.compressedModeMeasCapabGSMList;
      temp_gsm_list->n = num_gsm_bands;

      ALLOC_ASN1ARRAY2(&enc_ctxt,temp_gsm_list,rrc_CompressedModeMeasCapabGSM);

      for(index=0; index<num_gsm_bands; index++)
      {
        //set the GSM Band appropriately
        temp_gsm_list->elem[index].radioFrequencyBandGSM = rf_band_gsm[index];
        if((process_interfreq_interrat_meas_rel7 == FALSE) ||
          ((rrccsp_channel_locking_nv_status()) && !rrccsp_is_ue_in_true_limited_service()))
        {
          /*Temp: L1 doesnt support Compressed mode at present */
          temp_gsm_list->elem[index].dl_MeasurementsGSM = FALSE;
          temp_gsm_list->elem[index].ul_MeasurementsGSM = FALSE;
        }
        else
        {
          temp_gsm_list->elem[index].dl_MeasurementsGSM = TRUE;
          temp_gsm_list->elem[index].ul_MeasurementsGSM = TRUE;
        }
      }
    }
  }
}

/*===========================================================================
FUNCTION RRCUECI_APPEND_UE_RADIO_ACCESS_CAPABILITY_FDD2_EXT

DESCRIPTION
  This function appends the Radio Access Capability to the procedure which
  sends Inter-RAT HO Transfer
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcueci_append_ue_radio_access_capability_fdd2_ext
(
  rrc_UE_RadioAccessCapabBandFDD2 * fdd2_info, /* Pointer to UE Radio Access Capability Extension  */
  rrc_RadioFrequencyBandFDD2 fdd2_freq_band,     /* freq band to populate */
  const rrcueci_band_storage_type *ueci_band_ptr /* pointer to get supported FDD freq bands */
)
{
  /* temporary storage for gsm meas cap */
  rrc_CompressedModeMeasCapabGSMList * temp_gsm_list = NULL;
  /* Initialization required to satisfy LINT */
  rrc_RadioFrequencyBandGSM rf_band_gsm[MAXNO_BANDS] = {rrc_RadioFrequencyBandGSM_gsm450};  
  /* To store total number of gsm bands info that UE has to broadcast */
  uint8   num_gsm_bands = 0; 
  uint32 index = 0,idx=0;
  rrc_CompressedModeMeasCapabFDDList2 * temp_fdd2_meas_cap_list_ptr = NULL;

  /* Set the frequency band */
  fdd2_info->radioFrequencyBandFDD2 = fdd2_freq_band;

  /* Set the RFCapability of the freq band */
  RRC_MSG_COMMON_SET_BITMASK_IE_PTR(fdd2_info,
  rrc_UE_RadioAccessCapabBandFDD2,fddRF_Capability);

  /* Suports UE Powerclass rrc_class3 (24dBm) */
  fdd2_info->fddRF_Capability.ue_PowerClass = rrc_UE_PowerClassExt_class3;

  MSG_HIGH("Send UE power class: %d (%d->class1..%d->class4)",
      fdd2_info->fddRF_Capability.ue_PowerClass ,rrc_UE_PowerClassExt_class1,rrc_UE_PowerClassExt_class4);

  /* TX/RX Frequency separation 190 Mhz */
  fdd2_info->fddRF_Capability.txRxFrequencySeparation = 
  rrc_TxRxFrequencySeparation_default_TxRx_separation;

  /* Set the bitmask to 0's, in case dualmode flag is set to false, we do not append
     gsmMeasCapabGSMList, so ossEncoded should potentially not encode gsmCapabList */
  RRC_RESET_MSG_IE_PRESENT(fdd2_info->measurementCapability2);

  /* Initialise the compressed mode ptr */
  temp_fdd2_meas_cap_list_ptr = &fdd2_info->measurementCapability2.compressedModeMeasCapabFDDList;
  temp_fdd2_meas_cap_list_ptr->n =  ueci_band_ptr->num_supported_bands + ueci_band_ptr->num_supported_bands_fdd2;
  ALLOC_ASN1ARRAY2(&enc_ctxt,temp_fdd2_meas_cap_list_ptr,rrc_CompressedModeMeasCapabFDD2);

  /* Update FDD Measurement Capability.*/  
  for(index = 0; index < ueci_band_ptr->num_supported_bands; index++)
  {

    // Set the bitmask to indicate that this entry is a FDD band 
    RRC_MSG_COMMON_SET_BITMASK_IE(temp_fdd2_meas_cap_list_ptr->elem[idx],
    rrc_CompressedModeMeasCapabFDD2,radioFrequencyBandFDD);

    // Set the FDD Band appropriately 
    temp_fdd2_meas_cap_list_ptr->elem[idx].radioFrequencyBandFDD = 
        (rrc_RadioFrequencyBandFDD) ueci_band_ptr->band_type[index];
    if(((process_interfreq_interrat_meas_rel7 == FALSE)&&
     (process_interfreq_uei_meas_rel7 == FALSE)) || 
     ((rrccsp_channel_locking_nv_status()) && !rrccsp_is_ue_in_true_limited_service()))
    {
      /*Temp: L1 doesnt support Compressed mode at present */
      temp_fdd2_meas_cap_list_ptr->elem[idx].dl_MeasurementsFDD = FALSE;
      temp_fdd2_meas_cap_list_ptr->elem[idx].ul_MeasurementsFDD = FALSE;
    }
    else
    {
      temp_fdd2_meas_cap_list_ptr->elem[idx].dl_MeasurementsFDD = TRUE;
      temp_fdd2_meas_cap_list_ptr->elem[idx].ul_MeasurementsFDD = TRUE;
    }
    idx++;

  }
 
  /* Update FDD2 Measurement Capability.*/  
  for(index = 0; index < ueci_band_ptr->num_supported_bands_fdd2; index++)
  {
    

    RRC_MSG_COMMON_SET_BITMASK_IE(temp_fdd2_meas_cap_list_ptr->elem[idx],
  rrc_CompressedModeMeasCapabFDD2,radioFrequencyBandFDD2);

    //set the FDD2 Band appropriately
    temp_fdd2_meas_cap_list_ptr->elem[idx].radioFrequencyBandFDD2 = 
    (rrc_RadioFrequencyBandFDD2) ueci_band_ptr->band_type_fdd2[index];
    if(((process_interfreq_interrat_meas_rel7 == FALSE)&&
        (process_interfreq_uei_meas_rel7 == FALSE)) ||
        ((rrccsp_channel_locking_nv_status()) && !rrccsp_is_ue_in_true_limited_service()))
    {
      /*Temp: L1 doesnt support Compressed mode at present */
      temp_fdd2_meas_cap_list_ptr->elem[idx].dl_MeasurementsFDD = FALSE;
      temp_fdd2_meas_cap_list_ptr->elem[idx].ul_MeasurementsFDD = FALSE;
    }
    else
    {
      temp_fdd2_meas_cap_list_ptr->elem[idx].dl_MeasurementsFDD = TRUE;
      temp_fdd2_meas_cap_list_ptr->elem[idx].ul_MeasurementsFDD = TRUE;
    }
    idx++;
  }

  

  /* Check if UE is dualmode enabled and indicate GSM Measurement capabilities */
  if (rrcmcm_is_dualmode_enabled_no_f3())
  {
    RRC_MSG_COMMON_SET_BITMASK_IE(fdd2_info->measurementCapability2,
      rrc_MeasurementCapabilityExt2,compressedModeMeasCapabGSMList);

    // Initialize number of GSM bands to 0
    num_gsm_bands = 0;

    //query rr for 900E band support
    if(rr_rrc_is_supported_band_internal(BAND_EGSM))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm900E;
    }

    //query rr for 900P band support
    else if(rr_rrc_is_supported_band_internal(BAND_PGSM))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm900P;
    }

    //query rr for DCS band support
    if(rr_rrc_is_supported_band_internal(BAND_DCS))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm1800;
    }

    //query rr for PCS band support
    if(rr_rrc_is_supported_band_internal(BAND_PCS))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm1900;
    }

    //query rr for GSM 850
    if(rr_rrc_is_supported_band_internal(BAND_GSM_850 ))
    {
      rf_band_gsm[num_gsm_bands++] = rrc_RadioFrequencyBandGSM_gsm850;
    }

    temp_gsm_list = &fdd2_info->measurementCapability2.compressedModeMeasCapabGSMList;
    temp_gsm_list->n =num_gsm_bands;

    ALLOC_ASN1ARRAY2(&enc_ctxt,temp_gsm_list,rrc_CompressedModeMeasCapabGSM);
          
    for(index = 0; index < num_gsm_bands; index++)
    {
      //set the GSM Band appropriately
      temp_gsm_list->elem[index].radioFrequencyBandGSM = rf_band_gsm[index];
      if((process_interfreq_interrat_meas_rel7 == FALSE) ||
        ((rrccsp_channel_locking_nv_status()) && !rrccsp_is_ue_in_true_limited_service()))
      {
        /*Temp: L1 doesnt support Compressed mode at present */
        temp_gsm_list->elem[index].dl_MeasurementsGSM = FALSE;
        temp_gsm_list->elem[index].ul_MeasurementsGSM = FALSE;
      }
      else
      {
        temp_gsm_list->elem[index].dl_MeasurementsGSM = TRUE;
        temp_gsm_list->elem[index].ul_MeasurementsGSM = TRUE;
      }

     }
  }
}

/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_ext

DESCRIPTION
  This function appends the Radio Access Capability to the procedure which
  sends Inter-RAT HO Transfer
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcueci_append_ue_radio_access_capability_fdd_ext
(
  rrc_UE_RadioAccessCapabBandFDD_ext *  fdd_info_ext,  /* Pointer to UE Radio Access Capability Extension  */
  rrc_RadioFrequencyBandFDD freq_band,                  /* freq band to populate */
  const rrcueci_band_storage_type *ueci_band_ptr        /* pointer to get supported FDD freq bands */
)
{
  rrc_CompressedModeMeasCapabFDDList_ext *temp_fdd_meas_cap_list_ext_ptr = NULL;
  
  uint8   index = 0;  // for indexing purpose

  fdd_info_ext->radioFrequencyBandFDD = freq_band;
  temp_fdd_meas_cap_list_ext_ptr = &fdd_info_ext->compressedModeMeasCapabFDDList_ext;
  temp_fdd_meas_cap_list_ext_ptr->n = ueci_band_ptr->num_supported_bands_fdd2;
  ALLOC_ASN1ARRAY2(&enc_ctxt,temp_fdd_meas_cap_list_ext_ptr,rrc_CompressedModeMeasCapabFDD_ext);
  
  for(index = 0; index < ueci_band_ptr->num_supported_bands_fdd2 ; index++)
  {
    //set the FDD2 Band appropriately
    temp_fdd_meas_cap_list_ext_ptr->elem[index].radioFrequencyBandFDD2 =  
                   (rrc_RadioFrequencyBandFDD2) ueci_band_ptr->band_type_fdd2[index];
    if(((process_interfreq_interrat_meas_rel7 == FALSE)&&
        (process_interfreq_uei_meas_rel7 == FALSE)) ||
        ((rrccsp_channel_locking_nv_status()) && !rrccsp_is_ue_in_true_limited_service()))
    {
      /*Temp: L1 doesnt support Compressed mode at present */
      temp_fdd_meas_cap_list_ext_ptr->elem[index].dl_MeasurementsFDD = FALSE;
      temp_fdd_meas_cap_list_ext_ptr->elem[index].ul_MeasurementsFDD = FALSE;
    }
    else
    {
      temp_fdd_meas_cap_list_ext_ptr->elem[index].dl_MeasurementsFDD = TRUE;
      temp_fdd_meas_cap_list_ext_ptr->elem[index].ul_MeasurementsFDD = TRUE;
    }
  }

}

/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_non_ext

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcueci_append_ue_radio_access_capability_non_ext
(
  rrc_UE_RadioAccessCapability *ue_radio_access_capability_ptr,  
                                                /* Pointer to UE Radio Access 
                                                   Capability */
  rrcueci_internal_cap_update_cause_e_type update_cause  /* Capability query cause*/
)
{
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
  tm_umts_cp_wcdma_ue_pos_capability_struct_type pos_capability;
#endif

  uint32 int_algo_bit_mask = 0;
  uint32 cipher_algo_bit_mask = 0;

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
  /*Reset and update from position capability*/
  rrc_ue_pos_cap_pch = FALSE;
#endif
  /* Check whether Radio Access capability is a valid pointer */
  if(ue_radio_access_capability_ptr != NULL)
  {

    /* only for capability update send measurement info.. for 
       HO INFO transfer, this will be sent in extension */
    if(update_cause == RRCUECI_INT_CAP_UPDATE_2100)
    {
      /*Measurement Capability is supported */
     RRC_MSG_COMMON_SET_BITMASK_IE_PTR(ue_radio_access_capability_ptr,
        rrc_UE_RadioAccessCapability,measurementCapability);
    }
    else
    {
      /*Measurement Capability is supported */
      RRC_RESET_MSG_IE_PRESENT_PTR(ue_radio_access_capability_ptr);
    }

    /*UE-RadioAccessCapability is compatible with R99 , although accessStratumReleaseIndicator 
     * is removed from this IE , since its encoding did not does in bits . The 
     * accessStratumReleaseIndicator is provided in the relevant REL-4 extension IEs . */

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)

    /* First get positioning capability from LSM */
    if(rrcgps_cgps_ue_pos_capability_cb == NULL)
    {
      ERR_FATAL("GPS Call back function is NULL",0,0,0);
    }
    else
    {      
      rrcgps_cgps_ue_pos_capability_cb(&pos_capability);
    }
    ue_radio_access_capability_ptr->ue_positioning_Capability.
      standaloneLocMethodsSupported = 
      pos_capability.standalone_location_method_supported;
    
    switch(pos_capability.network_assisted_gps_support)
    {
      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NETWORK_BASED:
      #endif
        ue_radio_access_capability_ptr->ue_positioning_Capability.
          networkAssistedGPS_Supported = rrc_NetworkAssistedGPS_Supported_networkBased;
        break;

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_UE_BASED:
      #endif
        ue_radio_access_capability_ptr->ue_positioning_Capability.
          networkAssistedGPS_Supported = rrc_NetworkAssistedGPS_Supported_ue_Based;
        break;

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_BOTH:
      #endif
        ue_radio_access_capability_ptr->ue_positioning_Capability.
          networkAssistedGPS_Supported = rrc_NetworkAssistedGPS_Supported_bothNetworkAndUE_Based;
        break;

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case WCDMA_CLASSMARK_UE_CAPABILITY_NONE:
      #endif
        ue_radio_access_capability_ptr->ue_positioning_Capability.
          networkAssistedGPS_Supported = rrc_NetworkAssistedGPS_Supported_noNetworkAssistedGPS;
        break;

      default:break;
    } 
       
    ue_radio_access_capability_ptr->ue_positioning_Capability.
      supportForUE_GPS_TimingOfCellFrames = 
      pos_capability.gps_timing_of_cell_frames_supported;
    /*Update from pos capability*/
    rrc_ue_pos_cap_pch = pos_capability.up_meas_validity_cell_pch_and_ura_pch ;

#else
    /* Stand alone Location Methods are not supported */
    ue_radio_access_capability_ptr->ue_positioning_Capability.
      standaloneLocMethodsSupported = FALSE;

    /* Network assisted GPS is not supported */
    ue_radio_access_capability_ptr->ue_positioning_Capability.
      networkAssistedGPS_Supported = rrc_NetworkAssistedGPS_Supported_noNetworkAssistedGPS;; 
    /* Capability to measure GPS Reference is not supported */
    ue_radio_access_capability_ptr->ue_positioning_Capability.
       supportForUE_GPS_TimingOfCellFrames = FALSE;
#endif

    /*Capability to use IPDL is not supported */
    ue_radio_access_capability_ptr->ue_positioning_Capability.
      supportForIPDL = FALSE;

    /* UE Based OTDOA is not supported */
    ue_radio_access_capability_ptr->ue_positioning_Capability.
      ue_BasedOTDOA_Supported = FALSE;
   
    /* only for capability update send measurement info.. for 
       HO INFO transfer, this will be sent in extension */
    if(update_cause == RRCUECI_INT_CAP_UPDATE_2100)
    {    
      /* Update Measurement Capability.
       */  
      /* Downlink TDD,GSM and Multi Carrier Measurements are not supported */
      RRC_RESET_MSG_IE_PRESENT(ue_radio_access_capability_ptr->measurementCapability.
        downlinkCompressedMode);
  
      if (rrcmcm_is_dualmode_enabled_no_f3())
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_radio_access_capability_ptr->measurementCapability.
          downlinkCompressedMode,gsm_Measurements);
  
        //query rr for 900E/P band support and set the 900 supported flag to true
        if(rr_rrc_is_supported_band_internal(BAND_EGSM) || rr_rrc_is_supported_band_internal(BAND_PGSM))
        {
          ue_radio_access_capability_ptr->measurementCapability.
            downlinkCompressedMode.gsm_Measurements.gsm900 = TRUE;
        }
  
        //set appropriate flags for dcs and pcs bands..  850 band does not have an enum
        //hence not included in this bitmask
        ue_radio_access_capability_ptr->measurementCapability.
          downlinkCompressedMode.gsm_Measurements.dcs1800 = rr_rrc_is_supported_band_internal(BAND_DCS);
  
        ue_radio_access_capability_ptr->measurementCapability.
          downlinkCompressedMode.gsm_Measurements.gsm1900 = rr_rrc_is_supported_band_internal(BAND_PCS);
      }
    
      /* Compressed mode required to support Downlink FDD Mesaurements */
      if(((rrccsp_channel_locking_nv_status()) && !rrccsp_is_ue_in_true_limited_service()))
      {
        ue_radio_access_capability_ptr->measurementCapability.
          downlinkCompressedMode.fdd_Measurements = FALSE;

      }
      else
      {
        ue_radio_access_capability_ptr->measurementCapability.
          downlinkCompressedMode.fdd_Measurements = TRUE;
      }
  
      /* Uplink TDD,GSM and Multi Carrier Measurements are not supported */
      RRC_RESET_MSG_IE_PRESENT(ue_radio_access_capability_ptr->measurementCapability.
        uplinkCompressedMode);
  
      if (rrcmcm_is_dualmode_enabled_no_f3())
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_radio_access_capability_ptr->measurementCapability.
          uplinkCompressedMode,gsm_Measurements);
  
        ue_radio_access_capability_ptr->measurementCapability.uplinkCompressedMode.
          gsm_Measurements.gsm900 = 
          ue_radio_access_capability_ptr->measurementCapability.downlinkCompressedMode.
          gsm_Measurements.gsm900;
  
        ue_radio_access_capability_ptr->measurementCapability.uplinkCompressedMode.
          gsm_Measurements.dcs1800 =
          ue_radio_access_capability_ptr->measurementCapability.downlinkCompressedMode.
          gsm_Measurements.dcs1800;

        ue_radio_access_capability_ptr->measurementCapability.uplinkCompressedMode.
          gsm_Measurements.gsm1900 =
          ue_radio_access_capability_ptr->measurementCapability.downlinkCompressedMode.
          gsm_Measurements.gsm1900;

      }
  
      /* Compressed mode required to support Uplink FDD Mesaurements */
      if(((rrccsp_channel_locking_nv_status()) && !rrccsp_is_ue_in_true_limited_service()))
      {
        ue_radio_access_capability_ptr->measurementCapability.
          uplinkCompressedMode.fdd_Measurements = FALSE;
        
      }
      else
      {
        ue_radio_access_capability_ptr->measurementCapability.
          uplinkCompressedMode.fdd_Measurements = TRUE;
      }
    }

#ifdef FEATURE_UMTS_PDCP
    if(rrc_pdcp_enabled)
    { 
      /* Update PDCP Capability   
       */
      /* Lossless SRNS Relocation is not supported */
      ue_radio_access_capability_ptr->pdcp_Capability.
        losslessSRNS_RelocationSupport = FALSE;

      /* RFC2507 is not supported */
      ue_radio_access_capability_ptr->pdcp_Capability.
        supportForRfc2507.t = T_rrc_PDCP_Capability_supportForRfc2507_supported;

      ue_radio_access_capability_ptr->pdcp_Capability.
        supportForRfc2507.u.supported = rrc_MaxHcContextSpace_by1024;
    }
    else  
#endif /*FEATURE_UMTS_PDCP*/
    { 
      /* Update PDCP Capability   
       */
      /* Lossless SRNS Relocation is not supported */
      ue_radio_access_capability_ptr->pdcp_Capability.
        losslessSRNS_RelocationSupport = FALSE;

      /* RFC2507 is not supported */
      ue_radio_access_capability_ptr->pdcp_Capability.
        supportForRfc2507.t = T_rrc_PDCP_Capability_supportForRfc2507_notSupported;

    }

    /*Physiscal Channel Capability
     */
    /* Only FDD is supported */
    RRC_MSG_COMMON_SET_BITMASK_IE(ue_radio_access_capability_ptr->physicalChannelCapability,
      rrc_PhysicalChannelCapability,fddPhysChCapability);
  
    /* FDD Downlink Physical channel Capabilities */
    /* Supports upto 3 simultaneous DPCHs */
    ue_radio_access_capability_ptr->physicalChannelCapability.
      fddPhysChCapability.downlinkPhysChCapability.maxNoDPCH_PDSCH_Codes = 1; 

    /*Max number of physical channel bits received in any 10 ms TTI is 9600*/
    ue_radio_access_capability_ptr->physicalChannelCapability.
      fddPhysChCapability.downlinkPhysChCapability.
      maxNoPhysChBitsReceived = rrc_MaxNoPhysChBitsReceived_b9600;  
 
    /* SF 512 is not supported */
    ue_radio_access_capability_ptr->physicalChannelCapability.
      fddPhysChCapability.downlinkPhysChCapability.
      supportForSF_512 = FALSE; 

    /* PDSCH is not supported */
    ue_radio_access_capability_ptr->physicalChannelCapability.
      fddPhysChCapability.downlinkPhysChCapability.
      dummy = FALSE;                         
  
    /* Simultanous SCCPCH and DPCH is not supported */
    ue_radio_access_capability_ptr->physicalChannelCapability.
      fddPhysChCapability.downlinkPhysChCapability.
      dummy2.t = 
      T_rrc_SimultaneousSCCPCH_DPCH_Reception_notSupported; 

  

    /* FDD Uplink Physiscal Channel Capability */
    /* Max number of DPDCH bits transmitted per 10ms TTI is 9600*/
    ue_radio_access_capability_ptr->physicalChannelCapability.
      fddPhysChCapability.uplinkPhysChCapability.
      maxNoDPDCH_BitsTransmitted = rrc_MaxNoDPDCH_BitsTransmitted_b9600; 

    /* No support for PCPCH */
    ue_radio_access_capability_ptr->physicalChannelCapability.
      fddPhysChCapability.uplinkPhysChCapability.
      dummy = FALSE; 

    /* only for capability update send rf capability info.. for 
       HO INFO transfer, this will be sent in extension */
    if(update_cause == RRCUECI_INT_CAP_UPDATE_2100)
    {
      /* Update RF capability 
       */
      /* Supports FDD RF Capabilities */
      RRC_MSG_COMMON_SET_BITMASK_IE(ue_radio_access_capability_ptr->rf_Capability,
        rrc_RF_Capability,fddRF_Capability);
   
      /* Suports UE Powerclass 3 (24dBm) */
      /* Note: this power class is of type INTEGER */
      ue_radio_access_capability_ptr->rf_Capability.fddRF_Capability
           .ue_PowerClass = 3;

      WRRC_MSG1_HIGH("Send UE power class: %d (1->class1..4->class4)",
      ue_radio_access_capability_ptr->rf_Capability.fddRF_Capability.ue_PowerClass );

      /* TX/RX Frequency separation 190 Mhz */
      ue_radio_access_capability_ptr->rf_Capability.
        fddRF_Capability.txRxFrequencySeparation = rrc_TxRxFrequencySeparation_default_TxRx_separation;

    }
    else
    {
      /* RF capability is sent in ext
       */
      RRC_RESET_MSG_IE_PRESENT(ue_radio_access_capability_ptr->rf_Capability);
    }

    /* Update RLC Capability 
     */
    /* Maximum no of AM entities supported is 4 */
    ue_radio_access_capability_ptr->rlc_Capability.
      maximumAM_EntityNumber = rrcueci_get_support_am_entity();
    if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
    {
      /* Total AM buffer size is 1000KB */
      ue_radio_access_capability_ptr->rlc_Capability.
        totalRLC_AM_BufferSize = rrc_TotalRLC_AM_BufferSize_kb1000;
    }
    else
    if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
    {
      /* Total AM buffer size is 500KB */
      ue_radio_access_capability_ptr->rlc_Capability.
        totalRLC_AM_BufferSize = rrc_TotalRLC_AM_BufferSize_kb500;
    }
    else
    {
      /* Total AM buffer size is 150KB */
      ue_radio_access_capability_ptr->rlc_Capability.
         totalRLC_AM_BufferSize = rrc_TotalRLC_AM_BufferSize_kb150;
    }
    ue_radio_access_capability_ptr->rlc_Capability.
      maximumRLC_WindowSize = rrc_MaximumRLC_WindowSize_mws2047; /* Not Sure about this value */

    /* Security Capability 
     */
    /* Allocate memory for Ciphering Algorithm.Ciphering Algorithm is
       a 16 bit string. Hence 2 bytes are allocated */

/*lint -save -e778 -e572 */
    if (nv_ciphering_enabled_status)
    {
      cipher_algo_bit_mask = UEA_1_SUPPORTED + UEA_0_SUPPORTED;
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
      if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && snow3g_security_algo_supported)
      {
        cipher_algo_bit_mask += UEA_2_SUPPORTED;
      }
#endif
      /* UEA1 Encryption is supoorted in the current release*/
      ue_radio_access_capability_ptr->securityCapability.
       cipheringAlgorithmCap.numbits = 16;
      *(ue_radio_access_capability_ptr->securityCapability.
        cipheringAlgorithmCap.data) = 
       (uint8)(( cipher_algo_bit_mask & 0xFF00) >> 8 );
      *(ue_radio_access_capability_ptr->securityCapability.
        cipheringAlgorithmCap.data + 1) = 
        (uint8)( cipher_algo_bit_mask & 0xFF);
    }
    else
    {
      /* No Encryption is supoorted in the current release*/
      ue_radio_access_capability_ptr->securityCapability.
       cipheringAlgorithmCap.numbits = 16;
      *(ue_radio_access_capability_ptr->securityCapability.
        cipheringAlgorithmCap.data) = 
        ((UEA_0_SUPPORTED & 0xFF00) >> 8 );
      *(ue_radio_access_capability_ptr->securityCapability.
        cipheringAlgorithmCap.data + 1) = 
        (UEA_0_SUPPORTED & 0xFF);
    }
  


    /* No Integrity protection is supported in the current release. Actually 
       there is no way to inform this to UTRAN */ 
    ue_radio_access_capability_ptr->securityCapability.
      integrityProtectionAlgorithmCap.numbits = 16;

    int_algo_bit_mask = KASUMI_SUPPORTED;
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
    if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && snow3g_security_algo_supported)
    {
      int_algo_bit_mask += SNOW_3G_SUPPORTED;
    }
#endif

    *(ue_radio_access_capability_ptr->securityCapability.
      integrityProtectionAlgorithmCap.data) = 
      (uint8)( (int_algo_bit_mask & 0xFF00) >> 8 );
    *(ue_radio_access_capability_ptr->securityCapability.
      integrityProtectionAlgorithmCap.data + 1) = 
      (uint8)(int_algo_bit_mask & 0xFF);
  
/*lint -restore */


    /* Update downlink Transport Channel Capability */
#if (defined(FEATURE_6280_COMPILE) && (!defined(FEATURE_DISABLE_384K_ON_20MS_TTI_UE_CAPABILITY)))
    /* Max no of bits convolutionally coded Transport Blocks that can be
       received any time is 8960 */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.maxConvCodeBitsReceived = rrc_MaxNoBits_b8960;

    /* Max no of bits of all Transport Blocks that can be
       received any time is 8960 */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.maxNoBitsReceived = rrc_MaxNoBits_b8960;
#else
    /* Max no of bits convolutionally coded Transport Blocks that can be
       received any time is 6400 */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.maxConvCodeBitsReceived = rrc_MaxNoBits_b6400;

    /* Max no of bits of all Transport Blocks that can be
       received any time is 6400 */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.maxNoBitsReceived = rrc_MaxNoBits_b6400;
#endif  

    /* Max nomber of Transport Formats */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.maxNumberOfTF = rrc_MaxNumberOfTF_tf64;
  
    /* Max number of TFC in the TFCS */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.maxNumberOfTFC = 
      rrc_MaxNumberOfTFC_DL_tfc128;

    /* Max number of Transport Blocks that can be received in TTI */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.maxReceivedTransportBlocks = 
      rrc_MaxTransportBlocksDL_tb32;

    /* Max no of simultaneous transport channels */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.maxSimultaneousTransChs = 
      rrc_MaxSimultaneousTransChsDL_e8;

    /* Maximum Simultaneous CCTrCHs Count */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.maxSimultaneousCCTrCH_Count = 1;


    /* Turbo Coding is supported */
    ue_radio_access_capability_ptr->
      transportChannelCapability.dl_TransChCapability.turboDecodingSupport.
      t = T_rrc_TurboSupport_supported;

#if (defined(FEATURE_6280_COMPILE) && (!defined(FEATURE_DISABLE_384K_ON_20MS_TTI_UE_CAPABILITY)))
    /* Max no of bits supported for Turbo coding */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.turboDecodingSupport.u.supported = rrc_MaxNoBits_b8960;
#else
    /* Max no of bits supported for Turbo coding */
    ue_radio_access_capability_ptr->transportChannelCapability.
      dl_TransChCapability.turboDecodingSupport.u.supported = rrc_MaxNoBits_b6400;
#endif

    /* Update uplink Transport Channel Capability */

#if (defined(FEATURE_6280_COMPILE) && (!defined(FEATURE_DISABLE_384K_ON_20MS_TTI_UE_CAPABILITY)))    
  /* Max no of bits of all convolutionally coded tranport blocks */
    ue_radio_access_capability_ptr->transportChannelCapability.
      ul_TransChCapability.maxConvCodeBitsTransmitted = rrc_MaxNoBits_b8960;

    /* Max no of bits of all Transport Blocks */
    ue_radio_access_capability_ptr->transportChannelCapability.
      ul_TransChCapability.maxNoBitsTransmitted = rrc_MaxNoBits_b8960;
#else
    /* Max no of bits of all convolutionally coded tranport blocks */
    ue_radio_access_capability_ptr->transportChannelCapability.
      ul_TransChCapability.maxConvCodeBitsTransmitted = rrc_MaxNoBits_b6400;

    /* Max no of bits of all Transport Blocks */
    ue_radio_access_capability_ptr->transportChannelCapability.
      ul_TransChCapability.maxNoBitsTransmitted = rrc_MaxNoBits_b6400;
#endif

    /* Max no of Transport Formats */
    ue_radio_access_capability_ptr->transportChannelCapability.
      ul_TransChCapability.maxNumberOfTF = rrc_MaxNumberOfTF_tf64;

    /* Max no of TFC in TFCS */
    ue_radio_access_capability_ptr->transportChannelCapability.
      ul_TransChCapability.maxNumberOfTFC = rrc_MaxNumberOfTFC_UL_tfc64;

    /* Max no of Tranport Blocks that can be transmitted */
    ue_radio_access_capability_ptr->transportChannelCapability.
      ul_TransChCapability.maxTransmittedBlocks = rrc_MaxTransportBlocksUL_tb32;

    /* Max no of simultaneous Transport Channels */
    ue_radio_access_capability_ptr->transportChannelCapability.ul_TransChCapability.
      maxSimultaneousTransChs = rrc_MaxSimultaneousTransChsUL_e8;
  
    /* Turbo coding is supported */
     ue_radio_access_capability_ptr->transportChannelCapability.
      ul_TransChCapability.turboEncodingSupport.t =
      T_rrc_TurboSupport_supported;

#if (defined(FEATURE_6280_COMPILE) && (!defined(FEATURE_DISABLE_384K_ON_20MS_TTI_UE_CAPABILITY)))
    /* Max no of bits supported for Turbo coding */
    ue_radio_access_capability_ptr->transportChannelCapability.ul_TransChCapability.
      turboEncodingSupport.u.supported = rrc_MaxNoBits_b8960;
#else
    /* Max no of bits supported for Turbo coding */
    ue_radio_access_capability_ptr->transportChannelCapability.ul_TransChCapability.
      turboEncodingSupport.u.supported = rrc_MaxNoBits_b6400;
#endif

    /* Set Mode Specific Info in UL Tranport Channel Capability  to FDD*/
    ue_radio_access_capability_ptr->transportChannelCapability.ul_TransChCapability.
      modeSpecificInfo.t = T_rrc_UL_TransChCapability_modeSpecificInfo_fdd;


    /* Update Multi RAT capability 
    */
    /* FDD mode only supported */
    ue_radio_access_capability_ptr->ue_MultiModeRAT_Capability.
      multiModeCapability = rrc_MultiModeCapability_fdd;
    
    if (rrcmcm_is_dualmode_enabled_no_f3())
    {
      ue_radio_access_capability_ptr->ue_MultiModeRAT_Capability.
        multiRAT_CapabilityList.supportOfGSM = TRUE;
    }
    else
    { /* GSM is not supported */
      ue_radio_access_capability_ptr->ue_MultiModeRAT_Capability.
        multiRAT_CapabilityList.supportOfGSM = FALSE;
    }

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#else
    /* Multi Carrier is not supported */
    ue_radio_access_capability_ptr->ue_MultiModeRAT_Capability.
      multiRAT_CapabilityList.supportOfMulticarrier = FALSE;
#endif

  } /* Radio Access Capability is a valid pointer */
  else
  {
    WRRC_MSG0_ERROR("Invalid Radio Capability pointer");
  }

}

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================
FUNCTION rrcueci_append_lte_specific_capability

DESCRIPTION
  This function appends the EUTRA specifc capability
DEPENDENCIES
  None

RETURN VALUE
  TURE is successfully appended,
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
static boolean rrcueci_append_lte_specific_capability
(
  struct rrc_InterRAT_UE_RadioAccessCapability_v860ext *irat_ue_cap_860ext
)
{
  uint8 count =0;
  boolean eutra_cap_req = FALSE;

  for(count = 0; count < rrc_capability_update_requirement.system_specific_cap_update_req_list.count && count < 16;count++)
  {
    if(rrc_capability_update_requirement.system_specific_cap_update_req_list.value[count] == RRC_EUTRA_CAP)
    {
      eutra_cap_req = TRUE;
      break;
    }
  }

  if(!eutra_cap_req)
  {
    /*EUTRA cap is not requested*/
    return FALSE;
  }
  if((!rrcueci_interrat_lte_capability_info.lte_rsp_received) ||
     (rrcueci_interrat_lte_capability_info.cap_cont_ptr == NULL))
  {
    if((!rrcueci_interrat_lte_capability_info.lte_rsp_received) && rrcueci_interrat_lte_capability_info.lte_req_posted)
    {
      WRRC_MSG0_HIGH("LTE_IRAT: Didnt receive capability response from LTE");
    }
    return FALSE;
  }
  else
  {
    irat_ue_cap_860ext->m.eutra_RadioAccessCapabilityPresent = 1;
    irat_ue_cap_860ext->eutra_RadioAccessCapability.ue_EUTRA_Capability.numocts =
         rrcueci_interrat_lte_capability_info.cap_cont_length ;
    irat_ue_cap_860ext->eutra_RadioAccessCapability.ue_EUTRA_Capability.data = 
        rtxMemAllocZ(&enc_ctxt, rrcueci_interrat_lte_capability_info.cap_cont_length);
    if(NULL == irat_ue_cap_860ext->eutra_RadioAccessCapability.ue_EUTRA_Capability.data)
    {
      ERR_FATAL("Unable to allocate memory for LTE specific capability",0,0,0);
    }
    WCDMA_MEMCPY((void*)irat_ue_cap_860ext->eutra_RadioAccessCapability.ue_EUTRA_Capability.data, 
                 rrcueci_interrat_lte_capability_info.cap_cont_length,
                 rrcueci_interrat_lte_capability_info.cap_cont_ptr,
                 rrcueci_interrat_lte_capability_info.cap_cont_length);
    WRRC_MSG0_HIGH("Appended LTE RAT specific capability");
    return TRUE;
  }
}
#endif
/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcueci_append_ue_radio_access_capability
(
  rrc_UE_RadioAccessCapability *ue_radio_access_capability_ptr,  
  rrc_UE_RadioAccessCapability_v370ext *ue_radio_access_capability_ext_ptr,  // For fdd bands
  rrc_UE_RadioAccessCapability_v650ext *ue_radio_access_capability_v650_ptr, // For fdd2 bands
  rrcueci_cap_update_cause_e_type update_cause                               // Capability query cause
)
{
  uint8 count = 0;                                          //counter for various purposes
  uint32 num_total_supported_bands = 0;                     //total supported bands fdd+fdd2
  rrcueci_band_storage_type ueci_band;                      //type to get supported bands from csp
  boolean is_extensions_sent = FALSE;                       //keeps track if extensions are appended

  rrc_UE_RadioAccessCapabBandFDDList * local_fdd_list = NULL; //local ptr to walk thru fdd list
  rrc_UE_RadioAccessCapabBandFDDList * temp_fdd_list = NULL;  //temp pointer to store allocated memory address

  rrc_UE_RadioAccessCapabBandFDDList2 * temp_fdd2_list = NULL;

  rrc_UE_RadioAccessCapabBandFDDList_ext  *temp_fdd_list_ext = NULL;

  rrc_plmn_identity_type plmn_id;

  //reset the ueci_band type
  memset (&ueci_band, 0, sizeof(rrcueci_band_storage_type));

  //get the supported band list from csp
  rrccsp_get_supported_band_list(&ueci_band);

  //Get the total number of bands supported = fdd + fdd2
  num_total_supported_bands = ueci_band.num_supported_bands + ueci_band.num_supported_bands_fdd2;

  MSG_HIGH("Num supported bands: FDD:%d, FDD2:%d, Total:%d",
        ueci_band.num_supported_bands,
        ueci_band.num_supported_bands_fdd2,
        num_total_supported_bands);

  /* If Rel-5 is enabled, then we can also support FDD2 bands. So check if total_num_bands
     (FDD + FDD2)is equal to zero and error fatal is none of them is selected */
    //if no bands are supported, print MSG_ERROR here, this is a catastrophic problem
  if(num_total_supported_bands == 0)
  {
    ERR_FATAL("Unsupported band-config. No FDD/FDD2 bands selected",0,0,0);
  }


  if(update_cause == RRCUECI_CAP_HO_INFO_TRANSFER)
  {
    plmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(irat_plmn_id);
  }
  else
  {
    plmn_id = rrc_ccm_get_curr_camped_plmn();
  }
  //if there is only one freq supported, then check if update case is HO_INO_TRANSFER,
  //if so send extensions and regular capabilities.. if regular CAP_UPDATE, then send
  //only regular capabilities
  if(((num_total_supported_bands == 1) && (ueci_band.band_type_present[rrc_RadioFrequencyBandFDD_fdd2100] == TRUE)) &&
      (((RRCUECI_CAP_HO_INFO_TRANSFER == update_cause) 
#ifdef FEATURE_LTE_TO_WCDMA
          && !rrcmcm_is_lte_mode_enabled()
#endif
        )|| ((RRCUECI_CAP_HO_INFO_TRANSFER != update_cause) && 
            !((rrcmcm_is_dualmode_enabled_no_f3()) && 
            ((rr_rrc_is_supported_band_internal(BAND_EGSM)) ||
             (rr_rrc_is_supported_band_internal(BAND_PGSM)) ||
             (rr_rrc_is_supported_band_internal(BAND_GSM_850)))))
             ))
  {
    /* While reporting UE capabilities, if UE supports only IMT2k (wcdma) band, include
     * radio access capability extensions iff the UE supports any of the GSM bands 
     * 900E/900P/850/480/450. Note that the check for the GSM bands 480/450 is not 
     * added as UE currently does not support them. We may need add check for them 
     * when UE supports */
    if(RRCUECI_CAP_HO_INFO_TRANSFER == update_cause) 
    {
      WRRC_MSG0_HIGH("2100 band for HO");

      //update extensions sent flag to true
      is_extensions_sent=TRUE;

      local_fdd_list = &ue_radio_access_capability_ext_ptr->ue_RadioAccessCapabBandFDDList;
      local_fdd_list->n = 1;
      
      ALLOC_ASN1ARRAY2(&enc_ctxt, local_fdd_list, rrc_UE_RadioAccessCapabBandFDD);

      rrcueci_append_ue_radio_access_capability_ext(&local_fdd_list->elem[0], rrc_RadioFrequencyBandFDD_fdd2100, &ueci_band
#ifdef FEATURE_LTE_TO_WCDMA
                    ,RRCUECI_INT_CAP_UPDATE_2100
#endif
                    );
    }

    rrcueci_append_ue_radio_access_capability_non_ext(ue_radio_access_capability_ptr, 
                                                    RRCUECI_INT_CAP_UPDATE_2100);
  }
  else
  {
    //if more than one band is supported or if single band other than 1900 is
    //supported, then send cap_ext always, also send reduced capabilities in
    //the regular ue-capabilities
      
#ifdef FEATURE_LTE_TO_WCDMA
     if((RRCUECI_CAP_HO_INFO_TRANSFER == update_cause) && (rrcmcm_is_lte_mode_enabled()))
     {
       WRRC_MSG0_HIGH("EUTRA Enabled");
     }
     else
#endif
     {
       WRRC_MSG0_HIGH("Multi-Band or Non-2100 band");
     }
    //update extensions sent flag to true
    is_extensions_sent=TRUE;

    /* In Inter-RAT handover info message (Classmark) the Radio Access Capability
       is sent as a container IE within the message. The max size of this container
       is only 64 octets. The size of this container is much dependant on the number
       of FDD bands (FDD2 does not have any impact) that is advertised. By trial and 
       error method the the value of RRC_UECI_MAX_FDD_BANDS_FOR_IHO has been 
       determined that can be included without exceeding 64 octets. So if our UE 
       supports more than this number of bands, the list needs to be trimmed before
       the message can be constructed. The logic will be explained below. Note that
       this is a limitation only for IHO and hence this algorithm should be not invoked
       for RCE or UECI. This algorithm removes at max two bands from the list */

       /* num_total_supported_bands can be greater than RRC_UECI_MAX_BANDS_ALLOWED only if 
          FDD bands are greater than 4(RRCUECI_CAP_HO_INFO_TRANSFER). Since currently we support
          four FDD2 bands*/
    if(((ueci_band.num_supported_bands > RRC_UECI_MAX_FDD_BANDS_FOR_IHO) &&
       (update_cause == RRCUECI_CAP_HO_INFO_TRANSFER)) ||
       (num_total_supported_bands > RRC_UECI_MAX_BANDS_ALLOWED))
    {
      uint32 i = 0;
      uint32 j = 0;
      uint32 first_band_to_remove = RRC_UECI_INVALID_BAND_TYPE;
      uint32 second_band_to_remove = RRC_UECI_INVALID_BAND_TYPE;
      
      WRRC_MSG2_ERROR("Num FDD bands %d, Max bands supp in IHO %d. Needs filtering",
                ueci_band.num_supported_bands,RRC_UECI_MAX_FDD_BANDS_FOR_IHO);  

      

      /* Now check if this PLMN belongs to US or not. Based on the result, remove appropriate
         bands from the supported band list */
      WRRC_MSG1_HIGH("MCC belongs to US %d", rrccsp_does_mcc_belong_to_US(plmn_id));
      if(TRUE == rrccsp_does_mcc_belong_to_US(plmn_id))
      {
        /* The current camped PLMN is US. So we can go ahead and remove the foll bands 
           in the specified order:
           1. 1800 (Band iii)
           2. 800  (Band vi)
         */
        first_band_to_remove = (uint32)rrc_RadioFrequencyBandFDD_fdd1800;
        second_band_to_remove = (uint32)rrc_RadioFrequencyBandFDD_bandVI;
      }
      else
      {

        /* The current camped PLMN is non-US. So we can go ahead and remove the foll bands 
           in the specified order:
           1. 1800 (Band iii)
           2. AWS  (Band iv)
         */
        first_band_to_remove = (uint32)rrc_RadioFrequencyBandFDD_fdd1800;
        second_band_to_remove = (uint32)rrc_RadioFrequencyBandFDD_bandIV;
      }

      /* Go over the list and try to remove the first band */
      for(i=0;i < ueci_band.num_supported_bands;i++)
      {
        /* Remove first band */
        if((i < RRC_UECI_MAX_BANDS_ALLOWED) && (ueci_band.band_type[i] == first_band_to_remove))
        {
          WRRC_MSG1_HIGH("Removing band %d from supported bands list",ueci_band.band_type[i]);

          /* Decrement number of bands */
          ueci_band.num_supported_bands--;

          /* Now check if removal of this band will cause a hole, if so plug 
             the hole and compact the array */
          for(j = i; (j < ueci_band.num_supported_bands) && (j < (RRC_UECI_MAX_BANDS_ALLOWED - 1)) ; j++)
          {
            ueci_band.band_type[j] = ueci_band.band_type[j+1];
          }
          break;
        }
      }

      /* Now check if num supported bands is still greater that allowed. If so go over the 
         list again and remove the second band */
      if((ueci_band.num_supported_bands > RRC_UECI_MAX_FDD_BANDS_FOR_IHO) &&
         (second_band_to_remove != RRC_UECI_INVALID_BAND_TYPE))
      {
        for(i=0;i < ueci_band.num_supported_bands;i++)
        {
          /* Remove second band */
          if((i < RRC_UECI_MAX_BANDS_ALLOWED) && (ueci_band.band_type[i] == second_band_to_remove))
          {
            WRRC_MSG1_HIGH("Removing band %d from supported bands list",ueci_band.band_type[i]);
  
            /* Decrement number of bands */
            ueci_band.num_supported_bands--;
  
            /* Now check if removal of this band will cause a hole, if so plug 
               the hole and compact the array */
            for(j = i; (j < ueci_band.num_supported_bands) && (j < (RRC_UECI_MAX_BANDS_ALLOWED - 1)) ; j++)
            {
              ueci_band.band_type[j] = ueci_band.band_type[j+1];
            }

            break;
          }
        }
      }

      /* Recalculate the total number of bands supported = fdd + fdd2 */
      num_total_supported_bands = ueci_band.num_supported_bands + ueci_band.num_supported_bands_fdd2;

      MSG_HIGH("After filtering: Num supported bands: FDD:%d, FDD2:%d, Total:%d",
               ueci_band.num_supported_bands,ueci_band.num_supported_bands_fdd2,num_total_supported_bands);
    }
    temp_fdd_list = &ue_radio_access_capability_ext_ptr->ue_RadioAccessCapabBandFDDList;
    if((ueci_band.num_supported_bands == 0) && (ueci_band.num_supported_bands_fdd2 > 0))
    {
         temp_fdd_list->n = ueci_band.num_supported_bands +1;
    }
    else
    {
      temp_fdd_list->n = ueci_band.num_supported_bands;
    }
    ALLOC_ASN1ARRAY2(&enc_ctxt, temp_fdd_list, rrc_UE_RadioAccessCapabBandFDD);

    //count used to go thru number of supported bands
    for(count = 0; count < ueci_band.num_supported_bands; count++)
    {

#ifdef FEATURE_LTE_TO_WCDMA
      if(update_cause == RRCUECI_CAP_HO_INFO_TRANSFER)
      {
        rrcueci_append_ue_radio_access_capability_ext(&temp_fdd_list->elem[count], 
        (rrc_RadioFrequencyBandFDD) ueci_band.band_type[count], &ueci_band
          ,RRCUECI_INT_CAP_UPDATE_1900_MULTI_BAND);
      }
      else
#endif
      {
        rrcueci_append_ue_radio_access_capability_ext(&temp_fdd_list->elem[count], 
        (rrc_RadioFrequencyBandFDD) ueci_band.band_type[count], &ueci_band
#ifdef FEATURE_LTE_TO_WCDMA
          ,RRCUECI_INT_CAP_UPDATE_2100
#endif
          );      
       }
    }
    /* Now we have looped thru the FDD bands.Check to see if any FDD2 band is supported.If so,we need 
       to set the extension bit in fdd list to indicate that a fdd2 list is present in the Rel-6 
       cricitcal extensions. In our implementation the FDD2 bands can be signaled with REL-5 feature 
       and above. Hence make sure RRC version is atleast REL-5 before populating FDD2 related IEs */

     //Include the extension indicator only when number of FDD bands is zero.
    if (ueci_band.num_supported_bands != 0 )
    {
      WRRC_MSG0_ERROR("We will not fill FDD2 if even one FDD band is present");
    }
    else if(ueci_band.num_supported_bands_fdd2 > 0)
    {
      temp_fdd_list->elem[count].radioFrequencyBandFDD = rrc_RadioFrequencyBandFDD_extension_indicator;
      RRC_RESET_MSG_IE_PRESENT(temp_fdd_list->elem[count]);
      RRC_RESET_MSG_IE_PRESENT(temp_fdd_list->elem[count].measurementCapability);    

      // Allocate memory for CompressedModeFDDList since it is a mandatory parameter
      // Set all fields to NULL inside the structure
      temp_fdd_list->elem[count].measurementCapability.compressedModeMeasCapabFDDList.n =1;
      ALLOC_ASN1ARRAY2(&enc_ctxt,
       &temp_fdd_list->elem[count].measurementCapability.compressedModeMeasCapabFDDList,
       rrc_CompressedModeMeasCapabFDD);
      
      RRC_RESET_MSG_IE_PRESENT(temp_fdd_list->elem[count].measurementCapability.compressedModeMeasCapabFDDList.elem[0]);
      temp_fdd_list->elem[count].measurementCapability.compressedModeMeasCapabFDDList.elem[0].dl_MeasurementsFDD = FALSE;
      temp_fdd_list->elem[count].measurementCapability.compressedModeMeasCapabFDDList.elem[0].ul_MeasurementsFDD = FALSE;
    }
    



    if(ueci_band.num_supported_bands_fdd2 > 0)
    {
      temp_fdd2_list = &ue_radio_access_capability_v650_ptr->ue_RadioAccessCapabBandFDDList2;
      temp_fdd2_list->n = ueci_band.num_supported_bands_fdd2;
      ALLOC_ASN1ARRAY2(&enc_ctxt,temp_fdd2_list,rrc_UE_RadioAccessCapabBandFDD2);
      /* Append FDD2 bands if any */
      
      for(count = 0; count < ueci_band.num_supported_bands_fdd2; count++)
      {
        rrcueci_append_ue_radio_access_capability_fdd2_ext(&temp_fdd2_list->elem[count], 
          (rrc_RadioFrequencyBandFDD2)ueci_band.band_type_fdd2[count], &ueci_band);
    
       
      }
      if(ueci_band.num_supported_bands > 0)
      {
        temp_fdd_list_ext=
           &ue_radio_access_capability_v650_ptr->ue_RadioAccessCapabBandFDDList_ext;
        temp_fdd_list_ext->n =ueci_band.num_supported_bands;
        ALLOC_ASN1ARRAY2(&enc_ctxt,temp_fdd_list_ext,rrc_UE_RadioAccessCapabBandFDD_ext);
      
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2_PTR(ue_radio_access_capability_v650_ptr,
            ue_RadioAccessCapabBandFDDList_ext);
         
      }
      /* Populate the extensions */
      for(count = 0; (count < ueci_band.num_supported_bands) && (count < RRC_UECI_MAX_BANDS_ALLOWED);count++)
      {

        rrcueci_append_ue_radio_access_capability_fdd_ext(&temp_fdd_list_ext->elem[count], 
          (rrc_RadioFrequencyBandFDD)ueci_band.band_type[count], &ueci_band);

       
      }
      
    }
    rrcueci_append_ue_radio_access_capability_non_ext(ue_radio_access_capability_ptr, 
                                                      RRCUECI_INT_CAP_UPDATE_1900_MULTI_BAND);
  }
  return is_extensions_sent;
} /* rrcueci_append_ue_radio_access_capability */


#ifdef FEATURE_IRAT_HO_INFO_COMPRESSED
#error code not present
#endif /* FEATURE_IRAT_HO_INFO_COMPRESSED */
#ifdef FEATURE_WCDMA_DB_DC_HSDPA
/*===========================================================================
FUNCTION rrc_update_db_dc_band_supported

DESCRIPTION
  This function updates the DB DC bands supported on DB DC global based on RF capability and upper layer band capability

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void rrc_update_db_dc_band_supported(void)
{
  /*As per spec 16 combinations can be indicated to network*/
  const rfm_wcdma_dual_band_type* band_comb_list = NULL;
  uint16 num_band_comb_supported = 0;
  uint8 count = 0;
  db_dc_band_comb_list.num_db_dc_band_comb_supp = 0;

  if((TRUE == rfm_wcdma_get_dbdc_band_support(&num_band_comb_supported,&band_comb_list)) && (band_comb_list!=NULL))
  {
    for (count=0; (count < num_band_comb_supported) && (db_dc_band_comb_list.num_db_dc_band_comb_supp < RRC_NUMBER_OF_DB_DC_BAND_COMB) ; count++)
    {
      if((band_comb_list[count].prim_band == SYS_BAND_WCDMA_I_IMT_2000 ) && (rrccsp_is_supported_band(rrc_convert_sys_band_class_to_rrc_band(SYS_BAND_WCDMA_I_IMT_2000))))
      {
        if ((band_comb_list[count].sec_band == SYS_BAND_WCDMA_VIII_900) && (rrccsp_is_supported_band(rrc_convert_sys_band_class_to_rrc_band(SYS_BAND_WCDMA_VIII_900)))
			&& (wcdma_rrc_db_dc_band_comb_nv_control & (1 << (RRC_DB_DC_BAND1_BAND8-1))))
        {
          db_dc_band_comb_list.db_dc_band_comb_list[db_dc_band_comb_list.num_db_dc_band_comb_supp] = RRC_DB_DC_BAND1_BAND8;
          db_dc_band_comb_list.num_db_dc_band_comb_supp++;
        }
        else if ((band_comb_list[count].sec_band == SYS_BAND_WCDMA_V_850) && (rrccsp_is_supported_band(rrc_convert_sys_band_class_to_rrc_band(SYS_BAND_WCDMA_V_850)))
			&& (wcdma_rrc_db_dc_band_comb_nv_control & (1 << (RRC_DB_DC_BAND1_BAND5-1))))
        {
          db_dc_band_comb_list.db_dc_band_comb_list[db_dc_band_comb_list.num_db_dc_band_comb_supp] = RRC_DB_DC_BAND1_BAND5;
          db_dc_band_comb_list.num_db_dc_band_comb_supp++;
        }
        else if ((band_comb_list[count].sec_band == SYS_BAND_WCDMA_IX_1700) && (rrccsp_is_supported_band(rrc_convert_sys_band_class_to_rrc_band(SYS_BAND_WCDMA_IX_1700)))
			&& (wcdma_rrc_db_dc_band_comb_nv_control & (1 << (RRC_DB_DC_BAND1_BAND9-1))))
        {
          db_dc_band_comb_list.db_dc_band_comb_list[db_dc_band_comb_list.num_db_dc_band_comb_supp] = RRC_DB_DC_BAND1_BAND9;
          db_dc_band_comb_list.num_db_dc_band_comb_supp++;
        }
      }
      else if((band_comb_list[count].prim_band == SYS_BAND_WCDMA_II_PCS_1900 ) && (rrccsp_is_supported_band(rrc_convert_sys_band_class_to_rrc_band(SYS_BAND_WCDMA_II_PCS_1900))))
      {
        if ((band_comb_list[count].sec_band == SYS_BAND_WCDMA_IV_1700) && (rrccsp_is_supported_band(rrc_convert_sys_band_class_to_rrc_band(SYS_BAND_WCDMA_IV_1700)))
			&& (wcdma_rrc_db_dc_band_comb_nv_control & (1 << (RRC_DB_DC_BAND2_BAND4-1))))
        {
          db_dc_band_comb_list.db_dc_band_comb_list[db_dc_band_comb_list.num_db_dc_band_comb_supp] = RRC_DB_DC_BAND2_BAND4;
          db_dc_band_comb_list.num_db_dc_band_comb_supp++;
        }
        else if((band_comb_list[count].sec_band == SYS_BAND_WCDMA_V_850) && (rrccsp_is_supported_band(rrc_convert_sys_band_class_to_rrc_band(SYS_BAND_WCDMA_V_850)))
			&& (wcdma_rrc_db_dc_band_comb_nv_control & (1 << (RRC_DB_DC_BAND2_BAND5-1))))
        {
          db_dc_band_comb_list.db_dc_band_comb_list[db_dc_band_comb_list.num_db_dc_band_comb_supp] = RRC_DB_DC_BAND2_BAND5;
          db_dc_band_comb_list.num_db_dc_band_comb_supp++;
        }
      }
    }
  }
}
#endif
/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_later_than_r3

DESCRIPTION
  This function appends the Radio Access Capability to the message for
  Release 5.
  
  We should optimize the code later.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_append_ue_radio_access_capability_later_than_r3
(
  /* Pointer to UE Radio Access Capability */
  rrc_UE_RadioAccessCapability_v590ext *ue_radio_access_capability_ptr,  
   rrc_UE_CapabilityContainer_IEs *ue_capability_container,
  rrcueci_cap_update_cause_e_type update_cause  /* Capability query cause */
)
{

#ifdef FEATURE_WCDMA_DB_DC_HSDPA
  rrc_UE_RadioAccessCapabBandCombList *temp_band_comb_list_ptr = NULL;
  uint8 count = 0;
#endif
#ifdef FEATURE_WCDMA_3C_HSDPA
  rrc_UE_RadioAccessCapability_va40ext_IEs *radio_access_capability = NULL;
   rrc_UE_RadioAccessCapabBandCombList_va40ext *temp_ue_RadioAccessCapabBandCombList_va40ext = NULL;
  uint8 idx = 0;
  rrcueci_band_storage_type ueci_band = {0};	
  rrc_UE_RadioAccessCapabBandFDDList4 *temp_fdd_list4_ptr = NULL;
  uint8 num_of_sb_3c = 0; /* number of bands supported for single band 3C */
  rrc_RadioFrequencyBandFDD_Root sb_3c_band[2] = {0};
#endif
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR
  rrc_UE_CapabilityContainer_IEs_v860NonCriticalExtensions *rce_v860_ext = NULL;
#endif
  
  /* Check whether Radio Access capability is a valid pointer */
  if(ue_radio_access_capability_ptr != NULL)
  {
    
    /* The CapabilityWithSimultaneousHS_DSCHConfig_present is not given when 
    the access capability is not required by the UE capability. 
    */  
    if(update_cause == RRCUECI_CAP_UPDATE)
    {
      RRC_RESET_MSG_IE_PRESENT_PTR(ue_radio_access_capability_ptr);
      ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.t = 
        T_rrc_PhysicalChannelCapability_hspdsch_r5_fdd_hspdsch_unsupported; 
      RRC_RESET_MSG_IE_PRESENT(ue_radio_access_capability_ptr->rlc_Capability_r5_ext);
      /* CapabilityWithSimultaneousHS_DSCHConfig_present is 64 k */
      RRC_MSG_COMMON_SET_BITMASK_IE_PTR(ue_radio_access_capability_ptr,
        rrc_UE_RadioAccessCapability_v590ext,dl_CapabilityWithSimultaneousHS_DSCHConfig);
      ue_radio_access_capability_ptr->dl_CapabilityWithSimultaneousHS_DSCHConfig = rrc_DL_CapabilityWithSimultaneousHS_DSCHConfig_kbps64;
      /*In the R3 part the RLC buffer size is 500kb, so dont update
       * the R5 path for RLC buffer size
       */
      if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
      {
         RRC_RESET_MSG_IE_PRESENT(ue_radio_access_capability_ptr->rlc_Capability_r5_ext);
      }
      else
      {
        /* Fill in RLC info. */
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_radio_access_capability_ptr->rlc_Capability_r5_ext,totalRLC_AM_BufferSize);
        if(nv_hsdpa_category >= 8 )
        {
         ue_radio_access_capability_ptr->rlc_Capability_r5_ext.totalRLC_AM_BufferSize = rrc_TotalRLC_AM_BufferSize_r5_ext_kb400;
        }
        else
        {
          ue_radio_access_capability_ptr->rlc_Capability_r5_ext.totalRLC_AM_BufferSize = rrc_TotalRLC_AM_BufferSize_r5_ext_kb300;
        }
      }
      /* Fill in Phy */
      ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.t =
        T_rrc_PhysicalChannelCapability_hspdsch_r5_fdd_hspdsch_supported;

      ue_radio_access_capability_ptr->physicalChannelCapability.
                 fdd_hspdsch.u.supported =
      rtxMemAllocTypeZ(&enc_ctxt, rrc_PhysicalChannelCapability_hspdsch_r5_supported );
      
#ifdef FEATURE_WCDMA_REL7_SPECRS
      MSG_7(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHS:Setting HSDPA category rrcueci_ue_cap_originator %d ue_cap_change_requested.is_valid_mask 0x%x rrccouec.ue_cap_change_requested.hsdpa_category %d rrc_ue_cap_during_rrc_conn.is_valid_mask 0x%0x rrc_ue_cap_during_rrc_conn.hsdpa_category %d nv_hsdpa_category %d rrc_machs_cat %d", 
      rrcueci_ue_cap_originator, 
      rrccouec.ue_cap_change_requested.is_valid_mask, 
      rrccouec.ue_cap_change_requested.hsdpa_category, 
      rrc_ue_cap_during_rrc_conn.is_valid_mask, 
      rrc_ue_cap_during_rrc_conn.hsdpa_category, 
      nv_hsdpa_category, 
      rrc_machs_cat);

      if((rrcueci_ue_cap_originator == RRC_UE_CAPABILITY_ORIGINATOR_COUEC) && 
          (rrccouec.ue_cap_change_requested.is_valid_mask & HSDPA_CAT_INCL))
      {
         if( rrccouec.ue_cap_change_requested.hsdpa_category > 12 )
         {
           ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
                  supported->hsdsch_physical_layer_category = rrc_machs_cat;
         }
         else
         {
           ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
             supported->hsdsch_physical_layer_category = 
                  rrccouec.ue_cap_change_requested.hsdpa_category;
         }
      }
      else if(rrc_ue_cap_during_rrc_conn.is_valid_mask & HSDPA_CAT_INCL)
      {
        if( rrc_ue_cap_during_rrc_conn.hsdpa_category > 12 )
        {
          ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
          supported->hsdsch_physical_layer_category = rrc_machs_cat;
        }
        else
        {
          ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
                 supported->hsdsch_physical_layer_category = 
                   rrc_ue_cap_during_rrc_conn.hsdpa_category;
        }
      }
      else  
#endif /*FEATURE_WCDMA_REL7_SPECRS*/
      if(nv_hsdpa_category >12 )
      {
        ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
        supported->hsdsch_physical_layer_category = rrc_machs_cat;
      }
      else
      {
        ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
        supported->hsdsch_physical_layer_category = 
            nv_hsdpa_category;
      }
  
      /* DedicatedPilotsForChannelEstimationOfHSDSCH not supported */
      ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
        supported->dummy = FALSE;

      if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
          && (rrc_wcdma_hsupa_cm_enabled == FALSE)
          )
      {
        /* Set it to TRUE - this is an indication to NW that "REL-6" UE doesnt support HSUPA+CM,
           This will help infra vendors to identify early REL-6 UEs which doesnt support HSUPA+CM  */
        ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
          supported->dummy = TRUE;
      }



      ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
        supported->dummy2 = FALSE;

      if(rrc_mimo_64qam_cm_enabled == TRUE)
      {
        /*
           If IE "dummy2" is set to TRUE then:
             UE does not support 64 QAM and Compressed Mode simultaneously.
             UE does not support MIMO and Compressed Mode simultaneously
          */
        ue_radio_access_capability_ptr->physicalChannelCapability.fdd_hspdsch.u.
          supported->dummy2 = TRUE;
      }

      /* TDD not supported */
      ue_radio_access_capability_ptr->physicalChannelCapability.tdd128_hspdsch.t =
        T_rrc_PhysicalChannelCapability_hspdsch_r5_tdd128_hspdsch_unsupported;
      ue_radio_access_capability_ptr->physicalChannelCapability.tdd384_hspdsch.t =
        T_rrc_PhysicalChannelCapability_hspdsch_r5_tdd384_hspdsch_unsupported;
      
#ifdef FEATURE_UMTS_PDCP
      if(rrc_pdcp_enabled)
      { 
        /* Fill in PDCP Cap. */
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(
        ue_radio_access_capability_ptr->pdcp_Capability_r5_ext,
        maxHcContextSpace);
        
        ue_radio_access_capability_ptr->pdcp_Capability_r5_ext.maxHcContextSpace = rrc_MaxHcContextSpace_r5_ext_by16384;
        ue_radio_access_capability_ptr->pdcp_Capability_r5_ext.supportForRfc3095ContextRelocation = FALSE;
      }
      else
#endif /* FEATURE_UMTS_PDCP*/
      {
        /* Fill in PDCP Cap. */
        /* Not Support RFC 3095 for PDCP */
        ue_radio_access_capability_ptr->pdcp_Capability_r5_ext.supportForRfc3095ContextRelocation = 0;
        /* No maxHcContextSpace present */
        RRC_RESET_MSG_IE_PRESENT(ue_radio_access_capability_ptr->pdcp_Capability_r5_ext);
      }
      /* Fill in MultiModeRAT Cap. */
      if(rrcmcm_is_dualmode_enabled_no_f3() && nv_wtog_nacc_enabled )
      {
        ue_radio_access_capability_ptr->multiModeRAT_Capability_v590ext.supportOfUTRAN_ToGERAN_NACC = TRUE;
      }
      else
      {
        ue_radio_access_capability_ptr->multiModeRAT_Capability_v590ext.supportOfUTRAN_ToGERAN_NACC = FALSE;
      }
    }
    else
    {
      RRC_RESET_MSG_IE_PRESENT_PTR(ue_radio_access_capability_ptr);
    }
  } /* Radio Access Capability is a valid pointer */
  else
  {
    WRRC_MSG0_ERROR("Invalid Radio Capability pointer");
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
  {
    if(update_cause == RRCUECI_CAP_UPDATE)
    {    
      RRC_RESET_MSG_IE_PRESENT_PTR(ue_capability_container);
      
      RRC_RESET_MSG_IE_PRESENT(ue_capability_container->
        ue_RadioAccessCapability_v690ext);
  
      /* Set bit mask */
      RRC_MSG_COMMON_SET_BITMASK_IE_PTR(ue_capability_container,
      rrc_UE_CapabilityContainer_IEs,v6b0NonCriticalExtensions);
            
      RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions
          .ue_RadioAccessCapability_v6b0ext);
            
      RRC_MSG_COMMON_SET_BITMASK_IE(ue_capability_container->v6b0NonCriticalExtensions,
        rrc_UE_CapabilityContainer_IEs_v6b0NonCriticalExtensions,v6e0NonCriticalExtensions);
            
      RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions);
      
      RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.
        v6e0NonCriticalExtensions.ue_RadioAccessCapability_v6e0ext);

      if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && (!(rrc_disable_cpc_fdpch_nv & NV_FDPCH_DISABLE)))
      {
        /* F-DPCH Cap set */
        RRC_MSG_COMMON_SET_BITMASK_IE(ue_capability_container->v6b0NonCriticalExtensions.
          v6e0NonCriticalExtensions.ue_RadioAccessCapability_v6e0ext,
          rrc_UE_RadioAccessCapability_v6e0ext_IEs,supportForFDPCH);
              
        ue_capability_container->v6b0NonCriticalExtensions.
          v6e0NonCriticalExtensions.ue_RadioAccessCapability_v6e0ext.supportForFDPCH = rrc_UE_RadioAccessCapability_v6e0ext_IEs_supportForFDPCH_true;
              
      }

      if(rrc_device_type_nv)
      {  
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->ue_RadioAccessCapability_v690ext,
          deviceType);
        ue_capability_container->ue_RadioAccessCapability_v690ext.deviceType =
                                                      rrc_UE_RadioAccessCapability_v690ext_deviceType_doesNotBenefitFromBatteryConsumptionOptimisation;
      }
     
       MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCEUL: optional feature EUL/HSUPA(0x%x->0x%x), wcdma_nv_hsupa_category %d default category %d",
        wcdma_nv_optional_feature_list, CHECK_WCDMA_OPTIONAL_FEATURE_HSUPA_SUPPORTED, wcdma_nv_hsupa_category, RRC_WCMDA_HSUPA_DEFAULT_CATEGORY);
      if (CHECK_WCDMA_OPTIONAL_FEATURE_HSUPA_SUPPORTED)
      {
        ue_capability_container->ue_RadioAccessCapability_v690ext.
            physicalchannelcapability_edch.fdd_edch.t = T_rrc_PhysicalChannelCapability_edch_r6_fdd_edch_supported;
        
        ue_capability_container->ue_RadioAccessCapability_v690ext.
            physicalchannelcapability_edch.fdd_edch.u.supported =
      rtxMemAllocTypeZ(&enc_ctxt, rrc_PhysicalChannelCapability_edch_r6_supported );
#ifdef FEATURE_WCDMA_REL7_SPECRS
        if((rrcueci_ue_cap_originator == RRC_UE_CAPABILITY_ORIGINATOR_COUEC) && 
            (rrccouec.ue_cap_change_requested.is_valid_mask & HSUPA_CAT_INCL))
        {
                ue_capability_container->ue_RadioAccessCapability_v690ext.
                  physicalchannelcapability_edch.fdd_edch.u.supported->
                    edch_PhysicalLayerCategory = 
                    rrccouec.ue_cap_change_requested.hsupa_category;
        }
        else if(rrc_ue_cap_during_rrc_conn.is_valid_mask & HSUPA_CAT_INCL)
        {
                ue_capability_container->ue_RadioAccessCapability_v690ext.
                  physicalchannelcapability_edch.fdd_edch.u.supported->
                    edch_PhysicalLayerCategory = 
                    rrc_ue_cap_during_rrc_conn.hsupa_category;
        }
        else
#endif
        {
          ue_capability_container->ue_RadioAccessCapability_v690ext.
            physicalchannelcapability_edch.fdd_edch.u.supported->
              edch_PhysicalLayerCategory = MIN(wcdma_nv_hsupa_category,WNV_GET_DEFAULT(WNV_ID_WCDMA_HSUPA_DEFAULT_CATEGORY_I));
        }

      }
      else
      {
        ue_capability_container->ue_RadioAccessCapability_v690ext.
          physicalchannelcapability_edch.fdd_edch.t = T_rrc_PhysicalChannelCapability_edch_r6_fdd_edch_unsupported;
      }
  
      if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
      {
  
        ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.m.
            v770NonCriticalExtensionsPresent=1;
  
  
  
        rrcueci_append_ue_radio_access_capability_rel7_capability( ue_capability_container,
                                                                 RRCUECI_CAP_UPDATE
                                                                 );
      }

      if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
      {
        /*If extension v790NonCriticalExtensions is already present, it would be simply overwritten which is fine.*/
        ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
        v770NonCriticalExtensions.m.v790NonCriticalExtensionsPresent =1;

        /*Now add R8 extension v860NonCriticalExtensions*/
        ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.m.v860NonCriticalExtensionsPresent =1;

        /*Reset the bit_mask under ext v860NonCriticalExtensions. All the R8 related capabilities will be set from R8 function*/
        RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions);

        rrcueci_append_ue_radio_access_capability_rel8_capability(ue_capability_container);
#ifdef FEATURE_WCDMA_TO_LTE
        /*Here populate the EUTRA capability octer string in OTA*/
        if(rrcmcm_is_lte_mode_enabled() &&
            rrcueci_append_lte_specific_capability(&ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
             v770NonCriticalExtensions.v790NonCriticalExtensions.v860NonCriticalExtensions.ue_RATSpecificCapability_v860ext))
        {

          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions, ue_RATSpecificCapability_v860ext);
        }
#endif
#ifdef FEATURE_WCDMA_DC_HSDPA
		/*Added check to update cell-specific STTD support based on UE HSDPA category and  DC support of the current band */
        if((rrcueci_nv_supports_dc_hsdpa() == TRUE) &&
           (!rrccsp_check_if_em_plmn()))
        {

        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
           v790NonCriticalExtensions.v860NonCriticalExtensions, v880NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
           v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.ue_RadioAccessCapability_v880ext);

        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions, v890NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions);

        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.
          ue_RadioAccessCapability_v890ext, supportCellSpecificTxDiversityinDC_Operation);

        ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.
          ue_RadioAccessCapability_v890ext.supportCellSpecificTxDiversityinDC_Operation = rrc_UE_RadioAccessCapability_v890ext_IEs_supportCellSpecificTxDiversityinDC_Operation_true;

        }
		else
#endif			
        {
          WRRC_MSG0_HIGH("DC-HSDPA:Support for Diff TxD mode is not signaled.");
        }

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
        if(!RRC_CHECK_MSG_TYPE_IE(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions,v880NonCriticalExtensionsPresent))
        {

          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions, v880NonCriticalExtensions);

          RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions);

          RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
              v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.ue_RadioAccessCapability_v880ext);					
        }

        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
           v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.ue_RadioAccessCapability_v880ext, supportForPriorityReselectionInUTRAN);

        ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.ue_RadioAccessCapability_v880ext.
          supportForPriorityReselectionInUTRAN = rrc_UE_RadioAccessCapability_v880ext_IEs_supportForPriorityReselectionInUTRAN_true;

        WRRC_MSG0_HIGH("ABSOL_PRI: Set support for intra UTRA priority reselection IE");
#endif
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
        if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9) && (rrc_mfbi_support_nv 
#ifdef FEATURE_WCDMA_TO_LTE
|| lte_rrc_cap_mfbi_is_supported(FALSE) || lte_rrc_cap_mfbi_is_supported(TRUE)
#endif
          ))
        {
          /*Set the support of MFBI IE if version is REL10*/
          rce_v860_ext = &ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions;
          rce_v860_ext->m.v880NonCriticalExtensionsPresent =1;
          rce_v860_ext->v880NonCriticalExtensions.m.v890NonCriticalExtensionsPresent =1;
          rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.m.v920NonCriticalExtensionsPresent =1;
          rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions
            .m.v970NonCriticalExtensionsPresent =1;
          rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions
            .m.va40NonCriticalExtensionsPresent =1;
          rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions
            .va40NonCriticalExtensions.m.va60NonCriticalExtensionsPresent =1;
          rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions
            .va40NonCriticalExtensions.va60NonCriticalExtensions.m.va80NonCriticalExtensionsPresent =1;
          rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
            va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.m.laterNonCriticalExtensionsPresent =1;
          rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
            va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
            v9c0NonCriticalExtensions.m.vaa0NonCriticalExtensionsPresent =1;
          if(rrc_mfbi_support_nv)
          {
            rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
              v9c0NonCriticalExtensions.vaa0NonCriticalExtensions.ue_RadioAccessCapability_vaa0ext.m.rf_capabilityPresent=1;
            rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
              v9c0NonCriticalExtensions.vaa0NonCriticalExtensions.ue_RadioAccessCapability_vaa0ext.rf_capability.m.supportOfMFBIPresent=1;
            rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
              v9c0NonCriticalExtensions.vaa0NonCriticalExtensions.ue_RadioAccessCapability_vaa0ext.rf_capability.supportOfMFBI=rrc_RF_Capability_vaa0ext_supportOfMFBI_true;
            WRRC_MSG0_HIGH("set the support of MFBI IE");
          }

#ifdef FEATURE_WCDMA_TO_LTE
           if(lte_rrc_cap_mfbi_is_supported(FALSE) || lte_rrc_cap_mfbi_is_supported(TRUE))
          {
            rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
              v9c0NonCriticalExtensions.vaa0NonCriticalExtensions.m.vb50NonCriticalExtensionsPresent = 1;

            rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
              v9c0NonCriticalExtensions.vaa0NonCriticalExtensions.vb50NonCriticalExtensions.m.vb70NonCriticalExtensionsPresent = 1;

            rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
              v9c0NonCriticalExtensions.vaa0NonCriticalExtensions.vb50NonCriticalExtensions.vb70NonCriticalExtensions.m.vae0NonCriticalExtensionsPresent = 1;

            rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
              v9c0NonCriticalExtensions.vaa0NonCriticalExtensions.vb50NonCriticalExtensions.vb70NonCriticalExtensions.
              vae0NonCriticalExtensions.ue_RadioAccessCapability_vae0ext.m.multiModeRAT_CapabilityPresent = 1;

            rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
              v9c0NonCriticalExtensions.vaa0NonCriticalExtensions.vb50NonCriticalExtensions.vb70NonCriticalExtensions.
              vae0NonCriticalExtensions.ue_RadioAccessCapability_vae0ext.multiModeRAT_Capability.m.eutraSupportOfMFBIPresent = 1;

            rce_v860_ext->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.va60NonCriticalExtensions.va80NonCriticalExtensions.laterNonCriticalExtensions.
              v9c0NonCriticalExtensions.vaa0NonCriticalExtensions.vb50NonCriticalExtensions.vb70NonCriticalExtensions.
              vae0NonCriticalExtensions.ue_RadioAccessCapability_vae0ext.multiModeRAT_Capability.eutraSupportOfMFBI = rrc_MultiModeRAT_Capability_vae0ext_eutraSupportOfMFBI_true;

          }
#endif

        }
#endif 
      }

#ifdef FEATURE_WCDMA_DC_HSUPA

      if (rrcueci_nv_supports_dc_hsupa() == TRUE)
      {

        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         v790NonCriticalExtensions.v860NonCriticalExtensions,v880NonCriticalExtensionsPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions, v880NonCriticalExtensions);
        }

        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions,v890NonCriticalExtensionsPresent ))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions,v890NonCriticalExtensions);
        }

        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions,v920NonCriticalExtensionsPresent))
        {

          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions, v920NonCriticalExtensions);
        }

        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.ue_RadioAccessCapability_v920ext.physicalChannelCapability, fddPhysChCapabilityPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
           v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.ue_RadioAccessCapability_v920ext.physicalChannelCapability, fddPhysChCapability);
        }

        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.ue_RadioAccessCapability_v920ext.physicalChannelCapability.
          fddPhysChCapability.uplinkPhysChCapability,edch_PhysicalLayerCategory_extension2);
        ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.ue_RadioAccessCapability_v920ext.physicalChannelCapability.
          fddPhysChCapability.uplinkPhysChCapability.edch_PhysicalLayerCategory_extension2 = MAX_DC_HSUPA_CATEGORY;
      }

#endif  /* FEATURE_WCDMA_DC_HSUPA */

#ifdef FEATURE_WCDMA_REL9

      if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9)
      {
        /* Setting the measurement id extension support */
        if(rrc_meas_id_extn_support)
        {
          rrc_UE_CapabilityContainer_IEs_v860NonCriticalExtensions *ue_cap_v860 = NULL;
          rrc_UE_CapabilityContainer_IEs_v970NonCriticalExtensions *ue_cap_v970 = NULL;

          ue_cap_v860 = &ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions;
           
          ue_cap_v860->m.v880NonCriticalExtensionsPresent = 1;
          ue_cap_v860->v880NonCriticalExtensions.m.v890NonCriticalExtensionsPresent = 1;
          ue_cap_v860->v880NonCriticalExtensions.v890NonCriticalExtensions.m.v920NonCriticalExtensionsPresent = 1;
          ue_cap_v860->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.m.v970NonCriticalExtensionsPresent = 1;
           
          ue_cap_v970 = &ue_cap_v860->v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions;
          ue_cap_v970->ue_RadioAccessCapability_v970ext.m.ue_ExtendedMeasurementSupportPresent = 1;
          ue_cap_v970->ue_RadioAccessCapability_v970ext.ue_ExtendedMeasurementSupport = rrc_UE_RadioAccessCapability_v970ext_IEs_ue_ExtendedMeasurementSupport_true;
        }
      }
#endif /* FEATURE_WCDMA_REL9 */


#ifdef FEATURE_WCDMA_DB_DC_HSDPA
      if ((rrcueci_nv_supports_dc_hsdpa() == TRUE) && (rrcueci_nv_supports_db_dc_hsdpa() == TRUE))
      {
        rrc_update_db_dc_band_supported();
        /*Call RF API to know number of band combinations supported and the band combination list*/
        if (db_dc_band_comb_list.num_db_dc_band_comb_supp)      
        {
          if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
           v790NonCriticalExtensions.v860NonCriticalExtensions,v880NonCriticalExtensionsPresent))
          {
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
              v790NonCriticalExtensions.v860NonCriticalExtensions, v880NonCriticalExtensions);

            RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
              v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions);
          }

          if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
           v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions,v890NonCriticalExtensionsPresent ))
          {
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
              v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions,v890NonCriticalExtensions);

            RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
              v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions);
          }

          if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
           v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions,v920NonCriticalExtensionsPresent))
          {

            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
              v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions, v920NonCriticalExtensions);
            RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
              v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions);
          }

          if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
           v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.
           ue_RadioAccessCapability_v920ext, ue_RadioAccessCapabBandCombListPresent))
          {
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
             v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.
             ue_RadioAccessCapability_v920ext, ue_RadioAccessCapabBandCombList);
            
          }

          temp_band_comb_list_ptr = &(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.
            ue_RadioAccessCapability_v920ext.ue_RadioAccessCapabBandCombList);


          temp_band_comb_list_ptr->n=db_dc_band_comb_list.num_db_dc_band_comb_supp;
          ALLOC_ASN1ARRAY2(&enc_ctxt,temp_band_comb_list_ptr,rrc_BandComb);
      
          for (count=0; count < temp_band_comb_list_ptr->n; count++)
          {
            temp_band_comb_list_ptr->elem[count]=db_dc_band_comb_list.db_dc_band_comb_list[count];
            WRRC_MSG1_HIGH("DB_DC_HSDPA: Add bit mask to support band comb %d ",temp_band_comb_list_ptr->elem[count]);
          }
        }
      }

#endif /* FEATURE_WCDMA_DB_DC_HSDPA */
#ifdef FEATURE_WCDMA_3C_HSDPA
     if((rrcueci_nv_supports_3c_hsdpa() == TRUE) && (!rrccsp_check_if_em_plmn()))
      {
        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         v790NonCriticalExtensions.v860NonCriticalExtensions,v880NonCriticalExtensionsPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions, v880NonCriticalExtensions);
        
          RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions);
        }
        
        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions,v890NonCriticalExtensionsPresent ))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions,v890NonCriticalExtensions);
        
          RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions);
        }
        
        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions,v920NonCriticalExtensionsPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions, v920NonCriticalExtensions);
          RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions);
        }
        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
         v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions,v970NonCriticalExtensionsPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        	v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions,v970NonCriticalExtensions);
          RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        	v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions);
        }
        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v790NonCriticalExtensions.v860NonCriticalExtensions.
        v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions,va40NonCriticalExtensionsPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions,
          va40NonCriticalExtensions);
        }
        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v790NonCriticalExtensions.v860NonCriticalExtensions.
        v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.va40NonCriticalExtensions.ue_RadioAccessCapability_va40ext.physicalChannelCapability,
        fddPhysChCapabilityPresent))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v790NonCriticalExtensions.v860NonCriticalExtensions.
          v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.va40NonCriticalExtensions.ue_RadioAccessCapability_va40ext.physicalChannelCapability,
          fddPhysChCapability);
        }
        
        if(!RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v790NonCriticalExtensions.v860NonCriticalExtensions.
        v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.va40NonCriticalExtensions.ue_RadioAccessCapability_va40ext.physicalChannelCapability.
        fddPhysChCapability.downlinkPhysChCapability, hsdsch_physical_layer_category_ext4Present))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v790NonCriticalExtensions.v860NonCriticalExtensions.
          v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.va40NonCriticalExtensions.ue_RadioAccessCapability_va40ext.physicalChannelCapability.
          fddPhysChCapability.downlinkPhysChCapability,hsdsch_physical_layer_category_ext4);
        }
        radio_access_capability = &(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
      	v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
        	va40NonCriticalExtensions.ue_RadioAccessCapability_va40ext);
        radio_access_capability->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.hsdsch_physical_layer_category_ext4 = RRC_DEFAULT_3C_HSDPA_CATEGORY;
      
      /*
              25.331, sec 8.1.3.6
              10.3.3.42	UE radio access capability
              >Supported Carrier Combination 
              Absence of this IE means that the UE only supports the carrier combination (1,1) for this band combination (A,B). 
              If carrier combination (X,Y) is supported, then carrier combination (M,N) is supported, where 1<=M<=X and 1<=N<=Y, 
              so the corresponding value for that carrier combination  (M,N) should be set to TRUE
              */
  #ifdef FEATURE_WCDMA_DB_DC_HSDPA
        if((rrcueci_nv_supports_db_dc_hsdpa() && rrcueci_nv_supports_dc_hsdpa()) && 
           (db_dc_band_comb_list.num_db_dc_band_comb_supp))
        {
          if(!(RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
          	v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
          	va40NonCriticalExtensions.ue_RadioAccessCapability_va40ext,ue_RadioAccessCapabBandCombList_va40extPresent)))
          {
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
              v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
              va40NonCriticalExtensions.ue_RadioAccessCapability_va40ext,ue_RadioAccessCapabBandCombList_va40ext);
            
            /* This is capability of DB_3C and this should be filled only when UE is DB_DC capable, else Band combinations which are filled as part of DB_DC capability will not be known to network */
            radio_access_capability->ue_RadioAccessCapabBandCombList_va40ext.n = db_dc_band_comb_list.num_db_dc_band_comb_supp;
            
            temp_ue_RadioAccessCapabBandCombList_va40ext = &radio_access_capability->ue_RadioAccessCapabBandCombList_va40ext;
            ALLOC_ASN1ARRAY2(&enc_ctxt,temp_ue_RadioAccessCapabBandCombList_va40ext,rrc_SupportedCarrierCombination);
            
            for(count=0;count<temp_ue_RadioAccessCapabBandCombList_va40ext->n;count++)
            {
              temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination12=FALSE;
              temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination21=FALSE;
              temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination13=FALSE;
              temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination22=FALSE;
              temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination31=FALSE;
              if( db_dc_band_comb_list.db_dc_band_comb_list[count] == RRC_DB_DC_BAND1_BAND8)
              {
               temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination21=TRUE;
             }
             else if( db_dc_band_comb_list.db_dc_band_comb_list[count] == RRC_DB_DC_BAND2_BAND4)
             {
               temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination12=TRUE;
               temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination21=TRUE;
             }
             else if( db_dc_band_comb_list.db_dc_band_comb_list[count] == RRC_DB_DC_BAND1_BAND5)
             {
               temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination12=TRUE;
                temp_ue_RadioAccessCapabBandCombList_va40ext->elem[count].carrierCombination21=TRUE;
              }
            }
          }
        }
#endif
       rrccsp_get_supported_band_list(&ueci_band);
       
       if(0 == ueci_band.num_supported_bands)
       {
         ERR_FATAL("NO FDD bands supported",0,0,0);
       }
       
       if(TRUE == ueci_band.band_type_present[rrc_RadioFrequencyBandFDD_fdd2100])
       {
        sb_3c_band[num_of_sb_3c] = rrc_RadioFrequencyBandFDD_fdd2100;
        num_of_sb_3c ++;
       }
       if((RRC_SB_3C_BANDII_SUPPORTED & rrc_feature_nv_item_value) && 
   	     (TRUE == ueci_band.band_type_present[rrc_RadioFrequencyBandFDD_fdd1900]))
       {
         sb_3c_band[num_of_sb_3c] = rrc_RadioFrequencyBandFDD_fdd1900;
         num_of_sb_3c ++;
       }
       if(num_of_sb_3c)
       {
         if(!(RRC_CHECK_MSG_TYPE_IE (ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
            v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
            va40NonCriticalExtensions.ue_RadioAccessCapability_va40ext,ue_RadioAccessCapabBandFDDList4Present)))
         {
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.
        	v790NonCriticalExtensions.v860NonCriticalExtensions.v880NonCriticalExtensions.v890NonCriticalExtensions.v920NonCriticalExtensions.v970NonCriticalExtensions.
        	va40NonCriticalExtensions.ue_RadioAccessCapability_va40ext,ue_RadioAccessCapabBandFDDList4);
         }
          
         temp_fdd_list4_ptr = &(radio_access_capability->ue_RadioAccessCapabBandFDDList4);
         temp_fdd_list4_ptr->n = num_of_sb_3c;
         ALLOC_ASN1ARRAY2(&enc_ctxt,temp_fdd_list4_ptr,rrc_UE_RadioAccessCapabBandFDD4); 	 
         for(idx = 0; idx < num_of_sb_3c; idx++)
         {  
           RRC_RESET_MSG_IE_PRESENT(temp_fdd_list4_ptr->elem[idx]);
           if(!RRC_CHECK_MSG_TYPE_IE (temp_fdd_list4_ptr->elem[idx],radioFrequencyBandFDDPresent))
           { 			 
             RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(temp_fdd_list4_ptr->elem[idx],radioFrequencyBandFDD);
           }
           temp_fdd_list4_ptr->elem[idx].radioFrequencyBandFDD = sb_3c_band[idx];
           if(!RRC_CHECK_MSG_TYPE_IE (temp_fdd_list4_ptr->elem[idx],additionalSecondaryCellsPresent))
           { 			 
             RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(temp_fdd_list4_ptr->elem[idx],additionalSecondaryCells);
           }
           temp_fdd_list4_ptr->elem[idx].additionalSecondaryCells = rrc_UE_RadioAccessCapabBandFDD4_additionalSecondaryCells_a1;
         }
       }
      }
#endif

      WRRC_MSG0_HIGH("sib11bis: Add bit mask supportForSIB11bis to indicate SIB11bis support in ue_capability_container");
      if(!RRC_CHECK_MSG_TYPE_IE_PTR(ue_capability_container,v6b0NonCriticalExtensionsPresent))
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_PTR(ue_capability_container,
            rrc_UE_CapabilityContainer_IEs,v6b0NonCriticalExtensions);
   
        RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions);
      }

      RRC_MSG_COMMON_SET_BITMASK_IE(ue_capability_container->v6b0NonCriticalExtensions.ue_RadioAccessCapability_v6b0ext,
        rrc_UE_RadioAccessCapability_v6b0ext_IEs,supportForSIB11bis);

      ue_capability_container->v6b0NonCriticalExtensions.
        ue_RadioAccessCapability_v6b0ext.supportForSIB11bis = rrc_UE_RadioAccessCapability_v6b0ext_IEs_supportForSIB11bis_true;     
      
    }
  
  }
} /* rrcueci_append_ue_radio_access_capability_r5 */

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif

/*===========================================================================
FUNCTION rrcueci_append_gsm_specific_capability

DESCRIPTION
  This function appends the GSM Specific Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_append_gsm_specific_capability
(
  rrc_InterRAT_UE_RadioAccessCapability *inter_rat_capability_ptr   
                                /* Pointer to Inter RAT Capabilities List */
)
{
  classmark_2_ie_T classmark2;
  classmark_3_ie_T classmark3;
  uint32 counter;

#ifdef FEATURE_DUAL_SIM
  classmark_params_T classmark_params;
  classmark_params.gsm_supported = rrcmcm_is_dualmode_enabled_no_f3();
  classmark_params.wcdma_supported = TRUE;
  classmark_params.wcdma_in_use = TRUE;
  classmark_params.MSCR99 = TRUE;
  classmark_params.as_id = rrc_get_as_id();
#endif
  
  inter_rat_capability_ptr->t = 
    T_rrc_InterRAT_UE_RadioAccessCapability_gsm;

  /* Set Classmark 2 */
#ifdef FEATURE_DUAL_SIM
  gs_read_coded_classmark_2_per_subs(&classmark2,&classmark_params);
#else
  gs_read_coded_classmark_2(&classmark2,
    rrcmcm_is_dualmode_enabled_no_f3(), TRUE, TRUE, TRUE);
#endif
  
  
  
  if(classmark2.length > 5)
  {
    classmark2.length = 5;
    WRRC_MSG0_ERROR("Invalid Classmark2 Len is rcvd");
  }
  inter_rat_capability_ptr->u.gsm =
    rtxMemAllocTypeZ(&enc_ctxt, rrc_InterRAT_UE_RadioAccessCapability_gsm );
  inter_rat_capability_ptr->u.gsm->gsm_Classmark2.numocts= 5;

  for(counter = 2; counter < 5; counter++)
  {
    inter_rat_capability_ptr->u.gsm->
      gsm_Classmark2.data[counter] = classmark2.data[counter-2];
  }
  inter_rat_capability_ptr->u.gsm->gsm_Classmark2.data[0] = 0x33;
  inter_rat_capability_ptr->u.gsm->gsm_Classmark2.data[1] = 0x3;

  /* set Classmark 3 */
#ifdef FEATURE_DUAL_SIM
  (void)gs_read_coded_classmark_3_per_subs(&classmark3,&classmark_params);
#else
  (void)gs_read_coded_classmark_3(&classmark3,
    rrcmcm_is_dualmode_enabled_no_f3(), TRUE, TRUE, TRUE);
#endif

  if(classmark3.length < 1)
  {
    classmark3.length = 1;
    WRRC_MSG0_ERROR("Invalid Classmark3 Len is rcvd");
  }
  if(classmark3.length > 32)
  {
    classmark3.length = 32;
    WRRC_MSG0_ERROR("Invalid Classmark3 Len is rcvd");
  }
 

  

  inter_rat_capability_ptr->u.gsm->gsm_Classmark3.numocts= classmark3.length;
  for(counter = 0; (counter < classmark3.length) && (counter < CLASSMARK3_DATA_MAX_LENGTH); counter++)
  {
    inter_rat_capability_ptr->u.gsm->
      gsm_Classmark3.data[counter] = classmark3.data[counter];
  }
 
}/* rrcueci_append_gsm_specific_capability */
/*===========================================================================
FUNCTION rrcueci_send_ue_capability_info_msg

DESCRIPTION
  This function builds the UE CAPABILITY INFORMATION message and calls the 
  the SEND CHAIN to transmit the message in the uplink

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static rrcueci_status_e_type rrcueci_send_ue_capability_info_msg
(
  void 
)
{  
  rlc_lc_id_type ul_lc_id;                              /* Uplink logical channel ID */
  rrcueci_status_e_type status = RRCUECI_FAILURE_MINOR; /* A local variable to store the status */
  rrc_UL_DCCH_Message *msg_ptr;                         /* Pointer to uplink DCCH message */ 
  rrc_UE_RadioAccessCapability_v650ext local_v650ext;   /* Local copy of v650extensions */
  boolean ret_flag = FALSE;
  boolean support_ho_to_gan = FALSE;

#ifdef FEATURE_WCDMA_TO_LTE
  boolean eutra_cap_req = FALSE;
#endif
#ifdef FEATURE_WCDMA_MIMO
  boolean support_tx_diversity_mimo = FALSE;

  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
    support_tx_diversity_mimo = TRUE;
  }
#endif
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
#ifdef FEATURE_GAN_3G
  #error code not present
#endif
  }

  /* Checks whether Logical Channel entity setup for DCCH logical
     Channel type */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);

  /* If RLC Logical Channel Id is already setup, the send UE CAPABILITY
     INFORMATION message through SEND_CHAIN */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    /* Allocate a buffer for an uplink DCCH Message. This buffer should be released
       by SEND_CHAIN  */
    msg_ptr = (rrc_UL_DCCH_Message *) rrc_malloc ( 
                                       sizeof(struct rrc_UL_DCCH_Message));
    if (msg_ptr == NULL)
    {
       ERR_FATAL("Memory alloc failed for UE Cap Info Msg",
                  0,0,0);
    }
    /* Select the message as UE CAPABILITY INFORMATION message */
    msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_ueCapabilityInformation;
    msg_ptr->message.u.ueCapabilityInformation.ue_RATSpecificCapability.n = 0;

    /* Set optional fields to exclude */  
    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation);
  
    if(rrcueci_ue_cap_enquiry_trans_id < 4)
    { 
      RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.ueCapabilityInformation,
            rrc_UECapabilityInformation,rrc_TransactionIdentifier);

      /* Set the UE Capability Enquiry Transaction Id */
      msg_ptr->message.u.ueCapabilityInformation.rrc_TransactionIdentifier = 
        rrcueci_ue_cap_enquiry_trans_id;
    }

#ifdef FEATURE_WCDMA_TO_LTE
    if(rrcmcm_is_lte_mode_enabled())
    {
      uint8 count;
      for(count = 0; (count < rrc_capability_update_requirement.system_specific_cap_update_req_list.count) 
            && (count < RRC_MAX_SYS_CAPABILITY); count++)
      {
        if(rrc_capability_update_requirement.system_specific_cap_update_req_list.value[count] == RRC_EUTRA_CAP)
        {
          eutra_cap_req = TRUE;
          break;
        }
      }
    }
#endif

    if ((rrc_capability_update_requirement.
        ue_radio_capability_update_requirement == TRUE) 
#ifdef FEATURE_WCDMA_TO_LTE
      /* Update the radio access capabilities along with EUTRA system specfic capbabilities 
         even if ue_radio_capability_update_requirement=FALSE, as UE should send valid values for 
         critical extentions when sending system specific cap.*/
       || (eutra_cap_req)
#endif
       )
    {
      RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.ueCapabilityInformation,
        rrc_UECapabilityInformation,ue_RadioAccessCapability);

      /* Initialize the v650 ext before populating it */
      RRC_RESET_MSG_IE_PRESENT(local_v650ext);
      local_v650ext.ue_RadioAccessCapabBandFDDList2.n = 0;
      local_v650ext.ue_RadioAccessCapabBandFDDList_ext.n = 0;

      /* Appends the Radio Access Capability to the UE CAPABILITY INFORMATION message */  
      ret_flag = rrcueci_append_ue_radio_access_capability(
                  &msg_ptr->message.u.ueCapabilityInformation.ue_RadioAccessCapability,
                  &msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
                    ueCapabilityInformation_v370ext.ue_RadioAccessCapability_v370ext,
                  &local_v650ext, 
                  RRCUECI_CAP_UPDATE);

      // Set ueCapabilityInformation.bit_mask to include v370NonCriticalExtensions
      RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.ueCapabilityInformation,
          rrc_UECapabilityInformation,v370NonCriticalExtensions);

      // Initialize ueCapabilityInformation_v370ext.bit_mask
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
      ueCapabilityInformation_v370ext);

      // Include rrc_ue_RadioAccessCapability_v370ext if the above function returned TRUE 
      if(TRUE == ret_flag)
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
        ueCapabilityInformation_v370ext,ue_RadioAccessCapability_v370ext);
      }

      // Set v370NonCriticalExtensions.bit_mask to include v380NonCriticalExtensions
      msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
      m.v380NonCriticalExtensionsPresent =1;

      // Set ueCapabilityInformation_v380ext.bit_mask to include RadioAccessCapability_v380ext
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
      v380NonCriticalExtensions.ueCapabilityInformation_v380ext, 
        ue_RadioAccessCapability_v380ext);
      
      // Set rx_tx_TimeDifferenceType2Capable to FALSE. Revisit this section when UE supports this.
      msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
      v380NonCriticalExtensions.ueCapabilityInformation_v380ext.ue_RadioAccessCapability_v380ext.
      ue_PositioningCapabilityExt_v380.rx_tx_TimeDifferenceType2Capable = FALSE;
      
      // Initialize bit_mask of dl_PhysChCapabilityFDD_v380ext 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
      v380NonCriticalExtensions.ueCapabilityInformation_v380ext.
      dl_PhysChCapabilityFDD_v380ext);

      // Initialize v380NonCriticalExtensions.bit_mask 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
      v380NonCriticalExtensions);
      
      /* We always attach the Rel 5 UE Cap msg. */
      //if (msg_version == MSG_REL5)
      /* Append FDD2 list if present */
      MSG_HIGH("UECI FDD2 Status:%d, NV Release:%d, HO_GAN Status:%d",
                  (local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0 ? 1:0),
                  rrc_nv_rel_indicator,
                  support_ho_to_gan);
        
      if((local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0) || 
         (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
        )
      {
        // Select v3a0NonCriticalExtensions on v380NonCriticalExtensions 
        msg_ptr->message.u.ueCapabilityInformation.
        v370NonCriticalExtensions.v380NonCriticalExtensions.m.v3a0NonCriticalExtensionsPresent =1;

        // Set v3a0NonCriticalExtensions.bit_mask to include laterNonCriticalExtensions
        msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
        v380NonCriticalExtensions.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent =1;

        // Initialize ueCapabilityInformation_v3a0ext.bit_mask 
        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
        v380NonCriticalExtensions.v3a0NonCriticalExtensions.ueCapabilityInformation_v3a0ext);

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
        if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) && (rrc_ue_pos_cap_pch))
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.v380NonCriticalExtensions.
            v3a0NonCriticalExtensions.ueCapabilityInformation_v3a0ext, ue_RadioAccessCapability_v3a0ext);
          msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.v380NonCriticalExtensions.
            v3a0NonCriticalExtensions.ueCapabilityInformation_v3a0ext.ue_RadioAccessCapability_v3a0ext.ue_PositioningCapabilityExt_v3a0.
            validity_CellPCH_UraPCH = rrc_UE_PositioningCapabilityExt_v3a0_validity_CellPCH_UraPCH_true;
        }
#endif
        // Initialize laterNonCriticalExtensions.bit_mask 
        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
        v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions);

        /* Append FDD2 list if present */
        if((local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0) ||(support_ho_to_gan)
          || (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
          )
        {
         

          /* Set the bitmask for r3_add_ext */
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions,
            ueCapabilityInformation_r3_add_ext);

    

          RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          ueCapabilityInformation_r3_add_ext);

          if(local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0)
          {

            /* Set rrc_ueCapabilityInformation_v650ext_present */
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            ueCapabilityInformation_r3_add_ext,ueCapabilityInformation_v650ext);

            /* Assign the populated local structure to the OTA msg structure */
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            ueCapabilityInformation_r3_add_ext.ueCapabilityInformation_v650ext.
            ue_RadioAccessCapability_v650ext = local_v650ext;
          }

          if(support_ho_to_gan)
          {
            RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            ueCapabilityInformation_r3_add_ext,
            rrc_UECapabilityInformation_r3_add_ext_IEs,v680NonCriticalExtensions);

            RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions);

            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.
            ueCapabilityInformation_v680ext.ue_RadioAccessCapability_v680ext.
            multiModeRAT_Capability_v680ext,supportOfHandoverToGAN);

            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.
            ueCapabilityInformation_v680ext.ue_RadioAccessCapability_v680ext.
            multiModeRAT_Capability_v680ext.supportOfHandoverToGAN = 
            rrc_MultiModeRAT_Capability_v680ext_supportOfHandoverToGAN_doesSupportHandoverToGAN;
          }
          if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
          {
            if(!RRC_CHECK_MSG_TYPE_IE(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.v380NonCriticalExtensions.v3a0NonCriticalExtensions.
                 laterNonCriticalExtensions.ueCapabilityInformation_r3_add_ext, v680NonCriticalExtensionsPresent))
            {

              RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext, v680NonCriticalExtensions);
  
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions);


              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.
              ueCapabilityInformation_v680ext.ue_RadioAccessCapability_v680ext.multiModeRAT_Capability_v680ext);
            }
    
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions,v7e0NonCriticalExtensions);
  
            RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions);
  
            RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              ueCapabilityInformation_v7e0ext.ue_RadioAccessCapability);

            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
             v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
             ueCapabilityInformation_v7e0ext.ue_RadioAccessCapability, supportForTwoDRXSchemesInPCH);

            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
             v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
             ueCapabilityInformation_v7e0ext.ue_RadioAccessCapability.supportForTwoDRXSchemesInPCH = 
                rrc_UE_RadioAccessCapability_v7e0ext_supportForTwoDRXSchemesInPCH_true;

            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
             v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
             ueCapabilityInformation_v7e0ext.ue_RadioAccessCapability, supportEDPDCHPowerInterpolation);
         
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
             v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
             ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
             ueCapabilityInformation_v7e0ext.ue_RadioAccessCapability.supportEDPDCHPowerInterpolation = 
                rrc_UE_RadioAccessCapability_v7e0ext_supportEDPDCHPowerInterpolation_true;

#ifdef FEATURE_WCDMA_MIMO
            if(support_tx_diversity_mimo)
            {
              RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions,v7f0NonCriticalExtensions);
                  
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.v7f0NonCriticalExtensions);
    
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.ueCapabilityInformation_v7f0ext);
    
              RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.ueCapabilityInformation_v7f0ext, ue_RadioAccessCapability);
    
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.ueCapabilityInformation_v7f0ext.ue_RadioAccessCapability);
    
              RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.ueCapabilityInformation_v7f0ext.ue_RadioAccessCapability, supportofTxDivOnNonMIMOChannel);
    
              msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              ueCapabilityInformation_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.ueCapabilityInformation_v7f0ext.ue_RadioAccessCapability.supportofTxDivOnNonMIMOChannel = rrc_UE_RadioAccessCapability_v7f0ext_supportofTxDivOnNonMIMOChannel_true;
    
            }
#endif			
          }
   

         
        }

        if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
        {
          /* Select v4b0NonCriticalExtensions on laterNonCriticalExtensions */
          msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.
            laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent =1;
        
          /* Select ueCapabilityInformation_v4b0ext on v4b0NonCriticalExtensions for
                 accessStratumReleaseIndicator */
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext,
            ue_RadioAccessCapability_v4b0ext);
        
          /* Select accessStratumReleaseIndicator on ue_RadioAccessCapability_v4b0ext */
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.
            ue_RadioAccessCapability_v4b0ext,accessStratumReleaseIndicator);
#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */
#ifdef FEATURE_WCDMA_REL9
          
          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9)
          {
            /* Set accessStratumReleaseIndicator to be Release 9. */
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.
              ue_RadioAccessCapability_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_9;
          }
          else
#endif /* FEATURE_WCDMA_REL9 */

          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
          {
            /* Set accessStratumReleaseIndicator to be Release 8. */
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.
              ue_RadioAccessCapability_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_8;
          }
          else
          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
          {
            /* Set accessStratumReleaseIndicator to be Release 6. */
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.
              ue_RadioAccessCapability_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_7;
          }
          else
          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
          {
            /* Set accessStratumReleaseIndicator to be Release 6. */
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.
              ue_RadioAccessCapability_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_6;
          }
          else
          {
            /* Set accessStratumReleaseIndicator to be Release 5. */
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.
              ue_RadioAccessCapability_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_5;
          }
          WRRC_MSG1_HIGH("AccessStratumIndicator indicator set to %d (rrc_nv_rel_indicator)", rrc_nv_rel_indicator);
          /* Finish ue_RadioAccessCapability_v4b0ext */
          msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.
            ue_RadioAccessCapability_v4b0ext.pdcp_Capability_r4_ext.supportForRfc3095.t =
            T_rrc_PDCP_Capability_r4_ext_supportForRfc3095_notSupported;
        
          /* Select v590NonCriticalExtensions on v4b0NonCriticalExtensions for accessing 590 extension */
          msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent =1;
      

#ifdef FEATURE_UMTS_PDCP
          if (rrc_pdcp_enabled)
          { 
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.ue_RadioAccessCapability_v4b0ext.
              pdcp_Capability_r4_ext.supportForRfc3095.t = T_rrc_PDCP_Capability_r4_ext_supportForRfc3095_supported;

            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.ue_RadioAccessCapability_v4b0ext.
              pdcp_Capability_r4_ext.supportForRfc3095.u.supported = 
              rtxMemAllocTypeZ(&enc_ctxt, rrc_PDCP_Capability_r4_ext_supported );
       

            /* NOTE:  When changing the below IE, need to also update check in rrcllcrbie, func get_pdcp_info(), that 
                     checks for the chosen MAX_CID signalled by NW is not greater than the value "Maximum number of ROHC context sessions" as
                     indicated in the IE "PDCP Capability" below.
                    */
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.ue_RadioAccessCapability_v4b0ext.
              pdcp_Capability_r4_ext.supportForRfc3095.u.supported->maxROHC_ContextSessions = rrc_MaxROHC_ContextSessions_r4_s4;
            
            
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.ueCapabilityInformation_v4b0ext.ue_RadioAccessCapability_v4b0ext.
              pdcp_Capability_r4_ext.supportForRfc3095.u.supported->reverseCompressionDepth = 0;
          }
#endif /* FEATURE_UMTS_PDCP */

          /* No 590 noncritical extension */
          RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.v590NonCriticalExtensions);
        
          /* Select RadioAccessCapability_v590ext on ueCapabilityInformation_v590ext */
          RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.v590NonCriticalExtensions.ueCapabilityInformation_v590ext,
            rrc_UECapabilityInformation_v590ext,ue_RadioAccessCapability_v590ext);
      

          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
          {
            /* v590 noncritical extension present */
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v5c0NonCriticalExtensionsPresent = 1;
    
            RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.v590NonCriticalExtensions.
              v5c0NonCriticalExtensions.ueCapabilityInformation_v5c0ext);
    
            /* v5c0 noncritical extension present */
            msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.v590NonCriticalExtensions.
              v5c0NonCriticalExtensions.m.v690NonCriticalExtensionsPresent = 1;
    
            /* No v6xy noncritical extension */
            RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.v590NonCriticalExtensions.
              v5c0NonCriticalExtensions.v690NonCriticalExtensions);
    
            /* No v6xy noncritical extension */
            RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.ueCapabilityInformation.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtensions.v590NonCriticalExtensions.
              v5c0NonCriticalExtensions.v690NonCriticalExtensions.ueCapabilityInformation_v690ext,
              rrc_UECapabilityInformation_v690ext_IEs,ueCapabilityContainer);
          }



         /* Appends the Radio Access Capability to the RRC CONNECTION SETUP
                COMPLETE message */
          rrcueci_append_ue_radio_access_capability_later_than_r3(&msg_ptr->message.u.ueCapabilityInformation.
                                                     v370NonCriticalExtensions.
                                                     v380NonCriticalExtensions.
                                                     v3a0NonCriticalExtensions.
                                                     laterNonCriticalExtensions.
                                                     v4b0NonCriticalExtensions.
                                                     v590NonCriticalExtensions.
                                                     ueCapabilityInformation_v590ext.
                                                     ue_RadioAccessCapability_v590ext,
                                                     &msg_ptr->message.u.ueCapabilityInformation.
                                                     v370NonCriticalExtensions.
                                                     v380NonCriticalExtensions.
                                                     v3a0NonCriticalExtensions.
                                                     laterNonCriticalExtensions.
                                                     v4b0NonCriticalExtensions.
                                                     v590NonCriticalExtensions.
                                                     v5c0NonCriticalExtensions.
                                                     v690NonCriticalExtensions.
                                                     ueCapabilityInformation_v690ext.
                                                     ueCapabilityContainer,
                                                     RRCUECI_CAP_UPDATE);
        }
      }
    }

    if (rrcmcm_is_dualmode_enabled_no_f3())
    {
      if (rrc_capability_update_requirement.system_specific_cap_update_req_present == TRUE) 
      /* For Single mode operation IE "UE system specific capability" will not
         be included even though UTRAN requests the same */
      {   
        uint8 count =0;
        boolean gsm_cap_req = FALSE;
  
        for(count = 0; count < rrc_capability_update_requirement.system_specific_cap_update_req_list.count && count < 16;count++)
        {
          if(rrc_capability_update_requirement.system_specific_cap_update_req_list.value[count] == RRC_GSM_CAP)
          {
            gsm_cap_req = TRUE;
            break;
          }
        }
        if(gsm_cap_req)
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.ueCapabilityInformation, ue_RATSpecificCapability);

          /* Make sure memory is allocated for the first element of the linked list */ 
          msg_ptr->message.u.ueCapabilityInformation.ue_RATSpecificCapability.n = 1;
          ALLOC_ASN1ARRAY2(&enc_ctxt,&msg_ptr->message.u.ueCapabilityInformation.ue_RATSpecificCapability,rrc_InterRAT_UE_RadioAccessCapability);

          rrcueci_append_gsm_specific_capability(
            &msg_ptr->message.u.ueCapabilityInformation.ue_RATSpecificCapability.elem[0]);
        }
      }
    }

    /* This places SDU into RLC watermark queue. */
    if(RRCSEND_SUCCESS== 
       rrcsend_send_ul_sdu(RRC_PROCEDURE_RCE,msg_ptr,ul_lc_id,
       RRCSEND_L2ACK_NOT_REQUIRED) )
    {
      status = RRCUECI_SUCCESS;

      WRRC_MSG0_HIGH("UE Capability Info Msg is sent");
    }
  }
  else
  { 
    WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
  } 
  
  return (status);            
} /* rrcueci_send_ue_capability_info_msg */
/*===========================================================================

FUNCTION rrcueci_convert_t304_to_ms

DESCRIPTION
  This function converts the rrc_T_304 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  int

SIDE EFFECTS
  None
===========================================================================*/
static int rrcueci_convert_t304_to_ms
(
  rrc_T_304 rrcueci_t_304                  /* enum for Timer T304 */
)
{
  int timer_value;                         /* Timer Value in milli seconds */

  switch (rrcueci_t_304)
  {
    case rrc_T_304_ms100:
     timer_value = 100;
      break;
    case rrc_T_304_ms200:
      timer_value = 200;
      break;      
    case rrc_T_304_ms400:
      timer_value = 400;
      break;
    case rrc_T_304_ms1000:
      timer_value = 1000;
      break;
    case rrc_T_304_ms2000:
      timer_value = 2000;
      break;
    default:
      WRRC_MSG0_HIGH("Unknown Timer value received from SIB1");
      timer_value = 100;  /* Set lowest timer value as a default */
      break;
  }
  return (timer_value);
} /* rrcueci_convert_t304_to_ms*/

/*===========================================================================

FUNCTION rrcueci_increment_v304_start_t304

DESCRIPTION
  This function increments counter V304 and starts the timer T304.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcueci_increment_v304_start_t304
(
  void
)
{
  uint32 Minimum_T304 = 0, T304_optimum = 0;
  /* Increment the counter V304 */
   rrc_v_304++;

  if(nv_gcf_test_flag)
  {
    Minimum_T304 = rrc_get_minimum_t304_reqd(ueci_message_length);
   /*Start T304 as Max of the min calculated value and network configured value*/
    T304_optimum = MAX((uint32)rrcueci_convert_t304_to_ms( RRC_GET_T304() ), Minimum_T304);

    /* Start the Timer T304 */
    rrctmr_start_timer(RRCTMR_T_304_TIMER, T304_optimum );
  }
  else
  {
    /* Start the Timer T304 */
    rrctmr_start_timer(RRCTMR_T_304_TIMER,
                     (uint32)rrcueci_convert_t304_to_ms( RRC_GET_T304() ));
  }
} /* rrcueci_increment_v304_start_t304*/

/*===========================================================================

FUNCTION rrcueci_process_ueci_initial_substate

DESCRIPTION
  This function processes RRC_INITIATE_UE_CAPABILITY_REQ event that is
  dispatched to UE Capability Information procedure in RRCUECI_INITIAL substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcueci_process_ueci_initial_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_cmd_type *int_cmd_ptr;            /* Pointer to the RRC Command */

  rrc_state_e_type rrc_state;           /* Local var to get rrc state */

  rrc_state = rrc_get_state();          /* initialize RRC state       */

  if(cmd_ptr->cmd_hdr.cmd_id == RRC_MODE_STATUS_IND)
  {
    if(rrcueci_ue_cap_enquiry_trans_id != RRCUECI_INVALID_TRANSACTION_ID)
    {
      /* UECI initiated due to UE Capability changed
       * exmp: Mode change from WCDMA only to WCDMA+GSM or vice versa,
       * If condition is when there is cross over Between UECI initiated due to UE 
       * Capability changed & UTRAN has sent UE Cap enquiry, So set Trans I.D 
       * in UECI message to the one received in  UE Cap enquiry message
       */
      rrcueci_ue_cap_enquiry_trans_id = (rrcueci_ue_cap_enquiry_trans_id + 1) & 0x3;
    }
    else
    {
      /* UECI initiated due to UE Capability changed
       * exmp: Mode change from WCDMA only to WCDMA+GSM or vice versa,
       * So UECI should not carry Transation I.D, So set it to RRCUECI_INVALID_TRANSACTION_ID,
       * Note: when we are in else then value of rrcueci_ue_cap_enquiry_trans_id 
       * will be RRCUECI_INVALID_TRANSACTION_ID, so below assignment operation is to
       * increase readability of code
       */
      rrcueci_ue_cap_enquiry_trans_id = RRCUECI_INVALID_TRANSACTION_ID;
    }

    WRRC_MSG1_HIGH("DUAL:TransId used during mode change is - %d", rrcueci_ue_cap_enquiry_trans_id);

    /* As the bands would have changed moving from W to W+G.
      Band validation would have been done by CSP */
    memset(&rrc_capability_update_requirement, 0, sizeof(rrc_capability_update_requirement_type));
    rrc_capability_update_requirement.ue_radio_capability_update_requirement = TRUE;
    rrc_capability_update_requirement.system_specific_cap_update_req_list.count = 1;
    rrc_capability_update_requirement.system_specific_cap_update_req_list.value[0] = RRC_GSM_CAP;
    rrc_capability_update_requirement.system_specific_cap_update_req_present = TRUE;
  }
  else
  {

    /* Verify the UE Capability Enquiry Transaction Id */
    if (rrcueci_ue_cap_enquiry_trans_id == 
        cmd_ptr->cmd.initiate_ue_cap.transaction_id)
    {
      /* Ignore the Transaction */
      return;
    }

    /* Update the UE Capability Enquiry Transaction Id */
    rrcueci_ue_cap_enquiry_trans_id = 
      cmd_ptr->cmd.initiate_ue_cap.transaction_id;
  }

  /* Set conter V304 to zero */
  rrc_v_304 = 0;
#ifdef FEATURE_WCDMA_REL7_SPECRS
  if(cmd_ptr->cmd.initiate_ue_cap.originator == RRC_UE_CAPABILITY_ORIGINATOR_COUEC)
  {
    rrcueci_ue_cap_originator = RRC_UE_CAPABILITY_ORIGINATOR_COUEC;
  }
#endif
  switch(rrc_get_state())
  {
    case RRC_STATE_URA_PCH:
    case RRC_STATE_CELL_PCH:
      /* Allocates the buffer to RRC Internal command */
      if( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
      {     
        /* Fill in the command parameters for RRC_CELL_UPDATE_INITIATE_REQ */
        int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
        int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_UL_DATA_TRANSMISSION;
        int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
        int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_UECI;
 
        rrc_put_int_cmd(int_cmd_ptr);             
      }
      else
      {
        ERR_FATAL("Unable to allocate memmory for: %d cmd", 
                   RRC_CELL_UPDATE_INITIATE_REQ,0,0);
      }
      rrcueci_substate = RRCUECI_WAIT_FOR_CELL_UPDATE_COMP_CNF;
      break;

    case RRC_STATE_CELL_FACH:

      if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_UECI) )
      {
        /* Wait until Cell update procedure is completed */
        rrcueci_substate = RRCUECI_WAIT_FOR_CELL_UPDATE_COMP_CNF;
      }
      else /* Cell Update procedure is inactive */
      {
        /* Build and send the UE Capability Information message to Send Chain */
        if(RRCUECI_SUCCESS != rrcueci_send_ue_capability_info_msg() )
        {
           WRRC_MSG2_HIGH("Failed to send UE Cap Info Msg: %d, %d", rrc_state, rrcueci_substate);
        }
        else
        {
          /* Increment the counter V304 and start T304*/
          rrcueci_increment_v304_start_t304();

          rrcueci_substate = RRCUECI_WAIT_FOR_T304_TO_EXPIRE;
        }
      }
      break;

    case RRC_STATE_CELL_DCH:
      
      /* Build and send the UE Capability Information message to Send Chain */
      if(RRCUECI_SUCCESS != rrcueci_send_ue_capability_info_msg() )
      {
         WRRC_MSG2_HIGH("Failed to send UE Cap Info Msg: %d, %d",rrc_state, rrcueci_substate);
      }
      else
      {
        /* Increment the counter V304 and start T304*/
        rrcueci_increment_v304_start_t304();

        rrcueci_substate = RRCUECI_WAIT_FOR_T304_TO_EXPIRE;
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the UECI Substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id, rrcueci_substate);
      break;
  }

} /* rrcueci_process_ueci_initial_substate */

/*===========================================================================

FUNCTION rrcueci_process_ueci_non_initial_substate

DESCRIPTION
  This function processes RRC_INITIATE_UE_CAPABILITY_REQ event that is
  dispatched to UE Capability Information procedure in substate other than
  RRCUECI_INITIAL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcueci_process_ueci_in_non_initial_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{

  if(cmd_ptr->cmd_hdr.cmd_id == RRC_MODE_STATUS_IND)
  {
    WRRC_MSG2_HIGH("DUAL:%d(0: UE, 1: UTRAN) initiated UECI procedure in substate : %d(1:WaitCUCnf, 2:WaitT304Exp",
                                         RRC_UE_CAPABILITY_ORIGINATOR_UE, rrcueci_substate);
  }
  else    
  {
    WRRC_MSG2_HIGH("%d(0: UE, 1: UTRAN) initiated UECI procedure in substate : %d(1:WaitCUCnf, 2:WaitT304Exp",
                    cmd_ptr->cmd.initiate_ue_cap.originator, rrcueci_substate);
  }
  /* If the originator is UE then it means that UE capabilities have changed, so UE should 
      restart UECI procedure. Otherwise no need to re-start UECI procedure.
  */
  if (RRC_UE_CAPABILITY_ORIGINATOR_UE == cmd_ptr->cmd.initiate_ue_cap.originator 
      || cmd_ptr->cmd_hdr.cmd_id == RRC_MODE_STATUS_IND
     )
  {

    /* if RRC reset UECI procedure before UECI procedure is complete then there is a possibility
        that UE may receive UECI confirm message from network with old transaction identifier.
        UE will drop this message and waits for the UECI confirm message with correct
        transaction identifier.
    */
    rrcueci_reset_ueci_procedure();
    rrcueci_process_ueci_initial_substate(cmd_ptr);
  }
  else
  {
    WRRC_MSG2_HIGH("Invalid event: %d in UECI Substate: %d in CELL_FACH",
                                                         cmd_ptr->cmd_hdr.cmd_id,rrcueci_substate);

  }          

  /*What if UECI procedure is active when INITIATE UE CAP REQ is initiated by COUEC Procedure.*/
}

/*===========================================================================

FUNCTION rrcueci_process_ueci_wait_for_cell_update_comp_cnf

DESCRIPTION
  This function processes all events that are  dispatched
  to IUE Capability Information procedure  in 
  RRCUECI_WAIT_FOR_CELL_UPDATE_COMP_CNF substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcueci_process_ueci_wait_for_cell_update_comp_cnf
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{

  rrc_state_e_type rrc_state;           /* Local var to get rrc state */

  rrc_state = rrc_get_state();          /* initialize RRC state       */

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      /* Build and send the UE Capability Information message to Send Chain */
      if(RRCUECI_SUCCESS != rrcueci_send_ue_capability_info_msg() )
      {
         WRRC_MSG2_HIGH("Failed to send UE Cap Info Msg: %d, %d",rrc_state, rrcueci_substate);
      }
      else
      {
        /* Increment the counter V304 and start T304*/
        rrcueci_increment_v304_start_t304();

        rrcueci_substate = RRCUECI_WAIT_FOR_T304_TO_EXPIRE;
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the UECI Substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcueci_substate);
      break;
  }
 
} /* rrcueci_process_ueci_wait_for_cell_update_comp_cnf */

/*===========================================================================

FUNCTION rrcueci_process_ueci_wait_for_t304_to_expire

DESCRIPTION
  This function processes all events that are  dispatched
  to UE Capability Information procedure  in RRCUECI_WAIT_FOR_T304_TO_EXPIRE
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcueci_process_ueci_wait_for_t304_to_expire
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_cmd_type *int_cmd_ptr;            /* Pointer to the RRC Command */

  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;  /* Pointer to Downlink DCCH */
  
  uint8 message_choice;                 /* Local varibale for message
                                           t */
  rlc_lc_id_type lc_id;                      /* Local variable to store 
                                                RLC LC Id */

  rrc_state_e_type rrc_state;           /* Local var to get rrc state */

  rrc_state = rrc_get_state();          /* initialize RRC state       */

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_T304_EXPIRED_IND:
      
      /* Gets N304 from rrc_ue_conn_timers_and_constants database */
      if(rrc_v_304 <= RRC_GET_N304())               
      {        
        /* Build and send the UE Capability Information message to Send 
           Chain */
        if(RRCUECI_SUCCESS != rrcueci_send_ue_capability_info_msg() )
        {
           WRRC_MSG2_HIGH("Failed to send UE Cap Info Msg: %d, %d",rrc_state, rrcueci_substate);
        }
        else
        {
          /* Increment the counter V304 and start T304*/
          rrcueci_increment_v304_start_t304();

          rrcueci_substate = RRCUECI_WAIT_FOR_T304_TO_EXPIRE;
        }
      }
      else /* Radio Link Failure */
      {
#ifdef FEATURE_DUAL_SIM
        rrc_csp_scan_e_type curr_scan = rrccsp_get_curr_scan();
		
        if(curr_scan == RRC_CSP_UNLOCK_BY_SCAN)
        {  
          WRRC_MSG1_HIGH("Restarting T304 timer with value: %d as UE has not resumed from tuneaway",RRCTMR_T304_EXPIRED_N304TH_TIME_IN_MS);
          rrctmr_start_timer(RRCTMR_T_304_TIMER,RRCTMR_T304_EXPIRED_N304TH_TIME_IN_MS);
          rrcueci_substate = RRCUECI_WAIT_FOR_T304_TO_EXPIRE;
          break;
        }
#endif

        /* Allocates the buffer to RRC Internal command */
        if( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
        {
          /* Fill in the command parameters for RRC_CELL_UPDATE_INITIATE_REQ */
          int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
          int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_RADIO_LINK_FAILURE;
          int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = FALSE;
          int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_UECI;
          rrc_put_int_cmd(int_cmd_ptr); 

          /* Set Transaction Id to invalid */
          rrcueci_ue_cap_enquiry_trans_id = RRCUECI_INVALID_TRANSACTION_ID;          

          /* Reset the UECI state machine */
          rrcueci_substate = RRCUECI_INITIAL;
        }
        else
        {
          ERR_FATAL("Memory alloc failure for CELL UPDATE", 0, 0, 0);
        }
      }
      break;

    case RRC_DOWNLINK_SDU_IND:
      
      rrc_protocol_error_reject = FALSE;
      
      /* Assert received message is Downlink DCCH message */
      if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == 
           rrc_DL_DCCH_Message_PDU) 
      {
        message_choice = 
          rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

        if (cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
        {
          dcch_msg_ptr = (rrc_DL_DCCH_Message *) 
            cmd_ptr->cmd.downlink_sdu_ind.decoded_msg; 
        }
        else
        { /* ASN1 decoding failed */
          rrc_protocol_error_reject = TRUE;
        } 
        /* Assert the received message is UE Capability Information
           Confirm */
        if(  message_choice ==
              T_rrc_DL_DCCH_MessageType_ueCapabilityInformationConfirm )
        {
          WRRC_MSG1_HIGH("UE Cap Info Confirm Msg is received,stored Trans I.D = 0x%x",
                      rrcueci_ue_cap_enquiry_trans_id);

          /* Stop timer T304 */
          rrctmr_stop_timer(RRCTMR_T_304_TIMER);

          if(dcch_msg_ptr == NULL) /* DCCH ASN1 decoding failed */
          {
            lc_id = rrclcm_check_ul_rlc_lc_id(    
                    UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                    DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                                the above Downlink Logical 
                                                Channel type. */
                    UE_MODE_ACKNOWLEDGED_DATA);
                                              /* Mode of transmission for the RLC 
                                                 service entity. */
            if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
            {
              WRRC_MSG0_HIGH("Cann't find the RLC Id for RB 2");
            }       
            else /* Valid RLC Id */
            {
              if(SUCCESS != rrcsend_send_rrc_status_message(
                 T_rrc_ProtocolErrorMoreInformation_type1_asn1_ViolationOrEncodingError,
                                               // The protocol error cause to be placed in
                                               // the message
                 0,                            // Set transaction id to 0 since ASN1 
                                               // decoding failed 
                 rrc_ReceivedMessageType_ueCapabilityInformationConfirm, 
                                               // Received message identification
                 RRC_PROCEDURE_UECI,           // RRC procedure
                 lc_id,                        /* Logical Channel Id */
                 RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                         /* Indicates whether L2 Ack required or not */
              {
                WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
              } 
            } /* Valid RLC Id */
          }
          else /* ASN1 decoding is successful */
          {
            if(dcch_msg_ptr->message.u.ueCapabilityInformationConfirm.t !=
               T_rrc_UECapabilityInformationConfirm_later_than_r3 )
            {
              /* If rrcueci_ue_cap_enquiry_trans_id is RRCUECI_INVALID_TRANSACTION_ID
               * Then it means ueci initiated due to UE Capability changed
               * exmp: Mode change from WCDMA only to WCDMA+GSM or vice versa,
               * So accept any transaction I.D in UECI Confirmation message.
               */
              if ((dcch_msg_ptr->message.u.ueCapabilityInformationConfirm.
                  u.r3.ueCapabilityInformationConfirm_r3.
                  rrc_TransactionIdentifier ==  rrcueci_ue_cap_enquiry_trans_id)||
                  (rrcueci_ue_cap_enquiry_trans_id == RRCUECI_INVALID_TRANSACTION_ID)
                  )
              {
                WRRC_MSG0_HIGH("Process UE Cap Info Confirm Msg as Trans I.D match");
                /* Procedure ends */
                rrcueci_reset_ueci_procedure();

                /* Set Transaction Id to invalid */
                rrcueci_ue_cap_enquiry_trans_id = RRCUECI_INVALID_TRANSACTION_ID;

             
#ifdef FEATURE_WCDMA_REL7_SPECRS
                if((dcch_msg_ptr->message.u.ueCapabilityInformationConfirm.u.
                    r3.m.laterNonCriticalExtensionsPresent) &&
                (dcch_msg_ptr->message.u.ueCapabilityInformationConfirm.u.r3.
                laterNonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
                (dcch_msg_ptr->message.u.ueCapabilityInformationConfirm.u.r3.
                laterNonCriticalExtensions.v770NonCriticalExtensions.ueCapabilityInformationConfirm_v770ext.m.
                acceptanceOfChangeOfCapabilityPresent))
                {
                  rrc_UECapabilityInformationConfirm_v770ext_IEs * ueci_cnf_v770ext;
                  ueci_cnf_v770ext = &dcch_msg_ptr->message.u.ueCapabilityInformationConfirm.u.r3.
                    laterNonCriticalExtensions.v770NonCriticalExtensions.ueCapabilityInformationConfirm_v770ext;
                  
                  if(ueci_cnf_v770ext->acceptanceOfChangeOfCapability == rrc_UECapabilityInformationConfirm_v770ext_IEs_acceptanceOfChangeOfCapability_refused)
                  {
                    rrccouec_process_completion_of_ueci_req(CHANGE_OF_CAP_REFUSED);
                  }
                  else if(ueci_cnf_v770ext->acceptanceOfChangeOfCapability == rrc_UECapabilityInformationConfirm_v770ext_IEs_acceptanceOfChangeOfCapability_accepted)
                  {
                    rrccouec_process_completion_of_ueci_req(CHANGE_OF_CAP_ACCEPTED);
                  }
                  else if(ueci_cnf_v770ext->acceptanceOfChangeOfCapability == rrc_UECapabilityInformationConfirm_v770ext_IEs_acceptanceOfChangeOfCapability_acceptedWithReconfigurationToFollow)
                  {
                    rrccouec_process_completion_of_ueci_req(CHANGE_OF_CAP_WITH_RECONFIG);
                  }
                  else
                  {
                    rrccouec_process_completion_of_ueci_req(CHANGE_OF_CAP_ACCEPTED);
                  }				  
                }
#endif
              }
    
              else /* Invalid message is received */
              {
                lc_id = rrclcm_check_ul_rlc_lc_id(    
                    UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                    DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                                the above Downlink Logical 
                                                Channel type. */
                    UE_MODE_ACKNOWLEDGED_DATA);
                                              /* Mode of transmission for the RLC 
                                                 service entity. */
                if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
                {
                  WRRC_MSG0_HIGH("Cann't find the RLC Id for RB 2");
                }       
                else /* Valid RLC Id */
                {
                  if(SUCCESS != rrcsend_send_rrc_status_message(
                     T_rrc_ProtocolErrorMoreInformation_type1_ie_ValueNotComprehended,
                                                   // The protocol error cause to be placed in
                                                   // the message
                     0,                            // Set transaction id to 0 since ASN1 
                                                   // decoding failed 
                     rrc_ReceivedMessageType_ueCapabilityInformationConfirm, 
                                                   // Received message identification
                     RRC_PROCEDURE_UECI,           // RRC procedure
                     lc_id,                        /* Logical Channel Id */
                     RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                             /* Indicates whether L2 Ack required or not */
                  {
                    WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
                  } 
                  else
                  {
                    WRRC_MSG0_HIGH("Tranmited RRC STATUS");
                    /* Start the Timer T304 */
                    rrctmr_start_timer(RRCTMR_T_304_TIMER,
                     (uint32)rrcueci_convert_t304_to_ms( RRC_GET_T304() ));
                  }
                } /* Valid RLC Id */


              }
            }
            else
            { 
              /* Critical extenisons are not supported.  */
              WRRC_MSG0_ERROR("Critical extensions are not supported");

              lc_id = rrclcm_check_ul_rlc_lc_id(    
                    UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                    DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                                the above Downlink Logical 
                                                Channel type. */
                    UE_MODE_ACKNOWLEDGED_DATA);
                                              /* Mode of transmission for the RLC 
                                                 service entity. */
              if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
              {
                WRRC_MSG0_HIGH("Cann't find the RLC Id for RB 2");
              }       
              else /* Valid RLC Id */
              {
                if(SUCCESS != rrcsend_send_rrc_status_message(
                   T_rrc_ProtocolErrorMoreInformation_type1_messageExtensionNotComprehended,
                                                 // The protocol error cause to be placed in
                                                 // the message
                   dcch_msg_ptr->message.u.ueCapabilityInformationConfirm.
                   u.r3.ueCapabilityInformationConfirm_r3.
                   rrc_TransactionIdentifier,                            
                                                 // Set transaction id received transaction 

                   rrc_ReceivedMessageType_ueCapabilityInformationConfirm, 
                                                 // Received message identification
                   RRC_PROCEDURE_UECI,           // RRC procedure
                   lc_id,                        /* Logical Channel Id */
                   RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                           /* Indicates whether L2 Ack required or not */
                {
                  WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
                }
                else
                {
                  WRRC_MSG0_HIGH("Tranmited RRC STATUS");
                  /* Start the Timer T304 */
                  rrctmr_start_timer(RRCTMR_T_304_TIMER,
                   (uint32)rrcueci_convert_t304_to_ms( RRC_GET_T304() ));
                }
              } /* Valid RLC Id */
            }
          }
        }
        else
        {
          WRRC_MSG1_HIGH("Invalid message; % d is forwarded",
                    message_choice);
        }
      }
      else
      {
        WRRC_MSG1_HIGH("Invalid SDU type %d is received",
                  cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
      }
      break;       
      
    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the UECI Substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcueci_substate);
      break;
  }
 
} /* rrcueci_process_ueci_wait_for_t304_to_expire */
/*===========================================================================

FUNCTION rrcueci_process_disconnected_state

DESCRIPTION
  This function processes all events that are dispatched to UE Capability 
  Information procedure  in Disconnected state of Idle mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcueci_process_disconnected_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  if  (RRC_STATE_CHANGE_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    /* if new_state is RRC_STATE_DISCONNECTED then reset UECI procedure in
       in all UECI sub_states. So no need to check for UECI sub_state.
    */
    if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
    {
      WRRC_MSG1_HIGH("Reset UECI procedure as RRC state is Disconneted. Cur UECI state: %d",
                                                         rrcueci_substate);
      /* Reset the UECI state machine */
      rrcueci_reset_ueci_procedure();

      /* Set Transaction Id to invalid */
      rrcueci_ue_cap_enquiry_trans_id = RRCUECI_INVALID_TRANSACTION_ID;
    }
    else
    {
      WRRC_MSG0_ERROR("RRCSCMGR posted unregistered event to UECI procedure");
    }
  }
  else
  {
    WRRC_MSG1_ERROR("Unexpected cmd id: %d. to UECI procedure in Disconnected RRC State",
                                                                      cmd_ptr->cmd_hdr.cmd_id);
  }
}/* rrcueci_process_disconnected_state */

/*===========================================================================

FUNCTION rrcueci_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to UE Capability 
  Information procedure  in CELL_FACH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcueci_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcueci_substate)
  {
    case RRCUECI_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_MODE_STATUS_IND:
        case RRC_INITIATE_UE_CAPABILITY_REQ:
          rrcueci_process_ueci_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG2_HIGH("Invalid event: %d in UECI Substate: %d in CELL_FACH",
                    cmd_ptr->cmd_hdr.cmd_id,rrcueci_substate);
         break;
      }
      break;

    case RRCUECI_WAIT_FOR_CELL_UPDATE_COMP_CNF:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_CELL_UPDATE_COMPLETE_CNF:
          rrcueci_process_ueci_wait_for_cell_update_comp_cnf(cmd_ptr);
          break;
        
        case RRC_MODE_STATUS_IND:
        case RRC_INITIATE_UE_CAPABILITY_REQ:
          rrcueci_process_ueci_in_non_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG2_HIGH("Invalid event: %d in UECI Substate: %d in CELL_FACH",
                    cmd_ptr->cmd_hdr.cmd_id,rrcueci_substate);
          break;
      }
      break;

    case RRCUECI_WAIT_FOR_T304_TO_EXPIRE: 
                                 /* UECI waits for T304 to expire */
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_T304_EXPIRED_IND:
          rrcueci_process_ueci_wait_for_t304_to_expire(cmd_ptr);
          break;

        case RRC_DOWNLINK_SDU_IND:
          rrcueci_process_ueci_wait_for_t304_to_expire(cmd_ptr);
          break;

        case RRC_MODE_STATUS_IND:
        case RRC_INITIATE_UE_CAPABILITY_REQ:
          rrcueci_process_ueci_in_non_initial_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG2_HIGH("Invalid event: %d in UECI Substate: %d in CELL_FACH",
                    cmd_ptr->cmd_hdr.cmd_id,rrcueci_substate);
          break;
      }
      break;

   default:           
      WRRC_MSG1_HIGH("Invalid UECI Substate: %d in CELL_FACH",
                rrcueci_substate);
      break;
  }  
}/* rrcueci_process_cell_fach_state */
/*===========================================================================

FUNCTION rrcueci_process_cell_dch_state

DESCRIPTION
  This function processes all events that are dispatched to UE Capability 
  Information procedure  in CELL_DCH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcueci_process_cell_dch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcueci_substate)
  {
    case RRCUECI_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_MODE_STATUS_IND:
        case RRC_INITIATE_UE_CAPABILITY_REQ:
          rrcueci_process_ueci_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG2_HIGH("Invalid event: %d in UECI Substate: %d",
                    cmd_ptr->cmd_hdr.cmd_id,rrcueci_substate);
          break;
      }
      break;

    case RRCUECI_WAIT_FOR_T304_TO_EXPIRE: 
                                 /* UECI waits for T304 to expire */
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_T304_EXPIRED_IND:
          rrcueci_process_ueci_wait_for_t304_to_expire(cmd_ptr);
          break;

        case RRC_DOWNLINK_SDU_IND:
          rrcueci_process_ueci_wait_for_t304_to_expire(cmd_ptr);
          break;  
     
        case RRC_MODE_STATUS_IND:
        case RRC_INITIATE_UE_CAPABILITY_REQ:
          rrcueci_process_ueci_in_non_initial_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG2_HIGH("Invalid event: %d in UECI Substate: %d",
                    cmd_ptr->cmd_hdr.cmd_id,rrcueci_substate);
          break;
      }
      break;

   default:           
      WRRC_MSG1_HIGH("Invalid UECI Substate: %d in CELL_DCH",
                rrcueci_substate);
      break;
  }  
}/* rrcueci_process_cell_dch_state */

/*===========================================================================

FUNCTION rrcueci_process_cell_ura_pch_state

DESCRIPTION
  This function processes all events that are dispatched to UE Capability 
  Information procedure  in either in CELL_PCH or URA_PCH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcueci_process_cell_ura_pch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcueci_substate)
  {
    case RRCUECI_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_MODE_STATUS_IND:
        case RRC_INITIATE_UE_CAPABILITY_REQ:
          rrcueci_process_ueci_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG2_HIGH("Invalid event: %d in UECI Substate: %d",
                    cmd_ptr->cmd_hdr.cmd_id,rrcueci_substate);
          break;
      }
      break;

    default:           
      WRRC_MSG1_HIGH("Invalid UECI Substate:%d in PCH",
                rrcueci_substate);
      break;
  }  
}/* rrcueci_process_cell_ura_pch_state */

/*===========================================================================

FUNCTION rrcueci_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the UE Capability Enquiry procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC) containing UE 
    CAPABILITY INFORMATION CONFIRM message.

    RRC_INITIATE_UE_CAPABILITY_REQ Primitive
    RRC_STATE_CHANGE_IND Primitive
    RRC_CELL_UPDATE_COMPLETE_CNF Primitive

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcueci_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrcueci_procedure_event_handler is called in rrc_state rrc_state_e_type_value%d in rrcueci_substate rrcueci_substate_e_type_value%d", rrc_state,rrcueci_substate);

  /* if RRC Connection release procedure is active then ignore the command,
      also reset UECI procedure variables.
  */
  rrc_crash_debug_log_proc_substates(RRC_PROCEDURE_UECI,rrcueci_substate,cmd_ptr);
#ifdef FEATURE_WCDMA_TO_LTE
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_LTE_RRC_CMD) && 
     (cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == LTE_RRC_EUTRA_CAPABILITIES_RSP))
  {
    if (cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.lte_rrc_eutra_capabilites_rsp.dest_rat == LTE_RRC_IRAT_UTRA_FDD)
    {
      rrcueci_save_eutra_capability(&cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.lte_rrc_eutra_capabilites_rsp);
    }
    else
    {
      WRRC_MSG1_HIGH("Rcvd LTE_RRC_EUTRA_CAPABILITIES_RSP for other tech: %d. Ignored",
                  cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.lte_rrc_eutra_capabilites_rsp.dest_rat);
    }

    return;
  }
#endif

  if (FALSE == rrcrcr_is_proc_active())
  {
    switch (rrc_state)
    {
      case RRC_STATE_DISCONNECTED: 
        rrcueci_process_disconnected_state(cmd_ptr);
        break;
  
      case RRC_STATE_CELL_FACH:    
        rrcueci_process_cell_fach_state(cmd_ptr); 
        break;
  
      case RRC_STATE_CELL_DCH:  
        rrcueci_process_cell_dch_state(cmd_ptr); 
        break;
  
      case RRC_STATE_CELL_PCH:
      case RRC_STATE_URA_PCH:
        rrcueci_process_cell_ura_pch_state(cmd_ptr);
        break;
  
      default:  
        WRRC_MSG2_HIGH("Event: %d is fwded in invalid State: %d",
                  cmd_ptr->cmd_hdr.cmd_id,rrc_state);
        break;
     }  
  }
  else
  {
    WRRC_MSG0_HIGH("Reset UECI procedure as it received cmd when RCR is in progress");  
    rrcueci_reset_ueci_procedure();

    /* Set Transaction Id to invalid */
    rrcueci_ue_cap_enquiry_trans_id = RRCUECI_INVALID_TRANSACTION_ID;
  }
} /* rrcueci_procedure_event_handler */
/*===========================================================================

FUNCTION rrcueci_init_procedure

DESCRIPTION
  This function initializes required parameters for UE Capability Information
  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcueci_init_procedure(void)
{
  /* Set UECI procedure state to RRCUECI_INITIAL at powerup */
  rrcueci_substate = RRCUECI_INITIAL;

  /* Set the UE Capability Transaction Id to invalid */
  rrcueci_ue_cap_enquiry_trans_id = RRCUECI_INVALID_TRANSACTION_ID;


#ifdef FEATURE_WCDMA_REL7_SPECRS
  rrcueci_ue_cap_originator = RRC_UE_CAPABILITY_ORIGINATOR_NONE;
#endif
  /* Register for transition from any state to Disconnected state */
  rrcscmgr_register_for_scn(RRC_PROCEDURE_UECI,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_DISCONNECTED);
#ifdef FEATURE_WCDMA_TO_LTE
  rrcueci_interrat_lte_capability_info.lte_req_posted = rrcueci_interrat_lte_capability_info.lte_rsp_received = FALSE;
#endif
}



/*===========================================================================

FUNCTION rrcueci_get_nv_items

DESCRIPTION
  This  function gets NV item NV_HSDPA_CAT_I and stores it in the global variable

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcueci_get_nv_items
(
  sys_modem_as_id_e_type wrrc_as_id
)
{

  nv_hsdpa_category = WNV_GET_DEFAULT(WNV_ID_HSDPA_CAT_I);

#if  ((defined FEATURE_HSPA_PLUS_CAT29)||defined(FEATURE_HSPA_PLUS_CAT24) || defined(FEATURE_HSPA_PLUS_CAT20) \
   || defined(FEATURE_HSPA_PLUS_CAT18) || defined(FEATURE_HSPA_PLUS_CAT16) \
   || defined (FEATURE_HSPA_PLUS_CAT14) || defined (FEATURE_HSPA_PLUS_CAT10) \
   || defined (FEATURE_HSPA_PLUS_CAT9) || defined (FEATURE_HSPA_PLUS_CAT8) \
   || defined(FEATURE_HSPA_PLUS_CAT6))

  /*read HSDPA Category Variable from NV*/
  wnv_get_nv_msim(WNV_ID_HSDPA_CAT_I, &nv_hsdpa_category, sizeof(nv_hsdpa_category), wrrc_as_id);

  if ((nv_hsdpa_category == 8) || (nv_hsdpa_category == 6) || (nv_hsdpa_category == 12)
#ifdef FEATURE_HSPA_PLUS_CAT24
      ||(nv_hsdpa_category == 24)
#endif /*FEATURE_HSPA_PLUS_CAT24*/
#ifdef FEATURE_HSPA_PLUS_CAT29
      ||(nv_hsdpa_category == 29)
#endif /*FEATURE_HSPA_PLUS_CAT24*/
#ifdef FEATURE_HSPA_PLUS_CAT9
      ||(nv_hsdpa_category == 9)
#endif
#ifdef FEATURE_HSPA_PLUS_CAT10
      || (nv_hsdpa_category == 10)
#endif
    || (nv_hsdpa_category > 8)
  )
  {
    WRRC_MSG1_HIGH("HSDPACATEGORY %d", nv_hsdpa_category);
  }
  else
  {

    nv_hsdpa_category = WNV_GET_DEFAULT(WNV_ID_HSDPA_CAT_I);
    WRRC_MSG1_HIGH("HSDPACATEGORY FORCIBLY %d", nv_hsdpa_category);
  }
#ifdef FEATURE_HSPA_PLUS_CAT10
  if (nv_hsdpa_category > 12)
  {
    nv_hsdpa_category = WNV_GET_DEFAULT(WNV_ID_HSDPA_CAT_I);
    WRRC_MSG1_HIGH("HSDPACATEGORY FORCIBLY %d", nv_hsdpa_category);
  }
#endif
#ifdef FEATURE_WCDMA_3C_HSDPA
  if (nv_hsdpa_category >= 29 )
  {
    nv_hsdpa_category = RRC_DEFAULT_3C_HSDPA_CATEGORY;
    WRRC_MSG1_HIGH("HSDPACATEGORY FORCIBLY %d", nv_hsdpa_category);
  }
#endif

#else
  WRRC_MSG1_HIGH("HSDPACATEGORY %d", nv_hsdpa_category);
#endif

  if((nv_hsdpa_category > 14)&&(mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_HSPA_ABOVE_CAT14) == MCS_MODEM_CAP_UNAVAILABLE))
  {
    WRRC_MSG1_HIGH("HSDPACATEGORY %d is unsupported hence FORCIBLY set to 14", nv_hsdpa_category);
    nv_hsdpa_category= 14;
  }
#ifdef FEATURE_WCDMA_3C_HSDPA
  if(nv_hsdpa_category >= 29)
  {
    nv_hsdpa_category = RRC_DEFAULT_3C_HSDPA_CATEGORY;
  }
else
#endif
#ifdef FEATURE_WCDMA_DC_HSDPA
  if(nv_hsdpa_category > 20) 
  {
    nv_hsdpa_category = 24;
  }
#endif
}

#ifdef FEATURE_GANSS_SUPPORT
/*===========================================================================
FUNCTION rrcueci_fill_pos_cap_r7

DESCRIPTION
  This function fill positioning capability  for
  Release 7.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcueci_fill_pos_cap_r7(rrc_UE_RadioAccessCapability_v770ext_IEs * rel7_ue_capablity)
{
  uint8 i = 0;
  rrc_NetworkAssistedGANSS_Supported_List *ganss_list_ptr = NULL;
  
 #if defined (FEATURE_MMGPS) || defined (FEATURE_CGPS_UMTS_CP_WCDMA)
 #ifdef FEATURE_MMGPS
   lsm_ue_pos_capability_struct_type pos_capability;
 #else
   tm_umts_cp_wcdma_ue_pos_capability_struct_type pos_capability;
 #endif
 #endif

 #ifdef FEATURE_MMGPS
   lsm_wcdma_ue_pos_capability_get(&pos_capability);
 #else
 if(rrcgps_cgps_ue_pos_capability_cb == NULL)
 {
   ERR_FATAL("GPS Call back function is NULL",0,0,0);
 }
 else
 {      
   rrcgps_cgps_ue_pos_capability_cb(&pos_capability);
 }
 #endif
#ifdef FEATURE_GANSS_SUPPORT
  rrc_ganss_supported = pos_capability.ganss_supported;
  #endif
 if(pos_capability.ganss_supported == TRUE)
 {
   
   rel7_ue_capablity->ue_PositioningCapability.m.ue_GANSSPositioning_CapabilityPresent = 1;
   WRRC_MSG2_HIGH("GANSS : GANSS SUPPORTED LIST INCL is %d, list_cnt : %d",pos_capability.ganss_supported_list_incl,pos_capability.list_cnt);
   if(pos_capability.ganss_supported_list_incl)
   {
     if(pos_capability.list_cnt)
     {
       rel7_ue_capablity->ue_PositioningCapability.ue_GANSSPositioning_Capability.m.
        networkAssistedGANSS_supportedListPresent = 1;
       ganss_list_ptr = &(rel7_ue_capablity->ue_PositioningCapability.ue_GANSSPositioning_Capability.
        networkAssistedGANSS_supportedList);
       ganss_list_ptr->n = pos_capability.list_cnt;
       ALLOC_ASN1ARRAY2(&enc_ctxt,ganss_list_ptr,rrc_NetworkAssistedGANSS_Supported_List_element);
       if(ganss_list_ptr->elem == NULL)
       {
         ERR_FATAL("WTOL: Memory allocation failure.",0,0,0);
       }

       for(i= 0 ; i< ganss_list_ptr->n ; i++)
       {
         MSG_HIGH("GANSS : GANSS ID INCL is %d GANSS sig id incl: %d GANSS mode : %d",pos_capability.ganss_supported_list[i].ganss_id_incl,pos_capability.ganss_supported_list[i].ganss_sig_id_incl,pos_capability.ganss_supported_list[i].ganss_mode);
         if(pos_capability.ganss_supported_list[i].ganss_id_incl)
         {
           ganss_list_ptr->elem[i].m.gANSS_IdPresent = 1;
           ganss_list_ptr->elem[i].gANSS_Id = (uint32)pos_capability.ganss_supported_list[i].ganss_id;
         }
         ganss_list_ptr->elem[i].gANSS_Mode = (uint32)pos_capability.ganss_supported_list[i].ganss_mode;
         if(pos_capability.ganss_supported_list[i].ganss_sig_id_incl)
         {
           ganss_list_ptr->elem[i].m.gANSS_SignalIdPresent = 1;
           ganss_list_ptr->elem[i].gANSS_SignalId = (uint8)pos_capability.ganss_supported_list[i].ganss_sig_id;
           WRRC_MSG1_HIGH("GANSS : GANSS SIG ID  is %d ",pos_capability.ganss_supported_list[i].ganss_sig_id);
         }
         WRRC_MSG2_HIGH("GANSS : GANSS timing CF %d GANSS carrier phase incl: %d",
           pos_capability.ganss_supported_list[i].ganss_timing_of_cell_frames,
           pos_capability.ganss_supported_list[i].ganss_carrier_phase_incl);
         ganss_list_ptr->elem[i].supportForUE_GANSS_TimingOfCellFrames = pos_capability.ganss_supported_list[i].
                                                                          ganss_timing_of_cell_frames;
         if(pos_capability.ganss_supported_list[i].ganss_carrier_phase_incl)
         {
           ganss_list_ptr->elem[i].m.supportForUE_GANSS_CarrierPhaseMeasurementPresent = 1;
           ganss_list_ptr->elem[i].supportForUE_GANSS_CarrierPhaseMeasurement = pos_capability.ganss_supported_list[i].
                                                                                 ganss_carrier_phase_meas;
           WRRC_MSG1_HIGH("GANSS : GANSS Carrier PM  is %d ",pos_capability.ganss_supported_list[i].
                                                                                 ganss_carrier_phase_meas);
         }
       }

     }
     
   }
 }
}

#endif

/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_rel7_capability

DESCRIPTION
  This function appends the Radio Access Capability to the message for
  Release 7.
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 void  rrcueci_append_ue_radio_access_capability_rel7_capability
 (
   
    rrc_UE_CapabilityContainer_IEs *ue_capability_container,
    rrcueci_cap_update_cause_e_type update_cause
 )
{

  rrc_UE_RadioAccessCapability_v770ext_IEs * rel7_ue_capablity = &ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
                                                           v770NonCriticalExtensions.ue_RadioAccessCapability_v770ext;
#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
  rrc_UE_RadioAccessCapability_v790ext_IEs * rel7_ue_capablity_v790ext;
#endif /*FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST*/
  RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions);
     
  if(rel7_ue_capablity != NULL )
  {
    RRC_RESET_MSG_IE_PRESENT_PTR(rel7_ue_capablity);
    if(update_cause == RRCUECI_CAP_UPDATE)
    {
      RRC_RESET_MSG_IE_PRESENT_PTR(rel7_ue_capablity);
      RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability,
        rrc_PhysicalChannelCapability_v770ext,fddPhysChCapability);
      RRC_RESET_MSG_IE_PRESENT(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability);
      RRC_RESET_MSG_IE_PRESENT(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.uplinkPhysChCapability);
  
      /*MAC-EHS support can be used even for lower than 12 categories  */
      RRC_MSG_COMMON_SET_BITMASK_IE_PTR(rel7_ue_capablity,
      rrc_UE_RadioAccessCapability_v770ext_IEs,mac_ehsSupport);
    
      rel7_ue_capablity->mac_ehsSupport = rrc_UE_RadioAccessCapability_v770ext_IEs_mac_ehsSupport_true;
        
        
#ifdef FEATURE_WCDMA_REL7_SPECRS
      if((rrcueci_ue_cap_originator == RRC_UE_CAPABILITY_ORIGINATOR_COUEC) && 
          (rrccouec.ue_cap_change_requested.is_valid_mask & HSDPA_CAT_INCL) &&(rrccouec.ue_cap_change_requested.hsdpa_category >=8))
      {
        if(rrccouec.ue_cap_change_requested.hsdpa_category > rrc_macehs_cat)
        {
          RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability,
            rrc_DL_PhysChCapabilityFDD_v770ext,hsdsch_physical_layer_category_ext);
          rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.hsdsch_physical_layer_category_ext = 
            rrc_macehs_cat;
          WRRC_MSG1_HIGH("REL8:Using HSDPA CAT %d",rrc_macehs_cat);
        }
        else
        {
          RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability,
            rrc_DL_PhysChCapabilityFDD_v770ext,hsdsch_physical_layer_category_ext);
          rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.hsdsch_physical_layer_category_ext = 
            rrccouec.ue_cap_change_requested.hsdpa_category;
        }
      }
      else if((rrc_ue_cap_during_rrc_conn.is_valid_mask & HSDPA_CAT_INCL)&& (rrc_ue_cap_during_rrc_conn.hsdpa_category >=8))
      {
        if(rrc_ue_cap_during_rrc_conn.hsdpa_category > rrc_macehs_cat)
        {
          RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability, 
            rrc_DL_PhysChCapabilityFDD_v770ext,hsdsch_physical_layer_category_ext);
          rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.hsdsch_physical_layer_category_ext = 
            rrc_macehs_cat;
          WRRC_MSG1_HIGH("REL8:Using HSDPA CAT %d",rrc_macehs_cat);
        }
        else
        {
          RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability,
            rrc_DL_PhysChCapabilityFDD_v770ext,hsdsch_physical_layer_category_ext);
          rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.hsdsch_physical_layer_category_ext = 
                    rrc_ue_cap_during_rrc_conn.hsdpa_category;
        }
      }
      else  
#endif
         /* At present filling HS category in Ext field if HS category is more than 8, as E// requires.
           * even though maybe this needs to be filled only if HS category is more than 12*/

      if(nv_hsdpa_category > rrc_macehs_cat)
      {
        RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability,
         rrc_DL_PhysChCapabilityFDD_v770ext,hsdsch_physical_layer_category_ext);

        rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.hsdsch_physical_layer_category_ext = 
            rrc_macehs_cat;
        WRRC_MSG1_HIGH("REL8:Using HSDPA CAT %d for R7 UTRANs",rrc_macehs_cat);
      }
      else
      if(nv_hsdpa_category >= 8 )
      {
        RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability,
         rrc_DL_PhysChCapabilityFDD_v770ext,hsdsch_physical_layer_category_ext);

        rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.hsdsch_physical_layer_category_ext =
            nv_hsdpa_category;
      }
  
  
     /* Spec 25.306 v760 Table 5.1
      *A UE that supports categories greater or equal to category 13, also supports E-DPDCH.
      *A UE that supports categories greater or equal to category 13, also supports MAC-ehs.
     */
  
      if(nv_hsdpa_category > 12 )
      {
  #ifdef FEATURE_WCDMA_MIMO
        if(nv_hsdpa_category < 15 )
        {
          WRRC_MSG1_HIGH("RRCHSPA+: MIMO feature defined but NV returns %d , should be 15 or above ",nv_hsdpa_category);
        }
  #endif       
      }
      else
      { 
        WRRC_MSG1_HIGH("RRCHSPA+: 64QAM feature defined but NV returns %d , should be 13 or 14",nv_hsdpa_category);
      }
      if(!(rrc_disable_cpc_fdpch_nv & (NV_FDPCH_DISABLE|NV_EFDPCH_DISABLE)))
      {
        RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability,
                            rrc_DL_PhysChCapabilityFDD_v770ext,enhancedFdpch);
        rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.enhancedFdpch = 
          rrc_DL_PhysChCapabilityFDD_v770ext_enhancedFdpch_true;
      }

#ifdef FEATURE_WCDMA_HS_FACH
      if(rrcenhstate_nv_supports_e_fach() == TRUE)
      {
        rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.m.hsdschReception_CellFachPresent = 1;
        rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.hsdschReception_CellFach = 
          rrc_DL_PhysChCapabilityFDD_v770ext_hsdschReception_CellFach_true;  
  
      }
#endif /*FEATURE_WCDMA_HS_FACH*/
  
      /*PSR At present setting this FALSE*/
      rel7_ue_capablity->rlc_Capability.supportOfTwoLogicalChannel = FALSE;
      /*GAN capability */
      RRC_RESET_MSG_IE_PRESENT(rel7_ue_capablity->multiModeRAT_Capability);
  
      RRC_RESET_MSG_IE_PRESENT(rel7_ue_capablity->ue_PositioningCapability);
#ifdef FEATURE_GANSS_SUPPORT
      /* r8 ganss capabilities are not supported by MSM */
      if(rrc_ganss_support_nv == TRUE)
      {
      rrcueci_fill_pos_cap_r7(rel7_ue_capablity);
      }
      else
      {
         WRRC_MSG0_HIGH("GANSS NV not set");
      }
#endif

      if(rrcueci_nv_supports_cpc_dtx_on_dpcch())
      {
        RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.uplinkPhysChCapability,
          rrc_UL_PhysChCapabilityFDD_v770ext,discontinuousDpcchTransmission);
        rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.uplinkPhysChCapability.
           discontinuousDpcchTransmission = rrc_UL_PhysChCapabilityFDD_v770ext_discontinuousDpcchTransmission_true;

        WRRC_MSG0_HIGH("RRCCPC:Indicate UE support CPC-DTX operation on UL DPCCH & UE support Slot Format #4 ");
        RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.uplinkPhysChCapability,
          rrc_UL_PhysChCapabilityFDD_v770ext,slotFormat4);
        rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.uplinkPhysChCapability.
          slotFormat4 = rrc_UL_PhysChCapabilityFDD_v770ext_slotFormat4_true;
      }

    }
  }
#ifdef FEATURE_WCDMA_16_QAM
  if(CHECK_WCDMA_OPTIONAL_FEATURE_16_QAM_SUPPORTED)
  {
    RRC_MSG_COMMON_SET_BITMASK_IE(rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.uplinkPhysChCapability,
     rrc_UL_PhysChCapabilityFDD_v770ext,edch_PhysicalLayerCategory_extension);
    rel7_ue_capablity->physicalChannelCapability.fddPhysChCapability.uplinkPhysChCapability.edch_PhysicalLayerCategory_extension = MAX_UL_16QAM_HSUPA_CATEGORY;
  }
#endif
#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
    if(TRUE == rrcueci_nv_supports_edpcch_pwr_boost())
    {
      ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
     v770NonCriticalExtensions.m.v790NonCriticalExtensionsPresent = 1;

      RRC_RESET_MSG_IE_PRESENT(ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
             v770NonCriticalExtensions.v790NonCriticalExtensions);
      rel7_ue_capablity_v790ext = &ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
           v770NonCriticalExtensions.v790NonCriticalExtensions.ue_RadioAccessCapability_v790ext;
      RRC_MSG_COMMON_SET_BITMASK_IE_PTR(rel7_ue_capablity_v790ext,rrc_UE_RadioAccessCapability_v790ext_IEs,supportForEDPCCHPowerBoosting);
      rel7_ue_capablity_v790ext->supportForEDPCCHPowerBoosting =rrc_UE_RadioAccessCapability_v790ext_IEs_supportForEDPCCHPowerBoosting_true;
    }
#endif/*FEATURE_WCDMA_EDPCCH_POWER_BOOSTING*/

}

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
/*===========================================================================
FUNCTION rrcueci_check_fe_fach_concurrency

DESCRIPTION
  This function checks for concurrency NVs before showing FE-FACH capability 
  in connectionsetupcomplete message

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean rrcueci_check_fe_fach_concurrency()
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(rrc_fe_fach_support_nv == FE_FACH_NOT_SUPPORTED)
  {
    return FALSE;
  }
#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_FE_FACH(nv_value))
  {
    WRRC_MSG1_HIGH("FE_FACH : FE_FACH NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif

return TRUE;
}
#endif
/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_rel8_capability

DESCRIPTION
  This function appends the Radio Access Capability to the message for
  Release 8.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  rrcueci_append_ue_radio_access_capability_rel8_capability
(
  rrc_UE_CapabilityContainer_IEs * ue_capability_container
)
{
  rrc_UE_RadioAccessCapability_v860ext_IEs * v860_radioaccesscapability_ptr  = NULL;

  if(ue_capability_container == NULL)
  {
    WRRC_MSG0_ERROR("REL8:Cap Container is NULL!");
    return;
  }

  v860_radioaccesscapability_ptr = &ue_capability_container->v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
    v770NonCriticalExtensions.v790NonCriticalExtensions.v860NonCriticalExtensions.ue_RadioAccessCapability_v860ext;
  
  RRC_MSG_COMMON_SET_BITMASK_IE(v860_radioaccesscapability_ptr->physicalChannelCapability,
    rrc_PhysicalChannelCapability_v860ext,fddPhysChCapability);
  RRC_RESET_MSG_IE_PRESENT(v860_radioaccesscapability_ptr->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability);
  
#ifdef FEATURE_WCDMA_DC_HSDPA
  if((rrcueci_nv_supports_dc_hsdpa() == TRUE) && (!rrccsp_check_if_em_plmn()))
  {
    RRC_MSG_COMMON_SET_BITMASK_IE(v860_radioaccesscapability_ptr->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability,
      rrc_DL_PhysChCapabilityFDD_v860ext,hsdsch_physical_layer_category_ext2);
      v860_radioaccesscapability_ptr->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.
      hsdsch_physical_layer_category_ext2 = MIN(nv_hsdpa_category,RRC_DEFAULT_DC_HSDPA_CATEGORY);
  }
  else
  {
    WRRC_MSG0_HIGH("REL8:HSDPA Category isn't set from 21 to 24.");
  }
#endif /*FEATURE_WCDMA_DC_HSDPA*/

#ifdef FEATURE_WCDMA_TO_LTE
  RRC_RESET_MSG_IE_PRESENT(v860_radioaccesscapability_ptr->multiModeRAT_Capability);
  if(rrcmcm_is_lte_mode_enabled())
  {
    byte fgi=0;
    if(lte_rrc_lte_fdd_is_supported(rrccsp_get_lte_band_pref()))
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(v860_radioaccesscapability_ptr->multiModeRAT_Capability,supportOfEUTRAFDD);
      v860_radioaccesscapability_ptr->multiModeRAT_Capability.supportOfEUTRAFDD = rrc_MultiModeRAT_Capability_v860ext_supportOfEUTRAFDD_doesSupportEUTRAFDD;

#ifdef FEATURE_WTOL_PS_HANDOVER
      if(wcdma_rrc_wtol_ps_ho_support_nv) 
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(v860_radioaccesscapability_ptr->multiModeRAT_Capability,supportOfInterRATHOToEUTRAFDD);
        v860_radioaccesscapability_ptr->multiModeRAT_Capability.supportOfInterRATHOToEUTRAFDD = 
                 rrc_MultiModeRAT_Capability_v860ext_supportOfInterRATHOToEUTRAFDD_doesSupportInterRATHOToEUTRAFDD;
      }
#endif

    }
    /* We will say TDD supported if LTE says so. */
    if(lte_rrc_lte_tdd_is_supported(rrccsp_get_lte_band_pref()))
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(v860_radioaccesscapability_ptr->multiModeRAT_Capability,supportOfEUTRATDD);
      v860_radioaccesscapability_ptr->multiModeRAT_Capability.supportOfEUTRATDD = rrc_MultiModeRAT_Capability_v860ext_supportOfEUTRATDD_doesSupportEUTRATDD;

#ifdef FEATURE_WTOL_PS_HANDOVER
      if(wcdma_rrc_wtol_tdd_ps_ho_support_nv) 
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(v860_radioaccesscapability_ptr->multiModeRAT_Capability,supportOfInterRATHOToEUTRATDD);
        v860_radioaccesscapability_ptr->multiModeRAT_Capability.supportOfInterRATHOToEUTRATDD = 
                 rrc_MultiModeRAT_Capability_v860ext_supportOfInterRATHOToEUTRATDD_doesSupportInterRATHOToEUTRATDD;
      }
#endif

    }

    v860_radioaccesscapability_ptr->multiModeRAT_Capability.m.eutraFeatureGroupIndicatorsPresent = 1;
    v860_radioaccesscapability_ptr->multiModeRAT_Capability.eutraFeatureGroupIndicators.numbits = 4;
    fgi |= 0x08;

    if(rrc_wtol_cm_support_nv == TRUE)
    {
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH 
      if(lte_rrc_lte_fdd_is_supported(rrccsp_get_lte_band_pref()))
      {
        fgi |= 0x04; /*Support for EUTRA FDD measurements and reporting in connected mode*/
      }
#endif
#ifdef FEATURE_WTOL_TDD_CM_SUPPORT 
      if(lte_rrc_lte_tdd_is_supported(rrccsp_get_lte_band_pref()))
      {
        fgi |= 0x04; /*Support for EUTRA TDD measurements and reporting in connected mode*/
      }
#endif
    }
    #ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
    if(rrcueci_check_fe_fach_concurrency() == TRUE)
    {
      if(rrc_fe_fach_support_nv == FE_FACH_HIGH_PRIORITY_LAYERS)
      {
        fgi |= 0x02;
      }
      else if(rrc_fe_fach_support_nv == FE_FACH_ALL_LAYERS )
      { 
        fgi |= 0x01;
      }
      
    }
    #endif

    b_packb(fgi,(byte *)v860_radioaccesscapability_ptr->multiModeRAT_Capability.eutraFeatureGroupIndicators.data,RRC_DEFAULT_BIT_POS,4);
    if(rrcueci_append_ue_radio_access_capability_fdd3_list
        (&v860_radioaccesscapability_ptr->ue_RadioAccessCapabBandFDDList3,rrccsp_get_lte_band_pref()))
    {
      v860_radioaccesscapability_ptr->m.ue_RadioAccessCapabBandFDDList3Present = 1;
    }
  }
#endif

#ifdef  FEATURE_WCDMA_HS_FACH_DRX
  if(rrcenhstate_nv_supports_e_fach_drx())
  {
    v860_radioaccesscapability_ptr->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.m.supportOfHsdschDrxOperationPresent = 1;
    v860_radioaccesscapability_ptr->physicalChannelCapability.fddPhysChCapability.downlinkPhysChCapability.supportOfHsdschDrxOperation = 
      rrc_DL_PhysChCapabilityFDD_v860ext_supportOfHsdschDrxOperation_true;
  }
#endif
#ifdef FEATURE_MAC_I
  if(rrcueci_nv_supports_mac_i() == TRUE)
  {

  /*Add the Mac-i support*/
    v860_radioaccesscapability_ptr->m.supportOfMACiisPresent = 1;
    v860_radioaccesscapability_ptr->supportOfMACiis = rrc_UE_RadioAccessCapability_v860ext_IEs_supportOfMACiis_true;
  }
#endif /* FEATURE_MAC_I */
#ifdef FEATURE_WCDMA_HS_RACH
    if (TRUE == rrchsrach_nv_supports_hs_rach())
    {

    /*Add the Mac-i support*/
      v860_radioaccesscapability_ptr->m.supportOfCommonEDCHPresent = 1;
      v860_radioaccesscapability_ptr->supportOfCommonEDCH = rrc_UE_RadioAccessCapability_v860ext_IEs_supportOfCommonEDCH_true;
    }
#endif /* FEATURE_WCDMA_HS_RACH */

#ifdef FEATURE_FEMTO_CSG
    if (is_whitelist_valid == TRUE) 
    {
      v860_radioaccesscapability_ptr->m.supportOfCSGPresent = 1;
      v860_radioaccesscapability_ptr->supportOfCSG = rrc_UE_RadioAccessCapability_v860ext_IEs_supportOfCSG_true;
    }
#endif

  return;
}

#ifdef FEATURE_MAC_I

/*====================================================================
FUNCTION: rrcueci_nv_supports_mac_i()

DESCRIPTION:
This function checks whether the NV supports MAC_I or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_mac_i
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(!RRC_IS_MAC_I_NV_TURNED_ON)
  {
    WRRC_MSG0_MED("MAC_I NV disabled");
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_MAC_I(nv_value))
  {
    WRRC_MSG1_MED("MAC_I : MAC_I NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif

  WRRC_MSG0_MED("MAC_I NV enabled");
  return TRUE;
}

#endif /* FEATURE_MAC_I */


/*====================================================================
FUNCTION: rrcueci_nv_supports_cpc_dtx_on_dpcch()

DESCRIPTION:
This function checks whether the NV supports cpc_dtx_on_dpcch and slot format 4

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_cpc_dtx_on_dpcch
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(rrc_disable_cpc_fdpch_nv & (NV_CPC_DTX_DISABLE|NV_FDPCH_DISABLE))
  {
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_CPC(nv_value))
  {
    WRRC_MSG1_MED("CPC : CPC NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif

  return TRUE;
}


#ifdef FEATURE_WCDMA_DC_HSDPA

/*====================================================================
FUNCTION: rrcueci_nv_supports_dc_hsdpa()

DESCRIPTION:
This function checks whether the NV supports DC-HSDPA

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_dc_hsdpa
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(!CHECK_WCDMA_OPTIONAL_FEATURE_DC_HSDPA_SUPPORTED)
  {
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_DCHSDPA(nv_value))
  {
    WRRC_MSG1_MED("DCHSDPA : DCHSDPA NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif

  return TRUE;
}

#endif /*FEATURE_WCDMA_DC_HSDPA*/

#ifdef FEATURE_WCDMA_DC_HSUPA

/*====================================================================
FUNCTION: rrcueci_nv_supports_dc_hsupa()

DESCRIPTION:
This function checks whether the NV supports DC-HSUPA

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_dc_hsupa
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(!(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
  {
    return FALSE;
  }
  if(!CHECK_WCDMA_OPTIONAL_FEATURE_DC_HSUPA_SUPPORTED)
  {
    return FALSE;
  }

  if(rrcueci_nv_supports_mac_i() == FALSE)
  {
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_DCHSUPA(nv_value))
  {
    WRRC_MSG1_MED("DCHSUPA : DCHSUPA NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif
  return TRUE;
}

#endif /*FEATURE_WCDMA_DC_HSUPA*/

#ifdef FEATURE_WCDMA_3C_HSDPA

/*====================================================================
FUNCTION: rrcueci_nv_supports_3c_hsdpa()

DESCRIPTION:
This function checks whether the NV supports 3C-HSDPA

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_3c_hsdpa
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(!(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL10))
  {
    return FALSE;
  }

  if(!CHECK_WCDMA_OPTIONAL_FEATURE_3C_HSDPA_SUPPORTED)
  {
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_3CHSDPA(nv_value))
  {
    WRRC_MSG1_MED("3CHSDPA : 3CHSDPA NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif
  return TRUE;
}

#endif /*FEATURE_WCDMA_3C_HSDPA*/

#ifdef FEATURE_WCDMA_DB_DC_HSDPA

/*====================================================================
FUNCTION: rrcueci_nv_supports_db_dc_hsdpa()

DESCRIPTION:
This function checks whether the NV supports Dc HSUPA or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_db_dc_hsdpa
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(!((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9) && 
       (rrc_feature_nv_item_value & RRC_DB_DC_HSDPA_ENABLED)))
  {
    WRRC_MSG1_MED("DB_DC_HSDPA NV disabled DB DC HSDPA NV %d ",
      ((rrc_feature_nv_item_value & RRC_DB_DC_HSDPA_ENABLED)?1:0));
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_DBDC_HSDPA(nv_value))
  {
    WRRC_MSG1_MED("DBDC_HSDPA : DBDC_HSDPA NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif
  WRRC_MSG0_MED("DB_DC_HSDPA NV enabled");
  return TRUE;
}

#endif
#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
/*====================================================================
FUNCTION: rrcueci_nv_supports_edpcch_pwr_boost()
DESCRIPTION:
This function checks whether the NV supports EDPCCH_PWR_BOOST or not
DEPENDENCIES:
  None.
RETURN VALUE:
  True/False.
SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcueci_nv_supports_edpcch_pwr_boost
(
  void
)
{
   if (rrc_feature_nv_item_value & RRC_EDPCCH_PWR_BOOST_ENABLED)
   {
      return TRUE;
   }
    return FALSE;
}
#endif /* FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST */


/*===========================================================================

FUNCTION rrcueci_get_wtog_nacc_nv_item

DESCRIPTION
  This  function reads the NVs NV_GERAN_FEATURE_PACK_1_I and NV_INTERRAT_NACC_SUPPORT_I
  and based on these NV items updates global with WTOG NACC enabled status

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcueci_get_wtog_nacc_nv_item
(
  sys_modem_as_id_e_type wrrc_as_id
)
{
  boolean wtog_nacc_enabled = WNV_GET_DEFAULT(WNV_ID_INTERRAT_NACC_SUPPORT_I);
  boolean geran_pack_1_enabled = WNV_GET_DEFAULT(WNV_ID_GERAN_FEATURE_PACK_1_I);
  uint8 nacc_support;
  if ( WNV_SUCCESS ==  (wnv_get_nv_msim( WNV_ID_GERAN_FEATURE_PACK_1_I, 
                                        &geran_pack_1_enabled,
                                        sizeof(geran_pack_1_enabled),wrrc_as_id)))
  {
    if ( (geran_pack_1_enabled) && (WNV_SUCCESS ==  (wnv_get_nv_msim( WNV_ID_INTERRAT_NACC_SUPPORT_I, 
                                                                     &nacc_support,
                                                                     sizeof(nacc_support),wrrc_as_id))))
    {
      if((nacc_support & RRC_WTOG_NACC_SUPPORTED) != RRC_WTOG_NACC_SUPPORTED)
      {
        wtog_nacc_enabled = FALSE;
      }
    }
  }
  nv_wtog_nacc_enabled = wtog_nacc_enabled;
  WRRC_MSG1_HIGH("WTOG NACC NV Support %d",nv_wtog_nacc_enabled);
}


#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION rrcueci_send_eutra_capabilitites_req_to_lte

DESCRIPTION
  This  function sends EUTRA capabilities request to LTE

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcueci_send_eutra_capabilitites_req_to_lte
(
  void
)
{
  lte_rrc_eutra_capabilities_req_s eutra_cap_req;
  static uint8 count = 0;
  memset(&eutra_cap_req,0,sizeof(lte_rrc_eutra_capabilities_req_s));

  if ((!rrcueci_interrat_lte_capability_info.lte_rsp_received) && rrcueci_interrat_lte_capability_info.lte_req_posted) 
  {
    count++;
    if (count >= 3)
    {
      ERR_FATAL("WTOL: Didnt receive capability response from LTE for %d time",count,0,0);
    }
    else
    {
      WRRC_MSG1_ERROR("WTOL: Didnt receive capability response from LTE for %d time ",count);
    }
  }
  else
  {
    count=0;
  }

  eutra_cap_req.rat_capabilities.lte_is_supported = TRUE;
  eutra_cap_req.rat_capabilities.lte_band_preference = rrccsp_get_lte_band_pref();


  rrccsp_get_wg_band_pref(&eutra_cap_req.rat_capabilities.wcdma_band_preference,
    &eutra_cap_req.rat_capabilities.geran_band_preference);
  eutra_cap_req.rat_capabilities.wcdma_is_supported = TRUE;
  if(rrcmcm_is_dualmode_enabled_no_f3())
  {
    eutra_cap_req.rat_capabilities.geran_is_supported = TRUE;
  }

  eutra_cap_req.src_rat = LTE_RRC_IRAT_UTRA_FDD;

  /*LTE_RRC_EUTRA_CAPABILITIES_REQ*/
  rrc_send_lte_cmd(&eutra_cap_req.msg_hdr,LTE_RRC_EUTRA_CAPABILITIES_REQ,sizeof(lte_rrc_eutra_capabilities_req_s));
  WRRC_MSG0_HIGH("WTOL: LTE_RRC_EUTRA_CAPABILITIES_REQ sent to LTE RRC");

  rrcueci_interrat_lte_capability_info.lte_req_posted = TRUE;
  rrcueci_interrat_lte_capability_info.lte_rsp_received = FALSE;

  /* Freeing the capability contianer received from LTE */
  if(rrcueci_interrat_lte_capability_info.cap_cont_ptr)
  {
    rrc_free(rrcueci_interrat_lte_capability_info.cap_cont_ptr);
    rrcueci_interrat_lte_capability_info.cap_cont_ptr = NULL; 
  }
}

/*===========================================================================

FUNCTION rrcueci_save_eutra_capability

DESCRIPTION
  This  function saves the E-UTRA capability info

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcueci_save_eutra_capability
(
  lte_rrc_eutra_capabilities_rsp_s *lte_eutra_capabilities_rsp
)
{
  dsm_item_type  *dsm_ptr =NULL;
  msgr_attach_struct_type   *att_ptr = NULL;
  uint32  dsm_len = 0;


  att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&lte_eutra_capabilities_rsp->msg_hdr, 0);
  ASSERT(att_ptr != NULL);
  msgr_get_dsm_attach(att_ptr, &dsm_ptr);
  ASSERT(dsm_ptr != NULL);

  dsm_len = dsm_length_packet(dsm_ptr);
  WRRC_MSG1_HIGH("LTE container capability length %d",dsm_len);

  ASSERT(dsm_len != 0);

  rrcueci_interrat_lte_capability_info.cap_cont_length = dsm_len;

  rrcueci_interrat_lte_capability_info.cap_cont_ptr = (byte *)rrc_malloc(rrcueci_interrat_lte_capability_info.cap_cont_length);

  ASSERT(dsm_len == dsm_pullup(&dsm_ptr, rrcueci_interrat_lte_capability_info.cap_cont_ptr, (uint16)dsm_len) );

  rrcueci_interrat_lte_capability_info.lte_rsp_received = TRUE;
  rrcueci_interrat_lte_capability_info.lte_camped_band_mask = lte_eutra_capabilities_rsp->lte_camped_band_mask;
  rrcueci_interrat_lte_capability_info.lte_nghbr_band_mask = lte_eutra_capabilities_rsp->lte_neigh_band_mask;
}

/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_fdd3_list

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcueci_append_ue_radio_access_capability_fdd3_list
(
  rrc_UE_RadioAccessCapabBandFDDList3 *fddlist3,
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type lte_band_pref
#else
  sys_band_mask_e_type lte_band_pref
#endif
)
{
  uint8 lte_band_idx=0, comp_mode_idx=0, fdd_list3_index = 0;
  rrcueci_band_storage_type ueci_band = {0};  
  rrc_UE_RadioAccessCapabBandFDDList3 *temp_fdd_list3_ptr = fddlist3;
  uint8 num_lte_bands = 0;
  rrc_CompressedModeMeasCapabEUTRAList *temp_eutra_capab_ptr = NULL;
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type lte_fdd_band_pref;
  sys_lte_band_mask_e_type lte_tdd_band_pref;
  memset(&lte_fdd_band_pref,0,sizeof(sys_lte_band_mask_e_type));
  memset(&lte_tdd_band_pref,0,sizeof(sys_lte_band_mask_e_type));
#else
  sys_band_mask_e_type lte_fdd_band_pref=0;
  sys_band_mask_e_type lte_tdd_band_pref=0;
#endif
#ifdef LTE_BAND_NUM
  if(rrccsp_check_if_lte_band_pref_is_set(lte_band_pref) == FALSE)
#else
  if(lte_band_pref == 0)
#endif
  {
    WRRC_MSG0_ERROR("No LTE bands! ");

    return FALSE;
  }

  rrc_fill_lte_band_mask(&num_lte_bands, &lte_band_pref, &lte_fdd_band_pref, &lte_tdd_band_pref);

  if (num_lte_bands == 0)
  {
    WRRC_MSG0_ERROR("No good LTE bands ");
    return FALSE;
  }
#ifdef LTE_BAND_NUM
  #if (LTE_BAND_NUM == 256)
  WRRC_MSG5_HIGH("WTOL: Number of LTE bands %d & LTE band mask = 0x%x  0x%x  0x%x 0x%x", num_lte_bands,
                  lte_band_pref.bits_1_64,lte_band_pref.bits_65_128,lte_band_pref.bits_129_192,lte_band_pref.bits_193_256);
  #else
  WRRC_MSG2_HIGH("WTOL: Number of LTE bands %d & LTE band mask = 0x%x", num_lte_bands, lte_band_pref);
  #endif
#else
  WRRC_MSG2_HIGH("WTOL: Number of LTE bands %d & LTE band mask = 0x%x", num_lte_bands, lte_band_pref);
#endif
  if (num_lte_bands > RRC_UECI_MAX_BANDS_ALLOWED_FDD2) 
  {
     num_lte_bands = RRC_UECI_MAX_BANDS_ALLOWED_FDD2;
  }
  rrccsp_get_supported_band_list(&ueci_band);
  WRRC_MSG2_HIGH("WTOL: No of bands supported- fdd :%d, fdd2: %d", ueci_band.num_supported_bands, ueci_band.num_supported_bands_fdd2);

  if(0 == (ueci_band.num_supported_bands + ueci_band.num_supported_bands_fdd2))
  {
    ERR_FATAL("WTOL: No W bands enabled",0,0,0);
  }
 
  temp_fdd_list3_ptr->elem = NULL;
  
  temp_fdd_list3_ptr->n = 1;
    ALLOC_ASN1ARRAY2(&enc_ctxt,temp_fdd_list3_ptr,rrc_UE_RadioAccessCapabBandFDD3);    


    comp_mode_idx = 0;

    RRC_RESET_MSG_IE_PRESENT(temp_fdd_list3_ptr->elem[fdd_list3_index]);

    temp_fdd_list3_ptr->elem[fdd_list3_index].measurementCapability3.m.compressedModeMeasCapabEUTRAListPresent = 1;

    temp_fdd_list3_ptr->elem[fdd_list3_index].measurementCapability3.compressedModeMeasCapabEUTRAList.n = num_lte_bands;
    temp_eutra_capab_ptr = &temp_fdd_list3_ptr->elem[fdd_list3_index].measurementCapability3.compressedModeMeasCapabEUTRAList;    
    ALLOC_ASN1ARRAY2(&enc_ctxt,temp_eutra_capab_ptr,rrc_CompressedModeMeasCapabEUTRA);

    for(lte_band_idx =0 ; ((lte_band_idx < SYS_SBAND_LTE_EUTRAN_BAND_MAX)&&(comp_mode_idx < RRC_UECI_MAX_BANDS_ALLOWED_FDD2));lte_band_idx++)
    {
      if( lte_band_idx < RRC_MAX_LTE_BAND_SUPPORT)
      {
#ifdef LTE_BAND_NUM
        if(SYS_LTE_BAND_MASK_IS_BAND_PRESENT(&lte_band_pref,lte_band_idx))
#else
        if((((uint64)1) << lte_band_idx) & lte_band_pref)
#endif
        {
          temp_eutra_capab_ptr->elem[comp_mode_idx].radioFrequencyBandEUTRA = lte_band_idx+1;
          temp_eutra_capab_ptr->elem[comp_mode_idx].compressedMode = FALSE;

#ifndef FEATURE_WTOL_TDD_CM_SUPPORT
#ifdef LTE_BAND_NUM
          if(SYS_LTE_BAND_MASK_IS_BAND_PRESENT(&lte_fdd_band_pref,lte_band_idx))
#else
          if((((uint64)1) << lte_band_idx) & lte_fdd_band_pref)
#endif
#endif
          {
            /* We support W2L CM only for FDD*/
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
            if(rrc_wtol_cm_support_nv == TRUE)
            {
              /*Check if tdd is supported and tdd psho nv is set OR fdd is supported */
#ifdef LTE_BAND_NUM
              if((SYS_LTE_BAND_MASK_IS_BAND_PRESENT(&lte_tdd_band_pref,lte_band_idx))
                 || SYS_LTE_BAND_MASK_IS_BAND_PRESENT(&lte_fdd_band_pref,lte_band_idx))
#else
              if((((((uint64)1) << lte_band_idx) & lte_tdd_band_pref)) || ((((uint64)1) << lte_band_idx) & lte_fdd_band_pref))
#endif
                 temp_eutra_capab_ptr->elem[comp_mode_idx].compressedMode = TRUE;
              else
                 temp_eutra_capab_ptr->elem[comp_mode_idx].compressedMode = FALSE;
            }
#endif
          }
          comp_mode_idx++;
        }
      }
    }
  return TRUE;

}
/*===========================================================================
FUNCTION rrcueci_append_ue_radio_access_capability_fdd3_list

DESCRIPTION
  This function appends the Radio Access Capability to the message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcueci_append_ue_radio_access_capability_fdd4_list
(
  rrc_UE_RadioAccessCapabBandFDDList4 *fddlist4
)
{
  uint8 idx;
  rrcueci_band_storage_type ueci_band = {0};  
  rrc_UE_RadioAccessCapabBandFDDList4 *temp_fdd_list4_ptr = fddlist4;
  
  
  rrccsp_get_supported_band_list(&ueci_band);

  if(0 == ueci_band.num_supported_bands)
  {
    ERR_FATAL("NO FDD bands supported",0,0,0);
  }
 
  temp_fdd_list4_ptr->n = 0; 
  temp_fdd_list4_ptr->elem = NULL;
  for(idx=0;(idx < ueci_band.num_supported_bands) && (idx < RRC_UECI_MAX_BANDS_ALLOWED);idx++)
  {
    if(rrc_RadioFrequencyBandFDD_fdd2100  == ueci_band.band_type[idx])
    {
      temp_fdd_list4_ptr->n = 1;
      ALLOC_ASN1ARRAY2(&enc_ctxt,temp_fdd_list4_ptr,rrc_UE_RadioAccessCapabBandFDD4);    
      /*Ansah: Do we need to fill radioFrequencyBandFDD? If yes when do we need to use FDD2 and FDD3*/
      temp_fdd_list4_ptr->elem[0].m.radioFrequencyBandFDDPresent = 1;
      temp_fdd_list4_ptr->elem[0].radioFrequencyBandFDD = rrc_RadioFrequencyBandFDD_fdd2100;
      temp_fdd_list4_ptr->elem[0].m.additionalSecondaryCellsPresent = 1;
      temp_fdd_list4_ptr->elem[0].additionalSecondaryCells = rrc_UE_RadioAccessCapabBandFDD4_additionalSecondaryCells_a1;
      break;
    }
  }
}

/*===========================================================================
FUNCTION rrcueci_get_static_band_mask

DESCRIPTION
  This function returns the pre-defined band masks based on the current camped 
  UMTS band. The below table gives the pre-defined supported bands.
DEPENDENCIES
  None

RETURN VALUE
  sys_band_mask_type

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

Multi RAT Band Combinations  LTE<----->UMTS
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UMTS Band--->|UMTS Band I | UMTS Band II | UMTS Band III  |UMTS Band IV | UMTS Band V | UMTS Band VI | UMTS Band VIII| UMTS Band IX | UMTS Band XI | UMTS Band XIII | UMTS Band XVII | UMTS Band XIX |
LTE Band
|
V                                                                                                                      
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 01     |      Yes       |       Yes        |       Yes           |      Yes         |        Yes       |        Yes        |         Yes        |        Yes        |       Yes         |                      |                        |          Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 02     |                 |       Yes        |                       |      Yes         |                   |                     |                      |                     |                     |                      |                        |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 03     |      Yes       |       Yes        |       Yes           |      Yes         |		 Yes	 |        Yes        |		Yes         |		Yes	        |		Yes	        |                      |                        |        Yes                              
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 04     |                 |       Yes        |                       |      Yes          |        Yes       |                     |                      |                     |                    |                      |                        |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 05     |       Yes      |       Yes        |                       |      Yes         |        Yes       |                     |                      |                     |                    |                      |                        |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 07     |       Yes      |       Yes        |        Yes          |       Yes        |        Yes       |         Yes       |         Yes        |        Yes         |         Yes      |                      |                        |        Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 08     |       Yes      |                    |                       |                   |                    |                     |         Yes        |                      |                    |                      |                        |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 11     |       Yes      |                    |                       |                   |                    |        Yes        |                      |        Yes         |         Yes      |                      |                        |        Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 12     |                  |       Yes        |                       |      Yes        |                    |                     |                      |                      |                    |                      |                        |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 13     |       Yes      |       Yes        |                       |       Yes       |                    |                     |                      |                      |                    |                      |                        |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 14     |                  |       Yes        |                       |       Yes       |                    |                     |                      |                      |                    |                      |                        |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 17     |                  |       Yes        |                       |       Yes       |         Yes      |                     |                       |                      |                   |                       |                       |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 18     |       Yes      |                    |                       |                   |                    |       Yes         |                       |        Yes         |          Yes    |                       |                       |        Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 19     |       Yes      |                    |                       |                   |                    |        Yes        |                       |        Yes         |           Yes   |                       |                       |        Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 20     |       Yes      |                    |         Yes         |                   |                    |                     |        Yes          |                      |                   |                       |                       |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 21     |       Yes      |                    |                       |                   |                    |        Yes        |                       |        Yes         |         Yes     |                       |                       |         Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 25     |                  |      Yes         |                       |        Yes      |                    |                     |                       |                     |                    |                       |                       |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 26     |                  |                    |                       |                   |                    |                     |                      |                      |                    |                       |                       |        Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
FDD Band 28     |      Yes       |       Yes        |          Yes        |        Yes       |        Yes       |       Yes         |         Yes         |                     |                    |                        |                       |        Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
TDD Band 38     |      Yes       |       Yes        |          Yes        |        Yes       |        Yes       |       Yes         |         Yes         |         Yes       |        Yes        |                       |                       |        Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
TDD Band 39     |                  |                    |                       |                   |                    |                     |                       |                     |                    |                       |                        |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
TDD Band 40     |      Yes       |       Yes        |          Yes        |        Yes       |        Yes       |       Yes         |         Yes         |         Yes       |          Yes      |                       |                        |       Yes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
TDD Band 41     |      Yes       |       Yes        |                       |        Yes       |        Yes       |       Yes         |                       |        Yes        |        Yes        |                       |                        |       Yes 
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
===========================================================================*/
sys_band_mask_type rrcueci_get_static_band_mask(void)
{
  sys_band_mask_type static_table_bmask = 0;

  if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_IMT2K_BAND)
  {
    static_table_bmask = 0x1A0081E14D5ULL;
  }
  else if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_PCS_BAND)
  {
    static_table_bmask = 0x1A00901385FULL;
  }
  else if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_WCDMA_1800_BAND)
  {
    static_table_bmask = 0xA008080045ULL;
  }
  else if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_WCDMA_1700_2100_BAND)
  {
    static_table_bmask = 0x1A00901385FULL;
  }
  else if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_WCDMA_850_BAND)
  {
    static_table_bmask = 0x1A00801005DULL;
  }
  else if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_WCDMA_800_BAND)
  {
    static_table_bmask = 0x1A008160445ULL;
  }
  else if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_WCDMA_900_BAND)
  {
    static_table_bmask = 0xA0080800C5ULL;
  }
  else if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_WCDMA_1700_BAND)
  {
    static_table_bmask = 0x1A000160055ULL;
  }
  else if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_WCDMA_1500_BAND)
  {
    static_table_bmask = 0x1A000160445ULL;
  }
  else if(rrc_csp_int_data.curr_acq_entry.band_class  == RRC_CSP_WCDMA_BC19_BAND)
  {
    static_table_bmask = 0x1A008140445ULL;
  }

  return  static_table_bmask;
}

/*===========================================================================
FUNCTION rrc_fill_priority_bitmask_array

DESCRIPTION
  This function prepares the band bitmask which will be used for pruning.
  the bands

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrc_fill_priority_bitmask_array(sys_band_mask_type *ueci_priority_bmask)
{
  sys_band_mask_type static_band_bitmask = rrcueci_get_static_band_mask();

  /* SIB 19 band bitmask */
#ifdef LTE_BAND_NUM
  ueci_priority_bmask[2] = sys_map_lte_band_mask_to_band_mask(rrcueci_sib19_band_mask);
#else
  ueci_priority_bmask[2] = rrcueci_sib19_band_mask;
#endif

  /* SIB19 + static table band bitmask */
  ueci_priority_bmask[1] = (static_band_bitmask|ueci_priority_bmask[2]);

  /* SIB19 + LTE acq db + static table band bitmask */  
#ifdef LTE_BAND_NUM
  ueci_priority_bmask[0] = (sys_map_lte_band_mask_to_band_mask(rrcueci_interrat_lte_capability_info.lte_camped_band_mask)| ueci_priority_bmask[1] );
#else
  ueci_priority_bmask[0] = (rrcueci_interrat_lte_capability_info.lte_camped_band_mask | ueci_priority_bmask[1] );
#endif
}

/*===========================================================================
FUNCTION rrc_fill_lte_band_mask

DESCRIPTION
  This function prepares the LTE band bitmask to be used for filling the
  UEC. If the number of LTE bands exceed 16 then bands will be pruned 
  using prioritoization logic

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef LTE_BAND_NUM
void rrc_fill_lte_band_mask(uint8* num_lte_bands, sys_lte_band_mask_e_type *lte_band_pref, sys_lte_band_mask_e_type *lte_fdd_band_pref, sys_lte_band_mask_e_type *lte_tdd_band_pref)
{
  uint8 lte_band_idx;
  uint8 idx = 0;
  uint8 num_lte_band_pref = 0;

  uint8 num_bands_to_be_reduced = 0;
  sys_lte_band_mask_e_type tmp_lte_band_pref;
  sys_band_mask_type ueci_priority_bmask[MAX_BITMASK_TO_BE_PRIORITIZED];

  memset(ueci_priority_bmask,0,sizeof(sys_band_mask_type)*(MAX_BITMASK_TO_BE_PRIORITIZED));  
  memset(&tmp_lte_band_pref,0,sizeof(sys_lte_band_mask_e_type));
  for(lte_band_idx =0 ; lte_band_idx < SYS_SBAND_LTE_EUTRAN_BAND_MAX;lte_band_idx++)
  {
    if(lte_band_idx < RRC_MAX_LTE_BAND_SUPPORT)
    {
      if(SYS_LTE_BAND_MASK_IS_BAND_PRESENT(lte_band_pref,lte_band_idx))
      {
        SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&tmp_lte_band_pref);
        SYS_LTE_BAND_MASK_ADD_BAND(&tmp_lte_band_pref,lte_band_idx);
        /* If we dont support this band, then we need to ignore them*/
        if(lte_rrc_lte_tdd_is_supported(tmp_lte_band_pref))
        {
          num_lte_band_pref++;
          SYS_LTE_BAND_MASK_ADD_BAND(lte_tdd_band_pref,lte_band_idx);
        }
        else if(lte_rrc_lte_fdd_is_supported(tmp_lte_band_pref))
        {
          num_lte_band_pref++;
          SYS_LTE_BAND_MASK_ADD_BAND(lte_fdd_band_pref,lte_band_idx);
        }
        else
        {
          MSG_MED("Ignore TDD",0,0,0);
          /* Pruning the LTE bands that are not supported */
          SYS_LTE_BAND_MASK_REMOVE_BAND(lte_band_pref,lte_band_idx);
        }
        memset(&tmp_lte_band_pref,0,sizeof(sys_lte_band_mask_e_type));
      }
    }
    else
    {
       /*We do not support more than 64 LTE bands*/
    }
   }


  /* Number of LTE bands supported as per LTE band preference */
  *num_lte_bands = num_lte_band_pref;

  if(num_lte_band_pref <= RRC_UECI_MAX_BANDS_ALLOWED_FDD2)
  {
    return;
  }

  /* Prepare the band bitmask based on prioritization logic */
  rrc_fill_priority_bitmask_array(ueci_priority_bmask);

  num_bands_to_be_reduced = num_lte_band_pref - RRC_UECI_MAX_BANDS_ALLOWED_FDD2;  //Caluculating the number of bands to be reduced
  
  for(idx = 0; idx < (MAX_BITMASK_TO_BE_PRIORITIZED) && (num_bands_to_be_reduced > 0); idx++)
  {
    for(lte_band_idx =0 ; (lte_band_idx < SYS_SBAND_LTE_EUTRAN_BAND_MAX) && ((num_bands_to_be_reduced > 0));lte_band_idx++)
    {
      if(lte_band_idx < RRC_MAX_LTE_BAND_SUPPORT)
      {
        /* Band bitmasks have already been prepared */
        if((GET_64_BMASK(lte_band_idx)) &
           sys_map_lte_band_mask_to_band_mask(*lte_band_pref) &
           ~( ueci_priority_bmask[idx]))
        {
          /* Resetting the unwanted band bitmask */
          SYS_LTE_BAND_MASK_REMOVE_BAND(lte_band_pref,lte_band_idx);
          num_bands_to_be_reduced --;
        }
      }
      else
      {
          /* We do not support more than 64 LTE bands*/
      }
    }
  }

  /* Final number of LTE bands supported subtracted after pruning logic */
  *num_lte_bands = num_lte_band_pref - num_bands_to_be_reduced;
  return;
  /* Fill the UECI IE with "lte_band_pref" */
  /* If still "to_be_reduced" is greater than 16 Ò means all the bands are present in either SIB19 or static table so we fill 16 and ignore the rest */
}
#else
void rrc_fill_lte_band_mask(uint8* num_lte_bands, sys_band_mask_e_type *lte_band_pref, sys_band_mask_e_type *lte_fdd_band_pref, sys_band_mask_e_type *lte_tdd_band_pref)
{
  uint8 lte_band_idx;
  uint8 idx = 0;
  uint8 num_lte_band_pref = 0;

  uint8 num_bands_to_be_reduced = 0;

  sys_band_mask_type ueci_priority_bmask[MAX_BITMASK_TO_BE_PRIORITIZED];

  memset(ueci_priority_bmask,0,sizeof(sys_band_mask_type)*(MAX_BITMASK_TO_BE_PRIORITIZED));  

  for(lte_band_idx =0 ; lte_band_idx < SYS_SBAND_LTE_EUTRAN_BAND_MAX;lte_band_idx++)
  {
    if((GET_64_BMASK(lte_band_idx)) & *lte_band_pref)
    {
      /* If we dont support this band, then we need to ignore them*/
      if(lte_rrc_lte_tdd_is_supported(GET_64_BMASK(lte_band_idx)))
      {
        num_lte_band_pref++;
        *lte_tdd_band_pref |= (GET_64_BMASK(lte_band_idx));
      }
      else if(lte_rrc_lte_fdd_is_supported(GET_64_BMASK(lte_band_idx)))
      {
        num_lte_band_pref++;
        *lte_fdd_band_pref |= (GET_64_BMASK(lte_band_idx));
      }
      else 
      {
        MSG_MED("Ignore TDD",0,0,0);
        /* Pruning the LTE bands that are not supported */
        *lte_band_pref &= ~(GET_64_BMASK(lte_band_idx));
      }
    }
  }

  /* Number of LTE bands supported as per LTE band preference */
  *num_lte_bands = num_lte_band_pref;

  if(num_lte_band_pref <= RRC_UECI_MAX_BANDS_ALLOWED_FDD2)
  {
    return;
  }

  /* Prepare the band bitmask based on prioritization logic */
  rrc_fill_priority_bitmask_array(ueci_priority_bmask);

  num_bands_to_be_reduced = num_lte_band_pref - RRC_UECI_MAX_BANDS_ALLOWED_FDD2;  //Caluculating the number of bands to be reduced
  
  for(idx = 0; idx < (MAX_BITMASK_TO_BE_PRIORITIZED) && (num_bands_to_be_reduced > 0); idx++)
  {
    for(lte_band_idx =0 ; (lte_band_idx < SYS_SBAND_LTE_EUTRAN_BAND_MAX) && ((num_bands_to_be_reduced > 0));lte_band_idx++)
    {
      /* Band bitmasks have already been prepared */
      if((GET_64_BMASK(lte_band_idx)) & *lte_band_pref & ~( ueci_priority_bmask[idx])) 
      {
        /* Resetting the unwanted band bitmask */
        *lte_band_pref &= ~ (GET_64_BMASK(lte_band_idx));
        num_bands_to_be_reduced --;
      }
    }  
  }

  /* Final number of LTE bands supported subtracted after pruning logic */
  *num_lte_bands = num_lte_band_pref - num_bands_to_be_reduced;
  return;
  /* Fill the UECI IE with "lte_band_pref" */
  /* If still "to_be_reduced" is greater than 16 Ò means all the bands are present in either SIB19 or static table so we fill 16 and ignore the rest */
}
#endif
#endif
/*===========================================================================

FUNCTION  rrcueci_get_current_substate

DESCRIPTION

  This function will return the current substate
    
DEPENDENCIES

  None.
 
RETURN VALUE
returns rrcueci substate in uint8


SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrcueci_get_current_substate(void)
{
  return ((uint8)rrcueci_substate);
}
/*===========================================================================

FUNCTION rrcueci_return_ueci_substate

DESCRIPTION
Returns current ueci substate

DEPENDENCIES
  None.

RETURN VALUE
  rrcueci_substate_e_type.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrcueci_return_ueci_substate(void)
{
  return (uint8)rrcueci_substate;
}

#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
FUNCTION rrcueci_get_cap_container_length

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 rrcueci_get_cap_container_length(void)
{
#ifdef FEATURE_WCDMA_TO_LTE
  return rrcueci_interrat_lte_capability_info.cap_cont_length;
#else
  return 0;
#endif
}
#endif
