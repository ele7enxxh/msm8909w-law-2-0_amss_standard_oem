#ifndef CAII_V_H
#define CAII_V_H
/*===========================================================================

         C D M A   I N T E R N A L   D A T A   S T R U C T U R E S

DESCRIPTION
  This file contains data structures and constants which represent
  the messages transmitted between the mobile and base stations while
  operating in the CDMA mode. This format is only used internally to
  the Main Control task.

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/caii.h_v   1.78   30 Sep 2002 17:45:02   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/caii_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/18/10   jtm     Removed featurization.
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

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"               /* Basic definitions */
#include "qw.h"                   /* Quadword definitions */

#include "cai.h"
#include "cai_v.h"                  /* CAI definitions */
#include "caii.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                         IMSI ADDRESS TYPES                              */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* IMSI Class 0 Type specific subfields (Class Type = 0) */
typedef struct
{
  qword imsi_s;             /* Last ten digits of the IMSI */
} caii_imsi_class0_type0_type;

/* IMSI Class 0 Type specific subfields (Class Type = 1) */
typedef struct
{
  byte  imsi_11_12;         /* The 11th and 12th digits of IMSI */
  qword imsi_s;             /* Last ten digits of the IMSI */
} caii_imsi_class0_type1_type;

/* IMSI Class 0 Type specific subfields (Class Type = 2) */
typedef struct
{
  word  mcc;                /* Mobile country code */
  qword imsi_s;             /* Last ten digits of the IMSI */
} caii_imsi_class0_type2_type;

/* IMSI Class 0 Type specific subfields (Class Type = 3) */
typedef struct
{
  word  mcc;                /* Mobile country code */
  byte  imsi_11_12;         /* The 11th and 12th digits of IMSI */
  qword imsi_s;             /* Last ten digits of the IMSI */
} caii_imsi_class0_type3_type;

/* Union of structures for IMSI Class 0 type specific subfields */
typedef struct
{
  byte imsi_class_0_type;  /* IMSI Class 0 type */
  union
  {
    caii_imsi_class0_type0_type zero;
    caii_imsi_class0_type1_type one;
    caii_imsi_class0_type2_type two;
    caii_imsi_class0_type3_type three;
  } type;
} caii_imsi_class_0_type;

/* IMSI Class 1 Type specific subfields (Class Type = 0) */
typedef struct
{
  byte  imsi_addr_num;      /* Number of IMSI address digits */
  byte  imsi_11_12;         /* The 11th and 12th digits of IMSI */
  qword imsi_s;             /* Last ten digits of the IMSI */
} caii_imsi_class1_type0_type;

/* IMSI Class 1 Type specific subfields (Class Type = 1) */
typedef struct
{
  byte  imsi_addr_num;      /* Number of IMSI address digits */
  word  mcc;                /* Mobile country code */
  byte  imsi_11_12;         /* The 11th and 12th digits of IMSI */
  qword imsi_s;             /* Last ten digits of the IMSI */
} caii_imsi_class1_type1_type;

/* Union of structures for IMSI Class 1 type specific subfields */
typedef struct
{
  byte imsi_class_1_type;  /* IMSI Class 0 type */
  union
  {
    caii_imsi_class1_type0_type zero;
    caii_imsi_class1_type1_type one;
  } type;
} caii_imsi_class_1_type;

/* Union of structures for IMSI type specific subfields */
typedef struct
{
  byte imsi_class;       /* IMSI Class */
  union
  {
    caii_imsi_class_0_type zero;
    caii_imsi_class_1_type one;
  } class_type;
} caii_imsi_addr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  boolean sr_id_1;
  boolean sr_id_2;
  boolean sr_id_3;
  boolean sr_id_4;
  boolean sr_id_5;
  boolean sr_id_6;
} caii_sr_id_bitmap_type;

typedef struct
{
  byte tmsi_len;
  qword tmsi_zone;       /* TMSI zone */
  dword tmsi_code;       /* TMSI code */
} caii_tmsi_type;

/* Access Channel Mobile Station Identifier Fields (MSID_TYPE = '000') */
typedef struct
{
  dword min1;        /* First part of the mobile identification number */
  word  min2;        /* Second part of the mobile identification number */
  dword esn;         /* Mobile station's electronic serial number */
} caii_msid0_type;

/* Access Channel Mobile Station Identifier Fields (MSID_TYPE = '001')  */
/* (NOTE: For now the mobile station's ESN field is limited to 32 bits) */
typedef struct
{
  dword esn;         /* Mobile station's electronic serial number */
} caii_msid1_type;

/* Access Channel Mobile Station Identifier Fields (MSID_TYPE = '010')      */
/* (NOTE: For now the mobile station only supports Class 0 IMSI addresses)  */
typedef struct
{
  caii_imsi_addr_type imsi;  /* IMSI address type specific subfields */
} caii_msid2_type;

