/*===========================================================================

                         D S _ Q M I _ V S . C

NOTE TO VENDORS: Vendor-specific service source files will have to be added to 
                 datacommon_qmi.min file in order to compile.

DESCRIPTION

 Qualcomm-provided template source file for QMI Vendor Specific Service.

EXTERNALIZED FUNCTIONS

  qmi_vs1_init()
    Register the Vendor Specific Service1 with QMUX for all applicable QMI links.
     
      
Copyright (c) 2004-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_vs.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include <stdio.h>
#include <string.h>

#include "amssassert.h"
#include "dsm.h"

#include "ds_qmux_ext.h"
#include "ds_qmi_svc_ext.h"
#include "ds_qmi_vs.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_utils.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Service configuration in this example
  - maximum number of clients per Vendor Specific instance = 2
  - Vendor Specific service version is 0.0.0.0
---------------------------------------------------------------------------*/
#define VS1_MAX_CLIDS  (2) // Cannot exceed 255

#define VS1_BASE_VER_MAJOR  (0)
#define VS1_BASE_VER_MINOR  (0)
#define VS1_ADDENDUM_VER_MAJOR  (0)
#define VS1_ADDENDUM_VER_MINOR  (0)

#define VS1_CMD_MAX (1) // Needs to be updated when new command handlers are added

/*===========================================================================

                                DATA TYPES

===========================================================================*/

typedef enum
{
  VS1_CMD_VAL_CMD1     = 0x0000,
//  VS1_CMD_VAL_CMD2     = 0x0001,
  VS1_CMD_VAL_WIDTH    = 0xFFFF
} qmi_vs1_cmd_val_e_type;

/*---------------------------------------------------------------------------
  QMI Vendor Specific Service instance state definition &
  Vendor Specific Service client state definition
---------------------------------------------------------------------------*/
typedef struct qmi_vendor_specific_client_state_s
{
  byte  common[QMI_COMMON_CLIENT_STATE_SIZE]; // must be first since we alias
  int32 client_xyz;  // example
} qmi_vendor_specific_client_state_type;

/*---------------------------------------------------------------------------
  Structure to maintain Vendor Specific Service state
---------------------------------------------------------------------------*/
typedef struct qmi_vendor_specific_service_state_s
{
  byte   common[QMI_COMMON_SVC_STATE_SIZE]; // must be first since we alias
  qmi_vendor_specific_client_state_type  client[VS1_MAX_CLIDS];
  int32  service_xyz;  // example
} qmi_vendor_specific_service_state_type;

/*---------------------------------------------------------------------------
   Memory for holding the service state 
---------------------------------------------------------------------------*/
static qmi_vendor_specific_service_state_type qmi_vs1_state[1] = {0,};

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI Vendor Specific service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/
static dsm_item_type*  qmi_vs1_cmd_hdlr1( void*, 
                                           void*, 
                                           void*, 
                                           dsm_item_type**);

static qmi_svc_cmd_hdlr_type  qmi_vs1_cmd_callbacks[VS1_CMD_MAX] =
{
  {VS1_CMD_VAL_CMD1, "cmd1_description", (qmi_svc_hdlr_ftype)qmi_vs1_cmd_hdlr1},
//  {VS1_CMD_VAL_CMD2, "cmd2_description", (qmi_svc_hdlr_ftype)qmi_vs1_cmd_hdlr2}
};

static void *   qmi_vs1i_get_next_client_sp(void *, void *);
static void     qmi_vs1i_reset_client(void *);

/*---------------------------------------------------------------------------
  QMI_WS instance configurations
---------------------------------------------------------------------------*/
static qmux_svc_config_type  qmi_vs1_cfg[1] =
{
  { { VS1_BASE_VER_MAJOR, VS1_BASE_VER_MINOR },
    { VS1_ADDENDUM_VER_MAJOR, VS1_ADDENDUM_VER_MINOR },
    { NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      qmi_vs1i_get_next_client_sp,
      qmi_vs1i_reset_client },
      qmi_vs1_cmd_callbacks,
      VS1_CMD_MAX,
      &qmi_vs1_state[0]
  }
};

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_VS1_INIT()

  DESCRIPTION
    Register the Vendor Specific Service with QMUX for all applicable QMI links

  PARAMETERS
    QMI Instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_vs1_init
