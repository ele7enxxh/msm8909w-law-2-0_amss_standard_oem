/*===========================================================================
  FILE: target_sys_arch_config.c

  OVERVIEW: We have all the target specific system initialization routines
               defined here.

  NOTE:
    The data in the API may change as per the target.

  REFERENCES:

  DEPENDENCIES: We tap into system drivers to get the HWIO reg address to which
                   we need to write.

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/


/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "HWIOq6msa.h"


/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
#include "HALhwio.h"

static void update_mss_q6_msa(boolean action)
{
    if(action == TRUE) /*We have to set the FORCE_Q6_MSA bit*/
	{
	   HWIO_OUTF(MSS_MSA, FORCE_Q6_MSA, 1);
    }	   
	else 			  /*We have to clear the FORCE_Q6_MSA bit*/
	{
	   HWIO_OUTF(MSS_MSA, FORCE_Q6_MSA, 0);
	}
	   
	//return 1;  //We can put a different return value if the above apis go wrong.

}
/*We can put any target related config activites here. 
 *This can act as a place holder for such  initializtions 
 */
void qurt_kernel_init(void)
{
    update_mss_q6_msa(0);	   
}

