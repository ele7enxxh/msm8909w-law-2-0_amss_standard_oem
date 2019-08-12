
/*
WARNING: This file is auto-generated.

Generated using: asm_autogen.pl
Generated from:  3.1.121 of RFDevice_ASM.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2009 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_asm/src/rfdevice_efs_asm_data.cpp#1 $


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfdevice_efs_asm_data.h"
#include "rfcommon_msg.h"
#include "rfdevice_msg_log.h"
extern "C" {
#include "modem_mem.h"
}

rfdevice_asm_data * rfdevice_efs_asm_data::get_instance(uint8* data)
{

  rfdevice_asm_data* rfdevice_efs_asm_data_ptr = NULL;

  if (rfdevice_efs_asm_data_ptr == NULL)
  {
    rfdevice_efs_asm_data_ptr = (rfdevice_asm_data *)new rfdevice_efs_asm_data(data);
  }
  return( (rfdevice_asm_data *)rfdevice_efs_asm_data_ptr);
}

//constructor
rfdevice_efs_asm_data::rfdevice_efs_asm_data(uint8* data)
  :rfdevice_asm_data()
{
  uint16 j=0;
  uint8 num_regs;


  efs_asm_ptr = data;

  efs_asm.device_info.mfg_id = data[0] | data[1]<<8;
  efs_asm.device_info.prd_id = data[2];
  efs_asm.device_info.prd_rev = data[4];
  efs_asm.device_info.max_ports = data[6];

  /*initialize optinal field*/
  efs_asm.asm_data.init.num_regs = 0;
  efs_asm.asm_data.init.addr = NULL;
  efs_asm.asm_data.init.data = NULL;

  efs_asm.asm_data.trigger.num_regs = 0;
  efs_asm.asm_data.trigger.addr = NULL;
  efs_asm.asm_data.trigger.data = NULL;
  j=8;

  if (data[j] == 'I' && data[j+1] == 'N' &&  data[j+2] == 'I' && data[j+3] == 'T')
  {
    j=j+4;
    num_regs=data[j];
    efs_asm.asm_data.init.num_regs = num_regs;
    j=j+2;
    efs_asm.asm_data.init.addr = &data[j];
    j=j+num_regs;
    efs_asm.asm_data.init.data = (int16*)&data[j];
    j=j+(num_regs*(data[6]*2));
  }

  if (data[j] == 'A' && data[j+1] == 'S' &&  data[j+2] == 'O' && data[j+3] == 'N')
  {
    j=j+4;
    num_regs=data[j];
    efs_asm.asm_data.on.num_regs = num_regs;
    j=j+2;
    efs_asm.asm_data.on.addr = &data[j];
    j=j+num_regs;
    efs_asm.asm_data.on.data = (int16*)&data[j];
    j=j+(num_regs*(data[6]*2));
  }
  if (data[j] == 'A' && data[j+1] == 'S' &&  data[j+2] == 'O' && data[j+3] == 'F')
  {
    j=j+4;
    num_regs=data[j];
    efs_asm.asm_data.off.num_regs = num_regs;
    j=j+2;
    efs_asm.asm_data.off.addr = &data[j];
    j=j+num_regs;
    efs_asm.asm_data.off.data = (int16*)&data[j];
    j=j+(num_regs*(data[6]*2));
  }
  if (data[j] == 'T' && data[j+1] == 'R' &&  data[j+2] == 'I' && data[j+3] == 'G')
  {
    j=j+4;
    num_regs=data[j];
    efs_asm.asm_data.trigger.num_regs = num_regs;
    j=j+2;
    efs_asm.asm_data.trigger.addr = &data[j];
    j=j+num_regs;
    efs_asm.asm_data.trigger.data = (int16*)&data[j];
    j=j+(num_regs*(data[6]*2));
  }
}

//destructor
rfdevice_efs_asm_data::~rfdevice_efs_asm_data()
{
  modem_mem_free((void*)efs_asm_ptr,MODEM_MEM_CLIENT_RFA);
}

boolean rfdevice_efs_asm_data::settings_data_get( rfdevice_asm_cfg_params_type *cfg,
    rfdevice_asm_reg_settings_type *settings)
{
  boolean ret_val = FALSE;
  uint16 index=0;

  if (NULL == settings || NULL == cfg)
  {
    return FALSE;
  }

  if (cfg->port >= efs_asm.device_info.max_ports )
  {
    settings->addr = NULL;
    settings->data = NULL;
    settings->num_regs = 0;
    return FALSE;
  }

  RFDEVICE_LOG_MSG(LOG_RFA_DEVICE_ASM_C, MSG_LEGACY_HIGH, "Obtaining EFS ASM dat file settings");

  if ( (cfg->req == RFDEVICE_INIT_DATA) )
  {
    settings->addr = efs_asm.asm_data.init.addr;
    settings->num_regs = efs_asm.asm_data.init.num_regs;
    settings->data = &(efs_asm.asm_data.init.data[cfg->port * settings->num_regs]) ;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_ASM_ON_DATA) )
  {
    settings->addr = efs_asm.asm_data.on.addr;
    settings->num_regs = efs_asm.asm_data.on.num_regs;
    settings->data = &(efs_asm.asm_data.on.data[cfg->port * settings->num_regs]) ;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_ASM_OFF_DATA) )
  {
    settings->addr = efs_asm.asm_data.off.addr;
    settings->num_regs = efs_asm.asm_data.off.num_regs;
    settings->data = &(efs_asm.asm_data.off.data[cfg->port * settings->num_regs]) ;
    ret_val = TRUE;
  }

  if ( (cfg->req == RFDEVICE_ASM_TRIGGER_DATA) )
  {
    settings->addr = efs_asm.asm_data.trigger.addr;
    settings->num_regs = efs_asm.asm_data.trigger.num_regs;
    settings->data = &(efs_asm.asm_data.trigger.data[cfg->port * settings->num_regs]) ;
    ret_val = TRUE;
  }

  return ret_val;
}

boolean rfdevice_efs_asm_data::device_info_get( rfdevice_asm_info_type *asm_info )
{
  boolean ret_val = FALSE;

  if ( NULL == asm_info )
  {
    return FALSE;
  }
  else
  {
    asm_info->mfg_id = efs_asm.device_info.mfg_id;
    asm_info->prd_id = efs_asm.device_info.prd_id;
    asm_info->prd_rev = efs_asm.device_info.prd_rev;
    asm_info->num_ports = efs_asm.device_info.max_ports;
    ret_val = TRUE;
  }

  return ret_val;
}
