/*
===========================================================================
*/
/**
  @file ClockVoltage.c 
  
  Digital voltage management interface implementation for the clock
  device driver.
*/
/*  
  ====================================================================

  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header $
  $DateTime $
  $Author $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  07/31/11   vph     Add support for GFX rail
  10/27/11   vph     Initial version
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"
#include "swevent.h"
#include "railway.h"

/*=========================================================================
      Constants and Macros 
==========================================================================*/

#define CX_RAILWAY   "vddcx"
#define GFX_RAILWAY  "vddgfx"

/*=========================================================================
      Data
==========================================================================*/

/*
 * Define local state variable to track vdd_dig voltage suspension
 */
static boolean bSuspendVoltageRequest;


/*=========================================================================
      Functions
==========================================================================*/

static void Clock_VoteRailway
( 
  ClockDrvCtxt        *pDrvCtxt,
  ClockVRegConfigType *pVReg,
  railway_corner      eMinLevel
)
{
  if ( pVReg == &pDrvCtxt->CX_VRegConfig )
  {
    SWEVENT(CLOCK_EVENT_CX, eMinLevel);
  }
  else
  {
    SWEVENT(CLOCK_EVENT_GFX, eMinLevel);
  }

  // Now setup the vote.
  railway_level_vote(pVReg->sVoter, pDrvCtxt->mVregRailmap[eMinLevel]);

  // Always need to transition the rail after updating the votes for them to be effective.
  railway_transition_rail(pVReg->nRailID);
}
 
/* =========================================================================
**  Function : Clock_VoltageRequest
** =========================================================================*/
/*
  See ClockDriver.h
*/ 

