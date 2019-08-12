/*****************************************************************************
***
*** TITLE  gllchdlr.c
***
***  GPRS LLC Layer Event Management Functions
***
***
*** DESCRIPTION
***
***  The GPRS LLC Event Manager supports LLC control task primary event
***  processing, and further event diversity via a sub-event representation
***  and associated processing prioritisation.
***
***  The GPRS LLC Event Manager provides the event associated messaging
***  queues and handler functions. The handler functions serve to provide
***  a message pre-processing capability before message presentation to the
***  GPRS LLC State Manager.
***
***
*** EXTERNALIZED FUNCTIONS
***
***   gllc_event_handler()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/src/gllchdlr.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10/12/13   ra      CR563511
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "geran_tasks.h"
#include "gmutex.h"
#include "cfa.h"
#include "gs.h"
#include "ms.h"
#include "gprs_mem.h"
#include "gprs_pdu.h"
#include "gprsmod.h"
#include "gllc.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gllci.h"
#include "gllclog.h"
#include "gprsmod.h"
#include "dsm.h"
#include "mmtask.h"
#include "mmtask_v.h"
#include "geran_msgs.h"
#include "queue.h"
#include "rex.h"
#include "string.h"

#include "grlcllc.h"
#include "gsndcp_v_g.h"
#include "geran_dual_sim_g.h"

#if defined TEST_FRAMEWORK
#error code not present
#endif  

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/* Up Link Event Mask Lookup Table for LLME masking schemes. */

LOCAL const uint16 gllc_llme_ul_evt_mask_scheme_lookup
  [ GLLC_LLME_NUM_SCHEMES ][ GLLC_LL_NUM_SAPIS ] =
{
  /* ..................................................................... */

  /* Scheme -  GLLC_LLME_SCHEME_LLGMM_SUSPEND

     Evt -     s  s  S  S  I  I  U  U  U  U  T  T  L  L  L  L
               p  p  .  .  .  .  I  .  .  .  I  I  L  L  L  L
               a  a  F  F  F  F  .  F  F  F  M  M  .  .  .  .
               r  r  R  R  R  R  F  R  R  R  E  E  C  U  A  S
               e  e  .  .  .  .  R  .  .  .  R  R  R  N  C  I
                     A  E  R  E  .  U  S  C  .  .  .  A  K  G
                     B  V  E  V  E  R  R  M  T  T  Q  C  .  .
                     I  T  T  T  V  E  E  D  2  2  .  K  Q  Q
                     T     R     T  S  S  .  0  0  E  .  .  .
                     .     X        .  .  E  1  0  V  Q  E  E
                     E     .        E  E  V  .  .  T  .  V  V
                     V     E        V  V  T  E  E     E  T  T
                     T     V        T  T     V  V     V
                           T                 T  T     T

     Bit -    15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
              -----------------------------------------------              */
  {
    0xC3DD, /* 1  1  0  0  0  0  1  1  1  1  0  1  1  1  0  1  SAPI_1_GMM  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_3_LL3  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_5_LL5  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_7_SMS  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_9_LL9  */
    0xC1D9  /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_11_LL11*/
  },

  /* ..................................................................... */

  /* Scheme -  GLLC_LLME_SCHEME_GRR_SUSPEND

     Evt -     s  s  S  S  I  I  U  U  U  U  T  T  L  L  L  L
               p  p  .  .  .  .  I  .  .  .  I  I  L  L  L  L
               a  a  F  F  F  F  .  F  F  F  M  M  .  .  .  .
               r  r  R  R  R  R  F  R  R  R  E  E  C  U  A  S
               e  e  .  .  .  .  R  .  .  .  R  R  R  N  C  I
                     A  E  R  E  .  U  S  C  .  .  .  A  K  G
                     B  V  E  V  E  R  R  M  T  T  Q  C  .  .
                     I  T  T  T  V  E  E  D  2  2  .  K  Q  Q
                     T     R     T  S  S  .  0  0  E  .  .  .
                     .     X        .  .  E  1  0  V  Q  E  E
                     E     .        E  E  V  .  .  T  .  V  V
                     V     E        V  V  T  E  E     E  T  T
                     T     V        T  T     V  V     V
                           T                 T  T     T

     Bit -    15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
              -----------------------------------------------              */
  {
    0xC3DD, /* 1  1  0  0  0  0  1  1  1  1  0  1  1  1  0  1  SAPI_1_GMM  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_3_LL3  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_5_LL5  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_7_SMS  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_9_LL9  */
    0xC1D9  /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_11_LL11*/
  },

  /* ..................................................................... */

  /* Scheme -  GLLC_LLME_SCHEME_DSM_SUSPEND

     Evt -     s  s  S  S  I  I  U  U  U  U  T  T  L  L  L  L
               p  p  .  .  .  .  I  .  .  .  I  I  L  L  L  L
               a  a  F  F  F  F  .  F  F  F  M  M  .  .  .  .
               r  r  R  R  R  R  F  R  R  R  E  E  C  U  A  S
               e  e  .  .  .  .  R  .  .  .  R  R  R  N  C  I
                     A  E  R  E  .  U  S  C  .  .  .  A  K  G
                     B  V  E  V  E  R  R  M  T  T  Q  C  .  .
                     I  T  T  T  V  E  E  D  2  2  .  K  Q  Q
                     T     R     T  S  S  .  0  0  E  .  .  .
                     .     X        .  .  E  1  0  V  Q  E  E
                     E     .        E  E  V  .  .  T  .  V  V
                     V     E        V  V  T  E  E     E  T  T
                     T     V        T  T     V  V     V
                           T                 T  T     T

     Bit -    15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
              -----------------------------------------------              */
  {
    0xC3DD, /* 1  1  0  0  0  0  1  1  1  1  0  1  1  1  0  1  SAPI_1_GMM  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_3_LL3  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_5_LL5  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_7_SMS  */
    0xC1D9, /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_9_LL9  */
    0xC1D9  /* 1  1  0  0  0  0  0  1  1  1  0  1  1  0  0  1  SAPI_11_LL11*/
  },

  /* ..................................................................... */

  /* Scheme -  GLLC_LLME_SCHEME_TEST_MODE

     Evt -     s  s  S  S  I  I  U  U  U  U  T  T  L  L  L  L
               p  p  .  .  .  .  I  .  .  .  I  I  L  L  L  L
               a  a  F  F  F  F  .  F  F  F  M  M  .  .  .  .
               r  r  R  R  R  R  F  R  R  R  E  E  C  U  A  S
               e  e  .  .  .  .  R  .  .  .  R  R  R  N  C  I
                     A  E  R  E  .  U  S  C  .  .  .  A  K  G
                     B  V  E  V  E  R  R  M  T  T  Q  C  .  .
                     I  T  T  T  V  E  E  D  2  2  .  K  Q  Q
                     T     R     T  S  S  .  0  0  E  .  .  .
                     .     X        .  .  E  1  0  V  Q  E  E
                     E     .        E  E  V  .  .  T  .  V  V
                     V     E        V  V  T  E  E     E  T  T
                     T     V        T  T     V  V     V
                           T                 T  T     T

     Bit -    15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
              -----------------------------------------------              */
  {
    0xC000, /* 1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  SAPI_1_GMM  */
    0xC000, /* 1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  SAPI_3_LL3  */
    0xC000, /* 1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  SAPI_5_LL5  */
    0xC000, /* 1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  SAPI_7_SMS  */
    0xC000, /* 1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  SAPI_9_LL9  */
    0xC000  /* 1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  SAPI_11_LL11*/
  }

  /* ..................................................................... */
};

/* ------------------------------------------------------------------------ */

/* Up Link Event Mask Lookup table for LLE specific masking schemes. */

LOCAL const uint16 gllc_lle_ul_evt_mask_scheme_lookup[ GLLC_LLE_NUM_SCHEMES ] =
{

/* Evt -     s  s  S  S  I  I  U  U  U  U  T  T  L  L  L  L
             p  p  .  .  .  .  I  .  .  .  I  I  L  L  L  L
             a  a  F  F  F  F  .  F  F  F  M  M  .  .  .  .
             r  r  R  R  R  R  F  R  R  R  E  E  C  U  A  S
             e  e  .  .  .  .  R  .  .  .  R  R  R  N  C  I
                   A  E  R  E  .  U  S  C  .  .  .  A  K  G
                   B  V  E  V  E  R  R  M  T  T  Q  C  .  .
                   I  T  T  T  V  E  E  D  2  2  .  K  Q  Q
                   T     R     T  S  S  .  0  0  E  .  .  .
                   .     X        .  .  E  1  0  V  Q  E  E
                   E     .        E  E  V  .  .  T  .  V  V
                   V     E        V  V  T  E  E     E  T  T
                   T     V        T  T     V  V     V
                         T                 T  T     T

   Bit -    15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
            -----------------------------------------------                */
  0xF3FD, /* 1  1  1  1  0  0  1  1  1  1  1  1  1  1  0  1 PEER_RCVR_BUSY */
  0xFFFD, /* 1  1  1  1  1  1  1  1  1  1  1  1  1  1  0  1 RETRX_BUF_FULL */
  0xFFF7, /* 1  1  1  1  1  1  1  1  1  1  1  1  0  1  1  1 WAIT_LAY3_RESP */
  0xFFFF, /* 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 WAIT_PEER_RESP */
  0xFFFD  /* 1  1  1  1  1  1  1  1  1  1  1  1  1  1  0  1 WAIT_ABM       */
};


uint32  lle_last[ NUM_GERAN_DATA_SPACES ] = {(uint32)GLLC_LL_LAST_SAPI};

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */


/* ------------------------------------------------------------------------ */


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

/* Event handler prototypes for the handling of up link related events. */

LOCAL void gllc_llme_gmm_ul_q_event_handler( gas_id_t gas_id );
LOCAL void gllc_lle_cr_ul_q_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle );
LOCAL void gllc_lle_ul_timer_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle );
LOCAL void gllc_lle_ll_ul_sig_q_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle );
LOCAL void gllc_lle_ll_ul_ack_q_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle );
LOCAL void gllc_lle_ll_ul_unack_q_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle );
LOCAL void gllc_mpex_grr_ul_trx_event_handler
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  uint32         lle_unmasked_event_map
);

/* Support prototypes for use by the up link handlers. */

LOCAL void gllc_mpex_embody_supervisory_content
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  uint32         unmasked_trx_reqts
);

LOCAL void gllc_mpex_diagnostic_messaging
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_frame_t   *fr_p,
  boolean        i_s_frame_retrx
);

/* ------------------------------------------------------------------------ */

/* Event handler prototypes for the handling of down link related events. */

LOCAL void gllc_mpex_grr_dl_q_event_handler( gas_id_t gas_id );
LOCAL void gllc_ul_unnum_cr_put_mes( gas_id_t gas_id );
LOCAL void gllc_ul_frmr_put_mes( gas_id_t gas_id );


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/* ===========================================================================
   Up Link Event Queue Definitions
   ======================================================================== */

/* LLME GMM SAP Messaging Queue Declaration.

   For receipt of LLGMM service primitives from the GMM entity via the
   LLGMM SAP. */

q_type                gllc_gmm_ul_q[ NUM_GERAN_DATA_SPACES ];

/* ------------------------------------------------------------------------ */

/* LLE LL SAP Signalling Messaging Queue Array Declaration

   For receipt of LL service primitives (other than LL-UNITDATA-REQ &
   LL-DATA-REQ) from layer 3 network entities via the LL SAP interfaces. */

q_type                gllc_ll_ul_sig_q[ NUM_GERAN_DATA_SPACES ][ GLLC_LL_NUM_SAPIS ];

/* ------------------------------------------------------------------------ */

/* LLE LL SAP Unacknowledged Data Messaging Queue Array Declaration

   For receipt of LL-UNITDATA-REQ service primitives from layer 3 network
   entities via the LL SAP interfaces. */

dsm_watermark_type   gllc_ll_ul_unack_wm[ NUM_GERAN_DATA_SPACES ][ GLLC_LL_NUM_SAPIS ];
q_type               gllc_ll_ul_unack_q[ NUM_GERAN_DATA_SPACES ][ GLLC_LL_NUM_SAPIS ];

/* ------------------------------------------------------------------------ */

/* LLE LL SAP Acknowledged Data Messaging Queue Array Declaration

   For receipt of LL-DATA-REQ service primitives from layer 3 network
   entities via the LL SAP interfaces. */

dsm_watermark_type   gllc_ll_ul_ack_wm[ NUM_GERAN_DATA_SPACES ][ GLLC_LL_NUM_SAPIS ];
q_type               gllc_ll_ul_ack_q[ NUM_GERAN_DATA_SPACES ][ GLLC_LL_NUM_SAPIS ];

/* ------------------------------------------------------------------------ */

/* LLE C/R Messaging Queue Array Declaration

   For receipt of down link originated commands and responses from the peer
   LLE. */

q_type                gllc_cr_ul_q[ NUM_GERAN_DATA_SPACES ][ GLLC_LL_NUM_SAPIS ];


/* ===========================================================================
   Down Link Event Queue Declaration
   ======================================================================== */

/* LLE GRR SAP Messaging Queue Declaration

   For receipt of GRR service primitives from the RLC/MAC layer via the GRR
   SAP interface. */

q_type                gllc_grr_dl_q[ NUM_GERAN_DATA_SPACES ];


/* ===========================================================================
   Internal Event/Mask Record Declaration
   ======================================================================== */

/* Up Link Event Bitmap Record Declaration.

   For the recording of active NON LLE specific events.
   Use appropriate bitmap definitions in gllci.h. */

uint8                 gllc_ul_event_map[NUM_GERAN_DATA_SPACES] = {GLLC_UL_NO_EVT};


/* ------------------------------------------------------------------------ */

/* Up Link LLE Event/Mask Record Declaration. */

/* For the recording of active LLE specific events and event masking status.
   Use appropriate bitmap definitions in gllci.h. */

uint16                gllc_lle_ul_event_map[NUM_GERAN_DATA_SPACES][ GLLC_LL_NUM_SAPIS ];
uint16                gllc_lle_ul_mask_map[NUM_GERAN_DATA_SPACES][ GLLC_LL_NUM_SAPIS ];

/* For representing the or'd function of all unmasked active LLE specific
   events. */

/* For indicating those LLEs with an unmasked active LLE specific event.
   Each bit position flag represents the event status of each LLE. */

uint16                gllc_lle_ul_event_flags[ NUM_GERAN_DATA_SPACES ] = { 0 };

/* ------------------------------------------------------------------------ */

/* Up Link Event Masking Scheme Record Declaration. */

boolean               gllc_llme_ul_evt_mask_scheme_flags
                      [ NUM_GERAN_DATA_SPACES ][ GLLC_LLME_NUM_SCHEMES ] = { { 0 } };

boolean               gllc_lle_ul_evt_mask_scheme_flags
                      [ NUM_GERAN_DATA_SPACES ][ GLLC_LLE_NUM_SCHEMES ][ GLLC_LL_NUM_SAPIS ] = 
                                                                       { { { 0 } } };

/* ===========================================================================
   LLC Up Link Timer Storage

   REX extended timers for each LLE.
   ======================================================================== */

rex_timer_type       gllc_timer_t200[ NUM_GERAN_DATA_SPACES ][ GLLC_LL_NUM_SAPIS ];
rex_timer_type       gllc_timer_t201[ NUM_GERAN_DATA_SPACES ][ GLLC_LL_NUM_SAPIS ];


/* ===========================================================================
   LLC frame assembly working storage.
   ======================================================================== */

gllc_frame_assy_t     gllc_assy_info[ NUM_GERAN_DATA_SPACES ];


/* ===========================================================================
   LLC frame disassembly working storage.
   ======================================================================== */

gllc_frame_disassy_t  gllc_disassy_info[ NUM_GERAN_DATA_SPACES ];



