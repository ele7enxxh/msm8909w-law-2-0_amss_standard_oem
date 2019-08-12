#ifndef __PM_VREG_H__
#define __PM_VREG_H__
/*! \file 
 *  \n
 *  \brief  pm_vreg.h ---- PMIC voltage regulator driver 
 *  \n
 *  \n This header file contains enums and API definitions for PMIC voltage
 *  regulator driver.
 *  \n
 *  \n &copy; Copyright 2009-2011 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_vreg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/12   hs      Removed unsupported type defs.
                   Updated pm_pwr_buck_switching_freq_type.
04/04/12   hs      Removed the buck switch size alg.
03/30/12   hs      Removed the dirver size alg.
04/10/11   wra     Removed the API to set the driver size.
08/24/11   hw/dy   Replace deprecated APIs
04/17/11   wra     Added A2 output buffer to pm_vreg_smps_mode_type
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
02/07/11   hw      Added enum pm_vreg_smps_inductor_ilim_type
01/24/11   hw      Adding pm_vreg_mode_status() API support
07/26/10   wra     Changed documentation from @example to Examples so Deoxygen 
                   can parse the file
07/21/10   vk      Added pm_vreg_smps_set_stepper_control_delay()
                   pm_vreg_smps_is_stepping_done() API
07/09/10   jtn     Add APIs to return VREG status
07/02/10   wra     Changed pm_vreg_id_type to int
02/18/10   jtn     Added pm_lp_force_lpm_control() API
12/02/09   jtn     CMI cleanup
08/04/09   jtn     Fix Lint critical and high warnings.
07/21/09   jtn     Added required APIs for SCMM. Changed name for USB 3.3V VREG
07/02/09   jtn     Add PM_VREG_MSMC1_ID to list of VREGs
06/30/09   jtn     Updated file documentation
05/11/09   jtn     Removed some legacy VREG IDs
03/19/09   jtn     New file, replacement for pm66XXvregs.h
=============================================================================*/
#include "pm_lib_err.h"
#include "pm_lib_cmd.h"
#include "pm_resources_and_types.h"
#include "comdef.h"

#ifdef __RVCT__
#define PM_VREG_DEPRECATED __attribute__((__deprecated__))
#else
#define PM_VREG_DEPRECATED 
#endif

// VREG identifier used to call VREG APIs
typedef int pm_vreg_id_type;

/**
 * Typedef for VREG level in millivolts
 */
typedef unsigned pm_vreg_level_type_microvolts ;

typedef uint64 pm_vreg_mask_type;
typedef uint64 pm_vreg_masked_type;

/*****************************************************************************/
/* Enumerations used when calling PMIC VREG APIs */
/*****************************************************************************/
typedef enum 
{
    PM_PWR_SW_MODE_PFM = 0,	// Low power mode	
    PM_PWR_SW_MODE_BYPASS = 1,	// Bypass mode bit 5
    PM_PWR_SW_MODE_AUTO = 2,   // auto mode bit 6
    PM_PWR_SW_MODE_NPM = 4,	// Normal mode bit 7	
    PM_PWR_SW_MODE_INVALID
}pm_pwr_sw_mode_type;

/* BUCK VREGs Maximum compensation values. */
#define PM_VREG_BUCK_MAX_SAWTOOTH_I_COMP  0x07
#define PM_VREG_BUCK_MAX_GM_COMP          0x0f
#define PM_VREG_BUCK_MAX_RES_COMP         0x0f

/* BUCK VREGS Compensation settings. */
typedef struct
{
   uint8 sawtooth_set;
   uint8 gm_set;
   uint8 resistor_set;
}pm_vreg_buck_cfg_cp_type;

typedef enum
{
  PM_INDUCTOR_ILIM__300MA,
 
  PM_INDUCTOR_ILIM__700MA,
 
  PM_INDUCTOR_ILIM__1100MA,

  PM_INDUCTOR_ILIM__1500MA,

  PM_INDUCTOR_ILIM__1900MA,

  PM_INDUCTOR_ILIM__2300MA,

  PM_INDUCTOR_ILIM__2700MA,

  PM_INDUCTOR_ILIM__3100MA,

  PM_INDUCTOR_ILIM__INVALID
} pm_pwr_buck_inductor_ilim_type ;


