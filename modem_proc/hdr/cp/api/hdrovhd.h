#ifndef HDROVHD_H
#define HDROVHD_H

/*===========================================================================

          O V E R H E A D  M E S S A G E S   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Overheade Messages Protocol (OVHD).

Copyright (c) 2000 - 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrovhd.h_v   1.21   09 Jan 2003 11:01:10   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrovhd.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   vko     Fix compilation errors
07/24/15   vko     Skip earfcn for D2L reselection, if reselection failed with forbidden TA
                           reason
11/08/14   vko     DO CP to double check if D2L Reselection CH is supported before sending reselect
07/09/14   cnx     Support NV refresh without reset.
02/26/14   vko     FR 17879: Extended EARFCN
10/31/13   ukl     Keep NSET updated for system avoidance.
03/31/13   cnx     Support LTE deprioritization.
02/04/13   cnx     Replace PriorSession with UpdateUATI in some network.
02/04/13   cnx     Don't reset tunnel info if AT comes back to the same cell
                   after OOS.
01/14/13   ukl     Fixed the issue of not waking up for ONRL by default.
04/27/12   cnx     Avoid Reselection from eHRPD to LTE forbidden PLMNs.
04/20/12   cnx     Unsubcribe LTE_RRC_SERVICE_IND.
03/23/12   cnx     Support getting Pilot strength for CGI response.
03/08/12   wsh     Merged NLB support
12/05/11   cnx     Support hdrcp_config_info.conf.
10/10/11   cnx     Support LTE_RRC_SERVICE_IND.
09/24/11   cnx     Session maintainence for Optimized Handoff.
08/04/11   cnx     Supported  pre-regisration info from LTE.
07/27/11   cnx     Merged Optimized Handoff changes.
05/31/11   cnx     Added hdrovhd_get_subnet_mask();
                   Added hdrovhd_get_sp_msg_country_code();
                   Added hdrovhd_get_rutc_list_count().09/02/10   pba     Added support for eHRPD to LTE reselection phase 1
05/18/11   wsh     Added network load balancing support
09/02/10   pba     Added support for eHRPD to LTE reselection phase 1
11/12/09   sju     Added hdrovhd_get_preferred_channel().
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     CMI: updated included headers due to header split
02/10/09   sju     Added support for intra scc sleep.
12/23/08   pba     Support for Quick Idle Protocol
11/21/08   pba     Added events for OverheadCache enhancement feature.
09/17/08   pba     Added overhead cache enhancement. 
04/15/08   mpa     Added support for relB pilot gid/nset update + parsing.
12/14/07   mpa     Added hdrovhd_get_sector_is_mc_capable().
04/27/07   kss     Include support for max PN delay field in AP msg.
03/27/07   wsh     Added support for new prior session handling
01/08/07   yll     Added support for CSNA.
07/31/06   pba     Added SecondaryColorCode Support
07/18/06   dna     Support for RouteUpdateTriggerCode, to send RUP msg and 
                   also retry following max access probes, if necessary.
07/06/06   mpa     Added hdrovhd_suspend_and_sleep().
02/23/06   hal     Added accessor functions hdrovhd_get_sp_msg_location() and
                   hdrovhd_get_sp_msg_nset_info()
                   Removed hdrovhd_get_sp_msg()
11/21/05   hal     Added support for the AccessHashingChannelMask attribute
10/31/05   hal     Added support for the ExtendedChannelInformation fields 
10/06/05   dna     Parse new Release A fields in AccessParameters message
08/28/05   dna     (mpa for) Added hdrovhd_reparse_bc_msg().
07/11/05   dna     Keep entire channel list in cache.  Report cached sectors.
06/22/05   pba     Added hdrovhd_get_last_capsule_length_max()
01/26/05   hal     Converted all HDROVHD timers to use callbacks.
                   Added hdrovhd_process_timer().
12/20/04   pba     Fixed compiler warnings.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
02/27/04   mpa     Added hdrovhd_get_qc_link().
01/08/03   mpa     Fixed searcher dependencies
06/03/02   mpa     Mainlined FEATURE_HDR_7_4_DDR.
08/21/01   vas     DDR Changes added under FEATURE_HDR_7_4_DDR. Added Country
                   Code & Silence Params to SP msg structure. Added PowerStep
                   to AP msg structure. Removed FEATURE_IS856_NO_DDR
05/09/01   vas     Re-naming for consistency
04/24/01   vas     DDR Changes. Added capsule_length_max to AP msg.
                   Featurised under FEATURE_IS856_NO_DDR
04/24/01   vas     Correction to hdrovhd_get_sector_id
03/30/01   vas     Added hdrovhd_get_sector_id
03/28/01   vas     Added hdrovhd_get_subnet
03/27/00   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrerrno.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrind.h"
#include "hdrsrchtypes.h"
#include "hdraddr.h"
#include "lte_rrc_ext_msg.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "lte_rrc_irat_msg.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDR_MAX_SP_NEIGHBORS 32 /*(=2^5)NumNeighbors field of SP msg=5 bits*/
#define HDR_MAX_SP_CHANNELS 64 /*(=2^5)ChannelCount field of SP msg=5 bits + 
                                5 bit ExtendedChannelCount field of SP msg */
