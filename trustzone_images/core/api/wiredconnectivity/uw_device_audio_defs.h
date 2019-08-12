/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file uw_device_audio_defs.h

Contains definitions for the USBWare interface for Device Audio class. 

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_device_audio_defs.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/26/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/

#ifndef _UW_DEVICE_AUDIO_DEFS_H_
#define _UW_DEVICE_AUDIO_DEFS_H_

#include <jtypes.h>


/** @ingroup hsusb_jungo_api 
@{ */

/** Audio function driver context type.
*/
typedef void *uwd_audio_drv_h;

/** Application context type.
*/
typedef void *uwd_audio_app_h;

/** Audio entity types.
*/
typedef enum {
    UWD_AUDIO_ET_INPUT_TERMINAL  = 0x02,  /**< &nbsp; */
    UWD_AUDIO_ET_OUTPUT_TERMINAL = 0x03,  /**< &nbsp; */
    UWD_AUDIO_ET_MIXER_UNIT      = 0x04,  /**< &nbsp; */
    UWD_AUDIO_ET_SELECTOR_UNIT   = 0x05,  /**< &nbsp; */
    UWD_AUDIO_ET_FEATURE_UNIT    = 0x06,  /**< &nbsp; */
    UWD_AUDIO_ET_PROCESSING_UNIT = 0x07,  /**< &nbsp; */
    UWD_AUDIO_ET_EXTENSION_UNIT  = 0x08   /**< &nbsp; */
} uwd_audio_entity_type_t;

/** Audio terminal types (USB, Input, and Output).
*/
typedef enum {
/* USB Terminal Types */
   UWD_AUDIO_TT_USB_UNDEFINED                 = 0x0100, /**< &nbsp; */
   UWD_AUDIO_TT_USB_STREAMING                 = 0x0101, /**< &nbsp; */
   UWD_AUDIO_TT_USB_VENDOR_SPECIFIC           = 0x01FF, /**< &nbsp; */

/* Input Terminal Types */
   UWD_AUDIO_TT_INPUT_UNDEFINED               = 0x0200, /**< &nbsp; */
   UWD_AUDIO_TT_MICROPHONE                    = 0x0201, /**< &nbsp; */
   UWD_AUDIO_TT_DESKTOP_MICROPHONE            = 0x0202, /**< &nbsp; */
   UWD_AUDIO_TT_PERSONAL_MICROPHONE           = 0x0203, /**< &nbsp; */
   UWD_AUDIO_TT_OMNI_DIRECTIONAL_MICROPHONE   = 0x0204, /**< &nbsp; */
   UWD_AUDIO_TT_MICROPHONE_ARRAY              = 0x0205, /**< &nbsp; */
   UWD_AUDIO_TT_PROCESSING_MICROPHONE_ARRAY   = 0x0206, /**< &nbsp; */

/* Output Terminal Types */
   UWD_AUDIO_TT_OUTPUT_UNDEFINED              = 0x0300, /**< &nbsp; */
   UWD_AUDIO_TT_SPEAKER                       = 0x0301, /**< &nbsp; */
   UWD_AUDIO_TT_HEADPHONES                    = 0x0302, /**< &nbsp; */
   UWD_AUDIO_TT_HEAD_MOUNTED_DISPLAY_AUDIO    = 0x0303, /**< &nbsp; */
   UWD_AUDIO_TT_DESKTOP_SPEAKER               = 0x0304, /**< &nbsp; */
   UWD_AUDIO_TT_ROOM_SPEAKER                  = 0x0305, /**< &nbsp; */
   UWD_AUDIO_TT_COMMUNICATION_SPEAKER         = 0x0306, /**< &nbsp; */
   UWD_AUDIO_TT_LOW_FREQUENCY_EFFECTS_SPEAKER = 0x0307  /**< &nbsp; */
} uwd_audio_trm_type_t;

