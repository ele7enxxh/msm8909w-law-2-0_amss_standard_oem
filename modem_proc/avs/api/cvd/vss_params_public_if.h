#ifndef __VSS_PARAMS_PUBLIC_IF_H__
#define __VSS_PARAMS_PUBLIC_IF_H__

/**
  @file vss_params_public_if.h
  @brief This file contains module and parameter IDs supported by Core Voice Drivers.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_params_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/28/14     sw    (Tech Pubs) Updated with Doxygen comments from 2.0, and
                   edited for 2.6.

===========================================================================*/


/****************************************************************************
 * MODULE ID DEFINES                                                        *
 ****************************************************************************/

/** @ingroup cvd_common_module_rx_volume
  ID of the Rx volume module.
  This module uses one parameter: VSS_PARAM_VOLUME.
*/
#define VSS_MODULE_RX_VOLUME ( 0x000112D6 )


/** @addtogroup cvd_common_module_feature_cfg
@{ */
/** ID of the Feature Configuration module.

  This module contains feature configurations for enablement, operating Rx
  sampling rate, and the list of applicable system configurations.

  This module uses one parameter: VSS_PARAM_HDVOICE_CONFIG.
*/
#define VSS_MODULE_FEATURE_CONFIG ( 0x000130A5 )


/****************************************************************************
 * FEATURE CONFIG DEFINES                                                   *
 ****************************************************************************/

/*---------------------------------------------------------------------------*
 * Feature Enablement Modes                                                  *
 *---------------------------------------------------------------------------*/

/** Sets the feature Enable mode to Default Off. */
#define VSS_PARAM_FEATURE_DEFAULT_OFF ( 0 )

/** Sets the feature Enable mode to Default On. */
#define VSS_PARAM_FEATURE_DEFAULT_ON ( 1 )

/** Sets the feature Enable mode to Forced Off. */
#define VSS_PARAM_FEATURE_FORCED_OFF ( 2 )

/** Sets the feature Enable mode to Forced On. */
#define VSS_PARAM_FEATURE_FORCED_ON ( 3 )

/** @} */ /* end_addtogroup cvd_common_module_feature_cfg */


/****************************************************************************
 * PARAM ID DEFINES                                                         *
 ****************************************************************************/

/** @ingroup cvd_common_module_rx_volume
  ID of the parameter used by VSS_MODULE_RX_VOLUME to set the volume.
  On the Rx vocproc, the parameter data is a 32-bit value in millibels.
*/
#define VSS_PARAM_VOLUME ( 0x000112D7 )

