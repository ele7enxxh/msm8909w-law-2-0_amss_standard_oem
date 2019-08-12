/*===========================================================================

 Flash Programming Plug-In for Lauterbach JTAG/ICD TRACE32

GENERAL DESCRIPTION

This program is essentially a plug-in for Lauterbach JTAG ICD TRACE32
  Debugger FLASH command. The programming can be achieved in two different 
  modes: emulator controlled or target controlled. The emulator controlled
  programming needs no program running on the target, but is slow
  (typically about 1 to 5 KBytes/sec). The target controlled programming 
  requires a user designed FLASH programming routine (and hence this plug-in)
  to be available on the target. This plug-in is called by TRACE32 with 
  appropriate parameters to program FLASH devices. The advantage of this
  method is the higher programming speed and more flexibility in the 
  implementation of the programming algorithm.

  To use the plug-in, FLASH command has the FLASH.TARGET command.
  For example: (this assumes the programming voltage is setup beforehand)

    ; reset our flash command
    flash.reset
    ; let flash command know where our routine and buffer is located
    ; code at IMEM 0x80000000, data at 0x80010000, buffer size = 128K
    flash.target 80000000 80010000 4000 
    ; define our flash range and tell flash command we're using
    ; our own flash programming routine via keyword "target"
    flash.create 0--0FFFFF target word
    ; first erase our flash
    flash.erase 0--0FFFFF

    ; put into program mode and do it
    flash.program all
    ; the actual programming of FLASH, this will call our plug-in
    d.load.aif "surf.aif" 
    ; ok we're finished, turn off programming mode
    flash.program off

  In order for the Debugger to get control back after the plug-in completes,  
  a breakpoint is assumed to be in place after the last instruction
  executed in the plug-in.

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

 Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/jtagprogrammer/emmc/src/jtagprogrammer_main.c#1 $ 
  $DateTime: 2015/09/01 00:30:35 $ 
  $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
2012-04-12   ah      Added enable_hw_reset() feature
2011-12-02   ah      More feedback for error messages
2011-08-25   ah      More feedback on active SD slots when INFO is requested 
2011-07-27   ab      Add ERASEALL option which will zero-out the boot partitions
2011-06-13   ab      Do not convert sector offset to bytes, to prevent uint32 overflow
2011-06-06   ab      Changed CRC computation to non-lookup table based
2011-02-03   ab      Added parsing patch xml to apply patches directly to card
2011-01-25   ab      Added parsing of rawprogram xml to write files or erase partitions
2010-02-18   rh      Adding RX signal quality test
2010-02-01   rh      Write all MBR partitions into a single write protect area
2010-01-20   rh      Adding function to set active boot partition
2010-01-11   rh      New function to cluster EBR into the same write protect area
2009-12-10   rh      Increase the number of EBR parsed to 14
2009-07-20   rh      Fix: 1st sector in EBR is not relocated properly
2009-04-21   rh      Modified to use with mainline SDCC driver
2008-06-18   vtw     Initial version, ported from mjnand.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "jtagprogrammer_param.h"
#include "sdcc_api.h"
#include "jtagprogrammer_bsp.h"

/*===========================================================================

                           DEFINITIONS

===========================================================================*/

extern byte* Image$$APP_RAM_ZI$$ZI$$Base;
extern byte* Image$$APP_RAM_ZI$$ZI$$Limit;

/* Enumerated type to see if device probed OK so we only probe once */
typedef enum
{
  MEM_DEVICE_NOT_PROBED,    /* Not yet probed               */
  MEM_DEVICE_FOUND,         /* Successful probe             */
  MEM_DEVICE_UNKNOWN        /* Failed to probe successfully */
}
probe_code_type;

/* This structure is used to communicate between ourselves and TRACE32. */
static struct struct_jtag_prog_param jtag_prog_param;

/* Output buffer */
static char output_buffer[JSDCC_OUTPUT_BUFFER_SIZE];
static uint32 output_buffer_length;

/* enum to indicate status of probe */
static probe_code_type probe_status = MEM_DEVICE_NOT_PROBED;

/* These variables are used to redirect the programming routine to partition 
 * aware programming routine */
static uint32 redirect_offset = 0;
static uint32 redirect_partition = 0;
static uint32 card_slot_in_use = 0;
static uint32 sector_writing = 0;
static sdcc_mem_info_type current_mem_info;

