/*==================================================================
 *
 * FILE:        deviceprogrammer_firehose.c
 *
 * DESCRIPTION:
 *   
 *
 *        Copyright © 2013-2016 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_firehose.c#2 $ 
 *   $DateTime: 2016/01/19 02:26:29 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2016-01-19   sb      Change the attribute “storagedrive” for “physical_partition_number”
 * 2015-05-11   sb      Use crypto sha for ddr version only
 * 2015-04-01   sb      Use crypto bam for hash calculation
 * 2014-10-15   sb      Updated for 8909
 * 2014-03-03   dks     Added MMC FFU Support
 * 2013-06-03   ah      Added legal header
 * 2013-05-31   ab      Initial checkin
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "crc.h"
#include "stringl.h"
#include "deviceprogrammer_utils.h"
#include "deviceprogrammer_storage_device.h"
#include "deviceprogrammer_xml_parser.h"
#include "deviceprogrammer_security.h"
#include "deviceprogrammer_bsp.h"
#include "deviceprogrammer_firehose.h"

#include "qhsusb_al_bulk.h"
#include "qusb_al_bulk.h"
#include "qusb_common.h"
#include "boot_sahara.h"
#include "ClockBoot.h"

//#include "CeTL_sha2_sw.h"


#define FIREHOSE_XML_BUFFER_SIZE 4096
#define FIREHOSE_LOG_BUFFER_SIZE 4096
#define FIREHOSE_TEMP_BUFFER_SIZE 512
#define FIREHOSE_TEMP_LOG_BUFFER_SIZE 256
#define FIREHOSE_TEMP_ATTRIBUTE_BUFFER_SIZE 256
#define FIREHOSE_DEBUG_LOG_MESSAGE_SIZE 32
#define FIREHOSE_NUM_DEBUG_LOG_MESSAGES 32
#define FIREHOSE_SIGNED_HASH_PACKET_SIZE 8192
#define FIREHOSE_NUM_HASHES_IN_HASH_TABLE 256

typedef enum response_t {
    NAK,
    ACK
} response_t;

typedef enum firehose_error_t {
    FIREHOSE_SUCCESS,
    FIREHOSE_ERROR,
    FIREHOSE_TRANSPORT_ERROR,
    FIREHOSE_STORAGE_ERROR,
    FIREHOSE_VALIDATION_ERROR
} firehose_error_t;

typedef struct
{
    // Buffer which is passed to USB
    // Using a constant value instead of a #define since that would
    // give the impression that this value is flexible. It is currently not.
    byte* channel_buffer[2];

    // Main buffer which holds the XML file to be parsed
    byte xml_buffer[FIREHOSE_XML_BUFFER_SIZE];

    // Transmit buffer for any outgoing XML or data
    byte* tx_buffer;

    // XML reader
    xml_reader_t xml_reader;

    // XML writer
    xml_writer_t xml_writer;

    // Channel variables
    uint32 channel_error_code;
    SIZE_T channel_bytes_read;
    boolean channel_read_completed;
    boolean channel_write_completed;
    SIZE_T channel_bytes_written;
    SIZE_T channel_total_bytes_written;
    SIZE_T channel_total_bytes_read;
    SIZE_T channel_buffer_capacity;
    SIZE_T channel_num_errors;
    SIZE_T num_packets_received;
    boolean channel_buffer_queued;
    int channel_buffer_to_use;

    // Variables for validation mode
    boolean validation_enabled;
    boolean validation_signed_hash_received;
    byte validation_hash_buffer[FIREHOSE_NUM_HASHES_IN_HASH_TABLE][32];
    SIZE_T validation_hash_buffer_num_hashes;
    SIZE_T validation_hash_buffer_index;
    SIZE_T validation_hash_buffer_capacity;
    SIZE_T validation_hash_buffer_new_capacity;
    SIZE_T validation_table_count;
    uint8 validation_hash_value[32];

    // On target debug log buffer
    byte debug_log_buffer[FIREHOSE_NUM_DEBUG_LOG_MESSAGES][FIREHOSE_DEBUG_LOG_MESSAGE_SIZE];
    SIZE_T debug_log_position;

    // Storage medium
    storage_device_t emmc_storage;

    // Performance measurement
    boolean skip_storage_writes;
    boolean hash_in_non_secure;
    boolean verbose;
    unsigned int ack_raw_data_every;
    long start_time;
    long end_time;
} firehose_protocol_t;

// These buffers are being allocated separately so as to make them easy to relocate
// because these are currently accessed directly by the hardware
#pragma arm section zidata = ".bss.CHANNEL_BUFFER_SECTION"
byte main_channel_buffer_A[FIREHOSE_CHANNEL_BUFFER_SIZE];
byte main_channel_buffer_B[FIREHOSE_CHANNEL_BUFFER_SIZE];
byte transmit_buffer[FIREHOSE_TX_BUFFER_SIZE];
#pragma arm section zidata

firehose_protocol_t fh;

static const char xml_header[] = "<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>";

extern uint32 Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$Base;
extern uint32 Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$ZI$$Length;

/* Temporary bringup function to clear the USB Buffers. */
void deviceprogrammer_zi_buffers (void) {
#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
        // Since we don't know size or start of USB ZI, zero from known positions DLOAD to SCL_DDR_BOOT_LOG_META_INFO_BASE
    // This would be zeroed next anyway on regular SBL loading
        void *buf_start = (void *)SCL_DLOAD_BUFFERS_BASE;
        SIZE_T  size = SCL_SBL1_DDR_BOOT_LOG_META_INFO_BASE - SCL_DLOAD_BUFFERS_BASE;
//        SIZE_T  size = 0;
#else
        //void *buf_start = (void *)SBL1_DEVICEPROGRAMMER_USB_ZI_BASE;
        void *buf_start = (void *)&Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$Base;
        // HACK -- We don't know size of USB ZI, .map file says 0xa284, pick 0x10000 to be safe.
        //SIZE_T  size = 0x10000;
        //SIZE_T  size = 2 * FIREHOSE_CHANNEL_BUFFER_SIZE;
        SIZE_T  size = (unsigned int)&Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$ZI$$Length;
#endif

        memset (buf_start, 0, size);
}

static void firehoseResetValidationVariables() {
    fh.num_packets_received = 0;

    fh.validation_hash_buffer_num_hashes = 0;
    fh.validation_hash_buffer_capacity = sizeof(fh.validation_hash_buffer) /
                        sizeof(fh.validation_hash_value);
    fh.validation_hash_buffer_new_capacity = fh.validation_hash_buffer_capacity;

    fh.validation_hash_buffer_index = 0;
    fh.validation_table_count = 0;
    fh.validation_signed_hash_received = FALSE;
}

void initFirehoseProtocol() {

#ifdef FEATURE_FIREHOSE_VALIDATION_ENABLED
    fh.validation_enabled = TRUE;
#else
    fh.validation_enabled = FALSE;
#endif

    fh.tx_buffer = transmit_buffer;
    // Channel variables
    fh.channel_buffer[0] = main_channel_buffer_A;
    fh.channel_buffer[1] = main_channel_buffer_B;
    fh.channel_error_code = 0;
    fh.channel_bytes_read = 0;
    fh.channel_read_completed = FALSE;
    fh.channel_total_bytes_read = 0;
    fh.channel_write_completed = FALSE;
    fh.channel_bytes_written = 0;
    fh.channel_total_bytes_written = 0;
    fh.channel_buffer_capacity = FIREHOSE_CHANNEL_BUFFER_SIZE;
    fh.channel_num_errors = 0;
    fh.channel_buffer_queued = FALSE;
    fh.channel_buffer_to_use = 0;

    fh.debug_log_position = 0;

    xmlInitReader(&(fh.xml_reader), NULL, 0);
    xmlInitWriter(&(fh.xml_writer),
                 fh.tx_buffer,
                 FIREHOSE_TX_BUFFER_SIZE);
    init_storage_device_struct(&(fh.emmc_storage));

    fh.skip_storage_writes = FALSE;
    fh.hash_in_non_secure = FALSE;
    fh.verbose = FALSE;
    fh.ack_raw_data_every = 0;

    firehoseResetValidationVariables();
}

boolean initFirehoseStorage() {
    return init_storage_device_hw(&(fh.emmc_storage));
}

boolean isValidationMode(void) {
    return fh.validation_enabled;
}

void debugMessage(const char* format, ...) {
    char *temp_log_buffer = (char *) fh.debug_log_buffer[fh.debug_log_position];
    SIZE_T buffer_space = FIREHOSE_DEBUG_LOG_MESSAGE_SIZE;

    va_list args;
    va_start (args, format);

    vsnprintf(temp_log_buffer,
              buffer_space,
              format,
              args);
    va_end (args);
    fh.debug_log_position = (fh.debug_log_position + 1) % FIREHOSE_NUM_DEBUG_LOG_MESSAGES;
}

void packet_handle_incoming_buf (uint32 bytes_read, uint32 err_code) {
    debugMessage("cback %d err %d", bytes_read, err_code);
    fh.channel_error_code = err_code;
    if (err_code != BULK_SUCCESS) {
        fh.channel_bytes_read = 0;
        fh.channel_num_errors += 1;
        //qhsusb_al_bulk_cancel_rx();
    }
    else {
        fh.channel_bytes_read = bytes_read;
        fh.num_packets_received += 1;
    }
    fh.channel_total_bytes_read += bytes_read;
    fh.channel_read_completed = TRUE;
    return;
}