DALResult Clock_VoltageRequest
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockVRegLevelType  *peCurrentLevel,
  ClockVRegLevelType  eNewLevel,
  boolean             bGFXVoltage
)
{
  ClockVRegLevelType  eMinLevel;
  ClockVRegConfigType *pVReg;

  /* For non-gfx rail (CX), the minimun clock voltage is LOW */
  if ( !bGFXVoltage )
  {
    if ( *peCurrentLevel < CLOCK_VREG_LEVEL_LOW )
      *peCurrentLevel = CLOCK_VREG_LEVEL_LOW;

    if ( eNewLevel < CLOCK_VREG_LEVEL_LOW )
      eNewLevel = CLOCK_VREG_LEVEL_LOW;
  }
  
  /*-----------------------------------------------------------------------*/
  /* Find new minimum level.                                               */
  /*-----------------------------------------------------------------------*/

  if (*peCurrentLevel == eNewLevel)
  {
    return DAL_SUCCESS;
  }

  if ( (*peCurrentLevel >= CLOCK_VREG_NUM_LEVELS) || (eNewLevel >= CLOCK_VREG_NUM_LEVELS) )
  {
    return DAL_ERROR;
  }

  if ( bGFXVoltage == FALSE )           // CX Voltage
  {
    pVReg = &pDrvCtxt->CX_VRegConfig;
  }
  else
  {
    pVReg = &pDrvCtxt->GFX_VRegConfig;
  }

  /*-----------------------------------------------------------------------*/
  /* All domains default to LOW at init, but the ClientCount for LOW will  */
  /* not reflect this.  So we cannot track clients for the LOW voltage     */
  /* level, but this is ok since it is the "do not care" level.            */
  /*-----------------------------------------------------------------------*/

  if (*peCurrentLevel != CLOCK_VREG_LEVEL_LOW &&
      pVReg->nClientCount[*peCurrentLevel] > 0)
  {
    pVReg->nClientCount[*peCurrentLevel]--;
  }

  if (eNewLevel != CLOCK_VREG_LEVEL_LOW)
  {
    pVReg->nClientCount[eNewLevel]++;
  }

  *peCurrentLevel = eNewLevel;

  /*-----------------------------------------------------------------------*/
  /* Check if new min has changed.                                         */
  /*-----------------------------------------------------------------------*/

  if (eNewLevel > pVReg->eMinLevel)
  {
    eMinLevel = eNewLevel;
  }
  else
  {
    for (eMinLevel = pVReg->eMinLevel; 
         eMinLevel > eNewLevel; eMinLevel--)
    {
      if (pVReg->nClientCount[eMinLevel] > 0)
      {
        break;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update request if minimum level is different.                         */
  /*-----------------------------------------------------------------------*/

  if (eMinLevel != pVReg->eMinLevel)
  {
    if ( !bSuspendVoltageRequest )
    {
      Clock_VoteRailway( pDrvCtxt, pVReg, (railway_corner)eMinLevel );
    }
      
    pVReg->eMinLevel = eMinLevel;
  }

  /*-----------------------------------------------------------------------*/
  /* Success.                                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_VoltageRequest */


/* =========================================================================
**  Function : Clock_VoltageSuspend
** =========================================================================*/
/*
  See ClockDriver.h
*/ 

DALResult Clock_VoltageSuspend
(
  ClockDrvCtxt        *pDrvCtxt,
  boolean             bVoltageSuspend
)
{
  bSuspendVoltageRequest = bVoltageSuspend;

  /*-----------------------------------------------------------------------*/
  /* Make a voltage request when resuming from suspension.                 */
  /*-----------------------------------------------------------------------*/

  if (!bVoltageSuspend)
  {
    Clock_VoteRailway(  pDrvCtxt, 
                        &pDrvCtxt->CX_VRegConfig, 
                        (railway_corner)pDrvCtxt->CX_VRegConfig.eMinLevel );
  }

  return DAL_SUCCESS;

} /* END Clock_VoltageSuspend */


/* =========================================================================
**  Function : Clock_VoltageResume
** =========================================================================*/
/*
  See ClockDriver.h
*/ 

void Clock_VoltageResume
(
  ClockDrvCtxt        *pDrvCtxt,
  boolean             bUpdateRailway
)
{
  bSuspendVoltageRequest = FALSE;

  /*-----------------------------------------------------------------------*/
  /* Make a voltage request when resuming from suspension.                 */
  /*-----------------------------------------------------------------------*/

  if (bUpdateRailway)
  {
    Clock_VoteRailway(  pDrvCtxt, 
                        &pDrvCtxt->CX_VRegConfig, 
                        (railway_corner)pDrvCtxt->CX_VRegConfig.eMinLevel );
  }

} /* END Clock_VoltageResume */


/* =========================================================================
**  Function : Clock_InitVoltage
** =========================================================================*/
/*
  See ClockDriver.h
*/ 

DALResult Clock_InitVoltage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockPropertyValueType         PropVal;
  DALResult                      eResult;

  /* Initialize CX Rail ID */
  pDrvCtxt->CX_VRegConfig.nRailID = rail_id(CX_RAILWAY);
  assert(RAIL_NOT_SUPPORTED_BY_RAILWAY != pDrvCtxt->CX_VRegConfig.nRailID);
  
  /* Now create a voter for Clock Driver to vote on */
  pDrvCtxt->CX_VRegConfig.sVoter = railway_create_voter(pDrvCtxt->CX_VRegConfig.nRailID, 
                                                        TRUE, 
                                                        RAILWAY_CLOCK_DRIVER_VOTER_ID);

  assert(pDrvCtxt->CX_VRegConfig.sVoter);


  /* Common driver.  Default with no GFX rail. */
  pDrvCtxt->GFX_VRegConfig.nRailID = -1;


  eResult = Clock_GetPropertyValue( "ClockVregRailMap", &PropVal);

  if (eResult == DAL_SUCCESS)
  {
    pDrvCtxt->mVregRailmap = (rail_voltage_level *)PropVal;
    assert(pDrvCtxt->mVregRailmap);
  }
  else
  {
    assert(0);
  }
  return DAL_SUCCESS;

} /* END Clock_InitVoltage */


/* =========================================================================
**  Function : Clock_InitGFXVoltage
** =========================================================================*/
/*
  See ClockDriver.h
*/ 

DALResult Clock_InitGFXVoltage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  /* Initialize GFX Rail ID */
  pDrvCtxt->GFX_VRegConfig.nRailID = rail_id(GFX_RAILWAY);
  assert(RAIL_NOT_SUPPORTED_BY_RAILWAY != pDrvCtxt->GFX_VRegConfig.nRailID);

  /* Now create a voter for Clock Driver to vote on */
  pDrvCtxt->GFX_VRegConfig.sVoter = railway_create_voter(pDrvCtxt->GFX_VRegConfig.nRailID, 
                                                         TRUE, 
                                                         RAILWAY_CLOCK_DRIVER_VOTER_ID);
  assert(pDrvCtxt->GFX_VRegConfig.sVoter);
  
  return DAL_SUCCESS;

}
