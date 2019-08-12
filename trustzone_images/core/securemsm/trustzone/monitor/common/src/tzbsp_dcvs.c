/*===========================================================================
   Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/monitor/common/src/tzbsp_dcvs.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/23/11   tk      Initial version.
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <string.h>

#include "tzbsp.h"
#include "tzbsp_target.h"
#include "tzbsp_config.h"
#include "tzbsp_errno.h"
#include "tzbsp_mem.h"
#include "tzbsp_syscall_priv.h"
#include "tzbsp_log.h"

/* Syscall API definition needs to be in sync with the  */
#if (GPU_DCVS_NUMPWRLEVELS != TZBSP_GPU_DCVS_NUMPWRLEVELS)
#error Mismatch in GPU DCVS power levels
#endif

#define MAX_UINT32 0xffffffff
/* The definitions for the following constants/macros originate from the
 * .builds file: GPU_DCVS_*
 */

static uint32 TotalVector[GPU_DCVS_NUMGAPS], BusyVector[GPU_DCVS_NUMGAPS],
           goLow = 0;
static uint32 GapIndex = 0;
static uint32 BusyPcnt[GPU_DCVS_NUMPWRLEVELS],gpu_freq[GPU_DCVS_NUMPWRLEVELS];
static uint32 num_pwrlevels;
static boolean initDone = 0; 

#define down_busy_pcnt(pwr_level, busy) (gpu_freq[pwr_level]/1000000)\
        * busy/\
        (gpu_freq[pwr_level + 1]/1000000)
#define up_busy_pcnt(pwr_level, busy) (gpu_freq[pwr_level]/1000000)\
        * busy/\
        (gpu_freq[pwr_level - 1]/1000000)\


/**
 * Resets GFX DCVS state. Called by non-secure side in TZ monitor mode.
 */
void tzbsp_graphics_dcvs_reset(void)
{
  int i;
  for (i = 0; i < GPU_DCVS_NUMGAPS; i++)
  {
    TotalVector[i] = 100;
    BusyVector[i] = GPU_DCVS_PENALTY;
  }
  goLow = 0;
  GapIndex = 0;
}

static int busystats_go_low(unsigned int pwr_level, unsigned int busy)
{
  if (pwr_level == num_pwrlevels - 1)
    return 1;
  if (busy > BusyPcnt[pwr_level])
    return 0;
  else
    return 1;
}

/**
 * This is a monitor call.
 *
 * @param inIdle Input variable which will contain the idle gap in microsec.
 * @return 0 - Continue in last performance level.
 *         1 - Increase the performance level.
 *        -1 - Decrease the performance level.
 */
