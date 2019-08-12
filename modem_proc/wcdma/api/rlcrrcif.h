#ifndef RLCRRCIF_H
#define RLCRRCIF_H
/*===========================================================================
              R L C - R R C  I N T E R F A C E  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  RLC - RRC Interface.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rlcrrcif.h#1 $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
06/23/14  kc      Changes for data indication to RRC in HS-RACH state
07/24/12  geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/26/12  grk     Added new API file exposing L2 command structures
04/26/12  grk     Mainlined code under FEATURE_INACTIVITY_HANDLING.
04/24/12  kc      Moving WM (de)registration prototypes here for CMI compliance
04/03/12  grk     Feature cleanup.
03/20/12  grk     Dependency changes for modem API cleanup from FC.
07/29/11  kc      CMI Phase-4 de-featurization for Nikel 1.x
05/30/11  kc      CMI Phase-4 de-featurization for Nikel 1.x
03/09/11  ssg     MAC-ehs support on Apollo.
01/17/11  vp      Moved wcdma only declarations to rlcrrcif_v.h
01/12/11  grk     Compilation fixes for non-L2 opt.
12/10/10  grk     Fix to validate RLC PDU size on DL.
12/07/10  pj      Added changes to populate rlc_sn along with hfn for rrc
10/18/10  ssg     Added changes for new WCDMA-L2 interface.
10/27/10  grk     Added API to check DL SDU status.
09/28/10  grk     Added API to provide WM and ACK status for AM RBs to RRC.
03/30/10  grk     Modified prototype of DSM WM callback for CMI compliance.
04/13/09  pj      Added support for CS-over-HS
05/29/09  ssg     Added support for SNOW_3G algo.
05/11/09  ssg     Updated Copyright Information
03/01/09  rm      Added new featurization for FEATURE_CMI.
03/03/08  ssg     Added support to alternate e-bit interpretations under the
                  featue flag FEATURE_RLC_ALTERNATE_E_BIT.
10/17/08  grk     Merged taxis changes to mainline.
05/16/08  ssg     Added structures for DL stop, DL continue request.
04/28/08  ssg     Added function interface to indicate RRC about the LC 
                  registration for WM in Cell_PCH/Ura_PCH states.
02/01/08  pj      Changed FEATURE_MBMS flag to FEATURE_MODEM_MBMS.
12/06/07  vg      added support for multiple RRC operations handling at the
                  time of channel config req under feature flag 
                  FEATURE_MULTIPLE_RLC_OPERATIONS.
11/30/07  ssg     Added mbms support under FEATURE_MBMS.
09/18/07  vg      modified suspend  to suspend_stop to support SMC and SRNNS
                  scenarios
07/06/07  ssg     Added enum to indictae loop back mode under FEATURE_TM_LB.
11/23/06  ssg     Added prototype for crlc_ul_perform_suspend_activity().
09/11/06  ttl     Add support to overwrite the HFN being using.
08/23/06  ssg     Added the enum rlc_reestablish_e_type to rlc_am_config_type
                  under the feature FEATURE_RLC_ONE_SIDE_RE_ESTABLISHMENT.
07/28/06  ttl     Add support for RLC HFN query.
06/02/06  ttl     Add SRB2 suspending support.
04/28/06  ttl     Moved Release 5 specified code under FEATURE_REL5.
11/07/05  aw      Added support for inactivity handling of user plane
                  UM and AM channels under the feature 
                  FEATURE_INACTIVITY_HANDLING.
06/13/05  mv      Added LI_SAME_AS_BEFORE to the li enum type
                  rlc_li_e_type.
05/03/05  mv      Added the structure rlc_abort_cipher_config_type for
                  getting the information on logical channels for which
                  ongoing ciphering config needs to be aborted.
03/08/05  vk      Added support for LI indication for DL UM for Rel 5.
02/11/05  mv      Added the structure rlc_ul_purge_wm_ind_type for getting
                  the information for purging the UL WM.
01/07/04  ttl     Added CELL_PCH support.
10/28/03  ttl     Added rlc_rrc_get_wm_info_req() to give the total number
                  of bytes need to be sent next.
                  Added ciphering algorithm indication.
08/20/03  ttl     Merged branch code into tip.
08/11/03  ttl/mv  Added the RLC Re-establish interface.
05/30/03  ttl     Changed cipher_key_id from uint16 to uint32.
10/30/02  ttl     Modified rlc_hfn_req_type to have the hfns for UL and DL
                  to be in separate table.
09/16/02  ttl     Removed unused rlc_sn from rlc_dl_ciphering_type.
09/05/02  ttl     Added Ciphering feature.
                  Modified structure rlc_hfn_req_type, rlc_ul_resume_type and
                  rlc_ul_resume_cnf_type.
08/13/02  ttl     Added ciph_act_sn to rlc_ul_ch_ciphering_type and
                  rlc_dl_ciphering_type.
                  Added rlc_ul_resume_cnf_type, rlc_hfn_req_type, rlc_sn_type
                  and rlc_ul_sn_req_type.
                  Added function type rlc_rrc_get_hfn_req() and rlc_rrc_ul_rlc_sn_req().
05/13/02  ttl     Added RLC_STOP and RLC_CONTINUE for Config request primitive.
04/03/02  ttl     Moved rlc_size from common area to control config area in 
                  AM config request structure.
12/04/01  ttl     Changed PU concepts to PDU concept for June specification.
08/30/01  rc      Fixed some comments.
05/29/01  ttl     Changed naming convention from nchan to nentity for
                  am_config_req.
                  Added dl_nchan and ul_nchan.
05/21/01  rc      Cleaned up code.
05/02/01  ttl     Changed all lc_id to rlc_lc_id_type.
04/09/01  ttl     Removed redundant timer_discard definition in UL AM.
04/02/01  ttl     Modified the config structure for all three modes to move 
                  the action out of the existing config structure and become
                  a new structure -- rlc_lc_action_type.
02/28/01  ttl     Added support for Acknowledged mode.
                  RRC gives the RLC size instead of the li length. The
                  li length is computed by RLC. Added data types for
                  confirmation of configuration for TM, UM and AM modes.
01/08/01  rc      Modified code to use the data types defined in uecomdef.h.
11/22/00  rc      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/
#include "uecomdef.h"
#include "rlcdsapif.h"
#include "wl2api.h"
/* --------------------------------- */
/* PUBLIC APIs of rlcrrcif.h         */
/* --------------------------------- */



