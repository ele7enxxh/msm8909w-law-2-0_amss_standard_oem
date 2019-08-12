#ifndef __VSS_IMVM_PUBLIC_IF_H__
#define __VSS_IMVM_PUBLIC_IF_H__

/**
  @file vss_imvm_public_if.h
  @brief This file contains the APR API definitions for Multimode Voice Manager 
  (MVM) module.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CVD_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_imvm_public_if 
      group description in the CVD_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_imvm_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/


#include "apr_comdef.h"
#include "vss_imvm_deprecated_public_if.h"

/** @addtogroup vss_imvm_public_if
@{ */

/****************************************************************************
 * MULTIMODE VOICE MANAGER CORE APIS                                        *
 ****************************************************************************/

/**
  Creates a new passive control MVM session.
 
  Passive MVM sessions provide clients access to UI controls,
  such as Teletypewriter (TTY) toggle and WideVoice toggle, as well as connections
  between streams and vocprocs. The corresponding full control sessions
  internally manage additional system policies such as network setting and
  voice timing parameter setting.
 
  @par Payload
  The session name string used to identify the full control MVM session
  for which a handle is to be obtained. The string size, including the NULL 
  termination character, is limited to 31 characters.
 
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
 
  @comments
  Use the session name <i>default modem voice</i> to obtain a handle to the
  default MVM session for circuit-switched voice calls.
*/
#define VSS_IMVM_CMD_CREATE_PASSIVE_CONTROL_SESSION ( 0x000110FF )


/**
  Creates a new full control MVM session.
 
  Full MVM sessions give clients full control over the voice system,
  including network setting and voice timing parameter setting,
  in addition to UI control toggling and stream vocproc connections. 
 
  @par Payload
  The session name string given to the full control session being created.
  The string size, including the NULL termination character, is limited to 
  31 characters. The name must be NULL terminated if supplied. A 
  size 0 name is interpreted as a nameless session where only 
  the handle is used to access to the session. 
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
 
  @comments
  The session name <i>default modem voice</i> is reserved for the default MVM
  session for circuit-switched voice calls and cannot be given to any
  new full control sessions. Two or more full control MVM sessions cannot be 
  created with the same name.
*/
#define VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION ( 0x000110FE )


/**
  Sets dual control policy on MVM. In dual control mode, MVM state is controled 
  by both modem and apps. Dual control policy must be set on MVM immediately 
  after session creation and before starting voice. 

  @par Payload
  #vss_imvm_cmd_set_policy_dual_control_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/

#define VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL ( 0x00011327 )

/** @brief Type definition for #vss_imvm_cmd_set_policy_dual_control_t.
*/
typedef struct vss_imvm_cmd_set_policy_dual_control_t vss_imvm_cmd_set_policy_dual_control_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL command.
*/
struct vss_imvm_cmd_set_policy_dual_control_t
{
  bool_t enable_flag;
    /**< Set to TRUE to enable dual control policy. */
}
#include "apr_pack_end.h"
;


/**
  Attaches a stream to the MVM. The MVM symmetrically connects this
  stream to all the vocprocs currently attached to it. 
 
  @par Payload
  #vss_imvm_cmd_attach_stream_t
 
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_ATTACH_STREAM ( 0x0001123C )

/** @brief Type definition for #vss_imvm_cmd_attach_stream_t.
*/
typedef struct vss_imvm_cmd_attach_stream_t vss_imvm_cmd_attach_stream_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IMVM_CMD_ATTACH_STREAM command.
*/
struct vss_imvm_cmd_attach_stream_t
{
  uint16_t handle;
    /**< Stream handle to attach to the MVM. */
}
#include "apr_pack_end.h"
;


