/*============================================================================
@file cpr_target.c

Target-specific CPR configuration data for 9x35.

Copyright © 2013, 2014 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/tgt/9x35/cpr_target.c#1 $
=============================================================================*/
#include "HALhwio.h"
#include "HAL_cpr.h"
#include "mss_cpr_hwio.h"
#include "cpr_efuse.h"
#include "cpr_internal.h"
#include "CoreVerify.h"
#include "pm_version.h"
#include "DDIChipInfo.h"
#include "VCSDefs.h" /* Enums for the power modes */

/** Contains all the CPR context data. */
extern CprContextType cprContext; 

//there are 6 fuse values to read. 
#define NUM_9x35_VMODES_FUSES 6


/** Database of fuse locations and fields we want to read from. */
static CprEfuseMasterDatabaseType cprEfuseMasterDb =
{
  .redundancySel = { HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR, 0xe0000000, 29 },

  .numFuseEntries = NUM_9x35_VMODES_FUSES,
  .primaryDb = 
  {
    .disCpr  = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x100000, 20},
    .fuseVmodeCfgPtrs =
    {
      [0] =
      {
        .modeEnum  = VCS_CORNER_LOW_MINUS,  //SVS2 uses SVS fuse as of 5/2014
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x7C00, 10},  //CPR1_SVS_TARGET BIT[14:10]
      },
      [1] =
      {
        .modeEnum  = VCS_CORNER_LOW,		//SVS
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x7C00, 10},  //CPR1_SVS_TARGET BIT[14:10]
      },
      [2] =
        {
          .modeEnum = VCS_CORNER_LOW_PLUS,		//SVS+ uses SVS fuse
          .targetVsteps = { HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x7C00, 10 },  //CPR1_SVS_TARGET BIT[14:10]
        },

      [3] =
      {
        .modeEnum  = VCS_CORNER_NOMINAL,
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x3E0, 5},  //CPR1_NOMINAL_TARGET BIT[9:5]
      },
      [4] = 
      {
        .modeEnum  = VCS_CORNER_NOMINAL_PLUS,
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x1f, 0},  //CPR1_TURBO_TARGET BIT[4:0]
      },
      [5] =
      {
        .modeEnum  = VCS_CORNER_TURBO,
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x1f, 0},  //CPR1_TURBO_TARGET BIT[4:0]
      }
    }
  },
  .redundantDb =
  {
    .disCpr  = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR,  HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_BMSK, HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_SHFT},
    .fuseVmodeCfgPtrs =
     { 
      [0] =
      {
        .modeEnum  = VCS_CORNER_LOW_MINUS,  //SVS2 uses SVS fuse as of 5/2014
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, 0x1F, 0},  //	CPR1_SVS_TARGET_REDUN BIT[4:0]
      },
      [1] =
      {
        .modeEnum  = VCS_CORNER_LOW,  //SVS
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, 0x1F, 0},  //	CPR1_SVS_TARGET_REDUN BIT[4:0]
      },
      [2] =
        {
          .modeEnum = VCS_CORNER_LOW_PLUS,  //SVS+ uses SVS fuse
          .targetVsteps = { HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, 0x1F, 0 },  //	CPR1_SVS_TARGET_REDUN BIT[4:0]
        },
     [3] =
      {
        .modeEnum  = VCS_CORNER_NOMINAL,
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0xF8000000, 27},  //CPR1_NOMINAL_TARGET_REDUN BIT[31:27]
      },
      [4] =
      {
        .modeEnum  = VCS_CORNER_NOMINAL_PLUS,
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x7C00000, 22},  //	CPR1_TURBO_TARGET_REDUN BIT[26:22]
      },
      [5] =
      {
        .modeEnum  = VCS_CORNER_TURBO,
        .targetVsteps = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x7C00000, 22},  //	CPR1_TURBO_TARGET_REDUN BIT[26:22]
      }
    }
  }
};



#define NUM_9x35_VMODES 6

/** Target specific config. */
static CprTargetCfgType cprTargetCfg =
{
  .irq                     = 91,       // From MSS HPG document
  .delayBeforeNextMeas     = 5,        // millisecs
  .voltageStepSize         = 12500,    // microVolts
  .numVmodeQuotientsAvail  = NUM_9x35_VMODES,

};



