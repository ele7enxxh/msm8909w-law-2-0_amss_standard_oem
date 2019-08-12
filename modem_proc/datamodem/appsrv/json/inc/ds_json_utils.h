/*==============================================================================

                        ds_json_utils.h

GENERAL DESCRIPTION
  Wrapper API for C to parse JSON

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
#ifndef DS_JSON_UTILS_H
#define DS_JSON_UTILS_H

#include "comdef.h"
#include "ds_ASVector.h"
#include "ds_ASString.h"

class ASBuffer;

namespace Appsrv
{
namespace Utils
{
class ASIStream;
} /* namespace Utils */


namespace JSON
{


const char DS_JSON_DELIM_NAME         = ':';
const char DS_JSON_DELIM_VALUE        = ',';
const char DS_JSON_DELIM_STRING       = '"';
const char DS_JSON_DELIM_BEGIN_ARRAY  = '[';
const char DS_JSON_DELIM_END_ARRAY    = ']';
const char DS_JSON_DELIM_BEGIN_OBJECT = '{';
const char DS_JSON_DELIM_END_OBJECT   = '}';

const char DS_JSON_NULL_STRING[]      = "null";
const char DS_JSON_TRUE_STRING[]      = "true";
const char DS_JSON_FALSE_STRING[]     = "false";



bool ds_json_parse_string(Utils::ASIStream& json_iss, ASBuffer& str);
bool ds_json_parse_and_validate_raw_str(Utils::ASIStream& json_iss, ASBuffer& str, const char* cmp_value);
bool ds_json_peek_next_char(Utils::ASIStream& json_iss, char& next_char);


} /* namespace JSON */
} /* namespace Appsrv */

#endif /* DS_JSON_UTILS_H */
