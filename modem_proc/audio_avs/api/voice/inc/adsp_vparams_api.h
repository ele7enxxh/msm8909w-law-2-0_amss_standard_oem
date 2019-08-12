#ifndef __ADSP_VPARAMS_API_H__
#define __ADSP_VPARAMS_API_H__

/**
  @file adsp_vparams_api.h
  @brief This file contains module and parameter IDs for calibration purposes.
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
===========================================================================*/


/** @ingroup cal_param_ids
    Parameter used for enabling/disabling a module.

    @inputtable{Calibration_ID_VOICE_PARAM_MOD_ENABLE.tex}
*/
#define VOICE_PARAM_MOD_ENABLE             (0x00010E00)

/** @ingroup group cal_module_ids
    Identifies the Limiter algorithm in the voice stream
    on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_TX_STREAM_LIMITER
*/
#define VOICE_MODULE_TX_STREAM_LIMITER (0x00010F15)

/** @ingroup cal_param_ids
    Limiter used to control the dynamic range of signals.
    The maximum array size for this parameter is 16 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_TX_STREAM_LIMITER.tex}
*/
#define VOICE_PARAM_TX_STREAM_LIMITER (0x00010E33)

/** @ingroup group cal_module_ids
    Identifies the Comfort Noise Generation algorithm in the voice stream
    on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
*/
#define VOICE_MODULE_ENC_CNG                (0x00010F13)

/** @addtogroup cal_param_ids
@{ */
/** Single-mic echo canceller/noise suppressor. The maximum array size for
    this parameter is 92 bytes.

    @note1hang Single mic echo canceller/noise suppressor now also supports a
               versioned command, #VOICE_PARAM_SMECNS_EXT.

    @inputtable{Calibration_ID_VOICE_PARAM_SMECNS.tex} @newpage
*/
#define VOICE_PARAM_SMECNS                 (0x00010E01)

/** Contains preset coefficients for the single-mic echo canceller/noise
    suppressor. The maximum array size for this parameter is 1604 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_SMECNS_PRESET.tex}
*/
#define VOICE_PARAM_SMECNS_PRESET          (0x00010E02)

/** Contains VP3 data for all voice processing modules. The
    #VOICE_PARAM_VP3_SIZE parameter provides the size, which can be different
    for each software device ID (each device ID can correspond to a different
    topology ID for voice Tx processing).

    This parameter must be saved before destruction or reinitialization of the
    voice processing session, and it must be restored before the voice
    processing session is started.

    @inputtable{Calibration_ID_VOICE_PARAM_VP3.tex}
*/
#define VOICE_PARAM_VP3                    (0x00010E03)

/** Wide voice algorithm. This parameter has a fixed size of 44 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_WV.tex}
*/
#define VOICE_PARAM_WV                     (0x00010E04)

/** Wide voice V2 algorithm. This parameter has a fixed size of 64 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_WV_V2.tex}
*/
#define VOICE_PARAM_WV_V2                  (0x00010E42)


/** Slow talk voice activity detection and expansion algorithm. This
    parameter has a fixed size of 60 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_ST.tex}
*/
#define VOICE_PARAM_ST                     (0x00010E05)

/** Slow talk expansion algorithm. This parameter has a fixed size of 4 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_ST_EXP.tex} @newpage
*/
#define VOICE_PARAM_ST_EXP                 (0x00010E16)

/** AVC/RVE algorithm. This parameter has a fixed size of 148 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_AVC_RVE.tex} @newpage
*/
#define VOICE_PARAM_AVC_RVE                (0x00010E06)

/** AVC/RVE algorithm. This parameter has a fixed size of 164 bytes for version 0.
   @inputtable{Calibration_ID_VOICE_PARAM_AVC_RVE_V2.tex} @newpage
*/
#define VOICE_PARAM_AVC_RVE_V2                (0x00010E41)

/** Used for enabling the AVC/RVE algorithms. This parameter has a fixed
    size of 4 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_AVC_RVE_ENABLE.tex}
*/
#define VOICE_PARAM_AVC_RVE_ENABLE         (0x00010E15)

