#ifndef CAI_H
#define CAI_H
/*===========================================================================

       TIA/EIA-95-B   P R O T O C O L   D A T A    D E C L A R A T I O N S

DESCRIPTION

  This file contains the data structures for messages received and
  transmitted to the base station while the mobile station is operating
  in CDMA mode and other CDMA-related constants and indicators as
  defined in IS-95A.

Copyright (c) 1990-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/cai.h_v   1.108   02 Oct 2002 22:52:12   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/cai.h#1 $ $DateTime: 2016/12/13 07:57:47 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/10   ag      Moved retrievable parameters related APIs, which are
                   not accessed outside of 1XSU, to cai_v.h.
01/20/10   jj      Removed featurization.
12/23/09   jj      Moved few #defines for DTMF from cai_v.h to here.
10/15/09   ag      Defined CAI_SO_MARKOV_SO74.
09/29/09   ag      1X Advanced Signaling Support - Phase 2. Increased 
                   CAI_AC_ALT_SO_MAX to 10.
08/31/09   adw     Removed unnecessary customer.h and target.h includes.
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
08/02/09   ag      Changes for 1X Advanced Phase 2 signalling implementation
06/29/09   jj      Made changes to include a SCRM symbol.
06/26/09   ag      1X Adv : Defined CAI_NON_CRITICAL_RATE rate type
05/20/09   ag      Fixed compiler warnings
05/14/09   jj      CMI phase2 effort.
05/12/09   jj      Moved few #defines from cai_v.h to here.
03/24/09   jj      Added few more externally accessible parameters to this file.
04/01/09   adw     Added temporary CMI-lite featurization.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_CP.07.02.09
04/08/07   pg      Featurization and general code cleanup for Memory Reduction
                   Effort.
12/14/07   an      Added Passport Mode
12/02/07   bb	   Discarding BS ACK (order message) by MC task due to  incorrect paging message length
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
03/19/07   pg      Lint cleanup.
10/12/06   pg      Increased CAI_AC_ALT_SO_MAX from 7 to 9 under FEATURE_JCDMA_2.
09/25/06   fc      UMTS changes.
09/25/06   sb      Added support for Band Class 15.
09/08/06   fc      Updated comment.
07/10/06   fc      Added default srch handoff parameters.
06/15/06   an      Define the max range value of ORDQ when processing PPM order from BS
06/12/06   fc      Added service status order changes.
06/01/06   an      Added the bypass registration indicator changes
05/01/06   an      Fixed Lint Errors
04/26/06   an      Fixed Lint Errors
04/22/06   fc      Fixed Rel A featurization.
04/05/06   an      Changed the 4GV_WB SO value to 0x46.
03/01/06   fh      Added AES support
02/13/06   an      Added ECCLM Cross Band Hashing Support
01/19/06   fc      Merged the following :
           fh      Added AKA and MI support.
12/08/05   fc      Featurize gen_cmeakey under FEATURE_IS2000_REL_D.
11/03/05   an      Changed 4GV SO value, moved EVRC above 4GV_WB in 4GV_NB alt
                   so list. Changed certain 4GV featurizations.
10/19/05   sb      Added support for Band Class 14, Band Subclass I, J, K.
09/21/05   an      Added support for published Release D Addendum new Info Recs
09/19/05   pg      Added support for published Release D Addendum f-dsch and
                   r-dsch message changes.
09/15/05   fc      Added support for published Release D Addendum f-csh
                   message changes.
08/29/05   grl     Added CAI_IS_SO_SMS macro.
08/22/05   fc      Changes for Tools.
08/15/05   pg      Changed position of parentheses around feature check per
                   tools team request.
08/04/05   pg      Added support for Rel D Addendum f-dsch and r-dsch messages.
08/03/05   fc      Fixed lint errors.
08/01/05   an      Added support for Rel D Addendum r-csh messages, IRs and
                   Orders.
07/25/05   fc      Added support for Rel D Addendum f-csh messages.
07/12/05   ht      Added MEID Addressing support
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
05/20/05   fc      Merged the following from MSMSHARED:
05/20/05   an       Added band subclass info record
05/09/04   fc      Added MEID enums.
05/03/05   fc      Merged the following from MSMSHARED:
04/27/05   an       Added support for 4GV service option
03/28/05   fc      Added support for DCA.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/09/05   va      Merged following from MSMSHARED tip:
01/11/05   fh       Added CAI_6_BIT_FIELD_MAX for PMRM power strength checking.
                    The spec says this field has size of 6 bits
01/14/05   sb      Added a comment.
11/22/04   sb      Miscellaneous fixes for Rel C DV Handoffs.
09/27/04   va      Merged from IS2000_REL_B_C archive
09/16/04   sb        Added Rel C Addendum fields to BS Status Reponse Msg.
09/09/04   ejv       Replaced defines with cai_cpcch_rate_enum_type.
08/24/04   yll       Clean up MC-SRCH interface.
08/12/04   pg        Added Enhanced Multi-character Extended Record.
08/10/04   sb        Added cai_redirection_reason_type.
07/29/04   fc        Renamed bcch_index to bcn.
07/28/04   fc        Latest Rel C addendum changes on GNLM and UNLM.
07/08/04   sb        Added CAI_EXT_KEYPAD_REC definition.
07/05/04   sb        Added Extended Keypad Facility info rec.
06/29/04   sb        Removed featurization from cai_plcm_type.
06/16/04   fc        Rel C addendum changes for GNLM, UNLM and ECAM.
09/16/04   fc      Added enums for reverse DV channels.
09/15/04   va      Put ERM array sizes under #defines
09/15/04   va      Cut down BCMC field array sizes to just 1 if BCMCS is
                   not defined
08/12/04   bkm     Removed duplicate CAI_(ESN/IMSI)_ADDR_TYPE #defines
08/12/04   bkm     Added T79 timer for Rel D Bypass 2 Good Frames feature.
08/09/04   fc      Service negotiation changes for Release C and D.
08/03/04   ht      Added Rel D MEID LAC support
08/02/04   pg      Correct a compiling error due to wrong feature name.
07/28/04   pg      Added Enhanced Multi-character Extended Record.
07/28/04   fc      Latest Rel C addendum changes on GNLM and UNLM.
07/09/04   sb      Added support for Release D orders.
07/08/04   bkm     Fixed typo.
07/08/04   sb      Moved CAI_EXT_KEYPAD_REC defn to Rel C instead of Rel D.
07/07/04   sb      More Release D updates to info recs.
07/06/04   bkm     Support for REV_CQICH/ACKCH_REPS range checking.
07/06/04   sb      Made Release D updates to the info recs.
06/29/04   sb      Removed featurization from cai_plcm_type.
06/24/04   bkm     Added #define for CAI_MAX_NUM_SWITCHING_SLOTS_VALUE and
                   values from Table 3.7.2.3.2.21-9 for CQICH slots per frame.
06/16/04   fc      Rel C addendum changes on GNLM, UNLM and ECAM.
06/16/04   va      Rel C addendum changes for Registration msg
06/14/04   fc      Release C addendum changes on common channel messages.
06/11/04   sb      Release C Addendum changes for: ERM, RATCHGM, ITSPM.
06/09/04   sb      Release C Addendum changes for UHDM.
06/08/04   fc      Changes on BSPM.
06/03/04   fc      Added support for Release D common channel messages.
05/28/04   bkm     Merged the following from IS2000_REL_B_C:
  05/17/04   va      Merged the following from MSMSHARED:
   04/29/04  pg      Defined constants for range checking in PMRM.
   03/29/04  az        Changed the enum element name from CAI_KPCS_BAND_CLASS_5 to
                     CAI_BAND_CLASS_5 in enum cai_cdma_band_class
   03/25/04  bkm     To support unit testing set NV #define for maximum digits
                     to allow for 32 or 36 based on FEATURE_36_DIGIT_DIAL_STRING.
  04/08/04   fc      Removed duplicate definition of CAI_SYNC_ID_LEN_MAX.
05/19/04   az      Added support for ECAM Rel D parsing
05/14/04   va      Support for REL D access chan & rev TC messages
05/12/04   sb      Changed CAI_MAX_NUM_FOR_PDCCH to 8.
05/06/04   pg      Added Release D fields for UHDM.
04/23/04   az      Added Release D fields for
                   Service Connect Message
                   Service Configuration Record
                   Non-Negotiable Service Configuration record
04/19/04   bkm     Added Release D fields for RATCHGM and ITBSPM.
04/16/04   bkm     Added Release D fields for CLAM, ERM, ESCAM and BSSRSPM.
04/13/04   bkm     Added Release D fields for ITSPM and Power Control Message.
04/06/04   bkm     Added some initial Rel D types.
04/05/04   bkm     Made cai_ext_chind_type enum names more concise and added
                   Release D values.
04/02/04   az      Changed the enum element name from CAI_KPCS_BAND_CLASS_5 to
                   CAI_BAND_CLASS_5 in enum cai_cdma_band_class
03/31/04   bkm     Removed featurization around #defines for TDSO SOs.
03/25/04   bkm     Merged the following changes from BS header file.
    03/23/04   jpf     Added Release D Message, Order, & Info-Rec IDs.
    03/12/04   fc      Added support for Release C fast call setup.
02/05/04   bkm     Merged the following from IS2000_REL_B_C
    01/13/04   sb      Added p-rev 0 definition for SILK's benefit.
    01/06/04   sb      Added Release C Order fields.
02/02/04   bkm     Removed Rel C featurization from cai_ext_chind_type.
01/20/04   bkm     Switch CPCCH rate values from #defines to enum.
01/15/04   bkm     Added max enum value for cai_rpc_adj_rec_type.
01/05/04   bkm     Added enum value for RC 10.
12/09/03   sb      Added Release C fields of UHDM and GHDM.
12/02/03   bkm     Added #define for F-PDCH Mux Option 0xF00.
11/14/03   sb      Added Release C fields of Power Control Message.
11/13/03   sb      Miscellaneous Release C fixes.
11/11/03   sb      Added Release C fields of FTC Security Mode Command Msg.
11/10/03   sb      Added Release C fields of In-Traffic System Parameters Msg.
11/04/03   sb      Added Release C fields of Service Connect Message.
10/14/07   fc      Fixed User Zone Reject Message and info recs.
10/07/03   va      Merged the following from msmshared.
08/13/03   yll     Removed the FEATURE_IS2000_REL_A around CAI_MUX_VARIABLE.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
10/03/03   fc      Changes for CAM and order.
10/03/03   sb      Added Authentication Request Message on FTC.
10/02/03   fc      Added enum for PLCM type.
09/30/03   fc      Corrected CAI_REJ_MSG_CANNOT_DECRYPT.
09/26/03   fc      Changes for f-csh data burst message, authentication
                   challenge message, SSD update message and feature
                   notification messages.
09/23/03   fc      Changes for TMSI assignment message and PACA message.
09/19/03   fc      Changes for ECAM.
09/18/03   sb      Added Rate Change Msg, Rel C fields of ERM and CLAM.
09/18/03   fc      Changes for authentication request message.
09/16/03   fc      Changes for security mode command message.
09/10/03   sb      Changed CAI_AC_AUTH_REQ_MSG to CAI_TC_AUTH_REQ_MSG.
09/04/03   fc      Added support for UIM_ID and ESN_ME info recs.
08/08/03   fc      Added suport for IS2000 Release C messages.
07/31/03   yll     Added support for Dynamic Feature.
07/30/03   az      Added support for FEATURE_SO_8026
07/30/03   yll     Merged in the following changes:
           vas     Added non standard release reason CAI_REL_ABORT_CALL
                   Based on //depot/asic/msmshared/1x/cp/cai.h#131
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/18/03   fc      Added support for decoupling FCH and DCCH MUX.
04/09/03   sb      Added CAI_EPSMM_MAX_NUM_PILOTS.
03/25/03   fc      Corrected feature name.
03/20/03   sb      Changed CAI_GEN_NGHBR_MAX and CAI_UNLM_MAX_NUM_NGHBR
03/13/03   vr      Added CAI_REL_CTA_EXPIRED
02/27/03   sb      Added Release B changes.
02/05/03   fc      Added Release B f-csh changes.
02/05/03   va      Added Release B changes.
02/05/03   fc      Cleanup for f-csh forward compatibiliity.
01/27/03   jqi     Added band class 6 support
01/10/03   fc      Added support for sync channel workaround.
11/07/02   az      Removed FEATURE_IS2000_REV0_PREADDENDUM
11/07/02   az      Mainlined FEATURE_IS95B
10/02/02   yll     Linted the code.
10/01/02   fc      Added BCCH, FCCCH and EACH BA mode parameter support.
09/30/02   az      Mainlined FEATURE_IS2000 and FEATURE_IS2000_2_1_ADDENDUM.
08/23/02   jrp     Changed cai_ac_ext_type.body to have space for extended
                   message length in case of Release A.
08/15/02   fc      Updated comments.
08/08/02   sb      Changed BCCH/FCCCH constants to account for extended length.
07/17/02   ph      fixed call wait struct reserved length
07/12/02   ht      (et) added cai_cdma_band_class
07/08/02   az      Added definition for Short Data Burst
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
06/28/02   lh      Changed CAI_MUX_32X_VARIABLE to CAI_MUX_VARIABLE.
06/18/02   yll     Added support for Base Station Status Req/Rsp Message.
06/18/02   jrp     Corrected enum names for cai_fpc_mode_type.
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
06/04/02   az      Added a backslash to the macro CAI_IS_SO_VOICE to get rid
                   of the compiler error
05/29/02   ph      Added support for LPM table
05/24/02   az      Added support for SMV (SO 56) voice service option
05/20/02   fc      Added enums cai_page_class_type, cai_page_subclass_type and
                   cai_page_subclass_ext_type.
05/08/02   fc      Added support for Universal Page Messages.
05/06/02   yll     Moved the CAI_INV_REPEAT_TIME_OFFSET outside of FEATURE_
                   IS2000_REL_A_BCSMS for the correct message parsing.
04/17/02   sb      Increased size of CAI_MAX_NUM_CACH and CAI_MAX_NUM_CPCCH by 1.
04/12/02   lh      Added support for subaddress in Orig_C.
04/09/02   yll     Added Broadcast SMS support on Rel A common channel.
04/05/02   fc      Changed enums cai_fccch_data_rate_type and
                   cai_fccch_frame_size_type.
04/01/02   yll     Added retrievable and settable parameters for BCCH and
                   FCCCH.
03/26/02   fc      Changed CAI_BCCH_MAX_SIZE and CAI_FCCCH_MAX_SIZE.
03/21/02   sb      Changed defintion of CAI_UNLM_MAX_NUM_NGHBR to CAI_N8M
03/08/02   sb      Updated CAM, ESP, ECAM, Ansi-41, MC-RR, EAPM and SMCM to be in
                   sync with Addendum 2 published version.
03/04/02   ph      Updated SCM message format to be in sync with Add. 2 pub version
02/22/02   fc      Added support for Release A common channels.
02/13/02   lh      Merged following from common archive.
                   02/06/02   sh      Added CAI_REL_RUDE
                   01/25/02   sj      Added User Zone registration type.
                   01/22/02   bn      Added support for a counter of consecutive DTX frames
                   01/18/02   ht      Added FEATURE_DS_RLP3.
                   12/07/01   jqi     Mapped CAI_BAND_PCS to band class 5 if FEATURE_BAND_CLASS_5
                                      was defined.
02/11/02   va      Added CAI_IS_SI_VOICE and CAI_IS_SO_SIGNALING macros
01/24/02   ph      Fixed QOS record structure.
01/08/02   lh      Added calculation of MAX message length on REACH.
12/20/01   lh      Added Mux counters for MuxPDU 5.
12/11/01   ph      Merged from SVD branch
                   08/28/01   lh      SVD support.
                   08/23/01   ph      Added define for max number of connections.
12/06/01   lh      Merged following from common archive.
  11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
  10/29/01   sh      Updated comments for CAI_SO_HDR_PKT_DATA
11/07/01   sb      Changed definition of CAI_EXT_CHNLIST_MAX to 15 - dependent on NUM_FREQ
10/15/01   kk      Fixed compiler errors when Release A is not defined.
10/12/01   sb      Replaced probe_pn_ran in EAPM with reserved field; fixed spelling error
10/12/01   fc      Added cai_fccch_data_rate_type and cai_fccch_frame_size_type.
10/02/01   ph      Merged in CP VU 96
                   08/21/01   sj      Changed rs1_20ms_incl -> rc3_rc5_20ms_incl(Power Control
                                      Msg), rs2_20ms_incl ->rc4_rc6_20ms_incl(Power Control Msg)
                                      & num_visits -> max_num_visits(Candidate Frequency Search
                                      Response message).
09/28/01   kk      Added missing position location indicator to Flash msg.
09/26/01   fc      Changed definitions cai_bcch_rate_type, cai_fccch_rate_type
                   and cai_code_rate_type.
09/10/01   sb      Added definitions from caii.h.
08/27/01   lh      Changed FEATURE_EXTENDED_MSG_LENGTH to FEATURE_TC_EXTENDED
                   _MSG_LENGTH.
08/17/01   lh      Update Infor Rec type.
08/16/01   ph      Rel A Addendum 2 changes for the FTC
08/16/01   lh      Addendum 2 Changes for F-CSCH and R-CSCH.
08/15/01   kk      Addendum 2 changes on RTC.
08/10/01   lh      Resolve the duplicated defined cai_rate_set_type. See merge
                   comments below.
08/10/01   lh      Name change of the 32X counters.
08/10/01   kk      Increased mux option array size to 18, if REL_A is defined.
08/09/01   sb      Merged following from common archive:
                   08/02/01   jrp     Added CAI_SO_MARKOV_SO54 and CAI_SO_LOOPBACK_SO55.
                                      Added cai_rate_set_type to enumerate rate sets.
                                      Added CAI_MUX_WILDCARD.
                   07/11/01   lcc     Removed limitation of 3 SCCH when T_MSM5000 is defined.
07/18/01   fc      Renamed CAI_BCCH_1_RATE to CAI_BCCH_4800_RATE,
                   CAI_BCCH_2_RATE to CAI_BCCH_9600_RATE and
                   CAI_BCCH_3_RATE to CAI_BCCH_19200_RATE.
07/16/01   fc      Added CAI_MAX_NUM_CHAN_MONITOR.
07/16/01   fc      Added cai_pch_rate_type, cai_bcch_rate_type,
                   cai_fccch_rate_type and cai_code_rate_type enums.
06/26/01   kk      Added missing field in EOM.
06/26/01   kk      Merged the listed changes from common archive.
  06/15/01   lcc     Added FEATURE_IS2000 around align_timing in CFSCM.
  06/15/01   lcc     Corrected size of reserved in CFSCM to be 3.
  06/14/01   lcc     Corrected Candidate Frequency Search Control Message and
                     Channel Configuration Capability Information Record to be
                     Rel. 0 Addendum 2 compliant.
  05/27/01   snn     Added support for RS2 under feature FEATURE_DS_IS2000_RS2
  05/22/01   kk      Increased CAI_MAX_MULT_OPT_SCH size to 16, to accomodate
                     Rate Set 2 Mux options.
05/16/01   lh      Merged following changes from common archive.
  04/18/01   fc      Moved cai_chind_type under FEATURE_IS2000.
  04/17/01   va      Added cai_pilot_rec_type enum.
  04/13/01   sh      Added CAI_SO_HDR_PKT_DATA
  04/12/01   fc      Moved enumeration cai_chind_type outside FEATURE_IS2000 to
                   support events report.
  04/10/01   lcc/jq/ Added definitions related to DCCH and contorl hold support.
                         bc
05/10/01   lh      Added comments.
04/26/01   lh      Clean up Band Class Info Rec.
04/17/01   lh      Added code for unit testing.
04/12/01   ph/kk   Support for new Power Control fields.
04/09/01   lh      Added LTU_TABLE_SUPPORTED bit in the Capability Info Rec.
03/27/01   kk      Merged mainline fixes to CP_REL_A archive.
03/12/01   kk      Order related fixes.
03/13/01   ph      Fixes related to FTC msgs
03/12/01   lh      UNLM optimization.
03/08/01   lh      Bring back SAR changes.
03/07/01   lh      Add bug fixes for PCH/ACH messages.
03/06/01   kk      fixes related to Service config, connect complete
03/06/01   ph      removed con_ref from some ftc orders, also re-arranged
                   tc header by removing the ext. enc fields to a separate struct.
03/01/01   kk      Merged mainline changem from the latest CP VU.
01/24/01   ych     Merged T53 and JCDMA features.
11/29/00   ak      Added new XTRA parms for LTU CRC statistics.
02/27/01   kk      Added new literals for SO grouping and Silent ReOrigination
02/09/01   lh      Mux option mask and enum added.
02/09/01   va      Changed pilot rec  types into a enum.
01/29/01   ph      Added the enums CAI_TD_MODE_STS (& OTD) for Release A support.
01/29/01   va      Expanded the fpc_mode enum.
12/21/00   kk/lh   Added IS-2000 Release A support.
10/30/00   ks      Fixed the CAI_SO_FULL_TDSO from 0x032 to 0x20.
10/19/00   ks      Added Service Option for CAI_SO_FULL_TDSO.
10/02/00   yll     Added CAI_POSITION_DET burst type. Removed msid_hdr_type.
09/19/00   lcc     Corrected number of bits in rev_sch_id and for_sch_id in
                   reverse traffic SCR to 2 for BRV.
08/31/00   lcc     Added reserved bits in nn-SCR for BRV version of CDMA 2000.
07/26/00   ak      Added #defines for IS2000 RS1 Mux options.
07/21/00   fc      Changed FPC mode definitions to use enumerated type.
                   Changed power control step size to use enumerated type.
                   Added SCH rate definition.
07/21/00   va      Added cai_sch_rate_type enum.

07/17/00   va      Added CAI_DEFAULT_REV_PWR_CNTL_DELAY
07/17/00   jq      Added CAI_REL_SRV_INACT definition
07/10/00   ks      Added CAI_SO_TDSO service option for TDSO support.
07/06/00   yll     Added support for GPS.
07/06/00   jq      Added definititions for ECCLM and EGSRM. Modified CFS_RSP
                   for addendum compliance.
06/21/00   fc      Renamed MAX_SCRM_BLOB_SIZ to CAI_MAX_SCRM_BLOB_SIZ,
                   MAX_SCRM_ACT_PN to CAI_MAX_ACT_PN and MAX_SCRM_NGHBR_PN to
                   CAI_MAX_SCRM_NGHBR_PN.
                   Added CAI_MAX_SCRM_RPT_PN and retry type definitions.
06/20/00   cah     Remerged in changes that were lost in last checkin.
           jrw     Corrected a problem with power control message where the
                   init_setpt shouldn't have been included with the SCH set points.
06/19/00   lad     Updated IS2000 support for use in PARM database.
06/12/00   ks      Added support for align_timing fields in CFSREQ message.
06/09/00   ak      Added statistics for IS2000 MUX SCH0.
06/02/00   lcc/jrw Changes to support IS2000 Rev 0 published addendum.  Also
                   added some constant definitions.
05/25/00   ks      Added support for Neighbor SRCH window offset.
05/04/00   va      Changed N1M value to 13 for IS2000 targets.
04/24/00   lcc/jrw Added ESCAM, outer loop report message, and changes to PMRM.
04/17/00   va      Added constant definition of L2 length field.
04/06/00   va      Merged the following from MSM3100_CP.03.00.17
           ry      Added support for OTAPA (IS-683A SO 18 and 19)
03/10/00   ry      Added IS2000 featurization
           ry      Replaced FEATURE_SILK with FEATURE_MSG_PACK_UNIT_TEST
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
           fc      Added support for IS-733 service option 17.
03/06/00   lcc     Added definitions for maximum rates for SCH.
02/29/00   jrw     Fixed CAI_UNIV_HO_DIR_MSG (0x24 -> 0x22)
02/11/00   jrw     Added support for IS2000 Addendum
12/17/99   jrw     Added support for IS2000 Phase 1.
11/30/99   na      Added 1x Pkt Service option - 33.
10/20/99   lcc     Added max. mux options for MSM5000.  Corrected max number of
                   supplementals from 8 to 7.
09/16/99   lcc     Merged in radio configurations definitions from PLT archive.
09/02/99   nmn     Included mux values 3-16 always to enable cleaner DS SO CFG
                   code, for non-MDR builds.
08/17/99   kk      Modified the service redirection message for TC.
08/13/99   jq      Added support for incoming call forwarding enhancement.
                   FEATURE_IS95B_INC_CALL_FWD_ENH
08/13/99   doj     Unincluded new reject reasons for FEATURE_SPECIAL_MDR to
                   be consistent with original FEATURE_SPECIAL_MDR.
08/06/99   doj     Added masks for sign extension of add_intercept and
                   drop interecept for handoff soft slope equation.
07/09/99   doj     Included a few more reject reasons under FEATURE_SPECIAL_MDR
                   and added service negotiation types (Merge from HHO_DEV)
06/17/99   kmp     Added timer t72 to the cai_tmo_type structure. Changed
                   values of CAI_DEF_CONFIG_FOR1_REV2 and CAI_DEF_CONFIG_FOR2_REV1.
06/02/99   kmp     Merge IS-95B changes listed below from SPSW code base.
           lh      Support release order on access channel, name change
                   from reserve to auth_mode.
           kk      NDSS literals defined.
           lh      Updated for Published version of IS-95B.
           fc      Renamed CAI_MAX_PILOTS to CAI_N13M.
           fc      Put in changes based on input from code review.
           jq      Modified CAI_GRANT_MUX1 to CAI_GRANT_USE_DC
           fc      Fixed lint error.
           kmp     Merged in IS-95B additions listed below from TGP baseline
                   and ported IS-95B changes to ARM
                   Following are the IS-95B additions:
           jq      Added ECAM/CAM default_config definitions.
                   CAI_DEF_CONFIG_xxxxxx
           lh      Fixed msg format error in FTC service redir msg.
           fc      Changed CAI_MAX_ADD_PILOTS to 5. Added CAI_MAX_PILOTS.
                   Changed FEATURE_IS95B_AHO to FEATURE_IS95B_ACCESS_HO.
           ks      Added a new timer T72 for IS95B AHO & APHO.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           lh      Modified ECAM. Now parser will parse the ECAM and pass the
                   caller a packed internal format.
           lh      Added comments.
           lh      Added comments.
           jcw     Added defines for 95B Extended Display support
           lh      Change soft_slop to soft_slope in ESPM processing.
           jcw     Parsing of extended display record has been updated to
                   not treat blank & skip subrecords specially.
           lh      Define max length for pilot report external format.
           lh      IS-95B support.
05/25/99   lcc     Corrected pref_msid_type field in extended system parameters
                   message so that it is 2 bits for no-TMSI (IS-95-A) builds
                   instead of 3.
05/19/99   doj     Consolidated structures for HDM, EHDM and GHDM into one
                   generic handoff structure.
03/30/99   ram     Verified and check-ed in changes made for MDR
                   DM support by aaj.
01/27/99   yll     Fixed CR7630. Added CAI_RS1_RLP to comply with IS707.4.
02/16/99   doj     Added PACKED qualifier to cai_redirect_rec1_type.
02/13/99   doj     Added support for General Handoff Direction Message (GHDM).
                   Also includes several misc. updates by many people to
                   support FEATURE_SPECIAL_MDR.
02/13/99   doj     Added PACKED qualifier to cai_rtc_rej_type and
                   cai_orig_c_var_type
02/10/99   nmn     Added FEATURE_SPECIAL_MDR support.  Also changed below year
                   from 00 to 99.
01/27/99   ck      Added FEATURE_EVRC_SO_MGMT around CAI_SO_WILDCARD.
01/27/99   nmn     Added MDR PARAMER_IDs and #defined MAX_MUX for RS1 and RS2
01/16/99   ram     Merged in the changes from IS95B MDR Branch
12/11/98   lcc     Corrected CAI_MAX_SUP_PILOTS to be defined as CAI_N6M (6)
                   instead of 7.
12/01/98   ram     Added MDR DPT Service Options
11/27/98   lcc     Added definitions for MDR related service options and mux.
                   options.
10/20/98   ldg     Added SO numbers for analog end to end fax (IS-707A).
10/09/98   ck      Added CAI_SO_VOICE_WILDCARD for EVRC Service Negotiation
09/01/98   lcc     Merged in ARM support.
08/25/98   ck      Added CAI_SO_RS2_SMS Service Option.
08/12/98   lh      IS95B WLL support: Line Control, Parametric Alerting and
                   Meter Pulses.
06/15/98   ck      Changed CAI_SO_EVRC to CAI_SO_VOICE_EVRC
04/10/98   ldg     Fixed IS-707 RS1 SO numbers.  Specified as hex in SPP,
                   these were actually decimal per PN-3676.1.
02/02/98   day     Consolidating feature #define's for Korean PCS support.
                   Changed FEATURE_CP_KPCS to FEATURE_KPCS.
01/16/98   lh      "customer.h" included.
01/15/98   lh      Added IS-95B Line Control info record.
01/14/98   dna     Add support for Korean PCS in Band Class Status Response
11/03/97   ldg     Added (and renamed some) data service option numbers to
                   support IS-707.0, Qualcomm proprietary, and IS-707,
                   without a compile-time switch.
07/24/97   na      Added FEATURE_DS_PROPTRY_SO which includes the Qualcomm
                   proprietary service option numbers for rs2 data services.
07/24/97   jca     CAI_PRIV_PLAN changed to correct value as per IS-95A.
07/09/97   na      Changed rateset2 data service option numbers to Qualcomm
                   proprietary numbers used by CBS.
06/10/97   ck      Added CAI_SO_EVRC service Option
02/06/97   jca     Changed T50m from 200 msecs to 1 second.
12/13/96   jca     Added new fields to cai_chnasn_am2_type.
11/14/96   jjw     Added Rate Set 2 data service SO definitions
10/30/96   rdh     Changed N1m default from 3 to 9.
06/12/96   jca     Added #defines for Data Burst Msg Burst Type Assignments.
06/05/96   jpf     Reduced minimum size of several messages in JSTD008 because
                   the smallest MSID went from an ESN to a 2-byte TMSI.
05/28/96   dna     Class 1 IMSI support
04/15/96   jca     Added #define for CAI_STOP_CONT_DTMF order qualifier.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
03/13/96   day     Added J-STD-008 PCS message modifications.
                   Using FEATURE_JSTD008 ifdef.
02/06/96   fkm     Put ()s Around Macros to Make Lint Happy
12/12/95   jca     Added CAI_SO_WLL_OFF_HOOK service option.
09/22/95   jca     Restored N1m to IS95A specified value of 3.
08/22/95   dna     New sync channel message field
08/18/95   dna     Added support for assign mode '101'.
08/16/95   gb      Changed CAI_MAX_FRAME_SIZE to support rate set 2 frames.
08/14/95   gb      Added MUX2 parameter support.
08/10/95   rdb     Added defs for new information record types, Status Request
                   Message, Status Response Message per the IS-95-A TSB.
07/27/95   gb      Changed field in Extended Handoff Direction Message.
07/20/95   gb      Added Extended Handoff Direction Message & new Markov SOs.
07/20/95   jca     Added Extended Sys Parameters and Service Redirection
                   messages.  Modified other messages as per IS95A.
07/13/95   rdb     Added defs for channel assignment type 4 record,
                   along with defs for service negotiation messages and
                   time constants and mux option and rate sets and loopback13.
06/25/95   dy      Added define for SMS service option.
06/07/95   dna     Added SSD Update ORDQ's and T64m timeout
05/30/95   gb      Added 13k markov and voice service options.
03/01/95   jjw     Added IS-99 and packet Service Option numbers.
01/10/95   dna     Added reject type CAI_WLL_OFF_HOOK
08/09/94   jca     Added new defines for IS-96A/IS-96 voice service options.
05/12/94   gb      Changed N1m to 13 to support high FER testing.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/20/94   jca     Added mux1 enums and constants.  Added define for
                   Loopback service option.
12/22/93   jca     Changed message formats to IS-95 compatible.
10/29/92   jai     Added new System Parameter msg, In-Traffic System Parameter
                   message and renamed signal types for UI task.
07/23/92   jai     Modified file for lint and linking.
07/08/92   jai     Created file

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "qw.h"
#include "nv.h"


/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
** Frame Data Rates
---------------------------------------------------------------------------*/

