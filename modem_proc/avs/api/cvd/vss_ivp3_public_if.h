#ifndef __VSS_IVP3_PUBLIC_IF_H__
#define __VSS_IVP3_PUBLIC_IF_H__

/**
  @file vss_ivp3_public_if.h
  @brief This file contains the APR API definitions for CVD VP3.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_ivp3_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14    sw     (Tech Pubs) Updated with Doxygen comments from 2.0 and 2.2.

===========================================================================*/


#include "apr_comdef.h"


/** @cond internal */

/****************************************************************************
 * VP3 APIS                                                                 *
 ****************************************************************************/

/** @ingroup cvd_vp3_cmd_get_size
  Gets the size of the VP3 data.

  @note1hang This is a Qualcomm proprietary API and is not intended for
             public use.

  @apr_msgpayload
  None.

  @return
  #VSS_IVP3_RSP_GET_SIZE -- Success.
  @par
  APRV2_IBASIC_RSP_RESULT -- Failure (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/ /* Q2 = Async Packet Router API for MDM9x15 Interface Spec (80-N1463-2)
*/
#define VSS_IVP3_CMD_GET_SIZE ( 0x00012EDD )


/** @ingroup cvd_vp3_resp_get_size
  Response to the #VSS_IVP3_CMD_GET_SIZE command.

  @note1hang This is a Qualcomm proprietary API and is not intended for
             public use.

  @apr_msgpayload{vss_ivp3_rsp_get_size_t}
  @wktable{weak__vss__ivp3__rsp__get__size__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IVP3_RSP_GET_SIZE ( 0x00012EDE )

/* Type definition for vss_ivp3_rsp_get_size_t. */
typedef struct vss_ivp3_rsp_get_size_t vss_ivp3_rsp_get_size_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivp3_rsp_get_size_t
@{ */
/* Payload structure for the VSS_IVP3_RSP_GET_SIZE command response.
*/
struct vss_ivp3_rsp_get_size_t
{
  uint32_t size;
    /**< Size in bytes of the VP3 data. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivp3_rsp_get_size_t */


/** @ingroup cvd_vp3_cmd_get_data
  Gets the VP3 data.

  @note1hang This is a Qualcomm proprietary API and is not intended for
             public use.

  @apr_msgpayload{vss_ivp3_cmd_get_data_t}
  @wktable{weak__vss__ivp3__cmd__get__data__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  Before retrieving VP3 data, the client must first obtain the size of the
  VP3 data via the #VSS_IVP3_CMD_GET_SIZE command. The client cannot assume
  the VP3 size will remain unchanged throughout the lifetime of a session of
  the module exposing this interface; the client must query the VP3 size
  before every VP3 data retrieval.
*/
#define VSS_IVP3_CMD_GET_DATA ( 0x00012EDF )

/* Type definition for vss_ivp3_cmd_get_data_t. */
typedef struct vss_ivp3_cmd_get_data_t vss_ivp3_cmd_get_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivp3_cmd_get_data_t
@{ */
/* Payload structure for the VSS_IVP3_CMD_GET_DATA command.
*/
struct vss_ivp3_cmd_get_data_t
{
  uint32_t mem_handle;
  /**< Handle to the shared memory where the VP3 data is returned.

       For more information, see Section @xref{hdr:ShMemGuidelines}. */

  uint64_t mem_address;
  /**< Location where the VP3 data is returned.

       For information on how the address is intrepreted, see
       Section @xref{hdr:ShMemGuidelines}. */

  uint32_t mem_size;
  /**< Size of the memory in bytes where the VP3 data is returned.

       The size must meet the requirements specified in
       Section @xref{hdr:ShMemGuidelines}.

       The size must also be large enough to accommodate the VP3 data as
       obtained via the #VSS_IVP3_CMD_GET_SIZE command. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivp3_cmd_get_data_t */


/** @ingroup cvd_vp3_cmd_set_data
  Sets the VP3 data.

  @note1hang This is a Qualcomm proprietary API and is not intended for
             public use.

  @apr_msgpayload{vss_ivp3_cmd_set_data_t}
  @wktable{weak__vss__ivp3__cmd__set__data__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  VP3 data cannot be set while any module exposing this interface is in the Run
  state.
  @par
  The maximum supported VP3 data size is 1024 bytes.
*/
#define VSS_IVP3_CMD_SET_DATA ( 0x00012EE0 )

/* Type definition for vss_ivp3_cmd_set_data_t. */
typedef struct vss_ivp3_cmd_set_data_t vss_ivp3_cmd_set_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivp3_cmd_set_data_t
@{ */
/* Payload structure for the VSS_IVP3_CMD_SET_DATA command.
*/
struct vss_ivp3_cmd_set_data_t
{
  uint32_t mem_handle;
  /**< Handle to the shared memory where the VP3 data is provided.

       For more information, see Section @xref{hdr:ShMemGuidelines}. */

  uint64_t mem_address;
  /**< Location where the VP3 data is provided.

       For information on how the address is intrepreted, see
       Section @xref{hdr:ShMemGuidelines}. */

  uint32_t mem_size;
  /**< Size of the memory in bytes where the VP3 data is provided.

       The size must meet the requirements specified in
       Section @xref{hdr:ShMemGuidelines}. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivp3_cmd_set_data_t */
/** @endcond */

#endif /* __VSS_IVP3_PUBLIC_IF_H__ */
