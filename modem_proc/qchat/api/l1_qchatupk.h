#ifndef L1_QCHATUPK_H
#define L1_QCHATUPK_H
/*===========================================================================

           Q C H A T    W C D M A  L1   A I R    I N T E R F A C E
               O P T I M I Z A T I O N S    L I B R A R Y
 
GENERAL DESCRIPTION
  This defines the interface provided by the QChat WCDMA air interface
  optimizations library.
 
  QUALCOMM does not grant to you in this Agreement any rights in the OEM
  QChat Porting Kit including the accompanying associated documentation 
  (the Software) for any purpose whatsoever, including without limitation 
  any rights to copy, use or distribute the Software. Your rights to use the 
  Software shall be only as set forth in any separate license between you and
  QUALCOMM which grants to you a license in the Software, and such license
  shall be under the terms and for the limited purposes set forth in that
  agreement.
 
Copyright (c) 2000-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/09   suren   Seperated L1 and RRC interfaces.
05/28/09   ali     initial revision 
 
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 
#include "comdef.h"
#include "wl1m.h"
#include "wl1drxmanager.h"
#include "drx.h"
/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
extern boolean qchatupk_imo_enabled;
extern boolean qchatupk_wftcs_enabled;
extern boolean qchatupk_rdw_enabled;
extern boolean qchatupk_icup_enabled;
extern boolean qchatupk_epe_enabled;
extern boolean qchatupk_asc_enabled;
extern boolean qchatupk_for_enabled;
extern boolean qchatupk_faopts_enabled;
extern boolean qchatupk_fedl_enabled;
extern boolean qchatupk_imosa_enabled;

/*===========================================================================
FUNCTION qchatupk_wcdma_imo_enabled 
 
DESCRIPTION 
  Determines if QChat IMO optimization is enabled.
  Queries the value of 'qchatupk_imo_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if IMO is enabled or not 
===========================================================================*/
boolean qchatupk_wcdma_imo_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_wftcs_enabled 
 
DESCRIPTION 
  Determines if QChat WFTCS optimization is enabled.
  Queries the value of 'qchatupk_wftcs_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if sbccha is enabled or not  
===========================================================================*/ 
boolean qchatupk_wcdma_wftcs_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_rdw_enabled 
 
DESCRIPTION 
  Determines if QChat RDW optimization is enabled.
  Queries the value of 'qchatupk_rdw_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if RDW is enabled or not 
===========================================================================*/
boolean qchatupk_wcdma_rdw_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_icup_enabled 
 
DESCRIPTION 
  Determines if ICUP optimization is enabled.
  Queries the value of 'qchatupk_icup_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if ICUP is enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_icup_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_epe_enabled 
 
DESCRIPTION 
  Determines if EPE optimization is enabled.
  Queries the value of 'qchatupk_epe_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if EPE is enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_epe_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_asc_enabled 
 
DESCRIPTION 
  Determines if ASC ptimization is enabled.
  Queries the value of 'qchatupk_asc_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if ASC is enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_asc_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_amssopts_enabled 
 
DESCRIPTION 
  This flag enables/disables QChat to leverage misc AMSS functionality 
  e.g. RRC state change notifications
  Queries the value of 'qchatupk_amssopts_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if AMSS opts are enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_amssopts_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_for_enabled 
 
DESCRIPTION 
  Determines if FOR optimization is enabled.
  Queries the value of 'qchatupk_for_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if FOR is enabled or not 
===========================================================================*/
boolean qchatupk_wcdma_for_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_faopts_enabled 
 
DESCRIPTION 
  Determines if FA optimizations are enabled.
  Queries the value of 'qchatupk_faopts_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if FA opts are enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_faopts_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_fedl_enabled 
 
DESCRIPTION 
  Determines if Fast EDL Setup optimization is enabled.
  Queries the value of 'qchatupk_fedl_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if Fast EDL Setup optimization is enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_fedl_enabled(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_imosa_enabled 
 
DESCRIPTION 
  Determines if IMOSA optimization is enabled.
  Queries the value of 'qchatupk_imosa_enabled'.  This variable
  is also exposed by QChat library.
   
RETURN VALUE 
  Indicates if IMOSA optimization is enabled or not   
===========================================================================*/
boolean qchatupk_wcdma_imosa_enabled(void);

//l1msetup.c
/*===========================================================================
FUNCTION qchatupk_process_ul_dl_drop
===========================================================================*/
void qchatupk_process_ul_dl_drop(l1_ul_phychan_enum_type ul_phychan);

/*===========================================================================
FUNCTION qchatupk_dl_cctrch_setup_done_skip_next_cmd
===========================================================================*/
boolean qchatupk_dl_cctrch_setup_done_skip_next_cmd(uint16 l1_setup_req_mask);

