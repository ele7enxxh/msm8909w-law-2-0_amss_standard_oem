/*==============================================================================

                            ds_ASString.cpp

GENERAL DESCRIPTION
  A string class with utility functions for parsing AS.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/20/15    ml     Use memory from modem heap
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_ASString.h"

#include "ds_appsrv_mem.h"
#include "data_msg.h"
#include <stringl/stringl.h>
#include <cctype> // isspace, tolower

// #define DS_ASSTRING_MAX_LEN 4294967295 // uint32 max
#define DS_ASSTRING_MAX_LEN 400000

static const char AS_ESCAPE_MARK = '&';
static const char EMPTY_STRING[] = "";

static const char* ESCAPE_CHARS[] = {"&lt;", "&gt;", "&amp;", "&apos;", "&quot;"};
static const char UNESCAPE_CHARS[] = {'<', '>', '&', '\'', '"'};
static const uint32 NUM_ESCAPE_CHARS = 5;


static bool has_string_at_start(const char* buf, const char* cmp);


ASString::ASString()
: str(NULL) { }


ASString::ASString(const char* src)
: str(NULL)
{
  if(NULL == src)
  {
    str = NULL;
    return;
  }
  copy_string(src, strlen(src));
}


ASString::ASString(const char* src, uint32 len)
: str(NULL)
{
  copy_string(src, len);
}


// copy constructor
ASString::ASString(ASString& src)
: str(NULL)
{
  copy_string(src.c_str(), src.size());
}


ASString::ASString(const ASString& src)
: str(NULL)
{
  copy_string(src.c_str(), src.size());
}


ASString::~ASString()
{
  if(NULL != str)
  {
    ds_appsrv_free(str);
  }
}

// Assumes this->str with no memory allocated
void ASString::copy_string(const char* src, uint32 len)
{
  if(NULL == src || 0 == len)
  {
    str = NULL;
    return;
  }
  if(len > DS_ASSTRING_MAX_LEN)
    return;

  str = (char*)ds_appsrv_alloc(sizeof(char), len + 1);
  if(NULL != str)
  {
    memscpy(str, len, src, len);
  }
}






ASString& ASString::operator=(const ASString& rhs)
{
  if(&rhs != this)
  {
    if(NULL != str)
      ds_appsrv_free(str);

    if(NULL == rhs.str)
      str = NULL;
    else
      copy_string(rhs.str, rhs.size());
  }
  return *this;
}


ASString& ASString::operator=(const char* rhs)
{
  if(NULL != str)
    ds_appsrv_free(str);

  if(NULL == rhs)
    str = NULL;
  else
    copy_string(rhs, strlen(rhs));

  return *this;
}



char ASString::operator[](const int index) const
{
  return str[index];
}


char& ASString::operator[](int index)
{
  return str[index];
}


const char* ASString::c_str() const
{
  if(NULL == str)
    return EMPTY_STRING;
  return str;
}


uint32 ASString::size() const
{
  return length();
}


uint32 ASString::length() const
{
  if(NULL == str)
    return 0;
  return (uint32)strlen(str);
}


bool ASString::empty() const
{
  return (0 == length());
}

void ASString::remove_trailing_spaces()
{
  uint32 end = length();

  if(0 == end)
    return;

  while(0 != end)
  {
    if(!isspace(str[--end]))
    {
      end++;
      break;
    }
  }

  str[end] = '\0';
}




bool ASString::resolve_xml_escapes()
{
  bool   replaced = false;
  uint32 counter  = 0;
  uint32 len      = length();
  char*  buf      = str;
  char*  new_str  = NULL;

  if(0 == len)
    return true;

  new_str = (char*)ds_appsrv_alloc(sizeof(char), len + 1);

  if(NULL == new_str)
    return true;

  while('\0' != *buf && counter < len)
  {
    if(AS_ESCAPE_MARK == *buf)
    {
      replaced = false;
      for(uint32 i=0; i<NUM_ESCAPE_CHARS; i++)
      {
        if(has_string_at_start(buf, ESCAPE_CHARS[i]))
        {
          new_str[counter++] = UNESCAPE_CHARS[i];
          buf += strlen(ESCAPE_CHARS[i]);
          replaced = true;
          break;
        }
      }

      if(!replaced)
      {
        ds_appsrv_free(new_str);
        return false;
      }
    }
    else
    {
      new_str[counter++] = *buf;
      buf++;
    }
  }

  ds_appsrv_free(str);
  str = new_str;

  return true;
}



void ASString::to_lower()
{
  uint32 len = length();
  for(uint32 i = 0; i < len; ++i)
  {
    str[i] = tolower(str[i]);
  }
}



bool ASString::limit_cmp(const char* cstr, const uint32 len) const
{
  return (0 == strncmp(str, cstr, len));
}



void ASString::append(const char* append_str)
{
  if(NULL == append_str)
    return;

  append(append_str, strlen(append_str));
}



void ASString::append(const char* append_str, const uint32 len)
{
  uint32 prev_size, new_size;

  if(NULL == append_str)
    return;

  if(strlen(append_str) < len || 0 == len)
    return;

  prev_size = length();
  new_size  = prev_size + len + 1;

  if(new_size < prev_size || new_size < len || DS_ASSTRING_MAX_LEN < new_size) // overflow?
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "append - append size too long");
    return;
  }

  char* tmp = (char*)ds_appsrv_alloc(sizeof(char), new_size);
  if(NULL == tmp)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "append - Failed to allocate memory for result string");
    return;
  }

  if(NULL != str)
  {
    // copy the current string to the new string.
    memscpy(tmp, new_size, str, prev_size);
    ds_appsrv_free(str);
    str = NULL;
  }
  // add the new string
  memscpy(tmp+prev_size, new_size - prev_size, append_str, len);

  str = tmp;
}


void ASString::append(const ASString& append_str)
{
  if(append_str.empty())
    return;
  append(append_str.c_str(), append_str.length());
}





bool operator== (const ASString& lhs, const ASString& rhs)
{
  return (0 == strcasecmp(lhs.c_str(), rhs.c_str()));
}

bool operator!= (const ASString& lhs, const ASString& rhs)
{
  return (0 != strcasecmp(lhs.c_str(), rhs.c_str()));
}


bool operator== (const ASString& lhs, const char* rhs)
{
  return (0 == strcasecmp(lhs.c_str(), rhs));
}

bool operator!= (const ASString& lhs, const char* rhs)
{
  return (0 != strcasecmp(lhs.c_str(), rhs));
}


/*
res <  0 if lhs <  rhs
res == 0 if lhs == rhs
res >  0 if lhs <  rhs
*/
bool operator< (const ASString& lhs, const ASString& rhs)
{
  return (0 > strcasecmp(lhs.c_str(), rhs.c_str()));
}

bool operator> (const ASString& lhs, const ASString& rhs)
{
  return (0 < strcasecmp(lhs.c_str(), rhs.c_str()));
}







static bool has_string_at_start(const char* buf, const char* cmp)
{
  return (0 == strncasecmp(buf, cmp, strlen(cmp)));
}

