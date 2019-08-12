/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C A L L   M A N A G E R

                       CM EFS  I N T E R F A C E

GENERAL DESCRIPTION
  This file makes up the EFS-Interface component of the CM.

  The CM EFS-Interface component encapsulates all the functionality that is
  associated with reading and writing from/to EFS CM related items.

EXTERNALIZED FUNCTIONS
  cmefs_write
  cmefs_read

REGIONAL FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmefs_init() must be call to initialize this component before any other
  function declared in this component is called.


Copyright (c) 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmefs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/28/14   xs      Change ue_based_cw to NV
04/22/14   xs      hVOLTE SSAC support
03/06/14   xs      Per RAT SMS domain sel
11/13/13   xs      Add control for VOLTE SR LTE failure
11/13/13   aj/jvo  Added custom_emerg_info entry to mmode_config_items
10/07/13   jvo     Added UIM path, ENS subfeatures path for cmefs_tbl
08/13/13   sm      Adding EFS for MidCall Srvcc support
07/24/13   jh      cmefs_read to return fail for size 0 file and reorganize
                   the cmefs enum so only internal files to be created.
07/19/13   jvo     Remove all MDM/MSM Fusion features
04/19/13   dj      New EFS Item added for 1XCSFB_ECBM feature.
15/04/13   nv      Adding EFS Item to MMODE CONF File
03/26/12   fj      Add EFS item for RAT-based no srv/limited srv hold up time.
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/07/12   xs      Added PS supplemetary service support
10/09/12   sr      Support  to derive MCC of an EvDo system from eqprl.
09/17/12   skk     Do not send internal service lost or limited service during
                   uptime.
08/17/12   gm      Mainline EFS feature - Remove FEATURE_EFS
06/05/12   sg      CM changes for SGLTE requirement
03/23/12   gm      DIME integration fix: TMC deprecation and others.
16/02/12   vk      Replace usage of AEE library function with corresponding
                   CoreBSP library.
11/30/11   jh      Added feature for dynamic rat acq order change
12/19/11   ak      Enable GWL avoidance by default.
12/01/11   vk      PS voice domain selection migration from CNE to CM
10/20/11   rk      Adding config file creation support for EFS items
09/14/11   rk      NV items N_min_MO_call_soft_retry and N_maxSIB8
09/02/11   rk      S102 less 1XCSFB
08/25/11   xs      Added EPS network feature support definition (PS voice
                   indication)
07/05/11   rk      Support for sending voice domain preference to NAS
05/18/11   cl      Added support for GW throttle filtering based on regions.
05/18/11   rn      WCDMA Channel locking feature
05/10/11   sv      Make GSM Auxilary Speech value EFS configurable
05/10/11   skp     Using std_strcat in cmefs_map_efs_id_to_filename().
04/19/11   sv      Write default voice DRS bit value
04/11/11   am      Cleaning stubs and runnning qtf SUTE
04/08/11   sv      Add EFS item for DRS bit value for Voice calls
03/22/11   cl      Add GW-Throttle Mechanism during HDR/LTE Idle for FUSION2
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
03/02/11   rk      Domain selection phase 2 requirements
02/28/11   rk      Adding support for LTE UE mode preference
02/22/11   rk      SMS only registation
02/16/11   xs      Added JCDMA EFS control
02/07/11   xs      Mainline FEATURE_JCDMA features: call throttle parameter
                   need to be created by user instead of CM
01/25/11   ak      Added support for periodic GWL scans in DO only coverage.
01/19/11   fj      Added support for IDM timer value in EFS.
12/17/10   xs      Added support for call throttle parameter configurable in
                   EFS for JCDMA
12/16/10   am      Including MMODE unit test interceptors
09/15/10   rm      Initial revision
10/28/10   gm      Fix to compilation warning.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "cm.h"       /* External interface to cm.c */
#include "cm_v.h"     /* Interface to local CM defintions */
#include "cmdbg.h"    /* CM debug macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "cmefs.h"    /* CM EFS interface */
#ifdef CM_GW_SUPPORTED
#include "mm.h"
#endif
#include "fs_errno.h"
#include "mcfg_fs.h"    /* Interface to MCFG efs wrappers */
#include <string.h>
#include <stringl/stringl.h>
#if defined(CM_DEBUG) && !defined(FEATURE_MMODE_QTF)
#error code not present
#endif

#include "sd_v.h"

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
#include "cmsoa.h"
#endif

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

#include "cmpsm.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


#define CMEFS_MMODE_CONF_F_NAME "/nv/item_files/conf/mmode.conf"

/* Structure for EFS table
*/
typedef struct cmefs_tbl_s
{
   cmefs_id_e_type            efs_id;       /* EFS id */
   const char* const          file_name;    /* EFS file name */
} cmefs_tbl_s_type;

/* Filepaths
*/

#define CMEFS_PH_DIR        "/mmode/cmph/"
#define CMEFS_SS_DIR        "/mmode/cmss/"
#define CMEFS_CALL_DIR      "/mmode/cmcall/"
#define CMEFS_MM_DIR        "/mmode/cmmm/"
#define CMEFS_NV_MMODE_DIR  "/nv/item_files/modem/mmode/"
#define CMEFS_UIM_DIR       "/nv/item_files/modem/uim/"

