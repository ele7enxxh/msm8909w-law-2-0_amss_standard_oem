#ifndef cb_reg_h
#define cb_reg_h

/*******************************************************************************
 *  Description:    Cell Broadcast <-> REG interface
 *  ------------
 *  
 *  Comment:
 *  --------
 * 
 *  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cb_reg.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
 * 
 *  Modification History:
 *  ---------------------
 *  Date       Vers  Auth  Details
 *  ------     ----  ----  -------
 *  12-Dec-2008   1.0   nagt  Original version
 *
 *
 ******************************************************************************/


#include "sys.h"


/***********************************************************************/
/*                                                                     */
/*                         REG to CB Primitive IDs                      */
/*                                                                     */
/***********************************************************************/



/**********************************************/
/*                                            */
/*         REG_SERVICE_STATE_IND                 */
/*                                            */
/**********************************************/

enum        /* message_id of inter-task messages */
{
   REG_SERVICE_STATE_IND    = 0x00,
   REG_MODE_CHANGE_STATE_IND=0x01,
};

typedef struct
{
   IMH_T    message_header;    /* 4 bytes */
   boolean service_state ;
   boolean gsm_active_rat;

   sys_modem_as_id_e_type as_id;

}reg_service_state_ind_T;

typedef struct cb_reg_stat_chgd_req_s
{
  IMH_T    message_header;
  byte     active_subs;
} cb_reg_stat_chgd_req_s_type;

#endif

