/*==================================================================
 *
 * FILE:        deviceprogrammer_xml_parser.c
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_xml_parser.c#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2013-05-31   ab      Initial checkin
 */

#include "deviceprogrammer_xml_parser.h"
#include "stringl.h"

const char *state_desc[] = {
    "XML_STATE_INITIAL",
    "XML_STATE_WAIT_FOR_TAG_NAME",
    "XML_STATE_IN_TAG_NAME",
    "XML_STATE_WAIT_FOR_TEXT_OR_TAG",
    "XML_STATE_IN_TEXT",
    "XML_STATE_WAIT_FOR_START_OR_END_TAG",
    "XML_STATE_WAIT_FOR_END_TAG_NAME",
    "XML_STATE_IN_END_TAG_NAME",
    "XML_STATE_WAIT_FOR_ENDING",
    "XML_STATE_WAIT_FOR_ATTRIBUTE_OR_ENDING",
    "XML_STATE_IN_ATTRIBUTE_NAME",
    "XML_STATE_WAIT_FOR_ATTRIBUTE_VALUE_QUOTE",
    "XML_STATE_WAIT_FOR_ATTRIBUTE_VALUE",
    "XML_STATE_IN_ATTRIBUTE_VALUE",
    "XML_STATE_BEGIN_COMMENT_TAG",
    "XML_STATE_BEGIN_COMMENT",
    "XML_STATE_IN_COMMENT",
    "XML_STATE_END_COMMENT_TAG"
};

//static const char *xml_ignore_tags[] = {"xml", "!--", "comment"};
//static const byte XML_OPENING_TAG = '<';
//static const byte XML_CLOSING_TAG = '>';

void xmlInitReader(xml_reader_t* xml_reader, byte* buf, SIZE_T len) {
    xml_reader->buffer = buf;
    xml_reader->length = len;
    xml_reader->depth = 0;
    xml_reader->position = 0;
    xml_reader->parser_state = XML_STATE_INITIAL;
}

