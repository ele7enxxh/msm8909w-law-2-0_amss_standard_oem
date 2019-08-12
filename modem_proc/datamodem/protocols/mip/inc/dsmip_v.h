#ifndef DSMIP_V_H
#define DSMIP_V_H
/*===========================================================================

                         D S M I P  V . H

DESCRIPTION

 The Data Services Mobile IP configuration header file.

EXTERNALIZED DATA
   mip_ses_info_array       -  MIP session info array
   mip_fa_info              -  MIP foreign agent info
   mip_pwd_info             -  holds MIP shared secrets entered via UI
   mip_get_session_info_cb  -  callback registered by ui which provides
                               session info for next call
   mip_get_pwd_cb           -  callback registered by ui which provides
                               password info for next call

EXTERNALIZED FUNCTIONS
   mip_config_session()
     Load MIP session defaults from NV to all sessions.
   mip_reg_ses_info_cb()
     Register a callback to fill in MIP session information
   mip_reg_pwd_info_cb()
     Register a callback to fill in passwords for MIP session
   mip_get_rm_nai()
     Retrieves the NAIs for a given MIP profile for the
     tethered device and stores them in the mip session info.  

Copyright (c) 2000-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip.h_v   1.9   10 Oct 2002 15:54:16   jayanthm  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_v.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/25/13    sd     Boundary check for 707-mip profile variables.
06/11/13    sd     Fixing KW error.
06/03/13    sd     ATCoP decoupling changes
05/04/11    ttv    Added changes to compute mip statistics.
04/11/11    ttv    Added support for Hot Swap feature.
03/15/11    mg     Global variable cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
07/28/10    ttv    Merged the fix for the issue of not reading MIP active 
                   profile index from NV item NV_DS_MIP_ACTIVE_PROF_I during
                   call establishment.
11/17/09    ls     Added a function to deregister all the events
09/04/09    ss     CMI SU level modifications.
04/23/09    ls     Add the support to report MIP rrp code
08/01/08    mv/sa  Added support for DS NAA refresh.
12/10/07    as     Made MIP constants public with MOBILE_IP feature turned off
06/11/06    as     Added MIP_DLFT_PROFILE_NUM for use by otasp
12/07/06    as     Added new REGSM states, updated the rsminfo struct and
                   defined new mip events for MIP deregistration
11/13/06    ks     Added mip_get_rm_nai() to retrieve the RM_NAI from nv.
07/22/05    kvd    Add support for MIP event generation & report for DCTM;
                   mip_call_event_callback,mip_event_alloc,mip_event_register
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
06/22/03    kvd    Added  extern definition for new function mip_reset_config.
06/22/03    kvd    Added  extern definition for new function mip_config_valid.
04/28/04    jd     Added definitions for shared secret lengths, linted file
04/16/04    jd     MIP session keeps track of iface ptr instead of iface id
                   Moved MIP_MAX_SESSIONS into new dsmip_cfg.h
02/09/04    aku    Include dssocket_defs.h instead of ps_iface.h
02/02/04    jd     Remove app id from rsm_info, add iface to session info
06/25/03    jd     Mainlined FEATURE_DS_IS_835_B since all targets using this
                   archive enable the feature.
06/11/03    jd     Added RTT estimate to rsm_info
10/09/02    jay    Moved #defines from resgsm.h
08/16/02    jay    Created a bit field for various MIP related test AT
                   command
07/18/02    jay    Added DMU flag to the MIP session
04/29/02    jd     Created module
===========================================================================*/
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dsmip_cfg.h"
#include "dsmip.h"
#include "dsmip_api.h"

/*===========================================================================

                             PUBLIC CONSTANTS

===========================================================================*/

#define PRIMARY_HA_INDEX 0

#if MAX_HA_ADDRS > 1
#define SECONDARY_HA_INDEX 1
#endif /* MAX_HA_ADDRS > 1 */

#define MIP_RRQ_RETRY_BACKOFF_MULT 2 /* from Section 3.6.3 rfc2002         */

/*---------------------------------------------------------------------------
  Shared secret lengths are 16 bytes, corresponding to the following
  definitions in nv.h:

    MV_MAX_MN_HA_SHARED_SECRET_LEN
    MV_MAX_MN_AAA_SHARED_SECRET_LEN
---------------------------------------------------------------------------*/
#define MIP_MN_HA_SS_MAX_LEN   (16)
#define MIP_MN_AAA_SS_MAX_LEN  (16)
#define MIP_AUTH_CHAP_LEN  (0)                 /* RFC 3012 - ? bits?       */
#define MIP_AUTH_MD5_LEN   (16)                /* RFC 2002 - 128-bit auth. */

