#ifndef OTASPE_H
#define OTASPE_H
/*===========================================================================

                  O T A S P   D A T A   S T R U C T U R E S

DESCRIPTION
  This file contains data structures and constants which represent
  the external OTASP message formats and values transmitted between the
  mobile and base stations.  These message formats are only used
  between the Main Control and TX/RX tasks.

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
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

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/otaspe.h_v   1.0.2.0   30 Nov 2001 17:54:52   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/otaspe.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
02/09/11   ag      Mainlined FEATURE_EHRPD.
07/19/10   ag      Replaced "PACKED <name>" with "PACK (<name>)".
10/21/09   ag      Added support for eHRPD IMSI OTASP operation.
07/04/05   ht      Added OTASP MEID support.
11/30/04   jqi     Added FEATURE_IS683C_OTASP to complete OTASP support for
                   IS683C PRL.
07/01/03   RC/aaj  Changed PREV from 1 to 3 for IS683C PRLs
03/14/99   ry      Added OTAPA support
01/20/99   ck      Added support for IS-683A.
09/01/98   lcc     Merged in ARM support.
08/08/96   dna     Added defines for Sprint subsidy lock and pr list dload
06/25/96   dna     Checked in initial version.
04/16/96   day     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                     OTASP MESSAGE TYPES                                 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* -------------------------------------------------------
** See IS-683A Section 4.5 OTASP Forward Link Message Types
** ------------------------------------------------------- */

#define OTASP_CONFIG_REQ_MSG         0x00
  /* IS-683 OTASP Configuration Request Message */
#define OTASP_DOWNLOAD_REQ_MSG       0x01
  /* IS-683 OTASP Download Request Message */
#define OTASP_MS_KEY_REQ_MSG         0x02
  /* IS-683 OTASP MS Key Request Message */
#define OTASP_KEY_GEN_REQ_MSG        0x03
  /* IS-683 OTASP Commit Request Message */
#define OTASP_REAUTH_REQ_MSG         0x04
  /* IS-683 OTASP Key Generation Request Message */
#define OTASP_COMMIT_REQ_MSG         0x05
  /* IS-683 OTASP Re-Authenticate Request Message */
#define OTASP_PROT_CAP_REQ_MSG       0x06
  /* IS-683 OTASP Protocol Capability Request Message */
#define OTASP_SSPR_CONFIG_REQ_MSG    0x07
  /* IS-683A OTASP SSPR Configuration Request Message  */
#define OTASP_SSPR_DOWNLOAD_REQ_MSG  0x08
  /* IS-683A OTASP SSPR Download Request Message  */
#define OTASP_VALIDATION_REQ_MSG     0x09
  /* IS-683 OTASP Validation Request Message */
#define OTASP_OTAPA_REQ_MSG          0x0a
  /* IS-683 OTASP OTAPA Request Message */

/* -------------------------------------------------------
** See IS-683A Section 3.5 OTASP Reverse Link Message Types
** ------------------------------------------------------- */

#define OTASP_CONFIG_RSP_MSG        0x00
  /* IS-683 OTASP Configuration Response Message */
#define OTASP_DOWNLOAD_RSP_MSG      0x01
  /* IS-683 OTASP Download Response Message */
#define OTASP_MS_KEY_RSP_MSG        0x02
  /* IS-683 OTASP MS Key Response Message */
#define OTASP_KEY_GEN_RSP_MSG       0x03
  /* IS-683 OTASP Key Generation Response Message */
#define OTASP_REAUTH_RSP_MSG        0x04
  /* IS-683 OTASP Re-Authenticate Response Message */
#define OTASP_COMMIT_RSP_MSG        0x05
  /* IS-683 OTASP Commit Response Message */
#define OTASP_PROT_CAP_RSP_MSG      0x06
  /* IS-683 OTASP Protocol Capability Response Message */
#define OTASP_SSPR_CONFIG_RSP_MSG   0x07
  /* IS-683A OTASP SSPR Configuration Response Message  */