boolean xmlIsTag(xml_reader_t *xml_reader, const char *tag_name) {
    SIZE_T token_length = xml_reader->token_tag_end - xml_reader->token_tag_start;
    if ((strncasecmp(tag_name,
                (const char*) &xml_reader->buffer[xml_reader->token_tag_start],
                token_length) == 0)
        && tag_name[token_length] == '\0') {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

boolean xmlGetTag(xml_reader_t *xml_reader, byte* buffer, SIZE_T length) {
    SIZE_T tag_length = xml_reader->token_tag_end - xml_reader->token_tag_start;
    if (tag_length +1 > length) {
        return FALSE;
    }
    else {
        memscpy(buffer, length, &xml_reader->buffer[xml_reader->token_tag_start], tag_length);  // fix later
        buffer[tag_length] = '\0';
        return TRUE;
    }
}

boolean xmlIsAttribute(xml_reader_t *xml_reader, const char *attribute_name) {
    SIZE_T token_length = xml_reader->token_attribute_name_end - xml_reader->token_attribute_name_start;
    if ((strncasecmp(attribute_name,
                (const char*) &xml_reader->buffer[xml_reader->token_attribute_name_start],
                token_length) == 0)
        && attribute_name[token_length] == '\0') {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

boolean xmlGetAttributeName(xml_reader_t *xml_reader, byte* buffer, SIZE_T length) {
    SIZE_T value_length = xml_reader->token_attribute_name_end - xml_reader->token_attribute_name_start;
    if (value_length + 1 > length) {
        return FALSE;
    }
    else {
        memscpy(buffer, length, &xml_reader->buffer[xml_reader->token_attribute_name_start], value_length);
        buffer[value_length] = '\0';
        return TRUE;
    }
}

boolean xmlGetAttributeValue(xml_reader_t *xml_reader, byte* buffer, SIZE_T length) {
    SIZE_T value_length = xml_reader->token_attribute_value_end - xml_reader->token_attribute_value_start;
    if (value_length + 1 > length) {
        return FALSE;
    }
    else {
        memscpy(buffer, length, &xml_reader->buffer[xml_reader->token_attribute_value_start], value_length);
        buffer[value_length] = '\0';
        return TRUE;
    }
}

xml_token_t xmlGetToken(xml_reader_t *xml_reader) {
    byte ch;
    xml_state_t cur_state;
    xml_token_t token = XML_TOKEN_NONE;
    boolean xml_error = FALSE;

    while (xml_reader->position < xml_reader->length) {
        ch = xml_reader->buffer[xml_reader->position];
        cur_state = xml_reader->parser_state;
        //printf ("ch %c  state %s\n", ch, state_desc[cur_state]);

        if (cur_state == XML_STATE_INITIAL) {
            if (ch == '<') {
                if (xml_reader->depth == 0) {
                    xml_reader->parser_state = XML_STATE_WAIT_FOR_TAG_NAME;
                }
                else {
                    xml_reader->parser_state = XML_STATE_WAIT_FOR_START_OR_END_TAG;
                }
            }
            else if (!isspace(ch)) {
                xml_error = TRUE;
            }
        }
        else if (cur_state == XML_STATE_WAIT_FOR_TAG_NAME) {
            // Note: we are allowing ? here so as to treat tags like
            // <?xml as normal tags, as a tradeoff for added state
            // complexity
            if (isalpha(ch) || ch == '?') {
                xml_reader->parser_state = XML_STATE_IN_TAG_NAME;
                xml_reader->token_tag_start = xml_reader->position;
            }
            else if (ch == '!') {
                xml_reader->parser_state = XML_STATE_BEGIN_COMMENT_TAG;
            }
            else {
                xml_error = TRUE;
            }
        }
        else if (cur_state == XML_STATE_IN_TAG_NAME) {
            if (ch == '>') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_TEXT_OR_TAG;
                xml_reader->token_tag_end = xml_reader->position;
                token = XML_TOKEN_TAG;
                xml_reader->depth++;
            }
            else if (isspace(ch)) {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_ATTRIBUTE_OR_ENDING;
                xml_reader->token_tag_end = xml_reader->position;
                token = XML_TOKEN_TAG;
                xml_reader->depth++;
            }
            else if (isalnum(ch)) {

            }
            else if (ch == '/') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_ENDING;
                xml_reader->token_tag_end = xml_reader->position;
                token = XML_TOKEN_TAG;
                xml_reader->depth++;
            }
            else {
                xml_error = TRUE;
            }
        }
        else if (cur_state == XML_STATE_WAIT_FOR_TEXT_OR_TAG) {
            if (isalpha(ch)){
                xml_reader->parser_state = XML_STATE_IN_TEXT;
                xml_reader->token_text_start = xml_reader->position;
            }
            else if (ch == '<') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_START_OR_END_TAG;
            }
        }
        else if (cur_state == XML_STATE_IN_TEXT) {
            if (ch == '<') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_START_OR_END_TAG;
                token = XML_TOKEN_TEXT;
            }
        }
        else if (cur_state == XML_STATE_WAIT_FOR_START_OR_END_TAG) {
            if (ch == '/') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_END_TAG_NAME;
            }
            else if (isalpha(ch)) {
                xml_reader->parser_state = XML_STATE_IN_TAG_NAME;
                xml_reader->token_tag_start = xml_reader->position;
            }
            else if (ch == '!') {
                xml_reader->parser_state = XML_STATE_BEGIN_COMMENT_TAG;
            }
        }
        else if (cur_state == XML_STATE_WAIT_FOR_END_TAG_NAME) {
            if (isalpha(ch)) {
                xml_reader->parser_state = XML_STATE_IN_END_TAG_NAME;
                xml_reader->token_tag_start = xml_reader->position;
            }
        }
        else if (cur_state == XML_STATE_IN_END_TAG_NAME) {
            if (ch == '>') {
                xml_reader->parser_state = XML_STATE_INITIAL;
                xml_reader->token_tag_end = xml_reader->position;
                token = XML_TOKEN_TAG_ENDING;
                xml_reader->depth--;
            }
        }
        else if (cur_state == XML_STATE_WAIT_FOR_ATTRIBUTE_OR_ENDING) {
            // Note: By including ? here we are allowing any tag to end
            // with ?> instead of /> but that's a tradeoff with extra
            // state complexity
            if (ch == '/' || ch == '?') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_ENDING;
            }
            else if (isalpha(ch)) {
                xml_reader->parser_state = XML_STATE_IN_ATTRIBUTE_NAME;
                xml_reader->token_attribute_name_start = xml_reader->position;
            }
            else if (!isspace(ch)) {
                xml_error = TRUE;
            }
        }
        else if (cur_state == XML_STATE_WAIT_FOR_ENDING) {
            if (ch == '>') {
                xml_reader->parser_state = XML_STATE_INITIAL;
                token = XML_TOKEN_TAG_ENDING;
                xml_reader->depth--;
            }
        }
        else if (cur_state == XML_STATE_IN_ATTRIBUTE_NAME) {
            if (ch == '=') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_ATTRIBUTE_VALUE_QUOTE;
                xml_reader->token_attribute_name_end = xml_reader->position;
            }
        }
        else if (cur_state == XML_STATE_WAIT_FOR_ATTRIBUTE_VALUE_QUOTE) {
            if (ch == '"') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_ATTRIBUTE_VALUE;
            }
        }
        else if (cur_state == XML_STATE_WAIT_FOR_ATTRIBUTE_VALUE) {
            xml_reader->parser_state = XML_STATE_IN_ATTRIBUTE_VALUE;
            xml_reader->token_attribute_value_start = xml_reader->position;
            
            if (ch == '"') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_ATTRIBUTE_OR_ENDING;
                xml_reader->token_attribute_value_end = xml_reader->position;
                token = XML_TOKEN_ATTRIBUTE;
            }
        }
        else if (cur_state == XML_STATE_IN_ATTRIBUTE_VALUE) {
            if (ch == '"') {
                xml_reader->parser_state = XML_STATE_WAIT_FOR_ATTRIBUTE_OR_ENDING;
                xml_reader->token_attribute_value_end = xml_reader->position;
                token = XML_TOKEN_ATTRIBUTE;
            }
        }
        else if (cur_state == XML_STATE_BEGIN_COMMENT_TAG) {
            if (ch == '-') {
                xml_reader->position++;
                if (xml_reader->position < xml_reader->length) {
                    ch = xml_reader->buffer[xml_reader->position];
                    if (ch == '-') {
                        xml_reader->parser_state = XML_STATE_BEGIN_COMMENT;
                    }
                }
            }
        }
        else if (cur_state == XML_STATE_BEGIN_COMMENT) {
            xml_reader->parser_state = XML_STATE_IN_COMMENT;
            xml_reader->token_comment_start = xml_reader->position;
        }
        else if (cur_state == XML_STATE_IN_COMMENT) {
            if (ch == '-') {
                xml_reader->parser_state = XML_STATE_END_COMMENT_TAG;
                xml_reader->token_comment_end = xml_reader->position;
            }
        }
        else if (cur_state == XML_STATE_END_COMMENT_TAG) {
            if (ch == '-') {
                xml_reader->position++;
                if (xml_reader->position < xml_reader->length) {
                    ch = xml_reader->buffer[xml_reader->position];
                    if (ch == '>') {
                        xml_reader->parser_state = XML_STATE_INITIAL;
                        token = XML_TOKEN_COMMENT;
                    }
                    else {
                        xml_reader->parser_state = XML_STATE_IN_COMMENT;
                    }
                }
                else {
                    xml_reader->parser_state = XML_STATE_IN_COMMENT;
                }
            }
            else {
                xml_reader->parser_state = XML_STATE_IN_COMMENT;
            }
        }


       xml_reader->position++;
        if (token != XML_TOKEN_NONE || xml_error == TRUE) {
            break;
        }
    }
    return token;
}