/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_GMM_UL_Q_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Extracts ONE message from the GMM up link message queue and submits it
===   for up link state processing.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_llme_gmm_ul_q_event_handler( gas_id_t gas_id )
{
  gmmllc_llgmm_mes_type *llgmm_ul_mes;
  uint32                lle;

  /* --------------------------------------------------------------------- */

  /* Extract a message from the LLGMM up link queue. Submit the message for
     up link state processing, and free the message resource. */

  llgmm_ul_mes = q_get( &gllc_gmm_ul_q[ gas_id ] );

  if ( llgmm_ul_mes != NULL )
  {
    switch ( llgmm_ul_mes -> mes.prim )
    {
    case GMMLLC_LLGMM_ASSIGN_REQ:

      cfa_log_packet_ex
      (
        gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LLGMM_ASSIGN_REQ,
        sizeof( gmmllc_llgmm_assign_req_type ),
        (uint8 *)&llgmm_ul_mes -> mes.llgmm_assign_req
      );

      gllc_llme_process_assign_req_mes(
        gas_id, &llgmm_ul_mes -> mes.llgmm_assign_req );

      gllc_diag_rpt_llme_state(gas_id);

      for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
      {
        gllc_diag_rpt_lle_state( gas_id, (gllc_ll_sapi_t)lle );

        gllc_debug_print_lle_cipher_params( gas_id, (gllc_ll_sapi_t)lle );

      }
      break;

    case GMMLLC_LLGMM_TRIGGER_REQ:

      cfa_log_packet_ex
      (
        gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LLGMM_TRIGGER_REQ,
        sizeof( gmmllc_llgmm_trigger_req_type ),
        (uint8 *)&llgmm_ul_mes -> mes.llgmm_trigger_req
      );

      gllc_llme_process_trigger_req_mes(
        gas_id, &llgmm_ul_mes -> mes.llgmm_trigger_req );
      break;

    case GMMLLC_LLGMM_SUSPEND_REQ:

      cfa_log_packet_ex
      (
        gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LLGMM_SUSPEND_REQ,
        sizeof( gmmllc_llgmm_suspend_req_type ),
        (uint8 *)&llgmm_ul_mes -> mes.llgmm_suspend_req
      );

      gllc_llme_process_suspend_req_mes(
        gas_id, &llgmm_ul_mes -> mes.llgmm_suspend_req );
      break;

    case GMMLLC_LLGMM_RESUME_REQ:

      cfa_log_packet_ex
      (
        gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LLGMM_RESUME_REQ,
        sizeof( gmmllc_llgmm_resume_req_type ),
        (uint8 *)&llgmm_ul_mes -> mes.llgmm_resume_req
      );

      gllc_llme_process_resume_req_mes(
        gas_id, &llgmm_ul_mes -> mes.llgmm_resume_req );
      break;

    case GMMLLC_LLGMM_STRT_RDY_TMR_REQ:

      cfa_log_packet_ex
      (
        gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LLGMM_STRT_RDY_TMR_REQ,
        sizeof( gmmllc_llgmm_strt_rdy_tmr_req_type ),
        (uint8 *)&llgmm_ul_mes -> mes.llgmm_strt_rdy_tmr_req
      );

      gllc_llme_process_strt_rdy_tmr_req_mes(
        gas_id, &llgmm_ul_mes -> mes.llgmm_strt_rdy_tmr_req );
      break;

    case GMMLLC_LLGMM_STOP_RDY_TMR_REQ:

      cfa_log_packet_ex
      (
        gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LLGMM_STOP_RDY_TMR_REQ,
        sizeof( gmmllc_llgmm_stop_rdy_tmr_req_type ),
        (uint8 *)&llgmm_ul_mes -> mes.llgmm_stop_rdy_tmr_req
      );

      gllc_llme_process_stop_rdy_tmr_req_mes(
        gas_id, &llgmm_ul_mes -> mes.llgmm_stop_rdy_tmr_req );
      break;

    case GMMLLC_LLGMM_TEST_MODE_REQ:

      cfa_log_packet_ex
      (
        gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LLGMM_TEST_MODE_REQ,
        sizeof( gmmllc_llgmm_test_mode_req_type ),
        (uint8 *)&llgmm_ul_mes -> mes.llgmm_test_mode_req
      );

      gllc_llme_process_test_mode_req_mes(
        gas_id, &llgmm_ul_mes -> mes.llgmm_test_mode_req );
      break;

#ifdef FEATURE_GPRS_PS_HANDOVER

    case GMMLLC_LLGMM_PSHO_REQ:

      cfa_log_packet_ex
      (
        gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LLGMM_PSHO_REQ,
        sizeof( gmmllc_llgmm_psho_req_type),
        (uint8 *)&llgmm_ul_mes -> mes.llgmm_psho_req
      );

      gllc_llme_process_psho_req_mes(
        gas_id, &llgmm_ul_mes -> mes.llgmm_psho_req );
      break;

#endif /* FEATURE_GPRS_PS_HANDOVER */

    default:
      MSG_GERAN_ERROR_1_G( "GLC LLGMM UL Hdlr: Invld Prim %d.",
        llgmm_ul_mes -> mes.prim);
      break;
    }

    GPRS_MEM_FREE( llgmm_ul_mes );

  }

  /* ------------------------------------------------------------------- */

  /* Clear the event flag if the queue is empty. */

  GLLC_UL_MSG_LOCK(gas_id);

  if ( q_cnt( &gllc_gmm_ul_q[ gas_id ] ) == 0 )
    GLLC_CLR_UL_EVT( gas_id, GLLC_UL_GMM_Q_EVT );

  GLLC_UL_MSG_UNLOCK(gas_id);

  /* ------------------------------------------------------------------- */

} /* end of gllc_llme_gmm_ul_q_event_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_MPEX_EMBODY_SUPERVISORY_CONTENT()
===
===  DESCRIPTION
===
===   For use by the MPEX Up Link GRR Transmission Handler.
===
===   Determines the appropriate supervisory content to be embodied within
===   an I+S or S Frame prior to transmission.
===
===   The supervisory content is added to the frame which is currently
===   pointed to by the frame pointer element of the frame assembly working
===   structure.
===
===  DEPENDENCIES
===
===   The caller is required to clear down any supervisory transmission
===   requirements.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_mpex_embody_supervisory_content
(
  gas_id_t          gas_id,
  gllc_ll_sapi_t    lle,
  uint32            unmasked_trx_evts
)
{
  register gllc_frame_format_t format;

  /* ===================================================================== */

  if ( gllc_assy_info[gas_id].frame != NULL )
  {
    format = gllc_assy_info[gas_id].frame -> format;

    /* ------------------------------------------------------------------- */

    /* Set defaults and set the frame's A bit if a request for
       acknowledgement is required. */

    if ( format == GLLC_I_S_FORMAT )
    {
      gllc_assy_info[gas_id].frame -> i_s_frame.cmd_or_res_CR = GLLC_COMMAND;
      gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2  = GLLC_RR;
      gllc_assy_info[gas_id].frame -> i_s_frame.ack_req_A =
        (unmasked_trx_evts & GLLC_LLE_UL_S_FR_ABIT_EVT) ? TRUE : FALSE;
    }
    else if ( format == GLLC_S_FORMAT )
    {
      gllc_assy_info[gas_id].frame -> s_frame.cmd_or_res_CR   = GLLC_COMMAND;
      gllc_assy_info[gas_id].frame -> s_frame.cmd_res_S1S2    = GLLC_RR;
      gllc_assy_info[gas_id].frame -> s_frame.ack_req_A =
        (unmasked_trx_evts & GLLC_LLE_UL_S_FR_ABIT_EVT) ? TRUE : FALSE;
    }

    /* ------------------------------------------------------------------- */

    /* If the LLE is in the 'own receiver busy' state, then the
       appropriate supervisory frame is the RNR frame. */

    if ( gllc_lle[gas_id][lle].ack.own_rcvr_busy )
    {
      if ( format == GLLC_I_S_FORMAT )
        gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2  = GLLC_RNR;

      else if ( format == GLLC_S_FORMAT )
        gllc_assy_info[gas_id].frame -> s_frame.cmd_res_S1S2    = GLLC_RNR;
    }

    /* ------------------------------------------------------------------- */

    else if ( unmasked_trx_evts & GLLC_LLE_UL_S_FR_EVT )
    {
      /* If a supervisory frame transmission requirement has been
         identified, either in response to a request for acknowledgement
         from the peer, or the detection of a sequence error in the order
         of received frames, then the supervisory content must be determined
         as follows:

         If the highest numbered I frame was received with N(S) = V(R), then
         the appropriate supervisory frame is the RR frame.

         NOTE: As N(S) was 'in-sequence' in this case, V(R) has already been
         incremented during N(R) acknowledgement procedures when this frame
         was received. Therefore, in contrast to the test for ACK supervisory
         content, comparison must be made with V(R) - 1 and not the current
         value of V(R). */

      if
      (
        gllc_lle[gas_id][lle].ack.highest_rcvd_N_S ==
        GMODULO
        (
          512,
          GMODULO_ABS( 512, (gllc_lle[gas_id][lle].ack.recv_state_var_V_R - 1) )
        )
      )
      {
        if ( format == GLLC_I_S_FORMAT )
          gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2  = GLLC_RR;

        else if ( format == GLLC_S_FORMAT )
          gllc_assy_info[gas_id].frame -> s_frame.cmd_res_S1S2    = GLLC_RR;
      }

      /* ................................................................. */

      /* else if the highest numbered I frame was received with
         N(S) = V(R) + 1, then the appropriate supervisory frame is the
         ACK frame. */

      else if
      (
        gllc_lle[gas_id][lle].ack.highest_rcvd_N_S ==
        GMODULO
        (
          512,
          GMODULO( 512, gllc_lle[gas_id][lle].ack.recv_state_var_V_R + 1 )
        )
      )
      {
        if ( format == GLLC_I_S_FORMAT )
          gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2  = GLLC_ACK;

        else if ( format == GLLC_S_FORMAT )
          gllc_assy_info[gas_id].frame -> s_frame.cmd_res_S1S2    = GLLC_ACK;
      }

      /* ................................................................. */

      /* else the appropriate supervisory frame is the SACK frame. */

      else
      {
        if ( format == GLLC_I_S_FORMAT )
        {
          gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2 = GLLC_SACK;

          gllc_assy_info[gas_id].frame -> i_s_frame.sack_bitmap_len_K =
            gllc_lle[gas_id][lle].ack.sack_bitmap_len_K;

          /* Just point at the source of SACK bitmap rather than use the
             IS frame structure to hold it. */

          gllc_assy_info[gas_id].sack_bitmap_R_p = gllc_lle[gas_id][lle].ack.sack_bitmap_R;
        }

        else if ( format == GLLC_S_FORMAT )
        {
          gllc_assy_info[gas_id].frame -> s_frame.cmd_res_S1S2 = GLLC_SACK;

          gllc_assy_info[gas_id].frame -> s_frame.sack_bitmap_len_K =
            gllc_lle[gas_id][lle].ack.sack_bitmap_len_K;

          /* Just point at the source of SACK bitmap rather than use the
             S frame structure to hold it. */

          gllc_assy_info[gas_id].sack_bitmap_R_p = gllc_lle[gas_id][lle].ack.sack_bitmap_R;
        }
      }
    }
  }

  /* ===================================================================== */

  else
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: MPEX Embody S Err.",
      gllc_sapi_addr_lookup[lle]);
  }

  /* ===================================================================== */
} /* end of gllc_mpex_embody_supervisory_content() */


/*===========================================================================
===
===  FUNCTION      GLLC_MPEX_DIAGNOSTIC_MESSAGING()
===
===  DESCRIPTION
===
===   For use by the MPEX Up Link GRR Transmission Handler.
===   Formats an appropriate diagnostic message for the given transmit frame.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_mpex_diagnostic_messaging
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_frame_t   *fr_p,
  boolean        i_s_frame_retrx
)
{
  switch ( fr_p -> format )
  {
  /* ===================================================================== */

  case GLLC_I_S_FORMAT:  /* Information plus supervisory format. */

    if ( gllc_assy_info[gas_id].frame -> i_s_frame.ack_req_A )
    {
      if ( i_s_frame_retrx )
      {
        switch ( gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2 )
        {
        case GLLC_RR:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Retx IS RR S:%d R:%d A",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_RNR:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Retx IS RNR S:%d R:%d A",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_ACK:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Retx IS ACK S:%d R:%d A",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_SACK:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Retx IS SACK S:%d R:%d A",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        }
      }

      /* ................................................................. */

      else
      {
        switch ( gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2 )
        {
        case GLLC_RR:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Sent IS RR S:%d R:%d A",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_RNR:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Sent IS RNR S:%d R:%d A",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_ACK:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Sent IS ACK S:%d R:%d A",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_SACK:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Sent IS SACK S:%d R:%d A",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        }
      }
    }

    /* ------------------------------------------------------------------- */

    else
    {
      if ( i_s_frame_retrx )
      {
        switch ( gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2 )
        {
        case GLLC_RR:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Retx IS RR S:%d R:%d",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_RNR:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Retx IS RNR S:%d R:%d",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_ACK:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Retx IS ACK S:%d R:%d",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_SACK:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Retx IS SACK S:%d R:%d",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        }
      }

      /* ................................................................. */

      else
      {
        switch ( gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2 )
        {
        case GLLC_RR:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Sent IS RR S:%d R:%d",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_RNR:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Sent IS RNR S:%d R:%d",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_ACK:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Sent IS ACK S:%d R:%d",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        case GLLC_SACK:
          MSG_GERAN_MED_3_G
          (
            "GLC LL %d: Sent IS SACK S:%d R:%d",
            gllc_sapi_addr_lookup[lle],
            gllc_assy_info[gas_id].frame -> i_s_frame.send_seq_num_N_S,
            gllc_assy_info[gas_id].frame -> i_s_frame.recv_seq_num_N_R
          );
          break;
        }
      }
    }
    break;

  /* ===================================================================== */

  case GLLC_S_FORMAT:    /* Supervisory format. */

    if ( gllc_assy_info[gas_id].frame -> s_frame.ack_req_A )
    {
      switch ( gllc_assy_info[gas_id].frame -> s_frame.cmd_res_S1S2 )
      {
      case GLLC_RR:
        MSG_GERAN_MED_2_G
        (
          "GLC LL %d: Sent S RR R:%d A",
          gllc_sapi_addr_lookup[lle],
          gllc_assy_info[gas_id].frame -> s_frame.recv_seq_num_N_R
        );
        break;
      case GLLC_RNR:
        MSG_GERAN_MED_2_G
        (
          "GLC LL %d: Sent S RNR R:%d A",
          gllc_sapi_addr_lookup[lle],
          gllc_assy_info[gas_id].frame -> s_frame.recv_seq_num_N_R
        );
        break;
      case GLLC_ACK:
        MSG_GERAN_MED_2_G
        (
          "GLC LL %d: Sent S ACK R:%d A",
          gllc_sapi_addr_lookup[lle],
          gllc_assy_info[gas_id].frame -> s_frame.recv_seq_num_N_R
        );
        break;
      case GLLC_SACK:
        MSG_GERAN_MED_2_G
        (
          "GLC LL %d: Sent S SACK R:%d A",
          gllc_sapi_addr_lookup[lle],
          gllc_assy_info[gas_id].frame -> s_frame.recv_seq_num_N_R
        );
        break;
      }
    }
    else
    {
      switch ( gllc_assy_info[gas_id].frame -> s_frame.cmd_res_S1S2 )
      {
      case GLLC_RR:
        MSG_GERAN_MED_2_G
        (
          "GLC LL %d: Sent S RR R:%d",
          gllc_sapi_addr_lookup[lle],
          gllc_assy_info[gas_id].frame -> s_frame.recv_seq_num_N_R
        );
        break;
      case GLLC_RNR:
        MSG_GERAN_MED_2_G
        (
          "GLC LL %d: Sent S RNR R:%d",
          gllc_sapi_addr_lookup[lle],
          gllc_assy_info[gas_id].frame -> s_frame.recv_seq_num_N_R
        );
        break;
      case GLLC_ACK:
        MSG_GERAN_MED_2_G
        (
          "GLC LL %d: Sent S ACK R:%d",
          gllc_sapi_addr_lookup[lle],
          gllc_assy_info[gas_id].frame -> s_frame.recv_seq_num_N_R
        );
        break;
      case GLLC_SACK:
        MSG_GERAN_MED_2_G
        (
          "GLC LL %d: Sent S SACK R:%d",
          gllc_sapi_addr_lookup[lle],
          gllc_assy_info[gas_id].frame -> s_frame.recv_seq_num_N_R
        );
        break;
      }
    }
    break;

  /* ===================================================================== */

  case GLLC_UI_FORMAT:   /* Unconfirmed information format. */

    if ( gllc_assy_info[gas_id].frame -> ui_frame.encrypted_E )
    {
      MSG_GERAN_MED_3_G
      (
        "GLC LL %d: Sent UI U:%d Cipher Len:%d",
        gllc_sapi_addr_lookup[lle],
        gllc_assy_info[gas_id].frame -> ui_frame.send_seq_num_N_U,
        gllc_assy_info[gas_id].fr_info_length
      );
    }
    else
    {
      MSG_GERAN_MED_3_G
      (
        "GLC LL %d: Sent UI U:%d Plain Len:%d",
        gllc_sapi_addr_lookup[lle],
        gllc_assy_info[gas_id].frame -> ui_frame.send_seq_num_N_U,
        gllc_assy_info[gas_id].fr_info_length
      );
    }

    llc_log_send_ui_frame
    (
      gas_id,
      gllc_assy_info[gas_id].frame -> ui_frame.send_seq_num_N_U,
      gllc_sapi_addr_lookup[lle],
      gllc_assy_info[gas_id].fr_info_length,
      gllc_assy_info[gas_id].frame -> ui_frame.encrypted_E
    );
    break;

  /* ===================================================================== */

  default:
    break;

  /* ===================================================================== */
  }
} /* end of gllc_mpex_diagnostic_messaging() */


