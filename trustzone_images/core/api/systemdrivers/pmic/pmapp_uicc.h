#ifndef __PMAPP_UICC_H__
#define __PMAPP_UICC_H__
/*===========================================================================


               P M A P P_ U I C C  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations to support the HOTSWAP (UICC) feature inside the Qualcomm 
  PMIC chips.
  
Copyright (c) 2011            by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pmapp_uicc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/14/11   dy      Move UICC resource to pm_external_resources.h
07/14/11   dy      Rename APIs
05/27/11   vk      Created.
===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/**
     PMIC driver event codes

     This list specifies all the event types that are
     supported by the PMIC driver
*/
typedef enum
{
    PM_UICC_CARD_INSERTED,
    PM_UICC_CARD_REMOVED,
    PM_UICC_CARD_STATE_INVALID
}pm_uicc_card_detect_status;

/*===========================================================================

                 UICC DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/*=========================================================================== */
/*                        pmapp_uicc_query_status                             */
/*=========================================================================== */

/**
  This function is used to query the real-time status of the UICC card (inserted/removed)
  This function will be called by the UIM driver during bootup to detect which of the two slots
  is present/not present. Also, used in the UIM callback function as part of the SW debounce logic

  PARAMETERS
  UICC ID either PM_UICC_1 or PM_UICC_2 for the two slots  

  @return      Real time status of the UICC slot either inserted/removed

*/
pm_err_flag_type pmapp_uicc_query_status(int uicc_id, pm_uicc_card_detect_status* uicc_state );

/*=========================================================================== */
/*                      pmapp_uicc_unmask_interrupt                           */
/*=========================================================================== */

/**
  This function is used by the UIM driver to unmask the UICC interrupt once their SW debounce 
  logic is complete.
  PARAMETERS
  UICC ID either PM_UICC_1 or PM_UICC_2 for the two slots   

  @return      Boolean - TRUE or FALSE to indicate if the unmasking is successful or not

*/
pm_err_flag_type pmapp_uicc_unmask_interrupt (int uicc_id, boolean* status);

/*=========================================================================== */
/*                     pmapp_uicc_register_callback                           */
/*=========================================================================== */

/**
  This function is for registering the UICC callback that needs to be called inside the
  corresponding ISR for a UICC slot. 

  PARAMETERS
  UICC ID either PM_UICC_1 or PM_UICC_2 for the two slots 
  The registered callback function.  One of the parameters to the
  callback funtion tells whether the UICC card is inserted or not

  @return      Boolean - TRUE or FALSE to indicate if the cb registration is successful or not

*/

pm_err_flag_type pmapp_uicc_register_callback( int uicc_id, void (*pm_uicc_cb) (pm_uicc_card_detect_status));

#endif /* __PMAPP_UICC_H__ */
