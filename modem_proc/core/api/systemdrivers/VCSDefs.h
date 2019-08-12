#ifndef VCSDEFS_H
#define VCSDEFS_H
/*
===========================================================================
*/
/**
  @file VCSDefs.h
  @brief Public definitions include file for accessing the VCS device driver.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The VCS_Driver_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      LaTeX. To edit or update any of the file/group text in the PDF, edit the 
      VCS_Driver_mainpage.dox file or contact Tech Pubs.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/VCSDefs.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h" /* for "uint32" */
#include "npa_resource.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/** @addtogroup vcs_functions
@{ */


/**
  Rail Identifiers
  This list should be common across all images.
 */
typedef enum
{
  VCS_RAIL_CX,
  VCS_RAIL_MSS,
  VCS_RAIL_APCS,
  VCS_RAIL_MX,

  VCS_RAIL_NUM_OF_RAILS
} VCSRailType;


/**
  Corner enums.
 */
typedef enum
{
  VCS_CORNER_OFF            = 0,
  VCS_CORNER_RETENTION      = 1,
  VCS_CORNER_RETENTION_PLUS = 2,
  VCS_CORNER_LOW_MINUS      = 3,
  VCS_CORNER_LOW            = 4,
  VCS_CORNER_LOW_PLUS       = 5,
  VCS_CORNER_NOMINAL        = 6,
  VCS_CORNER_NOMINAL_PLUS   = 7,
  VCS_CORNER_TURBO          = 8,
  VCS_CORNER_MAX            = VCS_CORNER_TURBO,

  VCS_CORNER_NUM_OF_CORNERS
} VCSCornerType;


/**
  Rail modes.
 */
typedef enum
{
  VCS_RAIL_MODE_CPR = 0,
  VCS_RAIL_MODE_MVC = 1,

  VCS_RAIL_MODE_NUM_OF_MODES
} VCSRailModeType;


/*
 * VCS NPA Rail Events.
 */
typedef enum
{
  VCS_NPA_RAIL_EVENT_PRE_CHANGE   = NPA_TRIGGER_CUSTOM_EVENT1,
  VCS_NPA_RAIL_EVENT_POST_CHANGE  = NPA_TRIGGER_CUSTOM_EVENT2,
  VCS_NPA_RAIL_EVENT_LIMIT_MAX    = NPA_TRIGGER_CUSTOM_EVENT3,
} VCSNPARailEventType;


/*
 * VCS NPA LDO Events.
 */
typedef enum
{
  VCS_NPA_LDO_EVENT_PRE_CHANGE  = NPA_TRIGGER_CUSTOM_EVENT1,
  VCS_NPA_LDO_EVENT_POST_CHANGE = NPA_TRIGGER_CUSTOM_EVENT2,
} VCSNPALDOEventType;


/*
 * VCS NPA Rail Event Data.
 */
typedef struct
{
  struct
  {
    VCSCornerType eCorner; // Voltage corner before the change event completes.
                           // Empty for limit max events.
  } PreChange;
  struct
  {
    VCSCornerType eCorner; // Voltage corner after the change event completes.
                           // New max corner for limit max events.
  } PostChange;
} VCSNPARailEventDataType;


/*
 * VCS NPA LDO Event Data.
 */
typedef struct
{
  struct
  {
    VCSCornerType eCorner;    // Voltage corner before the change event completes.
    uint32        nVoltageUV; // Voltage in uV before the change event completes.
  } PreChange;
  struct
  {
    VCSCornerType eCorner;    // Voltage corner after the change event completes.
    uint32        nVoltageUV; // Voltage in uV after the change event completes.
  } PostChange;
} VCSNPALDOEventDataType;


/*
 * NPA Resources Exported by VCS
 * The VCS_NPA_RESOURCE_VDD_MSS resource accepts votes for voltage changes,
 * while the VCS_NPA_RESOURCE_VDD_MSS_UV only broadcasts the MSS voltage in uV.
 * It does not accept votes.
 */
#define VCS_NPA_RESOURCE_VDD_MSS    "/vdd/mss"
#define VCS_NPA_RESOURCE_VDD_MSS_UV "/vdd/mss/uv"
#define VCS_NPA_RESOURCE_VDD_CX     "/vdd/cx"
#define VCS_NPA_RESOURCE_VDD_MX     "/vdd/mx"
#define VCS_NPA_RESOURCE_LDO_MSS_Q6 "/vdd/ldo/mss_q6"


/** @} */ /* end_addtogroup vcs_functions */

/** @addtogroup npa_functions
@{ */
/** @name NPA Query Requests
@{ */

/**
  NPA query ID for the number of voltage levels.
  This query returns the number of voltage levels for the resource
  based on the target and configured limitations.
 */
#define VCS_NPA_QUERY_VOLTAGE_NUM_ENTRIES                                      \
  (NPA_QUERY_RESERVED_END + 1)

/**
  NPA query ID for the current voltage in uV.
  This query returns the current voltage in uV
  based on the target and configured limitations.
 */
#define VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_UV                               \
  (VCS_NPA_QUERY_VOLTAGE_NUM_ENTRIES + 1)

/**
  NPA query ID for the current voltage level enum value.
  This query returns the voltage level enum for the resource
  based on the target and configured limitations.
 */
#define VCS_NPA_QUERY_VOLTAGE_CURRENT_LEVEL_ID                                 \
  (VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_UV + 1)

/**
  NPA query ID for the voltage level id. This query gets the enum id
  for the requested level using the enum from VCSCornerType defined above.
  The actual voltage level is to be added to this ID (i.e., to query voltage
  level id for level 2, pass VCS_NPA_QUERY_VOLTAGE_LEVEL_ID + 2).
  This interfaces uses 0 as the first index.
 */
#define VCS_NPA_QUERY_VOLTAGE_LEVEL_ID                                         \
  (VCS_NPA_QUERY_VOLTAGE_CURRENT_LEVEL_ID + 1)

/**
  NPA query ID for the floor voltage at the requested level.
  The actual voltage level is to be added to this ID (i.e., to query
  voltage level id for level 2, pass VCS_NPA_QUERY_VOLTAGE_LEVEL_ID + 2).
  This interfaces uses 0 as the first index.
 */
#define VCS_NPA_QUERY_VOLTAGE_LEVEL_FLOOR                                      \
  (VCS_NPA_QUERY_VOLTAGE_LEVEL_ID + 0x20)

/**
  NPA query ID for the ceiling voltage at the requested level.
  The actual voltage level is to be added to this ID (i.e., to query
  voltage level id for level 2, pass VCS_NPA_QUERY_VOLTAGE_LEVEL_ID + 2).
  This interfaces uses 0 as the first index.
 */
#define VCS_NPA_QUERY_VOLTAGE_LEVEL_CEILING                                    \
  (VCS_NPA_QUERY_VOLTAGE_LEVEL_FLOOR + 0x20)


/** @} */ /* end_name_group NPA Query Requests */
/** @} */ /* end_addtogroup npa_functions */


#endif /* !VCSDEFS_H */

