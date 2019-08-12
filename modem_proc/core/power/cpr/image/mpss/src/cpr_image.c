/*===========================================================================

Copyright (c) 2014, 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.

===========================================================================*/

#include <stdlib.h>
#include "CoreVerify.h"
#include "CoreMutex.h"
#include "cpr_mpss.h"
#include "comdef.h"
#include "cpr_open_loop.h"
#include "cpr_closed_loop.h"
#include "VCSDefs.h" /* Enums for the VCS power modes */
#include "DALSys.h"
#include "DALSysTypes.h"
#include "DDIClock.h"
#include "DDIVCS.h"
#include "DDIInterruptController.h"
#include "ClockDefs.h"
#include <stdbool.h>
#include "cpr_voltage_ranges.h"
#include "cpr_target_quotients.h"
#include "cpr_enablement.h"
#include "cpr_image.h"
#include "cpr_logging.h"
#include "pm_version.h"
#include "DDIChipInfo.h"
#include <stringl/stringl.h>
#include "cpr.h"           //the MPSS public CPR API used by VCS
#include "npa.h"
#include "DDIPlatformInfo.h"
#if (defined(CPR2_8976) || defined(CPR2_8953) || defined(CPR2_8937))
#include "npa_resource.h"
#include "sleep_lpr.h"
#endif

#if (defined(CPR2_9x45) || defined(CPR2_8976) || defined(CPR2_8953) || defined(CPR2_8937))
#include "HALhwio.h"
#include "mss_rbcpr_access_hwio.h"
#endif


#ifdef CPR3_8996
#include "HALhwio.h"
#include "mss_rbcpr_access_hwio.h"
#include "HAL_cpr_v3.h"
#endif

//4/28/15 temporary fix for the new corner.
#ifndef VCS_CORNER_LOW_MIN      
#define VCS_CORNER_LOW_MIN  ((VCSCornerType) 24)
#endif


//enums for thermal state communication via npa
typedef enum {
    npa_thermal_region_normal = 0, 
    npa_thermal_region_cool = 1, 
    npa_thermal_region_cold = 2,
    npa_thermal_region_critical_cold = 3
} npa_thermal_region;

void cpr_post_mode_switch_reenable(cpr_closed_loop_rail_t rail, const cpr_corner_params_t* corner_params, uint32 rail_uv);
void cpr_pre_mode_switch_disable(cpr_closed_loop_rail_t rail, const cpr_corner_params_t* corner_params);
void cpr_handler(cpr_closed_loop_rail_t rail);
DALResult  CprGetTargetCXeLdoVoltageforMode(VCSCornerType vmode, uint32* voltageUV);
extern void cpr_init_closed_loop_init(void);
extern void cpr_init_initial_voltage_config(void);

/**
 * The enable/disable state needs a lock around it to handle ISRs coming in just after disable occurs
 */
CoreMutex * cpr_lockHandle;

/**
 * Holds the current recommended voltage for each mode
 */
uint32                  current_microvolts[CPR_VOLTAGE_MODE_COUNT];   //a big array until there's a cleaner solution.
uint32                  lowest_microvolts[CPR_VOLTAGE_MODE_COUNT];    //a big array until there's a cleaner solution.
uint8                   mode_enable_count[CPR_VOLTAGE_MODE_COUNT];    //how many times was CPR enabled in a given mode

/**
* VCS tells CPR what rail to prepare for when it asks when it queries CPR_GetRailVoltageRecommendation()
* We save the mode in this variable and use it the next time VCS calls CPR_Enable()
*/
cpr_voltage_mode_t      voltage_mode_to_enter_next = CPR_VOLTAGE_MODE_TURBO;

/**
* Records the current voltage mode.  This value is only changed in cpr_image_set_rail_mode()
*/
cpr_voltage_mode_t       current_voltage_level = CPR_VOLTAGE_MODE_TURBO;

/**
* While relinquished control is active, enable operations will be ignored. 
*/
boolean cpr_relinquished_control_active = false;

/**
* Handles for enabling/disabling the needed clock and changing the voltage.
*/
DalDeviceHandle          * cpr_clkHandle;
DalDeviceHandle          * cpr_vcsHandle;

/**
* For the modem CPR driver implementation we only use the mss rail.  This pointer is initialized at
* init time and used repeatedly in this file.  If the value of this pointer is NULL, closed
* loop CPR has not been enabled.
*/
cpr_closed_loop_rail_t mss_closed_loop_rail = NULL;

/**
 *   The first time we do a post switch CPR completes it's init with cpr_enable_closed_loop_feedback(cpr_closed_loop_rail_t rail) 
 */
boolean firstMSS_CPR_enable = true;

/**
* Used whenever we don't have real corner params to return .
* cpr_closed_loop.c asks for the current corner early on, and we don't have a good answer yet.
*/
cpr_corner_params_t cpr_initial_off_corner_params =
{
    .voltage_mode = CPR_VOLTAGE_MODE_OFF,
};

/**
* Do the initialization only once. 
*/
boolean cpr_initialized = false;

/**
 * Support aging if used on this target. 
 * This is the npa client handle to the Clk_proc vdd_mss resource.  It's used 
 * to ensure the corner is in Turbo mode before aging measurements are made.
 */ 
npa_client_handle hCPR_VDDMSS_ClkProcClient;   



/**
* This structure contains anything needed to control the rail's voltage.
* Only identifies the rail currently, but may be expanded to contain handles and other info.
*
*/
typedef struct cpr_image_rail_voltage_control_handle_s
{
    cpr_rail_id_t rail_id;
    
} cpr_image_rail_voltage_control_handle_s;



/**
* <!-- cpr_image_memscpy -->
*
* @brief Every image provides it's own version of the safe memcpy
*
*/
size_t cpr_image_memscpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
    return memscpy(dst, dst_size, src, src_size);
}



/**
* <!-- cpr_image_alloc_image_rail_state -->
*
* @brief Allocate and zero a cpr_image_closed_loop_rail_t type
*
*/
cpr_image_closed_loop_rail_t cpr_image_alloc_image_rail_state(void)
{
    cpr_image_closed_loop_rail_t image_rail_state = (cpr_image_closed_loop_rail_t)malloc(sizeof(cpr_image_closed_loop_rail_s));
    CORE_VERIFY_PTR(image_rail_state);
    memset(image_rail_state, 0, sizeof(cpr_image_closed_loop_rail_s));
    return image_rail_state;
}



/**
* <!-- cpr_image_alloc_voltage_control_handle -->
*
* @brief Create a voltage control handle for the requested rail id. 
*
*/
cpr_image_rail_voltage_control_handle_t cpr_image_alloc_voltage_control_handle(cpr_rail_id_t cpr_rail_id)
{
    cpr_image_rail_voltage_control_handle_t control_handle = (cpr_image_rail_voltage_control_handle_t)malloc(sizeof(cpr_image_rail_voltage_control_handle_s));
    CORE_VERIFY_PTR(control_handle);
    memset(control_handle, 0, sizeof(cpr_image_rail_voltage_control_handle_s));

    control_handle->rail_id = cpr_rail_id;

    return control_handle;
}



/**
* <!-- cpr_image_set_rail_mode_voltage -->
*
* @brief Records the voltage to be used in the voltage mode. 
*        physical voltage isn't changed here, only in ISR handling
*
*/
void cpr_image_set_rail_mode_voltage(cpr_image_rail_voltage_control_handle_t voltage_control_handle, cpr_voltage_mode_t voltage_mode, uint32 voltage_uv)
{
    CORE_VERIFY_PTR(voltage_control_handle);
    if (voltage_control_handle->rail_id != CPR_RAIL_MSS){
        return;
    }
    //CPR_LOGGING(2, "cpr_image_set_rail_mode_voltage - voltage mode: %s    Software will now recommend voltage: %d", cpr_logging_get_ascii_corner_name(voltage_mode), voltage_uv);
    current_microvolts[voltage_mode] = voltage_uv;
    //physical voltage on rail is not set in this function.  The ISR on the modem is the only function changing the physical voltage. 
}



