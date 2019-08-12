/*==============================================================================

                            ds_http_uri.cpp

GENERAL DESCRIPTION
  Internal iface info structure and functions for DS HTTP

  Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/02/15    ml     Permanent redirect
07/28/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_uri.h"

#include "ds_ASFStream.h"
#include "ds_ASList.h"
#include "ds_ASMutex.h"
#include "ds_ASString.h"

#include "data_msg.h"
#include <stringl/stringl.h>
#include <cctype>
#include <cstdlib>

extern "C"
{
  #include "time_svc.h"
}

using namespace Appsrv::Utils;


namespace
{

/*==============================================================================
                          Local structures and types
==============================================================================*/
struct ds_http_redirect_cache
{
  ds_http_request_uri origin_uri;
  ds_http_request_uri redirect_uri;
  time_julian_type    expiration;
  bool                expiration_set;

  ds_http_redirect_cache();
  ds_http_redirect_cache(ASIStream& istream);
  ds_http_redirect_cache(
                         const ds_http_request_uri& origin,
                         const ds_http_request_uri& redirect,
                         const time_julian_type&    expire_date,
                         bool                       expire_set
                         );

  bool expired() const;


private:
  bool parse_uri_cache(ASIStream& istream, ds_http_request_uri& request_uri);
};

typedef ASList<ds_http_redirect_cache*>::iterator       redirect_iterator;
typedef ASList<ds_http_redirect_cache*>::const_iterator redirect_const_iterator;


/*==============================================================================
                            Local variables
==============================================================================*/
const char   DS_HTTP_REDIRECT_CACHE_PATH[]      = "/data/http_redirect.txt";
const char   DS_HTTP_REDIRECT_CACHE_TEMP_PATH[] = "/data/http_redirect_temp.txt";
const char   URI_PORT_DELIM                     = ':';
const char   URI_PATH_DELIM                     = '/';

/* Redirect cache mutex is needed since it will be accessed from ds_http_request
    constructor, which will be called from ds_http_create_***_request() API fcns. */
ASMutex                         ds_http_redirect_uri_crit_sect;
ASList<ds_http_redirect_cache*> redirect_cache_list;



/*==============================================================================
                            Local fucntions
==============================================================================*/
void remove_expired_redirect();
void update_permanent_uri_cache();
void append_permanent_uri_cache(const ds_http_redirect_cache* uri_cache);
uint16 proto_to_uint16(ds_http_protocol protocol);
ds_http_protocol uint16_to_proto(uint16 protocol);

ASOStream& operator<<(ASOStream& os, const ds_http_redirect_cache* redirect_cache);
ASOStream& operator<<(ASOStream& os, const ds_http_request_uri& uri_info);


} /* namespace */






/*==============================================================================
                            ds_http_request_uri
==============================================================================*/
ds_http_request_uri::ds_http_request_uri()
: protocol(DS_HTTP_PROTOCOL_HTTP), port(0)
{
  memset(hostname, 0, DS_HTTP_HOSTNAME_MAX_LEN+1);
  memset(path, 0, DS_HTTP_PATH_MAX_LEN+1);
}



ds_http_request_uri::ds_http_request_uri(const ds_http_request_uri& other)
: protocol(other.protocol), port(other.port)
{
  memset(hostname, 0, DS_HTTP_HOSTNAME_MAX_LEN+1);
  memset(path, 0, DS_HTTP_PATH_MAX_LEN+1);
  memscpy(hostname, DS_HTTP_HOSTNAME_MAX_LEN, other.hostname, DS_HTTP_HOSTNAME_MAX_LEN);
  memscpy(path, DS_HTTP_PATH_MAX_LEN, other.path, DS_HTTP_PATH_MAX_LEN);
}



