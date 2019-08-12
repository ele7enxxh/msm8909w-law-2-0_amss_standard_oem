/*!
   @file
   rfcommon_mc.c

   @brief
   This file contains all type declarations and definitions for the RFCOMMON
   Main Control layer.


   @details
   
*/

/*===========================================================================
Copyright (c) 2008 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rfcommon_mc.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/22/15   dpk     Added lock for accessing the shared xpt cal data structure between tasks
04/23/15   vs      Fix issues with TDS BC config
04/15/15   ag      Adding mising bands in bc look up table
04/06/15   ag      Adding support for enabling/disabling band capability from bc config
03/03/15   px      Moved apps task start signal to be independent of NV success
11/17/14   px      Added common function to register apps tasks with RF Dispatch
08/05/14   jmf     [SelfTest] Remove RFCMD FW message to set self-test flag
07/24/14   jmf     [SelfTest] Addition of rfcommon_mc_set_selftest_flag for FW
07/18/14   dr      [RUMI] Making fbrx_nv_init_status true for RUMI
06/16/14   tks     Added support for initializing concurrency manager 
03/31/14   cri     Remove deprecated TxOverride commands
03/21/14    aa     Read fbrx NV items during reload nv
03/20/14   cri     Remove deprecated DPD MSGR commands
02/21/13    aa     Added support for fbrx control via nv 
02/06/14   jmf     Update for ftm_lte_xpt_cal_deconfigure to take in xpt mode
02/04/14   jmf     Adding xpt_mode as arg for ftm_wcdma_xpt_cal_config
01/30/14   jmf     Update Capture Done response handler to not deconfig FB 
                   for FBRx type IQ Caps
01/07/14   zhw     Fixed compiler warnings
12/04/13   nrk     Fixed compiler warnings
11/17/13   zhw     Tuner AOL. Check if an event is aborted when FW respond.
11/18/13   cri     Support for SW allocation of DPD sample capture buffers
11/18/13   cd      Hook-up RFLM Tx AGC override interface
11/14/13   kma     Added handler for Pin calibration
11/14/13   cd      Support for RFLM-based Tx AGC override
11/06/13   cd      Re-enable MDSP xPT Tx override
11/05/13   cd      Remove common MDSP Tx override
11/05/13   cd      Tx AGC override response handler only if XPT feature
10/24/13   vv       Enabling NV init to enable  EFS read/write
08/29/13   jmf     [xPT] Integrate leakage reduction in delay cal call flow
08/05/13   Saul    XPT. Processing in progress not to fail when already in progress
08/05/13   vbh     9x35 RUMI - Bypass NV init
07/30/13   aka     Self test API updates
07/24/13   cri     DPD capture buffer reuse support
07/19/13   aro     Featurized ASDIV
07/19/13   aro     Added ASDIV manager init/de-init
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/16/13   zhw     Skip AOL update when FW response is not back
06/15/13   ndb     Added rfcommon_mc_cl_tuner_update_cleanup() & removed 
                   rfcommon_mc_tuner_update_cl_meas_response_handler() for CL
06/16/13   zhw     Tuner AOL call flow update
06/03/13   ndb     Register the dispatcher for handling the tuner manager
05/22/13   kb      [XPT] Add logic for xpt cal deconfig after IQ capture done.
05/21/13   ra      Added support for Bolt 
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/20/13   spa/zhw Move script pointer out of tuner callback data
05/14/13   cri     Use FW response instead of global data structure contents
04/15/13   Saul    Common. DPD APIs use critical section.
04/12/13   Saul    [Common] Use RF dispatch for fw responses.
03/28/13   jtn     Support gps-only build.
03/27/13   kai     Added xpt cal deconfig for wcdma
03/27/13   zhw     Added featurization for tuner related code
03/21/13   zhw     Added support for Antenna Tuner Open Loop common module 
03/19/13   aro     Added XPT CDMA cal deconfig
03/19/13   aro     Added call to XPT cal config from protocal IQ capture
03/19/13   aro     Added Data module hookup
03/14/13   sar     Fixed compiler errors in APQ target. 
03/13/13   aro     Added RFRPE deinit to perform memory cleanup
03/13/13   aro     Compiler warning fix
03/11/13   sar     Updated file for APQ, GNSS only target.
03/11/13   aro     Added memory cleanup
03/04/13   zhw     Remove outdate 9x15 workaround
02/28/13   hdz     Added unvote npa bus clk in capture done handler
02/08/13   cri     Get Tx chain info for Tx override from RFC 
02/05/13   hdz     Fixed call flow issues in mc_handle_xxx APIs
02/05/13   aca     QMI server hookup
01/03/13   aro     Add tx manager initialization
12/12/12   Saul    Common XPT. Dbg msg update.
12/03/12   Saul    Sample Capture. Bug fix in handling capture/processing done.
11/28/12   Saul    Sample Capture. Temp features to FEATURE_RF_HAS_XPT_SUPPORT
11/27/12   Saul    Sample Capture. Bug fix in checking for processing done.
11/19/12   aki     Included rfcommon_semaphore.h to remove compiler warning
11/19/12   aki     Added 'rfcommon_mc_deinit' function
11/16/12   Saul    Sample Capture. Use latest FW interface. Save XPT DPD data
11/16/12   Saul    Sample Capture. Warning fixes.
11/16/12   Saul    Sample Capture. Use common response.
11/13/12   Saul    Common XPT. Reverted changes made to commit_dpd_data.
11/09/12   kai     Fixed xPT TX AGC override response data init
11/08/12   cd      Fixed xPT Tx AGC override response type
11/08/12   Saul    Common. Warning fixes.
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
10/19/12   Saul    COMMON XPT. Separated allocate and write DPD into two APIs.
10/11/12   php     LTE RUMI flag clean up
09/19/12   jyu     Obsoleted rfcommon_fake_init() and changed to read QCN
09/18/12   jyu     Added work-around to set TDS BC_config
09/06/12   zhw     CDMA. Use golden QCN for fake rf_bc_config[1].
08/30/12   Saul    CDMA. Moved write_dpd profiling to common_mc API.
08/28/12   rsr     Added support for TXAGC override.
08/20/12   jyu     Changed to use global variable to set HW config 
                   (work-around before NV becomes available) 
08/14/12   Saul    COMMON. Corrected debug message.
08/02/12   Saul    COMMON. Added support to /free DPD tables response.
08/02/12   Saul    COMMON. Added support to allocate/write/free DPD tables.
07/29/12   ac      Added WCDMA debug flags for RUMI Bring Up  
07/26/12   Saul    COMMON. Re-instating MDSP/MSGR calls.
07/26/12   Saul    RF COMMON. Temporary remove of MDSP/MSGR calls to fix OFT.
07/23/12   Saul    RF COMMON. MDSP/MSGR framework.
07/18/12   jyu     Added TDSCDMA debug flags for RUMI Bring Up  
07/12/12   tsr     Changes for GSM DIME Rumi bring-up
07/06/12   zhw     Modify CDMA fake QCN to enable Path 1 
07/06/12   php     Add LTE debug flags for RUMI Bring Up
06/27/12   zhw     Use fake QCN when RUMI debug flag is enabled.
04/18/12   hdz     Signalling RF_APPS_TASK to continue spawning, if RFcommon
                   init is successful
03/20/12   aro     Signalling RF_FWRSP_TASK to continue spawning, if the
                   RF Common init is successful
11/04/11   bmg     Disgusting MDM9615 workaround for bad hardware that won't
                   work for CDMA unless a dummy NPA client is active to force
                   an unsupported voltage setting.
09/21/11   aro     Changed rfcommon_core_init() to return boolean
09/02/11   sar     Removed deprecated function.
08/25/11   sar     Removed obsolete file bbrx.h.
08/12/11   jhe     Added support for TDS BC config
06/24/11   sty     Fixed return value in rfcommon_mc_init()
06/24/11   sty     Fixed typo - removed redundant call to rfcommon_core_init
06/24/11   sty     Removed debug flag 
05/19/11    dw     Enable NV read 
05/10/11   sty     8960 workaround for NV read 
04/13/11   sty     Changes for RUMI3 bringup
02/28/11   tnt     Merge to MDM9K PLF110
02/23/11    av     Disable GP_CLK once again 
02/17/11    av     Reverting GP_CLK changes
02/10/11   tnt     Move rfcommon_msm_init_sbi() from rfcommon_mc_init() to 
                   rfcommon_core_init() so rf_tlmm_dal_handle is initialized before reference
02/02/11   av      Disable GP_CLK
01/28/11   bmg     Changed clk_busy_wait to DALSYS_BusyWait
01/25/11   sr      removed the clk.h dependency.  
01/06/11   ap      Took away clk_busy_wait after Vreg power on 
10/20/10   shb     Replaced raw common device struct access with interface function
10/20/10   vb/shb  Deleted gp_clk_disable - library data externed here.
                   Replaced with common command dispatch to get gp clk status
10/19/10   vb      All device functions are now accessed thru device interface functions
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
08/17/10   jyu     Fixed bug for LTE BC config 
08/03/10   aro     Compiler Warning Fix
08/02/10   aro     Fixed BBRx Disable Bug
07/28/10   aki     Added call to rfcommon_rfnv_init code moved from rfcommon_core_init
07/20/10    av      Fix compiler warnings
6/30/10     ap     Adding support for Power Collapse & Early Clock Enable 
06/28/10   can     Removing Power collapse changes that cause a crash.
06/24/10   can     Support for LTE BC Config.
06/22/10    ap     Added support for Power Collapse 
03/01/10   kma     Fixed compilation issue
03/01/10   kma     Added power collapse status API
01/29/10   kma     Removed uncessary vote
01/28/10   kma     Moved power collapse functions to each technology
02/24/10   Wen     Fixed compilation and link errors 
02/09/10   lcl     Added HAL layer API to initialize SBI clock for BBRX  
01/21/10   aro     Added interface to init SBI
11/30/09   bmg     Changed init function return to boolean
                   Warnings cleanup
10/16/09   jhe     Added _power_clock_on/off
07/26/09   bmg     Filled in default values for num antennas, tunable, and
                   antenna difference.
03/12/09   sar     Added changes needed for 1x
10/14/08   sr      Initial version of COMMON Main Control.

============================================================================*/
#include "comdef.h"
#include "stringl.h" /* for memscpy */
#include "rfa_variation.h"
#include "rfcom.h"
#include "msg.h"
#include "rfcommon_nv.h"
#include "rfcommon_core_error.h"
#include "rfcommon_concurrency_manager.h"
#include "sys.h"
#ifdef FEATURE_CMI
#include "sys_v.h"
#endif /* FEATURE_CMI */
#include "rfcommon_mc.h"
#include "rfdevice_cmn_intf.h"
#ifdef FEATURE_WCDMA
#include "rfdevice_wcdma_intf.h"
#include "ftm_wcdma_ctl.h"
#endif
#ifdef FEATURE_GSM
#include "rfdevice_gsm_intf.h"
#endif
#include "rf_fwrsp_task.h"
#include "rf_apps_task.h"
#include "pm.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfcommon_msm.h"
#endif
#include "rfc_pm.h"
#include "rfc_card.h"
#include "rfcommon_nv.h"
#include "rfcommon_nv_mm.h"
#include "DALSys.h"     /* Replacement DALSYS_BusyWait for clk_busy_wait */
#include "npa.h" /* Only needed as long as MDM9615 CDMA volage hack is in */
#include "rfm_internal.h" /* RUMI debug use only*/
#include "rfcommon_data.h"
#include "rfcommon_mdsp.h"
#include "rfcommon_fw_response.h"
#include "rfcommon_msg.h"
#include "rfcommon_semaphore.h"
#include "rfcommon_tx_manager.h"
#include "rf_qmi_rfrpe_svc.h"
#include "ftm.h"
#include "ftm_calibration_data_module.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rf_cdma_ept.h"
#include "rf_cdma_mc.h"
#include "ftm_1x_xpt.h"
#endif
#ifdef FEATURE_LTE
#include "ftm_lte_xpt.h"
#endif
#ifdef FEATURE_TDSCDMA
#include "ftm_tdscdma_ctl.h"
#endif
#include "ftm_common_control.h"
#include "modem_mem.h"
#include "rfcommon_asdiv_manager.h"
#include "rfcommon_fbrx_api.h"
#include "rf_cmd_dispatcher.h"
#include "rfcommon_cmd_processing.h"
#include "rfcommon_locks.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the Allocate DPD Tables data passed with the callback function
*/
typedef struct
{
  /*! Pointer to FW response housekeeping data for Response */
  rfcommon_alloc_dpd_tables_rsp_data_type *fw_rsp;
  
} rfcommon_alloc_dpd_tables_cb_data_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the Free DPD Tables data passed with the callback function
*/
typedef struct
{
  /*! Pointer to FW response housekeeping data for Response */
  rfcommon_free_dpd_tables_rsp_data_type *fw_rsp;
  
} rfcommon_free_dpd_tables_cb_data_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Externalized functions
*/
/*----------------------------------------------------------------------------*/

