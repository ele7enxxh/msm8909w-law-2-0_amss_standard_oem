#ifndef RFDEVICE_GNSS_INTF_H
#define RFDEVICE_GNSS_INTF_H

/*!
  @file
  rfdevice_gnss_intf.h

  @brief
  This is an abstract software interface to control or query the RF
  device for the GNSS operation.

  @details
  This is a generic GNSS RF device driver interface which consists
  of a collection of data and function pointers for GNSS operation. The
  interface is transparent to its clients. In another word, the client
  does not need to know what physical RF devices it's talking to. The dynamic
  binding between the logical radio and the physical radio is done in RFC
  (RF Configuration) which is a combination of an RF Card and an QCT modem. 
 
  RFC knows what specific RF devices are present in an installed RF card.
  It's also aware of which specific RF device is in use for a given GNSS
  RF band. Once the connection betwen the logical radio path and the physical
  radio path is determined, RFC will call the explicit RF device driver to
  update the data and function pointers structure.
 
  After the data and function pointer structure is updated, all the device
  driver data and functions within above structure are mapped to an exact RF
  device, thus allowing GNSS Main Control, Core and Feature to control or
  query the RF Device.

*/

/*===========================================================================

Copyright (c) 2009 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_gnss_intf.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/15/15   zg      Added support for 2 wtr voting
04/30/13   gy      Added support for another device
08/16/12   shb     Renamed rfdevice_enum_type to rfdevice_id_enum_type
01/21/10   xw      Lint error fixes  
11/10/09   dbz     Moved device specifc typedefs into RTR library, cleanup intf
11/09/09   dbz     Changed to generic RTR naming convention
10/23/09   dbz     Seperate common/RTR specific func, cmd dispatcher, 
                   RTR library cleanup 
10/05/09   xw      Initial version.

============================================================================*/

#include "rfcom.h"
#include "rfdevice_type_defs.h"
#include "rfdevice_intf_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  uint8 tia_dac_i; 
  uint8 tia_dac_q; 
  uint8 pga_dac_i; 
  uint8 pga_dac_q; 
  int32 pga_gain;
  uint8 vco_tune;
}rfgnss_device_rtr_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initializes GPS portion of the RTR. This function must be called 
  at anytime GPS is activated.

  @details

  @param gsm_band: GSM Band 
  
  @return
*/
typedef boolean (*gnss_init_func_ptr_type)(rtr_gnss_function_type gnss_function);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function puts the GPS portion of RFR8600 to sleep.

  @details

  @param 
  
  @return
*/
typedef void (*gnss_sleep_func_ptr_type)(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles needed library support for pausing GPS when it shares 
   second rx chain with 1x page. Applicable when 1x target uses RF ASIC 
   typically found on UMTS target

   @details

   @param None

   @retval None
*/
typedef void (*gnss_pause_func_ptr_type)(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles needed library support for resuming GPS when it shares 
   second rx chain with 1x page. Applicable when 1x target uses RF ASIC 
   typically found on UMTS target

   @details

   @param None

   @retval None
*/
typedef boolean (*gnss_resume_func_ptr_type)(void);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   Provides gnss generic dispatch functions.
   
   @details   

   @param 

   @retval
*/
typedef void (*gnss_cmd_dispatch_func_ptr_type)( rfdevice_cmd_enum_type cmd, void *data);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function votes for sleeping WTR on behalf of GNSS

  @details

  @param 
  
  @return
*/
typedef boolean (*gnss_pwr_sleep_func_ptr_type)(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function votes for waking up WTR on behalf of GNSS

  @details

  @param 
  
  @return
*/
typedef boolean (*gnss_pwr_wakeup_func_ptr_type)(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   rf gnss interface function pointers.

   @details
   An rf gnss independent interface is provided to gnss main control, core 
   and feature modules through these functions pointers. These function pointers
   will be initialized by the specific rf device when entering gnss mode.
*/
typedef struct 
{
  rfdevice_id_enum_type device_id; 
  gnss_init_func_ptr_type init_fp;
  gnss_sleep_func_ptr_type sleep_fp;
  gnss_pause_func_ptr_type pause_fp ;
  gnss_resume_func_ptr_type resume_fp;
  gnss_cmd_dispatch_func_ptr_type cmd_dispatch_fp;
  gnss_pwr_wakeup_func_ptr_type pwr_wakeup_fp;
  gnss_pwr_sleep_func_ptr_type pwr_sleep_fp;
} rfdevice_gnss_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfgnss_ds: GNSS specific RF device structure interface
*/
extern rfdevice_gnss_type *rfgnss_ds;
extern rfdevice_gnss_type *rfgnss_ds1;

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_GNSS_INTF_H */
