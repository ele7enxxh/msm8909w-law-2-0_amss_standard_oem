/*===========================================================================

  gpio_toggle.c - resource to allow masters to toggle GPIO pins

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "Chipinfo.h"
#include "gpio_toggle.h"
#include "time_service.h"
#include "rpmserver.h"

void gpio_toggle_xlate(rpm_translation_info *info)
{
    unsigned               type, length, value;
    const char             *value_ptr;
    gpio_toggle_inrep *req = info->dest_buffer;

    // Read until we run out of KVPs
    while( !kvp_eof(info->new_kvps) )
    {
        if(!kvp_get( info->new_kvps, &type, &length, &value_ptr ))
        {
            abort();
        }

        CORE_VERIFY_PTR(value_ptr);
        value = *(unsigned*)value_ptr;

        switch(type)
        {
            case REQ_GPIO_NUM:
                if (sizeof(uint32) != length)
                  abort();
                req->pin = (uint8)value;
                break;
            case REQ_GPIO_STATE:
                if (sizeof(uint32) != length)
                  abort();
                req->state = (value?1:0);
            case REQ_GPIO_SETTTLING_TIME:
                if (sizeof(uint32) != length)
                  abort();
                req->settling_time = (uint16)value;
            default:
            {
                //unknown key
            }
        }
    }
}

static void gpio_toggle_apply(rpm_application_info *info)
{
    uint32 gpio_num = 0;
    uint32 state = 0;
    uint32 gpio_address;

    if(0 == info->settling_time)
    {
      //update state
      gpio_toggle_inrep *new_req = (gpio_toggle_inrep *)info->new_state;

      gpio_num = new_req->pin;
      state = new_req->state;

      gpio_address = GPIO_ADR_BASE+(state*GPIO_STATE_OFFSET)+((gpio_num/32)*GPIO_BNK_OFFSET);


      //configure GPIO for output
      HWIO_TLMM_GPIO_CFGn_OUTI(gpio_num,0x303);
      //set GPIO state on listed GPIO
      out_dword(gpio_address,1 << (gpio_num%32));

      //feedback to RPM server with settling timeinfo
      if(new_req->settling_time)
      {
        info->settling_time = time_service_now() + new_req->settling_time;
      }
    }
    else
    {
      // reset the settling time
      info->settling_time = 0;
    }
}

void gpio_toggle_init()
{
    // gpio0,gpio1,gpio2 For generic purpose
    rpm_register_resource(RPM_GPIO_TOGGLE_REQ, 3, sizeof(gpio_toggle_inrep), gpio_toggle_xlate, gpio_toggle_apply, 0);
}