#define MIP_AUTH_MAX_LEN   (MIP_AUTH_MD5_LEN)  /* max of lengths above     */

#define QCMIP_VAL_UPPER_BOUND          2
#define QCMIP_VAL_LOWER_BOUND          0
#define QCMIPP_VAL_UPPER_BOUND         5
#define QCMIPP_VAL_LOWER_BOUND         0
#define QCMIPT_VAL_UPPER_BOUND         1
#define QCMIPT_VAL_LOWER_BOUND         0
#define QCMIPEP_VAL_UPPER_BOUND        1
#define QCMIPEP_VAL_LOWER_BOUND        0
#define QCMIPGETP_VAL_UPPER_BOUND      5
#define QCMIPGETP_VAL_LOWER_BOUND      0
#define QCMIPRT_PRI_VAL_UPPER_BOUND    1
#define QCMIPRT_PRI_VAL_LOWER_BOUND    0
#define QCMIPRT_SEC_VAL_UPPER_BOUND    1
#define QCMIPRT_SEC_VAL_LOWER_BOUND    0

/*---------------------------------------------------------------------------
  Primary value of QCMIPMASPI ranges from 0 to FFFFFFFF and hence no bound
  check for it.
---------------------------------------------------------------------------*/

#define QCMIPMASPI_SEC_VAL_UPPER_BOUND 1 
#define QCMIPMASPI_SEC_VAL_LOWER_BOUND 0 

/*---------------------------------------------------------------------------
  Primary value of QCMIPMHSPI ranges from 0 to FFFFFFFF and hence no bound
  check for it.
---------------------------------------------------------------------------*/
#define QCMIPMHSPI_SEC_VAL_UPPER_BOUND 1 
#define QCMIPMHSPI_SEC_VAL_LOWER_BOUND 0 

/*---------------------------------------------------------------------------
  qcmipt value is a bit field and can be used for testing various Mobile
  IP  features
  
  | QCMIPT | MN_HA_Auth_2002bis |  Clear_txt DMU |   
  ------------------------------------------------
     0           No                    No 
     1           Yes                   No 
     2           No                    Yes
     3           Yes                   Yes
---------------------------------------------------------------------------*/
#define MIP_QCMIPT_MN_HA_AUTH_2002BIS_BIT 0x01  /* RFC2002bis MN-HA Auth   */
#define MIP_QCMIPT_DMU_CLEARTEXT_BIT      0x02  /* clear text mode in DMU  */
#define MIP_QCMIPT_RSVD_03_BIT            0x04  /* reserved for future use */
#define MIP_QCMIPT_RSVD_04_BIT            0x08  /* reserved for future use */
#define MIP_QCMIPT_RSVD_05_BIT            0x10  /* reserved for future use */
#define MIP_QCMIPT_RSVD_06_BIT            0x20  /* reserved for future use */
#define MIP_QCMIPT_RSVD_07_BIT            0x40  /* reserved for future use */
#define MIP_QCMIPT_RSVD_08_BIT            0x80  /* reserved for future use */

#define MIP_EVENT_CLIENT_MAX                     2
#define MIP_RT_MAX                        2
#define MIP_MA_SPI_MAX                    2
#define MIP_MH_SPI_MAX                    2
#define MIP_HA_MAX                        16
#define MIP_PHA_MAX                       16
#define MIP_SHA_MAX                       16
#define MIP_NON_ARRAY                     0

#ifdef FEATURE_DS_MOBILE_IP
#include "ps_iface_defs.h"
#include "pstimer.h"

/*===========================================================================

                         PUBLIC DATA STRUCTURES

===========================================================================*/

/*---------------------------------------------------------------------------
TYPEDEF MIP_EVENT_E_TYPE

DESCRIPTION
  enum of PPP events that can be registered for.
---------------------------------------------------------------------------*/
typedef enum
{
  MIP_MIN_EV       = 0,
  MIP_SUCCESS_EV   = 0,
  MIP_FAILURE_EV   = 1,
  MIP_DEREGED_EV   = 2,
  MIP_MAX_EV,
} mip_event_e_type;

