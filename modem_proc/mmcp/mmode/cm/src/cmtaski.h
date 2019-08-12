#ifndef CMTASKI_H
#define CMTASKI_H
/*===========================================================================

         C A L L   M A N A G E R   T A S K   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMTASK.C


Copyright (c) 2001 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmtaski.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/13/13   jvo     Added cmtask_is_init_complete()
09/04/13   mh/jvo  Mainline the modem statistics code
08/13/13   sm      Timer signal for SRVCC cache expiry 
07/19/13   jvo     Remove all MDM/MSM Fusion features
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
10/09/12   skk     Implemented fetch based mechanism for 3GPP RSSI
04/04/12   xs      Codec info propagation
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/02/12   vk      Added feature to get DCH cell info.
01/09/12   mj      Register with TDS-RRC for cell change indication
01/06/12   gm      Data synchronization changes for freefloating platform
07/29/11   rm      Assigning correct value for CM_STATS_NAS_SIG
05/19/11   rm      LTE Modem statistics
04/12/11   cl      MDM should not grant permission to MSM if HDR is pending
                   to be attempted for acquisition
03/21/11   rk      Adding declaration for sim state call back function
03/03/11   gm      Moved CM_M2M_Q_SIG under RPC client/server feature
03/03/11   gm      Added separate queue processing for M2M indications
01/12/10   rm      FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE changes for DSDS
12/23/10   sv      Integrate DSDS feature
05/21/10   ak      Mainlining memory optimization changes.
04/29/10   rm      Adding Modem Statistics changes for 1x/HDR
03/17/10   cl      Add support for CM_PH_EVENT_PRL_INIT to indicate PRL is loaded
10/22/09   rm      Adding Modem Statistics changes
10/14/09   sg      ZI memory reduction. Mmode should call new Heap Manager API to 
                   allocate memory dynamically.
12/22/08   fj      Defined CM_MSGR_Q_SIG for message router.
06/10/09   ks      Include new RR interface to support camped cell info
04/23/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes.
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
09/16/08   pk      Added support for WLAN UOoS
09/04/08   dm/sv   Added callback function for the WPS result
07/18/08   ks      Added new info for Current Operator's Cell ID and 
                   Neighbors Cell IDs
04/01/06   ka      Changes for emerg serv category during orig, aborting 
                   manual search and returning current band with srv ind.                  
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/06/06   ic      Lint cleanup  
12/07/05   ka      Exporting cm_si_network_list_cb
10/13/05   ic      Externalized cmtask_crit_sect_ptr()
09/08/04   jqi     More ps dormant optimization.
08/27/04   jqi     Added ps dormant optimization.
06/29/04   jqi     Avoid using the value of common signals.
06/14/04   jqi     Made CM MMGSDI calls to be asynchronouse.
06/09/04   dk      Added CM Heap support.
04/13/04   ic      Added prototype of cm_si_acq_fail_cb()
03/24/04   ic      Added prototype of cm_si_ok_to_orig_cb()
02/06/04   ws      Initial jaguar/mainline merge.
01/21/03   ws      Updated copyright information for 2003
11/01/02   vt      Rearranged CM signals.
10/07/02   vt      Defined signals for regproxy.
07/08/02   vt      Changed the definitions of SIG's to avoid conflicts with
                   TASK signal definitions.
04/16/02   AT      Merged Advice of Charge with GSDI and UI.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
09/13/01   RI      Initial Release
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "mmcp_variation.h"
#include "cmtask.h"
#include "cmi.h"
#include "cmcall.h"
#include "rex.h"
#include "rr_cb_if.h"

#if defined(FEATURE_TDSCDMA)
#include "tdsrrccsp.h"
#endif


#ifdef FEATURE_CM_LTE
#include "memheap.h"   // for mem_heap_type
#endif
/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/*
** CM task signals
*/
#define CM_CLIENT_CMD_Q_SIG   0x0001           /* CM client command signal */
#define CM_RPT_Q_SIG          0x0002       /* CM lower layer report signal */

#define CM_TIMER_SIG          0x0004                  /* Time to update CM */
#define CM_RSSI_TIMER_SIG     0x0008      /* Time to check and update RSSI */
#define CM_RPT_TIMER_SIG      0x0010              /* Time to kick watchdog */
#define CM_GWL_RSSI_TIMER_SIG 0x0020      /* Time to check and update RSSI */

#define CM_REGPRX_CMD_Q_SIG   0x0040         /* CM RegProxy command q sig. */

