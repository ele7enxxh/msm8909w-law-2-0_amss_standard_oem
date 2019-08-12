/*==============================================================================

                        ds_XMLDecoder_ATP.cpp

GENERAL DESCRIPTION
  Decoder for ATP

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
05/20/15    ml     Use memory from modem heap
12/08/14    ml     Fixed ATP XML decoding failure
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_XMLDecoder_ATP.h"

#include "ds_XMLElement.h"
#include "ds_XMLUtility.h"
#include "ds_appsrv_mem.h"

#include "data_msg.h"
#include <cstdlib> // strtoul

using namespace Appsrv::XML;


const char ELEM_NAME_ROOT[]          = "ATP_Policy";
const char ELEM_NAME_VERSION[]       = "Version";
const char ELEM_NAME_APN[]           = "APN";
const char ELEM_NAME_POLICY_SERVER[] = "PolicyServer";
const char ELEM_NAME_REPORT_SERVER[] = "ReportServer";
const char ELEM_NAME_REFRESH_TIME[]  = "PolicyRefreshTime";
const char ELEM_NAME_SERVICE[]       = "Service";
const char ELEM_NAME_PACKAGE[]       = "Package";
const char ELEM_NAME_PKG_NAME[]      = "Name";
const char ELEM_NAME_PKG_HASHCODE[]  = "HashCode";
const char ELEM_NAME_PKG_ACK[]       = "MaxAckTimeOut";
const char ELEM_NAME_PKG_FWUID[]     = "FrameworkUID";

const char ATTR_NAME_APN_NAME[]      = "Name";
const char ATTR_NAME_SVC_ID[]        = "Id";



ATP_Policy::~ATP_Policy()
{
  clear_ptr_vector(packages);
}


XMLDecoder_ATP::XMLDecoder_ATP(const char* xml_content)
: XMLDecoder(xml_content), policy(NULL)
{
  decode(root);

  if(flag)
  {
    DATA_APPSRV_MSG_SPRINTF_2(
                              MSG_LEGACY_ERROR,
                              "XMLDecoder::XMLDecoder - XMLDecoderException : %s; %s",
                              XMLDecoderException::get_instance().what(),
                              XMLDecoderException::get_instance().get_details()
                              );
    clean();
    XMLDecoder::clean();
  }
}

XMLDecoder_ATP::XMLDecoder_ATP(const char* xml_content, uint32 len)
: XMLDecoder(xml_content, len), policy(NULL)
{
  decode(root);

  if(flag)
  {
    DATA_APPSRV_MSG_SPRINTF_2(
                              MSG_LEGACY_ERROR,
                              "XMLDecoder::XMLDecoder - XMLDecoderException : %s; %s",
                              XMLDecoderException::get_instance().what(),
                              XMLDecoderException::get_instance().get_details()
                              );
    clean();
    XMLDecoder::clean();
  }
}

XMLDecoder_ATP::~XMLDecoder_ATP()
{
  XMLDecoder_ATP::clean();
}

void XMLDecoder_ATP::clean()
{
  if(NULL != policy)
    ds_appsrv_free(policy);

  policy = NULL;
}


void XMLDecoder_ATP::decode(Appsrv::XML::XMLElement* root)
{
  uint32        apn_counter     = 0;
  uint32        service_counter = 0;
  uint32        package_counter = 0;
  uint32        service_id;
  ASString      content;
  XMLElement*   apn_elem, *service_elem, *package_elem, *child;
  XMLAttribute* attr;

  if(flag) return;

  if(NULL == root)
  {
    XMLDecoderException::get_instance().raise(flag, "Empty root");
    return;
  }

  check_element_with_child(root, ELEM_NAME_ROOT, 0, 2, 2, flag); // *** remove max limit if multiple apn
  if(flag) return;


  /* ===== root ===== */
  policy = new(ds_appsrv_alloc<ATP_Policy>(), APPSRV_MEM) ATP_Policy();
  child = root->get_child(0);

  if(NULL == policy)
  {
    XMLDecoderException::get_instance().raise(flag, "Failed to allocate memory for policy");
    return;
  }

  check_element_with_content(child, ELEM_NAME_VERSION, 0, false, flag);
  if(flag) return;
  policy->version = child->get_text();


  // *** loop starts here if multiple apn ***

  apn_elem = root->get_child(1);
  check_element_with_child(apn_elem, ELEM_NAME_APN, 1, 2, flag);
  if(flag) return;



  /* ===== APN ===== */
  attr = apn_elem->get_attribute(0);
  check_attribute(attr, ATTR_NAME_APN_NAME, flag);
  if(flag) return;
  policy->apn_name = attr->value;


  if(!get_next_child(apn_elem, &child, apn_counter))
  {
    XMLDecoderException::get_instance().raise(flag, "Failed to get child");
    return;
  }

  if(ELEM_NAME_POLICY_SERVER == child->get_tag())
  {
    check_element_with_content(child, ELEM_NAME_POLICY_SERVER, 0, false, flag);
    if(flag) return;
    policy->policy_server = child->get_text();

    if(!get_next_child(apn_elem, &child, apn_counter))
    {
      XMLDecoderException::get_instance().raise(flag, "Failed to get child");
      return;
    }
  }

  check_element_with_content(child, ELEM_NAME_REPORT_SERVER, 0, false, flag);
  if(flag) return;
  policy->report_server = child->get_text();


  if(!get_next_child(apn_elem, &child, apn_counter))
  {
    XMLDecoderException::get_instance().raise(flag, "Failed to get child");
    return;
  }
  check_element_with_content(child, ELEM_NAME_REFRESH_TIME, 0, false, flag);
  if(flag) return;
  policy->policy_refresh_time = strtoul(child->get_text().c_str(), NULL, 10);


  // service loop
  service_counter = apn_elem->children_size();
  for(uint32 i = apn_counter; i < service_counter; i++)
  {
    service_elem = apn_elem->get_child(i);
    check_element_with_child(service_elem, ELEM_NAME_SERVICE, 1, 1, flag);
    if(flag) return;

    attr = service_elem->get_attribute(0);
    check_attribute(attr, ATTR_NAME_SVC_ID, flag);
    if(flag) return;
    service_id = strtoul(attr->value.c_str(), NULL, 10);

    // package loop
    package_counter = service_elem->children_size();
    for(uint32 j = 0; j < package_counter; j++)
    {
      uint32 max_ack_timeout;
      ASString package_name, hashcode, framework_uid;

      package_elem = service_elem->get_child(j);
      check_element_with_child(package_elem, ELEM_NAME_PACKAGE, 0, 4, 4, flag);
      if(flag) return;

      child = package_elem->get_child(0);
      check_element_with_content(child, ELEM_NAME_PKG_NAME, 0, false, flag);
      if(flag) return;
      package_name = child->get_text();

      child = package_elem->get_child(1);
      check_element_with_content(child, ELEM_NAME_PKG_HASHCODE, 0, false, flag);
      if(flag) return;
      hashcode = child->get_text();

      child = package_elem->get_child(2);
      check_element_with_content(child, ELEM_NAME_PKG_ACK, 0, false, flag);
      if(flag) return;
      max_ack_timeout = strtoul(child->get_text().c_str(), NULL, 10);

      child = package_elem->get_child(3);
      check_element_with_content(child, ELEM_NAME_PKG_FWUID, 0, true, flag);
      if(flag) return;
      framework_uid = child->get_text();

      ATP_Policy_Package* new_package
        = new(ds_appsrv_alloc<ATP_Policy_Package>(), APPSRV_MEM) ATP_Policy_Package(
                                                                                    service_id,
                                                                                    package_name,
                                                                                    hashcode,
                                                                                    max_ack_timeout,
                                                                                    framework_uid
                                                                                    );
      if(!ds_xml_add_obj_to_vector(new_package, policy->packages, flag, "ATP_Policy_Package"))
      {
        return;
      }
    }
  }
}