extern rf_path_enum_type rfcommon_core_device_to_path(rfcom_device_enum_type device );
extern boolean rfcommon_core_init(rf_path_enum_type path);
extern uint64 rfcommon_nv_get_rf_bc_config(rf_path_enum_type path);
extern uint64 rfcommon_nv_get_rf_lte_bc_config(void);


rfm_bands_bitmask rfc_bc_cfg_bands[RF_PATH_MAX] = {0};

rfm_bands_bitmask rfc_all_lte_mask =
{
{{0x0},{0xFF00000000000000},{0xFFFFFFFFFFFFFFFF}}
};

typedef struct
{
  uint8 index; /*Specifies the mask ID to be updated in RFC*/
  sys_band_class_e_type rfc_band_supp /*Maps the BC config band index to sys band index*/; 
}sys_band_bc_lookup_type;

rfm_devices_configuration_type *trm_dev_cfg ;


/*SYS band and BC config but field mapping look up table. This is to fetch the SYS band mask band ID given the band is set in BC config*/
sys_band_bc_lookup_type sys_band_bc_lookup[SYS_SBAND_MAX] = 
{
{0, SYS_BAND_BC0}, //BC0 A
{0, SYS_BAND_BC0}, //BC0 B	
{0, SYS_BAND_BC1}, //BC1
{255, SYS_BAND_CLASS_NONE}, //BC2 Not defined in sys band Enum. Hard coding to 'SYS_BAND_CLASS_NONE' for now.
{0, SYS_BAND_BC3}, //BC3
{0, SYS_BAND_BC4}, //BC4
{0, SYS_BAND_BC5}, //BC5
{0, SYS_BAND_GSM_DCS_1800}, //DCS 1800
{0, SYS_BAND_GSM_EGSM_900}, //EGSM 900
{0, SYS_BAND_GSM_PGSM_900}, //PGSM900
{0, SYS_BAND_BC6}, //BC6
{0, SYS_BAND_BC7}, //BC7
{0, SYS_BAND_BC8}, //BC8
{0, SYS_BAND_BC9}, //BC9
{0, SYS_BAND_BC10}, //BC10
{0, SYS_BAND_BC11}, //BC11
{0, SYS_BAND_GSM_450}, //GSM450
{0, SYS_BAND_GSM_480}, //GSM480
{0, SYS_BAND_GSM_750}, //GSM750
{0, SYS_BAND_GSM_850}, //GSM850
{0, SYS_BAND_GSM_RGSM_900}, //GSM railways 900
{0, SYS_BAND_GSM_PCS_1900}, //GSM PCS1900
{1, SYS_BAND_WCDMA_I_IMT_2000}, //WCDMA I IMT 2000
{1, SYS_BAND_WCDMA_II_PCS_1900}, //WCDMA II PCS 1900
{1, SYS_BAND_WCDMA_III_1700}, //WCDMA III 1700
{1, SYS_BAND_WCDMA_IV_1700}, //WCDMA IV 1700
{1, SYS_BAND_WCDMA_V_850}, //WCDMA V 850
{1, SYS_BAND_WCDMA_VI_800}, //WCDMA VI 800
{0, SYS_BAND_BC12}, //BC12
{0, SYS_BAND_BC14}, //BC14
{255, SYS_BAND_CLASS_NONE}, //RESERVED
{0, SYS_BAND_BC15}, //BC15
{1, SYS_BAND_WLAN_US_2400}, //WLAN US 2400
{1, SYS_BAND_WLAN_EUROPE_2400}, //WLAN EU 2400
{1, SYS_BAND_WLAN_FRANCE_2400}, //WLAN FRA 2400
{1, SYS_BAND_WLAN_SPAIN_2400}, //WLAN SPN 2400
{1, SYS_BAND_WLAN_JAPAN_2400}, //WLAN JPN 2400
{1, SYS_BAND_WLAN_US_5000}, //WLAN US 5000
{1, SYS_BAND_WLAN_EUROPE_5000}, //WLAN EU 5000
{1, SYS_BAND_WLAN_FRANCE_5000}, //WLAN FRA 5000
{1, SYS_BAND_WLAN_SPAIN_5000}, //WLAN SPN 5000
{1, SYS_BAND_WLAN_JAPAN_5000}, //WLAN JPN 5000 (41)
{255, SYS_BAND_CLASS_NONE},//RESERVED 
{255, SYS_BAND_CLASS_NONE}, //RESERVED
{255, SYS_BAND_CLASS_NONE}, //RESERVED
{255, SYS_BAND_CLASS_NONE}, //RESERVED
{255, SYS_BAND_CLASS_NONE},//RESERVED
{255, SYS_BAND_CLASS_NONE},//RESERVED
{1, SYS_BAND_WCDMA_VII_2600},//WCDMA VII 2600
{1, SYS_BAND_WCDMA_VIII_900},//WCDMA VIII 900
{1, SYS_BAND_WCDMA_IX_1700},//WCDMA IX 1700
{255, SYS_BAND_CLASS_NONE},//RESERVED
{255, SYS_BAND_CLASS_NONE},//RESERVED
{255, SYS_BAND_CLASS_NONE},//RESERVED
{255, SYS_BAND_CLASS_NONE},//RESERVED
{255, SYS_BAND_CLASS_NONE},//RESERVED
{0, SYS_BAND_BC16},//BC16
{0, SYS_BAND_BC17},//BC17
{0, SYS_BAND_BC18},//BC18
{0, SYS_BAND_BC19},//BC19
{1, SYS_BAND_WCDMA_XIX_850},	//WCDMA XIX 850
{1, SYS_BAND_WCDMA_XI_1500},//WCDMA XI 1500
{255, SYS_BAND_CLASS_NONE}//RESERVED
};


