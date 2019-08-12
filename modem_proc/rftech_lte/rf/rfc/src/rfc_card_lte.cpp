/*!
   @file
   rfc_card_lte.cpp

   @brief
   This file contains the implementation of RFC LTE card interfaces

*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/rf/rfc/src/rfc_card_lte.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
11/26/15   lhb     FR 31436: API to provide the HW band limitation to LTE RRC
01/14/15   dw      Ported : GRFC Alt Path Support
12/15/14   ars     Added fix to handle intra CA with split bands where PRx and DRx are on different splits.
12/05/14   dw      Changes to support split bands based on split specified in RFC.
10/20/14   ndb     Remove the Deprecated  API's 
10/09/14   rsr     Fixing compiler warning.
09/18/14   rsr     Changes to return TX LUT index when querying TX LUT info.
08/15/14   st      NS07 Port Update Support
07/31/14   ndb     Added support for extended EARFCN for LTE
07/10/14   jf      TM8 + SRS switching for TDD
07/08/14   gvn     Modify algorithm for split-intraband support
07/02/14   jf      AsDiv support
06/11/14   bar     Added rfc_lte_disable_fb_device() API to support IQ capture
05/15/14   gvn     Add Api for getting overlapping split bands
04/11/14   bsh     Split Band Support
01/09/14   svi     Split Band Support
10/21/13   nrk     Fix compiler warnings
05/21/13   pv      Removed the Temporary work around of disabling B28B split path to avoid crash
05/02/13   pv      Temporarily disabling B28B split path to avoid crash
04/25/13   gvn     Support for Split bands for LTE
04/10/13   kab     Added support for Tx Antenna Swap.
03/12/13   sn      XPT dual WTR fix for feedback path
10/31/12   vss     Change to get device API for returning a list of objects 
08/16/12   pl      Update RFC configuration strucutre to reflect new changes in RFC 
07/30/12   pl      Fix warning
07/27/12   sr      renamed rfdevice related enum types.
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type.
06/06/12   pl      Update RFC interface to take DEVICE instead of PATH
06/01/12   aca     Fix to clean up Tx signals when disable tx is called
04/24/12   cd      Fixed LTE exit mode to restore init settings for both Tx and Rx band GRFC/ANT_SELs
02/28/12   sr      made changes to put all the GPIOs into low power mode during sleep.
12/05/11   tnt     Add rfc_lte_mode_sleep() api
11/05/11   aak     Enable NPA calls from RFSW for power management on 9x15  
08/25/11   aak     Enable RF NPA voting only for MSM8960 
06/29/11   pl      Added path and band information in mode_exit()
06/23/11   pl      added mode_exit() interface
06/23/11   pl      Rename config_mode and get_config_mode_data
04/26/11   tnt     Porting to new vreg management interface
04/13/11   aak     Revert name to rfc_lte_manage_vregs()
04/13/11   aak     Change rfc_lte_manage_vregs() to rfc_lte_manage_power()
01/13/11   pl      Use rfcom_lte_band_type instead of rf_card_band_type
01/05/11   pl      Added band parameter to rfc_lte_get_rx/tx_band_config()
12/15/10   pl      Renamed to a c++ file
12/15/10   pl      Moves to call C++ rfc_lte class
07/08/09   av      Cleanup and added port configuration (RTR) data  in RFC
11/21/08   dyc     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfc_card.h"
#include "rfc_card_lte.h"
#include "rfcommon_core_error.h"
#include "rfcommon_core.h"
#include "mdsp_intf.h"
}
#include "rfc_lte.h"
#include "rflte_mc.h"
#include "rfc_lte_data.h"

/*structure for rflte to contain pm configuration info*/
/*This global should only be refered in this file*/
rfc_power_management_info rflte_pm_info;
/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function implements the card specific intializations for LTE mode

  @details


*/
void rfc_lte_config_mode_data( rfm_device_enum_type device )
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_config_mode_data:RFC LTE object not initialized");
    return;
  }

  rfc_lte_p->mode_config(device);
} /* rfc_lte_config_mode_data() */

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function implements the card specific intializations for LTE mode

  @details


