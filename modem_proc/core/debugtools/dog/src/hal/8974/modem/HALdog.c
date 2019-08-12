/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    W A T C H D O G     H A L

GENERAL DESCRIPTION
  These functions implement hardware specific instructions in watchdog

FUNCTIONS
  HAL_dogEnable
  HAL_dogDisable
  HAL_dogKick
  HAL_dogAutokick
  HAL_dogEnableBark
  HAL_dogDisableBark
  HAL_dogRegisterBark
  HAL_dogIsAutokicking




Copyright (c) 2002 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/dog/src/hal/8974/modem/HALdog.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/


#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "HALhwio.h"
#include "HALdogHWIO.h"
#include "msmhwiobase.h"

#include "DDIInterruptController.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"

#include "err.h"

#define DOG_BARK_ENABLE_TIMEOUT_SLCK    0x3000  /* 12288 slcks */
#define DOG_BITE_ENABLE_TIMEOUT_SLCK    0x31F3
#define DOG_NMI_ENABLE_TIMEOUT_SLCK     0x30AC
#define DOG_BARK_DISABLE_TIMEOUT_SLCK    0x3FFF

#define DOG_ISR                         68


void dog_bark_isr(uint32 client_param);

DalDeviceHandle * hInthandle;

volatile boolean dog_intentional_timeout_flag = FALSE;



/** =====================================================================
 * Function:
 *     HAL_dogEnable
 *
 * Description:
 *     This function enables the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogEnable( void )
{
    //set bite time
    HWIO_OUT(MSS_QDSP6SS_WDOG_NMI_TIME, DOG_NMI_ENABLE_TIMEOUT_SLCK);
    HWIO_OUT(MSS_QDSP6SS_WDOG_BITE_TIME, DOG_BITE_ENABLE_TIMEOUT_SLCK);

    //turn on bits 0 and 2, this enables the dog and turns on NMI
    HWIO_OUT( MSS_QDSP6SS_WDOG_CTL, 0x5 );
}

/** =====================================================================
 * Function:
 *     HAL_dogDisable
 *
 * Description:
 *     This function disables the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogDisable( void )
{
    if (!dog_intentional_timeout_flag)
    {
        HWIO_OUT( MSS_QDSP6SS_WDOG_CTL, 0x0 );
    }
}

/** =====================================================================
 * Function:
 *     HAL_dogKick
 *
 * Description:
 *     This function kicks the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogKick( void )
{
    if (!dog_intentional_timeout_flag)
    {
       HWIO_OUT( MSS_QDSP6SS_WDOG_RESET, 1 );
    }
}

/** =====================================================================
 * Function:
 *     HAL_dogAutokick
 *
 * Description:
 *     This function turns on dog autokick
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogAutokick( void )
{
    if (!dog_intentional_timeout_flag)
    {
       HWIO_OUT( MSS_QDSP6SS_WDOG_CTL, 0x0 );
    }
}

/** =====================================================================
 * Function:
 *     HAL_dogEnableBark
 *
 * Description:
 *       Enable the watchdog bark timer.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogEnableBark( void )
{
	HWIO_OUT( MSS_QDSP6SS_WDOG_BARK_TIME, DOG_BARK_ENABLE_TIMEOUT_SLCK );
}

/** =====================================================================
 * Function:
 *     HAL_dogDisableBark
 *
 * Description:
 *       Enable the watchdog bark timer.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogDisableBark( void )
{
    HWIO_OUT( MSS_QDSP6SS_WDOG_BARK_TIME, DOG_BARK_DISABLE_TIMEOUT_SLCK );
}

/** =====================================================================
 * Function:
 *     HAL_dogRegisterBark
 *
 * Description:
 *     Register an ISR for the watchdog bark interrupt.
 *
 * Parameters:
 *     none
 *
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogRegisterBark( void )
{
    if ((DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hInthandle) != DAL_SUCCESS) || (hInthandle == NULL))
        ERR_FATAL("Dog cannot register with DAL", 0, 0, 0);

    DalInterruptController_RegisterISR(hInthandle, DOG_ISR, (DALIRQ)dog_bark_isr, 0, DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);
}

 /** =====================================================================
 * Function:
 *     HAL_dogIsAutokicking
 *
 * Description:
 *     Check if dog is autokicking
 *
 * Parameters:
 *     uint32 status pointer
 *
 * Returns:
 *     pointer set to 1 if autokicking, 0 otherwise
 * =====================================================================  */
 void HAL_dogIsAutokicking(uint32 *status)
 {
	if(HWIO_INF(MSS_QDSP6SS_WDOG_CTL,ENABLE))
        *status = 1;
 	else
		*status = 0;
	return;
 }

 /** =====================================================================
 * Function:
 *     HAL_dogForceBite
 *
 * Description:
 *       Force the watchdog to bite
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogForceBite( void )
{
    /* Enable HW watchdog by writing on bits 0 and 2 */
    HWIO_OUT( MSS_QDSP6SS_WDOG_CTL, 0x5 );

	/* Trigger HW watchdog bite */
    HWIO_OUT( MSS_QDSP6SS_WDOG_BITE_TIME, 0x1 );
}