CprVmodeBoundsAndOffsetCfgType boundsAndOffsetCfg9x35[NUM_9x35_VMODES] =
  { [0] =
    {
      .voltageCeiling = 0,
      .voltageFloor = 0,
      .initVoltOffsetSteps = 0,
    },
    [1] =
    {
      .voltageCeiling = 0,
      .voltageFloor = 0,
      .initVoltOffsetSteps = 0,
    },
    [2] =
    {
      .voltageCeiling = 0,
      .voltageFloor = 0,
      .initVoltOffsetSteps = 0,
    },
    [3] =
    {
      .voltageCeiling = 0,
      .voltageFloor = 0,
      .initVoltOffsetSteps = 0,
    },
    [4] =
    {
      .voltageCeiling = 0,
      .voltageFloor = 0,
      .initVoltOffsetSteps = 0,
    },
    [5] =
      {
        .voltageCeiling = 0,
        .voltageFloor = 0,
        .initVoltOffsetSteps = 0,
      },
  };





/** Hardware version specific config needed by the HAL. */
static HalCprTargetConfigType halCprTargetCfg =
  {
    .staticCfg =
    {
      .configVersionText = "9x35 9/12/2014",

      .sensorMask[0] = 0,
      .sensorMask[1] = 0,

      .speedpushed_ROs = 0,     //No Speedpush RO masking needed

      .sensorBypass[0] = 0,
      .sensorBypass[1] = 0,

      .CTL__DN_THRESHOLD = 2,
      .CTL__UP_THRESHOLD = 1,

      .SW_VLEVEL__SW_VLEVEL = 0x20,

      .TIMER_ADJUST__CONSECUTIVE_DN = 0x2,

      .sensorsOnCpuBlock[0] = 0x3000,
      .sensorsOnCpuBlock[1] = 0,
    },
    .halVmodeCfgPtrs =  // pointers to the vmode (SVS, SVS+, etc) configs to be filled in at init
    { 
       0,0,0,0,0,0,0,0,0,
    }
  };

//----------------------------------------------------------------
// Version 1 quotients
//----------------------------------------------------------------
  HalCprVmodeCfgType vmode_mode_settings_9x35V1[NUM_9x35_VMODES] =
   {  [0] =
     {                           /* SVS2 75mv*/
      .modeEnum              = VCS_CORNER_LOW_MINUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 700000,
      .initialCeilingVoltage = 800000, 
      .factoryOpenLoopCeilingCalculationVoltage = 800000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 271,
      .oscCfg[1].target      = 252,
      .oscCfg[2].target      = 286,
      .oscCfg[3].target      = 279,
      .oscCfg[4].target      = 570,
      .oscCfg[5].target      = 515,
      .oscCfg[6].target      = 615,
      .oscCfg[7].target      = 581,
     },
    [1] =
    {                        /* SVS 75mV */                          
      .modeEnum             = VCS_CORNER_LOW,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 725000,
      .initialCeilingVoltage = 900000,      
      .factoryOpenLoopCeilingCalculationVoltage = 900000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 418,
      .oscCfg[1].target      = 396,
      .oscCfg[2].target      = 441,
      .oscCfg[3].target      = 433,
      .oscCfg[4].target      = 782,
      .oscCfg[5].target      = 723,
      .oscCfg[6].target      = 842,
      .oscCfg[7].target      = 805,
    },
    [2] =
      {                        /* SVS+ 75mV */
        .modeEnum = VCS_CORNER_LOW_PLUS,
        .stepQuotient = 40,
        .idleClocks = 15,
        .initialFloorVoltage = 775000,
        .initialCeilingVoltage = 950000,
        .factoryOpenLoopCeilingCalculationVoltage = 950000,
        .floorCalc_correction = 0,
        .oscCfg[0].target = 499,
        .oscCfg[1].target = 476,
        .oscCfg[2].target = 528,
        .oscCfg[3].target = 519,
        .oscCfg[4].target = 880,
        .oscCfg[5].target = 822,
        .oscCfg[6].target = 947,
        .oscCfg[7].target = 910,
      },
    [3] =
    {                          /*NOM 75mV */
      .modeEnum             = VCS_CORNER_NOMINAL,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 850000,
      .initialCeilingVoltage = 1012500,
      .factoryOpenLoopCeilingCalculationVoltage = 1000000,  //This is for calculations only.  The NOM voltage ceiling has been moved to 
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 616,
      .oscCfg[1].target      = 590,
      .oscCfg[2].target      = 652,
      .oscCfg[3].target      = 642,
      .oscCfg[4].target      = 1050,
      .oscCfg[5].target      = 987,
      .oscCfg[6].target      = 1129,
      .oscCfg[7].target      = 1089,
    },
    [4] =
    {                          /* Not in current spreadsheet. Using turbo values */
      .modeEnum             = VCS_CORNER_NOMINAL_PLUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 937500,
      .initialCeilingVoltage = 1112500,    
      .factoryOpenLoopCeilingCalculationVoltage = 1112500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 616,
      .oscCfg[1].target      = 590,
      .oscCfg[2].target      = 652,
      .oscCfg[3].target      = 642,
      .oscCfg[4].target      = 1050,
      .oscCfg[5].target      = 987,
      .oscCfg[6].target      = 1129,
      .oscCfg[7].target      = 1089,
    },
    [5] =
    {                    /* TURBO 75mV  */
      .modeEnum             = VCS_CORNER_TURBO,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 937500,
      .initialCeilingVoltage = 1112500,  
      .factoryOpenLoopCeilingCalculationVoltage = 1112500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 819,
      .oscCfg[1].target      = 792,
      .oscCfg[2].target      = 870,
      .oscCfg[3].target      = 857,
      .oscCfg[4].target      = 1290,
      .oscCfg[5].target      = 1226,
      .oscCfg[6].target      = 1386,
      .oscCfg[7].target      = 1344,
    },
};
  
