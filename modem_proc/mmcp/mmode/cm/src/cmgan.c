
/*===========================================================================

    C A L L   M A N A G E R   G A N  C O N T R O L L E R   I N T E R F A C E

                        H E A D E R   F I L E

DESCRIPTION

   This header describes functions that interface the GAN controller to
   Call Manager. The GAN UI (GAN controller's interface to CM) needs to send
   reports to CM which has to be published as serving system events to UI and 
   other interested CM clients. 

  Copyright (c) 2009 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmgan.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
01/27/09   aj      Initial release.
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#include "cmgan.h"   /* Interface header file for applications */
#include "cmdbg.h"     /* Interface to CM debug services */

/*===========================================================================

FUNCTION cmgan_rpt_ind

DESCRIPTION
  Allows GAN controller interface to CM (gan_ui) to queue up messages to
  the CM. All this will do is queue it and set a signal for the CM task. 

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
/*lint -esym(715,gan_rpt) */
void cmgan_rpt_ind (

  const cmgan_gw_gan_rpt_s_type gan_rpt
    /* GAN report with 
    ** GAN serving system info 
    */
 
)
{

} /* cmgan_rpt_ind */
/*lint +esym(715,gan_rpt) */

