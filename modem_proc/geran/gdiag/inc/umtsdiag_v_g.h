#ifndef DIAG_UMTS_CMD_V_G_H
#define DIAG_UMTS_CMD_V_G_H
/*==========================================================================

      Diagnostic Services Packet Processing Command Code Defintions

Description
  This file contains packet id definitions for the serial interface to
  the dmss. This file contains packets common to WCDMA and GSM modes (UMTS)

Copyright (c) 1993-2013 Qualcomm Technologies, Inc.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdiag/inc/umtsdiag_v_g.h#1 $

when       who     what, where, why
--------   ---    -----------------------------------------------------------
12/09/02   ATM    Move IMSI & IMEI logging to common location -- to be
                  triggered by MM
04/10/02   ATM    Added Front-End Filter to CFA
02/13/02   ATM    Updated command set to match UMTS ICD Revision A
12/14/01   ATM    Created the file adapted from diagpkt_wcdma_cmd.h
===========================================================================*/

#ifndef CUSTOMER_H
  # include "customer.h"
#endif
#ifndef COMDEF_H
  # include "comdef.h"
#endif

#include "diagpkt.h"

/* Max num of bytes for largest SIM item */
#define DIAG_MAX_SIM_ITEM_SIZE  40

/* Max num of bytes for largest SIM item */
#define DIAG_MAX_PIN_SIZE       8


/* This Table is from UMTS ICD (80-V4083-1 Rev A) section 3.1 */
#define UMTS_DIAG_VERNO_F      0
#define UMTS_SIM_READ_F        1
#define UMTS_SIM_WRITE_F       2
#define UMTS_SIM_STATUS_F      3
#define UMTS_VERIFY_PIN_F      4
#define UMTS_CHANGE_PIN_F      5
#define UMTS_DISABLE_PIN_F     6
#define UMTS_ENABLE_PIN_F      7
#define UMTS_UNLOCK_PIN_F      8
/*#define UMTS_CFA_CONFIG_F      9 moved to API file */
#define UMTS_CM_STATE_INFO_F   10
#define UMTS_AGC_INFO_F        11
#define DIAG_UMTS_MAX_F        12 /* Must be one greater than highest UMTS cmd */


/**************************************************************************
    PACKET DEFINITIONS
**************************************************************************/
/*---------------------------------------------------------------------------
    UMTS VERSION -- SUBCOMMAND 0
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_DIAG_VERNO_F)
 /* No additional fields */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_DIAG_VERNO_F)
  /* Rev number of 3GPP standard
     Month of release:  1 - Jan ... 12 - Dec
     Year of release:  99 - 1999
                       00 - 2000 ... 98 - 2098
  */
  uint8 g3gpp_rev_month;
  uint8 g3gpp_rev_year;
  /* Release num, from 3 up */
  uint8 g3gpp_rel;

  /* Classmark[2,3] info (see 24.008 sect10.5.1.5/6) - omit IEI*/
  uint8 class_mark2[4];
  uint8 class_mark3[13];

  /* DRX parameter (see 24.008 sect10.5.5.6) - omit IEI */
  uint8 drx_parameter[2];

DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    UMTS SIM READ    -- SUBCOMMAND 1
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_SIM_READ_F)
  /* SIM ITEM NUMBER - 16 bits */
  uint16  sim_item;
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_SIM_READ_F)
  /* SIM ITEM NUMBER - 16 bits */
  uint16  sim_item;
  /* SIM STATUS - 16 bits */
  uint16  sim_status;
  /* SIM ITEM DATA - variable */
  uint8  item_data[DIAG_MAX_SIM_ITEM_SIZE];
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    UMTS SIM WRITE   -- SUBCOMMAND 2
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_SIM_WRITE_F)
  /* SIM ITEM NUMBER - 16 bits */
  uint16  sim_item;
  /* SIM STATUS - 16 bits */
  uint16  sim_status;
  /* SIM ITEM DATA - variable */
  uint8  item_data[DIAG_MAX_SIM_ITEM_SIZE];
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_SIM_WRITE_F)
  /* SIM ITEM NUMBER - 16 bits */
  uint16  sim_item;
  /* SIM STATUS - 16 bits */
  uint16  sim_status;
  /* SIM ITEM DATA - variable */
  uint8  item_data[DIAG_MAX_SIM_ITEM_SIZE];
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    UMTS SIM STATUS  -- SUBCOMMAND 3
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_SIM_STATUS_F)
 /* No additional fields */
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_SIM_STATUS_F)
  /* SIM STATUS - 0=NO_SIM/1=UMTS_SIM/2=UMTS_SIM*/
  uint8  sim_status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    UMTS VERIFY PIN  -- SUBCOMMAND 4
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_VERIFY_PIN_F)
  /* PIN 1 or 2 */
  uint8  pin_num;
  /* 8-ascii PIN */
  uint8  pin_string[DIAG_MAX_PIN_SIZE];
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_VERIFY_PIN_F)
  /* PIN STATUS - 0=SUCCESS          */
  /*              1=FAILED_NO_SIM    */
  /*              2=FAILED_BAD_PIN   */
  /*              3=FAILED_NO_PIN    */
  /*              4=FAILED_WRONG_PIN */
  /*              5=FAILED_BLOCKED   */
  uint8  pin_status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    UMTS CHANGE PIN  -- SUBCOMMAND 5
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_CHANGE_PIN_F)
  /* PIN 1 or 2 */
  uint8  pin_num;
  /* 8-ascii original PIN */
  uint8  pin_old[DIAG_MAX_PIN_SIZE];
  /* 8-ascii new PIN */
  uint8  pin_new[DIAG_MAX_PIN_SIZE];
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_CHANGE_PIN_F)
  /* PIN STATUS - see description above */
  uint8  pin_status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    UMTS DISABLE PIN -- SUBCOMMAND 6
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_DISABLE_PIN_F)
  /* 8-ascii PIN */
  uint8  pin_string[DIAG_MAX_PIN_SIZE];
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_DISABLE_PIN_F)
  /* PIN STATUS - see description above */
  uint8  pin_status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    UMTS ENABLE PIN  -- SUBCOMMAND 7
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_ENABLE_PIN_F)
  /* 8-ascii PIN */
  uint8  pin_string[DIAG_MAX_PIN_SIZE];
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_ENABLE_PIN_F)
  /* PIN STATUS - see description above */
  uint8  pin_status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
     UMTS UNLOCK PIN -- SUBCOMMAND 8
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_UNBLOCK_PIN_F)
  /* PIN 1 or 2 */
  uint8  pin_num;
  /* 8-ascii original PIN */
  uint8  pin_old[DIAG_MAX_PIN_SIZE];
  /* 8-ascii new PIN */
  uint8  pin_new[DIAG_MAX_PIN_SIZE];
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_UNBLOCK_PIN_F)
  /* PIN STATUS - see description above */
  uint8  pin_status;
DIAGPKT_RSP_END


/*---------------------------------------------------------------------------
    UMTS CM STATE INFO -- SUBCOMMAND 10
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_CM_STATE_INFO_F)
 /* No additional fields */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_CM_STATE_INFO_F)
  uint8  call_state;
  uint8  operating_mode;
  uint8  system_mode;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    UMTS AGC INFO -- SUBCOMMAND 11
---------------------------------------------------------------------------*/
/* Request packet  -- modeled after DIAG_LOG_CONFIG_F */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_AGC_INFO_F)
  uint32 format;     /* AGC specific format       */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_AGC_INFO_F)
  uint32 format;     /* AGC specific format       */
  uint32 length;     /* data length of next field */
  uint8  data[1];    /* Variable length data */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    IMSI -- Logging routines for International Mobile Subscriber Identifier
         -- Set takes in 1 byte of length and 0-8 bytes of data
         -- Set will emit EVENT_IMSI (9 bytes) and store cached copy
         -- Get will return cached IMSI (9 bytes <length,data> format)
---------------------------------------------------------------------------*/
extern void umtsdiag_set_imsi(byte length, const byte *data);
extern void umtsdiag_get_imsi(byte *imsi);

/*---------------------------------------------------------------------------
    IMEI -- Logging routines for International Mobile Equipment Identifier
         -- Set takes in 9 bytes of data and will store cached copy
         -- Get will return cached IMEI (9 bytes <length,data> format)
---------------------------------------------------------------------------*/
extern void umtsdiag_set_imei(const byte *imei);
extern void umtsdiag_get_imei(byte *imei);

#endif
