/*-------------------------------------------------------------------
Copyright (c) 2013-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
--------------------------------------------------------------------*/

#ifndef _VTEST_COMDEF_H
#define _VTEST_COMDEF_H

#include "OMX_Core.h"
#include "OMX_Video.h"
#include "OMX_QCOMExtns.h"
#include "OMX_VideoExt.h"
#include "OMX_Skype_VideoExtensions.h"
#include <msm_media_info.h>

#define MAX_NAME_LENGTH 256

namespace vtest {

#define SZ_4K 0x1000
#define SZ_1M 0x100000
#define VTEST_ALIGN( num, to ) (((num) + (to-1)) & (~(to-1)))

#define OMX_SPEC_VERSION 0x00000101
#define OMX_INIT_STRUCT(_s_, _name_)            \
   memset((_s_), 0x0, sizeof(_name_));          \
   (_s_)->nSize = sizeof(_name_);               \
   (_s_)->nVersion.nVersion = OMX_SPEC_VERSION

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MAXHPLAYERS 8

// @brief Maximum length for a file name
static const OMX_S32 VTEST_MAX_STRING = 128;
static const OMX_U32 PORT_INDEX_IN  = 0;
static const OMX_U32 PORT_INDEX_OUT = 1;

// @brief Resync marker types
struct ConfigEnum {
    const OMX_STRING pEnumName;
    OMX_S32 eEnumVal;
};

/*File Type options
 *
 *    1--> PER ACCESS UNIT CLIP (.dat). Clip only available for H264 and Mpeg4
 2--> ARBITRARY BYTES (need .264/.264c/.m4v/.263/.rcv/.vc1/.m2v
 3--> NAL LENGTH SIZE CLIP (.264c)
 3--> MP4 VOP or H263 P0 SHORT HEADER START CODE CLIP (.m4v or .263)
 3--> VC1 clip Simple/Main Profile (.rcv)
 3--> DivX 4, 5, 6 clip (.cmp)
 3--> MPEG2 START CODE CLIP (.m2v)
 4--> VC1 clip Advance Profile (.vc1)
 4--> DivX 3.11 clip
 */
enum FileType {
    FILE_TYPE_DAT_PER_AU = 1,
    FILE_TYPE_ARBITRARY_BYTES,
    FILE_TYPE_COMMON_CODEC_MAX,

    FILE_TYPE_START_OF_H264_SPECIFIC = 10,
    FILE_TYPE_264_NAL_SIZE_LENGTH = FILE_TYPE_START_OF_H264_SPECIFIC,
    FILE_TYPE_264_START_CODE,

    FILE_TYPE_START_OF_MP4_SPECIFIC = 20,
    FILE_TYPE_PICTURE_START_CODE = FILE_TYPE_START_OF_MP4_SPECIFIC,

    FILE_TYPE_START_OF_VC1_SPECIFIC = 30,
    FILE_TYPE_RCV = FILE_TYPE_START_OF_VC1_SPECIFIC,
    FILE_TYPE_VC1,

    FILE_TYPE_START_OF_DIVX_SPECIFIC = 40,
    FILE_TYPE_DIVX_4_5_6 = FILE_TYPE_START_OF_DIVX_SPECIFIC,
    FILE_TYPE_DIVX_311,

    FILE_TYPE_START_OF_MPEG2_SPECIFIC = 50,
    FILE_TYPE_MPEG2_START_CODE = FILE_TYPE_START_OF_MPEG2_SPECIFIC,

    FILE_TYPE_START_OF_VP8_SPECIFIC = 60,
    FILE_TYPE_VP8_START_CODE = FILE_TYPE_START_OF_VP8_SPECIFIC,
    FILE_TYPE_VP8
};

enum ResyncMarkerType {
    RESYNC_MARKER_NONE,  // No resync marker

