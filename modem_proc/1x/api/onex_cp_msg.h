/*!
  @file   onex_cp_msg.h

  @brief  Defines the external message interface for the 1xCP module

*/

/*===========================================================================

  Copyright (c) 2009 - 2015 Qualcomm Technologies, Inc. 
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


===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/onex_cp_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/15   ppr     To include search_window_size in CGI Request
06/09/14   agh     FR 17879: Added support for extended EARFCN
03/15/14   ppr     SRVCC Feature Changes
12/06/13   gga     Added Handling for LTE Abort HO Request
11/25/13   ppr     1x CSFB CGI feature interface changes
10/31/13   gga     FR 17021: R8 CSFB Call setup optimization
05/23/13   nee     FEATURE LTE Deprioritization changes.
08/26/11   ssh     1X to LTE reselection support - part 2.
06/14/11   ssh     Added ATIM support to perform the 1X to LTE reselection.
12/07/10   jj      Updated HO interface to have timing information to perform
                   Time Transfer.
09/29/10   jj      Updated interface to handle LTE -> 1X Handover.
06/18/10   jj      Updated Resel Interface to have correct data type 
                   for lte_ostmr.
02/19/10   jj      Added error codes for Redir/Resel failure.
01/08/10   jj      Changes done to rename the MSG ID's in CP.
12/09/09   jtm     Added missing sys_v.h include.
11/30/09   jj      Updated interface file.
10/08/09   np      Updated documentation, deleted redundancies and misc cleanup.
09/30/09   np/jj   Initial rev.
===========================================================================*/

#ifndef ONEX_CP_MSG_H
#define ONEX_CP_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"
#include <sys.h>
#include <sys_v.h>

#ifdef __cplusplus
}  // extern "C"
#endif

#include "onex_msgr.h"
#include "lte_irat_types.h"
#include "lte_rrc_irat_msg.h"
#include "lte_rrc_ext_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief The text string for the external ONEX_CP Task Queue. */
#define ONEX_CP_MSG_Q_NAME              "ONEX_CP_Q" // FIXME ?? is this needed if REX wrapper is used?

#define ONEX_CP_MAX_LTE_DEPRI_FREQ      64 /*! \brief MAX number of EUTRA freq deprioritized */

/*-----------------------------------------------------------------------*/
/*     CDMA ONEX CP Supervisory                                                 */
/*-----------------------------------------------------------------------*/

/*! @brief Supervisory commands the CDMA ONEX CP module receives.  */
enum
{
  /*! @brief Not used for now, sent from APPMGR */
  MSGR_DEFINE_UMID(ONEX, CP, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_struct_type),
  /*! @brief Not used for now, send to APPMGR */
  MSGR_DEFINE_UMID(ONEX, CP, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_struct_type)

};

/*-----------------------------------------------------------------------*/

/*! @brief Supervisory Loopback  - This message is sent by the message router
           and is to be sent back to it in a onex_cp_spr_loopback_reply_t to
           confirm that the umb message queues are operating correctly. */
typedef msgr_spr_loopback_struct_type       onex_cp_spr_loopback_t;

/*! @brief Supervisory Loopback Reply  - This reply is sent back to the
           message router in response to receiving a onex_cp_spr_loopback_t
           message to confirm that the ONEX CP message queues are operating
           correctly. */
typedef msgr_spr_loopback_reply_struct_type onex_cp_spr_loopback_reply_t;


/*! @brief Requests the ONEX CP module receives. */
enum
{
  /*! @brief The first message in the enum */
  ONEX_CP_REQ_FIRST = MSGR_UMID_BASE(MSGR_ONEX_CP, MSGR_TYPE_REQ),

  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_REDIR,        0x00, onex_cp_lte_redir_req_msg_t),       // ONEX_CP_LTE_REDIR_REQ
  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_RESEL,        0x01, onex_cp_lte_resel_req_msg_t),       // ONEX_CP_LTE_RESEL_REQ
  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_ABORT_REDIR,  0x02, onex_cp_lte_abort_redir_req_msg_t), // ONEX_CP_LTE_ABORT_REDIR_REQ
  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_ABORT_RESEL,  0x03, onex_cp_lte_abort_resel_req_msg_t), // ONEX_CP_LTE_ABORT_RESEL_REQ
  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_1XCSFB_HO,    0x04, onex_cp_lte_1xcsfb_ho_req_msg_t),   // ONEX_CP_LTE_1XCSFB_HO_REQ
  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_DEPRI_FREQ,   0x05, onex_cp_lte_depri_freq_req_msg_t),  // ONEX_CP_LTE_DEPRI_FREQ_REQ
  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_GET_CGI,      0x06, onex_cp_lte_get_cgi_req_msg_t),     // ONEX_CP_LTE_GET_CGI_REQ
  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_ABORT_CGI,    0x07, onex_cp_lte_abort_cgi_req_msg_t),   // ONEX_CP_LTE_ABORT_CGI_REQ
  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_ABORT_1XCSFB_HO,    0x08, onex_cp_lte_abort_1xcsfb_ho_req_msg_t),   // ONEX_CP_LTE_ABORT_1XCSFB_HO_REQ
  MSGR_DEFINE_UMID(ONEX, CP, REQ, LTE_1XSRVCC_HO,    0x09, onex_cp_lte_1xsrvcc_ho_req_msg_t), // ONEX_CP_LTE_1XSRVCC_HO_REQ

  /*! @brief The last message in the enum */
  ONEX_CP_REQ_LAST = ONEX_CP_LTE_1XSRVCC_HO_REQ

};