/** Audio class-specific request codes.
*/
typedef enum {
    UWD_AUDIO_RC_SET_CUR  = 0x01,   /**< &nbsp; */
    UWD_AUDIO_RC_GET_CUR  = 0x81,   /**< &nbsp; */
    UWD_AUDIO_RC_SET_MIN  = 0x02,   /**< &nbsp; */
    UWD_AUDIO_RC_GET_MIN  = 0x82,   /**< &nbsp; */
    UWD_AUDIO_RC_SET_MAX  = 0x03,   /**< &nbsp; */
    UWD_AUDIO_RC_GET_MAX  = 0x83,   /**< &nbsp; */
    UWD_AUDIO_RC_SET_RES  = 0x04,   /**< &nbsp; */
    UWD_AUDIO_RC_GET_RES  = 0x84,   /**< &nbsp; */
    UWD_AUDIO_RC_SET_MEM  = 0x05,   /**< &nbsp; */
    UWD_AUDIO_RC_GET_MEM  = 0x85,   /**< &nbsp; */
    UWD_AUDIO_RC_GET_STAT = 0xFF    /**< &nbsp; */
} uwd_audio_req_code_t;

/** Endpoint control selectors.
*/
typedef enum {
    UWD_AUDIO_EP_SAMPLING_FREQ_CONTROL = 0x01,   /**< &nbsp; */
    UWD_AUDIO_EP_PITCH_CONTROL         = 0x02    /**< &nbsp; */
} uwd_audio_endpoint_cs_t;

/** Terminal control selectors.
*/
typedef enum {
    UWD_AUDIO_TR_CONTROL_UNDEFINED     = 0x00,   /**< &nbsp; */
    UWD_AUDIO_TR_COPY_PROTECT_CONTROL  = 0x01    /**< &nbsp; */
} uwd_audio_terminal_cs_t;

/** Feature Unit control selectors.
*/
typedef enum {
    UWD_AUDIO_FU_CONTROL_UNDEFINED         = 0x00,   /**< &nbsp; */
    UWD_AUDIO_FU_MUTE_CONTROL              = 0x01,   /**< &nbsp; */
    UWD_AUDIO_FU_VOLUME_CONTROL            = 0x02,   /**< &nbsp; */
    UWD_AUDIO_FU_BASS_CONTROL              = 0x03,   /**< &nbsp; */
    UWD_AUDIO_FU_MID_CONTROL               = 0x04,   /**< &nbsp; */
    UWD_AUDIO_FU_TREBLE_CONTROL            = 0x05,   /**< &nbsp; */
    UWD_AUDIO_FU_GRAPHIC_EQUALIZER_CONTROL = 0x06,   /**< &nbsp; */
    UWD_AUDIO_FU_AUTOMATIC_GAIN_CONTROL    = 0x07,   /**< &nbsp; */
    UWD_AUDIO_FU_DELAY_CONTROL             = 0x08,   /**< &nbsp; */
    UWD_AUDIO_FU_BASS_BOOST_CONTROL        = 0x09,   /**< &nbsp; */
    UWD_AUDIO_FU_LOUDNESS_CONTROL          = 0x0A    /**< &nbsp; */
} uwd_audio_feature_unit_cs_t;

/** Extension Unit control selectors.
*/
typedef enum {
    UWD_AUDIO_EU_CONTROL_UNDEFINED = 0x00,   /**< &nbsp; */
    UWD_AUDIO_EU_ENABLE_CONTROL    = 0x01    /**< &nbsp; */
} uwd_audio_extension_unit_cs_t;

/** Special audio channel selection flag for all channels.
*/
#define UWD_AUDIO_CTRL_MASTER_CH  0x00
/** Special audio channel selection flag for the master channel (channel 0).
*/
#define UWD_AUDIO_CTRL_ALL_CH     0xff

