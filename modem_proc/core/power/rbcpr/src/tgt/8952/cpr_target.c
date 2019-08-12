/*============================================================================
@file cpr_target.c

Target-specific CPR configuration data for 8952.

Copyright © 2013, 2015 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/tgt/8952/cpr_target.c#1 $
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

//there are 6 fuses
#define NUM_8952_VMODES_FUSES 6


/** Database of fuse locations and fields we want to read from. */
static CprEfuseMasterDatabaseType cprEfuseMasterDb =
{
  //.redundancySel = { HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR, 0xe0000000, 29 },
  .redundancySel = { 0, 0, 0 },

  .numFuseEntries = NUM_8952_VMODES_FUSES,
  .primaryDb = 
  {
    //.disCpr  = {HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, 0x100000, 20},
    .disCpr  = {0, 0, 0},
    .fuseVmodeCfgPtrs =
    {
      //MSS CPR fuses are CPR 1 on 9x45
      [0] =
      {
        .modeEnum  = VCS_CORNER_LOW_MINUS,  //SVS2
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_SHFT},  
        .shiftLeftBy = 0,
      },
      [1] =
      {
        .modeEnum  = VCS_CORNER_LOW,		//SVS
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_SHFT},  
        .shiftLeftBy = 0,
      },
      [2] =
      {
        .modeEnum = VCS_CORNER_LOW,		//SVS+
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_SHFT},  
        .shiftLeftBy = 0,
      },

      [3] =
      {
        .modeEnum  = VCS_CORNER_NOMINAL,
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_NOM_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_NOM_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_NOM_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_NOM_SHFT},  
        .shiftLeftBy = 0,
      },
      [4] = 
      {
        .modeEnum  = VCS_CORNER_NOMINAL_PLUS,
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_SHFT},  
        .shiftLeftBy = 0,
      },
      [5] =
      {
        .modeEnum  = VCS_CORNER_TURBO,
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_SHFT},  
        .shiftLeftBy = 0,        
      }
    }
  },
  .redundantDb =
  {
    .disCpr  = {0, 0, 0},
    .fuseVmodeCfgPtrs =
     { 
      [0] =
      {
        .modeEnum  = VCS_CORNER_LOW_MINUS,  //SVS2
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_SHFT},  
        .shiftLeftBy = 0,
      },
      [1] =
      {
        .modeEnum  = VCS_CORNER_LOW_MINUS,  //SVS2
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_SHFT},  
        .shiftLeftBy = 0,
      },
      [2] =
        {
        .modeEnum  = VCS_CORNER_LOW_MINUS,  //SVS2
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_SVS_3_0_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR1_TARG_VOLT_SVS_4_SHFT},  
        .shiftLeftBy = 0,
        },

      [3] =
      {
        .modeEnum  = VCS_CORNER_NOMINAL,
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_NOM_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_NOM_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_NOM_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_NOM_SHFT},  
        .shiftLeftBy = 0,
      },
      [4] = 
      {
        .modeEnum  = VCS_CORNER_NOMINAL_PLUS,
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_SHFT},  
        .shiftLeftBy = 0,
      },
      [5] =
      {
        .modeEnum  = VCS_CORNER_NOMINAL_PLUS,
        .targetVsteps0 = {HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_SHFT},  
        .targetVsteps1 = {HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_BMSK,
                          HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARG_VOLT_TUR_SHFT},  
        .shiftLeftBy = 0,
      }
    }
  }
};



#define NUM_8952_VMODES 6

/** Target specific config. */
static CprTargetCfgType cprTargetCfg =
{
  .irq                     = 91,       // From MSS HPG document
  .delayBeforeNextMeas     = 5,        // millisecs
  .voltageStepSize         = 12500,    // microVolts
  .numVmodeQuotientsAvail  = NUM_8952_VMODES,

};



CprVmodeBoundsAndOffsetCfgType boundsAndOffsetCfg8952[NUM_8952_VMODES] =
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
      .configVersionText = "8952 10_16",

      .sensorMask[0] = 0,
      .sensorMask[1] = 0,

      .speedpushed_ROs = 0xC,     //2 and 3 are masked

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


