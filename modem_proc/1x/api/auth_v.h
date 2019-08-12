#ifndef AUTH_V_H
#define AUTH_V_H
/*===========================================================================

               E X T E R N A L   A U T H E N T I C A T I O N
                      T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

  Copyright (c) 1995 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/auth.h_v   1.8   30 Aug 2002 13:34:04   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/auth_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/11   vks     Fix compilation error. 
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/18/10   jtm     Removed featurization.
11/15/10   jtm     Initial SU API split for CP.
09/17/10   ag      Added support for dynamic DOG.
06/24/10   ssh     Added support for EFS sync.
01/10/10   jj      CSIM-P2 , migration to MMGSDI APIs.
08/30/09   jj      Added include of uim_v.h as part of CMI.
03/26/09   adw     Removed unnecessary ulpn.h include.
11/30/05   va      Added new function auth_send_update_a_key_cmd  which is
                   remotable for calling from SVCPRG module.
11/30/05   an      Mainlined FEATURE_IS2000
05/03/05   fc      Mainlined FEATURE_IS2000.
03/19/04   sb      Removed auth_switch_to_private_lcm.
08/30/02   ts      Added support for Run Time R-UIM Enable
08/06/02   lh      Remove SVD ifdefs.
04/12/02   lh      Added support for subaddress in Orig_C.
03/12/02   lh      Added CMEA support for release A messages.
08/09/01   sb      Merged the following from common archive:
                   06/19/01   ts      Moved the VPM size constant to cave.c.  Took out the
                                      redundant #ifdef FEATURE_AUTH.
                   06/13/01   ts      Changed the Generate Keys octets from 1-65 to 0-64.
05/25/01   ts      Removed RUIM server code from auth and added interface to
                   new UIM server.
02/07/01   wli     Split RUIM_UPDATE_RECORD_ST to two states:
                   RUIM_UPDATE_LINEAR_FIXED_RECORD_ST and
                   RUIM_UPDATE_CYCLIC_RECORD_ST.
1/27/01    wli     Added new RUIM commands, INVALIDATE and REHABILITATE.
12/18/00   ck      Changed the request offset in the SSPR Configuration Request
                   Interface to a word from byte.
11/28/00   ck      Added definitions to support IS-683A with the R-UIM.
11/14/00   ts      Added access to the MSISDN in the CDMA directory.
10/11/00   ts      Moved the RUIM status outside of the rpt union field into its
                   own field.
07/16/00   ck      Added CDMA DF definition.
06/12/00   ck      Added definitions for checks related to PPS and access of
                   records from RUIM.
05/30/00   ck      Changed RUIM_UI_CHV_SIG to RUIM_CHV_SIG and also removed
                   RUIM_MAX_CHV_DIGITS as it was defined in ruim.h
05/26/00   ck      Added a comma that caused the compile to break
05/01/00   ck      Added support for RUIM software. Included the new interfaces
                   for RUIM software and featurised it under FEATURE_RUIM.
                   AUTH only interface is featurised under FEATURE_AUTH to give
                   the flexibility of defining only FEATURE_AUTH or
                   FEATURE_RUIM.
04/06/00   va      Merged the following from MSM3100_CP.03.00.17
           ry      Added OTAPA support
05/06/99   kmp     Changed T_ACP to FEATURE_ACP
03/18/99   ck      Merged the support for A-Key exchange and Re-Authentication
09/04/98   pms     Neared the variables to save ROM space
07/28/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
11/17/95   dna     Report back when keys are generated.
08/03/95   dna     Added support for message encryption
06/07/95   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "cmd.h"
#include "uim.h"
#include "uim_v.h"
#include "auth.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define RUIM_RUN_CAVE_DIGITS      3
#define RUIM_GLOBAL_RAND_CHAL     0x00
#define RUIM_UNIQUE_RAND_CHAL     0x01

  /* Process control bits */
#define RUIM_DEFLT_PROC_CNTRL     0x00
#define RUIM_SAVE_REGS_ON         0x10
#define RUIM_COMMIT_NEW_SSD       0x04



/*--------------------------------------------------------------------------
                         AUTHENTICATION REPORT

--------------------------------------------------------------------------*/
/* -------------------------------------------------------------------
** The Authentication Task optionally sends a report when it completes
** a command which produces a result which would be useful to the
** task which issued the command.
** ------------------------------------------------------------------- */
typedef enum
{
  AUTH_SIGNATURE_R,     /* Authentication Signature report */
  AUTH_A_KEY_R,         /* Whether A key was updated OK */
  AUTH_FINISH_SSD_R,    /* Whether SSD update completed OK */
  AUTH_GENERATE_KEY_R,  /* Indicates the key generation has completed */
  AUTH_RESYNC_R,         /* Verify AUTH has reset itself */
  AUTH_UPDATE_A_KEY_TEMP_R,/* Verfify that A_KEY_TEMP has been updated */
  AUTH_COMMIT_KEYS_R,
  AUTH_SIGNATURE_SPASM_R, /* Verify auth_otapa for SPASM has completed */
  AUTH_BS_CHAL_R,       /* Return the RANDBS */
  AUTH_ACCESS_RUIM_R,   /* Access to the R-UIM                         */
  AUTH_RUIM_STATUS_R,   /* Status of the card */
  AUTH_MAX_R
} auth_report_code_type;