#define CM_NV_SYNC_RESERVED   0x0080     /* CM_UI_NV_SIG - defined in cm.h */
#define CM_NV_ASYNC_SIG       0x0100          /* Asynchronous NV operation */

#define CM_ACM_TIMER_SIG      0x0400 /* Signal sent when ACM timer expires */
#define CM_CCM_TIMER_SIG      0x0800 /* Signal sent when CCM timer expires */
#define CM_RUIM_STATUS_SIG    0x0200                /* CM RUIM Done signal */

#define CM_GSDI_RESERVED_SIG  0x1000  /* Responds to synchronous responses */

#define CM_MMGSDI_REPORT_SIG  0x10000     /* Responds to MMGSDI operations */

/* removed CM_HDRMC_SIG , since its not being used */
/* when a request is sent to MMGSDI , 
    **timer will start with CM_MMGSDI_ABORT_TIMER_VALUE_SECS ,
    **if the response is not rcvd within the above time ,
    **below sig will be set , and while processing the sig , 
    **the callback will be called as MMGSDI ERROR happened */
#define CM_MMGSDI_ABORT_TIMER_SIG          0x20000              

#define CM_STATS_GSM_SIG    0x80000      /* CM STATS GSM signal */
#define CM_STATS_WCDMA_SIG  0x100000     /* CM STATS WCDMA signal */
#define CM_STATS_CDMA_SIG   0x200000     /* CM STATS CDMA signal  */
#define CM_STATS_HDR_SIG    0x400000     /* CM STATS HDRsignal    */
#define CM_STATS_DS_SIG     0x800000     /* CM STATS DS signal    */
#define CM_STATS_LTE_SIG    0x2000000    /* CM STATS LTE signal    */
#define CM_STATS_NAS_SIG    0x4000000    /* CM STATS NAS signal */
#define CM_STATS_NAS2_SIG   0x200000      /* CM STATS NAS signal */
#define CM_STATS_GSM2_SIG   0x400000      /* CM STATS GSM2 signal */
#define CM_STATS_GSM3_SIG   0x10000000    /* CM STATS GSM3 signal */
#define CM_STATS_NAS3_SIG   0x8000000      /* CM STATS NAS signal */
#define CM_STATS_WCDMA2_SIG 0x80000000    /* CM STATS WCDMA2 signal */



#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
#define CM_SRVCC_CACHE_SIG     0x20000000       
#endif

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
#define CM_ATTACH_COMPLETE_TIMER_SIG 0x40000000
#endif
/*
** Timer durations
*/
#define CM_RSSI_TIME          2240                             /* 320*7 ms */

#define CM_GWL_RSSI_TIME      3000                             /* 3000 ms */

#define CM_SRVCC_CACHE_TIME   100                             

/*
** Alias for signals
*/
#define CM_NV_SYNC_SIG        CM_UI_NV_SIG
  /* Signal for synchronous NV operation. Also used for UI initiated NV
  ** operation done in CM task context. Defined in cm.h as 0x0080 */

#define CM_SEGMENT_SIG        CM_CCM_TIMER_SIG
  /* Signal when segment is transferred. */


#define CM_MSGR_Q_SIG         0x40000        
  /* Signal for Message Router Q. */

/**--------------------------------------------------------------------------
** Externs
** --------------------------------------------------------------------------
*/
#ifdef CM_GW_SUPPORTED
extern rex_timer_type  cm_gwl_rssi_timer;
    /* RSSI checking timer */
#endif

/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

#ifdef CM_DEBUG
#error code not present
#else
#define cm_cmd_alloc(size) cm_cmd_alloc_debug(__LINE__, size)
#endif

/*===========================================================================

FUNCTION cm_cmd_alloc

DESCRIPTION
  Allocate a CM command buffer from the Heap.

DEPENDENCIES
  Heap must have been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If Heap is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_cmd_type *cm_cmd_alloc_debug
(
  #ifdef CM_DEBUG
  #error code not present
#endif
  
  uint32 line,
    /* Line number in the file above */

  size_t size
);


