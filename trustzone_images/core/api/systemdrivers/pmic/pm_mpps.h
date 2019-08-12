#ifndef PM_MPPS_H
#define PM_MPPS_H
/*===========================================================================


            P M _ M P P S   H E A D E R    F I L E

DESCRIPTION
    This file contains functions prototypes and variable/type/constant 
  declarations to support the different voltage regulators inside the 
  Qualcomm Power Manager ICs.
  
Copyright (c) 2003-2012  by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
                                                                         
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_mpps.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/12   hs      Updated the interface.
03/02/12   hs	   Removed deprecated APIs.
05/03/11   dy      Rename digital logic level enumerations
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/26/10   wra     Changed documentation from @example to Examples so Deoxygen 
                    can parse the file
07/11/10   jtn     Aded APIs to get MPP status, corrected pm_mpp_dlogic_inout_pup_type 
                   to reflect PMIC HW capabilities 
07/02/10   wra     Changed pm_mpp_which_type to int
03/15/10   fpe     CMI Merge - Made the interface common between 7x30 and SCMM
03/05/10   vk      Added API pm_mpp_config_dtest_output()
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
10/19/09   vk      Removed init API
07/06/09   jtn     Added enums for PM8028 digital logic levels
01/26/09   Vk      Introduced following APIs
                   pm_config_secure_mpp_config_digital_input()
                   pm_secure_mpp_config_digital_input()
12/17/08   APU     Introduced the APIs:
                   1. pm_secure_mpp_config_i_sink ()
                   2. pm_make_secure__mpp_config_i_sink ()
08/01/08   jtn     Move all proc comm functions to pm_pcil.c
06/27/08   jtn     Merge changes from QSC1100 branch, consolidated 
                   definition for max. number of MPPs
06/24/08   vk      Provided API that maintains the list of MPPs that have shunt
                   capacitors to ground. In the MPP configuration API for 
                   anaolog output this list is used.
03/01/08   jtn     Corrected number of MPPs for Han
02/27/08   jtn     Added support for Kip PMIC
01/07/08   jtn     Merged from QSC6270 branch
(begin QSC6270 changes) 
10/11/07   jnoblet Added MPPs for Han
(end QSC6270 changes) 
11/13/07   jtn     Added pm_config_secure_mpp_config_digital_output and 
                   pm_secure_mpp_config_digital_output to support RPC access
                   to MPP digital outputs
06/18/07   cng     Added an input parameter to pm_mpp_config_analog_output
                   to allow configurable analog output voltage 
05/18/07   hs      Restored the third parameter in pm_mpp_config_digital_input()
09/08/06   cng     Removed API pm_mpp_config_d_test and enum pm_mpp_dtest_in_dbus_type
06/22/06   hs      Removed the third parameter in pm_mpp_config_digital_input()
06/08/06   hs      Added pm_mpp_config_atest API support
11/09/05   cng     Added PM6620 support
10/24/05   cng     Added Panoramix support; Removed MPP config type enum;
                   Corrected number of MPPs for PM6650 
07/25/05   cng     PM7500 support
07/01/05   cng     Added MPP config type enum
03/15/05   cng     For the analog input function, added the functionality to
                   route the MPP input to one of the 3 analog buses 
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
11/03/03   rmd     Updated the settings for the BIDIRECTIONAL the pull up 
                   resistor.
09/08/03   rmd     enum "pm_mpp_aout_switch_type" was in the wrong order.
08/11/03   rmd     Created.
===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

#include "comdef.h"
#include "pm_lib_err.h"
#include "pm_lib_cmd.h"

typedef int pm_mpp_which_type;

/* Which MPP needs to be configured */
enum
{
  PM_MPP_1,
  PM_MPP_2,
  PM_MPP_3,
  PM_MPP_4,
  PM_MPP_5,
  PM_MPP_6,
  PM_MPP_7,
  PM_MPP_8,
  PM_MPP_9,
  PM_MPP_10,
  PM_MPP_11,
  PM_MPP_12,
  PM_MPP_13,
  PM_MPP_14,
  PM_MPP_15,
  PM_MPP_16,
  PM_MPP_INVALID,
};

/*************************************************************************** 
   The types defined below are used to configure the following MPPs modes:
   DIGITAL INPUT, DIGITAL OUTPUT and BIDIRECTIONAL.
 ***************************************************************************/