/*===========================================================================
===
===  FUNCTION      GLLC_MPEX_GRR_UL_TRX_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Examines the unmasked transmission requirement bitmap of the current
===   LLE to receive service.
===
===   Transmission requirements are serviced as follows:
===
===   (a) Unsolictied U Frame response transmission requirements are
===       serviced with 1st order of precedence.
===
===   (b) I frame re-transmission requirements are serviced with 2nd order
===       of precedence. Any simultaneous supervisory requirements are
===       embodied with the retransmitted I frame. Any simultaneous GMM
===       Trigger (page response) requirements are also serviced by this
===       frame.
===
===   (c) I frame transmission requirements are serviced with 3rd order
===       of precedence. Any simultaneous supervisory requirements are
===       embodied with the transmitted I frame. Any simultaneous GMM
===       Trigger (page response) requirements are also serviced by this
===       frame.
===
===   (d) Standalone S frame supervisory transmission requirements are
===       serviced with 4th order of precedence. Any simultaneous GMM
===       Trigger (page response) requirements are also satisfied by this
===       frame.
===
===   (e) U Frame transmission requirements are serviced with 5th order
===       of precedence.
===
===   (f) UI Frame transmission requirements are serviced with 6th order
===       of precedence.
===
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_mpex_grr_ul_trx_event_handler
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  uint32         lle_unmasked_event_map
)
{
  register gllc_lle_retrx_buf_t  *item_p;
  register gprs_grr_ul_mes_t     *grr_ul_mes;
  register uint32                unmasked_trx_evts;

  /* --------------------------------------------------------------------- */

  unmasked_trx_evts = (uint32)( lle_unmasked_event_map &
                                GLLC_LLE_UL_TRX_EVT      );

  /* --------------------------------------------------------------------- */

  /* Prepare the GRR message assembly working structure for new message
     build. */

  gllc_assy_info[gas_id].frame = NULL;
  gllc_assy_info[gas_id].lle   = lle;

  grr_ul_mes = gllc_assy_info[gas_id].grr_ul_mes =
    GPRS_MEM_CALLOC( sizeof(gprs_grr_ul_mes_t)/sizeof(uint8) );

  if ( grr_ul_mes == NULL )
  {
    ERR_GERAN_FATAL_1_G( "GLC LL %d: MPEX Trx Hdlr: Heap Exhausted",
      gllc_sapi_addr_lookup[lle]);
  }

  /* --------------------------------------------------------------------- */

  /* Unsolicited unnumbered response transmission requirements must be given
     first order of precedence. */

  if ( unmasked_trx_evts & GLLC_LLE_UL_U_FR_URES_EVT )
  {
    /* U Frames must be transfered with GRR_DATA_REQ primitives. */

    gllc_assy_info[gas_id].frame     = &gllc_lle[gas_id][lle].unnum.unsol_res_frame;

    grr_ul_mes -> prim               = GRR_DATA_REQ;
    grr_ul_mes -> tlli               = gllc_llme[gas_id].tlli;
    grr_ul_mes -> radio_priority     = gllc_lle[gas_id][lle].unnum.radio_priority;
    grr_ul_mes -> peak_throughput    = gllc_lle[gas_id][lle].unnum.peak_throughput;
    grr_ul_mes -> reliability_class  = 0;
    grr_ul_mes -> sndcp_ref.nsapi    = 0;
    grr_ul_mes -> sndcp_ref.npdu_num = 0;
    grr_ul_mes -> sndcp_ref.seg_num  = 0;
    grr_ul_mes -> pfi                = gllc_lle[gas_id][lle].unnum.pfi;
    grr_ul_mes -> cause              = gllc_lle[gas_id][lle].unnum.cause;
    grr_ul_mes -> llc_pdu_type       = GRR_LL_PDU_NON_ACKSACK;
    grr_ul_mes -> pdu_ptr            = NULL;
    grr_ul_mes -> sapi               = gllc_sapi_addr_lookup[lle];
    grr_ul_mes -> test_mode          = TEST_MODE_OFF;

    grr_ul_mes -> pdu_prio           = GPDU_HIGH_PRIORITY; 

    /* Reset the unnumbered frame cause back to 'LLC Signalling' in case
       this frame is being used to convey a 'Page Response' or
       'Cell Update'. */

    gllc_lle[gas_id][lle].unnum.cause = GRR_CAUSE_LLC_SIGNALLING;

    /* Only one unnumbered response transmission requirement can be
       scheduled at a time and so the requirement can be cleared down
       immediately. */

    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_URES_EVT );
  }

  /* --------------------------------------------------------------------- */

  /* I Frame retransmission requirements must be given must be given next
     order of precedence. If supervisory transmission requirements have
     also been identified, then these may be 'piggy-backed' into an I+S
     Frame. */

  else if ( unmasked_trx_evts & GLLC_LLE_UL_I_FR_RETRX_EVT )
  {
    /* Find the next I frame in the retransmission buffer which is marked
       for retransmission. */

    item_p = gllc_lle[gas_id][lle].ack.retrx_first_p;
    while ( (item_p != NULL) && (!item_p -> retrx_flag) )
      item_p = item_p -> next_p;

    if ( item_p != NULL )
    {
      /* Retrx buffer element is marked for retransmission. Point at the
         frame, clear the retrx marking and update N(R) to current V(R). */

      item_p -> retrx_flag = FALSE;
      item_p -> frame.i_s_frame.recv_seq_num_N_R =
        gllc_lle[gas_id][lle].ack.recv_state_var_V_R;

      /* Collate message parameters in the GRR message assembly working
         structure. */

      gllc_assy_info[gas_id].frame            = &item_p -> frame;
      gllc_assy_info[gas_id].fr_info_length   = item_p -> l3_pdu_length;

      grr_ul_mes -> prim              = GRR_DATA_REQ;
      grr_ul_mes -> tlli                      = gllc_llme[gas_id].tlli;
      grr_ul_mes -> radio_priority    = item_p -> radio_priority;
      grr_ul_mes -> peak_throughput   = item_p -> peak_throughput;
      grr_ul_mes -> reliability_class = 0;
      grr_ul_mes -> sndcp_ref         = item_p -> sndcp_ref;
      grr_ul_mes -> pfi               = item_p -> pfi;
      grr_ul_mes -> cause             = item_p -> cause;
      grr_ul_mes -> pdu_ptr           = NULL;
      grr_ul_mes -> sapi              = gllc_sapi_addr_lookup[lle];
      grr_ul_mes -> test_mode         = TEST_MODE_OFF;

      /* Reset the frame cause back to 'data' in case this frame
         is being used to convey a 'Page Response' or 'Cell Update'. */

      item_p -> cause = GRR_CAUSE_LAY3_DATA;

      /* Supervisory transmission requirements may be 'piggy-backed' with
         this I frame. */

      gllc_mpex_embody_supervisory_content( gas_id, lle, unmasked_trx_evts );

      if ( (gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2 == GLLC_ACK ) ||
           (gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2 == GLLC_SACK)    )
      {
        grr_ul_mes -> llc_pdu_type = GRR_LL_PDU_ACKSACK;
      }
      else
        grr_ul_mes -> llc_pdu_type = GRR_LL_PDU_NON_ACKSACK;


      grr_ul_mes -> pdu_prio = GPDU_HIGH_PRIORITY;

      GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_S_FR_TRX_EVT );

      /* Update transmission statistics. */

      gllc_lle[gas_id][lle].diag.pdu_stats.ack_l3pdu_retx_cnt++;
      gllc_lle[gas_id][lle].diag.pdu_stats.ack_octet_retx_cnt +=
          item_p -> l3_pdu_length;

      /* Determine whether there are any more frames in the retrx buffer
         which are marked for retransmission */

      while ( (item_p != NULL) && (!item_p -> retrx_flag) )
        item_p = item_p -> next_p;

      if ( item_p == NULL )
        GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_I_FR_RETRX_EVT );

      gllc_mpex_diagnostic_messaging( gas_id, lle, gllc_assy_info[gas_id].frame, TRUE );
    }
    else
    {
      /* Retransmission requirement may have been cancelled by an
         intervening downlink acknowledgement event. Just clear down the
         event. */

      GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_I_FR_RETRX_EVT );
    }
  }

  /* --------------------------------------------------------------------- */

  /* I Frame first transmission requirements must be given next order of
     precedence. If supervisory transmission requirements have also been
     identified, then these may be 'piggy-backed' into an I+S Frame. */

  else if ( unmasked_trx_evts & GLLC_LLE_UL_I_FR_EVT )
  {
    /* The I frame requiring initial transmission is placed at the end of
       the retransmission buffer, as it contains highest modulo N(S). */

    item_p = gllc_lle[gas_id][lle].ack.retrx_last_p;

    if ( item_p != NULL )
    {
      /* Set frame's N(R) to current V(R) */

      item_p -> frame.i_s_frame.recv_seq_num_N_R =
        gllc_lle[gas_id][lle].ack.recv_state_var_V_R;

      /* Collate message parameters in the GRR message assembly working
         structure. */

      gllc_assy_info[gas_id].frame            = &item_p -> frame;
      gllc_assy_info[gas_id].fr_info_length   = item_p -> l3_pdu_length;

      grr_ul_mes -> prim              = GRR_DATA_REQ;
      grr_ul_mes -> tlli                      = gllc_llme[gas_id].tlli;
      grr_ul_mes -> radio_priority    = item_p -> radio_priority;
      grr_ul_mes -> peak_throughput   = item_p -> peak_throughput;
      grr_ul_mes -> reliability_class = 0;
      grr_ul_mes -> sndcp_ref         = item_p -> sndcp_ref;
      grr_ul_mes -> pfi               = item_p -> pfi;
      grr_ul_mes -> cause             = item_p -> cause;
      grr_ul_mes -> pdu_ptr           = NULL;
      grr_ul_mes -> sapi              = gllc_sapi_addr_lookup[lle];
      grr_ul_mes -> test_mode         = TEST_MODE_OFF;

      /* Reset the frame cause back to 'data' in case this frame
         is being used to convey a 'Page Response' or 'Cell Update'. */

      item_p -> cause = GRR_CAUSE_LAY3_DATA;

      /* Supervisory transmission requirements may be 'piggy-backed' with
         this I frame. */

      gllc_mpex_embody_supervisory_content( gas_id, lle, unmasked_trx_evts );

      if ( (gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2 == GLLC_ACK ) ||
           (gllc_assy_info[gas_id].frame -> i_s_frame.cmd_res_S1S2 == GLLC_SACK)    )
      {
        grr_ul_mes -> llc_pdu_type = GRR_LL_PDU_ACKSACK;
      }
      else
        grr_ul_mes -> llc_pdu_type = GRR_LL_PDU_NON_ACKSACK;


      /* I frames considered higher priority in GLLC. */

      grr_ul_mes -> pdu_prio       = GPDU_HIGH_PRIORITY;

      /* Update transmission statistics. */

      gllc_lle[gas_id][lle].diag.pdu_stats.ack_l3pdu_tx_cnt++;
      gllc_lle[gas_id][lle].diag.pdu_stats.ack_octet_tx_cnt +=
        item_p -> l3_pdu_length;

      gllc_mpex_diagnostic_messaging( gas_id, lle, gllc_assy_info[gas_id].frame, FALSE );
    }
    else
    {
      /* Error condition. I Frame transmission requirement is flagged for
         this LLE but there are no frames in the buffer. */

      MSG_GERAN_ERROR_1_G( "GLC LL %d: MPEX Trx Hdlr no I frame",
        gllc_sapi_addr_lookup[lle]);
    }

    /* Note that only one element is expected to have been placed in the
       retransmission buffer for initial transmission, and so the
       requirement can be cleared down immediately, along with any
       supervisory requirement which will have been embodied. */

    GLLC_CLR_LLE_UL_EVT
    (
      gas_id,
      lle,
      ( GLLC_LLE_UL_I_FR_EVT | GLLC_LLE_UL_S_FR_TRX_EVT )
    );
  }

  /* --------------------------------------------------------------------- */

  /* Supervisory transmission requirements must be given next order of
     precedence. No I frame (re)transmission requirements have been
     identified here, and so a standalone S frame must be transmitted. */

  else if ( unmasked_trx_evts & GLLC_LLE_UL_S_FR_TRX_EVT )
  {
    /* This supervisory transmission requirement is standalone, and so no
       frame content has yet been collated. This must be done before any
       GRR message assembly can take place. */

    gllc_assy_info[gas_id].frame             = &gllc_lle[gas_id][lle].ack.frame;
    gllc_assy_info[gas_id].frame -> format   = GLLC_S_FORMAT;

    gllc_assy_info[gas_id].frame -> s_frame.recv_seq_num_N_R =
      gllc_lle[gas_id][lle].ack.recv_state_var_V_R;

    gllc_mpex_embody_supervisory_content( gas_id, lle, unmasked_trx_evts );

    /* S Frames must be transfered with GRR_DATA_REQ primitives. */

    grr_ul_mes -> prim               = GRR_DATA_REQ;
    grr_ul_mes -> tlli               = gllc_llme[gas_id].tlli;
    grr_ul_mes -> radio_priority     = gllc_lle[gas_id][lle].ack.radio_priority;
    grr_ul_mes -> peak_throughput    = gllc_lle[gas_id][lle].ack.peak_throughput;
    grr_ul_mes -> reliability_class  = 0;
    grr_ul_mes -> sndcp_ref.nsapi    = 0;
    grr_ul_mes -> sndcp_ref.npdu_num = 0;
    grr_ul_mes -> sndcp_ref.seg_num  = 0;
    grr_ul_mes -> pfi                = gllc_lle[gas_id][lle].ack.pfi;
    grr_ul_mes -> cause              = gllc_lle[gas_id][lle].ack.cause;
    grr_ul_mes -> pdu_ptr            = NULL;

    if ( (gllc_assy_info[gas_id].frame -> s_frame.cmd_res_S1S2 == GLLC_ACK ) ||
         (gllc_assy_info[gas_id].frame -> s_frame.cmd_res_S1S2 == GLLC_SACK)    )
    {
      grr_ul_mes -> llc_pdu_type     = GRR_LL_PDU_ACKSACK;
    }
    else
      grr_ul_mes -> llc_pdu_type     = GRR_LL_PDU_NON_ACKSACK;

    grr_ul_mes -> sapi               = gllc_sapi_addr_lookup[lle];
    grr_ul_mes -> test_mode          = TEST_MODE_OFF;

    grr_ul_mes -> pdu_prio           = GPDU_HIGH_PRIORITY;

    /* Reset the acknowledged mode cause back to 'data' in case this frame
       is being used to convey a 'Page Response' or 'Cell Update'. */

    gllc_lle[gas_id][lle].ack.cause = GRR_CAUSE_LAY3_DATA;

    /* Only one S frame transmission requirement can be scheduled at a time
       and so the requirement can be cleared down immediately. */

    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_S_FR_TRX_EVT );

    gllc_mpex_diagnostic_messaging( gas_id, lle, gllc_assy_info[gas_id].frame, FALSE );
  }

  /* --------------------------------------------------------------------- */

  /* Unnumbered response transmission requirements must be given next order
     of precedence. */

  else if ( unmasked_trx_evts & GLLC_LLE_UL_U_FR_SRES_EVT )
  {
    gllc_assy_info[gas_id].frame = &gllc_lle[gas_id][lle].unnum.sol_res_frame;

    /* U Frames must be transfered with GRR_DATA_REQ primitives. */

    grr_ul_mes -> prim               = GRR_DATA_REQ;
    grr_ul_mes -> tlli               = gllc_llme[gas_id].tlli;
    grr_ul_mes -> radio_priority     = gllc_lle[gas_id][lle].unnum.radio_priority;
    grr_ul_mes -> peak_throughput    = gllc_lle[gas_id][lle].unnum.peak_throughput;
    grr_ul_mes -> reliability_class  = 0;
    grr_ul_mes -> sndcp_ref.nsapi    = 0;
    grr_ul_mes -> sndcp_ref.npdu_num = 0;
    grr_ul_mes -> sndcp_ref.seg_num  = 0;
    grr_ul_mes -> pfi                = gllc_lle[gas_id][lle].unnum.pfi;
    grr_ul_mes -> cause              = gllc_lle[gas_id][lle].unnum.cause;
    grr_ul_mes -> llc_pdu_type       = GRR_LL_PDU_U_FR_SRES;
    grr_ul_mes -> pdu_ptr            = NULL;
    grr_ul_mes -> sapi               = gllc_sapi_addr_lookup[lle];
    grr_ul_mes -> test_mode          = TEST_MODE_OFF;

    grr_ul_mes -> pdu_prio           = GPDU_HIGH_PRIORITY;

    /* Reset the unnumbered frame cause back to 'LLC Signalling' in case
       this frame is being used to convey a 'Page Response' or
       'Cell Update'. */

    gllc_lle[gas_id][lle].unnum.cause = GRR_CAUSE_LLC_SIGNALLING;

    /* Only one unnumbered response transmission requirement can be
       scheduled at a time and so the requirement can be cleared down
       immediately. */

    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_SRES_EVT );
  }

  /* --------------------------------------------------------------------- */

  /* Unnumbered command transmission requirements must be given next order
     of precedence. */

  else if ( unmasked_trx_evts & GLLC_LLE_UL_U_FR_CMD_EVT )
  {
    gllc_assy_info[gas_id].frame = &gllc_lle[gas_id][lle].unnum.cmd_frame;

    /* U Frames must be transfered with GRR_DATA_REQ primitives. */

    grr_ul_mes -> prim               = GRR_DATA_REQ;
    grr_ul_mes -> tlli               = gllc_llme[gas_id].tlli;
    grr_ul_mes -> radio_priority     = gllc_lle[gas_id][lle].unnum.radio_priority;
    grr_ul_mes -> peak_throughput    = gllc_lle[gas_id][lle].unnum.peak_throughput;
    grr_ul_mes -> reliability_class  = 0;
    grr_ul_mes -> sndcp_ref.nsapi    = 0;
    grr_ul_mes -> sndcp_ref.npdu_num = 0;
    grr_ul_mes -> sndcp_ref.seg_num  = 0;
    grr_ul_mes -> pfi                = gllc_lle[gas_id][lle].unnum.pfi;
    grr_ul_mes -> cause              = gllc_lle[gas_id][lle].unnum.cause;
    grr_ul_mes -> llc_pdu_type       = GRR_LL_PDU_NON_ACKSACK;
    grr_ul_mes -> pdu_ptr            = NULL;
    grr_ul_mes -> sapi               = gllc_sapi_addr_lookup[lle];
    grr_ul_mes -> test_mode          = TEST_MODE_OFF;

    grr_ul_mes -> pdu_prio           = GPDU_HIGH_PRIORITY;

    /* Reset the unnumbered frame cause back to 'LLC Signalling' in case
       this frame is being used to convey a 'Page Response' or
       'Cell Update'. */

    gllc_lle[gas_id][lle].unnum.cause = GRR_CAUSE_LLC_SIGNALLING;

    /* Only one unnumbered command transmission requirement can be
       scheduled at a time and so the requirement can be cleared down
       immediately. */

    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_U_FR_CMD_EVT );
  }

  /* --------------------------------------------------------------------- */

  /* UI Frame transmission requirements must be given next order of
     precedence. */

  else if ( unmasked_trx_evts & GLLC_LLE_UL_UI_FR_EVT )
  {
    gllc_assy_info[gas_id].frame          = &gllc_lle[gas_id][lle].unack.frame;
    gllc_assy_info[gas_id].fr_info_length = gllc_lle[gas_id][lle].unack.l3_pdu_length;

    if
    (
      GLLC_LLE_IS_3_5_9_11(lle) &&
      (
        (gllc_lle[gas_id][lle].unack.reliability_class == 4) ||
        (gllc_lle[gas_id][lle].unack.reliability_class == 5)
      )
    )
    {
      /* UI Frames for LL SAPIs 3, 5, 9 & 11 must be transfered with
         GRR_UNITDATA_REQ or GRR_DATA_REQ primitives as dependent on
         reliability class. */

      grr_ul_mes -> prim = GRR_UNITDATA_REQ;
    }
    else
    {
      /* UI Frames for all other LL SAPIs must be transfered with
         GRR_DATA_REQ primitives. */

      grr_ul_mes -> prim = GRR_DATA_REQ;
    }

    grr_ul_mes -> radio_priority     = gllc_lle[gas_id][lle].unack.radio_priority;
    grr_ul_mes -> peak_throughput    = gllc_lle[gas_id][lle].unack.peak_throughput;
    grr_ul_mes -> reliability_class  = gllc_lle[gas_id][lle].unack.reliability_class;
    grr_ul_mes -> sndcp_ref          = gllc_lle[gas_id][lle].unack.sndcp_ref;
    grr_ul_mes -> pfi                = gllc_lle[gas_id][lle].unack.pfi;
    grr_ul_mes -> tlli               = gllc_llme[gas_id].tlli;
    grr_ul_mes -> cause              = gllc_lle[gas_id][lle].unack.cause;
    grr_ul_mes -> llc_pdu_type       = GRR_LL_PDU_NON_ACKSACK;
    grr_ul_mes -> pdu_ptr            = NULL;
    grr_ul_mes -> sapi               = gllc_sapi_addr_lookup[lle];
    grr_ul_mes -> test_mode          = TEST_MODE_OFF;

    grr_ul_mes -> pdu_prio           = gllc_lle[gas_id][lle].unack.pdu_priority;

    grr_ul_mes -> transaction_id  = gllc_lle[gas_id][lle].unack.transaction_id;


    /* Set the unacknowledged mode cause to 'data' in case this
       frame is being used to convey a 'Page Response' or 'Cell Update'. */

    gllc_lle[gas_id][lle].unack.cause = GRR_CAUSE_LAY3_DATA;


    /* Update transmission statistics. */

    gllc_lle[gas_id][lle].diag.pdu_stats.unack_l3pdu_tx_cnt++;
    gllc_lle[gas_id][lle].diag.pdu_stats.unack_octet_tx_cnt +=
        gllc_lle[gas_id][lle].unack.l3_pdu_length;

    /* Only one UI frame transmission requirement can be scheduled at a time
       and so the requirement can be cleared down immediately. */

    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_UI_FR_EVT );

    gllc_mpex_diagnostic_messaging( gas_id, lle, gllc_assy_info[gas_id].frame, FALSE );
  }

  /* --------------------------------------------------------------------- */

  /* Error condition. An unmasked transmission event is flagged for this
     LLE but there are no unmasked transmission events. */

  else
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: MPEX Trx Hdlr empty reqt.",
      gllc_sapi_addr_lookup[lle]);
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, unmasked_trx_evts );
  }


  /* --------------------------------------------------------------------- */

  /* At this stage, grr_ul_mes -> tlli = gllc_llme[gas_id].tlli for any type of UL
     LLC frame. Now check if current TLLI is valid. If valid, then use
     current TLLI. If not valid, then check if old TLLI is valid. If valid,
     then use old TLLI. Otherwise, use invalid TLLI which should not
     happen. */

  if( grr_ul_mes -> tlli == GLLC_TLLI_UNASSIGNED )
  {
    if ( gllc_llme[gas_id].tlli_old != GLLC_TLLI_UNASSIGNED )
    {
      grr_ul_mes -> tlli = gllc_llme[gas_id].tlli_old;
    }
  }


  /* --------------------------------------------------------------------- */

  /* Build an LL PDU if the necessary source information has been collated
     in the GRR message assembly working structure, otherwise free the
     GRR UL Message resource. */

  if ( gllc_assy_info[gas_id].frame != NULL )
  {
    gllc_assemble_frame_hdr(gas_id);
    gllc_assemble_frame_info_and_fcs(gas_id);

    gllc_lle[gas_id][lle].diag.pdu_stats.llpdu_tx_cnt++;

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */

    /* Post GRR message to the RLC layer for transmission. */
    grr_ul_mes -> gas_id = gas_id;  /* Set gas_id in one place */

    grlc_llc_ul_put_mes_buf( grr_ul_mes );
  }
  else
    GPRS_MEM_FREE( grr_ul_mes );

  /* --------------------------------------------------------------------- */

} /* end of gllc_mpex_grr_ul_trx_event_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_CR_UL_Q_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Extracts ONE message from the next peer command/response message queue
===   to receive priority and submits it for up link state processing.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_cr_ul_q_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gllc_lle_cr_mes_t *mes_ptr;

  /* --------------------------------------------------------------------- */

  /* Extract a message from the appropriate queue and submit it for up link
     state processing.*/

  mes_ptr = q_get( &gllc_cr_ul_q[gas_id][lle] );

  if ( mes_ptr != NULL )
  {
    if ( mes_ptr -> mes.format == GLLC_U_FORMAT )
    {
      gllc_lle_process_u_frame(
        gas_id, lle, &mes_ptr -> mes.u_frame, mes_ptr -> mes.rxd_tlli );

      if ( mes_ptr -> mes.u_frame.xid != GLLC_XID_NONE )
      {
        if ( mes_ptr -> mes.u_frame.info_field.xid_info.contents &
             GLLC_LAY3_XID                                         )
        {
          if ( mes_ptr->mes.u_frame.info_field.xid_info.l3_xid.params != NULL )
            GPRS_PDU_FREE(
              &mes_ptr -> mes.u_frame.info_field.xid_info.l3_xid.params );
        }
      }
    }
    else if ( mes_ptr -> mes.format == GLLC_REJ_FORMAT )
      gllc_lle_process_rej_frame(
        gas_id, lle, &mes_ptr -> mes.rej_frame, mes_ptr -> mes.rxd_tlli );

    else
    {
      MSG_GERAN_ERROR_1_G( "GLC CR UL Hdlr: Invld Fr Fmt %d.",
        mes_ptr -> mes.format);
    }

    GPRS_MEM_FREE( mes_ptr );
  }

  /* --------------------------------------------------------------------- */

  /* Report any state changes for diagnostic logging purposes. */

  gllc_diag_rpt_lle_state( gas_id, lle );

  /* --------------------------------------------------------------------- */

  /* Clear the event flag if the queue is empty. */

  if ( q_cnt( &gllc_cr_ul_q[gas_id][lle] ) == 0 )
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_CR_Q_EVT );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_cr_ul_q_event_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_CR_UL_Q()
===
===  DESCRIPTION
===
===   All messages in the given LLE Peer Command/Response Queue are flushed
===   from the queue, and all allocated memory resource is de-allocated.
===   The related event flag is cleared.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_flush_cr_ul_q( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gllc_lle_cr_mes_t *cr_ul_mes;

  /* --------------------------------------------------------------------- */

  cr_ul_mes = q_get( &gllc_cr_ul_q[gas_id][lle] );

  while ( cr_ul_mes != NULL )
  {
    switch ( cr_ul_mes -> mes.format )
    {
    case GLLC_U_FORMAT:

      if ( (cr_ul_mes -> mes.u_frame.xid == GLLC_XID_REQ) ||
           (cr_ul_mes -> mes.u_frame.xid == GLLC_XID_NEG)    )
      {
        if ( cr_ul_mes -> mes.u_frame.info_field.
             xid_info.contents & GLLC_LAY3_XID    )
        {
          if ( cr_ul_mes -> mes.u_frame.info_field.
               xid_info.l3_xid.params != NULL       )
          {
            GPRS_PDU_FREE(
              &cr_ul_mes -> mes.u_frame.info_field.xid_info.l3_xid.params );
          }
        }
      }
      break;

    case GLLC_REJ_FORMAT:
      break;

    case GLLC_NO_FORMAT:
    default:
      MSG_GERAN_ERROR_1_G( "GLC CR UL Flush: Invld Fr Fmt %d.",
        cr_ul_mes -> mes.format);
      break;
    }

    GPRS_MEM_FREE( cr_ul_mes );
    cr_ul_mes = q_get( &gllc_cr_ul_q[gas_id][lle] );
  }

  GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_CR_Q_EVT );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_flush_cr_ul_q() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_UL_TIMER_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Extracts ONE timer event from the list of expired timers as a function