/**
* <!-- cpr_image_get_rail_mode_voltage -->
*
* @brief Returns the current recommended rail voltage for a voltage mode.  
*
* This is only called for closed loop rails in files like cpr_closed_loop.c and hal_cpr_closed_loop.c
*
* @return The rail voltage in uV
*/
uint32 cpr_image_get_rail_mode_voltage(cpr_image_rail_voltage_control_handle_t voltage_control_handle, cpr_voltage_mode_t voltage_mode)
{
    CORE_VERIFY_PTR(voltage_control_handle);

    if (voltage_control_handle->rail_id != CPR_RAIL_MSS){
      return 0;
    }

    if (voltage_mode >= CPR_VOLTAGE_MODE_COUNT){
        return 0;
    }


    //CPR_LOGGING(2, "cpr_image_get_rail_mode_voltage voltage mode: %s  returning voltage: %d", cpr_logging_get_ascii_corner_name(voltage_mode), current_microvolts[voltage_mode]);
    return  current_microvolts[voltage_mode]; 
}



/**
* <!-- cpr_image_get_current_corner_params -->
*
* @brief Returns the current corner params for the rail
*
*/
const cpr_corner_params_t* cpr_image_get_current_corner_params(cpr_closed_loop_rail_t rail)
{
    if (rail->rail_config->rail_id == CPR_RAIL_MSS){
        if (mss_closed_loop_rail == NULL){
            return &cpr_initial_off_corner_params;     //this should only be at init currently.
        } else {
            return mss_closed_loop_rail->current_target_params;
        }
    }
  
    //for all other unused rails currently return an OFF placeholder.
    return &cpr_initial_off_corner_params;
}



/**
* <!-- cpr_image_rail_transition_voltage -->
*
* @brief Actually change the physical voltage on the rail. 
*
*/
void cpr_image_rail_transition_voltage(cpr_image_rail_voltage_control_handle_t voltage_control_handle)
{
    uint32 newVoltage;
    cpr_voltage_mode_t currentCorner;
    DALResult retval;

    if (voltage_control_handle->rail_id != CPR_RAIL_MSS){
        return;
    }

    currentCorner = cpr_image_get_rail_mode(mss_closed_loop_rail->voltage_control_handle);
    newVoltage = cpr_image_get_rail_mode_voltage(mss_closed_loop_rail->voltage_control_handle, currentCorner);

    CPR_LOGGING(1, "cpr_image_rail_transition_voltage %d", newVoltage);

    //update the lowest voltage if this is a new low
    if (lowest_microvolts[currentCorner] > newVoltage)
    {
        lowest_microvolts[currentCorner] = newVoltage;
        cpr_logging_update_lowest_voltage_stats(lowest_microvolts[CPR_VOLTAGE_MODE_SVS_MIN],
                                                lowest_microvolts[CPR_VOLTAGE_MODE_SVS2],
                                                lowest_microvolts[CPR_VOLTAGE_MODE_SVS],
                                                lowest_microvolts[CPR_VOLTAGE_MODE_SVS_L1],
                                                lowest_microvolts[CPR_VOLTAGE_MODE_NOMINAL],
                                                lowest_microvolts[CPR_VOLTAGE_MODE_NOMINAL_L1],
                                                lowest_microvolts[CPR_VOLTAGE_MODE_TURBO]);
    }
    
   
    //Change the physical voltage on the rail 
    retval = DalVCS_SetRailVoltage(cpr_vcsHandle, VCS_RAIL_MSS, newVoltage);

    cpr_logging_update_current_voltage_stats(current_microvolts[CPR_VOLTAGE_MODE_SVS_MIN],
                                             current_microvolts[CPR_VOLTAGE_MODE_SVS2],
                                             current_microvolts[CPR_VOLTAGE_MODE_SVS],
                                             current_microvolts[CPR_VOLTAGE_MODE_SVS_L1],
                                             current_microvolts[CPR_VOLTAGE_MODE_NOMINAL],
                                             current_microvolts[CPR_VOLTAGE_MODE_NOMINAL_L1],
                                             current_microvolts[CPR_VOLTAGE_MODE_TURBO]);

   
    //make sure the interrupts are correct (we might be at the floor or ceiling)
    cpr_prepare_cpr(mss_closed_loop_rail, cpr_corner_params(mss_closed_loop_rail, current_voltage_level), newVoltage);
}



/**
* <!-- cpr_image_set_rail_mode -->
*
* @brief set the rail's current voltage mode to the specified mode. 
*
*/
void cpr_image_set_rail_mode(cpr_image_rail_voltage_control_handle_t voltage_control_handle, cpr_voltage_mode_t voltage_mode)
{
    //only supporting MSS currently
    if (voltage_control_handle->rail_id != CPR_RAIL_MSS){
        return;
    }

    if (mss_closed_loop_rail != NULL){
        //set the driver's current mode to this new mode.
        current_voltage_level = voltage_mode;

        CPR_LOGGING(1, "cpr_image_set_rail_mode to voltage mode: %s  ", cpr_logging_get_ascii_corner_name(voltage_mode));
        mss_closed_loop_rail->current_target_params = cpr_corner_params(mss_closed_loop_rail, voltage_mode);
    }
}



/**
* <!-- cpr_image_get_rail_mode -->
*
* @brief Answers, what mode is this rail in currently?
*
*/
cpr_voltage_mode_t cpr_image_get_rail_mode(cpr_image_rail_voltage_control_handle_t voltage_control_handle)
{
  return current_voltage_level;
}

/**
* <!-- cpr_image_enable_clocks -->
*
* @brief Turn the CPR clocks on.
*/
void cpr_image_enable_clocks(cpr_closed_loop_rail_t rail)
{
    ClockIdType clockId;
  
    //CPR_LOGGING(1, "cpr_image_enable_clocks    rail: %s", cpr_logging_get_ascii_rail_id_name(rail->rail_id));	

    // Attach to Clock Driver
    CORE_DAL_VERIFY( DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &cpr_clkHandle));

    // Enable the rbcpr bus clock.
    DalClock_GetClockId(cpr_clkHandle, "clk_bus_rbcpr", &clockId);
    DalClock_EnableClock(cpr_clkHandle, clockId);

    DalClock_GetClockId(cpr_clkHandle, "clk_rbcpr_ref", &clockId);
    DalClock_EnableClock(cpr_clkHandle, clockId);

    // Take the clock out of reset state.
    DalClock_ResetClock(cpr_clkHandle, clockId, CLOCK_RESET_DEASSERT);

}



/**
* <!-- cpr_corner_params -->
*
* @brief Look up the corner parameters for the given mode.
*
*/
cpr_corner_params_t* cpr_corner_params(cpr_closed_loop_rail_t rail, cpr_voltage_mode_t level)
{
    if (rail->rail_config->rail_id == CPR_RAIL_MSS){
        for (int i = 0; i < mss_closed_loop_rail->target_params_count; i++){
            if (mss_closed_loop_rail->target_params[i].voltage_mode == level)
            {
                return &(mss_closed_loop_rail->target_params[i]);
            }
        }
    }
    
    if (rail->rail_config->rail_id == CPR_RAIL_CX){
        return &cpr_initial_off_corner_params;
    }
  
    CPR_LOGGING(0, "Tried to look up nonexistent corner!");
    CORE_VERIFY(0);  

    return NULL;
}



