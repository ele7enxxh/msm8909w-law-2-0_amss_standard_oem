#ifndef __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_deprecated_public_if.h
  @brief This file contains the deprecated APR API definitions for Core VocProc
  (CVP) module.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011-2012, 2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_ivocproc_deprecated_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/


#include "apr_comdef.h"
#include "vss_ivocproc_public_if.h"

/** @addtogroup vss_ivocproc_deprecated_public_if
@{ */

/*--------------------------------------------------------------------------*
 * VOCPROC TOPOLOGIES                                                       *
 *--------------------------------------------------------------------------*/

/** @name Deprecated Topology IDs
  @latexonly \label{deprecated_topology_ID} @endlatexonly
  @{ */

/** Tx topology ID for the tri-mic Fluence algorithm.

    @deprecated
    This topology is deprecated as it is never PORed/tested. Use #VSS_IVOCPROC_TOPOLOGY_ID_TX_DM_FLUENCE
*/
#define VSS_IVOCPROC_TOPOLOGY_ID_TX_TM_FLUENCE ( 0x00010F80 )


/****************************************************************************
 * CORE VOCPROC APIS                                                        *
 ****************************************************************************/

/**
  Creates a new full control vocproc session.

  @deprecated
  This command is deprecated. Use #VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2.

  @par Payload
  #vss_ivocproc_cmd_create_full_control_session_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION ( 0x000100C3 )

/** @brief Type definition for #vss_ivocproc_cmd_create_full_control_session_t.
*/
typedef struct vss_ivocproc_cmd_create_full_control_session_t vss_ivocproc_cmd_create_full_control_session_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the 
    #VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION command.
*/
struct vss_ivocproc_cmd_create_full_control_session_t
{
  uint16_t direction;
    /**< Stream direction. Possible values:\n
         - 0 -- Tx only \n
         - 1 -- Rx only \n
         - 2 -- Tx and Rx */
  uint32_t tx_port_id;
    /**< Tx device port ID to which the vocproc connects. If a port ID
         is not being supplied, set this to #VSS_IVOCPROC_PORT_ID_NONE. */
  uint32_t tx_topology_id;
    /**< Tx leg topology ID. If a topology ID is not being supplied, set
	     this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */
  uint32_t rx_port_id;
    /**< Rx device port ID to which the vocproc connects. If a port ID
         is not being supplied, set this to #VSS_IVOCPROC_PORT_ID_NONE. */
  uint32_t rx_topology_id;
    /**< Rx leg topology ID. If a topology ID is not being supplied, set
         this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */
  uint32_t network_id;
    /**< Network ID (see @latexonly \hyperref[network_ID]{Network IDs}@endlatexonly).
         If a network ID is not being supplied, set this to
         #VSS_NETWORK_ID_DEFAULT. */

#if __STDC_VERSION__ >= 199901L
  char_t session_name[];
    /**< Session name string used to identify a session that can be shared with
         passive controllers (optional). The string size, including the NULL
         termination character, is limited to 31 characters. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;


/**
  Sets a new voice device configuration on the vocproc.

  @deprecated
  This command is deprecated. Use #VSS_IVOCPROC_CMD_SET_DEVICE_V2.

  @par Payload
  #vss_ivocproc_cmd_set_device_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  The configuration provided must match the direction of the vocproc. For
  example, when a vocproc is created as a Tx only direction, the Rx
  parameters are ignored.
*/
#define VSS_IVOCPROC_CMD_SET_DEVICE ( 0x000100C4 )

/** @brief Type definition for  #vss_ivocproc_cmd_set_device_t.
*/
typedef struct vss_ivocproc_cmd_set_device_t vss_ivocproc_cmd_set_device_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_CMD_SET_DEVICE command.
*/
struct vss_ivocproc_cmd_set_device_t
{
  uint32_t tx_port_id;
    /**< Tx device port ID to which the vocproc connects. Setting this to
         #VSS_IVOCPROC_PORT_ID_NONE means the vocproc does not connect to
         any port. */
  uint32_t tx_topology_id;
    /**< Tx leg topology ID. Setting this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE
         means the vocproc does not contain any pre/postprocessing blocks and
         the data is passed through unmodified. */
  uint32_t rx_port_id;
    /**< Rx device port ID to which the vocproc connects. Setting this to
         #VSS_IVOCPROC_PORT_ID_NONE means the vocproc does not connect to
         any port. */
  uint32_t rx_topology_id;
    /**< Rx leg topology ID. Setting this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE
         means the vocproc does not contain any pre/postprocessing blocks and
         is passed through unmodified. */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/**
  Registers the memory that contains common calibration data table with
  the vocproc.

  As part of this registration, in addition to the address, size and handle
  of the memory containing the calibration table, the client also porvides
  the columns and associated column types which the vocproc uses to form a
  key that is used to index into the calibration table and look-up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.

  Calibration data is applied when the vocproc transitions to the run state.

  @deprecated
  This command is deprecated. Use
  #VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA for sending static 
  calibration data to the vocproc.

  @par Payload
  #vss_ivocproc_cmd_register_calibration_data_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Any memory registered here must first be mapped with CVD by following
  @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
  @endlatexonly. 

  @comments
  The client is not allowed to modify the calibration data memory while it
  is registered and the vocproc does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.

  The supported column IDs that can be registered in this command are:

  - #VSS_ICOMMON_CAL_COLUMN_NETWORK
  - #VSS_ICOMMON_CAL_COLUMN_RX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_TX_SAMPLING_RATE
*/
#define VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2 ( 0x00011373 )

/** @brief Type definition for
    #vss_ivocproc_cmd_register_calibration_data_v2_t.
*/
typedef struct vss_ivocproc_cmd_register_calibration_data_v2_t vss_ivocproc_cmd_register_calibration_data_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2 command.

