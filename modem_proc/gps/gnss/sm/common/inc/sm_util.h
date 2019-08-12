#ifndef SM_UTIL_H
#define SM_UTIL_H
/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*============================================================================
  FILE:         sm_util.h

  OVERVIEW:     


  DEPENDENCIES: If the code in this file has any notable dependencies,
                describe them here.  Any initialization and sequencing
                requirements, or assumptions about the overall state of
                the system belong here.
 
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/inc/sm_util.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------

============================================================================*/

/*------------------------------------------------------------------------------
File comment
------------------------------------------------------------------------------*/

/**
* @file sm_util.h
    
* file containing useful functions for SM module, e.g.
* translating between Gen7 and Gen8 structs
 
*/
#include "gps_variation.h"
#include <gnss_common.h>
#include "mgp_api.h"

/* converts the given argument to boolean */
#define SM_UTIL_MAKE_BOOL(arg) (!!(arg))

/* SM Internal Configuration State Flags for config handling, which should not be exposed outside of SM */
// In short, compile time flags indicates what is available. Runtime flags indicates what is currently 
// enabled. Please refer to sm_init_constellation_config() for further details.
// In SM software there's need to know if certain constellation is active. To avoid duplicating the logic
// for determining the current config state, there's a simple function to query the status sm_is_constellation_supported()

// Runtime constellation configuration is set on boot based on the value set in NV.   
// SM gets the runtime setting by calling mgp_GnssGetConstellationConfig(). 
#define SM_CCFG_RT_GPS  (0x0001<<0)
#define SM_CCFG_RT_GLO  (0x0001<<1)
#define SM_CCFG_RT_BDS  (0x0001<<2)
#define SM_CCFG_RT_GAL  (0x0001<<3)
#define SM_CCFG_RT_QZSS (0x0001<<4)
#define SM_CCFG_RT_ALL  (SM_CCFG_RT_GPS|SM_CCFG_RT_GLO|SM_CCFG_RT_BDS|SM_CCFG_RT_GAL|SM_CCFG_RT_QZSS)
/* Compile time Constellation Configuration */
// Compile time constallation configuration indicates which constellations can be enabled on runtime. 
// If certain constellation is disabled by compiletime featurization, there's no way to enabled it by
// changing NV settings. That's because compile time setting may remove portions of the code for memory
// saving purposes from the build.
#define SM_CCFG_CT_GPS  (0x0001<<0)
#define SM_CCFG_CT_GLO  (0x0001<<1)
#define SM_CCFG_CT_BDS  (0x0001<<2)
#define SM_CCFG_CT_GAL  (0x0001<<3)
#define SM_CCFG_CT_QZSS (0x0001<<4)
#define SM_CCFG_CT_ALL  (SM_CCFG_CT_GPS|SM_CCFG_CT_GLO|SM_CCFG_CT_BDS|SM_CCFG_CT_GAL|SM_CCFG_CT_QZSS)

#define SM_CCFG_NONE    (0x0)

typedef uint16 sm_ConstellationCfgType;


/**
* @brief  Query supported runtime constellations from MGP and initialize supported SM constellation configuration.
* 
* @return  void
* @sideeffects None
* @see mgp_GnssGetConstellationConfig()
* 
*/

void sm_init_constellation_config();


/**
* @brief  Sets supported GNSS constellations
* 
* @return  void
* @sideeffects None
* @see 
* 
*/

void sm_set_constellation_config(sm_ConstellationCfgType cfg_mask);

/**
* @brief  check if GNSS constelletion is supported.
* 
* @return  true if supported, false not supported.
* @side effects None
* @see
* 
*/
boolean
sm_is_constellation_supported(sm_ConstellationCfgType cfg_mask);


/**
* @brief  enable compile time GPS constellation supported.
* 
* @return  SM_CCFG_CT_GPS if supported, 0 not supported.
* @side effects None
* @see
* 
*/

