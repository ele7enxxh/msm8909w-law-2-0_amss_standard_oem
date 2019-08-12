#ifndef __VSS_ISOUNDFOCUS_PUBLIC_IF_H__
#define __VSS_ISOUNDFOCUS_PUBLIC_IF_H__

/*===========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * SOUND FOCUS DEFINES                                                      *
 ****************************************************************************/

/** Indicates that the sector is not used. */
#define VSS_ISOUNDFOCUS_SECTOR_NOT_USED ( 0xFF )

/** Indicates that the gain step value is ignored. */
#define VSS_ISOUNDFOCUS_GAIN_STEP_NOT_USED ( 0xFFFF )

/****************************************************************************
 * SOUND FOCUS APIS                                                         *
 ****************************************************************************/

/** 
  Sets the sectors for the sound focus feature. 

  @apr_msgpayload{vss_isoundfocus_cmd_set_sectors_t}
 
  @detdesc
  When device is being held in portrait position, using the wall clock
  reference, twelve o'clock represents zero degree. The angles are
  incremental in counter-clockwise direction.
  A sector is defined as the region between two boundary angles.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISOUNDFOCUS_CMD_SET_SECTORS ( 0x00013133 )

/* Type definition for vss_isoundfocus_cmd_set_sectors_t. */
typedef struct vss_isoundfocus_cmd_set_sectors_t vss_isoundfocus_cmd_set_sectors_t;

#include "apr_pack_begin.h"

/* Payload structure for the VSS_ISOUNDFOCUS_CMD_SET_SECTORS command.
*/
struct vss_isoundfocus_cmd_set_sectors_t
{
  uint16_t start_angles[ 8 ];
    /**< An array of boundary angles (in degrees) that form the sectors 
         to configure.
         The maximum number of supported sectors is eight. Client may
         configure less than eight sectors.
         With the exception of the last sector, sector n (where n=1,2,3,...,7)
         is defined as the region from start_angles[n-1] to start_angles[n].
         If sector N is the last sector to be configured (where N=1,2,3,...,8),
         it is defined as the region from start_angles[N-1] to start_angles[0]
         (if N=1, the sector represents the whole 360 degree plane).
     
         @values
         - Integer between 0 and 359.
         - Values in this array must be strictly increasing.
         - Values for un-used sectors are ignored. */

  uint8_t enables[ 8 ];
    /**< An array of enable flags corresponding to each sector. 
         The maximum number of supported sectors is eight.
         Client may configure less than eight sectors. In such case, client
         shall use #VSS_ISOUNDFOCUS_SECTOR_NOT_USED to indicate un-used
         sectors.
 
         @values 
         - #TRUE -- The sector is enabled. 
         - #FALSE -- The sector is disabled. 
         - #VSS_ISOUNDFOCUS_SECTOR_NOT_USED -- The sector is not used. */

  uint16_t gain_step;
    /**< The gain step value to be applied to all enabled sectors.
         Using gain step to change volume on Tx (uplink) path must be 
         restricted with the sound focus feature. It must not be used for 
         stand-alone voice call use case. 
         The client is reponsible for ensuring that the gain step does not 
         lie outside of the supported range, as defined by the voice Tx 
         path calibration database. 
         
         @values
         - Integer that lies in the supported range, as defined by the voice
           Tx path calibration database. 
         - #VSS_ISOUNDFOCUS_GAIN_STEP_NOT_USED - Gain adjustment is ignored. */
}
#include "apr_pack_end.h"
;

/** 
  Gets the sectors information.

  @apr_msgpayload 
  None.

  @detdesc
  Default values will be returned if this command is called before 
  #VSS_ISOUNDFOCUS_CMD_SET_SECTORS has been successfully processed, or if the 
  sound focus algorithm has not started to run yet.
   
  @return
  #VSS_ISOUNDFOCUS_RSP_GET_SECTORS - Success. Sector information was retrieved. 
  @par 
  #APRV2_IBASIC_RSP_RESULT - Failure; Sector information was not retrieved. 

  @dependencies
  None.
 */
#define VSS_ISOUNDFOCUS_CMD_GET_SECTORS ( 0x00013134 )

/** 
  Response to the #VSS_ISOUNDFOCUS_CMD_GET_SECTORS command.

  @apr_msgpayload{vss_isoundfocus_rsp_get_sectors_t}
   
  @detdesc 
  The response includes sector boundary angles, enable flag for each sector, 
  and gain step. 
   
  @return
  None.

  @dependencies
  None.
 */
#define VSS_ISOUNDFOCUS_RSP_GET_SECTORS ( 0x00013135 )

/* Type definition for vss_isoundfocus_rsp_get_sectors_t. */
typedef struct vss_isoundfocus_rsp_get_sectors_t vss_isoundfocus_rsp_get_sectors_t;

#include "apr_pack_begin.h"

/* Payload of the VSS_ISOUNDFOCUS_RSP_GET_SECTORS response.
*/
struct vss_isoundfocus_rsp_get_sectors_t
{
  uint16_t start_angles[ 8 ];
    /**< An array of boundary angles (in degrees) that form the sectors. 
         With the exception of the last sector, sector n (where n=1,2,3,...,7)
         is defined as the region from start_angles[n-1] to start_angles[n].
         If sector N is the last sector to be configured (where N=1,2,3,...,8),
         it is defined as the region from start_angles[N-1] to start_angles[0]
         (if N=1, the sector represents the whole 360 degree plane).
     
         @values
         - Integer between 0 and 359.
         - Values in this array must be strictly increasing.
         - Values for un-used sectors shall be ignored. 
         - By default, all sectors are enabled, in which case values for angles 
           shall be ignored. */

  uint8_t enables[ 8 ];
    /**< An array of enable flags corresponding to each sector. 
     
         @values 
         - #TRUE -- The sector is enabled. By default, all sectors are enabled. 
         - #FALSE -- The sector is disabled. 
         - #VSS_ISOUNDFOCUS_SECTOR_NOT_USED -- The sector is not used. */

  uint16_t gain_step;
    /**< The gain step value that has been applied to all enabled sectors.

         @valuse
         - Integer that lies in the supported range, as defined by the voice
           Tx path calibration database. 
         - #VSS_ISOUNDFOCUS_GAIN_STEP_NOT_USED - Default value. Gain 
           adjustment is ignored.  */
}
#include "apr_pack_end.h"
;

#endif /* __VSS_ISOUNDFOCUS_PUBLIC_IF_H__ */


