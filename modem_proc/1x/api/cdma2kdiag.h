#ifndef CDMA2KDIAG_H
#define CDMA2KDIAG_H
/*==========================================================================
          Diagnostic System Packet Definitions for CDMA2000

  Description: Packet definitions to support the interface between the
  DMSS software and the external device for CDMA2000-related packets.

  Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc. 
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

                            Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/cdma2kdiag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/14   agh     Support for new customer specific voice service option
07/29/13   ab      Remove DIAG_TA_PARM_F and DIAG_TEST_F commands
04/05/11   jtm     Removed feature guards.
11/09/10   jtm     Initial SU API split.
01/25/10   ag      Reverted changes done for EMSO stats.
01/20/10   mca     Updated PACKED macro
11/06/09   ag      Added support for enhanced Markov service option stats.
09/22/09   adw     Removed include dependency between this file and parmdiag.h.
08/13/09   mca     Added cdma2kdiag_init()
07/22/09   adw     Mainlined FEATURE_DIAG_COUPLED_ARCHITECTURE.
05/18/09   adw     Removed featurization as part of CMI phase-II cleanup.
05/08/09   mca     Moved operating states from diag.h
08/21/08   ag      Renamed cdma2kdiag_get_event_cnt() to diag_get_event_cnt()
                   Renamed cdma2kdiag_inc_event_cnt() to diag_inc_event_cnt
08/07/08   mca     Renamed diag_get_event_cnt() to cdma2kdiag_get_event_cnt()
                   Added prototype for cdma2kdiag_inc_event_cnt()
04/23/07   as      Enabled pragma pack support for WINCE targets
07/15/05   as      Added push & pop pragma pack
01/08/04   as      Moved prl specific functions to prldiag.c/h
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
08/26/03   as      Removed FEATURE_SPECIAL_MDR
10/14/02   sj      Removed definition of NV_SIO_BITRATE_115200 here.
08/28/02   lad     Created file.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"

/* Diag */
#include "parmdiag.h"

/* Other */
#include "nv.h" /* Need NV_MAX_DIAL_DIGITS */



/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------
   Operating modes for packet processing.  These masks are used to define
   permissions for packet processing in the dispatch table.
---------------------------------------------------------------------------*/
#define MODE_OFF_DIG  0x01  /* Packet accepted in offline-digital        */
#define MODE_OFF_ANA  0x02  /* Packet accepted in offline-analog         */
#define MODE_ONLINE   0x04  /* Packet accepted in online                 */
#define MODE_ANY      0xFF  /* Packet accepted in all modes.             */

#define MODE_OFFLINE  ( MODE_OFF_DIG | MODE_OFF_ANA )
                              /* Packet accepted in either offline mode    */

/* -------------------------------------------------------------------------
** Definitions of Analog and CDMA operating states of the phone
** ------------------------------------------------------------------------- */
#define  DIAG_NO_SVC_S       0x00

/* (A)nalog States
*/
#define  DIAG_A_INIT_S       0x01
#define  DIAG_A_IDLE_S       0x02
#define  DIAG_A_VC_INIT_S    0x03
#define  DIAG_A_WFO_S        0x04
#define  DIAG_A_WFA_S        0x05
#define  DIAG_A_CONV_S       0x06
#define  DIAG_A_RELEASE_S    0x07
#define  DIAG_A_SA_S         0x08

/* CDMA (D)igital States
*/
#define  DIAG_D_INIT_S           0x81
#define  DIAG_D_IDLE_S           0x82
#define  DIAG_D_VC_INIT_S        0x83
#define  DIAG_D_WFO_S            0x84
#define  DIAG_D_WFA_S            0x85
#define  DIAG_D_CONV_S           0x86
#define  DIAG_D_RELEASE_S        0x87
#define  DIAG_D_OVHD_S           0x88
#define  DIAG_D_ORIG_S           0x89
#define  DIAG_D_PAGE_RES_S       0x8a
#define  DIAG_D_ORD_RES_S        0x8b
#define  DIAG_D_REG_S            0x8c
#define  DIAG_D_MSG_S            0x8d
#define  DIAG_D_USE_SUBSYS_CMD_S 0x8e