#define OTASP_SSPR_DOWNLOAD_RSP_MSG 0x08
  /* IS-683A OTASP SSPR Download Response Message  */
#define OTASP_VALIDATION_RSP_MSG    0x09
  /* IS-683 OTASP Validation Response Message */
#define OTASP_OTAPA_RSP_MSG         0x0a
  /* IS-683 OTASP OTAPA Response Message */
#define OTASP_EXT_PROT_CAP_RSP_MSG  0x10
  /* IS-683D OTASP Protocol Capability Response Message */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                 OTASP PARAMETER BLOCK TYPES                             */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-683A, Table 3.5.2-1 & Table 4.5.2-1                               */
/* Used to identify type of OTASP parameter blocks.                        */

#define OTASP_CDMA_ANALOG_NAM_BLK     0x00
  /* IS-683 Cellular Indicator Download Data */
#define OTASP_MOB_DIR_NUM_BLK         0x01
  /* IS-683 Mobile Directory Number */
#define OTASP_CDMA_NAM_BLK            0x02
  /* IS-683 PCS Indicator Download Data */
#define OTASP_IMSI_T_BLK              0x03
  /* IS-683A IMSI_T Download data */
#define OTASP_EHRPD_IMSI_BLK          0x04
  /* IS-683 eHRPD IMSI Download data */

#define OTASP_VERIFY_SPC_BLK          0x80
  /* PCS PrimeCo. proposed programming lock 7/16/96 */
#define OTASP_PREF_ROAM_LIST_SIZE_BLK 0x81
  /* Preferred Roaming List Dimensions */
#define OTASP_PREF_ROAM_LIST_BLK      0x82
  /* Preferred Roaming List */

/* See IS-683A, Table 3.5.3-1
** Used to identify type of SSPR parameter block for Configuration
** Request and Response                                               */
#define OTASP_CFG_PRL_DIMENSIONS_BLK  0x00
  /* IS-683A Prefered roaming list dimensions  */
#define OTASP_CFG_PREF_ROAM_LIST_BLK  0x01
  /* IS_683A Preferred Roaming list */
/* See IS-683C, Table 3.5.3-1 */
#define OTASP_CFG_EXT_PRL_DIMENSIONS_BLK  0x02
  /* IS-683C Extended Prefered roaming list dimensions  */

/* See IS-683A, Table 4.5.3-1
** Used to identify type of SSPR parameter block for Download
** Request and Response  */
#define OTASP_DLOAD_PREF_ROAM_LIST    0x00
  /* IS_683A Preferred Roaming list */

#define OTASP_DLOAD_EXT_PREF_ROAM_LIST  0x01
  /* IS_683C Extended Preferred Roaming list */

/* See IS-683A, Table 4.5.4-1                                              */
/* Used to identify type of OTASP Validation parameter blocks.             */
#define OTASP_VERIFY_SPC_BLOCK        0x00
  /* Verify  SPC       */
#define OTASP_CHANGE_SPC_BLOCK        0x01
  /* Change  SPC       */
#define VALIDATE_SPASM                0x02
  /* Validate SPASM    */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                 OTASP DOWNLOAD RESULT CODES                             */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-683A Table 3.5.1.2-1                                              */
/* Used to identify download status of each OTASP parameter block.         */

#define OTASP_BLK_ACCEPT        0x00
  /* Accepted - Operation successful */
#define OTASP_BLK_REJ_UNKNOWN   0x01
  /* Rejected - Unknown reason */
#define OTASP_BLK_REJ_DATA_SIZE 0x02
  /* Rejected - Data size mismatch */
#define OTASP_BLK_REJ_PROTOCOL  0x03
  /* Rejected - Protocol version mismatch */
#define OTASP_BLK_REJ_INVALID   0x04
  /* Rejected - Invalid parameter */
#define OTASP_BLK_REJ_SID_NID   0x05
  /* Rejected - SID/NID length mismatch */
