#ifndef SDMMSS_H
#define SDMMSS_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

               I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions used internally by the System
  Determination.

  This header file should NOT be included by any source file outside the
  System Determination module.

  Copyright (c) 2012 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdmmss.h#1 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------- 
10/18/12   aj       increase num sys loc tags supported
09/18/12   qf       FR2502 - 3GPP scan in MMSS operation
11/02/11   fj       Add support for validate and write BST/TOT into EFS.
07/24/11   aj       400 acq rec support merge to mainline
08/30/10   aj       update max supported size of MMSS files
06/14/10   gm       Fix to KW and Lint errors.
03/20/10   aj       Initial version.


===========================================================================*/
#include "mmcp_variation.h"


#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "sd.h"       /* External header file */
#include "sys_v.h"
#include "prl.h"
#include "mmgsdilib_common.h"


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================== COMMON ===================================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */


/*===========================================================================
================================= MACRO =====================================
===========================================================================*/

/* MLPL related defines
*/

#define SDMMSS_MAX_NUM_MLPL_RECS      256             /* 2^NUM_MLPL_RECS_LEN */  
#define SDMMSS_MAX_NUM_SYS_LOC_TAG    30              /* Number of loc tags per rec */
 
#define SDMMSS_LOC_PARAM_TYPE_MCC     SYS_BM(0)
#define SDMMSS_LOC_PARAM_TYPE_MNC     SYS_BM(1)
#define SDMMSS_LOC_PARAM_TYPE_LOC_TAG SYS_BM(2)

/* MSPL related defines
*/
#define SDMMSS_MAX_NUM_MSPL_RECS      21   /* 3 (HOME,PREF,ANY) * 7 (3GPP,3GPP2,L,G,W,C,H)*/
#define SDMMSS_MAX_NUM_MSPL_ID        64   /* 2^NUM_MSPL_ID_LEN*/

/* BST related defines
*/
#define SDMMSS_DECODE_SIZE_8_BITS     8 
#define SDMMSS_DECODE_SIZE_16_BITS    16 
#define SDMMSS_BST_TBL_MAX_SIZE       4096

/*===========================================================================
=============================== ENUMERATION =================================
===========================================================================*/

/**
** Enumeration of system types defined by IS 683 E standard 
*/

typedef enum
{
  SDMMSS_SYS_TYPE_3GPP2,
  SDMMSS_SYS_TYPE_3GPP,
  SDMMSS_SYS_TYPE_WIMAX,
  SDMMSS_SYS_TYPE_CDMA,
  SDMMSS_SYS_TYPE_HDR,
  SDMMSS_SYS_TYPE_GSM,
  SDMMSS_SYS_TYPE_WCDMA,
  SDMMSS_SYS_TYPE_LTE,
  SDMMSS_SYS_TYPE_TDS,
  SDMMSS_SYS_TYPE_MAX = 9
      /**< FOR INTERNAL USE ONLY!                             */
} sdmmss_sys_type_e_type;

/**
*  Enumeration of bit masks for each of the system types
*/

#define SDMMSS_SYS_TYPE_3GPP2_MASK        BM(SDMMSS_SYS_TYPE_3GPP2)
#define SDMMSS_SYS_TYPE_3GPP_MASK         BM(SDMMSS_SYS_TYPE_3GPP)
#define SDMMSS_SYS_TYPE_WIMAX_MASK        BM(SDMMSS_SYS_TYPE_WIMAX)
#define SDMMSS_SYS_TYPE_CDMA_MASK         BM(SDMMSS_SYS_TYPE_CDMA)
#define SDMMSS_SYS_TYPE_HDR_MASK          BM(SDMMSS_SYS_TYPE_HDR)
#define SDMMSS_SYS_TYPE_GSM_MASK          BM(SDMMSS_SYS_TYPE_GSM)
#define SDMMSS_SYS_TYPE_WCDMA_MASK        BM(SDMMSS_SYS_TYPE_WCDMA)
#define SDMMSS_SYS_TYPE_LTE_MASK          BM(SDMMSS_SYS_TYPE_LTE)

#define SDMMSS_SYS_TYPE_3GPP2_ALL_MASK    ( BM(SDMMSS_SYS_TYPE_3GPP2)|\
                                            BM(SDMMSS_SYS_TYPE_CDMA)|\
                                            BM(SDMMSS_SYS_TYPE_HDR))