/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

#define RLC_MODIFY_CFG                0x0001
#define RLC_STOP_CFG                    0x0002
#define RLC_CONTINUE_CFG            0x0004
#define RLC_RE_ESTABLISH_CFG     0x0008



/*--------------------------------------------------------------------------- 
  Type that indicates the type of error that occured on the 
  uplink or downlink channel.
---------------------------------------------------------------------------*/
typedef enum {
  RLC_RESET = 0,                        /* The RLC protocol was reset due  */
                                        /* to an unrecoverable error. Data */
                                        /* loss may occur due to the reset.*/
  RLC_RESET_FAIL                        /* The RLC protocol reset failed,  */
                                        /* i.e., the link is down.         */
} rlc_error_e_type;



typedef enum {
  RLC_NO_CIPHER_CFG_PENDING = 0,         /* no ciphering config pending   */
  RLC_CIPHER_CFG_PENDING,                /* Yes, there is at least one    */
  RLC_CIPHER_STATUS_UNSPECIFIED,         /* Can not find the LC           */
  RLC_MAX_CIPHER_CFG_STATUS
} rlc_cipher_cfg_pending_e_type;

  typedef enum {
   FLEXIBLE_PDU_SIZE_CONF,
   FIXED_PDU_SIZE_CONF
}rlc_dl_am_pdu_e_type;

/* Type to indicate the command that triggers the RLC suspension or stop*/
typedef enum
{
  RLC_INVALID,
  RLC_SUSPEND_CMD,
  RLC_STOP_CMD
}rlc_cmd_type;

typedef enum {
  RLC_REG_DATA_IND_IN_FACH = 0,
  RLC_UNREG_DATA_IND_IN_FACH
} rlc_ul_reg_data_ind_e_type;

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/*---------------------------------------------------------------------------
  Type for notifying RRC about errors. Used only for Acknowledged Mode
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  boolean direction;                    /* Indicates whether the error     */
                                        /* occured on the uplink or the    */
                                        /* downlink.                       */
  rlc_lc_id_type lc_id;                 /* Identifies the UL or DL         */
                                        /* logical channel on which the    */
                                        /* error condition occured         */
  rlc_error_e_type error_type;          /* Indicates the type of error that*/
                                        /* occured.                        */
} rlc_status_ind_type;



