/*==================================================================
 *
 * FILE:        deviceprogrammer_xml_parser.h
 *
 * DESCRIPTION:
 *   This module parses XML file structures in a semi-limited manner
 *
 *        Copyright © 2008-2013 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_xml_parser.h#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2013-05-31   ab      Initial checkin
*/

#ifndef DEVICEPROGRAMMER_XML_H
#define DEVICEPROGRAMMER_XML_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "deviceprogrammer_utils.h"

typedef enum {
    XML_TOKEN_NONE,
    XML_TOKEN_TAG,
    XML_TOKEN_ATTRIBUTE,
    XML_TOKEN_TEXT,
    XML_TOKEN_TAG_ENDING,
    XML_TOKEN_COMMENT
} xml_token_t;

typedef enum {
    XML_STATE_INITIAL,
    XML_STATE_WAIT_FOR_TAG_NAME,
    XML_STATE_IN_TAG_NAME,
    XML_STATE_WAIT_FOR_TEXT_OR_TAG,
    XML_STATE_IN_TEXT,
    XML_STATE_WAIT_FOR_START_OR_END_TAG,
    XML_STATE_WAIT_FOR_END_TAG_NAME,
    XML_STATE_IN_END_TAG_NAME,
    XML_STATE_WAIT_FOR_ENDING,
    XML_STATE_WAIT_FOR_ATTRIBUTE_OR_ENDING,
    XML_STATE_IN_ATTRIBUTE_NAME,
    XML_STATE_WAIT_FOR_ATTRIBUTE_VALUE_QUOTE,
    XML_STATE_WAIT_FOR_ATTRIBUTE_VALUE,
    XML_STATE_IN_ATTRIBUTE_VALUE,
    XML_STATE_BEGIN_COMMENT_TAG,
    XML_STATE_BEGIN_COMMENT,
    XML_STATE_IN_COMMENT,
    XML_STATE_END_COMMENT_TAG
} xml_state_t;

typedef struct {
    byte *buffer;
    SIZE_T length;
    xml_state_t parser_state;
    SIZE_T position;
    SIZE_T depth;

    /* Here let's declare all the data that a client
     * would ever want to retrieve
     */
    SIZE_T token_tag_start;
    SIZE_T token_tag_end;
    SIZE_T token_attribute_name_start;
    SIZE_T token_attribute_name_end;
    SIZE_T token_attribute_value_start;
    SIZE_T token_attribute_value_end;
    SIZE_T token_text_start;
    SIZE_T token_text_end;
    SIZE_T token_comment_start;
    SIZE_T token_comment_end;
} xml_reader_t;


typedef enum {
    XML_STATE_WRITER_INITIAL,
    XML_STATE_WRITER_TAG_HALF_CLOSED,
    XML_STATE_WRITER_TAG_OPENED
} xml_writer_state_t;

typedef struct {
    byte *buffer;
    SIZE_T length;
    SIZE_T write_position;
    xml_writer_state_t writer_state;

    SIZE_T tag_name_position_stack[4];
    SIZE_T tag_name_position_stack_size;

    SIZE_T closing_tags_space;

    SIZE_T rollback_write_position;
    xml_writer_state_t rollback_writer_state;
    SIZE_T rollback_tag_name_position_stack_size;
    SIZE_T rollback_closing_tags_space;

} xml_writer_t;


void xmlInitReader(xml_reader_t* xml_reader, byte* buf, SIZE_T len);
boolean xmlIsTag(xml_reader_t *xml_reader, const char *tag_name);
boolean xmlGetTag(xml_reader_t *xml_reader, byte* buffer, SIZE_T length);
boolean xmlIsAttribute(xml_reader_t *xml_reader, const char *attribute_name);
boolean xmlGetAttributeName(xml_reader_t *xml_reader, byte* buffer, SIZE_T length);
boolean xmlGetAttributeValue(xml_reader_t *xml_reader, byte* buffer, SIZE_T length);
xml_token_t xmlGetToken(xml_reader_t *xml_reader);



void xmlInitWriter(xml_writer_t* xml_writer, byte* buf, SIZE_T len);
void xmlWriterReset(xml_writer_t* xml_writer);
boolean isXMLWriterReset(xml_writer_t *xml_writer);
void xmlWriterRollback(xml_writer_t *xml_writer);
boolean xmlWriterCloseTag(xml_writer_t *xml_writer);
boolean xmlWriterFinalize(xml_writer_t *xml_writer);
boolean xmlWriterStartTag(xml_writer_t *xml_writer, const char* tag_name);
boolean xmlWriterStartChildTag(xml_writer_t *xml_writer, const char* tag_name);
boolean xmlWriterWriteAttribute(xml_writer_t *xml_writer,
                                const char* attribute_name,
                                const char* attribute_value);
boolean xmlWriterWriteText(xml_writer_t *xml_writer, const char* text);
boolean xmlWriterWriteTextFromBuffer(xml_writer_t *xml_writer, byte* buffer, SIZE_T length);
boolean xmlWriterWriteAttributeValueFromBuffer(xml_writer_t *xml_writer,
                                               const char* attribute_name,
                                               byte* buffer,
                                               SIZE_T length);
boolean xmlWriterWriteMultipleAttributes(xml_writer_t *xml_writer,
                                         int n_args,
                                         va_list ap);
boolean xmlWriterWriteMultipleAttributesWrapper(xml_writer_t *xml_writer,
                                                int n_args,
                                                ...);

boolean xmlWriterRawWrite(xml_writer_t *xml_writer,
                          const char* format,
                          ...);
boolean xmlWriterRawWriteFromBuffer(xml_writer_t *xml_writer,
                                    byte* buffer,
                                    SIZE_T length);
boolean xmlWriterRawStartTag(xml_writer_t *xml_writer, const char* tag_name);
boolean xmlWriterRawWriteAttributeValueFromBuffer(xml_writer_t *xml_writer,
                                                 const char* name,
                                                 byte* buffer,
                                                 SIZE_T length);
boolean xmlWriterRawWriteAttribute(xml_writer_t *xml_writer,
                                   const char* name,
                                   const char* value);
boolean xmlWriterRawCloseTag(xml_writer_t *xml_writer);
boolean xmlWriterRawCloseTagName(xml_writer_t *xml_writer,
                                 const char *tag_name);
boolean xmlWriterRawStartTagNoAttributes(xml_writer_t *xml_writer,
                                        const char *tag_name);

#endif