typedef enum
{
  CAI_BLANK_RATE,     /* Indicates data was blanked */
  CAI_EIGHTH_RATE,    /* Indicates rate 1/8 data    */
  CAI_QUARTER_RATE,   /* Indicates rate 1/4 data    */
  CAI_HALF_RATE,      /* Indicates rate 1/2 data    */
  CAI_FULL_RATE,      /* Indicates rate 1   data    */
  CAI_NULL_RATE,       /* Indicates DTX rate         */
  CAI_NON_CRITICAL_RATE = 255 /* Indicates DTX-able 1/8 rate vocoder frame */
} cai_data_rate_type;

/*---------------------------------------------------------------------------
** Traffic types.
** Values for CAI_NULL_TRAF, CAI_PRI_TRAF, CAI_SEC_TRAF, CAI_SIG_TRAF and
** CAI_UNCLASSIFIED_TRAF are defined in the Standard.  Do not change these
** values. CAI_PRI_OR_SEC_TRAF is used to indicate if a particular service
** option allows either type of traffic. Internal use only.
---------------------------------------------------------------------------*/

#define CAI_PRI_OR_SEC_TRAF 3

typedef uint8   cai_traffic_type_type;
#define CAI_PRI_TRAF    ((cai_traffic_type_type) 1)
#define CAI_SEC_TRAF    ((cai_traffic_type_type) 2)
#define CAI_SIG_TRAF    ((cai_traffic_type_type) 3)
#define CAI_UNCLASSIFIED_TRAF  ((cai_traffic_type_type) 4)

