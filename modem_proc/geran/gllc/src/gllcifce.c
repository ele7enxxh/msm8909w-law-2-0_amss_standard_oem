/*****************************************************************************
***
*** TITLE  gllciface.c
***
***  GPRS LLC Layer Interface Functions
***
***
*** DESCRIPTION
***
***  Provides the access function implementations which support the following
***  interfaces:
***
***  (1) GPRS LLC Layer Service Provision; Interface at LLGMM and LL SAPs
***  (2) GPRS LLC GRR SAP Interface
***  (3) GPRS LLC Module Task Interface
***
***
*** EXTERNALIZED FUNCTIONS
***
***  Refer to gllc.h for detail of these externalised functions.
***
***    gmmllc_llgmm_get_mes_buf()
***    gmmllc_llgmm_put_mes_buf()
***
***    gllc_register_ll_client_sndcp()
***    gllc_register_ll_client_gmm()
***    gllc_register_ll_client_gsms()
***    gllc_register_ll_client_tom()
***    gllc_ll_client_registered()
***
***    gllc_ll_ul_sig_get_mes_buf()
***    gllc_ll_ul_sig_put_mes_buf()
***    gllc_ll_ul_dat_put_mes_buf()
***
***    gllc_ll_get_default_N201_I()
***    gllc_ll_get_default_N201_U()
***
***    gllc_grr_ul_suspend_ind()
***    gllc_grr_ul_resume_ind()
***    gllc_grr_dl_put_mes_buf()
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/src/gllcifce.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
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
#include "geran_tasks.h"
#include "gmutex.h"
#include "gprs_mem.h"
#include "gprs_pdu.h"
#include "geran_msgs.h"
#include "queue.h"
#include "rex.h"
#include "string.h"
#include "stringl.h"
#include "gllc.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gllci.h"

#include "gsndcp_v_g.h"

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




/* Diagnostic Client Registration Records. */

LOCAL gllc_diag_rpt_llme_state_fnc_t gllc_diag_rpt_llme_state_fnc_p[NUM_GERAN_DATA_SPACES] = { NULL };
LOCAL gllc_diag_rpt_lle_state_fnc_t  gllc_diag_rpt_lle_state_fnc_p[NUM_GERAN_DATA_SPACES]  = { NULL };
LOCAL gllc_diag_rpt_lle_xid_fnc_t    gllc_diag_rpt_lle_xid_fnc_p[NUM_GERAN_DATA_SPACES]    = { NULL };

gllc_diag_llme_state_rpt_t  llme_rpt[NUM_GERAN_DATA_SPACES];
gllc_diag_lle_state_rpt_t   lle_rpt[NUM_GERAN_DATA_SPACES];
gllc_diag_lle_xid_rpt_t     xid_rpt[NUM_GERAN_DATA_SPACES];
gllc_diag_pdu_stat_rsp_t    pdu_stat_rpt[NUM_GERAN_DATA_SPACES];
gllc_diag_pdu_stat_rsp_t    *pdu_stat_rpt_ptr[NUM_GERAN_DATA_SPACES] = {NULL};
gllc_diag_state_stat_rsp_t  pdu_stat_rsp[NUM_GERAN_DATA_SPACES];
gllc_diag_state_stat_rsp_t  *pdu_stat_rsp_ptr[NUM_GERAN_DATA_SPACES] = {NULL};

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/* Client Registration Records. */

gllc_ll_client_record_t     gllc_ll_client_record[ NUM_GERAN_DATA_SPACES ][ GLLC_LL_NUM_SAPIS ];


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


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/****************************************************************************
   GPRS LLC LLGMM SAP Interface Functions
****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GMMLLC_LLGMM_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Permits the GMM entity to request the dynamic allocation of sufficient
===    memory to accomodate a message for conveyance to the LLC entity over
===    the LLGMM interface connection point.
===
===    For a complete description refer to interface gllc.h
===
===========================================================================*/

gmmllc_llgmm_mes_type *gmmllc_llgmm_get_mes_buf( void )
{
  return ( GPRS_MEM_MALLOC(sizeof(gmmllc_llgmm_mes_type) / sizeof(uint8)) );

} /* end of gmmllc_llgmm_get_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GMMLLC_LLGMM_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Permits the GMM entity to convey an LLGMM message to the LLC entity's
===    LLGMM message queue over the LLGMM interface connection point.
===
===    For a complete description refer to interface gllc.h
===
===========================================================================*/