/* UPLINK */




/*---------------------------------------------------------------------------
  Type to suspend uplink data transmission on a single 
  Unacknowledged or Acknowledged Mode logical channel. 
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uint8  n;                             /* RLC shall not transmit a PDU    */
                                        /* with Sequence number >= Current */
                                        /* Sequence Number+n. The sequence */
                                        /* number should be less than the  */
                                        /* maximum allowed sequence number.*/
                                         /*  incase of RLC_STOP cmd, n should be set to 0*/
} rlc_ul_stop_suspend_info_type;


/*---------------------------------------------------------------------------
  Type to suspend uplink data transmission on one or more Unacknowledged  
  or Acknowledged Mode logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of Unacknowledged or     */
                                        /* unacknowledged mode uplink      */
/* Type to indicate the command that triggers the RLC suspension or stop*/
  rlc_cmd_type  ul_cmd_type;
                                        /* logical channels to suspend     */
  rlc_ul_stop_suspend_info_type   suspend_stop[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Suspend Information for each    */
                                        /* uplink logical channel          */
} rlc_ul_stop_suspend_request_type;

/*---------------------------------------------------------------------------
  Type to stop downlink data transmission on one or more Unacknowledged  
  or Acknowledged Mode logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of Unacknowledged or     */
                                        /* unacknowledged mode downlink      */
                                        /* logical channels to stop     */
  rlc_lc_id_type lc_id[UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* RLC ID of channels to be     */
                                        /* stopped          */
} rlc_dl_stop_request_type;

/*---------------------------------------------------------------------------
  Type to confirm that uplink data transmission on the specified 
  logical channel has been suspended.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uint16 rlc_sn;                        /* The sequence number of the next */
                                        /* PDU that will be sent when      */
                                        /* data transmission on the        */
                                        /* specified channel is resumed.   */
} rlc_ul_chsuspend_cnf_type;


/*---------------------------------------------------------------------------
  Type to confirm that uplink data transmission on the logical channels 
  specified by RRC, has been suspended.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of Unacknowledged or     */
                                        /* unacknowledged mode downlink    */
                                        /* logical channels for which      */
                                        /* suspend confirmation is sent to */
                                        /* RRC                             */
  rlc_ul_chsuspend_cnf_type cnf[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Suspend Confirmation Information*/
                                        /* for each uplink logical channel */
} rlc_ul_stop_suspend_cnf_type;

/*---------------------------------------------------------------------------
  Type to confirm to RRC uplink data transmission on one or more 
  unacknowledged or acknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/ 
                                        /* channels to configure           */
  rlc_lc_id_type  channel[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Uplink logical channels to      */
                                        /* resume                          */
} rlc_ul_resume_cnf_type;

/*---------------------------------------------------------------------------
  Type to request the current HFN value.
---------------------------------------------------------------------------*/
typedef struct {
  boolean hfn_valid;
  uint16 rlc_sn;
  uint32 hfn;
  rlc_lc_id_type rlc_id;
} rlc_hfn_type;

/*---------------------------------------------------------------------------
  Type to request the current COUNT_C value.
---------------------------------------------------------------------------*/
typedef struct {
  uint32 count_c;
  rlc_lc_id_type rlc_id;
} rlc_count_c_type;

/*---------------------------------------------------------------------------
  Type to specify whether there is any data received on user plane channels 
  or not. RRC is going to query RLC to see if there is any user plane traffic
---------------------------------------------------------------------------*/
typedef enum
{
  DATA_TRANSFERRED = 0,
  NO_DATA_TRANSFERRED
}rlc_data_activity_type;