/*----------------------------------------------------------------------------*/
/*!
  @brief 
  Cleanup allocations done in 'rfcommon_mc_init'
  
*/
void
rfcommon_mc_deinit(void)
{
  rfcommon_semaphore_deinit();
  rfcommon_data_deinit();

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  #ifdef FEATURE_CDMA1X
  rf_cdma_mc_deinit();
  #endif /* FEATURE_CDMA1X */
  #ifdef FEATURE_TDSCDMA
  rf_cmd_dispatch_deregister_tech( RF_TASK_MAIN, MSGR_TECH_TDSCDMA, 
                                   MSGR_MODULE_RF_MAX );
  #endif /* FEATURE_TDSCDMA */
  rf_qmi_rfrpe_svc_deinit();
  rfcommon_fw_response_deregistration();
#endif

  #ifdef FEATURE_RF_ASDIV
  rfcommon_asdiv_manager_deinit();
  #endif /* FEATURE_RF_ASDIV */
  
  rfcommon_rf_apps_task_deregister();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

  @param device: RF device
  @param caller_tcb_ptr: Calling task TCB pointer
  @param task_nv_wait_sig: Task signal to wait for when reading NV
  @param task_wait_func_ptr: Task's wait function to be called when reading NV

  @retval 
  rfcommon_core_init_success: Success(1) or Failure(0) reported by rfcommon_core_init
*/
boolean
rfcommon_mc_init(rfcom_device_enum_type device, 
                 rex_tcb_type *caller_tcb_ptr,
                 rex_sigs_type task_nv_wait_sig,
                 void (*task_wait_func_ptr)( rex_sigs_type ))
{
  rf_path_enum_type path = RF_PATH_0;
  rfcommon_nv_status_type nv_status = RFCOMMON_NV_READ_FAILED;
  static boolean nv_init_success = FALSE;
  boolean rfcommon_core_init_success = FALSE;
  boolean fbrx_nv_init_success = FALSE;

  path = rfcommon_core_device_to_path(device);

  /* BBRx Init */
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  /* Initialize SBI clock */
  rfcommon_msm_init_sbi_clk();
#endif

  //Enabling this function  to enable  EFS  read/write
  rfcommon_rfnv_init();

  #ifdef FEATURE_RF_ASDIV
  rfcommon_asdiv_manager_init();
  #endif /* FEATURE_RF_ASDIV */

  rf_fwrsp_signal_to_start_task();

  rf_apps_signal_to_start_task();

  /* RF Dispatch Commands Registration */
  rfcommon_rf_apps_task_register();

  /* Read NV if we have not previously been successful. */
  if ( nv_init_success != TRUE )
  {
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
    nv_status = rfcommon_nv_mm_retrive_items( caller_tcb_ptr, 
                                              task_nv_wait_sig, 
                                              task_wait_func_ptr );
    
    /* Read the FBRx Common NV data */
    fbrx_nv_init_success = rfcommon_fbrx_nv_retrieve_common_data( caller_tcb_ptr, 
                                                                  task_nv_wait_sig, 
                                                                  task_wait_func_ptr );

#ifdef T_RUMI_EMULATION 
    fbrx_nv_init_success = TRUE;
#endif
    
    if ((nv_status == RFCOMMON_NV_READ_SUCCESS) && (fbrx_nv_init_success == TRUE))
#endif
    {
      /* Not missing critical NV items */
      nv_init_success = TRUE;      
  
      /* Now that we have read the NV items successfully, we can continue in
         initializing common core configuration */
      rfcommon_core_init_success = rfcommon_core_init(path);

      /* Signal RF_FWRSP_TASK and RF_APPS_TASK to start */
      if ( rfcommon_core_init_success == TRUE )
      {
        /* Initialize Critical Section for XPT */
        rf_common_init_critical_section( rfcommon_get_xpt_lock_data() );

        /* Initialize common data */
        rfcommon_core_init_success &= rfcommon_data_init();

        /* Initialize Semaphore Module */
        rfcommon_core_init_success &= rfcommon_semaphore_init();

        /* Initialize commmon mdsp - this is a one-time init */
        rfcommon_core_init_success &= rfcommon_mdsp_init();

        /* Perform RF Responses registration */
        rfcommon_core_init_success &= rfcommon_fw_response_registration();

        /* Perform Tx resource manager initialization */
        rfcommon_core_init_success &= rf_tx_resource_mgr_init();
        
        /*RFRFPE Service initialization*/
        rf_qmi_rfrpe_svc_init();
      }
    } /* if (nv_status == RFCOMMON_NV_READ_SUCCESS) */
  } /* if ( nv_init_success != TRUE ) */

  /* Initialize concurrency manager state machine */
  rfcmn_concurrency_mgr_rx_init(); 
  
  /* Send command to perform efs dump of physical devices */
  rf_apps_wait_till_task_started(rex_self());
  rfc_common_rffe_scan_efs_dump_cmd_put();

  /* return final result of API */
  return rfcommon_core_init_success;

} /* rfcommon_mc_init */


/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
*/
int32 rfcommon_mc_cmd_dispatch(rfcom_device_enum_type device, int32 cmd, 
                               void *data) 
{
  int32 retval;

  switch(cmd)
  {
  case RFCOMMON_MC_GET_NUM_ANTENNA:
    retval = 2;
    break;
  case RFCOMMON_MC_IS_TUNABLE_ANTENNA:
    retval = 1;
    break;
  case RFCOMMON_MC_GET_ANTENNA_DIFF:
    retval = RF_ANT_QLTY_UNKNOWN;
    break;
  default:
    retval = 0;
    RF_FATAL_ERROR("Unsupported RF common dispatch command");
  }

  return retval;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
*/
uint64 rfcommon_mc_get_band_mask(rfcom_device_enum_type device )
{
  rf_path_enum_type path = RF_PATH_0;
  path = rfcommon_core_device_to_path(device);
  return rfcommon_nv_get_rf_bc_config(path);

} /* rfcommon_mc_get_band_mask() */


/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
*/
uint64 rfcommon_mc_get_lte_band_mask(void)
{
  /* since diversity always needs to be on for LTE, no need for
   * path information at this point.
   */
  return rfcommon_nv_get_rf_lte_bc_config();

} /* rfcommon_mc_get_lte_band_mask() */


/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
*/
uint64 rfcommon_mc_get_tds_band_mask(rfcom_device_enum_type device )
{
  rf_path_enum_type path = RF_PATH_0;
  path = rfcommon_core_device_to_path(device);
  return rfcommon_nv_get_rf_tds_bc_config(path);

} /* rfcommon_mc_get_tds_band_mask() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reading common NV which are independent of technology.

  @details
  Retrieves all the common NV items. This is called as part of the 
  LOAD_RF_NV function case where we need to read the calibration NV and write 
  them without having to reset.

  @param device: RF device
  @param caller_tcb_ptr: Calling task TCB pointer
  @param task_nv_wait_sig: Task signal to wait for when reading NV
  @param task_wait_func_ptr: Task's wait function to be called when reading NV

  @retval 
  nv_init_success: Success(1) or Failure(0) dependant on success or failure
  returned by rfcommon_nv_mm_retrive_items
*/
boolean
rfcommon_mc_reload_nv(rfcom_device_enum_type device, 
            rex_tcb_type *caller_tcb_ptr,
            rex_sigs_type task_nv_wait_sig,
            void (*task_wait_func_ptr)( rex_sigs_type ))
{
  rfcommon_nv_status_type nv_status = RFCOMMON_NV_READ_FAILED;
  boolean nv_init_success = FALSE;
  boolean fbrx_nv_init_success = TRUE;
    
  nv_status = rfcommon_nv_mm_retrive_items(caller_tcb_ptr, task_nv_wait_sig, 
                                             task_wait_func_ptr);

  /* Copy the common FBRx Contol NV data during bootup */
  fbrx_nv_init_success = rfcommon_fbrx_nv_retrieve_common_data( caller_tcb_ptr,
                                         task_nv_wait_sig,
                                         task_wait_func_ptr );

  if ((nv_status == RFCOMMON_NV_READ_SUCCESS) && (fbrx_nv_init_success == TRUE))
  {
    /* Not missing critical NV items */
    nv_init_success = TRUE;      
  } /* if (nv_status == RFCOMMON_NV_READ_SUCCESS) */

  return nv_init_success;
}

/*============================================================================*/
/*!
  @name RF COMMON Control Functions

  @brief
  Functions for RF COMMON control
*/
/*! @{ */

boolean
rfcommon_mc_allocate_dpd_buffer
(
   void
)
{
  return rfcommon_mdsp_allocate_dpd_buffer();
}

void
rfcommon_mc_free_dpd_buffer
(
   void
)
{
  rfcommon_mdsp_free_dpd_buffer();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override TXAGC parms..
 
  @param device
  Device to configure
 
  @param oride_data
  list of paramters that need to be overriden.

  @return
  Flag indicating the function execution success
*/
boolean 
rfcommon_mc_xpt_txagc_override
(
  rfcom_device_enum_type device,
  rfcommon_mdsp_txagc_oride_params_type* oride_data
)
{
  boolean api_status = TRUE;

  return api_status;

}/* rfcommon_mc_xpt_txagc_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common interface to override all Tx AGC parameters
 
  @details
  This function implements a linearizer based override interface. The
  override parameters are provided via the linearizer interface to RFLM. All
  overrides are then passed through here to RFLM and processed inside RFLM

  @param rfm_mode
  RFM mode for which Tx override is requested
 
  @param device
  Device to configure

  @param rfm_band
  Holder for tech specific band information
 
  @param tx_ovr_params
  Structure containing all override params

  @return
  Flag indicating the function execution success
*/
boolean 
rfcommon_mc_xpt_txagc_override_v2
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  rfcommon_core_xpt_tx_override_type* tx_ovr_params
)
{
  boolean api_status = TRUE;

  /* Configure linearizer interface first */
  api_status &= rfcommon_xpt_load_linearizer_for_override( rfm_mode, device,
                                                           rfm_band,
                                                           tx_ovr_params );

  /* Call MDSP/RFLM interface to apply Tx override */
  api_status &= rfcommon_mdsp_xpt_txagc_override_v2( rfm_mode, device, 
                                                     tx_ovr_params );

  RF_MSG_2( RF_HIGH, "rfcommon_mc_xpt_txagc_override_v2: Dev %d [Status %d]",
            device, api_status );

  RF_MSG_7( RF_HIGH, "rfcommon_mc_xpt_txagc_override_v2: XPT Mode %d, "
                     "IQ Gain Action %d, IQ Gain %d, Env Scale Action %d, "
                     "Env Scale %d, Delay Action %d, Delay %d",
            tx_ovr_params->xpt_mode, 
            tx_ovr_params->iq_gain_action_type, 
            tx_ovr_params->iq_gain, 
            tx_ovr_params->env_scale_action_type, 
            tx_ovr_params->env_scale, 
            tx_ovr_params->delay_action_type, 
            tx_ovr_params->et_delay );

  RF_MSG_4( RF_HIGH, "rfcommon_mc_xpt_txagc_override_v2: RGI %d, SMPS %d, "
                     "PA State %d, PA Curr %d",
            tx_ovr_params->rgi, tx_ovr_params->papm_bias, 
            tx_ovr_params->pa_state, tx_ovr_params->pa_curr );

  return api_status;

}/* rfcommon_mc_xpt_txagc_override_v2 */

/*! @} */

/*============================================================================*/
/*!
  @name Task Response Handlers

  @brief
  This section includes all the handlers for the responses received from RF
  Task.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for Capture Done Reponse received through RF task
 
  @details
  This function is called when response to XPT Sample Capture message is sent to RF task.
  This function will basically post the semaphore so that blocking call can be
  unblocked.
 
  @param req_ptr
  Pointer to RF Task command
*/
void
rfcommon_mc_handle_capture_done_response
(
  rf_fwrsp_cmd_type* req_ptr
)
{
  rfcommon_fw_response_data_type *fwrsp_data = NULL;
  rfcmd_sample_capt_done_rsp *rsp_payload;
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;
  boolean is_reduced_leakage_on;
  const rfc_common_logical_device_params_type *common_logical_device_ptr = NULL;
  rfcommon_xpt_cal_data_type *xpt_cal_data;
  ftm_rf_technology_type ftm_rf_tech  = FTM_RF_TECH_UNKNOWN;

  /* Get FW Response Data */
  fwrsp_data = rfcommon_get_fw_response_data();

  ftm_rf_tech = ftm_get_rf_technology();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_mc_handle_capture_done_response: NULL FWRSP Data" );
    return;
  } /* if ( fwrsp_data == NULL ) */

  /* Get Data */
  xpt_cal_data = rfcommon_data_get_xpt_cal_data();
  /* Typecast the payload */
  rsp_payload = (rfcmd_sample_capt_done_rsp*)req_ptr->payload;
  fwrsp_data->capture_done_rsp.capt_buf_id = rsp_payload->capt_buf_id;

  /* Fetch IQ Capture Data */
  rfcommon_xpt_fetch_iq_capture_data( &iq_cap_data );

  if (iq_cap_data.capture_type == XPT_CAPTURE_FBRX) 
  {
    RF_MSG(RF_HIGH, "rfcommon_mc_handle_capture_done_response: FBRx Int Cal type capture done, no deconfig of WTR or RxLM buffer done");
  }

  if (iq_cap_data.delay_capture_type > RF_COMMON_XPT_DELAY_CAP_INVALID)
  {
    RF_MSG_4(RF_HIGH,"rfcommon_mc_handle_capture_done_response: delay cap complete for chan %d, buf %d, delay_cap_type %d, samp_rate %d", 
             iq_cap_data.channel, fwrsp_data->capture_done_rsp.capt_buf_id, iq_cap_data.delay_capture_type,iq_cap_data.samp_rate);
    ftm_cal_data_set_delay_capture_complete(iq_cap_data.channel,
                                            fwrsp_data->capture_done_rsp.capt_buf_id,
                                            (ftm_cal_data_delay_cap_type)iq_cap_data.delay_capture_type,
                                            iq_cap_data.samp_rate);
  }
  else if ( iq_cap_data.pin_capture_type > RF_COMMON_XPT_PIN_CAP_INVALID )
  {
    ftm_cal_data_set_pin_capture_complete(iq_cap_data.channel,
                                          fwrsp_data->capture_done_rsp.capt_buf_id); 
    RF_MSG_2(RF_HIGH,"rfcommon_mc_handle_capture_done_response: pin cap complete for chan %d, buf %d", 
             iq_cap_data.channel, fwrsp_data->capture_done_rsp.capt_buf_id);
  }
  else
  {
    RF_MSG_3(RF_HIGH,"rfcommon_mc_handle_capture_done_response: dpd cap complete for chan %d, buf %d, delay_cap_type %d", 
             iq_cap_data.channel, fwrsp_data->capture_done_rsp.capt_buf_id, iq_cap_data.delay_capture_type);
    if ( ftm_cal_data_is_chan_initialized(iq_cap_data.channel) )
    {
      ftm_cal_data_set_capture_complete( iq_cap_data.channel,
                                         fwrsp_data->capture_done_rsp.capt_buf_id );
    }
  }

  /* For the  FBRx Internal Cal Capture types, control of FB Device setup is done explicitly  */
  if ( ( iq_cap_data.is_last_trig == TRUE ) &&  (iq_cap_data.capture_type != XPT_CAPTURE_FBRX) ) 
  {
    switch ( ftm_rf_tech )
    {
    case FTM_RF_TECH_CDMA:
      #ifdef FEATURE_CDMA1X
      ftm_1x_xpt_cal_deconfig( iq_cap_data.device,
                               iq_cap_data.device_fb );  
      #endif
      break;

      case FTM_RF_TECH_WCDMA:
        #ifdef FEATURE_WCDMA
        ftm_wcdma_xpt_cal_config( iq_cap_data.device,
                                  iq_cap_data.device_fb,
                                  (uint8)FTM_XPT_CONFIG_EPT,
                                  FALSE );
        #endif
        break;

    case FTM_RF_TECH_LTE:
      #ifdef FEATURE_LTE
      ftm_lte_xpt_cal_deconfigure(iq_cap_data.device,
                                  iq_cap_data.device_fb,
                                  (uint8)FTM_XPT_CONFIG_EPT );
      #endif
      break;

    case FTM_RF_TECH_TDSCDMA:
      #ifdef FEATURE_TDSCDMA
      ftm_tdscdma_xpt_cal_config( iq_cap_data.device,
                                  iq_cap_data.device_fb,
                                  (uint8)FTM_XPT_CONFIG_EPT,
                                  FALSE );
      #endif
      break;

    default:
      break;
    }

    if (iq_cap_data.delay_capture_type > RF_COMMON_XPT_DELAY_CAP_INVALID)
    {
      /* Re-store reduced-leakage device settings */
      common_logical_device_ptr = rfc_common_get_logical_device_params(iq_cap_data.device_fb);
      if (common_logical_device_ptr == NULL)
      {
        RF_MSG_1( RF_ERROR, "rfcommon_mc_handle_capture_done_response: "
                   "NULL logical device for device %d",
                   iq_cap_data.device_fb);
      }
      else
      {
        RF_MSG_1( RF_HIGH, "rfcommon_mc_handle_capture_done_response: RF device %d reduced leakage cfg OFF.",
                  iq_cap_data.device_fb );
        is_reduced_leakage_on = FALSE;
        rfdevice_cmn_cmd_dispatch( common_logical_device_ptr->cmn_device,
                                   RFDEVICE_CFG_REDUCED_LEAKAGE,
                                   &is_reduced_leakage_on );
      }
    }
  }

  RF_MSG_4( RF_MED, "[FW->SW] CAPTURE DONE 0x%x [ %d | %d ]. Capture buf %d", 
            rsp_payload->seq_id, 
            RFCOMMON_MDSP_SNUM_TECH(rsp_payload->seq_id),
            RFCOMMON_MDSP_SNUM_COUNT(rsp_payload->seq_id),
            fwrsp_data->capture_done_rsp.capt_buf_id );

  rfcommon_core_xpt_release_capture_buffer(fwrsp_data->capture_done_rsp.capt_buf_id );

  /*Perform NULL pointer check*/
  if ( xpt_cal_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_mc_handle_capture_done_response: NULL XPT cal data"  );
  } /* if( xpt_cal_data == NULL ) */
  else
  {
    /*Enter critical section*/
    rf_common_enter_critical_section(rfcommon_get_xpt_lock_data());
	
    xpt_cal_data->is_iq_capture_in_process = FALSE;

    /*Leave critical section*/
    rf_common_leave_critical_section(rfcommon_get_xpt_lock_data()); 
  }

  RF_MSG( RF_LOW, "rfcommon_mc_handle_capture_done_response: Handled" );

  return;
} /* rfcommon_mc_handle_capture_done_response */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for Processing Done Reponse received through RF task
 
  @details
  This function is called when response to XPT Sample Capture message is sent to RF task
  with indication to perform processing.
  This function will basically post the semaphore so that blocking call can be
  unblocked.
 
  @param req_ptr
  Pointer to RF Task command
*/
void
rfcommon_mc_handle_processing_done_response
(
  rf_fwrsp_cmd_type* req_ptr
)
{
  rfcommon_fw_response_data_type *fwrsp_data = NULL;
  rfcmd_xpt_cal_proc_done_rsp *rsp_payload;
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;
  rfcommon_xpt_cal_data_type *xpt_cal_data;

  /* Get FW Response Data */
  fwrsp_data = rfcommon_get_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_mc_handle_processing_done_response: NULL FWRSP Data" );
    return;
  } /* if ( fwrsp_data == NULL ) */

  /* Get Data */
  xpt_cal_data = rfcommon_data_get_xpt_cal_data();
  /* Typecast the payload */
  rsp_payload = (rfcmd_xpt_cal_proc_done_rsp*)req_ptr->payload;

  fwrsp_data->processing_done_rsp.capt_buf_id = rsp_payload->capt_buf_id;
  fwrsp_data->processing_done_rsp.proc_buf_id = rsp_payload->proc_buf_id;

    /* Fetch IQ Capture Data */
  rfcommon_xpt_fetch_iq_capture_data( &iq_cap_data );

  if ( iq_cap_data.delay_capture_type > RF_COMMON_XPT_DELAY_CAP_INVALID )
  {
    RF_MSG_3(RF_HIGH,"rfcommon_mc_handle_processing_done_response: delay proc complete for chan %d, buf %d, delay_cap_type %d", 
             iq_cap_data.channel, fwrsp_data->processing_done_rsp.proc_buf_id, iq_cap_data.delay_capture_type);
    ftm_cal_data_set_delay_proc_complete(iq_cap_data.channel,
                                         fwrsp_data->processing_done_rsp.proc_buf_id,
                                         (ftm_cal_data_delay_cap_type)iq_cap_data.delay_capture_type);
  }
  else if ( iq_cap_data.pin_capture_type > RF_COMMON_XPT_PIN_CAP_INVALID )
  {
    ftm_cal_data_set_pin_proc_complete(iq_cap_data.channel,
                                       fwrsp_data->processing_done_rsp.proc_buf_id);
    RF_MSG_2(RF_HIGH,"rfcommon_mc_handle_processing_done_response: pin proc complete for chan %d, buf %d", 
             iq_cap_data.channel, fwrsp_data->processing_done_rsp.proc_buf_id);
  }
  else
  {
    if ( ftm_cal_data_is_chan_initialized(iq_cap_data.channel) )
    {
      RF_MSG_3(RF_HIGH,"rfcommon_mc_handle_processing_done_response: dpd proc complete for chan %d, buf %d, delay_cap_type %d", 
               iq_cap_data.channel, fwrsp_data->processing_done_rsp.proc_buf_id, iq_cap_data.delay_capture_type);

      ftm_cal_data_set_proc_complete( iq_cap_data.channel,
                                      fwrsp_data->processing_done_rsp.proc_buf_id );
    }
  }

  RF_MSG_5( RF_MED, "[FW->SW] PROCESSING DONE 0x%x [ %d | %d ]. Capture buf %d. Proc buf %d", 
            rsp_payload->seq_id, 
            RFCOMMON_MDSP_SNUM_TECH(rsp_payload->seq_id),
            RFCOMMON_MDSP_SNUM_COUNT(rsp_payload->seq_id),
            fwrsp_data->processing_done_rsp.capt_buf_id,
            fwrsp_data->processing_done_rsp.proc_buf_id );

  rfcommon_core_xpt_release_results_buffer(fwrsp_data->processing_done_rsp.proc_buf_id);

  /*Perform NULL pointer check*/
  if ( xpt_cal_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_is_capture_done: NULL XPT cal data"  );
  } /* if( xpt_cal_data == NULL ) */
  else
  {
    /*Enter critical section*/
    rf_common_enter_critical_section(rfcommon_get_xpt_lock_data());
	
    xpt_cal_data->is_processing_in_process = FALSE;

    /*Leave critical section*/
    rf_common_leave_critical_section(rfcommon_get_xpt_lock_data());	
  }

  RF_MSG( RF_LOW, "rfcommon_mc_handle_processing_done_response: Handled" );

  return;
} /* rfcommon_mc_handle_processing_done_response */

