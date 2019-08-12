#ifndef RFGSM_MDSP_H
#define RFGSM_MDSP_H

/*==========================================================================

              G S M   R F   T O   M D S P   H E A D E R    F I L E

DESCRIPTION

  This header file contains all the definitions necessary for the RF driver
  to interface with the MDSP.

  Copyright (c) 2003 - 2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_mdsp.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/14/16   ase     Store and dump CCS build script errors
12/23/15   bp      Stop further processing of therm_read_rsp if temp. compensation is stopped through GSM Tx Disable.
07/12/15   bp      Add api to get TX stop priority for HMSC case
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/20/14   hoh     Remove unused function declaration
10/24/14   sw      Move therm read event handler cleanup process  to apps task
10/23/14   sc      Enable event type control from RFSW
09/22/14   sc      Update band specific Tx timing adjust data when Tx band change
07/01/14   tsr     DR-DSDS: Lower RX CCS event priority when GSM sub is in Idle mode
07/01/14   jr      Enable therm read on sub-2  
05/20/14   zbz     Fix OFT error
05/06/14   tsr     Reuse ccs event handles for RX/TX start stop & trans 
04/25/14   zbz     Add api to upsample pa ramp tables from NV using spline interpolation algorithm 
04/07/14   sc      Clean event API
03/26/14   sb      Add support for IP2 cal for Dime PM 
03/13/14   sw      Improve logging for therm read 
03/06/14   sc      Remove dependency on rfmeas_sbi_type 
02/28/14   sc      Pass core data through mc to core layers 
02/19/14   tsr     GSM receive diversity 
02/17/14   sc      Clean up mDSP APIs 
02/11/14   sw      Remove therm read TRITON featurisation 
02/06/14   sw      Therm read to use RFFE and not SBI 
01/22/14   sw      Thermistor read changes
01/21/14   tsr     WTR TXAGC support for Multislot TX 
01/15/14   sc      Remove unused APIs using FW shared memory 
10/28/13   ggs     Downsample base AMAM temp comp LUT by a factor of 4  
10/28/13   ggs     Add API to return final calculated amam and ampm tables
10/21/13   sc      Mainline dynamic event building  
10/14/13   sc      Mainline core DM and RFLM code 
10/09/13   tsr     Dynamic RF CCS event changes for WTR3925
10/03/13   sc      RFLM DM interface data and dynamic event development 
09/27/13   sc      Modify RFLM GSM interface data programming functionality 
09/12/13   sc      RFLM interface encompassed by core DM 
08/19/13   sc      Prepare for merge from core DM to RFLM
08/01/13   sc      Move mDSP CCS APIs to CCS-specific files 
07/29/13   sml/svi ASD support for GSM tech
07/18/13   sc      Update RFLM data information (featurised) 
06/25/13   ec      Make changes to support Therm Read Cancellation 
05/30/13   tws     Pass the UMID required to trigger therm read to MDSP
03/25/13   tsr     Use device driver api to extract therm read from retrieved ssbi buffer 
03/12/13   tsr     Added to tag to each therm read event handle to identify the successful retrieval of that handle
03/12/13   tsr     Cleanup the therm read event handle before exit and local copy of the handle after therm receive
02/19/13   sc      Added APIs to allow GL1 to alloc and free Rx meas buffers
02/12/13   tsr     Added Apis to send therm read cmd to GFW  
02/12/13   sr      changes to free the memory alloocated during rfm_init() 
01/31/13   tsr     Added framework for recieving therm read response from GFW 
01/22/12   sb      Changes to enable QPOET transactions to go out every gsm frame 
01/14/13   tsr     Added changes for Therm read implementation  
01/03/12   sc      Added therm read data extraction API 
12/18/12   sc      Added therm read event generation framework 
12/20/12   ggs     Add framework for appending rf_on info to transition script
12/06/12   sc      Fix compiler warnings
12/03/12   sc      Cleanup mDSP shared variable interface
12/03/12   aka     Removed some prototypes
11/27/12   tws     Add prototype used by FTM.
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/19/12   sc      RF-Only mDSP update shared variables cleanup
10/11/12   sc      Cleanup up unrequired Rx burst buffer params and unused SPI API
10/08/12   sc      Added zero RF CCS transactions to event builder status return type
10/02/12   tc      Remove obsolete functions and globals. 
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK. 
09/04/12   sr      Removed the rf_buffer max size macro defines and use macros
                   defined in fw interface layer.
08/25/12   tsr     Increased Max Number of SBI Transactions to 250 for Enter mode 
08/14/12   ec      Remove FW semaphores from rfgsm_dynamic_tx_semaphore enum
08/06/12   sc      Edit API to update CCS header FW vars 
08/02/12   sc      Add num slots to setup Tx burst API 
07/30/12   sc      Added temporary gain parameter struct to enable FW SU release 
07/23/12   sr      Fixed the cust & paste err for the transition script size.
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/12/12   sc      Changed void pointer parameters to rf ccs hal buffer types
07/06/12   sc      Edit API parameters to support CCS buffer format 
07/06/12   sc      Remove unrequired num transactions init functionality 
07/06/12   sc      Added RF GSM CCS event builder API prototypes
06/27/12   sc      Added compilation dependencies to accommodate coordinated merge FW/SW/RF
06/19/12   sc      Enable shared memory support
04/01/12   lcl     Removed empty mdsp interface header file  
03/17/12   jfc     Remove deprecated mdsp_intf_v.h include
02/17/12   sn      Added support for SMPS PDM control in FTM mode
01/20/12   sn      Updated FW interface for QFE1320 in rfgsm_get_pa_ctl_type
12/20/11   sn      Added support for QFE1320
11/21/11   vrb     Add RxLM Buf Index to rfgsm_mdsp call
09/15/11    sr     added antenna tuner support code.
09/08/11   sr      Added GST(Gain State Tracking) feature
09/07/11   sb/rsr  Changes for Delay match setting to use new FW interface.
08/02/11   sr      Made changes to remove unused consts.
07/25/11   vrb     Added device interface to get GSM TxAGC info from script
07/12/11   vrb     Moved the slot mode control reg info to the device driver
07/07/11   vrb     Move device specific Tx DPLL Training Sequence into Device Driver
06/16/10   rsr     Picking up QSC6x95 change-04/13/10 jps Adding support to add slope to AMAM table based on temp bin
03/20/10   tws     Export the max TX power array.
02/08/10   tws     Add API functions to return MDSP specific constants.
01/26/10   tws     Restructure as per qdsp4_gsm.
10/12/09   sr      Fixed warnings.
09/17/09   tws     Export variable and function.
07/20/09   sr      Added generic mdsp interface for init_once and enter_mode. 
07/06/09   sr     changes to make the operating band to rfcom_gsm_band type
04/02/08   sr      added new functions to set delay match and pa precharge values
10/26/07   rsr     Added support for PLATFORM_F_SMPS
03/15/06   rsr     Added support for TX RSB correction.
03/06/06    rv     Added support for multislot power back off.
01/13/06    xw     Added rfgsm_egprs_mode in rfgsmmdsp_set_amam_profile().
06/29/05    rv     Added rfgsmmdsp_update_max_index for getting highest pwr index.
02/09/05   rsr     Added prototypes for rfgsmmdsp_init_polar
12/02/04   rv      Added support for 8PSK.
05/05/04    rv     Added prototype for rfgsmmdsp_init().
02/24/01   thh     Initial version.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "mdsp_intf.h"
#include "rfgsm_core_types.h"
#include "rfgsm_core.h"
#include "rf_hal_buffer.h"
#include "rfcommon_mdsp_event_intf.h"

#include "rfgsm_data_manager.h"



/*===========================================================================

                                 CONSTANTS

===========================================================================*/
#define CAL_SCRIPT_COUNT 6

