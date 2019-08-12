/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Routines
                  GSM Target specific Routines

General Description
  Diagnostic packet processing routines that are specific to GSM
  target.

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdiag/src/gsmdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
20/05/14   gk      CR668704 Initialize the gsmdiag_cached_gsm_state variable to OFFLINE. A correction over CR662897
19/05/14   tw      CR662897 Unable to restore QCN on 1x compiled out flavour (M8916EAAAANWAM)
07/06/12   tjw     CR367010 Check for req_ptr buffer overflows.
10/05/11   sjw     Add DSDS featurisation for FEATURE_DUAL_IMEI
29/10/10   sjw     Added support for FEATURE_DUAL_SIM
09/19/05   pp      Don't wait for 7th call to gsmdiag_status() before reading
                   the IMSI.
02/02/05   pp      Lint cleanup
08/26/02   Rex     Implemented GSM diag for call orig and call end
12/09/02   ATM     Move IMSI & IMEI logging to umtsdiag -- simplifies deps
12/08/02   djm     corrected WPLT featurization issue
10/18/02   ATM     Bugfixes for GPLT
09/06/02   ADG     Added GPRS_LOG_PACKET_REQUEST command
10/06/02   ATM     Requested change for PLT
10/04/02   ATM     Interface change to GSDI
09/19/02   tjw     gsmdiag_tmc_state_retrieval returns TMC_STATE_ONLINE
                   for TMC_LPM to keep QPST happy
08/26/02   JC      Eliminated return of NULL from gsmdiag_status(),
                   doesn't seem to be handled well by DIAG.
07/22/02   JC      Reading IMSI from GSDI cache instead of coupling to MM.
06/19/02   ATM     Added changes suggested by Bala Sripathirathan for dual-mode build
05/31/02   ATM     Renamed FEATURE_GPLT to FEATURE_GSM_PLT
05/06/02   ATM     Added FEATURE_CM flags around CM_STATE code
04/30/02   ATM     Change gmdspext.h to mdsp_intf.h (externalized for both modes)
04/26/02   ATM     Replaced MS_STATE with CM_CALL_STATE,
                                          CM_OPERATING_STATE,
                                          CM_SYSTEM_STATE
04/16/02   bk      Changed mdspext.h to gmdspext.h to support new mdsp image
03/25/02   ATM     Get MDSP version info from mdspext.h not mdsp_int.h
03/08/02   ATM     Get DSP version info from DSP code, not mobile.h
02/13/02   ATM     Updated command set to match GSM ICD
02/12/02   ATM     Fixed bug - Packing the LAC incorrectly
02/01/02   ATM     Added GPLT support
12/10/01   ATM     Created file - adapted from diagpkt_gsm.c

===========================================================================*/