typedef enum
{
  PM_MPP__DLOGIC__LVL_VIO_0 = 0,
  PM_MPP__DLOGIC__LVL_VIO_1,
  PM_MPP__DLOGIC__LVL_VIO_2,
  PM_MPP__DLOGIC__LVL_VIO_3,
  PM_MPP__DLOGIC__LVL_VIO_4,
  PM_MPP__DLOGIC__LVL_VIO_5,
  PM_MPP__DLOGIC__LVL_VIO_6,
  PM_MPP__DLOGIC__LVL_VIO_7,
  PM_MPP__DLOGIC__LVL_INVALID = 8,
  
} pm_mpp_dlogic_lvl_type;

/* MODE = DIGITAL INPUT. Configures the output logic. */
typedef enum
{
  PM_MPP__DLOGIC_IN__DBUS1,
  PM_MPP__DLOGIC_IN__DBUS2,
  PM_MPP__DLOGIC_IN__DBUS3,
  PM_MPP__DLOGIC_IN__DBUS4,
  PM_MPP__DLOGIC_IN__DBUS_INVALID
}pm_mpp_dlogic_in_dbus_type;

/* MODE = DIGITAL OUT. Configures the output logic. */
//TODO: hshen: this enum seems obsolete for BADGER
typedef enum
{
  PM_MPP__DLOGIC_OUT__CTRL_LOW,    /* MPP OUTPUT= LOGIC LOW         */
  PM_MPP__DLOGIC_OUT__CTRL_HIGH,   /* MPP OUTPUT= LOGIC HIGH        */
  PM_MPP__DLOGIC_OUT__CTRL_MPP,    /* MPP OUTPUT= CORRESPONDING 
                                                  MPP INPUT         */
  PM_MPP__DLOGIC_OUT__CTRL_NOT_MPP,/* MPP OUTPUT= CORRESPONDING
                                                  INVERTED MPP INPUT*/
  PM_MPP__DLOGIC_OUT__CTRL_INVALID
}pm_mpp_dlogic_out_ctrl_type;

/* MODE = BIDIRECTIONAL. Configures the pull up resistor. */
typedef enum
{
  PM_MPP__DLOGIC_INOUT__PUP_1K,  /* PULL UP = 1  K Ohms*/
  PM_MPP__DLOGIC_INOUT__PUP_10K, /* PULL UP = 10 K Ohms*/
  PM_MPP__DLOGIC_INOUT__PUP_30K, /* PULL UP = 30 K Ohms*/   
  PM_MPP__DLOGIC_INOUT__PUP_OPEN,  /* PULL UP = OPEN */
  PM_MPP__DLOGIC_INOUT__PUP_INVALID
}pm_mpp_dlogic_inout_pup_type;

/* MODE = DIGITAL TEST OUTPUT. */
typedef enum
{
  PM_MPP__DLOGIC_OUT__DBUS1,
  PM_MPP__DLOGIC_OUT__DBUS2,
  PM_MPP__DLOGIC_OUT__DBUS3,
  PM_MPP__DLOGIC_OUT__DBUS4,
  PM_MPP__DLOGIC_OUT__DBUS_INVALID
}pm_mpp_dlogic_out_dbus_type;

/*************************************************************************** 
   The types defined below are used to configure the following MPPs modes:
   ANALOG INPUT.
 ***************************************************************************/
typedef enum
{
  PM_MPP__AIN__CH_AMUX5,
  PM_MPP__AIN__CH_AMUX6,
  PM_MPP__AIN__CH_AMUX7,
  PM_MPP__AIN__CH_AMUX8,
  PM_MPP__AIN__CH_ABUS1,
  PM_MPP__AIN__CH_ABUS2,
  PM_MPP__AIN__CH_ABUS3,
  PM_MPP__AIN__CH_ABUS4,
  PM_MPP__AIN__CH_INVALID
}pm_mpp_ain_ch_type;

/*************************************************************************** 
   The types defined below are used to configure the following MPPs modes:
   ANALOG OUTPUT.
 ***************************************************************************/
typedef enum
{     
  PM_MPP__AOUT__LEVEL_VREF_1p25_Volts,    
  PM_MPP__AOUT__LEVEL_VREF_0p625_Volts,
  PM_MPP__AOUT__LEVEL_VREF_0p3125_Volts,
  PM_MPP__AOUT__LEVEL_INVALID
}pm_mpp_aout_level_type;