void gmmllc_llgmm_put_mes_buf( gmmllc_llgmm_mes_type *mes_ptr )
{

  gas_id_t               gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
  sys_modem_as_id_e_type as_id  = SYS_MODEM_AS_ID_MAX;

  /* --------------------------------------------------------------------- */

  /* Extract AS_ID */

  if ( mes_ptr != NULL )
  {
    switch ( mes_ptr -> mes.prim )
    {
    case GMMLLC_LLGMM_ASSIGN_REQ:

      as_id = mes_ptr -> mes.llgmm_assign_req.as_id;
      break;

    case GMMLLC_LLGMM_TRIGGER_REQ:

      as_id = mes_ptr -> mes.llgmm_trigger_req.as_id;
      break;

    case GMMLLC_LLGMM_SUSPEND_REQ:

      as_id = mes_ptr -> mes.llgmm_suspend_req.as_id;
      break;

    case GMMLLC_LLGMM_RESUME_REQ:

      as_id = mes_ptr -> mes.llgmm_resume_req.as_id;
      break;

    case GMMLLC_LLGMM_STRT_RDY_TMR_REQ:

      as_id = mes_ptr -> mes.llgmm_strt_rdy_tmr_req.as_id;
      break;

    case GMMLLC_LLGMM_STOP_RDY_TMR_REQ:

      as_id = mes_ptr -> mes.llgmm_stop_rdy_tmr_req.as_id;
      break;

    case GMMLLC_LLGMM_TEST_MODE_REQ:

      as_id = mes_ptr -> mes.llgmm_test_mode_req.as_id;
      break;

#ifdef FEATURE_GPRS_PS_HANDOVER

    case GMMLLC_LLGMM_PSHO_REQ:

      as_id = mes_ptr -> mes.llgmm_psho_req.as_id;
      break;

#endif /* FEATURE_GPRS_PS_HANDOVER */

    default:
      MSG_GERAN_ERROR_1_G( "GLC LLGMM UL Hdlr: Invld Prim %d.",
        mes_ptr -> mes.prim );
      break;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("GLC ME: GMMLLC Put: No Msg");
  }

  /* --------------------------------------------------------------------- */

  /* Convert AS_ID to GAS_ID. */

  gas_id = geran_map_nas_id_to_gas_id( as_id );

  /* --------------------------------------------------------------------- */

  /* Validate the LLC initialization status. */

  if ( !gllc_initialised[ gas_id ] )
  {
    ERR_GERAN_FATAL_0_G( "GLC ME: GMMLLC Put: Uninitialised");
  }

  /* --------------------------------------------------------------------- */

  else
  {
    #ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
      #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    /* Initialise link, put to LLGMM message queue and set event. */

    GLLC_UL_MSG_LOCK(gas_id); 

    (void)q_link( mes_ptr, &mes_ptr -> link );
    q_put( &gllc_gmm_ul_q[ gas_id ], &mes_ptr -> link );

    GLLC_SET_UL_EVT( gas_id, GLLC_UL_GMM_Q_EVT );

    GLLC_UL_MSG_UNLOCK(gas_id); 
  }

  /* --------------------------------------------------------------------- */
} /* end of gmmllc_llgmm_put_mes_buf() */


/****************************************************************************
   GPRS LLC LL SAP Interface Functions
****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_REGISTER_LL_CLIENT_SNDCP()
===
===  DESCRIPTION
===
===   Provides a mechanism for an SNDCP client to register for this service
===   provision.
===
===   For a complete description refer to interface gllc.h
===
===========================================================================*/

void gllc_register_ll_client_sndcp
(
  gas_id_t                      gas_id,
  gllc_ll_dl_sig_get_mes_buf_t  ll_dl_sig_get_mes_buf_fnc_ptr,
  gllc_ll_dl_sig_put_mes_buf_t  ll_dl_sig_put_mes_buf_fnc_ptr,
  gllc_ll_dl_dat_put_mes_buf_t  ll_dl_dat_put_mes_buf_fnc_ptr
)
{

  /* --------------------------------------------------------------------- */

  /* Validate the SNDCP LL Client Register request. */

  if ( !gllc_initialised[ gas_id ] )
  {
    ERR_GERAN_FATAL_0_G( "GLC SNDCP LL Client Reg: Uninitialised");
  }
  else if
  (
    ( ll_dl_sig_get_mes_buf_fnc_ptr == NULL ) ||
    ( ll_dl_sig_put_mes_buf_fnc_ptr == NULL ) ||
    ( ll_dl_dat_put_mes_buf_fnc_ptr == NULL )
  )
  {
    MSG_GERAN_ERROR_0_G( "GLC SNDCP LL Client Reg: Invalid Params");
  }

  /* --------------------------------------------------------------------- */

  /* SNDCP LL Client Register request has been validated. */

  else
  {
    MSG_GERAN_HIGH_0_G( "GLC SNDCP LL Client Registration");

    /* Record SNDCP's Signalling Get Buf Function. */

    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_3_LL3].ll_dl_sig_get_mes_buf_fnc_ptr =
      ll_dl_sig_get_mes_buf_fnc_ptr;
    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_5_LL5].ll_dl_sig_get_mes_buf_fnc_ptr =
      ll_dl_sig_get_mes_buf_fnc_ptr;
    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_9_LL9].ll_dl_sig_get_mes_buf_fnc_ptr =
      ll_dl_sig_get_mes_buf_fnc_ptr;
    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_11_LL11].ll_dl_sig_get_mes_buf_fnc_ptr =
      ll_dl_sig_get_mes_buf_fnc_ptr;

    /* Record SNDCP's Signalling Put Buf Function. */

    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_3_LL3].ll_dl_sig_put_mes_buf_fnc_ptr =
      ll_dl_sig_put_mes_buf_fnc_ptr;
    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_5_LL5].ll_dl_sig_put_mes_buf_fnc_ptr =
      ll_dl_sig_put_mes_buf_fnc_ptr;
    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_9_LL9].ll_dl_sig_put_mes_buf_fnc_ptr =
      ll_dl_sig_put_mes_buf_fnc_ptr;
    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_11_LL11].ll_dl_sig_put_mes_buf_fnc_ptr =
      ll_dl_sig_put_mes_buf_fnc_ptr;

    /* Record SNDCP's Data Put Buf Function. */

    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_3_LL3].ll_dl_dat_put_mes_buf_fnc_ptr =
      ll_dl_dat_put_mes_buf_fnc_ptr;
    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_5_LL5].ll_dl_dat_put_mes_buf_fnc_ptr =
      ll_dl_dat_put_mes_buf_fnc_ptr;
    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_9_LL9].ll_dl_dat_put_mes_buf_fnc_ptr =
      ll_dl_dat_put_mes_buf_fnc_ptr;
    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_11_LL11].ll_dl_dat_put_mes_buf_fnc_ptr =
      ll_dl_dat_put_mes_buf_fnc_ptr;


    /* --------------------------------------------------------------------- */

  }


  /* --------------------------------------------------------------------- */
} /* end of gllc_register_ll_client_sndcp() */


/*===========================================================================
===
===  FUNCTION      GLLC_REGISTER_LL_CLIENT_GMM()
===
===  DESCRIPTION
===
===   Provides a mechanism for a GMM client to register for this service
===   provision.
===
===   For a complete description refer to interface gllc.h
===
===========================================================================*/

void gllc_register_ll_client_gmm
(
  gas_id_t                      gas_id,
  gllc_ll_dl_sig_get_mes_buf_t  ll_dl_sig_get_mes_buf_fnc_ptr,
  gllc_ll_dl_sig_put_mes_buf_t  ll_dl_sig_put_mes_buf_fnc_ptr,

  gllc_ll_dl_dat_put_mes_buf_t  ll_dl_dat_put_mes_buf_fnc_ptr
)
{
  /* --------------------------------------------------------------------- */

  /* Validate the GMM LL Client Register request. */

  if ( !gllc_initialised[ gas_id ] )
  {
    ERR_GERAN_FATAL_0_G( "GLC GMM LL Client Reg: Uninitialised");
  }
  else if
  (
    ( ll_dl_sig_get_mes_buf_fnc_ptr == NULL ) ||
    ( ll_dl_sig_put_mes_buf_fnc_ptr == NULL ) ||
    ( ll_dl_dat_put_mes_buf_fnc_ptr == NULL )
  )
  {
    MSG_GERAN_ERROR_0_G( "GLC GMM LL Client Reg: Invalid Params");
  }

  /* --------------------------------------------------------------------- */

  /* GMM LL Client Register request has been validated. */

  else
  {
    MSG_GERAN_HIGH_0_G( "GLC GMM LL Client Registration");

    /* Record GMM's Signalling Get Buf Function. */

    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_1_GMM].ll_dl_sig_get_mes_buf_fnc_ptr =
      ll_dl_sig_get_mes_buf_fnc_ptr;

    /* Record GMM's Signalling Put Buf Function. */

    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_1_GMM].ll_dl_sig_put_mes_buf_fnc_ptr =
      ll_dl_sig_put_mes_buf_fnc_ptr;

    /* Record GMM's Data Put Buf Function. */

    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_1_GMM].ll_dl_dat_put_mes_buf_fnc_ptr =
      ll_dl_dat_put_mes_buf_fnc_ptr;

  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_register_ll_client_gmm() */


/*===========================================================================
===
===  FUNCTION      GLLC_REGISTER_LL_CLIENT_GSMS()
===
===  DESCRIPTION
===
===   Provides a mechanism for a GSMS client to register for this service
===   provision.
===
===   For a complete description refer to interface gllc.h
===
===========================================================================*/

void gllc_register_ll_client_gsms
(
  gas_id_t                      gas_id,
  gllc_ll_dl_sig_get_mes_buf_t  ll_dl_sig_get_mes_buf_fnc_ptr,
  gllc_ll_dl_sig_put_mes_buf_t  ll_dl_sig_put_mes_buf_fnc_ptr,

  gllc_ll_dl_dat_put_mes_buf_t  ll_dl_dat_put_mes_buf_fnc_ptr
)
{
  /* --------------------------------------------------------------------- */

  /* Validate the GSMS LL Client Register request. */

  if ( !gllc_initialised[ gas_id ] )
  {
    ERR_GERAN_FATAL_0_G( "GLC GSMS LL Client Reg: Uninitialised");
  }
  else if
  (
    ( ll_dl_sig_get_mes_buf_fnc_ptr == NULL ) ||
    ( ll_dl_sig_put_mes_buf_fnc_ptr == NULL ) ||
    ( ll_dl_dat_put_mes_buf_fnc_ptr == NULL )
  )
  {
    MSG_GERAN_ERROR_0_G( "GLC GSMS LL Client Reg: Invalid Params");
  }

  /* --------------------------------------------------------------------- */

  /* GSMS LL Client Register request has been validated. */

  else
  {
    MSG_GERAN_HIGH_0_G( "GLC GSMS LL Client Registration");

    /* Record GSMS's Signalling Get Buf Function. */

    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_7_SMS].ll_dl_sig_get_mes_buf_fnc_ptr =
      ll_dl_sig_get_mes_buf_fnc_ptr;

    /* Record GSMS's Signalling Put Buf Function. */

    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_7_SMS].ll_dl_sig_put_mes_buf_fnc_ptr =
      ll_dl_sig_put_mes_buf_fnc_ptr;

    /* Record GSMS's Data Put Buf Function. */

    gllc_ll_client_record[ gas_id ][GLLC_LL_SAPI_7_SMS].ll_dl_dat_put_mes_buf_fnc_ptr =
      ll_dl_dat_put_mes_buf_fnc_ptr;

  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_register_ll_client_gsms() */


