#ifndef SLEEP_LPR_H
#define SLEEP_LPR_H
/*==============================================================================
  FILE:         sleep_lpr.h
  
  OVERVIEW:     This file provides the public externs and definitions that
                are exported by the sleep module for accessing the sleep LPR
                node and functions.

  DEPENDENCIES: None

                Copyright (c) 2010-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/api/power/sleep/sleep_lpr.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "DALStdDef.h"
#include "npa.h"
#include "sleep_stats.h"

/* 
===============================================================================

                             MACRO DEFINITIONS

===============================================================================
*/

/**
 * @brief SLEEP_LPR_SUCCESS
 *
 * Function completed successfully. 
 */
#define SLEEP_LPR_SUCCESS 0

/**
 * @brief SLEEP_LPR_FAILED
 *
 * Function failed. 
 */
#define SLEEP_LPR_FAILED 1

/**
 * @brief SLEEP_LPR_NODE_NAME
 *
 * Name of the sleep LPR node. 
 */
#define SLEEP_LPR_NODE_NAME "/sleep/lpr"

/* Used to create the bitmask passed in requests to /sleep/lpr. */
/**
 * @brief SLEEP_LPRM_NUM
 *
 * Used to create the bitmask that corresponds to mode enable/disable
 * request to /sleep/lpr
 */
#define SLEEP_LPRM_NUM(x) (1 << (x))

/**
 * @brief SLEEP_ALL_LPRMS
 *
 * Bitmask representing all modes (basically all 1's)
 */
#define SLEEP_ALL_LPRMS (NPA_MAX_STATE)


/* Macros for power function related fields in LPRMs (defined below) */

/*
 * Macros for latency functions in LPRMs (defined below). User can have
 * totally different macros for enter latency and exit latency.
 */

/**
 * @brief SLEEP_LAT_FUNC_SCALE
 *
 * Use this macro to fill latency fields if it is defined by multiple values
 * used for frequency scaling
 * 
 * @param latList: Pointer to latency list.
 */
#define SLEEP_LAT_FUNC_SCALE(latList) {LAT_FUNC_CONST_SCALE, \
                                       {(uint32 *)latList}}

/**
 * @brief SLEEP_LAT_FUNC_CUSTOM
 *
 * Use this macro to fill latency fields if it is defined by a custom
 * function.
 *
 * @param coeffs: Coefficients (of type sleep_coeffs) for the latency function 
 *                (UNUSED) 
 * @param lat_func: Pointer to latency function.
 */
#define SLEEP_LAT_FUNC_CUSTOM(coeffs, latFunc) {LAT_FUNC_CUSTOM, \
                                                {(uint32 *)latFunc}}

/* Macros for Backoff fields of the LPRM */

/**
 * @brief SLEEP_BACKOFF_NONE
 *
 * Use this macro to indicate that backoff function is not provided.
 */
#define SLEEP_BACKOFF_NONE  {LAT_FUNC_NONE, (uint32 *)0}

/**
 * @brief SLEEP_BACKOFF_POLY
 *
 * Indicates that LPRM's backoff time has multiple scaled 
 * entries 
 *
 * @param latList: Pointer to latency list.
 */
#define SLEEP_BACKOFF_FUNC_SCALE(latList) SLEEP_LAT_FUNC_SCALE(latList)

/**
 * @brief SLEEP_BACKOFF_CUSTOM
 *
 * Indicates custom function for back off time of the mode. 
 *
 * @param coeffs: Coefficients (of type sleep_coeffs) for the
 *                backoff function
 * @param pow_func: Pointer to the back off function.
 */
#define SLEEP_BACKOFF_CUSTOM(coeffs, back_off_fcn) \
                                     SLEEP_LAT_FUNC_CUSTOM(coeffs, back_off_fcn)

/**
 * SLEEP_NUM_CORES
 *
 * @brief Number of cores we expect sleep to run on (in one master). It is 
 *        expected that if master has more than one cores, this macro is
 *        defined at build time indicating number of cores on that master.
 */
#ifndef SLEEP_NUM_CORES 
#define SLEEP_NUM_CORES 1
#endif

/* Utility macros to fill the core fields of various sleep structures */

/**
 * SLEEP_ALL_CORES
 *
 * @brief Applicable to all cores on the processor.
 *
 * Sometimes 0 is used to indicate that field or value is applicable to 
 * all cores but that is just for backwards compatibility. Users are
 * encouraged to use this macros now.
 */
