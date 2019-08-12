/*===========================================================================


      R F C  C o m m o n  G S M  c o n t r o l   A P I

                            S o u r c e  F i l e

DESCRIPTION
  This file contains LTE MEAS functions for the RFC.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.



when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/15   dw      Ported : GRFC Alt Path Support
09/26/14   yzw     Fix compiling warnings
06/29/12   sr      RFC changes : use rf_hal_buffer to generate scripts 
04/21/12   aca     Mode config request fixed to use the right enum
07/14/11   whc     Initial version
============================================================================*/


/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "rfc_meas_lte.h"
#include "rfcom.h"
#include "msg.h"

#include "rfcommon_core.h"
#include "rfc_common.h"
#include "rfc_card_lte.h"
#include "rflte_mc.h"



void rfc_meas_lte_generate_grfc_script(  uint32 band, rfc_grfc_device_type  *device_info, rfc_grfc_script_type *grfc_buf_ptr)
{

  rfc_sig_cfg_type *rfc_mode_grfc_tbl_ptr = NULL;
  rfc_sig_cfg_type *rfc_prx_grfc_tbl_ptr = NULL;
  rfc_sig_cfg_type *rfc_drx_grfc_tbl_ptr = NULL;
  rf_path_enum_type primary_path = RF_PATH_MAX;
  rf_path_enum_type secondary_path = RF_PATH_MAX;
  uint8 alt_path = RFLTE_MC_DEFAULT_ALTERNATE_PATH;

      /*@TODO 
       *
       *   Fix rfc_lte_get_mode_config
       */

    /* ==========================================================================
     * STEP 1: Get grfc table pointers for particular band and mode. 
     *  
     *          *Note:  Not all pointers must be populated for all techs.  NULL
     *                  pointers will be ignored below.
     * ========================================================================== 
     */ 

      /*convert device to path*/
      //primary_path = rfcommon_core_device_to_path( (rfcom_device_enum_type)device_info->primary_rx_device);
      //secondary_path = rfcommon_core_device_to_path( (rfcom_device_enum_type)device_info->secondary_rx_device);

      rfc_mode_grfc_tbl_ptr = (rfc_sig_cfg_type *)rfc_lte_get_mode_config(device_info->primary_rx_device, 
                                                                               RFC_MODE_RX_GRFC_ENGINE_DATA);
      rfc_prx_grfc_tbl_ptr =  (rfc_sig_cfg_type *)rfc_lte_get_rx_band_config(device_info->primary_rx_device,
                                                                                  (rfcom_lte_band_type)band, 
                                                                                  RFC_GRFC_DATA,
                                                                                  alt_path);
      rfc_drx_grfc_tbl_ptr =  (rfc_sig_cfg_type *)rfc_lte_get_rx_band_config(device_info->secondary_rx_device,
                                                                                  (rfcom_lte_band_type)band, 
                                                                                  RFC_GRFC_DATA,
                                                                                  alt_path);

     /* ==========================================================================
     * STEP 2: For each valid pointer, Look up GRFC Engine Number 
     *         and populate the IRAT GRFC buffer with correct logic.
     *  
     * ========================================================================== 
     */ 
  
      /*@Todo: check these fcn calls for true/false return*/
      if( rfc_mode_grfc_tbl_ptr != NULL )
      { 
        /* Dime RFC changes: Call the following API with rf_hal_buffer ptr */
        // rfc_common_generate_grfc_script( rfc_mode_grfc_tbl_ptr, grfc_buf_ptr );
      }
      else
      {
            MSG_ERROR("rfc_mode_grfc_tbl_ptr is NULL",0,0,0);
      }
  
      if( rfc_prx_grfc_tbl_ptr != NULL )
      { 
        /* Dime RFC changes: Call the following API with rf_hal_buffer ptr */
        // rfc_common_generate_grfc_script( rfc_prx_grfc_tbl_ptr, grfc_buf_ptr );
      }
        else
      {
            MSG_ERROR("rfc_prx_grfc_tbl_ptr is NULL",0,0,0);
      }

      if( rfc_drx_grfc_tbl_ptr != NULL )
      { 
        /* Dime RFC changes: Call the following API with rf_hal_buffer ptr */
        // rfc_common_generate_grfc_script( rfc_drx_grfc_tbl_ptr, grfc_buf_ptr );
      }
      else
      {
            MSG_ERROR("rfc_drx_grfc_tbl_ptr is NULL",0,0,0);
      }

      grfc_buf_ptr->tech = RFM_LTE_MODE;

}