/*===========================================================================
===
===  FUNCTION      GLLC_REGISTER_LL_CLIENT_TOM()
===
===  DESCRIPTION
===
===   Provides a mechanism for a TOM client to register for this service
===   provision.
===
===   For a complete description refer to interface gllc.h
===
===========================================================================*/



/*===========================================================================
===
===  FUNCTION      GLLC_LL_CLIENT_REGISTERED()
===
===  DESCRIPTION
===
===    Determines whether the given LL SAP client has registered for service.
===
===    For a complete description refer to interface gllc.h.
===
===========================================================================*/

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_UL_SIG_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Permits an LL client entity to request the dynamic allocation of
===    sufficient memory to accomodate a message for conveyance to this LL
===    service provision via the LL interface connection point.
===
===    For a complete description refer to interface gllc.h.
===
===========================================================================*/

gprs_ll_ul_sig_mes_t *gllc_ll_ul_sig_get_mes_buf( void )
{
  return ( GPRS_MEM_MALLOC(sizeof(gprs_ll_ul_sig_mes_t) / sizeof(uint8)) );

} /* end of gllc_ll_ul_sig_get_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_UL_SIG_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Permits an LL client entity to convey a signalling message to this LL
===    service provision via the LL interface connection point.
===
===    For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_ll_ul_sig_put_mes_buf( gas_id_t gas_id, gprs_ll_ul_sig_mes_t *mes_ptr )
{
  gllc_ll_sapi_t  lle;

  /* --------------------------------------------------------------------- */

  /* Validate the LL up link signalling message request. */

  if ( mes_ptr == NULL )
  {
    MSG_GERAN_ERROR_0_G( "GLC LL: UL Sig Put: No Msg");
  }
  else if ( !gllc_initialised[ gas_id ] )
  {
    GPRS_MEM_FREE( mes_ptr );
    ERR_GERAN_FATAL_0_G( "GLC LL: UL Sig Put: Uninitialised");
  }
  else if ( mes_ptr -> sapi >= 16 )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL: UL Sig Put: Invalid SAPI %d",
      mes_ptr -> sapi);
    GPRS_MEM_FREE( mes_ptr );
  }
  else if ( gllc_lle_lookup[mes_ptr -> sapi] == GLLC_LL_SAPI_RESERVED )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL: UL Sig Put: Reserved SAPI %d",
      mes_ptr -> sapi);
    GPRS_MEM_FREE( mes_ptr );
  }

  /* --------------------------------------------------------------------- */

  else
  {
    #ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
      #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    /* .................................................................... */

    lle = gllc_lle_lookup[mes_ptr -> sapi];

    (void)q_link( mes_ptr, &mes_ptr -> link );
    q_put( &gllc_ll_ul_sig_q[ gas_id ][ lle ], &mes_ptr -> link );

    GLLC_SET_LLE_UL_EVT( gas_id, lle, GLLC_LLE_UL_LL_SIG_Q_EVT );
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_ll_ul_sig_put_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_UL_DAT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Permits an LL client entity to convey a data message to this LL
===    service provision via the LL interface connection point.
===
===    For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_ll_ul_dat_put_mes_buf( gas_id_t gas_id, gprs_ll_ul_dat_mes_t *mes_ptr )
{
  gllc_ll_sapi_t lle;
  dsm_item_type  *l3_pdu_ptr;

  /* --------------------------------------------------------------------- */

  /* Validate the LL up link data message request. */

  if ( mes_ptr == NULL )
  {
    MSG_GERAN_ERROR_0_G( "GLC LL: UL Dat Put: No Msg");
  }
  else if ( !gllc_initialised[ gas_id ] )
  {
    GPRS_MEM_FREE( mes_ptr );
    ERR_GERAN_FATAL_0_G( "GLC LL: UL Dat Put: Uninitialised");
  }
  else if ( mes_ptr -> sapi >= 16 )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL: UL Dat Put: Invalid SAPI %d",
      mes_ptr -> sapi);
    GPRS_MEM_FREE( mes_ptr );
  }
  else if ( gllc_lle_lookup[mes_ptr -> sapi] == GLLC_LL_SAPI_RESERVED )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL: UL Dat Put: Reserved SAPI %d",
      mes_ptr -> sapi);
    GPRS_MEM_FREE( mes_ptr );
  }

  /* --------------------------------------------------------------------- */

  else
  {
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    /* .................................................................... */

    /* Configure 'app_field' of lead DSM item in message l3_pdu to 'bear'
       associated message, and enqueue the PDU to the appropriate DSM
       watermark. */

    lle = gllc_lle_lookup[mes_ptr -> sapi];

    switch ( mes_ptr -> mes.prim )
    {
    case LL_DATA_REQ:

      l3_pdu_ptr = mes_ptr -> mes.data_req_mes.l3_pdu;
      if ( l3_pdu_ptr != NULL )
      {
        if ( ( gllc_ll_ul_ack_wm[ gas_id ][ lle ].current_cnt + GPRS_PDU_LENGTH(l3_pdu_ptr) <=
               gllc_ll_ul_ack_wm[ gas_id ][ lle ].dont_exceed_cnt                              ) &&
             ( gllc_llme[gas_id].xid_reset_in_progress == GLLC_LLME_XID_RESET_INACTIVE                 )
		       ) 
        {
          mes_ptr -> mes.data_req_mes.l3_pdu -> app_field = (uint32)mes_ptr;
          dsm_enqueue( &gllc_ll_ul_ack_wm[ gas_id ][ lle ], &l3_pdu_ptr );
        }
        else
        {
          MSG_GERAN_ERROR_1_G( "GLC LL %d: UL Dat Put: LL-DATA-REQ: Discard",
            gllc_sapi_addr_lookup[ lle ]);
          GPRS_PDU_FREE( &l3_pdu_ptr );
          GPRS_MEM_FREE( mes_ptr );
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G( "GLC LL %d: UL Dat Put: LL-DATA-REQ: No PDU",
          gllc_sapi_addr_lookup[ lle ]);
        GPRS_MEM_FREE( mes_ptr );
      }
      break;

    case LL_UNITDATA_REQ:

      l3_pdu_ptr = mes_ptr -> mes.unitdata_req_mes.l3_pdu;
      if ( l3_pdu_ptr != NULL )
      {
        if ( ( gllc_ll_ul_unack_wm[ gas_id ][ lle ].current_cnt + GPRS_PDU_LENGTH(l3_pdu_ptr) <=
               gllc_ll_ul_unack_wm[ gas_id ][ lle ].dont_exceed_cnt                              ) &&
             ( gllc_llme[gas_id].xid_reset_in_progress == GLLC_LLME_XID_RESET_INACTIVE                   )
		       ) 
        {
          mes_ptr -> mes.unitdata_req_mes.l3_pdu -> app_field = (uint32)mes_ptr;
          dsm_enqueue( &gllc_ll_ul_unack_wm[ gas_id ][ lle ], &l3_pdu_ptr );
        }
        else
        {
          MSG_GERAN_ERROR_1_G( "GLC LL %d: UL Dat Put: LL-UDATA-REQ: Discard",
            gllc_sapi_addr_lookup[ lle ]);
          GPRS_PDU_FREE( &l3_pdu_ptr );
          GPRS_MEM_FREE( mes_ptr );
        }
      }
      else
      {
        MSG_GERAN_ERROR_1_G( "GLC LL %d: UL Dat Put: LL-UDATA-REQ: No PDU",
          gllc_sapi_addr_lookup[ lle ]);
        GPRS_MEM_FREE( mes_ptr );
      }
      break;

    default:

      MSG_GERAN_ERROR_2_G( "GLC LL %d: UL Dat Put: Invalid Prim %d",
        gllc_sapi_addr_lookup[ lle ], mes_ptr ->mes.prim);
      GPRS_MEM_FREE( mes_ptr );
    }
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_ll_ul_dat_put_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_DL_SIG_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Provides LLC module internal access to the down link message buf get
===    function, which has been registered by the appropriate LL SAP client,
===    during client initialisation.
===
===  DEPENDENCIES
===
===    A client of the LL SAP API must have previously registered a
===    downlink message get buf function for the indexed SAP, during
===    client initialisation.
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

gprs_ll_dl_sig_mes_t *gllc_ll_dl_sig_get_mes_buf( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  gprs_ll_dl_sig_mes_t *mes_ptr = NULL;

  /* Determine whether a client has registered for service at the given
     LL SAPI */

  if ( gllc_ll_client_record[ gas_id ][lle].ll_dl_sig_get_mes_buf_fnc_ptr != NULL )
  {
    mes_ptr = (gllc_ll_client_record[ gas_id ][lle].ll_dl_sig_get_mes_buf_fnc_ptr)();

    if ( mes_ptr == NULL )
    {
      ERR_GERAN_FATAL_1_G( "GLC LL %d: DL Sig Get: No Buffers",
        gllc_sapi_addr_lookup[lle]);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: DL Sig Get: No Client SAPI %d",
      gllc_sapi_addr_lookup[lle]);
  }

  return ( mes_ptr );

} /* end of gllc_ll_dl_sig_get_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_DL_SIG_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Provides LLC module internal access to the down link message put buf
===    function, which has been registered by the appropriate LL SAP client,
===    during client initialisation.
===
===  DEPENDENCIES
===
===    A client of the LL SAP API must have previously registered a
===    downlink message queue put buf function for the indexed SAP, during
===    client initialisation.
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

void gllc_ll_dl_sig_put_mes_buf
(
  gas_id_t              gas_id,
  gllc_ll_sapi_t        lle,
  gprs_ll_dl_sig_mes_t  *mes_ptr
)
{
  /* Determine whether a client has registered for service at the given
     LL SAPI */

  if ( gllc_ll_client_record[ gas_id ][lle].ll_dl_sig_put_mes_buf_fnc_ptr == NULL )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: DL Sig Put: No Client",
      gllc_sapi_addr_lookup[lle]);
  }
  else if ( mes_ptr == NULL )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: DL Sig Put: No Msg",
      gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    /* Invoke the LL SAP Client's registered msg posting function. */

    ( gllc_ll_client_record[ gas_id ][ lle ].ll_dl_sig_put_mes_buf_fnc_ptr )
    ( mes_ptr );
  }

} /* end of gllc_ll_dl_sig_put_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_DL_DAT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Provides LLC module internal access to the down link message put buf
===    function, which has been registered by the appropriate LL SAP client,
===    during client initialisation.
===
===  DEPENDENCIES
===
===    A client of the LL SAP API must have previously registered a
===    downlink message queue put buf function for the indexed SAP, during
===    client initialisation.
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

void gllc_ll_dl_dat_put_mes_buf
(
  gas_id_t              gas_id,
  gllc_ll_sapi_t        lle,
  gprs_ll_dl_dat_mes_t  *mes_ptr
)
{
  /* Determine whether a client has registered for service at the given
     LL SAPI */

  if ( gllc_ll_client_record[ gas_id ][lle].ll_dl_dat_put_mes_buf_fnc_ptr == NULL )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: DL Dat Put: No Client",
      gllc_sapi_addr_lookup[lle]);
  }
  else if ( mes_ptr == NULL )
  {
    MSG_GERAN_ERROR_1_G( "GLC LL %d: DL Dat Put: No Msg",
      gllc_sapi_addr_lookup[lle]);
  }
  else
  {
    /* Invoke the LL SAP Client's registered msg posting function. */

    ( gllc_ll_client_record[ gas_id ][ lle ].ll_dl_dat_put_mes_buf_fnc_ptr )
    ( mes_ptr );
  }

} /* end of gllc_ll_dl_dat_put_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_GET_DEFAULT_N201_I()
===
===  DESCRIPTION
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

uint16 gllc_ll_get_default_N201_I( uint8 sapi )
{
  if ( sapi >= 16 )
  {
    MSG_GERAN_ERROR_1("GLC LL: Get Dflt N201-I: Invalid SAPI %d", sapi);
    return (0);
  }
  else if ( gllc_lle_lookup[sapi] == GLLC_LL_SAPI_RESERVED )
  {
    MSG_GERAN_ERROR_1("GLC LL: Get Dflt N201-I: Reserved SAPI %d", sapi);
    return (0);
  }
  else
    return ( gllc_dflt_N201_I_octets[ gllc_lle_lookup[sapi] ] );

} /* end of gprs_ll_get_default_N201_I() */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_GET_DEFAULT_N201_U()
===
===  DESCRIPTION
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