#include "geran_variation.h"
#if defined __cplusplus
  extern "C" {
#endif

#include "customer.h"
#include "comdef.h"

#include <stringl.h>     /* for memscpy, memset */

#include "gsmdiag.h"
#include "gsmdiag_i.h"
#include "diagcmd.h"
#include "msg.h"

#include "mdsp_intf.h"
#include "mdsp_intf_g.h"
#include "umtsdiag_v.h"
#include "umtsdiag_v_g.h"     /* For IMSI & IMEI       */
#include "rr_diag_if.h"    /* for system_information_database_T & rr_state_T */
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"   /* for ghdi_status_T */
#include "cmlog.h"
#include "cm.h"
#include "cm_v.h"
#if defined (CM_MAX_NUMBER_CHARS_UMTS) /* using the (newer) CMI CM API */
  # define LOCAL_CM_MAX_NUMBER_CHARS CM_MAX_NUMBER_CHARS_UMTS
#elif defined(CM_MAX_NUMBER_CHARS)
  # define LOCAL_CM_MAX_NUMBER_CHARS CM_MAX_NUMBER_CHARS
#endif
#ifdef FEATURE_DUAL_SIM
  # include "sys.h"
  # include "log_codes_gsm_v.h"
#endif /* FEATURE_DUAL_SIM */

#include "geran_multi_sim.h"
#include "geran_dual_sim_g.h"

static cm_call_id_type call_id = CM_CALL_ID_INVALID;

/*===========================================================================
Function: gsmdiag_set_imei
Description:
       This function updates the cached IMEI.
============================================================================*/
static byte gsmdiag_cached_imei[5] = {0,0,0,0,0};
void gsmdiag_set_imei(uint8 *plmn, uint16 lac)
{
  memscpy(gsmdiag_cached_imei, sizeof(gsmdiag_cached_imei), plmn, 3);
  gsmdiag_cached_imei[3] = (byte)( (lac & 0xFF00) >> 8 );
  gsmdiag_cached_imei[4] = (byte)(lac & 0x00FF);
}

/*===========================================================================
Function: gsmdiag_set_imsi
Description:

       This function propagates an IMSI to this module.

============================================================================*/
static uint8 gsmdiag_cached_imsi[DIAG_GSM_IMSI_LEN] = {0,0,0,0,0,0,0,0,0};
void gsmdiag_set_imsi(uint8 *imsi_ptr, uint8 imsi_len)
{
  if ( (imsi_ptr != NULL) && (imsi_len != 0) &&
       ( imsi_len <= sizeof(gsmdiag_cached_imsi) ) )
  {
    memscpy(gsmdiag_cached_imsi, sizeof(gsmdiag_cached_imsi),
            imsi_ptr, imsi_len);
  }

  return;
}

/*===========================================================================
Function: gsmdiag_set_lai
Description:
       This function updates the cached LAI.
============================================================================*/
#define _LAI_SIZE  (5)
static byte gsmdiag_cached_lai[_LAI_SIZE] = {0,0,0,0,0};
void gsmdiag_set_lai( uint8 *plmn, uint16 lac )
{
    memscpy(gsmdiag_cached_lai, _LAI_SIZE, plmn, 3);
    gsmdiag_cached_lai[3] = (byte)((lac & 0xFF00) >> 8);
    gsmdiag_cached_lai[4] = (byte)( lac & 0x00FF);
}

/*===========================================================================
Function: gsmdiag_get_lai
Description:
       This function returns a copy of the cached LAI (for GSM logging).
============================================================================*/
void gsmdiag_get_lai( uint8 *lai )
{
    memscpy(lai, _LAI_SIZE, gsmdiag_cached_lai, _LAI_SIZE);
}

/*===========================================================================
Function: gsmdiag_set_cell_id
Description:
       This function updates the cached CELL_ID.
============================================================================*/
static uint16 gsmdiag_cached_cell_id = 0;
void gsmdiag_set_cell_id( uint16  cell_id )
{
    gsmdiag_cached_cell_id = cell_id;
}

/*===========================================================================
Function: gsmdiag_get_cell_id
Description:
       This function returns a copy of the cached CELL_ID (for GSM logging).
============================================================================*/
uint16 gsmdiag_get_cell_id( void )
{
    return gsmdiag_cached_cell_id;
}

/*===========================================================================
Function: gsmdiag_set_geran_state
Description:
	  This function updates the cached GSM state for the specified subscription.
	  
    Diag is a transport mechanism and on receiving a command, will call the 
    handler registered for the same. The gsmdiag_state_retrieval() is the 
    handler which is registered with diag to be called whenever it receives 
    the GSM polling command.
	  In the current scenario, this command is being sent as part of QPST protocol
    as a polling command and also during QCN restore to know the state of the phone. 
    There may be other cases where this command is sent.

	  In one case, QCN restore is dependent on the phone state being received 
    in the GSM polling command response which is expected to be OFFLINE.

============================================================================*/
static gsm_state_t gsmdiag_cached_gsm_state[NUM_GERAN_DATA_SPACES] = {
  INITIAL_VALUE(GSM_STATE_OFFLINE) /* Default to offline */
};

void gsmdiag_set_geran_state(const gsm_state_t gsm_state, const gas_id_t gas_id)
{
  gsmdiag_cached_gsm_state[check_gas_id(gas_id)] = gsm_state;
}

/*===========================================================================
Function: gsmdiag_get_geran_state
Description:
       Return GSM_STATE_ONLINE if any sub is active.
============================================================================*/
static gsm_state_t gsmdiag_get_geran_state(void)
{
#if defined(FEATURE_GSM_PLT) || defined(FEATURE_WCDMA_PLT)
  #error code not present
#else
  int sub;

  for (sub = GERAN_DATA_SPACE_INDEX_1; sub < NUM_GERAN_DATA_SPACES; sub++)
  {
    if (gsmdiag_cached_gsm_state[sub] != GSM_STATE_OFFLINE)
    {
      return GSM_STATE_ONLINE;
    }
  }
  return GSM_STATE_OFFLINE;
#endif
}

/*===========================================================================
Function: gsmdiag_log_alloc
Description:
       Allocates a log packet pointer (and makes any necessary adjustments)
============================================================================*/
PACK(void *) gsmdiag_log_alloc(gsmdiag_pkt_info_t *info_ptr)
{
  PACK(void *) result_ptr;

  if (info_ptr != NULL)
  {
#ifdef FEATURE_DUAL_SIM
    info_ptr->orig_ptr = NULL;
    if (info_ptr->dual_sim_active == TRUE)
    {
      PACK(byte *) header_ptr;

      /* allocate one more byte than required, using an offset packet ID */
      header_ptr = (byte *)log_alloc(info_ptr->pkt_id+LOG_GSM_DUAL_SIM_CODE_OFFSET,
                                           info_ptr->pkt_size+1);
      if (header_ptr != NULL)
      {
        PACK(byte *) payload_ptr;

        /* establish a pointer beyond the log packet header, and store */
        /* the packet version as the high nybble, and the subscription */
        /* ID as the low nybble in the first octet of the payload data */
        payload_ptr  = header_ptr + sizeof(log_hdr_type);
        *payload_ptr = (info_ptr->pkt_version_id << 4) | (info_ptr->active_sub_id & 0x0F);

        /* adjust the output pointer to avoid overwriting this byte later */
        info_ptr->orig_ptr = (void *)header_ptr;
        payload_ptr = header_ptr + 1;
        info_ptr->pkt_ptr  = (void *)payload_ptr;
      }
      else
      {
        /* fall back to single SIM log packet allocation */
        info_ptr->pkt_ptr  = (void *)log_alloc(info_ptr->pkt_id, info_ptr->pkt_size);
      }
    }
    else /* use single SIM logging when Dual SIM is not active */
#endif /* FEATURE_DUAL_SIM */
    {
      info_ptr->pkt_ptr = (void *)log_alloc(info_ptr->pkt_id, info_ptr->pkt_size);
    }

    /* return the pointer that the caller can use to output their payload */
    result_ptr = (void *)(info_ptr->pkt_ptr);
  }
  else
  {
    result_ptr = NULL;
    MSG_GERAN_ERROR_0("Unable to alloc log packet from NULL ptr");
  }

  return(result_ptr);
}

/*===========================================================================
Function: gsmdiag_log_commit
Description:
       Commits a log packet pointer to the diag module
============================================================================*/
void gsmdiag_log_commit(gsmdiag_pkt_info_t *info_ptr)
{
  if (info_ptr != NULL)
  {
    if (info_ptr->pkt_ptr != NULL)
    {
      PACK(void *) commit_ptr;

#ifdef FEATURE_DUAL_SIM
      /* check whether this info structure indicates an offset packet */
      if (info_ptr->orig_ptr != NULL)
      {
        /* commit the original pointer, not the offset one */
        commit_ptr = (void *)(info_ptr->orig_ptr);
      }
      else
#endif /* FEATURE_DUAL_SIM */
      {
        commit_ptr = (void *)(info_ptr->pkt_ptr);
      }

      log_commit(commit_ptr);
    }
    else
    {
      MSG_GERAN_ERROR_0("GSM DIAG packet info not initialised correctly");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("Unable to commit log packet from NULL ptr");
  }

  return;
}

/*===========================================================================
Function: diag_gsm_verno
Description:

       This function processes the gsm specific version request packet
       from QXDM and sends the response back.

============================================================================*/
PACKED void * PACKED_POST gsmdiag_verno(
    PACKED void* PACKED_POST req_pkt,/* pointer to valid request packet */
    uint16 pkt_len       /* length of the request packet */
    )
{
  DIAG_SUBSYS_GSM_GSM_DIAG_VERNO_F_rsp_type *rsp_ptr ;

  NOTUSED(req_pkt);
  NOTUSED(pkt_len);

  /* Allocate the memory for this */
  rsp_ptr =
    (DIAG_SUBSYS_GSM_GSM_DIAG_VERNO_F_rsp_type *)diagpkt_subsys_alloc(
       (diagpkt_subsys_id_type)DIAG_SUBSYS_GSM,
                                  GSM_DIAG_VERNO_F,
       sizeof(DIAG_SUBSYS_GSM_GSM_DIAG_VERNO_F_rsp_type)
    );
  if (rsp_ptr) /* fill out the fields */
  { /* VDSP version comes from qdspext.h */
#ifdef QDSP_AMR_VERSION
    rsp_ptr->vdsp_ver     = QDSP_AMR_VERSION;
#else /* Not defined for this version of DSP -- use dummy value */
    rsp_ptr->vdsp_ver     = 0xDEAD;
#endif /* QDSP_AMR_VERSION */

    /* MDSP versions will come from mdspext.h - CR#20762*/
#if ( defined(MDSP_GSM_RAM_VERSION) && defined(MDSP_GSM_ROM_VERSION) )
    rsp_ptr->mdsp_ver_ram = MDSP_GSM_RAM_VERSION;
    rsp_ptr->mdsp_ver_rom = MDSP_GSM_ROM_VERSION;
#else /* Not defined for this version of DSP -- use dummy values */
    rsp_ptr->mdsp_ver_ram = 0xDEAD;
    rsp_ptr->mdsp_ver_rom = 0xDEAD;
#endif /* MDSP_GSM_RAM_VERSION */

  }
  return rsp_ptr ;

}

/*===========================================================================
Function: gsmdiag_status
Description:

       This function processes the gsm specific status request packet
       from QXDM and sends the response back.

============================================================================*/
PACKED void * PACKED_POST gsmdiag_status(
    PACKED void* PACKED_POST req_pkt,/* pointer to valid request packet */
    uint16 pkt_len       /* length of the request packet */
    )
{
  DIAG_SUBSYS_GSM_GSM_STATUS_F_rsp_type *rsp_ptr=NULL;

  NOTUSED(req_pkt);
  NOTUSED(pkt_len);

  /* Allocate the memory for this */
  rsp_ptr =
    (DIAG_SUBSYS_GSM_GSM_STATUS_F_rsp_type *)diagpkt_subsys_alloc(
       (diagpkt_subsys_id_type)DIAG_SUBSYS_GSM,
                                  GSM_STATUS_F,
       sizeof(DIAG_SUBSYS_GSM_GSM_STATUS_F_rsp_type)
    );

  if (rsp_ptr) /* fill out the fields */
  {
#ifndef FEATURE_GSM_PLT

# if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
    (void)ghdi_ds_read_imei((byte *)rsp_ptr->imei, SYS_MODEM_AS_ID_1);
# else
    (void)ghdi_read_imei((byte *)rsp_ptr->imei);
# endif /* FEATURE_DUAL_SIM && FEATURE_DUAL_IMEI */

    /* Use the current cached IMSI within this module */
    memscpy( (byte *)rsp_ptr->imsi, sizeof(rsp_ptr->imsi),
             gsmdiag_cached_imsi, sizeof(gsmdiag_cached_imsi) );

    rr_diag_get_lai(rsp_ptr->lai);
    rr_diag_get_cell_id(&rsp_ptr->cell_id);

#else  /*FEATURE_GSM_PLT*/
#error code not present
#endif /*FEATURE_GSM_PLT*/

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

  return rsp_ptr ;

}

/*===========================================================================
Function: gsmdiag_tmc_state_retrieval
Description:

      This function processes the gsm specific tmc_state_retrieval request packet
      from QXDM and sends the response back.
      Note that "TMC" is now a misnomer as RCInit has replaced TMC but
      this function is necessary because in T+G builds neither T nor G
      is registering for polling, so it ends up looking to QPST like there
      is no modem.

============================================================================*/
PACKED void * PACKED_POST gsmdiag_state_retrieval(
    PACKED void* PACKED_POST req_pkt,/* pointer to valid request packet */
    uint16 pkt_len       /* length of the request packet */
    )
{
    DIAG_SUBSYS_GSM_GSM_STATE_RETRIEVAL_F_rsp_type *rsp_ptr ;

  NOTUSED(req_pkt);
  NOTUSED(pkt_len);

    /* Allocate the memory for this */
  rsp_ptr =
    (DIAG_SUBSYS_GSM_GSM_STATE_RETRIEVAL_F_rsp_type *)diagpkt_subsys_alloc(
       (diagpkt_subsys_id_type)DIAG_SUBSYS_GSM,
                                    GSM_STATE_RETRIEVAL_F,
       sizeof(DIAG_SUBSYS_GSM_GSM_STATE_RETRIEVAL_F_rsp_type)
     );
    if(rsp_ptr)
    {
      /* Fill the response packet */
    rsp_ptr->ran_state = gsmdiag_get_geran_state();
    }

    return rsp_ptr ;
}

/*===========================================================================
Function: gsmdiag_call_orig
Description:

       This function processes the gsm specific call orig request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void * PACKED_POST gsmdiag_call_orig (PACKED void* PACKED_POST req_pkt,
                                             uint16 pkt_len)
{
  DIAG_SUBSYS_GSM_GSM_DIAG_CALL_ORIG_F_req_type *req_ptr =
    (DIAG_SUBSYS_GSM_GSM_DIAG_CALL_ORIG_F_req_type *)req_pkt;
  DIAG_SUBSYS_GSM_GSM_DIAG_CALL_ORIG_F_rsp_type *rsp_ptr;

  cm_num_s_type     numbers_buf;
  uint8       work_buf[LOCAL_CM_MAX_NUMBER_CHARS];
  uint8       i;
  cm_gw_cs_orig_params_s_type   call_orig_params;
  cm_num_s_type     calling_num;
  cm_orig_alpha_s_type    alpha_buf;

  const int rsp_len = sizeof(DIAG_SUBSYS_GSM_GSM_DIAG_CALL_ORIG_F_rsp_type);

  /* Check that req_ptr->num_digits exists */
  if (pkt_len < FPOS(DIAG_SUBSYS_GSM_GSM_DIAG_CALL_ORIG_F_req_type, num_digits))
  {
    MSG_GERAN_ERROR_0("");
    return (void *) diagpkt_err_rsp( DIAG_BAD_LEN_F, req_pkt, pkt_len );
  }

  if (req_ptr == NULL)
  {
    return NULL;
  }

  /* Check against the maximum number of digits in the received packet to avoid overflowing
     (i.e. pkt_len - the length of the non-digits fields of the packet) */
  if ( req_ptr->num_digits >
       ( pkt_len - ( sizeof(DIAG_SUBSYS_GSM_GSM_DIAG_CALL_ORIG_F_req_type)
                                        - sizeof(((DIAG_SUBSYS_GSM_GSM_DIAG_CALL_ORIG_F_req_type *)0)->digits))))
  {
    MSG_GERAN_ERROR_0("");
    return (void *) diagpkt_err_rsp( DIAG_BAD_LEN_F, req_pkt, pkt_len );
  }

  /* Allocate the memory for this */
  rsp_ptr =
    (DIAG_SUBSYS_GSM_GSM_DIAG_CALL_ORIG_F_rsp_type *)diagpkt_subsys_alloc(
       (diagpkt_subsys_id_type)DIAG_SUBSYS_GSM,
       GSM_DIAG_CALL_ORIG_F,
       rsp_len
    );

  if (rsp_ptr == NULL)
  {
    return NULL;
  }

  /* Initialize the buffer to CM */
  memset(&call_orig_params,
        CM_CALL_CMD_PARAM_DEFAULT_VALUE,
        sizeof(call_orig_params));

  if ( (req_ptr->num_digits > DIAG_MAX_GSM_DIGITS)
       || (req_ptr->num_digits > LOCAL_CM_MAX_NUMBER_CHARS) )
  {
    MSG_GERAN_ERROR_0("Dialed too many digits");
    return rsp_ptr;
  }

  // initialize work_buf
  memset (work_buf, 0, ARR_SIZE(work_buf));


  for (i=0; i<req_ptr->num_digits; i++) {
      /* Check for dial digits (0 - 9, *, #) */
      if ( ((req_ptr->digits[i] < '0') || (req_ptr->digits[i] > '9')) &&
           (req_ptr->digits[i] != '*')  &&
         (req_ptr->digits[i] != '#') )
    {
           /* Was an invalid digit (not 0 - 9, *, #) */
           MSG_GERAN_ERROR_1("Bad dial digit: %c", req_ptr->digits[i]);
           return rsp_ptr;
      }
      work_buf[i] = req_ptr->digits[i];
  }

  alpha_buf.len = (uint8)strlen("DIAG CALL");
  (void) memscpy(alpha_buf.buf,
                 sizeof(alpha_buf.buf),
                 (const byte *)"DIAG CALL",
                 alpha_buf.len);
  cm_num_init( &calling_num );
  cm_num_fill( &numbers_buf, work_buf, req_ptr->num_digits, CM_DIGIT_MODE_4BIT_DTMF);

  (void) cm_mm_call_cmd_orig(
      NULL, /* client callback function */
      NULL, /* pointer to client callback data block */
      CM_CLIENT_ID_ANONYMOUS, /* client id */
      CM_CALL_TYPE_VOICE, /* call type */
      CM_SRV_TYPE_AUTOMATIC,  /* service type */
      &calling_num,  /* service option */
      &numbers_buf,
      &alpha_buf,        /* Alpha buffer for call  */
      NULL,              /* CDMA call orig params  */
      &call_orig_params, /* GW CS call orig params */
      NULL,              /* GW PS call orig params */
      &call_id
      );


  return rsp_ptr ;

}

/*===========================================================================
Function: gsmdiag_call_end
Description:

       This function processes the gsm specific call end request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void * PACKED_POST gsmdiag_call_end(
    PACKED void* PACKED_POST req_pkt,/* pointer to valid request packet */
    uint16 pkt_len       /* length of the request packet */
    )
{
  DIAG_SUBSYS_GSM_GSM_DIAG_CALL_END_F_rsp_type *rsp_ptr ;

  cm_end_params_s_type end_params[1];

  const int rsp_len = sizeof(DIAG_SUBSYS_GSM_GSM_DIAG_CALL_END_F_rsp_type);

  NOTUSED(req_pkt);
  NOTUSED(pkt_len);

  /* Allocate the memory for this */
  rsp_ptr =
    (DIAG_SUBSYS_GSM_GSM_DIAG_CALL_END_F_rsp_type *)diagpkt_subsys_alloc(
       (diagpkt_subsys_id_type)DIAG_SUBSYS_GSM,
       GSM_DIAG_CALL_END_F,
       rsp_len
    );

  memset( end_params,
          CM_CALL_CMD_PARAM_DEFAULT_VALUE,
          sizeof ( cm_end_params_s_type));

  end_params[0].call_id   = call_id;
  end_params[0].info_type = CM_CALL_MODE_INFO_GW_CS;

  (void) cm_mm_call_cmd_end(
      NULL, /* client callback function */
      NULL, /* pointer to client callback data block */
      CM_CLIENT_ID_ANONYMOUS,
      1,
      &end_params[0]
      );
  return rsp_ptr ;

}


/*===========================================================================
There is new code in here to process the GPRS-specific log-packet-request
command. This works like an extended log-on-demand which allows additional
parameters to be included with the log code.
It is handled by means of a registered interface which is similar to the
interface for log-on-demand.
============================================================================*/

/* Define an entry in the dispatch table... */
typedef struct _gprs_log_packet_request_desc
{
    uint16                       log_code;
    gprs_log_packet_request_func handler;
} gprs_log_packet_request_desc;

/* ...and the dispatch table itself... */
gprs_log_packet_request_desc gprs_log_packet_request_dispatch_table[] =
{
    { 0x0000, NULL },   /* 00 */
    { 0x0000, NULL },   /* 01 */
    { 0x0000, NULL },   /* 02 */
    { 0x0000, NULL },   /* 03 */
    { 0x0000, NULL },   /* 04 */
    { 0x0000, NULL },   /* 05 */
    { 0x0000, NULL },   /* 06 */
    { 0x0000, NULL },   /* 07 */
    { 0x0000, NULL },   /* 08 */
    { 0x0000, NULL },   /* 09 */
    { 0x0000, NULL },   /* 10 */
    { 0x0000, NULL },   /* 11 */
    { 0x0000, NULL },   /* 12 */
    { 0x0000, NULL },   /* 13 */
    { 0x0000, NULL },   /* 14 */
    { 0x0000, NULL },   /* 15 */
    { 0x0000, NULL },   /* 16 */
    { 0x0000, NULL },   /* 17 */
    { 0x0000, NULL },   /* 18 */
    { 0x0000, NULL },   /* 19 */
};

/* ...which can hold this many entries */
#define LOG_PACKET_REQ_DISPATCH_TABLE_SIZE ( sizeof(gprs_log_packet_request_dispatch_table) /\
                                             sizeof(gprs_log_packet_request_dispatch_table[0]) )