/*! @} */

/*============================================================================*/
/*!
  @name XPT Calibration

  @brief
  This section contains functions for XPT calibration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set status vars related to sample capture
 
  @pre rfcommon_xpt_is_capture_done
  Check for done first, if no capture is in progress, then call this API to set vars
 
  @return
  TRUE if success, FALSE otherwise
*/
boolean 
rfcommon_xpt_set_capture_in_progress
(
   void
)
{
  boolean api_status = TRUE;
  rfcommon_xpt_cal_data_type *xpt_cal_data;

  /* Get Data */
  xpt_cal_data = rfcommon_data_get_xpt_cal_data();

  /*Perform NULL pointer check*/
  if ( xpt_cal_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_set_capture_in_progress: NULL XPT cal data" );
    return FALSE;
  } /* if( xpt_cal_data  == NULL ) */

  /*Enter critical section*/
  rf_common_enter_critical_section(rfcommon_get_xpt_lock_data());

  if ( xpt_cal_data->is_iq_capture_in_process == TRUE )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_set_capture_in_progress: Sample capture already in progress." );
    /*Leave critical section to release the lock before return*/
    rf_common_leave_critical_section(rfcommon_get_xpt_lock_data());	
    return FALSE;
  }

  xpt_cal_data->is_iq_capture_in_process = TRUE;

  /*Leave critical section*/
  rf_common_leave_critical_section(rfcommon_get_xpt_lock_data());
  
  return api_status;
} /* rfcommon_xpt_set_capture_in_progress */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set status vars related to sample processing
 
  @pre rfcommon_xpt_is_processing_done
  Check for done first, if no processing is in progress, then call this API to set vars
 
  @return
  TRUE if success, FALSE otherwise
