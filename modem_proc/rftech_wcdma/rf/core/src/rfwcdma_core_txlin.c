/*!
  @file
  rfwcdma_core_txlin.c

  @brief
  Provides WCDMA TX Linearizer Functionality

  @details

  @todo changlee All Tx linearizer functions should be reviewed after Tx AGC
  LUT operation implemented.
*/

/*===========================================================================

Copyright (c) 2008 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/src/rfwcdma_core_txlin.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/15   vs      Apply Pin Ref at Tx tune
12/02/15   ag      Add API for pin update through Autopin
                   Skip pin update during temp comp update when autopin is enabled
04/24/15   kai     Populate power cap params in temp comp
02/13/15   cd      Tx Lin max power cap/boost updates
10/09/14   kai     Added api to reload XPT DPD tables
10/01/14   kai     Added ref channel xpt pin offset txlin_update
09/24/14   rmb     Fix CW.
09/15/14   vbh     [1] Added device as a parameter to support Tx sharing
                   [2] Removed hardcoding on device 0
08/08/14   kai     Fix potential memory leak
07/21/14   aro/kai Added expansion ratio param in DPD expansion function
05/09/14   kai     Do not populate lin to RFLM if Tx is disabled in the mid of temp comp
04/20/14   bn      WCDMA 8 PA state support and related cleanup
02/25/14   zhh     fix KW warning
02/24/14   kai     Reset status_flag if linearizer data not present for one PA state
02/13/14   kai     Update linearizer APIs for temp comp
01/07/14   cd      Fixed xPT feature compiler error
01/06/14   cd      Linearizer and Tx AGC updates for loading xPT information to 
                   RFLM
12/23/13   vbh     Init_TX timing optimization 
11/25/13   kai     Added generic API to query WCDMA Tx device pointers
10/20/13   dw      Fix warning
10/15/13   dw      Fix invalid error message
10/10/13   dw      Skip linearizer process that is handled by common txlin.
10/08/13   kai     Added common TxAGC  
09/25/13   vbh     Warning fixes
08/12/13   kai     Added DPD scaling support 
08/08/13   vws     Fix Klockwork error (De-referencing Null Pointer)
08/01/13   kr       replace memcpy with memscpy
07/26/13   kr      Add SMPS/RGI logging support for HS-APT
07/17/13   yb      Fix for RGI starting from 0 value
07/15/13   yb      Fix crash in Freq Comp with reduced channel list
06/07/13   jf      Fix LTE freq comp
05/29/13   vs      Fix memory leaks with first sweep data when XPT is enabled
05/24/13   dw      Fix fow HSDPA HHO issue using HSAPT lin.
05/23/13   yb      Fixing the int16 array accesses and buffering of for 
                   pa_comp_up/down_vs_freq NVs
05/09/13   cd      Temp Comp - Support legacy and xPT temp comp when xPT is
                   enabled
04/04/13   ka      WCDMA state machine updates
03/28/13   bn      Support to process 'No EPT' Protocal Linearizer NV (MultiLin_V3).
03/26/13   rmb/kai Setting txLinLutMaxPaState to -1 when Multi-Lin V1 is used so that 
                   FW always look at txLinLutIdx to choose the lineariser table.
03/22/13   kai     Replace variable rfwcdma_core_tx_multi_lin_in_use 
03/21/13   Saul    MultiLin. Hooked up V3 NV PA current to mission mode struct.
03/20/13   cd      XPT Temp comp support
03/20/13   Saul    MultiLin. Each RGI now has info for which freqcomp to use.
03/20/13   kai     Removed TX_MULTI_LIN_REV_2
03/19/13   dw/kai  Added support for HDET based linearizer compensation
03/11/13   bn      check to ensure DPD processing only for non-Multilin V1 NVs
03/08/13   bn      XPT TxAGC update
03/08/13   bn      MultiLin_V3 NV read fix
03/07/13   bn      KW and OFT error fixes
03/07/13   bn      XPT TxAGC linearizer update
03/01/13   kai     Clean up old EPT DPD functions call
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type.
09/18/12    ac     rfc to take device as parameter
09/05/12     ka    Skipped loading of V3 NV until it is ready.
08/21/12   Saul    MultiLin. Added support to load EPT V2 and ET NV Data.
                   Renamed API to reflect its freq-comp work.
08/17/12  cri/Saul WCDMA. Added dpd copy API calls due to removal of
                   dpd_data copying in compensation APIs
07/09/12   vs      Fixed QPA temp comp issues
06/12/12   cri     Fixed remaining issues for W EPT online mode support  
06/07/12   vs      Add WCDMA Temp comp support for EPT
06/05/12   cri     Fix for KW and compiler Warning
06/04/12   cri     Add EPT support for WCDMA
05/01/12   cri     Updated EPT linearizer structure and processing
05/03/12   nv      Fixed KW warning
04/05/12   cri     Fix compiler issue
04/05/12   cri     Support for processing EPT linearizers 
04/04/12   swb     Add multi-band support for HSPA APT 
03/29/12   cri     Fix compiler issue
03/29/12   cri     Support for loading EPT linearizers from NV
03/27/12   sty     Changed return value in rfwcdma_core_txlin_get_band_data()
03/22/12   id      Added function to get the NV Id for the Tx Multi Lin NV
03/12/12   cri     EPT linearizer support in tx_band_cal_type
03/06/12   vb      Initialize all PA states to vaild part of rfwcdma_core_txlin_update()
03/02/12   ac      fix for cal v2
03/01/01   dw      [1] Add support for HSPA APT
                   [2] Rework some call flows for Tx AGC/linearizer control
02/27/12   swb     Return indication of missing cal data within txlin_update function
02/17/12   ac      fix for heap exhaustion
01/30/12   ac      pa_comp_up_vs_freq offset fix for pa_comp     
01/25/12   dw      Fix KW error
01/17/12   kg      Support for QPA state switch in online mode
01/13/12   dw      Change three local txlin tables to global to avoid overflowing stack
12/02/11   swb     Minor update to name rfwcdma_core_current_txlin_data
11/23/11   ac      kw fix for the Buffer overflow 
11/22/11   ac      compiler warning fix
11/18/11   ac      Enable PA Comp Up/Down to vary with frequency
11/16/11   dw      Change to update LUT buffer index after LUT for all PA ranges are updated
10/26/11   dw      Support for closed loop AGC in FTM
09/30/11   dw     [1] New Tx linearizer load functions
                  [2] Temp comp support
                  [3] MDSP Linearizer double buffer implementation
                  [4] Legacy code clean up   
09/26/11   dw      Klocwork fixes
09/21/11   dw      Replace modem_mem_alloc with modem_mem_calloc when creating TxLin data structure
09/07/11   dw      Fixed incorrect index in the LUT
09/07/11   dw      Use default SMPS from RFC for legacy Cal data conversion
09/06/11   dw      APT cleanup
08/23/11    ac     tech specific command dispatch
08/13/11    vb     Update the number of valid entries in Tx calibration channel list
08/11/11    vb     Fix perforce merge errors
08/11/11    vb     Fix compiler warning & an error
08/11/11   vb      Support for new multi-chan linearizer/APT 
08/11/11   vb      Removed all the freq comp update functions
08/08/11   dw      Remove legacy RFC function call
08/03/11   dw      Fix warning
08/02/11   dw      Add support to populate multi-linearizer data strucures
04/21/11   ad      Fix KW errors  
04/21/11   vb      Fix KW errors 
04/20/11   ad      Fix warning
04/11/11   sar     Replaced rfwcdma_core_convert_band_rfcom_to_rfnv with 
                   rfnv_wcdma_convert_band_rfcom_to_rfnv and included rfnv_wcdma.h.
04/08/11   aki     Moved variable declaration to beginning of a functions to make MSVC happy
04/06/11   ad      Port APT changes from MDM9k
03/22/11   dw      Fix warning 
03/07/11   dw      clean up LOCKs
11/08/10   ka      Remove warning 
10/29/10   ka      Updates for initial WCDMA APT functionality   
10/19/10   vb      Removed rfdevice_interface.h 
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
01/27/10   ka      Load linearizers from mdsp module instead of device.
01/20/10   kguo    Moved rfwcdma_core_txlin_data declaration to this file
12/02/09   jfc     Lint fix: Suppress lint warning because lint doesn't see how 
                    we check for array index out of bounds
11/06/09   av      Check WCDMA state for RXTX before writing the LUT tables.
                   Fixes corruption in GtoW.
10/23/09   bn      Freq/Temp update to MDSP
08/19/09    ka     Added header file
08/13/09   jhe     freq comp change
03/19/09   ckl     Added RF_SATURATE macro. Need to be defined in common header.
03/19/09   ckl     Arrange order of header files
03/17/09   ckl     Added RF_HAS_LINEAR_INTERPOLATION compile option
03/09/09   ckl     Code cleanup
02/03/09   ckl     Add LUT operation.
11/06/08   av      Initial version.

============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfumts.h"
#include "rfnv_wcdma.h"
#include "rfwcdma_mdsp.h"
#include "rfwcdma_core_txlin.h"
#include "rfwcdma_core.h"
#include "rfwcdma_mc.h"
#include "rfwcdma_core_util.h"
#include "rfnv_wcdma.h"
#include "modem_mem.h"
#include "rfcommon_nv_mm.h"
#include "ftm.h"
#include "rfwcdma_core_temp_comp.h"
#include "rfm_internal.h"
#include "rfcommon_math.h"
#include "rfcommon_core_txlin.h"
#include "rfwcdma_core_xpt.h"
#include "rfwcdma_core_txctl.h"
#include "rfwcdma_mdsp_async.h"
#include "rfcommon_autopin_api.h"
#include "rflm_api_autopin_hw.h"

/* memscpy */
#include "stringl.h"