firehose_error_t readChannel(byte** packet_ptr, SIZE_T bytes_to_read, SIZE_T *bytes_read, boolean queue_only) {
    // Since the 'channel' variables are directly manipulated here, they must not be used by clients such
    // as handleProgram directly. Only the copies made by this function (packet_ptr, bytes_read) must
    // be used by those client functions

    if (fh.channel_buffer_queued == FALSE) {
        debugMessage("queue %d", bytes_to_read);
#ifndef FEATURE_FIREHOSE_QUSB
        qhsusb_al_bulk_receive(fh.channel_buffer[fh.channel_buffer_to_use], bytes_to_read, packet_handle_incoming_buf, &fh.channel_error_code);
#else
        qusb_al_bulk_receive(fh.channel_buffer[fh.channel_buffer_to_use], bytes_to_read, packet_handle_incoming_buf, &fh.channel_error_code);
#endif
        if (fh.channel_error_code != BULK_SUCCESS)
            return FIREHOSE_TRANSPORT_ERROR;
        fh.channel_read_completed = FALSE;
        fh.channel_bytes_read = 0;
        fh.channel_buffer_queued = TRUE;
    }
    if (queue_only == TRUE)
        return FIREHOSE_SUCCESS;

    while (fh.channel_read_completed == FALSE) {
#ifndef FEATURE_FIREHOSE_QUSB
        qhsusb_al_bulk_poll();
#else
        qusb_al_bulk_poll();
#endif
    }

    if (fh.channel_bytes_read > fh.channel_buffer_capacity) {
		debugMessage("read %i bufsize %i", fh.channel_bytes_read, fh.channel_buffer_capacity);
        return FIREHOSE_TRANSPORT_ERROR;
    }

    *bytes_read = fh.channel_bytes_read;
    *packet_ptr = fh.channel_buffer[fh.channel_buffer_to_use];
    fh.channel_buffer_queued = FALSE;
    fh.channel_buffer_to_use = 1 - fh.channel_buffer_to_use;

    if (fh.channel_error_code != BULK_SUCCESS) {
        return FIREHOSE_TRANSPORT_ERROR;
    }

    return FIREHOSE_SUCCESS;
}

void packet_handle_outgoing_buf(uint32 bytes_written, uint32 err_code) {
    fh.channel_bytes_written = bytes_written;
    fh.channel_total_bytes_written += bytes_written;
    fh.channel_error_code = err_code;
    fh.channel_write_completed = TRUE;
    return;
}

firehose_error_t writeChannel(byte* buffer, SIZE_T length) {
    fh.channel_write_completed = FALSE;
#ifndef FEATURE_FIREHOSE_QUSB
    qhsusb_al_bulk_transmit(buffer, length, packet_handle_outgoing_buf, &fh.channel_error_code);
#else
    qusb_al_bulk_transmit(buffer, length, packet_handle_outgoing_buf, &fh.channel_error_code);
#endif
    while (fh.channel_write_completed == FALSE) {
#ifndef FEATURE_FIREHOSE_QUSB
        qhsusb_al_bulk_poll();
#else
        qusb_al_bulk_poll();
#endif
    }
    return FIREHOSE_SUCCESS;
}

firehose_error_t sendXMLBuffer() {
    firehose_error_t retval = writeChannel(fh.xml_writer.buffer, fh.xml_writer.write_position);
    xmlWriterReset(&fh.xml_writer);
    return retval;
}

void logMessage(const char* format, ...) {
    int retval;
    int written;
    char temp_log_buffer[FIREHOSE_TEMP_LOG_BUFFER_SIZE] = {0};
    SIZE_T buffer_space = sizeof(temp_log_buffer);

    va_list args;
    va_start (args, format);

    retval = vsnprintf(temp_log_buffer,
                       buffer_space,
                       format,
                       args);
    va_end (args);
    written = MIN(retval, buffer_space);

    xmlWriterRawWriteFromBuffer(&fh.xml_writer,
                                (byte *) xml_header,
                                strlen(xml_header));
    xmlWriterRawStartTagNoAttributes(&fh.xml_writer, "data");

    xmlWriterRawStartTag(&fh.xml_writer, "log");
    xmlWriterRawWriteAttributeValueFromBuffer(&fh.xml_writer,
                                              "value",
                                              (byte *) temp_log_buffer,
                                              written);

    xmlWriterRawCloseTag(&fh.xml_writer);
    xmlWriterRawCloseTagName(&fh.xml_writer, "data");

    sendXMLBuffer();
}

void logDigest(char* prefix, byte* digest, SIZE_T length) {
    int i = 0;
    int written;
    char log_string[FIREHOSE_TEMP_BUFFER_SIZE] = { 0 };

    if (sizeof(log_string) < ((length * 2) + strlen(prefix) + 1)) {
        return;
    }

    written = snprintf(log_string,
                       sizeof(log_string),
                       "%s",
                       prefix);
    while (written < sizeof(log_string) && i < length) {
        written += snprintf(log_string + written,
                            sizeof(log_string) - written,
                            "%02X",
                            digest[i]);
        i++;
    }

    logMessage(log_string);
}

firehose_error_t sendResponseFields(response_t response, int n_args, ...) {
    va_list args;
    int i;

    char format;
    int int_arg;
    SIZE_T size_arg;
    char char_arg;
    char* string_arg;
    char *attribute_name;
    char temp_attribute_buffer[FIREHOSE_TEMP_ATTRIBUTE_BUFFER_SIZE] = {0};
    int written;

    xmlWriterRawWriteFromBuffer(&fh.xml_writer,
                                (byte *) xml_header,
                                strlen(xml_header));
    xmlWriterRawStartTagNoAttributes(&fh.xml_writer, "data");

    xmlWriterRawStartTag(&fh.xml_writer, "response");
    xmlWriterRawWriteAttribute(&fh.xml_writer,
                                   "value",
                                   (response == ACK) ? "ACK" : "NAK");
    /*
    xmlWriterRawWriteAttribute(
            &fh.xml_writer,
            "rawmode",
            (fh.raw_data_mode == TRUE) ? "true" : "false");
    */

    va_start (args, n_args);
    for (i = 0; i < n_args; i++) {
        attribute_name = va_arg(args, char *);
        format = (char) va_arg(args, int);
        switch(format) {
            case 's':
                string_arg = va_arg(args, char *);
                written = snprintf(temp_attribute_buffer,
                                   sizeof(temp_attribute_buffer),
                                   "%s",
                                   string_arg);
                break;
            case 'd':
                int_arg = va_arg(args, int);
                written = snprintf(temp_attribute_buffer,
                                   sizeof(temp_attribute_buffer),
                                   "%d",
                                   int_arg);
                break;
            case 't':
                size_arg = va_arg(args, SIZE_T);
                written = snprintf(temp_attribute_buffer,
                                   sizeof(temp_attribute_buffer),
                                   SIZE_T_FORMAT,
                                   size_arg);
                break;
            case 'c':
                char_arg = (char) va_arg(args, int);
                written = snprintf(temp_attribute_buffer,
                                   sizeof(temp_attribute_buffer),
                                   "%c",
                                   char_arg);
                break;
            default:
                va_end (args);
                return FIREHOSE_ERROR;
        }
        if (written >= sizeof(temp_attribute_buffer)) {
            va_end (args);
            return FIREHOSE_ERROR;
        }
        xmlWriterRawWriteAttributeValueFromBuffer(
            &fh.xml_writer,
            attribute_name,
            (byte *) temp_attribute_buffer,
            written);
    }
    va_end (args);
    xmlWriterRawCloseTag(&fh.xml_writer);
    xmlWriterRawCloseTagName(&fh.xml_writer, "data");

    sendXMLBuffer();
    return FIREHOSE_SUCCESS;
}

firehose_error_t sendResponse(response_t response) {
    return sendResponseFields(response, 0);
}

void firehoseResetValidation() {
    firehoseResetValidationVariables();
    logMessage("Reset validation state. Expecting Digital signature");
}

