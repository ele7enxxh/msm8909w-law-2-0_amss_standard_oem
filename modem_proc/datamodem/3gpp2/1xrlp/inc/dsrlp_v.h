#ifndef _DSRLP_V_H
#define _DSRLP_V_H

/*===========================================================================

  D A T A   S E R V I C E S   R L P   E X T E R N A L   H E A D E R  F I L E

DESCRIPTION
   This file contains external variable and function declarations for the
   Radio Link Protocol.

   This should be included by files which interface with RLP, like the
   MuxLayer and Call Processing.

   Note, order of fields in structs is optimized to save memory in 32bit
   ARM environment. This should be kept in mind when changing the ordering
   of fields.

Copyright (c) 1999-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlp.h_v   1.29   31 Jan 2003 14:39:18   sarithay  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/inc/dsrlp_v.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/09/11   ack     Added is_1xrlp_ready flag to chk for 1X RLP init state 
07/07/10   ms      Data statistics support for Field Test Display.
09/04/09   ss      CMI SU level modifications.
05/11/09   ms      Included Required Diag files.
03/06/09   ms      CMI Integratin fixes.
02/27/09   spn     Fixed Lint Error
06/23/08   spn     Increased the Array size for MARQ Information
06/10/07   as      Moved dsrlp_resv_list_type outside FEATURE_IS2000_REL_A
03/22/07   ac      EMPA RLP Logging error fixing.
12/21/06   spn     Integration of the SPD module into dsrlp.
07/20/06   kvd     7500 re-arch changes.
07/25/06   ksu     EMPA: 1x RLP code review comments
05/30/06   ksu     EMPA: RLP init code review comments
03/02/06   spn     Added DSRLP_MAX_HDR_MRLP_FLOW_NUMBER defining the max 
                   number of RLP FLOW Numbers that is possible.
01/03/06   ksu     EMPA: hook up forward link with new phys link architecture
12/23/05   ksu     EMPA RSP: two fwd rx queues
12/15/05   TMR     Changed RLP RX queue to watermark and added callback data
                   to RLP RX function callback in dsrlp_reg_srvc()
11/21/05   vas     Changed number of max supported MRLP to be 8 fwd, 8 rev
08/29/05   ksu/sy  Remove dsrlp_reg_hdlc() in favor of uint32 app_field
                   parameter to dsrlp_reg_srvc().
08/08/05   sk/sy   Merged semi-dynamic QOS changes from sandbox.
11/29/04   gr      Merged PZID hysteresis changes into main line
11/22/04   vas     Changes to support GAUP (FEATURE_HDR_QOS). Added function
                   dsrlp_get_free_rscb_index()
09/20/04   vas     Merged QOS changes from September development branch.
09/08/04   vr      Added functionality to notify the pzid hysteresis
                   engine about the first ppp data exchange
07/12/04   gr      Added contiguous tx and rx idle frame count to RLP stats
06/08/04   atp     Support for RLP blob type 2.
05/07/04   atp     First cut at supporting EVDV (IS707-B).
8/25/03    junz    Change the RLP stats Get and Clear functions to fix data
                   aborts due to invalid SR_ID
05/21/03   vas     Added support for MMHDR data.Added dsrlp_reset and changed
                   index for AN auth RLP.
04/10/03   sy      Added enum dsrlp_tx_sig_enum_type to support SVD 
                   algorithm under REL_A feature.
01/31/03   sy      Changed funtion name dsrlp_next_turbo_pdu5_chain() to
                   dsrlp_get_next_dsm_item_chain().
12/18/02   atp     Implemented change in standard 707 for QOS.
12/02/02   sy      Added funtion dsrlp_insert_pdu5null_hdr_turbo.
10/03/02   vas     Added function dsrlp_is_hdr()
09/17/02   ak      Added header for dsrlp_is_inited(). (chked in js). 
07/17/02   js      Added dsrlp_next_turbo_pdu5_chain() to be used by MUX
                   to resolve a turbo encoded MUX PDU5 chain of DSM items.
07/15/02   ak      Under FEATURE_DATA_MM, FEATURE_HDR_AUTH_ID sr_id index
                   is available.
06/06/02   sy      Added max_frame_rate and data_on_ch fields to 
                   dsrlp_rev_frame_type structure so that mux can 
                   pass more information to RLP regarding the channel used
                   and maximum rate that RLP can use to build the frames. 
05/02/02   snn     Added a function dsrlp_eval_qos_blob and included snm.h. 
03/15/02   snn     Added function dsrlp_any_sig_to_send for SVD
03/01/02   vas     Added DSRLP_HDR_AN_AUTH_SR_ID_INDEX to support multiple 
                   simultaneous RLPs for HDR AN authentication.
02/25/02   atp     Moved definition for QOS default from dsrlp.c to dsrlp.h.
01/31/02   atp     Added support for QOS. 
01/28/02   sy      Changed the prototypes appropriate to the dsrlp.c file.
12/03/01   snn     Added extern definition for RLP3 Negotiations.
11/27/01   snn     Added support for modifying RLP3 nak policy. Added extern
                   definitions of the functions to set RLP Opts.
09/14/01   rama    SR_ID fix. Differentiate between SR_ID and RLP Instance. 
                   Set a mapping between SR_ID and RLP Instance.
09/04/01   hcg     Added RLP throughput logging TIMETEST support.
08/13/01   snn     Added enumeration type for Mux_PDU_5.
08/02/01   vas     Changed some fields in dsrlp_stats_info_type to be uint64.
                   Added new fields for HDR RLP.
06/13/01   sy      Added DSRLP_RATE_INACTIVE enum to support P2 Mode to 
                   show which FCH/DCCH is inactive.
05/24/01   hcg     Made changes to support HDR (1xEV).
05/11/01   ak      Further support for P2 and control/hold.  Some file
                   cleanup.
04/10/01   lcc     Added support for P2 mode.
02/22/01   ak      Extern for array which holds RLP Logging IDs.
11/29/00   ak      Added DSRLP_RATE_ERASURE_SCH, which indicates an erasure
                   on the SCH. Due to LTU CRC checks in MUX.
09/05/00   ttl     Added DSRLP_RATE_BLANK_MASK and DSRLP_CAT_BLANK_MASK for
                   Enabling the data TX on SCH when the FCH is muted for data.
07/08/00   ak      Added function to return if RLP has data to send.
06/22/00   igt     Added "state" field in dsrlp_stats_info_type and also
                   added dsrlp_stats_enum to support AT$QCRL3D, AT$QCRL3R
06/07/00   ak      Removed prototype for dsrlp_setup_watermarks, since that
                   was obsoleted by dsrlp_reg_srvc.
06/05/00   ak      Extern for dsrlp_reg_srvc(), which is how dsmgr registers
                   with RLP.
06/02/00   ak      Added void to new functions to remove compiler warnings.
05/30/00   igt     added dsrlp_get_max_ms_nak_rounds_fwd() and _rev()
05/10/00   igt     added reset_timestamp to statistics structure.
05/09/00   igt     added RLP_BLOB_used, curr_tx_nak_rounds to stat structure.
04/28/00   igt     added fields to dsrlp_stats_info_type
03/21/00   igt     add dsrlp_get_stats(), dsrlp_clear_stats() and stat struct
02/15/00   igt     Added signatures for BLOB process and creation functions
07/28/99   na/hcg  Created module.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
#include "dsm.h"
#include "log.h"
#include "log_dmss.h"
#ifdef FEATURE_IS2000_REL_A
#include "snm.h"
#include "snm_v.h"
#endif /* FEATURE_IS2000_REL_A */
#include "dsrlp.h"
#include "dsrlp_api.h"
#ifdef FEATURE_HDR_QOS
#include "ds707_qosdefs.h"
#endif /* FEATURE_HDR_QOS */
/*===========================================================================

             DEFINITIONS  FOR THE MODULE

===========================================================================*/


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Enum of the different RLP versions that this code supports.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_VER_1X_1,                       /* RLP Type I                      */
  DSRLP_VER_1X_2,                       /* RLP Type II                     */
  DSRLP_VER_1X_3,                       /* RLP Type III                    */
  DSRLP_VER_HDR,                        /* RLP for HDR DPA (AN and SN)     */

  /* the following two versions are used for HDR with QoS (aka
     multi-flow), which includes both MFPA and EMPA (SN only)  */
  DSRLP_VER_HDR_MULTIFLOW_FORWARD,      /* FwdLink MultiFlow Receiver      */
  DSRLP_VER_HDR_MULTIFLOW_REVERSE,      /* RevLink MultiFlow Transmitter   */

  DSRLP_VER_INVALID                     /* RLP version is invalid          */
} dsrlp_ver_enum_type;



