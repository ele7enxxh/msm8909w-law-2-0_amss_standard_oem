/******************************************************************************
 * @file    IRadioConfig.aidl
 * @brief   This interface describes the APIs to SET/GET Radio
 *          Configuration settings
 *
 * @version 00.00.01
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All rights reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import com.qualcomm.qti.radioconfiginterface.RadioConfigIntegerList;
import com.qualcomm.qti.radioconfiginterface.RadioConfigStringList;
import com.qualcomm.qti.radioconfiginterface.RadioConfigParcelable;
import com.qualcomm.qti.radioconfiginterface.LtedConfig;
import com.qualcomm.qti.radioconfiginterface.ILtedProvisioningListener;

interface IRadioConfig {

  RadioConfigIntegerList setIntConfig(in RadioConfigIntegerList config, in int slotId);

  RadioConfigIntegerList getIntConfig(in RadioConfigIntegerList config, in int slotId);

  RadioConfigStringList setStringConfig(in RadioConfigStringList config, in int slotId);

  RadioConfigStringList getStringConfig(in RadioConfigStringList config, in int slotId);

  int setParcelableConfig(in int item, in RadioConfigParcelable config, in int slotId);

  int getParcelableConfig(in int item, in RadioConfigParcelable extras, in int slotId,
          out RadioConfigParcelable config);

  boolean registerLtedProvisioningListener(ILtedProvisioningListener listener);

  boolean deregisterLtedProvisioningListener(ILtedProvisioningListener listener);
}