    RESYNC_MARKER_BITS,  // Resync marker for MPEG4/H.264/H.263
    RESYNC_MARKER_MB,    // MB resync marker for MPEG4/H.264/H.263
    RESYNC_MARKER_GOB    //GOB resync marker for H.263
};

enum CodecProfileType {
    MPEG4ProfileSimple,
    MPEG4ProfileAdvancedSimple,
    H263ProfileBaseline,
    AVCProfileBaseline,
    AVCProfileHigh,
    AVCProfileMain,
    VP8ProfileMain,
    HEVCProfileMain,
    HEVCProfileMain10,
};

enum CodecLevelType {
    DefaultLevel,
    VP8Version0,
    VP8Version1,
};

enum PlaybackModeType {
    DynamicPortReconfig,
    AdaptiveSmoothStreaming,
    QCSmoothStreaming,
    DynamicBufferMode,
};

enum SinkType {
    NativeWindow_Sink,
    MDPOverlay_Sink,
    File_Sink,
};

enum MetaBufferType {
    CameraSource,
    GrallocSource,
};

enum YuvColorSpace {
    ITUR601 = 0x200000,
    ITUR601FR = 0x400000,
    ITUR709 = 0x800000,
};

enum PostProcType {
    DefaultMemcopy,
    C2dColorConversion,
    MmColorConversion,
    GpuPostProcessing,
};

// @brief Encoder configuration
struct CodecConfigType {
    ////////////////////////////////////////
    //======== Common static config
    FileType eFileType;                     //Config File Key: FileType
    OMX_VIDEO_CODINGTYPE eCodec;                //Config File Key: Codec
    CodecProfileType eCodecProfile;             //Config File Key: Profile
    CodecLevelType eCodecLevel;                 //Config File Key: Level
    OMX_VIDEO_CONTROLRATETYPE eControlRate;     //Config File Key: RC
    OMX_S32 nResyncMarkerSpacing;
    //Config File Key: ResyncMarkerSpacing
    ResyncMarkerType eResyncMarkerType;
    //Config File Key: ResyncMarkerType
    OMX_S32 nIntraRefreshMBCount;
    //Config File Key: IntraRefreshMBCount
    OMX_S32 nFrameWidth;
    //Config File Key: FrameWidth
    OMX_S32 nFrameHeight;
    //Config File Key: FrameHeight
    OMX_S32 nOutputFrameWidth;
    //Config File Key: OutputFrameWidth
    OMX_S32 nOutputFrameHeight;
    //Config File Key: OutputFrameHeight
    OMX_S32 nDVSXOffset;
    //Config File Key: DVSXOffset
    OMX_S32 nDVSYOffset;
    //Config File Key: DVSYOffset
    OMX_S32 nBitrate;                           //Config File Key: Bitrate
    OMX_S32 nFramerate;                         //Config File Key: FPS
    OMX_S32 nRotation;
    //Config File Key: Rotation
    OMX_S32 nInBufferCount;
    //Config File Key: InBufferCount
    OMX_S32 nOutBufferCount;
    //Config File Key: OutBufferCount
    char cInFileName[VTEST_MAX_STRING];     //Config File Key: InFile
    char cOutFileName[VTEST_MAX_STRING];    //Config File Key: OutFile
    OMX_S32 nFrames;
    //Config File Key: NumFrames
    OMX_S32 nIntraPeriod;
    //Config File Key: IntraPeriod
    OMX_S32 nMinQp;                             //Config File Key: MinQp
    OMX_S32 nMaxQp;                             //Config File Key: MaxQp
    OMX_BOOL bProfileMode;
    //Config File Key: ProfileMode
    OMX_BOOL bExtradata;
    //Config file Key: Extradata
    OMX_S32 nIDRPeriod;
    //Config file Key: IDR Period
    OMX_BOOL bSecureSession;
    // Configure File Key: SecureMode

    ////////////////////////////////////////
    //======== encoder specific fields
    OMX_BOOL bMdpFrameSource;
    // Configure File Key: MDPFrameSource

    ////////////////////////////////////////
    //======== encoder Mpeg4 static config
    OMX_S32 nHECInterval;
    //Config File Key: HECInterval
    OMX_S32 nTimeIncRes;
    //Config File Key: TimeIncRes
    OMX_BOOL bEnableShortHeader;
    //Config File Key: EnableShortHeader

    ////////////////////////////////////////
    //======== encoder H.263 static config

    ////////////////////////////////////////
    //======== encoder H.264 static config
    OMX_BOOL bCABAC;                          //Config File Key: CABAC
    OMX_S32 nDeblocker;                       //Config File Key: Deblock
    OMX_U32 id;                               //Config File Key: FramePackId
    OMX_U32 cancel_flag;
    //Config File Key: FramePackCancelFlag
    OMX_U32 type;
    //Config File Key: FramePackType
    OMX_U32 quincunx_sampling_flag;
    //Config File Key: FramePackQuincunxSamplingFlag
    OMX_U32 content_interpretation_type;
    //Config File Key: FramePackContentInterpretationType
    OMX_U32 spatial_flipping_flag;
    //Config File Key: FramePackSpatialFlippingFlag
    OMX_U32 frame0_flipped_flag;
    //Config File Key: FramePackFrame0FlippedFlag
    OMX_U32 field_views_flag;
    //Config File Key: FramePackFieldViewsFlag
    OMX_U32 current_frame_is_frame0_flag;
    //Config File Key: FramePackCurrentFrameIsFrame0Flag
    OMX_U32 frame0_self_contained_flag;
    //Config File Key: FramePackFrame0SelfContainedFlag
    OMX_U32 frame1_self_contained_flag;
    //Config File Key: FramePackFrame1SelfContainedFlag
    OMX_U32 frame0_grid_position_x;
    //Config File Key: FramePackFrame0GridPositionX
    OMX_U32 frame0_grid_position_y;
    //Config File Key: FramePackFrame0GridPositionY
    OMX_U32 frame1_grid_position_x;
    //Config File Key: FramePackFrame1GridPositionX
    OMX_U32 frame1_grid_position_y;
    //Config File Key: FramePackFrame1GridPositionY
    OMX_U32 reserved_byte;
    //Config File Key: FramePackReservedByte
    OMX_U32 repetition_period;
    //Config File Key: FramePackRepetitionPeriod
    OMX_U32 extension_flag;
    //Config File Key: FramePackExtensionFlag
    OMX_BOOL bInsertInbandVideoHeaders;
    //Config File Key: InsertInbandVideoHeaders
    OMX_BOOL bInsertAUDelimiters;
    //Config File Key: InsertAUDelimiters

