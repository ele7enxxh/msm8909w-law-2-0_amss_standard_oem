/*==============================================================================

                        ds_ASStringStream.cpp

GENERAL DESCRIPTION
  Utility class for AS decoder that functions like an input stream.

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
04/21/21    ml     Created file/Initial version.
==============================================================================*/

#include "ds_ASStringStream.h"
#include "ds_ASString.h"
#include <cstring>
#include <cctype> // isspace

ASStringStream::ASStringStream(ASString& str)
: iss(str.c_str()), start(iss), prev(iss), end_of_file(false), skip_space(false)
{ }



ASStringStream::ASStringStream(const char* str)
: iss(str), start(iss), prev(iss), end_of_file(false), skip_space(false)
{ }



ASStringStream::ASStringStream(ASString& str, bool skip)
: iss(str.c_str()), start(iss), prev(iss), end_of_file(false), skip_space(skip)
{ }



ASStringStream::ASStringStream(const char* str, bool skip)
: iss(str), start(iss), prev(iss), end_of_file(false), skip_space(skip)
{ }




bool ASStringStream::next_ptr()
{
  if(end_of_file)
    return end_of_file;

  iss++;
  if('\0' == *iss)
  {
    end_of_file = true;
  }

  return end_of_file;
}


void ASStringStream::back()
{
  if(iss > start)
    iss--;
}


char ASStringStream::peek()
{
  return *iss;
}

bool ASStringStream::get_next(ASString& dst)
{
  if(end_of_file)
  {
    dst = ASString();
    return end_of_file;
  }

  prev = iss;
  end_of_file = true;

  while(0 != isspace(*iss))
  {
    iss++;
  }
  if('\0' == *iss)
  {
    dst = ASString();
    return end_of_file;
  }

  const char* seeker = iss;

  while(*seeker != '\0')
  {
    if(0 != isspace(*seeker))
    {
      end_of_file = false;
      break;
    }
    seeker++;
  }

  dst = ASString(iss, (seeker - iss));
  if(!end_of_file)
    iss = seeker+1;
  else
    iss = seeker;

  return end_of_file;
}



bool ASStringStream::get_next(ASString& dst, const char delim)
{
  if(end_of_file)
  {
    dst = ASString();
    return end_of_file;
  }

  prev = iss;
  end_of_file = true;

  // Skip whitespace at front
  if(skip_space)
  {
    while(0 != isspace(*iss))
    {
      iss++;
    }

    if('\0' == *iss)
    {
      dst = ASString();
      return end_of_file;
    }
  }

  const char* seeker = iss;

  while(*seeker != '\0')
  {
    if(*seeker == delim)
    {
      end_of_file = false;
      break;
    }
    seeker++;
  }

  dst = ASString(iss, (seeker - iss));
  if(!end_of_file)
    iss = seeker+1;
  else
    iss = seeker;

  return end_of_file;
}


bool ASStringStream::get_next(ASString& dst, const char* delim)
{
  if(end_of_file)
  {
    dst = ASString();
    return end_of_file;
  }

  const char* seeker = strstr(iss, delim);
  if(NULL == seeker)
  {
    end_of_file = true;
    dst = iss; // copy all of rest
  }
  else
  {
    dst = ASString(iss, seeker - iss);

    uint32 delim_size = strlen(delim);
    iss = seeker + delim_size; // skip delim string
  }

  return end_of_file;
}




void ASStringStream::step_back()
{
  iss = prev;
}



void ASStringStream::reset()
{
  iss = start;
  prev = iss;
}




bool ASStringStream::good() const
{
  return !end_of_file;
}



bool ASStringStream::eof() const
{
  return end_of_file;
}