===   of LLE priority, and submits it for up link state processing.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_ul_timer_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  register uint32 unmasked_timer_evts;

  /* --------------------------------------------------------------------- */

  unmasked_timer_evts = (uint32)( gllc_lle_ul_event_map[gas_id][lle] &
                                  gllc_lle_ul_mask_map[gas_id][lle]  &
                                  GLLC_LLE_UL_TIMER_EVT        );

  /* Establish which timer has expired and submit the timer identity to the
     up link state process for further processing.

     Note that the action of processing the timer expiry event may cause
     the timer to be set to expire again immediately. Just in case there is
     a legitimate requirement to set a timer with expiry time of 0, it would
     be best to clear timer and event flags first BEFORE proceeding. */

  if ( unmasked_timer_evts & GLLC_LLE_UL_TIMER_T200_EVT )
  {
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T200_EVT );
    gllc_lle_process_timer_t200_expiry( gas_id, lle );
  }

  /* ..................................................................... */

  else if ( unmasked_timer_evts & GLLC_LLE_UL_TIMER_T201_EVT )
  {
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_TIMER_T201_EVT );
    gllc_lle_process_timer_t201_expiry( gas_id, lle );
  }

  /* --------------------------------------------------------------------- */

  /* Report any state changes for diagnostic logging purposes. */

  gllc_diag_rpt_lle_state( gas_id, lle );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_ul_timer_event_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_LL_UL_SIG_Q_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Extracts ONE message from the next LL SAP up link signalling message
===   queue to receive priority and submits it for up link state processing.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_ll_ul_sig_q_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gprs_ll_ul_sig_mes_t *ll_ul_mes;


  /* --------------------------------------------------------------------- */

  /* Extract a message from the appropriate LL SAP signalling queue. */

  ll_ul_mes = q_get( &gllc_ll_ul_sig_q[ gas_id ][ lle ] );

  if ( ll_ul_mes != NULL )
  {
    /* ------------------------------------------------------------------- */

    /* Submit the message for up link state processing, and if necessary free
       up any discarded message PDU chain resource. */

    switch ( ll_ul_mes -> mes.prim )
    {
      /* ................................................................. */

      case LL_ESTABLISH_REQ:
        gllc_lle_process_est_req_mes( gas_id, lle, &ll_ul_mes -> mes.est_req_mes );

      if ( ll_ul_mes -> mes.est_req_mes.l3_xid_req != NULL )
        GPRS_PDU_FREE( &ll_ul_mes -> mes.est_req_mes.l3_xid_req );
      break;

      /* ................................................................. */

      case LL_ESTABLISH_RES:
        gllc_lle_process_est_res_mes( gas_id, lle, &ll_ul_mes -> mes.est_res_mes );

      if ( ll_ul_mes -> mes.est_res_mes.l3_xid_neg != NULL )
        GPRS_PDU_FREE( &ll_ul_mes -> mes.est_res_mes.l3_xid_neg );
      break;

      /* ................................................................. */

      case LL_RELEASE_REQ:
        gllc_lle_process_rel_req_mes( gas_id, lle, &ll_ul_mes -> mes.rel_req_mes );
      break;

      /* ................................................................. */

      case LL_ACTIVATE_IND:
        gllc_lle_process_act_ind_mes( gas_id, lle, &ll_ul_mes -> mes.act_ind_mes );
      break;

      /* ................................................................. */

      case LL_DEACTIVATE_IND:
        gllc_lle_process_deact_ind_mes( gas_id, lle, &ll_ul_mes -> mes.deact_ind_mes );
      break;

      /* ................................................................. */

      case LL_XID_REQ:
        gllc_lle_process_xid_req_mes( gas_id, lle, &ll_ul_mes -> mes.xid_req_mes );

        if ( ll_ul_mes -> mes.xid_req_mes.l3_xid_req != NULL )
          GPRS_PDU_FREE( &ll_ul_mes -> mes.xid_req_mes.l3_xid_req );
      break;

      /* ................................................................. */

      case LL_XID_RES:
        gllc_lle_process_xid_res_mes( gas_id, lle, &ll_ul_mes -> mes.xid_res_mes );

        if ( ll_ul_mes -> mes.xid_res_mes.l3_xid_neg != NULL )
          GPRS_PDU_FREE( &ll_ul_mes -> mes.xid_res_mes.l3_xid_neg );
      break;

      /* ................................................................. */

      case LL_INACT_SAPI_RES:
        gllc_lle_process_inact_sapi_res_mes( gas_id, lle );
      break;

      /* ................................................................. */

      case LL_RESET_RES:
        gllc_lle_process_reset_res_mes( gas_id, lle );
      break;

      /* ................................................................. */

      default:
        MSG_GERAN_ERROR_2_G( "GLC LL %d: UL Sig Hdlr: Invld Prim %d.",
          gllc_sapi_addr_lookup[lle], ll_ul_mes -> mes.prim);
      break;

      /* ................................................................. */
    }

    GPRS_MEM_FREE( ll_ul_mes );
  }
  else
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: UL Sig Hdlr: NULL ul msg ptr ",
                gllc_sapi_addr_lookup[lle]);
  }

  /* --------------------------------------------------------------------- */

  /* Report any state changes for diagnostic logging purposes. */

  gllc_diag_rpt_lle_state( gas_id, lle );

  /* --------------------------------------------------------------------- */

  /* Clear the event flag if the queue is empty. */

  if ( q_cnt( &gllc_ll_ul_sig_q[gas_id][lle] ) == 0 )
    GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_LL_SIG_Q_EVT );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_ll_ul_sig_q_event_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_LL_UL_SIG_Q()
