#ifndef RLC_REPORT_PRINTF_H
#define RLC_REPORT_PRINTF_H
/******************************************************
Monitor Reporting Through Tracer Header
******************************************************/

#include "comdef.h"
#include "rlc_v.h"

rlc_error_type rlc_report_printf_init(rlc_attribute_type *pAttribute);

/* Reports through printf */
rlc_error_type rlc_report_printf(rlc_attribute_type *pAttribute, char* string);

#endif /*RLC_REPORT_PRINTF_H*/
