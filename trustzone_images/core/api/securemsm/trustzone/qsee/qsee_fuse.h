#ifndef QSEE_FUSE_H
#define QSEE_FUSE_H

/**
@file qsee_fuse.h
@brief Provide HW & SW Fuse functionality
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_fuse.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
10/03/11    rv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * Enumeration for the SW fues TZ supports
 */
typedef enum
{
  QSEE_HLOS_IMG_TAMPER_FUSE = 0, /**< Used during boot to determine
                                        if the HLOS image has successfully
                                        been authenticated. */
  QSEE_WINSECAPP_LOADED_FUSE = 1, /**< Used by WinSec App to prevent
                                        reloading. */
  QSEE_UEFISECAPP_LOADED_FUSE = 2, /**< Used by UefiSecApp to prevent
                                        reloading. */
  QSEE_OEM_FUSE_1             = 3, /**< 8 reserved fuse bits for OEMs */

  QSEE_OEM_FUSE_2             = 4, 

  QSEE_OEM_FUSE_3             = 5, 

  QSEE_OEM_FUSE_4             = 6, 

  QSEE_OEM_FUSE_5             = 7, 

  QSEE_OEM_FUSE_6             = 8, 

  QSEE_OEM_FUSE_7             = 9, 

  QSEE_OEM_FUSE_8             = 10, 
                                     
  QSEE_NUM_SW_FUSES           = 11  /**< Number of supported 
                                        software fuses. */
} qsee_sw_fuse_t;

/**
 * @brief      Query whether the given SW fuse has been blown
 *
 * @param[in]  fuse_num    the SW fuse to query
 * @param[out] is_blown    whether the given fuse has been blown
 * @param[in]  is_blown_sz size of the return pointer
 *
 * @return     0 on success, negative on failure
 * @warning    function is not thread-safe
 */
int qsee_is_sw_fuse_blown
(
  qsee_sw_fuse_t    fuse_num,
  boolean*          is_blown,
  uint32            is_blown_sz
);

/**
 * @brief     Change the state of the SW fuse
 * @param[in] fuse_num the SW fuse to query
 * @return    0 on success, negative on failure
 */
int qsee_blow_sw_fuse(qsee_sw_fuse_t fuse_num);



/**
 * @brief     QFPROM write row
 * @param[in] raw_row_address
 * @param[in] row_data
 * @param[in] bus_clk_khz
 * @param[in] qfprom_api_status
 * @return    0 on success, negative on failure
 */
int qsee_fuse_write
(
  uint32    raw_row_address,
  uint32*   row_data,
  uint32    bus_clk_khz,
  uint32*   qfprom_api_status
);

/**
 * @brief     QFPROM read row
 * @param[in] row_address
 * @param[in] addr_type
 * @param[in] row_data
 * @param[in] qfprom_api_status
 * @return    0 on success, negative on failure
 */
int qsee_fuse_read
(
  uint32   row_address,
  int32    addr_type,
  uint32*  row_data,
  uint32*  qfprom_api_status
);

#endif /* QSEE_FUSE_H */

