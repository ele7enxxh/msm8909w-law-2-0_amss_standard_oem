#ifndef MBA_RMB_H
#define MBA_RMB_H

/**
@file mba_rmb.h
@brief Relay Message Buffer
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/inc/mba_rmb.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
01/11/12   mm       Made Compatible to Virtio 1.1.1 flat file
12/20/11   mm       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "secboot.h"
#include "secboot_hw.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Retrieved from FLat File - ARM_ADDRESS_FILE.FLAT*/
#include "msmhwiobase.h"


/* RMB SHadow Register  2 bit 0 */
/* 0x1 = This is a restart scenario. A cold boot has already occured 
 * since gcc_access_ares reset. 
 */
#define MBA_MODEM_RESTART_BIT 0x1


/* XPU unlocked without scribbling. MSS_SPIDEN is not set. */
#define MBA_XPU_UNLOCKED 0x1
/* XPU unlocked and memory scribbled. MSS_SPIDEN is set */
#define MBA_XPU_UNLOCKED_AND_SCRIBBLED 0x2
/* Authentication of the PMI Meta Data is successful */
#define MBA_PMI_META_DATA_AUTHENTICATION_SUCCESS 0x3
/* Authentication of the ELF Program Segments is complete */
#define MBA_ELF_PROGRAM_SEGMENTS_COMPLETE 0x4
/* All MBA Execution is complete */
#define MBA_ELF_EXECUTION_COMPLETE = 0x5
/* PIL has failed to load the modem and requesting to unlock the mba partition */
#define MBA_PMI_LOAD_FAILURE_PIL_UNLOCK_REQ 0x6
/* MBA Secure PIL Init Failure */
#define MBA_INIT_FAILURE -1
/* MBA Secure PIL Unexpected Command*/
#define MBA_UNEXPECTED_MBA_COMMAND -2
/* MBA Secure PIL Meta Data Authentication Failure */
#define MBA_META_DATA_AUTH_FAILURE -3
/* MBA Secure PIL Code Segment Authentication Failure */
#define MBA_CODE_SEGMENT_AUTH_FAILURE -4
/* MBA Secure PIL Invalid Image Entry Pointer */
#define MBA_IMAGE_ENTRY_POINTER_INVALID -5
/* Error in retrieving the Image Entry */
#define MBA_IMAGE_ENTRY_RETRIEVE_FAILURE -6
/* Error in Deinitialization of MBA */
#define MBA_DEINIT_FAILURE -7
/* PRNG INitialization Failure */
#define MBA_PRNG_INIT_FAILURE -8
/* PIL Initialization failure */
#define MBA_SEC_PIL_INIT_FAILURE -9
/* read mba debug information failed */
#define MBA_READ_RMB_DEBUG_FAILURE -10
/* BAM Initialization Failure */
#define MBA_BAM_INIT_FAILURE -11
/* DEHR Initialization Failure */
#define MBA_DEHR_INIT_FAILURE -12
/* qpic init failure */
#define MBA_QPIC_INIT_FAILURE -13
/* Meta DATA TLB Initialization Failure */
#define MBA_META_DATA_AUTH_TLB_FAILURE -14
/* Image Length failure */
#define MBA_INIT_IMAGE_LEN_FAILURE -15
/* MBA Code Start TLB Failure */
#define MBA_INIT_CODE_START_TLB_FAILURE -16
/* Segmented authentication length failure */
#define MBA_CODE_SEGMENT_AUTH_IMAGE_LEN_FAILURE -17
/* MBA Auth Reset Failure */
#define MBA_CODE_AUTH_RESET_FAILURE -18
/* segmented authentication failure */
#define MBA_CODE_SEGMENT_AUTH_SEGMENT_FAILURE -19
/* Error in Retrieving version of MBA */
#define MBA_SW_VERSION_RETRIEVE_FAILURE -21
/* elf header address range is not in bank zero */
#define MBA_ELF_RANGE_INVALID -22
/* CODE segement address range falls outside the bank0 */
#define MBA_CODE_RANGE_INVALID -23
/* Key Unwrap failure */
#define MBA_KEY_UNWRAP_FAILURE -24
/* MBA decryption/verification failure */
#define MBA_CODE_DECRYPT_VERIFY_FAILURE -25
/* MBA heap entry delete failure */
#define MBA_HEAP_ENTRY_DELETE_FAILURE -26

/* MBA XPU LOCK failure*/
#define MBA_XPU_LOCK_FAILURE -28
/* MBA debug policy meta data authentication failure */
#define MBA_DEBUG_POLICY_META_AUTH_FAILURE -27