#define SDMMSS_SYS_TYPE_3GPP_ALL_MASK     ( BM(SDMMSS_SYS_TYPE_3GPP)|\
                                            BM(SDMMSS_SYS_TYPE_GSM)|\
                                            BM(SDMMSS_SYS_TYPE_WCDMA)|\
                                            BM(SDMMSS_SYS_TYPE_LTE))

/* macros to decipher whether an access technology bit is set in the
** access technology bytes encoded in the plmn db files
*/
#define SDMMSS_GSM_ACT_BIT               0x80
#define SDMMSS_WCDMA_ACT_BIT             0x80
#define SDMMSS_LTE_ACT_BIT               0x40

/* Enumeration of Priority class which are
** defined in each MSPL record
*/
typedef enum
{
  SDMMSS_PRI_CLASS_HOME_ONLY,
  SDMMSS_PRI_CLASS_HOME_PREF,
  SDMMSS_PRI_CLASS_ANY = 15,
  SDMMSS_PRI_CLASS_MAX

} sdmmss_pri_class_e_type;

/* Enumeration of system priorities  which are
** defined in each MSPL record
*/
typedef enum
{
  SDMMSS_SYS_PRI_SAME = 0,
  SDMMSS_SYS_PRI_MORE = 1,
  SDMMSS_SYS_PRI_MAX

} sdmmss_sys_pri_e_type;

/* Enumeration of Higher Priority search timers which are
** defined in each MSPL record
*/
typedef enum
{
  SDMMSS_HP_SRCH_TIMER_NONE            = -1,
   /* internal only. for range checking
   */

  SDMMSS_HP_SRCH_TIMER_ONE_MIN         = 0,
   /* This indicates that higher priority searches have to be done
   ** every 1 minute 
   */
  SDMMSS_HP_SRCH_TIMER_TWO_MIN         = 1,
   /* This indicates that higher priority searches have to be done
   ** every 2 minutes 
   */
  SDMMSS_HP_SRCH_TIMER_FOUR_MIN        = 2,
   /* This indicates that higher priority searches have to be done
   ** every 4 minutes 
   */
  SDMMSS_HP_SRCH_TIMER_EIGHT_MIN       = 3,
   /* This indicates that higher priority searches have to be done
   ** every 8 minutes 
   */
  SDMMSS_HP_SRCH_TIMER_SIXTEEN_MIN     = 4,
   /* This indicates that higher priority searches have to be done
   ** every 16 minutes 
   */
  SDMMSS_HP_SRCH_TIMER_TWENTY_FOUR_MIN = 5,
   /* This indicates that higher priority searches have to be done
   ** every 24 minutes 
   */
  SDMMSS_HP_SRCH_TIMER_THIRTHY_TWO_MIN = 6,
   /* This indicates that higher priority searches have to be done
   ** every 32 minutes 
   */
  SDMMSS_HP_SRCH_TIMER_SIXTY_FOUR_MIN  = 7,
   /* This indicates that higher priority searches have to be done
   ** every 64 minutes
   */
  SDMMSS_HP_SRCH_TIMER_MAX 
   /* internal only. for range checking
   */

} sdmmss_hp_srch_timer_e_type;
 
/* Enumeration of network capability as specified in each MSPL record
*/
typedef enum
{
  SDMMSS_NETWORK_CAP_VOICE_DATA,
  SDMMSS_NETWORK_CAP_RESERVED 

} sdmmss_network_cap_e_type;

typedef enum
{
  SDMMSS_ERROR_NO_ERR,
  /* Errors encountered while populating the equivalent PRL
  */
  SDMMSS_ERROR_RECS_ALREADY_INSERTED,
  SDMMSS_ERROR_NO_RECS_FOUND,

  /* Errors encountered while decoding the MLPL
  */
  SDMMSS_ERROR_SDMMSS_MAX_NUM_SYS_LOC_TAG_EXCEED,
  SDMMSS_ERROR_MAX

} sdmmss_error_e_type;