*/
boolean 
rfcommon_xpt_set_processing_in_progress
(
   void
)
{
  boolean api_status = TRUE;
  rfcommon_xpt_cal_data_type *xpt_cal_data;

  /* Get Data */
  xpt_cal_data = rfcommon_data_get_xpt_cal_data();

  /*Perform NULL pointer check*/
  if ( xpt_cal_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_set_processing_in_progress: NULL XPT cal data" );
    return FALSE;
  } /* if( xpt_cal_data  == NULL ) */

  /*Enter critical section*/
  rf_common_enter_critical_section(rfcommon_get_xpt_lock_data()); 

  xpt_cal_data->is_processing_in_process = TRUE;

  /*Leave critical section*/
  rf_common_leave_critical_section(rfcommon_get_xpt_lock_data());

  return api_status;
} /* rfcommon_xpt_set_processing_in_progress */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Store IQ Capture Data
 
  @param iq_cap_data
  Pointer to the data structure containg the IQ Capture data to be stored
 
  @return
  TRUE if success, FALSE otherwise
*/
boolean 
rfcommon_xpt_store_iq_capture_data
(
  rfcommon_xpt_cal_iq_capture_data_type *iq_cap_data
)
{
  boolean api_status = TRUE;
  rfcommon_xpt_cal_data_type *xpt_cal_data;

  /* Get Data */
  xpt_cal_data = rfcommon_data_get_xpt_cal_data();

  /*Perform NULL pointer check*/
  if ( xpt_cal_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_store_iq_capture_data: NULL XPT cal data" );
    return FALSE;
  } /* if( xpt_cal_data  == NULL ) */

  if ( iq_cap_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_store_iq_capture_data: NULL data" );
    return FALSE;
  } /* if ( iq_cap_data == NULL ) */

  /*Enter critical section*/
  rf_common_enter_critical_section(rfcommon_get_xpt_lock_data()); 

  memscpy( &(xpt_cal_data->iq_cap_data), sizeof(rfcommon_xpt_cal_iq_capture_data_type),
           iq_cap_data, sizeof(rfcommon_xpt_cal_iq_capture_data_type) );

  /*Leave critical section*/
  rf_common_leave_critical_section(rfcommon_get_xpt_lock_data());

  return api_status;
} /* rfcommon_xpt_store_iq_capture_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Fetch IQ Capture Data
 
  @param iq_cap_data
  Pointer to the data structure where the IQ Capture data is to be fetched
 
  @return
  TRUE if success, FALSE otherwise
*/
boolean 
rfcommon_xpt_fetch_iq_capture_data
(
  rfcommon_xpt_cal_iq_capture_data_type *iq_cap_data
)
{
  boolean api_status = TRUE;
  rfcommon_xpt_cal_data_type *xpt_cal_data;

  /* Get Data */
  xpt_cal_data = rfcommon_data_get_xpt_cal_data();

  /*Perform NULL pointer check*/
  if ( xpt_cal_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_fetch_iq_capture_data: NULL XPT cal data" );
    return FALSE;
  } /* if( xpt_cal_data  == NULL ) */

  if ( iq_cap_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_fetch_iq_capture_data: NULL data" );
    return FALSE;
  } /* if ( iq_cap_data == NULL ) */

  /*Enter critical section*/
  rf_common_enter_critical_section(rfcommon_get_xpt_lock_data()); 

  memscpy( iq_cap_data, sizeof(rfcommon_xpt_cal_iq_capture_data_type),
           &(xpt_cal_data->iq_cap_data), sizeof(rfcommon_xpt_cal_iq_capture_data_type) );

  /*Leave critical section*/
  rf_common_leave_critical_section(rfcommon_get_xpt_lock_data());

  return api_status;
} /* rfcommon_xpt_fetch_iq_capture_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check if the capture is done

  @details
  This function will query xPT data structure to indicate, if the xPT
  capture process has completed or not

  @return
  TRUE indicates capture is done. FALSE otherwise
*/
boolean
rfcommon_xpt_is_capture_done
(
  void
)
{
  boolean cap_done = FALSE;

  rfcommon_xpt_cal_data_type *xpt_cal_data;

  /* Get Data */
  xpt_cal_data = rfcommon_data_get_xpt_cal_data();

  /*Perform NULL pointer check*/
  if ( xpt_cal_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_is_capture_done: NULL XPT cal data"  );
    return FALSE;
  } /* if( xpt_cal_data  == NULL ) */

  if (xpt_cal_data->is_iq_capture_in_process == TRUE)
  {
    cap_done = FALSE;
  }
  else
  {
    cap_done = TRUE;
  }

  RF_MSG_1( RF_MED, "rfcommon_xpt_is_capture_done: [Done? %d]", cap_done );

  return cap_done;

} /* rfcommon_xpt_is_capture_done */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Waits up to the prescribed timeout for capture done.
 
  @details
  If capture is done prior to time out, the API returns early.
 
  @param time_interval_us
  Used as a delay to determine how often to check for done.
  Must be less than time out.
 
  @param timeout_us
  The total time to wait before proceeding.
 
  @param buffer_index
  OUTPUT. The buffer containing the data.
 
  @return
  Flag indicating the if the capture is complete or not
 
  @retval TRUE if capture completed, FALSE otherwise.