extern rfwcdma_mc_state_type rfwcdma_mc_state;
extern int16 pout_comp_auto_pin_ref[RFCOM_TXLIN_NUM_LINEARIZERS];
extern int16 pin_comp_auto_pin_ref[RFCOM_TXLIN_NUM_LINEARIZERS];

rfnv_item_id_enum_type rfwcdma_core_txlin_nv_v1[RFCOM_NUM_WCDMA_BANDS] =
{
  RFNV_WCDMA_2100_TX_MULTI_LIN_DATA_I,  /* BC1 */
  RFNV_WCDMA_1900_TX_MULTI_LIN_DATA_I,  /* BC2 */
  RFNV_WCDMA_1800_TX_MULTI_LIN_DATA_I,  /* BC3 */  
  RFNV_WCDMA_BC4_TX_MULTI_LIN_DATA_I,   /* BC4 */  
  RFNV_WCDMA_800_TX_MULTI_LIN_DATA_I,   /* BC5 */
  RFNV_WCDMA_900_TX_MULTI_LIN_DATA_I,   /* BC8 */
  RFNV_WCDMA_1800_TX_MULTI_LIN_DATA_I,  /* BC9 */
  RFNV_WCDMA_1500_TX_MULTI_LIN_DATA_I,  /* BC11 */
  RFNV_WCDMA_800_TX_MULTI_LIN_DATA_I,  /* BC19 */
};

rfnv_item_id_enum_type rfwcdma_core_txlin_nv_v3[RFCOM_NUM_WCDMA_BANDS] =
{
  RFNV_WCDMA_2100_TX_MULTI_LIN_V3_DATA_I,  /* BC1 */
  RFNV_WCDMA_1900_TX_MULTI_LIN_V3_DATA_I,  /* BC2 */
  RFNV_WCDMA_1800_TX_MULTI_LIN_V3_DATA_I,  /* BC3 */  
  RFNV_WCDMA_BC4_TX_MULTI_LIN_V3_DATA_I,   /* BC4 */  
  RFNV_WCDMA_800_TX_MULTI_LIN_V3_DATA_I,   /* BC5 */
  RFNV_WCDMA_900_TX_MULTI_LIN_V3_DATA_I,   /* BC8 */
  RFNV_WCDMA_1800_TX_MULTI_LIN_V3_DATA_I,  /* BC9 */
  RFNV_WCDMA_1500_TX_MULTI_LIN_V3_DATA_I,  /* BC11 */
  RFNV_WCDMA_800_TX_MULTI_LIN_V3_DATA_I,   /* BC19 */
};

/*!
  @brief   
    This structure holds the linearizer data copied from either 
    V3 NVs(If available) or V1 NVs(backwards compatibilty) 
    for all supported WCDMA bands */
tx_band_cal_type* rfwcdma_core_tx_multi_lin_data[RFCOM_NUM_WCDMA_BANDS] =
{
  NULL,  /* BC1 */
  NULL,  /* BC2 */
  NULL,  /* BC3 */  
  NULL,  /* BC4 */  
  NULL,  /* BC5 */
  NULL,  /* BC8 */
  NULL,  /* BC9 */
  NULL,  /* BC11 */
  NULL, /* BC19 */
};

rfwcdma_core_txlin_record_type rfwcdma_core_current_txlin_data;

/*!
  @brief
  This function copies the linearizer data from either V3 NVs(If available) or V2 NVs(backwards compatibilty) 
  for all supported WCDMA bands 

  @details

  @param

*/
void rfwcdma_core_txlin_init(void)
{
  rfcom_wcdma_band_type band;
  boolean init_status;

  for (band = RFCOM_BAND_IMT; band<RFCOM_NUM_WCDMA_BANDS; band++)
  {
    if (rfwcdma_core_is_band_supported(band,RF_PATH_0))
    {
      /* Allocate memory to load linearizer */
      if (rfwcdma_core_tx_multi_lin_data[band] == NULL)
      {
        rfwcdma_core_tx_multi_lin_data[band] = 
          modem_mem_calloc(1,sizeof(tx_band_cal_type),MODEM_MEM_CLIENT_RFA);
      }

      if (rfwcdma_core_tx_multi_lin_data[band] == NULL)
      {
        /* Memory allocation failed */
        RF_MSG_1( RF_ERROR, "memory allocation failed for Txlin data on band:%d", band);
      }
      else
      {
        init_status = 
          rfcommon_core_load_linearizer_from_nv( TX_MULTI_LIN_REV_3,
                                                 rfwcdma_core_txlin_nv_v3[band],
                                                 rfwcdma_core_tx_multi_lin_data[band],
                                                 (rfcommon_core_convert_freq_handler_type)(&rfwcdma_core_get_freq_from_ul_arfcn) );
        /* Only newest revision should be loaded.
           If loading previous version failed... try multilin V1 */
        if ( init_status == FALSE )
        {
          RF_MSG_2( RF_ERROR,
                    "rfwcdma_core_txlin_init: Failed to load multilin V3 nv ID %d band %d. "
                    "Trying multilin V1",rfwcdma_core_txlin_nv_v3[band], band);
        
          init_status = 
            rfcommon_core_load_linearizer_from_nv( TX_MULTI_LIN_REV_1,
                                                   rfwcdma_core_txlin_nv_v1[band],
                                                   rfwcdma_core_tx_multi_lin_data[band],
                                                   (rfcommon_core_convert_freq_handler_type)(&rfwcdma_core_get_freq_from_ul_arfcn) );
          /* If even NV rev 1 failed */
          if (!init_status)
          {
            RF_MSG_1( RF_ERROR, "rfwcdma_core_txlin_init: "
                      "Failed to load multilin nv for band %d. ", band);
            /* free up the memeory*/
            modem_mem_free((void*)rfwcdma_core_tx_multi_lin_data[band], MODEM_MEM_CLIENT_RFA);
            rfwcdma_core_tx_multi_lin_data[band] = NULL;
          }
        }
      }/* (rfwcdma_core_tx_multi_lin_data[band] == NULL) */
    }/* if band supported */
  }/* for NUM BANDS */
}/* rfwcdma_core_txlin_init */

