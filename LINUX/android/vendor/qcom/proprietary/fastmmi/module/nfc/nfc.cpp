/*
 * Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <errno.h>
#include <hardware/nfc.h>
#include <hardware/hardware.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <memory>
#include <array>

#include "mmi_module.h"

/*=========================================================================*
*                            file scope local defnitions                   *
*==========================================================================*/
#define TRUE                                1
#define FALSE                               0

#define FILEHANLDEWORKAROUND

/* A temporary workaround for the NFC NCI HAL file handler bug, remove when HAL is fixed */
#ifdef FILEHANLDEWORKAROUND
static int fileHandle = 0;
#endif

enum                                                                // state machine states used in async thread
{
    NCI_HAL_INIT,
    NCI_HAL_WRITE,
    NCI_HAL_READ,
    NCI_HAL_ERROR
};

struct ndef_message_context                                         // context structure for NDEF message
{
    uint8_t             fNdefTextMessageFound;
    uint8_t             ndefMessageBlockNum;
    uint16_t            ndefTextMessageCopiedLen;
    uint16_t            ndefTextMessageLen;
    vector<uint8_t>     ndefTextMessage;
    string              ndefTextMessageInAscii;
};

pthread_t           nfc_thread_handle;                              // handle for async thread
sem_t               semaphore_async_thread;                         // semaphore to control state machine execution flow
sem_t               semaphore_command_timeout;                      // semaphore to wait for a response to be received

string              subCommand;                                     // can be either SUBCMD_MMI or SUBCMD_PCBA
string              testResultBuffer;                               // buffer that stores the test results


nfc_nci_device_t*   dev                         = nullptr;          // handle to NFC NCI HAL
static uint8_t      fHalOpened                  = FALSE;            // flag to indicate if HAL is opened
uint8_t             halState                    = NCI_HAL_INIT;     // indicate state of state machine
uint8_t             fThreadStarted              = FALSE;            // flag to indicate if the async thread has started
uint8_t             fCoreInitCommand            = FALSE;            // flag to indicate if the current command used is CORE INIT
uint8_t             fTagDetectCommand           = FALSE;            // flag to indicate if the current command used is tag detect
uint8_t             fWaitForNotification        = FALSE;            // flag to indicate whether or not to wait for a notification
uint8_t             fSaveNotificationBuffer     = FALSE;            // flag to indicate whether or not to save the notification
vector<uint8_t>     nfcCommandBuffer;                               // buffer to hold NFC NCI commands
vector<uint8_t>     responseBuffer;                                 // buffer to hold reponses
vector<uint8_t>     notificationBuffer;                             // buffer to hold notifications

const mmi_module_t* g_module;                                       // module passed in by main MMI thread

/*=========================================================================================================*
*                                              Function Defnitions                                         *
*=========================================================================================================*/


/*==========================================================================================================
FUNCTION
    nfc_hal_cback

DESCRIPTION
    callback function from HAL to indicate the status of an event

PARAMETERS
    uint8_t event   - HAL event
    uint8_t status  - status of the event

RETURN VALUE
    void

==========================================================================================================*/
static void nfc_hal_cback(uint8_t event, uint8_t status)
{
    switch(event)
    {
        case HAL_NFC_OPEN_CPLT_EVT:                                             // HAL open event
            if(status == HAL_NFC_STATUS_OK)
            {
                halState   = NCI_HAL_WRITE;                                     // ready to accept and send commands to HAL
                fHalOpened = TRUE;
                ALOGI("NFC - %s: HAL opened successfully, state changed to WRITE", __FUNCTION__);
            }
            else
            {
                ALOGE("NFC - %s: HAL open failed", __FUNCTION__);
                halState   = NCI_HAL_ERROR;
                fHalOpened = FALSE;
            }
#ifdef FILEHANLDEWORKAROUND
            close(fileHandle);
#endif
            sem_post(&semaphore_async_thread);
            break;

        case HAL_NFC_CLOSE_CPLT_EVT:                                            // HAL close event
            ALOGI("NFC - %s: HAL closed successfully", __FUNCTION__);
            break;

        default:
            ALOGE("NFC - %s: unhandled event %x", __FUNCTION__, event);
            break;
    }
}

