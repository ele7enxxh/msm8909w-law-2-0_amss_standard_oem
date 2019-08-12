#ifndef HOMER_H
#define HOMER_H
/*===========================================================================

                      H A N D O F F   M A N A G E R
                         H E A D E R   F I L E

DESCRIPTION
  This header file contains all the message definitions necessary for
  handoff between different protocols.
  
Copyright (c) 1999 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/homer.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/11   rm      CMI-4: Removing #ifdef's
01/25/07   pk      Changing Tunnled Message type to uint8
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
10/10/06   pk      Initial Revision

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "customer.h"  /* Customer configuration file                   */
#include "comdef.h"    /* Common definitions such as byte, uint16, etc. */

#include "sys.h"
#include "sys_v.h"


/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

#define HOMER_MAX_PDU_LENGTH 512
  /* Max Forward Traffic Channel msg length  */
  /* For dsch, the longest msg is UHDM, which could be more than 5000 bits
     long. This allocation should cover most cases */

#define HOMER_STANDARD_PDU_LENGTH 255
  /* Max Forward Traffic Channel msg length (see IS-95A Section 7.7.3.2.1) */


#define HOMER_MAX_CSNA_MSG  5

#define HOMER_CSNA_MSG_INVALID 0x0


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* Status for the tunneled message passed to the protocols
*/

typedef enum homer_tunnel_msg_status_e {

  HOMER_TUNNEL_MSG_STATUS_NONE = -1, /* INTERNAL CM use ONLY             */

  HOMER_TUNNEL_MSG_STATUS_SUCCESS,   /* The message was sent successfully */

  HOMER_TUNNEL_MSG_STATUS_FAIL_INVALID_MSG, 
                                     /* Invalid message passed */

  HOMER_TUNNEL_MSG_STATUS_FAIL_NO_ACK,
                                     /* Didn't receive ack from BS/AN */

  HOMER_TUNNEL_MSG_STATUS_FAIL_NO_SRV, 
                                     /* No service */

  HOMER_TUNNEL_MSG_STATUS_FAIL_INVALID_STATE, 
                                     /* Cannot accept tunnel message in the 
                                     ** current state */

 HOMER_TUNNEL_MSG_STATUS_MAX   /* INTERNAL CM use ONLY             */

} homer_tunnel_msg_status_e_type;

/*-------------------------------------------------------------------------*/

/* Cofiguration status of the CSNA messages
*/