/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This fucntion initializes parameters per band 
    
  @details 
  [1] Checks the multi-lin NV to determine the multi-lin version for the given band 
  [2] Initalize the linearizer compensation type per band 
*/
void rfwcdma_core_txlin_band_init(rfcom_wcdma_band_type band)
{
  uint8 pa_state;

  rfwcdma_core_current_txlin_data.multi_lin_rev = TX_MULTI_LIN_REV_3;
  rfwcdma_core_current_txlin_data.num_valid_pa_state = 
    rfwcdma_core_txlin_get_num_valid_pa_state(band);

  /* Initialize the Tx AGC linearizer compensation type per PA state */
  /* With EPT/Multi-Lin V2, top PA state is HDET comp, other PA states are temp comp*/
  for (pa_state = 0; pa_state < RFCOM_TXLIN_NUM_LINEARIZERS; pa_state++)
  {
    rfwcdma_core_current_txlin_data.txagc_comp_type[pa_state] = TX_AGC_COMP_INVALID;

    if (pa_state<(rfwcdma_core_current_txlin_data.num_valid_pa_state-1))
    {
      rfwcdma_core_current_txlin_data.txagc_comp_type[pa_state] = TX_AGC_TEMP_COMP;
    }

    if (pa_state ==(rfwcdma_core_current_txlin_data.num_valid_pa_state-1))
    {
      rfwcdma_core_current_txlin_data.txagc_comp_type[pa_state] = TX_AGC_TEMP_COMP;
    }

    if (pa_state >(rfwcdma_core_current_txlin_data.num_valid_pa_state-1))
    {
      RF_MSG_2( RF_ERROR, "rfwcdma_core_txlin_band_init: "
                "invalid PA state %d, num of valid PA states %d",
                pa_state,(rfwcdma_core_current_txlin_data.num_valid_pa_state-1));
    }
  }
}/* rfwcdma_core_txlin_band_init */

/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This function populate the Tx linearizer for the given frequency, apply temp 
  comp, and load it to mdsp FW 

  @details
   
  @return
  TRUE if all RFcal data is present
  FALSE if missing RFcal data   