/*==========================================================================================================
FUNCTION
    nfc_hal_data_cback

DESCRIPTION
    holds the data returned by HAL

PARAMETERS
    uint16_t    dataLen - length of the data returned from HAL
    uint8_t*    data    - data buffer returned from HAL

RETURN VALUE
    void

==========================================================================================================*/
static void nfc_hal_data_cback(uint16_t dataLen, uint8_t* data)
{
    if(fHalOpened == FALSE)                                                     // reject any incoming data until HAL is opened successfully
    {
        return;
    }
    if((dataLen == 0x00) || (data == nullptr))                                  // check for valid data
    {
        ALOGE("NFC - %s: invalid data or data length", __FUNCTION__);
        return;
    }

    if(fTagDetectCommand)                                                       // when the tag detect command is sent
    {
        if((data[0] == 0x61) && (data[1] == 0x05))                              // check for interface activation notification
        {
            fTagDetectCommand       = FALSE;
            fSaveNotificationBuffer = TRUE;
        }
        else                                                                    // do nothing with all other responses or notifications
        {
            fWaitForNotification    = TRUE;
        }
    }

    if(fCoreInitCommand)                                                        // when the core init command is sent
    {
        responseBuffer.assign(data, data + dataLen);

        fCoreInitCommand = FALSE;

        halState = NCI_HAL_READ;                                                // change to the state to READ to stop the timeout timer
        sem_post(&semaphore_async_thread);                                      // flag the semaphore for state machine to continue
    }
    else if(fWaitForNotification)
    {
        fWaitForNotification = FALSE;                                           // do nothing with the response and wait for notifications
    }
    else if(fSaveNotificationBuffer)                                            // check if there is a need to save notification packets
    {
        notificationBuffer.assign(data, data + dataLen);

        fSaveNotificationBuffer = FALSE;

        halState = NCI_HAL_READ;                                                // change to the state to READ to stop the timeout timer
        sem_post(&semaphore_async_thread);                                      // flag the semaphore for state machine to continue
    }
    else
    {
        halState = NCI_HAL_READ;                                                // change to the state to READ to stop the timeout timer
        sem_post(&semaphore_async_thread);                                      // flag the semaphore for state machine to continue
    }
}

/*==========================================================================================================
FUNCTION
    nfc_hal_open

DESCRIPTION
    opens a handle to NFC NCI HAL

PARAMETERS
    none

RETURN VALUE
    SUCCESS - if HAL handle opened successfully
    FAILED  - if HAL handle open failed

==========================================================================================================*/
static int8_t nfc_hal_open()
{
    int8_t              status                  = FAILED;
    const hw_module_t*  hw_module               = nullptr;                      // handle to hardware interface
    const string        NFC_NCI_HARDWARE_MODULE = "nfc_nci.nqx";

    status = hw_get_module(NFC_NCI_HARDWARE_MODULE.c_str(), &hw_module);        // obtain a handle to the hardware interface

    if(status == SUCCESS)
    {
        dev = make_shared<nfc_nci_device_t>().get();

        status = nfc_nci_open(hw_module, &dev);                                 // obtain a handle to HAL
        if(status != SUCCESS)
        {
            ALOGI("NFC - %s: call to nfc_nci_open() failed", __FUNCTION__);
        }
        else
        {
            dev->open(dev, nfc_hal_cback, nfc_hal_data_cback);                  // open NFC NCI HAL
            ALOGI("NFC - %s: HAL opened successfully", __FUNCTION__);
            sem_wait(&semaphore_async_thread);                                  // flag the semaphore to block state machine execution
        }
    }
    else
    {
       ALOGE("NFC - %s: call to hw_get_module() failed", __FUNCTION__);
    }

    return status;
}

