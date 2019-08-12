#ifndef CAII_I_H
#define CAII_I_H
/*===========================================================================

         C D M A   I N T E R N A L   D A T A   S T R U C T U R E S

DESCRIPTION
  This file contains data structures and constants which represent
  the messages transmitted between the mobile and base stations while
  operating in the CDMA mode. This format is only used internally to
  the Main Control task.

  Copyright (c) 1990 - 2015 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/caii.h_v   1.78   30 Sep 2002 17:45:02   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/caii_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/15   ppr     Feature clean up changes
06/09/14   agh     FR 17879: Added support for extended EARFCN
03/18/14   agh     FR 16951: Avoiding BSR from C2K->LTE when UE supports 
                   C2K-LTE Cell Reselection
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
05/06/13   ssh     Removed Pack keyword from ATIM OTA structures.
12/28/12   gga     Klocwork Reviews
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
05/09/12   ppr     Feature Cleanup
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/19/12   ppr     Reverting feature cleanup changes for
                   FEATURE_IS2000_REL_A_BSID
12/29/11   ssh     Added ATIM cycle support.
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
08/29/11   cjb     Renamed FEATURE_MODEM_1X_IRAT_1XTOL to FEATURE_1X_TO_LTE.
06/14/11   ssh     Added ATIM support to perform the 1X to LTE reselection.
04/29/11   ag      Support for SPM field rc_bitmap_capability_ext_allowed.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/07/11   ag      Renamed power_control_mode_01_supported to
                   rpc_mode_01_supported.
02/07/11   ag      Support for fpc_mode field in RCPM and RCP record of GEM.
01/04/11   ag      Fixed compiler and lint warnings.
12/15/10   ag      SU level CMI clean up.
11/24/10   ag      Renamed pwr_cntl_step_zero_rate to pwr_cntl_step.
11/18/10   ag      Added support for optional SO73 COP0.
11/15/10   jtm     Initial SU API split for CP.
10/13/10   ag      Included header file modem_1x_defs.h.
07/19/10   ag      Replaced "PACKED <name>" with "PACK (<name>)".
09/28/09   ag      1X Advanced Signaling Support - Phase 2
07/01/09   ag      Modified structure caii_radio_config_type
06/24/09   pk      Added new information record MEID_ME and support for ESN_ME
                   to be supported for all releases under FEATURE_HWID_SUPPORT
06/01/09   pk      1X Advanced Feature - Modified TXC RXC structures
03/31/09   adw     Added caii.h to eliminate implicit includes.
03/26/09   pk      1X Advanced Feature - Added Radio Configuration Parameters
                   Message
12/14/07   an      Added Passport Mode
12/02/07   bb      Discarding BS ACK (order message) by MC task due to
                   incorrect paging message length
08/31/06   an      Rel D Addendum Sync Id Feature.
08/23/06   fc      Removed redundant field msid from caii_ac_ext_strsp_type.
08/16/06   pg      Added support for release D Packet Zone Hysteresis.
08/07/06   fc      Fixed discrepanies for parser testing.
07/25/06   fc      Fixed discrepanies for parser testing.
06/12/06   an      Modified the EXT_TERM_INFO to return as many SO's as possible
04/22/06   fc      Fixed Rel D featurization.
04/18/06   pg      Added support for Extended SO Info Record.
02/16/06   fc      Fixed parser discrepancies.
01/19/06   fc      Merged the following :
           fh      Added support for AKA and MI.
12/08/05   fc      Featurize gen_cmeakey under FEATURE_IS2000_REL_D.
10/19/05   sb      Added support for Band Class 14, Band Subclass I, J, K.
09/21/05   an      Added support for published Release D Addendum new Info Recs
09/19/05   pg      Added support for published Release D Addendum f-dsch and
                   r-dsch message changes.
09/15/05   fc      Added support for published Release D Addendum f-csh
                   message changes.
08/19/05   sb      Miscellaneous fixes for info recs.
08/04/05   pg      Added support for Rel D Addendum f-dsch and r-dsch messages.
08/03/05   fc      Fixed lint errors.
08/01/05   an      Added support for Rel D Addendum r-csh messages, IRs and
                   Orders.
07/25/05   fc      Added support for Rel D Addendum f-csh messages.
07/12/05   ht      Added MEID Addressing support
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
01/14/05   sb      Added a comment.
12/08/04   va      Merged code from REL_B_C archive
11/02/04   va        Made add and drop intercepts in caii_ho_srch_type to be int8
09/27/04   va      Merged code from REL B/C archive
09/16/04   sb       Added Rel C Addendum fields to BS Status Reponse Msg.
08/20/04   yll      Clean up MC-SRCH interface.
08/12/04   pg       Added Enhanced Multi-character Extended Record.
07/29/04   fc       Renamed bcch_index to bcn.
07/05/04   sb       Added Extended Keypad Facility info rec.
06/16/04   fc       Rel C addendum changes for GNLM, UNLM and ECAM.
09/22/04   az      Removed _incl fields from ECAM AM0 and AM4
09/16/04   sb      Added Rel C Addendum fields to BS Status Reponse Msg.
09/15/04   va      Put ERM array sizes under #defines
09/01/04   bkm     Fixed type for ack_delay in caii_for_pdch_spec_fields_type.
08/19/04   az      ECAM parser changes. AM5
08/18/04   fc      Removed silk incl flags on overhead messages.
08/13/04   fc      Resolved silk discrepancies on overhead messages.
08/13/04   az      More changes for ECAM. Works fine with just one record.
08/06/04   az      Removed temporary fix for meid, Some more ECAM changes
08/02/04   ht      Added Rel D MEID LAC support
08/02/04   az      Removed featurization around rel_d_incl flag in ECAM
07/28/04   pg      Added Enhanced Multi-character Extended Record.
07/27/04   az      Changed from word to byte for num_pilots in
                   Assign mode 1. ECAM
07/12/04   az      Temporary fix for meid changes
07/09/04   sb      Added support for Release D orders.
07/07/04   sb      More Release D updates to info recs.
07/06/04   sb      Made Release D updates to the info recs.
06/29/04   pg      Replaced caii_rl_cqich_rec_type in caii_rate_change_type
                   with caii_rpc_adj3_type.
06/17/04   fc      Rel C addendum changes on GNLM, UNLM and ECAM.
06/17/04   bkm     Put BCMCS msgs under Rel D subfeature.
06/16/04   va      Rel C addendum changes for Registration msg
06/14/04   fc      Release C addendum changes on common channel messages.
06/11/04   sb      Release C Addendum changes for: ERM, RATCHGM, ITSPM.
06/09/04   sb      Release C Addendum changes for UHDM.
06/08/04   fc      Changes on BSPM.
06/03/04   fc      Added support for Release D common channel messages.
05/19/04   az      Added support for ECAM Rel D parsing
05/14/04   bkm     Update for PLT.
05/14/04   va      Support for REL D access chan & rev TC messages
05/12/04   sb      Use CAI_MAX_NUM_FOR_PDCCH instead of CAI_HO_MSG_MAX_NUM_PDCCH.
05/06/04   pg      Added Release D fields for UHDM.
04/23/04   az      Added Release D fields for
                   Service Connect Message
                   Service Configuration Record
                   Non-Negotiable Service Configuration record
04/19/04   bkm     Added Release D fields for RATCHGM and ITBSPM.
04/16/04   bkm     Added Release D fields for CLAM, ERM, ESCAM and BSSRSPM.
                   Use struct in PCNM for pz_hyst* fields.
04/13/04   bkm     Added Release D fields for ITSPM and Power Control Msg.
04/06/04   bkm     Added some initial Rel D types.
01/06/04   sb      Added Release C Order fields.
12/09/03   sb      Added Release C fields of UHDM and GHDM.
11/14/03   sb      Added Release C fields of Power Control Message.
11/11/03   sb      Added Release C fields of FTC Security Mode Command Msg.
11/10/03   sb      Added Release C fields of In-Traffic System Parameters Msg.
11/06/03   fc      Changed the data size of fch_dcch_mux_option_ind.
11/04/03   sb      Added Release C fields of Service Connect Message.
10/30/03   va      Fixed some compiler errors when REL C is turned on
10/24/03   fc      Fixed discrepancies on MC Extended Display Info Rec.
10/21/03   va      Mixed misc parser bugs
10/14/03   fc      Fixed info recs.
10/06/03   fc      Changes for service configuration.
10/03/03   sb      Added Authentication Request Message on FTC.
09/25/03   fc      Fixed record size discrepancy for ECAM.
09/22/03   va      Removed PACKED contruct for caii_for_pdch_specific_fields
                   _type
09/19/03   fc      Changes for ECAM.
09/18/03   sb      Added Rate Change Msg, Rel C fields of ERM and CLAM.
09/17/03   fc      Changes for authentication request message.
09/17/03   va      Fixed parser bugs
09/16/03   fc      Changes for system parameter message.
                   Changes for security mode command message.
09/08/03   va      Changed name to for_pdch_fields in Orig & page
                   Response caii structs
09/04/03   fc      Added support for UIM_ID and ESN_ME info recs.
08/08/03   fc      Added support for IS2000 Release C messages.
06/18/03   fc      Added support for the decoupling of FCH and DCCH MUX.
04/09/03   sb      Changed EPSMM to use CAI_EPSMM_MAX_NUM_PILOT.
03/31/03   sb      Moved cadence_type to fixed part of Para Alert info rec.
03/27/03   ph      Synchronized caii_ftc_srv_redir_type with SILK structure
                   to enable parser testing of the message.
03/21/03   fc      Added support for PCH BI.
03/11/03   bkm     Added caii_rec_hdr_type to T53 information records.
02/27/03   sb      Removed unnecessary is2000_rel_a_incl from Sync Channel Message.
02/27/03   sb      Added Release B changes.
02/05/03   fc      Added Release B f-csh changes.
02/05/03   va      Added Release B changes.
02/05/03   fc      Cleanup for f-csh forward compatability.
12/12/02   sb      Added reserved field to MC-RR.
11/07/02   az      Removed FEATURE_IS2000_REV0_PREADDENDUM
11/07/02   az      Mainlined FEATURE_IS95B
09/30/02   az      Mainlined FEATURE_IS2000 and FEATURE_IS2000_2_1_ADDENDUM
09/02/02   fc      Changes based on UPM code review.
08/28/08   fc      Updated comment.
08/08/02   fc      Changes to use CAI_FCCCH_BODY_SIZE in
                   caii_univ_page_block_type and caii_univ_page_seg_type.
07/29/02   fc      Added first_page_rec_offset to UPM structures.
06/27/02   fc      Changed caii_univ_page_interleaved_addr_type.
06/25/02   az      Changed the name of the union for IMSI type specific
                   subfields from class to class_type.
06/18/02   yll     Added support for Base Station Status Req/Rsp Message.
06/10/02   fc      Deleted num_page_recs from caii_univ_page_type.
05/28/02   fc      Changes to match UPM interleaved address structure with
                   message pack unit test.
05/22/02   fc      Changes for message pack unit test.
05/20/02   fc      Added support for Universal Page Message.
04/09/02   yll     Added Broadcast SMS support on IS2000 RelA common channel.
03/24/02   sb      Include ba_parms_len and ra_parms_len in caii_en_ac_parm_type.
03/08/02   sb      Updated CAM, ESP, ECAM, Ansi-41, MC-RR, EAPM and SMCM to be in
                   sync with Addendum 2 published version.
03/05/02   lcc     Corrected add_intercept and drop_intercept in caii_ho_srch_type
                   to be signed char instead of unsigned.
02/22/02   sb      Renamed mode_parm to mode_parms.
02/07/02   lh      Changed some info rec in RTC msg from byte arry to dword array.
01/24/02   ph      Fixed QOS record structure
10/15/01   kk      Fixed compiler errors when Release A is not defined.
10/12/01   sb      Removed probe_pn_ran from EAPM, fixed spelling error
10/03/01   ph      fixed dword-alignment problem in ac_ext_strsp and ac_dim
10/02/01   ph      Merged in from CP VU 96
                   08/21/01   sj      Changed rs1_20ms_incl -> rc3_rc5_20ms_incl(Power Control
                                      Msg), rs2_20ms_incl ->rc4_rc6_20ms_incl(Power Control Msg)
                                      & num_visits -> max_num_visits(Candidate Frequency Search
                                      Response message).
09/28/01   kk      Added missing position location indicator to Flash msg.
09/10/01   sb      Moved definitions to cai.h
09/06/01   sb      Removed length fields from caii_en_ac_parm_type.
08/24/01   ph      fixed internal struct of FTC security mode cmd msg in line with standard.
08/16/01   ph      Using CAI_SYNC_ID_LEN_MAX for the f-csch
08/16/01   ph      Rel A Addendum 2 changes for the FTC
08/16/01   lh      Addendum 2 Changes for F-CSCH and R-CSCH.
08/14/01   kk      Addendum 2 changes on RTC.
06/26/01   kk      Added missing field in EOM.
06/26/01   kk      Merge from main archive.
  06/15/01 lcc     Added FEATURE_IS2000 around align_timing in CFSCM.
  06/14/01 lcc     Corrected Candidate Frequency Search Control Message and
                   Channel Configuration Capability Information Record to be
                   Rel. 0 Addendum 2 compliant.

05/16/01   lh      Merged following changes from common archive.
  05/02/01   lh      Fixed possible mis-alignment for FWD and REV info records.
  04/10/01   lcc/jq/ Added definitions related to DCCH and contorl hold support.
           bc
04/26/01   lh      Clean up Band Class Info Rec.
04/18/01   lh      Removed UNIT_TEST ifdef for ltu_table_support.
04/18/01   ph      Moved "cadence_type" to match for SILK testing.
04/17/01   lh      Separate AC Status Rsp and Ext Status Rsp data structure.
04/12/01   ph/kk   Support for new power control fields, other silk test fixes.
04/09/01   lh      Added LTU_TABLE_SUPPORTED bit.
04/09/01   ph      FTC uhdm/ghdm fixes
03/14/01   kk      Reverse Traffic fixes
03/13/01   ph      Fixes related to FTC msgs
03/12/01   lh      UNLM optimization.
03/07/01   lh      Release A PCH/ACH bug fixes.
03/07/01   ph      removed "total_rec_len" from ext. status rsp. msg
                   moving forward with the new method of computing msg length
                   in REL A.
03/06/01   kk      removed pre rel 0 flags, various fixes
03/06/01   ph      removed con_ref from some ftc orders and also removed
                   reg. acc. order defined on the ftc.
03/01/01   kk      Merged mainline changes to rel a archive.
02/14/01   bgc     Moved unnecessary PACKED directives into PLT code.
01/24/01   ych     Merged T53 and JCDMA features.
01/02/01   lcc     Changed member rev_sch_rc_map from word to byte in
                   caii_rev_sch_spec_fields_type
02/09/01   lh      Sync orig inernal format for unit test.
02/09/01   va      Removed the enum for pilot rec types. They appear in the
                   cai.h file now.
01/29/01   ph      chan_capa_rec_type, moved rlp_cap_blob to keep packing code
                   unchanged (Release A support)
12/21/00   kk/lh   Added IS-2000 Release A support.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
07/06/00   jq      Added definititions for ECCLM and EGSRM. Modified CFS_RSP
                   and GSRM for addendum compliance.
06/20/00   fc      Renamed MAX_SCRM_BLOB_SIZ to CAI_MAX_SCRM_BLOB_SIZ,
                   MAX_SCRM_ACT_PN to CAI_MAX_ACT_PN and MAX_SCRM_NGHBR_PN to
                   CAI_MAX_SCRM_NGHBR_PN.
06/19/00   jrw     Corrected a problem with power control message where the
                   init_setpt shouldn't have been included with the SCH set points.
06/12/00   ks      Added support for align_timing field in CFSREQ message.
06/02/00   jrw     Changes to support IS2000 Rev 0 published addendum.
05/26/00   jq      Added CAII_BC_HDR_LEN
05/25/00   ks      Added support for Neighbor Search Window offset.
04/24/00   lcc/jrw Changed usage of SRCH_NSET_MAX and SRCH_ASET_MAX to be CAI_N8M
                   and CAI_N6M instead.  This avoid inclusion of srch.h in this
                   file, which can be problematic.
                   Added ESCAM, outer loop report message, and changes to PMRM and
                   CFSRM.
04/06/00   va      Merged with MSM3100_CP.03.00.17
           jq      Added broadcast SMS support.
03/21/00   va      Removed IS2000 #ifndef around srch.h inclusion
03/10/00   ry      Added IS2000 featurization
           ry      Removed FEATURE_IS2000_REV0_PREADDENDUM around
                   caii_feat_rec_type
           ry      Replaced FEATURE_SILK with FEATURE_MSG_PACK_UNIT_TEST
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
           dna     CR12461 CFS Report Message must hold up to 46 entries.
03/06/00   lcc     Corrected some booleans to be bytes instead.
02/29/00   jrw     Removed Phase2 stuff (saved for next release)
02/11/00   jrw     Added support for IS2000 Addendum
02/02/00   hrk     Added PACKED directive around structs for RPC.
12/17/99   jrw     Added support for IS2000 Phase 1.
10/07/99   lh      Fixed caii_redir_rec_fix_type.
08/09/99   ry      Added pre_proc_done to caii_cfs_req_fix_type and
                   caii_cfs_ctrl_type for action time processing
07/09/99   ry      Removed an extraneous '\' from caii_cfs_rpt_mod0_type
06/30/99   kmp     In the caii_ext_disp_var_type struct, made the chari field
                   array size to be one, to appease the ARM 2.50 compiler.
                   See comment in the code for more detail.
06/02/99   kmp     Merge in the following IS-95B changes
           lh      Updated for Published version of IS-95B.
           jq      Added parenthes to some macros
05/19/99   doj     Consolidate structures for HDM, EHDM and GHDM into one
                   generic handoff structure.
02/13/99   doj     Added support for General Handoff Direction Message (GHDM).
                   Also includes several misc. updates by many people to
                   support FEATURE_SPECIAL_MDR.
01/13/99   lcc     Merged in MDR support from IS95B branch.  Only messages
                   needed for MDR are merged in.
01/06/99   lh      Extended Status Response and Status Response now share the
                   same internal formats.
01/06/99   lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
12/23/98   lh      Modified ECAM. Now parser will parse the ECAM and pass the
                   caller a packed internal format.
12/16/98   lh      caii_pilot_rec_type defined.
                   ECAM implementation changed.
11/24/98   lh      Added comments.
11/19/98   lh      Added comments.
10/29/98   jcw     Changed Extended Display structs for 95B.
10/12/98   lh      Change soft_slop to soft_slope in ESPM processing.
09/15/98   lh      IS-95B support.
08/12/98   lh      IS95B WLL support: Line Control, Parametric Alerting and
                   Meter Pulses.
01/15/98   lh      Added IS-95B Line Control info record.
01/14/98   dna     Add support for Korean PCS in Band Class Status Response
07/16/97   jca     Deleted #include "ui.h".
12/13/96   jca     Added new fields to caii_chnasn_am2_type.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
03/13/96   day     Added J-STD-008 PCS message modifications.  Using FEATURE_JSTD008
                   ifdef.
08/22/95   dna     New sync channel message field
08/18/95   dna     Added CAI_EXT_PAGING_CHAN_ASSIGN assign mode '101'
08/10/95   rdb     Added internal types to support Status Request and Status
                   Response Messages.
08/09/95   jca     Added General Page Message.  Added new Access and Paging
                   Channel address types include support for IMSI addresses.
07/20/95   gb      Added Extended Handoff Direction Message.
07/20/95   jca     Added Extended Sys Parameters and Service Redirection
                   messages.  Modified other messages as per IS95A.
07/13/95   rdb     Added types to support service negotiation.
12/22/93   jca     Changed message formats to IS-95 compatible.
07/23/92   jai     Modified file for lint and linking.
07/03/92   jai     Modified file for CAI 2.0
10/19/90   jai     Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_defs.h"

#include "comdef.h"               /* Basic definitions */
#include "qw.h"                   /* Quadword definitions */

#include "cai.h"                    /* CAI definitions */
#include "cai_v.h"                  /* CAI definitions */
#include "cai_i.h"                  /* CAI definitions */
#include "caii.h"
#include "caii_v.h"
#include "lte_as.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        GENERIC MESSAGE TYPES                            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type to isolate msg_type */
typedef struct
{
  byte msg_type;         /* Message type */
} caii_gen_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Paging Channel common address fields (see IS-95A Section 7.7.2.3.1) */
typedef struct
{
  dword min1;   /* First part of the mobile identification number */
  word min2;    /* Second part of the mobile identification number */
} caii_addr0_type;

typedef struct
{
  qword esn;    /* Mobile station's electronic serial number */
} caii_addr1_type;

typedef struct
{
  caii_imsi_addr_type imsi;  /* IMSI address type specific subfields */
} caii_addr2_type;

typedef struct
{
  qword meid;    /* Mobile station's Equipment Identification number */
} caii_addr4_type;

#define CAII_BC_ADDR_MAX 15
  /* Limited by number of bits in ADDR_LEN field of a Paging Channel Msg */

#define CAII_BC_ADDR_LEN 5
  /* the broadcast address length is 5 bytes */

#define CAII_BC_HDR_LEN (sizeof(cai_pc_hdr_type) + sizeof(cai_addr5_type) * CAII_BC_ADDR_LEN)
  /* the length of data burst header that contains a BC SMS */

typedef struct
{
  byte bc_addr[CAII_BC_ADDR_MAX];  /* Broadcast address */
} caii_addr5_type;

typedef union
{
  caii_addr0_type type0;  /* Addr_type '000' subfields */
  caii_addr1_type type1;  /* Addr_type '001' subfields */
  caii_addr2_type type2;  /* Addr_type '010' subfields */
  caii_tmsi_type  type3;  /* Addr_type '011' subfields */
  caii_addr5_type type5;  /* Addr_type '101' subfields */
} caii_pc_address_type;

#ifdef FEATURE_IS2000_REL_C

/* Message Integrity Field */
typedef struct
{
  boolean maci_incl;              /* Message integrity fields included indicator */
  byte randc;                     /* Random challenge value */
  byte sdu_key_id;                /* Key id of the integrity key to be used to
                                     compute the MAC-I value for this PDU */
  byte sdu_integrity_algo;        /* Identity of the message integrity
                                     algorithm that is used for the SDU carried
                                     by this PDU */
  boolean sdu_sseq_or_sseqh;      /* Security sequence number format indicator */
  dword sdu_sseq_h;               /* The 24 most significant bits of the 32-bit
                                     security sequence number to be used to
                                     compute the MAC-I for this PDU */
  byte sdu_sseq;                  /* The 8 least significant bits of the 32-bit
                                     security sequence number to be used to
                                     compute the MAC-I value for this PDU. */
  dword maci;                     /* Message Authentication Code for Integrity */
} caii_msg_int_type;
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_A
/* Extended Encryption Fields IS-2000.4-A, 3.1.2.3.1.3 */
typedef struct
{
  boolean enc_fields_incl;  /* ENC_FIELDS_INCL */
  byte sdu_encrypt_mode;    /* SIG_ENCRYPT_MODE */
  byte enc_seq;             /* ENC_SEQ */
} caii_ext_encrypt_type;
#endif /*FEATURE_IS2000_REL_A*/

#ifdef FEATURE_IS2000_REL_C
/*  Signaling Encryption support mask (see IS-2000.5 Table 2.7.1.3.2.1-5)    */
typedef struct
{
  boolean cmea;
  boolean ecmea;
  boolean rea;
} caii_sig_encrypt_sup_type;

/*  UI Encryption support mask (see IS-2000.5 Table 2.7.1.3.2.4-9)           */
typedef struct
{
  boolean oryx;
  boolean rea;
} caii_ui_encrypt_sup_type;
#endif /* FEATURE_IS2000_REL_C */

/* Paging Channel common header fields */
typedef struct
{
  byte ack_seq;               /* Acknowledgement Sequence Number */
  byte msg_seq;               /* Message Sequence Number */
  boolean ack_req;            /* Acknowledgement Required */
  boolean valid_ack;          /* Valid Acknowledgement */
  byte addr_type;             /* Address field type */
  byte addr_len;              /* Address field length */
  caii_pc_address_type addr;  /* Mobile station address */
#ifdef FEATURE_IS2000_REL_C
  caii_msg_int_type msg_int;  /* Message integrity fields */
#endif /*FEATURE_IS2000_REL_C*/
#ifdef FEATURE_IS2000_REL_A
  caii_ext_encrypt_type enc;  /* Extended encryption fields for release A*/
#endif /*FEATURE_IS2000_REL_A*/
} caii_pc_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define CAII_IMSI_S_AND_ESN_MSID_TYPE       0x0
  /* IMSI_S and ESN address field type */
#define CAII_ESN_MSID_TYPE                  0x01
  /* ESN address field type */
#define CAII_IMSI_MSID_TYPE                 0x02
  /* IMSI address field type */
#define CAII_IMSI_ESN_MSID_TYPE             0x03
  /* IMSI and ESN address field type */
#define CAII_MEID_EXT_MSID_TYPE             0x04
  /* Use MEID Extended MSID field type */
#define CAII_IMSI_MEID_EXT_MSID_TYPE        0x14
  /* Use MEID Extended MSID field type */
#define CAII_IMSI_ESN_MEID_EXT_MSID_TYPE    0x24
  /* Use MEID Extended MSID field type */
#define CAII_TMSI_AC_MSID_TYPE              0x05
  /* TMSI address MSID field type */
#define CAII_TMSI_IMSI_PREF_MSID_TYPE       0x06
  /* TMSI address MSID field type */
#define CAII_TMSI_IMSI_ESN_MSID_TYPE        0x07
  /* TMSI address MSID field type */
#define CAII_NO_MSID_TYPE                   0xFF
  /* Unassigned address MSID field type */

/* Generic Access Channel Message Header */
typedef struct
{
  byte ack_seq;         /* Acknowledgement sequence number */
  byte msg_seq;         /* Message sequence number */
  byte ack_req;         /* Acknowledgement required indicator */
  byte valid_ack;       /* Valid acknowledgement indicator */
  byte ack_type;        /* Acknowledgement address type */
  byte msid_type;       /* Mobile station identifier field type */
  byte msid_len;        /* Mobile station identifier field length */
  caii_msid_type msid;  /* Mobile station identifier */
} caii_ac_hdr_type;

/* Auth Fields and Message Integrity Fields */
typedef struct
{

#ifdef FEATURE_IS2000_REL_C
  boolean maci_incl;         /* Message integrity fields included indicator */
  byte sdu_key_id;           /* Key id of the integrity key to be used to
                                compute the MAC-I value for this PDU */
  byte sdu_integrity_algo;   /* Identity of the message integrity
                                algorithm that is used for the SDU carried
                                by this PDU */
  boolean sdu_sseq_or_sseqh; /* Security sequence number format indicator */
  dword sdu_sseq_h;          /* The 24 most significant bits of the 32-bit
                                security sequence number to be used to
                                compute the MAC-I for this PDU */
  byte sdu_sseq;             /* The 8 least significant bits of the 32-bit
                                security sequence number to be used to
                                compute the MAC-I value for this PDU. */
  dword maci;                /* Message Authentication Code for Integrity */
#endif /* FEATURE_IS2000_REL_C */
  byte auth_mode;            /* Authentication mode */
  dword authr;               /* Authentication data - only present if
                                auth_mode == CAI_AUTH */
  byte randc;                /* Random challenge data - only present if
                                auth_mode == CAI_AUTH */
  byte count;                /* Call history parameter - only present if
                                auth_mode == CAI_AUTH */
} caii_ac_auth_msg_int_type;

#ifdef FEATURE_IS2000_REL_A
/* See IS2000 Release A 2.1.1.4.1.3.1 */
typedef struct
{
  boolean enc_fields_incl;  /* Encryption fields included */
  byte sdu_encrypt_mode;    /* Encryption mode */
  byte enc_seq;             /* Encryption sequence */
} caii_ac_enc_type;

#endif /* FEATURE_IS2000_REL_A */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Traffic Channel Layer 2 Header */

typedef struct
{
  byte ack_seq;               /* Acknowledgement sequence number */
  byte msg_seq;               /* Message sequence number */
  boolean ack_req;            /* Acknowledgement Required */
  byte encryption;            /* Message encryption indicator */
#ifdef FEATURE_IS2000_REL_C
  caii_msg_int_type msg_int;  /* Message integrity fields */
#endif /*FEATURE_IS2000_REL_C*/
#ifdef FEATURE_IS2000_REL_A
  byte sdu_encrypt_mode;   /* Signalling Encryption Mode */
  byte enc_seq;            /* Encrption Sequence number */
#endif /* FEATURE_IS2000_REL_A */
} caii_tc_hdr_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*     PAGING AND FORWARD TRAFFIC CHANNEL INFORMATION RECORD TYPES         */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Note - On the Paging Channel, information records may be included in
   the Feature Notification Message.  On the Forward Traffic Channel,
   information records may be included in the Alert With Information
   and the Flash with Information messages.                                */

#define CAII_MAX_INFO_REC_BYTES 498
  /* Maximum number of bytes allowed to accomodate internal form of variable
     length information record (based on worst case which is the Calling
     Party Subaddress information record received on the Forward Traffic
     Channel):
           2016 bits (maximum forward traffic channel message body size)
         -   24 bits (msg_type + common ack fields + reserved bits)
         -----------
           1992 bits (remaining for information records)
 divided by  24 bits (min size Calling Party Number info record)
         -----------
             83 (max possible Calling Party Number records per msg)
          *   6 (6 bytes per internal formatted record,  to make it even)
         -----------
            498 bytes */


#define CAII_MAX_INFO_REC_DWORDS (CAII_MAX_INFO_REC_BYTES/sizeof(dword) + 1)

#define ALIGN_DWORD(val) \
                ((val%4)?(val+(sizeof(dword))-val%4):val)

/* Display record type (see IS-95A Section 7.7.5.1) */
typedef struct
{
  caii_rec_hdr_type hdr;       /* Header information */
  byte              num_char;  /* Number of characters in chari array */
} caii_display_rec_fix_type;

typedef byte caii_display_rec_var_type;

#define CAII_DISPLAY_REC_MAX  247
/*  2016 bits (max forward traffic channel message body size)
  -   24 bits (msg_type + common acknowledgement fields + reserved bits)
  -   16 bits (info record type and length fields)
  -----------
    1976 bits == 247 bytes (remaining for variable chari field) */

typedef struct
{
  caii_display_rec_fix_type fix;
  caii_display_rec_var_type var[CAII_DISPLAY_REC_MAX];
} caii_display_rec_type;

/* Called Party Number record type (see IS-95A Section 7.7.5.2) */
typedef struct
{
  caii_rec_hdr_type hdr;          /* Header information */
  byte              number_type;  /* Type of number */
  byte              number_plan;  /* Numbering plan */
  byte              num_char;     /* Number of characters in chari array */
} caii_called_rec_fix_type;

typedef byte caii_called_rec_var_type;

#define CAII_CALLED_REC_MAX  246
/*  2016 bits (max forward traffic channel message body size)
  -   24 bits (msg_type + common acknowledgement fields + reserved bits)
  -   16 bits (info record type and length fields)
  -    8 bits (fixed type-specific fields including reserved bit)
  -----------
    1968 bits == 246 bytes (remaining for variable chari field) */

typedef struct
{
  caii_called_rec_fix_type fix;
  caii_called_rec_var_type var[CAII_CALLED_REC_MAX];
} caii_called_rec_type;

/* Calling Party Number record type (see IS-95A Section 7.7.5.3) */
typedef struct
{
  caii_rec_hdr_type hdr;          /* Header information */
  byte              number_type;  /* Type of number */
  byte              number_plan;  /* Numbering plan */
  byte              pi;           /* Presentation indicator */
  byte              si;           /* Screening indicator */
  byte              num_char;     /* Number of characters in variable type */
} caii_calling_rec_fix_type;

typedef byte caii_calling_rec_var_type;

#define CAII_CALLING_REC_MAX  245
/*  2016 bits (max forward traffic channel message body size)
  -   24 bits (msg_type + common acknowledgement fields + reserved bits)
  -   16 bits (info record type and length fields)
  -   16 bits (fixed type-specific fields including reserved bits)
  -----------
    1960 bits == 245 bytes (remaining for variable chari field) */

typedef struct
{
  caii_calling_rec_fix_type fix;
  caii_calling_rec_var_type var[CAII_CALLING_REC_MAX];
} caii_calling_rec_type;

/* Use Calling Party Number record type for Connected Number information
   record type (see IS-95A Section 7.7.5.4 Connected Number) */

/* Signal record type (see IS-95A Section 7.7.5.5 Signal) */
typedef struct
{
  caii_rec_hdr_type hdr;          /* Header information */
  byte              signal_type;  /* Signal type */
  byte              alert_pitch;  /* Pitch of the alerting signal */
  byte              signal;       /* Signal code */
} caii_signal_rec_type;

/* Message Waiting record type (see IS-95A Section 7.7.5.6) */
typedef struct
{
  caii_rec_hdr_type hdr;        /* Header information */
  byte              msg_count;  /* Number of waiting messages */
} caii_msg_waiting_rec_type;


typedef struct
{
  boolean fch_incl;
  uint8 fch_chan_adj_gain;
  boolean dcch_incl;
  uint8 dcch_chan_adj_gain;
  boolean sch0_incl;
  uint8 sch0_chan_adj_gain;
  boolean sch1_incl;
  byte sch1_chan_adj_gain;
#if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_IS2000_1X_ADV)
  boolean rev_ackch_incl;
  byte    rev_ackch_chan_adj_gain;
#endif /* FEATURE_IS2000_REL_C || FEATURE_IS2000_1X_ADV  */
#ifdef FEATURE_IS2000_REL_C
  boolean rev_cqich_incl;
  byte    rev_cqich_chan_adj_gain;
#endif /* FEATURE_IS2000_REL_C */
} caii_rpc_adj0_type;


typedef struct
{
  boolean rl_att_adj_gain_type;
  boolean rc3_rc5_20ms_incl;
  byte rl_att_adj_gain_1500;
  byte rl_att_adj_gain_2700;
  byte rl_att_adj_gain_4800;
  byte rl_att_adj_gain_9600;
  boolean rc4_rc6_20ms_incl;
  byte rl_att_adj_gain_1800;
  byte rl_att_adj_gain_3600;
  byte rl_att_adj_gain_7200;
  byte rl_att_adj_gain_14400;
  boolean five_ms_incl;
  byte norm_att_gain_9600_5ms;
#ifdef FEATURE_IS2000_REL_A
  boolean rc3_rc5_40ms_incl;
  byte rl_att_adj_gain_1350_40ms;
  byte rl_att_adj_gain_2400_40ms;
  byte rl_att_adj_gain_4800_40ms;
  byte rl_att_adj_gain_9600_40ms;
  boolean rc3_rc6_40ms_incl;
  byte rl_att_adj_gain_1800_40ms;
  byte rl_att_adj_gain_3600_40ms;
  byte rl_att_adj_gain_7200_40ms;
  byte rl_att_adj_gain_14400_40ms;
  boolean rc3_rc5_80ms_incl;
  byte rl_att_adj_gain_1200_80ms;
  byte rl_att_adj_gain_2400_80ms;
  byte rl_att_adj_gain_4800_80ms;
  byte rl_att_adj_gain_9600_80ms;
  boolean rc3_rc6_80ms_incl;
  byte rl_att_adj_gain_1800_80ms;
  byte rl_att_adj_gain_3600_80ms;
  byte rl_att_adj_gain_7200_80ms;
  byte rl_att_adj_gain_14400_80ms;
#endif /* FEATURE_IS2000_REL_A */
} caii_rpc_adj1_type;

typedef struct
{
  byte code_type;               // 0 - convolutional, 1 - turbo
  byte rl_att_adj_gain_type;    // 0 - nominal gain attribute adjustment, 1 - pilot reference
  boolean rc3_rc5_20ms_incl;
  byte rl_att_adj_gain_19200;
  byte rl_att_adj_gain_38400;
  byte rl_att_adj_gain_76800;
  byte rl_att_adj_gain_153600;
  byte rl_att_adj_gain_307200;
  byte rl_att_adj_gain_614400;
  boolean rc4_rc6_20ms_incl;
  byte rl_att_adj_gain_28800;
  byte rl_att_adj_gain_57600;
  byte rl_att_adj_gain_115200;
  byte rl_att_adj_gain_230400;
  byte rl_att_adj_gain_460800;
  byte rl_att_adj_gain_1036800;
#ifdef FEATURE_IS2000_REL_A
  boolean rc3_rc5_40ms_incl;
  byte rl_att_adj_gain_19200_40ms;
  byte rl_att_adj_gain_38400_40ms;
  byte rl_att_adj_gain_76800_40ms;
  byte rl_att_adj_gain_153600_40ms;
  byte rl_att_adj_gain_307200_40ms;
  boolean rc3_rc6_40ms_incl;
  byte rl_att_adj_gain_28800_40ms;
  byte rl_att_adj_gain_57600_40ms;
  byte rl_att_adj_gain_115200_40ms;
  byte rl_att_adj_gain_230400_40ms;
  byte rl_att_adj_gain_518400_40ms;
  boolean rc3_rc5_80ms_incl;
  byte rl_att_adj_gain_19200_80ms;
  byte rl_att_adj_gain_38400_80ms;
  byte rl_att_adj_gain_76800_80ms;
  byte rl_att_adj_gain_153600_80ms;
  boolean rc3_rc6_80ms_incl;
  byte rl_att_adj_gain_28800_80ms;
  byte rl_att_adj_gain_57600_80ms;
  byte rl_att_adj_gain_115200_80ms;
  byte rl_att_adj_gain_259200_80ms;
#endif /* FEATURE_IS2000_REL_A */
} caii_rpc_adj2_type;

#ifdef FEATURE_IS2000_REL_C

typedef struct
{
  boolean rl_cqich_att_adj_gain_type;
  boolean rl_cqich_att_adj_gain_high_incl;
  byte    rl_cqich_att_adj_gain_high;
  boolean rl_cqich_att_adj_gain_low_incl;
  byte    rl_cqich_att_adj_gain_low;
} caii_rpc_adj3_type;
#endif /* FEATURE_IS2000_REL_C */


typedef union
{
  caii_rpc_adj0_type adj0;
  caii_rpc_adj1_type adj1;
  caii_rpc_adj2_type adj2;
#ifdef FEATURE_IS2000_REL_C
  caii_rpc_adj3_type adj3;
#endif /* FEATURE_IS2000_REL_C */

} caii_rpc_adj_type;

typedef struct
{
  byte rpc_adj_rec_type;
  /* BKM Why doesn't this have the length?  */
  caii_rpc_adj_type rpc_adj;
} caii_rpc_type;


typedef struct
{
  byte sr_id;             /* Service reference identifier */
  byte logical_resource;  /* Logical resource identifer */
  byte physical_resource; /* Physical resource identifer */
  boolean forward_flag;   /* Forward mapping indicator */
  boolean reverse_flag;   /* Reverse mapping indciator */
  byte priority;          /* Multiplexing priority */
} caii_nnscr_lpm_entry_type;

#ifdef FEATURE_IS2000_REL_A
typedef struct
{
  byte sch_id;       /* SCH identification */
  byte frame_offset; /* SCH multiple frame offset */
} caii_nnscr_sch_type;

typedef struct
{
  byte sr_id;          /* Service reference identifer */
  boolean sdb_so_omit; /* Short data burst service option number omitted
                          indicator */
} caii_nnscr_so_rec_type;

