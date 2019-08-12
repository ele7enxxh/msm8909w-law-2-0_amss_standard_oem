/* =========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2009, The Linux Foundation. All rights reserved.
--------------------------------------------------------------------------*/
/*============================================================================
                            O p e n M A X   w r a p p e r s
                O p e n  M A X   C o m p o n e n t  I n t e r f a c e

*//** @file vpp_omx_component.cpp
  This module contains the abstract interface for the OpenMAX components.

*//*========================================================================*/

//////////////////////////////////////////////////////////////////////////////
//                             Include Files
//////////////////////////////////////////////////////////////////////////////
#include <dlfcn.h>
#include <sys/prctl.h>
#include <gralloc_priv.h>
#include <media/hardware/HardwareAPI.h>
#include "vpp_omx_component.h"
#include "omx_core_cmp.h"
#include "OMX_QCOMExtns.h"

#define _ANDROID_
#include "vidc_debug.h"
#undef _ANDROID_

#define QC_OMX_DECODER_LIBRARY "libOmxVdec.so"
#define DISPLAY_BUF_COUNT 4
#define DEFAULT_EXTRADATA OMX_QcomIndexParamInterlaceExtraData
#define ARG_TOUCH(x) (void)x

// static variable
OMX_CALLBACKTYPE VppOmxComponent::sOmxCallbacks = {
    &OmxEventHandler, &OmxEmptyBufferDone, &OmxFillBufferDone
};

// For runtime log level changes
int debug_level;

// factory function executed by the core to create instances
void *get_omx_component_factory_fn(void)
{
    return (new VppOmxComponent);
}

void* omx_message_thread(void *input);
void post_message(VppOmxComponent *omx, unsigned char id);