#define OTASP_BLK_REJ_MODE      0x06
  /* Rejected - Message not expected in this mode */
#define OTASP_BLK_REJ_BLOCK_ID  0x07
  /* Rejected - BLOCK_ID value not supported      */
#define OTASP_BLK_REJ_PRL_LEN   0x08
  /* Rejected - Pref Roam list length mismatch */
#define OTASP_BLK_REJ_CRC       0x09
  /* Rejected - CRC Error */
#define OTASP_BLK_REJ_MS_LOCKED 0x0a
  /* Rejected - Mobile Station locked */
#define OTASP_BLK_REJ_SPC       0x0b
  /* Rejected - Invalid SPC */
#define OTASP_BLK_REJ_SPC_USER  0x0c
  /* Rejected - SPC Change denied by the user */
#define OTASP_BLK_REJ_SPASM     0x0d
  /* Rejected - Invlaid SPASM */
#define OTASP_BLK_REJ_MODE_BLOCK_ID 0x0e
  /* Rejected - Block ID not expected in this mode */

#define OTASP_BLK_REJ_PRL_MISMATCH 0x23
  /* Rejected - PRL format mismatch */

#define OTASP_BLK_REJ_BAD_SPC   0x80
  /* Rejected - Incorrect SPC */
#define OTASP_BLK_REJ_SPC_REQ   0x81
  /* Rejected - SPC required */
#define OTASP_BLK_REJ_NO_MEM    0x82
  /* Rejected - Memory Overflow during Roaming List Download */
#define OTASP_BLK_REJ_BAD_SEQ   0x83
  /* Rejected - Sequence Number Error during Roaming List Download */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*               OTASP FEATURE IDENTIFIER CODES                            */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* See IS-683A, Table 3.5.1.7-1                                             */
/* Used to identify supported OTASP features.                              */

#define OTASP_NAM_DOWNLOAD 0x00
  /* NAM Download Block */
#define OTASP_KEY_EXCHANGE 0x01
  /* Key Exchange Block */

#define OTASP_PREF_ROAM    0x02
  /* System Selection for Pref Roaming */

#define OTASP_PROG_LOCK    0x03
 /* IS-683A SPL_P_REV */

#define OTASP_OTAPA        0x04
 /* OTAPA_P_REV */

#define OTASP_KEY_EXCHANGE_P_REV 0x02
#define OTASP_NAM_DOWNLOAD_P_REV 0x02

#define OTASP_NAM_DOWNLOAD_EHRPD_IMSI_P_REV 0x03
/* NAM Download with eHRPD IMSI provisioning */

#define OTASP_SPL_P_REV          0x01

#ifdef FEATURE_IS683C_PRL
  #define OTASP_PREF_ROAM_P_REV    0x03
#else
#define OTASP_PREF_ROAM_P_REV    0x01
#endif


#define OTASP_OTAPA_P_REV        0x01


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*           EXTERNAL TYPES USED FOR BIT-PACKED OTASP MESSAGES             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* For bit-representation (position and size of message fields) purposes,  */
/* each byte represents one bit.  External formats define the bit position */
/* and size of each field of the bit-packed OTASP message.  When the OTASP */
/* message is unpacked, internal message formats are used to represented   */
/* message data on byte-boundaries.  Bit-packing and bit-unpacking         */
/* routines utilize both definitions to translate OTASP message data to    */
/* and from bit-packed and byte-aligned formats.                           */

/* Type to isolate msg_type */
typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type    */
} otaspe_gen_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*            OTASP REVERSE LINK PARAMETER BLOCK TYPES                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CDMA/Analog NAM  Parameter Block (see IS-683 section 3.5.2.1)    */