void xmlInitWriter(xml_writer_t* xml_writer, byte* buf, SIZE_T len) {
    xml_writer->buffer = buf;
    xml_writer->length = len;
    xml_writer->write_position = 0;
    xml_writer->writer_state = XML_STATE_WRITER_INITIAL;
    xml_writer->tag_name_position_stack_size = 0;

    xml_writer->closing_tags_space = 0;

    xml_writer->rollback_write_position = 0;
    xml_writer->rollback_writer_state = XML_STATE_WRITER_INITIAL;
    xml_writer->rollback_tag_name_position_stack_size = 0;
    xml_writer->rollback_closing_tags_space = 0;
}

void xmlWriterMarkRollbackPoint(xml_writer_t *xml_writer) {
    xml_writer->rollback_write_position = xml_writer->write_position;
    xml_writer->rollback_writer_state = xml_writer->writer_state;
    xml_writer->rollback_tag_name_position_stack_size = xml_writer->tag_name_position_stack_size;
    xml_writer->rollback_closing_tags_space = xml_writer->closing_tags_space;
}

void xmlWriterReset(xml_writer_t* xml_writer) {
    xml_writer->write_position = 0;
    xml_writer->writer_state = XML_STATE_WRITER_INITIAL;
    xml_writer->tag_name_position_stack_size = 0;

    xml_writer->closing_tags_space = 0;

    xmlWriterMarkRollbackPoint(xml_writer);
}