//----------------------------------------------------------------
// Version 2 quotients
//----------------------------------------------------------------
HalCprVmodeCfgType vmode_mode_settings_9x35V2[NUM_9x35_VMODES] =
   {  [0] =
     {                           /* SVS2 30mv*/
      .modeEnum              = VCS_CORNER_LOW_MINUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 700000,
      .initialCeilingVoltage = 800000,
      .factoryOpenLoopCeilingCalculationVoltage = 800000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 168,
      .oscCfg[1].target      = 150,
      .oscCfg[2].target      = 175,
      .oscCfg[3].target      = 170,
      .oscCfg[4].target      = 433,
      .oscCfg[5].target      = 382,
      .oscCfg[6].target      = 468,
      .oscCfg[7].target      = 437,
     },
    [1] =
    {                        /* SVS 40mV */                          
      .modeEnum             = VCS_CORNER_LOW,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 725000,
      .initialCeilingVoltage = 900000,
      .factoryOpenLoopCeilingCalculationVoltage = 900000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 329,
      .oscCfg[1].target      = 308,
      .oscCfg[2].target      = 346,
      .oscCfg[3].target      = 340,
      .oscCfg[4].target      = 661,
      .oscCfg[5].target      = 605,
      .oscCfg[6].target      = 712,
      .oscCfg[7].target      = 677,
    },
    [2] =
      {                        /* SVS+ 8mV */
        .modeEnum = VCS_CORNER_LOW_PLUS,
        .stepQuotient = 40,
        .idleClocks = 15,
        .initialFloorVoltage = 775000,
        .initialCeilingVoltage = 950000,
        .factoryOpenLoopCeilingCalculationVoltage = 950000,
        .floorCalc_correction = 0,
        .oscCfg[0].target = 354,
        .oscCfg[1].target = 332,
        .oscCfg[2].target = 372,
        .oscCfg[3].target = 365,
        .oscCfg[4].target = 704,
        .oscCfg[5].target = 648,
        .oscCfg[6].target = 759,
        .oscCfg[7].target = 723,
      },
    [3] =
    {                          /* NOM 0mV */
      .modeEnum             = VCS_CORNER_NOMINAL,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 850000,
      .initialCeilingVoltage = 1012500,
      .factoryOpenLoopCeilingCalculationVoltage = 1000000,  //This is for calculations only.  The NOM voltage ceiling has been moved to 
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 430,
      .oscCfg[1].target      = 407,
      .oscCfg[2].target      = 453,
      .oscCfg[3].target      = 446,
      .oscCfg[4].target      = 810,
      .oscCfg[5].target      = 752,
      .oscCfg[6].target      = 873,
      .oscCfg[7].target      = 836,
    },
    [4] =
    {                          /* Not in current spreadsheet. Using turbo values */
      .modeEnum             = VCS_CORNER_NOMINAL_PLUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 937500,
      .initialCeilingVoltage = 1112500,      
      .factoryOpenLoopCeilingCalculationVoltage = 1112500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 623,
      .oscCfg[1].target      = 599,
      .oscCfg[2].target      = 660,
      .oscCfg[3].target      = 650,
      .oscCfg[4].target      = 1039,
      .oscCfg[5].target      = 979,
      .oscCfg[6].target      = 1117,
      .oscCfg[7].target      = 1078,
    },
    [5] =
    {                    /* TURBO 0mV */
      .modeEnum             = VCS_CORNER_TURBO,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 937500,
      .initialCeilingVoltage = 1112500,      
      .factoryOpenLoopCeilingCalculationVoltage = 1112500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 623,
      .oscCfg[1].target      = 599,
      .oscCfg[2].target      = 660,
      .oscCfg[3].target      = 650,
      .oscCfg[4].target      = 1039,
      .oscCfg[5].target      = 979,
      .oscCfg[6].target      = 1117,
      .oscCfg[7].target      = 1078,
    },
};