/* BUCK - FTS, SMPS, HFS. */
typedef enum 
{
   PM_CLK_TCXO,
   PM_CLK_RC,
   PM_CLK_SOURCE_INVALID
}pm_pwr_buck_clk_source_type;

/**
 * enumeration assumes input clock freq of 19.2MHz
 * clock frequency = (input clock freq) / ((CLK_PREDIV+1)(CLK_DIV + 1))
 * this enumeration is 5 bit long
 */
typedef enum 
{
    PM_CLK_19p2_MHz = 0,
    PM_CLK_9p6_MHz = 1,
    PM_CLK_6p4_MHz = 2,
    PM_CLK_4p8_MHz = 3,
    PM_CLK_3p84_MHz = 4,
    PM_CLK_3p2_MHz = 5,
    PM_CLK_2p74_MHz = 6,
    PM_CLK_2p4_MHz = 7,
    PM_CLK_2p13_MHz = 8,
    PM_CLK_1p92_MHz = 9,
    PM_CLK_1p75_MHz = 10,
    PM_CLK_1p6_MHz = 11,
    PM_CLK_1p48_MHz = 12,
    PM_CLK_1p37_MHz = 13,
    PM_CLK_1p28_MHz = 14,
    PM_CLK_1p2_MHz = 15,
    PM_CLK_1p13_MHz = 16,
    PM_CLK_1p07_MHz = 17,
    PM_CLK_1p01_MHz = 18,
    PM_CLK_960_KHz = 19,
    PM_CLK_914_KHz = 20,
    PM_CLK_873_KHz = 21,
    PM_CLK_835_KHz = 22,
    PM_CLK_800_KHz = 23,
    PM_CLK_768_KHz = 24,
    PM_CLK_738_KHz = 25,
    PM_CLK_711_KHz = 26,
    PM_CLK_686_KHz = 27,
    PM_CLK_662_KHz = 28,
    PM_CLK_640_KHz = 29,
    PM_CLK_619_KHz = 30,
    PM_CLK_600_KHz = 31,
    PM_SWITCHING_FREQ_INVALID,
	PM_SWITCHING_FREQ_FREQ_NONE
}pm_pwr_buck_switching_freq_type;

/**
 * Typedef for selecting SMPS stepper delay 
 */
typedef float pm_vreg_smps_stepper_delay_type;
/**
 * Typedef for selecting SMPS step size
 */
typedef float pm_vreg_smps_step_size_type;

/**
 * 
 * @param vreg
 * @param voltage
 * 
 * @return pm_err_flag_type
 */
//#pragma message("Do not use pm_vreg_set_level(), which is deprecated. Use pm_dev_pwr_volt_level() instead.")
pm_err_flag_type  pm_vreg_set_level
(
 pm_vreg_id_type    vreg, 
 pm_vreg_level_type_microvolts voltage
 );
pm_err_flag_type pm_dev_pwr_volt_level
(
    unsigned pmic_device_index, 
    unsigned resourceIndex, 
    pm_vreg_level_type_microvolts voltLevel
);

/*~ FUNCTION pm_vreg_set_level */
//#pragma message("Do not use pm_vreg_get_level(), which is deprecated. Use pm_dev_pwr_volt_level_status() instead.")
pm_err_flag_type  pm_vreg_get_level
(
 pm_vreg_id_type    vreg,
 pm_vreg_level_type_microvolts *vreg_level
 );
pm_err_flag_type pm_dev_pwr_volt_level_status
(
    unsigned pmic_device_index, 
    unsigned resourceIndex, 
    pm_vreg_level_type_microvolts *voltLevel
);
/* ~ FUNCTION pm_vreg_get_level */


/**
 * 
 * @param clk_sel
 *     Select RC or TCXO as the SMPS clock source
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_pwr_buck_clk_source
( 
    unsigned                        pmic_device_index,
	uint8 							externalResourceIndex, 
	pm_pwr_buck_clk_source_type 	clkSource 
);
//#pragma message("Do not use pm_pwr_buck_clk_source(), which is deprecated. Use pm_dev_pwr_buck_clk_source() instead.")
pm_err_flag_type pm_pwr_buck_clk_source
( 
    uint8 							externalResourceIndex, 
    pm_pwr_buck_clk_source_type 	clkSource 
);


/**
 * 
 * @param which_vreg Each SMPS VREG can have a different SMPS frequency.  This
 *                   parameter specifies the VREG_ID.
 * @param divider    The SMPS frequency is (19.2 MHz)/divider
 *                   Divider SMPS frequency (MHz)
 *                   2          9.600
 *                   3          6.400
 *                   4          4.800
 *                   5          3.840
 *                   6          3.200
 *                   7          2.743
 *                   8          2.400
 *                   9          2.133
 *                   10         1.920
 *                   11         1.745
 *                   12         1.600
 *                   13         1.477
 *                   14         1.371
 *                   15         1.280
 *                   16         1.200
 * 
 * @return 
 */
