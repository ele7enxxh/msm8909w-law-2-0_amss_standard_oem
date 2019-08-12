#ifndef RFM_DEVICE_CONFIG_TYPE_H
#define RFM_DEVICE_CONFIG_TYPE_H

/*
   @file
   rfm_device_config_type.h

   @brief
   RF Driver's common external interface file for device configuration
   information.

   @details

*/

/*===========================================================================
Copyright (c) 2011-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_device_config_type.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/17/14   krg     Updated the rf-trm interface structures for prx/drx and sub id
10/07/14   tks     Updated the rf-trm interface structures
09/02/14   Saul    Renamed var.
08/08/14   Saul    MULTI-SIM Capability Type For TRM
07/25/14   tks     Added new enum and strucut for reconfig status indication
05/28/14   tks     Added trm interface api's for concurrency management
03/13/14   sbm     3 mode support for SLTE.
02/18/14   kg      Update struct for shdr
01/03/13   vrb     Update TRM intf for EFS RFC compatibility
09/05/12   vrb     Deprecated the tech_supported and ant_supported elements
08/17/12   vrb     Further Updates to the RF interface to TRM
07/26/12   vrb     Updated interface to TRM
05/18/12   shb     Added gps_has_independent_receive_chain to 
                   rfm_devices_configuration_type
10/19/11   bmg     Added device/band concurrency restrictions data to the
                   rfm_devices_configuration_type data structure.
06/29/11   cd      Initial revision

============================================================================*/
#include "rfm_mode_types.h"
#include "rfm_device_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure defines a combination of devices, technologies, and bands
  which are incompatible in the current configuration.

  @details
  This data structure is used to return a table of mode combinations which
  are not compatible with the RF driver front end configuration in the
  target.  Most simultaneous configurations are expected to be valid, so
  only the disallowed configurations will be listed.

  As an example, assume an SVDO configuration where 1x and DO have no
  restrictions on single-chain operation, but if both 1x and HDR are
  on in BC0, neither technology can enable diversity.  This would be
  the case if there are only two BC0 antennas in the system, such that
  1x is using one antenna for its single-chain BC0 operation, and DO
  is using the other antenna for its single-chain BC0 operation.  With
  no antennas left, neither 1x nor DO can enable diversity.  Assume:
  - RFM_DEVICE_0: HDR Rx and Tx
  - RFM_DEVICE_1: HDR Rx diversity
  - RFM_DEVICE_2: 1x Rx and Tx
  - RFM_DEVICE_3: GPS
  - RFM_DEVICE_4: 1x Rx diversity

  A table with these rows:
  - { RFM_DEVICE_0, RFM_1XEVDO_MODE, RFM_CDMA_BC0, RFM_DEVICE_4, RFM_1X_MODE, RFM_CDMA_BC0 },
  - { RFM_DEVICE_2, RFM_1X_MODE, RFM_CDMA_BC0, RFM_DEVICE_1, RFM_1XEVDO_MODE, RFM_CDMA_BC0 }

  would mean:
  - "If HDR is on Device 0 on BC0 => 1x cannot use Device 4 for BC0 (diversity) AND
    If 1x is on Device 4 on BC0 (diversity) => HDR cannot use Device 0 for BC0"
  - "If 1x is on Device 2 on BC0 => HDR cannot use Device 1 for BC0 (diversity) AND
    If HDR is on Device 1 on BC0 (diversity) => 1x cannot use Device 2 for BC0"
*/
typedef struct rfm_concurrency_restriction_s
{
  rfm_device_enum_type device1;
  rfm_mode_enum_type   mode1;
  int32                band1;

  rfm_device_enum_type device2;
  rfm_mode_enum_type   mode2;
  int32                band2;
} rfm_concurrency_restriction_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure defines a bit mask for device capabilities

  @details
  RFM_DEVICE_RX_SUPPORTED represents whether Rx functionality is supported on
  a logical device.
  RFM_DEVICE_TX_SUPPORTED represents whether Tx functionality is supported on
  a logical device,
  RFM_DEVICE_PREFERRED_SUBS_2 represents whether the logical device should be
  the preferred choice for a second subscription under DSDA context
