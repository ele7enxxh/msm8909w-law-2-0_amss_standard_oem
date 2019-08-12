#ifndef DSMIP_API_H
#define DSMIP_API_H
/*===========================================================================

                 D A T A   S E R V I C E S   M O B I L E   I P

                    E X T E R N A L   H E A D E R   F I L E

DESCRIPTION

 The Data Services Mobile IP configuration external header file.

EXTERNALIZED FUNCTIONS
   mip_reg_ses_info_cb()
     Register a callback to fill in MIP session information
   mip_get_rm_nai()
     Retrieves the NAIs for a given MIP profile for the
     tethered device and stores them in the mip session info.
   mip_perf_delay()
     Sets start and end times for specified delay.
  mip_get_perf_info_ptr()
    Access funtion for mip_perf_info array.
   mip_meta_sm_init()
     Initialize the Meta State Machine.
   dmu_preencrypt_cmd_hdlr()
     Handle random data ready signal.
   mipio_handle_socket_event()
     Process socket event(s) on Mobile IP socket.

Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip.h_v   1.9   10 Oct 2002 15:54:16   jayanthm  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/api/dsmip_api.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/23/11    op     Data SU API cleanup
01/31/11    ttv    Removed feature wrapping in api files.
01/25/11   ss      SU API defeaturization and removal of customer.h.
09/04/09    ss     New file created for CMI SU level split.
10/15/09    vs     SU Level Api Defeaturization
===========================================================================*/
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#include "ps_iface_defs.h"
#include "pstimer.h"
#include "ps_svc.h"

/*===========================================================================

                             D S M I P

===========================================================================*/

/*===========================================================================
                         PUBLIC CONSTANTS
===========================================================================*/

/*---------------------------------------------------------------------------
   Maximum length of an NAI as per RFC 2794
---------------------------------------------------------------------------*/
#define MAX_NAI_LEN  (72)

/*---------------------------------------------------------------------------
   Max # of HA's
---------------------------------------------------------------------------*/
#define MAX_HA_ADDRS  (2)

/*===========================================================================
                        PUBLIC DATA STRUCTURES
===========================================================================*/


/*---------------------------------------------------------------------------
   Registration state machine states
---------------------------------------------------------------------------*/
typedef enum
{
  RSMI_MIN_STATE = -1,
  RSMI_NO_SESSION_STATE,             /* No Session state                   */
  RSMI_INIT_RRQ_STATE,               /* Initial RRQ state                  */
  RSMI_REGISTERED_STATE,             /* Registered state                   */
  RSMI_RE_RRQ_STATE,                 /* Re-RRQ state                       */
  RSMI_DE_RRQ_STATE,                 /* De-registration RRQ state          */
  RSMI_DEREGISTERED_STATE,           /* De-registered state                */
                                     /* for future enhancements */
  RSMI_MAX_STATE
} rsmi_state_type;

/*----------------------------------------------------------------------------
   Registration state machine info
----------------------------------------------------------------------------*/
typedef struct
{
  rsmi_state_type state;             /* state machine's state              */
  uint32  pre_re_rrq_time;           /* pre-registration retry time (ms)   */
  uint32  re_rrq_timeout;            /* period between RRQ's (ms)          */
  ps_timer_handle_type life_timer;   /* Registration lifetime timer handle */
  ps_timer_handle_type reg_timer;    /* Registration timeout timer handle  */
  ps_timer_handle_type dereg_timer;  /* Deregistration timeout timer handle */
                                     /* RRQ holdoff (1s) timer handle      */
  ps_timer_handle_type rrq_holdoff_timer;          
  uint16  rtt_estimate;              /* Estimated RTT from RRQ -> RRP      */ 
  byte    rrq_cnt;                   /* retries sent                       */
  byte    max_rrqs;                  /* max number of rrqs                 */
  byte    max_derrqs;                /* max number of dereg rrqs           */
  boolean one_second_since_last_rrq; /* false when holdoff timer is active */
  boolean force_dormancy;            /* This boolean determines if the     */
                                     /* callis forced to go dormant on     */
                                     /* completion of registration         */
} rsm_info_type;