/**
* <!-- convertVcsCornerToCprVoltageLevelEnum -->
*
* @brief The MSS CPR driver talks to VCS using VCSCorner types.  This function 
* coverts those types to the corner enums used by the CPR driver. 
*/
cpr_voltage_mode_t convertVcsCornerToCprVoltageLevelEnum(VCSCornerType eCorner){
    if (eCorner == ((VCS_CORNER_LOW_MIN)))
       return CPR_VOLTAGE_MODE_SVS_MIN;
       
    if (eCorner == VCS_CORNER_LOW_MINUS)
       return CPR_VOLTAGE_MODE_SVS2;

    if (eCorner == VCS_CORNER_LOW)
       return CPR_VOLTAGE_MODE_SVS;

    if (eCorner == VCS_CORNER_LOW_PLUS)
       return CPR_VOLTAGE_MODE_SVS_L1;

    if (eCorner == VCS_CORNER_NOMINAL)
       return CPR_VOLTAGE_MODE_NOMINAL;

    if (eCorner == VCS_CORNER_NOMINAL_PLUS)
       return CPR_VOLTAGE_MODE_NOMINAL_L1;

    if (eCorner == VCS_CORNER_TURBO)
       return CPR_VOLTAGE_MODE_TURBO;

    CORE_VERIFY(0);  //an unknown corner is being requested! 
    return CPR_VOLTAGE_MODE_SUPER_TURBO_NO_CPR;

}



/**
* <!-- cpr_image_get_current_voltageuV -->
*
* @brief Queries for the voltage before switching into a new corner.  Return the current
* voltage and record the next corner to enter. 
*/
uint32 cpr_image_get_current_voltageuV(VCSCornerType eCorner){
    const cpr_config_versioned_voltage_ranges_t* mss_voltage_ranges;
    const cpr_config_voltage_level_t* mss_voltage_level;
    cpr_voltage_mode_t level = convertVcsCornerToCprVoltageLevelEnum(eCorner);

    //CPR_LOGGING(2, "cpr_image_get_current_voltageuV (mode: %s)   (voltage: %d)", cpr_logging_get_ascii_corner_name(level), current_microvolts[level]);

    //if the initial voltage hasn't been set, calculate it as the open loop value. 
    if (current_microvolts[level] == 0){
        mss_voltage_ranges = cpr_config_find_versioned_voltage_ranges(CPR_RAIL_MSS);

        mss_voltage_level = cpr_config_find_voltage_level(mss_voltage_ranges, level);
        if (mss_voltage_level != NULL){
            current_microvolts[level] = cpr_config_open_loop_voltage(CPR_RAIL_MSS, level);
        } else {
            CPR_LOGGING(1, "cpr_image_get_current_voltageuV for unknown voltage level (mode: %s)", cpr_logging_get_ascii_corner_name(level));
        }
    }

    return current_microvolts[level];
}



/**
* <!-- cpr_image_pre_switch_callback -->
*
* @brief Turns off CPR before something significant (like a corner change) is done. 
*/
void cpr_image_pre_switch_callback()
{
    if (mss_closed_loop_rail!=NULL){  //only need to do work if closed loop is enabled. 
        Core_MutexLock(cpr_lockHandle);
        //we don't know what corner we're entering yet, but we need to disable CPR so we'll be calling this now and calling it again before enable.
        const cpr_corner_params_t* corner_params = cpr_corner_params(mss_closed_loop_rail, voltage_mode_to_enter_next);

        cpr_pre_mode_switch_disable(mss_closed_loop_rail, corner_params);

        Core_MutexUnlock(cpr_lockHandle);

        if (voltage_mode_to_enter_next!=current_voltage_level){
            //CPR_LOGGING(1, "cpr_pre_switch_callback ready to switch to : %s if needed ", cpr_logging_get_ascii_corner_name(voltage_mode_to_enter_next));
        } else {
            //CPR_LOGGING(0, "cpr_pre_switch_callback");
        }
    }
}



/**
* <!-- cpr_image_post_switch_callback -->
*
* @brief Turns on CPR measurements. 
*/
void cpr_image_post_switch_callback()
{
    cpr_corner_params_t* corner_params;

    //CPR_LOGGING(0, "cpr_image_post_switch_callback");
    cpr_closed_loop_rail_t rail = mss_closed_loop_rail;

    Core_MutexLock(cpr_lockHandle);

    //- Change corners NOW based on the last query, the mode to enter was saved in "voltage_mode_to_enter_next" -
    cpr_image_set_rail_mode(mss_closed_loop_rail->voltage_control_handle, voltage_mode_to_enter_next);

    mode_enable_count[voltage_mode_to_enter_next]++;
    cpr_logging_update_mode_enable_count_stats(mode_enable_count[CPR_VOLTAGE_MODE_SVS_MIN],
                                               mode_enable_count[CPR_VOLTAGE_MODE_SVS2],
                                               mode_enable_count[CPR_VOLTAGE_MODE_SVS],
                                               mode_enable_count[CPR_VOLTAGE_MODE_SVS_L1],
                                               mode_enable_count[CPR_VOLTAGE_MODE_NOMINAL],
                                               mode_enable_count[CPR_VOLTAGE_MODE_NOMINAL_L1],
                                               mode_enable_count[CPR_VOLTAGE_MODE_TURBO]);


    //the first time we do a post switch CPR completes it's init with cpr_enable_closed_loop_feedback(cpr_closed_loop_rail_t rail) 
    if (firstMSS_CPR_enable==true){
        firstMSS_CPR_enable = false;
        CPR_LOGGING(0, "cpr being enabled for the first time, finish HW initialization");
        cpr_enable_closed_loop_feedback(mss_closed_loop_rail);
    }
   
    corner_params = cpr_corner_params(rail, voltage_mode_to_enter_next);
    cpr_pre_mode_switch_disable(rail, corner_params);

    cpr_post_mode_switch_reenable(rail, corner_params, current_microvolts[voltage_mode_to_enter_next]);
    Core_MutexUnlock(cpr_lockHandle);
}



/**
* <!-- cpr_isr -->
*
* @brief CPR has a recommendation!
*        Should only be called when closed loop is enabled.  
*
*/
void cprIsr( uint32 unused )
{
    cpr_voltage_mode_t currentCorner;
    uint32  newVoltage;

    CPR_LOGGING(0, "- ISR -");
    if (mss_closed_loop_rail != NULL){
        Core_MutexLock(cpr_lockHandle);
        cpr_handler(mss_closed_loop_rail);

        currentCorner = cpr_image_get_rail_mode(mss_closed_loop_rail->voltage_control_handle);
        newVoltage = cpr_image_get_rail_mode_voltage(mss_closed_loop_rail->voltage_control_handle, currentCorner);
        Core_MutexUnlock(cpr_lockHandle);
        CPR_LOGGING(2, "- ISR End - Current Mode: %s   Current Voltage: %d", cpr_logging_get_ascii_corner_name(currentCorner), newVoltage);
    }
}


#if (defined(CPR2_8976) || defined(CPR2_8953) || defined(CPR2_8937))
/**
 * <!-- Cpr_image_init_low_power_mgr -->
 * 
 * @brief Register an LPRM with Sleep so that we can disable CPR during 
 * power collapse. 
 */
static void Cpr_image_init_low_power_mgr( void *context,
                                          unsigned int event_type,
                                          void *data,
                                          unsigned int data_size ) 
{
  npa_client_handle npaClientHandle;
  
  CPR_LOGGING(0, "/sleep/lpr resource is available, registering CPR LPR with sleep");

  CORE_VERIFY_PTR(npaClientHandle =
                  npa_create_sync_client(SLEEP_LPR_NODE_NAME,
                                         "mss_rbcpr",
                                         NPA_CLIENT_REQUIRED));

  sleep_define_lpr_str("rbcpr", npaClientHandle);

  npa_issue_required_request(npaClientHandle, SLEEP_ALL_LPRMS);
}

#endif

/**
* <!-- rbcpr_init -->
*
* @brief Initialize CPR.  
*
*/
void rbcpr_init(void)
{
    npa_query_type            queryResult;
    const cpr_config_versioned_voltage_ranges_t* mss_voltage_ranges;
    const cpr_config_voltage_level_t* mss_voltage_level; 
    DalDeviceHandle *interruptHandle = NULL;
    uint32 effective_floorUV[CPR_VOLTAGE_MODE_COUNT];   //used to log the floor values
    cpr_closed_loop_rail_t node;
        
    if (cpr_initialized == true){
        return;
    }
    cpr_initialized = true;

    cpr_logging_init();

    CPR_LOGGING(0, "rbcpr_init");

    cpr_lockHandle = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT);
    //CPR_LOGGING(1, "cpr_lock: %x", cpr_lockHandle);