/*---------------------------------------------------------------------------
  Typedef that identifies an RLP.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 flow;              /* RLP FLow used in sig msgs to identify RLP   */
  dsrlp_ver_enum_type ver; /* Version of M-RLP (Fwd or Rev)               */
} dsrlp_rlp_identity_type;

/*---------------------------------------------------------------------------
  Number of RLPs of each version that can be supported And their base indices
---------------------------------------------------------------------------*/
/* Only 1 RLP for 1X is supported */
#define DSRLP_MAX_NUM_VER_3_RLP                 1
#define DSRLP_MAX_RSCB_ELEMENTS  ((DSRLP_MAX_NUM_VER_3_RLP)+1)               

#ifdef FEATURE_HDR
/* 2 RLPs on Default HDR supported. The rscb indices for HDR are different 
   than the 1X RLP indices
*/
#define DSRLP_MAX_NUM_HDR_RLP               2
#define DSRLP_HDR_BASE_INDEX ((DSRLP_FIXED_SR_ID_INDEX) + (DSRLP_MAX_NUM_VER_3_RLP))
#undef  DSRLP_MAX_RSCB_ELEMENTS
#define DSRLP_MAX_RSCB_ELEMENTS ((DSRLP_MAX_NUM_VER_3_RLP)   + \
                                 (DSRLP_MAX_NUM_HDR_RLP) + 1)

#ifdef FEATURE_HDR_QOS
/* 8 MultiFlow Forward HDR RLPs and 8 MultiFlow Reverse HDR RLPs supports. They use
   different RLP control blocks than the 1X and default HDR RLPs
*/
#define DSRLP_MAX_NUM_HDR_MRLP_FWD  8
#define DSRLP_MAX_NUM_HDR_MRLP_REV  8

/* The size for the EMPA RLP log buffer
*/
#define DSRLP_HDR_ENHANCED_LOG_BUFF_FWD (DSRLP_MAX_NUM_HDR_MRLP_FWD * DSRSP_NUM_ROUTES)
#define DSRLP_HDR_ENHANCED_LOG_BUFF_REV (DSRLP_MAX_NUM_HDR_MRLP_REV * DSRSP_NUM_ROUTES)


/* Determine the Base index of the forward and Rever Link MRLPs
   based on the number of RLPs each one supports */
#define DSRLP_HDR_MRLP_FWD_BASE_INDEX ( (DSRLP_HDR_BASE_INDEX) + \
                                        (DSRLP_MAX_NUM_HDR_RLP) )
                                        
#define DSRLP_HDR_MRLP_REV_BASE_INDEX ((DSRLP_HDR_MRLP_FWD_BASE_INDEX) + \
                                       (DSRLP_MAX_NUM_HDR_MRLP_FWD))

/* Indices of the default forward and reverse M-RLPs */
#define DSRLP_HDR_DEFAULT_MRLP_FWD_INDEX DSRLP_HDR_MRLP_FWD_BASE_INDEX
#define DSRLP_HDR_DEFAULT_MRLP_REV_INDEX DSRLP_HDR_MRLP_REV_BASE_INDEX

#undef  DSRLP_MAX_RSCB_ELEMENTS
#define DSRLP_MAX_RSCB_ELEMENTS ((DSRLP_MAX_NUM_VER_3_RLP)   + \
                                 (DSRLP_MAX_NUM_HDR_RLP) + \
                                 (DSRLP_MAX_NUM_HDR_MRLP_FWD) + \
                                 (DSRLP_MAX_NUM_HDR_MRLP_REV) + 1)

#endif /* FEATURE_HDR_QOS */
#endif /* FEATURE_HDR */

#define DSRLP_INVALID_HDR_MRLP_FLOW 0xFF

/* Maximum allowed flow Number for the RLP as per Standard IS 856A. */
#define DSRLP_MAX_HDR_MRLP_FLOW_NUMBER     32