*/

boolean
rfcommon_xpt_wait_for_capture_done
(
  uint32 time_interval_us,
  uint32 timeout_us,
  uint8 *buffer_index
)
{
  boolean api_status = TRUE;
  timetick_type time_start = 0;
  timetick_type time_passed = 0;
  rfcommon_fw_response_data_type *fwrsp_data = NULL;

  if((time_interval_us == 0) || (timeout_us == 0))
  {
    RF_MSG_2( RF_ERROR, "rfcommon_xpt_wait_for_capture_done - "
              "Interval %d or timeout %d. Neither cannot be zero.",
              time_interval_us, timeout_us );
    return FALSE;
  }

  if(time_interval_us >= timeout_us)
  {
    RF_MSG_2( RF_ERROR, "rfcommon_xpt_wait_for_capture_done - "
              "Interval %d must be less than timeout %d.",
              time_interval_us, timeout_us );
    return FALSE;
  }

  /* Get FW Response Data */
  fwrsp_data = rfcommon_get_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_wait_for_capture_done: NULL FWRSP Data" );
  } /* if ( fwrsp_data == NULL ) */

  /* Get start time */
  time_start = timetick_get();
  /* If capture not done or timeout not reached... */
  while( ((api_status = rfcommon_xpt_is_capture_done()) == FALSE) &&
         ( (time_passed = timetick_get_elapsed( time_start, T_USEC )) <= timeout_us) )
  {
    DALSYS_BusyWait( time_interval_us );/* Check every this many us */
    RF_MSG_2( RF_HIGH, "rfcommon_xpt_wait_for_capture_done - Waiting for sample capture done.  "
              "Time left %d us of %d us ", ( timeout_us - time_passed), timeout_us );
  }

  if( buffer_index != NULL )
  {
    *buffer_index = -1;
  }

  if( (api_status == TRUE) && 
      ( buffer_index != NULL ) &&
      ( fwrsp_data != NULL ) )
  {
    *buffer_index = fwrsp_data->capture_done_rsp.capt_buf_id;
  }

  if( api_status == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_xpt_wait_for_capture_done - Sample capture NOT done in %d us",
              timeout_us );
  }

  return api_status;
} /* rfcommon_xpt_wait_for_capture_done */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check if XPT processing is done

  @details
  This function will query xPT data structure to indicate, if xPT
  processing has completed or not

  @return
  TRUE indicates capture is done. FALSE otherwise
