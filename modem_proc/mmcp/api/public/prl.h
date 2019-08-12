#ifndef PRL_H
#define PRL_H

/*===========================================================================

        P R E F F E R E D   R O A M I N G   L I S T   H E A D E R

DESCRIPTION
  This file contains Preferred Roaming List declarations for targets that
  support SD 2.0.

Copyright (c) 1996-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/prl.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/10/13   vs      Adding LTE band 27
09/30/13   sm      Adding LTE band 31  
07/24/13   vs      Adding LTE band 30
02/11/13   vs      Adding LTE band 28 and 29
08/24/12   fj      Adding LTE band 26.
08/10/12   fj      Adding LTE band 23
01/20/12   sg      Adding TD-SCDMA RAT into default PRL    
11/02/11   fj      Add support for validate and write BST/TOT into EFS.
06/18/10   xs      Add SENTINEL metacomments for maximum value sentinel in the enum 
03/19/10   aj      Add support for priority list info in GWL acq rec
02/11/10   fj      Added support for MCC-MNC system record.
01/28/10   mp      EF_EPRL Implementation.
01/11/10   fj      Added supoort for GWL-GENERIC acquisition record.
10/30/09   pm      Add define to fix KW error
09/19/09   sv      Define max CDMA/HDR/GW Channel and band constants
09/10/09   rm      Removing inclusion of customer.h from /api files
03/09/09   rm      Removing FEATURE_HTORPC_METACOMMENTS
02/26/09   rm      Removing private header files form public
02/16/08   ks      Porting SD code to support QDSP 6 based compiler
07/30/08   rm      CMI defeaturisation
09/02/08   sv      Add band channel denifitions
08/12/08   fj      Added new API prl_commit2()
04/04/08   ak      Fixed valid channel range for BC0 as per 
                   3GPP2 C.P0057-C v0.1
12/19/07   ka      Changes to support 16k PRL
11/29/07   ak      Standardized channel validation and updated channel
                   lowest and highest values as per standard.
08/21/07   jqi/cl  Removed the cell/pcs band mapping features.
04/16/07   rp      Adding RPC support for prl_validate
02/12/06   jqi     Externalized prl_get_ptr() API;
01/04/07   jqi     Provide the utility APIs for user to change the acquisition
                   table of PRL
10/10/06   jqi     Added CDMA450 BLOCK L support.
04/14/06   jqi     Doubled the GSM/UMTS PRL table size.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
01/12/05   pk      Modified prli_acq_rec_type, added is_valid flag
                   Added PRL_INVALID_UNIQUE_SYS to prl_validation_enum_type
12/02/05   jqi     Lint fixes for lint ver8.
11/18/05   pk      Added support for sub-blocks I,J,K for BC 5 and BC 11
05/04/05   jqi     Added support for new Japan BC0 subclass2 and 3.
11/30/04   jqi     Externalize the API prl_hdr_extract().
09/03/04   RC      Added prl_get_total_size() to return total size of PRL.
06/04/04   RC      Added support for FFEATURE_2ND_PRL
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/19/04   jqi     Lint fix update.
03/08/04   jqi     Lint fix.
10/31/03   JQ      Added support for latest IS-683D format changes.
10/09/03   jqi     Adjusted band preference enum.
09/22/03   SJ      Added max enum to sel_mode_e_type.
08/18/03   RC      Fixed issues with IS-683D implementation.
07/15/03   JQ      Added Extended PRL support
                   - D PRL
                   - Removed FEATURE_IS683C_PRL
05/06/03   JQ      Added IS683B PRL support.
01/21/03   RC      Relaxed channel checking to include invalid channels.
01/16/03   SJ      Removed use of DB for SD20.
07/18/02   RC      Added prl_map_acq_rec_to_sd_mode and prl_sys_rec_get_sid.
05/28/02   az      Added support for new korean channels
05/02/02   RC      Added FEATURE_NO_CELL_GUARD_BANDS to eliminate cellular
                   guard bands checking.
03/14/02   RC      More PRL format changes to support IS-683C/SSPR_P_REV=3.
03/05/02   RC      PRL format changes to support IS-683C/SSPR_P_REV=3.
01/10/02   jqi     For band class 5 support in SD 1.0
                   1.added definitions of PRL_BLOCK_G and PRL_BLOCK_H.
                   2.redefined PRL_LOWEST_PCS_CHANNEL and PRL_HIGHEST_PCS_CHANNEL
                     if either FEATURE_BAND_CLASS_5 or FEATURE_KPCS was defined.
12/11/01   sj      Made mcsyspr_map_roam_ind_to_tsb58() extern.
12/07/01   jqi     For band class 5 support in SD 2.0
                   1. added definitions of PRL_BLOCK_G and PRL_BLOCK_H.
                   2. redefined PRL_LOWEST_PCS_CHANNEL and PRL_HIGHEST_PCS_CHANNEL
                      if either FEATURE_BAND_CLASS_5 or FEATURE_KPCS was defined.
12/03/01   RC      Changed 311 to 333 in Korean PCS frequency range check.
07/20/01   RC      Added support for IS-683C.
07/02/01   ht      Added defines for Cellular Sys chan upper and lower
                   limits and macros to check for in-range channel.
02/09/01   RC      Created new prl.h from old prl.h and prli.h for SD 2.0.
06/29/00   vn      Added function prototype mcsyspr_multiple_geo() - CR13283
03/16/00   vn      Merged fix for CR7346.
03/09/00   vn      Merged Wildcard SID feature.
02/08/99   ht      Fix Lint warnings.
01/26/99   dna     Change GET_GEO_FOR() to not depend on signed value.
                   Use bpackw/bunpackw for 9 bit acq_index.
01/26/99   ht      Add #defines for Fast PRL support.
01/25/99   br      Packed Unions for ARM Support.
12/18/98   pms     Moved prl_reformat() prototype from mcsyspr.h to here.
09/01/98   lcc     Merged in ARM support.
08/28/98   ht      Add macros and definitions for Fast PRL Searching
08/06/98   ych     Fixed Lint warnings
08/04/98   ych     Merged E911, SSPR-800, and Acq. Improvements. Also added
                   SSPR CDMA available GAM processing support.
07/22/98   abh     Merged E911, SSPR-800, Acq. Improvements.
02/24/98   dna     New type for pri_sec and new enum for cell CDMA w/chans
11/05/97   abh     Deleted definitions for MAX/MIN Cellular Band CDMA
                   channels
01/20/97   jrs     Added PRL_ prefix to identifiers. Moved internal typedefs
                   to prli.h.
11/13/96   jrs     Updates for cellular analog.
06/24/96   jrs     Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"   /* Definition for basic types and macros */
#include "nv.h"       /* Interface to NV services */