(
  qmi_instance_e_type  qmi_inst
)
{
  qmi_vendor_specific_service_state_type * vs1_sp;
  qmux_svc_info_type    *      qmux_svc_handle;
  int                           client;
  qmi_vendor_specific_client_state_type *  cl_sp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(qmi_inst < QMI_INSTANCE_MAX);

  /* check that qmi_inst is within range in case ASSERT is removed */
  if (qmi_inst >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!", qmi_inst);
    return;
  }
  
  vs1_sp = &qmi_vs1_state[qmi_inst];
  memset( vs1_sp, 0, sizeof(qmi_vendor_specific_service_state_type) );

  qmux_svc_handle = qmux_reg_vs_service(qmi_inst,
                                        VENDOR_SERVICE_1,
                                        &qmi_vs1_cfg[qmi_inst]);

  /*-----------------------------------------------------------------------
    Service State Initialization
  -----------------------------------------------------------------------*/
  qmi_svc_common_svc_init(&vs1_sp->common, qmux_svc_handle);

  /* Add Vendor Specific Service State Initialization here*/
    
  /*-----------------------------------------------------------------------
    Client State Initialization
  -----------------------------------------------------------------------*/
  cl_sp = vs1_sp->client;
  for (client=0; client < VS1_MAX_CLIDS; client++, cl_sp++)
  {
    qmi_svc_common_cl_init(&vs1_sp->common, &cl_sp->common);

   /* Add Vendor Specific Client State Initialization here */
  }
} /* qmi_vs1_init() */

/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
static void *  qmi_vs1i_get_next_client_sp
(
  void *  sp,
  void *  cl_sp
)
{
  qmi_vendor_specific_service_state_type * vs1_sp;
  qmi_vendor_specific_client_state_type *  first_cl_sp;
  qmi_vendor_specific_client_state_type *  vs1_client_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);

  vs1_sp = (qmi_vendor_specific_service_state_type *) sp;
  first_cl_sp = vs1_sp->client;
  if ( cl_sp == NULL ) 
  {
    return first_cl_sp;
  }

  vs1_client_sp = (qmi_vendor_specific_client_state_type *) cl_sp;

  vs1_client_sp++;
  if (vs1_client_sp < first_cl_sp + VS1_MAX_CLIDS) 
  {
    return vs1_client_sp;
  }
  else
  {
    return NULL;
  }
} /* qmi_vs1i_get_next_client_sp() */


static void qmi_vs1i_reset_client
(
  void * cl_sp_in
)
{
  qmi_vendor_specific_client_state_type *  cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cl_sp_in);

  cl_sp = (qmi_vendor_specific_client_state_type *)cl_sp_in;
  cl_sp->client_xyz = 0;
  qmi_svc_common_reset_client(&cl_sp->common);
  return;
} /* qmi_vs1i_reset_client() */

/*===========================================================================
  FUNCTION QMI_VS1_CMD_HDLR1()

  DESCRIPTION
    Command handler 1
    
  PARAMETERS
    sp           : service provided state pointer (user data)
    cmd_buf_p    : cmd buffer
    cl_sp        : client state pointer
    sdu_in       : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_vs1_cmd_hdlr1
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type * response;
  int32 data_out = 3;
  uint16 errval = 0;
  uint16 result = 0;
  uint16 len;
  uint8 type = 1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  /*-------------------------------------------------------------------------
    In this case there are no input TLVs so no parsing to be done. 
    Form the response and send it
    e.g response consists of two TLVs: one is the result tlv and the 
    other is a tlv containing an int32. 
  -------------------------------------------------------------------------*/
  response = NULL;

  len = sizeof(int32);
  if (len != PS_UTILS_DSM_PUSHDOWN_PACKED(&response, &data_out, len, DSM_DS_SMALL_ITEM_POOL))
  {
    LOG_MSG_INFO1_0("cannot populate value response");
    errval = 1; 
    dsm_free_packet(&response);
  }
  else
  { 
    if (sizeof(len) != PS_UTILS_DSM_PUSHDOWN_PACKED(&response, &len, sizeof(len), DSM_DS_SMALL_ITEM_POOL))
    {
      LOG_MSG_INFO1_0("cannot populate length in  response");
      errval = 1; 
      dsm_free_packet(&response);
    }
    else
    {
      if (sizeof(type) != PS_UTILS_DSM_PUSHDOWN_PACKED(&response, &type, sizeof(type), DSM_DS_SMALL_ITEM_POOL))
      {
        LOG_MSG_INFO1_0("cannot populate type in  response");
        errval = 1; 
        dsm_free_packet(&response);
      }
    }
  }

  if (errval == 0)
  {
    LOG_MSG_INFO2_0("Success");
    result = 0;
  }
  else
  {
    LOG_MSG_INFO1_0("Error in cmd handler");
    result = 1;
  }
  /* Push result tlv in the response */
  type = 2;
  len = sizeof(uint16) + sizeof(uint16);
  PS_UTILS_DSM_PUSHDOWN_PACKED(&response, &result, sizeof(uint16), DSM_DS_SMALL_ITEM_POOL);
  PS_UTILS_DSM_PUSHDOWN_PACKED(&response, &errval, sizeof(uint16), DSM_DS_SMALL_ITEM_POOL);
  PS_UTILS_DSM_PUSHDOWN_PACKED(&response, &len, sizeof(len), DSM_DS_SMALL_ITEM_POOL);
  PS_UTILS_DSM_PUSHDOWN_PACKED(&response, &type, sizeof(type), DSM_DS_SMALL_ITEM_POOL);

  return response;
} /* qmi_vs1_cmd_hdlr1() */
