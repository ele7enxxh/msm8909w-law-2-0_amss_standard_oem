#ifndef RFDEVICE_CMN_TYPEDEF_H
#define RFDEVICE_CMN_TYPEDEF_H

/*!
  @file
  rfdevice_cmn_typedef.h

  @brief
  This file provides common interface functions to configure the RTR/WTR device.
*/

/*==============================================================================

  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_cmn_type_defs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/13/15   sd      Change PAR reading to 16bits supporting db100 format
10/06/15   sd      Added support for AutoPin PAR
09/21/15   bm      Added offset_valid flag to tx_power_offset_bw_type structure
08/03/15   sma     Added ULCA 20+20 Tx LUT power offset
07/24/15   ry      Added phase to rfdevice_tx_lut_data_type
03/04/14   vv      Added more lna gain states to support GRFC eLNA
12/03/14   vv      Added lna gain state and trigger type enum support
10/02/14   dps     Changed lna_gain_state to gain_state in rfdevice_rsb_cal_data_type
09/03/14   rsr     Added  TX LUT ID in TX LUT info.
09/03/14   lm      Added rfdevice_reg_dump_cmd_type enum
08/14/14   shb     Updated fbrx_setup/disable APIs to support slow_ramp ON and 
                   OFF sequence for LDO 26
08/05/14   sma     Added lna gain state for WCDMA RX RSB Calibration
08/05/14   sd      Added suppport for ILPC
07/17/14   aks     Added enum to indicate the max number of notches which can be 
                   returned from WTR
06/19/14   vv      Added support for physical device based sleep/wakeup voting
06/12/14   hs      Added suppport for JDET.
05/09/14   hm      Added RSB Coefficents Alpha and Beta to rfdevice_tx_lut_data_type
05/05/14   sn      Added ASM enum for trigger, cell, rxtx
05/05/14   sd      Support for Legacy drivers
04/29/14   sn      Added PA trigger enum
02/25/14   aa      Merged device power on/off sequence retaining SSBI contents
02/24/14   aa      DimePM compiler error fix
02/14/14   shb     Added API to retrieve FBRX RSB coefficients
11/11/13   tks     Added enum to diff startup and cleanup meas scenario
11/08/13   hm      Added enum to distinguish Standalone/IRAT(Meas) mode
10/26/13   shb     Updated IRAT APIs to support preload
09/30/13   shb     Added new common TXAGC APIs
07/07/13   sbo     Added BW support for PA device
05/20/13   kai     Added new feedback path attenuation state enum
04/25/13   shb     Removed special variables for pdet dc cal - will be handled
                   by WTR1625 internally.
04/16/13   shb     Added rfdevice_tx_saw_port_type
04/01/13   adk     Fixed off-target compilation error
03/21/13   shb     Updated to support PDET DC cal for WTR1625
12/03/12   aca     DIME Dac cal
09/18/12   shb     Added internal device cal to function pointer table
08/16/12   shb     Renamed rfdevice_enum_type to rfdevice_id_enum_type
08/07/12   shb     Removed SHIM layer functions from rfdevice_cmn_typedef.h.
                   This file contains data and func table types used by device
08/07/12   shb     Branched rfdevice_cmn_interface.h to rfdevice_cmn_typedef.h
                   in rfdevice/interface/api.
08/01/12   shb     Added rfdevice_rfc_data_version_type
07/16/12   shb     Deleted duplicate rf_device_execution_type
07/10/12   vss     Add function ptr for retrieving device type.
07/09/12   aak     Move Quiet mode API from device to RFC
07/02/12   sn      Added rf_device_execution_type
06/21/12   vb      Added support for QFE1510
06/20/12   shb     Added rf_device_execution_type
06/07/12   shb     Added rfdevice_dpd_enum_type
04/11/12   shb     Added support for shadow backup/restore
03/26/12   vb      Added support for new RF device function to program DAC Iref
03/15/12   shb     Added arg to return chip process during device creation
02/24/12   shb     Created internal dev cal struct to handle all int device cal
                   and a HDET autocal status type
02/16/12   pv      RF Common Core Device Manager Implementation.
02/14/12   shb     Removed process/version check interface
01/20/12   tks     Added support for WTR1605 process/version check
01/20/12   shb     Created generic rfdevice_script_type that can be used by all
                   tech APIs that need to return a SSBI script
01/16/12   aro     PA state as an argument to enable ET path function
01/13/12   aro     Added support for ET path enable for self calibration
01/13/12   aro     Documentation update
11/21/11   vb      Added RFDEVICE_IREF_INVALID in rfdevice_dac_iref_val
11/17/11   aak     Changes to implement Quiet mode
11/07/11   aak     Changes to implement Quiet mode
11/02/11   vb      DAC cal should not be performed for Iref less than 90uA,
                   hence reducing the number of DAC Cal points.
10/20/11   shb     Added rfdevice_ssbi_write_type for WTR1605 scripting support
09/27/11   aro     Added interface to load RC tuner calibration data
09/25/11   vb      Support for DAC cal using different Irefs
09/19/11   aro     Changed the Internal calibration error code definition
09/18/11   aro     Updated RC tuner interface to pass generic RC tuner
                   calibration data to remove RTR8600 specific interface
09/14/11   aro     Added error codes for internal calibration
09/14/11   aro     Updated RC tuner calibration interface to include pointer
                   to cal data as function argument
09/14/11   aro     Added doxygen documentation for RC tuner calibration intf
09/14/11   aro     Added internal calibration status enumeration
09/14/11   aro     Added documentation to make the file doxygen compatible
09/14/11   aro     Deleted unused variables
09/14/11   aro     Added Common device interface to perform RC tuner
                   calibration
09/13/11   aro     Updated common interface to have common function table
                   and common device data
09/13/11   aro     Updated the file with doxygen documentation
07/11/11   shb     Updated constness of rfdevice_cmn_type members for
                   robustness
09/22/10   shb     Added pointer to common data in common device structure
09/22/10   shb     Make common device struct rfdevice_cmn an array of pointers
                   of max supported paths size
09/22/10   shb     Added rfdevice_cmn_cmd_dispatch()
09/22/10   shb     Make common command dispatch function ptr accept a ptr to
                   rfdevice_cmn_type to access global data
08/13/10   vb      Changed datatype of rfdevice_cmn_type() for merging RTR9K device
07/27/10   shb     rfdevice_cmd_dispatch_type definition added
07/01/10   av      Initial version.

==============================================================================*/

