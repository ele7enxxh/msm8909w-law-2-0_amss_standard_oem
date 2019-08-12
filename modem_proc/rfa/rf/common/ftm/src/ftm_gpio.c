/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM GPIO COMMANDS

GENERAL DESCRIPTION
  This is the FTM GPIO file which contains GPIO READ or WRITE commands

Copyright (c) 2013   by Qualcomm Technologies, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_gpio.c#1 $
  $DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/23/13   sr     KW errors fix
02/22/13   sr     Initial revision
===========================================================================*/
#include "rfa_variation.h"
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_gpio.h"    
#include "rf_hal_grfc.h"
#include "DDITlmm.h"
#include "msg.h"                     /* Debug related stuff                         */     

#define FTM_RF_GPIO_DAL_SIG_MAX_NUM 20

extern void rfhal_cmd_intf_profile(void);
extern DalDeviceHandle * rf_tlmm_dal_handle;

typedef struct 
{
  uint16 gpio_num;
  DALGpioSignalType dal_signal;
} ftm_gpio_signal_type;

static ftm_gpio_signal_type ftm_gpio[FTM_RF_GPIO_DAL_SIG_MAX_NUM + 1] = {{0}};

/*===========================================================================
                Define LOCAL variables  
===========================================================================*/

static void ftm_gpio_config_cmd(ftm_gpio_config_data_type* gpio_config_data)
{
  DALResult result = DAL_SUCCESS;
  uint8 i = 0;

  if (gpio_config_data == NULL) /* check for valid input parameters */
  {
    return;
  }

  while (i < FTM_RF_GPIO_DAL_SIG_MAX_NUM)
  {
    /* if we already have the dal signal for the GPIO or if this is new one */
    if ((ftm_gpio[i].gpio_num == gpio_config_data->gpio_num) || 
        (ftm_gpio[i].gpio_num == 0))
    {
      break;
    }
    i++;
  }

  /* construct the DAL Signal */
  ftm_gpio[i].gpio_num = gpio_config_data->gpio_num;
  ftm_gpio[i].dal_signal = DAL_GPIO_CFG(gpio_config_data->gpio_num, \
                                        gpio_config_data->func_sel, \
                                        gpio_config_data->dir, \
                                        gpio_config_data->pull_type, \
                                        gpio_config_data->drv);


  if (gpio_config_data->enable == 1)
  {
    /* Enable the DAL Signal  */
    result = DalTlmm_ConfigGpio(rf_tlmm_dal_handle, ftm_gpio[i].dal_signal, DAL_TLMM_GPIO_ENABLE);
  }
  else
  {
    /* Enable the DAL Signal  */
    result = DalTlmm_ConfigGpio(rf_tlmm_dal_handle, ftm_gpio[i].dal_signal, DAL_TLMM_GPIO_DISABLE);
  }

  if (result != DAL_SUCCESS)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"ftm_gpio_config_cmd(%d, %d) failed!, result = %d", 
          gpio_config_data->gpio_num, gpio_config_data->enable, result);
  }
    
}

static void ftm_grfc_set_get_cmd(ftm_gpio_set_data_type* gpio_data)
{
  rf_hal_bus_result_type result;
  boolean grfc_state = FALSE;

  if (gpio_data == NULL) /* check for valid input parameters */
  {
    return;
  }

  if (gpio_data->logic == 0)
  {
    grfc_state = FALSE;
  }
  else
  {
    grfc_state = TRUE;
  }

  if (gpio_data->set_flag == 1)
  {
    result = rfhal_grfc_write_single(gpio_data->gpio_num, grfc_state, NULL);
    result &= rfhal_grfc_read_single(gpio_data->gpio_num, &grfc_state, NULL);
    gpio_data->gpio_status = grfc_state;
  }
  else
  {
    result = rfhal_grfc_read_single(gpio_data->gpio_num, &grfc_state, NULL);
    gpio_data->gpio_status = grfc_state;
  }

  if (result != RF_HAL_BUS_SUCCESS)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"ftm_grfc_set_get_cmd(%d, %d) failed!, result = %d", 
          gpio_data->gpio_num, gpio_data->set_flag, result);
  }
    
}


static void ftm_gpio_set_get_cmd(ftm_gpio_set_data_type* gpio_data)
{
  DALResult result = DAL_ERROR;
  DALGpioSignalType dal_sig = 0x0;
  DALGpioValueType gpio_logic = DAL_GPIO_LOW_VALUE;
  DALGpioValueType gpio_state = DAL_GPIO_LOW_VALUE;
  uint8 i = 0;

  if (gpio_data == NULL) /* check for valid input parameters */
  {
    return;
  }

  /* find the  DAL Signal for the GPIO */
  while (i < FTM_RF_GPIO_DAL_SIG_MAX_NUM)
  {
    if (ftm_gpio[i].gpio_num == gpio_data->gpio_num)
    {
      dal_sig = ftm_gpio[i].dal_signal;
      break;
    }

    i++;
  }

  if (gpio_data->logic == 0)
  {
    gpio_logic = DAL_GPIO_LOW_VALUE;
  }
  else
  {
    gpio_logic = DAL_GPIO_HIGH_VALUE;
  }

  if (dal_sig != 0x0) /* if there valid DAL Signal for the GPIO */
  {
    if (gpio_data->set_flag == 1)
    {
      result = DalTlmm_GpioOut(rf_tlmm_dal_handle, dal_sig, gpio_logic);
      result &= DalTlmm_GetOutput(rf_tlmm_dal_handle, gpio_data->gpio_num, &gpio_state);
      gpio_data->gpio_status = gpio_state;
    }
    else
    {
      result = DalTlmm_GetOutput(rf_tlmm_dal_handle, gpio_data->gpio_num, &gpio_state);
      gpio_data->gpio_status = gpio_state;
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"ftm_gpio_set_get_cmd(%d, 0x%x) failed! GPIO is NOT Configured", 
          gpio_data->gpio_num, dal_sig);
  }

  if (result != DAL_SUCCESS)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"ftm_gpio_set_get_cmd(%d, %d) failed!, result = %d", 
          gpio_data->gpio_num, gpio_data->logic, result);
  }
    
}


/*===========================================================================

FUNCTION FTM_GPIO_DISPATCH

DESCRIPTION
   This function performs GPIO dispatch operation.  
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_gpio_dispatch(ftm_gpio_pkt_type* gpio)
{
  ftm_rsp_pkt_type               rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  if (gpio == NULL) /* check for valid input parameters */
  {
    return rsp_pkt;
  }
  
  switch (gpio->gpio_cmd)
  {
  case FTM_GPIO_CONFIG_CMD:
    ftm_gpio_config_cmd(&gpio->ftm_gpio_params.gpio_config);
    break;

  case FTM_GPIO_SET_GET_CMD:
    ftm_gpio_set_get_cmd(&gpio->ftm_gpio_params.gpio_set);
    break;

  case FTM_GRFC_SET_GET_CMD:
    ftm_grfc_set_get_cmd(&gpio->ftm_gpio_params.gpio_set);
    break;
    
  default:
    MSG_HIGH("FTM GPIO:unknown command", 0, 0, 0);
    break;
  }

  return rsp_pkt;
}


#endif /* FEATURE_FACTORY_TESTMOD0E */
