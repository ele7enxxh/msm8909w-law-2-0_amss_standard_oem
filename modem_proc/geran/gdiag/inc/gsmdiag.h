#ifndef GSMDIAG_H
#define GSMDIAG_H
/*==========================================================================

      Diagnostic Services Packet Processing Command Code Definitions

Description
  This file contains function prototypes and packet ID definitions for the
  diagnostic interface.

Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdiag/inc/gsmdiag.h#1 $

when       who     what, where, why
--------   ---    -----------------------------------------------------------
14-01-14   tjw    Re-unified
09-02-19   tjw    Automatically split from original header file and then tidied.
===========================================================================*/
#ifndef CUSTOMER_H
  # include "customer.h"
#endif
#ifndef COMDEF_H
  # include "comdef.h"
#endif
#include "geran_variation.h"

#include "diagpkt.h"
#include "log.h"
#include "event.h"  /* For GERAN event types */
#include "geran_dual_sim.h"

/* Spec limits downlink tr block to 800 bytes so 2*800 + 20 should suffice */
#define MAX_DATA_LEN  200 /*For now */
/*#define MAX_DATA_LEN  1620 */

/* Max num of digits in dialed number for GSM call origination packet */
#define DIAG_MAX_GSM_DIGITS     32

/* Max num of bytes for largest SIM item */
#define DIAG_MAX_SIM_ITEM_SIZE  40

/* Max num of bytes for largest SIM item */
#define DIAG_MAX_PIN_SIZE       8

#define DIAG_GSM_IMSI_LEN       9

/* This Table is from GSM ICD section 3.2 */
/* NOTE: Values may shift as this document is still under construction */
#define GSM_DIAG_VERNO_F           0
#define GSM_STATUS_F               1
#define GSM_STATE_RETRIEVAL_F      2
#define GSM_DIAG_CALL_ORIG_F       3
#define GSM_DIAG_CALL_END_F        4
#define GSM_GPRS_LOG_PACKET_REQ_F  5
#define DIAG_GSM_MAX_F             6 /* Must be one greater than highest cmd */
#ifdef FEATURE_GL1_GPLT
# define GSM_PLT_DIAG_CMDS          28
#endif
typedef struct
{
  log_code_type  pkt_id;          /* INPUT  */
  unsigned int   pkt_size;        /* INPUT  */
  void          *pkt_ptr;         /* OUTPUT */
#ifdef FEATURE_DUAL_SIM
  boolean        dual_sim_active; /* INPUT  */
  byte           active_sub_id;   /* INPUT  */
  byte           pkt_version_id;  /* INPUT  */
  void          *orig_ptr;        /* OUTPUT */
#endif /* FEATURE_DUAL_SIM */
} gsmdiag_pkt_info_t;

/* definitions for packet version half-octet */
#define GSMDIAG_PKT_VERSION_0       0x00
#define GSMDIAG_PKT_VERSION_1       0x01
#define GSMDIAG_PKT_VERSION_2       0x02
#define GSMDIAG_PKT_VERSION_3       0x03
#define GSMDIAG_PKT_VERSION_4       0x04
#define GSMDIAG_PKT_VERSION_DEFAULT (GSMDIAG_PKT_VERSION_0)

/* GSM states defined in Serial Interface Control Document for W-CDMA
   (these were GSM_TMC_STATE_OFFLINE etc.
   "TMC" is now a misnomer as RCInit has replaced TMC.)
*/
typedef enum {
  GSM_STATE_OFFLINE = 0,
  GSM_STATE_ONLINE  = 1,
  GSM_STATE_UNKNOWN = 99
} gsm_state_t;


/**************************************************************************
    FUNCTION PROTOTYPES
**************************************************************************/
extern PACK(void *) gsmdiag_log_alloc(gsmdiag_pkt_info_t *info_ptr);
extern void         gsmdiag_log_commit(gsmdiag_pkt_info_t *info_ptr);
extern void         gsmdiag_set_imei(uint8 *plmn, uint16 lac);
extern void         gsmdiag_set_imsi(uint8 *imsi_ptr, uint8 imsi_len);
extern void         gsmdiag_set_lai(uint8 *plmn, uint16 lac);
/*===========================================================================
Function: gsmdiag_set_geran_state
Description:
       This function updates the cached GSM state for the specified subscription.
============================================================================*/
extern void         gsmdiag_set_geran_state(const gsm_state_t gsm_state,
                                            const gas_id_t gas_id);

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
 *   payload          - Actual payload for the event. The first byte shall always contain the sub id.
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/
extern void geran_diag_event(
  event_id_enum_type single_sim_event, 
  event_id_enum_type multi_sim_event, 
  size_t payload_size, 
  byte * payload_ptr
);