#include "rfcom.h"
#include "rfdevice_type_defs.h"
#include "rfdevice_intf_cmd.h"
#include "rfcommon_locks.h"
#include "rfm_gain_state_types.h"
#include "rf_buffer_intf.h"

/* Enable Quiet Mode if needed from NV */
#define RFDEVICE_ENABLE_V2_QUIET_MODE   20
#define RFDEVICE_ENABLE_V3_QUIET_MODE   30

/*! Maximum number of SSBI writes possible/supported in a device script */
#define RFDEVICE_MAX_SSBI_SCRIPT_SIZE     600

/*! Maximum number of SSBI writes possible/supported in a QFE script */
#define QFE_MAX_SSBI_SCRIPT_SIZE          30

/*! Maximum number of notches which can be returned from WTR */
#define RFDEVICE_MAX_NOTCHES 12
/*! Sleep and wakeup vote for physical devices*/
#define WAKEUP_VOTE 1
#define SLEEP_VOTE 0


/*!Default temperature value for QFE thermistors*/
/*the invalid temperature value is equivalent to -273 degree celcius (0 kelvin)*/
#define RFDEVICE_INVALID_TEMPERATURE 0xFEEF
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum types for different register dump
 
  @details
  Enum to distinguish which register users want to dump
 */
typedef enum
{
  /*! dump critical register */
  RFDEVICE_CRITICAL_REG_DUMP = 0,
  /*! dump all registers*/
  RFDEVICE_ALL_REG_DUMP = 1,
}rfdevice_reg_dump_cmd_type;

/*----------------------------------------------------------------------------*/
/*! Used to identify version of RFC data */
typedef enum
{
  /*! Version 1 of RFC data */
  RFDEVICE_RFC_DATA_V1,

  /*! Total number of RFC data versions supported by device */
  RFDEVICE_RFC_DATA_NUM_VERSIONS,

  /*! Invalid version */
  RFDEVICE_RFC_DATA_INVALID_VERSION = RFDEVICE_RFC_DATA_NUM_VERSIONS
} rfdevice_rfc_data_version_type;

