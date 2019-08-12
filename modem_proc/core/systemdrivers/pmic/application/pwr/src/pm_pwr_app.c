/*! \file pm_pwr_app.c
*  \n
*  \brief Pwr Application APIs implementation.
*  \n
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/pwr/src/pm_pwr_app.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/15   mr      Removed PMIC KW P1 errors (CR-789040)
05/20/14   rk      Provide PMIC API in MPSS to set voltage for Vdd_MSS Rail (CR-668036)
10/07/13   rh      File created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "DDITimetick.h"
#include "DALSys.h"

#include "hw_module_type.h"
#include "pm_pbs_client.h"
#include "pmapp_pwr.h"
#include "pm_target_information.h"
#include "pm_resources_and_types.h"
#include "pm_version.h"
#include "pm_npa.h"

#include "pm_ldo_driver.h"
#include "pm_smps_driver.h"

static pm_pwr_resource_info_type* mss_rail = NULL;

#define PM_STATUS_REG_POLL_INTERVAL             25    /* us time slice to poll the VREG status flag after */
#define PM_SMPS_ENABLE_MIN_SETTLE_TIME_US      120    /* SMPS min settling time to wait after turning ON the rail */
#define PM_SMPS_VOLT_MIN_SETTLE_TIME_US         25    /* SMPS min settling time to wait after voltage change on this rail */

/*
 * Structure to hold the Power rails Diag runtime variables
 */
