#ifndef CAI_V_H
#define CAI_V_H
/*===========================================================================

       TIA/EIA-95-B   P R O T O C O L   D A T A    D E C L A R A T I O N S

DESCRIPTION

  This file contains the data structures for messages received and
  transmitted to the base station while the mobile station is operating
  in CDMA mode and other CDMA-related constants and indicators as
  defined in IS-95A.

  Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


===========================================================================*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/cai.h_v   1.108   02 Oct 2002 22:52:12   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/cai_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/14   jh      remove qw.h include
06/14/11   ssh     Added ATIM support to perform the 1X to LTE reselection.
04/29/11   ag      Added 1xAdv fpc modes in comments for cai_fpc_mode_type.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/05/11   jtm     Removed feature guards.
02/07/11   ag      Support for fpc_mode field in RCPM and RCP record of GEM.
12/15/10   ag      SU level CMI clean up.
12/02/10   ag      Removed code under FEATURE_IS95B_MDR.
11/24/10   ag      Renamed pwr_cntl_step_zero_rate to pwr_cntl_step.
11/18/10   jtm     Revert feature removal for CAI_MAX_USABLE_MUX_RS*.
11/15/10   jtm     Initial SU API split for CP.
10/18/10   ag      Renamed Markov, Loopback related counters.
10/11/10   jtm     Added include for modem_1x_defs.h.
09/13/10   ag      Added new MAC counters for 1X Advanced.
07/22/10   ag      Fixed issues in processing radio configration parameters.
07/22/10   ag      Added support for GEM reject cases.
07/19/10   ag      Removed some SO definitions which are there in cai.h also.
07/19/10   ag      Replaced "PACKED <name>" with "PACK (<name>)".
05/21/10   ag      Defined CAI_SO_LOOPBACK_SO75.
12/23/09   jj      Moved few #defines for DTMF from here to cai_v.h.
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support
09/28/09   ag      1X Advanced Signaling Support - Phase 2
07/17/09   jj      Fixed Linker error
07/14/09   jj      CMI changes corresponding to making CAI_AC_ALT_SO_MAX
                   have a single value.
06/29/09   jj      Made changes to remove a SCRM symbol.
06/26/09   ag      1X Adv related changes
06/24/09   pk      Added new CAI fields for MEID_ME and modified ESN_ME
                   Added new information record MEID_ME and support for ESN_ME
                   to be supported for all releases under FEATURE_HWID_SUPPORT
06/01/09   pk      1X Advanced Feature - Modified 1xAdv structures
05/13/09   jj      CMI Phase2 changes.
05/12/09   jj      Moved few #defines to cai.h
03/25/09   pk      1X Advanced Feature - Added new RC definitions and Radio
                   configuration message support
03/19/09   jtm     Lint fixes.
10/21/08   ag      Added a macro to identify whether an SO is data SO or not.
04/08/07   pg      Featurization and general code cleanup for Memory Reduction
                   Effort.
12/14/07   an      Added Passport Mode
12/02/07   bb      Discarding BS ACK (order message) by MC task due to  incorrect paging message length
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

#include "customer.h" /*REMOVE*/
#include "comdef.h"
#include "nv.h"
#include "cai.h"

/*---------------------------------------------------------------------------
 Radio configuration types.
---------------------------------------------------------------------------*/


#define CAI_INIT_RC     ((cai_radio_config_type) 0)
#define CAI_RC_1        ((cai_radio_config_type) 1)
#define CAI_RC_2        ((cai_radio_config_type) 2)
#define CAI_RC_3        ((cai_radio_config_type) 3)
#define CAI_RC_4        ((cai_radio_config_type) 4)
#define CAI_RC_5        ((cai_radio_config_type) 5)
#define CAI_RC_7        ((cai_radio_config_type) 7)
#define CAI_RC_8        ((cai_radio_config_type) 8)
#define CAI_RC_10       ((cai_radio_config_type) 10)
#define CAI_RC_11       ((cai_radio_config_type) 11)
#define CAI_RC_12       ((cai_radio_config_type) 12)

/*----------------------------------------------------------------------
             Rate Set and Traffic Type Constants
----------------------------------------------------------------------*/

/* Transmission rate codes for rate set 1, forward and reverse link */
#define CAI_RS1_9600 0x80  /* 9.6 kbps */
#define CAI_RS1_4800 0x40  /* 4.8 kbps */
#define CAI_RS1_2400 0x20  /* 2.4 kbps */
#define CAI_RS1_1200 0x10  /* 1.2 kbps */
#define CAI_RS1_ALL  0xF0  /* All 4 rates */
#define CAI_RS1_RLP  0xD0  /* All rates without Quarter Rate */

/* Transmission rate codes for rate set 2, forward and reverse link */
#define CAI_RS2_14400 0x80  /* 14.4 kbps */
#define CAI_RS2_7200  0x40  /*  7.2 kbps */
#define CAI_RS2_3600  0x20  /*  3.6 kbps */
#define CAI_RS2_1800  0x10  /*  1.8 kbps */
#define CAI_RS2_ALL   0xF0  /* All 4 rates */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  CAI MESSAGE TYPES                                     */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Sync Channel message */
#define CAI_SYNC_MSG        0x0001

/* Paging Channel messages */

#define CAI_SYS_PARM_MSG      0x01
  /* System Parameters Message */
#define CAI_ACC_PARM_MSG      0x02
  /* Access Parameters Message */
#define CAI_NL_MSG            0x03
  /* Neighbor List Message */
#define CAI_CL_MSG            0x04
  /* CDMA Channel List Message */
#define CAI_SLT_PAGE_MSG      0x05
  /* Slotted Page Message */
#define CAI_PAGE_MSG          0x06
  /* Page Message */
#define CAI_PC_ORD_MSG        0x07
  /* Order Message */
#define CAI_CHN_ASN_MSG       0x08
  /* Channel Assignment Message */
#define CAI_PC_BURST_MSG      0x09
  /* Data Burst Message */
#define CAI_PC_AUTH_MSG       0x0A
  /* Authentication Challenge Message */