/** PBE algorithm. This parameter has a fixed size of 56 bytes followed by a
    variable size IIR filter structure.

    The filter sizes are defined by configurable parameters. The maximum size
    for voice applications is 236 bytes. The filter order is assumed to be 3
    for high, low, and band-pass filters.

    @inputtable{Calibration_ID_VOICE_PARAM_PBE.tex}
*/
#define VOICE_PARAM_PBE                    (0x00010E07)

/** Multiband Adaptive Dynamic Range Control (MADRC) algorithm. This parameter
    has a fixed size of 12 bytes, followed by variable size DRC and FIR filter
    structures.

    The filter sizes are defined by configurable parameters. The maximum size
    for voice applications is 472 bytes. For voice, the maximum number of bands
    is assumed to be 3.

    @inputtable{Calibration_ID_VOICE_PARAM_MBDRC.tex}
*/
#define VOICE_PARAM_MBDRC                  (0x00010E08)

/** IIR tuning filter algorithm for voice. This parameter has a fixed size of
    2 bytes followed by variable-sized IIR filter structures.

    The filter sizes are defined by configurable parameters. The maximum size
    for voice applications is 224 bytes. For voice, the maximum number of
    filter stages is assumed to be 10. If the number of stages is an odd
    number, two bytes of padding must be added at the end of the payload.

    @inputtable{Calibration_ID_VOICE_PARAM_IIR.tex}
*/
#define VOICE_PARAM_IIR                    (0x00010E09)

/** FIR tuning filter algorithm for voice. This parameter contains a field that
    indicates the number of filter taps, followed by an FIR filter coefficient
    structure. The supported Q-factor is Q14.

    The maximum number of FIR taps for voice applications is 128 taps. The
    maximum size is 260 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_FIR.tex}
*/
#define VOICE_PARAM_FIR                    (0x00010E0A)

/** FNS algorithm. This parameter has a fixed size of 76 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_FNS.tex}
*/
#define VOICE_PARAM_FNS                    (0x00010E0B)

/** FNS algorithm. The maximum array size for this parameter is:

     - Version 0 -- 84 bytes

    @inputtable{Calibration_ID_VOICE_PARAM_FNS_V2.tex}
*/
#define VOICE_PARAM_FNS_V2                    (0x00010E43)

/** Identifies the time warp algorithm that is part of voice postprocessing on
    the Rx path.
    This module supports the following parameter ID:
    - #VOICE_PARAM_MOD_ENABLE
*/
#define VOICE_MODULE_TIMEWARP        (0x00010F0B)

/** DTMF detection algorithm. This parameter has a fixed size of 36 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_DTMF_DETECTION.tex}
*/
#define VOICE_PARAM_DTMF_DETECTION         (0x00010E0C)

/** Echo canceller Rx algorithm. This parameter has a fixed size of 20 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_ECRX.tex}
*/
#define VOICE_PARAM_ECRX                   (0x00010E0D)

/** Dynamic Range Control (DRC) algorithm. This parameter has a fixed size of
    56 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_DRC.tex}
*/
#define VOICE_PARAM_DRC                    (0x00010E0E)

/** Dual-mic echo canceller/noise suppressor. The maximum array size for this
    parameter is 448 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_DMECNS.tex}
*/
#define VOICE_PARAM_DMECNS                 (0x00010E11)

/** Fluence dual-mic and tri-mic echo canceller/noise suppressor. The maximum
    array size for this parameter is 468 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_FLECNS.tex}
*/
#define VOICE_PARAM_FLECNS                 (0x00010E20)

/** Parameter structure for the Fluence dual-mic and tri-mic echo
    canceller/noise suppressor with versioning. The maximum array size in bytes
    for this parameter is 92 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_FLECNS_EXT.tex}
*/
#define VOICE_PARAM_FLECNS_EXT               (0x00010E1F)

