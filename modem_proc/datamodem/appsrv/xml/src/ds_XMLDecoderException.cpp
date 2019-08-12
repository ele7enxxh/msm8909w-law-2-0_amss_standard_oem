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
#include "ds_XMLDecoderException.h"

#include "ds_ASString.h"
namespace ASXML = Appsrv::XML;

/* =============================================================================
 *                            XMLDecoderException
 * ========================================================================== */
namespace
{

const char EMPTY_ERR_MSG[] = "Empty message.";
const char EMPTY_DETAILS[] = "No detail available.";

} // namespace


ASXML::XMLDecoderException::XMLDecoderException()
: err_msg(EMPTY_ERR_MSG), detail(EMPTY_DETAILS) { }

ASXML::XMLDecoderException& ASXML::XMLDecoderException::get_instance()
{
  static XMLDecoderException exception;
  return exception;
}



const char* ASXML::XMLDecoderException::what()
{
  if(err_msg.empty())
    return EMPTY_ERR_MSG;
  return err_msg.c_str();
}

const char* ASXML::XMLDecoderException::get_details()
{
  if(detail.empty())
    return EMPTY_DETAILS;
  return detail.c_str();
}



void ASXML::XMLDecoderException::raise(bool& input_flag, const char* msg)
{
  input_flag = true;
  err_msg    = msg;
}

void ASXML::XMLDecoderException::raise(bool& input_flag, const char* msg, const char* details)
{
  input_flag = true;
  err_msg    = msg;
  detail     = details;
}

void ASXML::XMLDecoderException::raise(bool& input_flag, const char* msg, const ASString& details)
{
  input_flag = true;
  err_msg    = msg;
  detail     = details;
}


void ASXML::XMLDecoderException::reset()
{
  err_msg = EMPTY_ERR_MSG;
  detail  = EMPTY_DETAILS;
}