*/
typedef enum
{
  RFM_DEVICE_RX_SUPPORTED = 0x01,
  RFM_DEVICE_TX_SUPPORTED = 0x02,
  RFM_DEVICE_PREFERRED_SUBS_2 = 0x04
}rfm_device_capability_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure defines a bit mask for Multi-SIM capabilities

  @details
  RFM_SINGLE_RX_DSDS_SUPPORTED - In this dual-sim mode, only one technology is
    active at a given time, other technology is suspended when new tech comes
    up. Also referred to as tune away DSDS.
  RFM_DUAL_RX_DSDS_SUPPORTED  - Two receivers can be active simultaneously. For
    eg. LTE B1 TX+RX in connected state and GSM in paging. Some transceivers
    support DR-DSDS with a single WTR, some bands can be supported without
    diversity sharing (i.e. without giving up diversity). Some transceivers can
    only do DR-DSDS with diversity sharing since WTR3905 can only have two LNAs
    ON at any given time.
  RFM_DUAL_TX_DSDA_SUPPORTED  - This is supported on HW that has two independent
    TX chains available for DSDA, going out of two independent antennas. This is
    legacy DSDA implementation.
  RFM_SINGLE_TX_DSDA_SUPPORTED - One TX chain is time multi-plexed between two
    technologies. For eg, with some transceivers, LTE and GSM can be in
    connected mode simultaneously. LTE RX on CA1, GSM RX on CA2, and TX time
    shared between LTE and GSM.

*/

typedef enum                           
{                                      
  RFM_SINGLE_RX_DSDS_SUPPORTED = 0x01, 
  RFM_DUAL_RX_DSDS_SUPPORTED = 0x02,   
  RFM_DUAL_TX_DSDA_SUPPORTED = 0x04,   
  RFM_SINGLE_TX_DSDA_SUPPORTED = 0x08  
}rfm_multisim_capability_type;         



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure defines a bit mask of bands

  @details
  This data structure contains an array of n elements to represent the bands
  supported, based on a bit mask of type sys_band_class_e_type
*/
typedef struct 
{
  uint64 mask[3];
}rfm_bands_bitmask;

typedef struct 
{  
  rfm_mode_enum_type tech;
  int32 band; /* sys_band_class_e_type*/
}rfm_tech_band_type;