/* Access Channel Mobile Station Identifier Fields (MSID_TYPE = '011')      */
/* (NOTE: For now the mobile station only supports Class 0 IMSI addresses)  */
typedef struct
{
  dword esn;                 /* Mobile's electronic serial number */
  caii_imsi_addr_type imsi;  /* IMSI address type specific subfields */
} caii_msid3_type;

/* Access Channel Mobile Station Identifier Fields (MSID_TYPE = '100') */
/* EXT_MSID_TYPE = '000' */
typedef struct
{
  qword meid;                /* Mobile station's Equipment Identification */
} caii_ext_msid_0_type;

/* Access Channel Mobile Station Identifier Fields (MSID_TYPE = '100')  */
/* EXT_MSID_TYPE = '001' */
typedef struct
{
  qword meid;                /* Mobile station's Equipment Identification */
  caii_imsi_addr_type imsi;  /* IMSI address type specific subfields */
} caii_ext_msid_1_type;

/* Access Channel Mobile Station Identifier Fields (MSID_TYPE = '100')      */
/* (NOTE: For now the mobile station only supports Class 0 IMSI addresses)  */
/* EXT_MSID_TYPE = '010' */
typedef struct
{
  dword esn;                 /* Mobile's (pseudo) electronic serial number */
  qword meid;                /* Mobile station's Equipment Identification */
  caii_imsi_addr_type imsi;  /* IMSI address type specific subfields */
} caii_ext_msid_2_type;

/* Union of structures for the Mobile station identifier (MSID) field */
typedef union
{
  caii_msid0_type type0;
  caii_msid1_type type1;
  caii_msid2_type type2;
  caii_msid3_type type3;
  caii_tmsi_type  type5;
  caii_ext_msid_0_type type4_ext0;   /* MEID */
  caii_ext_msid_1_type type4_ext1;   /* IMSI, MEID */
  caii_ext_msid_2_type type4_ext2;   /* IMSI, ESN, MEID */
} caii_msid_type;

/* Extended Display record ( see IS-95B Section 7.7.5.16 ) */
typedef struct
{
  caii_rec_hdr_type hdr;    /* Header information */
  boolean ext_display_ind;  /* The indicator if Extended Display record */
  byte display_type;        /* Display type */
  byte rec_len;             /* number of bytes in the variable part */
} caii_ext_disp_fix_type;


typedef struct  /* used to access the variable part of Extended Display */
{
  byte display_tag;  /* Indicator of the display information */
  byte display_len;  /* The display length */
                     /*lint -e43 */
  byte chari[1];     /* The character string (not null termd) */
                     /* This structure is only used for packing only. It
                     ** is not instantiated in the parser. The size
                     ** of the array is set to one to appease the ARM
                     ** 2.50 compiler. This is a variable length array
                     ** of length display_len and is determined when
                     ** the record is recieved.
                     */
                     /*lint +e43 */
} caii_ext_disp_var_type;


#define CAII_EXT_DISP_MAX  246
/*  2016 bits (max forward traffic channel message body size)
  -   24 bits (msg_type + common acknowledgement fields + reserved bits)
  -   16 bits (info record type and length fields)
  -    8 bits (type specific field)
  -----------
    1968 bits
  /    8 bits (how many bytes left )
  -----------
     246 byte( for variable variable field) */

typedef struct
{
  caii_ext_disp_fix_type fix;
  byte num_disp_recs;
  byte var[CAII_EXT_DISP_MAX];
} caii_ext_disp_type;

/* Interleaved address fields */
typedef struct
{
  boolean bcast_included;      /* Enhanced broadcast record indicator */
  byte num_bcast;              /* Number of enhanced broadcast record */
  boolean imsi_included;       /* IMSI record indicator */
  byte num_imsi;               /* Number of IMSI record */
  boolean tmsi_included;       /* TMSI record indicator */
  byte num_tmsi;               /* Number of TMSI record */
  boolean res_type_included;   /* Reserved type record indicator */
  byte num_res_type;           /* Number of reserved type record */
  byte burst_type[CAI_UNIV_PAGE_NUM_BCAST_REC_MAX];
  struct
  {
    dword bc_addr;        /* Interleaved Broadcast address portion */
    qword imsi_addr;      /* Interleaved IMSI address portion */
    qword tmsi_addr;      /* Interleaved TMSI address portion */
    qword res_type_addr;  /* Interleaved Reserved address portion*/
  } addr_block[CAI_UNIV_PAGE_INTERLEAVED_ADDR_PORTION];
    /* Interleaved Bcast, IMSI, TMSI and Reserved address block */
  boolean initialized;
    /* Indicates if interleaved address structure initialized */
  byte bc_addr[CAI_UNIV_PAGE_NUM_BCAST_REC_MAX][2];
    /* Interleaved Broadcast address block */
  qword imsi_addr;
    /* Interleaved IMSI address block */
  qword tmsi_addr;
    /* Interleaved TMSI address block */
  byte iaddr_portion_len;
    /* Length of each interleaved address portion in bits */
  byte iaddr_portion_rxed;
    /* Number of interleaved address portion received */
  byte iaddr_portion_matched;
    /* Number of interleaved address portions matched */
  boolean bcast_match;
    /* Interleaved Broadcast address match indicator */
  byte bcast_bit_matched;
    /* Number of interleaved broadcast address bits matched */
  qword imsi_match;
    /* Interleaved IMSI address match indicator */
  byte imsi_bit_matched;
    /* Number of interleaved IMSI address bits matched */
  qword tmsi_match;
    /* Interleaved TMSI address match indicator */
  byte tmsi_bit_matched;
    /* Number of interleaved TMSI address bits matched */
} caii_univ_page_interleaved_addr_type;

