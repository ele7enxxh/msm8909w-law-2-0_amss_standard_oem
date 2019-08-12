#ifndef GTMRS_H
#define GTMRS_H
/*===========================================================================

                 G S M   T I M E R S   H E A D E R    F I L E

DESCRIPTION
  This module contains declarations and definitions to interface
  with the GSM timers module.

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gtmrs.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
30/06/14    og      CR678957. Enforce GSTMR HW and SW FN synchronization after LTE turns on the GSM clocks.
12/02/14    cja     CR610689 Add API gstmr_rd_hw_fn_qs (synchronised FN and qs)
30/01/14    npt     CR608373 - Enable opt3 on Bolt/Remove unused apis 
13/01/14    ss      CR598548 Deregister/Register gstmr view in VSTMR during W Sleep / wakeup cycle
10/12/13    ws      CR 587586 - Remove DUAL_SIM featurisation in API
04/12/13    js      CR576915 GL1 needs to check if GSTMR is initialized before invoking gstmr_rd_qsymbol_count() - issue detected after panic reset
19/11/13    ssh/ip  CR547956 DSDS/DSDA GL1 timeline Opt (parallelize the RF config. with GL1 warm-up)
31/10/13    cgc     CR569801  Add wrapper function gl1_cm_advance_fn
24/10/13    cos     CR566292 - Request for API for reading current GFN based on XO cnt value 
24/04/13    pg      CR449656 - Remove GSTMR WCDMA Tick delay 
08/03/13    ws      CR460576 -  Implement gl1_cm_set_fn() API
22/02/13    pg      CR455347: Expose gstmr_wakeup_init as external API 
26/11/12    ws      CR424198 - API backward compatible with TRITON DSDA changes
19/11/12    cja     CR422874 Add pseudo Hw FN for Dime
23/10/12    npt     CR413635 - Port sleep code for DIME modem
21/03/12    ws      CR 345606 Improve stability of X2G Rxfe stuck recovery
11-03-30    tjw     API content split out from the original GERAN internal file
                    previously used by GERAN clients.
========================================================================== */

#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "msgr.h"

#include "sys.h"
#include "geran_dual_sim.h"
/*maximum full TDMA frame number*/
#define MULTIFRAME_26          26
#define MULTIFRAME_51          51
#define FRAMES_IN_SUPERFRAME  (MULTIFRAME_26 * MULTIFRAME_51)
#define FRAMES_IN_HYPERFRAME  (2048 * FRAMES_IN_SUPERFRAME)

/* Constants used for coordinating WCDMA to GSM stack changes */
#define GSTMR_WCDMA_TICK_DELAY   0



/* Handler type */
typedef void (*gstmr_handler_type)(sys_modem_as_id_e_type as_id);
typedef void (*gstmr_handler_multirat_type)(void);

extern void gstmr_gsm_core_reset( void );

extern void gstmr_register_handler( 
#ifdef FEATURE_DUAL_SIM
   gstmr_handler_type handler_ptr, sys_modem_as_id_e_type as_id
#else
   gstmr_handler_multirat_type handler_ptr
#endif /*FEATURE_DUAL_SIM */
                                    );
extern void gstmr_deregister_handler( 
#ifdef FEATURE_DUAL_SIM
   gstmr_handler_type handler_ptr, sys_modem_as_id_e_type as_id
#else
   gstmr_handler_multirat_type handler_ptr
#endif /*FEATURE_DUAL_SIM */
                                    );

void gstmr_register_handler_geran( gstmr_handler_type handler_ptr ,sys_modem_as_id_e_type as_id);
void gstmr_deregister_handler_geran( gstmr_handler_type handler_ptr,sys_modem_as_id_e_type as_id);

boolean gstmr_is_handler_registered_geran(gstmr_handler_type handler_ptr,sys_modem_as_id_e_type as_id);

extern boolean gstmr_is_handler_registered( 
#ifdef FEATURE_DUAL_SIM
   gstmr_handler_type handler_ptr , sys_modem_as_id_e_type as_id  
#else
   gstmr_handler_multirat_type handler_ptr
#endif /*FEATURE_DUAL_SIM*/
                                          );

uint32 gstmr_rd_qsymbol_count_geran( sys_modem_as_id_e_type as_id  );

extern uint32 gstmr_rd_qsymbol_count( 
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type as_id  
#else
   void
#endif /*FEATURE_DUAL_SIM*/
   );

extern void gl1_hw_mcpm_restore_gas_1_cb( void );
extern void gl1_hw_mcpm_restore_gas_2_cb( void );
extern void gl1_hw_mcpm_restore_gas_3_cb( void );

/* GSM TDMA frame number -- access functions */
extern uint32 gl1_cm_get_FN(sys_modem_as_id_e_type as_id);
extern void   gl1_cm_set_fn( uint32 new_fn, sys_modem_as_id_e_type as_id);
extern void   gl1_cm_advance_fn ( uint32 fn_diff, sys_modem_as_id_e_type as_id );

typedef struct
{
   uint32 fn;
   uint16 qs;
} gsmtr_fn_qs_type;

#ifdef FEATURE_DUAL_SIM
extern uint32 gstmr_rd_hw_frame_count( sys_modem_as_id_e_type as_id );
extern gsmtr_fn_qs_type gstmr_rd_hw_fn_qs(sys_modem_as_id_e_type as_id);
#else
extern uint32 gstmr_rd_hw_frame_count( void );
extern gsmtr_fn_qs_type gstmr_rd_hw_fn_qs(void);
#endif /*FEATURE_DUAL_SIM*/