/*---------------------------------------------------------------------------
  Enum for the return value dsrlp_rx_process_frames.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_RX_SYNC_TIMEOUT,                /* RLP timed out trying to SYNC    */
  DSRLP_RX_IDLE,                        /* Idle frame(s) rx'ed             */
  DSRLP_RX_NON_IDLE,                    /* Ctl/Data/Retx frame rx'ed       */
  DSRLP_RX_ERROR                        /* Errors in rx'ed frame(s)        */
} dsrlp_rx_retval_enum_type;

extern uint8 dsrlp_disable_inactivity_timer_bitmap;

#define DSRLP_MAX_NAKS_PER_ROUND 3 /* Maximum Naks that can be sent/round  */

#define DSRLP_BLOB_NOT_USED     0 /* RLP BLOB was not received             */
#define DSRLP_BLOB_NAK_SCHEME   1 /* RLP BLOB specified NAK scheme         */
#define DSRLP_BLOB_RTT          2 /* RLP BLOB specified RTT                */
#define DSRLP_BLOB_RTT_PLUS_NAK 3 /* RLP BLOB specified RTT & NAK scheme   */


#ifdef FEATURE_DS_PSTATS
#ifdef FEATURE_DS_RLP3
/*---------------------------------------------------------------------------
  Type to index the array of RLP stats.
  These stats apply to the current or most recent call.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_STATS_RX_DATA_CNT,         /* # of data frames received            */
  DSRLP_STATS_TX_DATA_CNT,         /* # of data frames Transmitted         */
  DSRLP_STATS_NAKS,                /* count of NAKs sent                   */
  DSRLP_STATS_DOUBLE_NAKS,         /* count of 2-in-a-row NAKs sent        */
  DSRLP_STATS_TRIPLE_NAKS,         /* count of 3-in-a-row NAKs sent        */
  DSRLP_STATS_RX_NAKS,             /* total # of NAKs received             */

/*DSRLP_STATS_TX_REXMITS,             count of REXMITs sent                */
/*DSRLP_STATS_RX_REXMITS,             count of REXMITs received            */
/*DSRLP_STATS_TX_IDLE_CNT,            # of transmitted IDLE frames         */
/*DSRLP_STATS_RX_IDLE_CNT,            # of received    IDLE frames         */
/*DSRLP_STATS_RX_BLANK_CNT,           # of received BLANK frames           */
/*DSRLP_STATS_RX_NULL_CNT,            # of received NULL frames            */
/*DSRLP_STATS_TX_FUNDAMENTAL,         # of txed  Fundamental Data frames   */
/*DSRLP_STATS_RX_FUNDAMENTAL,         # of received Fundamental Data frs   */

  DSRLP_STATS_TX_TOTAL_BYTES,      /* # of total bytes transmitted         */
  DSRLP_STATS_RX_TOTAL_BYTES,      /* # of total bytes received            */


  DSRLP_STATS_RE_XMITS_NOT_FOUND,  /* count of REXMITs not found           */
  DSRLP_STATS_FILL_FRAMES_RXED,    /* count of fill frames received        */

  DSRLP_STATS_RLP_ERASURES,        /* count of rlp level erasures          */
  DSRLP_STATS_MUX_ERASURES,        /* count of mux level erasures          */

  DSRLP_STATS_LARGEST_CONTIG_ERASE,/* largest block of contig. erasures    */

  DSRLP_STATS_LAST_RTT,            /* last round-trip time                 */

  DSRLP_STATS_RESETS,              /* count of RLP resets                  */
  DSRLP_STATS_ABORTS,              /* # of NAK aborts                      */

/*DSRLP_STATS_BLOB_USED,              shows if BLOB has been used and how  */

/*DSRLP_STATS_CURR_TX_NAK_RNDS,       # of tx nak rounds currently used    */

  DSRLP_STATS_RLP_STATE,           /* current state of the rlp             */

  DSRLP_STATS_COUNT                /* number of RLP stats                  */

} dsrlp_stats_enum;

/*---------------------------------------------------------------------------
  Array of pointers to stats names.
---------------------------------------------------------------------------*/
extern const byte * const dsrlp_stats_name_ptr[DSRLP_STATS_COUNT];

/*---------------------------------------------------------------------------
  Array of pointers to names of the last call ending state.
---------------------------------------------------------------------------*/
extern const byte * const dsrlp_last_call_synced[3];

/*---------------------------------------------------------------------------
  Pointer (into dsrlp_last_call_synced) to name of last call ending state.
---------------------------------------------------------------------------*/
extern byte * dsrlp_last_call_synced_ptr;

/*---------------------------------------------------------------------------
  Copy of the last round trip time.
---------------------------------------------------------------------------*/
extern word dsrlp_last_rtt;
#endif /* FEATURE_DS_RLP3   */
#endif /* FEATURE_DS_PSTATS */

#ifdef FEATURE_HDR_QOS
/*---------------------------------------------------------------------------
  Type for storing reservation related information
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 count;        /* Num of reservervations associated with this RLP */
  uint8 label[DS707_MAX_N_RESV_PER_DIR]; /* Reservations associated with the */
} dsrlp_resv_list_type;
#endif /* FEATURE_HDR_QOS*/

#ifdef FEATURE_IS2000_REL_A

/*---------------------------------------------------------------------------
   Default QOS parameter priority in case of non-assured mode, IS-707 A2
---------------------------------------------------------------------------*/
#define  DSRLP_QOS_NON_ASSUR_DEFAULT_PRI_ADJ 0x0D
#define  DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED   0x0
#define  DSRLP_QOS_DEFAULT_BLOB_TYPE         0x0

/*---------------------------------------------------------------------------
  Parameters for non-assured mode QOS
---------------------------------------------------------------------------*/
typedef struct
{
  boolean pri_adj_incl; /* If non-assured priority adjustment field present*/
  byte pri_adj;                                     /* priority adjustment */
} dsrlp_qos_non_assured_params_s_type;

