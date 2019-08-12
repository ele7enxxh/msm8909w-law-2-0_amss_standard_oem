/*============================================================================
  FILE:         VAdcSim.c

  OVERVIEW:     VAdc unit test.

  DEPENDENCIES: VADC_UNIT_TEST needs to be defined.

                Copyright (c) 2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/devices/vadc/src/VAdcSim.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-06-21  jjo  Initial revision

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "VAdcSim.h"
#ifdef VADC_UNIT_TEST

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) \
   (sizeof(a) / sizeof(a[0]))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
unsigned char gaVAdcReg[] = {
   0x00, // 0x00 DIG_MINOR
   0x00, // 0x01 DIG_MAJOR
   0x00, // 0x02 ANA_MINOR
   0x00, // 0x03 ANA_MAJOR
   0x08, // 0x04 PERPH_TYPE
   0x01, // 0x05 PERPH_SUBTYPE

   0x00, // 0x06 RESV
   0x00, // 0x07 RESV

   0x01, // 0x08 STATUS1
   0x00, // 0x09 STATUS2

   0x00, // 0x0A RESV
   0x00, // 0x0B RESV
   0x00, // 0x0C RESV
   0x00, // 0x0D RESV
   0x00, // 0x0E RESV
   0x00, // 0x0F RESV

   0x00, // 0x10 INT_RT_STS
   0x00, // 0x11 INT_SET_TYPE
   0x00, // 0x12 INT_POLARITY_HIGH
   0x00, // 0x13 INT_POLARITY_LOW
   0x00, // 0x14 INT_LATCHED_CLR
   0x00, // 0x15 INT_EN_SET
   0x00, // 0x16 INT_EN_CLR

   0x00, // 0x17 RESV

   0x00, // 0x18 INT_LATCHED_STS
   0x00, // 0x19 INT_PENDING_STS
   0x00, // 0x1A INT_MID_SEL
   0x00, // 0x1B INT_PRIORITY

   0x00, // 0x1C RESV
   0x00, // 0x1D RESV
   0x00, // 0x1E RESV
   0x00, // 0x1F RESV

   0x00, // 0x20 RESV
   0x00, // 0x21 RESV
   0x00, // 0x22 RESV
   0x00, // 0x23 RESV
   0x00, // 0x24 RESV
   0x00, // 0x25 RESV
   0x00, // 0x26 RESV
   0x00, // 0x27 RESV
   0x00, // 0x28 RESV
   0x00, // 0x29 RESV
   0x00, // 0x2A RESV
   0x00, // 0x2B RESV
   0x00, // 0x2C RESV
   0x00, // 0x2D RESV
   0x00, // 0x2E RESV
   0x00, // 0x2F RESV

   0x00, // 0x30 RESV
   0x00, // 0x31 RESV
   0x00, // 0x32 RESV
   0x00, // 0x33 RESV
   0x00, // 0x34 RESV
   0x00, // 0x35 RESV
   0x00, // 0x36 RESV
   0x00, // 0x37 RESV
   0x00, // 0x38 RESV
   0x00, // 0x39 RESV
   0x00, // 0x3A RESV
   0x00, // 0x3B RESV
   0x00, // 0x3C RESV
   0x00, // 0x3D RESV
   0x00, // 0x3E RESV
   0x00, // 0x3F RESV

   0x03, // 0x40 MODE_CTL

   0x00, // 0x41 RESV
   0x00, // 0x42 RESV
   0x00, // 0x43 RESV
   0x00, // 0x44 RESV
   0x00, // 0x45 RESV

   0x00, // 0x46 EN_CTL1

   0x00, // 0x47 RESV

   0x06, // 0x48 ADC_CH_SEL_CTL

   0x00, // 0x49 RESV
   0x00, // 0x4A RESV
   0x00, // 0x4B RESV
   0x00, // 0x4C RESV
   0x00, // 0x4D RESV
   0x00, // 0x4E RESV
   0x00, // 0x4F RESV

   0x50, // 0x50 ADC_DIG_PARAM
   0x00, // 0x51 HW_SETTLE_DELAY
   0x00, // 0x52 CONV_REQ

   0x00, // 0x53 RESV

   0x45, // 0x54 CONV_SEQ_CTL
   0x00, // 0x55 CONV_SEQ_TRIG_CTL

   0x00, // 0x56 RESV

   0x00, // 0x57 MEAS_INTERVAL_CTL

   0x00, // 0x58 RESV

   0x00, // 0x59 MEAS_INTERVAL_OP_CTL
   0x04, // 0x5A FAST_AVG_CTL
   0x80, // 0x5B FAST_AVG_EN
   0x00, // 0x5C LOW_THR0
   0x00, // 0x5D LOW_THR1
   0xFF, // 0x5E HIGH_THR0
   0xFF, // 0x5F HIGH_THR1

   0x00, // 0x60 DATA0
   0x00, // 0x61 DATA1

   0x00, // 0x62 RESV
   0x00, // 0x63 RESV
   0x00, // 0x64 RESV
   0x00, // 0x65 RESV
   0x00, // 0x66 RESV
   0x00, // 0x67 RESV
   0x00, // 0x68 RESV
   0x00, // 0x69 RESV
   0x00, // 0x6A RESV
   0x00, // 0x6B RESV
   0x00, // 0x6C RESV
   0x00, // 0x6D RESV
   0x00, // 0x6E RESV
   0x00, // 0x6F RESV
};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static int32 VAdcSim_GetCode(uint32 uChannel, uint16 *pu16Code)
{
   switch (uChannel)
   {
      case 0x00:
         *pu16Code = 0x6136; // 0x00 USBIN
         break;
      case 0x01:
               *pu16Code = 0x0000; // 0x01 DCIN
         break;
      case 0x02:
         *pu16Code = 0x6135; // 0x02 VCHG_SNS
         break;
      case 0x03:
               *pu16Code = 0x0000; // 0x03 SPARE1_X03
         break;
      case 0x04:
               *pu16Code = 0x0000; // 0x04 SPARE2_X03
         break;
      case 0x05:
         *pu16Code = 0x8b1f; // 0x05 VCOIN
         break;
      case 0x06:
         *pu16Code = 0x96c6; // 0x06 VBAT_SNS
         break;
      case 0x07:
               *pu16Code = 0x0000; // 0x07 VSYS
         break;
      case 0x08:
         *pu16Code = 0x799d; // 0x08 DIE_TEMP
         break;
      case 0x09:
         *pu16Code = 0x79c6; // 0x09 0P625
         break;
      case 0x0A:
         *pu16Code = 0x92d2; // 0x0A 1P250
         break;
      case 0x0B:
         *pu16Code = 0x6125; // 0x0B CHG_TEMP
         break;
      case 0x0C:
               *pu16Code = 0x0000; // 0x0C SPARE1
         break;
      case 0x0D:
               *pu16Code = 0x0000; // 0x0D SPARE2
         break;
      case 0x0E:
         *pu16Code = 0x60dc; // 0x0E GND_REF
         break;
      case 0x0F:
         *pu16Code = 0xa8d0; // 0x0F VDD_VADC
         break;

      // TODO: figure out correct channels
      case 0x30:
         *pu16Code = 0x8332; // 0x30 BATT_THERM temp
         break;
      case 0x32:
         *pu16Code = 0xa76d; // 0x32 XO_THERM temp
         break;
      case 0x33:
         *pu16Code = 0xa869; // 0x33 PA_THERM temp
         break;


      default:
         *pu16Code = 0x0000;
         break;
   }

   return 0;
}

static int32 VAdcSim_Conversion(void)
{
   uint32 uChannel;
   uint32 uReg;
   uint16 u16Code;

   /* indicate conversion in progress */
   gaVAdcReg[0x52] = 0x80;

   uReg = gaVAdcReg[0x08];
   uReg = (uReg & ~0x03) | 0x02;
   gaVAdcReg[0x08] = uReg;

   /* determine which channel is being read */
   uChannel = gaVAdcReg[0x48];

   /* write the ADC code */
   (void)VAdcSim_GetCode(uChannel, &u16Code);
   gaVAdcReg[0x60] = (unsigned char)(u16Code & 0xFF);
   gaVAdcReg[0x61] = (unsigned char)((u16Code >> 8) & 0xFF);

   /* indicate EOC */
   gaVAdcReg[0x52] = 0x00;

   uReg = gaVAdcReg[0x08];
   uReg = (uReg & ~0x03) | 0x01;
   gaVAdcReg[0x08] = uReg;

   return 0;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
int32 VAdcSim_ReadLong(uint32 uOffset, unsigned char *pucData, uint32 uDataLen, uint32 *puTotalBytesRead)
{
   uint32 uCurrentByte;

   for (uCurrentByte = 0; uCurrentByte < uDataLen; uCurrentByte++)
   {
      /* todo: implement a check to make sure we are reading valid registers */
      pucData[uCurrentByte] = gaVAdcReg[uCurrentByte + uOffset];
   }

   *puTotalBytesRead = uDataLen;

   return 0;
}

int32 VAdcSim_WriteLong(uint32 uOffset, unsigned char *pucData, uint32 uDataLen)
{
   int32 ret;
   uint32 uCurrentByte;

   if (uOffset == 0x52)
   {
      ret = VAdcSim_Conversion();

      if (ret != 0)
      {
         return ret;
      }
   }
   else
   {
      for (uCurrentByte = 0; uCurrentByte < uDataLen; uCurrentByte++)
      {
         /* todo: implement a check to make sure we are writing valid registers */
         gaVAdcReg[uCurrentByte + uOffset] = pucData[uCurrentByte];
      }
   }

   return 0;
}

#endif