typedef PACK (struct)
{
  byte firstchp[11];        /* First paging channel used in home system */
  byte home_sid[15];        /* Home system identification */
  byte ex[1];               /* Extended address indicator */
  byte scm[8];              /* Station class mark */
  byte mob_p_rev[8];        /* Mobile station protocol revision number */
  byte imsi_class[1];       /* IMSI Class assignment of the mobile station */
  byte imsi_addr_num[3];    /* Number of IMSI address digits */
  byte mcc[10];             /* Mobile country code */
  byte imsi_11_12[7];       /* 11th and 12th digits of IMSI */
  byte imsi_s[34];          /* The least significant ten digits of the IMSI */
  byte accolc[4];           /* Access overload class */
  byte local_control[1];    /* Local control status */
  byte mob_term_home[1];    /* Termination indicator for the home system */
  byte mob_term_for_sid[1]; /* Termination indicator for SID roaming */
  byte mob_term_for_nid[1]; /* Termination indicator for NID roaming */
  byte max_sid_nid[8];      /* Maximum stored SID/NID pairs */
  byte stored_sid_nid[8];   /* Number of stored SID/NID pairs */
} otaspe_rtc_cell_data_fix_type;

typedef PACK (struct)
{
  byte sid[15];    /* System identification  */
  byte nid[16];    /* Network identification */
} otaspe_sid_nid_data_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Mobile Directory Number (see IS-683 section 3.5.2.2)                    */

typedef PACK (struct)
{
  byte n_digits[4];   /* Number of digits */
} otaspe_mob_dir_data_fix_type;

typedef PACK (struct)
{
  byte digitn[4];  /* Digit */
} otaspe_mob_dir_data_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CDMA NAM Parameter Block (see IS-683 section 3.5.2.3)         */

typedef PACK (struct)
{
  byte reserved1[2];        /* Reserved bits */
  byte slotted_mode[1];     /* Slotted mode */
  byte reserved2[5];        /* Reserved bits */
  byte mob_p_rev[8];        /* Mobile station protocol revision number */
  byte imsi_class[1];       /* IMSI Class assignment of the mobile station */
  byte imsi_addr_num[3];    /* Number of IMSI address digits */
  byte mcc[10];             /* Mobile country code */
  byte imsi_11_12[7];       /* 11th and 12th digits of IMSI */
  byte imsi_s[34];          /* The least significant ten digits of the IMSI */
  byte accolc[4];           /* Access overload class */
  byte local_control[1];    /* Local control status */
  byte mob_term_home[1];    /* Termination indicator for the home system */
  byte mob_term_for_sid[1]; /* Termination indicator for SID roaming */
  byte mob_term_for_nid[1]; /* Termination indicator for NID roaming */
  byte max_sid_nid[8];      /* Maximum stored SID/NID pairs */
  byte stored_sid_nid[8];   /* Number of stored SID/NID pairs */
} otaspe_rtc_pcs_data_fix_type;

/* Note: Use otaspe_sid_nid_data_var_type for SID/NID pair data */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Preferred Roaming List Dimensions (as per Sprint proposal)              */

typedef PACK (struct)
{
  byte max_pr_list_size[16]; /* Max Preferred Roaming List Size */
  byte cur_pr_list_size[16]; /* Current Preferred Roaming List Size */
  byte num_acq_recs[6];      /* Number of Acquisition Records in current RL */
  byte num_sys_recs[8];      /* Number of System Records in current RL */
} otaspe_roam_siz_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Preferred Roaming List Dimensions (see IS-683A section 3.5.3.1)         */
typedef PACK (struct)
{
  byte max_pr_list_size[16]; /* Max Preferred Roaming List Size */
  byte cur_pr_list_size[16]; /* Current Preferred Roaming List Size */
  byte pr_list_id[16];       /* Pref Roam List Identification       */
  byte reserved[1];          /* Reserved bit                        */
  byte num_acq_recs[9];      /* Number of Acquisition Records in current RL */
  byte num_sys_recs[14];     /* Number of System Records in current RL */
} otaspe_rtc_roam_siz_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Extended Preferred Roaming List Dimensions (see IS-683C section 3.5.3.3)
*/
typedef PACK (struct)
{
  byte max_pr_list_size[16]; /* Max Preferred Roaming List Size */
  byte cur_pr_list_size[16]; /* Current Preferred Roaming List Size */
  byte pr_list_id[16];       /* Pref Roam List Identification       */
  byte cur_sspr_p_rev[8];    /* Protocol revision */
} otaspe_rtc_ext_roam_siz_fix_type;

