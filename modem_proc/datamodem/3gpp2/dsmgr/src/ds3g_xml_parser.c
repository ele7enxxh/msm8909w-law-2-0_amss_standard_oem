/*!
  @file
  ds3g_xml_parser.c

  @brief
  Parse DS3G XML configuration file

  @detail
  Simple version for XML parsing.

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3g_xml_parser.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/07/15   js      Move to DS3G folder and rename. 
08/13/15   fj      Initial version.
===========================================================================*/
/*============================================================================= 
 
===============================================================================

  ABNF for XML that is parsed:
 
  element     :=  empty_elem | content_elem
  empty_elem  :=  '<' name (WS+ attribute)* WS* '/>'
  content_elem:=  '<' name (WS+ attribute)* WS* '>'
                  (element | comment)* | text
                  '</' name WS* '>'           ;same name as start tag
  name        :=  ALPHA ALPHANUM*
  text        :=  TEXT_CHAR+
  WS          :=  ' ' | '\t' | '\n'

===============================================================================
=============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h" 
#include "comdef.h"

#include "sys.h"
#include "mcfg_fs.h"
#include "ds3g_xml_parser.h"
#include "ds_3gppi_utils.h"
#include "modem_mem.h"
#include "dsutil.h"

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS3G_XML_PROC_ELEMENT

DESCRIPTION
  For a given text document, parse into XML element format.

PARAMETERS doc: file pointer for a text document in XML format

DEPENDENCIES
  None.

RETURN VALUE   ds3g_xml_element: XML element ptr
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ds3g_xml_element* ds3g_xml_proc_element
(
  char                   **doc 
);

/*===========================================================================

                    INTERNAL FUNCTION DEFINITION

===========================================================================*/
/*===========================================================================
FUNCTION DS3G_XML_SKIP_WHITE_SPACE

DESCRIPTION
  Skip the white space, and move the pointer to the next non-white space
  character.

PARAMETERS  curr_pos: current character position of a file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_xml_skip_white_space
(
  char                  **curr_pos
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (curr_pos == NULL) || (*curr_pos == NULL) )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_skip_white_space(): "
                       "invalid curr_pos passed");
    return;
  }

  while( (**curr_pos == ' ') || (**curr_pos == '\t') || (**curr_pos == '\n') ||
      (**curr_pos == '\r') )
  {
    ++*curr_pos;
  }

  return;
}/*ds3g_xml_skip_white_space*/

/*===========================================================================
FUNCTION DS3G_XML_IS_TAG_CHAR

DESCRIPTION
  Check if a char is allowed in XML tag or not. If allowed, return TRUE;
  else, return FALSE.
  Currently, only allow 'A'-'Z', 'a'-'z', '0'-'9', '_', '-', ':', '.'
  in tag.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3g_xml_is_tag_char
(
  char      c
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ('A' <= c && c <= 'Z') ||
      ('a' <= c && c <= 'z') ||
      ('0' <= c && c <= '9') ||
      ( c == '_') || ( c== '.') || ( c == '-') || (c == ':'))
  {
    return TRUE;
  }
  else
    return FALSE;
}/*ds3g_xml_is_tag_char*/