static struct
{
    DalTimetickTime64Type     start_settling_time;                /**< start settling time */
    DalTimetickTime64Type     end_settling_time;                  /**< end settling time */
    uint64                    actual_settling_time_usec;          /**< actual settling time in usec */
    uint32                    mss_time_taken_to_settle;           /**< mss time taken to settle in usec */
    uint32                    calculated_settling_time_usec;      /**< calculated settling time based on formula in usec */
    uint32                    mss_settling_time_us;               /**< settling time if changed in different conditions in usec */
    uint32                    settling_loop_count;                /**< vreg_ok polling loop count */
    uint32                    vreg_ok_err_count;                   /**< vreg_ok error count */
    pm_err_flag_type          err_flag_vreg_ok;                   /**< vreg_ok error flag */
    pm_on_off_type            prev_rail_status;                   /**< prev_ON/OFF rail_status */
    pm_on_off_type            current_rail_status;                /**< current_ON/OFF rail_status */
    uint32                    prev_voltage;                       /**< previous voltage */
    uint32                    current_voltage;                    /**< current voltage request*/
    pm_err_flag_type          returned_err_flag;                  /**< current voltage request*/
    pm_pbs_client_type*       mss_pbs_level_trigger_sequence;
    pm_pbs_client_type*       mss_pbs_ocp_trigger_sequence;
    DalDeviceHandle           *timetick_handle;                   /**< handle for getting system time stamp */
} mss_rail_status;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pmapp_pwr_set_vdd_mss(uint32 voltage)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_err_flag_type err_flag_vreg_ok = PM_ERR_FLAG__SUCCESS;
    static uint8 run_once = 0;

    uint8 model = pm_get_pmic_model(0);
    static uint32  *mss_stepper_rate_ptr = NULL;
    static uint32 mss_stepper_rate = 0;
    static pm_pwr_data_type *pwr_data = NULL;
    static pm_comm_info_type *comm_ptr = NULL;
    static uint8 pmic_index = 0;
    pm_on_off_type vreg_ok_status = PM_OFF;
    pm_on_off_type vreg_enable_status = PM_OFF;
    pm_volt_level_type mss_prev_voltage = 0;
    const char* timetick_dev_str = "SystemTimer";

    if(run_once == 0)
    {
         DALSYS_memset(&mss_rail_status, 0, sizeof(mss_rail_status));
	
        /* Get MSS rail data from DAL CONFIG */
        mss_rail = (pm_pwr_resource_info_type*)pm_target_information_get_specific_info(PM_PROP_MSS);

        CORE_VERIFY_PTR(mss_rail);

        mss_rail_status.mss_pbs_level_trigger_sequence = (pm_pbs_client_type*)mss_rail->data1;
        mss_rail_status.mss_pbs_ocp_trigger_sequence = (pm_pbs_client_type*)mss_rail->data2;
        mss_stepper_rate_ptr = (uint32*) mss_rail->data3;

        CORE_VERIFY_PTR(mss_stepper_rate_ptr);
        mss_stepper_rate = *mss_stepper_rate_ptr;

        if ( ( mss_rail->resource_type == RPM_SMPS_B_REQ) || (mss_rail->resource_type == RPM_LDO_B_REQ) )
        {
              pmic_index = 1;
        }

        switch(mss_rail->resource_type)
        {
            pm_smps_data_type *smps_ptr = NULL;
            pm_ldo_data_type *ldo_ptr = NULL;

            // Apply supplied voltage to MSS rail. Note that in target config where we get the MSS rail info
            // from the regulator indices are 1-indexed while driver code expects 0-indexed resource ids.
            // Subtract resource_index by 1 to account for this.
            case RPM_SMPS_A_REQ:
            case RPM_SMPS_B_REQ:
                 smps_ptr = pm_smps_get_data(pmic_index);
                 if(smps_ptr != NULL)
                  {
                      pwr_data = &(smps_ptr->pm_pwr_data);
                      comm_ptr = smps_ptr->comm_ptr;
                  }
                  break;
            case RPM_LDO_A_REQ:
            case RPM_LDO_B_REQ:
                 ldo_ptr = pm_ldo_get_data(pmic_index);
                 if(ldo_ptr != NULL)
                  {
                      pwr_data = &(ldo_ptr->pm_pwr_data);
                      comm_ptr = ldo_ptr->comm_ptr;
                  }
                  break;
            default:
                  break;
        }

        CORE_VERIFY_PTR(pwr_data);
        CORE_VERIFY_PTR(comm_ptr);

        if (DAL_SUCCESS != DalTimetick_Attach(timetick_dev_str, &mss_rail_status.timetick_handle))
        {
            /* Update state to indicate DAL ERROR */
            CORE_VERIFY(0);
        }

        run_once = 1;
    }

    /* Get the VREG enable status status for MSS */
    err_flag |= pm_pwr_sw_enable_status_alg(pwr_data, comm_ptr, mss_rail->resource_index - 1, &vreg_enable_status);
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
        mss_rail_status.returned_err_flag = err_flag;
        DALSYS_LogEvent(0,DALSYS_LOGEVENT_FATAL_ERROR,
                        "DALLOG PMIC: Failed to read mss vreg_enable status");

        return err_flag;
    }

    mss_rail_status.prev_rail_status = vreg_enable_status;

    if(voltage == 0)
    {
        /* Disable MSS */
        err_flag |= pm_pwr_sw_enable_alg(pwr_data, comm_ptr, mss_rail->resource_index - 1, PM_OFF);
        mss_rail_status.returned_err_flag = err_flag;
        CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);
        return err_flag;
    }
    else
    {
        mss_rail_status.current_voltage = voltage;

        if( vreg_enable_status == PM_ON)   /* MSS Rail is already ON */
        {
            /* Get the volt level status for MSS rail */
            err_flag |= pm_pwr_volt_level_status_alg(pwr_data, comm_ptr, mss_rail->resource_index - 1,  &mss_prev_voltage);
            if (err_flag != PM_ERR_FLAG__SUCCESS)
            {
                mss_rail_status.returned_err_flag = err_flag;
                DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                                   "DALLOG PMIC: Failed to read mss volt level status");
            }

            mss_rail_status.prev_voltage = mss_prev_voltage;

            /* set new voltage level */
            err_flag |= pm_pwr_volt_level_alg(pwr_data, comm_ptr, mss_rail->resource_index - 1, voltage);
            mss_rail_status.returned_err_flag = err_flag;

            CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);
        }
        else     /* MSS Rail was OFF and should be turned on now */
        {
            mss_prev_voltage = 0;
            mss_rail_status.prev_voltage = mss_prev_voltage;

            /* set new voltage level */
            err_flag |= pm_pwr_volt_level_alg(pwr_data, comm_ptr, mss_rail->resource_index - 1, voltage);
            mss_rail_status.returned_err_flag = err_flag;
            CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);

            if(mss_rail_status.mss_pbs_ocp_trigger_sequence != NULL)
            {
                 /* MSS OCP Workaround : disable ocp, save mode, mode=pwm, Enable mss, enable ocp, restore mode */
                 err_flag |= pm_pbs_client_sw_trigger(0, *mss_rail_status.mss_pbs_ocp_trigger_sequence);
            }
            else
            {
                mss_rail_status.returned_err_flag = PM_ERR_FLAG__INVALID_POINTER;
                DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                                   "DALLOG PMIC: invalid pointer- mss_pbs_ocp_trigger_sequence ");

                return PM_ERR_FLAG__INVALID_POINTER;
            }
        }

        /* Calculate settling time if the voltage is increasing */
        if( voltage > mss_prev_voltage)
        {
            if(mss_rail_status.timetick_handle != NULL)
            {
                /* Settling start time in ticks. */
                DalTimetick_GetTimetick64(mss_rail_status.timetick_handle, &mss_rail_status.start_settling_time);
            }
            else
            {
                mss_rail_status.returned_err_flag = PM_ERR_FLAG__INVALID_POINTER;
                DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                                   "DALLOG PMIC: invalid pointer- timetick_handle ");

                return PM_ERR_FLAG__INVALID_POINTER;
            }

            mss_rail_status.calculated_settling_time_usec  = (voltage - mss_prev_voltage)/mss_stepper_rate;
            mss_rail_status.mss_settling_time_us = mss_rail_status.calculated_settling_time_usec;

            if( vreg_enable_status == PM_OFF) //mss rail was OFF
            {
                /* Since the minimum wait time is 120us, we make the below check and update accordingly */
                if(mss_rail_status.mss_settling_time_us < PM_SMPS_ENABLE_MIN_SETTLE_TIME_US)
                {
                   mss_rail_status.mss_settling_time_us =  PM_SMPS_ENABLE_MIN_SETTLE_TIME_US;
                }
            }
            else
            {
                if (mss_rail_status.mss_settling_time_us < PM_SMPS_VOLT_MIN_SETTLE_TIME_US)
                {
                    mss_rail_status.mss_settling_time_us  = PM_SMPS_VOLT_MIN_SETTLE_TIME_US;
                }
            }

            /* Wait for the calculated settling time */
            DALSYS_BusyWait(mss_rail_status.mss_settling_time_us);

            mss_rail_status.mss_time_taken_to_settle = mss_rail_status.mss_settling_time_us;

            /* Get the VREG_OK status for MSS */
            //pm_pwr_sw_enable_vreg_ok_status_alg(pwr_data, comm_ptr, mss_rail->resource_index - 1, &vreg_ok_status);

            /* Make sure MSS rail VREG_OK is set */
            while(vreg_ok_status == PM_OFF)
            {
                /* Get the VREG_OK status for MSS rail */
                err_flag_vreg_ok = pm_pwr_sw_enable_vreg_ok_status_alg(pwr_data, comm_ptr, mss_rail->resource_index - 1, &vreg_ok_status);

                mss_rail_status.settling_loop_count++;
                if (err_flag_vreg_ok != PM_ERR_FLAG__SUCCESS)
                {
                    vreg_ok_status = PM_OFF;
                    mss_rail_status.vreg_ok_err_count++;

                    mss_rail_status.err_flag_vreg_ok = err_flag_vreg_ok;
                }

                /* Wait for 25uS */
                DALSYS_BusyWait(PM_STATUS_REG_POLL_INTERVAL);

                mss_rail_status.mss_time_taken_to_settle += PM_STATUS_REG_POLL_INTERVAL;

                /*guard against infinite loop*/
                if ((vreg_ok_status != PM_ON) && (mss_rail_status.mss_time_taken_to_settle > mss_rail_status.mss_settling_time_us*10) )
                {
                    /* Settling end time in ticks. */
                    DalTimetick_GetTimetick64(mss_rail_status.timetick_handle, &mss_rail_status.end_settling_time);
                    DalTimetick_CvtFromTimetick64( mss_rail_status.timetick_handle,
                                                  (mss_rail_status.end_settling_time - mss_rail_status.start_settling_time),
                                                  T_USEC, &mss_rail_status.actual_settling_time_usec );

                    mss_rail_status.current_rail_status = PM_OFF;
                    CORE_VERIFY(0);
                }
            }

            mss_rail_status.current_rail_status = vreg_ok_status;

            /* Settling end time in ticks. */
            DalTimetick_GetTimetick64(mss_rail_status.timetick_handle, &mss_rail_status.end_settling_time);
            DalTimetick_CvtFromTimetick64( mss_rail_status.timetick_handle,
                                           (mss_rail_status.end_settling_time - mss_rail_status.start_settling_time),
                                           T_USEC, &mss_rail_status.actual_settling_time_usec );
        }    // end of settling time
        else
        {
            /* Settling time not required as voltage is decreasing */
            mss_rail_status.current_rail_status = vreg_ok_status;
        }

        /* PMD9635 needs PBS client from config to be triggered after setting MSS rail */
        /* Not required if rail is turned off */
        if(err_flag == PM_ERR_FLAG__SUCCESS && model == PMIC_IS_PMD9635)
        {
            if(mss_rail_status.mss_pbs_level_trigger_sequence == NULL)
            {
                mss_rail_status.returned_err_flag = PM_ERR_FLAG__INVALID_POINTER;
                DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                                   "DALLOG PMIC: invalid pointer- mss_pbs_level_trigger_sequence ");

                return PM_ERR_FLAG__INVALID_POINTER;
            }
            else
            {
                err_flag |= pm_pbs_client_sw_trigger(0, *mss_rail_status.mss_pbs_level_trigger_sequence);
                if (err_flag != PM_ERR_FLAG__SUCCESS)
                {
                    mss_rail_status.returned_err_flag = err_flag;
                    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                                   "DALLOG PMIC: pm_pbs_client_sw_trigger returned error ");

                    return err_flag;
                }
            }
        }
    } // voltage > 0

    mss_rail_status.returned_err_flag = err_flag;

    return err_flag;
}