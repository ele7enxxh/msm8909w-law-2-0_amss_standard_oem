

#ifndef DALPLATFORMINFO_H
#define DALPLATFORMINFO_H

/** 
  Target type of the device on which the platform is running.
 */
typedef enum
{
  DALPLATFORMINFO_TYPE_UNKNOWN      = 0x00,  /**< Unknown target device. */
  DALPLATFORMINFO_TYPE_SURF         = 0x01,  /**< Target is a SURF device. */
  DALPLATFORMINFO_TYPE_FFA          = 0x02,  /**< Target is an FFA device. */
  DALPLATFORMINFO_TYPE_FLUID        = 0x03,  /**< Target is a FLUID device. */
  DALPLATFORMINFO_TYPE_FUSION       = 0x04,  /**< Target is a FUSION device. */
  DALPLATFORMINFO_TYPE_OEM          = 0x05,  /**< Target is an OEM device. */
  DALPLATFORMINFO_TYPE_QT           = 0x06,  /**< Target is a QT device. */
  DALPLATFORMINFO_TYPE_CDP          = DALPLATFORMINFO_TYPE_SURF, 
                                            /**< Target is a CDP (aka SURF) device. */
  DALPLATFORMINFO_TYPE_MTP_MDM      = 0x07,  /**< Target is a MDM MTP device. */
  DALPLATFORMINFO_TYPE_MTP_MSM      = 0x08,  /**< Target is a MSM MTP device. */
  DALPLATFORMINFO_TYPE_LIQUID       = 0x09,  /**< Target is a LiQUID device. */
  DALPLATFORMINFO_TYPE_DRAGONBOARD  = 0x0A,  /**< Target is a DragonBoard device. */

  DALPLATFORMINFO_NUM_TYPES         = 0x0B,  /**< Number of known targets 
                                               (including unknown). */
  /** @cond */
  DALPLATFORMINFO_TYPE_32BITS       = 0x7FFFFFFF
  /** @endcond */
} DalPlatformInfoPlatformType;


static __inline DalPlatformInfoPlatformType
DalPlatformInfo_Platform(void)
{
  
  return DALPLATFORMINFO_TYPE_UNKNOWN;

}


#endif /* DALPLATFORMINFO_H */
