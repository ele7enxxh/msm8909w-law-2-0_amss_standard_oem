
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    W A T C H D O G     T A S K

GENERAL DESCRIPTION
  The Watchdog task monitors the other software tasks to ensure that they
  execute properly and prevents the watchdog timer from resetting the
  microprocessor.

EXTERNALIZED FUNCTIONS
  dog_report
    Report to the Watchdog task, so that it can tell that the monitored
    task is still functioning properly.
  dog_autokick
    This routine enables/disables the hardware watchdog auto-kick function.
  dog_kick
    This routine kicks the hardware watchdog.  This is not to be used except
    in unusual start-up/shutdown conditions.
  dog_task
    This is the entry procedure for the Watchdog task called by rex_def_task.
  dog_register
    This is for new tasks to request dog monitoring
  dog_get_report_period
    This is how new tasks determine their dog report timer period.
  dog_deregister
    This is how registered tasks inform dog to cease monitoring them. 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Dog_task must be defined (rex_def_task) prior to all other tasks which
  are to be monitored, and signaled to start after the last task has started.
  The reason for doing this is to allow the Watchdog task to start a timer
  when the task is initialized.  If the timer expires before the task is
  started, the Watchdog task initiates fatal error processing.

Copyright (c) 2002 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/dog/src/dog_stubs.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/


#include "rex.h"
#include "err.h"
#warning "dog_stubs.c : Not delivered by DnT. Stubbed by Products team"

//cannot include dog.h, so copy necessary data types 

typedef unsigned int dog_report_type;
typedef enum {
  DOG_UNDEFINED            = 0,
  DOG_LEGACY_TIMEOUT       = 1,
  DOG_FAST_TIMEOUT         = 2,
  DOG_DEFAULT_TIMEOUT      = 0xff,
} dog_timeout_enum_type;

typedef enum {
  DOG_DEADLKCHK_DISABLE   = 0,
  DOG_DEADLKCHK_ENABLE    = 1,
  DOG_DEADLKCHK_UNDEF     = 0xff,
} dog_deadlockcheck_type;

#define DOG_WAKEUP_TIMER_SIG    0x0800

