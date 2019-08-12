/******************************************************************************

                        QMI_IP_XML.C

******************************************************************************/

/******************************************************************************

  @file    qmi_ip_xml.c
  @brief   QMI XML file processing.

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        -------------------------------------------------------
5/19/14   sb         Initial version

******************************************************************************/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/


#include "qmi_ip.h"

/*===========================================================================
                          DEFINITIONS
============================================================================*/

/*---------------------------------------------------------------------------
 Max allowed size of the XML file (2 MB)
---------------------------------------------------------------------------*/
#define QMI_IP_XML_MAX_FILESIZE       (2 << 20)

/*-------------------------------------------------------------------------
 Defines for clipping space or space & quotes (single, double)
--------------------------------------------------------------------------*/
#define QMI_IP_XML_CLIP_SPACE         " "
#define QMI_IP_XML_CLIP_SPACE_QUOTES  " '\""

#define MAX_XML_STR_LEN                 120

/*-------------------------------------------------------------------------
  All the XML TAG
-------------------------------------------------------------------------*/
#define config_TAG                         "config"
#define mode_TAG                           "mode"
#define inactive_TAG                       "inactive"
#define single_client_TAG                  "single_client"
#define multi_client_TAG                   "multi_client"
#define tcp_server_port_TAG                "tcp_server_port"
#define single_client_udp_port_TAG         "single_client_udp_port"
#define fpop_udp_port_TAG                  "fpop_udp_port"
#define fpop_timeout_TAG                   "fpop_timeout"
#define fpop_threshold_TAG                 "fpop_threshold"
#define lan_clients_allowed_TAG            "lan_clients_allowed"
#define tmgi_count_allowed_TAG             "tmgi_count_allowed"

static qmi_ip_conf_t * qmi_ip_conf;

/*-----------------------------------------------------------------------
 Structure for storing the xml data from file, current read position and
   size information
------------------------------------------------------------------------*/
struct xml_data
{
   uint32 size;  /* Actual size of the file data */
   uint32 read;  /* Current read position in buff */
   char *buff;   /* Buffer containing the xml file contents */
};


/*----------------------------------------------------------------------
 Private function declarations
-----------------------------------------------------------------------*/

static int32 qmi_ip_xml_peekbytes_cb(
   struct xmllib_metainfo_s_type *metainfo,
   int32                          offset,
   int32                          bytecount,
   uint8                         *buffer
);

static int32 qmi_ip_xml_getbytes_cb(
   struct xmllib_metainfo_s_type *metainfo,
   int32                          bytecount,
   char                          *buffer
);

static int qmi_ip_xml_read_xml_file(
   const char *xm_file,
   struct xml_data *xml
);

static int qmi_ip_xml_parse_tree(
   xmllib_parsetree_node_s_type *xml_root
);


/*===========================================================================
                         FUNCTION DEFINITION
============================================================================*/

/*===========================================================================

FUNCTION QMI_IP_UTIL_ICMP_STRING()

DESCRIPTION

  This function returns the result of case insensitive comparison of a
  xmllib's string (xml_str) and a regular string (str)

DEPENDENCIES
  None.

RETURN VALUE
  0 - if both strings are equal
  1 - First string has greater value
  -1 - Second string has greater value

SIDE EFFECTS
  None

/*=========================================================================*/
static int32 qmi_ip_util_icmp_string
(
   const xmllib_string_s_type *xml_str,
   const char *str
)
{
   int32 ret = -1;

   if (NULL != xml_str && NULL != str)
   {
      uint32 len = strlen(str);
/*---------------------------------------------------------------------------
  If the lengths match, do the string comparison
----------------------------------------------------------------------------*/
      if (xml_str->len == len)
      {
         ret = strncasecmp(xml_str->string, str, len);
      }
   }

   return ret;
}