#define DBG_POLICY_RMB_FLAG_ONLINE_CRASH_DUMPS     1

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum 
{
  MBA_COMMAND_NOT_SET = 0x0,

  /* PMI Meta Data Ready. This signals the MBA to start the 
   * PMI PIL Authentication 
   */
  MBA_PMI_META_DATA_READY,
  /* PMI Segmented Load Ready. This signals the MBA to start the
   * PMI PIL Authentication 
   */
  MBA_PMI_SEGMENTED_LOAD_READY,
  MBA_PMI_LOAD_FAIL_UNLOCK = 0xFFFFDEAD,
  MBA_COMMAND_MAX
}mba_rmb_command_e_type;

typedef struct mba_rmb_pmi_s
{
  /* Physical address of the beginning of the elf_hdr */
  uint32 elf_hdr;
  /* Physical address of the beginning of the ELF Program Segments */
  uint32 code_start;
  /* Current number of bytes written by the host that will be read by the MBA
   * for PIL segmented authentication
   */
  uint32 code_len;
}mba_rmb_pmi_t;


typedef struct pbl_secboot_shared_info_type
{
  secboot_verified_info_type pbl_verified_info;    /**<  */
  secboot_ftbl_type          pbl_secboot_ftbl;     /**< Contains pointers to PBL secboot routines */
  secboot_hw_ftbl_type       pbl_secboot_hw_ftbl;  /**< Contains pointers to PBL secboot hw routines */
} pbl_secboot_shared_info_type;

typedef struct
{
  uint32 pbl_entry_timestamp;
  uint32 bootable_media_detect_entry_timestamp;
  uint32 bootable_media_detect_success_timestamp;
  uint32 elf_loader_entry_timestamp;
  uint32 auth_hash_seg_entry_timestamp;
  uint32 auth_hash_seg_exit_timestamp;
  uint32 elf_segs_Hash_verify_entry_timestamp;
  uint32 pbl_exit_timestamp;
} pbl_timestamp_type;


/*---------------------------------------------------------------------------
  Defines the data structure that is filled with data that is shared with
  the MBA image. It may be necessary for the MBA to make a deep copy of
  this structure for its own use if it wipes out PBL memory.
---------------------------------------------------------------------------*/

typedef struct pbl_modem_mba_shared_data_type
{
  /* Fields below are filled in by initialization function */
/* Shared data structure version */
uint32 shared_data_version;

  uint32 pbl_modem_version;

  /* Proc clock speed set up in PBL */
  uint32 proc_clk_speed;

  /* The region of TCM required for use by exported PBL routines. */
  uint8 *pbl_modem_mba_shared_section_base;
  uint32 pbl_modem_shared_section_size;

  /* Fields below are filled in by auth module */ 
  pbl_secboot_shared_info_type     pbl_modem_mba_secboot_shared_data;

/*Timestamps at various important PBL routines */
pbl_timestamp_type timestamps;
} pbl_modem_mba_shared_data_type;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
 
 /* Check if the Restart bit Flag in the RMB Shadow Register 0 is set*/
 uint32 mba_rmb_is_cold_boot(void);
 
 /* Pass debug flags to MPSS */
 void mba_rmb_set_debug_flags (uint32 flags);

 /* Set the Modem Jump Address and cold boot occurred flag*/
 void mba_rmb_set_modem_jump_address (uint32 modem_jump_addr);
 
 /* Set Modem SW Version */
 int mba_rmb_set_modem_sw_version (uint32 modem_sw_version);
 
 /* Set MBA SW Version */
 int mba_rmb_set_mba_sw_version(void);
 
 /* Read the MBA Command.  */
 mba_rmb_command_e_type mba_rmb_wait_for_mba_command (void);

 /* Writing the MBA Status */
 void mba_rmb_status_update(int);
 
 /* Writing the MBA Debug Info */
 void mba_rmb_dbg_update(int);
 
 /* Read the MBA Debug Address */
 void* mba_rmb_get_dbg_address(void);
 
 /* Get the modem meta data pointer */
 void mba_rmb_get_metadata_address(mba_rmb_pmi_t * mba_pmi_info);
 
 /* Get the modem code start address and len */
 void mba_rmb_get_code_info (mba_rmb_pmi_t * mba_pmi_info);

 /* during the modem authentication if modem sends unlock command, exit authentication with status 0x6*/
 void mba_check_pil_unlock_req(void);
 
 /* Is this a cold boot */
 void mba_rmb_set_restart_bit(void);
 
 /* Get PBL Shared Data */
 pbl_modem_mba_shared_data_type *mba_rmb_get_pbl_shared_data(void);

 /*  Check the debug setting of MBA */
 int mba_rmb_get_debug_state(uint32* debug_state);

#endif /* MBA_RMB_H */