/**************************************************************************
    PACKET DEFINITIONS
**************************************************************************/
/*---------------------------------------------------------------------------
    GSM VERSION -- SUBCOMMAND 0
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(GSM, GSM_DIAG_VERNO_F)
 /* No additional fields */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(GSM, GSM_DIAG_VERNO_F)
  /* Vocoder DSP version num, 0 ...0xffff */
  uint16  vdsp_ver;

  /* MDSP version number(first part) 16 bits */
  uint16  mdsp_ver_rom;
  /* MDSP version number(second part) 16 bits */
  uint16  mdsp_ver_ram;

DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    GSM STATUS -- SUBCOMMAND 1
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(GSM, GSM_STATUS_F)
 /* No additional fields */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(GSM, GSM_STATUS_F)
 /* International Mobile Equipment ID */
 uint8 imei[9];
 /* International Mobile Subscriber ID */
 uint8 imsi[9];
 /* Location Area ID */
 uint8 lai[5];
 /* Cell ID */
 uint16 cell_id;
 /* Call Manager Overall Call State */
 uint8 call_state;
 /* Call Manager Operating Mode */
 uint8 operating_mode;
 /* Call Manager System Mode */
 uint8 system_mode;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    STATE Retrieval -- SUBCOMMAND 2
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(GSM, GSM_STATE_RETRIEVAL_F)
 /* No additional fields */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(GSM, GSM_STATE_RETRIEVAL_F)
 /* GERAN state (online/offline) */
 uint8 ran_state;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    GSM CALL ORIGINATE -- SUBCOMMAND 3
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(GSM, GSM_DIAG_CALL_ORIG_F)
 /* DM shall set it to the number of valid digits in DIGITS field */
 uint8  num_digits;
 /* Dialed digits; ASCII chars; 0-9, #, * only */
 uint8  digits[DIAG_MAX_GSM_DIGITS];
 /* Rate of the vocoder: 0 ~ 7 for
    4.75, 5.15, 5.90, 6.70, 7.40, 7.95, 10.2, 12.2 kbps
  */
 uint8  amr_rate;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(GSM, GSM_DIAG_CALL_ORIG_F)
 /* No additional fields */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    GSM CALL END -- SUBCOMMAND 4
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(GSM, GSM_DIAG_CALL_END_F)
 /* No additional fields */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(GSM, GSM_DIAG_CALL_END_F)
 /* No additional fields */
DIAGPKT_RSP_END



/*---------------------------------------------------------------------------
    GPRS LOG PACKET REQUEST -- SUBCOMMAND 5
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(GSM, GSM_GPRS_LOG_PACKET_REQ_F)
 /* The log packet code being requested */
 uint16 log_packet_code;
 /* Reset statistics for this packet: non-zero ==> yes */
 uint8  zero_stats;
 /* Additional information dependent on packet type */
 uint8  additional_info;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(GSM, GSM_GPRS_LOG_PACKET_REQ_F)
 /* No additional fields */
DIAGPKT_RSP_END


/**************************************************************************
    FUNCTION PROTOTYPES
**************************************************************************/

/* Define a GPRS_LOG_PACKET_REQUEST handler */
typedef void (*gprs_log_packet_request_func)(uint16 log_code,
                                             uint8  zero_stats,
                                             uint8  additional_info);

/* Register a GPRS_LOG_PACKET_REQUEST handler for a log packet type */
extern boolean diagpkt_gsm_gprs_lpr_register(
    uint16                       log_code,    /* the log code to be handled */
    gprs_log_packet_request_func handler      /* the function to handle it  */
    );

/*---------------------------------------------------------------------------
    LAI  -- Logging routines for Location Area Identifier
         -- Set takes in 3 bytes of plmn and 1 word lac (loc area code)
            and caches a copy
         -- Get will return cached copy of LAI (5 bytes <BA 3C 21 hh ll> fmt)
            were ABC is the Mobile Country Code
                 123 is the Mobile Network Code
               hh ll is the Location Area Code (hh is msb, ll is lsb)
---------------------------------------------------------------------------*/
extern void gsmdiag_set_lai( uint8 *plmn, uint16 lac );
extern void gsmdiag_get_lai( uint8 *lai );

/*---------------------------------------------------------------------------
    CELLID -- Logging routines for Cell Identifier
           -- Set takes in 1 word of data and caches a copy
           -- Get will return cached copy of Cell_ID (1 word)
---------------------------------------------------------------------------*/
extern void   gsmdiag_set_cell_id( uint16  cell_id );
extern uint16 gsmdiag_get_cell_id( void );

#endif /* GSMDIAG_H */