typedef uint8   cai_radio_config_type;

/* The following values specify rate set 1 or rate set 2 */
typedef enum {
  CAI_RATE_9600    = 0x00, /* Select 9600 bps rate set   */
  CAI_RATE_14400   = 0x01, /* Select 14400 bps rate set  */
  CAI_RATE_INVALID = 0x02  /* Invalid rate set           */
} cai_rate_set_type;

/*-------------------------------------------------------------------------*/
/*  Sync Channel Constants                                                 */
/*-------------------------------------------------------------------------*/

#define CAI_SC_MAX_MSG_BYTES 148
  /* Max Sync Channel message length (see IS-95A Section 7.7.1.2.1) */

/*-------------------------------------------------------------------------*/
/* Paging Channel constants                                                */
/*-------------------------------------------------------------------------*/

#define CAI_PC_MAX_MSG_BYTES 255
  /* Maximum Paging Channel message length (see IS-95A Section 7.7.2.2.1) */

/*-------------------------------------------------------------------------*/
/* Traffic Channel constants                                               */
/*-------------------------------------------------------------------------*/

#define CAI_FWD_TC_MAX_MSG_BYTES 512
#define CAI_REV_TC_MAX_MSG_BYTES 300

/* Accessor functions for getting tc_max_msg_bytes */
extern uint16 cai_get_fwd_tc_extended_msg_len(void);
extern uint16 cai_get_rev_tc_extended_msg_len(void);

