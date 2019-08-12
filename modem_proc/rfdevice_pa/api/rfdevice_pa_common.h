#ifndef RFDEVICE_PA_COMMON_H
#define RFDEVICE_PA_COMMON_H
/*!
   @file
   rfdevice_pa_common.h

   @brief
   This file contains definition & prototypes for generic 3rd party pa devices

*/
/*==============================================================================

  Copyright (c) 2013 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/api/rfdevice_pa_common.h#1 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/25/14   vv      Cleanup old device creation APIs
05/12/14   vv      Physical device creation support
03/03/14   vv      Added new API on_off(with cfg)
02/26/14   vv      Update set_pa() signature
11/05/13   vv      Added new APIs to set current bias(set_current_bias with cfg) and gain range(set_gain_range with cfg),
                   Added private APIs set_pa()(Common API to configure PA settings) and rfdevice_pa_param_validity() (Checks parameter validity)
09/17/13   vs      Added API to modify autogen register settings
09/04/13   sr/zhw  Add explicit default PA Icq value flag in PA device driver
07/07/13   sbo     Added BW support for PA device
06/25/13    vbh     Added explicit trigger for the PA device
06/20/13   vs      API support for PA therm
05/15/13   vs      Updated changes for RFFE script timing
04/10/13   sn      Cleanup PA interface
03/26/13   sr      Added debug log msg support.
03/21/13  yzw     Add device driver support to query RFFE device information {mfg_id, prd_id, rev_id}
03/05/13   sr      Added self_test support.
02/19/13   sr      Split the autogen with on/off/range/bias scripts.
01/29/13   spa     Added mode and band as members of PA class
01/22/13   sr      changes to create the device only if PA Reg settings available.
01/17/13   sr      changes to assignUSID and then create PA device
01/16/13   sr      changes to create the device only if successful usid assignment..
01/14/13   sr      PA autogen design changes.
12/20/12   aca     Per device LUT mapping
12/03/12   aca     Added support for timing info in scripts
10/29/12   tks     Added support for specifying the LUT type for each band
10/28/12   tks     Added support for capturing band specific PA mode logic
10/17/12   tks     Added support for modifying pa current bias in ftm mode
08/10/12   tks     Added support to look up PA settings and prepare script
07/26/12   tks     Added support for reading & writing to hw
07/22/12   tks     Added support for creating a device
07/18/12   tks     Added RFFE ID's as private member variables
07/18/12   tks     Removed support for command dispatch
07/18/12   tks     Added support to store band port mapping info specific to PA
                   device
07/09/12   tks     Initial Revision

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "rfdevice_pa.h"
#include "rf_buffer_intf.h"
#include "rf_rffe_common.h"
#include "rfdevice_pa_types.h"
#include "rfdevice_pa_data.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_physical_third_party.h"

#define RFDEVICE_PA_MAX_REG_SIZE 20
#define	DEFAULT_PA_SWITCH_STATE 0
#define TOTAL_PA_SWITCH_STATE 2

#define MAX_PATH_SIZE 100
   /* API to program the PA device ID based on device_info parameters */
    boolean rfdevice_pa_program_device_id_phy(rfc_phy_device_info_type *phy_device_info);

