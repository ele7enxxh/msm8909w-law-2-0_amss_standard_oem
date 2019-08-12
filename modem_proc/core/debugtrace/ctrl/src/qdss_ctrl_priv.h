#ifndef QDSS_CTRL_PRIV_H
#define QDSS_CTRL_PRIV_H

/*=============================================================================

FILE:         qdss_ctrl_priv.h

DESCRIPTION:  Private data structures used by the qdss ctrl subsystem

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "DALSys.h"
#include "npa.h"
#include "tracer.h"

#define QDSS_CTRL_SUCCESS          0
#define QDSS_CTRL_ERROR            1

struct qdss_ctrl_s {
   npa_client_handle npa_client;
   DALSYSSyncHandle hSync;
   tracer_client_handle_t hTracer;
   uint32 test_data_count;
   boolean clk_enabled;
   uint8 etm_state;
   uint8 etm_state_rpm;
   uint32 etm_default_state;
};

extern struct qdss_ctrl_s qdss_ctrl_ctxt;

#endif //QDSS_CTRL_PRIV_H

