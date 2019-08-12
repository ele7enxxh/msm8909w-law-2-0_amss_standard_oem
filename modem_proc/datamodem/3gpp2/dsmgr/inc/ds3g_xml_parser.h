/*!
  @file
  ds3g_xml_parser.h

  @brief
  Header file to parse DS3G XML configuration file

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3g_xml_parser.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/07/15   js      Move to DS3G folder and rename. 
08/13/15   fj      Initial version.
===========================================================================*/
#ifndef DS3G_XML_PARSER_H
#define DS3G_XML_PARSER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "list.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*----------------------------------------------------------------------------
  Define XML element
-----------------------------------------------------------------------------*/
typedef struct 
{
  char                         *Tag;  //Element name
  char                         *Text; //text content
  list_type                    *Attributes; //Attribute list. No support now.
  list_type                    *Children; //Chidren list
}ds3g_xml_element;

/*----------------------------------------------------------------------------
  Define XML Child Info type
-----------------------------------------------------------------------------*/
typedef struct 
{ 
  list_link_type                link;
  ds3g_xml_element             *element;
}ds3g_xml_list_info_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS3G_XML_PARSE

DESCRIPTION
  Parse a text file into XML document tree

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ds3g_xml_element*    ds3g_xml_parse
(
  sys_modem_as_id_e_type           subs_id,
  char                            *file_path
);

/*===========================================================================
FUNCTION DS3G_XML_FREE_ELEMENT

DESCRIPTION
  Free element tree memory

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
  ds3g_xml_element            *element_ptr  
);

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
  ds3g_xml_element            *xml_root,
  char                        *tag_str,
  list_type                   *element_list
);

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
);


/*===========================================================================
FUNCTION DS3G_XML_FREE_ELEMENTS_LIST

DESCRIPTION
  Free element list and the allocated memory

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
  list_type            *list_head
);
#endif /*DS3G_XML_PARSER_H*/