/* Audio Channel Config */
#define UWD_AUDIO_CC_LEFT_FRONT            (1 << 0)
#define UWD_AUDIO_CC_RIGHT_FRONT           (1 << 1)
#define UWD_AUDIO_CC_CENTER_FRONT          (1 << 2)
#define UWD_AUDIO_CC_LOW_FREQ_ENHANCEMENT  (1 << 3)
#define UWD_AUDIO_CC_LEFT_SURROUND         (1 << 4)
#define UWD_AUDIO_CC_RIGHT_SURROUND        (1 << 5)
#define UWD_AUDIO_CC_LEFT_OF_CENTER        (1 << 6)
#define UWD_AUDIO_CC_RIGHT_OF_CENTER       (1 << 7)
#define UWD_AUDIO_CC_SURROUND              (1 << 8)
#define UWD_AUDIO_CC_SIDE_LEFT             (1 << 9)
#define UWD_AUDIO_CC_SIDE_RIGHT            (1 << 10)
#define UWD_AUDIO_CC_TOP                   (1 << 11)

/* Feature Unit Controls */
#define UWD_AUDIO_FC_MUTE              (1 << 0)
#define UWD_AUDIO_FC_VOLUME            (1 << 1)
#define UWD_AUDIO_FC_BASS              (1 << 2)
#define UWD_AUDIO_FC_MID               (1 << 3)
#define UWD_AUDIO_FC_TREBLE            (1 << 4)
#define UWD_AUDIO_FC_GRAPHIC_EQUALIZER (1 << 5)
#define UWD_AUDIO_FC_AUTOMATIC_GAIN    (1 << 6)
#define UWD_AUDIO_FC_DELAY             (1 << 7)
#define UWD_AUDIO_FC_BASS_BOOST        (1 << 8)
#define UWD_AUDIO_FC_LOUDNESS          (1 << 9)

/** Extension unit controls.
*/
#define UWD_AUDIO_EC_ENABLE_PROCESSING (1 << 0)

/** Audio entity handle.
*/
typedef struct uwd_audio_entity_s *uwd_audio_entity_h;


/* ===== Function type:  uwd_audio_entity_req_cb_t */
/**
  Application AudioControl interface request handler for a specific audio entity.

  @param app_ctx  [IN] -- Application context.
  @param entity   [IN] -- Pointer to an entity structure.
  @param code     [IN] -- Audio request code (see uwd_audio_entity_req_cb_t).
  @param value    [IN] -- wValue Set/Get Request field value (see the explanation 
                          in the Audio Class Specification). For requests that 
                          receive the Channel Number (CN) in the low byte (e.g., 
                          the Feature Unit control requests), use the 
                          UWD_AUDIO_CTRL_XXX flags to indicate special audio
                          channel targets:
					      - UWD_AUDIO_CTRL_ALL_CH -- All channels.
                          - UWD_AUDIO_CTRL_MASTER_CH -- The master channel (CN 0).
  @param len       [IN] -- Length of the data buffer, in bytes.
  @param *data [IN/OUT] -- Pointer to a buffer containing the parameters for the
                      request. Input for a Set request; output for a Get
                      request.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise, an error code.

  @requirement
  Optional.
*/
typedef jresult_t (*uwd_audio_entity_req_cb_t)(uwd_audio_app_h app_ctx, 
    uwd_audio_entity_h entity, uwd_audio_req_code_t code, juint16_t value, 
    juint16_t len, void *data);

/** Audio entity.
*/
typedef struct uwd_audio_entity_s {
    juint8_t                   id;        /**< Entity ID. */
    uwd_audio_entity_type_t   type;       /**< Entity type. */
    void                      *data;      /**< Pointer to buffer. */
    char                      *string;    /**< Entity name. */
    uwd_audio_entity_req_cb_t req_cb;     /**< Callback. */
    void                      *app_priv;  /**< Private application parameter. */
} uwd_audio_entity_t;

/** Input terminal.
*/
typedef struct {
    uwd_audio_trm_type_t trm_type;         /**< Terminal type. */
    juint8_t              assoc_terminal;
         /**< ID of the output terminal with which this input terminal is 
              associated. */
    juint8_t              nr_channels;
         /**< Number of logical output channels in the terminal's output
		      channel cluster. */
    juint16_t             channel_config;
         /**< Bitmap that describes the spatial location of the terminal's 
              logical channels. */
} uwd_audio_in_trm_t;