#define CAI_PC_SSD_UP_MSG     0x0B
  /* SSD Update Message */
#define CAI_FEATURE_MSG       0x0C
  /* Feature Notification Message */
#define CAI_EXT_SYS_PARM_MSG  0x0D
  /* Extended System Parameters Message */

#define CAI_EXT_NL_MSG        0x0E
  /* Extended Neighbor List Message */

#define CAI_PC_STREQ_MSG      0x0F
  /* Status Request Message */
#define CAI_SERVICE_REDIR_MSG 0x10
  /* Service Redirection Message */
#define CAI_GENERAL_PAGE_MSG  0x11
  /* General Page Message */
#define CAI_GLOBAL_REDIR_MSG  0x12
  /* Global Service Redirection Message */

#define CAI_PC_TMSI_ASGN_MSG  0x13
  /* TMSI Assignment Message */

#define CAI_PACA_MSG          0x14
  /* PACA Message */
#define CAI_EXT_CHN_ASN_MSG   0x15
  /* Extended Channel Assignment Message */
#define CAI_GEN_NL_MSG        0x16
  /* General Neighbor List Message */

#define CAI_EXT_GLB_REDIR_MSG 0x1A
  /* Extended Global Service Redirection Message */
#define CAI_EXT_CDMA_CHN_LIST_MSG 0x1B
  /* Extended CDMA channel list message */

#define CAI_SEC_MODE_CMD_MSG 0x22
 /* Security Mode Command Message */

#define CAI_AUTH_REQ_MSG 0x2C
   /* Authentication Request Message */

#define CAI_MEID_EXT_CHN_ASN_MSG 0X2E
  /* MEID Extended Channel Assignment Message */

/* BCCH Messages */

#define CAI_ANSI41_SYS_PARM_MSG 0x1D
 /* ANSI-41 System parameters Message */
#define CAI_MC_RR_PARM_MSG 0x1E
 /* EMC-RR Parameters Message */
#define CAI_ANSI41_RAND_MSG 0x1F
 /* ANSI-41 RAND Mesage */
#define CAI_EN_AC_PARM_MSG 0x20
 /* Enhanced Access Parameters Message */
#define CAI_UNIV_NL_MSG 0x21
 /* Universal Neighbor List Message */

#define CAI_BCMC_SVC_PARM_MSG 0x2D
   /* Broadcast Multicast (BCMC) Service Parameters Message (BSPM) */

#define CAI_ALT_TECH_INFO_MSG 0x2F
  /* Alternative Technologies Information Message */

/* FCCCH Messages */

#define CAI_UNIVERSAL_PAGE_MSG 0x23
 /* Universal Page Message */
#define CAI_UPM_FIRST_SEG_MSG 0x24
 /* Universal Page First Segment Message */
#define CAI_UPM_MIDDLE_SEG_MSG 0x25
 /* Universal Page Middle Segment Message */
#define CAI_UPM_FINAL_SEG_MSG 0x26
 /* Universal Page Final Segment Message */

/* Messages defined in Release E. Table 3.7.2.3-1 */
#define CAI_PC_GEN_EXTN_MSG 0x3F
 /* Forward common signaling channel General Extension Message */

/* Access Channel Messages  */

#define CAI_REG_MSG           0x01
  /* Registration Message */
#define CAI_AC_ORD_MSG        0x02
  /* Order Message */
#define CAI_AC_BURST_MSG      0x03
  /* Data Burst message */
#define CAI_ORIG_MSG          0x04
  /* Origination Message */
#define CAI_PAGE_RESP_MSG     0x05
  /* Page Response Message */
#define CAI_AC_AUTH_RESP_MSG  0x06
  /* Authentication Challenge Response Message */
#define CAI_AC_STRSP_MSG      0x07
  /* Status Response Message */
#define CAI_AC_TMSI_CMP_MSG   0x08
  /* TMSI Assignment Completion Message */
#define CAI_AC_PACA_CAN_MSG   0x09
  /* PACA Cancel Message */
#define CAI_AC_EXT_STRSP_MSG  0x0A
  /* Extended Status Response Message */

#define CAI_AC_DIM_MSG        0x0D
  /* Device Information Message */
#define CAI_AC_SMRM_MSG       0x0E
  /* Security Mode Request Message */

#define CAI_AC_AUTH_RSP_MSG   0x15
  /* Authenication Response Message */
#define CAI_AC_AUTH_RESYNC_MSG 0x16
  /* Authenication Resynchronization Message */
#define CAI_AC_RECONNECT_MSG  0x17
  /* Reconnect Message */

#define CAI_AC_RADIO_ENV_MSG  0x18
  /* Radio Environment Message (REM) */

/* Forward Traffic Channel Messages (see IS-95A Table 7.7.3.3-1) */

#define CAI_TC_FWD_ORD_MSG    0x01
  /* Order Message */
#define CAI_TC_AUTH_MSG       0x02
  /* Authentication Challenge Message */
#define CAI_ALERT_MSG         0x03
  /* Alert with Information Message */
#define CAI_TC_FWD_BURST_MSG  0x04
  /* Data Burst Message */
#define CAI_HO_DIR_MSG        0x05
  /* Handoff Direction Message */
#define CAI_FM_HO_MSG         0x06
  /* Analog Handoff Direction Message */
#define CAI_TC_SYS_PARM_MSG   0x07
  /* In-Traffic System Parameter Message */
#define CAI_TC_NLU_MSG        0x08
  /* Neighbor List Update Message */
#define CAI_SEND_BURST_MSG    0x09
  /* Send Burst DTMF Message */
#define CAI_PWR_CTL_MSG       0x0A
  /* Power Control Parameters Message */
#define CAI_RET_PARMS_MSG     0x0B
  /* Retrieve Parameters Message */
#define CAI_SET_PARMS_MSG     0x0C
  /* Set Parameters Message */
#define CAI_TC_SSD_UP_MSG     0x0D
  /* SSD Update Message */
#define CAI_FWD_FLASH_MSG     0x0E
  /* Flash with Information Message */
#define CAI_MOB_REG_MSG       0x0F
  /* Mobile Station Registered Message */