    //======== LTR encoding config
    OMX_S32 nLTRMode;                           //Config File Key: LTR mode
    OMX_S32 nLTRCount;                          //Config File Key: LTR count
    OMX_S32 nLTRPeriod;                         //Config File Key: LTR period

    ///////////////////////////////////////////
    //======== PlaybackMode
    PlaybackModeType ePlaybackMode;             // Config File Key: PlaybackMode
    OMX_U32 nAdaptiveWidth;                     // Config File Key: AdaptiveWidth
    OMX_U32 nAdaptiveHeight;                    // Config File Key: AdaptiveHeight

    //======== RotateDisplay
    OMX_BOOL bRotateDisplay;                       // Config File Key: RotateDisplay

    //======== DecoderPictureOrder
    QOMX_VIDEO_PICTURE_ORDER eDecoderPictureOrder; // Config File Key: DecoderPictureOrder

    //======== SinkType
    SinkType eSinkType;                            // Config File Key: SinkType

    //======== MetaMode
    OMX_BOOL bMetaMode;                            // Config File Key: MetaMode

    //======== MetaBufferType
    MetaBufferType eMetaBufferType;                // No Config File Key needed

    //======== YuvColorSpace
    YuvColorSpace eYuvColorSpace;                  // Config File Key: YuvColorSpace & Clamp_601to709(legacy)

    //======== DecoderDownScalar
    OMX_BOOL bDecoderDownScalar;                   // Config File Key: DecoderDownScalar

    //======== HierPNumLayers
    OMX_U32 nHierPNumLayers;                       // Config File Key: HierPNumLayers

    //======== Performance Mode
    OMX_U32 nPerfMode;                             // Config File Key: PerfMode

    //======== PostProcType
    PostProcType ePostProcType;                    // Config File Key: PostProcType

    //======== InputColorFormat
    OMX_U32 nInputColorFormat;                     // Config File Key: InputColorFormat

    //======== OutputColorFormat
    OMX_U32 nOutputColorFormat;                    // Config File Key: OutputColorFormat

    //======== DropHierPLayers
    OMX_S32 nDropHierPLayer;                       // Config File Key: DropHierPLayer

    //======== Deinterlace
    OMX_BOOL bDeinterlace;                         // Config File Key: Deinterlace

    //======== PictureTypeDecode
    OMX_U32 nPictureTypeDecode;                    // Config File Key: PictureType

    //======== Max Hierarchial P Layers
    OMX_U32 nMaxHPLayers;                         // Config File Key: MaxHPLayers

    //======== Rectangle information for cropping
    OMX_U32 nRectLeft;                            // Config File Key: RectLeft
    OMX_U32 nRectTop;                             // Config File Key: RectTop
    OMX_U32 nRectWidth;                           // Config File Key: RectWidth
    OMX_U32 nRectHeight;                          // Config File Key: RectHeight

    //======== Base Layer Id
    OMX_U32 nBaseLayerId;                         // Config File Key: BaseLayerID

    //======== Config QP
    OMX_U32 nConfigQp;                            // Config File Key: ConfigQP

    //======== Sar Width
    OMX_U64 nSarWidth;                            // Config File Key: SarWidth

    //======== Sar Height
    OMX_U32 nSarHeight;                            // Config File Key: SarHeight

    //======== nCurrentTestNum
    /* It is ok to store this in config because for the case of when running
     * a script, all test cases are run sequentially, so this number will
     * keep getting overwritten.
     */
    OMX_S32 nCurrentTestNum;

    //======== bForceCompressedForDPB
    /* To enable force split mode with DPB(compressed)/OPB(Linear) when
     * client requests Linear output. The default mode used would be combined Linear but
     * this will force to use split mode with DPB compressed and OPB Linear. This would
     * give power benefit at the cost of increased memory.For example on 8996 default mode
     * for resolutions <=1080 for Linear output is combined DPB/OPB Linear, but enabling
     * this parameter would force split mode.
     */
    OMX_BOOL bForceCompressedForDPB;