typedef struct 
{
  rfm_tech_band_type concurrency1;
  rfm_tech_band_type concurrency2;
  rfm_tech_band_type concurrency3;
  rfm_device_enum_type concurrency3_device;
} rfm_3mode_support_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure explains the features that are supported by all devices.
*/
typedef struct 
{
  uint32 hwid ; 

  uint32 multi_tech_support[RFM_NUM_MODES][RFM_NUM_MODES];
  /*!<
  @brief
  Two dimensional matrix of Masks which will be used to indicate if
  simultaneous multi-tech is supported.

  @details
  This two dimensional matrix will be a symmetrical matrix. Each field will be a 
  mask where bit 0 indicates if simultaneous Rx is possible. On the other hand 
  bit 1 will indicate if simultaneous Tx is possible. For instance, 
  multi_tech_support[1x][HDR] = 0b00000011 indicates that SVDO is supported
  multi_tech_support[1x][HDR] = 0b00000001 indicated that SHDR is supported
  multi_tech_support[1x][LTE] = 0b00000011 indicated that SVLTE is supported
  multi_tech_support[GSM][LTE] = 0b00000011 indicated that SGLTE is supported

  Bits 2 to bit 7 are reserved for future usage.

  The enum list in rfm_simultaneous_op_enum_type will be used to
  identify a bit corresponding to a particular simultaneous RF operation. For 
  instance, (RFM_SIMULTANEOUS_RX, enum in rfm_simultaneous_op_enum_type)'th
  bit starting from Least significant bit will indicate if simultaneous Rx is 
  supported. 1 in the bit-field indicates that simultaneous RF is 
  supported; whereas 0 indicates no support.
  */

  struct
  {
    rfm_bands_bitmask bands_supported;  
    /*!<
    @brief
    192 Bits of the Bands Supported Bit Mask

    @details
    Bits 0-191 of the Bit mask indicating the bands supported by this 
    logical device. 
    This uses the type sys_band_class_e_type to construct the bitmask
    */

    rfm_bands_bitmask ca1_bands_supported;  

    rfm_bands_bitmask dr_bands_supported ;
    /*!<
    @brief
    192 Bits of the Bands Supported Bit Mask for the first aggregated 
    carrier

    @details
    Bits 0-191 of the Bit mask indicating the bands supported on the first 
    aggregated carrier by this logical device. 
    This uses the type sys_band_class_e_type to construct the bitmask
    */

    rfm_bands_bitmask ho_rxd_bands_supported;

    uint32 capability;
    /*!<
    @brief
    A bit mask to identify the capabilities of the RFM device

    @details
    The bits of the bitmask is defined by rfm_device_capability_type
    */ 

    rfm_device_enum_type associated_device;
    /*!<
    @brief
    Associated Logical Device 

    @details
    Indicates the logical device that is associated with this device. 
    Typically, RFM_DEVICE_1 would operate as the RxD device to RFM_DEVICE_0 
    In this case, the associated_device for RFM_DEVICE_1 would be 0 and vice 
    versa. 
    */ 
  
  } device_support[RFM_MAX_DEVICES];
  /*!< This describes the functionality that each RF Device supports */ 

  /*!
    @brief
    This structure refers to an array of concurrency restrictions in the
    current RF configuration.

    @details
    The concurrency restrictions list modes and bands of operation that are
    incompatible with the current RF hardware configuration.  This will
    commonly be due to inter-antenna band restrictions, where two RF devices
    cannot simultaneously use the same antenna in certain bands.

    This incompatiblity table only describes restrictions due to concurrency,
    it does not provide any information about non-concurrent operation of
    a device on a given band or technology.  The basic capabilities of
    a device in a technology and band must be validated with the 
    rfm_is_band_chan_supported(), rfm_is_band_chan_supported_v2(), and
    rfm_is_tunable_on_antenna() APIs.
  */
  struct
  {
    /*!
      @brief
      The number of entries in the array pointed to by the restriction_table
      member of this structure.

      @details
      If this number is zero, the restriction_table member may be NULL, and
      there are no concurrency restrictions listed.
    */
    int32 num_restrictions;

    /*!
      @brief
      A pointer to the beginning of table of concurrency restrictions.

      @details
      If the num_restrictions member of this structure is zero, this
      pointer may be NULL, and should never be accessed.  Otherwise,
      this pointer will point to the beginning of an array of
      N rfm_concurrency_restriction_type entries, where N is
      num_restrictions.
    */
    const rfm_concurrency_restriction_type* restriction_table;
  } concurrency_restrictions;

  struct
  {
    /*!
      @brief
      The number of entries in the array pointed to by the support_3mode_tbl
      member of this structure.

      @details
      If this number is zero, the 3mode support_3mode_tbl may be NULL, and
      there are no entries listed.
    */

    int32 num_3mode_entries;
    
    /*!
      @brief
      A pointer to the beginning of table of 3mode support entries.

      @details
      If the num_3mode_entries member of this structure is zero, this
      pointer may be NULL, and should never be accessed.  Otherwise,
      this pointer will point to the beginning of an array of
      N rfm_3mode_support_type entries, where N is
      num_3mode_entries.
    */

    const rfm_3mode_support_type* support_3mode_tbl;
  } support_3mode;

  /*!
    @brief
    Flag indicating if GPS and SHDR can operate concurrently
  
    @details
    Some RF Devices like WTR1605 have independent SHDR and GPS PLL enabling
    concurrent operation of GPS and SHDR. This flag informs TRM whether the
    current RF hardware supports this configuration.
  */
  boolean gps_has_independent_receive_chain;

  boolean antenna_swap_supported;
  /* Bitmask, indexed by enums of rfm_multisim_capability_type */
  uint32 concurrency_features;
  
} rfm_devices_configuration_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  enum to differentiate the logical device reservation 
 
  @details
  RFM_DEVICE_ASSIGNED means that a particular logical device has been
  reserved for requesting tech
 
  RFM_DEVICE_EXPECTED means that a particular logical device is expected
  to have reservation for a new tech
 
  RFM_DEVICE_ALLOCATION_INVALID means that for a particular logical device
  there is no allocation 
 
