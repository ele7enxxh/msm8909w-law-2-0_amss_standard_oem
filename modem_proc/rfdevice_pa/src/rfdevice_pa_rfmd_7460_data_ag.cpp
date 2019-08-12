
/*
WARNING: This file is auto-generated.

Generated using: pa_autogen.pl
Generated from:  4.0.60 of RFDevice_PA.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2009 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/src/rfdevice_pa_rfmd_7460_data_ag.cpp#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfdevice_pa_rfmd_7460_data_ag.h"


#define RFDEVICE_PA_RFMD_7460_NUM_PORTS 38

#define RFDEVICE_PA_RFMD_7460_PA_SET_BIAS_NUM_REGS 1
static uint8 rfdevice_pa_rfmd_7460_pa_set_bias_regs[RFDEVICE_PA_RFMD_7460_PA_SET_BIAS_NUM_REGS] =  {0x01, };
static int16 rfdevice_pa_rfmd_7460_pa_set_bias_data[RFDEVICE_PA_RFMD_7460_NUM_PORTS][4][RFDEVICE_PA_RFMD_7460_PA_SET_BIAS_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    { 0xAA, },  /* PA Range: 0 */
    { 0xAA, },  /* PA Range: 1 */
    { 0xAA, },  /* PA Range: 2 */
    { 0xAA, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 1 */
    { 0xAA, },  /* PA Range: 0 */
    { 0xAA, },  /* PA Range: 1 */
    { 0xAA, },  /* PA Range: 2 */
    { 0xB9, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 2 */
    { 0xAA, },  /* PA Range: 0 */
    { 0xAA, },  /* PA Range: 1 */
    { 0xAA, },  /* PA Range: 2 */
    { 0xAA, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 3 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 4 */
    { 0xC8, },  /* PA Range: 0 */
    { 0xC8, },  /* PA Range: 1 */
    { 0xC8, },  /* PA Range: 2 */
    { 0xCD, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 5 */
    { 0xC8, },  /* PA Range: 0 */
    { 0xC8, },  /* PA Range: 1 */
    { 0xC8, },  /* PA Range: 2 */
    { 0xBE, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 6 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 7 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 8 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 9 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 10 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 11 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 12 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 13 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 14 */
    { 0xAA, },  /* PA Range: 0 */
    { 0xAA, },  /* PA Range: 1 */
    { 0xAA, },  /* PA Range: 2 */
    { 0xE6, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 15 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 16 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 17 */
    { 0xB9, },  /* PA Range: 0 */
    { 0xB9, },  /* PA Range: 1 */
    { 0xB9, },  /* PA Range: 2 */
    { 0xC3, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 18 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 19 */
    { 0xB4, },  /* PA Range: 0 */
    { 0xB4, },  /* PA Range: 1 */
    { 0xB4, },  /* PA Range: 2 */
    { 0xDC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 20 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 21 */
    { 0xF5, },  /* PA Range: 0 */
    { 0xF5, },  /* PA Range: 1 */
    { 0xF5, },  /* PA Range: 2 */
    { 0xD7, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 22 */
    { 0xF5, },  /* PA Range: 0 */
    { 0xF5, },  /* PA Range: 1 */
    { 0xF5, },  /* PA Range: 2 */
    { 0xD7, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 23 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 24 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 25 */
    { 0xF5, },  /* PA Range: 0 */
    { 0xF5, },  /* PA Range: 1 */
    { 0xF5, },  /* PA Range: 2 */
    { 0xD7, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 26 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 27 */
    { 0xFC, },  /* PA Range: 0 */
    { 0xFC, },  /* PA Range: 1 */
    { 0xFC, },  /* PA Range: 2 */
    { 0xFC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 28 */
    { 0xC8, },  /* PA Range: 0 */
    { 0xC8, },  /* PA Range: 1 */
    { 0xC8, },  /* PA Range: 2 */
    { 0xAA, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 29 */
    { 0xC8, },  /* PA Range: 0 */
    { 0xC8, },  /* PA Range: 1 */
    { 0xC8, },  /* PA Range: 2 */
    { 0xAA, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 30 */
    { 0xAA, },  /* PA Range: 0 */
    { 0xAA, },  /* PA Range: 1 */
    { 0xAA, },  /* PA Range: 2 */
    { 0xAA, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 31 */
    { 0xAA, },  /* PA Range: 0 */
    { 0xAA, },  /* PA Range: 1 */
    { 0xAA, },  /* PA Range: 2 */
    { 0xB9, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 32 */
    { 0xC8, },  /* PA Range: 0 */
    { 0xC8, },  /* PA Range: 1 */
    { 0xC8, },  /* PA Range: 2 */
    { 0xCD, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 33 */
    { 0xC8, },  /* PA Range: 0 */
    { 0xC8, },  /* PA Range: 1 */
    { 0xC8, },  /* PA Range: 2 */
    { 0xBE, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 34 */
    { 0xB9, },  /* PA Range: 0 */
    { 0xB9, },  /* PA Range: 1 */
    { 0xB9, },  /* PA Range: 2 */
    { 0xC3, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 35 */
    { 0xB4, },  /* PA Range: 0 */
    { 0xB4, },  /* PA Range: 1 */
    { 0xB4, },  /* PA Range: 2 */
    { 0xDC, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 36 */
    { 0xAA, },  /* PA Range: 0 */
    { 0xAA, },  /* PA Range: 1 */
    { 0xAA, },  /* PA Range: 2 */
    { 0xE6, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 37 */
    { 0xAA, },  /* PA Range: 0 */
    { 0xAA, },  /* PA Range: 1 */
    { 0xAA, },  /* PA Range: 2 */
    { 0xB9, },  /* PA Range: 3 */
  },
};


#define RFDEVICE_PA_RFMD_7460_PA_SET_RANGE_NUM_REGS 1
static uint8 rfdevice_pa_rfmd_7460_pa_set_range_regs[RFDEVICE_PA_RFMD_7460_PA_SET_RANGE_NUM_REGS] =  {0x00, };
static int16 rfdevice_pa_rfmd_7460_pa_set_range_data[RFDEVICE_PA_RFMD_7460_NUM_PORTS][4][RFDEVICE_PA_RFMD_7460_PA_SET_RANGE_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    { 0x0C, },  /* PA Range: 0 */
    { 0x0C, },  /* PA Range: 1 */
    { 0x0C, },  /* PA Range: 2 */
    { 0x0E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 1 */
    { 0x14, },  /* PA Range: 0 */
    { 0x14, },  /* PA Range: 1 */
    { 0x14, },  /* PA Range: 2 */
    { 0x16, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 2 */
    { 0x1C, },  /* PA Range: 0 */
    { 0x1C, },  /* PA Range: 1 */
    { 0x1C, },  /* PA Range: 2 */
    { 0x1E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 3 */
    { 0x24, },  /* PA Range: 0 */
    { 0x24, },  /* PA Range: 1 */
    { 0x24, },  /* PA Range: 2 */
    { 0x26, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 4 */
    { 0x2C, },  /* PA Range: 0 */
    { 0x2C, },  /* PA Range: 1 */
    { 0x2C, },  /* PA Range: 2 */
    { 0x2E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 5 */
    { 0x3C, },  /* PA Range: 0 */
    { 0x3C, },  /* PA Range: 1 */
    { 0x3C, },  /* PA Range: 2 */
    { 0x3E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 6 */
    { 0x44, },  /* PA Range: 0 */
    { 0x45, },  /* PA Range: 1 */
    { 0x46, },  /* PA Range: 2 */
    { 0x47, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 7 */
    { 0x44, },  /* PA Range: 0 */
    { 0x45, },  /* PA Range: 1 */
    { 0x46, },  /* PA Range: 2 */
    { 0x47, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 8 */
    { 0x4C, },  /* PA Range: 0 */
    { 0x4D, },  /* PA Range: 1 */
    { 0x4E, },  /* PA Range: 2 */
    { 0x4F, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 9 */
    { 0x4C, },  /* PA Range: 0 */
    { 0x4D, },  /* PA Range: 1 */
    { 0x4E, },  /* PA Range: 2 */
    { 0x4F, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 10 */
    { 0x64, },  /* PA Range: 0 */
    { 0x65, },  /* PA Range: 1 */
    { 0x66, },  /* PA Range: 2 */
    { 0x67, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 11 */
    { 0x64, },  /* PA Range: 0 */
    { 0x65, },  /* PA Range: 1 */
    { 0x66, },  /* PA Range: 2 */
    { 0x67, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 12 */
    { 0x6C, },  /* PA Range: 0 */
    { 0x6D, },  /* PA Range: 1 */
    { 0x6E, },  /* PA Range: 2 */
    { 0x6F, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 13 */
    { 0x6C, },  /* PA Range: 0 */
    { 0x6D, },  /* PA Range: 1 */
    { 0x6E, },  /* PA Range: 2 */
    { 0x6F, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 14 */
    { 0x2C, },  /* PA Range: 0 */
    { 0x2C, },  /* PA Range: 1 */
    { 0x2C, },  /* PA Range: 2 */
    { 0x2E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 15 */
    { 0x14, },  /* PA Range: 0 */
    { 0x14, },  /* PA Range: 1 */
    { 0x14, },  /* PA Range: 2 */
    { 0x16, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 16 */
    { 0x2C, },  /* PA Range: 0 */
    { 0x2C, },  /* PA Range: 1 */
    { 0x2C, },  /* PA Range: 2 */
    { 0x2E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 17 */
    { 0x0C, },  /* PA Range: 0 */
    { 0x0C, },  /* PA Range: 1 */
    { 0x0C, },  /* PA Range: 2 */
    { 0x0E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 18 */
    { 0x14, },  /* PA Range: 0 */
    { 0x14, },  /* PA Range: 1 */
    { 0x14, },  /* PA Range: 2 */
    { 0x16, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 19 */
    { 0x1C, },  /* PA Range: 0 */
    { 0x1C, },  /* PA Range: 1 */
    { 0x1C, },  /* PA Range: 2 */
    { 0x1E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 20 */
    { 0x24, },  /* PA Range: 0 */
    { 0x24, },  /* PA Range: 1 */
    { 0x24, },  /* PA Range: 2 */
    { 0x26, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 21 */
    { 0x2C, },  /* PA Range: 0 */
    { 0x2C, },  /* PA Range: 1 */
    { 0x2C, },  /* PA Range: 2 */
    { 0x2E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 22 */
    { 0x3C, },  /* PA Range: 0 */
    { 0x3C, },  /* PA Range: 1 */
    { 0x3C, },  /* PA Range: 2 */
    { 0x3E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 23 */
    { 0x74, },  /* PA Range: 0 */
    { 0x74, },  /* PA Range: 1 */
    { 0x74, },  /* PA Range: 2 */
    { 0x76, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 24 */
    { 0x74, },  /* PA Range: 0 */
    { 0x74, },  /* PA Range: 1 */
    { 0x74, },  /* PA Range: 2 */
    { 0x76, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 25 */
    { 0x34, },  /* PA Range: 0 */
    { 0x34, },  /* PA Range: 1 */
    { 0x34, },  /* PA Range: 2 */
    { 0x36, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 26 */
    { 0x74, },  /* PA Range: 0 */
    { 0x74, },  /* PA Range: 1 */
    { 0x74, },  /* PA Range: 2 */
    { 0x76, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 27 */
    { 0x24, },  /* PA Range: 0 */
    { 0x24, },  /* PA Range: 1 */
    { 0x24, },  /* PA Range: 2 */
    { 0x26, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 28 */
    { 0x24, },  /* PA Range: 0 */
    { 0x24, },  /* PA Range: 1 */
    { 0x24, },  /* PA Range: 2 */
    { 0x26, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 29 */
    { 0x24, },  /* PA Range: 0 */
    { 0x24, },  /* PA Range: 1 */
    { 0x24, },  /* PA Range: 2 */
    { 0x26, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 30 */
    { 0x24, },  /* PA Range: 0 */
    { 0x24, },  /* PA Range: 1 */
    { 0x24, },  /* PA Range: 2 */
    { 0x26, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 31 */
    { 0x0C, },  /* PA Range: 0 */
    { 0x0C, },  /* PA Range: 1 */
    { 0x0C, },  /* PA Range: 2 */
    { 0x0E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 32 */
    { 0x3C, },  /* PA Range: 0 */
    { 0x3C, },  /* PA Range: 1 */
    { 0x3C, },  /* PA Range: 2 */
    { 0x3E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 33 */
    { 0x2C, },  /* PA Range: 0 */
    { 0x2C, },  /* PA Range: 1 */
    { 0x2C, },  /* PA Range: 2 */
    { 0x2E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 34 */
    { 0x24, },  /* PA Range: 0 */
    { 0x24, },  /* PA Range: 1 */
    { 0x24, },  /* PA Range: 2 */
    { 0x26, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 35 */
    { 0x14, },  /* PA Range: 0 */
    { 0x14, },  /* PA Range: 1 */
    { 0x14, },  /* PA Range: 2 */
    { 0x16, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 36 */
    { 0x3C, },  /* PA Range: 0 */
    { 0x3C, },  /* PA Range: 1 */
    { 0x3C, },  /* PA Range: 2 */
    { 0x3E, },  /* PA Range: 3 */
  },
  { /* PORT NUM: 37 */
    { 0x1C, },  /* PA Range: 0 */
    { 0x1C, },  /* PA Range: 1 */
    { 0x1C, },  /* PA Range: 2 */
    { 0x1E, },  /* PA Range: 3 */
  },
};


#define RFDEVICE_PA_RFMD_7460_PA_ON_NUM_REGS 1
static uint8 rfdevice_pa_rfmd_7460_pa_on_regs[RFDEVICE_PA_RFMD_7460_PA_ON_NUM_REGS] =  {RFFE_INVALID_REG_ADDR /*Warning: Not Specified*/, };
static int16 rfdevice_pa_rfmd_7460_pa_on_data[RFDEVICE_PA_RFMD_7460_NUM_PORTS][RFDEVICE_PA_RFMD_7460_PA_ON_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 1 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 2 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 3 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 4 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 5 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 6 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 7 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 8 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 9 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 10 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 11 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 12 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 13 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 14 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 15 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 16 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 17 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 18 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 19 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 20 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 21 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 22 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 23 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 24 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 25 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 26 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 27 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 28 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 29 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 30 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 31 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 32 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 33 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 34 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 35 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 36 */
    RF_REG_INVALID, 
  },
  { /* PORT NUM: 37 */
    RF_REG_INVALID, 
  },
};


#define RFDEVICE_PA_RFMD_7460_PA_OFF_NUM_REGS 1
static uint8 rfdevice_pa_rfmd_7460_pa_off_regs[RFDEVICE_PA_RFMD_7460_PA_OFF_NUM_REGS] =  {0x00, };
static int16 rfdevice_pa_rfmd_7460_pa_off_data[RFDEVICE_PA_RFMD_7460_NUM_PORTS][RFDEVICE_PA_RFMD_7460_PA_OFF_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    0x0A, 
  },
  { /* PORT NUM: 1 */
    0xA, 
  },
  { /* PORT NUM: 2 */
    0xA, 
  },
  { /* PORT NUM: 3 */
    0xA, 
  },
  { /* PORT NUM: 4 */
    0x0A, 
  },
  { /* PORT NUM: 5 */
    0x0A, 
  },
  { /* PORT NUM: 6 */
    0x0A, 
  },
  { /* PORT NUM: 7 */
    0x0A, 
  },
  { /* PORT NUM: 8 */
    0x0A, 
  },
  { /* PORT NUM: 9 */
    0x0A, 
  },
  { /* PORT NUM: 10 */
    0x0A, 
  },
  { /* PORT NUM: 11 */
    0x0A, 
  },
  { /* PORT NUM: 12 */
    0x0A, 
  },
  { /* PORT NUM: 13 */
    0x0A, 
  },
  { /* PORT NUM: 14 */
    0x0A, 
  },
  { /* PORT NUM: 15 */
    0xA, 
  },
  { /* PORT NUM: 16 */
    0xA, 
  },
  { /* PORT NUM: 17 */
    0x0A, 
  },
  { /* PORT NUM: 18 */
    0xA, 
  },
  { /* PORT NUM: 19 */
    0x0A, 
  },
  { /* PORT NUM: 20 */
    0xA, 
  },
  { /* PORT NUM: 21 */
    0x0A, 
  },
  { /* PORT NUM: 22 */
    0x0A, 
  },
  { /* PORT NUM: 23 */
    0xA, 
  },
  { /* PORT NUM: 24 */
    0xA, 
  },
  { /* PORT NUM: 25 */
    0x0A, 
  },
  { /* PORT NUM: 26 */
    0x0A, 
  },
  { /* PORT NUM: 27 */
    0x0A, 
  },
  { /* PORT NUM: 28 */
    0x0A, 
  },
  { /* PORT NUM: 29 */
    0x0A, 
  },
  { /* PORT NUM: 30 */
    0x0A, 
  },
  { /* PORT NUM: 31 */
    0x0A, 
  },
  { /* PORT NUM: 32 */
    0x0A, 
  },
  { /* PORT NUM: 33 */
    0x0A, 
  },
  { /* PORT NUM: 34 */
    0x0A, 
  },
  { /* PORT NUM: 35 */
    0x0A, 
  },
  { /* PORT NUM: 36 */
    0x0A, 
  },
  { /* PORT NUM: 37 */
    0x0A, 
  },
};


#define RFDEVICE_PA_RFMD_7460_PA_TRIGGER_NUM_REGS 1
static uint8 rfdevice_pa_rfmd_7460_pa_trigger_regs[RFDEVICE_PA_RFMD_7460_PA_TRIGGER_NUM_REGS] =  {0x1C, };
static int16 rfdevice_pa_rfmd_7460_pa_trigger_data[RFDEVICE_PA_RFMD_7460_NUM_PORTS][RFDEVICE_PA_RFMD_7460_PA_TRIGGER_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    0x07, 
  },
  { /* PORT NUM: 1 */
    0x07, 
  },
  { /* PORT NUM: 2 */
    0x07, 
  },
  { /* PORT NUM: 3 */
    0x07, 
  },
  { /* PORT NUM: 4 */
    0x07, 
  },
  { /* PORT NUM: 5 */
    0x07, 
  },
  { /* PORT NUM: 6 */
    0x07, 
  },
  { /* PORT NUM: 7 */
    0x07, 
  },
  { /* PORT NUM: 8 */
    0x07, 
  },
  { /* PORT NUM: 9 */
    0x07, 
  },
  { /* PORT NUM: 10 */
    0x07, 
  },
  { /* PORT NUM: 11 */
    0x07, 
  },
  { /* PORT NUM: 12 */
    0x07, 
  },
  { /* PORT NUM: 13 */
    0x07, 
  },
  { /* PORT NUM: 14 */
    0x07, 
  },
  { /* PORT NUM: 15 */
    0x07, 
  },
  { /* PORT NUM: 16 */
    0x07, 
  },
  { /* PORT NUM: 17 */
    0x07, 
  },
  { /* PORT NUM: 18 */
    0x07, 
  },
  { /* PORT NUM: 19 */
    0x07, 
  },
  { /* PORT NUM: 20 */
    0x07, 
  },
  { /* PORT NUM: 21 */
    0x07, 
  },
  { /* PORT NUM: 22 */
    0x07, 
  },
  { /* PORT NUM: 23 */
    0x07, 
  },
  { /* PORT NUM: 24 */
    0x07, 
  },
  { /* PORT NUM: 25 */
    0x07, 
  },
  { /* PORT NUM: 26 */
    0x07, 
  },
  { /* PORT NUM: 27 */
    0x07, 
  },
  { /* PORT NUM: 28 */
    0x07, 
  },
  { /* PORT NUM: 29 */
    0x07, 
  },
  { /* PORT NUM: 30 */
    0x07, 
  },
  { /* PORT NUM: 31 */
    0x07, 
  },
  { /* PORT NUM: 32 */
    0x07, 
  },
  { /* PORT NUM: 33 */
    0x07, 
  },
  { /* PORT NUM: 34 */
    0x07, 
  },
  { /* PORT NUM: 35 */
    0x07, 
  },
  { /* PORT NUM: 36 */
    0x07, 
  },
  { /* PORT NUM: 37 */
    0x07, 
  },
};

/* singleton instance ptr */
rfdevice_pa_data *rfdevice_pa_rfmd_7460_data_ag::rfdevice_pa_rfmd_7460_data_ptr = NULL;

rfdevice_pa_data * rfdevice_pa_rfmd_7460_data_ag::get_instance()
{
  if (rfdevice_pa_rfmd_7460_data_ptr == NULL)
  {
    rfdevice_pa_rfmd_7460_data_ptr = (rfdevice_pa_data *)new rfdevice_pa_rfmd_7460_data_ag();
  }
  return( (rfdevice_pa_data *)rfdevice_pa_rfmd_7460_data_ptr);
}

//constructor
rfdevice_pa_rfmd_7460_data_ag::rfdevice_pa_rfmd_7460_data_ag()
  :rfdevice_pa_data()
{
}

//destructor
rfdevice_pa_rfmd_7460_data_ag::~rfdevice_pa_rfmd_7460_data_ag()
{
  rfdevice_pa_rfmd_7460_data_ptr = NULL;
}

boolean rfdevice_pa_rfmd_7460_data_ag::settings_data_get( rfdevice_pa_cfg_params_type *cfg, 
                                                          rfdevice_pa_reg_settings_type *settings)
{
  boolean ret_val = FALSE;

  if (NULL == settings || NULL == cfg)
  {
    return FALSE;
  }

  if (cfg->port >= RFDEVICE_PA_RFMD_7460_NUM_PORTS )
  {
    settings->addr = NULL;
    settings->data = NULL;
    settings->num_regs = 0;
    return FALSE;
  }

  if ( (cfg->req == RFDEVICE_PA_SET_BIAS_DATA) && (cfg->pa_range < 4) )
  {
    settings->addr = &(rfdevice_pa_rfmd_7460_pa_set_bias_regs[0]);
    settings->data = &(rfdevice_pa_rfmd_7460_pa_set_bias_data[cfg->port][cfg->pa_range][0]);
    settings->num_regs = RFDEVICE_PA_RFMD_7460_PA_SET_BIAS_NUM_REGS;
    settings->bias_reg_idx = 0;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_SET_RANGE_DATA) && (cfg->pa_range < 4) )
  {
    settings->addr = &(rfdevice_pa_rfmd_7460_pa_set_range_regs[0]);
    settings->data = &(rfdevice_pa_rfmd_7460_pa_set_range_data[cfg->port][cfg->pa_range][0]);
    settings->num_regs = RFDEVICE_PA_RFMD_7460_PA_SET_RANGE_NUM_REGS;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_ON_DATA) )
  {
    settings->addr = &(rfdevice_pa_rfmd_7460_pa_on_regs[0]);
    settings->data = &(rfdevice_pa_rfmd_7460_pa_on_data[cfg->port][0]);
    settings->num_regs = RFDEVICE_PA_RFMD_7460_PA_ON_NUM_REGS;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_OFF_DATA) )
  {
    settings->addr = &(rfdevice_pa_rfmd_7460_pa_off_regs[0]);
    settings->data = &(rfdevice_pa_rfmd_7460_pa_off_data[cfg->port][0]);
    settings->num_regs = RFDEVICE_PA_RFMD_7460_PA_OFF_NUM_REGS;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_TRIGGER_DATA) )
  {
    settings->addr = &(rfdevice_pa_rfmd_7460_pa_trigger_regs[0]);
    settings->data = &(rfdevice_pa_rfmd_7460_pa_trigger_data[cfg->port][0]);
    settings->num_regs = RFDEVICE_PA_RFMD_7460_PA_TRIGGER_NUM_REGS;
    ret_val = TRUE;
  }

  return ret_val;
}

boolean rfdevice_pa_rfmd_7460_data_ag::sequence_data_get( rfdevice_pa_cfg_params_type *cfg, 
                                                          rfdevice_pa_cmd_seq_type *cmd_seq)
{
  boolean ret_val = FALSE;

  if (NULL == cmd_seq || NULL == cfg)
  {
    return FALSE;
  }

  else if (RFDEVICE_PA_RESET_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else if (RFDEVICE_PA_THERM_ON_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else if (RFDEVICE_PA_THERM_READ_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else if (RFDEVICE_PA_THERM_OFF_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
  }

  return ret_val;
}

boolean rfdevice_pa_rfmd_7460_data_ag::device_info_get( rfdevice_pa_info_type *pa_info )
{
  boolean ret_val = FALSE;

  if ( NULL == pa_info )
  {
    return FALSE;
  }
  else
  {
    pa_info->mfg_id = 0x134;
    pa_info->prd_id = 0x00;
    pa_info->prd_rev = 1;
    pa_info->num_ports = RFDEVICE_PA_RFMD_7460_NUM_PORTS;
    pa_info->num_pa_ranges = 4;
	pa_info->lpm_pm_trig_flag = FALSE;	
    ret_val = TRUE;
  }
  return ret_val;
}

boolean rfdevice_pa_rfmd_7460_data_ag::get_temperature_lut(int16 **lut_addr, uint8 *lut_size)
{
   boolean ret_val = FALSE;   

   if (lut_addr != NULL)  
   {
      *lut_addr = NULL; 
      *lut_size = -1;
      ret_val = TRUE;
   }

   else 
   {
      ret_val = FALSE;
   }

   return ret_val;
}

