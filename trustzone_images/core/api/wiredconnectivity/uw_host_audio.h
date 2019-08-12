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


/**@file uw_host_audio.h

USB Audio Class Driver functions to support host communication 
with audio USB peripherals. 

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_host_audio.h#1 $
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

#ifndef __UW_HOST_AUDIO_H__
#define __UW_HOST_AUDIO_H__

#include <jos.h>
#include <jusb.h>
#include "uw_host_appmgr.h"


/** @ingroup hsusb_jungo_api 
@{ */

/** Class-specific Audio Control interface descriptor subtypes.
*/
enum uwh_audio_ctrl_desc_subtype {
    UWH_ACD_UNDEFINED            = 0x00,   /**< &nbsp; */
    UWH_ACD_HEADER               = 0x01,   /**< &nbsp; */
    UWH_ACD_INPUT_TERMINAL       = 0x02,   /**< &nbsp; */
    UWH_ACD_OUTPUT_TERMINAL      = 0x03,   /**< &nbsp; */
    UWH_ACD_MIXER_UNIT           = 0x04,   /**< &nbsp; */
    UWH_ACD_SELECTOR_UNIT        = 0x05,   /**< &nbsp; */
    UWH_ACD_FEATURE_UNIT         = 0x06,   /**< &nbsp; */
    UWH_ACD_PROCESSING_UNIT      = 0x07,   /**< &nbsp; */
    UWH_ACD_EXTENSION_UNIT       = 0x08    /**< &nbsp; */
};

/** Class-specific Audio Streaming Interface descriptor subtypes.
*/
enum uwh_audio_stream_desc_subtype {
    UWH_ASD_UNDEFINED            = 0x00,   /**< &nbsp; */
    UWH_ASD_GENERAL              = 0x01,   /**< &nbsp; */
    UWH_ASD_FORMAT_TYPE          = 0x02,   /**< &nbsp; */
    UWH_ASD_FORMAT_SPECIFIC      = 0x03    /**< &nbsp; */
};

/** Audio interface subclass codes.
*/
enum uwh_audio_iface_subclass_code {
    UWH_AI_UNDEFINED             = 0x00,   /**< &nbsp; */
    UWH_AI_AUDIOCONTROL          = 0x01,   /**< &nbsp; */
    UWH_AI_AUDIOSTREAMING        = 0x02,   /**< &nbsp; */
    UWH_AI_MIDISTREAMING         = 0x03    /**< &nbsp; */
};

/** Audio class-specific request codes.
*/
typedef enum uwh_audio_req_code {
    UWH_ARC_UNDEFINED             = 0x00,  /**< -- Undefined audio request. */
    UWH_ARC_SET_CUR               = 0x01,  /**< -- Set the current setting attribute. */
    UWH_ARC_GET_CUR               = 0x81,  /**< -- Get the current setting attribute. */
    UWH_ARC_SET_MIN               = 0x02,  /**< -- Set the minimum setting attribute. */
    UWH_ARC_GET_MIN               = 0x82,  /**< -- Get the minimum setting attribute. */
    UWH_ARC_SET_MAX               = 0x03,  /**< -- Set the maximum setting attribute. */
    UWH_ARC_GET_MAX               = 0x83,  /**< -- Get the maximum setting attribute. */
    UWH_ARC_SET_RES               = 0x04,  /**< -- Set the resolution attribute. */
    UWH_ARC_GET_RES               = 0x84,  /**< -- Get the resolution attribute. */
    UWH_ARC_SET_MEM               = 0x05,  /**< -- Set the memory space attribute. */
    UWH_ARC_GET_MEM               = 0x85,  /**< -- Get the memory space attribute. */
    UWH_ARC_GET_STAT              = 0xFF   /**< -- Get the status. */
} uwh_audio_req_code_t;

/** Terminal control selectors.
*/
enum uwh_audio_terminal_cs {
    UWH_AT_CONTROL_UNDEFINED     = 0x00,   /**< &nbsp; */
    UWH_AT_COPY_PROTECT_CONTROL  = 0x01    /**< &nbsp; */
};

