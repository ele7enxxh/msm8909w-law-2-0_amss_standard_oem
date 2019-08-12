/*****************************************************************************
***
*** TITLE
***
***  GMM & GSMS PROXY CLIENTS OF GPRS LL SAP SERVICE LAYER
***
***
*** DESCRIPTION
***
***  Provides an interface through which GMM and GSMS can connect to GPRS LLC
***  as clients of the LL SAP service layer. This proxy client adaptation
***  functionality adapts the messaging and data representation differences
***  between GMM/GSMS and GPRS LLC, and also registers for service at the
***  correct LL SAP addresses on behalf of GMM/GSMS.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  gmmllc_ll_get_mes_buf()
***  gmmllc_ll_put_mes()
***
***  gsmsllc_ll_get_mes_buf()
***  gsmsllc_ll_put_mes()
***
***  gllclient_reg_gmm_gsms_proxy()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  The LLC task implementation is expected to call function
***  gllclient_reg_gmm_gsms_proxy() at initialisation before any other
***  externalised function could be called.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/src/gllclient.c#1 $
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
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

#include "comdef.h"
#include "cfa.h"
#include "gllc.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gllclient.h"
#include "gllclog.h"
#include "gllsap.h"
#include "gprs_mem.h"
#include "gprs_pdu.h"
#include "gs.h"
#include "mmtask.h"
#include "mmtask_v.h"
#include "ms.h"
#include "geran_msgs.h"
#ifdef FEATURE_DUAL_SIM
#include "gllci.h"
#endif /* FEATURE_DUAL_SIM */
#include "gsndcp_v_g.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

#define GLLC_GPRS_TEST_MODE_CMD            0x24
#define GLLC_EGPRS_BLOCK_LOOPBACK_CMD      0x25
#define GLLC_GPRS_TEST_MODE_PDU_SIZE       500

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


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

LOCAL gprs_ll_dl_sig_mes_t *gllcmm_ll_sig_get_mes_buf( void );
LOCAL void gllcmm_ll_sig_put_mes_buf( gprs_ll_dl_sig_mes_t *mes_ptr );
LOCAL void gllcmm_ll_dat_put_mes_buf( gprs_ll_dl_dat_mes_t *mes_ptr );

LOCAL gprs_ll_dl_sig_mes_t *gllcsms_ll_sig_get_mes_buf( void );
LOCAL void gllcsms_ll_sig_put_mes_buf( gprs_ll_dl_sig_mes_t *mes_ptr );
LOCAL void gllcsms_ll_dat_put_mes_buf( gprs_ll_dl_dat_mes_t *mes_ptr );

LOCAL boolean gllcmm_intercept_test_mode_cmd( mm_cmd_type *cmd_ptr );


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#else
  #define GLLCMM_MEM_MALLOC(size)     GS_ALLOC(size)
#endif


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLCMM_LL_SIG_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Dynamically allocate sufficient memory from heap to accommodate a
===    message of type gprs_ll_dl_sig_mes_t.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to allocated gprs_ll_dl_sig_mes_t message buffer if resource
===    is available, otherwise NULL.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

LOCAL gprs_ll_dl_sig_mes_t *gllcmm_ll_sig_get_mes_buf( void )
{
  return ( GPRS_MEM_MALLOC(sizeof(gprs_ll_dl_sig_mes_t) / sizeof(uint8)) );

} /* end of gllcmm_ll_sig_get_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLCMM_LL_SIG_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Perform appropriate message adaptation and put the resultant
===    signalling message to the GMM command queue.
===
===  DEPENDENCIES
===
===    Message ptr must have been allocated by gllcmm_ll_sig_get_mes_buf().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    May cause a context switch.
===
===========================================================================*/