/*
** EFS table that maps EFS Id to filename
*/
static const cmefs_tbl_s_type cmefs_tbl[] =
{
  { CMEFS_USER_NET_SEL_MODE_PREF_NAM_1, CMEFS_PH_DIR "user_net_sel_mode_nam_1" },
  { CMEFS_USER_NET_SEL_MODE_PREF_NAM_2, CMEFS_PH_DIR "user_net_sel_mode_nam_2" },
  { CMEFS_USER_NET_SEL_MODE_PREF_NAM_3, CMEFS_PH_DIR "user_net_sel_mode_nam_3" },
  { CMEFS_USER_NET_SEL_MODE_PREF_NAM_4, CMEFS_PH_DIR "user_net_sel_mode_nam_4" },
  { CMEFS_HICPS_GWL_AVOIDANCE_PARAM,    CMEFS_PH_DIR "hicps_gwl_avoidance_param" },
  { CMEFS_SGLTE_DEVICE,                 CMEFS_PH_DIR "sglte_device"},
  { CMEFS_SOA_SVLTE_PLMN_ID_LIST,       CMEFS_PH_DIR "svlte_plmn_id_list"},
  { CMEFS_SOA_CONFIG,                   CMEFS_PH_DIR "soa_config"},
  { CMEFS_MMODE_1XSXLTE_OPTIMIZATION,   CMEFS_NV_MMODE_DIR "mmode_1xsxlte_optimization"},
  { CMEFS_SXLTE_TIMERS,                 CMEFS_NV_MMODE_DIR "sxlte_timers"},
  { CMEFS_C2KSWITCH2SRLTE,              CMEFS_NV_MMODE_DIR "c2k_switch_2_srlte"},

  { CMEFS_IDM_TIMER_VALUE,              CMEFS_SS_DIR "idm_timer_value" },
  { CMEFS_SRV_IND_TIMER_VALUES,         CMEFS_SS_DIR "srv_ind_timer_values"},

  { CMEFS_JCDMA_CALL_THROTTLE_TIME,     CMEFS_CALL_DIR "jcdma_call_throttle_time"},
  { CMEFS_JCDMA_CALL_THROTTLE_NUM,      CMEFS_CALL_DIR "jcdma_call_throttle_num"},
  { CMEFS_CDMA_VOICE_CALL_COLLISION,    CMEFS_CALL_DIR "cdma_voice_call_collision" },

  { CMEFS_MM_ARBITRATION_ENABLE,        CMEFS_MM_DIR "is_mm_arbitration_enabled" },

  { CMEFS_SMS_ONLY,                     CMEFS_NV_MMODE_DIR "sms_only" },
  { CMEFS_UE_USAGE_SETTING,             CMEFS_NV_MMODE_DIR "ue_usage_setting" },
  { CMEFS_VOICE_DOMAIN_PREF,            CMEFS_NV_MMODE_DIR "voice_domain_pref" },
  { CMEFS_LTE_DISABLE_DURATION,         CMEFS_NV_MMODE_DIR "lte_disable_duration" },
  { CMEFS_N_MIN_MO_CALL_SOFT_RETRY,     CMEFS_NV_MMODE_DIR "n_min_MO_call_soft_retry" },
  { CMEFS_N_MAX_SIB8_READ_ATTEMPTS,     CMEFS_NV_MMODE_DIR "n_maxSIB8" },
  { CMEFS_SMS_DOMAIN_PREF,              CMEFS_NV_MMODE_DIR "sms_domain_pref"},
  { CMEFS_SMS_MANDATORY,                CMEFS_NV_MMODE_DIR "sms_mandatory"},
  { CMEFS_SUPP_SRV_DOMAIN_PREF,         CMEFS_NV_MMODE_DIR "supplement_service_domain_pref"},
  { CMEFS_SMS_OVER_S102,                CMEFS_NV_MMODE_DIR "sms_over_s102"},
  { CMEFS_GET_NET_AUTO_MODE,            CMEFS_NV_MMODE_DIR "get_net_auto_mode"},
  { CMEFS_CSG_AUTO_MODE,                CMEFS_NV_MMODE_DIR "global_mode_csg"},
  { CMEFS_CUSTOM_EMERG_INFO,            CMEFS_NV_MMODE_DIR "custom_emerg_info"  },
  { CMEFS_LTE_DO_IRAT_DURATION,         CMEFS_NV_MMODE_DIR "lte_do_irat_duration"},
  { CMEFS_3GPP2_PS_CALL_OPTIMIZATION,   CMEFS_NV_MMODE_DIR "3gpp2_ps_call_optimization"},
  { CMEFS_RAT_SRV_IND_TIMER_VALUES,     CMEFS_NV_MMODE_DIR "rat_srv_ind_timer_values"},



  { CMEFS_FEATURE_JCDMA,                "/nv/item_files/jcdma/jcdma_mode" },

  { CMEFS_RAT_ACQ_ORDER,                "/sd/rat_acq_order" },
  { CMEFS_WCDMA_FREQ_LOCK,              "/nv/item_files/wcdma/rrc/wcdma_rrc_freq_lock_item" },
  { CMEFS_DUAL_RX_1XCSFB,               "/nv/item_files/modem/lte/rrc/cap/dual_rx_1x_csfb" },
  /* CMEFS_SUPPORT_DUAL_RX_UE is for 1xsrlte */
  { CMEFS_SUPPORT_DUAL_RX_UE,            "/nv/item_files/modem/mmode/supportfordualrxues" },
  { CMEFS_QMSS_ENABLED,                 CMEFS_NV_MMODE_DIR "qmss_enabled"},
  { CMEFS_OPERATOR_NAME,                CMEFS_NV_MMODE_DIR "operator_name"},
  { CMEFS_SRV_IND_TIMER_VALUES,         "srv_ind_timer_values"},
  { CMEFS_CU_IMSI,                      CMEFS_NV_MMODE_DIR "cu_imsi"},
  { CMEFS_CMCC_IMSI,                    CMEFS_NV_MMODE_DIR "cmcc_imsi"},
  { CMEFS_IMSI_MCC,                     CMEFS_NV_MMODE_DIR "imsi_mcc"},
  { CMEFS_IMSI_MIN1,                    CMEFS_NV_MMODE_DIR "imsi_min1"},
  { CMEFS_IMSI_MIN2,                    CMEFS_NV_MMODE_DIR "imsi_min2"},
  { CMEFS_IMSI_11_12,                   CMEFS_NV_MMODE_DIR "imsi_11_12"},
  { CMEFS_MANU_CODE,                    CMEFS_NV_MMODE_DIR "manufacturer_code"},
  { CMEFS_MANU_NAME,                    CMEFS_NV_MMODE_DIR "manufacturer_name"},
  { CMEFS_DEV_MODEL,                    CMEFS_NV_MMODE_DIR "device_model"},
  { CMEFS_SW_VER,                       CMEFS_NV_MMODE_DIR "sw_version"},
  { CMEFS_LOCAL_REG_IND,                CMEFS_NV_MMODE_DIR "reg_status"},
  { CMEFS_MID_SRVCC,                    CMEFS_NV_MMODE_DIR "mid_call_srvcc_info"},
  { CMEFS_ENS_SUB_FEATURES,             CMEFS_UIM_DIR "mmgsdi/ens_sub_features_status_list"},
  { CMEFS_EXTEND_LTE_DISABLE_DURATION,  CMEFS_NV_MMODE_DIR "extend_lte_disable_duration"},
  { CMEFS_MSIM_STANDBY_ESR_SUPPORT,     CMEFS_NV_MMODE_DIR "msim_standby_esr_support"},
  { CMEFS_SMS_DOMAIN_PREF_LIST,         CMEFS_NV_MMODE_DIR "sms_domain_pref_list"},
  { CMEFS_ALLOW_CSFB_UPON_IMS_REG,      CMEFS_NV_MMODE_DIR "allow_csfb_upon_ims_reg"},
  { CMEFS_SSAC_HVOLTE,                  CMEFS_NV_MMODE_DIR "ssac_hvolte"},
  { CMEFS_MODE_DISABLED_IN_TSTS,        CMEFS_NV_MMODE_DIR "mode_disabled_in_TSTS"},
  { CMEFS_1xSRVCC_CALLED_PARTY_NUM,     CMEFS_NV_MMODE_DIR "1xsrvcc_stn_sr_number"},
  { CMEFS_UE_BASED_CW,                  CMEFS_NV_MMODE_DIR "ue_based_cw"},
  { CMEFS_ALLOW_SMS_IN_ECBM,            CMEFS_NV_MMODE_DIR "allow_sms_in_ecbm"},
  { CMEFS_IMS_REG_STATUS_WAIT_TIMER,    CMEFS_NV_MMODE_DIR "ims_reg_status_wait_timer"},
  { CMEFS_CUSTOM_VOWIFI_CONFIG,         CMEFS_NV_MMODE_DIR "wifi_config"},
  { CMEFS_CUSTOM_VOICE_SR_TIMER,        CMEFS_NV_MMODE_DIR "custom_voice_sr_timer"},
  { CMEFS_ID_RPM_SYS_TIME,              CMEFS_NV_MMODE_DIR "rpm_sys_time"},
  { CMEFS_MTCSFB_CALL_REDIAL_OVER_1X,   CMEFS_NV_MMODE_DIR "mtcsfb_call_redial"},
  { CMEFS_DISABLE_GLOBAL_MODE,          CMEFS_NV_MMODE_DIR "disable_global_mode"},
  { CMEFS_CDMA_LESS_DEVICE,             CMEFS_NV_MMODE_DIR "cdma_less"},
  { CMEFS_ID_LTE_BANDPREF,              CMEFS_NV_MMODE_DIR "lte_bandpref"},
  { CMEFS_ID_LTE_BANDPREF_65_256, "/nv/item_files/modem/mmode/lte_bandpref_extn_65_256" },
  { CMEFS_ID_TDS_BANDPREF,              CMEFS_NV_MMODE_DIR"tds_bandpref"},
  { CMEFS_VOLTE_SR_CONTROL,             CMEFS_NV_MMODE_DIR "volte_sr_control"},
  { CMEFS_IS_PLMN_BLOCK_REQ_IN_LTE_ONLY_MODE, CMEFS_NV_MMODE_DIR "is_plmn_block_req_in_lte_only_mode"},
  { CMEFS_LTE_3GPP_RELEASE_VER,         "/nv/item_files/modem/lte/lte_3gpp_release_ver"},
  { CMEFS_IMS_ENABLE,                   "/nv/item_files/ims/IMS_enable"},
  { CMEFS_QIPCALL_DOM_SEL_ENABLE,       "/nv/item_files/ims/qipcall_domain_selection_enable"},
  { CMEFS_EMCALL_TO_NORMAL_VOICECALL,   CMEFS_NV_MMODE_DIR "emcall_to_voice_call"},
  { CMEFS_SELF_ACTIVATION_SUPPORT,      CMEFS_NV_MMODE_DIR "self_activation_support"},
  { CMEFS_PSM_CONFIG,                   CMEFS_NV_MMODE_DIR "psm_config"},
  { CMEFS_PSM_CONFIG_EXT,               CMEFS_NV_MMODE_DIR "psm_config_ext"},
  { CMEFS_PSM_UIM_OPT,                  "/nv/item_files/modem/uim/mmgsdi/psm_mode"},
  { CMEFS_ALLOW_CS_TO_PS_EMERG_REDIAL,  CMEFS_NV_MMODE_DIR "lte_emerg_redial"},
  { CMEFS_CDMA_HOLD_SRV_TIMER,          CMEFS_NV_MMODE_DIR "1x_hold_srv_timer"},
  { CMEFS_HOLD_HY2_OPER_FLAG_TIMER,     CMEFS_NV_MMODE_DIR "hold_hy2_oper_flag_timer"},
  { CMEFS_REJECT_OIR_CLIR_INTERROGATION,      CMEFS_NV_MMODE_DIR "Supplementary_Service_Reject_OIR_CLIR_Interrogation"},
  { CMEFS_SKIP_GWT_SCAN_DURING_RLF,     CMEFS_NV_MMODE_DIR "skip_gwt_scan_during_rlf"},
  { CMEFS_LTE_RPM_INFO_2,               CMEFS_NV_MMODE_DIR "lte_rpm_info_2"},
  { CMEFS_ATTACH_COMPLETE_TIMER,         CMEFS_NV_MMODE_DIR "attach_complete_timer"},
  { CMEFS_PS_SIGNALLING_WAIT_TIMER,      CMEFS_NV_MMODE_DIR "ps_signalling_wait_timer"}
};

