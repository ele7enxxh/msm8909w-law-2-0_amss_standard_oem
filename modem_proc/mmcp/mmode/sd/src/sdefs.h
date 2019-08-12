#ifndef SDEFS_H
#define SDEFS_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

        S Y S T E M   S E L E C T I O N   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with SDEFS.C (i.e. the SD EFS components).

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdefs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/13   ar      changing sdefs_id_e_type enum for fields Not having the ELF path
04/19/13   dj      New EFS Item added for 1XCSFB_ECBM feature.
10/09/12   sr      Support  to derive MCC of an EvDo system from eqprl.
07/17/12   jh      Added LTE Available File BSR support
05/11/12   jh      Fix SDEFS regular files/item files read handling
05/17/11   rn      WCDMA channel locking feature
03/10/11   gm      Add QXDM NV Browser support for ECBM timer.
02/16/11   gm      Extend acquisition while in ECBM & sys loss
02/07/11   xs      Dynamic FEATURE_JCDMA control in EFS
05/12/10   aj      Add support to save equivalent PRL in EFS
03/19/10   aj      MMSS support
02/17/10   rn      Get UE_cap from EFS
02/09/10   jqi     ESS support.

===========================================================================*/


#include "sd.h"       /* External interface to SD */
#include "sd_v.h"
#include "sdi.h"      /* Internal interface to sd.c */

#include "comdef.h"   /* Definition for basic types and macros */

/*===========================================================================
=============================================================================
=============================================================================
================================== TYPES ====================================
=============================================================================
=============================================================================
===========================================================================*/

/* Maximum length of file name for EFS files  (including full path)- 
** to be increased if filepath length increases 
*/
#define SDEFS_FILENAME_MAX_LEN         70

#define   SD_IMSI_MCC              999
#define   SD_IMSI_MNC              99
#define   SD_MAX_SID_NID_LOCK      10
#define   SD_RAL_TABLE_SIZE   10