/* Macro to check whether a CDMA band-class 0 channel is in the A side.
*/

  #define PRL_IS_CHAN_IN_CELLULAR_SYS_A( chan ) \
      ( ((chan) >= 1      &&  (chan) <= 333)  || \
        ((chan) >= 667    &&  (chan) <= 716)  || \
        ((chan) >= 991    &&  (chan) <= 1023) )

/* Macro to check whether a CDMA band-class 0 channel is in the B side.
*/

  #define PRL_IS_CHAN_IN_CELLULAR_SYS_B( chan ) \
      ( ((chan) >= 334    && (chan) <= 666) || \
        ((chan) >= 717    && (chan) <= 799) )
#include "sd.h"       /* External interface to sd.c */

/* Default PRL version number to be used for default PRL's. Note the a PRL
** that is read from NV might use this value as well.
*/
#define PRL_DEFAULT_VER 0
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration of PRL SSPR_P_REV
*/
typedef enum
{
  PRL_SSPR_P_REV_INVALID = 0,
  PRL_SSPR_P_REV_1 = 1,              /* PRL */
  PRL_SSPR_P_REV_3 = 3,              /* Extended PRL */
  PRL_SSPR_P_REV_RESERVED = 4
} prl_sspr_p_rev_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration of PRL validation results.
*/
typedef enum
{
  PRL_INVALID_CRC = 0,         /* Invalid CRC  */
  PRL_INVALID_NUM_OF_RECORDS,  /* Invalid number of records */
  PRL_INVALID_RECORD,          /* Invalid record */
  PRL_INVALID_SIZE,            /* Invalid size of the roaming list */
  PRL_VALID,                   /* Valid PRL */
  PRL_INVALID_SSPR_P_REV,      /* Invalid format revision (SSPR_P_REV) */
  PRL_INVALID_UNIQUE_SYS,      /* Invalid unique Mode, band and channel combinations */
  PRL_INVALID_SD_NOT_READY,
  PRL_VALIDATE_MAX_TYPE,
} prl_validation_enum_type;
/*~ SENTINEL prl_validation_enum_type.PRL_VALIDATE_MAX_TYPE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration of TOT validation and write to EFS results.
*/
typedef enum
{
  PRL_TOT_VALID_WRITE_NONE = 0,
  PRL_TOT_INVALID,
  PRL_TOT_INVALID_NUM_OF_RATS,     /* Invalid num of RATs  */
  PRL_TOT_INVALID_ACQ_SYS_MODE,    /* Invalid ACQ_SYS_MODE */
  PRL_TOT_VALID,                   /* Valid TOT */
  PRL_TOT_VALID_WRITTEN_ERR,       /* Valid TOT, but written failed */
  PRL_TOT_VALID_WRITTEN_SUCCESS,   /* Valid TOT, and wrtten successfully */
  PRL_TOT_VALIDATE_MAX_TYPE
} prl_tot_validation_write_enum_type;
/*~ SENTINEL prl_tot_validation_write_enum_type.PRL_TOT_VALIDATE_MAX_TYPE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration of BST validation and write to EFS results.
*/
typedef enum
{
  PRL_BST_VALID_WRITE_NONE = 0,
  PRL_BST_INVALID,                 /* Invalid BST  */
  PRL_BST_VALID_WRITTEN_ERR,       /* Valid TOT, but written failed */
  PRL_BST_VALID_WRITTEN_SUCCESS,   /* Valid TOT, and wrtten successfully */
  PRL_BST_VALIDATE_MAX_TYPE
} prl_bst_validation_write_enum_type;
/*~ SENTINEL prl_bst_validation_write_enum_type.PRL_BST_VALIDATE_MAX_TYPE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration of PRL commit results.
*/
typedef enum
{
  PRL_COMMIT_STATUS_NONE = -1,
  PRL_COMMIT_STATUS_DONE = 0,
  PRL_COMMIT_STATUS_NV_FAIL,
  PRL_COMMIT_STATUS_PRL_INVALID
} prl_commit_status_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type lists the types of acquisition records
*/
typedef enum
{
  PRL_ACQ_REC_RESERVED,                 /* Reserved */
  PRL_ACQ_REC_CELLULAR_ANALOG,          /* Cellular analog */
  PRL_ACQ_REC_CELLULAR_CDMA_STD,
  PRL_ACQ_REC_CELLULAR_CDMA_CUST,
  PRL_ACQ_REC_CELLULAR_GENERIC,         /* Cellular generic */
  PRL_ACQ_REC_PCS_CDMA_BLOCKS,          /* PCS CDMA using blocks */
  PRL_ACQ_REC_PCS_CDMA_CHANNELS,        /* PCS CDMA using channels */

  /* The following three ACQ REC types apply for IS683B PRL or above */
  PRL_ACQ_REC_JTACS_CDMA_STD,           /* JTACS CDMA standard channels */
  PRL_ACQ_REC_JTACS_CDMA_CUST,          /* JTACS CDMA custom channels */
  PRL_ACQ_REC_CDMA_2GHZ_CHANS,          /* 2G BAND CDMA using channels */

  /* The following three ACQ REC types apply for IS683C PRL or above */
  PRL_ACQ_REC_CDMA_GENERIC,             /* CDMA Generic */
  PRL_ACQ_REC_HDR_GENERIC,              /* HDR Generic */

  /* The following three ACQ REC types apply for IS683D PRL or above */
  PRL_ACQ_REC_GSM_GENERIC,              /* GSM Generic*/
  PRL_ACQ_REC_WCDMA_GENERIC,            /* WCDMA Generic*/
  PRL_ACQ_REC_GW_PREFERRED,             /* GSM/WCDMA perferred */

  /* GWL acquisition type, applicable for IS683E or above */
  PRL_ACQ_REC_GWL_GENERIC,
  
  PRL_ACQ_REC_NONE
} prl_acq_rec_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values that the a_b field can contain
*/
typedef enum
{
  PRL_AB_SYSTEM_A,            /* System A */
  PRL_AB_SYSTEM_B,            /* System B */
  PRL_AB_RESERVED,            /* Reserved */
  PRL_AB_SYSTEM_A_OR_B        /* System A or system B */
} prl_ab_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values that the pri_sec field can contain
*/
typedef enum
{
  PRL_PRISEC_RESERVED=0,           /* Reserved */
  PRL_PRISEC_PRIMARY=1,            /* Primary Channel Only */
  PRL_PRISEC_SECONDARY=2,          /* Secondary Channel Only */
  PRL_PRISEC_PRI_OR_SEC=3          /* Primary or Secondary Channel */
} prl_pri_sec_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values the UARFCN_INCL field in the ACQ
** Record
*/
typedef enum
{
  PRL_UARFCN_NOT_INCL=0,              /* UARFCN Not included in ACQ Record */
  PRL_UARFCN_INCL=1,                  /* UARFCN Included in ACQ Record */
  PRL_UARFCN_INCL_RESERVED=2
} prl_uarfcn_incl_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values the GSM/WCDMA preferred system
** indicator in the ACQ Record
*/
typedef enum
{
  PRL_GW_PREF_SYS_IND_GSM=0,          /* GSM system preferred */
  PRL_GW_PREF_SYS_IND_WCDMA=1,        /* WCDMA system preferred */
  PRL_GW_PREF_SYS_IND_RESERVED=2
} prl_gw_pref_sys_ind_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular analog system acquisition record
*/
typedef struct
{
  prl_ab_type a_b;                      /* System A/B selection type */
} prli_cellular_analog_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular CDMA system acquisition record
*/
typedef struct
{
  prl_ab_type a_b;                      /* System A/B selection type */
  prl_pri_sec_type pri_sec;             /* Primary/secondary CDMA channel */
} prli_cellular_cdma_type;              /* selection type */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define PRL_MAX_CDMA_CUSTOM_CELLULAR_CHAN 32