/*===========================================================================
FUNCTION DS3G_XML_PROC_TAG

DESCRIPTION
  Process XML tag and save the tag name in the element buff.
  If current char is not a tag, return error.

PARAMETERS   curr_pos: current position of the file 
             element_ptr: element buffer ptr passed. 

DEPENDENCIES
  None.

RETURN VALUE  TRUE: Processing the tag succeeds.
              FALSE: Processing the tag fails.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3g_xml_proc_tag
(
  char                      **curr_pos,
  ds3g_xml_element          *element_ptr
)
{
  char                      *start_ptr = NULL;
  uint8                      tag_size = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (curr_pos == NULL) || (*curr_pos == NULL) || (element_ptr == NULL) )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_proc_tag(): invalid curr_pos passed or"
                       "element_ptr is NULL");
    return FALSE;
  }  

  /*-----------------------------------------------------------------------
   XML tag starts with '<', if current char is not XML tag, return FALSE;
  ------------------------------------------------------------------------*/
  if( **curr_pos != '<' )
    return FALSE;
  else
    ++*curr_pos;

  /*-----------------------------------------------------------------------
   Get the tag name and save it to element_ptr. XML does not allow white
   space inside a tag.
  ------------------------------------------------------------------------*/
  start_ptr = *curr_pos;
  while( ds3g_xml_is_tag_char(**curr_pos))
  {
    ++*curr_pos;
  }

  /*-----------------------------------------------------------------------
  Allocated the memory to save the element tag.
  ------------------------------------------------------------------------*/
  tag_size = *curr_pos-start_ptr;
  element_ptr->Tag = (char*)modem_mem_alloc(tag_size+1, 
                                            MODEM_MEM_CLIENT_DATA);
  if( element_ptr->Tag == NULL )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_proc_tag(): memory allocation failed");
    return FALSE;
  }

  memset(element_ptr->Tag, 0, tag_size+1);
  memscpy(element_ptr->Tag, tag_size, 
          start_ptr, tag_size);

  /*-----------------------------------------------------------------------
  At the end of the tag, skip the white space if there is any.
  ------------------------------------------------------------------------*/
  ds3g_xml_skip_white_space(curr_pos);

  return TRUE;
}/*ds3g_xml_proc_tag*/

/*===========================================================================
FUNCTION DS3G_XML_PROC_END_TAG

DESCRIPTION
  Process XML end tag and verify that the end tag is the same as tag.

PARAMETERS  curr_pos: current position of the file
            element_ptr: element buff ptr passed.

DEPENDENCIES
  None.

RETURN VALUE  TRUE: Processing the end tag succeeds. 
              FALSE: Processing the end tag fails.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3g_xml_proc_end_tag
(
  char                    **curr_pos,
  ds3g_xml_element        *element_ptr
)
{
  boolean                               ret_val = FALSE;
  char                                  end_tag[2]="</";
  char                                 *start_pos = NULL;
  uint8                                 end_tag_size = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (curr_pos == NULL) || (*curr_pos == NULL) || (element_ptr == NULL) )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_proc_tag(): invalid curr_pos passed or"
                     "element_ptr is NULL");
    return FALSE;
  }  

  if ( strncmp(*curr_pos, end_tag, 2) == 0 )
  {
    *curr_pos += 2;
    start_pos = *curr_pos;
    /*-----------------------------------------------------------------------
    Get the end tag.
    ------------------------------------------------------------------------*/
    while( ds3g_xml_is_tag_char(**curr_pos))
    {
      ++*curr_pos;
    }

    end_tag_size = *curr_pos-start_pos;

    /*-----------------------------------------------------------------------
    If the end tag is the same as current tag in the element. Processing 
    this element successfully, other wise, return failure. 
    ------------------------------------------------------------------------*/
    if ( end_tag_size == strlen(element_ptr->Tag) )
    {
      if( strncmp(start_pos, element_ptr->Tag, end_tag_size ) == 0 )
      {
        if( **curr_pos == '>')
        {
          ++*curr_pos;
          ret_val = TRUE;
        }
      }
    }
  }

  return ret_val;
}/*ds3g_xml_proc_end_tag*/

/*===========================================================================
FUNCTION DS3G_XML_IF_NEXT_END_TAG

DESCRIPTION
  Check if there is end tag.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3g_xml_if_next_end_tag
(
  char                  *doc
)
{
   char                  end_tag[2]="</";
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 if ( strncmp(doc, end_tag, 2) == 0 )
   return TRUE;
 else
   return FALSE;
}/*ds3g_xml_if_next_end_tag*/

