/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2009, The Linux Foundation. All rights reserved.
--------------------------------------------------------------------------*/
/*============================================================================
                            O p e n M A X   w r a p p e r s
                O p e n  M A X   C o m p o n e n t  I n t e r f a c e

*//** @file vpp_omx_component.h
  This module contains the abstract interface for the OpenMAX components.

*//*========================================================================*/

#ifndef VPP_OMX_COMPONENT_H
#define VPP_OMX_COMPONENT_H
//////////////////////////////////////////////////////////////////////////////
//                             Include Files
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <utils/Vector.h>
#include <utils/Mutex.h>
#include <cutils/properties.h>
#ifndef PC_DEBUG
#include <semaphore.h>
#endif

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "qc_omx_component.h"
#include "vpp_buffer_manager.h"

extern "C" {
#include <utils/Log.h>
#include "vpp.h"
OMX_API void * get_omx_component_factory_fn(void); //used by qc_omx_core
}

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "OMX_VPP"
#define ROLE_MAX_SIZE 256

typedef void * (*create_qc_omx_component)(void);

using namespace android;
using OmxBufferOwner = VppBufferManager::OmxBufferOwner;

class VppOmxComponent : public qc_omx_component
{
private:

    friend class VppBufferManager;

    enum port_indexes {
       OMX_CORE_INPUT_PORT_INDEX        =0,
       OMX_CORE_OUTPUT_PORT_INDEX       =1
    };

public:

    VppOmxComponent();
    // this is critical, otherwise, sub class destructor will not be called
    virtual ~VppOmxComponent();

    // Initialize the component after creation
    virtual OMX_ERRORTYPE component_init(OMX_IN OMX_STRING componentName);

    /*******************************************************************/
    /*           Standard OpenMAX Methods                              */
    /*******************************************************************/

    // Query the component for its information
    virtual
    OMX_ERRORTYPE  get_component_version(OMX_HANDLETYPE       cmp_handle,
                                       OMX_STRING             cmp_name,
                                       OMX_VERSIONTYPE*    cmp_version,
                                       OMX_VERSIONTYPE*   spec_version,
                                       OMX_UUIDTYPE*          cmp_UUID);

    // Invoke a command on the component
    virtual
    OMX_ERRORTYPE  send_command(OMX_HANDLETYPE cmp_handle,
                              OMX_COMMANDTYPE       cmd,
                              OMX_U32            param1,
                              OMX_PTR          cmd_data);

    // Get a Parameter setting from the component
    virtual
    OMX_ERRORTYPE  get_parameter(OMX_HANDLETYPE     cmp_handle,
                               OMX_INDEXTYPE     param_index,
                               OMX_PTR            param_data);

    // Send a parameter structure to the component
    virtual
    OMX_ERRORTYPE  set_parameter(OMX_HANDLETYPE     cmp_handle,
                               OMX_INDEXTYPE     param_index,
                               OMX_PTR            param_data);

    // Get a configuration structure from the component
    virtual
    OMX_ERRORTYPE  get_config(OMX_HANDLETYPE      cmp_handle,
                            OMX_INDEXTYPE     config_index,
                            OMX_PTR            config_data);

    // Set a component configuration value
    virtual
    OMX_ERRORTYPE  set_config(OMX_HANDLETYPE      cmp_handle,
                            OMX_INDEXTYPE     config_index,
                            OMX_PTR            config_data);

    // Translate the vendor specific extension string to
    // standardized index type
    virtual
    OMX_ERRORTYPE  get_extension_index(OMX_HANDLETYPE  cmp_handle,
                                     OMX_STRING       paramName,
                                     OMX_INDEXTYPE*   indexType);

    // Get Current state information
    virtual
    OMX_ERRORTYPE  get_state(OMX_HANDLETYPE  cmp_handle,
                           OMX_STATETYPE*       state);

    // Component Tunnel Request
    virtual
    OMX_ERRORTYPE  component_tunnel_request(OMX_HANDLETYPE           cmp_handle,
                                          OMX_U32                        port,
                                          OMX_HANDLETYPE       peer_component,
                                          OMX_U32                   peer_port,
                                          OMX_TUNNELSETUPTYPE*   tunnel_setup);

    // Use a buffer already allocated by the IL client
    // or a buffer already supplied by a tunneled component
    virtual
    OMX_ERRORTYPE  use_buffer(OMX_HANDLETYPE                cmp_handle,
                            OMX_BUFFERHEADERTYPE**        buffer_hdr,
                            OMX_U32                             port,
                            OMX_PTR                         app_data,
                            OMX_U32                            bytes,
                            OMX_U8*                           buffer);