typedef struct
{
  byte num_bits_idx; /* Index to the number of bits array */
  word num_bits;     /* Number of bits array */
  byte crc_len_idx;  /* Array of number of CRC bits */
} caii_nnscr_num_bits_record_type;

#define MAX_RECS 16

typedef struct
{
  byte num_bits_table_id; /* Flexibile rate table ID */
  byte num_recs;          /* Number of records in the flexible rate table */
  caii_nnscr_num_bits_record_type recs[MAX_RECS];
} caii_nnscr_num_bits_table_type;

typedef struct
{
  byte num_bits_idx; /* Index to the number of bits array */
  word mask;         /* Number of bits array */
} caii_nnscr_var_rate_record_type;

typedef struct
{
  byte var_rate_table_id; /* Variable rate mask table ID */
  byte num_recs;          /* Number of records in the variable rate mask table */
  caii_nnscr_var_rate_record_type recs[MAX_RECS];
} caii_nnscr_var_rate_table_type;

typedef struct
{
  byte nbits_idx; /* Number of bits per frame index */
  byte num_ltus;  /* Number of LTUs per physical layer SCH frame */
} caii_nnscr_ltu_record_type;

#define MAX_ROW_1 16

typedef struct
{
  byte ltu_table_id; /* LTU table ID */
  byte num_rows;     /* Number of configurations associated with the LTU table
                        identified by LTU_TABLE_ID */
  caii_nnscr_ltu_record_type row[MAX_ROW_1];
} caii_nnscr_ltu_table_type;

typedef struct
{
  byte sr_id;        /* Service reference ID */
  word srv_num_bits; /* Number of bits allocated to the service */
} caii_nnscr_partition_record_type;

typedef struct
{
  byte category;       /* Category number */
  byte mux_header;     /* Multiplex sublayer header length */
  byte num_partitions; /* Number of partitions */
  caii_nnscr_partition_record_type part[8];
} caii_nnscr_partition_mux_type;

#define MAX_ROW_2 32

typedef struct
{
  byte partition_table_id; /* Partition table ID */
  byte num_rows;           /* Number of configurations associated with the
                              partition table identified by PARTITION_TABLE_ID */
  caii_nnscr_partition_mux_type row[MAX_ROW_2];
} caii_nnscr_partition_table_type;
#endif /* FEATURE_IS2000_REL_A */


#define MAX_BIT_TABLE 8
#define MAX_VAR_TABLE 8
#define MAX_TABLE 4
#define MAX_PART_TABLE 4


typedef struct
{
  boolean fpc_incl;             /* Forward power control information included
                                   indicator */
  byte fpc_pri_chan;            /* Power control subchannel indicator */
  byte fpc_mode;                /* Forward power control operation mode
                                   indicator */
  boolean fpc_olpc_fch_incl;    /* FCH outer loop power control parameter
                                   included indicator */
  byte fpc_fch_fer;             /* FCH target frame error rate */
  byte fpc_fch_min_setpt;       /* Min. FCH outer loop Eb/Nt setpoint */
  byte fpc_fch_max_setpt;       /* Max. FCH outer loop Eb/Nt setpoint */
  boolean fpc_olpc_dcch_incl;   /* DCCH outer loop power control parameter
                                   included indicator */
  byte fpc_dcch_fer;            /* DCCH target frame error rate */
  byte fpc_dcch_min_setpt;      /* Min. DCCH outer loop Eb/Nt setpoint */
  byte fpc_dcch_max_setpt;      /* Max. DCCH outer loop Eb/Nt setpoint */
  boolean gating_rate_incl;     /* Reverse pilot channel gating rate included
                                   flag */
  byte pilot_gate_rate;         /* Reverse pilot channel gating rate */
#ifdef FEATURE_IS2000_REL_A
  boolean for_sch_incl;         /* F-SCH included indicator */
  byte num_for_sch;             /* Number of F-SCH */
  caii_nnscr_sch_type for_sch[CAI_NNSCR_MAX_NUM_SCH];
  boolean rev_sch_incl;         /* R-SCH included indicator */
  byte num_rev_sch;             /* Number of R-SCH */
  caii_nnscr_sch_type rev_sch[CAI_NNSCR_MAX_NUM_SCH];
#endif /* FEATURE_IS2000_REL_A */
  byte lpm_ind;                 /* LPM indicator */
  byte num_lpm_entries;         /* Number of LPM entries */
  caii_nnscr_lpm_entry_type lpm_entries[CAI_MAX_NUM_LPM_ENTRIES];
#ifdef FEATURE_IS2000_REL_A
  byte numrec;                  /* Number of service-specific records */
  caii_nnscr_so_rec_type rec[CAI_NNSCR_MAX_NUM_SR_REC];
  boolean use_flex_num_bits;    /* Use flexible number of bits per frame indicator */
  boolean num_bits_tables_incl; /* Flexible rate tables included indicator */
  byte num_bits_tables_count;   /* Number of instances of the flexible rate
                                   table included */
  caii_nnscr_num_bits_table_type bit_table[MAX_BIT_TABLE];
  boolean use_var_rate;         /* Use variable on SCHs indicator */
  boolean var_tables_incl;      /* Variable rate tables included indicator */
  byte var_rate_tables_count;   /* Number of instances of the variable rate
                                   mask table included */
  caii_nnscr_var_rate_table_type var_table[MAX_VAR_TABLE];
  boolean use_old_flex_mapping; /* Use previous downloaded mapping between
                                   channels and flexible rate tables */
  byte fsch0_nbit_table_id;     /* Forward SCH 0 flexible rate table ID */
  byte rsch0_nbit_table_id;     /* Reverse SCH 0 flexible rate table ID */
  byte fsch1_nbit_table_id;     /* Forward SCH 1 flexible rate table ID */
  byte rsch1_nbit_table_id;     /* Reverse SCH 1 flexible rate table ID */
  byte ffch_nbit_table_id;      /* Forward FCH flexible rate table ID */
  byte rfch_nbit_table_id;      /* Reverse FCH flexible rate table ID */
  byte fdcch_nbit_table_id;     /* Forward DCCH flexible rate table ID */
  byte fdcch_nbits_idx;         /* Forward DCCH number of information bits per
                                   frame index */
  byte rdcch_nbit_table_id;     /* Reverse DCCH flexible rate table ID */
  byte rdcch_nbits_idx;         /* Reverse DCCH number of information bits per
                                   frame index */
  boolean use_old_var_mapping;  /* Use previous dowloaded mapping between
                                   channels and variable rate mask tables */
  byte fsch0_var_table_id;      /* Forward SCH 0 variable rate mask table ID */
  byte rsch0_var_table_id;      /* Reverse SCH 0 variable rate mask table ID */
  byte fsch1_var_table_id;      /* Forward SCH 1 variable rate mask table ID */
  byte rsch1_var_table_id;      /* Reverse SCH 1 variable rate mask table ID */
  boolean r_inc_rate_allowed;   /* Reverse increase rate within variable rate
                                   set allowed indicator */
  boolean f_inc_rate_allowed;   /* Forward increase rate within variable rate
                                   set allowed indicator */
  boolean ltu_info_incl;        /* LTU information included indicator */
  boolean ltu_tables_incl;      /* LTU tables included indicator */
  byte num_ltu_tables;          /* Number of LTU tables included */
  caii_nnscr_ltu_table_type table[MAX_TABLE];
  boolean use_old_ltu_mapping;  /* Use the previously downloaded mapping
                                   between channels and LTU tables */
  byte fsch0_ltu_tab_id;        /* Forward SCH 0 LTU table ID */
  byte rsch0_ltu_tab_id;        /* Reverse SCH 0 LTU table ID */
  byte fsch1_ltu_tab_id;        /* Forward SCH 1 LTU table ID */
  byte rsch1_ltu_tab_id;        /* Reverse SCH 1 LTU table ID */
  boolean partition_tables_info_incl;/* Partition tables information included
                                        indicator */
  boolean partition_tables_incl;/* Partition tables included indicator */
  byte num_partition_tables;    /* Number of partition tables */
  caii_nnscr_partition_table_type part_table[MAX_PART_TABLE];
  boolean use_old_part_mapping; /* Use the previously downloaded mapping
                                   between the channels and partition tables */
  byte ffpc_part_tab_id;        /* Forward FCH partition table ID */
  byte rfpc_part_tab_id;        /* Reverse FCH partition table ID */
  byte fdcch_part_tab_id;       /* Forward DCCH partition table ID */
  byte rdcch_part_tab_id;       /* Reverse DCCH partition table ID */
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_B
  boolean use_eram;                     /* Use the Enhanced Rate Adaptation
                                           Mode indicator */
#endif /* FEATURE_IS2000_REL_B */
#ifdef FEATURE_IS2000_REL_C
  boolean switching_params_incl;        /* R-CQICH switching parameters
                                           included */
  byte num_soft_switching_frames_chm;   /* Number pf frames for R-CQICH soft
                                           switching while in Control Hold */
  byte num_softer_switching_frames_chm; /* Number of frames for R-CQICH
                                           softer switching while in Control
                                           Hold */
#endif /* FEATURE_IS2000_REL_C */

} caii_non_neg_srv_cfg_type;

typedef struct
{
  caii_srv_cfg_type neg_scr;             /* Negotiable part of the SCR */
  caii_non_neg_srv_cfg_type non_neg_scr; /* Non-negotiable part of the SCR */
} caii_complete_srv_cfg_type;


/* Called Party Subaddress record type ( see IS-95B Section 7.7.5.8) */
/* Calling Party Subaddress info record IS-95B 7.7.5.9 */
/* Connected Subaddress info record IS-95B 7.7.5.10 */
/* Redirecting Subaddress info record IS-95B 7.7.5.12 */
typedef struct
{
  caii_rec_hdr_type hdr;       /* Header information */
  boolean extension_bit;       /* The extension bit */
  byte    subadd_type;         /* Type of subaddress */
  boolean odd_even_ind;        /* The indicator of even/odd bits */
  byte    num_char;            /* Number of characters in chari array */
} caii_subaddr_rec_fix_type;

typedef byte caii_subaddr_rec_var_type;

#define CAII_SUBADDR_REC_MAX  246
/*  2016 bits (max forward traffic channel message body size)
  -   24 bits (msg_type + common acknowledgement fields + reserved bits)
  -   16 bits (info record type and length fields)
  -    8 bits (type specific field)
  -----------
    1968 bits == 246 bytes (remaining for variable chari field) */

typedef struct
{
  caii_subaddr_rec_fix_type fix;
  caii_subaddr_rec_var_type var[CAII_SUBADDR_REC_MAX];
} caii_subaddr_rec_type;

/* Redirecting Number info record IS-95B 7.7.5.11 */
typedef struct
{
  caii_rec_hdr_type hdr;  /* Header information */
  boolean extension_bit1; /* The extension bit */
  byte number_type;       /* Type of number */
  byte number_plan;       /* Numbering plan */
  boolean extension_bit2; /* The extension bit */
  byte pi;                /* Presentation indicator */
  byte si;                /* Screening indicator */
  boolean extension_bit3; /* The extension bit */
  byte redir_reason;      /* The reason for redirection */
  byte num_char;          /* Number of characters in chari array */
} caii_redir_rec_fix_type;

typedef byte caii_redir_rec_var_type;

#define CAII_REDIR_REC_MAX  245
/*  2016 bits (max forward traffic channel message body size)
  -   24 bits (msg_type + common acknowledgement fields + reserved bits)
  -   16 bits (info record type and length fields)
  -   16 bits (type specific fields)
  -----------
    1960 bits == 245 bytes (remaining for variable chari field) */

typedef struct
{
  caii_redir_rec_fix_type fix;
  caii_redir_rec_var_type var[CAII_REDIR_REC_MAX];
} caii_redir_rec_type;


/* Meter Pulses record type (see IS-95B Section 7.7.5.13) */
typedef struct
{
  caii_rec_hdr_type hdr;        /* Header information */
  byte on_time;                 /* Pulse on time */
  byte off_time;                /* Pulse off time */
  byte pulse_cnt;               /* Pulse count */
  word pulse_freq;              /* Pulse frequency, put here to save 2 bytes*/
} caii_meter_pulses_rec_type;

/* Parametric Alerting record type (see IS-95B Section 7.7.5.14) */
typedef struct
{
  caii_rec_hdr_type hdr;          /* Header information */
  byte cadence_cnt;               /* Cadence count */
  byte num_groups;                /* Number of groups in variable part*/
#if defined(FEATURE_IS2000_REL_A)
  byte cadence_type;              /* Cadence Type */
#endif
} caii_para_alert_fix_type;

typedef struct
{
  byte amplitude;                 /* Amplitude */
  word freq_1;                    /* Frequency 1 */
  word freq_2;                    /* Frequency 2 */
  byte on_time;                   /* On time */
  byte off_time;                  /* Off time */
  byte repeat;                    /* Repeat */
  byte delay;                     /* Delay */
} caii_para_alert_var_type;

#define CAII_PARA_ALERT_MAX   16
/* This is the maximum possible value the base station could send in
   the NUM_GROUP field. */

typedef struct
{
  caii_para_alert_fix_type fix;
  caii_para_alert_var_type var[CAII_PARA_ALERT_MAX];
} caii_para_alert_rec_type;

/* Line Control Record ( see IS-95B Section 7.7.5.15 ) */
typedef struct
{
  caii_rec_hdr_type hdr;  /* Header information */
  boolean pol_inc;        /* Polarity included */
  boolean tgl_mod;        /* Toggle mode */
  boolean rev_pol;        /* Reverse polarity */
  byte    pow_dtm;        /* Power denial time */
} caii_line_ctrl_rec_type;

/* User Zone Update Record ( see IS-2000 Section 7.7.5.18 ) */
typedef struct
{
  caii_rec_hdr_type hdr;    /* Header information */
  word uzid;                /* User Zone Identifier */
} caii_user_zone_update_type;

/* User Zone Reject Record ( see IS-2000 Section 7.7.5.19 ) */
typedef struct
{
  caii_rec_hdr_type hdr;      /* Header information */
  word reject_uzid;           /* rejected user zone id */
  byte reject_action_indi;    /* rejection action ind */
  boolean uzid_assign_incl;   /* user zone id assignment incl */
  word assign_uzid;           /* assigned user zone id */
                              /*  (if user zone id assignment incl == 1) */
} caii_user_zone_reject_type;

#ifdef FEATURE_IS2000_REL_A
/* Multi-Character Extended Display record ( see IS-95B Section 7.7.5.16 ) */

#define CAII_MC_EXT_DISP_BUF_SIZ 288
  /* This constant is somewhat arbitrary.  Note that it is 288 dwords, so 1152 bytes.
     We need enough space to store the info recs and also our internal pointers (in
     the recs array).  For now 288 should be enough. */

#define CAII_MC_EXT_DISP_MAX 255

typedef struct
{
  byte display_encoding; /* Indicator of the display information */
  byte num_fields;       /* Number of occurrence of CHARi field */
  byte chari[CAII_MC_EXT_DISP_MAX];
} caii_mc_ext_disp_rec_info_type;

typedef struct
{
  byte display_tag; /* Indicator of the display information */
  byte num_record;  /* Number of record displaying */
  caii_mc_ext_disp_rec_info_type* recs[CAII_MC_EXT_DISP_MAX];
    /* Keep pointers to where the records are located in the buf array.  These pointers
       are packed inside the buf array to save space; that way, we don't actually store
       255 of them, we just store as many as we need for each display.
       SILK doesn't store these pointers, that's why they are featurized out for SILK. */
} caii_mc_ext_disp_rec_type;

typedef struct
{
  caii_rec_hdr_type hdr;         /* Header information */
  boolean mc_ext_display_ind;  /* The indicator if Extended Display record */
  byte display_type;           /* Display type */
  byte num_displays;             /* Number of occurrence of display text included */
  dword buf[CAII_MC_EXT_DISP_BUF_SIZ];
    /* Buffer size to hold the packed internal format of display records */
  caii_mc_ext_disp_rec_type* displays[CAII_MC_EXT_DISP_MAX];
    /* Pointer to display records in the buffer */
} caii_mc_ext_disp_type;

/* Call Waiting indicator record type (see IS-95A Section 3.7.5.22) */
typedef struct
{
  caii_rec_hdr_type hdr;        /* Header information */
  byte              call_waiting_ind;  /* Number of waiting messages */
} caii_call_waiting_ind_type;

#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
/* Enhanced Multi-Character Extended Display record ( see IS-95B Section 3.7.5.23 ) */

#define CAII_ENH_MC_EXT_DISP_BUF_SIZ 288
  /* This constant is somewhat arbitrary.  Note that it is 288 dwords, so 1152 bytes.
     We need enough space to store the info recs and also our internal pointers (in
     the recs array).  For now 288 should be enough. */

#define CAII_ENH_MC_EXT_DISP_MAX 255

typedef struct
{
  byte display_encoding; /* Indicator of the display information */
  byte num_fields;       /* Number of occurrence of CHARi field */
  byte chari[CAII_ENH_MC_EXT_DISP_MAX];
} caii_enh_mc_ext_disp_rec_info_type;

typedef struct
{
  byte display_tag; /* Indicator of the display information */
  byte num_record;  /* Number of record displaying */
  caii_enh_mc_ext_disp_rec_info_type* recs[CAII_ENH_MC_EXT_DISP_MAX];
    /* Keep pointers to where the records are located in the buf array.  These pointers
       are packed inside the buf array to save space; that way, we don't actually store
       255 of them, we just store as many as we need for each display.
       SILK doesn't store these pointers, that's why they are featurized out for SILK. */
} caii_enh_mc_ext_disp_rec_type;

typedef struct
{
  caii_rec_hdr_type hdr;         /* Header information */
  byte display_type;           /* Display type */
  byte num_displays;             /* Number of occurrence of display text included */
  dword buf[CAII_ENH_MC_EXT_DISP_BUF_SIZ];
    /* Buffer size to hold the packed internal format of display records */
  caii_enh_mc_ext_disp_rec_type* displays[CAII_ENH_MC_EXT_DISP_MAX];
    /* Pointer to display records in the buffer */
} caii_enh_mc_ext_disp_type;

#endif /* FEATURE_IS2000_REL_C */

/* Generic Record for national supplementary services
   (see ARIB STD-T53 section 7.7.5.17) */

typedef struct
{
  caii_rec_hdr_type hdr;        /* Header information */
  word mcc;                     /* Mobile country code */
  byte record_subtype;          /* Country-specific record type */
} caii_ntl_sup_svc_type;

/* CLIR national supplmentary services record (see ARIB STD-T53 section 7.7.5.18) */
typedef struct
{
  caii_rec_hdr_type hdr;        /* Header information */
  word mcc;                     /* Mobile country code */
  byte record_subtype;          /* Country-specific record type */
  byte cause;                   /* Reason code */
} caii_clir_sup_svc_type;

/* Release national supplmentary services record
   (see ARIB STD-T53 section 7.7.5.19) */
typedef struct
{
  caii_rec_hdr_type hdr;        /* Header information */
  word mcc;                     /* Mobile country code */
  byte record_subtype;          /* Country-specific record type */
} caii_release_sup_svc_type;

/* Audio Control national supplmentary services record
   (see ARIB STD-T53 section 7.7.5.20) */
typedef struct
{
  caii_rec_hdr_type hdr;        /* Header information */
  word mcc;                     /* Mobile country code */
  byte record_subtype;          /* Country-specific record type */
  byte up_link;                 /* Audio path from the MS to the BS */
  byte down_link;               /* Audio path from the BS to the MS */
} caii_audio_ctl_sup_svc_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*            REVERSE TRAFFIC CHANNEL INFORMATION RECORD TYPES             */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Note - these records are used in the Flash With Information and the
   Status messages (see IS-95A Sections 6.7.2.3.2.3 Flash With Information
   Message and 6.7.2.3.2.8 Status Message)                                 */

/* Note that Reverse Traffic Channel Info records use the same record
   header (caii_rec_hdr_type) as Forward Traffic Channel Info records */

#ifdef FEATURE_IS2000_REL_A
/*  Reverse Traffic Channel Information elements are defined
    in IS2000-5 Release A Section 2.7.4

    Information elements can be included in following messages:
      Origination Continuation Message
      Enhanced Origination Message
      Flash with Information Message
      Extended Flah with Information Message
      Service Request Message
      Service Response Message
      Status Message
      Status Response Message
    */
#endif /* FEATURE_IS2000_REL_A */

/* Feature Indicator record type (see IS-95A Section 6.7.4.1) */
typedef struct
{
  caii_rec_hdr_type hdr;      /* Header information */
  byte              feature;  /* Feature identifier */
} caii_feat_rec_type;

/* Keypad Facility record type (see IS-95A Section 6.7.4.2) */
typedef struct
{
  caii_rec_hdr_type hdr;       /* Header information */
  byte              num_char;  /* Number of characters in chari array */
} caii_key_rec_fix_type;

typedef byte caii_key_rec_var_type;

#define CAII_KEY_REC_MAX  ((CAII_MAX_INFO_REC_BYTES -            \
                             sizeof( caii_key_rec_fix_type )) /  \
                               sizeof( caii_key_rec_var_type ))
typedef struct
{
  caii_key_rec_fix_type fix;
  caii_key_rec_var_type var[CAII_KEY_REC_MAX];
} caii_key_rec_type;

/* Use Forward Traffic Channel information record types from above for
   Called Party Number and Calling Party Number infomation records (see
   IS-95A Sections 6.7.4.3 Called Party Number and 6.7.4.4 Calling
   Party Number) */

/* Call Mode record type (see IS-95A Section 6.7.4.6) */
typedef struct
{
  caii_rec_hdr_type hdr;          /* Header information */
  boolean           orig_mode;    /* Origination mode indicator */
  word              pri_service;  /* Primary service option */
  word              sec_service;  /* Secondary service option */
} caii_call_mode_rec_type;

/* Terminal Information record type (see IS-95A Section 6.7.4.7) */
typedef struct
{
  caii_rec_hdr_type hdr;  /* Header information */
  byte mob_p_rev;         /* Protocol revision of mobile station */
  byte mob_mfg_code;      /* Manufacturer code */
  byte mob_model;         /* Model number */
  word mob_firm_rev;      /* Firmware revision number */
  byte scm;               /* Station class mark */
  boolean local_ctrl;     /* Local control indicator */
  byte slot_cycle_index;  /* Slot cycle mode */
  byte num_so;            /* Number of entries in service option array */
} caii_term_rec_fix_type;

#define CAII_TERM_REC_MAX  ((CAII_MAX_INFO_REC_BYTES -           \
                              sizeof( caii_term_rec_fix_type )) /  \
                                sizeof( caii_term_rec_var_type ))

typedef struct
{
  word service_option;    /* Supported service option */
} caii_term_rec_var_type;

typedef struct
{
  caii_term_rec_fix_type fix;
  caii_term_rec_var_type var[CAII_TERM_REC_MAX];
} caii_term_rec_type;

/* Roaming Information record type (see IS-95A Section 6.7.4.8) */
typedef struct
{
  caii_rec_hdr_type hdr;     /* Header information */
  byte accolc;               /* Overload class */
  boolean mob_term_home;     /* Home registration enable indicator */
  boolean mob_term_for_sid;  /* Foreign SID roaming registration enable
                                indicator */
  boolean mob_term_for_nid;  /* Foreign NID roaming registration enable
                                indicator */
  word num_vars;             /* Number of sid,nid pairs included in record */
} caii_roam_rec_fix_type;

typedef struct
{
  word sid;                  /* System identification */
  word nid;                  /* Network identification */
} caii_roam_rec_var_type;

#define CAII_ROAM_REC_MAX  ((CAII_MAX_INFO_REC_BYTES -            \
                             sizeof( caii_roam_rec_fix_type )) /  \
                               sizeof( caii_roam_rec_var_type ))

typedef struct
{
  caii_roam_rec_fix_type fix;
  caii_roam_rec_var_type var[CAII_ROAM_REC_MAX];
} caii_roam_rec_type;

/* Security Status record type (see IS-95A Section 6.7.4.9) */
typedef struct
{
  caii_rec_hdr_type hdr;     /* Header information */
  byte auth_mode;            /* Authentication mode */
  byte encrypt_mode;         /* Message encryption mode */
  boolean private_lcm;       /* Private long code mask indicator */
} caii_sec_stat_rec_type;

/* Use Forward Traffic Channel Connected Number information record type for
   Connected Number type (see IS-95A Sections 6.7.4.10 and 7.7.5.4) */

/* IMSI record type (see IS-95A Section 6.7.4.11) */
typedef struct
{
  caii_rec_hdr_type hdr;     /* Header information */
  byte imsi_class;           /* IMSI Class number */
  byte imsi_addr_num;        /* Number of IMSI address digits */
  word mcc;                  /* Mobile country code */
  byte imsi_11_12;           /* The 11th and 12th digits of IMSI */
  qword imsi_s;              /* Last ten digits of the IMSI */
} caii_imsi_rec_type;

/* ESN record type (see IS-95A Section 6.7.4.12) */
typedef struct
{
  caii_rec_hdr_type hdr;     /* Header information */
  dword esn;                 /* Mobile station electronic serial number */
} caii_esn_rec_type;

/* Band class record type (see IS-95-A TSB) */
typedef struct
{
  caii_rec_hdr_type hdr; /* Header information */
  boolean band[CAI_MAX_BAND_CLASSES];
} caii_band_rec_type;

/* Power class record type (see IS-95-A TSB) */
typedef struct
{
  caii_rec_hdr_type hdr; /* Header information */
  byte max_eirp;         /* Max EIRP (Effective Isotropic Radiated Power) */
} caii_pwr_rec_type;

/* Operating mode record type (see IS-95-A TSB) */
typedef struct
{
  caii_rec_hdr_type hdr; /* Header information */
  boolean op_mode0;      /* ANSI J-STD-008 */
  boolean op_mode1;      /* TIA/EIA/IS-95-A CDMA mode */
  boolean op_mode2;      /* TIA/EIA/IS-95-A analog mode */
  boolean op_mode3;      /* TIA/EIA/IS-91 wide analog mode */
  boolean op_mode4;      /* TIA/EIA/IS-91 narrow analog mode */
} caii_op_mode_rec_type;

/* Service option record type (see IS-95-A TSB) */

typedef struct
{
  caii_rec_hdr_type hdr; /* Header information */
  byte  num_so;          /* Number of so recs in variable part of msg */
} caii_so_rec_fix_type;

typedef struct
{
  boolean fwd;           /* Forward link support indicator */
  boolean rev;           /* Reverse link support indicator */
  word so;               /* Service option number */
} caii_so_rec_var_type;

typedef struct
{
  caii_so_rec_fix_type  fix;       /* Fixed portion of record */
  caii_so_rec_var_type  var[ 1 ];  /* Variable portion. Although array is
                                      dimensioned for 1, there may be an
                                      arbitrary number of array elements */
} caii_so_rec_type;

/* Multiplex option record type (see IS-95-A TSB) */
typedef struct
{
  caii_rec_hdr_type hdr; /* Header information */
  byte num_mux;          /* Number of mux recs in variable part of msg */
} caii_mux_rec_fix_type;

typedef struct
{
  word mux;              /* Multiplex option number */
  byte fwd_rates;        /* Forward link rates supported */
  byte rev_rates;        /* Reverse link rates supported */
} caii_mux_rec_var_type;

typedef struct
{
  caii_mux_rec_fix_type  fix;       /* Fixed part of record */
  caii_mux_rec_var_type  var[ 1 ];  /* Variable part. There may be an
                                       arbitrary number of these */
} caii_mux_rec_type;

/* Service configuration record type */
/* Same as defined for forward traffic channel (see above) */

/* Called Party Subaddress, Calling Party Subaddress, and Connected
   Subaddress (see IS-95B 6.7.4.19-21 ) use subaddress type defined
   for Forward Link */

/* Power Control Information (see IS-95B Section 6.7.4.22) */
typedef struct
{
  caii_rec_hdr_type hdr;    /* Header information */
  byte min_pwr_cntl_step;   /* Minimum power control step */
} caii_pwr_ctrl_rec_type;

/* IMSI_M record type (see IS-95A Section 6.7.4.23) */
/* IMSI_T record type (see IS-95A Section 6.7.4.24) */
/* Use type defined for IMSI or IMSI_O */

/* Capability info record (see IS-95B Section 6.7.4.25) */
typedef struct
{
  caii_rec_hdr_type hdr;          /* Header information */
  boolean acc_entry_ho;           /* Access entry handoff supported */
  boolean acc_probe_ho;           /* Access probe handoff supported */
  boolean analog_search;          /* Analog search supported */
  boolean hop_beacon;             /* Hopping Beacon */
  boolean mahho;                  /* Mobile Assisted HHO */
  boolean puf;                    /* PUF */
  boolean analog_553;             /* TIA/EIA553 supported */
  boolean qpch;                   /* QPCH support */
  boolean slotted_timer;          /* Slotted timer support */
  boolean chs_supported;          /* Control hold mode support */
  byte gating_rate_set;           /* Set of supported reverse pilot gating rates */
  boolean ext_cap_included;       /* Extended capabilities included indicator */
  boolean mabo;                   /* Mobile assisted burst operation capability
                                     indicator */
  boolean sdb;                    /* Short data burst supported indicator */
#ifdef FEATURE_IS2000_1X_ADV
  boolean rel_e_cap_included;     /* Release E capabilities included indicator */
#endif /* FEATURE_IS2000_1X_ADV */
  byte rlp_cap_blob_len;          /* RLP capability information length */
  byte rlp_cap_blob[CAI_MAX_RLP_INFO_LEN]; /* RLP information block of bits */

#if defined(FEATURE_IS2000_REL_A) || defined(FEATURE_IS2000_1X_ADV)
  boolean flex_supported;         /* Flexible rate feature supported indicator */
  boolean f_fch_flex_supported;   /* F-FCH flexible rate feature supported
                                     indicator */
  boolean r_fch_flex_supported;   /* R-FCH flexible rate feature supported
                                     indicator */
  boolean f_dcch_flex_supported;  /* F-DCCH flexible rate feature supported
                                     indicator */
  boolean r_dcch_flex_supported;  /* R-DCCH flexible rate feature supported
                                     indicator */
  boolean f_sch_flex_supported;   /* F-SCH flexible rate feature supported
                                     indicator */
  boolean r_sch_flex_supported;   /* R-SCH flexible rate feature supported
                                     indicator */
  boolean var_supported;          /* Variable rate feature supported indicator */
  boolean f_sch_var_supported;    /* F-SCH variable rate feature supported
                                     indicator */
  boolean r_sch_var_supported;    /* R-SCH variable rate feature supported
                                     indicator */
  word max_num_bits_c;            /* Max. sum of number of bits corresponding
                                     to convolutional rates in the variable
                                     rate set */
  word max_num_bits_t;            /* Max. sum of number of bits corresponding
                                     to turbo rates in the variable rate set */
  boolean cs_supported;           /* Concurrent services supported indicator */
  boolean f_sch_ltu_tab_supported;/* F-SCH downloadable LTU tables supported indicator */
  boolean r_sch_ltu_tab_supported;/* R-SCH downloadable LTU tables supported indicator */
#endif /* FEATURE_IS2000_REL_A || FEATURE_IS2000_1X_ADV */

#if defined(FEATURE_IS2000_REL_B) || defined(FEATURE_IS2000_1X_ADV)
  boolean eram_supported;        /* Enhanced rate adaptation mode supported
                                    indicator */
#endif /* FEATURE_IS2000_REL_B || FEATURE_IS2000_1X_ADV */

#if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_IS2000_1X_ADV)
  boolean pdch_chm_supported;      /* PDCH control hold mode supported indicator */
  boolean rev_fch_gating_supported;/* Reverse FCH 1/8 gate mode supported
                                      indicator */
#endif /* FEATURE_IS2000_REL_C || FEATURE_IS2000_1X_ADV */

#if defined(FEATURE_IS2000_1X_ADV)
  boolean rer_mode_supported;
  boolean add_num_sync_id_incl;
  byte add_num_sync_id;
  boolean rsc_mode_supported;
  boolean tkz_mode_supported;
#endif /* FEATURE_IS2000_1X_ADV */

#ifdef FEATURE_IS2000_1X_ADV
  boolean f_sch_early_term_supported;      /* Forward Supplemental Channel early
                                              termination supported indicator */
  boolean rpc_mode_01_supported;           /* Reverse Power control mode '01'
                                              supported indicator */
  boolean f_sch_delayed_arq_supported;     /* FSCH delayed arq supported
                                              indicator */
  boolean r_sch_delayed_arq_supported;     /* RSCH delayed arq supported
                                              indicator */
  boolean so73_op0_supported;              /* Service Option 73 Operating
                                              point '0' supported indicator */
#endif /* FEATURE_IS2000_1X_ADV */

} caii_capa_rec_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* FCH Fields ( see IS-2000 Section 2.7.4.27.1 )                           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  boolean fch_frame_size; /* FCH frame size capability indicator */
  byte for_fch_len;       /* F-FCH configuration information length */
  word for_fch_rc_map;    /* F-FCH RC information */
  byte rev_fch_len;       /* R-FCH configuration information length */
  word rev_fch_rc_map;    /* R-FCH RC information */
} caii_fch_spec_fields_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* DCCH Fields ( see IS-2000 Section 2.7.4.27.2 )                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  byte dcch_frame_size; /* DCCH frame size capability indicator */
  byte for_dcch_len;    /* F-DCCH configuration information length */
  word for_dcch_rc_map; /* F-DCCH RC information */
  byte rev_dcch_len;    /* R-DCCH configuration information length */
  byte rev_dcch_rc_map; /* R-DCCH RC information */
} caii_dcch_spec_fields_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* FOR SCH Fields ( see IS-2000 Section 2.7.4.27.3 )                       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  boolean for_turbo_supported;    /* Forward turbo coding supported indicator */
  byte for_max_turbo_block_size;  /* Forward maximum turbo coding block size */
  boolean for_conv_supported;     /* Forward convolutional coding supported
                                     indicator */
  byte for_max_conv_block_size;   /* Forward maximum convolutional coding
                                     block size */
#ifdef FEATURE_IS2000_REL_A
  boolean for_frame_40_supported; /* Forward 40ms frame indicator */
  boolean for_frame_80_supported; /* Maximum 40ms F-SCH rate */
  byte for_max_rate;              /* Maximum F-SCH rate */
#endif /* FEATURE_IS2000_REL_A */
} caii_for_sch_rec_type;

typedef struct
{
  byte for_sch_len;                 /* F-SCH information length */
  word for_sch_rc_map;              /* F-SCH RC capability */
  byte for_sch_num;                 /* Number of F-SCHs */
  caii_for_sch_rec_type for_sch_recs[CAI_MAX_NUM_FOR_SCH];
} caii_for_sch_spec_fields_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REV SCH Fields ( see IS-2000 Section 2.7.4.27.4 )                       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  boolean rev_turbo_supported;    /* Reverse turbo coding supported indicator */
  byte rev_max_turbo_block_size;  /* Reverse maximum turbo coding block size */
  boolean rev_conv_supported;     /* Reverse convolutiona; coding supported
                                     indicator */
  byte rev_max_conv_block_size;   /* Reverse maximum convolutional coding
                                     block size */
#ifdef FEATURE_IS2000_REL_A
  boolean rev_frame_40_supported; /* Reverse 40ms frame indicator */
  boolean rev_frame_80_supported; /* Reverse 80ms frame indicator */
  byte rev_max_rate;              /* Maximum R-SCH rate */
#endif /* FEATURE_IS2000_REL_A */
} caii_rev_sch_rec_type;

typedef struct
{
  byte rev_sch_len; /* R-SCH information length */
  byte rev_sch_rc_map;              /* R-SCH RC capability */
  byte rev_sch_num;                 /* Number of R-SCHs */
  caii_rev_sch_rec_type rev_sch_recs[CAI_MAX_NUM_REV_SCH];
} caii_rev_sch_spec_fields_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* FOR PDCH Fields                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_IS2000_REL_C
typedef struct
{
  byte ack_delay;             /* Delay for acknowledgement sent on the R-ARQ */
  byte num_arq_chan;          /* Number of physical channels ARQ supported */
  byte for_pdch_len;          /* F-PDCH configuration information length */
  word for_pdch_rc_map;       /* F-PDCH RC information */
  byte ch_config_sup_map_len; /* Channel configuration supported bitmap
                                 length */
  word ch_config_sup_map;     /* Channel configuration supported bitmap */
} caii_for_pdch_spec_fields_type;
#endif /* FEATURE_IS2000_REL_C */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Channel Config Capability Info (See IS2000 2.7.4.27)                    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  caii_rec_hdr_type hdr;
  boolean otd_supported;          /* OTD supported indicator */
  boolean fch_supported;          /* FCH supported indicator */
  caii_fch_spec_fields_type fch_fields;
  boolean dcch_supported;         /* DCCH supported indicator */
  caii_dcch_spec_fields_type dcch_fields;
  boolean for_sch_supported;      /* F-SCH supported indicator */
  caii_for_sch_spec_fields_type for_sch_fields;
  boolean rev_sch_supported;      /* R-SCH supported indicator */
  caii_rev_sch_spec_fields_type rev_sch_fields;
#ifdef FEATURE_IS2000_REL_A
  boolean sts_supported;          /* STS supported indicator */
  boolean threex_cch_supported;   /* 3X common channel supported */
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_B
  boolean ccsh_supported;         /* CCSH supported indicator */
#endif /* FEATURE_IS2000_REL_B */
#ifdef FEATURE_IS2000_REL_C
  boolean for_pdch_supported;     /* F-PDCH supported indicator */
  caii_for_pdch_spec_fields_type for_pdch_fields;
  boolean for_pdch_sch_supported; /* Simultaneous support of F-PDCH and F-SCH
                                     indicator */
#endif /* FEATURE_IS2000_REL_C */

} caii_chan_cfg_capa_info_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Extended Multiplex Option Info (See IS2000 2.7.4.28)                    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  caii_rec_hdr_type hdr;
  byte num_mo_for_fch;                     /* Number of F-FCH MUX options */
  struct
  {
    word mo_for_fch;                       /* F-FCH MUX option */
    byte for_rates_fch;                    /* F-FCH number of bits per frame */
  } mo_for_fch_recs[CAI_MAX_MULT_OPT_FCH_DCCH];

  byte num_mo_rev_fch;                     /* Number of R-FCH MUX options */
  struct
  {
    word mo_rev_fch;                       /* R-FCH MUX option */
    byte rev_rates_fch;                    /* R-FCH number of bits per frame */
  } mo_rev_fch_recs[CAI_MAX_MULT_OPT_FCH_DCCH];

  byte num_mo_for_dcch;                    /* Number of F-DCCH MUX options */
  word mo_for_dcch[CAI_MAX_MULT_OPT_FCH_DCCH]; /* F-DCCH MUX option */
  byte num_mo_rev_dcch;                    /* Number of R-DCCH MUX options */
  word mo_rev_dcch[CAI_MAX_MULT_OPT_FCH_DCCH]; /* R-DCCH MUX option */

  byte num_mo_for_sch;                     /* Number of F-SCH MUX options */
  struct
  {
    byte for_sch_id;                       /* F-SCH ID */
    word mo_for_sch;                       /* F-SCH MUX option */
  } mo_for_sch_recs[CAI_MAX_MULT_OPT_SCH];

  byte num_mo_rev_sch;                     /* Number of R-SCH MUX options */
  struct
  {
    byte rev_sch_id;                       /* R-SCH ID */
    word mo_rev_sch;                       /* R-SCH MUX option */
  } mo_rev_sch_recs[CAI_MAX_MULT_OPT_SCH];