/*================================================*/ 
/* BEGIN rpt decl.... For dog_h */ 
/*================================================*/ 
 dog_report_type dog_snd_rpt;              
 dog_report_type dog_tx_rpt;               
 dog_report_type dog_rx_rpt;               
 dog_report_type dog_srch_rpt;             
 dog_report_type dog_rxtx_rpt;             
 dog_report_type dog_mc_rpt;               
 dog_report_type dog_hs_rpt;               
 dog_report_type dog_diag_rpt;             
 dog_report_type dog_ui_rpt;               
 dog_report_type dog_nv_rpt;               
 dog_report_type dog_auth_rpt;             
 dog_report_type dog_ds_rpt;               
 dog_report_type dog_ps_rpt;               
 dog_report_type dog_tcptest_rpt;          
 dog_report_type dog_sleep_rpt;            
 dog_report_type dog_qmip_rpt;             
 dog_report_type dog_dh_rpt;               
 dog_report_type dog_vs_rpt;               
 dog_report_type dog_graph_rpt;            
 dog_report_type dog_fs_rpt;               
 dog_report_type dog_cm_rpt;               
 dog_report_type dog_voc_rpt;              
 dog_report_type dog_pdsm_rpt;             
 dog_report_type dog_pdsmsclient_rpt;      
 dog_report_type dog_cd_rpt;                           
 dog_report_type dog_pb_rpt;               
 dog_report_type dog_qdsp_rpt;             
 dog_report_type dog_usbdc_rpt;            
 dog_report_type dog_sfat_rpt;             
 dog_report_type dog_bt_rpt;               
 dog_report_type dog_tdso_rpt;             
 dog_report_type dog_sec_rpt;              
 dog_report_type dog_uim_rpt;              
 dog_report_type dog_hdrmc_rpt;            
 dog_report_type dog_hdrrx_rpt;            
 dog_report_type dog_hdrtx_rpt;            
 dog_report_type dog_hdrsrch_rpt;          
 dog_report_type dog_hwtc_rpt;             
 dog_report_type dog_wms_rpt;              
 dog_report_type dog_tmc_rpt;              
 dog_report_type dog_mmoc_rpt;             
 dog_report_type dog_gsdi_rpt;             
 dog_report_type dog_mdsp_rpt;             
 dog_report_type dog_gcsd_rpt;             
 dog_report_type dog_gsm_l1_rpt;           
 dog_report_type dog_gsm_l2_rpt;           
 dog_report_type dog_gllc_rpt;             
 dog_report_type dog_grlc_ul_rpt;          
 dog_report_type dog_grlc_dl_rpt;          
 dog_report_type dog_gmac_rpt;             
 dog_report_type dog_l1_rpt;               
 dog_report_type dog_l2dl_rpt;             
 dog_report_type dog_l2ul_rpt;             
 dog_report_type dog_lsm_rpt;              
 dog_report_type dog_mn_cnm_rpt;           
 dog_report_type dog_mm_rpt;               
 dog_report_type dog_plt_rpt;              
 dog_report_type dog_test_rpt;             
 dog_report_type dog_reg_rpt;              
 dog_report_type dog_rr_rpt;               
 dog_report_type dog_rrc_rpt;              
 dog_report_type dog_sm_rpt;               
 dog_report_type dog_tc_rpt;               
 dog_report_type dog_gpssrch_demod_rpt;    
 dog_report_type dog_ftm_rpt;              
 dog_report_type dog_gstk_rpt;             
 dog_report_type dog_virtsat_rpt;          
 dog_report_type dog_secssl_rpt;                     
 dog_report_type dog_mgpnf_rpt;            
 dog_report_type dog_mgpcd_rpt;            
 dog_report_type dog_mgpai_rpt;            
 dog_report_type dog_interrupt_rpt;        
 dog_report_type dog_seccryptarm_rpt;      
 dog_report_type dog_secips_rpt;           
 dog_report_type dog_qvp_ae_rpt;           
 dog_report_type dog_qvpio_rpt;            
 dog_report_type dog_qvppl_rpt;            
 dog_report_type dog_qvp_app_rpt;          
 dog_report_type dog_qvp_mpeg4_rpt;        
 dog_report_type dog_gpssrch_rpt;          
 dog_report_type dog_cb_rpt;               
 dog_report_type dog_secdrm_rpt;           
 dog_report_type dog_bt_pf_rpt;            
 dog_report_type dog_hdrbc_rpt;            
 dog_report_type dog_bcdb_rpt;             
 dog_report_type dog_joyst_rpt;            
 dog_report_type dog_dtv_rpt;              
 dog_report_type dog_mac_hs_dl_rpt;        
 dog_report_type dog_pbm_rpt;              
 dog_report_type dog_qvp_vfe_rpt;          
 dog_report_type dog_mgpcc_rpt;            
 dog_report_type dog_irda_rpt;             
 dog_report_type dog_sirius_test_rpt;      
 dog_report_type dog_aam_rpt;              
 dog_report_type dog_bm_rpt;               
 dog_report_type dog_sipt_rpt;             
 dog_report_type dog_qvp_rtp_rpt;          
 dog_report_type dog_wlan_cp_rpt;          
 dog_report_type dog_wlan_trp_rpt;         
 dog_report_type dog_fmrds_rpt;            
 dog_report_type dog_qdiag_rpt;            
 dog_report_type dog_aps_rpt;              
 dog_report_type dog_efs_srvr_rpt;         
 dog_report_type dog_pms_rpt;              
 dog_report_type dog_cnv_rpt;              
 dog_report_type dog_qipcall_rpt;          
 dog_report_type dog_mfmc_rpt;             
 dog_report_type dog_mfpt_rpt;             
 dog_report_type dog_mfrxd_rpt;            
 dog_report_type dog_wlan_sec_rpt;         
 dog_report_type dog_disp_rpt;             
 dog_report_type dog_usbhc_urb_rpt;        
 dog_report_type dog_usbhc_dev_rpt;        
 dog_report_type dog_usbhc_test_rpt;       
 dog_report_type dog_sbcenc_rpt;           
 dog_report_type dog_camera_drv_rpt;       
 dog_report_type dog_adsp_rtos_rpt;        
 dog_report_type dog_graphics_rpt;         
 dog_report_type dog_ixhttpsvc_rpt;        
 dog_report_type dog_ats_rpt;              
 dog_report_type dog_comp_rpt;             
 dog_report_type dog_pdsm_comm_rpt;        
 dog_report_type dog_secrnd_rpt;           
 dog_report_type dog_ubm_l1_rpt;           
 dog_report_type dog_qdjtest_rpt;          
 dog_report_type dog_sfs_rpt;              
 dog_report_type dog_dmtask_rpt;           
 dog_report_type dog_fc_rpt;               
 dog_report_type dog_qtv_video_renderer_rpt; 
 dog_report_type dog_qtv_audio_rpt;        
 dog_report_type dog_usbebi2_rpt;          
 dog_report_type dog_usbapi_rpt;           
 dog_report_type dog_usbd_rpt;             
 dog_report_type dog_qtv_task10_rpt;       
 dog_report_type dog_hsu_al_ms_rpt;        
 dog_report_type dog_hsu_al_task_rpt;      
 dog_report_type dog_mgpmc_rpt;            
 dog_report_type dog_pp_rpt;               
 dog_report_type dog_cc_rpt;                          
 dog_report_type dog_nf_rpt;               
 dog_report_type dog_gpsfft_rpt;                
 dog_report_type dog_lm_rpt;               
 dog_report_type dog_sm_tm_rpt;            
 dog_report_type dog_pdcommtcp_rpt;        
 dog_report_type dog_pdcommwms_rpt;        
 dog_report_type dog_qtv_dlagenttask_rpt;  
 dog_report_type dog_sns_rpt;              
 dog_report_type dog_pgi_rpt;              
 dog_report_type dog_ukcc_rpt;             
 dog_report_type dog_chg_rpt;
 dog_report_type dog_vbatt_rpt;            
 dog_report_type dog_qmi_rpt;              
 dog_report_type dog_loc_middleware_rpt;   
 dog_report_type dog_dcc_rpt;              
 dog_report_type dog_ipa_main_rpt;         
 dog_report_type dog_ipa_schan_rpt;        
 dog_report_type dog_ipa_pdchan_rpt;       
 dog_report_type dog_ipaukcc_ipc_rpt;      
 dog_report_type dog_rtp_manager_rpt;      
 dog_report_type dog_ftm_camera_rpt;       
 dog_report_type dog_gfx_rpt;              
 dog_report_type dog_tcxomgr_rpt;              
 dog_report_type dog_lsm_app_proc_rpt;     
 dog_report_type dog_hitapp_rpt;           
 dog_report_type dog_flash_ig_srvr_rpt;    
 dog_report_type dog_bt_fs_rpt;            
 dog_report_type dog_flute_rpt;            
 dog_report_type dog_ims_rpt;                    
 dog_report_type dog_um_rpt;               
 dog_report_type dog_mvssmms_rpt;          
 dog_report_type dog_qtv_dldspsvctask_rpt; 
 dog_report_type dog_ims_cb_rpt;           
 dog_report_type dog_videoshare_rpt;       
 dog_report_type dog_imshom_rpt;                   
 dog_report_type dog_gpsfft_rpt;                            
 dog_report_type dog_auddrv_rpt;           
 dog_report_type dog_wlan_ds_rpt;          
 dog_report_type dog_rf_rpt;
 dog_report_type dog_ds_sig_rpt;
 dog_report_type dog_hdrdec_rpt;