===
===  DESCRIPTION
===
===   All messages in the given LL SAP Signalling Queue are flushed from the
===   queue, and all allocated memory resource is de-allocated. The related
===   event flag is cleared.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_flush_ll_ul_sig_q( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gprs_ll_ul_sig_mes_t *ll_ul_mes;

  /* --------------------------------------------------------------------- */

  ll_ul_mes = q_get( &gllc_ll_ul_sig_q[gas_id][ lle ] );

  while ( ll_ul_mes != NULL )
  {
    /* Free up any message PDU chain resource. */

    switch ( ll_ul_mes -> mes.prim )
    {
    case LL_ESTABLISH_REQ:
      if ( ll_ul_mes -> mes.est_req_mes.l3_xid_req != NULL )
        GPRS_PDU_FREE( &ll_ul_mes -> mes.est_req_mes.l3_xid_req );
      break;

    case LL_ESTABLISH_RES:
      if ( ll_ul_mes -> mes.est_res_mes.l3_xid_neg != NULL )
        GPRS_PDU_FREE( &ll_ul_mes -> mes.est_res_mes.l3_xid_neg );
      break;

    case LL_XID_REQ:
      if ( ll_ul_mes -> mes.xid_req_mes.l3_xid_req != NULL )
        GPRS_PDU_FREE( &ll_ul_mes -> mes.xid_req_mes.l3_xid_req );
      break;

    case LL_XID_RES:
      if ( ll_ul_mes -> mes.xid_res_mes.l3_xid_neg != NULL )
        GPRS_PDU_FREE( &ll_ul_mes -> mes.xid_res_mes.l3_xid_neg );
      break;

    case LL_RELEASE_REQ:
      break;

    default:
      MSG_GERAN_ERROR_2_G( "GLC LL %d: UL Sig Flush: Invld Prim %d.",
        gllc_sapi_addr_lookup[lle], ll_ul_mes -> mes.prim);
      break;
    }

    GPRS_MEM_FREE( ll_ul_mes );
    ll_ul_mes = q_get( &gllc_ll_ul_sig_q[gas_id][ lle ] );
  }

  GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_LL_SIG_Q_EVT );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_flush_ll_ul_sig_q() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_LL_UL_ACK_Q_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Extracts ONE message from the next LL SAP up link acknowledged mode