/* Enumeration of MLPL match levels.
** Used when comparing MCC MNC extracted from PRL with MLPL records
** The order is very important and used for comparison. So please do 
** not change it without due consideration.
*/
typedef enum {

  SDMMSS_MLPL_MATCH_LVL_NONE,     /* Indicate that no match was found
                                */

  SDMMSS_MLPL_MATCH_LVL_MCC_ONLY, /* Indicate that only MCC was
                                ** matched */

  SDMMSS_MLPL_MATCH_LVL_MCC_WCARD_MNC,  
                                /* Indicate that only MCC was
                                ** matched. MNC is wcard in MLPL */

  SDMMSS_MLPL_MATCH_LVL_MCC_MNC,  /* Indicate that MCC and MNC were both
                                ** matched */


  SDMMSS_MLPL_MATCH_LVL_MAX       /* Internal use only
                                */

} sdmmss_mlpl_match_lvl_e_type;


/* Enumeration of different location grouping methods
*/
typedef enum
{
  SDMMSS_LOC_GRP_NONE    = -1,
  /* Internal only
  */

  SDMMSS_LOC_GRP_MCC_BASED,
  /* This implies that the PRL has mcc info and this is used to tie 3gpp and 
  ** 3gpp2 systems together
  */

  SDMMSS_LOC_GRP_SID_BASED,
  /* This implies that the MLPL has SID/SUBNET info and this is used to tie 
  ** 3gpp and 3gpp2 systems together
  */

} sdmmss_loc_grp_e_type;


/*===========================================================================
========================== Data Types for MLPL ==============================
===========================================================================*/



typedef struct
{
  word                 SID; 
  word                 NID_start; 
  byte                 NID_range;

} sdmmss_sys_loc_tag_cdma_s_type;

typedef struct
{
  sys_plmn_id_s_type   PLMN_start;
  byte                 PLMN_range;

} sdmmss_sys_loc_tag_gwl_s_type;

typedef struct
{
  byte                 subnet_common_length;
  byte                 subnet_common[SYS_REC_MAX_SUBNET_ID_LENGTH];
  
} sdmmss_sys_loc_tag_hrpd_s_type;

typedef union
{
  sdmmss_sys_loc_tag_cdma_s_type    sys_loc_tag_cdma;
  sdmmss_sys_loc_tag_gwl_s_type     sys_loc_tag_gwl;
  sdmmss_sys_loc_tag_hrpd_s_type    sys_loc_tag_hrpd;
} sdmmss_sys_loc_tag_u_type;

typedef struct
{
  sdmmss_sys_type_e_type        sys_type;
  byte                          sys_loc_tag_size;
  sdmmss_sys_loc_tag_u_type     sys_loc_tag;

} sdmmss_sys_loc_tag_s_type;

typedef struct
{
  sys_mcc_type                MCC;  
  sys_mnc_type                MNC;  
  byte                        num_sys_loc_tag;
  sdmmss_sys_loc_tag_s_type   *loc_tag;
} sdmmss_loc_param_val_s_type;

typedef struct
{
  byte                         loc_param_type;  
  /* bit 0 specifies if MCC is valid in loc param val
  ** bit 1 specifies if MNC is valid in loc param val
  ** bit 2 specifies if NUM_SYS_LOC_TAG is valid in loc param val
  */
  sdmmss_loc_param_val_s_type  loc_param_val;   

  byte                         mspl_index;

} sdmmss_mlpl_recs_s_type;

typedef struct
{
  uint16                   mlpl_size;
  uint8                    mmss_p_rev;
  uint16                   mlpl_ver_id;
  uint8                    mlpl_id;
  uint8                    num_mlpl_recs;
  sdmmss_mlpl_recs_s_type  *mlpl_recs;

} sdmmss_mlpl_s_type;

/*===========================================================================
========================== Data Types 3gpp BST  ============================
===========================================================================*/
typedef struct
{
  sdmmss_sys_type_e_type         mode;
  prli_gwl_generic_recs_type     gwl_rec; 
} sdmmss_bst_mode_band_s_type;

typedef struct
{
  sys_plmn_id_s_type             plmn;
  byte                           num_mode;
  sdmmss_bst_mode_band_s_type    band_tbl[SYS_MAX_NUM_3GPP_RATS];
}sdmmss_bst_s_type;

typedef struct
{
  uint16                         length;
  sdmmss_bst_s_type              bst_tbl[SDMMSS_BST_MAX_LENGTH];
}sdmmss_bst_tbl_s_type;

/* Enumeration of BST REC TYPE
*/
typedef enum
{
  SDMMSS_BST_REC_TYPE_1  = 1,
  /* For the current BST version, only this is valid BST REC TYPE
  */
  
  SDMMSS_BST_REC_TYPE_MAX
  /* Internal use only
  */
} sdmmss_bst_rec_type_e_type;