/** Endpoint control selectors.
*/
enum uwh_audio_endpoint_cs {
    UWH_AEP_CONTROL_UNDEFINED             = 0x00,   /**< &nbsp; */
    UWH_AEP_SAMPLING_FREQ_CONTROL         = 0x01,   /**< &nbsp; */
    UWH_AEP_PITCH_CONTROL                 = 0x02    /**< &nbsp; */
};

/** Feature unit control selectors.
*/
enum uwh_audio_feature_unit_cs {
    UWH_AFU_CONTROL_UNDEFINED             = 0x00,   /**< &nbsp; */
    UWH_AFU_MUTE_CONTROL                  = 0x01,   /**< &nbsp; */
    UWH_AFU_VOLUME_CONTROL                = 0x02,   /**< &nbsp; */
    UWH_AFU_BASS_CONTROL                  = 0x03,   /**< &nbsp; */
    UWH_AFU_MID_CONTROL                   = 0x04,   /**< &nbsp; */
    UWH_AFU_TREBLE_CONTROL                = 0x05,   /**< &nbsp; */
    UWH_AFU_GRAPHIC_EQUALIZER_CONTROL     = 0x06,   /**< &nbsp; */
    UWH_AFU_AUTOMATIC_GAIN_CONTROL        = 0x07,   /**< &nbsp; */
    UWH_AFU_DELAY_CONTROL                 = 0x08,   /**< &nbsp; */
    UWH_AFU_BASS_BOOST_CONTROL            = 0x09,   /**< &nbsp; */
    UWH_AFU_LOUDNESS_CONTROL              = 0x0A    /**< &nbsp; */
};

/** Format type codes.
*/
enum uwh_audio_format_type {
    UWH_AF_TYPE_UNDEFINED         = 0x00,   /**< -- Undefined format type. */
    UWH_AF_TYPE_I                 = 0x01,   /**< -- Audio format type 1. */
    UWH_AF_TYPE_II                = 0x02,   /**< -- Audio format type 2. */
    UWH_AF_TYPE_III               = 0x03    /**< -- Audio format type 3. */
};

/** Audio data format Type I, II,and III codes.
*/
enum uwh_audio_data_format_type {
    UWH_ADF_I_UNDEFINED                   = 0x0000,   /**< &nbsp; */
    UWH_ADF_I_PCM                         = 0x0001,   /**< &nbsp; */
    UWH_ADF_I_PCM8                        = 0x0002,   /**< &nbsp; */
    UWH_ADF_I_IEEE_FLOAT                  = 0x0003,   /**< &nbsp; */
    UWH_ADF_I_ALAW                        = 0x0004,   /**< &nbsp; */
    UWH_ADF_I_MULAW                       = 0x0005,   /**< &nbsp; */
    UWH_ADF_II_UNDEFINED                  = 0x1000,   /**< &nbsp; */
    UWH_ADF_II_MPEG                       = 0x1001,   /**< &nbsp; */
    UWH_ADF_II_AC_3                       = 0x1002,   /**< &nbsp; */
    UWH_ADF_III_UNDEFINED                 = 0x2000,   /**< &nbsp; */
    UWH_ADF_III_IEC1937_AC_3              = 0x2001,   /**< &nbsp; */
    UWH_ADF_III_IEC1937_MPEG_1_L1         = 0x2002,   /**< &nbsp; */
    UWH_ADF_III_IEC1937_MPEG_1_L2_L3      = 0x2003,   /**< &nbsp; */
    UWH_ADF_III_IEC1937_MPEG_2_NOEXT      = 0x2003,   /**< &nbsp; */
    UWH_ADF_III_IEC1937_MPEG_2_EXT        = 0x2004,   /**< &nbsp; */
    UWH_ADF_III_IEC1937_MPEG_2_L1         = 0x2005,   /**< &nbsp; */
    UWH_ADF_III_IEC1937_MPEG_2_L2_L3      = 0x2006    /**< &nbsp; */
};