boolean isXMLWriterReset(xml_writer_t *xml_writer) {
    if (xml_writer->write_position == 0
     && xml_writer->writer_state == XML_STATE_WRITER_INITIAL
     && xml_writer->tag_name_position_stack_size == 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void xmlWriterRollback(xml_writer_t *xml_writer) {
    xml_writer->write_position = xml_writer->rollback_write_position;
    xml_writer->writer_state = xml_writer->rollback_writer_state;
    xml_writer->tag_name_position_stack_size = xml_writer->rollback_tag_name_position_stack_size;
    xml_writer->closing_tags_space = xml_writer->rollback_closing_tags_space;
}

boolean xmlWriterCloseTag(xml_writer_t *xml_writer) {
    int retval;
    SIZE_T i;

    if (xml_writer->tag_name_position_stack_size == 0) {
        // Nothing to close
        return FALSE;
    }
    if (xml_writer->writer_state != XML_STATE_WRITER_TAG_HALF_CLOSED) {
        if (xml_writer->write_position < xml_writer->length) {
            xmlWriterMarkRollbackPoint(xml_writer);
            xml_writer->buffer[xml_writer->write_position++] = '>';
            xml_writer->writer_state = XML_STATE_WRITER_TAG_HALF_CLOSED;
            xml_writer->closing_tags_space -= 1;
        }
        else {
            return FALSE;
        }
    }

    --xml_writer->tag_name_position_stack_size;
    if (xml_writer->length <= xml_writer->write_position) {
        return FALSE;
    }
    retval = snprintf((char *) &xml_writer->buffer[xml_writer->write_position],
                      xml_writer->length - xml_writer->write_position,
                      "</");
 
    if (retval >= xml_writer->length - xml_writer->write_position) {
        return FALSE;
    }
    xml_writer->write_position += retval;
    xml_writer->closing_tags_space -= retval;

    for (i = xml_writer->tag_name_position_stack[xml_writer->tag_name_position_stack_size]; i < xml_writer->write_position && isalpha(xml_writer->buffer[i]) ; i++, xml_writer->write_position++) {
        xml_writer->buffer[xml_writer->write_position] = xml_writer->buffer[i];
        xml_writer->closing_tags_space--;
    }

    if (xml_writer->write_position < xml_writer->length) {
        xml_writer->buffer[xml_writer->write_position++] = '>';
        xml_writer->closing_tags_space -= 1;
        xmlWriterMarkRollbackPoint(xml_writer);
    }
    else {
        return FALSE;
    }

    //xml_writer->writer_state = XML_STATE_WRITER_INITIAL;
    return TRUE;
}

boolean xmlWriterFinalize(xml_writer_t *xml_writer) {
    while (TRUE == xmlWriterCloseTag(xml_writer)) {

    };
    if (xml_writer->tag_name_position_stack_size == 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

boolean xmlWriterStartTag(xml_writer_t *xml_writer, const char* tag_name) {
    int retval;

    if (xml_writer->tag_name_position_stack_size == sizeof(xml_writer->tag_name_position_stack)) {
        // The tag stack is full, we cannot add any more tags
        return FALSE;
    }
    if (xml_writer->writer_state == XML_STATE_WRITER_TAG_OPENED) {
        if (xml_writer->write_position < xml_writer->length) {
            xmlWriterMarkRollbackPoint(xml_writer);
            xml_writer->buffer[xml_writer->write_position++] = '>';
            xml_writer->closing_tags_space -= 1;
        }
        else {
            return FALSE;
        }
    }
 
    xml_writer->closing_tags_space += strlen(tag_name) + strlen("></>");

    if (xml_writer->length <= (xml_writer->write_position + xml_writer->closing_tags_space)) {
        return FALSE;
    }
    retval = snprintf((char *) &xml_writer->buffer[xml_writer->write_position],
                      xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space),
                      "<%s",
                      tag_name);
    if (retval >= xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space)) {
        return FALSE;
    }
    xml_writer->tag_name_position_stack[xml_writer->tag_name_position_stack_size++] =
        xml_writer->write_position + 1;
    //printf ("stack size: %d, pos0: %d, pos1: %d", xml_writer->tag_name_position_stack_size, xml_writer->tag_name_position_stack[0], xml_writer->tag_name_position_stack[1]);
    xml_writer->write_position += retval;
    xml_writer->writer_state = XML_STATE_WRITER_TAG_OPENED;
    //xmlWriterMarkRollbackPoint(xml_writer);
    return TRUE;
}

boolean xmlWriterStartChildTag(xml_writer_t *xml_writer, const char* tag_name) {
    while (xml_writer->tag_name_position_stack_size > 1) {
        if (FALSE == xmlWriterCloseTag(xml_writer)) {
            return FALSE;
        }
    }
    return xmlWriterStartTag(xml_writer, tag_name);
}

boolean xmlWriterWriteAttribute(xml_writer_t *xml_writer,
                                const char* attribute_name,
                                const char* attribute_value) {
    int retval;

    if (xml_writer->writer_state != XML_STATE_WRITER_TAG_OPENED) {
        return FALSE;
    }
    if (xml_writer->length <= (xml_writer->write_position + xml_writer->closing_tags_space)) {
        return FALSE;
    }
    retval = snprintf((char *) &xml_writer->buffer[xml_writer->write_position],
                      xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space),
                      " %s=\"%s\"", attribute_name, attribute_value);
    if (retval >= xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space)) {
        return FALSE;
    }
    xml_writer->write_position += retval;
    return TRUE;
}

