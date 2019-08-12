/*!
   @file
   rfc_common_grfc_pa.cpp

   @brief
   GRFC PA device driver

*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/src/rfc_common_grfc_pa.cpp#1 $

when       who    what, where, why
--------   ---   ---------------------------------------------------------------
07/21/14   zhw   Set low gain range during PA config
07/07/13   sbo   Added BW support for PA device
03/11/13   sar     Updated file for APQ, GNSS only target.
01/10/12   zhw   Add support for possible R0/R1 1-line configuration
12/11/12   zhw   void init() API for interface compatibility
12/11/12   zhw   Initial script timing support
12/05/12   zhw   Initial version

============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfcommon_msg.h"
#include "rfc_common_grfc_pa.h"


#define RFC_COMMON_GRFC_PA_MAX_SCRIPT_SIZE 3

/*----------------------------------------------------------------------------*/
/*!
  @brief
  constructor

*/
grfc_pa::grfc_pa
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
grfc_pa::~grfc_pa
(
)
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Place holder for GRFC PA

  @param mode
  enum indicating the Radio Access Technology

  @param band
  enum indicating band number

  @param buff_obj_ptr
  void pointer used to pass address of CCS buffer

  @param dev_action
  indicates if it is immediate write to hw or build scripts
*/
boolean grfc_pa::config
(
    rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type dev_action,
    int16 script_timing
)
{
    boolean ret_val;

    if( pa_params_p != NULL )
    {
        pa_params_p->pa_gain_range = 3;

        ret_val = set_pa_txagc(pa_params_p, script_buffer, dev_action, script_timing); 
    }
    else
    {
        ret_val = FALSE;
    }

    return ret_val;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  this function turns the PA on or off

  @details
  this function turns the PA on or off

  @param mode
  enum indicating the Radio Access Technology

  @param band
  enum indicating band number

  @param on_off
  indicates to turn the pa on or off

  @param buff_obj_ptr
  void pointer used to pass address of CCS buffer

  @param dev_action
  indicates if it is immediate write to hw or build scripts
*/
boolean grfc_pa::set_pa_on_off
(
    rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type dev_action,
    int16 script_timing
)
{
    boolean status = TRUE;
    rfc_sig_cfg_type* rf_signals = NULL;

    switch( pa_params_p->mode )
    {
    case RFM_1X_MODE:
    case RFM_1XEVDO_MODE:
        rf_signals = this->cdma_tx_band_mapping[pa_params_p->band.cdma_band];
        break;
    case RFM_IMT_MODE:
        rf_signals = this->wcdma_tx_band_mapping[pa_params_p->band.wcdma_band];
        break;
    case RFM_EGSM_MODE:
        rf_signals = this->gsm_tx_band_mapping[pa_params_p->band.gsm_band];
        break;
    case RFM_LTE_MODE:
        rf_signals = this->lte_tx_band_mapping[pa_params_p->band.lte_band];
        break;
    case RFM_TDSCDMA_MODE:
        rf_signals = this->tdscdma_tx_band_mapping[pa_params_p->band.tdscdma_band];
        break;
    default:
        RF_MSG_1( RF_ERROR, "grfc_pa::on_off: mode %d not supported", pa_params_p->mode );
        status = FALSE;
    }

    /* Tech common */
    if( status == TRUE )
    {
        if( dev_action == RFDEVICE_CREATE_SCRIPT )
        {
            if( pa_params_p->on_off == TRUE )
            {
                rfc_common_get_grfc_overwritten_buffer( rf_signals,
                                                        RFC_PA_ON, RFC_HIGH,
                                                        script_buffer, script_timing );
            }
            else
            {
                rfc_common_get_grfc_overwritten_buffer( rf_signals,
                                                        RFC_PA_ON, RFC_LOW,
                                                        script_buffer, script_timing );
            }
        }
        else if ( dev_action == RFDEVICE_EXECUTE_IMMEDIATE )
        {
            int32 grfc_engine_num;

            grfc_engine_num = rfc_common_get_grfc_engine_num( rf_signals,
                              RFC_PA_ON );
            if( pa_params_p->on_off == TRUE )
            {
                status &= rfc_common_set_grfc( grfc_engine_num, RFC_HIGH );
            }
            else
            {
                status &= rfc_common_set_grfc( grfc_engine_num, RFC_LOW );
            }
        }
    }

    return status ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  this function sets the txagc registers based on gain range

  @details
  this function sets the txagc registers based on gain range

  @param mode
  enum indicating the Radio Access Technology

  @param band
  enum indicating band number

  @param pa_gain_range
  gain range

  @param mod_type
  indicates if modulation is GMSK or 8psk, for GSM only

  @param buff_obj_ptr
  void pointer used to pass address of CCS buffer

  @param dev_action
  indicates if it is immediate write to hw or build scripts
*/
boolean grfc_pa::set_pa_txagc
(
    rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type dev_action,
    int16 script_timing
)
{
    boolean status = TRUE;

    rfdevice_pa_state_type pa_gain_state = RFDEVICE_PA_STATE_INVALID;

    switch( pa_params_p->pa_gain_range )
    {
    case 0:
        pa_gain_state = RFDEVICE_PA_STATE_ET;
        break;
    case 1:
        pa_gain_state = RFDEVICE_PA_STATE_EPT;
        break;
    case 2:
        pa_gain_state = RFDEVICE_PA_STATE_APT;
        break;
    case 3:
        pa_gain_state = RFDEVICE_PA_STATE_LPM;
        break;
    default:
        RF_MSG( RF_ERROR, "grfc_pa::set_gain_range: Invalid PA gain range");
        status = FALSE;
    }

    if( status == TRUE )
    {
        status = this->set_gain_state( pa_params_p->mode,
                                       pa_params_p->band,
                                       pa_gain_state,
                                       pa_params_p->is_mod_type_gmsk,
                                       script_buffer,
                                       dev_action,
                                       script_timing );
    }

    return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup PA for a specific gain state

  @details
  This function can be used to configure the Tx AGC registers based on
  the desired gain state

  @param mode
  enum indicating the radio access technology

  @param band
  enum indicating band number

  @param pa_gain_state
  enum that indicates the pa gain state

  @param mod_type
  indicates the modulation type

  @param script_buffer
  void pointer to pass address of buffer script

  @param write_imm
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
*/
boolean grfc_pa::set_gain_state
(
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_pa_state_type pa_gain_state,
    boolean mod_type,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type dev_action,
    int16 script_timing
)
{
    boolean status = TRUE;

    rfc_sig_cfg_type* rf_signals = NULL;

    rfc_logic_type pa_range_0_logic = RFC_LOGIC_INVALID;
    rfc_logic_type pa_range_1_logic = RFC_LOGIC_INVALID;

    switch( mode )
    {
    case RFM_1X_MODE:
    case RFM_1XEVDO_MODE:
        rf_signals = this->cdma_tx_band_mapping[band.cdma_band];
        break;
    case RFM_IMT_MODE:
        rf_signals = this->wcdma_tx_band_mapping[band.wcdma_band];
        break;
    case RFM_EGSM_MODE:
        rf_signals = this->gsm_tx_band_mapping[band.gsm_band];
        break;
    case RFM_LTE_MODE:
        rf_signals = this->lte_tx_band_mapping[band.lte_band];
        break;
    case RFM_TDSCDMA_MODE:
        rf_signals = this->tdscdma_tx_band_mapping[band.tdscdma_band];
        break;
    default:
        RF_MSG_1( RF_ERROR, "grfc_pa::set_gain_state mode %d not supported", mode );
        status = FALSE;
    }

    /* Tech common */
    switch( pa_gain_state )
    {
    case RFDEVICE_PA_STATE_ET:
        pa_range_0_logic = RFC_LOW;
        pa_range_1_logic = RFC_LOW;
        break;
    case RFDEVICE_PA_STATE_EPT:
        pa_range_0_logic = RFC_HIGH;
        pa_range_1_logic = RFC_LOW;
        break;
    case RFDEVICE_PA_STATE_APT:
        pa_range_0_logic = RFC_LOW;
        pa_range_1_logic = RFC_HIGH;
        break;
    case RFDEVICE_PA_STATE_LPM:
        pa_range_0_logic = RFC_HIGH;
        pa_range_1_logic = RFC_HIGH;
        break;
    default:
        RF_MSG( RF_ERROR, "grfc_pa::set_gain_range: Invalid PA state");
        status = FALSE;
    }

    if( status == TRUE )
    {
        if( dev_action == RFDEVICE_CREATE_SCRIPT )
        {
            rfc_common_get_grfc_overwritten_buffer( rf_signals,
                                                    RFC_PA_RANGE0,
                                                    pa_range_0_logic,
                                                    script_buffer, script_timing );

            rfc_common_get_grfc_overwritten_buffer( rf_signals,
                                                    RFC_PA_RANGE1,
                                                    pa_range_1_logic,
                                                    script_buffer, script_timing );

        }
        else if ( dev_action == RFDEVICE_EXECUTE_IMMEDIATE )
        {
            int32 grfc_engine_num;

            grfc_engine_num = rfc_common_get_grfc_engine_num( rf_signals,
                              RFC_PA_RANGE0 );
            if( grfc_engine_num != -1 )
            {
                status &= rfc_common_set_grfc( grfc_engine_num, pa_range_0_logic );
            }

            grfc_engine_num = rfc_common_get_grfc_engine_num( rf_signals,
                              RFC_PA_RANGE1 );
            if( grfc_engine_num != -1 )
            {
                status &= rfc_common_set_grfc( grfc_engine_num, pa_range_1_logic );
            }
        }
    }

    return status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  placeholder for compilation

  @details
  placeholder for compilation
*/
void grfc_pa::init
(
)
{
    return;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  placeholder

  @details
  placeholder

  @param buff_obj_ptr
  void pointer used to pass address of CCS buffer

  @param dev_action
  indicates if it is immediate write to hw or build scripts
*/
boolean grfc_pa::sleep
(
    rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type dev_action,
    int16 script_timing
)
{
    return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  placeholder

  @details
  placeholder

  @param buff_obj_ptr
  void pointer used to pass address of CCS buffer

  @param dev_action
  indicates if it is immediate write to hw or build scripts
*/
boolean grfc_pa::wakeup
(
    rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *script_buffer,
    rf_device_execution_type dev_action,
    int16 script_timing
)
{
    return TRUE ;
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
uint16 grfc_pa::get_script_size
(
)
{
    return RFC_COMMON_GRFC_PA_MAX_SCRIPT_SIZE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Place holder for GRFC PA

  @details
  Place holder for GRFC PA

  @param mode
  enum indicating the Radio Access Technology

  @param band
  enum indicating band number

  @param data
  band port mapping enum passed as uint16
*/
boolean grfc_pa::set_band_port_mapping
(
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    uint16 data
)
{
    return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Place holder for GRFC PA

  @details
  Place holder for GRFC PA

  @param mode
  enum indicating the radio access technology

  @param band
  enum indicating the band number

  @param data_ptr
  Pointer to signal table

  @param size
  Don't care
*/

boolean grfc_pa::set_band_map
(
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    int32 *data_ptr,
    uint8 size
)
{
    switch( mode )
    {
    case RFM_1X_MODE:
    case RFM_1XEVDO_MODE:
        this->cdma_tx_band_mapping[band.cdma_band] = (rfc_sig_cfg_type*)data_ptr;
        break;
    case RFM_IMT_MODE:
        this->wcdma_tx_band_mapping[band.wcdma_band] = (rfc_sig_cfg_type*)data_ptr;
        break;
    case RFM_EGSM_MODE:
        this->gsm_tx_band_mapping[band.gsm_band] = (rfc_sig_cfg_type*)data_ptr;
        break;
    case RFM_LTE_MODE:
        this->lte_tx_band_mapping[band.lte_band] = (rfc_sig_cfg_type*)data_ptr;
        break;
    case RFM_TDSCDMA_MODE:
        this->tdscdma_tx_band_mapping[band.tdscdma_band] =
            (rfc_sig_cfg_type*)data_ptr;
        break;
    default:
        RF_MSG( RF_ERROR, "grfc_pa::set_band_map: mode or band not supported. GRFC"
                "band mapping not set");
        return FALSE;
    }
    return TRUE;
}
#endif