/* State of the (M)ode Controller when not in online state */
#define DIAG_M_OFFLINE_DIGITAL_S   0x10
#define DIAG_M_OFFLINE_HDR_S       0x11
#define DIAG_M_OFFLINE_ANALOG_S    0x12
#define DIAG_M_RESET_S             0x13
#define DIAG_M_POWERDOWN_S         0x14
#define DIAG_M_POWERSAVE_S         0x15
#define DIAG_M_POWERUP_S           0x16
#define DIAG_M_LPM_S               0x17
#define DIAG_M_MEAS_DED_S          0x18

#define DIAG_HDR_S                 0x40 /* Not hybrid, operating on HDR */



/*---------------------------------------------------------------------------

       Packet definitions for 1X-specific packets.

---------------------------------------------------------------------------*/

/*===========================================================================

PACKET   diag_status_req_type

ID       DIAG_STATUS_F

PURPOSE  Sent by the DM to request a set of status data

RESPONSE The DMSS gathers the status data and sends it in a
         diag_status_rsp_type packet to the DM.

============================================================================*/
typedef PACK( struct )
{
  byte cmd_code;               /* Command code */
} diag_status_req_type;

/*-----------------------------------------------------------
   The following 2 numbers are for the entry_reason in
   diag_status_rsp_type
-----------------------------------------------------------*/

/* Taken from MCC_ENTER_OFFLINE in mcc_entry_name_type.
   Please refer to mcc.h */
#define DIAG_OFFLINE_DIGITAL 2

/* Taken from APC_ENTER_OFFLINE in apc_entry_name_type.
   Please refer to acpmc.h*/
#define DIAG_OFFLINE_ANALOG  6

typedef PACK( struct )
{
  byte    cmd_code;               /* Command code                         */
  byte    demod;                  /* Demod id byte                        */
  byte    decode;                 /* Decoder id byte                      */
  byte    inter;                  /* Interleaver id byte                  */
  dword   esn;                    /* Mobile Electronic Serial Number      */

  word    rf_mode;                /* 0->analog,  1->cdma,  2->pcn         */
  dword   min1[NV_MAX_MINS];      /* all MIN1s                            */
  word    min2[NV_MAX_MINS];      /* all MIN2s                            */
  byte    orig_min;               /* index (0-3) of the orig MIN          */

  word    cdma_rx_state;          /* current state for cdma only          */
  int1    cdma_good_frames;       /* whether or not frames we are
                                     receiving are good -- cdma only      */
  word    analog_corrected_frames;/* good frame count - analog only       */
  word    analog_bad_frames;      /* bad frame count - analog only        */
  word    analog_word_syncs;      /*  -- analog only                      */

  word    entry_reason;           /* entry reason                         */
  word    curr_chan;              /* center frequency channel             */
  byte    cdma_code_chan;         /* code for current channel - cdma only */
  word    pilot_base;             /* pilot pn of current cell - cdma only */

  word    sid;                    /* Current System ID                    */
  word    nid;                    /* Current Network ID                   */
  word    locaid;                 /* Current Location ID                  */
  word    rssi;                   /* Current RSSI level                   */
  byte    power;                  /* Current mobile output power level    */
} diag_status_rsp_type;

/*===========================================================================

PACKET   diag_markov_req_type

ID       DIAG_MARKOV_F

PURPOSE  Sent by the DM to reqest Markov Statistics

RESPONSE The DMSS prepares and sends a diag_markov_rsp_type packet.

============================================================================*/
typedef PACK( struct )
{
  byte cmd_code;                /* Command code */
} diag_markov_req_type;

/* Definitions of values for the mode field - service option */
#define DIAG_SO_NONE            0x00
#define DIAG_SO_LOOPBACK        0x02
#define DIAG_SO_MARKOV          0x03
#define DIAG_SO_DATA            0x04
#define DIAG_SO_VOICE_96A       0x05
#define DIAG_SO_VOICE13         0x06 /* 13K (MSM2 only) Voice call */
#define DIAG_SO_RS2_MARKOV      0x07 /* 13K (MSM2 only) Markov call (new) */
#define DIAG_SO_RS1_MARKOV      0x08
#define DIAG_SO_LOOPBACK_13K    0x09
#define DIAG_SO_MARKOV_13K      0x0A /* 13K (MSM2 only) Markov call (old) */
#define DIAG_SO_EVRC            0x0B /* EVRC voice option */
#define DIAG_SO_VOICE_13K_IS733 0x11 /* IS-733 s.o. number */
#define DIAG_SO_MARKOV_SO54     0x36
#define DIAG_SO_LOOPBACK_SO55   0x37
#define DIAG_SO_MARKOV_SO74     0x4A
#define DIAG_SO_CUST_ENC_VOICE  0x4B  /* Customer defined voice SO with 
                                       * variable value */ 
                                       

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 1 forward and reverse.  Default Mux = 9 forward 1 reverse */
#define DIAG_SO_MDR_PKT_DATA_FRS1_RRS1   0x16 /* IS-707A s.o. number = 22 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 1 forward and rate set 2 reverse.
 Default Mux = 9 forward 2 reverse.  This SO is not support by MSM3000. */