/* SDCC device handle */
#define MAX_SDCC_PARTITIONS 7
struct sdcc_device *hsdev_user;           /* User partition handle */
struct sdcc_device *hsdev_handles[MAX_SDCC_PARTITIONS+1];
struct sdcc_device *hsdev_current;        /* Current operating partition handle */

char *sdcc_err_string[] = {
    "SDCC_NO_ERROR",
    "SDCC_ERR_UNKNOWN",
    "SDCC_ERR_CMD_TIMEOUT",
    "SDCC_ERR_TIMEOUT",
    "SDCC_ERR_CMD_CRC_FAIL",
    "SDCC_ERR_DATA_CRC_FAIL",
    "SDCC_ERR_CMD_SENT",
    "SDCC_ERR_PROG_DONE",
    "SDCC_ERR_CARD_READY",
    "SDCC_ERR_INVALID_TX_STATE",
    "SDCC_ERR_SET_BLKSZ",
    "SDCC_ERR_SDIO_R5_RESP",
    "SDCC_ERR_DMA",
    "SDCC_ERR_READ_FIFO",
    "SDCC_ERR_WRITE_FIFO",
    "SDCC_ERR_ERASE",
    "SDCC_ERR_SDIO",
    "SDCC_ERR_SDIO_READ",
    "SDCC_ERR_SDIO_WRITE",
    "SDCC_ERR_SWITCH",
    "SDCC_ERR_INVALID_PARAM",
    "SDCC_ERR_CARD_UNDETECTED",
    "SDCC_ERR_FEATURE_UNSUPPORTED",
    "SDCC_ERR_SECURE_COMMAND_IN_PROGRESS",
    "SDCC_ERR_READ_SEC_CMD_NOT_ALLOWED",
    "SDCC_ERR_ABORT_READ_SEC_CMD",
    "SDCC_ERR_CARD_INIT",
    "SDCC_ERR_CARD_REMOVED",
    "SDCC_ERR_PWR_ON_WRITE_PROT",
    "SDCC_ERR_WP_VIOLATION",
    "SDCC_ERR_SPS_MODE_USED",
    "SDCC_ERR_DML_INIT",
    "SDCC_ERR_SPS_GET_EVENT",
    "SDCC_ERR_SPS_WRITING_DESCRIPTOR"
};

uint32 BLOCK_SIZE = 4096;
#define SIZE_IN_BLOCKS(x)         ( ((x % BLOCK_SIZE) == 0) ? x/BLOCK_SIZE : x/BLOCK_SIZE+1 )

/* Calculate a CRC value for the given data */
static int reflect(int data, const uint32 len);
static uint32 calc_crc32(const uint8 *data_in, const uint32 nbytes_in);

/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/
uint32 do_probe(void);
uint32 jsdcc_mem_read(void);
uint32 jsdcc_mem_write(void);
uint32 jsdcc_mem_erase(void);
uint32 jsdcc_mem_compute_crc(void);
uint32 jsdcc_mem_print_info(void);
uint32 jsdcc_mem_program(void);
uint32 jsdcc_mem_chip_erase(void);
uint32 jsdcc_mem_force_erase(void);
uint32 jsdcc_enable_hw_reset(void);
uint32 jsdcc_mem_set_partition(void);
uint32 jsdcc_mem_set_hw_partition(void);
uint32 jsdcc_mem_select_card_slot(void);
uint32 jsdcc_mem_set_active_boot(void);
uint32 jsdcc_debug_frequency_scan(void);
uint32 jsdcc_debug_start_rx_test(void);
uint32 jsdcc_gpp_program(void);

void outputMessage(const char* format, ...) {
    int retval;
    char *buffer = &output_buffer[output_buffer_length];
    int buffer_space = sizeof(output_buffer) - output_buffer_length;

    va_list args;
    va_start (args, format);

    retval = vsnprintf(buffer,
                       buffer_space,
                       format,
                       args);
    va_end (args);
    output_buffer_length += MIN(buffer_space, retval);
}

