/*==============================================================================

                            ds_xml_ATP_Policy.h

GENERAL DESCRIPTION
  ATP structure

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
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_XML_ATP_POLICY_H
#define DS_XML_ATP_POLICY_H

#include "ds_ASVector.h"
#include "ds_ASString.h"



struct ATP_Policy_Package
{
  uint32   service_id;
  uint32   max_ack_timeout;
  ASString package_name;
  ASString hashcode;
  ASString framework_uid;

  ATP_Policy_Package(
                     const uint32    svc_id,
                     const ASString& pkg_name,
                     const ASString& hc,
                     const uint32    max_at,
                     const ASString& fuid
                     )
  : service_id(svc_id), max_ack_timeout(max_at), package_name(pkg_name), hashcode(hc), framework_uid(fuid)
  { }
};



struct ATP_Policy
{
  uint32                        policy_refresh_time;
  ASString                      version;
  ASString                      policy_server;
  ASString                      report_server;
  ASString                      apn_name;
  ASVector<ATP_Policy_Package*> packages;

  ~ATP_Policy();
};

#endif /* DS_XML_ATP_POLICY_H */
