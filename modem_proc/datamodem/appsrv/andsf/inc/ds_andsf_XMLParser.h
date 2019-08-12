/*==============================================================================

                        ds_andsf_XMLParser.h

GENERAL DESCRIPTION
  Decoder for ANDSF -> ISRP -> ServiceBased

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
03/20/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_ANDSF_XMLPARSER_H
#define DS_ANDSF_XMLPARSER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_ASVector.h"

class ASBuffer;


namespace Appsrv
{
namespace XML
{

class ANDSF_Isrp;
class XMLElement;

} /* namespace XML */


namespace ANDSF
{

/* =============================================================================
CLASS         XMLParser_ServiceBased

DESCRIPTION   Parser for ANDSF ISRP ServiceBased
============================================================================= */
class XMLParser_ServiceBased
{
public:
  XMLParser_ServiceBased();
  ~XMLParser_ServiceBased();

  /* ===========================================================================
  FUNCTION parse_andsf

  DESCRIPTION
    ANDSF parser constructor. It will parse the given xml content first. Then
    if the root is not NULL, it will parse it into an ANDSF structure. mo will
    be NULL if decoding fails.

  PARAMETERS
    xml_content - The XML content to parse. If this is the only given parameter
                  client MUST guarantee that it is a null terminating c-string.
    len         - Length of xml_content. Client must guarantee that xml_content
                  size is at least len.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  =========================================================================== */
  bool parse_andsf(const char* andsf_xml_content, uint32 subs_idx);

  /* Accessors */
  const ASVector<XML::ANDSF_Isrp*>& get_isrp_list() const;
  const XML::ANDSF_Isrp*            get_isrp_at(uint32 at) const;
  uint32                            get_isrp_size() const;

private:
  bool get_andsf_from_efs(const char* file_path, uint32 subs_idx, ASBuffer& file_content);
  bool parse_helper(const char* xml_content, uint32 len);
  void clear();

  bool parse_isrp(const XML::XMLElement* isrps);

private:
  ASVector<XML::ANDSF_Isrp*> isrp_list;
};



} /* namespace ANDSF */
} /* namespace Appsrv */

#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_XMLPARSER_H */