/** Fluence Pro (quad-mic) and Fluence V5 (single and dual mic) echo
    canceller/noise suppressor. The maximum array size for this parameter is:

     - Version 0 -- 904 bytes
     - Version 1 -- 908 bytes
     - Version 2 -- 920 bytes
     - Version 3 -- 932 bytes
     - Version 4 -- 976 bytes 

    @inputtable{Calibration_ID_VOICE_PARAM_FPECNS.tex}

    For version 1, the parameter is extended as follows.

      @inputtable{Calibration_ID_VOICE_PARAM_FPECNS_V1_ext.tex}

    For version 2, the parameter is extended as follows.

      @inputtable{Calibration_ID_VOICE_PARAM_FPECNS_V2_ext.tex}

    For version 3, the parameter is extended as follows.

      @inputtable{Calibration_ID_VOICE_PARAM_FPECNS_V3_ext.tex}

    For version 4, the parameter is extended as follows.
  
      @inputtable{Calibration_ID_VOICE_PARAM_FPECNS_V4_ext.tex} @newpage
*/
#define VOICE_PARAM_FPECNS                 (0x00010E1C)

/** Fluence V5 echo canceller/noise suppressor. The maximum array size for
    this parameter is:

    - Version 0 -- 904 bytes
    - Version 1 -- 908 bytes
    - Version 2 -- 912 bytes
    - Version 3 -- 1108 bytes
    - Version 4 -- 5380 bytes

    @inputtable{Calibration_ID_VOICE_PARAM_FV5ECNS.tex}

    For version 1, the parameter is extended as follows.

      @inputtable{Calibration_ID_VOICE_PARAM_FV5ECNS_V1_ext.tex}

    @keep{8} For version 2, the parameter is extended as follows.

      @inputtable{Calibration_ID_VOICE_PARAM_FV5ECNS_V2_ext.tex}

    For version 3, the parameter is extended as follows:

      @inputtable{Calibration_ID_VOICE_PARAM_FV5ECNS_V3_ext.tex}

    For version 4, the parameter is extended as follows:

      @inputtable{Calibration_ID_VOICE_PARAM_FV5ECNS_V4_ext.tex}
*/
#define VOICE_PARAM_FV5ECNS                (0x00010E26)

/** Fluence  V5 Broadside and Pro V2 echo canceller/noise suppressor. The maximum array size for
    this parameter is:

    - Version 0 -- 716 bytes

    @inputtable{Calibration_ID_VOICE_PARAM_FLUENCE_EC.tex}

*/
#define VOICE_PARAM_FLUENCE_EC                (0x00010E35)

/** Fluence V5 Broad side and Pro V2 echo canceller/noise suppressor. The maximum array size for
    this parameter is:

    - Version 0 -- 1064 bytes


    @inputtable{Calibration_ID_VOICE_PARAM_FLUENCE_NS_SPK.tex}
*/
#define VOICE_PARAM_FLUENCE_NS_SPK            (0x00010E36)

/** Single-mic echo canceller/noise suppressor with versioning. The maximum
    array size for this parameter is:

    - Version 0 -- 96 bytes
    - Version 1 -- 112 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_SMECNS_EXT.tex}

    For version 1, the parameter is extended as follows.

      @inputtable{Calibration_ID_VOICE_PARAM_SMECNS_EXT_V1_ext.tex}
*/
#define VOICE_PARAM_SMECNS_EXT			   (0x00010E27)

/** Read-only parameter structure for the Fluence dual-mic and Fluence ANC
    (tri-mic) echo canceller/noise suppressor. The maximum array size for this
    parameter is 240 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_FL_STATE.tex}
*/
#define VOICE_PARAM_FL_STATE               (0x00010E1D)

/** Read-only parameter structure for the Fluence Pro/V5 single-mic, dual-mic,
   and quad-mic echo canceller/noise suppressor's internal status monitoring.
    The maximum array size for this parameter is 860 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_FP_STATE.tex}
*/
#define VOICE_PARAM_FP_STATE               (0x00010E30)