/*==========================================================================================================
FUNCTION
    nfc_thread

DESCRIPTION
    an async thread that handles various states of the state machine

PARAMETERS
    void* args - not used in this function

RETURN VALUE
    void *

==========================================================================================================*/
static void* nfc_thread(void* args)
{
    while(TRUE)
    {
        ALOGI("NFC - %s: waiting for command or response", __FUNCTION__);
        sem_wait(&semaphore_async_thread);                                                  // state machine gets blocked here
        switch(halState)
        {
            case NCI_HAL_INIT:                                                              // initialise HAL
                if(nfc_hal_open() != SUCCESS)
                {
                    ALOGE("NFC - %s: call to nfc_hal_open() failed", __FUNCTION__);
                    halState    = NCI_HAL_ERROR;
                    fHalOpened  = FALSE;
                }
                else
                {
                    ALOGI("NFC - %s: HAL opened successfully", __FUNCTION__);
                    break;
                }

            case NCI_HAL_WRITE:                                                             // send commands to HAL
                if(dev != nullptr)
                {
                    if(nfcCommandBuffer.empty())                                            // check if the command buffer is valid
                    {
                        ALOGE("NFC - %s: invalid NFC NCI command", __FUNCTION__);
                        halState = NCI_HAL_ERROR;
                        break;
                    }

                    dev->write(dev, nfcCommandBuffer.size(), &nfcCommandBuffer[0]);         // send commands to HAL
                    ALOGI("NFC - %s: command sent to HAL successfully", __FUNCTION__);
                }
                else
                {
                    ALOGE("NFC - %s: invalid handle", __FUNCTION__);
                }
                break;

            case NCI_HAL_READ:                                                              // read data passed back from HAL
                ALOGI("NFC - %s: response read from HAL successfully", __FUNCTION__);
                sem_post(&semaphore_command_timeout);                                       // stop the timeout timer
                break;

            case NCI_HAL_ERROR:                                                             // error case
                ALOGE("NFC - %s: error case", __FUNCTION__);
                sem_post(&semaphore_command_timeout);
                break;

            default:
                ALOGE("NFC - %s: invalid state", __FUNCTION__);
                break;
        }
    }
}

/*==========================================================================================================
FUNCTION
    send_command_to_hal

DESCRIPTION
    sends NFC NCI commands to HAL

PARAMETERS
    const array<uint8_t, SIZE>& nciCommand - buffer of command to be sent

RETURN VALUE
    SUCCESS - if the commands are sent successfully
    FAILED  - if failed to send the commands

==========================================================================================================*/
template<size_t SIZE>
static int8_t send_command_to_hal(const array<uint8_t, SIZE>& nciCommand)
{
    struct timespec time_sec;
    int8_t          status                  = FAILED;
    const uint8_t   NFC_CMD_CMPL_TIMEOUT    = 15;                                       // time to wait for a response before time out

    do
    {
        nfcCommandBuffer.assign(nciCommand.data(), nciCommand.end());

        sem_post(&semaphore_async_thread);                                              // flag the semaphore for state machine to continue

        ALOGI("NFC - %s: waiting for response", __FUNCTION__);
        if (clock_gettime(CLOCK_REALTIME, &time_sec) == FAILED)                         // get the time in seconds
        {
            ALOGE("NFC - %s: error getting clock time", __FUNCTION__);
        }

        time_sec.tv_sec += NFC_CMD_CMPL_TIMEOUT;
        status           = sem_timedwait(&semaphore_command_timeout, &time_sec);        // start the timer and wait on a semaphore
        if(status == FAILED)
        {
            ALOGE("NFC - %s: nfc nci command timed out", __FUNCTION__);
            break;
        }

        halState = NCI_HAL_WRITE;                                                       // allow state machine to send subsequent commands
    }
    while(FALSE);

    return status;
}