*/
boolean rfwcdma_core_txlin_update( rfm_device_enum_type device,
                                   rfcom_wcdma_band_type band,
                                   uint16 rf_chan_tx,
                                   uint32 txlm_handle )
{
  rfcommon_core_xpt_tx_agc_info_type xpt_info;
  uint16 num_rgi_with_dpd_info;
  xpt_dpd_scaling_value_type dpd_scale_factor;
  xpt_expansion_ratio_type expansion_ratio;

  boolean status_flag = TRUE;
  uint8 pa_index = 0;
  int32 ref_xpt_pin_offset =0;
  tx_linearizer_search_param_type search_param;
  int16 pin_comp_val[RFCOM_TXLIN_NUM_LINEARIZERS];
  int16 pout_comp_val[RFCOM_TXLIN_NUM_LINEARIZERS];
  tx_linearizer_table_data_type txlin_first_sweep_tbl;
  tx_linearizer_table_data_type txlin_final_table;
  tx_linearizer_table_data_type txlin_cur_table;
  rfnv_wcdma_nv_tbl_type * nv_tbl;
  /* Common load linearizer */
  rfcom_band_type_u rfcom_band;
  rfcommon_core_txlin_agc_device_info device_info;
  rfcommon_core_txlin_agc_pa_stm_info pa_stm_info;
  /* Power cap/boost algorithm */
  rfcommon_core_txlin_cap_boost_type cap_boost_data;

  rfwcdma_mdsp_set_data_param_type txlin_temp_vs_freq = { 0 };

  /*No need to populate linearizer data to MDSP memory in FTM mode*/
  if ( (IS_FTM_IN_TEST_MODE()&&(rfm_get_calibration_state() == TRUE)))
  {
    return TRUE;  
  }

  /********************************************************************* 
    Initialization: get Tx band Cal data handler,populate search paramters, get the
    current temperature compensation data
   *********************************************************************/

  /* Get NV table for EPT/DPD config params */
  nv_tbl = rfwcdma_core_init_nv_table(band);

  if(nv_tbl == NULL)
  {
    RF_MSG( RF_ERROR, "WCDMA TxLin update: Null pointer access to NV data");
    return FALSE; 
  }

  /* Clear the Txlin shadow first */
  memset(&rfwcdma_core_current_txlin_data,0,sizeof(rfwcdma_core_current_txlin_data));

  /* Check the linearizer cal data for current band */
  if ( (rfwcdma_core_tx_multi_lin_data[band] == NULL) )
  {
    RF_MSG( RF_ERROR, "WCDMA TxLin update: Null pointer access to cal data");
    return FALSE;
  }

  if((rfwcdma_core_tx_multi_lin_data[band]->tx_cal_chan_size) > RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1( RF_ERROR, "Invalid WCDMA tx cal chan size: %d",
              (rfwcdma_core_tx_multi_lin_data[band]->tx_cal_chan_size)); 
    return FALSE;
  }

  /*Perform the necessary memory allocations*/
  txlin_first_sweep_tbl.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  if(txlin_first_sweep_tbl.lin_data == NULL)
  {
    RF_MSG( RF_ERROR, "Fail to allocate memory for processing linearizer");
    return FALSE;
  }
  txlin_final_table.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  if(txlin_final_table.lin_data == NULL)
  {
    modem_mem_free(txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_first_sweep_tbl.lin_data = NULL;
    RF_MSG( RF_ERROR, "Fail to allocate memory for processing linearizer");
    return FALSE;
  }
  /*! following early return should take care of memory free */

  /* Init Max PA state, Temp comp type, Multi Lin type */
  rfwcdma_core_txlin_band_init(band);

  /* Obtain the current temp comp algorithm and store it to device status */
  rfwcdma_core_current_txlin_data.temp_comp_algo = 
          rfcommon_core_txlin_get_temp_comp_algo( 
           (tx_multi_lin_rev_type)rfwcdma_core_tx_multi_lin_data[band]->nv_rev_type,
           (rfcom_txlin_temp_comp_nv_data_type*)&nv_tbl->lin_temp_comp_data );

  /* Cap-boost information from NV, used by freq comp */
  cap_boost_data.max_pwr_ovr_algo = rfwcdma_core_xpt_get_cfg_param( 
                                               band,
                                               XPT_CFG_CMN_MAX_PWR_OVR_ALGO );
  cap_boost_data.lin_cap_boost_nv_val = rfwcdma_core_xpt_get_cfg_param( 
                                               band, 
                                               XPT_CFG_CMN_MAX_PWR_OVR_VAL );

  /* Read the thermistor and get the Txlin temp comp data*/
  rfwcdma_core_get_txlin_temp_comp(device,pin_comp_val, pout_comp_val); /*Kai no seqence dependency */

  if(rfwcdma_core_xpt_is_enabled(band))
  {
    rf_common_xpt_init_dpd( device );
  }

  /*********************************************************************
     Start populating linearizer data for all PA ranges for voice/R99 waveform
  **********************************************************************/
  for ( pa_index = 0; pa_index < rfwcdma_core_current_txlin_data.num_valid_pa_state; pa_index++)
  {
    /* Clear final table for population */
    memset(txlin_final_table.lin_data,0,sizeof(tx_linearizer_table_type));
    txlin_final_table.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
    txlin_final_table.xpt_data = NULL;  

    /* Clear current first sweep table for population */
    memset(txlin_first_sweep_tbl.lin_data,0,sizeof(tx_linearizer_table_type));
    txlin_first_sweep_tbl.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA ;
    txlin_first_sweep_tbl.xpt_data = NULL;

    /* Ensure that the linearizer data is sorted prior to populating it */
    if( !x_is_sorted_u32( rfwcdma_core_tx_multi_lin_data[band]->tx_cal_freq, rfwcdma_core_tx_multi_lin_data[band]->tx_cal_chan_size ))
    {
      RF_MSG_1( RF_MED, " WCDMA TxLin update: frequency data is not sorted for pa %d", pa_index);
      continue;     
    }

      RF_MSG_1( RF_HIGH, "WCDMA TxLin update: "
                "Populate TxLin from Tx Band Cal data for PA index:%d ", pa_index);

    /*************************************************************************************************
        Populate Tx Linearizer LUT STEP 1: use common function to search for the linearizer table from
        Tx band Cal data, and apply frequency compensation for the given channel and PA range
     ***************************************************************************************/
    {
      /* Populate search parameter*/
      search_param.rfm_device = device;
      search_param.tx_freq = rfwcdma_core_get_freq_from_ul_arfcn(rf_chan_tx);
      search_param.tx_waveform = TX_LINEARIZER_VOICE; 
      search_param.pa_range = pa_index;

      status_flag &= rfcommon_core_freq_comp_linearizer( &search_param,
                                                         rfwcdma_core_tx_multi_lin_data[band],
                                                         &txlin_final_table,
                                                         (void*)&cap_boost_data,
                                                         TRUE );
      if (!status_flag)
      {
        RF_MSG_1( RF_MED, "WCDMA Tx Lin update: "
                  "Search linearizer table from Cal data failed with pa_index =%d ", pa_index);
        /* Continue reading next PA index may contain valid linearizer entries eg: APT,APT,Blank,ET */
        status_flag = TRUE;
        continue;
      }
      else
      {
        /* Need to sort TX Table in increasing order or else temp comp will bail out.
          *
          * QSORT uses the function given (rfcommon_core_qsort_compare_increasing_pwr)
          * to compare elements, and sorts the whole linearizer row based on comparing
          * tx power level.
          */
          qsort( &(txlin_final_table.lin_data->table),  
                RFCOM_TXLIN_NUM_SEGMENTS,
                sizeof(tx_linearizer_row_type), rfcommon_core_qsort_compare_increasing_pwr );
      }

      /* Search first sweep linearizer data */
      search_param.tx_waveform = TX_LINEARIZER_FIRST_SWEEP;
      status_flag &= rfcommon_core_freq_comp_linearizer( &search_param,
                                                         rfwcdma_core_tx_multi_lin_data[band],
                                                         &txlin_first_sweep_tbl,
                                                         (void*)&cap_boost_data,
                                                         TRUE );

      if (!status_flag)
      {
        RF_MSG_1( RF_MED, "WCDMA Tx Lin update: "
                  "Search first sweep linearizer table from Cal data failed with pa_index =%d ", pa_index);
        /* Continue reading next PA index may contain valid linearizer entries eg: APT,APT,Blank,ET */
        status_flag = TRUE;
        continue;
      }
      else
      {
       qsort( &(txlin_first_sweep_tbl.lin_data->table),  
              RFCOM_TXLIN_NUM_SEGMENTS,
              sizeof(tx_linearizer_row_type), rfcommon_core_qsort_compare_increasing_pwr );
      }

      memscpy(&rfwcdma_core_current_txlin_data.current_lin_tbl[pa_index], sizeof(tx_linearizer_table_type), 
              txlin_final_table.lin_data, sizeof(tx_linearizer_table_type));
      rfwcdma_core_current_txlin_data.current_aux_data[pa_index] = txlin_final_table.aux_lin_data;
      rfwcdma_core_current_txlin_data.txlin_valid_entries[pa_index] = RFCOM_TXLIN_NUM_SEGMENTS;
    }

    /*************************************************************************************************
        Populate Tx Linearizer LUT STEP 2: Apply temp comp to the current linearizer table
     *************************************************************************************************/
    {
      txlin_cur_table.lin_data = &rfwcdma_core_current_txlin_data.current_lin_tbl[pa_index];
      txlin_cur_table.aux_lin_data = rfwcdma_core_current_txlin_data.current_aux_data[pa_index];
      txlin_cur_table.xpt_data = txlin_final_table.xpt_data;

      ref_xpt_pin_offset = 0;
      if(txlin_cur_table.aux_lin_data == TX_LINEARIZER_EPT_DPD_V2_DATA)
      {
        ref_xpt_pin_offset =
          (int32)rfwcdma_core_xpt_get_cfg_param( band, XPT_CFG_CMN_REF_EPT_PIN_OFFSET );
      }
      else if(txlin_cur_table.aux_lin_data == TX_LINEARIZER_ET_DPD_DATA)
      {
        ref_xpt_pin_offset =  
          (int32)rfwcdma_core_xpt_get_cfg_param( band, XPT_CFG_CMN_REF_ET_PIN_OFFSET );
      }
      pin_comp_val[pa_index] += ref_xpt_pin_offset;
	  
      RF_MSG_2( RF_MED, "WCDMA Tx Lin update: "
                "ref_xpt_pin_offset %d for pa_index =%d ",ref_xpt_pin_offset, pa_index);

      status_flag  &= rfcommon_core_txlin_temp_vs_freq_comp_update_v2(
                                                        rfwcdma_core_current_txlin_data.temp_comp_algo,
                                                        &txlin_cur_table,
                                                        &txlin_final_table,
                                                        &txlin_first_sweep_tbl,
                                                        pin_comp_val[pa_index],
                                                        pout_comp_val[pa_index],
                                                        rfwcdma_core_current_txlin_data.txlin_valid_entries[pa_index],
                                                        FALSE);


      if (!status_flag)
      {
        RF_MSG_1( RF_MED, "WCDMA Tx Lin update: "
                  "Apply temp comp failed with pa_index =%d ", pa_index);
        /* Continue reading next PA index may contain valid linearizer entries eg: APT,APT,Blank,ET */
        status_flag = TRUE;
        continue;
      }

      /* Copy the temp-comped linearizer to shadow for logging */
      memscpy(&rfwcdma_core_current_txlin_data.current_lin_tbl_comp[pa_index], sizeof(tx_linearizer_table_type),
             txlin_final_table.lin_data,sizeof(tx_linearizer_table_type));
      rfwcdma_core_current_txlin_data.txlin_pin_comp[pa_index] = pin_comp_val[pa_index];
      rfwcdma_core_current_txlin_data.txlin_pout_comp[pa_index] = pout_comp_val[pa_index];
      rfwcdma_core_current_txlin_data.current_xpt_tbl[pa_index] = txlin_final_table.xpt_data;

      txlin_temp_vs_freq.data.set_tx_lin_temp_vs_freq.pin[pa_index] = pin_comp_val[pa_index];
      txlin_temp_vs_freq.data.set_tx_lin_temp_vs_freq.pout[pa_index] = pout_comp_val[pa_index];      

      pin_comp_auto_pin_ref[pa_index] = pin_comp_val[pa_index];
      pout_comp_auto_pin_ref[pa_index] = pout_comp_val[pa_index];
    }

    /* Update Pin Pout values for logging */
    txlin_temp_vs_freq.set_data_cmd = RFWCDMA_MDSP_SET_PIN_POUT;
    txlin_temp_vs_freq.rflm_handle = (rflm_dm_handle_id_t)txlm_handle;
    rfwcdma_mdsp_async_set_data(&txlin_temp_vs_freq);
    
    /*************************************************************************************************
        Config DPD data if current PA state is EPT/ET state
     *************************************************************************************************/            
    if(rfwcdma_core_xpt_is_enabled(band))
    {
      /* Initialize and load DPD variable for XPT PA ranges */
      if( (txlin_final_table.aux_lin_data == TX_LINEARIZER_EPT_DPD_V2_DATA) || 
          (txlin_final_table.aux_lin_data == TX_LINEARIZER_ET_DPD_DATA) )
      {

        /* Init DPD to invalid vals */
        memset( xpt_info.amam_bank_info, 0xFF, 64*sizeof(rfcommon_xpt_dpd_bank_info_type) );
        memset( xpt_info.ampm_bank_info, 0xFF, 64*sizeof(rfcommon_xpt_dpd_bank_info_type) );
        memset( xpt_info.epdt_bank_info, 0xFF, 64*sizeof(rfcommon_xpt_dpd_bank_info_type) );

        rf_common_xpt_dpd_scale_computation ( &nv_tbl->dpd_scaling_data,
                                              &nv_tbl->lin_temp_comp_data.tx_char_freq[0],
                                              nv_tbl->lin_temp_comp_data.tx_char_chan_size,
                                              rfwcdma_core_get_freq_from_ul_arfcn(rf_chan_tx),
                                              &dpd_scale_factor);
  
        if ( rfwcdma_mc_state.tx_multi_carrier_idx == RFWCDMA_MC_DC )
        {
          expansion_ratio.amam = 2;
          RF_MSG( RF_HIGH, "rfwcdma_core_txlin_update: DC Mode" );
        }
        else
        {
          expansion_ratio.amam = 4;
          RF_MSG( RF_HIGH, "rfwcdma_core_txlin_update: SC Mode" );
        }
        expansion_ratio.ampm = 4;
        expansion_ratio.epdt = 8;          
        status_flag &= rfcommon_core_populate_tx_dpd( device,
                                                     &txlin_final_table,
                                                     &(xpt_info.amam_bank_info[0]),
                                                     &(xpt_info.ampm_bank_info[0]),
                                                     &(xpt_info.epdt_bank_info[0]),
                                                     &num_rgi_with_dpd_info,
                                                     &dpd_scale_factor,
                                                     &expansion_ratio );
        if(!status_flag)
        {
          RF_MSG_2( RF_MED, "Populate DPD failed for pa_index =%d dpd_scale_factor =%d",
                    pa_index, dpd_scale_factor.scale_value);
          /* Continue reading next PA index may contain valid linearizer entries eg: APT,APT,Blank,ET */
          status_flag = TRUE;
          continue;
        }  
        
        rfwcdma_core_current_txlin_data.dpd_valid_entries[pa_index] = num_rgi_with_dpd_info;                                  
      }
    }/* Config DPD data if current PA state is EPT/ET state */

    /*************************************************************************************************
        Populate Tx Linearizer LUT STEP 3: Populate the final linearizer table to be loaded to FW
     *************************************************************************************************/
    /* Load the TX linearizer table (pertaining to the current Tx chan)
       for each PA range into MDSP */
    /* Common load linearizer */
    {
      /* Retrieve device info */
      rfwcdma_core_txctl_get_device_info( device,
                                          band, 
                                          &device_info );

      /* Update common band information */
      rfcom_band.wcdma_band = band;

      /* Initialize PA STM info */
      pa_stm_info.pa_state = pa_index;
      pa_stm_info.pa_range = rfwcdma_core_util_get_pa_range_from_nv(pa_index);

      /* Configure the linearizer to RFLM through common Tx AGC */ 
      status_flag &= rfcommon_core_txlin_config_init_linearizer( 
                           RFM_IMT_MODE, device, 
                           rfcom_band, txlm_handle,
                           &txlin_final_table, 
                           &txlin_first_sweep_tbl, 
                           &device_info,
                           &pa_stm_info,
                           (void*)&xpt_info); 
    }/* Common load linearizer */

  } /* for ( pa_index = 0; pa_index < rfwcdma_core_current_txlin_data.num_valid_pa_state; pa_index++) */

  if(rfwcdma_core_xpt_is_enabled(band))
  {
    rf_common_xpt_commit_dpd_data( device );
  }

  /*Deallocate the first sweep data */
  if (txlin_first_sweep_tbl.lin_data != NULL)
  {
    modem_mem_free((void*)txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_first_sweep_tbl.lin_data = NULL;
  }
  /*Deallocate the final lin data */
  if (txlin_final_table.lin_data != NULL)
  {
    modem_mem_free(txlin_final_table.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_final_table.lin_data = NULL;
  }

  return status_flag;
}/* rfwcdma_core_txlin_update */

/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This function update the Tx linearizer with temperature compensation only

  @details
  Initial linearizer on the current band/channel needs to be populated first 

*/
boolean rfwcdma_core_txlin_hdet_temp_comp_update
(
  rfm_device_enum_type device,
  int16 pin_comp_val[],
  int16 pout_comp_val[],
  rfwcdma_core_txagc_comp_type update_type[]
)
{
  boolean status_flag = TRUE;
  uint8 pa_index = 0;
  tx_linearizer_search_param_type search_param;
  tx_linearizer_table_data_type txlin_first_sweep_tbl;
  tx_linearizer_table_data_type txlin_final_table;
  tx_linearizer_table_data_type txlin_cur_table;
  /* Common load linearizer */
  rfcom_band_type_u rfcom_band;
  rfcommon_core_txlin_agc_pa_stm_info pa_stm_info;
  rfcom_wcdma_band_type curr_band_tx;
  /* Power cap/boost algorithm */
  rfcommon_core_txlin_cap_boost_type cap_boost_data;
  rfwcdma_mdsp_set_data_param_type txlin_temp_vs_freq = { 0 };

  /*No need to populate linearizer data to MDSP memory in FTM mode*/
  if ( (IS_FTM_IN_TEST_MODE()&&(rfm_get_calibration_state() == TRUE))||
       (rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state!=RFWCDMA_MC_STATE_RXTX))
  {
    return FALSE;  
  }

  /*Perform the necessary memory allocations*/
  txlin_first_sweep_tbl.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  if(txlin_first_sweep_tbl.lin_data == NULL)
  {
    RF_MSG( RF_ERROR, "Fail to allocate memory for processing linearizer");
    return FALSE;
  }
  txlin_final_table.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  if(txlin_final_table.lin_data == NULL)
  {
    modem_mem_free(txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_first_sweep_tbl.lin_data = NULL;
    RF_MSG( RF_ERROR, "Fail to allocate memory for processing linearizer");
    return FALSE;
  }
  /*! following early return should take care of memory free */
  
  /* Cap-boost information from NV, used by freq comp */
  cap_boost_data.max_pwr_ovr_algo = rfwcdma_core_xpt_get_cfg_param( 
                                               rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx,
                                               XPT_CFG_CMN_MAX_PWR_OVR_ALGO );
  cap_boost_data.lin_cap_boost_nv_val = rfwcdma_core_xpt_get_cfg_param( 
                                               rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx, 
                                               XPT_CFG_CMN_MAX_PWR_OVR_VAL );  
  
  for ( pa_index = 0; pa_index < rfwcdma_core_current_txlin_data.num_valid_pa_state; pa_index++)
  {
    if (update_type[pa_index] == TX_AGC_COMP_INVALID)
    {
      RF_MSG_1( RF_MED, "TxLin HDET Temp Comp no update on PA:%d ", pa_index);
      continue;
    }

    /* Check if the current linearizer table is valid */
    if (rfwcdma_core_current_txlin_data.current_lin_tbl[pa_index].chan_freq!=\
        rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx))
    {
      RF_MSG_1( RF_MED, "WCDMA Txlin temp comp: "
                "current TxLin data not valid for pa_index:%d",pa_index);
      continue;
    }
    
    /* Clear final table for population */
    memset(txlin_final_table.lin_data,0,sizeof(tx_linearizer_table_type));
    txlin_final_table.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
    txlin_final_table.xpt_data = NULL;  

    /* Clear current first sweep table for population */
    memset(txlin_first_sweep_tbl.lin_data,0,sizeof(tx_linearizer_table_type));
    txlin_first_sweep_tbl.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA ;
    txlin_first_sweep_tbl.xpt_data = NULL;

    /* Populate search parameter*/
    search_param.rfm_device = device;
    search_param.tx_freq = 
      rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
    search_param.tx_waveform = TX_LINEARIZER_FIRST_SWEEP; 
    search_param.pa_range = pa_index;

    /* Search first sweep linearizer data */
    curr_band_tx = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;

    if(curr_band_tx < RFCOM_NUM_WCDMA_BANDS)
    {
      status_flag &= rfcommon_core_freq_comp_linearizer( 
                                  &search_param,
                                  rfwcdma_core_tx_multi_lin_data[curr_band_tx],
                                  &txlin_first_sweep_tbl,
                                  (void*)NULL,
                                  TRUE );
    }
    else
    {
      status_flag = FALSE;
    }

    if (!status_flag)
    {
      RF_MSG_1( RF_MED, "WCDMA Tx Lin update: "
                "Search first sweep linearizer table from Cal data failed with pa_index =%d ", pa_index);
      /* Continue reading next PA index may contain valid linearizer entries eg: APT,APT,Blank,ET */
      status_flag = TRUE;
      continue;
    }
    else
    {
     qsort( &(txlin_first_sweep_tbl.lin_data->table),  
            RFCOM_TXLIN_NUM_SEGMENTS,
            sizeof(tx_linearizer_row_type), rfcommon_core_qsort_compare_increasing_pwr );
    }

    txlin_cur_table.lin_data = &rfwcdma_core_current_txlin_data.current_lin_tbl[pa_index];
    txlin_cur_table.aux_lin_data = rfwcdma_core_current_txlin_data.current_aux_data[pa_index];

	if ( !(rfcommon_autopin_is_enabled(RFM_IMT_MODE) && rflm_autopin_sanity_check(rfwcdma_mc_state.txlm_buf_idx)) || ( txlin_cur_table.aux_lin_data == TX_LINEARIZER_NO_AUX_DATA ))
    {
            
         status_flag  &= rfcommon_core_txlin_temp_vs_freq_comp_update(
                                                          rfwcdma_core_current_txlin_data.multi_lin_rev,
                                                          &txlin_cur_table,
                                                          &txlin_final_table,
                                                          &txlin_first_sweep_tbl,
                                                          pin_comp_val[pa_index],
                                                          pout_comp_val[pa_index],
                                                          rfwcdma_core_current_txlin_data.txlin_valid_entries[pa_index],
                                                          FALSE /* Don't Skip Pout Comp */);
            
    }

    else
    {
      pin_comp_val[pa_index] = 0; 
      pout_comp_val[pa_index] = 0;  
    }

    if (!status_flag)
    {
      RF_MSG_1( RF_MED, "WCDMA Tx Lin update: "
                "Apply temp comp failed with pa_index =%d ", pa_index);
      /* Continue reading next PA index may contain valid linearizer entries eg: APT,APT,Blank,ET */
      status_flag = TRUE;
      continue;
    }

    /* Copy the temp-comped linearizer to shadow for logging */
    memscpy(&rfwcdma_core_current_txlin_data.current_lin_tbl_comp[pa_index], sizeof(tx_linearizer_table_type),
           txlin_final_table.lin_data,sizeof(tx_linearizer_table_type));
    rfwcdma_core_current_txlin_data.txlin_pin_comp[pa_index] = pin_comp_val[pa_index];
    rfwcdma_core_current_txlin_data.txlin_pout_comp[pa_index] = pout_comp_val[pa_index];

    txlin_temp_vs_freq.data.set_tx_lin_temp_vs_freq.pin[pa_index] = pin_comp_val[pa_index];
    txlin_temp_vs_freq.data.set_tx_lin_temp_vs_freq.pout[pa_index] = pout_comp_val[pa_index];

    /* Load the TX linearizer table (pertaining to the current Tx chan)
       for each PA range into MDSP */
    /* Common load linearizer */
    {
      /* Update common band information */
      rfcom_band.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;

      /* Initialize PA STM info */
      pa_stm_info.pa_state = pa_index;
      pa_stm_info.pa_range = rfwcdma_core_util_get_pa_range_from_nv(pa_index);

      /* Do not populate linezrizer to RFLM if Tx is disabled in the mid of temp comp */
      if ( rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state!=RFWCDMA_MC_STATE_RXTX)
      {
        RF_MSG_1( RF_HIGH, 
                  "TxLin HDET/Temp Comp break, since Tx is disabled at PA state %d", pa_index);
        break;  
      }

	 if ( !(rfcommon_autopin_is_enabled(RFM_IMT_MODE) && rflm_autopin_sanity_check(rfwcdma_mc_state.txlm_buf_idx)) || ( txlin_cur_table.aux_lin_data == TX_LINEARIZER_NO_AUX_DATA ))
	{
      /* Configure the linearizer to RFLM through common Tx AGC */ 
      status_flag &= rfcommon_core_txlin_config_update_linearizer( 
                           RFM_IMT_MODE, device,
                           rfcom_band, rfwcdma_mc_state.txlm_buf_idx,
                           &txlin_final_table, 
                           &txlin_first_sweep_tbl,
                           &pa_stm_info,
                           FALSE /* Don't Skip Pout Comp */);
      }
    }/* Common load linearizer */
  }/* for ( pa_index = 0; pa_index < rfwcdma_core_current_txlin_data.num_valid_pa_state; pa_index++) */
  
  /* Update Pin Pout values for RFLM logging */
  txlin_temp_vs_freq.set_data_cmd = RFWCDMA_MDSP_SET_PIN_POUT;
  txlin_temp_vs_freq.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
  rfwcdma_mdsp_async_set_data(&txlin_temp_vs_freq);

  /*Deallocate the first sweep data */
  if (txlin_first_sweep_tbl.lin_data != NULL)
  {
    modem_mem_free((void*)txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_first_sweep_tbl.lin_data = NULL;
  }
  /*Deallocate the final lin data */
  if (txlin_final_table.lin_data != NULL)
  {
    modem_mem_free(txlin_final_table.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_final_table.lin_data = NULL;
  }

  RF_MSG_4( RF_HIGH, "TxLin HDET/Temp Comp finshed, Pin comp values:%d,%d,%d,%d", 
            pin_comp_val[0],pin_comp_val[1],pin_comp_val[2],pin_comp_val[3]);
  RF_MSG_4( RF_HIGH, "TxLin HDET/Temp Comp finshed, Pout comp values:%d,%d,%d,%d", 
            pout_comp_val[0],pout_comp_val[1],pout_comp_val[2],pout_comp_val[3]);

  return status_flag;
}/* rfwcdma_core_txlin_hdet_temp_comp_update */


/*--------------------------------------------------------------------------*/

/*!
  @brief 
  This function update the Tx linearizer with auto pin compensation

  @details
  Initial linearizer on the current band/channel needs to be populated first 

*/
boolean rfwcdma_core_txlin_auto_pin_comp_update
(
 rfm_device_enum_type device,
 uint32 tx_handle,
 int16 pin_comp_val,
 uint8 pa_index 
)
{
	  boolean status_flag = TRUE;
	  tx_linearizer_search_param_type search_param;
	  tx_linearizer_table_data_type txlin_first_sweep_tbl;
	  tx_linearizer_table_data_type txlin_final_table;
	  tx_linearizer_table_data_type txlin_cur_table;
	  /* Common load linearizer */
	  rfcom_band_type_u rfcom_band;
	  rfcommon_core_txlin_agc_pa_stm_info pa_stm_info;
	  rfcom_wcdma_band_type curr_band_tx;
	  /* Power cap/boost algorithm */
	  rfcommon_core_txlin_cap_boost_type cap_boost_data;
	  rfwcdma_mdsp_set_data_param_type txlin_temp_vs_freq = { 0 };
	
	  /*No need to populate linearizer data to MDSP memory in FTM mode*/
	  if ( (IS_FTM_IN_TEST_MODE()&&(rfm_get_calibration_state() == TRUE))||
		   (rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state != RFWCDMA_MC_STATE_RXTX))
	  {
		return FALSE;  
	  }
	
	
	  RF_MSG_3( RF_LOW, "rfwcdma_core_txlin_auto_pin_comp_update[Dev %d] : pin_comp_val = %d, pa_index =%d " , device, pin_comp_val, pa_index);
	
	  /*Perform the necessary memory allocations*/
	  txlin_first_sweep_tbl.lin_data = 
		(tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
	  if(txlin_first_sweep_tbl.lin_data == NULL)
	  {
		RF_MSG( RF_ERROR, "Fail to allocate memory for processing linearizer");
		return FALSE;
	  }
	  txlin_final_table.lin_data = 
		(tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 

	  if(txlin_final_table.lin_data == NULL)
	  {
		modem_mem_free(txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
		txlin_first_sweep_tbl.lin_data = NULL;
		RF_MSG( RF_ERROR, "Fail to allocate memory for processing linearizer");
		return FALSE;
	  }
	  /*! following early return should take care of memory free */
	  
	  /* Cap-boost information from NV, used by freq comp */
	  cap_boost_data.max_pwr_ovr_algo = rfwcdma_core_xpt_get_cfg_param( 
												   rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx,
												   XPT_CFG_CMN_MAX_PWR_OVR_ALGO );

	  cap_boost_data.lin_cap_boost_nv_val = rfwcdma_core_xpt_get_cfg_param( 
												   rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx, 
												   XPT_CFG_CMN_MAX_PWR_OVR_VAL );  
	  
	
		/* Check if the current linearizer table is valid */
		if (rfwcdma_core_current_txlin_data.current_lin_tbl[pa_index].chan_freq!=\
			rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx))
		{
		  RF_MSG_1( RF_MED, "WCDMA Txlin temp comp: "
					"current TxLin data not valid for pa_index:%d",pa_index);
		}
		
		/* Clear final table for population */
		memset(txlin_final_table.lin_data,0,sizeof(tx_linearizer_table_type));
		txlin_final_table.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
		txlin_final_table.xpt_data = NULL;	
	
		/* Clear current first sweep table for population */
		memset(txlin_first_sweep_tbl.lin_data,0,sizeof(tx_linearizer_table_type));
		txlin_first_sweep_tbl.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA ;
		txlin_first_sweep_tbl.xpt_data = NULL;
	
		/* Populate search parameter*/
		search_param.rfm_device = device;
		search_param.tx_freq = 
		  rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
		search_param.tx_waveform = TX_LINEARIZER_FIRST_SWEEP; 
		search_param.pa_range = pa_index;
	
		/* Search first sweep linearizer data */
		curr_band_tx = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;
	
		RF_MSG_2( RF_ERROR, "rfwcdma_core_txlin_auto_pin_comp_update current_band = %d, tx_freq = %d, pa_index =%d " , curr_band_tx,  search_param.tx_freq);
	
	   
		if(curr_band_tx < RFCOM_NUM_WCDMA_BANDS)
		{
		  status_flag &= rfcommon_core_freq_comp_linearizer( 
									  &search_param,
									  rfwcdma_core_tx_multi_lin_data[curr_band_tx],
									  &txlin_first_sweep_tbl,
									  (void*)NULL,
									  TRUE );
		}
		else
		{
		  status_flag = FALSE;
		}
	
		if (!status_flag)
		{
		  RF_MSG_1( RF_MED, "WCDMA Tx Lin update: "
					"Search first sweep linearizer table from Cal data failed with pa_index =%d ", pa_index);
		  /* Continue reading next PA index may contain valid linearizer entries eg: APT,APT,Blank,ET */
		  status_flag = TRUE;
		}
		else
		{
		 qsort( &(txlin_first_sweep_tbl.lin_data->table),  
				RFCOM_TXLIN_NUM_SEGMENTS,
				sizeof(tx_linearizer_row_type), rfcommon_core_qsort_compare_increasing_pwr );
		}
	
		txlin_cur_table.lin_data = &rfwcdma_core_current_txlin_data.current_lin_tbl[pa_index];
		txlin_cur_table.aux_lin_data = rfwcdma_core_current_txlin_data.current_aux_data[pa_index];
	
		if (( txlin_cur_table.aux_lin_data != TX_LINEARIZER_NO_AUX_DATA ) && 
			(txlin_cur_table.aux_lin_data != TX_LINEARIZER_DPD_INVALID ))
		{
	
		  RF_MSG_1( RF_LOW, "WCDMA Tx Lin update auto pin: calling rfcommon_core_txlin_temp_vs_freq_comp_update  %d ", txlin_cur_table.aux_lin_data );
	
		  status_flag  &= rfcommon_core_txlin_temp_vs_freq_comp_update(rfwcdma_core_current_txlin_data.multi_lin_rev,
															           &txlin_cur_table,
															           &txlin_final_table,
															           &txlin_first_sweep_tbl,
															           pin_comp_val,
															           0,
															           rfwcdma_core_current_txlin_data.txlin_valid_entries[pa_index],
															           TRUE /* Skip Pout Comp */);
		}
	
	    if (!status_flag)
		{
		  RF_MSG_1( RF_ERROR, "WCDMA Tx Lin update: "
					"Apply temp comp failed with pa_index =%d ", pa_index);
		  /* Continue reading next PA index may contain valid linearizer entries eg: APT,APT,Blank,ET */
		  status_flag = TRUE;
		}
	
		/* Copy the temp-comped linearizer to shadow for logging */
		memscpy(&rfwcdma_core_current_txlin_data.current_lin_tbl_comp[pa_index], sizeof(tx_linearizer_table_type),
			   txlin_final_table.lin_data,sizeof(tx_linearizer_table_type));
		
		rfwcdma_core_current_txlin_data.txlin_pin_comp[pa_index] = pin_comp_val;
		
		txlin_temp_vs_freq.data.set_tx_lin_temp_vs_freq.pin[pa_index] = pin_comp_val;
	
		/* Load the TX linearizer table (pertaining to the current Tx chan)
		   for each PA range into MDSP */
		/* Common load linearizer */
		{
		  /* Update common band information */
		  rfcom_band.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;
	
		  /* Initialize PA STM info */
		  pa_stm_info.pa_state = pa_index;
		  pa_stm_info.pa_range = rfwcdma_core_util_get_pa_range_from_nv(pa_index);
	
		  /* Do not populate linezrizer to RFLM if Tx is disabled in the mid of temp comp */
		  if ( rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state!=RFWCDMA_MC_STATE_RXTX)
		  {
			RF_MSG_1( RF_HIGH, 
					  "TxLin HDET/Temp Comp break, since Tx is disabled at PA state %d", pa_index);
		  }
		  
		  /* Configure the linearizer to RFLM through common Tx AGC */ 
	 	  status_flag &= rfcommon_core_txlin_config_update_linearizer( RFM_IMT_MODE, 
		                                                               device,
							                                           rfcom_band, 
							                                           tx_handle,
                                   							           &txlin_final_table, 
   							                                           &txlin_first_sweep_tbl,
							                                           &pa_stm_info,
							                                           TRUE /*Skip Pout Comp */);
	 
		}/* Common load linearizer */
	
	  /* Update Pin Pout values for RFLM logging */
	  txlin_temp_vs_freq.set_data_cmd = RFWCDMA_MDSP_SET_PIN_POUT;
	  txlin_temp_vs_freq.rflm_handle = (rflm_dm_handle_id_t)tx_handle;
	  rfwcdma_mdsp_async_set_data(&txlin_temp_vs_freq);
	
	  /*Deallocate the first sweep data */
	  if (txlin_first_sweep_tbl.lin_data != NULL)
	  {
		modem_mem_free((void*)txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
		txlin_first_sweep_tbl.lin_data = NULL;
	  }
	  /*Deallocate the final lin data */
	  if (txlin_final_table.lin_data != NULL)
	  {
		modem_mem_free(txlin_final_table.lin_data, MODEM_MEM_CLIENT_RFA);
		txlin_final_table.lin_data = NULL;
	  }
	
   RF_MSG_1( RF_LOW, "TxLin AutoPin Comp finished, Pin comp values:%d ", pin_comp_val);
	
	
	  return status_flag;
}/* rfwcdma_core_txlin_auto_pin_comp_update */


/*--------------------------------------------------------------------------*/
/*!
  @brief 
  Reload XPT DPD tables

  @details
  Reload XPT DPD tables on the current band/channel

*/
boolean rfwcdma_core_txlin_reload_dpd
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 rf_chan_tx,
  uint32 txlm_handle
)
{
  boolean status_flag = TRUE;
  uint8 pa_index = 0;
  tx_linearizer_table_data_type txlin_final_table;
  rfnv_wcdma_nv_tbl_type * nv_tbl;
  /* DPD variables */
  rfcommon_core_xpt_tx_agc_info_type xpt_info;
  uint16 num_rgi_with_dpd_info;
  xpt_dpd_scaling_value_type dpd_scale_factor;
  xpt_expansion_ratio_type expansion_ratio;

  /* Get NV table for EPT/DPD config params */
  nv_tbl = rfwcdma_core_init_nv_table(band);

  if(nv_tbl == NULL)
  {
    RF_MSG( RF_ERROR, "WCDMA reload DPD tables: Null pointer access to NV data");
    return FALSE; 
  }
 
  if(rfwcdma_core_xpt_is_enabled(band))
  {
    rf_common_xpt_init_dpd( device );
  }
   
  for ( pa_index = 0; pa_index < rfwcdma_core_current_txlin_data.num_valid_pa_state; pa_index++)
  {

    txlin_final_table.lin_data = &rfwcdma_core_current_txlin_data.current_lin_tbl_comp[pa_index];
    txlin_final_table.aux_lin_data = rfwcdma_core_current_txlin_data.current_aux_data[pa_index];
    txlin_final_table.xpt_data = rfwcdma_core_current_txlin_data.current_xpt_tbl[pa_index];

    /*************************************************************************************************
        Config DPD data if current PA state is EPT/ET state
     *************************************************************************************************/            
    if(rfwcdma_core_xpt_is_enabled(band))
    {
      /* Initialize and load DPD variable for XPT PA ranges */
      if( (txlin_final_table.aux_lin_data == TX_LINEARIZER_EPT_DPD_V2_DATA) || 
          (txlin_final_table.aux_lin_data == TX_LINEARIZER_ET_DPD_DATA) )
      {

        /* Init DPD to invalid vals */
        memset( xpt_info.amam_bank_info, 0xFF, 64*sizeof(rfcommon_xpt_dpd_bank_info_type) );
        memset( xpt_info.ampm_bank_info, 0xFF, 64*sizeof(rfcommon_xpt_dpd_bank_info_type) );
        memset( xpt_info.epdt_bank_info, 0xFF, 64*sizeof(rfcommon_xpt_dpd_bank_info_type) );

        rf_common_xpt_dpd_scale_computation ( &nv_tbl->dpd_scaling_data,
                                              &nv_tbl->lin_temp_comp_data.tx_char_freq[0],
                                              nv_tbl->lin_temp_comp_data.tx_char_chan_size,
                                              rfwcdma_core_get_freq_from_ul_arfcn(rf_chan_tx),
                                              &dpd_scale_factor);
  
        if ( rfwcdma_mc_state.tx_multi_carrier_idx == RFWCDMA_MC_DC )
        {
          expansion_ratio.amam = 2;
        }
        else
        {
          expansion_ratio.amam = 4;
        }
        expansion_ratio.ampm = 4;
        expansion_ratio.epdt = 8;          
        status_flag &= rfcommon_core_populate_tx_dpd( device,
                                                     &txlin_final_table,
                                                     &(xpt_info.amam_bank_info[0]),
                                                     &(xpt_info.ampm_bank_info[0]),
                                                     &(xpt_info.epdt_bank_info[0]),
                                                     &num_rgi_with_dpd_info,
                                                     &dpd_scale_factor,
                                                     &expansion_ratio );
        if(!status_flag)
        {
          RF_MSG_2( RF_ERROR, "Populate DPD failed for pa_index =%d dpd_scale_factor =%d",
                    pa_index, dpd_scale_factor.scale_value);
          /* Continue reading next PA index may contain valid linearizer entries eg: APT,APT,Blank,ET */
          status_flag = TRUE;
          continue;
        }                                  
      }
    }/* Config DPD data if current PA state is EPT/ET state */
  }/* for ( pa_index = 0; pa_index < rfwcdma_core_current_txlin_data.num_valid_pa_state; pa_index++) */

  return status_flag;
}/* rfwcdma_core_txlin_reload_dpd */

/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This function will return the max valid PA state based on NV setting of PA 
  switchpoints. 
 
  @details
  For all invalid PA state, switchpoints will be railed.  
   
*/
uint8 rfwcdma_core_txlin_get_num_valid_pa_state(rfcom_wcdma_band_type band)
{
  rfnv_wcdma_nv_tbl_type * nv_tbl = rfwcdma_core_init_nv_table(band);

  /* Null pointer check */
  if (nv_tbl == NULL)
  {
    RF_MSG( RF_ERROR, "rfwcdma_core_txlin_get_max_valid_pa_stateL:Null pointer access");
    return 0;
  }

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    /* Check if switchpoints are railed */
    if (nv_tbl->pa_static.pa_rise_sw[3] < 0x1FF)
    {
      return 4;
    }
    else if (nv_tbl->pa_static.pa_rise_sw[2] < 0x1FF)
    {
      return 3;
    }
    else if (nv_tbl->pa_static.pa_rise_sw[1] < 0x1FF)
    {
      return 2;
    }
    else
    {
      return 1;   
    }
  }
  else if(rfnv_wcdma_get_pa_static_nv_version(band) != RFWCDMA_NV_PA_STATIC_VER_INVALID)
  {
    return nv_tbl->pa_static.header[PA_STATIC_NUM_ACTIVE_PA];
  }
  else
  {
    return 0;
  }
}/* rfwcdma_core_txlin_get_num_valid_pa_state */

/*!
  @brief
  This function returns the multi linearizer data (copied from NVs) for
  the band requested

  @details

  @param band WCDMA band

*/
tx_band_cal_type *rfwcdma_core_txlin_get_band_data( rfcom_wcdma_band_type band )
{
  if(band >= RFCOM_NUM_WCDMA_BANDS)
  {
    RF_MSG_1( RF_ERROR, "rfwcdma_core_txlin_get_band_data: invalid band %d", band);
    return NULL;
  }

  if ( rfwcdma_core_tx_multi_lin_data[band] == NULL )
  {
    RF_MSG( RF_ERROR, "rfwcdma_core_txlin_get_band_data: multi lin data not available");
  }

  return rfwcdma_core_tx_multi_lin_data[band];
} /*rfwcdma_core_txlin_get_band_data*/

/*!
  @brief
  This function returns the Tx AGC compensation type for the current linearizer given PA state
  
  @details

  @return Tx AGC compensation type (TEMP COMP or HDET COMP)

*/
rfwcdma_core_txagc_comp_type rfwcdma_core_txlin_get_comp_type(uint8 pa_state)
{
  if (pa_state>=RFCOM_TXLIN_NUM_LINEARIZERS)
  {
    RF_MSG_1( RF_ERROR, "invalid PA state:%d",pa_state);
    return 0;
}
  return(rfwcdma_core_current_txlin_data.txagc_comp_type[pa_state]);
}/* rfwcdma_core_txlin_get_comp_type */

/*!
  @brief
  This function returns the pointer to the current Tx linearizer data
 
  @details
 
  @return pointer to the current Tx linearizer data

*/
rfwcdma_core_txlin_record_type* rfwcdma_core_get_current_txlin_data(void)
{
  return (&rfwcdma_core_current_txlin_data);
}/* rfwcdma_core_get_current_txlin_data */