int tzbsp_graphics_dcvs_busystats_update(int active_pwrlevel, int total, int inBusy)
{
  int i =0;
  int outDecision = 0;
  unsigned int average_total = 0;
  unsigned int average_busy = 0;
  unsigned int new_busy = 0;

  if(initDone == 0)
  {
    TZBSP_ERROR_CODE(GFX_DCVS_INIT_NULL_DATA);
    return -E_BAD_ADDRESS;
  }
  
  /* Ensure the pwrlevel index is within allowed bounds. */
  if(active_pwrlevel < 0 || active_pwrlevel >= num_pwrlevels)
  {
    return -E_OUT_OF_RANGE;
  }
  
  TotalVector[GapIndex] = total;
  BusyVector[GapIndex] = inBusy;
  GapIndex ++;
  GapIndex = GapIndex % GPU_DCVS_NUMGAPS ;

  for (i=0; i<GPU_DCVS_NUMGAPS; i++)
  {
  	if (TotalVector[i] < (MAX_UINT32 - average_total))
      average_total += TotalVector[i];
	else
		average_total = MAX_UINT32;
	if (BusyVector[i] < (MAX_UINT32 - average_busy))
      average_busy += BusyVector[i];
	else
		average_busy = MAX_UINT32;
  }

  if(0 == average_total)
  {
    return -E_FAILURE;
  }

  if(average_busy > MAX_UINT32 / 100)
  	average_busy = MAX_UINT32;
  else
  average_busy *= 100;
  average_busy /= average_total;

  /* Make a decision */
  if (average_busy <= GPU_DCVS_PENALTY)
  {
      if (busystats_go_low(active_pwrlevel, average_busy))
      {
         goLow++;
         if (goLow >= GPU_DCVS_MINGAPCOUNT)
         {
             /*
              * move one level lower since
              * MINBUSYCOUNT decisions to go low
              * were taken
              */
              goLow -= GPU_DCVS_NUMBUSY;
             if (active_pwrlevel < (num_pwrlevels - 1))
             {
              outDecision = 1;
              new_busy = down_busy_pcnt(active_pwrlevel, average_busy);
              for (i = 0; i < GPU_DCVS_NUMGAPS; i++)
              {
                TotalVector[i] = 100;
                BusyVector[i] = new_busy;
              }
             }
         }
      }
      else
      {
          goLow = 0;
          outDecision = 0;
      }
  }
  else
  {
      /*
       * move one level higher since GPU busy percetage
       * is greater than GPU_DCVS_PENALTY
       */
      goLow = 0;
      if (active_pwrlevel > 0)
      {
       	new_busy = up_busy_pcnt(active_pwrlevel, average_busy);
       	for (i = 0; i < GPU_DCVS_NUMGAPS; i++)
       	{
           TotalVector[i] = 100;
           BusyVector[i] = new_busy;
        }
        outDecision = -1;
     }
  }
  return outDecision;

}

/**
 * TZ system call to initialize GFX DCVS arrays.
 *
 * @param [in] data  Pointer to the frequency data structure.
 *
 * @return Zero if on success, negative error code on failure.
 */
int tzbsp_graphics_dcvs_init(tzbsp_gfx_dcvs_init_data_t* data)
{
  int i;
  uint32 num_pwrlevels_cpy = 0;
  initDone = 0;

  if(NULL == data)
  {
    TZBSP_ERROR_CODE(GFX_DCVS_INIT_NULL_DATA);
    return -E_BAD_ADDRESS;
  }

  if(!tzbsp_is_ns_range(data, sizeof(tzbsp_gfx_dcvs_init_data_t)))
  {
    TZBSP_ERROR_CODE(GFX_DCVS_INIT_BAD_DATA_RANGE);
    return -E_BAD_ADDRESS;
  }

  num_pwrlevels_cpy = data->nlevels;

  if (num_pwrlevels_cpy > GPU_DCVS_NUMPWRLEVELS)
  {
    TZBSP_ERROR_CODE(GFX_DCVS_INIT_BAD_PWRLEVEL_1);
    return -E_OUT_OF_RANGE;
  }

  if (num_pwrlevels_cpy == 0)
  {
    TZBSP_ERROR_CODE(GFX_DCVS_INIT_BAD_PWRLEVEL_2);
    return -E_FAILURE;
  }

  num_pwrlevels = num_pwrlevels_cpy;
  
  /* We know that num_pwrlevels is always <= GPU_DCVS_NUMPWRLEVELS, and
   * GPU_DCVS_NUMPWRLEVELS is always <= the number of fields in the frequency
   * data structure. */
  for(i=0; i < num_pwrlevels; i++)
  {
    gpu_freq[i] = data->freq[i];
  }
  
  for(i=1; i < num_pwrlevels; i++)
  {
    if((gpu_freq[i-1] /1000000) != 0)
	{
		BusyPcnt[i - 1] = (gpu_freq[i]/1000000) * GPU_DCVS_DOWN_PENALTY /
		(gpu_freq[i-1] /1000000);
	}
	else 
	{
		TZBSP_ERROR_CODE(GFX_DCVS_INIT_BAD_DATA_RANGE);			
		return -E_BAD_DATA; 	
	}
  }
  
  /* init done successfully */
  initDone = 1;

  /* Reset vectors before we start collecting samples */
  tzbsp_graphics_dcvs_reset();
  
  return E_SUCCESS;
}