firehose_error_t recvNextPacket(byte** packet_ptr, SIZE_T bytes_to_read, SIZE_T *bytes_read, boolean queue_only) {
    boolean packet_validated = FALSE;
    byte* hash_location;
    SIZE_T signed_payload_size;
    SIZE_T len_copied;
    firehose_error_t read_retval = FIREHOSE_SUCCESS;

    if (fh.validation_enabled == TRUE) {

        // This code section handles Validated programming
        // if a failure occurs call the function to reset
        // all validation related variables to their initial
        // states

        while(packet_validated == FALSE) {
        if (fh.validation_signed_hash_received == FALSE) {

            read_retval = readChannel(packet_ptr, FIREHOSE_SIGNED_HASH_PACKET_SIZE, bytes_read, queue_only);
            if (read_retval != FIREHOSE_SUCCESS) {
                firehoseResetValidation();
                return read_retval;
            }
            if (queue_only == TRUE)
                return FIREHOSE_SUCCESS;
            logMessage("Validation is enabled.");
            // Verify the data in the receive buffer, then copy the data
            // into validation_hash_buffer
            if (authenticateSignedHash(*packet_ptr,
                                       *bytes_read,
                                       &hash_location,
                                       &signed_payload_size) == FALSE) {
                // Failed to authenticate the signed hash
                logMessage("ERROR: Failed to authenticate Digital Signature");
                logMessage("Resetting validation state");
                sendResponse(NAK);
                firehoseResetValidation();
                return FIREHOSE_VALIDATION_ERROR;
            }
            fh.validation_signed_hash_received = TRUE;

            // Resetting num_pacets so as to match zeno when
            // debugging validated image programming
            fh.num_packets_received = 0;

            fh.validation_hash_buffer_num_hashes = signed_payload_size / sizeof(fh.validation_hash_value);
            memscpy(
                &fh.validation_hash_buffer[fh.validation_hash_buffer_capacity - fh.validation_hash_buffer_num_hashes],
                sizeof(fh.validation_hash_buffer),
                hash_location,
                signed_payload_size);
            fh.validation_hash_buffer_index = fh.validation_hash_buffer_capacity - fh.validation_hash_buffer_num_hashes;
            sendResponse(ACK);
            continue;
        }
        else {
            if (fh.validation_hash_buffer_index
                == fh.validation_hash_buffer_capacity - 1) {
                read_retval = readChannel(packet_ptr,
                            fh.validation_hash_buffer_new_capacity * sizeof(fh.validation_hash_value),
                            bytes_read,
                            queue_only);
            }
            else {
                read_retval = readChannel(packet_ptr, bytes_to_read, bytes_read, queue_only);
            }
            if (read_retval != FIREHOSE_SUCCESS) {
                firehoseResetValidation();
                return read_retval;
            }
            if (queue_only == TRUE)
                return FIREHOSE_SUCCESS;

            if (fh.validation_hash_buffer_num_hashes == 0) {
                logMessage("No more hashes to validate against");
                sendResponse(NAK);
                firehoseResetValidation();
                return FIREHOSE_VALIDATION_ERROR;
            }
#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
            PerformSHA256HW(*packet_ptr,
                          *bytes_read,
                          fh.validation_hash_value);
#else
            PerformSHA256(*packet_ptr,
                          *bytes_read,
                          fh.validation_hash_value);
#endif
            if (memcmp(
                    fh.validation_hash_value,
                    fh.validation_hash_buffer[fh.validation_hash_buffer_index],
                    sizeof(fh.validation_hash_value)) != 0) {
                // Invalid Hash value
                logMessage("ERROR: Hash value mismatch. Rejecting this packet.");
                logDigest("Expected:      ", fh.validation_hash_buffer[fh.validation_hash_buffer_index], sizeof(fh.validation_hash_value));
                logDigest("Found instead: ", fh.validation_hash_value, sizeof(fh.validation_hash_value));
                logMessage("(P%04d) (H%04d) Table %d", fh.num_packets_received - 1, fh.validation_hash_buffer_index, fh.validation_table_count);
                logMessage("Resetting validation state.");

                sendResponse(NAK);
                firehoseResetValidation();
                return FIREHOSE_VALIDATION_ERROR;
            }
            fh.validation_hash_buffer_num_hashes--;

            if (fh.validation_hash_buffer_index
                == fh.validation_hash_buffer_capacity - 1) {
                len_copied = MIN(
                                sizeof(fh.validation_hash_buffer),
                                *bytes_read);
                memscpy(fh.validation_hash_buffer,
                sizeof(fh.validation_hash_buffer),
                            *packet_ptr,
                            len_copied);
                fh.validation_hash_buffer_index = 0;
                fh.validation_hash_buffer_num_hashes = len_copied / sizeof(fh.validation_hash_value);
                if (fh.validation_hash_buffer_capacity != fh.validation_hash_buffer_new_capacity)
                    fh.validation_hash_buffer_capacity = fh.validation_hash_buffer_new_capacity;

                if (fh.validation_hash_buffer_num_hashes < 2) {
                    debugMessage("read hashes %d", fh.validation_hash_buffer_num_hashes);
                }
                fh.validation_table_count++;
                sendResponse(ACK);
                continue;
            }
            else {
                fh.validation_hash_buffer_index++;
                packet_validated = TRUE;
            }
        }
        }
    }
    else {
        read_retval = readChannel(packet_ptr, bytes_to_read, bytes_read, queue_only);
        if (read_retval != FIREHOSE_SUCCESS) {
            return read_retval;
        }
        if (queue_only == TRUE)
            return FIREHOSE_SUCCESS;
        if (fh.hash_in_non_secure == TRUE) {
#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
            PerformSHA256HW(*packet_ptr,
                          *bytes_read,
                          fh.validation_hash_value);
#else
            PerformSHA256(*packet_ptr,
                          *bytes_read,
                          fh.validation_hash_value);
#endif
        }
    }
    return FIREHOSE_SUCCESS;
}

firehose_error_t queueNextPacket(SIZE_T bytes_to_read) {
    return recvNextPacket(NULL, bytes_to_read, NULL, TRUE);
}

firehose_error_t getNextPacket(byte** packet_ptr, SIZE_T bytes_to_read, SIZE_T *bytes_read) {
    return recvNextPacket(packet_ptr, bytes_to_read, bytes_read, FALSE);
}

firehose_error_t handleConfigure() {
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    response_t response = ACK;

    // Let's declare the variables we care about
    SIZE_T max_payload_size_to_target;
    SIZE_T max_hash_table_size_in_bytes;

    boolean num_conversion;


    // Let's first clear the variables that the <configure> can
    // set, by assigning default values
    fh.skip_storage_writes = FALSE;
    fh.ack_raw_data_every = 0;
    fh.hash_in_non_secure = FALSE;
    fh.verbose = FALSE;

    // Let's extract all the attributes
    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "MaxPayloadSizeToTargetInBytes")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            max_payload_size_to_target =
                    stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get max payload size");
                response = NAK;
            }
            else {
                if (max_payload_size_to_target > 0) {
                    if (max_payload_size_to_target > FIREHOSE_CHANNEL_BUFFER_SIZE) {
                        logMessage("Host's payload to target size is too large");
                        response = NAK;
                    }
                    else {
                        fh.channel_buffer_capacity = max_payload_size_to_target;
                    }
                }
                else {
                    logMessage("Host's payload to target size is zero");
                    response = NAK;
                }
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "MaxDigestTableSizeInBytes")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            max_hash_table_size_in_bytes =
                    stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get max hash table size");
                response = NAK;
            }
            else {
                if (max_hash_table_size_in_bytes > 0) {
                    if (max_hash_table_size_in_bytes > sizeof(fh.validation_hash_buffer)) {
                        logMessage("Host's hash buffer size is too large");
                        response = NAK;
                    }
                    else {
                        // TODO: MAYBE: there might be a more elegant way to handle
                        // this but at the moment this is the simplest, i.e. keep a
                        // separate variable and when the time comes to fetch a new
                        // hash table, grab this size instead of the original size
                        fh.validation_hash_buffer_new_capacity = max_hash_table_size_in_bytes / sizeof(fh.validation_hash_value);
                    }
                }
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader,
                                "ZlpAwareHost")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            if (temp_buffer[0] == '0') {
#ifndef FEATURE_FIREHOSE_QUSB
                qhsusb_al_bulk_set_zlp_mode(FALSE);
#else
                qusb_al_bulk_set_zlp_mode(FALSE);
#endif
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader,
                                "SkipWrite")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            if (temp_buffer[0] == '1') {
                fh.skip_storage_writes = TRUE;
                logMessage("skipping write");
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader,
                                "AckRawData")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            if (temp_buffer[0] == '1') {
                fh.ack_raw_data_every = 1;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader,
                                "AckRawDataEveryNumPackets")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            fh.ack_raw_data_every = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                fh.ack_raw_data_every = 0;
                logMessage("Failed to get ack_raw_data_every");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader,
                                "AlwaysValidate")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            if (temp_buffer[0] == '1') {
                fh.hash_in_non_secure = TRUE;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader,
                                "Verbose")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            if (temp_buffer[0] == '1') {
                fh.verbose = TRUE;
            }
        }
    }
    logMessage("logbuf@0x%08X fh@0x%08X", fh.debug_log_buffer, &fh);
    return sendResponseFields(
           response,
           8,
           "MinVersionSupported"                   , 'd', 1,
           "MemoryName"                            , 's', "eMMC",
           "MaxPayloadSizeFromTargetInBytes"       , 'd', FIREHOSE_TX_BUFFER_SIZE,
           "MaxPayloadSizeToTargetInBytes"         , 't', fh.channel_buffer_capacity,
           "MaxPayloadSizeToTargetInBytesSupported", 'd', FIREHOSE_CHANNEL_BUFFER_SIZE,
           "MaxXMLSizeInBytes"                     , 'd', sizeof(fh.xml_buffer),
           "Version"                               , 'd', 1,
           "TargetName"                            , 's', DEVICEPROGRAMMER_TARGET_NAME);
}

