#ifndef RFWCDMA_CORE_TXLIN_H
#define RFWCDMA_CORE_TXLIN_H
/*!
   @file
   rfwcdma_core_txlin.h

   @brief
   Provides WCDMA TX Linearizer Functionality

   @details
    

*/

/*===========================================================================

Copyright (c) 2008 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_core_txlin.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/15   ag      Add API to update pin through Autopin
09/15/14   vbh     Added device as a parameter to support Tx sharing
08/18/13   kai     Fix dpd_valid_entries index
02/13/13   kai     Update linearizer APIs for temp comp
06/14/13   kr      Add SMPS/RGI logging support for HS-APT
05/23/13   yb      Fixing the int16 array accesses and buffering of for 
                   pa_comp_up/down_vs_freq NVs
05/09/13   cd      Temp Comp - Support legacy and xPT temp comp when xPT is
                   enabled
03/26/13   rmb/kai Defined RFLIN_TX_MAX_PA_STATE_UNUSED to set txLinLutMaxPaState to -1 for Multi-Lin V1
03/20/13   cd      XPT Temp comp support
03/19/13   dw/kai  Added support for HDET based linearizer compensation
12/11/12   ac      pa_ range event builder support, pa q current as a txagc task
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type.
06/07/12   vs      Add WCDMA Temp comp support for EPT
06/04/12   cri     Add EPT support for WCDMA
05/01/12   cri     Updated EPT linearizer structure and processing
04/04/12   cri     Support for processing EPT linearizers 
03/29/12   cri     Support for loading EPT linearizers from NV
03/22/12   id      Added function to get the NV Id for the Tx Multi Lin NV
03/01/01   dw      [1] Add support for HSPA APT
                   [2] Rework some call flows for Tx AGC/linearizer control
02/27/12   swb     Return indication of missing cal data within txlin_update function
01/16/12   ac      fix the pa_comp_up_r1_vs_freq data type 
11/23/11   ac      kw fix for the Buffer overflow
11/18/11   ac      Enable PA Comp Up/Down to vary with frequency
11/16/11   dw      Change to update LUT buffer index after LUT for all PA ranges are updated
09/30/11   dw     [1] New Tx linearizer load functions
                  [2] Temp comp support
                  [3] MDSP Linearizer double buffer implementation
                  [4] Legacy code clean up
09/26/11   dw      Klocwork fixes
08/11/11    vb     Fix compiler warning
08/11/11    vb     Support for new multi-chan linearizer/APT
08/02/11   dw      Add default multi-linearizer index
08/02/11   dw      Add support to populate multi-linearizer data strucures
04/06/11   ad      Port APT changes from MDM9k
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
01/20/10   kguo    Removed rfwcdma_core_txlin_data declaration from this file 
11/06/08   av     Initial version.

============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "nv_items.h"
#include "rfc_card_wcdma.h"
#include "rfcommon_core_txlin.h"


/* Number of TX linearizer table */
#define RFLIN_NUM_TX_LIN_LEGACY_4PA       4 

/* Maxium linearizer entries for HS table */
#define RFLIN_MAX_HS_LINEARIZER_ENTRIES 16

/* Maximum number of TX linearizer values passed to mdsp */
/*! 
  @todo Changkee For the first step, I just used 32 index instead of 128.
  The reason choosing 32 is QDSP for MSM8200 platform couldn't support
  128 index.
  If this first step works fine, it should be changed to 128 items.
 */
//#define RFLIN_MDSP_MAX_SEGMENTS           128
#define RFLIN_MDSP_MAX_SEGMENTS           NV_TX_PDM_LIN_TYPE
#define RFLIN_ENH_MDSP_MAX_SEGMENTS   NV_TX_LIN_MASTER_ENH_SIZE
/* Number of element in each TX linearizer table */
#define RFLIN_TX_LIN_MASTER_SIZ           NV_WCDMA_TX_LIN_MASTER_SIZ