*/
void* rfc_lte_get_mode_config(rfm_device_enum_type device, 
                              rfc_mode_config_data_enum_type config_data)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_mode_config:RFC LTE object not initialized");
    return NULL;
  }

  return(rfc_lte_p->mode_config_get(device, config_data));
}
/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function implements the card specific intializations for rx band
  changes.

  @details
  The band table which this RF card supports is not indexed in any way.
  Explicit logic will need to be provided to extract the band specific data
  structures.

*/
void rfc_lte_reset_grfc_band_config(rfm_device_enum_type device, rfcom_lte_band_type band, uint8 alt_path)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_reset_grfc_band_config:RFC LTE object not initialized");
    return;
  }

  rfc_lte_p->reset_grfc_band_config(device, band, alt_path);
}
/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function implements the card specific intializations for rx band
  changes.

  @details
  The band table which this RF card supports is not indexed in any way.
  Explicit logic will need to be provided to extract the band specific data
  structures.

*/
void rfc_lte_config_rx_band_data(rfm_device_enum_type device, rfcom_lte_band_type band, uint8 alt_path)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_config_rx_band_data:RFC LTE object not initialized");
    return;
  }

  rfc_lte_p->rx_band_config(device, band, alt_path);
} /* rfc_lte_config_rx_band_data() */

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function returns top level ptr to the current Rx band configuration.

  @details
  The function will return Rx configuration such as GPIO/GRFC settings along
  with any other parameter data structures for a particular band.  The void*
  allows for casting for usage under various band types.

  Dependency that rfc_set_band() be called before using this function.

  @parameter
  path request Rx path
  config_data type of data to obtain

  @Returns
  void * ptr of the current Rx band configuration ptr.

*/
void *rfc_lte_get_rx_band_config
(
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfc_band_config_data_enum_type config_data,
  uint8  alt_path
)
{
  void *data_config_ptr = NULL;
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_rx_band_config:RFC LTE object not initialized");
    return data_config_ptr;
  }

  data_config_ptr = rfc_lte_p->rx_band_config_get(device, band, config_data, alt_path);

  return (data_config_ptr);
} /* rfc_lte_get_rx_band_config() */

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function implements the card specific intializations for tx band
  changes.

  Needed only for TDD B40 on RFC_8853_B
  Notneeded on RFC_8853_A

  @details
  The band table which this RF card supports is not indexed in any way.
  Explicit logic will need to be provided to extract the band specific data
  structures.

*/
void rfc_lte_config_tx_band_data(rfm_device_enum_type device, rfcom_lte_band_type band)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_config_tx_band_data:RFC LTE object not initialized");
    return;
  }

  rfc_lte_p->tx_band_config(device, band);
} /* rfc_lte_config_tx_band_data() */

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function returns top level ptr to the current Tx band configuration.

  @details
  The function will return Tx configuration such as GPIO/GRFC settings along
  with any other parameter data structures for a particular band.  The void*
  allows for casting for usage under various band types.

  Dependency that rfc_set_band() be called before using this function.

  @parameter
  path request Rx path
  config_data type of data to obtain

  @Returns
  void * ptr of the current Tx band configuration ptr.

*/
void *rfc_lte_get_tx_band_config
(
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfc_band_config_data_enum_type config_data
)
{
  void *data_config_ptr = NULL;
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_tx_band_config:RFC LTE object not initialized");
    return data_config_ptr;
  }

  data_config_ptr = rfc_lte_p->tx_band_config_get(device, band ,config_data);

  return (data_config_ptr);
} /* rfc_lte_get_tx_band_config() */

/* ----------------------------------------------------------------------- */
/*!
  @brief

  @details


*/
void *rfc_lte_get_meas_data
(
   rfm_device_enum_type device,
   rfcom_lte_band_type band,
   rfc_band_config_data_enum_type config_data
)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_config_mode_data:RFC LTE object not initialized");
    return NULL;
  }

  return ( rfc_lte_p->rx_band_config_get(device, band, config_data, RFLTE_MC_DEFAULT_ALTERNATE_PATH) );
} /* rfc_lte_get_meas_data() */

