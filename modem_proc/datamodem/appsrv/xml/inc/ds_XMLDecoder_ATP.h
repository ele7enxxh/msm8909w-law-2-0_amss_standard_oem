/*==============================================================================

                        ds_XMLDecoder_ATP.h

GENERAL DESCRIPTION
  Decoder for ATP

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
#ifndef DS_XML_DECODER_ATP_H
#define DS_XML_DECODER_ATP_H

#include "ds_XMLDecoder.h"
#include "ds_xml_ATP_Policy.h"

namespace Appsrv
{
namespace XML
{

class XMLElement;

} /* namespace XML */
} /* namespace Appsrv */

/*===========================================================================
CLASS XMLDecoder_ATP

DESCRIPTION
  Decoder for ATP. policy will be null if decode failed
===========================================================================*/
class XMLDecoder_ATP : private Appsrv::XML::XMLDecoder
{
public:
  /*===========================================================================
  FUNCTION XMLDecoder_ATP CONSTRUCTOR

  DESCRIPTION
    ATP decoder constructor. It will decode the given xml content first. Then
    if the root is not NULL, it will decode it into an ATP structure. policy
    will be NULL if decoding fails.

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
  XMLDecoder_ATP(const char* xml_content);
  XMLDecoder_ATP(const char* xml_content, uint32 len);
  ~XMLDecoder_ATP();

protected:
  virtual void clean();

private:
  void decode(Appsrv::XML::XMLElement* elem);


public:
  ATP_Policy* policy;
};



#endif /* DS_XML_DECODER_ATP_H */
