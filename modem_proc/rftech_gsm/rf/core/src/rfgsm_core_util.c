/*!
   @file rfgsm_core_util.c

   @brief

   @details

   @todo satheesha 10/30/08: state machine information to let the users know
   about the function call sequence restrictions.
*/

/*! @todo satheesha 10/30/08 do we need to keep the edit history??? */

/*===========================================================================
Copyright (c) 2008 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_util.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/14   ec      Correct EGSM channel normalisation
08/01/14   sg      Add 1800_B split band support in utility functions
02/26/14   adk     Updated function rfgsm_core_get_get_band_num
07/11/14   ec      Utility functions for HL/LL decision table
09/25/13   sb      Added changes to support GSM TX calibration/online mode operation in coexistence
09/17/13   sb      Support for SGLTE/SGTDS coexistence
07/18/13   svi     Adding [QTUNER][RL] GSM NV Support. 
04/05/13   kg      Adding new API to get band num from the rfcom band type enum
02/04/13   kg      Adding new util functions to get the max/min freq in hz and 
                   return Rx/Tx band mask 
01/12/11   lcl     Merged rfgsm_core_power_lvl_to_pcl from QSC6295
10/11/10   lcl     Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
05/25/10   pl      Add API function to convert rfcom_gsm_band_type to rf_card_band
05/21/10   tws     Add API function to translate rf_card_band type to 
                   rfcom_gsm_band type
12/22/09   sr      changed the debug msg macro from legacy to new one.
12/05/09   sr      added new function to return chan index for G900 band.
07/06/09   sr      changes to make the operating band to rfcom_gsm_band type
06/26/09   sr      Added arfcn to frequency conversion function 
03/13/09   sr      code cleanup for SCMM
10/30/08   sr      Initial revision.

============================================================================*/

#include "rfcom.h"
#include "msg.h"
#include "rfgsm_nv_cmn.h"
#include "rfgsm_core_util.h"

typedef struct
{
  rfgsm_band_type rfgsm_band;
  rfcom_gsm_band_type rfcom_band;
  rfgsm_nv_supported_type rfnv_band;
}  rfgsm_band_map_struct;

rfgsm_band_map_struct rfgsm_band_map[RFCOM_NUM_GSM_BANDS] =
{
  {RFGSM_BAND_GSM850,  RFCOM_BAND_GSM850,  RFGSM_NV_850},
  {RFGSM_BAND_GSM900,  RFCOM_BAND_GSM900,  RFGSM_NV_900},
  {RFGSM_BAND_GSM1800, RFCOM_BAND_GSM1800, RFGSM_NV_1800},
  {RFGSM_BAND_GSM1900, RFCOM_BAND_GSM1900, RFGSM_NV_1900},
  {RF_MAX_BAND_TYPES,  RFCOM_BAND_GSM1800_B, RFGSM_NV_1800_B},
};

uint32 rfgsm_hl_table_base_map[RFCOM_NUM_GSM_BANDS] =
{
  RFGSM_CORE_850_HL_TABLE_BASE,  //RFCOM_BAND_GSM850
  RFGSM_CORE_900_HL_TABLE_BASE,  //RFCOM_BAND_GSM900
  RFGSM_CORE_1800_HL_TABLE_BASE, //RFCOM_BAND_GSM1800
  RFGSM_CORE_1900_HL_TABLE_BASE, //RFCOM_BAND_GSM1900
  RFGSM_CORE_1800_HL_TABLE_BASE, //RFCOM_BAND_GSM1800_B
};
/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param  

   @retval 