typedef enum
{  
  PM_MPP__AOUT__SWITCH_OFF,
  PM_MPP__AOUT__SWITCH_ON,
  PM_MPP__AOUT__SWITCH_ON_IF_MPP_HIGH,
  PM_MPP__AOUT__SWITCH_ON_IF_MPP_LOW,
  PM_MPP__AOUT__SWITCH_INVALID
}pm_mpp_aout_switch_type;

/*************************************************************************** 
   The types defined below are used to configure the following MPPs modes:
   CURRENT SINK.
 ***************************************************************************/
typedef enum
{
  PM_MPP__I_SINK__LEVEL_5mA,
  PM_MPP__I_SINK__LEVEL_10mA,
  PM_MPP__I_SINK__LEVEL_15mA,
  PM_MPP__I_SINK__LEVEL_20mA,
  PM_MPP__I_SINK__LEVEL_25mA,
  PM_MPP__I_SINK__LEVEL_30mA,
  PM_MPP__I_SINK__LEVEL_35mA,
  PM_MPP__I_SINK__LEVEL_40mA,
  PM_MPP__I_SINK__LEVEL_INVALID
}pm_mpp_i_sink_level_type;

typedef enum
{
  PM_MPP__I_SINK__SWITCH_DIS,
  PM_MPP__I_SINK__SWITCH_ENA,
  PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_HIGH,
  PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_LOW,
  PM_MPP__I_SINK__SWITCH_INVALID
}pm_mpp_i_sink_switch_type;
/***************************************************************************/

/* ATEST*/
typedef enum
{
  PM_MPP__CH_ATEST1,
  PM_MPP__CH_ATEST2,
  PM_MPP__CH_ATEST3,
  PM_MPP__CH_ATEST4,
  PM_MPP__CH_ATEST_INVALID
}pm_mpp_ch_atest_type;

/**
 * Structure used for returning digital input configuration
 */
typedef struct 
{
    pm_mpp_dlogic_lvl_type mpp_dlogic_lvl;
    pm_mpp_dlogic_in_dbus_type mpp_dlogic_in_dbus;
} pm_mpp_digital_input_status_type;

/**
 * Structure used for returning digital output configuration
 */
typedef struct 
{
    pm_mpp_dlogic_lvl_type mpp_dlogic_lvl;
    pm_mpp_dlogic_out_ctrl_type mpp_dlogic_out_ctrl;
} pm_mpp_digital_output_status_type;

/**
 * Structure used for returning bidirectional configuration
 */
typedef struct
{
    pm_mpp_dlogic_lvl_type mpp_dlogic_lvl;
    pm_mpp_dlogic_inout_pup_type mpp_dlogic_inout_pup;
}  pm_mpp_bidirectional_status_type;

/**
 * Structure used for returning analog input configuration
 */
typedef struct
{
    pm_mpp_ain_ch_type mpp_ain_ch;
}  pm_mpp_analog_input_status_type;

/**
 * Structure used for returning analog output configuration
 */
typedef struct
{
    pm_mpp_aout_level_type mpp_aout_level;
    pm_mpp_aout_switch_type mpp_aout_switch;
}  pm_mpp_analog_output_status_type;

/**
 * Structure used for returning current sink configuration
 */
typedef struct
{
    pm_mpp_i_sink_level_type  mpp_i_sink_level;
    pm_mpp_i_sink_switch_type mpp_i_sink_switch;
}  pm_mpp_current_sink_status_type;

/**
 * Structure used for returning DTEST output configuration
 */
typedef struct
{
    pm_mpp_dlogic_lvl_type mpp_dlogic_lvl;
    pm_mpp_dlogic_out_dbus_type  mpp_dlogic_out_dbus;
} pm_mpp_dtest_output_status_type;

/**
 * Enumeration of available MPP modes
 */
typedef enum
{
    PM_MPP_CFG_DIGITAL_IN,
    PM_MPP_CFG_DIGITAL_OUT,
    PM_MPP_CFG_BIDIRECTIONAL,
    PM_MPP_CFG_ANALOG_IN,
    PM_MPP_CFG_ANALOG_OUT,
    PM_MPP_CFG_ISINK,
    PM_MPP_CFG_DTEST_OUT
} pm_mpp_config_type;

/**
 * Structure type used for returning MPP status
 */