#define SLEEP_ALL_CORES ((1 << SLEEP_NUM_CORES)-1)

/**
 * SLEEP_CORE
 *
 * @brief To indicate particular core on a processor.
 *
 * Core number starts with 0. It does not check if input argument is 
 * greater than available cores on processor. If something is related
 * to more than one cores but not all, bitwise OR can be used with
 * this macro.
 *
 * @param num: Core number (starting with 0).
 */
#define SLEEP_CORE(num) (1 << num)

/**
 * SLEEP_LAST_CORE
 *
 * @brief Indicates the index of the last core.
 *
 * Core numbers are in range of 0 to n-1 in case of n core processor
 * where n-1 is the last core.
 */
#define SLEEP_LAST_CORE (SLEEP_NUM_CORES - 1)

/*
===============================================================================

                        GENERAL SLEEP DESCRIPTION

===============================================================================
*/

/* Sleep LPR management node
 *
 * The sleep LPR node is responsible for allowing registration of low power
 * resources (LPR's) each of which have one or more low power resource mode
 * (LPRM).  LPR's are resources critical to the compute path that will not be
 * needed when the processor is idle as turning them off provides a desirable
 * power savings (depending on the current dynamic conditions).
 *  
 * There are basically two tiers at which resources are filtered for whether or
 * not they enter a low power mode. The first is the explicit LPRM tracking:
 * NPA nodes for the affected resources will make requests into the sleep LPR
 * node.  Requests into the sleep LPR node are 32-bit bitmasks, where each set
 * bit is an enable signal for a given LPRM.  The mapping between bits and
 * LPRM's can be queried from the LPR node (see below for more detail).  Unless
 * an LPRM is requested by at least one client of the sleep LPR node, they are
 * considered explicity disabled and will not be considered for entry at idle
 * time.
 *  
 * The second and more complex tier is the sleep "solver."  The solver's task
 * is to look at all the enabled LPRM's and select the low power modes which
 * result in optimal power consumption based on the current expected sleep
 * period, latency restrictions, and temperature.  The solver functions
 * independently of the sleep LPR node, other than a simple query to figure out
 * which LPRM's are currently enabled.
 *
 * Making a request into this node (and thereby indicating to the LPRM tracking
 * system that a mode is able to be entered when the CPU has become idle) does
 * not mean that the resource necessarily *will* be put to sleep.  The actual
 * decision to select a sleep mode for entry is not made until idle time, when
 * the sleep solver will select modes based on what configurations would
 * provide the optimal power savings given the conditions that exist (expected
 * sleep length, latency budget, etc.). 
 *
 * However, when nobody has made a request into this node with a given bit set,
 * the LPRM associated with that bit is considered disabled.  This means that
 * regardless of whatever conditions may exist, the LPM solver cannot select a
 * low power resource mode until someone has made a request to enable it.
 *
 *                == Summary of supported operations ==
 *
 * Query this node with query id SLEEP_LPR_QUERY_BITMASK:
 *    - Takes an LPRM name string as an input in the 'reference' field.
 *    - LPRM name string should be immutable.
 *    - Returns the bitmask that should be requested into the sleep LPR field
 *      when the LPRM should be enabled as an output in the value field.
 *
 * Query this node with query id SLEEP_LPR_QUERY_RESOURCES:
 *    - Returns a pointer to the internal array of registered LPR's.  The array
 *      is returned in the reference field, but should only be accessed while
 *      in an INTLOCK'd context, as the array is raw internal node state.  This
 *      query should be exclusively used by the sleep solver to enumerate
 *      modes.
 *
 * Query this node with query id NPA_QUERY_CURRENT_STATE:
 *    - Returns a bitwise OR of all currently requested bitmasks.  This is
 *      basically the mask of LPRM's that are considered for selection when the
 *      sleep solver runs.
 *
 * Make a required request into this node:
 *    - For each set bit in the request bitmask, enables the associated LPRM
 *      for selection when the sleep solver runs.
 *
 * Complete a request into this node:
 *    - Remove the previously enabled LPRM's from consideration when the sleep
 *      solver runs.
 */

/* 
===============================================================================

                           GLOBAL TYPE DEFINITIONS

=============================================================================== 
*/