typedef enum homer_csna_msg_config_status_e {

  HOMER_CSNA_MSG_CONFIG_STATUS_NONE = -1, /* INTERNAL CM use ONLY */

  HOMER_CSNA_MSG_CONFIG_STATUS_SUPPORTED, /* Message supported */

  HOMER_CSNA_MSG_CONFIG_STATUS_SUPPORTED_AND_CONFIGURED,
                                          /* Message supported and configured */

  HOMER_CSNA_MSG_CONFIG_STATUS_MAX        /* INTERNAL CM use ONLY */

} homer_csna_msg_config_status_e_type;


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* 3G1X parameters message
*/
typedef struct {

  uint8       msg_id;                    /* Message Id */

  word        signature;                 /* 3G1X Parameter Signature */

  uint64      include_mask;              /* Bit mask of the included fields */

  boolean     sid_included;

  word        sid;                       /* SID */

  boolean     nid_included;

  word        nid;                       /* NID */

  boolean     reg_zone_included;

  word        reg_zone;                  /* Reg Zone */

  boolean     total_zones_included;

  uint8       total_zones;               /* Total Zones */

  boolean     zone_timer_included;

  uint8       zone_timer;                /* Zone timer */

  boolean     pzid_included;

  uint8       pzid;                      /* pzid */

  boolean     pzid_hyst_param_included;

  boolean     pz_hyst_enabled;           /* pzid hysteresis enabled */

  boolean     pz_hyst_info_incl;         /* pzid hysteresis info incl */

  boolean     pz_hyst_list_len;          /* pzid hysteresis length */

  uint8       pz_hyst_act_timer;         /* pzid hysteresis act timer */

  uint8       pz_hyst_timer_mul;         /* pzid hysteresis timer mul */

  uint8       pz_hyst_timer_exp;         /* pzid hysteresis timer exp */

  boolean     p_rev_included;

  uint8       p_rev;                     /* p_rev */

  boolean     neg_slot_cycle_index_sup_included;

  boolean     neg_slot_cycle_index_sup;  /* neg_slot_cycle_index_sup */

  boolean     encrypt_mode_included;

  uint8       encrypt_mode;              /* Encrypt mode */

  boolean     enc_supported_included;

  boolean     enc_supported;             /* enc supported */

  boolean     sig_encrypt_sup_included;

  uint8       sig_encrypt_sup;           /* sig encrypt supported */

  boolean     msg_integrity_sup_included;

  boolean     msg_integrity_sup;         /* msg integrity sup */

  boolean     sig_integrity_sup_incl_included;

  boolean     sig_integrity_sup_incl;    /* sig integrity sup incl */

  boolean     sig_integrity_sup_included;

  uint8       sig_integrity_sup;         /* sig integrity sup */

  boolean     auth_included;

  uint8       auth;                      /* auth */

  boolean     max_num_alt_so_included;

  uint8       max_num_alt_so;            /* max alt so */

  boolean     use_sync_id_included;

  boolean     use_sync_id;               /* use sync id */

  boolean     ms_init_pos_loc_sup_ind_included;

  boolean     ms_init_pos_loc_sup_ind;

  boolean     mob_qos_included;

  boolean     mob_qos;

  boolean     band_class_info_req_included;

  boolean     band_class_info_req;

  boolean     alt_band_class_included;

  uint8       alt_band_class;

  boolean     max_add_serv_instance_included;

  uint8       max_add_serv_instance;

  boolean     home_reg_included;

  boolean     home_reg;

  boolean     for_sid_reg_included;

  boolean     for_sid_reg;

  boolean     for_nid_reg_included;

  boolean     for_nid_reg;

  boolean     power_up_reg_included;

  boolean     power_up_reg;

  boolean     power_down_reg_included;

  boolean     power_down_reg;

  boolean     parameter_reg_included;

  boolean     parameter_reg;

  boolean     reg_prd_included;

  uint8       reg_prd;

  boolean     reg_dist_included;

  word        reg_dist;

  boolean     pref_msid_type_included;

  uint8       pref_msid_type;

  boolean     ext_pref_msid_type_included;

  uint8       ext_pref_msid_type;

  boolean     meid_reqd_included;

  boolean     meid_reqd;

  boolean     mcc_included;

  word        mcc;

  boolean     imsi_11_12_included;

  uint8       imsi_11_12;

  boolean     imsi_t_supported_included;

  boolean     imsi_t_supported;

  boolean     reconnect_msg_ind_included;

  boolean     reconnect_msg_ind;

  boolean     rer_mode_supported_included;

  boolean     rer_mode_supported;

  boolean     tkz_mode_supported_included;

  boolean     tkz_mode_supported;

  boolean     tkz_id_included;

  word        tkz_id;

  boolean     pilot_report_included;

  boolean     pilot_report;

  boolean     sdb_supported_included;

  boolean     sdb_supported;

  boolean     auto_fcso_allowed_included;

  boolean     auto_fcso_allowed;

  boolean     sdb_in_rcnm_ind_included;

  boolean     sdb_in_rcnm_ind;

  boolean     fpc_fch_included;

  uint8       fpc_fch_init_setpt_rc3;

  uint8       fpc_fch_init_setpt_rc4;

  uint8       fpc_fch_init_setpt_rc5;

  boolean     t_add_included;

  uint8       t_add;

  boolean     pilot_inc_included;

  uint8       pilot_inc;

  uint32      rand;

  uint8       reserved;
 
} homer_3g1x_param_s_type;

/*-------------------------------------------------------------------------*/

/* Type for the tunneled message
*/
typedef struct {

  sys_sys_id_type_e_type dest_protocol;

  uint16                 len;

  uint8                  p_rev;                     /* p_rev */

  uint8                  chan;

  byte                   payload[HOMER_MAX_PDU_LENGTH];

}homer_tunnel_msg_s_type;

/*-------------------------------------------------------------------------*/

/* Type for message tunnled from HDR to CM
*/
typedef struct {

  homer_tunnel_msg_s_type hdr_payload;

  homer_3g1x_param_s_type   hdr_3g1x_param;

} homer_hdr_cm_tunnel_msg_s_type;

/*-------------------------------------------------------------------------*/

/* Type for CSNA message configuration
*/
typedef struct {

  uint8 msg_id;

  homer_csna_msg_config_status_e_type csna_msg_status;

} homer_csna_msg_config_s_type;

#endif  /* HOMER_H */