/*==========================================================================================================
FUNCTION
    build_test_result

DESCRIPTION
    builds the buffer required to store the test results and output to screen or log file

PARAMETERS
    const print_type_t    option          - option to print result to screen, log the result or both
    const string&         resultString    - string to be added to the buffer

RETURN VALUE
    void

==========================================================================================================*/
static void build_test_result(const print_type_t option, const string& resultString)
{
    testResultBuffer.append(resultString);
    testResultBuffer.append("\n");

    g_module->cb_print(nullptr,
                       subCommand.c_str(),
                       testResultBuffer.c_str(),
                       testResultBuffer.length(),
                       option);                                                         // send buffer to main thread
}

/*==========================================================================================================
FUNCTION
    get_firmware_version

DESCRIPTION
    gets the firmware version on the NFCC from CORE INIT response

PARAMETERS
    none

RETURN VALUE
    void

==========================================================================================================*/
static void get_firmware_version()
{
    if(!responseBuffer.empty())
    {
        stringstream    fimrwareVersionStream;
        string          firmwareVersionString;

        fimrwareVersionStream << "Firmware version: 10."                                        // build string for firmware version
                              << hex
                              << static_cast<int>(responseBuffer[responseBuffer.size() - 2])    // cast to int because uint8_t is quivalent to unsigned char
                              << "."
                              << hex
                              << static_cast<int>(responseBuffer[responseBuffer.size() - 1]);

        firmwareVersionString = fimrwareVersionStream.str();                                    // convert stream to string

        build_test_result(PRINT_DATA, firmwareVersionString);                                   // add to the test result
    }
    else
    {
        ALOGE("NFC - %s: failed to get firmware version\n", __FUNCTION__);
    }
}

/*==========================================================================================================
FUNCTION
    send_core_commands

DESCRIPTION
    sends CORE RESET and CORE INIT commands to NFCC

PARAMETERS
    none

RETURN VALUE
    SUCCESS - if commands are sent successfully
    FAILED  - if failed to send the commands

==========================================================================================================*/
static int8_t send_core_commands()
{
    const array<uint8_t, 4> nfc_core_reset  = {0x20, 0x00, 0x01, 0x01};
    const array<uint8_t, 3> nfc_core_init   = {0x20, 0x01, 0x00};

    int8_t status                   = FAILED;

    do
    {
        status = send_command_to_hal(nfc_core_reset);
        if(status == FAILED)
        {
            ALOGE("NFC - %s: failed to send NFC Core Reset command", __FUNCTION__);
            break;
        }

        fCoreInitCommand    = TRUE;                                                     // flag to save the CORE INIT response
        status              = send_command_to_hal(nfc_core_init);
        if(status == FAILED)
        {
            ALOGE("NFC - %s: failed to send NFC Core Reset command", __FUNCTION__);
            break;
        }
        get_firmware_version();
    }
    while(FALSE);

    return status;
}

/*==========================================================================================================
FUNCTION
    detect_tag

DESCRIPTION
    tries to detect an NFC Forum Type 2 tag

PARAMETERS
    none

RETURN VALUE
    SUCCESS - type 2 tag is detected successfully
    FAILED  - failed to detect a type 2 tag

==========================================================================================================*/
static int8_t detect_tag()
{
    const array<uint8_t, 28> nfc_detect_tag =
                    {0x21, 0x03, 0x19, 0x0C, 0x00, 0x01, 0x01, 0x01, 0x02, 0x01, 0x03, 0x01, 0x05, 0x01,
                     0x80, 0x01, 0x81, 0x01, 0x82, 0x01, 0x83, 0x01, 0x85, 0x01, 0x06, 0x01, 0x70, 0x01};

    int8_t status               = FAILED;
    fTagDetectCommand           = TRUE;

    const string tagDetected          ("Type 2 tag detected!");
    const string tagNotDetected       ("Type 2 tag not detected!");

    const string positionTagMessage   ("Please position the tag for read");
    build_test_result(PRINT, positionTagMessage);                                               // add to the test result

    do
    {
        status = send_command_to_hal(nfc_detect_tag);
        if(status == FAILED)
        {
            ALOGE("NFC - %s: failed to send NFC Tag Discovery command", __FUNCTION__);
            break;
        }

        if(!notificationBuffer.empty() && (notificationBuffer.at(5) == 0x02))                   // check for valid notification buffer from type 2 tag
        {
            build_test_result(PRINT_DATA, tagDetected);                                         // add to the test result
        }
        else
        {
            build_test_result(PRINT_DATA, tagNotDetected);                                      // add to the test result
            status = FAILED;
        }
    }
    while(FALSE);

    return status;
}

