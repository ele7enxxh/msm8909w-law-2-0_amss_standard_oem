/******************************************************************************
#  @file    lpa_misc.c
#  @brief   Local profile assistant misc
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "qcril_uim_lpa_misc.h"
#include "qcril_log.h"
#include "qcrili.h"

#define UIM_LPA_TOKEN_SPACE (0xD0000000)

//===========================================================================
// qcril_uim_lpa_convert_uim_token_to_ril_token
//===========================================================================
RIL_Token qcril_uim_lpa_convert_uim_token_to_ril_token(uint32_t uim_token)
{
  RIL_Token ret = qcril_malloc(sizeof(uint32_t));
  if (NULL != ret)
  {
    uint32_t *tmp = (uint32_t*) ret;
    *tmp = uim_token ^ UIM_LPA_TOKEN_SPACE;
  }
  return ret;
} // qcril_uim_lpa_convert_uim_token_to_ril_token

//===========================================================================
// qcril_uim_lpa_free_and_convert_ril_token_to_uim_token
//===========================================================================
uint32_t qcril_uim_lpa_free_and_convert_ril_token_to_uim_token(RIL_Token ril_token)
{
  uint32_t ret = 0xFFFFFFFF;
  if (ril_token)
  {
      ret = (*((uint32_t *) ril_token)) ^ UIM_LPA_TOKEN_SPACE;
      QCRIL_LOG_INFO("uim token: %d", ret);
      qcril_free((void*) ril_token);
  }
  else
  {
      QCRIL_LOG_INFO("ril_token is NULL");
  }

  return ret;
} // qcril_uim_lpa_free_and_convert_ril_token_to_uim_token

//===========================================================================
// qcril_uim_lpa_map_event_to_request
//===========================================================================
lpa_MessageId qcril_uim_lpa_map_event_to_request(int event)
{
    lpa_MessageId ret;

    switch(event)
    {
      case QCRIL_EVT_UIM_LPA_USER_REQ:
        ret = lpa_MessageId_UIM_LPA_USER_REQUEST;
        break;

      case QCRIL_EVT_UIM_LPA_HTTP_REQ:
        ret = lpa_MessageId_UIM_LPA_HTTP_TXN_COMPLETED_REQUEST;
        break;

      default:
        QCRIL_LOG_DEBUG("didn't find direct mapping for event %d", event);
        if ( event > QCRIL_EVT_UIM_LPA_REQ_BASE && event < QCRIL_EVT_UIM_LPA_REQ_MAX )
        {
          ret = event - QCRIL_EVT_UIM_LPA_REQ_BASE;
        }
        else
        {
          ret = lpa_MessageId_UNKNOWN_REQ;
        }
        break;
    }

    QCRIL_LOG_INFO("event %d mapped to uim_msg %d", event, ret);
    return ret;
} //qcril_uim_lpa_map_event_to_request
