/*
===========================================================================

FILE:         ddr_external_api.c

DESCRIPTION:  
  This is external APIs to support usage of SCALe sequences

  Copyright 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 
===========================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/msm8952/ddr_external_api.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/09/14   aus     Modified DDR frequencies
05/22/14   aus     Fixed the low BIMC frequency
05/14/14   aus     Initial version.
================================================================================
*/

/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "seq_hwio.h"
#include "busywait.h"
#include "boot_extern_clk_interface.h"
#include "ddr_target.h"
#include "DDIChipInfo.h"
#include "ddr_params.h"

/*==============================================================================
                                  MACROS
==============================================================================*/
/* Macro for round-up division */
#define div_ceil(n, d)  (((n) + (d) - 1) / (d))

/*==============================================================================
                                  DATA
==============================================================================*/

/*==============================================================================
                                  ASSEMBLY
==============================================================================*/
/* inline assembly to do burst write */
__asm void __blocksCopy(volatile uint32 *source, volatile uint32 *destination, uint32 num_words)
{
                        MOVS    r3,r2, LSR #3           ; Number of eight word multiples.
                        BEQ     readwords               ; Less than eight words to move?
                        STMFD   sp!, {r4-r11}           ; Save some working registers.
octread                 ;LDMIA   r0, {r4-r11}            ; Load 8 words from the source
                        LDMIA   r0!, {r4-r11}
                        STMIA   r1!, {r4-r11}           ; and put them at the destination.
                        SUBS    r3, r3, #1              ; Decrement the counter.
                        BNE     octread                 ; ... copy more.
                        LDMFD   sp!, {r4-r11}           ; Dont need these now - restore

readwords               ANDS    r2, r2, #7              ; Number of odd words to copy.
                        BEQ     ret                     ; No words left to copy?

copy                    STMFD   sp!, {r4-r10}           ; Save some working registers.
                        cmp     r2,#1
                        beq     oneword                 ; write the remaining one word
                        cmp     r2,#2
                        beq     twowords                ; do a burst of two words
                        cmp     r2,#3
                        beq     threewords              ; do a burst of three words
                        cmp     r2,#4
                        beq     fourwords               ; do a burst of four words
                        cmp     r2,#5
                        beq     fivewords               ; do a burst of five words
                        cmp     r2,#6
                        beq     sixwords                ; do a burst of six words
                        cmp     r2,#7
                        beq     sevenwords              ; do a burst of seven words
                        b       stop

oneword                 ldr     r3, [r0]
                        str     r3, [r1]
                        b       stop

twowords                ldmia   r0!,{r4-r5}
                        stmia   r1!,{r4-r5}
                        b       stop

threewords              ldmia   r0!,{r4-r6}
                        stmia   r1!,{r4-r6}
                        b       stop

fourwords               ldmia   r0!,{r4-r7}
                        stmia   r1!,{r4-r7}
                        b       stop

fivewords               ldmia   r0!,{r4-r8}
                        stmia   r1!,{r4-r8}
                        b       stop

sixwords                ldmia   r0!,{r4-r9}
                        stmia   r1!,{r4-r9}
                        b       stop

sevenwords              ldmia   r0!,{r4-r10}
                        stmia   r1!,{r4-r10}

stop                    LDMFD   sp!, {r4-r10}           ; restore r4 to r11
ret                     BX      lr                      ; Return.
}

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* ============================================================================
**  Function : HAL_SDRAM_Get_Training_Address
** ============================================================================
*/
/*!
*   @brief
*   This helper function finds the appropriate address for ddr to do training on.
*
*   @details
*   This helper function finds the appropriate address for ddr to do training on.
*   Taking into account for interleave status, interface, and chip select
*
*   @param interface_name              -  [IN] Interface to train on
*   @param chip_select                 -  [IN] chip select to train on

*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

static uint32 HAL_SDRAM_Get_Training_Address(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select)
{
  uint32 test_address = 0;
#if 1  
	ddr_info ddr_configuration = ddr_get_info();
  
	if (chip_select == SDRAM_CS0)
	{
	  test_address = ddr_configuration.ddr_size.sdram0_cs0_addr;
	}
	else if (chip_select == SDRAM_CS1)
	{
	  test_address = ddr_configuration.ddr_size.sdram0_cs1_addr;	
	}
#endif
//  test_address += 0x80000000;
  return test_address;
};

/* ============================================================================
**  Function : ddr_write
** ============================================================================
*/
/*!
*   @brief
*   This function does single transaction writes
*
*   @details
*   This function does single transaction writes
*
*   @param interface_name             -  [IN] interface to set pattern for
*   @param chip_select                -  [IN] chip select to set pattern for
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void ddr_write(volatile uint32 *address, uint32 * pattern, uint32 size)
{
  uint32 pattern_offset;
  
    /* Memory controller writes data pattern into DRAM */
    for(pattern_offset = 0; pattern_offset <  size; pattern_offset++)
    {
      address[pattern_offset] = pattern[pattern_offset];
    }
}