#ifdef FEATURE_IS2000_REL_C
  byte num_mo_for_pdch;                    /* Number of F-PDCH MUX options */
  word mo_for_pdch[CAI_MAX_MULT_OPT_PDCH]; /* F-PDCH MUX option */
#endif /* FEATURE_IS2000_REL_C */

} caii_ext_mult_opt_info_type;

#ifdef FEATURE_IS2000_REL_C
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Signaling Message Integrity Info                                        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  caii_rec_hdr_type hdr;
  byte sig_integrity_sup; /* Signaling message integrity supported */
} caii_sig_msg_int_info_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* UIM ID Info                                                             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  caii_rec_hdr_type hdr;
  byte uim_id_len; /* Length of mobile station User Identity Module identity */
  dword uim_id;    /* Mobile station User Identity Module identity */
} caii_uim_id_info_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Extended Keypad Facility Info Rec                                       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  caii_rec_hdr_type hdr;
  boolean number_info_incl;
  byte number_type;
  byte number_plan;
  byte num_fields;
} caii_ext_keypad_facility_info_fix_type;

typedef struct
{
  byte chari;
} caii_ext_keypad_facility_info_var_type;

typedef struct
{
  caii_ext_keypad_facility_info_fix_type fix;
  caii_ext_keypad_facility_info_var_type var[CAI_EXT_KEYPAD_FACILITY_MAX];
} caii_ext_keypad_facility_info_type;
#endif /* FEATURE_IS2000_REL_C */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* ESN_ME Info                                                             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  caii_rec_hdr_type hdr;
  byte esn_me_len; /* Length of mobile equipment electronic serial number */
  dword esn_me;    /* Mobile equipment electronic serial number */
} caii_esn_me_info_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* MEID Info Rec                                                           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  caii_rec_hdr_type hdr;
  byte meid_len;
  qword meid;             /* MEID is 56 bits */
} caii_meid_info_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* MEID ME Info Rec                                                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  caii_rec_hdr_type hdr;
  byte meid_me_len;
  qword meid_me;             /* MEID is 56 bits */
} caii_meid_me_info_type;

/* User Zone Update Request Record ( see IS-2000 Section 2.7.4.29 ) */
typedef struct
{
  caii_rec_hdr_type hdr;    /* Header information */
  word uzid;                /* User Zone Identifier */
} caii_user_zone_update_req_type;

/* ---------------------------------------------------*/
/* Geo-location information                           */
/*                Section 2.7.4.29                    */
/* ---------------------------------------------------*/
typedef struct
{
  caii_rec_hdr_type hdr;
  byte geo_loc; /* geo location */
} caii_geo_loc_info_type;


#ifdef FEATURE_IS2000_REL_A
/* ---------------------------------------------------*/
/* Band Sub-Class information                         */
/*                Section 2.7.4.30                    */
/* ---------------------------------------------------*/
typedef struct
{
  caii_rec_hdr_type hdr;
  boolean subband[CAI_MAX_BAND_SUBCLASSES];
} caii_band_subclass_type;

/* ---------------------------------------------------*/
/* Global Emergency Call                              */
/*                Section 2.7.4.31                    */
/* ---------------------------------------------------*/

typedef struct
{
  caii_rec_hdr_type hdr;
  boolean     num_incl;
  boolean     digit_mode;   /* Digit mode indicator */
  byte        number_type;  /* Type of number */
  byte        number_plan;  /* Numbering plan  */
  byte        num_char;     /* digits number */
  byte        chari[CAI_ORIG_C_MAX];
  boolean     ms_orig_pos_loc_ind;
} caii_global_emerg_type;


/* ---------------------------------------------------*/
/* Hook Status                                        */
/*                Section 2.7.4.32                    */
/* ---------------------------------------------------*/
typedef struct
{
  caii_rec_hdr_type hdr;
  byte hook_status; /* Hook status Info */
} caii_hook_status_type;

/* ---------------------------------------------------*/
/* QOS Parameters                                     */
/*                Section 2.7.4.33                    */
/* ---------------------------------------------------*/

/* This IE needs to be redefined!!!*/
typedef struct
{
  caii_rec_hdr_type hdr;
  byte qos_parms_len;
  byte qos_parms[CAI_ORIG_QOS_LEN_MAX];
} caii_qos_parms_type;


/* ---------------------------------------------------*/
/* Encryption Capability                              */
/*                Section 2.7.4.34                    */
/* ---------------------------------------------------*/

typedef struct
{
  caii_rec_hdr_type hdr;
  byte sig_enc_sup;   /* signaling encryption supported */
  byte ui_enc_sup;    /* user info encryption supported */
} caii_enc_cap_type;

#endif /* FEATURE_IS2000_REL_A */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        SYNC CHANNEL MESSAGE                             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  byte msg_type;        /* Message type */
  byte p_rev;           /* Protocol revision level */
  byte min_p_rev;       /* Minimum protocol revision level */
  word sid;             /* System identification */
  word nid;             /* Network identification */
  word pilot_pn;        /* Pilot PN sequence offset index */
  qword lc_state;       /* Long code state */
  qword sys_time;       /* System time */
  byte lp_sec;          /* Number of leap seconds that have occurred since
                           the start of System Time */
  byte ltm_off;         /* Offset of local time from System Time */
  boolean daylt;        /* Daylight savings time indicator */
  byte prat;            /* Paging channel data rate */
  word cdma_freq;       /* Frequency assignment */
  word ext_cdma_freq;   /* Extended Frequency Assignment */

#ifdef FEATURE_IS2000_REL_A
  boolean sr1_bcch_non_td_incl;   /* common channel on sr1 supported */
  boolean sr1_non_td_freq_incl; /* non-TD frequency included */
  word sr1_cdma_freq_non_td;    /* frequency assignment for non TD operation */
  byte sr1_brat_non_td;         /* BCCH data rate in non-TD mode for SR1 */
  boolean sr1_crat_non_td;         /* BCCH code rate in non-TD mode for SR1 */
  byte sr1_bcch_code_chan_non_td;/* walsh code for sr1 BCCH in non-TD mode */
  boolean sr1_td_incl;          /* SR1 TD frequency included */
  word sr1_cdma_freq_td;        /* frequency assignment for TD operation */
  byte sr1_brat_td;             /* BCCH data rate in TD mode for SR1 */
  boolean sr1_crat_td;             /* BCCH code rate in TD mode for SR1 */
  byte sr1_bcch_code_chan_td;   /* walsh code for sr1 BCCH in TD mode */
  byte sr1_td_mode;             /* SR1 TD mode (OTD or STS) */
  byte sr1_td_power_level;      /* SR1 TD transmit power level */
  boolean sr3_incl;             /* SR3 information included */
  boolean sr3_center_freq_incl; /* center SR3 frequency information included */
  word sr3_center_freq;         /* center SR3 frequency assignment */
  byte sr3_brat;                /* SR3 BCCH data rate */
  byte sr3_bcch_code_chan;      /* SR3 BCCH walsh code */
  byte sr3_primary_pilot;       /* primary SR3 pilot */
  byte sr3_pilot_power1;        /* dB diff between primary and lower freq SR3 pilot */
  byte sr3_pilot_power2;        /* dB diff between primary and higher freq SR3 pilot */
  boolean ds_incl;              /* DS system info available */
  dword ds_blob;                /* DS info BLOB */
#endif /* FEATURE_IS2000_REL_A */

} caii_sync_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        PAGING CHANNEL MESSAGES                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* GENERIC PAGING CHANNEL MESSAGE                                          */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_pc_hdr_type hdr;  /* Layer 2 header */
} caii_gen_pc_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SYSTEM PARAMETERS MESSAGE (see IS-95A Section 7.7.2.3.2.1)             */

typedef struct
{
  byte msg_type;              /* Message type */
  word pilot_pn;              /* Pilot PN sequence offset index */
  byte config_msg_seq;        /* Configuration message sequence number */
  word sid;                   /* System identification */
  word nid;                   /* Network identification */
  word reg_zone;              /* Registration zone */
  byte total_zones;           /* Number of registration zones
                                 to be retained */
  byte zone_timer;            /* Zone timer length */
  boolean mult_sids;          /* Multiple SID storage indicator */
  boolean mult_nids;          /* Multiple NID storage indicator */
  word base_id;               /* Base station identification */
  byte base_class;            /* Base station class */
  byte page_chan;             /* Number of Paging Channels */
  byte max_slot_cycle_index;  /* Maximum slot cycle index */
  boolean home_reg;           /* Home registration indicator */
  boolean for_sid_reg;        /* SID roamer registration indicator */
  boolean for_nid_reg;        /* NID roamer registration indicator */
  boolean power_up_reg;       /* Power-up registration indicator */
  boolean power_down_reg;     /* Power-down registration indicator */
  boolean parameter_reg;      /* Parameter-change registration indicator */
  byte reg_prd;               /* Registration period */
  dword base_lat;             /* Base station latitude */
  dword base_long;            /* Base station longitude */
  word reg_dist;              /* Registration distance */
  byte srch_win_a;            /* Search window size for the Active Set and
                                 Candidate Set */
  byte srch_win_n;            /* Search window size for the Neighbor Set */
  byte srch_win_r;            /* Search window size for the Remaining Set */
  byte nghbr_max_age;         /* Neighbor Set maximum AGE */
  byte pwr_rep_thresh;        /* Power control reporting threshold */
  byte pwr_rep_frames;        /* Power control reporting frame count */
  boolean pwr_thresh_enable;  /* Threshold report mode indicator */
  boolean pwr_period_enable;  /* Threshold report mode indicator */
  byte pwr_rep_delay;         /* Power report delay */
  boolean rescan;             /* Rescan indicator */
  byte t_add;                 /* Pilot detection threshold */
  byte t_drop;                /* Pilot drop threshold */
  byte t_comp;                /* Active Set versus Candidate Set
                                 comparison threshold */
  byte t_tdrop;               /* Drop timer value */
  boolean ext_sys_parameter;  /* Extended System Parameters Msg indicator */
  boolean ext_nghbr_list;     /* Extended Neighbor List Message indicator */
  boolean gen_nghbr_list;     /* General Neighbor List Message indicator */
  boolean global_redirect;    /* Global Service Redirection Msg indicator */
  boolean is2000_incl;        /* is2000 fields included indicator */
  boolean pri_nghbr_list;     /* Private Neighbor List Message indicator */
  boolean user_zone_id;       /* User Zone Identification Message indicator */
  boolean ext_global_redirect;/* Extended Global Service Redirection
                                  Message indicator */
  boolean ext_chan_list;      /* Extended CDMA Channel List Message
                                  indicator */
#if defined(FEATURE_IS2000_1X_ADV) || defined(FEATURE_1X_TO_LTE) || defined(FEATURE_IS2000_REL_C)
  boolean is2000_rel_c_incl;  /* IS2000 Release C fields included indicator */
  boolean t_tdrop_range_incl; /* Drop timer range value included indicator */
  byte t_tdrop_range;         /* Drop timer range value */
#endif /* FEATURE_IS2000_1X_ADV || FEATURE_1X_TO_LTE || FEATURE_IS2000_REL_C */

#if defined(FEATURE_IS2000_1X_ADV) || defined(FEATURE_1X_TO_LTE)
  boolean is2000_rel_d_incl;        /* IS2000 Release D fields included indicator */
  boolean neg_slot_cycle_index_sup; /* Negative slot cycle index supported indicator */
#endif /* FEATURE_IS2000_1X_ADV || FEATURE_1X_TO_LTE */

#if defined (FEATURE_IS2000_1X_ADV) || defined(FEATURE_1X_TO_LTE)
  boolean is2000_rel_e_incl;        /* IS2000 Release E fields included indicator */
  boolean rc_bitmap_capability_ext_allowed; /* RC bitmap and Capability Record
                                               Extension Indicator */
#endif /* FEATURE_IS2000_1X_ADV || FEATURE_1X_TO_LTE */

#ifdef FEATURE_1X_TO_LTE
  boolean crrm_msg_ind; /* CRRM_MSG_IND */
  byte num_opt_msg_bits; /* NUM_OPT_MSG_BITS */
  boolean ap_pilot_info; /* AP_PILOT_INFO */
  boolean ap_idt; /* AP_IDT */
  boolean ap_id_text; /* AP_ID_TEXT */
  boolean gen_ovhd_inf_ind; /* GEN_OVHD_INF_IND */
  boolean fd_chan_lst_ind; /* FD_CHAN_LST_IND */
  boolean atim_ind; /* ATIM_IND */
  boolean freq_gen_ovhd_inf_ind; /* FREQ_GEN_OVHD_INF_IND */
  byte reserved; /* RESERVED */
  byte appim_period_index; /* APPIM_PERIOD_INDEX */
  byte gen_ovhd_cycle_index; /* GEN_OVHD_CYCLE_INDEX */
  byte atim_cycle_index; /* ATIM_CYCLE_INDEX */
  boolean add_loc_info_inc; /* ADD_LOC_INFO_INCL */
  byte loc_unc_h; /* LOC_UNC_H */
  word height; /* HEIGHT */
  byte loc_unc_v; /* LOC_UNC_V */
  boolean add_loc_gem_allowed; /* ADD_LOC_GEM_ALLOWED */
#endif /* FEATURE_1X_TO_LTE */

} caii_sysparm_type;

#ifdef FEATURE_1X_TO_LTE

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Alternative Technologies Information Message (see C.S0005-E)            */

typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  byte num_radio_interface;  /* Number of radio interface */

  struct
  {
    byte radio_interface_type;  /* Radio interface type */
    word radio_interface_len;  /* Radio interface length */

    struct
    {
      boolean pri_inc; /* Priority included */
      byte pri;     /* Serving Priority */
      byte thresh_srv; /* Thresh Serving */
      boolean perearfcn_params_inc; /*PerEARFCNParamsIncluded */
      byte rx_lev_min_eutra_common; /* RxLevMinEUTRACommon */
      byte pe_max_common;  /* PEMaxCommon */
      boolean rx_lev_min_eutra_off_common_inc; /* RxLevMinEUTRAOffsetCommonIncl */
      byte rx_lev_min_eutra_off_common; /* RxLevMinEUTRAOffsetCommon */
      boolean max_resel_timer_inc; /* MaxReselectionTimerIncluded */
      byte max_resel_timer; /* MaxReselectionTimer */
      boolean srch_backoff_timer_inc; /* SearchBackOffTimerIncluded */
      byte min_meas_back_off; /* MinMeasurementBackoff */
      byte max_meas_back_off; /* MaxMeasurementBackoff */
      boolean plmnid_inc; /* PLMNIDIncluded */
      byte num_eutra_freq; /* NumEUTRAFrequencies */

      struct
      {
        lte_earfcn_t earfcn; /* EARFCN */
        byte earfcn_pri; /* EARFCNPriority */
        byte thresh_x; /* ThreshX */
        byte rx_lev_min_eutra; /* RxLevMinEUTRA */
        byte pe_max; /* PEMax */
        boolean rx_lev_min_eutra_offset_inc; /* RxLevMinEUTRAOffset Incl */
        byte rx_lev_min_eutra_offset; /* RxLevMinEUTRAOffset */
        byte meas_band_width; /* MeasurementBandwidth */
        byte plmn_same_as_prev_chn; /* PLMNSameAsPreviousChannel */
        byte num_plmnids; /* NumPLMNIDs */

        struct
        {
          dword plmn; /* plmn id */
        }plmnid[MAX_NUM_PLMNIDS];

      }eutra_freq[MAX_NUM_EUTRA_FREQ];

    #ifdef FEATURE_1X_SPLMN_SUPPORT
      boolean splmn_inc;  /* ServingNetworkPLMNIncl */
      byte num_splmnids; /* NumServingPLMNIDs */
    
      struct
      {
        dword splmn; /* splmn id */
      }splmnid[MAX_NUM_SPLMNIDS];
    
      boolean rsrq_parms_inc; /* RSRQParamtersIncluded */
      byte qual_min_eutra_common; /* QualMinEUTRACommon */
      boolean qual_min_eutra_offset_common_inc; /* QualMinEutraOffsetCommonInc */
      byte qual_min_eutra_offset_common; /* QualMinEUTRAOffsetCommon */
      
      struct
      {
        byte thresh_xq; /* ThreshXQ */
        byte qual_min_eutra; /* QualMinEUTRA */
        boolean qual_min_eutra_offset_inc; /* QualMinEutraOffsetInc */
        byte qual_min_eutra_offset; /*QualMinEUTRAOffset */
      } qual_eutra_freq[MAX_NUM_EUTRA_FREQ];
      
    #endif /* FEATURE_1X_SPLMN_SUPPORT */
      
      byte reserved;

    }radio_interface; /* Radio interface type specific fields */

  } radio_interface_rec[MAX_NUM_RADIO_INTERFACE];

} caii_tc_alt_tech_info_msg_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Alternative Technologies Information Message (see C.S0005-E)            */

typedef struct
{
  byte msg_type;             /* Message type */
  word pilot_pn;             /* Pilot PN sequence offset index */
  byte config_msg_seq;       /* Configuration message type */
  byte num_radio_interface;  /* Number of radio interface */

  struct
  {
    byte radio_interface_type;  /* Radio interface type */
    word radio_interface_len;  /* Radio interface length */

    struct
    {
      boolean pri_inc; /* Priority included */
      byte pri;     /* Serving Priority */
      byte thresh_srv; /* Thresh Serving */
      boolean perearfcn_params_inc; /*PerEARFCNParamsIncluded */
      byte rx_lev_min_eutra_common; /* RxLevMinEUTRACommon */
      byte pe_max_common;  /* PEMaxCommon */
      boolean rx_lev_min_eutra_off_common_inc; /* RxLevMinEUTRAOffsetCommonIncl */
      byte rx_lev_min_eutra_off_common; /* RxLevMinEUTRAOffsetCommon */
      boolean max_resel_timer_inc; /* MaxReselectionTimerIncluded */
      byte max_resel_timer; /* MaxReselectionTimer */
      boolean srch_backoff_timer_inc; /* SearchBackOffTimerIncluded */
      byte min_meas_back_off; /* MinMeasurementBackoff */
      byte max_meas_back_off; /* MaxMeasurementBackoff */
      boolean plmnid_inc; /* PLMNIDIncluded */
      byte num_eutra_freq; /* NumEUTRAFrequencies */

      struct
      {
        lte_earfcn_t earfcn; /* EARFCN */
        byte earfcn_pri; /* EARFCNPriority */
        byte thresh_x; /* ThreshX */
        byte rx_lev_min_eutra; /* RxLevMinEUTRA */
        byte pe_max; /* PEMax */
        boolean rx_lev_min_eutra_offset_inc; /* RxLevMinEUTRAOffset Incl */
        byte rx_lev_min_eutra_offset; /* RxLevMinEUTRAOffset */
        byte meas_band_width; /* MeasurementBandwidth */
        byte plmn_same_as_prev_chn; /* PLMNSameAsPreviousChannel */
        byte num_plmnids; /* NumPLMNIDs */

        struct
        {
          dword plmn; /* plmn id */
        }plmnid[MAX_NUM_PLMNIDS];

      }eutra_freq[MAX_NUM_EUTRA_FREQ];

    #ifdef FEATURE_1X_SPLMN_SUPPORT
      boolean splmn_inc;  /* ServingNetworkPLMNIncl */
      byte num_splmnids; /* NumServingPLMNIDs */

      struct
      {
        dword splmn; /* splmn id */
      }splmnid[MAX_NUM_SPLMNIDS];

      boolean rsrq_parms_inc; /* RSRQParamtersIncluded */
      byte qual_min_eutra_common; /* QualMinEUTRACommon */
      boolean qual_min_eutra_offset_common_inc; /* QualMinEutraOffsetCommonInc */
      byte qual_min_eutra_offset_common; /* QualMinEUTRAOffsetCommon */
      
      struct
      {
        byte thresh_xq; /* ThreshXQ */
        byte qual_min_eutra; /* QualMinEUTRA */
        boolean qual_min_eutra_offset_inc; /* QualMinEutraOffsetInc */
        byte qual_min_eutra_offset; /*QualMinEUTRAOffset */
      } qual_eutra_freq[MAX_NUM_EUTRA_FREQ];

    #endif /* FEATURE_1X_SPLMN_SUPPORT */

    byte reserved;

    }radio_interface; /* Radio interface type specific fields */

  } radio_interface_rec[MAX_NUM_RADIO_INTERFACE];

} caii_alt_tech_info_msg_type;

#endif /* FEATURE_1X_TO_LTE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* ACCESS PARAMETERS MESSAGE (see IS-95A Section 7.7.2.3.2.2)             */

typedef struct
{
  byte msg_type;       /* Message type */
  word pilot_pn;       /* Pilot PN sequence offset index */
  byte acc_msg_seq;    /* Access parameters message sequence number */
  byte acc_chan;       /* Number of Access Channels */
  byte nom_pwr;        /* Nominal transmit power offset */
  byte init_pwr;       /* Initial power offset for access */
  byte pwr_step;       /* Power increment */
  byte num_step;       /* Number of access probes */
  byte max_cap_sz;     /* Maximum Access Channel message capsule size */
  byte pam_sz;         /* Access Channel preamble length */
  byte psist_0_9;      /* Persistence value for access overload classes
                          0 through 9 */
  byte psist_10;       /* Persistence value for Access Overload Class 10 */
  byte psist_11;       /* Persistence value for Access Overload Class 11 */
  byte psist_12;       /* Persistence value for Access Overload Class 12 */
  byte psist_13;       /* Persistence value for Access Overload Class 13 */
  byte psist_14;       /* Persistence value for Access Overload Class 14 */
  byte psist_15;       /* Persistence value for Access Overload Class 15 */
  byte msg_psist;      /* Persistence modifier for Access Channel attempts
                          for message transmissions */
  byte reg_psist;      /* Persistence modifier for Access Channel attempts
                          for registrations */
  byte probe_pn_ran;   /* Time randomization for Access Channel probes */
  byte acc_tmo;        /* Acknowledgement timeout */
  byte probe_bkoff;    /* Access Channel probe backoff range */
  byte bkoff;          /* Access Channel probe sequence backoff range */
  byte max_req_seq;    /* Maximum number of acces probe sequences for an
                          Access Channel request */
  byte max_rsp_seq;    /* Maximum number of access probe sequences for an
                          Access Channel response */
  byte auth;           /* Authentication mode */
  dword rand;          /* Random challenge value - only valid if auth
                          field is set to 1 */
  boolean nom_pwr_ext; /* Extended nominal transmit power */
#ifdef FEATURE_IS2000_REL_A
  boolean psist_emg_incl;/* Emergency persistence included*/
  byte psist_emg;     /* Persistence value for emergency
                            access overload classes 0 through 9 */
  boolean acct_incl;     /* ACCT info included */
  boolean acct_incl_emg; /* EMG call included indicator */
  boolean acct_aoc_bitmap_incl;  /* Access Overload info included */
  boolean acct_so_incl;  /* SO info included */
  byte num_acct_so;      /* Number of SO for ACCT */

  struct
  {
    byte acct_aoc_bitmap1; /* AOC bitmap */
    word acct_so;          /* ACCT SOs */
  } acct_so_info[CAI_MAX_ACCT_SO_NUM];

  boolean acct_so_grp_incl; /* SO group indicator */
  byte num_acct_so_grp;     /* Number of SO groups */

  struct
  {
    byte acct_aoc_bitmap2; /* AOC bitmap */
    byte acct_so_grp;      /* SO group */
  } acct_so_grp_info[CAI_MAX_ACCT_SO_GRP_NUM];
#endif /* FEATURE_IS2000_REL_A */
} caii_accparm_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* EXTENDED NEIGHBOR LIST MESSAGE (see J-STD-008 Section 3.7.2.3.2.14)     */

typedef struct
{
  byte msg_type;         /* Message type */
  word pilot_pn;         /* Pilot PN sequence offset index */
  byte config_msg_seq;   /* Configuration message sequence number */
  byte pilot_inc;        /* Pilot PN sequence offset index increment */
  word num_nghbrs;       /* Number of neighbors listed in the nghbr array */
  struct
  {
    byte nghbr_config;        /* Neighbor configuration */
    word nghbr_pn;            /* Neighbor pilot PN sequence offset index */
    byte search_pri;          /* Pilot Channel search priority */
    boolean freq_incl;        /* Frequency included indicator */
    byte nghbr_band;          /* Neighbor band class */
    word nghbr_freq;          /* Neighbor frequency assignment */
  } nghbr[CAI_EXT_NGHBR_MAX];
} caii_ext_nghbr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* NEIGHBOR LIST MESSAGE (see IS-95A Section 7.7.2.3.2.3)                 */

typedef struct
{
  byte msg_type;         /* Message type */
  word pilot_pn;         /* Pilot PN sequence offset index */
  byte config_msg_seq;   /* Configuration message sequence number */
  byte pilot_inc;        /* Pilot PN sequence offset index increment */
  word num_nghbrs;       /* Number of neighbors listed in the nghbr array */
  struct
  {
    byte nghbr_config;        /* Neighbor configuration */
    word nghbr_pn;            /* Neighbor pilot PN sequence offset index */
  } nghbr[CAI_NGHBR_MAX];
} caii_nghbr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CDMA CHANNEL LIST MESSAGE (see IS-95A Section 7.7.2.3.2.4)             */

typedef struct
{
  byte msg_type;         /* Message type */
  word pilot_pn;         /* Pilot PN sequence offset index */
  byte config_msg_seq;   /* Configuration message sequence number */
  word num_freq;         /* Number of entries in the cdma_freq array */
  word cdma_freq[CAI_CHNLIST_MAX]; /* CDMA Channel frequency assignments */
} caii_chnlist_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Generic Page Body for Slotted Page Message and Page Message   */

typedef struct
{
  byte msg_seq;            /* Message sequence number */
  boolean ext_addr;        /* Extra address indicator */
  dword min1;              /* First part of mobile identification number */
  word min2;               /* Second part of mobile identification number */
  boolean special_service; /* Special service option indicator */
  word service_option;     /* Service option */
} caii_page_body_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SLOTTED PAGE MESSAGE (see IS-95A Section 7.7.2.3.2.5)                  */

typedef struct
{
  byte msg_type;          /* Message type */
  byte config_msg_seq;    /* Configuration message sequence number */
  byte acc_msg_seq;       /* Access parameters message sequence number */
  boolean more_pages;     /* More slotted pages to follow indicator */
  word num_pages;         /* Number of pages in the pages array */
  caii_page_body_type pages[CAI_SLT_PAGE_MAX];
} caii_slt_page_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PAGE MESSAGE (see IS-95A Section 7.7.2.3.2.6)                          */

typedef struct
{
  byte msg_type;          /* Message type */
  byte config_msg_seq;    /* Configuration message sequence number */
  byte acc_msg_seq;       /* Access parameters message sequence number */
  word num_pages;         /* Number of pages in the pages array */
  caii_page_body_type pages[CAI_PAGE_MAX];
} caii_page_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PAGING CHANNEL ORDER MESSAGE (IS-95A Sections 7.7.2.3.2.7 and 7.7.4)   */

/*  Order without order qualifier */
typedef struct
{
  caii_pc_hdr_type hdr;     /* Paging Channel common header fields */
  byte order;               /* Order code */

} caii_pc_gen_ord_type;

/*  Order With Order Qualifier type                                        */
typedef struct
{
  caii_pc_hdr_type hdr;     /* Paging Channel common header fields */
  byte order;               /* Order code */
  byte ordq;                /* Order qualification code */
} caii_pc_ordq_type;

/* Base Station Challenge Confirmation Order */
typedef struct
{
  caii_pc_hdr_type hdr;     /* Paging Channel common header fields */
  byte order;               /* Order code */
  byte ordq;                /* Order qualification code */
  dword authbs;             /* Challenge response */
} caii_pc_bs_chal_type;

/* Registration Accepted Order */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte order;            /* Order field */
  byte ordq;             /* Order qualification code */
  byte roam_indi;        /* Roaming Display Indication */
#ifdef FEATURE_IS2000_REL_A
  byte c_sig_encrypt_mode;
  byte key_size;
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_C
  boolean msg_int_info_incl;
  boolean change_keys;
  boolean use_uak;
#endif /* FEATURE_IS2000_REL_C */
} caii_pc_reg_acc_type;

typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte order;            /* Order field */
  byte ordq;             /* Order qualification code */
  byte retry_type;
  byte retry_delay;
} caii_pc_retry_type;

/* Base Station Reject Order */
typedef struct
{
  caii_pc_hdr_type hdr;   /* Paging Channel common header fields */
  byte order;             /* Order field */
  byte ordq;              /* Order qualification code */
#ifdef FEATURE_IS2000_REL_C
  byte reject_reason;     /* Reason of rejection */
  byte rejected_msg_type; /* Message type of the rejected message */
  byte rejected_msg_seq;  /* Layer 2 message sequence number of the rejected
                             message */
#endif /* FEATURE_IS2000_REL_C */
} caii_bs_reject_type;

/* Union of structures for the Paging Channel Order Message */
typedef union
{
  caii_pc_gen_ord_type           gen;
  caii_pc_ordq_type              ordq;
  caii_pc_bs_chal_type           chal;
  caii_pc_reg_acc_type           regacc;
  caii_pc_retry_type             retry;
  caii_bs_reject_type            bsrej;

} caii_pc_ord_var_type;

typedef struct
{
  byte msg_type;         /* Message type */
  word num_ords;         /* Number of orders in the message */
  caii_pc_ord_var_type ords[CAI_PC_ORDER_MAX];  /* Array of orders */
} caii_pc_ord_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CHANNEL ASSIGNMENT MESSAGE (see IS-95A Section 7.7.2.3.2.8)             */

/* Base of Channel Assignment Message */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
} caii_chnasn_gen_type;

/* Channel Assignment Message with Assign mode = 000 */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
  boolean freq_incl;     /* Frequency included indicator */
  byte code_chan;        /* Code channel */
  word cdma_freq;        /* Frequency assignment (only present if
                            freq_incl field is set to 1) */
  byte frame_offset;     /* Frame offset */
  byte encrypt_mode;     /* Message encryption mode */
#ifdef FEATURE_IS2000_REL_A
  byte d_sig_encrypt_mode; /* DSCH Encrypt Mode */
  byte enc_key_size;       /* Encryption key size */
  boolean c_sig_encrypt_mode_incl; /* Common Channel Encrypt Mode */
  byte c_sig_encrypt_mode; /* Common Channel Encrypt Mode */
#endif /* FEATURE_IS2000_REL_A */

} caii_chnasn_am0_type;

/* Channel Assignment Message with Assign mode = 001 */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
  boolean respond;       /* Respond on new Access Channel indicator */
  boolean freq_incl;     /* Frequency included indicator */
  word cdma_freq;        /* Frequency assignment (only present if
                            freq_incl field is set to 1) */
  word num_pilots;       /* Number of entries in pilot_pn array */
  word pilot_pn[CAI_CHNASN_AM1_MAX]; /* Pilot PN sequence offset index */
} caii_chnasn_am1_type;

/* Channel Assignment Message with Assign mode = 010 */
typedef struct
{
  caii_pc_hdr_type hdr;   /* Paging Channel common header fields */
  byte assign_mode;       /* Assignment mode */
  boolean respond;        /* Respond on analog control channel indicator */
  byte analog_sys;        /* System indicator */
  boolean use_analog_sys; /* Use analog system indicator */
  byte band_class;        /* Band class */
} caii_chnasn_am2_type;

/* Channel Assignment Message with Assign mode = 011 */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
  word sid;              /* System identification of the analog system */
  byte vmac;             /* Voice mobile station attenuation code */
  word analog_chan;      /* Voice channel number */
  byte scc;              /* SAT color code */
  boolean mem;           /* Message encryption mode indiator */
  byte an_chan_type;     /* Analog voice channel type */
  byte dscc_msb;         /* DSCC most significant bit */
  byte band_class;       /* Band class */
} caii_chnasn_am3_type;

/* Channel Assignment Message with Assign mode = 100 */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
  boolean freq_incl;     /* Frequency included indicator */
  boolean bypass_alert_answer;  /* Bypass Indicator */
  byte default_config;   /* Default configuration */
  byte grant_mode;       /* Granted mode */
  byte code_chan;        /* Code channel */
  byte frame_offset;     /* Frame offset */
  byte encrypt_mode;     /* Message encryption mode */
  byte band_class;       /* Band class (only present if freq_incl
                            is set to 1) */
  word cdma_freq;        /* Frequency assignment (only present if
                            freq_incl field is set to 1) */
#ifdef FEATURE_IS2000_REL_A
  byte d_sig_encrypt_mode; /* DSCH Encrypt Mode */
  byte enc_key_size;       /* Encryption key size */
  boolean c_sig_encrypt_mode_incl; /* Common Channel Encrypt Mode */
  byte c_sig_encrypt_mode; /* Common Channel Encrypt Mode */
#endif /* FEATURE_IS2000_REL_A */
} caii_chnasn_am4_type;

/* Channel Assignment Message with Assign mode = 101 */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
  boolean respond;       /* Respond on new Access Channel indicator */
  boolean freq_incl;     /* Frequency included indicator */
  byte band;             /* Band class (only present if freq_incl
                            is set to 1) */
  word cdma_freq;        /* Frequency assignment (only present if
                            freq_incl field is set to 1) */
  word num_pilots;       /* Number of entries in pilot_pn array */
  word pilot_pn[CAI_CHNASN_AM5_MAX]; /* Pilot PN sequence offset index */
} caii_chnasn_am5_type;

/* Union of Channel Assignment messages */
typedef union
{
  caii_chnasn_gen_type gen;
  caii_chnasn_am0_type am0;
  caii_chnasn_am1_type am1;
  caii_chnasn_am2_type am2;
  caii_chnasn_am3_type am3;
  caii_chnasn_am4_type am4;
  caii_chnasn_am5_type am5;
} caii_ca_ord_type;

/* Channel Assignment message type */

typedef struct
{
  byte msg_type;     /* Message type */
  word num_chns;     /* Number of channel assignments in the chns array */
  caii_ca_ord_type chns[CAI_CHN_ASN_MAX];
} caii_chnasn_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* TMSI ASSIGNMENT MESSAGE (see J-STD-008 Section 3.7.2.3.2.19)            */

typedef struct  /* TMSI Assignment Message data */
{
  byte zone_len;        /* TMSI zone length      */
  caii_tmsi_type info;  /* TMSI zone & TMSI code */
  dword exp_time;       /* TMSI expiration time  */
} caii_tmsi_data_type;

typedef struct  /* TMSI Assignment Message - Paging Channel */
{
  byte msg_type;             /* Message type */
  caii_pc_hdr_type hdr;      /* Paging Channel common header fields */
  caii_tmsi_data_type data;  /* TMSI Assignment Message data */
} caii_pc_tmsi_asgn_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* DATA BURST MESSAGE (see IS-95A Section 7.7.2.3.2.9)                    */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte msg_number;       /* Message number */
  byte burst_type;       /* Data burst type */
  byte num_msgs;         /* Number of messages in the data burst stream */
  byte num_fields;       /* Number of characters in this message */
  byte chari[CAI_PC_BURST_MAX]; /* Characters */
} caii_pc_burst_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* AUTHENTICATION CHALLENGE MESSAGE (see IS-95A Section 7.7.2.3.2.10)     */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  dword randu;           /* Random challenge data */
} caii_pc_auth_ch_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SSD UPDATE MESAGE (see IS-95A Section 7.7.2.3.2.11)                    */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  qword randssd;         /* Random data for the computation of SSD */
} caii_pc_ssd_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* FEATURE NOTIFICATION MESSAGE (see IS-95A Section 7.7.2.3.2.12)         */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  boolean release;       /* Origination completion indicator */
  byte num_recs;         /* Number of information records in message */
  dword recs[CAII_MAX_INFO_REC_DWORDS];
    /* Records - consecutive Paging Channel Information Records -
       use caii_rec_hdr type to parse different records
       Use dword to ensure alignment */
} caii_pc_feature_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* EXTENDED SYSTEM PARAMETERS MESSAGE (see IS-95A Section 7.7.2.3.2.13)    */

#if defined(FEATURE_1X_TO_LTE)
typedef struct
{
  byte cand_band_class;       /* Candidate band class */
  boolean subclass_info_incl; /* Band subclass info included */
  byte subclass_rec_len;      /* Band subclass subrecord length */
  boolean band_subclass_ind[CAI_EXT_SYSPARM_BAND_SUBCLASS_MAX]; /* Band subclass indicator */
} caii_cand_band_info_type;
#endif /* FEATURE_1X_TO_LTE */