#define MDSP_MAX_APT_SBI_TRANSACTIONS 8
#define MDSP_MAX_APT_NON_SBI_TRANSACTIONS 8

#define RFGSM_MDSP_SCALE_3P25_TO_3P6MHZ  72 /* 57.6MHz DAC frequency for 2 bit GP Data */
#define RFGSM_MDSP_SCALE_3P25_TO_4P8MHZ  96 /* 76.8MHz DAC frequency for 1 bit GP Data */


/*! @brief this used to retain the most commonly used ccs event handles to prevent 
  repeated allocs and frees */
typedef enum
{
  START_TASKQA,
  START_TASKQB,
  TRANS_TASKQA = START_TASKQA,
  TRANS_TASKQB = START_TASKQB,
  FE_TASKQA,
  FE_TASKQB,
  STOP_TASKQA,
  STOP_TASKQB,
  GSM_CCS_NUM_CCS_HANDLES_EVENTS
} rfgsm_ccs_handle_event_enum;


typedef struct
{

  rfcommon_mdsp_event_handle * rx_event[GSM_CCS_NUM_CCS_HANDLES_EVENTS];
  rfcommon_mdsp_event_handle * tx_event[GSM_CCS_NUM_CCS_HANDLES_EVENTS];
  rfcommon_mdsp_event_handle * tx_trans_event[TRANS_TASKQB+1][RFGSM_MAX_TX_SLOTS_PER_FRAME];

} rfgsm_mdsp_ccs_handles_s;


/* Data type for RXF digital gain setting */
typedef struct
{
   int16  gain_dB;   /* Gain in dB associated with the rxf_gain */
} rfgsm_mdsp_rxf_gain_type;


typedef struct
{
  uint32 sequence[10];
  uint32 num_guard_bits;
} rfgsm_mdsp_dpll_training_seq_type;


typedef struct
{
  boolean use_slot_mode_ctrl_flag;
  uint8 slot_mode_ctrl_1_reg_addr;
  uint8 slot_mode_ctrl_2_reg_addr;
}rfgsm_mdsp_slot_mode_ctrl_type;

typedef struct
{
  uint8 slot_num;
  rf_buffer_intf* buffer;
}rfdevice_tx_agc_info_type;

typedef enum
{
  AMAM_TABLE,
  AMPM_TABLE,
}rfgsm_mdsp_amam_ampm_type;