boolean xmlWriterWriteText(xml_writer_t *xml_writer, const char* text) {
    int retval;

    if (xml_writer->writer_state != XML_STATE_WRITER_TAG_OPENED) {
        return FALSE;
    }
    if (xml_writer->write_position < xml_writer->length) {
        xmlWriterMarkRollbackPoint(xml_writer);
        xml_writer->buffer[xml_writer->write_position++] = '>';
        xml_writer->closing_tags_space -= 1;
    }
    else {
        return FALSE;
    }
 
    if (xml_writer->length <= (xml_writer->write_position + xml_writer->closing_tags_space)) {
        return FALSE;
    }
    retval = snprintf((char *) &xml_writer->buffer[xml_writer->write_position],
                      xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space),
                      "%s", text);
    if (retval >= xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space)) {
        return FALSE;
    }
    xml_writer->write_position += retval;
    xml_writer->writer_state = XML_STATE_WRITER_TAG_HALF_CLOSED;
    xmlWriterMarkRollbackPoint(xml_writer);
    return TRUE;
}

boolean xmlWriterWriteTextFromBuffer(xml_writer_t *xml_writer, byte* buffer, SIZE_T length) {

    if (xml_writer->writer_state != XML_STATE_WRITER_TAG_OPENED) {
        return FALSE;
    }
    if (xml_writer->write_position < xml_writer->length) {
        xmlWriterMarkRollbackPoint(xml_writer);
        xml_writer->buffer[xml_writer->write_position++] = '>';
        xml_writer->closing_tags_space -= 1;
    }
    else {
        return FALSE;
    }
    if (length + (xml_writer->write_position + xml_writer->closing_tags_space) > xml_writer->length) {
        return FALSE;
    }
    memscpy(&xml_writer->buffer[xml_writer->write_position], xml_writer->length,
                buffer,
                length);

    xml_writer->write_position += length;
    xml_writer->writer_state = XML_STATE_WRITER_TAG_HALF_CLOSED;
    xmlWriterMarkRollbackPoint(xml_writer);
    return TRUE;
}

boolean xmlWriterWriteAttributeValueFromBuffer(xml_writer_t *xml_writer,
                                               const char* attribute_name,
                                               byte* buffer,
                                               SIZE_T length) {
    int retval;

    if (xml_writer->writer_state != XML_STATE_WRITER_TAG_OPENED) {
        return FALSE;
    }
    if (xml_writer->length <= (xml_writer->write_position + xml_writer->closing_tags_space)) {
        return FALSE;
    }
    retval = snprintf((char *) &xml_writer->buffer[xml_writer->write_position],
                      xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space),
                      " %s=\"", attribute_name);
    if (retval >= xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space)) {
        return FALSE;
    }
    xml_writer->write_position += retval;

    if (length + (xml_writer->write_position + xml_writer->closing_tags_space) > xml_writer->length) {
        return FALSE;
    }
    memscpy(&xml_writer->buffer[xml_writer->write_position], xml_writer->length, 
                buffer,
                length);
    xml_writer->write_position += length;
    if ((xml_writer->write_position + xml_writer->closing_tags_space) < xml_writer->length) {
        xml_writer->buffer[xml_writer->write_position++] = '"';
    }
    else {
        return FALSE;
    }

    return TRUE;
}