/*---------------------------------------------------------------------------
  Type to request the current HFN values for all established 
  logical channels w/ Ciphering being turned on.
---------------------------------------------------------------------------*/
typedef struct {
  uint8  ul_nchan;                       /* Number of UM and/or AM channels*/
                                         /* HFN for  UL                    */
  uint8  dl_nchan;                       /* Number of UM and/or AM channels*/
                                         /* HFN for  DL                    */
  rlc_hfn_type hfn_per_rb_ul[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
  rlc_hfn_type hfn_per_rb_dl[UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                         /* Values of all UM and/or AM ch  */
                                         /* for both UL and DL             */
} rlc_hfn_req_type;

/*---------------------------------------------------------------------------
  Type to request the current COUNT_C values for all established 
  logical channels w/ Ciphering being turned on.
---------------------------------------------------------------------------*/
typedef struct {
  uint8  ul_nchan;                       /* Number of UM and/or AM channels*/
                                         /* HFN for  UL                    */
  uint8  dl_nchan;                       /* Number of UM and/or AM channels*/
                                         /* HFN for  DL                    */
  rlc_count_c_type count_c_per_rb_ul[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
  rlc_count_c_type count_c_per_rb_dl[UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                         /* Values of all UM and/or AM ch  */
                                         /* for both UL and DL             */
} rlc_count_c_req_type;

/*---------------------------------------------------------------------------
  Type which contains logical channel ID and the current sequence number
  for the corresponding lc_id.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type  lc_id;                 /* Logical ch ID for which SN is  */
                                         /* set                            */
  uint16 rlc_sn;                         /* SN for the lc_id               */
} rlc_sn_type;

/*---------------------------------------------------------------------------
  Type to request Uplink SN's for a number of Logical Channels
---------------------------------------------------------------------------*/
typedef struct {
  uint8  nchan;                          /* Number of UM and/or AM channels*/
                                         /* for which SN is requested      */
  rlc_sn_type rlc_ul_sn[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                         /* SN type of all UM and/or AM ch */
                                         /* for both UL and DL             */
} rlc_ul_sn_req_type;

/*---------------------------------------------------------------------------
  Type to report the current HFN value for a uplink logical 
  channel to RRC.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uint32 hfn;                           /* The current hfn value (upper 20 */
                                        /* bits of count)                  */
} rlc_ul_hfn_info_type;


/*---------------------------------------------------------------------------
  Type to report the current HFN value for all established uplink UM and AM
  logical channels to RRC.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of UL logical channels   */
                                        /* for which hfn is to be reported */
  rlc_ul_hfn_info_type hfn_info[UE_MAX_UL_UM_CHANNEL +
                                            UE_MAX_UL_AM_CHANNEL];
                                        /* Stores the hyperframe info for  */
                                        /* each logical channel            */
} rlc_ul_hfn_cnf_type;


/*---------------------------------------------------------------------------
  Type to report the config result for TM in Uplink to RRC.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uecomdef_status_e_type  result;       /* SUCCESS or FAILURE              */
} rlc_ul_tm_config_status_type;


/*---------------------------------------------------------------------------
  Type to report the config result for all TM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of UL logical channels   */
                                        /* for which hfn is to be reported */
  rlc_ul_tm_config_status_type tm_config_result[UE_MAX_UL_TM_CHANNEL];
                                        /* Stores the hyperframe info for  */
                                        /* each logical channel            */
} rlc_ul_tm_config_cnf_type;

/*---------------------------------------------------------------------------
  Type to report the config result for UM in Uplink to RRC.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uecomdef_status_e_type  result;       /* SUCCESS or FAILURE              */
} rlc_ul_um_config_status_type;


/*---------------------------------------------------------------------------
  Type to report the config result for all UM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of UL logical channels   */
                                        /* for which hfn is to be reported */
  rlc_ul_um_config_status_type um_config_result[UE_MAX_UL_UM_CHANNEL];
                                        /* Stores the hyperframe info for  */
                                        /* each logical channel            */
} rlc_ul_um_config_cnf_type;


/* DOWNLINK */

/*---------------------------------------------------------------------------
  Type to report the current HFN value for a downlink logical 
  channel to RRC.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the DL    */ 
                                        /* RLC logical channel.            */
  uint32 hfn;                           /* The current hfn value (upper 20 */
                                        /* bits of count ).                */
} rlc_dl_hfn_info_type;


/*---------------------------------------------------------------------------
  Type to report the current HFN value for all established 
  downlink acknowledged and unacknowledged mode logical channels to RRC.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of downlink logical      */
                                        /* channels for which hfn is to be */
                                        /* reported                        */
  rlc_dl_hfn_info_type hfn_info[UE_MAX_DL_UM_CHANNEL + 
                                         UE_MAX_DL_AM_CHANNEL];
                                        /* Stores the hyperframe info for  */
                                        /* each logical channel            */
} rlc_dl_hfn_cnf_type;

 
/*---------------------------------------------------------------------------
  Type to report the config result for TM in Downlink to RRC.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the DL    */ 
                                        /* RLC logical channel. Value range*/
                                        /* is from 0 to 18.                */
  uecomdef_status_e_type  result;       /* SUCCESS or FAILURE              */
} rlc_dl_tm_config_status_type;