/*----------------------------------------------------------------------------*/
/*! Used to differentiate between SAW and SAWLESS ports */
typedef enum
{
  /*! TX port does not use SAW filter */
  RFDEVICE_TX_SAWLESS_PORT,

  /*! TX port uses internal SAW filter */
  RFDEVICE_TX_SAW_PORT,

  /*! Max type of ports = 2, saw and sawless */
  RFDEVICE_TX_MAX_PORT,

  /*! Indicates invalid port */
  RFDEVICE_TX_INVALID_PORT = RFDEVICE_TX_MAX_PORT,

} rfdevice_tx_saw_port_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration for the status if internal calibration */
typedef enum
{

  RFDEVICE_INT_CAL_PASS = 1,
  /*!< Status indicating that the internal calibration was attempted and
  passed successfully. The calibration data populated in the data container
  is valid to be pushed to NV */

  RFDEVICE_INT_CAL_SKIPPED = 0,
  /*!< Status indicating that the internal calibration was not attempted because
  the data container (which came from NV) already has valid calibration data.
  Thus the calibration request is skipped to avoid redundancy. */

  RFDEVICE_INT_CAL_FAIL = -1,
  /*!< Status indicating that the internal calibration was attempted and
  failed. The calibration data populated in the data structure is invalid,
  and should not be pushed into NV. */

  RFDEVICE_INT_CAL_FAIL_INVALID_PARAM = -2,
  /*!< Status indicating Invalid Parameter for internal device calibration */

  RFDEVICE_INT_CAL_RC_PROC_ERR_RC_MEAS_TO = -3,
  /*!< RC Process Error Cal, RC Measurement Timeout */

  RFDEVICE_INT_CAL_RC_PROC_ERR_R_MEAS_TO = -4,
  /*!< RC Process Error Cal, V drop Measurement Timeout */

  RFDEVICE_INT_CAL_RC_PROC_ERR_VDROP_MEAS_ERR = -5,
  /*!< RC Process Error Cal, Voltage-Drop Measurement Error */

  RFDEVICE_INT_CAL_RC_PROC_ERR_TAU_MEAS_ERR = -6
  /*!< RC Process Error Cal, RC tau Measurement Error ( if <= 0 ) */

} rfdevice_internal_cal_status;

/*----------------------------------------------------------------------------*/
/*! Enumeration for the status of HDET autocal */
typedef enum
{
  RFDEVICE_HDET_AUTOCAL_PASS = 1,
  /*!< HDET autocal passed successfuly */

  RFDEVICE_HDET_AUTOCAL_SKIPPED = 0,
  /*!< HDET autocal skipped for some reason */

  RFDEVICE_HDET_AUTOCAL_FAILED = -1
  /*!< HDET autocal failed - cal data not updated */
} rfdevice_hdet_autocal_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Iref enumeration

  @details
  Enumeration for different Iref supported by RF device.
  Any Qualcomm RF device supports 6 different Iref currents

  <b>
     NOTE:
     Iref number for each chip would mean different Iref.
     For eg: RFDEVICE_IREF_0 would produce different Iref current for RTR8600 vs WTR1605
   </b>

*/
typedef enum
{
  RFDEVICE_IREF_0,
  /*RFDEVICE_IREF_0 is invalid for all Qualcomm RF devices*/

  RFDEVICE_IREF_1,

  RFDEVICE_IREF_2,

  /*! NOTE: For all the known Qualcomm devices; Iref2, Iref1 and Iref0 produce
  Iref currents lower than 90uA. DAC chip design team has adviced not to
  calibrate Iref currents lower than 90uA */
  RFDEVICE_DAC_CAL_IREF_MIN,

  RFDEVICE_IREF_3 = RFDEVICE_DAC_CAL_IREF_MIN,

  RFDEVICE_IREF_4,

  RFDEVICE_IREF_5,

  RFDEVICE_IREF_6,

  /*RFDEVICE_IREF_6 is max allowed Iref for all Qualcomm RF devices*/
  RFDEVICE_DAC_CAL_IREF_MAX = RFDEVICE_IREF_6,

  RFDEVICE_IREF_INVALID

} rfdevice_dac_iref_val;

typedef enum
{
  RFDEVICE_ENABLE_IREF,

  RFDEVICE_DISABLE_IREF,

  RFDEVICE_INVALID_CMD

} rfdevice_cmn_dac_cal_cmd_enum_type;

/*Total number of Irefs for which DAC cal need to be performed*/
#define RFDEVICE_DAC_CAL_IREF_TOTAL_NUM (RFDEVICE_DAC_CAL_IREF_MAX - RFDEVICE_DAC_CAL_IREF_MIN + 1)


typedef enum
{
  RFDEVICE_PA_BW_NOT_APPLICABLE = 0,
  RFDEVICE_PA_BW_1P4MHZ,
  RFDEVICE_PA_BW_3MHZ,
  RFDEVICE_PA_BW_5MHZ,
  RFDEVICE_PA_BW_10MHZ,
  RFDEVICE_PA_BW_15MHZ,
  RFDEVICE_PA_BW_20MHZ,
  RFDEVICE_PA_CDMA_BW_1X,
  RFDEVICE_PA_CDMA_BW_3X,
  RFDEVICE_PA_CDMA_BW_5X,
  RFDEVICE_PA_CDMA_BW_7X,
  RFDEVICE_PA_BW_INVALID,
} rfdevice_pa_bw_enum_type;

