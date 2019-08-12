/*==============================================================================

                          ds_XMLDecoderException.h

GENERAL DESCRIPTION
  A singleton class to give descriptive information of why the XML decoding failed

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  reset() must be called before reusing the object, else it will contain
  previous error message and details

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_XML_DECODER_EXCEPTION_H
#define DS_XML_DECODER_EXCEPTION_H

#include "ds_ASString.h"


namespace Appsrv
{
namespace XML
{

/* Error messages */
const char ERR_MSG_EOF[]               = "XMLDecoderException : Incomplete XML";
const char ERR_MSG_MALLOC[]            = "XMLDecoderException : Failed to allocate memory";
const char ERR_MSG_MALLOC_OBJ[]        = "XMLDecoderException : Failed to allocate memory for object";
const char ERR_MSG_MALLOC_VEC[]        = "XMLDecoderException : Failed to allocate memory for vector";
const char ERR_MSG_INIT[]              = "XMLDecoderException : Error at beginning of XML";
const char ERR_MSG_COMMENT[]           = "XMLDecoderException : Failed to find end of comment";
const char ERR_MSG_NO_BRACKET[]        = "XMLDecoderException : Empty bracket";
const char ERR_MSG_INVALID_TAG[]       = "XMLDecoderException : Invalid tag";
const char ERR_MSG_INVALID_CLOSURE[]   = "XMLDecoderException : Invalid element closure";
const char ERR_MSG_INVALID_DOC[]       = "XMLDecoderException : Invalid doctype declaration";
const char ERR_MSG_ATTR_DECODE[]       = "XMLDecoderException : Attribute error";
const char ERR_MSG_ESCAPE_DECODE[]     = "XMLDecoderException : Escape decode error";
const char ERR_MSG_NULL_ELEMENT[]      = "XMLDecoderException : NULL element!";
const char ERR_MSG_NULL_ATTRIBUTE[]    = "XMLDecoderException : NULL attribute!";
const char ERR_MSG_INVALID_ATTR_NAME[] = "XMLDecoderException : Invalid attribute name!";
const char ERR_MSG_EMPTY_CONTENT[]     = "XMLDecoderException : Empty content!";
const char ERR_MSG_ATTR_COUNT[]        = "XMLDecoderException : Incorrect number of attributes!";
const char ERR_MSG_CHILD_LOW[]         = "XMLDecoderException : Not enough children!";
const char ERR_MSG_CHILD_HIGH[]        = "XMLDecoderException : Too many children!";
const char ERR_MSG_OTHER[]             = "XMLDecoderException : Other";

const char ERR_MSG_PARSE_INCOMPLETE[] = "Failed parsing - Unordered or unknown element";
const char ERR_MSG_PARSE_EXCESS[]     = "Failed parsing - More elements than expected";
const char ERR_MSG_PARSE_MISSING[]    = "Failed parsing - Missing required element";
const char ERR_MSG_PARSE_INVALID[]    = "Failed parsing - Invalid format or content";

/*==============================================================================
CLASS XMLDecoderException

DESCRIPTION
  A singleton class to give descriptive information of why the XML decoding failed
==============================================================================*/
class XMLDecoderException
{
public:
  // XMLDecoderException(const char* err);
  // XMLDecoderException(const char* err, const char* details);
  // XMLDecoderException(const char* err, const ASString& details);
  // XMLDecoderException(const char* fnc_name, const char* err_msg, const char* details);
  // ~XMLDecoderException();

  /*===========================================================================
  FUNCTION XMLDecoderException::get_instance

  DESCRIPTION
    Get the singleton XMLDecoderException object

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  static XMLDecoderException& get_instance();

  /*===========================================================================
  FUNCTION XMLDecoderException::what

  DESCRIPTION
    Returns the error messaage

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  const char* what();

  /*===========================================================================
  FUNCTION XMLDecoderException::get_details

  DESCRIPTION
    Returns a more detailed information on the error. Might be empty.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  const char* get_details();

  /*===========================================================================
  FUNCTION XMLDecoderException::raise

  DESCRIPTION
    Raise an exception. (Nothing really happens with the stack.)
    Error message and details will be set

  PARAMETERS
    [In] msg     - The error message
    [In] details - Error detail information

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void raise(bool& flag, const char* msg);
  void raise(bool& flag, const char* msg, const char* details);
  void raise(bool& flag, const char* msg, const ASString& details);

  /*===========================================================================
  FUNCTION XMLDecoderException::reset

  DESCRIPTION
    Reset the error messages to be empty.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void reset();

private:
  ASString err_msg; // The error message
  ASString detail;  // Error details

  // Hide constructors and assignment operator to make this class singleton
  XMLDecoderException();
  XMLDecoderException(XMLDecoderException &src);
  XMLDecoderException(const XMLDecoderException &src);
  XMLDecoderException& operator=(const XMLDecoderException &rhs);
};




} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_DECODER_EXCEPTION_H */
