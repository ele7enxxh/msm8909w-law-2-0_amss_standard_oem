#ifndef TIME_REMOTE_MM_FUSION_H
#define TIME_REMOTE_MM_FUSION_H

/**
@file time_remote_mm_fusion.h
@brief
This module contains contains functions to allow remote fusion calls.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_remote group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2010 - 2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_remote_mm_fusion.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
date       xxx     Initial version.
===========================================================================*/


#include "time_types.h"
#include "time_remote_reg.h"


/** @addtogroup time_remote
@{ */

/**
Sends an update from the 9K target to the MSM<sup>&reg;</sup> 7x30.

@param[in] jul_time Pointer to the timestamp to which the TOD is set.

@return
None.

@dependencies
None.
*/
extern void time_remote_mm_set_julian
(
  time_julian_type                *jul_time
);
/*~ FUNCTION time_remote_mm_set_julian */
/*~ PARAM jul_time POINTER  */

/** @addtogroup time_remote
@{ */

/**
Sends an update from the 9K target to the MSM<sup>&reg;</sup> 7x30.

@param[in] 
base - base to be updated
jul_time Pointer to the timestamp to which the TOD is set.

@return
None.

@dependencies
None.
*/
extern void time_remote_mm_set_time_bases
(
  time_bases_type                 base,
  /* Timestamp to set time of day to */
  time_julian_type                *jul_time
);
/*~ FUNCTION time_remote_mm_set_time_bases */
/*~ PARAM jul_time POINTER  */


/**
Allows the client to register itself with the API.

@param[in] client_type       Type of the client to register.
@param[in,out] client_id_ptr Pointer to the ID of the client to register.
@param[in] processor         Processor type of the client.

@return
Error codes for the client.

@dependencies
None.

@sideeffects
Internal data is initialized for the new client.
*/
extern ats_client_err_e_type ats_mm_client_init
(
  ats_mm_client_type_e_type   client_type,
  ats_client_id_type          *client_id_ptr,
  ats_client_processor_e_type  processor
);
/*~ FUNCTION ats_mm_client_init 
    RELEASE_FUNC ats_mm_client_release(*client_id_ptr) 
    ONERROR return ATS_CLIENT_ERR_RPC */
/*~ PARAM INOUT client_id_ptr POINTER */


/**
Allows the client to release itself from the API.

@param[in] client_id ID of the client to be released.

@return
Error codes for the client.

@dependencies
None.

@sideeffects
Internal data is reset for this client.
*/
extern ats_client_err_e_type ats_mm_client_release
(
  ats_client_id_type       client_id
);
/*~ FUNCTION ats_mm_client_release 
    ONERROR return ATS_CLIENT_ERR_RPC */


/**
Allows the client to register its configuration event callback function. When a
NULL pointer is passed, the callback is deregistered.
 
@param[in] client_id    ID of the client that wants to register the callback.
@param[in] cfg_event_cb Configuration callback function to be registered.

@return
Error codes for the client.

@dependencies
None.

@sideeffects
Internal data is updated.
*/
extern ats_client_err_e_type ats_mm_client_reg_cfg_cb
(
  ats_client_id_type           client_id,
  ats_mm_cfg_event_cb_type    cfg_event_cb
);
/*~ FUNCTION ats_mm_client_reg_cfg_cb 
    ONERROR return ATS_CLIENT_ERR_RPC */

/** @} */ /* end_addtogroup time_remote */

#endif /* TIME_REMOTE_MM_FUSION_H */