#ifdef CPR3_8996
    CPR_LOGGING(0, "MSS_BUS_RBCPR_CFG_CBCR setting CLKEN bit!");

    HWIO_OUT(MSS_BUS_RBCPR_CBCR, 1);
    HWIO_OUT(MSS_RBCPR_REF_CBCR, 1);

    CPR_LOGGING(1, "CPR Version is %x", HWIO_IN(MSS_CPR_VERSION));
#endif
    
#if (defined(CPR2_8976) || defined(CPR2_8953) || defined(CPR2_8937))
    HWIO_MSS_CLAMP_IO_OUTM (HWIO_MSS_CLAMP_IO_RBCPR_BMSK, 0x0);
    CPR_LOGGING(0, "MSS RBCPR clamp cleared");
#endif
    
    
    cpr_init_initial_voltage_config();

    //initialize the array that will be used to log the floor values in the static log.
    for (int i=0; i < CPR_VOLTAGE_MODE_COUNT; i++){
        effective_floorUV[i] = 0;
    }
    
    // Find the mss rail open loop voltages for all the corners we might use for MSS
    // and set the current voltages to the open loop voltages. 
    mss_voltage_ranges = cpr_config_find_versioned_voltage_ranges(CPR_RAIL_MSS);
    
    mss_voltage_level = cpr_config_find_voltage_level(mss_voltage_ranges, CPR_VOLTAGE_MODE_SVS_MIN);
    if (mss_voltage_level!=NULL){
       current_microvolts[CPR_VOLTAGE_MODE_SVS_MIN] = cpr_config_open_loop_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_SVS_MIN);
       lowest_microvolts[CPR_VOLTAGE_MODE_SVS_MIN] = current_microvolts[CPR_VOLTAGE_MODE_SVS_MIN];
       effective_floorUV[CPR_VOLTAGE_MODE_SVS_MIN] = cpr_config_closed_loop_floor_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_SVS_MIN);
    }
    
    mss_voltage_level = cpr_config_find_voltage_level(mss_voltage_ranges, CPR_VOLTAGE_MODE_SVS2);
    if (mss_voltage_level!=NULL){
       current_microvolts[CPR_VOLTAGE_MODE_SVS2] = cpr_config_open_loop_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_SVS2);
       lowest_microvolts[CPR_VOLTAGE_MODE_SVS2] = current_microvolts[CPR_VOLTAGE_MODE_SVS2];
       effective_floorUV[CPR_VOLTAGE_MODE_SVS2] = cpr_config_closed_loop_floor_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_SVS2);
    }

    mss_voltage_level = cpr_config_find_voltage_level(mss_voltage_ranges, CPR_VOLTAGE_MODE_SVS);
    if (mss_voltage_level!=NULL){
       current_microvolts[CPR_VOLTAGE_MODE_SVS] = cpr_config_open_loop_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_SVS);
       lowest_microvolts[CPR_VOLTAGE_MODE_SVS] = current_microvolts[CPR_VOLTAGE_MODE_SVS];
       effective_floorUV[CPR_VOLTAGE_MODE_SVS] = cpr_config_closed_loop_floor_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_SVS);
    }

    mss_voltage_level = cpr_config_find_voltage_level(mss_voltage_ranges, CPR_VOLTAGE_MODE_SVS_L1);
    if (mss_voltage_level!=NULL){
       current_microvolts[CPR_VOLTAGE_MODE_SVS_L1] = cpr_config_open_loop_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_SVS_L1);
       lowest_microvolts[CPR_VOLTAGE_MODE_SVS_L1] = current_microvolts[CPR_VOLTAGE_MODE_SVS_L1];
       effective_floorUV[CPR_VOLTAGE_MODE_SVS_L1] = cpr_config_closed_loop_floor_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_SVS_L1);
    }
    
    mss_voltage_level = cpr_config_find_voltage_level(mss_voltage_ranges, CPR_VOLTAGE_MODE_NOMINAL);
    if (mss_voltage_level!=NULL){
       current_microvolts[CPR_VOLTAGE_MODE_NOMINAL] = cpr_config_open_loop_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_NOMINAL);
       lowest_microvolts[CPR_VOLTAGE_MODE_NOMINAL] = current_microvolts[CPR_VOLTAGE_MODE_NOMINAL];
       effective_floorUV[CPR_VOLTAGE_MODE_NOMINAL] = cpr_config_closed_loop_floor_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_NOMINAL);
    }

    mss_voltage_level = cpr_config_find_voltage_level(mss_voltage_ranges, CPR_VOLTAGE_MODE_NOMINAL_L1);
    if (mss_voltage_level!=NULL){
       current_microvolts[CPR_VOLTAGE_MODE_NOMINAL_L1] = cpr_config_open_loop_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_NOMINAL_L1);
       lowest_microvolts[CPR_VOLTAGE_MODE_NOMINAL_L1] = current_microvolts[CPR_VOLTAGE_MODE_NOMINAL_L1];
       effective_floorUV[CPR_VOLTAGE_MODE_NOMINAL_L1] = cpr_config_closed_loop_floor_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_NOMINAL_L1);
    }

    mss_voltage_level = cpr_config_find_voltage_level(mss_voltage_ranges, CPR_VOLTAGE_MODE_TURBO);
    if (mss_voltage_level!=NULL){
       current_microvolts[CPR_VOLTAGE_MODE_TURBO] = cpr_config_open_loop_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_TURBO);
       lowest_microvolts[CPR_VOLTAGE_MODE_TURBO] = current_microvolts[CPR_VOLTAGE_MODE_TURBO];
       effective_floorUV[CPR_VOLTAGE_MODE_TURBO] = cpr_config_closed_loop_floor_voltage(CPR_RAIL_MSS, CPR_VOLTAGE_MODE_TURBO);
    }

    cpr_logging_update_starting_voltage_stats(current_microvolts[CPR_VOLTAGE_MODE_SVS_MIN],
                                              current_microvolts[CPR_VOLTAGE_MODE_SVS2],
                                              current_microvolts[CPR_VOLTAGE_MODE_SVS],
                                              current_microvolts[CPR_VOLTAGE_MODE_SVS_L1],
                                              current_microvolts[CPR_VOLTAGE_MODE_NOMINAL],
                                              current_microvolts[CPR_VOLTAGE_MODE_NOMINAL_L1],
                                              current_microvolts[CPR_VOLTAGE_MODE_TURBO]);

    cpr_logging_update_lowest_voltage_stats(lowest_microvolts[CPR_VOLTAGE_MODE_SVS_MIN],
                                            lowest_microvolts[CPR_VOLTAGE_MODE_SVS2],
                                            lowest_microvolts[CPR_VOLTAGE_MODE_SVS],
                                            lowest_microvolts[CPR_VOLTAGE_MODE_SVS_L1],
                                            lowest_microvolts[CPR_VOLTAGE_MODE_NOMINAL],
                                            lowest_microvolts[CPR_VOLTAGE_MODE_NOMINAL_L1],
                                            lowest_microvolts[CPR_VOLTAGE_MODE_TURBO]);

    cpr_logging_update_floor_voltage_stats(effective_floorUV[CPR_VOLTAGE_MODE_SVS_MIN],
                                           effective_floorUV[CPR_VOLTAGE_MODE_SVS2],
                                           effective_floorUV[CPR_VOLTAGE_MODE_SVS],
                                           effective_floorUV[CPR_VOLTAGE_MODE_SVS_L1],
                                           effective_floorUV[CPR_VOLTAGE_MODE_NOMINAL],
                                           effective_floorUV[CPR_VOLTAGE_MODE_NOMINAL_L1],
                                           effective_floorUV[CPR_VOLTAGE_MODE_TURBO]);

    cpr_logging_update_current_voltage_stats(current_microvolts[CPR_VOLTAGE_MODE_SVS_MIN],
                                             current_microvolts[CPR_VOLTAGE_MODE_SVS2],
                                             current_microvolts[CPR_VOLTAGE_MODE_SVS],
                                             current_microvolts[CPR_VOLTAGE_MODE_SVS_L1],
                                             current_microvolts[CPR_VOLTAGE_MODE_NOMINAL],
                                             current_microvolts[CPR_VOLTAGE_MODE_NOMINAL_L1],
                                             current_microvolts[CPR_VOLTAGE_MODE_TURBO]);

    //log the fuse values
    cpr_logging_update_fuse_stats(CPR_RAIL_MSS);

    /* Create vdd_mss client handle. */
    hCPR_VDDMSS_ClkProcClient = npa_create_sync_client("/vdd/mss","CPR_client", NPA_CLIENT_REQUIRED);

  /* Make sure we're in turbo mode if aging measurements will be taken. */
  
    //vote for Turbo to make sure the system is in Turbo
    npa_issue_required_request(hCPR_VDDMSS_ClkProcClient, VCS_CORNER_TURBO);
  
