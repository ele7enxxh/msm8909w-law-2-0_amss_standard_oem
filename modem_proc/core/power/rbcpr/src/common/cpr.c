/*============================================================================
@file cpr.c

Implements the Rapid Bridge Core Power Reduction driver.

Copyright © 2013, 2015 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/common/cpr.c#1 $
============================================================================*/
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "CoreVerify.h"
#include "cpr_internal.h"
#include "VCSDefs.h" /* Enums for the power modes */
#include "npa.h"
#include "sleep_lpr.h"
#include "ULogFront.h"
#include "DALSys.h"
#include "cpr.h"
#ifdef WINSIM
#include "cpr_offline.h"
#endif /* WINSIM */

/** Contains all the CPR context data. */
CprContextType cprContext;
CprApiType cprApi;


/**
 * <!-- CprInitStateData -->
 *
 * @brief Set initial values for the cprContext data structure before 
 * beginning to use the HW.
 */
inline static void CprInitStateData( void )
{
  uint32 vmode;

  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprInitStateData");

  /* Until we get a callback from clkrgm, we won't know what voltage mode
     clkrgm is currently using. */
  cprContext.currentVmode = CPR_VMODE_INVALID;

  /* Correct each mode's starting voltage with the offset specified in the  fuse target config values.
     This offset is typically derived from HW characterization. */
  for (vmode=0; vmode<CPR_MAX_VMODE_COUNT; vmode++)
  {
    //Do the initialization only for modes we're using. 
    if (cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode] != NULL)
    {
      //fill in the current voltage for the mode. 
      CprGetFloorAndCeiling(VCS_RAIL_MSS, vmode, &(cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode]->voltageFloor),&(cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode]->voltageCeiling));
      
      //The effective ceiling becomes the starting voltage
      cprContext.measurements[vmode].currentVoltage = cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode]->voltageCeiling;
      
      ULOG_RT_PRINTF_6(cprContext.ulogHandle, "Calculated initials Vmode %u (min: %u) (floor = %u) (ceiling = %u) (max: %u) (currentVoltage: %u)", 
                                              vmode,
                                              cprContext.halTargetCfg->halVmodeCfgPtrs[vmode]->initialFloorVoltage,
                                              cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode]->voltageFloor,
                                              cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode]->voltageCeiling,
                                              cprContext.halTargetCfg->halVmodeCfgPtrs[vmode]->initialCeilingVoltage,
                                              cprContext.measurements[vmode].currentVoltage);
      
      //initialize the minimum voltage debug information.
      cprContext.debugMinVoltageInEachMode[vmode] = cprContext.measurements[vmode].currentVoltage;
    }      
  }

  // Initialize the static log
  ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle, 
                          CPR_ULOG_STATIC_SLOT1_START_VOLTAGE_INFO,
                          "Starting Voltages (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)", 
                          cprContext.measurements[VCS_CORNER_RETENTION].currentVoltage, cprContext.measurements[VCS_CORNER_LOW_MINUS].currentVoltage, cprContext.measurements[VCS_CORNER_LOW].currentVoltage,
                          cprContext.measurements[VCS_CORNER_NOMINAL].currentVoltage, cprContext.measurements[VCS_CORNER_NOMINAL_PLUS].currentVoltage,
                          cprContext.measurements[VCS_CORNER_TURBO].currentVoltage);
  ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle, 
                          CPR_ULOG_STATIC_SLOT2_CURR_VOLTAGE_INFO,
                          "Current Voltages (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)", 
                          cprContext.measurements[VCS_CORNER_RETENTION].currentVoltage, cprContext.measurements[VCS_CORNER_LOW_MINUS].currentVoltage, cprContext.measurements[VCS_CORNER_LOW].currentVoltage,
                          cprContext.measurements[VCS_CORNER_NOMINAL].currentVoltage, cprContext.measurements[VCS_CORNER_NOMINAL_PLUS].currentVoltage,
                          cprContext.measurements[VCS_CORNER_TURBO].currentVoltage);		
  ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle, 
                          CPR_ULOG_STATIC_SLOT3_MIN_VOLTAGE_INFO,
                          "Min Voltages (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)", 
                          cprContext.debugMinVoltageInEachMode[VCS_CORNER_RETENTION], cprContext.debugMinVoltageInEachMode[VCS_CORNER_LOW_MINUS], cprContext.debugMinVoltageInEachMode[VCS_CORNER_LOW],
                          cprContext.debugMinVoltageInEachMode[VCS_CORNER_NOMINAL], cprContext.debugMinVoltageInEachMode[VCS_CORNER_NOMINAL_PLUS],
                          cprContext.debugMinVoltageInEachMode[VCS_CORNER_TURBO]);							
  ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle, 
	                        CPR_ULOG_STATIC_SLOT5_UPISR_INFO,
                          "Up ISR count (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)", 
                          cprContext.debugUpIsrInEachMode[VCS_CORNER_RETENTION], cprContext.debugUpIsrInEachMode[VCS_CORNER_LOW_MINUS], cprContext.debugUpIsrInEachMode[VCS_CORNER_LOW],
                          cprContext.debugUpIsrInEachMode[VCS_CORNER_NOMINAL], cprContext.debugUpIsrInEachMode[VCS_CORNER_NOMINAL_PLUS],
                          cprContext.debugUpIsrInEachMode[VCS_CORNER_TURBO]);
  ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle, 
                          CPR_ULOG_STATIC_SLOT6_DOWNISR_INFO,
                          "Down ISR count (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)", 
                          cprContext.debugDownIsrInEachMode[VCS_CORNER_RETENTION], cprContext.debugDownIsrInEachMode[VCS_CORNER_LOW_MINUS], cprContext.debugDownIsrInEachMode[VCS_CORNER_LOW],
                          cprContext.debugDownIsrInEachMode[VCS_CORNER_NOMINAL], cprContext.debugDownIsrInEachMode[VCS_CORNER_NOMINAL_PLUS],
                          cprContext.debugDownIsrInEachMode[VCS_CORNER_TURBO]);
  ULOG_RT_STATIC_PRINTF_6(cprContext.ulogStaticHandle,
                          CPR_ULOG_STATIC_SLOT4_MODECOUNT_INFO,
                          "Entrance Count (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)",
                          cprContext.debugTimesInEachMode[VCS_CORNER_RETENTION], cprContext.debugTimesInEachMode[VCS_CORNER_LOW_MINUS], cprContext.debugTimesInEachMode[VCS_CORNER_LOW],
                          cprContext.debugTimesInEachMode[VCS_CORNER_NOMINAL], cprContext.debugTimesInEachMode[VCS_CORNER_NOMINAL_PLUS],
                          cprContext.debugTimesInEachMode[VCS_CORNER_TURBO]);
  ULOG_RT_STATIC_PRINTF_9(cprContext.ulogStaticHandle, 
                          CPR_ULOG_STATIC_SLOT7_FINFO,
                          "Debug (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d) (Redund: %u) (Disable: %u) (ref: %x)", 
                          cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[VCS_CORNER_RETENTION]->initVoltOffsetSteps,
                          cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[VCS_CORNER_LOW_MINUS]->initVoltOffsetSteps,
                          cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[VCS_CORNER_LOW]->initVoltOffsetSteps,
                          cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[VCS_CORNER_NOMINAL]->initVoltOffsetSteps,
                          cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[VCS_CORNER_NOMINAL_PLUS]->initVoltOffsetSteps,
                          cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[VCS_CORNER_TURBO]->initVoltOffsetSteps,
                          cprContext.fuse_redund,
                          cprContext.fuse_disable,
                          cprContext.fuse_reference_value);

              
}