/*API to create 3rd party pa with physical device interface support.Once created successfully it registers itself as a child of the physical device object passed to it*/
rfdevice_logical_component* create_gen_pa_object(
                                                  rfdevice_physical_device     *rfdevice_physical_third_party_p,
                                                  rfc_logical_device_info_type      *logical_device_cfg
                                                  ); 


    class rfdevice_pa_common:public rfdevice_pa
    {
    public:

/* Constructor overloaded for physical device support */
  rfdevice_pa_common(rfdevice_physical_device *phy_dev_obj_ptr,rfc_phy_device_info_type *phy_device_info, rfc_logical_device_info_type *logical_device_info); 

        /* Constructor overloaded to get the RFFE device details */
        rfdevice_pa_common(rfc_device_cfg_info_type *device_info);

        virtual ~rfdevice_pa_common();

        boolean config(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer,
                       rf_device_execution_type execution_type, int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

        boolean set_pa_txagc(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer,
                             rf_device_execution_type dev_action,
                             int16 script_timing);

        boolean set_pa_on_off(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer,
                              rf_device_execution_type dev_action,
                              int16 script_timing);

        boolean set_pa_trigger(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer, rf_device_execution_type dev_action,
                               int16 script_timing);

        boolean sleep(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer, rf_device_execution_type execution_type,
                      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

        boolean wakeup(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer, rf_device_execution_type execution_type,
                       int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

        uint16 get_script_size();

        boolean set_band_map(rfcom_mode_enum_type mode, rfcom_band_type_u band,
                             int32 *data_ptr, uint8 size);

        boolean pa_therm_on_off(boolean turn_on_off);

        boolean get_pa_temperature(int16 *temperature);

        boolean set_switch_state(rfcom_mode_enum_type mode,
                                 rfcom_band_type_u band,
                                 uint8 switch_state_set);

        boolean modify_pa_ag_setting(rfdevice_pa_mod_cfg_type *pa_reg_cfg,
                                     rfdevice_mod_reg_info_type *pa_reg_data);

        void reset(uint8 usid);

        virtual boolean self_test(uint32 mfg_id, uint32 prd_id, uint32 rev_id);

    protected:


        void init();

        /*! pointer to the PA device reg settings data object */
        rfdevice_pa_data *pa_data_ptr;

        boolean init_status; /* PA device initialization status */

        uint8 rffe_bus;  /*! RFFE channel/bus information */

        uint8 default_usid; /*! RFFE Default Unique Slave ID */
        uint8 assigned_usid;  /*! RFFE Programmable Unique Slave ID*/
        uint8 gsid;  /*! RFFE Group ID */

        uint16 low_gain_range; /*! Lowest supported PA gain range */
		
        boolean lpm_pm_trig_flag;

        /*! PA band_port info for each tech/band */
        uint8 cdma_band_port[RFM_CDMA_MAX_BAND];
        uint8 wcdma_band_port[RFCOM_NUM_WCDMA_BANDS];
        uint8 lte_band_port[RFCOM_NUM_LTE_BANDS][TOTAL_PA_SWITCH_STATE];
        uint8 gsm_band_pri_port[RFCOM_NUM_GSM_BANDS]; /* for GMSK */
        uint8 gsm_band_sec_port[RFCOM_NUM_GSM_BANDS];  /* for 8PSK */
        uint8 tdscdma_band_port[RFCOM_NUM_TDSCDMA_BANDS][TOTAL_PA_SWITCH_STATE];
        rf_rffe_data_type rffe_script[RFDEVICE_PA_MAX_REG_SIZE];
        uint8 switch_state_lte[RFCOM_NUM_LTE_BANDS];
        uint8 switch_state_tdscdma[RFCOM_NUM_TDSCDMA_BANDS];

        rfdevice_physical_device* parent_physical_device_ptr;

    private:

        uint8 get_port(rfcom_mode_enum_type mode, rfcom_band_type_u band, boolean mod_type);

        boolean prepare_buffer(rfdevice_pa_reg_settings_type *settings, rf_buffer_intf *script_buffer,
                               rf_device_execution_type execution_type, int16 timing, uint8 *num_trans);

        boolean process_cmd_sequence(uint8 usid, rfdevice_pa_cmd_seq_type *cmd_seq, uint8 *reg_read);


        boolean rfdevice_pa_param_validity(rfcom_mode_enum_type mode, rfcom_band_type_u band,rfdevice_pa_bw_enum_type bw,
                                           rf_buffer_intf *script_buffer,rf_device_execution_type dev_action,int16 script_timing);

        void print_device_info(void);

        void reset_rffe_script(void);
		
		rfdevice_pa_data* check_efs_pa(uint32 mfg_id, uint32 prd_id, uint32 rev_id);
		
		

    };

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*RFDEVICE_PA_COMMON_H*/