/*! @brief Responses the ONEX CP module sends. */
enum
{
  /*! @brief The first message in the enum */
  ONEX_CP_RSP_FIRST = MSGR_UMID_BASE(MSGR_ONEX_CP, MSGR_TYPE_RSP),

  MSGR_DEFINE_UMID(ONEX, CP, RSP, LTE_REDIR_FAILED,     0x00, onex_cp_lte_redir_failed_rsp_msg_t), // ONEX_CP_LTE_REDIR_FAILED_RSP
  MSGR_DEFINE_UMID(ONEX, CP, RSP, LTE_RESEL_FAILED,     0x01, onex_cp_lte_resel_failed_rsp_msg_t), // ONEX_CP_LTE_RESEL_FAILED_RSP
  MSGR_DEFINE_UMID(ONEX, CP, RSP, LTE_ABORT_REDIR,      0x03, onex_cp_lte_abort_redir_rsp_msg_t),  // ONEX_CP_LTE_ABORT_REDIR_RSP
  MSGR_DEFINE_UMID(ONEX, CP, RSP, LTE_ABORT_RESEL,      0x04, onex_cp_lte_abort_resel_rsp_msg_t),  // ONEX_CP_LTE_ABORT_RESEL_RSP
  MSGR_DEFINE_UMID(ONEX, CP, RSP, LTE_1xCSFB_HO_FAILED, 0x05, onex_cp_lte_1xcsfb_ho_fail_rsp_t),   // ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP
  MSGR_DEFINE_UMID(ONEX, CP, RSP, LTE_GET_CGI,          0x06, onex_cp_lte_get_cgi_rsp_msg_t),      // ONEX_CP_LTE_GET_CGI_RSP
  MSGR_DEFINE_UMID(ONEX, CP, RSP, LTE_ABORT_1xCSFB_HO,  0x07, onex_cp_lte_abort_1xcsfb_ho_rsp_t),   // ONEX_CP_LTE_ABORT_1xCSFB_HO_RSP
  MSGR_DEFINE_UMID(ONEX, CP, RSP, LTE_1xSRVCC_HO_FAILED, 0x08, onex_cp_lte_1xsrvcc_ho_fail_rsp_t), // ONEX_CP_LTE_1xSRVCC_HO_FAILED_RSP

 /*! @brief The last message in the enum */
  ONEX_CP_RSP_LAST = ONEX_CP_LTE_1xSRVCC_HO_FAILED_RSP
};

/*! @brief Indications the ONEX CP module sends. */
enum
{
  /*! @brief The first message in the enum */
  ONEX_CP_IND_FIRST = MSGR_UMID_BASE(MSGR_ONEX_CP, MSGR_TYPE_IND),
  
  MSGR_DEFINE_UMID(ONEX, CP, IND, LTE_1xCSFB_CALL_REL, 0x00, onex_cp_lte_1xcsfb_call_rel_ind_t),    // ONEX_CP_LTE_1xCSFB_CALL_REL_IND
  
  /*! @brief The last message in the enum */
  ONEX_CP_IND_LAST = ONEX_CP_LTE_1xCSFB_CALL_REL_IND
};
/*-----------------------------------------------------------------------*/
/* Requests received by ONEX CP */
/*-----------------------------------------------------------------------*/

#define ONEX_CP_PILOT_PN_LIST_MAX_SIZE  32  // FIXME, filler value!