/*================================================*/ 
/* END  rpt decl.... */ 
/*================================================*/ 

/*================================================*/ 
/* BEGIN  time decl.... For dog.h */ 
/*================================================*/ 
 const rex_timer_cnt_type   dog_sbcenc_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_qvp_ae_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_qvpio_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_qvppl_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_qvp_mpeg4_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_qvp_app_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_qvp_vfe_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_qvp_rtp_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_sipt_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_gsdi_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_graphics_rpt_time;             /* time_decl */ 
 const rex_timer_cnt_type   dog_graph_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_gfx_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_camera_drv_rpt_time;           /* time_decl */ 
 const rex_timer_cnt_type   dog_hdrmc_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_hdrrx_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_hdrtx_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_hdrsrch_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_mmoc_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_tmc_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_uim_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_sec_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_secrnd_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_sfs_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_seccryptarm_rpt_time;          /* time_decl */ 
 const rex_timer_cnt_type   dog_secips_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_secssl_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_secdrm_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_sfat_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_bt_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_bt_pf_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_qdsp_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_cm_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_wms_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_usbdc_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_usbhc_urb_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_usbhc_dev_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_usbhc_test_rpt_time;           /* time_decl */ 
 const rex_timer_cnt_type   dog_hwtc_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_pdsm_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_pdsmsclient_rpt_time;          /* time_decl */ 
 const rex_timer_cnt_type   dog_cd_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_pb_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_gpssrch_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_gpssrch_demod_rpt_time;        /* time_decl */ 
 const rex_timer_cnt_type   dog_pdsm_comm_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_fs_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_voc_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_tcptest_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_dh_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_vs_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_qmip_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_pd_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_ps_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_dcc_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_wlan_cp_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_wlan_trp_rpt_time;             /* time_decl */ 
 const rex_timer_cnt_type   dog_wlan_sec_rpt_time;             /* time_decl */ 
 const rex_timer_cnt_type   dog_auth_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_ds_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_comp_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_diag_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_hs_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_mc_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_nv_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_rx_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_rxtx_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_snd_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_srch_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_tx_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_ui_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_sleep_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_tdso_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_mdsp_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_gcsd_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_gsm_l1_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_gsm_l2_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_l1_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_l2dl_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_l2ul_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_lsm_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_mm_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_mn_cnm_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_plt_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_reg_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_rr_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_rrc_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_sm_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_tc_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_test_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_gllc_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_grlc_ul_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_grlc_dl_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_gmac_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_ftm_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_gstk_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_virtsat_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_mgpmc_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_mgpnf_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_mgpcd_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_mgpai_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_mgpcc_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_mgpmc_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_pp_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_cc_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_pgi_rpt_time;                  /* time_decl */  
 const rex_timer_cnt_type   dog_nf_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_lm_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_pdcommtcp_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_pdcommwms_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_sm_tm_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_gpsfft_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_loc_middleware_rpt_time;       /* time_decl */ 
 const rex_timer_cnt_type   dog_cb_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_hdrbc_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_bcdb_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_dtv_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_joyst_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_mac_hs_dl_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_interrupt_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_pbm_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_irda_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_sirius_test_rpt_time;          /* time_decl */ 
 const rex_timer_cnt_type   dog_aam_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_bm_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_qdiag_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_aps_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_ats_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_efs_srvr_rpt_time;             /* time_decl */ 
 const rex_timer_cnt_type   dog_pms_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_cnv_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_dmtask_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_qipcall_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_disp_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_mfmc_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_mfpt_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_mfrxd_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_fc_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_adsp_rtos_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_ixhttpsvc_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_ubm_l1_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_qdjtest_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_usbebi2_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_usbapi_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_usbd_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_qtv_video_renderer_rpt_time;   /* time_decl */ 
 const rex_timer_cnt_type   dog_qtv_audio_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_qtv_task10_rpt_time;           /* time_decl */ 
 const rex_timer_cnt_type   dog_tcxomgr_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_hsu_al_ms_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_hsu_al_task_rpt_time;          /* time_decl */ 
 const rex_timer_cnt_type   dog_qtv_dlagenttask_rpt_time;      /* time_decl */ 
 const rex_timer_cnt_type   dog_sns_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_ukcc_rpt_time;                 /* time_decl */ 
 const rex_timer_cnt_type   dog_ipa_main_rpt_time;             /* time_decl */ 
 const rex_timer_cnt_type   dog_ipa_schan_rpt_time;            /* time_decl */ 
 const rex_timer_cnt_type   dog_ipa_pdchan_rpt_time;           /* time_decl */ 
 const rex_timer_cnt_type   dog_ipaukcc_ipc_rpt_time;          /* time_decl */ 
 const rex_timer_cnt_type   dog_rtp_manager_rpt_time;          /* time_decl */ 
 const rex_timer_cnt_type   dog_chg_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_vbatt_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_ftm_camera_rpt_time;           /* time_decl */ 
 const rex_timer_cnt_type   dog_um_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_wlan_ds_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_auddrv_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_gpsfs_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_flute_rpt_time;                /* time_decl */
 const rex_timer_cnt_type   fmrds_dog_rpt_time;                    /* HACK */
 const rex_timer_cnt_type   dog_lsm_app_proc_rpt_time;         /* time_decl */ 
 const rex_timer_cnt_type   dog_hitapp_rpt_time;               /* time_decl */ 
 const rex_timer_cnt_type   dog_flash_ig_srvr_rpt_time;        /* time_decl */ 
 const rex_timer_cnt_type   dog_bt_fs_rpt_time;                /* time_decl */ 
 const rex_timer_cnt_type   dog_ims_rpt_time;                  /* time_decl */ 
 const rex_timer_cnt_type   dog_ims_cb_rpt_time;               /* time_decl */  
 const rex_timer_cnt_type   dog_mvssmms_rpt_time;              /* time_decl */ 
 const rex_timer_cnt_type   dog_qtv_dldspsvctask_rpt_time;     /* time_decl */ 
 const rex_timer_cnt_type   dog_videoshare_rpt_time;           /* time_decl */ 
 const rex_timer_cnt_type   dog_imshom_rpt_time;               /* time_decl */
 const rex_timer_cnt_type   dog_rf_rpt_time;                   /* time_decl */ 
 const rex_timer_cnt_type   dog_hdrdec_rpt_time;               /* time_decl */

