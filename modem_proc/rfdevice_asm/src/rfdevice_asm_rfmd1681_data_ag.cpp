
/*
WARNING: This file is auto-generated.

Generated using: asm_autogen.pl
Generated from:  v3.0.63 of RFDevice_ASM.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2009, 2010, 2011, 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1/Main/modem_proc/rfdevice_asm/etc/asm_autogen.pl#1 n

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfdevice_asm_rfmd1681_data_ag.h"


#define RFDEVICE_ASM_RFMD1681_NUM_PORTS 42

#define RFDEVICE_ASM_RFMD1681_ASM_ON_NUM_REGS 2
static uint8 rfdevice_asm_rfmd1681_asm_on_regs[RFDEVICE_ASM_RFMD1681_ASM_ON_NUM_REGS] =  {0x00, 0x01, };
static int16 rfdevice_asm_rfmd1681_asm_on_data[RFDEVICE_ASM_RFMD1681_NUM_PORTS][RFDEVICE_ASM_RFMD1681_ASM_ON_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    1, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 1 */
    2, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 2 */
    3, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 3 */
    4, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 4 */
    5, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 5 */
    6, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 6 */
    7, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 7 */
    8, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 8 */
    9, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 9 */
    10, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 10 */
    11, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 11 */
    12, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 12 */
    13, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 13 */
    14, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 14 */
    15, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 15 */
    16, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 16 */
    17, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 17 */
    18, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 18 */
    19, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 19 */
    20, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 20 */
    21, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 21 */
    RFDEVICE_ASM_REG_INVALID, 1, 
  },
  { /* PORT NUM: 22 */
    RFDEVICE_ASM_REG_INVALID, 2, 
  },
  { /* PORT NUM: 23 */
    RFDEVICE_ASM_REG_INVALID, 3, 
  },
  { /* PORT NUM: 24 */
    RFDEVICE_ASM_REG_INVALID, 4, 
  },
  { /* PORT NUM: 25 */
    RFDEVICE_ASM_REG_INVALID, 5, 
  },
  { /* PORT NUM: 26 */
    RFDEVICE_ASM_REG_INVALID, 6, 
  },
  { /* PORT NUM: 27 */
    RFDEVICE_ASM_REG_INVALID, 7, 
  },
  { /* PORT NUM: 28 */
    RFDEVICE_ASM_REG_INVALID, 8, 
  },
  { /* PORT NUM: 29 */
    RFDEVICE_ASM_REG_INVALID, 9, 
  },
  { /* PORT NUM: 30 */
    RFDEVICE_ASM_REG_INVALID, 10, 
  },
  { /* PORT NUM: 31 */
    RFDEVICE_ASM_REG_INVALID, 11, 
  },
  { /* PORT NUM: 32 */
    RFDEVICE_ASM_REG_INVALID, 12, 
  },
  { /* PORT NUM: 33 */
    RFDEVICE_ASM_REG_INVALID, 13, 
  },
  { /* PORT NUM: 34 */
    RFDEVICE_ASM_REG_INVALID, 14, 
  },
  { /* PORT NUM: 35 */
    RFDEVICE_ASM_REG_INVALID, 15, 
  },
  { /* PORT NUM: 36 */
    RFDEVICE_ASM_REG_INVALID, 16, 
  },
  { /* PORT NUM: 37 */
    RFDEVICE_ASM_REG_INVALID, 17, 
  },
  { /* PORT NUM: 38 */
    RFDEVICE_ASM_REG_INVALID, 18, 
  },
  { /* PORT NUM: 39 */
    RFDEVICE_ASM_REG_INVALID, 19, 
  },
  { /* PORT NUM: 40 */
    RFDEVICE_ASM_REG_INVALID, 20, 
  },
  { /* PORT NUM: 41 */
    RFDEVICE_ASM_REG_INVALID, 21, 
  },
};