/* ============================================================================
**  Function : ddr_mem_write_low_speed
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to perform a write at low speed
*   
*   @details
*   This api will call the relevent clock function to scale down the clock frequency
*   so we can have a guaranteed write, then scale back up to the previous speed.
*   
*   @param 
*   @param chnl             -  [IN] channel to set pattern for
*   @param rank             -  [IN] rank to set pattern for
*   @param pattern          -  [IN] array of pattern to set
*   @param size             -  [IN] size of pattern array (in terms of number 
*                                    of elements
*   @param burst            -  [IN] burst mode on\off
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_mem_write_low_speed(uint32 chnl, uint32 rank, uint32 * pattern, uint32 size, uint32 burst)
{
  volatile uint32 * address = (uint32 *)HAL_SDRAM_Get_Training_Address((SDRAM_INTERFACE)chnl, (SDRAM_CHIPSELECT)rank);
  struct ddr_device_params_common *ddr_params_ch0;

  ddr_params_ch0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
  
  /* Clock API to scale to low speed */
  ddr_pre_clock_switch(0, 331200, SDRAM_INTERFACE_0);
  
  boot_clock_set_bimcspeed(165600);

  ddr_post_clock_switch(0, 331200, SDRAM_INTERFACE_0);


  if(burst)
  {
    /* perform a burst write */
    __blocksCopy(pattern, address, size);
  }
  else
  {
    ddr_write(address, pattern, size);
  }

  if(ddr_params_ch0->device_type == DDR_TYPE_LPDDR3){
  
  	/* clock api to restore to high speed */
	  ddr_pre_clock_switch(0, 796800, SDRAM_INTERFACE_0);
  
	  boot_clock_set_bimcspeed(398400);

	  ddr_post_clock_switch(0, 796800, SDRAM_INTERFACE_0);

  }
}

/* ============================================================================
**  Function : ddr_mem_write_high_speed
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to perform a write at high speed
*   
*   @details
*   This api will make the assumption that ddr is already at high speed and 
*   just does a burst\non-burst write based on input parameter
*   
*   @param 
*   @param chnl             -  [IN] channel to set pattern for
*   @param rank             -  [IN] rank to set pattern for
*   @param pattern          -  [IN] array of pattern to set
*   @param size             -  [IN] size of pattern array (in terms of number 
*                                    of elements
*   @param burst            -  [IN] burst mode on\off
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_mem_write_high_speed(uint32 chnl, uint32 rank, uint32 * pattern, uint32 size, uint32 burst)
{
  volatile uint32 * address = (uint32 *)HAL_SDRAM_Get_Training_Address((SDRAM_INTERFACE)chnl, (SDRAM_CHIPSELECT)rank);
  
  if(burst)
  {
    /* perform a burst write */
    __blocksCopy(pattern, address, size);
  }
  else
  {
    ddr_write(address, pattern, size);
  }
}


void seq_wait(uint32 time_value, SEQ_TimeUnit time_unit)
{
  if(time_unit == SEC)
  {
     busywait(time_value * 1000 * 1000);
  }
  else if(time_unit == MS)
  {
     busywait(time_value * 1000);
  }
  else if(time_unit == US)
  {
     busywait(time_value);
  }
  else
  {
    /* time_unit == NS */
     busywait(time_value / 1000);
  }
 
}

void vv_msg(uint32 info, uint32 func, char string[256])
{}

chip_platform_info  Ddr_get_chip_platform_info(void )
{
   chip_platform_info cpi;
   DalDeviceHandle *hChipinfo;
   DalChipInfoIdType chip_id;
   DalChipInfoVersionType version;
   
   DALResult ret;

   ret = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipinfo);
   if((ret == DAL_SUCCESS) && (hChipinfo != NULL)) {
       DalChipInfo_GetChipId(hChipinfo, &chip_id);
       DalChipInfo_GetChipVersion(hChipinfo, &version);
   }

   cpi.chip_id = (DdrChipInfoIdType)chip_id;
   cpi.version = version ;

   return cpi;
}


/* ============================================================================
**  Function : ddr_mem_read
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to perform a read
*   
*   @details
*   This api will fill the input data array with the size specified
*   
*   @param 
*   @param chnl             -  [IN] channel to set pattern for
*   @param rank             -  [IN] rank to set pattern for
*   @param data             -  [IN/OUT] array of data to read to
*   @param size             -  [IN] size of pattern array (in terms of number 
*                                    of elements
*   @param burst            -  [IN] burst mode on\off
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_mem_read( uint32 chnl, uint32 rank, uint32 * data, uint32 size, uint32 burst)
{
  volatile uint32 * address = (uint32 *)HAL_SDRAM_Get_Training_Address((SDRAM_INTERFACE)chnl, (SDRAM_CHIPSELECT)rank);
  uint32 i;

  for(i = 0; i < size; i++)
  {
    data[i] = address[i];
  }

}


