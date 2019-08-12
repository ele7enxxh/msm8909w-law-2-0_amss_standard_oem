#ifndef FTM_RF_TEST_INTERFACE_H
#define FTM_RF_TEST_INTERFACE_H
/*!
  @file
  ftm_rf_test_interface.h

  @brief
  Common framework to perform radio test in FTM mode
*/

/*======================================================================================================================

  Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

======================================================================================================================*/

/*======================================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rffactory/intf/ftm_rf_test_interface.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
06/24/16   jfc     Rename rx_mode continuous_mode and add burst_pattern property
06/21/16   jfc     Add wait_trigger subcommand to FTM RF Test
06/07/16   jps     Add new properties to TX measure (GSM measurements)
06/06/16   jfc     Rename ftm_rf_test functions with ftm_properties_framework prefix
06/02/16   jps     Use DEBUG_DATA property for blob data
05/25/16   jps     TX Measure - Remove Sig Path and Antenna Path properties
05/16/16   chm     Added new property type for rx measure cmd
05/11/16   jps     Add new properties for sig path & ant switch path in TX Measure
05/06/16   jfc     Added support for freq adj in radio config
04/28/16   chm     Return the IQ samples used to compute the IQ reports
04/20/16   jfc     Added support for diag subsys128 and multi response packets
04/10/16   zhw     Added support for FTM IRAT CONFIG
04/04/16   zhw     Addeds support for New FTM irat Framework
03/31/16   zhw     Moved RF Test Name Info Display to rffactory component
03/25/16   jps     Added support for TX MEASURE command
03/22/16   zhw     Added support for MSIM CONFIG in radio config
03/21/16   Saul    [FTM_TRM] Antenna Path support.
03/18/16   zhw     Added support for Ant path in radio config
03/08/16   zhw     Added support for Sensitivty, CtoN, Peak Freq measurement in Rx Measure
02/24/16   vv      [FTM_RF_TEST] - Adding enum ftm_trm_rra_property_type  
02/24/16   jfc     Add IQ capture subcommand code and enums
02/19/16   br      Added support for Mod_type change in FTM mode
02/18/16   jfc     [FTM RF TEST] Move subscriber and tech out of RF test header
02/16/16   zc      Adding Number of Rx Bursts for GSM
01/18/16   Saul    [FTM_RF_TEST] Recommended Radio Allocations API
01/04/16   zhw     Remove LTE PATH IDX property
12/14/15   zhw     Added Tx slot & Tx waveform support for GSM
11/20/15   zhw     Added support for LTE Path Index in Tx control
11/15/15   zhw     Added support for LTE Path Index in Rx Measure
10/27/15   zhw     Added support for LTE Path Index & Tx Control Properties
10/22/15   zhw     Added support for sig path property in rx measure and Tx control
09/16/15   zhw     Added support for sig path property in radio config
08/11/15   zhw     Added support for Tx Control Command
08/07/15   zhw     Update Rx Measure Property List. Add Rx Mode support in Radio Config
08/04/15   zhw     Added Tech Override support for Rx Measure Command
07/29/15   zhw     Moved REQ & RSP payload type to interface
                   Added support for Rx Measure command
04/15/15   aro     Added subcriber info in radio config unpack data structure
04/15/15   aro     Doxygen fix
04/15/15   aro     Added error codes to detect uncompression failures
04/15/15   aro     Added support for compressed packet
04/15/15   aro     PACK fix for OFT
04/14/15   aro     Extern C
04/14/15   aro     Added Radio Congig test case
04/14/15   aro     Added generic dispatch mechanism
04/13/15   aro     [1] Added support for radio config handler function
                   [2] Added error reporting
04/13/15   aro     Added common response infrastructure
04/13/15   aro     Restructuring/Renaming
04/13/15   aro     Renamed filenames
04/13/15   aro     Added radio config unpack function
04/08/15   aro     [1] Added error code
                   [2] Rename type/fuunc/variable name
                   [3] Initial support for radio config handler
04/08/15   aro     Fixed subscriber ID enum name
04/08/15   aro     Added Subscriber ID
04/08/15   aro     Added command ID
04/08/15   aro     Added initial type definition
04/08/15   aro     Initial Release

======================================================================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef T_WINNT
#error code not present
#endif

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the reserved field in FTM test command packet.  */
typedef uint16 ftm_rf_test_comamnd_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the reserved field in FTM test command packet.  */
typedef uint32 ftm_rf_test_field_reserved_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the version field in FTM test command packet.  */
typedef uint32 ftm_rf_test_field_version_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the subscription field in FTM test command packet.  */
typedef uint32 ftm_rf_test_field_subscriber_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the size of the technology field in FTM test command packet.  */
typedef uint32 ftm_rf_test_field_technology_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition to define the error code mask for response packet. Each bit indicates a type of error. */ 
typedef uint32 ftm_rf_test_field_error_code_mask_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet */
typedef PACK(struct)
{
  uint8 diag_cmd;
  /*!< Command Identification */

  uint8 diag_subsys_id;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 rf_subsys_id;
  /*!< RF Mode Identification. 101 for FTM_RF_TEST_C */

  ftm_rf_test_comamnd_type test_command;
  /*!< FTM test command defined by #ftm_rf_test_command_enum_type */

  uint16 req_len;
  /*!< Defines the request length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes 
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

  uint16 rsp_len;
  /*!< Defines the response length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes 
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

} ftm_rf_test_diag_header_type;


/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM command request packet */
typedef PACK(struct)
{
  uint8 diag_cmd;
  /*!< Command Identification */

  uint8 diag_subsys_id;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 rf_subsys_id;
  /*!< RF Mode Identification. 101 for FTM_RF_TEST_C */

  uint32 status;
  /*!< Status field. Not currently used */

  uint16 delayed_rsp_id;
  /*!< Delayed response ID. 0 if only 1 response packet. >0 for more than 1 responses. This is a common
  ID for all packets belonging to the same request  */

  uint16 rsp_cnt;
  /*!< Response count. 0 for the first response packet, 1 for the second response packet, etc. If more than
  two responses, the MSB (0x8000) will be set for all responses except for the last response. (e.g. 0x8000,
  0x8001, 0x8002, 0x0003). The MSB can be thought of as the "more to come" bit. The astute will wonder why 
  the "two response" case doesn't set the MSB of the first response...good question, that's just the way 
  the diag team defined this field. */

  ftm_rf_test_comamnd_type test_command;
  /*!< FTM test command defined by #ftm_rf_test_command_enum_type */

  uint16 req_len;
  /*!< Defines the request length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes 
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

  uint16 rsp_len;
  /*!< Defines the response length of the FTM diag packet, <b>when the packet is uncompressed</b>. This includes 
  the summation of the size of ftm_rf_test_diag_header_type and size of remaning paylaod (when it is uncompressed) */

} ftm_rf_test_diag_header_subsys128_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command request packet */
typedef PACK(struct)
{
  ftm_rf_test_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_test_field_version_type version;
  /*!< Version of packet */

  ftm_rf_test_field_subscriber_type subscriber;
  /*!< Indicates the subscription ID defined by #ftm_rf_test_command_enum_type */

  ftm_rf_test_field_technology_type technology;
  /*!< Indicates the technology (defined by ftm_rf_technology_type) for which the command is sent */

} ftm_rf_test_command_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command request packet 
  V2 moves subscriber and technology fields out of test command header and into payload */
typedef PACK(struct)
{
  ftm_rf_test_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_test_field_version_type version;
  /*!< Version of packet */

} ftm_rf_test_command_header_type_v2;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command request packet */
typedef PACK(struct)
{
  ftm_rf_test_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM request packet */

  ftm_rf_test_command_header_type command_header;
  /*!<  Structure defining the command header for RF Test command packet */

} ftm_rf_test_req_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command request packet 
  V2 moves subscriber and technology fields out of test command header and into payload */
typedef PACK(struct)
{
  ftm_rf_test_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM request packet */

  ftm_rf_test_command_header_type_v2 command_header;
  /*!<  Structure defining the command header for RF Test command packet */

} ftm_rf_test_req_header_type_v2;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command response packet */
typedef PACK(struct)
{
  ftm_rf_test_field_reserved_type reserved;
  /*!< Reserved Field */

  ftm_rf_test_field_version_type version;
  /*!< Version of packet */

  ftm_rf_test_field_error_code_mask_type common_error_code;
  /*!< Error code from the common framework */

} ftm_rf_test_command_rsp_header_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command response packet */
typedef PACK(struct)
{
  ftm_rf_test_diag_header_type diag_header;
  /*!<  Structure defining the header for FTM response packet */

  ftm_rf_test_command_rsp_header_type command_header;
  /*!<  Structure defining the command header for RF Test response packet */

} ftm_rf_test_header_rsp_type;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Defintion of command header for FTM RF Test command response packet, Diag subsys 128 */
typedef PACK(struct)
{
  ftm_rf_test_diag_header_subsys128_type diag_header;
  /*!<  Structure defining the header for FTM response packet using subsys128 */

  ftm_rf_test_command_rsp_header_type command_header;
  /*!<  Structure defining the command header for RF Test response packet */

} ftm_rf_test_header_rsp_subsys128_type;

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_CID
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of commands available for FTM test command */
typedef enum
{
  FTM_RF_TEST_CMD_UNASSIGNED = 0, /*!< 0 : Unassigned command */

  FTM_RF_TEST_CMD_RADIO_CONFIGURE = 1, /*!< 1 : Radio Configure command */

  FTM_RF_TEST_CMD_RX_MEASURE = 2, /*!< 2 : Rx Measure command */

  FTM_RF_TEST_CMD_TX_CONTROL = 3, /*!< 3 : Tx Control command */

  FTM_RF_TEST_CMD_QUERY_RECOMMENDED_RADIO_ALLOCATION = 4, /*!< 4 : RF Radio Allocation command */

  FTM_RF_TEST_CMD_RESERVED_5 = 5, /*!< 5 : Reserved  */

  FTM_RF_TEST_CMD_IQ_CAPTURE = 6, /*!< 6 : IQ Capture command */

  FTM_RF_TEST_CMD_TX_MEASURE = 7, /*!< 8 : TX Measure command */

  FTM_RF_TEST_CMD_MSIM_CONFIG = 8, /*!< 8 : MSIM Config command */

  FTM_RF_TEST_CMD_IRAT_CONFIG = 9, /*!< 9 : IRAT CONFIG command */

  FTM_RF_TEST_CMD_WAIT_TRIGGER = 10, /*!< 10 :  Wait Trigger command */

  FTM_RF_TEST_CMD_NUM  /*!< Max : Defines maximum number of command IDs */

} ftm_rf_test_command_enum_type;

/*! @} */ /* FTM_TEST_TOOLS_CID */

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_SUBS
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of subscribers */
typedef enum
{
  FTM_RF_TEST_SUBSCRIBER_0 = 0, /*!< 0 : First subscriber */

  FTM_RF_TEST_SUBSCRIBER_1 = 1, /*!< 1 : Second subscriber */

  FTM_RF_TEST_SUBSCRIBER_NUM,  /*!< Max : Defines maximum number of subscriber IDs */

  FTM_RF_TEST_SUBSCRIBER_NA = 0xFF  /*!< Not Applicable */

} ftm_rf_test_subscriber_enum_type;

/*! @} */ /* FTM_TEST_TOOLS_SUBS */

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_ERROR_CODE
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of subscribers */
typedef enum
{
  FTM_RF_TEST_EC_SUB_ALLOC = 0, /*!< 0 : Indicates subscription allocation failure */

  FTM_RF_TEST_EC_SUB_RELEASE = 1, /*!< 1 : Indicates subscription release failure */

  FTM_RF_TEST_CMD_HANDLER_FAIL = 2, /*!< 2 : Indicates command handlder failure */

  FTM_RF_TEST_EC_NULL_PARAM = 3, /*!< 3 : Indicates NULL parameter */

  FTM_RF_TEST_EC_RSP_CREATION = 4, /*!< 4 : Failure during response creation */

  FTM_RF_TEST_EC_BAD_CMD = 5, /*!< 5 : Bad Command */

  FTM_RF_TEST_EC_BAD_CMD_HANDLER = 6, /*!< 6 : Indicates bad command hander */

  FTM_RF_TEST_EC_UMCOMPRESS_FAIL = 7, /*!< 7 : Uncompress failure */

  FTM_RF_TEST_EC_MALLOC_FAILRE = 8, /*!< 8 : Malloc failure */

  FTM_RF_TEST_EC_NUM  /*!< Max : Defines maximum number of error codes */

} ftm_rf_test_error_code_enum_type;

/*! @} */ /* FTM_TEST_TOOLS_ERROR_CODE */

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_COMMAND Property IDs
  @{
*/

/*====================================================================================================================*/
/*!
  @name Radio Config Property ID

  @brief
  Radio Config Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM Radio Config command. These enumeration are used to define 
the content of #ftm_properties_framework_field_property_type in FTM test command packet. 
radio_config_property_names Must be updated when this list is updated.*/ 
typedef enum
{
  FTM_RF_TEST_RADIO_CFG_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RF_TEST_RADIO_CFG_PROP_RX_CARRIER = 1, /*!< 1 : Receiver Carrier Number */

  FTM_RF_TEST_RADIO_CFG_PROP_TX_CARRIER = 2, /*!< 2 : Transmit Carrier Number */

  FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE_PRI = 3, /*!< 3 : Primary RFM device */

  FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE_DIV = 4, /*!< 4 : Diversity RFM device */

  FTM_RF_TEST_RADIO_CFG_PROP_BAND = 5, /*!< 5 : Operating Tech Band */

  FTM_RF_TEST_RADIO_CFG_PROP_CHANNEL = 6, /*!< 6 : Operating Tech channel */

  FTM_RF_TEST_RADIO_CFG_PROP_BANDWIDTH = 7,  /*!< 7 : Radio Bandwidth */

  FTM_RF_TEST_RADIO_CFG_PROP_RX_MODE= 8,          /*!< 8 : For backwards compatibility--Rx Mode: Burst or Continous*/
  FTM_RF_TEST_RADIO_CFG_PROP_CONTINUOUS_MODE= 8,  /*!< 8 : Continuous Mode: Burst or Continous*/

  FTM_RF_TEST_RADIO_CFG_PROP_SIG_PATH = 9,  /*!< 9 : Signal Path for AT forward */

  FTM_RF_TEST_RADIO_CFG_PROP_ANT_PATH = 10,  /*!< 10 : Antenna Path */

  FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_USER_ADJUST = 11,    /*!< 11 : Frequency adjust: User adjustment in ppb */

  FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_CENTER_FREQ = 12,    /*!< 12 : Frequency adjust: Center frequency in Hz */

  FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_ENABLE_XO = 13,    /*!< 13 : Frequency adjust: Enable XO */

  FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_TOTAL_ADJUST = 14,    /*!< 14 : Frequency adjust: Total adjustment made in Hz */

  FTM_RF_TEST_RADIO_CFG_PROP_BURST_PATTERN = 15,    /*!< 15 : Burst pattern of Rx and Tx slots (for GSM and TDS) */

  FTM_RF_TEST_RADIO_CFG_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_radio_config_property_type;
      
extern const char *radio_config_property_names[];
      
/*! @} */
      
/*====================================================================================================================*/
/*!
  @name Rx measure Property ID

  @brief
  Rx measure Property ID list
*/
/*! @{ */
      
/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM Rx Measure Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet. 
rx_measure_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_RF_MEASURE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_RX_MEASURE_PROP_RX_CARRIER = 1, /*!< 1 : Receiver Carrier Number */

  FTM_RX_MEASURE_PROP_RFM_DEVICE = 2, /*!< 2 : RFM device for this measurement*/

  FTM_RX_MEASURE_PROP_EXPECTED_AGC = 3, /*!< 3 : Expected AGC Value */

  FTM_RX_MEASURE_PROP_AGC = 4, /*!< 4 : RxAGC value */

  FTM_RX_MEASURE_PROP_LNA_GAIN_STATE = 5, /*!< 5 : LNA Gain State */

  FTM_RX_MEASURE_PROP_SIG_PATH = 6,  /*!< 6 : Signal Path for AT forward */  

  FTM_RX_MEASURE_PROP_RX_MODE = 7,  /*!< 7 : GSM Rx Mode: Burst or Continous*/

  FTM_RX_MEASURE_PROP_RX_SLOT = 8,  /*!< 8 : GSM Rx Slot Number */

  FTM_RX_MEASURE_PROP_NUM_OF_BURST = 9,  /*!< 9 : GSM Number of Burst */

  FTM_RX_MEASURE_PROP_SENSITIVITY = 10,  /*!< 10 : Sensitivity*/

  FTM_RX_MEASURE_PROP_CTON = 11,  /*!< 11 : CtoN Measurement */

  FTM_RX_MEASURE_PROP_PEAK_FREQ = 12,  /*!< 12 : Peak Frequency Measurement*/
  
  FTM_RX_MEASURE_PROP_FETCH_RAW_IQ_SAMPLES = 13, /*!< 13 : Raw IQ Samples Capture for Rx Measurement*/

  FTM_RX_MEASURE_PROP_NUM_OF_SAMPLES = 14,  /*!< 14 : Number of IQ samples for Rx Measurement*/

  FTM_RX_MEASURE_PROP_IQ_DATA_FORMAT = 15,  /*!< 15 : Format of IQ samples for Rx Measurement*/

  FTM_RX_MEASURE_PROP_SAMP_FREQ = 16,  /*!< 16 : Sampling Frequency in unit of hz for Rx Measurement*/

  FTM_RX_MEASURE_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_rx_measure_property_type;

/*! Enumeration to indicate the type of the IQ capture data format */
typedef enum
{
  FTM_RF_TEST_IQ_FORMAT_8SC    = 0, /*!< 0 : 8SC IQ format  */ 
  FTM_RF_TEST_IQ_FORMAT_16SC   = 1, /*!< 1 : 16SC IQ format  */ 
  FTM_RF_TEST_IQ_FORMAT_MAX         /*!< Max */

} ftm_rf_test_iq_capture_format;

extern const char *rx_measure_property_names[];

/*! @} */

/*====================================================================================================================*/
/*!
  @name Tx Control Property ID

  @brief
  Tx Control Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM TX CONTROL Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet
tx_control_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_TX_CONTROL_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_TX_CONTROL_PROP_TX_CARRIER = 1, /*!< 1 : Receiver Carrier Number */

  FTM_TX_CONTROL_PROP_RFM_DEVICE = 2, /*!< 2 : RFM device for this measurement*/

  FTM_TX_CONTROL_PROP_TX_ACTION = 3, /*!< 3 : TX ACTION ENABLE/DISABLE */

  FTM_TX_CONTROL_PROP_TX_POWER = 4, /*!< 4 : Transimit Power */

  FTM_TX_CONTROL_PROP_RB_CONFIG = 5, /*!< 5 : LTE Start RB */

  FTM_TX_CONTROL_PROP_SIG_PATH = 6,  /*!< 6 : Signal Path for AT forward */

  FTM_TX_CONTROL_PROP_NUM_OF_RB = 7,  /*!< 7 : LTE Number of RBs */

  FTM_TX_CONTROL_PROP_TX_WAVEFORM = 8,  /*!< 8 : LTE & GSM Tx waveform */

  FTM_TX_CONTROL_PROP_NETWORK_SIGNAL = 9,  /*!< 9 : LTE Network signalling value */

  FTM_TX_CONTROL_PROP_TX_SLOT = 10,  /*!< 10 : GSM TX SLOT Number */

  FTM_TX_CONTROL_PROP_MODULATION_TYPE = 11,  /*!< 11 : LTE Modulation Type */

  FTM_TX_CONTROL_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_tx_control_property_type;

extern const char *tx_control_property_names[];

/*! @} */

/*====================================================================================================================*/
/*!
  @name Recommended Radio Allocations Property ID

  @brief
  Recommended Radio Allocations Property ID list
*/
/*! @{ */
      
/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Recommended Radio Allocations Command. 
  These enumeration are used to define the content of test command packet. 
  ftm_trm_rra_property_names[] Must be updated when this list is updated.*/
typedef enum
{
  FTM_TRM_RRA_PROP_UNASSIGNED          = 0,  /*!< Unassigned property */
  FTM_TRM_RRA_PROP_SUB_IDX             = 1,  /*!< Subscription Index from ftm_rf_test_subscriber_enum_type */
  FTM_TRM_RRA_PROP_TECH                = 2,  /*!< Tech from ftm_rf_technology_type values */
  FTM_TRM_RRA_PROP_BAND                = 3,  /*!< Band from sys_band_class_e_type values */
  FTM_TRM_RRA_PROP_CHAN                = 4,  /*!< Chan */
  FTM_TRM_RRA_PROP_BANDWIDTH           = 5,  /*!< Bandwidth in Hz */
  FTM_TRM_RRA_PROP_RXTX                = 6,  /*!< Indicates if request is for Rx or Tx path from ftm_rf_test_device_radio_trx_t*/
  FTM_TRM_RRA_PROP_CHAIN               = 7,  /*!< Chain 0/1/2/3/n */
  FTM_TRM_RRA_PROP_CARRIER_IDX         = 8,  /*!< Carrier Index 0/1/2/n */
  FTM_TRM_RRA_PROP_RESOURCE            = 9,  /*!< Resource from ftm_trm_resource_type values */
  FTM_TRM_RRA_PROP_REASON              = 10, /*!< Resource Reason from ftm_trm_reason_type values */
  FTM_TRM_RRA_PROP_PATH_FUNCTIONALITY  = 11, /*!< Path Functionality from ftm_trm_path_functionality_type values */
  FTM_TRM_RRA_PROP_DEVICE              = 12, /*!< Device from rfm_device_enum_type values */
  FTM_TRM_RRA_PROP_SIGNAL_PATH         = 13, /*!< Signal */
  FTM_TRM_RRA_PROP_ANTENNA_PATH        = 14, /*!< Antenna Path */

  /* ADD MORE ITEMS ABOVE THIS LINE */
  FTM_TRM_RRA_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_trm_rra_property_type;

extern const char *ftm_trm_rra_property_names[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM MSIN CONFIG Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet. 
msim_config_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_MSIM_CONFIG_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_MSIM_CONFIG_PROP_SUBSCRIBER = 1, /*!< 1 : FTM Subscriber */

  FTM_MSIM_CONFIG_PROP_TECH = 2,       /*!< 2 : SUB FTM RF TECHNOLOGY */

  FTM_MSIM_CONFIG_PROP_SCENARIO = 3,   /*!< 3 : SUB SCENARIO */

  FTM_MSIM_CONFIG_PROP_NUM             /*!< Max : Defines maximum number of properties */

} ftm_rf_test_msim_config_property_type;

extern const char *msim_config_property_names[];

/*! @} */



/*====================================================================================================================*/
/*!
  @name IQ Capture Property ID

  @brief
  IQ Capture Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM IQ Capture Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet
tx_control_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_IQ_CAPTURE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_IQ_CAPTURE_PROP_SUBSCRIBER = 1, /*!< 1 : Subscriber Number */

  FTM_IQ_CAPTURE_PROP_TECHNOLOGY = 2, /*!< 2 : Technology */

  FTM_IQ_CAPTURE_PROP_TX_CARRIER = 3, /*!< 3 : Transmit Carrier Number */

  FTM_IQ_CAPTURE_PROP_RX_CARRIER = 4, /*!< 4 : Receiver Carrier Number */

  FTM_IQ_CAPTURE_PROP_RFM_DEVICE = 5, /*!< 5 : RFM device for this measurement*/

  FTM_IQ_CAPTURE_PROP_SIG_PATH = 6,  /*!< 6 : Signal Path */

  FTM_IQ_CAPTURE_PROP_ACTION_GET_CFG = 7,   /*!< 7 : IQ capture action, get config */
  FTM_IQ_CAPTURE_PROP_ACTION_ACQUIRE = 8,   /*!< 8 : IQ capture action, acquire samples */
  FTM_IQ_CAPTURE_PROP_ACTION_FETCH = 9,     /*!< 9 : IQ capture action, retrieve samples */
  FTM_IQ_CAPTURE_PROP_ACTION_EST_SENS = 10, /*!< 10 : IQ capture action, retrieve estimated sensitivity */
  FTM_IQ_CAPTURE_PROP_ACTION_CTON = 11,     /*!< 11 : IQ capture action, retrieve CtoN */
  FTM_IQ_CAPTURE_PROP_ACTION_PEAK_FREQ = 12,/*!< 12 : IQ capture action, retrieve CtoN */

  FTM_IQ_CAPTURE_PROP_IQ_CAPTURE_SOURCE = 13, /*!< 13 : IQ capture source */

  FTM_IQ_CAPTURE_PROP_SAMPLE_SIZE = 14, /*!< 14 : Sample size */

  FTM_IQ_CAPTURE_PROP_IQ_DATA_FORMAT = 15, /*!< 15 : IQ data format */

  FTM_IQ_CAPTURE_PROP_SAMP_FREQ = 16, /*!< 16 : Sampling frequency */

  FTM_IQ_CAPTURE_PROP_MAX_DIAG_SIZE = 17, /*!< 17 : Maximum support diag size  */

  FTM_IQ_CAPTURE_PROP_SAMPLE_OFFSET = 18, /*!< 18 : Offset of bytes returned */

  FTM_IQ_CAPTURE_PROP_NUM_SAMPLE_BYTES = 19, /*!< 19 : Number of sample bytes returned */

  FTM_IQ_CAPTURE_PROP_EST_SENSITIVITY = 20, /*!< 20 : Estimated sensitivity */

  FTM_IQ_CAPTURE_PROP_CTON = 21, /*!< 21 : Calculated C/N */

  FTM_IQ_CAPTURE_PROP_CW_PEAK_FREQ = 22, /*!< 22 : CW peak frequency */

  FTM_IQ_CAPTURE_PROP_NUM,  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_iq_capture_property_type;

extern const char *iq_capture_property_names[];
       

/*! @} */



/*! @} */ /* FTM_TEST_TOOLS_CMD_IQ_CAPTURE */

/*====================================================================================================================*/
/*!
  @name TX Measure Property ID

  @brief
  TX Measure Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM TX MEASURE Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet
tx_measure_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_TX_MEASURE_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */

  FTM_TX_MEASURE_PROP_NUM_AVERAGES = 1, /*!< 1 : Number of Averages */

  FTM_TX_MEASURE_PROP_TX_CARRIER = 2, /*!< 2 : TX carrier for this measurement*/

  FTM_TX_MEASURE_PROP_TX_DEVICE = 3, /*!< 3 : TX device for this measurement */

  FTM_TX_MEASURE_ACTION_TX_POWER = 4, /*!< 4 : Action: Measure Transimit Power */

  FTM_TX_MEASURE_ACTION_EVM = 5, /*!< 5 : Action - Measure EVM */

  FTM_TX_MEASURE_ACTION_ACLR = 6,  /*!< 6 : Action - Measure ACLR */

  FTM_TX_MEASURE_ACTION_VSWR = 7,  /*!< 7 : Action - Measure VSWR */

  FTM_TX_MEASURE_ACTION_DEBUG_DATA = 8,  /*!< 8 : Action - Generate debug data in blob property format */

  FTM_TX_MEASURE_ACTION_PHASE_ERROR = 9,  /*!< 9 : Action - Measure Phase Error */

  FTM_TX_MEASURE_ACTION_SPECTRUM_FLATNESS = 10,  /*!< 10 : Action - Measure Spectrum Flatness */

  FTM_TX_MEASURE_TX_POWER = 11,  /*!< 11 : Response - TX power measurement */

  FTM_TX_MEASURE_EVM = 12,  /*!< 12 : Response - EVM measurement */

  FTM_TX_MEASURE_ACLR_UTRA1_POS = 13, /*!< 13 : Response - ACLR UTRA1 POS Report */

  FTM_TX_MEASURE_ACLR_UTRA1_NEG = 14, /*!< 14 : Response - ACLR UTRA1 NEG Report */

  FTM_TX_MEASURE_ACLR_UTRA2_POS = 15, /*!< 15 : Response - ACLR UTRA2 POS Report */

  FTM_TX_MEASURE_ACLR_UTRA2_NEG = 16, /*!< 16 : Response - ACLR UTRA2 NEG Report */

  FTM_TX_MEASURE_ACLR_EUTRA_POS = 17, /*!< 17 : Response - ACLR EUTRA POS Report */

  FTM_TX_MEASURE_ACLR_EUTRA_NEG = 18, /*!< 18 : Response - ACLR EUTRA POS Report */

  FTM_TX_MEASURE_VSWR_RATIO = 19, /*!< 19 : Response - VSWR ratio Report */

  FTM_TX_MEASURE_VSWR_RETURN_LOSS = 20, /*!< 20 : Response - VSWR return loss Report */

  FTM_TX_MEASURE_VSWR_S11_MAG = 21, /*!< 21 : Response - VSWR S11 magnitude Report */

  FTM_TX_MEASURE_VSWR_S11_PHASE = 22, /*!< 22 : Response - VSWR S11 phase Report */

  FTM_TX_MEASURE_VSWR_FWD_POWER = 23, /*!< 23 : Response - VSWR forward power Report */

  FTM_TX_MEASURE_VSWR_REV_POWER = 24, /*!< 24 : Response - VSWR reverse power Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R1_RIPPLE = 25, /*!< 25 : Response - Spectrum Flatness R1 Ripple Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R2_RIPPLE = 26, /*!< 26 : Response - Spectrum Flatness R2 Ripple Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R12_RIPPLE = 27, /*!< 27 : Response - Spectrum Flatness R12 Ripple Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R21_RIPPLE = 28, /*!< 28 : Response - Spectrum Flatness R21 Ripple Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R1_RIPPLE_EXTREME = 29, /*!< 29 : Response - Spectrum Flatness R1 Ripple Extreme Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R2_RIPPLE_EXTREME = 30, /*!< 30 : Response - Spectrum Flatness R2 Ripple Extreme Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R12_RIPPLE_EXTREME = 31, /*!< 31 : Response - Spectrum Flatness R12 Ripple Extreme Report */

  FTM_TX_MEASURE_SPECTRUM_FLATNESS_R21_RIPPLE_EXTREME = 32, /*!< 32 : Response - Spectrum Flatness R21 Ripple Extreme Report */

  FTM_TX_MEASURE_PHASE_ERROR = 33, /*!< 33 : Response - Phase error (GSM) */

  FTM_TX_MEASURE_ACTION_TX_TOTAL = 34, /*!< 34 : Action - Measure total TX power */

  FTM_TX_MEASURE_TOTAL_TX_POWER = 35, /*!< 35 : Response - Total TX power Report */

  FTM_TX_MEASURE_DEBUG_DATA = 36, /*!< 36 : Response - Debug data in blob property format */

  FTM_TX_MEASURE_ACTION_ORFS_MODULATION = 37, /*!< 37 : Action - ORFS Modulation (GSM) */

  FTM_TX_MEASURE_ACTION_ORFS_SWITCHING = 38, /*!< 38 : Action - ORFS Switching (GSM) */

  FTM_TX_MEASURE_RESERVED6= 39, /*!< 39 : Reserved */

  FTM_TX_MEASURE_RESERVED1 = 40, /*!< 40 : Reserved */

  FTM_TX_MEASURE_RESERVED2 = 41, /*!< 41 : Reserved */

  FTM_TX_MEASURE_PROP_TECH = 42, /*!< 42 : Current Tech */

  FTM_TX_MEASURE_PROP_SUBSCRIBER = 43, /*!< 43 : Subscriber */

  FTM_TX_MEASURE_ORFS_MODULATION_400KHZ_POS = 44, /*!< 44 : Response - ORFS Modulation (GSM) for +400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_400KHZ_NEG = 45, /*!< 45 : Response - ORFS Modulation (GSM) for -400KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_600KHZ_POS = 46, /*!< 46 : Response - ORFS Modulation (GSM) for +600KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_600KHZ_NEG = 47, /*!< 47 : Response - ORFS Modulation (GSM) for -600KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1200KHZ_POS = 48, /*!< 48 : Response - ORFS Modulation (GSM) for +1200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1200KHZ_NEG = 49, /*!< 49 : Response - ORFS Modulation (GSM) for -1200KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1800KHZ_POS = 50, /*!< 50 : Response - ORFS Modulation (GSM) for +1800KHz */

  FTM_TX_MEASURE_ORFS_MODULATION_1800KHZ_NEG = 51, /*!< 51 : Response - ORFS Modulation (GSM) for -1800KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_100KHZ_POS = 52, /*!< 52 : Response - ORFS Switching (GSM) for +100KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_100KHZ_NEG = 53, /*!< 53 : Response - ORFS Switching (GSM) for -100KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_200KHZ_POS = 54, /*!< 54 : Response - ORFS Switching (GSM) for +200KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_200KHZ_NEG = 55, /*!< 55 : Response - ORFS Switching (GSM) for -200KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_250KHZ_POS = 56, /*!< 56 : Response - ORFS Switching (GSM) for +250KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_250KHZ_NEG = 57, /*!< 57 : Response - ORFS Switching (GSM) for -250KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_400KHZ_POS = 58, /*!< 58 : Response - ORFS Switching (GSM) for +400KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_400KHZ_NEG = 59, /*!< 59 : Response - ORFS Switching (GSM) for -400KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_600KHZ_POS = 60, /*!< 60 : Response - ORFS Switching (GSM) for +600KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_600KHZ_NEG = 61, /*!< 61 : Response - ORFS Switching (GSM) for -600KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1800KHZ_POS = 62, /*!< 62 : Response - ORFS Switching (GSM) for +1800KHz */

  FTM_TX_MEASURE_ORFS_SWITCHING_1800KHZ_NEG = 63, /*!< 63 : Response - ORFS Switching (GSM) for -1800KHz */

  FTM_TX_MEASURE_PROP_NUM  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_tx_measure_property_type;

extern const char *tx_measure_property_names[];

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for FTM IRAT CONFIG Command. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet. 
irat_config_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_IRAT_CONFIG_PROP_UNASSIGNED = 0,    /*!< 0 : Unassigned property */ 

  FTM_IRAT_CONFIG_PROP_TGT_ACTION = 1,    /*!< 1 : iRAT Action - Setup/Tune Away/Tune Back */

  FTM_IRAT_CONFIG_PROP_TGT_SUB = 2,       /*!< 2 : Target Subscription */

  FTM_IRAT_CONFIG_PROP_TGT_TECH = 3 ,     /*!< 3 : Target Tech */

  FTM_IRAT_CONFIG_PROP_TGT_CARRIER = 4,   /*!< 4 : Target Carrier */

  FTM_IRAT_CONFIG_PROP_TGT_DEVICE = 5,    /*!< 5 : Target Device */

  FTM_IRAT_CONFIG_PROP_TGT_SIG_PATH = 6,  /*!< 6 : Target Sig Path */

  FTM_IRAT_CONFIG_PROP_TGT_ANT_PATH = 7,  /*!< 6 : Target Ant Path */

  FTM_IRAT_CONFIG_PROP_TGT_BAND = 8,      /*!< 6 : Target Band */

  FTM_IRAT_CONFIG_PROP_TGT_CHANNEL = 9,   /*!< 6 : Target Channel */

  FTM_IRAT_CONFIG_PROP_TGT_BANDWIDTH = 10,/*!< 10 : Target Band Width */

  FTM_IRAT_CONFIG_PROP_NUM

} ftm_rf_test_irat_config_property_type;

extern const char *irat_config_property_names[];

/*====================================================================================================================*/
/*!
  @name Wait Trigger Property IDs

  @brief
  Wait Trigger Property ID list
*/
/*! @{ */

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of properties for Wait Trigger Subcommand. 
  These enumeration are used to define the content of #ftm_properties_framework_field_property_type in FTM test command packet
  wait_trigger_property_names Must be updated when this list is updated.*/
typedef enum
{
  FTM_WAIT_TRIGGER_PROP_UNASSIGNED = 0, /*!< 0 : Unassigned property */
  FTM_WAIT_TRIGGER_PROP_SUBSCRIBER = 1, /*!< 1 : Subscriber Number */
  FTM_WAIT_TRIGGER_PROP_TECHNOLOGY = 2, /*!< 2 : Technology */
  FTM_WAIT_TRIGGER_PROP_TX_CARRIER = 3, /*!< 3 : Transmit Carrier Number */
  FTM_WAIT_TRIGGER_PROP_RX_CARRIER = 4, /*!< 4 : Receiver Carrier Number */
  FTM_WAIT_TRIGGER_PROP_RFM_DEVICE = 5, /*!< 5 : RFM device for this measurement*/
  
  FTM_WAIT_TRIGGER_PROP_RXWAITDBM_THRESHOLD  = 6, /*!< 6 : Rx Wait dBm - Threshold (dBm10) */
  FTM_WAIT_TRIGGER_PROP_RXWAITDBM_DIRECTION  = 7, /*!< 7 : Rx Wait dBm - Direction (positive/negative) */
  FTM_WAIT_TRIGGER_PROP_RXWAITDBM_TIMEOUT    = 8, /*!< 8 : Rx Wait dBm - Timeout (us) */
  FTM_WAIT_TRIGGER_PROP_RXWAITDBM_RXAGC      = 9, /*!< 8 : Rx Wait dBm - Measured RxAGC (dBm10) */

  FTM_WAIT_TRIGGER_PROP_NUM,  /*!< Max : Defines maximum number of properties */

} ftm_rf_test_wait_trigger_property_type;

extern const char *wait_trigger_property_names[];
       

/*! @} */



/*! @} */

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FTM_RF_TEST_INTERFACE_H */