LOCAL void gllcmm_ll_sig_put_mes_buf( gprs_ll_dl_sig_mes_t *mes_ptr )
{
  mm_cmd_type     *cmd_ptr;

  /* --------------------------------------------------------------------- */

  #ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#endif

  /* --------------------------------------------------------------------- */

  switch ( mes_ptr -> mes.prim )
  {
  /* ..................................................................... */

  case LL_STATUS_IND:

    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LL_STATUS_IND );

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.ll_status_ind.tlli = mes_ptr -> mes.status_ind_mes.tlli;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.ll_status_ind.as_id =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.status_ind_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      if ( mes_ptr -> mes.status_ind_mes.cause == GPRS_LL_NO_PEER_RESPONSE )
      {
        cmd_ptr -> cmd.ll_status_ind.cause = GLLCMM_LL_NO_PEER_RESPONSE;
        cfa_log_packet_ex
        (
          GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LL_STATUS_IND,
          sizeof( gllcmm_ll_status_ind_type ),
          (uint8 *)&cmd_ptr -> cmd.ll_status_ind
        );
        mm_put_cmd( cmd_ptr );
      }
      else if ( mes_ptr -> mes.status_ind_mes.cause == GPRS_LL_INVALID_XID_RESP )
      {
        cmd_ptr -> cmd.ll_status_ind.cause = GLLCMM_LL_INVALID_XID_RESP;
        cfa_log_packet_ex
        (
          GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LL_STATUS_IND,
          sizeof( gllcmm_ll_status_ind_type ),
          (uint8 *)&cmd_ptr -> cmd.ll_status_ind
        );
        mm_put_cmd( cmd_ptr );
      }
      else
      {
        /* Any other status message should not originate from the LLE
           supporting GMM. */

        GPRS_MEM_FREE( cmd_ptr );

        MSG_GERAN_HIGH_1("GLLGMM CLIENT: Filtered Status Cause %d",
          mes_ptr -> mes.status_ind_mes.cause);
      }
    }
    else
      ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case LL_RESET_IND:

    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LL_RESET_IND );

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.ll_reset_ind.tlli = mes_ptr -> mes.rst_ind_mes.tlli;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.ll_reset_ind.as_id =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.rst_ind_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      cfa_log_packet_ex
      (
        GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LL_RESET_IND,
        sizeof( gllcmm_ll_reset_ind_type ),
        (uint8 *)&cmd_ptr -> cmd.ll_reset_ind
      );
      mm_put_cmd( cmd_ptr );
    }
    else
      ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case LL_XID_IND:

    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LL_XID_IND );

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.ll_xid_ind.tlli   = mes_ptr -> mes.xid_ind_mes.tlli;
      cmd_ptr -> cmd.ll_xid_ind.n201_u = mes_ptr -> mes.xid_ind_mes.n201_u;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.ll_xid_ind.as_id  =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.xid_ind_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      if ( mes_ptr -> mes.xid_ind_mes.l3_xid_req != NULL )
      {
        cmd_ptr -> cmd.ll_xid_ind.l3_xid_req_len =
          GPRS_PDU_LENGTH( mes_ptr -> mes.xid_ind_mes.l3_xid_req );

        cmd_ptr -> cmd.ll_xid_ind.l3_xid_req =
          GLLCMM_MEM_MALLOC( cmd_ptr -> cmd.ll_xid_ind.l3_xid_req_len );

        if ( cmd_ptr -> cmd.ll_xid_ind.l3_xid_req != NULL )
        {
          (void)gprs_pdu_strip_head
          (
            &mes_ptr -> mes.xid_ind_mes.l3_xid_req,
            cmd_ptr  -> cmd.ll_xid_ind.l3_xid_req,
            cmd_ptr  -> cmd.ll_xid_ind.l3_xid_req_len
          );
        }
        else
          ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
      }
      else
      {
        cmd_ptr -> cmd.ll_xid_ind.l3_xid_req     = NULL;
        cmd_ptr -> cmd.ll_xid_ind.l3_xid_req_len = 0;
      }

      cfa_log_packet_ex
      (
        GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LL_XID_IND,
        sizeof( gllcmm_ll_xid_ind_type ),
        (uint8 *)&cmd_ptr -> cmd.ll_xid_ind
      );
      mm_put_cmd( cmd_ptr );
    }
    else
      ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case LL_XID_CNF:

    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LL_XID_CNF );

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.ll_xid_cnf.tlli   = mes_ptr -> mes.xid_cnf_mes.tlli;
      cmd_ptr -> cmd.ll_xid_cnf.n201_u = mes_ptr -> mes.xid_cnf_mes.n201_u;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.ll_xid_cnf.as_id  =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.xid_cnf_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      if ( mes_ptr -> mes.xid_cnf_mes.l3_xid_neg != NULL )
      {
        cmd_ptr -> cmd.ll_xid_cnf.l3_xid_neg_len =
          GPRS_PDU_LENGTH( mes_ptr -> mes.xid_cnf_mes.l3_xid_neg );

        cmd_ptr -> cmd.ll_xid_cnf.l3_xid_neg =
          GLLCMM_MEM_MALLOC( cmd_ptr -> cmd.ll_xid_cnf.l3_xid_neg_len );

        if ( cmd_ptr -> cmd.ll_xid_cnf.l3_xid_neg != NULL )
        {
          (void)gprs_pdu_strip_head
          (
            &mes_ptr -> mes.xid_cnf_mes.l3_xid_neg,
            cmd_ptr  -> cmd.ll_xid_cnf.l3_xid_neg,
            cmd_ptr  -> cmd.ll_xid_cnf.l3_xid_neg_len
          );
        }
        else
          ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
      }
      else
      {
        cmd_ptr -> cmd.ll_xid_cnf.l3_xid_neg_len = 0;
        cmd_ptr -> cmd.ll_xid_cnf.l3_xid_neg     = NULL;
      }

      cfa_log_packet_ex
      (
        GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LL_XID_CNF,
        sizeof( gllcmm_ll_xid_cnf_type ),
        (uint8 *)&cmd_ptr -> cmd.ll_xid_cnf
      );
      mm_put_cmd( cmd_ptr );
    }
    else
      ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  default:
    MSG_GERAN_HIGH_1("GLLGMM CLIENT: Filtered Msg Id %d",
      mes_ptr -> mes.prim);

  /* ..................................................................... */
  }

  GPRS_MEM_FREE( mes_ptr );

  /* --------------------------------------------------------------------- */
} /* end of gllcmm_ll_sig_put_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLCMM_LL_DAT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Perform appropriate message adaptation and put the resultant data
===    bearing message to the GMM command queue.
===
===  DEPENDENCIES
===
===    Caller is expected to have preallocated gprs_ll_dl_sig_mes_t message
===    resource from heap.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    May cause a context switch.
===
===========================================================================*/