/*===========================================================================

FUNCTION cm_cmd_dealloc

DESCRIPTION
  Deallocate a CM command buffer by returning it to the
  heap.

DEPENDENCIES
  Heap must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_cmd_dealloc( void  *ptr );


#define cm_mem_malloc(x) cm_mem_malloc_debug(x, #x)

/*===========================================================================

FUNCTION cm_mem_malloc_debug

DESCRIPTION
  Allocate a chunk of Memory from the Heap. If CM heap is exhausted,
  then it tries to get memory from the TMC Heap.

DEPENDENCIES

   Heap is already initialized.

RETURN VALUE
   Pointer to the memory block.

SIDE EFFECTS
  none

===========================================================================*/
extern void *cm_mem_malloc_debug(

  size_t size,

  char *objname
    /* Used to see the name of the object being freed if "sizeof" operator is
    ** used.
    */
);

#ifndef T_WINNT
#if defined(FEATURE_QDSP6)
  /*Evaluates to the return address of the function using this macro, if caller_level is 0*/
  #define CM_MEM_ALLOC_CALLER_ADDRESS(caller_level) ((void *)__builtin_return_address(caller_level))
#else
  #define CM_MEM_ALLOC_CALLER_ADDRESS(caller_level) ((void *)__return_address())
#endif
#else
  #error code not present
#endif

#if(defined _ARM_ASM_)
#define cm_mem_free(x) cm_mem_free_debug(x, #x, __func__, __LINE__)
#elif (defined __GNUC__) || (defined T_WINNT)
#define cm_mem_free(x) cm_mem_free_debug(x, #x, __FUNCTION__, __LINE__)
#endif
/*===========================================================================

FUNCTION cm_mem_free_debug

DESCRIPTION
   This function returuns the specified memory back to the Heap.

   If the memory came from CM heap then it is returned their else
   it is returned to the TMC heap.

DEPENDENCIES
   CM and TMC heap must have been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_mem_free_debug
(
  
  void *ptr,
  
  char *objname,
    /* Name of object being freed. */

  const char* funcName,
    /* Used to see the name of the function */

  int lineNumber
    /* Used to see the line number */

);


/*===========================================================================

FUNCTION cm_cmd_queue

DESCRIPTION
  Queue a CM command in its appropriate CM command queue.

DEPENDENCIES
  All CM command queues must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_cmd_queue
(
  cm_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION cm_wait

DESCRIPTION
  Suspend CM from executing until at least one of the specified signals
  in the input mask is set.

  Note that this function keep reporting to the watchdog while waiting
  for the signal(s) to be set.

DEPENDENCIES
  cm_rpt_timer timer must have been defined with rex_def_timer().

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
extern rex_sigs_type cm_wait
(
  rex_sigs_type    mask
    /* Mask of signals to wait on */
);

/*===========================================================================

FUNCTION cm_si_ok_to_orig_cb

DESCRIPTION
  Ok To Orig callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever the Ok To Orig status changes from FALSE
  to TRUE while the origination mode is other than none.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_ok_to_orig_cb(

  sd_ss_e_type                     ss,
      /* System selection - MAIN or HDR.
      */

  const sd_si_ok_to_orig_s_type    *si_ok_to_orig_ptr
        /* System indications related to call origination 
        */

);

/*===========================================================================

FUNCTION cm_si_acq_fail_cb

DESCRIPTION
  Acquisition Failed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever acquisition fails (i.e., SD have
  attempted full acquisitions on all systems with no success).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_acq_fail_cb_v02(

  sd_ss_e_type              ss,
        /* System selection - MAIN or HDR.
        */

  sd_ss_mode_pref_e_type     mode_pref
  /* Acq failed on above mode*/

);


/*===========================================================================

FUNCTION cm_si_data_suspend_cb

DESCRIPTION
  Data suspend state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever data suspend state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_data_suspend_cb(

        boolean             data_suspend,
           /* New Data suspend state.
           */
      
        sd_ss_e_type        ss
           /* Stack on which the data suspend indication is being sent
           */
);



/*===========================================================================

FUNCTION cm_si_sim_state_cb

DESCRIPTION
  SIM state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever SIM state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_sim_state_cb(

     sys_sim_state_e_type       sim_state,
        /* New sim State.
        */

     sd_ss_e_type               ss
);