/*---------------------------------------------------------------------------
  Parameters for assured mode QOS
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
     FL
  -------------------------------------------------------------------------*/

  /* FL qos included indicator */
  boolean FL_qos_incl;

  /* FL priority */
  boolean FL_pri_adj_incl;
  byte FL_pri_adj;

  /* FL min user data rates */
  boolean FL_requested_data_rate_incl;
  boolean FL_acceptable_data_rate_incl;
  byte FL_requested_data_rate;
  byte FL_acceptable_data_rate;

  /* FL data loss rates */
  boolean FL_requested_loss_rate_incl;
  boolean FL_acceptable_loss_rate_incl;
  byte FL_requested_loss_rate;
  byte FL_acceptable_loss_rate;

  /* FL max delays */
  boolean FL_requested_delay_incl;
  boolean FL_acceptable_delay_incl;
  byte FL_requested_delay;
  byte FL_acceptable_delay;

  /*-------------------------------------------------------------------------
     RL
  -------------------------------------------------------------------------*/

  /* RL qos included indicator */
  boolean RL_qos_incl;

  /* RL priority */
  boolean RL_pri_adj_incl;
  byte RL_pri_adj;

  /* RL min user data rates */
  boolean RL_requested_data_rate_incl;
  boolean RL_acceptable_data_rate_incl;
  byte RL_requested_data_rate;
  byte RL_acceptable_data_rate;

  /* RL data loss rates */
  boolean RL_requested_loss_rate_incl;
  boolean RL_acceptable_loss_rate_incl;
  byte RL_requested_loss_rate;
  byte RL_acceptable_loss_rate;

  /* RL max delays */
  boolean RL_requested_delay_incl;
  boolean RL_acceptable_delay_incl;
  byte RL_requested_delay;
  byte RL_acceptable_delay;

} dsrlp_qos_assured_params_s_type;


/*---------------------------------------------------------------------------
  QOS parameters, assured or non-assured depends on mode
---------------------------------------------------------------------------*/
typedef union
{
  dsrlp_qos_non_assured_params_s_type non_assur_params;
  dsrlp_qos_assured_params_s_type assur_params;

} dsrlp_qos_params_u_type;

/*---------------------------------------------------------------------------
  Representation of QOS parameters as a structure
---------------------------------------------------------------------------*/
typedef struct
{
  boolean qos_incl; /* QOS included or not indicator. Other fields valid   */
                    /* only if this indicator is TRUE.                     */
  boolean max_blob_type_supp_incl; /* If max blob type supported field     */
  boolean blob_type_incl;          /* If blob type field present or not    */
  byte    max_blob_type_supported; /* Max blob type supported              */
  byte    blob_type;               /* Blob type                            */
  boolean assur_mode; /* true - assured mode, false - nonassured mode      */
  dsrlp_qos_params_u_type params; /* Actual parameters depend on mode      */

} dsrlp_qos_s_type;


/*---------------------------------------------------------------------------
  Maximum length of the QOS blob sent by mobile, including reserved bits.
  Values obtained from IS-707 Section 2.2.9.2
---------------------------------------------------------------------------*/
#define DSRLP_MOBILE_QOS_BLOB_LEN_MAX  11                 /*     87 bits   */
#define DSRLP_BS_QOS_BLOB_LEN_MAX       6                 /*     41 bits   */

/*---------------------------------------------------------------------------
  One place to hold all the different QOS values - default, desired,
  received from BS.
---------------------------------------------------------------------------*/
typedef struct
{
  dsrlp_qos_s_type defaults;      /* Default QOS values from IS-707 2.2.9  */
  dsrlp_qos_s_type desired;       /* QOS values desired by application     */
  dsrlp_qos_s_type rx_from_bs;    /* QOS values received from base-station */
  dsrlp_qos_s_type * curr_p;      /* Current values. Ptr to one of above   */

} dsrlp_qos_values_type;

extern dsrlp_qos_values_type dsrlp_qos_values;

#endif /* FEATURE_IS2000_REL_A */

/*---------------------------------------------------------------------------
  Struct used to pass Srvc Ref ID to DSRLP
---------------------------------------------------------------------------*/
typedef struct
{
  byte sr_id;                             /* Service reference identifier */
  byte rlp_blob_len;              /* RLP information block of bits length */
  byte rlp_blob[15];                                 /* RLP block of bits */
  boolean rlp_info_incl;            /* RLP information included indicator */
} dsrlp_blob_cmd_type;

/*===========================================================================


                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION       DSRLP_GET_LAST_FILL_NAK_GAP

DESCRIPTION    Accessor method for dsrlp_last_fill_nak_gap

PARAMETERS
  None

RETURN VALUE   
  dsrlp_last_fill_nak_gap

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
uint8 dsrlp_get_last_fill_nak_gap(void);

/*===========================================================================
FUNCTION       DSRLP_SET_LAST_FILL_NAK_GAP

DESCRIPTION    Mutator method for dsrlp_last_fill_nak_gap

PARAMETERS
  new value for dsrlp_last_fill_nak_gap

RETURN VALUE   
  None

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
void dsrlp_set_last_fill_nak_gap
(
  uint8 last_fill_nak_gap       /* new value */
);

/*===========================================================================

FUNCTION       DSRLP_STARTUP

DESCRIPTION    This function is to be called only once per power-up
               of the phone.  This function initializes any structs
               which may be global across all RLP versions or instances.

               DSRLP_INIT is called per RLP instance, and so can be
               called many times, but this function must be called
               only once, and before any other RLP functions are called.


DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlp_startup(void);

/*===========================================================================

FUNCTION DSRLP_INIT

DESCRIPTION
  This function initializes the RLP session corresponding to the passed
  sr_id for the given version of RLP. All data structures are setup and
  RLP is reset to start new operation. This function should be called at the
  beginning of each call for each service.

  Input Parameters:
    rscb_index: Index in the rscb array where this record will be stored 
    sr_id: The Service ID for the particular service. Primary is usually 1
           and secondary is usually 2.
    ver  : The RLP version - 1,2,3...
    octet_sequenced: This RLP will run in byte sequenced mode. This means
           that all new frames and retransmissions will have a single
           byte sequence space.
           Mode of operation cannot be changed in the middle of a RLP
           session.
    enhanced: This RLP will carry EMPA/MMPA traffic


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_init
(
  byte rscb_index,                           /* index in the rscb array where
                                                this record will be stored */
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver,                   /* RLP version                */
  boolean octet_sequenced,                   /* Byte sequenced operation?  */
  dsrlp_enhanced_ver_enum_type enhanced      /* EMPA?                      */
);
/*===========================================================================

FUNCTION DSRLP_CLEANUP

DESCRIPTION
  This function cleansup the memory and resets the RLP state for a given
  sr_id. This should be called at the end of a RLP session.

  Input Parameters:
    sr_id: The Service ID for the particular service. Primary is usually 1
           and secondary is usually 2.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_cleanup
(
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type rlp_type               /* RLP Type                   */
);

