#ifndef RFDEVICE_ASM_TUNER_H
#define RFDEVICE_ASM_TUNER_H
/*!
   @file
   rfdevice_asm_tuner.h

   @brief
   This file contains definition & prototypes for generic 3rd party GRFC Tuner devices

*/
/*==============================================================================

  Copyright (c) 2015-2015 Qualcomm Technologies Incorporated. All Rights Reserved

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
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_asm/api/rfdevice_asm_tuner.h#1 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/23/15   sr     Initial Revision

==============================================================================*/



#define RFDEVICE_ASM_TUNER_MAX_SCENARIO_NUM 7

#define RFDEVICE_ASM_TUNER_SCN_LOGIC_BITSHIFT(scn_num)       ((2 * scn_num) + 18) // SCN0:18,19 bits; SCN1:20,21 bits, etc.
#define RFDEVICE_ASM_TUNER_LOGIC_BITMASK             0x3       // 2 Bits
#define RFDEVICE_ASM_TUNER_VALID_FLAG_BITSHIFT       0x10      // Shift 16 bits : Bit 16,17
#define RFDEVICE_ASM_TUNER_VALID_FLAG_BITMASK        0x3       // 2 Bits
#define RFDEVICE_ASM_TUNER_GRFC_SIG_ID_BITSHIFT      0x0       // Shift 0 bits : Bits 0 - 15
#define RFDEVICE_ASM_TUNER_GRFC_SIG_ID_BITMASK       0xFFFF    // 16 bits

#define RFDEVICE_ASM_TUNER_SCN_LOGIC_GET(x, scn_num)   ((x>>RFDEVICE_ASM_TUNER_SCN_LOGIC_BITSHIFT(scn_num)) & RFDEVICE_ASM_TUNER_LOGIC_BITMASK)
#define RFDEVICE_ASM_TUNER_VALID_FLAG_GET(x)        ((x>>RFDEVICE_ASM_TUNER_VALID_FLAG_BITSHIFT) & RFDEVICE_ASM_TUNER_VALID_FLAG_BITMASK)
#define RFDEVICE_ASM_TUNER_GRFC_NUMBER_GET(x)       ((x>>RFDEVICE_ASM_TUNER_GRFC_SIG_ID_BITSHIFT) & RFDEVICE_ASM_TUNER_GRFC_SIG_ID_BITMASK)

#define RFDEVICE_ASM_TUNER_SIG_ID_SCN0_TO_SCN6_LOGIC(sig_name, scn0_logic, scn1_logic, scn2_logic, scn3_logic, scn4_logic, scn5_logic, scn6_logic) \
        (( (sig_name & RFDEVICE_ASM_TUNER_GRFC_SIG_ID_BITMASK) << RFDEVICE_ASM_TUNER_GRFC_SIG_ID_BITSHIFT ) | \
         ( (scn0_logic & RFDEVICE_ASM_TUNER_LOGIC_BITMASK ) << RFDEVICE_ASM_TUNER_SCN_LOGIC_BITSHIFT(0)  ) | \
         ( (scn1_logic & RFDEVICE_ASM_TUNER_LOGIC_BITMASK ) << RFDEVICE_ASM_TUNER_SCN_LOGIC_BITSHIFT(1)  ) | \
         ( (scn2_logic & RFDEVICE_ASM_TUNER_LOGIC_BITMASK ) << RFDEVICE_ASM_TUNER_SCN_LOGIC_BITSHIFT(2)  ) | \
         ( (scn3_logic & RFDEVICE_ASM_TUNER_LOGIC_BITMASK ) << RFDEVICE_ASM_TUNER_SCN_LOGIC_BITSHIFT(3)  ) | \
         ( (scn4_logic & RFDEVICE_ASM_TUNER_LOGIC_BITMASK ) << RFDEVICE_ASM_TUNER_SCN_LOGIC_BITSHIFT(4)  ) | \
         ( (scn5_logic & RFDEVICE_ASM_TUNER_LOGIC_BITMASK ) << RFDEVICE_ASM_TUNER_SCN_LOGIC_BITSHIFT(5)  ) | \
         ( (scn6_logic & RFDEVICE_ASM_TUNER_LOGIC_BITMASK ) << RFDEVICE_ASM_TUNER_SCN_LOGIC_BITSHIFT(6)  ) | \
         ( ( (TRUE & RFDEVICE_ASM_TUNER_VALID_FLAG_BITMASK) << RFDEVICE_ASM_TUNER_VALID_FLAG_BITSHIFT )))


#ifdef __cplusplus

#include "rfdevice_asm.h"
#include "rf_buffer_intf.h"
#include "rf_rffe_common.h"
#include "rfdevice_asm_types.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_physical_third_party.h"