/* Global array populated from CMEFS_MMODE_1XSXLTE_OPTIMIZATION 
** EFS file during cmefs_init()
** One feature per index, features defined in c2k_sxlte_opti_e_type
*/
static boolean c2k_sxlte_opti_array[C2K_SXLTE_OPTI_ARRAY_USED];

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
)
{
  size_t  i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0 ; i < ARR_SIZE(cmefs_tbl) ; i++)
  {
     if (cmefs_tbl[i].efs_id == efs_id)
     {
        return cmefs_tbl[i].file_name;
     }
  }

  CM_ERR_1("Invalid EFS ID: %d", efs_id);
  return NULL;
}

/*===========================================================================

FUNCTION cmefs_map_as_id_to_mcfg_id

DESCRIPTION
  Map modem asubs_id to mcfg id

DEPENDENCIES
  None

RETURN VALUE
  mcfg id

SIDE EFFECTS
  None

===========================================================================*/
static mcfg_fs_sub_id_e_type cmefs_map_as_id_to_mcfg_id(
  sys_modem_as_id_e_type    asubs_id
)
{
  mcfg_fs_sub_id_e_type  mcfg_subs;

  switch (asubs_id)
  {
    case SYS_MODEM_AS_ID_3:
      mcfg_subs = MCFG_FS_SUBID_2;
      break;

    case SYS_MODEM_AS_ID_2:
      mcfg_subs = MCFG_FS_SUBID_1;
      break;

    case SYS_MODEM_AS_ID_NONE:
      mcfg_subs = MCFG_FS_SUBID_NONE;
      break;

    case SYS_MODEM_AS_ID_1:
    default:
      mcfg_subs = MCFG_FS_SUBID_0;
      break;
  }

  return mcfg_subs;
}