uint16 gllc_ll_get_default_N201_U( uint8 sapi )
{
  if ( sapi >= 16 )
  {
    MSG_GERAN_ERROR_1("GLC LL: Get Dflt N201-U: Invalid SAPI %d", sapi);
    return (0);
  }
  else if ( gllc_lle_lookup[sapi] == GLLC_LL_SAPI_RESERVED )
  {
    MSG_GERAN_ERROR_1("GLC LL: Get Dflt N201-U: Reserved SAPI %d", sapi);
    return (0);
  }
  else
    return ( gllc_dflt_N201_U_octets[ gllc_lle_lookup[sapi] ] );

} /* end of gprs_ll_get_default_N201_U() */


/*===========================================================================
===
===  FUNCTION      GLLC_LL_GET_MAX_N201_U()
===
===  DESCRIPTION
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

uint16 gllc_ll_get_max_N201_U( uint8 sapi )
{
  if ( sapi >= 16 )
  {
    MSG_ERROR( "GLC LL: Get Max N201-U: Invalid SAPI %d", sapi, 0, 0 );
    return (0);
  }
  else if ( gllc_lle_lookup[sapi] == GLLC_LL_SAPI_RESERVED )
  {
    MSG_ERROR( "GLC LL: Get Max N201-U: Reserved SAPI %d", sapi, 0, 0 );
    return (0);
  }
  else
    return ( gllc_max_N201_U_octets[ gllc_lle_lookup[sapi] ] );

} /* end of gprs_ll_get_max_N201_U() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_GET_GEA_ALGORITHM()
===
===  DESCRIPTION
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

gmmllc_gea_e_type gllc_llme_get_gea_algorithm( gas_id_t gas_id )
{
  
  gmmllc_gea_e_type gea = GMMLLC_GEA_NONE;
  
  if ( ( gas_id >= GERAN_ACCESS_STRATUM_ID_1 ) && ( gas_id <= GERAN_ACCESS_STRATUM_ID_MAX ) )
  {
    gea = gllc_llme[gas_id].gea;
  }

  return gea;

} /* gllc_llme_get_gea_algorithm() */


