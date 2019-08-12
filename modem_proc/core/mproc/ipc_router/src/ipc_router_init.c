/*===========================================================================

                    I P C    R O U T E R   I N I T

   This file does the node and OS specific initialization of IPC Router
   and sets up all the links.

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/ipc_router/src/ipc_router_init.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_core.h"
#include "ipc_router_xal.h"
#include "ipc_router_xal_smdl.h"
#include "DALSys.h"
#include "DALSysTypes.h"

/* EXAMPLE INIT FOR MPSS DIME (8974) */

/* Change default for each image PW */
#define IPC_ROUTER_DEFAULT_PROCESSOR_ID 0

static int ipc_router_inited = 0;
extern void ipc_router_qdi_init(void);

static unsigned int ipc_router_get_processor_id(void)
{
  DALSYS_PROPERTY_HANDLE_DECLARE(hSpmDevCfg);
  DALSYSPropertyVar prop;
  DALResult result;

  result = DALSYS_GetDALPropertyHandleStr("/dev/core/mproc/ipc_router", hSpmDevCfg);
  if(result != DAL_SUCCESS)
  {
    return IPC_ROUTER_DEFAULT_PROCESSOR_ID;
  }
  result = DALSYS_GetPropertyValue( hSpmDevCfg, "local_processor_id", 0, &prop );
  if(result != DAL_SUCCESS)
  {
    return IPC_ROUTER_DEFAULT_PROCESSOR_ID;
  }
  return (unsigned int)prop.Val.dwVal;
}

extern void qsocket_init(void);

void ipc_router_init(void)
{
  if(ipc_router_inited)
    return;
  ipc_router_inited = 1;
  /* Initialize router and start transports */
  ipc_router_core_init(ipc_router_get_processor_id());

  qsocket_init();

#ifdef FEATURE_IPC_ROUTER_QDI_DRIVER
  ipc_router_qdi_init();
#endif

  /* Link to Apps */
  {
    static ipc_router_xal_smdl_param_type param = 
    {
      "IPCRTR",
      SMD_APPS_MODEM,
      SMD_STANDARD_FIFO,
      SMDL_OPEN_FLAGS_MODE_PACKET,
      TRUE
    };

    ipc_router_xal_start_xport(&ipc_router_xal_smdl, (void *)&param, 0, "APPS");
  }

  /* Link to Adsp */
  {
    static ipc_router_xal_smdl_param_type param = 
    {
      "IPCRTR",
      SMD_MODEM_QDSP,
      SMD_STANDARD_FIFO,
      SMDL_OPEN_FLAGS_MODE_PACKET,
      TRUE
    };

    ipc_router_xal_start_xport(&ipc_router_xal_smdl, (void *)&param, 0, "QDSP");
  }

  /* Link to Riva */
  {
    static ipc_router_xal_smdl_param_type param = 
    {
      "IPCRTR",
      SMD_MODEM_RIVA,
      SMD_STANDARD_FIFO,
      SMDL_OPEN_FLAGS_MODE_PACKET,
      TRUE
    };

    ipc_router_xal_start_xport(&ipc_router_xal_smdl, (void *)&param, 0, "RIVA");
  }
}