#define DIAG_SO_MDR_PKT_DATA_FRS1_RRS2   0x17 /* IS-707A s.o. number = 23 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 2 forward and rate set 1 reverse.
 Default Mux = 10 forward 1 reverse.  This SO is not support by MSM3000. */
#define DIAG_SO_MDR_PKT_DATA_FRS2_RRS1   0x18 /* IS-707A s.o. number = 24 */

/* Medium Data Rate (externally known as High Speed Data) PPP Packet Data Service
 (IS-707A) with rate set 2 forward and reverse.  Default Mux = 10 forward 2 reverse */
#define DIAG_SO_MDR_PKT_DATA_FRS2_RRS2   0x19 /* IS-707A s.o. number = 25 */

#define DIAG_SO_MDR_DPT_FRS1_RRS1        0x24 /* MDR Data Pattern Test: Proprietary
                                               Service Option rate set 1 fwd and rev */

#define DIAG_SO_MDR_DPT_FRS2_RRS2        0x25 /* MDR Data Pattern Test: Proprietary
                                               Service Option rate set 2 fwd and rev */


typedef PACK( struct )
{
  byte cmd_code;               /* Command code                        */
  byte mode;                   /* rxc mode                            */
  word markov_rate;            /* rate for markov processing          */
  word rx_state;               /* receive task state                  */
  dword total_frames;          /* total number of Markov frames       */
  dword bad_frames;            /* total number of bad frames          */
  word data[ 4 ][ 10 ];        /* mar_test array items for expected
                                  full, half, quarter and eighth
                                  rate packets                        */
  dword bit_errs;              /* Markov bit errors                   */
  word good_errs[4];           /* Rate decision good,  but bits bad   */
} diag_markov_rsp_type;


/*===========================================================================

PACKET   diag_markov_reset_req_type

ID       DIAG_MARKOV_RESET_F
PURPOSE  Sent by the DM to reset the DMSS's Markov Statistics

RESPONSE DMSS resets the accumulated Markov Statistics and replies with
         a diag_m_reset_rsp_type packet.

============================================================================*/
typedef PACK( struct )
{
  byte cmd_code;                /* Command code */
} diag_markov_reset_req_type;

typedef PACK( struct )
{
  byte cmd_code;               /* Command code */
} diag_markov_reset_rsp_type;

/*===========================================================================

PACKET   diag_tagraph_req_type

ID       DIAG_TAGRAPH_F

PURPOSE  Sent by the DM to request info for lower-right corner of TA

RESPONSE The DMSS prepares and sends a diag_tagraph_rsp_type packet.

============================================================================*/
typedef PACK( struct )
{
  byte cmd_code;                /* Command code */
} diag_tagraph_req_type;

typedef PACK( struct )
{
  byte cmd_code;               /* Command code */
  byte  rx_rssi;               /* Current value of rx agc register */
  byte  adj_rssi;              /* Current value of tx gain adjust register */
  byte  tx_voc;                /* Current tx vocoder data rate */
  byte  rx_voc;                /* Current rx vocoder data rate */
  dword bad_frames;            /* Number of bad frames */
  dword total_frames;          /* Number of all the frames */
} diag_tagraph_rsp_type;



/*===========================================================================

PACKET   diag_ser_reset_req_type

ID       DIAG_SER_RESET_F

PURPOSE  Sent by DM to direct the DMSS reset the SER counts

RESPONSE DMSS resets counts and sends reply packet acknowledging the action

===========================================================================*/
typedef PACK( struct )
{
  byte cmd_code;
} diag_ser_reset_req_type;


typedef PACK( struct )
{
  byte cmd_code;
} diag_ser_reset_rsp_type;