    Expected buffer format:

    @verbatim
                                           <-------- 32 bits -------->
      -------------------------
     | column_1                | --> eg. network_id
      -------------------------
     | column_2                | --> eg. rx_sampling_rate
      -------------------------
     |  .                      |
     |	.                      |
      -------------------------
     | column_n                |
      -------------------------
     | data_size               | --> byte length of data block that follows
      -------------------------
     | data                    | -------->  -------------------------
     |                         |           | mod_id                  |
     |                         |            -------------------------
     | ...                     |           | param_id                |
      -------------------------             -------------------------
     | column_1                |           | param_size | reserved   |
      -------------------------	            -------------------------
     | column_2                |           | param_data              |
      -------------------------	           |                         |
     |  .                      |           | ...                     |
     |  .                      |            -------------------------
      -------------------------            | mod_id                  |
     | column_n                |           .                         .
      -------------------------            .                         .
     | data_size               |           .                         .
      -------------------------
     | data                    |
     |                         |
     |                         |
     | ...                     |
      -------------------------
     |                         |
     .                         .
     .                         .

    @endverbatim                                               

*/
struct vss_ivocproc_cmd_register_calibration_data_v2_t
{
  uint32_t cal_mem_handle;
    /**< Handle to the shared memory that holds the per-network calibration
         data. See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
         Guideline} @endlatexonly. */
  uint64_t cal_mem_address;
    /**< Location of calibration data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t cal_mem_size;
    /**< Size of the calibration data in bytes. The amount of memory allocated
         at mem_address must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
  uint32_t num_columns;
    /**< Number of columns used to form the key for looking up a calibration data
         entry. */
#if __STDC_VERSION__ >= 199901L
  vss_ivocproc_cal_column_t columns[];
#endif /* __STDC_VERSION__ >= 199901L */
    /**< Array of columns of size num_columns. The order in which the colums are
         provided here must match the order in which they exist in the calibration
         table provided. */
}
#include "apr_pack_end.h"
;


/**
  Deregisters the memory that contains the common calibration data table from
  the vocproc and any cal-indexing memory provided.

  @deprecated
  This command is deprecated. Use
  #VSS_IVOCPROC_CMD_DEREGISTER_STATIC_CALIBRATION_DATA to deregister static
  calibration data from the vocproc.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_CALIBRATION_DATA ( 0x00011276 )


/**
  Registers the memory that contains the volume calibration data table with
  the vocproc.

  As part of this registration, in addition to the address, size and handle
  of the memory containing the calibration table, the client also porvides
  the columns and associated column types which the vocproc uses to form a
  key that is used to index into the calibration table and look-up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.

  @deprecated
  This command is deprecated. Use
  #VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA for sending dynamic
  calibration data to the vocproc.

  @par Payload
  #vss_ivocproc_cmd_register_volume_calibration_data_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Any memory registered here must first be mapped with CVD by following
  @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
  @endlatexonly. 

  @comments
  The client is not allowed to modify the calibration data memory while it
  is registered and the vocproc does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.

  The supported column IDs that can be registered in this command are:

  - #VSS_ICOMMON_CAL_COLUMN_NETWORK
  - #VSS_ICOMMON_CAL_COLUMN_RX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_TX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_VOLUME_INDEX
*/
#define VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA ( 0x00011374 )

/** @brief Type definition for
    #vss_ivocproc_cmd_register_volume_calibration_data_t.
*/
typedef struct vss_ivocproc_cmd_register_volume_calibration_data_t vss_ivocproc_cmd_register_volume_calibration_data_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA command.