/**
 * <!-- CprInitUlog -->
 *
 * @brief Create and initialize a "CprLog" ULog for CPR.
 */
inline static void CprInitUlog( void )
{
  static boolean cprInitUlogDone = FALSE;
  const char logName[] = "CprLog";

  if (cprInitUlogDone)
  {
    return;
  }
  cprInitUlogDone = TRUE;
  
  
 /* Create a Ulog handle */
  CORE_DAL_VERIFY( ULogFront_RealTimeInit(&cprContext.ulogHandle,
                                          logName,
                                          CPR_ULOG_DEFAULT_BUFFER_SIZE_BYTES,
                                          ULOG_MEMORY_LOCAL,
                                          ULOG_LOCK_OS));

  /* The ULog Header allows the visualization tool USee to apply the appropriate parser. */
  ULogCore_HeaderSet(cprContext.ulogHandle, "Content-Type: text/tagged-log-1.0; title=CPR running history");
  
  //create the static log and log handle for statistics
  ULogFront_RealTimeStaticInit(&cprContext.ulogStaticHandle,
                               "CprStatsLog",
                               CPR_ULOG_STATIC_NUM_SLOTS,
                               ULOG_MEMORY_LOCAL,
                               ULOG_LOCK_OS);
}


/**
 * <!-- CprPopulateVoltageLimitValues -->
 *
 * @brief Query the clock driver for the voltage ranges for each voltage mode
 * and populate our internal table with that data. The results of the query can only 
 * constrain the voltage limits further, never expand them. 
 */