firehose_error_t handleGetStorageInfo() {
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    int physical_partition_number = -1;
    boolean num_conversion;
    response_t response = ACK;

    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "physical_partition_number")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            physical_partition_number = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get physical partition number");
                response = NAK;
            }
        }
    }
    if (physical_partition_number == -1 || NAK == response) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (FALSE == storage_device_open_partition(&fh.emmc_storage,
                                  physical_partition_number)) {
        logMessage("Invalid physical partition %d", physical_partition_number);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (FALSE == storageDeviceGetStorageInfo(&fh.emmc_storage)) {
      sendResponse(NAK);
      return FIREHOSE_ERROR;
    }
    else {
      sendResponse(ACK);
      return FIREHOSE_SUCCESS;
    }
}

firehose_error_t handlePower() {
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    long delay_time = 0;
    typedef enum {POWER_NONE, POWER_RESET, POWER_OFF} power_action_t;
    power_action_t power_action = POWER_NONE;
    boolean num_conversion;

    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "value")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            if (strncasecmp((const char *)temp_buffer,
                        "reset",
                        strlen("reset")) == 0) {
                power_action = POWER_RESET;
            }
            else if (strncasecmp((const char *)temp_buffer,
                        "off",
                        strlen("off")) == 0) {
                power_action = POWER_OFF;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "delayinseconds")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            delay_time = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get delay time. Using default.");
                delay_time = 1;
            }
            else if (delay_time < 1) {
                delay_time = 1;
            }
        }
    }
    if (power_action == POWER_RESET) {
        sendResponse(ACK);
        delaySeconds(delay_time);
        bsp_target_reset();
        while(1);
    }
    else if (power_action == POWER_OFF) {
        sendResponse(ACK);
        delaySeconds(delay_time);
        bsp_target_poweroff();
        while(1);
    }

	return FIREHOSE_ERROR;
}

firehose_error_t handlePeek() {
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    byte another_temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    boolean num_conversion;
    response_t response = ACK;
    byte* address = NULL;
    SIZE_T size_in_bytes = 0,
           i = 0,

           // Each byte will take 5 characters [0 x 0 0 space]
           bytes_per_log_message = FIREHOSE_TEMP_LOG_BUFFER_SIZE / 5;

    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "SizeInBytes")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            size_in_bytes = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get size in bytes");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "address64")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            address = (byte *) (uintptr_t) stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get address");
                response = NAK;
            }
        }
    }
    if (NAK == response) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (NULL == address || 0 == size_in_bytes) {
        logMessage("Invalid parameters");
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    logMessage("Using address %p", address);
    for (i = 0; i < size_in_bytes; ++i) {
        snprintf((char *) temp_buffer, sizeof(temp_buffer), "0x%02X ", *(address + i));
        strlcat((char *) another_temp_buffer, (const char *) temp_buffer, sizeof(another_temp_buffer));
        if ((i + 1) % bytes_per_log_message == 0 || i == size_in_bytes - 1) {
            logMessage("%s", another_temp_buffer);
            another_temp_buffer[0] = '\0';
        }
    }
    sendResponse(ACK);
	return FIREHOSE_SUCCESS;
}

firehose_error_t handlePoke() {
    byte         temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    boolean num_conversion;
    response_t response = ACK;
    byte* address = NULL;
    SIZE_T size_in_bytes = 0,
                   value = 0,
                       i = 0;

    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "SizeInBytes")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            size_in_bytes = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get size in bytes");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "address64")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            address = (byte *) (uintptr_t) stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get address");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "value")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            value = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get size in bytes");
                response = NAK;
            }
        }
    }
    if (NAK == response) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (NULL == address || 0 == size_in_bytes) {
        logMessage("Invalid parameters address %p size_in_bytes "SIZE_T_FORMAT, address, size_in_bytes);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (size_in_bytes > sizeof(value)) {
        logMessage("Cannot handle size in bytes greater than "SIZE_T_FORMAT, sizeof(value));
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    logMessage("Using address %p", address);
    for (i = 0; i < size_in_bytes; ++i) {
        *address = value & 0xFF;
        address++;
        value = value >> 8;
    }
    sendResponse(ACK);
    return FIREHOSE_SUCCESS;
}

firehose_error_t handleErase() {
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    int storage_drive_number = -1;
    boolean retval;
    boolean num_conversion;
    response_t response = ACK;

    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "physical_partition_number")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            storage_drive_number = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get physical_partition_number");
                response = NAK;
            }
        }
    }
    if (NAK == response) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (storage_drive_number == -1) {
        logMessage("No physical_partition_number");
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (FALSE == storage_device_open_partition(&fh.emmc_storage,
                                  storage_drive_number)) {
        logMessage("Invalid physical partition %d", storage_drive_number);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    retval = storageDeviceErase(&fh.emmc_storage);
    sendResponse((retval == TRUE) ? ACK : NAK);
	return ((retval == TRUE) ? FIREHOSE_SUCCESS : FIREHOSE_STORAGE_ERROR);
}

firehose_error_t handleNop() {
    sendResponse(ACK);
    return FIREHOSE_SUCCESS;
}

firehose_error_t handleSetBootableStorageDrive() {
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    boolean num_conversion;
    response_t response = ACK;

    // Declare the variables we care about
    int bootable_drive = -1;

    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "value")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            bootable_drive = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get bootable drive");
                response = NAK;
            }
        }
    }
    if (NAK == response) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (bootable_drive == -1) {
        logMessage("Could not find bootable drive value.");
        return sendResponse(NAK);
    }
    if (FALSE == storage_device_open_partition(&fh.emmc_storage, bootable_drive)) {
        logMessage("Failed to set bootable drive to %d.",
                   bootable_drive);
        return sendResponse(NAK);
    }
    storageDeviceSetBootableStorageDrive(&fh.emmc_storage);
    storageDeviceClosePartition(&fh.emmc_storage);
    logMessage("Set bootable drive to %d.", bootable_drive);
    return sendResponse(ACK);
}

firehose_error_t handleStorageExtras() {

#ifndef FEATURE_FIREHOSE_ENABLE_STORAGE_EXTRAS
    logMessage("Storage extras not supported");
    return sendResponse(NAK);
#else

    // This function is a little different from the others because
    // we will delegate most of the work to the storage code
    // since we don't want to clutter firehose code with storage
    // peculiarities


    // Declare the variables we care about
    byte attribute_name[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    byte attribute_value[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    boolean commit = FALSE;
    boolean display = FALSE;
    boolean storage_retval = TRUE, temp_retval;
    int errno;

    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "commit")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 attribute_value,
                                 sizeof(attribute_value));
            if (attribute_value[0] == '1') {
                commit = TRUE;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "display")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 attribute_value,
                                 sizeof(attribute_value));
            if (attribute_value[0] == '1') {
                display = TRUE;
            }
        }
        else {
            xmlGetAttributeName(&fh.xml_reader,
                                 attribute_name,
                                 sizeof(attribute_name));

            xmlGetAttributeValue(&fh.xml_reader,
                                 attribute_value,
                                 sizeof(attribute_value));
            temp_retval = storageDeviceSetExtras(&fh.emmc_storage,
                                   (const char*) attribute_name,
                                   (const char*) attribute_value);
            if (temp_retval == FALSE) {
                storage_retval = FALSE;
                logMessage("Invalid storage extra parameter %s",
                            attribute_name);
            }
        }
    }
    if (FALSE == storage_retval) {
        return sendResponse(NAK);
    }
    if (TRUE == display) {
        storageDeviceGetExtras(&fh.emmc_storage);
    }
    if (FALSE == commit)
        return sendResponse(ACK);

    if (FALSE == storage_device_open_partition(&fh.emmc_storage, 0)) {
        logMessage("Could not open partition for creating new drives");
        return sendResponse(NAK);
    }
    if (FALSE == storageDeviceExtrasCommit(&fh.emmc_storage, &errno)) {
        logMessage("Failed to set storage extra parameters");
        return sendResponse(NAK);
    }
    storageDeviceClosePartition(&fh.emmc_storage);
    logMessage("Set storage parameters successfully");
    return sendResponse(ACK);
#endif

}