typedef enum
{
  RFDEVICE_PA_TRIGGER_NONE,
  RFDEVICE_PA_TRIGGER_WAKEUP,
  RFDEVICE_PA_TRIGGER_SLEEP,
  RFDEVICE_PA_TRIGGER_CONFIG,
  RFDEVICE_PA_TRIGGER_ON_OFF,
  RFDEVICE_PA_TRIGGER_TXAGC,
  RFDEVICE_PA_TRIGGER_ALL,
} rfdevice_pa_trigger_enum_type;


/*LNA Trigger Types*/
typedef enum
{
  RFDEVICE_LNA_TRIGGER_NONE,
  RFDEVICE_LNA_TRIGGER_WAKEUP,
  RFDEVICE_LNA_TRIGGER_SLEEP,
  RFDEVICE_LNA_TRIGGER_GAIN,
  RFDEVICE_LNA_TRIGGER_ALL,
} rfdevice_lna_trigger_enum_type;

/*LNA Gain States*/
typedef enum
{
  RFDEVICE_LNA_G0,
  RFDEVICE_LNA_G1,
  RFDEVICE_LNA_G2,
  RFDEVICE_LNA_G3,
  RFDEVICE_LNA_G4,
  RFDEVICE_LNA_G5,
  RFDEVICE_LNA_GAIN_MAX,
  RFDEVICE_LNA_GAIN_INVALID = RFDEVICE_LNA_GAIN_MAX,
} rfdevice_lna_gain_enum_type;

typedef enum
{
  RFDEVICE_ASM_RX_0,
  RFDEVICE_ASM_TX_0,
  RFDEVICE_ASM_RX_TX_MAX ,
  RFDEVICE_ASM_RX_TX_INVALID = RFDEVICE_ASM_RX_TX_MAX,
} rfdevice_asm_rx_tx_type;


typedef enum
{
  /*primary cell*/
  RFDEVICE_ASM_CELL_0,
  /*secondary cell 0*/
  RFDEVICE_ASM_CELL_1,
  RFDEVICE_ASM_CELL_2,
  RFDEVICE_ASM_CELL_3,
  RFDEVICE_ASM_CELL_4
} rfdevice_asm_cell_type;

typedef enum
{
  RFDEVICE_ASM_TRIGGER_NONE,
  RFDEVICE_ASM_TRIGGER_WAKEUP,
  RFDEVICE_ASM_TRIGGER_SLEEP,
  RFDEVICE_ASM_TRIGGER_ENABLE_RX,
  RFDEVICE_ASM_TRIGGER_ENABLE_TX,
  RFDEVICE_ASM_TRIGGER_DISABLE_RX,
  RFDEVICE_ASM_TRIGGER_DISABLE_TX,
  RFDEVICE_ASM_TRIGGER_ALL,
} rfdevice_asm_trigger_enum_type;


/*----------------------------------------------------------------------------*/
/*! Enumeration for the set of JDET output */
typedef enum
{
  RFDEVICE_CMN_JDET_SAMPLE_0,
  RFDEVICE_CMN_JDET_SAMPLE_1,
  RFDEVICE_CMN_JDET_SAMPLE_2,
  RFDEVICE_CMN_JDET_SAMPLE_3,
  RFDEVICE_CMN_JDET_SAMPLE_4,
  RFDEVICE_CMN_JDET_SAMPLE_5,
  RFDEVICE_CMN_JDET_SAMPLE_6,
  RFDEVICE_CMN_JDET_SAMPLE_7,
  RFDEVICE_CMN_JDET_SAMPLE_INVALID,

} rfdevice_cmn_jdet_sample_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold RC Tuner calibration data */
typedef struct
{

  uint8 *rc_process_errors;
  /*!< Byte pointer to the RF Process error Calibration data */

  uint16 number_of_bytes;
  /*!< Number of bytes of RC process errors in the location pointed to RC
  Process error pointer. */

} rfdevice_cmn_rc_tuner_cal_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold all internal device calibration data for a device */
typedef struct
{
  /*! RC tuner calibration data */
  rfdevice_cmn_rc_tuner_cal_data_type rc_tuner;

  /*! Return parameter indicating RC tuner calibration status */
  rfdevice_internal_cal_status rc_tuner_cal_status;

  /*! Pointer to memory where HDET autocal result will be stored */
  uint8* hdet_autocal;

  /*! Return parameter indicating HDET autocal status for this device */
  rfdevice_hdet_autocal_status_type hdet_autocal_status;

  /*! Chip Process read back from WTR1605 */
  uint8* process;

} rfdevice_cmn_int_dev_cal_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold all dac cal data for device */
typedef struct
{
  /*! Command for device handler */
  rfdevice_cmn_dac_cal_cmd_enum_type dac_cal_cmd;

} rfdevice_cmn_dac_cal_data_type;