/*----------------------------------------------------------------------------
 * Public interfaces
 * -------------------------------------------------------------------------*/
/**
 * <!-- CprGetTargetConfiguration -->
 *
 * @brief Get target-specific configuration info, including reading efuses etc.
 *
 * @param cprContext: cprContext.targetCfg and cprContext.halTargetCfg will
 * will be set to non-NULL pointers.
 */
void CprGetTargetConfiguration( void )
{
  DalChipInfoVersionType chipVersion;
  uint32 pmic_version;
  int i;
  static boolean cprTargetInitDone = FALSE;

  if (cprTargetInitDone)
  {
    return;
  }
  cprTargetInitDone = TRUE;
  
  CORE_VERIFY(chipVersion = DalChipInfo_ChipVersion());
  
  //read the chip reference 
  cprContext.fuse_reference_value = *(uint32*)0XFC4B80B4;
 
  // New feature April 2014 Floor calculations will be enabled after validation
  cprContext.enableFlags_OpenLoop_FloorVoltageEnabled = TRUE;

  // Fill in more cprContext info with the target config pointer. 
  cprContext.targetCfg = &cprTargetCfg;
  
  // Initialize the vmode array pointers to null.
  // The halCprTargetCfg.halVmodeCfgPtrs array will hold quotients, initial voltages, and 
  // other mode (SVS, SVS+ etc) info.  The mode enums determine which pointers get filled in.
  for (i=0; i<CPR_MAX_VMODE_COUNT; i++)
  {
    halCprTargetCfg.halVmodeCfgPtrs[i] = 0;
  }

  //store the version text and sensor mask info info 
  cprContext.halTargetCfg = &halCprTargetCfg;

  
  //fill in the modes that we have configurations for in this file. 
  for (i=0; i<NUM_9x35_VMODES; i++)
  {
  
    if (chipVersion >= 0x20000) 
    { //Load the V2 values
	
      //make sure the enum will fit in the list of modes. 
      if (vmode_mode_settings_9x35V2[i].modeEnum >= CPR_MAX_VMODE_COUNT){
        //ULogFront_RealTimePrintf(cprContext->ulogHandle, 0, "Fatal Error mode enum larger than supported range!");
        CORE_VERIFY(0);
      }

      // Put the pointer to the HalCprVmodeCfgType in the appropriate location so they can be looked up based on the enums.
      // The unused slots remain NULL.
      halCprTargetCfg.halVmodeCfgPtrs[vmode_mode_settings_9x35V2[i].modeEnum] = &(vmode_mode_settings_9x35V2[i]);

      //fill in the vmodeBoundsAndOffset pointers in appropriate location so they can be looked up based on the enums
      cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode_mode_settings_9x35V2[i].modeEnum] = &(boundsAndOffsetCfg9x35[i]);
	  
    } else { //Load the V1 values
	
	  //make sure the enum will fit in the list of modes. 
      if (vmode_mode_settings_9x35V1[i].modeEnum >= CPR_MAX_VMODE_COUNT){
        // add an error message to the ulog to try to make this easy to debug if it ever happens. 
        //ULogFront_RealTimePrintf(cprContext->ulogHandle, 0, "Fatal Error mode enum larger than supported range!");
        CORE_VERIFY(0);
      }

      // Put the pointer to the HalCprVmodeCfgType in the appropriate location so they can be looked up based on the enums.
      // The unused slots remain NULL.
      halCprTargetCfg.halVmodeCfgPtrs[vmode_mode_settings_9x35V1[i].modeEnum] = &(vmode_mode_settings_9x35V1[i]);

      //fill in the vmodeBoundsAndOffset pointers in appropriate location so they can be looked up based on the enums
      cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode_mode_settings_9x35V1[i].modeEnum] = &(boundsAndOffsetCfg9x35[i]);
    }
  }
  
  // Some items in cprTargetCfg should be read in from efuses. (per chip CPR enable, initVoltOffsetSteps) 
  cprContext.enableFlags_DisabledByFuse = CprEfuseGetData(&cprEfuseMasterDb, &(cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[0]));

  // Disable CPR pending validation and enable decisions.
  cprContext.enableFlags_ClosedLoopEnabled = TRUE;
  
  pmic_version = pm_get_pmic_model(0);       //get the (0 primary) pmic info	

  cprContext.detectedPMIC = pmic_version;   //store the PMIC for debug and logging later 
  cprContext.detectedChipVer = chipVersion; //store the chip version for debug and logging later 

  //9x35 needs the Mem and IO unclamped at init.  
  HalCprUnclampMemAndIO();
}