typedef PACK (struct)
{
  byte reserved[1];          /* Reserved bit                        */
  byte num_acq_recs[9];      /* Number of Acquisition Records in current RL*/
  byte num_comm_subnet_recs[9];
                          /* Number of common subnet Records in current RL */
  byte num_sys_recs[14];     /* Number of System Records in current RL */
} otaspe_rtc_ext_roam_siz_var_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                REVERSE LINK OTASP MESSAGES                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CONFIGURATION RESPONSE MESSAGE (see IS-683 section 3.5.1.1)             */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type    */
  byte num_blocks[8];     /* Number of parameter blocks */
} otaspe_config_rsp_fix_type;

typedef PACK (struct)
{
  byte block_id[8];       /* Parameter block identifier */
  byte block_len[8];      /* Parameter block length     */
} otaspe_config_rsp_var_type;

typedef PACK (struct)
{
  byte result_code[8];
} otaspe_config_rsp_var2_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* DOWNLOAD RESPONSE MESSAGE (see IS-683 section 3.5.1.2)                  */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type    */
  byte num_blocks[8];     /* Number of parameter blocks */
} otaspe_download_rsp_fix_type;

typedef PACK (struct)
{
  byte block_id[8];       /* Parameter block identifier */
  byte result_code[8];    /* Download result code       */
} otaspe_download_rsp_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* MS KEY RESPONSE MESSAGE (see IS-683 section 3.5.1.3)                    */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type   */
  byte result_code[8];    /* Key exchange result code  */
} otaspe_ms_key_rsp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* KEY GENERATION RESPONSE MESSAGE (see IS-683 section 3.5.1.4)            */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type   */
  byte result_code[8];    /* Key exchange result code  */
  byte ms_result_len[8];  /* Length of MS_RESULT field */
} otaspe_key_gen_rsp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* RE-AUTHENTICATE RESPONSE MESSAGE (see IS-683 section 3.5.1.5)           */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type             */
  byte authr[18];         /* Authentication signature data       */
  byte randc[8];          /* Random challenge value              */
  byte count[6];          /* Call history parameter              */
  byte auth_data[24];     /* Authentication Data input parameter */
} otaspe_reauth_rsp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* COMMIT RESPONSE MESSAGE (see IS-683 section 3.5.1.6)                    */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type */
  byte result_code[8];    /* Data commit result code */
} otaspe_commit_rsp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROTOCOL CAPABILITY RESPONSE MESSAGE (see IS-683 section 3.5.1.7)       */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type                    */
  byte mob_firm_rev[16];  /* Mobile station firmware revision number    */
  byte mob_model[8];      /* Mobile station manufacturer's model number */
  byte num_features[8];   /* Number of features                         */
} otaspe_prot_cap_rsp_fix_type;

typedef PACK (struct)
{
  byte feature_id[8];     /* Feature identifier       */
  byte feature_p_rev[8];  /* Feature protocol version */
} otaspe_prot_cap_rsp_var_type;

typedef PACK (struct)
{
  byte add_length[8];     /* Additional length   */
} otaspe_prot_cap_rsp_var2_type;

typedef PACK (struct)
{
  byte addnl_byte[8];     /* Additional byte   */
} otaspe_prot_cap_rsp_fix2_type;

typedef PACK (struct)
{
  byte otasp_mob_p_rev[8]; /* OTASP mobile protocol revision              */
} otaspe_prot_cap_rsp_fix3_type;

