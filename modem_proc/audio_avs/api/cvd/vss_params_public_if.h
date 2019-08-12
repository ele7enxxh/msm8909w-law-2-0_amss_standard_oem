#ifndef __VSS_PARAMS_PUBLIC_IF_H__
#define __VSS_PARAMS_PUBLIC_IF_H__

/**
  @file vss_params_public_if.h
  @brief This file contains module and parameter IDs supported by Core Voice Drivers.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_params_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011-2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_params_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/

/** @addtogroup vss_params_public_if
@{ */

/****************************************************************************
 * MODULE ID DEFINES                                                        *
 ****************************************************************************/

/** @name Module ID definitions
  @latexonly \label{module_id_definition} @endlatexonly
  @{ */

/** 
  Rx volume module ID.

  Parameter for this module ID is:
  #VSS_PARAM_VOLUME
*/
#define VSS_MODULE_RX_VOLUME ( 0x000112D6 )


/** 
  Feature Configuration module ID.
  Contains feature configurations for enablement, operating Rx sampling rate,
  and the list applicable system configurations.

  Parameter for this module ID is:
  #VSS_PARAM_HDVOICE_CONFIG

*/
#define VSS_MODULE_FEATURE_CONFIG ( 0x000130A5 )


/** @} */ /* end_name_group Module ID definitions */

/****************************************************************************
 * FEATURE CONFIG DEFINES                                                   *
 ****************************************************************************/

/*---------------------------------------------------------------------------*
 * Feature Enablement Modes                                                  *
 *---------------------------------------------------------------------------*/

/** @name Feature Enablement Modes
@{ */

/** Select feature enable mode to Default Off. */
#define VSS_PARAM_FEATURE_DEFAULT_OFF ( 0 )

/** Select feature enable mode to Default On. */
#define VSS_PARAM_FEATURE_DEFAULT_ON ( 1 )

/** Select feature enable mode to Forced Off. */
#define VSS_PARAM_FEATURE_FORCED_OFF ( 2 )

/** Select feature enable mode to Forced On. */
#define VSS_PARAM_FEATURE_FORCED_ON ( 3 )

/** @} */ /* end_name_group Feature Enablement Modes */

/****************************************************************************
 * PARAM ID DEFINES                                                         *
 ****************************************************************************/

/** @name Param ID definitions
  @latexonly \label{param_id_definition} @endlatexonly
  @{ */

/** 
  Parameter for setting the volume.

  @comments
  For setting the volume on rx vocproc, the parameter data is 32 bit 
  value in milli-Bels.
*/
#define VSS_PARAM_VOLUME ( 0x000112D7 )

/** 
  Parameter for applying configurations for HD Voice features.

  @comments
  The data in this parameter is a table which is indexed by columns.
  Each set of columns is a system configuration which an HD voice
  feature can be enabled and configured for.

  The parameter data is specified by six values:
  1. version
  2. size
  3. number of columns
  4. columns
  5. number of entries
  6. entries

  The version specifies the data structure used in the entries As the 
  version increases, values are appended to the original data structure.
  
  The size specifies the data size in bytes required for specifying the
  number of columns, columns, number of entries, and entries.

  The number of columns specifies the number of columns there will be in the
  parameter.

  The columns specify the vss_param_cal_column_t data structure, which
  defines the columns to be used to find entries.  The order and IDs of columns
  reflected in this section are expected to be reflected in the entries. The
  list of applicable column IDs and types are the same as the Static
  Calibration table defined in #VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA.

  The number of entries specifies the number of entries there will be in the
  parameter.

  The entries specify the dynamic structure dynamic_syscfg_entry_t. This
  structure specifies the column values that need to be used to find the
  associated entry of data, which is specified by the data structure
  vss_param_hdvoice_config_data_t.  The column values in each entry must reflect
  the definition (ID and type) and the order defined in the columns section.
  dynamic_cal_type_t signifies the column type defined in the columns section.

  When calibration is applied, this table will be checked if a data entry
  can be found for the current system configuration, which are column values.  
  If no data is found, all features will be disabled.  If data is found,
  the feature specified in data will be configured to its enablement mode, 
  and Rx sampling rate.

  More descriptions of the data structure for this parameter are illustrated
  below.

  The expected data format:

    @verbatim
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
    @endverbatim                                               

  Expected Data format for the entire table:
    @verbatim
    
    TYPE                                                              PARAMETER

    uint32                                                            version
    uint32                                                            size
    uint32                                                            num_columns
    vss_param_cal_column_t[num_columns]                               columns
    uint32                                                            num_entries
    dynamic_syscfg_entry_t[num_sys_config_entries]                    entries

    @endverbatim                                               

  Expected Data format for vss_param_cal_column_t type:
    @verbatim
    
    TYPE                                                              PARAMETER

    uint32                                                            id
    uint32                                                            type
    vss_param_cal_na_value_t                                          na_value

    @endverbatim                                               

  Expected Data format for dynamic_syscfg_entry_t type:
    @verbatim
    
    TYPE                                                              PARAMETER

    dynamic_cal_type_t[num_columns]                                   column_value
    vss_param_hdvoice_config_data_t                                   config_data

    @endverbatim                                               

  Expected Data format for vss_param_hdvoice_config_data_t type:
    @verbatim
    
    TYPE                                                              PARAMETER

    uint32                                                            feature_id
    uint32                                                            enable_mode
    uint32                                                            rx_sampling_rate

    @endverbatim                                               

*/
#define VSS_PARAM_HDVOICE_CONFIG ( 0x000130A6 )