#ifdef CPR2_9x45
    //need to unclamp before doing the aging measurements
    HWIO_OUTF(MSS_CLAMP_MEM, UNCLAMP_ALL, 0x1);
    HWIO_OUTF(MSS_CLAMP_IO,  UNCLAMP_ALL, 0x1);
#endif
  
    CPR_LOGGING(1, "CPR do aging /vdd/mss requests handle = %x", hCPR_VDDMSS_ClkProcClient);
  
   if ( (DalPlatformInfo_Platform() != DALPLATFORMINFO_TYPE_RUMI) &&
	    (DalPlatformInfo_Platform() != DALPLATFORMINFO_TYPE_VIRTIO) )
   {
     //query the resource's active state to make sure we're in Turbo
     if(NPA_QUERY_SUCCESS == npa_query_by_client(hCPR_VDDMSS_ClkProcClient, NPA_QUERY_CURRENT_STATE, &queryResult))
     {
       CPR_LOGGING(1,"vdd_mss state is %s",  cpr_logging_get_ascii_corner_name(convertVcsCornerToCprVoltageLevelEnum(queryResult.data.state)));
     } else {
     CPR_LOGGING(0,"Query fail");
     }	
   }

    cpr_init_closed_loop_init();

#ifdef CPR2_9x45
    HWIO_OUTF(MSS_CLAMP_MEM, UNCLAMP_ALL, 0x0);
    HWIO_OUTF(MSS_CLAMP_IO,  UNCLAMP_ALL, 0x0);
#endif


    //Remove the TURBO vote.
    npa_complete_request(hCPR_VDDMSS_ClkProcClient);

    //find the MSS CPR closed loop rail if closed loop cpr has been enabled for this rail.
    node = cpr_closed_loop_rail_root;

    while (node)
    {
        if (node->rail_config->rail_id == CPR_RAIL_MSS)  //found it, save the mss rail pointer. 
        {
            mss_closed_loop_rail = node;
            break;
        }
        node = node->next_link;
    }

    //Set our current mode to Turbo if closed loop is enabled
    if (mss_closed_loop_rail){
        //set CPR structs to a valid initial state, this will be corrected as soon as VCS calls into us with their query
        current_voltage_level = CPR_VOLTAGE_MODE_TURBO;
        mss_closed_loop_rail->current_target_params = cpr_corner_params(mss_closed_loop_rail, CPR_VOLTAGE_MODE_TURBO);

        //CPR_LOGGING(0, "rbcpr_init closed-loop is enabled");
        cpr_logging_initialize_stats_log( mss_closed_loop_rail,
                                          "(OpenLoopFloorEnabled: 1) (ClosedLoopEnabled: 1)", 
                                          /*disabled by hw*/0,
                                          /*pmic_version*/ 0,
                                          cpr_image_get_chip_info_version(), 
                                          cpr_config_which_device_fuses_used(&cpr_bsp_fuse_config));

        mss_closed_loop_rail->block_enabled=true;

        // Attach to VCS for voltage changes.
        CORE_DAL_VERIFY( DAL_StringDeviceAttach("VCS", &cpr_vcsHandle));
  

        // Get an interrupt controller handle
        CORE_DAL_VERIFY(DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &interruptHandle));

        // Register the RBCPR interrupt.
        CORE_DAL_VERIFY(DalInterruptController_RegisterISR(interruptHandle, 
                                                           mss_closed_loop_rail->rail_config->interrupt_id,
                                                           (DALIRQ)cprIsr, 
                                                           (DALISRCtx)0, 
                                                           DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER));
    } else {
        //CPR_LOGGING(0, "rbcpr_init running open-loop only");
        cpr_logging_initialize_stats_log( NULL /*mss_closed_loop_rail*/,
                                          "(OpenLoopFloorEnabled: 1) (ClosedLoopEnabled: 0)", 
                                          /*disabled by hw*/0,
                                          /*pmic_version*/ 0,
                                          cpr_image_get_chip_info_version(),
                                          cpr_config_which_device_fuses_used(&cpr_bsp_fuse_config));
    }
    
    

#if 0
    // Test the GetTargetCXeLdoVoltage queries
    {
        uint32 voltageUV;
		if (CprGetTargetCXeLdoVoltageforMode(VCS_CORNER_LOW_MIN, &voltageUV) == DAL_SUCCESS){
            CPR_LOGGING(1, "Test CX eLDO SVS_MIN value: %d", voltageUV);
        }
        if (CprGetTargetCXeLdoVoltageforMode(VCS_CORNER_LOW_MINUS, &voltageUV) == DAL_SUCCESS){
            CPR_LOGGING(1, "Test CX eLDO SVS2 value: %d", voltageUV);
        }
        if (CprGetTargetCXeLdoVoltageforMode(VCS_CORNER_LOW, &voltageUV) == DAL_SUCCESS){
            CPR_LOGGING(1, "Test CX eLDO SVS value: %d", voltageUV);
        }
    }
#endif

  npa_resource_available_cb(SLEEP_LPR_NODE_NAME,
                            Cpr_image_init_low_power_mgr, NULL );

}


void cpr_image_cancel_task(cpr_closed_loop_rail_t rail)
{
}

void cpr_image_prepare_for_voltage_settling(void)
{
}


void rbcpr_core_dump(void)
{
}

void cpr_image_clear_interrupt_at_interrupt_controller(uint32 interrupt_id)
{
}

void cpr_image_prepare_to_access_smem(void)
{
 
}

void cpr_image_smem_access_complete(void)
{
    
}



/**
* <!-- handle_thermal_event -->
*
* @brief Adjusts margin based on thermal events
*
*/
static void handle_thermal_event(npa_thermal_region newState)
{
    static npa_thermal_region oldState = npa_thermal_region_cool;
    cpr_closed_loop_rail_t rail = cpr_closed_loop_rail_root;
    boolean toggle_cpr_to_engage_new_margin = false;
    
    if(oldState == newState) {
        return;
    }
    
    CPR_LOGGING(2, "Processing thermal region change %u -> %u", oldState, newState );
    
    Core_MutexLock(cpr_lockHandle);

    while(rail)
    {
        if (rail->block_enabled == TRUE)
        {
            //pause CPR if it was previously running
            toggle_cpr_to_engage_new_margin = true;
            cpr_pause_cpr(rail);
        }

        //recalculate all the quotients for all the corners. 
        for(int i=0; i < rail->target_params_count; i++)     
        {
            cpr_corner_params_t* corner_target_params = &rail->target_params[i];
            cpr_target_quotient_set_t* ro_targets = &corner_target_params->rosc_targets;
            
            if(oldState == npa_thermal_region_normal && newState > npa_thermal_region_normal) {
                //if the part has cooled off add the margin back  (subtracting a negative number here)
                ro_targets->dynamic_margin_mv -= rail->rail_config->thermal_margin;                
            }
            else if(oldState > npa_thermal_region_normal && newState == npa_thermal_region_normal) {
                //if the part has heated up the themal margin gets subtracted. (adding a negative number here) 
                ro_targets->dynamic_margin_mv += rail->rail_config->thermal_margin;
            }
            
            cpr_target_quotient_calculate_quotients(rail->rail_id, 
                                                    corner_target_params->voltage_mode, 
                                                    ro_targets->static_margin_mv + ro_targets->dynamic_margin_mv,
                                                    ro_targets);
        }
        
        if (toggle_cpr_to_engage_new_margin == true)
        {
            //resume CPR if it was previously running
            uint32 rail_uv = cpr_image_get_rail_mode_voltage(rail->voltage_control_handle, rail->current_target_params->voltage_mode);
            cpr_configure_roscs(rail, rail->current_target_params);
            cpr_prepare_cpr(rail, rail->current_target_params, rail_uv);
            cpr_restart_cpr(rail);
        }
       
        rail = rail->next_link;
    }
    
    oldState = newState;
    Core_MutexUnlock(cpr_lockHandle);
}