/** USB terminal types (USB, Input, and Output).
*/
enum uwh_audio_terminal_type {
    
/* USB Terminal Types */
    UWH_ATT_USB_UNDEFINED                 = 0x0100,  /**< &nbsp; */
    UWH_ATT_USB_STREAMING                 = 0x0101,  /**< &nbsp; */
    UWH_ATT_USB_VENDOR_SPECIFIC           = 0x01FF,  /**< &nbsp; */

/* Input Terminal Types */
    UWH_ATT_INPUT_UNDEFINED               = 0X0200,  /**< &nbsp; */
    UWH_ATT_MICROPHONE                    = 0X0201,  /**< &nbsp; */
    UWH_ATT_DESKTOP_MICROPHONE            = 0X0202,  /**< &nbsp; */
    UWH_ATT_PERSONAL_MICROPHONE           = 0X0203,  /**< &nbsp; */
    UWH_ATT_OMNI_DIRECTIONAL_MICROPHONE   = 0X0204,  /**< &nbsp; */
    UWH_ATT_MICROPHONE_ARRAY              = 0X0205,  /**< &nbsp; */
    UWH_ATT_PROCESSING_MICROPHONE_ARRAY   = 0X0206,  /**< &nbsp; */

/* Output Terminal Types */
    UWH_ATT_OUTPUT_UNDEFINED              = 0X0300,  /**< &nbsp; */
    UWH_ATT_SPEAKER                       = 0X0301,  /**< &nbsp; */
    UWH_ATT_HEADPHONES                    = 0X0302,  /**< &nbsp; */
    UWH_ATT_HEAD_MOUNTED_DISPLAY_AUDIO    = 0X0303,  /**< &nbsp; */
    UWH_ATT_DESKTOP_SPEAKER               = 0X0304,  /**< &nbsp; */
    UWH_ATT_ROOM_SPEAKER                  = 0X0305,  /**< &nbsp; */
    UWH_ATT_COMMUNICATION_SPEAKER         = 0X0306,  /**< &nbsp; */
    UWH_ATT_LOW_FREQUENCY_EFFECTS_SPEAKER = 0X0307   /**< &nbsp; */
};

/* ADC v1.0 Descriptors */

/* Class-specific Audio Control interface descriptors. */
typedef __PACKED_PRE__ struct {
    uByte    bLength;             /* Descriptor length, in bytes. */
    uByte    bDescriptorType;     /* Descriptor type. */
    uByte    bDescriptorSubtype;  /* Descriptor subtype. */
    uByte    bTerminalID;         /* Terminal ID. */
    uWord    wTerminalType;       /* Enum uwh_audio_terminal_type -- defines 
                                       the terminal type. */
    uByte    bAssocTerminal;      /* ID of the output terminal with 
                                       which this input terminal is associated. */
    uByte    bNrChannels;         /* Number of logical output channels 
                                       in the terminal’s output channel cluster. */
    uWord    wChannelConfig;      /* Bitmap that describes the spatial location 
                                       of the terminal’s logical channels. */
    uByte    iChannelNames;       /* Index of a string descriptor that 
                                       describes the name of the terminal’s 
                                       first logical channel. */
    uByte    iTerminal;           /* Index of a string descriptor that describes 
                                       the input terminal. */
} __PACKED_POST__ uwh_audio_input_terminal_desc_t;

/* Output terminal descriptor structure. */
typedef __PACKED_PRE__ struct {
    uByte    bLength;             /* Descriptor length, in bytes. */
    uByte    bDescriptorType;     /* Descriptor type. */
    uByte    bDescriptorSubtype;  /* Descriptor subtype. */
    uByte    bTerminalID;         /* Terminal ID. */
    uWord    wTerminalType;       /* Enum uwh_audio_terminal_type -- defines 
                                       the terminal type.  */
    uByte    bAssocTerminal;      /* ID of the input terminal to which this
                                       output terminal is associated.  */
    uByte    bSourceID;           /* ID of the unit or terminal to which
                                       this terminal is connected. */
    uByte    iTerminal;           /* Index of a string descriptor that describes
                                       this output terminal. */
}  __PACKED_POST__ uwh_audio_output_terminal_desc_t;