/*===========================================================================

FUNCTION cm_cs_emerg_scan_fail_cb

DESCRIPTION
  CS EMERG Scan fail report from SD 

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever when one round scan finishes and no CS service
  found

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_cs_emerg_scan_fail_cb(
    sd_ss_e_type ss
);

/*===========================================================================

FUNCTION cm_cdma_bsr_stat_chgd_cb

DESCRIPTION
  BSR progress status changed callback from SD.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever BSR status changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_cdma_bsr_stat_chgd_cb(
     boolean bsr_status
);

/*===========================================================================

FUNCTION get_cm_tcb

DESCRIPTION
  Gets pointer MMOC TCB.
  
DEPENDENCIES
  set_cm_tcb.

RETURN VALUE
  Pointer to MMOC TCB.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type* get_cm_tcb(void);

/*===========================================================================

FUNCTION cmtask_crit_sect_ptr

DESCRIPTION

  Return pointer to CM critical section. 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern rex_crit_sect_type* cmtask_crit_sect_ptr(void);

/*===========================================================================

FUNCTION cm_si_network_list_cb

DESCRIPTION
  SIM state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever SIM state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_si_network_list_cb(

    sys_plmn_id_s_type             *rplmn_ptr,
       /* Pointer to a structure containing the last registered PLMN id.
       */

    sd_ss_e_type                   ss,

    sys_found_plmn_list_u_type   *plmn_list_ptr,
       /* Pointer to a structure containing the list of available PLMNs.
       */

    sys_plmn_list_status_e_type    plmn_list_status
       /* Gives the status of plmn list request (success/abort).
       */

);

#if defined(FEATURE_WCDMA)
/*===========================================================================
FUNCTION cm_cell_info_rrc_cb

DESCRIPTION
  RRC cell change indication callback function

  CM registers this function with RRC during power-up initialization.

  RRC calls on this function whenever Cell_info changes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cm_cell_info_rrc_cb(

   cell_info_type   cell_info   
       /* Cell Info update from rrc
       */
);

#endif /* FEATURE_WCDMA */

#if defined(FEATURE_TDSCDMA)
/*===========================================================================
FUNCTION cm_cell_info_tdsrrc_cb

DESCRIPTION
  TD-SCDMA RRC cell change indication callback function

  CM registers this function with TDS RRC during power-up initialization.

  TDS RRC calls on this function whenever Cell_info changes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cm_cell_info_tdsrrc_cb(

   tdscell_info_type   tds_cell_info   
       /* Cell Info update from TDS rrc
       */
);

#endif /* FEATURE_TDSCDMA */

#if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK))
/*===========================================================================
FUNCTION cm_cell_info_rr_cb

DESCRIPTION
  RR cell change indication callback function

  CM registers this function with RR during power-up initialization.

  RR calls on this function whenever Cell_info changes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cm_cell_info_rr_cb(

   #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
   eng_mode_scell_info_t scell_info
   #else
   rr_cell_info_t cell_info   
   #endif /* FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE */
       /* Cell Info update from rr
       */
);


#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================
FUNCTION cm_hybr_2_cell_info_rr_cb

DESCRIPTION
  RR cell change indication callback function

  CM registers this function with RR during power-up initialization.

  RR calls on this function whenever Cell_info changes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cm_hybr_2_cell_info_rr_cb(

   #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
   eng_mode_scell_info_t scell_info
   #else
   rr_cell_info_t cell_info   
   #endif /* FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE */
       /* Cell Info update from rr
       */
);
#endif /* FEATURE_MMODE_DUAL_SIM */

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/*===========================================================================
FUNCTION cm_hybr_3_cell_info_rr_cb

DESCRIPTION
  RR cell change indication callback function

  CM registers this function with RR during power-up initialization.

  RR calls on this function whenever Cell_info changes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cm_hybr_3_cell_info_rr_cb(

   #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
   eng_mode_scell_info_t scell_info
   #else
   rr_cell_info_t cell_info   
   #endif /* FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE */
       /* Cell Info update from rr
       */
);
#endif /* FEATURE_MMODE_TRIPLE_SIM */


#endif /* FEATURE_GSM && FEATURE_GSM_RR_CELL_CALLBACK */

void cm_gsm_codec_info_cb(sys_codec_ho_info codec_ho_info);
void cm_wcdma_codec_info_cb(sys_codec_ho_info codec_ho_info);
void cm_td_codec_info_cb(sys_codec_ho_info codec_ho_info);

/*===========================================================================

FUNCTION cm_srv_lost_cb

DESCRIPTION
  Acquisition Failed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever acquisition fails (i.e., SD have
  attempted full acquisitions on all systems with no success).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_srv_lost_cb(

  sd_ss_e_type              ss
        /* System selection - MAIN or HDR.
        */
);

