/*! \file pm_pbs_client_driver.c
*  \n
*  \brief PBS Client driver APIs implemention.
*  \n
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/pbs/src/pm_pbs_client_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/13   kt      Initial version
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_pbs_client_driver.h"
#include "CoreVerify.h"
#include "hw_module_type.h"
#include "pm_malloc.h"
#include "device_info.h"


/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

/* Static global variable to store the PBS data */
static pm_pbs_client_data_type *pm_pbs_client_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

/* PBS Client register offsets */
#define PMIO_PBS_CLIENT_INT_RT_STS_ADDR                                   0x0010
#define PMIO_PBS_CLIENT_INT_SET_TYPE_ADDR                                 0x0011
#define PMIO_PBS_CLIENT_INT_POLARITY_HIGH_ADDR                            0x0012
#define PMIO_PBS_CLIENT_INT_POLARITY_LOW_ADDR                             0x0013
#define PMIO_PBS_CLIENT_INT_LATCHED_CLR_ADDR                              0x0014
#define PMIO_PBS_CLIENT_INT_EN_SET_ADDR                                   0x0015
#define PMIO_PBS_CLIENT_INT_EN_CLR_ADDR                                   0x0016
#define PMIO_PBS_CLIENT_INT_LATCHED_STS_ADDR                              0x0018
#define PMIO_PBS_CLIENT_INT_PENDING_STS_ADDR                              0x0019
#define PMIO_PBS_CLIENT_TRIG_CFG_ADDR                                     0x0040
#define PMIO_PBS_CLIENT_TRIG_CTL_ADDR                                     0x0042
#define PMIO_PBS_CLIENT_EN_CTL_ADDR                                       0x0046

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/* Internal PBS Client driver data init function */
static void pm_pbs_client_driver_data_init(pm_pbs_client_data_type *pbs_client_ptr, peripheral_info_type *peripheral_info);

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_pbs_client_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info,uint8 pmic_index)
{
    pm_pbs_client_data_type *pbs_client_ptr = NULL;

    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    if (peripheral_info->peripheral_type != PM_HW_MODULE_PBS ||
        peripheral_info->peripheral_subtype != PM_HW_MODULE_PBS_CLIENT)
    {
        return;
    }

    pbs_client_ptr = pm_pbs_client_data_arr[pmic_index];

    if (pbs_client_ptr == NULL)
    {
        pm_malloc( sizeof(pm_pbs_client_data_type), (void**)&pbs_client_ptr);

        /* Assign Comm ptr */
        pbs_client_ptr->comm_ptr = comm_ptr;

        /* Save PBS Client Common and Register info */
        pm_pbs_client_driver_data_init(pbs_client_ptr, peripheral_info);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(pbs_client_ptr->pbs_client_common->num_peripherals != 0);

        pm_pbs_client_data_arr[pmic_index] = pbs_client_ptr;
    }
    else
    {
        pbs_client_ptr->pbs_client_common->num_peripherals++;
    }
}

void pm_pbs_client_driver_data_init(pm_pbs_client_data_type *pbs_client_ptr, peripheral_info_type *peripheral_info)
{
    pm_pbs_client_common_info_type *pbs_client_common_info = NULL;
    pm_pbs_client_register_info_type *pbs_client_reg_info = NULL;

    pm_malloc(sizeof(pm_pbs_client_common_info_type), (void**)&pbs_client_common_info);
    pm_malloc(sizeof(pm_pbs_client_register_info_type), (void**)&pbs_client_reg_info);

    /* PBS Client Common Info */
    pbs_client_common_info->base_address = peripheral_info->base_address;
    pbs_client_common_info->num_peripherals = 1;

    /* PBS Client Register Info */
    pbs_client_reg_info->int_rt_status = PMIO_PBS_CLIENT_INT_RT_STS_ADDR;
    pbs_client_reg_info->int_set_type = PMIO_PBS_CLIENT_INT_SET_TYPE_ADDR;
    pbs_client_reg_info->int_polarity_high = PMIO_PBS_CLIENT_INT_POLARITY_HIGH_ADDR;
    pbs_client_reg_info->int_polarity_low = PMIO_PBS_CLIENT_INT_POLARITY_LOW_ADDR;
    pbs_client_reg_info->int_latched_clr = PMIO_PBS_CLIENT_INT_LATCHED_CLR_ADDR;
    pbs_client_reg_info->int_en_set = PMIO_PBS_CLIENT_INT_EN_SET_ADDR;
    pbs_client_reg_info->int_en_clr = PMIO_PBS_CLIENT_INT_EN_CLR_ADDR;
    pbs_client_reg_info->int_latched_status = PMIO_PBS_CLIENT_INT_LATCHED_STS_ADDR;
    pbs_client_reg_info->int_pending_status = PMIO_PBS_CLIENT_INT_PENDING_STS_ADDR;
    pbs_client_reg_info->trig_cfg = PMIO_PBS_CLIENT_TRIG_CFG_ADDR;
    pbs_client_reg_info->trig_ctl = PMIO_PBS_CLIENT_TRIG_CTL_ADDR;
    pbs_client_reg_info->en_ctl1 = PMIO_PBS_CLIENT_EN_CTL_ADDR;

    /* Save the Info in resource */
    pbs_client_ptr->pbs_client_common = pbs_client_common_info;
    pbs_client_ptr->pbs_client_reg_table = pbs_client_reg_info;
}

pm_pbs_client_data_type* pm_pbs_client_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_pbs_client_data_arr[pmic_index];
    }

    return NULL;
}

uint8 pm_pbs_client_get_num_peripherals(uint8 pmic_index)
{
  if( (pmic_index < PM_MAX_NUM_PMICS) &&
      (pm_pbs_client_data_arr[pmic_index] != NULL))
  {
      return pm_pbs_client_data_arr[pmic_index]->pbs_client_common->num_peripherals;
  }

  return NULL;
}