/*---------------------------------------------------------------------------
  Type to report the config result for all TM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of downlink TM logical   */
                                        /* channels for which the config   */
                                        /* status is reported              */
  rlc_dl_tm_config_status_type tm_config_result[UE_MAX_DL_TM_CHANNEL];
                                        /* Stores the config status for    */
                                        /* each logical channel            */
} rlc_dl_tm_config_cnf_type;

/*---------------------------------------------------------------------------
  Type to report the config result for UM in Uplink to RRC.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the DL    */
                                        /* RLC logical channel. value range*/ 
                                        /* is from 0 to 18.                */
  uecomdef_status_e_type  result;       /* SUCCESS or FAILURE              */
} rlc_dl_um_config_status_type;


/*---------------------------------------------------------------------------
  Type to report the config result for all UM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of downlink UM logical   */
                                        /* channels for which the config   */
                                        /* status is reported              */        
  rlc_dl_um_config_status_type um_config_result[UE_MAX_DL_UM_CHANNEL];
                                        /* Stores config status for        */
                                        /* each logical channel            */
} rlc_dl_um_config_cnf_type;

/*---------------- Acknowledged Mode --------------------------------------*/



  
/*--------------------------------------------------------------------------- 
  Type to establish, release or re-configure acknowledged mode entity
  in downlink.
---------------------------------------------------------------------------*/
typedef struct {
  rb_id_type rb_id;
  uecomdef_status_e_type  result;
                                        /* SUCCESS or FAILURE              */
} am_entity_config_cnf_type;

/*---------------------------------------------------------------------------
  Type to report the config result for all TM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of UL logical channels   */
                                        /* for which hfn is to be reported */
  am_entity_config_cnf_type am_config_result[UE_MAX_AM_ENTITY];
                                        /* Config confirm information for  */
                                        /* each entity.                    */
} rlc_am_config_cnf_type;



/*---------------------------------------------------------------------------
  Type to request the pending HFN values for a given 
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  boolean   ul_pending_hfn_present;     /* UL HFN pending present flag for rb_id */
  boolean   dl_pending_hfn_present;    /* DL HFN pending for rb_id  */
  uint32    ul_pending_hfn;
  uint32    dl_pending_hfn;
} rlc_pending_hfn_req_type;

/*===========================================================================
**                  FUNCTION PROTO-TYPES
**=========================================================================*/

/*===========================================================================

FUNCTION RLC_UL_REGISTER_SRVC_HANDLER

DESCRIPTION
  This function handles the registration of the watermark for the uplink
  channel.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_register_srvc_handler 
(
  rlc_ul_register_srvc_type *reg_ptr      /* Ptr to WM registration info   */
);

/*===========================================================================

FUNCTION RLC_UL_DEREGISTER_SRVC_HANDLER

DESCRIPTION
  This function handles the registration of the watermark for the uplink
  channel.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlc_ul_deregister_srvc_handler 
(
  rlc_ul_deregister_srvc_type *de_reg_ptr /* Ptr to WM registration info   */
);

/*===========================================================================

FUNCTION RLC_DL_REGISTER_SRVC_HANDLER

DESCRIPTION
  This function handles the registration of the watermark and the callback
  function for the logical channels on the downlink.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_register_srvc_handler
(
  rlc_dl_register_srvc_type *reg_ptr      /* Ptr to WM registration Info   */
);
 
/*===========================================================================

FUNCTION RLC_DL_DEREGISTER_SRVC_HANDLER

DESCRIPTION
  This function handles the de-registration of the watermark and the callback
  function for the logical channels on the downlink.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rlc_dl_deregister_srvc_handler
(
  rlc_dl_deregister_srvc_type *de_reg_ptr      /* Ptr to WM registration Info   */
);

/*===========================================================================

FUNCTION RLC_UL_REG_DATA_IND_IN_FACH

DESCRIPTION
  Register/Deregister Data Indication callback function with RRC
  This function is for CELL_FACH.
  
DEPENDENCIES - None.
  
RETURN VALUE - None.
  
SIDE EFFECTS - None.
===========================================================================*/
void rlc_ul_reg_data_ind_in_fach
(
  rlc_ul_reg_data_ind_e_type data_ind_reg_type  
);
 
#endif //RLCRRCIF_H