typedef struct
{
  int num_chans;                                /* Number of channels */
  word chan[PRL_MAX_CDMA_CUSTOM_CELLULAR_CHAN];/* Channels */
} prli_cellular_cdma_custom_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular generic system acquisition record
*/
typedef struct
{
  prl_ab_type a_b;                      /* System A/B selection type */
} prli_cellular_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define PRL_MAX_CDMA_PCS_BLOCKS 7

/* The PCS CDMA system acquisition (using blocks) record
*/
typedef struct
{
  int num_blocks;                       /* Number of blocks */
  word block[PRL_MAX_CDMA_PCS_BLOCKS];  /* Blocks */
} prli_pcs_cdma_block_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define PRL_MAX_CDMA_PCS_CHAN 32

/* The PCS CDMA system acquisition (using channels) record
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[PRL_MAX_CDMA_PCS_CHAN];     /* Channels */
} prli_pcs_cdma_chan_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
** The following three acq record types apply for IS683B PRL or above.
**- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The JTACS CDMA system acquisition record (standard channel)
*/
typedef struct
{
  prl_ab_type a_b;                      /* System A/B selection type */
  prl_pri_sec_type pri_sec;             /* Primary/secondary CDMA channel */
} prli_jtacs_cdma_std_type;              /* selection type */

