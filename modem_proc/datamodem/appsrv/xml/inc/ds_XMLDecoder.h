/*==============================================================================

                            ds_XMLDecoder.h

GENERAL DESCRIPTION
  Base XML decoder

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

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
#ifndef DS_XML_DECODER
#define DS_XML_DECODER

#include "ds_ASString.h"
#include "ds_ASStringStream.h"
#include "ds_XMLDecoderException.h"


namespace Appsrv
{
namespace XML
{

class XMLElement;



/*==============================================================================
CLASS XMLDecoder

DESCRIPTION
  Base XML decoder for extracting the raw input data into XMLElement structure

==============================================================================*/
class XMLDecoder
{
public:
  /*===========================================================================
  FUNCTION XMLDecoder CONSTRUCTOR

  DESCRIPTION
    XML decoder constructor. It will decode the given xml content to a formatted
    structure. root will be NULL if decoding fails.

  PARAMETERS
    xml_content - The XML content to decode. If this is the only given parameter
                  client MUST guarantee that it is a null terminating c-string.
    len         - Length of xml_content. Client must guarantee that xml_content
                  size is at least len.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  XMLDecoder(const char* xml_content);
  XMLDecoder(const char* xml_content, uint32 len);
  ~XMLDecoder();

protected:
  /*===========================================================================
  FUNCTION XMLDecoder::cleanup

  DESCRIPTION
    Cleanup function for failure/destructor

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  virtual void clean();

private:
  /*===========================================================================
  FUNCTION XMLDecoder::decode

  DESCRIPTION
    Sets up the input stream so that it can be looped starting from the root

  PARAMETERS
    [In] raw - The raw string of the XML data to parse

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void decode(ASString& raw);

  /*===========================================================================
  FUNCTION XMLDecoder::decode_loop

  DESCRIPTION
    Decoder helper. Loops through the XML starting from the root until its
    closure. Anything beyond the root's closure tag will be ignored.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void decode_loop();

  /*===========================================================================
  FUNCTION XMLDecoder::decode_attr

  DESCRIPTION
    Decoder helper. Adds all attributes in the given stream to the current element

  PARAMETERS
    current  - The current element to add the attributes to
    attr_iss - The attribute string stream to parse

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void decode_attr(XMLElement* current, ASStringStream& attr_iss);


  /*===========================================================================
  FUNCTION XMLDecoder::get_initial_content

  DESCRIPTION
    Get the initial element of the XML from the stream.

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  ASString get_initial_content();

  /*===========================================================================
  FUNCTION XMLDecoder::find_end_comment

  DESCRIPTION
    Looks for the end comment indicator (-->) in the stream

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void find_end_comment();

  /*===========================================================================
  FUNCTION XMLDecoder::get_next_content

  DESCRIPTION
    Gets the string starting from the current stream until it finds the
    delimiter or eof.

  PARAMETERS
    [In]  xss     - The string stream
    [In]  delim   - The delimiter to stop at.
    [Out] err_msg - Error code

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  ASString get_next_content(ASStringStream& xss, const char delim, const char* err_msg);

  // disable default & copy constructor
  XMLDecoder();
  XMLDecoder(XMLDecoder &src);
  XMLDecoder(const XMLDecoder &src);
  XMLDecoder& operator=(const XMLDecoder &rhs);
  XMLDecoder& operator=(const char* rhs);



  /* Member */
public:
  XMLElement* root; // The root element of the XML. This will be NULL if XML parsing fails

protected:
  bool flag; // Set this to true when something goes wrong. Add details in XMLDecoderException singleton

private:
  ASStringStream iss; // XML input stream
};


} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_DECODER */
