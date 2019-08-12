#ifndef TDSEULCFG_H
#define TDSEULCFG_H

/*============================================================================
             WCDMA L1 HSUPA UL configuration related header file

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
HSUPA uplink physical channel configuration.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006-2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdseulcfg.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
1/12/11     weijunz Initial edit for TDSCDMA HSUPA
===========================================================================*/

/* <EJECT> */

/*===========================================================================

                      Define/undefine local features   

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_TDSCDMA_HSUPA
#include "tdsl1macdata.h"
#include "tdsl1rrcif.h"
#include "tdseul.h"
#include "tdseullog.h"
#include "tdsl1m_rcinit.h"



/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/

/* max num of EDPDCH supported- 25.306-670  4.5.4*/
#define   TDSEULCFG_MAX_NUM_EDPDCH                     4

/* Invalid page index. */
#define  TDSEUL_INVALID_PAGE  0xFF

/* Index of the other buffer in double buffers */
#define TDSEDPCH_DBL_BUF_OTHER_IDX(idx) ((idx+1) & 0x1)



// FIXME
#define FIQIRQFREE() TDS_INTFREE()
#define FIQIRQLOCK() TDS_INTLOCK()
#define FIQIRQLOCK_SAV(sav_var) TDS_INTLOCK_SAV(sav_var)
#define FIQIRQFREE_SAV(sav_var) TDS_INTFREE_SAV(sav_var)

/*EUL channel action types.These actions are used in mDSP config cmds*/
typedef enum 
{
  /*drop the channel*/
  TDSEULCFG_CHAN_DISABLE,
  
  /* add the channel*/
  TDSEULCFG_CHAN_ENABLE,
  
  /* reconfig the channel*/
  TDSEULCFG_CHAN_RECONFIG
  
} tdseulcfg_chan_action_enum_type;



/*The DPCH parameters that effect the building of semi static tables for EDPCH.
  This information is maintained even if E-DCH is not active.These parameters
  are received from uplink DCH module
*/
typedef struct
{
  /* TRUE if UL DPCH TFCS has non-zero data rate. Must be determined by DPCH 
     configuration module even if there is no E-DCH  change 
  */
  boolean                       dpdch_cfged;

  /* DPCCH slot format ( 0, 1, 2, 3 ONLY ) in normal mode as in 25.211-660
     Table 2.Must be determined by DPCH configuration module even if there 
     is no E-DCH  change 
  */
  uint32                        ul_dpcch_slot_fmt_nm;

} tdseulcfg_dpch_cfg_db_struct_type;


/*Struct type for E-DCH config parameters that are received from RRC as part
  of cphy setup req 
*/
typedef struct
{

  /* 25.331, 10.3.5.7d: If "rv0" is indicated, the UE shall only use 
     E_DCH RV index 0.Default value is "rvtable", in which case the UE 
     shall use an RSN based RV index. 
  */
  tdsl1_harq_rv_config_enum_type   harq_rv_config;
 
 
  /*Added for TDSCDMA HSUPA */
  /*Number of reference ETFCI for QPSK. 1..8*/
  uint8 num_ref_e_tfci_qpsk ;
  /*ETFCI refeence table for QPSK*/
  tdsl1_ref_e_tfci_struct_type ref_e_tfci_list_qpsk[TDSL1_MAX_REF_E_TFCI_LIST] ;
  /*Number of reference ETFCI for 16QAM. 1..8*/
  uint8 num_ref_e_tfci_16qam ;
  /*ETFCI refeence table for QPSK*/
  tdsl1_ref_e_tfci_struct_type ref_e_tfci_list_16qam[TDSL1_MAX_REF_E_TFCI_LIST] ;
  /*SNPL report type*/
  tdsl1_snpl_rep_enum_type snpl_rep_type ;
  /*PRX_DES_BASE. -112dbm ...-50dbm in step of 1dbm*/
  int8 prx_des_base ;
  /*if Beacon path loss estimation is used or not. */
  boolean beacon_pl_est ;
  /*E-PUCH TPC step size.1,2,3*/
  tdsl1_edl_tpc_step_size_enum_type tpc_step;
  /*Pebase power control  gap. 1...255. 255 means infinite and closed loop power control is always used*/
  uint8 p_base_contr_gap ;
  /*UL synchronization step size.1...8*/
  uint8 ul_sync_step ;
  /*UL synchronization frequency. 1...8*/
  uint8 ul_sync_freq ;
  /*Minimumm code rate. index 0..63. mapped to 0.055 to 1.0 in step of 0.055*/
  uint8 min_code_rate ;
  /*Maximumm code rate. index 0..63. mapped to 0.055 to 1.0 in step of 0.055*/
  uint8 max_code_rate ;
  /*Number of E-PUCH time slots*/
  uint8 num_epuch_ts ;
  tdsl1_epuch_ts_info_struct_type epuch_ts_cfg_list[TDSL1_MAX_EPUCH_TS] ;
  /*E-RUCCH info*/
  tdsl1_e_rucch_info_struct_type e_rucch_info ;
  /* E-RUCCH ASC info list */
  tdsl1_ul_asc_struct_type asc_info_erucch[TDSL1_UL_ASC_MAX];
} tdseulcfg_db_struct_type;

