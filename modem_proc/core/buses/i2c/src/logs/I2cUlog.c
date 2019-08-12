
/*
===========================================================================

FILE:   I2cULog.c

DESCRIPTION:
    This file contains functions for I2c logging  driver
 
===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/logs/I2cUlog.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/20/14 MS     Created from 8916 MPSS

===========================================================================
        Copyright c 2011 - 2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "ULogFront.h"
#include "DALSys.h"

#include "I2cLog.h"

#define I2C_LOG_NAME "I2C_Logs"
#define I2C_LOGBUF_SIZE 2048


/*  Handle to the log */
ULogHandle i2cDynLogHandle;              

/* There are there log levels.
 * Refer SpiLog.h for details.
 * Default it level SPI_LOG_LEVEL_INFO, 
 * but during debugging the levels can be 
 * increased through JTAG.
 */
static uint32 ui2cLogLevel = 2;//I2C_LOG_LEVEL_ERROR; 

// Initialization 
void 
I2cLog_Init( )
{
    
   ULogResult      ures;
   int32           res;
   
   do {
      /* Size of I2c Log - Defaults to 2048 so that logging is enabled */

      ures = ULogFront_RealTimeInit( 
            &i2cDynLogHandle,  /* Log Handle*/
            I2C_LOG_NAME,          /* Name of the log */
            I2C_LOGBUF_SIZE,  /* Initial Buffer. Normally set to 0 for a disabled log. */
            ULOG_MEMORY_LOCAL, /* Local memory (normal case) */
            ULOG_LOCK_OS);     /* No lock is necessary since this 
                                    case is always run 
                                    within a lock.*/
      if (DAL_SUCCESS != ures) {
         res = ures;
         break;
      }
   } while ( 0 );
}

uint32 I2cLog_GetLogLevel(void)
{
   return(ui2cLogLevel);
}

void* I2cLog_GetUlogHandle(void)
{
   return(i2cDynLogHandle);
}

void I2cLog_SetLogLevel(uint32 uLogLevel)
{
	ui2cLogLevel = (uLogLevel > I2C_LOG_LEVEL_MAX) ? 
									I2C_LOG_LEVEL_MAX : uLogLevel;
}