/*-------------------------------------------------------------------------*/
/* Access Channel constants                                                */
/*-------------------------------------------------------------------------*/

#define CAI_AC_MAX_MSG_BYTES 255
  /* Maximum Access Channel message length (see IS-95A Section 6.7.1.2.1) */

/* -----------------
** Retry Order Types
** ----------------- */
typedef enum
{
  CAI_RETRY_CLR_ALL = 0, /* Clear all retry delay */
  CAI_RETRY_ORIG    = 1, /* Origination retry delay */
  CAI_RETRY_RESA    = 2, /* Resource request retry delay */
  CAI_RETRY_SCRM    = 3  /* Supplemental Channel request retry delay */
  ,CAI_RETRY_SDB    = 4  /* Short Data Burst retry delay */
  ,CAI_RETRY_SDB_AND_ORIG = 5 /* Short Data Burst and Origination retry delay */
} cai_retry_order_type;

/* Reverse Channel Specific Order Codes */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define CAI_REL_NORMAL 0x00
  /* Value of ORDQ to indicate a normal release */
#define CAI_REL_PWDN   0x01
  /* Value of Order qualifier to indicate a release with a powerdown
     indication */
#define CAI_REL_SRV_INACT  0x02
  /* Value of ORDQ to indicate a release with service inactive indication */