/**
 * @brief Power/latency coefficients structure.
 *
 * The structure below stores the coefficients of power function
 * if they depend on duration or temparature. The num_coeffs will store the 
 * order of the function (+1) and coeffs points to an array of coefficients. 
 * Coefficient index 0 will have power of 0 for the variable, the one at index
 * 1 will have power of 1 and so on. So for example, to represent 
 * 2X^4 - 3X^2 + 4, set the num_coeffs to 5 and coeffs to an array {4,0,-3,0,2}
 */
typedef struct
{
  unsigned int num_coeffs;
  int *coeffs;
}sleep_coeffs;

/**
 * @brief The prototype for a function that returns the latency involved in
 *        recovering from the low power mode when an interrupt is received.
 *
 * Each low power resource mode is required to define a function of this type.
 * Its purpose is to give the solver a concept of how much latency is involved
 * with both entering and leaving a mode.  This is important because the whole
 * purpose of the solver is to allocate the latency budget, selecting the modes
 * that optimize power savings while not violating the latency requirements of
 * clients.
 *  
 * @param coeffs: Coefficients for the latency equation. In case of polynomial
 *                coeffs[0] + coeffs[1]*x_value + coeffs[2]*x_value^2 + ... +
 *                coeffs[n]*x_value^n.
 * @param x_value: Variable part for the latency equation.
 *
 * @return time required in ticks to enter or exit a low power mode.
 */
typedef uint32 (*sleep_latency_func_type1)(sleep_coeffs *coeffs,
                                           uint64 x_value);

/**
 * @brief The prototype for a function called when the low power mode should be
 *        set up to happen at sleep time.
 *  
 * Each low power mode is required to define a function of this type.  Its
 * purpose it to isolate the code required to enter a low power mode in an
 * easy-to-call way.  This allows us to call or not call the code based on
 * whether this particular mode was selected. 
 *  
 * Note that this function is NOT allowed to fail.  You should place all checks
 * for whether the power mode can be entered outside of the actual sleep mode
 * selection via LPM mask bits.
 *
 * @param wakeup_tick: Absolute wake-up tick.
 */
typedef void (*sleep_enter_func)(uint64 wakeup_tick);

/**
 * @brief The prototype for a function called when the low power mode should be
 *        torn down.
 *  
 * Each low power mode is required to define a function of this type.  Its
 * purpose it to isolate the code required to clean up after a low power mode
 * in an easy-to-call way.  This allows us to call the exit functions for modes
 * that we entered during sleep. 
 *  
 * It is safe to assume that exit functions will only be called if the
 * associated enter function was called first. 
 */
typedef void (*sleep_exit_func)(void);

/**
 * @brief The prototype for the function that gets called when a low power
 *        resource is initialized from Sleep framework. 
 *
 * Each low power resource (LPR) must be initialized before it is used. Sleep
 * framework can do so if pointer to an init function (that adheres following
 * prototype) is provided in LPR definition. If the init function needs to 
 * be called outside sleep framework, a NULL pointer can be used for that
 * field in the resource definition. In either case, it MUST be ensured that
 * all necessary initialization has been performed before any of resource's 
 * low power mode is entered for the first time.
 */
typedef void (*sleep_lpr_init_func_type)(void);

/**
 * @brief Enum for the latency (enter, exit & back off) functions.
 *
 * Based on this tags, enter, exit and backoff function pointers will be cast.
 */
typedef enum latency_func_tag
{
  LAT_FUNC_NONE = 0x0,            // Indicates absence of latency
  LAT_FUNC_CONST_SCALE,           // Constant latency per frequency
  LAT_FUNC_CUSTOM,                // tag for user's power function
}sleep_latency_tag;

/**
 * @brief Sleep lock type to ensure that only one of enter and exit functions
 *        of Low Power Resource Mode (LPRM) is called at a time in case of
 *        multicore environment.
 *
 * This lock is intended to be used only when an LPRM is shared on more than
 * one core.
 */
typedef void * sleep_mode_fn_lock;

/**
 * @brief Enum that will map various LPRM options (like reference count and
 *        use of locks) to that mode's attribute bitmask.
 */