LOCAL void gllcmm_ll_dat_put_mes_buf( gprs_ll_dl_dat_mes_t *mes_ptr )
{
  mm_cmd_type     *cmd_ptr;

  /* --------------------------------------------------------------------- */

  #ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#endif

  /* --------------------------------------------------------------------- */

  if ( mes_ptr -> mes.prim == LL_UNITDATA_IND )
  {
#ifdef TEST_FRAMEWORK
#error code not present
#else

    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCMM_LL_UNITDATA_IND );

#endif /* TEST_FRAMEWORK */

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.ll_unitdata_ind.tlli =
        mes_ptr -> mes.unitdata_ind_mes.tlli;
      cmd_ptr -> cmd.ll_unitdata_ind.ciphered =
        mes_ptr -> mes.unitdata_ind_mes.ciphered;
      cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu_len =
        GPRS_PDU_LENGTH( mes_ptr -> mes.unitdata_ind_mes.l3_pdu );
      cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu =
        GLLCMM_MEM_MALLOC( cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu_len );
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.ll_unitdata_ind.as_id  =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.unitdata_ind_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      if ( cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu != NULL )
      {
        (void)gprs_pdu_strip_head
        (
          &mes_ptr -> mes.unitdata_ind_mes.l3_pdu,
          cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu,
          cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu_len
        );

        if ( !gllcmm_intercept_test_mode_cmd(cmd_ptr) )
        {
          /* ATM: Just log the l3_pdu portion for CFA */

#ifndef TEST_FRAMEWORK
          cfa_log_packet_ex
          (
            GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCMM_LL_UNITDATA_IND,
            cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu_len,
            cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu
          );

          llc_log_ota_message
          (
            mes_ptr -> mes.unitdata_ind_mes.gas_id,
            TRUE, /* Downlink */
            cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu[1],
            cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu_len,
            cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu
          );

          mm_put_cmd( cmd_ptr );
#endif
        }
        else
        {
          GPRS_MEM_FREE( cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu );
          GPRS_MEM_FREE( cmd_ptr );
        }
      }
      else
        ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
    }
    else
      ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
  }
  else
    MSG_GERAN_HIGH_1("GLLGMM CLIENT: Filtered Msg Id %d", mes_ptr -> mes.prim);

  GPRS_MEM_FREE( mes_ptr );

  /* --------------------------------------------------------------------- */
} /* end of gllcmm_ll_dat_put_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLCMM_INTERCEPT_TEST_MODE_CMD()
===
===  DESCRIPTION
===
===    Intercepts any GPRS Test Mode related messages.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE if MM Command should be intercepted, FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

LOCAL boolean gllcmm_intercept_test_mode_cmd( mm_cmd_type *cmd_ptr )
{
  boolean               intercepted = FALSE;
  gmmllc_llgmm_mes_type *mes_ptr;
  uint8                 L, mode, ts_offset;
  uint16                d;

  /* Check the length, 'Protocol Discriminator', 'Skip Indicator', and
     'Message Type' fields of the OTA layer 3 message... */

  if
  (
    (
      (cmd_ptr->cmd.ll_unitdata_ind.l3_pdu_len == 5  )||
      (cmd_ptr->cmd.ll_unitdata_ind.l3_pdu_len == 3  )   )&&
    (cmd_ptr->cmd.ll_unitdata_ind.l3_pdu[0]  == 0x0F     )
  )
  {
    if( cmd_ptr->cmd.ll_unitdata_ind.l3_pdu[1]  == GLLC_EGPRS_BLOCK_LOOPBACK_CMD )
    {
     /* EGPRS Switched Radio Block Loopback Mode command */
      intercepted = TRUE;

      mode      =   cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu[2] & 0x01;
      ts_offset =  (cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu[2] & 0x0F) >> 1;

      MSG_GERAN_HIGH_2( "GLLGMM CLIENT: EGPRS_BLOCK_LOOPBACK_CMD  M=%d ts_offset=%d Intercept",
                  mode, ts_offset);

      mes_ptr = gmmllc_llgmm_get_mes_buf();

      if ( mes_ptr != NULL )
      {
        mes_ptr -> mes.llgmm_test_mode_req.prim =
          GMMLLC_LLGMM_TEST_MODE_REQ;

        /* Set pdu number to a non zero value to indicate start of
           test mode B. The pdu number is set to zero when the UL/DL
           TBF is relased to indicate the end of the SRB test */

        mes_ptr -> mes.llgmm_test_mode_req.llc_pdu_num   = 0xFF;

        mes_ptr -> mes.llgmm_test_mode_req.test_mode     = 0x02;

        mes_ptr -> mes.llgmm_test_mode_req.loopback_mode = mode;

        mes_ptr -> mes.llgmm_test_mode_req.ts_offset     = ts_offset;

#ifdef FEATURE_DUAL_SIM
        mes_ptr -> mes.llgmm_test_mode_req.as_id         =
          cmd_ptr -> cmd.ll_unitdata_ind.as_id;
#endif /* FEATURE_DUAL_SIM */

        gmmllc_llgmm_put_mes_buf( mes_ptr );
      }
      else
        ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
    }
    else if( cmd_ptr->cmd.ll_unitdata_ind.l3_pdu[1]  == GLLC_GPRS_TEST_MODE_CMD )
    {
      intercepted = TRUE;

      L = (cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu[2] & 0x80) >> 7;
      d = ((cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu[2] & 0xF) << 8) |
                     cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu[3];

      mode = cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu[4] & 0x01;

      ts_offset = (cmd_ptr -> cmd.ll_unitdata_ind.l3_pdu[4] & 0x0F) >> 1;

      MSG_GERAN_HIGH_2( "GLLGMM CLIENT: GPRS_TEST_MODE_CMD M=%d ts_offset=%d Intercept",
                  mode, ts_offset);

      /* d = 0 means an infinite number of UL PDUs requested. Valid range for
         finite number of UL PDUs is 1 to 4095. To facilitate infinite number
         of PDUs, set d = 0xFFFF */

      if ( d == 0 )
      {
        d = 0xFFFF;
      }

      if ( (L == 1) && (d != 0) && (mode == 0 ) )
      {
        mes_ptr = gmmllc_llgmm_get_mes_buf();

        if ( mes_ptr != NULL )
        {
          mes_ptr -> mes.llgmm_test_mode_req.prim =
            GMMLLC_LLGMM_TEST_MODE_REQ;

          mes_ptr -> mes.llgmm_test_mode_req.llc_pdu_num   = d;

          mes_ptr -> mes.llgmm_test_mode_req.llc_pdu_size  =
              GLLC_GPRS_TEST_MODE_PDU_SIZE;

          mes_ptr -> mes.llgmm_test_mode_req.test_mode     = 0;

          mes_ptr -> mes.llgmm_test_mode_req.loopback_mode = mode;

#ifdef FEATURE_DUAL_SIM
          mes_ptr -> mes.llgmm_test_mode_req.as_id         =
            cmd_ptr -> cmd.ll_unitdata_ind.as_id;
#endif /* FEATURE_DUAL_SIM */

          gmmllc_llgmm_put_mes_buf( mes_ptr );
        }
        else
          ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
      }
      else if( mode == 1 )
      {
        mes_ptr = gmmllc_llgmm_get_mes_buf();

        if ( mes_ptr != NULL )
        {
          mes_ptr -> mes.llgmm_test_mode_req.prim =
            GMMLLC_LLGMM_TEST_MODE_REQ;

          /* Set pdu number to a non zero value to indicate start of
             test mode B. The pdu number is set to zero when the UL/DL
             TBF is relased to indicate the end of the test mode B */

          mes_ptr -> mes.llgmm_test_mode_req.llc_pdu_num   = 0xFF;

          mes_ptr -> mes.llgmm_test_mode_req.test_mode     = 0x01;

          mes_ptr -> mes.llgmm_test_mode_req.loopback_mode = mode;

          mes_ptr -> mes.llgmm_test_mode_req.ts_offset     = ts_offset;

#ifdef FEATURE_DUAL_SIM
          mes_ptr -> mes.llgmm_test_mode_req.as_id         =
            cmd_ptr->cmd.ll_unitdata_ind.as_id;
#endif /* FEATURE_DUAL_SIM */

          gmmllc_llgmm_put_mes_buf( mes_ptr );
        }
        else
          ERR_GERAN_FATAL_0( "GLLGMM CLIENT: Heap Exhaustion.");
      }
    }
  }
  /* else check for GMM Information messages of length 2 only and discard,
     as the Agilent 8960 insists on transmitting these in BLER test mode. */

  else if ( (cmd_ptr->cmd.ll_unitdata_ind.l3_pdu[0]  == 0x08) &&
            (cmd_ptr->cmd.ll_unitdata_ind.l3_pdu[1]  == 0x21) &&
            (cmd_ptr->cmd.ll_unitdata_ind.l3_pdu_len == 2   )    )
  {
    intercepted = TRUE;
  }

  return ( intercepted );

  /* --------------------------------------------------------------------- */
} /* end of gllcmm_intercept_test_mode_cmd() */


