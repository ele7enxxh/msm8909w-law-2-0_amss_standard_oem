#ifndef __SDCC_HANDLE_UTIL_H
#define __SDCC_HANDLE_UTIL_H
/******************************************************************************
 * File: sdcc_handle_util.h
 *
 * Services:
 *    This file provides SDCC handle util definitions for SDIO and memory
 *    devices.
 *
 * Description:
 *    This file contains the SDCC handle specific utility function prototypes.
 *
 * Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_handle_util.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
2013-08-12   bn      Code clean up
2009-03-09   sc      Initial version

=============================================================================*/
#include "sdcc_priv.h"

/* maximum number of client handle array entries */
#define SDCC_CLIENT_HANDLE_MAX_NUM         32

/* dereference handle to get the driveno */
#define SDCC_HANDLE_GET_DRIVENO(h) ((h)->hdev->driveno)

/* internal client handle structure to track the device handle, partition, */
/* task control block and SPS data transfer end point */
/* This sdcc_device is a common structure to SD/MMC and SDIO. */
struct sdcc_device
{
   struct sdcc_card_info *hdev;             /* pointer to the device info structure */
   uint32                phy_partition_num; /* physical partition number */
   void                 *data_xfer_end_pt;  /* SPS end pt for data xfer */
};

/******************************************************************************
* Name: sdcc_register_client_handle
*
* Description:
*    This function registers a client handle with the specified driveno and
*    phy_partition_num.  It returns a pointer to a client handle entry if the
*    registration succeeds.
*
* Arguments:
*    driveno           [IN] : a valid drive number between 0 and the total
*                             number of card slots - 1
*    phy_partition_num [IN] : physical partition number.  Use
*                             \ref SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE
*                             to get a handle representing the entire memory
*                             device.  For eMMC v4.3, the valid boot partition
*                             numbers are 1 and 2.  User area partition number
*                             is 0.
*
* Returns:
*    A pointer to SDCC device if successful.  NULL if failed.
*
******************************************************************************/
struct sdcc_device *sdcc_register_client_handle( uint32 driveno,
                                                 uint32 phy_partition_num );

/******************************************************************************
* Name: sdcc_deregister_client_handle
*
* Description:
*    This function de-registers a client handle.  It clears all client handle
*    fields in an entry and set the handle to NULL.
*
* Arguments:
*    handle     [IN/OUT] : a pointer to a pointer to the SDCC device
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_deregister_client_handle( struct sdcc_device **handle );

/******************************************************************************
* Name: sdcc_is_device_still_in_use
*
* Description:
*    This function examines if there are any entries in the client handle list
*    still referring to the SDIO/memory device specified in driveno.
*
* Arguments:
*    pdev           [IN] : pointer to sdcc device handle that is in use
*                             
*
* Returns:
*    TRUE if SDIO/memory device is still in use.  FALSE otherwise.
*
******************************************************************************/
boolean sdcc_is_device_still_in_use( sdcc_dev_type *pdev );

#endif /* __SDCC_HANDLE_UTIL_H */

