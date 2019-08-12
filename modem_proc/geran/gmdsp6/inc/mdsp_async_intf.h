#ifndef MDSP_ASYNC_INTF_H
#define MDSP_ASYNC_INTF_H
/*===========================================================================
                          M D S P _ A S Y N C _ I F . H


GENERAL DESCRIPTION
  This header file contains macros and definitions for the mDSP driver
  commands.

  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmdsp6/inc/mdsp_async_intf.h#1 $

when         who     what, where, why
----------  -------- ------------------------------------------------------
07/04/14    cja      CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF + HwFN incmd trace as 16-bit
22/11/13    jj       CR 580519 log  hw fn send over  asynch command 
10/05/13    cs       TSTS Updates
14/04/13    pjr      CR475908 dual data spaced firmware queue
12-11-22    tjw      Initial version.
===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "geran_msg.h"
#include "msgr_umid.h"
#include "geran_dual_sim.h"

/****************************************************************************
****************************************************************************
****************************************************************************
                          DEFINITIONS AND ENUMERATIONS


****************************************************************************
****************************************************************************
***************************************************************************/
/* Async commands sent via MSGR to / from FW */
typedef union 
{
  msgr_hdr_s   hdr;
  GfwHostAsyncCmd gfwHostAsyncCmd;
  GfwHostAsyncRsp gfwHostAsyncRsp;
  GfwHostAppModeConfigCmd gfwHostAppModeConfigCmd;
  GfwHostAppModeConfigRsp gfwHostAppModeConfigRsp;
  GfwHostRfmEnterModeCmd gfwHostRfmEnterModeCmd;
  GfwHostRfmEnterModeRsp gfwHostRfmEnterModeRsp;
  GfwHostSleepCmd gfwHostSleepCmd;
  GfwHostSleepRsp gfwHostSleepRsp; 
  GfwHostAsyncSleepReadyCmd gfwHostAsyncSleepReadyCmd;
  GfwHostAsyncImmediateCmd gfwHostAsyncImmediateCmd;
  GfwHostAsyncImmediateRsp gfwHostAsyncImmediateRsp;
  GfwHostAsyncExitCmd gfwHostAsyncExitCmd;
} gl1_gfw_msgr_u;

/*===========================================================================

FUNCTION  mdsp_async_intf_init

DESCRIPTION
  Set up MSGR message to GFW Task.

===========================================================================*/
void mdsp_async_intf_init(gas_id_t gas_id);


/*===========================================================================

FUNCTION  mdsp_async_intf_teardown

DESCRIPTION
  Tear down MSGR message to GFW Task.

===========================================================================*/
void mdsp_async_intf_teardown(gas_id_t gas_id);

/*===========================================================================

FUNCTION  mdsp_async_intf_send_blocking_cmd

DESCRIPTION
  Send MSGR message to GFW Task.
  Use MSGR blocking wait to receive rsp for this immediate cmd from GFW before continuing

===========================================================================*/
void mdsp_async_intf_send_blocking_cmd(msgr_umid_type msg, void * payload, void * response, gas_id_t gas_id );

/*===========================================================================

FUNCTION  mdsp_log_qdsp6_async_cmd

DESCRIPTION

===========================================================================*/
void  mdsp_log_qdsp6_async_cmd( msgr_umid_type async_cmd, gas_id_t gas_id ,uint16 hwfn );
#endif /* MDSP_ASYNC_INTF_H */