#define CAI_TC_STREQ_MSG      0x10
  /* Status Request Message */
#define CAI_EXT_HO_DIR_MSG    0x11
  /* Extended Handoff Direction Message */
#define CAI_FTC_SRV_REQ_MSG   0x12
  /* Service Request Message */
#define CAI_FTC_SRV_RSP_MSG   0x13
  /* Service Response Message */
#define CAI_SRV_CON_MSG       0x14
  /* Service Connect Message */
#define CAI_FTC_SRV_CTL_MSG   0x15
  /* Service Option Control Message */

#define CAI_FTC_TMSI_ASGN_MSG 0x16
  /* TMSI Assignment Message */

#define CAI_FTC_SRV_REDIR_MSG 0x17
  /* Service Redirection Message */
#define CAI_SUP_CHN_ASN_MSG   0x18
  /* Supplemental CHannel Assignment Message */
#define CAI_FTC_PWR_CTRL_MSG  0x19
  /* Power Control Message */
#define CAI_EXT_NLU_MSG       0x1A
  /* Extended Neighbor List Update Message */
#define CAI_CF_SRCH_REQ_MSG   0x1B
  /* Candidate Frequency Search Request Message */
#define CAI_CFS_CTRL_MSG      0x1C
  /* Candidate Frequency Search Control Message */
#define CAI_PUF_MSG           0x1D
  /* Power Up Function Message */
#define CAI_PUF_CMP_MSG       0x1E
  /* Power Up Function Completion Message */

#define CAI_GEN_HO_DIR_MSG    0x1F
  /* General Handoff Direction Message */

#define CAI_RES_ALLOC_MSG     0x20
  /* Resource Allocation Message */
#define CAI_EXT_RELEASE_MSG   0x21
  /* Extended Release Message */

#define CAI_UNIV_HO_DIR_MSG   0X22
  /* Universal Handoff Direction Message */
#define CAI_EXT_SUP_CHN_ASN_MSG   0x23
  /* Extended Supplemental Channel Assignment Message */

 /* New messages defined in Release A */

#define CAI_MOB_ASSIST_BURST_OP_MSG 0x24
  /* Mobile Assisted Burst Operation Parameters Message */
#define CAI_USER_ZONE_REJ_MSG       0x25
  /* User Zone Reject Message */
#define CAI_USER_ZONE_UPDATE_MSG    0x26
  /* Uzer Zone Update Message */
#define CAI_CALL_ASSIGN_MSG         0x27
  /* Call Assignement Message */
#define CAI_EXT_ALERT_W_INFO_MSG    0x28
  /* Extended Alert with Info Message */
#define CAI_EXT_FLASH_W_INFO_MSG    0x2A
  /* Extended Flash with Info Message */
#define CAI_SECURITY_MODE_MSG       0x2B
  /* Security Mode Command Message */

#define CAI_BS_STATUS_RSP_MSG       0x31

#define CAI_TC_AUTH_REQ_MSG   0x32
  /* Authentication Request Message */
#define CAI_RATE_CHANGE_MSG   0x33
  /* Rate Change Message */

#define CAI_TC_BCMC_SVC_PARM_MSG   0x34
  /* In-Traffic Broadcast Multicast (BCMC) Service Parameters Message (ITBSPM) */

#define CAI_MEID_UNIV_HO_DIR_MSG    0x35
  /* MEID Universal Handoff Message */

/* Messages defined in CDMA2000 Release E. Table 3.7.3.3-1 */
#define CAI_RADIO_CONFIG_PARAM_MSG    0x36
 /* Radio Configuration Parameter Message */

#define CAI_TC_ALT_TECH_INFO_MSG 0x38
  /* Alternative Technologies Information Message */ 

#define CAI_FTC_GEN_EXTN_MSG    0xFF
 /* Forward traffic channel General Extension Message */

/* Reverse Traffic Channel Messages (see IS-95A Table 6.7.2.3-1) */

#define CAI_TC_REV_ORD_MSG    0x01
  /* Order Message */
#define CAI_TC_AUTH_RESP_MSG  0x02
  /* Authentication Challenge Response Message */
#define CAI_REV_FLASH_MSG     0x03
  /* Flash with Information Message */
#define CAI_TC_REV_BURST_MSG  0x04
  /* Data Burst Message */
#define CAI_PIL_STR_MSG       0x05
  /* Pilot Strength Measurement Message */
#define CAI_PWR_MSR_MSG       0x06
  /* Power Measurement Report Message */
#define CAI_SEND_DTMF_MSG     0x07
  /* Send Burst DTMF Message */
#define CAI_STATUS_MSG        0x08
  /* Status Message */
#define CAI_ORIG_C_MSG        0x09
  /* Origination Continuation Message */
#define CAI_HO_COMP_MSG       0x0A
  /* Handoff Completion Message */
#define CAI_PARM_RSP_MSG      0x0B
  /* Parameters Response message */
#define CAI_RTC_SRV_REQ_MSG   0x0C
  /* Service Request Message */
#define CAI_RTC_SRV_RSP_MSG   0x0D
  /* Service Response Message */
#define CAI_SRV_CMP_MSG       0x0E
  /* Service Connect Completion Message */
#define CAI_RTC_SRV_CTL_MSG   0x0F
  /* Service Option Control Message */
#define CAI_TC_STRSP_MSG      0x10
  /* Status Response Message */
#define CAI_RTC_TMSI_CMP_MSG  0x11
  /* TMSI Assignment Completion Message */
/* Following are for IS-95B */
#define CAI_SUP_CHAN_REQ_MSG  0x12
  /* Supplemental Channel Request Message */
#define CAI_CFS_RSP_MSG       0x13
  /* Candidate Frequency Search Response Message */
#define CAI_CFS_RPT_MSG       0x14
  /* Candidate Frequency Search Report Message */
#define CAI_PER_PLT_STR_MSG   0x15
  /* Periodic Pilot Strength Measurement Message */
#define CAI_OUTER_RPT_MSG     0x16
  /* Outer Loop Report Message */

#define CAI_RES_REQ_MSG       0x17
  /* Resource Request Message */