/*===========================================================================

FUNCTION       DSRLP_GET_RLP_RCSB_INDEX

DESCRIPTION    Get the RLP instance (rscb array index) used by given RLP. 
               The RLP is identified by RLP Type and ID (Flow)

DEPENDENCIES   None

RETURN VALUE   The RLP instance, if found, an invalid index otherwise

SIDE EFFECTS   None
===========================================================================*/
byte dsrlp_get_rlp_rscb_index
(
  byte sr_id, 
  dsrlp_ver_enum_type ver
);
#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION      DSRLP_ACTIVATE_HDRMRLPCTL_BLK

DESCRIPTION   This Activates the HDR Multi Flow RLP. It reserves an RSCB index
              for the particular RLP (sr_id). The control block is needed in 
              order to maintain state of the RLP even when the RLP is not 
              initialized. For example, the RLP may be Active Open or Active 
              Close irrespective of the state of the connection. 
              
              Note that this function is specific for HDR MultiFlow RLP
               
DEPENDENCIES  None

RETURN VALUE  The RLP instance, if successfully activated.
              an invalid index otherwise

SIDE EFFECTS  None
===========================================================================*/
byte dsrlp_activate_hdrmrlp_ctl_blk
(
  byte rscb_index,                           /* index in the rscb array where
                                                this record will be stored */
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver                    /* RLP version                */
);
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION      DSRLP_DEACTIVATE_HDRMRLPCTL_BLK

DESCRIPTION   This Deactivates the HDR Multi Flow RLP. 

It reserves an RSCB index
              for the particular RLP (sr_id). The control block is needed in 
              order to maintain state of the RLP even when the RLP is not 
              initialized. For example, the RLP may be Active Open or Active 
              Close irrespective of the state of the connection. 
              
              Note that this function is specific for HDR MultiFlow RLP
               
DEPENDENCIES  None

RETURN VALUE  The RLP instance, if successfully activated.
              an invalid index otherwise

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_deactivate_hdrmrlp_ctl_blk
(
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver                    /* RLP version                */
);
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION      DSRLP_IS_RLP_ACTIVE

DESCRIPTION   Returns whether the RLP is activated or not.              
              Note that this function is specific for HDR MultiFlow RLP
               
DEPENDENCIES  None

RETURN VALUE  TRUE if RLP is active
              FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_is_rlp_active
(
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver                    /* RLP version                */
);
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION       DSRLP_GET_FREE_RCSB_INDEX

DESCRIPTION    This function returns an index to a free RLP control block of 
               the version specified. By free, it means that this index has
               not been reserved for any RLP.
               
               For the HDRMRLP, activating an RLP reserves an index, 
               deactivating frees the index.               
               
DEPENDENCIES   None

RETURN VALUE   If a free index of the version specified is found, it returns
               the index, otherwise an invalid index is returned.

SIDE EFFECTS   None
===========================================================================*/
byte dsrlp_get_free_rscb_index
(
  dsrlp_ver_enum_type ver
);
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_QOS
/*===========================================================================

FUNCTION       DSRLP_UPDATE_ACTIVE_MRLP_CHANGED_PARAMS

DESCRIPTION    Determines the parameters that have changed for an active RLP. 
               If any have chnaged, it sets a flag to the RLP indicating that 
               parameters have changed and need to be updated.   
               
               Note that currently this function only works for the the
               Multiflow Packet App bound to the SN. Will need to check 
               for the app subtype if it is to be supported for the AN stream.
               
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlp_update_active_mrlp_changed_params
(
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver                    /* RLP version                */
);
#endif /* FEATURE_HDR_QOS */

/*===========================================================================

FUNCTION DSRLP_PROCESS_RX_FRAMES

DESCRIPTION
  This function processes the RLP frame(s) received over the air. This is the
  interface between RLP receive and the multiplex sublayer. The multiplex
  sublayer can send all the frames it received in one frame time to RLP or
  split it up into multiple calls of this function.

  Input Parameters:

  The parameter to this function is a pointer to a structure which contains
  an array of frames that the mux sublayer wants RLP to process. Each element
  of this array includes all the information that RLP needs to process the
  frame independently. The information passed with each frame includes a
  pointer to the dsm item containing the frame, the muxPDU type, service type
  (primary or secondary).

  The fr_item_ptr can be a pointer to dsm packet chain (with multiple
  dsm items).

  This per frame data structure is passed to a number of functions within
  the RLP code.

DEPENDENCIES
  dsrlp_init() should have been previously invoked. rlp_establish_srvc_type()
  should have been previously invoked to set up the traffic type, rate set
  and RLP type in use.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_process_rx_frames
(
  dsrlp_fwd_frame_block_type *fwd_frame_block/* Ptr to rcvd RLP frames     */
);

/*===========================================================================

FUNCTION DSRLP_BUILD_RLP_BLOB

DESCRIPTION

  This function builds an RLP BLOB (Block of Bytes)

  Input Parameters:

  *blob_ptr:   is a pointer pointing to free space for new blob be built.


DEPENDENCIES
  None

RETURN VALUE
  *blob_ptr: is a pointer pointing to the blob that was build.

   blob_len: is returned by the function and contains the length of the new
             blob.

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_build_rlp_blob
(
  byte  *blob_ptr
);

/*===========================================================================

FUNCTION DSRLP_PROCESS_RLP_BLOB

DESCRIPTION
  This function processes the received RLP BLOB (Block of Bytes)

  Input Parameters:

  sr_id: used to find the appropriate session control block pointer

  *blob_ptr: points to the BLOB to be processed

  blob_len: specifies the length of the BLOB to be processed

DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean dsrlp_process_rlp_blob
(
  byte   sr_id,
  byte  *blob_ptr,
  byte   blob_len
);

/*===========================================================================

FUNCTION DSRLP_GET_STATS_EX

DESCRIPTION
  This function gets an sr_id and then returns a pointer to the appropriate
  structure containing the related statistics.

  Input Parameters:

  sr_id:         used to find the appropriate session control block pointer

  route:         The route for which stats are needed

  stats_ptr_ptr: is a pointer to a pointer to the stats structure

DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  stats_ptr_ptr:  is the pointer to the pointer to the stats structure
  return TRUE if the *stats_ptr_ptr points to a valid memory location,
  otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean dsrlp_get_stats_ex(
  byte                      rscb_index,
  byte                      route,
  dsrlp_stats_info_type   **stats_ptr_ptr
);


/*===========================================================================

FUNCTION DSRLP_CLEAR_STATS

DESCRIPTION
  This function takes an sr_id and uses that to get the appropriate session
  control block pointer and from there it will initialize the related
  structure with the statistics.

  Input Parameters:

  rscb_index:  used to find the appropriate session control block pointer


DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_clear_stats
(
    byte rscb_index
);


#ifdef FEATURE_DS_PSTATS
#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION DSRLP_GET_STATS_AT

DESCRIPTION
  This function copies the RLP stats into the given array.
  This particular function is for use with the AT commands.

DEPENDENCIES
  The given array must be of size DSRLP_STATS_COUNT or greater.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_get_stats_at
(
  uint32 *stats_ptr,  /* pointer to array to load with stats */
  byte    rscb_index
);
#endif /* FEATURE_DS_RLP3   */
#endif /* FEATURE_DS_PSTATS */