/* Class-specific Audio Streaming interface descriptors.
*/
typedef __PACKED_PRE__ struct {
    uByte    bLength;             /* Descriptor length, in bytes. */
    uByte    bDescriptorType;     /* Descriptor type. */    
    uByte    bDescriptorSubtype;  /* Descriptor subtype -– must be ASD_GENERAL. */
    uByte    bTerminalLink;       /* Terminal ID of the terminal to which the
                                       interface’s endpoint is connected. */
    uByte    bDelay;              /* Delay introduced by the data path, expressed 
                                       in number of frames. */
    uWord    wFormatTag;          /* Audio data format that has to be used
                                       to communicate with this interface,
                                       (Enum uwh_audio_data_format_type). */
} __PACKED_POST__ uwh_audio_sif_desc_t;

typedef juint8_t uwh_audio_samp_freq_t[3];

/* Audio Type I format type descriptor. */
typedef __PACKED_PRE__ struct {
    uByte    bLength;             /* Descriptor length, in bytes. */
    uByte    bDescriptorType;     /* Descriptor type. */
    uByte    bDescriptorSubtype;  /* Descriptor subtype. */
    uByte    bFormatType;         /* Type I format type -- enum 
                                     uwh_audio_format_type. */
    uByte    bNrChannels;         /* Number of physical channels in the 
                                       audio data stream. */
    uByte    bSubframeSize;       /* Number of bytes occupied by one audio 
                                       subframe. */
    uByte    bBitResolution;      /* Number of effectively used bits from the
                                       available bits in the audio subframe. */
    uByte    bSamFreqType;        /* Indicates how the sampling frequency can 
                                       be programmed (continuous/discrete 
                                       sampling)*/
    uwh_audio_samp_freq_t tSamFreq[1];   /* Sampling frequencies expressed in 
                                              a 3-byte format. */
} __PACKED_POST__ uwh_audio_format_I_desc_t;

/* Audio Type II format type descriptor. */
typedef __PACKED_PRE__ struct {
    uByte    bLength;             /* Descriptor length, in bytes. */
    uByte    bDescriptorType;     /* Descriptor type. */
    uByte    bDescriptorSubtype;  /* Descriptor subtype. */
    uByte    bFormatType;         /* Type II format type -– enum 
                                       uwh_audio_format_type. */
    uWord    wMaxBitRate;         /* Maximum number of bits per second that 
                                       this interface can handle, expressed in 
                                       kilobits/second. */
    uWord    wSamplesPerFrame;    /* Number of PCM audio samples contained
                                       in one encoded audio frame. */
    uByte    bSamFreqType;        /* Indicates how the sampling frequency can 
                                        be programmed (continuous/discrete 
                                        sampling). */
    uwh_audio_samp_freq_t tSamFreq[1];  /* Sampling frequencies expressed in a 
                                             3-byte format. */
} __PACKED_POST__ uwh_audio_format_II_desc_t;

/** Audio Type III format type descriptor.
*/
typedef uwh_audio_format_I_desc_t uwh_audio_format_III_desc_t;

/* Audio isochronous endpoint descriptor. */
typedef __PACKED_PRE__ struct {
    uByte    bLength;             /* Descriptor length, in bytes. */
    uByte    bDescriptorType;     /* Descriptor type. */
    uByte    bDescriptorSubtype;  /* Descriptor subtype. */
    uByte    bmAttributes;        /* Descriptor attributes. */
    uByte    bLockDelayUnits;     /* Lock delay unit. */
    uWord    wLockDelay;          /* Lock delay. */
} __PACKED_POST__ uwh_audio_isoc_ep_desc_t;