HalCprVmodeCfgType vmode_mode_settings_8952v1[NUM_8952_VMODES] =
   {  [0] =
     {                           /* Low SVS 5mV*/
      .modeEnum              = VCS_CORNER_RETENTION,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 700000,
      .initialCeilingVoltage = 800000,
      .factoryOpenLoopCeilingCalculationVoltage = 800000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 114,
      .oscCfg[1].target      = 96,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 367,
      .oscCfg[5].target      = 316,
      .oscCfg[6].target      = 398,
      .oscCfg[7].target      = 367,
     },
    [1] =
    {                           /* SVS 27.5mV */
      .modeEnum             = VCS_CORNER_LOW_MINUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 725000,
      .initialCeilingVoltage = 900000, 
      .factoryOpenLoopCeilingCalculationVoltage = 900000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 302,
      .oscCfg[1].target      = 281,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 628,
      .oscCfg[5].target      = 573,
      .oscCfg[6].target      = 677,
      .oscCfg[7].target      = 643,
    },
    [2] =
      {                           /* SVS+ 25mV */
        .modeEnum = VCS_CORNER_LOW,
        .stepQuotient = 40,
        .idleClocks = 15,
        .initialFloorVoltage = 775000,    
        .initialCeilingVoltage = 950000, 
        .factoryOpenLoopCeilingCalculationVoltage = 950000,
        .floorCalc_correction = 0,
        .oscCfg[0].target = 391,
        .oscCfg[1].target = 368,
        .oscCfg[2].target = 0,
        .oscCfg[3].target = 0,
        .oscCfg[4].target = 749,
        .oscCfg[5].target = 692,
        .oscCfg[6].target = 807,
        .oscCfg[7].target = 770,
      },
    [3] =
    {                           /* NOM 12.5mV */
      .modeEnum             = VCS_CORNER_NOMINAL,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 837500,
      .initialCeilingVoltage = 1000000,   
      .factoryOpenLoopCeilingCalculationVoltage = 1000000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 458,
      .oscCfg[1].target      = 434,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 843,
      .oscCfg[5].target      = 785,
      .oscCfg[6].target      = 908,
      .oscCfg[7].target      = 870,
    },
    [4] =
    {                           /* NOM+  25mV  */
      .modeEnum             = VCS_CORNER_NOMINAL_PLUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 900000,
      .initialCeilingVoltage = 1087500,
      .factoryOpenLoopCeilingCalculationVoltage = 1087500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 615,
      .oscCfg[1].target      = 590,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 1030,
      .oscCfg[5].target      = 971,
      .oscCfg[6].target      = 1108,
      .oscCfg[7].target      = 1069,
    },
    [5] =
    {                           /* TUR 0mV */
      .modeEnum             = VCS_CORNER_TURBO,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 937500,
      .initialCeilingVoltage = 1112500, 
      .factoryOpenLoopCeilingCalculationVoltage = 1112500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 623,
      .oscCfg[1].target      = 599,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 1039,
      .oscCfg[5].target      = 979,
      .oscCfg[6].target      = 1117,
      .oscCfg[7].target      = 1078,
    },
};


HalCprVmodeCfgType vmode_mode_settings_8952v2[NUM_8952_VMODES] =
   {  [0] =
     {                           /* Low SVS 30mV*/
      .modeEnum              = VCS_CORNER_RETENTION,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 700000,
      .initialCeilingVoltage = 800000,
      .factoryOpenLoopCeilingCalculationVoltage = 800000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 168,
      .oscCfg[1].target      = 150,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 433,
      .oscCfg[5].target      = 382,
      .oscCfg[6].target      = 468,
      .oscCfg[7].target      = 437,
     },
    [1] =
    {                           /* SVS 28mV */
      .modeEnum             = VCS_CORNER_LOW_MINUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 725000,
      .initialCeilingVoltage = 900000, 
      .factoryOpenLoopCeilingCalculationVoltage = 900000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 303,
      .oscCfg[1].target      = 282,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 629,
      .oscCfg[5].target      = 574,
      .oscCfg[6].target      = 679,
      .oscCfg[7].target      = 644,
    },
    [2] =
    {                           /* SVS+ 33mV */
      .modeEnum = VCS_CORNER_LOW,
      .stepQuotient = 40,
      .idleClocks = 15,
      .initialFloorVoltage = 775000,    
      .initialCeilingVoltage = 950000, 
      .factoryOpenLoopCeilingCalculationVoltage = 950000,
      .floorCalc_correction = 0,
      .oscCfg[0].target = 408,
      .oscCfg[1].target = 386,
      .oscCfg[2].target = 0,
      .oscCfg[3].target = 0,
      .oscCfg[4].target = 770,
      .oscCfg[5].target = 712,
      .oscCfg[6].target = 829,
      .oscCfg[7].target = 792,
    },
    [3] =
    {                           /* NOM 0mV */
      .modeEnum             = VCS_CORNER_NOMINAL,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 837500,
      .initialCeilingVoltage = 1000000,   
      .factoryOpenLoopCeilingCalculationVoltage = 1000000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 430,
      .oscCfg[1].target      = 407,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 810,
      .oscCfg[5].target      = 752,
      .oscCfg[6].target      = 873,
      .oscCfg[7].target      = 836,
    },
    [4] =
    {                           /* NOM+  25mV  */
      .modeEnum             = VCS_CORNER_NOMINAL_PLUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 900000,
      .initialCeilingVoltage = 1087500,
      .factoryOpenLoopCeilingCalculationVoltage = 1087500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 615,
      .oscCfg[1].target      = 590,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 1030,
      .oscCfg[5].target      = 971,
      .oscCfg[6].target      = 1108,
      .oscCfg[7].target      = 1069,
    },
    [5] =
    {                           /* TUR 0mV */
      .modeEnum             = VCS_CORNER_TURBO,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 937500,
      .initialCeilingVoltage = 1112500, 
      .factoryOpenLoopCeilingCalculationVoltage = 1112500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 623,
      .oscCfg[1].target      = 599,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 1039,
      .oscCfg[5].target      = 979,
      .oscCfg[6].target      = 1117,
      .oscCfg[7].target      = 1078,
    },
};