/*===========================================================================
FUNCTION      DSRLP_REG_SRVC

DESCRIPTION   Registers the watermarks and queues/callbacks for an RLP
              session.

DEPENDENCIES  A valid sr_id value must be passed into this function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_reg_srvc
(
  byte                rscb_index,            /* index for a session id     */
  dsrlp_ver_enum_type ver,                   /* RLP version                */
  dsm_watermark_type  *tx_watermark_ptr,     /* ptr to Tx wm for txmit frs */
  void                (*post_rx_func_ptr)(uint32), /* rx callback for rx frames  */
  dsm_watermark_type   *post_rx_wm_ptr,      /* watermarks for rx'ed frames    */
  byte                 route,                /* indicates the route for this RLP */
  uint32               post_rx_data          /* arg to post_rx_func_ptr()  */
);

/*===========================================================================
FUNCTION      DSRLP_REG_PZID_HYST_NOTIFY_FUNC

DESCRIPTION   This registers the function to notify PZID hysterisis engine
              if a new RLP packet is sent/received after going dormant.

              If a NULL function is passed, the "notify PZID" function is
              deregistered.

DEPENDENCIES  A valid index (corresponding to an sr_id) must be passed 
              into this function. 

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_reg_ppp_data_transfer_notify_func
(
  byte                rscb_index,            /* index for a session id     */
  void (*ppp_data_transfer_notify_func) (byte rlp_instance)
);

/*===========================================================================
FUNCTION      DSRLP_BYTES_DATA_TO_SEND

DESCRIPTION   Gives some indication of the # new bytes of data to send by
              RLP 3.

DEPENDENCIES  The passed in SR_ID is good.

RETURN VALUE  TRUE - new data or rexmits to be sent.  FALSE - no data to be
              rexmitted.

SIDE EFFECTS  None
===========================================================================*/
word dsrlp_bytes_data_to_send
(
  byte  rscb_index                       /* passed in SR_ID                */
);

/*===========================================================================
FUNCTION      DSRLP_HAS_DATA_TO_SEND

DESCRIPTION   Does RLP3 have stuff to send? (either naks, rexmits, new data,
              etc).

DEPENDENCIES  The passed in SR_ID is good.

RETURN VALUE  TRUE - RLP needs to send some data.

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_has_data_to_send
(
   byte  rscb_index                      /* passed in SR_ID                */
);
#ifdef TIMETEST_DSRLP_STATS 
/*===========================================================================

FUNCTION DSRLP_LOG_TIMETEST_THROUGHPUT

DESCRIPTION
  This function is a clock callback function to control logging of the
  total Rx and Tx RLP bytes to the TIMETEST port.  It is controlled by the
  dsrlp_timetest_cb clk_cb_type and is registered in dsrlp_init() to go off
  every 500 ms.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dsrlp_log_timetest_throughput
( 
  int4 ms 
);
#endif /* TIMETEST_DSRLP_STATS */

/*===========================================================================
FUNCTION      DSRLP_QUEUE_SDB_RX_DATA

DESCRIPTION   Queues SDB RX data into RLP RX queue and calls registered
              RX callback

DEPENDENCIES  The passed in RSCB_INDEX is good.

RETURN VALUE  TRUE if operation is sucessful, FALSE if not.  If function
              returns FALSE, input data packet will need to be freed by
              caller

SIDE EFFECTS  Adds data to RX watermark
===========================================================================*/
extern boolean 
dsrlp_queue_sdb_rx_data
(
  byte          rscb_index,
  dsm_item_type *data_packet
);


/*===========================================================================

FUNCTION     DSRLP_GET_ALL_DEF_NAKS
DESCRIPTION  Returns the number of default rounds and the naks per round to be
             used when negotiating an RLP3 service option
DEPENDENCIES There must be sufficient space in the naks_per_round arrays
             to hold the returned data
RETURN VALUE Boolean: 
             True: the operation was success.
             False: The operation was not done. None of the values in the
                    passed pointers may be good.
                    Values are returned by argument pointers
SIDE EFFECTS None
===========================================================================*/
extern boolean dsrlp_get_all_def_naks
(
   byte *nak_rounds_fwdp,                /* Num of rounds for forward NAKs */
   byte *naks_per_round_fwdp,            /* Num of NAKs per round forward  */
   byte *nak_rounds_revp,                /* Num of rounds for reverse NAKs */
   byte *naks_per_round_revp             /* Num of NAKs per round reverse  */
);  
/*===========================================================================
  FUNCTION      DSRLP_SET_ALL_DEF_NAKS
 
  DESCRIPTION   Set the number of rounds and the naks per round to be
                used when negotiating an RLP3 service option
 
  DEPENDENCIES  Can't lengthen the number of rounds beyond max_rounds. 
                Its assumed that the calling routine confirmed that this 
                change is done only for RLP3 call.
  
  RETURN VALUE  TRUE: If we were able to set the values.
                FALSE: None of the values are changed, since some parameter
                is having a wrong value.
  
  SIDE EFFECTS  Changes don't take effect until next SO negotiation
===========================================================================*/
  extern boolean dsrlp_set_all_def_naks
  (
     byte *nak_rounds_fwdp,              /* Num of rounds for forward NAKs */
     byte *naks_per_round_fwdp,          /* Num of NAKs per round forward  */
     byte *nak_rounds_revp,              /* Num of rounds for reverse NAKs */
     byte *naks_per_round_revp           /* Num of NAKs per round reverse  */
  );   
