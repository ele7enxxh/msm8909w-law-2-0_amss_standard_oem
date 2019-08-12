/*!
  @file
  rfwcdma_core_antenna_tuner.c

  @brief
  Provides WCDMA antenna tuner functionality

  @details

*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/src/rfwcdma_core_antenna_tuner.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/09/15   yb      Fix for snum exhaustion
02/04/15   ac      clean up,Concurrency of ASDiv and Tuner configuration & Concurrency of AOL and Update Tuner 
12/03/14   ac      Skip update tune code if already under progress
11/26/14   ac      Workaround to ABORT all pending Snums during disable Tx
11/13/14   ac      WCDMA ASDiv + Tuner issues in AOL update and CA Port assignment
10/21/14   yb      Added priority configuration to config Tuners during CRAT scenarios
09/30/14   ndb     Pass LM_handle in tuner_cb to help with debug
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/28/14   kg      Pass tech_id in tuner_cb to help with debug
08/11/14   rmb     Add create script support for Tx Disable API.
08/06/14   nv      We are making sure that rfwcdma_core_antenna_tuner_incremental_update()
                   is done only when RF is in TX state.
07/31/14   ak      AOL updates to be done only in RXTX mode.
06/05/14   yb      Tuner force_detune() for supporting FTM_PROGRAM_QFE_SCENARIO
06/04/14   vws     Call Tuner ack function after setting tuner tx/rx modes
06/04/14   vws     Support script based interface rfwcdma_core_antenna_tuner_program_tx_mode 
06/04/14   vws     port change about tuner tx disable during Tx Disable 
05/28/14   vws     WCDMA IRAT tuner_disable in X2W
05/12/14   kai     Initialise CCS handles to NULL
04/29/14   kg      Correct LM handle for AOL
03/28/14   kg      Added support for CL LM handle 
03/27/14   ac      porting the latest changes from dime for asdiv
03/20/14   ac      asd changes from dime
03/12/14   kg      Combining pcell-scell scripts for aol.
02/10/14   kg      Adding Tuner Open Loop Event Support
1/29/14    vbh     Warning fix
10/09/13   yb      Removing tune_code_override from rfdevice_antenna_tuner_intf layer.
                   Replacing with rfcommon_atuner_tune_code_override API.
10/07/13   kr      Remove redundant error messages from tuner module
10/3/13    kr      Add antenna switch is in progress check in rfwcdma_core_antenna_tuner_incremental_update()
07/24/13   vs      Pass Closed loop NV info to Tuner 
07/12/13   aro     Compiler Warning fix
07/12/13   aa      Pass the buffer to tuner manager API
06/28/13   vs      Add WCDMA support for FTM_PROGRAM_QFE_SCENARIO
06/27/13   ndb     Added rfcommon_nv_ant_tuner_cl_ctrl_type in tuner tx mode 
06/20/13   aa      Tuner init optimization
06/07/13   vs      Updated number of tuner from 3 to 5
05/20/13   rmb     Fix to avoid null pointer access for Null Pointer access of Antenna Tuner Call Back data.
05/17/13   rmb     Removed tuner_script reference for the Call Back data in 
                   rfwcdma_core_antenna_tuner_incremental_update().
05/13/13   rmb     Fix Memory Leak Issue in rfwcdma_core_antenna_tuner_incremental_update().
05/06/13   vb      Bug Fix: wrong channel passed based on RX or RXTX state of the active chain
05/06/13   rmb     Check for valid path and cell type in antenna_tuner_incremental_update() API.
05/03/13   rmb     Fix KW Error.
05/03/13   rmb     Added rfcommon_core_convert_rfm_device_to_cell_type 
                   API in Antenna Tuner Update.
05/03/13   rmb     Fix Klockwork Errors.
05/02/13   vb      Tuner CA changes 
05/01/13   rmb     Added QTF AOL Tuner update event for all devices/chains by pushing scripts into 
                   shared memory as a CCS Event in rfwcdma_core_antenna_tuner_incremental_update() API.
04/30/13   vb      Fix for wrong argument when device script needs to run in 
                   IMMEDIATE mode
04/12/13   sr    removed legacy code which is not used.
04/04/13   ka      WCDMA state machine updates
03/11/13   vb      Fix bug in a loop for tuner incremental updates
03/04/13   vb      Fix bug in a loop checking tuner object status
02/14/13   vb      Added Dime specific implementation of tune code override
02/14/13   vb      Further support added for new Tuner manager 
02/14/13   vb      Support for WCDMA Tuner incrementatl updates from WL1
02/14/13   vb      Support for tuner_disable()
02/13/13   vb      Support for new tuner manager in WCDMA
02/13/13   vb      Old tuner architure cleanup 
02/06/13   rmb     Added rf_buffer_intf and rf_device_execution_type params
                   to antenna_tuner APIs for wakeup optimization.
01/07/13   ndb     Added Tech specific Settings support in rx_init
12/03/12    vb     Added support for multiple tuner instances from RFC
12/03/12    vb     Code to support Tuner OL - temporary solution
11/16/12    vb     Dime RFC updates for getting Tuner device instances
10/22/12   vb      Added support for Antenna Tuner's IRAT meas tune away script
10/11/12   gh      Add support for rx_init
10/10/12   kai/vb  Added IRAT interface APIs
10/10/12   kai/vb  Added call back function register API
10/10/12   kai     Changed interface rfwcdma_core_antenna_program_scenario_standalone
09/21/12   kai     Updated to new cpp APIs, removed old rfwcdma_core_ant_tuner.c
09/04/12    gh     Added support for tuner class implementation and removed
                   old implementations
08/02/12   kai     Added feature FEATURE_RF_HAS_QFE1510_HDET for QFE1510 HDET
06/21/12    vb     Added support for QFE1510

============================================================================*/