#define NUM_APERSIST  4         /* Corresponds to NOMPAPersist    */

#define HDROVHD_MAX_SEC_COLORCODES 7

#ifdef FEATURE_HDR_REVC
#define HDROVHD_SUBASET_PER_CHAN   HDRSRCH_ASET_MAX_SIZE
#endif /* FEATURE_HDR_REVC */

#define HDROVHD_HDR_TO_LTE_RESEL_ENABLED_FILENAME "/nv/item_files/modem/hdr/cp/ovhd/d2lresel"

/* LongTermOverheadCachePeriod and ShortTermOverheadCachePeriod - 
   implemantation specific default */
#define HDROVHD_PREF_DEFAULT_CACHE_PERIOD_FILENAME "/nv/item_files/modem/hdr/cp/ovhd/cacheperiod"

/* MCC list where prior session is not supported */
#ifdef FEATURE_LTE_TO_HDR_OH
#define HDROVHD_MCC_NO_PRIOR_SESSION_FILENAME "/nv/item_files/modem/hdr/cp/ovhd/mccnops"
#endif /* FEATURE_LTE_TO_HDR_OH */

typedef enum
{
  HDROVHD_CACHE_HIT = 0,
  HDROVHD_CACHE_MISS
} hdrovhd_ovhd_cache_find_result_type;

/*---------------------------------------------------------------------------
                           SECTORPARAMETERS MESSAGE 

  Following data structures are used to store the received SectorParameters
  messages.
---------------------------------------------------------------------------*/

/* Extension of the sys_hdr_channel_record_s_type type.  
   Also stores the paired reverse channel for non FDD-paired channels.

   Defined in hdrovhd.h (instead of sys.h) and built as a parallel array to
   keep the code structure / interfaces fairly intact. 

   Note that non-FDD paired channels are not supported in relB.
*/
typedef struct
{
  sys_hdr_chan_rec_mode_e_type mode[HDR_MAX_SP_CHANNELS];
  sys_channel_type chan[HDR_MAX_SP_CHANNELS];
  sys_channel_type rev_chan[HDR_MAX_SP_CHANNELS];
} hdrovhd_paired_chan_list_type;

typedef struct
{
  hdraddr_type sector_id;           /* 128 bit SectorID                     */
  hdrsrch_pilot_gid_type pilot_gid; /* Pilot Group */
  uint8  subnet_mask;               /* Subnet Mask                          */
  uint16 sector_sig;                /* Sector Signature                     */
  int32  latitude;                  /* Latitude                             */
  int32  longitude;                 /* Longitude                            */
  uint16 rup_radius;                /* Route Update Radius                  */
  uint8  leap_secs;                 /* Leap Seconds                         */
  int16  local_time_offset;         /* Local Time Offset                    */
  uint16 country_code;              /* Country Code                         */
  uint8  silence_duration;          /* Reverse Link Silence Duration        */
  uint8  silence_period;            /* Reverse Link Silence Period          */
  uint8  num_neighbors;             /* Number of Neighbors                  */
  hdrsrch_nset_pilot_type nset[HDRHAI_MAX_SP_NEIGHBORS];
                                    /* Record for neighbors of this sector  */
  uint8  channel_count;             /* Number of channels                   */
  uint8  extended_channel_count;    /* Number of extended channels          */
  uint8  unpaired_channel_count;    /* Number of unpaired channels          */
  hdrovhd_paired_chan_list_type channel_list; /* The full chan lst  */
  uint16 channel_hash_mask_list[HDR_MAX_SP_CHANNELS]; /* The hash mask lst  */
  sys_channel_type hash_channel;    /* Channel to which the message hashes  */
   /* Keep the hash_channel type as sys_channel_type since we can't hash over
      non FDD-paired channels.  If we ever do, then update this type + the 
      hashable channel list */
  
  uint16 route_update_trigger_code; /* RouteUpdateTriggerCode value         */
  uint8 route_update_trigger_max_age; /* units 2^(RUTMaxAge + 3) * 1.28s    */
  uint8 sec_color_code_count;
  uint8 sec_color_code_list[HDROVHD_MAX_SEC_COLORCODES];
  boolean ps_gaup_is_allowed;       /* does the AN support prior session GAUP */
  boolean is_sector_mc_capable;     /* Is the the current sector multicarrier 
                                       capable */
} hdrovhd_sp_msg_type;

/*---------------------------------------------------------------------------
  hdrovhd_sp_msg_location_type

  Struct used by the accessor function hdrovhd_get_sp_msg_location() to
  return the most recently stored latitude and longitudes.
---------------------------------------------------------------------------*/
typedef struct
{
  int32 latitude;
  int32 longitude;
  uint16 rup_radius;
} hdrovhd_sp_msg_location_type;

/*---------------------------------------------------------------------------
  hdrovhd_sp_msg_nset_info_type

  Struct used by the accessor function 
  hdrovhd_get_sp_msg_pilot_gid_and_nset_info() to return the most recently 
  stored neighbor set information.
---------------------------------------------------------------------------*/
typedef struct
{
  hdrsrch_nset_pilot_type nset[HDRHAI_MAX_SP_NEIGHBORS];
  sys_channel_type channel_list[HDR_MAX_SP_CHANNELS];
  uint8 num_neighbors;
  uint8 channel_count;
} hdrovhd_sp_msg_nset_info_type;

