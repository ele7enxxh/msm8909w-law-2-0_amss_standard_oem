#ifndef _POLICYMAN_XML_H_
#define _POLICYMAN_XML_H_

/**
  @file policyman_xml.h

  @brief  XML parser for policyman
*/

/*
    Copyright (c) 2013,2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_xml.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"

//#define policyman_xml_root_t            mre_xml_root_t
#define policyman_xml_parse             mre_xml_parse
//#define policyman_xml_get_root_element  mre_xml_get_root_element
#define policyman_xml_get_tag           mre_xml_get_tag
#define policyman_xml_tag_is            mre_xml_tag_is
#define policyman_xml_get_attribute     mre_xml_get_attribute
#define policyman_xml_get_text          mre_xml_get_text
#define policyman_xml_num_children      mre_xml_num_children
#define policyman_xml_get_child         mre_xml_get_child

#endif /* _POLICYMAN_XML_H_ */