/* **************************************************************************
   GPRS LLC GRR SAP Interface Functions
   *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_SUSPEND_IND()
===
===  DESCRIPTION
===
===   For use by the RLC layer implementation to signal a data 'suspend'
===   flow control condition to the LLC layer.
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_grr_ul_suspend_ind( gas_id_t gas_id )
{
  MSG_GERAN_LOW_0_G( "GLC ME: Info GRR UL Suspend Ind");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  if ( gllc_llme[ gas_id ].state == GLLC_LLME_TEST_MODE )
  {
    if ( gas_id == GERAN_ACCESS_STRATUM_ID_1 )
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
    else
    {
      MSG_GERAN_ERROR_0_G("GLC LL: Invalid GAS_ID");
    }
  }
  else
    gllc_llme_set_ul_event_mask_scheme( gas_id, GLLC_LLME_SCHEME_GRR_SUSPEND );

} /* end of gllc_grr_ul_suspend_ind() */


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_L3L2_SUSPEND_IND()
===
===  DESCRIPTION
===
===    For use by the RLC layer implementation to signal a data 'suspend'
===    flow control condition to the LLC layer AND the SNDCP layer.
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_grr_ul_L3L2_suspend_ind( gas_id_t gas_id )
{
  uint32 lle;

  MSG_GERAN_MED_0_G( "GLC ME: Info GRR UL L3/L2 Suspend Ind");

  gllc_llme[gas_id].force_l3_flow_control = TRUE;

  for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
  {
    if ( gllc_ll_ul_ack_wm[gas_id][lle].hiwater_func_ptr != NULL )
    {  
#ifdef FEATURE_DSM_WM_CB
      (gllc_ll_ul_ack_wm[gas_id][lle].hiwater_func_ptr)( NULL, NULL);
#else
      (gllc_ll_ul_ack_wm[gas_id][lle].hiwater_func_ptr)();
#endif /* FEATURE_DSM_WM_CB */
    }
    
    if ( gllc_ll_ul_unack_wm[gas_id][lle].hiwater_func_ptr != NULL )
    {
#ifdef FEATURE_DSM_WM_CB
      (gllc_ll_ul_unack_wm[gas_id][lle].hiwater_func_ptr)(NULL, NULL);
#else
      (gllc_ll_ul_unack_wm[gas_id][lle].hiwater_func_ptr)();
#endif /* FEATURE_DSM_WM_CB */
    }
  }

  gllc_grr_ul_suspend_ind( gas_id );
}


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_RESUME_IND()
===
===  DESCRIPTION
===
===   For use by the RLC layer implementation to signal a data 'resume'
===   flow control condition to the LLC layer.
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_grr_ul_resume_ind( gas_id_t gas_id )
{
  uint32 lle;

  MSG_GERAN_LOW_0_G( "GLC ME: Info GRR UL Resume Ind");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  if ( gllc_llme[gas_id].state == GLLC_LLME_TEST_MODE )
  {
    if ( gas_id == GERAN_ACCESS_STRATUM_ID_1 )
    {
      (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_TEST_MODE_SIG );
    }
    else if ( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
    {
      (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2), GLLC_TEST_MODE_SIG );
    }
#ifdef FEATURE_TRIPLE_SIM
    else if ( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
    {
      (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3), GLLC_TEST_MODE_SIG );
    }
#endif /* FEATURE_TRIPLE_SIM */
  }
  else
    gllc_llme_clr_ul_event_mask_scheme( gas_id, GLLC_LLME_SCHEME_GRR_SUSPEND );

  /* If a forcing of layer 3 flow control is active then release layer 3. */

  if ( gllc_llme[gas_id].force_l3_flow_control )
  {
    MSG_GERAN_MED_0_G( "GLC ME: Info GRR UL L3/L2 Resume Ind");

    gllc_llme[gas_id].force_l3_flow_control = FALSE;

    for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
    {
      /* Ensure that the watermark hiwater level has not been crossed
         already before forcibly resuming data flow to that watermark. */

      if ( gllc_ll_ul_ack_wm[gas_id][lle].lowater_func_ptr != NULL )
      {
        if ( gllc_ll_ul_ack_wm[gas_id][lle].current_cnt <=
             gllc_ll_ul_ack_wm[gas_id][lle].hi_watermark   )
        {
#ifdef FEATURE_DSM_WM_CB
          (gllc_ll_ul_ack_wm[gas_id][lle].lowater_func_ptr)( NULL, NULL );
#else
          (gllc_ll_ul_ack_wm[gas_id][lle].lowater_func_ptr)();
#endif /* FEATURE_DSM_WM_CB */
        }
      }
    
      if ( gllc_ll_ul_unack_wm[gas_id][lle].lowater_func_ptr != NULL )
      {
        if ( gllc_ll_ul_unack_wm[gas_id][lle].current_cnt <=
             gllc_ll_ul_unack_wm[gas_id][lle].hi_watermark   )
        {
#ifdef FEATURE_DSM_WM_CB
          (gllc_ll_ul_unack_wm[gas_id][lle].lowater_func_ptr)( NULL, NULL );
#else
          (gllc_ll_ul_unack_wm[gas_id][lle].lowater_func_ptr)();
#endif /* FEATURE_DSM_WM_CB */
        }
      }
    }
  }

} /* end of gllc_grr_ul_resume_ind() */


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_TBF_REL_IND()
===
===  DESCRIPTION
===
===   For use by the RLC layer implementation to inform the LLC layer of
===   UL TBF release.
===
===   This serves to terminate any active Etsi Test Mode A processes.
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

boolean gllc_grr_ul_tbf_rel_ind( gas_id_t gas_id )
{
  if ( gllc_llme[gas_id].state == GLLC_LLME_TEST_MODE )
  {
    MSG_GERAN_HIGH_1_G("GLC ME: Info GPRS-TEST-MODE %d - Ul TBF Rel", gllc_llme[gas_id].test_mode);

    gllc_llme[gas_id].test_llc_pdu_num = 0;

    if( gllc_llme[gas_id].test_mode == 0x01 || gllc_llme[gas_id].test_mode == 0x02 )
    {
      /* For Test Mode B or SRB set the signal to terminate the test mode */
      if ( gas_id == GERAN_ACCESS_STRATUM_ID_1 )
      {
        (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_TEST_MODE_SIG );
      }
      else if ( gas_id == GERAN_ACCESS_STRATUM_ID_2 )
      {
        (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_2), GLLC_TEST_MODE_SIG );
      }

#ifdef FEATURE_TRIPLE_SIM

      else if ( gas_id == GERAN_ACCESS_STRATUM_ID_3 )
      {
        (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GLLC_3), GLLC_TEST_MODE_SIG );
      }

#endif /* FEATURE_TRIPLE_SIM */

    }
    return( TRUE );
  }
  else
    return( FALSE );

} /* end of gllc_grr_ul_tbf_rel_ind() */


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_UL_TRANSMIT_IND()
===
===  DESCRIPTION
===
===   For use by the RLC layer implementation to inform the LLC layer of
===   an LLC PDU transmission on the radio interface, and the transmission
===   cause.
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_grr_ul_transmit_ind
( 
  gas_id_t              gas_id,
  uint8                 sapi,
  uint8                 reliability_class,
  ll_sndcp_ref_t        *sndcp_ref,
  grr_data_req_cause_t  cause
)
{

  gllc_ll_sapi_t lle = gllc_lle_lookup[sapi];

  if ( gas_id > GERAN_ACCESS_STRATUM_ID_MAX )
  {
    MSG_GERAN_ERROR_0_G("GLC GRR UL Tx Ind: Invalid GAS_ID");
    return;
  }

  if ( gas_id != gllc_llme[gas_id].gas_id )
  {
    MSG_GERAN_ERROR_2_G("GLC GRR UL Tx Ind: Invalid GAS_ID: Rcvd %d, Expected %d",
      gas_id, gllc_llme[gas_id].gas_id );
  }
  else
  {

  /* If the transmitted frame is other than a U Frame NULL Command which is
     used for Cell Notification, restart the GMM ready timer if running,
     otherwise signal GMM about frame transmission. */

  if ( cause != GRR_CAUSE_GMM_CELL_NOTIFY )
  {
      if ( gllc_gmm_ready_timer_active[ gas_id ] )
        (void)rex_set_timer( &gllc_gmm_ready_timer[ gas_id ], gllc_gmm_ready_time[ gas_id ] );

    else
        gllc_llme_send_llgmm_trigger_ind_mes( gas_id );
  }

  /* Send LL_UDATA_CNF message to SNDCP to indiate that transmission of UL 
     N-PDU referred to by sndcp_ref has been completed. This is only 
     applicable to layer3 user data PDUs on SAPI = 3, 5, 9, or 11 when 
     operating in reliability class 3, 4, or 5. */
  
  if ( ( GLLC_LLE_IS_3_5_9_11( lle )                ) && 
       ( cause               == GRR_CAUSE_LAY3_DATA ) &&
       ( ( reliability_class == 3 ) ||  
         ( reliability_class == 4 ) ||
         ( reliability_class == 5 )
       ) 
     )
  {
      gllc_lle_send_ll_unitdata_cnf_mes ( gas_id, lle, sndcp_ref );
    }

  }

} /* end of gllc_grr_ul_transmit_ind() */


