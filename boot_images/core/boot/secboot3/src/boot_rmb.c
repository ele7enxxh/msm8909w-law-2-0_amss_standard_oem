/*=============================================================================

                              Boot Loader

GENERAL DESCRIPTION
  This module Supports communicating to subsytem image over rmb registers.

Copyright 2012 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_rmb.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/14   jz         Cleaned up the interface of boot_rmb_set_mba_image_addr
08/26/14   jz         Updated the interface of boot_rmb_set_pmi_code_start_addr
07/17/14   jz         Added BOOT_PRE_SILICON to skip RMB status check on RUMI
08/29/13   kedara     Temporarily turn off mba status check.
09/28/12   kedara     Flush cache before communicating with subsys Image.
09/20/12   kedara     Update Conditional checks.
08/16/12   kedara     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_loader.h"
#include BOOT_PBL_H
#include "boot_rmb.h"
#include "boot_cache_mmu.h"


#ifndef MBA_META_DATA_BUFF_ADDR
/* 128kb below end of modem image */
#define MBA_META_DATA_BUFF_ADDR 0x075E0000
#endif

#ifdef BOOT_PRE_SILICON
/*Skip all MBA image handshake status checks on RUMI */
static boolean boot_rmb_status_chk_skip = TRUE;
#else
static boolean boot_rmb_status_chk_skip = FALSE;
#endif

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  boot_rmb_get_meta_info_buffer
** ==========================================================================
*/
/*!
* 
* @brief
*  This function returns pointer to meta info buffer. The buffer shall
*  contain the following information about elf image, expected by self
*  authenticating subsystem:
*  "elf header" followed by "program header table" then "hash segment"
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  uint32 *  - Pointer to elf meta information buffer
* 
*/
uint8 *boot_rmb_get_meta_info_buffer ( void )
{
  int32 rmb_status = 0x0;
  
  /* Wait for MBA image to unlock XPU. Do not proceed meta info population
    till MBA is ready
  */
    rmb_status =(int32)HWIO_IN(RMB_MBA_STATUS);
  while((rmb_status == RMB_REG_UNINITIALIZED) && !boot_rmb_status_chk_skip)
  {
    rmb_status =(int32)HWIO_IN(RMB_MBA_STATUS);
  }

  /* TO-DO: ensure mba sets these status's.*/
  BL_VERIFY(((MBA_XPU_UNLOCKED == rmb_status) ||
             (MBA_XPU_UNLOCKED_MEM_SCRIBBLED == rmb_status) ||
             (boot_rmb_status_chk_skip)),
             BL_ERR_SUBSYS_BOOT_FAIL );
  
  
  return (uint8 *)MBA_META_DATA_BUFF_ADDR ;
}

/*===========================================================================
**  Function :  boot_rmb_populate_meta_info
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys authenticator image (eg MBA image)
*  the location of elf meta information buffer.
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
void boot_rmb_populate_meta_info ( void )
{
  /* Flush cache  */
  mmu_flush_cache();

  /* Set RMB_PMI_META_DATA with the physical address of the meta-data */
  HWIO_OUT(RMB_PMI_META_DATA, (uint32)MBA_META_DATA_BUFF_ADDR);

  /* Set the RMB_MBA_COMMAND to 0x1 to start the Meta Data
     authentication/validation.
  */
  HWIO_OUT(RMB_MBA_COMMAND, (uint32)CMD_PMI_META_DATA_READY);

  /* RMB_MBA_STATUS value MBA_PMI_META_DATA_AUTHENTICATION_SUCCESS indicates
    success retrieving meta data by subsys image, if CMD_PMI_META_DATA_READY 
    goes ok. Not necessary to wait for it.
  */
}

/*===========================================================================
**  Function :  boot_rmb_set_pmi_code_start_addr
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys authenticator image (eg MBA image)
*  the start of first elf segment.
*
* @param[in] 
*  uintnt  pmi_code_start_addr  - start of first elf segment
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
void boot_rmb_set_pmi_code_start_addr ( uintnt  pmi_code_start_addr )
{
  /* Flush cache  */
  mmu_flush_cache();

  /*
    Program the RMB_PMI_CODE_START to the start address of the first
    loadable segment.
  */
  HWIO_OUT(RMB_PMI_CODE_START, pmi_code_start_addr);
}

/*===========================================================================
**  Function :  boot_rmb_set_pmi_code_length
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys authenticator image (eg MBA image)
*  the loaded elf length.
*
* @param[in] 
*  uint32  pmi_code_length - length of loaded elf
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
void boot_rmb_set_pmi_code_length ( uint32 pmi_code_length )
{
  /* Flush cache before Updating pmi length */
  mmu_flush_cache();
  
  /* Set RMB_PMI_CODE_LENGTH to whole modem image length  */
  HWIO_OUT(RMB_PMI_CODE_LENGTH, (uint32)pmi_code_length);
}  

/*===========================================================================
**  Function :  boot_rmb_set_pmi_elf_load_ready
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys authenticator image (eg MBA image)
*  the  start of elf load.
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
void boot_rmb_set_pmi_elf_load_ready ( void )
{
  /* Flush cache  */
  mmu_flush_cache();

  /* Set the RMB_MBA_COMMAND to 0x2 to start the PMI
    authentication/validation.
  */
  HWIO_OUT(RMB_MBA_COMMAND, (uint32)CMD_PMI_SEGMENTED_LOAD_READY);

  /*
    RMB_MBA_STATUS of MBA_EXECUTE_SUCCESS indicates MBA executed successfully
    and was able to jump to modem image. This status can be checked at end
    of boot. 
  */
} 

/*===========================================================================

**  Function :  boot_rmb_check_subsys_status

** ==========================================================================
*/
/*!
* 
* @brief
*  This function Checks if subsystem PBL and auth(eg MBA) images are able to
*  boot successfuly.
* 
* @param[in] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void boot_rmb_check_subsys_status( void )
{
  int32 subsys_boot_status = 0;

  /* Check if subsys PBL was able to jumb to authenticator image (MBA)
    successfully via rmb regiter 01, ie RMB_PBL_STATUS.
  */
  subsys_boot_status = (int32)HWIO_IN(RMB_PBL_STATUS);
  
  /* A negative value indicates error */
  BL_VERIFY( ((0 <= subsys_boot_status) ||
            (boot_rmb_status_chk_skip)) , BL_ERR_SUBSYS_BOOT_FAIL);

  /* Check if MBA image executes successfully via rmb regiter 03,
    ie RMB_MBA_STATUS.
  */
  subsys_boot_status = (int32)HWIO_IN(RMB_MBA_STATUS);

   /* A negative value indicates error */
  BL_VERIFY(((0 <= subsys_boot_status) ||
              (boot_rmb_status_chk_skip)) , BL_ERR_SUBSYS_BOOT_FAIL );
}

/*===========================================================================
**  Function :  boot_rmb_set_mba_image_addr
** ==========================================================================
*/
/*!
* 
* @brief
*  This function signals subsys PBL image, the MBA image start address.
*
* @param[in] 
*  uint32  mba_image_addr  - loaded MBA image base address.
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*  None
* 
*/
void boot_rmb_set_mba_image_addr ( uint32  mba_image_addr )
{
  /* Flush cache  */
  mmu_flush_cache();

  /* Communicate MBA image start address to modem pbl via RMB register 0,
    ie RMB_MBA_IMAGE
  */
   HWIO_OUT(RMB_MBA_IMAGE, mba_image_addr);
}
