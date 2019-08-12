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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/jtagprogrammer/ufs/src/jtagprogrammer_main.c#1 $ 
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
#include <inttypes.h>
#include "jtagprogrammer_param.h"
#include "ufs_api.h"
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
static struct struct_jtag_prog_param jtag_prog_param __attribute__((used));

/* Output buffer */
static char output_buffer[JSDCC_OUTPUT_BUFFER_SIZE];
static uint32 output_buffer_length;

/* enum to indicate status of probe */
static probe_code_type probe_status = MEM_DEVICE_NOT_PROBED;

/* These variables are used to redirect the programming routine to partition 
 * aware programming routine */
static uint32 redirect_offset __attribute__((used)) = 0;
static uint32 redirect_partition __attribute__((used)) = 0;
static uint32 card_slot_in_use __attribute__((used)) = 0;
static uint32 sector_writing __attribute__((used)) = 0;
static struct ufs_info_type current_mem_info __attribute__((used));

static struct ufs_config_descr storage_extras __attribute__((used));

/* device handle */
#define MAX_PARTITIONS 8
static unsigned int max_partitions __attribute__((used)) = MAX_PARTITIONS;
struct ufs_handle *partition_config;           /* User partition handle */
struct ufs_handle *partition_handles[MAX_PARTITIONS];
static unsigned int current_handle_index __attribute__((used)) = 0;
static int lun_to_grow __attribute__((used)) = -1;

char *partition_names[MAX_PARTITIONS] = {
    "LUN 0",
    "LUN 1",
    "LUN 2",
    "LUN 3",
    "LUN 4",
    "LUN 5",
    "LUN 6",
    "LUN 7",
};