VppOmxComponent::VppOmxComponent()
    : mState(OMX_StateInvalid),
      mAppData(NULL),
      mCreateOmxFn(NULL),
      mFlushing(false),
      mReconfigPending(false),
      mVppFlushStatus(0),
      mVppContext(NULL),
      mVppFlags(0),
      mVppBypassMode(false),
      mVppEnable(true),
      mVppExtradataMode(false),
      mVppColorFormat(OMX_COLOR_FormatUnused)
{
    memset(&m_cmp, 0, sizeof(m_cmp));
    memset(&mOmxCallbacks,0,sizeof(mOmxCallbacks));
    sem_init(&mEventLock,0,0);

    //Initializing the vppLibray specific structures
    memset(&mVppRequirements, 0, sizeof(mVppRequirements));
    memset(&mVppPortParam, 0, sizeof(mVppPortParam));
    sVppCallbacks = {(void*)this, &VppEmptyBufferDone, &VppOutputBufferDone, &VppEvent};

    //Set HQV_CONTROL as AUTO
    memset(&mVppHqvControl, 0, sizeof(mVppHqvControl));
    mVppHqvControl.mode = HQV_MODE_AUTO;
    memset(&mOutputPortDef, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
    mOutputPortDef.nSize = 0;

    char property_value[PROPERTY_VALUE_MAX] = {0};
    property_get("vpp.debug.level", property_value, "1");
    debug_level = atoi(property_value);
    property_value[0] = '\0';

    memset(&mDecRcvdParamData, 0, sizeof(mDecRcvdParamData));


    DEBUG_PRINT_INFO("Constructed VppOmxComponent");
}

VppOmxComponent::~VppOmxComponent()
{
    DEBUG_PRINT_INFO("VppOmxComponent::~VppOmxComponent");
    sem_destroy(&mEventLock);
}

OMX_ERRORTYPE VppOmxComponent::loadOmxDecoder()
{
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("loadOmxDecoder %s", QC_OMX_DECODER_LIBRARY);
    mOmxDecoderLib = dlopen(QC_OMX_DECODER_LIBRARY, RTLD_NOW);

    if (mOmxDecoderLib == NULL) {
        // check for errors
        const char* pErr = dlerror();
        if (!pErr) {
            // No error reported, but no handle to the library
            DEBUG_PRINT_ERROR("loadOmxDecoder: Error opening "
                 "library (%s) but no error reported\n", QC_OMX_DECODER_LIBRARY);
        } else {
            // Error reported
            DEBUG_PRINT_ERROR("loadOmxDecoder: Error opening library (%s): %s\n",
                                QC_OMX_DECODER_LIBRARY, pErr);
        }
        eRet = OMX_ErrorUndefined;
        return eRet;
    }
    mCreateOmxFn = (create_qc_omx_component)dlsym(mOmxDecoderLib,
                                             "get_omx_component_factory_fn");
    if (mCreateOmxFn == NULL) {
        DEBUG_PRINT_ERROR("Unable to load sym get_omx_component_factory_fn");
        dlclose(mOmxDecoderLib);
        mOmxDecoderLib = NULL;
        eRet = OMX_ErrorUndefined;
    }
    return eRet;
}

OMX_ERRORTYPE VppOmxComponent::component_init(OMX_IN OMX_STRING cmpName)
{
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("component_init In %s", (char*)cmpName); //OMX.QCOM.H264.decoder.super
    strlcpy(mOmxRole, (char*)cmpName, ROLE_MAX_SIZE);

    if ((eRet = loadOmxDecoder()) != OMX_ErrorNone) {
        DEBUG_PRINT_ERROR("component_init failed to load omx core lib \n");
        return eRet;
    }

    void* omxCmp = mCreateOmxFn();
    mOmxHandle = qc_omx_create_component_wrapper((OMX_PTR)omxCmp);

    eRet = qc_omx_component_init(mOmxHandle, mOmxRole);
    if (eRet != OMX_ErrorNone) {
        DEBUG_PRINT_ERROR("Component init failed %s %d", mOmxRole, eRet);
        dlclose(mOmxDecoderLib);
        mOmxDecoderLib = NULL;
        return eRet;
    }

    qc_omx_component_set_callbacks(mOmxHandle, &sOmxCallbacks, this);
    mState = OMX_StateLoaded;

#ifdef DEFAULT_EXTRADATA
    QOMX_ENABLETYPE paramEnable;
    paramEnable.bEnable = OMX_TRUE;
    eRet = qc_omx_component_set_parameter(mOmxHandle, (OMX_INDEXTYPE) OMX_QcomIndexParamInterlaceExtraData, &paramEnable);
    if (eRet != OMX_ErrorNone) {
        DEBUG_PRINT_ERROR("Enable Extradata failed!");
    } else {
        mVppExtradataMode = true;
    }
#endif

    int len = strlen(cmpName);
    if (!strcmp(cmpName+len-7, ".secure")) mVppFlags |= VPP_SESSION_SECURE;
        mVppContext = vpp_init(mVppFlags, sVppCallbacks);
    if(!mVppContext) {
        DEBUG_PRINT_ERROR("Error Initing vpp");
    }

    // send Default vppHqvControls.
    char value[PROPERTY_VALUE_MAX];
    int32_t vppRet = 0;
    if (!mVppContext)
        return eRet;
    property_get("media.vpp.default.config", value, NULL);
    if (!strcmp("1", value) || !strcmp("true", value)) {
        DEBUG_PRINT_LOW("Sending default Config. %d %d", mVppHqvControl.mode, mVppHqvControl.ctrl_type);
        vppRet = vpp_set_ctrl(mVppContext, mVppHqvControl, &mVppRequirements);
        if (vppRet != VPP_OK) {
            //TODO set vpp bypass mode here
            if(vppRet == VPP_ERR_PARAM) {
                DEBUG_PRINT_ERROR("vpp_set_ctrl with default config failed Error_Param :\
                       %d with vpp_requirements, i/p[%d,%d] and o/p[%d,%d]", vppRet,
                       mVppRequirements.in_factor.mul, mVppRequirements.in_factor.add,
                       mVppRequirements.out_factor.mul, mVppRequirements.out_factor.add);
            } else {
                // VPP_ERR_INVALID_CFG
                DEBUG_PRINT_ERROR("vpp_set_ctrl with default config failed %d", vppRet);
            }
        } else {
            DEBUG_PRINT_INFO("vpp_set_ctrl successful in: %d %d, out: %d %d, meta cnt %d",
                   mVppRequirements.in_factor.mul, mVppRequirements.in_factor.add,
                   mVppRequirements.out_factor.mul, mVppRequirements.out_factor.add,
                   mVppRequirements.metadata.cnt);
        }
    }

    //send codec Info to Vpp
    eRet = set_codec_info(cmpName, mVppContext);

    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                  get_component_version(OMX_HANDLETYPE       cmp_handle,
                                       OMX_STRING             cmp_name,
                                       OMX_VERSIONTYPE*    cmp_version,
                                       OMX_VERSIONTYPE*   spec_version,
                                       OMX_UUIDTYPE*          cmp_UUID)
{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(cmp_name);

    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("Get Comp Version in Invalid State\n");
        return OMX_ErrorInvalidState;
    }
    return qc_omx_component_get_version(mOmxHandle, mOmxRole,
                                 cmp_version, spec_version, cmp_UUID);
}

OMX_ERRORTYPE  VppOmxComponent::
                 send_command(OMX_HANDLETYPE cmp_handle,
                              OMX_COMMANDTYPE       cmd,
                              OMX_U32            param1,
                              OMX_PTR          cmd_data)
{
    ARG_TOUCH(cmp_handle);

    if(mState == OMX_StateInvalid) {
         DEBUG_PRINT_ERROR("send_command called in invalid state");
         return OMX_ErrorInvalidState;
    }
    DEBUG_PRINT_INFO("send_command In %d param1 %d", cmd, param1);

    if(mVppContext && !mVppBypassMode) {
        if(cmd == OMX_CommandFlush && ((param1 == OMX_ALL) || (param1 == OMX_CORE_OUTPUT_PORT_INDEX))) {
            AutoMutex autoLock(mFlushLock);
            mFlushing = true;
            vpp_flush(mVppContext,VPP_PORT_INPUT);
            vpp_flush(mVppContext,VPP_PORT_OUTPUT);
            // VPP flush is issued once flush done is received from decoder
        } else if(cmd == OMX_CommandPortDisable && ((param1 == OMX_ALL) || (param1 == OMX_CORE_OUTPUT_PORT_INDEX))) {
            AutoMutex autoLock(mFlushLock);
            mFlushing = true;
            vpp_flush(mVppContext,VPP_PORT_INPUT);
            vpp_flush(mVppContext,VPP_PORT_OUTPUT);
            if(mReconfigPending) mReconfigPending = false;
            // VPP flush is issued once flush done is received from decoder
        } else if(cmd == OMX_CommandStateSet && (param1 == OMX_StateIdle) && mState > OMX_StateIdle) {
            mFlushing = true;
        } else if(cmd == OMX_CommandPortEnable && (param1 == OMX_CORE_OUTPUT_PORT_INDEX)) {
            //CLEAR vector..
            mBufferManager.ClearBuffers();
        }
    }
    return qc_omx_component_send_command(mOmxHandle, cmd, param1, cmd_data);
}

OMX_ERRORTYPE  VppOmxComponent::
                 get_parameter(OMX_HANDLETYPE     cmp_handle,
                               OMX_INDEXTYPE     param_index,
                               OMX_PTR            param_data)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("get_parameter In");
    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("get_parameter in Invalid State\n");
        return OMX_ErrorInvalidState;
    }

    eRet = qc_omx_component_get_parameter(mOmxHandle,
                            param_index, param_data);

    if (mVppContext && (param_index == OMX_IndexParamPortDefinition)) {
        OMX_PARAM_PORTDEFINITIONTYPE* portDef =
                           (OMX_PARAM_PORTDEFINITIONTYPE*)param_data;
        OMX_VIDEO_PORTDEFINITIONTYPE *videoDef = &(portDef->format.video);
        if (portDef->nPortIndex == 1) {
            DEBUG_PRINT_INFO("FRAME WIDTH %d FRAME HEIGHT %d",videoDef->nFrameHeight,videoDef->nFrameWidth);
            mBufferManager.UpdateBufferRequirements(mVppRequirements,portDef);
            DEBUG_PRINT_INFO("get_parameter counts %d %d ",portDef->nBufferCountActual, portDef->nBufferCountMin);
        }
    }


    // VPP can operate only on VENUS NV12 format
    if (mVppContext && param_index == OMX_IndexParamVideoPortFormat)  {
        OMX_VIDEO_PARAM_PORTFORMATTYPE *portFmt =
               (OMX_VIDEO_PARAM_PORTFORMATTYPE*)param_data;
        if (OMX_CORE_OUTPUT_PORT_INDEX == portFmt->nPortIndex) {

            if (portFmt->eColorFormat == (OMX_COLOR_FORMATTYPE)QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed) {
                portFmt->eColorFormat = (OMX_COLOR_FORMATTYPE)QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m;
                DEBUG_PRINT_HIGH("VPP switched from NV12 UBWC to NV12");
            }
            if (mVppEnable) {
                mVppColorFormat = portFmt->eColorFormat;
                if (portFmt->eColorFormat == (OMX_COLOR_FORMATTYPE)QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m) {
                    mVppBypassMode = false;
                    DEBUG_PRINT_HIGH("Get_param Normal Mode");
                } else {
                    mVppBypassMode = true;
                    DEBUG_PRINT_HIGH("Get_param Bypass Mode");
                }
            }
        }
    }

    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 set_parameter(OMX_HANDLETYPE     cmp_handle,
                               OMX_INDEXTYPE     param_index,
                               OMX_PTR            param_data)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("set_parameter In");
    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("set_parameter in Invalid State\n");
        return OMX_ErrorInvalidState;
    }

    if(param_index == (OMX_INDEXTYPE) OMX_QcomIndexParamEnableVpp){

        QOMX_VPP_ENABLE* vppEnableCtrl = (QOMX_VPP_ENABLE*) param_data;
        mVppEnable = vppEnableCtrl->enable_vpp;
        DEBUG_PRINT_LOW("VPP Enable Flag set to %d", mVppEnable);
        mVppBypassMode = !mVppEnable;
        return OMX_ErrorNone;

    } else if (param_index == (OMX_INDEXTYPE) OMX_QcomIndexParamVppHqvControl) {

        QOMX_VPP_HQVCONTROL *hqvControl = (QOMX_VPP_HQVCONTROL *) param_data;
        hqv_control_type prevCtrlType = mVppHqvControl.ctrl_type;
        hqv_mode prevMode = mVppHqvControl.mode;
        hqv_control tmpHqvControl;
        tmpHqvControl.mode = (enum hqv_mode) hqvControl->mode;
        tmpHqvControl.ctrl_type = (enum hqv_control_type) hqvControl->ctrl_type;
        tmpHqvControl.custom = *((struct hqv_ctrl_custom *) &hqvControl->custom);

        DEBUG_PRINT_LOW("Setting Hqv Control via set param: VPP mode:%d",tmpHqvControl.mode);
        DEBUG_PRINT_LOW("VPP Ctrl type:%d",tmpHqvControl.ctrl_type);
        DEBUG_PRINT_LOW("VPP AIE mode:%d",tmpHqvControl.aie.mode);
        DEBUG_PRINT_LOW("VPP HUE mode:%d",tmpHqvControl.aie.hue_mode);
        DEBUG_PRINT_LOW("VPP CADE level:%d",tmpHqvControl.aie.cade_level);
        DEBUG_PRINT_LOW("VPP LTM level:%d",tmpHqvControl.aie.ltm_level);

        auto eRet = vpp_set_ctrl(mVppContext, tmpHqvControl, &mVppRequirements);

        /*
            If eRet is VPP_OK, means the above change in ctrl is allowed and successful.
            In case return is not VPP_OK, we allow it to run with
            the previous algo and corresponding level since it is dynamic ctrl setting.
            */

        if(eRet != VPP_OK){
           DEBUG_PRINT_ERROR("vpp_set_ctrl failed return %d,algo and settings will default to previous", eRet);
           return OMX_ErrorNone;
        }

        /* Update the algo and level only if vpp_set_ctrl did not failed*/
        mVppHqvControl.mode = (enum hqv_mode) hqvControl->mode;
        mVppHqvControl.ctrl_type = (enum hqv_control_type) hqvControl->ctrl_type;
        mVppHqvControl.custom = *((struct hqv_ctrl_custom *) &hqvControl->custom);

        return OMX_ErrorNone;
    }

    /* Other parameters first need to go to the decoder and then to VPP*/

    eRet = qc_omx_component_set_parameter(mOmxHandle, param_index, param_data);

    if (!mVppContext  || (eRet != OMX_ErrorNone))
    {
        DEBUG_PRINT_ERROR("set parameter return, mVppContext %p",(void*)mVppContext);
        return eRet;
    }

    if (mVppContext && param_index == OMX_IndexParamVideoPortFormat)  {
        OMX_VIDEO_PARAM_PORTFORMATTYPE *portFmt =
               (OMX_VIDEO_PARAM_PORTFORMATTYPE*)param_data;
        if (OMX_CORE_OUTPUT_PORT_INDEX == portFmt->nPortIndex) {
            if (mVppEnable) {
                mVppColorFormat = portFmt->eColorFormat;
                if (portFmt->eColorFormat == (OMX_COLOR_FORMATTYPE)QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m) {
                    mVppBypassMode = false;
                    DEBUG_PRINT_HIGH("Set_param Normal Mode");
                } else {
                    mVppBypassMode = true;
                    DEBUG_PRINT_HIGH("Set_param Bypass Mode, YUV format not supported by vpp");
                }
            }
        }
    }

    if (param_index == OMX_IndexParamPortDefinition) {
        OMX_PARAM_PORTDEFINITIONTYPE* portDef =
                       (OMX_PARAM_PORTDEFINITIONTYPE*)param_data;
            OMX_VIDEO_PORTDEFINITIONTYPE *videoDef = &(portDef->format.video);
        if (portDef->nPortIndex == 1) {
            DEBUG_PRINT_LOW("set_parameter for output port, port def %d ", portDef->nBufferCountActual);
            if (mVppPortParam.height != videoDef->nFrameHeight ||
                mVppPortParam.width != videoDef->nFrameWidth) {
            mVppPortParam.height = videoDef->nFrameHeight;
            mVppPortParam.width = videoDef->nFrameWidth;
            mVppPortParam.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
            DEBUG_PRINT_LOW("Set vpp heigth %d & width %d ", videoDef->nFrameHeight, videoDef->nFrameWidth);
            int32_t vpp_err = vpp_set_parameter(mVppContext, VPP_PORT_OUTPUT, mVppPortParam);
            DEBUG_PRINT_INFO("vpp_set_parameter output err %d", vpp_err);
            vpp_err = vpp_set_parameter(mVppContext, VPP_PORT_INPUT, mVppPortParam);
            DEBUG_PRINT_INFO("vpp_set_parameter input err %d", vpp_err);
            }
        }
    } else if (param_index == (OMX_INDEXTYPE) OMX_QcomIndexParamVideoSyncFrameDecodingMode) {
        // Vpp doesn't involve in thumbnail generation
        DEBUG_PRINT_LOW("Thumbnail generation not supported ");
        vpp_term(mVppContext);
        mVppContext = NULL;
    } else if ((param_index == (OMX_INDEXTYPE) OMX_QTIIndexParamVideoPreferAdaptivePlayback) ||
                (param_index == (OMX_INDEXTYPE) OMX_QcomIndexParamVideoAdaptivePlaybackMode)) {
        mBufferManager.SetBufferMode(((QOMX_ENABLETYPE *)param_data)->bEnable);
        if (mBufferManager.GetBufferMode()) {
            DEBUG_PRINT_HIGH("Prefer Adaptive Playback is set also in VPP");
        }
    }

    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 get_config(OMX_HANDLETYPE      cmp_handle,
                            OMX_INDEXTYPE     config_index,
                            OMX_PTR            config_data)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("get_config In");
    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("get_config in Invalid State\n");
        return OMX_ErrorInvalidState;
    }

    eRet = qc_omx_component_get_config(mOmxHandle,
                            config_index, config_data);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 set_config(OMX_HANDLETYPE      cmp_handle,
                            OMX_INDEXTYPE     config_index,
                            OMX_PTR            config_data)
{
    ARG_TOUCH(cmp_handle);

    DEBUG_PRINT_LOW("set_config In");
    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("set_config in Invalid State\n");
        return OMX_ErrorInvalidState;
    }
    return qc_omx_component_set_config(mOmxHandle,
                            config_index, config_data);
}