/*================================================*/ 
/* END  time decl.... */ 
/*================================================*/ 


/*===========================================================================

FUNCTION set_arm9_sw_watchdog_state

DESCRIPTION
  This procedure will alter watchdog state to either disable watchdog
  monitoring, or halt the modem entirely, as determined by a remote proc.

DEPENDENCIES
  None

RETURN VALUE
  Always returns true as proc_comm requires return value.

SIDE EFFECTS
  Remote proc will block until this returns.

===========================================================================*/
boolean set_arm9_sw_watchdog_state
(
  uint32 *state, uint32 *ignore
)
{
    return TRUE; // command was executed successfully
}

/*===========================================================================

FUNCTION DOG_TASK_DISABLE

DESCRIPTION
  This procedure will disable software watchdog feature in watchdog task so
  taks counts will not decrement or expire.  Dog_task will still run but not
  expire.

DEPENDENCIES
  This should only be called when we have received a debugbreak notification
  from ARM11.

RETURN VALUE
  None

SIDE EFFECTS
  This will prevent the software watchdog from decrementing timeouts for sw tasks
  in the sw dog's list.

===========================================================================*/
void dog_task_disable
(
  boolean disable
)
{

}


/*===========================================================================

FUNCTION DOG_REPORT

DESCRIPTION
  Report to the Watchdog task, so that it can tell that the monitored
  task is still functioning properly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dog_report
(
  dog_report_type report_id
    /* Tell the Watchdog task which task is reporting */
)
{



} /* end of dog_report */