    // Request that the component allocate new buffer and associated header
    virtual
    OMX_ERRORTYPE  allocate_buffer(OMX_HANDLETYPE                cmp_handle,
                                 OMX_BUFFERHEADERTYPE**        buffer_hdr,
                                 OMX_U32                             port,
                                 OMX_PTR                         app_data,
                                 OMX_U32                            bytes);

    // Release the buffer and associated header from the component
    virtual
    OMX_ERRORTYPE  free_buffer(OMX_HANDLETYPE         cmp_handle,
                             OMX_U32                      port,
                             OMX_BUFFERHEADERTYPE*      buffer);

    // Send a filled buffer to an input port of a component
    virtual
    OMX_ERRORTYPE  empty_this_buffer(OMX_HANDLETYPE         cmp_handle,
                                   OMX_BUFFERHEADERTYPE*      buffer);

    // Send an empty buffer to an output port of a component
    virtual
    OMX_ERRORTYPE  fill_this_buffer(OMX_HANDLETYPE         cmp_handle,
                                  OMX_BUFFERHEADERTYPE*      buffer);

    // Set callbacks
    virtual
    OMX_ERRORTYPE  set_callbacks( OMX_HANDLETYPE        cmp_handle,
                                OMX_CALLBACKTYPE*      callbacks,
                                OMX_PTR                 app_data);

    // Component De-Initialize
    virtual
    OMX_ERRORTYPE  component_deinit(OMX_HANDLETYPE cmp_handle);

    // Use the Image already allocated via EGL
    virtual
    OMX_ERRORTYPE  use_EGL_image(OMX_HANDLETYPE                cmp_handle,
                               OMX_BUFFERHEADERTYPE**        buffer_hdr,
                               OMX_U32                             port,
                               OMX_PTR                         app_data,
                               void*                          egl_image);

    // Component Role enum
    virtual
    OMX_ERRORTYPE  component_role_enum( OMX_HANDLETYPE cmp_handle,
                                      OMX_U8*              role,
                                      OMX_U32             index);

private:

    // To load omx code so and create component
    OMX_ERRORTYPE loadOmxDecoder();

    // callbacks from component
    static OMX_CALLBACKTYPE   sOmxCallbacks;

    static OMX_ERRORTYPE OmxEventHandler(OMX_HANDLETYPE pHandle,
                                      OMX_PTR pAppData,
                                      OMX_EVENTTYPE eEvent,
                                      OMX_U32 nData1,
                                      OMX_U32 nData2,
                                      OMX_PTR pEventData);

    static OMX_ERRORTYPE OmxEmptyBufferDone(OMX_HANDLETYPE hComponent,
                                         OMX_PTR pAppData,
                                         OMX_BUFFERHEADERTYPE* pBufHdr);

    static OMX_ERRORTYPE OmxFillBufferDone(OMX_HANDLETYPE hComponent,
                                        OMX_PTR pAppData,
                                        OMX_BUFFERHEADERTYPE* pBufHdr);

    static void VppEmptyBufferDone(void *, struct vpp_buffer *buf);
    static void VppOutputBufferDone(void *, struct vpp_buffer *buf);
    static void VppEvent(void *, struct vpp_event e);

    OMX_ERRORTYPE set_codec_info(OMX_STRING cmpName, void* vpp_ctxt);

    // FOR Vpp Flush and Reconfigure event masks
    enum VppEventPorts {
       VPP_NONE,
       VPP_INPUT,
       VPP_OUTPUT,
       VPP_ALL
    };

private:
    OMX_STATETYPE         mState;
    OMX_PTR               mAppData;
    OMX_CALLBACKTYPE      mOmxCallbacks;
    sem_t                 mEventLock;
    Mutex                 mDecoderSyncLock;
    Mutex                 mVppSyncLock;
    Mutex                 mFbdSyncLock;
    Mutex                 mFlushLock;


    // OMX component for vdec
    OMX_PTR                   mOmxHandle;
    char                      mOmxRole[ROLE_MAX_SIZE];

    // OMX Core
    void*                     mOmxDecoderLib;
    create_qc_omx_component   mCreateOmxFn;

    bool                      mFlushing;
    bool                      mReconfigPending;
    uint8_t                   mVppFlushStatus;
    // vpp_library
    struct vpp_requirements mVppRequirements;
    void*                   mVppContext;
    uint32_t                mVppFlags;
    bool                    mVppBypassMode;
    bool                    mVppEnable;
    bool                    mVppExtradataMode;
    OMX_U32                 mVppColorFormat;
    struct vpp_port_param   mVppPortParam;
    struct hqv_control      mVppHqvControl;
    struct vpp_callbacks    sVppCallbacks;
    OMX_PARAM_PORTDEFINITIONTYPE  mOutputPortDef;
    VppBufferManager        mBufferManager;
    struct vpp_port_param   mDecRcvdParamData;

};

#endif /* VPP_OMX_COMPONENT_H */