/*===========================================================================

DESCRIPTION
  This function calls the appropriate routine based on what TRACE32 told us.

DEPENDENCIES
   Assumes jtag_flash_param (global variable) has valid parameters.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void main_c(void)
{
   uint32 ret_code = STATUS_FAIL;
   output_buffer_length = 0;

   /* Call probe function.  It will only call actual probe function once */
   /* Skip probe function for RX test because it does not require the card
    * to be initialized */
   if (probe_status == MEM_DEVICE_NOT_PROBED &&
       jtag_prog_param.cmd_status != JTAG_CMD_DEBUG_RX_TEST)
   {
      ret_code = do_probe();
      if (ret_code != STATUS_SUCCESS)
        outputMessage("ERROR: MMC device not found \n");
   }

   switch ( jtag_prog_param.cmd_status )
   {
      case JTAG_CMD_PROGRAM:
         outputMessage("Executing PROGRAM\n");
         ret_code = jsdcc_mem_program();
         break;
         
      case JTAG_CMD_WRITE_SECTOR:
         outputMessage("Executing WRITE_SECTOR\n");
         ret_code = jsdcc_mem_write();
         break;

      case JTAG_CMD_READ_SECTOR:
         outputMessage("Executing READ_SECTOR\n");
         ret_code = jsdcc_mem_read();
         break;
         
      case JTAG_CMD_ERASE_GENERIC:
         outputMessage("Executing ERASE_SECTOR\n");
         ret_code = jsdcc_mem_erase();
         break;

      case JTAG_CMD_COMPUTE_CRC:
         outputMessage("Executing CRC Compute\n");
         ret_code = jsdcc_mem_compute_crc();
         break;

      case JTAG_CMD_PRINT_INFO:
         outputMessage("Executing PRINT_INFO MAN\n");
         ret_code = jsdcc_mem_print_info();
         break;

      case JTAG_CMD_CHIP_ERASE:
         outputMessage("Executing CHIP_ERASE\n");
         ret_code = jsdcc_mem_chip_erase();
         break;

      case JTAG_CMD_SET_PARTITION:
         outputMessage("Executing SET_PARTITION\n");
         ret_code = jsdcc_mem_set_partition();
         break;

      case JTAG_CMD_SET_HW_PARTITION:
         outputMessage("Executing SET_HW_PARTITION\n");
         ret_code = jsdcc_mem_set_hw_partition();
         break;

      case JTAG_CMD_SET_ACTIVE_BOOT:
         outputMessage("Executing SET_ACTIVE_BOOT\n");
         ret_code = jsdcc_mem_set_active_boot();
         break;

      case JTAG_CMD_DEBUG_FREQ_SCAN:
         outputMessage("Executing DEBUG_FREQ_SCAN\n");
         ret_code = jsdcc_debug_frequency_scan();
         break;

      case JTAG_CMD_DEBUG_RX_TEST:
         outputMessage("Executing DEBUG_RX_TEST\n");
         ret_code = jsdcc_debug_start_rx_test();
         break;

      case JTAG_CMD_FORCE_ERASE:
         outputMessage("Executing FORCE ERASE\n");
         ret_code = jsdcc_mem_force_erase();
         break;

      case JTAG_CMD_PROG_GPP:
         outputMessage("Executing PROGRAM GPP\n");
         ret_code = jsdcc_gpp_program();
         break;

      case JTAG_CMD_ENABLE_HW_RESET:
         outputMessage("Executing HW reset\n");
         ret_code = jsdcc_enable_hw_reset();
         break;

      default:
         ret_code = STATUS_FAIL;
         outputMessage ("ERROR: Invalid SDCC command 0x%x\n", jtag_prog_param.cmd_status);
         break;
   }
   /* return error code back to TRACE32, 0 = success, 0x100 = fail */
   jtag_prog_param.cmd_status = ret_code;

} /* main_c */

void jsdcc_init(void)
{
    byte *data;
    for(data = (byte *) &Image$$APP_RAM_ZI$$ZI$$Base; data != (byte *) &Image$$APP_RAM_ZI$$ZI$$Limit; data++)
        *data = 0;

    while (1)
    {
        main_c();
    }
}

/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real flash probe routine. 
  It does some checking to ensure not calling multiple times and 
  calls the real probe function.

DEPENDENCIES
  none