firehose_error_t handleFirmwareProgram() {
    SIZE_T sector_size = 0;
    SIZE_T num_partition_sectors = 0;
    SIZE_T bytes_to_read = 0;
    SIZE_T bytes_read_in_layer = 0;
    firehose_error_t channel_read_retval = FIREHOSE_SUCCESS;
    byte* raw_data_buffer;
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    response_t response = ACK;
    boolean num_conversion;
    int errno;

    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "SECTOR_SIZE_IN_BYTES")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            sector_size = stringToNumber((const char *) temp_buffer,
                                          &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get Sector size value");
                response = NAK;
            }
        }
        if (xmlIsAttribute(&fh.xml_reader, "num_partition_sectors")) {
            xmlGetAttributeValue(&fh.xml_reader, temp_buffer,
                                 sizeof(temp_buffer));
            num_partition_sectors = stringToNumber((const char *) temp_buffer,
                                                    &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get Number of sectors to write");
                response = NAK;
            }
        }
    }

    if (response == NAK) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }

    logMessage ("firmwarewrite cmd: Sector size "SIZE_T_FORMAT, sector_size);
    logMessage ("firmwarewrite cmd: Number of Sectors "SIZE_T_FORMAT,
                 num_partition_sectors);

    if (fh.emmc_storage.sector_size != sector_size) {
        logMessage ("Disk sector size does not match xml\n");
        logMessage ("Disk sector size :"SIZE_T_FORMAT" XML gave:"SIZE_T_FORMAT,
                    fh.emmc_storage.sector_size, sector_size);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }

    bytes_to_read = sector_size * num_partition_sectors;
    logMessage("Bytes to read from channel: "SIZE_T_FORMAT, bytes_to_read);

    if (bytes_to_read == 0) {
        logMessage ("Firmware Binary must be non-zero in size. Check binary");
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }

    if (bytes_to_read > fh.channel_buffer_capacity) {
        logMessage("Bytes to read > buffer capacity of "SIZE_T_FORMAT"bytes",
                    fh.channel_buffer_capacity);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }

    /* Switch to raw mode */
    sendResponseFields(
       ACK,
       1,
       "rawmode" ,'s', "true");

    channel_read_retval = getNextPacket(&raw_data_buffer, bytes_to_read,
                                        &bytes_read_in_layer);
    if (channel_read_retval != FIREHOSE_SUCCESS) {
        logMessage("Failed to read full FW binary. Read "SIZE_T_FORMAT" Bytes",
                    channel_read_retval);
        response = NAK;
        goto end;
    }

    if (bytes_to_read != bytes_read_in_layer) {
        logMessage("Read Error.Bytes expected:"SIZE_T_FORMAT",read:"SIZE_T_FORMAT,
                   bytes_to_read, bytes_read_in_layer);
        response = NAK;
        goto end;
    }

    if (response == ACK) {
        logMessage("Channel read "SIZE_T_FORMAT" bytes", bytes_read_in_layer);
    }

    if (FALSE == storageDeviceFWUpdate (&fh.emmc_storage, raw_data_buffer,
                                        num_partition_sectors, &errno))  {
        response = NAK;
        goto end;
    }

    response = ACK;

end:
    sendResponseFields(
        response,
        1,
        "rawmode" ,'s', "false");

    if (response == NAK)
        return FIREHOSE_ERROR;

    return FIREHOSE_SUCCESS;
}

firehose_error_t handlePatch() {
    boolean num_conversion;
    response_t response = ACK;

    // Let's declare the variables we care about
    SIZE_T start_sector = 0;
    int byte_offset = -1,
        physical_partition_number = -1,
        size_in_bytes = -1,
        sector_size_in_bytes = 512;
    char filename[64] = {0};
    char patch_value[64] = {0};
    char start_sector_value[64] = {0};
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    SIZE_T patch_result = 0;
    int errno;
    byte * raw_data_buffer;

    // Declare vars used in the CRC patch value parsing
    SIZE_T crc_start_sector, crc_over_num_bytes;
    char *comma_pos;
    char *end_crc_pos;
    SIZE_T patch_value_len;

    // Let's extract all the attributes
    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "byte_offset")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            byte_offset = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get byte_offset");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "physical_partition_number")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            physical_partition_number = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get physical_partition_number");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "size_in_bytes")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            size_in_bytes = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get size_in_bytes");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "start_sector")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 (byte *) start_sector_value,
                                 sizeof(start_sector_value));
            //start_sector = stringToNumber((const char *) temp_buffer, &num_conversion);
        }
        else if (xmlIsAttribute(&fh.xml_reader, "SECTOR_SIZE_IN_BYTES")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            sector_size_in_bytes = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get sector_size_in_bytes");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "filename")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 (byte *) filename,
                                 sizeof(filename));
        }
        else if (xmlIsAttribute(&fh.xml_reader, "value")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 (byte *) patch_value,
                                 sizeof(patch_value));
        }
    }
    if (NAK == response) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (byte_offset == -1
        || physical_partition_number == -1
        || size_in_bytes == -1
        || sector_size_in_bytes == -1) {
        return FIREHOSE_ERROR;
    }
    if (strncasecmp("DISK", filename, sizeof(filename)) != 0) {
        // Nothing to be done if it is not a DISK patch
        return FIREHOSE_SUCCESS;
    }
    if (fh.emmc_storage.sector_size != sector_size_in_bytes) {
        logMessage("Sector size in XML %d does not match disk sector size %d",
                   sector_size_in_bytes,
                   fh.emmc_storage.sector_size);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (FALSE == storage_device_open_partition(&fh.emmc_storage,
                                  physical_partition_number)) {
        logMessage("Invalid physical partition %d", physical_partition_number);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }

    // TODO: Clean this up. This functionality should not know so much
    // about the buffers available
    raw_data_buffer = fh.channel_buffer[1 - fh.channel_buffer_to_use];

    snprintf((char *)temp_buffer, sizeof(temp_buffer), SIZE_T_FORMAT, storageDeviceGetPartitionSizeSectors(&fh.emmc_storage));
    simpleStrReplace(patch_value,
                     "NUM_DISK_SECTORS",
                     (const char *) temp_buffer);
    simpleStrReplace(patch_value,
                     ".",
                     "");

    simpleStrReplace(start_sector_value,
                     "NUM_DISK_SECTORS",
                     (const char *) temp_buffer);
    simpleStrReplace(start_sector_value,
                     ".",
                     "");

    parseExpression(start_sector_value, &start_sector);

    if (strstr(patch_value, "CRC32")) {
        simpleStrReplace(patch_value,
                         "CRC32",
                         "");

        patch_value_len = strlen(patch_value);
        comma_pos = strchr(patch_value, ',');
        end_crc_pos = strrchr(patch_value, ')');
	if (NULL == comma_pos || NULL == end_crc_pos) {
            logMessage("Invalid CRC patch value");
            return FIREHOSE_ERROR;
	}
        if (end_crc_pos < comma_pos) {
            return FIREHOSE_ERROR;
        }
        memscpy(temp_buffer,
                    sizeof(temp_buffer),
                    comma_pos + 1,
                    end_crc_pos - (comma_pos + 1));
        temp_buffer[end_crc_pos - (comma_pos + 1)] = '\0';
        //crc_start_sector
        crc_over_num_bytes = stringToNumber((const char *) temp_buffer, &num_conversion);
        if (FALSE == num_conversion) {
            logMessage("Failed to get crc_over_num_bytes");
            sendResponse(NAK);
            return FIREHOSE_ERROR;
        }
        memsmove(comma_pos, sizeof(temp_buffer),
                end_crc_pos,
                1 + patch_value + patch_value_len - end_crc_pos);
        parseExpression(patch_value, &crc_start_sector);
        logMessage("crc start sector "SIZE_T_FORMAT", over bytes "SIZE_T_FORMAT, crc_start_sector, crc_over_num_bytes);

        storageDeviceRead(
            &fh.emmc_storage,
            raw_data_buffer,
            crc_start_sector,
            (crc_over_num_bytes / fh.emmc_storage.sector_size)
            + ((crc_over_num_bytes % fh.emmc_storage.sector_size == 0) ? 0 : 1));
        patch_result = calc_crc32(raw_data_buffer, crc_over_num_bytes);
    }
    else {
        parseExpression(patch_value, &patch_result);
    }

    storageDeviceRead(&fh.emmc_storage,
                      raw_data_buffer,
                      start_sector,
                      1);

    // TODO: clean this up a bit
    if (4 == size_in_bytes)
        patch_result &= 0xFFFFFFFF;
    memscpy(raw_data_buffer + byte_offset,
                fh.channel_buffer_capacity-byte_offset,
                &patch_result,
                size_in_bytes);
    /*
    if (sizeof(patch_result) < size_in_bytes)
    {
        if ((byte_offset + size_in_bytes) < sizeof(temp_buffer)) {
            memset(temp_buffer + byte_offset + sizeof(patch_result),
                   0,
                   size_in_bytes - sizeof(patch_result));
        }
    }
    */
    storageDeviceWrite(&fh.emmc_storage,
                       raw_data_buffer,
                       start_sector,
                       1,
                       &errno);

    logMessage("Patched sector "SIZE_T_FORMAT" with %08X", start_sector, patch_result);

    return sendResponse(ACK);
}

boolean parseIntegerValue(char* valueArray,
                          SIZE_T value_array_size,
                          SIZE_T *parsedValue) {

    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    boolean num_conversion;

    simpleStrReplace(valueArray,
                         ".",
                         "");

    if (strstr(valueArray, "NUM_DISK_SECTORS") != NULL) {
        snprintf((char *)temp_buffer, sizeof(temp_buffer), SIZE_T_FORMAT, storageDeviceGetPartitionSizeSectors(&fh.emmc_storage));
        simpleStrReplace(valueArray,
                         "NUM_DISK_SECTORS",
                         (const char *) temp_buffer);
        num_conversion = parseExpression(valueArray, parsedValue);
    }
    else {
        num_conversion = parseExpression(valueArray, parsedValue);
    }
    if (FALSE == num_conversion)
        return FALSE;

    if (*parsedValue > storageDeviceGetPartitionSizeSectors(&fh.emmc_storage)) {
        logMessage("Parsed value exceeds size of partition in sectors");
        return FALSE;
    }
    return TRUE;
}