#ifdef FEATURE_RF_HAS_QTUNER

#include "rfwcdma_core_antenna_tuner.h"
#include "rfcommon_msg.h"
#include "rfwcdma_mc.h"
#include "rfc_card.h"
#include "rfc_card_wcdma.h"
#include "rfcommon_atuner_intf.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_tuner.h"
#include "rfcommon_core.h"
#include "modem_mem.h"
#include "rflm_api_wcdma.h"
#include "rflm_dm_api.h"
#include "rflm_cmn_intf.h"

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_manager.h"
#endif

/*Max number of Tuner objects that RFC can pass for any given */
#define RFWCDMA_CORE_MAX_ANTENNA_TUNERS 5

extern uint32 aol_pending_snum[5];
uint8 snum_counter = 0;

extern boolean rfwcdma_mdsp_associate_event_id(uint16 event_id,rflm_dm_handle_id_t rflm_handle);
/*---------------------------------------------------------------------------*/
/*!
  @brief
  Initial QFE tuner
  @details
  Initial QFE tuner
 
  @param device
  The device path to be enabled.

  @param band
   
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rfwcdma_core_antenna_tuner_init
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rf_buffer_intf* buffer_ptr,
  rf_device_execution_type rfwcdma_exec_type
)
{
  boolean init_status = TRUE;
  void **qtuner_mgr_obj = NULL;
  uint8 device_idx = 0;
  uint32 script_token =0;

  /* Get Qtuner object from RFC */
  qtuner_mgr_obj = rfc_wcdma_get_device( device, 
                                         RFC_CONFIG_RX, 
                                         band, 
                                         RFDEVICE_TUNER_MANAGER );

  if (qtuner_mgr_obj != NULL)
  {
    while(qtuner_mgr_obj[device_idx] != NULL)
      {
        /* Initialize tuner managers associated */
        init_status &= rfcommon_atuner_manager_init ( qtuner_mgr_obj[device_idx], 
                                                      RFCOM_WCDMA_MODE, 
                                                      rfwcdma_exec_type, 
                                                      buffer_ptr, 
                                                      0, 
                                                      &script_token);

        if (init_status && (script_token != ATUNER_INVALID_TOKEN_VAL) && (rfwcdma_exec_type == RFDEVICE_CREATE_SCRIPT))
        {
           /*Ack if the script construction was successful */
           rfcommon_atuner_manager_acknowledge_script ( qtuner_mgr_obj[device_idx], 
                                                       script_token );
        }

      device_idx++;

    } /*End of looping thru all devices*/
  }
  else
  {
    RF_MSG_1(RF_MED,"ANT_TUNER: No Antenna tuner manager found from RFC for path %d", device);
    init_status = FALSE;
  }

  if(init_status)
  {   
   // RF_MSG_1(RF_HIGH,"ANT_TUNER: Antenna tuner manager init on device path %d", device);
  }
  else
  {
   RF_MSG_1(RF_ERROR,"ANT_TUNER: Antenna tuner manager init on device path %d failed !", device);
  }
  
  return init_status;
} /* rfwcdma_core_antenna_tuner_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the QFE RX.

  @details
  Programs tuner Rx sequence and the associated tune code for the 
  frequency (based on the band, chan information provided) from the information 
  captured in the Tuner DB.

  @param device
  The device path to be enabled.

  @param band

  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rfwcdma_core_antenna_tuner_program_rx_mode 
( 
  rfm_device_enum_type rfm_device,
  rfcom_wcdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type rfwcdma_exec_type,
  rf_buffer_intf* buffer_ptr
)
{
  boolean success = TRUE;
  rfcom_band_type_u band_u;
  void **qtuner_mgr_obj = NULL;
  uint8 device_idx = 0;
  uint32 script_token =0;
  rfcom_antenna_tuner_priority_cfg tuner_priority_cfg;

  /* Get Tuner manager object from RFC */
  qtuner_mgr_obj = rfc_wcdma_get_device( rfm_device, 
                                         RFC_CONFIG_RX, 
                                         band, 
                                         RFDEVICE_TUNER_MANAGER );

  band_u.wcdma_band = band;
  tuner_priority_cfg.rfm_device = rfm_device;

  if (qtuner_mgr_obj != NULL)
  {
    while(qtuner_mgr_obj[device_idx] != NULL)
      {
        /* Enable tuner manager's rx_mode */
        success &= rfcommon_atuner_manager_set_rx_mode( tuner_priority_cfg,
                                                        qtuner_mgr_obj[device_idx], 
                                                        RFCOM_WCDMA_MODE,
                                                        band_u,
                                                        chan_num, 
                                                        rfwcdma_exec_type, 
                                                        buffer_ptr, 
                                                        0, 
                                                        &script_token);

        if (success && (script_token != ATUNER_INVALID_TOKEN_VAL) && 
             (rfwcdma_exec_type == RFDEVICE_CREATE_SCRIPT))
        {
          /*Ack if the script construction was successful */
          rfcommon_atuner_manager_acknowledge_script ( qtuner_mgr_obj[device_idx], 
                                                      script_token );
        }

      device_idx++;

    } /*End of looping thru all devices*/
  }
  else
  {
    RF_MSG_1(RF_MED,"ANT_TUNER: No Antenna Tuner Manager found from RFC for path %d", 
      rfm_device);
    success = FALSE;
  }

  if(success)
  {         
   // RF_MSG_1(RF_HIGH,"ANT_TUNER: Programmed in Rx mode on device path#%d",
   //   rfm_device);
  }
  else
  {
    RF_MSG_1(RF_ERROR,"ANT_TUNER: Program Rx mode on device path#%d failed !", rfm_device);
  }
  

  return success;
} /* rfwcdma_core_antenna_tuner_program_rx_mode */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Enable the QFE TX.

  @details
  Programs QFE tuner to set up TX path.

  @param device
  The device path to be enabled.

  @param band
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rfwcdma_core_antenna_tuner_program_tx_mode 
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type rfwcdma_exec_type,
  rf_buffer_intf* buffer_ptr,
  rfcommon_atuner_cl_ctrl_info_type *cl_ctrl_info
)
{
  boolean success = TRUE;
  void **qtuner_mgr_obj = NULL;
  uint8 device_idx = 0;
  rfcom_band_type_u band_u;
  uint32 script_token =0;

  /* Get Tuner manager object from RFC */
  qtuner_mgr_obj = rfc_wcdma_get_device( device, 
                                         RFC_CONFIG_TX, 
                                         band, 
                                         RFDEVICE_TUNER_MANAGER);

  band_u.wcdma_band = band;

  if (qtuner_mgr_obj != NULL)
  {
    while(qtuner_mgr_obj[device_idx] != NULL)
      {     
        // set tuner tx mode */
        success &= rfcommon_atuner_manager_set_tx_mode ( qtuner_mgr_obj[device_idx],
                                                         RFCOM_WCDMA_MODE, 
                                                         band_u,
                                                         chan_num,
                                                         rfwcdma_exec_type, 
                                                         buffer_ptr, 
                                                         0, /*No timing*/
                                                         &script_token,
                                                         cl_ctrl_info);

        if (success && (script_token != ATUNER_INVALID_TOKEN_VAL) && (rfwcdma_exec_type == RFDEVICE_CREATE_SCRIPT))
        {
           /*Ack if the script construction was successful */
           rfcommon_atuner_manager_acknowledge_script ( qtuner_mgr_obj[device_idx], 
                                                       script_token );
        }													 

      device_idx++;

    } /*End of looping thru all devices*/
  }
  else
  {
    RF_MSG_1(RF_MED,"ANT_TUNER: No Antenna tuner managers found from RFC for path %d", device);
    success = FALSE;
  }


  if(success)
  {   
  //RF_MSG_1(RF_HIGH,"ANT_TUNER: Programmed in Tx mode on device path#%d", device); 
  }
  else
  {
    RF_MSG_1(RF_ERROR,"ANT_TUNER: Program Tx mode on device path#%d failed !", device);
  }
  
  return success;
} /* rfwcdma_core_antenna_tuner_program_tx_mode */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Disable the QFE.

  @details
  Disable the QFE.

  @param device
  The device path to be disabled.

  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rfwcdma_core_antenna_tuner_disable 
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing
)
{
  boolean success = TRUE;
  void **qtuner_mgr_obj = NULL;
  uint8 device_idx = 0;
  uint32 script_token =0;

  /* Get Qtuner object from RFC */
  qtuner_mgr_obj = rfc_wcdma_get_device( device, 
                                         RFC_CONFIG_RX, 
                                         band, 
                                         RFDEVICE_TUNER_MANAGER );

  /* Loop thru all the Tuner manager objects */
  if (qtuner_mgr_obj != NULL)
  {
    while(qtuner_mgr_obj[device_idx] != NULL)
      { 
        /* Disable tuner */
        success &= rfcommon_atuner_manager_disable ( qtuner_mgr_obj[device_idx],
                                                     dev_action, 
                                                     script_buffer, 
                                                     script_timing, 
                                                     &script_token );

        if (success && (script_token != ATUNER_INVALID_TOKEN_VAL) && (dev_action == RFDEVICE_CREATE_SCRIPT))
        {
           /*Ack if the script construction was successful */
           rfcommon_atuner_manager_acknowledge_script ( qtuner_mgr_obj[device_idx], 
                                                       script_token );     
        }

      device_idx++;

      if(device_idx == RFWCDMA_CORE_MAX_ANTENNA_TUNERS )
      {
        /*Max number of Tuner devices scanned*/
        break;
      }

    } /*End of looping thru all devices*/
  }
  else
  {
    RF_MSG_1(RF_MED,"ANT_TUNER: No Antenna tuner managers found from RFC for path %d", device);
    success = FALSE;
  }

  if(success)
  {       
   // RF_MSG_1(RF_HIGH,"ANT_TUNER: Disabled on device path#%d", device);    
  }
  else
  {
   RF_MSG_1(RF_ERROR,"ANT_TUNER: Disable on device path#%d failed !", device);
  }
  
  return success;
} /* rfwcdma_core_antenna_tuner_disable */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Disable the QFE.

  @details
  Disable the QFE.

  @param device
  The device path to be disabled.

  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rfwcdma_core_antenna_tuner_tx_disable 
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing
)
{
 boolean success = TRUE;
 rfcom_band_type_u band_u;
 void **qtuner_mgr_obj = NULL;
 uint8 device_idx = 0;
 uint32 script_token =0;


  /* Get Qtuner object from RFC */
  qtuner_mgr_obj = rfc_wcdma_get_device( device, 
                                         RFC_CONFIG_TX, 
                                         band, 
                                         RFDEVICE_TUNER_MANAGER );

  
  band_u.wcdma_band = band;

  /* Loop thru all the Tuner manager objects */
  if (qtuner_mgr_obj != NULL)
  {
    while(qtuner_mgr_obj[device_idx] != NULL)
      { 
        /* Disable tuner */
        success &= rfcommon_atuner_manager_tx_disable (  qtuner_mgr_obj[device_idx],
                                                         RFCOM_WCDMA_MODE, 
                                                         band_u,
                                                         chan_num,
                                                         dev_action, 
                                                         script_buffer, 
                                                         script_timing, /*No timing*/
                                                         &script_token);
       

      device_idx++;

    } /*End of looping thru all devices*/
  }
  else
  {
    RF_MSG_1(RF_MED,"ANT_TUNER: No Antenna tuner managers found from RFC for path %d", device);
    success = FALSE;
  }

  if(success)
  {       
   // RF_MSG_1(RF_HIGH,"ANT_TUNER: Disabled on device path#%d", device);    
  }
  else
  {
    RF_MSG_1(RF_ERROR,"ANT_TUNER: Disable Tx on device path#%d failed !", device);
  }
  return success;
} /* rfwcdma_core_antenna_tuner_disable */


