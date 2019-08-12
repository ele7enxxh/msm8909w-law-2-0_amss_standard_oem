#ifndef __ADSP_VPM_API_H__
#define __ADSP_VPM_API_H__

/**
  @file adsp_vpm_api.h
  @brief This file contains API definitions for the Voice Processing Manager 
  (VPM).
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Voice_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Voice_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================

  Copyright (c) 2010, 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/13   sw      (Tech Pubs) Updated Doxygen for 2.0 internal draft.
02/27/12   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/11/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
12/09/10    dp     Update VPM version, since VFR API changed (common VSM/VPM) 
===========================================================================*/

#include "mmdefs.h"
#include "adsp_vcmn_api.h"

/** @ingroup vpm_version
    Version of the aDPS VPM module.
*/
#define ADSP_VPM_VERSION    0x00070005


/*============================================================================
   VPM 8x60 ISOD commands
============================================================================*/

/** @ingroup vpm_cmd_start_session
    Starts any existing Tx/Rx path on a specified device session.

  @apr_hdr_fields
    Opcode -- VPM_CMD_START_SESSION \n
    Dst_port -- Handle to the session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
/* Q3 = Async Packet Router API (for MDM9x15 and Newer Devices) Interface Spec
        (80-N1463-2)*/
#define VPM_CMD_START_SESSION ( 0x00011002 )

/** @ingroup vpm_cmd_stop_session
    Stops any existing Tx/Rx path on a specified device session.

  @apr_hdr_fields
    Opcode -- VPM_CMD_STOP_SESSION \n
    Dst_port -- Handle to the session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
#define VPM_CMD_STOP_SESSION ( 0x00011003 )

/** @ingroup vpm_cmd_destroy_session
    Destroys any existing Tx/Rx paths on a specified device session.

  @apr_hdr_fields
    Opcode -- VPM_CMD_DESTROY_SESSION \n
    Dst_port -- Handle to the session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created, and it must be in the Stop state.
*/
#define VPM_CMD_DESTROY_SESSION ( 0x00011004 )

/*cond OEM_only */
/** @addtogroup vpm_cmd_config_tx_dtmf
@{ */
/** Configures DTMF detection in the Tx direction.

  @apr_hdr_fields
    Opcode   -- VPM_CMD_SET_TX_DTMF_DETECTION \n
    Dst_port -- Handle of the client that enabled DTMF detection

  @msg_payload{vpm_set_tx_dtmf_detection_t}
    @table{weak__vpm__set__tx__dtmf__detection__t}

  @description
    The DTMF status is sent to the client that requested detection via
    #VPM_EVT_TX_DTMF_DETECTED. Only one client can request Tx DTMF detection on
    a given stream at a given time. If another client requests Tx DTMF
    detection while the previous client's Tx DTMF detection is still active,
    the request fails.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
#define VPM_CMD_SET_TX_DTMF_DETECTION  ( 0x00011033 )

/** Valid values for configuring DTMF detection on the Tx path.
*/
typedef enum
{
   VPM_TX_DTMF_DETECTION_DISABLE = 0, /**< Tx DTMF detection is disabled. */
   VPM_TX_DTMF_DETECTION_ENABLE       /**< Tx DTMF detection is enabled. */
}vpm_set_tx_dtmf_detection_mode_t;

/** @} */ /* end_addtogroup vpm_cmd_config_tx_dtmf */

/* Used by the structure that allows the client to enable or disable 
  DTMF detection. */
