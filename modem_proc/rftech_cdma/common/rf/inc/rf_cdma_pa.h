#ifndef RF_CDMA_PA_H
#define RF_CDMA_PA_H
/*!
  @file
  rf_cdma_pa.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_pa.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
11/06/12   spa     Removed references to unused asm_type and asm_bus_type  
07/14/12   APU     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_cdma_band_types.h"

#ifdef __cplusplus
extern "C" 
{
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
//namespace cdma_devices 
//{
  class cdma_pa
  {
    protected:
      int 				  	  	device_id ;
      void*						    misc_device_descriptor ;
      int*                tx_band_port_mapping ;
      int*                rx_band_port_mapping ;
      
    public:
      /* constructor */
      cdma_pa ( int device_id ) ;
  
      /* Getters */
      int get_device_id ( void ) ;  
      void* get_misc_device_descriptor ( void ) ;
  
      /* Setters */
      bool set_misc_device_descriptor ( void* misc_device_descriptor ) ;
      bool set_tx_band_port (rfm_cdma_band_class_type band, 
                              uint8 tx_band_port ) ;
      bool set_rx_band_port (rfm_cdma_band_class_type band, 
                              uint8 rx_band_port ) ;
  } ;
//} /* namespace::cdma_devices */

#else
/* For visibilty from C code */
typedef struct cdma_pa cdma_pa ;

void* 
get_cdma_pa_device ( cdma_pa* pa ) ;

#endif /* #ifdef __cplusplus */

/*! @} */
#endif /* #ifndef RF_CDMA_PA_H */