/*===========================================================================
Function: diagpkt_gsm_gprs_lpr_register
Description:

       This function registers a handler for the GPRS_LOG_PACKET_REQUEST
       subcommand, for a specific log packet type.

       The function returns TRUE if the handler is successfully registered,
       or FALSE otherwise - which can only be because the table is full.

============================================================================*/

boolean diagpkt_gsm_gprs_lpr_register(
    uint16                       log_code,    /* the log code to be handled */
    gprs_log_packet_request_func handler      /* the function to handle it  */
    )
{
    unsigned int entry;

    /* First check the entire table for an existing handler for this log code */
    for (entry = 0; entry < LOG_PACKET_REQ_DISPATCH_TABLE_SIZE; ++entry)
    {
    gprs_log_packet_request_desc *desc_ptr =
      &gprs_log_packet_request_dispatch_table[entry];

        if (desc_ptr->log_code == log_code)
        {
            if (desc_ptr->handler == handler)
            {
                MSG_GERAN_LOW_1( "lpr_reg> code %x already has same handler", log_code);
            }
            else
            {
                if (handler == NULL)
                {
                    MSG_GERAN_HIGH_1( "lpr_reg> code %x delete existing handler", log_code);
                    desc_ptr->log_code = 0;
                    desc_ptr->handler  = NULL;
                }
                else
                {
                    MSG_GERAN_HIGH_1( "lpr_reg> code %x overwrite existing handler", log_code);
                    desc_ptr->handler = handler;
                }
            }

            return(TRUE);
        }
    }

    /* No existing handler found. Look for a vacant slot for install one */
    for (entry = 0; entry < LOG_PACKET_REQ_DISPATCH_TABLE_SIZE; ++entry)
    {
    gprs_log_packet_request_desc *desc_ptr =
      &gprs_log_packet_request_dispatch_table[entry];

        if (desc_ptr->log_code == 0x0000)
        {
            desc_ptr->log_code = log_code;
            desc_ptr->handler  = handler;
            return(TRUE);
        }
    }

    MSG_GERAN_HIGH_1( "lpr_reg> code %x table full!!", log_code);
    return(FALSE);
}


