/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM GPIO   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM GPIO commands comming from the diag service 

Copyright (c) 2013  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_gpio.h#1 $ 
$DateTime: 2016/12/13 07:59:52 $ 
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/22/12   sr     Initial revision
===========================================================================*/

#ifndef FTM_GPIO_H
#define FTM_GPIO_H

#include "rfa_variation.h"
#ifdef FEATURE_FACTORY_TESTMODE

#include "comdef.h"
#include "target.h"
#include "ftm.h"
#include "ftm_rf_cmd.h"
#include "ftmdiag.h"
#include "diagpkt.h"

typedef enum{
   FTM_GPIO_CONFIG_CMD,
   FTM_GPIO_SET_GET_CMD,
   FTM_GRFC_SET_GET_CMD,
} ftm_gpio_cmd_types;


/******************************* GPIO CONFIG DATA STRUCTURE **************************/
typedef PACK(struct)
{
  uint8 gpio_num;
  uint8 func_sel;
  uint8 dir;
  uint8 pull_type;
  uint8 drv;
  uint8 enable;
} ftm_gpio_config_data_type;

/******************************* GPIO/GRFC SET DATA STRUCTURE **************************/
typedef PACK(struct)
{
  uint8 gpio_num;
  uint8 logic;
  uint8 set_flag;
  uint8 gpio_status;
} ftm_gpio_set_data_type;


/************************* FTM DATA STRUCTURE **************************/

/* hwtc gpio request/response type */
typedef PACK(struct)
{
  diagpkt_subsys_header_type  header;
  ftm_cmd_header_type         ftm_cmd_header;
  uint8                       gpio_cmd;
  PACK(union)
  {
    ftm_gpio_config_data_type   gpio_config;
    ftm_gpio_set_data_type      gpio_set;
  } ftm_gpio_params;  
} ftm_gpio_pkt_type;


ftm_rsp_pkt_type ftm_gpio_dispatch(ftm_gpio_pkt_type*);

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_HWTC_GPIO_H */