#define CAI_EXT_REL_RSP_MSG   0x18
  /* Extended Release Response Message */
#define CAI_RES_REL_REQ_MSG   0x1E
  /* Resource Release Request Message */

/* Following messages are introduced in IS 2000 Release A */
#define CAI_EOM_MSG           0x1A
  /* Enhanced Origination Message */
#define CAI_EFWIM_MSG         0x1B
  /* Extended Flash with Information Message */
#define CAI_EPSMM_MSG         0x1C
  /* Extended Pilot Strength Measurement Message */
#define CAI_EHOCM_MSG         0x1D
  /* Extended Handoff Completion Message */
#define CAI_SMRM_MSG          0x1F
  /* Security Mode Request Message */
#define CAI_UZURM_MSG         0x22
  /* User Zone Update Request Message */
#define CAI_CLCM_MSG          0x23
  /* Call Cancel Message */
#define CAI_DIM_MSG           0x24
  /* Device Information Message */

#define CAI_BS_STATUS_REQ_MSG 0x28

#define CAI_COTRM_MSG 0x29
  /* CDMA Off Time Report Message */

#define CAI_AUTH_RESYNC_MSG 0x2A
  /* Authentication Resynchronization Message */
#define CAI_AUTH_RSP_MSG 0x2B
  /* Authentication Response Message */

#define CAI_ITBSPM_REQ_MSG 0x2C
  /* ITBSPM Request Message (ITBSPMRM) */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                 PAGING CHANNEL ADDRESS TYPES                            */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 7.7.2.3.1-1 Address Types                              */

#define CAI_MIN_ADDR_TYPE   0x0
  /* MIN (MIN1 and MIN2) address field type */
#define CAI_ESN_ADDR_TYPE   0x1
  /* ESN address field type */
#define CAI_IMSI_ADDR_TYPE  0x2
  /* IMSI address field type */
#define CAI_TMSI_ADDR_TYPE  0x3
  /* TMSI address field type */
#define CAI_USE_EXT_ADDR_TYPE   0x4
  /* Use the extended address field type */
#define CAI_BCAST_ADDR_TYPE 0x5
  /* BROADCAST address field type */

#define CAI_MEID_EXT_ADDR_TYPE   0x0
  /* Extended address MEID field type */

#define CAI_USE_TMSI_MASK 0x04
  /* Mask to extract use_tmsi bit from internal stored pref_msid_type.
     Internal stored pref_msid_type is composed of 1-bit field use_tmsi and
     2-bit field pref_msid from ESPM or MCRRPM. */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                      REQUEST_MODE CODES                                 */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 6.7.1.3.2.4-1. REQUEST_MODE Codes                      */

#define CAI_CDMA_ONLY      0x1
  /* CDMA only */
#define CAI_ANALOG_ONLY    0x2
  /* Analog only */
#define CAI_CDMA_OR_ANALOG 0x3
  /* Either CDMA or analog */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                        NUMBER TYPES                                     */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 6.7.1.3.2.4-2. Number Types                            */

#define CAI_UNKNOWN_NUM  0x00
  /* Unknown number type */
#define CAI_INT_NUM      0x01
  /* International number */
#define CAI_NAT_NUM      0x02
  /* National number */
#define CAI_NET_SPEC_NUM 0x03
  /* Network-specific number */
#define CAI_SUB_NUM      0x04
  /* Suscriber number */
#define CAI_ABB_NUM      0x06
  /* Abbreviated number */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                        NUMBER PLANS                                     */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 6.7.1.3.2.4-3. Numbering Plan Identification           */

#define CAI_UNKNOWN_PLAN 0x00
  /* Unknown number plan */
#define CAI_ISDN_PLAN    0x01
  /* ISDN/Telephony numbering plan (CCITT E.164 and CCITT E.163) */
#define CAI_DATA_PLAN    0x03
  /* Data numbering plan (CCITT X.121) */
#define CAI_TELEX_PLAN   0x04
  /* Telex numbering plan (CCITT F.69) */
#define CAI_PRIV_PLAN    0x09
  /* Private numbering plan */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                       SCREENING INDICATORS                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 6.7.4.4-2. Screening Indicators                        */

#define CAI_NOT_SI  0x00
  /* User-provided, not screened */
#define CAI_PASS_SI 0x01
  /* User-provided, verified and passed */
#define CAI_FAIL_SI 0x02
  /* User-provided, verified and failed */
#define CAI_NET_SI  0x03
  /* Network-provided */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                      ALERT PITCHES                                      */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 7.7.5.5-2. Alert Pitch                                 */

#define CAI_PITCH_MED  0x00
  /* Medium pitch (standard alert) */
#define CAI_PITCH_HIGH 0x01
  /* High pitch */
#define CAI_PITCH_LOW  0x02
  /* Low pitch */
#define CAI_PITCH_RSV  0x03
  /* Reserved pitch */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                          TONE SIGNALS                                   */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95A Table 7.7.5.5-3. Tone Signals (SIGNAL_TYPE = '00')           */

#define CAI_TONE_DIAL           0x00
  /* Dial tone on */
#define CAI_TONE_RING           0x01
  /* Ring back tone on */
#define CAI_TONE_INTERCEPT      0x02
  /* Intercept tone on */
#define CAI_TONE_ABBR_INTERCEPT 0x03
  /* Abbreviated intercept: alternating tones repeated for four seconds */
#define CAI_TONE_REORDER        0x04
  /* Network congestion (reorder) tone on */
#define CAI_TONE_ABBR_REORDER   0x05
  /* Abbreviated network congestion (reorder) */
#define CAI_TONE_BUSY           0x06
  /* Busy tone on */
#define CAI_TONE_CONFIRM        0x07
  /* Confirm tone on */
#define CAI_TONE_ANSWER         0x08
  /* Answer tone on */
#define CAI_TONE_CALLWAIT       0x09
/* Call waiting tone on */
#define CAI_TONE_PIP            0x0A
  /* Pip tone on */
#define CAI_TONE_OFF            0x3F
  /* Tones off */

/* See IS-95A Table 7.7.5.5-4. ISDN Alerting (SIGNAL_TYPE = '01')         */