typedef struct s_pm_mpp_status_type
{
    /**
     * This returns the MPP's mode (input, output, etc.)
     */
    pm_mpp_config_type mpp_config;
    /**
     * Union of structures used to return MPP status.  The structure
     * that has valid data will depend on the MPP mode (input,
     * output, etc.) returned in mpp_config
     */
    union 
    {
        pm_mpp_digital_input_status_type mpp_digital_input_status;   /*~ IF ( _DISC_ == 0 ) s_pm_mpp_status_type.mpp_config_settings.mpp_digital_input_status */
        pm_mpp_digital_output_status_type mpp_digital_output_status; /*~ IF ( _DISC_ == 1 ) s_pm_mpp_status_type.mpp_config_settings.mpp_digital_output_status */
        pm_mpp_bidirectional_status_type mpp_bidirectional_status;   /*~ IF ( _DISC_ == 2 ) s_pm_mpp_status_type.mpp_config_settings.mpp_bidirectional_status */
        pm_mpp_analog_input_status_type mpp_analog_input_status;     /*~ IF ( _DISC_ == 3 ) s_pm_mpp_status_type.mpp_config_settings.mpp_analog_input_status */
        pm_mpp_analog_output_status_type mpp_analog_output_status;   /*~ IF ( _DISC_ == 4 ) s_pm_mpp_status_type.mpp_config_settings.mpp_analog_output_status */
        pm_mpp_current_sink_status_type mpp_current_sink_status;     /*~ IF ( _DISC_ == 5 ) s_pm_mpp_status_type.mpp_config_settings.mpp_current_sink_status */
        pm_mpp_dtest_output_status_type mpp_dtest_output_status;     /*~ IF ( _DISC_ == 6 ) s_pm_mpp_status_type.mpp_config_settings.mpp_dtest_output_status */
    } mpp_config_settings;                                           /*~ FIELD s_pm_mpp_status_type.mpp_config_settings DISC s_pm_mpp_status_type.mpp_config */
} pm_mpp_status_type;

/**
 * @brief Returns the status of one of the PMIC MPPs
 * @param pmic_device_index mic_device_index Selects the device in which the coin cell charger being controlled are located.
 * @param mpp Which MPP
 * @param mpp_status Pointer to the MPP status structure defined
 *                   above
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_mpp_status_get(unsigned pmic_device_index, pm_mpp_which_type mpp, pm_mpp_status_type *mpp_status);
//#pragma message("Do not use pm_mpp_status_get(), which is deprecated. Use pm_dev_mpp_status_get() instead.")
pm_err_flag_type pm_mpp_status_get(pm_mpp_which_type mpp, pm_mpp_status_type *mpp_status);

/*~ FUNCTION  pm_mpp_status_get */
/*~ PARAM OUT mpp_status POINTER */
/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION pm_mpp_config_digital_input                    EXTERNAL FUNCTION

DESCRIPTION
  Configure the selected Multi Purpose pin (MPP) to be a digital input pin.
       
  example:
    "Configure MPP5 to be a digital input, set the voltage reference
     to MSMP VREG and drive Data bus 2"
        
    errFlag = pm_mpp_config_digital_input(PM_MPP_5, 
                                           PM_MPP__DLOGIC__LVL_MSMP,
                                            PM_MPP__DLOGIC_IN__DBUS2);

  Note: The MPPs can be paired as MPP(input) -> MPP(output) or  
        MPP(Bidirec) <-> MPP(Bidirec) according to the table below.
                                           
        MPP level translator pairs:
            MPP1   <->  MPP2
            MPP3   <->  MPP4
            MPP5   <->  MPP6
            MPP7   <->  MPP8
            MPP9   <->  MPP10
            MPP11  <->  MPP12 
                                                                            
PARAMETERS
  1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 
    
    - Valid Inputs: Device index starts with zero

  2) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

  3) Parameter name: level. 
     - The logic level reference that we want to use with this MPP.

     Parameter type: pm_mpp_dlogic_lvl_type (enum).
     - Valid Inputs:   
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD     

  4) Parameter name: dbus. 
     - Which data line will the MPP drive. Please note that only one MPP 
       can be assigned per DBUS.

     Parameter type:  (enum).
     - Valid Inputs:
          PM_MPP__DLOGIC_IN__DBUS_NONE
          PM_MPP__DLOGIC_IN__DBUS1
          PM_MPP__DLOGIC_IN__DBUS2
          PM_MPP__DLOGIC_IN__DBUS3   
               
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
      PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is already in use by
                                          another MPP.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.  
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_dev_mpp_config_digital_input(
                                          unsigned                    pmic_device_index, 
                                          pm_mpp_which_type           mpp,
                                          pm_mpp_dlogic_lvl_type      level,
                                          pm_mpp_dlogic_in_dbus_type  dbus);