typedef struct
{
  byte msg_type;             /* Message type */
  word pilot_pn;             /* Pilot PN sequence offset index */
  byte config_msg_seq;       /* Configuration message sequence number */
  boolean delete_for_tmsi;   /* Delete foreign TMSI */
  byte pref_msid_type;       /* Preferred Access Channel MSID type */
  word mcc;                  /* Mobile country code */
  byte imsi_11_12;           /* 11th and 12th digits of the IMSI */
  byte tmsi_zone_len;        /* TMSI zone length */
  qword tmsi_zone;           /* TMSI zone */
  byte bcast_index;          /* Broadcast slot cycle index */
  boolean is95b_incl;             /* is95b fields included indicator */
  boolean imsi_t_supported;       /* IMSI_T_SUPPORTED flag */
  byte p_rev;                     /* P_REV supported by BS */
  byte min_p_rev;                 /* Minimum P_REV required for access */
  byte soft_slope;                /* Soft slop for soft handoff */
  byte add_intercept;             /* For IS-95B soft handoff */
  byte drop_intercept;            /* For IS-95B soft handoff */
  byte packet_zone_id;            /* Packet Data service zone identifier */
  byte max_num_alt_so;            /* Max. Num. of alternative service options */
  boolean reselect_included;      /* System Reselection parameters included */
  byte ec_thresh;                 /* Pilot Power Threshold */
  byte ec_io_thresh;              /* Pilot Ec/Io Threshold */
  boolean pilot_report;           /* Pilot reporting indicator */
  boolean nghbr_set_entry_info;   /* Access entry handoff info included */
  boolean acc_ent_ho_order;       /* Entry handoff permitted indicator */
  boolean nghbr_set_access_info;  /* Neighbor set access handoff included */
  boolean access_ho;              /* Access handoff permitted indicator */
  boolean access_ho_msg_rsp;      /* AHO permitted for msg response */
  boolean access_probe_ho;        /* APHO permitted indicator */
  boolean acc_ho_list_upd;        /* APHO List Update permitted indicator */
  boolean acc_probe_ho_other_msg; /* APHO for msg other than ORIG/PR */
  byte max_num_probe_ho;          /* Times limit for access probe handoff */
  byte nghbr_set_size;            /* Size of neighbor set */
  boolean access_entry_ho[MAX_NGHBR_SET_SIZE];   /* Entry handoff permitted */
  boolean access_ho_allowed[MAX_NGHBR_SET_SIZE]; /* AHO and APHO permitted */
  boolean broadcast_gps_asst;     /* Broadcast GPS Assist indicator */
  boolean qpch_supported;         /* Quick Paging Channel Supported indicator */
  byte num_qpch;                  /* Number of Quick Paging Channels */
  boolean qpch_rate;              /* Quick Paging Channel Indicator rate */
  byte qpch_power_level_page;     /* Quick Paging Channel paging indicator
                                     transmit power level */
  boolean qpch_cci_supported;     /* Quick Paging Channel configuration change
                                     indicator supported */
  byte qpch_power_level_config;   /* Quick Paging Channel configuration change
                                      indicator transmit power level */
  boolean sdb_supported;          /* Short Data Burst supported indicator */
  byte rlgain_traffic_pilot;      /* Gain Adjustment of the Reverse Traffic
                                     Channel relative to the Reverse Pilot
                                     Channel for Radio Configurations greater
                                     than 2 */
  boolean rev_pwr_cntl_delay_incl;
  byte rev_pwr_cntl_delay;

#ifdef FEATURE_IS2000_REL_A
  boolean auto_msg_supported;       /* autonomous message supported indicator */
  byte auto_msg_interval;           /* autonomous message interval */
  boolean mob_qos;                  /* permission indicator for QoS in Origination message */
  boolean enc_supported;            /* encryption fields included */
  byte sig_encrypt_sup;             /* signaling encryption supported indicator */
  byte ui_encrypt_sup;              /* user information encryption supported indicator */
  boolean use_sync_id;              /* use sync_id indicator */
  boolean cs_supported;             /* concurrent service supported */
  boolean bcch_supported;           /* BCCH supported indicator */
  boolean ms_init_pos_loc_sup_ind;  /* MS initiated position location supported */
  boolean pilot_info_req_supported; /* pilot information request supported indicator */
#endif /* FEATUER_IS2000_REL_A */

#if defined(FEATURE_IS2000_REL_B)
  boolean qpch_bi_supported;        /* QPCH broadcast indicator supported */
  boolean qpch_power_level_bcast;   /* QPCH broadcast indicator transmit power level */
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_B
  byte band_class_info_req;         /* Band class information request indicator */
  byte alt_band_class;              /* Alternate CDMA band class supported by the base station */
  boolean cdma_off_time_rep_sup_ind;/* CDMA off time rport supported indicator */
  boolean cdma_off_time_rep_threshold_unit; /* CDMA off time report unit */
  byte cdma_off_time_rep_threshold; /* CDMA off time report threshold */
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C
  boolean chm_supported;            /* Control Mode Mode supported indicator */
  boolean release_to_idle_ind;      /* Release to idle allowed indicator */
  boolean reconnect_msg_ind;        /* Reconnect Message supported indicator */
  boolean msg_integrity_sup;        /* Message integrity supported indicator */
  boolean sig_integrity_sup_incl;   /* Signaling message integrity information
                                       included indicator */
  byte sig_integrity_sup;           /* Signaling integrity algorithm supported
                                       by the base station */
  boolean for_pdch_supported;       /* Forward Packet Data Channel supported
                                       indicator */
  boolean pdch_chm_supported;       /* PDCH Control Hold Mode supported
                                       indicator */
  boolean pdch_parms_incl;          /* Forward Packet Data Channel related
                                       parameters included indicator */
  boolean for_pdch_rlgain_incl;     /* F-PDCH parameters related to reverse
                                       link adjustment gains included indicator */
  byte rlgain_ackch_pilot;          /* R-ACKCH to pilot adjustment gain */
  byte rlgain_cqich_pilot;          /* R-CQICH to pilot adjustment gain */
  byte num_soft_switching_frames;   /* Number of frames for R-CQICH soft
                                       switching */
  byte num_softer_switching_frames; /* Number of frames for R-CQICH softer
                                       switching */
  byte num_soft_switching_slots;    /* Number of slots per frame for R-CQICH
                                          soft switching */
  byte num_softer_switching_slots;  /* Number of slots per frame for R-CQICH
                                          softer switching */
  byte pdch_soft_switching_delay;   /* F-PDCH soft switching delay */
  byte pdch_softer_switching_delay; /* F-PDCH softer switching delay */
  byte walsh_table_id;              /* The index of the Walsh Table used */
  byte num_pdcch;                   /* The number of Packet Data Control */
  byte for_pdcch_walsh[CAI_MAX_NUM_FOR_PDCCH]; /* Forward packet data control
                                                  walsh code */
  boolean imsi_10_incl;             /* IMSI 10 included */
  byte imsi_10;                     /* The least significant digit of MNC when MNC
                                       is a 3-digit number */
#endif /* FEATURE_IS2000_REL_C */

} caii_ext_sysparm_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* STATUS REQUEST MESSAGE (see IS-95-A TSB)                                */

#define         CAI_REC_TYP_MAX 15

typedef struct
{
  byte msg_type;         /* Message type */
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte qual;             /* Qualification information */
  byte band;             /* Band class qualifier */
  byte op_mode;          /* Operating mode qualifier */
  byte num_fields;       /* Number of active elements in rec_typ */
  byte rec_typ[ CAI_REC_TYP_MAX ]; /* List of requested rec types */
} caii_pc_streq_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* SERVICE REDIRECTION MESSAGE (see IS-95A Section 7.7.2.3.2.16)           */

/* Service Redirection Record Type = '00000001' */
/* Historically, we share the following structures in our code. However,
   in IS-95B, cai_redirect_rec1_type for Service Redirection and Global
   Service Redirection is slightly different. In SRM, 5 bits are reserved.
   In GSRM, the previously reserved 5 bits are defined as max_redirect_delay.
   To minimize impact to the Call Processing code, we will still share these
   structures, but the parser will take the responsibility of clearing
   the max_redirect_delay field for the SRM. */

typedef struct
{
  word expected_sid;       /* Expected SID */
  boolean ignore_cdma;     /* Ignore CDMA Available indicator */
  byte sys_ordering;       /* System ordering */
  byte max_redirect_delay; /* Max delay upon redirection. This field is
                              only for Global Service Redir, not for SRM */
} caii_redirect_rec1_type;

/* Service Redirection Record Type = '00000002' */

typedef struct
{
  byte band_class;         /* Band class */
  word expected_sid;       /* Expected SID */
  word expected_nid;       /* Expected NID */
  byte num_chans;          /* Number of CDMA channels */
  word cdma_chan[CAI_MAX_CDMA_CHANS];

} caii_redirect_rec2_type;

typedef struct
{
  word expected_aid;       /* Expected AID */
  boolean ignore_cdma;     /* Ignore CDMA available indicator */
} caii_redirect_rec4_type;

typedef struct
{
  byte msg_type;             /* Message type */
  caii_pc_hdr_type hdr;      /* Paging Channel common header fields */
  boolean return_if_fail;    /* Return if fail indicator */
  boolean delete_tmsi;       /* Delete TMSI indicator */
  boolean redirect_type;     /* Redirect indicator for IS-95B */
  byte record_type;          /* Redirection record type */
  byte record_len;           /* Redirection record length */
  union
  {
    caii_redirect_rec1_type rec1;
    caii_redirect_rec2_type rec2;
    caii_redirect_rec4_type rec4;
  } redir;
} caii_serv_redirect_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* GENERAL PAGE MESSAGE (see IS-95A Section 7.7.2.3.2.17)                  */

/* PAGE_CLASS = '00' and PAGE_SUBCLASS = '00' */
typedef struct
{
  byte msg_seq;             /* Message sequence number */
  qword imsi_s;             /* Last ten digits of the IMSI */
  boolean special_service;  /* Special service option indicator */
  word service_option;      /* Service option (only present if
                               special_service field is set to 1) */
} caii_pc_0_sc_0_type;

/* PAGE_CLASS = '00' and PAGE_SUBCLASS = '01' */
typedef struct
{
  byte msg_seq;             /* Message sequence number */
  byte imsi_11_12;          /* The 11th and 12th digits of the IMSI */
  qword imsi_s;             /* Last ten digits of the IMSI */
  boolean special_service;  /* Special service option indicator */
  word service_option;      /* Service option (only present if
                               special_service field is set to 1) */
} caii_pc_0_sc_1_type;

/* PAGE_CLASS = '00' and PAGE_SUBCLASS = '10' */
typedef struct
{
  byte msg_seq;             /* Message sequence number */
  word mcc;                 /* Mobile country code */
  qword imsi_s;             /* Last ten digits of the IMSI */
  boolean special_service;  /* Special service option indicator */
  word service_option;      /* Service option (only present if
                               special_service field is set to 1) */
} caii_pc_0_sc_2_type;

/* PAGE_CLASS = '00' and PAGE_SUBCLASS = '11' */
typedef struct
{
  byte msg_seq;             /* Message sequence number */
  word mcc;                 /* Mobile country code */
  byte imsi_11_12;          /* The 11th and 12th digits of the IMSI */
  qword imsi_s;             /* Last ten digits of the IMSI */
  boolean special_service;  /* Special service option indicator */
  word service_option;      /* Service option (only present if
                               special_service field is set to 1) */
} caii_pc_0_sc_3_type;

/* PAGE_CLASS = '01' and PAGE_SUBCLASS = '00' */
typedef struct
{
  byte msg_seq;             /* Message sequence number */
  byte imsi_addr_num;       /* Number of IMSI address digits */
  byte imsi_11_12;          /* The 11th and 12th digits of the IMSI */
  qword imsi_s;             /* Last ten digits of the IMSI */
  boolean special_service;  /* Special service option indicator */
  word service_option;      /* Service option (only present if
                               special_service field is set to 1) */
} caii_pc_1_sc_0_type;

/* PAGE_CLASS = '01' and PAGE_SUBCLASS = '01' */
typedef struct
{
  byte msg_seq;             /* Message sequence number */
  byte imsi_addr_num;       /* Number of IMSI address digits */
  word mcc;                 /* Mobile country code */
  byte imsi_11_12;          /* The 11th and 12th digits of the IMSI */
  qword imsi_s;             /* Last ten digits of the IMSI */
  boolean special_service;  /* Special service option indicator */
  word service_option;      /* Service option (only present if
                               special_service field is set to 1) */
} caii_pc_1_sc_1_type;

/* PAGE_CLASS = '10' and PAGE_SUBCLASS = '00', '01', '10' */
typedef struct
{
  byte msg_seq;             /* Message sequence number */
  dword tmsi_code_addr;     /* TMSI code */
  boolean special_service;  /* Special service option indicator */
  word service_option;      /* Service option (only present if
                               special_service field is set to 1) */
} caii_pc_2_sc_012_type;

/* PAGE_CLASS = '10' and PAGE_SUBCLASS = '11' */
typedef struct
{
  byte msg_seq;             /* Message sequence number */
  byte tmsi_zone_len;       /* TMSI zone length      */
  caii_tmsi_type info;      /* TMSI zone & TMSI code */
  boolean special_service;  /* Special service option indicator */
  word service_option;      /* Service option (only present if
                               special_service field is set to 1) */
} caii_pc_2_sc_3_type;

/* PAGE_CLASS = '11' and PAGE_SUBCLASS = '01' */
typedef struct
{
  byte burst_type;                 /* Data burst type */
  byte addr_len;                   /* Address field length */
  byte bc_addr[CAII_BC_ADDR_MAX];  /* Broadcast address */
} caii_pc_3_sc_0_type;

#ifdef FEATURE_IS2000_REL_A

/* PAGE_CLASS = '11', PAGE_SUBCLASS = '01', PAGE_SUBCLASS_EXT = '00' */
/* Format 13.0 */
typedef PACK (struct)
{
  byte  msg_seq;             /* Message sequence number */
  boolean special_service;   /* Special service option indicator */
  word service_option;   /* Service option (only present if
                                special_service field is set to 1) */
} caii_pc_3_sc_1_sce_0_type;

/* PAGE_CLASS = '11', PAGE_SUBCLASS = '01', PAGE_SUBCLASS_EXT = '01' */
/* Format 13.1 */
typedef PACK (struct)
{
  byte  msg_seq;             /* Message sequence number */
  boolean special_service;   /* Special service option indicator */
  word service_option;       /* Service option (only present if
                                special_service field is set to 1) */
} caii_pc_3_sc_1_sce_1_type;

/* PAGE_CLASS = '11', PAGE_SUBCLASS = '01', PAGE_SUBCLASS_EXT = '10' */
/* Format 13.2 */
typedef PACK (struct)
{
  byte  msg_seq;             /* Message sequence number */
  boolean special_service;   /* Special service option indicator */
  word service_option;       /* Service option (only present if
                                special_service field is set to 1) */
} caii_pc_3_sc_1_sce_2_type;

/* PAGE_CLASS = '11', PAGE_SUBCLASS = '01', PAGE_SUBCLASS_EXT = '11' */
/* Format 13.3 */
typedef PACK (struct)
{
  byte msg_seq;          /* Message sequence number */
  byte length;           /* Address length */
  boolean special_service;   /* Special service option indicator */
  word service_option;       /* Service option (only present if
                                special_service field is set to 1) */
} caii_pc_3_sc_1_sce_3_type;

/* PAGE_CLASS = '11', PAGE_SUBCLASS = '10', PAGE_SUBCLASS_EXT = '00' */
/* Format 14.0 */
typedef PACK (struct)
{
  byte  msg_seq;             /* Message sequence number */
  boolean special_service;   /* Special service option indicator */
  word service_option;       /* Service option (only present if
                                special_service field is set to 1) */
} caii_pc_3_sc_2_sce_0_type;

/* PAGE_CLASS = '11', PAGE_SUBCLASS = '10', PAGE_SUBCLASS_EXT = '01' */
/* Format 14.1 */
typedef PACK (struct)
{
  byte msg_seq;          /* Message sequence number */
  boolean special_service;   /* Special service option indicator */
  word service_option;       /* Service option (only present if
                                special_service field is set to 1) */
} caii_pc_3_sc_2_sce_1_type;

/* PAGE_CLASS = '11', PAGE_SUBCLASS = '10', PAGE_SUBCLASS_EXT = '10' */
/* Format 14.2 */
typedef PACK (struct)
{
  byte msg_seq;          /* Message sequence number */
  boolean special_service;   /* Special service option indicator */
  word service_option;       /* Service option (only present if
                                special_service field is set to 1) */
} caii_pc_3_sc_2_sce_2_type;


/* PAGE_CLASS = '11', PAGE_SUBCLASS = '11', PAGE_SUBCLASS_EXT = '00' */
/* Format 15.0 */

#define CAII_EXT_BC_SDU_LEN_MAX 15
typedef struct
{
  byte burst_type;                 /* Data burst type */
  byte addr_len;                   /* Address field length */
  byte bc_addr[CAII_BC_ADDR_MAX];  /* Broadcast address */
  byte ext_bcast_sdu_length_ind;
  byte ext_bcast_sdu_length;
  byte bcn;
  word time_offset;
  byte repeat_time_offset;
  byte add_bcast_record[CAII_EXT_BC_SDU_LEN_MAX];
} caii_pc_3_sc_3_sce_0_type;


/* PAGE_CLASS = '11', PAGE_SUBCLASS = '11', PAGE_SUBCLASS_EXT = '01' */
/* Format 15.1 */
typedef PACK (struct)
{
  byte msg_seq;          /* Message sequence number */
  boolean special_service;   /* Special service option indicator */
  word service_option;       /* Service option (only present if
                                special_service field is set to 1) */
} caii_pc_3_sc_3_sce_1_type;

/* PAGE_CLASS = '11', PAGE_SUBCLASS = '11', PAGE_SUBCLASS_EXT = '10' */
/* Format 15.2 */
typedef PACK (struct)
{
  byte msg_seq;          /* Message sequence number */
  boolean special_service;   /* Special service option indicator */
  word service_option;       /* Service option (only present if
                                special_service field is set to 1) */
} caii_pc_3_sc_3_sce_2_type;


#endif /* FEATURE_IS2000_REL_A */


typedef struct
{
  byte page_class;
  byte page_subclass;
#ifdef FEATURE_IS2000_REL_A
  byte page_subclass_ext;
#endif /* FEATURE_IS2000_REL_A */

  union
  {
    caii_pc_0_sc_0_type format0;
    caii_pc_0_sc_1_type format1;
    caii_pc_0_sc_2_type format2;
    caii_pc_0_sc_3_type format3;
    caii_pc_1_sc_0_type format4;
    caii_pc_1_sc_1_type format5;
    caii_pc_2_sc_012_type format8;
    caii_pc_2_sc_012_type format9;
    caii_pc_2_sc_012_type format10;
    caii_pc_2_sc_3_type   format11;
    caii_pc_3_sc_0_type format12;
#ifdef FEATURE_IS2000_REL_A
    caii_pc_3_sc_1_sce_0_type format13_0;
    caii_pc_3_sc_1_sce_1_type format13_1;
    caii_pc_3_sc_1_sce_2_type format13_2;
    caii_pc_3_sc_1_sce_3_type format13_3;
    caii_pc_3_sc_2_sce_0_type format14_0;
    caii_pc_3_sc_2_sce_1_type format14_1;
    caii_pc_3_sc_2_sce_0_type format14_2;
    caii_pc_3_sc_3_sce_0_type format15_0;
    caii_pc_3_sc_3_sce_1_type format15_1;
    caii_pc_3_sc_3_sce_2_type format15_2;
#endif /* FEATURE_IS2000_REL_A */

  } rec;
} caii_gen_page_rec_type;

typedef struct
{
  byte msg_type;          /* Message type */
  byte config_msg_seq;    /* Configuration message sequence number */
  byte acc_msg_seq;       /* Access parameters message sequence number */
  boolean class_0_done;   /* Class 0 pages are done */
  boolean class_1_done;   /* Class 1 pages are done */
  boolean tmsi_done;      /* TMSI pages are done */
  boolean ordered_tmsis;  /* TMSIs sent in numerical order */
  boolean broadcast_done; /* Broadcast pages are done */
  word num_pages;         /* Number of pages in the pages array */
  caii_gen_page_rec_type gen_page[CAI_GEN_PAGE_MAX];
} caii_gen_page_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* GLOBAL SERVICE REDIRECTION MESSAGE (see IS-95A Section 7.7.2.3.2.18)    */

typedef struct
{
  byte msg_type;             /* Message type */
  word pilot_pn;             /* Pilot PN sequence offset index */
  byte config_msg_seq;       /* Configuration message sequence number */
  word redirect_accolc;      /* Redirected access overload classes */
  boolean return_if_fail;    /* Return if fail indicator */
  boolean delete_tmsi;       /* Delete TMSI indicator */
  boolean excl_p_rev_ms;     /* Exclude redirection indicator */
  byte record_type;          /* Redirection record type */
  byte record_len;           /* Redirection record length */
  union
  {
    caii_redirect_rec1_type rec1;
    caii_redirect_rec2_type rec2;
    caii_redirect_rec4_type rec4;
  } redir;
} caii_glob_redirect_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* PACA MESSAGE (see IS-95B Section 7.7.2.3.2.20)    */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte purpose;          /* Purpose of the PACA message */
  byte q_pos;            /* PACA Q position */
  byte paca_timeout;     /* PACA state timer duration */
} caii_paca_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* EXTENDED CHANNEL ASSIGNMENT MESSAGE (see IS-95B Section 7.7.2.3.2.21)             */

/* Base of Extended Channel Assignment Message */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
} caii_ext_chnasn_gen_type;

/* Pilot Record Type Definition */
typedef struct
{
  word pilot_pn;       /* Pilot PN offset index */
  boolean pwr_comb_ind;/* Power control symbol combining indicator */
  byte code_chan;      /* Code channel */
} caii_pilot_rec_type;

/* Extended Channel Assignment Message with Assign mode = 000 */
typedef struct
{
  caii_pc_hdr_type hdr;     /* Paging Channel common header fields */
  byte assign_mode;         /* Assignment mode */
  boolean freq_incl;        /* Frequency included indicator */
  byte default_config;      /* Default configuration */
  boolean bypass_alert_answer; /* Bypass indicator */
  byte num_pilots;          /* Number of pilot records following */
  byte granted_mode;        /* Granted mode */
  byte frame_offset;        /* Frame offset */
  byte encrypt_mode;        /* Message encryption mode */
  byte band_class;          /* Band class */
  word cdma_freq;           /* Frequency assignment (only present if
                               freq_incl field is set to 1) */
  caii_pilot_rec_type pilot_rec[CAI_EXT_CHNASN_AM0_MAX];
                            /* Active set pilot record */
  byte for_fch_rc;          /* Forward Fundamental Channel Radio Config */
  byte rev_fch_rc;          /* Reverse Fundamental Channel Radio Config */
  byte fpc_fch_init_setpt;  /* Initial Fundamental Channel Outer Loop
                               Eb/Nt setpoint */
  byte fpc_subchan_gain;    /* Forward Power Control Subchannel Rel Gain */
  byte rl_gain_adj;         /* Reverse Traffic Chan Power Relative to
                               access power */
  byte fpc_fch_fer;         /* Fundamental Channel Target Frame Err Rate */
  byte fpc_fch_min_setpt;   /* Min Fund Chan Outer Loop Eb/Nt setpoint */
  byte fpc_fch_max_setpt;   /* Max Fund Chan Outer Loop Eb/Nt setpoint */
  byte rev_fch_gating_mode; /* Reverse 1/8 gating mode indicator */
  boolean rev_pwr_cntl_delay_incl; /* Reverse power control delay included
                                      indicator */
  byte rev_pwr_cntl_delay;  /* Reverse power control delay */

#ifdef FEATURE_IS2000_REL_A
  byte d_sig_encrypt_mode;  /* Dedicated Channel Encrypt mode indicator */
  byte enc_key_size;        /* Encryption key size indication */
  boolean c_sig_encrypt_mode_incl; /* Common channel signaling encryption mode
                                      included indicator */
  byte c_sig_encrypt_mode;  /* Common channel signaling encryption mode
                               indicator */
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
  boolean msg_int_info_incl; /* Message integrity information included
                                indicator */
  boolean change_keys;       /* Change key indicator */
  boolean use_uak;           /* Use UAK indicator */
  boolean plcm_type_incl;    /* Public Long Code Mask type included indicator */
  byte plcm_type;            /* Public Long Code Mask type indicator */
  qword plcm_39;             /* 39 LSB bits of Public Long Code Mask */
#endif /* FEATURE_IS2000_REL_C */

} caii_ext_chnasn_am0_type;

/* Extended Channel Assignment Message with Assign mode = 001 */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
  boolean respond;       /* Respond on new Access Channel indicator */
  boolean freq_incl;     /* Frequency included indicator */
  byte band_class;       /* Band class */
  word cdma_freq;        /* Frequency assignment (only present if
                            freq_incl field is set to 1) */
  byte num_pilots;       /* Number of entries in pilot_pn array */
  word pilot_pn[CAI_EXT_CHNASN_AM1_MAX]; /* Pilot PN sequence offset index */
} caii_ext_chnasn_am1_type;

/* Extended Channel Assignment Message with Assign mode = 010 */
typedef struct
{
  caii_pc_hdr_type hdr;   /* Paging Channel common header fields */
  byte assign_mode;       /* Assignment mode */
  boolean respond;        /* Respond on analog control channel indicator */
  byte analog_sys;        /* System indicator */
  boolean use_analog_sys; /* Use analog system indicator */
  byte band_class;        /* Band class */
} caii_ext_chnasn_am2_type;

/* Extended Channel Assignment Message with Assign mode = 011 */
typedef struct
{
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
  word sid;              /* System identification of the analog system */
  byte vmac;             /* Voice mobile station attenuation code */
  word analog_chan;      /* Voice channel number */
  byte scc;              /* SAT color code */
  boolean mem;           /* Message encryption mode indiator */
  byte an_chan_type;     /* Analog voice channel type */
  byte dscc_msb;         /* DSCC most significant bit */
  byte band_class;       /* Band class */
} caii_ext_chnasn_am3_type;

/* Channel Indicator Type definitions */

#ifdef FEATURE_IS2000_REL_A
typedef struct
{
  boolean x3_fch_low_incl;   /* FCH code channel on the lowest SR3 frequency
                                included indicator */
  byte qof_mask_id_fch_low;  /* QOF index of the FCH on the lowest SR3
                                frequency */
  word code_chan_fch_low;    /* Code channel for the FCH on the lowest SR3
                                frequency */
  boolean x3_fch_high_incl;  /* FCH code channel on the highest SR3 frequency
                                included indicator */
  byte qof_mask_id_fch_high; /* QOF index for the FCH on the highest SR3
                                frequency */
  word code_chan_fch_high;   /* Code channel for the FCH on the highest SR3
                                frequency */
} caii_ext_ch_3x_fch_type;

typedef struct
{
  boolean x3_dcch_low_incl;   /* DCCH code channel on the lowest SR3 frequency
                                 included indicator */
  byte qof_mask_id_dcch_low;  /* QOF index for the DCCH on the lowest SR3
                                 frequency */
  word code_chan_dcch_low;    /* Code channel for DCCH on the lowest SR3
                                 frequency */
  boolean x3_dcch_high_incl;  /* DCCH code channle on the highest SR3 frequency
                                 included indicator */
  byte qof_mask_id_dcch_high; /* QOF index for the DCCH on the highest SR3
                                 frequency */
  word code_chan_dcch_high;   /* Code channel for the DCCH on the highest SR3
                                 frequency */
} caii_ext_ch_3x_dcch_type;
#endif /* FEATURE_IS2000_REL_A */

typedef struct
{
  word pilot_pn;              /* Pilot PN sequence offset index */
  boolean add_pilot_rec_incl; /* Additional pilot info included indicator */
  byte pilot_rec_type;        /* Pilot record type */
  caii_add_pilot_rec_type add_pilot_rec;
  boolean pwr_comb_ind;       /* Power control symbol combining indicator */
  word code_chan_fch;         /* Code channel index for FCH */
  byte qof_mask_id_fch;       /* Quasi-Orthogonal function mask identifier for
                                 FCH */
} caii_ext_ch_ind1_pilot_rec_type;

typedef struct
{
  byte fpc_fch_init_setpt;  /* Initial FCH outer loop Eb/Nt setpoint */
  byte fpc_fch_fer;         /* FCH target frame error rate */
  byte fpc_fch_min_setpt;   /* Min. FCH outer loop Eb/Nt setpoint */
  byte fpc_fch_max_setpt;   /* Max. FCH outer loop Eb/Nt setpoint */
  caii_ext_ch_ind1_pilot_rec_type pilot_rec[CAI_EXT_CHNASN_AM4_MAX];

#ifdef FEATURE_IS2000_REL_A
  boolean x3_fch_info_incl; /* 3X FCH info included indicator */
  caii_ext_ch_3x_fch_type fch_info[CAI_EXT_CHNASN_AM4_MAX];
#endif /* FEATURE_IS2000_REL_A */

} caii_ext_ch_ind1_type;

typedef struct
{
  word pilot_pn;              /* Pilot PN sequence offset index */
  boolean add_pilot_rec_incl; /* Additional pilot info. included indicator */
  byte pilot_rec_type;        /* Pilot record type */
  caii_add_pilot_rec_type add_pilot_rec;
  boolean pwr_comb_ind;       /* Power control symbol combining indicator */
  word code_chan_dcch;        /* Code Channel index for the DCCH */
  byte qof_mask_id_dcch;      /* Quasi-Orthogonal function Mask Identifier for
                                 DCCH */
} caii_ext_ch_ind2_pilot_rec_type;

typedef struct
{
  byte fpc_dcch_init_setpt;  /* Initial DCCH outer loop Eb/Nt setpoint */
  byte fpc_dcch_fer;         /* DCCH target frame error rate */
  byte fpc_dcch_min_setpt;   /* Min. DCCH outer loop Eb/Nt setpoint */
  byte fpc_dcch_max_setpt;   /* Max. DCCH outer loop Eb/Nt setpoint */
  caii_ext_ch_ind2_pilot_rec_type pilot_rec[CAI_EXT_CHNASN_AM4_MAX];

#ifdef FEATURE_IS2000_REL_A
  boolean x3_dcch_info_incl; /* 3X DCCH info. included */
  caii_ext_ch_3x_dcch_type dcch_info[CAI_EXT_CHNASN_AM4_MAX];
#endif /* FEATURE_IS2000_REL_A */

} caii_ext_ch_ind2_type;

typedef struct
{
  word pilot_pn;              /* Pilot PN sequence offset index */
  boolean add_pilot_rec_incl; /* Additional pilot info. included indicator */
  byte pilot_rec_type;        /* Pilot record type */
  caii_add_pilot_rec_type add_pilot_rec;
  boolean pwr_comb_ind;       /* Power control symbol combining indicator */
  word code_chan_fch;         /* Code channel index for the FCH */
  byte qof_mask_id_fch;       /* Quasi-Orthogonal function mask identifier for
                                 FCH */
  word code_chan_dcch;        /* Code channel index for DCCH */
  byte qof_mask_id_dcch;      /* Quasi-Orthogonal function mask identiifer for
                                 DCCH */
} caii_ext_ch_ind3_pilot_rec_type;

typedef struct
{
  byte fpc_fch_init_setpt;  /* Initial FCH outer loop Eb/Nt setpoint */
  byte fpc_dcch_init_setpt; /* Initial DCCH outer loop Eb/Nt setpoint */
  byte fpc_pri_chan;        /* Power control subchannel indicator */
  byte fpc_fch_fer;         /* FCH target frame error rate */
  byte fpc_fch_min_setpt;   /* Min. FCH outer loop Eb/Nt setpoint */
  byte fpc_fch_max_setpt;   /* Max. FCH outer loop Eb/Nt setpoint */
  byte fpc_dcch_fer;        /* DCCH target frame error rate */
  byte fpc_dcch_min_setpt;  /* Min. DCCH outer loop Eb/Nt setpoint */
  byte fpc_dcch_max_setpt;  /* Max. DCCH outer loop Eb/Nt setpoint */
  caii_ext_ch_ind3_pilot_rec_type pilot_rec[CAI_EXT_CHNASN_AM4_MAX];
#ifdef FEATURE_IS2000_REL_A
  boolean x3_fch_info_incl;  /* 3X FCH info included indicator */
  caii_ext_ch_3x_fch_type fch_info[CAI_EXT_CHNASN_AM4_MAX];
  boolean x3_dcch_info_incl; /* 3X DCCH info. included */
  caii_ext_ch_3x_dcch_type dcch_info[CAI_EXT_CHNASN_AM4_MAX];
#endif /* FEATURE_IS2000_REL_A */

} caii_ext_ch_ind3_type;

/* Union of Channel Indicator Type definitions */
typedef union
{
  caii_ext_ch_ind1_type ind1;
  caii_ext_ch_ind2_type ind2;
  caii_ext_ch_ind3_type ind3;
} caii_ext_ch_ind_type;

/* Extended Channel Assignment Message with Assign mode = 100 */
typedef struct
{
  caii_pc_hdr_type hdr;   /* Paging Channel common header fields */
  byte assign_mode;       /* Assignment mode */
  boolean freq_incl;      /* Frequency included indicator */
  byte band_class;        /* Band class */
  word cdma_freq;         /* Frequency assignment */
  boolean bypass_alert_answer; /* Bypass alert indicator */
  byte granted_mode;      /* Granted mode */

#ifdef FEATURE_IS2000_REL_C
  byte sr_id_restore;     /* Service reference identifier to be restored */
#endif /* FEATURE_IS2000_REL_C */

  byte default_config;    /* Default config */
  byte for_rc;            /* Forward Traffic Channel radio configuration */
  byte rev_rc;            /* Reverse Traffic Channel radio configuration */
  byte frame_offset;      /* Frame offset */
  byte encrypt_mode;      /* Message encryption mode */
  byte fpc_subchan_gain;  /* Forward Power Control Subchannel relative gain */
  byte rlgain_adj;        /* Reverse Traffic Channel power relative to
                             access power */
  byte num_pilots;        /* Number of pilots in the Active Set */
  byte ch_ind;            /* Channel indicator */
  caii_ext_ch_ind_type ch_rec;
  byte rev_fch_gating_mode; /* Reverse 1/8 gating mode indicator */
  boolean rev_pwr_cntl_delay_incl; /* Reverse power control delay included
                                      indicator */
  byte rev_pwr_cntl_delay;/* Reverse power control delay */


  byte d_sig_encrypt_mode;/* Dedicated Channel Encrypt mode indicator */
  byte enc_key_size;      /* Encryption key size */
  boolean c_sig_encrypt_mode_incl; /* Common Channel Encrypt Mode */
  byte c_sig_encrypt_mode;/* Common Channel Encrypt Mode */
  boolean x3fl_1xrl_incl; /* 3X forward link and 1x reverse link indicator */
  byte x1rl_freq_offset;  /* 1X reverse link frequency offset */



  boolean msg_int_info_incl; /* Message integrity information included
                                indicator */
  boolean change_keys;    /* Change keys indicator */
  boolean use_uak;        /* Use UAK indicator */
  boolean plcm_type_incl; /* Public Long Code Mask type included indicator */
  byte plcm_type;         /* Public Long Code Mask type indicator */
  qword plcm_39;          /* 39 LSB bits of the Public Long Code Mask */

} caii_ext_chnasn_am4_type;

typedef struct
{
  boolean add_pilot_rec_incl; /* Additional pilot information included
                                 indicator */
  byte pilot_rec_type;        /* Pilot record type */
  caii_add_pilot_rec_type add_pilot_rec;

#ifdef FEATURE_IS2000_REL_C
  boolean for_pdch_incl;      /* Forward packet data channel configuration
                                 field included indicator */
  byte walsh_table_id;        /* Index of the Walsh Table used */
  byte num_pdcch;             /* Number of packet data control channels
                                 supported */
  byte for_pdcch_walsh[CAI_EXT_CHNASN_AM5_MAX]; /* F-PDCCH walsh code */
  byte mac_id;                /* Medium access control index */
  byte rev_cqich_cover;       /* R-CQICH cover */
  byte for_cpcch_walsh;       /* F-CPCCH walsh code */
  byte for_cpcsch;            /* Forward common power control channel subchannel */
#endif /* FEATURE_IS2000_REL_C */

  boolean pwr_comb_ind;       /* Power control symbol combining indicator */

#ifdef FEATURE_IS2000_REL_C
  boolean pdch_group_ind;     /* Packet data channel group indicator */
#endif /* FEATURE_IS2000_REL_C */

  word code_chan_fch;         /* Code channel on FCH */
  byte qof_mask_id_fch;       /* Quasi-orthogonal function index on FCH */
  word code_chan_dcch;        /* Code channke on the DCCH */
  byte qof_mask_id_dcch;      /* Quasi-orthogonal function index on the DCCH */
  word pilot_pn;          /* Pilot PN sequence offset index */

} caii_ext_chnasn_pilot_info_type;

#ifdef FEATURE_IS2000_REL_C
/* Extended Channel Assignment Message with Assign mode = 101 */
typedef struct
{
  boolean for_pdch_rlgain_incl;        /* F-PDCH parameters related to reverse
                                          link adjustment gains included
                                          indicator */
  byte rlgain_ackch_pilot;             /* R-ACKCH to pilot adjustment gain */
  byte rlgain_cqich_pilot;             /* R-CQICH to pilot adjustment gain */
  byte num_soft_switching_frames;      /* Number of frames for R-CQICH soft
                                          switching */
  byte num_softer_switching_frames;    /* Number of frames for R-CQICH softer
                                          switching */
  byte num_soft_switching_slots;       /* Number of slots per frame for R-CQICH soft
                                          switching */
  byte num_softer_switching_slots;     /* Number of slots per frame for R-CQICH
                                          softer switching */
  boolean chm_switching_params_incl;   /* Control hold mode fields inclusion
                                          indicator */
  byte num_soft_switching_frames_chm;  /* Number of frames for R-CQICH soft
                                          switching while in control hold */
  byte num_softer_switching_frames_chm;/* Number of frames for R-CQICH
                                          softer switching while in control
                                          hold */
  byte num_soft_switching_slots_chm;   /* Number of slots per frame for
                                          R-CQICH soft switching while in
                                          control hold */
  byte num_softer_switching_slots_chm; /* Number of slots per frame for
                                          R-CQICH softer swicthing while in
                                          control hold */
  byte pdch_soft_switching_delay;      /* F-PDCH soft switching delay */
  byte pdch_softer_switching_delay;    /* F-PDCH softer switching delay */
} caii_for_pdch_parm_type;
#endif /* FEATURE_IS2000_REL_C */


#ifdef FEATURE_IS2000_REL_C
typedef struct
{
  caii_pc_hdr_type hdr;     /* Paging Channel common header fields */
  byte assign_mode;         /* Assignment mode */
  boolean freq_incl;        /* Frequency included indicator */
  byte band_class;          /* Band class */
  word cdma_freq;           /* Frequency assignment */
  boolean bypass_alert_answer; /* Bypass alert indicator */
  byte granted_mode;        /* Granted mode */
  byte sr_id_restore;       /* Service reference identifier to be restored */

  byte frame_offset;        /* Frame offset */
  byte encrypt_mode;        /* Message encryption mode */
  byte d_sig_encrypt_mode;  /* Dedicated Channel encryption mode indicator */
  byte enc_key_size;        /* Encryption key size */
  boolean c_sig_encrypt_mode_incl; /* Common Channel signaling encryption mode */
  byte c_sig_encrypt_mode;  /* Common Channel signaling encrypt mode */
  boolean msg_int_info_incl;/* Message integrity information included
                               indicator */
  boolean change_keys;      /* Change keys indicator */
  boolean use_uak;          /* Use UAK indicator */
  boolean plcm_type_incl;   /* Public Long Code Mask type included
                               indicator */
  byte plcm_type;           /* Public Long Code Mask type indicator */
  qword plcm_39;            /* 39 LSB bits of the Public Long Code Mask */
  byte rlgain_adj;          /* Reverse traffic channel power relative to
                               access power */
  byte num_pilots;          /* Number of pilots in the active set */
  byte ext_ch_ind;          /* Extended channel indicator */
  byte fpc_subchan_gain;    /* Forward power control subchannel relative
                               gain */
  byte rev_fch_gating_mode; /* Reverse 1/8 gating mode indicator */
  boolean rev_pwr_cntl_delay_incl; /* Reverse power control delay included
                                      indicator */
  byte rev_pwr_cntl_delay;  /* Reverse power control delay  */
  boolean full_ci_feedback_ind; /* Full C/I feedback rate indicator */
  byte for_cpcch_rate;      /* F-CPCCH rate */
  byte for_cpcch_update_rate; /* F-CPCCH update rate */
  byte rev_cqich_frame_offset; /* Reverse channle quality indicator channel
                                     frame offset */
  byte rev_cqich_reps;      /* Reverse channel quality indicator channel
                               repetition factor */
  byte rev_ackch_reps;      /* Reverse acknowledgment channel repetition
                               factor */
  byte for_pdch_rc;         /* Forward packet data channel radio
                               configuration */

  byte for_fch_dcch_rc;     /* Forward FCH or forward DCCH radio
                               configuration */
  byte rev_fch_dcch_rc;     /* Reverse FCH or reverse DCCH radio
                               configuration */
  byte fpc_pri_chan;        /* Power control subchannel indicator */
  byte fpc_fch_init_setpt;  /* Initial FCH outer loop Eb/Nt setpoint */
  byte fpc_fch_fer;         /* FCH target frame error rate */
  byte fpc_fch_min_setpt;   /* Min. FCH outer loop Eb/Nt setpoint */
  byte fpc_fch_max_setpt;   /* Max. FCH outer loop Eb/Nt setpoint */
  byte fpc_dcch_init_setpt; /* Initial DCCH outer loop Eb/Nt setpoint */
  byte fpc_dcch_fer;        /* DCCH target frame error rate */
  byte fpc_dcch_min_setpt;  /* Min. DCCH outer loop Eb/Nt setpoint */
  byte fpc_dcch_max_setpt;  /* Max. DCCH outer loop Eb/Nt setpoint */
  boolean pdch_group_ind_incl; /* Packet data channel indicator
                                  inclusion flag */
  boolean for_pdch_parms_incl; /* Indicator of the inclusion of forward
                                  packet data channel configuration fields */
  caii_for_pdch_parm_type for_pdch_parm;

  caii_ext_chnasn_pilot_info_type pilot_rec[CAI_EXT_CHNASN_AM5_MAX];

} caii_ext_chnasn_am5_type;
#endif /* FEATURE_IS2000_REL_C */

