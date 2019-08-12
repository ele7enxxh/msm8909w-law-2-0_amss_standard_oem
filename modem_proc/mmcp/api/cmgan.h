#ifndef CMGAN_H
#define CMGAN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cmgan.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/09   rm      Removing inclusion of customer.h from /api files
01/27/09   aj   Initial release.
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/


#include "cm.h"

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/
/** Enumeration of UMA-GAN reports
*/
typedef enum cmgan_gw_gan_rpt_e{

  CMGAN_GW_GAN_RPT_NONE = -1,              
  /**< @internal */

  /* 0 */
  CMGAN_GW_GAN_RPT_INIT,               
  /**< indicate Kineto Client initialization */

  CMGAN_GW_GAN_RPT_CONN_STATUS,   
  /**< indicate UMA GAN connection status */

  CMGAN_GW_GAN_RPT_SIG_CHAN_STATUS,        
  /**< indicate signal channel status */

  CMGAN_GW_GAN_RPT_MODE_DISPLAY_ON,      
  /**< indicate WLAN becomes the active RAT */

  /* 4 */
  CMGAN_GW_GAN_RPT_MODE_DISPLAY_OFF,     
  /**< indicate GERAN becomes the active RAT */

  CMGAN_GW_GAN_RPT_ERROR_PROMPT,        
  /**< indicate error category */

  CMGAN_GW_GAN_RPT_SHUT_DOWN,           
  /**< indicate GAN UI module should be shutdown */

  CMGAN_GW_GAN_RPT_MAX
  /**< @internal */

} cmgan_gw_gan_rpt_e_type;

/**--------------------------------------------------------------------------
** Datatypes - Report structures
** --------------------------------------------------------------------------
*/

/* Union of all GAN report types.
*/
typedef union cmgan_rpt_u{

    sys_gw_gan_conn_status_e_type                 gan_conn_status;
     /* GAN connection status */

    sys_gw_gan_sig_chan_status_e_type             gan_sig_chan_status;
     /* GAN signal channel status */

    sys_gw_gan_mode_display_s_type                gan_mode_display;
     /* GAN report type */

    sys_gw_gan_error_s_type                       gan_error;
     /* GAN error */

}cmgan_rpt_u_type;

/*~ CASE CMGAN_GW_GAN_RPT_INIT  cmgan_rpt_u.void */
/*~ CASE CMGAN_GW_GAN_RPT_CONN_STATUS  cmgan_rpt_u.gan_conn_status */
/*~ CASE CMGAN_GW_GAN_RPT_SIG_CHAN_STATUS cmgan_rpt_u.gan_sig_chan_status */
/*~ CASE CMGAN_GW_GAN_RPT_MODE_DISPLAY_ON  cmgan_rpt_u.gan_mode_display */
/*~ CASE CMGAN_GW_GAN_RPT_MODE_DISPLAY_OFF  cmgan_rpt_u.gan_mode_display */
/*~ CASE CMGAN_GW_GAN_RPT_ERROR_PROMPT  cmgan_rpt_u.gan_error */
/*~ CASE CMGAN_GW_GAN_RPT_SHUT_DOWN  cmgan_rpt_u.void  */


/* Type for GAN report. The header is always present and it
** specifies the report type and attributes. If the command has
** arguments they follow in the union.
*/
typedef struct cmgan_gw_gan_rpt_s{

  cmgan_gw_gan_rpt_e_type   gan_rpt_name;
    /* Name of report being queued to CM
    */

  cmgan_rpt_u_type gan_rpt;


}cmgan_gw_gan_rpt_s_type;

/*~ FIELD cmgan_gw_gan_rpt_s.gan_rpt DISC cmgan_gw_gan_rpt_s.gan_rpt_name */


/*===========================================================================

FUNCTION cmgan_rpt_ind

DESCRIPTION
  Allows GAN controller interface to CM (gan_ui) to queue up messages to
  the CM. All this will do is queue it and set a signal for the CM task. 

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void cmgan_rpt_ind (

  const cmgan_gw_gan_rpt_s_type gan_rpt
    /* GAN report with 
    ** GAN serving system info 
    */

);

/*~ FUNCTION cmgan_rpt_ind*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CMGAN_H */
