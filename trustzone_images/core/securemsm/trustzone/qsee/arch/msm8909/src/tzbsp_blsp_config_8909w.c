/*===========================================================================
  Copyright (c) 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  when       who     what, where, why
  --------   ---     ------------------------------------------------------------
  04/06/16   dpk     Created for 8909
  =============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


#include "tzbsp_blsp.h"
#include "tzbsp_xpu_def.h"
#include "tzbsp_xpu.h"


/* This structure contains QUP/UART information to be assigned to a subsystem 
 * at cold boot stage, according to the use case.
 * 
 * Note: For GPIO index value, the customer must contact Buses team before 
 * using. The unused indices must be to 0xFFFFFFFF.
 */
 
/*
Information Format::
TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL, PERSISTENCE, GPIO_PROTECTION_TYPE, 
                    PERIPHERAL_ID, PERIPHERAL_RG_INDEX, BLSP_XPU_ID, 
                    GPIO_1, GPIO_2, GPIO_3, GPIO_4, GPIO_5, GPIO_6, NUM_GPIOS, 
					GPIO_INDEX_1, GPIO_INDEX_2, GPIO_INDEX_3, SUBSYSTEM_ID, IS_PERIPHERAL_TZ_USED)
*/ 
TzBsp_Blsp_Peripheral_Info  TzBspBlspPeripheralInfo[] = 
{
   /* QUP 1 */
   TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL_I2C, NON_PERSISTENT, PROTECTION_TYPE_MPU, 
                       BLSP_QUP_1, TZBSP_BAM_BLSP1_RG_QUP0, HAL_XPU2_BAM_BLSP1_DMA,
                       6,7,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,2,
                       0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,TZBSP_NONE_ID,FALSE),

   /* QUP 2 */ /* NFC */
   TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL_I2C, NON_PERSISTENT, PROTECTION_TYPE_MPU, 
                       BLSP_QUP_2, TZBSP_BAM_BLSP1_RG_QUP1, HAL_XPU2_BAM_BLSP1_DMA,
                       111,112,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,2,
                       0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,TZBSP_APSS_ID,FALSE),

   /* QUP 3 */
   TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL_SPI, NON_PERSISTENT, PROTECTION_TYPE_MPU, 
                       BLSP_QUP_3, TZBSP_BAM_BLSP1_RG_QUP2, HAL_XPU2_BAM_BLSP1_DMA,
                       0,1,2,3,0xFFFFFFFF,0xFFFFFFFF,4,
                       0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,TZBSP_NONE_ID,FALSE),

   /* QUP 4 */
   TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL_I2C, NON_PERSISTENT, PROTECTION_TYPE_MPU, 
                       BLSP_QUP_4, TZBSP_BAM_BLSP1_RG_QUP3, HAL_XPU2_BAM_BLSP1_DMA,
                       14,15,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,2,
                       0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,TZBSP_APSS_ID,FALSE),

   /* QUP 5 */
   TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL_I2C, NON_PERSISTENT, PROTECTION_TYPE_MPU, 
                       BLSP_QUP_5, TZBSP_BAM_BLSP1_RG_QUP4, HAL_XPU2_BAM_BLSP1_DMA,
                       18,19,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,2,
                       0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,TZBSP_APSS_ID,FALSE),

   /* QUP 6 */ 
   TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL_SPI, NON_PERSISTENT, PROTECTION_TYPE_MPU, 
                       BLSP_QUP_6, TZBSP_BAM_BLSP1_RG_QUP5, HAL_XPU2_BAM_BLSP1_DMA,
                       8,9,10,11,0xFFFFFFFF,0xFFFFFFFF,4,
                       15,0xFFFFFFFF,0xFFFFFFFF,TZBSP_APSS_ID,FALSE),   

   /* UART 1 */
   TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL_UART_2_LINE, NON_PERSISTENT, PROTECTION_TYPE_MPU, 
                       BLSP_UART_1, TZBSP_BAM_BLSP1_RG_UART0, HAL_XPU2_BAM_BLSP1_DMA,
                       4,5,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,2,
                       0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,TZBSP_APSS_ID,FALSE),

   /* UART 2 */
   TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL_UART_2_LINE, NON_PERSISTENT, PROTECTION_TYPE_MPU, 
                       BLSP_UART_2, TZBSP_BAM_BLSP1_RG_UART1, HAL_XPU2_BAM_BLSP1_DMA,
                       20,21,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,2,
                       0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,TZBSP_APSS_ID,FALSE),
};

uint32 TzBspBlspConfigArrLen = sizeof(TzBspBlspPeripheralInfo)/sizeof(TzBsp_Blsp_Peripheral_Info);