/*===========================================================================
Function: diagpkt_gsm_gprs_log_packet_request
Description:

       This function processes the gprs specific log-packet-request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void * PACKED_POST diagpkt_gsm_gprs_log_packet_request(
    PACKED void* PACKED_POST req_pkt,   /* pointer to valid request packet */
    uint16 pkt_len          /* length of the request packet */
    )
{
  DIAG_SUBSYS_GSM_GSM_GPRS_LOG_PACKET_REQ_F_req_type *req_ptr =
    (DIAG_SUBSYS_GSM_GSM_GPRS_LOG_PACKET_REQ_F_req_type *)req_pkt;
    DIAG_SUBSYS_GSM_GSM_GPRS_LOG_PACKET_REQ_F_rsp_type *rsp_ptr;
    gprs_log_packet_request_func                        handler = NULL;
    unsigned int                                        entry;

    if (pkt_len < sizeof(DIAG_SUBSYS_GSM_GSM_GPRS_LOG_PACKET_REQ_F_req_type))
    {
      MSG_GERAN_HIGH_0("diagpkt too short");
      return (void *) diagpkt_err_rsp( DIAG_BAD_LEN_F, req_pkt, pkt_len );
    }

    if (req_ptr == NULL)
    {
      return (NULL);
    }

    MSG_GERAN_LOW_1( "GPRS LPR> log_packet_code = %d", req_ptr->log_packet_code);
    MSG_GERAN_LOW_1( "GPRS LPR> zero_stats      = %d", req_ptr->zero_stats);
    MSG_GERAN_LOW_1( "GPRS LPR> additional_info = %d", req_ptr->additional_info);

    /* Look for a registered handler for log packets of this type */
    for (entry = 0; entry < LOG_PACKET_REQ_DISPATCH_TABLE_SIZE; ++entry)
    {
    gprs_log_packet_request_desc *desc_ptr =
      &gprs_log_packet_request_dispatch_table[entry];

        if (desc_ptr->log_code == req_ptr->log_packet_code)
        {
            MSG_GERAN_LOW_2( "GPRS LPR> code %x handler offset %d",
                     req_ptr->log_packet_code, entry);

            handler = desc_ptr->handler;
            break;
        }
    }

    /* Abandon if no handler found */
    if (handler == NULL)
    {
        MSG_GERAN_HIGH_1( "GPRS LPR> code %x no handler found",
                  req_ptr->log_packet_code);
        return(NULL);
    }

    /* Allocate the memory for the response */
    rsp_ptr =
      (DIAG_SUBSYS_GSM_GSM_GPRS_LOG_PACKET_REQ_F_rsp_type *)diagpkt_subsys_alloc(
        (diagpkt_subsys_id_type)DIAG_SUBSYS_GSM,
        GSM_GPRS_LOG_PACKET_REQ_F,
        sizeof(DIAG_SUBSYS_GSM_GSM_GPRS_LOG_PACKET_REQ_F_rsp_type)
      );
    if (rsp_ptr == NULL)
    {
        MSG_GERAN_HIGH_0( "GPRS LPR> diagpkt_subsys_alloc failed");
    }
    else
    {
        MSG_GERAN_LOW_0( "GPRS LPR> invoking handler");

        (*handler)(req_ptr->log_packet_code,
                   req_ptr->zero_stats,
                   req_ptr->additional_info);

        MSG_GERAN_LOW_0( "GPRS LPR> finished");
    }

    return(rsp_ptr);
}