#define CAI_ISDN_NORMAL     0x00
  /* Normal Alerting */
#define CAI_ISDN_INTERGROUP 0x01
  /* Intergroup Alerting */
#define CAI_ISDN_SPECIAL    0x02
  /* Special/Priority Alerting */
#define CAI_ISDN_RESERVED3  0x03
  /* Reserved (ISDN Alerting pattern 3) */
#define CAI_ISDN_PING       0x04
  /* "Ping ring": single burst of 500 ms */
#define CAI_ISDN_RESERVED5  0x05
  /* Reserved (ISDN Alerting pattern 5) */
#define CAI_ISDN_RESERVED6  0x06
  /* Reserved (ISDN Alerting pattern 6) */
#define CAI_ISDN_RESERVED7  0x07
  /* Reserved (ISDN Alerting pattern 7) */
#define CAI_ISDN_OFF        0x0F
  /* Alerting off */

/* See IS-95A Table 7.7.5.5-5. IS-54B Alerting (SIGNAL_TYPE = '10')        */

#define CAI_IS54B_OFF      0x00
  /* No Tone: off */
#define CAI_IS54B_L        0x01
  /* Long: (standard alert) */
#define CAI_IS54B_SS       0x02
  /* Short-short */
#define CAI_IS54B_SSL      0x03
  /* Short-short-long */
#define CAI_IS54B_SS2      0x04
  /* Short-short-2 */
#define CAI_IS54B_SLS      0x05
  /* Short-long-short */
#define CAI_IS54B_SSSS     0x06
  /* Short-short-short-short */
#define CAI_IS54B_PBX_L    0x07
  /* PBX Long */
#define CAI_IS54B_PBX_SS   0x08
  /* PBX Short-short */
#define CAI_IS54B_PBX_SSL  0x09
  /* PBX Short-short-long */
#define CAI_IS54B_PBX_SLS  0x0A
  /* PBX Short-long-short */
#define CAI_IS54B_PBX_SSSS 0x0B
  /* PBX Short-short-short-short */
#define CAI_IS54B_PPPP     0x0C
  /* Pip-Pip-Pip-Pip */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  TRAFFIC CHANNEL RATE SETS                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define CAI_MUX1 0x1    /* 9600 bps */
#define CAI_MUX2 0x2    /* 14400 bps */
#define CAI_MUX3  0x3    /* 9600  bps, 1 supplemental */
#define CAI_MUX4  0x4    /* 14400 bps, 1 supplemental */
#define CAI_MUX5  0x5    /* 9600  bps, 2 supplemental */
#define CAI_MUX6  0x6    /* 14400 bps, 2 supplemental */
#define CAI_MUX7  0x7    /* 9600  bps, 3 supplemental */
#define CAI_MUX8  0x8    /* 14400 bps, 3 supplemental */
#define CAI_MUX9  0x9    /* 9600  bps, 4 supplemental */
#define CAI_MUX10 0xa    /* 14400 bps, 4 supplemental */
#define CAI_MUX11 0xb    /* 9600  bps, 5 supplemental */
#define CAI_MUX12 0xc    /* 14400 bps, 5 supplemental */
#define CAI_MUX13 0xd    /* 9600  bps, 6 supplemental */
#define CAI_MUX14 0xe    /* 14400 bps, 6 supplemental */
#define CAI_MUX15 0xf    /* 9600  bps, 7 supplemental */
#define CAI_MUX16 0x10   /* 14400 bps, 7 supplemental */

#define CAI_MAX_NUM_SUP_RS1 7   /* Absolute Max num of sup in RS1 */
#define CAI_MAX_NUM_SUP_RS2 7   /* Absolute Max num of sup in RS2 */

#define CAI_MAX_USABLE_MUX_RS1 CAI_MUX1
#define CAI_MAX_USABLE_MUX_RS2 CAI_MUX2

#define CAI_MUX_1X_RS1                   0x03   /* 1X, MUX PDU 1           */

#define CAI_MUX_2X_RS1_SINGLE            0x809  /* 2X, single size pdu     */
#define CAI_MUX_2X_RS1_DOUBLE            0x905  /* 2X, double size pdu     */

#define CAI_MUX_4X_RS1_SINGLE            0x811  /* 4X, single size pdu     */
#define CAI_MUX_4X_RS1_DOUBLE            0x909  /* 4X, double size pdu     */

#define CAI_MUX_8X_RS1_SINGLE            0x821  /* 8X, single size pdu     */
#define CAI_MUX_8X_RS1_DOUBLE            0x911  /* 8x, double size pdu     */

#define CAI_MUX_16X_RS1_DOUBLE           0x921  /* 16x, double size  only  */

#define CAI_MUX_1X_RS2                   0x04   /* 1X, MUX PDU 2           */

#define CAI_MUX_2X_RS2_SINGLE            0x80A  /* 2X, single size pdu     */
#define CAI_MUX_2X_RS2_DOUBLE            0x906  /* 2X, double size pdu     */

#define CAI_MUX_4X_RS2_SINGLE            0x812  /* 4X, single size pdu     */
#define CAI_MUX_4X_RS2_DOUBLE            0x90A  /* 4X, double size pdu     */

#define CAI_MUX_8X_RS2_SINGLE            0x822  /* 8X, single size pdu     */
#define CAI_MUX_8X_RS2_DOUBLE            0x912  /* 8x, double size pdu     */

#define CAI_MUX_16X_RS2_DOUBLE           0x922  /* 16x, double size  only  */

#define CAI_MUX_VARIABLE                 0xf20  /* 1X-32x, var size pdu    */
#define CAI_MUX_0XF00                    0xf00  /* For IS-2000-C F-PDCH    */
#define CAI_MUX_0X1301                   0x1301 /* For IS-2000-D R-PDCH    */
#define CAI_MUX_0X1305                   0x1305 /* For IS-2000-D R-PDCH    */


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  1X ADVANCED DEFINITIONS                                */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define CAI_RC_PARM_MAX CAI_N6M
  /* Maximum number of RC parameter records. This is limited by
   * the number of pilot in ASET during traffic state. */