/** Read-only parameter structure for the Fluence V5 single-mic, dual-mic echo
    canceller/noise suppressor's internal status monitoring.
    The maximum array size for this parameter is
    - Version 0 -- 116 bytes
    - Version 1 -- 180 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_FV5_STATE.tex}

    For version 1, the parameter is extended as follows.

    @inputtable{Calibration_ID_VOICE_PARAM_FV5_STATE_V1_ext.tex}
*/
#define VOICE_PARAM_FV5_STATE              (0x00010E31)

/** Read-only parameter structure for the Fluence V5 SPF coeff information
    (single-mic and dual-mic echo canceller/noise suppressor's internal status
    monitoring). The maximum array size for Version 0 of this parameter is 4100
    bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_FV5_SPF_COEFF.tex} @newpage
*/
#define VOICE_PARAM_FV5_SPF_COEFF              (0x00010E2D)

/** Read-only parameter structure for the Fluence V5 dual-mic Broadside, quad-mic
    FluencePro V2 speaker phone mode noise suppressor's internal status monitoring.
    The maximum array size for this parameter is:
    - Version 0 -- 980 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_FLUENCE_NS_SPK_STATE.tex}
*/
#define VOICE_PARAM_FLUENCE_NS_SPK_STATE    (0x00010E40)

/** Fixed echo path delay in the firmware. The delay is in multiples of 8 kHz
    samples. The maximum array size for this parameter is 4 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_ECHO_PATH_DELAY.tex}
*/
#define VOICE_PARAM_ECHO_PATH_DELAY        (0x00010E1E)

/** Volume control algorithm on the Tx and Rx paths. This parameter has a
    fixed size of 4 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_VOL.tex}
*/
#define VOICE_PARAM_VOL                    (0x00010E12)

/** @cond OEM_only */
/** ID of the Gain parameter. This parameter has a fixed size of 4 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_GAIN.tex} @newpage
*/
#define VOICE_PARAM_GAIN                   (0x00010E13)

/** ID of the Adaptive Input Gain (AIG) algorithm parameter on the Tx and Rx
    paths. This parameter has a fixed size of 36 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_AIG.tex}
*/
#define VOICE_PARAM_AIG                    (0x00010E17)

