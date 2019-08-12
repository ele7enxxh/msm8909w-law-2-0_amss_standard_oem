/**
@file PrngCL.c 
@brief PRNG Engine source file 
*/

/*===========================================================================

                     P R N G E n g i n e D r i v e r

DESCRIPTION
  This file contains declarations and definitions for the
  interface between PRNG engine api and the PRNG hardware

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2009 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/rpm.bf/2.1.1/core/securemsm/cryptodrivers/prng/chipset/msm8952/src/PrngCL.c#1 $
 $DateTime: 2015/02/25 02:53:43 $
 $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2014-02-20   nk     Sosftware Prng removed
2014-01-31   nk     Software PRNG for bringup
2012-09-19   amen   Added PRNG lite api's, dont use mutex or dal apis, direct register config
2011-09-08   nk     Added the implementation of de-init routine. 
2010-06-25   yk     Initial version
============================================================================*/
#include "comdef.h"
#include "PrngCL.h"
#include "PrngEL.h"
#include "PrngCL_Target.h"
#include "string.h"

/**Enable this flag to allow run of standalone builds (without TZ) */
uint32 g_prng_config_enable = 0;

/**
 * @brief This function initializes PRNG Engine.
 *
 * @param None
 * @param None
 *
 * @return None
 *
 * @see PrngCL_getdata
 *
 */
PrngCL_Result_Type PrngCL_init(void)
{
   PrngCL_Result_Type stat = PRNGCL_ERROR_NONE;

   /* Enable clock for random number generator */
   stat = (PrngCL_Result_Type)PrngEL_ClkEnable();

   if(g_prng_config_enable) // TZ will configure PRNG
   {
     HWIO_OUT(SEC_PRNG_LFSR_CFG,
          (0x5                                                                                 | //Set RING_OSC0_CFG to 101, which is feedback point 1
          (1 << HWIO_SHFT(SEC_PRNG_LFSR_CFG, SEC_LFSR0_EN)) | //Enable LFSR0_EN
          0x50                                                                                 | //Set RING_OSC1_CFG to 101, which is feedback point 1 
          (1 << HWIO_SHFT(SEC_PRNG_LFSR_CFG, SEC_LFSR1_EN)) | //Enable LFSR1_EN
          0x500                                                                                | //Set RING_OSC2_CFG to 101, which is feedback point 1
          (1 << HWIO_SHFT(SEC_PRNG_LFSR_CFG, SEC_LFSR2_EN)) | //Enable LFSR2_EN
          0x5000                                                                               | //Set RING_OSC3_CFG to 101, which is feedback point 1
          (1 << HWIO_SHFT(SEC_PRNG_LFSR_CFG, SEC_LFSR3_EN))    //Enable LFSR3_EN
          ));

     HWIO_OUT(SEC_PRNG_CONFIG,
          ((1 << HWIO_SHFT(SEC_PRNG_CONFIG, SEC_PRNG_EN)) ));
   }
   return stat;
}
/**
 * @brief  This function returns the contents of the PRNG_DATA_OUT register.
 *
 * @param random_ptr [in]pointer to random number
 * @param random_len [in]length of random number
 *
 * @return PrngCL_Resut_Type
 *
 * @see PrngCL_init
 *
 */