/*==========================================================================================================
FUNCTION
    get_ndef_message

DESCRIPTION
    gets and determines if the NDEF message on the detected tag is valid

PARAMETERS
    ndef_message_context& messageContext - structure containing information on the NDEF message

RETURN VALUE
    SUCCESS - if the NDEF message contains valid information
    FAILED  - if the NDEF message does not contain valid information

==========================================================================================================*/
static int8_t get_ndef_message(ndef_message_context& messageContext)
{
    int8_t          status                          = SUCCESS;
    uint8_t         bufferIndex                     = 0;

    /* values obtained from NFCForum T2T Specification */
    const uint8_t   T2T_NDEF_MESSAGE_TEXT_TYPE      = 0x54U;
    const uint8_t   T2T_NDEF_MESSAGE_START_OF_TEXT  = 0x02U;
    const uint8_t   T2T_NDEF_MESSAGE_DATA_PRESENT   = 0xE1U;
    const uint8_t   T2T_NDEF_MESSAGE_TLV_PRESENT    = 0x03U;
    const uint8_t   T2T_NDEF_MESSAGE_TERMINATOR_TLV = 0xFEU;

    const string    onlyTextMessage("only data of type text is supported!");

    if(notificationBuffer.empty())                                                  // check for valid notification buffer
    {
        ALOGE("NFC - %s: failed to get notification from tag", __FUNCTION__);

        status = FAILED;
        return status;
    }

    do
    {
        if(messageContext.ndefMessageBlockNum == 0x00)                              // first memory block on type 2 tag
        {
            if(notificationBuffer.at(15) == T2T_NDEF_MESSAGE_DATA_PRESENT)          // check for NDEF message
            {
                ALOGI("NFC - %s: NDEF data present", __FUNCTION__);
            }
            else
            {
                ALOGE("NFC - %s: NDEF data not present", __FUNCTION__);

                status = FAILED;
            }
            break;
        }

        if(messageContext.ndefMessageBlockNum == 0x01)                              // second memory block on type 2 tag
        {
            if((notificationBuffer.at(3) == T2T_NDEF_MESSAGE_TLV_PRESENT) &&        // check for message TLV
               (notificationBuffer.at(4) != 0x00))                                  // check for NDEF message
            {
                ALOGI("NFC - %s: NDEF message TLV present", __FUNCTION__);
            }
            else
            {
                ALOGE("NFC - %s: NDEF message TLV not present", __FUNCTION__);

                status = FAILED;
                break;
            }

            messageContext.ndefTextMessageLen = notificationBuffer.at(7);

            if(notificationBuffer.at(8) != T2T_NDEF_MESSAGE_TEXT_TYPE)              // check for type of data, only "text" is supported
            {
                ALOGE("NFC - %s: %s", __FUNCTION__, onlyTextMessage.c_str());
                build_test_result(PRINT, onlyTextMessage);

                status = FAILED;
                break;
            }

            if(notificationBuffer.at(9) == T2T_NDEF_MESSAGE_START_OF_TEXT)          // check for "start of text" symbol
            {
                ALOGI("NFC - %s: start of text detected", __FUNCTION__);
            }

            bufferIndex = 9;                                                        // the 10th byte is the start of text data
        }
        else                                                                        // for subsequent memory blocks on type 2 tag
        {
            bufferIndex = 3;                                                        // the 4th byte is the start of text data
        }

        /* save the text data from the notification packet until the terminator TLV is found */
        while((messageContext.ndefTextMessageCopiedLen < messageContext.ndefTextMessageLen) &&
              (notificationBuffer[bufferIndex] != T2T_NDEF_MESSAGE_TERMINATOR_TLV) &&
              (notificationBuffer[bufferIndex] != 0x00))
        {
            messageContext.ndefTextMessage.push_back(notificationBuffer[bufferIndex]);
            messageContext.ndefTextMessageCopiedLen++;
            bufferIndex++;
        }

        if(notificationBuffer.at(bufferIndex) == T2T_NDEF_MESSAGE_TERMINATOR_TLV)   // indicate that the terminator TLV is found
        {
            ALOGI("NFC - %s: terminator TLV found", __FUNCTION__);
            messageContext.fNdefTextMessageFound = TRUE;
        }
    }
    while(FALSE);

    return status;
}

