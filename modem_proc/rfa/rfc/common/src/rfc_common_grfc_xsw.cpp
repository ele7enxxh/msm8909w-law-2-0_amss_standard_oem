/*!
   @file
   rfc_common_grfc_xsw.cpp

   @brief
   GRFC XSW device driver

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/src/rfc_common_grfc_xsw.cpp#1 $

when       who    what, where, why
--------   ---   ---------------------------------------------------------------
05/14/14   dbc   Initial version

============================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfcommon_msg.h"
#include "rfc_common_grfc_xsw.h"


extern int32 rfc_common_get_signal_num(int32 rf_sig_name, rfc_signal_type sig_type);

#define RFC_COMMON_GRFC_XSW_MAX_SCRIPT_SIZE 3

/*----------------------------------------------------------------------------*/
/*!
  @brief
  constructor

*/
grfc_xsw::grfc_xsw
(
    void
)
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  destructor

*/
grfc_xsw::~grfc_xsw
(
)
{
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function returning the state of the cross switch

  @param xsconfig
  Struct containg the current switch state
    
  @return
  boolean indicating pass or fail  
*/	
boolean grfc_xsw::get_cross_switch_config
(
   rfdevice_xsw_config_type* xsconfig
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function configures the XSW according to the parameters in xsconfig

  @param xsconfig
  struct indicating the configuration to set

  @param script_timing
  int16 indicating timing to use

  @param buff_obj_ptr
  void pointer used to pass address of CCS buffer

  @param dev_action
  indicates if it is immediate write to hw or build scripts
*/
boolean grfc_xsw::set_cross_switch_config
(
  rfdevice_xsw_config_type* xsconfig,
  rf_buffer_intf*           script_buffer, 
  rf_device_execution_type  dev_action, 
  int16                     script_timing
)
{
  boolean status = TRUE;
  rfc_sig_cfg_type* rf_signals = NULL;

  /* Check if the provided buffer is valid */
  if ( ( dev_action == RFDEVICE_CREATE_SCRIPT && NULL == script_buffer ) || 
          ( NULL == xsconfig ) )
  {
    RF_MSG( RF_ERROR, "grfc_xsw set_cross_switch_config failed: Script not allocated" );
    return FALSE; 
  }

  switch( xsconfig->mode )
  {
  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    if (  RFDEVICE_XSW_PRX_DRX_PATH_DEFAULT == xsconfig->xsmode ) 
    {
      rf_signals = this->cdma_band_mapping_default[xsconfig->band.cdma_band];
    }
    else if ( RFDEVICE_XSW_PRX_DRX_PATH_SWAP == xsconfig->xsmode )
    {
      rf_signals = this->cdma_band_mapping_swap[xsconfig->band.cdma_band];
    }
    else
    {
      status = FALSE;
    }
    break;
  case RFCOM_WCDMA_MODE:
    if ( RFDEVICE_XSW_PRX_DRX_PATH_DEFAULT == xsconfig->xsmode ) 
    {
      rf_signals = this->wcdma_band_mapping_default[xsconfig->band.wcdma_band];
    }
    else if ( RFDEVICE_XSW_PRX_DRX_PATH_SWAP == xsconfig->xsmode )
    {
      rf_signals = this->wcdma_band_mapping_swap[xsconfig->band.wcdma_band];
    }
    else
    {
      status = FALSE;
    }
    break;
  case RFCOM_GSM_MODE:
    if ( RFDEVICE_XSW_PRX_DRX_PATH_DEFAULT == xsconfig->xsmode ) 
    {
      rf_signals = this->gsm_band_mapping_default[xsconfig->band.gsm_band];
    }
    else if ( RFDEVICE_XSW_PRX_DRX_PATH_SWAP == xsconfig->xsmode )
    {
      rf_signals = this->gsm_band_mapping_swap[xsconfig->band.gsm_band];
    }
    else
    {
      status = FALSE;
    }
    break;
  case RFCOM_LTE_MODE:
    if ( RFDEVICE_XSW_PRX_DRX_PATH_DEFAULT == xsconfig->xsmode ) 
    {
      rf_signals = this->lte_band_mapping_default[xsconfig->band.lte_band];
    }
    else if ( RFDEVICE_XSW_PRX_DRX_PATH_SWAP == xsconfig->xsmode )
    {
      rf_signals = this->lte_band_mapping_swap[xsconfig->band.lte_band];
    }
    else
    {
      status = FALSE;
    }
    break;
  case RFCOM_TDSCDMA_MODE:
    if ( RFDEVICE_XSW_PRX_DRX_PATH_DEFAULT == xsconfig->xsmode ) 
    {
      rf_signals = this->tdscdma_band_mapping_default[xsconfig->band.tdscdma_band];
    }
    else if ( RFDEVICE_XSW_PRX_DRX_PATH_SWAP == xsconfig->xsmode )
    {
      rf_signals = this->tdscdma_band_mapping_swap[xsconfig->band.tdscdma_band];
    }
    else
    {
      status = FALSE;
    }
    break;
  default:
      status = FALSE;
  }

  if ( FALSE == status ) 
  {
    RF_MSG_2( RF_ERROR, "grfc_xsw::set_cross_switch_config: mode/tech %d/%d not supported", 
              xsconfig->xsmode, xsconfig->mode );
  }

  /* Tech common */
  if( status == TRUE )
  {
    if ( dev_action == RFDEVICE_EXECUTE_IMMEDIATE )
    {
      /* loop through the signal list and configure them based on their type to specified logic  */
      int32 rf_sig_name = RFC_SIG_LIST_END;
      uint16 i = 0; 
      while (rf_signals[i].sig_name != RFC_SIG_LIST_END)
      {
        int32 signal_num = -1;
        /* get the RF-CARD Signal name */
        rf_sig_name = rf_signals[i].sig_name;

        /* get corresponding GRFC engine num*/
        signal_num = rfc_common_get_signal_num(rf_sig_name, RFC_ANT_SEL );

        if (signal_num >= 0)
        {
          status &= rfc_common_set_grfc( signal_num, rf_signals[i].start.logic );
        }
        else 
        {
          status = FALSE;
        }
        
        if ( !status ) break;
        i++;
      }
    }
    else
    {
      /* return script */
#ifdef TEST_FRAMEWORK
      #error code not present
#else
      status = rfc_common_generate_grfc_script( rf_signals, 
                                                script_buffer, 
                                                script_timing );
#endif
    }

  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  this function sets the band map for the default XSW position

  @param mode
  enum indicating the Radio Access Technology

  @param band
  enum indicating band number

  @param data_ptr
  int32 pointer containg the GRFC info 

*/
boolean grfc_xsw::set_band_map_default
(
 rfcom_mode_enum_type mode, 
 rfcom_band_type_u band, 
 int32 *data_ptr, 
 uint8 size 
)
{
  switch( mode )
  {
  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
      this->cdma_band_mapping_default[band.cdma_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_WCDMA_MODE:
      this->wcdma_band_mapping_default[band.wcdma_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_GSM_MODE:
      this->gsm_band_mapping_default[band.gsm_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_LTE_MODE:
      this->lte_band_mapping_default[band.lte_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_TDSCDMA_MODE:
      this->tdscdma_band_mapping_default[band.tdscdma_band] =
          (rfc_sig_cfg_type*)data_ptr;
      break;
  default:
      RF_MSG( RF_ERROR, "grfc_xsw::set_band_map_default: mode or band not supported. GRFC"
              "band mapping not set");
      return FALSE;
  }

  return TRUE;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  this function sets the band map for the swap XSW position

  @param mode
  enum indicating the Radio Access Technology

  @param band
  enum indicating band number

  @param data_ptr
  int32 pointer containg the GRFC info 

*/
boolean grfc_xsw::set_band_map_swap
(
 rfcom_mode_enum_type mode, 
 rfcom_band_type_u band, 
 int32 *data_ptr, 
 uint8 size 
)
{
  switch( mode )
  {
  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
      this->cdma_band_mapping_swap[band.cdma_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_WCDMA_MODE:
      this->wcdma_band_mapping_swap[band.wcdma_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_GSM_MODE:
      this->gsm_band_mapping_swap[band.gsm_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_LTE_MODE:
      this->lte_band_mapping_swap[band.lte_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_TDSCDMA_MODE:
      this->tdscdma_band_mapping_swap[band.tdscdma_band] =
          (rfc_sig_cfg_type*)data_ptr;
      break;
  default:
      RF_MSG( RF_ERROR, "grfc_xsw::set_band_map_swap: mode or band not supported. GRFC"
              "band mapping not set");
      return FALSE;
  }

  return TRUE;

}


boolean grfc_xsw::set_srs_band_map_default
(
 rfcom_mode_enum_type mode, 
 rfcom_band_type_u band, 
 int32 *data_ptr, 
 uint8 size 
)
{
  switch( mode )
  {
  case RFCOM_LTE_MODE:
      this->lte_srs_band_mapping_default[band.lte_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
  case RFCOM_TDSCDMA_MODE:
  case RFCOM_GSM_MODE:
  case RFCOM_WCDMA_MODE:
  default:
      RF_MSG( RF_ERROR, "grfc_xsw::set_srs_band_map_default: mode or band not supported. GRFC"
              "band mapping not set");
      return FALSE;
  }

  return TRUE;

}

boolean grfc_xsw::set_srs_band_map_swap
(
 rfcom_mode_enum_type mode, 
 rfcom_band_type_u band, 
 int32 *data_ptr, 
 uint8 size 
)
{
  switch( mode )
  {
  case RFCOM_LTE_MODE:
      this->lte_srs_band_mapping_swap[band.lte_band] = (rfc_sig_cfg_type*)data_ptr;
      break;
  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
  case RFCOM_TDSCDMA_MODE:
  case RFCOM_GSM_MODE:
  case RFCOM_WCDMA_MODE:
  default:
      RF_MSG( RF_ERROR, "grfc_xsw::set_srs_band_map_swap: mode or band not supported. GRFC"
              "band mapping not set");
      return FALSE;
  }

  return TRUE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  placeholder for compilation

  @details
  placeholder for compilation
*/
void grfc_xsw::init
(
)
{
    return;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  placeholder for compilation

  @details
  placeholder for compilation
*/
boolean grfc_xsw::init
(
  int32* rfc_data
)
{
  /* Not used for now */
  return TRUE;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  returns max script size needed by this device

  @details
  returns max script size needed by this device

  @return
  max script size of type uint16
*/
uint16 grfc_xsw::get_script_size
(
)
{
  return RFC_COMMON_GRFC_XSW_MAX_SCRIPT_SIZE;
}