/*===========================================================================
===
===  FUNCTION      GLLC_GRR_DL_PUT_MES_BUF()
===
===  DESCRIPTION
===
===   For use by the RLC layer implementation to put a dynamically
===   allocated message resource of type gprs_grr_dl_mes_t to the GRR
===   downlink message queue.
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_grr_dl_put_mes_buf( gprs_grr_dl_mes_t *mes_ptr )
{
  gas_id_t gas_id;

  if ( mes_ptr == NULL )
  {
    MSG_GERAN_ERROR_0("GLC GRR DL Put: No Msg");
  }
  else if ( mes_ptr ->gas_id > GERAN_ACCESS_STRATUM_ID_MAX )
  {
    gas_id = mes_ptr ->gas_id;

    MSG_GERAN_ERROR_0_G("GLC GRR DL Put: Invalid GAS_ID ");

    if
    (
      ( mes_ptr -> prim == GRR_DATA_IND     ) ||
      ( mes_ptr -> prim == GRR_UNITDATA_IND )
    )
    {
      if ( mes_ptr -> ll_pdu != NULL )
        GPRS_PDU_FREE( &mes_ptr -> ll_pdu );
    }

    GPRS_MEM_FREE( mes_ptr );  
  }
  else if ( mes_ptr ->gas_id != gllc_llme[ mes_ptr ->gas_id ].gas_id )
  {
    gas_id = mes_ptr ->gas_id;

    MSG_GERAN_ERROR_1_G("GLC GRR DL Put: Invalid GAS_ID: Expected %d", gllc_llme[ mes_ptr ->gas_id ].gas_id);

    if
    (
      ( mes_ptr -> prim == GRR_DATA_IND     ) ||
      ( mes_ptr -> prim == GRR_UNITDATA_IND )
    )
    {
      if ( mes_ptr -> ll_pdu != NULL )
        GPRS_PDU_FREE( &mes_ptr -> ll_pdu );
    }

    GPRS_MEM_FREE( mes_ptr );

  }
  else
  {
    gas_id = mes_ptr ->gas_id;

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    GLLC_DL_MSG_LOCK(gas_id);

    (void)q_link( mes_ptr, &mes_ptr -> link );
    q_put( &gllc_grr_dl_q[ gas_id ], &mes_ptr -> link );

    GLLC_SET_DL_EVT( gas_id );
    
    GLLC_DL_MSG_UNLOCK( gas_id );
  }

} /* end of gllc_grr_dl_put_mes_buf() */

/* **************************************************************************
   GPRS LLC DSM Event Interface Functions
   *********************************************************************** */

/*===========================================================================
===
===  FUNCTION      GLLC_DSM_EVENT_IND_1()
===
===  DESCRIPTION
===
===   For use as a callback by the DSM utilities implementation to signal a
===   low DSM resources condition to the LLC layer.
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

void gllc_dsm_event_ind_1
(
  dsm_mempool_id_enum_type  mempool_id,
  dsm_mem_level_enum_type   mem_level,
  dsm_mem_op_enum_type      mem_op
)
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  /* --------------------------------------------------------------------- */

  if ( mempool_id == DSM_DUP_ITEM_POOL )
  {
    if ( (mem_level == DSM_MEM_LEVEL_MANY) &&
         (mem_op    == DSM_MEM_OP_FREE   )    )
    {
      if ( gllc_llme_ul_evt_mask_scheme_flags[gas_id][GLLC_LLME_SCHEME_DSM_SUSPEND] )
      {
        MSG_GERAN_HIGH_0_G("GLC ME: Info DSM DUP Resume Ind");
        gllc_llme_clr_ul_event_mask_scheme( gas_id , GLLC_LLME_SCHEME_DSM_SUSPEND );
      }
    }
    else if ( (mem_level == DSM_MEM_LEVEL_FEW) &&
              (mem_op    == DSM_MEM_OP_NEW   )    )
    {
      if ( !gllc_llme_ul_evt_mask_scheme_flags[gas_id][GLLC_LLME_SCHEME_DSM_SUSPEND] )
      {
        MSG_GERAN_HIGH_0_G("GLC ME: Info DSM DUP Suspend Ind");
        gllc_llme_set_ul_event_mask_scheme( gas_id , GLLC_LLME_SCHEME_DSM_SUSPEND );
      }
    }
  }

  /* --------------------------------------------------------------------- */

  else
  {
    MSG_GERAN_ERROR_1_G("GLC ME: DSM Evt Ind - Pool Err %d", mempool_id);
  }

  /* --------------------------------------------------------------------- */

} /* end of gllc_dsm_event_ind_1() */

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
===
===  FUNCTION      GLLC_DSM_EVENT_IND_2()
===
===  DESCRIPTION
===
===   For use as a callback by the DSM utilities implementation to signal a
===   low DSM resources condition to the LLC layer.
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

void gllc_dsm_event_ind_2
(
  dsm_mempool_id_enum_type  mempool_id,
  dsm_mem_level_enum_type   mem_level,
  dsm_mem_op_enum_type      mem_op
)
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_2;

  /* --------------------------------------------------------------------- */

  if ( mempool_id == DSM_DUP_ITEM_POOL )
  {
    if ( (mem_level == DSM_MEM_LEVEL_MANY) &&
         (mem_op    == DSM_MEM_OP_FREE   )    )
    {
      if ( gllc_llme_ul_evt_mask_scheme_flags[gas_id][GLLC_LLME_SCHEME_DSM_SUSPEND] )
      {
        MSG_GERAN_HIGH_0_G( "GLC ME: Info DSM DUP Resume Ind");
        gllc_llme_clr_ul_event_mask_scheme( gas_id , GLLC_LLME_SCHEME_DSM_SUSPEND );
      }
    }
    else if ( (mem_level == DSM_MEM_LEVEL_FEW) &&
              (mem_op    == DSM_MEM_OP_NEW   )    )
    {
      if ( !gllc_llme_ul_evt_mask_scheme_flags[gas_id][GLLC_LLME_SCHEME_DSM_SUSPEND] )
      {
        MSG_GERAN_HIGH_0_G( "GLC ME: Info DSM DUP Suspend Ind");
        gllc_llme_set_ul_event_mask_scheme( gas_id , GLLC_LLME_SCHEME_DSM_SUSPEND );
      }
    }
  }

  /* --------------------------------------------------------------------- */

  else
  {
    MSG_GERAN_ERROR_1_G( "GLC ME: DSM Evt Ind - Pool Err %d", mempool_id);
  }

  /* --------------------------------------------------------------------- */

} /* end of gllc_dsm_event_ind_2() */
#endif

#ifdef FEATURE_TRIPLE_SIM