/*---------------------------------------------------------------------------

DESCRIPTION
  Enum describing reason for MIP failure
---------------------------------------------------------------------------*/
typedef enum mip_fail_reason_e_type
{
  MIP_FAIL_REASON_NONE              = 0,
  MIP_FAIL_REASON_PDSN_FAILURE      = 1,
  MIP_FAIL_REASON_SOL_TIMEOUT       = 2,
  MIP_FAIL_REASON_RRQ_TIMEOUT       = 3,
  MIP_FAIL_REASON_FA_FAILURE        = 4,
  MIP_FAIL_REASON_HA_FAILURE        = 5,
  MIP_FAIL_REASON_HA_ADMIN_PROHIBIT = 6,
  MIP_FAIL_REASON_UNKNOWN      = MIP_FAIL_REASON_NONE
} mip_fail_reason_e_type;

/*---------------------------------------------------------------------------
   Mobile IP session information - one per session
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_type *  iface_ptr;      /* pointer to interface MIP configures  */
  char nai[MAX_NAI_LEN];           /* Network Access Identifier            */

  rsm_info_type rsm_info;          /* registration state control info      */

  uint32  ha_addrs[MAX_HA_ADDRS];  /* Home Agent IP address array          */
  uint32  home_addr;               /* current home IP address              */
  uint32  curr_ccoa;               /* Current co-located care-of address   */
  uint32  home_addr_subnet_len;    /* current home IP address subnet len   */
  uint32  reg_id_msw;              /* ID sent with registration request    */
  uint32  reg_id_lsw;              /*  (second half of RRQ ID field)       */
  uint32  time_offset;             /* time difference between MN-HA        */
  uint32  mn_ha_spi;               /* SPI value for the MN-HA extension    */
  uint32  mn_aaa_spi;              /* SPI value for the AAA extension      */
  uint16  reg_lifetime;            /* reg lifetime: from RRP (seconds)     */
  uint16  max_reg_time;            /* maximum time spent trying to reg (ms)*/
  uint16  base_retry_interval;     /* the base interval for retries (ms)   */
  uint16  retry_interval_offset;   /* this offset is added to the base     */
                                   /*   interval for initial rrq           */
  byte    curr_ha_index;           /* index into the ha_addrs array        */
  byte    nai_len;                 /* length of NAI field above            */
  boolean mn_request_rev_tun;      /* MN rev tunneling requested?          */
  boolean rrq_if_tfrk;             /* Send RRQ only if some data has been  */
                                   /* transferred since last registration  */

  boolean valid;                   /* contents are valid (nv read ok)      */
  byte    profile_index;           /* current NV profile index             */
  boolean dmu;                     /* flag true when DMU in progress       */
  mip_fail_reason_e_type mip_fail_reason; /* reason for mip call fail      */
  byte    mip_rrp_code;            /* mobility agent response code         */
  byte    mip_event_client_cnt;    /* no. of clients for MIP events        */
} mip_session_info_type;


/*===========================================================================
                     PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION MIP_REG_SES_INFO_CB()

DESCRIPTION
  Registers a user callback to populate MIP session info control block.

PARAMETERS
  fn_ptr:  Pointer to user routine of the following signature:
           void fn_ptr( byte profile_num, mip_session_info_type * ses_info)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  The session info block settings may affect MIP negotiation
===========================================================================*/
void mip_reg_ses_info_cb
(
  void (* callback)( byte profile_num, mip_session_info_type * ses_info )
);

/*===========================================================================
FUNCTION MIP_GET_RM_NAI()

DESCRIPTION
  This function retrieves the NAIs for a given MIP profile for the
  tethered device and stores them in the mip session info.

PARAMETERS
  session: pointer to the mip session control block where info is stored
  profile: profile number associated with the mip session.

RETURN VALUE
  -1 if session information retrieved from NV is insufficient to make a call
  otherwise, the current profile no. that was passed in as argument.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sint15 mip_get_rm_nai
(
  mip_session_info_type *  session,
  int                      active_mip_profile
);

/*===========================================================================

                         D S M I P _ P E R F

===========================================================================*/

/*===========================================================================
                      PUBLIC DATA STRUCTURES
===========================================================================*/