/****************************************************************************
 *   Function name:  geran_diag_event
 *   --------------------------
 *
 *   Description: This is a generic function for GERAN use that logs either
 *   single sim event or multi sim event based on multi sim mode of the UE.
 *   The first byte of the payload shall be the sub id irrespective of the
 *   multisim mode.
 *
 *   Parameters:
 *   -----------
 *   single_sim_event - Event id of the single sim version of the event. EVENT_MAX_ID if NA.
 *   multi_sim_event  - Event id of the multi sim version of the event. EVENT_MAX_ID if NA.
 *   payloadsize      - Size of the payload
 *   payload          - Actual payload for the event. The first byte shall
 *   always contain the sub id.
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/
void geran_diag_event(event_id_enum_type single_sim_event, event_id_enum_type multi_sim_event, size_t payload_size, byte *payload_ptr)
{
  if (SYS_MODEM_DEVICE_MODE_SINGLE_SIM == geran_get_multi_sim_mode())
  {
    /* Safe check: In single sim config, single_sim_event parameter cannot hold the invalid value EVENT_MAX_ID */
    if(EVENT_MAX_ID == single_sim_event)
    {
      MSG_GERAN_HIGH_2("Invalid single sim event id logged: SS:0x%x DS:0x%x", single_sim_event, multi_sim_event);
    }
    else
    {
      /* For single sim event, calculate payload size leaving the sub id byte */
      if ((payload_size-1 != 0) && (payload_ptr != NULL))
      {
        byte *offset_ptr = payload_ptr + 1; /* skip over the subscription ID */

        event_report_payload(single_sim_event, payload_size-1, (void *)offset_ptr);
      }
      else  /* There is no payload for the event */
      {
        event_report(single_sim_event);
      }
    }
  }
  else
  {
    /* Safe check: In multi sim config, multi_sim_event parameter cannot hold the invalid value EVENT_MAX_ID */
    if(EVENT_MAX_ID == multi_sim_event)
    {
      MSG_GERAN_HIGH_2("Invalid multisim event id logged: SS:0x%x DS:0x%x", single_sim_event, multi_sim_event);
    }
    else if ((payload_size !=0) && (payload_ptr != NULL))
    {
      event_report_payload(multi_sim_event, payload_size, (void *)payload_ptr);
    }
  }

  return;
}