pm_err_flag_type pm_dev_pwr_buck_switching_freq
( 
    unsigned                            pmic_device_index,
	uint8 								externalResourceIndex, 
	pm_pwr_buck_switching_freq_type		switchingFreq 
    );
//#pragma message("Do not use pm_pwr_buck_switching_freq(), which is deprecated. Use pm_dev_pwr_buck_switching_freq() instead.")
pm_err_flag_type pm_pwr_buck_switching_freq
( 
    uint8 								externalResourceIndex, 
    pm_pwr_buck_switching_freq_type		switchingFreq 
 );


/*===========================================================================

FUNCTION pm_vreg_buck_config_comp                       EXTERNAL FUNCTION

DESCRIPTION
    This function allows the user to configure the BUCK VREGs compensation 
  parameters.                                                           
  
INPUT PARAMETERS
  1) Parameter name: vreg_buck. 
     - Which BUCK Voltage Regulator we want to configure.

     Parameter type: pm_vreg_buck_id_type (enum)
     - Valid Inputs:
        PM_VREG_BUCK_PA_ID
        PM_VREG_BUCK_MSME_ID
        PM_VREG_BUCK_MSMC1_ID
        PM_VREG_BUCK_MSMC2_ID            Not supported in PM6650 

  2) Parameter name: cfg (pointer).
     - It contains the user's BUCK VREGs compensation parameters.

     Parameter type: pm_vreg_buck_cfg_cp_type (struct).
     - Valid Inputs: 
          1.1) Structure member: sawtooth_set.
               - Sawtooth current setting.
                         
               Type: uint8
               - Valid Inputs: 
                    Integer from 0 to 7, where:
                    
                    Sawtooth I = 4.5uA - (sawtooth_set * 0.5uA)                    
                       
          1.2) Structure member: gm_set.
               - Error Amp Transconductance setting.
                         
               Type: uint8
               - Valid Inputs: 
                    Integer from 0 to 15, where:
                    
                    gm = 1/((4 * gm_set) + 2000 Ohms)
               
          1.3) Structure member: resistor_set.
               - Compensation Resistor setting.
                         
               Type: uint8
               - Valid Inputs: 
                    Integer from 0 to 15, where:
                    
                    resistance = (10 + (20 * resistor_set)) kOhms.

RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = cfg pointer is NULL.  
     PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = One of the compensation settings
                                         within the cfg pointer is out of 
                                         range.
     
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                         version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                         with the PMIC.          
     PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are LOCKED.
===========================================================================*/
pm_err_flag_type pm_dev_vreg_buck_config_comp(
                            unsigned             pmic_device_index,
                            pm_vreg_id_type     vreg_id,
                            const pm_vreg_buck_cfg_cp_type *cfg);
//#pragma message("Do not use pm_vreg_buck_config_comp(), which is deprecated. Use pm_dev_vreg_buck_config_comp() instead.")
pm_err_flag_type pm_vreg_buck_config_comp(pm_vreg_id_type     vreg_id,
                                          const pm_vreg_buck_cfg_cp_type *cfg);


/**
 * 
 * @param vreg_cmd
 * @param vreg
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_pwr_pull_down
(
    unsigned            pmic_device_index,
	uint8 				externalResourceIndex, 
	pm_switch_cmd_type 	enableDisableCmd 
    );
//#pragma message("Do not use pm_pwr_pull_down(), which is deprecated. Use pm_dev_pwr_pull_down() instead.")
pm_err_flag_type pm_pwr_pull_down
(
 uint8 				externalResourceIndex, 
 pm_switch_cmd_type 	enableDisableCmd 
 );


/*~ FUNCTION pm_vreg_pull_down_switch */