ds_http_request_uri& ds_http_request_uri::operator=(const ds_http_request_uri& other)
{
  memset(hostname, 0, DS_HTTP_HOSTNAME_MAX_LEN+1);
  memset(path, 0, DS_HTTP_PATH_MAX_LEN+1);
  memscpy(hostname, DS_HTTP_HOSTNAME_MAX_LEN, other.hostname, DS_HTTP_HOSTNAME_MAX_LEN);
  memscpy(path, DS_HTTP_PATH_MAX_LEN, other.path, DS_HTTP_PATH_MAX_LEN);

  protocol = other.protocol;
  port     = other.port;

  return *this;
}



bool ds_http_request_uri::operator==(const ds_http_request_uri& other)
{
  return !( protocol != other.protocol ||
            port     != other.port     ||
            0        != memcmp(hostname, other.hostname, DS_HTTP_HOSTNAME_MAX_LEN) ||
            0        != memcmp(path, other.path, DS_HTTP_PATH_MAX_LEN) );
}






/*==============================================================================
                            ds_http_redirect_cache
==============================================================================*/
namespace
{
ds_http_redirect_cache::ds_http_redirect_cache()
: expiration_set(false)
{
  memset(&expiration, 0, sizeof(time_julian_type));
}



ds_http_redirect_cache::ds_http_redirect_cache(
                                               const ds_http_request_uri& origin,
                                               const ds_http_request_uri& redirect,
                                               const time_julian_type&    expire_date,
                                               bool                       expire_set
                                               )
: origin_uri(origin), redirect_uri(redirect), expiration_set(expire_set)
{
  memscpy(&expiration, sizeof(time_julian_type), &expire_date, sizeof(time_julian_type));
}



ds_http_redirect_cache::ds_http_redirect_cache(ASIStream& istream)
: expiration_set(false)
{
  if( !istream.good() ||
      !parse_uri_cache(istream, origin_uri) ||
      !parse_uri_cache(istream, redirect_uri) )
  {
    return;
  }

  uint32 secs = 0;
  memset(&expiration, 0, sizeof(time_julian_type));

  istream >> expiration_set >> secs;

  if( !istream.good() )
    return;

  if(expiration_set)
  {
    time_jul_from_secs(secs, &expiration);
    expiration_set = true;
  }
}



bool ds_http_redirect_cache::parse_uri_cache(ASIStream& istream, ds_http_request_uri& request_uri)
{
  ASString hostname;
  ASString path;
  uint16   protocol   = 0;
  uint32   port       = 0;

  istream >> hostname >> path >> protocol >> port;

  if( !istream.good() )
    return false;

  memscpy( request_uri.hostname, DS_HTTP_HOSTNAME_MAX_LEN, hostname.c_str(), hostname.size() );
  memscpy( request_uri.path, DS_HTTP_PATH_MAX_LEN, path.c_str(), path.size() );
  request_uri.protocol = uint16_to_proto(protocol);
  request_uri.port     = port;

  return true;
}


bool ds_http_redirect_cache::expired() const
{
  if(!expiration_set)
    return false;

  return time_get_secs() >= time_jul_to_secs(&expiration);
}

} /* namespace */





/*==============================================================================
                            ds_http_ip_addr_info
==============================================================================*/
ds_http_ip_addr_info::ds_http_ip_addr_info()
: ip_addr_type(DS_HTTP_IPV4), ip_addr(0)
{ }



ds_http_ip_addr_info::ds_http_ip_addr_info(const ds_http_ip_addr_info& other)
: ip_addr_type(other.ip_addr_type), ip_addr(other.ip_addr)
{
  memscpy(ip_addr6, DS_HTTP_IPV6_SIZE, other.ip_addr6, DS_HTTP_IPV6_SIZE);
}



ds_http_ip_addr_info& ds_http_ip_addr_info::operator=(const ds_http_ip_addr_info& other)
{
  ip_addr_type = other.ip_addr_type;
  ip_addr      = other.ip_addr;
  memscpy(ip_addr6, DS_HTTP_IPV6_SIZE, other.ip_addr6, DS_HTTP_IPV6_SIZE);

  return *this;
}