/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_MASK_E_TYPE

DESCRIPTION
  enum of PPP event mask that can be registered for
---------------------------------------------------------------------------*/
typedef enum
{
  MIP_INIT_EV_MASK = 0,
  MIP_FAILURE_EV_MASK = (1 << MIP_FAILURE_EV),
  MIP_SUCCESS_EV_MASK = (1 << MIP_SUCCESS_EV),
  MIP_DEREGED_EV_MASK = (1 << MIP_DEREGED_EV)
} mip_event_mask_e_type;

/*---------------------------------------------------------------------------
TYPEDEF MIP_EVENT_CBACK_F_PTR_TYPE

DESCRIPTION
  type of the callback function that is called when a MIP event occurs.
---------------------------------------------------------------------------*/
typedef void (*mip_event_cback_f_ptr_type)
(
  mip_event_e_type        mip_event,
  mip_fail_reason_e_type  reason,
  byte                    mip_rrp_code,
  void                    *user_data
);

/*---------------------------------------------------------------------------
  Type for MIP event information - allocated from ps_mem
---------------------------------------------------------------------------*/
typedef struct mip_event_info_s_type
{
  mip_event_cback_f_ptr_type  cback;
  void                       *user_data;
  mip_event_mask_e_type       event_mask;
} mip_event_info_s_type;

/*---------------------------------------------------------------------------
   Mobility Agent (foreign and/or home agent) Info Block
   - all sessions transmit use the same ("active") FA
---------------------------------------------------------------------------*/

typedef struct
{
  uint32   ma_addr;             /* IP address of mobility agent            */
  uint16   ma_lifetime;         /* registration lifetime granted by MA     */
  uint16   last_seq_num;        /* sequence number of last agent adv.      */
  byte     mn_AAA_SPI[4];       /* Security Parameter Index for AAA auth   */
  boolean  ma_supp_rev_tun;     /* MA supports reverse tunneling           */
  byte     ma_prefix_len;       /* subnet prefix len of ma_addr            */
  byte     challenge_length;    /* if 0 there was no challenge             */
  byte     fa_challenge[255];   /* the challenge itself                    */
} mip_ma_info_type;


/*---------------------------------------------------------------------------
  Structure to hold password info obtained via user interface.
---------------------------------------------------------------------------*/
typedef struct
{
  byte  mn_ha_ss[MIP_MN_HA_SS_MAX_LEN];    /* MN-HA shared secret          */
  byte  mn_aaa_ss[MIP_MN_AAA_SS_MAX_LEN];  /* MN-AAA shared secret         */
  byte  mn_ha_ss_len;                      /* MN-HA shared secret length   */
  byte  mn_aaa_ss_len;                     /* MN-AAA shared secret length  */
} mip_pwd_info_type;

/*---------------------------------------------------------------------------
  Structure to hold mip staticstics.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  rrp_cnt;
  uint32  rrq_cnt;
  uint32  sol_cnt;
  uint32  adv_cnt;
} mip_stats_type;

/*---------------------------------------------------------------------------
  Ident to identify the mip statistic.
---------------------------------------------------------------------------*/
typedef enum
{
  RRP_CNT_ID = 0x01,
  RRQ_CNT_ID = 0x02,
  SOL_CNT_ID = 0x03,
  ADV_CNT_ID = 0x04
} mip_stats_identifier;

typedef struct
{
  uint8      ds_qcmip_val;                /* holds uint8 */
  uint8      ds_qcmipp_val;               /* holds uint8 */
  uint8      ds_qcmipt_val;               /* holds uint8 */
  uint8      ds_qcmipep_val;              /* holds uint8 */
  uint8      ds_qcmipgetp_val;            /* holds uint8 */
  uint32     ds_qcmiprt_val[MIP_RT_MAX];  /* holds uint32, 2nd is commit */
  byte       ds_qcmipnai_val[MAX_NAI_LEN+1];
  byte       ds_qcmipmass_val[MIP_MN_AAA_SS_MAX_LEN+1];
  byte       ds_qcmipmhss_val[MIP_MN_HA_SS_MAX_LEN+1];
  byte       ds_qcmipmassx_val[MIP_MN_AAA_SS_MAX_LEN+1];
  byte       ds_qcmipmhssx_val[MIP_MN_HA_SS_MAX_LEN+1];
  uint32     ds_qcmipmaspi_val[MIP_MA_SPI_MAX];  /* holds uint32 */
  uint32     ds_qcmipmhspi_val[MIP_MH_SPI_MAX];  /* holds uint32 */
  byte       ds_qcmipha_val[MIP_HA_MAX+1];         /* holds string */
  byte       ds_qcmippha_val[MIP_PHA_MAX+1];       /* holds string */
  byte       ds_qcmipsha_val[MIP_SHA_MAX+1];       /* holds string */
  uint32     ds_qcmip_ha_num_val;                /* holds uint32 */
  uint32     ds_qcmip_pri_ha_num_val;            /* holds uint32 */
  uint32     ds_qcmip_sec_ha_num_val;            /* holds uint32 */
} dsmip_1x_profile;