/*---------------------------------------------------------------------------
                           ACCESSPARAMETERS MESSAGE 

  Following data structures are used to store the received AccessParameters
  messages.
---------------------------------------------------------------------------*/

typedef struct
{
   uint8  access_cycle_duration;  /* Access Cycle Duration                 */
   uint16 access_sig;            /* Access Signature                       */
   uint8  nominal_pwr;           /* Nominal Power                          */
   int16  probe_init_adjust;     /* Probe Initial Adjust                   */
   uint8  probe_num_step;        /* Max Number Probes in 1 probe sequence  */
   uint8  power_step;            /* Power Step                             */
   uint8  preamble_len;          /* Preamble Length                        */
   uint8  capsule_length_max;    /* Max number of frames in access capsule */
   uint8  apersistence[NUM_APERSIST];
   uint8  enhanced_access_parameters_included;/* EnhancedAParamsIncluded   */ 
   uint8  preamble_length_slots;           /* PreambleLengthSlots          */
   uint8  access_offset;                   /* AccessOffset                 */
   uint8  sector_access_max_rate;          /* SectorAccessMaxRate          */
   uint8  probe_time_out_adjust;           /* ProbeTimeOutAdjust           */
   uint8  pilot_strength_nominal;          /* PilotStrengthNominal         */
   uint8  pilot_strength_correction_min;   /* PilotStrengthCorrectionMin   */
   uint8  pilot_strength_correction_max;   /* PilotStrengthCorrectionMax   */
   uint8  max_pn_randomization_delay_included;/*EnhPNRandomizationIncluded */
   uint8  max_pn_randomization_delay;
} hdrovhd_ap_msg_type;

#ifdef FEATURE_HDR_REVC
typedef enum
{
  HDROVHD_NLB_CACHE_INVALID,
  HDROVHD_NLB_CACHE_QC_RCVD,
  HDROVHD_NLB_CACHE_SP_RCVD
} hdrovhd_nlb_cache_state_enum_type;

/* LoadInfoMsg requires a matching SP msg to provide neighbor info
   This cache holds unpacked neighborlist from SP */
typedef struct
{
  hdrovhd_nlb_cache_state_enum_type state;
  time_type timestamp;

  sys_link_type serving_sector;
  uint8 color_code;
  uint32 sector24;
  uint16 sector_sig;

  uint8 neighbor_cnt;
  sys_link_type neighbors[HDRHAI_MAX_SP_NEIGHBORS];

  /* FW only needs neighbors that are part of ASET, since
     ASET do not change often, we can keep a record to avoid
     sending FW all the neighbors */
  uint8 aset_cnt;

  /* index into neighbors[] array */
  uint8 aset_neighbors[HDROVHD_SUBASET_PER_CHAN];
} hdrovhd_nlb_cache_type;
#endif /* FEATURE_HDR_REVC */

/*----------------------------------------------------------------------------
                        OVERHEAD INFORMATION CACHE

  Up to 20 SectorParameters messages are stored. This is an arbitrarily chosen
  cache size to be consistent with IS95( in which 20 sectors are cached.) 
-----------------------------------------------------------------------------*/
#define HDROVHD_MAX_CACHE_RECORDS 20 

/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/
/*=============================================================================
FUNCTION HDROVHD_POWERUP_INIT

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon system startup.  The QuickConfig & SectorParameters supervision 
  timers are initialized, the protocol state is set to Inactive and various 
  variables set to their initial values.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrovhd_powerup_init( void );

/*=============================================================================
FUNCTION HDROVHD_INIT

DESCRIPTION
  This function initializes the protocol each time HDR mode is entered.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrovhd_init( void );

/*===========================================================================

FUNCTION HDROVHD_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to OVHD.

DEPENDENCIES
  None.
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrovhd_msg_cb
(
  dsm_item_type *msg_ptr
);

/*===========================================================================

FUNCTION HDROVHD_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to OVHD.

DEPENDENCIES
  None.
  
PARAMETERS
  ind_name - Indication to process, is a global enum for all possible 
             indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrovhd_ind_cb
(
  hdrind_ind_name_enum_type ind_name, 
  void *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDROVHD_ACTIVATE

DESCRIPTION
  This function commands OVHD to activate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrovhd_activate
( 
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDROVHD_DEACTIVATE

DESCRIPTION
  This function commands OVHD to deactivate and stop processing overhead 
  messages.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrovhd_deactivate
( 
  hdrhai_protocol_name_enum_type sender
);

/*=============================================================================

FUNCTION HDROVHD_WAKEUP

DESCRIPTION
  This function commands OVHD to Wakeup from sleep & start processing the 
  overhead messages.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrovhd_wakeup
( 
  hdrhai_protocol_name_enum_type sender
);

/*=============================================================================

FUNCTION HDROVHD_SLEEP

DESCRIPTION
  This function commands OVHD to sleep.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrovhd_sleep
( 
  hdrhai_protocol_name_enum_type sender 
);

/*=============================================================================

FUNCTION HDROVHD_SUSPEND_AND_SLEEP

DESCRIPTION
  This function commands OVHD to suspend its timers then go to sleep.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrovhd_suspend_and_sleep
( 
  hdrhai_protocol_name_enum_type sender 
);

/*=============================================================================

FUNCTION HDROVHD_RETURN_TO_IDLE

DESCRIPTION
  This function commands OVHD to return to PROCESS_ALL_MSGS state and mark
  the overhead as invalid to ensure we re-hash to the correct channel, among
  other things.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrovhd_return_to_idle( hdrhai_protocol_name_enum_type sender );

#ifdef FEATURE_HDR_BCMCS
/*=============================================================================

FUNCTION HDROVHD_REPARSE_BC_MSG

DESCRIPTION
  This function commands OVHD to re-parse the BroadcastOverhead message.
  This is necessary when the set of desired flows changes.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrovhd_reparse_bc_msg( hdrhai_protocol_name_enum_type sender );

#endif /* FEATURE_HDR_BCMCS */

