
/*!
  @file
  rfdevice_efs_pa_data.cpp

  @brief
  Data settings for EFS based 3rd party pa devices

*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/src/rfdevice_efs_pa_data.cpp#1 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/24/14   sn      Initial creation
==============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"
#include "rfdevice_efs_pa_data.h"
#include "rfdevice_msg_log.h"
extern "C" {
#include "modem_mem.h"
}

rfdevice_pa_data * rfdevice_efs_pa_data::get_instance(uint8* data)
{
  rfdevice_pa_data* rfdevice_efs_pa_data_ptr = NULL;
  rfdevice_efs_pa_data_ptr = (rfdevice_pa_data *)new rfdevice_efs_pa_data(data);
  return(rfdevice_efs_pa_data_ptr);
}

//constructor
rfdevice_efs_pa_data::rfdevice_efs_pa_data(uint8* data)
  :rfdevice_pa_data()
{
  uint16 j=0;
  uint8 num_regs;

  efs_pa_ptr = data;
  efs_pa.pa_data.cfg.num_regs = 0;
  efs_pa.pa_data.cfg.addr=NULL;
  efs_pa.pa_data.cfg.data=NULL;

  efs_pa.device_info.mfg_id = data[0] | data[1]<<8;
  efs_pa.device_info.prd_id = data[2];
  efs_pa.device_info.prd_rev = data[4];
  efs_pa.device_info.max_ports = data[6];
  efs_pa.device_info.num_pa_ranges = data[8];

  j=10;

  if (data[j] == 'C' && data[j+1] == 'N' &&  data[j+2] == 'F' && data[j+3] == 'G')
  {
    j=j+4;
    num_regs=data[j];
    efs_pa.pa_data.cfg.num_regs = num_regs;
    j=j+2;
    efs_pa.pa_data.cfg.addr = &data[j];
    j=j+num_regs;
    efs_pa.pa_data.cfg.data = (int16*)&data[j];
    j=j+(num_regs*(data[6]*2));
  }
  if (data[j] == 'B' && data[j+1] == 'I' &&  data[j+2] == 'A' && data[j+3] == 'S')
  {
    j=j+4;
    num_regs=data[j];
    efs_pa.pa_data.bias.num_regs = num_regs;
    j=j+2;
    efs_pa.pa_data.bias.addr = &data[j];
    j=j+num_regs;
    efs_pa.pa_data.bias.data = (int16*)&data[j];
    j=j+(num_regs*data[6]*data[8]*2);
  }
  if (data[j] == 'R' && data[j+1] == 'N' &&  data[j+2] == 'G' && data[j+3] == 'E')
  {
    j=j+4;
    num_regs=data[j];
    efs_pa.pa_data.range.num_regs = num_regs;
    j=j+2;
    efs_pa.pa_data.range.addr = &data[j];
    j=j+num_regs;
    efs_pa.pa_data.range.data = (int16*)&data[j];
    j=j+(num_regs*data[6]*data[8]*2);
  }
  if (data[j] == 'P' && data[j+1] == 'A' &&  data[j+2] == 'O' && data[j+3] == 'N')
  {
    j=j+4;
    num_regs=data[j];
    efs_pa.pa_data.on.num_regs = num_regs;
    j=j+2;
    efs_pa.pa_data.on.addr = &data[j];
    j=j+num_regs;
    efs_pa.pa_data.on.data = (int16*)&data[j];
    j=j+(num_regs*(data[6]*2));
  }
  if (data[j] == 'P' && data[j+1] == 'A' &&  data[j+2] == 'O' && data[j+3] == 'F')
  {
    j=j+4;
    num_regs=data[j];
    efs_pa.pa_data.off.num_regs = num_regs;
    j=j+2;
    efs_pa.pa_data.off.addr = &data[j];
    j=j+num_regs;
    efs_pa.pa_data.off.data = (int16*)&data[j];
    j=j+(num_regs*(data[6]*2));
  }
  if (data[j] == 'T' && data[j+1] == 'R' &&  data[j+2] == 'I' && data[j+3] == 'G')
  {
    j=j+4;
    num_regs=data[j];
    efs_pa.pa_data.trigger.num_regs = num_regs;
    j=j+2;
    efs_pa.pa_data.trigger.addr = &data[j];
    j=j+num_regs;
    efs_pa.pa_data.trigger.data = (int16*)&data[j];
    j=j+(num_regs*(data[6]*2));
  }
}

//destructor
rfdevice_efs_pa_data::~rfdevice_efs_pa_data()
{

  modem_mem_free((void*)efs_pa_ptr,MODEM_MEM_CLIENT_RFA);

}

boolean rfdevice_efs_pa_data::settings_data_get( rfdevice_pa_cfg_params_type *cfg,
    rfdevice_pa_reg_settings_type *settings)
{
  boolean ret_val = FALSE;
  uint16 index=0;

  if (NULL == settings || NULL == cfg)
  {
    return FALSE;
  }

  if (cfg->port >= efs_pa.device_info.max_ports )
  {
    settings->addr = NULL;
    settings->data = NULL;
    settings->num_regs = 0;
    return FALSE;
  }
  RFDEVICE_LOG_MSG(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_HIGH, "Obtaining EFS PA dat file settings");

  if ( (cfg->req == RFDEVICE_PA_CONFIG_DATA) )
  {
    settings->addr = efs_pa.pa_data.cfg.addr;
    settings->num_regs = efs_pa.pa_data.cfg.num_regs;
    index = (cfg->port)*efs_pa.pa_data.cfg.num_regs;
    settings->data = &(efs_pa.pa_data.cfg.data[index]);
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_SET_BIAS_DATA) && (cfg->pa_range < efs_pa.device_info.num_pa_ranges) )
  {
    settings->addr = efs_pa.pa_data.bias.addr;
    settings->num_regs = efs_pa.pa_data.bias.num_regs;
    index = (efs_pa.pa_data.bias.num_regs*efs_pa.device_info.num_pa_ranges*cfg->port) + (cfg->pa_range * efs_pa.pa_data.bias.num_regs);
    settings->data = efs_pa.pa_data.bias.data + index;
    settings->bias_reg_idx = 0;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_SET_RANGE_DATA) && (cfg->pa_range < efs_pa.device_info.num_pa_ranges) )
  {
    settings->addr = efs_pa.pa_data.range.addr;
    settings->num_regs = efs_pa.pa_data.range.num_regs;
    index = (efs_pa.pa_data.range.num_regs*efs_pa.device_info.num_pa_ranges*cfg->port) + (cfg->pa_range * efs_pa.pa_data.range.num_regs);
    settings->data = efs_pa.pa_data.range.data + index ;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_ON_DATA) )
  {
    settings->addr = efs_pa.pa_data.on.addr;
    settings->data = &(efs_pa.pa_data.on.data[cfg->port]) ;
    settings->num_regs = efs_pa.pa_data.on.num_regs;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_OFF_DATA) )
  {
    settings->addr = efs_pa.pa_data.off.addr;
    settings->data = &(efs_pa.pa_data.off.data[cfg->port]) ;
    settings->num_regs = efs_pa.pa_data.off.num_regs;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_PA_TRIGGER_DATA) )
  {
    settings->addr = efs_pa.pa_data.trigger.addr;
    settings->data = &(efs_pa.pa_data.trigger.data[cfg->port]) ;
    settings->num_regs = efs_pa.pa_data.trigger.num_regs;
    ret_val = TRUE;
  }

  return ret_val;
}

boolean rfdevice_efs_pa_data::device_info_get( rfdevice_pa_info_type *pa_info )
{
  boolean ret_val = FALSE;

  if ( NULL == pa_info )
  {
    return FALSE;
  }
  else
  {
    pa_info->mfg_id = efs_pa.device_info.mfg_id;
    pa_info->prd_id = efs_pa.device_info.prd_id;
    pa_info->prd_rev = efs_pa.device_info.prd_rev;
    pa_info->num_ports = efs_pa.device_info.max_ports;
    pa_info->num_pa_ranges = efs_pa.device_info.num_pa_ranges;
    ret_val = TRUE;
  }

  return ret_val;
}