/** ID of the read-only parameter used to return the Adaptive Filter (AF)
    coefficients of the currently configured echo canceller.

    @inputtable{Calibration_ID_VOICE_PARAM_AF_COEFFS.tex}

    AFCoeffData is defined in the following topology AF data structure tables
    and is based on the voice processing topology. The structure length is
    variable based on the number of taps used by the module at the time of the
    query.

  @keep{8} @subhead{AF data structure for the VPM\_TX\_SM\_ECNS topology}
    @inputtable{Calibration_ID_VOICE_PARAM_AF_COEFFS_topo_VPM_TX_SM_ECNS.tex}

  @keep{8} @subhead{AF data structure for the VPM\_TX\_DM\_FLUENCE topology}
    @inputtable{Calibration_ID_VOICE_PARAM_AF_COEFFS_topo_VPM_TX_DM_FLUENCE.tex}

  @keep{8} @subhead{AF data structure for the VPM\_TX\_TM\_FLUENCE topology}
    @inputtable{Calibration_ID_VOICE_PARAM_AF_COEFFS_topo_VPM_TX_TM_FLUENCE.tex}

  @keep{8} @subhead{AF data structure for the VPM\_TX\_QM\_FLUENCE\_PRO\,
    VPM\_TX\_SM\_FLUENCEV5\, and VPM\_TX\_DM\_FLUENCEV5 topologies}
    @inputtable{Calibration_ID_VOICE_PARAM_AF_COEFFS_topo_VPM_TX_QM_FLUENCE_pro.tex}

  @keep{8} @subhead{AF data structure for the VPM\_TX\_QM\_FLUENCE\_PROV2\  topology}
    @inputtable{Calibration_ID_VOICE_PARAM_AF_COEFFS_topo_VPM_TX_QM_FLUENCE_PROV2.tex}

  @keep{8} @subhead{AF data structure for the VPM\_TX\_DM\_FLUENCEV5\_BROADSIDE  topology}
    @inputtable{Calibration_ID_VOICE_PARAM_AF_COEFFS_topo_VPM_TX_DM_FLUENCEV5_BROADSIDE.tex}

    For the VPM_TX_QM_FLUENCE_PRO, VPM_TX_DM_FLUENCEV5, VPM_TX_SM_FLUENCEV5,
    VPM_TX_QM_FLUENCE_PROV2 and VPM_TX_DM_FLUENCEV5_BROADSIDE
    topologies, each AecPathStruct is defined by the following variable length
    payload. AFLen need not be the same across all AecPaths.

    The total number of taps over all filters for both Fg and Bg arrays must
    not exceed 2000. Fg and Bg filters for a particular filter index i use the
    same length AFLen[i]. This means the sum of AFLen[i] over i from 0 to
    (NumAfFilter-1) must not exceed (2000 / 2) = 1000. The maximum size is
    4024&nbsp;bytes.

    BASE represents the byte offset of the current AecPath i, which has filter
    length AFLen[i]. The next AecPathStruct for index (i+1) starts at offset
    BASE+4+4*AFLen[i].

  @keep{8} @subhead{AecPathStruct variable length payload}
    @inputtable{Calibration_ID_VOICE_PARAM_AF_COEFFS_topo_VPM_TX_QM_FLUENCE_PRO_payload.tex}

  @keep{8} @subhead{Maximum size of VOICE\_PARAM\_AF\_COEFFS}
    The memory allocated to get the AF coefficents must be large enough to
    accommodate the maximum size of the VOICE_PARAM_AF_COEFFS structure. The
    maximum size depends on the maximum number of AF taps allowed for the
    current topology configuration.

    The following table lists the maximum size according to topology.
    @inputtable{Calibration_ID_VOICE_PARAM_AF_COEFFS_maximum_size.tex}
*/
#define VOICE_PARAM_AF_COEFFS              (0x00010E19)

/** Read-only parameter used to read the size of VP3 data. This parameter is a
    multiple of 4 bytes, which is defined by the voice processor Tx to maintain
    parameter alignment.

    @inputtable{Calibration_ID_VOICE_PARAM_VP3_SIZE.tex}
*/
#define VOICE_PARAM_VP3_SIZE               (0x00010E1A)

/** @endcond */

/** Soft-stepping volume control algorithm on Tx and Rx paths. This parameter
    is used to change the volume linearly over ramp duration. This is a
    read-only parameter with a total size of 8 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_SOFT_VOL.tex}
*/
#define VOICE_PARAM_SOFT_VOL               (0x00010E21)

/** Voice Plus Dual-mic echo canceller/noise suppressor. The maximum array size
    for this parameter is 80 bytes.

    @inputtable{Calibration_ID_VOICE_PARAM_VPECNS.tex}
*/
#define VOICE_PARAM_VPECNS                 (0x00010E34)


/** ID of Source tracking parameters.
    The parameters listed in this structure correspond to how the user
    configures a Sound focus application. This is a read/write parameter
    (SET_PARAM and GET_PARAM are supported). This parameter ID applies
    to VOICE_MODULE_FLUENCE_PROV2 and VOICE_MODULE_FLUENCEV5_BROADSIDE
    modules only.

    Given that there is more than one topology supporting this parameter
    and that client may not have knowledge of the ECNS module ID, this
    parameter shall be supported by the generic module ID VOICEPROC_MODULE_TX.
    VPM shall ensure that the payload is forwarded to the ECNS module in
    the chain.

    Upon query (GET_PARAM), VPM shall ensure that the fields are
    up-to-date with what Fluence is using.

    The maximum array size for this parameter is: 28 bytes

    @inputtable{Cal_ID_0x00010E37.tex}
*/
#define VOICE_PARAM_FLUENCE_SOUNDFOCUS (0x00010E37)


