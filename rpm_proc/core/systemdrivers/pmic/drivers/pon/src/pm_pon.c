/*! \file pm_pon.c
*  \n
*  \brief This file contains the implementation of the public APIs for PMIC PON peripheral.
*  \n
*  &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/drivers/pon/src/pm_pon.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/13   kt      Added PON Reasons and IRQ related APIs
01/24/13   umr     Add new version support.
11/20/12   umr     Created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_pon.h"
#include "pm_pon_driver.h"
#include "pm_utils.h"
#include "pm_comm.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/

#define PON_RESET_CFG_WARM_RESET_VAL                      0x01
#define PON_RESET_CFG_IMMEDIATE_XVDD_SHUTDOWN_VAL         0x02
#define PON_RESET_CFG_NORMAL_SHUTDOWN_VAL                 0x04
#define PON_RESET_CFG_D_VDD_SHUTDOWN_VAL                  0x05
#define PON_RESET_CFG_X_VDD_SHUTDOWN_VAL                  0x06
#define PON_RESET_CFG_HARD_RESET_VAL                      0x07

#define PON_RESET_CFG_DVDD_HARD_RESET_VAL                 0x08
#define PON_RESET_CFG_XVDD_HARD_RESET_VAL                 0x09
#define PON_RESET_CFG_WARM_RESET_AND_DVDD_SHUTDOWN_VAL    0x0A
#define PON_RESET_CFG_WARM_RESET_AND_XVDD_SHUTDOWN_VAL    0x0B
#define PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN_VAL         0x0C
#define PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET_VAL      0x0D
#define PON_RESET_CFG_WARM_RESET_THEN_DVDD_HARD_RESET_VAL 0x0E
#define PON_RESET_CFG_WARM_RESET_THEN_XVDD_HARD_RESET_VAL 0x0F

/* Max PMIC Watchdog S1 and S2 timer value in seconds */
#define PON_WDOG_MAX_S1_S2_TIMER_VAL           127

/* Maximum Delay time between the pet for 5 sleep clock cycles in us. */
#define PM_WDOG_PET_MAX_DELAY_USEC             150

/*===========================================================================

                     STATIC and TYPE DEFINITIONS

===========================================================================*/

static uint64 pm_wdog_pet_curr_tick = 0;
static uint64 pm_wdog_pet_prev_tick = 0;

/*===========================================================================

                     API IMPLEMENTATION

===========================================================================*/