typedef struct vpm_set_tx_dtmf_detection_t vpm_set_tx_dtmf_detection_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vpm_set_tx_dtmf_detection_t
@{ */
/* Payload of the VPM_CMD_SET_TX_DTMF_DETECTION command, which allows
    the client to enable DTMF detection on the Tx path.
*/
struct vpm_set_tx_dtmf_detection_t
{
   uint32_t enable;
   /**< Specifies whether Tx DTMF detection is disabled.

        @values
        - #VPM_TX_DTMF_DETECTION_DISABLE
        - #VPM_TX_DTMF_DETECTION_ENABLE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vpm_set_tx_dtmf_detection_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vpm_evt_tx_dtmf_detected
    Event sent by the stream to the client that enabled Tx DTMF detection
    whenever DTMF has been detected on the Tx path.

  @apr_hdr_fields
    Opcode -- VPM_EVT_TX_DTMF_DETECTED \n
    Dst_port -- Handle of the client that enabled it

  @msg_payload{vpm_tx_dtmf_detected_t}
    @table{weak__vpm__tx__dtmf__detected__t}

  @description
    The DTMF detection feature can only be used to detect DTMF frequencies as
    listed in ITU-T Recommendation Q.23, Section 6 (refer to
    @xhyperref{S1,[S1]}) and not to arbitrary frequencies. Therefore, the
    possible values for low and high frequencies are listed in
    %vpm_tx_dtmf_detected_t.

  @return
    None.

  @dependencies
    A valid session must have been created.
    @par
    Tx DTMF detection must be enabled for these events to be generated.
 */
#define VPM_EVT_TX_DTMF_DETECTED ( 0x00011034 )

/* Used by the structure that sends a DTMF detection tone status to the 
  client that enabled it. */
typedef struct vpm_tx_dtmf_detected_t vpm_tx_dtmf_detected_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vpm_tx_dtmf_detected_t
@{ */
/* Payload of the VPM_EVT_TX_DTMF_DETECTED event, which sends a DTMF
    detection tone status to the client that enabled it.
*/
struct vpm_tx_dtmf_detected_t
{
   uint16_t low_freq;
   /**< Low frequency detected in the DTMF tone.

        @values 697, 770, 852, 941 Hz */

   uint16_t high_freq;
   /**< High frequency detected in the DTMF tone.

        @values 1209, 1336, 1477, 1633 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vpm_tx_dtmf_detected_t */
/*endcond */

/*cond OEM_only */
/** @addtogroup vpm_cmd_create_session
@{ */
/** Creates a paired Tx/Rx device leg of a voice call.

  @apr_hdr_fields
    Opcode   -- VPM_CMD_CREATE_SESSION_V2 \n

  @msg_payload{vpm_create_session_v2_t}
    @table{weak__vpm__create__session__v2__t}

  @description
    A voice processing session is a paired Tx/Rx session with postprocessing
    blocks. If the Tx or Rx port is set to NO_INTERFACE, the corresponding
    module is not created.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    - Includes the handle to the paired Tx and Rx
    - Is NULL if there is an error

  @dependencies
    This is the first command before starting a voice processing session.
*/
#define VPM_CMD_CREATE_SESSION_V2 ( 0x00011037 )

/** Topology ID for Tx pass through. */
#define VPM_TX_NONE       ( 0x00010F70 )

/** @cond OEM_only */
/** @xreflabel{hdr:ParamVpmTxSmEcnsTopo}
    Topology ID for Tx single-mic echo cancellation noise suppression.
	@newpage */
#define VPM_TX_SM_ECNS    ( 0x00010F71 )

/** @xreflabel{hdr:ParamVpmTxDmFluenceTopo}
    Topology ID for the Tx dual-mic Fluence&tm;&nbsp;algorithm. */
#define VPM_TX_DM_FLUENCE ( 0x00010F72 )

/** @xreflabel{hdr:ParamVpmTxSmFluenceV5Topo}
    Topology ID for the Tx single-mic Fluence V5 algorithm. */
#define VPM_TX_SM_FLUENCEV5 ( 0x00010F73 )

/** @xreflabel{hdr:ParamVpmTxDmFluenceV5Topo}
    Topology ID for the Tx dual-mic Fluence V5 algorithm. */
#define VPM_TX_DM_FLUENCEV5 ( 0x00010F74 )

/** @xreflabel{hdr:ParamVpmTxQmFluenceProV2Topo}
    Topology ID for the Tx quad-mic Fluence Pro V2 algorithm. */
#define VPM_TX_QM_FLUENCE_PROV2 ( 0x00010F87 )

/** @xreflabel{hdr:ParamVpmTxDmFluenceV5BroadSideTopo}
    Topology ID for the Tx dual-mic  Fluence V5 broadside spkphone algorithm. */
#define VPM_TX_DM_FLUENCEV5_BROADSIDE ( 0x00010F88 )

/** @xreflabel{hdr:ParamVpmTxTmFluenceTopo}
    Topology ID for the Tx tri-mic Fluence algorithm. */
#define VPM_TX_TM_FLUENCE ( 0x00010F80 )

/** @xreflabel{hdr:ParamVpmTxQmFluenceProTopo}
    Topology ID for the Tx quad-mic Fluence Pro algorithm. */
#define VPM_TX_QM_FLUENCE_PRO ( 0x00010F75 )

/** @xreflabel{hdr:ParamVpmTxDmFluenceTopo}
    Topology ID for the Tx dual-mic Snapdragon Voice Plus&tm;&nbsp; ECNS algorithm. */
#define VPM_TX_DM_VPECNS ( 0x00010F86 )
/** @endcond */

/** This is the topology ID for exercising dynamically loadable voice 
  modules in the shared object voiceproc_tx.so. This topology is specific 
  and needs to be used by modules that operate on the uplink device path. 
  The media type supported is defined by the following:
  1. The input and output of this topology needs to be Signed 16-bit Mono. 
  2. Sampling rate of this topology is defined by the VPM_CMD_CREATE_SESSION_V2 command.
  3. The reference signal is Signed 16-bit Mono and is of the same sampling rate as the primary.
*/
#define VPM_TX_SINGLE_MIC_DYNAMIC_TOPOLOGY (0x00010F83)

/** This is the topology ID for exercising dynamically loadable voice 
  modules in the shared object voiceproc_tx.so. This topology is specific 
  and needs to be used by modules that operate on the uplink device path. 
  The media type supported is defined by the following:
  1. The input and output of this topology needs to be Signed 16-bit. 
  2. The input number of channels supported is Stereo (2), while the output must be Mono.
  3. Sampling rate of this topology is defined by the VPM_CMD_CREATE_SESSION_V2 command.
  4. The reference signal is Signed 16-bit Mono and is of the same sampling rate as the primary.
  */
#define VPM_TX_DUAL_MIC_DYNAMIC_TOPOLOGY (0x00010F84)

/** This is the topology ID for exercising dynamically loadable voice 
  modules in the shared object voiceproc_tx.so. This topology is specific 
  and needs to be used by modules that operate on the uplink device path. 
  The media type supported is defined by the following:
  1. The input and output of this topology needs to be Signed 16-bit. 
  2. The input number of channels supported is Quad (4), while the output must be Mono.
  3. Sampling rate of this topology is defined by the VPM_CMD_CREATE_SESSION_V2 command.
  4. The reference signal is Signed 16-bit Mono and is of the same sampling rate as the primary.
*/
#define VPM_TX_QUAD_MIC_DYNAMIC_TOPOLOGY (0x00010F85)

/** Topology ID for Rx pass through. */
#define VPM_RX_NONE       ( 0x00010F70 )

/** Topology ID for Rx processing with all available algorithms. */
#define VPM_RX_DEFAULT    ( 0x00010F77 )

/** This is the topology ID for exercising dynamically loadable voice 
  modules in the shared object voiceproc_rx.so. This topology is specific 
  and needs to be used by modules that operate on the downlink device path. 
  The media type supported is defined by the following:
  1. The input and output of this topology needs to be Signed 16-bit Mono. 
  2. Sampling rate of this topology is defined by the VPM_CMD_CREATE_SESSION_V2 command. 
*/
#define VPM_RX_DYNAMIC_TOPOLOGY (0x00010F82)

/** Indicates that the far-end reference for echo cancellation is composed by
    mixing audio and voice inside the aDSP.
*/
#define	VPM_TX_INT_MIX_MODE       ( 0x00010F7C )

/** Indicates that the far-end reference for echo cancellation is composed by
    mixing audio and voice outside the aDSP.
*/
#define	VPM_TX_EXT_MIX_MODE       ( 0x00010F7D )

/** @} */ /* end_addtogroup vpm_cmd_create_session */

/* Used by the structure that creates a paired Tx/Rx device leg of
  the voice call. */
typedef struct vpm_create_session_v2_t vpm_create_session_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vpm_create_session_v2_t
@{ */
/* Payload of the VPM_CMD_CREATE_SESSION_V2 command, which creates a
    paired Tx/Rx device leg of a voice call. It is also used by the
    VPM_CMD_REINIT_SESSION_V2 command, which reconfigures an existing device
    session.
*/
struct vpm_create_session_v2_t
{
   uint16_t tx_port;
   /**< AFE port ID for voice Tx. This is the port ID expected by the AFE that
        connects to voice Tx device processing.

        @values Refer to @xhyperref{Q6,[Q6]} */
   /* Q6 = Hex Multimedia ADSP.BF.2.0: Audio API Interface Spec */

   uint16_t tx_sampling_rate;
   /**< Tx sampling rate.

        @values 8000, 16000, 48000 Hz */

   uint32_t tx_topology_id;
   /**< Identifies the predefined algorithms required for the session.

        @values
        - #VPM_TX_NONE
        - #VPM_TX_SM_ECNS
        - #VPM_TX_DM_FLUENCE
        - #VPM_TX_SM_FLUENCEV5
        - #VPM_TX_DM_FLUENCEV5
        - #VPM_TX_TM_FLUENCE 
        - #VPM_TX_QM_FLUENCE_PRO 
        - #VPM_TX_DM_FLUENCEV5_BROADSIDE 
        - #VPM_TX_QM_FLUENCE_PROV2 @tablebulletend */


   uint16_t rx_port;
   /**< AFE port ID for voice Rx. This is the port ID expected by the AFE that
        connects to voice Rx device processing.

        @values Refer to @xhyperref{Q6,[Q6]} */

   uint16_t rx_sampling_rate;
   /**< Rx sampling rate.
   
        @values 8000, 16000, 48000 Hz */

   uint32_t rx_topology_id;
   /**< Identifies the predefined algorithms required for the session.

        @values
        - #VPM_RX_NONE
        - #VPM_RX_DEFAULT @tablebulletend */

   uint32_t ec_mode;
   /**<	Echo Cancellation mode.

        @values
          - #VPM_TX_INT_MIX_MODE
          - #VPM_TX_EXT_MIX_MODE
        
        The far-end reference signal contains both audio and voice in the In
        Call Audio feature.

        Echo cancellation supports both Synchronous and Asynchronous mode. 
        - In Synchronous mode, echo cancellation expects an audio-plus-voice
          mixed signal as the echo reference.
        - In Asynchrounous mode, echo cancellation expects audio and voice as
          different echo references.
        
        Audio and voice can be mixed within the aDSP or outside the aDSP. This
        mode word indicates where the mixing is happening. */

   uint16_t audio_ref_port;
   /**< AFE port ID for an external audio signal. This is the port ID expected
        by the AFE that connects to the voice Tx device processing.

        @values Refer to @xhyperref{Q6,[Q6]}
        
        If this field is set to 0xFFFF, audio and voice are assumed to be
        mixed internally; otherwise, they are mixed externally. */

   uint16_t reserved;
   /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vpm_create_session_v2_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vpm_cmd_reconfig_session
    Reconfigures an existing device session with new parameters if the
    parameters of the existing session change. Use this command if
    #VPM_CMD_CREATE_SESSION_V2 was used to create the session.

  @apr_hdr_fields
    Opcode   -- VPM_CMD_REINIT_SESSION_V2 \n
    Dst_port -- Handle to the session

  @msg_payload
    This command uses the same APR message payload as
    #VPM_CMD_CREATE_SESSION_V2.

  @description
    A voice processing session is a paired Tx/Rx session with postprocessing
    blocks. If the Tx or Rx port is set to NO_INTERFACE, the corresponding
    module is not created. If, in the original handle, the Tx port was set to
    NO_INTERFACE but it is now a valid interface, a corresponding module is
    created.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created, and it must be in the Stop state.
    @par
    Typically, a new set of calibration data is sent after this
    reinitialization and before the session is restarted.
*/
#define VPM_CMD_REINIT_SESSION_V2  ( 0x0001103A )
/*endcond */

/** @ingroup vpm_cmd_set_timing_params_offsets
    Sets the Vocoder Frame Reference (VFR) mode and timing offsets in a
    voice stream session.

  @apr_hdr_fields
    Opcode   -- VPM_CMD_SET_TIMING_PARAMS \n
    Dst_port -- Handle to the session

  @msg_payload{vpm_set_timing_params_t}
    @table{weak__vpm__set__timing__params__t}

  @detdesc
    This command is used to provide timing information for a packet exchange
    to the aDSP services. All circuit-switched calls must use either the
    internal or external #VFR_HARD mode. All packet-switched calls with time
    warping enabled or disabled, and other applications, must use #VFR_NONE
    mode.

  @dependencies
    A valid session must have been created.
    @par
    This command can only be issued in the Stop state.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/
#define VPM_CMD_SET_TIMING_PARAMS ( 0x0001105A )

/* Payload used to provide timing information for a packet exchange
  to the voice services. */
typedef struct vpm_set_timing_params_t vpm_set_timing_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vpm_set_timing_params_t
@{ */
/* Payload of the VPM_CMD_SET_TIMING_PARAMS command, which sets the
    VFR mode and timing offsets in a session.
*/
struct vpm_set_timing_params_t
{
   uint16_t mode;
   /**< Sets the stream VFR mode. The decoder derives timing based on its
        output length.
    
        @values
        - #VFR_NONE -- No periodic tick is used
        - #VFR_HARD -- Internal modem generates a 20 ms periodic tick
        - #VFR_HARD_EXT -- External modem generates a 20 ms periodic tick 
		@tablebulletend */

   uint16_t vptx_start_offset;
   /**< Offset from the VFR for the voice device uplink to synchronize to
        incoming data from the AFE.

        @values 0 to 20000 microseconds with 1-microsecond granularity

        The offset is reverse calculated from the encoder offset, leaving room
		for encoder and voice device uplink processing times. Clock settings
		and system load are factors considered for this calculation. */

   uint16_t vptx_delivery_offset;
   /**< Offset from the VFR for the voice device uplink to deliver processed
        data to the encoder.

        @values 0 to 20000 microseconds with 1-microsecond granularity

        The offset is reverse calculated from the encoder offset, leaving room
		for encoder processing times. Clock settings and system load are
		factors considered for this calculation. */

   uint16_t vprx_delivery_offset;
   /**< Offset from the VFR for the voice device downlink to deliver processed
        data to the AFE. 

        @values 0 to 20000 microseconds with 1-microsecond granularity

        The offset is forward calculated from the decode begin offset,
		considering room for the decoder, voice stream downlink, and voice
		device downlink processing times. Clock settings and system load are
		factors considered for this calculation. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vpm_set_timing_params_t */

/** @ingroup vpm_cmd_get_kpps
    Queries the VPM for required Kilo Packets per Second (KPPS) by aggregating
	the KPPS of all modules independently on the Tx and Rx paths.

  @apr_hdr_fields
    Opcode -- VPM_CMD_GET_KPPS \n
    Dst_port -- Handle to the session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}) .
	@par
    An acknowledgment is sent back in response to this command. 
    In the event of a failure, an acknowledgment is not sent back.

  @dependencies
    A valid session must have been created.
*/
#define VPM_CMD_GET_KPPS ( 0x00011056 )

/** @ingroup vpm_resp_get_kpps_ack
    Response to the #VPM_CMD_GET_KPPS command.

  @apr_hdr_fields
    Opcode   -- VPM_RSP_GET_KPPS_ACK \n
    Dst_port -- Client handle \n
    Src_port -- Handle to the stream session

  @msg_payload{vpm_get_kpps_ack_t}
    @table{weak__vpm__get__kpps__ack__t}

  @return
    None.

  @dependencies
    A valid session must have been created.
*/
#define VPM_RSP_GET_KPPS_ACK ( 0x00011058 )

/* Used by the structure that responds to the Get KPPS command. */
typedef struct vpm_get_kpps_ack_t vpm_get_kpps_ack_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vpm_get_kpps_ack_t
@{ */
/* Payload of the VPM_RSP_GET_KPPS_ACK event, which acknowledges
    the VPM_CMD_GET_KPPS command.
*/
struct vpm_get_kpps_ack_t
{
   uint32_t vptx_kpps;
   /**< Specifies the KPPS consumed by the voice device Tx processing thread.
   
        @values Any uint32 value */

   uint32_t vprx_kpps;
   /**< Specifies the KPPS consumed by the voice device Rx processing thread.
   
        @values Any uint32 value */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vpm_get_kpps_ack_t */

/** @addtogroup common_cmd_map_config_vfr_mode
@{ */
/** Sets the timing mode, VSID and timing offsets in a voice stream session.

    This command is used to provide timing information for a packet exchange
    to the aDSP services. Clients who intend to drive timing must set the mode 
    to VFR_HARD and send appropriate VSID. Clients who doesn't drive the timing
    must set the mode to VFR_NONE where internal timing is used.

  @apr_hdr_fields
    Opcode -- VPM_CMD_SET_TIMING_PARAMS_V2\n
    Dst_port -- handle to the stream session

  @apr_msg_payload
    @structure{vpm_set_timing_params_v2_t}
    @tablespace
    @inputtable{VPM_CMD_SET_TIMING_PARAMS_V2_APR_message_payload.tex}

  @dependencies
    A valid session must have been created.
      @negparspace
    This command is allowed only in STOP state.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/

#define VPM_CMD_SET_TIMING_PARAMS_V2 ( 0x0001105C )

/* Payload used to provide timing information for a packet exchange
  to the voice services. */
typedef struct vpm_set_timing_params_v2_t vpm_set_timing_params_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #VPM_CMD_SET_TIMING_PARAMS_V2 command, which sets the
    timing mode, VSID and timing offsets in a session.
*/
struct vpm_set_timing_params_v2_t
{
   uint16_t mode;
   /**< Sets the timing mode.
    
        Supported values:
        - #VFR_NONE -- internal timing is used and vsid is ignored
        - #VFR_HARD -- timing is driven by the client, refer to vsid further*/
        
   uint32_t vsid;
   /**< Voice System ID has information about which HARD VFR timing should be used by the session.
        bits[21:18] represents VFR ID which is also used by AFE for direct mapping of mux input 
        in LPA_IF HW. Mux input lines are hard-wired to different modems. 
        
        Supported values:
        - Refer to agile document (80-NF711-1)*/

   uint16_t vptx_start_offset;
   /**< Offset from the VFR for Voice Device uplink to sync to incoming data from AFE.

        Supported values: 0 to 20000 microseconds with 1-microsecond
                          granularity

        The offset is reverse calculated from the encoder offset, leaving room for 
        Encoder and Voice device uplink processing times. Clock settings and system 
        load are factors considered for this calculation.*/

   uint16_t vptx_delivery_offset;
   /**< Offset from the VFR for Voice Device uplink to deliver processed data to Encoder.

        Supported values: 0 to 20000 microseconds with 1-microsecond
                          granularity
        The offset is reverse calculated from the encoder offset, leaving room for 
        Encoder processing times. Clock settings and system load are factors considered 
        for this calculation.

        @newpage */

   uint16_t vprx_delivery_offset;
   /**< Offset from the VFR for Voice Device downlink to deliver processed data to AFE. 

        Supported values: 0 to 20000 microseconds with 1-microsecond
                          granularity

        The offset is forward calculated from the decode begin offset, considering room 
        for Decoder, Voice Stream downlink and Voice device downlink processing times. 
        Clock settings and system load are factors considered for this calculation.
        @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup common_cmd_map_config_vfr_mode */

/** @ingroup vpm_cmd_get_delay
    Queries the VPM for the total delay added by aggregating
    the delays of all modules independently on the Tx and Rx paths,
    including delay added by AFE. Valid AFE delay can be provided only
    if the delay is queried when the session is in run state and the
    AFE ports being used are already calibrated and running.

  @apr_hdr_fields
    Opcode -- VPM_CMD_GET_DELAY \n
    Dst_port -- Handle to the session

  @msg_payload
    None.

  @return
    VPM_RSP_GET_DELAY_ACK .
	@par
    An acknowledgment is sent back in response to this command. 
    In the event of a failure, the error is reported back.

  @dependencies
    A valid session must have been created.
*/
#define VPM_CMD_GET_DELAY ( 0x00011062 )

/** @ingroup vpm_resp_get_delay_ack
    Response to the #VPM_CMD_GET_DELAY command.

  @apr_hdr_fields
    Opcode   -- VPM_RSP_GET_DELAY_ACK \n
    Dst_port -- Client handle \n
    Src_port -- Handle to the stream session

  @msg_payload{vpm_get_delay_ack_t}
    @table{weak__vpm__get__delay__ack__t}

  @return
    None.

  @dependencies
    A valid session must have been created.
*/
#define VPM_RSP_GET_DELAY_ACK ( 0x00011063 )

/* Used by the structure that responds to the Get Delay command. */
typedef struct vpm_get_delay_ack_t vpm_get_delay_ack_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vpm_get_delay_ack_t
@{ */
/* Payload of the VPM_RSP_GET_DELAY_ACK event, which acknowledges
    the VPM_CMD_GET_DELAY command.
*/
struct vpm_get_delay_ack_t
{
   uint32_t vptx_delay;
   /**< Specifies the total delay (in microseconds)
        on the tx device path including algorithmic and buffering delay
        added by the voice device Tx processing thread and group delay
        added by the AFE tx port.
   
        @values Any uint32 value */

   uint32_t vprx_delay;
   /**< Specifies the total delay (in microseconds)
        on the rx device path including algortithmic and buffering delay
        added by the voice device Rx processing thread and group delay
        added by the AFE rx port.
   
        @values Any uint32 value */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vpm_get_delay_ack_t */


#endif /* __ADSP_VPM_API_H__ */