*/
boolean
rfcommon_xpt_is_processing_done
(
  void
)
{
  boolean proc_done = FALSE;

  rfcommon_xpt_cal_data_type *xpt_cal_data;

  /* Get Data */
  xpt_cal_data = rfcommon_data_get_xpt_cal_data();

  /*Perform NULL pointer check*/
  if ( xpt_cal_data  == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_is_processing_done: NULL XPT cal data"  );
    return FALSE;
  } /* if( xpt_cal_data  == NULL ) */

  if (xpt_cal_data->is_processing_in_process == TRUE)
  {
    proc_done = FALSE;
  }
  else
  {
    proc_done = TRUE;
  }

  RF_MSG_1( RF_MED, "rfcommon_xpt_is_processing_done: [Done? %d]", proc_done );

  return proc_done;

} /* rfcommon_xpt_is_processing_done */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Waits up to the prescribed timeout for processing done.
 
  @details
  If processing is done prior to time out, the API returns early.
 
  @param time_interval_us
  Used as a delay to determine how often to check for done.
  Must be less than time out.
 
  @param timeout_us
  The total time to wait before proceeding.
 
  @param buffer_index
  OUTPUT. The buffer containing the data.
 
  @return
  Flag indicating the if the processing is complete or not
 
  @retval TRUE if processing completed, FALSE otherwise.
*/

boolean
rfcommon_xpt_wait_for_processing_done
(
  uint32 time_interval_us,
  uint32 timeout_us,
  uint8 *buffer_index
)
{
  boolean api_status = TRUE;
  timetick_type time_start = 0;
  timetick_type time_passed = 0;
  rfcommon_fw_response_data_type *fwrsp_data = NULL;

  if((time_interval_us == 0) || (timeout_us == 0))
  {
    RF_MSG_2( RF_ERROR, "rfcommon_xpt_wait_for_processing_done - "
              "Interval %d or timeout %d. Neither cannot be zero.",
              time_interval_us, timeout_us );
    return FALSE;
  }

  if(time_interval_us >= timeout_us)
  {
    RF_MSG_2( RF_ERROR, "rfcommon_xpt_wait_for_processing_done - "
              "Interval %d must be less than timeout %d.",
              time_interval_us, timeout_us );
    return FALSE;
  }

  /* Get FW Response Data */
  fwrsp_data = rfcommon_get_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_wait_for_processing_done: NULL FWRSP Data" );
  } /* if ( fwrsp_data == NULL ) */

  /* Get start time */
  time_start = timetick_get();
  /* If capture not done or timeout not reached... */
  while( ((api_status = rfcommon_xpt_is_processing_done()) == FALSE) &&
         ( (time_passed = timetick_get_elapsed( time_start, T_USEC )) <= timeout_us) )
  {
    DALSYS_BusyWait( time_interval_us );/* Check every this many us */
    RF_MSG_2( RF_HIGH, "rfcommon_xpt_wait_for_processing_done - Waiting for processing done. "
              "Time left %d us of %d us ", ( timeout_us - time_passed), timeout_us );
  }

  if( buffer_index != NULL )
  {
    *buffer_index = -1;
  }

  if( ( api_status == TRUE ) && 
      ( buffer_index != NULL ) &&
      ( fwrsp_data != NULL ) )
  {
    *buffer_index = fwrsp_data->processing_done_rsp.proc_buf_id;
  }

  if( api_status == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_xpt_wait_for_processing_done - Processing NOT done in %d us",
              timeout_us );
  }

  return api_status;
} /* rfcommon_xpt_wait_for_processing_done */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Saves captured XPT IQ data.
 
  @param buffer_index
  Index to the IQ buffer containing the data to save.
 
  @param num_samples
  The amount of samples requested.
 
  @param iq_file_name
  A null-terminated string representing the file name.
 
  @return
  TRUE when success, else FALSE