/*===========================================================================
FUNCTION DS3G_XML_PROC_CONTENT_TEXT

DESCRIPTION
  Process XML content text.

PARAMETERS  curr_pos: current position of a file. 
            element_ptr: element buffer pointer paassed. 

DEPENDENCIES None

RETURN VALUE TRUE: processing text content succeeds. 
             FALSE: processing text content fails. 

SIDE EFFECTS None
===========================================================================*/
boolean ds3g_xml_proc_content_text
(
  char                      **curr_pos,
  ds3g_xml_element          *element_ptr
)
{
  char                      *start_pos = NULL;
  uint8                      text_len = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (curr_pos == NULL) || (*curr_pos == NULL) || (element_ptr == NULL) )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_proc_content_text(): invalid curr_pos passed "
                       "or element_ptr is NULL");
    return FALSE;
  }  

  start_pos = *curr_pos;
  while ( (**curr_pos != '\0') && (**curr_pos != '<') )
  {
    ++*curr_pos;
  }

  text_len = *curr_pos - start_pos;

  /*-----------------------------------------------------------------------
   Allocate the memory to save the text.
  ------------------------------------------------------------------------*/
  element_ptr->Text = modem_mem_alloc(text_len+1,
                                      MODEM_MEM_CLIENT_DATA);
  if( element_ptr->Text == NULL )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_proc_tag(): memory allocation failed");
    return FALSE;
  }

  memset(element_ptr->Text, 0, text_len+1);
  memscpy(element_ptr->Text, text_len,
          start_pos, text_len);

  return TRUE;
}/*ds3g_xml_proc_content_text*/

/*===========================================================================
FUNCTION DS3G_XML_PROC_ELEMENT_CONTENT

DESCRIPTION
  Process XML element contents.

PARAMETERS curr_pos: current position of the file.
           element_ptr: element buffer ptr passed.

DEPENDENCIES
  None.

RETURN VALUE TRUE: Processing XML content succeeds. 
             FALSE: Processing XML content fails.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3g_xml_proc_element_content
(
  char                      **curr_pos,
  ds3g_xml_element          *element_ptr
)
{
  ds3g_xml_element            *child_element_ptr = NULL;
  ds3g_xml_list_info_type     *temp_item_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (curr_pos == NULL) || (*curr_pos == NULL) || (element_ptr == NULL) )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_proc_element_content(): invalid curr_pos "
                       "passed or element_ptr is NULL");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
  If element content begins with '<', it contains children elements; 
  if not, it contains text content.
  ------------------------------------------------------------------------*/
  if( **curr_pos != '<' )
  {
    if( ds3g_xml_proc_content_text(curr_pos, element_ptr) == FALSE )
      return FALSE;
  }
  else
  {
    /*-----------------------------------------------------------------------
     If next char is end tag or file end char, current element ends. Else,
     process and fill its children elements one by one.
    ------------------------------------------------------------------------*/
    while( (**curr_pos != '\0') && !ds3g_xml_if_next_end_tag(*curr_pos) )
    {
      /*-----------------------------------------------------------------------
      Get the child element.
      ------------------------------------------------------------------------*/
      child_element_ptr = ds3g_xml_proc_element(curr_pos);

      /*----------------------------------------------------------------------- 
       If the children list is NULL, allocate the list head and initialize the
       children list.
      ------------------------------------------------------------------------*/
      if( element_ptr->Children == NULL )
      {
        element_ptr->Children = modem_mem_alloc
                                       (sizeof(list_type),MODEM_MEM_CLIENT_DATA);
        if( element_ptr->Children == NULL )
        {
          DS_3GPP_MSG0_LOW("ds3g_xml_proc_element_content(): memory "
                             "allocation failure");
          ASSERT(0);
          return FALSE;
        }
        else
          list_init( element_ptr->Children );
      }

      /*-----------------------------------------------------------------------
       Allocate memory for the new list item.
      ------------------------------------------------------------------------*/
      temp_item_ptr = modem_mem_alloc(sizeof(ds3g_xml_list_info_type),
                                      MODEM_MEM_CLIENT_DATA);
      if( temp_item_ptr == NULL )
      {
        DS_3GPP_MSG0_LOW("ds3g_xml_proc_element_content(): memory "
                           "allocation failure");
        ASSERT(0);
        return FALSE;
      }
      temp_item_ptr->element = child_element_ptr;
      list_push_back( element_ptr->Children, &(temp_item_ptr->link) );
    }

    /*-----------------------------------------------------------------------
    Skip the white space in case there is white space at the end of element.
    ------------------------------------------------------------------------*/
    //ds3g_xml_skip_white_space(curr_pos);
  }

  /*-----------------------------------------------------------------------
    Process the end tag
  ------------------------------------------------------------------------*/
  if( ds3g_xml_proc_end_tag(curr_pos,element_ptr) == FALSE )
      return FALSE;

  /*-----------------------------------------------------------------------
  Skip the white space in case this is white space at the end of the element. 
  ------------------------------------------------------------------------*/
  ds3g_xml_skip_white_space(curr_pos);
  return TRUE;
}/*ds3g_xml_proc_element_content*/

