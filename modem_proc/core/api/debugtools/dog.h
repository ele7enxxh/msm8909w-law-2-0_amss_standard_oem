#ifndef DOG_H
#define DOG_H
/** 
  @file dog.h
  @brief This file contains external procedure declarations necessary for
  accessing procedures contained within the watchdog task file.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The watchdog_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and LaTeX. To edit or 
      update any of the file/group text in the PDF, edit the 
      watchdog_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the dog group 
      description in the watchdog_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
  Copyright (c) 1990, 1992-1995, 1998-2010 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

            W A T C H D O G    T A S K    H E A D E R    F I L E

DESCRIPTION
  This file contains external procedure declarations necessary for
  accessing procedures contained within the watchdog task file.


===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/debugtools/dog.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
12/08/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
07/02/09   va      Added TCXO Manager Task
05/26/09   st      Added GFX task
03/12/09   vas     Added entries for IPAUKCC_IPC
03/05/09   am      Added DCC Task.
01/27/09   bdh     Added proc comm function set_arm9_sw_watchdog_state
                   for disable/enable sw watchdog when Apps proc is in debugbreak
                   and continue states
11/10/08   bdh     Added dog task disable function to disable sw watchdog
                   to support WinMob ARM11 debugbreak/continue in PB.
07/29/08   ar      Added QMI task.
06/27/08   apu	   Added support for FEATURE_VBATT_TASK	
02/28/08   cr/aar  Added PGI Task
06/18/07   ry      Added support for FEATURE_SENSORS
05/25/07   gfr     Fixed DLAGENT report time.
05/16/07   gfr     Support for QVPHONE_RTP
04/19/07   gfr     HSU updates.
04/12/07   gfr     Added support for QTV_STATIC_DLAGENT_TASK
11/01/06   pc      Added support for FEATURE_HS_USB
10/03/06   pc      Added support for QTV_TASK10
08/29/06   pc      Added support for FEATURE_EXT_USB_P1
08/14/06   pc      Added feature FEATURE_QTV_STATIC_TASKS.
08/09/06   pc      Added support for QTV_VIDEO_RENDERER and QTV_AUDIO
07/25/06   pc      Added support for FEATURE_CPU_BASED_FLOW_CONTROL
07/17/06   pc      Added support for FEATURE_DMTASK.
06/19/06   pc      Added support for FEATURE_SEC_USFS.
05/05/06   ac      Added support for FEATURE_QDJTEST
04/24/06   bt      Added support for UBM L1 task.
03/24/06   pc      Added support for SECRND task
03/23/06   cl      Added PDSM_COMM_TASK
01/23/06   rlp     Added HID Support.
01/02/06   dle     Added FEATURE_CAMERA_DRV_TASK.
11/28/05   wd      Added L4 dog opt.
11/10/05   pc      Added support for AVS_SBCENC_TASK
11/08/05   pc      Added support for GSM_PLT task
09/12/05   pc      Added support for FEATURE_USB_HOST_ENHANCED.
08/26/05   pc      Added support for DISP_TASK.
06/12/05   hba     Added support for WLAN Sc task.
04/21/05   pc      Added support for FEATURE_MFLO
04/20/05   pc      Added support for FEATURE_QIPCALL
04/09/05   pc      Added support for CNV task
03/15/05   wal     Added APS task for L4/Linux under FEATURE_APS
03/15/05   adg     Added QDIAG task for L4/Linux under FEATURE_IG_QDIAG
03/15/05   jqi     Added support for FS_FEATURE_RMTEFS_SERVER, FEATURE_L4
                   FEATURE_PM_SERVER,FEATURE_IG_UI_CLIENT_SELECT
02/24/05 t_jarmis  Added FEATURE_FM for FM-RDS Radio.
02/05/05   aku     Added support for WLAN_CP and WLAN_TRP tasks.
02/04/05   dwp     Added QVP support for SIP.
02/03/05   epr     Addition of Sirius specific tasks using define SIRIUS_PLATFORM
01/31/05   jqi     Revised FEATURE_MGPCC_TASK report ID
01/21/05   ldaita  Added support MGPCC task (needed for GPS).
01/06/05   aar     Added support for GPSSRCH demod task.
11/01/04   jz      Added QVP VFE Task
11/18/04   pa      Added support for PBM task.
10/22/04   jqi     Featurization fixes.
10/15/04   rp      Added support for page and interrupt tasks.
10/08/04   jqi     Added support for FEATURE_HSDPA
09/29/04   ss      Added support for DTV (Digital TV) feature
07/06/04   hl      Added support for joystick feature
05/19/04   vr      Added BCDB and HDRBC tasks
           jqi     Removed FEATURE_THIN_UI.
04/01/04   sg      removed support for LP task as task no longer exists.
02/23/04   balas   Added support for Bluetooth External Profiles feature support .
02/19/04   Jayanth Added support for FEATURE_SEC_DRM.
02/12/04   vdrapk  Added support for FEATURE_GSM_CB.
11/07/03   dd      Added GPS Search Task support.
10/13/03   dwp     Added initial QVPHONE support.
10/03/03   ajn     Removed MSM5500's HDR autokicker & related code
07/23/03    bs     Merged ThinUI changes from 6200 archive. This is featurized
                   as FEATURE_THIN_UI.
07/21/03   mk/dlr  Added dog_gsm_autokick
07/11/03    gr     Added a new signal to enable asynchronous filesystem access
                   from the Dog task.
05/29/03   Om      Added support for ARM_CRYPTO and IPSEC tasks.
05/28/03   TMR/dgy Added changes for WCDMA single-mode build.
05/14/03    ld     Added support for MGP related tasks
03/14/03   jqi     Added GSM/WCDMA feature around TMC task.
03/07/03    gr     Corrected a potential problem with the removal of the MS
                   task.
03/04/03    lz     Removed FEATURE_MOBILE_SUITE
02/21/03   jay     Added suuport for SSL task
02/18/03   jqi/jar Added support for the Generic SIM Toolkit Task (FEATURE_GSTK)
12/31/02   jqi     Merged with msmshared/umts/services/task/dog.h#39
                   1.Added support for FEATURE_GPRS.
11/29/02   lz      Added DOG_MS_RPT under FEATURE_MOBILE_SUITE.
10/18/02   ma, ao  Added FEATURE_GSM_PLT for Vocoder Dog report
08/18/02   jqi     Moved to MSMSHARED/SERVICES/TASK directory.
09/16/02   jqi     Merged with COMMON and MSMSHARED UMTS archive.

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc. */
#include "task.h"       /* Task definitions                 */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/** @addtogroup dog_api
@{ */

/** Notifies tasks to use the dynamic API.

*/
#define DOG_DYNAMIC_API   /* Dog 1.5 */

/** Default timeouts (in ms) for use in dynamic API

*/
#define DOG_FAST_TIMEOUT        2000
#define DOG_DEFAULT_TIMEOUT     60000

/**
This define is deprecated.

@deprecated
This define is scheduled for removal from the API.
*/
#define DOG_HALT_TIMER_SIG      0x1000

/**
This define is deprecated.

@deprecated
This define is scheduled for removal from the API.
*/
#define DOG_WAKEUP_TIMER_SIG    0x0800

/**
This define is deprecated.

@deprecated
This define is scheduled for removal from the API.
*/
#define DOG_FS_COMPLETE_SIG 0x400


/*----------------------------------------------------------------------------
    Bits to be set in Sleep Control Register (SLEEP_CTL)
----------------------------------------------------------------------------*/

/**
This define is deprecated.

@deprecated
This define is scheduled for removal from the API.
*/
#define SLEEP_CTL_SLEEP_CTLR_AUTOKICK_ENA_V     0x00000002

/**
This define is deprecated.

@deprecated
This define is scheduled for removal from the API.
*/
#define SLEEP_CTL_WATCH_DOG_V                   0x00000001

/*----------------------------------------------------------------------------
    Bits indicating watch dog state as set by remote processors
----------------------------------------------------------------------------*/

/** Used by the remote processor to request the default watchdog function. This 
  define is used in set_arm9_sw_watchdog_state(); however, it is planned for 
  deprecation.
*/
#define DOG_DEFAULT_STATE                       0x00000000

/** Used by the remote processor to request the halt of watchdog monitoring. This 
  define is used in set_arm9_sw_watchdog_state(); however, it is planned for 
  deprecation.
*/
#define DOG_HALT_MONITORING                     0x00000001

/** Used by the remote processor to request the modem halt state. This define 
  is used in set_arm9_sw_watchdog_state(); however, it is planned for 
  deprecation.
*/
#define DOG_HALT_MODEM                          0x00000002


/*------------------------------------------------------------------------*/



/** Represents the handle used in the Watchdog API.
*/
typedef unsigned int dog_report_type;

/** @name Legacy Watchdog Values
  These legacy watchdog report values and timer values are replaced by the scalable 
  Watchdog API.
  @{ 
*/

/******************************************************************
|\     /|(  ___  )(  ____ )( (    /|\__   __/( (    /|(  ____ \( )
| )   ( || (   ) || (    )||  \  ( |   ) (   |  \  ( || (    \/| |
| | _ | || (___) || (____)||   \ | |   | |   |   \ | || |      | |
| |( )| ||  ___  ||     __)| (\ \) |   | |   | (\ \) || | ____ | |
| || || || (   ) || (\ (   | | \   |   | |   | | \   || | \_  )(_)
| () () || )   ( || ) \ \__| )  \  |___) (___| )  \  || (___) | _ 
(_______)|/     \||/   \__/|/    )_)\_______/|/    )_)(_______)(_)

LEGACY DOG IS DEPRECATED.  DO NOT EDIT. ALL CHANGES WILL BE REJECTED
******************************************************************/


/* ===============================================*/
/* BEGIN: LEGACY DOG CHANGES                        */
/*================================================*/

/*================================================*/ 
/*   DOG.H                                        */ 
/*================================================*/ 
/*================================================*/ 
/* BEGIN extern rpt decl.... For dog_h */ 
/*================================================*/ 
extern dog_report_type dog_snd_rpt;              
extern dog_report_type dog_tx_rpt;               
extern dog_report_type dog_rx_rpt;               
extern dog_report_type dog_srch_rpt;             
extern dog_report_type dog_rxtx_rpt;             
extern dog_report_type dog_mc_rpt;               
extern dog_report_type dog_hs_rpt;               
extern dog_report_type dog_diag_rpt;             
extern dog_report_type dog_ui_rpt;               
extern dog_report_type dog_nv_rpt;               
extern dog_report_type dog_auth_rpt;             
extern dog_report_type dog_ds_rpt;               
extern dog_report_type dog_ps_rpt;               
extern dog_report_type dog_tcptest_rpt;          
extern dog_report_type dog_sleep_rpt;            
extern dog_report_type dog_qmip_rpt;             
extern dog_report_type dog_dh_rpt;               
extern dog_report_type dog_vs_rpt;               
extern dog_report_type dog_graph_rpt;            
extern dog_report_type dog_fs_rpt;               
extern dog_report_type dog_cm_rpt;               
extern dog_report_type dog_voc_rpt;              
extern dog_report_type dog_pdsm_rpt;             
extern dog_report_type dog_pdsmsclient_rpt;      
extern dog_report_type dog_cd_rpt;                           
extern dog_report_type dog_pb_rpt;               
extern dog_report_type dog_qdsp_rpt;             
extern dog_report_type dog_usbdc_rpt;            
extern dog_report_type dog_sfat_rpt;             
extern dog_report_type dog_bt_rpt;               
extern dog_report_type dog_tdso_rpt;             
extern dog_report_type dog_sec_rpt;              
extern dog_report_type dog_uim_rpt;              
extern dog_report_type dog_hdrmc_rpt;            
extern dog_report_type dog_hdrrx_rpt;            
extern dog_report_type dog_hdrtx_rpt;            
extern dog_report_type dog_hdrsrch_rpt;          
extern dog_report_type dog_hwtc_rpt;             
extern dog_report_type dog_wms_rpt;              
extern dog_report_type dog_tmc_rpt;              
extern dog_report_type dog_mmoc_rpt;             
extern dog_report_type dog_gsdi_rpt;             
extern dog_report_type dog_mdsp_rpt;             
extern dog_report_type dog_gcsd_rpt;             
extern dog_report_type dog_gsm_l1_rpt;           
extern dog_report_type dog_gsm_l2_rpt;           
extern dog_report_type dog_gllc_rpt;             
extern dog_report_type dog_grlc_ul_rpt;          
extern dog_report_type dog_grlc_dl_rpt;          
extern dog_report_type dog_gmac_rpt;             
extern dog_report_type dog_l1_rpt;               
extern dog_report_type dog_l2dl_rpt;             
extern dog_report_type dog_l2ul_rpt;             
extern dog_report_type dog_lsm_rpt;              
extern dog_report_type dog_mn_cnm_rpt;           
extern dog_report_type dog_mm_rpt;               
extern dog_report_type dog_plt_rpt;              
extern dog_report_type dog_test_rpt;             
extern dog_report_type dog_reg_rpt;              
extern dog_report_type dog_rr_rpt;               
extern dog_report_type dog_rrc_rpt;              
extern dog_report_type dog_sm_rpt;               
extern dog_report_type dog_tc_rpt;               
extern dog_report_type dog_gpssrch_demod_rpt;    
extern dog_report_type dog_ftm_rpt;              
extern dog_report_type dog_gstk_rpt;             
extern dog_report_type dog_virtsat_rpt;          
extern dog_report_type dog_secssl_rpt;                     
extern dog_report_type dog_mgpnf_rpt;            
extern dog_report_type dog_mgpcd_rpt;            
extern dog_report_type dog_mgpai_rpt;            
extern dog_report_type dog_interrupt_rpt;        
extern dog_report_type dog_seccryptarm_rpt;      
extern dog_report_type dog_secips_rpt;           
extern dog_report_type dog_qvp_ae_rpt;           
extern dog_report_type dog_qvpio_rpt;            
extern dog_report_type dog_qvppl_rpt;            
extern dog_report_type dog_qvp_app_rpt;          
extern dog_report_type dog_qvp_mpeg4_rpt;        
extern dog_report_type dog_gpssrch_rpt;          
extern dog_report_type dog_cb_rpt;               
extern dog_report_type dog_secdrm_rpt;           
extern dog_report_type dog_bt_pf_rpt;            
extern dog_report_type dog_hdrbc_rpt;            
extern dog_report_type dog_bcdb_rpt;             
extern dog_report_type dog_joyst_rpt;            
extern dog_report_type dog_dtv_rpt;              
extern dog_report_type dog_mac_hs_dl_rpt;        
extern dog_report_type dog_pbm_rpt;              
extern dog_report_type dog_qvp_vfe_rpt;          
extern dog_report_type dog_mgpcc_rpt;            
extern dog_report_type dog_irda_rpt;             
extern dog_report_type dog_sirius_test_rpt;      
extern dog_report_type dog_aam_rpt;              
extern dog_report_type dog_bm_rpt;               
extern dog_report_type dog_sipt_rpt;             
extern dog_report_type dog_qvp_rtp_rpt;          
extern dog_report_type dog_wlan_cp_rpt;          
extern dog_report_type dog_wlan_trp_rpt;         
extern dog_report_type dog_fmrds_rpt;            
extern dog_report_type dog_qdiag_rpt;            
extern dog_report_type dog_aps_rpt;              
extern dog_report_type dog_efs_srvr_rpt;         
extern dog_report_type dog_pms_rpt;              
extern dog_report_type dog_cnv_rpt;              
extern dog_report_type dog_qipcall_rpt;          
extern dog_report_type dog_mfmc_rpt;             
extern dog_report_type dog_mfpt_rpt;             
extern dog_report_type dog_mfrxd_rpt;            
extern dog_report_type dog_wlan_sec_rpt;         
extern dog_report_type dog_disp_rpt;             
extern dog_report_type dog_usbhc_urb_rpt;        
extern dog_report_type dog_usbhc_dev_rpt;        
extern dog_report_type dog_usbhc_test_rpt;       
extern dog_report_type dog_sbcenc_rpt;           
extern dog_report_type dog_camera_drv_rpt;       
extern dog_report_type dog_adsp_rtos_rpt;        
extern dog_report_type dog_graphics_rpt;         
extern dog_report_type dog_ixhttpsvc_rpt;        
extern dog_report_type dog_ats_rpt;              
extern dog_report_type dog_comp_rpt;             
extern dog_report_type dog_pdsm_comm_rpt;        
extern dog_report_type dog_secrnd_rpt;           
extern dog_report_type dog_ubm_l1_rpt;           
extern dog_report_type dog_qdjtest_rpt;          
extern dog_report_type dog_sfs_rpt;              
extern dog_report_type dog_dmtask_rpt;           
extern dog_report_type dog_fc_rpt;               
extern dog_report_type dog_qtv_video_renderer_rpt; 
extern dog_report_type dog_qtv_audio_rpt;        
extern dog_report_type dog_usbebi2_rpt;          
extern dog_report_type dog_usbapi_rpt;           
extern dog_report_type dog_usbd_rpt;             
extern dog_report_type dog_qtv_task10_rpt;       
extern dog_report_type dog_hsu_al_ms_rpt;        
extern dog_report_type dog_hsu_al_task_rpt;      
extern dog_report_type dog_mgpmc_rpt;            
extern dog_report_type dog_pp_rpt;               
extern dog_report_type dog_cc_rpt;                          
extern dog_report_type dog_nf_rpt;               
extern dog_report_type dog_gpsfft_rpt;                
extern dog_report_type dog_lm_rpt;               
extern dog_report_type dog_sm_tm_rpt;            
extern dog_report_type dog_pdcommtcp_rpt;        
extern dog_report_type dog_pdcommwms_rpt;        
extern dog_report_type dog_qtv_dlagenttask_rpt;  
extern dog_report_type dog_sns_rpt;              
extern dog_report_type dog_pgi_rpt;              
extern dog_report_type dog_ukcc_rpt;             
extern dog_report_type dog_chg_rpt;
extern dog_report_type dog_vbatt_rpt;            
extern dog_report_type dog_qmi_rpt;              
extern dog_report_type dog_loc_middleware_rpt;   
extern dog_report_type dog_dcc_rpt;              
extern dog_report_type dog_ipa_main_rpt;         
extern dog_report_type dog_ipa_schan_rpt;        
extern dog_report_type dog_ipa_pdchan_rpt;       
extern dog_report_type dog_ipaukcc_ipc_rpt;      
extern dog_report_type dog_rtp_manager_rpt;      
extern dog_report_type dog_ftm_camera_rpt;       
extern dog_report_type dog_gfx_rpt;              
extern dog_report_type dog_tcxomgr_rpt;              
extern dog_report_type dog_lsm_app_proc_rpt;     
extern dog_report_type dog_hitapp_rpt;           
extern dog_report_type dog_flash_ig_srvr_rpt;    
extern dog_report_type dog_bt_fs_rpt;            
extern dog_report_type dog_flute_rpt;            
extern dog_report_type dog_ims_rpt;                    
extern dog_report_type dog_um_rpt;               
extern dog_report_type dog_mvssmms_rpt;          
extern dog_report_type dog_qtv_dldspsvctask_rpt; 
extern dog_report_type dog_ims_cb_rpt;           
extern dog_report_type dog_videoshare_rpt;       
extern dog_report_type dog_imshom_rpt;                   
extern dog_report_type dog_gpsfft_rpt;                            
extern dog_report_type dog_auddrv_rpt;           
extern dog_report_type dog_wlan_ds_rpt;          
extern dog_report_type dog_rf_rpt;
extern dog_report_type dog_ds_sig_rpt;
extern dog_report_type dog_hdrdec_rpt;

/*================================================*/ 
/* END extern rpt decl.... */ 
/*================================================*/ 

/*================================================*/ 
/* BEGIN extern rpt defines.... For dog_h */ 
/*================================================*/ 
#define  DOG_SND_RPT                dog_snd_rpt               /* rpt_def */ 
#define  DOG_TX_RPT                 dog_tx_rpt                /* rpt_def */ 
#define  DOG_RX_RPT                 dog_rx_rpt                /* rpt_def */ 
#define  DOG_SRCH_RPT               dog_srch_rpt              /* rpt_def */ 
#define  DOG_RXTX_RPT               dog_rxtx_rpt              /* rpt_def */ 
#define  DOG_MC_RPT                 dog_mc_rpt                /* rpt_def */ 
#define  DOG_HS_RPT                 dog_hs_rpt                /* rpt_def */ 
#define  DOG_DIAG_RPT               dog_diag_rpt              /* rpt_def */ 
#define  DOG_UI_RPT                 dog_ui_rpt                /* rpt_def */ 
#define  DOG_NV_RPT                 dog_nv_rpt                /* rpt_def */ 
#define  DOG_AUTH_RPT               dog_auth_rpt              /* rpt_def */ 
#define  DOG_DS_RPT                 dog_ds_rpt                /* rpt_def */ 
#define  DOG_PS_RPT                 dog_ps_rpt                /* rpt_def */ 
#define  DOG_TCPTEST_RPT            dog_tcptest_rpt           /* rpt_def */ 
#define  DOG_SLEEP_RPT              dog_sleep_rpt             /* rpt_def */ 
#define  DOG_QMIP_RPT               dog_qmip_rpt              /* rpt_def */ 
#define  DOG_DH_RPT                 dog_dh_rpt                /* rpt_def */ 
#define  DOG_VS_RPT                 dog_vs_rpt                /* rpt_def */ 
#define  DOG_GRAPH_RPT              dog_graph_rpt             /* rpt_def */ 
#define  DOG_FS_RPT                 dog_fs_rpt                /* rpt_def */ 
#define  DOG_CM_RPT                 dog_cm_rpt                /* rpt_def */ 
#define  DOG_VOC_RPT                dog_voc_rpt               /* rpt_def */ 
#define  DOG_PDSM_RPT               dog_pdsm_rpt              /* rpt_def */ 
#define  DOG_PDSMSCLIENT_RPT        dog_pdsmsclient_rpt       /* rpt_def */ 
#define  DOG_CD_RPT                 dog_cd_rpt                /* rpt_def */ 
#define  DOG_NF_RPT                 dog_nf_rpt                /* rpt_def */ 
#define  DOG_PB_RPT                 dog_pb_rpt                /* rpt_def */ 
#define  DOG_QDSP_RPT               dog_qdsp_rpt              /* rpt_def */ 
#define  DOG_USBDC_RPT              dog_usbdc_rpt             /* rpt_def */ 
#define  DOG_SFAT_RPT               dog_sfat_rpt              /* rpt_def */ 
#define  DOG_BT_RPT                 dog_bt_rpt                /* rpt_def */ 
#define  DOG_TDSO_RPT               dog_tdso_rpt              /* rpt_def */ 
#define  DOG_SEC_RPT                dog_sec_rpt               /* rpt_def */ 
#define  DOG_UIM_RPT                dog_uim_rpt               /* rpt_def */ 
#define  DOG_HDRMC_RPT              dog_hdrmc_rpt             /* rpt_def */ 
#define  DOG_HDRRX_RPT              dog_hdrrx_rpt             /* rpt_def */ 
#define  DOG_HDRTX_RPT              dog_hdrtx_rpt             /* rpt_def */ 
#define  DOG_HDRSRCH_RPT            dog_hdrsrch_rpt           /* rpt_def */ 
#define  DOG_HWTC_RPT               dog_hwtc_rpt              /* rpt_def */ 
#define  DOG_WMS_RPT                dog_wms_rpt               /* rpt_def */ 
#define  DOG_TMC_RPT                dog_tmc_rpt               /* rpt_def */ 
#define  DOG_MMOC_RPT               dog_mmoc_rpt              /* rpt_def */ 
#define  DOG_GSDI_RPT               dog_gsdi_rpt              /* rpt_def */ 
#define  DOG_MDSP_RPT               dog_mdsp_rpt              /* rpt_def */ 
#define  DOG_GCSD_RPT               dog_gcsd_rpt              /* rpt_def */ 
#define  DOG_GSM_L1_RPT             dog_gsm_l1_rpt            /* rpt_def */ 
#define  DOG_GSM_L2_RPT             dog_gsm_l2_rpt            /* rpt_def */ 
#define  DOG_GLLC_RPT               dog_gllc_rpt              /* rpt_def */ 
#define  DOG_GRLC_UL_RPT            dog_grlc_ul_rpt           /* rpt_def */ 
#define  DOG_GRLC_DL_RPT            dog_grlc_dl_rpt           /* rpt_def */ 
#define  DOG_GMAC_RPT               dog_gmac_rpt              /* rpt_def */ 
#define  DOG_L1_RPT                 dog_l1_rpt                /* rpt_def */ 
#define  DOG_L2DL_RPT               dog_l2dl_rpt              /* rpt_def */ 
#define  DOG_L2UL_RPT               dog_l2ul_rpt              /* rpt_def */ 
#define  DOG_LSM_RPT                dog_lsm_rpt               /* rpt_def */ 
#define  DOG_MN_CNM_RPT             dog_mn_cnm_rpt            /* rpt_def */ 
#define  DOG_MM_RPT                 dog_mm_rpt                /* rpt_def */ 
#define  DOG_PLT_RPT                dog_plt_rpt               /* rpt_def */ 
#define  DOG_TEST_RPT               dog_test_rpt              /* rpt_def */ 
#define  DOG_REG_RPT                dog_reg_rpt               /* rpt_def */ 
#define  DOG_RR_RPT                 dog_rr_rpt                /* rpt_def */ 
#define  DOG_RRC_RPT                dog_rrc_rpt               /* rpt_def */ 
#define  DOG_SM_RPT                 dog_sm_rpt                /* rpt_def */ 
#define  DOG_TC_RPT                 dog_tc_rpt                /* rpt_def */ 
#define  DOG_GPSSRCH_DEMOD_RPT      dog_gpssrch_demod_rpt     /* rpt_def */ 
#define  DOG_FTM_RPT                dog_ftm_rpt               /* rpt_def */ 
#define  DOG_GSTK_RPT               dog_gstk_rpt              /* rpt_def */ 
#define  DOG_VIRTSAT_RPT            dog_virtsat_rpt           /* rpt_def */ 
#define  DOG_SECSSL_RPT             dog_secssl_rpt            /* rpt_def */ 
#define  DOG_MGPMC_RPT              dog_mgpmc_rpt             /* rpt_def */ 
#define  DOG_MGPNF_RPT              dog_mgpnf_rpt             /* rpt_def */ 
#define  DOG_MGPCD_RPT              dog_mgpcd_rpt             /* rpt_def */ 
#define  DOG_MGPAI_RPT              dog_mgpai_rpt             /* rpt_def */ 
#define  DOG_INTERRUPT_RPT          dog_interrupt_rpt         /* rpt_def */ 
#define  DOG_SECCRYPTARM_RPT        dog_seccryptarm_rpt       /* rpt_def */ 
#define  DOG_SECIPS_RPT             dog_secips_rpt            /* rpt_def */ 
#define  DOG_QVP_AE_RPT             dog_qvp_ae_rpt            /* rpt_def */ 
#define  DOG_QVPIO_RPT              dog_qvpio_rpt             /* rpt_def */ 
#define  DOG_QVPPL_RPT              dog_qvppl_rpt             /* rpt_def */ 
#define  DOG_QVP_APP_RPT            dog_qvp_app_rpt           /* rpt_def */ 
#define  DOG_QVP_MPEG4_RPT          dog_qvp_mpeg4_rpt         /* rpt_def */ 
#define  DOG_GPSSRCH_RPT            dog_gpssrch_rpt           /* rpt_def */ 
#define  DOG_CB_RPT                 dog_cb_rpt                /* rpt_def */ 
#define  DOG_SECDRM_RPT             dog_secdrm_rpt            /* rpt_def */ 
#define  DOG_BT_PF_RPT              dog_bt_pf_rpt             /* rpt_def */ 
#define  DOG_HDRBC_RPT              dog_hdrbc_rpt             /* rpt_def */ 
#define  DOG_BCDB_RPT               dog_bcdb_rpt              /* rpt_def */ 
#define  DOG_JOYST_RPT              dog_joyst_rpt             /* rpt_def */ 
#define  DOG_DTV_RPT                dog_dtv_rpt               /* rpt_def */ 
#define  DOG_MAC_HS_DL_RPT          dog_mac_hs_dl_rpt         /* rpt_def */ 
#define  DOG_PBM_RPT                dog_pbm_rpt               /* rpt_def */ 
#define  DOG_QVP_VFE_RPT            dog_qvp_vfe_rpt           /* rpt_def */ 
#define  DOG_MGPCC_RPT              dog_mgpcc_rpt             /* rpt_def */ 
#define  DOG_IRDA_RPT               dog_irda_rpt              /* rpt_def */ 
#define  DOG_SIRIUS_TEST_RPT        dog_sirius_test_rpt       /* rpt_def */ 
#define  DOG_AAM_RPT                dog_aam_rpt               /* rpt_def */ 
#define  DOG_BM_RPT                 dog_bm_rpt                /* rpt_def */ 
#define  DOG_SIPT_RPT               dog_sipt_rpt              /* rpt_def */ 
#define  DOG_QVP_RTP_RPT            dog_qvp_rtp_rpt           /* rpt_def */ 
#define  DOG_WLAN_CP_RPT            dog_wlan_cp_rpt           /* rpt_def */ 
#define  DOG_WLAN_TRP_RPT           dog_wlan_trp_rpt          /* rpt_def */ 
#define  DOG_FMRDS_RPT              dog_fmrds_rpt             /* rpt_def */ 
#define  DOG_QDIAG_RPT              dog_qdiag_rpt             /* rpt_def */ 
#define  DOG_APS_RPT                dog_aps_rpt               /* rpt_def */ 
#define  DOG_EFS_SRVR_RPT           dog_efs_srvr_rpt          /* rpt_def */ 
#define  DOG_PMS_RPT                dog_pms_rpt               /* rpt_def */ 
#define  DOG_CNV_RPT                dog_cnv_rpt               /* rpt_def */ 
#define  DOG_QIPCALL_RPT            dog_qipcall_rpt           /* rpt_def */ 
#define  DOG_MFMC_RPT               dog_mfmc_rpt              /* rpt_def */ 
#define  DOG_MFPT_RPT               dog_mfpt_rpt              /* rpt_def */ 
#define  DOG_MFRXD_RPT              dog_mfrxd_rpt             /* rpt_def */ 
#define  DOG_WLAN_SEC_RPT           dog_wlan_sec_rpt          /* rpt_def */ 
#define  DOG_DISP_RPT               dog_disp_rpt              /* rpt_def */ 
#define  DOG_USBHC_URB_RPT          dog_usbhc_urb_rpt         /* rpt_def */ 
#define  DOG_USBHC_DEV_RPT          dog_usbhc_dev_rpt         /* rpt_def */ 
#define  DOG_USBHC_TEST_RPT         dog_usbhc_test_rpt        /* rpt_def */ 
#define  DOG_SBCENC_RPT             dog_sbcenc_rpt            /* rpt_def */ 
#define  DOG_CAMERA_DRV_RPT         dog_camera_drv_rpt        /* rpt_def */ 
#define  DOG_ADSP_RTOS_RPT          dog_adsp_rtos_rpt         /* rpt_def */ 
#define  DOG_GRAPHICS_RPT           dog_graphics_rpt          /* rpt_def */ 
#define  DOG_IXHTTPSVC_RPT          dog_ixhttpsvc_rpt         /* rpt_def */ 
#define  DOG_ATS_RPT                dog_ats_rpt               /* rpt_def */ 
#define  DOG_COMP_RPT               dog_comp_rpt              /* rpt_def */ 
#define  DOG_PDSM_COMM_RPT          dog_pdsm_comm_rpt         /* rpt_def */ 
#define  DOG_SECRND_RPT             dog_secrnd_rpt            /* rpt_def */ 
#define  DOG_UBM_L1_RPT             dog_ubm_l1_rpt            /* rpt_def */ 
#define  DOG_QDJTEST_RPT            dog_qdjtest_rpt           /* rpt_def */ 
#define  DOG_SFS_RPT                dog_sfs_rpt               /* rpt_def */ 
#define  DOG_DMTASK_RPT             dog_dmtask_rpt            /* rpt_def */ 
#define  DOG_FC_RPT                 dog_fc_rpt                /* rpt_def */ 
#define  DOG_QTV_VIDEO_RENDERER_RPT  dog_qtv_video_renderer_rpt /* rpt_def */ 
#define  DOG_QTV_AUDIO_RPT          dog_qtv_audio_rpt         /* rpt_def */ 
#define  DOG_USBEBI2_RPT            dog_usbebi2_rpt           /* rpt_def */ 
#define  DOG_USBAPI_RPT             dog_usbapi_rpt            /* rpt_def */ 
#define  DOG_USBD_RPT               dog_usbd_rpt              /* rpt_def */ 
#define  DOG_QTV_TASK10_RPT         dog_qtv_task10_rpt        /* rpt_def */ 
#define  DOG_HSU_AL_MS_RPT          dog_hsu_al_ms_rpt         /* rpt_def */ 
#define  DOG_HSU_AL_TASK_RPT        dog_hsu_al_task_rpt       /* rpt_def */  
#define  DOG_PP_RPT                 dog_pp_rpt                /* rpt_def */ 
#define  DOG_CC_RPT                 dog_cc_rpt                /* rpt_def */  
#define  DOG_GPSFFT_RPT             dog_gpsfft_rpt            /* rpt_def */ 
#define  DOG_LM_RPT                 dog_lm_rpt                /* rpt_def */ 
#define  DOG_SM_TM_RPT              dog_sm_tm_rpt             /* rpt_def */ 
#define  DOG_PDCOMMTCP_RPT          dog_pdcommtcp_rpt         /* rpt_def */ 
#define  DOG_PDCOMMWMS_RPT          dog_pdcommwms_rpt         /* rpt_def */ 
#define  DOG_QTV_DLAGENTTASK_RPT    dog_qtv_dlagenttask_rpt   /* rpt_def */ 
#define  DOG_SNS_RPT                dog_sns_rpt               /* rpt_def */ 
#define  DOG_PGI_RPT                dog_pgi_rpt               /* rpt_def */ 
#define  DOG_UKCC_RPT               dog_ukcc_rpt              /* rpt_def */ 
#define  DOG_CHG_RPT	            dog_chg_rpt               /* rpt_def */ 
#define  DOG_VBATT_RPT              dog_vbatt_rpt             /* rpt_def */ 
#define  DOG_QMI_RPT                dog_qmi_rpt               /* rpt_def */ 
#define  DOG_LOC_MIDDLEWARE_RPT     dog_loc_middleware_rpt    /* rpt_def */ 
#define  DOG_DCC_RPT                dog_dcc_rpt               /* rpt_def */ 
#define  DOG_IPA_MAIN_RPT           dog_ipa_main_rpt          /* rpt_def */ 
#define  DOG_IPA_SCHAN_RPT          dog_ipa_schan_rpt         /* rpt_def */ 
#define  DOG_IPA_PDCHAN_RPT         dog_ipa_pdchan_rpt        /* rpt_def */ 
#define  DOG_IPAUKCC_IPC_RPT        dog_ipaukcc_ipc_rpt       /* rpt_def */ 
#define  DOG_RTP_MANAGER_RPT        dog_rtp_manager_rpt       /* rpt_def */ 
#define  DOG_FTM_CAMERA_RPT         dog_ftm_camera_rpt        /* rpt_def */ 
#define  DOG_GFX_RPT                dog_gfx_rpt               /* rpt_def */ 
#define  DOG_TCXOMGR_RPT            dog_tcxomgr_rpt           /* rpt_def */ 
#define  DOG_LSM_APP_PROC_RPT       dog_lsm_app_proc_rpt      /* rpt_def */ 
#define  DOG_HITAPP_RPT             dog_hitapp_rpt            /* rpt_def */ 
#define  DOG_FLASH_IG_SRVR_RPT      dog_flash_ig_srvr_rpt     /* rpt_def */ 
#define  DOG_BT_FS_RPT              dog_bt_fs_rpt             /* rpt_def */ 
#define  DOG_FLUTE_RPT              dog_flute_rpt             /* rpt_def */ 
#define  DOG_IMS_RPT                dog_ims_rpt               /* rpt_def */  
#define  DOG_UM_RPT                 dog_um_rpt                /* rpt_def */ 
#define  DOG_MVSSMMS_RPT            dog_mvssmms_rpt           /* rpt_def */ 
#define  DOG_QTV_DLDSPSVCTASK_RPT   dog_qtv_dldspsvctask_rpt  /* rpt_def */ 
#define  DOG_IMS_CB_RPT             dog_ims_cb_rpt            /* rpt_def */ 
#define  DOG_VIDEOSHARE_RPT         dog_videoshare_rpt        /* rpt_def */ 
#define  DOG_IMSHOM_RPT             dog_imshom_rpt            /* rpt_def */  
#define  DOG_AUDDRV_RPT             dog_auddrv_rpt            /* rpt_def */ 
#define  DOG_WLAN_DS_RPT            dog_wlan_ds_rpt           /* rpt_def */ 
#define  DOG_RF_RPT                 dog_rf_rpt                /* rpt_def */ 
#define  DOG_DS_SIG_RPT             dog_ds_sig_rpt            /* rpt_def */
#define  DOG_HDRDEC_RPT             dog_hdrdec_rpt            /* rpt_def */
/*================================================*/ 
/* END extern rpt defines.... */ 
/*================================================*/ 

/*================================================*/ 
/* BEGIN extern time decl.... For dog.h */ 
/*================================================*/ 
extern const rex_timer_cnt_type   dog_sbcenc_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_qvp_ae_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_qvpio_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_qvppl_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_qvp_mpeg4_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_qvp_app_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_qvp_vfe_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_qvp_rtp_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_sipt_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_gsdi_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_graphics_rpt_time;             /* time_decl */ 
extern const rex_timer_cnt_type   dog_graph_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_gfx_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_camera_drv_rpt_time;           /* time_decl */ 
extern const rex_timer_cnt_type   dog_hdrmc_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_hdrrx_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_hdrtx_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_hdrsrch_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_mmoc_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_tmc_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_uim_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_sec_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_secrnd_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_sfs_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_seccryptarm_rpt_time;          /* time_decl */ 
extern const rex_timer_cnt_type   dog_secips_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_secssl_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_secdrm_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_sfat_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_bt_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_bt_pf_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_qdsp_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_cm_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_wms_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_usbdc_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_usbhc_urb_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_usbhc_dev_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_usbhc_test_rpt_time;           /* time_decl */ 
extern const rex_timer_cnt_type   dog_hwtc_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_pdsm_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_pdsmsclient_rpt_time;          /* time_decl */ 
extern const rex_timer_cnt_type   dog_cd_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_pb_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_gpssrch_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_gpssrch_demod_rpt_time;        /* time_decl */ 
extern const rex_timer_cnt_type   dog_pdsm_comm_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_fs_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_voc_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_tcptest_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_dh_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_vs_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_qmip_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_pd_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_ps_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_dcc_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_wlan_cp_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_wlan_trp_rpt_time;             /* time_decl */ 
extern const rex_timer_cnt_type   dog_wlan_sec_rpt_time;             /* time_decl */ 
extern const rex_timer_cnt_type   dog_auth_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_ds_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_comp_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_diag_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_hs_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_mc_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_nv_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_rx_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_rxtx_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_snd_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_srch_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_tx_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_ui_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_sleep_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_tdso_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_mdsp_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_gcsd_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_gsm_l1_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_gsm_l2_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_l1_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_l2dl_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_l2ul_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_lsm_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_mm_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_mn_cnm_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_plt_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_reg_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_rr_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_rrc_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_sm_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_tc_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_test_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_gllc_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_grlc_ul_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_grlc_dl_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_gmac_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_ftm_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_gstk_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_virtsat_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_mgpmc_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_mgpnf_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_mgpcd_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_mgpai_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_mgpcc_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_mgpmc_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_pp_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_cc_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_pgi_rpt_time;                  /* time_decl */  
extern const rex_timer_cnt_type   dog_nf_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_lm_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_pdcommtcp_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_pdcommwms_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_sm_tm_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_gpsfft_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_loc_middleware_rpt_time;       /* time_decl */ 
extern const rex_timer_cnt_type   dog_cb_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_hdrbc_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_bcdb_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_dtv_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_joyst_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_mac_hs_dl_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_interrupt_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_pbm_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_irda_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_sirius_test_rpt_time;          /* time_decl */ 
extern const rex_timer_cnt_type   dog_aam_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_bm_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_qdiag_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_aps_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_ats_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_efs_srvr_rpt_time;             /* time_decl */ 
extern const rex_timer_cnt_type   dog_pms_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_cnv_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_dmtask_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_qipcall_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_disp_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_mfmc_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_mfpt_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_mfrxd_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_fc_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_adsp_rtos_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_ixhttpsvc_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_ubm_l1_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_qdjtest_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_usbebi2_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_usbapi_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_usbd_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_qtv_video_renderer_rpt_time;   /* time_decl */ 
extern const rex_timer_cnt_type   dog_qtv_audio_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_qtv_task10_rpt_time;           /* time_decl */ 
extern const rex_timer_cnt_type   dog_tcxomgr_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_hsu_al_ms_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_hsu_al_task_rpt_time;          /* time_decl */ 
extern const rex_timer_cnt_type   dog_qtv_dlagenttask_rpt_time;      /* time_decl */ 
extern const rex_timer_cnt_type   dog_sns_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_ukcc_rpt_time;                 /* time_decl */ 
extern const rex_timer_cnt_type   dog_ipa_main_rpt_time;             /* time_decl */ 
extern const rex_timer_cnt_type   dog_ipa_schan_rpt_time;            /* time_decl */ 
extern const rex_timer_cnt_type   dog_ipa_pdchan_rpt_time;           /* time_decl */ 
extern const rex_timer_cnt_type   dog_ipaukcc_ipc_rpt_time;          /* time_decl */ 
extern const rex_timer_cnt_type   dog_rtp_manager_rpt_time;          /* time_decl */ 
extern const rex_timer_cnt_type   dog_chg_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_vbatt_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_ftm_camera_rpt_time;           /* time_decl */ 
extern const rex_timer_cnt_type   dog_um_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_wlan_ds_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_auddrv_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_gpsfs_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_flute_rpt_time;                /* time_decl */
extern const rex_timer_cnt_type   fmrds_dog_rpt_time;                    /* HACK */
extern const rex_timer_cnt_type   dog_lsm_app_proc_rpt_time;         /* time_decl */ 
extern const rex_timer_cnt_type   dog_hitapp_rpt_time;               /* time_decl */ 
extern const rex_timer_cnt_type   dog_flash_ig_srvr_rpt_time;        /* time_decl */ 
extern const rex_timer_cnt_type   dog_bt_fs_rpt_time;                /* time_decl */ 
extern const rex_timer_cnt_type   dog_ims_rpt_time;                  /* time_decl */ 
extern const rex_timer_cnt_type   dog_ims_cb_rpt_time;               /* time_decl */  
extern const rex_timer_cnt_type   dog_mvssmms_rpt_time;              /* time_decl */ 
extern const rex_timer_cnt_type   dog_qtv_dldspsvctask_rpt_time;     /* time_decl */ 
extern const rex_timer_cnt_type   dog_videoshare_rpt_time;           /* time_decl */ 
extern const rex_timer_cnt_type   dog_imshom_rpt_time;               /* time_decl */
extern const rex_timer_cnt_type   dog_rf_rpt_time;                   /* time_decl */ 
extern const rex_timer_cnt_type   dog_hdrdec_rpt_time;               /* time_decl */

/*================================================*/ 
/* END extern time decl.... */ 
/*================================================*/ 

/*================================================*/ 
/* BEGIN extern time defines.... For dog.h */ 
/*================================================*/ 
#define  DOG_SBCENC_RPT_TIME                  dog_sbcenc_rpt_time                 /* time_def */ 
#define  DOG_QVP_AE_RPT_TIME                  dog_qvp_ae_rpt_time                 /* time_def */ 
#define  DOG_QVPIO_RPT_TIME                   dog_qvpio_rpt_time                  /* time_def */ 
#define  DOG_QVPPL_RPT_TIME                   dog_qvppl_rpt_time                  /* time_def */ 
#define  DOG_QVP_MPEG4_RPT_TIME               dog_qvp_mpeg4_rpt_time              /* time_def */ 
#define  DOG_QVP_APP_RPT_TIME                 dog_qvp_app_rpt_time                /* time_def */ 
#define  DOG_QVP_VFE_RPT_TIME                 dog_qvp_vfe_rpt_time                /* time_def */ 
#define  DOG_QVP_RTP_RPT_TIME                 dog_qvp_rtp_rpt_time                /* time_def */ 
#define  DOG_SIPT_RPT_TIME                    dog_sipt_rpt_time                   /* time_def */ 
#define  DOG_GSDI_RPT_TIME                    dog_gsdi_rpt_time                   /* time_def */ 
#define  DOG_GRAPHICS_RPT_TIME                dog_graphics_rpt_time               /* time_def */ 
#define  DOG_GRAPH_RPT_TIME                   dog_graph_rpt_time                  /* time_def */ 
#define  DOG_GFX_RPT_TIME                     dog_gfx_rpt_time                    /* time_def */ 
#define  DOG_CAMERA_DRV_RPT_TIME              dog_camera_drv_rpt_time             /* time_def */ 
#define  DOG_HDRMC_RPT_TIME                   dog_hdrmc_rpt_time                  /* time_def */ 
#define  DOG_HDRRX_RPT_TIME                   dog_hdrrx_rpt_time                  /* time_def */ 
#define  DOG_HDRTX_RPT_TIME                   dog_hdrtx_rpt_time                  /* time_def */ 
#define  DOG_HDRSRCH_RPT_TIME                 dog_hdrsrch_rpt_time                /* time_def */ 
#define  DOG_MMOC_RPT_TIME                    dog_mmoc_rpt_time                   /* time_def */ 
#define  DOG_TMC_RPT_TIME                     dog_tmc_rpt_time                    /* time_def */ 
#define  DOG_UIM_RPT_TIME                     dog_uim_rpt_time                    /* time_def */ 
#define  DOG_SEC_RPT_TIME                     dog_sec_rpt_time                    /* time_def */ 
#define  DOG_SECRND_RPT_TIME                  dog_secrnd_rpt_time                 /* time_def */ 
#define  DOG_SFS_RPT_TIME                     dog_sfs_rpt_time                    /* time_def */ 
#define  DOG_SECCRYPTARM_RPT_TIME             dog_seccryptarm_rpt_time            /* time_def */ 
#define  DOG_SECIPS_RPT_TIME                  dog_secips_rpt_time                 /* time_def */ 
#define  DOG_SECSSL_RPT_TIME                  dog_secssl_rpt_time                 /* time_def */ 
#define  DOG_SECDRM_RPT_TIME                  dog_secdrm_rpt_time                 /* time_def */ 
#define  DOG_SFAT_RPT_TIME                    dog_sfat_rpt_time                   /* time_def */ 
#define  DOG_BT_RPT_TIME                      dog_bt_rpt_time                     /* time_def */ 
#define  DOG_BT_PF_RPT_TIME                   dog_bt_pf_rpt_time                  /* time_def */ 
#define  DOG_QDSP_RPT_TIME                    dog_qdsp_rpt_time                   /* time_def */ 
#define  DOG_CM_RPT_TIME                      dog_cm_rpt_time                     /* time_def */ 
#define  DOG_WMS_RPT_TIME                     dog_wms_rpt_time                    /* time_def */ 
#define  DOG_USBDC_RPT_TIME                   dog_usbdc_rpt_time                  /* time_def */ 
#define  DOG_USBHC_URB_RPT_TIME               dog_usbhc_urb_rpt_time              /* time_def */ 
#define  DOG_USBHC_DEV_RPT_TIME               dog_usbhc_dev_rpt_time              /* time_def */ 
#define  DOG_USBHC_TEST_RPT_TIME              dog_usbhc_test_rpt_time             /* time_def */ 
#define  DOG_HWTC_RPT_TIME                    dog_hwtc_rpt_time                   /* time_def */ 
#define  DOG_PDSM_RPT_TIME                    dog_pdsm_rpt_time                   /* time_def */ 
#define  DOG_PDSMSCLIENT_RPT_TIME             dog_pdsmsclient_rpt_time            /* time_def */ 
#define  DOG_CD_RPT_TIME                      dog_cd_rpt_time                     /* time_def */ 
#define  DOG_NF_RPT_TIME                      dog_nf_rpt_time                     /* time_def */ 
#define  DOG_PB_RPT_TIME                      dog_pb_rpt_time                     /* time_def */ 
#define  DOG_GPSSRCH_RPT_TIME                 dog_gpssrch_rpt_time                /* time_def */ 
#define  DOG_GPSSRCH_DEMOD_RPT_TIME           dog_gpssrch_demod_rpt_time          /* time_def */ 
#define  DOG_PDSM_COMM_RPT_TIME               dog_pdsm_comm_rpt_time              /* time_def */ 
#define  DOG_FS_RPT_TIME                      dog_fs_rpt_time                     /* time_def */ 
#define  DOG_VOC_RPT_TIME                     dog_voc_rpt_time                    /* time_def */ 
#define  DOG_TCPTEST_RPT_TIME                 dog_tcptest_rpt_time                /* time_def */ 
#define  DOG_DH_RPT_TIME                      dog_dh_rpt_time                     /* time_def */ 
#define  DOG_VS_RPT_TIME                      dog_vs_rpt_time                     /* time_def */ 
#define  DOG_QMIP_RPT_TIME                    dog_qmip_rpt_time                   /* time_def */ 
#define  DOG_PD_RPT_TIME                      dog_pd_rpt_time                     /* time_def */ 
#define  DOG_PS_RPT_TIME                      dog_ps_rpt_time                     /* time_def */ 
#define  DOG_DCC_RPT_TIME                     dog_dcc_rpt_time                    /* time_def */ 
#define  DOG_WLAN_CP_RPT_TIME                 dog_wlan_cp_rpt_time                /* time_def */ 
#define  DOG_WLAN_TRP_RPT_TIME                dog_wlan_trp_rpt_time               /* time_def */ 
#define  DOG_WLAN_SEC_RPT_TIME                dog_wlan_sec_rpt_time               /* time_def */ 
#define  DOG_AUTH_RPT_TIME                    dog_auth_rpt_time                   /* time_def */ 
#define  DOG_DS_RPT_TIME                      dog_ds_rpt_time                     /* time_def */ 
#define  DOG_COMP_RPT_TIME                    dog_comp_rpt_time                   /* time_def */ 
#define  DOG_DIAG_RPT_TIME                    dog_diag_rpt_time                   /* time_def */ 
#define  DOG_HS_RPT_TIME                      dog_hs_rpt_time                     /* time_def */ 
#define  DOG_MC_RPT_TIME                      dog_mc_rpt_time                     /* time_def */ 
#define  DOG_NV_RPT_TIME                      dog_nv_rpt_time                     /* time_def */ 
#define  DOG_RX_RPT_TIME                      dog_rx_rpt_time                     /* time_def */ 
#define  DOG_RXTX_RPT_TIME                    dog_rxtx_rpt_time                   /* time_def */ 
#define  DOG_SND_RPT_TIME                     dog_snd_rpt_time                    /* time_def */ 
#define  DOG_SRCH_RPT_TIME                    dog_srch_rpt_time                   /* time_def */ 
#define  DOG_TX_RPT_TIME                      dog_tx_rpt_time                     /* time_def */ 
#define  DOG_UI_RPT_TIME                      dog_ui_rpt_time                     /* time_def */ 
#define  DOG_SLEEP_RPT_TIME                   dog_sleep_rpt_time                  /* time_def */ 
#define  DOG_TDSO_RPT_TIME                    dog_tdso_rpt_time                   /* time_def */ 
#define  DOG_MDSP_RPT_TIME                    dog_mdsp_rpt_time                   /* time_def */ 
#define  DOG_GCSD_RPT_TIME                    dog_gcsd_rpt_time                   /* time_def */ 
#define  DOG_GSM_L1_RPT_TIME                  dog_gsm_l1_rpt_time                 /* time_def */ 
#define  DOG_GSM_L2_RPT_TIME                  dog_gsm_l2_rpt_time                 /* time_def */ 
#define  DOG_L1_RPT_TIME                      dog_l1_rpt_time                     /* time_def */ 
#define  DOG_L2DL_RPT_TIME                    dog_l2dl_rpt_time                   /* time_def */ 
#define  DOG_L2UL_RPT_TIME                    dog_l2ul_rpt_time                   /* time_def */ 
#define  DOG_LSM_RPT_TIME                     dog_lsm_rpt_time                    /* time_def */ 
#define  DOG_MM_RPT_TIME                      dog_mm_rpt_time                     /* time_def */ 
#define  DOG_MN_CNM_RPT_TIME                  dog_mn_cnm_rpt_time                 /* time_def */ 
#define  DOG_PLT_RPT_TIME                     dog_plt_rpt_time                    /* time_def */ 
#define  DOG_REG_RPT_TIME                     dog_reg_rpt_time                    /* time_def */ 
#define  DOG_RR_RPT_TIME                      dog_rr_rpt_time                     /* time_def */ 
#define  DOG_RRC_RPT_TIME                     dog_rrc_rpt_time                    /* time_def */ 
#define  DOG_SM_RPT_TIME                      dog_sm_rpt_time                     /* time_def */ 
#define  DOG_TC_RPT_TIME                      dog_tc_rpt_time                     /* time_def */ 
#define  DOG_TEST_RPT_TIME                    dog_test_rpt_time                   /* time_def */ 
#define  DOG_GLLC_RPT_TIME                    dog_gllc_rpt_time                   /* time_def */ 
#define  DOG_GRLC_UL_RPT_TIME                 dog_grlc_ul_rpt_time                /* time_def */ 
#define  DOG_GRLC_DL_RPT_TIME                 dog_grlc_dl_rpt_time                /* time_def */ 
#define  DOG_GMAC_RPT_TIME                    dog_gmac_rpt_time                   /* time_def */ 
#define  DOG_FTM_RPT_TIME                     dog_ftm_rpt_time                    /* time_def */ 
#define  DOG_GSTK_RPT_TIME                    dog_gstk_rpt_time                   /* time_def */ 
#define  DOG_VIRTSAT_RPT_TIME                 dog_virtsat_rpt_time                /* time_def */ 
#define  DOG_MGPMC_RPT_TIME                   dog_mgpmc_rpt_time                  /* time_def */ 
#define  DOG_MGPNF_RPT_TIME                   dog_mgpnf_rpt_time                  /* time_def */ 
#define  DOG_MGPCD_RPT_TIME                   dog_mgpcd_rpt_time                  /* time_def */ 
#define  DOG_MGPAI_RPT_TIME                   dog_mgpai_rpt_time                  /* time_def */ 
#define  DOG_MGPCC_RPT_TIME                   dog_mgpcc_rpt_time                  /* time_def */  
#define  DOG_PP_RPT_TIME                      dog_pp_rpt_time                     /* time_def */ 
#define  DOG_CC_RPT_TIME                      dog_cc_rpt_time                     /* time_def */ 
#define  DOG_PGI_RPT_TIME                     dog_pgi_rpt_time                    /* time_def */   
#define  DOG_LM_RPT_TIME                      dog_lm_rpt_time                     /* time_def */ 
#define  DOG_PDCOMMTCP_RPT_TIME               dog_pdcommtcp_rpt_time              /* time_def */ 
#define  DOG_PDCOMMWMS_RPT_TIME               dog_pdcommwms_rpt_time              /* time_def */ 
#define  DOG_SM_TM_RPT_TIME                   dog_sm_tm_rpt_time                  /* time_def */ 
#define  DOG_GPSFFT_RPT_TIME                  dog_gpsfft_rpt_time                 /* time_def */ 
#define  DOG_LOC_MIDDLEWARE_RPT_TIME          dog_loc_middleware_rpt_time         /* time_def */ 
#define  DOG_CB_RPT_TIME                      dog_cb_rpt_time                     /* time_def */ 
#define  DOG_HDRBC_RPT_TIME                   dog_hdrbc_rpt_time                  /* time_def */ 
#define  DOG_BCDB_RPT_TIME                    dog_bcdb_rpt_time                   /* time_def */ 
#define  DOG_DTV_RPT_TIME                     dog_dtv_rpt_time                    /* time_def */ 
#define  DOG_JOYST_RPT_TIME                   dog_joyst_rpt_time                  /* time_def */ 
#define  DOG_MAC_HS_DL_RPT_TIME               dog_mac_hs_dl_rpt_time              /* time_def */ 
#define  DOG_INTERRUPT_RPT_TIME               dog_interrupt_rpt_time              /* time_def */ 
#define  DOG_PBM_RPT_TIME                     dog_pbm_rpt_time                    /* time_def */ 
#define  DOG_IRDA_RPT_TIME                    dog_irda_rpt_time                   /* time_def */ 
#define  DOG_SIRIUS_TEST_RPT_TIME             dog_sirius_test_rpt_time            /* time_def */ 
#define  DOG_AAM_RPT_TIME                     dog_aam_rpt_time                    /* time_def */ 
#define  DOG_BM_RPT_TIME                      dog_bm_rpt_time                     /* time_def */ 
#define  DOG_QDIAG_RPT_TIME                   dog_qdiag_rpt_time                  /* time_def */ 
#define  DOG_APS_RPT_TIME                     dog_aps_rpt_time                    /* time_def */ 
#define  DOG_ATS_RPT_TIME                     dog_ats_rpt_time                    /* time_def */ 
#define  DOG_EFS_SRVR_RPT_TIME                dog_efs_srvr_rpt_time               /* time_def */ 
#define  DOG_PMS_RPT_TIME                     dog_pms_rpt_time                    /* time_def */ 
#define  DOG_CNV_RPT_TIME                     dog_cnv_rpt_time                    /* time_def */ 
#define  DOG_DMTASK_RPT_TIME                  dog_dmtask_rpt_time                 /* time_def */ 
#define  DOG_QIPCALL_RPT_TIME                 dog_qipcall_rpt_time                /* time_def */ 
#define  DOG_DISP_RPT_TIME                    dog_disp_rpt_time                   /* time_def */ 
#define  DOG_MFMC_RPT_TIME                    dog_mfmc_rpt_time                   /* time_def */ 
#define  DOG_MFPT_RPT_TIME                    dog_mfpt_rpt_time                   /* time_def */ 
#define  DOG_MFRXD_RPT_TIME                   dog_mfrxd_rpt_time                  /* time_def */ 
#define  DOG_FC_RPT_TIME                      dog_fc_rpt_time                     /* time_def */ 
#define  DOG_ADSP_RTOS_RPT_TIME               dog_adsp_rtos_rpt_time              /* time_def */ 
#define  DOG_IXHTTPSVC_RPT_TIME               dog_ixhttpsvc_rpt_time              /* time_def */ 
#define  DOG_UBM_L1_RPT_TIME                  dog_ubm_l1_rpt_time                 /* time_def */ 
#define  DOG_QDJTEST_RPT_TIME                 dog_qdjtest_rpt_time                /* time_def */ 
#define  DOG_USBEBI2_RPT_TIME                 dog_usbebi2_rpt_time                /* time_def */ 
#define  DOG_USBAPI_RPT_TIME                  dog_usbapi_rpt_time                 /* time_def */ 
#define  DOG_USBD_RPT_TIME                    dog_usbd_rpt_time                   /* time_def */ 
#define  DOG_QTV_VIDEO_RENDERER_RPT_TIME      dog_qtv_video_renderer_rpt_time     /* time_def */ 
#define  DOG_QTV_AUDIO_RPT_TIME               dog_qtv_audio_rpt_time              /* time_def */ 
#define  DOG_QTV_TASK10_RPT_TIME              dog_qtv_task10_rpt_time             /* time_def */ 
#define  DOG_TCXOMGR_RPT_TIME                 dog_tcxomgr_rpt_time                /* time_def */ 
#define  DOG_HSU_AL_MS_RPT_TIME               dog_hsu_al_ms_rpt_time              /* time_def */ 
#define  DOG_HSU_AL_TASK_RPT_TIME             dog_hsu_al_task_rpt_time            /* time_def */ 
#define  DOG_QTV_DLAGENTTASK_RPT_TIME         dog_qtv_dlagenttask_rpt_time        /* time_def */ 
#define  DOG_SNS_RPT_TIME                     dog_sns_rpt_time                    /* time_def */ 
#define  DOG_UKCC_RPT_TIME                    dog_ukcc_rpt_time                   /* time_def */ 
#define  DOG_IPA_MAIN_RPT_TIME                dog_ipa_main_rpt_time               /* time_def */ 
#define  DOG_IPA_SCHAN_RPT_TIME               dog_ipa_schan_rpt_time              /* time_def */ 
#define  DOG_IPA_PDCHAN_RPT_TIME              dog_ipa_pdchan_rpt_time             /* time_def */ 
#define  DOG_IPAUKCC_IPC_RPT_TIME             dog_ipaukcc_ipc_rpt_time            /* time_def */ 
#define  DOG_RTP_MANAGER_RPT_TIME             dog_rtp_manager_rpt_time            /* time_def */ 
#define  DOG_CHG_RPT_TIME                     dog_chg_rpt_time                    /* time_def */ 
#define  DOG_VBATT_RPT_TIME                   dog_vbatt_rpt_time                  /* time_def */ 
#define  DOG_FTM_CAMERA_RPT_TIME              dog_ftm_camera_rpt_time             /* time_def */ 
#define  DOG_UM_RPT_TIME                      dog_um_rpt_time                     /* time_def */ 
#define  DOG_WLAN_DS_RPT_TIME                 dog_wlan_ds_rpt_time                /* time_def */ 
#define  DOG_AUDDRV_RPT_TIME                  dog_auddrv_rpt_time                 /* time_def */ 
#define  DOG_GPSFS_RPT_TIME                   dog_gpsfs_rpt_time                  /* time_def */ 
#define  DOG_FLUTE_RPT_TIME                   dog_flute_rpt_time                  /* time_def */ 
#define  FMRDS_DOG_RPT_TIME                      fmrds_dog_rpt_time                     /* HACK */ 
#define  DOG_LSM_APP_PROC_RPT_TIME            dog_lsm_app_proc_rpt_time           /* time_def */ 
#define  DOG_HITAPP_RPT_TIME                  dog_hitapp_rpt_time                 /* time_def */ 
#define  DOG_FLASH_IG_SRVR_RPT_TIME           dog_flash_ig_srvr_rpt_time          /* time_def */  
#define  DOG_BT_FS_RPT_TIME                   dog_bt_fs_rpt_time                  /* time_def */ 
#define  DOG_IMS_RPT_TIME                     dog_ims_rpt_time                    /* time_def */ 
#define  DOG_IMS_CB_RPT_TIME                  dog_ims_cb_rpt_time                 /* time_def */ 
#define  DOG_MVSSMMS_RPT_TIME                 dog_mvssmms_rpt_time                /* time_def */ 
#define  DOG_QTV_DLDSPSVCTASK_RPT_TIME        dog_qtv_dldspsvctask_rpt_time       /* time_def */ 
#define  DOG_VIDEOSHARE_RPT_TIME              dog_videoshare_rpt_time             /* time_def */ 
#define  DOG_IMSHOM_RPT_TIME                  dog_imshom_rpt_time                 /* time_def */ 
#define  DOG_RF_RPT_TIME                      dog_rf_rpt_time                     /* time_def */ 
#define  DOG_HDRDEC_RPT_TIME                  dog_hdrdec_rpt_time                 /* time_def */
/*================================================*/ 
/* END extern time defs.... */ 
/*================================================*/ 

/******************************************************************
|\     /|(  ___  )(  ____ )( (    /|\__   __/( (    /|(  ____ \( )
| )   ( || (   ) || (    )||  \  ( |   ) (   |  \  ( || (    \/| |
| | _ | || (___) || (____)||   \ | |   | |   |   \ | || |      | |
| |( )| ||  ___  ||     __)| (\ \) |   | |   | (\ \) || | ____ | |
| || || || (   ) || (\ (   | | \   |   | |   | | \   || | \_  )(_)
| () () || )   ( || ) \ \__| )  \  |___) (___| )  \  || (___) | _ 
(_______)|/     \||/   \__/|/    )_)\_______/|/    )_)(_______)(_)

LEGACY DOG IS DEPRECATED.  DO NOT EDIT. ALL CHANGES WILL BE REJECTED
******************************************************************/
/* ===============================================*/
/* END: LEGACY DOG CHANGES                        */
/* ===============================================*/

/** @} */  /* end_name Legacy Watchdog Values */ 


/** Supported watchdog monitor states.
*/
typedef enum {
  DOG_RESET_MONITOR,        /**< Resets the montior. */
  DOG_TURN_OFF_MONITOR      /**< Turns off the montior. */
} dog_monitor_state;


/** Supported deadlock types.
*/
typedef enum {
  DOG_DEADLKCHK_DISABLE   = 0,      /**< Disable. */
  DOG_DEADLKCHK_ENABLE    = 1,      /**< Enable. */
  DOG_DEADLKCHK_UNDEF     = 0xff,   /**< Undefined. */
} dog_deadlockcheck_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/**
  Enables/disables the software watchdog feature in the watchdog task so
  dog_task() counts do not decrement or expire. Use of this function is highly 
  discouraged and is not supported by all targets.
  
  @param[in] value      Desired state change.   
  @param[in] ignore     Argument is ignored.
  
  @return 
  TRUE -- Successfully set the watchdog to disable.\n
  FALSE -- Unsuccessful in setting the watchdog.
  
  @dependencies
  None.
*/
extern boolean set_arm9_sw_watchdog_state
(
  uint32 *value, uint32 *ignore
);


/**
  Disables the watchdog feature in the watchdog task so task counts do not 
  decrement or expire. dog_task() still runs but does not expire.

  @param[in] disable    Flag used to toggle watchdog monitoring:\n
                        TRUE -- Disables watchdog monitoring.\n 
                        FALSE -- Enables watchdog monitoring.
                        
  @return
  None.
  
  @dependencies
  None.
  
  @sideeffects
  If watchdog monitoring is disabled, the watchdog does not monitor starved 
  tasks (tasks that do not get enough CPU cycles).
*/
extern void dog_task_disable
(
  boolean disable
);

/**
  Reports to the watchdog task whether the monitored task is 
  functioning properly.

  @param[in] report    Dog handle used to tell the watchdog which task is 
                       reporting.
                        
  @return
  None.
  
  @dependencies
  None.
*/
extern void dog_report
(
  dog_report_type report
);


/**
  Enables/disables the autokick feature for Global System for Mobile (GSM)
  communications only. Autokick is used when the microprocessor is expected 
  to be asleep.

  @param[in] enable    Flag that determines whether to enable GSM autokick.
                        
  @return
  None.
  
  @dependencies
  This function is to be called only when the sleep timer has been programmed
  or when the warm-up timer has expired.
  
  @sideeffects
  When enabled, the function pauses the watchdog wakeup timer. The timer 
  resumes when autokick is disabled via this function.
*/
void dog_gsm_autokick
(
  boolean enable
);


/**
  Enables/disables the autokick feature. Autokick is used when the 
  microprocessor is expected to be asleep.

  @param[in] enable  Flag used to toggle autokick:\n
                     TRUE -- Enables autokick.\n
                     FALSE -- Disables autokick.
                        
  @return
  None.
  
  @dependencies
  This function is to be called only when the sleep timer has been programmed
  or when the warmup timer has expired.
  
  @sideeffects
  When enabled, the function pauses the watchdog wakeup timer. The timer 
  resumes when autokick is disabled via this function.
*/
extern void dog_autokick
(
  boolean enable
);


/**
  Resets the watchdog timer circuit. This function is not used when the 
  microprocessor is functioning normally. It is used during unusual 
  startup/shutdown conditions.
                        
  @return
  None.
  
  @dependencies
  Assumes that system is in known state, since function uses timers/locking.
  If this assumption can't be met, use dog_force_kick
*/
extern void dog_kick( void );

/**
  Resets the watchdog timer circuit. This function is a direct register
  write, to be used in extraordinary situations (e.g. err handling)
                        
  @return
  None.
  
  @dependencies
  None.
*/
extern void dog_force_kick( void );

/**
  Contains the main processing loop for the watchdog task.
  This function checks that each task has reported in during that task's report
  interval. If a task has not reported, dog_task() calls the macro 
  for fatal errors to log the error and reset the microprocessor.

  @param[in] param    Least significant OS signal type portion of this dword is
                      a mask of the task report signals to be monitored. This 
                      argument is currently ignored.
                        
  @return
  None.
  
  @dependencies
  This function is called by the OS task registration function. Each task must 
  report in at least once during each task report interval, which is defined 
  in the watchdog table variable.
  
  @sideeffects
  This function may reset the processor.
*/
extern void dog_task
(
  dword param
);

/**
  Pauses the monitoring of a specified task.

  @note1hang
  The kernel calls dog_monitor_resume() by default. Calling 
  dog_deregister() or dog_register() to pause or resume monitoring provides full 
  control over the task. 

  @param[in] report_id    Dog handle that the watchdog uses to identify the task.
                        
  @return
  None.
  
  @dependencies
  None.
*/
extern void dog_monitor_pause(
   unsigned int report_id
);

/**
  Resumes the monitoring of a specified task.

  @note1hang
  The kernel calls dog_monitor_resume() by default. Calling dog_deregister() to 
  pause monitoring provides full control over the task. 

  @param[in] report_id    Dog handle that the watchdog uses to identify the task.
                        
  @return
  None.
  
  @dependencies
  None.
*/
extern void dog_monitor_resume(
   unsigned int report_id
);

/**
  Indicates to the watchdog how long to wait in milliseconds before starting to 
  monitor tasks. 

  @param[in] startup_time    Time in milliseconds to delay before monitoring 
                             tasks.
                        
  @return
  None.
  
  @dependencies
  None.
  
  @sideeffects
  Setting a long delay before the startup time (using a large number for 
  startup_time) can result in malfunctions being caught later than usual.
*/
void dog_set_startup_time(
   uint32 startup_time
);



/**
Signifies that the watchdog has the crash report feature enabled.
*/
#define DOG_HAS_CRASH_REPORT_API

/**
  Uses the provided function pointer to print a text dump of the watchdog table 
  variable.

  @param[in] print_ptr    Function pointer called with the text dump.
                        
  @return
  None.
  
  @dependencies
  None.
*/
void dog_crash_report(
  void (*print_ptr)(char *)
);


/**
  Dynamically registers tasks for watchdog services. A replacement for hard coded 
  timeouts in dog.c for common modem interface (CMI) compliance.

  @param[in] p_tcb                Pointer to the Task Control Block (TCB).
  @param[in] timeout              Desired timeout in milliseconds
  @param[in] deadlockcheck_flag   Deadlock check. This argument is currently ignored.
                        
  @return
  Handle for tasks to be identified with the watchdog.
  
  @dependencies
  None.
*/
extern dog_report_type dog_register(
  rex_tcb_type            *p_tcb,
  uint32                   timeout,
  dog_deadlockcheck_type   deadlockcheck_flag  /* Not yet supported */
);

/**
  Returns the number of milliseconds between expected calls to dog_report(). 
  This duration is less than the watchdog timeout period by a safety margin.

  @param[in] dog_report_id   Dog handle that the watchdog uses to identify the task.
                        
  @return
  Period in milliseconds during which tasks can call dog_report(). If the period is 0,
  dog_report() is not to be called.
  
  @dependencies
  None.
*/
extern uint32 dog_get_report_period(
  dog_report_type dog_report_id
);


/**
  Dynamically de-registers tasks for watchdog services for CMI compliance.

  @param[in] dog_report_id   Dog handle that the watchdog uses to identify the task.
                        
  @return
  None.
  
  @dependencies
  None.
*/
extern void dog_deregister(
  dog_report_type dog_report_id
);

/** @} */ /* end_addtogroup dog_api */


#endif  /* DOG_H */

