/**********************************************************************
 * flash_clocks.c
 *
 * SERVICES:    QPIC/PNOC clock and SPI NOR related node voting management
 *
 * DESCRIPTION: Provides flash operation clocks voting for NAND/SPI NOR
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2013-2014, 2016 QUALCOMM  Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================
 *
 *                   EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_clocks.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 * when         who     what, where, why
 * --------     ---     ----------------------------------------------
 * 06/27/16     sb      Add syncronization for clock request/complete APIs
 * 09/30/14     sb      NPA handle for PCNOC on Bear family
 * 05/23/13     eo      Initial Version
 *===================================================================*/

 /*===================================================================

===========================================================================*/

#include "core_variation.h"
#include "flash.h"

#include "flash_clocks.h"
#include "flash_dal_config.h"

#include "npa.h"

/* Temporarily disable NPA votes */
#define FEATURE_SPI_DISABLE_NPA_VOTE

/* CPU clock request for 288 MIPS */
#ifndef SPI_NOR_CPU_REQ_MIPS
  #define SPI_NOR_CPU_REQ_MIPS 288
#endif

/* Init PNOC and QPIC clock source request of 100MHZ */ 
#define FLASH_NAND_NPA_PNOC_KHZ 100000
#define FLASH_NAND_NPA_QPIC_KHZ 100000

/* NPA handles for PNOC and QPIC clock voting */
static npa_client_handle flash_nand_npa_pnoc_handle = NULL;
static npa_client_handle flash_nand_npa_qpic_handle = NULL;

/* Global provides ability to adjust value in t32 for tuning and testing */
uint32 flash_pnoc_clock_request = FLASH_NAND_NPA_PNOC_KHZ;
uint32 flash_qpic_clock_request = FLASH_NAND_NPA_PNOC_KHZ;
  
/* NPA handle for latency */
static npa_client_handle spi_npa_latency_handle = NULL;

/* NPA handle for CPU voting */
static npa_client_handle spi_npa_cpu_handle = NULL;

/* Enable NPA clock voting */
static uint32 flash_npa_enable_vote = FALSE;

/* CPU clock request value */
uint32 spi_clock_request = SPI_NOR_CPU_REQ_MIPS;

/* Counter for flash_clocks_requests without an equivalent */
uint32 flash_clocks_req_counter = 0;

/* Flash Operations Clocks init */
void flash_clocks_init(void)
{
  /* Check which device driver type is active */
  if (flash_dal_config.config == FLASH_DAL_CONFIG_NAND)
  {
    /* check if flash nand npa pnoc handle exists */
    if (!flash_nand_npa_pnoc_handle)
    {
      /* Create this client's NPA handle for PNOC requests only once.
       * NPA resource release by client should be done by invoking
       * npa_complete_request() API.
       */
      flash_nand_npa_pnoc_handle = npa_create_sync_client(
        "/clk/pcnoc", "FLASH-PNOC", NPA_CLIENT_REQUIRED);
    }
    if (!flash_nand_npa_qpic_handle)
    {
      /* Create this client's NPA handle for QPIC clock requests only once.
       * NPA resource release by client should be done by invoking
       * npa_complete_request() API.
       */
      flash_nand_npa_qpic_handle = npa_create_sync_client(
        "/clk/qpic", "FLASH-QPIC", NPA_CLIENT_REQUIRED);
    }
  }
  else if (flash_dal_config.config == FLASH_DAL_CONFIG_NOR)
  {
    /* check if spi npa latency handle exists */
    if (!spi_npa_latency_handle)
    {
      /* Create this client's NPA handle for latency requests only once.
       * NPA resource release by client should be done by invoking
       * npa_complete_request() API.
       */
      spi_npa_latency_handle = npa_create_sync_client(
        "/core/cpu/latency", "SPI_NOR", NPA_CLIENT_REQUIRED);
    }

    /* check if spi npa cpu handle exists */
    if (!spi_npa_cpu_handle)
    {
      /* Create this client's NPA handle for cpu only once.
       * NPA resource release by client should be done by invoking
       * npa_complete_request() API.
       */
#ifndef FEATURE_SPI_DISABLE_NPA_VOTE
      spi_npa_cpu_handle = npa_create_sync_client(
        "/core/cpu", "SPI_NOR", NPA_CLIENT_REQUIRED);
#endif
    }
  }
}