/*=========================================================================== 
FUNCTION DS3G_XML_PROC_ELEMENT

DESCRIPTION
  For a given text document, parse into XML element format.

PARAMETERS doc: file pointer for a text document in XML format

DEPENDENCIES
  None.

RETURN VALUE   ds3g_xml_element: XML element ptr
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ds3g_xml_element*       ds3g_xml_proc_element
(
  char                     **doc 
)
{
  ds3g_xml_element         *new_element_ptr = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
  Allocate memory for XML element
  ------------------------------------------------------------------------*/
  new_element_ptr = (ds3g_xml_element*)modem_mem_alloc
                                             (sizeof(ds3g_xml_element),
                                              MODEM_MEM_CLIENT_DATA);
  if(new_element_ptr == NULL)
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_proc_element(): memory allocate fail");
    ASSERT(0);
    return NULL;
  }  
  memset(new_element_ptr, 0, sizeof(ds3g_xml_element));

  /*-----------------------------------------------------------------------
  Skip the white space.
  ------------------------------------------------------------------------*/
  ds3g_xml_skip_white_space(doc);  

  /*-----------------------------------------------------------------------
  Process XML tag
  ------------------------------------------------------------------------*/
  if( ds3g_xml_proc_tag(doc, new_element_ptr) == FALSE )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_proc_element(): process tag failed");
    ds3g_xml_free_element(new_element_ptr);      
    return NULL;
  }

  /*-----------------------------------------------------------------------
  Process element content. Currently, XML parser does not support attributes.
  ------------------------------------------------------------------------*/
  if( **doc != '>' ) 
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_proc_element(): XML format does not support");
    //Free element memory
    ds3g_xml_free_element(new_element_ptr);
    return NULL;
  }
  else
  {
    ++*doc;
    ds3g_xml_skip_white_space(doc);
    /*-----------------------------------------------------------------------
    Get the content of the element.
    ------------------------------------------------------------------------*/
    if( ds3g_xml_proc_element_content(doc,new_element_ptr) == FALSE )
    {
      ds3g_xml_free_element(new_element_ptr);
      return NULL;
    }     
  }

  ds3g_xml_skip_white_space(doc);

  return new_element_ptr;
}/*ds3g_xml_proc_element*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS3G_XML_PARSE

DESCRIPTION Parse a file into XML document tree

PARAMETERS  subs_id: subscription id 
            file_path: the file location. 

DEPENDENCIES
  None.

RETURN VALUE 
  ds3g_xml_element *: XML root element tree pointer

SIDE EFFECTS
  None.

===========================================================================*/
ds3g_xml_element* ds3g_xml_parse
(
  sys_modem_as_id_e_type           subs_id,
  char                            *file_path
)
{  
  struct fs_stat             st;
  byte                      *pData = NULL;
  char                      *doc = NULL;
  mcfg_fs_status_e_type      mcfg_status;
  ds3g_xml_element          *ret_ptr = NULL;           
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( file_path == NULL )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_parse(): file path passed is NULL");
    return NULL; 
  }

  /*-----------------------------------------------------------------------
  Allocate a temp buff and read the file contents into buff.
  ------------------------------------------------------------------------*/
  memset(&st, 0, sizeof(st));

  //Get the size of the file
  mcfg_status = mcfg_fs_stat(file_path,
                             &st,
                             MCFG_FS_TYPE_EFS,
                             (mcfg_fs_sub_id_e_type)subs_id);

  if ( mcfg_status != MCFG_FS_STATUS_OK )
  {
    DS_3GPP_MSG0_ERROR("File is not configured");
    return NULL;
  }

  // Allocate the buff
  pData = modem_mem_alloc( st.st_size+1, MODEM_MEM_CLIENT_DATA);
  if( pData == NULL )
  {
    DS_3GPP_MSG1_LOW("allocate memory size %d failed",
                       st.st_size+1);
    ASSERT(0);
    return NULL;
  }

  //Read file data into buff
  mcfg_status = mcfg_fs_read(file_path,
                             pData,
                             st.st_size,
                             MCFG_FS_TYPE_EFS,
                             (mcfg_fs_sub_id_e_type)subs_id);
  if( mcfg_status != MCFG_FS_STATUS_OK ) 
  {
    modem_mem_free(pData, MODEM_MEM_CLIENT_DATA);
    return NULL;
  }

  /*-----------------------------------------------------------------------
  Process the file into XML element tree
  ------------------------------------------------------------------------*/
  doc = (char*)pData;
  ret_ptr = ds3g_xml_proc_element(&doc);

  /*-----------------------------------------------------------------------
  Free the data buffer
  ------------------------------------------------------------------------*/
  modem_mem_free(pData, MODEM_MEM_CLIENT_DATA);

  return ret_ptr;
}/*ds3g_xml_parse*/