/**
* <!-- therm_change_event_cb -->
*
* @brief Listener for thermal events
*
*/
static void therm_change_event_cb(void *context,
                                  unsigned int event_type,
                                  void *data,
                                  unsigned int data_size)
{
    if(data != NULL) {
        handle_thermal_event( (npa_thermal_region) ((npa_event_data*) data)->state );
    }
}



/**
* <!-- therm_resource_available_cb -->
*
* @brief Listener for thermal event availability
*
*/
static void therm_resource_available_cb(void *context,
                                        unsigned int event_type,
                                        void *data,
                                        unsigned int data_size)
{
    npa_event_handle eventClient;
    npa_query_type query_result;
    
    CPR_LOGGING(0, "Registering for therm change cb" );
    
    eventClient = npa_create_change_event_cb( "/therm/mitigate/cpr_cold",
                                              "CPR Thermal Notification",
                                              therm_change_event_cb,
                                              NULL );

    if(npa_query_by_event( eventClient, NPA_QUERY_CURRENT_STATE, &query_result ) == NPA_QUERY_SUCCESS) {
        handle_thermal_event( query_result.data.state );
    }    
}



/**
* <!-- cpr_image_init_image_closed_loop -->
*
*
*/
void cpr_image_init_image_closed_loop(cpr_closed_loop_rail_t rail)
{
    static boolean init = true;
    
    if(init) 
    {
        CPR_LOGGING(0, "Registering for therm available cb" );
        npa_resource_available_cb( "/therm/mitigate/cpr_cold", therm_resource_available_cb, NULL );
        init = false;
    }
}



/**
* <!-- cpr_image_get_chip_info_version -->
*
* @brief Calls DalChipInfo_ChipVersion on the modem version of the CPR driver. 
*
*/
uint32 cpr_image_get_chip_info_version(void)
{
    DalChipInfoVersionType chipVersion;
    CORE_VERIFY(chipVersion = DalChipInfo_ChipVersion());
    return chipVersion;
}



/**
* <!-- cpr_image_get_foundry_info -->
*
* @brief Get the foundry and return a foundry enum that our CPR driver understands. 
*
*/
cpr_foundry_id cpr_image_get_foundry_info(void)
{
    cpr_foundry_id chipInfo_foundry = 0;
    DalChipInfoFamilyType familyVersion;

    chipInfo_foundry = (cpr_foundry_id)DalChipInfo_FoundryId();
    CORE_VERIFY(familyVersion = DalChipInfo_ChipFamily());
    CORE_VERIFY(chipInfo_foundry);
    
    return chipInfo_foundry;
}




/**
* <!-- cpr_image_get_pmic_step_size -->
*
* @brief Get the pmic step size for a given rail.
*
* @param rail_id : Which CPR supported rail to lookup
*
* @return The step size voltage in uV
*/

uint32 cpr_image_get_pmic_step_size(cpr_rail_id_t rail_id){
    int i;

    for (i = 0; i < cpr_bsp_misc_info_config.rail_info_count; i++)
    {
        if (cpr_bsp_misc_info_config.rail_info[i].rail_id == rail_id)
        {
            return   cpr_bsp_misc_info_config.rail_info[i].pmic_step_size;
        }
    }

    //requested the PMIC step size of an unsupported rail
    CORE_VERIFY_PTR(0);
    return 0;
}



/**
* <!-- cpr_image_get_safe_voltage -->
*
* @brief Return a safe voltage for the mode
*
* @param mode : SVS, NOM, etc...
*
* @return The safe voltage in uV
*/
uint32 cpr_image_get_safe_voltage(cpr_voltage_mode_t mode){
    uint32 lowest_voltage_plus_two_pmic_stepsUV;
    uint32 ol_voltageUV;
    uint32 new_voltageUV;

    //set the mode's voltage to the lowest voltage plus two PMIC steps or the open loop voltage depending
    //on which is lower.  This is a safe voltage. 
    ol_voltageUV = cpr_config_open_loop_voltage(CPR_RAIL_MSS, mode);
    lowest_voltage_plus_two_pmic_stepsUV = lowest_microvolts[mode] + (cpr_image_get_pmic_step_size(CPR_RAIL_MSS) * 2);

    if (ol_voltageUV <= lowest_voltage_plus_two_pmic_stepsUV){
        new_voltageUV = ol_voltageUV;
    }
    else
    {
        new_voltageUV = lowest_voltage_plus_two_pmic_stepsUV;
    }

    return new_voltageUV;
}



/**
* <!-- CprGetTargetCXeLdoVoltageforMode -->
*
* @brief Calculate the CX eLDO voltage for a corner
*
* @param vmode : VCS enum for the corner (SVS, NOM etc)
* @param voltageUV : pointer to uint32 where the eLDO voltage will be returned.
*
* @return DAL_SUCCESS is a value is returned, DAL_ERROR if the corner is not supported for eLDO queries
*/
DALResult  CprGetTargetCXeLdoVoltageforMode(VCSCornerType vmode, uint32* voltageUV)
{
    //only SVS, SVS2, and SVS_MIN are supported currently.
    if ((vmode != VCS_CORNER_LOW_MINUS) && (vmode != VCS_CORNER_LOW) && (vmode != ((VCS_CORNER_LOW_MIN))) ){
        return DAL_ERROR;
    }
  
    //this function may be called early in the boot so we make sure the log is created and ready
    cpr_logging_init();
  
    //CPR_LOGGING(0, "--CprGetTargetCXeLdoVoltageforMode --  ");

#ifdef CPR2_9x45
    //12/9/14 eLDO voltage calculations
    //SVS = CPR open loop for vdd_cx @ SVS – 20mV
    //SVS2 = CPR open loop for vdd_cx @ SVS2 


    if (vmode== ((VCS_CORNER_LOW_MIN)) ){
       return DAL_ERROR;
    }


    if (vmode==VCS_CORNER_LOW_MINUS){  /*SVS2*/
        int32 fuseValue = cpr_fuses_get_fuse_value(CPR_RAIL_CX, CPR_FUSE_SVS2);
        if (fuseValue < 0) {
            //Dec 12 2014 SVS2 formula updated to remove the 20mV subtraction that is still part of the SVS calculation. 
            *voltageUV = 800000 - (-fuseValue*10000);  //fuse values like -4 are typical. 
        } else {
            //clip positive fuse values to 0
            *voltageUV = 800000;  //fuse values like -4 are typical. 
        }
        CPR_LOGGING(2, "Calculated SVS2 ELDO fuse: %d voltage %d", fuseValue, *voltageUV);
    }


    if (vmode==VCS_CORNER_LOW){
        int32 fuseValue = cpr_fuses_get_fuse_value(CPR_RAIL_CX, CPR_FUSE_SVS);
        if (fuseValue < 0) {
            *voltageUV = 900000 - (-fuseValue * 10000) - 20000;  //fuse values like -4 are typical. 
        } else {
            //clip positive fuse values to 0
            *voltageUV = 900000 - 20000;
        }
        CPR_LOGGING(2, "Calculated SVS ELDO fuse: %d voltage %d", fuseValue, *voltageUV);
    }
#endif

#ifdef CPR3_8996
    //6/8/15 eLDO voltage calculations. The proposed SW Vref_set value is
    //CPR_SVS – 60mV. for minSVS
    //20mV below CPR_SVS (chip) for SVS2
    //90mV below CPR_Nom (chip) for SVS.


    if (vmode== ((VCS_CORNER_LOW_MIN)) ){ /*minSVS*/
        int32 fuseValue = cpr_fuses_get_fuse_value(CPR_RAIL_CX, CPR_FUSE_SVS);
        *voltageUV = cpr_config_open_loop_voltage(CPR_RAIL_CX, CPR_VOLTAGE_MODE_SVS) - 60000;
        CPR_LOGGING(2, "Calculated SVS_MIN ELDO fuse: %d voltage %d", fuseValue, *voltageUV);
    }


    if (vmode==VCS_CORNER_LOW_MINUS){    /*SVS2*/
        int32 fuseValue = cpr_fuses_get_fuse_value(CPR_RAIL_CX, CPR_FUSE_SVS);
        *voltageUV = cpr_config_open_loop_voltage(CPR_RAIL_CX, CPR_VOLTAGE_MODE_SVS) - 20000;
        CPR_LOGGING(2, "Calculated SVS2 ELDO fuse: %d voltage %d", fuseValue, *voltageUV);
    }


    if (vmode==VCS_CORNER_LOW){          /*SVS*/
        int32 fuseValue = cpr_fuses_get_fuse_value(CPR_RAIL_CX, CPR_FUSE_NOMINAL);
        *voltageUV = cpr_config_open_loop_voltage(CPR_RAIL_CX, CPR_VOLTAGE_MODE_NOMINAL);  - 90000;
        CPR_LOGGING(2, "Calculated SVS ELDO fuse: %d voltage %d", fuseValue, *voltageUV);
    }
#endif


    return DAL_SUCCESS;
}