/* This structure contains the pointers to current and ordered config databases
   maintained for EDPCH signalled info , DPCH and HSDPA parameters that effect 
   semistatic tables of EDPCH.if there is no change for the hsdpa params then 
   the  new_hs_db_ptr is null . if there is no change for the dpch params then
   the new_dpch_db_ptr is null .

   if the new pointer of a channel is NULL then that channel information is not
   used in the tables building

   Initially all the current and new pointers are null & all the double buffers
   are filled with zeros
   
   When DPCH is started/recfged one of the DPCH double buffer is filled with the
   signaled params and the new dpch pointer points to the filled buffer 
   
   When EUL is started/recfged, one of the double buffer is filled with the 
   signaled params and the new eul pointer points to the filled buffer
   
   The above operations are done if the validation of the corresponding channel
   is successful.
   
   After the semi static tables are built,inside tdsedpch_process_cfg_data(),
   the current pointers of each channel are changed to point to the corresponding
   buffers(i.e new pointers if new pointer is not NULL) that are used in the 
   calculations & Make the new pointers of all the channels as NULL
*/
typedef struct
{

  /*pointer to the existing EPUCH active database that is already used in building
   the ss tables
  */
  tdseulcfg_db_struct_type   *curr_db_ptr;
  
  /*pointer to the newly received EPUCH database that contains the signalled EDPCH
   params
  */
  tdseulcfg_db_struct_type   *new_db_ptr;
  
  /*pointer to the existing DPCH active database that contains the DPCH params that
   are already used in building the EPUCH semistatic tables
  */
  tdseulcfg_dpch_cfg_db_struct_type  *curr_dpch_db_ptr;
  
  /*pointer to the newly received DPCH database that contains the DPCH params that
    effect the EPUCH semistatic tables and these params should be used in next ss
    table build
  */
  tdseulcfg_dpch_cfg_db_struct_type  *new_dpch_db_ptr;

} tdseulcfg_info_struct_type;


/*EUL EPUCH states. These states are used only when EUL uplink module is
  in TDSEULCFG_E_UL_EPUCH_TX 
*/
typedef enum
{
  /* EPUCH default state */
  TDSEULCFG_EPUCH_INIT,

  /* EPUCH is in set up */
  TDSEULCFG_EPUCH_STATE_SETUP,
  
  /* EPUCH is in normal tx state*/
  TDSEULCFG_EPUCH_STATE_TX,
  
  /* EPUCH is disabled due to out of sync */
  TDSEULCFG_EPUCH_STATE_DISABLED,
  
} tdseulcfg_epuch_state_enum_type;

/* EUL uplink  states maintained in uplink module*/
typedef enum 
{
  /*un-initialized state.This is the default EUL uplink state before 
    the TDSCDMA is active
  */
  TDSEULCFG_E_UL_UNINIT,
  
  /*initialized state. EUL uplink enters into this state whenever the 
    WCDMA is started and l1m init is called
  */
  TDSEULCFG_E_UL_INIT,
  
  /*EUL EDPCH active state. EUL uplink enters into this state when  EPUCH 
    is requested and leaves this state when EPUCH is dropped
  */
  TDSEULCFG_E_UL_EPUCH_TX,

} tdseulcfg_state_enum_type;


typedef enum
{
  TDSEUL_SNPL_TYPE_1,
  TDSEUL_SNPL_TYPE_2,
  TDSEUL_NO_SNPL_MEAS,
  TDSEUL_NUM_SNPL_TYPE
} tdsl1_eul_snpl_measure_type_enum_type;

extern tdsl1_eul_snpl_measure_type_enum_type  tdseulsg_snpl_meas_type;

#define TDSEUL_GET_SNPL_MEAS_TYPE  ( tdseulsg_snpl_meas_type )

#define TDSEUL_DEFAULT_SNPL_INDEX        9


/*===========================================================================

                     PUBLIC GLOBAL DATA DECLARATIONS

===========================================================================*/