typedef struct
{
  cmd_hdr_type         rpt_hdr;
    /* Includes the queue links,  pointer to requesting task TCB,
       signal to set on completion,  and done queue. */
  auth_report_code_type rpt_type; /* What type of report this is */
  uim_rpt_status       rpt_status;      /* whether the access was OK */
  union
  {
    dword              auth;           /* auth_signature (18 bits) */
    boolean            ssd_update_ok;  /* Whether SSD update completed OK */
    boolean            a_key_update_ok;/* Whether A key was updated OK */
    boolean            commit_keys_ok; /* Whether the keys committed were OK */
    dword              randbs;         /* RANDBS from BS challenge */
  } rpt;
}
auth_rpt_type;

/*--------------------------------------------------------------------------
                    AUTHENTICATION COMMANDS

--------------------------------------------------------------------------*/

/* Status values for a command */
typedef enum {
  AUTH_RECEIVED_S,                  /* Command received */
  AUTH_INVALID_S,                   /* Invalid command */
  AUTH_ERR_S,                       /* An error occured */
  AUTH_UNINITIALIZED_S,             /* Authentication is not initialized */
  AUTH_MAXIMUM_S                    /* always last, this counts statuses */
} auth_status_type;

typedef auth_status_type auth_ruim_status_type;
typedef uim_items_enum_type ruim_items_enum_type;
typedef uim_access_type ruim_access_type;

/* Command types */
typedef enum {
  AUTH_INIT_F,                      /* Initialize on powerup or NAM change */
  AUTH_SSD_UPDATE_F,                /* Begin process to update SSD */
  AUTH_FINISH_SSD_UPDATE_F,         /* Finish process to update SSD */
  AUTH_SIGNATURE_F,                 /* Perform auth signature */
  AUTH_GENERATE_KEY_F,              /* Generate CMEA key and VPM */
  AUTH_UPDATE_A_KEY_F,              /* Update the A-key */
  AUTH_INC_COUNT_F,                 /* Increment count value */
  AUTH_START_TIMER_F,               /* Starts SSD Timer */
  AUTH_RESYNC_F,                    /* Re-synchronizes the command queue */
  AUTH_RESET_F,                     /* Reset Auth Task state */
  AUTH_UPDATE_A_KEY_TEMP_F,         /* Update the A-key */
  AUTH_COMMIT_KEYS_F,               /* Commit the key */
  AUTH_SIGNATURE_SPASM_F,           /* Perform auth signature for SPASM */
  AUTH_BS_CHAL_F,                   /* Do a Base station Challenge */
  AUTH_ACCESS_RUIM_F,               /* Read or Write from RUIM */
  AUTH_RUIM_STATUS_F,               /* Status of the Card */
  AUTH_MAX_F
} auth_cmd_name_type;

/* Indicate command handling options */
typedef enum {
  RUIM_OPTION_NONE       = 0,        /* No options used */
  RUIM_OPTION_ALWAYS_RPT = 1         /* Always report   */
} auth_ruim_option_type;

/* The common Header for all the commands */
typedef struct {
  cmd_hdr_type          cmd_hdr;       /* command header */
  auth_cmd_name_type    command;       /* The command */
  auth_status_type      status;        /* Space for message status */
  auth_ruim_option_type options;       /* Indicate command handling options */
  byte                  cmd_count;     /* Number of Attempts  */
} auth_hdr_type;

/* Initialize on powerup or NAM change */
typedef struct {                  /* AUTH_INIT_F */
  auth_hdr_type   hdr;            /* Command header */
  byte            nam;            /* Which NAM info to initialize with */
} auth_init_type;

/* Begin process to update SSD */
typedef struct {                  /* AUTH_SSD_UPDATE_F */
  auth_hdr_type   hdr;            /* Command header */
  qword           randssd;        /* 56 bits from BS for SSD Generation */
  byte            process_control;/* process control bits */
  dword           esn;            /* ESN ( 7 bytes per standard ) */
  dword           randbs;         /* 32 random bits for Auth Signature */
  dword           auth_data;      /* 24 data bits for Auth Signature */
} auth_ssd_update_type;