typedef PACK (struct)
{
  byte num_cap_records[8];  /* number of capability records   */
  byte cap_record_type[8];  /* capability record type   */
  byte cap_record_length[8];  /* capability record length   */
  byte op_mode[8];          /* Type Specific Field, op_mode 8 bits */
  byte band_class[16];       /* Type Specific Field, band_calss 16 bits */
  byte meid[56];        /* Type Specific Field, max 56 bits for MEID */
} otaspe_prot_cap_rsp_var3_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SSPR CONFIGURATION  RESPONSE MESSAGE (see IS-683A section 3.5.1.8)       */
typedef PACK (struct)
{
   byte msg_type[8];       /* OTASP Data Message type            */
   byte block_id[8];       /* Parameter Block Identifier         */
   byte result_code[8];    /* Result Code                        */
   byte block_len[8];      /* Parameter Block Length             */
}  otaspe_sspr_cfg_rsp_fix_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SSPR DOWNLOAD  RESPONSE MESSAGE (see IS-683A section 3.5.1.9)       */
typedef PACK (struct)
{
   byte msg_type[8];       /* OTASP Data Message type            */
   byte block_id[8];       /* Parameter Block Identifier         */
   byte result_code[8];    /* Result Code                        */
   byte segment_offset[16];/* Segment offset                     */
   byte segment_size[8];   /* Segment size                       */
} otaspe_sspr_dload_rsp_fix_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* VALIDATION  RESPONSE MESSAGE         (see IS-683 section 3.5.1.10)      */
typedef PACK (struct)
{
   byte msg_type[8];       /* OTASP Data Message type            */
   byte num_blocks[8];     /* Number of parameter blocks         */
} otaspe_validn_rsp_fix_type;

typedef PACK (struct)
{
  byte block_id[8];       /* Parameter block identifier */
  byte result_code[8];    /* Download result code       */
} otaspe_validn_rsp_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* OTAPA  RESPONSE MESSAGE              (see IS-683 section 3.5.1.11)      */
typedef PACK (struct)
{
   byte msg_type[8];       /* OTASP Data Message type      */
   byte result_code[8];    /* Result Code                  */
   byte reserved[7];       /* Reserved                     */
   byte nam_lock_ind[1];    /* NAM LOCK identifier          */
}  otaspe_otapa_rsp_fix_type;

typedef PACK (struct)
{
   byte rand_otapa[32];    /* SPASM Random Challenge       */
} otaspe_otapa_rsp_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*            OTASP FORWARD LINK PARAMETER BLOCK TYPES                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CDMA/Analog NAM Parameter Block (see IS-683 section 4.5.2.1)    */

typedef PACK (struct)
{
  byte firstchp[11];        /* First paging channel used in home system */
  byte home_sid[15];        /* Home system identification */
  byte ex[1];               /* Extended address indicator */
  byte imsi_class[1];       /* IMSI Class assignment of the mobile station */
  byte imsi_addr_num[3];    /* Number of IMSI address digits */
  byte mcc[10];             /* Mobile country code */
  byte imsi_11_12[7];       /* 11th and 12th digits of IMSI */
  byte imsi_s[34];          /* The least significant ten digits of the IMSI */
  byte accolc[4];           /* Access overload class */
  byte local_control[1];    /* Local control status */
  byte mob_term_home[1];    /* Termination indicator for the home system */
  byte mob_term_for_sid[1]; /* Termination indicator for SID roaming */
  byte mob_term_for_nid[1]; /* Termination indicator for NID roaming */
  byte n_sid_nid[8];        /* Number of SID/NID pairs */
} otaspe_ftc_cell_data_fix_type;

/* Note: Use otaspe_sid_nid_data_var_type for SID/NID pair data */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Mobile Directory Number (see IS-683 section 4.5.2.2)                    */

/* Note: Use otaspe_mob_dir_data_fix_type and otaspe_mob_dir_data_var_type
   for Mobile Directory Number data */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CDMA NAM Parameter Block (see IS-683 section 4.5.2.3)                   */