===   data transfer request message queue to receive priority and submits
===   it for up link state processing.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_ll_ul_ack_q_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gprs_ll_ul_dat_mes_t  *ll_ul_mes;
  dsm_item_type         *lead_item_p;

  /* --------------------------------------------------------------------- */

  /* Extract a message from the appropriate LL SAP acknowledged data request
     queue. DSM dequeue processing handles flow control and event
     notification activities. */

  lead_item_p = dsm_dequeue( &gllc_ll_ul_ack_wm[gas_id][lle] );

  if ( lead_item_p != NULL )
  {
    ll_ul_mes = ( gprs_ll_ul_dat_mes_t * )lead_item_p -> app_field;

    if ( ll_ul_mes != NULL )
    {
      if ( ll_ul_mes -> mes.prim == LL_DATA_REQ )
      {
        /* Submit the message for up link state processing, and if necessary
           free up any discarded message PDU chain resource. */

        gllc_lle_process_data_req_mes
        (
          gas_id,
          lle,
          &ll_ul_mes -> mes.data_req_mes,
          (uint32)gllc_ll_ul_ack_wm[gas_id][lle].q_ptr -> cnt
        );

        if ( ll_ul_mes -> mes.data_req_mes.l3_pdu != NULL )
          GPRS_PDU_FREE( &ll_ul_mes -> mes.data_req_mes.l3_pdu );
      }
      else
      {
        MSG_GERAN_ERROR_2_G( "GLC LL %d: UL Ack Hdlr: Prim Err %d.",
          gllc_sapi_addr_lookup[lle], ll_ul_mes -> mes.prim);
      }

      GPRS_MEM_FREE( ll_ul_mes );
    }
    else
    {
      MSG_GERAN_ERROR_1_G( "GLC LL %d: UL Ack Hdlr: App Field Err.",
        gllc_sapi_addr_lookup[lle]);
      GPRS_PDU_FREE( &lead_item_p );
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: UL Ack Hdlr: NULL Pkt",
      gllc_sapi_addr_lookup[lle]);
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_ll_ul_ack_q_event_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_LL_UL_ACK_Q()
===
===  DESCRIPTION
===
===   All messages in the given LL SAP Acknowledged Data Queue are flushed
===   from the queue, and all allocated memory resource is de-allocated.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_flush_ll_ul_ack_q( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gprs_ll_ul_dat_mes_t  *ll_ul_mes;
  dsm_item_type         *lead_item_p;

  /* --------------------------------------------------------------------- */

  /* Extract a message from the appropriate LL SAP acknowledged data request
     queue. DSM dequeue processing handles flow control and event
     notification activities. */

  lead_item_p = dsm_dequeue( &gllc_ll_ul_ack_wm[gas_id][lle] );

  /* --------------------------------------------------------------------- */

  while ( lead_item_p != NULL )
  {
    ll_ul_mes = ( gprs_ll_ul_dat_mes_t * )lead_item_p -> app_field;
    GPRS_PDU_FREE( &lead_item_p );
    GPRS_MEM_FREE( ll_ul_mes );

    lead_item_p = dsm_dequeue( &gllc_ll_ul_ack_wm[gas_id][lle] );
  }

  /* --------------------------------------------------------------------- */

  GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_LL_ACK_Q_EVT );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_flush_ll_ul_ack_q() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_LL_UL_UNACK_Q_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Extracts ONE message from the next LL SAP up link unacknowledged mode
===   data transfer request message queue to receive priority and submits
===   it for up link state processing.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_lle_ll_ul_unack_q_event_handler( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gprs_ll_ul_dat_mes_t  *ll_ul_mes;
  dsm_item_type         *lead_item_p;

  /* --------------------------------------------------------------------- */

  /* Extract a message from the appropriate LL SAP unacknowledged data
     request queue. DSM dequeue processing handles flow control and event
     notification activities. */

  lead_item_p = dsm_dequeue( &gllc_ll_ul_unack_wm[gas_id][lle] );

  if ( lead_item_p != NULL )
  {
    ll_ul_mes = ( gprs_ll_ul_dat_mes_t * )lead_item_p -> app_field;

    if ( ll_ul_mes != NULL )
    {
      if ( ll_ul_mes -> mes.prim == LL_UNITDATA_REQ )
      {
        /* Submit the message for up link state processing, and if necessary
           free up any discarded message PDU chain resource. */

        gllc_lle_process_unitdata_req_mes(
          gas_id, lle, &ll_ul_mes -> mes.unitdata_req_mes );

        if ( ll_ul_mes -> mes.unitdata_req_mes.l3_pdu != NULL )
          GPRS_PDU_FREE( &ll_ul_mes -> mes.unitdata_req_mes.l3_pdu );
      }
      else
      {
        MSG_GERAN_ERROR_2_G( "GLC LL %d: UL Unack Hdlr: Invld Prim %d.",
          gllc_sapi_addr_lookup[lle], ll_ul_mes -> mes.prim);
      }

      GPRS_MEM_FREE( ll_ul_mes );
    }
    else
    {
      MSG_GERAN_ERROR_1_G( "GLC LL %d: UL Unack Hdlr: App Field Err",
        gllc_sapi_addr_lookup[lle]);
      GPRS_PDU_FREE( &lead_item_p );
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: UL Unack Hdlr: NULL Pkt",
      gllc_sapi_addr_lookup[lle]);
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_ll_ul_unack_q_event_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_FLUSH_LL_UL_UNACK_Q()
===
===  DESCRIPTION
===
===   All messages in the given LL SAP Unacknowledged Data Queue are flushed
===   from the queue, and all allocated memory resource is de-allocated.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_lle_flush_ll_ul_unack_q( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gprs_ll_ul_dat_mes_t  *ll_ul_mes;
  dsm_item_type         *lead_item_p;

  /* --------------------------------------------------------------------- */

  /* Extract a message from the appropriate LL SAP unacknowledged data
     request queue. DSM dequeue processing handles flow control and event
     notification activities. */

  lead_item_p = dsm_dequeue( &gllc_ll_ul_unack_wm[gas_id][lle] );

  /* --------------------------------------------------------------------- */

  while ( lead_item_p != NULL )
  {
    ll_ul_mes = ( gprs_ll_ul_dat_mes_t * )lead_item_p -> app_field;
    GPRS_PDU_FREE( &lead_item_p );
    GPRS_MEM_FREE( ll_ul_mes );

    lead_item_p = dsm_dequeue( &gllc_ll_ul_unack_wm[gas_id][lle] );
  }

  /* --------------------------------------------------------------------- */

  GLLC_CLR_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_LL_UNACK_Q_EVT );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_flush_ll_ul_unack_q() */


/*===========================================================================
===
===  FUNCTION      GLLC_UL_UNNUM_CR_PUT_MES()
===
===  DESCRIPTION
===
===    Constructs a message from the U frame command/response which is
===    currently stored in the frame disassembly working structure, and
===    queues the message to the uplink process CR message queue for uplink
===    state processing.
===
===  DEPENDENCIES
===
===    The disassembled frame must be a U frame.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

LOCAL void gllc_ul_unnum_cr_put_mes( gas_id_t gas_id )
{
  register gllc_lle_cr_mes_t *mes_ptr;

  /* --------------------------------------------------------------------- */

  mes_ptr = GPRS_MEM_MALLOC( sizeof(gllc_lle_cr_mes_t) / sizeof(uint8) );

  if ( mes_ptr == NULL )
  {
    ERR_GERAN_FATAL_0_G( "GLC UL CR Put: Heap Exhaustion.");
  }
  else
  {
    /* Copy frame and de-link any layer 3 XID item chain from the source
       U frame. */

    mes_ptr -> mes.rxd_tlli = gllc_disassy_info[gas_id].grr_dl_mes -> tlli;
    mes_ptr -> mes.u_frame  = gllc_disassy_info[gas_id].frame.u_frame;
    if ( gllc_disassy_info[gas_id].frame.u_frame.xid != GLLC_XID_NONE )
      gllc_disassy_info[gas_id].frame.u_frame.info_field.xid_info.l3_xid.params = NULL;

    /* Append message to the appropriate CR Up Link Messaging Queue,
       and set the appropriate event flags. */

    (void)q_link( mes_ptr, &mes_ptr -> link );
    q_put( &gllc_cr_ul_q[gas_id][ gllc_disassy_info[gas_id].lle ], &mes_ptr -> link );

    GLLC_SET_LLE_UL_EVT( gas_id, gllc_disassy_info[gas_id].lle, GLLC_LLE_UL_CR_Q_EVT );
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_ul_unnum_cr_put_mes() */


#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      GLLC_UL_XID_CMD_PSHO_PUT_MES()
===
===  DESCRIPTION
===
===    Constructs an XID frame using the XID parameters received from GMM
===    via LLGMM-PSHO-REQ message and queues the message to the uplink process
===    CR message queue for uplink state processing.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_ul_xid_cmd_psho_put_mes( gas_id_t gas_id, uint32 iov_ui )
{
  register gllc_lle_cr_mes_t *mes_ptr;
  uint32                     lle;
  boolean                    actv_sapi_available = FALSE;

  /* --------------------------------------------------------------------- */

  mes_ptr = GPRS_MEM_MALLOC( sizeof(gllc_lle_cr_mes_t) / sizeof(uint8) );

  if ( mes_ptr == NULL )
  {
    ERR_GERAN_FATAL_0_G( "GLC UL XID CMD PSHO Put: Heap Exhaustion.");
  }
  else
  {
    /* Prepare XID command */

    mes_ptr -> mes.rxd_tlli                             = gllc_llme[gas_id].tlli;
    mes_ptr -> mes.u_frame.cmd_res_M4_M1                = GLLC_XID;
    mes_ptr -> mes.u_frame.cmd_or_res_CR                = GLLC_COMMAND;
    mes_ptr -> mes.u_frame.format                       = GLLC_U_FORMAT;
    mes_ptr -> mes.u_frame.poll_final_PF                = GLLC_PF_POLL;
    mes_ptr -> mes.u_frame.xid                          = GLLC_XID_REQ;

    mes_ptr -> mes.u_frame.info_field.xid_info.contents =
      ( GLLC_IOV_UI_XID | GLLC_RESET_XID );
    mes_ptr -> mes.u_frame.info_field.xid_info.errors   = 0;

    mes_ptr -> mes.u_frame.info_field.xid_info.iov_ui_xid = iov_ui;


    /* Find an active SAPI */

    for ( lle = 0; lle < (uint32)GLLC_LL_NUM_SAPIS; lle++ )
    {
      if ( (gllc_lle[gas_id][lle].gen.pdp_active) &&
            GLLC_LLE_IS_3_5_9_11( (gllc_ll_sapi_t)lle )      )
      {
        actv_sapi_available = TRUE;
        break;
      }
    }

    /* Append message to the appropriate CR Up Link Messaging Queue,
       and set the appropriate event flags. */

    if ( ( actv_sapi_available ) && ( lle < (uint32)GLLC_LL_NUM_SAPIS) )
    {
      (void)q_link( mes_ptr, &mes_ptr -> link );
      q_put( &gllc_cr_ul_q[gas_id][ lle ], &mes_ptr -> link );

      GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_CR_Q_EVT );
    }
    else
    {
      MSG_GERAN_ERROR_0_G( "GLC UL PSHO XID CMD PUT: No active SAPI");
      GPRS_MEM_FREE( mes_ptr );
    }
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_ul_xid_cmd_psho_put_mes() */

#endif /* FEATURE_GPRS_PS_HANDOVER */


/*===========================================================================
===
===  FUNCTION      GLLC_UL_FRMR_PUT_MES()
===
===  DESCRIPTION
===
===    Constructs a frame reject message from the frame disassembly error
===    map, and queues the message to the uplink process CR message queue
===    for uplink state processing.
===
===  DEPENDENCIES
===
===    The frame disassembly error map must be checked for error marking
===    by the caller.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

LOCAL void gllc_ul_frmr_put_mes( gas_id_t gas_id )
{
  register gllc_lle_cr_mes_t *mes_ptr;

  /* --------------------------------------------------------------------- */

  mes_ptr = GPRS_MEM_MALLOC( sizeof(gllc_lle_cr_mes_t) / sizeof(uint8) );

  if ( mes_ptr == NULL )
  {
    ERR_GERAN_FATAL_0_G( "GLC UL FRMR Put: Heap Exhaustion.");
  }
  else
  {
    /* Construct a frame reject report in the form of a 'psuedo' frame */

    mes_ptr -> mes.format = GLLC_REJ_FORMAT;

    mes_ptr -> mes.rej_frame.fr_dis_err_map =
      gllc_disassy_info[gas_id].fr_dis_err_map;

    mes_ptr -> mes.rej_frame.rejected_frame_CR =
      gllc_disassy_info[gas_id].rejected_frame_CR;

    mes_ptr -> mes.rej_frame.rej_fr_ctrl_field[0] =
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[0];
    mes_ptr -> mes.rej_frame.rej_fr_ctrl_field[1] =
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[1];
    mes_ptr -> mes.rej_frame.rej_fr_ctrl_field[2] =
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[2];
    mes_ptr -> mes.rej_frame.rej_fr_ctrl_field[3] =
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[3];
    mes_ptr -> mes.rej_frame.rej_fr_ctrl_field[4] =
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[4];
    mes_ptr -> mes.rej_frame.rej_fr_ctrl_field[5] =
      gllc_disassy_info[gas_id].rej_fr_ctrl_field[5];

    /* Append message to the appropriate CR Up Link Messaging Queue,
       and set the appropriate event flags. */

    (void)q_link( mes_ptr, &mes_ptr -> link );
    q_put( &gllc_cr_ul_q[gas_id][ gllc_disassy_info[gas_id].lle ], &mes_ptr -> link );

    GLLC_SET_LLE_UL_EVT( gas_id, gllc_disassy_info[gas_id].lle, GLLC_LLE_UL_CR_Q_EVT );
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_ul_frmr_put_mes() */


/*===========================================================================
===
===  FUNCTION      GLLC_MPEX_GRR_DL_Q_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Extracts one message from the GRR down link message queue, and
===   performs a preliminary examination of the frame header to determine
===   a few key parameters.
===
===   These key parameters are primarily used to determine whether the
===   message LL PDU must be deciphered first before full frame disassembly
===   to the disassembly working structure is performed. Any peer command or
===   response information is redirected off to the LLC up link process to
===   deal with. The down link state processing is initiated to process
===   information frame content.
===
===   Message resource is deallocated and event flags are cleared if the
===   message queue has been emptied by the extraction of this message.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

LOCAL void gllc_mpex_grr_dl_q_event_handler( gas_id_t gas_id )
{
  register gprs_grr_dl_mes_t *grr_dl_mes;
           boolean           tlli_error     = FALSE;

  uint16 qcnt = 0;

  /* --------------------------------------------------------------------- */

  /* Extract a message from the GRR down link queue and prepare for LL PDU
     frame disassembly. */

  GLLC_DL_MSG_LOCK(gas_id);

  qcnt = q_cnt(&gllc_grr_dl_q[gas_id]);
  grr_dl_mes = q_get( &gllc_grr_dl_q[ gas_id ] );

  GLLC_DL_MSG_UNLOCK(gas_id);

  if ( grr_dl_mes == NULL )
  {
    /* There is no message on the queue yet the task mask is set!
    ** Correct this system fault by clearing the mask
    */
    MSG_GERAN_ERROR_1_G( "GLC GRR DL Hdlr: No Msg qcnt = %u", qcnt);

    if (0 == qcnt)
    {
      GLLC_CLR_DL_EVT( gas_id );
    }

    return;                                                  /*** RETURN ***/
  }

  /* --------------------------------------------------------------------- */

  if( grr_dl_mes -> gas_id != gllc_llme[gas_id].gas_id ) 
  {
    MSG_GERAN_ERROR_0_G("GLC GRR DL Hdlr: Invalid GAS_ID");     
  }

  /* --------------------------------------------------------------------- */

  else if ( (grr_dl_mes -> prim == GRR_DATA_IND    ) ||
       (grr_dl_mes -> prim == GRR_UNITDATA_IND)    )
  {
    /* Ensure that the GRR message TLLI relates to this MS. */

    if ( (gllc_llme[gas_id].tlli     == GLLC_TLLI_UNASSIGNED) ||
         (grr_dl_mes -> tlli == GLLC_TLLI_UNASSIGNED)    )
    {
      tlli_error = TRUE;
    }
    else if ( (grr_dl_mes -> tlli != gllc_llme[gas_id].tlli         ) &&
              (grr_dl_mes -> tlli != gllc_llme[gas_id].tlli_old     ) &&
              (grr_dl_mes -> tlli != gllc_llme[gas_id].tlli_last_rxd)    )
    {
      tlli_error = TRUE;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* A GRR message with an invalid TLLI field must be discarded... */

    if ( tlli_error )
    {
      MSG_GERAN_HIGH_1_G( "GLC GRR DL Hdlr: TLLI Error 0x%x", grr_dl_mes -> tlli);

      gllc_llme[gas_id].tlli_errors_ctr++;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* ... otherwise processing may continue. */

    else
    {
      /* Initialise relevent fields of the disassembly working structure. */

      gllc_disassy_info[gas_id].grr_dl_mes     = grr_dl_mes;
      gllc_disassy_info[gas_id].fr_dis_err_map = 0;
      (void)memset( gllc_disassy_info[gas_id].rej_fr_ctrl_field, 0, 6 );
      (void)memset( gllc_disassy_info[gas_id].fr_info_debug_sample, 0, 4 );

      /* ................................................................. */

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */

      /* ................................................................. */

      /* Perform frame disassembly. */

      gllc_disassemble_frame_hdr(gas_id);
      gllc_disassemble_frame_info_and_fcs(gas_id);

      /* ................................................................. */

      /* Ensure the LL SAP to which this frame is addressed bears a
         registered client, else flag addressing error. */

      if ( gllc_ll_client_record[ gas_id ][ gllc_disassy_info[gas_id].lle ].
           ll_dl_dat_put_mes_buf_fnc_ptr == NULL           )
      {
        gllc_disassy_info[gas_id].fr_dis_err_map |= GLLC_DIS_ADDR_ERR;
      }

      /* ................................................................. */

      /* Report invalid frames ... */

      if ( gllc_disassy_info[gas_id].fr_dis_err_map &
           (GLLC_DIS_SHORT_ERR | GLLC_DIS_FCS_ERR | GLLC_DIS_ADDR_ERR) )
      {
        boolean filler_pdu_match    = FALSE;
        boolean report_error        = FALSE;
        uint32  frame_ctrl_field    = 0;
        uint32  frame_info_field    = 0;

        /* Most network infrastructure manufacturers except Ericsson make
           use of dummy UI Frames which are intentionally inserted into the
           downlink traffic stream to ensure that DL TBFs are maintained
           over short interuptions in traffic flow. A characteristic UI
           Frame pattern could be, for example:

           43 c0 01 2b 2b 2b 2b 2b 2b 2b 2b ...
           8f c0 01 2b 2b 2b 2b 2b 2b 2b 2b ...

           These dummy UI frames will result in either FCS or ADDR errors.
           To prevent undesirable errored frame reporting, these need to be
           filtered by checking for an information field containing the
           RLC/MAC 2b filler octets. The frame lengths can vary but a
           maximum 3 octet comparison will suffice. */

        if ( gllc_disassy_info[gas_id].frame.format == GLLC_UI_FORMAT )
        {
          uint8  filler_pdu_pattern[3] = {0x2b,0x2b,0x2b};
          uint8  filler_pdu_suspect[3] = {0x00,0x00,0x00};
          uint16 filler_cmp_length;

          filler_cmp_length =
            GPRS_PDU_LENGTH( gllc_disassy_info[gas_id].grr_dl_mes -> ll_pdu );

          if ( filler_cmp_length > 3 )
            filler_cmp_length = 3;

          (void)gprs_pdu_strip_head
          (
            &gllc_disassy_info[gas_id].grr_dl_mes -> ll_pdu,
            &filler_pdu_suspect[0],
            filler_cmp_length
          );

          /* If the PDU carries an information field then examine this for the
             filler signature ... */

          if ( filler_cmp_length > 0 )
          {
            filler_pdu_match = TRUE;

            while ( filler_cmp_length )
            {
              filler_cmp_length--;

              if ( filler_pdu_suspect[filler_cmp_length] !=
                   filler_pdu_pattern[filler_cmp_length]    )
                filler_pdu_match = FALSE;
            }
          }

          /* ... else an empty UI frame in the SGSN to MS direction with an
             addressing or FCS error must be considered a filler match, as the
             FCS octets must have contained the filler octets. */

          else if ( (gllc_disassy_info[gas_id].fr_dis_err_map & GLLC_DIS_ADDR_ERR) ||
                    (gllc_disassy_info[gas_id].fr_dis_err_map & GLLC_DIS_FCS_ERR )    )
          {
            filler_pdu_match = TRUE;
          }
        }

#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
        #error code not present
#endif

        /* Genuine errors can now be reported with confidence. */

        if ( (gllc_disassy_info[gas_id].fr_dis_err_map & GLLC_DIS_ADDR_ERR) &&
             !filler_pdu_match                                         )
        {
          if ( gllc_disassy_info[gas_id].lle != GLLC_LL_SAPI_RESERVED )
            gllc_lle[gas_id][gllc_disassy_info[gas_id].lle].diag.pdu_stats.llpdu_addr_err_cnt++;
          else
            gllc_llme[gas_id].addr_errors_ctr++;

          MSG_GERAN_HIGH_0_G("GLC Fr E: ADDR ");
          report_error = TRUE;
        }

        if ( gllc_disassy_info[gas_id].fr_dis_err_map & GLLC_DIS_SHORT_ERR )
        {
          if ( gllc_disassy_info[gas_id].lle != GLLC_LL_SAPI_RESERVED )
            gllc_lle[gas_id][gllc_disassy_info[gas_id].lle].diag.pdu_stats.llpdu_short_err_cnt++;
          else
            gllc_llme[gas_id].short_errors_ctr++;

          MSG_GERAN_HIGH_0_G("GLC Fr E: SHORT ");
          report_error = TRUE;
        }

        if ( (gllc_disassy_info[gas_id].fr_dis_err_map & GLLC_DIS_FCS_ERR) &&
             !filler_pdu_match                                        )
        {
          if ( gllc_disassy_info[gas_id].lle != GLLC_LL_SAPI_RESERVED )
            gllc_lle[gas_id][gllc_disassy_info[gas_id].lle].diag.pdu_stats.llpdu_fcs_err_cnt++;
          else
            gllc_llme[gas_id].fcs_errors_ctr++;

          MSG_GERAN_HIGH_0_G("GLC Fr E: FCS ");
          report_error = TRUE;
        }

        if ( report_error )
        {
          frame_ctrl_field =
            ( ((uint32)(gllc_disassy_info[gas_id].rej_fr_ctrl_field[0]) << 24) |
              ((uint32)(gllc_disassy_info[gas_id].rej_fr_ctrl_field[1]) << 16) |
              ((uint32)(gllc_disassy_info[gas_id].rej_fr_ctrl_field[2]) <<  8) |
              ((uint32)(gllc_disassy_info[gas_id].rej_fr_ctrl_field[3]) <<  0)   );

          frame_info_field =
            ( ((uint32)(gllc_disassy_info[gas_id].fr_info_debug_sample[0]) << 24) |
              ((uint32)(gllc_disassy_info[gas_id].fr_info_debug_sample[1]) << 16) |
              ((uint32)(gllc_disassy_info[gas_id].fr_info_debug_sample[2]) <<  8) |
              ((uint32)(gllc_disassy_info[gas_id].fr_info_debug_sample[3]) <<  0)   );

          MSG_GERAN_HIGH_3_G( "GLC Fr E: Err CF:%08x IF:%08x IF Len:%d" ,
            frame_ctrl_field, frame_info_field,
            gllc_disassy_info[gas_id].fr_info_length );
        }
      }

      /* ................................................................. */

      else if ( !gllc_disassy_info[gas_id].fr_dis_err_map )
      {
        /* LL PDU disassembly has been successfull, so check if this is the
           first received frame on a new TLLI, and notify GMM if so. */

        if ( gllc_llme[gas_id].tlli_rxd == FALSE )
        {
          if ( grr_dl_mes -> tlli == gllc_llme[gas_id].tlli )
          {
            #ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
              gllc_llme_send_llgmm_new_tlli_ind_mes( gas_id, gllc_llme[gas_id].tlli );
            #endif
            gllc_llme[gas_id].tlli_last_rxd = gllc_llme[gas_id].tlli;
            gllc_llme[gas_id].tlli_rxd = TRUE;
          }
        }

        /* The disassembled frame can now be processed. */

        switch ( gllc_disassy_info[gas_id].frame.format )
        {
        case GLLC_I_S_FORMAT:

          gllc_lle_process_i_s_frame
          (
            gas_id,
            gllc_disassy_info[gas_id].lle,
            &gllc_disassy_info[gas_id].frame.i_s_frame,
            gllc_disassy_info[gas_id].fr_info_length,
            grr_dl_mes -> tlli
          );
          break;

        case GLLC_S_FORMAT:

          gllc_lle_process_s_frame
          (
            gas_id,
            gllc_disassy_info[gas_id].lle,
            &gllc_disassy_info[gas_id].frame.s_frame,
            grr_dl_mes -> tlli
          );
          break;

        case GLLC_UI_FORMAT:

          gllc_lle_process_ui_frame
          (
            gas_id,
            gllc_disassy_info[gas_id].lle,
            &gllc_disassy_info[gas_id].frame.ui_frame,
            gllc_disassy_info[gas_id].fr_info_length,
            grr_dl_mes -> tlli
          );
          break;

        case GLLC_U_FORMAT:

          /* Queue any unnumbered peer command/response signalling for up
             link processing. */

          gllc_ul_unnum_cr_put_mes(gas_id);
          break;

        default:

          MSG_GERAN_ERROR_1_G( "GLC GRR DL Hdlr: Format Err %d",
            gllc_disassy_info[gas_id].frame.format );
          break;
        }

        gllc_lle[gas_id][gllc_disassy_info[gas_id].lle].diag.pdu_stats.llpdu_rx_cnt++;
      }

      /* ................................................................. */

      else if ( gllc_disassy_info[gas_id].fr_dis_err_map & GLLC_DIS_FRMR_ERR )
      {
        /* Disassembly error marking indicates a Frame Reject Condition.
           Construct a frame reject information message and queue it for
           up link processing. */

        gllc_ul_frmr_put_mes( gas_id );

        if ( gllc_disassy_info[gas_id].lle != GLLC_LL_SAPI_RESERVED )
          gllc_lle[gas_id][gllc_disassy_info[gas_id].lle].diag.pdu_stats.llpdu_frm_rej_cnt++;
        else
          gllc_llme[gas_id].frmr_errors_ctr++;
      }
    }
  }

  /* --------------------------------------------------------------------- */

  else if ( grr_dl_mes -> prim == GRR_STATUS_IND )
  {
    gllc_llme_process_grr_status_ind_mes( gas_id, grr_dl_mes );
  }


  /* --------------------------------------------------------------------- */

  else if ( grr_dl_mes -> prim == GRR_RESUME_CNF )
  {
    gllc_llme_process_grr_resume_cnf_mes( gas_id );
  }  

  /* --------------------------------------------------------------------- */

  else
  {
    MSG_GERAN_HIGH_1_G( "GLC GRR DL Hdlr: Invld Prim %d", grr_dl_mes -> prim);
  }

  /* --------------------------------------------------------------------- */

  /* Firstly free any potentially discarded message PDU item chain resource,
     before freeing message resource. */

  if
  (
    ( grr_dl_mes -> prim == GRR_DATA_IND     ) ||
    ( grr_dl_mes -> prim == GRR_UNITDATA_IND )
  )
  {
    if ( grr_dl_mes -> ll_pdu != NULL )
      GPRS_PDU_FREE( &grr_dl_mes -> ll_pdu );
  }

  GPRS_MEM_FREE( grr_dl_mes );

  /* --------------------------------------------------------------------- */

  /* Clear the event flag if the queue is empty and clear the REX master
     signal if all event flags have been cleared. */

  /* the macro GLLC_CLR_DL_EVT() already locks and unlocks mutex but we want to lock
  ** around the access to the queue, at the same time to ensure atomic operation to
  ** the queue and the signal mask
  */
  GLLC_DL_MSG_LOCK(gas_id);

  if ( q_cnt( &gllc_grr_dl_q[gas_id] ) == 0 )
    GLLC_CLR_DL_EVT(gas_id);

  GLLC_DL_MSG_UNLOCK(gas_id);

  /* --------------------------------------------------------------------- */
} /* end of gllc_mpex_grr_dl_q_event_handler() */


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_GMM_READY_TIMER_HANDLER()
===
===  DESCRIPTION
===
===   The GMM Ready Timer is hosted by LLC on the behalf of GMM to avoid
===   having to signal GMM every time the timer is restarted on frame
===   transmission.
===
===   When the timer expires this handler serves to notify GMM via a message.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_gmm_ready_timer_handler( gas_id_t gas_id )
{
  /* Reset the ready timer in case the timer expiry was followed by a timer
     setting before the Rex signal could be serviced. */

  (void)rex_clr_timer( &gllc_gmm_ready_timer[ gas_id ] );
  (void)rex_clr_sigs( rex_self(), GLLC_GMM_RDY_TMR_SIG );
  gllc_gmm_ready_timer_active[ gas_id ] = FALSE;
  llc_log_ready_timer_status(gas_id);

  gllc_llme_send_llgmm_rdy_tmr_ind_mes( gas_id );

} /* end of gllc_gmm_ready_timer_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_TIMER_CALLBACK_1()
===
===  DESCRIPTION
===
===   For use when defining an extended REX timer. On timer expiry REX calls
===   the registered callback and presents a parameter which is specified at
===   the timer definition stage. In this case the parameter is the
===   associated LLE and timer event identifier.
===
===   Sets the appropropriate event bit of the LLE's event bitmap, and if
===   unmasked, the REX up link master signal is set.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_timer_callback_1( uint32 lle_and_timer_evt )
{
  gllc_ll_sapi_t lle       = (gllc_ll_sapi_t)(lle_and_timer_evt >> 16);
  uint16         timer_evt = (uint16)(lle_and_timer_evt & 0xFFFF);
  gas_id_t       gas_id    =  GERAN_ACCESS_STRATUM_ID_1;

  GLLC_SET_LLE_UL_EVT( gas_id, lle, timer_evt );

} /* end of gllc_timer_callback() */

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
===
===  FUNCTION      GLLC_TIMER_CALLBACK_2()
===
===  DESCRIPTION
===
===   For use when defining an extended REX timer. On timer expiry REX calls
===   the registered callback and presents a parameter which is specified at
===   the timer definition stage. In this case the parameter is the
===   associated LLE and timer event identifier.
===
===   Sets the appropropriate event bit of the LLE's event bitmap, and if
===   unmasked, the REX up link master signal is set.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_timer_callback_2( uint32 lle_and_timer_evt )
{
  gllc_ll_sapi_t lle       = (gllc_ll_sapi_t)(lle_and_timer_evt >> 16);
  uint16         timer_evt = (uint16)(lle_and_timer_evt & 0xFFFF);
  gas_id_t       gas_id    =  GERAN_ACCESS_STRATUM_ID_2;

  GLLC_SET_LLE_UL_EVT( gas_id, lle, timer_evt );

} /* end of gllc_timer_callback() */
#endif

#ifdef FEATURE_TRIPLE_SIM

/*===========================================================================
===
===  FUNCTION      GLLC_TIMER_CALLBACK_3()
===
===  DESCRIPTION
===
===   For use when defining an extended REX timer. On timer expiry REX calls
===   the registered callback and presents a parameter which is specified at
===   the timer definition stage. In this case the parameter is the
===   associated LLE and timer event identifier.
===
===   Sets the appropropriate event bit of the LLE's event bitmap, and if
===   unmasked, the REX up link master signal is set.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_timer_callback_3( uint32 lle_and_timer_evt )
{
  gllc_ll_sapi_t lle       = (gllc_ll_sapi_t)(lle_and_timer_evt >> 16);
  uint16         timer_evt = (uint16)(lle_and_timer_evt & 0xFFFF);
  gas_id_t       gas_id    =  GERAN_ACCESS_STRATUM_ID_3;

  GLLC_SET_LLE_UL_EVT( gas_id, lle, timer_evt );

} /* end of gllc_timer_callback() */

#endif /* FEATURE_TRIPLE_SIM */



/*===========================================================================
===
===  FUNCTION      GLLC_TEST_MODE_HANDLER()
===
===  DESCRIPTION
===
===   Handles ETSI Tests: Test Mode A & B and SRB.
===   ETSI Test Mode A requires the transmission of a specified number of
===   dummy LLC PDUs with pseudo-random content. One PDU is transmitted
===   per handler call.
===
===   On completion of all test PDU transmissions, ETSI Test Mode A is
===   exited.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gllc_test_mode_handler( gas_id_t gas_id )
{
  gprs_grr_ul_mes_t  *grr_ul_mes;
  uint16              test_llc_pdu_remaining_now;
  uint16              test_llc_pdu_remaining_next;

  /* ---------------------------------------------------------------------- */

  if( gllc_llme[gas_id].test_mode ==  0 )
  {
    GLLC_LOCK(gas_id);

    test_llc_pdu_remaining_now = gllc_llme[gas_id].test_llc_pdu_num;

    /* Valid range for finite number of UL PDUs is 1 to 4095. */
    if ( (gllc_llme[gas_id].test_llc_pdu_num > 0    ) &&
         (gllc_llme[gas_id].test_llc_pdu_num <= 4095)    )
    {
      gllc_llme[gas_id].test_llc_pdu_num--;
    }

    test_llc_pdu_remaining_next = gllc_llme[gas_id].test_llc_pdu_num;
    GLLC_UNLOCK(gas_id);

    if ( test_llc_pdu_remaining_now != 0 )
    {
      grr_ul_mes = gllc_assy_info[gas_id].grr_ul_mes =
        GPRS_MEM_MALLOC( sizeof(gprs_grr_ul_mes_t)/sizeof(uint8) );

      if ( grr_ul_mes != NULL )
      {
        /* Tag test mode A info to RLC PDU.*/
        grr_ul_mes -> test_mode         = TEST_MODE_A;

        grr_ul_mes -> srb_loopback_mode = gllc_llme[gas_id].loopback_mode;

        /* Collate a GRR UL message bearing a dummy test PDU. */

        grr_ul_mes -> prim = GRR_UNITDATA_REQ;
        grr_ul_mes -> tlli = gllc_llme[gas_id].tlli;

        grr_ul_mes -> radio_priority  =
          gllc_lle[gas_id][GLLC_LL_SAPI_1_GMM].unack.radio_priority;

        grr_ul_mes -> peak_throughput =
          gllc_lle[gas_id][GLLC_LL_SAPI_1_GMM].unack.peak_throughput;

        grr_ul_mes -> cause        = GRR_CAUSE_LAY3_DATA;
        grr_ul_mes -> llc_pdu_type = GRR_LL_PDU_NON_ACKSACK;

        grr_ul_mes -> reliability_class  = 
          gllc_lle[gas_id][GLLC_LL_SAPI_1_GMM].unack.reliability_class;

        grr_ul_mes -> sndcp_ref.nsapi    = 0;
        grr_ul_mes -> sndcp_ref.npdu_num = 0;
        grr_ul_mes -> sndcp_ref.seg_num  = 0;

        grr_ul_mes -> pfi                = 
          gllc_lle[gas_id][GLLC_LL_SAPI_1_GMM].unack.pfi;

        grr_ul_mes -> sapi               = 
          gllc_sapi_addr_lookup[GLLC_LL_SAPI_1_GMM];

        grr_ul_mes -> gas_id             = gas_id;

        grr_ul_mes -> pdu_prio           = GPDU_HIGH_PRIORITY;

        grr_ul_mes -> pdu_ptr = gllc_pdu_construct(
          gllc_llme[gas_id].test_llc_pdu_p, gllc_llme[gas_id].test_llc_pdu_size );

        /* Post GRR message to the RLC layer for transmission. */

        MSG_GERAN_LOW_2_G
        (
          "GLC ME: Sent TST PDU L:%d N:%d",
          gllc_llme[gas_id].test_llc_pdu_size,
          test_llc_pdu_remaining_next
        );

        /* Perform one last minute check to see if Etsi Test Mode A has been
           terminated by RLC through UL TBF release. If this is the case then
           the LLME record of remaining PDUs will have been forced to zero. */

        GLLC_LOCK(gas_id);
        if ( test_llc_pdu_remaining_next == gllc_llme[gas_id].test_llc_pdu_num )
        {
          /* Etsi Test Mode A is still active so send the PDU to RLC for
             transmission. */

          grlc_llc_ul_put_mes_buf( grr_ul_mes );
        }
        else
        {
          /* Etsi Test Mode A has been terminated at the last minute via
             UL TBF release indication from RLC. */

          GPRS_PDU_FREE( &grr_ul_mes -> pdu_ptr );
          GPRS_MEM_FREE( grr_ul_mes );
        }
        GLLC_UNLOCK(gas_id);
      }
      else
        ERR_GERAN_FATAL_0_G( "GLC ME: Test Mode A Hdlr: Heap Exhausted");
    }
    else
    {
      /* All Test PDUs have now been transmitted so exit Etsi Test Mode A. */

      MSG_GERAN_HIGH_0_G( "GLC ME: Info GPRS-TEST-MODE-A Complete");

      GLLC_LOCK(gas_id);
      gllc_llme[gas_id].state = GLLC_LLME_TLLI_ASSIGNED;
      GLLC_UNLOCK(gas_id);

      GPRS_MEM_FREE( gllc_llme[gas_id].test_llc_pdu_p );
      gllc_llme[gas_id].test_llc_pdu_p = NULL;

      if ( gas_id == GERAN_ACCESS_STRATUM_ID_1  )
      {
        (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_TEST_MODE_SIG );
      }
      else if ( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
      {
        (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2), GLLC_TEST_MODE_SIG );
      }

#ifdef FEATURE_TRIPLE_SIM

      else if ( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
      {
        (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3), GLLC_TEST_MODE_SIG );
      }

#endif /* FEATURE_TRIPLE_SIM */

      gllc_llme_clr_ul_event_mask_scheme( gas_id, GLLC_LLME_SCHEME_TEST_MODE );

#ifdef FEATURE_TC
      geran_send_cm_test_mode_off( gas_id );
#endif /* FEATURE_TC */
    }
  } /* End of Test Mode A */
  else if( gllc_llme[gas_id].test_mode == 0x01 || gllc_llme[gas_id].test_mode == 0x02 )
  {
    /* Test Mode B or SRB start command */
    if( !gllc_llme[gas_id].active_test_mode_b_or_srb )
    {
      grr_ul_mes = gllc_assy_info[gas_id].grr_ul_mes =
        GPRS_MEM_MALLOC( sizeof(gprs_grr_ul_mes_t)/sizeof(uint8) );

      if( grr_ul_mes != NULL )
      {
        grr_ul_mes -> prim            = GRR_TEST_MODE_REQ;
        grr_ul_mes -> ts_offset       = gllc_llme[gas_id].ts_offset;
        grr_ul_mes -> tlli            = gllc_llme[gas_id].tlli;

        if( gllc_llme[gas_id].test_mode == 0x01)
        {
          grr_ul_mes -> test_mode     = TEST_MODE_B;
          MSG_GERAN_MED_0_G( "GLC ME: Sent GRR-TEST-MODE-B-REQ");
        }
        else
        {
          grr_ul_mes -> test_mode     = TEST_MODE_SRB;
          MSG_GERAN_MED_0_G( "GLC ME: Sent GRR-TEST-MODE-SRB-REQ");
        }

        grr_ul_mes -> srb_loopback_mode = gllc_llme[gas_id].loopback_mode;
        grr_ul_mes -> peak_throughput   = 0;                         /* N/A */
        grr_ul_mes -> radio_priority    = 0;                         /* N/A */
        grr_ul_mes -> llc_pdu_type      = GRR_LL_PDU_NON_ACKSACK;    /* N/A */
        grr_ul_mes -> cause             = GRR_CAUSE_LLC_SIGNALLING;  /* N/A */
        grr_ul_mes -> pdu_ptr           = NULL;                      /* N/A */
        grr_ul_mes -> gas_id            = gas_id;

        grr_ul_mes -> pdu_prio          = GPDU_HIGH_PRIORITY;

        gllc_llme[gas_id].active_test_mode_b_or_srb  = TRUE;

        /* Post GRR message to the RLC layer. */

        grlc_llc_ul_put_mes_buf( grr_ul_mes );

        if ( gas_id == GERAN_ACCESS_STRATUM_ID_1  )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_TEST_MODE_SIG );
        }
        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2), GLLC_TEST_MODE_SIG );
        }

#ifdef FEATURE_TRIPLE_SIM

        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3), GLLC_TEST_MODE_SIG );
        }

#endif /* FEATURE_TRIPLE_SIM */

      }
      else
      {
        ERR_GERAN_FATAL_0_G( "GLC ME: Test Mode A Hdlr: Heap Exhausted");
      }
    }
    else
    {
      if( gllc_llme[gas_id].test_llc_pdu_num == 0 )
      {
        /* End of test Mede B or SRB */

        MSG_GERAN_HIGH_0_G( "GLC ME: Info GPRS-TEST-MODE Complete");

        gllc_llme[gas_id].active_test_mode_b_or_srb  = FALSE;

        GLLC_LOCK(gas_id);
        gllc_llme[gas_id].state = GLLC_LLME_TLLI_ASSIGNED;
        GLLC_UNLOCK(gas_id);

        if ( gas_id == GERAN_ACCESS_STRATUM_ID_1  )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_TEST_MODE_SIG );
        }
        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2), GLLC_TEST_MODE_SIG );
        }