/* Feature unit descriptor. */
typedef __PACKED_PRE__ struct {
    uByte    bLength;             /* Descriptor length, in bytes. */
    uByte    bDescriptorType;     /* Descriptor type. */
    uByte    bDescriptorSubtype;  /* Descriptor subtype. */
    uByte    bUnitID;             /* Unit ID. */
    uByte    bSourceID;           /* The ID of the unit or terminal to which 
                                       this feature unit is connected. */
    uByte    bControlSize;        /* Size, in bytes, of an element in the
                                       bmaControls array. */
} __PACKED_POST__ uwh_audio_feature_unit_desc_t;

/** Audio class driver context.
*/
typedef void *uwh_audio_drv_h;

/** Audio application context.
*/
typedef void *uwh_audio_app_h;

/** Audio entity type.\ (This is a copy of the class-specific Audio Control 
  interface descriptor).
 */
typedef enum uwh_audio_ctrl_desc_subtype uwh_audio_entity_type_t;

/** Audio Streaming Interface information structure.
*/
typedef struct
{
    uwh_audio_sif_desc_t            *sif_desc;
      /**< Pointer to a class-specific streaming interface descriptor structure. */
    usb_endpoint_descriptor_t       *ep_desc;
      /**< Pointer to a USB endpoint descriptor. */
    uwh_audio_isoc_ep_desc_t        *ep_audio_desc;
      /**< Pointer to an Audio isochronous endpoint descriptor. */

    enum uwh_audio_format_type      format_type;
      /**< Audio format type. */

     /** Audio data format type descriptor union. */
    union {
        uwh_audio_format_I_desc_t   *type_I;
          /**< Pointer to an Audio Type I format type descriptor. */
        uwh_audio_format_II_desc_t  *type_II;
          /**< Pointer to an Audio Type II format type descriptor. */
        uwh_audio_format_III_desc_t *type_III;
          /**< Pointer to an Audio Type III format type descriptor. */
    } format;
} uwh_audio_stream_iface_t;

/** Audio terminals and units.
Only input and output terminals are currently supported.
*/
typedef struct {
    uwh_audio_input_terminal_desc_t *terminal_desc; 
      /**< Pointer to an audio input terminal descriptor structure. */
    juint8_t sif_num;
      /**< Audio Streaming Interface number. */
    uwh_audio_stream_iface_t *sif;
      /**< Pointer to the beginning of an array of ASI information structures. */
} uwh_audio_input_terminal_t;

/** Audio output terminal information structure.
*/
typedef struct {
    uwh_audio_output_terminal_desc_t *terminal_desc;
      /**< Pointer to an audio output terminal descriptor structure. */
    juint8_t sif_num; 
      /**< Audio Streaming Interface number. */
    uwh_audio_stream_iface_t *sif;
      /**< Pointer to the beginning of an array of ASI information structures. */
} uwh_audio_output_terminal_t;

/** Audio entity.
*/
typedef struct {
    juint8_t entity_id;             /**< Entity ID. */
    uwh_audio_entity_type_t type;
      /**< Entity type -- uwh_audio_entity_type_t enumeration. */

     /** Entity information union. */
    union {
        uwh_audio_input_terminal_t  *in_trm;
          /**< Pointer to an input terminal information structure. */
        uwh_audio_output_terminal_t *out_trm;
          /**< Pointer to an output terminal information structure. */
        void                        *unit_desc;
          /**< Pointer to unit descriptor information. */
    } u;
} uwh_audio_entity_t;

/** Audio notification information.
*/
typedef enum {
    UWH_AN_CTRL_INTERFACE = 0,     /**< &nbsp; */
    UWH_AN_STREAM_INTERFACE = 1,   /**< &nbsp; */
    UWH_AN_ENDPOINT = 2            /**< &nbsp; */
} uwh_audio_notify_originator_t;