/* Filepaths - increase SDEFS_FILENAME_MAX_LEN if new path exceeds 
** the current value 
*/
#define SDEFS_MMODE_MRU_NAM_1_F_NAME    "/sd/mru001"
#define SDEFS_MMODE_MRU_NAM_2_F_NAME    "/sd/mru002"
#define SDEFS_MMSS_TIMER_F_NAME         "/sd/mmss_timer"
#define SDEFS_BST_TBL_F_NAME            "/sd/bst_tbl"
#define SDEFS_MMSS_EQPRL_F_NAME         "/sd/mmss_eqprl"
#define SDEFS_MMSS_IGNORE_UPLMN_F_NAME  "nv/item_files/modem/nas/ignore_uplmn"
#define SDEFS_EOOS_PARAM_F_NAME         "/sd/eoos_param"
#define SDEFS_SDSSSCR_TIMERS_F_NAME     "/nv/item_files/modem/mmode/sd/sdssscr_timers"
#define SDEFS_LTE_AVAIL_FILE_F_NAME     "/sd/lte_avail_file"
#define SDEFS_LTE_NOT_AVAIL_FILE_F_NAME "/sd/lte_not_avail_file"
#define SDEFS_LBSR_MCC_LIST_F_NAME      "/nv/item_files/modem/mmode/sd/loc_base_bsr_mcc_list"
#define SDEFS_NO_FILE                   "sd/no_file"
#define SDEFS_FEATURE_FLAG_CFG_F_NAME   "/nv/item_files/jcdma/jcdma_mode"
#define SDEFS_WCDMA_FREQ_LOCK_F_NAME    "/nv/item_files/wcdma/rrc/wcdma_rrc_freq_lock_item"
#define SDEFS_QMSS_ENABLED_F_NAME       "/nv/item_files/modem/mmode/qmss_enabled"
#define SDEFS_OPERATOR_NAME_F_NAME      "/nv/item_files/modem/mmode/operator_name"
#define SDEFS_1XCSFB_ECBM_F_NAME        "/nv/item_files/modem/mmode/sd/1xcsfb_ecbm_status"
#define SDEFS_FULL_SRV_REQ_EMERG_F_NAME "/nv/item_files/modem/mmode/sd/full_srv_req_in_emerg"
#define SDEFS_LOCK_SID_NID_F_NAME       "/nv/item_files/modem/mmode/lock_parameters"
#define SDEFS_CUSTOM_EMERG_INFO_F_NAME  "/nv/item_files/modem/mmode/custom_emerg_info"     
#define SDEFS_MAX_ACQ_DB_SCAN_TIME_F_NAME "/nv/item_files/modem/mmode/max_acq_db_scan_time"
#define SDEFS_T_ACQ_SCAN_F_NAME         "/nv/item_files/modem/mmode/t_acq_scan"
#define SDEFS_ACQ_SCAN_DURATION_F_NAME  "/nv/item_files/modem/mmode/acq_scan_duration"
#define SDEFS_SCAN_SCOPE_RULE_F_NAME    "/nv/item_files/modem/mmode/scan_scope_rule"
#define SDEFS_ENHANCED_F3_DBG_F_NAME    "/nv/item_files/modem/mmode/sd/enhanced_f3_dbg"
#define SDEFS_MANUAL_SEARCH_IN_WRLF_F_NAME "/nv/item_files/modem/mmode/sd/manual_search_in_wrlf"
#define SDEFS_CSG_AUTO_MODE_NAME        "/nv/item_files/modem/mmode/global_mode_csg"
#define SDEFS_EHRPD_OPT_REDIAL_F_NAME   "/nv/item_files/modem/mmode/eHRPD_opt_redial"
#define SDEFS_C2K_RESEL_SPLMN_F_NAME    "/nv/item_files/modem/mmode/sd/c2k_resel_splmn_supp"
#define SDEFS_BUFFER_INT_SRV_LOST_F_NAME   "/nv/item_files/modem/mmode/sd/buffer_int_srv_lost"
#define SDEFS_MANUAL_SRCH_BST_TBL_F_NAME "/sd/man_srch_bst_tbl"
#define SDEFS_1XCSFB_CALL_END_OPT_F_NAME "/nv/item_files/modem/mmode/sd/1xcsfb_call_end_opt"
#define SDEFS_RECENTLY_ACQUIRED_LIST_F_NAME   "/sd/ral"
#define SDEFS_EMERGENCY_RAT_ACQ_ORDER_F_NAME  "/sd/emergency_rat_acq_order"
#define SDEFS_EXTEND_LBSR_1x_ACTIVE_F_NAME    "/nv/item_files/modem/mmode/sd/extend_lbsr_if_1x_active"
#define SDEFS_ROAM_IND_UPDATE_IN_L_TO_1X_HO_F_NAME "/nv/item_files/modem/mmode/sd/roam_ind_update_in_l_to_1x_ho"
#define SDEFS_1XSIB8_SCAN_OPT_F_NAME    "/nv/item_files/modem/mmode/1xsib8_scan_opt"
#define SDEFS_EMERG_CALL_AFTER_LU_IN_LPM_F_NAME "/nv/item_files/modem/nas/emergency_call_after_lu_in_lpm"
#define SDEFS_EMERG_CALL_1X_SCAN_FIRST_F_NAME "/sd/emergency_call_1x_scan_first"
#define SDEFS_CUSTOM_VOWIFI_CONFIG_F_NAME "/nv/item_files/modem/mmode/wifi_config"



/* SD EFS Item files prefix */
#define SDEFS_ITEM_FILES_PREFIX "/nv/item_files/"

/* MMODE BAND PREF */
typedef struct{
sd_ss_band_pref_e_type          band_pref;
sd_ss_band_pref_e_type          lte_band_pref;
} sd_mmode_band_pref_s_type;

