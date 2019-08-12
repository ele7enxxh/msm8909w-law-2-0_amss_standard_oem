/******************************************************
Monitor Task
******************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rlc.h"
#include "rlc_v.h"
#include "rlc_os.h"
#include "rlc_report_printf.h"

/**
Functions
*/
rlc_error_type rlc_report_printf_init(rlc_attribute_type *pAttribute)
{
  pAttribute->report_ptr->report_handle = rlc_malloc(sizeof(uint32));
  printf("Monitor Printf correctly initialized!\n");
  return RLC_SUCCESS;
}

rlc_error_type rlc_report_printf(rlc_attribute_type *pAttribute, char* string)
{
  if(string == NULL)
  {
    return RLC_FAILED;
  }
  printf("%s\n",string);
  return RLC_SUCCESS;
}