/** Status interrupt endpoint notification orginiator information structure.
*/
typedef struct {
    juint_t  memory_contents_changed : 1;
      /**< Indicates whether the memory contents of an addressable entity 
           inside the audio function have changed. */
    juint_t  interrupt_pending       : 1;
      /**< Pending interrupt indicator. */

    uwh_audio_notify_originator_t originator_type;
      /**< The type of the notification's originator. */

     /** Notification originator ID union. */
     union {
        juint8_t entity_id;
          /**< Entity (terminal/unit) ID for an entity originator. */
        juint8_t stream_iface;
          /**< Audio Streaming Interface ID for an ASI originator. */
        juint8_t ep;
          /**< Audio Streaming endpoint ID for an endpoint originator. */
    } originator;
} uwh_audio_notify_info_t;

/* ///////////////////////////////
     Audio Application Callbacks
 * ////////////////////////////// */

typedef jresult_t (*uwh_audio_attach_cb_t)(uwh_audio_drv_h drv_ctx, 
    uwh_audio_app_h *app_ctx);

typedef void (*uwh_audio_detach_cb_t)(uwh_audio_app_h app_ctx);

typedef void (*uwh_audio_notify_cb_t)(uwh_audio_app_h app_ctx,
    uwh_audio_notify_info_t *notify_info);

/** Application callback functions structure.
*/
typedef struct {
    uwh_app_probe_cb_t    probe;     /**< Probe callback. */
    uwh_audio_attach_cb_t attach;    /**< Device attach callback. */
    uwh_audio_detach_cb_t detach;    /**< Device detach callback. */
    uwh_audio_notify_cb_t notify;    /**< Status interrupt endpoint notifications 
                                          callback. */
} uwh_audio_callbacks_t;

/* ///////////////////////////////
    Audio Class Driver Functions
 * ////////////////////////////// */

/* ===== Function name:  uwh_audio_register */
/**
  Registers an audio application with the Audio Class Driver.\n

  The class driver is operational only after the registration function is 
  called and returns successfully.

  @param *cbs [IN] -- Application callbacks.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Application handle.

  @scope
  Global.
*/
uwh_app_h uwh_audio_register(uwh_audio_callbacks_t *cbs);


/* ===== Function name:  uwh_audio_unregister */
/**
  Unregisters an audio application from the Audio Class Driver.

  @param app_h [IN] -- Application handle.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void uwh_audio_unregister(uwh_app_h app_h);

/* /////////////////////////////////////////////
   Audio Units and Terminals Access Functions 
 * ///////////////////////////////////////////// */

/* ===== Function name:  uwh_audio_get_first_entity */
/**
  Returns the first audio entity in the Audio class driver's device entities list.

  @param drv_ctx [IN] -- Audio driver context.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Pointer to the first audio entity in the list.

  @scope
  Global.
*/
uwh_audio_entity_t *uwh_audio_get_first_entity(uwh_audio_drv_h drv_ctx);


/* ===== Function name:  uwh_audio_get_next_entity */
/**
  Returns the next audio entity in the Audio class driver's device entities list.

  @param drv_ctx     [IN] -- Audio driver context.
  @param *cur_entity [IN] -- Pointer to the current audio entity in the class 
                             driver's device entities list.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Pointer to next audio entity in the list.\n
  NULL if the current entity is the last entity in the list.

  @scope
  Global.
*/
uwh_audio_entity_t *uwh_audio_get_next_entity(uwh_audio_drv_h drv_ctx, 
    uwh_audio_entity_t *cur_entity);


/* ===== Function name:  uwh_audio_get_entity */
/**
  Returns an audio entity by ID.
 
  @param drv_ctx   [IN] -- Audio driver context.
  @param entity_id [IN] -- Audio entity ID.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Pointer to the entity. NULL if no matching entity is found.

  @scope
  Global.
*/
uwh_audio_entity_t *uwh_audio_get_entity(uwh_audio_drv_h drv_ctx, 
    juint8_t entity_id);

/* ////////////////
   Audio Requests 
 * /////////////// */

/** Enumeration of audio control request types.
*/
typedef enum {
    UWH_AR_ENDPOINT,           /**< &nbsp; */
    UWH_AR_CTRL_INTERFACE,     /**< &nbsp; */
    UWH_AR_STREAM_INTERFACE    /**< &nbsp; */
} uwh_audio_ctrl_req_type_t;

