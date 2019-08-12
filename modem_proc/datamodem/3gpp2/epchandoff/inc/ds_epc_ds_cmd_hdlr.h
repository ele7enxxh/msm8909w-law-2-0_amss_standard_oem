#ifndef  DS_EPC_DS_CMD_HDLR_H
#define  DS_EPC_DS_CMD_HDLR_H

#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "dstask_v.h"
#include "ps_iface_defs.h"
#include "ps_acl.h"
#include "list.h"

typedef struct
{
  uint16  attach_pf_id;
  ps_iface_type* v4_iface_ptr;
  ps_iface_type* v6_iface_ptr;
} epc_lte_attach_init_cmd_params_type;

typedef struct
{
  uint32  tech_type;
  uint32  so_mask;
  uint32  rat_mask;
} epc_ss_bearer_tech_chg_cmd_params_type;

typedef struct
{
  boolean  tunnel_call_allowed;
  boolean  lcp_throttled;
  boolean  virtual_connection_throttled;
  boolean  hdr_actv_ho_in_prog;
  uint32   subs_id;
} epc_ss_3gpp2_s101_cmd_params_type;

typedef struct
{
  boolean prereg_handoff_succeeded;
  uint32  subs_id;
} epc_ss_ehrpd_prereg_complete_cmd_param_type;

typedef struct
{
  acl_policy_info_type    policy_info;
  ps_iface_type           *iface_ptr;
  boolean                 re_post_proc; 
}epc_cb_init_cmd_params_type;

typedef struct
{
  list_type               *list;
  uint32                   subs_id;
}epc_thrtl_pdn_list_cmd_params_type;

/*===========================================================================
FUNCTION      DS_EPC_DS_CMD_PROCESS

DESCRIPTION

PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_ds_cmd_process 
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
);
#endif /*FEATURE_EPC_HANDOFF*/
#endif /*DS_EPC_DS_CMD_HDLR_H*/