/*===========================================================================

FUNCTION cmefs_feature_jcdma_ptr

DESCRIPTION
  Return the pointer to feature_jcdma data

DEPENDENCIES
  None

RETURN VALUE
  pointer to feature_jcdma data

SIDE EFFECTS
  None.

===========================================================================*/
static jcdma_me_capability* cmefs_feature_jcdma_ptr( void )
{
  static jcdma_me_capability cmefs_feature_jcdma;

  return &cmefs_feature_jcdma;
}


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

void cmefs_refresh()
{
 
  int32 efs_size = sizeof(jcdma_me_capability);

  if(!cmefs_get(CMEFS_FEATURE_JCDMA, (byte*)cmefs_feature_jcdma_ptr(), efs_size,
    SYS_MODEM_AS_ID_1))
  {
    CM_MSG_HIGH_0("JCDMA NV not configured!");
    cmefs_feature_jcdma_ptr()->service = JCDMA_SERVICE_DISABLED;
  }

}

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

void cmefs_init()
{
  #ifndef FEATURE_DISABLE_CM_INIT_NV_WRITE
  int efs_id;
  const char* file_name;
  #endif
  struct fs_stat file_stat;
  int conf_fd = -1;
  int result = -1;
  char *mmode_config_items = "/nv/item_files/modem/mmode/sms_only\n\
/nv/item_files/modem/mmode/sms_only_Subscription01\n\
/nv/item_files/modem/mmode/ue_usage_setting\n\
/nv/item_files/modem/mmode/ue_usage_setting_Subscription01\n\
/nv/item_files/modem/mmode/voice_domain_pref\n\
/nv/item_files/modem/mmode/voice_domain_pref_Subscription01\n\
/nv/item_files/modem/mmode/sms_domain_pref\n\
/nv/item_files/modem/mmode/sms_domain_pref_Subscription01\n\
/nv/item_files/modem/mmode/lte_disable_duration\n\
/nv/item_files/modem/mmode/lte_disable_duration_Subscription01\n\
/nv/item_files/modem/mmode/n_min_MO_call_soft_retry\n\
/nv/item_files/modem/mmode/n_maxSIB8\n\
/nv/item_files/modem/mmode/sms_mandatory\n\
/nv/item_files/modem/mmode/sms_mandatory_Subscription01\n\
/nv/item_files/modem/mmode/lte_bandpref\n\
/nv/item_files/modem/mmode/lte_bandpref_Subscription01\n\
/nv/item_files/modem/mmode/tds_bandpref\n\
/nv/item_files/modem/mmode/tds_bandpref_Subscription01\n\
/nv/item_files/modem/mmode/device_mode\n\
/nv/item_files/mcs/mtf/cp_mutex_tracking_enabled\n\
/nv/item_files/modem/mmode/sd/loc_base_bsr_mcc_list\n\
/nv/item_files/modem/mmode/supplement_service_domain_pref\n\
/nv/item_files/modem/mmode/supplement_service_domain_pref_Subscription01\n\
/nv/item_files/modem/mmode/sms_over_s102\n\
/nv/item_files/modem/mmode/sms_over_s102_Subscription01\n\
/nv/item_files/modem/mmode/operator_name\n\
/nv/item_files/modem/mmode/operator_name_Subscription01\n\
/nv/item_files/modem/mmode/qmss_enabled\n\
/nv/item_files/modem/mmode/qmss_enabled_Subscription01\n\
/nv/item_files/modem/mmode/sd/1xcsfb_ecbm_status\n\
/nv/item_files/modem/mmode/get_net_auto_mode\n\
/nv/item_files/modem/mmode/global_mode_csg\n\
/nv/item_files/modem/mmode/custom_emerg_info\n\
/nv/item_files/modem/mmode/custom_emerg_info_Subscription01\n\
/nv/item_files/modem/mmode/manufacturer_name\n\
/nv/item_files/modem/mmode/manufacturer_code\n\
/nv/item_files/modem/mmode/device_model\n\
/nv/item_files/modem/mmode/sw_version\n\
/nv/item_files/modem/mmode/cu_imsi\n\
/nv/item_files/modem/mmode/cmcc_imsi\n\
/nv/item_files/modem/mmode/imsi_mcc\n\
/nv/item_files/modem/mmode/imsi_min1\n\
/nv/item_files/modem/mmode/imsi_min2\n\
/nv/item_files/modem/mmode/imsi_11_12\n\
/nv/item_files/modem/mmode/reg_status\n\
/nv/item_files/modem/mmode/mid_call_srvcc_info\n\
/nv/item_files/modem/mmode/mid_call_srvcc_info_Subscription01\n\
/nv/item_files/modem/mmode/lte_do_irat_duration\n\
/nv/item_files/modem/mmode/volte_sr_control\n\
/nv/item_files/modem/mmode/volte_sr_control_Subscription01\n\
/nv/item_files/modem/mmode/extend_lte_disable_duration\n\
/nv/item_files/modem/mmode/sd/manual_search_in_wrlf\n\
/nv/item_files/modem/mmode/sd/manual_search_in_wrlf_Subscription01\n\
/nv/item_files/modem/mmode/supportfordualrxues\n\
/nv/item_files/modem/mmode/sxlte_timers\n\
/nv/item_files/modem/mmode/msim_standby_esr_support\n\
/nv/item_files/modem/mmode/3gpp2_ps_call_optimization\n\
/nv/item_files/modem/mmode/sms_domain_pref_list\n\
/nv/item_files/modem/mmode/sms_domain_pref_list_Subscription01\n\
/nv/item_files/modem/mmode/allow_csfb_upon_ims_reg\n\
/nv/item_files/modem/mmode/allow_csfb_upon_ims_reg_Subscription01\n\
/nv/item_files/modem/mmode/sd/c2k_resel_splmn_supp\n\
/nv/item_files/modem/mmode/sd/buffer_int_srv_lost\n\
/nv/item_files/modem/mmode/sd/1xcsfb_call_end_opt\n\
/nv/item_files/modem/mmode/ssac_hvolte\n\
/nv/item_files/modem/mmode/ssac_hvolte_Subscription01\n\
/nv/item_files/modem/mmode/mode_disabled_in_TSTS\n\
/nv/item_files/modem/mmode/1xsrvcc_stn_sr_number\n\
/nv/item_files/modem/mmode/sd/full_srv_req_in_emerg\n\
/nv/item_files/modem/mmode/sd/full_srv_req_in_emerg_Subscription01\n\
/nv/item_files/modem/mmode/scan_scope_rule\n\
/nv/item_files/modem/mmode/allow_sms_in_ecbm\n\
/nv/item_files/modem/mmode/allow_sms_in_ecbm_Subscription01\n\
/nv/item_files/modem/mmode/sd/extend_lbsr_if_1x_active\n\
/nv/item_files/modem/mmode/ims_reg_status_wait_timer\n\
/nv/item_files/modem/mmode/ims_reg_status_wait_timer_Subscription01\n\
/nv/item_files/modem/mmode/ue_based_cw\n\
/nv/item_files/modem/mmode/ue_based_cw_Subscription01\n\
/nv/item_files/modem/mmode/wifi_config\n\
/nv/item_files/modem/mmode/wifi_config_Subscription01\n\
/nv/item_files/modem/mmode/c2k_switch_2_srlte\n\
/nv/item_files/modem/mmode/custom_voice_sr_timer\n\
/nv/item_files/modem/mmode/custom_voice_sr_timer_Subscription01\n\
/nv/item_files/modem/mmode/sd/roam_ind_update_in_l_to_1x_ho\n\
/nv/item_files/modem/mmode/mmode_1xsxlte_optimization\n\
/nv/item_files/modem/mmode/rpm_sys_time\n\
/nv/item_files/modem/mmode/mtcsfb_call_redial\n\
/nv/item_files/modem/mmode/rat_srv_ind_timer_values\n\
/nv/item_files/modem/mmode/1xsib8_scan_opt\n\
/nv/item_files/modem/mmode/disable_global_mode\n\
/nv/item_files/modem/mmode/is_plmn_block_req_in_lte_only_mode\n\
/nv/item_files/modem/mmode/is_plmn_block_req_in_lte_only_mode_Subscription01\n\
/nv/item_files/modem/mmode/cdma_less\n\
/nv/item_files/modem/mmode/emcall_to_voice_call\n\
/nv/item_files/modem/mmode/emcall_to_voice_call_Subscription01\n\
/nv/item_files/modem/mmode/self_activation_support\n\
/nv/item_files/modem/mmode/self_activation_support_Subscription01\n\
/nv/item_files/modem/mmode/psm_config\n\
/nv/item_files/modem/mmode/psm_config_ext\n\
/nv/item_files/modem/mmode/lte_emerg_redial\n\
/nv/item_files/modem/mmode/lte_emerg_redial_Subscription01\n\
/nv/item_files/modem/mmode/1x_hold_srv_timer\n\
/nv/item_files/modem/mmode/hold_hy2_oper_flag_timer\n\
/nv/item_files/modem/mmode/Supplementary_Service_Reject_OIR_CLIR_Interrogation\n\
/nv/item_files/modem/mmode/Supplementary_Service_Reject_OIR_CLIR_Interrogation_Subscription01\n\
/nv/item_files/modem/mmode/skip_gwt_scan_during_rlf\n\
/nv/item_files/modem/mmode/skip_gwt_scan_during_rlf_Subscription01\n\
/sd/emergency_call_1x_scan_first\n\
/nv/item_files/modem/mmode/sd/enhanced_f3_dbg\n\
/nv/item_files/modem/mmode/attach_complete_timer\n\
/nv/item_files/modem/mmode/lte_rpm_info_2\n\
/nv/item_files/modem/mmode/ps_signalling_wait_timer\n\
";

  cmefs_refresh();

  (void)mcfg_mkdir("/mmode",0777, MCFG_FS_TYPE_EFS);
  CM_MSG_MED_1("created mmode directory, efs_errno=%d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));
  /* Create NV folder if it doesnt exist
  */
  (void)mcfg_mkdir("/nv",0777, MCFG_FS_TYPE_EFS);
  CM_MSG_MED_1("create nv directory, efs_errno=%d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

  /* Create item_files folder if it doesnt exist
  */
  (void)mcfg_mkdir("/nv/item_files",0777, MCFG_FS_TYPE_EFS);
  CM_MSG_MED_1("create directory /nv/item_files, efs_errno=%d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

  /* Create conf folder if it doesn't exist
  */
  (void)mcfg_mkdir("/nv/item_files/conf",0777, MCFG_FS_TYPE_EFS);

  /* Create modem folder if it doesnt exist
  */
  (void)mcfg_mkdir("/nv/item_files/modem",0777, MCFG_FS_TYPE_EFS);
  CM_MSG_MED_1("create directory /nv/item_files/modem, efs_errno=%d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

  /* Create mmode folder if it doesnt exist
  */
  (void)mcfg_mkdir("/nv/item_files/modem/mmode",0777, MCFG_FS_TYPE_EFS);
  CM_MSG_MED_1("create directory /nv/item_files/modem/mmode, efs_errno=%d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

  result = mcfg_mkdir("/nvm/alpha/modem/cm",0777, MCFG_FS_TYPE_EFS);
  if(result == -1)
  {
    CM_MSG_HIGH_1("mkdir error %d",result);

  }

  (void)mcfg_mkdir(CMEFS_PH_DIR,0777, MCFG_FS_TYPE_EFS);
  CM_MSG_MED_1("created cmph directory, efs_errno=%d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

  (void)mcfg_mkdir(CMEFS_SS_DIR,0777, MCFG_FS_TYPE_EFS);
  CM_MSG_MED_1("created cmss directory, efs_errno=%d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

  (void)mcfg_mkdir(CMEFS_CALL_DIR,0777, MCFG_FS_TYPE_EFS);
  CM_MSG_MED_1("created cmcall directory, efs_errno=%d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

  (void)mcfg_mkdir(CMEFS_MM_DIR,0777, MCFG_FS_TYPE_EFS);
  CM_MSG_MED_1("created cmmm directory, efs_errno=%d", mcfg_fs_errno(MCFG_FS_TYPE_EFS));

  /* Create mmode.conf file */
  if( (mcfg_fs_stat(CMEFS_MMODE_CONF_F_NAME, &file_stat, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE)
       != MCFG_FS_STATUS_OK)
     && mcfg_fs_errno(MCFG_FS_TYPE_EFS) == ENOENT )
  {
    CM_MSG_HIGH_0("mmode.conf not present yet, creating it now");

    /* Open /nv/item_files/mmode.conf file */
    conf_fd =  mcfg_fopen (CMEFS_MMODE_CONF_F_NAME, 
                           MCFG_FS_O_WRONLY|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC, 
                           MCFG_FS_ALLPERMS, 
                           MCFG_FS_TYPE_EFS, 
                           MCFG_FS_SUBID_NONE);

    if ( conf_fd >= 0 )
    {
      (void)mcfg_fwrite(conf_fd,
                        mmode_config_items,
                        strlen(mmode_config_items),
                        MCFG_FS_TYPE_EFS);

    }else
    {
      CM_ERR_2 ( " Failed to open mmode.conf efs_err=%d errno %d", conf_fd, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    }
    (void)mcfg_fclose(conf_fd, MCFG_FS_TYPE_EFS);
    CM_MSG_HIGH_1("mmode.conf file fd %d",conf_fd);
  }

  /* Read the efs item for 1XSXLTE optimization
  ** and set default value if read fails 
  */
  cmefs_init_c2k_sxlte_opti();

  return;
}

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
boolean cmefs_get_c2k_sxlte_opti(c2k_sxlte_opti_e_type opti_index)
{
  return c2k_sxlte_opti_array[opti_index];
}

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
void cmefs_init_c2k_sxlte_opti()
{
  int32 opti_index = 0;
  int32 size_of_opti_struct = 0;
  c2k_sxlte_opti_e_type opti;
  mmode_1xsxlte_optimization_s_type efs_opti;
  size_of_opti_struct = sizeof(mmode_1xsxlte_optimization_s_type);

  /* Read efs item for 1XSXLTE optimization and 
  ** populate c2k_sxlte_opti_array 
  */
  if(cmefs_read(CMEFS_MMODE_1XSXLTE_OPTIMIZATION, 
                (byte*)&efs_opti, &size_of_opti_struct))
  {
    memscpy(c2k_sxlte_opti_array, sizeof(c2k_sxlte_opti_array), efs_opti.opti, sizeof(c2k_sxlte_opti_array));
  }
  else
  {
    /* Read failed, populate c2k_sxlte_opti_array with
    ** default values and write back to EFS 
    */
    memset(&efs_opti, 0, sizeof(efs_opti));
    for(opti_index = 0; opti_index < sizeof(c2k_sxlte_opti_array); opti_index++)
    {
      opti = (c2k_sxlte_opti_e_type) opti_index;

      switch(opti)
      {
        case C2K_SXLTE_OPTI_PWRUP_FROM_LAST_SUBMODE:
          efs_opti.opti[opti_index] = FALSE;
          break;

        case C2K_SXLTE_OPTI_PS_DETACH:
          efs_opti.opti[opti_index] = TRUE;
          break;

        case C2K_OPTI_ESR_E911_3RD_PARTY:
          efs_opti.opti[opti_index] = TRUE;
          break;

        default:
          break;
      }
    }
    memscpy(c2k_sxlte_opti_array, sizeof(c2k_sxlte_opti_array), efs_opti.opti, sizeof(c2k_sxlte_opti_array));

    CM_MSG_MED_0("cmefs_init_c2k_sxlte_opti: Writing DEFAULT mmode_1xsxlte_optimization values to EFS");
    cmefs_write(CMEFS_MMODE_1XSXLTE_OPTIMIZATION,
                (byte*)&efs_opti,
                sizeof(mmode_1xsxlte_optimization_s_type));
  }
}

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

==========================================================================*/
boolean                  cmefs_read(

        cmefs_id_e_type          efs_id,
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
)
{
  int         fd;
  const char* file;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( data_ptr != NULL );
  CM_ASSERT( size_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  file = cmefs_map_efs_id_to_filename(efs_id);
  if (NULL == file)
  {
    return FALSE;
  }

  /* Open the specified resource file.
  ** If open fails, return error code
  */
  CM_MSG_HIGH_2( "Open file=%s buffer size %d",
               file, *size_ptr);

  fd = mcfg_fopen( file, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0);

  if( fd < 0 )
  {
    CM_MSG_HIGH_2( "open efs_err=%d errno %d", fd, mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read data from file.
  */
  *size_ptr = mcfg_fread( fd, data_ptr, *size_ptr, MCFG_FS_TYPE_EFS );


  /* If the file read fails or file size is 0, return FALSE
  */
  if( *size_ptr <= 0 )
  {
    CM_MSG_HIGH_2( "efs read error size - %d errno - %d", *size_ptr, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    *size_ptr = 0;
    (void) mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the number of bytes actually read.
  */
  CM_MSG_LOW_1( "size_read=%d", *size_ptr);


  /* Close the file */
  (void) mcfg_fclose(fd, MCFG_FS_TYPE_EFS);

  return TRUE;
}



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

boolean                  cmefs_write(

        cmefs_id_e_type          efs_id,
            /* passing the efs Id to write.
            */
        const byte               *data_ptr,
            /* Pointer to a data buffer to write to EFS.
            */
        int32                    size
            /* Size of the data_ptr buffer.
            */
)
{
  int         fd;
  int32       size_committed;
  const char* file_name;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( data_ptr != NULL );
  CM_ASSERT( size != 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  file_name = cmefs_map_efs_id_to_filename(efs_id);
  if (NULL == file_name)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Open the specified resource file.  If the file does not exists, create one.
  */
  CM_MSG_HIGH_1( "Open file=%s",file_name);

  fd = mcfg_fopen( file_name, 
                   MCFG_FS_O_CREAT | MCFG_FS_O_WRONLY | MCFG_FS_O_TRUNC | MCFG_FS_O_AUTODIR,
                   MCFG_FS_ALLPERMS,
                   MCFG_FS_TYPE_EFS, 
                   MCFG_FS_SUBID_0);

  if ( fd < 0 )
  {
    CM_MSG_HIGH_2( "open efs_err=%d errno %d", fd, mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write data to file.
  */
  size_committed = mcfg_fwrite( fd, (byte*)data_ptr, size, MCFG_FS_TYPE_EFS );

  if( size_committed < 0 )
  {
    CM_MSG_HIGH_2( "write efs_err=%d errno %d", size_committed , mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    /* Close the file */
    (void) mcfg_fclose(fd, MCFG_FS_TYPE_EFS);

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the number of bytes actually written.
  */
  CM_MSG_HIGH_2( "size_req=%d, size_wrote=%d",
               size, size_committed);


  /* Close the file */
  (void) mcfg_fclose(fd, MCFG_FS_TYPE_EFS);

  return TRUE;

}

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
)
{
  mcfg_fs_status_e_type       status;
  const char                  *file_name;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(data_ptr != NULL);
  CM_ASSERT(size     != 0);

  /* Map efs_id to filename
  */
  file_name = cmefs_map_efs_id_to_filename(efs_id);

  if(NULL == file_name)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write the specified resource file
  */
  CM_MSG_HIGH_2("mcfg_fs_write file = %s, sub_id %d", file_name, sub_id);

  status = mcfg_fs_write(file_name,
                         (void*)data_ptr,
                         size,
                         MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                         MCFG_FS_ALLPERMS,
                         MCFG_FS_TYPE_EFS,
                         cmefs_map_as_id_to_mcfg_id(sub_id));

  if(status != MCFG_FS_STATUS_OK)
  {
    CM_MSG_HIGH_3("mcfg_fs_write error, sub_id %d, status=%d, errno %d", sub_id, status,
                   mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  return TRUE;
}

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
cmefs_id_e_type cmefs_get_efs_id_for_curr_nam (

   cm_nam_e_type   nam_value
   /* Current NAM value */
)
{
  switch (nam_value)
  {
    case  CM_NAM_1:
      return CMEFS_USER_NET_SEL_MODE_PREF_NAM_1;

    case  CM_NAM_2:
      return CMEFS_USER_NET_SEL_MODE_PREF_NAM_2;

    case  CM_NAM_3:
      return CMEFS_USER_NET_SEL_MODE_PREF_NAM_3;

    case  CM_NAM_4:
      return CMEFS_USER_NET_SEL_MODE_PREF_NAM_4;

    default:
      return CMEFS_USER_NET_SEL_MODE_PREF_NAM_1;
      /* Return NAM 1 as default for AUTO case */
  }
}

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
boolean cm_is_jcdma_enable(void)
{
  jcdma_me_capability *feature_jcdma = cmefs_feature_jcdma_ptr();

  switch(feature_jcdma->service)
  {
    case JCDMA_SERVICE_ENABLED:
      return TRUE;

    case JCDMA_SERVICE_DISABLED:
    default:
      return FALSE;
  }
}

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
byte cmefs_get_itc_aux_val(void)
{
  byte itc_aux_val = CMALS_ORANGE_GSM_LL_AUXILIARY_SPEECH;
  #ifdef CM_GW_SUPPORTED
  mm_nas_nv_items_T mm_nas_feature_to_be_configured;
  mcfg_fs_status_e_type status;

  status = mcfg_fs_read ("/nv/item_files/modem/nas/nas_config_feature", 
                   &mm_nas_feature_to_be_configured,
                         sizeof(mm_nas_nv_items_T), 
                         MCFG_FS_TYPE_EFS, 
                         MCFG_FS_SUBID_0 );

  if(status != MCFG_FS_STATUS_OK)
  {
    CM_MSG_MED_1("mcfg fs read error status %d", status);
  }
  else
  {
    CM_MSG_MED_1("EFS get ITC AUX Val %d",mm_nas_feature_to_be_configured.als_itc_value);

    if(mm_nas_feature_to_be_configured.als_itc_value == (byte)ALS_ITC_VALUE_6)
    {
      itc_aux_val = CMALS_ORANGE_GSM_LL_AUXILIARY_SPEECH_SIX;
    }
  }
  #endif
  return itc_aux_val;

}



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
)
{
  const char* filename;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( data_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  filename = cmefs_map_efs_id_to_filename(efs_id);

  return cmefs_get2( filename, data_ptr, size, sub_id );
}

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
)
{
  struct fs_stat          st;
  mcfg_fs_status_e_type   mcfg_status;
  mcfg_fs_sub_id_e_type   mcfg_subs = cmefs_map_as_id_to_mcfg_id(sub_id);

   CM_ASSERT( size != NULL );
  
  if (NULL == filename)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&st, 0, sizeof(st));

  /* Get the size of the file.
  */
  mcfg_status = mcfg_fs_stat( filename,
                              &st,
                              MCFG_FS_TYPE_EFS,
                              mcfg_subs );

  if (MCFG_FS_STATUS_OK != mcfg_status)
  {
       CM_MSG_HIGH_3("mcfg_fs_stat error, sub_id %d, status %d, errno %d", sub_id, mcfg_status,
                     mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return FALSE;
  }

   size	= st.st_size;

  /* Get data from file.
  */
  mcfg_status = mcfg_fs_read (filename, 
                         data_ptr, 
                         size, 
                         MCFG_FS_TYPE_EFS, 
                         mcfg_subs );

  if(mcfg_status != MCFG_FS_STATUS_OK)
  {
    CM_MSG_HIGH_3("mcfg_fs_read error, sub_id %d, status %d, errno %d", sub_id, mcfg_status,
                     mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;
}

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
)
{
  mcfg_fs_status_e_type       status;
  const char                  *file_name;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(data_ptr != NULL);
  CM_ASSERT(size     != 0);

  /* Map efs_id to filename
  */
  file_name = cmefs_map_efs_id_to_filename(efs_id);

  if(NULL == file_name)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write the specified resource file
  */
  CM_MSG_HIGH_2("mcfg_fs_write file = %s, sub_id %d", file_name, sub_id);

  status = mcfg_fs_write(file_name,
                         (void*)data_ptr,
                         size,
                         MCFG_FS_O_RDWR,
                         MCFG_FS_ALLPERMS,
                         MCFG_FS_TYPE_EFS,
                         cmefs_map_as_id_to_mcfg_id(sub_id));

  if(status != MCFG_FS_STATUS_OK)
  {
    CM_MSG_HIGH_3("mcfg_fs_write error, sub_id %d, status=%d, errno %d", sub_id, status,
                   mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return FALSE;
  }

  return TRUE;
}