/*----------------------------------------------------------------------------*/
/*! Enum to identify Digital Pre-distortion state in RF device APIs */
typedef enum
{
  /*! Indicates Digital Pre-distortion should not be enabled in device */
  RFDEVICE_DPD_DISABLED,

  /*! Indicates Digital Pre-distortion must be enabled in device */
  RFDEVICE_DPD_ENABLED,

  /*! Max num of DPD states */
  RFDEVICE_DPD_NUM,

  /*! Enum to specify invalid DPD state */
  RFDEVICE_DPD_INVALID
} rfdevice_dpd_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic SSBI write type usable by all RF devices
*/
typedef struct
{
  /*! Slave_ID or bus to write SSBI to */
  uint8 bus;

  /*! SSBI address to write to */
  uint8 address;

  /*! Data to write to given bus/address */
  uint8 data;

  /*! Delay in us to wait before sending this SSBI write */
  uint16 delay;
} rfdevice_ssbi_write_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This structure defines the structure of a RF device script
*/
typedef struct
{
  uint16 size;
  /*!< Actual number of SSBI entries in the script */

  rfdevice_ssbi_write_type  buffer[ RFDEVICE_MAX_SSBI_SCRIPT_SIZE ];
  /*!< Buffer for the device SSBI script */

} rfdevice_script_type;

//#ifdef FEATURE_RF_HAS_QTUNER
/*----------------------------------------------------------------------------*/
/*!
   @brief
   This structure defines the structure of a QFE device script
*/
typedef struct
{
  uint16 size;
  /*!< Actual number of SSBI entries in the script */

  rfdevice_ssbi_write_type  buffer[ QFE_MAX_SSBI_SCRIPT_SIZE ];
  /*!< Buffer for the device SSBI script */

} qfe_script_type;
//#endif /* FEATURE_RF_HAS_QTUNER */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Used by device APIs to distinguish between returning a script and executing
  internally inside device

  @details
  Several RF device APIs support both 1) returning a script 2) execute script
  internally. This enum can be accepted as an argument by device APIs to
  decide what to do.
*/
typedef enum
{
  /*! RF device will execute script internally immediately */
  RFDEVICE_EXECUTE_IMMEDIATE,

  /*! RF device will populate buffer passed in by caller and expects caller
  to execute it */
  RFDEVICE_CREATE_SCRIPT
} rf_device_execution_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Indicates mode of execution of scripts for measurement APIs

  @details
  Compared to rf_device_execution_type, this enum adds the option of skipping
  execution completely. This is needed for measurement APIs that need the
  option of skipping certain scripts
*/
typedef enum
{
  /*! RF Device should execute script internally immediately */
  RFDEVICE_MEAS_EXECUTE_IMMEDIATE,

  /*! RF Device will return script to caller by populating a buffer passed in */
  RFDEVICE_MEAS_CREATE_SCRIPT,

  /*! RF Device should skip script creating or generation completely */
  RFDEVICE_MEAS_SKIP_EXECUTION
} rfdevice_meas_execution_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  IRAT script type enumeration

  @details
  Enum to indicate the scenario for which the caller needs a measurement
  script. The script contents could vary between tune away to neighbor cell
  and tune back to home cell.
*/
typedef enum
{
  RFDEVICE_MEAS_STARTUP_SCRIPT = 0,
  /*!< Prepare script for tune away scenario */

  RFDEVICE_MEAS_CLEANUP_SCRIPT = 1,
  /*!< Prepare script for tune back scenario */

  RFDEVICE_MEAS_INVALID_SCRIPT = 2
  /*!< Invalid IRAT or IFREQ scenario */
} rfdevice_meas_scenario_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration used for communication to device whether it
is for IRAT or Standalone mode of operation */
typedef enum
{
  /*! Current operation is MEAS or IRAT measurement. */
  RFDEVICE_OPER_MODE_MEAS,

  /*! Current operation is in Standalone mode */
  RFDEVICE_OPER_MODE_STANDALONE
} rfdevice_operation_mode;

/*! Enum to differentiate between FBRX scenarios of operation - different 
  setup/disable scripts have to be used based on state of FBRx LDO due
  to its effects on GPS. */
