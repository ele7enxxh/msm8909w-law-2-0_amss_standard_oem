#ifndef RFGSMCUST_H
#define RFGSMCUST_H

/*==========================================================================

          R F   C U S T O M E  R   D R I V E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for the RF Customer
  Driver

Copyright (c) 2001 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsmcust.h#1 $ $DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/03   aks     Bring the LDO control out of the Library.  Functions are
                   called going into RX Sleep and coming out of RX Sleep.
                   Allows selection of LDOs.
12/11/02    eh     Added semicolon after rfgsmcust_init() declaration.
09/13/02   thh     Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

FUNCTION rfgsmcust_init

DESCRIPTION
  This functions initializes the RF HW.

DEPENDENCIES
  This function is invoked after the GSM RF library has finished with its own
  initializations.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rfgsmcust_init( void );


/*===========================================================================

FUNCTION rfgsmcust_ldo_shutdown

DESCRIPTION
  This function is called by the Library during the disabling of the RX
  power, while in GSM mode.  This allows control of which LDOs are
  actually shutdown.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rfgsmcust_ldo_shutdown( void );


/*===========================================================================

FUNCTION rfgsmcust_ldo_powerup

DESCRIPTION
  This function is called, by the Library, during the enabling of the RX
  power.  This allows control of which LDOs are turned on during the enable
  of RX, while in GSM mode.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rfgsmcust_ldo_powerup( void );


#endif /* RFGSMCUST_H */