/*----------------------------------------------------------------------------*/
/*!
  @brief
    rfwcdma_core_antenna_tuner_tune_code_override.
    This function is used to override the antenna tuner settings


  @param 
    

  @details
*/
uint32 rfwcdma_core_antenna_tuner_tune_code_override( rfcom_device_enum_type device,
                                                      rfcom_wcdma_band_type band,
                                                      uint8 override_flag,
                                                      void *data,
                                                      uint8* tuner_nv_ptr,
                                                      uint8 *tuner_id_ptr)
{
  uint32 status = RFCMN_ATUNER_ERROR;

#ifdef FEATURE_RF_HAS_QTUNER 
  void **qtuner_obj = NULL;

  rfcommon_atuner_intf* tuner_mgr = NULL;
  /* Get Tuner device object from RFC */
  qtuner_obj = rfc_wcdma_get_device( device, 
                                     RFC_CONFIG_RX, 
                                     band, 
                                     RFDEVICE_TUNER_MANAGER );

  if(qtuner_obj == NULL)
  {
    RF_MSG_1(RF_MED,"ANT_TUNER: No Anntenna tuner manager found from RFC for path %d", device);
    return(RFCMN_ATUNER_NOT_PRESENT);
  }

  /* Current support is for 1 Tuner per chain */
  tuner_mgr = (rfcommon_atuner_intf*)qtuner_obj[0];
  if (tuner_mgr != NULL)
  {
    /* Override the tuner on the path of interest */
    status = rfcommon_atuner_tune_code_override( tuner_mgr,
                                                  override_flag,
                                                  data,
                                                  tuner_nv_ptr,
                                                  tuner_id_ptr);
  }
  else
  {
    RF_MSG_1(RF_MED,"ANT_TUNER: No Anntenna qtuner manager found from RFC for path %d", device);
    status = RFCMN_ATUNER_NOT_PRESENT;
  }

  if(status)
  {         
    RF_MSG_1(RF_HIGH,"ANT_TUNER: Antennat tuners overriden for the device path#%d", device);   
  }

#endif /* FEATURE_RF_HAS_QTUNER */

  return status;

} /*rfwcdma_core_antenna_tuner_tune_code_override*/