#define RFDEVICE_ASM_RFMD1681_ASM_OFF_NUM_REGS 2
static uint8 rfdevice_asm_rfmd1681_asm_off_regs[RFDEVICE_ASM_RFMD1681_ASM_OFF_NUM_REGS] =  {0x00, 0x01, };
static int16 rfdevice_asm_rfmd1681_asm_off_data[RFDEVICE_ASM_RFMD1681_NUM_PORTS][RFDEVICE_ASM_RFMD1681_ASM_OFF_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 1 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 2 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 3 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 4 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 5 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 6 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 7 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 8 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 9 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 10 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 11 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 12 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 13 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 14 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 15 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 16 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 17 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 18 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 19 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 20 */
    0x00, RFDEVICE_ASM_REG_INVALID, 
  },
  { /* PORT NUM: 21 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 22 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 23 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 24 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 25 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 26 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 27 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 28 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 29 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 30 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 31 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 32 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 33 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 34 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 35 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 36 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 37 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 38 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 39 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 40 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
  { /* PORT NUM: 41 */
    RFDEVICE_ASM_REG_INVALID, 0x00, 
  },
};


#define RFDEVICE_ASM_RFMD1681_ASM_TRIGGER_NUM_REGS 1
static uint8 rfdevice_asm_rfmd1681_asm_trigger_regs[RFDEVICE_ASM_RFMD1681_ASM_TRIGGER_NUM_REGS] =  {0x1C, };
static int16 rfdevice_asm_rfmd1681_asm_trigger_data[RFDEVICE_ASM_RFMD1681_NUM_PORTS][RFDEVICE_ASM_RFMD1681_ASM_TRIGGER_NUM_REGS] =
{
  { /* PORT NUM: 0 */
    0x01, 
  },
  { /* PORT NUM: 1 */
    0x01, 
  },
  { /* PORT NUM: 2 */
    0x01, 
  },
  { /* PORT NUM: 3 */
    0x01, 
  },
  { /* PORT NUM: 4 */
    0x01, 
  },
  { /* PORT NUM: 5 */
    0x01, 
  },
  { /* PORT NUM: 6 */
    0x01, 
  },
  { /* PORT NUM: 7 */
    0x01, 
  },
  { /* PORT NUM: 8 */
    0x01, 
  },
  { /* PORT NUM: 9 */
    0x01, 
  },
  { /* PORT NUM: 10 */
    0x01, 
  },
  { /* PORT NUM: 11 */
    0x01, 
  },
  { /* PORT NUM: 12 */
    0x01, 
  },
  { /* PORT NUM: 13 */
    0x01, 
  },
  { /* PORT NUM: 14 */
    0x01, 
  },
  { /* PORT NUM: 15 */
    0x01, 
  },
  { /* PORT NUM: 16 */
    0x01, 
  },
  { /* PORT NUM: 17 */
    0x01, 
  },
  { /* PORT NUM: 18 */
    0x01, 
  },
  { /* PORT NUM: 19 */
    0x01, 
  },
  { /* PORT NUM: 20 */
    0x01, 
  },
  { /* PORT NUM: 21 */
    0x01, 
  },
  { /* PORT NUM: 22 */
    0x01, 
  },
  { /* PORT NUM: 23 */
    0x01, 
  },
  { /* PORT NUM: 24 */
    0x01, 
  },
  { /* PORT NUM: 25 */
    0x01, 
  },
  { /* PORT NUM: 26 */
    0x01, 
  },
  { /* PORT NUM: 27 */
    0x01, 
  },
  { /* PORT NUM: 28 */
    0x01, 
  },
  { /* PORT NUM: 29 */
    0x01, 
  },
  { /* PORT NUM: 30 */
    0x01, 
  },
  { /* PORT NUM: 31 */
    0x01, 
  },
  { /* PORT NUM: 32 */
    0x01, 
  },
  { /* PORT NUM: 33 */
    0x01, 
  },
  { /* PORT NUM: 34 */
    0x01, 
  },
  { /* PORT NUM: 35 */
    0x01, 
  },
  { /* PORT NUM: 36 */
    0x01, 
  },
  { /* PORT NUM: 37 */
    0x01, 
  },
  { /* PORT NUM: 38 */
    0x01, 
  },
  { /* PORT NUM: 39 */
    0x01, 
  },
  { /* PORT NUM: 40 */
    0x01, 
  },
  { /* PORT NUM: 41 */
    0x01, 
  },
};

/* singleton instance ptr */
rfdevice_asm_data *rfdevice_asm_rfmd1681_data_ag::rfdevice_asm_rfmd1681_data_ptr = NULL;