/*===========================================================================

PACKET   diag_ser_report_req_type

ID       DIAG_SER_REPORT_F

PURPOSE  Sent by DM to direct the DMSS to report the SER counts.  They
         are not reset.

RESPONSE DMSS send reply packet containing the counts.

===========================================================================*/
typedef PACK( struct )
{
  byte cmd_code;
} diag_ser_report_req_type;


typedef PACK( struct )
{
  byte cmd_code;
  dword frame_cnt;  /* Total frame count */
  dword ser1;   /* Sum of all 192 bit frame symbol error counts */
  dword ser2;   /* Sum of all 96 bit frame symbol error counts */
  dword ser4;   /* Sum of all 48 bit frame symbol error counts */
  dword ser8;   /* Sum of all 24 bit frame symbol error counts */
} diag_ser_report_rsp_type;


/*===========================================================================

PACKET   diag_state_req_type

ID       DIAG_STATE_F

PURPOSE  Sent by external device to obtain the current state of operation
         the phone is in.

RESPONSE The diag_state_rsp_type will be sent in response.

===========================================================================*/
typedef PACK( struct )
{
  byte cmd_code;              /* Command code */
} diag_state_req_type;

typedef PACK( struct )
{
  byte cmd_code;              /* Command code */
  byte phone_state;           /* Current phone state */
  word event_count;           /* Count of possible state-changing events */
} diag_state_rsp_type;


/*===========================================================================

PACKET   diag_pilot_sets_req_type

ID       DIAG_PILOT_SETS_F

PURPOSE  Sent by external device to obtain the current contents of the
         various pilot sets.

RESPONSE The diag_pilot_sets_rsp_type will be sent in response.

===========================================================================*/
typedef PACK( struct )
{
  byte cmd_code;
} diag_pilot_sets_req_type;

typedef PACK( struct )
{
    word pilot_pn;            /* Pilot offset   */
    word pilot_strength;      /* Pilot strength */
} diag_pilot_data_type;

typedef PACK( struct )
{
  byte cmd_code;              /* Command code */
  word pilot_inc;             /* Pilot increment for remaining sets */
  byte active_cnt;            /* Count of pilots in active set (up to 6) */
  byte cand_cnt;              /* Count of pilots in candidate set (up to 6) */
  byte neigh_cnt;             /* Count of pilots in neighbor set (up to 40) */
  diag_pilot_data_type pilot_sets[52];
                              /* Sets of pilots (in the above order) (6+6+40) */
} diag_pilot_sets_rsp_type;


/*===========================================================================

PACKET   DIAG_AKEY_VERIFY_F

PURPOSE  Sent by DM to verify AKEY update.  Contains AKEY.  DM or System
         Unit can ask AKEY update for a particular NAM or if NAM is 0xff,
         AKEY for current nam will be updated.

RESPONSE DMSS responds with message saying whether or not AKEY update was
         successful.

============================================================================*/
#define DIAG_AKEY_DIGITS 26
#define DIAG_AKEY_REQ_RESERVED 8

typedef PACK( struct )
{
  byte                cmd_code;                    /* Command code          */
  byte                akey[DIAG_AKEY_DIGITS];      /* AKEY with             */
                                                   /* cryptographic check   */
  byte                nam;                         /* NAM for which AKEY    */
                                                   /* applies               */
                                                   /* If NAM is set to 0xff,*/
                                                   /* AKEY for current NAM  */
                                                   /* is changed            */
  byte                reserved[DIAG_AKEY_REQ_RESERVED]; /* for future expansion  */
} diag_akey_verify_req_type;

typedef PACK( struct )
{
  byte cmd_code;              /* Command code */
  boolean akey_ok;            /* The AKEY was entered correctly or not */
} diag_akey_verify_rsp_type;


/*===========================================================================

PACKET   DIAG_SSD_VERIFY_F

PURPOSE  Sent by DM to verify SSD update.  Contains SSDs.  DM or System
         Unit can ask for SSDs verified for a particular NAM or if NAM
         is 0xff, SSDs for current nam will be verified.

RESPONSE DMSS responds with message saying whether or not SSD verification
         was successful.

============================================================================*/
#define DIAG_SSD_A   0
#define DIAG_SSD_B   1