    //======== MBIStatisticsmode
    OMX_QOMX_VIDEO_MBISTATISTICSTYPE eMBIStatisticsMode;       // Config File Key: MBIStatisticsMode

    //======== nHybridHpLayers
    QOMX_EXTNINDEX_VIDEO_HYBRID_HP_MODE sHybridHP;  // Config File Key: HybridHpLayers

    char cDynamicFile[VTEST_MAX_STRING];
    struct DynamicConfig *pDynamicProperties;
    OMX_U32 dynamic_config_array_size;
};

// @brief Dynamic configuration

struct DynamicConfigType {
    OMX_S32 nIFrameRequestPeriod;    //Config File Key: IFrameRequestPeriod
    OMX_S32 nUpdatedFramerate;       //Config File Key: UpdatedFPS
    OMX_S32 nUpdatedBitrate;         //Config File Key: UpdatedBitrate
    OMX_S32 nUpdatedFrames;          //Config File Key: UpdatedNumFrames
    OMX_S32 nUpdatedIntraPeriod;     //Config File Key: UpdatedIntraPeriod
};

union DynamicConfigData {
    OMX_VIDEO_CONFIG_BITRATETYPE bitrate;
    OMX_CONFIG_FRAMERATETYPE framerate;
    QOMX_VIDEO_INTRAPERIODTYPE intraperiod;
    OMX_CONFIG_INTRAREFRESHVOPTYPE intravoprefresh;
    OMX_CONFIG_ROTATIONTYPE rotation;
    OMX_VIDEO_VP8REFERENCEFRAMETYPE vp8refframe;
    OMX_QCOM_VIDEO_CONFIG_LTRMARK_TYPE markltr;
    OMX_QCOM_VIDEO_CONFIG_LTRUSE_TYPE useltr;
    QOMX_EXTNINDEX_VIDEO_PERFMODE perfmode;
    OMX_VIDEO_CONFIG_DEINTERLACE deinterlace;
    OMX_QCOM_VIDEO_CONFIG_PICTURE_TYPE_DECODE pictypedecode;
    OMX_SKYPE_VIDEO_CONFIG_QP configqp;
    float f_framerate;
};

struct DynamicConfig {
    unsigned frame_num;
    OMX_INDEXTYPE config_param;
    union DynamicConfigData config_data;
};

// @brief Test description structure

struct TestDescriptionType {
    char cTestName[VTEST_MAX_STRING];     //The name of the test
    char cConfigFile[VTEST_MAX_STRING];   //Encoder config file name
    OMX_S32 nSession;                     //Number of sessions to run
};

struct NativeHandle {
    OMX_S32 version;        /* sizeof(native_handle_t) */
    OMX_S32 numFds;         /* number of file-descriptors at &data[0] */
    OMX_S32 numInts;        /* number of ints at &data[numFds] */
    OMX_S32 data[0];        /* numFds + numInts ints */
};
typedef NativeHandle* BufferHandle;

struct MetaBuffer {
    MetaBufferType buffer_type;
    BufferHandle meta_handle;
};

struct BufferInfo {
    unsigned long pHandle;
    OMX_BUFFERHEADERTYPE *pHeaderIn;
    OMX_BUFFERHEADERTYPE *pHeaderOut;

    OMX_BUFFERHEADERTYPE** GetHeaderPtr(OMX_U32 ePort) {
        return (ePort == PORT_INDEX_IN ? &pHeaderIn : &pHeaderOut);
    };
};

static inline OMX_U32 GetFrameSize(OMX_U32 nFormat,
        OMX_S32 nWidth, OMX_S32 nHeight) {
    OMX_U32 adjust = VENUS_EXTRADATA_SIZE(nWidth, nHeight);
    switch (nFormat) {
        case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m:
            return nWidth * nHeight * 3 / 2;
    /* In addition to extradata size of 8K, another extra 8k bytes is included by
     * Venus firmware when reporting output buffer size requirements for NV12 UBWC.
     * This shouldn't be accounted for while reading the frame data. Hence adjusting
     * the frame size for NV12 Linear UBWC
     */
        case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed:
            return VENUS_BUFFER_SIZE(COLOR_FMT_NV12_UBWC, nWidth, nHeight) - 2 * adjust;
        case QOMX_COLOR_Format32bitRGBA8888:
            return nWidth * 4 * nHeight;
        case QOMX_COLOR_Format32bitRGBA8888Compressed:
            return VENUS_BUFFER_SIZE(COLOR_FMT_RGBA8888_UBWC, nWidth, nHeight);
        default:
            return 0x1000;
    }
}

}

#endif // #ifndef _VTEST_COMDEF_H