typedef enum
{
   /* Mode does not need reference count mechanism to identify last core
    * to go down. This attribute should be used when in single core environment
    * or LPR owner wants identify last core to go down himself. If LPR owner
    * uses this attribute for non local mode, he is responsible for managing
    * mode frequency and time spent in that mode as well. */
   SLEEP_MODE_ATTR_NO_REF_COUNT = 1,
   
   /* Mode does not want locking to prevent overlapping of enter and exit
    * function. It will be the case when mode trivial exit function or it
    * is not for multicore setup. */
   SLEEP_MODE_ATTR_NO_FCN_LOCK = 1 << 1,
   
   /* Mode does not want to perform optimization while calling its latency
    * and power functions. This should only be used when for same input
    * parameters (duration, temperature, frequency), repetitive call to
    * the function returns different values.
    */
   SLEEP_MODE_ATTR_NO_FCN_OPTIMIZATION = 1 << 2,
   
    /* Mode will return its own backoff latency enter & exit times
     * Typically used for power collapse mode where automatic monitoring
     * would not work
     */ 
   SLEEP_MODE_ATTR_NO_AUTO_LATENCY_TRACK = 1 << 3,

   /* Sets bit field for all the attributes defined above and enforces enum
    * of 32bit size */
   SLEEP_MODE_ALL_ATTRS = 0x7FFFFFFF,
}sleep_mode_attribute;
   
/* Forward declaration of low power resource (LPR) and modes (LPRM) structs */
typedef struct sleep_lpr        sleep_lpr;
typedef struct sleep_lprm       sleep_lprm;
typedef struct sleep_lat_data   sleep_lat_data;

/* Helper structure to store latency values */
struct sleep_lat_data
{
  sleep_latency_tag tag;
  union
  {
    uint32  *fList;     /* constant values corresponding to frequency table */ 
    void    *function;  /* custom function */
  }latency;
};

/* struct to store low power mode information */ 
struct sleep_lprm
{
  /* 
   * The name of this low power mode, independent of the resoruce name.  For
   * example, for apps processor core voltage retention-only, mode this would
   * be "retention" as the resource name is already embedded in the owning
   * resource structure. Names must be valid C-language identifiers.
   */
  const char *mode_name;
  
  /* 
   * Functions that describe the mode's characteristics. 
   * We want to have different types of power and latency functions. The type of
   * functions that will be called are based on p_tag and l_[enter/exit]_tag.
   * In case user decides to use polynomial function for any of them,
   * the corresponding sleep_coeffs cannot be NULL.
   *
   * NOTE: The recommended way to fill coefficients, tag and function type is
   *       to use the macros provided above (see MACRO DEFINITIONS section) in
   *       this file.
   */

  /* Attributes for entering latency function */
  sleep_lat_data    enter_lat;

  /* Attributes for exiting latency function */
  sleep_lat_data    exit_lat;

  /*
   * Functions that implement the actual entry/setup and exit/teardown of a 
   * mode. Either of them must be provided. 
   */
  sleep_enter_func  enter_func;
  sleep_exit_func   exit_func;

  /* Attributes for mode back off function */
  sleep_lat_data    backoff_lat;

  /* Attributes for the mode */
  uint32            attributes;

  /***************************************************************************
   * NOTE: Initialize to zero (or don't initialize) from the field below. They
   *       are placeholders for sleep's internal data.
   ***************************************************************************/
  
  /* Lock to prevent interleaving of mode's enter and exit functions */
  sleep_mode_fn_lock fn_lock;
   
  /* Reference count for cores sharing this mode. If attribute is not set for
   * custom reference count, this field will be used if the mode is shared 
   * between two or more cores. In that case, owners of LPRM are recommended
   * not to modify this count. */
  uint32 cores_count;   
   
  unsigned char mode_enabled;

  unsigned char disabled_in_global_config;

  /* The following fields are for sleep statistics. Sleep will 
   * calculate stats if it is handling reference counting for the shared
   * mode or the mode is local. If LPR owner chooses to handle ref count 
   * for shared mode(s) by himself, it is his responsibility to calculate
   * these statistics */
  
  sleepStats_mode_stats cycleStats;

  /* Total number of times this mode is entered @ exact frequency match */
  uint32 *mode_cycles_exact;

  /* LPRM time statistics
   * 
   * Exit stats are not represented here because there is currently not
   * enough information at runtime to determine proper exit latency. It will be 
   * added once this information is avaliable.   
   */
  void *mode_stats_enter;
  void *mode_stats_backoff;

  /** End sleep statistics **/

  /* Pointer to structure that holds the data for a given cycle so that 
   * we don't keep calling lprm's non-constant function for the same
   * sleep cycle */
  void *cached_data;

  /* Back-pointer to parent LPR, espcially useful for logging.  Automatically
   * filled in at LPR registration time. */
  sleep_lpr *lpr;