typedef PACK( struct )
{
  byte               cmd_code;
  byte               ssd_select;                   /* 0 ssd_a and 1 ssd_b   */
  qword              ssd_a;                       /* Shared secret A       */
  qword              ssd_b;                       /* Shared secret B       */
  byte               nam;                         /* NAM for which SSDs    */
                                                  /* applies               */
                                                  /* If NAM is set to 0xff,*/
                                                  /* SSD for current NAM   */
                                                  /* is checked            */
} DIAG_SSD_VERIFY_F_req_type;

typedef PACK( struct )
{
  byte cmd_code;
  boolean ssd_ok;             /* The SSDs were entered correctly or not */
} DIAG_SSD_VERIFY_F_rsp_type;


/*===========================================================================

PACKET   DIAG_STATUS_SNAPSHOT_REQ_TYPE
PURPOSE  This command gives a variable length response snapshot of all
  information needed at the start of logging to accuratley analyze the log
  data.

===========================================================================*/
typedef PACK( struct )
{
 byte                cmd_code;  /* Command code   */
} diag_status_snapshot_req_type;

typedef PACK( struct )
{
  byte                cmd_code;  /* Command code   */
  uint32 esn;

  PACK( struct )
  {
    uint32 imsi_s1;       /* First part of Operational IMSI (IMSI_O_S1) */
    uint16 imsi_s2;        /* Second part of Operational IMSI (IMSI_O_S2) */
    uint64 imsi_s;        /* IMSI_S1 and IMSI_S2 converted to a qword imsi_s */
    uint8  imsi_11_12;     /* Mobile Network Code (MNC) for IMSI_O address */
    uint16 mcc;            /* Mobile Country Code (MCC) for IMSI_O address */
    uint8  imsi_addr_num;  /* Number of digits in NMSI - 4, or class 0 IMSI flag */
  } full_imsi;

  uint16 sid;
  uint16 nid;
  uint8  p_rev;
  uint8  prev_in_use;
  uint8  mob_p_rev;
  uint8  band_class;
  uint16 frequency;
  uint8  operation_mode; //CDMA, Analog, GPS, etc (AKA rf_mode) -LD

  /* MC state: Initialization, Idle, Access, Traffic */
  uint8  state;

  /* The sub-state depends on the value in the above 'state' value.
  ** init state:    System Determination
  **                Pilot Channel Acquisition
  **                Sync Channel Acquisition
  **                Timing Change
  ** idle state:    Slotted mode
  **                Non-slotted mode
  ** access state:  Update Overhead Information
  **                Registration Access
  **                Order/Message Response
  **                Transmission
  **                Origination Attempt
  **                Page Response
  ** traffic state: Traffic Channel Initialization
  **                Waiting for Order
  **                Waiting for MS Answer
  **                Conversation
  **                Release
  */
  uint8  sub_state;
} diag_status_snapshot_rsp_type;

#define   DIAG_SRCH_SUP_CHANNEL_MAX 7
#define   DIAG_SRCH_ASET_MAX 6
/*===========================================================================

PACKET   DIAG_SUP_WALSH_CODES_F

PURPOSE  Sent by DMSS, contains

============================================================================*/
typedef struct
{
  byte cmd_code;
} diag_sup_walsh_codes_req_type;

typedef PACK( struct )
{
  byte cmd_code;                                /* Command code */

  word num_aset;
    /* number of active set */

  PACK( struct )
  {
    int2 pilot_off;
      /* Pilot offset used by serach */

    byte num_sup_channels;
      /* Number of supplemental channels */

    word walsh_code[ DIAG_SRCH_SUP_CHANNEL_MAX + 1 ];
      /* Specifies which walsh code is being used, including fundamental */

  } ch_data [ DIAG_SRCH_ASET_MAX ];
    /* One for each Aset */

} diag_sup_walsh_codes_rsp_type;


/*===========================================================================

PACKET   diag_sup_fer_rsp_type

ID       DIAG_SUP_FER_F

PURPOSE  Sent by DMSS, contains Frame Error Rate data for the small
         window.  This is called if supplemental channels (MDR) is supported.
         This packet gives a snapshot of the FER info for all 8 channels at
         once.

============================================================================*/
typedef struct
{
  byte cmd_code;
} diag_sup_fer_req_type;