/* API to create 3rd party GRFC Tuner with physical device interface support.
   Once created successfully it registers itself as a child of the physical device object passed to it
*/
extern "C" rfdevice_logical_component* create_gen_asm_tuner_object(rfdevice_physical_device *rfdevice_physical_third_party_p,
                                                                   rfc_logical_device_info_type *logical_device_cfg);



class rfdevice_asm_tuner : public rfdevice_asm
{
public:

  boolean enable(rfdevice_asm_cfg *asm_cfg_p,
                 rf_buffer_intf *buff_obj_ptr,
                 rf_device_execution_type dev_action,
                 int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean sleep(rfdevice_asm_cfg *asm_cfg_p,
                rf_buffer_intf *buff_obj_ptr,
                rf_device_execution_type dev_action,
                int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean wakeup(rfdevice_asm_cfg *asm_cfg_p,
                 rf_buffer_intf *buff_obj_ptr,
                 rf_device_execution_type dev_action,
                 int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean disable(rfdevice_asm_cfg *asm_cfg_p,
                  rf_buffer_intf *buff_obj_ptr,
                  rf_device_execution_type dev_action,
                  int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean set_tx_band_map(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode,
                          rfcom_band_type_u band,
                          int32 *data_ptr, uint8 size);

  boolean set_rx_band_map(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode,
                          rfcom_band_type_u band, int32 *data_ptr,
                          uint8 size);

  boolean trigger(rfdevice_asm_cfg *asm_cfg_p, rf_buffer_intf *script_buffer,
                  rf_device_execution_type dev_action,
                  int16 script_timing);

  virtual ~rfdevice_asm_tuner();

  /* Constructor overloaded to get the RFFE device details */
  rfdevice_asm_tuner(rfc_device_cfg_info_type *device_info);

  /* Constructor overloaded physical device support */
  rfdevice_asm_tuner(rfdevice_physical_device *phy_dev_obj_ptr, rfc_phy_device_info_type 
                     *phy_device_info, rfc_logical_device_info_type *logical_device_info);

  /* **** following APIs are NOT supported in this device driver ***** */
  uint16 get_script_size();

  // GRFC Tuner special APIs
  boolean update_scenario(uint8 scenario_num);

  boolean update_state(rfdevice_asm_cfg *asm_cfg_p, rf_buffer_intf *script_buffer,
                       rf_device_execution_type dev_action,
                       int16 script_timing);

protected:

  // scenario of the device based on the QMI message.
  uint8 scenario;

  /*! pointer to the ASM device reg settings data object */
  boolean init_status;

  uint64 LTE_active_bands;    // For a voting mechanism to track Active LTE bands for rx before disabling the ASM

  /*! ASM band_port info for each tech/band */
  int32 * cdma_band_data[RFM_MAX_WAN_DEVICES][RFM_CDMA_MAX_BAND][RFDEVICE_ASM_RXTX_MAX];
  int32 * wcdma_band_data[RFM_MAX_WAN_DEVICES][RFCOM_NUM_WCDMA_BANDS][RFDEVICE_ASM_RXTX_MAX];
  int32 * gsm_band_data[RFM_MAX_WAN_DEVICES][RFCOM_NUM_GSM_BANDS][RFDEVICE_ASM_RXTX_MAX];
  int32 * lte_band_data[RFM_MAX_WAN_DEVICES][RFCOM_NUM_LTE_BANDS][RFDEVICE_ASM_RXTX_MAX]; 
  int32 * tdscdma_band_data[RFM_MAX_WAN_DEVICES][RFCOM_NUM_TDSCDMA_BANDS][RFDEVICE_ASM_RXTX_MAX];

  uint8 band_data_size;

  rfdevice_physical_device *parent_physical_device_ptr;

private:

  boolean verify_params(rfdevice_asm_cfg *asm_cfg_p,
                        rf_buffer_intf *script_buffer,
                        rf_device_execution_type execution_type,
                        int16 script_timing);

  boolean set_band_data(rfm_device_enum_type rfm_device, 
                        rfcom_mode_enum_type mode, 
                        rfcom_band_type_u band,
                        rfdevice_asm_rx_tx_type rxtx,
                        int32 *data_ptr, uint8 size);

  int32 * get_band_data(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band,
                        rfdevice_asm_rx_tx_type rxtx);


  boolean prepare_buffer(int32 *band_data, uint8 band_data_size,
                         rf_buffer_intf *script_buffer,
                         rf_device_execution_type execution_type,
                         int16 timing);

  void print_device_info(void);

};


#endif /* extern "C" */

#endif /*RFDEVICE_ASM_TUNER_H*/