RETURN VALUE
  If success, return FLASH_SUCCESS else FLASH_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 do_probe(void)
{
   uint32 ret_code = STATUS_FAIL;
   //uint32 card_slot = 0;
   unsigned int i;

   outputMessage("\n\ndo_probe(card_slot_in_use=%d)\n",card_slot_in_use);

   /* Kill the watchdog so it won't bark */
   jsdcc_bsp_kill_watchdog();
   
   /* Only probe device if it has not yet been probed. */
   switch (probe_status)
   {
      case MEM_DEVICE_NOT_PROBED:
         /* Configure the clock if is required */
         jsdcc_bsp_tool_config_pll();   
         /* Enable the PMIC SSBI bus clock */
         jsdcc_bsp_tool_enable_pmic_ssbi_clk();
         /* Before issue open command, make sure SSBI clock is on */

        /*
        while(card_slot<=3)							// let's show what we find neatly here
        {
            outputMessage("Slot %d ---> ",card_slot);
            hsdev_user  = sdcc_handle_open(card_slot, 0);

            if(hsdev_user != NULL)
                outputMessage("responded (SD/MMC device exists).\n");
            else
                outputMessage("not present\n");

            card_slot++;
        }
        */

        outputMessage("\nUsing 'user specified' card slot %d\n",card_slot_in_use);

         for (i = 0; i <= MAX_SDCC_PARTITIONS; i++)
         {
            if (i==3) /* Skip opening RPMB partition */
                hsdev_handles[i] = NULL;
            else
                hsdev_handles[i] = sdcc_handle_open(card_slot_in_use, i);
         }
         hsdev_user = hsdev_handles[0];
         if(hsdev_user != NULL)
         {
            ret_code = STATUS_SUCCESS;
            probe_status = MEM_DEVICE_FOUND;
         }
         else
         {
            ret_code = STATUS_FAIL;
            probe_status = MEM_DEVICE_UNKNOWN;
            outputMessage("Warning: Unable to open USER partition.\n");	// can't find boot partitions OR this (people ask about this)
            outputMessage("Possible causes: card not present? No power or clock to the card?\n");
         }

         hsdev_current = hsdev_user;
         break;
      case MEM_DEVICE_UNKNOWN:
		 outputMessage("\nMEM_DEVICE_UNKNOWN\n");
         ret_code = STATUS_FAIL;
         break;
      case MEM_DEVICE_FOUND:
		 outputMessage("\nMEM_DEVICE_FOUND\n");
         ret_code = STATUS_SUCCESS;
         break;
      default:
		 outputMessage("\nMEM_DEVICE_DEFAULT_STATUS_FAIL\n");
         ret_code = STATUS_FAIL;
         break;
  }

  return ret_code;
}  /* do_probe */


