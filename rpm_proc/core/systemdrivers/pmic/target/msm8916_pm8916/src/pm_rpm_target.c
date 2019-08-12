/*! \file pm_rpm_target.c
*  \n
*  \brief This file contains PMIC functions to run sleep enter and sleep exit settings.
*  \n
*  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/target/msm8916_pm8916/src/pm_rpm_target.c#1 $  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/14   kt      Initial version.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "assert.h"
#include "err.h"
#include "CoreVerify.h"

#include "pm_qc_pmic.h"
#include "pm_comm.h"
#include "bare_metal_info.h"
#include "pm_target_information.h"
#include "pmapp_npa.h"
#include "railway.h"
#include "pm_sleep.h"
#include "pm_sleep_config.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

static uint8 pm_chip_major_revision = 0;

static pm_sleep_cfg_type* sleep_enter_settings = NULL;
static pm_sleep_cfg_type* sleep_exit_settings = NULL;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/**
 * @name pm_rpm_sleep_init
 *
 * @brief Initializes the sleep settings LUT.
 *
 * @param  sleep_mode: can be vdd_min/XO shutdown
 *
 * @return None.
 *
 * @sideeffects None.
 *
 * @sideeffects Sleep settings should be available in config.
 *
 */
void pm_rpm_sleep_init()
{
    pm_chip_major_revision = pm_get_pmic_revision(0);

    sleep_enter_settings = (pm_sleep_cfg_type*)pm_target_information_get_specific_info(PM_PROP_SLEEP_ENTER_INFO);
    sleep_exit_settings = (pm_sleep_cfg_type*)pm_target_information_get_specific_info(PM_PROP_SLEEP_EXIT_INFO);
    CORE_VERIFY((sleep_enter_settings != NULL) && (sleep_exit_settings != NULL));
}

/**
 * @name pm_rpm_enter_sleep
 *
 * @brief This function calls the driver layer PMIC calls to set
 *        the Rails to the state prior to entering sleep.
 *
 * @param  sleep_mode: can be vdd_min/XO shutdown
 *
 * @return PMIC error code.
 *
 * @sideeffects Will over-write any client configuration
 *              requests. Any additions to the below function
 *              should be cautiously be added.
 *
 */
pm_err_flag_type
pm_rpm_enter_sleep (pm_sleep_mode_type sleep_mode)
{
    uint32 i = 0;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    const pm_sleep_cfg_type* p_settings;
    uint8 data = 0;

    if (sleep_mode > PM_SLEEP_MODE_VDD_MIN)
    {
      return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    p_settings = &sleep_enter_settings[(uint16)sleep_mode];

    /* Two config data arrays on enter sleep: XO_SHUTDOWN and VDD_MIN.
       "The VDD_MIN one has a copy of the XO_SHUTDOWN data most likely."   */
    while( (p_settings->p_cfg[i].regAddr != 0xFFFF) &&
          (err_flag == PM_ERR_FLAG__SUCCESS) )
    {
      err_flag |= pm_comm_write_byte( p_settings->p_cfg[i].slaveID,
                              p_settings->p_cfg[i].regAddr,
                              p_settings->p_cfg[i].data, 0 );
      i++;
    }

    //For PMIC 2.0 the SW Workaround #3 – Force
    //VMUX to LDO6 (CR-0000163990) has been moved to MPM
    if (pm_chip_major_revision == 1)
    {
       err_flag |= pm_comm_write_byte( 0, 0x5040, 0x02 , 1);
    }

    /* Read the value of XO_ADJ_FINE */
    err_flag |= pm_comm_read_byte(0, 0x505C, &data, 0);

    /* Copy value of  XO_ADJ_FINE to CLK_DIST_SPARE1 */
    err_flag |= pm_comm_write_byte(0, 0x5951, data, 0);

    /* Calculate ( XO_ADJ_FINE+2) & Copy value to CLK_DIST_SPARE2 */
    err_flag |= pm_comm_write_byte(0, 0x5952, data+2, 0);

    return err_flag;
}

/**
 * @name pm_rpm_exit_sleep
 *
 * @brief This function calls the driver layer PMIC calls to set
 *        the Rails back to the prior state of entering sleep.
 *
 * @param sleep_mode: can be vdd_min/XO shutdown
 *
 * @return PMIC error code.
 *
 * @sideeffects Will restore any client configuration requests.
 *              Any additions to the below function should be
 *              cautiously be added.
 *
 */
pm_err_flag_type
pm_rpm_exit_sleep (pm_sleep_mode_type sleep_mode)
{
    uint32 i = 0;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    const pm_sleep_cfg_type* p_settings;

    if (sleep_mode > PM_SLEEP_MODE_VDD_MIN)
    {
       return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    //For PMIC 2.0 the SW Workaround #3
    // Force VMUX back to Auto (CR-0000163990) has been moved to MPM
    if (pm_chip_major_revision == 1)
    {
       err_flag |= pm_comm_write_byte( 0, 0x5040, 0x00, 1);
    }

    p_settings = &sleep_exit_settings[(uint16)sleep_mode];

    /* Two config data arrays on exit sleep: XO_SHUTDOWN and VDD_MIN.
      "The VDD_MIN one has a copy of the XO_SHUTDOWN data most likely."  */
    while( (p_settings->p_cfg[i].regAddr != 0xFFFF) &&
          (err_flag == PM_ERR_FLAG__SUCCESS) )
    {
      err_flag |= pm_comm_write_byte( p_settings->p_cfg[i].slaveID,
                              p_settings->p_cfg[i].regAddr,
                              p_settings->p_cfg[i].data, 0) ;
      i++;
    }

    return err_flag;
}

void pm_rpm_platform_init(void)
{
    // Keeping this dummy as it might be required to add any platform specific changes. So retaining this API
}