bool ds_http_ip_addr_info::operator==(const ds_http_ip_addr_info& rhs) const
{
  if(ip_addr_type != rhs.ip_addr_type)
    return false;

  switch(ip_addr_type)
  {
    case DS_HTTP_IPV4:
      return (ip_addr == rhs.ip_addr);

    case DS_HTTP_IPV6:
      return 0 == memcmp(ip_addr6, rhs.ip_addr6, DS_HTTP_IPV6_SIZE);

    default:
      return false;
  }
}



void ds_http_ip_addr_info::clear()
{
  ip_addr = 0;
  memset(ip_addr6, 0, DS_HTTP_IPV6_SIZE);
}




/*==============================================================================
                        Header fucntion definitions
==============================================================================*/
bool parse_uri(const char* uri_str, ds_http_request_uri& uri_info)
{
  const char* uri_ref = NULL;
  const char* tmp     = NULL;
  uint32 counter      = 0;
  uint32 path_len     = 0;

  if(NULL == uri_str || 0 == strlen(uri_str))
    return false;

  memset(&uri_info, 0, sizeof(ds_http_request_uri));

  if(strncmp(uri_str, "http://", 7) == 0)
  {
    uri_info.protocol = DS_HTTP_PROTOCOL_HTTP;
    uri_info.port     = DS_HTTP_DEFAULT_PORT;
    uri_str += 7;
  }
  else if(strncmp(uri_str, "https://", 8) == 0)
  {
    uri_info.protocol = DS_HTTP_PROTOCOL_HTTPS;
    uri_info.port     = DS_HTTPS_DEFAULT_PORT;
    uri_str += 8;
  }
  else
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_decode_uri - Invalid protocol type");
    return false;
  }

  uri_ref = uri_str;

  if('[' == *uri_ref)
  {
    do
    {
      if('\0' == *uri_ref)
      {
        DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_decode_uri - IPv6 address end delimiter not found");
        return false;
      }

      uri_ref++;
      counter++;
    } while(']' != *uri_ref);

    // Add ']' to hostname
    uri_ref++;
    counter++;
  }
  else
  {
    while('\0' != *uri_ref && URI_PATH_DELIM != *uri_ref && URI_PORT_DELIM != *uri_ref)
    {
      uri_ref++;
      counter++;
    }
  }


  // Get domain name (hostname)
  if(0 == counter || DS_HTTP_HOSTNAME_MAX_LEN < counter) // Empty hostname
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_decode_uri - Invalid hostname len %d", counter);
    return false;
  }
  memscpy(uri_info.hostname, DS_HTTP_HOSTNAME_MAX_LEN, uri_str, counter);


  // Get port if specified
  if(URI_PORT_DELIM == *uri_ref)
  {
    // get port
    uri_ref++;
    tmp           = uri_ref;
    uri_info.port = 0;

    while('\0' != *uri_ref)
    {
      if(URI_PATH_DELIM == *uri_ref)
        break;

      if(0 == isdigit(*uri_ref))
      {
        DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_decode_uri - Nondigit port number");
        return false;
      }

      uri_info.port = (uri_info.port*10) + (*uri_ref - '0');
      uri_ref++;
    }

    if(tmp == uri_ref)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_decode_uri - Port delimiter with no port number");
      return false;
    }
  }


  // remaining chars is the request path
  path_len = strlen(uri_ref);
  if(0 == path_len)
  {
    uri_info.path[0] = '/';
  }
  else
  {
    memscpy(uri_info.path, DS_HTTP_PATH_MAX_LEN, uri_ref, path_len);
  }

  return true;
}