/*!
    @brief

    Parameters for the redirection info include the band and frequency
*/
typedef struct
{
  /*! Band Indicator */
  sys_band_class_e_type           band;

   /*! Frequency */
  sys_channel_num_type            freq;

} onex_cp_redir_info;

/*!
    @brief

    Payload for the actual redirection request that 1xCP receives from
    external modules like LTE.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;

  /*! Actual redirection info as contained in LTE RRC Connection Rel Msg. */
  onex_cp_redir_info              redir_info;

  /*! Additional (Optional) information for redirection from LTE SIB-8 */
  uint8                           pilot_pn_count;

  /*! List of pilots corresponding to band/chan in redirection req */
  sys_pilot_pn_type               pilot_pn_info[ ONEX_CP_PILOT_PN_LIST_MAX_SIZE ];

  /*! Cell id of the LTE system in redirection request */
  lte_rrc_global_cell_id_t        global_cell_id;
  
  /*! PLMN ID of the LTE system in redirection request*/ 
  lte_rrc_plmn_s                  redir_plmn;

} onex_cp_lte_redir_req_msg_t;

/*!
    @brief

    Payload for the actual reselection request that 1xCP receives from
    external modules like LTE.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;

  /*! Band and Frequency  */
  sys_channel_type                channel;

  /*! Cell Id - 0..maxPNOffset(511) */
  sys_pilot_pn_type               pilot_pn_offset;

  /*! LTE RTC counter */
  uint32                          lte_ostmr;

  /*! CDMA System time */
  uint64                          onex_sys_time;

} onex_cp_lte_resel_req_msg_t;

/*!
    @brief
    Payload of redirection abort request received by 1xCP if redirection aborts
    from external module like LTE.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;
} onex_cp_lte_abort_redir_req_msg_t;

/*!
    @brief
    Payload of reselection abort request received by 1xCP if redirection aborts
    from external module like LTE.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;
} onex_cp_lte_abort_resel_req_msg_t;

/*!
    @brief
    Payload of Handover request received by 1xCP from external module like LTE.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type  hdr;

  /*! LTE RTC counter, needed for Time Transfer */
  uint32                          lte_ostmr;

  /*! CDMA System time,needed for Time Transfer */
  uint64                          onex_sys_time;

  /*! DLInformationTransfer RRC Message */
  lte_irat_tunnel_dl_msg_dl_info_tx_s  ho_info_tx;
} onex_cp_lte_1xcsfb_ho_req_msg_t;

/* SRVCC: Using same structure definitions similar to CSFB */
typedef onex_cp_lte_1xcsfb_ho_req_msg_t onex_cp_lte_1xsrvcc_ho_req_msg_t;
 
/*!
    @brief
    Payload of LTE deprioritization request received by 1xCP from LTE.
    1x should apply the lowest priority value for the EUTRA frequencies
    that are deprioritized.
*/
typedef struct
{
  /*! brief Message header */
  msgr_hdr_struct_type         hdr;

  /*! Indicates if all EUTRA neighbor freqs should be deprioritized. If set to TRUE
    Num_depri_freq is not populated */
  boolean                      depri_all_freq;

  /*! Number of freqs being deprioritized; Will be set to 0 if none of EUTRA
  frequencies are deprioritized */
  uint8                        num_depri_freq;
  
  /*! Set of deprioritized EUTRA frequencies */
  lte_earfcn_t                 depri_freqs[ONEX_CP_MAX_LTE_DEPRI_FREQ];

} onex_cp_lte_depri_freq_req_msg_t;


/*!
    @brief
    Various Failure codes for 1x CGI info 
*/
typedef enum
{
  ONEX_CP_CGI_RSP_SUCCESS, 
  ONEX_CP_CGI_RSP_FAIL_SYS_PARAM_NOT_RCVD,
  ONEX_CP_CGI_RSP_FAIL_SYS_PARAM_INVALID,
  ONEX_CP_CGI_RSP_FAIL_INVALID_STATE,
  ONEX_CP_CGI_RSP_ABORTED,
  ONEX_CP_CGI_RSP_FAIL_OTHER,
} onex_cp_cgi_info_status_e;


/*!
    @brief
    Payload of Get CGI request received by 1x CP from LTE.
*/
typedef struct
{

  /*! Message header */
  msgr_hdr_struct_type    hdr;

  /*! Band and Frequency  */
  sys_channel_type        channel;

  /*! Cell Id - 0..maxPNOffset(511) */
  sys_pilot_pn_type       pilot_pn_offset;

  /*! LTE RTC counter */
  uint32                  lte_ostmr;

  /*! CDMA System time */
  uint64                  onex_sys_time;

  /* Time in ms that 1x CP has, for acquiring CGI info; 1x CP has to send a 
        response back to LTE with a success/failure within this time */
  uint32                  time_available_for_cgi;  

  /*! CDMA search_window_size
       If Value is 0xFF, then 1x CP should use default parameters*/
  uint8                   search_window_size;
} onex_cp_lte_get_cgi_req_msg_t;

