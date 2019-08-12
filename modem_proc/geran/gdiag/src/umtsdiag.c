/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Routines
                  UMTS Target specific Routines

General Description
  Diagnostic packet processing routines that are specific to UMTS
  target.

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdiag/src/umtsdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
22/10.14   js       CR693771 W+T removal. Resolving compilation errors
14/03/12   pg      Add support for local HWIO macros (msm.h deprecated)
26/04/11   sjw     Removed calls to classmark functions in umtsdiag_verno
29/10/10   sjw     Added support for FEATURE_DUAL_SIM
27/10/08   cs      Fix more HIGH Lint errors
06/01/05   ty      Lint fixes
04/06/04   ao      GSM PLT mods with FEATURE_GSM_PLT flag
12/11/02   gsc     Fixed typo FETAURE_WPLT.
12/11/02   gsc     Added support to process the wcdma AGC_INFO request
                   packet from QXDM.
12/08/02   djm     corrected WPLT featurization issue
10/18/02   ATM     Bugfixes for GPLT
09/10/02   ATM     Added CM_STATE and AGC subcommands
08/28/02   gsc     Featurized out CFA stuff for WCDMA PLT.
07/17/02   at      Change calls to gs_read_coded_classmark_2 and 3.
04/10/02   ATM     Added Front-End Filter to CFA
04/01/02   ATM     Added check for NULL after call to diagpkt_subsys_alloc
03/06/02   abp     For FEATURE_WPLT libprot.h is not included. This is header
                   file needed to read classmark info
02/13/02   ATM     Updated command set to match UMTS ICD Revision A
12/10/01   ATM     Created file - adapted from diagpkt_umts.c

===========================================================================*/

