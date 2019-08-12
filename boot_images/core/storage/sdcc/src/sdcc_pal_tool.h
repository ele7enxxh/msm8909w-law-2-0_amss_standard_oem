#ifndef __SDCC_PAL_TOOL_H
#define __SDCC_PAL_TOOL_H
/**********************************************************************
 * sdcc_pal_tool.h
 *
 * This file provides SDCC PAL definitions for TARGET TOOL platform
 *
 * Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_pal_tool.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     -----------------------------------------------
2013-09-03   bn      Add SDHCi Support
2013-08-12   bn      Code clean up
2010-10-27   rh      Port over the optimized open APIs
2010-04-20   vj      Added stub for sdcc_enable_interrupts()
2010-03-20   yg      sdcc_init_pal() returns a boolean
2009-11-23   rh      Add a separate function to wait for program done
2009-11-09   vj      Add stub sdcc_deinit_pal
2009-10-28   rh      Fix issue caused by added DM support 
2009-06-25   rh      Add stub for sdcc_free_dma_buffs
2009-06-05   rh      Initial version
==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#include "sdcc_priv.h"

/*************************************************************/
/*              DEFINITIONS AND CONSTANTS                    */
/*************************************************************/
/* SDCC related stuff */
#define sdcc_handle_silent_reinit(x)   TRUE
/* Card status check */
#define sdcc_busy_check(x)   sdcc_wait_prog_done(x)
#define sdcc_deinit_pal(x)

/*********************************************************************/
/*                           PROTOTYPES                              */
/*********************************************************************/

/******************************************************************************
* Name: sdcc_init_pal
*
* Description:
*    This function is to perform PAL specific sdcc_init
*
* Arguments:
*    pslot                    [IN] : Pointer to slot data structure
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean sdcc_init_pal(sdcc_slot_type *pslot);

/******************************************************************************
 * Name: sdcc_set_partition
 *
 * Description:
 *    This function sets the partition region on the memory device by sending
 *    a command to the device specified by the handle parameter.
 *
 * Arguments:
 *    handle       [IN] : a pointer to the SDCC device that was returned from
 *                        sdcc_handle_open()
 *
 * Returns:
 *    SDCC_NO_ERROR if successful, error code otherwise
 *
 ******************************************************************************/
SDCC_STATUS sdcc_set_partition( struct sdcc_device *handle );

/******************************************************************************
* Name: sdcc_config_rca
*
* Description:
*    This function is to set/get the RCA from the card. 
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to the device structure
*
* Returns:
*    rca value
*
******************************************************************************/
uint16 sdcc_config_rca( sdcc_dev_type *sdcc_pdev );


/******************************************************************************
* Name: sdcc_setup_data_xfer
*
* Description:
*    This function sets up the controller prior to data transfer.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*    direction          [IN] : data read or data write
*    units              [IN] : number of sectors to be read
*
* Returns:
*    data length
*
******************************************************************************/
uint32 sdcc_setup_data_xfer( sdcc_dev_type  *sdcc_pdev,
                             uint32          direction,
                             uint16          units );

/******************************************************************************
* Name: sdcc_process_signals
*
* Description:
*    This function is to process SDCC signals
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_process_signals( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_init_memory_device
*
* Description:
*    This function sends CMD2 to get the device ID and product related
*    information.  It then configures the device Relative Card Address and
*    completes the card identification process.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*   Card is in Identification mode. 
*
*****************************************************************************/
SDCC_STATUS sdcc_init_memory_device( sdcc_dev_type  *sdcc_pdev );

