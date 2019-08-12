/*==============================================================================

                        ds_JSONDocument.h

GENERAL DESCRIPTION

  Copyright (c) 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/21/16    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_JSON_DOCUMENT_H
#define DS_JSON_DOCUMENT_H

#include "comdef.h"

#include "ds_JSONValueType.h"

#include "ds_ASList.h"
#include "ds_ASStrStream.h"

class ASString;

namespace Appsrv
{
namespace JSON
{


class JSONDocument
{
public:
  JSONDocument();
  JSONDocument(const ASString& json_content);
  JSONDocument(const char* json_content, uint32 content_size);
  ~JSONDocument();

  void clear();

  JSONValueType* get_root();
  const JSONValueType* get_root() const;

  /* Read-only accessors */
  const JSONValueType& operator[](const ASString& name) const;
  const JSONValueType& operator[](uint32 offset) const;

  bool parse(const char* file_path);
  bool parse(const char* json_content, uint32 content_len);
  bool serialize(const char* file_path);
  bool serialize(const char*& content_ptr, uint32& content_size);

private:
  bool parse_helper(Utils::ASIStream& json_iss);


private:
  JSONValueType root; // Root is object or array

  Utils::ASOStringStream serialize_oss; // Used for JSON serialization on cache only
};



} /* namespace JSON */
} /* namespace Appsrv */

#endif /* DS_JSON_DOCUMENT_H */