/** Class-specific audio control request information structure.
*/
typedef struct {
    uwh_audio_entity_t        *entity;     /**< Pointer to an audio entity information structure. */

    void                      *buffer;     /**< Pointer to the request's data buffer. */
    juint32_t                  buf_len;    /**< The length of the data buffer, in bytes. */

    uwh_audio_ctrl_req_type_t type;        /**< Control request type. */
    uwh_audio_req_code_t      code;        /**< Class-specific audio request code. */
    juint8_t                   ctrl_sel;   /**< Control selector - Enum xxx_cs  */
    juint16_t                  value;      /**< The value of the request's wValue field. */
} uwh_audio_ctrl_req_t;


/* ===== Function name:  uwh_audio_ctrl_req */
/**
  Sends a control request to the audio device.

  @param drv_ctx  [IN] -- Audio driver context.
  @param *req [IN/OUT] -- Pointer to an audio control request structure.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise, an error code.

  @scope
  Global.
*/
jresult_t uwh_audio_ctrl_req(uwh_audio_drv_h drv_ctx, uwh_audio_ctrl_req_t *req);

/* ////////////////
   Audio Streaming 
 * /////////////// */

/* ===== Function name:  uwh_audio_set_stream */
/**
  Sets the alternate setting of the Audio Streaming Inteface for a given USB 
  streaming terminal.

  @param drv_ctx [IN] -- Audio driver context 
  @param *trm    [IN] -- Pointer to a USB streaming terminal entity structure.
  @param *sif    [IN] -- Pointer to an Audio Streaming Interface structure.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise, an error code.

  @scope
  Global.
*/
jresult_t uwh_audio_set_stream(uwh_audio_drv_h drv_ctx, uwh_audio_entity_t *trm, 
    uwh_audio_stream_iface_t *sif);


/* ===== Function name:  uwh_audio_samp_freq_req */
/**
  Sends a SAMPLING_FREQ_CONTROL request to a given terminal's endpoint.

  @param drv_ctx      [IN] -- Audio driver context.
  @param *trm         [IN] -- Pointer to a USB streaming terminal entity structure.
  @param code         [IN] -- Request code.
  @param *freq_hz [IN/OUT] -- Pointer to sampling frequency in Hz.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise, an error code.

  @scope
  Global.
 */
jresult_t uwh_audio_samp_freq_req(uwh_audio_drv_h drv_ctx, 
    uwh_audio_entity_t *trm, uwh_audio_req_code_t code, juint32_t *freq_hz);


/* Function type:  uwh_audio_write_cb_t */
/**
  Writes a completion application callback.

  @param *arg   [IN] -- Pointer to callback arguments.
  @param status [IN] -- Write transfer status.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.
 */
typedef void (*uwh_audio_write_cb_t)(void *arg, jresult_t status);


/* ===== Function name:  uwh_audio_write_data */
/**
  Writes data to the device using a given USB audio stream.

  @param entity [IN] -- Pointer to a USB streaming terminal entity structure.
  @param *buf   [IN] -- Handle to a memory buffer containing the data to write.
  @param len    [IN] -- Data length, in bytes.
  @param cb     [IN] -- Writes the completion application callback.
  @param *arg   [IN] -- Pointer to arguments for the application's write
                        completion callback.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise, an error code.

  @scope
  Global.
*/
jresult_t uwh_audio_write_data(uwh_audio_entity_t *entity, void *buf, 
    juint32_t len, uwh_audio_write_cb_t cb, void *arg);


/* ===== Function type:  uwh_audio_read_cb_t
/**
  Application read callback.

  @param *arg   [IN] -- Pointer to the callback arguments.
  @param status [IN] -- Read stream status.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.
 */
typedef void (*uwh_audio_read_cb_t)(void *arg, jresult_t status);