sm_ConstellationCfgType sm_set_constellation_config_gps();
/**
* @brief  enable compile time GLO constellation supported.
* 
* @return  SM_CCFG_CT_GLO if supported, 0 not supported.
* @side effects None
* @see
* 
*/
sm_ConstellationCfgType sm_set_constellation_config_glo();
/**
* @brief  enable compile time BDS constellation supported.
* 
* @return  SM_CCFG_CT_BDS if supported, 0 not supported.
* @side effects None
* @see
* 
*/

sm_ConstellationCfgType sm_set_constellation_config_bds();
/**
* @brief  enable compile time GAL constellation supported.
* 
* @return  SM_CCFG_CT_GAL if supported, 0 not supported.
* @side effects None
* @see
* 
*/
sm_ConstellationCfgType sm_set_constellation_config_gal();
/**
* @brief  enable compile time QZSS constellation supported.
* 
* @return  SM_CCFG_CT_QZSS if supported, 0 not supported.
* @side effects None
* @see
* 
*/
sm_ConstellationCfgType sm_set_constellation_config_qzss();

/**
* sm_is_meas_blk_active
*
* @brief 
*  This function checks if measurement block source is supported by
*  current configuration. Function returns true if eMeasBlkSrc1 == eMeasBlkSrc2
*  and constellation is enabled by software configuration. 
* @param e_MeasBlkSrc1 - first measurement block source for comparison
* @param e_MeasBlkSrc2 - second measurement block source for comparison
* @return
*  TRUE or FALSE
* @sideeffects None
*
*/

boolean sm_is_meas_blk_active(uint32 e_MeasBlkSrc1,uint32 e_MeasBlkSrc2);

/**
* @brief  converts GNSS time to gps time
* 
* @return  boolean. false on failure
* @side effects None
* @see
* 
*/
boolean 
sm_translate_gnss_time_to_gps_time(
   sm_ReportGpsTimeStructType *p_Dest, 
   const sm_ReportGnssTimeStructType *p_Src);

/**
* @brief  converts GNSS assistance data to internal assistance
*         data type. this function is used to help bring about
*         changes in internal code in phases.
* 
* @return  boolean. false on failure
* @sideeffects None
* @see
* 
*/
boolean 
sm_translate_gnss_assist_data_to_internal_assist_data( sm_InternalAssistDataStatusStructType* p_Dest,
                                                       const sm_GnssAssistDataStatusStructType *p_Src);

boolean
sm_translate_sm_fix_report_to_gnss_nav_solution(
   gnss_NavSolutionStructType* p_dest, 
   const sm_GnssFixRptStructType* p_src,
   boolean b_FinalFix,
   boolean b_backgroundFix);


/**
 * Translates sm's new SVInfo structure into the old array which
 * is defined in gnss nav solution structure so 
 * that it may be used to generate old-style log 
 * @param p_destSVInfo
 * @param p_srcSVInfo 
 * @param q_validBitmask  all bits must be present (if more than 
 *                        one bit)
 * 
 * @return uint32  number of SVs populated in destSVInfo array
 */
uint32
sm_generate_sv_array_from_svinfo_struct(
   uint16 *p_destSV,   
   uint32 q_destMaxNum,
   const sm_GnssSvInfoType *p_srcSVInfo,
   uint32 q_srcMaxNum,
   uint32 q_validBitmask);

/**
 * Translates sm's new SVInfo structure into the old 8-bit array which
 * is defined in gnss nav solution structure so 
 * that it may be used to generate old-style log 
 * @param p_destSVInfo
 * @param p_srcSVInfo 
 * @param q_validBitmask  all bits must be present (if more than 
 *                        one bit)
 * 
 * @return uint32  number of SVs populated in destSVInfo array
 */
uint32
sm_generate_sv_array8_from_svinfo_struct(
   uint8 *p_destSV,
   uint32 q_destMaxNum,
   const sm_GnssSvInfoType *p_srcSVInfo,
   uint32 q_srcMaxNum,
   uint32 q_validBitmask);

#endif /* SM_UTIL_H */