/**
  Detaches a stream from the MVM. The MVM disconnects this stream from
  all the vocprocs to which it is currently connected.
 
  @par Payload
  #vss_imvm_cmd_detach_stream_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_DETACH_STREAM ( 0x0001123D )

/** @brief Type definition for #vss_imvm_cmd_detach_stream_t.
*/
typedef struct vss_imvm_cmd_detach_stream_t vss_imvm_cmd_detach_stream_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IMVM_CMD_DETACH_STREAM command.
*/
struct vss_imvm_cmd_detach_stream_t
{
  uint16_t handle;
    /**< Stream handle to detach from the MVM. */
}
#include "apr_pack_end.h"
;


/**
  Attaches a vocproc to the MVM. The MVM symmetrically connects this
  vocproc to all the streams currently attached to it. 
 
  @par Payload
  #vss_imvm_cmd_attach_vocproc_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_ATTACH_VOCPROC ( 0x0001123E )

/** @brief Type definition for #vss_imvm_cmd_attach_vocproc_t.
*/
typedef struct vss_imvm_cmd_attach_vocproc_t vss_imvm_cmd_attach_vocproc_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IMVM_CMD_ATTACH_VOCPROC command.
*/
struct vss_imvm_cmd_attach_vocproc_t
{
  uint16_t handle;
    /**< Vocproc handle to attach to the MVM. */
}
#include "apr_pack_end.h"
;


/**
  Detaches a vocproc from the MVM. The MVM disconnects this vocproc from
  all the streams to which it is currently connected.
 
  @par Payload
  #vss_imvm_cmd_detach_vocproc_t
 
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_DETACH_VOCPROC ( 0x0001123F )

/** @brief Type definition for #vss_imvm_cmd_detach_vocproc_t.
*/
typedef struct vss_imvm_cmd_detach_vocproc_t vss_imvm_cmd_detach_vocproc_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IMVM_CMD_DETACH_VOCPROC command.
*/
struct vss_imvm_cmd_detach_vocproc_t
{
  uint16_t handle;
    /**< Vocproc handle to detach from the MVM. */
}
#include "apr_pack_end.h"
;


/**
  Starts a voice call.
 
  Placing the voice call into Start mode activates the vocproc and stream.
 
  @par Payload
  None.
 
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_START_VOICE ( 0x00011190 )


/**
  Places a voice call into Standby mode.
 
  Placing the voice call into Standby mode suspends the stream but keeps the
  vocproc active.
 
  @par Payload
  None.
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_STANDBY_VOICE ( 0x00011191 )


/**
  Places a voice call into Pause mode.
 
  Placing the voice call into Pause mode suspends the stream and the vocproc,
  without re-initializing the stream or the vocproc.
 
  @par Payload
  None.
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_PAUSE_VOICE ( 0x0001137D )


/**
  Stops a voice call.
 
  Placing the voice call into Stop mode suspends the stream and the vocproc
  activities and re-initializes the stream and the vocproc.
 
  @par Payload
  None.
 
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_STOP_VOICE ( 0x00011192 )


/****************************************************************************
 * MULTIMODE VOICE MANAGER CORE MODEM APIS                                  *
 ****************************************************************************/

/**
  Starts a voice call.
 
  Placing the voice call into Start mode activates the vocproc and stream.
 
  @par Payload
  None.
 
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_MODEM_START_VOICE ( 0x00011193 )

/**
  Places a voice call into Standby mode.
 
  Placing the voice call into Standby mode suspends the stream but keeps the
  vocproc active.
 
  @par Payload
  None.
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_MODEM_STANDBY_VOICE ( 0x00011194 )


/**
  Stops a voice call.
 
  Placing the voice call into Stop mode suspends the stream and the vocproc
  activities.
 
  @par Payload
  None.
 
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_MODEM_STOP_VOICE ( 0x00011195 )


/****************************************************************************
 * MULTIMODE VOICE MANAGER CALIBRATION FRAMEWORK APIS                       *
 ****************************************************************************/