typedef enum
{
  /*! Indicates that FBRx is in a state where its LDO needs to be left 
    ON all the time - whether FBRx is in operation or disabled. This
    is needed when GPS is in operation to avoid interference due to LDO
    toggling */
  RFDEVICE_FBRX_LDO_ALWAYS_ON,

  /*! Indicates that the LDO needs to go from OFF to ON state. This applies 
    only for FBRX setup script. This scenario is not applicable for FBRx
    disable script as LDO should always be in ON state while FBRx is enabled */
  RFDEVICE_FBRX_LDO_OFF_TO_ON,

  /*! Indicates that FBRx needs to be from ON to OFF state. This applies 
    only for FBRX disable script. This scenario is not applicable for FBRx
    setup script as LDO should always be in ON state while FBRx is enabled*/
  RFDEVICE_FBRX_LDO_ON_TO_OFF,

  /*! To maintain size of enum */
  RFDEVICE_FBRX_NUM_SCENARIOS,

  /*! Invalid entry for initialization */
  RFDEVICE_FBRX_INVALID_SCENARIO = RFDEVICE_FBRX_NUM_SCENARIOS

} rfdevice_fbrx_scenario_type;

/*----------------------------------------------------------------------------*/
/*!
  Structure containing all the data needed to prepare device scripts for IRAT
  measurement
*/
typedef struct
{
  /*! Indicates action requested for preload script - caller may want to
    device to execute preload script immediately, return a script or skip
    preload */
  rfdevice_meas_execution_type preload_action;

  /*! Ptr to rf script buffer where preload script can be populated */
  rf_buffer_intf* preload_buf;

  /*! Indicates action requested for measurement script - caller may want to
    device to execute measurement script immediately, return a script or skip
    it. Measurement script must be executed within the gap */
  rfdevice_meas_execution_type meas_action;

  /*! Ptr to rf script buffer where measurement script can be populated */
  rf_buffer_intf* meas_buf;

  uint16 buffer_mask;
  /*!< mask to determine what segments of memory are being used */
} rfdevice_meas_script_data_type;

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Forward declaration of device structure types to use in function pointers
  definitions
*/
typedef struct rfcmn_device rfdevice_cmn_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to execute routine to perform RF Tuner calibration
  for the given instance of RTR driver