/*=============================================================================

FUNCTION HDROVHD_AN_REDIRECTED

DESCRIPTION
  This function commands OVHD to return AN redirected indication.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrovhd_an_redirected( hdrhai_protocol_name_enum_type sender );

/*=============================================================================

FUNCTION HDROVHD_CHECK_CONFIGURATION

DESCRIPTION
  This function commands OVHD to check whether the overhead information 
  is still valid.

DEPENDENCIES
  None.

PARAMETERS
  config_change - ConfigurationChange field values from QP message.
  link_info - Link on which the QP message was received.
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void hdrovhd_check_configuration
( 
  uint8 config_change,
  hdrsrch_link_id_type  link_id,
  sys_link_type link_info,
  hdrhai_protocol_name_enum_type sender 
);

/*=============================================================================

FUNCTION HDROVHD_GET_AP_MSG

DESCRIPTION
  This function is used to access the current AccessParameters message.
  If the AccessParameters are not current & the AccessParameters supervision
  timer is not already running, it starts a the timer.

DEPENDENCIES
  Access Parameters should be current when calling this function.
  The input parameter ap_msg_ptr should point to valid memory, ie.,
  memory for storing the AccessParameters message should be allocated before
  calling this function.
  
PARAMETERS
  ap_msg_ptr - Pointer to allocated memory where the current AcccessParamters
  message will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NOT_ALLOWED - If AccessParameters are not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrovhd_get_ap_msg
( 
  hdrovhd_ap_msg_type *ap_msg_ptr,
  hdrhai_protocol_name_enum_type caller
);
/*=============================================================================

FUNCTION HDROVHD_GET_QC_LINK

DESCRIPTION
  This function is used to retrieve the link id of the current qc message.

DEPENDENCIES
  None

PARAMETERS
  link_id_ptr - Pointer to allocated memory where the link info
  will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the no qc message has been received yet

SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrovhd_get_qc_link
(
  sys_link_type *link_info_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================

FUNCTION HDROVHD_GET_COLOR_CODE

DESCRIPTION
  This function is used to retrieve the color code of the current sector.

DEPENDENCIES
  This function only returns a value when overhead information is current.
  
PARAMETERS
  color_code_ptr - Pointer to allocated memory where the color code 
  will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has not been received yet
  
SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrovhd_get_color_code
( 
  uint8 *color_code_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================

FUNCTION HDROVHD_GET_SUBNET

DESCRIPTION
  This function is used to retrieve the Subnet of the current sector.

DEPENDENCIES
  None
  
PARAMETERS
  subnet_ptr - Pointer to allocated memory where the subnet will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has changed and we have not been received yet
              the new subnet from the SP message yet.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrovhd_get_subnet
( 
  hdraddr_type subnet,
  hdrhai_protocol_name_enum_type caller
);

/*============================================================================

FUNCTION HDROVHD_GET_SUBNET_MASK

DESCRIPTION
  This function is used to retrieve the Subnet mask of the current sector.

DEPENDENCIES
  None

PARAMETERS
  subnet_mask - Pointer to allocated memory where the subnet mask will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has changed and we have not been received yet
              the new subnet from the SP message yet.

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_subnet_mask
(
  uint8 *subnet_mask,
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================

FUNCTION HDROVHD_GET_SECTOR_ID

DESCRIPTION
  This function is used to retrieve the 128 bit SectorID of the current sector

DEPENDENCIES
  None

PARAMETERS
  sector_id - Pointer to allocated memory where the sector id will get copied.
  subnet_mask_ptr - Pointer to where the subnet_mask will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - if overhead parameters are not current, so the current
               128 bit sector id is unknown

SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrovhd_get_sector_id
(
  hdraddr_type sector_id,
  uint8 * subnet_mask_ptr,
  hdrhai_protocol_name_enum_type caller
);


/*=============================================================================

FUNCTION HDROVHD_GET_SECTOR_ID2

DESCRIPTION
  This function is used to retrieve the 128 bit SectorID and subnet mask
  from the overhead information, even if it is not marked as being current.
  If E_SUCCESS is returned, the SectorID given is guaranteed to be from the
  same subnet as the link (pilot PN and channel) passed in, but is not
  necessarily from the same exact sector.

DEPENDENCIES
  None.
  
PARAMETERS
  sector_id - Pointer to allocated memory where the sector id will get copied.
  subnet_mask_ptr - Pointer to where the subnet_mask will get copied.
  link_ptr - Pointer to information on the current link
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - if the given link info is not current, so the current
               128 bit sector id is unknown
  
SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrovhd_get_sector_id2
(
  hdraddr_type sector_id,
  uint8 * subnet_mask_ptr,
  sys_link_type *link_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================

FUNCTION HDROVHD_GET_SECTOR_ID_24

DESCRIPTION
  This function is used to retrieve the lowest 24 bits of the sector ID in
  the last received quick config message.

DEPENDENCIES
  This function only returns a value when overhead information is current.
  
PARAMETERS
  sector_id_24_ptr - Pointer to allocated memory where the lowest 24 bits of 
  the current sector's sector ID will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the sector ID has not been received yet
  
SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrovhd_get_sector_id_24
( 
  uint32 *sector_id_24_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================

FUNCTION HDROVHD_GET_CACHED_SECTORS

DESCRIPTION
  This function returns a list of what sectors are in the cache.

DEPENDENCIES
  None

PARAMETERS
  sector_list_ptr - Where to store the sector list
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  Number of sectors currently in the cache

SIDE EFFECTS
  None.
=============================================================================*/
extern uint8 hdrovhd_get_cached_sectors
(
  sys_link_type *sector_list_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================

FUNCTION HDROVHD_INFO_IS_UPDATED

DESCRIPTION
   This function returns TRUE if the overhead info has been updated, otherwise
   it returns FALSE.

DEPENDENCIES
  None.
  
PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  - If overhead info has been updated
  FALSE - otherwise.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern boolean hdrovhd_info_is_updated
( 
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================

FUNCTION HDROVHD_AP_IS_UPDATED

DESCRIPTION
  This function returns TRUE if the AccessParameters have been updated, 
  otherwise it returns FALSE. If the AccessParameters have not been updated
  & the  AccessParameters supervision timer is not already running, it starts a
  the timer.

DEPENDENCIES
  None.
  
PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  - If AccessParameters have been updated
  FALSE - otherwise.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern boolean hdrovhd_ap_is_updated
( 
  hdrhai_protocol_name_enum_type caller 
);

/*=============================================================================

FUNCTION HDROVHD_IS_WAITING_TO_UPDATE_AP

DESCRIPTION
  This function returns whether AT needs to force 426 ms sleep to update AP.

DEPENDENCIES
  None.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  - AP is not updated and AP update timer has not expired.
  FALSE - otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
extern boolean hdrovhd_is_waiting_to_update_ap
( 
  hdrhai_protocol_name_enum_type caller 
);

/*=============================================================================
FUNCTION HDROVHD_PROCESS_TIMER

DESCRIPTION
  This function is an entry point for hdrmc to process the timer message
  that has been queued up.

DEPENDENCIES
  None.
  
PARAMETERS
  timer_id - The id of the expired timer triggering the function.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrovhd_process_timer
(
  uint32 timer_id
);

/*=============================================================================

FUNCTION HDROVHD_GET_LAST_CAPSULE_LENGTH_MAX

DESCRIPTION
  This function returns the most recent capsule length max to allow upper
  layers to make a "best estimate" of the largest message size that can be
  sent on the current access channel.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  last_capsule_length_max - most recent capsule_length_max field value

SIDE EFFECTS
  None

=============================================================================*/
extern uint8 hdrovhd_get_last_capsule_length_max
(
  void
);

/*===========================================================================

FUNCTION HDROVHD_GET_SP_MSG_LOCATION

DESCRIPTION
  This function returns the latitude and longitude of the most recently
  received Sector Parameters message.

DEPENDENCIES
  The input parameter location_ptr should point to allocated memory.

PARAMETERS
  location_ptr - A pointer to the struct that will hold the returned values.
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  E_SUCCESS - If the operation is successful.
  E_NOT_ALLOWED - If the Control Channel is not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrovhd_get_sp_msg_location
(
  hdrovhd_sp_msg_location_type *location_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*===========================================================================

FUNCTION HDROVHD_GET_SP_MSG_COUNTRY_CODE

DESCRIPTION
  This function returns the country code of the most recently
  received Sector Parameters message.

DEPENDENCIES
  The input parameter location_ptr should point to allocated memory.

PARAMETERS
  country_code_ptr - A pointer to the memory that will hold the returned values.
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  E_SUCCESS - If the operation is successful.
  E_NOT_ALLOWED - If the Control Channel is not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrovhd_get_sp_msg_country_code
(
  uint16 *country_code_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*===========================================================================

FUNCTION HDROVHD_GET_SP_MSG_PILOT_GID_AND_NSET_INFO

DESCRIPTION
  This function returns the values of nset, num_neighbors, channel_list
  and num_channels of the most recently received Sector Parameters message.

DEPENDENCIES
  The input parameter nset_info_ptr should point to allocated memory.

PARAMETERS
  pilot_gid_ptr - returns the pilot group ID for the serving sector
  nset_info_ptr - A pointer to the struct that will hold the returned values.
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  E_SUCCESS - If the operation is successful.
  E_NOT_ALLOWED - If the Control Channel is not current.
  E_NOT_AVAILABLE - If the buffer to be read is already in use.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrovhd_get_sp_msg_pilot_gid_and_nset_info
(
  uint8 *pilot_gid_ptr,
  hdrovhd_sp_msg_nset_info_type *nset_info_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================

FUNCTION HDROVHD_REGISTRATION_SUCCESS

DESCRIPTION
  This function updates the RouteUpdateTriggerCodeList entry to reflect
  that the RouteUpdate message was sent successfully.

DEPENDENCIES

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None.

=============================================================================*/
extern void hdrovhd_registration_success( void );

/*=============================================================================
FUNCTION HDROVHD_GET_SECONDARY_COLOR_CODES

DESCRIPTION
  This function returns the count and current list of SecondaryColorCodes.

DEPENDENCIES
  None.
  
PARAMETERS
 sec_cc_count_ptr - Count of SecondaryColorCode returned in the "sec_cc_list".
 sec_cc_list  - List of SecondaryColorCodes

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrovhd_get_secondary_color_codes
(
  uint8 *sec_cc_count_ptr,
  uint8 *sec_cc_list
);


/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_GET_CURR_RUTC

DESCRIPTION
  This function returns the current RouteUpdateTriggerCode.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  This function returns the current RouteUpdateTriggerCode.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern uint16 hdrovhd_get_curr_rutc ( void );

/* <EJECT> */
/*=============================================================================
FUNCTION HDROVHD_GET_RUTC_LIST_COUNT

DESCRIPTION
  This function returns number of RouteUpdateTriggerCode in the list.

DEPENDENCIES
  None.
  
PARAMETERS
  None

RETURN VALUE
  The number of RouteUpdateTriggerCode in the list.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern int hdrovhd_get_rutc_list_count ( void );

/*=============================================================================
FUNCTION HDROVHD_GET_SP_PS_GAUP_IS_ALLOWED

DESCRIPTION
  This function returns if AN supports prior session GAUP.

DEPENDENCIES
  None.
  
PARAMETERS
 caller  - Calling protocol

RETURN VALUE
  TRUE - if priorsession GAUP flag is set in SectorParam
  FALSE - Otherwise
  
SIDE EFFECTS
  None.
=============================================================================*/
extern boolean hdrovhd_get_sp_ps_gaup_is_allowed
(
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================

FUNCTION HDROVHD_GET_SECTOR_IS_MC_CAPABLE

DESCRIPTION
  This function is used to query from the SP message whether the current
  sector is MC capable.

DEPENDENCIES
  None
  
PARAMETERS
  sector_is_mc_capable_ptr - OUT whether the sector is multicarrier capable
  caller - The protocol calling this externalized function (for sw trace)
  
RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the information is not available at this time
  
SIDE EFFECTS
  None.
=============================================================================*/
extern hdrerrno_enum_type hdrovhd_get_sector_is_mc_capable
( 
  boolean *sector_is_mc_capable_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*=============================================================================
FUNCTION HDROVHD_QP_IS_OK_TO_PROCESS

DESCRIPTION
  This function checks whether the received QP message can be processed. 
  In non-idle handoff case if the command is ignored AT will stay awake 
  to process the QC message. In idle handoff case if the command is ignored
  overhead updated flag will be set to not updated which will force same
  behavior as the case when QP message is not received.

DEPENDENCIES
  None

PARAMETERS
  qp_link_ptr  - Link on which the QP msg was received.

RETURN VALUE
  TRUE  - If it is okay to continue processing configuration change cmd.
  FALSE - Otherwise

SIDE EFFECTS
  None

=============================================================================*/
extern boolean hdrovhd_qp_is_ok_to_process
(
  sys_link_type *qp_link_ptr
);

/*=============================================================================
FUNCTION HDROVHD_SET_QC_IS_RECEIVED

DESCRIPTION
   This function set if the QuickConfig message has been received or not 

DEPENDENCIES
  None.
  
PARAMETERS
  qc_is_received - if QC message has been received or not
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrovhd_set_qc_is_received
( 
  boolean qc_is_received
);

/*=============================================================================
FUNCTION HDROVHD_QC_IS_RECEIVED

DESCRIPTION
   This function returns TRUE if the QuickConfig message has been received after 
   the phone enters Monitor State, otherwise, it returns FALSE.

DEPENDENCIES
  None.
  
PARAMETERS
  None
  
RETURN VALUE
  TRUE - QuickConfig message has been received
  FALSE -   otherwise
  
SIDE EFFECTS
  None.
=============================================================================*/
extern boolean hdrovhd_qc_is_received( void );

/*=============================================================================
FUNCTION HDROVHD_GET_PREFERRED_CHANNEL 

DESCRIPTION
   This function is used to get the list of channles on which the AT prefers 
   to be assigned a Traffic Channel  
                                     
DEPENDENCIES
  None.

PARAMETERS
  pref_chan_count_ptr - returns the number of preferred channels 
  pref_chan_list_ptr - returns the preferred bc idle channel if it exists
  max_pref_chan_count - max number of preferred channels 
                
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrovhd_get_preferred_channel
(
  uint8 *pref_chan_list_cnt,
  sys_hdr_channel_record_s_type *pref_chan_list_ptr,
  uint8 max_pref_chan_count
);

#ifdef FEATURE_HDR_TO_LTE
/*=============================================================================
FUNCTION HDROVHD_GET_OTHER_RAT_TX_CYCLE

DESCRIPTION
  This function returns the other RAT TX cycle.

DEPENDENCIES
  None.
  
PARAMETERS
  None
  
RETURN VALUE
  Other RAT TX Cycle value included in the QC message
  
SIDE EFFECTS
  None.
=============================================================================*/
uint8 hdrovhd_get_other_rat_tx_cycle( void );

/*=============================================================================

FUNCTION HDROVHD_ORNL_IS_UPDATED

DESCRIPTION
  This function returns TRUE if the OtherRATNeighborList have been updated,
  otherwise it returns FALSE. 

DEPENDENCIES
  None.

PARAMETERS
  Caller - protocol calling this externalized function

RETURN VALUE
  TRUE  - If ORNL have been updated
  FALSE - otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_ornl_is_updated( hdrhai_protocol_name_enum_type caller );

/*=============================================================================

FUNCTION HDROVHD_GET_PLMNIDS_ASSOCIATED_WITH_EARFCN

DESCRIPTION
  This function returns the PLMNIDs associated with EARFCN. 

DEPENDENCIES
  None.

PARAMETERS
  earfcn - Frequency of the LTE cell.
  req_plmnid - Matching PLMNID to be returned.

RETURN VALUE
   Number of PLMNIDs that are associated with the given EARFCN.

SIDE EFFECTS
  None.
=============================================================================*/
uint8 hdrovhd_get_plmnids_associated_with_earfcn
(   
  lte_earfcn_t earfcn,
  sys_plmn_id_s_type *req_plmnid
);

/*=============================================================================

FUNCTION HDROVHD_CEHCK_OK_TO_RESEL_TO_LTE

DESCRIPTION
  This function checks if it is ok to resel to LTE. 

DEPENDENCIES
  None.

PARAMETERS
  d2l_rat_pri_list - the RAT priority list to be given in reselecton command
                     if D2L reselection is allowed.

RETURN VALUE
  TRUE is ok to resel to LTE. Otherwise FALSE.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_check_ok_to_resel_to_lte
( 
  sys_rat_pri_list_info_s_type *d2l_rat_pri_list,
  lte_earfcn_t earfcn
);

/*=============================================================================

FUNCTION HDROVHD_UPDATE_LTE_FEARFCN

DESCRIPTION
  This function updates LTE forbidden EARFCN list. 

DEPENDENCIES
  None.

PARAMETERS
  earfcn - EARFCN to be added to forbidden list
  bar_time - time for which this EARFCN has to be barred in seconds

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_update_lte_fearfcn
(
   lte_earfcn_t earfcn, 
   uint16 bar_time_s
);

#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_REVC
/* <EJECT> */
/*=============================================================================

FUNCTION HDROVHD_GET_NLB_ASET_NEIGHBORS

DESCRIPTION
  This function populates pn[] and pos[] array with ASET members
  from NLB cache

DEPENDENCIES
  hdrovhd_init should have been called before calling this function.

PARAMETERS
  serving_sector - sector cache is received on
  neighbor_cnt   - number of ASET neighbors returned
  pn             - array to be filled with neighbor PNs
  pos            - ASET's position in SP neighborlist, needed to match
                   LoadInfo msg

RETURN VALUE
  E_SUCCESS      - If success
  E_NO_DATA      - If there is no NLB cache for on given serving sector

SIDE EFFECTS
  None

=============================================================================*/
errno_enum_type hdrovhd_get_nlb_aset_neighbors
( 
  const sys_link_type* serving_sector,
  uint8 *neighbor_cnt,
  sys_pilot_pn_type pn[HDROVHD_SUBASET_PER_CHAN],
  uint8 pos[HDROVHD_SUBASET_PER_CHAN]
);
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
/*=============================================================================

FUNCTION HDROVHD_UPDATE_SIB8_INFO

DESCRIPTION
  This function updates the SIB8 info received from LTE. 

DEPENDENCIES
  None.

PARAMETERS
  sib8_info - Pointer to latest SIB8 info.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_update_sib8_info 
( 
  lte_rrc_hdr_sib8_info_ind_s *sib8_info, 
  boolean give_sib8_ind 
);

/*=============================================================================

FUNCTION HDROVHD_UPDATE_PREREG_INFO

DESCRIPTION
  This function updates the pre-regisration info received from LTE. 

DEPENDENCIES
  None.

PARAMETERS
  prereg_info - Pointer to latest pre-registration info.
  give_prereg_ind - Whether to give pre-registration update indication. FALSE if
                    HDR is inactive. TRUE when HDR is online.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/

void hdrovhd_update_prereg_info 
( 
  lte_rrc_hdr_pre_reg_info_ind_s *prereg_info, 
  boolean give_prereg_ind 
);

/*=============================================================================
FUNCTION HDROVHD_UPDATE_LTE_CELL_INFO

DESCRIPTION
  This function updates the lte cell info received from LTE. 

DEPENDENCIES
  None.

PARAMETERS
  cell_id_ptr - Pointer to allocated memory where the lte cell id will
                get copied.

RETURN VALUE
  E_SUCCESS - cell id is initialized
  E_FAILURE - otherwise

SIDE EFFECTS
  None.
=============================================================================*/
hdrerrno_enum_type hdrovhd_get_lte_cell_id 
( 
  lte_rrc_global_cell_id_t *cell_id_ptr 
);


/*=============================================================================
FUNCTION HDROVHD_PREREG_IS_ALLOWED

DESCRIPTION
  This function tells whether pre-registration is allowed or not in tunnel mode.

DEPENDENCIES
  None.

PARAMETERS
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  TRUE - pre-registration is allowed 
  FALSE - pre-registration is not allowed

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_prereg_is_allowed 
( 
  hdrhai_protocol_name_enum_type caller
);


/*=============================================================================
FUNCTION HDROVHD_PREV_PREREG_IS_ALLOWED

DESCRIPTION
  This function tells whether the previous pre-registration is allowed or not
  in tunnel mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - previous pre-registration is allowed 
  FALSE - previous pre-registration is not allowed

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrovhd_prev_prereg_is_allowed( void );


/*=============================================================================

FUNCTION HDROVHD_TUNNEL_DEACTIVATE

DESCRIPTION
  This function commands OVHD to exit tunnel state.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrovhd_tunnel_deactivate( hdrhai_protocol_name_enum_type sender );

/*=============================================================================

FUNCTION HDROVHD_SET_PRE_REG_IS_ALLOWED

DESCRIPTION
  This function tells sets pre-registration is allowed or not in tunnel mode.

DEPENDENCIES
  None.

PARAMETERS
  pre_reg_is_allowed - whether pre-reg is allowed or not.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_set_pre_reg_is_allowed 
( 
  boolean pre_reg_is_allowed
);


/*===========================================================================
FUNCTION HDROVHD_IS_SIB8_UPDATED

DESCRIPTION
  This function tells whether sib8 or preregistration info has been received.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrovhd_is_sib8_updated( void );

/*===========================================================================
FUNCTION HDROVHD_SET_WAITING_PREREG_AFTER_OOS

DESCRIPTION
  This function sets the flag of waiting_prereg_after_oos.
  
DEPENDENCIES
  None.

PARAMETERS
  value - the boolean value to be set.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrovhd_set_waiting_prereg_after_oos
( 
  boolean value
);
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */
/*===========================================================================

FUNCTION HDROVHD_GET_CGI_PILOT_STRENGTH

DESCRIPTION
  This function finds out pilot strength for CGI response.

DEPENDENCIES
  None

PARAMETERS
  pilot_pn - pilot PN

RETURN VALUE
  the strength of the PN

SIDE EFFECTS
  None

===========================================================================*/
int16 hdrovhd_get_cgi_pilot_strength
( 
  uint16 pilot_pn
);

/*=============================================================================

FUNCTION HDROVHD_UPDATE_LTE_FPLMN

DESCRIPTION
  This function updates LTE forbidden PLMN list. 

DEPENDENCIES
  None.

PARAMETERS
  num_fplmn - Number of FPLMNs in the list.
  fplmn - pointer to the FPLMN list.
  is_online - whether HDR is online.

RETURN VALUE
  None

SIDE EFFECTS
  None.
=============================================================================*/
void hdrovhd_update_lte_fplmn 
( 
  uint32 num_fplmn,
  sys_plmn_id_s_type *fplmn,
  boolean is_online
);

/*===========================================================================

FUNCTION:  hdrovhd_lte_rrc_plmn_to_sys_plmn_id

===========================================================================*/
/*!
    @brief
    This function converts from the RRC PLMN ID format (1 BCD digit per byte)
    to the Sys PLMN_id_T format (2 BCD digits per byte).

    @return
    PLMN ID converted to PLMN_id_T format (3 byte array - 2 BCD digits/byte)

 */
/*=========================================================================*/
sys_plmn_id_s_type hdrovhd_lte_rrc_plmn_to_sys_plmn_id
( 
  lte_rrc_plmn_s plmn_id  /*! RRC PLMN format */
);

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
/*=============================================================================
FUNCTION HDROVHD_UPDATE_SYS_AVOID_LIST

DESCRIPTION
  This function reads system avoidance list from SD and updates
  system avoidance timer

DEPENDENCIES
  This function must be called from the same task as HDROVHD.
  
PARAMETERS
  num_channels - Max number of entries in avoided_chan_rec
  avoided_chan_rec - num_channels entries of avoided channels
  
RETURN VALUE
  Number of avoided channels read from SD.
  
SIDE EFFECTS
  None.
=============================================================================*/
uint8 hdrovhd_update_sys_avoid_list
(
  uint8 max_channels,
  sys_avoided_channel_record_type* avoided_chan_rec
);
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

#endif  /* HDROVHD_H */
