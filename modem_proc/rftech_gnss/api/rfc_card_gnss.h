
#ifndef RFC_CARD_GNSS_H
#define RFC_CARD_GNSS_H
/*!
  @file rfc_card_gnss.h

  @brief
  This file contains gnss mode data definitions and prototypes.

  @details

*/

/*===========================================================================

Copyright (c) 2008 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:49 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/api/rfc_card_gnss.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
10/31/13   shb     Converted TRX common device interface to C++
05/06/13   pv      Triton Support for multiple transceivers for GPS.
07/17/12   jr      Added support for non-ELNA configuration.
07/15/11   gy      Enabled command dispatch
06/28/11   gy      Added cmd for eLNA
05/26/11   dbz     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfc_card.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                           Data Definitions
===========================================================================*/

/* Enum for GNSS RFC command dispatch*/
typedef enum
{
  RFC_GNSS_ELNA_ON,
  RFC_GNSS_ELNA_OFF,
} rfcs_gnss_cmd_type;

/* ----------------------------------------------------------------------- */
/*!
   @brief
   GNSS Mode core configuration data types.

   @details

*/
typedef struct
{
	int sample1;
} rfc_gnss_core_config_type;

/*! @todo dchow:11/21/08: Sample implemented only.. needs definition */
/* ----------------------------------------------------------------------- */
/*!
   @brief
   GNSS Mode feature configuration data types.

   @details

*/
typedef struct
{
	int sample1;

} rfc_gnss_feature_config_type;

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This generic data structure defines the RF configuration attributes on
   a per mode and band basis.

   @details
   The core, feature and gpio configuration pointers will be updated
   with a mode change.  There will be a rfc_mode_attribute structure assigned
   on a path basis.

   The rx and tx band configuration pointers will be updated with a band
   change.

   Pointers not used in a mode or band should be assigned to NULL.
*/
typedef struct
{
   /*! RF mode ID to identify this structure type */
   rfcom_mode_enum_type mode_id;

   /*! ptr to card specfic GNSS core settings */
   rfc_gnss_core_config_type *rfc_core_config_ptr;

   /*! ptr to card specfic GNSS feature settings */
   rfc_gnss_feature_config_type *rfc_feature_config_ptr;

   /*! ptr to card specfic GNSS GPIO settings */
   rfc_gpio_config_type *rfc_gpio_config_ptr;

   /*! ptr to the power management configuration function ptrs */
   rfc_pwr_mgnt_type *rfc_pwr_mgnt_ptr;

} rfc_gnss_attribute_type;

/* ----------------------------------------------------------------------- */
/*!
   @brief
   GNSS LNA configuration types.

   @details

*/
typedef enum
{
   RFC_GNSS_ELNA,
   RFC_GNSS_NO_ELNA,
   RFC_GNSS_ILNA,
	
} rfc_gnss_lna_config_type;

void rfc_gnss_get_LNA_config(rfc_gnss_lna_config_type *lna);

void rfc_gnss_mode_config( rf_path_enum_type path );

rfdevice_rxtx_common_class* 
rfc_gnss_get_dev_obj(rfdevice_type_enum_type dev_type, uint8 instance);

void rfc_gnss_manage_vregs
( 
  rf_path_enum_type path, 
  rfcom_mode_enum_type mode, 
  rfm_path_state state
);

int32 rfc_gnss_command_dispatch
( 
  rf_path_enum_type path, 
  rfcs_gnss_cmd_type cmd, 
  void *data
);

#ifdef __cplusplus
}
#endif

#endif /* RFC_CARD_GNSS_H */
