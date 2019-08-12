/*=============================================================================

FILE:         qdss_ctrl_etm_qurt.c

DESCRIPTION:

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "qdss_ctrl_etm.h"
#include "qurt/qurt_trace.h"
#include "tracer_cfgrpm.h"

int qdss_ctrl_etm_enable(unsigned char etm_mode)
{
   unsigned int ret1,ret2;
   ret1 =  qurt_etm_set_config(etm_mode,
                               QURT_ETM_ROUTE_TO_QDSS,
                               QURT_ETM_TRACE_FILTER_ALL);
   ret2 = qurt_etm_enable(QURT_ETM_ON);

   return ((int)(ret1||ret2));
}

int qdss_ctrl_etm_disable(void)
{
   return ((int) qurt_etm_enable(QURT_ETM_OFF));
}

int qdss_ctrl_etm_rpm_set(uint8 state)
{
   return (0 == tracer_cfgrpm_etm(state)) ? 0 : 1;
}