/*===========================================================================
===
===  FUNCTION      GLLC_DSM_EVENT_IND_3()
===
===  DESCRIPTION
===
===   For use as a callback by the DSM utilities implementation to signal a
===   low DSM resources condition to the LLC layer.
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

void gllc_dsm_event_ind_3
(
  dsm_mempool_id_enum_type  mempool_id,
  dsm_mem_level_enum_type   mem_level,
  dsm_mem_op_enum_type      mem_op
)
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_3;

  /* --------------------------------------------------------------------- */

  if ( mempool_id == DSM_DUP_ITEM_POOL )
  {
    if ( (mem_level == DSM_MEM_LEVEL_MANY) &&
         (mem_op    == DSM_MEM_OP_FREE   )    )
    {
      if ( gllc_llme_ul_evt_mask_scheme_flags[gas_id][GLLC_LLME_SCHEME_DSM_SUSPEND] )
      {
        MSG_GERAN_HIGH_0_G( "GLC ME: Info DSM DUP Resume Ind");
        gllc_llme_clr_ul_event_mask_scheme( gas_id , GLLC_LLME_SCHEME_DSM_SUSPEND );
      }
    }
    else if ( (mem_level == DSM_MEM_LEVEL_FEW) &&
              (mem_op    == DSM_MEM_OP_NEW   )    )
    {
      if ( !gllc_llme_ul_evt_mask_scheme_flags[gas_id][GLLC_LLME_SCHEME_DSM_SUSPEND] )
      {
        MSG_GERAN_HIGH_0_G( "GLC ME: Info DSM DUP Suspend Ind");
        gllc_llme_set_ul_event_mask_scheme( gas_id , GLLC_LLME_SCHEME_DSM_SUSPEND );
      }
    }
  }

  /* --------------------------------------------------------------------- */

  else
  {
    MSG_GERAN_ERROR_1_G( "GLC ME: DSM Evt Ind - Pool Err %d", mempool_id);
  }

  /* --------------------------------------------------------------------- */

} /* end of gllc_dsm_event_ind_3() */

#endif /* FEATURE_TRIPLE_SIM */


/****************************************************************************
   GPRS LLC Diagnostic Interface Functions
****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_REGISTER()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to register the necessary
===    callback functions for unsolicited event reporting.
===
===    Registering or re-registering with a NULL callback pointer
===    disables the associated unsolicited event reporting category.
===
===    For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_diag_register
(
  gas_id_t                         gas_id,
  gllc_diag_rpt_llme_state_fnc_t   llme_state_fnc_p,
  gllc_diag_rpt_lle_state_fnc_t    lle_state_fnc_p,
  gllc_diag_rpt_lle_xid_fnc_t      lle_xid_fnc_p
)
{
  uint32 lle;

  GLLC_LOCK(gas_id);

  if ( llme_state_fnc_p != NULL )
  {
    gllc_diag_rpt_llme_state_fnc_p[gas_id] = llme_state_fnc_p;
    gllc_diag_rpt_llme_state(gas_id);
  }

  if ( lle_state_fnc_p != NULL )
  {
    gllc_diag_rpt_lle_state_fnc_p[gas_id] = lle_state_fnc_p;

    for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
      gllc_diag_rpt_lle_state( gas_id, (gllc_ll_sapi_t)lle );
  }

  if ( lle_xid_fnc_p != NULL )
  {
    gllc_diag_rpt_lle_xid_fnc_p[gas_id] = lle_xid_fnc_p;

    for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
      gllc_diag_rpt_lle_xid( gas_id, (gllc_ll_sapi_t)lle );
  }

  GLLC_UNLOCK(gas_id);

} /* end of gllc_diag_register() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RPT_LLME_STATE()
===
===  DESCRIPTION
===
===   Constructs an LLME state report for internal use by LLC to initiate
===   unsolicited reporting of changes to the DIAG process.
===
===   If the DIAG process has not registered a reporting callback function
===   for this category then no report is generated.
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

void gllc_diag_rpt_llme_state( gas_id_t gas_id )
{

  if ( gllc_diag_rpt_llme_state_fnc_p[gas_id] != NULL )
  {
    llme_rpt[gas_id].llme_state               = (uint8)gllc_llme[gas_id].state;
    llme_rpt[gas_id].llme_xid_version         = gllc_llme[gas_id].llc_version;
    llme_rpt[gas_id].llme_new_tlli            = gllc_llme[gas_id].tlli;
    llme_rpt[gas_id].llme_old_tlli            = gllc_llme[gas_id].tlli_old;
    llme_rpt[gas_id].llme_encrypt_alg         = (uint8)gllc_llme[gas_id].gea;
    llme_rpt[gas_id].llme_encrypt_key[0]      = gllc_llme[gas_id].key_Kc[0];
    llme_rpt[gas_id].llme_encrypt_key[1]      = gllc_llme[gas_id].key_Kc[1];
    llme_rpt[gas_id].llme_encrypt_key128[0]   = gllc_llme[gas_id].key_Kc128[0];
    llme_rpt[gas_id].llme_encrypt_key128[1]   = gllc_llme[gas_id].key_Kc128[1];
    llme_rpt[gas_id].llme_encrypt_key128[2]   = gllc_llme[gas_id].key_Kc128[2];
    llme_rpt[gas_id].llme_encrypt_key128[3]   = gllc_llme[gas_id].key_Kc128[3];
    llme_rpt[gas_id].llme_input_offset_iov_ui = gllc_llme[gas_id].input_offset_IOV_UI;

    ( gllc_diag_rpt_llme_state_fnc_p[gas_id] )( &llme_rpt[gas_id] );
  }

} /* end of gllc_diag_rpt_llme_state() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RPT_LLE_STATE()
===
===  DESCRIPTION
===
===   Constructs an LLE state report for internal use by LLC to initiate
===   unsolicited reporting of changes to the DIAG process.
===
===   If the DIAG process has not registered a reporting callback function
===   for this category then no report is generated.
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

void gllc_diag_rpt_lle_state( gas_id_t gas_id, gllc_ll_sapi_t lle )
{

  if ( gllc_diag_rpt_lle_state_fnc_p[gas_id] != NULL )
  {
    lle_rpt[gas_id].lle_sapi_addr = gllc_sapi_addr_lookup[lle];
    lle_rpt[gas_id].lle_state     = (uint8)gllc_lle[gas_id][lle].state;
    lle_rpt[gas_id].lle_substate  = (uint8)gllc_lle[gas_id][lle].substate;

    ( gllc_diag_rpt_lle_state_fnc_p[gas_id] )( &lle_rpt[gas_id] );
  }

} /* end of gllc_diag_rpt_lle_state() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RPT_LLE_XID()
===
===  DESCRIPTION
===
===   Constructs an LLE XID report for internal use by LLC to initiate
===   unsolicited reporting of changes to the DIAG process.
===
===   If the DIAG process has not registered a reporting callback function
===   for this category then no report is generated.
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

void gllc_diag_rpt_lle_xid( gas_id_t gas_id, gllc_ll_sapi_t lle )
{

  if ( gllc_diag_rpt_lle_xid_fnc_p[gas_id] != NULL )
  {
    xid_rpt[gas_id].lle_sapi_addr      = gllc_sapi_addr_lookup[lle];
    xid_rpt[gas_id].pending_xid_bitmap = gllc_lle[gas_id][lle].unnum.xid_pending;
    xid_rpt[gas_id].current_iov_i      = gllc_lle[gas_id][lle].ack.input_offset_IOV_I;

    xid_rpt[gas_id].current_t200       = gllc_lle[gas_id][lle].gen.retrx_interval_T200;
    xid_rpt[gas_id].preferred_t200     = gllc_prfd_T200_ms[lle];

    xid_rpt[gas_id].current_n200       = (uint8)gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200;
    xid_rpt[gas_id].preferred_n200     = gllc_prfd_N200[lle];

    xid_rpt[gas_id].current_n201_u     = gllc_lle[gas_id][lle].unack.max_info_octets_N201_U;
    xid_rpt[gas_id].preferred_n201_u   = gllc_prfd_N201_U_octets[gllc_llme[gas_id].gan_state][lle];

    xid_rpt[gas_id].current_n201_i     = gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;;
    xid_rpt[gas_id].preferred_n201_i   = gllc_prfd_N201_I_octets[gllc_llme[gas_id].gan_state][lle];

    xid_rpt[gas_id].current_md         = gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD;
    xid_rpt[gas_id].preferred_md       = (uint16)(gllc_prfd_mD_octet16[lle] * 16);

    xid_rpt[gas_id].current_mu         = gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU;
    xid_rpt[gas_id].preferred_mu       = (uint16)(gllc_prfd_mU_octet16[lle] * 16);

    xid_rpt[gas_id].current_kd         = gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD;
    xid_rpt[gas_id].preferred_kd       = gllc_prfd_kD_frames[lle];

    xid_rpt[gas_id].current_ku         = gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU;
    xid_rpt[gas_id].preferred_ku       = gllc_prfd_kU_frames[lle];

    ( gllc_diag_rpt_lle_xid_fnc_p[gas_id] )( &xid_rpt[gas_id] );
  }

} /* end of gllc_diag_rpt_lle_xid() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_GET_PDU_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the current set of
===    PDU and octet statistics for the LLE indicated by the given SAPI
===    address.
===
===    For a complete description refer to interface gllc.h.
===
===========================================================================*/