/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real memory read routine. It extracts
  the required parameters from TRACE32 and passes it onto the real thing.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_read(void)
{

   byte *pdata;
   SDCC_STATUS rc;

   pdata = jtag_prog_param.data;
   rc = sdcc_handle_read(hsdev_current, jtag_prog_param.addr, 
                         pdata, jtag_prog_param.data_length);

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("SDCC READ ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }
   return STATUS_SUCCESS;
}  /* jsdcc_mem_read */


/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real memory write routine. It extracts
  the required parameters from TRACE32 and passes it onto the real thing.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_write(void)
{

   byte *pdata;
   SDCC_STATUS rc;

   outputMessage("Sector Write begins: \n");
   outputMessage("Addr %x, Size %x, Offset %x \n", 
            jtag_prog_param.addr, jtag_prog_param.data_length, 
            jtag_prog_param.mod_addr);
   
   pdata = jtag_prog_param.data;
   rc = sdcc_handle_write(hsdev_current, jtag_prog_param.addr, 
                         pdata, jtag_prog_param.data_length);

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("SDCC WRITE ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }
   return STATUS_SUCCESS;

}  /* jsdcc_mem_write */


/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real memory program routine. It extracts
  the required parameters from TRACE32 and passes it onto the real thing.  
  Program is very similar to write, except the length of the data is divided
  by BLOCK_SIZE.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_program(void)
{

   byte *pdata;
   SDCC_STATUS rc;

   if(sector_writing) {
      jtag_prog_param.addr = redirect_offset + SIZE_IN_BLOCKS(jtag_prog_param.addr);
      jtag_prog_param.data_length = jtag_prog_param.data_length < redirect_partition ? jtag_prog_param.data_length : redirect_partition;
      if (jtag_prog_param.data_length == 0) {
        return STATUS_SUCCESS;
      }
      redirect_partition -= jtag_prog_param.data_length;
   }

   /* Print out debugging information */
   /*
   outputMessage("Addr %x, Size %x, Offset %x \n", 
            jtag_prog_param.addr, jtag_prog_param.data_length, 
            jtag_prog_param.offset);*/

   pdata = jtag_prog_param.data;
   rc = sdcc_handle_write(hsdev_current, jtag_prog_param.addr, 
                         pdata, SIZE_IN_BLOCKS(jtag_prog_param.data_length));

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("SDCC WRITE ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }
   return STATUS_SUCCESS;
}  /* jsdcc_mem_program */


/*===========================================================================

DESCRIPTION
  This function calls the mem_get_device_info API to get the information of 
  the memory device attached.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_print_info(void)
{
   SDCC_STATUS rc;

   if (do_probe() != STATUS_SUCCESS)
   {
     outputMessage("ERROR: MMC device not found \n");
     return STATUS_FAIL;
   }


   outputMessage("\n==============================\n");
   outputMessage("=====       INFO         =====\n");
   outputMessage("==============================\n");

   if(hsdev_current == NULL)
   {
	 outputMessage("\nERROR: No eMMC device detected on that slot\n");
     return STATUS_FAIL;
   }

   rc = sdcc_handle_mem_get_device_info(hsdev_current, &current_mem_info);

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("SDCC GET INFO ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }

   /* Format and print out the memory information */
   switch(current_mem_info.card_type)
   {
      case SDCC_CARD_UNKNOWN:
         outputMessage("MEMORY TYPE: UNKNOWN\n");
         break;
      case SDCC_CARD_SD:      
         outputMessage("MEMORY TYPE: SD\n");
         break;
      case SDCC_CARD_MMC:     
         outputMessage("MEMORY TYPE: MMC\n");
         break;
      case SDCC_CARD_COMB:    
         outputMessage("MEMORY TYPE: COMB\n");
         break;
      case SDCC_CARD_SDIO:   
         outputMessage("MEMORY TYPE: SDIO\n");
         break;
      case SDCC_CARD_SDHC:    
         outputMessage("MEMORY TYPE: SDHC\n");
         break;
      case SDCC_CARD_MMCHC:   
         outputMessage("MEMORY TYPE: MMCHC\n");
         break;
   }

   outputMessage("Size (sectors): %d\n", current_mem_info.card_size_in_sectors);
   outputMessage("Block Length (bytes): %d\n", current_mem_info.block_len);
   outputMessage("Size (kB): %d\n", current_mem_info.card_size_in_sectors / 1024 * current_mem_info.block_len);
   if(current_mem_info.card_type == SDCC_CARD_SDHC)
   {
      outputMessage("Speed Class: %d\n", 
               current_mem_info.speed_class.sd_speed_class == SDCC_SD_SPEED_CLASS_0 ? 0 :
               current_mem_info.speed_class.sd_speed_class == SDCC_SD_SPEED_CLASS_2 ? 2 :
               current_mem_info.speed_class.sd_speed_class == SDCC_SD_SPEED_CLASS_4 ? 4 :
               current_mem_info.speed_class.sd_speed_class == SDCC_SD_SPEED_CLASS_6 ? 6 :
               0
              );
   }
   else if(current_mem_info.card_type == SDCC_CARD_MMCHC)
   {
      outputMessage("Speed Class: %c\n", 
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_LOWEST ? '0' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_A ? 'A' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_B ? 'B' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_C ? 'C' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_D ? 'D' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_E ? 'E' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_F ? 'F' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_G ? 'G' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_H ? 'H' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_J ? 'J' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_K ? 'K' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_M ? 'M' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_O ? 'O' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_R ? 'R' :
               current_mem_info.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_T ? 'T' :
               'x'
              );
   }

   outputMessage("Reliable write sector count: %d\n", current_mem_info.reliable_write_sector_count);
   outputMessage("Physical partition count: %d\n", current_mem_info.num_phy_partition_created);
   outputMessage("Manufacture ID: %x\n", current_mem_info.mfr_id);
   outputMessage("OEM/Application ID: %x\n", current_mem_info.oem_id);
   outputMessage("Product Name: %c%c%c%c%c%c\n", current_mem_info.prod_name[0], 
                                            current_mem_info.prod_name[1],
                                            current_mem_info.prod_name[2],
                                            current_mem_info.prod_name[3],
                                            current_mem_info.prod_name[4],
                                            current_mem_info.prod_name[5]
                                            );
   outputMessage("Product revision: %x\n", current_mem_info.prod_rev);
   outputMessage("Product serial #: %x\n", current_mem_info.prod_serial_num);
   current_mem_info.mfr_date[7] = 0;
   outputMessage("Manufacturing Date: %s\n", current_mem_info.mfr_date);

   jtag_prog_param.data_length = current_mem_info.card_size_in_sectors;
   BLOCK_SIZE = 512;
   return STATUS_SUCCESS;
}  /* jsdcc_mem_print_info */

/*===========================================================================

DESCRIPTION
  This function erases a specified number of sectors from the card
  starting from the specified start sector

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_erase(void)
{
   SDCC_STATUS rc;
   byte *pdata;
   uint32 erase_data;
   
   while(jtag_prog_param.data_length > 0)
   {
       erase_data = jtag_prog_param.data_length < (PARAM_BUFFER_SIZE/BLOCK_SIZE) ? jtag_prog_param.data_length : (PARAM_BUFFER_SIZE/BLOCK_SIZE);

       /*rc = sdcc_handle_erase(hsdev_user, BLOCK_SIZE*jtag_prog_param.addr, jtag_prog_param.data_length);*/
       if (hsdev_current != NULL) {
         pdata = jtag_prog_param.data;
         memset(pdata, 0x00, BLOCK_SIZE*erase_data);
         rc = sdcc_handle_write(hsdev_current, jtag_prog_param.addr, pdata, erase_data);
       }
       else {
         outputMessage("No current handle to erase");
         return STATUS_FAIL;
       }
       if (rc != SDCC_NO_ERROR) 
       {
         outputMessage("SDCC ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
         return STATUS_FAIL;
       }
       jtag_prog_param.addr += erase_data;
       jtag_prog_param.data_length -= erase_data;
   }
   outputMessage("SDCC ERASE: Success! \n");
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION
  This function computes the CRC32 over a buffer

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_compute_crc(void)
{ 
   byte *pdata;
   SDCC_STATUS rc;

   uint32 sectors_to_read;
   sectors_to_read = jtag_prog_param.data_length / BLOCK_SIZE;
   if (jtag_prog_param.data_length % BLOCK_SIZE != 0)
        sectors_to_read++;

   pdata = jtag_prog_param.data;
   rc = sdcc_handle_read(hsdev_current, jtag_prog_param.addr, 
                        pdata, sectors_to_read);

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("SDCC READ ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }

   jtag_prog_param.data_length = calc_crc32((uint8 *)pdata, jtag_prog_param.data_length);

   outputMessage("CRC Compute: Success! \n");
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real memory erase routine.  
  The step to erase the entire memory device is to check the size
  of each physical partition and erase the partition by issue the 
  block erase command.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_chip_erase(void)
{
   SDCC_STATUS rc;
   sdcc_mem_info_type mem_info;

   rc = sdcc_handle_mem_get_device_info(hsdev_user, &mem_info);
   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("SDCC CHIP_ERASE ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }

   outputMessage("Issuing the eMMC ERASE command which erases the USER partition.\n");
   rc = sdcc_handle_erase(hsdev_user, 0, mem_info.card_size_in_sectors - 1);
   if (rc != SDCC_NO_ERROR) 
   {
      outputMessage("SDCC CHIP_ERASE ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
      return STATUS_FAIL;
   }

   outputMessage("SDCC CHIP_ERASE: Success! \n");
   return STATUS_SUCCESS;
}  /* jsdcc_mem_chip_erase */

/*===========================================================================

DESCRIPTION
  This function will erase the entire card contents. It also unlocks the password

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_force_erase(void)
{
   SDCC_STATUS rc;
   
   outputMessage("Performing Force Erase on the Card.\n");
   
   rc = sdcc_handle_force_erase(hsdev_user);
   if (rc == SDCC_ERR_FEATURE_UNSUPPORTED)
   {
      outputMessage("Force Erase failed as the Card inserted is unlocked\n");
      return STATUS_FAIL;
   }
   
   if (rc != SDCC_NO_ERROR)
   {
      outputMessage("Force Erase failed ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
      return STATUS_FAIL;
   }

   outputMessage("SDCC FORCE ERASE: Success! \n");
   return STATUS_SUCCESS;
}  /* jsdcc_mem_force_erase */

/*===========================================================================

DESCRIPTION
  This function enables the hardware reset function provided by the MMC

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_enable_hw_reset(void)
{
   SDCC_STATUS rc = SDCC_NO_ERROR;
   
   outputMessage("Performing Enable HW reset on the Card.\n");
   
   //rc = sdcc_enable_hw_reset(hsdev_user);
   if (rc == SDCC_ERR_FEATURE_UNSUPPORTED)
   {
      outputMessage("Enable HW reset is not supported on this card\n");
      return STATUS_FAIL;
   }

   if (rc != SDCC_NO_ERROR)
   {
      outputMessage("Enable HW reset failed 0x%X: %s\n", rc, sdcc_err_string[rc]);
      return STATUS_FAIL;
   }

   outputMessage("SDCC HW RESET: Success! \n");
   
   return STATUS_SUCCESS;
}  // jsdcc_enable_hw_reset


/*===========================================================================

DESCRIPTION
  This function set up the partition table for the memory device.  The 
  function assume the jtag_prog_param variable has been filled with the 
  partition table information.
 

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_set_partition(void)
{
   // TODO: remove this function and associated command
   return STATUS_SUCCESS;
}  /* jsdcc_mem_set_partition */

/*===========================================================================

DESCRIPTION
  This function is used to switch which HW partition that the current 
  operations are operating on.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_set_hw_partition(void)
{
   /* Not closing the old handle on purpose - Ensure the clock not getting turned off*/
   if (jtag_prog_param.addr > MAX_SDCC_PARTITIONS || jtag_prog_param.addr == 3) {
        outputMessage("Failed to switch partition.\n");
        if (jtag_prog_param.addr == 3)
            outputMessage("ERROR: We do not support switching to RPMB partition. Please consult SDCC driver team.\n");
        else if (jtag_prog_param.addr > MAX_SDCC_PARTITIONS)
            outputMessage("ERROR: You requested to switch to partition %d that doesn't exist.\nOnly %d partitions exist.\n", jtag_prog_param.addr, MAX_SDCC_PARTITIONS);
        probe_status = MEM_DEVICE_UNKNOWN;
        return STATUS_FAIL;
   }
   else
   {
        hsdev_current = hsdev_handles[jtag_prog_param.addr];
   }

   probe_status = MEM_DEVICE_FOUND;
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION
  This function call the SDCC API to set the active boot partition

DEPENDENCIES

RETURN VALUE
  If success, return the starting sector of the givin partition, if
  failed, return 0

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_set_active_boot(void)
{
   SDCC_STATUS rc;

   /* Not closing the old handle on purpose - Ensure the clock not getting turned off*/
   if (jtag_prog_param.addr > MAX_SDCC_PARTITIONS || jtag_prog_param.addr == 3) {
        outputMessage("Failed to switch partition.\n");
        if (jtag_prog_param.addr == 3)
            outputMessage("ERROR: We do not support switching to RPMB partition. Please consult SDCC driver team.\n");
        else if (jtag_prog_param.addr > MAX_SDCC_PARTITIONS)
            outputMessage("ERROR: You requested to switch to partition %d that doesn't exist.\nOnly %d partitions exist.\n", jtag_prog_param.addr, MAX_SDCC_PARTITIONS);

        probe_status = MEM_DEVICE_UNKNOWN;
        return STATUS_FAIL;
   }

   rc = sdcc_handle_set_active_bootable_partition( hsdev_handles[jtag_prog_param.addr] );
   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("SDCC READ ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }

   return STATUS_SUCCESS;
}



/*===========================================================================

DESCRIPTION
  This function cycle though different PLL frequency and does a read 
  each time.  It report the frequency setting and the success/fail status
  of the read.

DEPENDENCIES

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_debug_frequency_scan(void)
{
   return STATUS_SUCCESS;
}


/*===========================================================================

DESCRIPTION
  This function evokes the SDCC driver's internal RX test.  The test
  is to check signal quality on the data line and can be used to 
  measure the timing margin available on the particular setup.

DEPENDENCIES

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
boolean sdcc_rx_test( int16 driveno, unsigned char *buffer );
uint32 jsdcc_debug_start_rx_test(void)
{
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_gpp_program(void)
{
#ifdef FEATURE_GPP
   boolean rc = FALSE;
/*   outputMessage("Sizes are: %d, %d, %d, %d\n", *((uint32*)(jtag_prog_param.data)), *((uint32*)(jtag_prog_param.data+8)), *((uint32*)(jtag_prog_param.data+16)), *((uint32*)(jtag_prog_param.data+24)));*/
   sdcc_emmc_gpp_enh_desc_type desc;
   
   /* Extract the GPP sizes from the data buffer, sizes are stored using %LONG so are 4 bytes each */
   /* Sizes are in KB, so multiply by 2 to convert to sectors */
   desc.GPP_size[0] = (*((uint32*)(jtag_prog_param.data)))*2;
   desc.GPP_size[1] = (*((uint32*)(jtag_prog_param.data+4)))*2;
   desc.GPP_size[2] = (*((uint32*)(jtag_prog_param.data+8)))*2;
   desc.GPP_size[3] = (*((uint32*)(jtag_prog_param.data+12)))*2;
   desc.ENH_size = 0;
   desc.ENH_start_addr = 0;
   desc.GPP_enh_flag = 0;
   outputMessage("Sector sizes are: %d, %d, %d, %d\n", desc.GPP_size[0], desc.GPP_size[1], desc.GPP_size[2], desc.GPP_size[3]);

   rc = sdcc_config_emmc_gpp_enh (hsdev_current, &desc);

   if (rc != SDCC_NO_ERROR)
   {
     outputMessage("Status FAIL SDCC ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }
   else
   {
     outputMessage("Status SUCCESS\n");
   }
   return STATUS_SUCCESS;
#else
   outputMessage("GPP creation not permitted\n");
   return STATUS_FAIL;
#endif
}


/* Stub out codes */
uint32 rex_int_lock ( void )
{
   return 0;
} 


uint32 rex_int_free ( void )
{
   return 0;
} 

/*===========================================================================

FUNCTION  calc_crc32

DESCRIPTION
  This function calculate CRC32 on input data.

DEPENDENCIES
  None

RETURN VALUE
  Returns CRC32 of given data

SIDE EFFECTS
  None

===========================================================================*/
static uint32 calc_crc32
(
  const uint8   *data_in,
  const uint32  nbytes_in
)
{
    uint32 k = 8;                   // length of unit (i.e. byte)
    int MSB = 0;
    int gx = 0x04C11DB7;         // IEEE 32bit polynomial
    int regs = 0xFFFFFFFF;       // init to all ones
    int regsMask = 0xFFFFFFFF;   // ensure only 32 bit answer
    int regsMSB = 0;
    uint32 i, j;
    uint8 DataByte;

    if ( (data_in == NULL) || (nbytes_in == 0) )
        return 0;

    for( i=0; i < nbytes_in; i++) {
        DataByte = data_in[i];
        DataByte = reflect(DataByte,8);
        for(j=0; j < k; j++) {
            MSB = DataByte >> (k-1);  // get MSB
            MSB &= 1;                 // ensure just 1 bit
            regsMSB = (regs>>31) & 1; // MSB of regs
            regs = regs<<1;           // shift regs for CRC-CCITT
            if(regsMSB ^ MSB) {       // MSB is a 1
                regs = regs ^ gx;       // XOR with generator poly
            }
            regs = regs & regsMask;   // Mask off excess upper bits
            DataByte <<= 1;           // get to next bit
        }
    }

    regs = regs & regsMask;       // Mask off excess upper bits
    return reflect(regs,32) ^ 0xFFFFFFFF;
}

static int reflect
(
  int data,
  const uint32 len
)
{
    int ref = 0;
    uint32 i;

    for(i=0; i < len; i++) {
        if(data & 0x1) {
            ref |= (1 << ((len - 1) - i));
        }
        data = (data >> 1);
    }

   return ref;
}