/* EDPCH Phychan drop pending flag */
extern boolean tdseulcfg_e_ul_drop_pending ;

/* Initial EUL state is Un-Initialized */
extern tdseulcfg_state_enum_type tdseulcfg_e_ul_state;

/* EDPCH state maintained during EUL uplink module is EDPCH TX state. */
extern tdseulcfg_epuch_state_enum_type tdseulcfg_epuch_state;

/* pointer to active E-DCH config parameters double buffer and index are define
 in eulcfg.c 
 */
extern tdseulcfg_db_struct_type *tdseulcfg_curr_phychan_cfg_db_ptr;

/* indicate whether to activate SS tables */
extern boolean tdseulcfg_activate_ss_tables_pending;



extern uint32 tdseulcfg_curr_phychan_cfg_db_idx;

extern tdseulcfg_db_struct_type  tdseulcfg_phychan_cfg_db_info[TDSEUL_MAX_CFG_DB_COUNT];

extern uint32 tdseulcfg_ord_phychan_cfg_db_idx;

/* Local Mutex for the protection of EUL cfg params in ISR and task context*/
#define EUL_CB_PENDING_BTMSK_INTLOCK() REX_ISR_LOCK(&tdseul_cbfunc_pending_bitmask_mutex)
#define EUL_CB_PENDING_BTMSK_INTFREE() REX_ISR_UNLOCK(&tdseul_cbfunc_pending_bitmask_mutex)
#define EUL_CFG_INTLOCK() REX_ISR_LOCK(&tdsedpch_params_mutex)
#define EUL_CFG_INTFREE() REX_ISR_UNLOCK(&tdsedpch_params_mutex)
#define EUL_STATE_INTLOCK() REX_ISR_LOCK(&tdsedpch_state_params_mutex)
#define EUL_STATE_INTFREE() REX_ISR_UNLOCK(&tdsedpch_state_params_mutex)


/*===========================================================================

                     FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION        TDSEULCFG_EUL_DCH_CLEANUP

FILE NAME       

DESCRIPTION     
  This is the DCH state cleanup routine for the HSUPA uplink controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
//boolean tdseulcfg_dch_cleanup(tdsl1m_state_enum_type next_state) ;
//boolean tdseulcfg_dch_cleanup(uint8 next_state) ;

/*===========================================================================
FUNCTION        TDSEULCFG_EUL_INIT

FILE NAME       

DESCRIPTION     
  This is the one time initialization function for the EUL uplink controller. 
  It initializes the EUL state and calls the eul encoder driver initializer.
  This function is called from tdsulstates_ul_init .

DEPENDENCIES    
  None

RETURN VALUE    
  None

SIDE EFFECTS    
  None
===========================================================================*/
void tdseulcfg_eul_init(void);

/*===========================================================================
FUNCTION        TDSEULCFG_EUL_IDLE_INIT

FILE NAME       

DESCRIPTION     
  This is the IDLE state initializer for eul uplink controller.This function 
  invokes the eul enc driver cleanup routine if the eul state is active.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void tdseulcfg_eul_idle_init(void) ;


/*===========================================================================
FUNCTION        E_UL_PHYCHAN_CFG_VALIDATE

DESCRIPTION     This function validates that the operation on  EDPCH channel as
                specified by the parameters referenced by the setup command can
                be carried out successfully, i.e. that the parameters are valid.

DEPENDENCIES    None

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdseulcfg_phychan_cfg_validate(tdsl1_setup_cmd_type *setup);

/*===========================================================================
FUNCTION        TDSEULCFG_PROCESS_CPHY_SETUP_REQ

DESCRIPTION     This function process the operation on EDPCH channel as specified
                by the parameters referenced by the setup command.

DEPENDENCIES    None

RETURN VALUE    TRUE if the command is carried out successfully, FALSE otherwise

SIDE EFFECTS    EDPCH shall be configured/reconfigured/stopped
===========================================================================*/
extern boolean tdseulcfg_process_cphy_setup_req( uint8 e_mask, tdsl1_e_ul_info_struct_type *cmd);

/*===========================================================================
FUNCTION        TDSEULCFG_EDPCH_DROP_CMD

DESCRIPTION     This function starts the process of dropping an active 
                EDPCH channel.This function will cause the following to happen
                    0) Set E-DPCH cleanup flag in this function
                    1) FW and HW E-DPCH cleanup in FIQ
                   
DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    The uplink module will start the process of dropping the 
                EDPCH channel.
===========================================================================*/
extern void tdseulcfg_epuch_drop_cmd(void);