/** @cond OEM_only */
/**
    Based on the sector configuration set by the user, this structure gets
    populated and is made available by Fluence once every 20ms. This is a
    read-only parameter (only GET_PARAM is supported). This parameter ID
    applies only to VOICE_MODULE_FLUENCE_PROV2 and VOICE_MODULE_FLUENCEV5_BROADSIDE
    modules only.

    Given that there is more than one topology supporting this parameter and
    that client may not have knowledge of the ECNS module ID, this parameter
    shall be supported by the generic module ID VOICEPROC_MODULE_TX (0x00010EF6).
    VPM shall ensure that the payload is forwarded to the ECNS module in the chain.

    Maximum APR packet size allowed varies across chipsets based on the overall
    memory available. Such considerations need to be made by the client to choose
    from Out-of-band or In-band form of transfer for this parameter.

    The maximum array size for this parameter is: 376 bytes

   @inputtable{Cal_ID_0x00010E38.tex}
*/
#define VOICE_PARAM_FLUENCE_SOURCETRACKING (0x00010E38)

/** @} */ /* end_addtogroup cal_param_ids */

/** @addtogroup cal_module_ids
@{ */
/** @cond OEM_only */
/** Identifies the single-mic echo canceller in the voice processor on the Tx
    path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_SMECNS (supported only for backward compatibility)
    - #VOICE_PARAM_SMECNS_PRESET
    - #VOICE_PARAM_AF_COEFFS
    - #VOICE_PARAM_FNS
    - #VOICE_PARAM_SMECNS_EXT
*/
#define VOICE_MODULE_SMECNS                (0x00010EE0)

/** Identifies the dual-mic echo canceller in the voice processor on the Tx
    path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_DMECNS (supported only for backward compatability)
    - #VOICE_PARAM_FLECNS
    - #VOICE_PARAM_AF_COEFFS
    - #VOICE_PARAM_FL_STATE
    - #VOICE_PARAM_FLECNS_EXT
*/
#define VOICE_MODULE_DMECNS                (0x00010EE1)

/** Identifies the tri-mic Fluence echo canceller in the voice processor on
    the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_FLECNS
    - #VOICE_PARAM_AF_COEFFS
*/
#define VOICE_MODULE_TMECNS                (0x00010F02)

/** Identifies the Fluence Pro quad-mic-based echo canceller/noise suppressor
    in the voice processor on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_FPECNS
    - #VOICE_PARAM_AF_COEFFS
    - #VOICE_PARAM_ECHO_PATH_DELAY
*/
#define VOICE_MODULE_FPECNS_QM             (0x00010F08)


/** Identifies the Fluence Pro V2 quad-mic-based echo canceller/noise suppressor
    in the voice processor on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_AF_COEFFS
    - #VOICE_PARAM_ECHO_PATH_DELAY
    - #VOICE_PARAM_FLUENCE_EC
    - #VOICE_PARAM_FLUENCE_NS_SPK
*/
#define VOICE_MODULE_FLUENCE_PROV2 (0x00010F17)

/** Identifies the dual-mic based Fluence V5 echo canceller
    and noise suppressor in the voice processor Tx.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_FPECNS
    - #VOICE_PARAM_AF_COEFFS
    - #VOICE_PARAM_ECHO_PATH_DELAY
    - #VOICE_PARAM_FV5ECNS
*/
#define VOICE_MODULE_FV5ECNS_DM            (0x00010F0A)

/** Identifies the dual-mic based Fluence V5 broadside echo canceller
    and noise suppressor in the voice processor Tx.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_AF_COEFFS
    - #VOICE_PARAM_ECHO_PATH_DELAY
    - #VOICE_PARAM_FLUENCE_EC
    - #VOICE_PARAM_FLUENCE_NS_SPK
*/
#define VOICE_MODULE_FLUENCEV5_BROADSIDE (0x00010F18)

/** Identifies the single-mic based Fluence V5 echo canceller
    and noise suppressor in the voice processor Tx.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_FPECNS
    - #VOICE_PARAM_AF_COEFFS
    - #VOICE_PARAM_ECHO_PATH_DELAY
    - #VOICE_PARAM_FV5ECNS @newpage
*/
#define VOICE_MODULE_FV5ECNS_SM            (0x00010F09)