#define PRL_MAX_CDMA_JTACS_CHAN 32

/* The JTACS CDMA system acquisition record (customer channels)
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[PRL_MAX_CDMA_JTACS_CHAN];   /* Channels */
} prli_jtacs_cdma_cust_type;

#define PRL_MAX_CDMA_2GHZ_CHAN 32

/* The 2G BAND CDMA system acquisition record (using channels)
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[PRL_MAX_CDMA_2GHZ_CHAN];    /* Channels */
} prli_cdma_2ghz_chans_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define PRL_MAX_CDMA_GENERIC_REC 128

/* The CDMA generic acquisition record
*/
typedef struct
{
  int   num_recs;                         /* Number of records */
  word  rec[PRL_MAX_CDMA_GENERIC_REC];    /* Packed records - 5bit band-class
                                          ** plus 11bit Channel number */
} prli_cdma_generic_type;

#define PRL_MAX_HDR_GENERIC_REC 128

/* The HDR generic acquisition record
*/
typedef struct
{
  int   num_recs;                         /* Number of records */
  word  rec[PRL_MAX_HDR_GENERIC_REC];     /* Packed records - 5bit band-class
                                          ** plus 11bit Channel number */
} prli_hdr_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define PRL_MAX_GSM_GENERIC_BAND 32