/*===========================================================================
FUNCTION        TDSEULCFG_SET_UL_DPCH_INFO

DESCRIPTION     This function passes ul_dpcch_slot_fmt_nm and UL DPDCH presence
                info to EUL UL module at config time

DEPENDENCIES    Called by R99 UL DPCH add or reconfig module

RETURN VALUE    None

SIDE EFFECTS    EUL UL config params will update
===========================================================================*/
extern void tdseulcfg_set_ul_dpch_info(uint32 ul_dpcch_slot_fmt_nm,
                                        boolean dpdch_cfged);

/*===========================================================================
FUNCTION        TDSEULCFG_BUILD_SS_TABLES

DESCRIPTION     This function passes the ss table depended cfg information
                maintained and after completion of the table building, the 
                function switches the config pointers

DEPENDENCIES    This function is called after successful validation of cfg
                data for DPCH,HS,EUL channel 

RETURN VALUE    None

SIDE EFFECTS    SS tables are built
===========================================================================*/
extern void tdseulcfg_config_build_ss_tables(void);

/*===========================================================================
FUNCTION        TDSEULCFG_SWITCH_SS_TABLE_CFG_PTRS

DESCRIPTION     This function switches the current config pointers to ordered
                config pointers used in the current ss table build

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void tdseulcfg_switch_ss_table_cfg_ptrs(void);

extern void tdseulcfg_activate_cfg_params(uint16 sub_frame_num,uint32 reg_id);



/*=========================================================================

FUNCTION        TDSEULCFG_CLEANUP

DESCRIPTION     This routine re-initializes the eul encoder driver. It aborts
                all Tx processing in the encoder. It commands the mDSP to stop
                EDPCH channel processing.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

=========================================================================*/
extern void tdseulcfg_e_ul_cleanup( void );

/*=========================================================================

FUNCTION        TDSEULCFG_IS_EUL_ACTIVE

DESCRIPTION     This routine checks whether EUL uplink is already active or not

DEPENDENCIES    None.

RETURN VALUE    returns TRUE if EUL uplink module is in DPCH_TX or ERROR
                state else returns FALSE.

SIDE EFFECTS    None.

=========================================================================*/

extern boolean tdseulcfg_is_eul_active(void);



/*=========================================================================

FUNCTION        TDSEULCFG_CFG_REUSE_ALL_SS_TABLES

DESCRIPTION     This function determines whether saved SS tables can all
                be reused in case of reverting after HHO falure and WCDMA
                resume after W->G failure.

DEPENDENCIES    Must be called by EDPCH module in table building command.

RETURN VALUE    boolean

SIDE EFFECTS    None.

=========================================================================*/
extern boolean tdseulcfg_reuse_all_ss_tables(void);

/*===========================================================================
FUNCTION        TDSEULCFG_ENABLE_SS_TABLE_PENDING_FLAG

DESCRIPTION     This function will enable the pending SS table flag(CR 115236)
                to switch to the new built SS tables for any UL DPCH reconfig
DEPENDENCIES    none
  

RETURN VALUE    none
  

SIDE EFFECTS    none
  
===========================================================================*/

extern void tdseulcfg_enable_ss_table_pending_flag(void);

#ifdef FEATURE_TDSCDMA_DSDA
/*=========================================================================

FUNCTION        tdseulcfg_get_hsupa_cfg_timeslot_bitmask

DESCRIPTION     This function returns bit mask of RRC configured HSUPA timeslot

DEPENDENCIES    Must be called when E-PUCH config is done.

RETURN VALUE    Timeslot bitmap. From LSB, Bit 15-14 for TS#0, Bit 13-12 for TS#1, 
?-, Bit 3-2 for TS#6
00 - idle, 11 -> HSUPA UL timeslot configured by RRC 

SIDE EFFECTS    None.

=========================================================================*/
extern uint16 tdseulcfg_get_hsupa_cfg_timeslot_bitmask(void);
#endif
#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        EUL_PROCESS_UL_CHAN_CFG_LOG_ON_DEMAND

FILE NAME       eulcfg.c

DESCRIPTION     This funtion creates an eul ul chan cfg info log packet and submits it
                to QXDM on demand

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void tdseulcfg_e_ul_process_chan_cfg_log_on_demand(void);
#endif /* end of FEATURE_L1_LOG_ON_DEMAND*/

boolean tdseul_module_drop(void) ;

extern void tdseul_init_mutex(void);

#endif /*end of FEATURE_TDSCDMA_HSUPA*/
#endif/*end of TDSEULCFG_H*/