/* Finish process to update SSD */
typedef struct {                  /* AUTH_FINISH_SSD_UPDATE_F */
  auth_hdr_type   hdr;            /* Command header */
  dword           authbs;         /* Auth Signature from Base Station */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_finish_ssd_update_type;

/* Perform auth signature */
typedef struct {                  /* AUTH_SIGNATURE_F */
  auth_hdr_type   hdr;            /* Command header */
  byte            rand_type;      /* Rand/Randu     */
  byte            dig_len;        /* digit length expressed in bits */
  byte            digits[RUIM_RUN_CAVE_DIGITS];
                                  /* maximum of 6 BCD coded digits */
  byte            process_control;/* process control bits */
  dword           esn;            /* ESN ( 7 bytes per standard ) */
  dword           auth_data;      /* 24 data bits for Auth Signature */
  boolean         save_reg;       /* TRUE if registers are to be saved */
  dword           rand_chal;      /* 32 bits from BS for Auth Signature */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_signature_type;

/* Generate CMEA key and VPM */
typedef struct {                  /* AUTH_GENERATE_KEY_F */
  auth_hdr_type   hdr;            /* Command header */
  byte            vpm_first_octet;/* First octet of VPM to be output */
  byte            vpm_last_octet; /* Last  octet of VPM to be output */
  boolean         do_vpm;         /* TRUE if Voice Privacy Mask is needed */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_generate_key_type;

/* Update the A-key */
typedef struct {                  /* AUTH_UPDATE_A_KEY_F */
  auth_hdr_type   hdr;            /* Command header */
  byte            a_key[AUTH_A_KEY_DIGITS]; /* New A-key to be used */
  byte            nam;            /* NAM for which the A-key is intended */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_update_a_key_type;

/* Increment count value */
typedef struct {                  /* AUTH_START_TIMER_F */
  auth_hdr_type   hdr;            /* Command header */
} auth_inc_count_type;

/* Starts SSD Timer */
typedef struct {                  /* AUTH_START_TIMER_F */
  auth_hdr_type   hdr;            /* Command header */
} auth_start_timer_type;

typedef struct {                  /* AUTH_RESYNC_F */
  auth_hdr_type   hdr;            /* Command header */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_resync_type;

typedef struct {                  /* AUTH_RESET_F */
  auth_hdr_type   hdr;            /* Command header */
} auth_reset_type;

typedef struct {                  /* AUTH_COMMIT_KEYS_F */
  auth_hdr_type   hdr;            /* Command header */
  byte            nam_index;       /* the nam to commit the a_key into */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_commit_keys_type;

typedef struct {                  /* AUTH_UPDATE_A_KEY_TEMP_F */
  auth_hdr_type   hdr;            /* Command header */
  qword           a_key;/* a_key_temp */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_update_a_key_temp_type;

typedef struct {                  /* AUTH_BS_CHAL_F */
  auth_hdr_type   hdr;            /* Command header */
  dword           randseed;          /* Random number generator seed */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_bs_chal_type;

typedef struct {
  auth_hdr_type   hdr;            /* Command header */
  ruim_items_enum_type item;      /* Item to be accessed */
  byte            nam;            /* NAM info */
  ruim_access_type access;        /* Type of access, Read or Write */
  word            num_bytes;      /* Number of bytes to be read or written */
  word            num_bytes_rsp;  /* Number of bytes returned */
  word            num_records_rsp;/* Number of records in the response */
  word            offset;         /* Offset into the EF */
  void            *data_ptr;      /* Pointer for data */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_access_ruim_type;

typedef struct {
  auth_hdr_type   hdr;            /* Command header */
  void            (*rpt_function)( auth_rpt_type * );
                                  /* Pointer to Function to send report */
} auth_ruim_status_cmd_type;

typedef union {
  auth_hdr_type                hdr;     /* Generic header */
  auth_init_type               init;    /* Init auth variables */
  auth_ssd_update_type         ssd;     /* SSD Update*/
  auth_finish_ssd_update_type  f_ssd;   /* Finish SSD Update*/
  auth_signature_type          sig;     /* Authentication Signature */
  auth_generate_key_type       key;     /* Generate CMEA key and VPM */
  auth_update_a_key_type       a_key;   /* Update the A-key */
  auth_start_timer_type        start;   /* Start SSD timer */
  auth_inc_count_type          inc;     /* Increment COUNTsp */
  auth_resync_type             resync;  /* Re-syncronize command queue */
  auth_reset_type              reset;   /* Reset AUTH task state */
  auth_commit_keys_type        commit_keys;  /* commit the keys  */
  auth_update_a_key_temp_type  temp;    /* Update the a_key_temp */
  auth_bs_chal_type            chal;    /* Base station challenge type */
  auth_access_ruim_type        access_ruim; /* Read from or Write to RUIM */
  auth_ruim_status_cmd_type    status;      /* Status of the Card */
} auth_cmd_type;

/*===========================================================================

FUNCTION AUTH_CMD

DESCRIPTION
  The auth_cmd pointed to by the parameter passed in is queued up for AUTH.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
extern void auth_cmd (
  auth_cmd_type *cmd_ptr                    /* the command to queue up */
);

#endif /*AUTH_V_H*/