const gllc_diag_pdu_stat_rsp_t *gllc_diag_get_pdu_stats
(
  gas_id_t   gas_id,
  uint8 sapi_addr
)
{

         gllc_ll_sapi_t            lle   = gllc_lle_lookup[sapi_addr];

  if ( sapi_addr < 16 )
  {
    if ( lle != GLLC_LL_SAPI_RESERVED )
    {
      GLLC_LOCK(gas_id);
      (void)memscpy
      (
        &pdu_stat_rpt[gas_id],
        sizeof(gllc_diag_pdu_stat_rsp_t),
        &gllc_lle[gas_id][lle].diag.pdu_stats, 
        sizeof(gllc_diag_pdu_stat_rsp_t)
      );
      GLLC_UNLOCK(gas_id);

      pdu_stat_rpt_ptr[gas_id] = &pdu_stat_rpt[gas_id];
    }
    else
    {
      MSG_GERAN_ERROR_1_G( "GLC DIAG: Reserved SAPI %d", sapi_addr);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G( "GLC DIAG: Invalid SAPI %d", sapi_addr);
  }

  return ( pdu_stat_rpt_ptr[gas_id] );

} /* end of gllc_diag_get_pdu_stats() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RESET_PDU_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the reset of the
===    PDU and octet statistics for the LLE indicated by the given SAPI
===    address.
===
===    For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_diag_reset_pdu_stats
(
  gas_id_t gas_id,
  uint8 sapi_addr
)
{
  gllc_ll_sapi_t lle = gllc_lle_lookup[sapi_addr];

  if ( sapi_addr < 16 )
  {
    if ( lle != GLLC_LL_SAPI_RESERVED )
    {
      GLLC_LOCK(gas_id);
      (void)memset
      (
        &gllc_lle[gas_id][lle].diag.pdu_stats,
        0,
        sizeof(gllc_diag_pdu_stat_rsp_t)
      );
      gllc_lle[gas_id][lle].diag.pdu_stats.lle_sapi_addr = sapi_addr;
      GLLC_UNLOCK(gas_id);
    }
    else
    {
      MSG_GERAN_ERROR_1("GLC DIAG: Reserved SAPI %d", sapi_addr);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GLC DIAG: Invalid SAPI %d", sapi_addr);
  }

} /* end of gllc_diag_reset_pdu_stats() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_GET_STATE_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the current set of
===    state variables and anciliary counts for the LLE indicated by the
===    given SAPI address.
===
===    For a complete description refer to interface gllc.h.
===
===========================================================================*/

const gllc_diag_state_stat_rsp_t *gllc_diag_get_state_stats
(
  gas_id_t gas_id,
  uint8 sapi_addr
)
{
         gllc_ll_sapi_t              lle   = gllc_lle_lookup[sapi_addr];

  if ( sapi_addr < 16 )
  {
    if ( lle != GLLC_LL_SAPI_RESERVED )
    {
      GLLC_LOCK(gas_id);

      pdu_stat_rsp[gas_id].lle_sapi_addr = sapi_addr;

      /* Capture snapshot of LLE state variables. */

      pdu_stat_rsp[gas_id].unack_snd_seq_var_v_u  = gllc_lle[gas_id][lle].unack.send_state_var_V_U;
      pdu_stat_rsp[gas_id].unack_rcv_seq_var_v_ur = gllc_lle[gas_id][lle].unack.recv_state_var_V_UR;
      pdu_stat_rsp[gas_id].unack_encipher_oc      = gllc_lle[gas_id][lle].unack.encipher_oc;
      pdu_stat_rsp[gas_id].unack_decipher_oc      = gllc_lle[gas_id][lle].unack.decipher_oc;

      pdu_stat_rsp[gas_id].ack_snd_seq_var_v_s    = gllc_lle[gas_id][lle].ack.send_state_var_V_S;
      pdu_stat_rsp[gas_id].ack_rcv_seq_var_v_r    = gllc_lle[gas_id][lle].ack.recv_state_var_V_R;
      pdu_stat_rsp[gas_id].ack_seq_var_v_a        = gllc_lle[gas_id][lle].ack.ackn_state_var_V_A;
      pdu_stat_rsp[gas_id].ack_encipher_oc        = gllc_lle[gas_id][lle].ack.encipher_oc;
      pdu_stat_rsp[gas_id].ack_decipher_oc        = gllc_lle[gas_id][lle].ack.decipher_oc;

      /* Capture snapshot of LLE ancilliary statistics. */

      pdu_stat_rsp[gas_id].reestablishment_cnt    = gllc_lle[gas_id][lle].diag.reestablishment_cnt;
      pdu_stat_rsp[gas_id].gen_purpose_a_cnt      = gllc_lle[gas_id][lle].diag.general_a_cnt;
      pdu_stat_rsp[gas_id].gen_purpose_b_cnt      = gllc_lle[gas_id][lle].diag.general_b_cnt;
      pdu_stat_rsp[gas_id].gen_purpose_c_cnt      = gllc_lle[gas_id][lle].diag.general_c_cnt;

      pdu_stat_rsp_ptr[gas_id]                    = &pdu_stat_rsp[gas_id];

      GLLC_UNLOCK(gas_id);
    }
    else
    {
      MSG_GERAN_ERROR_1("GLC DIAG: Reserved SAPI %d", sapi_addr);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GLC DIAG: Invalid SAPI %d", sapi_addr);
  }

  return pdu_stat_rsp_ptr[gas_id];

} /* end of gllc_diag_get_state_stats() */


/*===========================================================================
===
===  FUNCTION      GLLC_DIAG_RESET_ANCILLIARY_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the reset of the
===    anciliary statistics counters for the LLE indicated by the given SAPI
===    address.
===
===    For a complete description refer to interface gllc.h.
===
===========================================================================*/

void gllc_diag_reset_ancilliary_stats
(
  gas_id_t gas_id,
  uint8 sapi_addr
)
{
  gllc_ll_sapi_t lle = gllc_lle_lookup[sapi_addr];

  if ( sapi_addr < 16 )
  {
    if ( lle != GLLC_LL_SAPI_RESERVED )
    {
      GLLC_LOCK(gas_id);

      gllc_lle[gas_id][lle].diag.reestablishment_cnt = 0;
      gllc_lle[gas_id][lle].diag.general_a_cnt       = 0;
      gllc_lle[gas_id][lle].diag.general_b_cnt       = 0;
      gllc_lle[gas_id][lle].diag.general_c_cnt       = 0;

      GLLC_UNLOCK(gas_id);
    }
    else
    {
      MSG_GERAN_ERROR_1("GLC DIAG: Reserved SAPI %d", sapi_addr);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GLC DIAG: Invalid SAPI %d", sapi_addr);
  }

} /* end of gllc_diag_reset_ancilliary_stats() */


/*** EOF: don't remove! ***/