/*!
    @brief
    Payload of Abort CGI request received by 1x CP from LTE.
*/
typedef struct
{

  /*! Message header */
  msgr_hdr_struct_type    hdr;

} onex_cp_lte_abort_cgi_req_msg_t;

/*!
    @brief
    Payload of 1x CGI info
*/
typedef struct
{
  /*! BASEID */
  uint16        base_id;

  /*! SID */
  uint16        sid;

  /*! NID */
  uint16        nid;

} onex_cgi_info_t ;


/*!
    @brief
    Payload of Get CGI response sent by 1x CP to LTE. This can be used as a 
    response for abort CGI request as well.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type        hdr;

  /*!< Band and Frequency of the cell for which CGI is requested */
  sys_channel_type            channel;

  /*! Cell Id - 0..maxPNOffset(511) */
  sys_pilot_pn_type           pilot_pn_offset;

  /*! Pilot strength based on Measurements on the PN (0..63), a valid value 
          should always be included in the response */
  uint8                       pilot_strength;

  /* CGI info status */
  onex_cp_cgi_info_status_e   cgi_info_status;     

  /*! CGI info in 1x*/
  onex_cgi_info_t             cgi_info;

} onex_cp_lte_get_cgi_rsp_msg_t;


/*!
    @brief
    Payload of reselection abort request received by 1xCP if csfb ho aborts
    from external module like LTE.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;
} onex_cp_lte_abort_1xcsfb_ho_req_msg_t;

/*-----------------------------------------------------------------------*/
/* Indications sent out by ONEX CP */
/*-----------------------------------------------------------------------*/

typedef enum
{
  ONEX_CP_FAIL_REDIR, 
  ONEX_CP_FAIL_RESEL, 
} onex_cp_fail_cause_e;

/*!
    @brief
    Payload of Indication sent by 1xCP if redirection fails
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;

  /*! failure cause */
  onex_cp_fail_cause_e            cause;

} onex_cp_lte_redir_failed_rsp_msg_t;

/*!
    @brief
    Payload of Indication sent by 1xCP if reselection fails
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;
  /*! failure cause */
  onex_cp_fail_cause_e            cause;

} onex_cp_lte_resel_failed_rsp_msg_t;

/*!
    @brief
    Payload of Response sent by 1xCP if handover fails.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type  hdr;
  /*! failure cause */
  lte_irat_1xcsfb_ho_failure_cause_e  ho_fail_cause;
} onex_cp_lte_1xcsfb_ho_fail_rsp_t;


/*!
    @brief
    Payload of Response sent by 1xCP if SRVCC handover fails.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type  hdr;
  /*! failure cause - lte_irat_1xsrvcc_ho_failure_cause_e 
   * ho_fail_cause declared as int32 here to avoid compilation issues because 
   * of interface interdependency between LTE-RRC and 1xCP when FEATURE_1X_SRVCC 
   * feature not enabled on that particular PL's so RRC interface were not available
   * but 1x SRVCC interface changes were available due to 1x common branch */
  int32  ho_fail_cause;
} onex_cp_lte_1xsrvcc_ho_fail_rsp_t;

/*!
    @brief
    Payload of Indication sent by 1xCP for network intiated call releases.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type  hdr;

  /*! Call release cause */
  lte_irat_1xcsfb_ho_failure_cause_e  ho_fail_cause;
} onex_cp_lte_1xcsfb_call_rel_ind_t;

/*-----------------------------------------------------------------------*/
/* Confirmations sent out by ONEX CP */
/*-----------------------------------------------------------------------*/
/*!
    @brief
    Payload of Confirmation sent by 1xCP if redirection aborts
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;
} onex_cp_lte_abort_redir_rsp_msg_t;

/*!
    @brief
    Payload of Confirmation sent by 1xCP if reselection aborts
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;
} onex_cp_lte_abort_resel_rsp_msg_t;

/*!
    @brief
    Payload of Confirmation sent by 1xCP if csfb ho aborts
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type            hdr;
} onex_cp_lte_abort_1xcsfb_ho_rsp_msg_t;

#endif /* ONEX_CP_MSG_H */