boolean xmlWriterWriteMultipleAttributes(xml_writer_t *xml_writer,
                                         int n_args,
                                         va_list ap) {
    int retval;
    //va_list ap;
    int i;
    char format;
    int int_arg;
    char char_arg;
    char* string_arg;
    char *attribute_name;

    if (xml_writer->writer_state != XML_STATE_WRITER_TAG_OPENED) {
        return FALSE;
    }
     
    //va_start(ap, n_args);
    for (i = 0; i < n_args; i++) {
        attribute_name = va_arg(ap, char *);
        if (xml_writer->length <= (xml_writer->write_position + xml_writer->closing_tags_space)) {
            return FALSE;
        }
        retval = snprintf(
                    (char *) &xml_writer->buffer[xml_writer->write_position],
                    xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space),
                    " %s=",
                    attribute_name);

        if (retval >= xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space)) {
            return FALSE;
        }
        xml_writer->write_position += retval;

        if (xml_writer->length <= (xml_writer->write_position + xml_writer->closing_tags_space)) {
            return FALSE;
        }
        format = (char) va_arg(ap, int);
        switch(format) {
            case 's':
                string_arg = va_arg(ap, char *);
                retval = snprintf(
                    (char *) &xml_writer->buffer[xml_writer->write_position],
                    xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space),
                    "\"%s\"",
                    string_arg);
                break;
            case 'd':
                int_arg = va_arg(ap, int);
                retval = snprintf(
                    (char *) &xml_writer->buffer[xml_writer->write_position],
                    xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space),
                    "\"%d\"",
                    int_arg);
                break;
            case 'c':
                char_arg = (char) va_arg(ap, int);
                retval = snprintf(
                    (char *) &xml_writer->buffer[xml_writer->write_position],
                    xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space),
                    "\"%c\"",
                    char_arg);
                break;
            default:
                return FALSE;
        }
        if (retval >= xml_writer->length - (xml_writer->write_position + xml_writer->closing_tags_space)) {
            return FALSE;
        }
        xml_writer->write_position += retval;
    }
    return TRUE;
}

boolean xmlWriterWriteMultipleAttributesWrapper(xml_writer_t *xml_writer,
                                                int n_args,
                                                ...) {
    boolean retval;

    va_list args;
    va_start (args, n_args);
    retval = xmlWriterWriteMultipleAttributes(xml_writer, n_args, args);
    va_end (args);
    return retval;
}
 



// These RAW write APIs offer no assistance. Use carefully

boolean xmlWriterRawWriteArgList(xml_writer_t *xml_writer,
                                 const char* format,
                                 va_list args) {
    int written;
    if (xml_writer->write_position >= xml_writer->length) {
        return FALSE;
    }
    written = vsnprintf(
                (char *) &xml_writer->buffer[xml_writer->write_position],
                xml_writer->length - xml_writer->write_position,
                format,
                args);
    if (written >= xml_writer->length - xml_writer->write_position) {
        return FALSE;
    }
    xml_writer->write_position += written;
    return TRUE;
}

boolean xmlWriterRawWrite(xml_writer_t *xml_writer,
                          const char* format,
                          ...) {
    boolean retval;
    va_list args;
    va_start (args, format);
    retval = xmlWriterRawWriteArgList(xml_writer, format, args);
    va_end (args);
    return retval;
}

boolean xmlWriterRawWriteFromBuffer(xml_writer_t *xml_writer,
                                    byte* buffer,
                                    SIZE_T length) {
    if (xml_writer->write_position + length > xml_writer->length) {
        return FALSE;
    }
    memscpy(&xml_writer->buffer[xml_writer->write_position], xml_writer->length, 
                buffer,
                length);

    xml_writer->write_position += length;
    return TRUE;
}