#define CAI_REL_REDUCED_SCI 0x03
  /* Value of Order qualifier: reduced slot cycle mode indication */

#define CAI_REL_SO     0x02
  /* Value of Order qualification code to indicate that call is released
     because the requested service option is rejected */

#define CAI_REL_RUDE 0xFE
  /* Rule close used internally by HDRMC */

#define CAI_REL_ABORT_CALL 0xFD
  /* Close reason used internally by HDRMC to abort the call id rather than
     bring down the traffic channel */

#define CAI_REL_CTA_EXPIRED  0x04
  /* Used internally by DS to check if call dropped due to CTA expiry */

#define CAI_EXT_REL_IND 0xFF
  /* Extended release indication for existing TC state */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                    DTMF_ON_LENGTHS                                      */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 6.7.2.3.2.7-1. Recommended DTMF Pulse Width            */

#define CAI_DTMF_ON_95  0x00
#define CAI_DTMF_ON_200 0x02
  /* 200 ms recommended pulse width */
#define CAI_DTMF_ON_250 0x03
  /* 250 ms recommended pulse width */
#define CAI_DTMF_ON_300 0x04
  /* 300 ms recommended pulse width */
#define CAI_DTMF_ON_350 0x05
  /* 350 ms recommended pulse width */
#define CAI_DTMF_ON_SMS 0x07
  /* SMS Tx special pulse width */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                    DTMF_OFF_LENGTHS                                      */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 6.7.2.3.2.7-2 Recommended Minimum Interdigit Interval  */