typedef PACKED struct PACKED_POST{

  uint8                            version;
  boolean                          is_ecbm_required;  
  boolean                          hold_concurrent_voicecall_during_emergency;  
  boolean                          is_alternate_redial_algorithm;  
  boolean                          is_ral_stored_in_efs;
  /* indicates if 1. recently Acqed list has to be stored in EFS across 
  ** operating modes, and power cycles. 2. band level granular system records
  ** for 3gpp based systems records have to be used for recently acqed lists.
  */
  uint8                           reserved[7];
}sd_custom_emerg_call_info_s_type;


/* This holds information regarding custom wifi
 * configuarations
 */
typedef PACKED struct PACKED_POST{

  uint8                   version;

  boolean                 is_wifi_only_mode_for_voice;
  /* if set to TRUE, indicates support of voice calls in wifi only mode */

  boolean                 is_e911_over_wifi;
  /* if set to TRUE indicates support of emergency calls over wifi and the
          corresponding silent redial requirements support.*/

  uint8                   wifi_cs_scan_timer;
  /* This field indicates the time for which call would be attempted
     on CS domain. If the call does not get an ok_to_orig in the above
     mentioned time, call would be re-attempted on wifi. Default value
     for this timer is 5sec */

  boolean                   wifi_e911_wwan_full_srv;
  /* this feature will trigger the scan sequence of 
       LTE -> full srv -> limited srv - >WLAN 
       */

  uint8                           reserved[6];
  /* Reserved Fields */

}sd_custom_vowifi_config_s_type;


/*SID NID Structure to be avoided*/
typedef struct 
{
  uint16                      sid;
  uint16                      nid;
  uint16                      mnc;
  uint16                      mcc;
} sdprl_sid_nid_pair_type;

typedef struct 
{ 
  byte                        nam;
  sdprl_sid_nid_pair_type     pair[SD_MAX_SID_NID_LOCK];
} sdprl_sid_nid_lock_type;

typedef PACKED struct PACKED_POST{

  uint8                            version;
  boolean                          is_extend_lbsr;
  /* indicates if 1. Check for on-going 1x activity.
  ** If 1x in TRAFFIC or ACCESS states, then extend 
  ** LTE BSR on HYBR_2 stack 
  */
  uint8                           reserved[7];
}sd_extend_lbsr_s_type;


/* Structure to hold the information from NV item, SDEFS_1XSIB8_SCAN_OPT */
typedef PACKED struct PACKED_POST{

  uint8                         version;
  /* Version number */

  boolean                       is_1xsib8_scan_enabled;
  /* The feature to scan 1x sys based on 1XSIB8 info - enabled or disabled */
  
  uint8                         num_loops;
  /* Number of times to scan the new sequence if feature is enabled */

  uint8                         reserved[8];
  /* reserved for future use */

}sd_1x_sib8_scan_s_type;


/* currently we use same structures for RAL as  that of  MRU 
** later if some additions are done for MRU structure and are not required in RAL
we can evaluate and change RAL structure.*/
typedef sd_mmode_mru_table_entry_u_type sd_mmode_ral_table_entry_u_type;

typedef sd_mmode_mru_table_s_type sd_mmode_ral_table_s_type;