/*===========================================================================
FUNCTION DS3G_XML_FREE_ELEMENT

DESCRIPTION
  Free element tree memory. It does deep free.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_xml_free_element
(
  ds3g_xml_element              *element_ptr  
)
{
  ds3g_xml_list_info_type       *list_entry_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( element_ptr == NULL )
  {
    return;
  }

  /*-----------------------------------------------------------------------
  Free the Tag memory
  ------------------------------------------------------------------------*/
  if( element_ptr->Tag != NULL )
  {
    modem_mem_free( element_ptr->Tag, MODEM_MEM_CLIENT_DATA);
    element_ptr->Tag = NULL;
  }

  /*-----------------------------------------------------------------------
  Free the Text memory
  ------------------------------------------------------------------------*/
  if( element_ptr->Text != NULL )
  {
    modem_mem_free( element_ptr->Text, MODEM_MEM_CLIENT_DATA);
    element_ptr->Text = NULL;
  }

  /*-----------------------------------------------------------------------
  Free the Children list memory
  ------------------------------------------------------------------------*/
  if( element_ptr-> Children != NULL )
  {
    list_entry_ptr = (ds3g_xml_list_info_type*)
                            list_pop_front(element_ptr-> Children);
    while( list_entry_ptr != NULL )
    {
      /*-------------------------------------------------------------------
      Deep free element memory of each child.
      -------------------------------------------------------------------*/
      ds3g_xml_free_element(list_entry_ptr->element);
      list_entry_ptr->element = NULL;
      modem_mem_free(list_entry_ptr, MODEM_MEM_CLIENT_DATA);
      list_entry_ptr = list_pop_front(element_ptr-> Children);
    }
    list_destroy(element_ptr->Children);
    element_ptr->Children = NULL;
  }

  modem_mem_free(element_ptr, MODEM_MEM_CLIENT_DATA);

}/*ds3g_xml_free_element*/

/*===========================================================================
FUNCTION DS3G_XML_FREE_ELEMENTS_LIST

DESCRIPTION
  Free element list.
  Please note: this function does not free the memory allocated for element.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_xml_free_elements_list
(
  list_type                    *list_head
)
{
  ds3g_xml_list_info_type      *list_entry_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( list_head == NULL )
  {
    return;
  }

  list_entry_ptr = (ds3g_xml_list_info_type*)
                            list_pop_front(list_head);
  while( list_entry_ptr != NULL )
  {
    modem_mem_free(list_entry_ptr, MODEM_MEM_CLIENT_DATA);
    list_entry_ptr = (ds3g_xml_list_info_type*)list_pop_front(list_head);
  }
}/*ds3g_xml_free_elements_list*/