/*===========================================================================
===
===  FUNCTION      GLLCSMS_LL_SIG_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Dynamically allocate sufficient memory from heap to accommodate a
===    message of type gprs_ll_dl_sig_mes_t.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to allocated gprs_ll_dl_sig_mes_t message buffer if resource
===    is available, otherwise NULL.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

LOCAL gprs_ll_dl_sig_mes_t *gllcsms_ll_sig_get_mes_buf( void )
{
  return ( GPRS_MEM_MALLOC(sizeof(gprs_ll_dl_sig_mes_t) / sizeof(uint8)) );

} /* end of gllcsms_ll_sig_get_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLCSMS_LL_SIG_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Perform appropriate message adaptation and put the resultant
===    signalling message to the GSMS command queue.
===
===  DEPENDENCIES
===
===    Message ptr must have been allocated by gllcsms_ll_sig_get_mes_buf().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    May cause a context switch.
===
===========================================================================*/

LOCAL void gllcsms_ll_sig_put_mes_buf( gprs_ll_dl_sig_mes_t *mes_ptr )
{
  mm_cmd_type     *cmd_ptr;

  /* --------------------------------------------------------------------- */

  #ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#endif

  /* --------------------------------------------------------------------- */

  switch ( mes_ptr -> mes.prim )
  {
  /* ..................................................................... */

  case LL_STATUS_IND:

    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCSMS_LL_STATUS_IND );

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.llsms_status_ind.tlli = mes_ptr -> mes.status_ind_mes.tlli;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.llsms_status_ind.as_id =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.status_ind_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      if ( mes_ptr -> mes.status_ind_mes.cause == GPRS_LL_NO_PEER_RESPONSE )
      {
        cmd_ptr -> cmd.llsms_status_ind.cause = GLLCSMS_LL_NO_PEER_RESPONSE;
        cfa_log_packet_ex
        (
          GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCSMS_LL_STATUS_IND,
          sizeof( gllcsms_ll_status_ind_type ),
          (uint8 *)&cmd_ptr -> cmd.llsms_status_ind
        );
        mm_put_cmd( cmd_ptr );
      }
      else if ( mes_ptr -> mes.status_ind_mes.cause == GPRS_LL_INVALID_XID_RESP )
      {
        cmd_ptr -> cmd.llsms_status_ind.cause = GLLCSMS_LL_INVALID_XID_RESP;
        cfa_log_packet_ex
        (
          GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCSMS_LL_STATUS_IND,
          sizeof( gllcsms_ll_status_ind_type ),
          (uint8 *)&cmd_ptr -> cmd.llsms_status_ind
        );
        mm_put_cmd( cmd_ptr );
      }
      else
      {
        /* Any other status message should not originate from the LLE
           supporting GSMS. */

        GPRS_MEM_FREE( cmd_ptr );

        MSG_GERAN_HIGH_1("GLLSMS CLIENT: Filtered Status Cause %d",
          mes_ptr -> mes.status_ind_mes.cause);
      }
    }
    else
      ERR_GERAN_FATAL_0( "GLLSMS CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case LL_RESET_IND:

    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCSMS_LL_RESET_IND );

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.llsms_reset_ind.tlli = mes_ptr -> mes.rst_ind_mes.tlli;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.llsms_reset_ind.as_id  =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.rst_ind_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      cfa_log_packet_ex
      (
        GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCSMS_LL_RESET_IND,
        sizeof( gllcsms_ll_reset_ind_type ),
        (uint8 *)&cmd_ptr -> cmd.llsms_reset_ind
      );
      mm_put_cmd( cmd_ptr );
    }
    else
      ERR_GERAN_FATAL_0( "GLLSMS CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case LL_XID_IND:

    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCSMS_LL_XID_IND );

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.llsms_xid_ind.tlli   = mes_ptr -> mes.xid_ind_mes.tlli;
      cmd_ptr -> cmd.llsms_xid_ind.n201_u = mes_ptr -> mes.xid_ind_mes.n201_u;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.llsms_xid_ind.as_id  =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.xid_ind_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      if ( mes_ptr -> mes.xid_ind_mes.l3_xid_req != NULL )
      {
        cmd_ptr -> cmd.llsms_xid_ind.l3_xid_req_len =
          GPRS_PDU_LENGTH( mes_ptr -> mes.xid_ind_mes.l3_xid_req );

        cmd_ptr -> cmd.llsms_xid_ind.l3_xid_req =
          GLLCMM_MEM_MALLOC( cmd_ptr -> cmd.llsms_xid_ind.l3_xid_req_len );

        if ( cmd_ptr -> cmd.llsms_xid_ind.l3_xid_req != NULL )
        {
          (void)gprs_pdu_strip_head
          (
            &mes_ptr -> mes.xid_ind_mes.l3_xid_req,
            cmd_ptr  -> cmd.llsms_xid_ind.l3_xid_req,
            cmd_ptr  -> cmd.llsms_xid_ind.l3_xid_req_len
          );
        }
        else
          ERR_GERAN_FATAL_0( "GLLSMS CLIENT: Heap Exhaustion.");
      }
      else
      {
        cmd_ptr -> cmd.llsms_xid_ind.l3_xid_req_len = 0;
        cmd_ptr -> cmd.llsms_xid_ind.l3_xid_req     = NULL;
      }

      cfa_log_packet_ex
      (
        GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCSMS_LL_XID_IND,
        sizeof( gllcsms_ll_xid_ind_type ),
        (uint8 *)&cmd_ptr -> cmd.llsms_xid_ind
      );
      mm_put_cmd( cmd_ptr );
    }
    else
      ERR_GERAN_FATAL_0( "GLLSMS CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case LL_XID_CNF:

    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCSMS_LL_XID_CNF );

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.llsms_xid_cnf.tlli   = mes_ptr -> mes.xid_cnf_mes.tlli;
      cmd_ptr -> cmd.llsms_xid_cnf.n201_u = mes_ptr -> mes.xid_cnf_mes.n201_u;
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.llsms_xid_cnf.as_id  =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.xid_cnf_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      if ( mes_ptr -> mes.xid_cnf_mes.l3_xid_neg != NULL )
      {
        cmd_ptr -> cmd.llsms_xid_cnf.l3_xid_neg_len =
          GPRS_PDU_LENGTH( mes_ptr -> mes.xid_cnf_mes.l3_xid_neg );

        cmd_ptr -> cmd.llsms_xid_cnf.l3_xid_neg =
          GLLCMM_MEM_MALLOC( cmd_ptr -> cmd.llsms_xid_cnf.l3_xid_neg_len );

        if ( cmd_ptr -> cmd.llsms_xid_cnf.l3_xid_neg != NULL )
        {
          (void)gprs_pdu_strip_head
          (
            &mes_ptr -> mes.xid_cnf_mes.l3_xid_neg,
            cmd_ptr  -> cmd.llsms_xid_cnf.l3_xid_neg,
            cmd_ptr  -> cmd.llsms_xid_cnf.l3_xid_neg_len
          );
        }
        else
          ERR_GERAN_FATAL_0( "GLLSMS CLIENT: Heap Exhaustion.");
      }
      else
      {
        cmd_ptr -> cmd.llsms_xid_cnf.l3_xid_neg_len = 0;
        cmd_ptr -> cmd.llsms_xid_cnf.l3_xid_neg     = NULL;
      }

      cfa_log_packet_ex
      (
        GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCSMS_LL_XID_CNF,
        sizeof( gllcsms_ll_xid_cnf_type ),
        (uint8 *)&cmd_ptr -> cmd.llsms_xid_cnf
      );
      mm_put_cmd( cmd_ptr );
    }
    else
      ERR_GERAN_FATAL_0( "GLLSMS CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  default:
    MSG_GERAN_HIGH_1("GLLSMS CLIENT: Filtered Msg Id %d",
      mes_ptr -> mes.prim);

  /* ..................................................................... */
  }

  GPRS_MEM_FREE( mes_ptr );

  /* --------------------------------------------------------------------- */
} /* end of gllcsms_ll_sig_put_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLCSMS_LL_DAT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Perform appropriate message adaptation and put the resultant data
===    bearing message to the GSMS command queue.
===
===  DEPENDENCIES
===
===    Caller is expected to have preallocated gprs_ll_dl_dat_mes_t message
===    resource from heap.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    May cause a context switch.
===
===========================================================================*/