typedef enum
{
  DS_QCMIP_VAL            = 0,
  DS_QCMIPP_VAL           = 1,
  DS_QCMIPT_VAL           = 2,
  DS_QCMIPEP_VAL          = 3,
  DS_QCMIPGETP_VAL        = 4,
  DS_QCMIPRT_VAL          = 5,
  DS_QCMIPNAI_VAL         = 6,
  DS_QCMIPMASS_VAL        = 7,
  DS_QCMIPMHSS_VAL        = 8,
  DS_QCMIPMASSX_VAL       = 9,
  DS_QCMIPMHSSX_VAL       = 10,
  DS_QCMIPMASPI_VAL       = 11,
  DS_QCMIPMHSPI_VAL       = 12,
  DS_QCMIPHA_VAL          = 13,
  DS_QCMIPPHA_VAL         = 14,
  DS_QCMIPSHA_VAL         = 15,
  DS_QCMIP_HA_NUM_VAL     = 16,
  DS_QCMIP_PRI_HA_NUM_VAL = 17,
  DS_QCMIP_SEC_HA_NUM_VAL = 18,
  DS_QCMIP_MAX_VAL
} dsmip_val_enum_type;

/*------------------------------------------------------------------------------
  Shadow NV
------------------------------------------------------------------------------*/
typedef struct
{
  struct                                    /* Shadow NAI                  */
  {
    byte  value[MAX_NAI_LEN];               /* NAI                         */
    byte  len;                              /* NAI Length                  */
  } nai;
  struct                                    /* Shadow MN-AAA shared secret */
  {
    byte  value[MIP_MN_AAA_SS_MAX_LEN];     /* MN-AAA Shared Secret        */
    byte  len;                              /* MN-AAA Shared Secret Length */
  } mn_aaa_ss;
  struct                                    /* Shadow MN-HA shared secret  */
  {
    byte  value[MIP_MN_HA_SS_MAX_LEN];      /* MN-HA Shared Secret         */
    byte  len;                              /* MN-HA Shared Secret Length  */
  } mn_ha_ss;
  struct                                    /* Shadow Reverse Tunneling    */
  {
    boolean  value;                         /* Reverse Tunneling toggle    */
    boolean  set;
  } rev_tun;
  struct                                    /* Shadow MN-AAA SPI           */
  {
    uint32   value;                         /* MN-AAA SPI                  */
    boolean  set;
  } mn_aaa_spi;
  struct                                    /* Shadow MN-HA SPI            */
  {
    uint32   value;                         /* MN-HA SPI                   */
    boolean  set;
  } mn_ha_spi;
  struct                                    /* Shadow Home address         */
  {
    uint32   value;                         /* Home IP address             */
    boolean  set;
  } home_addr;
  struct                                    /* Shadow Primary HA           */
  {
    uint32   value;                         /* Primary HA IP address       */
    boolean  set;
  } pri_ha_addr;
  struct                                    /* Shadow Secondary HA         */
  {
    uint32   value;                         /* Primary HA IP address       */
    boolean  set;
  } sec_ha_addr;
} dsmip_shadow_nv_type;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Callback to retrieve session information - if non-null, this callback
  will be called to have the UI modify any parameters it wants after 
  the MIP session information has been retrieved from NV
---------------------------------------------------------------------------*/
extern void (* mip_get_session_info_cb)( byte, mip_session_info_type * );

/*---------------------------------------------------------------------------
  Callback to retrieve shared secret information.  
  Externalized so dsmip_outmsg can check if a function is registered.
---------------------------------------------------------------------------*/
extern void (* mip_get_pwd_cb)( byte, mip_pwd_info_type * );



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIP_CONFIG_SESSION()