/* The GSM generic acquisition record
*/
typedef struct
{
  int num_bands;                          /* Number of bands */
  byte band[PRL_MAX_GSM_GENERIC_BAND];    /* Bands - 5bit band-class*/

} prli_gsm_generic_type;

/* The GWL generic acquisition record
*/
typedef struct
{
  prl_uarfcn_incl_e_type uarfcn_incl;     /* UARFCN include */
  byte band;                              /* Bands -  5bit band-class */
  word uarfcn;                            /* Chans - 14bit UARFCN */
} prli_gwl_generic_rec_type;

#define PRL_MAX_GWL_GENERIC_BAND 44

typedef struct
{
  int num_bands;                                              /* Number of bands */
  prli_gwl_generic_rec_type rec[PRL_MAX_GWL_GENERIC_BAND];    /* WCDMA generic record */
} prli_gwl_generic_recs_type;

/* The GSM/WCDMA perferred acquisition record
*/
typedef struct
{
  prl_gw_pref_sys_ind_e_type pref_sys_ind;   /* Preferred system indicator */
  int num_gsm_bands;                         /* Number of GSM bands */
  int num_wcdma_bands;                       /* Number of WCDMA bands */
  byte gsm_band[PRL_MAX_GSM_GENERIC_BAND];   /* GSM bands - 5bit band-class*/
  prli_gwl_generic_rec_type wcdma_rec[PRL_MAX_GWL_GENERIC_BAND];
                                             /* WCDMA generic record */
}prli_gw_pref_type;


/* The GSM/WCDMA/LTE perferred acquisition record
*/
typedef struct
{
  byte                           num_of_rats;      
                                        /* Number of RATs (GSM/WCDMA/LTE) */
  sys_sys_mode_e_type            pri_lst[SYS_MAX_NUM_3GPP_RATS];
                            /* Determines the priority among the 3gpp RATS */
  prli_gwl_generic_recs_type     gsm_rec;       /* GSM generic record */
  prli_gwl_generic_recs_type     wcdma_rec;     /* WCDMA generic record */
  prli_gwl_generic_recs_type     lte_rec;       /* LTE generic record */
  prli_gwl_generic_recs_type     tds_rec;       /* TD-SCDMA generic record */

}prli_gwl_generic_type;

/* PLMN based system specific fields.
*/
#define PRL_MAX_PLMN_LAC_ARRAY 16

typedef struct
{
  sys_plmn_id_s_type        plmn_id;             /* PLMN ID use */

  word                      lac;                 /* LAC ID array*/

  /* Combined plmn_lac_incl field, plmn select mode (plmn lac not incl), and
  ** network selection mode (plmn included)
  */
  sd_net_select_mode_e_type net_select_mode;     /* network selection mode */

  byte                      num_lac;             /* Number of lac */

  word                      lac_a[PRL_MAX_PLMN_LAC_ARRAY]; /* lac array */
} prli_plmn_sys_type;

/* Macros used for MCC-MNC based system records
 */
#define SYS_REC_MCC_MNC_SUBTYPE_LENGTH  3
#define SYS_REC_MCC_MNC_RESERVED_LENGTH 4
#define SYS_REC_MCC_LEN                 12
#define SYS_REC_MNC_LEN                 12
#define SYS_REC_SID_LEN                 16
#define SYS_REC_NID_LEN                 16
#define SYS_REC_SID_NID_NUM_LENGTH      4
#define SYS_REC_SUBNET_ID_NUM_LENGTH    4
#define SYS_REC_SUBNET_LENGTH_LEN       8
#define SYS_REC_MAX_SUBNET_ID_LENGTH    16
                                        /* 128/8 bytes */
