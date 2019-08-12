/*==============================================================================

                              ds_json_document.cpp

GENERAL DESCRIPTION



  Copyright (c) 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/26/16    ml     Created file/Initial version.
==============================================================================*/
#include "ds_json_utils.h"

#include "ds_ASStream.h"
#include "ds_ASBuffer.h"
#include "ds_ASIOStream.h"

#include "data_msg.h"

#include <stringl/stringl.h>
#include <cctype>
#include <cstdlib>



namespace Appsrv
{
namespace JSON
{


bool ds_json_parse_string(Appsrv::Utils::ASIStream& json_iss, ASBuffer& str)
{
  char next_char;
  json_iss.get(next_char);

  while(json_iss.good())
  {
    if( 0 == isprint(next_char) )
    {
      return false;
    }

    if('\"' == next_char)
    {
      // Found end delim
      return true;
    }

    if( '\\' == next_char)
    {
      json_iss.get(next_char);
      if( !json_iss.good() )
      {
        return false;
      }

      switch(next_char)
      {
        case '\"':
        case '\\':
        case '/':
          break;

        case 'b':
          next_char = '\b';
          break;

        case 'f':
          next_char = '\f';
          break;

        case 'n':
          next_char = '\n';
          break;

        case 'r':
          next_char = '\r';
          break;

        case 't':
          next_char = '\t';
          break;

        case 'u':
        {
          // Hex string; get next 4 chars and convert
          char hexval[3];
          memset(hexval, 0, sizeof(char) * 3);

          for(uint16 i = 0; i < 4; ++i)
          {
            json_iss.get(next_char);
            if( !json_iss.good() || 0 == isxdigit(next_char) )
            {
              return false;
            }

            hexval[i%2] = next_char;

            if(1 == i)
            {
              if(0 != strtoul(hexval, NULL, 16))
              {
                // ???: Unsupported? UTF-8 only?
                DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "json_parse_string - UTF-16 escape not supported; UTF-8 only %d %d", hexval[0], hexval[1]);
                return false;
              }
            }
            else if(3 == i)
            {
              next_char = (char)strtoul(hexval, NULL, 16);
            }
          }
        }
        break;

        default:
        {
          DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "json_parse_string - Invalid escape char");
          return false;
        }
      } // switch
    } // if

    str.append(next_char);
    json_iss.get(next_char);
  }

  return false;
}




bool ds_json_parse_and_validate_raw_str(Appsrv::Utils::ASIStream& json_iss, ASBuffer& raw_str, const char* cmp_value)
{
  uint32 read_size = strlen(cmp_value);

  if(read_size < raw_str.size())
    return false;

  read_size -= raw_str.size();

  for(uint16 i = 0; i < read_size; ++i)
  {
    char read_char;
    json_iss.get(read_char);

    if( !json_iss.good() || 0 == isalpha(read_char) )
    {
      return false;
    }

    raw_str.append(read_char);
  }

  if ( 0 != strcasecmp(raw_str.c_str(), cmp_value) )
    return false;

  return true;
}



bool ds_json_peek_next_char(Appsrv::Utils::ASIStream& json_iss, char& next_char)
{
  next_char = json_iss.peek();

  while( json_iss.good() && isspace(next_char) )
  {
    json_iss.get();
    next_char = json_iss.peek();
  }

  return json_iss.good();
}



} /* namespace JSON */
} /* namespace Appsrv */