void CprPopulateVoltageLimitValues( void )
{
  npa_query_handle          clkCpuQueryNpaHandle;
  npa_query_type            queryResult;
  uint32                    vmode_count;
  uint32                    vmode;
  uint32                    i;
  CprVmodeBoundsAndOffsetCfgType * vmodeBoundsAndOffsetCfgCfg;

  /* Bitfield for keeping track of modes used vs modes available. Usefull
   * if there's a case where clocks is not using all the modes we have config
   * data for. We'll set a 1 in the bit number for each mode used.*/
  uint32                    modes_used = 0;  

  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprPopulateVoltageLimitValues");

  CORE_VERIFY_PTR(clkCpuQueryNpaHandle = npa_create_query_handle("/vdd/mss"));


  //How many modes will be used on this system?
  CORE_VERIFY(NPA_QUERY_SUCCESS == npa_query(clkCpuQueryNpaHandle, VCS_NPA_QUERY_VOLTAGE_NUM_ENTRIES, &queryResult));

  vmode_count = queryResult.data.value;

  ULOG_RT_PRINTF_2(cprContext.ulogHandle, 
                   "CPR  (NumberOfClockPowerModes: %d) (NumberOfDefinedPowerModes: %d)",
                   vmode_count, cprContext.targetCfg->numVmodeQuotientsAvail);

  /* Number of levels in the clock driver must be equal or less than our expectation. */
  CORE_VERIFY(vmode_count < CPR_MAX_VMODE_COUNT);


  // We now know how many modes clkrgm has at it's disposal to use.  Cycle through each one, get the details. 
  // to populate our voltage levels. 
  for (i=0; i<vmode_count; i++)
  {
    // the variable i counts up from 0 and we get the enums for the VCS corners (e.g. LOW_MINUS, LOW, NOMINAL, TURBO).
    CORE_VERIFY(NPA_QUERY_SUCCESS == npa_query(clkCpuQueryNpaHandle, VCS_NPA_QUERY_VOLTAGE_LEVEL_ID + i, &queryResult));

    vmode = queryResult.data.value; //We recieve a VCSCornerType... enum for this mode.

    //ensure the vmode returned looks reasonable. 
    CORE_VERIFY(vmode < CPR_MAX_VMODE_COUNT);

    //record that we're filling in data for this mode
    modes_used |= 1<<vmode; 

    if (cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode] == NULL){
      // There should be a structure ready to store the ceiling and floor.  If
      // there isn't the MSS CPR configuration doesn't match the modes clocks intends
      // to use. 
      ULOG_RT_PRINTF_1(cprContext.ulogHandle, "Fatal Error! Initializing Vmode %u No MSS CPR quotient data available!", vmode);
      // Error fatal so the quotient data can be fixed. 
      CORE_VERIFY(0);
    } 
    else
    {
      // Compare our voltages against the clocks voltages. 

      // Get the pointer to the bounds and offset data for this mode. 
      vmodeBoundsAndOffsetCfgCfg = cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode];
         
      //get the floor value for the voltage level.
      CORE_VERIFY(NPA_QUERY_SUCCESS == npa_query(clkCpuQueryNpaHandle, VCS_NPA_QUERY_VOLTAGE_LEVEL_FLOOR + i, &queryResult));

      //if the VCS floor value is higher than our value raise the floor
      if ((vmodeBoundsAndOffsetCfgCfg->voltageFloor < queryResult.data.value) 
         && (queryResult.data.value < cprContext.halTargetCfg->halVmodeCfgPtrs[vmode]->initialCeilingVoltage ))
      {
        ULOG_RT_PRINTF_3(cprContext.ulogHandle,
                         "Vmode %u Floor raised from %u to %u",
                         vmode,
                         vmodeBoundsAndOffsetCfgCfg->voltageFloor,
                         queryResult.data.value);
         vmodeBoundsAndOffsetCfgCfg->voltageFloor = queryResult.data.value;
      }

      //get the ceiling value for that voltage level.
      CORE_VERIFY(NPA_QUERY_SUCCESS == npa_query(clkCpuQueryNpaHandle, VCS_NPA_QUERY_VOLTAGE_LEVEL_CEILING + i, &queryResult));

      //if the VCS ceiling voltage is lower than the current ceiling but higher than the floor we'll use it. 
      if ((vmodeBoundsAndOffsetCfgCfg->voltageCeiling > queryResult.data.value)&&
          (queryResult.data.value > cprContext.halTargetCfg->halVmodeCfgPtrs[vmode]->initialFloorVoltage ))
      {
        ULOG_RT_PRINTF_3(cprContext.ulogHandle,
                         "Vmode %u Ceiling lowered from %u to %u",
                         vmode,
                         vmodeBoundsAndOffsetCfgCfg->voltageCeiling,
                         queryResult.data.value);
        vmodeBoundsAndOffsetCfgCfg->voltageCeiling = queryResult.data.value;
        cprContext.measurements[vmode].currentVoltage = vmodeBoundsAndOffsetCfgCfg->voltageCeiling;
      }


      //on early 9x35 parts the ceiling was clipped to 1V.  Make sure the floor is below or the same as the ceiling
      if(vmodeBoundsAndOffsetCfgCfg->voltageFloor > vmodeBoundsAndOffsetCfgCfg->voltageCeiling)
      {
        
        ULOG_RT_PRINTF_3(cprContext.ulogHandle,
                         "Vmode %u Floor was higher than ceiling. Floor lowered from %u to %u",
                         vmode,
                         vmodeBoundsAndOffsetCfgCfg->voltageFloor,
                         vmodeBoundsAndOffsetCfgCfg->voltageCeiling);
        vmodeBoundsAndOffsetCfgCfg->voltageFloor = vmodeBoundsAndOffsetCfgCfg->voltageCeiling;
      }


      ULOG_RT_PRINTF_6(cprContext.ulogHandle, "Vmode %u (min: %u) (floor = %u) (ceiling = %u) (max: %u) (currentVoltage: %u)", 
                                               vmode,
                                               cprContext.halTargetCfg->halVmodeCfgPtrs[vmode]->initialFloorVoltage,
                                               cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode]->voltageFloor,
                                               cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[vmode]->voltageCeiling,
                                               cprContext.halTargetCfg->halVmodeCfgPtrs[vmode]->initialCeilingVoltage,
                                               cprContext.measurements[vmode].currentVoltage);

    }
  }

  // Use the modes_used bitfield that we've filled in to check if all the modes
  // with quotients defined in this target's configuration have been used. If a
  // mode isn't used, add a log message and zero out the pointer to it's quotients. 
  for (i=0; i<CPR_MAX_VMODE_COUNT; i++){
    if (cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[i] != NULL){
      if (!(modes_used & 1<<i)){
        // We have quotients, and a bounds and offset ready, but our query
        // didn't include this mode. Set the pointers to NULL in the
        // quotient and bounds tables. 
        cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[i] = NULL;
        cprContext.halTargetCfg->halVmodeCfgPtrs[i] = NULL;
        ULOG_RT_PRINTF_1(cprContext.ulogHandle, "Vmode %u configuration won't be used.", i);
      }
    }
  }

  npa_destroy_query_handle(clkCpuQueryNpaHandle);
}