*/
rfgsm_band_type rfgsm_core_rfcom_to_rfband(rfcom_gsm_band_type rfcom_band) 
{
  uint8 band_index = 0;

  for (band_index = 0; band_index < (uint8)RFCOM_NUM_GSM_BANDS; band_index++)
  {
    if (rfgsm_band_map[band_index].rfcom_band == rfcom_band)
      break;
  }

  if (band_index == (uint8)RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid RFCOM band: %d", rfcom_band);
    band_index = 0;
  }

  return(rfgsm_band_map[band_index].rfgsm_band);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param  

   @retval 

*/
rfgsm_nv_supported_type rfgsm_core_rfcom_band_to_nvband(rfcom_gsm_band_type rf_band) 
{
  uint8 band_index = 0;

  for (band_index = 0; band_index < (uint8)RFCOM_NUM_GSM_BANDS; band_index++)
  {
    if (rfgsm_band_map[band_index].rfcom_band == rf_band)
      break;
  }

  if (band_index >= (uint8)RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid RF band: %d", rf_band);
    return RFGSM_NV_MAX_SUPPORTED;
  }
  else
  {
    return(rfgsm_band_map[band_index].rfnv_band);
  }
  
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param  

   @retval 

*/
rfcom_gsm_band_type rfgsm_core_convert_rfband_to_rfcom(rfgsm_band_type band) 
{
  uint8 band_index = 0;

  for (band_index = 0; band_index < (uint8)RFCOM_NUM_GSM_BANDS; band_index++)
  {
    if (rfgsm_band_map[band_index].rfgsm_band == band)
      break;
  }

  if (band_index == (uint8)RFCOM_NUM_GSM_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid RF band: %d", band);
    band_index = 0;
  }

  return(rfgsm_band_map[band_index].rfcom_band);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param  

   @retval 

*/
rfcom_gsm_band_type rfgsm_core_convert_arfcn_to_band( uint16 chan )
{
  rfcom_gsm_band_type band=RFCOM_NUM_GSM_BANDS;

  if (  (chan >= 128) &&  (chan <= 251)  )
  {
    band = RFCOM_BAND_GSM850;
  } 
  else if (  (chan <= 124) || (chan >= 975 && chan <= 1023)  )
  {
    band = RFCOM_BAND_GSM900;
  } 
  else
  {
    if ( (chan >= 512) && (chan <= 810) )
    {
      band = RFCOM_BAND_GSM1900;
    } 
    else if ( (chan >= 811) && (chan <= 885) )
    {
      band = RFCOM_BAND_GSM1800;
    }
  }
  return(band);
} /* end of rfgsm_convert_arfcn_to_band */

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param  

   @retval 

*/

uint32 rfgsm_core_convert_arfcn_to_tx_freq(rfcom_gsm_band_type band, uint16 arfcn )
{
  uint32 rf_freq=0;

  switch (band) 
  {
  case RFCOM_BAND_GSM850:
    if (arfcn >= 128 && arfcn <= 251)
    {
      rf_freq = 824200000 + 200000*(arfcn - 128);      
    }
    else
    {
      MSG_FATAL("Invalid channel for GSM850(128 - 251) band: %d", arfcn, 0, 0);
    }
    break;

  case RFCOM_BAND_GSM900:
    if (arfcn <= 124)
    {
      rf_freq = 890000000 + 200000*arfcn;
    }
    else if (arfcn >= 975 && arfcn <= 1023)
    {
      rf_freq = 890000000 + 200000*(arfcn - 1024);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
            "Invalid channel for GSM900(0-124 975-1023) band: %d", arfcn); 
    }
    break;

  case RFCOM_BAND_GSM1800:
  case RFCOM_BAND_GSM1800_B:
    if (arfcn >= 512 && arfcn <= 885)
    {
      rf_freq = 1710200000 + 200000*(arfcn - 512);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
            "Invalid channel for GSM1800(512-885) band: %d", arfcn);
    }
    break;

  case RFCOM_BAND_GSM1900:
    if (arfcn >= 512 && arfcn <= 810)
    {
      rf_freq = 1850200000 + 200000*(arfcn - 512);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid channel for GSM1800(512-810) band: %d", arfcn);
    }
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unsupported RF band: %d", band);
  }

  return rf_freq;

} /* end of rfgsm_core_convert_arfcn_to_tx_freq */

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param  

   @retval 

*/
uint32 rfgsm_core_convert_arfcn_to_rx_freq(rfcom_gsm_band_type band, uint16 chan )
{
  uint32 rf_freq=0;

  /* get the rx freq fro the channel */
  rf_freq = rfgsm_core_convert_arfcn_to_tx_freq(band, chan);
  
  /* if the rf_freq is zero then the arfcn is invalid and a error message has
     been already printed so, just return the 0 value. */
  if (rf_freq == 0)
  {
    return rf_freq;
  }

  /* add the Uplink and downlink separation freq offset */
  switch (band)
  {
  case RFCOM_BAND_GSM850:
  case RFCOM_BAND_GSM900:
    rf_freq += 45000000;;
    break;

  case RFCOM_BAND_GSM1800:
  case RFCOM_BAND_GSM1800_B:
    rf_freq += 95000000;
    break;

  case RFCOM_BAND_GSM1900:
    rf_freq += 80000000;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unsupported RF band: %d", band);
  }

  return rf_freq;

} /* end of rfgsm_core_convert_arfcn_to_rx_freq */


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details

   @param  

   @retval 

*/
uint16 rfgsm_core_G900_chan_index_get(uint16 chan )
{
  uint16 chan_index = 0;

  if ( (chan >= 975) && (chan < 1024) )
  {
    chan_index = chan - 975;
  }
  else if ( chan <= 124 )
  {
    chan_index = chan + 49;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid channel for G900 band: %d", chan);;
  }

  return chan_index;
}

/*!
   @brief Converts rf_card_band to rfcom_gsm_band

   @details

   @param  

   @retval 

*/
rfcom_gsm_band_type rfgsm_core_rf_card_band_to_rfcom_gsm_band
(
  rf_card_band_type rf_band
) 
{
  rfcom_gsm_band_type gsm_band = RFCOM_NUM_GSM_BANDS;

  switch( rf_band )
  {
  case RF_EGSM_BAND:
    gsm_band = RFCOM_BAND_GSM900;
    break;
  case RF_DCS_BAND:
    gsm_band = RFCOM_BAND_GSM1800;
    break;
  case RF_GSM_850_BAND:
    gsm_band = RFCOM_BAND_GSM850;
    break;
  case RF_GSM_1900_BAND:
    gsm_band = RFCOM_BAND_GSM1900;
    break;

  default:
    break;
  }

  return( gsm_band );

}

/*----------------------------------------------------------------------------*/
/*!
   @brief Converts rfcom_gsm_band to rf_card_band

   @details

   @param  

   @retval 

*/
rf_card_band_type rfgsm_core_rfcom_gsm_band_to_rf_card_band
(
  rfcom_gsm_band_type gsm_band
) 
{
  rf_card_band_type rfc_band = RF_MAX_BAND;

  switch( gsm_band )
  {
  case RFCOM_BAND_GSM900:
    rfc_band = RF_EGSM_BAND;
    break;
  case RFCOM_BAND_GSM1800:
  case RFCOM_BAND_GSM1800_B:
    rfc_band = RF_DCS_BAND;
    break;
  case RFCOM_BAND_GSM850:
    rfc_band = RF_GSM_850_BAND;
    break;
  case RFCOM_BAND_GSM1900:
    rfc_band = RF_GSM_1900_BAND;
    break;
  default:
    break;
  }
  return( rfc_band );
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   API to convert TX power level(from GL1) to PCL


*/
uint16 rfgsm_core_power_lvl_to_pcl(rfcom_gsm_band_type band, uint16 pwr_lvl)
{
  /* Set default PCL as min PCL for LB*/
  uint16 pcl = 15;

  switch(band)
  {
  case RFCOM_BAND_GSM850: 
  case RFCOM_BAND_GSM900:
    /* For LB 5<PCL<19 */
    pcl = 14 - pwr_lvl + 5;
    break;

  case RFCOM_BAND_GSM1800:
  case RFCOM_BAND_GSM1900:
    /* For HB 0<PCL<15 */
    pcl = 15-pwr_lvl;
    break;

  default:
    /* Return min pcl */
    pcl=15;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Incorrect band type: %d",band);
    break;
  }

  return pcl;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the min and max Rx freq (in Hz) of a GSM band

  @details

  @return
  boolean TRUE if band is supported, FALSE otherwise
*/
boolean rfgsm_core_get_min_max_rx_freqs(rfcom_gsm_band_type band, uint32* min, uint32* max)
{
  boolean ret_val = TRUE;

    switch(band)
    {
    case RFCOM_BAND_GSM850:
      *min = RFGSM_CORE_850_RX_MIN_FREQ;
      *max = RFGSM_CORE_850_RX_MAX_FREQ;
      break;

    case RFCOM_BAND_GSM900:
      *min = RFGSM_CORE_900_RX_MIN_FREQ;
      *max = RFGSM_CORE_900_RX_MAX_FREQ;
      break;

    case RFCOM_BAND_GSM1800:
    case RFCOM_BAND_GSM1800_B:
      *min = RFGSM_CORE_1800_RX_MIN_FREQ;
      *max = RFGSM_CORE_1800_RX_MAX_FREQ;
      break;

    case RFCOM_BAND_GSM1900:
      *min = RFGSM_CORE_1900_RX_MIN_FREQ;
      *max = RFGSM_CORE_1900_RX_MAX_FREQ;
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_get_min_max_rx_freqs: GSM band %d NOT SUPPORTED!",band);
      ret_val = FALSE;
    }

   return ret_val;
} //rfgsm_core_get_min_max_rx_freqs

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the min and max Tx freq (in Hz) of a GSM band

  @details

  @return
  boolean TRUE if band is supported, FALSE otherwise
*/
boolean rfgsm_core_get_min_max_tx_freqs(rfcom_gsm_band_type band, uint32* min, uint32* max)
{
  boolean ret_val = TRUE;

   switch(band)
   {
    case RFCOM_BAND_GSM850:
      *min = RFGSM_CORE_850_TX_MIN_FREQ;
      *max = RFGSM_CORE_850_TX_MAX_FREQ;
      break;

    case RFCOM_BAND_GSM900:
      *min = RFGSM_CORE_900_TX_MIN_FREQ;
      *max = RFGSM_CORE_900_TX_MAX_FREQ;
      break;

   case RFCOM_BAND_GSM1800:
   case RFCOM_BAND_GSM1800_B:
      *min = RFGSM_CORE_1800_TX_MIN_FREQ;
      *max = RFGSM_CORE_1800_TX_MAX_FREQ;
      break;

    case RFCOM_BAND_GSM1900:
      *min = RFGSM_CORE_1900_TX_MIN_FREQ;
      *max = RFGSM_CORE_1900_TX_MAX_FREQ;
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_core_get_min_max_tx_freqs: GSM band %d NOT SUPPORTED!",band);
      ret_val = FALSE;
   }

   return ret_val;
} // rfgsm_core_get_min_max_tx_freqs

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the normalised channel number (starting at 0) from
  and ARFCN

  @details

  @return
  boolean TRUE if successful, FALSE otherwise
*/
boolean rfgsm_core_get_norm_chan(rfcom_gsm_band_type band, uint16* arfcn, uint32* norm_chan)
{
  boolean ret_val = TRUE;

   switch(band)
   {
    case RFCOM_BAND_GSM850:
      *norm_chan = *arfcn - RFGSM_CORE_850_MIN_ARFCN;
      break;

    case RFCOM_BAND_GSM900:
      *norm_chan = (RFGSM_CORE_900_MAX_ARFCN >= *arfcn) ? (*arfcn + (RFGSM_CORE_900_INT_ARFCN - RFGSM_CORE_900_MIN_ARFCN)) : (*arfcn - RFGSM_CORE_900_MIN_ARFCN);
      break;

   case RFCOM_BAND_GSM1800:
   case RFCOM_BAND_GSM1800_B:
      *norm_chan = *arfcn - RFGSM_CORE_1800_MIN_ARFCN;
      break;

    case RFCOM_BAND_GSM1900:
      *norm_chan = *arfcn - RFGSM_CORE_1900_MIN_ARFCN;
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_core_get_norm_chan: GSM band %d NOT SUPPORTED!",band);
      ret_val = FALSE;
   }

   return ret_val;
} // rfgsm_core_get_norm_chan
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function offset in the HL flag table for the specified band

  @details

  @return
  boolean TRUE if successful, FALSE otherwise
*/
boolean rfgsm_core_get_hl_table_base(rfcom_gsm_band_type band, uint32* base)
{
  if ( band > RFCOM_NUM_GSM_BANDS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_core_get_hl_table_base: GSM band %d NOT SUPPORTED!", band);
    return FALSE;
  }

  *base = rfgsm_hl_table_base_map[band];

  return TRUE; 
} // rfgsm_core_get_hl_table_base
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets GSM Rx band mask

  @return
  uint8
*/
uint8 rfgsm_core_get_rx_band_mask(rfcom_gsm_band_type band)
{
  uint8 ret_val = TRUE;

  switch(band)
    {
    case RFCOM_BAND_GSM850:
      ret_val = RFGSM_CORE_GSM850_RX_MASK;
      break;
    case RFCOM_BAND_GSM900:
      ret_val = RFGSM_CORE_GSM900_RX_MASK;
      break;
    case RFCOM_BAND_GSM1800:
      ret_val = RFGSM_CORE_GSM1800_RX_MASK;
      break;
    case RFCOM_BAND_GSM1800_B:
      ret_val = RFGSM_CORE_GSM1800_RX_MASK;
      break;
    case RFCOM_BAND_GSM1900:
      ret_val = RFGSM_CORE_GSM1900_RX_MASK;
      break;
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"get_gsm_rx_tx_mask: Invalid GSM Rx band %d!", band);
      ret_val = FALSE;
    }

  return ret_val;
} // rfgsm_core_get_rx_band_mask


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets GSM Tx band mask

  @return
  uint8
*/
uint8 rfgsm_core_get_tx_band_mask(rfcom_gsm_band_type band)
{
  uint8 ret_val = TRUE;

  switch(band)
    {
    case RFCOM_BAND_GSM850:
      ret_val = RFGSM_CORE_GSM850_TX_MASK;
      break;
    case RFCOM_BAND_GSM900:
      ret_val = RFGSM_CORE_GSM900_TX_MASK;
      break;
    case RFCOM_BAND_GSM1800:
      ret_val = RFGSM_CORE_GSM1800_TX_MASK;
      break;
    case RFCOM_BAND_GSM1800_B:
      ret_val = RFGSM_CORE_GSM1800_TX_MASK;
      break;
    case RFCOM_BAND_GSM1900:
      ret_val = RFGSM_CORE_GSM1900_TX_MASK;
      break;
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"get_gsm_rx_tx_mask: Invalid GSM Rx band %d!", band);
      ret_val = FALSE;
    }

  return ret_val;
} // rfgsm_core_get_tx_band_mask


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets GSM band no. from the rfcom band type

  @return
  boolean
*/
boolean rfgsm_core_get_get_band_num(rfcom_gsm_band_type band , uint16* num)
{
  uint8 ret_val = TRUE;

  switch(band)
    {
    case RFCOM_BAND_GSM850:
      *num = 850;
      break;
    case RFCOM_BAND_GSM900:
      *num = 900;
      break;
    case RFCOM_BAND_GSM1800:
      *num = 1800;
      break;
    case RFCOM_BAND_GSM1800_B:
      *num = 1800;
      break;
    case RFCOM_BAND_GSM1900:
      *num = 1900;
      break;
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_get_get_band_num: Invalid GSM Rx band %d!", band);
      ret_val = FALSE;
    }

  return ret_val;
} // rfgsm_core_get_get_band_num

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts power unit from dBm10 to dBm100.

  @return
  int16 power
*/
/*power conversion*/
int16 rfgsm_core_dbm10_to_dbm100(int16 power)
{
 return power*10;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts power unit from dBm100 to dBm10.

  @return
  int16 power
*/
int16 rfgsm_core_dbm100_to_dbm10(int16 power)
{
 return power/10;
}