/*-------------------------------------------------------------------------*/
/* N2M Indicator definitions                                               */
/* Table 3.7.3.3.2.51-3. N2m value for Forward Link RC11 and RC12          */
/*-------------------------------------------------------------------------*/
typedef enum
{
  CAI_N2M_2_BAD_FRAMES    = 0,
  CAI_N2M_4_BAD_FRAMES    = 1,
  CAI_N2M_6_BAD_FRAMES    = 2,
  CAI_N2M_8_BAD_FRAMES    = 3
} cai_for_n2m_ind_type;

/*-------------------------------------------------------------------------*/
/* FPC_MODE definitions                                                    */
/*-------------------------------------------------------------------------*/
typedef enum
{
  CAI_FPC_MODE_800_PRI         = 0, /* Also 400Hz with RC11 */
  CAI_FPC_MODE_400_PRI_400_SEC = 1,
  CAI_FPC_MODE_200_PRI_600_SEC = 2, /* Also 200Hz/200Hz with RC 11 */
  CAI_FPC_MODE_50_PRI          = 3, /* Also 200Hz with RC 11 */
  CAI_FPC_MODE_QIB_50_PRI      = 4,
  CAI_FPC_MODE_QIB_50_PRI_EIB_50_SEC  = 5,
  CAI_FPC_MODE_400_PRI_EIB_50_SEC  = 6,
  CAI_FPC_MODE_RESERVED=0xff
} cai_fpc_mode_type;

/*-------------------------------------------------------------------------*/
/* Power Control Mode                                                      */
/*-------------------------------------------------------------------------*/
typedef enum
{
  CAI_PWR_CNTL_200_OR_400_HZ  = 0,
  CAI_PWR_CNTL_200_HZ         = 1
} cai_rcpm_pwr_cntl_mode_type;

/*-------------------------------------------------------------------------*/
/* CLosed Loop Power control step size    (Table 3.7.3.3.2.25-1)           */
/*-------------------------------------------------------------------------*/
typedef enum
{
  CAI_PWR_CNTL_STEP_1_DB    = 0,
  CAI_PWR_CNTL_STEP_0_5_DB  = 1,
  CAI_PWR_CNTL_STEP_0_25_DB = 2,
  CAI_PWR_CNTL_STEP_1_5_DB  = 3,
  CAI_PWR_CNTL_STEP_2_0_DB  = 4
} cai_pwr_cntl_step_type;

/*-------------------------------------------------------------------------*/
/* N (Duty Cycle) value for RC11, RC12 and RC8                             */
/* Table 3.7.3.3.2.51-5. N (duty cycle) value for FL RC11,RC12 and RL RC08 */
/*-------------------------------------------------------------------------*/
typedef enum
{
  CAI_FCH_BLANKING_DISABLED  = 0,
  CAI_FCH_N_1_OF_4_FRAMES    = 1,
  CAI_FCH_N_1_OF_8_FRAMES    = 2
} cai_fch_blanking_dutycycle_type;

/* Radio Configuration Parameters */
typedef struct
{
  byte qof_set_in_use;                /* QOF Set in use */

  byte rev_sch0_ack_ch_walsh;         /* Rev SCH0 ACK Chn Walsh Code */
  byte rev_sch0_ack_bit;              /* Rev SCH0 ACK Bit */

  byte rev_sch1_ack_ch_walsh;         /* Rev SCH1 ACK Chn Walsh Code */
  byte rev_sch1_ack_bit;              /* Rev SCH1 ACK Bit */

  boolean for_fch_ccsh_interleaver_type;  /* Interleaver type used for Fwd FCH RC 12 */
} cai_rc_pilot_params_type;

/* MC - SRCH 1XADV Parameters Interface */
typedef enum
{
  RCP_PILOT_UNKNOWN,
  RCP_PILOT_INVALID,
  RCP_PILOT_ACTIVE
} cai_rcp_pilot_state;

typedef struct
{
  byte num_rc_params_rec;               /* Number of RC Parameters records */
  struct
  {
    word pilot_pn;                      /* Pilot PN */
    cai_rc_pilot_params_type rc_pilot_params; /* Parameters specific to a pilot */
  } rc_params_recs[CAI_RC_PARM_MAX];
  /* RC parameter records */
} cai_rcp_srch_type;

/* Structure used to pass 1XADV parameters to RXC/TXC */
typedef struct
{
  word for_fch_ack_mask_rl_blanking;
  /* Fwd FCH ACK Mask for RL Blanking */

  word for_fch_ack_mask_no_rl_blanking;
  /* Fwd FCH ACK Mask for No RL Blanking */

  word rev_fch_ack_mask;
  /* Rev FCH ACK Mask */

  word for_sch_ack_mask_rl_blanking;
  /* Fwd SCH ACK Mask for RL Blanking */

  word for_sch_ack_mask_no_rl_blanking;
  /* Fwd SCH ACK Mask for No RL Blanking */

  word rev_sch_ack_mask;
  /* Rev SCH ACK Mask */

  cai_for_n2m_ind_type for_n2m_ind;
  /* Fwd N2M Indicator */

  struct
  {
    boolean           incl;
    /* fpc_mode included indicator */
    cai_fpc_mode_type value;
    /* Forward Power Control Operation Mode Indicator */
  } fpc_mode;

  cai_rcpm_pwr_cntl_mode_type power_control_mode;
  /* Power Control Mode */

  cai_pwr_cntl_step_type power_control_step;
  /* Power control step size during RL Blanking */

  cai_fch_blanking_dutycycle_type for_fch_blanking_dutycycle;
  /* Duty cycle value for Fwd Link */

  cai_fch_blanking_dutycycle_type rev_fch_blanking_dutycycle;
  /* Duty cycle value for Rev Link */

  byte rev_gain_adj_acs1;
  /* Chn adj gain for Rev Ack Chn (No Cells = 1) */

  byte rev_gain_adj_acs2plus;
  /* Chn adj gain for Rev Ack Chn (No Cells > 1)*/

} cai_rcp_rxc_txc_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*             SERVICE NEGOTIATION TYPES                                   */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define CAI_SERV_OPT_NEG 0
  /* Indicates service option negotiation. */