typedef PACK( struct )
{
  byte cmd_code;                                /* Command code */

  dword bad_frames[1 + DIAG_SRCH_SUP_CHANNEL_MAX]; /* Number of bad frames for:
                                                      Fundamental +
                                                      all sup channels. */

  dword total_frames[1 + DIAG_SRCH_SUP_CHANNEL_MAX]; /* Total frames for:
                                                        Fundamental +
                                                        all sup channels. */

} diag_sup_fer_rsp_type;

/*===========================================================================

PACKET   diag_set_max_sup_ch_rsp_type

ID       DIAG_SET_MAX_SUP_CH_F

PURPOSE  This response acknowledges a valid setting.

============================================================================*/
typedef PACK( struct )
{
  byte cmd_code;            /* Command code */
  word service_option;      /* Specifies service option (rate set) */
  byte num_sup_ch;          /* Number of supplemental channels to be allowed*/
} diag_set_max_sup_ch_req_type;

typedef PACK( struct )
{
  byte  cmd_code;                           /* Command code */
} diag_set_max_sup_ch_rsp_type;


/*===========================================================================

PACKET   DIAG_RLP_STAT_RESET_F
PURPOSE  Reset RLP accumulated statistics

===========================================================================*/

/* No need to PACK this */
typedef struct {
  byte cmd_code;
  byte  sr_id;
} DIAG_RLP_STAT_RESET_F_req_type;

typedef struct {
  byte cmd_code;
} DIAG_RLP_STAT_RESET_F_rsp_type;

/*===========================================================================

PACKET   DIAG_TDSO_STAT_RESET_F
PURPOSE  Reset (S)TDSO accumulated statistics

===========================================================================*/

typedef struct {
  byte cmd_code;
} DIAG_TDSO_STAT_RESET_F_req_type;

typedef DIAG_TDSO_STAT_RESET_F_req_type DIAG_TDSO_STAT_RESET_F_rsp_type;


/*===========================================================================

PACKET   diag_ftm_req_type

ID       DIAG_FTM_CMD_F

PURPOSE  Sent by DM to request Factory Test Mode commands.

RESPONSE DMSS sends back status of the requested command.

===========================================================================*/

typedef PACK( struct )
{
  byte cmd_code;
  word sub_cmd;
  word length;      /* Length of the request[] */
  byte request[1];  /* Actual data content varies with request */
} diag_ftm_req_type;


typedef PACK( struct )
{
  byte cmd_code;
  word sub_cmd;
  byte status;
} diag_ftm_rsp_type;


/* Some functions in 1X targets still want to see the union.
 * This method will be phased out. */
typedef PACK( union )
{
  byte                         cmd_code;

  diag_status_req_type         stat;

  diag_markov_req_type         markov;
  diag_markov_reset_req_type   m_reset;

  diag_parm_set_req_type       parm_set;

  diag_parm_get2_req_type      parm_get2;

  diag_parm_get_is95b_req_type parm_get_is95b;

  diag_parm_retrieve_req_type  parm_retrieve;

} diag_req_type;

typedef PACK( union )
{
  byte                         cmd_code;

  diag_status_rsp_type         stat;

  diag_markov_rsp_type         markov;
  diag_markov_reset_rsp_type   m_reset;

  diag_parm_set_rsp_type       parm_set;

  diag_parm_get2_rsp_type      parm_get2;

  diag_parm_get_is95b_rsp_type parm_get_is95b;

  diag_parm_retrieve_rsp_type  parm_retrieve;

} diag_rsp_type;

/*===========================================================================

FUNCTION DIAG_GET_EVENT_CNT

DESCRIPTION
  This function returns the count of events maintained by this module.

DEPENDENCIES
  None.

RETURN VALUE
  diag_event_count.

SIDE EFFECTS
  None.

===========================================================================*/
word diag_get_event_cnt(void);

/*===========================================================================

FUNCTION DIAG_INC_EVENT_CNT

DESCRIPTION
  This function increments a count of events that took place in the phone
  which might imply that the phone had changed enough that other kinds of
  status might want to be requested from diag by external devices.

===========================================================================*/
void diag_inc_event_cnt (void);

/*===========================================================================

FUNCTION CDMA2KDIAG_INIT

DESCRIPTION
  This function sets up the dispatch table for the cdma2kdiag functions.

===========================================================================*/
void cdma2kdiag_init (void);

#endif /* CDMA2KDIAG_H  */

