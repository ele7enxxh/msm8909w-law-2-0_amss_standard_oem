#ifndef __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_ivocproc_deprecated_public_if.h
  @brief This file contains the deprecated APR API definitions for Core VocProc
  (CVP) module.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2011, 2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_ivocproc_deprecated_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14    sw     (Tech Pubs) Updated with Doxygen comments from 2.0 and 2.4.

===========================================================================*/


#include "apr_comdef.h"
#include "vss_ivocproc_public_if.h"

/** @addtogroup cvd_cvp_deprecated
@{ */
/** @name Voice processing
@{ */
/*--------------------------------------------------------------------------*
 * VOCPROC TOPOLOGIES                                                       *
 *--------------------------------------------------------------------------*/

/* This topology is deprecated as it is never PORed/tested.   */

/** @cond OEM_only */
/** Deprecated. Use #VSS_IVOCPROC_TOPOLOGY_ID_TX_DM_FLUENCE.

  Tx topology ID for the tri-mic Fluence algorithm. @newpage
*/
#define VSS_IVOCPROC_TOPOLOGY_ID_TX_TM_FLUENCE ( 0x00010F80 )
/** @endcond */


/****************************************************************************
 * CORE VOCPROC APIS                                                        *
 ****************************************************************************/

/** Deprecated. Use #VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2.

  Creates a new full control vocproc session.

  @apr_msgpayload{vss_ivocproc_cmd_create_full_control_session_t}
  @wktable{weak__vss__ivocproc__cmd__create__full__control__session__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION ( 0x000100C3 )

/* Type definition for vss_ivocproc_cmd_create_full_control_session_t. */
typedef struct vss_ivocproc_cmd_create_full_control_session_t vss_ivocproc_cmd_create_full_control_session_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_create_full_control_session_t 
@{ */
/* Payload structure for the 
    VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION command.
*/
struct vss_ivocproc_cmd_create_full_control_session_t
{
  uint16_t direction;
  /**< Specifies the path on which the session is created.
    
       @values
       - 0 -- Tx only
       - 1 -- Rx only
       - 2 -- Tx and Rx @tablebulletend */

  uint32_t tx_port_id;
  /**< Tx device port ID to which the vocproc connects.

       If a port ID is not supplied, set this field to
       #VSS_IVOCPROC_PORT_ID_NONE. */

  uint32_t tx_topology_id;
  /**< Tx topology IDs that identify the predefined algorithms required for
       the session.

       If a topology ID is not supplied, set this field to
       #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */

  uint32_t rx_port_id;
  /**< Rx device port ID to which the vocproc connects.

       If a port ID is not supplied, set this field to
       #VSS_IVOCPROC_PORT_ID_NONE. */

  uint32_t rx_topology_id;
  /**< Rx topology IDs that identify the predefined algorithms required for
       the session.

       If a topology ID is not supplied, set this field to
       #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */

  uint32_t network_id;
  /**< Network ID (see @xhyperref{hdr:DeprecatedNetworkIDs,Deprecated Network IDs}).

       If a network ID is not supplied, set this field to
         #VSS_NETWORK_ID_DEFAULT. */

#if __STDC_VERSION__ >= 199901L
  char_t session_name[];
    /**< Session name string used to identify a session that can be shared with
       passive controllers (optional).

       The string size, including the NULL termination character, is limited
       to 31 characters. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_create_full_control_session_t */

/** Deprecated. Use #VSS_IVOCPROC_CMD_SET_DEVICE_V2.

  Sets a new voice device configuration on the vocproc.

  @apr_msgpayload{vss_ivocproc_cmd_set_device_t}
  @wktable{weak__vss__ivocproc__cmd__set__device__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The configuration must match the direction of the vocproc. For example, when
  a vocproc is created as a Tx only direction, the Rx parameters are ignored. @newpage
*/
#define VSS_IVOCPROC_CMD_SET_DEVICE ( 0x000100C4 )

/* Type definition for vss_ivocproc_cmd_set_device_t. */
typedef struct vss_ivocproc_cmd_set_device_t vss_ivocproc_cmd_set_device_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_set_device_t 
@{ */
/* Payload structure for the VSS_IVOCPROC_CMD_SET_DEVICE command.
*/
struct vss_ivocproc_cmd_set_device_t
{
  uint32_t tx_port_id;
  /**< Tx device port ID to which the vocproc connects.

       If this field is set to #VSS_IVOCPROC_PORT_ID_NONE, the vocproc does
       not connect to any port. */

  uint32_t tx_topology_id;
  /**< Tx topology IDs that identify the predefined algorithms required for
       the session.

       If this field is set to #VSS_IVOCPROC_TOPOLOGY_ID_NONE, the vocproc
       does not contain any pre/postprocessing blocks, and the data is passed
       through unmodified. */

  uint32_t rx_port_id;
  /**< Rx device port ID to which the vocproc connects.

       If this field is set to #VSS_IVOCPROC_PORT_ID_NONE, the vocproc does
       not connect to any port. */

  uint32_t rx_topology_id;
  /**< Rx topology IDs that identify the predefined algorithms required for
       the session.

       If this field is set to #VSS_IVOCPROC_TOPOLOGY_ID_NONE, the vocproc
       does not contain any pre/postprocessing blocks and is passed through
       unmodified. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_set_device_t */

/** @} */ /* end_name Voice processing */

/** @name Calibration data
@{ */
/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/**	Deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA.

  Provides per-network calibration data to the vocproc. Data provided here
  is cached and applied every time the vocproc transitions to the Run state.
  If this command is issued when the vocproc is in the Run state, the data
  is not applied until the next time the vocproc transitions to the Run state.

  @par Expected buffer format
@code
<-------- 32 bits -------->           <-------- 32 bits -------->

 -------------------------
| network_id              | --> network_id
 -------------------------
| data_len                | --> byte length of data block that follows
 -------------------------
| data                    | -------->  -------------------------
|                         |           | mod_id                  |
|                         |            -------------------------
| ...                     |           | param_id                |
 -------------------------             -------------------------
| network_id              |           | param_len  | reserved   |
 -------------------------             -------------------------
| data_len                |           | param_data              |
 -------------------------            |                         |
| data                    |           | ...                     |
|                         |            -------------------------
|                         |           | mod_id                  |
| ...                     |           .                         .
 -------------------------            .                         .
|                         |           .                         .
.                         .
.                         .

@endcode

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_CACHE_CALIBRATION_DATA ( 0x000110E3 )


/**	Deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA.

  Provides per-volume calibration data to the vocproc. Data provided here
  is cached and applied every time the volume changes.

  @par Expected buffer format
@code
<-------- 32 bits -------->           <-------- 32 bits -------->

 -------------------------
| num_vol_steps           | --> number of volume steps per network ID
 -------------------------
| network_id              | --> network_id
 -------------------------
| vol_step_len            | --> length of calibration data for this volume step
 -------------------------
| vol_step_cal_data       | -------->  -------------------------
|                         |           | mod_id                  |
| ...                     |            -------------------------
 -------------------------            | param_id                |
| vol_step_len            |            -------------------------
 -------------------------            | param_len  | reserved   |
|                         |           |-------------------------
| vol_step_cal_data       |           | param_data              |
|                         |           |                         |
|                         |           | ...                     |
| ...                     |            -------------------------
 -------------------------            | mod_id                  |
|                         |           .                         .
.                         .           .                         .
.                         .           .                         .
.                         .
 -------------------------
| network_id              |
 -------------------------
| vol_step_len            |
 -------------------------
| vol_step_cal_data       |
|                         |
| ...                     |
 -------------------------
|                         |
.                         .
.                         .

@endcode

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_CACHE_VOLUME_CALIBRATION_TABLE ( 0x000110E4 )


/** Deprecated. Use #VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA.

  Deregisters the memory that contains the volume calibration data table from
  the vocproc and any calibration-indexing memory provided.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_VOLUME_CALIBRATION_TABLE ( 0x00011278 )


/**	Deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA.

  Registers the memory that contains common calibration data table with
  the vocproc.

  @note1hang For guidelines on using shared memory, see
             Section @xref{hdr:ShMemGuidelines}.

  @apr_msgpayload{vss_ivocproc_cmd_register_calibration_data_v2_t}
  @wktable{weak__vss__ivocproc__cmd__register__calibration__data__v2__t}

  @par Expected buffer format
@code
                                      <-------- 32 bits -------->
 -------------------------
| column_1                | --> eg. network_id
 -------------------------
| column_2                | --> eg. rx_sampling_rate
 -------------------------
|  .                      |
|  .                      |
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

@endcode

  @detdesc
  As part of this registration, in addition to the address, size, and handle
  of the memory containing the calibration table, the client also provides
  the columns and associated column types that the vocproc uses to form a
  key that is used to index into the calibration table and look up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.
  @par
  Calibration data is applied when the vocproc transitions to the Run state.
  @par
  The client is not allowed to modify the calibration data memory while it
  is registered and the vocproc does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail. 
 
  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  Any memory registered here must first be mapped with the CVD by following
  the guidelines in Section @xref{hdr:ShMemGuidelines}. @newpage
*/
#define VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2 ( 0x00011373 )

/* Type definition for vss_ivocproc_cmd_register_calibration_data_v2_t. */

typedef struct vss_ivocproc_cmd_register_calibration_data_v2_t vss_ivocproc_cmd_register_calibration_data_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_register_calibration_data_v2_t 
@{ */
/* Payload of the VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2 command.
*/
struct vss_ivocproc_cmd_register_calibration_data_v2_t
{
  uint32_t cal_mem_handle;
  /**< Handle to the shared memory that holds the per-network calibration data. */

  uint64_t cal_mem_address;
  /**< Location of the calibration data. */

  uint32_t cal_mem_size;
  /**< Size of the calibration data in bytes.

       The amount of memory allocated at cal_mem_address must meet the
       requirement as specified in Section @xref{hdr:ShMemGuidelines}. */

  uint32_t num_columns;
  /**< Number of columns used to form the key for looking up a calibration 
       data entry. */

#if __STDC_VERSION__ >= 199901L
  vss_ivocproc_cal_column_t columns[];
  /**< Array of columns of size num_columns. The order in which the columns
       are provided here must match the order in which they exist in the
       calibration table. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_register_calibration_data_v2_t */

/** Deprecated. Use #VSS_IVOCPROC_CMD_DEREGISTER_STATIC_CALIBRATION_DATA.
  
  Deregisters the memory that contains the common calibration data table from
  the vocproc and any cal-indexing memory provided.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_CALIBRATION_DATA ( 0x00011276 )


/** Deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA to send
  dynamic calibration data to the vocproc.
  
  Registers the memory that contains the volume calibration data table with
  the vocproc.

  @note1hang For guidelines on using shared memory, see
             Section @xref{hdr:ShMemGuidelines}.

  @apr_msgpayload{vss_ivocproc_cmd_register_volume_calibration_data_t}
  @wktable{weak__vss__ivocproc__cmd__register__volume__calibration__data__t}

  @par Expected buffer format
@code
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

@endcode

  @detdesc
  As part of this registration, in addition to the address, size, and handle
  of the memory containing the calibration table, the client also provides
  the columns and associated column types that the vocproc uses to form a
  key that is used to index into the calibration table and look up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.
  @par
  Calibration data is applied when the vocproc transitions to the Run state
  as well as when the volume step changes during the Run state.
  @par
  The client is not allowed to modify the calibration data memory while it
  is registered and the vocproc does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.
  @par
  The supported column IDs that can be registered in this command are:
  - #VSS_ICOMMON_CAL_COLUMN_NETWORK
  - #VSS_ICOMMON_CAL_COLUMN_RX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_TX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_VOLUME_INDEX

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  Any memory registered here must first be mapped with the CVD by following
  the guidelines in Section @xref{hdr:ShMemGuidelines}. @newpage
*/
#define VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA ( 0x00011374 )

/* Type definition for vss_ivocproc_cmd_register_volume_calibration_data_t. */

typedef struct vss_ivocproc_cmd_register_volume_calibration_data_t vss_ivocproc_cmd_register_volume_calibration_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_register_volume_calibration_data_t 
@{ */
/* Payload of the VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA command.
*/
struct vss_ivocproc_cmd_register_volume_calibration_data_t
{
  uint32_t cal_mem_handle;
  /**< Handle to the shared memory that holds the volume calibration data. */

  uint64_t cal_mem_address;
  /**< Location of volume calibration data. */

  uint32_t cal_mem_size;
  /**< Size of the volume calibration data in bytes. The amount of memory
       allocated at cal_mem_address must meet the requirement as specified
       in Section @xref{hdr:ShMemGuidelines}. */

  uint32_t num_columns;
  /**< Number of columns used to form the key for looking up a calibration
       data entry. */

#if __STDC_VERSION__ >= 199901L
  vss_ivocproc_cal_column_t columns[];
  /**< Array of columns of size num_columns. The order in which the
       columns are provided here must match the order in which they exist
       in the calibration table. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_register_volume_calibration_data_t */


/** Deprecated. Use #VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA.

  Deregisters the memory that contains the volume calibration data table from
  the vocproc and any provided calibration-indexing memory.

  @apr_msgpayload
  None.

  @par Payload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_VOLUME_CALIBRATION_DATA ( 0x00011375 )

/** @} */ /* end_name Calibration data */

/** @name Voice processing volume/mute
@{ */
/****************************************************************************
 *  VOCPROC VOLUME/MUTE APIS                                                *
 ****************************************************************************/

/** Deprecated. Use #VSS_IVOLUME_CMD_SET_STEP.

  Changes the volume and recalibrates for the new volume.

  @apr_msgpayload{vss_ivocproc_cmd_set_rx_volume_index_t}
  @wktable{weak__vss__ivocproc__cmd__set__rx__volume__index__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX ( 0x000110EE )

/* Type definition for vss_ivocproc_cmd_set_rx_volume_index_t. */
typedef struct vss_ivocproc_cmd_set_rx_volume_index_t vss_ivocproc_cmd_set_rx_volume_index_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_set_rx_volume_index_t 
@{ */
/* Payload structure for the VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX
    command.
*/
struct vss_ivocproc_cmd_set_rx_volume_index_t
{
  uint16_t vol_index;
  /**< Volume index used by the vocproc to index the volume table provided in
       #VSS_IVOCPROC_CMD_CACHE_VOLUME_CALIBRATION_TABLE, and to set the volume
       on the Voice Digital Signal Processor (VDSP). */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivocproc_cmd_set_rx_volume_index_t */


/** Deprecated. Use #VSS_IVOLUME_CMD_MUTE_V2.

  Changes the mute setting.

  @apr_msgpayload{vss_ivocproc_cmd_set_mute_t}
  @wktable{weak__vss__ivocproc__cmd__set__mute__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_SET_MUTE ( 0x000110EF )

/* Type definition for vss_ivocproc_cmd_set_mute_t. */
typedef struct vss_ivocproc_cmd_set_mute_t vss_ivocproc_cmd_set_mute_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivocproc_cmd_set_mute_t 
@{ */
/* Payload structure for the VSS_IVOCPROC_CMD_SET_MUTE command.
*/
struct vss_ivocproc_cmd_set_mute_t
{
  uint16_t direction;
  /**< Specifies the path on which mute is configured.
    
       @values
       - 0 -- Tx only
       - 1 -- Rx only
       - 2 -- Tx and Rx @tablebulletend */

  uint16_t mute_flag;
  /**< Turn mute on or off.
    
       @values
       - 0 -- Disables mute
       - 1 -- Enables mute @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* weakgroup weak_vss_ivocproc_cmd_set_mute_t */
/** @} */ /* end_name Voice processing volume/mute */
/** @} */ /* end_addtogroup cvd_cvp_deprecated */


#endif /* __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__ */