/* -----------------------------------------------------------------
 *  Public CPR interface defined in api\cpr.h and used by VCS
 *------------------------------------------------------------------*/

/**
 * <!-- CprGetFloorAndCeiling -->
 *
 * @brief Get the floor and ceiling voltage for the specified mode. This is usually the first MSS CPR 
 * function called.  VCS calls it to understand what CPR's limits for the supported rails are. 
 *
 * @param eRail : Which rail to provide information for.  
 * @param mode : enum for svs, svs+, nom, turbo etc. 
 * @param floorVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 * @param ceilingVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 *
 * @return DAL_SUCCESS if there are no issues filling the provided pointers. 
 */
DALResult CprGetFloorAndCeiling( VCSRailType rail_id, VCSCornerType mode, uint32* floorVoltageUV, uint32* ceilingVoltageUV ){
  
    //this function may be called early in the boot so we make sure the log is created and ready
    cpr_logging_init();

    //CPR_LOGGING(0, "CprGetFloorAndCeiling");  

    if (rail_id != VCS_RAIL_MSS){
        return DAL_ERROR;
    }

    //calculate the floor voltage
    *floorVoltageUV = cpr_config_closed_loop_floor_voltage(CPR_RAIL_MSS, convertVcsCornerToCprVoltageLevelEnum(mode));
 
  
    //calculate the ceiling voltage  
    *ceilingVoltageUV = cpr_config_open_loop_voltage(CPR_RAIL_MSS, convertVcsCornerToCprVoltageLevelEnum(mode));

    CPR_LOGGING(3, "Calculate floor and ceiling for %s   Floor: %d   Ceiling: %d ", cpr_logging_get_ascii_corner_name(convertVcsCornerToCprVoltageLevelEnum(mode)), *floorVoltageUV, *ceilingVoltageUV);
  
    return DAL_SUCCESS;
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
    if (eRail != VCS_RAIL_MSS)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    if (cpr_relinquished_control_active == true){
        CPR_LOGGING(0, "--CPR Enable--  Relinquished Control is active, nothing to do");
        return DAL_SUCCESS;
    }
  
    if (mss_closed_loop_rail!=NULL){  //only need to do work if closed loop is enabled. 
        CPR_LOGGING(0, "--CPR Enable--");
        cpr_image_post_switch_callback();
    } else {
        CPR_LOGGING(0, "--CPR Enable--  Running open loop, nothing to do");
    }
    return DAL_SUCCESS;
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
    if (eRail != VCS_RAIL_MSS)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }
  
    if (mss_closed_loop_rail!=NULL){  //only need to do work if closed loop is enabled. 
        CPR_LOGGING(0, "--CPR Disable--");
        cpr_image_pre_switch_callback();
    }else{
        CPR_LOGGING(0, "--CPR Disable--  Running open-loop nothing to do");
    }

    return DAL_SUCCESS;
}



/**
 * <!-- CPR_GetRailVoltageRecommendation -->
 *
 * @brief  What voltage does CPR expect used for this corner. 
 *         If closed loop CPR is enabled this voltage MUST be the voltage used for the driver to work correctly. 
 * 
 * 
 * @param eRail : Only VCS_RAIL_MSS is currently supported. 
 * @param eCorner : VCS_CORNER_LOW_MINUS, VCS_CORNER_LOW, etc...
 * @param nVoltageUV : Pointer to the uint32 to fill in the voltage info. 
 *
 * @return DAL_SUCCESS if the query is successful.
 */  
DALResult CPR_GetRailVoltageRecommendation( VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageUV )
{
    //CPR_LOGGING(0, "CPR_GetRailVoltageRecommendation");
    if (eRail != VCS_RAIL_MSS || eCorner >= VCS_CORNER_NUM_OF_CORNERS || nVoltageUV == NULL)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    Core_MutexLock(cpr_lockHandle);
    //this sets the expectation that this is the corner we will switch to the next time post switch is called.   
    voltage_mode_to_enter_next = convertVcsCornerToCprVoltageLevelEnum(eCorner);

    *nVoltageUV = cpr_image_get_current_voltageuV(eCorner);
    Core_MutexUnlock(cpr_lockHandle);

    CPR_LOGGING(2, "CPR_GetRailVoltageRecommendation for %s   (Voltage : %d)", cpr_logging_get_ascii_corner_name(convertVcsCornerToCprVoltageLevelEnum(eCorner)), *nVoltageUV);
  
    return DAL_SUCCESS;
}



/**
* <!-- CPR_GetEldoVoltageRecommendation -->
*
* @brief Return the CPR eLDO voltage for the corner.
*
* @param eRail : Which rail to provide information for.
* @param eCorner : VCS_CORNER_LOW_MINUS, VCS_CORNER_LOW
* @param nVoltageUV : The recommended voltage is returned in this uint32*.
*
* @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
*/
DALResult CPR_GetEldoVoltageRecommendation(VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageUV)
{
    //only supporting CX currently. 
    if (eRail != VCS_RAIL_CX || eCorner >= VCS_CORNER_NUM_OF_CORNERS || nVoltageUV == NULL)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    return CprGetTargetCXeLdoVoltageforMode(eCorner, nVoltageUV);
}



/**
* <!-- CPR_Init_Phase1 -->
*
* @brief  Called by rcinit via  currently.  Initializes CPR clocks, ISRs and other setup requirements.
*
* @param eRail : Which rail to initialize
*
* @return DAL_SUCCESS if the rail is valid.
*/
DALResult CPR_Init_Phase1(VCSRailType  eRail){
    if (eRail != VCS_RAIL_MSS)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }
    rbcpr_init();
  
    return DAL_SUCCESS;
}



