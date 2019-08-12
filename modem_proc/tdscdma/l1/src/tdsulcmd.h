#ifndef TDS_ULCMD_H
#define TDS_ULCMD_H

/*===========================================================================
                          T D S U L C M D . H

GENERAL DESCRIPTION
  This header file contains the MACRO definitions, variables and functions
  statement for tdsulcmd.c
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2000 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsulcmd.h#1 $ 
  $DateTime: 2016/12/13 07:58:19 $ 
  $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
12/01/10    cdf/cxc File created.
                    
===========================================================================*/
#include "tdscdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "tdsl1def.h"
#include "tdsl1sapcommon.h"
#include "tdsl1ulcfg.h"
#include "tdsl1rrcif.h"
#include "tdsl1macif.h"
#include "tdsl1mtask.h"
#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
#include "tdsullog.h"
#endif

/* ----------------------------------------------------------------------- */
/*                          Macro and Constant                             */
/* ----------------------------------------------------------------------- */

/* if the real TFCI of ref TFCI for a computed gain factor indicates Invalid value
   then the ref tfci is not signalled or sent as calculated */
#define   TDSL1_INVALID_REF_TFCI   0xFF

/* Maximum No. of Transport Channels for RACH Channel */
#define   TDSL1_MAX_NUM_TRCH_FOR_RACH    1

/* current r4 physical channel in PRACH state */
#define TDSL1_PRACH_PHYCHANN_COUNT  2
#define TDSL1_DPCH_PHYCHANN_COUNT 1
#define TDSL1_DPCH_ERUCCH_PHYCHANN_COUNT 3
#define TDSL1_HHO_RESYNC_PHYCHANN_COUNT 2

/* ----------------------------------------------------------------------- */
/*                         Structure Definition                            */
/* ----------------------------------------------------------------------- */

/* Local Command for UL Physical Channel Setup */
typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type           hdr;

  tdsl1_ul_phychan_enum_type         phychan_type;

} tdsulcmd_cphy_setup_local_cmd_type;

/* This is the generic command structure used for Uplink physical channel add,
 * cfg, and drop command.
 */
typedef struct
{
  /* request mask */
  uint16 req_mask;

  /* Maximum allowed Tx power in dBm (-50..33)
   * See 25.331 v9.1.0 10.3.6.39*/
  int8 max_tx_pwr;

  /* data for CPHY_UL_CCTRCH_REQ */
  tdsl1_ul_cctrch_cfg_struct_type  *ul_cctrch_info_ptr;

  /* data for CPHY_UL_PHYCHAN_ADD_REQ */
  tdsl1_ul_phychan_enum_type  add_ul_phychan;
  tdsl1_ul_phych_cfg_struct_type    *add_ul_phychan_info_ptr;

  /* data for CPHY_UL_PHYCHAN_DROP_REQ */
  tdsl1_ul_phychan_enum_type  drop_ul_phychan;

  /* data for CPHY_UL_PHYCHAN_CFG_REQ.  recfg_ul_phychan is the uplink physical
  channel to be reconfigured.  It is assumed that it will be reconfigured
  to use the CCTrCH referred to in the CPHY_UL_CCTRCH_REQ also included
  in the command packet */
  tdsl1_ul_phychan_enum_type recfg_ul_phychan;
  tdsl1_ul_phych_cfg_struct_type *recfg_ul_phychan_info_ptr;

  tdsmac_ul_tfcs_type  *tfcs_ptr;

} tdsulcmd_cphy_setup_cmd_type;

/* + defang 02/18/2011 */
#ifdef FEATURE_TDSCDMA_HSUPA
/* Local command structure for E-RUCCH command */
typedef struct
{
  /* local command header type */
  tdsl1def_l1_local_cmd_hdr_type       hdr;

  /* ASC index selected by MAC */
  uint8 asc_index;

  /* E-RUCCH configuration */
  tdsl1_e_rucch_info_struct_type      *erucch_cfg;
} tdsl1_eul_e_rucch_tx_cmd_type;
#endif
/* - defang 02/18/2011 */

