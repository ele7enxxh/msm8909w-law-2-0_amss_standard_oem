/*==============================================================================

                        ds_XMLDecoder_ANDSF.h

GENERAL DESCRIPTION
  Decoder for ANDSF

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
#ifndef DS_XML_DECODER_ANDSF_H
#define DS_XML_DECODER_ANDSF_H

#include "ds_XMLDecoder.h"


namespace Appsrv
{
namespace XML
{
  class ANDSFManagementObject;


/*===========================================================================
CLASS XMLDecoder_ADNSF

DESCRIPTION
  Decoder for ANDSF. mo will be null if decode failed
===========================================================================*/
class XMLDecoder_ANDSF : private XMLDecoder
{
public:
  /*===========================================================================
  FUNCTION XMLDecoder_ANDSF CONSTRUCTOR

  DESCRIPTION
    ANDSF decoder constructor. It will decode the given xml content first. Then
    if the root is not NULL, it will decode it into an ANDSF structure. mo will
    be NULL if decoding fails.

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
  XMLDecoder_ANDSF(const char* xml_content);
  XMLDecoder_ANDSF(const char* xml_content, uint32 len);
  ~XMLDecoder_ANDSF();

  ANDSFManagementObject* get_andsf_mo();

protected:
  virtual void clean();

private:
  void decode();

  ANDSFManagementObject* mo;
};

} /* namespace XML */
} /* namespace Appsrv */


#endif /* DS_XML_DECODER_ANDSF_H */
