/*-------------------------------------------------------------------
Copyright (c) 2013-2016 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
--------------------------------------------------------------------*/

#include "venc/inc/omx_video_common.h"
#include "vtest_ComDef.h"
#include "vtest_Debug.h"
#include "vtest_Thread.h"
#include "vtest_SignalQueue.h"
#include "vtest_Sleeper.h"
#include "vtest_File.h"
#include "vtest_Time.h"
#include "vtest_Parser.h"
#include "vtest_DecoderFileSink.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <media/msm_media_info.h>

#undef LOG_TAG
#define LOG_TAG "VTEST_DECODER_FILE_SINK"

namespace vtest {

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DecoderFileSink::DecoderFileSink(Crypto *pCrypto)
    : ISource(),
      m_nFrames(0),
      m_nFramerate(0),
      m_nFrameWidth(0),
      m_nFrameHeight(0),
      m_pFile(NULL),
      m_nFileFd(-1),
      m_bSecureSession(OMX_FALSE),
      m_pCrypto(pCrypto),
      m_nColorFormat(0) {

    snprintf(m_pName, MAX_NAME_LENGTH, "%s", "DecoderFileSink");
    VTEST_MSG_LOW("%s: created...", Name());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DecoderFileSink::~DecoderFileSink() {

    if (m_pFile != NULL) {
        m_pFile->Close();
        delete m_pFile;
        m_pFile = NULL;
    }
    if (m_nFileFd != -1) {
        close(m_nFileFd);
        m_nFileFd = -1;
    }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
PortBufferCapability DecoderFileSink::GetBufferRequirements(OMX_U32 ePortIndex) {

    PortBufferCapability sBufCap;

    Mutex::Autolock autoLock(m_pLock);
    memset(&sBufCap, 0, sizeof(PortBufferCapability));

    if (ePortIndex == PORT_INDEX_IN) {
        sBufCap.bAllocateBuffer = OMX_FALSE;
        sBufCap.bUseBuffer = OMX_TRUE;
        sBufCap.pSource = this;
        sBufCap.ePortIndex = ePortIndex;
        sBufCap.nMinBufferSize = 0x1000;
        sBufCap.nMinBufferCount = 1;
        sBufCap.nBufferUsage = 0;
        sBufCap.nExtraBufferCount = 0;
    } else {
        VTEST_MSG_ERROR("Error: invalid port selection");
    }
    return sBufCap;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::Configure(CodecConfigType *pConfig,
        BufferManager *pBufManager, ISource *pSource, ISource *pSink) {

    OMX_ERRORTYPE result = OMX_ErrorNone;

    result = ISource::Configure(pConfig, pBufManager, pSource, pSink);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("DecoderFileSink configure failed");
        return result;
    }

    Mutex::Autolock autoLock(m_pLock);

    char *pFileName = pConfig->cOutFileName;
    OMX_S32 nTestNum = pConfig->nCurrentTestNum;
    m_bSecureSession = pConfig->bSecureSession;
    m_nColorFormat = pConfig->nOutputColorFormat;

    if (pConfig->nFramerate > 0 &&
        pConfig->nFrameWidth > 0 &&
        pConfig->nFrameHeight > 0) {

        m_nFramerate = pConfig->nFramerate;
        m_nFrameWidth = pConfig->nFrameWidth;
        m_nFrameHeight = pConfig->nFrameHeight;
        m_nFrames = pConfig->nFrames;

        if ((pFileName != NULL) &&
            (Parser::StringICompare((OMX_STRING)"", pFileName) != 0)) {
            (void)Parser::AppendTestNum(pFileName, nTestNum);

            m_pFile = new File();
            if (m_pFile != NULL) {
                VTEST_MSG_MEDIUM("Opening output file...");
                result = m_pFile->Open(pFileName, OMX_FALSE);
                if (result != OMX_ErrorNone) {
                    VTEST_MSG_ERROR("Failed to open file");
                }
            } else {
                VTEST_MSG_ERROR("Failed to allocate file");
                result = OMX_ErrorInsufficientResources;
            }
        } else {
            VTEST_MSG_MEDIUM("No output file");
        }
    } else {
        VTEST_MSG_ERROR("bad params in config");
        result = OMX_ErrorBadParameter;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::SetBuffer(
        BufferInfo *pBuffer, ISource *pSource) {

    OMX_ERRORTYPE result = OMX_ErrorNone;

    result = ISource::SetBuffer(pBuffer, pSource);
    if (result != OMX_ErrorNone) {
        return result;
    }

    VTEST_MSG_LOW("queue push (%p %p)", pBuffer->pHeaderIn, pBuffer->pHeaderOut);
    result = m_pBufferQueue->Push(&pBuffer, sizeof(BufferInfo**));
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::PortReconfig(OMX_U32 ePortIndex,
        OMX_U32 nWidth, OMX_U32 nHeight, const OMX_CONFIG_RECTTYPE& sRect) {

    (void)sRect;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    VTEST_MSG_LOW("PortReconfig port %s", OMX_PORT_NAME(ePortIndex));

    if (m_nFrameWidth != nWidth || m_nFrameHeight != nHeight) {
        VTEST_MSG_MEDIUM("PortReconfig WxH (%uX%u) ==> WxH (%uX%u)",
                (unsigned int)m_nFrameWidth, (unsigned int)m_nFrameHeight, (unsigned int)nWidth, (unsigned int)nHeight);
        m_nFrameWidth = nWidth;
        m_nFrameHeight = nHeight;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::ThreadRun(OMX_PTR pThreadData) {

    (void)pThreadData;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pHeader = NULL;
    BufferInfo *pBuffer = NULL;

    for (OMX_U32 i = 1; i <= m_nFrames && !m_bThreadStop; i++) {

        result = m_pBufferQueue->Pop(&pBuffer, sizeof(pBuffer), 0);
        VTEST_MSG_LOW("queue pop %u of %u (qsize %u)",
                (unsigned int)i, (unsigned int)m_nFrames, (unsigned int)m_pBufferQueue->GetSize());

        if ((pBuffer == NULL) || (result != OMX_ErrorNone)) {
            /* Can only happen if stop is called or someone else ran into an
             * error */
            VTEST_MSG_HIGH("Stopping thread");
            result = OMX_ErrorNone;
            continue;
        }

        pHeader = pBuffer->pHeaderIn;
        if (pHeader->nFlags & OMX_BUFFERFLAG_ENDOFFRAME) {
            VTEST_MSG_HIGH("frames written %u", (unsigned int)i);
        }

        if ((pHeader->nFlags & OMX_BUFFERFLAG_EOS) || (i >= m_nFrames)) {
            VTEST_MSG_HIGH("got EOS for frame : %u", (unsigned int)i);
            m_bThreadStop = OMX_TRUE;
        }

        if (m_pFile != NULL) {

            if (pHeader->nFilledLen > 0) {
                VTEST_MSG_MEDIUM("writing frame %u with %u bytes...",
                        (unsigned int)i, (unsigned int)pHeader->nFilledLen);


                OMX_S32 nBytes;

                if (m_bSecureSession) {

                    /* This non-sense is done for secure decoder down scalar
                     * feature where beyond a certain threshold, buffer can be
                     * taken out of CPZ by venus */
                    OMX_U8 *data = (OMX_U8 *)mmap(NULL, pHeader->nFilledLen,
                            PROT_READ | PROT_WRITE, MAP_SHARED, (unsigned long)pHeader->pBuffer, 0);
                    if (data == MAP_FAILED) {
                        data = NULL;
                        VTEST_MSG_HIGH("Mapping failed - Assume secure buffer!");
                        if (m_pCrypto != NULL) {
                            data = new OMX_U8[pHeader->nAllocLen];
                            result = m_pCrypto->Copy(SAMPLECLIENT_COPY_SECURE_TO_NONSECURE,
                                    data, (unsigned long)pHeader->pBuffer, pHeader->nFilledLen);
                        } else {
                            VTEST_MSG_ERROR("Crypto object null");
                            result = OMX_ErrorBadParameter;
                        }

                        if (result != OMX_ErrorNone) {
                            VTEST_MSG_ERROR("OEMCrypto_Copy failed, result is %d", result);
                        } else {
                            result = WriteToFile(data, pHeader->nFilledLen, nBytes);
                        }

                        if (data) {
                            delete[] data;
                            data = NULL;
                        }
                    } else {
                        VTEST_MSG_HIGH("Mapping passed! - Assume buffer out of CPZ");
                        result = WriteToFile(data, pHeader->nFilledLen, nBytes);
                        if (munmap(data, pHeader->nFilledLen)) {
                            VTEST_MSG_ERROR("munmap failed for data : %p", data);
                        }
                        data = NULL;
                    }
                } else {
                    result = WriteToFile(
                            (OMX_U8 *)pHeader->pBuffer, pHeader->nFilledLen, nBytes);
                }

                if (result != OMX_ErrorNone) {
                    VTEST_MSG_ERROR("Error writing to file...");
                    SetError();
                } else if ((OMX_U32)nBytes > pHeader->nFilledLen) {
                    VTEST_MSG_ERROR(
                            "Error mismatched number of bytes %d vs expected : %d in file write",
                            nBytes, (OMX_S32)pHeader->nFilledLen);
                    SetError();
                    result = OMX_ErrorUndefined;
                }
            } else {
                VTEST_MSG_HIGH("skipping frame %u, 0 length...", (unsigned int)i);
            }
        } else {
            VTEST_MSG_MEDIUM("received frame %u... but did not write as no file present", (unsigned int)i);
        }

        m_pSource->SetBuffer(pBuffer, this);
    }

    //clean up
    while(m_pBufferQueue->GetSize() > 0) {
        VTEST_MSG_LOW("cleanup: q-wait (qsize %u)", (unsigned int)m_pBufferQueue->GetSize());
        m_pBufferQueue->Pop(&pBuffer, sizeof(BufferInfo **), 0);
        m_pSource->SetBuffer(pBuffer, this);
    }

    VTEST_MSG_HIGH("thread exiting...");
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE DecoderFileSink::WriteToFile(
        OMX_U8 *pData, OMX_U32 nFilledLen, OMX_S32 &nTotalBytes) {

    OMX_U32 i = 0;
    OMX_S32 nBytes = 0;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    nTotalBytes = 0;
    OMX_U8 *pBuffer = pData;

    if (m_nColorFormat == QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m) {
        OMX_U32 nStride = VENUS_Y_STRIDE(COLOR_FMT_NV12, m_nFrameWidth);
        OMX_U32 nScanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12, m_nFrameHeight);

        for (i = 0; i < m_nFrameHeight; i++) {
            result = m_pFile->Write(pData, m_nFrameWidth, &nBytes);
            pData += nStride;
            nTotalBytes += nBytes;
            FAILED1(result, "Failed to write to file");
        }

        pData = pBuffer + nStride * nScanlines;
        for (i = 0; i < m_nFrameHeight/2; i++) {
            result = m_pFile->Write(pData, m_nFrameWidth, &nBytes);
            pData += nStride;
            nTotalBytes += nBytes;
            FAILED1(result, "Failed to write to file");
        }
    } else {
        result = m_pFile->Write(pData, nFilledLen, &nTotalBytes);
    }
    return result;
}

} // namespace vtest