/*===========================================================================

FUNCTION DOG_SET_AUTOKICKING

DESCRIPTION
  This procedure sets the autokicking state for the dog task.

DEPENDENCIES
  Only call when in an INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  Enables or disables the dog_wakeup_timer, depending on autokick state.

===========================================================================*/
LOCAL void dog_set_autokicking
(
  boolean enable
)
{



} /* end of dog_set_autokicking */


/*===========================================================================

FUNCTION DOG_GSM_AUTOKICK

DESCRIPTION
  This procedure enables/disables the auto-kick feature for GSM only.
  Auto-kick is used when the microprocessor is expected to be asleep.

DEPENDENCIES
  This should only be called when the sleep timer has been programmed
  or when the warm-up timer has expired.

RETURN VALUE
  None

SIDE EFFECTS
  When enabled, pauses the watchdog's wakeup timer.  The timer resumes
  when autokick is disabled via this function.

===========================================================================*/
void dog_gsm_autokick
( /*lint -esym(715, enable) *//* Symbol enable is not used */
  boolean enable
)
{

} /* end of dog_gsm_autokick *//*lint +esym(715, enable) */


/*===========================================================================

FUNCTION DOG_AUTOKICK

DESCRIPTION
  This procedure enables/disables the auto-kick feature.  Auto-kick
  is used when the microprocessor is expected to be asleep.

DEPENDENCIES
  This should only be called when the sleep timer has been programmed
  or when the warm-up timer has expired.

RETURN VALUE
  None

SIDE EFFECTS
  When enabled, pauses the watchdog's wakeup timer.  The timer resumes
  when autokick is disabled via this function.

===========================================================================*/
void dog_autokick
(
  boolean enable
)
{



} /* end of dog_autokick */