/* ----------------------------------------------------------------------- */
/*!
  @brief

  @details


*/
void rfc_lte_manage_vregs(rfm_device_enum_type device, rfcom_lte_band_type band, rfm_path_state state)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();
  rf_path_enum_type path = rfcommon_core_device_to_path(device);

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_manage_vreg:RFC LTE object not initialized");
    return;
  }
  if(path > RF_PATH_1)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfc_lte_manage_vregs:RFC LTE Wrong device %d, Path %d", 
          device, path );
    return;
  }

  /* Protect the rfc sleep/wakeup functions with critical section */
  rf_common_enter_critical_section(rfc_get_lock_data());

  if (  state == RF_PATH_SLEEP_STATE)
  {
    /* disable the rf signals, put the signals into low power mode */
    rfc_common_sleep(path, RFM_LTE_MODE);
  }
  else
  {
    /* enable the rf signals, if the signals are in low power mode */
    rfc_common_wakeup(path, RFM_LTE_MODE);
  }
  
  rf_common_leave_critical_section(rfc_get_lock_data());

  /*rflte_pm_info structure is filled out here so all cards version can reference */
  rflte_pm_info.mode = RFM_LTE_MODE;
  rflte_pm_info.band = band;
  rflte_pm_info.path = path;
  rflte_pm_info.state = state;
  rflte_pm_info.rf_chip_num = RF_CHIP_0;  /*need to change for sglte in future*/

  rfc_lte_p->manage_vregs(&rflte_pm_info);

} /* rfc_lte_manage_vreg() */

/* ----------------------------------------------------------------------- */
/*!
  @brief

  @details


*/
int32 rfc_lte_command_dispatch( rfm_device_enum_type device, int32 cmd, void *data)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_command_dispatch:RFC LTE object not initialized");
    return 0;
  }

  return ( rfc_lte_p->command_dispatch(device, cmd,data) );
} /* rfc_lte_command_dispatch() */

/* ----------------------------------------------------------------------- */
/*!
  @brief

  @details


*/
void rfc_lte_tx_disable(rfm_device_enum_type device, rfcom_lte_band_type band)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_tx_disable:RFC LTE object not initialized");
    return;
  }

  rfc_lte_p->tx_disable(device, band);
}

/* ----------------------------------------------------------------------- */
/*!
  @brief

  @details


*/
void rfc_lte_mode_sleep(rfm_device_enum_type device, rfcom_lte_band_type band, uint8 alt_path)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_mode_sleep:RFC LTE object not initialized");
    return;
  }

  rfc_lte_p->mode_sleep(device, band, alt_path);
}

/* ----------------------------------------------------------------------- */
/*!
  @brief

  @details


*/
void rfc_lte_mode_exit(rfm_device_enum_type device, rfcom_lte_band_type rx_band, rfcom_lte_band_type tx_band, uint8 alt_path)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_mode_exit:RFC LTE object not initialized");
    return;
  }

  rfc_lte_p->mode_exit(device, rx_band, tx_band, alt_path);
}

void** rfc_lte_get_device(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                   rfcom_lte_band_type band, rfdevice_type_enum_type dev_type)
{
   rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_device:RFC LTE object not initialized");
    return NULL;
  }

  return (rfc_lte_p->get_rf_device_obj(rfm_dev, rx_tx, band, dev_type ));
}

boolean rfc_lte_setup_fb_path (rfm_device_enum_type device_fb, rfcom_lte_band_type band, boolean et_enable)
{
  boolean api_status = TRUE ;
  rfc_lte_intf *rfc_obj = rfc_lte_intf::Get_Instance();
	
  api_status = rfc_obj->setup_fb_path(device_fb, band, et_enable);
	
  return(api_status);
	
}

boolean rfc_lte_setup_fb_device (rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_lte_band_type band, uint32 tx_freq)
{
  boolean api_status = TRUE ;
  rfc_lte_intf *rfc_obj = rfc_lte_intf::Get_Instance();
	
  api_status = rfc_obj->setup_fb_device(device, device_fb, band, tx_freq);
	
  return(api_status);
	
}

boolean rfc_lte_disable_fb_device (rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_lte_band_type band, uint32 tx_freq)
{
  boolean api_status = TRUE ;
  rfc_lte_intf *rfc_obj = rfc_lte_intf::Get_Instance();
	
  api_status = rfc_obj->disable_fb_device(device, device_fb, band, tx_freq);
	
  return(api_status);
	
}
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band info. for the specified rx channel by querying RFC.

   @param
   split_band: Pointer to boolean which will be set to true if split band is found
   rx_chan: earfcn passed
   rfc_band: pointer to lte band, which will be updated if split band