/* ----------------------------------------------------------------------- */
/*                          Variable Statement                             */
/* ----------------------------------------------------------------------- */

/* Calculated betaD table */
extern uint16                           tdsl1_ul_gain_factor[TDSL1_UL_TFC_MAX];

/* Status for each TFC based on rate matching calculated:
 * 0: allowed
 * 1: not allowed, because the rate matched size is too large to fit in
 *    available physical channels
 */
extern tdsl1_ul_rm_state_enum_type      tdsl1_ul_rm_state[TDSL1_UL_TFC_MAX];


/* Indicate whether use the tdsl1_tadv is valid */
extern boolean tdsl1_use_init_tadv;
extern boolean tdsl1_is_ho_recovery;

/* Tadv in 1/8 chip.*/
extern int16 tdsl1_tadv;


/* ----------------------------------------------------------------------- */
/*                          Function Statement                             */
/* ----------------------------------------------------------------------- */


/*===========================================================================
FUNCTION        tdsulcmd_phychan_add_is_valid

DESCRIPTION     This function validates that the addition of the physical
                channel as specified by the parameters referenced by the
                command can be carried out successfully, i.e. that hardware
                resources are available and the parameters are valid.

DEPENDENCIES    The physical channel database and command parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsulcmd_phychan_add_is_valid
(
  tdsulcmd_cphy_setup_cmd_type *cmd
);


/*===========================================================================
FUNCTION        tdsulcmd_phychan_add_cmd

DESCRIPTION     This function adds UL PRACH/DPCH physical channel configuration
                parameters transferred in the Cphy_Setup_Req and 
                Cphy_Cell_Trans_Req commands. 

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of adding the
                specified physical channel.
===========================================================================*/
extern void tdsulcmd_phychan_add_cmd
(
  tdsulcmd_cphy_setup_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION        tdsulcmd_phychan_recfg_cmd

DESCRIPTION     This function reconfigures UL DPCH configuration parameters 
                transferred in the Cphy_Setup_Req commands.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of DPCH reconfiguration
===========================================================================*/
extern void tdsulcmd_phychan_recfg_cmd
(
  tdsulcmd_cphy_setup_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION        tdsulcmd_phychan_ctrl_cmd

DESCRIPTION     This function process UL outer loop power control, timing 
                advance and SBGP parameters transferred in the 
                CPHY_UL_PHY_CHANNEL_CTRL_REQ commands.

DEPENDENCIES    Receive CPHY_UL_PHY_CHANNEL_CTRL_REQ from RRC.

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void tdsulcmd_phychan_ctrl_cmd
(
  tdsl1_ul_phych_control_cmd_type *cmd_ptr
);


/*===========================================================================
FUNCTION        tdsulcmd_phychan_hho_resync_cmd

DESCRIPTION     This function configures FPACH and UpPCH parameters which is
                used for random access in HHO re-sync procedure. These parameters
                are transferred in the CPHY_SETUP_REQ commands.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of random access procedure
                for HHO re-sync.
===========================================================================*/
extern void tdsulcmd_phychan_hho_resync_cmd
(
  tdsulcmd_cphy_setup_cmd_type *cmd_ptr
);

#ifdef FEATURE_TDSCDMA_HSUPA
/*===========================================================================
FUNCTION        tdsulcmd_e_rucch_cfg_cmd

DESCRIPTION     This function configures FPACH and UpPCH parameters which is
                used for E-RUCCH procedure. These parameters are passed from
                EUL via L1M command.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of E-RUCCH transmission.
===========================================================================*/
void tdsulcmd_e_rucch_cfg_cmd
(
  tdsl1_eul_e_rucch_tx_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION        tdsulcmd_e_rucch_abort_cmd

DESCRIPTION     This function abort random access procedure for E-RUCCH 
                transmission.

DEPENDENCIES    E-RUCCH can be aborted only if UL SYNC is not finished.

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void tdsulcmd_e_rucch_abort_cmd(void);

#endif

/*===========================================================================
FUNCTION        tdsulcmd_cctrch_cfg_is_valid

DESCRIPTION     This function validates that the reconfiguration of the
                CCTrCH as specified by the parameters referenced
                in the command can be carried out successfully.

DEPENDENCIES    The CCTrCH parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsulcmd_cctrch_cfg_is_valid
(
  tdsulcmd_cphy_setup_cmd_type *cmd
);

/*===========================================================================

FUNCTION tdsulcmd_compute_gain_factors

DESCRIPTION
  This function computes the gain factor Bd as described in 3GPP TS 25.224,
  4.2.2.3.1 Gain factors.

  These computed gain factors for the passed-in TFC use the signalled gain
  factors of a reference TFC in the computation.  A TFCS may contain up to
  4 reference TFCs, as shown in 3GPP TS 25.331:10.3.5.8 Power Offset Information.

DEPENDENCIES
  Rate matching processing is done before calculation gain factors.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tdsulcmd_compute_gain_factors(void);

/*===========================================================================
FUNCTION        tdsulcmd_cctrch_cfg_cmd

DESCRIPTION     This function starts the process for reconfiguring an
                active CCTrCH.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of reconfiguring
                the specified CCTrCH.
===========================================================================*/
extern void tdsulcmd_cctrch_cfg_cmd
(
  tdsulcmd_cphy_setup_cmd_type *cmd
);


/*===========================================================================
FUNCTION        tdsulcmd_phychan_cfg_is_valid

DESCRIPTION     This function validates that the reconfiguration of the
                physical channel as specified by the parameters referenced
                in the command can be carried out successfully, i.e. that
                the channel is currently active and that the parameters are
                valid.

DEPENDENCIES    The physical channel database and command parameters

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
boolean tdsulcmd_phychan_cfg_is_valid
(
  tdsulcmd_cphy_setup_cmd_type *cmd_ptr
);


/*===========================================================================
FUNCTION        tdsulcmd_phychan_drop_is_valid

DESCRIPTION     This function validates whether the drop process can be carried 
                out successfully.
                
DEPENDENCIES    The uplink state PRACH_TX, DPCH_TX and ERROR are allowed.

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsulcmd_phychan_drop_is_valid(void);


/*===========================================================================
FUNCTION        tdsulcmd_phychan_drop_cmd

DESCRIPTION     This function starts the process of dropping an active 
                physical channel.
                
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of dropping the
                physical channel in tdsenc_cleanup().
===========================================================================*/
extern void tdsulcmd_phychan_drop_cmd(void);


/*===========================================================================
FUNCTION        UL_CELL_TRANS_DROP_CMD

DESCRIPTION     This function starts the process of dropping a PRACH
                physical channel. This is called when performing a
                cell transition.

DEPENDENCIES    The command parameters

RETURN VALUE    Boolean if a physical channel is being dropped.
                False otherwise.

SIDE EFFECTS    The uplink module will start the process of dropping the
                PRACH if it is already set up.
===========================================================================*/
boolean tdsulcmd_cell_trans_drop_cmd( void );

/*===========================================================================
FUNCTION tdsulcmd_save_ciphering_keys

DESCRIPTION
  This function save the ciphering key information for uplink
  In case it is missing after tx block diable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  tdsulcmd_save_ciphering_keys(
  /* DL ciphering info structure */
  tdsl1_cipher_key_info_struct_type *ul_cipher_info);


/*===========================================================================
FUNCTION UL_SET_CIPHERING_KEYS

DESCRIPTION
  This function validates and sets the ciphering key information for uplink
  ciphering operation.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void tdsulcmd_set_ciphering_keys(
  /* DL ciphering info structure */
  tdsl1_cipher_key_info_struct_type *ul_cipher_info);

/*===========================================================================
FUNCTION UL_VALIDATE_CIPHERING_KEY_REQ

DESCRIPTION
  This function validates the ciphering key information for uplink
  ciphering operation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE validate result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsulcmd_validate_ciphering_key_req(
  /* DL ciphering info structure to validate the information */
  tdsl1_cipher_key_info_struct_type *ul_cipher_info);



