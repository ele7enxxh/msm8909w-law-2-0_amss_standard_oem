/*===========================================================================

FILE:         BSPspm.c

DESCRIPTION:  This file contains initializing/obtaining target specific SPM 
              data and related functions. Depending on the target, these 
              data can be obtained in different way like through device 
              config or by direct initialization.


              Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
              All Rights Reserved.
              Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/src/hal/BSPspm.c#1 $

============================================================================*/


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DALSysTypes.h"
#include <DDIChipInfo.h>

#include "spm.h"
#include "BSPspm.h"
#include "CoreVerify.h"

/* -----------------------------------------------------------------------
**                           Function Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
**                           GLOBAL TARGET DATA
** ----------------------------------------------------------------------- */

/**
 * SPM_NUM_CORES
 */
unsigned int SPM_NUM_CORES = 0;

/**
 * SPM_BSP_DATA
 */
BSP_spm_ConfigDataType *SPM_BSP_DATA = NULL;

/**
 * gSpmCmdSeqArray
 */
BSP_spm_CmdSequenceType (*gSpmCmdSeqArray)[SPM_NUM_LOW_POWER_MODES] = NULL;
    
/* -----------------------------------------------------------------------
**                         Functions  
** ----------------------------------------------------------------------- */

/**
 * BSP_spm_InitTargetData
 */
void BSP_spm_InitTargetData( void )
{
  DALSYS_PROPERTY_HANDLE_DECLARE(hSpmDevCfg);
  DALSYSPropertyVar prop;

  CORE_DAL_VERIFY(DALSYS_GetDALPropertyHandleStr("/dev/core/power/spm", 
                                                   hSpmDevCfg));

  CORE_DAL_VERIFY(DALSYS_GetPropertyValue( hSpmDevCfg, "spm_num_cores", 0, 
                                           &prop));
  SPM_NUM_CORES = (unsigned int)prop.Val.dwVal;
  
  CORE_DAL_VERIFY(DALSYS_GetPropertyValue( hSpmDevCfg, "spm_bsp_data", 0, 
                                           &prop)); 
  SPM_BSP_DATA = (BSP_spm_ConfigDataType *)prop.Val.pStruct;
  
  CORE_DAL_VERIFY(DALSYS_GetPropertyValue( hSpmDevCfg, "spm_cmd_seq_info_array",
                                           0, &prop));
  gSpmCmdSeqArray = 
      (BSP_spm_CmdSequenceType (*)[SPM_NUM_LOW_POWER_MODES])prop.Val.pStruct;
  
  return;
}

/**
 * BSP_spm_GetNumSupportedLPMs
 */
uint32 BSP_spm_GetNumSupportedLPMs( uint32 coreNum )
{
  int i = 0;

  CORE_VERIFY( coreNum < SPM_NUM_CORES );
  CORE_VERIFY_PTR( gSpmCmdSeqArray );

  while ( i < SPM_NUM_LOW_POWER_MODES ) 
  {
    if ( 0 == gSpmCmdSeqArray[coreNum][i].len )
    {
      break;
    }
    i++;
  }

  return i;
}
 