char *err_strings[] = {
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

static uint32 BLOCK_SIZE __attribute__((used)) = 4096;
static uint32 blocks_per_alloc_unit __attribute__((used)) = 1024;

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
uint32 jsdcc_mem_set_hw_partition(void);
uint32 jsdcc_mem_set_active_boot(void);
uint32 jsdcc_extras_program(void);

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
   if (probe_status == MEM_DEVICE_NOT_PROBED)
   {
      ret_code = do_probe();
      if (ret_code != STATUS_SUCCESS)
        outputMessage("ERROR: device not found\n");
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
         outputMessage("Executing PRINT_INFO\n");
         ret_code = jsdcc_mem_print_info();
         break;

      case JTAG_CMD_CHIP_ERASE:
         outputMessage("Executing CHIP_ERASE\n");
         ret_code = jsdcc_mem_chip_erase();
         break;

      case JTAG_CMD_SET_HW_PARTITION:
         outputMessage("Executing SET_HW_PARTITION\n");
         ret_code = jsdcc_mem_set_hw_partition();
         break;

      case JTAG_CMD_SET_ACTIVE_BOOT:
         outputMessage("Executing SET_ACTIVE_BOOT\n");
         ret_code = jsdcc_mem_set_active_boot();
         break;

      case JTAG_CMD_PROG_EXTRAS:
         outputMessage("Executing PROGRAM EXTRAS\n");
         ret_code = jsdcc_extras_program();
         break;

      default:
         ret_code = STATUS_FAIL;
         outputMessage ("ERROR: Invalid command 0x%x\n", jtag_prog_param.cmd_status);
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

void open_handles(void) {
    unsigned int i;
    for (i = 0; i < max_partitions; i++)
    {
        partition_handles[i] = ufs_open(0, i);
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
   int32_t rc = 0;

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

        //outputMessage("\nUsing 'user specified' card slot %d\n",card_slot_in_use);

        partition_config = ufs_open(0, UFS_WLUN_DEVICE);

        // Basic NULL handle check and get required device parameters if any
        if (partition_config != NULL && (rc = ufs_get_device_info(partition_config, &current_mem_info)) == 0) {
            BLOCK_SIZE = current_mem_info.bMinAddrBlockSize * 512;
            blocks_per_alloc_unit = (current_mem_info.dSegmentSize * current_mem_info.bAllocationUnitSize * 512) / BLOCK_SIZE;

			// Open all handles
			open_handles();

            ret_code = STATUS_SUCCESS;
            probe_status = MEM_DEVICE_FOUND;
        }
        else {
            if (NULL == partition_config) {
                outputMessage("Error: Unable to open CONFIG partition.\n");
            }
            else if (rc != 0) {
                outputMessage("Could not get config info. ERROR 0x%X: %s\n", rc, err_strings[rc]);
            }
            outputMessage("Possible causes: card not present? No power or clock to the card?\n");
            ret_code = STATUS_FAIL;
            probe_status = MEM_DEVICE_UNKNOWN;
        }

        current_handle_index = 0;

        // TODO: remove below few lines, only added since virtio doesn't support the config LUN
        //ret_code = STATUS_SUCCESS;
        //probe_status = MEM_DEVICE_FOUND;
        //current_handle_index = 1;

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
    int32_t rc = 0;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];

    pdata = jtag_prog_param.data;
    rc = ufs_read(partition_current, pdata, jtag_prog_param.addr, jtag_prog_param.data_length);

    if (rc != 0) 
    {
        outputMessage("Read ERROR 0x%X: %s\n", rc, err_strings[rc]);
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}  // jsdcc_mem_read


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
    int32_t rc = 0;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];

    outputMessage("Sector Write begins: \n");
    outputMessage("Addr %x, Size %x, Offset %x \n", 
        jtag_prog_param.addr, jtag_prog_param.data_length, 
        jtag_prog_param.mod_addr);
   
    pdata = jtag_prog_param.data;
    rc = ufs_write(partition_current, pdata, jtag_prog_param.addr, jtag_prog_param.data_length);

    if (rc != 0) 
    {
        outputMessage("Write ERROR 0x%X: %s\n", rc, err_strings[rc]);
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}  // jsdcc_mem_write


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
    int32_t rc = 0;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];

    if(sector_writing) {
        jtag_prog_param.addr = redirect_offset + SIZE_IN_BLOCKS(jtag_prog_param.addr);
        jtag_prog_param.data_length = jtag_prog_param.data_length < redirect_partition ? jtag_prog_param.data_length : redirect_partition;
        if (jtag_prog_param.data_length == 0) {
            return STATUS_SUCCESS;
        }
        redirect_partition -= jtag_prog_param.data_length;
    }

    // Print out debugging information
    //outputMessage("Addr %x, Size %x, Blocks %x\n", 
    //                jtag_prog_param.addr, jtag_prog_param.data_length, SIZE_IN_BLOCKS(jtag_prog_param.data_length));

    pdata = jtag_prog_param.data;
    rc = ufs_write(partition_current, pdata, jtag_prog_param.addr, 
                   SIZE_IN_BLOCKS(jtag_prog_param.data_length));

    if (rc != 0) 
    {
        outputMessage("WRITE ERROR 0x%X: %s\n", rc, err_strings[rc]);
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}  // jsdcc_mem_program


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
    int32_t rc = 0;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];

   if (NULL == partition_current)
   {
     outputMessage("Using UFS_WLUN_DEVICE");
     partition_current = partition_config;
   }


   outputMessage("\n==============================\n");
   outputMessage(  "=====       INFO         =====\n");
   outputMessage(  "==============================\n");

   if(partition_current == NULL)
   {
	 outputMessage("\nERROR: No device detected on that slot\n");
     return STATUS_FAIL;
   }

   rc = ufs_get_device_info(partition_current, &current_mem_info);

   if (rc != 0) 
   {
     outputMessage("GET INFO ERROR 0x%X: %s\n", rc, err_strings[rc]);
     return STATUS_FAIL;
   }

	// Print out the detail
   outputMessage ("fInitialized: %d\n", current_mem_info.fInitialized);
   outputMessage ("bLUN: %d\n", current_mem_info.bLUN);
   outputMessage ("bNumberLu: %d\n", current_mem_info.bNumberLu);
   outputMessage ("wManufacturerID: %x\n", current_mem_info.wManufacturerID);
   outputMessage ("bBootEnable: %d\n", current_mem_info.bBootEnable);
   outputMessage ("qTotalRawDeviceCapacity: %" PRIu64 "\n", current_mem_info.qTotalRawDeviceCapacity);
   outputMessage ("bMinAddrBlockSize: %d\n", current_mem_info.bMinAddrBlockSize);
   outputMessage ("dSegmentSize: %d\n", current_mem_info.dSegmentSize);
   outputMessage ("bAllocationUnitSize: %d\n", current_mem_info.bAllocationUnitSize);
   outputMessage ("bRPMB_ReadWriteSize: %d\n", current_mem_info.bRPMB_ReadWriteSize);
   outputMessage ("dNumAllocUnits: %d\n", current_mem_info.dNumAllocUnits);
   outputMessage ("bLogicalBlockSize: %d\n", current_mem_info.bLogicalBlockSize);
   outputMessage ("bProvisioningType: %x\n", current_mem_info.bProvisioningType);
   outputMessage ("bLUWriteProtect: %x\n", current_mem_info.bLUWriteProtect);
   outputMessage ("bBootLunID: %x\n", current_mem_info.bBootLunID);
   outputMessage ("bMemoryType: %x\n", current_mem_info.bMemoryType);
   outputMessage ("dLuTotalBlocks: %d\n", current_mem_info.dLuTotalBlocks);

   jtag_prog_param.data_length = current_mem_info.dLuTotalBlocks;
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
   int32_t rc = 0;
   byte *pdata;
   uint32 erase_data;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];
   
   while(jtag_prog_param.data_length > 0)
   {
       erase_data = jtag_prog_param.data_length < (PARAM_BUFFER_SIZE/BLOCK_SIZE) ? jtag_prog_param.data_length : (PARAM_BUFFER_SIZE/BLOCK_SIZE);

       /*rc = sdcc_handle_erase(partition_user, BLOCK_SIZE*jtag_prog_param.addr, jtag_prog_param.data_length);*/
       if (partition_current != NULL) {
         pdata = jtag_prog_param.data;
         memset(pdata, 0x00, BLOCK_SIZE*erase_data);
         //rc = sdcc_handle_write(partition_current, jtag_prog_param.addr, pdata, erase_data);
       }
       else {
         outputMessage("No current handle to erase");
         return STATUS_FAIL;
       }
       if (rc != 0) 
       {
         outputMessage("ERROR 0x%X: %s\n", rc, err_strings[rc]);
         return STATUS_FAIL;
       }
       jtag_prog_param.addr += erase_data;
       jtag_prog_param.data_length -= erase_data;
   }
   outputMessage("ERASE: Success! \n");
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
   int32_t rc = 0;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];

   uint32 sectors_to_read;
   sectors_to_read = jtag_prog_param.data_length / BLOCK_SIZE;
   if (jtag_prog_param.data_length % BLOCK_SIZE != 0)
        sectors_to_read++;

   pdata = jtag_prog_param.data;
   rc = ufs_read(partition_current, pdata, jtag_prog_param.addr, sectors_to_read);

   if (rc != 0) 
   {
     outputMessage("READ ERROR 0x%X: %s\n", rc, err_strings[rc]);
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
    int32_t rc = 0;
    struct ufs_handle *partition_current;
    unsigned int i;

    for (i = 0; i <= max_partitions; i++)
    {
        partition_current = partition_handles[i];
        if (NULL != partition_current) {
            rc = ufs_get_device_info(partition_handles[i], &current_mem_info);
            if (rc != 0) {
                outputMessage("CHIP ERASE ERROR 0x%X: %s\n", rc, err_strings[rc]);
            }
            else {
                rc = ufs_erase(partition_handles[i], 0, current_mem_info.dLuTotalBlocks);
                if (rc != 0) {
                    outputMessage("CHIP ERASE ERROR 0x%X: %s\n", rc, err_strings[rc]);
                    continue;
                }
            }
        }
    }

    outputMessage("CHIP_ERASE: Success! \n");
    return STATUS_SUCCESS;
}  // jsdcc_mem_chip_erase

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
   // Not closing the old handle on purpose - Ensure the clock not getting turned off
   if (jtag_prog_param.addr > max_partitions) {
        outputMessage("Failed to switch partition.\n");
        outputMessage("ERROR: You requested to switch to partition %d that doesn't exist.\nOnly %d partitions exist.\n", jtag_prog_param.addr, max_partitions);
        probe_status = MEM_DEVICE_UNKNOWN;
        return STATUS_FAIL;
   }
   else
   {
        current_handle_index = jtag_prog_param.addr;
   }

   probe_status = MEM_DEVICE_FOUND;
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION
  This function call the API to set the active boot partition

