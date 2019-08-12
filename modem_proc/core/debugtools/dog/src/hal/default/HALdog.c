/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    W A T C H D O G     H A L

GENERAL DESCRIPTION
  These functions implement hardware specific instructions in watchdog

FUNCTIONS
  HAL_dogEnable
  HAL_dogKick
  HAL_dogAutokick
  HAL_dogEnableBark
  HAL_dogDisableBark
  HAL_dogRegisterBark
  HAL_dogIsAutokicking


  

Copyright (c) 2002 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/dog/src/hal/default/HALdog.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#include "comdef.h"

#define DOG_BARK_ENABLE_TIMEOUT_SLCK    0x3000  /* 12288 slcks */
#define DOG_BARK_DISABLE_TIMEOUT_SLCK    0x3FFF 

#define DOG_BITE_ENABLE_TIMEOUT_SLCK    0x31F3



void dog_bark_isr(void);

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

 }


