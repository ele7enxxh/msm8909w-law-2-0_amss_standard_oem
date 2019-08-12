/*============================================================================
@file cpr_efuses.c

Gathers config data from Efuses.

Copyright © 2013-2015 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/common/cpr_efuses.c#1 $
============================================================================*/
//#include "HALhwio.h"
#include "cpr_internal.h"
#include "cpr_efuse.h"
#include "CoreVerify.h"

/* Contains all the CPR context data. */
extern CprContextType cprContext;

/**
 * <!-- CprEfuseGetDataFromFuses -->
 *
 * @brief Reads CPR configuration data blown into efuses.
 * 
 * @param fuseDb : Pointer to the database of efuse registers and fields from
 * which CPR data can be read.
 * @param vmodeCfg : Pointer to an array of structs in which the efused data
 * will be stored. The array size must match CPR_MAX_VMODE_COUNT.
 * 
 * @return TRUE if efuses have indicated that CPR should be disabled; 
 * FALSE otherwise.
 */
static boolean CprEfuseGetDataFromFuses( CprEfuseDatabaseType *    fuseDb,
                                         uint32                    numFuseEntries,
                                         CprVmodeBoundsAndOffsetCfgType** vmodeBoundsAndOffsetCfg )
{
  uint32 fuse_data_index;

 //read the CPR disable fuse if there is one defined.
  if (fuseDb->disCpr.addr != 0){
    cprContext.fuse_disable = CPR_EFUSE_IN(fuseDb->disCpr);
  } else {
    cprContext.fuse_disable = 0;
  }

  // Bail if CPR is disabled.
  if (cprContext.fuse_disable == 1)
  {
    return TRUE;
  }

  // Get efuse data for each voltage mode.
  for (fuse_data_index=0; fuse_data_index<numFuseEntries; fuse_data_index++)
  {
    uint32 targetVsteps0, targetVsteps1;
    int8 initVoltOffsetSteps;


    CprEfuseVModeCfgType * efuseVmode = &fuseDb->fuseVmodeCfgPtrs[fuse_data_index];

    //read the fuse value if the location has been defined. 
    if (efuseVmode->targetVsteps0.addr != 0){
      targetVsteps0 = CPR_EFUSE_IN(efuseVmode->targetVsteps0);
      targetVsteps1 = CPR_EFUSE_IN(efuseVmode->targetVsteps1);

      initVoltOffsetSteps = (int8) ((targetVsteps1 << efuseVmode->shiftLeftBy)
                                	|  targetVsteps0);
    } else {
	  initVoltOffsetSteps = 0;
    }

    // Treat as negative number if MSB is set in the 5-bit field and make it
    // a valid signed int8 value.
    if (initVoltOffsetSteps & 0x10)
    {
      initVoltOffsetSteps = -(initVoltOffsetSteps & 0xf);
    }
    if (vmodeBoundsAndOffsetCfg[efuseVmode->modeEnum]!=NULL)
    {
      vmodeBoundsAndOffsetCfg[efuseVmode->modeEnum]->initVoltOffsetSteps = initVoltOffsetSteps;
    } else {
      //should not be trying to fill in information for a mode enum that we're not prepared for. 
      CORE_VERIFY(0);
    }
  }

  return FALSE;
}


/*----------------------------------------------------------------------------
 * Public interfaces
 * -------------------------------------------------------------------------*/
/**
 * <!-- CprEfuseGetData -->
 *
 * @brief Reads CPR configuration data blown into efuses correcting for the
 * redundancy select bit.
 * 
 * @param fuseDb : Pointer to the database of efuse registers and fields from
 * which CPR data can be read.
 * @param vmodeCfg : Pointer to an array of structs in which the efused data
 * will be stored. The array size 
 * 
 * @return TRUE if efuses have indicated that CPR should be disabled; 
 * FALSE otherwise.
 */
boolean CprEfuseGetData( CprEfuseMasterDatabaseType * fuseDb,
                         CprVmodeBoundsAndOffsetCfgType** vmodeBoundsAndOffsetCfg )
{
 
  //read the redundant selection fuse if there is one defined.
  if (fuseDb->redundancySel.addr != 0){
    cprContext.fuse_redund = CPR_EFUSE_IN(fuseDb->redundancySel);
  } else {
    cprContext.fuse_redund = 0;
  }

  if (cprContext.fuse_redund == 0x1)
  {
    return CprEfuseGetDataFromFuses(&fuseDb->redundantDb, fuseDb->numFuseEntries, vmodeBoundsAndOffsetCfg);
  }
  else
  {
    return CprEfuseGetDataFromFuses(&fuseDb->primaryDb, fuseDb->numFuseEntries, vmodeBoundsAndOffsetCfg);
  }
}