/* Enumeration of band_chan_REC_TYPE
*/
typedef enum
{
  SDMMSS_BAND_CHAN_REC_TYPE_1  = 1,
  /* For the current BST version, only this is valid BST REC TYPE
  */
  
  SDMMSS_BAND_CHAN_REC_TYPE_MAX
  /* Internal use only
  */
} sdmmss_band_chan_rec_type_e_type;

/*==========================================================================
========================== Data Types for MSPL ==============================
===========================================================================*/

typedef struct
{
  sdmmss_sys_type_e_type         sys_type;
  sdmmss_pri_class_e_type        pri_class;
  sdmmss_sys_pri_e_type          sys_pri;
  sdmmss_hp_srch_timer_e_type    hp_srch_time;
  boolean                        network_cap_ind;
  sdmmss_network_cap_e_type      network_cap;
} sdmmss_mspl_recs_s_type;

typedef struct
{
  uint8                      mspl_id;
  uint8                      num_mspl_recs;
  sdmmss_mspl_recs_s_type    mspl_recs[SDMMSS_MAX_NUM_MSPL_RECS];  
} sdmmss_mspl_tbl_s_type;

typedef struct
{
  uint16                   mspl_size;
  /* Size of the MSPL structure. 
  ** 7(header) + NUM_MSPL_ID * 2 + total_NUM_MSPL_RECS * 3
  */
  uint8                    cur_sdmmss_p_rev;
  uint16                   mspl_ver_id;
  uint8                    num_mspl_id;
  sdmmss_mspl_tbl_s_type   *mspl_tbl;
} sdmmss_mspl_s_type;


/*===========================================================================
=============== DATA STRUCTURES FOR MLPL DECODING ===========================
===========================================================================*/


typedef PACKED struct  {
  byte reserved[1];
  byte SID[15];
  byte NID_start[16];
  byte NID_range[8];
}sdmmssi_sys_loc_tag_cdma_s_type;


typedef PACKED struct {
  byte PLMN_start[24];
  byte PLMN_range[8];
}sdmmssi_sys_loc_tag_gwl_s_type ;

typedef PACKED struct {
  byte subnet_common_length[8];
}sdmmssi_sys_loc_tag_hrpd_s_type;

typedef PACKED struct {
  byte sys_type[8];
  byte sys_loc_tag_size[8];
}sdmmssi_sys_loc_tag_s_type;

typedef PACKED struct 
{
  byte MCC[12];
  byte MNC[12]; 
  byte num_sys_loc_tag[8];
}sdmmssi_loc_param_val_s_type;

typedef PACKED struct 
{
  byte loc_param_type[8];
  byte mspl_index[8];
} sdmmssi_mlpl_recs_s_type;

typedef PACKED struct {

  byte mlpl_size[16];
  byte cur_mmss_p_rev[8];
  byte mlpl_ver_id[16];
  byte mlpl_id[8];
  byte num_mlpl_recs[8];
}sdmmssi_mlpl_s_type;

/*===========================================================================
=============== DATA STRUCTURES FOR MSPL DECODING ===========================
===========================================================================*/

typedef PACKED struct {

    byte sys_type[8];
    byte pri_class[4];
    byte sys_pri[1];
    byte higher_pri_srch_time[3];
    byte network_cap_ind[1];
    byte network_cap[6];
    byte reserved[1];

}sdmmssi_mspl_recs_s_type;

typedef PACKED struct  {
    byte mspl_size[16];
    byte cur_mmss_p_rev[8];
    byte mspl_ver_id[16];
    byte num_mspl_id[8];
    byte mspl_id[8];
    byte num_mspl_recs[8];
}sdmmssi_mspl_s_type;



/*===========================================================================
=========================== 3GPP BST External APIs ========================
===========================================================================*/

extern sdmmss_bst_tbl_s_type  *sdmmss_bst_ptr(

  boolean   is_reset
   /* Resets the BST buffer */

);


/*===========================================================================
=========================== Misc External APIs ==============================
===========================================================================*/


/*===========================================================================
=========================== MMOC file db interface  ========================
===========================================================================*/
typedef enum
{
  SDMMSS_PLMN_FILE_TYPE_IMSI,
  SDMMSS_PLMN_FILE_TYPE_WACT,
  SDMMSS_PLMN_FILE_TYPE_NO_ACT,
  SDMMSS_PLMN_FILE_TYPE_MAX

} sdmmss_plmn_file_type_e_type;