/*===========================================================================

FUNCTION DOG_KICK

DESCRIPTION
  This procedure resets the watchdog timer circuit.  This is
  not to be used except in unusual start-up/shutdown conditions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the circuit so that we have another N milliseconds before
  the circuit will reset the system.  Arms auto-kick.

===========================================================================*/
void dog_kick( void )
{



} /* end of dog_kick */

/*===========================================================================

FUNCTION DOG_FORCE_KICK

DESCRIPTION
  This procedure resets the watchdog timer circuit.  This function is a wrapper
  for a direct hardware register write, to be used in exceptional situations 
  (err handling, boot, etc)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the circuit so that we have another N milliseconds before
  the circuit will reset the system.

===========================================================================*/
void dog_force_kick( void )
{

}

/*===========================================================================

FUNCTION DOG_SHUTDOWN

DESCRIPTION
  This procedure performs Watchdog task shutdown processing. It processes
  task stop procedure and then returns.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dog_shutdown( void )
{


} /* end of dog_shutdown */




/*===========================================================================

FUNCTION dog_wait

DESCRIPTION
  Wait function for use by the Dog task. Calls rex_wait with the signal mask
  passed in, but kicks the hardware dog periodically in addition. The function
  also takes a timeout parameter that determines the maximum duration of the
  wait in milliseconds. If this a parameter is 0, then there is no timeout.

===========================================================================*/
rex_sigs_type dog_wait (rex_sigs_type sigs, uint32 tmo)
{
    return NULL;
}




/*===========================================================================

FUNCTION DOG_BARK_ISR

DESCRIPTION
   This function is called when the watchdog bark interrupt occurs.  It
   calls ERR_FATAL, in order to restart AMSS.
===========================================================================*/
void dog_bark_isr(void);

extern void dog_bark_isr()
{

} /* END dog_bark_isr */




/*===========================================================================

FUNCTION DOG_REGISTER

DESCRIPTION
   Dynamic method for tasks to register for dog services.
   A replacement for hard coded timeouts in dog.c, _AND_ for CMI compliance.

PARAMETERS
   timeout: Starvation detection threshhold in milliseconds
   deadlockcheck_flag: Check for long blocks on crit sect?
       DOG_DEADLKCHK_ENABLE      - check for deadlocks enabled
       DOG_DEADLKCHK_DISABLE     - check for deadlocks disabled

===========================================================================*/
LOCAL dog_report_type dog_register(
  rex_tcb_type             *p_tcb, 
  uint32                   timeout,
  dog_deadlockcheck_type   deadlkchk
)
{
    return 0;
}  /* End of dog_register() */