  /* Array of CLprms that this component mode is part of. */
  void **cLprmParents;

  /* Next index to write to, when filling in the cLprmParents array. */
  uint32 parentsIdx;

  /* Number of statistic data elements currently allocated */
  uint32 num_stats_elements;
};

/* 
 * struct that contains information of Low Power Resource (LPR). It will be 
 * defined by user who wants to register his LPR with sleep system.
 */
struct sleep_lpr
{
  /* 
   * The name associated with this resource, that is used to reference the
   * resource by other LPRs.  This name can be used to indicate the LPR name
   * and the LPRM group name.  The name must be a valid C-language identifier.
   * Further more, if the control of this resource is via an NPA resource, the
   * name of the LPR should be the "file" part of the NPA resource name (e.g.
   * an NPA resource named "/xo/cxo" would have a corresponding LPR named just
   * "cxo"). 
   */
  const char *resource_name;

  /* The number of low power modes this resource has. */
  uint32 mode_count;

  /* 
   * The list of low power modes this resource can enter, preferred modes
   * listed first. 
   */
  sleep_lprm *modes;
  
  /************************************************************************* 
   * The field(s) below are optional.
   * They are arranged in a way so that they work with existing resource
   * definitions in backwards compatibility mode.
   ************************************************************************/
   
  /* Bitmask indicating which cores are sharing this resource. Core number
   * starts from 0. For example, if the resource is shared by core 0 
   * and core 2, corresponding bits should be set. Owners of LPRs are 
   * encouraged to use sleep core macros defined above instead of directly
   * setting bits. */
  uint32 sharing_cores;
  
  /* Resource initiliazation function */
  sleep_lpr_init_func_type init;
  
  /* Indicates if this LPR has already been registered with sleep subsystem */
  unsigned char registered;

  /* Indicates the number of times the LPR has been entered */
  uint32 run_count;
};

/* FIXME: move to core/utils.  Macro to assist in declaring the dependency and
 * exclusion arrays. */
#define CORE_ARRAY_SIZE( array ) (sizeof(array)/sizeof(array[0]))
#define CORE_ARRAY( array ) CORE_ARRAY_SIZE(array), array
#define CORE_EMPTY_ARRAY 0, NULL


/*
===============================================================================

                        GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
 * @brief If sleep_power_fuction is known to be polynomial in terms of duration
 *        user can directly use this function. 
 *
 * The function returns the power saved in uA*tick (microampere-ticks). The
 * coefficient of polynomial power function should be in uA (microampere). So 
 * for example, if power function is simply 2*duration_tick(mA*tick), then
 * coefficients will be {0,2000}.
 *
 * @param coeffs: Coefficients for the polynomial of power equation i.e.
 *                coeffs[0] + coeffs[1]*duration_tick + ...
 *                +coeffs[n]*duration_tick^n
 * @param duration: time duration in ticks.
 * 
 * @return Total power saved according to polynomial function for given
 *         duration.
 */
int64 sleep_power_poly_duration(sleep_coeffs *coeffs, uint64 duration);

/**
 * @brief If latency function (either for entering or exiting or both) is 
 *        polynomial, user can directly use this function.
 *
 * @param coeffs: coeffs[0]+ coeffs[1]*x+ coeffs[2]*x^2+ .. +coeffs[n]*x^n
 * @param x_value: Variable part (x) of the polynomial.
 *
 * @return Time required to enter or exit the mode based on given polynomial.
 */
uint64 sleep_latency_poly(sleep_coeffs *coeffs, uint64 x_value);

/**
 * @brief Register a low power resource (and all its associated modes) with the
 *        sleep subsystem.
 *
 * This function is intended to be called once per LPR at startup time.  It
 * registers all of the LPRM's of the resource with the sleep LPR node.
 * This function works in conjunction with the LPRs generated from 
 * an LPR configuration file.  Call this function instead of sleep_define_lpr
 * and pass in the name of the low power resource (eg. "cxo") to register
 * the resource.
 *
 * @param name:          Low power resource name we want to register with sleep 
 *                       system.
 * @param client_handle: The NPA client handle associated with the above LPR
 *
 * @return SLEEP_LPR_SUCCESS if the resource name was found, 
 *         SLEEP_LPR_FAILED if the resource name was not found
 */
int sleep_define_lpr_str(const char* name, npa_client_handle client_handle);

#ifdef __cplusplus
}
#endif

#endif /* SLEEP_LPR_H */