/*---------------------------------------------------------------------------
  Enumerated delay types.
---------------------------------------------------------------------------*/
typedef enum
{
  PERF_MIN_DELAY = -1,
  PERF_UM_PPP_DELAY,              /* Um PPP delay                          */
  PERF_RM_PPP_DELAY,              /* Rm PPP delay                          */
  PERF_MIP_INIT_REG_DELAY,        /* mobile IP registration delay          */
  PERF_MIP_REREG_DELAY,           /* mobile IP re-registration delay       */
  PERF_TOTAL_CALL_SETUP_DELAY,    /* total call setup time                 */
  PERF_TOTAL_HANDOFF_DELAY,       /* total inter-PDSN handoff setup time   */
  PERF_MIP_DEREG_DELAY,           /* mobile IP de-registration delay       */
  PERF_MAX_DELAY
} mip_perf_delay_type;

/*---------------------------------------------------------------------------
  Enumerated time stamp types.
---------------------------------------------------------------------------*/
typedef enum
{
  PERF_MIN_TS = -1,
  PERF_START_TS,                  /* start time stamp                      */
  PERF_END_TS,                    /* end time stamp                        */
  PERF_CANCEL_TS,
  PERF_MAX_TS
} mip_perf_ts_type;

/*---------------------------------------------------------------------------
  Delay info block.
---------------------------------------------------------------------------*/
typedef struct
{
  qword ts_start;                 /* start time stamp for delay            */
  qword ts_end;                   /* end time stamp for delay              */
  uint32 delay_in_ms;             /* difference time stamp for delay in ms */
} mip_perf_ts_info_type;


/*===========================================================================
                   PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION MIP_PERF_DELAY

  DESCRIPTION
    This function manages the start timestamp and end time stamps, as well
    as the calculation of the elapsed time for the specified delay in the 
    performance info block.
    
  PARAMETERS
    ts_info: the time stamp info struct for the delay type
    delay_type: the delay type
    ts_type: type of timestamp to be set
  
  RETURN VALUE
    None
 
  DEPENDENCIES
    None

  SIDE EFFECTS
    The previously elapsed time in ts_info will be over-written.
===========================================================================*/
void mip_perf_delay
(
  mip_perf_ts_info_type * ts_info,
  mip_perf_delay_type delay_type,
  mip_perf_ts_type ts_type
);

/*===========================================================================
FUNCTION       MIP_GET_PERF_INFO_PTR

DESCRIPTION    Access method for mip_perf_info

PARAMETERS
  Delay type
 
RETURN VALUE   
  Pointer to mip_perf_info

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
mip_perf_ts_info_type *mip_get_perf_info_ptr
(
  mip_perf_delay_type delay_type
);

/*===========================================================================

                        D S M I P _ D M U

===========================================================================*/

/*===========================================================================
                   PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*============================================================================

FUNCTION DMU_PREENCRYPT_CMD_HDLR

DESCRIPTION
  Handle random data ready signal from
  i)  Security services at startup to indicate that random data is avaiable
      for random key generation
  ii) PS task if qcmipp is changed, so random keys can be re-generated (for
      safety, in case keys were captured from an incomplete DMU update with
      the previous profile during a key request spoof), public key for new
      profile is read and news keys encrypted.

PARAMETERS
  Ignored

RETURN VALUE
  TRUE - tells PS that signal handling is complete

DEPENDENCIES
  None

============================================================================*/
void dmu_preencrypt_cmd_hdlr
(
  ps_cmd_enum_type  cmd,                    /* Command from issuer          */
  void *            data_ptr                /* user data provided (ignored) */
);

/*===========================================================================

                        D S M I P _ M E T A S M

===========================================================================*/

/*===========================================================================
                   PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION MIP_META_SM_INIT()

DESCRIPTION
  This function initializes the meta state machine.

PARAMETERS
  None

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int mip_meta_sm_init
(
  void
);

/*===========================================================================

                             D S M I P _ I O

===========================================================================*/

/*===========================================================================
                   PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION MIPIO_HANDLE_SOCKET_EVENT

DESCRIPTION
  Process socket event(s) on Mobile IP socket specified in identifier

PARAMETERS
  identifier - socket on which event occurred
  event_mask - current event_mask for socket

RETURN VALUE
  None.

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
void mipio_handle_socket_event
(
  uint32  identifier,                            /* MIP socket identifier  */
  uint32  event_mask                             /* Event(s) that occurred */
);

#endif /* DSMIP_API_H */
