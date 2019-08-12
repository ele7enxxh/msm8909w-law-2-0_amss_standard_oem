#ifndef RF_CDMA_ASM_H
#define RF_CDMA_ASM_H
/*!
  @file
  rf_cdma_asm.h

  @brief
  <b>This is the ASM interface exposed to the MC layer.</b> This module
  contains all CDMA-ASM-specific data and implementations.

  @addtogroup RF_RFC_CDMA_MC_INTF
  @{

*/

/*==============================================================================

  Copyright (c) 2008 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_asm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/03/14   zhw     Remove hard-coded mode & band in sleep/wakeup callflow
05/05/14   sn      Added ASM sleep wakeup callflow
10/02/13   APU     Split ASM logically into Rx and TX ASMs
07/12/13   spa     Update #def being used for max possible instances per band
07/08/13   yzw     Add method get_asm_device_info()
06/10/13   sty     Deleted unused method
01/15/13   cd      Add support for CDMA ASM aggregator functionality
11/06/12   spa     Removed references to unused asm_type and asm_bus_type
07/14/12   APU     Initial version

==============================================================================*/
#include "comdef.h"
#include "rfm_cdma_band_types.h"
#include "rfc_common.h"
#include "rfc_class.h"

#ifdef __cplusplus

/*!
  @brief
  RF CDMA ASM wrapper device class

  @details
  This class acts as an aggregator class for RF ASM device objects. It is
  provided to configure more than one ASM device for a specific band/path
  configuration.
*/

typedef enum
{
  RX_ASM ,
  TX_ASM ,
  MAX_CDMA_ASM ,
  UNKNOWN_ASM = MAX_CDMA_ASM
} rf_cdma_asm_type ;

class rf_cdma_asm
{
protected:
  int    rfm_device;
  void*  misc_device_descriptor[MAX_CDMA_ASM][RFC_MAX_DEVICE_INSTANCES_PER_BAND];
  uint8  num_asm_instances[MAX_CDMA_ASM];

public:
  /* Constructor - Initialize the aggregator object */
  rf_cdma_asm( int rfm_device );

  /* Add a new RF ASM device to the aggregator */
  bool add_device_instance( void* misc_device_descriptor , rf_cdma_asm_type asm_type ) ;

  /* Interface for ASM RX device wakeup */
  bool device_wakeup_rx( rfm_mode_enum_type mode,
                         rfm_cdma_band_class_type band,
                         const rf_buffer_intf* rf_script,
                         rf_device_execution_type dev_action );

  /* Interface for ASM RX device sleep */
  bool device_sleep_rx( rfm_mode_enum_type mode,
                        rfm_cdma_band_class_type band, 
                        const rf_buffer_intf* rf_script,
                        rf_device_execution_type dev_action );


  /* Interface for ASM TX device wakeup */
  bool device_wakeup_tx( rfm_mode_enum_type mode,
                         rfm_cdma_band_class_type band, 
                         const rf_buffer_intf* rf_script,
                         rf_device_execution_type dev_action );

  /* Interface for ASM TX device sleep */
  bool device_sleep_tx( rfm_mode_enum_type mode,
                        rfm_cdma_band_class_type band, 
                        const rf_buffer_intf* rf_script,
                        rf_device_execution_type dev_action );

  /* Interface for ASM device Rx configuration */
  bool device_enable_rx( rfm_mode_enum_type mode,
                         rfm_cdma_band_class_type band,
                         const rf_buffer_intf* rf_script,
                         rf_device_execution_type dev_action );

  /* Interface for ASM device Tx configuration */
  bool device_enable_tx( rfm_mode_enum_type mode,
                         rfm_cdma_band_class_type band,
                         const rf_buffer_intf* rf_script,
                         rf_device_execution_type dev_action );

  /* Interface for ASM device Rx disable */
  bool device_disable_rx( rfm_mode_enum_type mode,
                          rfm_cdma_band_class_type band,
                          const rf_buffer_intf* rf_script,
                          rf_device_execution_type dev_action );

  /* Interface for ASM device Tx disable */
  bool device_disable_tx( rfm_mode_enum_type mode,
                          rfm_cdma_band_class_type band,
                          const rf_buffer_intf* rf_script,
                          rf_device_execution_type dev_action );

  /* Update the ASM info when the alt path changes */
  bool update_device_instance( void* updated_device , 
                               rf_cdma_asm_type asm_type , 
                               uint8 asm_number ) ;

  /*Get ASM device ID information*/
  bool get_asm_device_info( uint16 *mfg_id, uint16 *prd_id,uint16 *rev_id );
};

#else
/* For visibilty from C code */
typedef struct rf_cdma_asm rf_cdma_asm ;

boolean
rf_cdma_asm_device_wakeup_rx
(
  rf_cdma_asm* sm,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band, 
  const rf_buffer_intf* rf_script,
  rf_device_execution_type dev_action
);

boolean
rf_cdma_asm_device_sleep_rx
(
  rf_cdma_asm* sm,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band, 
  const rf_buffer_intf* rf_script,
  rf_device_execution_type dev_action
);

boolean
rf_cdma_asm_device_wakeup_tx
(
  rf_cdma_asm* sm,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band, 
  const rf_buffer_intf* rf_script,
  rf_device_execution_type dev_action
);

boolean
rf_cdma_asm_device_sleep_tx
(
  rf_cdma_asm* sm,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band, 
  const rf_buffer_intf* rf_script,
  rf_device_execution_type dev_action
);

boolean
rf_cdma_asm_device_enable_rx
(
  rf_cdma_asm* sm,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  const rf_buffer_intf* rf_script,
  rf_device_execution_type dev_action
);

boolean
rf_cdma_asm_device_enable_tx
(
  rf_cdma_asm* sm,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  const rf_buffer_intf* rf_script,
  rf_device_execution_type dev_action
);

boolean
rf_cdma_asm_device_disable_rx
(
  rf_cdma_asm* sm,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  const rf_buffer_intf* rf_script,
  rf_device_execution_type dev_action
);

boolean
rf_cdma_asm_device_disable_tx
(
  rf_cdma_asm* sm,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  const rf_buffer_intf* rf_script,
  rf_device_execution_type dev_action
);
#endif /* #ifdef __cplusplus */

/*! @} */
#endif /* #ifndef RF_CDMA_ASM_H */



