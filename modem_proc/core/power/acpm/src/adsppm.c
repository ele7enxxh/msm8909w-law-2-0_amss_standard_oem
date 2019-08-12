/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * main.c
 *
 *  Created on: Feb 10, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "hal.h"
#include "core.h"
#include "asic.h"
#include "api.h"
#include "adsppm_utils.h"
#include "ULog.h"
#include "ULogFront.h"
#include "DDIClock.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "ddiaxicfg.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"

#define ADSPPM_ULOG_BUFFER_SIZE 32768

typedef struct{
    ULogHandle m_hLog;
    uint32 m_log_buffer_size;
    uint32 initialized;
}AdsppmGlbCtxType;

static AdsppmGlbCtxType gAdsppmCtx;

uint32 gADSPPMINITSTATE = 0;

typedef struct{
    DalDeviceHandle  *dalClkHandle;		//Handle for ahb clock
	ClockIdType       clkId;			//Id for ahb clock
} ClkRgmInfoType;

ClkRgmInfoType gAhbClkHandle;

const char ULTAUDIO_AHB_CLK_NAME1[] = "gcc_ultaudio_ahbfabric_ixfabric_clk";

/* @enum ULTAudioResetState - 
 * Adsppm State while doing Ultaudio Reset
 */

typedef enum {
	ClockDeviceAttach_Failed = 0x1,
	GetClockId_Failed 		 = 0x2,
	ClkEnable_Failed		 = 0x4,
	ClkDisable_Failed		 = 0x8,
	BusHalt_Failed			 = 0x10,
	BusUnHalt_Failed		 = 0x20,
	ClkAssert_Failed		 = 0x40,
	ClkDeAssert_Failed		 = 0x80,
	Reset_Success			 = 0x100
} UltAudioResetState;

/* Global variables to store current adsppm and Axi state 
 * as can't use ulogs
 */
 uint32 gUltAudioCurrentState = 0;
 AxiCfg_ReturnCodeType gAxiResult = AXICFG_SUCCESS;

/* Global Flag to determine UltAudio Reset Done */
 boolean gULTAudioResetDone = FALSE;


__inline ULogHandle GetUlogHandle(void)
{
    return 	gAdsppmCtx.m_hLog;
}

/*============================================================================

FUNCTION ADSPPM_LOCK

DESCRIPTION
    Lock the critical section using the dalsys API.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

============================================================================*/
__inline void adsppm_lock( DALSYSSyncHandle lock )
{
    DALSYS_SyncEnter(lock);
}


/*============================================================================

FUNCTION ADSPPM_UNLOCK

DESCRIPTION
    Unlock the critical section using the dalsys API.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

============================================================================*/
__inline void adsppm_unlock( DALSYSSyncHandle lock )
{
    DALSYS_SyncLeave(lock);
}

Adsppm_Status ADSPPM_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    // Initialize ADSPPM ULOG
    gAdsppmCtx.m_log_buffer_size = ADSPPM_ULOG_BUFFER_SIZE;
    gAdsppmCtx.initialized = 0;
    result = ULogFront_RealTimeInit(&gAdsppmCtx.m_hLog,
            "ADSPPM Log",
            gAdsppmCtx.m_log_buffer_size,
            ULOG_MEMORY_LOCAL,
            ULOG_LOCK_OS );

    if (DAL_SUCCESS == result)
    {

        // Create ASIC Config Manager
        result = ACM_Init();
        // Create OS abstraction components

        if(Adsppm_Status_Success == result)
        {
            // Create all CoreBSP wrapper components
            result = Hal_Init();

            if(Adsppm_Status_Success == result)
            {
                // Create ADSPPM core
                result = Core_Init();

                if(Adsppm_Status_Success == result)
                {
                    // Create API components
                    result = API_Init();
                }
            }
        }
    }
    else
    {
        result = Adsppm_Status_CoreResourceNotAvailable;
    }
    if(Adsppm_Status_Success == result)
    {
        gAdsppmCtx.initialized = 1;
    }
    return result;
}

__inline uint32 ADSPPM_IsInitialized(void)
{
    return gAdsppmCtx.initialized;
}

void ADSPPM_EnterSleep(void)
{
    Adsppm_Status result = Core_EnterSleep();
    if(Adsppm_Status_Success != result)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Failed with code %d", result);
    }
}

void ADSPPM_ExitSleep(void)
{
    Adsppm_Status result = Core_ExitSleep();
    if(Adsppm_Status_Success != result)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Failed with code %d", result);
    }
}

void adsppm_ultaudio_reset( void)
{

	DALResult dalResult = DAL_SUCCESS;
	
	/* Check if Reset is done already */
	if ( TRUE == gULTAudioResetDone)
	{
		return;
	}
	
	/* Assign TRUE to avoid calling multiple times */
	gULTAudioResetDone = TRUE;
	
	if( DAL_SUCCESS == DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &(gAhbClkHandle.dalClkHandle)))
	{
		if( DAL_SUCCESS == DalClock_GetClockId(gAhbClkHandle.dalClkHandle, ULTAUDIO_AHB_CLK_NAME1, &(gAhbClkHandle.clkId)))
		{
			if( DAL_SUCCESS == DalClock_EnableClock(gAhbClkHandle.dalClkHandle, gAhbClkHandle.clkId))
			{
				/* Halt Bus Traffic */
				gAxiResult = AxiCfg_HaltMasterPort(ICBID_MASTER_AUDIO);
				if(AXICFG_SUCCESS == gAxiResult)
				{
					/* Assert Reset */
					dalResult = DalClock_ResetClock(gAhbClkHandle.dalClkHandle, gAhbClkHandle.clkId, CLOCK_RESET_ASSERT);
					if (DAL_SUCCESS == dalResult)
					{
						/* Need at least 3 sleep clocks delay */
						DALSYS_BusyWait(100);
						
						/* Unhalt bus traffic */
						gAxiResult = AxiCfg_UnhaltMasterPort(ICBID_MASTER_AUDIO);
						if (AXICFG_SUCCESS == gAxiResult)
						{
							/* DeAssert Reset */
							dalResult = DalClock_ResetClock(gAhbClkHandle.dalClkHandle, gAhbClkHandle.clkId, CLOCK_RESET_DEASSERT);
							if (DAL_SUCCESS != dalResult)
							{
								gUltAudioCurrentState |= ClkDeAssert_Failed;
							}
						}
						else
						{
							/* Unhalt Bus Failed */
							gUltAudioCurrentState |= BusUnHalt_Failed;
						}						
					}
					else
					{
						/* Clock Assert Failed */
						gUltAudioCurrentState |= ClkAssert_Failed;
					}
				}
				else
				{
					/* Bus Halt failed */
					gUltAudioCurrentState |= BusHalt_Failed;
				}
				
				/* Disable AHB clock */
				dalResult = DalClock_DisableClock(gAhbClkHandle.dalClkHandle, gAhbClkHandle.clkId);
				if ( DAL_SUCCESS == dalResult)
				{
					gUltAudioCurrentState |= Reset_Success;
				}
				else
				{
					gUltAudioCurrentState |= ClkDisable_Failed;
				}
			}
			else
			{
				/* Enabling clock failed */
				gUltAudioCurrentState |= ClkEnable_Failed;
			}
		}
		else
		{
			/* Getting clock Id failed */
			gUltAudioCurrentState |= GetClockId_Failed;
		}
	}
	else
	{
		/* Attach to dalDevice failed */
		gUltAudioCurrentState |= ClockDeviceAttach_Failed;
	}
}	
	
