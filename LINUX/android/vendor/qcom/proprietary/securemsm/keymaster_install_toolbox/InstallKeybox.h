/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef INSTALLKEYBOX_H
#define INSTALLKEYBOX_H

#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <keymaster_defs.h>
#include <UniquePtr.h>
#include <keymaster_provision.h>

using namespace keymasterhal;

namespace KM {
#define MAX_KEYS_IN_KEYBOX  2

enum keybox_format {
    FORMAT_NONE,
    PEM
};
class InstallKeybox {
 public:
    InstallKeybox(std::string filename, char *OutFile)
            : mFilename(filename),
              mOutFile(OutFile) {
    }
    ~InstallKeybox() {
    }

    void Run(const char *myDeviceID);
    void ProcessOneXmlKeybox(std::ifstream &inFile, const char *myDeviceID);

 private:

    void ConsumeTag(std::ifstream &inFile, const std::string &tag);
    std::string ConsumeElement(std::ifstream &inFile,
                               const std::string &startTag);
    void cleanKeyBox(km_install_keybox_t *keyboxCtx);

    void ProcessDeviceID(std::ifstream &inFile, keymaster_blob_t *device_id);
    void ProcessKeyAlgo(std::ifstream &inFile,
                        keymaster_algorithm_t *algorithm);
    void ProcessKeyFormat(std::ifstream &inFile);
    void ProcessCertFormat(std::ifstream &inFile);
    void ProcessPrivKey(std::ifstream &inFile, keymaster_key_blob_t *priv_key);
    void ProcessCertificate(std::ifstream &inFile,
                            keymaster_blob_t *certificate);

    std::string mFilename;
    char *mOutFile;
};
}
;

#endif