LOCAL void gllcsms_ll_dat_put_mes_buf( gprs_ll_dl_dat_mes_t *mes_ptr )
{
  mm_cmd_type     *cmd_ptr;

  /* --------------------------------------------------------------------- */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

  #ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#endif

  /* --------------------------------------------------------------------- */

  if ( mes_ptr -> mes.prim == LL_UNITDATA_IND )
  {
    cmd_ptr = mm_gllc_get_cmd_buf( (uint8)GLLCSMS_LL_UNITDATA_IND );

    if ( cmd_ptr != NULL )
    {
      cmd_ptr -> cmd.llsms_unitdata_ind.tlli =
        mes_ptr -> mes.unitdata_ind_mes.tlli;
      cmd_ptr -> cmd.llsms_unitdata_ind.ciphered =
        mes_ptr -> mes.unitdata_ind_mes.ciphered;
      cmd_ptr -> cmd.llsms_unitdata_ind.l3_pdu_len =
        GPRS_PDU_LENGTH( mes_ptr -> mes.unitdata_ind_mes.l3_pdu );
      cmd_ptr -> cmd.llsms_unitdata_ind.l3_pdu =
        GLLCMM_MEM_MALLOC( cmd_ptr -> cmd.llsms_unitdata_ind.l3_pdu_len );
#ifdef FEATURE_DUAL_SIM
      cmd_ptr -> cmd.llsms_unitdata_ind.as_id  =
        geran_map_gas_id_to_nas_id(mes_ptr -> mes.unitdata_ind_mes.gas_id);
#endif /* FEATURE_DUAL_SIM */

      if ( cmd_ptr -> cmd.llsms_unitdata_ind.l3_pdu != NULL )
      {
        (void)gprs_pdu_strip_head
        (
          &mes_ptr -> mes.unitdata_ind_mes.l3_pdu,
          cmd_ptr -> cmd.llsms_unitdata_ind.l3_pdu,
          cmd_ptr -> cmd.llsms_unitdata_ind.l3_pdu_len
        );

        /* ATM: Just log the l3_pdu portion for CFA */
        cfa_log_packet_ex
        (
          GS_QUEUE_MM, MS_LLC_GMM, (uint8)GLLCSMS_LL_UNITDATA_IND,
          cmd_ptr -> cmd.llsms_unitdata_ind.l3_pdu_len,
          cmd_ptr -> cmd.llsms_unitdata_ind.l3_pdu
        );

        mm_put_cmd( cmd_ptr );
      }
      else
        ERR_GERAN_FATAL_0( "GLLSMS CLIENT: Heap Exhaustion.");
    }
    else
      ERR_GERAN_FATAL_0( "GLLSMS CLIENT: Heap Exhaustion.");
  }
  else
    MSG_GERAN_HIGH_1("GLSMS CLIENT: Filtered Msg Id %d", mes_ptr -> mes.prim);

  GPRS_MEM_FREE( mes_ptr );

  /* --------------------------------------------------------------------- */
} /* end of gllcsms_ll_dat_put_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GMMLLC_LL_GET_GAS_ID()
===
===  DESCRIPTION
===
===    Extract AS_ID and convert it to GAS_ID.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    gas_id.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

gas_id_t gmmllc_ll_get_gas_id ( gmmllc_ll_mes_type *mes_ptr )
{

  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;

  /* --------------------------------------------------------------------- */

  /* Extract AS_ID */

  if ( mes_ptr != NULL )
  {
    switch ( mes_ptr -> mes.prim )
    {
    case GMMLLC_LL_UNITDATA_REQ:

      as_id = mes_ptr -> mes.ll_unitdata_req.as_id;
      break;

    case GMMLLC_LL_XID_REQ:

      as_id = mes_ptr -> mes.ll_xid_req.as_id;
      break;

    case GMMLLC_LL_XID_RES:

      as_id = mes_ptr -> mes.ll_xid_res.as_id;
      break;

    default:
      MSG_GERAN_ERROR_1( "GLC GMMLLC get AS_ID: Invld Prim %d.",
        mes_ptr -> mes.prim );
      break;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("GLC GMMLLC Put: No Msg");
  }

  /* --------------------------------------------------------------------- */

  /* Convert AS_ID to GAS_ID. */

  return ( geran_map_nas_id_to_gas_id( as_id ) );

  /* --------------------------------------------------------------------- */

}