*/
typedef enum
{
  RFM_DEVICE_ASSIGNED,
  RFM_DEVICE_EXPECTED,
  RFM_DEVICE_ALLOCATION_INVALID
}rfm_device_assignment_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  enumeration to indicate prx/drx for the concurrency manager
*/
typedef enum
{
  RFM_PRX_OPERATION = 0,
  RFM_DRX_OPERATION,
  RFM_INVALID_OPERATION = 0xFF 
}rfm_device_rx_operation_type; 

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  enumeration to indicate sub ID for the concurrency manager
*/
typedef enum
{
  RFM_SUB_ID_1 = 0,               /* Multi-mode subscription*/
  RFM_SUB_ID_2,                   /* Typically GSM only subscription*/
  RFM_INVALID_SUB_ID = 0xFF 
}rfm_device_sub_id_type; 

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold the tech, band and logical device allocation information
 
  @details
  The structure is used primarily for TRM interface. The structure is populated
  by TRM and used to determine the concurrency scenario. 
 
*/
typedef struct
{
  struct
  {
    rfm_mode_enum_type tech;
    rfm_device_assignment_type allocation_type;
    sys_band_class_e_type band;
    uint8 alt_path; 
    rfm_device_rx_operation_type rx_operation;
    rfm_device_sub_id_type sub_id;
  }device_allocation[RFM_MAX_DEVICES];
}rfm_device_allocation_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold the tech, band and logical device allocation information
 
  @details
  The structure is used primarily for TRM interface. The structure is populated
  by TRM and used to determine the concurrency scenario. 
 
*/
typedef struct
{
  rfm_device_enum_type rfm_dev;
  rfm_mode_enum_type tech;
  sys_band_class_e_type band;
  rfm_device_rx_operation_type rx_operation;
  rfm_device_sub_id_type sub_id;
}rfm_device_client_info_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  enumeration to indicate re-config status to trm
*/
typedef enum
{
  RFM_DEVICE_RECONFIG_REQUIRED = 0,
  RFM_DEVICE_RECONFIG_NOT_REQD = 1, 
  RFM_DEVICE_CURRENTLY_INACTIVE = 2, 
  RFM_DEVICE_STATUS_NUM,
  RFM_DEVICE_STATUS_INVALID
}rfm_device_reconfig_status_type; 

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold the reconfiguration status for all logical devices
 
  @details
  The structure is used primarily for TRM interface. It is populated by RF
  concurrency mananger as part of the validate scenario check. RF response
  is used by TRM to decide on whether a chain should be granted or not. 
 
*/
typedef struct
{
  rfm_device_reconfig_status_type curr_status[RFM_MAX_WAN_DEVICES]; 
  uint8 alt_path[RFM_MAX_WAN_DEVICES]; 
}rfm_device_allocation_status_type; 

/*----------------------------------------------------------------------------*/
boolean
rfm_get_devices_configuration
(
  rfm_devices_configuration_type *dev_cfg
);

/*----------------------------------------------------------------------------*/
boolean
rfm_inform_device_allocation
(
  rfm_device_allocation_type *dev_allocation
);

/*----------------------------------------------------------------------------*/
boolean
rfm_validate_device_concurrency
(
  rfm_device_client_info_type *dev_client 
);

/*----------------------------------------------------------------------------*/
rfm_device_allocation_status_type* rfm_verify_rf_concurrency
(
  rfm_device_client_info_type *dev_client
); 

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* RFM_DEVICE_CONFIG_TYPE_H */


