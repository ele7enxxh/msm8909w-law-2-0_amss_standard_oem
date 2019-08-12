/*============================================================================
  FILE:         uimdrv_rumi.c

  OVERVIEW:     File contains RUMI specific driver code.

  DEPENDENCIES: N/A

                Copyright (c) 2013QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/rumi/uimdrv_rumi.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

  when      who     what, where, why
------      ----    -----------------------------------------------------------
08/15/13    ak      Fix VSEL control functions for UIM2
07/09/13    ak      Created
============================================================================*/
#include "uimdrv_main.h"
#include "uim_v.h"
#include "DDITlmm.h"


#define UIM1_RUMI_V_ENABLE_GPIO_NUM                        67
#define UIM1_RUMI_V_SEL_GPIO_NUM                           65 
#define UIM2_RUMI_V_ENABLE_GPIO_NUM                        68
#define UIM2_RUMI_V_SEL_GPIO_NUM                           66


#define UIM1_RUMI_V_ENABLE_GPIO \
	    DAL_GPIO_CFG(UIM1_RUMI_V_ENABLE_GPIO_NUM, 0, (uint32)DAL_GPIO_OUTPUT, (uint32)DAL_GPIO_NO_PULL, (uint32)DAL_GPIO_2MA)   /* UIM1 VOLTAGE ENABLE   */

#define UIM1_RUMI_V_SEL_GPIO \
	    DAL_GPIO_CFG(UIM1_RUMI_V_SEL_GPIO_NUM, 0, (uint32)DAL_GPIO_OUTPUT, (uint32)DAL_GPIO_NO_PULL, (uint32)DAL_GPIO_2MA)      /* UIM1 VOLTAGE SELECT  */

#define UIM2_RUMI_V_ENABLE_GPIO \
	    DAL_GPIO_CFG(UIM2_RUMI_V_ENABLE_GPIO_NUM, 0, (uint32)DAL_GPIO_OUTPUT, (uint32)DAL_GPIO_NO_PULL, (uint32)DAL_GPIO_2MA)   /* UIM2 VOLTAGE ENABLE   */

#define UIM2_RUMI_V_SEL_GPIO \
	    DAL_GPIO_CFG(UIM2_RUMI_V_SEL_GPIO_NUM, 0, (uint32)DAL_GPIO_OUTPUT, (uint32)DAL_GPIO_NO_PULL, (uint32)DAL_GPIO_2MA)      /* UIM2 VOLTAGE SELECT  */

/*===========================================================================
FUNCTION uimdrv_rumi_config_gpios

DESCRIPTION
  CONFIG_VLTG_ENABLE_VSEL_SLOT configures the GPIOs connected to the V ENABLE and
  V SELECT lines.  For RUMI only.

===========================================================================*/
void uimdrv_rumi_config_gpios(uim_instance_enum_type uim_instance_id)
{
#ifdef T_RUMI_EMULATION
  if (UIM_INSTANCE_1 == uim_instance_id) 
  {
    if(NULL != uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr) 
    {
      /* Configure v_enable line */
      (void)DalTlmm_ConfigGpio( uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr,
                                (uint32)UIM1_RUMI_V_ENABLE_GPIO,
                                (uint32)DAL_TLMM_GPIO_ENABLE );
      /* Configure v_sel line */
      (void)DalTlmm_ConfigGpio( uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr,
                                (uint32)UIM1_RUMI_V_SEL_GPIO,
                                (uint32)DAL_TLMM_GPIO_ENABLE );
    }
  } 
  else 
  {
    if(NULL != uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr) 
    {
      /* Configure v_enable line */
      (void)DalTlmm_ConfigGpio( uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr,
                                (uint32)UIM2_RUMI_V_ENABLE_GPIO,
                                (uint32)DAL_TLMM_GPIO_ENABLE );
      /* Configure v_sel line */
      (void)DalTlmm_ConfigGpio( uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr,
                                (uint32)UIM2_RUMI_V_SEL_GPIO,
                                (uint32)DAL_TLMM_GPIO_ENABLE );
    }
  }
#endif /* T_RUMI_EMULATION */

  return;
}