*/
boolean
rfcommon_mc_save_xpt_iq_data
( 
   uint8 buffer_index,
   uint32 num_samples,
   char* iq_file_name
)
{
  return rfcommon_mdsp_save_xpt_iq_data(buffer_index, num_samples, iq_file_name);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Saves captured Self Test IQ data.
 
  @param buffer_index
  Index to the IQ buffer containing the data to save.
 
  @param num_samples
  The amount of samples requested.
 
  @param iq_file_name
  A null-terminated string representing the file name.
 
  @return
  TRUE when success, else FALSE
*/
boolean
rfcommon_mc_fetch_selftest_iq_data
( 
  uint8 buffer_index,
  uint32 num_samples,
  int32 **rx_sample_buffer,
  int32 **tx_sample_buffer
)
{
  return rfcommon_mdsp_fetch_selftest_iq_data(buffer_index, num_samples, rx_sample_buffer, tx_sample_buffer);
}

boolean
rfcommon_mc_fetch_selftest_meas
( 
   uint8 buffer_index,
   uint16 *evm,
   uint32 *rx_rms_mag
)
{
  return rfcommon_mdsp_fetch_selftest_meas(buffer_index, evm, rx_rms_mag );
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Saves Processed XPT DPD data.
 
  @param buffer_index
  Index to the DPD buffer containing the data to save.
 
  @param dpd_file_name
  A null-terminated string representing the file name.
 
  @return
  TRUE when success, else FALSE
*/
boolean
rfcommon_mc_save_xpt_dpd_data
( 
   uint8 buffer_index,
   char* dpd_file_name
)
{
  return rfcommon_mdsp_save_xpt_dpd_data(buffer_index, dpd_file_name);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get ET Path Delay from MDSP.
 
  @param buffer_index
  Index to the result buffer containing delay data.

  @param et_path_delay
  Pointer return ET path delay (IQ - ENV) in 1/1000th samples
 
  @return
  TRUE when success, else FALSE
*/
boolean
rfcommon_mc_et_path_delay
( 
   uint8 buffer_index,
   int32 * et_path_delay
)
{
  return rfcommon_mdsp_et_path_delay(buffer_index, et_path_delay);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function to be executed after mdsp_queue_atuner_update
 
  @details
  This function will be passed to RF-MDSP layer as a callback function to
  be executed after sending mdsp_queue_atuner_update. This function will
  acknowledge the token and free up allocation tuner script buffer and token.
 
  @param cb_event
  Event indicating the success and failure of RF-MDSP action
   
  @param cb_data
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.
*/
boolean
rfcommon_mc_tuner_update_response_handler
( 
  void *cmd_ptr,
  const rf_dispatch_snum_info_type *snum_info
)
{
  #ifdef FEATURE_RF_HAS_QTUNER
  boolean api_status = TRUE;
  rfcommon_mc_tuner_cb_data_type *tuner_cb_data;
  uint16 tuner_chain_idx;
  boolean event_aborted;

  rfcommon_mdsp_tuner_common_status_type* common_tuner_cb_status;
  rfcommon_mdsp_tuner_tech_status_type* tuner_cb_status;

  tuner_cb_data = (rfcommon_mc_tuner_cb_data_type*)(snum_info->snum_cb_data);

  /* NULL Pointer check */
  if ( tuner_cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_mc_tuner_update_response_handler: NULL CB Data" );
    return FALSE;
  } /* if ( fws_cfg_cb != NULL ) */

  common_tuner_cb_status = rfcommon_mdsp_get_common_tuner_cb_status(RFCOMMON_AOL_EVENT);

  if( common_tuner_cb_status == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_mc_tuner_update_response_handler: "
                      "NULL Tuner callback status pointer" );
    return FALSE;
  }

  /* check for valid chain */
  if ( tuner_cb_data->tuner_tech >= RF_TUNER_BUF_ID_NUM_TECHS || 
       tuner_cb_data->tuner_chain >= RF_TUNER_CHAIN_NUM )
  {
    RF_MSG_2( RF_ERROR, "rfcommon_mc_tuner_update_response_handler: "
                        "Invalid tech %d chain %d", 
                        tuner_cb_data->tuner_tech, tuner_cb_data->tuner_chain );
    return FALSE;
  }

  tuner_cb_status = &(common_tuner_cb_status->tech_tuner_cb_status
                      [tuner_cb_data->tuner_tech][tuner_cb_data->tuner_chain]);

  tuner_cb_status->is_waiting_for_fw_rsp = FALSE;

  /* Check if an event is aborted by FW. If aborted, do NOT acknowledge token
  and reset CCS event smem status */
  event_aborted = rfcommon_mdsp_tuner_check_aborted_update(
                                     tuner_cb_data->tuner_tech, 
                                     tuner_cb_data->tuner_chain );

  if ( event_aborted == FALSE )
  {
  for( tuner_chain_idx = 0; tuner_chain_idx < RF_TUNER_CHAIN_NUM; tuner_chain_idx++  )
  {
    if( tuner_cb_data->tuner_data[tuner_chain_idx].ant_tuner != NULL )
    {
      api_status &= rfcommon_atuner_manager_acknowledge_script( 
                                             tuner_cb_data->tuner_data[tuner_chain_idx].ant_tuner, 
                                             tuner_cb_data->tuner_data[tuner_chain_idx].tuner_token );
    }
    else
    {
      RF_MSG_1( RF_LOW, "rfcommon_mc_tuner_update_response_handler: "
                        "Null antenna tuner object for tuner chain %d", 
                        tuner_chain_idx );
    }
  }
  } /* if ( event_aborted == FALSE ) */
  else 
  {
    rfcommon_mdsp_ccs_abort_event( &(tuner_cb_data->tuner_ccs_event_handle) );
  } /* if ( event_aborted == TRUE ) */

  RF_MSG_6( RF_MED, "[FW->SW] [AOL] [TECH%d] [C%d] Antenna Tuner Update "
                    "SNUM 0x%08x, Token: C0 %d C1 %d, event aborted %d",
            tuner_cb_data->tuner_tech, tuner_cb_data->tuner_chain, 
            snum_info->snum, 
            tuner_cb_data->tuner_data[RF_TUNER_CHAIN_0].tuner_token,
            tuner_cb_data->tuner_data[RF_TUNER_CHAIN_1].tuner_token,
            event_aborted );  

  rfcommon_mc_tuner_update_cleanup( tuner_cb_data );

  if( api_status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfcommon_mc_tuner_update_response_handler: "
                      "Callback Error" );
  }

  return api_status;
  #else
  return FALSE;
  #endif

}/* rfcommon_mc_tuner_update_response_handler */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function to be executed after mdsp_queue_atuner_update 
 
  @details
  This function will be passed to RF-MDSP layer as a callback function to
  be executed in case mdsp_queue_atuner_update needs to be executed before 
  FW response happens, hence not executing the registered fwrsp call back 
  function. This function free up resources that should have been cleared by the 
  registered fwrsp call back function and also reset all the state variables.
  
  @param registered_cb_data
  callback data that got passed in along with the registered call back. It was 
  meant to be handled by the registered call back but since registered call back 
  never got executed, we handle the call back data here including cleaning up 
*/
void
rfcommon_mc_tuner_update_cleanup
( 
  void *registered_cb_data
)
{
  rfcommon_mc_tuner_cb_data_type *tuner_cb_data;

  tuner_cb_data = (rfcommon_mc_tuner_cb_data_type*)(registered_cb_data);

  /* NULL Pointer check */
  if ( tuner_cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_mdsp_tuner_update_abort_cb: NULL CB Data" );
    return;
  } /* if ( fws_cfg_cb != NULL ) */

  /* Clean up CCS event handle */
  rfcommon_mdsp_ccs_cleanup_event( &(tuner_cb_data->tuner_ccs_event_handle) );

  modem_mem_free( tuner_cb_data, MODEM_MEM_CLIENT_RFA );

  return;
}/* rfcommon_mc_tuner_update_cleanup */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function to be executed after mdsp_tuner_update_cl_meas_script
 
  @details
  This function will be passed to RF-MDSP layer as a callback function to
  be executed in case mdsp_tuner_update_cl_meas_script needs to be executed before 
  FW response happens, hence not executing the registered fwrsp call back 
  function. This function free up resources that should have been cleared by the 
  registered fwrsp call back function and also reset all the state variables.
  This function can be used to clean-up the cl tuner call back data if we failed 
  to commit the CL event to FW ,but allocated the call back data.
 
  @param registered_cb_data
  callback data that got passed in along with the registered call back. It was 
  meant to be handled by the registered call back but since registered call back 
  never got executed, we handle the call back data here including cleaning up 
*/
void
rfcommon_mc_cl_tuner_update_cleanup
( 
  void *registered_cb_data
)
{
  rfcommon_mc_cl_tuner_cb_data_type *cl_tuner_cb_data;
  
  cl_tuner_cb_data = (rfcommon_mc_cl_tuner_cb_data_type*)(registered_cb_data);

  /* NULL Pointer check */
  if ( cl_tuner_cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_mdsp_tuner_update_abort_cb: NULL CB Data" );
    return;
  } /* if ( fws_cfg_cb != NULL ) */


  /* Abort CCS event and clean up CCS event handle */
  rfcommon_mdsp_ccs_abort_event( &(cl_tuner_cb_data->tuner_ccs_event_handle) );
  rfcommon_mdsp_ccs_cleanup_event( &(cl_tuner_cb_data->tuner_ccs_event_handle) );

  modem_mem_free( cl_tuner_cb_data, MODEM_MEM_CLIENT_RFA );

  return;
}/* rfcommon_mc_tuner_update_cleanup*/

/*! @} */


/*----------------------------------------------------------------------------*/
/*!
  @brief
 Fucntion to be called from rfm_init()
 
  @details
This function is called to update the RFC information to be returned to TRM based on the BC config.
This is to allow customer to set a superset of supported bands in RFC and change band support only via BC config NV without changing the RFC.
This takes care of WCDMA, GSM and 1x. 
*/
void rfcommon_mc_config_trm_band_support()
{
 uint64 tmp_band_supp = 0;
 uint8 loop_cntr = 0;
 rf_path_enum_type rf_path = RF_PATH_0 ;

 memset(&rfc_bc_cfg_bands, 0, RF_PATH_MAX*sizeof(rfm_bands_bitmask));

 for(rf_path = RF_PATH_0;rf_path < RF_PATH_MAX;rf_path++) 
 { 
 /* Loop through all the RF paths*/
  if(rfcommon_nv_get_rf_bc_config(rf_path) != 0)  
  {   
   /*Update for the path with non-zero BC config*/
   tmp_band_supp = rfcommon_nv_get_rf_bc_config(rf_path);
   for(loop_cntr = 0; loop_cntr < SYS_SBAND_MAX; loop_cntr++)
   {
    if((tmp_band_supp & (1ULL << loop_cntr)) && (sys_band_bc_lookup[loop_cntr].index != 255)) 
    /*Check for the bits set in the BC config and use the look up table to set the corresponding bit in the sys band format in the correct mask ID */
    {  
          rfc_bc_cfg_bands[rf_path].mask[sys_band_bc_lookup[loop_cntr].index] = 
                                   rfc_bc_cfg_bands[rf_path].mask[sys_band_bc_lookup[loop_cntr].index] | 
                                   (1ULL << (sys_band_bc_lookup[loop_cntr].rfc_band_supp - (64*sys_band_bc_lookup[loop_cntr].index)));	
    }
   }
  }/*LTE uses a different BC config NV. Don't touch LTE band support. Make it all 1s. Same goes for TDS*/
    rfc_bc_cfg_bands[rf_path].mask[0] |= rfc_all_lte_mask.mask[0];
    rfc_bc_cfg_bands[rf_path].mask[1] |= rfc_all_lte_mask.mask[1];  
    rfc_bc_cfg_bands[rf_path].mask[2] |= rfc_all_lte_mask.mask[2]; 
 }
}