/**
 * <!-- CprInitContinueCB -->
 *
 * @brief Continue initialization after our dependencies have become available.
 * 
 * @param context : Unused
 * @param event_type : Unused
 * @param data : Unused
 * @param data_size : Unused
 */
static void CprInitContinueCB( void         * context,
                               unsigned int   event_type,
                               void         * data,
                               unsigned int   data_size )
{
  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprInitContinueCB");
                          
  /* Register the Sleep "mss_rbcpr" LPR */
  CprInitLowPowerMgr();
 
}


/**
 * <!-- CprWaitForDependencies -->
 *
 * @brief Ask NPA to call CprInitContinueCB once clocks and sleep external
 * dependencies become available
 */
inline static void CprWaitForDependencies( void )
{
  static const char *dependencies[] =
  {
    "/clk/cpu",
    "/clk/mss/config_bus",
    SLEEP_LPR_NODE_NAME
  };

  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprWaitForDependencies");

  // Wait for clkrgm so that it can be queried for the voltage ranges of each
  // voltage mode. Also wait for sleep so that the CPR LPRM can be registered.
  npa_resources_available_cb(NPA_ARRAY(dependencies), CprInitContinueCB, NULL);
}


/*----------------------------------------------------------------------------
 * Public interfaces
 * -------------------------------------------------------------------------*/