/******************************************************************************
	                           WL1 INTERFACE APIs
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*!
  @brief
  rfwcdma_core_Antenna_tuner_incremental_update

  @details
  This API should be called  by rfm_wcdma_Antenna_tuner_tick()
  Inside this API check all the active paths then give a call to
  tuner manager associated with appropriate tuners
 
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
boolean rfwcdma_core_antenna_tuner_incremental_update ( void )
{
 boolean success = FALSE;
 boolean update_tuner_api_status = {FALSE};
 rfcom_band_type_u band_u;
 void **qtuner_mgr_obj = NULL;
 rfm_device_enum_type rfm_device;
 rf_time_tick_type prof_tick;
 rf_time_type prof_time = 0; /* Time spent in function */
 boolean is_rx_mode = TRUE;
 rfcommon_tuner_update_cb_data_type* tuner_cb_data = {NULL};
 rf_buffer_intf *tuner_script = {NULL};
 rfcommon_mdsp_event_handle** tuner_event_handle_ptr = {NULL};
 script_token_type* chain_token_ptr;
 rfwcdma_mc_car_path_state_type car_path_state[RFCOM_MAX_CARRIERS];
 uint8 valid_num_of_carrier;
 int8 device_idx = 0;
 rflm_dm_handle_id_t rflm_handle;
 uint32* buf_ptr;
 rflm_cmn_tuner_event_data_type* rflm_tuner_intf = NULL;
 static boolean update_tune_code_in_progress = FALSE;

  #ifdef FEATURE_RF_ASDIV
     if(rfcommon_asdiv_manager_try_enter_lock() == TRUE)
     {
       if (rfcommon_asdiv_is_switch_in_progress() == TRUE || update_tune_code_in_progress == TRUE) 
   {
         RF_MSG_1(RF_HIGH, 
             "rfwcdma_core_update_tuner_code: [ASDIV] Switch in progress OR update in progress, Skip update",0);
         rfcommon_asdiv_manager_release_lock();
         return TRUE;
       }
       /* Set flag to indicate update tune code in progress to avoid concurrency of
        regular AOL with ASDiv triggered update tune code*/
       update_tune_code_in_progress = TRUE; 
       rfcommon_asdiv_manager_release_lock();
     }
     else
     {
       RF_MSG_1(RF_HIGH, 
             "rfwcdma_core_update_tuner_code: [ASDIV] Switch in progress OR update in progress, Skip update",0);
     return TRUE;
   }