HalCprVmodeCfgType vmode_mode_settings_8992[NUM_8952_VMODES] =
   {  [0] =
     {                           /* Low SVS 5mV*/
      .modeEnum              = VCS_CORNER_RETENTION,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 700000,
      .initialCeilingVoltage = 800000,
      .factoryOpenLoopCeilingCalculationVoltage = 800000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 114,
      .oscCfg[1].target      = 96,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 367,
      .oscCfg[5].target      = 316,
      .oscCfg[6].target      = 398,
      .oscCfg[7].target      = 367,
     },
    [1] =
    {                           /* SVS 27.5mV */
      .modeEnum             = VCS_CORNER_LOW_MINUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 725000,
      .initialCeilingVoltage = 900000, 
      .factoryOpenLoopCeilingCalculationVoltage = 900000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 302,
      .oscCfg[1].target      = 281,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 628,
      .oscCfg[5].target      = 573,
      .oscCfg[6].target      = 677,
      .oscCfg[7].target      = 643,
    },
    [2] =
      {                           /* SVS+ 25mV */
        .modeEnum = VCS_CORNER_LOW,
        .stepQuotient = 40,
        .idleClocks = 15,
        .initialFloorVoltage = 775000,    
        .initialCeilingVoltage = 950000, 
        .factoryOpenLoopCeilingCalculationVoltage = 950000,
        .floorCalc_correction = 0,
        .oscCfg[0].target = 391,
        .oscCfg[1].target = 368,
        .oscCfg[2].target = 0,
        .oscCfg[3].target = 0,
        .oscCfg[4].target = 749,
        .oscCfg[5].target = 692,
        .oscCfg[6].target = 807,
        .oscCfg[7].target = 770,
      },
    [3] =
    {                           /* NOM 12.5mV */
      .modeEnum             = VCS_CORNER_NOMINAL,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 837500,
      .initialCeilingVoltage = 1000000,   
      .factoryOpenLoopCeilingCalculationVoltage = 1000000,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 458,
      .oscCfg[1].target      = 434,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 843,
      .oscCfg[5].target      = 785,
      .oscCfg[6].target      = 908,
      .oscCfg[7].target      = 870,
    },
    [4] =
    {                           /* NOM+  25mV  */
      .modeEnum             = VCS_CORNER_NOMINAL_PLUS,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 900000,
      .initialCeilingVoltage = 1087500,
      .factoryOpenLoopCeilingCalculationVoltage = 1087500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 615,
      .oscCfg[1].target      = 590,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
      .oscCfg[4].target      = 1030,
      .oscCfg[5].target      = 971,
      .oscCfg[6].target      = 1108,
      .oscCfg[7].target      = 1069,
    },
    [5] =
    {                           /* TUR 0mV */
      .modeEnum             = VCS_CORNER_TURBO,
      .stepQuotient          = 40,
      .idleClocks            = 15,
  	  .initialFloorVoltage   = 937500,
      .initialCeilingVoltage = 1112500, 
      .factoryOpenLoopCeilingCalculationVoltage = 1112500,
      .floorCalc_correction  = 0,
      .oscCfg[0].target      = 623,
      .oscCfg[1].target      = 599,
      .oscCfg[2].target      = 0,
      .oscCfg[3].target      = 0,
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
  DalChipInfoFamilyType familyVersion;
  uint32 pmic_version;
  int i;
  static boolean cprTargetInitDone = FALSE;

  if (cprTargetInitDone)
  {
    return;
  }
  cprTargetInitDone = TRUE;
  
  CORE_VERIFY(familyVersion = DalChipInfo_ChipFamily());
  CORE_VERIFY(chipVersion = DalChipInfo_ChipVersion());
  
 
  //read the chip reference 
  cprContext.fuse_reference_value = 0; //*(uint32*)0XFC4B81F0;
 
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

  if (familyVersion == DALCHIPINFO_FAMILY_MSM8952)
  {
    if(chipVersion == 0x10000)
    {
      // ====== v1 8952 =================
      //fill in the modes that we have configurations for in this file. 
      for (i=0; i<NUM_8952_VMODES; i++)
      {
        //make sure the enum will fit in the list of modes. 
        if (vmode_mode_settings_8952v1[i].modeEnum >= CPR_MAX_VMODE_COUNT){
          // add an error message to the ulog to try to make this easy to debug if it ever happens. 
          //ULogFront_RealTimePrintf(cprContext.ulogHandle, 0, "Fatal Error mode enum larger than supported range!");
          CORE_VERIFY(0);
        }

        // Put the pointer to the HalCprVmodeCfgType in the appropriate location so they can be looked up based on the enums.
        // The unused slots remain NULL.
        halCprTargetCfg.halVmodeCfgPtrs[vmode_mode_settings_8952v1[i].modeEnum] = &(vmode_mode_settings_8952v1[i]);

        //fill in the vmodeBoundsAndOffset pointers in appropriate location so they can be looked up based on the enums
        cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode_mode_settings_8952v1[i].modeEnum] = &(boundsAndOffsetCfg8952[i]);
      }
      // Enable Closed Loop CPR on v1 8952
      cprContext.enableFlags_ClosedLoopEnabled = TRUE;
    } else {
      // ====== assume v2 of 8952 =================
      //fill in the modes that we have configurations for in this file. 
      for (i=0; i<NUM_8952_VMODES; i++)
      {
        //make sure the enum will fit in the list of modes. 
        if (vmode_mode_settings_8952v2[i].modeEnum >= CPR_MAX_VMODE_COUNT){
          // add an error message to the ulog to try to make this easy to debug if it ever happens. 
          //ULogFront_RealTimePrintf(cprContext.ulogHandle, 0, "Fatal Error mode enum larger than supported range!");
          CORE_VERIFY(0);
        }

        // Put the pointer to the HalCprVmodeCfgType in the appropriate location so they can be looked up based on the enums.
        // The unused slots remain NULL.
        halCprTargetCfg.halVmodeCfgPtrs[vmode_mode_settings_8952v2[i].modeEnum] = &(vmode_mode_settings_8952v2[i]);

        //fill in the vmodeBoundsAndOffset pointers in appropriate location so they can be looked up based on the enums
        cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode_mode_settings_8952v2[i].modeEnum] = &(boundsAndOffsetCfg8952[i]);
      }
      // Enable Closed Loop CPR on v2 8952
      cprContext.enableFlags_ClosedLoopEnabled = TRUE;	
    }
  } else {
   // ====== 8992 =================
   //fill in the modes that we have configurations for in this file. 
    for (i=0; i<NUM_8952_VMODES; i++)
    {
      //make sure the enum will fit in the list of modes. 
      if (vmode_mode_settings_8992[i].modeEnum >= CPR_MAX_VMODE_COUNT){
        // add an error message to the ulog to try to make this easy to debug if it ever happens. 
        //ULogFront_RealTimePrintf(cprContext.ulogHandle, 0, "Fatal Error mode enum larger than supported range!");
        CORE_VERIFY(0);
      }

      // Put the pointer to the HalCprVmodeCfgType in the appropriate location so they can be looked up based on the enums.
      // The unused slots remain NULL.
      halCprTargetCfg.halVmodeCfgPtrs[vmode_mode_settings_8992[i].modeEnum] = &(vmode_mode_settings_8992[i]);

      //fill in the vmodeBoundsAndOffset pointers in appropriate location so they can be looked up based on the enums
      cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode_mode_settings_8992[i].modeEnum] = &(boundsAndOffsetCfg8952[i]);
    }
      // Disable CPR on 8992 pending validation and enable decisions.
    cprContext.enableFlags_ClosedLoopEnabled = FALSE;	
  }	 
    
  // Some items in cprTargetCfg should be read in from efuses. (per chip CPR enable, initVoltOffsetSteps) 
  cprContext.enableFlags_DisabledByFuse = CprEfuseGetData(&cprEfuseMasterDb, &(cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[0]));


  
  pmic_version = pm_get_pmic_model(0);       //get the (0 primary) pmic info	

  cprContext.detectedPMIC = pmic_version;   //store the PMIC for debug and logging later 
  cprContext.detectedChipVer = chipVersion; //store the chip version for debug and logging later 

  //9x35 needed the Mem and IO unclamped at init.  
  //HalCprUnclampMemAndIO();
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