void add_redirect_cache(
                        const ds_http_request_uri& origin_uri,
                        const ds_http_request_uri& redirect_uri,
                        const time_julian_type&    expiration_date,
                        bool                       expiration_set
                        )
{
  ASLockGuard lock(ds_http_redirect_uri_crit_sect);

  // Check if redirect is expired
  if(expiration_set && time_get_secs() >= time_jul_to_secs(&expiration_date) )
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "add_redirect_cache - redirect expired, not adding to cache");
    return;
  }

  // TODO: Check if origin uri is already in cache; if so, update redirect uri with new one and update efs

  ds_http_redirect_cache* uri_cache
    = new(ds_appsrv_alloc<ds_http_redirect_cache>(), APPSRV_MEM) ds_http_redirect_cache(
                                                                                        origin_uri,
                                                                                        redirect_uri,
                                                                                        expiration_date,
                                                                                        expiration_set
                                                                                        );

  if(NULL == uri_cache)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "add_redirect_cache - failed to allocate memory");
    return;
  }

  if(!redirect_cache_list.push_back(uri_cache))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "add_redirect_cache - failed to allocate memory to list");
    ds_appsrv_free(uri_cache);
    return;
  }

  append_permanent_uri_cache(uri_cache);
}



/* If uri is in redirect cache, it will overwrite the query object with the redirect uri */
void check_and_update_redirect_uri(ds_http_request_uri& query_uri)
{
  ASLockGuard lock(ds_http_redirect_uri_crit_sect);

  for(redirect_iterator redirect_it = redirect_cache_list.begin(); redirect_cache_list.end() != redirect_it; ++redirect_it)
  {
    if( (*redirect_it)->origin_uri == query_uri )
    {
      if( (*redirect_it)->expired() )
      {
        // Queried URI redirect cache is expired. Trigger cache cleanup.
        remove_expired_redirect();
      }
      else
      {
        query_uri = (*redirect_it)->redirect_uri;
      }

      return;
    }
  }
}




/*==============================================================================
                        Local fucntion definitions
==============================================================================*/
namespace
{

void remove_expired_redirect()
{
  bool removed = false;

  // Find and remove all expired permanent redirect
  redirect_iterator redirect_it = redirect_cache_list.begin();
  while(redirect_cache_list.end() != redirect_it)
  {
    if( (*redirect_it)->expired() )
    {
      removed = true;
      ds_appsrv_free(*redirect_it);
      redirect_it = redirect_cache_list.erase(redirect_it);
    }
    else
    {
      ++redirect_it;
    }
  }

  // If an expired cache was found, recreate efs cache file
  if(removed)
  {
    update_permanent_uri_cache();
  }
}



/* Save the current redirect cache to the efs */
void update_permanent_uri_cache()
{
  // Open a temporary file to write to
  ASOFStream ofstream(DS_HTTP_REDIRECT_CACHE_TEMP_PATH, O_CREAT | O_TRUNC | O_WRONLY, DEFFILEMODE);

  if(!ofstream.is_open())
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "update_permanent_uri_cache - Failed to open file");
    return;
  }

  for(redirect_iterator redirect_it = redirect_cache_list.begin(); redirect_cache_list.end() != redirect_it; ++redirect_it)
  {
    ofstream << (*redirect_it);
    if( ofstream.fail() )
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "update_permanent_uri_cache - Failed to overwrite original file with temp file");
      ofstream.close_file();
      efs_unlink(DS_HTTP_REDIRECT_CACHE_TEMP_PATH); // Erase temp file
      return;
    }
  }

  ofstream.close_file();

  // rename temp file to overwrite original
  if( 0 != efs_rename(DS_HTTP_REDIRECT_CACHE_TEMP_PATH, DS_HTTP_REDIRECT_CACHE_PATH) )
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "update_permanent_uri_cache - Failed to overwrite original file with temp file");
    efs_unlink(DS_HTTP_REDIRECT_CACHE_TEMP_PATH); // Erase the temp file if rename fails
    return;
  }
}