/**
 * @brief Return the VREG on/off state
 * 
 * @detailed
 *     This function returns the real-time status of the voltage regulators.
 *     The return value is a bit-mapped variable where each bit represents
 *     the on (1) or off (0) state of the regulators enumerated in
 *     pm_vreg_mask_type
 *     vreg_rt_status is initialized during PMIC init through a call
 *     to pm_vreg_status_initialize()
 *     bits are set or cleared in pm_vreg_control
 * 
 * @return 
 *   Return Value Type: pm_vreg_mask_type
 *   each bit represents one voltage regulator
 */
pm_vreg_mask_type pm_dev_vreg_status_get(unsigned pmic_device_index);
//#pragma message("Do not use pm_vreg_status_get(), which is deprecated. Use pm_dev_vreg_status_get() instead.")
pm_vreg_mask_type pm_vreg_status_get(void);


/**
 * Function: pm_vreg_smps_pulse_skipping_enable(
 *                                         pm_vreg_smps_id_type smps_vreg,
 *                                         boolean              enable)
 * 
 * Description:  Enable or disable SMPS pulse skipping
 * 
 * @param smps_vreg The SMPS that is to be controlled:
 *                  PM_VREG_SMPS_S1_ID
 *                  PM_VREG_SMPS_S2_ID
 * @param enable    TRUE--enable pulse skipping
 *                  FALSE--disable pulse skipping
 * 
 * @return pm_err_flag_type
 * Examples pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S1_ID, TRUE);
 * pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S1_ID, FALSE);
 * pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S2_ID, TRUE);
 * pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S2_ID, FALSE);
 */
pm_err_flag_type pm_dev_vreg_smps_pulse_skipping_enable
(
    unsigned             pmic_device_index,
    pm_vreg_id_type      vreg_id,
    boolean              enable
    );
//#pragma message("Do not use pm_vreg_smps_pulse_skipping_enable(), which is deprecated. Use pm_dev_vreg_smps_pulse_skipping_enable() instead.")
pm_err_flag_type pm_vreg_smps_pulse_skipping_enable
(
    pm_vreg_id_type      vreg_id,
    boolean              enable
);

/**
 * @brief Get SMPS voltage stepping done status
 *
 * @detailed Get SMPS voltage stepping done status
 *
 * @param vreg_smps
 * @param stepper_status  Returns true if stepping is complete, 
 *                        false otherwise
 * 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_pwr_mode_status(unsigned pmic_device_index, uint8 resourceIndex, pm_pwr_sw_mode_type* mode); 
//#pragma message("Do not use pm_vreg_mode_status(), which is deprecated. Use pm_dev_pwr_mode_status() instead.")
pm_err_flag_type pm_vreg_mode_status
(
    pm_vreg_id_type     vreg ,
    pm_pwr_sw_mode_type*  mode_status
);

/**
 * @brief Allow client to change the current limit depending on their inductor size (value)
 *
 *
 * @param ilim_level - set the current limit value
 * @param externalResourceIndex - choose the desire buck
 * 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_vreg_smps_inductor_ilim
(
    unsigned                         pmic_device_index,
    pm_pwr_buck_inductor_ilim_type   ilim_level, 
    pm_vreg_id_type                 externalResourceIndex 
);
//#pragma message("Do not use pm_vreg_smps_inductor_ilim(), which is deprecated. Use pm_dev_vreg_smps_inductor_ilim() instead.")
pm_err_flag_type pm_vreg_smps_inductor_ilim
(
    pm_pwr_buck_inductor_ilim_type  ilim_level, 
    pm_vreg_id_type                 externalResourceIndex 
);

/**
 * @brief Allow client to read the current limit set (value)
 *
 *
 * @param ilim_level - read the current limit value
 * @param externalResourceIndex - choose the desire buck
 * 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_dev_vreg_smps_inductor_ilim_status
( 
  unsigned                         pmic_device_index,
  pm_pwr_buck_inductor_ilim_type*  ilim_level, 
  pm_vreg_id_type                  externalResourceIndex 
  ) ;
//#pragma message("Do not use pm_vreg_smps_inductor_ilim_status(), which is deprecated. Use pm_dev_vreg_smps_inductor_ilim_status() instead.")
pm_err_flag_type pm_vreg_smps_inductor_ilim_status
( 
 pm_pwr_buck_inductor_ilim_type*  ilim_level, 
 pm_vreg_id_type                  externalResourceIndex 
 );

#endif /* pm_vreg.h */