/*===========================================================================
  FUNCTION      DSRLP_GET_ALL_CUR_NAKS
 
  DESCRIPTION   Return the number of rounds and the naks per round currently
                in use (or last used) in an RLP3 service option call
  
  DEPENDENCIES  There must be sufficient space in the naks_per_round arrays
                to hold the returned data
                The passed in RSCB_INDEX is good. Also dsrlp_init must have 
                been invoked first for the returned SR_ID to be good.
  
  RETURN VALUE  Values are returned by argument pointers
  
  SIDE EFFECTS  None
  ===========================================================================*/
  extern boolean dsrlp_get_all_cur_naks
  (
     byte rscb_index,                      /* RLP instance                   */
     byte *nak_rounds_fwdp,                /* Num of rounds for forward NAKs */
     byte *naks_per_round_fwdp,            /* Num of NAKs per round forward  */
     byte *nak_rounds_revp,                /* Num of rounds for reverse NAKs */
     byte *naks_per_round_revp             /* Num of NAKs per round reverse  */
  );

/*===========================================================================
  FUNCTION      DSRLP_SET_CUR_NAKS
 
  DESCRIPTION   Set the number of rounds and the naks per round to be
                used in the current call, overiding the defaults
  
  DEPENDENCIES  Each NAK round must be less than or equal to what was
                specified as the default when the call was originated.
                The passed in RSCB_INDEX is good. Also dsrlp_init must have 
                been invoked first for the returned SR_ID to be good.
  
  RETURN VALUE  None
  
  SIDE EFFECTS  None
 ===========================================================================*/
  extern boolean dsrlp_set_all_cur_naks
  (
     byte rscb_index,                     /* RLP instance                   */
     byte *nak_rounds_fwdp,               /* Num of rounds for forward NAKs */
     byte *naks_per_round_fwdp,           /* Num of NAKs per round forward  */
     byte *nak_rounds_revp,               /* Num of rounds for reverse NAKs */
     byte *naks_per_round_revp            /* Num of NAKs per round reverse  */
  );

/*===========================================================================
  FUNCTION      DSRLP_GET_ALL_NEG_NAKS
 
  DESCRIPTION   This function will return the negotiated parameters. Note 
                that this function has to be called only for getting 
                the parameters. (we donot have a corresponding set_all_
                neg_naks function).
  
  DEPENDENCIES  The values returned here are valid only within a call.
  
  RETURN VALUE  Values are returned by argument pointers
                TRUE:  If we were able to get the values.
                FALSE: Some argument is wrong. So could not return values.
  
  SIDE EFFECTS  None
===========================================================================*/
  extern boolean dsrlp_get_all_neg_naks
  ( 
     byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
     byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
     byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
     byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
  );


#ifdef FEATURE_IS2000_REL_A

/*===========================================================================
  FUNCTION      DSRLP_BUILD_QOS_BLOB

  DESCRIPTION   This function creates a qos blob to be sent by mobile to BS

  DEPENDENCIES  Parameter passed in, ie array of bytes to hold blob should
                have enough space to hold blob.

  RETURN VALUE  Length of blob (0 if no blob needs to be sent, ie default)
                Blob is returned by argument pointer

  SIDE EFFECTS  None
===========================================================================*/
  extern byte dsrlp_build_qos_blob
  (
     byte *qos_blob_p                                    /* Resulting blob */
  );


/*===========================================================================
  FUNCTION      DSRLP_PROCESS_QOS_BLOB

  DESCRIPTION   This function processes the qos blob received from BS.

  DEPENDENCIES  None

  RETURN VALUE
              TRUE:  If we were able to get the values.
              FALSE: If blob didn't have qos params

  SIDE EFFECTS  The rx_from_bs Qos value struct is filled up
===========================================================================*/
  extern boolean dsrlp_process_qos_blob
  (
     const byte *qos_blob_p,                             /* Ptr to blob rx */
     byte blob_len
  );

/*===========================================================================
  FUNCTION      DSRLP_CALC_CURR_QOS_VALUES

  DESCRIPTION   This function calculates and stores current Qos values
                based on desired values requested and the values received
                from BS.

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_calc_curr_qos_values(void);

/*===========================================================================
  FUNCTION      DSRLP_GET_CURR_QOS_VALUES

  DESCRIPTION   This function returns current QOS values

  DEPENDENCIES  None

  RETURN VALUE  Current values of Qos  are returned as an argument pointer

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_get_curr_qos_values
  (
     dsrlp_qos_s_type *dsrlp_qos_s_p           /* Curr Qos values returned */
  );

/*===========================================================================
  FUNCTION      DSRLP_GET_DESIRED_QOS_VALUES

  DESCRIPTION   This function returns desired QOS values

  DEPENDENCIES  None

  RETURN VALUE  Desired values of Qos  are returned as an argument pointer

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_get_desired_qos_values
  (
     dsrlp_qos_s_type *dsrlp_qos_s_p        /* Desired Qos values returned */
  );

/*===========================================================================
  FUNCTION      DSRLP_SET_DESIRED_QOS_VALUES

  DESCRIPTION   This function sets desired Qos values

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_set_desired_qos_values
  (
     dsrlp_qos_s_type *dsrlp_qos_s_p                 /* Desired Qos values */
  );

/*===========================================================================
  FUNCTION      DSRLP_SET_DESIRED_QOS_NON_ASSUR_PRI

  DESCRIPTION   This function sets desired value of QOS non-assured mode
                priority adjustment

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_set_desired_qos_non_assur_pri
  (
     byte pri_adj_value            /* Desired value of priority adjustment */
  );

/*===========================================================================
  FUNCTION      DSRLP_GET_DESIRED_QOS_NON_ASSUR_PRI

  DESCRIPTION   This function gets desired value of QOS non-assured mode
                priority adjustment

  DEPENDENCIES  None

  RETURN VALUE  TRUE- If value of non-assured mode priority got successfully
                FALSE- If desired values not set, or if mode is assured
                Desired priority adjustment value returned as argument ptr.

  SIDE EFFECTS  None
===========================================================================*/
  extern boolean dsrlp_get_desired_qos_non_assur_pri
  (
     byte *pri_adj_value_p         /* Desired value of priority adjustment */
  );


