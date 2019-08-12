/******************************************************************************
 *
 *  Copyright (c) 2016 Qualcomm Technologies International, Ltd.
 *  All Rights Reserved.
 *  Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 *
 ******************************************************************************/

#ifndef __QTI_APTX_ENCODER__
#define __QTI_APTX_ENCODER__
/*****************************************************************************
**  Type Definitions
*****************************************************************************/
/* data type for the aptX Codec Information Element*/
#define NON_A2DP_MEDIA_CT 0xff
#define APTX_VENDOR_ID    0
#define APTX_CODEC_ID     1
#define APTX_SAMPLE_RATE  2
#define APTX_CHANNEL      3
#define APTX_FUTURE1      4
#define APTX_FUTURE2      5

typedef short SINT16;
typedef long SINT32;
typedef UINT16 CsrCodecType;

/* aptX codec specific settings*/
#define BTA_AV_CO_APTX_CODEC_LEN          9

#define QTI_APTX_VENDOR_ID             (0x0000004F)
#define QTI_APTX_CODEC_ID_BLUETOOTH    ((CsrCodecType) 0x0001)
#define QTI_APTX_SAMPLERATE_44100       (0x20)
#define QTI_APTX_SAMPLERATE_48000       (0x10)
#define QTI_APTX_CHANNELS_STEREO        (0x02)
#define QTI_APTX_CHANNELS_MONO          (0x01)
#define QTI_APTX_FUTURE_1        (0x00)
#define QTI_APTX_FUTURE_2        (0x00)
#define QTI_APTX_OTHER_FEATURES_NONE  (0x00000000)
#define QTI_AV_APTX_AUDIO        (0x00)
#define QTI_APTX_CHANNEL         (0x0001)
#define QTI_APTX_SAMPLERATE      (0x22)

typedef struct
{
    UINT32 vendorId;
    UINT16 codecId;         /* Codec ID for aptX */
    UINT8   sampleRate;     /* Sampling Frequency */
    UINT8   channelMode;    /* STEREO/DUAL/MONO */
    UINT8   future1;
    UINT8   future2;
} tA2D_APTX_CIE;


typedef struct  {
    SINT16 s16SamplingFreq;  /* 16k, 32k, 44.1k or 48k*/
    SINT16 s16ChannelMode;   /* mono, dual, streo or joint streo*/
    UINT16 u16BitRate;
    UINT16 *ps16NextPcmBuffer;
    UINT8  *pu8Packet;
    UINT8  *pu8NextPacket;
    UINT16 u16PacketLength;
    void* encoder;
}APTX_ENC_PARAMS;

UINT8 A2D_BldAptxInfo(UINT8 media_type, tA2D_APTX_CIE *p_ie, UINT8 *p_result);
UINT8 A2D_ParsAptxInfo(tA2D_APTX_CIE *p_ie, UINT8 *p_info, BOOLEAN for_caps);
UINT8 bta_av_aptx_cfg_in_cap(UINT8 *p_cfg, tA2D_APTX_CIE *p_cap);
#endif