/*===========================================================================

FUNCTION cmtask_is_there_a_high_priority_activity

DESCRIPTION
  Checks if there is a high priority activity (voice/emergency/CS_DATA call)  in UE.

DEPENDENCIES
  None

RETURN VALUE
  TRU if if any voice/emergency/CS_DATA call is active

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmtask_is_there_a_high_priority_activity
(
  cm_ss_e_type        ss,

  cm_act_type_e_type  cmp_act_type
);

/*===========================================================================

FUNCTION cmtask_orig_para_change_mode_pref

DESCRIPTION
  Changes the mode_pref of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmtask_orig_para_change_mode_pref(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_mode_pref_e_type         mode_pref,
    /* Mode pref of phone
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
);

/*===========================================================================

FUNCTION cmtask_get_cm_rpt_q

DESCRIPTION
  Provide ponter for report queue so that other modules can also queue
  reports to CM ( CMLL, CMWLL )

DEPENDENCIES

RETURN VALUE
  Pointer to cm report queue.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_q_type* cmtask_get_cm_rpt_q( void );


#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cmtask_srvcc_cache_timer_start

DESCRIPTION
  Starts srvcc cache timer 

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
rex_timer_cnt_type cmtask_srvcc_cache_timer_start(void);
#endif


/*===========================================================================

FUNCTION cmtask_rssi_timer_start

DESCRIPTION
  Starts RSSI timer with the value given in the parameter

DEPENDENCIES

RETURN VALUE
  time remaining for timer to expire

SIDE EFFECTS
  none

===========================================================================*/
rex_timer_cnt_type cmtask_rssi_timer_start( unsigned long time );

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
/*===========================================================================

FUNCTION cmtask_lte_disable_timer_start

DESCRIPTION
  Starts LTE disable timer with specififed time.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
rex_timer_cnt_type cmtask_attach_complete_timer_start( unsigned long time );

/*===========================================================================

FUNCTION cmtask_lte_disable_timer_clear

DESCRIPTION
  Clears LTE disable timer

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/

void cmtask_attach_complete_timer_clear(void);
#endif

#if (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) )
/*===========================================================================

FUNCTION cmtask_convert_rrc_plmn_id_to_nas_plmn_id

DESCRIPTION
  This function converts from the RRC PLMN ID format (1 BCD digit per byte)
  to plmn_id value of type sys_plmn_id_s_type (2 BCD digits per byte).

DEPENDENCIES
  None

RETURN VALUE
  plmn_id value of type sys_plmn_id_s_type

SIDE EFFECTS
  None

===========================================================================*/

sys_plmn_id_s_type cmtask_convert_rrc_plmn_id_to_nas_plmn_id( 
	
   rrc_plmn_identity_type plmn_id 

);
#endif

/*===========================================================================

FUNCTION cm_si_prl_load_complete_cb

DESCRIPTION
  PRL load complete callback function

  CM registers this function with System Determination (SD) during power-up
  initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_prl_load_complete_cb(

        boolean  is_prl_load_success
        /* Indicate if prl load is successful
        */

);


/*===========================================================================

FUNCTION cm_si_ss_stack_status_cb

DESCRIPTION
  cm_si_ss_stack_status_cb is a callback from CM, registered with SD so it 
  can indicates CM with its internal stack states.

  CM registers this function with System Determination (SD) during power-up
  initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_ss_stack_status_cb(

        sd_ss_e_type  ss,
        /* Indicates which ss the status is reported on
        */
        boolean       is_stack_status_active
        /* Indicate the reported stack is active or not
        */

);

/*===========================================================================

FUNCTION cm_si_generic_data_cb

DESCRIPTION
   This function provide a generic callback from SD to CM
   The data is interpreted based on the type. Future expansion
   does not required to add new callback function, just adding new
   data type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_generic_data_cb(

     const sd_rpt_generic_s_type     *sd_generic_data
       /* Storing generic data
       */
);

/*===========================================================================

FUNCTION cmtask_is_init_complete

DESCRIPTION
  Returns whether CM task has completed initialization.

DEPENDENCIES
  none

RETURN VALUE
  TRUE    if CM task is fully initialized
  FALSE   otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmtask_is_init_complete( void );

/*===========================================================================

FUNCTION cm_client_cmd_q_put

DESCRIPTION
  Enqueue a command buffer onto the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_client_cmd_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_cmd_q_put(

    cm_cmd_type    *cmd_ptr
        /* pointer to a command buffer to be enqueued */
);

#endif  /* CMTASKI_H */