typedef struct {

  byte *                imsi;
    /* SIM/USIM - IMSI  (used to form the HOME plmn list in SIM and used in 
    ** usim if ehplmn is absent) 
    */
  byte *                ad;
    /* SIM/USIM - admin data (used to form the HPLMN from IMSI) 
    */
  byte *                usim_ehplmn;
    /* USIM - EHPLMN   (used to form the HOME plmn list) 
    */
  uint32                usim_ehplmn_size;
    /* USIM - size of ehPLMN file  
    */
  byte *                oplmnwact;
    /* SIM/USIM - OPLMN   (used to form the pref plmn list) 
    */
  uint32                oplmnwact_size;
    /* size of oplmnwact file  
    */
  byte *                plmnwact;
    /* SIM/USIM - UPLMN   (used to form the pref plmn list) 
    */
  uint32                plmnwact_size;
    /* size of plmnwact file  
    */
  byte *                gsm_plmn;
    /* SIM - PLMN file (used to form the pref plmn list if oplmn and
    ** uplmn files are not present in sim) 
    */
  uint32                gsm_plmn_size;
    /* SIM - size of PLMN file  
    */
}sdmmss_plmndb_s_type;


typedef struct {

  byte *                mspl;
    /* MSPL - system priority list - encoded form 
    */
  byte *                mlpl;
    /* MLPL - location priority list - encoded form 
    */

  mmgsdi_app_enum_type  app_type;
    /* app type - whether sim or usim 
    */

  sdmmss_plmndb_s_type    plmndb;
  /* structure  of plmn db files
  */

}sdmmss_file_s_type;


/*===========================================================================
=========================== FILE DB External APIs ==============================
===========================================================================*/

/*===========================================================================

FUNCTION sdmmss_validate_and_write_manual_srch_bst()

DESCRIPTION
  Validate and encode the BST. If it's valid, write into EFS.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern prl_bst_validation_write_enum_type sdmmss_validate_and_write_manual_srch_bst( 
  const sd_bst_tbl_s_type *          bst_ptr
    /* pointer to BST info to be encoded.
    */
);

/*===========================================================================

FUNCTION sdmmss_update_filedb

DESCRIPTION
  MMOC calls this API to update SDMMSS module with the files it has 
  read.

DEPENDENCIES


RETURN VALUE 


SIDE EFFECTS
  None.

===========================================================================*/
extern void         sdmmss_update_filedb(

       mmgsdi_file_enum_type       file_name,
       /* The name of the file being updated 
       */

       const  byte  *              data_buf,
       /* The contents of the file
       */

       uint32                      data_size
       /* The size of the file 
       */

);

/*===========================================================================
=============================================================================
=============================================================================
============================= @FUNCTION DECLARATION ==========================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION sdmmss_create_equivalent_prl

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  True if equivalent PRL is created successfully.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                   sdmmss_create_equivalent_prl( void );
/*===========================================================================

FUNCTION sdmmss_init

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  Initializes the sdmmss component

SIDE EFFECTS
  None.

===========================================================================*/
extern void                   sdmmss_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION sdmmss_update_tot_list 

DESCRIPTION
 Populate default bst entry into SDSR_TOT_LST

DEPENDENCIES
 

RETURN VALUE 
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void         sdmmss_update_tot_list ( void );

/*===========================================================================

FUNCTION sdmmss_bst_ptr

DESCRIPTION
  Return a pointer to local cache of 3GPP band support table


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern sdmmss_bst_tbl_s_type *sdmmss_bst_ptr(

       boolean  is_reset
        /* Reset the buffer Pointer
        */
);

/*===========================================================================

FUNCTION sdmmss_extract_bst()

DESCRIPTION
  Read and decode the 3GPP band support table from EFS. 


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdmmss_extract_bst( void );

/*===========================================================================

FUNCTION sdmmss_update_mmss_gwl_list

DESCRIPTION
 Populate default bst entry into SDSR_MMSS_GWL_LST.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void         sdmmss_update_mmss_gwl_list ( void );

/*===========================================================================

FUNCTION sdmmss_create_default_bst_entry

DESCRIPTION
 Check if there exists an entry for which mcc is not defined.  If such entry does not exist,
 create a default bst entry.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void         sdmmss_create_default_bst_entry ( void );


#endif /* SDMMSS_H */
