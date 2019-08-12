#ifndef CMEFS_H
#define CMEFS_H
/*===========================================================================

                         C A L L   M A N A G E R

                       CM EFS  I N T E R F A C E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMEFS.C (i.e. the CM EFS components).

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmefs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/22/14   xs      hVOLTE SSAC support
03/06/14   xs      Per RAT SMS domain sel
10/07/13   jvo     Added CMEFS_ENS_SUB_FEATURES to cmefs_id_e_type
07/24/13   jh      cmefs_read to return fail for size 0 file and reorganize
                   the cmefs enum so only internal files to be created.
07/19/13   jvo     Remove all MDM/MSM Fusion features
03/26/12   fj      Add EFS item for RAT-based no srv/limited srv hold up time.
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/07/12   xs      Added PS supplemetary service support
10/09/12   sr      Support  to derive MCC of an EvDo system from eqprl.
09/12/12   skk     Do not send internal service lost or limited service during uptime.
06/05/12   sg      CM changes for SGLTE requirement 
03/23/12   gm      DIME integration fix: TMC deprecation and others.
11/30/11   jh      Added feature for dynamic rat acq order change
12/01/11   vk      PS voice domain selection migration from CNE to CM 
09/14/11   rk      NV items N_min_MO_call_soft_retry and N_maxSIB8 
09/02/11   rk      S102 less 1XCSFB
07/05/11   rk      Support for sending voice domain preference to NAS
05/18/11   rn      WCDMA Channel locking feature
05/10/11   sv      Make GSM Auxilary Speech value EFS configurable 
04/08/11   sv      Add EFS item for DRS bit value for Voice calls
03/24/11   cl      FUSION2 MMSS-Arbitration centralization rework
03/22/11   cl      Add GW-Throttle Mechanism during HDR/LTE Idle for FUSION2
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
02/22/11   rk      SMS only registation
02/16/11   xs      Added JCDMA EFS control
02/07/11   xs      Mainline FEATURE_JCDMA features: call throttle parameter need to be
                   created by user instead of CM
01/25/11   ak      Added support for periodic GWL scans in DO only coverage.
01/19/11   fj      Added IDM timer value in EFS.
12/14/10   xs      Added support for call throttle parameter configurable in EFS for JCDMA
09/15/10   rm      Initial revision

===========================================================================*/


#include "cm.h"       /* External interface to CM */
#include "cm_v.h"     /* Interface to local CM defintions */
#include "cmi.h"      /* Internal interface to cm.c */
#include "comdef.h"   /* Definition for basic types and macros */

#define C2K_SXLTE_OPTI_ARRAY_SIZE 50 /* Array size of 1xsxlte_optimization EFS */
#define C2K_SXLTE_OPTI_ARRAY_USED 4 /* Number of elements used in 1xsxlte_optimization array */

/*===========================================================================
=============================================================================
=============================================================================
================================== TYPES ====================================
=============================================================================
=============================================================================
===========================================================================*/

/* Features controlled by c2k_sxlte_opti_array, one feature per index 
** When you add a new feature you must:
** - increment C2K_SXLTE_OPTI_ARRAY_USED
** - update cmefs_init_c2k_sxlte_opti() to set default value
*/
typedef enum {

  C2K_SXLTE_OPTI_PWRUP_FROM_LAST_SUBMODE = 0,

  C2K_SXLTE_OPTI_PS_DETACH,
  
  C2K_OPTI_ESR_E911,
  
  C2K_OPTI_ESR_E911_3RD_PARTY

} c2k_sxlte_opti_e_type;

