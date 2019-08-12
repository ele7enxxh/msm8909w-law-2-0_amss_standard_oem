/**
@file debug_policy_private.h
@brief Debug policy implementation definitions/routines

This file defines a private definition of debug policy implementation

*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secdbgplcy/inc/debug_policy_private.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2014/07/08     st          Initial version
=============================================================================*/

#ifndef DEBUG_POLICY_H
#define DEBUG_POLICY_H

#include "sec_pil_util.h"

/**
 * @brief 
 *        Check whether the current image should be authenticated against the debug policy roots
 *
 * @param[in] sw_type is the current image ID being authenticated.
 *                @retval TRUE if the image should be authenticated against the debug policy, FALSE otherwise
 *
 */
boolean is_dbg_policy_rot_for_image( uint32 sw_type );

/**
 * @brief
 * When TZ is initialized we can no longer access the policy
 * from the shared region. We therefore need to keep a local
 * copy. Before copying, verify it.
 *
 * @param[in] dp The debug policy
 * @retval TRUE if the policy is valid, bound and was copied locally
*/
boolean copy_debug_policy(dbg_policy_t* dp);

/**
 * @brief
 * Process the debug policy and apply any relevant settings
 */
void apply_dbg_policy();

/**
* @brief
 * Return the debug policy
 * @retval NULL if the debug policy has not been validated
*/
dbg_policy_t* get_dbg_policy();

/**
 * @brief 
 *        Check whether a feature flag is set in the debug policy.
 *
 * @param[in] flag The feature flag in question
 * @retval    TRUE if the flag is set, FALSE otherwise
 *
 */
boolean is_debug_policy_flag_set ( uint32 flag );

/*!
* 
* @brief
*    This function checks if debug policy ELF file passes basic sanity checks
*
* @retval
* @param[in/out]  TRUE   The debug policy is valid and is OK to enable it.
*                 FALSE  The debug policy contains bogus information. Disable the policy.
 *
 */
boolean is_dbg_policy_valid(dbg_policy_t *dbgp_ptr);

boolean is_sec_dbg_skip_serial_number(void);

#endif