void append_permanent_uri_cache(const ds_http_redirect_cache* uri_cache)
{
  ASOFStream ofstream(DS_HTTP_REDIRECT_CACHE_PATH, O_CREAT | O_APPEND | O_WRONLY, DEFFILEMODE);

  if(!ofstream.is_open())
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "append_permanent_uri_cache - Failed to open file");
    return;
  }

  ofstream << uri_cache;

  // If appending fails, assume cache is corrupt and attempt to recreate cache
  if( ofstream.fail() )
  {
    ofstream.close_file();
    update_permanent_uri_cache();
  }
}



/*
  cache file format for permanent redirect:
    original URI
    redirect URI
    expiry in epoch secs
*/
ASOStream& operator<<(ASOStream& os, const ds_http_redirect_cache* redirect_cache)
{
  os << redirect_cache->origin_uri << redirect_cache->redirect_uri;

  uint32 secs = 0;

  if(redirect_cache->expiration_set)
  {
    secs = time_jul_to_secs( &(redirect_cache->expiration) );
  }

  os << redirect_cache->expiration_set << ' ' << secs << endl;

  return os;
}



/*
  cache file format for URI:
    hostname path protocol port
*/
ASOStream& operator<<(ASOStream& os, const ds_http_request_uri& uri_info)
{
  os << uri_info.hostname << ' '
    << uri_info.path << ' '
    << proto_to_uint16(uri_info.protocol) << ' '
    << uri_info.port << endl;

  return os;
}



uint16 proto_to_uint16(ds_http_protocol protocol)
{
  switch(protocol)
  {
    case DS_HTTP_PROTOCOL_HTTP:
      return 0;

    case DS_HTTP_PROTOCOL_HTTPS:
      return 1;

    default:
      return 0;
  }
}



ds_http_protocol uint16_to_proto(uint16 protocol)
{
  switch(protocol)
  {
    case 0:
      return DS_HTTP_PROTOCOL_HTTP;

    case 1:
      return DS_HTTP_PROTOCOL_HTTPS;

    default:
      return DS_HTTP_PROTOCOL_HTTP;
  }
}


} /* namespace */




/*==============================================================================
                            Initializer
==============================================================================*/
void ds_http_init_uri()
{
  ASLockGuard lock(ds_http_redirect_uri_crit_sect);

  ASIFStream ifstream(DS_HTTP_REDIRECT_CACHE_PATH, O_RDONLY, 0);
  bool changed = false;

  if( !ifstream.is_open() )
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_init_uri - Failed to open redirect cache file");
    return;
  }

  do
  {
    ds_http_redirect_cache* uri_cache
      = new(ds_appsrv_alloc<ds_http_redirect_cache>(), APPSRV_MEM) ds_http_redirect_cache(ifstream);

    if(NULL == uri_cache)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_init_uri - failed to allocate memory");
      return;
    }

    if( ifstream.eof() )
    {
      ds_appsrv_free(uri_cache);
      break;
    }
    else if( ifstream.fail() )
    {
      // assume file is corrupt
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_init_uri - Failed to parse redirect cache file");
      ds_appsrv_free(uri_cache);

      // TODO: Remove all loaded cache and efs file?
      // clear_redirect_cache_list();
      // ifstream.close();
      // efs_unlink(DS_HTTP_REDIRECT_CACHE_PATH); // Erase corrupt file
      return;
    }

    if( uri_cache->expired() || !redirect_cache_list.push_back(uri_cache) )
    {
      DATA_APPSRV_MSG1(
                       MSG_LEGACY_ERROR,
                       "ds_http_init_uri - expired:%d or alloc failed",
                       uri_cache->expired()
                       );
      ds_appsrv_free(uri_cache);
      changed = true;
    }
  } while( ifstream.good() );

  // Update efs if cache list is different (e.g. expired redirect)
  if(changed)
  {
    update_permanent_uri_cache();
  }
}

