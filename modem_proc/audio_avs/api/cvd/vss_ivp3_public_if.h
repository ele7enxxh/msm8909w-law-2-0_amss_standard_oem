#ifndef __VSS_IVP3_PUBLIC_IF_H__
#define __VSS_IVP3_PUBLIC_IF_H__

/**
  @file vss_ivp3_public_if.h
  @brief This file contains the APR API definitions for CVD VP3.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CVD_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_ivp3_public_if 
      group description in the CVD_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (C) 2013 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_ivp3_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_ivp3_public_if
@{ */

/****************************************************************************
 * VP3 APIS                                                                 *
 ****************************************************************************/

/**
  Gets the size of the VP3 data.

  This is a Qualcomm proprietary API and not intended for public use.

  @par Payload
  None.

  @return
  #VSS_IVP3_RSP_GET_SIZE -- In case of success.
  #APRV2_IBASIC_RSP_RESULT -- In case of failure.

  @dependencies
  None.
*/
#define VSS_IVP3_CMD_GET_SIZE ( 0x00012EDD )


/**
  Response to the #VSS_IVP3_CMD_GET_SIZE command.

  This is a Qualcomm proprietary API and not intended for public use.

  @par Payload
  #vss_ivp3_rsp_get_size_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IVP3_RSP_GET_SIZE ( 0x00012EDE )

/** @brief Type definition for #vss_ivp3_rsp_get_size_t.
*/
typedef struct vss_ivp3_rsp_get_size_t vss_ivp3_rsp_get_size_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVP3_RSP_GET_SIZE command response.
*/
struct vss_ivp3_rsp_get_size_t
{
  uint32_t size;
    /**< Size in bytes of the VP3 data. */
}
#include "apr_pack_end.h"
;


/**
  Gets the VP3 data.

  This is a Qualcomm proprietary API and not intended for public use.

  @par Payload
  #vss_ivp3_cmd_get_data_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Before retrieving VP3 data, the client must first obtain the size of the
  VP3 data via the #VSS_IVP3_CMD_GET_SIZE command. The client may not assume
  the VP3 size will remain unchanged throughout the lifetime of a session of
  the module exposing this interface; the client must query the VP3 size
  before every VP3 data retrieval.

  @comments
  None.
*/
#define VSS_IVP3_CMD_GET_DATA ( 0x00012EDF )

/** @brief Type definition for #vss_ivp3_cmd_get_data_t.
*/
typedef struct vss_ivp3_cmd_get_data_t vss_ivp3_cmd_get_data_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVP3_CMD_GET_DATA command.
*/
struct vss_ivp3_cmd_get_data_t
{
  uint32_t mem_handle;
    /**< The handle to the shared memory where the VP3 data is returned. See
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly. */
  uint64_t mem_address;
    /**< Location where the VP3 data is returned. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t mem_size;
    /**< Size of the memory in bytes where the VP3 data is returned. The size
         must meet the requirements specified in @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly.
         The size must also be large enough to accommodate the VP3 data as
         obtained via #VSS_IVP3_CMD_GET_SIZE command. */
}
#include "apr_pack_end.h"
;


/**
  Sets the VP3 data.

  This is a Qualcomm proprietary API and not intended for public use.

  @par Payload
  #vss_ivp3_cmd_set_data_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Setting VP3 data in run state is not allowed for any module exposing this
  interface. The maximum supported VP3 data size is 1024 bytes.

  @comments
  None.
*/
#define VSS_IVP3_CMD_SET_DATA ( 0x00012EE0 )

/** @brief Type definition for #vss_ivp3_cmd_set_data_t.
*/
typedef struct vss_ivp3_cmd_set_data_t vss_ivp3_cmd_set_data_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVP3_CMD_SET_DATA command.
*/
struct vss_ivp3_cmd_set_data_t
{
  uint32_t mem_handle;
    /**< The handle to the shared memory where the VP3 data is provided. See
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly. */
  uint64_t mem_address;
    /**< Location where the VP3 data is provided. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t mem_size;
    /**< Size of the memory in bytes where the VP3 data is provided. The size
         must meet the requirements specified in @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly. */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_ivp3_public_if */

#endif /* __VSS_IVP3_PUBLIC_IF_H__ */