/*===========================================================================
===
===  FUNCTION      GSMSLLC_LL_GET_GAS_ID()
===
===  DESCRIPTION
===
===    Extract AS_ID and convert it to GAS_ID.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    gas_id.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

gas_id_t gsmsllc_ll_get_gas_id ( gsmsllc_ll_mes_type *mes_ptr )
{

  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;

  /* --------------------------------------------------------------------- */

  /* Extract AS_ID */

  if ( mes_ptr != NULL )
  {
    switch ( mes_ptr -> mes.prim )
    {
    case GSMSLLC_LL_UNITDATA_REQ:

      as_id = mes_ptr -> mes.ll_unitdata_req.as_id;
      break;

    case GSMSLLC_LL_XID_REQ:

      as_id = mes_ptr -> mes.ll_xid_req.as_id;
      break;

    case GSMSLLC_LL_XID_RES:

      as_id = mes_ptr -> mes.ll_xid_res.as_id;
      break;

    default:
      MSG_GERAN_ERROR_1( "GLC GSMSLLC get AS_ID: Invld Prim %d.",
        mes_ptr -> mes.prim );
      break;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("GLC GSMSLLC Put: No Msg");
  }

  /* --------------------------------------------------------------------- */

  /* Convert AS_ID to GAS_ID. */

  return ( geran_map_nas_id_to_gas_id( as_id ) );

  /* --------------------------------------------------------------------- */

}


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GMMLLC_LL_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Dynamically allocate sufficient memory from heap to accommodate a
===    message of type gmmllc_ll_mes_type.
===
===    For a complete description refer to interface file gllclient.h
===
===========================================================================*/

gmmllc_ll_mes_type *gmmllc_ll_get_mes_buf( void )
{
  return ( GPRS_MEM_MALLOC(sizeof(gmmllc_ll_mes_type) / sizeof(uint8)) );

}  /* end of gmmllc_ll_get_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GMMLLC_LL_PUT_MES()
===
===  DESCRIPTION
===
===    Perform appropriate message adaptation and post the resultant message
===    to the appropriate LL SAP message queue of LLC.
===
===    For a complete description refer to interface file gllclient.h
===
===========================================================================*/