OMX_ERRORTYPE  VppOmxComponent::
                 get_extension_index(OMX_HANDLETYPE  cmp_handle,
                                     OMX_STRING       paramName,
                                     OMX_INDEXTYPE*   indexType)

{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("get_extension_index In");
    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("get_extension_index in Invalid State\n");
        return OMX_ErrorInvalidState;
    }
    eRet = qc_omx_component_get_extension_index(mOmxHandle,
                            paramName, indexType);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                  get_state(OMX_HANDLETYPE  cmp_handle,
                           OMX_STATETYPE*       state)

{
    ARG_TOUCH(cmp_handle);

    return qc_omx_component_get_state(mOmxHandle, state);
}

OMX_ERRORTYPE  VppOmxComponent::
                  component_tunnel_request(OMX_HANDLETYPE           cmp_handle,
                                          OMX_U32                        port,
                                          OMX_HANDLETYPE       peer_component,
                                          OMX_U32                   peer_port,
                                          OMX_TUNNELSETUPTYPE*   tunnel_setup)
{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(port);
    ARG_TOUCH(peer_port);
    ARG_TOUCH(peer_component);
    ARG_TOUCH(tunnel_setup);
    OMX_ERRORTYPE eRet = OMX_ErrorNotImplemented;
    ALOGW("component_tunnel_request not implemented");
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 use_buffer(OMX_HANDLETYPE                cmp_handle,
                            OMX_BUFFERHEADERTYPE**        buffer_hdr,
                            OMX_U32                             port,
                            OMX_PTR                         app_data,
                            OMX_U32                            bytes,
                            OMX_U8*                           buffer)
{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(port);
    ARG_TOUCH(buffer_hdr);
    ARG_TOUCH(app_data);
    ARG_TOUCH(bytes);
    ARG_TOUCH(buffer);
    DEBUG_PRINT_LOW("Use Buffer In");
    auto eRet = qc_omx_component_use_buffer(mOmxHandle, buffer_hdr, port, app_data, bytes, buffer);
    if (mVppContext) {
        mBufferManager.RegisterClientBuffer(*buffer_hdr,buffer);
        DEBUG_PRINT_LOW("use_buffer add OmxVppBuffer owner = omx_client, *buffer_hdr %p \
              buffer %p ",(void*)(*buffer_hdr), (void *)buffer);
    }
    return eRet;
}


OMX_ERRORTYPE  VppOmxComponent::
                 allocate_buffer(OMX_HANDLETYPE                cmp_handle,
                                 OMX_BUFFERHEADERTYPE**        buffer_hdr,
                                 OMX_U32                             port,
                                 OMX_PTR                         app_data,
                                 OMX_U32                            bytes)

{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(port);
    ARG_TOUCH(buffer_hdr);
    ARG_TOUCH(app_data);
    ARG_TOUCH(bytes);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    eRet = qc_omx_component_allocate_buffer(mOmxHandle, buffer_hdr, port, app_data, bytes);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 free_buffer(OMX_HANDLETYPE         cmp_handle,
                             OMX_U32                      port,
                             OMX_BUFFERHEADERTYPE*      buffer)
{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(port);
    ARG_TOUCH(buffer);
    auto eRet = OMX_ErrorNone;
    DEBUG_PRINT_INFO("free_buffer called omxHeader %p",(void *)buffer);

    eRet = qc_omx_component_free_buffer(mOmxHandle, port, buffer);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                  empty_this_buffer(OMX_HANDLETYPE         cmp_handle,
                                   OMX_BUFFERHEADERTYPE*      buffer)

{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(buffer);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("Etb to the decoder ");
    eRet = qc_omx_component_empty_this_buffer(mOmxHandle, buffer);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                  fill_this_buffer(OMX_HANDLETYPE         cmp_handle,
                                  OMX_BUFFERHEADERTYPE*      buffer)

{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("Ftb from client for buffer: %p",(void*)buffer);
    if (mVppContext != NULL && !mVppBypassMode) {
        mBufferManager.PrintBufferStatistics();
        struct vpp_buffer *vppBuffer = NULL;
        OmxBufferOwner bufDestination = mBufferManager.GetFtbDestination(buffer,&vppBuffer, mReconfigPending);
        if(bufDestination == OmxBufferOwner::OMX_VDEC) {
            AutoMutex autoLock(mDecoderSyncLock);
            eRet = qc_omx_component_fill_this_buffer(mOmxHandle, buffer);
            if(eRet)
                DEBUG_PRINT_ERROR("omx_fill_this_buffer to Decoder returned %d", eRet);
        } else if(bufDestination == OmxBufferOwner::VPP_OUT) {
            AutoMutex autoLock(mVppSyncLock);
            int32_t vpp_err = vpp_queue_buf(mVppContext, VPP_PORT_OUTPUT, vppBuffer);
            if(vpp_err)
                 DEBUG_PRINT_ERROR("vpp_queue_buf output port returned %d", vpp_err);
        } else {
            DEBUG_PRINT_ERROR("You shouldn't be here.. ");
        }
        return eRet; //Need to take vpp_err also into consideration depending on severity
    }
    eRet = qc_omx_component_fill_this_buffer(mOmxHandle, buffer);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 set_callbacks( OMX_HANDLETYPE        cmp_handle,
                                OMX_CALLBACKTYPE*      callbacks,
                                OMX_PTR                 app_data)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("set_callbacks In");

    mOmxCallbacks       = *callbacks;
    mAppData = app_data;
    return eRet;
}

OMX_ERRORTYPE VppOmxComponent::component_deinit(OMX_HANDLETYPE cmp_handle)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("component_deinit In");
    qc_omx_component_deinit(mOmxHandle);
    int err = dlclose(mOmxDecoderLib);
    mOmxDecoderLib = NULL;
    if (err) {
        DEBUG_PRINT_ERROR("Error in dlclose of vdec lib\n");
    }
    vpp_term(mVppContext);
    mVppContext = NULL;
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 use_EGL_image(OMX_HANDLETYPE                cmp_handle,
                               OMX_BUFFERHEADERTYPE**        buffer_hdr,
                               OMX_U32                             port,
                               OMX_PTR                         app_data,
                               void*                          egl_image)
{
    ARG_TOUCH(cmp_handle);
    return qc_omx_component_use_EGL_image(mOmxHandle, buffer_hdr, port,
                                          app_data, egl_image);

}

OMX_ERRORTYPE  VppOmxComponent::
                component_role_enum( OMX_HANDLETYPE cmp_handle,
                                      OMX_U8*              role,
                                      OMX_U32             index)
{
    ARG_TOUCH(cmp_handle);
    return qc_omx_component_role_enum(mOmxHandle, role, index);
}

// callbacks from component
OMX_ERRORTYPE VppOmxComponent::OmxEventHandler(
                                      OMX_HANDLETYPE pHandle,
                                      OMX_PTR pAppData,
                                      OMX_EVENTTYPE eEvent,
                                      OMX_U32 nData1,
                                      OMX_U32 nData2,
                                      OMX_PTR pEventData)
{
    ARG_TOUCH(pHandle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    auto super_omx = reinterpret_cast<VppOmxComponent*>(pAppData);
    auto omx_comp = (OMX_COMPONENTTYPE*)(super_omx->mOmxHandle);
    struct timespec ts;

    DEBUG_PRINT_LOW("OmxEventHandler In, event %d", eEvent);
    switch ((OMX_U32)eEvent)
    {
        case OMX_EventCmdComplete:
        {
            DEBUG_PRINT_LOW("Command complete %d %d", nData1, nData2);
            if (nData1 == OMX_CommandFlush && (nData2 == OMX_ALL || nData2 == 1)) {
                if(super_omx->mVppContext && !super_omx->mVppBypassMode) {
                    AutoMutex autoLock(super_omx->mFlushLock);
                    DEBUG_PRINT_INFO("send flush to VPP library ");
                    sem_wait(&super_omx->mEventLock);
                    super_omx->mFlushing = false;
                    DEBUG_PRINT_HIGH("FLUSH RECEIVED");
                }
            }

            if(nData1 == OMX_CommandPortDisable && nData2 == 1) {
                if(super_omx->mVppContext && !super_omx->mVppBypassMode) {
                    AutoMutex autoLock(super_omx->mFlushLock);
                    DEBUG_PRINT_INFO("Port Disable send flush to VPP library ");
                    sem_wait(&super_omx->mEventLock);
                    super_omx->mFlushing = false;
                }
            }

            if(nData1 ==  OMX_CommandStateSet) {
                auto prevState = super_omx->mState;
                super_omx->mState = (OMX_STATETYPE)nData2;
                if(super_omx->mVppContext && !super_omx->mVppBypassMode && nData2 == OMX_StateIdle) {
                        DEBUG_PRINT_INFO("send flush to VPP library ");
                        if(prevState > super_omx->mState) {
                        vpp_flush(super_omx->mVppContext,VPP_PORT_INPUT);
                        vpp_flush(super_omx->mVppContext,VPP_PORT_OUTPUT);
                        sem_wait(&super_omx->mEventLock);
                        super_omx->mFlushing = false;
                        }
                }
            }

            //TODO: Add a wait to serialize VPP flush complete and disable cmd complete
            eRet = super_omx->mOmxCallbacks.EventHandler(&super_omx->m_cmp,
                                  super_omx->mAppData,
                                  OMX_EventCmdComplete,
                                  nData1, nData2, pEventData);
            break;
        }
        case OMX_EventPortFormatDetected:
        {
            DEBUG_PRINT_LOW("OMX_EventPortFormatDetected %d", nData1);
            break;
        }
        case OMX_EventPortSettingsChanged:
        {
            if (nData2 == OMX_IndexParamPortDefinition) {
                DEBUG_PRINT_LOW("OMX_EventPortSettingsChanged %d", nData1);
            } else if (nData2 == OMX_IndexConfigCommonOutputCrop) {
                DEBUG_PRINT_LOW("Reconfig - IndexConfigCommonOutputCrop  ");
            } else {
                DEBUG_PRINT_ERROR("Invalid Port Reconfig Event. ");
                break;
            }

            if(super_omx->mVppContext && !super_omx->mVppBypassMode) {

                OMX_VIDEO_PORTDEFINITIONTYPE *videoFormat =&super_omx->mOutputPortDef.format.video;
                uint32_t *frameData = (uint32_t *)pEventData;

                if(!frameData) {
                    DEBUG_PRINT_ERROR("Reconfig height and width absent, frameData is NULL !!");
                    return eRet;
                }
                    DEBUG_PRINT_INFO("VT %dx%d", frameData[1], frameData[0]);
                    struct vpp_port_param& sVppPortParam = super_omx->mDecRcvdParamData;

                    // Reconfigure VPP only when the height or width has changed, this is
                    // required to prevent calling vpp_reconfigure in regular cases when
                    //the reconfig dimensions are same as crop dimensions. vpp_reconfigure is
                    //expensive as it results in reopening the HVX/GPU session
                    if ((frameData[0] != sVppPortParam.height) ||
                        (frameData[1] != sVppPortParam.width)) {
                        sVppPortParam.height = frameData[0];
                        sVppPortParam.width = frameData[1];
                        sVppPortParam.fmt = VPP_COLOR_FORMAT_NV12_VENUS;
                        DEBUG_PRINT_INFO("Set vpp heigth %d & width %d ", frameData[0],frameData[1]);

                        // Deferring the event for vpp_reconfigure complete.
                        DEBUG_PRINT_INFO("sending reconfigure to VPP library ");
                        super_omx->mReconfigPending = true;
                        vpp_reconfigure(super_omx->mVppContext, sVppPortParam, sVppPortParam);
                        sem_wait(&super_omx->mEventLock);
                    }
                 }

            eRet = super_omx->mOmxCallbacks.EventHandler(&super_omx->m_cmp,
                                  super_omx->mAppData,
                                  OMX_EventPortSettingsChanged,
                                  nData1, nData2, pEventData);
            break;
        }
        case OMX_EventBufferFlag:
        {
            DEBUG_PRINT_LOW("OMX_EventBufferFlag %d", nData1);
            eRet = super_omx->mOmxCallbacks.EventHandler(&super_omx->m_cmp,
                                  super_omx->mAppData,
                                  OMX_EventBufferFlag,
                                  nData1, nData2, pEventData);
            break;
        }
        case OMX_EventError:
        {
            DEBUG_PRINT_LOW("OMX_EventError %d", nData1);
            eRet = super_omx->mOmxCallbacks.EventHandler(&super_omx->m_cmp,
                                  super_omx->mAppData,
                                  OMX_EventError,
                                  nData1, nData2, pEventData);
            break;
        }
        default:
            DEBUG_PRINT_ERROR("unknown event %d", eEvent);
    }
    return eRet;
}

OMX_ERRORTYPE VppOmxComponent::OmxEmptyBufferDone(
                                         OMX_HANDLETYPE hComponent,
                                         OMX_PTR pAppData,
                                         OMX_BUFFERHEADERTYPE* pBufHdr)
{
    ARG_TOUCH(hComponent);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("Ebd from decoder received %p ",(void *)pBufHdr);
    VppOmxComponent* super_omx = reinterpret_cast<VppOmxComponent*>(pAppData);
    eRet = super_omx->mOmxCallbacks.EmptyBufferDone(&super_omx->m_cmp, super_omx->mAppData, pBufHdr);
    return eRet;
}

OMX_ERRORTYPE VppOmxComponent::OmxFillBufferDone(
                                        OMX_HANDLETYPE hComponent,
                                        OMX_PTR pAppData,
                                        OMX_BUFFERHEADERTYPE* buffer)
{
    ARG_TOUCH(hComponent);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    auto super_omx = reinterpret_cast<VppOmxComponent*>(pAppData);
    DEBUG_PRINT_LOW("Fill buffer done from decoder %p",(void *)buffer);
    super_omx->mBufferManager.PrintBufferStatistics();
    if (super_omx->mVppContext && !super_omx->mVppBypassMode) {
        AutoMutex autoLock(super_omx->mFlushLock);
        struct vpp_buffer *vppBuffer = super_omx->mBufferManager.GetBufferForVppIn(
                                            buffer, super_omx->mVppExtradataMode);
        if(!vppBuffer) {
            AutoMutex autoLock(super_omx->mFbdSyncLock);
            DEBUG_PRINT_ERROR("No corresponding vppBuffer, returning to client. ");
            super_omx->mBufferManager.UpdateBufferOwnership(buffer, OmxBufferOwner::OMX_CLIENT);
            eRet = super_omx->mOmxCallbacks.FillBufferDone(&super_omx->m_cmp,
                        super_omx->mAppData, buffer);
            return eRet;
        }

        DEBUG_PRINT_LOW("fd, cookie, allocLen, filledLen, offset, timestamp. %d %p %d %d %d %llu ",
            vppBuffer->pixel.fd, vppBuffer->cookie, vppBuffer->pixel.alloc_len, vppBuffer->pixel.filled_len,
            vppBuffer->pixel.offset, (long long unsigned)vppBuffer->timestamp);

        if (super_omx->mFlushing || super_omx->mReconfigPending || !vppBuffer ||
             (vppBuffer->pixel.filled_len == 0)) {
            AutoMutex autoLock(super_omx->mFbdSyncLock);
            if (super_omx->mFlushing || super_omx->mReconfigPending) {
                DEBUG_PRINT_HIGH("Flush/Reconfig: Buffer returned to client. ");
            } else if (!vppBuffer) {
                DEBUG_PRINT_HIGH("No corresponding vppBuffer, returning to client. ");
            } else if (!vppBuffer->pixel.filled_len) {
                DEBUG_PRINT_HIGH("Buffer filled len is zero. Returning to client. ");
            }
            super_omx->mBufferManager.UpdateBufferOwnership(buffer,OmxBufferOwner::OMX_CLIENT);
            eRet = super_omx->mOmxCallbacks.FillBufferDone(&super_omx->m_cmp, super_omx->mAppData, buffer);
            return eRet;
        }

        int32_t vpp_err = vpp_queue_buf(super_omx->mVppContext, VPP_PORT_INPUT, vppBuffer);

        if(vpp_err)
            DEBUG_PRINT_ERROR("vpp_queue_buf input port returned %d", vpp_err);
        return OMX_ErrorNone; //Need to decide on the return value.
    }

    // No VPP
    eRet = super_omx->mOmxCallbacks.FillBufferDone(&super_omx->m_cmp, super_omx->mAppData, buffer);
    return eRet;
}

// vpp empty_buffer_done callback
void VppOmxComponent::VppEmptyBufferDone(void *handle, struct vpp_buffer *buf)
{
    VppOmxComponent *sOmxVpp = (VppOmxComponent *)handle;

    DEBUG_PRINT_LOW("VppEmptyBufferDone for buffer [%p]",(void*)buf->cookie);
    auto buffer = (OMX_BUFFERHEADERTYPE *)buf->cookie;
    sOmxVpp->mBufferManager.PrintBufferStatistics();
    if (sOmxVpp->mFlushing || sOmxVpp->mReconfigPending) {
        AutoMutex autoLock(sOmxVpp->mFbdSyncLock);
        bool bDisableDynamicBufMode = sOmxVpp->mBufferManager.GetBufferMode();
        DEBUG_PRINT_LOW("Flush/Reconfig: %s :returning buffer to client",__func__);
        sOmxVpp->mBufferManager.UpdateBufferOwnership(buffer,OmxBufferOwner::OMX_CLIENT);
        buffer->nAllocLen = bDisableDynamicBufMode? buffer->nAllocLen: sizeof(struct VideoDecoderOutputMetaData);

        if(!bDisableDynamicBufMode) {
            buffer->nFilledLen = (buffer->nFilledLen ? sizeof(struct VideoDecoderOutputMetaData) : 0);
        }
        sOmxVpp->mOmxCallbacks.FillBufferDone(&sOmxVpp->m_cmp, sOmxVpp->mAppData,
             (OMX_BUFFERHEADERTYPE*)buf->cookie);
        return;
    }

    struct vpp_buffer *vppBuffer = NULL;
    auto bufDestination = sOmxVpp->mBufferManager.GetFtbDestination(buffer,&vppBuffer, sOmxVpp->mReconfigPending);

    if(bufDestination == OmxBufferOwner::OMX_VDEC) {
        AutoMutex autoLock(sOmxVpp->mDecoderSyncLock);
        DEBUG_PRINT_LOW("To Vdec : (buffer Header ,offset, allocLen, nFilledLen, nTimestamp) = (%p,%d,%d,%d,%lld)",
                         (void *)buffer, buffer->nOffset, buffer->nAllocLen, buffer->nFilledLen, buffer->nTimeStamp);

        OMX_ERRORTYPE eRet = qc_omx_component_fill_this_buffer(sOmxVpp->mOmxHandle, buffer);
        if(eRet)
            DEBUG_PRINT_ERROR("omx_fill_this_buffer to Decoder returned %d", eRet);
    } else if(bufDestination == OmxBufferOwner::VPP_OUT) {
        AutoMutex autoLock(sOmxVpp->mVppSyncLock);
        DEBUG_PRINT_LOW("To Vpp : fd, cookie, allocLen, filledLen, offset, timestamp. %d %p %d %d %d %llu ",
        vppBuffer->pixel.fd, vppBuffer->cookie, vppBuffer->pixel.alloc_len, vppBuffer->pixel.filled_len,
        vppBuffer->pixel.offset, (long long unsigned)vppBuffer->timestamp);

        auto vpp_err = vpp_queue_buf(sOmxVpp->mVppContext, VPP_PORT_OUTPUT, vppBuffer);
        if(vpp_err)
                 DEBUG_PRINT_LOW("vpp_queue_buf output port returned %d", vpp_err);
    } else {
        DEBUG_PRINT_ERROR("You shouldn't be here.. ");
    }
}

// vpp output_buffer_done callback
void VppOmxComponent::VppOutputBufferDone(void *handle, struct vpp_buffer *buf)
{
    VppOmxComponent *sOmxVpp = (VppOmxComponent *)handle;

    AutoMutex autoLock(sOmxVpp->mFbdSyncLock);
    DEBUG_PRINT_LOW("Vpp Output buffer done");
    auto buffer = (OMX_BUFFERHEADERTYPE *)buf->cookie;
    bool bDisableDynamicBufMode = sOmxVpp->mBufferManager.GetBufferMode();

    if(sOmxVpp->mBufferManager.HandleBypass(buffer)) {
        DEBUG_PRINT_HIGH("Vpp running in bypass.");
    } else {
        DEBUG_PRINT_LOW("Vpp running is normal mode.");
    }
    sOmxVpp->mBufferManager.PrintBufferStatistics();
    sOmxVpp->mBufferManager.UpdateBufferOwnership(buffer,OmxBufferOwner::OMX_CLIENT);

    /* Populate timestamp and filled length from the vpp_buffer*/

    buffer->nTimeStamp = buf->timestamp;
    DEBUG_PRINT_LOW("VPP output port(buffer,filledLen, allocLen, timestamp) = (%p,%u,%u,%lld) ", (void*)buffer, buf->pixel.filled_len,
                                                                                      buffer->nAllocLen, buffer->nTimeStamp);
    buffer->nAllocLen = bDisableDynamicBufMode? buffer->nAllocLen:sizeof(struct VideoDecoderOutputMetaData);
    buffer->nFilledLen = buf->pixel.filled_len;
    if(bDisableDynamicBufMode) {

        sOmxVpp->mBufferManager.UpdateBufferGeometry(buffer,&sOmxVpp->mVppPortParam);

    } else {
        buffer->nFilledLen = (buffer->nFilledLen ? sizeof(struct VideoDecoderOutputMetaData) : 0);
    }

    sOmxVpp->mBufferManager.UpdateMetaData(buffer,sOmxVpp->mVppColorFormat);
    sOmxVpp->mBufferManager.ConvertOmxVppFlags(buf->flags, buffer->nFlags, false);
    sOmxVpp->mOmxCallbacks.FillBufferDone(&sOmxVpp->m_cmp, sOmxVpp->mAppData,buffer);
}

// vpp event callback
void VppOmxComponent::VppEvent(void *handle, struct vpp_event e)
{
    VppOmxComponent *sOmxVpp = (VppOmxComponent *)handle;
    if (e.type == VPP_EVENT_FLUSH_DONE) {
        if(e.flush_done.port == VPP_PORT_INPUT)
            sOmxVpp->mVppFlushStatus |= (uint8_t)VPP_INPUT;
        else
            sOmxVpp->mVppFlushStatus |= (uint8_t)VPP_OUTPUT;
        DEBUG_PRINT_HIGH("Vpp flush event status %x", sOmxVpp->mVppFlushStatus);

        if (sOmxVpp->mFlushing && sOmxVpp->mVppFlushStatus == VPP_ALL) {
            sem_post(&sOmxVpp->mEventLock);
            sOmxVpp->mVppFlushStatus = 0;
        }
    } else if (e.type == VPP_EVENT_RECONFIG_DONE) {
        DEBUG_PRINT_HIGH("VPP Reconfig event received ");
        sOmxVpp->mVppRequirements = e.port_reconfig_done.req;

        sOmxVpp->mVppPortParam = sOmxVpp->mDecRcvdParamData;

        if(sOmxVpp->mReconfigPending) {
            sem_post(&sOmxVpp->mEventLock);
            sOmxVpp->mReconfigPending = false;
      }
    }
}


//Helper function to parse OMX component name , convert to a codec enum and send it to vpp library
OMX_ERRORTYPE VppOmxComponent::set_codec_info(OMX_STRING cmpName, void* vpp_ctxt)
{
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    struct video_property prop;
    uint32_t vppRet = VPP_OK;

    if (!strncmp(cmpName,"OMX.qcom.video.decoder.avc",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_AVC;
    } else if (!strncmp(cmpName,"OMX.qcom.video.decoder.avc.dsmode",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_AVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.avc.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_AVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.avc.secure.dsmode",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_AVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.divx4",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_DIVX4;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.divx",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_DIVX;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.divx311",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_DIVX311;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.mpeg4",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_MPEG4;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.mpeg4.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_MPEG4;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.mpeg2",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_MPEG2;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.mpeg2.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_MPEG2;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.vc1",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_VC1;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.vc1.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_VC1;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.wmv",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_WMV;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.wmv.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_WMV;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.h263",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_H263;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.hevc",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_HEVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.hevc.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_HEVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.vp8",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_VP8;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.vp9",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_VP9;
    } else {
        DEBUG_PRINT_ERROR("ERROR:Unknown Component");
        prop.codec.eCodec = VPP_CODEC_TYPE_UNKNOWN;
    }

    if (prop.codec.eCodec != VPP_CODEC_TYPE_UNKNOWN) {
        prop.property_type = VID_PROP_CODEC;
        DEBUG_PRINT_LOW(" Codec Info is %d", prop.codec.eCodec);
        vppRet = vpp_set_vid_prop(vpp_ctxt, prop);
        if (vppRet) {
            DEBUG_PRINT_ERROR(" set codec property to vpp failed");
        }
    }
    return eRet;
}