/** EFS id */
typedef enum
{
  SDEFS_NONE = -1,          /**< Not in EFS id - internal use only */
  SDEFS_MRU_NAM_1,          /**< EFS id for MRU for NAM 1 */
  SDEFS_MRU_NAM_2,          /**< EFS id for MRU for NAM 2 */
  SDEFS_MMSS_TIMER,         /**< EFS id MMSS timer */
  SDEFS_BST_TBL,            /**< EFS BST table */
  SDEFS_MMSS_EQPRL,         /**< EFS id for Equivalent PRL created for MMSS */
  SDEFS_SDSSSCR_TIMERS,     /**< EFS id for storing Timers used SD Scripts*/
  SDEFS_LTE_AVAIL_FILE,     /**< EFS id for LTE Available File */
  SDEFS_LTE_NOT_AVAIL_FILE, /**< EFS id for LTE Not Available File */
  SDEFS_LBSR_MCC_LIST,      /**< EFS id for storing MCCs to enable DO-LTE LBSR*/
  /* Enter all EFS items that can be read and written to by SD above this
  */
  SDEFS_QMSS_ENABLED,       /**< EFS id for enabling QMSS/QRD Features */
  SDEFS_OPERATOR_NAME,      /**< EFS id for operator name */
  SDEFS_1XCSFB_ECBM,        /**< EFS id for 1X_CSFB_ECBM Feature */
  SDEFS_LOCK_SID_NID,       /**< EFS id for Lock SID NID */
  SDEFS_FULL_SRV_REQ_EMERG, /**< EFS id for FULL SRV REQ in emerge orig */
  SDEFS_MANUAL_SRCH_BST_TBL,/**< EFS id for manual search bst table */
  SDEFS_MANUAL_SEARCH_IN_WRLF, /**< EFS id for MANU SEARCH in WRLF */
  SDEFS_1XCSFB_CALL_END_OPT,   /**< EFS id for 1X_CSFB call end optimisation  Feature */
  SDEFS_BUFFER_INT_SRV_LOST, /**< EFS id for buffering no srv due to TRM failure*/
  SDEFS_RECENTLY_ACQUIRED_LIST, /* EFS id for RAL */
  SDEFS_EMERGENCY_RAT_ACQ_ORDER,
  SDEFS_EXTEND_LBSR_1x_ACTIVE,
  SDEFS_ROAM_IND_UPDATE_IN_L_TO_1X_HO, /*EFS for controlling the romaing ins updation during L to 1X HO */
  SDEFS_1XSIB8_SCAN_OPT,    /* EFS id for 1x scan based on SIB8 info feature*/
  SDEFS_ENHANCED_F3_DBG_RULE,  /* EFS id for enabling enhanced F3*/
  SDEFS_INT_MAX,            /**< Maximum number of SD EFS items (owned by SD) */
  /* Enter all EFS items that can be read but not written to by SD 
  ** (owned by other modules) below
  */
  SDEFS_MMSS_IGNORE_UPLMN,  /**< EFS ignore uplmn  */
  SDEFS_EOOS_PARAM,         /* EFS id for EOOS parameters */
  SDEFS_FEATURE_JCDMA,     /* EFS id for JCDMA feature */
  SDEFS_WCDMA_FREQ_LOCK,   /* EFS id for WCDMA FREQ LOCK status */
  SDEFS_CUSTOM_EMERG_INFO, /* EFS id for custom emerg info */
  SDEFS_SCAN_SCOPE_RULE,   /* EFS id for scan_scope_type */
  SDEFS_CSG_AUTO_MODE,    /*  EFS id for csg auto mode info */
  SDEFS_EHRPD_OPT_REDIAL, /* EFS id for new opt SR feature */
  SDEFS_C2K_RESEL_SPLMN,
  SDEFS_EMERG_CALL_AFTER_LU_IN_LPM,
  SDEFS_EMERG_CALL_1X_SCAN_FIRST, /* EFS ID for 1x scan first for China operators */
  SDEFS_CUSTOM_VOWIFI_CONFIG,
  SDEFS_MAX
} sdefs_id_e_type;



/*===========================================================================
=============================================================================
=============================================================================
============================== FUNCTIONS ====================================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION MCC_NV_read

DESCRIPTION
Read EFS item denoted by efs_id and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/

extern boolean                  MCC_NV_read(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to read. 
            */ 
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    *size_ptr
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
);

/*===========================================================================

FUNCTION sdefs_read

DESCRIPTION
Read EFS item denoted by efs_id and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern  boolean                  sdefs_read(

        sdefs_id_e_type           efs_id,
            /* passing the efs Id to read. 
            */ 
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS Data.
            */
        int32                    *size_ptr
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
);