pm_err_flag_type pm_pon_wdog_cfg(uint8 pmic_index, uint32 s1_timer, uint32 s2_timer, pm_pon_reset_cfg_type reset_cfg_type)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pon_data_type *pon_ptr = pm_pon_get_data(pmic_index);
    pm_register_data_type wdog_enable = 0;
    pm_register_data_type reset_val = 0;
    
    if (pon_ptr == NULL) 
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (s1_timer > PON_WDOG_MAX_S1_S2_TIMER_VAL)
    {
       err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else if (s2_timer > PON_WDOG_MAX_S1_S2_TIMER_VAL)
    {
       err_flag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
    else if (reset_cfg_type >= PM_PON_RESET_CFG_INVALID)
    {
       err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
    }
    else
    {
       err_flag = pm_comm_read_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR, 0x80, &wdog_enable,0);

       if (err_flag != PM_ERR_FLAG__SUCCESS)
       {
          return err_flag;
       }

       /* Disable PMIC Wdog if already enabled before configuring the Wdog */
       if (wdog_enable)
       {
          /* Disable PMIC Wdog */
          err_flag = pm_comm_write_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR, 0x80, 0x00, 0);

          if (err_flag != PM_ERR_FLAG__SUCCESS)
          {
             return err_flag;
          }

          /* Requires 10 sleep clock cycles of delay */
          DALSYS_BusyWait(300);
       }

       /* Configure S1 timer */
       err_flag = pm_comm_write_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_PMIC_WD_RESET_S1_TIMER_ADDR, 0x7F, (uint8)s1_timer, 0);

       if (err_flag != PM_ERR_FLAG__SUCCESS)
       {
          return err_flag;
       }

       /* Configure S2 timer */
       err_flag = pm_comm_write_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_PMIC_WD_RESET_S2_TIMER_ADDR, 0x7F, (uint8)s2_timer, 0);

       if (err_flag != PM_ERR_FLAG__SUCCESS)
       {
          return err_flag;
       }

       switch (reset_cfg_type)
       {
          case PM_PON_RESET_CFG_WARM_RESET:
             reset_val = PON_RESET_CFG_WARM_RESET_VAL;
             break;

          case PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
             reset_val = PON_RESET_CFG_IMMEDIATE_XVDD_SHUTDOWN_VAL;
             break;

          case PM_PON_RESET_CFG_NORMAL_SHUTDOWN:
             reset_val = PON_RESET_CFG_NORMAL_SHUTDOWN_VAL;
             break;

          case PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN:
             reset_val = PON_RESET_CFG_D_VDD_SHUTDOWN_VAL;
             break;

          case PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
             reset_val = PON_RESET_CFG_X_VDD_SHUTDOWN_VAL;
             break;

          case PM_PON_RESET_CFG_HARD_RESET:
             reset_val = PON_RESET_CFG_HARD_RESET_VAL;
             break;

          case PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET:
             reset_val = PON_RESET_CFG_DVDD_HARD_RESET_VAL;
             break;

          case PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET:
             reset_val = PON_RESET_CFG_XVDD_HARD_RESET_VAL;
             break;

          case PM_PON_RESET_CFG_WARM_RESET_AND_D_VDD_BATT_REMOVE_SHUTDOWN:
             reset_val = PON_RESET_CFG_WARM_RESET_AND_DVDD_SHUTDOWN_VAL;
             break;

          case PM_PON_RESET_CFG_WARM_RESET_AND_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
             reset_val = PON_RESET_CFG_WARM_RESET_AND_XVDD_SHUTDOWN_VAL;
             break;

          case PM_PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN:
             reset_val = PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN_VAL;
             break;

          case PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET:
             reset_val = PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET_VAL;
             break;

          case PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET:
             reset_val = PON_RESET_CFG_WARM_RESET_THEN_DVDD_HARD_RESET_VAL;
             break;

          case PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET:
             reset_val = PON_RESET_CFG_WARM_RESET_THEN_XVDD_HARD_RESET_VAL;
             break;

          default:
             /* We should never get here as the check is already made */
             return PM_ERR_FLAG__PAR5_OUT_OF_RANGE;
       }

       /* Configure reset value */
       err_flag = pm_comm_write_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_PMIC_WD_RESET_S2_CTL_ADDR, 0x0F, reset_val, 0);

       if (err_flag != PM_ERR_FLAG__SUCCESS)
       {
          return err_flag;
       }

       /* Enable PMIC Wdog back if it was already enabled */
       if (wdog_enable)
       {
          /* Enable PMIC Wdog */
          err_flag = pm_comm_write_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR, 0x80, 0xFF, 0);

          if (err_flag != PM_ERR_FLAG__SUCCESS)
          {
             return err_flag;
          }
       }
    }

    return err_flag;
}

