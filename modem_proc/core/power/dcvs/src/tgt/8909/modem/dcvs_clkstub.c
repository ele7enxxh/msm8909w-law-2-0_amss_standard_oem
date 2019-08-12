

/**********************************************************************
 * dcvs_clkstub.c
 *
 * Provides the interface for querying clock information on the 8960 Target.
 * Every target has an implementation of this file.
 * The targets can have one of the following clock plan data types::
 * DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE
 * DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE
 * 
 * The priority of the clock plan retrieval is ::
 * DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE
 * Query from the resource
 * DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE
 * 
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2011-2014 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*===========================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/tgt/8909/modem/dcvs_clkstub.c#1 $
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ----------------------------------------------------------- 
13/04/04   nj      Added V2 clock plan + chip detection logic to choose plan 
12/07/12   nj      Updates to clock plan
11/06/12   sg      Fixed bandwidth values in clock plan.
11/06/12   nj      Updates to the Clock plan table 
09/11/12   nj      Initializing clkticks per second variable 
09/05/12   nj      Updated the clock plan
03/16/12   nj      initial checkin
=============================================================================*/


#include "comdef.h"
#include "dcvs_target.h"
#include "dcvs_core.h"
#include "DDIChipInfo.h"
#include "dcvs_hwio.h"
#include "CoreVerify.h"

#define Q6_FMAX_FUSE_SHIFT            16
#define Q6_FMAX_FUSE_MASK	     0x7

/* This local clock plan table is converted into the format of the
   primary clock plan table and is used if the primary plan is not available.*/

DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE localClockPlanData[]=
{
  {691200}
};

/* The lookup table of requested mips and the corresponding 
   CPU frequency(In KHz)  and Bus AXI Frequency.(In MBps) 
*/
/* Clock plan for 499.2 MHz */
DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE primaryClockPlanDataV1_0[] =
{
  {115,    115200, 0},
  {144,    144000, 800},
  {288,    288000, 800},
  {384,    384000, 800},
  {499,    499200, 2128},
};

/* Clock plan for 595.2 MHz */
DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE primaryClockPlanDataV1_1[] =
{
  {115,    115200,   0},
  {144,    144000, 800},
  {288,    288000, 800}, 
  {384,    384000, 800},
  {595,    595000, 2128},
};

/* Clock plan for 691.2 MHz */
DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE primaryClockPlanDataV1_2[] =
{
  {115,    115200,   0},
  {144,    144000, 800},
  {288,    288000, 800}, 
  {384,    384000, 800},
  {480,    576000, 1600},
  {576,    576000, 2128},
  {630,    691200, 3200},
  {691,    691200, 4264},
};

/* Default Clock plan for 691.2 MHz */
DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE primaryClockPlanDataV2[] =
{
  {115,    115200, 0},
  {144,    144000, 800},
  {288,    192000, 800}, 
  {384,    384000, 800},
  {480,    576000, 1600},
  {576,    576000, 2128},
  {630,    691200, 3200},
  {691,    691200, 4264},
};

DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE* DCVSTargetClockPlanLocalGet(void)
{
  /* Return the local clock plan table */
  return(localClockPlanData);
}

uint32 DCVSTargetClockPlanLocalNumLevelsGet(void)
{
  /* Return the number of entries in the local clock plan table. */
  return(sizeof(localClockPlanData)/sizeof(DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE));
}

DCVS_TARGET_CLOCK_PLAN_INFO_TYPE dcvsTargetClockPlan = 
{
  /* The clock plan table having mappings of mips and processor frequency.*/
  primaryClockPlanDataV2,
  /* The number of performance levels in the clock plan for this target. */
  sizeof(primaryClockPlanDataV2)/sizeof(DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE),
  /* Target specific frequency conversion functions. */
  NULL,
  NULL,
  NULL,
  19200000    /* This is the freq. of the QTimer source (timer clock frequency) */
};


static uint32  _GetSpeedBinFuseVal (void)
 {
   uint32  nPVS, CurrentSpeedBin;

   nPVS = HWIO_IN(QFPROM_CORR_PTE1);
   CurrentSpeedBin = ((nPVS >> Q6_FMAX_FUSE_SHIFT) & Q6_FMAX_FUSE_MASK);
  
   return CurrentSpeedBin;
}

DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* DCVSTargetClockPlanInfoGet(void)
{
  // Determine chip version

  DalChipInfoVersionType chipVersion;
  CORE_VERIFY(chipVersion = DalChipInfo_ChipVersion());

  if (chipVersion < 0x00020000)
  {
    uint32 speedBinFuseVal = _GetSpeedBinFuseVal();
    if (0 == speedBinFuseVal)
    {
      dcvsTargetClockPlan.clockPlanTable = primaryClockPlanDataV1_0;  
      dcvsTargetClockPlan.numLevels = sizeof(primaryClockPlanDataV1_0)/sizeof(DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE);	    
    }
    else if (1 == speedBinFuseVal)
    {
      dcvsTargetClockPlan.clockPlanTable = primaryClockPlanDataV1_1;  
      dcvsTargetClockPlan.numLevels = sizeof(primaryClockPlanDataV1_1)/sizeof(DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE);
    }
     else if (2 == speedBinFuseVal)
    {
      dcvsTargetClockPlan.clockPlanTable = primaryClockPlanDataV1_2;  
      dcvsTargetClockPlan.numLevels = sizeof(primaryClockPlanDataV1_2)/sizeof(DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE);
    }
  }
  /* Return the local clock plan info of the target. */
  return(&dcvsTargetClockPlan);
}