/** @brief Union for specifying a system configuration column N/A value.
*/
typedef union vss_param_cal_na_value_t vss_param_cal_na_value_t;

#include "apr_pack_begin.h"
union vss_param_cal_na_value_t
{
  uint8_t uint8_val;
    /**< UINT8 N/A value. */
  uint16_t uint16_val;
    /**< UINT16 N/A value. */
  uint32_t uint32_val;
    /**< UINT32 N/A value. */
  uint64_t uint64_val;
    /**< UINT64 N/A value. */
}
#include "apr_pack_end.h"
;

/** @brief Structure for specifying a single column for system configuration
*/

typedef struct vss_param_cal_column_t vss_param_cal_column_t;

#include "apr_pack_begin.h"
struct vss_param_cal_column_t
{
  uint32_t id;
    /**< Column ID. Supported values: \n
         see @latexonly \hyperref[calibration_column]{Calibration Columns}
         @endlatexonly. */
  uint32_t type;
    /**< Column type. Supported values: \n
         see @latexonly \hyperref[calibration_column_type]{Calibration Column Types}
         @endlatexonly. */
  vss_param_cal_na_value_t na_value;
    /**< The N/A column value associated with this column ID and type.
         
         The N/A value is a reseved value indicating that entry is independent 
         of this column.

         The size of na_value is dependent on the column type. */
}
#include "apr_pack_end.h"
;

/** Structure for specifying a configuration data in #VSS_PARAM_HDVOICE_CONFIG.

  @comments
  For setting the enablement mode value, the data is a 32-bit value with the
  following supported modes:

  0 – Default Off (Accepts BBWE UI commands)
  1 – Default On  (Accepts BBWE UI commands)
  2 – Forced Off  (Ignores BBWE UI commands)
  3 – Forced On   (Ignores BBWE UI commands)

  Default mode allows the enablement for the feature to be overwritten by BBWE
  UI commands.  Forced mode enablement is not be affected by BBWE UI commands.

  To determine if a feature is enabled or not based on enablement mode or UI 
  command refer to the truth table below:
                  
                        BBWE UI command
                  Enable         Disable      No Command
  Enable mode |_____________|_____________|______________
              |             |             |
  Default Off |   Enabled   |   Disabled  |   Disabled
  Default On  |   Enabled   |   Disabled  |   Enabled
  Forced Off  |   Disabled  |   Disabled  |   Disabled
  Forced On   |   Enabled   |   Enabled   |   Enabled
              |             |             |
              |             |             |

  If an enablement value specified is not supported, the table data registration
  associated with this parameter will be rejected.
*/

typedef struct vss_param_hdvoice_config_data_t vss_param_hdvoice_config_data_t;

#include "apr_pack_begin.h"
struct vss_param_hdvoice_config_data_t
{
  uint32_t feature_id;
    /**< Feature ID. Supported values: \n
         see @latexonly \hyperref[cal_feature]{Calibration Features}
         @endlatexonly. */
  uint32_t enable_mode;
    /**< The feature enablement mode. Supported values: \n
         - 0 –- VSS_PARAM_FEATURE_DEFAULT_OFF \n
         - 1 –- VSS_PARAM_FEATURE_DEFAULT_ON \n
         - 2 –- VSS_PARAM_FEATURE_FORCED_OFF \n
         - 3 –- VSS_PARAM_FEATURE_FORCED_ON */
  uint32_t rx_sampling_rate;
    /**< Supports the following values: \n
         - 16000 Hz */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_name_group Param ID definitions */

/** @} */ /* end_addtogroup vss_params_public_if */

#endif /* __VSS_PARAMS_PUBLIC_IF_H__ */

