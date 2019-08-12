/******************************************************************************
 *
 *  Copyright (c) 2016 Qualcomm Technologies International, Ltd.
 *  All Rights Reserved.
 *  Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Utility functions to help build and parse the aptX Codec Information
 *  Element and Media Payload.
 *
 ******************************************************************************/

#include "bt_target.h"

#include <string.h>
#include "a2d_api.h"
#include "aptXCaps.h"
#include <utils/Log.h>
#define QTI_APTX_CODEC_ID_BLUETOOTH   ((CsrCodecType) 0x0001)

#define QTI_CHANNELS_STEREO                                              (0x02)

/******************************************************************************
** Function         Chk_BitsSet
**
** Description      Check the given num for the number of bits set
** Returns          A2D_SET_ONE_BIT, if one and only one bit is set
**                  A2D_SET_ZERO_BIT, if all bits clear
**                  A2D_SET_MULTL_BIT, if multiple bits are set
******************************************************************************/
UINT8 Chk_BitsSet(UINT8 num)
{
    UINT8   count;
    BOOLEAN res;
    if(num == 0)
        res = A2D_SET_ZERO_BIT;
    else
    {
        count = (num & (num - 1));
        res = ((count==0)?A2D_SET_ONE_BIT:A2D_SET_MULTL_BIT);
    }
    return res;
}

/******************************************************************************
**
** Function         A2D_Bld_QTI_APTXInfo
**
******************************************************************************/

UINT8 A2D_BldAptxInfo(UINT8 media_type, tA2D_APTX_CIE *p_ie, UINT8 *p_result)
{
    UINT8 status = 0;
    status = A2D_SUCCESS;
    *p_result++ = BTA_AV_CO_APTX_CODEC_LEN;
    *p_result++ = media_type;
    *p_result++ = NON_A2DP_MEDIA_CT;
    *p_result++ = (UINT8)(p_ie->vendorId & 0x000000FF);
    *p_result++ = (UINT8)(p_ie->vendorId & 0x0000FF00)>> 8;
    *p_result++ = (UINT8)(p_ie->vendorId & 0x00FF0000)>> 16;
    *p_result++ = (UINT8)(p_ie->vendorId & 0xFF000000)>> 24;
    *p_result++ = (UINT8)(p_ie->codecId & 0x00FF);
    *p_result++ = (UINT8)(p_ie->codecId & 0xFF00) >> 8;
    *p_result++ = p_ie->sampleRate | p_ie->channelMode;
    return status;
}

/******************************************************************************
**
** Function         A2D_ParsAptxInfo
**
******************************************************************************/
tA2D_STATUS A2D_ParsAptxInfo(tA2D_APTX_CIE *p_ie, UINT8 *p_info, BOOLEAN for_caps)
{
    tA2D_STATUS status;
    UINT8   losc;
    UINT8   mt;

    if( p_ie == NULL || p_info == NULL)
        status = A2D_INVALID_PARAMS;
    else
    {
        losc    = *p_info++;
        mt      = *p_info++;
        ALOGD("losc %d, mt %02x", losc, mt);
        /* If the function is called for the wrong Media Type or Media Codec Type */
        if(losc != BTA_AV_CO_APTX_CODEC_LEN || *p_info != NON_A2DP_MEDIA_CT) {
           ALOGE("A2D_ParsAptxInfo: wrong media type %02x",*p_info);
              status = A2D_WRONG_CODEC;
        }
        else
        {
            p_info++;
            p_ie->vendorId = (*p_info & 0x000000FF) |
                                        (*(p_info+1) << 8    & 0x0000FF00) |
                                        (*(p_info+2) << 16  & 0x00FF0000) |
                                        (*(p_info+3) << 24  & 0xFF000000);
            p_info = p_info+4;
            p_ie->codecId = (*p_info & 0x00FF) |(*(p_info+1) << 8 & 0xFF00);
            p_info = p_info+2;
            p_ie->channelMode= *p_info & 0x0F;
            p_ie->sampleRate = *p_info & 0xF0;

            status = A2D_SUCCESS;

            if(for_caps == FALSE)
            {
                if(Chk_BitsSet(p_ie->sampleRate) != A2D_SET_ONE_BIT)
                    status = A2D_BAD_SAMP_FREQ;
                if(Chk_BitsSet(p_ie->channelMode) != A2D_SET_ONE_BIT)
                    status = A2D_BAD_CH_MODE;
            }
        }
    }
    return status;
}

/*******************************************************************************
**
** Function         bta_av_aptx_cfg_in_cap
**
** Description      This function checks whether an aptX codec configuration
**                  is allowable for the given codec capabilities.
**
** Returns          0 if ok, nonzero if error.
**
*******************************************************************************/
UINT8 bta_av_aptx_cfg_in_cap(UINT8 *p_cfg, tA2D_APTX_CIE *p_cap)
{
    UINT8           status = 0;
    tA2D_APTX_CIE    cfg_cie;

    /* parse configuration */
    if ((status = A2D_ParsAptxInfo(&cfg_cie, p_cfg, FALSE)) != 0)
    {
        ALOGE("bta_av_aptx_cfg_in_cap, aptx parse failed");
        return status;
    }

    /* verify that each parameter is in range */

    /* sampling frequency */
    if ((cfg_cie.sampleRate & p_cap->sampleRate) == 0)
    {
        status = A2D_NS_SAMP_FREQ;
    }
    /* channel mode */
    else if ((cfg_cie.channelMode & p_cap->channelMode) == 0)
    {
        status = A2D_NS_CH_MODE;
    }
    return status;
}