firehose_error_t handleGetDigest(boolean isCRC) {
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    boolean num_conversion;
    response_t response = ACK;

    // Let's declare the variables we care about
    SIZE_T start_sector = 0,
        num_partition_sectors = 0;
    int physical_partition_number = -1,
        sector_size_in_bytes = 512;

    char start_sector_value[64] = {0};

    // Variables to deal with raw data
    boolean retval = FALSE;
    SIZE_T sectors_to_send,
           bytes_to_send_to_layer;
    byte* raw_data_buffer;
    SIZE_T raw_data_buffer_sector_capacity;
    //int errno;
    struct __sechsh_ctx_s   context;
    //SHA256_CTX sha2_ctx;
    uint8 data_digest[32] = {0};
    // The log string size is currently based on
    // sizeof(data_digest) * 2 + strlen("SHA256 ") + 1
    // and some extra space
    char log_string[72] = {0};
    int written;
    int i = 0;
    SIZE_T sectors_done = 0;
    SIZE_T log_msg_every_sectors = 16384;
    SIZE_T log_start_sector, log_num_sectors;
    word crc_value = (word) CRC_16_STEP_SEED;

    // Let's extract all the attributes
    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "start_sector")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 (byte *) start_sector_value,
                                 sizeof(start_sector_value));
        }
        else if (xmlIsAttribute(&fh.xml_reader, "num_partition_sectors")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            num_partition_sectors = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get num_partition_sectors");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "physical_partition_number")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            physical_partition_number = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get physical_partition_number");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "SECTOR_SIZE_IN_BYTES")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            sector_size_in_bytes = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get sector_size_in_bytes");
                response = NAK;
            }
        }
    }
    if (NAK == response) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (physical_partition_number == -1) {
        return FIREHOSE_ERROR;
    }
    if (fh.emmc_storage.sector_size != sector_size_in_bytes) {
        logMessage("Sector size in XML %d does not match disk sector size %d",
                   sector_size_in_bytes,
                   fh.emmc_storage.sector_size);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }

    raw_data_buffer_sector_capacity = fh.channel_buffer_capacity / sector_size_in_bytes;

    if (FALSE == storage_device_open_partition(&fh.emmc_storage,
                                  physical_partition_number)) {
        logMessage("Invalid physical partition %d", physical_partition_number);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (FALSE == parseIntegerValue(start_sector_value, sizeof(start_sector_value), &start_sector)) {
        logMessage("Failed to parse start sector");
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }

    if (FALSE == isCRC)
        sechsharm_sha256_init (&context);

    //SHA256_Init(&sha2_ctx );


    // TODO: Clean this up. Read functionality should not know so much
    // about the buffers available
    raw_data_buffer = fh.channel_buffer[1 - fh.channel_buffer_to_use];

    log_start_sector = start_sector;
    log_num_sectors = num_partition_sectors;

    while (num_partition_sectors > 0) {
        sectors_to_send = MIN(num_partition_sectors, raw_data_buffer_sector_capacity);
        bytes_to_send_to_layer = sectors_to_send * sector_size_in_bytes;

        retval = storageDeviceRead(&fh.emmc_storage,
                              raw_data_buffer,
                              start_sector,
                              sectors_to_send);

        if (retval == FALSE) {
            logMessage("emmc read failed");
            break;
        }
        else {
            start_sector += sectors_to_send;
        }

        if (FALSE == isCRC) {
            sechsharm_sha256_update(&context,
                            context.leftover,
                            &(context.leftover_size),
                            raw_data_buffer,
                            bytes_to_send_to_layer);
            //SHA256_Update(&sha2_ctx, raw_data_buffer, bytes_to_send_to_layer);
        }
        else {
            crc_value = calc_crc16_wrapper(raw_data_buffer, bytes_to_send_to_layer, crc_value);
        }

        num_partition_sectors -= sectors_to_send;

        sectors_done += sectors_to_send;
        if (sectors_done >= log_msg_every_sectors) {
            sectors_done = 0;
            if (num_partition_sectors > 0)
                logMessage(SIZE_T_FORMAT" sectors more to hash", num_partition_sectors);
        }
    }
    if (retval == TRUE) {
        if (FALSE == isCRC)
            sechsharm_sha256_final (&context,
                            context.leftover,
                            &(context.leftover_size),
                            data_digest);
        //SHA256_Final(data_digest, &sha2_ctx);

        written = snprintf(log_string,
                       sizeof(log_string),
                       "%s",
                       "Digest ");
        if (FALSE == isCRC) {
            while (written < sizeof(log_string) && i < sizeof(data_digest)) {
                written += snprintf(log_string + written,
                                    sizeof(log_string) - written,
                                    "%02X",
                                    data_digest[i]);
                i++;
            }
        }
        else {
            written += snprintf(log_string + written,
                                sizeof(log_string) - written,
                                "%04X",
                                crc_value);
        }

        logMessage("Hash start sector "SIZE_T_FORMAT" num sectors "SIZE_T_FORMAT,
                   log_start_sector,
                   log_num_sectors);
        logMessage(log_string);
    }
    sendResponse(ACK);
    return FIREHOSE_SUCCESS;
}