/**
 * <!-- CprInit -->
 *
 * @brief Initialize the CPR driver including the "CprLog" ULog.
 *
 * If CPR is disallowed by the efuse settings, the driver will continue to
 * correct clkrgm's default voltage with the offset values blown into the efuses.
 */
void CprInit( void )
{
  static boolean cprInitFnDone = FALSE;

  if (cprInitFnDone)
  {
    return;
  }
  cprInitFnDone = TRUE;

  CprInitUlog();

  /* Get the chip specific HAL, appropriate CPR target quotients and read efuses.*/
  CprGetTargetConfiguration();


  /* Log the settings used */
  ULOG_RT_STATIC_PRINTF_7(cprContext.ulogStaticHandle,
                          CPR_ULOG_STATIC_SLOT0_DRIVER_INFO,
                          "MSS CPR  (OpenLoopFloorEnabled: %d) (ClosedLoopEnabled: %d) (DisabledByHW: %d) (PmicVer: %d) (DriverVer: %d) (CPUVersion: %x) (QuotientVer: %s) ",
                          cprContext.enableFlags_OpenLoop_FloorVoltageEnabled, cprContext.enableFlags_ClosedLoopEnabled, cprContext.enableFlags_DisabledByFuse, cprContext.detectedPMIC,
                          CPR_DRIVER_VERSION, cprContext.detectedChipVer, cprContext.halTargetCfg->staticCfg.configVersionText);


  /* Calculate the cprContext.measurements[vmode].currentVoltage based on 
     fuse, target configuration and clock driver voltage information. 
     If the system is running closed-loop CPR these will be a starting points. 
   */
  CprInitStateData();

  /* Check Ceiling and Floor values of cprContext.targetCfg->vmodeCfg[] 
     entries against clock driver voltage ranges.*/
  CprPopulateVoltageLimitValues();



  if (!(cprContext.enableFlags_ClosedLoopEnabled) || (cprContext.enableFlags_DisabledByFuse))
  {
    /* Register the disabled mode functions with DalClock_RegisterCPRCallbacks */
    CprInitDisabledMode(&cprApi);
  }
  else
  {
    /* Setup the mutex, clocks, interrupts, register the enabled mode functions
       with DalClock_RegisterCPRCallbacks, and configure the CPR hardware registers*/
    CprInitEnabledMode(&cprApi);
  }


 /* Setup the sleep LPR when sleep resources are available */
  CprWaitForDependencies();
}


