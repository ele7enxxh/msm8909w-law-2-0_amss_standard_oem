/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2009, The Linux Foundation. All rights reserved.
--------------------------------------------------------------------------*/
/*============================================================================
                            O p e n M A X   w r a p p e r s
                O p e n  M A X   C o m p o n e n t  I n t e r f a c e

*//** @file vpp_buffer_manager.h
  This module contains the abstract interface for the OpenMAX components.

*//*========================================================================*/

#ifndef VPP_BUFFER_MANAGER_H
#define VPP_BUFFER_MANAGER_H
//////////////////////////////////////////////////////////////////////////////
//                             Include Files
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utils/Mutex.h>
#include <linux/videodev2.h>
#include <linux/msm_vidc_dec.h>

extern "C" {
#include <utils/Log.h>
#include "vpp.h"
OMX_API void * get_omx_component_factory_fn(void); //used by qc_omx_core
}

#undef LOG_TAG
#define LOG_TAG "OMX_VPP"

using namespace android;

class VppBufferManager
{

public:

    enum OmxBufferOwner {
       OMX_CLIENT = 0x01,
       OMX_VDEC = 0x02,
       VPP_IN = 0x03,
       VPP_OUT = 0x04,
       UNKNOWN
    };

private:

    struct OmxVppBuffer {
        OmxBufferOwner owner;
        OMX_BUFFERHEADERTYPE* omxHeader;
        struct vpp_buffer vppBuffer;
        //fd handle to be passed to VPP is stored in handle
        //for static buffer mode
        OMX_U8* handle;

        OmxVppBuffer() : owner(UNKNOWN),
                           omxHeader(NULL),
                           handle(NULL)
        {
        }
    };

    // Map of omx_buffers to their owners
    Vector<OmxVppBuffer> mBuffers;
    Mutex       mVppBufferLock;

    // indicates if the vpp component is in bypass
    bool                          mInBypass;
    bool                          mDisableDynamicBufMode;


    // Criterion for determining whether buffer requirements are adequate or not
    uint32_t                      mOmxMinBufferCount;
    uint32_t                      mOmxBufferCount;
    uint32_t                      mOmxVppTotalBufferCount;
    uint32_t                      mOmxFtbCount;
    uint32_t                      mVppFtbCount;

    // Factored out ratio.
    uint32_t                      mVdecFtbRatio;
    uint32_t                      mVppFtbRatio;

    // Get omx_buffer
    OmxVppBuffer& GetOmxVppBuffer(OMX_BUFFERHEADERTYPE *buffer);
    OmxBufferOwner GetFtbDestination(OmxVppBuffer& omxBuffer,unsigned vdecCount,
                                       unsigned vdecPlusVppCount);
    void GetBufferCounts(unsigned& vdecCount,unsigned& vppInCount,unsigned& vppOutCount,unsigned& clientCount);
public:
    VppBufferManager();
    ~VppBufferManager();

    // returns true if the requirements needs to be updated
    // false if it was already updated
    bool UpdateBufferRequirements(struct vpp_requirements&, OMX_PARAM_PORTDEFINITIONTYPE*);

    // Maintains an internal buffer wrapper for the omx buffer from client
    bool RegisterClientBuffer(OMX_BUFFERHEADERTYPE*, OMX_U8*);

    // Maintains an internal buffer wrapper for the omx buffer from client
    bool DeRegisterClientBuffer(OMX_BUFFERHEADERTYPE*);

    // To update Buffer ownership
    bool UpdateBufferOwnership(OMX_BUFFERHEADERTYPE*,OmxBufferOwner);

    // Return ftb destination, returns the vpp_buffer_pointer if needed
    OmxBufferOwner GetFtbDestination(OMX_BUFFERHEADERTYPE*,
                                        struct vpp_buffer **,bool=false);

    // Returns Vpp Buffer after updating buffer ownership.
    struct vpp_buffer* GetBufferForVppIn(OMX_BUFFERHEADERTYPE*, bool);

    // Get buffer statistics.
    void PrintBufferStatistics();

    // Check if in bypass, (call only from outputbuffer done)
    bool HandleBypass(OMX_BUFFERHEADERTYPE *);

    // Clear Vector
    void ClearBuffers();

    // Returns 1 if dynamic buffer mode is disabled
    bool GetBufferMode();

    // Set 1 to disable dynamic buffer mode
    void SetBufferMode(bool);

    void UpdateBufferGeometry(OMX_BUFFERHEADERTYPE *buffer,
                                        struct vpp_port_param *vppParams);

    void ConvertOmxVppFlags(uint32_t inflags, uint32_t& outflags,bool fromOmxToVpp);

    void UpdateMetaData(OMX_BUFFERHEADERTYPE *buffer, uint32_t colorFormat);

};

#endif /* VPP_BUFFER_MANAGER_H */
