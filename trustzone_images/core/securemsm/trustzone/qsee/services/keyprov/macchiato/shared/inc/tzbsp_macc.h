#ifndef MACCHIATO_H
#define MACCHIATO_H

/** @file tzbsp_macc.h
 * @brief
 * This file contains the definitions of the constants, data structures
 * and interfaces to the QSEE Macchiato key provisioning lib.
 */

/*===========================================================================
 Copyright (c) 2014 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/14   cz      Added tzbsp_macc_calc_segs_hash to calculate TA hash
12/11/14   cz      Initial Version.

===========================================================================*/
#include "tzbsp_pil.h"
#include "tzbsp_hash.h"

/* defined in tz_macc_keygen.c */
extern uint8 global_tz_app_img_hash[TZBSP_SHA256_HASH_SZ];


/***********************************************************************************************
 * External APIs declaration
 *
 * This header file includes all APIs that need to be called by other components in TZ Kernel
 *
 ***********************************************************************************************/
 
 
 
/**
 * @brief Calculate the digest on the segment hashes.
 *
 * @param[in] elf The ELF header of the loaded image
 * @return 0 on success, negative on failure
 */
int32 tzbsp_macc_calc_segs_hash(tz_pil_proc_e_type proc, const tzbsp_elf_info_t *elf);


#endif /* MACCHIATO_H */
