/*==============================================================================

                        ds_XMLDecoder_ANDSF.cpp

GENERAL DESCRIPTION
  Decoder for ANDSF

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
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_XMLDecoder_ANDSF.h"
#include "ds_xml_ANDSFManagementObject.h"
#include "ds_appsrv_mem.h"

#include "data_msg.h"

namespace ASXML = Appsrv::XML;



ASXML::XMLDecoder_ANDSF::XMLDecoder_ANDSF(const char* xml_content)
: XMLDecoder(xml_content), mo(NULL)
{
  decode();
}

ASXML::XMLDecoder_ANDSF::XMLDecoder_ANDSF(const char* xml_content, uint32 len)
: XMLDecoder(xml_content, len), mo(NULL)
{
  decode();
}

ASXML::XMLDecoder_ANDSF::~XMLDecoder_ANDSF()
{
  XMLDecoder_ANDSF::clean();
}

ASXML::ANDSFManagementObject* ASXML::XMLDecoder_ANDSF::get_andsf_mo()
{
  return mo;
}

void ASXML::XMLDecoder_ANDSF::clean()
{
  if(NULL != mo)
    ds_appsrv_free(mo);

  mo = NULL;
}

void ASXML::XMLDecoder_ANDSF::decode()
{
  if(flag) return;
  mo = new(ds_appsrv_alloc<ANDSFManagementObject>(), APPSRV_MEM) ANDSFManagementObject(root, flag);

  if(NULL == mo)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "XMLDecoder_ANDSF::decode - Failed to allocate memory for ANDSF management object");
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, "ANDSF MO");
  }
  else if(flag)
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