typedef PACK (struct)
{
  byte imsi_class[1];       /* IMSI Class assignment of the mobile station */
  byte imsi_addr_num[3];    /* Number of IMSI address digits */
  byte mcc[10];             /* Mobile country code */
  byte imsi_11_12[7];       /* 11th and 12th digits of IMSI */
  byte imsi_s[34];          /* The least significant ten digits of the IMSI */
  byte accolc[4];           /* Access overload class */
  byte local_control[1];    /* Local control status */
  byte mob_term_home[1];    /* Termination indicator for the home system */
  byte mob_term_for_sid[1]; /* Termination indicator for SID roaming */
  byte mob_term_for_nid[1]; /* Termination indicator for NID roaming */
  byte n_sid_nid[8];        /* Number of SID/NID pairs */
} otaspe_ftc_pcs_data_fix_type;

/* Note: Use otaspe_sid_nid_data_var_type for SID/NID pair data */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* IMSI_T Parameter Block   (see IS-683A section 4.5.2.4)                  */
typedef PACK (struct)
{
  byte imsi_t_class[1];     /* IMSI_T Class assignment                     */
  byte imsi_t_addr_num[3];  /* Number of IMSI_T address digits             */
  byte mcc_t[10];           /* Mobile Country Code of the IMSI_T           */
  byte imsi_t_11_12[7];     /* 11th and 12th digits of the IMSI_T          */
  byte imsi_t_s[34];        /* The least significant 10 digits of IMSI_T   */
  byte reserved[1];         /* Reserved                                    */
} otaspe_imsi_t_fix_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* eHRPD IMSI Parameter Block   (see IS-683A section 4.5.2.5)              */
typedef PACK (struct)
{
  byte ehrpd_mnc_num_digits[1];   /* Number of Mobile Network Code digits */
  byte ehrpd_imsi_num_digits[4];  /* Number of eHRPD International Mobile Station Identity Digits */
  byte reserved[3];               /* Reserved  */
} otaspe_ehrpd_imsi_fix_type;

typedef PACK (struct)
{
  byte ehrpd_imsi_digits[4];      /* eHRPD International Mobile Station Identity Digits */
  byte reserved[1];               /* 0 to 4 reserved bits as needed */
} otaspe_ehrpd_imsi_digits_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Preferred Roaming List Parameter Block (as per Sprint's proposal)       */

typedef PACK (struct)
{
  byte reserved[4];         /* Keeps variable BITS field byte alligned */
  byte seq_num[8];          /* Sequence Number for fragmented RL download */
  byte num_bits[12];        /* Number of bits of Roaming List in block */
} otaspe_ftc_roam_lst_fix_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Preferred Roaming List Parameter Block (see IS-683A 4.5.3.1)            */
typedef PACK (struct)
{
   byte reserved[7];              /* Reserved                 */
   byte last_segment[1];          /* Last segment indicator   */
   byte segment_offset[16];       /* Segment offset           */
   byte segment_size[8];          /* Maximum segment size     */
} otaspe_sspr_prl_fix_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Verify Service Programming Code Block      (see IS-683A 4.5.4.1)        */
typedef PACK (struct)
{
  byte bcd_digit[4];        /* Each SPC digit is encoded in BCD */
} otaspe_ftc_verify_spc_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Change Service Programming Code Block       (see IS-683A 4.5.4.2)       */
typedef PACK (struct)
{
   byte bcd_digit[4];        /* Each SPC digit is encoded in BCD */
} otaspe_ftc_change_spc_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Validate SPASM  Block                       (see IS-683A 4.5.4.3)       */
typedef PACK (struct)
{
   byte auth_otapa_incl[1];
} otaspe_ftc_valid_spasm_fix_type;

typedef PACK (struct)
{
   byte auth_otapa[18];
} otaspe_ftc_valid_spasm_var_type;

typedef PACK (struct)
{
   byte nam_lock[1];
} otaspe_ftc_valid_spasm_fix2_typ;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Verify Service Programming Code        (as per Sprint's proposal)       */
typedef PACK (struct)
{
  byte bcd_digit[4];        /* Each SPC digit is encoded in BCD */
} otaspe_ftc_spc_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                FORWARD LINK OTASP MESSAGES                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CONFIGURATION REQUEST MESSAGE (see IS-683 section 4.5.1.1)              */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type              */
  byte num_blocks[8];     /* Number of parameter blocks requested */
} otaspe_config_req_fix_type;

typedef PACK (struct)
{
  byte block_id[8];       /* Parameter block identifier */
} otaspe_config_req_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* DOWNLOAD REQUEST MESSAGE (see IS-683 section 4.5.1.2)                   */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type    */
  byte num_blocks[8];     /* Number of parameter blocks */
} otaspe_download_req_fix_type;