/* Union of Extended Channel Assignment messages */
typedef union
{
  caii_ext_chnasn_gen_type gen;
  caii_ext_chnasn_am0_type am0;
  caii_ext_chnasn_am1_type am1;
  caii_ext_chnasn_am2_type am2;
  caii_ext_chnasn_am3_type am3;
  caii_ext_chnasn_am4_type am4;
#ifdef FEATURE_IS2000_REL_C
  caii_ext_chnasn_am5_type am5;
#endif /* FEATURE_IS2000_REL_C */
} caii_ext_ca_ord_type;

#ifdef FEATURE_IS2000_REL_C
#define CAII_MAX_INT_ECAM_SIZ sizeof(caii_ext_chnasn_am5_type)
#else
#define CAII_MAX_INT_ECAM_SIZ sizeof(caii_ext_chnasn_am4_type)
#endif /* FEATURE_IS2000_REL_C */

/* This is the maximum size possible for the internal format of a channel
   assignment record */

#define CAII_MAX_EXT_CHNASN_MSG_BUF_SIZ  ( CAI_EXT_CHN_ASN_MAX* \
                                         sizeof(caii_ext_chnasn_am2_type) \
                                         + CAII_MAX_INT_ECAM_SIZ )
/* Buffer for the packed internal format */


/* Extended Channel Assignment message type */
typedef struct
{
  byte msg_type;     /* Message type */
  word num_chns;     /* Number of channel assignments in the chns array */
  byte buf[CAII_MAX_EXT_CHNASN_MSG_BUF_SIZ];
                     /* Buffer to hold the packed internal format of
                        channel assignment records */
  caii_ext_ca_ord_type* chns[CAI_EXT_CHN_ASN_MAX];
                     /* Pointer to the channel assignment record in the
                        following buffer */
} caii_ext_chnasn_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* GENERAL NEIGHBOR LIST MESSAGE (see IS95B Section 3.7.2.3.2.22)          */

typedef struct
{
  byte band_class;
  byte sys_a_b;
} caii_anl_nghbr_type;

#ifdef FEATURE_IS2000_REL_C
typedef struct
{
  word nghbr_pn;                 /* Neighbor pilot PN sequence offset index */
  boolean nghbr_freq_incl;       /* Neighbor frequency information included
                                    indicator */
  byte nghbr_band;               /* Neighbor band class */
  word nghbr_freq;               /* Neighbor frequency assignment */
  boolean pn_association_ind;    /* Neighbor PN association indicator */
  boolean data_association_ind;  /* Neighbor data association indicator */
} caii_hrpd_nghbr_type;
#endif /* FEATURE_IS2000_REL_C */

typedef struct
{
  byte msg_type;         /* Message type */
  word pilot_pn;         /* Pilot PN sequence offset index */
  byte config_msg_seq;   /* Configuration message sequence number */
  byte pilot_inc;        /* Pilot PN sequence offset index increment */
  byte nghbr_srch_mode;  /* Neighbor search mode */
  boolean nghbr_config_pn_incl; /* Neighbor configuration and PN offset
                                   included. */
  boolean freq_fields_incl;   /* Frequency fields included */
  boolean use_timing;         /* Use timing indicator */
  boolean global_timing_incl; /* Global timing included.  (Included if
                                 use_timing is set to 1) */
  byte global_tx_duration;    /* Global neighbor tranmit time duration
                                 (Included if global_timing_incl is included
                                  and set to 1). */
  byte global_tx_period;      /* Global neighbor transmit time period.
                                 (Included if global_timing_incl is included
                                  and set to 1). */
  byte num_nghbr;
  struct
  {
    byte nghbr_config;        /* Neighbor configuration (Included if
                                 nghbr_config_pn_incl is set to 1) */
    word nghbr_pn;            /* Neighbor pilot PN sequence offset index
                                 (Included if nghbr_config_pn_incl is set to
                                 1)*/
    byte search_pri;          /* Pilot Channel search priority */
    byte srch_win_nghbr;      /* Neighbor search window size */
    boolean freq_incl;        /* Frequency included indicator (Included if
                                 freq_fields_incl is set to 1.) */
    byte nghbr_band;          /* Neighbor band class */
    word nghbr_freq;          /* Neighbor frequency assignment */
    boolean timing_incl;      /* Timing included indicator (Included if
                                 use_timing is set to 1). */
    byte nghbr_tx_offset;     /* Neighbor transmit time offset. (Included
                                 if timing_incl is included and set to 1).*/
    byte nghbr_tx_duration;   /* Neighbor transit time duration.  (Included
                                 if timing_incl is included and set to 1,
                                 and global_timing_incl is set to 0) */
    byte nghbr_tx_period;     /* Neighbor transit time period.  (Included
                                 if timing_incl is included and set to 1,
                                 and global_timing_incl is set to 0). */
  } nghbr[CAI_GEN_NGHBR_MAX];
  byte num_analog_nghbr;      /* Number of neighboring analog systems
                                 (number listed in analog_nghbr array) */
  caii_anl_nghbr_type analog_nghbr[CAI_GEN_ANALOG_NGHBR_MAX];
  boolean is2000_incl;
  boolean srch_offset_incl;
  struct
  {
    boolean add_pilot_rec_incl;
    byte nghbr_pilot_rec_type;
    caii_add_pilot_rec_type add_pilot_rec;
    byte srch_offset_nghbr;
  } add_nghbr[CAI_GEN_NGHBR_MAX];

#ifdef FEATURE_IS2000_REL_A
  boolean is2000_rel_a_incl;
  boolean bcch_ind_incl;
  boolean bcch_support[CAI_GEN_NGHBR_MAX];
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_B
  boolean is2000_rel_b_incl;
  boolean resq_enabled;            /* Call rescue feature enabled indicator */
  byte resq_delay_time;            /* Call rescue delay timer value */
  byte resq_allowed_time;          /* Call rescue allowed timer value */
  byte resq_attempt_time;          /* Call rescue attempt timer value */
  word resq_code_chan;             /* Code channel index for the Rescue
                                      Channel */
  byte resq_qof;                   /* Quasi-Orthogonal Function mask
                                      indentifier for the Rescue Channel */
  boolean resq_min_period_incl;    /* Minimum time between consecutive rescues
                                      included indicator */
  byte resq_min_period;            /* Minimum time between consecutive
                                        rescues */
  boolean resq_num_tot_trans_incl; /* Required # of transmissions before
                                      declaring L2 Acknowledgment Failure when
                                      Call Rescue is enabled included
                                      indicator */
  byte resq_num_tot_trans_20ms;    /* Required # of transmissions of a
                                      regular PDU before declaring L2
                                      Acknowledgement Failure when Call
                                      Rescue is enabled */
  byte resq_num_tot_trans_5ms;     /* Required # of transmissions of a mini
                                      PDU before declaring L2 Acknowledgement
                                      Failure when Call Rescue is enabled */
  byte resq_num_preamble_rc1_rc2;  /* Traffic Channel preamble length for Call
                                      Rescue Soft Handoff when operating in
                                      RC 1 or 2 */
  byte resq_num_preamble;          /* Traffic Channel preamble length for Call
                                      Rescue Soft Handoff when operating in RC
                                      > 2 */
  byte resq_power_delta;           /* Power level adjustment to be applied to
                                      the last closed-loop power level when
                                      re-enabling the transmitter for Call
                                      Rescue soft handoff */
  boolean nghbr_resq_configured[CAI_GEN_NGHBR_MAX];
    /* Neighbor Rescue Channel configured indicator */
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C
  boolean is2000_rel_c_incl;
  boolean nghbr_pdch_supported[CAI_GEN_NGHBR_MAX];
  boolean hrpd_nghbr_incl;         /* HRPD neighnor information included
                                      indicator */
  byte num_hrpd_nghbr;             /* Number of HRPD neighbor pilot PN
                                      sequences */
  caii_hrpd_nghbr_type hrpd_nghbr[CAI_GEN_NGHBR_MAX];
#endif /* FEATURE_IS2000_REL_C */
} caii_gen_nghbr_type;

typedef struct
{
  byte num_analog_nghbr;
  caii_anl_nghbr_type anl_nghbr[CAI_GEN_ANALOG_NGHBR_MAX];
} caii_analog_nghbr_type;

/* Extended Global Service Redirection Message (EGSRM), IS2000, 3.7.2.3.2.27 */
typedef union
{
  caii_redirect_rec1_type rec1;
  caii_redirect_rec2_type rec2;
} caii_ext_glb_redir_rec_type;

typedef struct
{
  word redirect_accolc;           /* Redirected access overload classes */
  boolean delete_tmsi;            /* Delete TMSI indicator */
  boolean redirect_p_rev_incl;    /* Redirection mobile protocol revision included */
  boolean excl_p_rev_ind;         /* Excluding mobile protocol revision indicator */
  byte redirect_p_min;            /* Minimum redirection protocol revision */
  byte redirect_p_max;            /* Maximum direction protocol revision */
  byte record_type;               /* Redirection record type */
  caii_ext_glb_redir_rec_type redir;
} caii_ext_glb_redir_info_type;

typedef struct
{
  byte msg_type;        /* Message type */
  word pilot_pn;        /* Pilot PN sequence offset index */
  byte config_msg_seq;  /* Configuration message sequence number */
  boolean return_if_fail;  /* return if fail indicator */
  caii_ext_glb_redir_info_type first;
} caii_ext_glb_redir_type;

/* Extended CDMA Channel List Message (ECCLM), IS2000, 3.7.2.3.2.28  */
#ifdef FEATURE_IS2000_REL_A
typedef struct
{
  boolean td_hash_ind; /* transmit diversity hash indicators */
  byte td_power_level; /* transmit diveristy power levels */
} caii_ext_chnlist_td_sel_type;

#endif /* FEATURE_IS2000_REL_A */

typedef struct
{
  byte msg_type;                   /* Message type */
  word pilot_pn;                   /* Pilot PN sequence offset index */
  byte config_msg_seq;             /* Configuration message sequence number */
  byte num_freq;                   /* Number of entries in the cdma_freq array */
  word cdma_freq[CAI_EXT_CHNLIST_FREQ_MAX];/* CDMA Channel frequency assignments */
  boolean rc_qpch_sel_incl;        /* RC and QPCH selection included indicator */
  boolean rc_qpch_hash_ind[CAI_EXT_CHNLIST_FREQ_MAX];/* RC_QPCH channel hashing indicator */

#ifdef FEATURE_IS2000_REL_A
  boolean is2000_rel_a_incl;
  boolean td_sel_incl;             /* TD selection indicator included */
  byte td_mode;                    /* Transmit diversity mode */
  caii_ext_chnlist_td_sel_type td_sel[CAI_EXT_CHNLIST_FREQ_MAX];
#endif /* FEATURE_IS2000_REL_A */

} caii_ext_chnlist_type;

#ifdef FEATURE_IS2000_REL_A
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                BROADCAST CONTROL CHANNEL MESSAGE                        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  byte msg_type;              /* Message type */
  word pilot_pn;              /* Pilot PN sequence offset index */
  byte config_msg_seq;        /* Configuration message sequence number */
  word sid;                   /* system identification */
  word nid;                   /* network identification */
  byte packet_zone_id;        /* packet data services zone identifier */
  word reg_zone;              /* registration zone */
  byte total_zones;           /* number of registration zones to be retained */
  byte zone_timer;            /* zone timer length */
  boolean mult_sids;          /* multiple SID storage indicator */
  boolean mult_nids;          /* multiple NID storage indicator */
  boolean home_reg;           /* home registration indicator */
  boolean for_sid_reg;        /* SID roamer registratino indicator */
  boolean for_nid_reg;        /* NID roamer registration indicator */
  boolean power_up_reg;      /* power-up registration indicator */
  boolean power_down_reg;    /* power-down registration indicator */
  boolean parameter_reg;      /* paramter change registration indicator */
  byte reg_prd;               /* registration period */
  boolean dist_reg_incl;      /* distance-based registration information included */
  word reg_dist;              /* registration distance */
  boolean delete_for_tmsi;    /* delete foreign TMSI */
  boolean use_tmsi;           /* use TMSI indicator */
  byte pref_msid_type;        /* preferred enhanced access channel mobile station identifier */
  byte tmsi_zone_len;         /* TMSI zone length */
  qword tmsi_zone;             /* TMSI zone */
  boolean imsi_t_supported;   /* IMSI_t support indicator */
  byte max_num_alt_so;        /* Maximum number of alternative service options */
  boolean auto_msg_supported; /* autonomous message supported indicator */
  byte auto_msg_interval;     /* autonomous message interval */
  boolean other_info_incl;    /* other information included indicator */
  word base_id;               /* base station identification */
  word mcc;                   /* mobile country code */
  byte imsi_11_12;            /* 11th and 12th digits of IMSI */
  boolean broadcast_gps_asst; /* Broadcast GPS assist indicator */
  byte sig_encrypt_sup;       /* signaling encryption supported indicator */
  boolean cs_supported;       /* concurrent service supported */
  boolean ms_init_pos_loc_sup_ind; /* MS init position location indicator */
#ifdef FEATURE_IS2000_REL_C
  boolean is2000_rel_c_incl;     /* IS2000 Release C fields included */
  boolean msg_integrity_sup;     /* Message integrity supported indicator */
  boolean sig_integrity_sup_incl;/* Signalling message integrity information
                                    included indicator */
  byte sig_integrity_sup;        /* Signalling inetegrity algorithm supported
                                    by the base station */
  boolean imsi_10_incl;          /* IMSI 10 included */
  byte imsi_10;                  /* The least significant digit of MNC when MNC
                                    is a 3-digit number */
#endif /* FEATURE_IS2000_REL_C */

} caii_a41_sys_parm_type;

typedef struct
{
  byte bcch_code_chan;          /* walsh code index for the BCCH specified by BCCH_ID */
  byte brat;                    /* BCCH data rate */
  boolean bcch_code_rate;       /* BCCH code rate */
} caii_bcch_bcast_info_type;

typedef struct
{
  byte msg_type;              /* Message type */
  word pilot_pn;              /* pilot PN sequence offset index */
  byte config_msg_seq;        /* configuration message sequence number */
  word base_id;               /* base station identification */
  byte p_rev;                 /* protocol revision level */
  byte min_p_rev;             /* minimum protocol revision level */
  boolean sr3_incl;           /* SR3 common channel parameters included indicator */
  boolean sr3_center_freq_incl;/* center SR3 frequency assignment included */
  word sr3_center_freq;       /* center SR3 frequency assignment */
  byte sr3_brat;              /* SR3 BCCH data rate */
  byte sr3_bcch_code_chan;    /* SR3 BCCH Walsh code index */
  byte sr3_primary_pilot;     /* primary SR3 pilot */
  byte sr3_pilot_power1;      /* power level diff between primary and lower freq SR3 pilots */
  byte sr3_pilot_power2;      /* power level diff between primary and higher freq SR3 pilots */
  byte srch_win_a;            /* search window size for the active set and candidate set */
  byte srch_win_r;            /* search window size for the remaining set */
  byte t_add;                 /* pilot detection threshold */
  byte t_drop;                /* pilot drop threshold */
  byte t_comp;                /* active set versus candidate set comparision threshold */
  byte t_tdrop;               /* drop timer value */
  byte nghbr_max_age;         /* neighbor set maximum AGE */
  byte soft_slope;            /* slope in inequality criterion for adding/dropping pilot from active set */
  byte add_intercept;         /* intercept to inequality criterion for adding a pilot to active set */
  byte drop_intercept;        /* intercept to inequality criterion for dropping a pilot from active set */
  boolean enc_supported;      /* encryption fields supported */
  byte sig_encrypt_sup;       /* signaling enctyrption supported indicator */
  byte ui_encrypt_sup;        /* user inforamtion encryption supported indicator */
  byte add_fields_len;        /* additional fields length */
  boolean cch_info_incl;      /* common channel information included indicator */
  word mcc;                   /* mobile country code */
  byte imsi_11_12;            /* 11th and 12th difits of the IMSI */
  byte max_slot_cycle_index;  /* maximum slot cycle index */
  byte pwr_rep_thresh;        /* power control reporting threshold */
  byte pwr_rep_frames;        /* power control reporting frame count */
  boolean pwr_thresh_enable;  /* threshold report mode indicator */
  boolean pwr_period_enable;  /* periodic report mode indicator */
  byte pwr_rep_delay;         /* power report delay */
  boolean reselect_included;     /* system reselection paramters included */
  byte ec_thresh;             /* pilot power threshold */
  byte ec_io_thresh;          /* Pilot Ec / Io threshold */
  dword base_lat;             /* Base station lattitude */
  dword base_long;            /* Base station longitude */
  boolean pilot_report;       /* pilot reporting indicator */
  boolean acc_ent_ho_order;   /* access entry handoff permitted indicator */
  boolean access_ho;          /* access handoff permitted indicator */
  boolean access_ho_msg_rsp;  /* access handoff permitted for message response indicator */
  boolean access_probe_ho;    /* access probe handoff permitted indicator */
  boolean acc_ho_list_upd;    /* access handoff list update permitted indicator */
  boolean acc_probe_ho_other_msg;/* access probe handoff permitted for other message */
  byte max_number_probe_ho;   /* maximum number of permitted access probe handoffs */
  byte num_fccch;             /* total number of Forward common control channel */
  byte fccch_rate;            /* rate words for the FCCCH */
  boolean fccch_code_rate;    /* code rate for the FCCCH */
  byte fccch_code_chan[CAI_MAX_NUM_FCCCH]; /* Code channel index for FCCCH */
  byte bcast_index;           /* Broadcasdt index */
  byte num_bcch_bcast;        /* number of BCCCH used for transmitting broadcast messages */
  caii_bcch_bcast_info_type bcch_info[CAI_MAX_NUM_BCCH_BCAST];
  boolean qpch_supported;     /* Quick Paging channel supported indication */
  byte num_qpch;              /* number of quick paging channels */
  boolean qpch_rate;          /* Quick paging channel indicator rate */
  byte qpch_power_level_page; /* Quick Paging channel paging indicator transmit power level */
  boolean qpch_cci_supported; /* qpch configuration change indicator supported */
  byte qpch_power_level_config;/* qpch configuration change indicator transmit power level */
  byte qpch_code_chan[CAI_MAX_NUM_QPCH]; /* code channel index of the QPCH for SR3 */
  boolean qpch_bi_supported;  /* qpch broadcast indicator supported */
  byte qpch_power_level_bcast;/* qpch broadcast indicator transmit power level */
  boolean sdb_supported;      /* short data burst supported indicator */
  boolean broadcast_gps_asst; /* Broadcast GPS assist indicator */
  byte rlgain_traffic_pilot;  /* adjustment of the reverse traffic channel to the rev pilot chan */
  boolean rev_pwr_cntl_delay_incl;/* reverse power control delay included indicator */
  byte rev_pwr_cntl_delay;    /* reverse power control delay */
  boolean mob_qos;            /* indicator granting permission to mobile station to request QoS */
  boolean use_sync_id;        /* Sync ID supported indicator */
  byte num_opt_msg;           /* number of optional overhead messages to be sent */
  boolean sending_rand;       /* ANSI-41 Rand Message indicator */
  boolean pri_nghbr_list;     /* private neighbor list message indicator */
  boolean user_zone_id;       /* user zone identification message indicator */
  boolean ext_global_redirect;/* extended global service redirection message indicator */

  #ifdef FEATURE_1X_TO_LTE
  boolean ap_pilot_info;      /* AP_PILOT_INFO */
  boolean ap_idt;             /* AP_IDT */
  boolean ap_id_text;         /* AP_ID_TEXT */
  boolean gen_ovhd_inf_ind;   /* GEN_OVHD_INF_IND */
  boolean fd_chan_lst_ind;    /* FD_CHAN_LST_IND */
  boolean atim_ind;           /* ATIM_IND */
  boolean freq_gen_ovhd_inf_ind; /* FREQ_GEN_OVHD_INF_IND */
  byte reserved;                 /* RESERVED */
  #endif /* FEATURE_1X_TO_LTE */

  boolean pilot_info_req_supported; /* pilot information request supported indicator */

#if defined(FEATURE_IS2000_REL_B) || defined(FEATURE_1X_TO_LTE)
  boolean band_class_info_req;              /* Band class information request
                                               indicator */
  byte alt_band_class;                      /* Alternate band class */
  boolean cdma_off_time_rep_sup_ind;        /* CDMA off time report supported
                                               indicator */
  boolean cdma_off_time_rep_threshold_unit; /* CDMA off time report unit */
  byte cdma_off_time_rep_threshold;         /* CDMA off time report threshold */
#endif /* FEATURE_IS2000_REL_B || FEATURE_1X_TO_LTE*/

#if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_1X_TO_LTE)
  boolean chm_supported;            /* Control Mode Mode supported indicator */
  boolean release_to_idle_ind;      /* Release to idle allowed indicator */
  boolean reconnect_msg_ind;        /* Reconnect Message supported indicator */
  boolean t_tdrop_range_incl;       /* Drop timer range value included indicator */
  byte t_tdrop_range;               /* Drop timer range value */
  boolean for_pdch_supported;       /* Forward Packet Data Channel supported
                                       indicator */
  boolean pdch_chm_supported;       /* PDCH Control Hold Mode supported
                                       indicator */
  boolean pdch_parms_incl;          /* Forward Packet Data Channel related
                                       parameters included indicator */
  boolean for_pdch_rlgain_incl;     /* F-PDCH parameters related to reverse
                                       link adjustment gains included indicator */
  byte rlgain_ackch_pilot;          /* R-ACKCH to pilot adjustment gain */
  byte rlgain_cqich_pilot;          /* R-CQICH to pilot adjustment gain */
  byte num_soft_switching_frames;   /* Number of frames for R-CQICH soft
                                       switching */
  byte num_softer_switching_frames; /* Number of frames for R-CQICH softer
                                       switching */
  byte num_soft_switching_slots;    /* Number of slots per frame for R-CQICH
                                          soft switching */
  byte num_softer_switching_slots;  /* Number of slots per frame for R-CQICH
                                          softer switching */
  byte pdch_soft_switching_delay;   /* F-PDCH soft switching delay */
  byte pdch_softer_switching_delay; /* F-PDCH softer switching delay */
  byte walsh_table_id;              /* The index of the Walsh Table used */
  byte num_pdcch;                   /* The number of Packet Data Control */
  byte for_pdcch_walsh[CAI_MAX_NUM_FOR_PDCCH]; /* Forward packet data control
                                                  walsh code */
  boolean imsi_10_incl;             /* IMSI 10 included */
  byte imsi_10;                     /* The least significant digit of MNC when MNC
                                      is a 3-digit number */
#endif /* FEATURE_IS2000_REL_C || FEATURE_1X_TO_LTE */

#if defined(FEATURE_1X_TO_LTE)
  boolean neg_slot_cycle_index_sup;/* Negative slot cycle index supported indicator */
  boolean rer_mode_supported;      /* Radio environment reporting mode supported
                                      indicator */
  boolean auto_fcso_allowed;       /* Autonomous fast call setup order allowed */
  boolean sending_bspm;            /* BCMC service parameter message indicator */
  byte bspm_period_index;          /* BSPM transmission periodicity index */
  boolean rev_pdch_supported;      /* R-PDCH supported indicator */
  boolean rev_pdch_parms_incl;     /* R-PDCH related parameters included indicator */
  boolean rev_pdch_rlgain_incl;    /* R-PDCH parameters related to reverse link
                                      adjustment gains included indicator */
  byte rlgain_spich_pilot;         /* Reverse secondary pilot channel to pilot
                                      adjustment gain */
  byte rlgain_reqch_pilot;         /* Reverse request channel to pilot adjustment
                                      gain */
  byte rlgain_pdcch_pilot;         /* R-PDCCH to pilot adjustment gain */
  boolean rev_pdch_parms_1_incl;   /* R-PDCH parameters subset included indicator */
  byte rev_pdch_table_sel;         /* R-PDCH table selector */
  byte rev_pdch_max_auto_tpr;      /* R-PDCH maximum traffic to pilot ratio for
                                      autonomous transmission */
  byte rev_pdch_num_arq_rounds_normal; /* Maximum number of allowed ARQ rounds
                                          on the R-PDCH in the non-boosted
                                          mode */
  boolean rev_pdch_oper_parms_incl;    /* R-PDCH operational parameters included
                                          indicator */
  byte rev_pdch_max_size_allowed_encoder_packet; /* Maximum allowed R-PDCH
                                                    encoder packet size */
  byte rev_pdch_default_persistence;/* R-PDCH default initial persistence */
  byte rev_pdch_reset_persistence; /* R-PDCH reset persistence indicator */
  byte rev_pdch_grant_precedence;  /* R-PDCH grant predence indicator */
  boolean rev_pdch_msib_supported; /* R-PDCH MSIB usage indicator */
  boolean rev_pdch_soft_switching_reset_ind; /* R-PDCH soft handoff reset indicator */
  boolean sdb_in_rcnm_ind;         /* SDB allowed in reconnect message indicator */
  boolean cand_band_info_req;      /* Candidate band class information request
                                     indicator */
  byte num_cand_band_class;        /* Number of candidate band classes */
  caii_cand_band_info_type cand_band_info[CAI_EXT_SYSPARM_CAND_BAND_MAX];
  boolean rescan;                  /* Rescan indicator */
  boolean tx_pwr_limit_incl;       /* Transmit power limit inclusion for the current
                                      Base Station */
  byte tx_pwr_limit;               /* Transmit power limit for the current Base
                                      Station */
  byte bypass_reg_ind;             /* Indication to bypass powerup registration
                                      upon band, frequency block or serving system
                                      change */
#endif /* FEATURE_1X_TO_LTE */

#ifdef FEATURE_1X_TO_LTE
  boolean crrm_msg_ind;            /* CRRM_MSG_IND */
  byte appim_period_index;         /* APPIM_PERIOD_INDEX */
  byte gen_ovhd_cycle_index;       /* GEN_OVHD_CYCLE_INDEX */
  byte atim_cycle_index;           /* ATIM_CYCLE_INDEX */
  boolean add_loc_info_inc;        /* ADD_LOC_INFO_INCL */
  byte loc_unc_h;                  /* LOC_UNC_H */
  word height;                     /* HEIGHT */
  byte loc_unc_v;                  /* LOC_UNC_V */
  boolean add_loc_gem_allowed;     /* ADD_LOC_GEM_ALLOWED */
#endif /* FEATURE_1X_TO_LTE */

} caii_mc_rr_parm_type;

typedef struct
{
  byte msg_type;                /* Message type */
  word pilot_pn;                /* pilot PN sequence offset index */
  byte acc_msg_seq;             /* EAPM sequence number */
  dword rand;                   /* random challenge value */
} caii_a41_rand_type;

typedef struct
{
  byte access_mode;              /* Access mode used for the EACH */
  word access_mode_min_duration; /* Minimum message duration for the corresponding Access mode */
  word access_mode_max_duration; /* Maximum message duration for the corresponding Access mode */
} caii_access_mode_rec_type;

typedef struct
{
  byte applicable_modes;      /* access modes to these records apply */
  byte each_nom_pwr;          /* nominal transmit power offset for the EACH */
  byte each_init_pwr;         /* initial pwoer offset for the EACH */
  byte each_pwr_step;         /* power increment for the EACH */
  byte each_num_step;         /* Number of access probes */
  boolean each_preamble_enabled;  /* premable enabled indicator for EACH */
  byte each_preamble_num_frac; /* number of fractional premables on the EACH */
  byte each_preamble_frac_duration; /* fractional preamble duration on EACH */
  byte each_preamble_off_duration;  /* fractional preamble gated-off durction on EACH */
  byte each_preamble_add_duration;  /* additional premable duration on the EACH */
  byte each_probe_bkoff;       /* EACH probe backoff range */
  byte each_bkoff;             /* EACH probe sequence backoff range */
  byte each_slot;              /* slot duration for the EACHs*/
  byte each_slot_offset1;      /* First slot offset for the EACHs*/
  byte each_slot_offset2;      /* Relative slot offset for the EACHs */
} caii_mode_parm_rec_type;

typedef struct
{
  byte msg_type;              /* Message type */
  word pilot_pn;               /* Pilot PN sequence offset index */
  byte acc_msg_seq;            /* Enhanced access parameters message sequence number */
  boolean psist_parms_incl;       /* Persistence parameters included indicator*/
  byte psist_0_9_each;         /* Persistence value for access overload classes 0-9 */
  byte psist_10_each;          /* persist value for access overload class 10 - test ms*/
  byte psist_11_each;          /* persist value for access overload class 11 - emg ms*/
  byte psist_12_each;          /* persist value for access overload class 12*/
  byte psist_13_each;          /* persist value for access overload class 13 */
  byte psist_14_each;          /* persist value for access overload class 14 */
  byte psist_15_each;          /* persist value for access overload class 15 */
  byte psist_emg;              /* persist value for emergency call for ac overload classes 0-9 */
  byte msg_psist_each;         /* persist mod. for EACH attempts for msg. transmission */
  byte reg_psist_each;         /* persist mod. for EACH attempt for registration */
  byte acc_tmo;                /* Acknowledgement timeout */
  byte max_req_seq;            /* Maximum number of access probe sequence for EACH request */
  byte max_rsp_seq;            /* Maximum number of access probe sequence for EACH response */
  byte num_mode_selection_entries; /* Numbers of entires of the Mode Selection table */
  caii_access_mode_rec_type mode_selection[CAI_MAX_NUM_MODE_SEL_ENTRIES];
                               /* mode selection table */
  byte rlgain_common_pilot;    /* Gain adjustment of EACH or RCCCH relative to reverse pilot */
  byte ic_thresh;              /* interference correction threshhold */
  byte ic_max;                 /* maximum interference correction that can be applied */
  byte num_mode_parm_rec;      /* number of mode specific parameter records */
  caii_mode_parm_rec_type mode_parms[CAI_MAX_NUM_MODE_PARM_REC];
                               /* mode parameter records; after processing, this array is indexed
                                  by access mode (not record number) */
  byte ba_parms_len;           /* Length is zero if BA fields not included */
  byte num_each_ba;            /* number of EACHs used for the Basic Access Mode */
  byte each_ba_rates_supported;/* Supported rate words for the Basic Access mode on the EACH */
  byte ra_parms_len;           /* Length is zero if RA fields not included */
  byte num_each_ra;            /* number of EACHs used for the reservation access mode */
  byte num_cach;               /* number of Common Assignment Channels */
  boolean cach_code_rate;      /* Code rate for the common assignment channels */
  byte cach_code_chan[CAI_MAX_NUM_CACH];
                               /* Code channel index table for the Common Assignment Channel */
  byte num_rccch;              /* number of reverse CCH used for the reservation mode */
  byte rccch_rates_supported;  /* supported rate words on the RCCCH */
  boolean rccch_preamble_enabled; /* Preamble enabled indicator for the RCCCH */
  byte rccch_preamble_num_frac;/* number of fractional preambles on the RCCCH */
  byte rccch_preamble_frac_duration;/* fractional preamble duration for RCCCH */
  byte rccch_preamble_off_duration;/* fractional preamble gated-off duration on RCCCH */
  byte rccch_preamble_add_duration;/* additional preamble duration on RCCCH */
  byte rccch_slot;             /* slot interval for the RCCCH */
  byte rccch_slot_offset1;     /* first slot offset for the RCCCH */
  byte rccch_slot_offset2;     /* second slot offset for the RCCCH */
  byte rccch_nom_pwr;          /* nominal transmit power offset for the TCCCH */
  byte rccch_init_pwr;         /* initial pwoer offset for the RCCCH */
  byte ra_pc_delay;            /* Power control delay for the RCCCH */
  byte eacam_cach_delay;       /* max. time after EAC header for response */
  boolean rccch_ho_supported;     /* RCCCH handoff supported indicator */
  byte rccch_ho_thresh;        /* RCCCH soft handoff threshold */
  byte eacam_pccam_delay;      /* maximum response delay from RCCH soft handoff request  */
  byte num_cpcch;              /* number of CPCCHs  */
  byte cpcch_rate;             /* power control rate for the CPCCH */
  byte cpcch_code_chan[CAI_MAX_NUM_CPCCH];
                               /* code channel index table for the CPCCH */
  byte num_pcsch_ra;           /* number of power control subchannels used for RAM */
  boolean acct_incl;           /* ACCT info included */
  boolean acct_incl_emg;       /* EMG call included indicator */
  boolean acct_aoc_bitmap_incl;  /* Access Overload info included */
  boolean acct_so_incl;        /* SO info included */
  byte num_acct_so;            /* Number of SO for ACCT */
  struct
  {
    byte acct_aoc_bitmap1;     /* AOC bitmap */
    word acct_so;              /* ACCT SOs */
  } acct_so_info[CAI_MAX_ACCT_SO_NUM];
  boolean acct_so_grp_incl;    /* SO group indicator */
  byte num_acct_so_grp;        /* Number of SO groups */
  struct
  {
    byte acct_aoc_bitmap2;     /* AOC bitmap */
    byte acct_so_grp;          /* SO group */
  } acct_so_grp_info[CAI_MAX_ACCT_SO_GRP_NUM];
} caii_en_ac_parm_type;

typedef struct
{
  byte nghbr_config;          /* Neighbor configuration */
  word nghbr_pn;              /* Neighbor pilot PN sequence offset index */
  boolean bcch_support;       /* BCCH support indicator */
  boolean add_pilot_rec_incl; /* additional pilot information included indicator */
  byte nghbr_pilot_rec_type;  /* neighbor pilot rec type */
  caii_add_pilot_rec_type pilot_rec;
                              /* pilot record */
  byte search_priority;       /* pilot channel search priority */
  byte srch_win_nghbr;        /* neighbor pch search window size */
  byte srch_offset_nghbr;     /* neihgbor pch search window size offset */
  boolean freq_incl;          /* frequency included indicator */
  byte nghbr_band;            /* neighbor band class */
  word nghbr_freq;            /* neighbor frequency assignment */
  boolean timing_incl;        /* timeing included indicator */
  byte nghbr_tx_offset;       /* neighbor transmit time offset */
  byte nghbr_tx_duration;     /* neighbor transmit time duration */
  byte nghbr_tx_period;       /* neighbor transmit time period */
  boolean access_entry_ho;    /* access entry handoff permitted on system access state entry */
  boolean access_ho_allowed;  /* access handof and probe permitted for pilot while sas */
} caii_ri_type0_nghbr_type;

typedef struct
{
  byte pilot_inc;             /* Pilot PN sequence offset index increment */
  byte nghbr_srch_mode;       /* Search mode */
  byte srch_win_n;            /* Search window size for the Neighbor set */
  boolean srch_offset_incl;   /* Neighbor pilot channel search window offset included */
  boolean freq_fields_incl;   /* Frequency fields included */
  boolean use_timing;         /* use timing indicator */
  boolean global_timing_incl; /* Global timing included */
  byte global_tx_duration;    /* Global neighbor transmit time duration */
  byte global_tx_period;      /* Global neighbor transmit time period */
  boolean nghbr_set_entry_info;   /* Neighbor set access entry handoff information included */
  boolean nghbr_set_access_info;  /* Neighbor set accesss handoff included indicator */
  byte num_nghbr;             /* number of neighbor PN sequences */
  caii_ri_type0_nghbr_type nghbr[CAI_UNLM_MAX_NUM_NGHBR];
                              /* neighbor table */
#ifdef FEATURE_IS2000_REL_B
  boolean is2000_rel_b_incl;
  boolean resq_enabled;            /* Call rescue feature enabled indicator */
  byte resq_delay_time;            /* Call rescue delay timer value */
  byte resq_allowed_time;          /* Call rescue allowed timer value */
  byte resq_attempt_time;          /* Call rescue attempt timer value */
  word resq_code_chan;             /* Code channel index for the Rescue Channel */
  byte resq_qof;                   /* Quasi-Orthogonal Function mask identifier
                                      for the Rescue Channel */
  boolean resq_min_period_incl;    /* Minimum time between consecutive rescues
                                      included indicator */
  byte resq_min_period;            /* Minimum time between consecutive rescues */
  boolean resq_num_tot_trans_incl; /* Required # of transmissions before
                                      declaring L2 acknowledgment Failure when
                                      Call Rescue is enabled included
                                      indicator */
  byte resq_num_tot_trans_20ms;    /* Required # of transmissions of a regular
                                      PDU before declaring L2 Acknowledgement
                                      Failure when Call Rescue is enabled */
  byte resq_num_tot_trans_5ms;     /* Required # of transmissions of a mini
                                      PDU before declaring L2 Acknowledgement
                                      Failure when Call Rescue is enabled */
  byte resq_num_preamble_rc1_rc2;  /* Traffic Channel preamble length for Call
                                      Rescue Soft Handoff when operating in RC
                                      1 or 2 */
  byte resq_num_preamble;          /* Traffic Channel preamble length for Call
                                      Rescue Soft Handoff when operating in RC
                                      greater than 2 */
  byte  resq_power_delta;          /* Neighbor Rescue Channel configured
                                      indicator */
  boolean nghbr_resq_configured[CAI_UNLM_MAX_NUM_NGHBR];
                                   /* Neighbor Rescue Channel configured indicator */
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C
  boolean is2000_rel_c_incl;
  boolean nghbr_pdch_supported[CAI_UNLM_MAX_NUM_NGHBR];
                                   /* Neighbor PDCH supported indicator */
#endif /* FEATURE_IS2000_REL_C */
} caii_ri_type0_rec_type;

typedef struct
{
  byte band_class;            /* band class */
  byte sys_a_b;               /* System A/B */
} caii_ri_type1_nghbr_type;

#define CAII_MAX_NUM_ANALOG_NGHBR 7 /* from length of field - 3 bits */

typedef struct
{
  byte num_analog_nghbr;      /* number of neighboring analog systems */
  caii_ri_type1_nghbr_type nghbr[CAI_UNLM_MAX_NUM_ANALOG_NGHBR];
} caii_ri_type1_rec_type;

#ifdef FEATURE_IS2000_REL_C
typedef PACK (struct)
{
  word nghbr_pn;                /* Neighbor pilot PN sequence offset index */
  boolean nghbr_freq_incl;      /* Neighbor frequency information included indicator */
  byte nghbr_band;              /* Neighbor band class */
  word nghbr_freq;              /* Neighbor frequency assignment */
  boolean pn_association_ind;   /* Neighbor PN association indicator */
  boolean data_association_ind; /* Neighbor data association indicator */
} caii_ri_type2_nghbr_type;

typedef struct
{
  byte num_nghbr;               /* number of neighboring HRPD systems */
  caii_ri_type2_nghbr_type nghbr[CAI_UNLM_MAX_NUM_NGHBR];
} caii_ri_type2_rec_type;
#endif /* FEATURE_IS2000_REL_C */

