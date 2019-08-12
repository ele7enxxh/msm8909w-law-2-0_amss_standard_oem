/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#include <InstallKeybox.h>

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

static char g_sec_level = '0';

using namespace std;

namespace KM {
static void PrintCertData(keymaster_blob_t *key_blob) {
    int i = 0;
    uint8_t *keyboxData = (uint8_t *) key_blob->data;
    cerr << "key_blob->data_length" << key_blob->data_length << endl;
    for (int i = 0; i < (int) key_blob->data_length; i++) {
        char buf[4];
        if (i % 16 == 0)
            cout << endl;
        snprintf(buf, sizeof(buf), "%02x ", *keyboxData);
        keyboxData++;
        cout << buf;
    }
    cout << endl;
}

static void PrintKeyData(keymaster_key_blob_t *key_blob) {
    int i = 0;
    uint8_t *keyboxData = (uint8_t *) key_blob->key_material;
    cerr << "key_blob->key_material_size" << key_blob->key_material_size
         << endl;
    for (int i = 0; i < (int) key_blob->key_material_size; i++) {
        char buf[4];
        if (i % 16 == 0)
            cout << endl;
        snprintf(buf, sizeof(buf), "%02x ", *keyboxData);
        keyboxData++;
        cout << buf;
    }
    cout << endl;
}

void InstallKeybox::cleanKeyBox(km_install_keybox_t *keyboxCtx) {
    int certCount = 0;
    int keysCount = MAX_KEYS_IN_KEYBOX;
    keymaster_blob_t* cert_entry = NULL;

    if (keyboxCtx->device_id.data) {
        free((void *) keyboxCtx->device_id.data);
        keyboxCtx->device_id.data = NULL;
    }

    while ((keysCount--) && (keysCount < INT_MAX)) {
        if (keyboxCtx->keybox[keysCount].key.key_material)
            free((void *) keyboxCtx->keybox[keysCount].key.key_material);
        keymaster_free_cert_chain(&keyboxCtx->keybox[keysCount].cert_chain);
    }
    memset(keyboxCtx, 0, sizeof(km_install_keybox_t));
}

void InstallKeybox::ConsumeTag(ifstream &inFile, const string &tag) {
    string input;

    getline(inFile, input, '<');
    getline(inFile, input, '>');
    if (input.compare(tag) != 0) {
        cerr << "Corrupt input, expected tag: '" << tag << "' found '" << input
                << "'" << endl;
        inFile.close();
        exit(-1);
    }
}

string InstallKeybox::ConsumeElement(ifstream &inFile, const string &startTag) {
    string result;

    ConsumeTag(inFile, startTag);

    getline(inFile, result, '<');
    inFile.putback('<');

    string endTag = "/" + startTag;
    ConsumeTag(inFile, endTag);
    return result;
}

void InstallKeybox::ProcessDeviceID(ifstream &inFile,
                                    keymaster_blob_t *device_id) {
    string input;
    size_t input_length = 0;

    //Seek and discard the first quotation mark in device id
    getline(inFile, input, '<');
    getline(inFile, input, '"');

    if (input.compare("Keybox DeviceID=") != 0) {
        cerr << "Failed to find 'Keybox DeviceID= in input" << endl;
        inFile.close();
        exit(-1);
    }

    //Grab the characters up to the second quotation mark
    getline(inFile, input, '"');

    //Report current keybox device id and save it off
    cout << "Keybox DeviceID='" << input.c_str() << "'" << endl;
    input_length = input.size();
    device_id->data = (uint8_t*) malloc(input_length);
    if (!device_id->data) {
        cerr << "device_id->data NULL" << endl;
        inFile.close();
        exit(-1);
    }

    memcpy((void *) device_id->data, &input[0], input_length);
    device_id->data_length = input_length;

    //Discard '>' at end of <Keybox DeviceID="...">
    getline(inFile, input, '>');
}

void InstallKeybox::ProcessKeyAlgo(ifstream &inFile,
                                   keymaster_algorithm_t *algorithm) {
    string input;

    //Seek and discard the first quotation mark in key algorithm
    getline(inFile, input, '<');
    getline(inFile, input, '"');

    if (input.compare("Key algorithm=") != 0) {
        cerr << "Failed to find 'Key algorithm in input" << endl;
        inFile.close();
        exit(-1);
    }
    //Grab the characters up to the second quotation mark
    getline(inFile, input, '"');
    //Report current key algorithm and save it off
    cout << "Key Algorithm='" << input.c_str() << "'" << endl;
    if (input.compare("ecdsa") == 0) {
        *algorithm = KM_ALGORITHM_EC;
    } else if (input.compare("rsa") == 0) {
        *algorithm = KM_ALGORITHM_RSA;
    } else {
        cerr << "KeyAlgorithm is not 'ecdsa' or 'rsa' in input" << endl;
        inFile.close();
        exit(-1);
    }
    //Discard '>' at end of <Key algorithm="...">
    getline(inFile, input, '>');

}

void InstallKeybox::ProcessKeyFormat(ifstream &inFile) {
    string input;

    //Seek and discard the first quotation mark in key algorithm
    getline(inFile, input, '<');
    getline(inFile, input, '"');
    if (input.compare("PrivateKey format=") != 0) {
        cerr << "Failed to find 'PrivateKey format' in input" << endl;
        inFile.close();
        exit(-1);
    }
    //Grab the characters up to the second quotation mark
    getline(inFile, input, '"');
    //Report current key format and save it off
    cout << "Key Format='" << input.c_str() << "'" << endl;
    if (input.compare("pem") == 0) {
    } else {
        cerr << "KeyFormat is not 'pem' in input" << endl;
        inFile.close();
        exit(-1);
    }
    //Discard '>' at end of <Key algorithm="...">
    getline(inFile, input, '>');
}

void InstallKeybox::ProcessCertFormat(ifstream &inFile) {
    string input;

    //Seek and discard the first quotation mark in key algorithm
    getline(inFile, input, '<');
    getline(inFile, input, '"');
    if (input.compare("Certificate format=") != 0) {
        cerr << "Failed to find 'Certificate format' in input" << endl;
        inFile.close();
        exit(-1);
    }
    //Grab the characters up to the second quotation mark
    getline(inFile, input, '"');

    cout << "Certificate Format='" << input.c_str() << "'" << endl;
    if (input.compare("pem") == 0) {
    } else {
        cerr << "certFormat is not 'pem' in input" << endl;
        inFile.close();
        exit(-1);
    }
    //Discard '>' at end of <Key algorithm="...">
    getline(inFile, input, '>');
}

void InstallKeybox::ProcessPrivKey(ifstream &inFile,
                                   keymaster_key_blob_t *priv_key) {
    string input;

    // extract the key content and store them in input
    getline(inFile, input);
    getline(inFile, input, '<');

    //copy hex of key content in private key data
    priv_key->key_material = (uint8_t*) malloc(input.size());
    if (!priv_key->key_material) {
        cerr << "priv_key->key_material NULL" << endl;
        inFile.close();
        exit(-1);
    }
    priv_key->key_material_size = 0;
    size_t size = input.size();
    size_t offset = 0;
    istringstream inputstream(input);
    while (size) {
        string input1;
        getline(inputstream >> std::ws, input1);
        if (!input1.size())
            break;
        memcpy((uint8_t *) priv_key->key_material + offset, input1.c_str(),
               input1.size());
        // Add \n to the end, getline omits the \n
        // Is there a better way ?
        memset((uint8_t *) priv_key->key_material + offset + input1.size(),
               0x0A, 1);
        priv_key->key_material_size += input1.size() + 1;
        offset += input1.size() + 1;
        size -= input1.size() + 1;
    }
}

void InstallKeybox::ProcessCertificate(ifstream &inFile,
                                       keymaster_blob_t *certificate) {
    string input;

    // extract the key content and store them in input
    getline(inFile, input);
    getline(inFile, input, '<');

    certificate->data = (uint8_t*) malloc(input.size());
    if (!certificate->data) {
        cerr << "certificate->data" << endl;
        inFile.close();
        exit(-1);
    }
    certificate->data_length = 0;

    size_t size = input.size();
    size_t offset = 0;
    istringstream inputstream(input);
    while (size) {
        string input1;
        getline(inputstream >> std::ws, input1);
        if (!input1.size())
            break;
        memcpy((uint8_t *) certificate->data + offset, input1.c_str(),
               input1.size());
        // Add \n to the end, getline omits the \n
        // Is there a better way ?
        memset((uint8_t *) certificate->data + offset + input1.size(), 0xA, 1);
        certificate->data_length += input1.size() + 1;
        offset += input1.size() + 1;
        size -= input1.size() + 1;
    }
}

void InstallKeybox::ProcessOneXmlKeybox(ifstream &inFile,
                                        const char *myDeviceID) {
    string input;
    int certCount = 0;
    int keyitem = 0, j;
    keymaster_blob_t* cert_entry;
    char tmp[] = { ' ', ' ', '\n' };
    km_install_keybox_t keyboxCtx;

    //cleanup keyboxCtx
    memset(&keyboxCtx, 0, sizeof(keyboxCtx));

    //proces device ID:  <Keybox DeviceID=”mfg_mod123_0000001”>
    ProcessDeviceID(inFile, &(keyboxCtx.device_id));
    cerr << "myDeviceID" << myDeviceID << endl;
    cerr << "keyboxCtx.device_id.data" << keyboxCtx.device_id.data << endl;
    for (keyitem = 0; keyitem < MAX_KEYS_IN_KEYBOX; keyitem++) {
        //proces key algorithm:  <<Key algorithm=”ecdsa"> or <Key algorithm=”rsa”>
        ProcessKeyAlgo(inFile, &(keyboxCtx.keybox[keyitem].algo));
        //proces key format:  <PrivateKey format=”pem”>
        ProcessKeyFormat(inFile);
        //process key content
        ProcessPrivKey(inFile, &(keyboxCtx.keybox[keyitem].key));

        //consume tag: <CertificateChain>
        ConsumeTag(inFile, "CertificateChain");
        //process number of certificates: <NumberOfCertificates>2</NumberOfCertificates>
        certCount = atol(
                ConsumeElement(inFile, "NumberOfCertificates").c_str());
        keyboxCtx.keybox[keyitem].cert_chain.entry_count = certCount;
        //allocate certcount number of keymaster_blob_t type of space.
        //certificate and key shares the same data structure keymaster_blob_t
        keyboxCtx.keybox[keyitem].cert_chain.entries =
                (keymaster_blob_t *) malloc(
                        certCount * sizeof(keymaster_blob_t));
        if (!keyboxCtx.keybox[keyitem].cert_chain.entries) {
            cerr << "keyboxCtx.keybox[keyitem].cert_chain.entries NULL"
                    << keyitem << endl;
            inFile.close();
            exit(-1);
        }

        cert_entry = keyboxCtx.keybox[keyitem].cert_chain.entries;
        for (j = 0; j < certCount; j++) {
            //proces certificate format:  <Certificate format=”pem”>
            ProcessCertFormat(inFile);
            //process certificate content
            ProcessCertificate(inFile, cert_entry);
            //consume tag: /Certificate
            //ConsumeTag(inFile, "/Certificate");
            cert_entry++;
        }
        //consume tag: /CertificateChain
        ConsumeTag(inFile, "/CertificateChain");
        //consume tag: /Key
        ConsumeTag(inFile, "/Key");
    }
    //consume tag: /Keybox
    ConsumeTag(inFile, "/Keybox");
    //current Max keys in keybox is 2, one for ECDSA, another one for RSA
    if (!memcmp(keyboxCtx.device_id.data, myDeviceID,
                keyboxCtx.device_id.data_length)) {
        cout << "Found my device ID in XML keybox file! : " << myDeviceID
                << endl;

        UniquePtr < KmKeyProvision > keymaster_provision1(new KmKeyProvision);
        int result = -1;

        result = keymaster_provision1->KeyMasterProvisionInit();
        if (result) {
            cerr << "KeyMasterProvisionInit error" << result << endl;
            inFile.close();
            exit(-1);
        }

        result = keymaster_provision1->KeyMasterInstallKeybox(&keyboxCtx);
        if (result) {
            cerr << "KeyMasterInstallKeybox error" << result << endl;
            inFile.close();
            exit(-1);
        }
        cerr << "KeyMaster Attestation Key Provisioning success for KeyID"
                << myDeviceID << endl;
        cleanKeyBox(&keyboxCtx);
        inFile.close();
        exit(0);
    }
    cleanKeyBox(&keyboxCtx);
}

void InstallKeybox::Run(const char *myDeviceID) {
    ifstream inFile(mFilename.c_str());
    string input;
    int keyCount = 0;

    if (inFile.is_open()) {
        ConsumeTag(inFile, "?xml version=\"1.0\"?");
        ConsumeTag(inFile, "AndroidAttestation");
        keyCount = atol(ConsumeElement(inFile, "NumberOfKeyboxes").c_str());
        cout << "Number of keyboxes " << keyCount << endl;

        while ((keyCount--) && (keyCount < INT_MAX)) {
            ProcessOneXmlKeybox(inFile, myDeviceID);
        }

        //consume tag: /AndroidAttestation
        ConsumeTag(inFile, "/AndroidAttestation");
        cerr << "Keybox for '" << myDeviceID << "' not found!" << endl;
        inFile.close();
        exit(-1);
    } else {
        cerr << "ERROR: Unable to open xml keyfile!" << endl;
    }
    inFile.close();
    exit(0);
}
}  //end of namespace KM

int main(int argc, char **argv) {
    char *outFile = NULL;
    while (1) {
        cout << "usage: KmInstallKeybox security_level Keybox_file Device_ID"
             << endl;
        cout << "e.g.,: KmInstallKeybox 1 keybox.xml mfgID_xxxx_0000" << endl;
        if (argc < 4 || argc > 5) {
            cerr << "usage: " << argv[0] << " keyfile deviceID [outfile]"
                 << endl;
            cerr << "if outfile is given, output is written in binary form to the file.  Otherwise"
                 << endl;
            cerr << "the keybox is installed to the device" << endl;
            exit(-1);
        }

        if (argc == 5)
            outFile = argv[4];

        g_sec_level = (char) *argv[1];

        if (g_sec_level == '1' || g_sec_level == 3)
            break;
    }
    KM::InstallKeybox installer(argv[2], outFile);
    installer.Run(argv[3]);
    cout << "InstallKeybox is done!" << endl;
    return 0;
}