rfdevice_asm_data * rfdevice_asm_rfmd1681_data_ag::get_instance()
{
  if (rfdevice_asm_rfmd1681_data_ptr == NULL)
  {
    rfdevice_asm_rfmd1681_data_ptr = (rfdevice_asm_data *)new rfdevice_asm_rfmd1681_data_ag();
  }
  return( (rfdevice_asm_data *)rfdevice_asm_rfmd1681_data_ptr);
}

//constructor
rfdevice_asm_rfmd1681_data_ag::rfdevice_asm_rfmd1681_data_ag()
  :rfdevice_asm_data()
{
}

//destructor
rfdevice_asm_rfmd1681_data_ag::~rfdevice_asm_rfmd1681_data_ag()
{
  rfdevice_asm_rfmd1681_data_ptr = NULL;
}

boolean rfdevice_asm_rfmd1681_data_ag::settings_data_get( rfdevice_asm_cfg_params_type *cfg, 
                                                          rfdevice_asm_reg_settings_type *settings)
{
  boolean ret_val = FALSE;

  if (NULL == settings || NULL == cfg)
  {
    return FALSE;
  }

  if (cfg->port >= RFDEVICE_ASM_RFMD1681_NUM_PORTS)
  {
    settings->addr = NULL;
    settings->data = NULL;
    settings->num_regs = 0;
    return FALSE;
  }

  if (cfg->req == RFDEVICE_ASM_ON_DATA)
  {
    settings->addr = &(rfdevice_asm_rfmd1681_asm_on_regs[0]);
    settings->data = &(rfdevice_asm_rfmd1681_asm_on_data[cfg->port][0]);
    settings->num_regs = RFDEVICE_ASM_RFMD1681_ASM_ON_NUM_REGS;
    ret_val = TRUE;
  }

  if (cfg->req == RFDEVICE_ASM_OFF_DATA)
  {
    settings->addr = &(rfdevice_asm_rfmd1681_asm_off_regs[0]);
    settings->data = &(rfdevice_asm_rfmd1681_asm_off_data[cfg->port][0]);
    settings->num_regs = RFDEVICE_ASM_RFMD1681_ASM_OFF_NUM_REGS;
    ret_val = TRUE;
  }

  if (cfg->req == RFDEVICE_ASM_TRIGGER_DATA)
  {
    settings->addr = &(rfdevice_asm_rfmd1681_asm_trigger_regs[0]);
    settings->data = &(rfdevice_asm_rfmd1681_asm_trigger_data[cfg->port][0]);
    settings->num_regs = RFDEVICE_ASM_RFMD1681_ASM_TRIGGER_NUM_REGS;
    ret_val = TRUE;
  }

  return ret_val;
}

boolean rfdevice_asm_rfmd1681_data_ag::sequence_data_get( rfdevice_asm_cfg_params_type *cfg, 
                                                          rfdevice_asm_cmd_seq_type *cmd_seq)
{
  boolean ret_val = FALSE;

  if (NULL == cmd_seq || NULL == cfg)
  {
    return FALSE;
  }

  else if (RFDEVICE_ASM_RESET_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else if (RFDEVICE_ASM_THERM_ON_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else if (RFDEVICE_ASM_THERM_READ_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else if (RFDEVICE_ASM_THERM_OFF_DATA == cfg->req)
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
    ret_val = TRUE;
  }
  else
  {
    cmd_seq->cmds = NULL;
    cmd_seq->num_cmds = 0;
  }

  return ret_val;
}

boolean rfdevice_asm_rfmd1681_data_ag::get_temperature_lut(int16 **lut_addr, uint8 *lut_size)
{
   boolean ret_val = FALSE;   

   if (lut_addr != NULL)  
   {
      *lut_addr = NULL; 
      *lut_size = -1;
      ret_val = TRUE;
   }

   else 
   {
      ret_val = FALSE;
   }

   return ret_val;
}

boolean rfdevice_asm_rfmd1681_data_ag::device_info_get( rfdevice_asm_info_type *asm_info )
{
  boolean ret_val = FALSE;

  if ( NULL == asm_info )
  {
    return FALSE;
  }
  else
  {
    asm_info->mfg_id = 0x134;
    asm_info->prd_id = 0x22;
    asm_info->prd_rev = 0;
    asm_info->num_ports = RFDEVICE_ASM_RFMD1681_NUM_PORTS;
    ret_val = TRUE;
  }
  return ret_val;
}