/*===========================================================================
FUNCTION uimdrv_rumi_vsel_off

DESCRIPTION
  CONFIG_VSEL_OFF_SLOT disables the V ENABLE line and sets the V SELECT line
  to 1.8v.  For RUMI only.
===========================================================================*/
void uimdrv_rumi_vsel_off(uim_instance_enum_type uim_instance_id)
{
#ifdef T_RUMI_EMULATION
  /* Set v_enable line to low */
  if (UIM_INSTANCE_1 == uim_instance_id) 
  {
    if(NULL != uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr) 
    {
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM1_RUMI_V_ENABLE_GPIO, DAL_GPIO_LOW_VALUE);
      /* set v_sel line to 1.8 v */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM1_RUMI_V_SEL_GPIO, DAL_GPIO_LOW_VALUE);
    }
  } 
  else 
  {
    if(NULL != uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr) 
    {
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM2_RUMI_V_ENABLE_GPIO, DAL_GPIO_LOW_VALUE);
      /* set v_sel line to 1.8 v */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM2_RUMI_V_SEL_GPIO, DAL_GPIO_LOW_VALUE);
    }
  }
#endif /* T_RUMI_EMULATION */

  return;
}


/*===========================================================================
FUNCTION uimdrv_rumi_vsel_on_class_c

DESCRIPTION
  Turns on the V_ENABLE line and sets the V_SELECT line for 1.8v operation.
  For RUMI only.
===========================================================================*/
void uimdrv_rumi_vsel_on_class_c(uim_instance_enum_type uim_instance_id)
{
#ifdef T_RUMI_EMULATION
  if (UIM_INSTANCE_1 == uim_instance_id) 
  {
    if(NULL != uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr) 
    {
      /* Set v_enable line to high */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM1_RUMI_V_ENABLE_GPIO, DAL_GPIO_HIGH_VALUE);
      /* set v_sel line to 1.8 v */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM1_RUMI_V_SEL_GPIO, DAL_GPIO_LOW_VALUE);
    }
  } 
  else 
  {
    if(NULL != uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr) 
    {
      /* Set v_enable line to high */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM2_RUMI_V_ENABLE_GPIO, DAL_GPIO_HIGH_VALUE);
      /* set v_sel line to 3.0 v */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM2_RUMI_V_SEL_GPIO, DAL_GPIO_LOW_VALUE);
    }
  }
#endif /* T_RUMI_EMULATION */

  return;
}

/*===========================================================================
FUNCTION uimdrv_rumi_vsel_on_class_b

DESCRIPTION
  Turns on the V_ENABLE line and sets the V_SELECT line for 3v operation.
  For RUMI only.
===========================================================================*/
void uimdrv_rumi_vsel_on_class_b(uim_instance_enum_type uim_instance_id)
{
#ifdef T_RUMI_EMULATION
  if (UIM_INSTANCE_1 == uim_instance_id)
  {
    if(NULL != uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr) 
    {
      /* Set v_enable line to high */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM1_RUMI_V_ENABLE_GPIO, DAL_GPIO_HIGH_VALUE);
      /* set v_sel line to 1.8 v */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM1_RUMI_V_SEL_GPIO, DAL_GPIO_HIGH_VALUE);
    }
  } 
  else
  {
    if(NULL != uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr) 
    {
      /* Set v_enable line to high */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM2_RUMI_V_ENABLE_GPIO, DAL_GPIO_HIGH_VALUE);
      /* set v_sel line to 3.0 v */
      (void)DalTlmm_GpioOut(uim_hw_if.gpio[uim_instance_id].m_TlmmHandle_ptr, (uint32)UIM2_RUMI_V_SEL_GPIO, DAL_GPIO_HIGH_VALUE);
    }
  }
#endif /* T_RUMI_EMULATION */

  return;
}