#endif

 if(rfwcdma_mc_state.car_path_state[0][0].rf_state != RFWCDMA_MC_STATE_RXTX)
 {
   RF_MSG_1(RF_ERROR,"rfwcdma_core_antenna_tuner_incremental_update:"
	"WL1 calling for tune code update in non RXTX mode. No harm.", 0 );
     update_tune_code_in_progress = FALSE;
     return success;
  }


 rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;

 if(!rfwcdma_mdsp_associate_event_id(RF_WCDMA_EVENT_TUNER_OPEN_LOOP,rflm_handle))
 {
   RF_MSG_1(RF_HIGH,"rfwcdma_core_update_tuner_code: associate open loop event with DM failed!Not building scripts",0);
   update_tune_code_in_progress = FALSE;
   return FALSE;
 }

  /* Initialze all members of carrier path state to 0 */
 memset( car_path_state, 0, (size_t)RFCOM_MAX_CARRIERS * sizeof(rfwcdma_mc_car_path_state_type) );

 /* Start to Time Measurement */
 prof_tick = rf_time_get_tick();

 buf_ptr = rflm_dm_get_script_buf_ptr(rflm_handle,RF_WCDMA_EVENT_TUNER_OPEN_LOOP);

 /* Create Call back data and Tuner Script  */

   /* 1. Allocate memory for call back data and initialze all members to 0 */
   tuner_cb_data =  rfcommon_tuner_event_create_cb_data(buf_ptr, RFM_IMT_MODE,
                                                        (uint32)rflm_handle);
   if(tuner_cb_data != NULL)   
   {

     /* Obtain ccs event handle from call back data */
     tuner_event_handle_ptr = &(tuner_cb_data->tuner_ccs_event_handle);

   }
   else
   {
     RF_MSG_1( RF_ERROR, "rfwcdma_core_antenna_tuner_incremental_update:" 
                          "mem_alloc failed ",0);
       /* Reset update tune code flag */
      update_tune_code_in_progress = FALSE;
     return success;
   }

   /* 2. Create RF script object */
   tuner_script = rf_buffer_create ( 0, RFWCDMA_MAX_RFFE_SCRIPT_SIZE, 0 );
   
   if(tuner_script == NULL)
   {  
      RF_MSG_1( RF_ERROR, "rfwcdma_core_antenna_tuner_incremental_update: "
                            "Creating Antenna Tuner Script Failed ", 0 );
        /* Reset update tune code flag */
      update_tune_code_in_progress = FALSE;
      return success;
   } 


 /*** Program incremental updates for all the active devices ***/
  /* RFM_DEVICE_4 is actually maximum number of devices */
 for( rfm_device = RFM_DEVICE_0; rfm_device < RFM_DEVICE_4; rfm_device++)
 {

   /* Number of carriers supported by the device */
   valid_num_of_carrier = rfwcdma_mc_get_car_state_info_from_device(rfm_device, car_path_state);

   /* As Tuner does not depend on number of carriers(or channels) in one particular band for a device, do this only once for this particular device by checking the last index of car_path_state */ 
   if( valid_num_of_carrier > 0) 
   {
     if( ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RX ) || ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RXTX ) )
     {
       /* Program tuner scenario */
       band_u.wcdma_band = car_path_state[valid_num_of_carrier-1].curr_band_rx;

       /* 3. Get Qtuner object from RFC */
       qtuner_mgr_obj = rfc_wcdma_get_device( rfm_device, 
                                             RFC_CONFIG_RX, 
                                             car_path_state[valid_num_of_carrier-1].curr_band_rx, 
                                             RFDEVICE_TUNER_MANAGER );

       if (qtuner_mgr_obj == NULL)
       {
         RF_MSG_1(RF_ERROR,"ANT_TUNER: No Antenna tuner manager device found from RFC for path %d", rfm_device);
         continue;
       }

       else 
       {
          /*! Need to do: Loop it for all antenna devices per RFM device. Now doing it for only one Tuner device per RFM Device. So device_idx = 0*/
          if(qtuner_mgr_obj[device_idx] != NULL)
          {
     
              /* 4. Assign script object and tuner device pointer to call back data */
              tuner_cb_data->tuner_data[rfm_device].ant_tuner = (rfcommon_atuner_intf *)(qtuner_mgr_obj[device_idx]);
         

              /* 5. Obtain address of token from call back data*/
              chain_token_ptr = &(tuner_cb_data->tuner_data[rfm_device].tuner_token);
              
              /* 6. Update tune code based on the current status of WCDMA */
              /* TODO Combime the scripts for PCELL & SCELL under one tuner event to aviod corruption*/  
              if ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RX )
              {
                is_rx_mode = TRUE;
                success = rfcommon_atuner_manager_update_tune_code( (rfcommon_atuner_intf *)(qtuner_mgr_obj[device_idx]), /*Manager instance*/
                                                   RFCOM_WCDMA_MODE,           /*mode in use*/
                                                   band_u,                     /*band in use */
                                                   car_path_state[valid_num_of_carrier-1].curr_chan_rx ,/*channel in use*/
                                                   is_rx_mode, 
                                                   RFDEVICE_CREATE_SCRIPT,  /*Script execution type*/
                                                   tuner_script,                        /*Script pointer*/
                                                   0,                           /*Script timing*/
                                                   chain_token_ptr );             /*Script token*/
              }
              else
              {
                is_rx_mode = FALSE;
                success = rfcommon_atuner_manager_update_tune_code( (rfcommon_atuner_intf *)(qtuner_mgr_obj[device_idx]), /*Manager instance*/
                                                   RFCOM_WCDMA_MODE,           /*mode in use*/
                                                   band_u,                     /*band in use */
                                                   car_path_state[valid_num_of_carrier-1].curr_chan_tx ,/*channel in use*/
                                                   is_rx_mode, 
                                                   RFDEVICE_CREATE_SCRIPT,  /*Script execution type*/
                                                   tuner_script,                        /*Script pointer*/
                                                   0,                           /*Script timing*/
                                                   chain_token_ptr );             /*Script token*/
              }


              if(success == TRUE)
              {
                 /* skip mdsp update if token is invalid, to avoid redundant F3 messages in case of no Tuner QCN */
              	if(*chain_token_ptr != ATUNER_INVALID_TOKEN_VAL )
              	{
                  update_tuner_api_status = TRUE;
              }
              }

              else
              {
                RF_MSG_1( RF_ERROR, "rfwcdma_core_antenna_tuner_incremental_update: "
                                  "Antenna Tuner Manager Update Tune Code failed for device: %d", rfm_device );               
              }

              /* if((tuner_mdsp_chain == RF_PATH_MAX) || (cell_type == RFCOM_INVALID_CELL))*/
          } /*  if(qtuner_mgr_obj[device_idx] != NULL) */
        } /* if (qtuner_mgr_obj == NULL) */

      } /* if( ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RX ) || ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RXTX ) )*/

    }  /*    if( (valid_num_of_carrier > 0) */

  } /*   for( rfm_device = RFM_DEVICE_0; rfm_device < RFM_MAX_WAN_DEVICES; rfm_device++)*/
     
  /* A single event in rfcommon_mdsp_tuner_update_queue_script() pushes the script for both the chains(on a single device) and rfcommon_mc_tuner_update_response_handler() takes care for both the chains as well */   

   /* 7. Update mdsp data and register callback for FW response */
   if( update_tuner_api_status == TRUE )
    {
       /* Common Call for both SBDC and DBDC */

       *tuner_event_handle_ptr = NULL;
       if(rfwcdma_mc_state.car_path_state[0][0].rf_state != RFWCDMA_MC_STATE_RXTX)
       {
         RF_MSG_1(RF_ERROR,"rfwcdma_core_antenna_tuner_incremental_update:"
           "WL1 calling for tune code update in non RXTX mode. No harm.", 0 );
         success = FALSE;
       }
       else
       {
         success = rfcommon_mdsp_tuner_open_loop_queue_script( buf_ptr,
                                                               RF_WCDMA_EVENT_TUNER_OPEN_LOOP,
                                                               tuner_script,
                                                               tuner_event_handle_ptr,
                                                               rfcommon_tuner_open_loop_response_handler,
                                                               (void*)tuner_cb_data);
       }
        /* 8. If mdsp_tuner_update_queue_script() API fails, clean up objects and free memory */
       if ( success == FALSE)
       {
         /* Force cleaning up all allocated memory */
         rfcommon_tuner_event_cleanup(tuner_cb_data );

         RF_MSG_1( RF_ERROR, "rfwcdma_core_antenna_tuner_incremental_update: "
                                    "MDSP Tuner Update Queue Script failed for device 0", 0 );
       }
       else
       {
         rflm_tuner_intf = (rflm_cmn_tuner_event_data_type*) buf_ptr;
         aol_pending_snum[snum_counter%5] = rflm_tuner_intf->sequence_number;
         snum_counter++;		
       }
    } /*   if( update_tuner_api_status[0]  == TRUE ) */

   /* Clean up all allocated memory */
   if ( tuner_script != NULL )
   {
     rf_buffer_destroy( tuner_script );
   }

    if((update_tuner_api_status  == FALSE) && (tuner_cb_data != NULL))
    {
      rfcommon_tuner_event_cleanup( tuner_cb_data );
    }

    /* End Time Measurement */
   prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );

   RF_MSG_2( RF_HIGH, "rfwcdma_core_antenna_tuner_incremental_update: [Status %d] Profile %d", success, prof_time );

     /* Reset update tune code flag */
   update_tune_code_in_progress = FALSE;

   return success;
} /* rfwcdma_core_Antenna_tuner_incremental_update */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  rfwcdma_core_Antenna_tuner_incremental_update_imm

  @details
  This API should be registered by rfwcdma_asdiv_handle_set_antenna
  as a callback function which gets executed after switch completion.
  This is just a wrapper function to make each tech's callback function 
  same in terms of passed argument.
 
  @return
  void