/**
 * <!-- CPR_Enable -->
 *
 * @brief Enabled Closed Loop CPR measurements to continue. 
 *
 * @param eRail : Which rail's CPR is being enabled.
 *
 * @return DAL_SUCCESS if the rail is enabled.
 */
DALResult CPR_Enable( VCSRailType eRail )
{
  
  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR_Enable");

  if (eRail != VCS_RAIL_MSS)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (cprApi.Enable)
  {
    return cprApi.Enable(eRail);
  }

  return DAL_ERROR_INTERNAL;
}


/**
 * <!-- CPR_Disable -->
 *
 * @brief Disable closed loop CPR measurements. 
 *
 * @param eRail : Which rail's CPR is being disabled.
 *
 * @return DAL_SUCCESS if the rail is disabled.
 */
DALResult CPR_Disable( VCSRailType eRail )
{
  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR_Disable");
  if (eRail != VCS_RAIL_MSS)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (cprApi.Disable)
  {
    return cprApi.Disable(eRail);
  }

  return DAL_ERROR_INTERNAL;
}


/**
 * <!-- CprGetFloorAndCeiling -->
 *
 * @brief Get the floor and ceiling voltage for the specified mode. 
 *
 * @param eRail : Which rail to provide information for.  
 * @param mode : enum for svs, svs+, nom, turbo etc. 
 * @param floorVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 * @param ceilingVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 *
 * @return DAL_SUCCESS if there are no issues filling the provided pointers. 
 */
DALResult CprGetFloorAndCeiling( VCSRailType rail_id, VCSCornerType mode, uint32* floorVoltageUV, uint32* ceilingVoltageUV ){
  DALResult floorResult = DAL_SUCCESS;
  DALResult ceilingResult = DAL_SUCCESS;
  static uint32 logged_CPR_calculation = 0;  //bit field to log the calculations only once. 

  if (rail_id != VCS_RAIL_MSS){
     return DAL_ERROR;
  }

  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CprGetFloorAndCeiling");

  //make sure the log is initialized
  CprInitUlog();


  //calculate the floor voltage
  if(floorVoltageUV!=NULL){
    floorResult = CprGetTargetFloorVoltageForMode((uint32)mode, floorVoltageUV);

    //log the calculation results only the first time it's done. 
    if(!(logged_CPR_calculation & (1 << (uint32)mode))){
      ULOG_RT_PRINTF_3(cprContext.ulogHandle, "CprGetTargetFloorVoltageForMode Vmode %u  Voltage %u Offset was %d", mode, *floorVoltageUV, cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[mode]->initVoltOffsetSteps);
      //set a bit to indicate this result has been logged
      logged_CPR_calculation |= (1 << (uint32)mode);
    }
  }

  //calculate the ceiling voltage  
  if(ceilingVoltageUV!=NULL){
    ceilingResult = CprGetTargetCeilingVoltageForMode((uint32)mode, ceilingVoltageUV);

    //log the calculation results only the first time it's done. 
    if(!(logged_CPR_calculation & (0x10000 << (uint32)mode))){
      ULOG_RT_PRINTF_3(cprContext.ulogHandle, "CprGetTargetCeilingVoltageForMode Vmode %u  Voltage%u  Offset was %d", mode, *ceilingVoltageUV, cprContext.targetCfg->vmodeBoundsAndOffsetCfgPtrs[mode]->initVoltOffsetSteps);
      //set a bit to indicate this result has been logged
      logged_CPR_calculation |= (0x10000 << (uint32)mode);
    }
  }


  if ((floorResult==DAL_SUCCESS) && (ceilingResult==DAL_SUCCESS))
  {
     return DAL_SUCCESS;
  } else {
     return DAL_ERROR;
  }
}