/*===========================================================================

FUNCTION QMI_IP_READ_XML()

DESCRIPTION

  This function
  - reads the QMI IP config parameters

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

/*=========================================================================*/
int qmi_ip_read_xml
(
 char *xml_file
)
{
   xmllib_parsetree_node_s_type *xml_root = NULL;
   struct xml_data xml_data = {0, 0, NULL};
/*---------------------------------------------------------------------------
  Update the xml_data structure with data from the xml file
----------------------------------------------------------------------------*/

   int32 ret_val = qmi_ip_xml_read_xml_file(xml_file, &xml_data);

   if (SUCCESS == ret_val)
   {
      xmllib_error_e_type error = 0;
      int32 xmllib_ret;
      struct xmllib_metainfo_s_type metainfo = {
                                                  &xml_data,
                                                  (xmllib_memalloc_fptr_type)malloc,
                                                  free,
                                                  qmi_ip_xml_peekbytes_cb,
                                                  qmi_ip_xml_getbytes_cb
                                               };
/*---------------------------------------------------------------------------
  Invoke the XML parser and obtain the parse tree
----------------------------------------------------------------------------*/
      xmllib_ret = xmllib_parser_parse(XMLLIB_ENCODING_UTF8,
                                       &metainfo,
                                       &xml_root,
                                       &error);

      if (XMLLIB_SUCCESS == xmllib_ret)
      {
         ret_val = SUCCESS;
      }
      else
      {
         LOG_MSG_ERROR("qmi_ip_xml_parse: xmllib returned parse error", 0, 0, 0);
         ret_val = FAILURE;
      }
   }

   if (ret_val != SUCCESS)
   {
     if (xml_data.buff)
       free(xml_data.buff);

     return ret_val;
   }

   if (SUCCESS == ret_val)
   {
/*---------------------------------------------------------------------------
  parse the xml tree returned by the xmllib
----------------------------------------------------------------------------*/
      ret_val = qmi_ip_xml_parse_tree(xml_root);

      if (ret_val != SUCCESS)
         LOG_MSG_ERROR("qmi_ip_xml_parse: qmi_ip_xml_parse_tree returned parse error", 0, 0, 0);
/*---------------------------------------------------------------------------
  Free up the xmllib's parse tree
----------------------------------------------------------------------------*/
      xmllib_parser_free(free, xml_root);
   }
/*---------------------------------------------------------------------------
  Free the buffer allocated by the xml file reading utility function
----------------------------------------------------------------------------*/
   if (xml_data.buff)
     free(xml_data.buff);

   return ret_val;

}

/*===========================================================================

FUNCTION QMI_IP_XML_READ_XML_FILE()

DESCRIPTION

  This function reads the given XML file and stores the contents in
  the given xml_data structure

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

/*=========================================================================*/
static int qmi_ip_xml_read_xml_file
(
   const char *file_name,
   struct xml_data *xml_data
)
{
   FILE *fp = NULL;
   int32 ret_val = FAILURE;
   int32 cur_pos;

   fp = fopen(file_name, "rb");
/*---------------------------------------------------------------------------
  File not found at the given path
----------------------------------------------------------------------------*/
   if (NULL == fp)
   {
      LOG_MSG_ERROR("qmi_ip_xml_read_xml_file: unable to open file ",
                    0, 0, 0);
      ret_val = FAILURE;
   }
/*---------------------------------------------------------------------------
  If seek to the end failed or file size is greater than what we support
----------------------------------------------------------------------------*/
   else if (fseek(fp, 0, SEEK_END) ||
            ((cur_pos = ftell(fp)) < 0 || cur_pos > QMI_IP_XML_MAX_FILESIZE) )
   {
      fclose(fp);
   }
   else
   {
      xml_data->size = cur_pos;
/*---------------------------------------------------------------------------
  Allocate storage for reading the xml file into memory
----------------------------------------------------------------------------*/
      if (NULL == (xml_data->buff = malloc(xml_data->size)))
      {
         LOG_MSG_ERROR("qmi_ip_xml_read_xml_file: failed to allocate "
                       "memory for read buffer", 0, 0, 0);
         ret_val = FAILURE;
      }
/*---------------------------------------------------------------------------
  Reset to the beginning of the file
----------------------------------------------------------------------------*/
      else if (!fseek(fp, 0, SEEK_SET))
      {
         size_t read_size;
/*---------------------------------------------------------------------------
  Read the data from file to buffer
----------------------------------------------------------------------------*/
         read_size = fread(xml_data->buff, 1, xml_data->size, fp);

         if (!ferror(fp) && (read_size == xml_data->size))
         {
            xml_data->read = 0;
            ret_val = SUCCESS;
         }
      }

      fclose(fp);
   }
   return ret_val;
}

/*===========================================================================

FUNCTION QMI_IP_XML_PEEKBYTES_CB()

DESCRIPTION

  This function
  - facilitates xml read

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

static int32 qmi_ip_xml_peekbytes_cb
(
  struct xmllib_metainfo_s_type *metainfo,
  int32                          offset,
  int32                          bytecount,
  uint8                         *buffer
)
{
   struct xml_data *xml_data = NULL;
/*---------------------------------------------------------------------------
  Validate arguments
----------------------------------------------------------------------------*/
   if ((NULL == metainfo)                       ||
       (NULL == (xml_data = metainfo->xmltext)) ||
       (offset < 0)                             ||
       (bytecount < 0)                          ||
       (NULL == buffer)                         ||
       (xml_data->read+offset+bytecount > xml_data->size))
   {
      return XMLLIB_ERROR;
   }

   memcpy(buffer, xml_data->buff+xml_data->read+offset, bytecount);
   return XMLLIB_SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_XML_GETBYTES_CB()