*/

void rfc_lte_get_rx_band(boolean *split_band, rfcom_lte_earfcn_type rx_chan, rfcom_lte_band_type *rfc_band)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();
  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_device:RFC LTE object not initialized");
  }
  else
  {
    rfc_lte_p->get_rx_band(split_band, rx_chan, rfc_band);
  }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the tx band info. for the specified tx channel by querying RFC.

   @param
   split_band: Pointer to boolean which will be set to true if split band is found
   tx_chan: earfcn passed
   rfc_band: pointer to lte band, which will be updated if split band

*/
void rfc_lte_get_tx_band(boolean *split_band, rfcom_lte_earfcn_type tx_chan, rfcom_lte_band_type *rfc_band, boolean filtered_band)
{
   rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_device:RFC LTE object not initialized");
  }
  else
  {
    rfc_lte_p->get_tx_band(split_band, tx_chan, rfc_band, filtered_band);
  }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the low and high freq for the split band
 
   @param
   split_band: band for which low and high channels are desired
   *low_earfcn: Pointer to low chan
   *high_earfcn: Pointer to high chan
 
   @return

*/
void rfc_lte_get_rx_split_band_earfcns(rfcom_lte_band_type split_band, rfcom_lte_earfcn_type *low_earfcn, rfcom_lte_earfcn_type *high_earfcn)
{
   rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_rx_split_band_earfcns:RFC LTE object not initialized");
  }
  else
  {
    rfc_lte_p->get_rx_hi_lo_earfcn(split_band, low_earfcn, high_earfcn);
  }
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the low and high freq for the split band
 
   @param
   split_band: band for which low and high channels are desired
   *low_earfcn: Pointer to low chan
   *high_earfcn: Pointer to high chan
 
   @return

*/
void rfc_lte_get_tx_split_band_earfcns(rfcom_lte_band_type split_band, 
                                    rfcom_lte_earfcn_type *low_earfcn, 
                                    rfcom_lte_earfcn_type *high_earfcn)
{
   rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, 
        MSG_LEGACY_ERROR, 
        "rfc_lte_get_tx_split_band_earfcns:RFC LTE object not initialized");
  }
  else
  {
    rfc_lte_p->get_tx_hi_lo_earfcn(split_band, low_earfcn, high_earfcn);
  }
}

rfm_device_enum_type rfc_lte_get_mapped_device_for_scell
(
 rfcom_lte_band_type pcell_band,
 rfcom_lte_band_type scell_band,
 rfm_device_enum_type scell_rfm_device
)
{
  rfm_device_enum_type result_rfm_device = RFM_INVALID_DEVICE;
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_mapped_device_for_scell:RFC LTE object not initialized");
    return scell_rfm_device;
  }

  result_rfm_device = rfc_lte_p->get_mapped_device_for_scell(pcell_band,
                                                             scell_band,
                                                             scell_rfm_device);

  return result_rfm_device;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band based on certain selection params.

   @details
   The function will return rx band part of the rfc_band pointer. 

   @parameter
   @param
   split_band: Flag specifying if the path was split or not. 
   @param 
   rx_chan: tx channel
   @param 
   curr_bw: current system bw
   @param 
   path_index_override: Path override index to match the Tx path on the Band  
   @param 
   *rfc_band: updated rfc band.

*/

void rfc_lte_get_rx_band_v2(boolean *split_band, 
                            rfcom_device_enum_type device,
                            rfcom_lte_earfcn_type rx_chan,
                            rfc_lte_bw_type curr_bw,
                            uint8 path_index_override,
                            rfcom_lte_band_type *rfc_band)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();
  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, 
        "rfc_lte_get_rx_band_v2:RFC LTE object not initialized");
  }
  else
  {
    rfc_lte_p->get_rx_band_v2(split_band, device, rx_chan, curr_bw, path_index_override, rfc_band);
  }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the tx band based on certain selection params.

   @details
   The function will return tx band part of the rfc_band pointer. 

   @parameter
   @param
   split_band: Flag specifying if the path was split or not. 
   @param 
   rx_chan: tx channel
   @param 
   curr_bw: current system bw
   @param 
   path_index_override: Path override index to match the Tx path on the Band  
   @param 
   *rfc_band: updated rfc band.

