/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM RFFE COMMANDS

GENERAL DESCRIPTION
  This is the FTM RFFE file which contains RFFE READ or WRITE commands

Copyright (c) 2012   by Qualcomm Technologies, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_rffe.c#1 $
  $DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/26/12   jfc     Change RFFE profile command to a generic RFFE test 
09/14/12   jfc     Move RFFE commands to common FTM 
09/07/12   jfc     Add hooks for RFCMD profiling 
08/17/12   jfc     Initial revision
===========================================================================*/
#include "rfa_variation.h"
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_rffe.h"    
#include "rf_hal_rffe.h"
#include "msg.h"                     /* Debug related stuff                         */     

extern void rfhal_cmd_intf_profile(void);


/*===========================================================================
                Define LOCAL variables  
===========================================================================*/


static void ftm_rffe_cmd(ftm_rffe_data_type* rffe_data)
{
  rf_hal_bus_result_type err;
  rf_hal_bus_resource_script_settings_type settings;
  rf_hal_bus_rffe_type script;

  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  settings.half_rate = rffe_data->halfspeed;
  settings.rffe_force_ext = rffe_data->ext;

  /* Set the RFFE transaction settings */
  script.addr = rffe_data->address;
  script.channel = rffe_data->channel;
  script.data[0] = rffe_data->data;
  script.extended_cmd = 1;
  script.half_rate = rffe_data->halfspeed;
  script.num_bytes = 1;
  script.rd_delay = 0;
  script.slave_id = rffe_data->slave;
  script.start_delta = 0;

  /* Issue the command to the HAL. Always use the 2 word ("ext") CCS format since we're doing single transactions */
  if (rffe_data->readwrite == 1)
  {
    script.rd_wr = 1;
    err = rfhal_rffe_read_bytes_ext(&script, rffe_data->channel, 1, &settings);
  }
  else
  { 
    script.rd_wr = 0;
    err = rfhal_rffe_write_bytes_ext(&script, rffe_data->channel, 1, &settings);
  }

  /* Record the read value if applicable */
  if ( (err == RF_HAL_BUS_SUCCESS) && (rffe_data->readwrite == 1) )
  {
    rffe_data->data = script.data[0];
  }
  else if ((err != RF_HAL_BUS_SUCCESS) && (rffe_data->readwrite == 1) )
  {
    rffe_data->data = 0;
  }
  /* Do nothing for write */
  
  
}


/*===========================================================================

FUNCTION FTM_RFFE_DISPATCH

DESCRIPTION
   This function performs RFFE dispatch operation.  
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_rffe_dispatch(ftm_rffe_pkt_type* rffe)
{
  ftm_rsp_pkt_type               rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  switch (rffe->rffe_cmd_header.rffe_cmd_id)
  {
  case FTM_RFFE_CMD:
    ftm_rffe_cmd(&rffe->rffe_params);
    break;

  case FTM_RFFE_DUMP:
    //TBD
    break;

  case FTM_RFFE_TEST:
    MSG_HIGH("FTM RFFE:test command called", 0, 0, 0);
    break;
    
  default:
    MSG_HIGH("FTM RFFE:unknown command", 0, 0, 0);
    break;
  }

  return rsp_pkt;
}


#endif /* FEATURE_FACTORY_TESTMOD0E */
