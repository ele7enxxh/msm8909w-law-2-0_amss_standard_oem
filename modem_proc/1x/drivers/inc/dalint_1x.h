#ifndef __DALINT_1X_H
#define __DALINT_1X_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       DEVICE ACCESS LIBRARY INTERRUPT CONTROLLER - H E A D E R   F I L E

GENERAL DESCRIPTION
  This is the header file containing the declarations and typedefs used by
  dalint_1x.c file. This file should be included for any DAL interrupt
  related operations.

EXTERNALIZED FUNCTIONS
  dalint_1x_intr_reg - This function registers the DAL Interrupt Controller
  dalint_1x_intr_dereg - This function deregisters the DAL Interrupt Controller
  dalint_1x_intr_dereg_all - This function deregisters all the IRQs for a
           particular client
  dalint_1x_irq_reg_by_client - This function tells if the client uses a
        particular IRQ or not.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 2012 - 2014 Qualcomm Technologies, Inc.
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/drivers/inc/dalint_1x.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/14   srk     Remove the unused Quick paging ISR.
03/03/14   srk     Dynamically specify stack size for ISRs.
10/15/13   cjb     Removed unused IRQs and related cleanup
10/10/13   cjb     DEMBACK0 vs DEMBACK1: Use DEMBACK ID from FW shared memory
04/02/13   pap     Feature cleanup
02/16/12   srk     Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

/* 1X common */
#include "modem_1x_utils.h"

/* DALInterruptController */
#include "DalDevice.h"            /* DAL_OPEN_SHARED */
#include "DALDeviceId.h"
#include "DALSys.h"               /* DAL_DeviceAttach() */
#include "DALStdErr.h"            /* DAL_SUCCESS */
#include "DDIInterruptController.h"

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* Callback Function */


typedef const DALISR dalint_1x_isr;

/* Client ID */
typedef enum
{
  DALINT_1X_ATMR = 0,
  DALINT_1X_RX,
  DALINT_1X_TX,
  DALINT_1X_SRCH,
  /*
  Add a new client here.
  Whenever a new client is added here, its corresponding entry
  should be initialized in the dalint_1x.c file.
  */
  DALINT_1X_MAX_CLIENTS
}dalint_1x_client_name;

/* IRQ VECTOR */
typedef enum
{
  DALINT_1X_FIRST_IRQ = 0,    /* First IRQ */

  /* IRQ list */
  DALINT_1X_RX_DB0_ISR = DALINT_1X_FIRST_IRQ,
  DALINT_1X_RX_DB1_ISR,
  /*
  Add a new IRQ here.
  Whenever a new IRQ is added here, its corresponding entry
  should be initialized in the dalint_1x.c file.
  */
  DALINT_1X_MAX_IRQ
}dalint_1x_irq_types;

/* Stack Size */
typedef enum
{
  DALINT_1X_STACK_2K = 0,
  DALINT_1X_STACK_4K,
  DALINT_1X_MAX_STACK = DALINT_1X_STACK_4K
}dalint_1x_stack_size;

/*===========================================================================

                    FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION       DALINT_1X_INTR_REG

DESCRIPTION    This function registers the DAL Interrupt Controller

DEPENDENCIES   None

RETURN VALUE   boolean

SIDE EFFECTS   None

===========================================================================*/
extern boolean dalint_1x_intr_reg
(
  dalint_1x_client_name client_id,    /* Client ID */
  dalint_1x_irq_types   irq_vector,   /* IRQ VECTOR */
  dalint_1x_isr         isr_callback,   /* Callback Function */
  dalint_1x_stack_size  stack_size      /* Stack size for the task */
);

/*===========================================================================

FUNCTION       DALINT_1X_INTR_DEREG

DESCRIPTION    This function deregisters the DAL Interrupt Controller

DEPENDENCIES   None

RETURN VALUE   boolean

SIDE EFFECTS   None

===========================================================================*/
extern boolean dalint_1x_intr_dereg
(
  dalint_1x_client_name client_id,    /* Client ID */
  dalint_1x_irq_types   irq_vector    /* IRQ VECTOR */
);

/*===========================================================================

FUNCTION       DALINT_1X_INTR_DEREG_ALL

DESCRIPTION    This function deregisters all the IRQs for a particular client

DEPENDENCIES   None

RETURN VALUE   boolean

SIDE EFFECTS   None

===========================================================================*/
extern boolean dalint_1x_intr_dereg_all
(
  dalint_1x_client_name client_id     /* Client ID */
);


/*===========================================================================

FUNCTION       DALINT_1X_IRQ_REG_BY_CLIENT

DESCRIPTION    This function tells if the client uses a particular IRQ or not.

DEPENDENCIES   None

RETURN VALUE   boolean

SIDE EFFECTS   None

===========================================================================*/
extern boolean dalint_1x_irq_reg_by_client
(
  dalint_1x_client_name client_id,    /* Client ID */
  dalint_1x_irq_types   irq_vector    /* IRQ VECTOR */
);

/*===========================================================================

FUNCTION       DALINT_1X_IRQ_PENDING

DESCRIPTION    This function returns whether an IRQ is pending or not.

DEPENDENCIES   Client should have registered for the IRQ

RETURN VALUE   boolean

SIDE EFFECTS   None

===========================================================================*/
extern boolean dalint_1x_irq_pending
(
  dalint_1x_client_name client_id,    /* Client ID */
  dalint_1x_irq_types   irq_vector     /* IRQ VECTOR */
);

#endif /* __DALINT_1X_H */