/** EFS id */
typedef enum {

  CMEFS_NONE = -1,
  /**< Not in EFS id - internal use only */


  CMEFS_USER_NET_SEL_MODE_PREF_NAM_1,
  /**< User network selection mode_pref for NAM - 1 */

  CMEFS_USER_NET_SEL_MODE_PREF_NAM_2,
  /**< User network selection mode_pref for NAM - 2 */

  CMEFS_USER_NET_SEL_MODE_PREF_NAM_3,
  /**< User network selection mode_pref for NAM - 3 */

  /* 3 */
  CMEFS_USER_NET_SEL_MODE_PREF_NAM_4,
  /**< User network selection mode_pref for NAM - 4 */

  CMEFS_HICPS_GWL_AVOIDANCE_PARAM,
  /**< Parameters for HICPS enhancement for GWL avoidance */

  CMEFS_SGLTE_DEVICE,
  /**< Parameters to identify SGLTE target */  

  CMEFS_IDM_TIMER_VALUE,
  /**< IDM timer */

  /* 7 */
  CMEFS_SRV_IND_TIMER_VALUES,
  /**< srv ind uptime timers */

  CMEFS_RAT_SRV_IND_TIMER_VALUES,
  /**< Rat-based srv ind uptime timers */

  CMEFS_MM_ARBITRATION_ENABLE,
  /**< Flag to enable/disable intermodem arbitration on SVLTE II */

  CMEFS_SOA_SVLTE_PLMN_ID_LIST,
  /**< Plmn ID list store the plmns which support SVLTE */

  /* 11 */
  CMEFS_SOA_CONFIG,
  /**< Configuration for CMSOA */
 
  CMEFS_JCDMA_CALL_THROTTLE_TIME,
  /**< JCDMA call throttle timer */

  CMEFS_JCDMA_CALL_THROTTLE_NUM,
  /**< JCDMA call throttle times */

  CMEFS_DOMAIN_SELECTION_BEGIN,

  CMEFS_SMS_ONLY,
  /**< Is CS registation is for SMS only */

  CMEFS_UE_USAGE_SETTING,
  /**< Voice centric or data centric */

  /* 17 */
  CMEFS_VOICE_DOMAIN_PREF,
  /**< Voice domain preference */

  CMEFS_LTE_DISABLE_DURATION,
  /**< Duration for how long to diable LTE */

  CMEFS_N_MIN_MO_CALL_SOFT_RETRY,
  /**< Min number of soft retry attempts */

  CMEFS_N_MAX_SIB8_READ_ATTEMPTS,
  /**< Max number of sib8 read attempts */ 

  /* 21 */
  CMEFS_SMS_DOMAIN_PREF,
  /**< SMS domainpreferance */

  CMEFS_RAT_ACQ_ORDER,
  /**< RAT Acquisition order */

  CMEFS_SMS_MANDATORY,
  /**< SMS mandatory */
    
  /* 24 */
  CMEFS_SUPP_SRV_DOMAIN_PREF,
  /**< PS supplementary service domain preference */

  CMEFS_QMSS_ENABLED,
  /** QRD Features Enabled Flag */

  CMEFS_OPERATOR_NAME, 
 /** Operator Name required for QRD Features */
  
  CMEFS_SMS_OVER_S102,
  /**< SMS Over S102 support */

  /* 28 */
  CMEFS_GET_NET_AUTO_MODE,
  /**< Support GET NET in Auto Mode */

  CMEFS_CUSTOM_EMERG_INFO,
  /**< Emergency call customization information*/

  CMEFS_CU_IMSI,
  /* China Unicom IMSI Information */

  CMEFS_CMCC_IMSI,
  /* China Unicom IMSI Information */

  /* 32 */
  CMEFS_IMSI_MCC,

  CMEFS_IMSI_MIN1,

  CMEFS_IMSI_MIN2,

  CMEFS_IMSI_11_12,
  
  CMEFS_MANU_CODE,

  /* 37 */
  CMEFS_MANU_NAME,

  CMEFS_DEV_MODEL,

  CMEFS_SW_VER,

  CMEFS_LOCAL_REG_IND,

  CMEFS_MID_SRVCC,

  CMEFS_LTE_DO_IRAT_DURATION,

  /* 43 */
  CMEFS_EXTEND_LTE_DISABLE_DURATION,
  /**< Extended LTE disable duration */

  CMEFS_CDMA_VOICE_CALL_COLLISION,

  CMEFS_3GPP2_PS_CALL_OPTIMIZATION,
  
  CMEFS_SMS_DOMAIN_PREF_LIST,
  
  CMEFS_CSG_AUTO_MODE,
  /**<Support CSG in Auto Mode */
  CMEFS_SSAC_HVOLTE,

  /* 49 */
  CMEFS_ALLOW_CSFB_UPON_IMS_REG,

  CMEFS_UE_BASED_CW,
  /*UE based call waiting setting*/
  CMEFS_ALLOW_SMS_IN_ECBM,
  /**<Support for allowing SMS in ECBM for Sprint */

  CMEFS_C2KSWITCH2SRLTE,
  /* Enable/disable CM support to switch to SRLTE mode to enable
  ** background BSR for LTE when camped on 1x
  */

  CMEFS_MMODE_1XSXLTE_OPTIMIZATION,
  /**< mmode optimization flags*/

  CMEFS_IMS_REG_STATUS_WAIT_TIMER,
  /**< IMS registration wait timer */

  CMEFS_CUSTOM_VOICE_SR_TIMER,
  /** EFS item to increase the silent redial timer  */

  CMEFS_CDMA_LESS_DEVICE,
  /** CDMA less device config, should not be written on the fly */

  CMEFS_ID_LTE_BANDPREF,
  /**< User selected LTE band pref */

  CMEFS_ID_LTE_BANDPREF_65_256,
  /**< User selected LTE band pref bands 65-256*/

  CMEFS_ID_TDS_BANDPREF,
  /**< User selected TD-SCDMA band pref */ 

  CMEFS_VOLTE_SR_CONTROL,
  /**< Volte SR Control */

  CMEFS_IS_PLMN_BLOCK_REQ_IN_LTE_ONLY_MODE,
  /**< Inidcates if PLMN blocking is needed in LTE only mode or not */
  
  CMEFS_SELF_ACTIVATION_SUPPORT,
  /** If CSIM is not activated then do not switch to SRLTE mode  */

  CMEFS_ALLOW_CS_TO_PS_EMERG_REDIAL,

  CMEFS_CDMA_HOLD_SRV_TIMER,
  /**< Timer to hold 1x service reported to clients */

  CMEFS_HOLD_HY2_OPER_FLAG_TIMER,
  /**< Timer to hold is_oper flag reported to clients */

  CMEFS_SKIP_GWT_SCAN_DURING_RLF,
  /** EFS item to skip GWT scan when VOLTE is active
      during RLF */

  CMEFS_ATTACH_COMPLETE_TIMER,
  /** ATTCH complete timer  */

  CMEFS_PS_SIGNALLING_WAIT_TIMER,
  /* At the end of last Packet call for GSM/WCDMA, we would
  ** wait for this time. This allows network to teardown signaling
  ** during this time. By introducing wait, we will avoid the
  ** race condition when a PDP CONTEXT ACTIVE request is sent
  ** while network is tearing down the PS signaling.
  */

  CMEFS_INT_MAX,
  /* Enter all EFS items that can be read but not written to by CM
  ** (owned by other modules) below
  */

  CMEFS_FEATURE_JCDMA,
  /**< JCDMA global config */

  CMEFS_WCDMA_FREQ_LOCK,
  /**< WCDMA channel locking */

  CMEFS_DUAL_RX_1XCSFB,
  /**< Dual Rx 1XCSFB */

  CMEFS_ENS_SUB_FEATURES,
  /**< ENS sub features, including RAT balancing support */

  CMEFS_MODE_DISABLED_IN_TSTS,
  /**<Disable LTE from multiomode sub
   ** when moving to tsts from dsds */

  CMEFS_DISABLE_GLOBAL_MODE,
  /* Disable UE's ability to move to global mode when full service is lost */

  /* !!IMPORTANT NOTE!! 
  ** If it's MMODE owned item, add the new EFS item above the CMEFS_INT_MAX
  ** If it's other module owned item, add the new EFS item above here 
  */
  CMEFS_SXLTE_TIMERS,
  /**< SXLTE timers*/

  CMEFS_SUPPORT_DUAL_RX_UE,
  /**< SXLTE timers*/

  CMEFS_MSIM_STANDBY_ESR_SUPPORT,
  /**< Indicates if ESR should be done in case of LTE sharing RF */

  CMEFS_1xSRVCC_CALLED_PARTY_NUM,
  /**< STN-SR Number for 1xSRVCC orig*/

  CMEFS_CUSTOM_VOWIFI_CONFIG,
  /**< Wifi configuration details*/

  CMEFS_ID_RPM_SYS_TIME,
  /*<System time till last power down>*/

  CMEFS_MTCSFB_CALL_REDIAL_OVER_1X,
  /**< MTCSFB call to be redialled over 1x*/

  CMEFS_LTE_3GPP_RELEASE_VER,
  /**< EFS file owned by LTE team*/

  CMEFS_IMS_ENABLE,
  /**< EFS file owned by IMS team*/

  CMEFS_QIPCALL_DOM_SEL_ENABLE,
  /**< EFS file owned by IMS team*/

  CMEFS_EMCALL_TO_NORMAL_VOICECALL,
  /*<Convert emergency call to normal voice call>*/

  CMEFS_PSM_CONFIG,
  /**< PSM configuration */

  CMEFS_PSM_CONFIG_EXT,
  /**< PSM configuration extension*/

  CMEFS_PSM_UIM_OPT,
  /**< PSM UIM optimization */

  CMEFS_REJECT_OIR_CLIR_INTERROGATION,
  /**< Reject OIR and CLIR Interrogation request*/

  CMEFS_LTE_RPM_INFO_2,
  /** LTE RPM configuration details */
  
  CMEFS_MAX
  /**< Maximum number of CM EFS items */
} cmefs_id_e_type;