    Expected buffer format.

    @verbatim
                                           <-------- 32 bits -------->
      -------------------------
     | column_1                | --> eg. network_id
      -------------------------
     | column_2                | --> eg. rx_sampling_rate
      -------------------------
     |  .                      |
     |  .                      |
      -------------------------
     | column_n                | --> eg. volume_index
      -------------------------
     | data_size               | --> byte length of data block that follows
      -------------------------
     | data                    | -------->  -------------------------
     |                         |           | mod_id                  |
     |                         |            -------------------------
     | ...                     |           | param_id                |
      -------------------------             -------------------------
     | column_1                |           | param_size | reserved   |
      -------------------------	            -------------------------
     | column_2                |           | param_data              |
      -------------------------	           |                         |
     |  .                      |           | ...                     |
     |  .                      |            -------------------------
      -------------------------            | mod_id                  |
     | column_n                |           .                         .
      -------------------------            .                         .
     | data_size               |           .                         .
      -------------------------
     | data                    |
     |                         |
     |                         |
     | ...                     |
      -------------------------
     |                         |
     .                         .
     .                         .

   @endverbatim
*/
struct vss_ivocproc_cmd_register_volume_calibration_data_t
{
  uint32_t cal_mem_handle;
    /**< Handle to the shared memory that holds the volume calibration
         data. See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
         Guideline} @endlatexonly. */
  uint64_t cal_mem_address;
    /**< Location of volume calibration data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t cal_mem_size;
    /**< Size of the volume calibration data in bytes. The amount of memory allocated
         at mem_address must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
  uint32_t num_columns;
    /**< Number of columns used to form the key for looking up a calibration data
         entry. */
#if __STDC_VERSION__ >= 199901L
  vss_ivocproc_cal_column_t columns[];
#endif /* __STDC_VERSION__ >= 199901L */
    /**< Array of columns of size num_columns. The order in which the colums are
         provided here must match the order in which they exist in the calibration
         table provided. */
}
#include "apr_pack_end.h"
;


/**
  Deregisters the memory that contains the volume calibration data table from
  the vocproc and any cal-indexing memory provided.

  @deprecated
  This command is deprecated. Use
  #VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA to deregister dynamic
  calibration data from the vocproc.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_VOLUME_CALIBRATION_DATA ( 0x00011375 )


/****************************************************************************
 *  VOCPROC VOLUME/MUTE APIS                                                *
 ****************************************************************************/

/**
  Changes the volume and recalibrates for the new volume.

  @deprecated
  This command is deprecated. Use #VSS_IVOLUME_CMD_SET_STEP.

  @par Payload
  #vss_ivocproc_cmd_set_rx_volume_index_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX ( 0x000110EE )

/** @brief Type definition for #vss_ivocproc_cmd_set_rx_volume_index_t.
*/
typedef struct vss_ivocproc_cmd_set_rx_volume_index_t vss_ivocproc_cmd_set_rx_volume_index_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX
    command.
*/
struct vss_ivocproc_cmd_set_rx_volume_index_t
{
  uint16_t vol_index;
    /**< Volume index used by the vocproc to index into the volume table
         provided in #VSS_IVOCPROC_CMD_CACHE_VOLUME_CALIBRATION_TABLE and
         to set the volume on the Voice Digital Signal Processor (VDSP). */
}
#include "apr_pack_end.h"
;


/**
  Changes the mute setting.

  @deprecated
  This command is deprecated. Use #VSS_IVOLUME_CMD_MUTE_V2.

  @par Payload
  #vss_ivocproc_cmd_set_mute_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_SET_MUTE ( 0x000110EF )

/** @brief Type definition for #vss_ivocproc_cmd_set_mute_t.
*/
typedef struct vss_ivocproc_cmd_set_mute_t vss_ivocproc_cmd_set_mute_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_CMD_SET_MUTE command.
*/
struct vss_ivocproc_cmd_set_mute_t
{
  uint16_t direction;
    /**< Stream direction. Possible values:\n
         - 0 -- Tx only \n
         - 1 -- Rx only \n
         - 2 -- Tx and Rx */
  uint16_t mute_flag;
    /**< Turn mute on or off. Possible values:\n
         - 0 -- Silence is disabled (mute is off).\n
         - 1 -- Silence is enabled (mute is on). */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_ivocproc_deprecated_public_if */

#endif /* __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__ */

