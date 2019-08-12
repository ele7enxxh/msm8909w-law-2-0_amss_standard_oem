/*!
  @file
  rf_cdma_pa.cpp

  @brief
  <b>This is the PA interface exposed to the MC layer.</b> This module 
  contains all CDMA-PA-specific data and implementations. 

  @addtogroup RF_RFC_CDMA_MC_INTF
  @{

*/

/*==============================================================================

  Copyright (c) 2008 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 
  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.d.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_pa.cpp#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
04/04/12   zhw     Initialze members to avoid garbage values
11/06/12   spa     Removed references to unused pa_type and pa_bus_type  
07/14/12   APU     Initial version

==============================================================================*/
#include "rf_cdma_pa.h"

//using namespace cdma_devices ;

#ifdef __cplusplus
extern "C" {
#endif
void* 
get_cdma_pa_device ( cdma_pa* pa ) 
{
  return (pa->get_misc_device_descriptor()) ;
}
#ifdef __cplusplus
}
#endif

/* constructor */
cdma_pa::cdma_pa ( int device_id ) 
{
  this->device_id =   device_id ;
  this->misc_device_descriptor = NULL;
  this->tx_band_port_mapping = NULL;
  this->rx_band_port_mapping = NULL;
}

/* Getters */
int
cdma_pa::get_device_id ( void ) 
{
 return this->device_id ;  
}

/* Setters */
bool
cdma_pa::set_tx_band_port  (
  rfm_cdma_band_class_type band,
  uint8 tx_band_port )
{
  return false ;
} /* tx_set_band_port */

bool
cdma_pa::set_rx_band_port  (
  rfm_cdma_band_class_type band,
  uint8 rx_band_port )
{
  return false ;
} /* tx_set_band_port */

bool 
cdma_pa::set_misc_device_descriptor ( void* misc_device_descriptor ) 
{
  bool is_null = true ;
  this->misc_device_descriptor = misc_device_descriptor ;
  if ( NULL != misc_device_descriptor )
  {
    is_null = false ;
  }
  return is_null ;
}


void* 
cdma_pa::get_misc_device_descriptor ( void ) 
{
  return this->misc_device_descriptor ;
}


/*! @} */
