#ifndef PM_LDO__H
#define PM_LDO__H

/**
@file pm_ldo.h

     This header file contains enums and API definitions for PMIC LDO.
     power rail driver.

Copyright (c) 2009-2014  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_ldo.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/14   leo	   (Tech Pubs) Edited/added Doxygen comments and markup.
12/06/12   hw      Rearchitecturing module driver to peripheral driver
=============================================================================*/

/*===========================================================================

                 HEADER FILE INCLUDE

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/** @addtogroup pm_ldo
@{ */

/** @anonenum{ldo_peripheral_index} @xreflabel{hdr:ldoPeripheralIndex}
    Contains all of the required LDO regulators.
 */
enum
{
  PM_LDO_1,       /**< LDO 1. */
  PM_LDO_2,       /**< LDO 2. */
  PM_LDO_3,       /**< LDO 3. */
  PM_LDO_4,       /**< LDO 4. */
  PM_LDO_5,       /**< LDO 5. */
  PM_LDO_6,       /**< LDO 6. */
  PM_LDO_7,       /**< LDO 7. */
  PM_LDO_8,       /**< LDO 8. */
  PM_LDO_9,       /**< LDO 9. */
  PM_LDO_10,      /**< LDO 10. */
  PM_LDO_11,      /**< LDO 11. */
  PM_LDO_12,      /**< LDO 12. */
  PM_LDO_13,      /**< LDO 13. */
  PM_LDO_14,      /**< LDO 14. */
  PM_LDO_15,      /**< LDO 15. */
  PM_LDO_16,      /**< LDO 16. */
  PM_LDO_17,      /* LDO 17. */
  PM_LDO_18,      /* LDO 18. */
  PM_LDO_19,      /* LDO 19. */
  PM_LDO_20,      /* LDO 20. */
  PM_LDO_21,      /* LDO 21. */
  PM_LDO_22,      /* LDO 22. */
  PM_LDO_23,      /* LDO 23. */
  PM_LDO_24,      /* LDO 24. */
  PM_LDO_25,      /* LDO 25. */
  PM_LDO_26,      /* LDO 26. */
  PM_LDO_27,      /* LDO 27. */
  PM_LDO_28,      /* LDO 28. */
  PM_LDO_29,      /* LDO 29. */
  PM_LDO_30,      /* LDO 30. */
  PM_LDO_31,      /* LDO 31. */
  PM_LDO_32,      /* LDO 32. */
  PM_LDO_33,      /* LDO 33. */
  PM_LDO_INVALID  /**< Invalid LDO. */
};

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/** Switches between NPM, LPM, and other modes of a regulator.

    @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
    
    @param[in] ldo_peripheral_index LDO peripheral index. Starts from 0
                                    (for the first LDO peripheral).
                                    See @xnameref{hdr:ldoPeripheralIndex}.

    @param[in] sw_mode              Selects the mode of a regulator, e.g.,
                                    HPM, LPM, BYPASS. See #pm_sw_mode_type.
 
    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_ldo_sw_mode
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type sw_mode);

/**
 * Gets the status of the modes of a regulator.
 * 
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *   
 * @param[in] ldo_peripheral_index LDO peripheral index. Starts from 0
 *                                 (for the first LDO peripheral).
 *                                 See @xnameref{hdr:ldoPeripheralIndex}.
 *
 * @param[out] sw_mode             Pointer to the mode of a regulator, e.g.,
 *                                 HPM, LPM, BYPASS. See #pm_sw_mode_type.
 * 
 * @return 
 * SUCCESS or Error -- See #pm_err_flag_type.
 */

pm_err_flag_type pm_ldo_sw_mode_status
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_sw_mode_type *sw_mode);

/**
 * Gets the register value (MODE_CTL) at an	instance.
 *
 * @note1hang The mode of a regulator changes dynamically.
 *
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *   
 * @param[in] ldo_peripheral_index LDO peripheral index. Starts from 0
 *                                 (for the first LDO peripheral).
 *                                 See @xnameref{hdr:ldoPeripheralIndex}.
 *
 * @param[out] mode                Pointer to the mode_control register
 *                                 value of a regulator.
 *
 * @return 
 * SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_ldo_sw_mode_status_raw
(uint8 pmic_chip, uint8 ldo_peripheral_index, uint8* mode);

/**
   Enables or disables a regulator or voltage switcher.

    @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] ldo_peripheral_index LDO peripheral index. The first LDO
                                    peripheral starts at 0. See
                                    @xnameref{hdr:ldoPeripheralIndex}.

    @param[in] on_off               Turns the regulator on and off.
                                    See #pm_on_off_type.
 
    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_ldo_sw_enable
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type on_off);

/** Returns software enable status of the LDOs.

    @param[in]  pmic_chip Primary PMIC: 0, Secondary PMIC: 1.
    
    @param[in]  ldo_peripheral_index LDO peripheral index.
                Starts from 0 (for the first LDO peripheral).
                See @xnameref{hdr:ldoPeripheralIndex}.

    @param[in] on_off Pointer to the on/off status of the regulator.
 
    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_ldo_sw_enable_status
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type* on_off);

/** Returns the VREG_OK status of LDOs.

    @param[in]  pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
    
    @param[in]  ldo_peripheral_index LDO peripheral index. Starts from 0
                                     (for the first LDO peripheral).
                                     See @xnameref{hdr:ldoPeripheralIndex}.

    @param[in]  on_off               Pointer to return the VREG_OK status.
 
    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_ldo_vreg_ok_status
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_on_off_type* on_off);

/** Gets VSET for an LDO.

    @param[in]  pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
    
    @param[in]  ldo_peripheral_index LDO peripheral index. Starts from 0
                                     (for the first LDO peripheral).
                                     See @xnameref{hdr:ldoPeripheralIndex}.

    @param[in]  volt_level           Voltage level status in microvolts (uint32).

	@param[out] vset                 Deduced voltage register value using
                                     volt_level.

    @return 
    SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_ldo_calculate_vset
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type volt_level, uint32* vset); 

/**
 * Sets the voltage level for an LDO.
 * 
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] ldo_peripheral_index LDO peripheral index. Starts from 0
 *                                 (for the first LDO peripheral).
 *                                 See @xnameref{hdr:ldoPeripheralIndex}.
 *
 * @param[in] volt_level           Voltage level.
 * 
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */

pm_err_flag_type pm_ldo_volt_level
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type volt_level);

/**
 *  Gets the voltage level status for an LDO.
 * 
 * @param[in] pmic_chip            Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] ldo_peripheral_index LDO peripheral index. Starts from 0
 *                                 (for the first LDO peripheral).
 *                                 See @xnameref{hdr:ldoPeripheralIndex}.
 *
 * @param[out] volt_level          Pointer to the voltage level.
 * 
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_ldo_volt_level_status
(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type *volt_level);

/** @} */ /* end_addtogroup pm_ldo */

#endif /* PM_LDO__H */