/******************************************************************************
* Name: sdcc_get_memory_info
*
* Description:
*    This function is to get the specific information related to the
*    memory device via CMD9 and/or CMD10 before it goes into data
*    transfer mode. 
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in stand-by mode.
*
******************************************************************************/
SDCC_STATUS sdcc_get_memory_info( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_config_memory_device
*
* Description:
*    This function is to programm the card into data transfer mode.  It
*    selects the card, sets up the bus width and block data transfer mode. 
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS sdcc_config_memory_device( sdcc_dev_type  *sdcc_pdev );

/******************************************************************************
* Name: sdcc_complete_data_xfer
*
* Description:
*    This function is to finish up data transfer after the data has been
*    successfully transferred.  If this is a multi-block data transfer,
*    we need to send CMD12 to stop the data transfer.  If this is a
*    single block write, we need to send CMD13 to wait for card is ready.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*    direction              [IN] : read or write
*    multi_block            [IN] : TRUE or FALSE
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_complete_data_xfer( sdcc_dev_type  *sdcc_pdev,
                                     uint32          direction,
                                     boolean         multi_block );

/******************************************************************************
* Name: sdcc_read_fifo
*
* Description:
*    This function is to process "read" in non-dma mode.  It reads data
*    out of the FIFO.
*
* Arguments:
*    pslot          [IN] : Pointer to slot data structure
*    buff          [OUT] : pointer of the data buffer
*    length         [IN] : data length
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_read_fifo (sdcc_slot_type *pslot,
                            byte           *buff,
                            uint32          length);

/******************************************************************************
* Name: sdcc_write_fifo
*
* Description:
*    This function is to process "write" in non-dma mode.  It writes data
*    directly to FIFO. 
*
* Arguments:
*    pslot          [IN] : Pointer to slot data structure
*    buff           [IN] : pointer of the data buffer
*    length         [IN] : data length
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_write_fifo (sdcc_slot_type *pslot,
                             byte     *buff,
                             uint32    length);

/******************************************************************************
* Name: sdcc_vreg_switch_on
*
* Description:
*    This function switches on the voltage regulator for SDCC.
*
* Arguments:
*    driveno     [IN] : drive number
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_vreg_switch_on( int16 driveno );

/******************************************************************************
* Name: sdcc_handle_find_card
*
* Description: 
*    This function finds out the card type specified in handle parameter. 
*
* Arguments:
*    handle [IN] : a pointer to the SDCC device that was returned from
*                  sdcc_handle_open()
*
* Returns:
*    card type that indicates if device is SD, MMC, SDIO, or unknown
*
******************************************************************************/
SDCC_CARD_TYPE sdcc_handle_find_card( struct sdcc_device *handle );

/******************************************************************************
* Name: sdcc_wait_card_ready
*
* Description:
*    This function is to check the card busy status by calling "send
*    status" command.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_wait_card_ready( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
 * Name: sdcc_do_transfer
 *
 * Description:
 *    This is a wrapper function that decides whether to use the Qualcomm
 *    or Host Controller standard interface for communicating with the 
 *    memory device.
 *
 * Arguments:
 *    p_device             [IN] : SDCC context data for this SD-card
 *    p_command            [IN] : command to send to the SD-card to perform 
 *                                the read or write
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_do_transfer(sdcc_dev_type *p_device, sdcc_cmd_type *p_command,
                             byte *buffer, uint16 transfer_size);

/******************************************************************************
 * Name: sdcc_do_transfer_qc_mode
 *
 * Description:
 *    This is a generic function to perform a read or write data transfer on
 *    the SD-card using the Qualcomm interface.
 *
 * Arguments:
 *    p_device             [IN] : SDCC context data for this SD-card
 *    p_command            [IN] : command to send to the SD-card to perform 
 *                                the read or write
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_do_transfer_qc_mode(sdcc_dev_type *p_device, sdcc_cmd_type *p_command, 
                                     byte *buffer, uint16 transfer_size);

/******************************************************************************
* Name: sdcc_handle_optimized_open
*
* Description:
*    This function does optimized initialization of the MMC device.
*    It returns NULL if there is no memory device attached.
*    It then sets up the device for data transfer.
*    A non-null pointer to the SDCC device is returned.
*
* Arguments:
*    phy_partition_num [IN] : physical partition number.
*
* Returns:
*    A pointer to the SDCC device if successful.  NULL if failed.
*
******************************************************************************/
struct sdcc_device *
sdcc_handle_optimized_open ( uint32 phy_partition_num );

/******************************************************************************
* Name: sdcc_is_partial_enumeration
*
* Description:
*    This function checks if partial Initialization is to be done.
*
* Arguments:
*    driveno       [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean sdcc_is_partial_enumeration ( int16 driveno );

/******************************************************************************
 * Name: sdcc_send_app_command
 *
 * Description:
 *    This function is to send an App-command to the SD-card
 *
 * Arguments:
 *    p_app_command          [IN] : pointer to a structure describing the
 *                                  App-Command to send.  The command should
 *                                  be one of the SD_ACMDXXXX enums.
 *    p_device               [IN] : pointer to the device structure
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_send_app_command (sdcc_dev_type *, sdcc_cmd_type *);

#endif /* __SDCC_PAL_TOOL_H */