/*===========================================================================

FUNCTION DOG_AUTO_REGISTER

DESCRIPTION
   Register EACH legacy task thru dynamic registration system.
   Only to be called at init time. (??)

===========================================================================*/
LOCAL void dog_auto_register(void)
{
  

}


/*===========================================================================

FUNCTION DOG_INIT

DESCRIPTION
  This procedure performs the initialization for the Watchdog task.
  While waiting for a startup signal from the Main Control task, this
  routine repeatedly resets the watchdog.  If the startup signal from
  the Main Control task is not received before a time-out period, a
  fault is declared.

DEPENDENCIES
  The Watchdog task main processing loop should be started directly after
  this procedure.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dog_init( void )
{


} /* end of dog_init */



/*===========================================================================

FUNCTION DOG_TASK

DESCRIPTION
  This procedure contains the main processing loop for the Watchdog task.
  It checks that each task has reported in at least once during its task
  report interval.  If a task has not reported within its timeout period,
  the ERR_FATAL macro is used to log the error and reset the
  microprocessor.

DEPENDENCIES
  This procedure should be called by rex_def_task.  Each task must report
  in at least once each task report interval.  (Intervals are defined in
  the dog_state_table[] array.)

RETURN VALUE
  None, this procedure does not return.

SIDE EFFECTS
  The microprocessor may be reset during this task.

===========================================================================*/
void dog_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
	/* Signals from rex wait */
	rex_sigs_type sigs;
	
	for(;;)
	{
		/* Wait on any dog signal as we have nothing to be done */
		sigs = rex_wait(DOG_WAKEUP_TIMER_SIG);

	}
} /* end of dog_task */




/*===========================================================================

FUNCTION DOG_MONITOR_PAUSE

DESCRIPTION
   Pauses the monitoring of a specified task.

===========================================================================*/
void dog_monitor_pause(
   unsigned int report_id
)
{  

} /* END dog_monitor_pause */


/*===========================================================================

FUNCTION DOG_MONITOR_RESUME

DESCRIPTION
   Resumes the monitoring of a specified task.

===========================================================================*/
void dog_monitor_resume(
   unsigned int report_id
)
{  

} /* END dog_monitor_resume */


/*===========================================================================

FUNCTION DOG_SET_STARTUP_TIME

DESCRIPTION
   Tells Dog how long to wait in milliseconds before starting to monitor tasks.
   Use with care: calling this function to set the startup time to a large
   number will mean that malfunctions may get caught later than usual.
===========================================================================*/
void dog_set_startup_time(
   uint32 startup_time
)
{

} /* END dog_set_startup_time */


/*===========================================================================

FUNCTION DOG_CRASH_REPORT

DESCRIPTION
   Uses provided function pointer to print a text dump of dog_state_table

===========================================================================*/

void dog_crash_report(void (*print_ptr)(char *))
{

}



/*===========================================================================

FUNCTION DOG_GET_REPORT_PERIOD

DESCRIPTION
   Function to return the number of msec between expected calls to
   dog_report(). This will be less than the dog timeout period by a safety 
   margin. 

   NOT DESIGNED TO BE CALLED FROM LEGACY TASKS THAT HAVE NOT FORMALLY 
   REGISTERED THROUGH THE NEW API.

RETURN VALUE
   0  - DO NOT REPORT TO DOG.
  >0  - Report to dog every "return_value" msec.

===========================================================================*/
extern uint32 dog_get_report_period(
  dog_report_type dog_report_id
)
{
    return 5000;

}

/*===========================================================================

FUNCTION DOG_DEREGISTER

DESCRIPTION
   Dynamic way for tasks to de-register from dog services AND for CMI compliance.

===========================================================================*/
extern void dog_deregister(
  dog_report_type dog_report_id
)
{


}