/*==========================================================================================================
FUNCTION
    packet_bytes_to_ascii

DESCRIPTION
    converts the message buffer to ascii text

PARAMETERS
    ndef_message_context& messageContext - structure containing information on the NDEF message

RETURN VALUE
    void

==========================================================================================================*/
static void packet_bytes_to_ascii(ndef_message_context& messageContext)
{
    stringstream    textMessageStream;

    /* first byte is the "start of text" symbol, second and third bytes are "language symbols" */
    uint16_t    actualTextLen = (messageContext.ndefTextMessageLen) - 3;

    if(messageContext.ndefTextMessage.empty())                                                          // check for valid NDEF message received
    {
        ALOGE("NFC - %s: invalid message received", __FUNCTION__);
        return;
    }

    /* start copying from the fourth byte, first byte is the "start of text" symbol, second and third bytes are "language symbols" */
    for(uint16_t i = 0; i < actualTextLen; i++)
    {
        textMessageStream << messageContext.ndefTextMessage[i + 3];                                     // convert buffer bytes to ascii text
    }

    messageContext.ndefTextMessageInAscii = textMessageStream.str();                                    // convert stream to string

    ALOGI("NFC - %s: %s", __FUNCTION__, messageContext.ndefTextMessageInAscii.c_str());
}

/*==========================================================================================================
FUNCTION
    read_ndef_message_data

DESCRIPTION
    reads blocks of the tag's memory and tries to retrieve the data present in the NDEF message

PARAMETERS
    ndef_message_context& messageContext - structure containing information on the NDEF message

RETURN VALUE
    SUCCESS - retrieved the data successully
    FAILED  - failed to retrieve the data

==========================================================================================================*/
static int8_t read_ndef_message_data(ndef_message_context& messageContext)
{
    array<uint8_t, 5>   nfc_get_tag_data                    = {0x00, 0x00, 0x02, 0x30, 0x00};

    int8_t              status                              = SUCCESS;
    const uint8_t       T2T_NDEF_MESSAGE_BYTES_PER_BLOCK    = 0x04U;

    /* keep reading from increasing blocks of memory until the complete text is found */
    while(!(messageContext.fNdefTextMessageFound) && (status == SUCCESS))
    {
        fWaitForNotification    = TRUE;
        fSaveNotificationBuffer = TRUE;
        status                  = send_command_to_hal(nfc_get_tag_data);
        if(status == FAILED)
        {
            break;
        }

        status                  = get_ndef_message(messageContext);

        nfc_get_tag_data[4]    += T2T_NDEF_MESSAGE_BYTES_PER_BLOCK;
        messageContext.ndefMessageBlockNum++;
    }

    packet_bytes_to_ascii(messageContext);                                                          // convert buffer bytes to ascii text

    return status;
}

