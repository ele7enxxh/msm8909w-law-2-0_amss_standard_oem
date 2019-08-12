#ifndef DALSPS_PROPS_H
#define DALSPS_PROPS_H
/*==============================================================================

FILE:      dalsps_props.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/dal/config/dalsps_props.h#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-04-16  jc   Eliminated most of the BAM HAL config data.
  2009-07-16  jc   Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/* Export SPS DAL and BAM HAL configuration structs as uint32 arrays
 * to eliminate need for exporting the struct types and issues with
 * include paths.
 */
extern unsigned long DalSps_Config[];
extern unsigned long DalSps_Config_Bams[];
extern unsigned long DalSps_Config_Mem[];
extern unsigned long DalSps_Config_Maps[];

/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif   /* DALSPS_PROPS_H */