#define SYS_REC_MAX_SID_NID             16
#define SYS_REC_MAX_SUBNET_ID           16

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* MCC-MNC Subtype for MCC-MNC based system records.
*/
typedef enum
{
  PRL_MCC_MNC_SUBTYPE_NONE = -1,             
    /* bounds checking */
  PRL_MCC_MNC_SUBTYPE_ONLY = 0,              
    /* MCC MNC only */
  PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID = 1,      
    /* MCC MNC with multiple SID */
  PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID_NID = 2,  
    /* MCC MNC with multiple SID/NID*/
  PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID = 3, 
    /* MCC MNC with multiple subnet IDs */
  PRL_MCC_MNC_SUBTYPE_MAX                    
    /* For SD internal use only */
} prl_mcc_mnc_subtype_e_type;
/*~ SENTINEL prl_mcc_mnc_subtype_e_type.PRL_MCC_MNC_SUBTYPE_MAX */

/* SID NID info.
*/
typedef struct {

/* Number of sid-nid pairs */
uint8		   sid_nid_pair_len;

/* The system ID */
uint16		   sid[SYS_REC_MAX_SID_NID];

/* The network ID */
uint16		   nid[SYS_REC_MAX_SID_NID];

} prli_sid_nid_type;

/* Subnet ID info.
*/
typedef struct {

/* Number of subnet common */
uint8		   num_subnet_id;

/* The subnet common length */
uint8		   subnet_length[SYS_REC_MAX_SUBNET_ID];

/* The subnet common */
uint8		   subnet_id[SYS_REC_MAX_SUBNET_ID][SYS_REC_MAX_SUBNET_ID_LENGTH];

} prli_subnet_id_type;


/* MCC_MNC based system specific fields.
*/
typedef struct {

  /* With sid or with sid-nid */
  prl_mcc_mnc_subtype_e_type   mcc_mnc_subtype;

  /* MCC - Mobile County Code */
  sys_mcc_type           mcc;

  /* MNC - Mobile Network Code */
  sys_mnc_type           mnc;

  union
  {
    prli_sid_nid_type          sid_nid;
	prli_subnet_id_type        subnet_id;
  }info;
  
} prli_mcc_mnc_sys_type;

/* This structure defines the acquisition record type
*/
typedef struct
{
  prl_acq_rec_type acq_type;
  boolean          is_valid;
  union
  {
    prli_cellular_analog_type       cellular_analog;
    prli_cellular_cdma_type         cellular_cdma;
    prli_cellular_cdma_custom_type  cellular_cdma_cust;
    prli_cellular_generic_type      cellular_generic;
    prli_pcs_cdma_block_type        pcs_cdma_block;
    prli_pcs_cdma_chan_type         pcs_cdma_chan;

    /* The following three acq record types apply for IS683B PRL or above.*/
    prli_jtacs_cdma_std_type        jtacs_cdma_std;
    prli_jtacs_cdma_cust_type       jtacs_cdma_cust;
    prli_cdma_2ghz_chans_type       cdma_2ghz_chans;

    /* The following two acq record types apply for IS683C PRL or above. */
    prli_cdma_generic_type          cdma_generic;
    prli_hdr_generic_type           hdr_generic;

    /* The following two acq record types apply for IS683D PRL or above. */
    prli_gsm_generic_type           gsm_generic;
    prli_gwl_generic_recs_type      wcdma_generic;
    prli_gw_pref_type               gw_pref;
    prli_gwl_generic_type           gwl_generic;

  } type;
  prli_plmn_sys_type plmn;          /* PLMN information from system record
                                    ** only valide for gsm/wcdma related acq
                                    ** record
                                    */

} prli_acq_rec_type;
/*~ FIELD prli_acq_rec_type.type    DISC prli_acq_rec_type.acq_type */
/*~ CASE  PRL_ACQ_REC_CELLULAR_ANALOG    prli_acq_rec_type.type.cellular_analog */
/*~ CASE  PRL_ACQ_REC_CELLULAR_CDMA_STD  prli_acq_rec_type.type.cellular_cdma */
/*~ CASE  PRL_ACQ_REC_CELLULAR_CDMA_CUST prli_acq_rec_type.type.cellular_cdma_cust */
/*~ CASE  PRL_ACQ_REC_CELLULAR_GENERIC   prli_acq_rec_type.type.cellular_generic */
/*~ CASE  PRL_ACQ_REC_PCS_CDMA_BLOCKS    prli_acq_rec_type.type.pcs_cdma_block */
/*~ CASE  PRL_ACQ_REC_PCS_CDMA_CHANNELS  prli_acq_rec_type.type.pcs_cdma_chan */
/*~ CASE  PRL_ACQ_REC_JTACS_CDMA_STD     prli_acq_rec_type.type.jtacs_cdma_std */
/*~ CASE  PRL_ACQ_REC_JTACS_CDMA_CUST    prli_acq_rec_type.type.jtacs_cdma_cust */
/*~ CASE  PRL_ACQ_REC_CDMA_2GHZ_CHANS    prli_acq_rec_type.type.cdma_2ghz_chans */
/*~ CASE  PRL_ACQ_REC_CDMA_GENERIC       prli_acq_rec_type.type.cdma_generic */
/*~ CASE  PRL_ACQ_REC_HDR_GENERIC        prli_acq_rec_type.type.hdr_generic */
/*~ CASE  PRL_ACQ_REC_GSM_GENERIC        prli_acq_rec_type.type.gsm_generic */
/*~ CASE  PRL_ACQ_REC_WCDMA_GENERIC      prli_acq_rec_type.type.wcdma_generic */
/*~ CASE  PRL_ACQ_REC_GW_PREFERRED       prli_acq_rec_type.type.gw_pref */
/*~ CASE  PRL_ACQ_REC_GWL_GENERIC        prli_acq_rec_type.type.gwl_generic */
/*~ CASE  PRL_ACQ_REC_NONE               prli_acq_rec_type.type.void */
/*~ CASE  PRL_ACQ_REC_RESERVED           prli_acq_rec_type.type.void */