#define CAI_DTMF_OFF_60  0x00
#define CAI_DTMF_OFF_150 0x02
  /* 150 ms recommended minimum interdigit interval */
#define CAI_DTMF_OFF_200 0x03
  /* 200 ms recommended minimum interdigit interval */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

  /* 95 ms recommended pulse width */
#define CAI_DTMF_ON_150 0x01
  /* 150 ms recommended pulse width */
  /* 60 ms recommended minimum interdigit interval */
#define CAI_DTMF_OFF_100 0x01
  /* 100 ms recommended minimum interdigit interval */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  SERVICE OPTIONS                                        */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define CAI_SO_REJ          0x0000
  /* Service Option number to indicate service option rejection */

#define CAI_SO_VOICE_WILDCARD  0x0000
 /* Service Option to indicate a wildcard voice service option that
    is internal to UI and Call Processing. UI sends this Service
    Option in the origination command to CP if the call is not orig by DM
    or if it is not a voice call */

#define CAI_SO_VOICE_IS96A  0x0001
 /* Basic Variable Rate Voice Service (IS-96A) */

#define CAI_SO_LOOPBACK     0x0002
  /* Mobile Station Loopback (IS-126) */

#define CAI_SO_VOICE_EVRC   0x0003
  /* Enhanced Variable Rate Voice Service */