/**
 * <!-- CprGetTargetCeilingVoltageForMode -->
 *
 * @brief Calculate the ceiling voltage based on the limits and configuration/
 *
 * @param vmode : enum for the corner (SVS, NOM etc) 
 * @param voltageUV : pointer to uint32 where the voltage will be returned. 
 *
 * @return Currently always returns DAL_SUCCESS, function will error fatal if an unknow enum is used.
 */
DALResult CprGetTargetCeilingVoltageForMode( uint32 vmode, uint32* voltageUV ){
  int proposed_cpr_ceiling;

  // Get the target configuration setup, will return if already done.
  CprGetTargetConfiguration();
  
  //Make sure the mode we're being asked about is one that we have 
  //configuration data for.
  CORE_VERIFY(halCprTargetCfg.halVmodeCfgPtrs[vmode] != NULL);

  // If only the ceiling was being used:
  //proposed_cpr_ceiling = halCprTargetCfg.halVmodeCfgPtrs[vmode]->initialCeilingVoltage;

  // Apply the open loop fuse adjustment
  proposed_cpr_ceiling = cprContext.halTargetCfg->halVmodeCfgPtrs[vmode]->initialCeilingVoltage +
           (cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode]->initVoltOffsetSteps * 10000);

  //clip positive voltages above the ceiling
  if (proposed_cpr_ceiling > cprContext.halTargetCfg->halVmodeCfgPtrs[vmode]->initialCeilingVoltage)
  {
    proposed_cpr_ceiling = cprContext.halTargetCfg->halVmodeCfgPtrs[vmode]->initialCeilingVoltage;
  } 

  //round up to the nearest PMIC step
  if(proposed_cpr_ceiling%12500)
        proposed_cpr_ceiling += 12500 - proposed_cpr_ceiling%12500;
		
  CORE_VERIFY(proposed_cpr_ceiling%12500==0);  

  *voltageUV = proposed_cpr_ceiling;
  return DAL_SUCCESS;
}


/**
 * <!-- CprGetTargetFloorVoltageForMode -->
 *
 * @brief Calculate the floor voltage based on the limits and configuration/
 *
 * @param vmode : enum for the corner (SVS, NOM etc) 
 * @param voltageUV : pointer to uint32 where the voltage will be returned. 
 *
 * @return Currently always returns DAL_SUCCESS, function will error fatal if an unknow enum is used.
 */
DALResult CprGetTargetFloorVoltageForMode( uint32 vmode, uint32* voltageUV ){
  uint32 proposed_cpr_floor;
  
  // Get the target configuration setup, will return if already done.
  CprGetTargetConfiguration();

  //Make sure the mode we're being asked about is one that we have 
  //configuration data for.
  CORE_VERIFY(halCprTargetCfg.halVmodeCfgPtrs[vmode] != NULL);

  proposed_cpr_floor = halCprTargetCfg.halVmodeCfgPtrs[vmode]->initialFloorVoltage;

  if (cprContext.enableFlags_OpenLoop_FloorVoltageEnabled == TRUE)
  {  
    uint32 capped_mss_fuse;
    int32 fuse = cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode]->initVoltOffsetSteps;

	//the fuse should be 0 or a negative value
    capped_mss_fuse = fuse > 0 ? 0 : fuse;

	//apply the formula
    proposed_cpr_floor = ((halCprTargetCfg.halVmodeCfgPtrs[vmode]->initialCeilingVoltage * 90)/100)
                            + 10000 * capped_mss_fuse + 37500;

    //apply the floorCalc_correction
    proposed_cpr_floor = proposed_cpr_floor +  halCprTargetCfg.halVmodeCfgPtrs[vmode]->floorCalc_correction;
	
	  //clip to the minimum voltage for the corner.  
    if (proposed_cpr_floor < halCprTargetCfg.halVmodeCfgPtrs[vmode]->initialFloorVoltage){
      proposed_cpr_floor = halCprTargetCfg.halVmodeCfgPtrs[vmode]->initialFloorVoltage;
    }
  } 

  //round up to the nearest PMIC step
  if(proposed_cpr_floor%12500)
        proposed_cpr_floor += 12500 - proposed_cpr_floor%12500;

  CORE_VERIFY(proposed_cpr_floor%12500==0);
  
  *voltageUV = proposed_cpr_floor;

  return DAL_SUCCESS;
}