firehose_error_t handleRead() {

#ifndef FEATURE_FIREHOSE_ENABLE_READ
    logMessage("Read operation not allowed");
    // ACK-ing here since Read is (usually) not
    // a critical operation
    return sendResponse(ACK);
#else

    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    boolean num_conversion;
    response_t response = ACK;

    // Let's declare the variables we care about
    SIZE_T start_sector = 0,
           num_partition_sectors = 0,
           read_fail_sector = 0;
    int physical_partition_number = -1,
        sector_size_in_bytes = 512;

    char start_sector_value[64] = {0};

    // Variables to deal with raw data
    boolean retval = FALSE;
    response_t raw_data_status;
    SIZE_T sectors_to_send,
           bytes_to_send_to_layer;
    byte* raw_data_buffer;
    SIZE_T raw_data_buffer_sector_capacity;
    //int errno;


    // Let's extract all the attributes
    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "start_sector")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 (byte *) start_sector_value,
                                 sizeof(start_sector_value));
        }
        else if (xmlIsAttribute(&fh.xml_reader, "num_partition_sectors")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            num_partition_sectors = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get num_partition_sectors");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "physical_partition_number")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            physical_partition_number = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get physical_partition_number");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "SECTOR_SIZE_IN_BYTES")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            sector_size_in_bytes = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get sector_size_in_bytes");
                response = NAK;
            }
        }
    }
    if (NAK == response) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (physical_partition_number == -1) {
        return FIREHOSE_ERROR;
    }
    if (fh.emmc_storage.sector_size != sector_size_in_bytes) {
        logMessage("Sector size in XML %d does not match disk sector size %d",
                   sector_size_in_bytes,
                   fh.emmc_storage.sector_size);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    raw_data_buffer_sector_capacity = fh.channel_buffer_capacity / sector_size_in_bytes;

    if (FALSE == storage_device_open_partition(&fh.emmc_storage,
                                  physical_partition_number)) {
        logMessage("Invalid physical partition %d", physical_partition_number);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (FALSE == parseIntegerValue(start_sector_value, sizeof(start_sector_value), &start_sector)) {
        logMessage("Failed to parse start sector");
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }

    sendResponseFields(
           ACK,
           1,
           "rawmode" ,'s', "true");

    raw_data_status = ACK;

    while (num_partition_sectors > 0) {
        sectors_to_send = MIN(num_partition_sectors, raw_data_buffer_sector_capacity);
        bytes_to_send_to_layer = sectors_to_send * sector_size_in_bytes;

        // TODO: Clean this up. Read functionality should not know so much
        // about the buffers available
        raw_data_buffer = fh.channel_buffer[1 - fh.channel_buffer_to_use];

        if (raw_data_status == ACK) {

            retval = storageDeviceRead(&fh.emmc_storage,
                              raw_data_buffer,
                              start_sector,
                              sectors_to_send);

            if (retval == FALSE) {
                raw_data_status = NAK;

                // We cannot send out a log message in the middle of Read
                // raw data mode because the host will interpret it as raw
                // data and that would throw off its calculation of the number
                // of bytes to expect, so we save the failed sector (since that
                // is the only point of failure), and send it out later
                read_fail_sector = start_sector;
            }
            else {
                start_sector += sectors_to_send;
            }
        }

        writeChannel(raw_data_buffer, bytes_to_send_to_layer);
        num_partition_sectors -= sectors_to_send;

        /*
        if (fh.ack_raw_data == TRUE) {
            getNextPacket(&raw_data_buffer,
                          sizeof(fh.xml_buffer),
                          &bytes_read_in_layer);
        }
        */
    }


    if (NAK == raw_data_status)
        logMessage("Read failed at sector "SIZE_T_FORMAT, read_fail_sector);
    logMessage("Finished sector address %d", start_sector);
    sendResponseFields(
           raw_data_status,
           1,
           "rawmode" ,'s', "false");
    return (raw_data_status == ACK) ? FIREHOSE_SUCCESS : FIREHOSE_ERROR;

#endif

}

firehose_error_t handleProgram() {
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};

    // Let's declare the variables we care about
    SIZE_T start_sector = 0;
    SIZE_T num_partition_sectors = 0;
    int physical_partition_number = -1,
        sector_size_in_bytes = 512;

    char start_sector_value[64] = {0};
    char num_partition_sectors_value[64] = {0};

    // Variables to deal with raw data
    boolean retval = FALSE;
    firehose_error_t channel_read_retval = FIREHOSE_SUCCESS;
    SIZE_T rx_length_sectors;
    response_t raw_data_status;
    boolean read_back_verify = FALSE;
    boolean first_time_read_back_verify = TRUE;
    uint8 actual_data_digest[32] = {0};
    uint8 read_back_digest[32] = {0};
    //unsigned short actual_data_digest, read_back_digest;
    SIZE_T sectors_to_read,
           bytes_to_read_from_layer,
           bytes_read_in_layer;
    byte* raw_data_buffer;
    SIZE_T raw_data_buffer_sector_capacity;
    int errno;
    unsigned int ack_every_counter = 0;
    boolean num_conversion;
    response_t response = ACK;


    // Let's extract all the attributes
    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "start_sector")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 (byte *) start_sector_value,
                                 sizeof(start_sector_value));
        }
        else if (xmlIsAttribute(&fh.xml_reader, "num_partition_sectors")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 (byte *) num_partition_sectors_value,
                                 sizeof(num_partition_sectors_value));
        }
        else if (xmlIsAttribute(&fh.xml_reader, "physical_partition_number")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            physical_partition_number = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get physical partition number");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "SECTOR_SIZE_IN_BYTES")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            sector_size_in_bytes = stringToNumber((const char *) temp_buffer, &num_conversion);
            if (FALSE == num_conversion) {
                logMessage("Failed to get sector size");
                response = NAK;
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "read_back_verify")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            if (temp_buffer[0] == '1') {
                read_back_verify = TRUE;
            }
        }
    }
    if (NAK == response) {
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (physical_partition_number == -1) {
        return FIREHOSE_ERROR;
    }
    if (fh.emmc_storage.sector_size != sector_size_in_bytes) {
        logMessage("Sector size in XML %d does not match disk sector size %d",
                   sector_size_in_bytes,
                   fh.emmc_storage.sector_size);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    raw_data_buffer_sector_capacity = fh.channel_buffer_capacity / sector_size_in_bytes;
    if (FALSE == storage_device_open_partition(&fh.emmc_storage,
                                  physical_partition_number)) {
        logMessage("Invalid physical partition %d", physical_partition_number);
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (FALSE == parseIntegerValue(start_sector_value, sizeof(start_sector_value), &start_sector)) {
        logMessage("Failed to parse start sector");
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    if (FALSE == parseIntegerValue(num_partition_sectors_value, sizeof(num_partition_sectors_value), &num_partition_sectors)) {
        logMessage("Failed to parse num_partition_sectors");
        sendResponse(NAK);
        return FIREHOSE_ERROR;
    }
    logMessage("start "SIZE_T_FORMAT", num "SIZE_T_FORMAT, start_sector, num_partition_sectors);

    sendResponseFields(
           ACK,
           1,
           "rawmode" ,'s', "true");

    raw_data_status = ACK;

    while (num_partition_sectors > 0) {
        sectors_to_read = MIN(num_partition_sectors, raw_data_buffer_sector_capacity);
        bytes_to_read_from_layer = sectors_to_read * sector_size_in_bytes;

        channel_read_retval = getNextPacket(&raw_data_buffer, bytes_to_read_from_layer, &bytes_read_in_layer);
        debugMessage("toread %d read %d", bytes_to_read_from_layer, bytes_read_in_layer);

        if (channel_read_retval != FIREHOSE_SUCCESS) {
            if (fh.validation_enabled == TRUE) {
                logMessage("Encountered RAW data transport error in validation mode.");
            }
            else {
                delaySeconds(10);
                sendResponseFields(
                    NAK,
                    2,
                    "rawmode" ,'s', "false",
                    "AttemptRetry", 'd', 1);
            }
            return channel_read_retval;
        }
        if (bytes_read_in_layer > bytes_to_read_from_layer
            || bytes_read_in_layer % sector_size_in_bytes != 0) {
            continue;
        }

        rx_length_sectors = bytes_read_in_layer / sector_size_in_bytes;
        debugMessage("rx "SIZE_T_FORMAT" start "SIZE_T_FORMAT, rx_length_sectors, start_sector);

        if (num_partition_sectors - rx_length_sectors > 0)
            queueNextPacket(MIN(num_partition_sectors - rx_length_sectors, raw_data_buffer_sector_capacity) * sector_size_in_bytes);

        if (raw_data_status == ACK) {
            // logMessage("Writing to address %d",
            //           fh.raw_data_address);

            if (fh.skip_storage_writes == TRUE) {
                retval = TRUE;
            }
            else {
                retval = storageDeviceWrite(&fh.emmc_storage,
                                            raw_data_buffer,
                                            start_sector,
                                            rx_length_sectors,
                                            &errno);
                if (retval == FALSE) {
                    logMessage("Write failed Sector "SIZE_T_FORMAT", Errno %d", start_sector, errno);
                }
                if (retval == TRUE && (first_time_read_back_verify == TRUE || read_back_verify == TRUE)) {
                    if (first_time_read_back_verify == TRUE) {
                        first_time_read_back_verify = FALSE;
                    }

#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
                    PerformSHA256HW(raw_data_buffer,
                                  bytes_read_in_layer,
                                  actual_data_digest);
#else
                    PerformSHA256(raw_data_buffer,
                                  bytes_read_in_layer,
                                  actual_data_digest);
#endif
                    //actual_data_digest = calc_crc16(raw_data_buffer,
                    //                                bytes_read_in_layer * 8);

                    storageDeviceRead(&fh.emmc_storage,
                                      raw_data_buffer,
                                      start_sector,
                                      rx_length_sectors);

#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
                    PerformSHA256HW(raw_data_buffer,
                                  bytes_read_in_layer,
                                  read_back_digest);
#else
                    PerformSHA256(raw_data_buffer,
                                  bytes_read_in_layer,
                                  read_back_digest);
#endif
                    //read_back_digest = calc_crc16(raw_data_buffer,
                    //                              bytes_read_in_layer * 8);
                    if (memcmp(actual_data_digest,
                               read_back_digest,
                               sizeof(actual_data_digest)) != 0) {
                        // Read back verify failed
                        logMessage("Read back verify failed at sector %d,"
                               "num sectors %d",
                               start_sector,
                               rx_length_sectors);
                        retval = FALSE;
                    }
                }
            }
            if (retval == FALSE) {
                raw_data_status = NAK;
            }
            else {
                start_sector += rx_length_sectors;
            }
        }
        num_partition_sectors -= rx_length_sectors;
        if (fh.verbose == TRUE) {
            logMessage("%d bytes received, %d sectors more to go", bytes_read_in_layer, num_partition_sectors);
            //logMessage("%02X %02X %02X %02X, %02X %02X %02X %02X", raw_data_buffer[0], raw_data_buffer[1], raw_data_buffer[2], raw_data_buffer[3], raw_data_buffer[bytes_read_in_layer - 4], raw_data_buffer[bytes_read_in_layer - 3],raw_data_buffer[bytes_read_in_layer - 2],raw_data_buffer[bytes_read_in_layer - 1]);
        }

        if (fh.ack_raw_data_every > 0) {
            ack_every_counter += 1;
            if (ack_every_counter == fh.ack_raw_data_every) {
                sendResponse(raw_data_status);
                ack_every_counter = 0;
            }
        }
    }

    logMessage("Finished sector address "SIZE_T_FORMAT, start_sector);
    sendResponseFields(
           raw_data_status,
           1,
           "rawmode" ,'s', "false");
    return (raw_data_status == ACK) ? FIREHOSE_SUCCESS : FIREHOSE_ERROR;
}

firehose_error_t handleBenchmark() {
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    SIZE_T i, j, trials = 1000;
    long start_time, end_time, average_time;
    uint8 temp_hash_value[32] = {0};
    boolean retval = TRUE;
    byte* buffer = fh.channel_buffer[0];
    SIZE_T sector_size = fh.emmc_storage.sector_size;
    SIZE_T buffer_size = fh.channel_buffer_capacity;
    int errno;

    // TODO: Change reference to 512 to get the sector size from the storage device
    // TODO: Do not reference channel_buffer[0] directly. This func should not have any knowledge
    // of how many buffers are in use

    while (xmlGetToken(&fh.xml_reader) == XML_TOKEN_ATTRIBUTE) {
        if (xmlIsAttribute(&fh.xml_reader, "TestWritePerformance")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            if (temp_buffer[0] == '1') {
                logMessage("Going to test writes for lengths "SIZE_T_FORMAT" to "SIZE_T_FORMAT" for "SIZE_T_FORMAT" trials. This can take a while.",
                           sector_size, buffer_size, trials);
                storage_device_open_partition(&fh.emmc_storage, 0);
                // First read the data, so that when we write we're writing back what we read out
                // so that the benchmark tests are non-destructive!
                storageDeviceRead(&fh.emmc_storage,
                                  buffer,
                                  0,
                                  buffer_size / sector_size);
                for (i = sector_size; i <= buffer_size; i = i * 2) {
                    average_time = 0;

                    start_time = get_time();
                    for (j = 0; j < trials; j++) {
                        retval = storageDeviceWrite(&fh.emmc_storage,
                                        buffer,
                                        0,
                                        i/sector_size,
                                        &errno);
                        if (retval != TRUE) {
                            logMessage("Failure in write during perf testing");
                        }
                    }
                    end_time = get_time();
                    average_time += (end_time - start_time);

                    average_time = average_time / j;
                    logMessage("Writing "SIZE_T_FORMAT" bytes %d us", i, average_time);
                }
            }
        }
        if (xmlIsAttribute(&fh.xml_reader, "TestReadPerformance")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            if (temp_buffer[0] == '1') {
                logMessage("Going to test reads for lengths "SIZE_T_FORMAT" to "SIZE_T_FORMAT" for "SIZE_T_FORMAT" trials. This can take a while.",
                           sector_size, buffer_size, trials);
                storage_device_open_partition(&fh.emmc_storage, 0);
                for (i = sector_size; i <= buffer_size; i = i * 2) {
                    average_time = 0;

                    start_time = get_time();
                    for (j = 0; j < trials; j++) {
                        retval = storageDeviceRead(&fh.emmc_storage,
                                        buffer,
                                        0,
                                        i/sector_size);
                        if (retval != TRUE) {
                            logMessage("Failure in read during perf testing");
                        }
                    }
                    end_time = get_time();
                    average_time += (end_time - start_time);

                    average_time = average_time / j;
                    logMessage("Reading "SIZE_T_FORMAT" bytes %d us", i, average_time);
                }
            }
        }
        else if (xmlIsAttribute(&fh.xml_reader, "TestDigestPerformance")) {
            xmlGetAttributeValue(&fh.xml_reader,
                                 temp_buffer,
                                 sizeof(temp_buffer));
            if (temp_buffer[0] == '1') {
                for (i = sector_size; i <= buffer_size; i = i * 2) {
                    average_time = 0;
                    for (j = 0; j < trials; j++) {
                        start_time = get_time();
#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
                        PerformSHA256HW(buffer, i, temp_hash_value);
#else
                        PerformSHA256(buffer, i, temp_hash_value);
#endif
                        end_time = get_time();
                        average_time += (end_time - start_time);
                    }
                    average_time = average_time / j;
                    logMessage("Hashing "SIZE_T_FORMAT" bytes %d us", i, average_time);
                }
            }
        }
    }
    return sendResponse(ACK);
}

firehose_error_t processNextXMLTag() {
    xml_token_t token = xmlGetToken(&fh.xml_reader);
    byte temp_buffer[FIREHOSE_TEMP_BUFFER_SIZE] = {0};

    if (token == XML_TOKEN_NONE) {
        // If XML commands have been exhausted, get out of RAW data mode
        return FIREHOSE_ERROR;
    }
    else if (token == XML_TOKEN_TAG_ENDING) {
        // Check if </data> has been reached
        // Remove this check to be able to handle multiple tags/commands in a
        // single XML file
        xmlGetTag(&fh.xml_reader, temp_buffer, sizeof(temp_buffer));
        if (xmlIsTag(&fh.xml_reader, "data"))
            return FIREHOSE_ERROR;
    }
    else if (token == XML_TOKEN_TAG) {
        // found some valid tag
        if (xmlIsTag(&fh.xml_reader, "configure")) {
            return handleConfigure();
        }
        else if (xmlIsTag(&fh.xml_reader, "program")) {
            return handleProgram();
        }
        else if (xmlIsTag(&fh.xml_reader, "firmwarewrite")) {
            return handleFirmwareProgram();
        }
        else if (xmlIsTag(&fh.xml_reader, "patch")) {
            return handlePatch();
        }
        else if (xmlIsTag(&fh.xml_reader, "setbootablestoragedrive")) {
            return handleSetBootableStorageDrive();
        }
        else if (xmlIsTag(&fh.xml_reader, "ufs")) {
            return handleStorageExtras();
        }
		else if (xmlIsTag(&fh.xml_reader, "emmc")) {
            return handleStorageExtras();
        }
        else if (xmlIsTag(&fh.xml_reader, "power")) {
            return handlePower();
        }
        else if (xmlIsTag(&fh.xml_reader, "benchmark")) {
            return handleBenchmark();
        }
        else if (xmlIsTag(&fh.xml_reader, "read")) {
            return handleRead();
        }
        else if (xmlIsTag(&fh.xml_reader, "getstorageinfo")) {
            return handleGetStorageInfo();
        }
        else if (xmlIsTag(&fh.xml_reader, "getcrc16digest")) {
            return handleGetDigest(TRUE);
        }
        else if (xmlIsTag(&fh.xml_reader, "getsha256digest")) {
            return handleGetDigest(FALSE);
        }
        else if (xmlIsTag(&fh.xml_reader, "erase")) {
            return handleErase();
        }
        else if (xmlIsTag(&fh.xml_reader, "peek")) {
            return handlePeek();
        }
        else if (xmlIsTag(&fh.xml_reader, "poke")) {
            return handlePoke();
        }
        else if (xmlIsTag(&fh.xml_reader, "nop")) {
            return handleNop();
        }
        else if (xmlIsTag(&fh.xml_reader, "?xml")
              || xmlIsTag(&fh.xml_reader, "data")
              || xmlIsTag(&fh.xml_reader, "patches")) {
            // found start of XML file, i.e. <?xml version="1.0"?>, so just ignore
            // found next line of xml file we expect, <data> so ignore

        }
        else {
            if (FALSE == xmlGetTag(&fh.xml_reader, temp_buffer, sizeof(temp_buffer))) {
                logMessage("WARNING: Ignoring unrecognized tag too long to display. Continuing.");
            }
            else {
                logMessage("WARNING: Ignoring unrecognized tag '%s'. Continuing.",
                           temp_buffer);
            }
            return sendResponse(ACK);
        }
    }
    return FIREHOSE_SUCCESS;
}

// NOTE: How 'we' choose to validate the XML right now is not very generic.
// Hence this should not be put under the xml parser code
firehose_error_t validateXMLBuffer() {
    xml_token_t token;
    byte opening_tag[FIREHOSE_TEMP_BUFFER_SIZE] = {0};
    byte closing_tag[FIREHOSE_TEMP_BUFFER_SIZE] = {0};

    do {
        token = xmlGetToken(&fh.xml_reader);
    } while (token != XML_TOKEN_NONE && token != XML_TOKEN_TAG);

    if (token == XML_TOKEN_NONE)
        return FIREHOSE_ERROR;

    if (!xmlIsTag(&fh.xml_reader, "?xml"))
        return FIREHOSE_ERROR;

    do {
        token = xmlGetToken(&fh.xml_reader);
    } while (token != XML_TOKEN_NONE && token != XML_TOKEN_TAG);

    if (token != XML_TOKEN_TAG)
        return FIREHOSE_ERROR;
    xmlGetTag(&fh.xml_reader, opening_tag, sizeof(opening_tag));

    while ((token = xmlGetToken(&fh.xml_reader)) != XML_TOKEN_NONE) {
        if (token == XML_TOKEN_TAG_ENDING) {
            xmlGetTag(&fh.xml_reader, closing_tag, sizeof(closing_tag));
            if (strncasecmp((const char *) opening_tag,
                        (const char *) closing_tag,
                        sizeof(opening_tag)) == 0)
                return FIREHOSE_SUCCESS;
        }
        else if (token == XML_TOKEN_TEXT) {
            return FIREHOSE_ERROR;
        }
    }
    return FIREHOSE_ERROR;
}

void readAndProcessXML() {
    SIZE_T bytes_read = 0;
    byte* packet;
    firehose_error_t read_retval=FIREHOSE_SUCCESS;

    while (bytes_read == 0)
        read_retval = getNextPacket(&packet, MIN(sizeof(fh.xml_buffer), fh.channel_buffer_capacity), &bytes_read);

    if (FIREHOSE_SUCCESS != read_retval)
        return;

    xmlInitReader(&fh.xml_reader, packet, bytes_read);
    if (FIREHOSE_ERROR == validateXMLBuffer()) {
        logMessage("XML (%d bytes) not validated", bytes_read);
        sendResponse(NAK);
        xmlInitReader(&fh.xml_reader, NULL, 0);
        return;
    }
    if (sizeof(fh.xml_buffer) < bytes_read) {
        logMessage("XML file cannot fit in buffer");
        xmlInitReader(&fh.xml_reader, NULL, 0);
        return;
    }

    memscpy(fh.xml_buffer, sizeof(fh.xml_buffer), packet, bytes_read);
    xmlInitReader(&fh.xml_reader, fh.xml_buffer, bytes_read);
    while (processNextXMLTag() != FIREHOSE_ERROR) {

    }
    return;
}

void initFirehoseTransport()
{
#ifndef FEATURE_FIREHOSE_QUSB
    qhsusb_al_bulk_init(0x0, QHSUSB_MODE__SER_ONLY);
#else
    qusb_al_bulk_init(QUSB_MAX_SPEED_HIGH);
#endif
}

void main_firehose(void)
{
    boolean one_time_log = FALSE;
    for (;;)
    {
        readAndProcessXML();
        if (one_time_log == FALSE) {
            one_time_log = TRUE;
            logMessage("logbuf@0x%08X fh@0x%08X", fh.debug_log_buffer, &fh);
        }
    }
}