/*===========================================================================
FUNCTION qchatupk_skip_aich_drop
===========================================================================*/
boolean qchatupk_skip_aich_drop(void);

/*===========================================================================
FUNCTION qchatupk_l1m_cphy_setup_next_cmd_handler
===========================================================================*/
boolean qchatupk_l1m_cphy_setup_next_cmd_handler(cphy_setup_cmd_id_enum_type cphy_setup_cmd_id);

/*===========================================================================
FUNCTION qchatupk_process_aich_add_done_skip_ul_cctrch_cfg
===========================================================================*/
boolean qchatupk_process_aich_add_done_skip_ul_cctrch_cfg(dl_cphy_setup_local_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION qchatupk_dl_phychan_add_done_skip_next
===========================================================================*/
boolean qchatupk_dl_phychan_add_done_skip_next(uint16 l1_setup_req_mask, dl_cphy_setup_local_cmd_type *cmd_ptr);


/*===========================================================================
FUNCTION qchatupk_enc_process_tx_data
===========================================================================*/
boolean qchatupk_enc_process_tx_data(boolean skip_enc_process_tx_data);

//wenc.c
/*===========================================================================
FUNCTION qchatupk_get_enc_build_fr_ind_cfn
===========================================================================*/
void qchatupk_get_enc_build_fr_ind_cfn(void);

/*===========================================================================
FUNCTION qchatupk_l1_init 
===========================================================================*/
void qchatupk_l1_init(void);

// wl1m.c
/*===========================================================================
FUNCTION qchatupk_l1m_process_ext_cmd 
===========================================================================*/
void qchatupk_l1m_process_ext_cmd(void);

/*===========================================================================
FUNCTION qchatupk_process_rdw
===========================================================================*/
void qchatupk_process_rdw(void);

// drx.c
/*===========================================================================
FUNCTION qchatupk_drx_sleep_ok 
===========================================================================*/
boolean qchatupk_drx_sleep_ok(void);

/*===========================================================================
FUNCTION qchatupk_l1m_process_local_cmd_cphy_setup
===========================================================================*/
void qchatupk_l1m_process_local_cmd_cphy_setup(l1m_local_cmd_type *cmd_ptr);


//mcalwcdma_edl.c
boolean qchatupk_mcalwcdma_edl_cfg_fw_in_same_frame(void);

/*===========================================================================
FUNCTION qchatupk_wcdma_hfn_mismatch_workaround_enabled()
===========================================================================*/
boolean qchatupk_wcdma_hfn_mismatch_workaround_enabled(void);










#if 0 // Not used
/*===========================================================================
FUNCTION qchatupk_process_eul_dl_cfg
===========================================================================*/
boolean qchatupk_process_eul_dl_cfg(uint16 l1_setup_req_mask, l1_setup_cmd_type *l1_setup_cmd);

/*===========================================================================
FUNCTION qchatupk_process_eul_ul_cfg
===========================================================================*/
boolean qchatupk_process_eul_ul_cfg(uint16 l1_setup_req_mask, l1_setup_cmd_type *l1_setup_cmd);



// rrccu.c
/*===========================================================================
FUNCTION qchatupk_rrccu_transition_to_pch 
===========================================================================*/
boolean qchatupk_rrccu_transition_to_pch(void);

/*===========================================================================
FUNCTION qchatupk_process_dl_cctrch_req
===========================================================================*/
boolean qchatupk_process_dl_cctrch_req(uint16 l1_setup_req_mask, l1_setup_cmd_type *l1_setup_cmd);

/*===========================================================================
FUNCTION qchatupk_process_dl_phychan_add
===========================================================================*/
boolean qchatupk_process_dl_phychan_add(uint16 l1_setup_req_mask, l1_setup_cmd_type *l1_setup_cmd);

/*===========================================================================
FUNCTION qchatupk_process_ul_cctrch_req
===========================================================================*/
boolean qchatupk_process_ul_cctrch_req(uint16 l1_setup_req_mask, l1_setup_cmd_type *l1_setup_cmd);

/*===========================================================================
FUNCTION qchatupk_process_ul_phychan_add
===========================================================================*/
boolean qchatupk_process_ul_phychan_add(uint16 l1_setup_req_mask, l1_setup_cmd_type *l1_setup_cmd);

/*===========================================================================
FUNCTION qchatupk_process_hs_channel_config
===========================================================================*/
boolean qchatupk_process_hs_channel_config(uint16 l1_setup_req_mask, l1_setup_cmd_type *l1_setup_cmd);


#endif

#endif /* L1_QCHATUPK_H */