/*===========================================================================
FUNCTION        tdsulcmd_set_target_cell_info

DESCRIPTION     This fucntion is used to set target cell frequency and cell 
                parameter id.


DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void tdsulcmd_set_target_cell_pri_freq_and_cpi(uint16 primary_freq, uint8 cpid);



/*===========================================================================
FUNCTION        tdsulcmd_get_last_ul_dpch_ts

DESCRIPTION     This fucntion is used to provide the last UL DPCH timeslot to L1M.
                L1M compare it with the 1st DL DPCH timeslot to decide BHO type:
                T=0ms or T=80ms.


DEPENDENCIES    None

RETURN VALUE    uint8
                Timeslot number, (1..5)

SIDE EFFECTS    None

===========================================================================*/

extern uint8 tdsulcmd_get_last_ul_dpch_ts(tdsl1_setup_cmd_type* cmd);

/*===========================================================================
FUNCTION        UL_ASU_CFG_DB_COPY_CMD

DESCRIPTION     This function copies UL chan cfg for ASU to avoid re-config

DEPENDENCIES    RRC shall make sure NO change in the chan cfg.
                This function shall be called by L1M to replace ASU caused
                re-config. 

RETURN VALUE    None

SIDE EFFECTS    NONE
===========================================================================*/
void tdsulcmd_asu_cfg_db_copy_cmd(tdsl1_setup_cmd_type* l1_setup_cmd_ptr);