#define CAI_SO_VOICE_SMV     0x0038
   /* Selectable Mode Vocoder, Service option 56 */

#define CAI_SO_VOICE_4GV_NB  0x44
   /* 4GV Narrow Band Service Option */
#define CAI_SO_VOICE_4GV_WB  0x46
   /* 4GV Wide Band Service Option */

#define CAI_SO_VOICE_EVRC_NW 0x49
  /* Enhanced Variable Rate Voice Service EVRC-NW Service Option 73
   * EVRC-NW : EVRC-WB with NB capacity operating points and DTX */

#define CAI_SO_ASYNC_DATA_PRE707     0x0004 /* IS-99 s.o. number      */
  /* Asynchronous Data Service (IS-99) */

#define CAI_SO_G3_FAX_PRE707         0x0005 /* IS-99 s.o. number      */
  /* Group 3 FAX Service (IS-99) */

#define CAI_SO_SMS          0x0006
  /* Short Message Services (IS-637) */

#define CAI_SO_PPP_PKT_DATA_PRE707  0x0007 /* IS-657 s.o. number     */
  /* Internet Standard PPP Packet Data Service (IS-657) */

#define CAI_SO_LOOPBACK_13K 0x0009
  /* Mobile Station Loopback (IS-126) */

#define CAI_SO_ASYNC_DATA_13K_PRE707     0x000C /* IS-99 s.o. number  */
  /* Asynchronous Data Service (IS-99) over Rate set 2 */

#define CAI_SO_G3_FAX_13K_PRE707         0x000D /* IS-99 s.o. number  */
  /* Group 3 FAX Service (IS-99) over Rate set 2 */

#define CAI_SO_RS2_SMS                   0x000E
  /* Short Message Services using Mux Option 2 (TSB-79) */

#define CAI_SO_PPP_PKT_DATA_13K_PRE707  0x000F /* IS-657 s.o. number */
  /* Internet Standard PPP Packet Data Service (IS-657) over Rate set 2 */

#define CAI_SO_VOICE_13K_IS733          0X0011 /* IS-733 s.o. number */
  /* High Rate Voice Service (13 kbps) IS-733 */

#define CAI_SO_RS1_OTAPA                0x0012 /* IS-683A s.o. number */
  /* Over The Air Parameter Administration over Rate set 1 */

#define CAI_SO_RS2_OTAPA                0x0013 /* IS-683A s.o. number */
  /* Over The Air Parameter Administration over Rate set 2 */

#define CAI_SO_MDR_PKT_DATA_FRS1_RRS1   0x0016 /* IS-707A s.o. number = 22 */
  /* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
   (IS-707A) with rate set 1 forward and reverse.  Default Mux = 9 forward 1 reverse */

#define CAI_SO_MDR_PKT_DATA_FRS1_RRS2   0x0017 /* IS-707A s.o. number = 23 */
  /* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
   (IS-707A) with rate set 1 forward and rate set 2 reverse.
   Default Mux = 9 forward 2 reverse.  This SO is not support by MSM3000. */

#define CAI_SO_MDR_PKT_DATA_FRS2_RRS1   0x0018 /* IS-707A s.o. number = 24 */
  /* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
   (IS-707A) with rate set 2 forward and rate set 1 reverse.
   Default Mux = 10 forward 1 reverse.  This SO is not support by MSM3000. */

#define CAI_SO_MDR_PKT_DATA_FRS2_RRS2   0x0019 /* IS-707A s.o. number = 25 */
  /* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
   (IS-707A) with rate set 2 forward and reverse.  Default Mux = 10 forward 2 reverse */

#define CAI_SO_RS1_PD                   0x0023
  /* IS-801 Position Determination Service Option 35 for rate set 1  */
#define CAI_SO_RS2_PD                   0x0024
  /* IS-801 Position Determination Service Option 36 for rate set 2  */

#define CAI_SO_MDR_DPT_FRS1_RRS1        0x8024 /* MDR Data Pattern Test: Proprietary
                                               Service Option rate set 1 fwd and rev */

#define CAI_SO_MDR_DPT_FRS2_RRS2        0x8025 /* MDR Data Pattern Test: Proprietary
                                               Service Option rate set 2 fwd and rev */

#define CAI_SO_ANALOG_FAX_RS1    0x0014 /* IS-707A */
  /* Analog End to End Fax */