#ifdef FEATURE_TRIPLE_SIM

        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3), GLLC_TEST_MODE_SIG );
        }

#endif /* FEATURE_TRIPLE_SIM */

        gllc_llme_clr_ul_event_mask_scheme( gas_id, GLLC_LLME_SCHEME_TEST_MODE );

#ifdef FEATURE_TC
        geran_send_cm_test_mode_off( gas_id );
#endif /* FEATURE_TC */
      }
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G( "GLC ME: Info Invalid GPRS-TEST-MODE %d",
      gllc_llme[gas_id].test_mode );
  }

  /* ---------------------------------------------------------------------- */
} /* end of gllc_test_mode_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_EVENT_HANDLER()
===
===  DESCRIPTION
===
===    For use by the LLC control task to invoke primary event dispatch.
===    The primary or master event in this case refers to a REX signal in
===    the OS domain. Serves as an entry function to the LLC sub-event
===    processing.
===
===    The LLC event management serves to prioritise the order of processing
===    based on the functional implications of processing the events.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    May result in a task context switch.
===
===========================================================================*/

void gllc_event_handler( gas_id_t in_gas_id )
{
  register uint32  lle;
  register uint32  lle_ul_event_flags;
  register uint32  lle_unmasked_event_map;
  const gas_id_t gas_id = check_gas_id(in_gas_id);
  
  lle      = lle_last[ gas_id ];

  /* ----------------------------------------------------------------------- */

  while ( gllc_task_sigs[ gas_id ] & GLLC_DL_MASTER_SIG )
    gllc_mpex_grr_dl_q_event_handler( gas_id );

  /* ----------------------------------------------------------------------- */

  while ( gllc_task_sigs[ gas_id ] & GLLC_UL_MASTER_SIG )
  {
    /* An Up Link Event is active. */

    lle_ul_event_flags = (uint32)gllc_lle_ul_event_flags[ gas_id ];

    if ( lle_ul_event_flags )
    {
      /* Determine which LLE is next in line to receive event handling
         service. */

      do
      {
        if ( lle != 0 )
          lle--;
        else
          lle = (uint32)GLLC_LL_LAST_SAPI;
      }
      while ( !(lle_ul_event_flags & (1 << lle)) );

      lle_last[ gas_id ] = lle;
    }

    GLLC_LOCK(gas_id);
    lle_unmasked_event_map =
      (uint32)(gllc_lle_ul_event_map[gas_id][lle] & gllc_lle_ul_mask_map[gas_id][lle]);
    GLLC_UNLOCK(gas_id);

    do
    {

      /* Logical link management messages from the GMM layer take highest
         order of precedence, and have the potential to clear all lower
         priority LLC events. */

      if ( gllc_ul_event_map[gas_id] & GLLC_UL_GMM_Q_EVT )
        gllc_llme_gmm_ul_q_event_handler( gas_id );

      /* ................................................................. */

      /* Re-check and handle any downlink event which may have become
         recently active. This may yield acknowledgement information from
         the peer which may cancel a scheduled I frame retransmission
         sequence. */

      else if ( gllc_task_sigs[ gas_id ] & GLLC_DL_MASTER_SIG )
      {
        do
          gllc_mpex_grr_dl_q_event_handler( gas_id );
        while ( gllc_task_sigs[ gas_id ] & GLLC_DL_MASTER_SIG );
      }

      /* ................................................................. */

      /* LLE LL SAP signalling messages from layer 3 must take order of
         precedence over any LLE specific event, as these have the potential
         to cancel/flush all other LLE specific events/messages. */

      else if ( GLLC_LLE_UL_LL_SIG_Q_EVT & lle_unmasked_event_map )
        gllc_lle_ll_ul_sig_q_event_handler( gas_id, (gllc_ll_sapi_t)lle );

      /* ................................................................. */

      /* LLE command/response messages from the peer LLE must take next order
         of precedence over any other LLE specific event, as these also have
         the potential to cancel/flush all other LLE specific events/msgs. */

      else if ( GLLC_LLE_UL_CR_Q_EVT & lle_unmasked_event_map )
        gllc_lle_cr_ul_q_event_handler( gas_id, (gllc_ll_sapi_t)lle );

      /* ................................................................. */

      /* Currently established up link transmit or retransmission requirements
         must take order of precedence over any other event which may
         result in the identification of NEW up link transmission reqts. */

      else if ( GLLC_LLE_UL_TRX_EVT & lle_unmasked_event_map )
      {
        /* If there is a supervisory S frame transmission requirement and
           currently no I frame transmission requirement on which to
           piggyback the supervisory content, then check for a candidate PDU
           in the LLE's uplink LL ACK queue first. */

        if ( GLLC_LLE_UL_S_FR_TRX_EVT & lle_unmasked_event_map )
        {
          if ( !(GLLC_LLE_UL_I_FR_TRX_EVT & lle_unmasked_event_map) )
          {
            if ( GLLC_LLE_UL_LL_ACK_Q_EVT & lle_unmasked_event_map )
              gllc_lle_ll_ul_ack_q_event_handler( gas_id, (gllc_ll_sapi_t)lle );
          }
        }
        gllc_mpex_grr_ul_trx_event_handler(
          gas_id, (gllc_ll_sapi_t)lle, lle_unmasked_event_map );
      }

      /* ................................................................. */

      /* LLE timer events relating to absent acknowledgement status from the
         peer entity, must take order of precedence over any other LLE
         specific event which may result in the identification of NEW up
         link transmission requirements. This allows retransmission
         requirements to be dealt with first. */

      else if ( GLLC_LLE_UL_TIMER_EVT & lle_unmasked_event_map )
        gllc_lle_ul_timer_event_handler( gas_id, (gllc_ll_sapi_t)lle );

      /* ................................................................. */

      /* LLE LL SAP acknowledged and unacknowledged mode data messages from
         layer 3 represent new transmission requirements and must therefore
         take a low order of precedence to allow all pending transmission
         requirements to be dealt with first. */

      else if ( GLLC_LLE_UL_LL_DAT_Q_EVT & lle_unmasked_event_map )
      {
        if ( GLLC_LLE_UL_LL_ACK_Q_EVT & lle_unmasked_event_map )
          gllc_lle_ll_ul_ack_q_event_handler( gas_id, (gllc_ll_sapi_t)lle );

        if ( GLLC_LLE_UL_LL_UNACK_Q_EVT & lle_unmasked_event_map )
          gllc_lle_ll_ul_unack_q_event_handler( gas_id, (gllc_ll_sapi_t)lle );
      }

      /* ................................................................. */

      /* An unidentified LLE event flag has become active. */

      else if ( lle_unmasked_event_map )
      {
        MSG_GERAN_ERROR_2_G
        (
          "GLC UL Evt Hdlr: LLE %d Invld event 0x%x.",
          gllc_sapi_addr_lookup[lle], lle_unmasked_event_map
        );

        GLLC_CLR_LLE_UL_EVT( gas_id, lle, lle_unmasked_event_map );
      }

      /* ................................................................. */

      /* An LLE event flag could be active in an unexpected mask. Check and
         clear that event. */

      else if ( !lle_unmasked_event_map )
      {
        MSG_GERAN_ERROR_3_G
        (
          "GLC UL Evt Hdlr: LLE %d Unexpected event map 0x%x, Event mask 0x%x.",
          gllc_sapi_addr_lookup[lle], 
          gllc_lle_ul_event_map[gas_id][lle],
          gllc_lle_ul_mask_map[gas_id][lle]
        );

        GLLC_CLR_LLE_UL_EVT( gas_id, lle, lle_unmasked_event_map );
      }

      /* ................................................................. */

      /* An unidentified LLME event flag has become active. */

      else
      {
        MSG_GERAN_ERROR_1_G
        (
          "GLC UL Evt Hdlr: Invld event 0x%x.",
          gllc_ul_event_map[gas_id]
        );

        GLLC_CLR_UL_EVT( gas_id, gllc_ul_event_map[gas_id] );
      }

      /* ................................................................. */

      GLLC_LOCK(gas_id);
      lle_unmasked_event_map =
        (uint32)(gllc_lle_ul_event_map[gas_id][lle] & gllc_lle_ul_mask_map[gas_id][lle]);
      GLLC_UNLOCK(gas_id);

      /* ................................................................. */
    }
    while ( gllc_ul_event_map[gas_id] || lle_unmasked_event_map );
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_event_handler() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_SET_UL_EVENT_MASK_SCHEME()
===
===  DESCRIPTION
===
===    Records the new LLME event masking scheme requirement, and applies
===    the scheme to each LLE event mask map from lookup tables of LLE event
===    masking requirements as indexed by scheme.
===
===    An appropriate decision is made as to whether the LLC UL REX master
===    signal should be cleared as dependent on whether there are any
===    unmasked events left outstanding after masking.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_llme_set_ul_event_mask_scheme
(
  gas_id_t                       in_gas_id,
  gllc_llme_ul_evt_mask_scheme_t llme_ul_scheme
)
{
  register uint32  lle;
  const gas_id_t gas_id = check_gas_id(in_gas_id);

  /* ---------------------------------------------------------------------- */

  GLLC_LOCK(gas_id);

  if ( !gllc_llme_ul_evt_mask_scheme_flags[ gas_id ][ llme_ul_scheme ] )
  {
    gllc_llme_ul_evt_mask_scheme_flags[ gas_id ][llme_ul_scheme] = TRUE;

    /* Apply the given LLME masking scheme to each LLE's event bit mask. */

    for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
    {
      gllc_lle_ul_mask_map[gas_id][lle] &=
        gllc_llme_ul_evt_mask_scheme_lookup[llme_ul_scheme][lle];

      if ( !(gllc_lle_ul_event_map[gas_id][lle] & gllc_lle_ul_mask_map[gas_id][lle]) )
        gllc_lle_ul_event_flags[gas_id] &= ~(uint16)(1 << lle);
    }

    /* Clear the UL master REX signal if there are no longer any unmasked
       events. */

    if ( !gllc_ul_event_map[ gas_id ] && !gllc_lle_ul_event_flags[gas_id] )
    {
      if ( gllc_task_sigs[ gas_id ] & GLLC_UL_MASTER_SIG )
      {
        if ( gas_id == GERAN_ACCESS_STRATUM_ID_1 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1) );
        }
        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2) );
        }

#ifdef FEATURE_TRIPLE_SIM

        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3) );
        }