typedef struct
{
  uint8 version;
  /* version number of the NV item */
  boolean opti[50];
  /* optimization flags for mmode 1xsxlte optimizations*/
} mmode_1xsxlte_optimization_s_type;


/*===========================================================================
=============================================================================
=============================================================================
============================== FUNCTIONS ====================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmefs_read

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

extern  boolean                  cmefs_read(

        cmefs_id_e_type           efs_id,
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

FUNCTION cmefs_write

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

extern    boolean                  cmefs_write(

        cmefs_id_e_type           efs_id,
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

FUNCTION cmefs_fs_write

DESCRIPTION
  Write contents of data_ptr to EFS item denoted by efs_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean                  cmefs_fs_write(

        cmefs_id_e_type          efs_id,
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

FUNCTION cmefs_init

DESCRIPTION
  Initialize CM EFS items.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void cmefs_init(void);

/*===========================================================================

FUNCTION cmefs_get_c2k_sxlte_opti

DESCRIPTION
  Check if the specified optimization is enabled

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the optimization is enabled
  FALSE otherwise

SIDE EFFECTS
  None

==========================================================================*/
boolean cmefs_get_c2k_sxlte_opti(c2k_sxlte_opti_e_type opti_index);

/*===========================================================================

FUNCTION cmefs_init_c2k_sxlte_opti

DESCRIPTION
  Read the efs item for 1XSXLTE optimization and set default value if read fails

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void cmefs_init_c2k_sxlte_opti();

/*===========================================================================

FUNCTION cmefs_get_efs_id_for_curr_nam

DESCRIPTION
  Returns the EFS id for the given NAM

DEPENDENCIES
  None

RETURN VALUE
  cmefs_id_e_type.

SIDE EFFECTS
  None.

===========================================================================*/
extern cmefs_id_e_type cmefs_get_efs_id_for_curr_nam (cm_nam_e_type);


