/*! \file pm_droop_detect.c
 *  \n
 *  \brief This file contains implementation for PMIC RPM droop detection functions.
 *  \n  
 *  \n &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/src/pm_droop_detect.c#1 $  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/22/14   kt      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_droop_detect.h"

/*===========================================================================

                        STATIC AND GLOBAL VARIABLES

===========================================================================*/

// Droop detection debug info: time stamp, slave_id and periph_id
pm_droop_detect_info_type pm_droop_detect_info[PM_DROOP_LOG_ARRAY_SIZE];

// Droop detect isr hit count
static uint32 pm_droop_detect_count = 0;

// Isr prototype 
static void* pm_droop_detect_isr (uint32 isr_ctx, uint32 irq_mask);

/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_droop_detect_irq_enable(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, 
                                            uint8 resource_index, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pwr_irq_type irq_type = PM_PWR_IRQ_VREG_OK;

    if (PM_HW_MODULE_FTS == pwr_data->pwr_specific_info[resource_index].periph_type)
    {
       /* For FTS 2.5, irq type is VREG_ERROR */
       irq_type = PM_PWR_IRQ_VREG_ERROR;
    }

    /* Clear the VREG_OK/VREG_ERROR interrupt */
    err_flag = pm_pwr_irq_clear_alg(pwr_data, comm_ptr, resource_index, irq_type);

    /* Enable/disable the VREG_OK/VREG_ERROR interrupt */
    err_flag |= pm_pwr_irq_enable_alg(pwr_data, comm_ptr, resource_index, irq_type, enable); 
    
    return err_flag;
}


pm_err_flag_type pm_droop_detect_irq_config(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, 
                                            uint8 resource_index, uint32 isr_ctx)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pwr_irq_type irq_type = PM_PWR_IRQ_VREG_OK;
    pm_irq_trigger_type trig_type = PM_IRQ_TRIGGER_FALLING_EDGE;

    if (PM_HW_MODULE_FTS == pwr_data->pwr_specific_info[resource_index].periph_type)
    {
       /* For FTS 2.5, irq type is VREG_ERROR and trigger type is RISING_EDGE */
       irq_type = PM_PWR_IRQ_VREG_ERROR;
       trig_type = PM_IRQ_TRIGGER_RISING_EDGE;
    }

    /* Clear the VREG_OK/VREG_ERROR interrupt */
    err_flag = pm_pwr_irq_clear_alg(pwr_data, comm_ptr, resource_index, irq_type);

    /* Disable the VREG_OK/VREG_ERROR interrupt */
    err_flag |= pm_pwr_irq_enable_alg(pwr_data, comm_ptr, resource_index, irq_type, FALSE); 

    /* Set the power rails's VREG_OK/VREG_ERROR interrupt polarity to FALLING/RISING_EDGE */
    err_flag |= pm_pwr_irq_set_trigger_alg(pwr_data, comm_ptr, resource_index, irq_type, trig_type);

    /* Register the ISR to the power rails's VREG_OK/VREG_ERROR interrupt */
    err_flag |= pm_pwr_irq_register_isr_alg(pwr_data, comm_ptr, resource_index, irq_type, (void*)&pm_droop_detect_isr, isr_ctx); 
    
    return err_flag;
}

static void* pm_droop_detect_isr (uint32 isr_ctx, uint32 irq_mask)
{
   static uint8 pm_droop_detect_arr_index = 0;

   uint32 slave_id = 0;
   uint32 periph_id = 0;
   uint16 addr = 0;
   uint8 data = 0;

   if(pm_droop_detect_arr_index >= PM_DROOP_LOG_ARRAY_SIZE)
   {
      // reset the droop detect array index
      pm_droop_detect_arr_index = 0;
   }

   // isr_ctx (periph_id <0:15> slave_id <16:31>)
   slave_id = ((isr_ctx>>16) & 0xFFFF);
   periph_id = (isr_ctx & 0xFFFF);

   // save the time_stamp, slave_id and periph_id
   pm_droop_detect_info[pm_droop_detect_arr_index].time_stamp = timetick_get64();
   pm_droop_detect_info[pm_droop_detect_arr_index].slave_id = slave_id;
   pm_droop_detect_info[pm_droop_detect_arr_index].periph_id = periph_id;


   // increment the droop detect array index
   pm_droop_detect_arr_index++;

   // increment the isr hit count
   pm_droop_detect_count++;
   
   // get the addr and data to clear the interrupt latched status   
   addr = (uint16)(((periph_id<<8) & 0xFF00) + PM_INT_LATCHED_CLR_REG_OFFSET);
   data = (uint8)(irq_mask & 0xFF);

   // clear the interrupt latched status
   pm_comm_write_byte(slave_id, addr, data, 0);

   return NULL;
}