#ifdef THIS_FUNCTION_HAS_NOT_BEEN_USED
/****************************************************************************
 *   Function name:  geran_diag_event_ms
 *   -----------------------------------
 *
 *   Description: This is a generic function for GERAN use that logs either
 *   single sim event or multi sim event based on multi sim mode of the UE.
*
 *   Parameters:
 *   -----------
 *   single_sim_event - Event id of the single sim version of the event. EVENT_MAX_ID if not defined.
 *   payloadsize      - Size of the payload
 *   payload          - Actual payload for the event.
 *   gas_id
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/

/* Table to translate single SIM event into multi SIM event */
const struct {event_id_enum_type sse; event_id_enum_type mse;} ss_event_to_ms_event[] =
{
  {EVENT_GSM_LINK_FAILURE,      EVENTS_DS_GSM_LINK_FAILURE},
  {EVENT_RANDOM_ACCESS_REQUEST, EVENTS_DS_RANDOM_ACCESS_REQUEST},
  // etc.
};

void geran_diag_event_ms(
  const event_id_enum_type single_sim_event,
  const size_t payload_size,
  const byte *payload_ptr,
  const gas_id_t gas_id)
{
  /* Safe check: single_sim_event parameter cannot hold the invalid value EVENT_MAX_ID */
  if(EVENT_MAX_ID == single_sim_event)
  {
    MSG_GERAN_HIGH_1("Invalid single sim event id logged: SS:0x%x", single_sim_event);
    return;
  }
  if (SYS_MODEM_DEVICE_MODE_SINGLE_SIM == geran_get_multi_sim_mode())
  {
    if ((payload_size != 0) && (payload_ptr != NULL))
    {
      event_report_payload(single_sim_event, payload_size, (void *)payload_ptr);
    }
    else  /* There is no payload for the event */
    {
      event_report(single_sim_event);
    }
  }
  else /* MULTI_SIM */
  {
    if ((payload_size !=0) && (payload_ptr != NULL))
    {
      event_id_enum_type multi_sim_event = EVENT_MAX_ID;
      int i;

      for (i = 0; i < ARR_SIZE(ss_event_to_ms_event); i++)
      {
        if (ss_event_to_ms_event[i].sse == single_sim_event)
        {
          multi_sim_event = ss_event_to_ms_event[i].mse;
          break;
        }


        if(EVENT_MAX_ID == multi_sim_event)
        {
          MSG_GERAN_HIGH("Invalid multisim event id logged: SS:0x%x", single_sim_event);
        }
        else
        {
          byte  payload[payload_size+1];

          payload[0] = geran_get_diag_sub_id(gas_id);
          memscpy(&payload[1], payload_size, payload_ptr, payload_size);
          event_report_payload(multi_sim_event, payload_size+1, (void *)payload);
        }
      }
    }
  }
}
#endif

/**************************************************************************
** GSM SUBSYSTEM COMMAND DISPATCH TABLE.
Note:  Any new commands for GSM need to be added here.
****************************************************************************/

static const diagpkt_user_table_entry_type gsmdiag_tbl[] =
{
  {GSM_DIAG_VERNO_F, GSM_DIAG_VERNO_F, gsmdiag_verno},
  {GSM_STATUS_F, GSM_STATUS_F, gsmdiag_status},
  {GSM_STATE_RETRIEVAL_F, GSM_STATE_RETRIEVAL_F, gsmdiag_state_retrieval},
  {GSM_DIAG_CALL_ORIG_F, GSM_DIAG_CALL_ORIG_F, gsmdiag_call_orig},
  {GSM_DIAG_CALL_END_F, GSM_DIAG_CALL_END_F, gsmdiag_call_end},
  {GSM_GPRS_LOG_PACKET_REQ_F, GSM_GPRS_LOG_PACKET_REQ_F, diagpkt_gsm_gprs_log_packet_request}
};

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_GSM, gsmdiag_tbl);

#else

  void gsmdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER ( (word)DIAG_SUBSYS_GSM, gsmdiag_tbl );
  }

#endif

#if defined __cplusplus
  }
#endif