typedef PACK (struct)
{
  byte block_id[8];       /* Parameter block identifier */
  byte block_len[8];      /* Parameter block length     */
} otaspe_download_req_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* COMMIT REQUEST MESSAGE (see IS-683 section 4.5.1.3)                     */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type */
} otaspe_commit_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* RE-AUTHENTICATE REQUEST MESSAGE (see IS-683 section 4.5.1.4)            */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type */
  byte rand[32];          /* Random Challenge value  */
} otaspe_reauth_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* MS KEY REQUEST MESSAGE (see IS-683 section 4.5.1.5)                     */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type       */
  byte a_key_p_rev[8];    /* Key Exchange protocol version */
  byte param_p_len[8];    /* Length of PARAM_P field       */
} otaspe_ms_key_req_fix1_type;

typedef PACK (struct)
{
  byte param_g_len[8];    /* Length of PARAM_G field       */
} otaspe_ms_key_req_fix2_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* KEY GENERATION REQUEST MESSAGE (see IS-683 section 4.5.1.6)             */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type   */
  byte bs_result_len[8];  /* Length of BS_RESULT field */
} otaspe_key_gen_req_fix_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROTOCOL CAPABILITY REQUEST MESSAGE (see IS-683 section 4.5.1.7)        */

typedef PACK (struct)
{
  byte msg_type[8];       /* OTASP Data Message type   */
  byte otasp_p_rev[8];  /* OTASP Mob protocol revision   */
  byte num_cap_records[8];  /* number of capability records   */
  byte cap_record_type[8];  /* capability record type   */
} otaspe_prot_cap_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SSPR CONFIGURATION REQUEST MESSAGE (see IS-683A section 4.5.1.8)        */
typedef PACK (struct)
{
   byte msg_type[8];       /* OTASP Data Message type    */
   byte block_id[8];       /* Parameter block identifier */
} otaspe_sspr_cfg_req_fix_type;

typedef PACK (struct)
{
  byte req_offset[16];     /* Segment offset             */
  byte req_max_size[8];    /* Maximum segment size       */
} otaspe_sspr_cfg_req_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SSPR DOWNLOAD REQUEST MESSAGE (see IS-683A section 4.5.1.9)             */
typedef PACK (struct)
{
   byte msg_type[8];       /* OTASP Data Message type    */
   byte block_id[8];       /* Parameter block identifier */
   byte block_len[8];      /* Block length               */
} otaspe_sspr_dload_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* VALIDATION REQUEST MESSAGE    (see IS-683A section 4.5.1.10)            */
typedef PACK (struct)
{
   byte msg_type[8];       /* OTASP Data Message type    */
   byte num_blocks[8];     /* Number of blocks           */
} otaspe_validation_req_fix_type;

typedef PACK (struct)
{
  byte block_id[8];        /* Parameter block identifier */
  byte block_len[8];       /* Parameter block length     */
} otaspe_validation_req_var_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* OTAPA REQUEST MESSAGE         (see IS-683A section 4.5.1.11)            */
typedef PACK (struct)
{
   byte msg_type[8];       /* OTASP Data Message type            */
   byte start_stop[1];     /* OTAPA session start_stop indicator */
   byte reserved[7];       /* reserved                           */
} otaspe_otapa_req_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#endif /* OTASPE_H */