/**
* <!-- CPR_Init_Phase2 -->
*
* @brief  Indication to CPR that the system is in it's lowest power state and can do bootsafe voltage training.
*
* @param eRail : Which rail to initialize
*
* @return DAL_SUCCESS if the rail is valid.
*/
DALResult CPR_Init_Phase2(VCSRailType  eRail){
    if (eRail != VCS_RAIL_MSS)
    {
        return DAL_ERROR_INVALID_PARAMETER;
    }

    CPR_LOGGING(0, "CPR_Init_Phase2");

    //Do bootsafe voltage calculations now. 

    return DAL_SUCCESS;
}



/**
* <!-- CPR_GetSafeVoltage -->
*
* @brief Get the bootsafe measured voltage for the corner
*
* @param eRail : Which rail to provide information for.
* @param eCorner : VCS_CORNER_LOW_MINUS, VCS_CORNER_LOW
* @param nVoltageUV : The recommended voltage is returned in this uint32*.
*
* @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
*/
DALResult  CPR_GetSafeVoltage(VCSRailType  eRail, VCSCornerType eCorner, uint32 *nSafeVoltageuV)
{
    if (eRail != VCS_RAIL_MSS || nSafeVoltageuV == NULL)
    {
        //return DAL_ERROR;
        return DAL_ERROR_INVALID_PARAMETER;
    }

    *nSafeVoltageuV = cpr_image_get_safe_voltage(convertVcsCornerToCprVoltageLevelEnum(eCorner));

    return DAL_SUCCESS;
}



/**
* <!-- CPR_RelinquishedControl_Enter -->
*
* @brief CPR will not be operating for an extended period of time.
*
* All CPR closed loop recommendations will be reset to higher safe values and
* enable/disable calls will do nothing until CPR_RelinquishedControl_Exit is called.
*
* @param eRail : Which rail to be switched.
*
* @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
*/
DALResult CPR_RelinquishedControl_Enter(VCSRailType  eRail)
{
    if (eRail != VCS_RAIL_MSS)
    {
        return DAL_ERROR_INVALID_PARAMETER;
        //return DAL_ERROR;
    }

    Core_MutexLock(cpr_lockHandle);

    cpr_relinquished_control_active = true;

    CPR_LOGGING(0, "CPR_RelinquishedControl_Enter: Disable CPR and set bootsafe voltages");

    //make sure CPR is disabled. 
    CPR_Disable(VCS_RAIL_MSS);


    if (mss_closed_loop_rail!=NULL){  //the next steps are only needed if closed loop CPR is being used. 
        for (int i = 0; i < cpr_enablement_number_of_rails(); i++)
        {
            cpr_rail_id_t rail_id;
            const cpr_enablement_versioned_rail_config_t* rail_enablement_config = NULL;
            cpr_enablement_rail_info(i, &rail_id, &rail_enablement_config);

            if (rail_id == CPR_RAIL_MSS){
                if (rail_enablement_config != NULL){   //KW Fix make sure our rail enable structure is present before accessing it. 
                    //loop through all the supported corners and make sure they're at safe levels. 
                    for (int j = 0; j < rail_enablement_config->supported_level_count; j++){
                        uint32 new_voltageUV;
                        cpr_voltage_mode_t voltage_mode;
                        voltage_mode = rail_enablement_config->supported_level[j].voltage_mode;
                        new_voltageUV = cpr_image_get_safe_voltage(voltage_mode);
                        cpr_image_set_rail_mode_voltage(mss_closed_loop_rail->voltage_control_handle, voltage_mode, new_voltageUV);
                        //CPR_LOGGING(2, "CPR_RelinquishedControl_Enter setting %s to safe voltage : %d", cpr_logging_get_ascii_corner_name(voltage_mode), new_voltageUV);
                    }
                }
            }
        }
    }
    Core_MutexUnlock(cpr_lockHandle);

    return DAL_SUCCESS;
}



/**
* <!-- CPR_RelinquishedControl_Exit -->
*
* @brief CPR will be used again after an extended period of time where it was unused.
*
* All CPR closed loop recommendations will be reset to higher safe values and
* enable/disable calls will do nothing until CPR_RelinquishedControl_Exit is called.
*
* @param eRail : Which rail to be switched.
*
* @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
*/
DALResult CPR_RelinquishedControl_Exit(VCSRailType  eRail)
{
    if (eRail != VCS_RAIL_MSS)
    {
        //return DAL_ERROR;
        return DAL_ERROR_INVALID_PARAMETER;
    }

    //nothing to do currently,  the voltage resets were done when Relinquished Control was started. 
    CPR_LOGGING(0, "CPR_RelinquishedControl_Exit");
    Core_MutexLock(cpr_lockHandle);
    cpr_relinquished_control_active = false;
    Core_MutexUnlock(cpr_lockHandle);
    return DAL_SUCCESS;
}


/**
 * <!-- CprGoToLowPowerMode -->
 *
 * @brief Enable/disable CPR HW when entering/exiting low power modes.
 * 
 * @param inLowPowerMode: TRUE if entering low power mode; FALSE otherwise.
 */
void CprGoToLowPowerMode( boolean inLowPowerMode )
{
  ClockIdType clockId;

  CPR_LOGGING(1, "CprGoToLowPowerMode()-- inLowPowerMode : %d", inLowPowerMode);
  
  if (!inLowPowerMode)
  {
    if (cpr_clkHandle != NULL) {
	  // turn on the clocks after waking up
      DalClock_GetClockId(cpr_clkHandle, "clk_bus_rbcpr", &clockId);
      DalClock_EnableClock(cpr_clkHandle, clockId);

      DalClock_GetClockId(cpr_clkHandle, "clk_rbcpr_ref", &clockId);
      DalClock_EnableClock(cpr_clkHandle, clockId);
    }
    rbcpr_exit_sleep();
  }
  
  //disable or re-enable CPR
  //CprEnable(!inLowPowerMode);
   
  if (inLowPowerMode)
  {
    rbcpr_prepare_for_sleep();
    if (cpr_clkHandle != NULL)
    {  
      // turn off the clocks before going to sleep
      DalClock_GetClockId(cpr_clkHandle, "clk_bus_rbcpr", &clockId);
      DalClock_DisableClock(cpr_clkHandle, clockId);
	  
      DalClock_GetClockId(cpr_clkHandle, "clk_rbcpr_ref", &clockId);
      DalClock_DisableClock(cpr_clkHandle, clockId);
    }
	
  }
}

/**
 * <!-- CprSetLowPowerMode -->
 *
 * @brief Invoke low power mode function based on CPR enablement status.
 * 
 * @param enteringLowPowerMode: TRUE if entering low power mode; FALSE otherwise.
 */
void CprSetLowPowerMode(unsigned long int enteringLowPowerMode)
{
  CPR_LOGGING(1, "CprSetLowPowerMode()-- inLowPowerMode : %d", enteringLowPowerMode);
  if (cpr_bsp_enablement_config.rail_enablement_config[0]->versioned_rail_config[0]->enablement_init_params != &CPR_ENABLE_CLOSED_LOOP )
  {
    CPR_LOGGING(0, "CprSetLowPowerMode() closed loop CPR not enabled");
    return;
  }
  CprGoToLowPowerMode (enteringLowPowerMode);
}

/**
 * <!-- CprLprmDisableEnter -->
 * 
 * @brief Entering power collapse mode. Disable the CPR HW.
 * 
 * @param wakeupTick : Unused
 */
void CprLprmDisableEnter(uint64_t wakeupTick)
{
  CprSetLowPowerMode(TRUE);
  HWIO_MSS_CLAMP_IO_OUTM(HWIO_MSS_CLAMP_IO_RBCPR_BMSK, 0x1);
}


/**
 * <!-- CprLprmDisableExit -->
 * 
 * @brief Leaving power collapse mode. Re-enable the CPR HW.
 */
void CprLprmDisableExit( void )
{
  HWIO_MSS_CLAMP_IO_OUTM(HWIO_MSS_CLAMP_IO_RBCPR_BMSK, 0x0);
  CprSetLowPowerMode(FALSE);
}
