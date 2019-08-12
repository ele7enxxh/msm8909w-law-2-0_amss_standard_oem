/*==============================================================================

                          ds_andsf_xml_updater.h

GENERAL DESCRIPTION
  ANDSF Threshold Manager

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/20/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_ANDSF_XML_UPDATER_H
#define DS_ANDSF_XML_UPDATER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_ASString.h"

namespace Appsrv
{
namespace XML
{
/* Forward declaration */
class XMLElement;
} /* namespace XML */


namespace ANDSF
{

struct ANDSFThresholdValues;

struct ANDSFChangeFnc
{
  virtual void operator()(XML::XMLElement* elem) const = 0;
};



class ANDSFXMLUpdater
{
public:
  ANDSFXMLUpdater(const char* fpath);

  uint16 update_threshold_values(const ANDSFThresholdValues& orig_values, const ANDSFThresholdValues& chg_values);
  bool update_apn_name(const ASString& old_apn_name, const ASString& chg_apn_name);



private:
  uint16 update_xml(const ANDSFChangeFnc& change_functor);
  bool output_to_file(const XML::XMLElement* root);

  ASString file_path;
};


} /* namespace ANDSF */
} /* namespace Appsrv */


#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_XML_UPDATER_H */