#define CAI_SO_ANALOG_FAX_RS2  0x0015 /* IS-707A */
  /* Analog End to End Fax */

#define CAI_SO_ASYNC_DATA_IS707         0x1004 /* decimal 4100 */
  /* Asynchronous Data Service (IS-707) */

#define CAI_SO_G3_FAX_IS707             0x1005 /* decimal 4101 */
  /* Group 3 FAX Service (IS-707) */

#define CAI_SO_PPP_PKT_DATA_IS707       0x1007 /* decimal 4103 */
  /* Internet Standard PPP Packet Data Service (IS-707) */

#define CAI_SO_VOICE_13K    0x8000
 /* Speech Service Option (13K) */

#define CAI_SO_VOICE_IS96   0x8001
 /* Speech Service Option (IS-96) */

#define CAI_SO_MARKOV       0x8002
  /* Markov Service Option */

#define CAI_SO_DATA         0x8003
  /* Data Services Service Option, HACK Packet method */

#define CAI_SO_MARKOV_13K   0x801C
  /* Markov Service Option (13k) */

#define CAI_SO_RS1_MARKOV   0x801E
  /* Markov Service Option (8k) */

#define CAI_SO_RS2_MARKOV   0x801F
  /* Markov Service Option (13k) */

#define CAI_SO_WLL_OFF_HOOK 0x801D
  /* WLL phone off hook Service Option */

#define CAI_SO_PPP_PKT_DATA_3G 0x0021
  /* CDMA2000 pkt service option */

#define CAI_SO_MARKOV_SO54          0x0036
  /* Markov Service Option 54*/
  /* See 3GPP2 C.S0025 */
#define CAI_SO_MARKOV_SO74          0x004A
  /* Enhanced Markov Service Option 74 with DTX capability */

#define CAI_SO_LOOPBACK_SO55        0x0037
  /* Loopback Service Option 55*/
  /* See 3GPP2 C.S0013-A */

/* Note, if you add more SOs make sure the macros following
these (CAI_IS_SO_VOICE and CAI_IS_SO_SIGNALING) are up to date */

/* Service option for Simple TDSO */
#define CAI_SO_TDSO         0x8008
/* Service option for Full TDSO */
#define CAI_SO_FULL_TDSO    0x0020

#define CAI_SO_PPP_PKT_DATA_13K_PROPTRY 0x8020 /* QC proprietary number */
  /* Internet Standard PPP Packet Data Service (QC Prptry) over Rate set 2 */

#define CAI_SO_ASYNC_DATA_13K_PROPTRY   0x8021 /* QC proprietary number */
  /* Asynchronous Data Service (QC Proptry) over Rate set 2 */

#define CAI_SO_G3_FAX_13K_PROPTRY       0x8022 /* QC proprietary number */
  /* Group 3 FAX Service (QC Proptry) over Rate set 2 */

#define CAI_SO_PROPRIETARY_8026 0x8026 /* QC proprietary number */
#define CAI_SO_HDR_PKT_DATA 0x80FF   /* For internal use only!
                                        HDR pkt data service option
                                        can not be used for packet
                                        call origination */

/* Note, if you add more SOs make sure the macros following
these (CAI_IS_SO_VOICE and CAI_IS_SO_SIGNALING) are up to date */

#define CAI_SO_NULL         0xFFFF
  /* NULL value for service option negotiation */

/*---------------------------------------------------------------*/

  /* Number of received consecutive good Forward Traffic Channel
     frames before a mobile enables its transmitter in TCI */
#define CAI_N6M 6     /* Supported Traffic Channel Active Set size */

#define CAI_N7M 10    /* Supported Traffic Channel Candidate Set size */
#define CAI_N8M 40    /* Minimum supported neighbor set size */


#define CAI_MAX_NUM_FOR_SCH           2
#define CAI_MAX_NUM_REV_SCH           2

#define CAI_MAX_MULT_OPT_SCH          18

/* Maximum number of alternative service options */
#define CAI_AC_ALT_SO_MAX             10

/* Acccessor function to access max alt so */
extern uint8 cai_get_ac_alt_so_max(void);

/* Maximum length of the QOS fields, including reserved bits */
#define CAI_ORIG_QOS_LEN_MAX  31

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PAGE RESPONSE MESSAGE  (see IS-95A Section 6.7.1.3.2.5)                 */


#define CAI_SBDTMF_MAX 255
  /* Max number of digits is constrained by size of the num_digits field */
#define CAI_MAX_NUM_SUP     7       /* Maximum # of supplemental channels */
/*-------------------------------------------------------------------------*/
/* SCH constants                                                */
/*-------------------------------------------------------------------------*/
/* SCH rate enum, As defined in the std, do not change these */
typedef enum
{
  CAI_SCH_RATE_RS1_1X_RS2_1X = 0,
  CAI_SCH_RATE_RS1_2X_RS2_2X = 1,
  CAI_SCH_RATE_RS1_4X_RS2_4X = 2,
  CAI_SCH_RATE_RS1_8X_RS2_8X = 3,
  CAI_SCH_RATE_RS1_16X_RS2_16X = 4,
  CAI_SCH_RATE_RS1_32X_RS2_18X = 5,
  CAI_SCH_RATE_RS1_64X_RS2_32X = 6,
  CAI_SCH_RATE_RS1_INVALID_RS2_36X = 7,
  CAI_SCH_RATE_RS1_INVALID_RS2_72X = 8,
  CAI_SCH_RATE_NULL=0xff
}cai_sch_rate_type;


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                        PRESENTATION INDICATORS                          */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 6.7.4.4-1. Presentation Indicators                     */

#define CAI_ALLOW_PI     0x00
  /* Presentation allowed */
#define CAI_RES_PI       0x01
  /* Presentation restricted */
#define CAI_NOT_AVAIL_PI 0x02
  /* Number not available */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                        SIGNAL TYPES                                     */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 7.7.5.5-1. Signal Type                                 */

#define CAI_SIG_TONE  0x00
  /* Tone signal */
#define CAI_SIG_ISDN  0x01
  /* ISDN Alerting */
#define CAI_SIG_IS54B 0x02
  /* IS-54B Alerting */

/* SUPPLEMENTAL CHANNEL REQUEST MESSAGE (see IS-95B Section 6.7.2.3.2.18)  */
#define CAI_MAX_SCRM_BLOB_SIZE  15 /* 4 bit value */

#endif /* CAI_H */