/*===========================================================================
========================== Data Types 3gpp BST  ============================
===========================================================================*/
#define SDMMSS_BST_MAX_LENGTH   40

/* Enumeration of RAT_ID in 3GPP BST table
*/
typedef enum
{
  SDMMSS_BST_RAT_ID_GSM,
  SDMMSS_BST_RAT_ID_WCDMA,
  SDMMSS_BST_RAT_ID_LTE,
  SDMMSS_BST_RAT_ID_TDS,
  SDMMSS_BST_RAT_ID_MAX
    /**< FOR INTERNAL USE ONLY! */
} sdmmss_bst_rat_id_e_type;

typedef struct
{
  sdmmss_bst_rat_id_e_type       mode;
    /**< Only GSM, WCDMA, and LTE are allowed. */
  prli_gwl_generic_recs_type     gwl_rec; 
} sd_bst_mode_band_s_type;

typedef struct
{
  boolean                        is_mcc_included;
  sys_mcc_type                   mcc;     
    /**< HEX MCC */
  byte                           num_mode;
  sd_bst_mode_band_s_type        band_tbl[SYS_MAX_NUM_3GPP_RATS];
}sd_bst_s_type;

typedef struct
{
  uint16                         length;   
    /**< NUM_BST_RECs */
  sd_bst_s_type*                  bst_tbl[SDMMSS_BST_MAX_LENGTH];
}sd_bst_tbl_s_type;



/* <EJECT> */
/*===========================================================================

FUNCTION prl_extract_acq_rec

DESCRIPTION
  Extract a specified acquisition record from the currently loaded PRL.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the acquisition record is successfully extracted. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   prl_extract_acq_rec(

        prli_acq_rec_type         *acq_rec,
            /* Pointer to where to build the acq record.
            */

        int                       index
            /* Index of the acquisition entry to extract.
            */
);
/*~ FUNCTION prl_extract_acq_rec */
/*~ PARAM OUT acq_rec POINTER */