#define RFLIN_TX_DEFAULT_REF_CHAN_INDEX 7
#define RFLIN_TX_MAX_RGI_INDEX 128

#define RFLIN_TX_DEFAULT_REF_CHAN_INDEX 7
#define RFLIN_TX_MAX_RGI_INDEX 128

/* For Multi-Lin V1, use this to to set txLinLutMaxPaState to -1 */
#define RFLIN_TX_MAX_PA_STATE_BASED_LUT_IDX_NOT_USED -1

typedef struct
{
  tx_band_cal_type* txlin_data;
  rfnv_item_id_enum_type txlin_nv;
}rfwcdma_core_tx_multi_lin_data_type;

typedef enum
{
  TX_AGC_TEMP_COMP,
  TX_AGC_HDET_COMP,
  TX_AGC_AUTO_PIN_COMP,
  TX_AGC_NUM_COMP,
  TX_AGC_COMP_INVALID
}rfwcdma_core_txagc_comp_type;

typedef struct
{
  /* Multi Linearizer revision*/
  tx_multi_lin_rev_type multi_lin_rev;
  /* Number of valid PA states */
  uint8 num_valid_pa_state;
  /* Linearizer compensation type per PA state */
  rfwcdma_core_txagc_comp_type txagc_comp_type[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* R99 Linearizer table shadow for the current Tx frequency */
  tx_linearizer_table_type current_lin_tbl[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* XPT table shadow for the current Tx frequency */
  xpt_data_type *current_xpt_tbl[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* Current R99 linearizer table after temp/hdet compensation */
  tx_linearizer_table_type current_lin_tbl_comp[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* Aux data flag */
  tx_linearizer_aux_data_type current_aux_data[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* R99 Linearizer table shadow size*/
  uint8 txlin_valid_entries[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* Valid DPD entries */
  uint16 dpd_valid_entries[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* Flag to indicate Tx Linearizer HDET comp in progress */
  boolean hdet_comp_in_progress;
  /* Current Temp Comp Algorithm being used by the driver */
  rfcom_txlin_temp_comp_algo_type temp_comp_algo;
  /* Current Pin Comp value */
  int16 txlin_pin_comp[RFCOM_TXLIN_NUM_LINEARIZERS];
  /* Current Pout Comp value */
  int16 txlin_pout_comp[RFCOM_TXLIN_NUM_LINEARIZERS];
}rfwcdma_core_txlin_record_type;

/*===========================================================================

                     EXTERNALIZED DATA DECLARATIONS
===========================================================================*/

/*===========================================================================

                     FUNCTION DECLARATIONS
===========================================================================*/

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_txlin_update
( 
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 rf_chan_tx,
  uint32 txlm_handle
);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txlin_init(void);

/*--------------------------------------------------------------------------*/
tx_band_cal_type *rfwcdma_core_txlin_get_band_data( rfcom_wcdma_band_type band );

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_txlin_hdet_temp_comp_update
(
  rfm_device_enum_type device,
  int16 pin_comp_val[],
  int16 pout_comp_val[],
  rfwcdma_core_txagc_comp_type update_type[]
);


/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_txlin_auto_pin_comp_update
(
  rfm_device_enum_type device,
  uint32 tx_handle,
  int16 pin_comp_val,
  uint8 pa_index 
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_txlin_reload_dpd
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 rf_chan_tx,
  uint32 txlm_handle
);

/*--------------------------------------------------------------------------*/
uint8 rfwcdma_core_txlin_get_num_valid_pa_state(rfcom_wcdma_band_type band);

/*--------------------------------------------------------------------------*/
rfwcdma_core_txagc_comp_type rfwcdma_core_txlin_get_comp_type(uint8 pa_state);

/*--------------------------------------------------------------------------*/
rfwcdma_core_txlin_record_type* rfwcdma_core_get_current_txlin_data(void);

#endif /* RFWCDMA_CORE_TXLIN_H */