/** Identifies the wide voice algorithm that is part of voice decoder
    postprocessing.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_WV
*/
#define VOICE_MODULE_WV                    (0x00010EE2)

/** Identifies the wide voice V2 algorithm that is part of voice decoder
    postprocessing.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_WV_V2
*/
#define VOICE_MODULE_WV_V2                 (0x00010F1A)

/** Identifies the slow talk algorithm that is part of voice decoder
    postprocessing.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_ST
    - #VOICE_PARAM_ST_EXP
*/
#define VOICE_MODULE_ST                    (0x00010EE3)

/** Identifies the AVC/RVE algorithm that is part of voice preprocessing on
    the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_AVC_RVE_ENABLE
    - #VOICE_PARAM_AVC_RVE
*/
#define VOICE_MODULE_TX_AVC                (0x00010EE4)

/** Identifies the PBE algorithm in the voice processor on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_PBE
*/
#define VOICE_MODULE_PBE                   (0x00010EE5)

/** Identifies the Multiband Dynamic Range Control (MDRC) algorithm in the
    voice processor on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_MBDRC
*/
#define VOICE_MODULE_MBDRC                 (0x00010EE6)

/** Identifies the IIR algorithm in the voice processor on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_IIR @newpage
*/
#define VOICE_MODULE_TX_IIR                (0x00010EE7)

/** Identifies the IIR algorithm in the voice processor on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_IIR
*/
#define VOICE_MODULE_RX_IIR                (0x00010EE8)

/** Identifies the FIR algorithm in the voice processor on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_FIR
 */
#define VOICE_MODULE_TX_FIR                (0x00010EE9)

/** Identifies the FIR algorithm in the voice processor on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_FIR
*/
#define VOICE_MODULE_RX_FIR                (0x00010EEA)

/** Identifies the FNS algorithm that is part of voice postprocessing on the
    Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_FNS
*/
#define VOICE_MODULE_FNS                   (0x00010EEB)

/** Identifies the DTMF detection algorithm that is part of voice
    postprocessing on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_DTMF_DETECTION
*/
#define VOICE_MODULE_DTMF_DETECTION        (0x00010EEC)

/** Identifies the DTMF detection algorithm that is part of voice
    postprocessing on the  Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_DTMF_DETECTION @newpage
*/
#define VOICE_MODULE_TX_DTMF_DETECTION     (0x00010F01)

/** Identifies the echo canceller Rx algorithm that is part of voice
    postprocessing on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_ECRX
*/
#define VOICE_MODULE_RX_EC                 (0x00010EED)

/** Identifies the IIR algorithm in the voice processor on the Tx mic1 input
    path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_IIR
*/
#define VOICE_MODULE_IIR_MIC1              (0x00010EF0)

/** Identifies the IIR algorithm in the voice processor on the Tx mic2 input
    path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_IIR
*/
#define VOICE_MODULE_IIR_MIC2              (0x00010EF1)

/** Identifies the IIR algorithm in the voice processor on the Tx mic3 input
    path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_IIR
*/
#define VOICE_MODULE_IIR_MIC3              (0x00010F03)

/** Identifies the IIR algorithm in the voice processor on the Tx mic4 input
    path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_IIR
*/
#define VOICE_MODULE_IIR_MIC4              (0x00010F04)

/** Identifies the Dynamic Range Control algorithm in the voice preprocessor on
    the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_DRC @newpage
*/
#define VOICE_MODULE_RX_DRC                (0x00010EF2)

/** Identifies the Dynamic Range Control algorithm in the voice preprocessor on
    the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_DRC
*/
#define VOICE_MODULE_TX_DRC                (0x00010EF3)

/** @endcond */

/** Identifies the volume control algorithm in the voice preprocessor on the
    Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_VOL
    - #VOICE_PARAM_SOFT_VOL
*/
#define VOICE_MODULE_TX_VOL                (0x00010EF4)