/*===========================================================================

FUNCTION cm_is_jcdma_enable

DESCRIPTION
  If a specified feature is enabled return TRUE, otherwise return FALSE


DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE
SIDE EFFECTS
  None.

===========================================================================*/
boolean cm_is_jcdma_enable(void);


/*===========================================================================

FUNCTION cmefs_get_itc_aux_val

DESCRIPTION
  Get ALS ITC value to use

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

byte cmefs_get_itc_aux_val(void);


/*===========================================================================

FUNCTION cmefs_get

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
boolean                  cmefs_get(

        cmefs_id_e_type          efs_id,
            /* passing the efs Id to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    size,
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
        sys_modem_as_id_e_type  sub_id
            /* A different EFS file is read based on the sub_id
            */
);

/*===========================================================================

FUNCTION cmefs_get2

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
boolean                  cmefs_get2(

        const char*              filename,
            /* passing the filename to read.
            */
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    size,
            /* The  maximum number of bytes to be copied into the 
            ** buffer pointed by data_ptr.
            */
        sys_modem_as_id_e_type  sub_id
            /* A different EFS file is read based on the sub_id
            */
);


/*===========================================================================

FUNCTION cmefs_map_efs_id_to_filename

DESCRIPTION
  Map EFS id to EFS filename


DEPENDENCIES
  None

RETURN VALUE
  Return TRUE if mapping was successful, else FALSE

SIDE EFFECTS
  None.

===========================================================================*/

const char* const  cmefs_map_efs_id_to_filename(

        cmefs_id_e_type          efs_id
            /* passing the efs Id to map.
            */
);
/*===========================================================================

FUNCTION cmefs_refresh

DESCRIPTION
  Refresh NV items

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void cmefs_refresh(void);

/*===========================================================================

FUNCTION cmefs_fs_write_no_create

DESCRIPTION
  Write contents of data_ptr to EFS item denoted by efs_id with oflag = MCFG_FS_O_RDWR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean                  cmefs_fs_write_no_create(

        cmefs_id_e_type          efs_id,
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

#endif