*/
void rfc_lte_get_tx_band_v2(boolean *split_band, 
                            rfcom_lte_earfcn_type tx_chan,
                            rfc_lte_bw_type curr_bw,
                            uint8 path_index_override,
                            rfcom_lte_band_type *rfc_band,
                            boolean filtered_band)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();
  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, 
        "rfc_lte_get_tx_band_v2:RFC LTE object not initialized");
  }
  else
  {
    rfc_lte_p->get_tx_band_v2(split_band, 
                              tx_chan, 
                              curr_bw, 
                              path_index_override, 
                              rfc_band, 
                              filtered_band);
  }

}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band info. for the specified rx channel by querying RFC.

   @param
   split_band: Pointer to boolean which will be set to true if split band is found
   rx_chan: earfcn passed
   rfc_band: pointer to lte band, which will be updated if split band

*/

void rfc_lte_get_split_rx_bands(boolean *split_band, rfcom_lte_earfcn_type rx_chan, rfcom_lte_band_type *rfc_band1, rfcom_lte_band_type *rfc_band2)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();
  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_device:RFC LTE object not initialized");
  }
  else
  {
    rfc_lte_p->get_split_rx_bands((boolean *)split_band, rx_chan, (rfcom_lte_band_type *)rfc_band1, (rfcom_lte_band_type *)rfc_band2);
  }
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band info. for the specified rx channel by querying RFC.

   @param
   split_band: Pointer to boolean which will be set to true if split band is found
   rx_chan: earfcn passed
   rfc_band: pointer to lte band, which will be updated if split band

*/

void rfc_lte_get_rx_split_band_channel_rasters(rfcom_lte_band_type split_band, 
                                               rfcom_lte_earfcn_type *low_earfcn,
                                               rfcom_lte_earfcn_type *high_earfcn,
                                               rfcom_lte_band_type *band_list,
                                               uint8 *num_of_splits)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();
  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_device:RFC LTE object not initialized");
  }
  else
  {
    rfc_lte_p->get_rx_split_band_channel_rasters(split_band, low_earfcn, high_earfcn, band_list, num_of_splits);
  }
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function generates the AsDiv GRFCs script for the requested device
  and band.

  @param position
  RF device for which to generate GRFC script
 
  @param cb_data
  Call back data to pass in additional info for extendability

  @param asdiv_script_ptr
  Output - The GRFC script
 
  @return
  True on success and FALSE on failure
*/

#ifdef FEATURE_RF_ASDIV  
boolean rfc_lte_get_asdiv_xsw_script
( 
  uint32 position,
  rf_buffer_intf *asdiv_script_ptr,
  void* cb_data
)
{
  boolean status;
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();


  if ( asdiv_script_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "generate_asdiv_xsw_script : scrpit ptr is "
                      "null"); 
    return FALSE;
  }

  if ( rfc_lte_p == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rfc_lte_p is NOT created. ",0);
    return FALSE ;
  }

  status = rfc_lte_p->generate_asdiv_xsw_script((int)position ,asdiv_script_ptr, cb_data);


  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_lte_get_asdiv_xsw_script : "
                      "script generation failed");
  }

  return status;
} /* rfc_lte_get_asdiv_xsw_script */
#endif

void* rfc_lte_get_asd_xsw_config
(
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  uint32 position
)
{
  rfc_sig_cfg_type *rfc_asd_sig_info = NULL;
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if ( rfc_lte_p == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rfc_lte_p is NOT created. ",0);
    return rfc_asd_sig_info ;
  }

  rfc_asd_sig_info = 
    (rfc_sig_cfg_type *) rfc_lte_p->get_asd_xsw_config( device, band, (int)position );


  return rfc_asd_sig_info;

}
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function generates the AsDiv GRFCs script for the requested device
   and band.
  
   @param position
   RF device for which to generate GRFC script
  
   @param cb_data
   Call back data to pass in additional info for extendability
  
   @param asdiv_script_ptr
   Output - The GRFC script
  
   @return
   True on success and FALSE on failure