/*==========================================================================================================
FUNCTION
    compare_text_messages

DESCRIPTION
    compares the text data configured in .cfg file with the one read from tag

PARAMETERS
    const string&               stringToCompare       - the configured string to compare against
    ndef_message_context&       messageContext          - structure containing information on the NDEF message

RETURN VALUE
    void

==========================================================================================================*/
static void compare_text_messages(const string& stringToCompare, ndef_message_context& messageContext)
{
    int8_t          compareStatus = 0;
    string          configuredTextMessage    ("Configured text message: \n");
    string          textMessageFromTag       ("Text message from tag: \n");
    const string    textMessagesMatch        ("text messages match!");
    const string    textMessagesNotMatch     ("text messages do not match!");
    const string    noMessageReceivedMessage ("no message received from tag!");

    do
    {
        if(messageContext.ndefTextMessageInAscii.empty())                                           // check for valid NDEF message data
        {
            ALOGE("NFC - %s: %s\n", __FUNCTION__, noMessageReceivedMessage.c_str());
            build_test_result(PRINT, noMessageReceivedMessage);
            break;
        }

        /* add the string configured in the .cfg file to test result */
        ALOGI("NFC - %s: configured text message - %s", __FUNCTION__, stringToCompare.c_str());
        configuredTextMessage.append(stringToCompare);
        build_test_result(PRINT_DATA, configuredTextMessage);

        /* add the string obtained from the tag to test result */
        ALOGI("NFC - %s: text message from tag - %s", __FUNCTION__,
               messageContext.ndefTextMessageInAscii.c_str());
        textMessageFromTag.append(messageContext.ndefTextMessageInAscii);
        build_test_result(PRINT_DATA, textMessageFromTag);

        /* compare both strings */
        compareStatus = stringToCompare.compare(messageContext.ndefTextMessageInAscii);

        if(compareStatus == 0)
        {
            ALOGI("NFC - %s: %s", __FUNCTION__, textMessagesMatch.c_str());
            build_test_result(PRINT, textMessagesMatch);                                            // add result of comparison to test result
        }
        else
        {
            ALOGI("NFC - %s: %s", __FUNCTION__, textMessagesNotMatch.c_str());
            build_test_result(PRINT, textMessagesNotMatch);                                         // add result of comparison to test result
        }
    }
    while(FALSE);
}

/*==========================================================================================================
FUNCTION
    module_init

DESCRIPTION
    module_init called from main thread

PARAMETERS


RETURN VALUE


==========================================================================================================*/
static int32_t module_init(const mmi_module_t* module,
                           unordered_map < string, string > &params)
{
    ALOGI("NFC - %s", __FUNCTION__);
    if(module == nullptr)
    {
        ALOGE("%s nullptr point received", __FUNCTION__);
        return FAILED;
    }

    int status = SUCCESS;

#ifdef FILEHANLDEWORKAROUND
    int fileHandle = open(MMI_PATH_CONFIG, O_RDONLY);
#endif

    if(!fThreadStarted)
    {
        if(sem_init(&semaphore_async_thread, 0, 1) != SUCCESS)
        {
            ALOGE("NFC - %s: semaphore_async_thread creation failed", __FUNCTION__);
        }
        if(sem_init(&semaphore_command_timeout, 0, 0) != SUCCESS)
        {
            ALOGE("NFC - %s: semaphore_command_timeout creation failed", __FUNCTION__);
        }

        if(pthread_create(&nfc_thread_handle, nullptr, &nfc_thread, nullptr) != SUCCESS)    // create async thread for state machine
        {
            ALOGE("NFC - %s: thread creation failed", __FUNCTION__);
        }
        else
        {
            ALOGI("NFC - %s: thread created successfully", __FUNCTION__);
            fThreadStarted = TRUE;
        }
    }

    ALOGE("NFC - %s: NFC initialised successfully", __FUNCTION__);

    return status;
}

/*==========================================================================================================
FUNCTION
    module_deinit

DESCRIPTION
    module_deinit called from main thread

PARAMETERS


RETURN VALUE


==========================================================================================================*/
static int32_t module_deinit(const mmi_module_t* module)
{
    ALOGI("NFC - %s", __FUNCTION__);

    int8_t status = FAILED;

    if(module == nullptr)
    {
        ALOGE("%s nullptr point received", __FUNCTION__);
        return FAILED;
    }

    status = send_core_commands();                                                          // send CORE commands

    ALOGI("NFC FTM : closing NCI HAL");
    if(dev != nullptr)
    {
        dev->close(dev);
    }

    return status;
}

