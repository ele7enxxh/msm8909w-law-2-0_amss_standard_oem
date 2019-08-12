#ifndef PM_TARGET_INFORMATION_H
#define PM_TARGET_INFORMATION_H

/*! \file
*  \n
*  \brief  pm_target_information.h
*  \details
*  \n &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/framework/inc/pm_target_information.h#1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_dal_prop_ids.h"
#include "pm_comm.h"


/*===========================================================================

                     FUNCTION DECLARATION

===========================================================================*/

/*! \brief This function initializes the target/PMIC related information.
 *  \param[in] None
 *
 *  \return None.
 *
 *  <b>Dependencies</b>
 *  \li DAL framework.
 */
void pm_target_information_init(void);

/*! \brief This function returns the common props information for the parameter specified property id.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return A pointer to the information.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
void* pm_target_information_get_common_info(uint32 prop_id);

/*! \brief This function returns the target specific props information for the parameter specified property id.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return A pointer to the information.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
void* pm_target_information_get_specific_info(uint32 prop_id );

/*! \brief This function returns the target specific props count info for the parameter specified property id.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return count value.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
uint32 pm_target_information_get_periph_count_info(uint32 prop_id, uint8 pmic_index);

/*! \brief This function reads the parameter specified peripheral information.
 *  \param[in] comm_ptr The Comm Channel.
 *
 *  \param[in] peripheral_info A pointer to where the peripheral information is going to be saved.
 *
 *  \return Error flag.
 *
 *  <b>Dependencies</b>
 *  \li pm_processor_init_comms() .
 */
pm_err_flag_type pm_target_information_read_peripheral_rev(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info);

#endif //PM_TARGET_INFORMATION_H