typedef struct
{
  boolean for_gch_assigned;      /* F-GCH assignment indicator */
  boolean for_rcch_assigned;     /* F-RCCH assignment indicator */
  boolean for_rcch_drc_mode;     /* F-RCCH DRC mode */
  byte for_rcch_repetition;      /* F-RCCH repetition factor */
  byte for_rcch_update_rate;     /* F-RCCH update rate */
  boolean for_ackch_assigned;    /* F-ACKCH assignment indicator */
  byte for_ackch_mode;           /* F-ACKCH mode */
  boolean for_ackch_comb_sel;    /* F-ACKCH combining methos selector */
  boolean rev_pdch_parms_incl;   /* R-PDCH parameters subset included indicator */
  boolean rev_pdch_rlgain_incl;  /* R-PDCH parameters related to reverse link
                                    adjustment gains included indicator */
  byte rlgain_spich_pilot;       /* R-SPICH to pilot adjustment gain */
  byte rlgain_reqch_pilot;       /* R-REQCH to pilot adjustment gain */
  byte rlgain_pdcch_pilot;       /* R-PDCCH to pilot adjustment gain */
  boolean rev_pdch_parms_1_incl; /* R-PDCH parameters subset included indicator */
  boolean rev_pdch_table_sel;    /* R-PDCH table selector */
  byte rev_pdch_max_auto_tpr;    /* R-PDCH maximum traffic to pilot ratio for
                                    autonomous transmission */
} caii_rev_pdch_parm_type;


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Additional Pilot Record Type Definition */

typedef struct
{
  byte td_power_level;      /* TD power level, same as OTD one above */
  byte td_mode;             /* transmit diversity mode */
} caii_add_pilot_rec0_type;

typedef PACK (struct)
{
  byte qof;             /* quasi-orthogonal function index */
  byte walsh_length;    /* length of the walsh code in addition to base (6 bits)*/
  word aux_pilot_walsh; /* walsh code (minimum length) */
} caii_add_pilot_rec1_type;

typedef PACK (struct)
{
  byte qof;                /* quasi-orthogonal function index for Aux TDP */
  byte walsh_length;        /* length of the walsh code in addition to base (6 bits)*/
  word aux_walsh;       /* walsh code for the auxiliary TD pilot (minimum) */
  byte aux_td_power_level; /* aux TD pilot power level */
  byte td_mode;            /* transmit diversity mode */
} caii_add_pilot_rec2_type;

typedef PACK (struct)
{
  byte sr3_primary_pilot; /* primary SR3 pilot */
  byte sr3_pilot_power1;  /* power level diff between primary and lower freq SR3 pilot */
  byte sr3_pilot_power2;  /* power level diff between primary and higher freq SR3 pilot */
} caii_add_pilot_rec3_type;

typedef PACK (struct)
{
  byte sr3_primary_pilot;/* primary SR3 pilot */
  byte sr3_pilot_power1; /* power level diff between primary and lower freq SR3 pilot */
  byte sr3_pilot_power2; /* power level diff between primary and higher freq SR3 pilot */
  byte qof;              /* quasi-orthogonal function index */
  byte walsh_length;     /* length of the walsh code in addition to base (6 bits) */
  word aux_pilot_walsh;  /* walsh code for the auxiliary pilot (minimum) */
  boolean add_info_incl1;/* additional info for lower freq pilot included indicator */
  byte qof1;             /* quasi-orthogonal function index for lower freq pilot */
  byte walsh_length1;    /* length of the walsh code for lower freq pilot */
  word aux_pilot_walsh1; /* walsh code for the lower freq pilot */
  boolean add_info_incl2;/* additional info for higher freq pilot included indicator */
  byte qof2;             /* quasi-orthogonal function index for higher freq pilot */
  byte walsh_length2;     /* length of the walsh code for the higher freq pilot */
  word aux_pilot_walsh2; /* walsh code for the higher freq pilot */
} caii_add_pilot_rec4_type;

typedef union
{
  caii_add_pilot_rec0_type rec0;
  caii_add_pilot_rec1_type rec1;
  caii_add_pilot_rec2_type rec2;
  caii_add_pilot_rec3_type rec3;
  caii_add_pilot_rec4_type rec4;
} caii_add_pilot_rec_type;

#endif /*CAII_V_H*/