pm_err_flag_type pm_pon_wdog_enable(uint8 pmic_index, pm_on_off_type enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pon_data_type *pon_ptr = pm_pon_get_data(pmic_index);
    pm_register_data_type wdog_enable = 0;

    if(pon_ptr == NULL) 
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
       wdog_enable = (enable == PM_ON) ? 0xFF : 0;

       /* Enable or Disable PMIC Wdog */
       err_flag = pm_comm_write_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR, 0x80, wdog_enable, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pon_wdog_pet(uint8 pmic_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pon_data_type *pon_ptr = pm_pon_get_data(pmic_index);
    /* Requires to wait for 5 clocks cycles between pets.*/
    uint32 delay_time = PM_WDOG_PET_MAX_DELAY_USEC;

    if(pon_ptr == NULL) 
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
       /* Store last pet time. */
       pm_wdog_pet_prev_tick = pm_wdog_pet_curr_tick;
       /* Get current pet time. */
       pm_wdog_pet_curr_tick = time_service_now();

       /* If this is the first pet, delay for the whole 150us
          else only delay a needed time. */
       if (pm_wdog_pet_prev_tick)
       {
          /* Delay time in us */
          delay_time = pm_convert_timetick_to_time(pm_wdog_pet_curr_tick - pm_wdog_pet_prev_tick);
          if (delay_time <= PM_WDOG_PET_MAX_DELAY_USEC)
          {
            /* delay time between 0..150us */
            delay_time = PM_WDOG_PET_MAX_DELAY_USEC - delay_time;
          }
          else
          {
             /* elsapse time > 150us, no need to wait */
             delay_time = 0;
          }
       }
       else
       {
         /* No need to wait for the first pet. */
         delay_time = 0;
       }

       /* We must for 150us before the next pet happens. */
       DALSYS_BusyWait(delay_time);

       /* Writing 0x1 to the wdog_pet addr clears the PMIC Watchdog timer */
       err_flag = pm_comm_write_byte(pon_ptr->comm_ptr->slave_id, PMIO_PON_PMIC_WD_RESET_PET_ADDR, 0x01, 0);

    }

    return err_flag;
}

pm_err_flag_type
pm_pon_wdog_get_cfg
(
  uint8 pmic_index,
  uint8* s1_timer,
  uint8* s2_timer,
  uint8* reset_type
)
{
    pm_pon_data_type *pon_ptr = pm_pon_get_data(pmic_index);
    pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type reg_data[4] = {0};

    if (pon_ptr == NULL) 
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (!s1_timer || !s2_timer || !reset_type)
    {
       err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        /* Read wdog config. */
        err_flag = pm_comm_read_byte_array(pon_ptr->comm_ptr->slave_id,
                                        PMIO_PON_PMIC_WD_RESET_S1_TIMER_ADDR,
                                        4, reg_data, 0);
        *s1_timer = reg_data[0];
        *s2_timer = reg_data[1];
        *reset_type = reg_data[2];
    }

    return err_flag;
}

pm_err_flag_type pm_pon_qdss_set_flag(uint8 pmic_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pon_data_type *pon_ptr = pm_pon_get_data(pmic_index);
  
    if(pon_ptr == NULL) 
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
       /* Writing 0x02 to the DVDD_SPARE reg bit<1> to set the QDSS debug flag */
       err_flag = pm_comm_write_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_DVDD_RB_SPARE_ADDR, 0x02, 0xFF, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pon_qdss_clear_flag(uint8 pmic_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pon_data_type *pon_ptr = pm_pon_get_data(pmic_index);
    
    if(pon_ptr == NULL) 
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
       /* Writing 0x00 to the DVDD_SPARE reg bit<1> to clear the QDSS debug flag */
       err_flag = pm_comm_write_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_DVDD_RB_SPARE_ADDR, 0x02, 0x00, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pon_qdss_get_flag(uint8 pmic_index, boolean *status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pon_data_type *pon_ptr = pm_pon_get_data(pmic_index);
    pm_register_data_type data = 0;

    if(pon_ptr == NULL) 
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (status == NULL)
    {
       err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        /* Read the DVDD_SPARE reg bit<1> to get the QDSS debug flag status */
       err_flag = pm_comm_read_byte_mask(pon_ptr->comm_ptr->slave_id, PMIO_PON_DVDD_RB_SPARE_ADDR, 0x02, &data,0);

       *status = data ? TRUE : FALSE;
    }

    return err_flag;
}