extern void gstmr_rd_hw_frame_count_from_xo( 
  uint32 xo_cnt, 
  uint32 *hwfn, 
  uint32 *qsymbol_cnt
  #ifdef FEATURE_DUAL_SIM
  , sys_modem_as_id_e_type as_id
  #endif
);

uint32 gstmr_rd_hw_frame_count_geran( sys_modem_as_id_e_type as_id );
extern uint32 get_gstmr_FN_mod4( sys_modem_as_id_e_type as_id );

extern void  geran_l2g_sync_hw_sw_frame_count( sys_modem_as_id_e_type  as_id );

/*check if GSTMR HW is ON to be able to read Qsym count*/
extern boolean gstmr_hw_on (sys_modem_as_id_e_type as_id);

extern void gstmr_wakeup_init(
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type as_id
#else
    void
#endif /*FEATURE_DUAL_SIM*/
   );



extern void gstmr_sleep_init(
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type as_id
#else
    void
#endif /*FEATURE_DUAL_SIM*/
   );



extern void   GSTMR_SET_FN( uint32 new_fn
#ifdef FEATURE_DUAL_SIM
                     , sys_modem_as_id_e_type as_id  
#endif /*FEATURE_DUAL_SIM */
                     );
/*===========================================================================

                         MACRO DECLARATIONS

===========================================================================*/
/* ----------------------------------------------------- *
 * IMPORTANT: The routines below are useful given the    *
 *            following preconditions:                   *
 *  - A must be in the range [0..M-1]                    *
 *  - B must be in the range [0..M-1]                    *
 *  - RESULT will be in the range [0..M-1]               *
 *     but requires a datatype in the range [0..2M-2]    *
 *     to prevent overflow.                              *
 *                                                       *
 *  If these conditions are not true, you can NOT use    *
 *  these functions!!                                    *
 * ----------------------------------------------------- */

// Routine returns (A + B) mod M
#define ADD_MOD( a, b, m ) ( ( (a) + (b) < (m) ) ? ( (a) + (b) ) : ( (a) + (b) - (m) ) )
// Routine returns (A - B) mod M
#define SUB_MOD( a, b, m ) ( ( (a) >= (b) ) ? ( (a) - (b) ) : ( (m) + (a) - (b) ) )

// Routine returns (-A) mod M
#define NEG_MOD( a, m ) ( (m) - (a) )

#define FRAMES_PER_HYPERFRAME (26L*51L*2048L)
#define ADD_FN( a, b ) ADD_MOD( (a), (b), FRAMES_PER_HYPERFRAME )
#define SUB_FN( a, b ) SUB_MOD( (a), (b), FRAMES_PER_HYPERFRAME )
#define NEG_FN( a )    NEG_MOD( (a),      FRAMES_PER_HYPERFRAME )

#define GSTMR_RD_QSYMBOL_COUNT(as_id)\
           ( gstmr_hw_on(as_id)? gstmr_rd_qsymbol_count(as_id): 0 )

/* Macros to set, read the value of gstmr_odd_even_check_disable*/
void gstmr_enable_odd_even_check(boolean enable
#ifdef FEATURE_DUAL_SIM
, sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM */
);

void gstmr_enable_odd_even_check_geran(boolean enable, sys_modem_as_id_e_type as_id);

#ifdef FEATURE_DUAL_SIM
#define GSTMR_ENABLE_ODD_EVEN_CHECK(as_id) (gstmr_enable_odd_even_check(TRUE ,as_id))
#else
#define GSTMR_ENABLE_ODD_EVEN_CHECK() (gstmr_enable_odd_even_check(TRUE))
#endif /*FEATURE_DUAL_SIM*/

#define GSTMR_ENABLE_ODD_EVEN_CHECK_GERAN(as_id) (gstmr_enable_odd_even_check_geran(TRUE ,as_id))
#define GSTMR_DISABLE_ODD_EVEN_CHECK_GERAN(as_id) (gstmr_enable_odd_even_check_geran(FALSE ,as_id))

#ifdef FEATURE_DUAL_SIM
#define GSTMR_DISABLE_ODD_EVEN_CHECK(as_id) (gstmr_enable_odd_even_check(FALSE ,as_id))
#else
#define GSTMR_DISABLE_ODD_EVEN_CHECK() (gstmr_enable_odd_even_check(FALSE))
#endif /*FEATURE_DUAL_SIM*/


/* Elongate the Current TDMA Frame */
extern void gstmr_adjust_terminal_count(int16 quarter_symbol_count
#ifdef FEATURE_DUAL_SIM
                                        ,sys_modem_as_id_e_type as_id
#endif /*FEATURE_DUAL_SIM*/
                                        );

#ifdef FEATURE_DUAL_SIM
extern uint32 GSTMR_GET_FN(sys_modem_as_id_e_type as_id);
#else
extern uint32 GSTMR_GET_FN(void);
#endif /*FEATURE_DUAL_SIM*/

extern void   GSTMR_ADVANCE_FN( uint32 fn_diff
#ifdef FEATURE_DUAL_SIM
                                , sys_modem_as_id_e_type as_id 
#endif /*FEATURE_DUAL_SIM*/
                                );
void gstmr_adjust_terminal_count_geran(int16 quarter_symbol_count,sys_modem_as_id_e_type as_id);

/* used in DSDx to align or delay time bases between two gstmrs*/
extern void gstmr_align_timebase(uint16 delay_qs);


#endif /* GTMRS_H */