/** @ingroup cvd_common_module_feature_cfg
  ID of the parameter used by VSS_MODULE_FEATURE_CONFIG to apply
  configurations for HD Voice features.

  The data in this parameter is a table that is indexed by columns.
  Each set of columns is a system configuration for which an HD voice
  feature can be enabled and configured.

  The parameter data is specified by six values:
  - version -- Specifies the data structure used in the entries. As the 
    version increases, values are appended to the original data structure.
  - size -- Indicates the data size in bytes required for specifying the
    number of columns, columns, number of entries, and entries.
  - num_columns -- Indicates the number of columns in the parameter.
  - columns -- Data structure of type %vss_param_cal_column_t, which defines
    the columns to be used for finding entries.
       - The order and IDs of the columns specified in this structure must be
         the same for each individual entry.
       - The list of applicable column IDs and types must be the same as
         the static calibration table defined in
         #VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA.
  - num_entries -- Indicates the number of entries in the parameter.
  - entries -- Data structure of type dynamic_syscfg_entry_t, which
    specifies the column values for a data entry. The data entry is of
    type %vss_param_hdvoice_config_data_t.
       - The column values for each entry must match the definition (ID and
         type) and order defined in the columns section.
       - %vss_param_cal_na_value_t indicates a varying size for the column 
         value described by the column type.
       - The possible data types for a column value are uint8, uint16, uint32,
         and uint64_t.

  When calibration is applied, the calibration table is checked for a matching
  data entry for the current system configuration.
  - If no entry is found, all features are disabled.
  - If an entry is found, the feature specified in the entry is configured to
    its enablement mode and Rx sampling rate.

  The data structures for this parameter are specified below.

  @code
                                           <-------- 32 bits -------->
      -------------------------
     | version                 |
      -------------------------
     | size                    | -------> size of the data below
      -------------------------
     | num_columns             |
      ------------------------- 
     | columns_1               | ------->   -------------------------
     |                         |           | column_id               |  --> eg. network_id
      -------------------------             -------------------------
     | columns_2               |           | column_type             |  --> eg. uint32
     |                         |            -------------------------
      -------------------------            | na_value                |
     |  .                      |            -------------------------
     |  .                      |
      -------------------------
     | columns_n               |
     -------------------------
     | num_entries             |
      -------------------------  --> entry start
     | column_1_value          | --> eg. CDMA (network_id)
      -------------------------
     | column_2_value          | --> eg. FR (media_id)
      -------------------------
     |  .                      |
     |  .                      |
      -------------------------
     | column_n_value          |
      -------------------------
     | config_data             |
     |                         |
     |                         |
     | ...                     |
      -------------------------  --> entry start
     | column_1_value          | 
      ------------------------- 
     | column_2_value          |
      -------------------------
     |  .                      |
     |  .                      |
      -------------------------
     | column_n_value          |
      -------------------------- ------->   -------------------------
     | config_data             |           | feature_id              |  --> e.g. WideVoice
     |                         |            -------------------------
     |                         |           | enable_mode             |  --> e.g. Default On
     | ...                     |            -------------------------
      -------------------------            | rx_sampling_rate        |  --> e.g. 16000 Hz
     | column_1_value          |            -------------------------
      -------------------------  
     .                         .
     .                         .
  @endcode                                               

  @par Expected data format for the entire table
@code
Type                                                       Parameter
----                                                       ---------
uint32                                                     version
uint32                                                     size
uint32                                                     num_columns
vss_param_cal_column_t[num_columns]                        columns
uint32                                                     num_entries
dynamic_syscfg_entry_t[num_sys_config_entries]             entries

@endcode                                               

  @par Expected data format for the vss_param_cal_column_t type
  @wktable{weak__vss__param__cal__column__t}

  @par Expected data format for the dynamic_syscfg_entry_t type
    @code
    Type                                                       Parameter
    ----                                                       ---------
    vss_param_cal_na_value_t[num_columns]                      column_value
    vss_param_hdvoice_config_data_t                            config_data

    @endcode                                               

  @par Expected data format for the %vss_param_cal_na_value_t type
  @wktable{weak__vss__param__cal__na__value__t}

  @par Expected data format for the %vss_param_hdvoice_config_data_t type
  @wktable{weak__vss__param__hdvoice__config__data__t}
*/
#define VSS_PARAM_HDVOICE_CONFIG ( 0x000130A6 )

typedef union vss_param_cal_na_value_t vss_param_cal_na_value_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_cal_na_value_t
@{ */
/* Union for specifying a system configuration column N/A value.
*/
union vss_param_cal_na_value_t
{
  uint8_t uint8_val;      /**< UINT8 N/A value. */
  uint16_t uint16_val;    /**< UINT16 N/A value. */
  uint32_t uint32_val;    /**< UINT32 N/A value. */
  uint64_t uint64_val;    /**< UINT64 N/A value. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_param_cal_na_value_t */

typedef struct vss_param_cal_column_t vss_param_cal_column_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_cal_column_t
@{ */
/* Structure for specifying a single column for system configuration
*/
struct vss_param_cal_column_t
{
  uint32_t id;
  /**< Column ID.

       @values See @xhyperref{hdr:CalibrationColumns,Calibration columns} */

  uint32_t type;
  /**< Column type.

       @values See @xhyperref{hdr:CalibrationColumnTypes,Calibration column types} */

  vss_param_cal_na_value_t na_value;
  /**< N/A column value associated with this column ID and type.
         
       The N/A value is a reserved and indicates that the entry is independent 
       of this column.

       The size of na_value is dependent on the column type. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_param_cal_column_t */

typedef struct vss_param_hdvoice_config_data_t vss_param_hdvoice_config_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_hdvoice_config_data_t
@{ */
/* Structure for specifying a configuration data in #VSS_PARAM_HDVOICE_CONFIG
*/
struct vss_param_hdvoice_config_data_t
{
  uint32_t feature_id;
  /**< Unique ID of a feature.

       @values See @xhyperref{hdr:CalColFeature,Calibration features} */

  uint32_t enable_mode;
  /**< Enablement mode for a feature.
  
       @values
       - #VSS_PARAM_FEATURE_DEFAULT_OFF
       - #VSS_PARAM_FEATURE_DEFAULT_ON
       - #VSS_PARAM_FEATURE_FORCED_OFF
       - #VSS_PARAM_FEATURE_FORCED_ON @tablebulletend */

  uint32_t rx_sampling_rate;
  /**< Sampling rate on the Rx path.

       @values 16000 Hz */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_param_hdvoice_config_data_t */


#endif /* __VSS_PARAMS_PUBLIC_IF_H__ */