*/
void rfwcdma_core_antenna_tuner_incremental_update_imm ( uint8 cb_data )
{
  rfwcdma_core_antenna_tuner_incremental_update();
}

/*---------------------------------------------------------------------------*/
/*!
  @brief
  rfwcdma_core_antenna_tuner_program_qfe_scenario

  @details
  This API will be called to program the tuner immediately and no script or FW 
  action is required. Inside this API check all the active paths then give a call to
  tuner manager associated with appropriate tuners
  
  @return
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)
*/
  boolean rfwcdma_core_antenna_tuner_program_qfe_scenario ( void )
  {
   boolean success = FALSE;
   rfcom_band_type_u band_u;
   void **qtuner_mgr_obj = NULL;
   rfm_device_enum_type rfm_device;
   rf_time_tick_type prof_tick;
   rf_time_type prof_time = 0; /* Time spent in function */
   boolean is_rx_mode = TRUE;
   uint16 tuner_mdsp_chain = 0;
   script_token_type dummy_token;
   rfwcdma_mc_car_path_state_type car_path_state[RFCOM_MAX_CARRIERS];
   uint8 valid_num_of_carrier;
   int8 device_idx = 0;
   rfcom_cell_type cell_type = RFCOM_PRIMARY_CELL;
  
    /* Initialze all members of carrier path state to 0 */
   memset( car_path_state, 0, (size_t)RFCOM_MAX_CARRIERS * sizeof(rfwcdma_mc_car_path_state_type) );
  
   /* Start to Time Measurement */
   prof_tick = rf_time_get_tick();
     
   /*** Program to the antenna tuner immediatelyfor all the active devices ***/
    /* RFM_DEVICE_4 is actually maximum number of devices */
   for( rfm_device = RFM_DEVICE_0; rfm_device < RFM_DEVICE_4; rfm_device++)
   {
  
     /* Number of carriers supported by the device */
     valid_num_of_carrier = rfwcdma_mc_get_car_state_info_from_device(rfm_device, car_path_state);
  
     /* As Tuner does not depend on number of carriers(or channels) in one particular band for a device, do this only once for this particular device by checking the last index of car_path_state */ 
     if( valid_num_of_carrier > 0) 
     {
       if( ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RX ) || ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RXTX ) )
       {
         /* Program tuner scenario */
         band_u.wcdma_band = car_path_state[valid_num_of_carrier-1].curr_band_rx;
  
         /*  Get Qtuner object from RFC */
         qtuner_mgr_obj = rfc_wcdma_get_device( rfm_device, 
                                               RFC_CONFIG_RX, 
                                               car_path_state[valid_num_of_carrier-1].curr_band_rx, 
                                               RFDEVICE_TUNER_MANAGER );
  
         if (qtuner_mgr_obj == NULL)
         {
           RF_MSG_1(RF_ERROR,"ANT_TUNER: No Antenna tuner manager device found from RFC for path %d", rfm_device);
           continue;
         }
  
         else 
         {
            /*! Need to do: Loop it for all antenna devices per RFM device. Now doing it for only one Tuner device per RFM Device. So device_idx = 0*/
            if(qtuner_mgr_obj[device_idx] != NULL)
            {
       
             tuner_mdsp_chain = (uint16)rfcommon_core_device_to_path(rfm_device);
             
             cell_type = rfcommon_core_convert_rfm_device_to_cell_type(rfm_device);
  
             if((tuner_mdsp_chain >= RF_TUNER_CHAIN_NUM) || (cell_type == RFCOM_INVALID_CELL))
             {
               RF_MSG_1( RF_ERROR, "rfwcdma_core_antenna_tuner_program_qfe_scenario:" 
                                    "Invalid path and cell for device: %d", rfm_device );
               continue;
             }
  
             else
             {
                /*  Update tune code based on the current status of WCDMA */
                if ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RX )
                {
                  is_rx_mode = TRUE;
                  success = rfcommon_atuner_manager_force_detune( (rfcommon_atuner_intf *)(qtuner_mgr_obj[device_idx]), /*Manager instance*/
                                                     RFCOM_WCDMA_MODE,           /*mode in use*/
                                                     band_u,                     /*band in use */
                                                     car_path_state[valid_num_of_carrier-1].curr_chan_rx ,/*channel in use*/
                                                     is_rx_mode, 
                                                     RFDEVICE_EXECUTE_IMMEDIATE,  /*Script execution type*/
                                                     NULL,                        /*Script pointer*/
                                                     0,                           /*Script timing*/
                                                     &dummy_token );             /*Script token*/
                }
                else
                {
                  is_rx_mode = FALSE;
                  success = rfcommon_atuner_manager_force_detune( (rfcommon_atuner_intf *)(qtuner_mgr_obj[device_idx]), /*Manager instance*/
                                                     RFCOM_WCDMA_MODE,           /*mode in use*/
                                                     band_u,                     /*band in use */
                                                     car_path_state[valid_num_of_carrier-1].curr_chan_tx ,/*channel in use*/
                                                     is_rx_mode, 
                                                     RFDEVICE_EXECUTE_IMMEDIATE,  /*Script execution type*/
                                                     NULL,                        /*Script pointer*/
                                                     0,                           /*Script timing*/
                                                     &dummy_token );             /*Script token*/
                }
  
  
                if(success != TRUE)
                {
                  RF_MSG_1( RF_ERROR, "rfwcdma_core_antenna_tuner_program_qfe_scenario: "
                                    "Antenna Tuner Manager Update Tune Code failed for device: %d", rfm_device );               
                }
  
              } /* if((tuner_mdsp_chain == RF_PATH_MAX) || (cell_type == RFCOM_INVALID_CELL))*/
            } /*  if(qtuner_mgr_obj[device_idx] != NULL) */
          } /* if (qtuner_mgr_obj == NULL) */
  
        } /* if( ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RX ) || ( car_path_state[valid_num_of_carrier-1].rf_state == RFWCDMA_MC_STATE_RXTX ) )*/
  
      }  /*    if( (valid_num_of_carrier > 0) */
  
    } /*   for( rfm_device = RFM_DEVICE_0; rfm_device < RFM_MAX_WAN_DEVICES; rfm_device++)*/
       
      /* End Time Measurement */
     prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );
  
     RF_MSG_2( RF_HIGH, "rfwcdma_core_antenna_tuner_program_qfe_scenario: [Status %d] Profile %d", success, prof_time );
  
     return success;
  } /* rfwcdma_core_Antenna_tuner_incremental_update */

#endif /* FEATURE_RF_HAS_QTUNER */
