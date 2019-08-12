/**********************************************************************
 * flash_ops_setup.c
 *
 * SERVICES:    Provides flash operation setup API's used by EFS
 *
 * DESCRIPTION: Provides flash operation setup API's used by EFS
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================
 *
 *                   EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_ops_setup.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * --------     ---     ----------------------------------------------
 * 03/22/11     sv      Fix issue with NPA release
 * 03/07/11     sv      Initial Version
 *===================================================================*/

 /*===================================================================

===========================================================================*/

#include "flash_ops_setup.h"
#include "sleep.h"
#include "npa.h"
#include "customer.h"

/* This macro can be defined in .builds file with a new clk adjustment setting should
 * it be necessary.  Otherwise, default setting of 160MHz will be used. */
#ifdef FLASH_SPI_NPA_HCLK_KHZ
  #define NPA_HCLK_CLOCK_KHZ  FLASH_SPI_NPA_HCLK_KHZ
#else
  /* Default hclk request setting 160MHz */
  #define NPA_HCLK_CLOCK_KHZ 160000
#endif

/* Sleep okts(ok-to-sleep) handle */
static sleep_okts_handle spi_sleep_handle = NULL;

/* NPA handle for CPU voting */
static npa_client_handle spi_npa_bus_handle = NULL;

/* Global provides ability to adjust value in t32 for tuning and testing */
static uint32 spi_clock_request = NPA_HCLK_CLOCK_KHZ;

/* Flash Operations deinit */
void flash_ops_init(void)
{
  /* check if spi sleep handle is registered */
  if (!spi_sleep_handle)
  {
    spi_sleep_handle = sleep_register("SPI-NOR", FALSE);
  }

  /* check if spi npa bus handle exists */
  if (!spi_npa_bus_handle)
  {
    /* Create this client's NPA handle for hclk requests only once.
     * NPA resource release by client should be done by invoking
     * npa_complete_request() API.
     */
    spi_npa_bus_handle = npa_create_sync_client(
      "/clk/bus/sys", "SPI-NOR", NPA_CLIENT_REQUIRED);
  }
}

/* Flash Operations deinit */
void flash_ops_deinit(void)
{
  /* check if spi sleep handle is registered */
  if(spi_sleep_handle)
  {
    sleep_deregister(spi_sleep_handle);
    spi_sleep_handle = NULL;
  }

  /* check if spi npa handle is registered */
  if(spi_npa_bus_handle)
  {
    npa_destroy_client(spi_npa_bus_handle);
    spi_npa_bus_handle = NULL;
  }
}

/* Flash Operations start - will be called by EFS before any
 * Flash operation
 */
void flash_ops_start(void)
{
  if(spi_sleep_handle)
  {
    /* Vote to disable sleep */
    sleep_negate_okts(spi_sleep_handle);
  }

  if (spi_npa_bus_handle)
  {
    /* Request hclk clock during SPI transfer */
    npa_issue_required_request(spi_npa_bus_handle, spi_clock_request);
  }
}

/* Flash Operations end - will be called by EFS after any
 * Flash operation
 */
void flash_ops_end(void)
{
  if(spi_sleep_handle)
  {
    /* Vote to enable sleep */
    sleep_assert_okts(spi_sleep_handle);
  }

  if (spi_npa_bus_handle)
  {
    /* Remove request for hclk */
    npa_complete_request(spi_npa_bus_handle);
  }
}