/*===========================================================================

                            FUNCTION PROTOTYPES

===========================================================================*/
void rfgsm_mdsp_init_once(rfm_device_enum_type rfm_dev);

void rfgsm_mdsp_deinit(void);

void rfgsm_mdsp_enter_mode_init(rfgsm_core_data_type *core_data);

void rfgsm_mdsp_set_tx_band(rfgsm_core_data_type *core_data);

void rfgsm_mdsp_compute_linear_amam_freq_comp (rfm_device_enum_type device, rfcom_gsm_band_type band, 
                                               uint16 arfcn);

void rfgsm_mdsp_compute_linear_amam_temp_comp(rfm_device_enum_type rfm_dev, boolean force_scaling);

boolean rfgsm_mdsp_get_therm_data(rfgsm_core_data_type *rfgsm_core_data, rf_hal_bus_rffe_type* buf_ptr, uint8 tag, uint16* num_rffe_retrieved);

boolean rfgsm_mdsp_get_ssbi_therm_data(rfgsm_core_handle_type *rfgsm_core_handle_ptr, rf_hal_bus_sbi_type* buf_ptr, uint8 tag, uint16* num_ssbi_retrieved);

boolean rfgsm_mdsp_populate_enter_mode_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx );

boolean rfgsm_mdsp_populate_wakeup_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx );

boolean rfgsm_mdsp_populate_tx_band_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx );

boolean rfgsm_mdsp_populate_therm_read_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx );

boolean rfgsm_mdsp_populate_rx_start_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx );

boolean rfgsm_mdsp_populate_rx_stop_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx );

boolean rfgsm_mdsp_populate_tx_start_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx );

boolean rfgsm_mdsp_populate_tx_trans_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx, uint8 slot_num );

boolean rfgsm_mdsp_populate_tx_stop_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx );

boolean rfgsm_mdsp_create_buffer( rfgsm_core_data_type *core_data_p,
                                  void **event_handle,
                                  uint32 *event_mem_ptr,
                                  uint8 script_type,
                                  uint8 task_q_id );

boolean rfgsm_mdsp_push_script_to_buffer( void* event_handle,
                                          rf_hal_bus_resource_type bus_resource,
                                          rf_buffer_intf* script,
                                          uint8 script_type,
                                          uint8 task_q_id ,
                                          boolean lower_rx_ccs_prio,
										  boolean ssbi_protocol_supported);

boolean rfgsm_mdsp_commit_buffer( void* event_handle );

void rfgsm_mdsp_cleanup_buffer( void** event_handle );

rflm_gsm_ccs_event_info_type * rfgsm_mdsp_get_event_map( void );

void rfgsm_mdsp_construct_read_therm_script_buffer(rf_buffer_intf* read_therm_buffer_ptr, 
                                                   rf_hal_bus_rffe_type* read_buf_ptr,
                                                   uint16 num_retrieved);

void rfgsm_mdsp_construct_read_therm_ssbi_script_buffer(rf_buffer_intf* read_therm_buffer_ptr, 
                                                        rf_hal_bus_sbi_type* read_buf_ptr,
                                                        uint16 num_retrieved);

void rfgsm_mdsp_send_gfw_therm_read_process_cmd( msgr_umid_type umid, uint8 event_handle_tag );

void rfgsm_mdsp_allocate_rx_script_buffers(rfgsm_core_handle_type *rfgsm_core_handle_ptr);


void rfgsm_mdsp_free_rx_script_buffers(rfgsm_core_handle_type *rfgsm_core_handle_ptr);

void rfgsm_mdsp_allocate_read_therm_script_buffers(rfgsm_core_handle_type *rfgsm_core_handle_ptr);

void rfgsm_mdsp_free_read_therm_script_buffers(rfgsm_core_handle_type *rfgsm_core_handle_ptr);

void rfgsm_mdsp_therm_read_event_cleanup(rfm_device_enum_type rfm_dev);

void rfgsm_mdsp_therm_read_event_store(rfgsm_core_handle_type *rfgsm_core_handle_ptr, void* event_handle);

uint16* rfgsm_mdsp_get_final_amam_ampm_table( rfm_device_enum_type rfm_dev, 
                                              rfgsm_mdsp_amam_ampm_type table_type );

boolean rfgsm_mdsp_populate_ip2_cal_setup_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx );


boolean rfgsm_mdsp_populate_ip2_cal_step_index( void *shared_mem_ptr, uint32 rflm_gsm_dm_intf_data_idx, uint8 step_index );

boolean rfgsm_mdsp_clean_event( void* event_ptr );

void rfgsm_mdsp_upsample_pa_ramp( const uint16* source_nv_ramp, uint16* destination_up_sampled_ramp, uint8 sample_freq_scaling );

void rfgsm_mdsp_device_status_read(void* task_ptr, uint8 num_tasks, uint8 event_tag);

uint8 rfgsm_mdsp_get_hmsc_tx_stop_priority( void );

void rfgsm_mdsp_dump_ccs_error_events_ext( uint8 sub_id );

#endif /* RFGSM_MDSP_H */
