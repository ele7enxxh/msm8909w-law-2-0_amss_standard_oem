#ifndef BAMI_H_
#define BAMI_H_

/**
  @file bam_private.h
  @brief
  This file contains definitions of constants, data structures, and
  interfaces for private clients.

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/10/15   PR      Created

===============================================================================
                   Copyright (c) 2015 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/**
 *  @brief bam_pipe_reassign
 *
 * This API will provide dynamic configuration change of EE for BAM pipe.
 *
 * @param[in] bamhandle     Pointer to the handle to the BAM instance.
 * @param[in] ee            New Expected EE for BAM pipe.
 * @param[in] pipemask      Mask of BAM pipe needs EE to be updated.
 *
 *
 * @return Status of the operation.
 *
 */
bam_status_type bam_pipe_reassign(bam_handle bamhandle, uint32 ee, uint32 pipemask);

/** @}  - bam_func_api */

#endif /* BAMI_H_ */