/* Flash Operations Clocks deinit */
void flash_clocks_deinit(void)
{
  /* Check which device driver type is active */
  if (flash_dal_config.config == FLASH_DAL_CONFIG_NAND)
  {
    /* check if npa pnoc handle is registered */
    if (flash_nand_npa_pnoc_handle)
    {
      npa_destroy_client(flash_nand_npa_pnoc_handle);
      flash_nand_npa_pnoc_handle = NULL;
    }
    /* check if npa qpic handle is registered */
    if (flash_nand_npa_qpic_handle)
    {
      npa_destroy_client(flash_nand_npa_qpic_handle);
      flash_nand_npa_qpic_handle = NULL;
    }
  }
  else if (flash_dal_config.config == FLASH_DAL_CONFIG_NOR)
  {
    /* check if spi npa latency handle is registered and if so, 
     * deregister it */
    if (spi_npa_latency_handle)
    {
      npa_destroy_client(spi_npa_latency_handle);
      spi_npa_latency_handle = NULL;
    }
  
    /* check if spi npa cpu handle is registered and if so, deregister
     * it */
    if (spi_npa_cpu_handle)
    {
      npa_destroy_client(spi_npa_cpu_handle);
      spi_npa_cpu_handle = NULL;
    }
  }
}

/* Enter vote for QPIC and PNOC clocks for flash before
 * begining of flash operations.
 */
void flash_clocks_request(void)
{
  if ((flash_clocks_req_counter == 0) && (flash_npa_enable_vote == FALSE))
  {
    /* Check which device driver type is active */
    if (flash_dal_config.config == FLASH_DAL_CONFIG_NAND)
    {
      if (flash_nand_npa_pnoc_handle && flash_nand_npa_qpic_handle)
      {
        /* Request pnoc clock for flash nand transfer */
        npa_issue_required_request(flash_nand_npa_pnoc_handle,
          flash_pnoc_clock_request);
        /* Request qpic clock for flash nand transfer */
        npa_issue_required_request(flash_nand_npa_qpic_handle,
          flash_qpic_clock_request);
      }
    }
    else if (flash_dal_config.config == FLASH_DAL_CONFIG_NOR)
    {
      /* Request for a latency of a minimum value so that there wont be any
       * latency in processing the SPI interrupts 
       */
      if (spi_npa_latency_handle)
      {
        /* Request for max allowable latency of 1 sclk */
        npa_issue_required_request(spi_npa_latency_handle, 1);
      }

      if (spi_npa_cpu_handle)
      {
        /* Request for CPU to prevent the clocks from scaling down */
        npa_issue_required_request(spi_npa_cpu_handle, spi_clock_request);
      }
    }
    flash_npa_enable_vote = TRUE;
  }
  flash_clocks_req_counter++;
}

/* Release vote for QPIC and PNOC clocks for flash at end of 
 * flash operations.
 */
void flash_clocks_request_complete(void)
{
  flash_clocks_req_counter--;
  
  if ((flash_clocks_req_counter == 0) && (flash_npa_enable_vote == TRUE))
  { 
    /* Check which device driver type is active */
    if (flash_dal_config.config == FLASH_DAL_CONFIG_NAND)
    {
      if (flash_nand_npa_pnoc_handle && flash_nand_npa_qpic_handle)
      {
        /* Release qpic clock vote after flash nand transfer*/
        npa_complete_request(flash_nand_npa_qpic_handle);
        
        /* Release pnoc clock vote after flash nand transfer*/
        npa_complete_request(flash_nand_npa_pnoc_handle);
      }
    }  
    else if (flash_dal_config.config == FLASH_DAL_CONFIG_NOR)
    {
      if (spi_npa_latency_handle)
      {
        /* complete request for latency so that there is no restriction on 
         * latency 
         */
        npa_complete_request(spi_npa_latency_handle);
      }

      if (spi_npa_cpu_handle)
      {
        /* Complete NPA request for CPU */
        npa_complete_request(spi_npa_cpu_handle);
      }
    }
    flash_npa_enable_vote = FALSE;
  }
}