/* ===== Function name:  uwh_audio_read_start */
/**
  Starts reading data from the device using a given USB audio stream.\ The size 
  of the read stream buffer is determined by the buf_time_ms parameter. 

  When the data is received from the device, the application's read callback 
  function (cb) is called. If the application does not call 
  uwh_audio_read_get_data() to retrieve the read data within buf_time_ms of the 
  callback's arrival:
   - The next read attempt using the same stream buffer fails.
   - The read stream is stopped (aborted).
   - The application's read callback is called with a JETIMEDOUT status.

  @param *entity     [IN] -- Pointer to a USB streaming terminal entity structure.
  @param buf_time_ms [IN] -- Stream buffering time in milliseconds.
  @param cb          [IN] -- Application read callback.
  @param *arg        [IN] -- Pointer to the arguments for the application's read
                             callback.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise, an error code.

  @scope
  Global.
*/
jresult_t uwh_audio_read_start(uwh_audio_entity_t *entity, juint16_t buf_time_ms, 
    uwh_audio_read_cb_t cb, void *arg);


/* ===== Function name:  uwh_audio_read_get_data_size */
/**
  Returns the actual amount of data received from the device and not yet read by 
  the application.

  This function is used for read transfers of fixed-size audio formats 
  (i.e., Type I and Type III audio data format types).

  @param *entity [IN] -- Pointer to a USB streaming terminal entity structure.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Read data size, in bytes.

  @scope
  Global.
*/
juint32_t uwh_audio_read_get_data_size(uwh_audio_entity_t *entity);


/* ===== Function name:  uwh_audio_read_get_frame_size */
/**
  Returns the size of a given USB audio stream's next read frame available for 
  the application.

  This function is used for audio formats with non-fixed frame (i.e., Type II audio 
  data format types).

  @param *entity [IN] -- Pointer to a USB streaming terminal entity structure.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Frame size, in bytes.

  @scope
  Global.
*/
juint32_t uwh_audio_read_get_frame_size(uwh_audio_entity_t *entity);


/* ===== Function name:  uwh_audio_read_get_data */
/**
Reads data from the read buffer of a given USB audio stream.

  @param *entity  [IN] -- Pointer to a USB streaming terminal entity structure.
  @param *buffer [OUT] -- Handle to a pre-allocated memory buffer to be filled 
                          with the read data.
  @param size     [IN] -- Size of the provided data buffer, in bytes.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Zero (0) on success; otherwise, an error code.

  @scope
  Global.
 */
jresult_t uwh_audio_read_get_data(uwh_audio_entity_t *entity, 
    void *buffer, juint32_t size);


/* ===== Function name:  uwh_audio_stream_abort */
/**
  Cancels all transfers for the given USB audio stream and closes the stream.

  @param drv_ctx  [IN] -- Audio driver context.
  @param *entity  [IN] -- Pointer to a USB streaming terminal entity structure.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void uwh_audio_stream_abort(uwh_audio_drv_h drv_ctx, 
    uwh_audio_entity_t *entity);

/* ////////////////////
   Utility Functions 
 * /////////////////// */


/* ===== Function name:  uwh_audio_samp_freq_to_uint32 */
/**
  Converts the sampling frequency from a 3-byte format to Hz.

  @param *freq [IN] -- Sampling frequency expressed in a 3-byte format.

  @dependencies
  None.

  @sideeffects
  None.

  @return
  Frequency in units of Hz.

  @scope
  Global.
*/
juint32_t uwh_audio_samp_freq_to_uint32(uwh_audio_samp_freq_t *freq);


/* ===== Function name:  uwh_audio_uint32_to_samp_freq */
/**
  Converts the sampling frequency from Hz to a 3-byte format.
  
  @param freq_hz  [IN] -- Sampling frequency in Hz.
  @param *freq   [OUT] -- Pointer to a 3-byte format presentation of the given
                          sampling frequency (freq_hz).

  @dependencies
  None.

  @sideeffects
  None.

  @return
  None.

  @scope
  Global.
*/
void uwh_audio_uint32_to_samp_freq(juint32_t freq_hz, 
    uwh_audio_samp_freq_t *freq);

/** @} */ /* end_group hsusb_jungo_api */

#endif