boolean xmlWriterRawStartTag(xml_writer_t *xml_writer, const char* tag_name) {
    return xmlWriterRawWrite(xml_writer, "<%s", tag_name);
}

boolean xmlWriterRawWriteAttributeValueFromBuffer(xml_writer_t *xml_writer,
                                                 const char* name,
                                                 byte* buffer,
                                                 SIZE_T length) {
    // ideally we should do everything in a single raw write
    // since it is easier to break down though, we must then
    // check that everything will fit in

    if (xml_writer->write_position + strlen(name) + length + strlen(" =\"\"")
        > xml_writer->length) {
        return FALSE;
    }
    xmlWriterRawWrite(xml_writer, " %s=\"", name);
    xmlWriterRawWriteFromBuffer(xml_writer, buffer, length);
    xmlWriterRawWrite(xml_writer, "\"");
    return TRUE;
}

boolean xmlWriterRawWriteAttribute(xml_writer_t *xml_writer,
                                   const char* name,
                                   const char* value) {
    return xmlWriterRawWrite(xml_writer, " %s=\"%s\"", name, value);
}

boolean xmlWriterRawCloseTag(xml_writer_t *xml_writer) {
    return xmlWriterRawWrite(xml_writer, " />");
}

boolean xmlWriterRawCloseTagName(xml_writer_t *xml_writer,
                                 const char *tag_name) {
    return xmlWriterRawWrite(xml_writer, "</%s>", tag_name);
}

boolean xmlWriterRawStartTagNoAttributes(xml_writer_t *xml_writer,
                                        const char *tag_name) {
    return xmlWriterRawWrite(xml_writer, "<%s>", tag_name);
}

/*
int main() {
    char filename[] = "temp.xml";
    unsigned char buf[16384];
    FILE* fp = fopen(filename, "w");
    SIZE_T length;

    xml_writer_t my_writer;
    xmlInitWriter(&my_writer, buf, sizeof(buf));
    xmlWriterStartTag(&my_writer, "data");
    xmlWriterWriteAttribute(&my_writer, "version", "3");
    xmlWriterStartTag(&my_writer, "qwerty");
    xmlWriterWriteAttribute(&my_writer, "somefield", "50");
    xmlWriterStartTag(&my_writer, "dvorak");
    xmlWriterWriteAttribute(&my_writer, "orother", "68");
    xmlWriterWriteText(&my_writer, "logging here");
    //xmlWriterCloseTag(&my_writer);
    xmlWriterFinalize(&my_writer);

    length = fwrite(my_writer.buffer, sizeof(unsigned char), my_writer.write_position, fp);
    fclose(fp);
    return 0;
}

int main() {
    char filename[] = "rawprogram0.xml";
    unsigned char buf[16384];
    FILE* fp = fopen(filename, "r");
    SIZE_T length = fread(buf, sizeof(unsigned char), sizeof(buf), fp);
    fclose(fp);

    xml_reader_t my_file;
    xmlInitReader(&my_file, buf, length);
    xml_token_t token;
    byte* start;
    byte* end;
    byte tempbuf[1024];
    while ((token = xmlGetToken(&my_file)) != XML_TOKEN_NONE) {
        start = end = NULL;

        if (token == XML_TOKEN_TAG) {
            printf("Tag: ");
            start = &my_file.buffer[my_file.token_tag_start];
            end = &my_file.buffer[my_file.token_tag_end];
            
            for (; start != end; start++)
                printf("%c", *start);
            printf("  %d", xmlIsTag(&my_file, "program"));
            printf("\n");
            
        }
        else if (token == XML_TOKEN_ATTRIBUTE) {
            printf("Attribute: ");
            start = &my_file.buffer[my_file.token_attribute_name_start];
            end = &my_file.buffer[my_file.token_attribute_name_end];
            
            for (; start != end; start++)
                printf("%c", *start);
            printf(" Value: ");
            start = &my_file.buffer[my_file.token_attribute_value_start];
            end = &my_file.buffer[my_file.token_attribute_value_end];
            
            for (; start != end; start++)
                printf("%c", *start);

            printf("  %d", xmlIsAttribute(&my_file, "filename"));
            xmlGetAttributeValue(&my_file, tempbuf, sizeof(tempbuf));
            printf("  got: %s", tempbuf); 
            printf("\n");
        }
    }
    return 0;
}
*/