typedef union
{
  caii_ri_type0_rec_type type0;   /* union structure for type 0 - MC system  */
  caii_ri_type1_rec_type type1;   /* union structure for type 1 - analog system */
#ifdef FEATURE_IS2000_REL_C
  caii_ri_type2_rec_type type2;   /* union structure for type 2 - HRPD system */
#endif /* FEATURE_IS2000_REL_C */
} caii_univ_nl_gen_ri_type;

typedef struct
{
  byte radio_interface_type;   /* the radio interface type */
  caii_univ_nl_gen_ri_type ri; /* the type specfic radio interface */
} caii_univ_nl_ri_type;

typedef struct
{
  byte msg_type;               /* Message type */
  word pilot_pn;               /* Pilot PN sequence offset index  */
  byte config_msg_seq;         /* Configuration message sequence number */
  byte num_radio_interface;    /* number of interface types  */
  caii_univ_nl_ri_type ri_info[CAI_MAX_NUM_RADIO_INTERFACE];
                               /* radio interface info table */
} caii_univ_nl_type;

/* Security Mode Command Message (SMCM), IS2000A, 3.7.2.3.2.35 */
typedef struct
{
  byte msg_type;             /* Message type */
  caii_pc_hdr_type hdr;      /* Paging Channel common header fields */
  byte c_sig_encrypt_mode;   /* Encryption mode indicator */
  byte enc_key_size;         /* Encryption key size */
#ifdef FEATURE_IS2000_REL_C
  boolean is2000_rel_c_incl; /* IS2000 Release C fields included indicator */
  boolean msg_int_info_incl; /* Message integrity information included indicator */
  boolean change_keys;       /* Cahnge keys indicator */
  boolean use_uak;           /* Use UAK indicator */
#endif /* FEATURE_IS2000_REL_C */
} caii_sec_mode_cmd_type;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
/* Authentication Request Message (AUREQM) */
typedef struct
{
  byte msg_type;        /* Message type */
  caii_pc_hdr_type hdr; /* Paging Channel common header fields */
  qword randa_hi;       /* Most significant qword of random challenge number */
  qword randa_lo;       /* Least significant qword of random challenge number */
  qword con_sqn;        /* Concealed sequence number */
  word  amf;            /* Authentication management field */
  qword mac_a;          /* Message authentication code */
} caii_auth_req_type;
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_1X_ADV
/*----------------------------------------------------------------------*/
/* RADIO CONFIGURATION PARAMETERS */

/* Radio Configuration Parameters */
typedef struct
{
  word pilot_pn;                        /* Pilot PN of active pilot set*/
  struct
  {
    byte qof_set_in_use;                /* QOF Set in use */

    boolean rev_sch0_ack_bit_inc;       /* Rev SCH0 ACK Mask Included */
    byte rev_sch0_ack_ch_walsh;         /* Rev SCH0 ACK Chn Walsh Code */
    byte rev_sch0_ack_bit;              /* Rev SCH0 ACK Bit */

    boolean rev_sch1_ack_bit_inc;       /* Rev SCH1 ACK Mask Included */
    byte rev_sch1_ack_ch_walsh;         /* Rev SCH1 ACK Chn Walsh Code */
    byte rev_sch1_ack_bit;              /* Rev SCH1 ACK Bit */

    boolean for_fch_ccsh_interleaver_type;  /* Interleaver type used for Fwd FCH RC 12 */
  } rc_pilot_params;
} caii_rc_params_record_type;

typedef struct
{
  boolean use_time;                /* Use action time indicator */
  byte action_time;                /* Action time */

  /* Forward FCH */
  boolean for_fch_ack_mask_rl_blanking_inc;    /* Fwd FCH ACK Mask for RL Blanking Included */
  word for_fch_ack_mask_rl_blanking;           /* Fwd FCH ACK Mask for RL Blanking */
  boolean for_fch_ack_mask_no_rl_blanking_inc; /* Fwd FCH ACK Mask for No RL Blanking Included */
  word for_fch_ack_mask_no_rl_blanking;        /* Fwd FCH ACK Mask for No RL Blanking */
  /* Reverse FCH */
  boolean rev_fch_ack_mask_inc;                /* Rev FCH ACK Mask Included */
  word rev_fch_ack_mask;                       /* Rev FCH ACK Mask */
  /* Forward SCH */
  boolean for_sch_ack_mask_rl_blanking_inc;    /* Fwd SCH ACK Mask for RL Blanking Included */
  word for_sch_ack_mask_rl_blanking;           /* Fwd SCH ACK Mask for RL Blanking */
  boolean for_sch_ack_mask_no_rl_blanking_inc; /* Fwd SCH ACK Mask for No RL Blanking Included */
  word for_sch_ack_mask_no_rl_blanking;        /* Fwd SCH ACK Mask for No RL Blanking */
  /* Reverse SCH */
  boolean rev_sch_ack_mask_inc;                /* Rev SCH ACK Mask Included */
  word rev_sch_ack_mask;                       /* Rev SCH ACK Mask */

  /* Other Paramters */
  byte for_n2m_ind;                            /* Fwd N2M Indicator */
  byte fpc_mode;                               /* Forward Power Control Operation Mode Indicator */
  byte power_control_mode;                     /* Power Control Mode */
  byte pwr_cntl_step;                          /* Power control step size */

  byte for_fch_blanking_dutycycle;             /* Duty cycle value for Fwd Link */
  byte rev_fch_blanking_dutycycle;             /* Duty cycle value for Rev Link */
  boolean rev_gain_adj_acs1_inc;               /* Chn adj gain for Rev Ack Chn Inc (No Cells =1) */
  byte rev_gain_adj_acs1;                      /* Chn adj gain for Rev Ack Chn (No Cells =1) */
  boolean rev_gain_adj_acs2plus_inc;           /* Chn adj gain for Rev Ack Chn Inc (No Cells >1)*/
  byte rev_gain_adj_acs2plus;                  /* Chn adj gain for Rev Ack Chn (No Cells >1)*/
  byte num_rc_params_rec;                      /* Number of RC Parameters records */
  caii_rc_params_record_type rc_params_records[CAI_RC_PARM_MAX]; /* RC Param Records */
} caii_radio_config_params_type;

/*----------------------------------------------------------------------*/
/* GENERAL EXTENSION MESSAGE */

/* General Extension Message (GEM) Reverse Channel Record type */
typedef struct
{
  byte band_class; /* Band Class of the Reverse Channel */
  word rev_chan;   /* Channel number of the reverse link frequency */
} caii_gem_rev_chn_rec_type;

/* Union of General Extension Record types */
typedef union
{
  caii_gem_rev_chn_rec_type rev_chn_info; /* Reverse channel information */
  caii_radio_config_params_type rcp;   /* Radio Configuration Parameters */
} caii_gem_rec_type;

typedef struct
{
  byte ge_rec_type;             /* Type of General Extension Record */
  byte ge_rec_len;              /* Length of the General Extension Record */
  caii_gem_rec_type ge_rec;     /* General Extension Record */
} caii_gen_extn_rec_type;

/* Paging Channel General Extension Message */
typedef struct
{
  byte msg_type;                /* Message type */
  caii_pc_hdr_type hdr;         /* Paging Channel common header fields */
  byte num_ge_rec;              /* Number of General Extension Records
                                   included in this message */
  caii_gen_extn_rec_type gen_extn_rec[CAI_MAX_NUM_GE_REC];
                                /* General Extension Records included in the message */
  byte message_type;            /* Message type of the message included in the General
                                   Extension Message */
  byte *message_rec;            /* Pointer to message extended through the
                                   General Extension message */
} caii_pc_gen_extn_type;
#endif /* FEATURE_IS2000_1X_ADV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                      ACCESS CHANNEL MESSAGES                            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PILOT REPORT ( see IS-95B Section 6.7.1.3.2 )                           */

typedef struct
{
  byte act_plt_str;       /* Active Pilot strength */
  boolean first_is_act;   /* Active Pilot is the first pilot on which an
                             access probe was sent */
  boolean first_is_pta;   /* The first pilo tis the previous to the active
                             pilot on which an access probe was sent */
  byte num_add_plt;       /* Number of additional reported pilots */
} caii_plt_rpt_fix_type;

typedef struct
{
  word plt_pn_phase;      /* Pilot measured phase */
  byte plt_strength;      /* Pilot strength */
  boolean acc_ho_en;      /* Access Handoff enable */
  boolean acc_attempted;  /* Access attempted flag */
} caii_plt_rpt_var_type;

#ifdef FEATURE_IS2000_REL_A
/* Aux Pilot fix type */
typedef struct
{
  byte num_aux_plt;       /* Number of additional reported pilots */
} caii_aux_plt_rpt_fix_type;


#define CAI_AUX_PILOT_TYPE_0    0

/* Aux Pilot Type '000' related fields */
typedef struct
{
  byte qof;             /* Index of Quasi-orthogonal function */
  byte walsh_length;    /* Length of the walsh code */
  word pilot_walsh;     /* Aux Pilot Walsh code */
} caii_aux_plt_rec0_type;

typedef struct
{
  word plt_pn_phase;      /* Pilot measured phase */
  byte plt_strength;      /* Pilot strength */
  byte plt_rec_type;      /* Access Handoff enable */
  caii_aux_plt_rec0_type rec0;
} caii_aux_plt_rpt_var_type;


#endif /* FEATURE_IS2000_REL_A */

typedef struct
{
  caii_plt_rpt_fix_type fix;
  caii_plt_rpt_var_type var[CAI_MAX_ADD_PILOTS];
#ifdef FEATURE_IS2000_REL_A
  caii_aux_plt_rpt_fix_type aux_fix;
  caii_aux_plt_rpt_var_type aux_var[CAI_MAX_AUX_PILOTS];
#endif /* FEATURE_IS2000_REL_A */
} caii_plt_rpt_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* ACCESS CHANNEL MESSAGE HEADER                                           */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_ac_hdr_type hdr;  /* Layer 2 header */
} caii_gen_ac_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REGISTRATION MESSAGE (see IS-95A Section 6.7.1.3.2.1)                  */

typedef struct
{
  byte msg_type;                 /* Message type */
  caii_ac_hdr_type hdr;          /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami; /* Auth fields and message integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;          /* P_REV 7 fields */
#endif /* FEATURE_IS2000_REL_A */
  byte reg_type;                 /* Registration type */
  byte slot_cycle_index;         /* Slot cycle mode */
  byte mob_p_rev;                /* Protocol revision of the mobile station */
  byte scm;                      /* Station class mark */
  boolean mob_term;              /* Mobile terminated calls accepted indicator */
  byte return_cause;             /* Return Cause for IS-95B */
  caii_plt_rpt_type pilot_rpt;
  boolean qpch_supported;
  boolean enhanced_rc;
  boolean uzid_incl;
  word uzid;
#ifdef FEATURE_IS2000_REL_A
  boolean geo_loc_incl;
  byte geo_loc_type;
  boolean otd_supported;
  boolean sts_supported;
  boolean thx_cch_supported;
  boolean wll_incl;
  byte wll_device_type;
  byte hook_status;
  boolean enc_info_incl;
  byte sig_encrypt_sup;
  boolean c_sig_encrypt_req;
  dword enc_seq_h;
  byte enc_seq_h_sig;
  byte ui_encrypt_sup;
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_C
  boolean msg_int_info_incl;
  boolean sig_integrity_sup_incl;
  byte sig_integrity_sup;
  byte sig_integrity_req;
  byte new_key_id;
  boolean new_sseq_h_incl;
  dword new_sseq_h;
  byte new_sseq_h_sig;
#endif /* FEATURE_IS2000_REL_C*/

} caii_reg_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* ACCESS CHANNEL ORDER MESSSAGE (see IS-95A Section 6.7.1.3.2.2)         */

/* Generic Order type */
typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and Message Integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;
#endif /* FEATURE_IS2000_REL_A */
  caii_plt_rpt_type pilot_rpt;
  byte order;            /* Order code */
} caii_ac_gen_ord_type;

/* Order Message with Order Qualifier */
typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and Message Integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;
#endif /* FEATURE_IS2000_REL_A */
  caii_plt_rpt_type pilot_rpt;
  byte order;                   /* Order code */
  byte ordq;                    /* Order qualification code */
} caii_ac_ordq_type;

/* Base Station Challenge Order */
typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and Message Integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;
#endif /* FEATURE_IS2000_REL_A */
  caii_plt_rpt_type pilot_rpt;
  byte order;                   /* Order code */
  byte ordq;                    /* Order qualification code */
  dword randbs;                 /* Random challenge data */
} caii_ac_bs_chal_type;

/* Mobile Station Reject Orders */
typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and Message Integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;
#endif /* FEATURE_IS2000_REL_A */
  caii_plt_rpt_type pilot_rpt;
  byte order;                   /* Order code */
  byte rejected_pdu_type;
  byte ordq;                    /* Order qualification code */
  byte rej_msg_type;            /* Message type of rejected message */
} caii_ac_rej_type;

typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and Message Integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;
#endif /* FEATURE_IS2000_REL_A */
  caii_plt_rpt_type pilot_rpt;
  byte order;                   /* Order code */
  byte rejected_pdu_type;
  byte ordq;                    /* Order qualification code */
  byte rej_msg_type;            /* Message type of rejected message */
  byte rej_order;               /* Order type of rejected message */
  byte rej_ordq;                /* Order qualification code of rejected message */
} caii_ac_rej_ord_type;

typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and Message Integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;
#endif /* FEATURE_IS2000_REL_A */
  caii_plt_rpt_type pilot_rpt;
  byte order;                   /* Order code */
  byte rejected_pdu_type;
  byte ordq;                    /* Order qualification code */
  byte rej_msg_type;            /* Message type of rejected message */
  byte rej_record;              /* Record type of the rejected info record */
} caii_ac_rej_rec_type;

/* Order Message */
typedef union
{
  caii_ac_gen_ord_type gen;
  caii_ac_ordq_type    ordq;
  caii_ac_bs_chal_type chal;
  caii_ac_rej_type     rej;
  caii_ac_rej_ord_type rej_ord;
  caii_ac_rej_rec_type rej_rec;
} caii_ac_ord_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* TMSI ASSIGNMENT C0MPLETION MESSAGE (see J-STD-008 Section 2.7.1.3.2.8)  */

typedef struct  /* TMSI Assignment Completion Message - Access Channel */
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and message integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;         /* Extended Encryption Fields */
#endif /* FEATURE_IS2000_REL_A */

  caii_plt_rpt_type pilot_rpt;
} caii_ac_tmsi_cmp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* DATA BURST MESSAGE (see IS-95A Section 6.7.1.3.2.3)                    */

typedef struct
{
  byte msg_type;                 /* Message type */
  caii_ac_hdr_type hdr;          /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami; /* Auth fields and message integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;          /* Extended Encryption fields for rel A */
#endif /* FEATURE_IS2000_REL_A */
  byte msg_number;               /* Message number within the data burst stream */
  byte burst_type;               /* Data burst type */
  byte num_msgs;                 /* Number of messages in the data burst stream */
  byte num_fields;               /* Number of characters in this message */
  byte chari[CAI_AC_BURST_MAX];  /* Characters */
  caii_plt_rpt_type pilot_rpt;
} caii_ac_burst_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* ORIGINATION MESSAGE (see IS-95A Section 6.7.1.3.2.4)                   */

typedef struct
{
  word alt_so;                   /* Alternative service option */
} caii_alt_so_type;

typedef struct
{
  byte msg_type;                 /* Message type */
  caii_ac_hdr_type hdr;          /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami; /* Auth fields and message integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;          /* P_REV 7 fields */
#endif /* FEATURE_IS2000_REL_A */

  boolean mob_term;              /* Mobile terminated calls accepted indicator */
  byte slot_cycle_index;         /* Slot cycle index */
  byte mob_p_rev;                /* Protocol revision of the mobile station */
  byte scm;                      /* Station class mark */
  byte request_mode;             /* Requested mode code */
  boolean special_service;       /* Speical service option indicator */
  word service_option;           /* Requested service option for this origination
                                    (only present if special_service field is
                                    set to 1) */
  boolean pm;                    /* Privacy mode indicator */
  boolean digit_mode;            /* Digit mode indicator */
  byte number_type;              /* Type of number (only present if digit_mode
                                    field is set to 1) */
  byte number_plan;              /* Numbering plan (only present if digit_mode
                                    field is set to 1) */
  boolean more_fields;           /* More dialed digits indicator */
  byte num_fields;               /* Number of dialed digits in the message */
  byte chari[CAI_AC_ORIG_MAX];   /* Dialed digits */
  boolean nar_an_cap;            /* Narrow analog capability */
  boolean paca_reorig;           /* PACA reorigination for IS-95B */
  byte return_cause;             /* Return cause for IS-95B */
  boolean more_records;          /* More records indicator */
  byte encry_sup;                /* Encryption Supported */
  boolean paca_sup;              /* PACA supported field */
  byte num_alt_so;               /* Number of alternative SO */
  word alt_so[CAI_AC_ALT_SO_MAX];
                                 /* Alternative service options */
  caii_plt_rpt_type pilot_rpt;
  boolean drs;
  boolean uzid_incl;
  word uzid;
  byte ch_ind;
  byte sr_id;
  boolean otd_supported;
  boolean qpch_supported;
  boolean enhanced_rc;
  byte for_rc_pref;
  byte rev_rc_pref;
  boolean fch_supported;
  caii_fch_spec_fields_type fch_fields;
  boolean dcch_supported;
  caii_dcch_spec_fields_type dcch_fields;
#ifdef FEATURE_IS2000_REL_A  /* Don't move these fields */
  boolean geo_loc_incl;
  byte geo_loc_type;
#endif /* FEATURE_IS2000_REL_A */
  boolean rev_fch_gating_req;

#ifdef FEATURE_IS2000_REL_A
  byte orig_reason;
  byte orig_count;
  boolean sts_supported;
  boolean thx_cch_supported;
  boolean wll_incl;
  byte wll_device_type;
  boolean global_emergency_call;
  boolean ms_init_pos_loc_ind;
  boolean qos_parms_incl;
  byte qos_parms_len;
  byte qos_parms[CAI_ORIG_QOS_LEN_MAX];
  boolean enc_info_incl;
  byte sig_encrypt_sup;
  boolean d_sig_encrypt_req;
  boolean c_sig_encrypt_req;
  dword enc_seq_h;
  byte enc_seq_h_sig;
  boolean ui_encrypt_req;
  byte ui_encrypt_sup;
  boolean sync_id_incl;
  byte sync_id_len;
  byte sync_id[CAI_SYNC_ID_LEN_MAX];
  boolean prev_sid_incl;
  word prev_sid;
  boolean prev_nid_incl;
  word prev_nid;
  boolean prev_pzid_incl;
  byte prev_pzid;
  byte so_bitmap_ind;
  byte so_group_num;
  word so_bitmap;
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_B
  boolean sdb_desired_only;
  boolean alt_band_class_sup;
#endif /*FEATURE_IS2000_REL_B*/
#ifdef FEATURE_IS2000_REL_C
  boolean msg_int_info_incl;
  boolean sig_integrity_sup_incl;
  byte sig_integrity_sup;
  byte sig_integrity_req;
  byte new_key_id;
  boolean new_sseq_h_incl;
  dword new_sseq_h;
  byte new_sseq_h_sig;
  boolean for_pdch_supported;
  caii_for_pdch_spec_fields_type for_pdch_fields;
  byte ext_ch_ind;
#endif /* FEATURE_IS2000_REL_C*/

} caii_orig_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PAGE RESPONSE MESSAGE  (see IS-95A Section 6.7.1.3.2.5)                */

typedef struct
{
  byte msg_type;                 /* Message type */
  caii_ac_hdr_type hdr;          /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami; /* Auth fields and message integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;          /* P_REV 7 fields */
#endif /* FEATURE_IS2000_REL_A */
  boolean mob_term;              /* Mobile terminated calls accepted indicator */
  byte slot_cycle_index;         /* Slot cycle index */
  byte mob_p_rev;                /* Protocol revision of the mobile station */
  byte scm;                      /* Station class mark */
  byte request_mode;             /* Requested mode code */
  word service_option;           /* Service option */
  boolean pm;                    /* Privacy mode indicator */
  byte nar_an_cap;               /* Narrow analog capability */
  byte encry_sup;                /* Encryption Supported for IS-95B */
  byte num_alt_so;               /* Number of alternative SO */
  word alt_so[CAI_AC_ALT_SO_MAX];/* Alternative service options */
  caii_plt_rpt_type pilot_rpt;
  boolean uzid_incl;
  word uzid;
  byte ch_ind;
  boolean otd_supported;
  boolean qpch_supported;
  boolean enhanced_rc;
  byte for_rc_pref;
  byte rev_rc_pref;
  boolean fch_supported;
  caii_fch_spec_fields_type fch_fields;
  boolean dcch_supported;
  caii_dcch_spec_fields_type dcch_fields;
  boolean rev_fch_gating_req;
#ifdef FEATURE_IS2000_REL_A
  boolean sts_supported;
  boolean thx_cch_supported;
  boolean wll_incl;
  byte wll_device_type;
  byte hook_status;
  boolean enc_info_incl;
  byte sig_encrypt_sup;
  boolean d_sig_encrypt_req;
  boolean c_sig_encrypt_req;
  dword enc_seq_h;
  byte enc_seq_h_sig;
  boolean ui_encrypt_req;
  byte ui_encrypt_sup;
  boolean sync_id_incl;
  byte sync_id_len;
  byte sync_id[CAI_SYNC_ID_LEN_MAX];
  byte so_bitmap_ind;
  byte so_group_num;
  word so_bitmap;
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_B
  boolean alt_band_class_sup;
#endif /*FEATURE_IS2000_REL_B*/
#ifdef FEATURE_IS2000_REL_C
  boolean msg_int_info_incl;
  boolean sig_integrity_sup_incl;
  byte sig_integrity_sup;
  byte sig_integrity_req;
  byte new_key_id;
  boolean new_sseq_h_incl;
  dword new_sseq_h;
  byte new_sseq_h_sig;
  boolean for_pdch_supported;
  caii_for_pdch_spec_fields_type for_pdch_fields;
  byte ext_ch_ind;
#endif /* FEATURE_IS2000_REL_C*/

} caii_page_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* AUTHENTICATION CHALLENGE RESPONSE MSG (see IS-95A Section 6.7.1.3.2.6) */

typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and message integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;         /* Extended Encryption fields */
#endif /* FEATURE_IS2000_REL_A */
  dword authu;                  /* Authentication challenge response */
  caii_plt_rpt_type pilot_rpt;
} caii_auth_ch_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* STATUS RESPONSE MESSAGE (see IS-95-A TSB)                               */

typedef struct
{
  byte msg_type;            /* Message type */
  caii_ac_hdr_type hdr;     /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and message integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;         /* Extended Encryption fields */
#endif /* FEATURE_IS2000_REL_A */
  byte qual;                /* Qualification information type */
  byte band;                /* Band class qualifier */
  byte op_mode;             /* Operating mode qualifier */
  byte num_recs;            /* Number of information records */
  dword recs[CAII_MAX_INFO_REC_DWORDS];
    /* Records - consecutive Information Records -
       use caii_rec_hdr type to parse different records */
  caii_plt_rpt_type pilot_rpt;
} caii_ac_strsp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PACA CANCEL MESSAGE (see IS-95B Section 6.7.1.3.2.9)                    */

typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and message integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;         /* Extended Encryption fields */
#endif /* FEATURE_IS2000_REL_A */
  caii_plt_rpt_type pilot_rpt;
} caii_ac_paca_can_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* EXTENDED STATUS RESPONSE MESSAGE (see IS-95B Section 6.7.1.3.2.10)      */

typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and message integrity fields */
#ifdef FEATURE_IS2000_REL_A
  caii_ac_enc_type enc;         /* Extended Encryption fields */
#endif /* FEATURE_IS2000_REL_A */
  byte qual;                    /* Qualification information type */
  byte band;                    /* Band class qualifier */
  byte op_mode;                 /* Operating mode qualifier */
  byte  num_recs;               /* Number of information records */
  dword recs[CAII_MAX_INFO_REC_DWORDS];
    /* Records - consecutive Information Records -
       use caii_rec_hdr type to parse different records */
  caii_plt_rpt_type pilot_rpt;
} caii_ac_ext_strsp_type;

#ifdef FEATURE_IS2000_REL_A
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* DEVICE INFORMATION MESSAGE (see IS-2000A Section 2.7.1.3.2.11)                    */

typedef struct
{
  /* LAC fields */
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and message integrity fields */
  caii_ac_enc_type  enc;        /* Encryption fields */
  caii_plt_rpt_type pilot_rpt;  /* Pilot report */

  /* L3 fields */
  byte wll_device_type;         /* WLL device type indicator */
  byte  num_recs;               /* Number of info records included */
  dword recs[CAII_MAX_INFO_REC_DWORDS];
} caii_ac_dim_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SECURITY MODE REQUEST MESSAGE (see IS-2000A Section 2.7.1.3.2.12)                    */

typedef struct
{
  /* LAC fields */
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Access Channel common header fields */
  caii_ac_auth_msg_int_type ami;/* Auth fields and message integrity fields */
  caii_ac_enc_type  enc;        /* Encryption fields */
  caii_plt_rpt_type pilot_rpt;  /* Pilot report */

  /* L3 fields */
  boolean ui_enc_incl;          /* User Infor encryption fields included */
  byte ui_encrypt_sup;          /* User Infor encryption supported indicator */
  boolean sig_enc_incl;         /* Signaling encryption supported included */
  byte sig_encrypt_sup;         /* Signaling encryption supported */
  boolean c_sig_encrypt_req;    /* Signaling encryption request */
  boolean enc_seq_h_incl;       /* ENC_SEQ_H_INCL field */
  dword enc_seq_h;
  byte enc_seq_h_sig;
#ifdef FEATURE_IS2000_REL_C
  boolean  msg_int_info_incl;   /* signaling mesg integrity info included*/
  boolean  sig_integrity_sup_incl; /* signaling mesg integrity algo supported*/
  byte  sig_integrity_sup;      /* signaling integrity algo supported by MS */
  byte  sig_integrity_req;      /* signaling mesg integrity algo requested*/
#endif /* FEATURE_IS2000_REL_C */
} caii_ac_smrm_type;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
/* Authentication Response Message */
typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Layer 2 header */
  caii_ac_auth_msg_int_type ami;/* Auth fields and message integrity fields */
  caii_ac_enc_type  enc;        /* Encryption fields */
  caii_plt_rpt_type pilot_rpt;  /* Pilot report */

  /* Following is a 128-bit field */
  qword res_hi;                 /* O/p of user auth fn */
  qword res_lo;                 /* O/p of user auth fn */

  boolean sig_integrity_sup_incl; /* sig mesg integrity info included indicator*/
  byte sig_integrity_sup;       /* Signaling integrity algo supported by MS */
  byte sig_integrity_req;       /* Signaling integrity algo requested by MS */
  byte new_key_id;              /* 24 bit value used for MSB of crypto sync */
  dword new_sseq_h;
} caii_ac_aurspm_type;

/* Authentication Resynchronization Message */
typedef struct
{
  byte msg_type;                  /* Message type */
  caii_ac_hdr_type hdr;           /* Layer 2 header */
  caii_ac_auth_msg_int_type ami;  /* Auth fields and message integrity fields */
  caii_ac_enc_type  enc;          /* Encryption fields */
  caii_plt_rpt_type pilot_rpt;    /* Pilot report */

  qword con_ms_sqn;               /* Conceled seq num of auth vector */
  qword mac_s;                    /* Msg auth code for resynchronization */
} caii_ac_aursynm_type;

/* Reconnect Message */
typedef struct
{
  byte msg_type;                /* Message type */
  caii_ac_hdr_type hdr;         /* Layer 2 header */
  caii_ac_auth_msg_int_type ami;/* Auth fields and message integrity fields */
  caii_ac_enc_type  enc;        /* Encryption fields */
  caii_plt_rpt_type pilot_rpt;  /* Pilot report */
  boolean orig_ind;             /* Indicates if this is instead of orig or a page response */
  boolean sync_id_incl;         /* Sync ID included */
  byte sync_id_len;             /* Sync ID len */
  byte sync_id[CAI_SYNC_ID_LEN_MAX]; /* Sync ID */
  word service_option;          /* service option */
  byte sr_id;                   /* SR_ID */

} caii_ac_rcnm_type;
#endif /* FEATURE_IS2000_REL_C */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  FORWARD TRAFFIC CHANNEL MESSAGES                       */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* FORWARD TRAFFIC CHANNEL MESSAGE HEADER                                  */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
} caii_gen_tc_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* FORWARD TRAFFIC CHANNEL ORDER MESSAGE (see IS-95A Section 7.7.3.3.2.1) */

/* Generic Order Message */
typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte use_time;               /* Use action time indicator */
  byte action_time;            /* Action time */
  byte order;                  /* Order field */
#ifdef FEATURE_IS2000_REL_A
  boolean con_ref_incl;
  byte    con_ref;
#endif /* FEATURE_IS2000_REL_A */
} caii_ftc_gen_ord_type;

/* Order Message with Order Qualifier */
typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte use_time;               /* Use action time indicator */
  byte action_time;            /* Action time */
  byte order;                  /* Order field */
  byte ordq;                   /* Order qualification field */
#ifdef FEATURE_IS2000_REL_A
  boolean con_ref_incl;
  byte    con_ref;
#endif /* FEATURE_IS2000_REL_A */
} caii_ftc_ordq_type;

/* Base Station Challenge Confirmation Order (see IS-95A Section 7.7.4.1) */
typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte use_time;               /* Use action time indicator */
  byte action_time;            /* Action time */
  byte order;                  /* Order field */
  byte ordq;                   /* Order qualification field */
  dword authbs;                /* Challenge response */
} caii_ftc_chal_type;

/* Service Option Request Order (see IS-95A Section 7.7.4.2) */
typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte use_time;               /* Use action time indicator */
  byte action_time;            /* Action time */
  byte order;                  /* Order field */
  byte ordq;                   /* Order qualification field */
  word service_option;         /* Service Option */
} caii_ftc_so_req_type;

/* Service Option Response Order (see IS-95A Section 7.7.4.3) */
typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte use_time;               /* Use action time indicator */
  byte action_time;            /* Action time */
  byte order;                  /* Order field */
  byte ordq;                   /* Order qualification field */
  word service_option;         /* Service Option */
} caii_ftc_so_res_type;

/* Status Request Order (see IS-95A Section 7.7.4.4) */
typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte use_time;               /* Use action time indicator */
  byte action_time;            /* Action time */
  byte order;                  /* Order field */
  byte ordq;                   /* Order qualification field */
  word sid;                    /* System identification number - only present if
                                  ordq == CAI_ID_REQ */
  word nid;                    /* Network identification number - only present if
                                  ordq == CAI_ID_REQ */
} caii_ftc_stat_req_type;

/* Periodic Pilot Measurement Order (see IS-95B Section 7.7.4.6) */
typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte use_time;               /* Use action time indicator */
  byte action_time;            /* Action time */
  byte order;                  /* Order field */
  byte ordq;                   /* Order qualification code */
  byte min_pilot_pwr_thresh;   /* Threshold of Ec of pilots in Active set */
  byte min_pilot_ec_io_thresh; /* Pilot Strength Threshold of Serving Frequency */
#ifdef FEATURE_IS2000_REL_A
  boolean incl_setpt;          /* set point included */
#endif
} caii_ftc_ppm_type;

typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte use_time;               /* Use action time indicator */
  byte action_time;            /* Action time */
  byte order;                  /* Order field */
  byte ordq;                   /* Order qualification code */
  byte retry_type;
  byte retry_delay;
} caii_ftc_retry_type;

#ifdef FEATURE_IS2000_REL_A
/* Base Station Reject Order (see Section 3.7.4.9) */
typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte use_time;               /* Use action time indicator */
  byte action_time;            /* Action time */
  byte order;                  /* Order field */
  byte ordq;                   /* Order qualification field */
#ifdef FEATURE_IS2000_REL_C
  byte reject_reason;
  byte rejected_msg_type;
  byte rejected_msg_seq;
#endif /* FEATURE_IS2000_REL_C */
} caii_ftc_bs_rej_type;
#endif /* FEATURE_IS2000_REL_A */

/* Order Message */
typedef union
{
  caii_ftc_gen_ord_type           gen;
  caii_ftc_ordq_type              ordq;
  caii_ftc_chal_type              chal;
  caii_ftc_so_req_type            so_req;
  caii_ftc_so_res_type            so_res;
  caii_ftc_stat_req_type          stat_req;
  caii_ftc_ppm_type               ppm;
  caii_ftc_retry_type             retry;
#ifdef FEATURE_IS2000_REL_A
  caii_ftc_bs_rej_type            bsrej;
#endif /* FEATURE_IS2000_REL_A */

} caii_ftc_ord_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* AUTHENTICATION CHALLENGE MESSAGE (see IS-95A Section 7.7.3.3.2.2)      */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  dword randu;           /* Random challenge data */

} caii_auth_ch_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* ALERT WITH INFORMATION MESSAGE (see IS-95A Section 7.7.3.3.2.3)        */

typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  byte num_recs;              /* Number of records in message */
  dword recs[CAII_MAX_INFO_REC_DWORDS];
    /* Records - consecutive Forward Traffic Channel Information Records -
       use caii_rec_hdr type to parse different records */
} caii_alert_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* TMSI ASSIGNMENT MESSAGE (see J-STD-008 Section 3.7.3.3.2.22)            */

typedef struct  /* TMSI Assignment Message - Forward Traffic Channel */
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  caii_tmsi_data_type data;   /* TMSI Assignment Message data */
} caii_ftc_tmsi_asgn_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* DATA BURST MESSAGE (see IS-95A Section 7.7.3.3.2.4)                    */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  byte msg_number;       /* Number of message in the data stream burst */
  byte burst_type;       /* Type of data burst */
  byte num_msgs;         /* Number of messages in the data stream burst */
  byte num_fields;       /* Number of characters in chari field */
  byte chari[CAI_FWD_TC_BURST_MAX];
} caii_fwd_tc_burst_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* ANALOG HANDOFF DIRECTION MESSAGE (see IS-95A Section 7.7.3.3.2.6)      */

typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  boolean use_time;          /* Indicator of whether the action_time field
                                should be used */
  byte action_time;          /* Time at which order should be executed */
  word sid;                  /* System ID of analog system */
  byte vmac;                 /* Voice mobile station attenuation code */
  word analog_chan;          /* Analog voice channel number */
  byte scc;                  /* SAT color code */
  boolean mem;               /* Message encryption mode indicator */
  byte an_chan_type;         /* Analog voice channel type */
  byte dscc_msb;             /* DSCC most significant bit */
  byte band_class;           /* Band class for IS-95B */
  byte dtx;                  /* Discontinuous transmission */
#ifdef FEATURE_IS2000_REL_A
  boolean con_ref_incl;
  byte    con_ref;
#endif /* FEATURE_IS2000_REL_A */
} caii_fm_ho_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* IN-TRAFFIC SYSTEM PARAMETER MESSAGE (see IS-95A Section 7.7.3.3.2.7)   */

typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  word sid;                  /* System identification */
  word nid;                  /* Network identification */
  byte srch_win_a;           /* Characteristic field for Active Pilot Set */
  byte srch_win_n;           /* Characteristic field for Neighbor Pilot Set */
  byte srch_win_r;           /* Characteristic field for the Remaining Pilot set */
  byte t_add;                /* Pilot detection threshold */
  byte t_drop;               /* Pilot drop threshold */
  byte t_comp;               /* Active Set vs Candidate set comparsion
                                threshold */
  byte t_tdrop;              /* Drop timer value */
  byte nghbr_max_age;        /* Neighbor set member maximum age for retention */
  byte p_rev;                /* Protocol Revision Level */
  byte soft_slope;           /* Slope in criterion to add/drop active set pilots*/
  byte add_intercept;        /* Intercept in criterion to add to active set */
  byte drop_intercept;       /* Intercept in criterion for dropping pilot */
  byte packet_zone_id;       /* Packet data services zone identifier */
  boolean extension;         /* Indicator that extension fields are present */
  byte t_mulchan;            /* Supplemental Channel Request Message pilot
                                strength reporting offset. */
  byte begin_preamble;       /* Number of preamble frames at beginning */
  byte resume_preamble;      /* Number of preamble frames at resumption */
  boolean t_slotted_incl;
  byte t_slotted;
#ifdef FEATURE_IS2000_REL_A
  boolean enc_supported;
  byte    sig_encrypt_sup;
  byte    ui_encrypt_sup;
  boolean cs_supported;
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_C
  boolean chm_supported;
  boolean cdma_off_time_rep_sup_ind;
  boolean cdma_off_time_rep_threshold_unit;
  byte    cdma_off_time_rep_threshold;
  boolean t_tdrop_range_incl;
  byte    t_tdrop_range;
  boolean for_pdch_supported;
  boolean pdch_chm_supported;
  boolean sdb_supported;
  boolean mob_qos;
  boolean ms_init_pos_loc_sup_ind;
#endif /* FEATURE_IS2000_REL_C */
} caii_tc_sysparm_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* NEIGHBOR LIST UPDATE MESSAGE (see IS-95A Section 7.7.3.3.2.8)          */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  byte pilot_inc;        /* Pilot Index Increment value */
  word num_nghbr;        /* Number of entries in the nghbr array */
  struct
  {
    word nghbr_pn;       /* Offset of neighbor's PN pilot offset relative
                            to system time */
  } nghbr[CAI_NLU_MAX];
} caii_nlu_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SEND BURST DTMF MESSAGE (see IS-95A Section 7.7.3.3.2.9)               */

typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  byte num_digits;            /* Number of DTMF digits in message */
  byte dtmf_on_length;        /* DTMF pulse width code */
  byte dtmf_off_length;       /* DTMF interdigit interval code */
  byte digit[CAI_SBDTMF_MAX]; /* Array of 4 bit representation of digits */
#ifdef FEATURE_IS2000_REL_A
  boolean con_ref_incl;
  byte    con_ref;
#endif /* FEATURE_IS2000_REL_A */
} caii_sbdtmf_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* POWER CONTROL PARAMETERS MESSAGE (see IS-95A Section 7.7.3.3.2.10)     */

typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  byte pwr_rep_thresh;        /* Power control reporting threshold */
  byte pwr_rep_frames;        /* Power control reporting frame count */
  boolean pwr_thresh_enable;  /* Threshold report mode indicator */
  boolean pwr_period_enable;  /* Threshold report mode indicator */
  byte pwr_rep_delay;         /* Power Measurement report delay */
} caii_pwr_ctl_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* RETRIEVE PARAMETERS MESSAGE (see IS-95A Section 7.7.3.3.2.11)          */

typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  word num_parms;            /* Number of parameters in parameter_id array */
  word parameter_id[CAI_RET_PARMS_MAX];  /* Array of Parameter ids */
} caii_ret_parms_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  SET PARAMETERS MESSAGE (see IS-95A Section 7.7.3.3.2.12)              */

typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  word num_parms;            /* Number of parameters in parms array */
  struct
  {
    word parameter_id;       /* Parameter identification */
    word parameter_len;      /* length of parameter */
    union
    {
      byte parm_b;           /* byte length parameter */
      word parm_w;           /* word length parameter */
      dword parm_d;          /* dword length parameter */
      qword parm_q;          /* qword length parameter */
    } parameter;
  } parms[CAI_SET_PARM_MAX]; /* Array of parameters */
} caii_set_parm_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SSD UPDATE MESSAGE (see IS-95A Section 7.7.3.3.2.13)                   */

typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  qword randssd;               /* Random data */
} caii_ssd_up_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* FLASH WITH INFORMATION MESSAGE (See IS-95A Section 7.7.3.3.2.14)       */

/* Use caii_alert_type from Alert With Information record type
   definition above */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  MOBILE STATION REGISTERED MESSAGE (see IS-95A Section 7.7.3.3.2.15)   */

typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  word sid;                    /* System identification */
  word nid;                    /* Netword identification */
  word reg_zone;               /* Registration zone */
  byte total_zones;            /* Number of registration zones to be
                                  retained */
  byte zone_timer;             /* Zone timer length */
  boolean mult_sids;           /* Multiple SID storage indicator */
  boolean mult_nids;           /* Multiple NID storage indicator */
  dword base_lat;              /* Base station latitude */
  dword base_long;             /* Base Station longitude */
  word reg_dist;               /* Registration distance */
} caii_registered_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  STATUS REQUEST MESSAGE (see IS-95-A TSB)                               */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  byte qual;             /* Qualification information */
  byte band;             /* Band class qualifier */
  byte op_mode;          /* Operating mode qualifier */
  byte num_fields;       /* Number of active elements in rec_typ */
  byte rec_typ[ CAI_REC_TYP_MAX ]; /* List of requested rec types */
} caii_tc_streq_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  SERVICE REQUEST MESSAGE (See IS-95-A TSB)                              */

typedef struct
{
  byte  msg_type;            /* Message type */
  caii_tc_hdr_type  hdr;     /* Layer 2 header */
  byte  req_seq;             /* Request sequence number */
  byte  purp;                /* Purpose code */
  caii_srv_cfg_type  cfg;    /* Service configuration record */
} caii_ftc_srv_req_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  SERVICE RESPONSE MESSAGE (See IS-95-A TSB)                             */

typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte req_seq;
  byte purp;
  caii_srv_cfg_type cfg;
} caii_ftc_srv_rsp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  SERVICE CONNECT MESSAGE (See IS-95-A TSB)                              */


typedef struct
{
  boolean cc_info_incl;
  byte    num_calls_assign;
  struct
  {
    byte con_ref;
    boolean response_ind;
    byte tag;
    boolean bypass_alert_answer;
  } conn[CAI_SCM_MAX_NUM_CALLS_ASSIGN];
} caii_cc_info_type;

typedef struct
{
  byte  msg_type;            /* Message type */
  caii_tc_hdr_type  hdr;     /* Layer 2 header */
  boolean  use_time;         /* Use action time indicator */
  byte  action_time;         /* Action time */
  byte  con_seq;             /* Connect sequence number */
#ifdef FEATURE_IS2000_REL_A
  byte  use_old_srv_cfg;     /* Indication to use the old record */
#ifdef FEATURE_IS2000_REL_C
  byte sr_id;
#endif /* FEATURE_IS2000_REL_C */

  boolean sync_id_incl;
  byte  sync_id_len;         /* valid only if sync_id_incl == TRUE */
  byte  sync_id[CAI_SYNC_ID_LEN_MAX];
#endif /* FEATURE_IS2000_REL_A */
  caii_srv_cfg_type  cfg;    /* Service configuration record */
  caii_non_neg_srv_cfg_type  non_neg_cfg;
  #ifdef FEATURE_IS2000_REL_A
  caii_cc_info_type   cc;
  #endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
  boolean use_type0_plcm;
#endif /* FEATURE_IS2000_REL_C */

} caii_srv_con_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  SERVICE OPTION CONTROL MESSAGE (See IS-95-A TSB)                       */

typedef struct
{
  byte  msg_type;                  /* Message type */
  caii_tc_hdr_type  hdr;           /* Layer 2 header */
  boolean  use_time;               /* Use action time indicator */
  byte  action_time;               /* Action time */
  byte  con_ref;                   /* Service option connection ref */
  word  so;                        /* Service option number */
  byte  tsf_len;                   /* Type specific fields length */
  byte  tsf[CAI_FTC_SRV_CTL_MAX];  /* Type specific fields */
} caii_ftc_srv_ctl_type;


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*  SERVICE REDIRECTION MESSAGE (see IS-95A Section 7.7.3.3.2.23)          */
typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  boolean return_if_fail;    /* Return if fail indicator */
  boolean delete_tmsi;       /* Delete TMSI indicator */
  byte redirect_type;        /* Redirect indicator for IS-95B */
  byte record_type;          /* Redirection record type */
  byte record_len;           /* Redirection record length */
  union
  {
    caii_redirect_rec1_type rec1;
    caii_redirect_rec2_type rec2;
    caii_redirect_rec4_type rec4;
  } redir;
} caii_ftc_srv_redir_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* SUPPLEMENTAL CHANNEL ASSIGNMENT MESSAGE(see IS-95A Section 7.7.3.3.2.24)*/
/* This message is not used in special MDR but the following definition is */
/* used, so included it as part of special MDR.                            */
typedef struct
{
  word plt_pn;                 /* Pilot PN */
  boolean expl_code_chan;      /* Explicit Code Channel indicator */
  union
  {
    byte sup_code_chan[CAI_MAX_NUM_SUP];
                                  /* Supplemental conde channel index */
    byte base_code_chan;          /* Base code channel index */
  } chan_lst;
} caii_scam_var_type;

typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  boolean use_retry_delay;     /* Assign or retry indicator */
  byte retry_delay;            /* SCRM retry delay */
  boolean rev_included;        /* Reverse Supplemental Code Channel config */
  byte rev_dtx_duration;       /* Reverse Discontinuous Transmission Duration*/
  boolean expl_rev_start_tm;   /* Explicit reverse supp code chan assign
                                  start time */
  byte rev_start_tm;           /* Explicit start time */
  boolean use_rev_dur;         /* Use reverse duration indicator */
  byte rev_dur;                /* Reverse duration */
  boolean use_rev_hdm_seq;     /* Use reverse GHDM sequence # indicator */
  byte rev_hdm_seq;            /* Sequence # of the reverse linked GHDM */
  byte num_rev_codes;          /* Number of reverse sup code channels */
  boolean use_t_add_abt;       /* Reverse use T_ADD abort indicator */
  boolean use_scrm_seq;        /* Use SCRM sequence # indicator */
  byte scrm_seq_num;           /* SCRM sequence number */
  boolean rev_parms_inc;       /* Reverse additional parameter included */
  byte t_mulchan;              /* SCRM pilot strength reporting offset */
  byte begin_pre;              /* Number of preamble frames */
  byte resume_pre;             /* Number of preamble at resumption of xmit */
  boolean for_inc;             /* FWD supp channel config included indicator */
  byte for_sup_config;         /* FWD supp channel config indicator */
  boolean expl_for_start_tm;   /* Explicit forward start time indicator */
  byte for_start_tm;           /* Forward start time */
  boolean use_for_dur;         /* Use forward duration indicator */
  byte for_dur;                /* Forward duration */
  boolean use_for_hdm_seq;     /* User forward GHDM sequence # indicator */
  byte for_hdm_seq;            /* Sequence # of GHDM */
  byte num_sup_plt;            /* Number of pilots in the active set which
                                  xmitting the sup chan */
  byte num_sup;                /* Number of FWD supp code channel */
} caii_scam_fix_type;

typedef struct
{
  caii_scam_fix_type fix;
  caii_scam_var_type var[CAI_MAX_SUP_PILOTS];
} caii_scam_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*  POWER CONTROL MESSAGE (see IS-95A Section 7.7.3.3.2.25)                */

typedef struct
{
  byte sch_id;            // SCH can be 0 or 1
  byte fpc_sch_fer;
  /*   byte fpc_sch_init_setpt; */
  byte fpc_sch_min_setpt;
  byte fpc_sch_max_setpt;
} caii_fpc_sup_chn_type;


typedef struct
{
  byte msg_type;               /* Message type */
  caii_tc_hdr_type hdr;        /* Layer 2 header */
  byte pwr_cntl_step;          /* Assign or retry indicator */
  boolean use_time;
  byte action_time;
  boolean fpc_incl;
  byte fpc_mode;
  byte fpc_pri_chan;          // 0 - f-fch, 1 - f-dcch
  boolean fpc_olpc_fch_incl;
  byte fpc_fch_fer;
  byte fpc_fch_min_setpt;
  byte fpc_fch_max_setpt;
  boolean fpc_olpc_dcch_incl;
  byte fpc_dcch_fer;
  byte fpc_dcch_min_setpt;
  byte fpc_dcch_max_setpt;
  boolean fpc_sec_chan;
  byte num_sup;
  caii_fpc_sup_chn_type sup_chns[CAI_MAX_NUM_FOR_SCH];
  boolean fpc_thresh_incl;
  byte fpc_setpt_thresh;
  boolean fpc_thresh_sch_incl;
  byte fpc_setpt_thresh_sch;
  boolean rpc_incl;
  byte rpc_num_rec;
  caii_rpc_type rpc_recs[CAI_MAX_NUM_RPC_RECS];
} caii_pwr_ctrl_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* EXTENDED NEIGHBOR LIST UPDATE MESSAGE (see IS-95A Section 7.7.3.3.2.26) */

typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  byte pilot_inc;             /* Pilot Index Increment value */
  byte nghbr_srch_mode;       /* Neighbor Search Mode */
  byte srch_win_n;            /* Default window size for neighbor set */
  boolean use_timing;         /* Use timing indicator */
  boolean global_timing_incl; /* Global Timing Included */
  byte global_tx_duration;    /* Global neighbor transmit time duration */
  byte global_tx_period;      /* Global neighbor transmit time period */
  byte num_nghbr;             /* Number of entries in the nghbr array */
  struct
  {
    word nghbr_pn;            /* Offset of neighbor's PN pilot offset relative
                                 to system time */
    byte search_pri;          /* Search Piority */
    byte srch_win_nghbr;      /* Per neighbor search window */
    boolean timing_incl;      /* Timing Inlcuded Indicator */
    byte nghbr_tx_offset;     /* Neighbor transmit time offset */
    byte nghbr_tx_duration;   /* Global neighbor transmit time duration */
    byte nghbr_tx_period;     /* Global neighbor transmit time period */
  } nghbr[CAI_EXT_NLU_MAX];
  boolean srch_offset_incl;
  struct
  {
    boolean add_pilot_rec_incl;
    byte nghbr_pilot_rec_type;
    caii_add_pilot_rec_type add_pilot_rec;
    byte srch_offset_nghbr;
  } add_nghbr[CAI_EXT_NLU_MAX];
#ifdef FEATURE_IS2000_REL_B
  boolean resq_enabled;             /* Call rescue feature enabled indicator */
  byte resq_delay_time;             /* Call rescue delay timer value */
  byte resq_allowed_time;           /* Call rescue allowed timer value */
  byte resq_attempt_time;           /* Call rescue attempt timer value */
  word resq_code_chan;              /* Code channel index for the Rescue Channel */
  byte resq_qof;                    /* QOF mask id for the Rescue Channel */
  boolean resq_min_period_incl;     /* Min time between consecutive rescues included */
  byte resq_min_period;             /* Min time between consecutive rescues */
  boolean resq_num_tot_trans_incl;  /* Required number of transmissions included */
  byte resq_num_tot_trans_20ms;     /* Required number of transmissions of a regular PDU */
  byte resq_num_tot_trans_5ms;      /* Required number of transmissions of a mini PDU */
  byte resq_num_preamble;           /* Traffic Channel preamble length for Call Rescue Soft Handoff */
  byte resq_power_delta;            /* Power level adjustment */
  struct
  {
    boolean nghbr_resq_configured;  /* Neighbor Rescue Channel configured indicator */
  } resq[CAI_EXT_NLU_MAX];
#endif /* FEATURE_IS2000_REL_B */
} caii_ext_nlu_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  CANDIDATE FREQUENCY SEARCH REQUEST MESSAGE                             */
/*  (see IS-95B Section 7.7.3.3.2.27)                                      */
typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  boolean pre_proc_done;     /* Is pre-processing done? */
  boolean use_time;          /* Indicator of whether the action_time field should
                                be used */
  byte action_time;          /* Action time */
  byte cfsrm_seq;            /* CFSR message sequence number */
  byte srch_type;            /* Search Type Code */
  byte srch_period;          /* Search Period    */
  byte srch_mode;            /* Search Mode Type */
  byte mod_sp_len;           /* Mode specific length */
} caii_cfs_req_fix_type;

typedef struct
{
  byte band_class;           /* Band Class     */
  word cdma_freq;            /* CDMA Frequence Assignment */
  byte sf_total_ec_thresh;   /* Serving Frequency total pilot Ec threshold */
  byte sf_total_ec_io_thresh;/* Serving Frequency total pilot Ec/Io thresh.*/
  byte diff_rx_pwr_thresh;   /* Minimum Difference in received power */
  byte min_total_pilot_ecio; /* Minimum Total Pilot Ec/Io */
  byte cf_t_add;             /* Pilot Detection Threshold for the CDMA
                              * Candidate Frequency                      */
  byte tf_wait_time;         /* CDMA Candidate Frequence Total Wait Time */
  byte cf_pilot_inc;         /* Pilot PN sequence offset index increment
                              * to be used on the CDMA Candidate Frequency
                              * after handoff                             */
  byte cf_srch_win_n;        /* Default Search Window Size for the
                              * Candidate Frequency Search Set            */
  byte cf_srch_win_r;        /* Search Window Size for the Remaining Set  */
  boolean pilot_update;      /* Pilot Search Parameter update Indicator   */
  byte num_pilot;            /* Number of Pilots included in the Message  */
  byte cf_nghbr_srch_mode;   /* Search mode for candidate frequency search
                              * set                                       */
  struct
  {
    word    nghbr_pn;        /* Offset of neighbor pilot pn */
    boolean search_set;      /* Pilot Search Indicator      */
    byte    search_priority; /* Pilot Channel Search priority */
    byte    srch_win_nghbr;  /* Beighbor pilot channel search window size */
  } pilot_update_rec[CAI_CFSREQ_REC_MAX];

  boolean cf_srch_offset_incl;
  struct {
    boolean add_pilot_rec_incl;
    byte nghbr_pilot_rec_type;
    caii_add_pilot_rec_type add_pilot_rec;
    byte srch_offset_nghbr;
  } add_nghbr[CAI_CFSREQ_REC_MAX];
} caii_cfs_req_mod0_type;

typedef struct
{
  byte band_class;           /* Band Class     */
  byte sf_total_ec_thresh;   /* Serving Frequency total pilot Ec threshold */
  byte sf_total_ec_io_thresh;/* Serving Frequency total pilot Ec/Io thresh.*/
  byte num_analog_freqs;     /* Number of analog freq */
  word analog_freq[CAI_CFS_ANALOG_MAX]; /* Analog frequency */
} caii_cfs_req_mod1_type;

typedef struct
{
  caii_cfs_req_fix_type fix;
  union
  {
    caii_cfs_req_mod0_type mod0;
    caii_cfs_req_mod1_type mod1;
  } var;
  byte align_timing;     /* Align timing for CF Search */
  byte search_offset;    /* Srch offset time depends on align_timing */
} caii_cfs_req_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* CANDIDATE FREQUENCY SEARCH CONTROL MESSAGE(see IS-95A Sec 7.7.3.3.2.28) */
typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  boolean pre_proc_done;     /* Is pre-processing done? */
  boolean use_time;          /* Indicator of whether the action_time field should
                                be used */
  byte action_time;          /* Action time */
  byte cfscm_seq;            /* CFSR message sequence number */
  byte srch_type;            /* Search Type Code */
  boolean align_timing;
} caii_cfs_ctrl_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* POWER UP FUNCTION MESSAGE (see IS-95A Section 7.7.3.3.2.29)     */
typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  boolean use_time;           /* Indicator of whether the action_time field should
                                 be used */
  byte action_time;           /* Action time */
  byte act_tm_frm;            /* Action time frame */
  byte puf_setup_sz;          /* Puf setup size */
  byte puf_pulse_sz;          /* Puf pulse size */
  word puf_interval;          /* Puf interval */
  byte puf_init_pwr;          /* Power increase of initial pulse */
  byte puf_pwr_step;          /* PUF power step */
  byte tot_puf_prob;          /* Total number of PUF probes */
  byte max_pwr_puf;           /* Max # of probe at full power */
  boolean puf_freq_incl;      /* Frequency included indicator */
  byte band_class;            /* PUF band class */
  word cdma_freq;             /* PUF CDMA frequency */
} caii_puf_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* POWER UP FUNCTION COMPLETION MESSAGE (see IS-95A Section 7.7.3.3.2.30)  */
typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  boolean loc_ind;           /* Location indicator */
  dword ms_lat;              /* Mobile station latitude */
  dword ms_long;             /* Mobile station longitude */
  dword tm_stamp;            /* Time stamp */
} caii_puf_comp_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* This section defines a generic handoff direction message.               */
/* HANDOFF DIRECTION MESSAGE (see IS-95A Section 7.7.3.3.2.5)              */
/* EXTENDED HANDOFF DIRECTION MESSAGE (see IS-95B Section 7.7.3.3.2.17)    */
/* GENERAL HANDOFF DIRECTION MESSAGE (see IS-95B Section 7.7.3.3.2.31)     */
/* UNIVERSAL HANDOFF DIRECTION MESSAGE (see IS-95B Section 7.7.3.3.2.36)   */

typedef struct
{
  byte srch_win_a;       /* Active and Candidate Set search window size */

  byte srch_win_n;       /* Search window size for the Neighbor Set */
  byte srch_win_r;       /* Search window size for the Remaining Set */

  byte t_add;            /* Pilot detection threshold */
  byte t_drop;           /* Pilot drop threshold */
  byte t_comp;           /* Active vs Candidate set comparison threshold */
  byte t_tdrop;          /* Drop timer value */

  byte soft_slope;       /* Soft Slope for soft HO inequality */
  /* The following need to stored internally as signed values */
  int8 add_intercept;    /* Add Intercept for soft HO inequality */
  int8 drop_intercept;   /* Drop Intercept for soft HO inequality */

} caii_ho_srch_type;

typedef struct
{
  byte p_rev;            /* Protocol Revision for new base station */
  byte packet_zone_id;   /* Packet Data Zone ID for new base station */

  byte frame_offset;     /* Frame offset */
  boolean private_lcm;   /* Private long code mask indicator */
  boolean reset_l2;      /* Reset acknowledgement procedures command */
  boolean reset_fpc;     /* Reset forward power control procedures command */
  byte serv_neg_type;    /* Service Negotiation type */
  byte encrypt_mode;     /* Message encryption mode */
  boolean nom_pwr_ext;   /* Extended nominal transmit power indicator */
  byte nom_pwr;          /* Nominal transmit power */
  byte num_preamble;     /* Number of preamble frames to transmit */
  byte band_class;       /* Which band the CDMA channel frequency is in */
  byte freq_incl;        /* Only in HDM.  TRUE if CDMA_FREQ is included */
  word cdma_freq;        /* Frequency assignment of CDMA channel */

  boolean return_if_ho_fail; /* Return to old config if handoff fails */
  boolean complete_srch; /* Complete candidate frequency search flag */
  boolean periodic_srch; /* Periodic search of candidate freq flag */
  boolean scr_included;  /* Service configuration included flag */
  byte serv_con_seq;     /* Connect sequence number */
  caii_srv_cfg_type cfg; /* Service configuration record */
  byte rlgain_traffic_pilot;
  boolean default_rlag;
  boolean nnscr_included;
  caii_non_neg_srv_cfg_type non_neg_cfg;
} caii_ho_extra_type;

typedef struct
{
  byte add_length;
  byte ext_ho_p_rev;
} caii_ho_add_type;


typedef struct
{
  boolean for_included;      /* FWD supp channel config included indicator */
  byte for_sup_config;       /* FWD supp channel config indicator */
  byte num_for_sup;          /* Number of FWD supp code channel */
  boolean use_for_dur;       /* Use forward duration indicator */
  byte for_dur;              /* Forward duration */
  boolean rev_included;      /* Reverse Supplemental Code Channel config */
  byte rev_dtx_duration;     /* Reverse Discontinuous Transmission Duration*/
  boolean clear_retry_delay; /* Clear SCRM retry delay indicator */
  boolean use_rev_dur;       /* Use reverse duration indicator */
  byte rev_dur;              /* Reverse duration */
  byte num_rev_codes;        /* Number of reverse sup code channels */
  boolean use_t_add_abt;     /* Reverse use T_ADD abort indicator */
  boolean rev_parms_inc;     /* Reverse additional parameter included */
  byte t_mulchan;            /* SCRM pilot strength reporting offset */
  byte begin_pre;            /* Number of preamble frames */
  byte resume_pre;           /* Number of preamble at resumption of xmit */
} caii_ho_sup_chan_parms_type;


typedef struct
{
  byte for_sch_id;
  byte sccl_index;
  boolean pilot_incl;
  word code_chan_sch;
  byte qof_mask_id_sch;
} caii_sch_chan_rec_type;

typedef struct
{
  byte num_for_sch;
  struct {
   byte for_sch_id;   // Only 0 or 1 here
   byte sccl_index;
   byte for_sch_rate;
  } for_sch_cfg[CAI_MAX_NUM_FOR_SCH_CFG];

  byte num_rev_sch;
  struct {
   byte rev_sch_id;   // Only 0 or 1 here
   byte rev_walsh_id; // See IS2000.5 table 3.7.3.3.2.37-2
   byte rev_sch_rate;
  } rev_sch_cfg[CAI_MAX_NUM_REV_SCH_CFG];
} caii_sch_cfg_type;

typedef struct
{
  word    pilot_pn;          /* Pilot PN sequence offset index */
  boolean pwr_comb_ind;      /* Power control symbol combining indicator */
  boolean add_pilot_rec_incl;
  byte pilot_rec_type;
  caii_add_pilot_rec_type add_pilot_rec;
  word code_chan_fch;
  byte qof_mask_id_fch;
  word code_chan_dcch;
  byte qof_mask_id_dcch;
  byte num_sch;
  byte srch_offset;

  boolean for_sup_inc;       /* Forward sup code chan included indicator */
  boolean expl_code_chan;    /* Explicit Code Channel indicator */
  union
  {
    byte sup_code_chan[CAI_MAX_NUM_SUP];
                             /* Forward supplemental code channels */
    byte base_code_chan;     /* Base code channel index */
    caii_sch_chan_rec_type sch_chan_rec[CAI_MAX_NUM_SCH_CHAN];
  } chan_rec;

#ifdef FEATURE_IS2000_REL_C
  boolean for_pdch_incl;
  byte walsh_table_id;
  byte num_pdcch;
  byte for_pdcch_walsh[CAI_MAX_NUM_FOR_PDCCH];
  byte mac_id;
  byte rev_cqich_cover;
  byte for_cpcch_walsh;
  byte for_cpcsch;
  boolean pdch_group_ind;
#endif /* FEATURE_IS2000_REL_C */
} caii_ho_pilot_type;

#ifdef FEATURE_IS2000_REL_A
typedef struct
{
   boolean thx_fch_low_incl;
   byte qof_mask_id_fch_low;
   word code_chan_fch_low;
   boolean thx_fch_high_incl;
   byte qof_mask_id_fch_high;
   word code_chan_fch_high;

   boolean thx_dcch_low_incl;
   byte qof_mask_id_dcch_low;
   word code_chan_dcch_low;
   boolean thx_dcch_high_incl;
   byte qof_mask_id_dcch_high;
   word code_chan_dcch_high;
   boolean thx_sch_info_incl;

   struct
   {
      byte for_sch_id;
      boolean thx_sch_low_incl;
      byte qof_mask_id_sch_low;
      word code_chan_sch_low;
      boolean thx_sch_high_incl;
      byte qof_mask_id_sch_high;
      word code_chan_sch_high;
   } sch_chan_rec_threex[CAI_MAX_NUM_SCH_CHAN];


} caii_ho_pilot_threex_type;

typedef struct
{
  boolean             thx_fch_info_incl;
  boolean             thx_dcch_info_incl;
  caii_ho_pilot_threex_type pilot_threex[CAI_HO_MSG_MAX_PILOTS];
#ifdef FEATURE_IS2000_REL_B
  boolean ccsh_included;
  boolean use_ccsh_encoder_time;
  byte ccsh_encoder_action_time;
  struct
  {
    struct
    {
      boolean ccsh_encoder_type;
    } sch[CAI_MAX_NUM_SCH_CHAN];
  } pilot[CAI_HO_MSG_MAX_PILOTS];
#endif /* FEATURE_IS2000_REL_B */
} caii_ho_threex_and_ccsh_type;

#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
typedef struct
{
  boolean for_pdch_rlgain_incl;
  byte rlgain_ackch_pilot;
  byte rlgain_cqich_pilot;
  byte num_soft_switching_frames;
  byte num_softer_switching_frames;
  byte num_soft_switching_slots;
  byte num_softer_switching_slots;
  boolean chm_switching_params_incl;
  byte num_soft_switching_frames_chm;
  byte num_softer_switching_frames_chm;
  byte num_soft_switching_slots_chm;
  byte num_softer_switching_slots_chm;
  byte pdch_soft_switching_delay;
  byte pdch_softer_switching_delay;
} caii_ho_for_pdch_params_type;
#endif /* FEATURE_IS2000_REL_C */

typedef struct
{
  byte msg_type;                /* Message type */
  caii_tc_hdr_type hdr;         /* Layer 2 header */
  boolean pre_proc_done;        /* Is pre-processing done? */
  boolean use_time;             /* Use action_time indicator */
  byte action_time;             /* Action time */
  byte hdm_seq;                 /* Handoff direction message sequence # */
  boolean parms_incl;
  boolean srch_inc;             /* Pilot search parameter included */
  caii_ho_srch_type srch;       /* Search parameters */
  boolean extra_parms;          /* Hard Handoff plus other parms included */
  caii_ho_extra_type extra;     /* Extra parameters (mostly hard HO) */
  caii_ho_add_type add;         /* Additional fields for EHDM */

  boolean sup_chan_parms_inc;   /* MDR supplemental channel parms included */
  caii_ho_sup_chan_parms_type sup; /* MDR supplemental channel parameters */
  boolean use_pwr_cntl_step;    /* Use Power Control Step indicator */
  byte pwr_cntl_step;           /* Power Control Step */

  byte num_pilots;              /* Num pilot records included */
  caii_ho_pilot_type pilot[CAI_HO_MSG_MAX_PILOTS]; /* Pilot records */
#ifdef FEATURE_IS2000_REL_A
  caii_ho_threex_and_ccsh_type thx_and_ccsh_info;
#endif /* FEATURE_IS2000_REL_A */
  /* reserved field is not included, since we don't care what it is */
  byte fpc_subchan_gain;
  boolean use_pc_time;
  byte pc_action_time;
  boolean rev_fch_gating_mode;
  boolean rev_pwr_cntl_delay_incl;
  byte rev_pwr_cntl_delay;
  boolean clear_retry_delay;
  boolean srch_offset_incl;
  boolean sch_incl;
  byte num_for_assign;
  struct {
    byte for_sch_id;
    byte for_sch_duration;
    boolean for_sch_start_time_incl;
    byte for_sch_start_time;
    byte sccl_index;
  } for_assign[CAI_MAX_NUM_FOR_SCH];
  byte num_rev_assign;
  struct {
    byte rev_sch_id;
    byte rev_sch_duration;
    boolean rev_sch_start_time_incl;
    byte rev_sch_start_time;
    byte rev_sch_rate;
  } rev_assign[CAI_MAX_NUM_REV_SCH];
  byte ch_ind;
#ifdef FEATURE_IS2000_REL_C
  byte    ext_ch_ind;
#endif /* FEATURE_IS2000_REL_C */
  /* active_set_rec_len is used only for parsing */
  caii_sch_cfg_type sch_cfg;

  byte                d_sig_encrypt_mode;
  byte                key_size;

  boolean             sync_id_incl;
  byte                sync_id_len;       /* valid only if sync_id_incl == TRUE */
  byte                sync_id[CAI_SYNC_ID_LEN_MAX];

  caii_cc_info_type   cc;

  boolean             cs_supported;
  boolean             threexfl_1xrl_incl;
  byte                onexrl_freq_offset;


  boolean chm_supported;
  boolean cdma_off_time_rep_sup_ind;
  boolean cdma_off_time_rep_threshold_unit;
  byte    cdma_off_time_rep_threshold;
  boolean release_to_idle_ind;
  boolean msg_integrity_sup;
  boolean gen_2g_key;
  boolean register_in_idle;
  boolean plcm_type_incl;
  byte    plcm_type;
  qword   plcm_39;

#ifdef FEATURE_IS2000_REL_C
  boolean t_tdrop_range_incl;
  byte    t_tdrop_range;
  boolean for_pdch_supported;
  boolean pdch_chm_supported;
  boolean pilot_info_req_supported;
  boolean enc_supported;
  caii_sig_encrypt_sup_type sig_encrypt_sup;
  caii_ui_encrypt_sup_type ui_encrypt_sup;
  boolean use_sync_id;
  boolean sid_incl;
  word    sid;
  boolean nid_incl;
  word    nid;
  boolean sdb_supported;
  boolean mob_qos;
  boolean ms_init_pos_loc_sup_ind;
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_C
  boolean pdch_control_hold;
  boolean full_ci_feedback_ind;
  byte    rev_cqich_frame_offset;
  byte    rev_cqich_reps;
  byte    rev_ackch_reps;
  boolean pdch_group_ind_incl;
  boolean for_pdch_parms_incl;
  caii_ho_for_pdch_params_type for_pdch_params;
  boolean for_pdch_common_parms;
  byte    walsh_table_id;
  byte    num_pdcch;
  byte    for_pdcch_walsh[CAI_MAX_NUM_FOR_PDCCH];
#endif /* FEATURE_IS2000_REL_C */
} caii_ho_msg_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*  Extended Supplemental Channel Assignment                               */
/*  (see IS-2000 Section 3.7.3.3.2.37)                                     */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


typedef struct
{
  byte msg_type;                /* Message type */
  caii_tc_hdr_type hdr;         /* Layer 2 header */
  byte    start_time_unit;
  byte    rev_sch_dtx_duration;
  boolean use_t_add_abort;
  boolean use_scrm_seq_num;
  byte    scrm_seq_num;
  boolean add_info_incl;
  byte    fpc_pri_chan;         // 0 - Power control on on f-fch, 1 - power control on f-dcch
  boolean rev_cfg_included;
  byte    num_rev_cfg_recs;
  struct
  {
    byte    rev_sch_id;      // Only 0 and 1 valid
    boolean rev_walsh_id;
    byte    rev_sch_rate;
  } rev_cfg[CAI_ESCAM_MAX_REV_CFG];
  byte num_rev_sch;
  struct
  {
    byte    rev_sch_id;      // Only 0 and 1 valid
    byte    rev_sch_duration;
    boolean rev_sch_start_time_incl;
    byte    rev_sch_start_time;
    byte    rev_sch_rate;
  } rev_sch[CAI_ESCAM_MAX_REV_SCH];
  boolean for_cfg_included;
  boolean for_sch_fer_rep;
  byte num_for_cfg_recs;
  struct
  {
    byte for_sch_id; // Only 0 and 1 valid
    byte sccl_index;
    byte for_sch_rate;
    byte num_sup_sho;
    struct
    {
      word pilot_pn;
      byte add_pilot_rec_incl;
      byte active_pilot_rec_type;
      caii_add_pilot_rec_type add_pilot_rec;
      word for_sch_cc_index;
      byte qof_mask_id_sch;
    } sup_sho[CAI_ESCAM_MAX_SUP_SHO];
  } for_cfg[CAI_ESCAM_MAX_FOR_CFG];
  byte num_for_sch;
  struct
  {
    byte    for_sch_id;      // Only 0 and 1 valid
    byte    for_sch_duration;
    boolean for_sch_start_time_incl;
    byte    for_sch_start_time;
    byte    sccl_index;
  } for_sch[CAI_ESCAM_MAX_FOR_SCH];
  boolean fpc_incl;
  byte    fpc_mode_sch;
  boolean fpc_sch_init_setpt_op;
  byte    fpc_sec_chan;   // 0 = SCH 0, 1 = SCH 1
  byte num_sup;
  struct
  {
    byte sch_id;
    byte fpc_sch_fer;
    byte fpc_sch_init_setpt;
    byte fpc_sch_min_setpt;
    byte fpc_sch_max_setpt;
  } sup[CAI_ESCAM_MAX_SUP];
  boolean fpc_thresh_sch_incl;
  byte    fpc_setpt_thresh_sch;
  boolean rpc_incl;
  boolean rpc_num_sup;
  struct
  {
    boolean sch_id;
    byte rlgain_sch_pilot;
  } rpc_sup[CAI_ESCAM_MAX_SUP];
#ifdef FEATURE_IS2000_REL_A
  boolean  threex_sch_info_incl;
  byte     num_threex_cfg;
  struct
  {
    boolean  for_sch_id;
    byte  num_threex_rec;
    struct
    {
      byte  sccl_index;
      struct
      {
  boolean threex_sch_low_incl;
  byte    qof_mask_id_sch_low;
  word    code_chan_sch_low;
  boolean threex_sch_high_incl;
  byte    qof_mask_id_sch_high;
  word    code_chan_sch_high;
      } sup[CAI_ESCAM_MAX_SUP_SHO];
    } rec[CAI_ESCAM_MAX_THREEX_REC];
  } threex_cfg[CAI_ESCAM_MAX_THREEX_CFG];
#endif  // FEATURE_IS2000_REL_A
#ifdef FEATURE_IS2000_REL_B
  boolean ccsh_included;
  struct
  {
    struct
    {
      boolean ccsh_encoder_type;
    } sup[CAI_ESCAM_MAX_SUP_SHO];
  } ccsh_cfg[CAI_ESCAM_MAX_FOR_CFG];

  boolean for_sch_cc_incl;
  caii_for_sch_type for_sch_cc[CAI_ESCAM_MAX_FOR_SCH];

  boolean rev_sch_cc_incl;
  caii_rev_sch_type rev_sch_cc[CAI_ESCAM_MAX_REV_SCH];
#endif /* FEATURE_IS2000_REL_B */
} caii_escam_type;

/* Resource Allocation Message 3.7.3.3.2.32 */
typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  boolean use_time;
  byte action_time;          /* if use_time is included */
  boolean fpc_pri_chan;
} caii_ram_type;

/* Extended Release Message 3.7.3.3.2.34 */
typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  boolean   use_time;
  byte      action_time;     /* if use_time is included */
  byte      ch_ind;
  boolean   gating_rate_incl;
  byte      pilot_gating_rate;
#ifdef FEATURE_IS2000_REL_C
  boolean   use_ext_ch_ind;
  byte      ext_ch_ind;
  boolean   pdch_control_hold;
  boolean   switching_parms_incl;
  byte      num_soft_switching_frames_chm;
  byte      num_softer_switching_frames_chm;
  boolean   direct_to_idle_info_incl;
  byte      release_type;
  word      sid;
  word      nid;
  boolean   freq_incl;
  word      cdma_freq;
  byte      band_class;
  byte      page_ch;
  byte      prat;
  byte      sr1_bcch_code_chan_non_td;
  boolean   sr1_crat_non_td;
  byte      sr1_brat_non_td;
  byte      sr1_td_mode;
  byte      sr1_bcch_code_chan_td;
  boolean   sr1_crat_td;
  byte      sr1_brat_td;
  byte      sr1_td_power_level;
  boolean   num_pilots_d2i_incl;
  byte      num_pilots_d2i;
  word      pilot_pn[CAI_MAX_NUM_D2I_PILOTS];
#endif /* FEATURE_IS2000_REL_C */
} caii_erm_type;

#ifdef FEATURE_IS2000_REL_A
/********************************************/
/*        Forward Traffic Channel           */
/* Release A Message Definitions Start here */
/********************************************/


/* Mobile Assisted Burst Operation Parameters Message 3.7.3.3.2.40 */
typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  boolean   order_flag;
  byte      psmin_delta;
  byte      order_interval;
  boolean   periodic_flag;
  byte      num_pilots;
  byte      periodic_interval;
  boolean   threshold_flag;
  byte      ps_floor_high;
  byte      ps_floor_low;
  byte      ps_ceiling_high;
  byte      ps_ceiling_low;
  byte      threshold_interval;
} caii_mabop_type;

/* User Zone Reject Message 3.7.3.3.2.41 */
typedef struct
{
  byte    msg_type;           /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  word    reject_uzid;
  byte    reject_action_indi;
  boolean user_assign_included;
  word    assign_id;
} caii_user_zone_rej_type;

/* User Zone Update Message 3.7.3.3.2.42 */
typedef struct
{
  byte    msg_type;           /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  word    uz_id;
} caii_user_zone_upd_type;

/* Call Assignment Message  3.7.3.3.2.43 */


typedef struct
{
  byte      msg_type;         /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  boolean   response_ind;
  byte      tag;
  boolean   accept_ind;
  boolean   reject_pktdata_ind;
  boolean   bypass_alert_answer;
  boolean   so_incl;
  word      so;
  boolean   con_ref_incl;
  byte      con_ref;
#ifdef FEATURE_IS2000_REL_C
  boolean   use_old_serv_config;
  byte      sr_id;
#endif /* FEATURE_IS2000_REL_C */
} caii_call_assignment_type;

 /* Extended Alert with Info 3.7.3.3.2.44 */
 typedef struct
 {
   byte      msg_type;        /* Message type */
   caii_tc_hdr_type hdr;      /* Layer 2 header */
   boolean   con_ref_incl;
   byte      con_ref;
   byte      num_recs;
   dword     recs[CAII_MAX_INFO_REC_DWORDS];
     /* Records - consecutive Forward Traffic Channel Information Records -
        use caii_rec_hdr type to parse different records */
 } caii_extended_alert_w_info_type;

/* Extended Flash with Info 3.7.3.3.2.45 */
/* Use caii_extended_alert_w_info_type from Extended Alert With Information
    record type definition above */

 /* Security Mode Command 3.7.3.3.2.46 */

 typedef struct
 {
   byte con_ref;
   byte ui_encrypt_mode;
 } caii_security_record_type;

#define MAX_SEC_REC 7

 typedef struct
 {
    byte      msg_type;       /* Message type */
    caii_tc_hdr_type hdr;     /* Layer 2 header */
    boolean   use_time;
    byte      action_time;
    byte      d_sig_encrypt_mode;
    byte      num_recs;
    caii_security_record_type recs[MAX_SEC_REC];
    byte      key_size;
    boolean   c_sig_encrypt_mode_incl;
    byte      c_sig_encrypt_mode;
#ifdef FEATURE_IS2000_REL_C
   boolean    msg_int_info_incl;
   boolean    change_keys;
   boolean    use_uak;
#endif /* FEATURE_IS2000_REL_C */
 } caii_security_mode_cmd_type;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A
/* Base Station Status Response Message, 3.7.3.3.2.47 */
typedef struct
{
  byte      num_pilots;
  boolean   sid_nid_ind;

  struct
  {
    word    pilot_pn;
    word    base_id;
    boolean sid_nid_incl;
    word    sid;
    word    nid;
#ifdef FEATURE_IS2000_REL_C
    boolean base_lat_long_incl;
    uint32  base_lat;
    uint32  base_long;
#endif /* FEATURE_IS2000_REL_C */
  } pi_rec[CAI_BSS_NUM_PILOTS];

} caii_bss_rsp_pi_type;

