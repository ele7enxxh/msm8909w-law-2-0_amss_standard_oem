/*===========================================================================

                       QFPROM  Driver Source  Code

DESCRIPTION
 Contains mpss specific defintions and APIs to setup clock/pmic
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright  2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/boot/qfprom/hw/core_2_0/qfprom_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/12   kedara  Initial revision for 9x25

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "DDIClock.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include QFPROM_TARGET_INCLUDE_H
#include "npa.h"
#include "pm.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#define QFPROM_SEC_CTRL_CLK_FREQ_KHZ 19200

static DalDeviceHandle *qfprom_clk_handle = NULL;
static boolean qfprom_sec_ctrl_clk_updated = FALSE;
ClockIdType clock_Id;

/*=============================================================================
                                 LOCAL FUNCTION DEFINITIONS
=============================================================================*/

/*---------------------------------------------------------------------------
  NPA handles for setting SFPB clock and QFPROM_BLOW_VDD
---------------------------------------------------------------------------*/
npa_client_handle handle_qfuse = NULL;

/*=============================================================================
                                 GLOBAL FUNCTION DEFINITIONS
=============================================================================*/


/*===========================================================================

FUNCTION  QFPROM_CLOCK_ENABLE

DESCRIPTION
  Enable/setup clock for qfprom fuse block.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32  Any Errors while enabling qfprom block clock.

SIDE EFFECTS
  Turns on qfprom clock.

===========================================================================*/
uint32 qfprom_clock_enable( void )
{
  uint32 ret_val = QFPROM_CLOCK_SETTINGS_INVALID_ERR, result_freq = 0x0;
  DALResult dwResult = DAL_SUCCESS;
  
  do
  {
    dwResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &qfprom_clk_handle);
    if (DAL_SUCCESS == dwResult)
    {
      dwResult = DalClock_GetClockId(qfprom_clk_handle,"gcc_sec_ctrl_clk",
                                     &clock_Id);
      if (DAL_SUCCESS != dwResult)
      {
        break;
      }
      
      dwResult = DalClock_SetClockFrequency(qfprom_clk_handle, clock_Id,
                                           (QFPROM_BLOW_TIMER_CLK_FREQ_MHZ * 1000),
                                            CLOCK_FREQUENCY_KHZ_EXACT,
                                            &result_freq );
      if ((DAL_SUCCESS != dwResult) ||
           ((QFPROM_BLOW_TIMER_CLK_FREQ_MHZ * 1000) != result_freq))
      {
        break;
      }

      /*gcc_sec_ctrl_clk is assumed to be ON by default */
        ret_val = QFPROM_NO_ERR;
        qfprom_sec_ctrl_clk_updated = TRUE;
            
    }   
  }while(0);

  return ret_val;
}

/*===========================================================================

FUNCTION  QFPROM_CLOCK_DISABLE

DESCRIPTION
  Disables /Restores to previous state the clock for qfprom fuse block

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32  Any Errors while disabling qfprom block clock.

SIDE EFFECTS
  Disables /Restores to previous state the clock for qfprom fuse block.

===========================================================================*/
uint32 qfprom_clock_disable( void )
{
  uint32 ret_val = QFPROM_NO_ERR, result_freq = 0x0;
  DALResult dwResult = DAL_SUCCESS;
 
    do
    {
      if(qfprom_sec_ctrl_clk_updated == TRUE)
      {
        dwResult = DalClock_SetClockFrequency(qfprom_clk_handle, clock_Id,
                                            QFPROM_SEC_CTRL_CLK_FREQ_KHZ,
                                            CLOCK_FREQUENCY_KHZ_EXACT,
                                            &result_freq );
        if ((DAL_SUCCESS != dwResult) ||
           (QFPROM_SEC_CTRL_CLK_FREQ_KHZ != result_freq))
        {
          ret_val = QFPROM_CLOCK_SETTINGS_INVALID_ERR;
		  break;
        }
        qfprom_sec_ctrl_clk_updated = FALSE;
      }      
    }while(0);

    return ret_val;
}

/*===========================================================================

FUNCTION  qfprom_write_set_voltage_dependent_settings

DESCRIPTION
    This function shall set the appropriate voltage dependent settings for the qfprom block.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
  Turns on qfprom clock and enables VREG_GP5.

===========================================================================*/
uint32 qfprom_write_set_voltage_dependent_settings( void )
{
    uint32 ret_val = QFPROM_FAILED_TO_CHANGE_VOLTAGE_ERR;

    /* Voltage is applied enable the logic to write. Set QFPROM_BLOW_VDD to
       1.9v by turning on LDO3 */
    if (handle_qfuse == NULL)
    {
      handle_qfuse = npa_create_sync_client (PMIC_NPA_GROUP_ID_QFPROM, "QFPROM", NPA_CLIENT_REQUIRED);
    }

    if (handle_qfuse)
    {
      /* Set voltage rail to 1.9v */
      npa_issue_required_request(handle_qfuse, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);

      /* Apply enough wait time here to stabilize the voltage */
      DALSYS_BusyWait(1000);

      ret_val = QFPROM_NO_ERR;
    }

    return ret_val;
}

/*===========================================================================

FUNCTION  qfprom_write_reset_voltage_dependent_settings

DESCRIPTION
    This function shall reset the appropriate voltage dependent settings for the qfprom block.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
  Disables the vreg being used.

===========================================================================*/
uint32 qfprom_write_reset_voltage_dependent_settings( void )
{
  /* Disable power to the qfprom block  by turning off switch
     LOW_VOLTAGE_SWITCH_LVS_1_CONTROL
  */
  if (handle_qfuse)
  {
    /* Set voltage rail back to 1.8v */
    npa_issue_required_request(handle_qfuse, PMIC_NPA_MODE_ID_GENERIC_STANDBY);

    /* Disable LDO3 */
    npa_destroy_client (handle_qfuse ); 
	
	/* Set handle to null to enable multiple calls to npa client */
    handle_qfuse = NULL;

    /* Apply enough wait time here to stabilize the voltage */
    DALSYS_BusyWait(1000);

  }

  return QFPROM_NO_ERR;    
}


