#ifndef __SDCC_UTIL_H
#define __SDCC_UTIL_H
/**********************************************************************
 * sdcc_util.h
 *
 * This file provides SDCC util definitions.
 *
 * Copyright (c) 2007-2013
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_util.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------- 
2013-09-03   bn      Add SDHCi Support
2013-08-12   bn      Code Cleanup
2010-02-18   yg      Added extern boolean sdcc_timer_status[]
2009-12-14   vin     sdcc_reset_reinit brought under common interface 
2009-12-16   sp      Move sdcc_wait_prog_done out of extracted APIs marker
                     since WM no longer is using this API
2009-11-23   rh      Modify sdcc_wait_prog_done to take sdcc_pdev as input
2009-11-10   rh      Adding reinitialize routine for resetting FIFO error
2009-09-14   rcc     Added sdcc_clk_regime_reset to the list of extracted APIs
2009-07-28   vj      Moved sdcc_clk_regime_reset prototype from common
2009-07-20   ah      created a function prototype for sdcc_clk_regime_reset
2009-06-30   vin     Exported sdcc_clk_regime_sel_clk
2009-06-15   rh      Adding common interface extraction tag
2009-05-08   sp      PAL support added
2009-03-09   vj      Boot Support Added
2009-03-09   sc      Added handle support
2009-02-18   rh      Remove Alternate-GPIO usage
2009-02-09   vj      OS Abstraction Layer
2009-01-28   rh      Interface cleanup
2008-01-20   rcc     Removed sdcc_intf_park and sdcc_intf_switch
2009-01-09   rh      Cleanup of cust file dependencies
2008-10-23   vin     Support for MMC power collapse
2008-09-29   vin     HAL implementation
2008-09-22   sc      Separated out read/write function for DMOV non-compliant memory
2008-07-27   rcc     Removed an unnecessary WM-specific prototype
2008-06-23   sc      Added dmov_enable and dmov_disable
2008-05-21   sc      Updated the #include
2008-05-16   sc      Extracted the temp data buffer codes into a new function
2008-05-07   vin     Added support for SD & SDIO concurrency
04/01/08     sc      Fixed power down issue on slot4 when no cards on other slots
02/25/08     vin     Added functions for SDCC critical section
02/13/08     umr     Removed feature FEATURE_SDCC_EXTERNAL_DM and T_MSM7500.
12/6/07      vin     Added checks for compilability on 7500
11/29/07     vin     Added support for four SD controllers
11/29/07     sc      Updated sdcc_set_blksz function description
11/20/07     umr     Implement wrapper functions to control VREG MMC and WLAN.
11/13/07     umr     Featurised sdcc_switch_intf () and sdcc_intf_park ().
11/08/07     rcc     Removed redundant WINCE-only sdcc_config_dma prototype
10/19/07     sc      Fixed the data length overflow defect
10/09/07     vin     Moved sdcc_find_card() to sdcc_api.h
06/11/07     sc      Moved sdcc_temp_data_buff_array to sdcc_api.c
02/16/07     sc      Moved MMC specifics to sdcc_mmc_util
02/14/07     sc      Minor cleanup
02/13/07     sc      Updated a MMC v4.2 function prototype
01/31/07     sc      Ported sdcc_util.h to 7K target and did major
                     code clean up

==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#include "sdcc_priv.h"
#include SDCC_PAL_H

/*************************************************************/
/*              DEFINITIONS AND CONSTANTS                    */
/*************************************************************/

/*********************************************************************/
/*                           PROTOTYPES                              */
/*********************************************************************/

/******************************************************************************
 * Name: sdcc_deinit
 *
 * Description:
 *    This function de-selects the device, turns off the power supply to the
 *    device and switches off the controller clock.
 *
 * Arguments:
 *    driveno     [IN] : drive number
 *
 * Returns:
 *    SDCC_NO_ERROR if successful.  Error code otherwise
 *
 ******************************************************************************/
SDCC_STATUS sdcc_deinit( sdcc_dev_type  *sdcc_pdev );

/******************************************************************************
* Name: sdcc_get_slot_handle
*
* Description:
*    This function return the handle to the sdcc data structure
*
* Arguments:
*    driveno          [IN] : sdcc drive number
*
* Returns:
*    None
*
******************************************************************************/
struct sdcc_slot_data *sdcc_get_slot_handle (uint32 driveno);


/******************************************************************************
* Name: sdcc_reset_reinit
*
* Description:
*    This function resets and reinitializes the SDCC controller after
*    the controller has encountered an transfer error causing the FIFO
*    to be corrupted.
*
* Arguments:
*    driveno       [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_reset_reinit( sdcc_slot_type  *sdcc_pslot );

/******************************************************************************
* Name: sdcc_command
*
* Description:
*    This is a wrapper function for sending the command and poll 
*    the status. It decides whether to use the Qualcomm or standard 
*    Host Controller interface to communicate with the memory device
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device structure
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_command(sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd);

/******************************************************************************
* Name: sdcc_command_qc_mode
*
* Description:
*    This function is to send the command and poll the status using the
*    Qualcomm interface to communicate with the memory devices
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device structure
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_command_qc_mode (sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd );

/******************************************************************************
* Name: sdcc_send_cmd
*
* Description:
*    This function is to send out the command.
*
* Arguments:
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_send_cmd (sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd);

/******************************************************************************
* Name: sdcc_send_status
*
* Description:
*    This function is to let the currently selected device send
*    the device status.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns the current device status
*
* Note:
*    - Still used by sdcc_api.c
*
******************************************************************************/
SDCC_CARD_STATUS sdcc_send_status( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_wait_prog_done
*
* Description:
*    This function is to poll the status to make sure the device is not
*    busy.  CMD12(stop) and CMD38(erase) need to call this to make sure
*    the device programming has finished.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_wait_prog_done( sdcc_dev_type *sdcc_pdev );

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
* Name: sdcc_config_clk
*
* Description:
*    This function is to configure the SDCC controller clock depending
*    on current mode.
*
* Arguments:
*    sdcc_pdev        [IN] : pointer to device type 
*    mode             [IN] : card mode
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_config_clk (sdcc_dev_type *sdcc_pdev, SDCC_CARD_MODE mode);

/******************************************************************************
* Name: sdcc_init_qc_mode
 *
* Description:
*    This function initializes the SDCC controller to using Qualcomm interface 
*    for communication with the memory device. It first turns on
*    the clock, configures the GPIOs and then turns on the power supply
*    to the device.  It also initializes the related software data
*    structures.
*
* Arguments:
*    driveno       [IN] : drive number
 *
* Returns:
*    TRUE if successful
*    FALSE if failed
 *
 ******************************************************************************/
boolean sdcc_init_qc_mode (uint32 driveno);

/******************************************************************************
*
* Description:
*    This function is to select/deselect the specified card.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*    select             [IN] : select if TRUE, deselect if FALSE
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_select_card
(
   sdcc_dev_type  *sdcc_pdev,
   boolean         select
);


/******************************************************************************
* This function is to initialize the DMA data transfer
*
* Arguments:
*    sdcc_pslot         [IN] : pointer to slot structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
int sdcc_init_dma(sdcc_slot_type *pslot);


#endif /* __SDCC_UTIL_H */