typedef struct
{
  byte msg_type;             /* Message type */
  caii_tc_hdr_type hdr;      /* Layer 2 header */
  byte qual;                 /* Qualification information */
  byte band;                 /* Band class qualifier */
  byte op_mode;              /* Operating mode qualifier */

  byte num_record;
  struct
  {
    byte record_type;
    caii_bss_rsp_pi_type pi;
  } bss_rec[CAI_BSS_NUM_RECORDS];
} caii_bs_status_rsp_type;

#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
/* Authentication Request Message 3.7.3.3.2.48 */
typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  qword randa_hi;       /* Most significant qword of random challenge number */
  qword randa_lo;       /* Least significant qword of random challenge number */
  qword con_sqn;        /* Concealed sequence number */
  word  amf;            /* Authentication management field */
  qword mac_a;          /* Message authentication code */
} caii_tc_auth_req_type;

/* Rate Change Message 3.7.3.3.2.49 */
typedef struct
{
  byte msg_type;                           /* Message type */
  caii_tc_hdr_type hdr;                    /* Layer 2 header */
  boolean use_time;                        /* Use action time indicator */
  byte action_time;                        /* Action time */
  boolean rev_cqich_rate_change_incl;      /* Reverse CQICH rate change included */
  boolean full_ci_feedback_ind;            /* Full C/I feedback rate indicator */
  byte rev_cqich_reps;                     /* Reverse CQICH repetition factor */
  boolean switching_parms_incl;            /* R-CQICH switching parameters included */
  byte num_soft_switching_frames;          /* Number of frames for R-CQICH soft switching */
  byte num_softer_switching_frames;        /* Number of frames for R-CQICH softer switching */
  boolean chm_switching_parms_incl;        /* Control Hold Mode fields included */
  byte num_soft_switching_frames_chm;      /* Number of frames for R-CQICH soft switching while in Control Hold */
  byte num_softer_switching_frames_chm;    /* Number of frames for R-CQICH softer switching while in Control Hold */
  boolean rl_cqich_att_adj_gain_incl;      /* Attribute adjustment gain for R-CQICH included */
  boolean num_recs;                        /* Number of the records */
  caii_rpc_adj3_type rl_cqich[CAI_MAX_RL_CQICH_RECS];
  boolean rev_ackch_rate_change_incl;      /* Reverse Ack Channel rate change included */
  byte rev_ackch_reps;                     /* Reverse Ack Channel repetition factor */
} caii_rate_change_type;
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_1X_ADV
/* Radio Configuration Parameters Message 3.7.3.3.2.51 */
typedef struct
{
  byte msg_type;                   /* Message type */
  caii_tc_hdr_type hdr;            /* Layer 2 header */
  caii_radio_config_params_type rcp; /* Radio Configuration Parameters */
} caii_radio_config_type;

/* Traffic Channel General Extension Message */
typedef struct
{
  byte msg_type;                /* Message type */
  caii_tc_hdr_type hdr;         /* Layer 2 header */
  byte num_ge_rec;              /* Number of General Extension Records
                                   included in this message */
  caii_gen_extn_rec_type gen_extn_rec[CAI_MAX_NUM_GE_REC];
                                /* General Extension Records included in the message */
  byte message_type;            /* Message type of the message included in the General
                                   Extension Message */
  byte *message_rec;            /* Pointer to message extended through the
                                   General Extension message */
} caii_tc_gen_extn_type;
#endif /* FEATURE_IS2000_1X_ADV */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                REVERSE TRAFFIC CHANNEL MESSAGES                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* ORDER MESSAGE (see IS-95A Section 6.7.2.3.2.1                           */

/* Generic Order type */
typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  byte order;           /* Order field */
#ifdef FEATURE_IS2000_REL_A
/* This part is filled only for Call Control Orders
   For Reverse Traffic it only applies to Connect Order */
  boolean con_ref_incl;
  byte    con_ref;
#endif /* FEATURE_IS2000_REL_A */
} caii_rtc_gen_ord_type;

/* Order with order qualifier */
typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  byte order;           /* Order field */
  byte ordq;            /* Order qualifier field */
#ifdef FEATURE_IS2000_REL_A
/* This part is filled only for Call Control Orders
   For Reverse Traffic it only applies to DTMF Order */
  boolean con_ref_incl;
  byte    con_ref;
#endif /* FEATURE_IS2000_REL_A */
} caii_rtc_ordq_type;

/* Base Station Challenge Order (see IS-95A Section 6.7.3.1) */
typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  byte order;           /* Order field */
  byte ordq;            /* Order qualifier field */
  dword randbs;         /* Random challenge data */
} caii_rtc_bs_chal_type;

/* Service option request or Service Option response type (see IS-95A
   Sections 6.7.3.2 and 6.7.3.3) */
typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  byte order;           /* Order field */
  byte ordq;            /* Order qualifier field */
  word service_option;  /* Service option */
} caii_rtc_so_type;

/* Mobile Station Reject Order (see IS-95A Section 6.7.3.4) */

/* Fixed part of Mobile Station Reject Order */
typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  byte order;           /* Order field */
  byte ordq;            /* Order qualifier field */
  byte rej_msg_type;    /* Message type for the message being rejected */
} caii_rtc_rej_fixed_type;

/* Only present if the rejected message was an Order Message */
typedef struct
{
  byte rej_order;  /* Order type of rejected message */
  byte rej_ordq;   /* Order qualification code of rejected message */
} caii_rtc_rej_order_type;

/* Only present if the rejected message was a Retrieve Parameters Message
   or a Set Parameters Message */
typedef struct
{
  word rej_param_id;  /* Parameter id of the rejected parameter */
} caii_rtc_rej_param_type;

/* Only present if the rejected message was an Alert with Information
   Message or a Flash with Information Message */
typedef struct
{
  byte rej_record; /* Record type of the rejected information record */
} caii_rtc_rej_record_type;

typedef struct
{
  caii_rtc_rej_fixed_type fix;
  union {
    caii_rtc_rej_order_type  order;
    caii_rtc_rej_param_type  param;
    caii_rtc_rej_record_type rec;
  } var;
#ifdef FEATURE_IS2000_REL_A
/* this one is used in Reject Order for following ORDER values:
  00010010, 00010001, 00010011
  L3 should ignore CON_REF and TAG for other Reject ORDER values */
  byte con_ref;
  byte tag;
#endif /* FEATURE_IS2000_REL_A */
  byte rejected_pdu_type;
} caii_rtc_rej_type;

/* Release Order (see Section 2.7.3.5) */
typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  byte order;           /* Order field */
  byte ordq;            /* Order qualifier field */
} caii_rtc_release_type;

/* Order Message */
typedef union
{
  caii_rtc_gen_ord_type gen;
  caii_rtc_ordq_type    ordq;
  caii_rtc_bs_chal_type chal;
  caii_rtc_so_type      so;
  caii_rtc_rej_type     rej;
  caii_rtc_release_type rel;
} caii_rtc_ord_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* AUTHENTICATION CHALLENGE RESPONSE MSG (see IS-95A Section 6.7.2.3.2.2) */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  dword authu;           /* Authentication challenge response */
} caii_auth_rsp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* FLASH WITH INFORMATION MESSAGE (see IS-95A Section 6.7.2.3.2.3)        */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  byte num_recs;         /* Number of information records in message */
  dword recs[CAII_MAX_INFO_REC_DWORDS];
    /* Records - consecutive Reverse Traffic Channel Information Records -
       use caii_rec_hdr type to parse different records */
} caii_flash_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* TMSI ASSIGNMENT C0MPLETION MESSAGE (see J-STD-008 Section 2.7.2.3.2.17) */

typedef struct /* TMSI Assignment Completion Message - Reverse Traffic Chn */
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
} caii_rtc_tmsi_cmp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* DATA BURST MESSAGE (see IS-95A Section 6.7.2.3.2.4)                    */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  byte msg_number;       /* Number of message in the data stream burst */
  byte burst_type;       /* Type of data burst */
  byte num_msgs;         /* Number of messages in the data stream burst */
  byte num_fields;       /* Number of characters in chari field */
  byte chari[CAI_REV_TC_BURST_MAX];
} caii_rev_tc_burst_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PILOT STRENGTH MEASUREMENT MESSAGE (see IS-95A Section 6.7.2.3.2.5)    */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  word ref_pn;           /* Time reference PN sequence offset */
  byte pilot_strength;   /* Pilot strength */
  boolean keep;          /* Keep pilot indicator */
  word num_msr;          /* Number of entries in the meas array */
  struct
  {
    word pilot_pn_phase;  /* Pilot measured phase */
    byte pilot_strength;  /* Pilot strength */
    boolean keep;         /* Keep pilot indicator */
  } meas[CAI_PIL_STR_MAX];
} caii_pil_str_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* POWER MEASUREMENT REPORT MESSAGE (see IS-95A Section 6.7.2.3.2.6)      */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  byte errors_detected;  /* Number of frame errors detected */
  word pwr_meas_frames;  /* Number of Forward Traffic Channel frames
                            in the measurement period */
  byte last_hdm_seq;     /* Handoff Direction Message sequence number */
  byte num_pilots;       /* Number of pilots */
  byte pilot_strength[CAI_PWR_MAX];  /* Active set pilots */
  boolean dcch_pwr_meas_incl;
  word dcch_pwr_meas_frames;
  byte dcch_errors_detected;
  boolean sch_pwr_meas_incl;
  boolean sch_id;
  word sch_pwr_meas_frames;
  word sch_errors_detected;
} caii_pwr_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SEND BURST DTMF MESSAGE (see IS-95A Section 6.7.2.3.2.7)               */

/* Use caii_sbdtmf_type from Forward Traffic Channel Message record type
   definitions above */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* STATUS MESSAGE (see IS-95A Section 6.7.2.3.2.8)                        */

typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  dword recs[CAII_MAX_INFO_REC_DWORDS];
    /* Records - consecutive Reverse Traffic Channel Information Records -
       use caii_rec_hdr type to parse different records */
} caii_status_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* ORIGINATION CONTINUATION MESSAGE (see IS-95A Section 6.7.2.3.2.9)      */

typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  boolean digit_mode;   /* Digit mode indicator */
  byte num_fields;      /* Number of dialed digits in the message */
  byte chari[CAI_ORIG_C_MAX];  /* Dialed digits */
  /* Following fields are for IS-95B only. Higher level should set num_fields
     to 0 for lower P_REV */
  byte num_recs;              /* Number of records in message */
  dword recs[CAII_MAX_INFO_REC_DWORDS];
    /* Records - consecutive Forward Traffic Channel Information Records -
       use caii_rec_hdr type to parse different records */
} caii_orig_c_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* HANDOFF COMPLETION MESSAGE (see IS-95A Section 6.7.2.3.2.10)           */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  byte last_hdm_seq;     /* Handoff Direction Message sequence number */
  byte num_pilots;       /* Number of pilots in pilot_pn array */
  word pilot_pn[CAI_MAX_COMP];  /* Pilot PN sequence offsets */
} caii_comp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PARAMETERS RESPONSE MESSAGE (see IS-95A Section 6.7.2.3.2.11)          */

typedef struct
{
  byte msg_type;        /* Message type */
  caii_tc_hdr_type hdr; /* Layer 2 header */
  word num_parms;       /* Number of parameters in parameter array */
  struct
  {
    word parameter_id;  /* Parameter identification */
    word parameter_len; /* Length of parameter */
    union
    {
      byte parm_b;      /* byte length parameter */
      word parm_w;      /* word length parameter */
      dword parm_d;     /* dword length parameter */
      qword parm_q;     /* qword length parameter */
    } parameter;
  } parms[CAI_PARM_RSP_MAX ];  /* Array of parameters */
} caii_parm_rsp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  SERVICE REQUEST MESSAGE (See IS-95-A TSB)                              */

typedef struct
{
  byte  msg_type;          /* Message type */
  caii_tc_hdr_type  hdr;   /* Layer 2 header */
  byte  req_seq;           /* Request sequence number */
  byte  purp;              /* Purpose code */
  caii_srv_cfg_type  cfg;  /* Service configuration record */
/* This part is removed in IS-2000 Release A */
} caii_rtc_srv_req_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  SERVICE RESPONSE MESSAGE (See IS-95-A TSB)                             */

typedef struct
{
  byte  msg_type;          /* Message type */
  caii_tc_hdr_type  hdr;   /* Layer 2 header */
  byte  req_seq;           /* Request sequence number */
  byte  purp;              /* Purpose code */
  caii_srv_cfg_type  cfg;  /* Service configuration record */
} caii_rtc_srv_rsp_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  SERVICE CONNECT COMPLETION MESSAGE (See IS-95-A TSB)                   */

typedef struct
{
  byte  msg_type;         /* Message type */
  caii_tc_hdr_type  hdr;  /* Layer 2 header */
  byte  con_seq;          /* Connect sequence number */
} caii_srv_cmp_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  SERVICE OPTION CONTROL MESSAGE (See IS-95-A TSB)                       */

typedef struct
{
  byte  msg_type;                    /* Message type */
  caii_tc_hdr_type  hdr;             /* Layer 2 header */
  byte  con_ref;                     /* Service option connection ref */
  word  so;                          /* Service option number */
  byte  tsf_len;                     /* Type specific fields length */
  byte  tsf[ CAI_RTC_SRV_CTL_MAX ];  /* Type specific fields */
} caii_rtc_srv_ctl_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* STATUS RESPONSE MESSAGE (see IS-95-A TSB)                               */

typedef struct
{
  byte  msg_type;         /* Message type */
  caii_tc_hdr_type  hdr;  /* Layer 2 header */
  byte qual;              /* Qualification information type */
  byte band;              /* Band class qualifier */
  byte op_mode;           /* Operating mode qualifier */
  byte num_recs;          /* Number of information records */
  dword recs[CAII_MAX_INFO_REC_DWORDS];
    /* Records - consecutive Information Records -
       use caii_rec_hdr type to parse different records */
} caii_tc_strsp_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* SUPPLEMENTAL CHANNEL REQUEST MESSAGE (see IS-95B Section 6.7.2.3.2.18)  */
typedef struct
{
  word pn_phase;    /* PN phase */
  byte pn_strength; /* Strength of the PN */
} caii_scrm_pilot_info_type;

typedef struct
{
  caii_add_pilot_rec1_type pilot;
} caii_scrm_aux_pilot_rec_type;

typedef struct
{
  boolean pilot_rec_incl;
  byte pilot_rec_type;
  caii_scrm_aux_pilot_rec_type aux_pil_rec;
} caii_scrm_pilot_rec_type;

typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  byte sz_req_blob;           /* Size of the request info block of bytes */
  byte req_blob[CAI_MAX_SCRM_BLOB_SIZE];
                              /* request info block */
  boolean use_scrm_seq_num;   /* Use SCRM sequence number indicator */
  byte scrm_seq_num;          /* SCRM sequence number */
  word ref_pn;                /* Time reference PN offset */
  byte pilot_str;             /* Pilot strength */
  byte num_act_pn;            /* Number of pn in the active set */
  caii_scrm_pilot_info_type act_pn[CAI_MAX_SCRM_ACT_PN];
  byte num_nghbr_pn;          /* Number of neigbor set pn */
  caii_scrm_pilot_info_type nghbr_pn[CAI_MAX_SCRM_NGHBR_PN];

  #ifdef FEATURE_IS2000_REL_A
  /* reference pilot record information */
  caii_scrm_pilot_rec_type ref_pil_rec;
  /* num_act_pn occurences must be filled */
  caii_scrm_pilot_rec_type act_pil_rec[CAI_MAX_SCRM_ACT_PN];
  /* num_nghbr_pn occurences must be filled */
  caii_scrm_pilot_rec_type nghbr_pil_rec[CAI_MAX_SCRM_NGHBR_PN];
  #endif /* FEATURE_IS2000_REL_A */

} caii_scrm_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* CANDIDATE FREQUENCY SEARCH RESPONSE MESSAGE(see IS-95B Sec 6.7.2.3.2.19)*/
typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  byte last_cfsrm_seq;        /* CFSRM sequence number */
  byte tot_off_tm_fwd;        /* Total time the mobile is off the FTC */
  byte max_off_tm_fwd;        /* Max time the mobile is off the FTC */
  byte tot_off_tm_rev;        /* Total time the mobile is off the RTC */
  byte max_off_tm_rev;        /* Max time the mobile is off the RTC */
  boolean pcg_off_times;
  boolean align_timing_used;
  byte max_num_visits;
  byte inter_visit_time;
} caii_cfs_rsp_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* CANDIDATE FREQUENCY SEARCH REPORT MESSAGE (see IS-95B Sec 6.7.2.3.2.20) */
typedef struct
{
  byte msg_type;              /* Message type */
  caii_tc_hdr_type hdr;       /* Layer 2 header */
  boolean last_srch_msg;      /* Type of msg started the search */
  byte last_srch_seq;         /* Seq # in the msg started the search */
  byte srch_mode;             /* Search mode */
} caii_cfs_rpt_fix_type;

typedef struct
{
  byte band_class;            /* Band class */
  word cdma_freq;             /* CDMA frequency */
  byte sf_tot_rx_pwr;         /* Total received pwr on the serving freq */
  byte cf_tot_rx_pwr;         /* Total received pwr on the candidate freq*/
  byte num_pilots;            /* Number of pilots */
  struct
  {
    word plt_pn_phase;         /* Pilot PN phase */
    byte plt_str;              /* Pilot strength */
  } pilot[CAI_MAX_CFS_RPT_PLTS];  /* CF srch set same size as NSET. */
  /* Report gives results from CF srch and/or ASET from failed handoff */
#ifdef FEATURE_IS2000_REL_A
  struct
  {
    boolean pilot_rec_incl;    /* pilot record included */
    byte    pilot_rec_type;    /* pilot rec type */
    caii_add_pilot_rec1_type rec;
  } pilot2[CAI_MAX_CFS_RPT_PLTS];  /* CF srch set same size as NSET. */
#endif /* FEATURE_IS2000_REL_A */
} caii_cfs_rpt_mod0_type;

typedef struct
{
  byte band_class;            /* Band Class */
  byte sf_tot_rx_pwr;         /* Total receiving power on the serving freq */
  byte num_analog_freqs;      /* Number of analog frequencies */
  struct
  {
    word analog_freq;         /* Analog frequency */
    byte sig_str;             /* Signal strength */
  } analog[CAI_MAX_ANALOG_FREQS];
} caii_cfs_rpt_mod1_type;

typedef struct
{
  caii_cfs_rpt_fix_type fix;
  union
  {
    caii_cfs_rpt_mod0_type mod0;
    caii_cfs_rpt_mod1_type mod1;
  } var;
} caii_cfs_rpt_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* PERIODIC PILOT STRENGTH MEASUREMENT MESSAGE(see IS-95B Sec 6.7.2.3.2.21)*/
typedef struct
{
  word ref_pn;                   /* Time reference PN seq offset */
  byte pilot_str;                /* Pilot strength */
  boolean keep;                  /* Keep pilot indicator */
  byte sf_rx_pwr;                /* Serving Freq received power density */
  byte num_pilot;                /* Number of pilots */
} caii_plt_str_meas_fix_type;

typedef struct
{
  word    plt_pn_phase;          /* Pilot PN phase */
  byte    plt_str;               /* Pilot strength */
  boolean keep;                  /* keep pilot indicator */
} caii_plt_str_meas_var_type;

#ifdef FEATURE_IS2000_REL_A
typedef struct
{
  boolean    sch_id;
  byte    fpc_sch_curr_setpt;
} caii_plt_str_meas_var2_type;

typedef struct
{
  byte setpt_incl;
  boolean fch_incl;
  byte fpc_fch_curr_setpt;
  boolean dcch_incl;
  byte fpc_dcch_curr_setpt;
  byte num_sup;
  caii_plt_str_meas_var2_type sup[3];
} caii_plt_str_meas_fix2_type;

#endif /* FEATURE_IS2000_REL_A */

typedef struct
{
  byte msg_type;                 /* Message type */
  caii_tc_hdr_type hdr;          /* Layer 2 header */
  caii_plt_str_meas_fix_type  fix;
  caii_plt_str_meas_var_type  var[CAI_PLT_STR_MEAS_MAX];
#ifdef FEATURE_IS2000_REL_A
  struct /* num_nghbr_pn occurences must be filled */
  {
    boolean pilot_rec_incl;
    byte pilot_rec_type;
    caii_add_pilot_rec1_type rec;
  } var2[CAI_PLT_STR_MEAS_MAX];
  caii_plt_str_meas_fix2_type fix2;
#endif /* FEATURE_IS2000_REL_A */
} caii_plt_str_meas_type;


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* OUTER LOOP REPORT MESSAGE (see IS-2000 Sec 2.7.2.3.2.22)                */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef struct
{
  byte msg_type;                 /* Message type */
  caii_tc_hdr_type hdr;          /* Layer 2 header */
  boolean fch_incl;
  byte fpc_fch_curr_setpt;
  boolean dcch_incl;
  byte fpc_dcch_curr_setpt;
  byte num_sup;
  struct
  {
    boolean sch_id;
    byte fpc_sch_curr_setpt;
  } sup[CAI_OUTER_RPT_MAX_SUP];
} caii_outer_rpt_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* RESOURCE REQUEST MESSAGE  (see IS-2000 Sec 2.7.2.3.2.23)                                              */

typedef struct /* Resource Request Message - Reverse Traffic Chn */
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
} caii_rtc_rrm_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* EXTENDED RELEASE RESPONSE MESSAGE  (see IS-2000 Sec 2.7.2.3.2.25)                                              */

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
} caii_rtc_errm_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* RESOURCE RELEASE REQUEST MESSAGE (see IS-2000 Sec 2.7.2.3.2.29)        */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef struct
{
  byte msg_type;                 /* Message type */
  caii_tc_hdr_type hdr;          /* Layer 2 header */
  boolean gating_disconnect_ind;
  byte con_ref;
  boolean purge_service;
} caii_rtc_resource_rel_req_type;

#ifdef FEATURE_IS2000_REL_A
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* USER ZONE UPDATE REQUEST MESSAGE (see IS-2000 Sec 2.7.2.3.2.31)         */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef struct
{
  byte msg_type;                 /* Message type */
  caii_tc_hdr_type hdr;          /* Layer 2 header */
  word uzid;
} caii_rtc_user_zone_update_req_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* ENHANCED ORIGINATION MESSAGE (see IS-2000 Sec 2.7.2.3.2.32)             */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

typedef struct
{
  byte add_sr_id; /* Additional SR_ID */
  byte add_drs; /* DRS value for this additional service */
  byte add_tag; /* Tag value, Used only in EOM not in Orig */
  boolean add_service_option_incl; /* additional SO included indicator */
  word add_service_option; /* additional SO */
  byte add_num_recs; /* number of additional information records */
  dword add_recs[CAII_MAX_INFO_REC_DWORDS];
    /* Additional information records */
} caii_eom_add_serv_instance_type;

typedef struct
{
  byte    msg_type;                 /* Message type */
  caii_tc_hdr_type hdr;             /* Layer 2 header */
  byte    tag;                  /* Tag used for the origination */
  byte    ch_ind;
  #ifdef FEATURE_IS2000_REL_C
  byte    ext_ch_ind;
  #endif
  byte    sr_id;
  boolean global_em_flag;
  boolean ms_init_pos_loc_ind;
  boolean enc_info_incl;
  boolean ui_enc_req;
  byte    ui_enc_sup;
  word    service_option;
  boolean more_so_incl;
  byte    num_alt_so;
  word    alt_so[CAI_AC_ALT_SO_MAX];
  byte    so_bitmap_ind;
  byte    so_group_num;
  word    so_bitmap;
  boolean drs;
  boolean prev_sid_incl;
  word    prev_sid;
  boolean prev_nid_incl;
  word    prev_nid;
  boolean prev_pzid_incl;
  byte    prev_pzid;
  boolean dialed_digs_incl;
  boolean digit_mode;
  byte    number_type;
  byte    number_plan;
  byte    num_fields;
  byte    chari[CAI_AC_ORIG_MAX];  /* Dialed digits */
  byte    num_recs;
  dword   recs[CAII_MAX_INFO_REC_DWORDS];
  /* Records - consecutive Reverse Traffic Channel Information Records -
     use caii_rec_hdr type to parse different records */
  #ifdef FEATURE_IS2000_REL_C
  boolean sync_id_incl; /* Sync ID Included */
  byte sync_id_len;     /* Sync ID length */
  byte sync_id[CAI_SYNC_ID_LEN_MAX]; /* Sync ID */
  #endif
} caii_rtc_enhanced_orig_type;


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* EXTENDED FALSH WITH INFO MESSAGE (see IS-2000 Sec 2.7.2.3.2.33)         */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef struct
{
  byte    msg_type;                 /* Message type */
  caii_tc_hdr_type hdr;             /* Layer 2 header */
  boolean con_ref_incl;
  byte    con_ref;
  byte    num_rec;
  dword   recs[CAII_MAX_INFO_REC_DWORDS];
  /* Records - consecutive Reverse Traffic Channel Information Records -
     use caii_rec_hdr type to parse different records */
} caii_rtc_ext_flash_w_info_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* EXTENDED PILOT STRENGTH MEAS MSG (IS-2000 Release A Sec 2.7.2.3.2.34)   */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

typedef struct
{
  byte msg_type;            /* Message type */
  caii_tc_hdr_type hdr;     /* Layer 2 header */
  word ref_pn;              /* Time reference PN sequence offset */
  byte pilot_strength;      /* Pilot strength */
  boolean keep;             /* Keep pilot indicator */
  boolean ref_pilot_rec_incl;
  byte    ref_pilot_rec_type;
  caii_add_pilot_rec1_type    ref_rec;    /* Note that only auxiliary pilot
                                          record type parameters can be used */
  byte  sf_rx_power;        /* sf received power */
  byte  num_pilots;             /* Number of entries in the meas array */
  struct
  {
    word pilot_pn_phase;  /* Pilot measured phase */
    byte pilot_strength;  /* Pilot strength */

    boolean keep;         /* Keep pilot indicator */
    boolean pilot_rec_incl;
    byte pilot_rec_type;
    caii_add_pilot_rec1_type rec;
  } meas[CAI_EPSMM_MAX_NUM_PILOTS];

#ifdef FEATURE_IS2000_REL_B
  boolean resq_ind_incl;
  struct
  {
    boolean resq_ind;
  } resq[CAI_EPSMM_MAX_NUM_PILOTS];
#endif /* FEATURE_IS2000_REL_B */

} caii_rtc_ext_pil_str_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* EXTENDED HANDOFF COMP MSG (IS-2000 Release A Sec 2.7.2.3.2.35)          */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

typedef struct
{
  byte msg_type;         /* Message type */
  caii_tc_hdr_type hdr;  /* Layer 2 header */
  byte last_hdm_seq;     /* Handoff Direction Message sequence number */
  byte num_pilots;       /* Number of pilots in pilot_pn array */
  struct
  {
    word pilot_pn;
    boolean pilot_rec_incl;
    byte pilot_rec_type;
    caii_add_pilot_rec1_type rec;
  } act_pil_rec[CAI_MAX_COMP];
} caii_rtc_ehcm_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* SECURITY MODE REQUEST MSG (IS-2000 Release A Sec 2.7.2.3.2.36)          */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define MAX_TSMRM_RECS 8

typedef struct
{
  byte    msg_type;         /* Message type */
  caii_tc_hdr_type hdr;   /* Layer 2 header */
  boolean ui_enc_incl;   /* UI Encryption included */
  byte    ui_encrypt_sup;/* UI Encryption supported */
  byte    num_recs;      /* number of records */
  struct  /* identified by num_rec */
  {
    byte       con_ref;  /* Connection reference */
    boolean    ui_encrypt_req;
  } recs[MAX_TSMRM_RECS];
  boolean sig_enc_incl;
  byte    sig_encrypt_sup;
  boolean d_sig_encrypt_req;
  boolean enc_seq_h_incl;
  dword   enc_seq_h;
  byte    enc_seq_h_sig;
#ifdef FEATURE_IS2000_REL_C
  boolean  msg_int_info_incl;  /* signaling mesg integrity info included*/
  boolean  sig_integrity_sup_incl; /* signaling mesg integrity algo supported*/
  byte  sig_integrity_sup; /* signaling integrity algo supported by MS */
  byte  sig_integrity_req; /* signaling mesg integrity algo requested*/
#endif /* FEATURE_IS2000_REL_C */
} caii_rtc_smrm_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* CALL CANCEL  MSG (IS-2000 Release A Sec 2.7.2.3.2.37)                   */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef struct
{
  byte    msg_type;       /* Message type */
  caii_tc_hdr_type hdr;   /* Layer 2 header */
  byte    tag;            /* tag */
} caii_rtc_ccm_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* DEVICE INFORMATION MSG (IS-2000 Release A Sec 2.7.2.3.2.38)             */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

typedef struct
{
  byte    msg_type;                 /* Message type */
  caii_tc_hdr_type hdr;             /* Layer 2 header */
  byte    wll_device_type;
  byte    num_recs;
  dword   recs[CAII_MAX_INFO_REC_DWORDS];
  /* Records - consecutive Reverse Traffic Channel Information Records -
     use caii_rec_hdr type to parse different records */
} caii_rtc_dim_type;

#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A
/* Base Station Request Message. 2.7.2.3.2.39 */
typedef struct
{
  byte      num_pilots;
  boolean   sid_nid_req;
  word      pilot_pn[CAI_BSS_NUM_PILOTS];
} caii_bss_req_pi_type;

typedef struct
{
  byte                      msg_type;       /* Message type */
  caii_tc_hdr_type          hdr;            /* Layer 2 header */
  byte                      qual;           /* Qualification information */
  byte                      band;           /* Band class qualifier */
  byte                      op_mode;        /* Operating mode qualifier */
  byte                      num_record;
  struct
  {
    byte                    record_type;
    caii_bss_req_pi_type    pi;
  } bss_rec[CAI_BSS_NUM_RECORDS];

} caii_bs_status_req_type;

#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_B
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* CDMA OFF TIME REPORT MESSAGE (Release B Sec 2.7.2.3.2.40)               */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef struct
{
  byte msg_type;                  /* Message type */
  caii_tc_hdr_type hdr;           /* Layer 2 header */
  boolean cdma_off_time_enable;   /* Off time reporting enable */
  byte cdma_off_time_start;       /* Start time when ms moves away from Traffic Channel */
  byte cdma_off_time_unit;        /* Time unit used in cdma_off_time */
  boolean cdma_off_time_mode;     /* Indicates whether cdma_off_time is periodic */
  byte cdma_off_time;             /* Total time ms plans to be away from Traffic Channel */
  byte cdma_off_time_period;      /* Time between beginning of successive off times */
} caii_rtc_cotrm_type;
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C

/* Authentication Response Message */
typedef struct
{
  byte msg_type;                  /* Message type */
  caii_tc_hdr_type hdr;           /* Layer 2 header */
  /* Following is a 128-bit field */
  qword res_hi;                 /* O/p of user auth fn */
  qword res_lo;                 /* O/p of user auth fn */

  boolean sig_integrity_sup_incl; /* sig mesg integrity info included indicator*/
  byte sig_integrity_sup;      /* Signaling integrity algo supported by MS */
  byte sig_integrity_req;      /* Signaling integrity algo requested by MS */
  byte new_key_id;             /* New key Identifier */
  dword new_sseq_h;           /* 24 bit value used for MSB of crypto sync */
} caii_rtc_aurspm_type;

/* Authentication Resynchronization Message */
typedef struct
{
  byte msg_type;                  /* Message type */
  caii_tc_hdr_type hdr;           /* Layer 2 header */
  qword con_ms_sqn;        /* Conceled seq num of auth vector */
  qword mac_s;             /* Msg auth code for resynchronization */
} caii_rtc_aursynm_type;

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   Union of Received Messages                                            */

typedef union
{
  /* Generic header. For non-test mode only */
  caii_gen_type                   gen;

  /* Sync Channel Message Type */
  caii_sync_type                  sync;

  /* f-csch Message Types */
  caii_gen_pc_type                gen_pc;
  caii_sysparm_type               sysparm;
  caii_accparm_type               accparm;
  caii_ext_nghbr_type             ext_nghbr;
  caii_nghbr_type                 nghbr;
  caii_chnlist_type               chnlist;
  caii_slt_page_type              slt_page;
  caii_page_type                  page;
  caii_pc_ord_type                pc_ord;
  caii_chnasn_type                chnasn;
  caii_pc_burst_type              pc_burst;
  caii_pc_auth_ch_type            pc_auth;
  caii_pc_ssd_type                pc_ssd;
  caii_pc_feature_type            pc_feat;
  caii_ext_sysparm_type           ext_sysparm;
  caii_serv_redirect_type         serv_redir;
  caii_pc_streq_type              pc_streq;
  caii_gen_page_type              gen_page;
  caii_glob_redirect_type         glob_redir;
  caii_paca_type                  paca;
  caii_ext_chnasn_type            ext_chnasn;
  caii_gen_nghbr_type             gen_nghbr;
  caii_ext_glb_redir_type         egsrm;
  caii_ext_chnlist_type           ecclm;
#ifdef FEATURE_IS2000_REL_A
  caii_sec_mode_cmd_type          smcm;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
  caii_auth_req_type              auth_req;
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_1X_ADV
  caii_pc_gen_extn_type           pc_gem;
#endif /* FEATURE_IS2000_1X_ADV */

#ifdef FEATURE_1X_TO_LTE
  caii_alt_tech_info_msg_type     atim;
#endif /* FEATURE_1X_TO_LTE */

  /* f-dsch Message Types */
  caii_gen_tc_type                gen_tc;
  caii_ftc_ord_type               tc_ord;
  caii_auth_ch_type               auth_ch;
  caii_alert_type                 alert;
  caii_fwd_tc_burst_type          tc_burst;
  caii_ho_msg_type                ho;       /* Generic TC handoff msg. */
  caii_fm_ho_type                 fm_ho;
  caii_tc_sysparm_type            tc_sys;
  caii_nlu_type                   nlu;
  caii_sbdtmf_type                sbdtmf;
  caii_pwr_ctl_type               pwr_ctl;
  caii_ret_parms_type             ret;
  caii_set_parm_type              set;
  caii_ssd_up_type                ssd_up;
  caii_registered_type            reg;
  caii_tc_streq_type              tc_streq;
  caii_ftc_srv_req_type           srv_req;
  caii_ftc_srv_rsp_type           srv_rsp;
  caii_srv_con_type               srv_con;
  caii_ftc_srv_ctl_type           srv_ctl;
  caii_pc_tmsi_asgn_type          pc_tmsi;
  caii_ftc_tmsi_asgn_type         tc_tmsi;
  caii_ftc_srv_redir_type         tc_srv_redir;
  caii_scam_type                  scam;
  caii_pwr_ctrl_type              pwr_ctrl;
  caii_ext_nlu_type               ext_nlu;
  caii_cfs_req_type               cfs_req;
  caii_cfs_ctrl_type              cfs_ctrl;
  caii_puf_type                   puf;
  caii_puf_comp_type              puf_comp;
  caii_escam_type                 escam;
  caii_erm_type                   erm;
  caii_ram_type                   ram;

#ifdef FEATURE_IS2000_REL_A
  caii_mabop_type                 tcmabop;
  caii_user_zone_rej_type         tcuzrm;
  caii_user_zone_upd_type         tcuzum;
  caii_call_assignment_type       tccam;
  caii_extended_alert_w_info_type tceawim;
    /* extended flash, use extended alert */
  caii_security_mode_cmd_type     tcsmcm;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A
  caii_bs_status_rsp_type         bssrsp;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C
  caii_tc_auth_req_type           tcauthreq;
  caii_rate_change_type           rate_change;
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_1X_ADV
  caii_radio_config_type          rcpm;
  caii_tc_gen_extn_type           tc_gem;
#endif /* FEATURE_IS2000_1X_ADV */

#ifdef FEATURE_1X_TO_LTE
  caii_tc_alt_tech_info_msg_type     tc_atim;
#endif /* FEATURE_1X_TO_LTE */

} caii_rx_msg_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   Union of Transmitted Messages                                         */

typedef union
{
  caii_gen_type          gen;
  caii_gen_ac_type       gen_ac;
  caii_gen_tc_type       gen_tc;
  caii_reg_type          reg;
  caii_ac_ord_type       ac_ord;
  caii_ac_burst_type     ac_burst;
  caii_orig_type         orig;
  caii_page_resp_type    page_resp;
  caii_auth_ch_resp_type auth_ch;
  caii_auth_rsp_type     auth_resp;
  caii_ac_strsp_type     ac_strsp;
  caii_rtc_ord_type      tc_ord;
  caii_orig_c_type       orig_c;
  caii_flash_type        flash;
  caii_status_type       status;
  caii_rev_tc_burst_type tc_burst;
  caii_pil_str_type      pil_str;
  caii_pwr_rpt_type      pwr_rpt;
  caii_sbdtmf_type       sbdtmf;
  caii_parm_rsp_type     parm_rsp;
  caii_comp_type         comp;
  caii_rtc_srv_req_type  srv_req;
  caii_rtc_srv_rsp_type  srv_rsp;
  caii_srv_cmp_type      srv_cmp;
  caii_rtc_srv_ctl_type  srv_ctl;
  caii_tc_strsp_type     tc_strsp;
  caii_ac_tmsi_cmp_type  ac_tmsi_cmp;
  caii_rtc_tmsi_cmp_type tc_tmsi_cmp;
  caii_ac_paca_can_type  ac_paca_can;
  caii_ac_ext_strsp_type ac_ext_strsp;
  caii_scrm_type         scrm;
  caii_cfs_rsp_type      cfs_rsp;
  caii_cfs_rpt_type      cfs_rpt;
  caii_plt_str_meas_type per_plt;
  caii_outer_rpt_type    outer_rpt;

  caii_rtc_rrm_type                   rrm;
  caii_rtc_errm_type                  errm;
  caii_rtc_resource_rel_req_type      rrrm;

#ifdef FEATURE_IS2000_REL_A
  caii_ac_dim_type       dim;
  caii_ac_smrm_type      smrm;
  /* Reverse Traffic in Rel A */
  caii_rtc_user_zone_update_req_type  uzurm;
  caii_rtc_enhanced_orig_type         eom;
  caii_rtc_ext_flash_w_info_type      efwim;
  caii_rtc_ext_pil_str_type           epsmm;
  caii_rtc_ehcm_type                  ehcm;
  caii_rtc_smrm_type                  tsmrm;
  caii_rtc_ccm_type                   ccm;
  caii_rtc_dim_type                   rdim;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A
  caii_bs_status_req_type             bssreq;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_B
  caii_rtc_cotrm_type                 cotrm;
#endif /* FEATURE_IS2000_REL_B */
#ifdef FEATURE_IS2000_REL_C
  caii_ac_rcnm_type                   rcnm;
  caii_ac_aurspm_type                 ac_aurspm;
  caii_ac_aursynm_type                ac_aursynm;
  caii_rtc_aurspm_type                taurspm;
  caii_rtc_aursynm_type               taursynm;
#endif /* FEATURE_IS2000_REL_C */

} caii_tx_msg_type;
#endif /*CAII_I_H*/