#define CAI_SERV_NEG     1
  /* Indicates service negotiation. */



/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  SERVICE OPTION GROUPS                                  */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define CAI_SO_GROUP_VOICE                0x00
#define CAI_SO_GROUP_LOW_SPEED_ASYNC_DATA 0x01
#define CAI_SO_GROUP_DIGITAL_FACSIMILE    0x02
#define CAI_SO_GROUP_ANALOG_FACSIMILE     0x03
#define CAI_SO_GROUP_NON_CDPD_PACKET_DATA 0x04
#define CAI_SO_GROUP_CDPD_PACKET_DATA     0x05
#define CAI_SO_GROUP_SMS                  0x06
#define CAI_SO_GROUP_OTAPA                0x07
#define CAI_SO_GROUP_LOCATION             0x08
#define CAI_SO_GROUP_ENHANCED_VOICE       0x09

/* Making the group value 0x0A because CAI_SO_GROUP_ENHANCED_VOICE is 0x09 */
#define CAI_SO_GROUP_PROPRIETARY_8026     0x0A
#define CAI_SO_GROUP_UNASSIGNED           0xFF


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   - */
/*                  SERVICE OPTION GROUP BITMAPS                            */
/*  Those are only used when SO Grouping is used when reporting alternative */
/*  Service Options                                                         */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   - */
#define CAI_SO_BITMAP_SO_VOICE_IS96A                0x01
#define CAI_SO_BITMAP_SO_VOICE_EVRC                 0x02
#define CAI_SO_BITMAP_SO_VOICE_13K_IS733            0x04
#define CAI_SO_BITMAP_SO_VOICE_SMV                  0x08
#define CAI_SO_BITMAP_SO_VOICE_4GV_NB               0x10
#define CAI_SO_BITMAP_SO_VOICE_4GV_WB               0x80

#define CAI_SO_BITMAP_SO_ASYNC_DATA_PRE707          0x01
#define CAI_SO_BITMAP_SO_ASYNC_DATA_13K_PRE707      0x02
#define CAI_SO_BITMAP_SO_ASYNC_DATA_IS707           0x04

#define CAI_SO_BITMAP_SO_G3_FAX_PRE707              0x01
#define CAI_SO_BITMAP_SO_G3_FAX_13K_PRE707          0x02
#define CAI_SO_BITMAP_SO_G3_FAX_IS707               0x04

#define CAI_SO_BITMAP_SO_ANALOG_FAX_RS1             0x01
#define CAI_SO_BITMAP_SO_ANALOG_FAX_RS2             0x02

#define CAI_SO_BITMAP_SO_PPP_PKT_DATA_PRE707        0x01
#define CAI_SO_BITMAP_SO_PPP_PKT_DATA_13K_PRE707    0x02
#define CAI_SO_BITMAP_SO_PPP_PKT_DATA_IS707         0x04
#define CAI_SO_BITMAP_SO_MDR_PKT_DATA_FRS1_RRS1     0x08
#define CAI_SO_BITMAP_SO_MDR_PKT_DATA_FRS1_RRS2     0x10
#define CAI_SO_BITMAP_SO_MDR_PKT_DATA_FRS2_RRS1     0x20
#define CAI_SO_BITMAP_SO_MDR_PKT_DATA_FRS2_RRS2     0x40
#define CAI_SO_BITMAP_SO_PPP_PKT_DATA_3G            0x80

#define CAI_SO_BITMAP_SO_SMS                        0x01
#define CAI_SO_BITMAP_SO_RS2_SMS                    0x02

#define CAI_SO_BITMAP_SO_RS1_OTAPA                  0x01
#define CAI_SO_BITMAP_SO_RS2_OTAPA                  0x02

#define CAI_SO_BITMAP_SO_RS1_PD                     0x01
#define CAI_SO_BITMAP_SO_RS2_PD                     0x02

#define CAI_SO_BITMAP_SO_PROPRIETARY_8026           0x01

#define CAI_SO_BITMAP_SO_ENHANCED_VOICE_EVRC_NW     0x01

#define CAI_SO_BITMAP_POSITION_UNASSIGNED           0xFF
/* SO bitmap specific size fields */

#define SO_BITMAP_SIZE_0    0x00
#define SO_BITMAP_SIZE_4    0x01
#define SO_BITMAP_SIZE_8    0x02

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  SERVICE OPTIONS                                        */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define CAI_SO_LOOPBACK_SO75        0x004B
  /* Enhanced Loopback Service Option */
  /* See 3GPP2 C.S0013-B */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                         BAND CLASSES                                    */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95-A TSB                                                         */

/* The number of band classes we currently support. */
#define CAI_MAX_BAND_CLASSES 16

#define CAI_BAND_CLASS0    0    /* Cellular 800  */
#define CAI_BAND_CLASS1    1    /* PCS 1900      */
#define CAI_BAND_CLASS2    2    /* TACS          */
#define CAI_BAND_CLASS3    3    /* JTACS         */
#define CAI_BAND_CLASS4    4    /* Korean PCS    */
#define CAI_BAND_CLASS5    5    /* 450 MHz       */
#define CAI_BAND_CLASS6    6    /* 2 GHz         */
#define CAI_BAND_CLASS7    7    /* 700 MHz       */
#define CAI_BAND_CLASS8    8    /* 1800 MHz      */
#define CAI_BAND_CLASS9    9    /* 900 MHz       */
#define CAI_BAND_CLASS10   10   /* Secondary 800 */
#define CAI_BAND_CLASS11   11   /* 400 MHz PAMR  */
#define CAI_BAND_CLASS12   12   /* 800 MHz PAMR  */
#define CAI_BAND_CLASS13   13   /* 2.5 GHz IMT-2000 Extensions */
#define CAI_BAND_CLASS14   14   /* US PCS 1.9 GHz */
#define CAI_BAND_CLASS15   15   /* AWS */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*         BASE STATION STATUS INFORMATION RECORD TYPE                     */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-2000 Rel A Table 2.7.2.3.2.39-1 and 2.7.2.3.2.47-1               */