/*===========================================================================
  FUNCTION      DSRLP_ARE_QOS_VALUES_SAME

  DESCRIPTION   This function takes as argument two qos structures and
                compares whether they are the same. (Eg: useful for
                checking if desired values equals default values)

  DEPENDENCIES  None

  RETURN VALUE
              TRUE:  If values are identical in both qos structures
              FALSE: If values are not identical

  SIDE EFFECTS  None
===========================================================================*/
  extern boolean dsrlp_are_qos_values_same
  (
     dsrlp_qos_s_type *dsrlp_qos_s_1_p,     /* Ptr to struct of qos params */
     dsrlp_qos_s_type *dsrlp_qos_s_2_p      /* Ptr to struct of qos params */
  );

/*===========================================================================
  FUNCTION      DSRLP_EVAL_QOS_BLOB

  DESCRIPTION   This function evaluates the qos blob rx from BS. For now,
                we donot do anything. So accept whatever the BS gives. 

  DEPENDENCIES  None

  RETURN VALUE
                SNM_ACCEPT_CFG: DS can accept anything that it receives from BS
                because, currently DS does not process it.  

  SIDE EFFECTS  
===========================================================================*/

extern snm_eval_status_type dsrlp_eval_qos_blob( byte   bs_blob_len,
                                          byte * bs_blob,
                                          boolean can_suggest_alt,
                                          byte * ret_alt_blob_len,
                                          byte * ret_alt_blob
                                         );

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================
  FUNCTION      DSRLP_IS_INITED

  DESCRIPTION   If RLP is already INIT'd, then returns TRUE.  Else returns
                FALSE.

                Takes an RLP instance as input.

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_is_inited
(
  byte rscb_index                        /* RLP instance                   */
);

/*===========================================================================
  FUNCTION      DSRLP_IS_RLP_ENCRYPT_ON

  DESCRIPTION   Tells whether RLP encryption should be on

  DEPENDENCIES  None

  RETURN VALUE  True/False 

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_is_rlp_encrypt_on ( void );

/*===========================================================================
  FUNCTION      DSRLP_IS_L2_L3_ENCRYPT_ON

  DESCRIPTION   Tells whether L2 or L3 encryption is on

  DEPENDENCIES  None

  RETURN VALUE  True/False

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_is_l2_l3_encrypt_on ( void );


/*===========================================================================

FUNCTION DSRLP_GET_THROUPUT_STATS

DESCRIPTION
  This function is  an API to get the RLP throuput info. exported to outside
  modules to query current RLP thput info. There is an ioctl provided to
  query this information. The icotl handler calls this function.

DEPENDENCIES
  None.

RETURN VALUE
   RX & TX thput values are returned.

SIDE EFFECTS
  None.

===========================================================================*/

void dsrlp_get_throuput_stats
(
  uint32 *rx_throuput,
  uint32 *tx_throuput
);

/*===========================================================================

FUNCTION DSRLP_ENABLE_QOS

DESCRIPTION
  This function is sets the rx_from_bs.qos_incl to TRUE.

DEPENDENCIES
  None.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_enable_qos(void);

/*===========================================================================

FUNCTION DSRLP_DISABLE_QOS

DESCRIPTION
  This function is sets the rx_from_bs.qos_incl to TRUE.

DEPENDENCIES
  None.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_disable_qos(void);

/*===========================================================================

FUNCTION dsrlp_register_snm_initializers

DESCRIPTION
  This function registers the RLP callback functions with SNM.

DEPENDENCIES
  None.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_IS2000_REL_A
snm_status_type dsrlp_register_snm_initializers(void);
#endif
#endif  /* RLP3 || HDR */

#ifdef FEATURE_HDR_QOS

/*===========================================================================

FUNCTION dsrlp_get_resv_list

DESCRIPTION
   Given SRID & ver num, returns teh reservation list, if any associated with
   that RLP.

DEPENDENCIES
  None.

RETURN VALUE
   Current reservation list, success/fail status

SIDE EFFECTS
  None.

===========================================================================*/
int dsrlp_get_resv_list
(
 dsrlp_rlp_identity_type   rlp,
 dsrlp_resv_list_type *resv_list
);

/*===========================================================================

FUNCTION dsrlp_update_resv_list

DESCRIPTION
   Given rscb index, updates the rsv_list corresponding to this rscb index
   with the new rsv list.

DEPENDENCIES
  None.

RETURN VALUE
   resets resv count to 0.

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_update_resv_list
(
 dsrlp_rlp_identity_type   rlp,
 dsrlp_resv_list_type  new_rsv_list
);

/*===========================================================================

FUNCTION DSRLP_GET_DROPPED_STALE_PACKETS

DESCRIPTION
   Returns if packets has been dropped for this RLP

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  Packets have been dropped
  FALSE: Packets have not been dropped  

SIDE EFFECTS
  Resets variable tracking dropping of packets

===========================================================================*/
void dsrlp_get_dropped_stale_packets
(
 dsrlp_rlp_identity_type   rlp,
 boolean                   *dropped
);

/*===========================================================================

FUNCTION DSRLP_GET_MARQ_INFO

DESCRIPTION This function clears the MARQ information for a given
            RLP after it is read. This allows the application to
            get the latest info about MARQ since it's last request.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Resets the boolean in the sess_ctl_blk of the rlp for which the query is 
  made.

===========================================================================*/
void dsrlp_get_marq_info
(
 dsrlp_rlp_identity_type   rlp,
 boolean                   *marq_dropped
);
#endif /* FEATURE_HDR_QOS */

/*===========================================================================
FUNCTION      dsrlp_get_rscb_cfg_ver

DESCRIPTION   This function returns the Config version type for a given RSCB
              Index.

DEPENDENCIES  NONE.

RETURN VALUE  Config Version Enum type for a particular RLP Flow

SIDE EFFECTS  NONE
===========================================================================*/
dsrlp_ver_enum_type dsrlp_get_rscb_cfg_ver
(
  byte index
);

/*===========================================================================
FUNCTION       DSRLP_GET_1XRLP_READY_FLAG

DESCRIPTION    Accessor method for is_1xrlp_ready flag

PARAMETERS
  None

RETURN VALUE   
  Returns the value of is_1xrlp_ready flag

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
boolean dsrlp_get_1xrlp_ready_flag(void);

/*===========================================================================
FUNCTION       DSRLP_SET_1XRLP_READY_FLAG

DESCRIPTION    Accessor method to set the is_1xrlp_ready flag

PARAMETERS
  None

RETURN VALUE   
  None

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
void dsrlp_set_1xrlp_ready_flag(boolean flag);


#endif  /* _DSRLP_V_H */


