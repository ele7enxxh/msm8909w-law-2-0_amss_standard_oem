#ifndef RFDEVICE_ASM_COMMON_H
#define RFDEVICE_ASM_COMMON_H
/*!
   @file
   rfdevice_pa_common.h

   @brief
   This file contains definition & prototypes for generic 3rd party pa devices

*/
/*==============================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_asm/api/rfdevice_asm_common.h#1 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/16/14   sn      ASM set band RFM Device support
08/04/14   sn      EFS ASM support
07/22/14   sn      Added SV voting to handle dual chain scenarios
06/25/14   vv      Cleanup old device creation APIs
06/18/14   aca     ASM therm support
06/02/14   sn      state machine to handle disable tx
05/28/14   aca     Per port trigger support
05/12/14   vv      Physical device creation support
05/05/14   sn      ASM class based API
4/22/2014  aca     TP130 Dev3 support
12/4/2013  vv      Added a voting mechanism to track active LTE bands for rx
                   Added private API parameter validity check
09/17/13   vs      Added API to modify autogen register settings
07/22/13   vs      updated port state changes for LTE and TDSCDMA bands only
07/08/13   yzw     Add ASM support for FTM command FTM_GET_RFFE_DEVICE_INFO
6/13/13    vbh     Added explicit trigger for the ASM device
05/31/13   vs      updated port state changes
05/15/13   vs      Updated changes for RFFE script timing
04/10/13   sn      Cleanup ASM interface
03/05/13   sr      Added self_test support.
12/11/12   aca     LTE CA shadow register support
12/04/12   aca     ASM timing, CA support
08/24/12   jyu     Added support for TDSCDMA
08/12/12   tks     Added support for returning script or direct write to hw
08/11/12   tks     Added support for storing band port information sent by rfc
07/22/12   tks     Added support for creating a device
07/18/12   tks     Added RFFE ID's as private member variables
07/18/12   tks     Removed support for command dispatch
07/18/12   tks     Added support to store band port mapping info specific to
                   ASM device
07/09/12   tks     Initial Revision

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "rfdevice_asm.h"
#include "rf_buffer_intf.h"
#include "rf_rffe_common.h"
#include "rfdevice_asm_types.h"
#include "rfdevice_asm_data.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_physical_third_party.h"

#define RFDEVICE_ASM_MAX_REGS 256
#define RFDEVICE_ASM_MAX_REG_SIZE 10
#define	DEFAULT_ASM_SWITCH_STATE 0
#define TOTAL_ASM_SWITCH_STATE 2

#define MAX_PATH_SIZE 100

  /*API to create 3rd party asm with physical device interface support.Once created successfully it registers itself as a child of the physical device object passed to it*/
  rfdevice_logical_component* create_gen_asm_object(
    rfdevice_physical_device         *rfdevice_physical_third_party_p,
    rfc_logical_device_info_type     *logical_device_cfg
  );



  class rfdevice_asm_common:public rfdevice_asm
  {
  public:

    boolean enable
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean sleep
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean wakeup
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean disable
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean set_tx_band_map(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode,
	                        rfcom_band_type_u band,
                            int32 *data_ptr,uint8 size);

    boolean set_rx_band_map(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode,
                            rfcom_band_type_u band, int32 *data_ptr,
                            uint8 size);
							
    boolean trigger(rfdevice_asm_cfg *asm_cfg_p, rf_buffer_intf *script_buffer,
                    rf_device_execution_type dev_action,
                    int16 script_timing);

    boolean asm_therm_on_off(boolean turn_on_off);

    boolean get_asm_temperature(int16 *temperature);

    virtual ~rfdevice_asm_common();

    /* Constructor overloaded to get the RFFE device details */
    rfdevice_asm_common(rfc_device_cfg_info_type *device_info);


    /* Constructor overloaded physical device support */
    rfdevice_asm_common(rfdevice_physical_device *phy_dev_obj_ptr,rfc_phy_device_info_type *phy_device_info, rfc_logical_device_info_type *logical_device_info);

    virtual boolean self_test(uint32 mfg_id, uint32 prd_id, uint32 rev_id);

    void reset(uint8 usid);

    /* **** following APIs are NOT supported in this device driver ***** */
    uint16 get_script_size();

    boolean set_switch_state(uint8 switch_state_set);
    boolean get_switch_state(uint8 *switch_state_get);

    boolean set_rx_switch_state(rfcom_mode_enum_type mode,
                                rfcom_band_type_u band,
                                uint8 switch_state_set);

    boolean set_tx_switch_state(rfcom_mode_enum_type mode,
                                rfcom_band_type_u band,
                                uint8 switch_state_set);

    boolean modify_asm_ag_setting(rfdevice_asm_mod_cfg_type *asm_reg_cfg,
                                  rfdevice_mod_reg_info_type *asm_reg_data);

    boolean enable_rx(rfdevice_asm_cfg *asm_cfg_p,
                      rf_buffer_intf *script_buffer,
                      rf_device_execution_type execution_type,
                      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

    boolean enable_tx(rfdevice_asm_cfg *asm_cfg_p,
                      rf_buffer_intf *script_buffer,
                      rf_device_execution_type execution_type,
                      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

    boolean disable_rx(rfdevice_asm_cfg *asm_cfg_p,
                       rf_buffer_intf *script_buffer,
                       rf_device_execution_type execution_type,
                       int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

    boolean disable_tx(rfdevice_asm_cfg *asm_cfg_p,
                       rf_buffer_intf *script_buffer,
                       rf_device_execution_type execution_type,
                       int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  protected:

    /*! pointer to the ASM device reg settings data object */
    rfdevice_asm_data *asm_data_ptr;
    boolean init_status;

    uint8 rffe_bus;  /*! RFFE channel/bus information */
    uint8 default_usid; /*! RFFE Default Unique Slave ID */
    uint8 assigned_usid;  /*! RFFE Programmable Unique Slave ID*/
    uint8 gsid;  /*! RFFE Group ID */

    boolean trigger_write_flag;

    int16 non_trigger_script_timing;


    uint64 LTE_active_bands;    // For a voting mechanism to track Active LTE bands for rx before disabling the ASM

    /*! ASM band_port info for each tech/band */
    uint8 cdma_band_port[RFM_MAX_DEVICES][RFM_CDMA_MAX_BAND][RFDEVICE_ASM_RXTX_MAX];
    uint8 wcdma_band_port[RFM_MAX_DEVICES][RFCOM_NUM_WCDMA_BANDS][RFDEVICE_ASM_RXTX_MAX];
    uint8 gsm_band_port[RFM_MAX_DEVICES][RFCOM_NUM_GSM_BANDS][RFDEVICE_ASM_RXTX_MAX];
    uint8 lte_band_port[RFM_MAX_DEVICES][RFCOM_NUM_LTE_BANDS][RFDEVICE_ASM_RXTX_MAX][TOTAL_ASM_SWITCH_STATE];    /*Switch state*/
    uint8 tdscdma_band_port[RFM_MAX_DEVICES][RFCOM_NUM_TDSCDMA_BANDS][RFDEVICE_ASM_RXTX_MAX][TOTAL_ASM_SWITCH_STATE];

    uint8 lte_shadow_regs[RFDEVICE_ASM_MAX_REGS];
    rf_rffe_data_type rffe_script[RFDEVICE_ASM_MAX_REG_SIZE];

    uint8 switch_state_lte[RFCOM_NUM_LTE_BANDS][RFDEVICE_ASM_RXTX_MAX];
    uint8 switch_state_tdscdma[RFCOM_NUM_TDSCDMA_BANDS][RFDEVICE_ASM_RXTX_MAX];

    rfdevice_physical_device* parent_physical_device_ptr;

    uint64 rx_tx_vote[RFM_NUM_MODES][RFDEVICE_ASM_RXTX_MAX];

    uint32 sv_vote;

  private:
    void init();

    boolean rfdevice_asm_param_validity(
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band,
      rf_buffer_intf *script_buffer,
      rf_device_execution_type execution_type,
      int16 script_timing= RFCOM_INVALID_TIMING_OFFSET);

    uint8 get_port(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band,
                   rfdevice_asm_rxtx_enum_type rxtx);

    boolean set_port(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band,
                     rfdevice_asm_rxtx_enum_type rxtx, int32 *data_ptr, uint8 size);

    boolean prepare_buffer(rfdevice_asm_reg_settings_type *asm_settings,
                           uint8 mask, uint8 *shadow_reg,
                           rf_buffer_intf *script_buffer,
                           rf_device_execution_type execution_type,
                           int16 timing,
                           uint8 *num_trans);

    boolean process_cmd_sequence(uint8 usid, rfdevice_asm_cmd_seq_type *cmd_seq, uint8 *reg_read);

    boolean config(rfdevice_asm_cfg *asm_cfg_p,
                   boolean asm_enable,
                   rf_buffer_intf *script_buffer,
                   rf_device_execution_type execution_type, int16 timing);

    boolean set_band_switch_state(rfcom_mode_enum_type mode,
                                  rfcom_band_type_u band,
                                  rfdevice_asm_rxtx_enum_type rxtx,
                                  uint8 switch_state_set);

    uint8 get_reg_mask(uint8 port);
    void print_device_info(void);
    void reset_rffe_script(void);

    rfdevice_asm_data* check_efs_asm(uint32 mfg_id, uint32 prd_id, uint32 rev_id);

  };

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*RFDEVICE_ASM_COMMON_H*/