DESCRIPTION

  This function
  - facilitates xml read

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

static int32 qmi_ip_xml_getbytes_cb
(
  struct xmllib_metainfo_s_type *metainfo,
  int32                          bytecount,
  char                          *buffer
)
{
   struct xml_data *xml_data = NULL;
/*---------------------------------------------------------------------------
  If requesting to read more than what we have return error
----------------------------------------------------------------------------*/
   if ((NULL == metainfo)                       ||
       (NULL == (xml_data = metainfo->xmltext)) ||
       (bytecount < 0)                          ||
       (xml_data->read+bytecount > xml_data->size))
   {
      return XMLLIB_ERROR;
   }
/*---------------------------------------------------------------------------
  If a valid buffer is given, copy the data
----------------------------------------------------------------------------*/
   if (NULL != buffer)
   {
      memcpy(buffer, xml_data->buff+xml_data->read, bytecount);
   }
/*---------------------------------------------------------------------------
  Increment to the next unread data block
----------------------------------------------------------------------------*/
   xml_data->read += bytecount;

   return XMLLIB_SUCCESS;
}

/*===========================================================================

FUNCTION QMI_IP_READ_CONTENT_ELEMENT()

DESCRIPTION

  This function
  - facilitates xml read

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/
static xmllib_string_s_type* qmi_ip_read_content_element
(
 xmllib_parsetree_node_s_type* element
)
{
  xmllib_parsetree_node_s_type* child_ptr;

  for ( child_ptr  = element->payload.element.child;
        child_ptr != 0;
        child_ptr  = child_ptr->sibling )
  {
    if ( child_ptr->nodetype == XMLLIB_PARSETREE_NODE_CONTENT )
    {
      return &(child_ptr->payload.content);
    }
  }

  return ((xmllib_string_s_type*)NULL);
}

/*===========================================================================

FUNCTION QMI_IP_XML_PARSE_TREE()

DESCRIPTION

  This function
  - parses through the xml file

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

/*=========================================================================*/