/*===========================================================================
FUNCTION DS3G_XML_GET_ELEMENT_BY_TAG

DESCRIPTION
  Get XML element by passing XML tag info.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3g_xml_get_element_by_tag
(
  ds3g_xml_element              *xml_root,
  char                          *tag_str,
  list_type                     *element_list
)
{
  ds3g_xml_list_info_type       *child_ptr = NULL;
  ds3g_xml_list_info_type       *temp_list_entry = NULL;
  boolean                        found_tag = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (xml_root == NULL) || (tag_str == NULL) || (element_list == NULL) )
  {
    DS_3GPP_MSG0_LOW("ds3g_xml_get_element_by_tag(): xml_root or tag_str"
                       " or element_list passed is NULL");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
  If XML root contains the same tag, return root element
  ------------------------------------------------------------------------*/
  if ( xml_root->Tag != NULL )
  {
    if( strcmp( xml_root->Tag, tag_str ) == 0 )
    {
      if( element_list->list_initialized == FALSE )
      {
        list_init(element_list);
      }
      temp_list_entry = modem_mem_alloc(sizeof(ds3g_xml_list_info_type),
                                        MODEM_MEM_CLIENT_DATA);
      if (temp_list_entry == NULL)
      {
        DS_3GPP_MSG0_LOW("ds3g_xml_get_element_by_tag(): memory "
                           "allocation failure");
        ASSERT(0);
        return FALSE;
      }
      temp_list_entry->element = xml_root;
      list_push_back( element_list, &(temp_list_entry->link) );
      found_tag = TRUE;    
      return found_tag;    
    }
  }

  /*-----------------------------------------------------------------------
  Search children element
  ------------------------------------------------------------------------*/
  if( xml_root->Children != NULL )
  {
    child_ptr = (ds3g_xml_list_info_type*)
                            list_peek_front(xml_root->Children);
    while( child_ptr != NULL )
    {
      if( child_ptr->element != NULL )
      {
        if ( (child_ptr->element->Tag != NULL) &&
             (strcmp( child_ptr->element->Tag, tag_str ) == 0) )
        {
          /*-------------------------------------------------------------------
          Found the matching tag from Children element. Continue searching other
          sibling element. Don't need to search its Children element.
          -------------------------------------------------------------------*/
          if( element_list->list_initialized == FALSE )
          {
            list_init(element_list);
          }
          temp_list_entry = modem_mem_alloc(sizeof(ds3g_xml_list_info_type),
                                            MODEM_MEM_CLIENT_DATA);
          if (temp_list_entry == NULL)
          {
            DS_3GPP_MSG0_LOW("ds3g_xml_get_element_by_tag(): memory "
                               "allocation failure");
            ASSERT(0);
            return FALSE;
          }
          temp_list_entry->element = child_ptr->element;
          list_push_back( element_list, &(temp_list_entry->link) );
          found_tag = TRUE;
        }
        else if( child_ptr->element->Children != NULL )
        {
          /*-------------------------------------------------------------------
          Found the matching tag from its Children element. If found matach, 
          don't need to search sibling elements. Break the loop.
          -------------------------------------------------------------------*/
          if( found_tag == FALSE )
          {
            if( ds3g_xml_get_element_by_tag(child_ptr->element,
                                               tag_str,
                                               element_list))
            {
              found_tag = TRUE;
              break;
            }
          }
        }
        
        child_ptr = (ds3g_xml_list_info_type*)
                            list_peek_next(xml_root->Children,
                                           &(child_ptr->link));          

      }
    }
  }
  return found_tag;
}/*ds3g_xml_get_element_by_tag*/

/*===========================================================================
FUNCTION DS3G_XML_GET_TEXT_CONTENT

DESCRIPTION
  Get XML test content from an element.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
char* ds3g_xml_get_text_content
(
  ds3g_xml_element         *element
)
{
  if( element == NULL )
  {
    DS_3GPP_MSG0_LOW("element ptr passed is NULL, return NULL");
    return NULL;
  }

  return element->Text;
}/*ds3g_xml_get_text_content*/






