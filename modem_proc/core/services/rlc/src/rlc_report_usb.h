#ifndef RLC_REPORT_QXDM_H
#define RLC_REPORT_QXDM_H
#ifndef FEATURE_WINDOWS_SIM
/******************************************************
Monitor Reporting Through Tracer Header
******************************************************/

#include "rlc.h"

/* Reports through tracer APIs for the client */
rlc_error_type rlc_report_qxdm_output(rlc_attribute_type *pAttribute, rlc_param_data_type *pData);

/* Composes a message for the client */
void rlc_report_printf_qxdm_msg(rlc_param_data_type *pData, char *buf);

#endif /*FEATURE_WINDOWS_SIM*/
#endif /*RLC_REPORT_QXDM_H*/