static int qmi_ip_xml_parse_tree
(
   xmllib_parsetree_node_s_type *xml_node
)
{
   int32 ret_val = SUCCESS;
   xmllib_string_s_type* content;
   char content_buf[MAX_XML_STR_LEN];

   if (NULL == xml_node)
      return ret_val;

   while ( (xml_node != (xmllib_parsetree_node_s_type *)NULL) &&
          (ret_val == SUCCESS) )
   {
     switch (xml_node->nodetype)
     {
       case XMLLIB_PARSETREE_NODE_ELEMENT:
       {
         if (0 == qmi_ip_util_icmp_string(&xml_node->payload.element.name,
                                         config_TAG)
             )
         {
           // go to child
           ret_val = qmi_ip_xml_parse_tree(xml_node->payload.element.child);
         }
         else if (0 == qmi_ip_util_icmp_string(&xml_node->payload.element.name,
                  mode_TAG))
         {
           content = qmi_ip_read_content_element(xml_node);
           if (content)
           {
             memset(content_buf, 0, sizeof(content_buf));
             memcpy(content_buf, (void *)content->string,
                    ((content->len < sizeof(content_buf))? content->len : (sizeof(content_buf) - 1)));
             if (strncasecmp(single_client_TAG, content_buf, strlen(single_client_TAG)) == 0)
             {
               qmi_ip_conf->mode = SINGLE_CLIENT_MODE;
             }
             else if (strncasecmp(multi_client_TAG, content_buf, strlen(multi_client_TAG)) == 0)
             {
               qmi_ip_conf->mode = MULTI_CLIENT_MODE;
             }
             else
             {
               qmi_ip_conf->mode = INACTIVE_MODE;
             }

             LOG_MSG_INFO1("QMI IP mode is %d", qmi_ip_conf->mode, 0, 0);
           }
         }
         else if (0 == qmi_ip_util_icmp_string(&xml_node->payload.element.name,
                                          tcp_server_port_TAG))
         {
           content = qmi_ip_read_content_element(xml_node);
           if (content)
           {
             memset(content_buf, 0, sizeof(content_buf));
             memcpy(content_buf, (void *)content->string, ((content->len < sizeof(content_buf))? content->len : (sizeof(content_buf) - 1)));
             qmi_ip_conf->tcp_server_ctrl_port = htons(atoi(content_buf));
             LOG_MSG_INFO1("TCP server port %d", ntohs(qmi_ip_conf->tcp_server_ctrl_port), 0, 0);
           }
         }
         else if (0 == qmi_ip_util_icmp_string(&xml_node->payload.element.name,
                                               single_client_udp_port_TAG))
         {
           content = qmi_ip_read_content_element(xml_node);
           if (content)
           {
             memset(content_buf, 0, sizeof(content_buf));
             memcpy(content_buf, (void *)content->string, ((content->len < sizeof(content_buf))? content->len : (sizeof(content_buf) - 1)));
             qmi_ip_conf->single_client_udp_port = htons(atoi(content_buf));
             LOG_MSG_INFO1("Single client UDP port %d", ntohs(qmi_ip_conf->single_client_udp_port), 0, 0);
           }
         }
         else if (0 == qmi_ip_util_icmp_string(&xml_node->payload.element.name,
                                          fpop_udp_port_TAG))
         {
           content = qmi_ip_read_content_element(xml_node);
           if (content)
           {
             memset(content_buf, 0, sizeof(content_buf));
             memcpy(content_buf, (void *)content->string, ((content->len < sizeof(content_buf))? content->len : (sizeof(content_buf) - 1)));
             qmi_ip_conf->fpop_udp_port = htons(atoi(content_buf));
             LOG_MSG_INFO1("FPOP udp port %d", ntohs(qmi_ip_conf->fpop_udp_port), 0, 0);
           }
         }
         else if (0 == qmi_ip_util_icmp_string(&xml_node->payload.element.name,
                                               fpop_timeout_TAG))
         {
           content = qmi_ip_read_content_element(xml_node);
           if (content)
           {
             memset(content_buf, 0, sizeof(content_buf));
             memcpy(content_buf, (void *)content->string, ((content->len < sizeof(content_buf))? content->len : (sizeof(content_buf) - 1)));
             qmi_ip_conf->fpop_timeout = atoi(content_buf);
             LOG_MSG_INFO1("FPOP timeout value in seconds %d", qmi_ip_conf->fpop_timeout, 0, 0);
           }
         }
         else if (0 == qmi_ip_util_icmp_string(&xml_node->payload.element.name,
                                               fpop_threshold_TAG))
         {
           content = qmi_ip_read_content_element(xml_node);
           if (content)
           {
             memset(content_buf, 0, sizeof(content_buf));
             memcpy(content_buf, (void *)content->string, ((content->len < sizeof(content_buf))? content->len : (sizeof(content_buf) - 1)));
             qmi_ip_conf->fpop_threshold = atoi(content_buf);
             LOG_MSG_INFO1("FPOP threshold value %d", qmi_ip_conf->fpop_threshold, 0, 0);
           }
         }
         else if (0 == qmi_ip_util_icmp_string(&xml_node->payload.element.name,
                                               lan_clients_allowed_TAG))
         {
           content = qmi_ip_read_content_element(xml_node);
           if (content)
           {
             memset(content_buf, 0, sizeof(content_buf));
             memcpy(content_buf, (void *)content->string, ((content->len < sizeof(content_buf))? content->len : (sizeof(content_buf) - 1)));
             qmi_ip_conf->lan_clients_allowed = atoi(content_buf);
             LOG_MSG_INFO1("LAN clients allowed %d", qmi_ip_conf->lan_clients_allowed, 0, 0);
           }
         }
         else if (0 == qmi_ip_util_icmp_string(&xml_node->payload.element.name,
                                               tmgi_count_allowed_TAG))
         {
           content = qmi_ip_read_content_element(xml_node);
           if (content)
           {
             memset(content_buf, 0, sizeof(content_buf));
             memcpy(content_buf, (void *)content->string, ((content->len < sizeof(content_buf))? content->len : (sizeof(content_buf) - 1)));
             qmi_ip_conf->tmgi_count_allowed = atoi(content_buf);
             LOG_MSG_INFO1("TMGI count allowed %d", qmi_ip_conf->tmgi_count_allowed, 0, 0);
           }
         }
      }
      break;
      default:
      break;
    }
    // go to sibling
    xml_node = xml_node->sibling;
  } // while

  return ret_val;
}

/*===========================================================================

FUNCTION QMI_IP_XML_INIT()

DESCRIPTION

  This function
  - initializes config for XML

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

/*=========================================================================*/

int qmi_ip_xml_init
(
  qmi_ip_conf_t   * qmi_ip_cfg,
  char            * xml_file
)
{
  int ret_val;

  qmi_ip_conf = qmi_ip_cfg;

  ret_val = qmi_ip_read_xml(xml_file);
  if (ret_val != SUCCESS)
  {
    LOG_MSG_ERROR("Error in reading QMI IP XML file", 0, 0, 0);
    return FAILURE;
  }
  return SUCCESS;
}
