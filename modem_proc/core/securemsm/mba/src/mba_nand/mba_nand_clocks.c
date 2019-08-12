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
 * Copyright (c) 2013-2014 QUALCOMM  Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================
 *
 *                   EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/src/mba_nand/mba_nand_clocks.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 * when         who     what, where, why
 * --------     ---     ----------------------------------------------
 * 09/30/14     sb      NPA handle for PCNOC on Bear family
 * 05/23/13     eo      Initial Version
 *===================================================================*/

 /*===================================================================

===========================================================================*/

#include "mba_nand_clocks.h"

#include "npa.h"

/* Temporarily disable NPA votes */
#define FEATURE_SPI_DISABLE_NPA_VOTE

/* Init PNOC and QPIC clock source request of 100MHZ */ 
#define FLASH_NAND_NPA_PNOC_KHZ 100000
#define FLASH_NAND_NPA_QPIC_KHZ 100000

/* NPA handles for PNOC and QPIC clock voting */
static npa_client_handle flash_nand_npa_pnoc_handle_mba = NULL;
static npa_client_handle flash_nand_npa_qpic_handle_mba = NULL;

/* Global provides ability to adjust value in t32 for tuning and testing */
uint32 flash_pnoc_clock_request_mba = FLASH_NAND_NPA_PNOC_KHZ;
uint32 flash_qpic_clock_request_mba = FLASH_NAND_NPA_PNOC_KHZ;
  
/* Enable NPA clock voting */
static uint32 flash_npa_enable_vote_mba = TRUE;


/* Counter for flash_clocks_requests without an equivalent */
uint32 flash_clocks_req_counter_mba = 0;


/* Flash Operations Clocks init */
void mba_flash_clocks_init(void)
{
  
    /* check if flash nand npa pnoc handle exists */
    if (!flash_nand_npa_pnoc_handle_mba)
    {
      /* Create this client's NPA handle for PNOC requests only once.
       * NPA resource release by client should be done by invoking
       * npa_complete_request() API.
       */
      flash_nand_npa_pnoc_handle_mba = npa_create_sync_client(
        "/clk/pcnoc", "FLASH-MBA-PNOC", NPA_CLIENT_REQUIRED);
    }
    if (!flash_nand_npa_qpic_handle_mba)
    {
      /* Create this client's NPA handle for QPIC clock requests only once.
       * NPA resource release by client should be done by invoking
       * npa_complete_request() API.
       */
      flash_nand_npa_qpic_handle_mba = npa_create_sync_client(
        "/clk/qpic", "FLASH-MBA-QPIC", NPA_CLIENT_REQUIRED);
    }
}

/* Flash Operations Clocks deinit */
void mba_flash_clocks_deinit(void)
{
    /* check if npa pnoc handle is registered */
    if (flash_nand_npa_pnoc_handle_mba)
    {
      npa_destroy_client(flash_nand_npa_pnoc_handle_mba);
      flash_nand_npa_pnoc_handle_mba = NULL;
    }
    /* check if npa qpic handle is registered */
    if (flash_nand_npa_qpic_handle_mba)
    {
      npa_destroy_client(flash_nand_npa_qpic_handle_mba);
      flash_nand_npa_qpic_handle_mba = NULL;
    }
}

/* Enter vote for QPIC and PNOC clocks for flash before
 * begining of flash operations.
 */
void mba_flash_clocks_request(void)
{
  if (flash_clocks_req_counter_mba == 0)
  {
      if (flash_nand_npa_pnoc_handle_mba && flash_nand_npa_qpic_handle_mba &&
        (flash_npa_enable_vote_mba == TRUE))
      {
        /* Request pnoc clock for flash nand transfer */
        npa_issue_required_request(flash_nand_npa_pnoc_handle_mba,
          flash_pnoc_clock_request_mba);
        /* Request qpic clock for flash nand transfer */
        npa_issue_required_request(flash_nand_npa_qpic_handle_mba,
          flash_qpic_clock_request_mba);
      }
  }
  flash_clocks_req_counter_mba++;
}

/* Release vote for QPIC and PNOC clocks for flash at end of 
 * flash operations.
 */
void mba_flash_clocks_request_complete(void)
{
  flash_clocks_req_counter_mba--;
  
  if (flash_clocks_req_counter_mba == 0)
  { 
      if (flash_nand_npa_pnoc_handle_mba && flash_nand_npa_qpic_handle_mba &&
        (flash_npa_enable_vote_mba == TRUE))
      {
        /* Release qpic clock vote after flash nand transfer*/
        npa_complete_request(flash_nand_npa_qpic_handle_mba);
        
        /* Release pnoc clock vote after flash nand transfer*/
        npa_complete_request(flash_nand_npa_pnoc_handle_mba);
      }  
  }
}