/** Identifies the volume control algorithm in the voice preprocessor on the
    Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_VOL
    - #VOICE_PARAM_SOFT_VOL
*/
#define VOICE_MODULE_RX_VOL                (0x00010EF5)

/** Identifies supported generic parameters that are not tied to a specific
    algorithm module in the voice preprocessor on the Tx path.
    This module ID is also used as the default identifier for the host PCM.

  @if OEM_only
    This module supports the following parameter IDs:

    - #VOICE_PARAM_VP3
    - #VOICE_PARAM_VP3_SIZE (read only)
  @endif
*/
#define VOICEPROC_MODULE_TX          (0x00010EF6)

/** Identifies supported generic parameters that are not tied to a specific
    algorithm module in the voice preprocessor on the Rx path.
    This module ID is also used as the default identifier for the host PCM.
*/
#define VOICEPROC_MODULE_RX          (0x00010F06)

/** Identifies supported generic parameters that are not tied to a specific
    algorithm module on the voice Tx stream.
*/
#define VOICESTREAM_MODULE_TX              (0x00010F07)

/** Identifies supported generic parameters that are not tied to a particular
    algorithm module in the voice rx stream.
*/
#define VOICESTREAM_MODULE_RX              (0x00010F14)

/** Identifies the voice mic gain in the voice preprocessor on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
  @if OEM_only
    - #VOICE_PARAM_GAIN
  @endif
*/
#define VOICE_MODULE_TX_MIC_GAIN           (0x00010EF7)

/** Identifies the voice encoder gain in the voice preprocessor on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
  @if OEM_only
    - #VOICE_PARAM_GAIN
  @endif
*/
#define VOICE_MODULE_TX_ENC_GAIN           (0x00010EF8)

/** Identifies the voice speaker gain in the voice preprocessor on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
  @if OEM_only
    - #VOICE_PARAM_GAIN
  @endif
*/
#define VOICE_MODULE_RX_SPKR_GAIN          (0x00010EF9)

/** Identifies the voice decoder gain in the voice preprocessor on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
  @if OEM_only
    - #VOICE_PARAM_GAIN
  @endif
*/
#define VOICE_MODULE_RX_DEC_GAIN           (0x00010EFA)

/** @cond OEM_only */
/** Identifies the HPF 12 IIR algorithm in the voice processor on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_IIR
*/
#define VOICE_MODULE_TX_HPF                (0x00010F11)

/** Identifies the fixed elliptical filter in the voice preprocessor on the Tx
    path.
    This module supports the following parameter ID:

    - #VOICE_PARAM_MOD_ENABLE
*/
#define VOICE_MODULE_TX_ELLIPTICAL_FILTER  (0x00010EFD)

/** Identifies the fixed slope filterin the voice preprocessor on the Tx path.
    This module supports the following parameter ID:

    - #VOICE_PARAM_MOD_ENABLE
*/
#define VOICE_MODULE_TX_SLOPE_FILTER       (0x00010EFE)

/** Identifies the HPF 12 IIR algorithm in the voice processor on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_IIR
*/
#define VOICE_MODULE_RX_HPF                (0x00010F12)

/** Identifies the automatic gain control algorithm in the voice preprocessor
    on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_AIG
*/
#define VOICE_MODULE_RX_AIG                (0x00010EFF)

/** Identifies the automatic gain control algorithm in the voice preprocessor
    on the Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_AIG
*/
#define VOICE_MODULE_TX_AIG                (0x00010F00)

/** Identifies the dual-mic Voice Plus echo canceller in the voice processor on
    he Tx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
    - #VOICE_PARAM_VPECNS
*/
#define VOICE_MODULE_VPECNS                (0x00010F16)

/** Identifies the Blind bandwidth extension for eAMR/AMR vocoders on the Rx path.
    This module supports the following parameter IDs:

    - #VOICE_PARAM_MOD_ENABLE
*/
#define VOICE_MODULE_BEAMR                    (0x00010F19)

/** @endcond */

/** @} */ /* end_addtogroup cal_module_ids */

#endif /* __ADSP_VPARAMS_API_H__ */