*/
boolean rfc_lte_get_srs_xsw_script( uint32 position,
                                    rf_buffer_intf *srs_xsw_script_ptr,
                                    rfm_device_enum_type device,
                                    rfcom_lte_band_type band,
                                    boolean immediate)
{
  boolean status = TRUE;
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  if ( srs_xsw_script_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "generate_srs_xsw_script : scrpit ptr is null"); 
    status = FALSE;
  }
  else
  {
     if ( rfc_lte_p == NULL )
     {
       MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"LTE RFC Obj is NOT created. ",0);
       status = FALSE;
     }
     else
     {
        status = rfc_lte_p->generate_srs_xsw_script((int)position,
                                                    srs_xsw_script_ptr,
                                                    device,
                                                    band,
                                                    immediate);
     }
  }

  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_lte_get_srs_xsw_script : "
                      "script generation failed");
  }

  return status;
} /* rfc_lte_get_srs_xsw_script */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function updates device ports with NS07 specific settings

   @param
   

*/

void rfc_lte_update_ns07_ports(rfm_device_enum_type rfm_dev,rfcom_lte_band_type curr_band)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();
  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_device:RFC LTE object not initialized");
  }
  else
  {
    rfc_lte_p->get_ns07_ports(rfm_dev,curr_band);
  }
}

rfdevice_rxtx_common_class * rfc_lte_get_common_rxtx_device
(
  rfm_device_enum_type device, 
  rfcom_lte_band_type band,
  uint32 alt_path
)
{
  rfc_device_info_type *device_info_ptr = NULL;
  /*Device type used to extract tech specific object*/
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  /*Device instance used to extract tech specific object*/
  uint32 instance = 0;
  rfc_cfg_params_type cfg;

  cfg.alternate_path = alt_path;
  cfg.band = band;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.path = rfcommon_core_device_to_path(device);

  /* Get RFC information */
  if (rfc_lte_data_get_device_info(&cfg, &device_info_ptr) == TRUE)
  {
     instance = device_info_ptr->rf_asic_info[0].instance;
     /*Retrieve device object*/
     return rfc_common_get_cmn_device_object((uint8)instance);
  }
  else
  {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Could not get device info for LTE band  %d",band);
     return NULL;
  }
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function gets the split bands for a particular 
  band.

  @param full_band
  Band for which split band info is required
 
  @return
  Value with bits corresponding to present split bands set to 1. 
  0 if no split bands.
*/
uint64 rfc_lte_get_split_bands(rfcom_lte_band_type full_band)
{
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();
  uint64 split_bands = 0;

  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_split_bands:RFC LTE object not initialized");
  }
  else
  {
    split_bands = rfc_lte_p->get_split_bands(full_band);
  }

  return split_bands;
  
}/* rfc_lte_get_split_bands(rfcom_lte_band_type full_band) */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band info. for the specified rx channel by querying RFC.

   @param pcc_info
   PCell carrier info pointer.
 
   @param scc_info
   SCell carrier info pointer.
 
   @param intra_ca_split_band
   Pointer to lte band, which will be updated if split band is feasible.

*/

boolean rfc_lte_check_intra_ca_feasible(void *pcc_info, 
                                     void *scc_info,
                                     rfcom_lte_band_type *intra_ca_split_band)
{
  boolean intra_band = FALSE;
  rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();

  MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfc_lte_check_intra_ca_feasible: check in");  
  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_lte_get_device:RFC LTE object not initialized");
  }
  else
  {
    intra_band = rfc_lte_p->check_intra_ca_feasible((rflte_mc_carrier_info_rfc_type*)pcc_info, 
                                       (rflte_mc_carrier_info_rfc_type*)scc_info, 
                                       intra_ca_split_band);
  }
  return intra_band;
}
boolean rfc_lte_get_partial_band_info (rfc_lte_properties_type **ptr)	
{
    boolean status = TRUE;
    rfc_lte_intf *rfc_lte_p = rfc_lte_intf::Get_Instance();
	
  if(rfc_lte_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, 
        "rfc_lte_get_rx_band_v2:RFC LTE object not initialized");
     status = FALSE;
    }
 else
   {
     status = rfc_lte_p->rfc_get_lte_properties(ptr);  
    }

  return status;
}