/*
  Sets the network ID to be used for calibration.

  Stream and vocproc calibration for voice calls is dependent on the network. 
  The CVD client must provide the network ID to the MVM, which propagates it
  to the stream/s and vocproc/s attached to it for purposes of indexing into
  the calibration table and to apply the corresponding calibration data subset.

  If a network ID is not provided to the MVM by the client, no calibration data
  is applied by the stream/s and vocproc/s attached to the MVM.
 
  @par Payload
  #vss_imvm_cmd_set_cal_network_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  None.
*/
#define VSS_IMVM_CMD_SET_CAL_NETWORK ( 0x0001137A )

/** @brief Type definition for #vss_imvm_cmd_set_cal_network_t.
*/
typedef struct vss_imvm_cmd_set_cal_network_t vss_imvm_cmd_set_cal_network_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IMVM_CMD_SET_CAL_NETWORK command.
*/
struct vss_imvm_cmd_set_cal_network_t
{
  uint32_t network_id;
    /**< Calibration network ID. Supported values: \n
         see @latexonly \hyperref[cal_network_ID]{Calibration Network IDs} @endlatexonly. */
}
#include "apr_pack_end.h"
;


/*
  Sets the media type to be used for calibration.

  Stream and vocproc calibration is depenedent on the tx/rx sampling rate. It is
  also dependent on the vocoder class if such calibration data is provided to CVD.
  The CVD client must provide the calibration media type to the MVM which uses it
  for two purposes:
  
    1. Determining the tx and rx sample rates that the system should run at/be 
       calibrated for, and propagating them to the stream/s and vocproc/s attached
       to it for purposes of configuring their sample rates and indenxing into the
       calibration table to apply the corresponding calibration data subset. 
       (This is in the case of fixed sample rate vocoders. In the case of variable
       sample rate vocoders, the MVM cannot determine the sample rates from the 
       media type alone and requires the sample rates to be provided by the client:
       see #VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE)
    
    2. Determing the vocoder class and propagating it to the stream/s and vocproc/s
       attached to it for purposes of indexing into the calibration table to apply 
       the corresponding calibration data subset.
 
  @par Payload
  #vss_imvm_cmd_set_cal_media_type_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE ( 0x0001137B )

/** @brief Type definition for #vss_imvm_cmd_set_cal_media_type_t.
*/
typedef struct vss_imvm_cmd_set_cal_media_type_t vss_imvm_cmd_set_cal_media_type_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE command.
*/
struct vss_imvm_cmd_set_cal_media_type_t
{
  uint32_t media_id;
    /**< Media ID. Supported values: \n 
	 see @latexonly \hyperref[media_ID]{Media Type IDs}@endlatexonly. */
}
#include "apr_pack_end.h"
;


/*
  Sets the sampling rates to be used for vocoders that support variable
  sampling rates.

  If the vocoder provided to MVM via #VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE
  does not support variable sample rates, this command does not have any effect.

  If the vocoder provided to MVM via #VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE
  supports variable sample rates and no sample rates are provided to MVM
  (i.e. this command is not called) then defaults are used.
 
  @par Payload
  #vss_imvm_cmd_set_cal_var_voc_sampling_rate_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE ( 0x0001137C )

/** @brief Type definition for #vss_imvm_cmd_set_cal_var_voc_sampling_rate_t.
*/
typedef struct vss_imvm_cmd_set_cal_var_voc_sampling_rate_t vss_imvm_cmd_set_cal_var_voc_sampling_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE command.
*/
struct vss_imvm_cmd_set_cal_var_voc_sampling_rate_t
{
  uint32_t rx;
    /**< Rx sample rate. Supported values: \n
         - 8000 Hz \n
         - 16000 Hz \n
         - 48000 Hz */
  uint32_t tx;
    /**< Tx sampling rate. Supported values: \n
         - 8000 Hz \n
         - 16000 Hz \n
         - 48000 Hz */
}
#include "apr_pack_end.h"
;


/** @} */ /* end_addtogroup vss_imvm_public_if */

#endif /* __VSS_IMVM_PUBLIC_IF_H__ */