/** Output terminal.
*/
typedef struct {
    uwd_audio_trm_type_t trm_type;         /**< Terminal type. */
    juint8_t              assoc_terminal;
         /**< ID of the input terminal with which this output terminal is 
              associated. */
    juint8_t              source_id; 
         /**< ID of the unit or terminal to which this terminal is
		      connected. */
} uwd_audio_out_trm_t;

/** Feature unit.
*/
typedef struct {
  juint8_t              source_id;
       /**< ID of the unit or terminal to which this feature unit is
	        connected. */
  juint8_t              nr_channels;
       /**< Size, in bytes, of an element in the controls array. */
  juint16_t             *controls; 
       /**< A bitmap of the supported controls for the feature unit. */
} uwd_audio_feature_unit_t;

/** Selector unit.
*/
typedef struct {
    juint8_t                  nr_in_pins;     /**< Number of pins. */
    juint8_t                  *source_id;     /**< Source identification. */
} uwd_audio_selector_unit_t;

/** Extension unit.
*/
typedef struct {
    juint16_t                 extension_code;  /**< Extension code. */
    juint8_t                  nr_in_pins;      /**< Number of pins. */
    juint8_t                  *source_id;      /**< Source identification. */
    juint8_t                  nr_channels;     /**< Number of channels. */
    juint16_t                 channel_config;
         /**< Bitmap that describes the spatial location of the terminal's 
              logical channels. */
    juint8_t                  controls;
         /**< Bitmap of the supported controls for the feature unit. */     
} uwd_audio_extension_unit_t;

/** Audio data format type I, II, and III codes.
*/
typedef enum {
    UWD_AUDIO_DF_I_UNDEFINED                   = 0x0000,   /**< &nbsp; */
    UWD_AUDIO_DF_I_PCM                         = 0x0001,   /**< &nbsp; */
    UWD_AUDIO_DF_I_PCM8                        = 0x0002,   /**< &nbsp; */
    UWD_AUDIO_DF_I_IEEE_FLOAT                  = 0x0003,   /**< &nbsp; */
    UWD_AUDIO_DF_I_ALAW                        = 0x0004,   /**< &nbsp; */
    UWD_AUDIO_DF_I_MULAW                       = 0x0005,   /**< &nbsp; */
    UWD_AUDIO_DF_II_UNDEFINED                  = 0x1000,   /**< &nbsp; */
    UWD_AUDIO_DF_II_MPEG                       = 0x1001,   /**< &nbsp; */
    UWD_AUDIO_DF_II_AC_3                       = 0x1002,   /**< &nbsp; */
    UWD_AUDIO_DF_III_UNDEFINED                 = 0x2000,   /**< &nbsp; */
    UWD_AUDIO_DF_III_IEC1937_AC_3              = 0x2001,   /**< &nbsp; */
    UWD_AUDIO_DF_III_IEC1937_MPEG_1_L1         = 0x2002,   /**< &nbsp; */
    UWD_AUDIO_DF_III_IEC1937_MPEG_1_L2_L3      = 0x2003,   /**< &nbsp; */
    UWD_AUDIO_DF_III_IEC1937_MPEG_2_NOEXT      = 0x2003,   /**< &nbsp; */
    UWD_AUDIO_DF_III_IEC1937_MPEG_2_EXT        = 0x2004,   /**< &nbsp; */
    UWD_AUDIO_DF_III_IEC1937_MPEG_2_L1         = 0x2005,   /**< &nbsp; */
    UWD_AUDIO_DF_III_IEC1937_MPEG_2_L2_L3      = 0x2006    /**< &nbsp; */
} uwd_audio_format_type_t;

/** Audio Streaming Interface handle.
*/
typedef struct uwd_audio_stream_s *uwd_audio_stream_h;

/** Audio Streaming Interface alternate setting handle.
*/
typedef struct uwd_audio_stream_alt_s *uwd_audio_stream_alt_h;


