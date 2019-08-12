#ifndef A2_PLATFORM_H
#define A2_PLATFORM_H
/*!
  @file a2_platform.h

  @brief
   A file defining platform specific parameters/functionality.

*/

/*==============================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_platform.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/26/13   yjz     Add A2_DAL_DBG_ERROR_IRQ
03/20/13   ars     CR465878: Added checksum offload feature 
08/08/12   ars     DIME PHY changes after RUMI validation 
04/11/12   bn      Added in support for DIME
10/06/11   ar      added a2 power collapse support
10/04/09   sm      Remove feature FEATURE_A2_VIRTIO and moved the macro to .c 
                   file
06/12/09   sah     Pull out unused interupt numbers.
05/14/09   yg      Split the A2 initialization for Espresso into Memory
                   Mapping section and A2 Init section.
05/14/09   sah     Add in some new espresso initialization, featurization,
                   and split init.
05/13/09   sah     Fix build break and featurize espresso code.
05/13/09   yg      Added stub function:a2_platform_espresso_init for
                   Espresso related A2 initialization.
03/02/09   sah     Initial Checkin

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/*==============================================================================

                                MACROS

==============================================================================*/


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @DAL Interrupt Vector Numbers */
#define A2_DAL_DBG_ERROR_IRQ        125  /* ERROR IRQ*/
#define A2_DAL_FRAG_MOD_IRQ         153  /* FRAG MODEM */
#define A2_DAL_FRAG_APP_IRQ         154  /* FRAG APP */
#define A2_DAL_TIMER_0_IRQ          174  /* DL PHY TIMER*/
#define A2_DAL_TIMER_1_IRQ          175  /* UL PER TIMER*/
#define A2_DAL_TIMER_2_IRQ          176  /* TBD. Not used */
#define A2_DAL_TIMER_3_IRQ          177  /* TBD. Not used */
#define A2_DAL_DECOB_IRQ            178  /* DL PHY for LTE case*/
#define A2_DAL_UL_PHY_IRQ           198  /* UL PHY */
#define A2_DAL_DL_PHY_IRQ           199  /* DL PHY for WCDMA case*/
#define A2_DAL_UL_SEC_IRQ           200  /* UL SEC */
#define A2_DAL_DL_PER_IRQ           201  /* DL PER */
#define A2_DAL_UL_PER_IRQ           202 
#define A2_DAL_UL_PER_FIRST_PKT_IRQ 138  /* UL PER First packet arrival */
#define A2_DAL_BAM_IRQ              0 /* TODO: Check what this is for */

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*===========================================================================

  FUNCTION:  a2_platform_init

===========================================================================*/
/*!
    @brief
    Initializes the A2 blocks for that specific platform

    @return
    None
*/
/*=========================================================================*/
void a2_platform_init( void );

/*===========================================================================

  FUNCTION:  a2_platform_complete_init

===========================================================================*/
/*!
    @brief
    Finishes initialization of the platform.

    @return
    None
*/
/*=========================================================================*/
void a2_platform_complete_init(void);

/*===========================================================================*/
/*!
    @brief
    Stop HW registers.

    @return
    None
*/
/*=========================================================================*/
void a2_platform_stop_hw_regs(void);

/*===========================================================================

  FUNCTION:  a2_platform_espresso_mem_map

===========================================================================*/
/*!
  @brief
  Memory mapping needed by A2 for Espresso platform.

*/
/*=========================================================================*/
void a2_platform_espresso_mem_map (void);

/*===========================================================================

  FUNCTION:  a2_platform_espresso_init

===========================================================================*/
/*!
  @brief
  A2 Initialization for Espresso platform.

*/
/*=========================================================================*/
void a2_platform_espresso_init (void);

/*===========================================================================

  FUNCTION:  a2_init

===========================================================================*/
/*!
    @brief
    Initializes the A2 driver

    This function does not start up the a2 task however.

*/
/*=========================================================================*/
void a2_init( void );

/*==============================================================================

  FUNCTION:  a2_deinit

==============================================================================*/
/*!
    @brief
    De-initializes the A2 driver

*/
/*============================================================================*/
void a2_deinit(void);

/*===========================================================================

  FUNCTION:  a2_platform_deregister_interrupts

===========================================================================*/
/*!
    @brief
    Deregisters all interrupts for a specific platform.

    @return
    None
*/
/*=========================================================================*/
void a2_platform_deregister_interrupts
(void);

#endif /* A2_PLATFORM_H */