DESCRIPTION
  Load MIP session defaults from NV to all sessions. Note: this function
  be invoked before the traffic channel is established.

PARAMETERS
  mip_index - index specifies which profile paramters to be read from NV

RETURN VALUE
  TRUE: No errors encoutered
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_config_session 
( 
  int8 mip_index
);

/*===========================================================================
FUNCTION MIP_REG_PWD_INFO_CB()

DESCRIPTION
  Registers a user callback to provide MIP password info. 

PARAMETERS
  fn_ptr:  Pointer to user routine of the following signature:
           void fn_ptr( byte profile_num, mip_pwd_info_type * pwd_info )

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  The session info block settings may affect MIP negotiation
===========================================================================*/
void mip_reg_pwd_info_cb
(
  void (* callback)( byte profile_num, mip_pwd_info_type * pwd_info )
);

/*===========================================================================
FUNCTION MIP_CONFIG_VALID()

DESCRIPTION
  Check if MIP session is configured currently

PARAMETERS
  None

RETURN VALUE
  Flag  indicating config status of MIP

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mip_config_valid
(
  void
);
/*===========================================================================
FUNCTION MIP_RESET_CONFIG()

DESCRIPTION
  Reset mip configuration at end of MIP call, so that subsequent calls can
  do mip_config

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reset_config
(
  void
);

/*===========================================================================
FUNCTION MIP_EVENT_ALLOC()

DESCRIPTION
  This function will allocate a MIP event information structure and
  register callback functions.

PARAMETERS
  mip_cb_f:         callback function to call
  user_data:        user data to be returned with the callback fn.

RETURN VALUE
  NULL on error
  handle to the MIP event information structure on success

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a MIP event information structure and places it on the event
  queue.  Initializes event masks and registers callbacks for MIP protocol.
===========================================================================*/

void *mip_event_alloc
(
  mip_event_cback_f_ptr_type  mip_cb_f,
  void                        *user_data
);

/*===========================================================================
FUNCTION MIP_CALL_EVENT_CALLBACK()

DESCRIPTION
  Traverses the MIP event queue and calls the passed event.

  NOTE: This function TASKLOCKs inside and need not be called with TASKLOCK

  NOTE: The callback function should not dealloc the event notification
        structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Generates MIP events
===========================================================================*/
void mip_call_event_callback
(
  mip_event_e_type    event
);

/*===========================================================================
FUNCTION MIP_EVENT_REGISTER()

DESCRIPTION
  This function will add to an event mask for a  MIP session.
  It will  generate an UP event if the event mask is registering for an UP
  event and the UP event was previously not registerd for this protocol and
  the associated protocol is already up.

PARAMETERS
  handle      PPP event structure to add this event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Adds the passed event mask to the already registered event mask
===========================================================================*/
int32 mip_event_register
(
  void                  *handle,
  mip_event_mask_e_type  event_mask
);

/*===========================================================================
FUNCTION MIP_EVENT_DEREGISTER_ALL()

DESCRIPTION
  This function will deregister all events for a  MIP session.

PARAMETERS
  handle      deregister all events to this handle

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 mip_event_deregister_all
(
  void                  *handle
);

/*===========================================================================
FUNCTION MIP_EVENT_DEREGISTER()

DESCRIPTION
  This function will remove an event mask for a PPP protocol.

PARAMETERS
  handle      PPP event structure to add this event mask to
  protocol    Protocol to add the event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Removes the passed event mask from the already registered event mask
===========================================================================*/
int32 mip_event_deregister
(
  void                  *handle,
  mip_event_mask_e_type  event_mask
);

#ifdef FEATURE_DATA_PS_MIP_CCOA
/*===========================================================================
FUNCTION MIP_GET_CURR_HA_ADDR()

DESCRIPTION
  This function retrieves the current Home Address of the MIP session and
  returns it.

PARAMETERS
  void

RETURN VALUE
  uint32 - Current HA address

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 mip_get_curr_ha_addr
(
  void
);
#endif /* FEATURE_DATA_PS_MIP_CCOA */