#endif /* FEATURE_TRPLE_SIM */

      }
    }
  }

  GLLC_UNLOCK(gas_id);

  /* ---------------------------------------------------------------------- */
} /* end of gllc_llme_set_ul_event_mask_scheme() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_CLR_UL_EVENT_MASK_SCHEME()
===
===  DESCRIPTION
===
===    Records the new LLME event masking scheme requirement.
===
===    Runs through each of the currently enabled NON LLE specific and LLE
===    specific masking schemes, and recomputes each LLE's event mask
===    maps based on lookup tables of event masking requirements as indexed
===    by scheme.
===
===    An appropriate decision is made as to whether the LLC UL REX master
===    signal should be set as dependent on whether there are any unmasked
===    events left outstanding after masking.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_llme_clr_ul_event_mask_scheme
(
  gas_id_t                       in_gas_id,
  gllc_llme_ul_evt_mask_scheme_t llme_ul_scheme
)
{
  register uint32 scheme;
  register uint32 lle;
  const gas_id_t gas_id = check_gas_id(in_gas_id);  

  /* ---------------------------------------------------------------------- */

  GLLC_LOCK(gas_id);

  if ( gllc_llme_ul_evt_mask_scheme_flags[ gas_id ][ llme_ul_scheme ] )
  {
    gllc_llme_ul_evt_mask_scheme_flags[ gas_id ][llme_ul_scheme] = FALSE;

    /* Recompute each LLE's event bit mask as the combined effect of all LLE
       specific and NON LLE specific masking schemes which are currently in
       force. */

    for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
    {
      gllc_lle_ul_mask_map[gas_id][ lle ] = (uint16)~GLLC_LLE_UL_NO_EVT;

      for ( scheme = 0; scheme < (uint32)GLLC_LLME_NUM_SCHEMES; scheme++ )
      {
        if ( gllc_llme_ul_evt_mask_scheme_flags[ gas_id ][ scheme ] )
          gllc_lle_ul_mask_map[gas_id][ lle ] &=
            gllc_llme_ul_evt_mask_scheme_lookup[ scheme ][ lle ];
      }

      for ( scheme = 0; scheme < (uint32)GLLC_LLE_NUM_SCHEMES; scheme++ )
      {
        if ( gllc_lle_ul_evt_mask_scheme_flags[ gas_id ][ scheme ][ lle ] )
          gllc_lle_ul_mask_map[gas_id][ lle ] &=
            gllc_lle_ul_evt_mask_scheme_lookup[ scheme ];
      }

      if ( gllc_lle_ul_event_map[gas_id][ lle ] & gllc_lle_ul_mask_map[gas_id][ lle ] )
        gllc_lle_ul_event_flags[gas_id] |= (1 << lle);
    }

    /* If there are any LLE's with active unmasked events then set the UL
       Master REX signal if not set already. */

    if ( gllc_lle_ul_event_flags[gas_id] )
    {
      if ( !(gllc_task_sigs[ gas_id ] & GLLC_UL_MASTER_SIG) )
      {
        if( gas_id == GERAN_ACCESS_STRATUM_ID_1 )
        {
          (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1) );
        }
        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_2 ) 
        {
          (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2) );
        }

#ifdef FEATURE_TRIPLE_SIM

        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_3 ) 
        {
          (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3) );
        }

#endif /* FEATURE_TRPLE_SIM */

      }
    }
  }

  GLLC_UNLOCK(gas_id);

  /* ---------------------------------------------------------------------- */
} /* end of gllc_llme_clr_ul_event_mask_scheme() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SET_UL_EVENT_MASK_SCHEME()
===
===  DESCRIPTION
===
===    Records the new LLE event masking scheme requirement, and applies
===    the scheme to the given LLE event mask map from a lookup table of LLE
===    event masking requirements as indexed by scheme.
===
===    An appropriate decision is made as to whether the LLC UL REX master
===    signal should be cleared as dependent on whether there are any
===    unmasked events left outstanding after masking.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_lle_set_ul_event_mask_scheme
(
  gas_id_t                      in_gas_id,
  gllc_ll_sapi_t                lle,
  gllc_lle_ul_evt_mask_scheme_t lle_ul_scheme
)
{
  const gas_id_t gas_id = check_gas_id(in_gas_id);

  /* ---------------------------------------------------------------------- */

  GLLC_LOCK(gas_id);

  if ( !gllc_lle_ul_evt_mask_scheme_flags[ gas_id ][ lle_ul_scheme ][ lle ] )
  {
    gllc_lle_ul_evt_mask_scheme_flags[ gas_id ][ lle_ul_scheme ][ lle ] = TRUE;

    /* Apply the given masking scheme to the LLE's current event bit mask. */

    gllc_lle_ul_mask_map[ gas_id ][ lle ] &=
      gllc_lle_ul_evt_mask_scheme_lookup[ lle_ul_scheme ];

    if ( !(gllc_lle_ul_event_map[ gas_id ][ lle ] & gllc_lle_ul_mask_map[ gas_id ][ lle ]) )
      gllc_lle_ul_event_flags[ gas_id ] &= ~(uint16)(1 << (uint16)lle);

    /* Clear the UL master REX signal if there are no longer any unmasked
       events. */

    if ( !gllc_ul_event_map[ gas_id ] && !gllc_lle_ul_event_flags[ gas_id ] )
    {
      if ( gllc_task_sigs[ gas_id ] & GLLC_UL_MASTER_SIG )
      {
        if( gas_id == GERAN_ACCESS_STRATUM_ID_1 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1) );
        }
        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2) );
        }

#ifdef FEATURE_TRIPLE_SIM

        else if ( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
        {
          (void)rex_clr_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3) );
        }

#endif /* FEATURE_TRPLE_SIM */

      }
    }
  }

  GLLC_UNLOCK(gas_id);

  /* ---------------------------------------------------------------------- */
} /* end of gllc_lle_set_ul_event_mask_scheme() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_CLR_UL_EVENT_MASK_SCHEME()
===
===  DESCRIPTION
===
===    Records the new LLE specific event masking scheme requirement.
===
===    Runs through each of the currently enabled NON LLE specific and LLE
===    specific masking schemes, and recomputes the given LLE's event mask
===    maps based on lookup tables of event masking requirements as indexed
===    by scheme.
===
===    An appropriate decision is made as to whether the LLC UL REX master
===    signal should be set or cleared as dependent on whether there are
===    any unmasked events left outstanding after masking.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_lle_clr_ul_event_mask_scheme
(
  gas_id_t                      in_gas_id,
  gllc_ll_sapi_t                lle,
  gllc_lle_ul_evt_mask_scheme_t lle_ul_scheme
)
{
  register uint32 scheme;
  const gas_id_t gas_id = check_gas_id(in_gas_id);

  /* ---------------------------------------------------------------------- */

  GLLC_LOCK(gas_id);

  if ( gllc_lle_ul_evt_mask_scheme_flags[ gas_id ][ lle_ul_scheme ][ lle ] )
  {
    gllc_lle_ul_evt_mask_scheme_flags[ gas_id ][ lle_ul_scheme ][ lle ] = FALSE;

    /* Recompute the LLE's event bit mask as the combined effect of all LLE
       specific and NON LLE specific masking schemes which are currently in
       force. */

    gllc_lle_ul_mask_map[ gas_id ][ lle ] = (uint16)~GLLC_LLE_UL_NO_EVT;

    for ( scheme = 0; scheme < (uint32)GLLC_LLME_NUM_SCHEMES; scheme++ )
    {
      if ( gllc_llme_ul_evt_mask_scheme_flags[ gas_id ][ scheme ] )
        gllc_lle_ul_mask_map[ gas_id ][ lle ] &=
          gllc_llme_ul_evt_mask_scheme_lookup[ scheme ][ lle ];
    }

    for ( scheme = 0; scheme < (uint32)GLLC_LLE_NUM_SCHEMES; scheme++ )
    {
      if ( gllc_lle_ul_evt_mask_scheme_flags[ gas_id ][ scheme ][ lle ] )
        gllc_lle_ul_mask_map[ gas_id ][ lle ] &=
          gllc_lle_ul_evt_mask_scheme_lookup[ scheme ];
    }

    /* If the LLE has any unmasked events then flag the LLE and set the UL
       Master REX signal if not set already. */

    if ( gllc_lle_ul_event_map[ gas_id ][ lle ] & gllc_lle_ul_mask_map[ gas_id ][ lle ] )
    {
      gllc_lle_ul_event_flags[ gas_id ] |= (uint16)(1 << (uint16)lle);

      if ( !(gllc_task_sigs[ gas_id ] & GLLC_UL_MASTER_SIG) )
      {
        if( gas_id == GERAN_ACCESS_STRATUM_ID_1 )
        {
          (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1) );
        }
        else if( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
        {
          (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2) );
        }

#ifdef FEATURE_TRIPLE_SIM

        else if( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
        {
          (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3), GLLC_UL_MASTER_SIG );
          gllc_task_sigs[ gas_id ] = rex_get_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3) );
        }

#endif /* FEATURE_TRPLE_SIM */

      }
    }
  }

  GLLC_UNLOCK(gas_id);

  /* ---------------------------------------------------------------------- */
} /* end of gllc_lle_clr_ul_event_mask_scheme() */


 /*===========================================================================
===
===  FUNCTION      GLLC_LLE_CHK_UL_EVENT_MASK_SCHEME()
===
===  DESCRIPTION
===
===    Return value indicates whether a specific LLE event masking scheme is
===    set or not.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Boolean.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

boolean gllc_lle_chk_ul_event_mask_scheme
(
  gas_id_t                      gas_id,
  gllc_ll_sapi_t                lle,
  gllc_lle_ul_evt_mask_scheme_t lle_ul_scheme
)
{

  boolean is_ul_scheme_set = FALSE;


  GLLC_LOCK(gas_id);

  if ( gllc_lle_ul_evt_mask_scheme_flags[ gas_id ][ lle_ul_scheme ][ lle ] )
  {
    is_ul_scheme_set = TRUE;
  }

  GLLC_UNLOCK(gas_id);


  return ( is_ul_scheme_set );

} /* end of gllc_lle_chk_ul_event_mask_scheme() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_GET_CURRENT_OCTET_COUNT()
===
===  DESCRIPTION
===
===    Provides for a client of this service provision (eg GMM, SNDCP,
===    GSMS, etc) to directly enquire as to the quantity of buffered data
===    and signalling awaiting transmission in the LLC and RLC entities.
===
===    A typical usage of this enquiry would be in support of the Follow On
===    Request feature.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Octet count of data in LLC watermark interfaces, and in RLC awaiting
===    transmission.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

uint32 gllc_llme_get_current_octet_count( gas_id_t gas_id )
{
  uint32 octet_count = 0;
  uint32 lle;

  GLLC_LOCK(gas_id);

  for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
  {
    octet_count += gllc_ll_ul_unack_wm[gas_id][ lle ].current_cnt;
    octet_count += gllc_ll_ul_ack_wm[gas_id][ lle ].current_cnt;
  }

  GLLC_UNLOCK(gas_id);

  octet_count += grlc_llc_get_current_octet_count(gas_id);

  return ( octet_count );
}


/*===========================================================================
===
===  FUNCTION      GLLC_DEBUG_PRINT_LLME_CIPHER_PARAMS()
===
===  DESCRIPTION
===
===    Prints cipher/decipher parameters.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_debug_print_llme_cipher_params
(
  gas_id_t       gas_id
)
{

  MSG_GERAN_MED_1_G( "GLC ME: GEA %d",gllc_llme[gas_id].gea );

  MSG_GERAN_MED_1_G( "GLC ME: IOV-UI 0x%08x", gllc_llme[gas_id].input_offset_IOV_UI );

  if (( gllc_llme[gas_id].gea == GMMLLC_GEA_1    ) ||
      ( gllc_llme[gas_id].gea == GMMLLC_GEA_2    ) ||
      ( gllc_llme[gas_id].gea == GMMLLC_GEA_3    )
     )
  {

    MSG_GERAN_MED_2_G( "GLC ME: Kc[1]:0x%08x, Kc[0]:0x%08x",
      gllc_llme[gas_id].key_Kc[1], gllc_llme[gas_id].key_Kc[0]);

  }
  else if ( gllc_llme[gas_id].gea == GMMLLC_GEA_4 )
  {

    MSG_GERAN_MED_2_G( "GLC ME: Kc[1]:0x%08x, Kc[0]:0x%08x",
      gllc_llme[gas_id].key_Kc128[1], gllc_llme[gas_id].key_Kc128[0]);
    MSG_GERAN_MED_2_G( "GLC ME: Kc[3]:0x%08x, Kc[2]:0x%08x",
      gllc_llme[gas_id].key_Kc128[3], gllc_llme[gas_id].key_Kc128[2]);

  }

} /* gllc_debug_print_llme_cipher_params() */


/*===========================================================================
===
===  FUNCTION      GLLC_DEBUG_PRINT_LLE_CIPHER_PARAMS()
===
===  DESCRIPTION
===
===    Prints LLE cipher/decipher parameters.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_debug_print_lle_cipher_params
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle
)
{

  if ( ( gllc_lle[gas_id][lle].gen.pdp_active == TRUE               ) ||
       ( lle                                  == GLLC_LL_SAPI_1_GMM )
     )
  {

    MSG_GERAN_MED_3_G( "GLC LL %d: Encipher OC 0x%08x,Decipher OC 0x%08x", 
       gllc_sapi_addr_lookup[lle],
       gllc_lle[gas_id][lle].unack.encipher_oc,
       gllc_lle[gas_id][lle].unack.decipher_oc);


  }

} /* gllc_debug_print_lle_cipher_params() */


#ifdef DEBUG_GSM_GPRS_GLLC_FRAME
#error code not present
#endif /* DEBUG_GSM_GPRS_GLLC_FRAME */


/*** EOF: don't remove! ***/