/*===========================================================================
FUNCTION        tdsulcmd_prach_cfg_is_valid

DESCRIPTION     This function validates PRACH physical channel configuration
                parameters.

DEPENDENCIES    PRACH configuration should be set up by L3 (RRC)

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsulcmd_prach_cfg_is_valid
(
  tdsl1_ul_prach_cfg_struct_type *ul_prach_cfg_ptr
);

/*===========================================================================
FUNCTION        tdsulcmd_dpch_cfg_is_valid

DESCRIPTION     This function validates that DPCH configuration.

DEPENDENCIES    The DPCH database and command parameters

RETURN VALUE    TRUE if the command can be carried out and validation is OK, 
                FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern  boolean tdsulcmd_dpch_cfg_is_valid
(
  tdsl1_ul_dpch_cfg_struct_type *ul_dpch_cfg_ptr
);

#ifdef FEATURE_TDSCDMA_DSDA
/*=========================================================================
FUNCTION tdsulcmd_get_ul_tx_power_for_rf_coex

DESCRIPTION
  DSDA use this interface to query ul tx power for RF COEX

DEPENDENCIES
  None.

RETURN VALUE
  filtered TX power in DB domain

SIDE EFFECTS
  None.
=========================================================================*/
int16 tdsulcmd_get_ul_tx_power_for_rf_coex(void);

/*=========================================================================
FUNCTION tdsulcmd_get_ul_tx_power_for_sar

DESCRIPTION
  DSDA use this interface to query ul tx power for SAR

DEPENDENCIES
  None.

RETURN VALUE
  Linear filtered TX power

SIDE EFFECTS
  None.
=========================================================================*/
int16 tdsulcmd_get_ul_tx_power_for_sar(void);

#endif /*FEATURE_TDSCDMA_DSDA*/


/*=========================================================================
FUNCTION tdsulcmd_get_ul_tx_power

DESCRIPTION
  DSDA use this interface to query ul tx power 

DEPENDENCIES
  None.

RETURN VALUE
  filtered TX power in Q4 DB domain

SIDE EFFECTS
  None.
=========================================================================*/
int8 tdsulcmd_get_ul_tx_power(void);

/*===========================================================================
FUNCTION        tdsulcmd_get_active_ul_phychann_num

DESCRIPTION     This function gets the number of current R4 tx physical channel.

DEPENDENCIES    None

RETURN VALUE    uint8 number of physical channels.

SIDE EFFECTS    None
===========================================================================*/
uint8 tdsulcmd_get_active_ul_phychann_num(void);


#endif  /* TDS_ULCMD_H */