#ifdef FEATURE_MMGSDI
/*==============================================================================
FUNCTION: DSMIP_READ_MIP_NV_ITEMS

DESCRIPTION:
  This function reads the MIP related NV Items NV_DS_MIP_RETRIES_I, 
  NV_DS_PRE_RE_RRQ_TIME_I and NV_DS_MIP_RETRY_INT_I that needs to be
  read during MMGSDI_CARD_INIT_COMPLETED. 
  All three are under the EF MMGSDI_CDMA_3GPD_MIPUPP which is call 
  stack related.

PARAMETERS
  void

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void dsmip_read_mip_nv_items
(
  void
);

/*==============================================================================
FUNCTION: DSMIP_RESET_MIP_NV_ITEMS

DESCRIPTION:
  This function reset the mip_ses_info_array information to zero.

PARAMETERS
  void

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void dsmip_reset_mip_nv_items(void);
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION MIP_GET_ACTIVE_PROFILE_INDEX_FROM_NVRUIM()

DESCRIPTION
  This function retrieves Mobile IP active profile index from NV/RUIM.

PARAMETERS
  None

RETURN VALUE
  -1 if MIP active profile index couldn't be read.
  Otherwise, the active profile index 0-5.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int8 mip_get_active_profile_index_from_nvruim
(
  void 
);

/*============================================================================
FUNCTION MIP_GET_SES_INFO_ARRAY

DESCRIPTION
  Access function for mip_ses_info_array.

PARAMETERS
  None

RETURN VALUE
  Pointer to mip_ses_info_array.

DEPENDENCIES
  None

============================================================================*/
mip_session_info_type* mip_get_ses_info_array(void);

/*============================================================================
FUNCTION MIP_GET_MA_INFO_PTR

DESCRIPTION
  Access function for ma_info.

PARAMETERS
  None

RETURN VALUE
  Pointer to ma_info.

DEPENDENCIES
  None

============================================================================*/
mip_ma_info_type* mip_get_ma_info_ptr(void);

/*============================================================================
FUNCTION MIP_GET_PWD_INFO_PTR

DESCRIPTION
  Access function for mip_pwd_info.

PARAMETERS
  None

RETURN VALUE
  Pointer to mip_pwd_info.

DEPENDENCIES
  None

============================================================================*/
mip_pwd_info_type* mip_get_pwd_info_ptr(void);

/*============================================================================
FUNCTION MIP_GET_MIP_STAT_INFO

DESCRIPTION
  Function to retrive the value form stat_val based on identifier value.

PARAMETERS
  None

RETURN VALUE
  uint32 - stat value.

DEPENDENCIES
  None
============================================================================*/
uint32 mip_get_mip_stat_info(mip_stats_identifier ident);

/*============================================================================
FUNCTION MIP_INCREMENT_MIP_STAT_INFO

DESCRIPTION
  Function to increment the particular stat value.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None
============================================================================*/
void mip_increment_mip_stat_info(mip_stats_identifier ident);

/*===========================================================================
FUNCTION      DSMIP_DIAG_INIT

DESCRIPTION   Registers functions with DIAG.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsmip_stats_init(void);

/*===========================================================================
FUNCTION MIP_GET_MA_ADDR()

DESCRIPTION   Get current Mobility Agent address of the MIP session

PARAMETERS    None

RETURN VALUE  uint32 - MA address
===========================================================================*/
uint32 mip_get_ma_addr
(
  void
);

/*===========================================================================
FUNCTION DS_QCMIP_UPDATE

DESCRIPTION
  Called by $QCMIP command and refresh event handler to update
  values associated with NV_DS_QCMIP_I.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if success
  FALSE: if failure

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_qcmip_update
(
  boolean read_flag
);

/*===========================================================================
FUNCTION DS_QCMIPP_UPDATE

DESCRIPTION
  Called by refresh event handler and $QCMIPP command execution
  to update values associated with NV_DS_MIP_ACTIVE_PROF_I.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if success
  FALSE: if failure

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_qcmipp_update
(
  boolean read_write_flag /* flag denoting write/read to NV is required or not*/
);

/*===========================================================================
FUNCTION    DS_QCMIP_GET_RUNTIME_VAL

DESCRIPTION
  This function will return the value of qcmip, from runtime profile structure.

DEPENDENCIES
  None

RETURN VALUE
  UINT8 : Value of ds_qcmip_val.

SIDE EFFECTS
  None
===========================================================================*/
uint8 ds_qcmip_get_runtime_val
( 
  void
);

