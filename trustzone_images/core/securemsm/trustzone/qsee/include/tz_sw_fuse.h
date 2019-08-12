#ifndef TZ_SW_FUSE_H
#define TZ_SW_FUSE_H

/**
 * @file tz_sw_fuse.h
 * @brief Device rooting detection
 *
 * These APIs are used by APPSBL to verify the HLOS image and
 * communicate that information to any HLOS driver who wants to query
 * for it
 */

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The TZ_SysCall_API_mainpage.dox file contains all descriptions that 
      are displayed in the output PDF generated using Doxygen and LaTeX. 
      To edit or update any of the file/group text in the PDF, edit the 
      TZ_SysCall_API_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
   Copyright (c) 2011-2013 by Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tz_sw_fuse.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/08/13   leo      (Tech Pubs) Edited/added Doxygen comments and markup.
08/10/11   pre      Initial version.

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
/** @ingroup tz_datatypes
    List of software fuses supported by the QSEE.
 */
typedef enum
{
  TZ_HLOS_IMG_TAMPER_FUSE   = 0, /**< Used during boot to determine
                                        if the HLOS image has successfully
                                        been authenticated. */
  TZ_WINSECAPP_LOADED_FUSE  = 1, /**< Used by the WinSec application to
                                        prevent reloading. */
  TZ_UEFISECAPP_LOADED_FUSE = 2, /**< Used by UefiSecApp to prevent
                                        reloading. */
  TZ_OEM_FUSE_1             = 3, /**< 8 reserved fuse bits for OEMs */

  TZ_OEM_FUSE_2             = 4, 

  TZ_OEM_FUSE_3             = 5, 

  TZ_OEM_FUSE_4             = 6, 

  TZ_OEM_FUSE_5             = 7, 

  TZ_OEM_FUSE_6             = 8, 

  TZ_OEM_FUSE_7             = 9, 

  TZ_OEM_FUSE_8             = 10, 
                                     
  TZ_NUM_SW_FUSES           = 11  /**< Number of supported software fuses.
                                      @newpage */
} tz_sw_fuse_t;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/** @cond */

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
int tz_is_sw_fuse_blown(tz_sw_fuse_t fuse_num,
                           boolean*        is_blown,
                           uint32          is_blown_sz);

/**
 * @brief      Query whether the given SW fuse has been blown. 
 *             Should be called only from secure side
 *
 * @param[in]  fuse_num    the SW fuse to query
 * @param[out] is_blown    whether the given fuse has been blown
 * @param[in]  is_blown_sz size of the return pointer
 *
 * @return     0 on success, negative on failure
 * @warning    function is not thread-safe
 */
int tz_is_sw_fuse_blown_secure(tz_sw_fuse_t fuse_num,
                                  boolean*        is_blown,
                                  uint32          is_blown_sz);


/**
 * @brief     Change the state of the SW fuse
 * @param[in] fuse_num the SW fuse to query
 * @return    0 on success, negative on failure
 */
int tz_blow_sw_fuse(tz_sw_fuse_t fuse_num);

/** @endcond */


#endif /* TZ_SW_FUSE_H */
