/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef VOLTAGE_LEVEL_H
#define VOLTAGE_LEVEL_H
/*
 * Volatge Level defiantion for RPM rails
 *
 *
 * Notes for addign new corners into the Enum::::
 * FIXED ENUM definations: DO NOT ENUM values for belwo SVS/NOM/TURBO 
 * RAIL_VOLTAGE_LEVEL_SVS     = 128
 * RAIL_VOLTAGE_LEVEL_NOMINAL = 256
 * RAIL_VOLTAGE_LEVEL_TURBO   = 384
 * 
 * Please don't change current exiting enum values for the defiend corners. 
 * Newly added corners needs to be defined with new enum value without changing the existing values.
 *
 * Please check with Power/ System driver folks before udpating the header file 
 * */

typedef enum
{
  RAIL_VOLTAGE_LEVEL_OFF           = 0,
  //RETENTION Volatge Level 
  RAIL_VOLTAGE_LEVEL_RETENTION     = 16,
  //SVS Volatge Level
  RAIL_VOLTAGE_LEVEL_SVS_LOW       = 64,
  RAIL_VOLTAGE_LEVEL_SVS           = 128,
  RAIL_VOLTAGE_LEVEL_SVS_HIGH      = 192,
  //NOMINAL Volatge Level
  RAIL_VOLTAGE_LEVEL_NOMINAL       = 256,
  RAIL_VOLTAGE_LEVEL_NOMINAL_HIGH  = 320,
  //TURBO Volatge Level
  RAIL_VOLTAGE_LEVEL_TURBO         = 384,


  RAIL_VOLTAGE_LEVEL_MAX           = 0x7FFFFFF
} rail_voltage_level;


#endif // VOLTAGE_LEVEL_H

