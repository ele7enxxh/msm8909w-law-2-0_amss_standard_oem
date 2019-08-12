#ifndef NASDIAG_H
#define NASDIAG_H

/*==================================================================  
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/nasdiag.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $  
 
when       who     what, where, why
--------   ---     --------------------------------------------------------- 
=====================================================================*/
#include "mmcp_variation.h"
#include "diagpkt.h"
#include "customer.h"
#include "sys_v.h"
#include "sys_cnst_v.h"
#include "sys.h" 

#ifdef FEATURE_NAS_DIAG_INIT

#define NAS_DIAG_PTMSI_SIZE             PTMSI_SIZE
#define NAS_DIAG_SYS_PLMN_LIST          (SYS_PLMN_LIST_MAX_LENGTH+1)*3


/* NAS Diag Request Command Codes:  */
enum {
    NAS_DIAG_GMM_STATE_INFO_F,
    NAS_DIAG_PTMSI_RAC_F,
    NAS_DIAG_NMO_F,
    NAS_DIAG_ATT_COUNTERS_F,
    NAS_DIAG_RAU_COUNTERS_F,
    NAS_DIAG_SMS_PRIORITY_F,
    NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F,
    NAS_DIAG_EPLMN_F,
    NAS_DIAG_T3212_F,
    NAS_DIAG_T3312_F,
    NAS_MAX_CMD_F
};
/************************************************************************** 
    PACKET DEFINITIONS
**************************************************************************/

/* NAS GMM state Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_GMM_STATE_INFO_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_GMM_STATE_INFO_F)
  /* GMM States */
  uint8  gmm_state_diag;
  uint8  gmm_substate_diag;
DIAGPKT_RSP_END




/* NAS PTMSI RAC Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_PTMSI_RAC_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_PTMSI_RAC_F)
  /* GMM PTMSI and RAC */
  uint8  ptmsi[NAS_DIAG_PTMSI_SIZE];
  uint8  routing_area_code_diag;
DIAGPKT_RSP_END




/* NAS NMO Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_NMO_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_NMO_F)
  /* NMO */
  uint8  network_op_mode_diag;
DIAGPKT_RSP_END




/* NAS ATTACH COUNTER Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_ATT_COUNTERS_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_ATT_COUNTERS_F)
  /* Attach Counter */
  uint8  gprs_attach_attempt_counter_diag;
DIAGPKT_RSP_END




/* NAS RAU Counter Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_RAU_COUNTERS_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_RAU_COUNTERS_F)
  /* RAU Counter */
  uint8  gmm_rau_attempt_counter_diag;
DIAGPKT_RSP_END




/* NAS SMS Priority Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_SMS_PRIORITY_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_SMS_PRIORITY_F)
  /* SMS Radio Priority */
  uint8  sms_radio_priority_diag;
DIAGPKT_RSP_END




/* NAS MSC and SGSN Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F)
  /* MSC/SGSN version no. */
  uint8  mscr_diag;
  uint8  sgsnr_diag;
DIAGPKT_RSP_END




/* NAS EPLMN Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_EPLMN_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_EPLMN_F)
  /* EPLMN List */
  uint8 eplmn_list_length_diag;
  uint8 eplmn_list_diag[NAS_DIAG_SYS_PLMN_LIST];
DIAGPKT_RSP_END





/* NAS T3212 Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_T3212_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_T3212_F)
  /* T3212 timer value */
  dword  T3212_timer_value_diag;
DIAGPKT_RSP_END




/* NAS T3312 Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(NAS, NAS_DIAG_T3312_F)
  /* SUB ID*/
  sys_modem_as_id_e_type    as_id;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(NAS, NAS_DIAG_T3312_F)
  /* T3312 timer value */
  dword  gmm_t3312_timer_value_diag;
DIAGPKT_RSP_END




#endif

#endif
