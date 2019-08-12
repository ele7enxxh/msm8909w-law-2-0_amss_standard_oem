/******************************************************************************
#  @file    qcril_uim_lpa_misc.h
#  @brief   Local profile assistant misc
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef QCRIL_UIM_LPA_MISC_H
#define QCRIL_UIM_LPA_MISC_H


#include "ril.h"
#include "qcrili.h"
#include "lpa.pb.h"

lpa_MessageId qcril_uim_lpa_map_event_to_request                    (int event);
RIL_Token     qcril_uim_lpa_convert_uim_token_to_ril_token          (uint32_t ims_token);
uint32_t      qcril_uim_lpa_free_and_convert_ril_token_to_uim_token (RIL_Token ril_token);

#endif /* QCRIL_UIM_LPA_MISC_H */
