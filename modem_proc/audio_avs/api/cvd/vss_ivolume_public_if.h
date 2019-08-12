#ifndef __VSS_IVOLUME_PUBLIC_IF_H__
#define __VSS_IVOLUME_PUBLIC_IF_H__

/**
  @file vss_ivolume_public_if.h
  @brief This file contains the APR API definitions for volume related 
  functions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_ivolume_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011, 2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_ivolume_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_ivolume_public_if
@{ */

/****************************************************************************
 * VOLUME DEFINES                                                           *
 ****************************************************************************/

/*---------------------------------------------------------------------------*
 * Volume Directions                                                         *
 *---------------------------------------------------------------------------*/

/** @name Volume Directions
@{ */

/** Select Tx path. */
#define VSS_IVOLUME_DIRECTION_TX ( 0 )

/** Select Rx path. */
#define VSS_IVOLUME_DIRECTION_RX ( 1 )

/** @} */ /* end_name_group Volume Directions */

/*--------------------------------------------------------------------------*
 * Mute Modes                                                               *
 *--------------------------------------------------------------------------*/

/** @name Mute Modes
@{ */

/** Disable mute. */
#define VSS_IVOLUME_MUTE_OFF ( 0 )

/** Enable mute. */
#define VSS_IVOLUME_MUTE_ON ( 1 )

/** @} */ /* end_name_group Mute Modes */


/****************************************************************************
 * MUTE API                                                                 *
 ****************************************************************************/

 /**
  This command changes the mute setting. The new mute setting will  
  be applied over the specified ramp duration.
  
  @par Payload
  #vss_ivolume_cmd_mute_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOLUME_CMD_MUTE_V2 ( 0x0001138B )

/** @brief Type definition for #vss_ivolume_cmd_mute_v2_t.
*/
typedef struct vss_ivolume_cmd_mute_v2_t vss_ivolume_cmd_mute_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOLUME_CMD_MUTE_V2 command.
*/
struct vss_ivolume_cmd_mute_v2_t
{
  uint16_t direction;
    /**< The direction field sets the direction to apply the mute command.
         The Supported values: \n
         #VSS_IVOLUME_DIRECTION_TX \n
         #VSS_IVOLUME_DIRECTION_RX */
  uint16_t mute_flag;
    /**< Turn mute on or off. The Supported values: \n
         #VSS_IVOLUME_MUTE_OFF \n
         #VSS_IVOLUME_MUTE_ON */
  uint16_t ramp_duration_ms;
    /**< Mute change ramp duration in milliseconds.
         The Supported values: 0 to 5000. */
}
#include "apr_pack_end.h"
;


/****************************************************************************
 * VOLUME APIS                                                              *
 ****************************************************************************/

 /**
  This command sets the total number of volume steps.

  @par Payload
  #vss_ivolume_cmd_set_number_of_steps_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS ( 0x000112C1 )

/** @brief Type definition for #vss_ivolume_cmd_set_number_of_steps_t.
 */
typedef struct vss_ivolume_cmd_set_number_of_steps_t vss_ivolume_cmd_set_number_of_steps_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS 
    command.
*/
struct vss_ivolume_cmd_set_number_of_steps_t
{
  uint32_t value;
    /**< The number of volume steps. */
}
#include "apr_pack_end.h"
;

/**
  This command sets the volume step.
    
  @par Payload
  #vss_ivolume_cmd_set_step_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  1. The default number of volume steps will be set to the number of volume
     calibration indices found in the registered volume calibration data.

  2. #VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS should be called after volume
     calibration data registration to override the default number of volume
     steps.

  3. The volume calibration data will contain n volume indices (0 to n-1 with
     0 being the lowest volume), where n is defined by client. This applies
     to #VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX and #VSS_IVOLUME_CMD_SET_STEP.
     #VSS_IVOLUME_CMD_SET_STEP should behave equivalently to
     #VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX when the number of volume steps
     is n.

  4. When the number of volume steps is set equal to the number of indices in
     the registered volume calibration data, #VSS_IVOLUME_CMD_SET_STEP is
     effectively a linear mapping.

  5. When the number of volume steps is set greater than the number of
     indices in the registered volume calibration data,
     #VSS_IVOLUME_CMD_SET_STEP causes interpolation of the volume level
     and search of the closest matching volume level in the registered
     volume calibration data to apply.

  6. When the number of volume steps is set lesser than the number of indices
     in the registered volume calibration data,
     #VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS and #VSS_IVOLUME_CMD_SET_STEP will
     fail.

  7. The OEM is responsible for ensuring the client required number of volume
     steps and the number of volume indices in the volume calibration data
     match unless interpolation is intended.

  8. #VSS_IVOLUME_CMD_SET_STEP applies the volume setting over the specified
     ramp duration in milliseconds.
*/
#define VSS_IVOLUME_CMD_SET_STEP ( 0x000112C2 )

/** @brief Type definition for #vss_ivolume_cmd_set_step_t.
*/
typedef struct vss_ivolume_cmd_set_step_t vss_ivolume_cmd_set_step_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOLUME_CMD_SET_STEP command.
*/
struct vss_ivolume_cmd_set_step_t
{
  uint16_t direction;
    /**< The direction field sets the direction to apply the volume command.
         The supported values: \n
         #VSS_IVOLUME_DIRECTION_RX */  
  uint32_t value;
    /**< Volume step used to find the corresponding linear volume and 
         the best match index in the registered volume calibration table. */
  uint16_t ramp_duration_ms;
    /**< Volume change ramp duration in milliseconds.
         The supported values: 0 to 5000. */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_ivolume_public_if */

#endif /* __VSS_IVOLUME_PUBLIC_IF_H__ */