/* <EJECT> */
/*===========================================================================

FUNCTION prl_validate

DESCRIPTION
  Validate a specified PRL.

  This function is expected to be called by clients such as OTASP or DIAG
  upon writing a new PRL into NV. The returned validation indication should
  be saved along with the PRL.

  System determination consults the validation indication when reading the
  PRL from NV at power-up or NAM change.

  The function return the PRL version number through the prl_ver_ptr
  parameter.

  The function update the PRL protocol revision through sspr_p_rev_ptr
  if is_full_validation is set to true and validation is succeeded with
  protocol revision other than the specified one.

    - If is_full_validation is set to true, validate with the specific PRL
      protocol revision first. If fail, try other protocl revisions and
      update protocol revision parameter.
    - If is_full_validation is set to false, only validate with the specific
      PRL protocol revision.

DEPENDENCIES
  None.

RETURN VALUE
  PRL_VALID if the roaming list is valid. Specific PRL_INVALID_XXX code
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  prl_validation_enum_type    prl_validate(

        byte                        *prl_ptr,
            /* Pointer to the PRL to validate */

        word                        *prl_ver_ptr,
            /* Pointer where to save the PRL version.*/

        prl_sspr_p_rev_e_type       *sspr_p_rev_ptr,
            /* Pointer to PRL protocol revision information. */

        boolean                     is_full_validation
);
/*~ FUNCTION prl_validate */
/*~ PARAM prl_ptr POINTER */
/*~ PARAM prl_ver_ptr POINTER */
/*~ PARAM sspr_p_rev_ptr POINTER */

/* <EJECT> */
/*===========================================================================

FUNCTION prl_commit2

DESCRIPTION
  Validate a specified PRL and commits it to NV if validation is successful.

  Wrapper function around prl_validate() that transfers the PRL locally then 
  calls the validation function.

DEPENDENCIES
  None.

RETURN VALUE
  SD error code corresponding to the error state:
  TRUE        - success
  FALSE       - PRL invalid

SIDE EFFECTS
  None.

===========================================================================*/
extern  prl_commit_status_e_type    prl_commit2(

        byte                       *prl,
            /* PRL to validate */

        dword                       size,
           /*  Size of PRL array in BYTES */

        sd_nam_e_type               curr_nam,
           /* The current NAM to write for */

        word                        *prl_ver_ptr,
            /* Where to save the PRL version.*/

        prl_sspr_p_rev_e_type       *sspr_p_rev_ptr,
            /* PRL protocol revision information. */
        
        boolean                     is_full_validation
);
/*~ FUNCTION prl_commit2 */
/*~ PARAM prl VARRAY LENGTH size */
/*~ PARAM curr_nam */
/*~ PARAM INOUT prl_ver_ptr POINTER */
/*~ PARAM INOUT sspr_p_rev_ptr POINTER */
/*~ PARAM is_full_validation */

/*===========================================================================

FUNCTION sdmmss_validate_and_write_bst()

DESCRIPTION
  Validate and encode the BST. If it's valid, write into EFS.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern prl_bst_validation_write_enum_type sdmmss_validate_and_write_bst( 
  const sd_bst_tbl_s_type *          bst_ptr
    /* pointer to BST info to be encoded.
    */
);
/*~ FUNCTION sdmmss_validate_and_write_bst */
/*~ PARAM IN bst_ptr POINTER */

/*===========================================================================

FUNCTION prl_update_acq_rec

DESCRIPTION
  Update the PRL acquisition table entry with the specified acqusition record.

DEPENDENCIES
  None.

RETURN VALUE
  True if acquisition record is updated successful.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                    prl_update_acq_rec(

       const prli_acq_rec_type    *acq_rec_ptr,
            /* The input of the acq record pointer to be updated.
            */
       int                        index
            /* The index of the acq table to be updated.
            */
);
/*~ FUNCTION prl_update_acq_rec */
/*~ PARAM acq_rec_ptr POINTER */

/*===========================================================================

FUNCTION prl_commit

DESCRIPTION
  Commit PRL in NV

DEPENDENCIES
  None.

RETURN VALUE
  True if PRL is valide.
  False if PRL is invalid

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                    prl_commit(void);

/*~ FUNCTION prl_commit */
/**===========================================================================

FUNCTION prl_update_from_mmgsdi

DESCRIPTION
  This function is used to copy the received PRL parameters and PRL data
  to the SD prl buffer.


DEPENDENCIES
  sd_init has to be called before this function call.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void   prl_update_from_mmgsdi
(
       const dword prl_size,
       /**< Size of the PRL in bytes
       */
       const word  prl_version,
       /**< version of the PRL
       */
       const byte  *session_read_prl
       /**< Pointer to the PRL buffer read through MMGSDI card
       */
);
/*~ FUNCTION prl_update_from_mmgsdi */
/*~ PARAM prl_size */
/*~ PARAM prl_version */
/*~ PARAM session_read_prl POINTER */

#endif /* PRL_H */
