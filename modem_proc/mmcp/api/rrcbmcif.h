
#ifndef RRCBMCIF_H
#define RRCBMCIF_H
/*========================================================================================

           R R C B M C  I N T E R F A C E   H E A D E R    F I L E

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by RRC and LSM

  Copyright (c) 2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/rrcbmcif.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $  
  
 when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/01/2005  da     Created File.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/
#include "rrcmmif.h"

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/


/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

enum        /* message_id of inter-task messages */
{
   RRC_CELL_CHANGE_IND

   ,RRC_ETWS_PRIM_NOTIFICATION_IND

};


#define RRC_ETWS_WARN_TYPE_LEN 2
#define RRC_ETWS_WARN_SEC_INFO_LEN 50


typedef struct
{
  /* number of radio frame per TTI */
  uint8 ctch_rf_per_tti;

  /* Period of CTCH allocation ( N ) */
  uint16 ctch_allocation_period;

  /* CBS frame offset ( K ) */
  uint8 cbs_frame_offset;

  /* RLC pdu size */
  uint16 rlc_pdu_size;

  /* RLC message queue ID */
  uint8 rlc_lc_id;

  /* Period of BMC scheduling messages (P)*/
  uint16  bmc_schedul_msg_period;

} rrcbmc_ctch_info_type;


/*-------------------------------------------------------------------
RRC to BMC: RRCBMC_CELL_CHANGE_IND
Used by RRC to indicate location information and CTCH specific information
whenever there is a state change and/or cell re-selection or Cell selection 
has happened.
--------------------------------------------------------------------*/
typedef struct
{
  /*Message Header*/
  IMH_T   message_header;  /* 4 bytes */
  /* PLMN indentity */
  rrc_plmn_identity_type plmn_id;
  
  /* Location Area Code - LAC within a PLMN. FFFE and 0000 are
  * reserved values of LAC for special cases */
  uint8                     lac[RRC_MAX_GSMMAP_LAC_LENGTH];
 
  /* cell indentity */
  uint32 cell_id;
  
  /*Boolean to indicate if ctch is supported */
  boolean ctch_is_supported;
  
  /*Struct to hold ctch related info */
  rrcbmc_ctch_info_type rrcbmc_ctch_info;


  sys_modem_as_id_e_type as_id;


} rrcbmc_cell_change_ind_type;

/*
** Refer to section 9.4.1.3 ETWS primary Notification message
** in 23.041
** The flag warning secuirity info present will indicate whether security info
** is included in the ETWS primary notification or not 
** If it is set to False NAS will not update the contents of warning_sec_info to WMS
*/
typedef struct
{
  IMH_T  msg_hdr; /*!< Message router header */

  byte sr_num[2];

  byte msg_id[2];

  /* PLMN indentity */
  rrc_plmn_identity_type plmn_id;
  
  sys_modem_as_id_e_type as_id;

  byte warning_type[RRC_ETWS_WARN_TYPE_LEN]; /*!< Warning type */

  boolean warning_sec_info_present; /*!< Boolean indicating if warning_sec_info 
                                         is present */

  byte warning_sec_info[RRC_ETWS_WARN_SEC_INFO_LEN]; /*!< Warning 
                                         security info */
} rrc_etws_prim_notification_ind_type;


/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/






#endif /* RRCBMCIF_H */