/*==========================================================================================================
FUNCTION
    module_stop

DESCRIPTION
    module_stop called from main thread

PARAMETERS


RETURN VALUE


==========================================================================================================*/
static int32_t module_stop(const mmi_module_t* module)
{
    ALOGI("NFC - %s", __FUNCTION__);
    if(module == nullptr)
    {
        ALOGE("%s nullptr point received", __FUNCTION__);
        return FAILED;
    }

    kill_thread(module->run_pid);

    return SUCCESS;
}

/*==========================================================================================================
FUNCTION
    module_run

DESCRIPTION
    module_run called from main thread

PARAMETERS


RETURN VALUE


==========================================================================================================*/
/**
* Before call Run function, caller should call module_init first to initialize the module.
* the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
*
*/
static int32_t module_run(const mmi_module_t* module, const char* cmd,
                          unordered_map < string, string > &params)
{
    ALOGI("NFC - %s: %s", __FUNCTION__, cmd);

    int8_t status                           = FAILED;
    g_module                                = module;

    ndef_message_context  messageContext    = {};

    string      configuredString            = params["data"];                           // string configured in .cfg file

    testResultBuffer.erase();                                                           // clear the test result buffer

    do
    {
        if(!module)                                                                     // check for valid module handle
        {
            ALOGE("NFC - %s: invalid module", __FUNCTION__);
            status = FAILED;
            break;
        }

        if(!cmd)                                                                        // check for valid command
        {
            ALOGE("NFC - %s: invalid command", __FUNCTION__);
            status = FAILED;
            break;
        }

        if(!string(cmd).compare(SUBCMD_MMI))
        {
            subCommand = SUBCMD_MMI;
        }
        else if(!string(cmd).compare(SUBCMD_PCBA))
        {
            subCommand = SUBCMD_PCBA;
        }
        else
        {
            ALOGE("NFC - %s: invalid command", __FUNCTION__);
            status = FAILED;
            break;
        }

        if(fHalOpened == FALSE)
        {
            const string openHalFailed("Failed to open HAL, exiting...");
            build_test_result(PRINT_DATA, openHalFailed);                               // add to the test result
            break;
        }

        status = send_core_commands();                                                  // send CORE commands
        if(status == FAILED)
        {
            ALOGE("NFC - %s: send_core_commands() failed", __FUNCTION__);
            status = FAILED;
            break;
        }

        status = detect_tag();                                                          // detect type 2 tag
        if(status == FAILED)
        {
            ALOGE("NFC - %s: detect_tag() failed", __FUNCTION__);
            status = FAILED;
            break;
        }

        status = read_ndef_message_data(messageContext);                                // read the NDEF message on type 2 tag
        if(status == FAILED)
        {
            ALOGE("NFC - %s: read_ndef_message_data() failed", __FUNCTION__);
            status = FAILED;
            break;
        }

        compare_text_messages(configuredString, messageContext);                        // compare the texts
    }
    while(FALSE);

    sleep(3);                                                                           // allow output to stay on screen for MMI

   /** Default RUN mmi*/
    return status;
}

/**
* Methods must be implemented by module.
*/
static struct mmi_module_methods_t module_methods = {
    .module_init    = module_init,
    .module_deinit  = module_deinit,
    .module_run     = module_run,
    .module_stop    = module_stop,
};

/**
* Every mmi module must have a data structure named MMI_MODULE_INFO_SYM
* and the fields of this data structure must be initialize in strictly sequence as definition,
* please don't change the sequence as g++ not supported in CPP file.
*/
mmi_module_t MMI_MODULE_INFO_SYM = {
    .version_major              = 1,
    .version_minor              = 0,
    .name                       = "NFC",
    .author                     = "Qualcomm Technologies, Inc.",
    .methods                    = &module_methods,
    .module_handle              = nullptr,
    .supported_cmd_list         = nullptr,
    .supported_cmd_list_size    = 0,
    .cb_print                   = nullptr, /**it is initialized by mmi agent*/
    .run_pid                    = -1,
};
