#ifndef FEATURE_WINDOWS_SIM
/******************************************************
Monitor Task
******************************************************/
#include "rlc.h"
#include "rlc_v.h"
#include "rlc_os.h"
#include "rlc_report_qxdm.h"
//#include "msg.h"
#include "msg_diag_service.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>

//const char* rlc_generic_report_string = "The value of variable 1 is 0x%llx and variable 2 is 0x%llx\n";

/**
Functions
*/
rlc_error_type rlc_report_qxdm_output(rlc_attribute_type *pAttribute, rlc_param_data_type *pData)
{
  rlc_param_data_type *internalParam = &pAttribute->report_ptr->param_data;

  if(pAttribute == NULL)
  {
    return RLC_FAILED;
  }

  if(pAttribute->report_ptr == NULL)
  {
    return RLC_FAILED;
  }

  if(pData)
  {
      if(pData->buf1 &&
         pData->buf2)
      {
        //rlc_report_qxdm_compose_msg(pAttribute, &pAttribute->report_ptr->param_data);

        MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Reporting through QXDM, variable 1 is 0x%x and variable 2 is 0x%x",
                 *(uint32*)pData->buf1,
               *(uint32*)pData->buf2);
    }
  }
  else if(internalParam != NULL)
  {
    if((internalParam->buf1 != NULL) &&
       (internalParam->buf2 != NULL))
    {
      //rlc_report_qxdm_compose_msg(pAttribute, &pAttribute->report_ptr->param_data);

      MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Reporting through QXDM, variable 1 is 0x%x and variable 2 is 0x%x",
               *(uint32*)internalParam->buf1,
               *(uint32*)internalParam->buf2);
    }
  }

  return RLC_SUCCESS;
}

void rlc_report_printf_qxdm_msg(rlc_param_data_type *pData, char *buf)
{
//    memset(buf, 0, RLC_REPORT_BUF_SIZE);
#if 0
    if(pData->size == sizeof(uint32))
    {
      lval1 = (uint64)*((uint32*)pData->buf1);
      lval2 = (uint64)*((uint32*)pData->buf2);
      snprintf(buf,
                  RLC_REPORT_BUF_SIZE,
                  rlc_generic_report_string,
                  lval1,
                  lval2);
    }
    else if(pData->size == sizeof(uint64))
    {
      lval1 = (uint64)*((uint64*)pData->buf1);
      lval2 = (uint64)*((uint64*)pData->buf2);
      snprintf(buf,
                  RLC_REPORT_BUF_SIZE,
                  rlc_generic_report_string,
                  lval1,
                  lval2);
    }
    else
    {
      return;
    }
#endif


    return;

}

#endif /* FEATURE_WINDOWS_SIM */