/*===========================================================================
FUNCTION DS_QCMIP_SET_VAL

DESCRIPTION
  This function sets the QCMIP value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_qcmip_set_val
(
  uint32 qcmip_val 
);

/*===========================================================================
FUNCTION DSMIP_GET_SHADOW_NV_PTR

DESCRIPTION
  This functions returns the shadow pointer to interface with ATCOP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dsmip_shadow_nv_type* dsmip_get_shadow_nv_ptr
(
  void
);

/*===========================================================================
FUNCTION DSMIP_IS_USING_SHADOW_NV

DESCRIPTION
  Checks if any of the shadow NV values are in use for MIP.

  Called from PS so make sure no inter-task contention issues

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - using shadow NV
  FALSE - not using shadow NV

SIDE EFFECTS
  None
===========================================================================*/
boolean dsmip_is_using_shadow_nv
(
  void
);

/*===========================================================================
FUNCTION DSMIP_ERASE_SHADOW_NV

DESCRIPTION
  This functions flushes the shadow NV structure. It is called at the
  initilization ,when the active profile has been changed and
  when the call is ended.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmip_erase_shadow_nv
(
  void
);

/*===========================================================================
FUNCTION DS_INIT_MIP_PROF

DESCRIPTION
  This function initializes the MIP user profile and sets the default
  values.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - success
  FALSE - error

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_init_mip_prof
(
  void
);

/*===========================================================================
FUNCTION DSMIP_FILL_MIP_VALS

DESCRIPTION
  This function copies the values retreived from NV into dsmip_val which is
  used for display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmip_fill_mip_vals
(
  nv_stat_enum_type (* nv_get_fn)(nv_items_enum_type, nv_item_type *),
  nv_item_type      *nv_item
);

/*===========================================================================
FUNCTION DSMIP_GET_1X_PROFILE_VAL_NUM

DESCRIPTION
  This function will return value for the requested val of type num.
  The first parameter is enum type for the val, second parameter is
  the value pointer and the third parameter specifies the number of
  elements to be returned if the val is of array type and it should 
  be 0 if the val is of non-array type.
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE : Value of the corresponding val from static profile was 
         returned successfully.
  FALSE: Value of the corresponding val from static profile was 
         not returned successfully.

SIDE EFFECTS
  None
===========================================================================*/
boolean dsmip_get_1x_profile_val_num
(
  dsmip_val_enum_type        type,
  void                      *val,
  uint8                      num_elements
);

/*===========================================================================
FUNCTION DSMIP_GET_1X_PROFILE_VAL_STR

DESCRIPTION
  This function will fill the passed string buffer with the requested value
  and return whether the buffer is properly filled or not with the string in
  corresponding variable of static profile. 
  
  The first parameter is enum type for the val, second parameter is
  the string buffer pointer and the third parameter specifies the 
  length of the buffer.
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE  :  Value set in static profile.
  FALSE :  Value did not set.

SIDE EFFECTS
  None
===========================================================================*/
boolean dsmip_get_1x_profile_val_str
(
  dsmip_val_enum_type         type,
  char*                       str,
  uint32                      length
);

/*===========================================================================
FUNCTION DSMIP_SET_1X_PROFILE_VAL_NUM

DESCRIPTION
  This function will set members of ds707_mip_static_profile .
  The first parameter is enum type for the val, second parameter is
  the value pointer and the third parameter specifies the number of
  elements to be filled in the corresponding variable of static
  profile if the val is of array type and it should  be 0 if the val 
  is of non-array type.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - True : Success
            False: Failure

SIDE EFFECTS
  None
===========================================================================*/
boolean dsmip_set_1x_profile_val_num
(
  dsmip_val_enum_type         type,
  void                       *val,
  uint8                       num_elements
);

/*===========================================================================
FUNCTION DSMIP_SET_1X_PROFILE_VAL_STR
  
DESCRIPTION
  This function will copy the value in passed string buffer in the 
  corresponding string of static profile and return whether the string is
  properly copied or not with the string in corresponding variable of 
  static profile. 
  
  The first parameter is enum type for the val, second parameter is
  the string buffer pointer and the third parameter specifies the 
  length of the buffer.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - True : Success
            False: Failure

SIDE EFFECTS
  None  
===========================================================================*/
boolean dsmip_set_1x_profile_val_str
(
  dsmip_val_enum_type         type, 
  char*                       str,
  uint32                      length
);

#endif /* FEATURE_DS_MOBILE_IP */
#endif /* DSMIP_V_H */
