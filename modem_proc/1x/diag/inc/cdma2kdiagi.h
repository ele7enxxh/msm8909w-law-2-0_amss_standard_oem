#ifndef CDMA2KDIAGI_H
#define CDMA2KDIAGI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C D M A   2 K   D I A G   I N T E R N A L   H E A D E R

GENERAL DESCRIPTION
  This module contains declarations pertaining to the cdma2k diag interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2009 - 2013 Qualcomm Technologies, Inc. 
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

$Header $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/10   vks     Remove Genesis revisit feature as DAL support is now
                   available on MDM9K.
08/03/09   adw     Initial revision.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

/* Other */
#include "DDISoftwareSwitches.h"



/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* Dipswitch mapping */
typedef enum
{
  /* Enable IF loopback */              
  CDMA2KDIAG_SW_ENABLE_IF_LOOPBACK      = DAL_SOFTWARE_SWITCH_1,

  /* Reload reverse link long code each 80-ms frame */
  CDMA2KDIAG_SW_RELOAD_LONG_CODE        = DAL_SOFTWARE_SWITCH_2,

  /* Disable forward link scrambling */
  CDMA2KDIAG_SW_DISABLE_FL_SCRAMBLE     = DAL_SOFTWARE_SWITCH_3,

  /* Disable forward link puncturing */
  CDMA2KDIAG_SW_DISABLE_FL_PUNCTURE     = DAL_SOFTWARE_SWITCH_4,

  /* Disable reverse link power control */
  CDMA2KDIAG_SW_DISABLE_RL_PWR_CTRL     = DAL_SOFTWARE_SWITCH_5,

  /* Enable Tx power on during acquisition and sync channel */
  CDMA2KDIAG_SW_ENABLE_TX_PWR_ACQ_SYNC  = DAL_SOFTWARE_SWITCH_6,

  /* Enable sounder mode */
  CDMA2KDIAG_SW_ENABLE_SOUNDER_MODE     = DAL_SOFTWARE_SWITCH_7,

  /* Force reverse link voice to full rate */
  CDMA2KDIAG_SW_FORCE_RL_FULL_RATE      = DAL_SOFTWARE_SWITCH_8,

  /* Disable sleep mode operation */
  CDMA2KDIAG_SW_DISABLE_SLEEP           = DAL_SOFTWARE_SWITCH_9,

  /* Supress vocoder and audio filters */
  CDMA2KDIAG_SW_DISABLE_VOCODER         = DAL_SOFTWARE_SWITCH_10,

  /* Disable reverse link max power protection */
  CDMA2KDIAG_SW_DISABLE_RL_MAX_PWR_PROT = DAL_SOFTWARE_SWITCH_11,

  /* Disable other frequency searching */
  CDMA2KDIAG_SW_DISABLE_OFREQ_SRCH      = DAL_SOFTWARE_SWITCH_12,

  /* Disable quick paging channel */
  CDMA2KDIAG_SW_DISABLE_QPCH            = DAL_SOFTWARE_SWITCH_13,

  /* Disable quick paging channel estimator */
  CDMA2KDIAG_SW_DISABLE_CH_EST          = DAL_SOFTWARE_SWITCH_14,

  /* Disable quick paging channel offline timeline */
  CDMA2KDIAG_SW_DISABLE_QPCH_OFFTL      = DAL_SOFTWARE_SWITCH_15,

  /* Enable acquisition testing */
  CDMA2KDIAG_SW_ENABLE_ACQ_TEST         = DAL_SOFTWARE_SWITCH_16
}
cdma2kdiag_dip_sw_type;



/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       CDMA2KDIAG_SWITCH_INIT

DESCRIPTION    This function initializes the global cdma2kdiag switches 
               DAL handle and must be called before any other switch
               function.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Initializes the 1x handle into the switches DAL.

===========================================================================*/
void cdma2kdiag_switch_init( void );

/*===========================================================================

FUNCTION       CDMA2KDIAG_GET_SWITCH

DESCRIPTION    This function returns the state of a dipswitch.

DEPENDENCIES   The cdma2kdiag_switch_init() routine must be called
               prior to invoking this function.

RETURN VALUE   Returns the state of the dipswitch. The Switches DAL returns
               a value of 0 when a dipswitch is set. This function translates
               the DAL API value into a boolean.

               TRUE - the dipswitch is set.

SIDE EFFECTS   None.

===========================================================================*/
boolean cdma2kdiag_get_switch
(
  /* Dip switch to check */
  cdma2kdiag_dip_sw_type dip_sw
);

/*===========================================================================

FUNCTION       CDMA2KDIAG_SET_SWITCH

DESCRIPTION    This function sets the state of a dipswitch.

DEPENDENCIES   The cdma2kdiag_switch_init() routine must be called prior
               to invoking this function.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void cdma2kdiag_set_switch
(
  /* Dip switch to set */
  cdma2kdiag_dip_sw_type dip_sw
);

/*===========================================================================

FUNCTION       CDMA2KDIAG_CLEAR_SWITCH

DESCRIPTION    This function clears the state of a dipswitch.

DEPENDENCIES   The cdma2kdiag_switch_init() routine must be called prior
               to invoking this function.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void cdma2kdiag_clear_switch
(
  /* Dip switch to set */
  cdma2kdiag_dip_sw_type dip_sw
);

#endif /* CDMA2KDIAGI_H */
