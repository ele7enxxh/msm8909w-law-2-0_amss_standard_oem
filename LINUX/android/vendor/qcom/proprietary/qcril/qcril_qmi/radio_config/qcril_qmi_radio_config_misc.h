/*!
  @file
  qcril_qmi_radio_config_misc.h

  @brief
  Contains miscellaneous functions/structures needed for eadio config

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef QCRIL_QMI_RADIO_CONFIG_MISC_H
#define QCRIL_QMI_RADIO_CONFIG_MISC_H

#include "qcrili.h"
#include "ril.h"
#include "qcril_qmi_radio_config_meta.h"
#include "radio_config_interface.pb.h"

RIL_Token qcril_qmi_radio_config_convert_to_ril_token(uint32_t radio_config_token);

uint32_t qcril_qmi_radio_config_convert_ril_token_to_send(RIL_Token token);

qcril_qmi_radio_config_item_type qcril_qmi_radio_config_map_socket_item_to_config_item
(
   com_qualcomm_qti_radioconfiginterface_ConfigItem config_item
);

com_qualcomm_qti_radioconfiginterface_ConfigItem qcril_qmi_radio_config_map_config_item_to_socket_item
(
   qcril_qmi_radio_config_item_type config_item
);

#endif