/*===========================================================================

FUNCTION sdefs_write

DESCRIPTION
Write contents of data_ptr in EFS item denoted by efs_id

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if write is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern    boolean                  sdefs_write(

        sdefs_id_e_type           efs_id,
            /* passing the efs Id to read. 
            */ 
        const byte                *data_ptr,
            /* Pointer to a data buffer to write to WLAN MRU.
            */
        int32                     size
            /* Size of the data_ptr buffer. 
            */
);

/*===========================================================================

FUNCTION sdefs_write2

DESCRIPTION
Write contents of data_ptr in EFS item denoted by filename

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if write is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern    boolean                  sdefs_write2(

        const char                *filename,
            /* passing the filename to read. 
            */ 
        const byte                *data_ptr,
            /* Pointer to a data buffer to write to
            */
        int32                     size
            /* Size of the data_ptr buffer. 
            */
);

/*===========================================================================

FUNCTION sdefs_fs_read_per_subs

DESCRIPTION
Read EFS item denoted by efs_id and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/
extern boolean                  sdefs_fs_read_per_subs(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    *size_ptr,
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
        sys_modem_as_id_e_type   sub_id
            /* A different EFS file is read based on the sub_id 
            */
);

/*===========================================================================

FUNCTION sdefs_fs_write

DESCRIPTION
  Write contents of data_ptr to EFS item denoted by efs_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean                  sdefs_fs_write_per_subs(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to write.
            */
        const byte                  *data_ptr,
            /* Pointer to a data buffer to write to EFS.
            */
        int32                       size,
            /* Size of the data_ptr buffer.
            */
        sys_modem_as_id_e_type      sub_id
            /* A different EFS file is read based on the sub_id
            */
);

/*===========================================================================

FUNCTION sdefs_init

DESCRIPTION
  Initialize SD EFS items.
  
DEPENDENCIES
  None

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

extern void sdefs_init(void);

/*===========================================================================

FUNCTION sdefs_get

DESCRIPTION
Get EFS item denoted by efs_id and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/
extern      boolean                  sdefs_get(

        sdefs_id_e_type          efs_id,
            /* passing the efs Id to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    size);

/*===========================================================================

FUNCTION sdefs_get2

DESCRIPTION
Get EFS item denoted by filename and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful and size read matches the size passed in,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/
extern      boolean                  sdefs_get2(

        const char*              filename,
            /* passing the filename to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    size
            /* The  maximum number of bytes to be copied into the buffer
            ** pointed by data_ptr.
            */
);

/*===========================================================================

FUNCTION sdefs_get_per_subs

DESCRIPTION
  Get EFS item denoted by filename from given subscription and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  TRUE if read is successful and size read matches the size passed in,
  FALSE otherwise.

SIDE EFFECTS
  None.

==========================================================================*/
extern       boolean                  sdefs_get_per_subs(

        const char*              filename,
            /* passing the filename to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    size,
            /* The  maximum number of bytes to be copied into the buffer
            ** pointed by data_ptr.
            */
        sys_modem_as_id_e_type  asubs_id
            /* subscription to read the EFS file from */
);

#ifdef FEATURE_EOOS
#error code not present
#endif

/*===========================================================================

FUNCTION sd_wcdma_freq_lock_ptr

DESCRIPTION
  Return the pointer to WCDMA Freq Lock status


DEPENDENCIES
  None

RETURN VALUE
  pointer of the WCDMA Freq Lock status

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16* sdefs_wcdma_freq_lock_ptr(sys_modem_as_id_e_type asubs_id);

/*===========================================================================

FUNCTION sdefs_csg_auto_mode_bsr_disable

DESCRIPTION
  Returns true if bsr is disabled in NV


DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdefs_csg_auto_mode_bsr_disable(void);

/*===========================================================================

FUNCTION sdefs_nv_refresh

DESCRIPTION
  Initialize SD EFS items.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void sdefs_nv_refresh(void);


#endif