DEPENDENCIES

RETURN VALUE
  If success, return the starting sector of the givin partition, if
  failed, return 0

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_mem_set_active_boot(void)
{
   int32_t rc = 0;

   /* Not closing the old handle on purpose - Ensure the clock not getting turned off*/
   if (jtag_prog_param.addr > max_partitions) {
        outputMessage("Failed to switch partition.\n");
        outputMessage("ERROR: You requested to switch to partition %d that doesn't exist.\nOnly %d partitions exist.\n", jtag_prog_param.addr, max_partitions);
        probe_status = MEM_DEVICE_UNKNOWN;
        return STATUS_FAIL;
   }

   rc = ufs_set_bootable(partition_config, jtag_prog_param.addr);
   if (rc != 0)
   {
     outputMessage("ERROR: 0x%X: %s\n", rc, err_strings[rc]);
     return STATUS_FAIL;
   }
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 jsdcc_extras_program(void)
{
#ifdef FEATURE_JTAGPROGRAMMER_STORAGE_EXTRAS
   int32_t rc = FALSE;
   size_t alloc_units, units_to_create = 0;
   size_t capacity_to_alloc_factor;
   size_t enhanced1_units = 0, enhanced2_units = 0;
   size_t conversion_ratio = 1;
   int i;

   if(partition_config == NULL)
   {
		outputMessage("Opening UFS device LU failed\n");
		return STATUS_FAIL;
   }
   rc = ufs_get_device_info(partition_config, &current_mem_info);
   if (0 != rc) {
        outputMessage("Could not get partition info\n");
        return STATUS_FAIL;
   }
   capacity_to_alloc_factor = (blocks_per_alloc_unit * BLOCK_SIZE) / 512;
   if (current_mem_info.qTotalRawDeviceCapacity % capacity_to_alloc_factor != 0) {
        outputMessage("Raw capacity not a multiple of alloc unit size\n");
        return FALSE;
   }
   alloc_units = (current_mem_info.qTotalRawDeviceCapacity / capacity_to_alloc_factor) ;

   units_to_create = 0;
   enhanced1_units = enhanced2_units = 0;
   for (i = 0; i < max_partitions && units_to_create <= alloc_units; i++) {
        if (0 == storage_extras.unit[i].bMemoryType) {
            units_to_create += storage_extras.unit[i].dNumAllocUnits;
        }
        else if (3 == storage_extras.unit[i].bMemoryType) {
            enhanced1_units += storage_extras.unit[i].dNumAllocUnits;
            units_to_create += storage_extras.unit[i].dNumAllocUnits * (current_mem_info.wEnhanced1CapAdjFac / 0x100);
        }
        else if (4 == storage_extras.unit[i].bMemoryType) {
            enhanced2_units += storage_extras.unit[i].dNumAllocUnits;
            units_to_create += storage_extras.unit[i].dNumAllocUnits * (current_mem_info.wEnhanced2CapAdjFac / 0x100);
        }
        else {
            outputMessage("Unsupported memory type %d\n", storage_extras.unit[i].bMemoryType);
            return FALSE;
        }
   }
   if (enhanced1_units > current_mem_info.dEnhanced1MaxNAllocU) {
        outputMessage("Size %d exceeds max enhanced1 area size %d\n", enhanced1_units, current_mem_info.dEnhanced1MaxNAllocU);
        return FALSE;
   }
   if (enhanced2_units > current_mem_info.dEnhanced2MaxNAllocU) {
        outputMessage("Size %d exceeds max enhanced2 area size %d\n", enhanced2_units, current_mem_info.dEnhanced2MaxNAllocU);
        return FALSE;
   }
   if (units_to_create > alloc_units) {
        outputMessage("Specified size %d exceeds device size %d\n", units_to_create, alloc_units);
        return FALSE;
    }

   if (lun_to_grow != -1) {
        if (0 == storage_extras.unit[i].bMemoryType)
            conversion_ratio = 1;
        else if (3 == storage_extras.unit[i].bMemoryType)
            conversion_ratio = (current_mem_info.wEnhanced1CapAdjFac / 0x100);
        else if (4 == storage_extras.unit[i].bMemoryType)
            conversion_ratio = (current_mem_info.wEnhanced2CapAdjFac / 0x100);

        storage_extras.unit[lun_to_grow].dNumAllocUnits += ( (alloc_units - units_to_create) / conversion_ratio );
   }
	
   rc = ufs_configure_device (partition_config, &storage_extras);

   if (rc != 0)
   {
     outputMessage("Status FAIL ERROR %d\n", rc);
     return STATUS_FAIL;
   }
   else
   {
     outputMessage("%d\n", storage_extras.bNumberLU);
     outputMessage("Status SUCCESS\n");
   }
   open_handles();
   return STATUS_SUCCESS;
#else
   outputMessage("LUN creation not permitted\n");
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