/**
 * <!-- CPR_GetRailVoltageRecommendation -->
 *
 * @brief Return the current CPR recomendation for the corner.
 *
 * @param eRail : Which rail to provide information for.  
 * @param eCorner : SVS, NOM, etc...
 * @param nVoltageUV : The recommended voltage is returned in this uint32*. 
 *
 * @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid. 
 */
DALResult CPR_GetRailVoltageRecommendation( VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageUV )
{
  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR_GetRailVoltageRecommendation");
  if (eRail != VCS_RAIL_MSS || eCorner >= VCS_CORNER_NUM_OF_CORNERS || nVoltageUV == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (cprApi.GetRailVoltageRecommendation)
  {
    return cprApi.GetRailVoltageRecommendation(eRail, eCorner, nVoltageUV);
  }

  return DAL_ERROR_INTERNAL;
}



/**
 * <!-- CPR_MeasureNow -->
 *
 * @brief Take a CPR measurement for the corner right now and return the recommendation.
 *
 * @param eRail : Which rail to measure.  
 * @param eCorner : SVS, NOM, etc...
 * @param nVoltageUV : The recommended voltage is returned in this uint32*. 
 *
 * @return DAL_SUCCESS if the measurement is taken successfully. 
 */
DALResult CPR_MeasureNow( VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageUV )
{
  //ULOG_RT_PRINTF_0(cprContext.ulogHandle, "CPR_MeasureNow");
  if (eRail != VCS_RAIL_MSS || eCorner >= VCS_CORNER_NUM_OF_CORNERS || nVoltageUV == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (cprApi.MeasureNow)
  {
    return cprApi.MeasureNow(eRail, eCorner, nVoltageUV);
  }

  return DAL_ERROR_INTERNAL;
}



/**
 * <!-- CPR_EnableSensor -->
 *
 * @brief  Enable a specific CPR sensor section. This function is only called while CPR is disabled.
 * 
 * CPUs or parts of the chip may be powered down of subregulated by an LDO.  This function 
 * communicates those changes to CPR so the CPR hardware can be reconfigured. 
 * 
 * @param nSensorCpuId : Value indicating which sensor to disable.
 *
 * @return DAL_SUCCESS if the enable is done and values passed all look good. 
 */ 
 DALResult CPR_EnableSensor( uint32 nSensorCpuId )
 {
   ULOG_RT_PRINTF_1(cprContext.ulogHandle, "CPR_EnableSensor %d", nSensorCpuId);

   //this could be enhanced, but currently there is only one sensor that gets enabled or disabled. 
   cprContext.cpuNotUsingMssRail = FALSE;
   HalCprBypassCpuBlock(cprContext.cpuNotUsingMssRail);
   return DAL_SUCCESS;
 }

 
/**
 * <!-- CPR_DisableSensor -->
 *
 * @brief  - Disable a specific CPR sensor section. This function is only called while CPR is disabled.
 * 
 * CPUs or parts of the chip may be powered down of subregulated by an LDO.  This function 
 * communicates those changes to CPR so the CPR hardware can be reconfigured. 
 * 
 * @param nSensorCpuId : Value indicating which sensor to disable.
 *
 * @return DAL_SUCCESS if the disable is done and values passed all look good. 
 */
 DALResult CPR_DisableSensor( uint32 nSensorCpuId )
 {
   ULOG_RT_PRINTF_1(cprContext.ulogHandle, "CPR_DisableSensor %d", nSensorCpuId);

   //this could be enhanced, but currently there is only one sensor that gets enabled or disabled. 
   cprContext.cpuNotUsingMssRail = TRUE;
   HalCprBypassCpuBlock(cprContext.cpuNotUsingMssRail);
   return DAL_SUCCESS;   
 }
 