#define CAI_BSS_PILOT_INFO_REQ  0x00    /* Pilot Information Request    */
#define CAI_BSS_PILOT_INFO_RSP  0x00    /* Pilot Information Response   */

/* See IS-2000 Rel A, Base Station Status Response Message 3.7.3.3.2.47    */
/* and Base Station Request Message. 2.7.2.3.2.39.                         */
#define CAI_BSS_NUM_RECORDS    0x0F
#define CAI_BSS_NUM_PILOTS     0x0F

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  RETURN CAUSE CODES                                     */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-95B Table 6.7.1.3.2.1-2. RETURN CAUSE codes                      */
#define CAI_NORMAL_ACCESS_RETURN       0x0
  /* Normal access */
#define CAI_SYSTEM_NOT_ACQ_RETURN      0x1
  /* System not acquired */
#define CAI_PROTOCOL_MISMATCH_RETURN   0x2
  /* Protocol mismatch encountered */
#define CAI_REG_REJ_RETURN             0x3
  /* Registration rejection return */
#define CAI_WRONG_SID_RETURN           0x4
  /* Wrong SID return */
#define CAI_WRONG_NID_RETURN           0x5
  /* Wrong NID return */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*          SETTABLE AND RETRIEVABLE PARAMETERS                            */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* The following parameters are from IS-95A Table E-1.                     */

#define CAI_MUX_WILDCARD 0
 /* Mux Option to indicate a wildcard MUX option that
    is internal to Call Processing.  This is used to
    indicate that SNM should allow any MUX option for
    this service option.  */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*              DATA BURST MESSAGE BURST TYPE ASSIGNMENTS                  */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See TSB58 Table 4.1-1 Standard Data Burst Msg Burst Type Assignments    */

#define CAI_UNKNOWN_BURST_TYPE  0x00
  /* Unknown burst type */
#define CAI_ASYNC_DATA_SERVICES 0x01
  /* Asynchronous Data Services */
#define CAI_GROUP_3_FACSIMILE   0x02
  /* Group-3 Facsimile */
#define CAI_SHORT_MSG_SERVICES  0x03
  /* Short Message Services */
#define CAI_OTA_ACTIVATION      0x04
  /* Over-the-Air Activation */
#define CAI_POSITION_DET        0x05
  /* Position Determination Services */
#define CAI_SHORT_DATA_BURST    0x06
  /* Short Data Burst */
#define CAI_EXTENDED_BURST_TYPE_INTL 0x3E
  /* Extended Burst Type - International */
#define CAI_EXTENDED_BURST_TYPE 0x3F
  /* Extended Burst Type */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Generic Access Channel Message Header */

/* Access Channel Mobile Station Identifier Fields (MSID_TYPE = '010')      */
typedef PACK (struct)
{
  byte imsi_class[1];         /* IMSI Class */
} cai_msid2_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* IMSI Address types, used in both Paging and Access channel headers      */

typedef PACK (struct)
{
  byte imsi_class_0_type[2];  /* IMSI class 0 type */
} cai_imsi_class0_type;

/* IMSI Class 0 Type specific subfields (Class Type = 3) */
typedef PACK (struct)
{
  byte reserved[2];       /* Reserved bits */
  byte mcc[10];           /* Mobile country code */
  byte imsi_11_12[7];     /* The 11th and 12th digits of IMSI */
  byte imsi_s[34];        /* Last ten digits of the IMSI */
} cai_imsi_class0_type3_type;

/* Calling Party Number record (see IS-95A Section 7.7.5.3 Calling Party
   Number) */

typedef enum
{
  CAI_PI_PRESENTATION_ALLOWED = 0,
  CAI_PI_PRESENTATION_RESTRICTED,
  CAI_PI_NUMBER_NOT_AVAILABLE,
  CAI_PI_RESERVED
} cai_call_party_num_pi_type;

typedef enum
{
  CAI_SI_USER_NOT_SCREENED = 0,
  CAI_SI_USER_VERIFIED_PASSED,
  CAI_SI_USER_VERIFIED_FAILED,
  CAI_SI_NETWORK
} cai_call_party_num_si_type;

/* Some enum definition used in subaddress */
typedef enum
{
  CAI_SUBADDR_TYPE_NSAP = 0,
  CAI_SUBADDR_TYPE_USER
} cai_subaddress_types_type;

typedef enum
{
  CAI_SUBADDR_EVEN = 0,
  CAI_SUBADDR_ODD
} cai_subaddress_eo_type;

/* Maximum number of enhanced broadcast record in message */
#define CAI_UNIV_PAGE_NUM_BCAST_REC_MAX 32

/* Number of interleaved address portion */
#define CAI_UNIV_PAGE_INTERLEAVED_ADDR_PORTION 16

/* Maximum number of SO connections (including SVD) */
#define CAI_MAX_NUM_CONNECTIONS 3


/* Definition of CPCCH PC Rate. Table 3.7.2.3.2.33-4 */
typedef enum
{
  CAI_CPCCH_RATE_200 = 0,
  CAI_CPCCH_RATE_400 = 1,
  CAI_CPCCH_RATE_800 = 2
} cai_cpcch_rate_enum_type;

#define CAI_OUTER_RPT_MAX_SUP         2

#define CAI_LAST_PARM_ID      866 /* CAI_ACK_RX_PCG_15_SCH_ID */

#define CAI_XTRA_LAST_PARM_ID 17  /* CAI_XTRA_SCH0_FWD_SAVED_MUXPDU5_LTUS_ID */

typedef enum
{
  CAI_PILOT_REC_1X_COM_TD =    0x00,
  CAI_PILOT_REC_1X_AUX =       0x01,
  CAI_PILOT_REC_1X_AUX_TD =    0x02,
  CAI_PILOT_REC_3X_COM =       0x03,
  CAI_PILOT_REC_3X_AUX =       0x04,
  CAI_PILOT_1X_COMMON =        0xff
  /* Not part of the std, Added for internal use*/
} cai_pilot_rec_type;

#endif /*CAI_V_H*/
