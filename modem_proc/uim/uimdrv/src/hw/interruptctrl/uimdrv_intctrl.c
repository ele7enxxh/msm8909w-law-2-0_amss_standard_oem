/*============================================================================
  FILE:         uimdrv_intctrl.c

  OVERVIEW:     File contains the interrupt control functions.

  DEPENDENCIES: N/A

                Copyright (c) 2012-15 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/interruptctrl/uimdrv_intctrl.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       -----------------------------------------------------------
12/10/15    ks         8917 bringup changes
09/16/15    ks         8937 bringup changes
06/01/15    sam        9x07 bringup changes
11/11/14    ks         Using the DAL chip enums instad of hardcoding values
11/06/14    nmb        8952 bring up changes
09/23/14    ks         8909 bring up changes
09/08/14    ks         Enable hotswap feature if flaky mechanical switch is enabled
08/21/14    ak         Replace 9x45 DAL chip family reference for compilation on BOLT
08/20/14    nmb        9x45 Bring up changes
05/12/14    ak         8994 Bring up changes
05/06/14    ks         8936 bring up changes
02/18/14    sam        Usage of newer style diag macros
01/24/14    yk         Code Clean up
01/23/14    rm         Fix for 8916 to check for correct family
12/27/13    ks         8916 bring up changes
11/27/13    akv        Initialization of hw enumeration based on dal chip family
10/08/13    nmb        HW header file cleanup
09/11/13    na         8x62 bring up changes
08/06/13    rm/ak      Bolt RUMI code
07/12/13    akv        HW Enumeration - modifying target specific hardware values
                       and organizing them in value-key pairs
07/12/13    nmb        Global Reorganization
02/12/13    js         General F3 macro cleanup
12/05/12    js         UIM Parallel Processing changes
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uimdrv_intctrl.h"
#include "uimglobals.h"
#include "uim_hotswap.h"

#define INVALID_IRQ_NUM  0xFFFF

/* Table holding the default UIM Controller IRQ number for different UIM instances. */
const uim_controller_irq_num_table_type   uim_controller_irq_num_table[] = {

  /* ARAGORN_8974 */  { DALCHIPINFO_FAMILY_MSM8974,
                       /*  UIM1_IRQ_NUM,  UIM2_IRQ_NUM,  UIM3_IRQ_NUM,  UIM4_IRQ_NUM  */
                       { 89, 90, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* ARAGORN_8974 */

  /* ARAGORN_PRO */   { DALCHIPINFO_FAMILY_MSM8974_PRO,
                       { 89, 90, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* ARAGORN_PRO */

     /* ELAN_9X25 */  { DALCHIPINFO_FAMILY_MDM9x25,
                       { 89, 90, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* ELAN_9X25 */

     /* DINO_8X10 */  { DALCHIPINFO_FAMILY_MSM8x10,
                       { 89, 90, 254, INVALID_IRQ_NUM  }
                      },  /* DINO_8X10 */

     /* FRODO_8X26 */ { DALCHIPINFO_FAMILY_MSM8x26,
                       { 89, 90, 254, INVALID_IRQ_NUM  }
                      },  /* FRODO_8X26 */

    /* GIMLI_8926 */  { DALCHIPINFO_FAMILY_MSM8926,
                       { 89, 90, 254, INVALID_IRQ_NUM  }
                      },  /* GIMLI_8926 */

     /* TORINO_9X35 */{ DALCHIPINFO_FAMILY_MDM9x35,
                       { 89, 90, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* TORINO_9X35 */

     /* THORIN_8X62 */{ DALCHIPINFO_FAMILY_MSM8x62, 
                       { 89, 90, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* THORIN_8X62 */

  /* BAGHEERA_8916 */ { DALCHIPINFO_FAMILY_MSM8916,
                       { 89, 90, 254, INVALID_IRQ_NUM  }
                      },  /* BAGHEERA_8916 */

     /* SHERE_8936 */ { DALCHIPINFO_FAMILY_MSM8936,
                       { 89, 90, 254, INVALID_IRQ_NUM  }
                      },  /* SHERE_8936 */

    /* ELESSAR_8994 */{ DALCHIPINFO_FAMILY_MSM8994,
                       { 89, 90, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* ELESSAR_8994 */

     /* TESLA_9x45 */ { DALCHIPINFO_FAMILY_MDM9x45,
                       { 147, 148, 149, 150 }
                      },  /* TESLA_9x45 */

     /* CHEEL_8909 */ { DALCHIPINFO_FAMILY_MSM8909,
                       { 89, 90, 254, INVALID_IRQ_NUM  }
                      },  /* CHEEL_8909 */

   /* TABASCO_SAHI */ { /* DALCHIPINFO_FAMILY_MSM8952 */ 60,
                       { 89, 90, 254, INVALID_IRQ_NUM  }
                      },  /* TABASCO_SAHI_8952 */

     /* LYKAN_9x07 */ { DALCHIPINFO_FAMILY_MDM9x07,
                       { 89, 90, INVALID_IRQ_NUM, INVALID_IRQ_NUM  }
                      },  /* CHEEL_8909 */

     /* FEERO_8937 */ { DALCHIPINFO_FAMILY_MSM8937,
                       { 89, 90, INVALID_IRQ_NUM, INVALID_IRQ_NUM  }
                      },  /* FEERO_8937 */

 /* FEERO_LITE_8917 */{ DALCHIPINFO_FAMILY_MSM8917,
                       { 89, 90, INVALID_IRQ_NUM, INVALID_IRQ_NUM  }
                      },  /* FEERO_LITE_8917 */

     /* DALCHIPINFO_FAMILY_UNKNOWN */{ DALCHIPINFO_FAMILY_UNKNOWN, 
                       { INVALID_IRQ_NUM, INVALID_IRQ_NUM, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      }  /* DALCHIPINFO_FAMILY_UNKNOWN */
};

/* Table holding the default UARTDM IRQ number for different UIM instances. */
const uim_uartdm_irq_num_table_type         uim_uartdm_irq_num_table[]  = {

  /* ARAGORN_8974 */  { DALCHIPINFO_FAMILY_MSM8974,
                        /*  UIM1_IRQ_NUM,  UIM2_IRQ_NUM,  UIM3_IRQ_NUM,  UIM4_IRQ_NUM  */
                       { 87, 88, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* ARAGORN_8974 */

    /* ARAGORN_PRO */ { DALCHIPINFO_FAMILY_MSM8974_PRO,
                       { 87, 88, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* ARAGORN_PRO */

     /* ELAN_9X25 */  { DALCHIPINFO_FAMILY_MDM9x25,
                       { 87, 88, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* ELAN_9X25 */

     /* DINO_8X10 */  { DALCHIPINFO_FAMILY_MSM8x10,
                       { 87, 88, 253, INVALID_IRQ_NUM }
                      },  /* DINO_8X10 */

     /* FRODO_8X26 */ { DALCHIPINFO_FAMILY_MSM8x26,
                       { 87, 88, 253, INVALID_IRQ_NUM }
                      },  /* FRODO_8X26 */

     /* GIMLI_8926 */ { DALCHIPINFO_FAMILY_MSM8926,
                       { 87, 88, 253, INVALID_IRQ_NUM }
                      },  /* GIMLI_8926 */

     /* TORINO_9X35 */{ DALCHIPINFO_FAMILY_MDM9x35,
                       { 87, 88, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* TORINO_9X35 */

     /* THORIN_8X62 */{ DALCHIPINFO_FAMILY_MSM8x62,
                       { 87, 88, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* THORIN_8X62 */

  /* BAGHEERA_8916 */ { DALCHIPINFO_FAMILY_MSM8916,
                       { 87, 88, 253, INVALID_IRQ_NUM }
                      },  /* BAGHEERA_8916 */

     /* SHERE_8936 */ { DALCHIPINFO_FAMILY_MSM8936,
                       { 87, 88, 253, INVALID_IRQ_NUM }
                      },  /* SHERE_8936 */

    /* ELESSAR_8994 */{ DALCHIPINFO_FAMILY_MSM8994,
                       { 87, 88, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* ELESSAR_8994 */

     /* TESLA_9x45 */ { DALCHIPINFO_FAMILY_MDM9x45,
                       { 151, 152, 153, 154 }
                      },  /* TESLA_9x45 */

     /* CHEEL_8909 */ { DALCHIPINFO_FAMILY_MSM8909,
                       { 87, 88, 253, INVALID_IRQ_NUM }
                      },  /* CHEEL_8909 */

   /* TABASCO_SAHI */ { /* DALCHIPINFO_FAMILY_MSM8952 */ 60,
                       { 87, 88, 253, INVALID_IRQ_NUM }
                      },  /* TABASCO_SAHI */

     /* LYKAN_9x07 */ { DALCHIPINFO_FAMILY_MDM9x07,
                       { 87, 88, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* CHEEL_8909 */

     /* FEERO_8937 */ { DALCHIPINFO_FAMILY_MSM8937,
                       { 87, 88, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* FEERO_8937 */

 /* FEERO_LITE_8917 */ { DALCHIPINFO_FAMILY_MSM8917,
                       { 87, 88, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      },  /* FEERO_LITE_8917 */

     /* DALCHIPINFO_FAMILY_UNKNOWN */{ DALCHIPINFO_FAMILY_UNKNOWN, 
                       { INVALID_IRQ_NUM, INVALID_IRQ_NUM, INVALID_IRQ_NUM, INVALID_IRQ_NUM }
                      }  /* DALCHIPINFO_FAMILY_UNKNOWN */
};


void uimIntctrlIsr (uint32 *context )
{
  uim_rx_isr((uim_instance_global_type*)context);
} /* uimIntctrlIsr */


void uimUimctrlIsr (uint32 *context )
{
  uim_hotswap_gpio_isr((uim_instance_global_type*)context);
} /* uimUimctrlIsr*/


/* Initialize interrupt controller */
void initialize_intctrl(uim_instance_global_type *uim_ptr)
{
  DALResult dal_attach                                  = DAL_SUCCESS;
  uim_instance_enum_type uim_instance                   = uim_ptr->id;
  uim_hw_if.intctrl[uim_instance].client_callback_param = (uint32)uim_ptr;
  uim_hw_if.intctrl[uim_instance].m_UimCtrl_handle_ptr  = NULL;
  uim_hw_if.intctrl[uim_instance].m_IntCtrl_handle_ptr  = NULL;
  uim_hw_if.intctrl[uim_instance].m_hs_enable = FALSE;

  uim_hw_if.intctrl[uim_instance].m_hs_enable           = m_HWConfig.uim_slot_config[uim_instance].enableUimHotswap;

  if( (FALSE == uim_hw_if.intctrl[uim_instance].m_hs_enable) && 
      (TRUE == uim_ptr->hotswap.hotswap_flaky_switch_support))
  {
    /* Enable hotswap feature (even NV says it is disabled) 
       if the flaky mechanical switch feature is enabled */
    uim_hw_if.intctrl[uim_instance].m_hs_enable = TRUE;  
  }
  
  uim_hw_if.intctrl[uim_instance].m_hs_polarity         = m_HWConfig.uim_slot_config[uim_instance].uimHotswapPolarity;
  uim_hw_if.intctrl[uim_instance].m_uartIRQNum          = m_HWConfig.uim_slot_config[uim_instance].uimUartIRQNum;
  uim_hw_if.intctrl[uim_instance].m_uimControllerIRQNum = m_HWConfig.uim_slot_config[uim_instance].uimControllerIRQNum;


  dal_attach = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &(uim_hw_if.intctrl[uim_instance].m_IntCtrl_handle_ptr) );
  if((!uim_hw_if.intctrl[uim_instance].m_IntCtrl_handle_ptr)||(dal_attach != DAL_SUCCESS))
  {
    UIMDRV_MSG_ERR_0(uim_instance,"Failed to register with Interrupt Controller");
    return;
  }
  (void) DalDevice_Open(uim_hw_if.intctrl[uim_instance].m_IntCtrl_handle_ptr,DAL_OPEN_SHARED);
  DalInterruptController_RegisterISR(uim_hw_if.intctrl[uim_instance].m_IntCtrl_handle_ptr,
                                     uim_hw_if.intctrl[uim_instance].m_uartIRQNum,
                                     (DALISR) uimIntctrlIsr,
                                     (const DALIRQCtx) uim_ptr,
                                     DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);
  if (TRUE == uim_hw_if.intctrl[uim_instance].m_hs_enable)
  {
    dal_attach = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &(uim_hw_if.intctrl[uim_instance].m_UimCtrl_handle_ptr));
    if((!uim_hw_if.intctrl[uim_instance].m_UimCtrl_handle_ptr)||(dal_attach != DAL_SUCCESS))
    {
      UIMDRV_MSG_ERR_0(uim_instance,"Failed to register with UIM Interrupt Controller");
      uim_hw_if.intctrl[uim_instance].m_hs_enable = FALSE;
      return;
    }
    (void) DalDevice_Open(uim_hw_if.intctrl[uim_instance].m_UimCtrl_handle_ptr,DAL_OPEN_SHARED);
    DalInterruptController_RegisterISR((DalDeviceHandle*)uim_hw_if.intctrl[uim_instance].m_UimCtrl_handle_ptr,
                                      (DALInterruptID) uim_hw_if.intctrl[uim_instance].m_uimControllerIRQNum,
                                      (DALISR) uimUimctrlIsr,
                                      (const DALIRQCtx) uim_ptr,
                                       DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);

  }
} /* initialize_intctrl */