//#pragma message("Do not use pm_mpp_config_digital_input(), which is deprecated. Use pm_dev_mpp_config_digital_input() instead.")
extern pm_err_flag_type pm_mpp_config_digital_input(
    pm_mpp_which_type           mpp,
    pm_mpp_dlogic_lvl_type      level,
    pm_mpp_dlogic_in_dbus_type  dbus);

/*===========================================================================
FUNCTION pm_mpp_config_digital_output                      EXTERNAL FUNCTION

DESCRIPTION
  Configure the selected Multi Purpose pin (MPP) to be a digital output pin.
  
  example:
    " Configure MPP10 to be a digital output.
      Set the MPP voltage reference to RUIM VREG.
      The Digital Ouput is equal the input of MPP9(See parameter 3
                                                   description)"
      
    err |= pm_mpp_config_digital_output(PM_MPP_10, 
                                         PM_MPP__DLOGIC__LVL_RUIM, 
                                          PM_MPP__DLOGIC_OUT__CTRL_MPP);                                            

  Note: The MPPs can be paired as MPP(input) -> MPP(output) or  
        MPP(Bidirec) <-> MPP(Bidirec) according to the table below.
                                           
        MPP level translator pairs:
            MPP1   <->  MPP2
            MPP3   <->  MPP4
            MPP5   <->  MPP6
            MPP7   <->  MPP8
            MPP9   <->  MPP10
            MPP11  <->  MPP12 
            
PARAMETERS
    1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 

    - Valid Inputs: Device index starts with zero

  2) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

  3) Parameter name: level. 
     - The logic level reference that we want to use with this MPP pin.

     Parameter type: pm_mpp_dlogic_lvl_type (enum).
     - Valid Inputs:
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD       

  4) Parameter name: output_ctrl. 
     - The output setting of the selected MPP.

     Parameter type: pm_mpp_dlogic_out_ctrl_type (enum).     
     - Valid Inputs:
        PM_MPP__DLOGIC_OUT__CTRL_LOW,    = LOGIC LOW.
        PM_MPP__DLOGIC_OUT__CTRL_HIGH,   = LOGIC HIGH.
        PM_MPP__DLOGIC_OUT__CTRL_MPP,    = CORRESPONDING MPP INPUT 
                                           (see the note regarding MPP pairs
                                            within the decription section).         
        PM_MPP__DLOGIC_OUT__CTRL_NOT_MPP = CORRESPONDING INVERTED MPP INPUT
                                           (see the note regarding MPP pairs
                                            within the decription section).         
     
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter three is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.          
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_dev_mpp_config_digital_output(
                                    unsigned pmic_device_index, 
                                    pm_mpp_which_type           mpp,
                                    pm_mpp_dlogic_lvl_type      level,
                                    pm_mpp_dlogic_out_ctrl_type output_ctrl);
//#pragma message("Do not use pm_mpp_config_digital_output(), which is deprecated. Use pm_dev_mpp_config_digital_output() instead.")
extern pm_err_flag_type pm_mpp_config_digital_output(
    pm_mpp_which_type           mpp,
    pm_mpp_dlogic_lvl_type      level,
    pm_mpp_dlogic_out_ctrl_type output_ctrl);


/*===========================================================================
FUNCTION pm_mpp_config_digital_inout                    EXTERNAL FUNCTION

DESCRIPTION
     Configure the selected Multi Purpose pin (MPP) to be a digital 
  bidirectional pin.
                                           
  example:
  
      Configure MPP11 and MPP12 to be bidirectionial(see the note below).
      Set their logic voltage reference to RUIM VREG.
      Set up the pull up ressitor setting to 30K Ohms.
      
      err =  pm_mpp_config_digital_inout( PM_MPP_11, 
                                          PM_MPP__DLOGIC__LVL_RUIM,
                                          PM_MPP__DLOGIC_INOUT__PUP_30K);

      err |= pm_mpp_config_digital_inout( PM_MPP_12, 
                                          PM_MPP__DLOGIC__LVL_RUIM,
                                          PM_MPP__DLOGIC_INOUT__PUP_30K);

  Note: The MPPs can be paired as MPP(input) -> MPP(output) or  
        MPP(Bidirec) <-> MPP(Bidirec) according to the table below.
                                           
        MPP level translator pairs:
            MPP1   <->  MPP2
            MPP3   <->  MPP4
            MPP5   <->  MPP6
            MPP7   <->  MPP8
            MPP9   <->  MPP10
            MPP11  <->  MPP12
            
PARAMETERS
    1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 

    - Valid Inputs: Device index starts with zero

  2) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16
            
  3) Parameter name: level. 
     - The logic level reference that we want to use with this MPP.

     Parameter type: pm_mpp_dlogic_lvl_type (enum).
     - Valid Inputs:
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD         

  4) Parameter name: pull_up. 
     - The pull-up resistor setting of the selected MPP.

     Parameter type: pm_mpp_dlogic_inout_pup_type (enum).
     - Valid Inputs:
            PM_MPP__DLOGIC_INOUT__PUP_30K  = 30K Ohms
            PM_MPP__DLOGIC_INOUT__PUP_10K  = 10K Ohms
            PM_MPP__DLOGIC_INOUT__PUP_3K   = 3K  Ohms
            PM_MPP__DLOGIC_INOUT__PUP_1K   = 1K  Ohms     
          
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
       PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
       PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
       PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter three is out of range.
       PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                           version of the PMIC.         
       PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                           with the PMIC.      
       PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_dev_mpp_config_digital_inout(
                                    unsigned                     pmic_device_index, 
                                    pm_mpp_which_type            mpp,
                                    pm_mpp_dlogic_lvl_type       level,
                                    pm_mpp_dlogic_inout_pup_type pull_up);
//#pragma message("Do not use pm_mpp_config_digital_inout(), which is deprecated. Use pm_dev_mpp_config_digital_inout() instead.")
extern pm_err_flag_type pm_mpp_config_digital_inout(
    pm_mpp_which_type            mpp,
    pm_mpp_dlogic_lvl_type       level,
    pm_mpp_dlogic_inout_pup_type pull_up);

/*===========================================================================
FUNCTION pm_mpp_config_dtest_output                      EXTERNAL FUNCTION

DESCRIPTION
  Configure the selected Multi Purpose pin (MPP) to be a digital test output pin.
  
  example:
    " Configure MPP10 to be a digital test output.
      Set the MPP voltage reference to RUIM VREG and connect it to DBUS1.
            
    err |= pm_mpp_config_dtest_output(PM_MPP_10, 
                                         PM_MPP__DLOGIC__LVL_RUIM, 
                                          PM_MPP__DLOGIC_IN__DBUS1);
                                          
PARAMETERS
    1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 

    - Valid Inputs: Device index starts with zero

  2) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12

  3) Parameter name: level. 
     - The logic level reference that we want to use with this MPP.

     Parameter type: pm_mpp_dlogic_lvl_type (enum).
     - Valid Inputs:
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD     

  4) Parameter name: dbus. 
     - Which data line will the MPP drive.

     Parameter type: pm_mpp_dlogic_out_dbus_type (enum).
     - Valid Inputs:
           PM_MPP__DLOGIC_OUT__DBUS1,
           PM_MPP__DLOGIC_OUT__DBUS2,
           PM_MPP__DLOGIC_OUT__DBUS3,
           PM_MPP__DLOGIC_OUT__DBUS4,
           PM_MPP__DLOGIC_OUT__DBUS_INVALID      
                                                
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter three is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.          
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
pm_err_flag_type pm_dev_mpp_config_dtest_output(
                                             unsigned                    pmic_device_index, 
                                             pm_mpp_which_type           mpp,
                                             pm_mpp_dlogic_lvl_type      level,
                                             pm_mpp_dlogic_out_dbus_type  dbus);
//#pragma message("Do not use pm_mpp_config_dtest_output(), which is deprecated. Use pm_dev_mpp_config_dtest_output() instead.")
pm_err_flag_type pm_mpp_config_dtest_output(
    pm_mpp_which_type           mpp,
    pm_mpp_dlogic_lvl_type      level,
    pm_mpp_dlogic_out_dbus_type  dbus);


/*===========================================================================
FUNCTION pm_mpp_config_analog_input                    EXTERNAL FUNCTION

DESCRIPTION
    This function configures the selected MPP as an Analog Input.
       
    example:
    
        Configure MPP4 to be an analog input and connected it to analog 
        MUX6.
        
        err = pm_mpp_config_analog_input(PM_MPP_6,PM_MPP__AIN__CH_AMUX9);

PARAMETERS
    1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 

    - Valid Inputs: Device index starts with zero

  2) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

  3) Parameter name: ch_select. 
     - Which analog mux or analog bus will the selected MPP be routed to.

     Parameter type: (enum).
     - Valid Inputs:
            PM_MPP__AIN__CH_AMUX5
            PM_MPP__AIN__CH_AMUX6
            PM_MPP__AIN__CH_AMUX7
            PM_MPP__AIN__CH_AMUX8
            PM_MPP__AIN__CH_AMUX9
            PM_MPP__AIN__CH_ABUS1
            PM_MPP__AIN__CH_ABUS2
            PM_MPP__AIN__CH_ABUS3
     
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.                  
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_dev_mpp_config_analog_input(
                                    unsigned             pmic_device_index, 
                                    pm_mpp_which_type    mpp,
                                    pm_mpp_ain_ch_type   ch_select);
//#pragma message("Do not use pm_mpp_config_analog_input(), which is deprecated. Use pm_dev_mpp_config_analog_input() instead.")
extern pm_err_flag_type pm_mpp_config_analog_input(
    pm_mpp_which_type    mpp,
    pm_mpp_ain_ch_type   ch_select);


/*===========================================================================
FUNCTION pm_mpp_config_analog_output                    EXTERNAL FUNCTION

DESCRIPTION
    This function configures the selected MPP as an analog output.
        
    example:
    
        Configure MPP14 to be an analog out and turn it on.        
        
        err = pm_mpp_config_analog_output(PM_MPP_14,PM_MPP__AOUT__SWITCH_ON);

PARAMETERS
    1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 

    - Valid Inputs: Device index starts with zero

  2) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16            
            
  3) Parameter name: level. 
     - Specifies the analog output reference voltage level 

     Parameter type: pm_mpp_aout_level_type (enum).
     - Valid Inputs:                        
        PM_MPP__AOUT__LEVEL_VREF_1p25_Volts = 1.25V,
        PM_MPP__AOUT__LEVEL_VREF_0p625_Volts = 0.625V,
        PM_MPP__AOUT__LEVEL_VREF_2p50_Volts = 2.50V,

  4) Parameter name: OnOff. 
     - This option allows the user to enable/disable the MPP output.

     Parameter type: pm_mpp_aout_switch_type (enum).
     - Valid Inputs:                        
        PM_MPP__AOUT__SWITCH_ON,
        PM_MPP__AOUT__SWITCH_OFF
        PM_MPP__AOUT__SWITCH_ON_IF_MPP_HIGH
        PM_MPP__AOUT__SWITCH_ON_IF_MPP_LOW                 
     
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter three is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.        
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_dev_mpp_config_analog_output(
                                    unsigned                 pmic_device_index, 
                                    pm_mpp_which_type        mpp,
                                    pm_mpp_aout_level_type   level,
                                    pm_mpp_aout_switch_type  OnOff);
//#pragma message("Do not use pm_mpp_config_analog_output(), which is deprecated. Use pm_dev_mpp_config_analog_output() instead.")
extern pm_err_flag_type pm_mpp_config_analog_output(
    pm_mpp_which_type        mpp,
    pm_mpp_aout_level_type   level,
    pm_mpp_aout_switch_type  OnOff);

/*===========================================================================
FUNCTION pm_mpp_config_i_sink                           EXTERNAL FUNCTION

DESCRIPTION
  This function configures the selected MPP as a current sink.
  
  example:
    
    Configure MPP7 to be a current sink. Set the current sink to 25mA and
    enable the current sink.
    
    err = pm_mpp_config_i_sink(PM_MPP_15, 
                                    PM_MPP__I_SINK__LEVEL_25mA,
                                        PM_MPP__I_SINK__SWITCH_ENA);
 

PARAMETERS
    1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 

    - Valid Inputs: Device index starts with zero

  2) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

  3) Parameter name: level. 
     - How much current we want to allow the MPP to sink.

     Parameter type: pm_mpp_i_sink_level_type (enum).
     - Valid Inputs:
            PM_MPP__I_SINK__LEVEL_5mA
            PM_MPP__I_SINK__LEVEL_10mA
            PM_MPP__I_SINK__LEVEL_15mA
            PM_MPP__I_SINK__LEVEL_20mA
            PM_MPP__I_SINK__LEVEL_25mA
            PM_MPP__I_SINK__LEVEL_30mA
            PM_MPP__I_SINK__LEVEL_35mA
            PM_MPP__I_SINK__LEVEL_40mA     

  4) Parameter name: OnOff. 
     - This option allows the user to enable/disable current sink.

     Parameter type: pm_mpp_i_sink_switch_type (enum).
     - Valid Inputs:
            PM_MPP__I_SINK__SWITCH_DIS
            PM_MPP__I_SINK__SWITCH_ENA
            PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_HIGH
            PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_LOW     
                 
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.         
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_dev_mpp_config_i_sink(
                                    unsigned                  pmic_device_index, 
                                    pm_mpp_which_type         mpp,
                                    pm_mpp_i_sink_level_type  level,
                                    pm_mpp_i_sink_switch_type OnOff);
//#pragma message("Do not use pm_mpp_config_i_sink(), which is deprecated. Use pm_dev_mpp_config_i_sink() instead.")
extern pm_err_flag_type pm_mpp_config_i_sink(
    pm_mpp_which_type         mpp,
    pm_mpp_i_sink_level_type  level,
    pm_mpp_i_sink_switch_type OnOff);


/*===========================================================================
FUNCTION pm_mpp_config_atest                           EXTERNAL FUNCTION

DESCRIPTION
    This function configures the selected MPP as ATEST.
       
PARAMETERS
    1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 

    - Valid Inputs: Device index starts with zero

  2) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

  3) Parameter name: atest_select. 
     - Which atest will the selected MPP be routed to.

     Parameter type: (enum).
     - Valid Inputs:
            PM_MPP__CH_ATEST1
            PM_MPP__CH_ATEST2
            PM_MPP__CH_ATEST3
     
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.                  
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_dev_mpp_config_atest(
                                    unsigned             pmic_device_index, 
                                    pm_mpp_which_type    mpp,
                                    pm_mpp_ch_atest_type atest_select);
//#pragma message("Do not use pm_mpp_config_atest(), which is deprecated. Use pm_dev_mpp_config_atest() instead.")
extern pm_err_flag_type pm_mpp_config_atest(
    pm_mpp_which_type    mpp,
    pm_mpp_ch_atest_type   atest_select);


/*===========================================================================
FUNCTION pm_mpp_set_list_mpp_with_shunt_cap                 EXTERNAL FUNCTION

DESCRIPTION
  This function can be used to create an array of MPPs that have shunt 
  capacitors to ground.
  
  example:
    
    If mpp8 has shunt capacitor to ground.
    
    err = pm_mpp_set_list_mpp_with_shunt_cap(PM_MPP_8);

PARAMETERS
    1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 

    - Valid Inputs: Device index starts with zero

  2) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.         
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/

pm_err_flag_type pm_dev_mpp_set_list_mpp_with_shunt_cap(unsigned pmic_device_index, pm_mpp_which_type mpp);
//#pragma message("Do not use pm_mpp_set_list_mpp_with_shunt_cap(), which is deprecated. Use pm_dev_mpp_set_list_mpp_with_shunt_cap() instead.")
pm_err_flag_type pm_mpp_set_list_mpp_with_shunt_cap(pm_mpp_which_type mpp);

/*===========================================================================

FUNCTION pm_get_mpp_with_shunt_cap_list_status_for_device             EXTERNAL FUNCTION

DESCRIPTION
    This function returns the 32bit status which represents that the 
    corresponding mpp has shunt capacitor to ground.

    e.g if MPP8 and MPP1 has shunt capacitor to ground then status would be
        status = 00000000 00000000 00000000 10000001

INPUT PARAMETER
    1) Parameter name: pm_device_index
    - pmic_device_index Selects the device in which the coin cell charger being controlled are located. 

    - Valid Inputs: Device index starts with zero


RETURN VALUE
  Description:
  - 32 bit status which represents that the corresponding mpp has shunt 
    capacitor to ground.

  Return value type: uint32

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
pm_err_flag_type pm_dev_get_mpp_with_shunt_cap_list_status_for_device(unsigned pmic_device_index, pm_mpp_which_type mpp, uint32* shuntList);
//#pragma message("Do not use pm_get_mpp_with_shunt_cap_list_status_for_device(), which is deprecated. Use pm_dev_get_mpp_with_shunt_cap_list_status_for_device() instead.")
pm_err_flag_type pm_get_mpp_with_shunt_cap_list_status_for_device(pm_mpp_which_type mpp, uint32* shuntList);


#endif /* PM66XXMPPS_H */