#include "geran_variation.h"
#if defined __cplusplus
  extern "C" {
#endif

#include "customer.h"
#include "comdef.h"

#include <stringl.h>     // For memscpy()

#include "umtsdiag.h"
#include "umtsdiag_v.h"
#include "umtsdiag_v_g.h"
#include "gsmdiag_i.h"
#include "diagcmd.h"
#include "msg.h"
#include "event.h"          // For logging IMSI event and event codes
#include "l1_log.h"

/* For PLT, or if NAS is not included, do not include libprot.h */
#if !defined (FEATURE_WPLT) && !defined (FEATURE_GSM_PLT)
# include "libprot.h"
# ifdef FEATURE_DUAL_SIM
#  include "sys.h"
# endif /* FEATURE_DUAL_SIM */
#endif

#include "cm.h"
#include "cm_v.h"
#include "cmlog.h"

/* Moved here from mobile.h */
/* 3GPP REVISION -- This has been revised for UMTS version packet rsp  */
#if defined(FEATURE_WCDMA_REL6)
# define G3GPP_REV_MONTH    06 /* June    (Jan=1...Dec=12)             */
# define G3GPP_REV_YEAR     07 /* 2006    (99=1999,00=2000...98=2098)  */
# define G3GPP_REL        0x06 /* Release number, from 3 up*/
#else
# define G3GPP_REV_MONTH    03 /* March    (Jan=1...Dec=12)             */
# define G3GPP_REV_YEAR     04 /*  2002    (99=1999,00=2000...98=2098)  */
# define G3GPP_REL        0x03 /* Release number, from 3 up*/
#endif

/*===========================================================================
Function: UMTSDIAG_VERNO
Description:

       This function processes the umts specific version request packet
       from QXDM and sends the response back.

============================================================================*/
PACKED void * PACKED_POST umtsdiag_verno(
    /*lint -save -esym(715,req_pkt) -esym(715,pkt_len) */
    /* request packet is not used, so no need to check pkt_len.
     * Having the packet activate this handler is enough to
     * elicit the rsp pkt
     */
    PACKED void* PACKED_POST req_pkt,/* pointer to valid request packet */
    uint16 pkt_len       /* length of the request packet */
    /*lint -restore */
    )
{
  DIAG_SUBSYS_UMTS_UMTS_DIAG_VERNO_F_rsp_type *rsp_ptr ;

  const int32 rsp_len = sizeof(DIAG_SUBSYS_UMTS_UMTS_DIAG_VERNO_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr =
    (DIAG_SUBSYS_UMTS_UMTS_DIAG_VERNO_F_rsp_type *)
    diagpkt_subsys_alloc((int32)DIAG_SUBSYS_UMTS, UMTS_DIAG_VERNO_F,
    rsp_len );

  if (rsp_ptr != NULL)
  {
    /* Fill the response packet */
    /* 3GPP Revision Info -- from mobile.h */
    rsp_ptr->g3gpp_rev_month  = G3GPP_REV_MONTH;
    rsp_ptr->g3gpp_rev_year   = G3GPP_REV_YEAR;
    rsp_ptr->g3gpp_rel        = G3GPP_REL;

    /* Classmark Info defaults to zero length fields since it is not used */
    rsp_ptr->class_mark2[0]   = 0;
    rsp_ptr->class_mark3[0]   = 0;

    /* Discontinuous Receive Info */
    rsp_ptr->drx_parameter[0] = 0x0F;
    rsp_ptr->drx_parameter[1] = 0xF0;
  }

  return rsp_ptr;
}

/*===========================================================================
Function: umtsdiag_cm_state_info
Description:

       This function processes the umts specific call manager state
       information request packet from QXDM and sends the response back.

============================================================================*/
PACKED void * PACKED_POST umtsdiag_cm_state_info (PACKED void* PACKED_POST req_pkt,
                                                  uint16 pkt_len)
{
  DIAG_SUBSYS_UMTS_UMTS_CM_STATE_INFO_F_rsp_type *rsp_ptr ;

  const int32 rsp_len = sizeof(DIAG_SUBSYS_UMTS_UMTS_CM_STATE_INFO_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr =
    (DIAG_SUBSYS_UMTS_UMTS_CM_STATE_INFO_F_rsp_type *)
    diagpkt_subsys_alloc((int32)DIAG_SUBSYS_UMTS, UMTS_CM_STATE_INFO_F,
    rsp_len );

  if (rsp_ptr != NULL) /* Fill the response packet */
  {
    /* Query Call Manager for State information */
    rsp_ptr->operating_mode = cmlog_get_operating_mode();
#ifdef FEATURE_DUAL_SIM
    /* for Dual SIM, default to the first subscription ID */
    rsp_ptr->call_state     = cmlog_get_call_state_per_subs(SYS_MODEM_AS_ID_1);
    rsp_ptr->system_mode    = cmlog_get_system_mode_per_subs(SYS_MODEM_AS_ID_1);
#else
    rsp_ptr->call_state     = cmlog_get_call_state();
    rsp_ptr->system_mode    = cmlog_get_system_mode();
#endif /* FEATURE_DUAL_SIM */
  }

  return rsp_ptr;
}

/*===========================================================================
Function: umtsdiag_get_agc_info
Description:

       This function processes the umts specific AGC_INFO request packet
       from QXDM and sends the response back.

============================================================================*/
#if defined(FEATURE_WCDMA) && !defined(FEATURE_GSM_PLT)
# include "dldemlog.h"  /* for agc_sample_struct_type */
extern  WCDMA_AGC_LOG_PACKET_type rf_agc_log_pkt;
extern  uint16 rf_agc_log_cnt;
#endif

PACKED void * PACKED_POST umtsdiag_get_agc_info (PACKED void* PACKED_POST req_pkt,
                                                 uint16 pkt_len)
  /*lint -esym(613,req_pkt) Diag system does not dispatch to handler
   * functions if the req pkt is null
   */
{
  /* AGC FORMATS DEFINED */
  enum {
    WCDMA_AGC_INFO = 0
    ,GSM_AGC_INFO   = 1
  };

  DIAG_SUBSYS_UMTS_UMTS_AGC_INFO_F_rsp_type *rsp_ptr = NULL;
  DIAG_SUBSYS_UMTS_UMTS_AGC_INFO_F_req_type *req_ptr
    = (DIAG_SUBSYS_UMTS_UMTS_AGC_INFO_F_req_type *)req_pkt;

  /* Set minimum rsp_len */
  uint32 rsp_len = FPOS(DIAG_SUBSYS_UMTS_UMTS_AGC_INFO_F_rsp_type, data);
#if (defined(FEATURE_WCDMA) && !defined(FEATURE_GSM_PLT)) || (!defined(FEATURE_GSM_PLT) && !defined(FEATURE_WCDMA_PLT))
  uint32 rsp_data_bfr_max_len;
#endif
  if ((pkt_len < sizeof(DIAG_SUBSYS_UMTS_UMTS_AGC_INFO_F_req_type)) ||
    (req_ptr == NULL))
  {
    return rsp_ptr;
  }

  /* This switch statement computes rsp_len for the operation */
  switch ( req_ptr->format )
  {
#if defined(FEATURE_WCDMA) && !defined(FEATURE_GSM_PLT)
    case WCDMA_AGC_INFO:
    {
      rsp_len += sizeof(agc_sample_struct_type);
    }
      break;
#endif
#if  !defined(FEATURE_GSM_PLT) && !defined(FEATURE_WCDMA_PLT)
    case GSM_AGC_INFO:
    {
      rsp_len += sizeof(l1_log_burst_metrics_T);
    }
      break;
#endif
    default:
    {
      /* No adjustment to rsp_len */
    }
      break;
  }/*end switch*/

  /* Allocate the memory for this */
  rsp_ptr =
    (DIAG_SUBSYS_UMTS_UMTS_AGC_INFO_F_rsp_type *)
    diagpkt_subsys_alloc((int32)DIAG_SUBSYS_UMTS, UMTS_AGC_INFO_F,
    rsp_len );
#if (defined(FEATURE_WCDMA) && !defined(FEATURE_GSM_PLT)) || (!defined(FEATURE_GSM_PLT) && !defined(FEATURE_WCDMA_PLT))
  /* calculate the maximum length for the data buffer in that struct */
  rsp_data_bfr_max_len = rsp_len - FPOS(DIAG_SUBSYS_UMTS_UMTS_AGC_INFO_F_rsp_type, data);
#endif
  if (rsp_ptr != NULL) /* Fill the response packet */
  { /* Copy values from request */
    rsp_ptr->format = req_ptr->format;

    /* FILL IN AGC INFO ACCORDINGLY */
    switch ( req_ptr->format ) 
    {
#if defined(FEATURE_WCDMA) && !defined(FEATURE_GSM_PLT)
      case WCDMA_AGC_INFO:
      {
        rsp_ptr->length = sizeof(agc_sample_struct_type);
        /*lint -save -e419 */
        /* apparent data overrun warning is not applicable. Code is
        * written according to Diagnostic Services User Guide
        * and is larger than necessary.
        */
        if (!rf_agc_log_cnt)
        {
          memscpy((void*)rsp_ptr->data,
            rsp_data_bfr_max_len,
            (void *)&(rf_agc_log_pkt.agc_samp[rf_agc_log_cnt]),
            sizeof(agc_sample_struct_type));
        }
        else
        {
          memscpy((void*)rsp_ptr->data,
            rsp_data_bfr_max_len,
            (void *)&(rf_agc_log_pkt.agc_samp[rf_agc_log_cnt-1]),
            sizeof(agc_sample_struct_type));
        }
        /*lint -restore */
      }
        break;
#endif
#if  !defined(FEATURE_GSM_PLT) && !defined(FEATURE_WCDMA_PLT)
      case GSM_AGC_INFO:
      {
        rsp_ptr->length = sizeof(l1_log_burst_metrics_T);
        /*lint -save -e419 */
        memscpy((void*) rsp_ptr->data,
          rsp_data_bfr_max_len,
          (void*)&l1_log_gsm_agc_info,
          sizeof(l1_log_burst_metrics_T) );
        /*lint -restore */
      }
        break;
#endif
      default:
      {
        rsp_ptr->length = 0;
      }
        break;
    }/*end switch*/
  }

  return rsp_ptr ;
}

/*===========================================================================
Function: umtsdiag_set_imsi
Description:
       This function updates the cached IMSI.
Side Effect:
       Emits the IMSI event.
============================================================================*/
#define _IMSI_SIZE  (9)
static uint8 umtsdiag_cached_imsi[_IMSI_SIZE] = {0,0,0,0,0,0,0,0,0};
void umtsdiag_set_imsi( uint8 length, const uint8 *data )
{
  if (data == NULL)
    return;

  umtsdiag_cached_imsi[0] = length;
  memscpy(&umtsdiag_cached_imsi[1],_IMSI_SIZE-1, data,_IMSI_SIZE-1);
  event_report_payload(EVENT_WCDMA_IMSI,_IMSI_SIZE,(void *)umtsdiag_cached_imsi);
}

/*===========================================================================
Function: umtsdiag_get_imsi
Description:
       This function returns a copy of the cached IMSI (for GSM logging).
============================================================================*/
void umtsdiag_get_imsi( uint8 *imsi )
{
  if (imsi == NULL)
    return;
  memscpy(imsi, _IMSI_SIZE, umtsdiag_cached_imsi, _IMSI_SIZE);
}

/*===========================================================================
Function: umtsdiag_set_imei
Description:
       This function updates the cached IMEI.
============================================================================*/
#define _IMEI_SIZE  (9)
static uint8 umtsdiag_cached_imei[_IMEI_SIZE] = {0,0,0,0,0,0,0,0,0};
void umtsdiag_set_imei( const uint8 *imei )
{
  if (imei == NULL)
    return;
  memscpy(umtsdiag_cached_imei, _IMEI_SIZE, imei, _IMEI_SIZE);
}

/*===========================================================================
Function: umtsdiag_get_imei
Description:
       This function returns a copy of the cached IMEI (for GSM logging).
============================================================================*/
void umtsdiag_get_imei( uint8 *imei )
{
  if (imei == NULL)
    return;
  memscpy(imei, _IMEI_SIZE, umtsdiag_cached_imei, _IMEI_SIZE);
}

/**************************************************************************
** UMTS SUBSYSTEM COMMAND DISPATCH TABLE.
Note:  Any new commands for UMTS need to be added here.
****************************************************************************/

#ifndef FEATURE_WPLT
extern PACKED void * PACKED_POST cfa_diag_config_pkt (PACKED void * PACKED_POST, word);
#endif

static const diagpkt_user_table_entry_type umtsdiag_tbl[] =
{
  {UMTS_DIAG_VERNO_F, UMTS_DIAG_VERNO_F, umtsdiag_verno}
#ifndef FEATURE_WPLT
  ,{UMTS_CFA_CONFIG_F, UMTS_CFA_CONFIG_F, cfa_diag_config_pkt}
#endif
  ,{UMTS_CM_STATE_INFO_F, UMTS_CM_STATE_INFO_F, umtsdiag_cm_state_info}
  ,{UMTS_AGC_INFO_F, UMTS_AGC_INFO_F, umtsdiag_get_agc_info}
};

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_UMTS, umtsdiag_tbl);

#else

  void umtsdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER ((int32)DIAG_SUBSYS_UMTS, umtsdiag_tbl);
  }

#endif

#if defined __cplusplus
  }
#endif