void gmmllc_ll_put_mes( gmmllc_ll_mes_type *mes_ptr )
{
  gprs_ll_ul_sig_mes_t *sig_mes_ptr;
  gprs_ll_ul_dat_mes_t *dat_mes_ptr;
  gas_id_t             gas_id;

  /* --------------------------------------------------------------------- */

  if ( mes_ptr == NULL )
  {
    MSG_GERAN_ERROR_0("GLLGMM CLIENT: LL Put Mes: No Msg");
    return;
  }

  /* --------------------------------------------------------------------- */

  /* Extract gas_id */
  gas_id = gmmllc_ll_get_gas_id ( mes_ptr );

  /* --------------------------------------------------------------------- */

  if( gas_id > GERAN_ACCESS_STRATUM_ID_MAX )
  {
    MSG_GERAN_ERROR_0_G("GLLGMM CLIENT: LL Put Mes: Invalid GAS_ID ");
  }

  /* --------------------------------------------------------------------- */

  else switch ( mes_ptr -> mes.prim )
  {
  /* ..................................................................... */

  case GMMLLC_LL_UNITDATA_REQ:

    /* ATM: Just log the l3_pdu portion for CFA */
    cfa_log_packet_ex
    (
      gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LL_UNITDATA_REQ,
      mes_ptr -> mes.ll_unitdata_req.l3_pdu_len,
      mes_ptr -> mes.ll_unitdata_req.l3_pdu
    );

    llc_log_ota_message
    (
      gas_id,
      FALSE, /* Uplink */
      mes_ptr -> mes.ll_unitdata_req.l3_pdu[1],
      mes_ptr -> mes.ll_unitdata_req.l3_pdu_len,
      mes_ptr -> mes.ll_unitdata_req.l3_pdu
    );

    dat_mes_ptr = GPRS_MEM_MALLOC(sizeof(gprs_ll_ul_dat_mes_t)/sizeof(uint8));

    if ( dat_mes_ptr != NULL )
    {
      dat_mes_ptr -> sapi     = 1;
      dat_mes_ptr -> mes.prim = LL_UNITDATA_REQ;

      dat_mes_ptr -> mes.unitdata_req_mes.protocol_disc =
        mes_ptr -> mes.ll_unitdata_req.protocol_disc;

      dat_mes_ptr -> mes.unitdata_req_mes.transaction_id =
        mes_ptr -> mes.ll_unitdata_req.transaction_id;

      dat_mes_ptr -> mes.unitdata_req_mes.tlli =
        mes_ptr -> mes.ll_unitdata_req.tlli;
      dat_mes_ptr -> mes.unitdata_req_mes.ciphered =
        mes_ptr -> mes.ll_unitdata_req.ciphered;

      dat_mes_ptr -> mes.unitdata_req_mes.peak_throughput   = 0;
      dat_mes_ptr -> mes.unitdata_req_mes.radio_priority    = 1;
      dat_mes_ptr -> mes.unitdata_req_mes.reliability_class = 3;
      dat_mes_ptr -> mes.unitdata_req_mes.pfi               = 1;

      dat_mes_ptr -> mes.unitdata_req_mes.gas_id            = gas_id;

      dat_mes_ptr -> mes.unitdata_req_mes.pdu_priority      = GPDU_HIGH_PRIORITY;

      dat_mes_ptr -> mes.unitdata_req_mes.l3_pdu =
        gllc_pdu_construct( mes_ptr -> mes.ll_unitdata_req.l3_pdu,
                            mes_ptr -> mes.ll_unitdata_req.l3_pdu_len );

      GPRS_MEM_FREE( mes_ptr -> mes.ll_unitdata_req.l3_pdu );

      gllc_ll_ul_dat_put_mes_buf( gas_id, dat_mes_ptr );
    }
    else
      ERR_GERAN_FATAL_0_G( "GLLGMM CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case GMMLLC_LL_XID_REQ:

    cfa_log_packet_ex
    (
      gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LL_XID_REQ,
      sizeof( gmmllc_ll_xid_req_type ),
      (uint8 *)&mes_ptr -> mes.ll_xid_req
    );

    sig_mes_ptr = gllc_ll_ul_sig_get_mes_buf();

    if ( sig_mes_ptr != NULL )
    {
      sig_mes_ptr -> sapi                 = 1;
      sig_mes_ptr -> mes.prim             = LL_XID_REQ;
      sig_mes_ptr -> mes.xid_req_mes.tlli = mes_ptr -> mes.ll_xid_req.tlli;

      sig_mes_ptr -> mes.xid_req_mes.peak_throughput = 0;
      sig_mes_ptr -> mes.xid_req_mes.radio_priority  = 1;
      sig_mes_ptr -> mes.xid_req_mes.pfi             = 1;

      sig_mes_ptr -> mes.xid_req_mes.gas_id          = gas_id;

      sig_mes_ptr -> mes.xid_req_mes.l3_xid_req =
        gllc_pdu_construct( mes_ptr -> mes.ll_xid_req.l3_xid_req,
                            mes_ptr -> mes.ll_xid_req.l3_xid_req_len );

      GPRS_MEM_FREE(  mes_ptr -> mes.ll_xid_req.l3_xid_req );

      gllc_ll_ul_sig_put_mes_buf( gas_id, sig_mes_ptr );
    }
    else
      ERR_GERAN_FATAL_0_G( "GLLGMM CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case GMMLLC_LL_XID_RES:

    cfa_log_packet_ex
    (
      gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GMMLLC_LL_XID_RES,
      sizeof( gmmllc_ll_xid_res_type ),
      (uint8 *)&mes_ptr -> mes.ll_xid_res
    );

    sig_mes_ptr = gllc_ll_ul_sig_get_mes_buf();

    if ( sig_mes_ptr != NULL )
    {
      sig_mes_ptr -> sapi                 = 1;
      sig_mes_ptr -> mes.prim             = LL_XID_RES;
      sig_mes_ptr -> mes.xid_res_mes.tlli = mes_ptr -> mes.ll_xid_res.tlli;

      sig_mes_ptr -> mes.xid_res_mes.peak_throughput = 0;
      sig_mes_ptr -> mes.xid_res_mes.radio_priority  = 1;
      sig_mes_ptr -> mes.xid_res_mes.pfi             = 1;

      sig_mes_ptr -> mes.xid_res_mes.gas_id          = gas_id;

      sig_mes_ptr -> mes.xid_res_mes.l3_xid_neg =
        gllc_pdu_construct( mes_ptr -> mes.ll_xid_res.l3_xid_neg,
                            mes_ptr -> mes.ll_xid_res.l3_xid_neg_len );

      GPRS_MEM_FREE(  mes_ptr -> mes.ll_xid_res.l3_xid_neg );

      gllc_ll_ul_sig_put_mes_buf( gas_id, sig_mes_ptr );
    }
    else
      ERR_GERAN_FATAL_0_G( "GLLGMM CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  default:

    MSG_GERAN_ERROR_1("GLLGMM CLIENT: Invalid Msg Id %d", mes_ptr -> mes.prim);

  /* ..................................................................... */
  }

  GPRS_MEM_FREE( mes_ptr );

  /* --------------------------------------------------------------------- */
}  /* end of gmmllc_ll_put_mes() */


/*===========================================================================
===
===  FUNCTION      GSMSLLC_LL_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Dynamically allocate sufficient memory from heap to accommodate a
===    message of type gsmsllc_ll_mes_type.
===
===    For a complete description refer to interface file gllclient.h
===
===========================================================================*/

gsmsllc_ll_mes_type *gsmsllc_ll_get_mes_buf( void )
{
  return ( GPRS_MEM_MALLOC(sizeof(gsmsllc_ll_mes_type) / sizeof(uint8)) );

} /* end of gsmsllc_ll_get_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GSMSLLC_LL_PUT_MES()
===
===  DESCRIPTION
===
===    Perform appropriate message adaptation and post the resultant message
===    to the appropriate LL SAP message queue of LLC.
===
===    For a complete description refer to interface file gllclient.h
===
===========================================================================*/

void gsmsllc_ll_put_mes( gsmsllc_ll_mes_type *mes_ptr )
{
  gprs_ll_ul_sig_mes_t *sig_mes_ptr;
  gprs_ll_ul_dat_mes_t *dat_mes_ptr;
  gas_id_t              gas_id;

  /* --------------------------------------------------------------------- */

  if ( mes_ptr == NULL )
  {
    MSG_GERAN_ERROR_0("GLLSMS CLIENT: LL Put Mes: No Msg");
    return;
  }

  /* --------------------------------------------------------------------- */

  /* Extract gas_id */
  gas_id = gsmsllc_ll_get_gas_id ( mes_ptr );

  /* --------------------------------------------------------------------- */

  if( gas_id > GERAN_ACCESS_STRATUM_ID_MAX )
  {
    MSG_GERAN_ERROR_1_G("GLLSMS CLIENT: LL Put Mes: Invalid GAS_ID %d", gas_id );
  }

  /* --------------------------------------------------------------------- */

  else switch ( mes_ptr -> mes.prim )
  {
  /* ..................................................................... */

  case GSMSLLC_LL_UNITDATA_REQ:

    cfa_log_packet_ex
    (
      gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GSMSLLC_LL_UNITDATA_REQ,
      sizeof( gsmsllc_ll_unitdata_req_type ),
      (uint8 *)&mes_ptr -> mes.ll_unitdata_req
    );

    dat_mes_ptr = GPRS_MEM_MALLOC(sizeof(gprs_ll_ul_dat_mes_t)/sizeof(uint8));

    if ( dat_mes_ptr != NULL )
    {
      dat_mes_ptr -> sapi     = 7;
      dat_mes_ptr -> mes.prim = LL_UNITDATA_REQ;

      dat_mes_ptr -> mes.unitdata_req_mes.tlli =
        mes_ptr -> mes.ll_unitdata_req.tlli;
      dat_mes_ptr -> mes.unitdata_req_mes.ciphered =
        mes_ptr -> mes.ll_unitdata_req.ciphered;
      dat_mes_ptr -> mes.unitdata_req_mes.radio_priority =
        mes_ptr -> mes.ll_unitdata_req.radio_priority;

      dat_mes_ptr -> mes.unitdata_req_mes.peak_throughput   = 0;
      dat_mes_ptr -> mes.unitdata_req_mes.reliability_class = 3;
      dat_mes_ptr -> mes.unitdata_req_mes.pfi               = 2;

      dat_mes_ptr -> mes.unitdata_req_mes.gas_id            = gas_id;

      dat_mes_ptr -> mes.unitdata_req_mes.pdu_priority      = GPDU_HIGH_PRIORITY;

      dat_mes_ptr -> mes.unitdata_req_mes.l3_pdu =
        gllc_pdu_construct( mes_ptr -> mes.ll_unitdata_req.l3_pdu,
                            mes_ptr -> mes.ll_unitdata_req.l3_pdu_len );

      GPRS_MEM_FREE( mes_ptr -> mes.ll_unitdata_req.l3_pdu );

      gllc_ll_ul_dat_put_mes_buf( gas_id, dat_mes_ptr );
    }
    else
      ERR_GERAN_FATAL_0_G( "GLLSMS CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case GSMSLLC_LL_XID_REQ:

    cfa_log_packet_ex
    (
      gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GSMSLLC_LL_XID_REQ,
      sizeof( gsmsllc_ll_xid_req_type ),
      (uint8 *)&mes_ptr -> mes.ll_xid_req
    );

    sig_mes_ptr = gllc_ll_ul_sig_get_mes_buf();

    if ( sig_mes_ptr != NULL )
    {
      sig_mes_ptr -> sapi                 = 7;
      sig_mes_ptr -> mes.prim             = LL_XID_REQ;
      sig_mes_ptr -> mes.xid_req_mes.tlli = mes_ptr -> mes.ll_xid_req.tlli;

      sig_mes_ptr -> mes.xid_req_mes.peak_throughput = 0;
      sig_mes_ptr -> mes.xid_req_mes.radio_priority  = 1;
      sig_mes_ptr -> mes.xid_req_mes.pfi             = 2;

      sig_mes_ptr -> mes.xid_req_mes.gas_id          = gas_id;

      sig_mes_ptr -> mes.xid_req_mes.l3_xid_req =
        gllc_pdu_construct( mes_ptr -> mes.ll_xid_req.l3_xid_req,
                            mes_ptr -> mes.ll_xid_req.l3_xid_req_len );

      GPRS_MEM_FREE(  mes_ptr -> mes.ll_xid_req.l3_xid_req );

      gllc_ll_ul_sig_put_mes_buf( gas_id, sig_mes_ptr );
    }
    else
      ERR_GERAN_FATAL_0_G( "GLLSMS CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  case GSMSLLC_LL_XID_RES:

    cfa_log_packet_ex
    (
      gllc_gs_queue[ gas_id ], MS_LLC_GMM, (uint8)GSMSLLC_LL_XID_RES,
      sizeof( gsmsllc_ll_xid_res_type ),
      (uint8 *)&mes_ptr -> mes.ll_xid_res
    );

    sig_mes_ptr = gllc_ll_ul_sig_get_mes_buf();

    if ( sig_mes_ptr != NULL )
    {
      sig_mes_ptr -> sapi                 = 7;
      sig_mes_ptr -> mes.prim             = LL_XID_RES;
      sig_mes_ptr -> mes.xid_res_mes.tlli = mes_ptr -> mes.ll_xid_res.tlli;

      sig_mes_ptr -> mes.xid_res_mes.peak_throughput = 0;
      sig_mes_ptr -> mes.xid_res_mes.radio_priority  = 1;
      sig_mes_ptr -> mes.xid_res_mes.pfi             = 2;

      sig_mes_ptr -> mes.xid_res_mes.gas_id          = gas_id;

      sig_mes_ptr -> mes.xid_res_mes.l3_xid_neg =
        gllc_pdu_construct( mes_ptr -> mes.ll_xid_res.l3_xid_neg,
                            mes_ptr -> mes.ll_xid_res.l3_xid_neg_len );

      GPRS_MEM_FREE(  mes_ptr -> mes.ll_xid_res.l3_xid_neg );

      gllc_ll_ul_sig_put_mes_buf( gas_id, sig_mes_ptr );
    }
    else
      ERR_GERAN_FATAL_0_G( "GLLSMS CLIENT: Heap Exhaustion.");
    break;

  /* ..................................................................... */

  default:

    MSG_GERAN_ERROR_1("GLLSMS CLIENT: Invalid Msg Id %d", mes_ptr -> mes.prim);

  /* ..................................................................... */
  }

  GPRS_MEM_FREE( mes_ptr );

  /* --------------------------------------------------------------------- */
} /* end of gsmsllc_ll_put_mes() */


/*===========================================================================
===
===  FUNCTION      GLLCLIENT_REG_GMM_GSMS_PROXY()
===
===  DESCRIPTION
===
===    For use by the GPRS LLC Task at initialisation to register the GMM,
===    GSMS, GSNDCP proxy clients at the appropriate LL SAP service access
===    point addresses.
===
===    For a complete description refer to interface file gllclient.h
===
===========================================================================*/

void gllclient_reg_gmm_gsms_proxy( gas_id_t gas_id )
{
  /* Register the GMM proxy client with GPRS LLC LL SAPI 1 */

  gllc_register_ll_client_gmm
  (
    gas_id,
    gllcmm_ll_sig_get_mes_buf,
    gllcmm_ll_sig_put_mes_buf,
    gllcmm_ll_dat_put_mes_buf
  );

  /* Register the GSMS proxy client with GPRS LLC LL SAPI 7 */

  gllc_register_ll_client_gsms
  (
    gas_id,
    gllcsms_ll_sig_get_mes_buf,
    gllcsms_ll_sig_put_mes_buf,
    gllcsms_ll_dat_put_mes_buf
  );

  /* Register the GSNDCP proxy client with GPRS LLC LL SAPI 3,5,9, & 11 */

  gllc_register_ll_client_sndcp
  (
    gas_id,
    gsn_ll_dl_sig_get_mes_buf,
    gsn_ll_dl_sig_put_mes_buf,
    gsn_ll_dl_dat_put_mes_buf
  );

} /* end of gllclient_reg_gmm_gsms_proxy() */


/*===========================================================================
===
===  FUNCTION      GLLCLIENT_GEA_CLASSMARK_ENQUIRY()
===
===  DESCRIPTION
===
===    For use by a classmark enquiring client to determine whether the
===    indicated GPRS Encryption Algorithm (GEA) is supported.
===
===    For a complete description refer to interface file gllclient.h
===
===========================================================================*/

boolean gllclient_gea_classmark_enquiry
(
  gmmllc_gea_e_type gea
)
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  ERR_GERAN_FATAL_0_G( "GLC CLIENT: GMM calling wrong API.");

  return ( gllc_is_gea_supported( gas_id, gea ) );

} /* end of gllclient_gea_classmark_enquiry() */



/*===========================================================================
===
===  FUNCTION      GLLCLIENT_GEA_CLASSMARK_ENQUIRY_PER_SUB()
===
===  DESCRIPTION
===
===    For use by a classmark enquiring client to determine whether the
===    indicated GPRS Encryption Algorithm (GEA) is supported in a given
===    as_id.
===
===    For a complete description refer to interface file gllclient.h
===
===========================================================================*/

boolean gllclient_gea_classmark_enquiry_per_subs
(
  sys_modem_as_id_e_type as_id,
  gmmllc_gea_e_type      gea
)
{
  gas_id_t gas_id;

  gas_id = geran_map_nas_id_to_gas_id( as_id );

  return ( gllc_is_gea_supported( gas_id, gea ) );

} /* end of gllclient_gea_classmark_enquiry_per_sub() */



/*** EOF: don't remove! ***/