/* ===== Function type:  uwd_audio_stream_select_cb_t */
/**
  Notifies the application of a need to change the audio settings of an 
  Audio Streaming Interface.  Notification can be through either a 
  USB SET_INTERFACE request or an audio sampling frequency control 
  SAMPLING_FREQ_CONTROL Set request.

  @param app_ctx    [IN] -- Application context.
  @param stream     [IN] -- Pointer to an Audio Streaming Interface structure.
  @param stream_alt [IN] -- Pointer to an Audio Streaming Interface alternate
                            setting structure.
                            NULL represents alternate setting 0, which indicates
                            that the host requested to stop the audio stream.
  @param samp_freq  [IN] -- Sampling frequency, in Hz.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @requirement
  Mandatory.
*/
typedef void (*uwd_audio_stream_select_cb_t)(uwd_audio_app_h app_ctx,
    uwd_audio_stream_h stream, uwd_audio_stream_alt_h stream_alt, 
    juint32_t samp_freq);

/* ===== Function type:  uwd_audio_stream_req_cb_t */
/**
 Application Audio Streaming Interface control request handler.

  @param app_ctx   [IN] -- Application context.
  @param stream    [IN] -- Pointer to an Audio Streaming Interface structure.
  @param code      [IN] -- Enumeration of audio class-specific request codes.
  @param value     [IN] -- Value.
  @param len       [IN] -- Data length.
  @param *data [IN/OUT] -- Data buffer.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @requirement
  Optional.
*/
typedef jresult_t (*uwd_audio_stream_req_cb_t)(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, uwd_audio_req_code_t code, juint16_t value, 
    juint16_t len, void *data);

/* ===== Function type:  uwd_audio_ep_req_cb_t */
/**
  Application Audio Streaming endpoint control request handler.

  @param app_ctx   [IN] -- Application context.
  @param stream    [IN] -- Pointer to an Audio Streaming Interface structure.
  @param code      [IN] -- Audio request code (see uwd_audio_entity_req_cb_t).
  @param cs        [IN] -- Endpoint control selector code (see 
                          uwd_audio_endpoint_cs_t).
  @param len       [IN] -- Length of the data buffer, in bytes.
  @param *data [IN/OUT] -- Pointer to a buffer containing the parameters for the
                           request. Input for a Set request; output for a Get
                           request.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @requirement
  Optional.
*/
typedef jresult_t (*uwd_audio_ep_req_cb_t)(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, uwd_audio_req_code_t code, 
    uwd_audio_endpoint_cs_t cs, juint16_t len, void *data);

/** Audio polling interval.
    For Audio Streaming endpoints, only an interval of 1 millisecond can be
	used (according to the Audio Class 1.0 Specification).
*/
typedef enum {
    UWD_AUDIO_NO_DATA = 0,   /**< &nbsp; */
    UWD_AUDIO_IVAL_1_MS,     /**< &nbsp; */
    UWD_AUDIO_IVAL_2_MS,     /**< &nbsp; */
    UWD_AUDIO_IVAL_4_MS,     /**< &nbsp; */
    UWD_AUDIO_IVAL_8_MS,     /**< &nbsp; */
    UWD_AUDIO_IVAL_16_MS,    /**< &nbsp; */
    UWD_AUDIO_IVAL_32_MS,    /**< &nbsp; */
    UWD_AUDIO_IVAL_64_MS,    /**< &nbsp; */
    UWD_AUDIO_IVAL_128_MS    /**< &nbsp; */
} uwd_audio_ival_t;

/** Streaming interface alternate setting.
*/
typedef struct uwd_audio_stream_alt_s {
    uwd_audio_ival_t         ep_ival;     /**< Polling interval. */

/** Used by the Audio function driver to calculate the maximum endpoint packet 
    size.
*/
#define UWD_AUDIO_FD_CALC_MAX_PACKET 0

/** Used by the Audio function driver to calculate the maximum endpoint packet 
    size, allowing for clock synchronization using uwd_audio_stream_sync().
*/
#define UWD_AUDIO_FD_CALC_MAX_PACKET_SYNC 0xffff
    juint16_t                 ep_max_packet;
    juint8_t                  trm_link;
    juint8_t                  delay;
    uwd_audio_format_type_t  format_type;
    void                     *format;
} uwd_audio_stream_alt_t;