*/
typedef rfdevice_internal_cal_status (*rfdevice_cmn_do_rc_tuner_cal_fp_type)
(
  rfdevice_cmn_type *cmn_dev,
  rfdevice_cmn_rc_tuner_cal_data_type cal_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to execute routine to perform RF Tuner calibration
  for the given instance of RTR driver
*/
typedef boolean (*rfdevice_cmn_rc_tuner_load_cal_fp_type)
(
  rfdevice_cmn_type *cmn_dev,
  rfdevice_cmn_rc_tuner_cal_data_type cal_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to which either programs or gets the 32 gain word from the RF device
  to output the DAC Iref of interest.
  This function should be used for calibration routines in FTM only.
*/
typedef boolean (*rfdevice_cmn_set_dac_iref_fp_type)
(
  rfdevice_cmn_type *cmn_dev,
  rfdevice_dac_iref_val iref_val,
  boolean program_iref,
  uint32 *agc_gainword
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to set ET mode
*/
typedef boolean (*rfdevice_cmn_set_et_path_fp_type)
(
  rfdevice_cmn_type *cmn_dev,
  rfm_mode_enum_type mode,
  rfm_fb_attenuation_state_type fb_state,
  boolean enable_et_path
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for API that will trigger RF device to backup its internal
  state - shadow
*/
typedef boolean (*rfdevice_cmn_backup_state_fp_type)
(
  rfdevice_cmn_type *cmn_dev,
  uint8* backup_token
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for API that will trigger RF device to restore its internal
  state to previously backed up value
*/
typedef boolean (*rfdevice_cmn_restore_state_fp_type)
(
  rfdevice_cmn_type *cmn_dev,
  uint8 backup_token
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration for different power state of a RF device.

*/
typedef enum
{
  RFDEV_OFF = 0,
  RFDEV_ON = 1,
  RFDEV_PARTIAL_OFF = 2,
  RFDEV_PARTIAL_ON = 3

} rfdev_cmn_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration for power voting state of a RF device.

*/
typedef enum
{
  RFDEV_VOTE_OFF = 0,
  RFDEV_VOTE_ON = 1,
  RFDEV_VOTE_PARTIAL_OFF = 2,
  RFDEV_VOTE_PARTIAL_ON = 3

} rfdev_cmn_vote_status_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration for power voting types of a RF device.

*/
typedef enum
{
  RFDEV_CMN_NO_VOTING = 0,
  RFDEV_CMN_NORMAL_VOTING,
  RFDEV_CMN_CRITICAL_VOTING = RFDEV_CMN_NORMAL_VOTING,
  RFDEV_CMN_NON_CRITICAL_VOTING

} rfdev_cmn_voting_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration for power down/reset types for a RF device.

*/

typedef enum
{
  RFDEV_CMN_WWAN_RESET = 0,
  RFDEV_CMN_GPS_CHIP_OFF

} rfdev_cmn_power_down_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to execute Power up/down sequence
*/

typedef rfdev_cmn_status_type (*rfdevice_cmn_power_up_type)
(
  rfdevice_cmn_type*,
  rfdev_cmn_vote_status_type,
  rfdev_cmn_voting_type
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to execute WWAN Reset/GPS Chip OFF sequence
*/

typedef boolean (*rfdevice_cmn_power_reset_type)
(
  rfdevice_cmn_type*,
  rfdev_cmn_power_down_type
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to execute common device dispatch
*/
typedef int32 (*rfdevice_cmn_cmd_dispatch_type)
(
  rfdevice_cmn_type*,
  rfdevice_cmd_enum_type,
  void*
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for API that will retrieve device instance based on rfm_mode
  and chain.
*/
typedef void* (*rfdevice_cmn_get_device_fp_type)
(
  rfdevice_cmn_type *cmn_device,
  rfm_mode_enum_type rfm_mode,
  rfdevice_chain_enum_type chain
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to execute internal device cal data and return results
*/
typedef boolean (*rfdevice_cmn_do_internal_dev_cal_fp_type)
(
  rfdevice_cmn_type* cmn_device,
  rfdevice_cmn_int_dev_cal_data_type* cal_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure of Common interface function pointer
*/
typedef struct
{
  rfdevice_cmn_do_rc_tuner_cal_fp_type do_rc_tuner_cal_fp;
  /*!< Interface to perform RF Tuner Calibration */

  rfdevice_cmn_rc_tuner_load_cal_fp_type rc_tuner_load_cal_fp;
  /*!< Interface to perform RF Tuner Calibration */

  rfdevice_cmn_set_dac_iref_fp_type set_dac_iref_fp;
  /*!< Interface to set Iref of interest */

  rfdevice_cmn_set_et_path_fp_type set_et_path_fp;
  /*!< Interface to Enable or disable ET path */

  rfdevice_cmn_cmd_dispatch_type cmd_dispatch_fp;
  /*!< Common Device Dispatch function */

  rfdevice_cmn_power_up_type do_power_up;
  /*!< Interface to Enable/Disable Power */

  rfdevice_cmn_power_reset_type do_power_reset;
  /*!< Interface to perform Power for tech/ turn off GPS*/

  rfdevice_cmn_backup_state_fp_type backup_fp;
  /*!< Backs up device internal state for a possible restore later */

  rfdevice_cmn_restore_state_fp_type restore_fp;
  /*!< Restores device internal state to a previous backup */

  rfdevice_cmn_get_device_fp_type get_device_fp;
  /*!< Retrieves device instance based on rfm_mode and chain */

  rfdevice_cmn_do_internal_dev_cal_fp_type do_int_dev_cal_fp;
  /*!< Executes internal device calibration */

  rfdevice_cmn_power_up_type do_partial_power_up;
  /*!< Interface to Enable/Disable Power retaining SSBIs */

} rfdevice_cmn_func_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF Common Device related interface function pointers and state variables

  @details
  A RF Device independent interface will ve provided to Main Control, Core and
  Feature modules through these functions pointers. These function pointers
  will be initialized by the specific RF device whenever there is change in
  mode or band.
*/
typedef struct rfcmn_device
{
  rf_lock_data_type *critical_sect;
  /*!< Pointer to critical section lock pertaining to common RTR driver */

  const rfdevice_id_enum_type device_id;
  /*!<  Current Device Driver  */

  const rfdevice_cmn_func_tbl_type* const func_tbl;
  /*!< Table of function pointers pertaining to common Device driver */

  void *data;
  /*!< Void pointer to device data which can be device specific */

  void* tech_instance[RFM_NUM_MODES];
  /*!< Void pointer to tech instances */

} rfcmn_device_type;

/*!
  @brief
  SBI client initialization information.
*/
typedef struct
{
  uint8 rev;
  uint8 bus_num;
  uint8 num_pages;
  uint16 num_regs;

  uint8 *client_pagex_regs;
  uint8 **client_default_reg_values;
} sbi_client_init_type;

/*!
  Captures Transceiver scripts and data that varies per RGI
*/
typedef struct
{
  /*! Contains write to setup Base band filters */
  rf_buffer_intf* bbf_script;

  /*! All TX settings besides bbf and fbrx */
  rf_buffer_intf* tx_gain_script;

  /*! Writes to trigger TX gain to take effect */
  rf_buffer_intf* trigger_script;

  /*! IREF value requested in 1/10th of micro Ampere units */
  uint16 iref_uamp_x10;

/*! RSB Coefficent Alpha calculated from Gain and Phase */
  uint16 rsb_coeff_alpha;

/*! RSB Coefficent Beta calculated from Gain and Phase */
  uint16 rsb_coeff_beta;

 /*! LUT ID */
  uint16 lut_id;

 /*! PHASE value */
  int16 phase;

} rfdevice_tx_lut_data_type;


/*!
  @brief
  defines the bandwidth for which the offset power been requested
  along with the offset power from calibrated bandwidth
*/

typedef struct
{
   /*! requested bandwidth */
   rfcom_bw_type_u requested_offset_cal_bw;

   /*! offset power */
   int16 offset_db10;

   /*!offset valid flag*/
   boolean offset_valid;

} rfdevice_tx_power_offset_bw_type;

/*!
  Captures FBRx RSB coefficients
*/
typedef struct
{
  /*! A coefficient for IQ Mismatch */
  int32 a_coeff;

  /*! B coefficient for IQ Mismatch */
  int32 b_coeff;
} rfdevice_fbrx_rsb_data_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint64 i_sq;
  uint64 q_sq;
  int64  iq;
  int16  sin_theta_f;
  int16  cos_theta_f;
  int16  gain_inv_f;
  uint8  gain_state;
} rfdevice_rsb_cal_data_type;

/*----------------------------------------------------------------------------*/

/*!
  @brief
  information showing which combination of band, dev, gain state
  RSB cal is done for.
*/

typedef struct
{

  /*! Band for which RSB cal data is available */

  uint8 band;

  /*! devcie for which RSB cal data is available */

  uint8 rfm_device;

  /*! gain state for which RSB cal data is available */

  uint8 gain_state;

} rfdevice_rsb_calibrated_data_type;

/*----------------------------------------------------------------------------*/

/*!
  @brief
  information showing the the number of cal'ed items in RSB data struc and
  the pointer to the list
*/
typedef struct
{

  /*! size of allocated buffer in ftm side */

  uint16 num_elements;

  /*! number of items which rsb data is available for */

  uint16 num_calibrated_item;

  /*! pointer to the list of vailable RSB data per band, dev, gain state*/

  rfdevice_rsb_calibrated_data_type* cal_data;

} rfdevice_rsb_cal_verfication_data_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  /*! @brief  SBI transaction time relative to the burst  */
  union
  {
    int16     start_delta;  //union of the types used by Software and firmware
    int16     startDelta;
  };

  /*! @brief  SBI register */
  union
  {
    uint8     data;
    uint8     regData;
    uint8     reg_data;
  };

  /*! @brief  SBI address */
  union
  {
    uint8  addr;
    uint8 regAddr;
    uint8 reg_addr;
  };

  /*! @brief  SBI mode control. May be used for the 2 msb of SSBI 2.0 on RFD targets */
  union
  {
    uint8     iq_mode;
    uint8     iq_mode_slave_id;
    uint8 sbiMode;
  };

  /*! @brief  SBI  bus */
  union
  {
    uint8     sbi_bus;
    uint8     ctl_fifo;
    struct
    {
      uint8 sbiMasterId     :3;
      uint8 waitTimeRef    :1;
    };
  };
} rfmeas_sbi_type;

/*!
  Captures ILPC accumulation information
*/
typedef struct
{
  /*! Number of accumulation samples */
  uint16 fbrx_ilpc_accum_samples;

  /*! Total accumulation time (uS) */
  uint8  fbrx_ilpc_accum_time;
} rfdevice_fbrx_ilpc_accum_data_type;

/*!
  Captures AutoPin data
*/
typedef struct
{
  uint64  sum_i_q_sqr;
  uint64  sum_i_and_q_sqr_ave;
  uint64  p_ave;
  int32   sum_i;
  int32   sum_q;
  int32   i_dc;
  int32   q_dc;
  uint32  fbrx_accum;
  uint32  sum_peaks;
  uint16  par;
  uint8   num_peaks;
} rfdevice_fbrx_autopin_data_type;

#endif /* RFDEVICE_CMN_INTF_H */

