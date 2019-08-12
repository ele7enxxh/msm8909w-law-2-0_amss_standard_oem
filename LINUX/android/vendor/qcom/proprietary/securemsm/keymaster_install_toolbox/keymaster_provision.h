/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef KEYMASTER_PROVISION_H_
#define KEYMASTER_PROVISION_H_

#include <cstdlib>
#include <map>
#include <vector>
#include <UniquePtr.h>

#include <hardware/keymaster0.h>
#include <hardware/keymaster1.h>
#include <hardware/keymaster2.h>

namespace keymasterhal {

#define MAX_KEYS_IN_KEYBOX  2

typedef struct {
    keymaster_algorithm_t algo;
    keymaster_key_blob_t key;
    keymaster_cert_chain_t cert_chain;
} keybox_t;

typedef struct _km_install_keybox_t {
    keymaster_blob_t device_id;
    keybox_t keybox[MAX_KEYS_IN_KEYBOX];
} km_install_keybox_t;

class KmKeyProvision {
 public:
    int KeyMasterProvisionInit();

    int KeyMasterInstallKeybox(km_install_keybox_t *keybox);
 private:
    hw_device_t *device;
};

}  // namespace keymaster_provision

#endif //KEYMASTER_PROVISION_H_