PrngCL_Result_Type PrngCL_getdata(uint8*  random_ptr,  uint16  random_len)
{
  PrngCL_Result_Type ret_val = PRNGCL_ERROR_NONE;
  uint32 tmp_iv;
  uint32 i;
  uint32 tmp_random_len = random_len;
  const uint16 unit_random_len = 4;
   
  if(!random_ptr || !random_len)
  {
      return PRNGCL_ERROR_INVALID_PARAM; 
  }

  if(random_len%unit_random_len !=0)
  {
    tmp_random_len += (unit_random_len - random_len%unit_random_len);
  }
  
  /* Generate random numbers. Unit length of PRNG is 4 Bytes. 
  *  Hence, multiple PRNG random numbers are generated if random_len > 4 */
  for (i=0; i<(tmp_random_len/unit_random_len); i++)
  {
    /* check PRNG_STATUS */
    while(1)
    {
      uint32 prng_status = HWIO_IN(SEC_PRNG_STATUS);
      if(prng_status &  SEC_HWIO_PRNG_STATUS_DATA_AVAIL_BMSK)	
      {
        break;
      }
    }

    /* Get the randum number from PRNG_DATA_OUT and check if it is not 0 */
    while(1)
    {
      tmp_iv = HWIO_IN(SEC_PRNG_DATA_OUT);
      if (tmp_iv != 0) break;
    }

    /* Generate mutiple of 4 bytes random numbers. Then, if necessary, adjust total random
     *  number to random_len size */
    if( i < (tmp_random_len/unit_random_len -1) )
    {
       memcpy(random_ptr, &tmp_iv, unit_random_len);
       random_ptr += unit_random_len;
    }
    else 
    {
      if ( (random_len%unit_random_len) !=0)
      {
        memcpy(random_ptr, &tmp_iv, random_len%unit_random_len);
      }
      else
      {
        memcpy(random_ptr, &tmp_iv, unit_random_len);
      }
    }
  }
  return ret_val;
}

/**
 * @brief This function de-initializes PRNG Engine.
 *
 * @param None
 * @param None
 *
 * @return None
 *
 * @see PrngCL_init
 *
 */
PrngCL_Result_Type PrngCL_deinit(void)
{
   PrngCL_Result_Type stat = PRNGCL_ERROR_NONE;
   /* Disable clock for random number generator */
   stat = (PrngCL_Result_Type) PrngEL_ClkDisable();

   return stat;
}



/**
 * @brief    This function initializes PRNG Engine. Uses direct register write/reads  
 *
 * @param None
 *
 * @return PrngCL_Resut_Type
 *
 * @see PrngCL_lite_init
 *
 */
PrngCL_Result_Type PrngCL_lite_init(void)
{
  PrngCL_Result_Type stat = PRNGCL_ERROR_NONE;

  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 1);
  while (HWIO_INF(GCC_PRNG_AHB_CBCR, CLK_OFF)) {}

  if(g_prng_config_enable) // TZ will configure PRNG
  {
    HWIO_OUT(SEC_PRNG_LFSR_CFG,
         (0x5                                                                                 | //Set RING_OSC0_CFG to 101, which is feedback point 1
         (1 << HWIO_SHFT(SEC_PRNG_LFSR_CFG, SEC_LFSR0_EN)) | //Enable LFSR0_EN
         0x50                                                                                 | //Set RING_OSC1_CFG to 101, which is feedback point 1 
         (1 << HWIO_SHFT(SEC_PRNG_LFSR_CFG, SEC_LFSR1_EN)) | //Enable LFSR1_EN
         0x500                                                                                | //Set RING_OSC2_CFG to 101, which is feedback point 1
         (1 << HWIO_SHFT(SEC_PRNG_LFSR_CFG, SEC_LFSR2_EN)) | //Enable LFSR2_EN
         0x5000                                                                               | //Set RING_OSC3_CFG to 101, which is feedback point 1
         (1 << HWIO_SHFT(SEC_PRNG_LFSR_CFG, SEC_LFSR3_EN))    //Enable LFSR3_EN
         ));

    HWIO_OUT(SEC_PRNG_CONFIG,
        ((1 << HWIO_SHFT(SEC_PRNG_CONFIG, SEC_PRNG_EN)) ));
  }

  return stat;
}

/**
 * @brief This function de-initializes PRNG Engine. Uses direct register write/reads
 *
 * @param None
 * @param None
 *
 * @return None
 *
 * @see PrngCL_lite_deinit
 *
 */
PrngCL_Result_Type PrngCL_lite_deinit(void)
{
  PrngCL_Result_Type stat = PRNGCL_ERROR_NONE;

  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 0);

  return stat;
}
