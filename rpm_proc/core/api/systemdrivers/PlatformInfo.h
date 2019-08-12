#ifndef PLATFORMINFO_H
#define PLATFORMINFO_H

/**
  @file PlatformInfo.c

  Public header file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/PlatformInfo.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * Generates a platform version number.
 *
 * This macro generates the platform version number from the specified
 * major and minor numbers. For example, version 1.2 is represented by
 * @code PLATORMINFO_VERSION(1,2) @endcode
 *
*/
#define PLATFORMINFO_VERSION(major, minor)  (((major) << 16) | (minor))


/**
  Target type of the device on which the platform is running.
 */
typedef enum
{
  PLATFORMINFO_TYPE_UNKNOWN      = 0x00,  /**< Unknown target device. */
  PLATFORMINFO_TYPE_SURF         = 0x01,  /**< Target is a SURF device. */
  PLATFORMINFO_TYPE_FFA          = 0x02,  /**< Target is an FFA device. */
  PLATFORMINFO_TYPE_FLUID        = 0x03,  /**< Target is a FLUID device. */
  PLATFORMINFO_TYPE_FUSION       = 0x04,  /**< Target is a FUSION device. */
  PLATFORMINFO_TYPE_OEM          = 0x05,  /**< Target is an OEM device. */
  PLATFORMINFO_TYPE_QT           = 0x06,  /**< Target is a QT device. */
  PLATFORMINFO_TYPE_CDP          = PLATFORMINFO_TYPE_SURF,
                                          /**< Target is a CDP (aka SURF) device. */
  PLATFORMINFO_TYPE_MTP          = 0x08,  /**< Target is a MDM device. */
  PLATFORMINFO_TYPE_MTP_MDM      = PLATFORMINFO_TYPE_MTP,
                                          /**< Target is a MDM MTP device. */
  PLATFORMINFO_TYPE_MTP_MSM      = PLATFORMINFO_TYPE_MTP,
                                          /**< Target is a MSM MTP device. */
  PLATFORMINFO_TYPE_LIQUID       = 0x09,  /**< Target is a LiQUID device. */
  PLATFORMINFO_TYPE_DRAGONBOARD  = 0x0A,  /**< Target is a DragonBoard device. */
  PLATFORMINFO_TYPE_QRD          = 0x0B,  /**< Target is a QRD device. */
  PLATFORMINFO_TYPE_EVB          = 0x0C,  /**< Target is an EVB device. */
  PLATFORMINFO_TYPE_HRD          = 0x0D,  /**< Target is a HRD device. */
  PLATFORMINFO_TYPE_DTV          = 0x0E,  /**< Target is a DTV device. */
  PLATFORMINFO_TYPE_RUMI         = 0x0F,  /**< Target is on RUMI. */
  PLATFORMINFO_TYPE_VIRTIO       = 0x10,  /**< Target is on VIRTIO. */
  PLATFORMINFO_TYPE_GOBI         = 0x11,  /**< Target is a GOBI device. */
  PLATFORMINFO_TYPE_CBH          = 0x12,  /**< Target is a CBH device. */
  PLATFORMINFO_TYPE_BTS          = 0x13,  /**< Target is a BTS device. */
  PLATFORMINFO_TYPE_XPM          = 0x14,  /**< Target is a XPM device. */
  PLATFORMINFO_TYPE_RCM          = 0x15,  /**< Target is a RCM device. */
  PLATFORMINFO_TYPE_DMA          = 0x16,  /**< Target is a DMA device. */
  PLATFORMINFO_TYPE_STP          = 0x17,  /**< Target is a STP device. */
  PLATFORMINFO_TYPE_SBC          = 0x18,  /**< Target is a SBC device. */
  PLATFORMINFO_TYPE_ADP          = 0x19,  /**< Target is a ADP device. */

  PLATFORMINFO_NUM_TYPES         = 0x20,  /**< Number of known targets
                                               (including unknown). */
  /** @cond */
  PLATFORMINFO_TYPE_32BITS       = 0x7FFFFFFF
  /** @endcond */
} PlatformInfoPlatformType;

/**
 * Keys to get data out of the CDT.
 */
typedef enum
{
  PLATFORMINFO_KEY_UNKNOWN       = 0x00,
  PLATFORMINFO_KEY_DDR_FREQ      = 0x01,
  PLATFORMINFO_KEY_GFX_FREQ      = 0x02,
  PLATFORMINFO_KEY_CAMERA_FREQ   = 0x03,
  PLATFORMINFO_KEY_FUSION        = 0x04,
  PLATFORMINFO_KEY_CUST          = 0x05,

  PLATFORMINFO_NUM_KEYS          = 0x06,

  /** @cond */
  PLATFORMINFO_KEY_32BITS  = 0x7FFFFFFF
  /** @endcond */
} PlatformInfoKeyType;


/**
  @brief Stores the target platform, the platform version, and the
  platform subtype.
 */
typedef struct
{
  PlatformInfoPlatformType     ePlatform;   /**< Type of the current target. */
  uint32                       nVersion;    /**< Version of the platform in use. */
  uint32                       nSubtype;    /**< Sub-type of the platform. */
  uint32                       nFusion;     /**< TRUE if Fusion; FALSE otherwise. */
} PlatformInfoPlatformInfoType;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : PlatformInfo_GetPlatform
** =========================================================================*/
/**
  Returns the current platform.
*/
PlatformInfoPlatformType PlatformInfo_GetPlatform (void);


/* =========================================================================
**  Function : PlatformInfo_GetPlatformInfo
** =========================================================================*/
/**
  Returns information on the current platform.

  @return
    TRUE -- Success.
    FALSE -- The pInfo parameter is NULL.
*/
boolean PlatformInfo_GetPlatformInfo
(
  PlatformInfoPlatformInfoType *pInfo
);


/* =========================================================================
**  Function : PlatformInfo_GetKeyValue
** =========================================================================*/
/**
  Returns a value for a given key.

  @return
    TRUE -- Key was found in the CDT.
    FALSE -- Key wasn't found in the CDT.
*/
boolean PlatformInfo_GetKeyValue
(
  PlatformInfoKeyType eKey,
  uint32              *nValue
);


/* =========================================================================
**  Function : PlatformInfo_Init
** =========================================================================*/
/**
  Initializes the PlatformInfo driver.
*/
void PlatformInfo_Init (void);

#endif