/** Audio Streaming Interface structure.
*/
typedef struct uwd_audio_stream_s {
    juint8_t                      alts_num;  /**< Number of alternate
											      settings. */
    uwd_audio_stream_alt_t       *alts;      /**< Alternate settings array. */

    uwd_audio_stream_select_cb_t select_cb;  /**< Stream select callback. */
    uwd_audio_stream_req_cb_t    req_cb;     /**< Stream control request
											      callback. */
    uwd_audio_ep_req_cb_t        ep_req_cb;  /**< Endpoint control request 
                                                  callback. */
    
    void                         *app_priv;  /**< Application private
											      parameter. */
    jbool_t                       read;      /**< Initialized by the function 
                                                  driver. */
} uwd_audio_stream_t;

/** Continuous sampling frequency.
*/
typedef struct {
    juint32_t min;                /**< Minimum frequency. */
    juint32_t max;                /**< Maximum frequency. */
} uwd_audio_freq_continuous_t;

/** Discrete sampling frequency.
*/
typedef struct {
    juint8_t freqs_num;           /**< Number of frequencies. */
    juint32_t *freqs;             /**< Frequencies array. */
} uwd_audio_freq_discrete_t; 

/** Audio Type I format type.
*/
typedef struct {
    juint8_t                     nr_channels;     /**< Number of channels. */
    juint8_t                     bit_resolution;
         /**< Number of effectively used bits from the available bits in the 
              audio subframe. */

    uwd_audio_freq_discrete_t   *freq_discrete; 
         /**< Discrete sample frequencies. */

    uwd_audio_freq_continuous_t *freq_continuous;
         /**< Continious sample frequencies. */
} uwd_audio_format_I_t;

#define UWD_AUDIO_SUBFRAME_SIZE(format) (((format)->bit_resolution+7) >> 3)

/* Audio Type II Format - currently not supported */

/** Audio Type III format type.
*/
typedef uwd_audio_format_I_t uwd_audio_format_III_t;

/* ===== Function type:  uwd_audio_stream_cb_t */
/**
  Application audio stream transfer completion callback, for the 
  uwd_audio_stream_read() and uwd_audio_stream_write() functions.

  @param app_ctx [IN] -- Application context.
  @param stream  [IN] -- Pointer to an Audio Streaming Interface structure.
  @param *arg    [IN] -- Pointer to arguments received from uwd_audio_stream_read()
                         or uwd_audio_stream_write(). NULL if not required.
  @param status  [IN] -- Transfer completion status: Zero (0) on success; otherwise,
                         an error code.
  @param len     [IN] -- Length.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @requirement
  Mandatory.
*/
typedef void (*uwd_audio_stream_cb_t)(uwd_audio_app_h app_ctx, 
    uwd_audio_stream_h stream, void *arg, jresult_t status, juint32_t len);

/* ===== Function type:  uwd_audio_uninit_cb_t */
/**
  Application un-initialization callback.

  @param app_ctx [IN] -- Application context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @requirement
  Mandatory.
*/
typedef void (*uwd_audio_uninit_cb_t)(uwd_audio_app_h app_ctx);

/** Audio application information.
    This structure should be available and unchanged until the uninit callback is 
    called.
*/
typedef struct {
    uwd_audio_entity_t *entities;        /**< Audio entities array. */
    juint8_t             entities_num;   /**< Size of the audio entities
										      array. */

    uwd_audio_stream_t *streams;         /**< Streams array. */
    juint8_t             streams_num;    /**< Size of the streams array. */

    uwd_audio_ival_t    intr_ep_ival;    /**< Polling interval. */

    uwd_audio_uninit_cb_t uninit;        /**< Uninit callback. */
} uwd_audio_config_t;

/** @} */ /* end_group hsusb_jungo_api */

#endif
