/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

            D A T A B A S E   E X T E R N A L   I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the routines necessary for all protocols to access and
  change their session configuration information. This includes support for
  restoring the prior session and for multiple personalities.
  
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  
  
  Copyright (c) 2005 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmdb.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/31/16   vko     Reduce excessive F3
05/09/16   vko     Reduce excessive F3 logging
05/06/16   vko     Reduce excessive F3 logging
07/09/14   cnx     Support NV refresh without reset.
04/07/14   vko     Fixed klocwork issues
01/15/14   vko     Fixed compiler warnings
02/05/14   sju     Added check that AN should send SCC only for restore 
                   prior session with more than 1 personality.
01/18/14   vko     Fixed not sending pre-reg status when NV67252 is 1
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
06/20/13   ukl     Reduce efs writes while writing hdr config file to efs.
06/12/13   ukl     Fixed the scmdb_delete for the efs compression.
05/22/13   ukl     Compress and decompress hdr efs file before storing and reading.
04/29/13   smd     Disable RevB code if FEATURE_HDR_REVB_SUPPORTED is not defined.
04/25/13   ukl     Change HDRSCMDB_PROT_SUB_N_STREAM_LOG_PKT_REV to version 1.
04/24/13   cnx     Only write KeepAlive NV on powerdown.
03/21/13   cnx     Write NV when device is powerdown/reset.
01/30/13   vko     Optimize write to EFS
03/01/13   cnx     Removed efs_sync in hdrscmdb_write_all_config_data.
04/06/12   cnx     Removed FEATURE_EHRPD_HRPD_FALLBACK feature.
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
01/27/11   cnx     Support pre-Registration status update.
12/02/11   cnx     Flush EFS after writing session conf to EFS.
11/28/11   cnx     Support NV for set preReg status.
10/28/11   pba     Optimized handoff phase 2 support
10/27/11   vlc     Fixed compiler errors.  
09/16/11   wsh     Reduce heap mem usage by moving memory back to static
09/01/11   cnx     Refeaturized 'query data's eHRPD capability'.
08/26/11   cnx     Query DS's eHRPD capability before send out SESSION CHANGED IND.
07/27/11   cnx     Merged Optimized Handoff changes.
06/03/11   cnx     Supported eHRPD network setting and eHRPD APN.
05/31/11   cnx     Added hdrscmdb_get_current_personality().
05/02/11   cnx     Reverted the changed made on 12/21/10 and 01/14/11.
02/24/11   cnx     Removed hdrscmdb_write_config_to_nv().
01/14/11   cnx     Featurized the change of "Flushed data to eMMC ... during resetting."
12/21/10   cnx     Flushed data to eMMC after all EFS operations during resetting.
06/21/10   cnx     Fixed compiler warnings.
01/26/10   wsh     Merged ZI memory reduction to main branch
10/30/09   pba     Added ehrpd support.
10/27/09   pba     Klockwork fixes.
08/19/09   pba     Featurization fix for SEC API
06/18/09   wsh     Fixed error related to SHA1 hash computation 
01/12/09   etv     Disable DTXMode after connection close or fail.
12/22/08   pba     Fixed memory corruption.
12/04/08   pba     Lint and Compiler warning fixes
09/18/08   wsh     AT always reopens session after power-cycle
09/08/08   etv     Added support for PHY module.
08/06/08   pba     Populate the inuse config of SCP subtypes as negotiated, 
                   do not replace hardlink with main personality subtype.
08/06/08   pba     Log SCP and AMP subtypes as hardlink by default.
07/28/08   pba     To avoid watchdog time out during hash computation register 
                   hdrmc wait routine with SEC module.
05/20/08   pba     Added hdrscmdb_set_amp_inconfig_to_inuse()
05/22/08   pba     Klockwork fixes.
09/24/07   wsh     Fixed AMP OVHD_INFO_UPDATED_IND handling
09/11/07   wsh     Removed lint high warnings
08/22/07   etv     Fixed high lint warnings.
08/16/07   wsh     Removed compiler and lint critical warnings
08/08/07   pba     Added hdrscmdb_set_pktapp_protocols_to_default().
                   Added hdrscmdb_set_csna_protocol_to_default().
07/24/07   pba     Allow stream protocol hardlinking even when MPA or EMPA or 
                   MMPA is negotiated.
06/19/07   cc      Added DOrB RMAC4 support.
05/22/07   sju     Added support for phase2 of AT-init GAUP of SlottedMode attribute. 
05/22/07   pba     If the config subtype and current subtype are different 
                   set the fallback value to default value of the attribute
                   in the config subtype.
04/26/07   sju     Store scmidle slotted mode info to SCMDB 
04/17/07   pba     Give HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS session change 
                   reason only when there is prior session restore.
04/13/07   pba     If AN switched the AT's personality during prior session 
                   restore give the new session change reason -
                   HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS.
03/27/07   wsh     Added support for new prior session handling
03/21/07   pba     Added hdrscmdb_negotiated_session_is_valid().
03/21/07   pba     Fixed potential bug attrib_verno wrap around.
03/06/07   wsh     Removal of RVCT compiler warnings
02/21/07   wsh     Fixed lint warnings.
01/08/07   yll     Added support for CSNA
12/20/06   pba     Fixed a bug where GAUP of AMP attribute was not updating
                   the database correctly.
12/20/06   pba     Fixed a bug where AT was not updating attribute values
                   for hardink protocols in other personalities when main
                   personality was updated.
12/19/06   pba     Allow  only one personality at max to have non-default 
                   security layer protocol subtypes.
12/07/06   pba     When the protocol subtype changes during negotiation reset
                   the inconfig values to default.
10/30/06   pba     Inconfig value for SCP's subtype attribute should be the 
                   exact same as the inuse value.
10/30/06   pba     All AMP attributes should be hardlinked in all personalities.
10/17/06   pba     Restore LUP session information on successful prior session
                   restore.
10/16/06   pba     Ensure session database is valid before writing to EFS.
10/16/06   etv     Added support for LOG_HDR_SUBTYPE_STREAM_CONFIG_C log pkt.
10/10/06   pba     Fixed issue with prior session negotiation, when prior 
                   session has protocol's whose subtype is hardlink.
10/04/06   pba     Since now we write only during graceful powerdwon, removed
                   code that delays write to EFS on back-to-back GAUP.
08/01/06   sju     Added EVENT_HDR_SCM_SESSION_CHANGED event.
06/21/06   etv     Added hdrscmovhd module to support OMP config attribs.
06/14/06   etv     Add AT-init GAUP support for slotted Mode attribute.
06/20/06   pba     Removed redundant AN auth status maintained by SCP.
                   Added changes to update config lock value in SCMDB.
                   Config Lock is hardlinked attribute of SCP instead of per
                   personality.
06/13/06   pba     Do not look at upper 4 bits of SCT if there is only one 
                   personality.
                   Fixed an incorrect comparison statement in 
                   hdrscmdb_set_inconfig_to_inuse().
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
06/05/06   pba     Added support to store AMP, LUP, FCP and DOS session data
                   in EFS.
06/05/06   pba     Updated hdrscmdb_personality_is_valid() routine to also be
                   used for checking validity of prior session personalities.
05/24/06   pba     Added hdrscmdb_get_negotiated_app_subtypes() routine to 
                   return a list with the names of all the negotiated app 
                   subtypes itself instead of a bit mask.
05/23/06   etv     Added cast to handle compiler warning.
05/08/06   etv     Freed secapi resource after use.
05/03/06   pba     Added support to avoid too many back-to-back writes to EFS. 
04/28/06   etv     Added support to secure HDR session info.
04/27/06   pba     Added new API that updates SCMDB's copy of inuse PCCC value.
04/21/06   hal     Extended existing APIs to include session change and session
                   close reasons
04/17/06   pba     Added support for accepting/proposing only AT supported
                   stream configurations in multiple personality scenario.
03/30/06   pba     Added and modified some F3 messages.
03/22/06   pba     Added support for DeletePersonality.
03/07/06   pba     Updated F3 messages.
03/01/06   pba     Fixed initialization of AMP in hdrscmdb_reg table.
02/23/06   hal     Added support for configuring the AMP module
02/10/06   pba     Reset attribute verno only when personality is switched.
02/09/06   pba     Fixed typo in getting the protocol verno offset.
02/01/06   pba     Fixes for HardLink subtype negotiation.
01/14/06   dna     Upper 4 bits of SCT is _not_ the personality index if  there
                   is only one personality.
12/16/05   pba     Use the correct enum boundary for loop index
12/13/05   pba     Added support for multiple personality negotiation
12/06/05   etv     Added API to get SupportedCDMAChannels attrib from DB.
                   Added SCM support for AMAC, CMAC, and GMMCDP.
11/10/05   etv     Fixed bug in Find Changed NN attributes function.
11/07/05   pba     Added support to write only changed config parameters to EFS
                   Added version number handling in SCMDB for GAUP
                   Code cleanup and restructured SCMDB data structures
                   Added handle functions to store/retrieve configuration data
11/04/05   etv     Added function for updating the default values
11/02/05   etv     Changed SCM design to propogate subtype info.
10/26/05   hal     Added support for configuring the ALMP module
10/21/05   pba     Added Broadcast protocol to SCM
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
                   for legacy protocols
09/09/05   etv     Included RMAC into SCM support
09/06/05   etv     Changed the return type of get_attrib_offset_and_size func.
08/22/05   pba     Initial SCMDB Module support
07/18/05   dna     Created Module

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "fs_public.h"
#include "mobile.h"
#include "secapi.h"
#include "nv_items.h"
#include "stringl.h"
#include "modem_mem.h"
#ifdef FEATURE_HDR_EFS_COMPRESS
#include "zlib.h"
#endif /*FEATURE_HDR_EFS_COMPRESS*/
#include "hdrdebug.h"
#include "hdrkep.h"
#include "hdrauth.h"
#include "hdridlec.h"
#include "hdrrupc.h"
#include "hdrsmp.h"
#include "hdrscp.h"
#include "hdrstream.h"
#include "hdrlupc.h"
#if defined(FEATURE_HDR_QOS)
#include "hdrmrlpc.h"
#endif /*FEATURE_HDR_QOS*/
#include "hdramp.h"
#include "hdrmci.h"
#include "hdrutil.h"
#include "hdrlogi.h"
#include "hdrlog.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#include "hdrscm.h"
#include "hdrscmfmac.h"
#include "hdrscmrmac.h"
#include "hdrscmbccp.h"
#include "hdrscmalmp.h"
#include "hdrscmamac.h"
#include "hdrscmcmac.h"
#include "hdrscmgmmcdp.h"
#include "hdrscmrup.h"
#include "hdrscmamp.h"
#include "hdrscmidle.h"
#include "hdrscmovhd.h"
#include "hdrscmcsna.h"
#include "hdrscmphy.h"

#include "hdrdos.h"
#include "hdrfcp.h"
#include "hdrlup.h"
#include "amssassert.h"

#ifdef FEATURE_EHRPD
#include "hdrpac_api.h"
#endif /* FEATURE_EHRPD */

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*===========================================================================
            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Buffer memory size for compression */
#define HDRSCMDB_UNCOMPRESSED_BUF_MEMSIZE  2000

/* Size of the uncompressed hdr_protocol.z file */
#define HDRSCMDB_UNCOMPRESSED_MEMSIZE  (sizeof(hdrscmdb_session_db_status_enum_type) \
       + sizeof(hdrscmdb_build_info_type) + sizeof(hdramp_session_info_type) \
       + sizeof(hdrscp_hardlink_config_params_type) + sizeof(hdrlup_session_info_type) \
       + sizeof(hdrscmdb.personality) +  HDRSCMDB_UNCOMPRESSED_BUF_MEMSIZE)

#ifdef FEATURE_HDR_EFS_COMPRESS

#define HDRSCMDB_COMPRESSED_RATIO   30

/* Size of the compressed hdr_protocol.z file */
#define HDRSCMDB_COMPRESSED_MEMSIZE ( HDRSCMDB_UNCOMPRESSED_MEMSIZE / HDRSCMDB_COMPRESSED_RATIO)

#endif /*FEATURE_HDR_EFS_COMPRESS*/

/* PersonalityIndex of zero is AT's main personality */
#define HDRSCMDB_MAIN_PERSONALITY  0

/* Maximum length of build compile date string */
#define HDRSCMDB_VERNO_DATE_STRLEN 15

/* Maximum length of build compile time string */
#define HDRSCMDB_VERNO_TIME_STRLEN 15

/* Number of protocols registered with SCMDB */
#ifdef FEATURE_HDR_QOS
#define HDRSCMDB_NUM_REG_PROTOCOLS 22
#else
#define HDRSCMDB_NUM_REG_PROTOCOLS 21
#endif 

/* Hardlink subtype ID */
#define HDRSCMDB_HARDLINK_SUBTYPE  0xFFFE

#define HDRSCMDB_PROT_SUB_N_STREAM_LOG_PKT_REV    1

#define HDRSCMDB_DATA_INVALID 0xFFFFFFFF

/*----------------------------------------------------------------------------
  Structure of HDR configuration file in EFS.

                     |-----------------------------------------|
                     |              BUILD INFO                 |
                     |      < hdrscmdb_build_info_type >       |
                     |          hdrscmdb.build_info            |
                     |-----------------------------------------|
                     |               AMP INFO                  |
                     |       < hdramp_session_info_type >      |
                     |           hdrscmdb.amp_info             |
                     |-----------------------------------------|
                     |               SCP INFO                  |
                     | < hdrscp_hardlink_config_params_type >  |
                     |           hdrscmdb.scp_info             |
                     |-----------------------------------------|
                     |               LUP INFO                  |
                     |      < hdrlup_session_info_type >       |
                     |           hdrscmdb.lup_info             |
                     |-----------------------------------------|
                     |       PERSONALITY INDEX 0 CONFIG        |
                     | < hdrscmdb_protocol_config_data_type >  |
                     |        hdrscmdb.personality[0]          |
                     |-----------------------------------------|
                     |       PERSONALITY INDEX 1 CONFIG        |
                     | < hdrscmdb_protocol_config_data_type >  |
                     |        hdrscmdb.personality[1]          |
                     |-----------------------------------------|
                     |                                         |
                                     .........

                     |-----------------------------------------|
                     |       PERSONALITY INDEX N CONFIG        |
                     | < hdrscmdb_protocol_config_data_type >  |
                     |        hdrscmdb.personality[N]          |
                     |-----------------------------------------|
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
Build information datatype
----------------------------------------------------------------------------*/
typedef struct
{
  char build_date[HDRSCMDB_VERNO_DATE_STRLEN];
  char build_time[HDRSCMDB_VERNO_TIME_STRLEN];

}hdrscmdb_build_info_type;

/*----------------------------------------------------------------------------
Protocol configuration information datatype
----------------------------------------------------------------------------*/
typedef struct
{
  boolean  personality_is_valid;
    /* Flag to indicate if the personality is valid or not */
  hdrscmamac_config_struct_type              amac;
    /* HDRHAI_AC_MAC_PROTOCOL */
  hdrscmcmac_config_struct_type              cmac;
    /* HDRHAI_CC_MAC_PROTOCOL */
  hdrscmfmac_config_struct_type              fmac;  
    /* HDRHAI_FTC_MAC_PROTOCOL */
  hdrscmrmac_config_attrib_union_type        rmac; 
    /* HDRHAI_RTC_MAC_PROTOCOL */
  hdrkep_config_params_type                  kep;   
    /* HDRHAI_KEY_EXCHANGE_PROTOCOL */
  hdrauth_config_params_type                 auth;  
    /* HDRHAI_AUTHENTICATION_PROTOCOL */
  hdrscmalmp_config_params_type              almp;  
    /* HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL */
  hdridlec_config_params_type                idle;  
    /* HDRHAI_IDLE_STATE_PROTOCOL */
  hdrrupc_config_params_type                 rup;   
    /* HDRHAI_ROUTE_UPDATE_PROTOCOL */
  hdrsmp_config_params_type                  smp;   
    /* HDRHAI_SESSION_MANAGEMENT_PROTOCOL */
  hdrscmamp_config_params_type               amp;
    /* HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL */
  hdrscp_per_personality_config_params_type  scp;   
    /* HDRHAI_SESSION_CONFIG_PROTOCOL */
  hdrstream_config_params_type               stream; 
    /* HDRHAI_STREAM_PROTOCOL */
  hdrlupc_config_params_type                 lup;  
    /* HDRHAI_LOCATION_UPDATE_PROTOCOL */
#if defined(FEATURE_HDR_QOS)
  hdrmrlpc_config_params_type                mrlp; 
#endif /*FEATURE_HDR_QOS*/
    /* HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL*/
  hdrdos_config_struct_type                  dos;
    /* HDRHAI_DATA_OVER_SIGNALING_PROTOCOL */
  hdrfcp_config_struct_type                  fcp;
    /* HDRHAI_FLOW_CONTROL_PROTOCOL */
  hdrscmbccp_config_params_type              bccp; 
    /* HDRHAI_BROADCAST_PROTOCOL */
  hdrscmgmmcdp_config_struct_type            gmmcdp;
    /* HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL */
  hdrscmovhd_config_struct_type              ovhd;
    /* HDRHAI_OVERHEAD_MESSAGES_PROTOCOL */
  hdrscmcsna_config_struct_type              csna;
    /* HDRHAI_CIRCUIT_SERVICES_NOTIFICATION_PROTOCOL */
  hdrscmphy_config_struct_type               phy;
    /* HDRHAI_PHYSICAL_LAYER_PROTOCOL */

}hdrscmdb_protocol_config_data_type;

/*----------------------------------------------------------------------------
This structure is used to hold the inconfig instance of SCM protocols only.
Legacy protocols are excluded from this because they keep their inconfig 
instance locally.
----------------------------------------------------------------------------*/
typedef struct
{
  hdrscmamac_config_struct_type              amac;
    /* HDRHAI_AC_MAC_PROTOCOL */
  hdrscmcmac_config_struct_type              cmac;
    /* HDRHAI_CC_MAC_PROTOCOL */
  hdrscmfmac_config_struct_type              fmac;  
    /* HDRHAI_FTC_MAC_PROTOCOL */
  hdrscmrmac_config_attrib_union_type        rmac; 
    /* HDRHAI_RTC_MAC_PROTOCOL */
  hdrscmalmp_config_params_type              almp;  
    /* HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL */
  hdrscmamp_config_params_type               amp;
    /* HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL */
  hdrscmbccp_config_params_type              bccp; 
    /* HDRHAI_BROADCAST_PROTOCOL */
  hdrscmgmmcdp_config_struct_type            gmmcdp;
    /* HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL */
  hdrscmovhd_config_struct_type              ovhd;
    /* HDRHAI_OVERHEAD_MESSAGES_PROTOCOL */
  hdrscmcsna_config_struct_type              csna;
    /* HDRHAI_CIRCUIT_SERVICES_NOTIFICATION_PROTOCOL */
  hdrscmphy_config_struct_type               phy;
    /* HDRHAI_PHYSICAL_LAYER_PROTOCOL */

}hdrscmdb_inconfig_config_data_type;

/*-----------------------------------------------------------------------------
Protocol version number information datatype
-----------------------------------------------------------------------------*/
typedef struct
{
  hdrscmamac_verno_type       amac;
  hdrscmcmac_verno_type       cmac;
  hdrscmfmac_verno_type       fmac; 
  hdrscmrmac_verno_union_type rmac; 
  hdrscmalmp_verno_type       almp;
  hdrscmamp_verno_type        amp;
  hdrscmbccp_verno_type       bccp;
  hdrscmgmmcdp_verno_type     gmmcdp;
  hdrscmovhd_verno_type       ovhd;
  hdrscmcsna_verno_type       csna;
  hdrscmphy_verno_type        phy;
  /* --------------------------------------------------------------------------
     Version number information are needed only for protocols that use SCM for
     config request/response and GAUP message handling. Other protocols already
     have their current configuration hence it just needs to use SCMDB routines
     to store and restore from EFS.
   --------------------------------------------------------------------------*/

}hdrscmdb_protocol_attrib_verno_type;

/*-----------------------------------------------------------------------------
This structure has all the global data of SCMDB
-----------------------------------------------------------------------------*/
typedef struct 
{
  hdrscmdb_build_info_type            build_info;
   /* Build compile time and date */

  hdramp_session_info_type            amp_info;
   /* Address Management Protocol's session information it is hard linked for
      all personalities */

  hdrscp_hardlink_config_params_type  scp_info;
   /* Session Configuration Protocol is half way hardlink i.e. some attributes
      are same for all personalities, where as others - subtype and AN auth 
      status are per personality attributes */

  hdrlup_session_info_type            lup_info;
  /* Location and StorageBlob session parameters of LUP are hardlinked */

  hdrscmdb_protocol_config_data_type  personality[HDRSCMDB_MAX_PERSONALITIES];
   /* Negotiated configuration of all the protocols for MAX personalities */

   /*--------------------------------------------------------------------------
    HDR EFS configuration file stores build_info, amp_info, scp_info and
    MAX personalities that were succesfully negotiated.
   --------------------------------------------------------------------------*/

  hdrscmdb_protocol_config_data_type  inuse;
   /* Each protocol's config data that is currently being used, this would be
      same as the item at index "curr_personality" in personality array.
      amp_info, scp_info and inuse together for the AT's current config data */

  hdrscmdb_inconfig_config_data_type  inconfig;
   /* Temporary copy of protocol configuration, it is used during a given round
      of negotiation. After successfully negotiating a personality it is
      copied to "personality" array, the index is specified by 
      personalityIndexStore of SoftCC */

  hdramp_session_info_type            prev_amp_info;
   /* Address Management Protocol's session information it is hard linked for
      all personalities */

  hdrscp_hardlink_config_params_type  prev_scp_info;
   /* SCP's previous session configuration information for hardlink attribs */

  hdrlup_session_info_type            prev_lup_info;
  /* Previous Location and StorageBlob session parameters */

  hdrscmdb_protocol_config_data_type  previous[HDRSCMDB_MAX_PERSONALITIES];
   /* Each protocols previous session configuration for all personalities */

  hdrscmdb_protocol_attrib_verno_type version_num;
   /* Version number of protocol attributes for the current personality */

  uint16  curr_personality;
   /* AT's current personality index */

  uint16  file_write_counter;
   /* Counter for number of times the config file was opened for write */

  nv_cmd_type nv_cmd;
   /* Static instance of NV cmd for blocking NV write */

  nv_hdrscmdb_secure_config_type nv_sec_config;
    /* Secure Configuration items  */

  hdrscmdb_session_db_status_enum_type session_db_status;
  /* The session_db_status flag reflects the validity of the in-memory copies
     of session configuration data, previous[] and personality[]. At any
     time, at most one of the two is valid. The flag is not saved in EFS,
     and the saved session data is always assumed to be the current copy */

  hdrscmdb_inconfig_config_data_type protocol_default_cfg;
    /* Temp configuration structure of SCM protocol's used to find the
       default value of a given attribute */

  boolean efs_is_up_to_date;
    /* This variable is set to FALSE, if EFS DB and SCMDB are out of sync, and set to TRUE
       if they are in sync. Used for optimize EFS write */    
	   
} hdrscmdb_type;

static hdrscmdb_type hdrscmdb;

/* Access to protocol config data within SCMDB data structures is done through
   handle functions, which treats config data as a byte array and returns a 
   byte pointer (hdrscmdb_config_handle_type) to start of config data. Treating
   the structure as a byte array provides a convenient way to offset into the
   structure to get the given protocol's config data */
typedef byte* hdrscmdb_config_handle_type;

/*----------------------------------------------------------------------------
                     S C M D B     L O O K U P     T A B L E
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 Following are the function prototypes of the callbacks that each protocol 
 registers with SCMDB for various purposes
-----------------------------------------------------------------------------*/

/* This callback function is for SCM protocols to provide offset of the
   attribute in protocol's config structure and its size */
typedef hdrerrno_enum_type (*hdrscmdb_get_attrib_offset_and_size_type)
(
  uint16  attrib_id,
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16  subtype
);

/* This callback function is for SCM protocols to provide index of the 
   attribute version number in protocol's version number structure */
typedef hdrerrno_enum_type (*hdrscmdb_get_attrib_verno_index_type)
(
  uint16  attrib_id,
  uint16 *index,
  uint16  subtype
);

/* This callback function is for SCM and legacy protocols, to provide a
   pointer to it's inconfig instance */
typedef void* (*hdrscmdb_get_inconfig_handle)
(
  void
);

/* This function sets the specified config instance (inconfig or inuse 
   instance) of a given protocol to default values, according to the 
   specified subtype */
typedef void (*hdrscmdb_set_config_to_default)
( 
  void  *cfg_ptr,
  uint16 subtype
);

static __inline void* hdrscmdb_get_amac_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.amac;
} /* hdrscmdb_get_amac_inconfig_handle() */

static __inline void* hdrscmdb_get_cmac_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.cmac;
} /* hdrscmdb_get_cmac_inconfig_handle() */

static __inline void* hdrscmdb_get_fmac_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.fmac;
} /* hdrscmdb_get_fmac_inconfig_handle() */

static __inline void* hdrscmdb_get_rmac_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.rmac;
} /* hdrscmdb_get_rmac_inconfig_handle() */

static __inline void* hdrscmdb_get_almp_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.almp;
} /* hdrscmdb_get_almp_inconfig_handle() */

static __inline void* hdrscmdb_get_amp_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.amp;
} /* hdrscmdb_get_amp_inconfig_handle() */

static __inline void* hdrscmdb_get_bccp_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.bccp;
} /* hdrscmdb_get_bccp_inconfig_handle() */

static __inline void* hdrscmdb_get_gmmcdp_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.gmmcdp;
} /* hdrscmdb_get_gmmcdp_inconfig_handle() */

static __inline void* hdrscmdb_get_ovhd_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.ovhd;
} /* hdrscmdb_get_ovhd_inconfig_handle */

static __inline void* hdrscmdb_get_csna_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.csna;
} /* hdrscmdb_get_csna_inconfig_handle */

static __inline void* hdrscmdb_get_phy_inconfig_handle( void )
{
  return (void*)&hdrscmdb.inconfig.phy;
} /* hdrscmdb_get_phy_inconfig_handle */

static void hdrscmdb_amac_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmamac_set_config_to_default(cfg_ptr);
} /* hdrscmdb_amac_set_config_to_default() */

static void hdrscmdb_cmac_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmcmac_set_config_to_default(cfg_ptr);
} /* hdrscmdb_cmac_set_config_to_default() */

static void hdrscmdb_fmac_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmfmac_set_config_to_default(cfg_ptr); 
} /* hdrscmdb_fmac_set_config_to_default() */

static void hdrscmdb_rmac_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmrmac_set_config_to_default(cfg_ptr, subtype); 
} /* hdrscmdb_rmac_set_config_to_default() */

static void hdrscmdb_kep_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrkep_set_config_to_default(cfg_ptr);
} /* hdrscmdb_kep_set_config_to_default() */

static void hdrscmdb_auth_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrauth_set_config_to_default(cfg_ptr);
} /* hdrscmdb_auth_set_config_to_default() */

static void hdrscmdb_almp_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
   hdrscmalmp_set_config_to_default(cfg_ptr);
} /* hdrscmdb_almp_set_config_to_default() */

static void hdrscmdb_idle_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdridlec_set_config_params_default(cfg_ptr);
} /* hdrscmdb_idle_set_config_to_default() */

static void hdrscmdb_rup_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrrupc_set_config_to_default(cfg_ptr);
} /* hdrscmdb_rup_set_config_to_default() */

static void hdrscmdb_smp_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrsmp_set_config_to_default(cfg_ptr);
} /* hdrscmdb_smp_set_config_to_default() */

static void hdrscmdb_amp_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmamp_set_config_to_default(cfg_ptr);
} /* hdrscmdb_amp_set_config_to_default() */

static void hdrscmdb_scp_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscp_set_per_personality_attribs_to_default(cfg_ptr);
} /* hdrscmdb_scp_set_config_to_default() */

static void hdrscmdb_gmmcdp_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmgmmcdp_set_config_to_default(cfg_ptr);  
} /* hdrscmdb_gmmcdp_set_config_to_default() */

static void hdrscmdb_stream_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrstream_set_config_to_default(cfg_ptr);  
} /* hdrscmdb_stream_set_config_to_default() */

static void hdrscmdb_fcp_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrfcp_set_config_to_default(cfg_ptr);
} /* hdrscmdb_fcp_set_config_to_default() */

static void hdrscmdb_dos_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrdos_set_config_to_default(cfg_ptr);
} /* hdrscmdb_dos_set_config_to_default() */

static void hdrscmdb_lup_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrlupc_set_config_to_default(cfg_ptr);
} /* hdrscmdb_lup_set_config_to_default() */

#if defined(FEATURE_HDR_QOS)
static void hdrscmdb_mrlp_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrmrlpc_set_config_params_default(cfg_ptr);
} /* hdrscmdb_mrlp_set_config_to_default() */
#endif /*FEATURE_HDR_QOS*/

static void hdrscmdb_bccp_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmbccp_set_config_params_default(cfg_ptr);
} /* hdrscmdb_bccp_set_config_to_default() */

static void hdrscmdb_ovhd_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmovhd_set_config_to_default(cfg_ptr);
} /* hdrscmdb_ovhd_set_config_to_default() */

static void hdrscmdb_csna_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmcsna_set_config_to_default(cfg_ptr);
} /* hdrscmdb_csna_set_config_to_default() */

static void hdrscmdb_phy_set_config_to_default(void* cfg_ptr, uint16 subtype)
{
  hdrscmphy_set_config_to_default(cfg_ptr);
} /* hdrscmdb_phy_set_config_to_default() */

/* forward declaration of functions */
static void hdrscmdb_set_efs_up_to_date_flag(boolean value );

typedef struct
{
  hdrhai_protocol_name_enum_type            protocol_name;
    /* Name of the protocol */

  uint32                                    cfg_offset; 
    /* Byte offset of the protocol's config structure in
       in hdrscmdb_protocol_config_data_type */

  uint32                                    verno_offset;
    /* Byte offset of the protocol's version number structure 
       in hdrscmdb_protocol_attrib_verno_type */

  uint32                                    cfg_size;
    /* Size of protocol's entire configuration data */

  hdrscmdb_get_attrib_offset_and_size_type  get_attrib_offset_and_size;
    /* Returns the attribute offset and size */

  hdrscmdb_get_attrib_verno_index_type      get_verno_index;
    /* Returns the attribute version number index */

  hdrscmdb_get_inconfig_handle              get_inconfig_handle;
    /* Returns a pointer to protocol's inconfig instance */

  hdrscmdb_set_config_to_default            set_config_to_default;
    /* Set's a given instance of protocol's config values to default */

  uint32                                    incfg_cfg_offset; 
    /* Byte offset of the protocol's config structure in
       in hdrscmdb_inconfig_config_data_type */

} hdrscmdb_reg_type;

/*-----------------------------------------------------------------------------
 This table contains one instance of hdrscmdb_reg_type struct for each of the 
 registered protocols.
-----------------------------------------------------------------------------*/
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 210000)
#pragma diag_suppress 1296
#endif
static const hdrscmdb_reg_type hdrscmdb_reg[HDRSCMDB_NUM_REG_PROTOCOLS] = 
{
  {
    HDRHAI_AC_MAC_PROTOCOL,                                 /* Protocol name */
    FPOS(hdrscmdb_protocol_config_data_type, amac),  /* Cfg data byte offset */
    FPOS(hdrscmdb_protocol_attrib_verno_type, amac),    /* Verno byte offset */
    sizeof(hdrscmamac_config_struct_type),             /* Size of entire cfg */
    hdrscmamac_get_attrib_offset_and_size,   /* Get offset and size function */
    hdrscmamac_get_attrib_verno_index,           /* Get verno index function */
    hdrscmdb_get_amac_inconfig_handle,                   /* Get incfg handle */
    hdrscmdb_amac_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, amac)
  },
  {
    HDRHAI_CC_MAC_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, cmac),
    FPOS(hdrscmdb_protocol_attrib_verno_type, cmac),
    sizeof(hdrscmcmac_config_struct_type),
    hdrscmcmac_get_attrib_offset_and_size,
    hdrscmcmac_get_attrib_verno_index,
    hdrscmdb_get_cmac_inconfig_handle,
    hdrscmdb_cmac_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, cmac)
  },
  {
    HDRHAI_FTC_MAC_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, fmac),
    FPOS(hdrscmdb_protocol_attrib_verno_type, fmac),
    sizeof(hdrscmfmac_config_struct_type),
    hdrscmfmac_get_attrib_offset_and_size,
    hdrscmfmac_get_attrib_verno_index,
    hdrscmdb_get_fmac_inconfig_handle,
    hdrscmdb_fmac_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, fmac)
  },
  {
    HDRHAI_RTC_MAC_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, rmac),
    FPOS(hdrscmdb_protocol_attrib_verno_type, rmac),
    sizeof(hdrscmrmac_config_attrib_union_type),    
    hdrscmrmac_get_attrib_offset_and_size,
    hdrscmrmac_get_attrib_verno_index,
    hdrscmdb_get_rmac_inconfig_handle,
    hdrscmdb_rmac_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, rmac)
  },
  {
    HDRHAI_KEY_EXCHANGE_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, kep),
    0xFFFFFFFF,
    sizeof(hdrkep_config_params_type),
    NULL, 
    NULL,
    hdrkep_get_inconfig_handle,
    hdrscmdb_kep_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
  {
    HDRHAI_AUTHENTICATION_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, auth),
    0xFFFFFFFF,
    sizeof(hdrauth_config_params_type),
    NULL, 
    NULL,
    hdrauth_get_inconfig_handle,
    hdrscmdb_auth_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
  {
    HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, almp),
    FPOS(hdrscmdb_protocol_attrib_verno_type, almp),
    sizeof(hdrscmalmp_config_params_type),
    hdrscmalmp_get_attrib_offset_and_size,
    hdrscmalmp_get_attrib_verno_index,
    hdrscmdb_get_almp_inconfig_handle,
    hdrscmdb_almp_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, almp)
  },
  {
    HDRHAI_IDLE_STATE_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, idle),
    0xFFFFFFFF,
    sizeof(hdridlec_config_params_type),
    hdrscmidle_get_attrib_offset_and_size, 
    NULL,
    hdridlec_get_inconfig_handle,
    hdrscmdb_idle_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
  {
    HDRHAI_ROUTE_UPDATE_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, rup),
    0xFFFFFFFF,
    sizeof(hdrrupc_config_params_type),
    hdrscmrup_get_attrib_offset_and_size, 
    NULL,
    hdrrupc_get_inconfig_handle,
    hdrscmdb_rup_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
  {
    HDRHAI_SESSION_MANAGEMENT_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, smp),
    0xFFFFFFFF,
    sizeof(hdrsmp_config_params_type),
    NULL, 
    NULL,
    hdrsmp_get_inconfig_handle,
    hdrscmdb_smp_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
  {
    HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, amp),
    FPOS(hdrscmdb_protocol_attrib_verno_type, amp),
    sizeof(hdrscmamp_config_params_type),
    hdrscmamp_get_attrib_offset_and_size,
    hdrscmamp_get_attrib_verno_index,
    hdrscmdb_get_amp_inconfig_handle,
    hdrscmdb_amp_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, amp)
  },
  {
    HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, scp),
    0xFFFFFFFF,
    sizeof(hdrscp_per_personality_config_params_type),
    NULL, 
    NULL,
    hdrscp_get_inconfig_handle,
    hdrscmdb_scp_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
  {
    HDRHAI_STREAM_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, stream),
    0xFFFFFFFF,
    sizeof(hdrstream_config_params_type),
    NULL, 
    NULL,
    hdrstream_get_inconfig_handle,
    hdrscmdb_stream_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
  {
    HDRHAI_LOCATION_UPDATE_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, lup),
    0xFFFFFFFF,
    sizeof(hdrlupc_config_params_type),
    NULL, 
    NULL,
    hdrlupc_get_inconfig_handle,
    hdrscmdb_lup_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
#if defined(FEATURE_HDR_QOS)
  {
    HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, mrlp),
    0xFFFFFFFF,
    sizeof(hdrmrlpc_config_params_type),
    NULL, 
    NULL,
    hdrmrlpc_get_inconfig_handle,
    hdrscmdb_mrlp_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
#endif /*FEATURE_HDR_QOS*/
  {
    HDRHAI_DATA_OVER_SIGNALING_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, dos),
    0xFFFFFFFF,
    sizeof(hdrdos_config_struct_type),
    NULL, 
    NULL,
    hdrdos_get_inconfig_handle,
    hdrscmdb_dos_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
  {
    HDRHAI_FLOW_CONTROL_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, fcp),
    0xFFFFFFFF,
    sizeof(hdrfcp_config_struct_type),
    NULL, 
    NULL,
    hdrfcp_get_inconfig_handle,
    hdrscmdb_fcp_set_config_to_default,
    HDRSCMDB_DATA_INVALID
  },
  {
    HDRHAI_BROADCAST_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, bccp),
    FPOS(hdrscmdb_protocol_attrib_verno_type, bccp),
    sizeof(hdrscmbccp_config_params_type),
    hdrscmbccp_get_attrib_offset_and_size,
    hdrscmbccp_get_attrib_verno_index,
    hdrscmdb_get_bccp_inconfig_handle,
    hdrscmdb_bccp_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, bccp)
  },
  {
    HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, gmmcdp),
    FPOS(hdrscmdb_protocol_attrib_verno_type, gmmcdp),
    sizeof(hdrscmgmmcdp_config_struct_type),
    hdrscmgmmcdp_get_attrib_offset_and_size,
    hdrscmgmmcdp_get_attrib_verno_index,
    hdrscmdb_get_gmmcdp_inconfig_handle,
    hdrscmdb_gmmcdp_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, gmmcdp)
  },
  {
    HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, ovhd),
    FPOS(hdrscmdb_protocol_attrib_verno_type, ovhd),
    sizeof(hdrscmovhd_config_struct_type),
    hdrscmovhd_get_attrib_offset_and_size,
    hdrscmovhd_get_attrib_verno_index,
    hdrscmdb_get_ovhd_inconfig_handle,
    hdrscmdb_ovhd_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, ovhd)
  },
  {
    /* CSNA: protocol ID is HDRHAI_PACKET_APPLICATION_CONTROLLER */
    HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, csna),
    FPOS(hdrscmdb_protocol_attrib_verno_type, csna),
    sizeof(hdrscmcsna_config_struct_type),
    hdrscmcsna_get_attrib_offset_and_size,
    hdrscmcsna_get_attrib_verno_index,
    hdrscmdb_get_csna_inconfig_handle,
    hdrscmdb_csna_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, csna)
  },
  {
    /* PHY: protocol ID is HDRHAI_PHYSICAL_LAYER_PROTOCOL */
    HDRHAI_PHYSICAL_LAYER_PROTOCOL,
    FPOS(hdrscmdb_protocol_config_data_type, phy),
    FPOS(hdrscmdb_protocol_attrib_verno_type, phy),
    sizeof(hdrscmphy_config_struct_type),
    hdrscmphy_get_attrib_offset_and_size,
    hdrscmphy_get_attrib_verno_index,
    hdrscmdb_get_phy_inconfig_handle,
    hdrscmdb_phy_set_config_to_default,
    FPOS(hdrscmdb_inconfig_config_data_type, phy)
  }
};
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 210000)
#pragma diag_default 1296
#endif
/*===========================================================================

                    LOCAL FUNCTION DECLARATIONS
                    
===========================================================================*/


/*============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================
FUNCTION HDRSCMDB_GET_INUSE_CONFIG_HANDLE

DESCRIPTION
 This function returns a pointer to inuse instance in database for
 a given protocol.
  
DEPENDENCIES
  None.

PARAMETERS
 cfg_offset - Byte offset of the protocol config structure in 
              hdrscmdb_protocol_config_data_type  
  
RETURN VALUE
  Pointer to inuse configuration data in database for the given protocol.
  
SIDE EFFECTS
  None  
=============================================================================*/
static __inline byte* hdrscmdb_get_inuse_config_handle
( 
  uint32 cfg_offset
)
{ 
  return ( (byte*)(&hdrscmdb.inuse) + cfg_offset );
} /* hdrscmdb_get_inuse_config_handle() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_NEGOTIATED_CONFIG_HANDLE

DESCRIPTION
 This function returns a pointer to protocol's negotiated config of
 a specified personality.
 
DEPENDENCIES
  None.

PARAMETERS
  pindex     - Personality index
  cfg_offset - Byte offset of the protocol config structure in 
               hdrscmdb_protocol_config_data_type  
  
RETURN VALUE
  Pointer to negotiated config data in database for the given protocol.
  
SIDE EFFECTS
  None  
=============================================================================*/
static __inline byte* hdrscmdb_get_negotiated_config_handle
( 
  uint16 pindex, 
  uint32 cfg_offset
)
{
  return ( (byte*)(&hdrscmdb.personality[pindex]) + cfg_offset );
} /* hdrscmdb_get_negotiated_config_handle() */ 

/*=============================================================================
FUNCTION HDRSCMDB_GET_PREVIOUS_CONFIG_HANDLE

DESCRIPTION
 This function returns a pointer to protocol's config in previous session
 of a specified personality.
  
DEPENDENCIES
  None.

PARAMETERS
  pindex     - Personality index
  cfg_offset - Byte offset of the protocol config structure in 
               hdrscmdb_protocol_config_data_type  
  
RETURN VALUE
  Pointer to previous session config data in database for the given protocol.

SIDE EFFECTS
  None
=============================================================================*/
static __inline byte* hdrscmdb_get_previous_config_handle
( 
  uint16 pindex, 
  uint32 cfg_offset
)
{
  return ( (byte*)(&hdrscmdb.previous[pindex]) + cfg_offset );
} /* hdrscmdb_get_previous_config_handle() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_ATTRIB_VERNO_HANDLE

DESCRIPTION
  This function returns a pointer to specified attribute's version number value
  
DEPENDENCIES
  None.

PARAMETERS
  verno_offset - Byte offset of the protocol version number structure in 
                 hdrscmdb_protocol_attrib_verno_type  
  verno_index  - Index of the given attribute in protocol's version
                 number array
  
RETURN VALUE
  Pointer to attributes version number value

SIDE EFFECTS
  None  
=============================================================================*/
static __inline byte* hdrscmdb_get_attrib_verno_handle
(
  uint32 verno_offset,
  uint16 verno_index
)
{
  ASSERT(verno_offset != 0xFFFFFFFF);
  
  if(verno_offset + verno_index < sizeof(hdrscmdb.version_num))
     return ( (byte*)(&hdrscmdb.version_num) + verno_offset + verno_index );

  ERR_FATAL("out of bound accessing of attribute version (offset,index,size) (%d,, %d, %d)", verno_offset, verno_index, sizeof(hdrscmdb.version_num));  
  
} /* hdrscmdb_get_attrib_verno_handle() */

/*=============================================================================
FUNCTION HDRSCMDB_STORE_ATTRIB

DESCRIPTION
  This function stores a given protocol's attribute value to database.
  
DEPENDENCIES
  None.

PARAMETERS
  pcfg_in_db_ptr - Pointer to protocol's configuration data structure in DB
  attrib_offset  - Offset of the attribute in protocol configuration structure
  attrib_val_ptr - Pointer to value of the attribute
  attrib_size    - Size of the attribute
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information.
=============================================================================*/
static __inline void hdrscmdb_store_attrib
(
  byte   *pcfg_in_db_ptr,
  uint16  attrib_offset,
  void   *attrib_val_ptr,
  uint16  attrib_size
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy((pcfg_in_db_ptr + attrib_offset), attrib_size, attrib_val_ptr, attrib_size);
#else
  memcpy((pcfg_in_db_ptr + attrib_offset), attrib_val_ptr, attrib_size);
#endif /* FEATURE_MEMCPY_REMOVAL */
} /* hdrscmdb_store_attrib() */


/*=============================================================================
FUNCTION HDRSCMDB_RETRIEVE_ATTRIB

DESCRIPTION
  This function retrieves a given protocol's attribute value from database.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_val_ptr - Pointer passed by calling function, where the attribute's 
                   value will be copied to.
  pcfg_in_db_ptr - Pointer to protocol's configuration data structure in DB
  attrib_offset  - Offset of the attribute in protocol configuration structure
  attrib_size    - Size of the attribute
  
RETURN VALUE
  None.

SIDE EFFECTS
  None  
=============================================================================*/
static __inline void hdrscmdb_retrieve_attrib
(
  void   *attrib_val_ptr,
  byte   *pcfg_in_db_ptr,
  uint16  attrib_offset,
  uint16  attrib_size
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy(attrib_val_ptr, attrib_size, 
            (pcfg_in_db_ptr + attrib_offset), attrib_size);
#else
  memcpy(attrib_val_ptr, (pcfg_in_db_ptr + attrib_offset), attrib_size);
#endif /* FEATURE_MEMCPY_REMOVAL */
} /* hdrscmdb_retrieve_attrib() */

/*=============================================================================
FUNCTION HDRSCMDB_STORE_CONFIG_ATTRIBS

DESCRIPTION
  This function stores a given protocol's entire configuration data to database
  
DEPENDENCIES
  None.

PARAMETERS
  pcfg_in_db_ptr - Pointer to protocol's configuration data structure in DB
  pcfg_val_ptr   - Pointer to protocol's configuration data, that need to be
                   stored to Database.
  pcfg_size      - Size of the protocol configuration data
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information.
=============================================================================*/
static __inline void hdrscmdb_store_config_attribs
(
  byte   *pcfg_in_db_ptr,
  void   *pcfg_val_ptr,
  uint32  pcfg_size
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy(pcfg_in_db_ptr, pcfg_size, pcfg_val_ptr, pcfg_size);
#else
  memcpy(pcfg_in_db_ptr, pcfg_val_ptr, pcfg_size);
#endif /* FEATURE_MEMCPY_REMOVAL */
} /* hdrscmdb_store_config_attribs() */

/*=============================================================================
FUNCTION HDRSCMDB_RETRIEVE_CONFIG_ATTRIBS

DESCRIPTION
  This function retrieves a given protocol's entire configuration data from 
  database.
  
DEPENDENCIES
  None.

PARAMETERS
  pcfg_val_ptr   - Pointer passed by calling function, where the config data
                   will be copied to.
  pcfg_in_db_ptr - Pointer to protocol's configuration data structure in DB
  pcfg_size      - Size of the protocol configuration data
  
RETURN VALUE
  None.

SIDE EFFECTS
  None 
=============================================================================*/
static __inline void hdrscmdb_retrieve_config_attribs
(
  void   *pcfg_val_ptr,
  byte   *pcfg_in_db_ptr,
  uint32  pcfg_size
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy(pcfg_val_ptr, pcfg_size, pcfg_in_db_ptr, pcfg_size);
#else
  memcpy(pcfg_val_ptr, pcfg_in_db_ptr, pcfg_size);
#endif /* FEATURE_MEMCPY_REMOVAL */
} /* hdrscmdb_retrieve_config_attribs() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_INUSE_PERSONALITY

DESCRIPTION
 This function returns the inuse personality index
  
DEPENDENCIES
  None.

PARAMETERS
  None  
  
RETURN VALUE
  Current personality index aka session config token 

SIDE EFFECTS
  None 
=============================================================================*/
static __inline uint16 hdrscmdb_get_inuse_personality( void )
{
  return hdrscmdb.curr_personality;
} /* hdrscmdb_get_inuse_personality() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_CURRENT_PERSONALITY

DESCRIPTION
 This function is a wrapper function of hdrscmdb_get_inuse_personality().
 It returns the inuse current personality index.
  
DEPENDENCIES
  None.

PARAMETERS
  None  
  
RETURN VALUE
  Current personality index aka session config token 

SIDE EFFECTS
  None 
=============================================================================*/
uint16 hdrscmdb_get_current_personality( void )
{
  return hdrscmdb_get_inuse_personality();
} /* hdrscmdb_get_inuse_personality() */

#ifdef FEATURE_SEC
/*=============================================================================
FUNCTION HDRSCMDB_CREATE_SHA1_DIGEST

DESCRIPTION
  This function computes the msg digest (160 bits) using the SHA-1 algorithm.

DEPENDENCIES
  None.

PARAMETERS
  hash - Address where the computed message digest would be stored.
   
RETURN VALUE
  E_SUCCESS if the message digest was computes successfully. 
  E_FAILURE otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
static hdrerrno_enum_type hdrscmdb_create_sha1_digest
(
  uint8 hash[]
)
{
  hdrerrno_enum_type                err_no = E_FAILURE;
    /* Error code */

  secapi_handle_type                hash_handle;
    /* Hash Handle */

  secapi_hsh_param_data_type        params;
    /* Hash Parameters */

  boolean registered = FALSE;
    /* is wait func registered before? */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Register the wait function */
  err_no = secapi_register_wait_func( HDRMC_TASK_PTR, hdrmc_wait );

  if ( (err_no != E_SUCCESS) &&
       (err_no != E_ALREADY_DONE) )
  {
    /* Cannot register hdrmc wait func */
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "HDRMC cannot register signals waiting func", 
                   err_no);
    return err_no;
  }

  if (err_no == E_ALREADY_DONE)
  {
    /* wait function is registerd at other places. Should not 
       register it in this function */
    registered = TRUE;
  }

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "SHA-1 Hash Computation begins");

  /*-----------------------------------------------------------------------
                                INITIALIZATION
  ------------------------------------------------------------------------*/
  /* Hash output */
  memset(hash, 0, SECAPI_HSH_SHA_DIGEST_SIZE);

  /*-----------------------------------------------------------------------
                               CREATE HASH 
  ------------------------------------------------------------------------*/
  /* Allocate a new hash instance from the crypto layer */
  if ( (err_no = (hdrerrno_enum_type) secapi_new( &hash_handle, SECAPI_SHA )) != E_SUCCESS )
  {
    /* Cannot get handle to create hash */
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "New hash error: %d", err_no);
  }
  /* Set the Hash Input Mode */
  else
  { 
    params.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
    if( (err_no = (hdrerrno_enum_type) secapi_hsh_set_param(hash_handle, 
                                           SECAPI_SHA,
                                           SECAPI_HSH_INPUT_MODE,
                                           &params)) != E_SUCCESS)
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Set mode error: %d", err_no);
    }
    else
    {
      /* Preferred execution platform to ARM */
      params.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
    
      if ((err_no = (hdrerrno_enum_type) secapi_hsh_set_param(hash_handle,
                        SECAPI_SHA, SECAPI_EXEC_PLATFORM,
                        &params)) != E_SUCCESS)
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Set pref. platform error: %d", err_no);
      }
      /* Create the hash */
      else if ((err_no = (hdrerrno_enum_type) secapi_hsh_create_digest(
                          hash_handle, 
                          SECAPI_SHA,
                          (uint8*)hdrscmdb.personality,
                          (uint16) sizeof(hdrscmdb.personality), 
                          (uint8*) hash)) != E_SUCCESS)
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "HashA: Atomic digest error: %d", err_no);
      }
    }
    
    /* Delete the hash instance */
    if ((err_no = (hdrerrno_enum_type) secapi_delete( &hash_handle ))!= E_SUCCESS)
    {
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Delete instance error: %d", err_no );
    }
  }

  if (!registered)
  {
    /* Only deregister wait function if it is registered 
       by this function. */
    err_no = secapi_register_wait_func( HDRMC_TASK_PTR, NULL );

    if ( err_no != E_SUCCESS )
    {
      /* Cannot de-register signals waiting func */
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "HDRMC cannot de-reg wait func from SECAPI code %d", err_no );
    }
  }

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "SHA-1 Hash Computation ends");

  return err_no;

} /* hdrscmdb_create_sha1_digest */

#endif /* FEATURE_SEC */

/*=============================================================================
FUNCTION HDRSCMDB_READ

DESCRIPTION
  This function initializes the database from the session information stored 
  in EFS, reads attribute configuration values for all personalities.

DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  E_SUCCESS if database was read successfully from EFS. E_FAILURE otherwise.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static hdrerrno_enum_type hdrscmdb_read( void )
#ifdef FEATURE_HDR_EFS_COMPRESS
{
  int fd = -1;
    /* EFS file descriptor */
  hdrerrno_enum_type err_no = E_SUCCESS; 
    /* EFS read status flag */
  uint32 bytes_remaining = 0;
    /* Bytes left to be written to EFS config file */
  uint32 offset = 0;
    /* Byte offset in the file at which to write the data */
  int32 bytes_read = 0;
  byte *buffer = NULL;
    /* Pointer to data buffer */
#ifdef FEATURE_SEC
  uint8 hash[SECAPI_HSH_SHA_DIGEST_SIZE];
#endif /* FEATURE_SEC */
    /* Hash value in bytes */
  uint8 personality = 0;
    /* Personality Index */
  uint8 *hdr_cfg_data_compressed = NULL;
  uint8 *hdr_cfg_data_uncompressed = NULL;
    /* for storing the compressed and uncompressed hdrscmdb data */
  int8 uncompress_status = Z_ERRNO;
  uint32 uncompress_size = HDRSCMDB_UNCOMPRESSED_MEMSIZE;
  uint32 compress_size = HDRSCMDB_COMPRESSED_MEMSIZE;
  uint8 *mem_read_ptr = (void *)NULL;
  size_t memscpy_size = 0;
  struct fs_stat  sbuf;
    /* for determining file size */
 
/*---------------------------------------------------------------------------*/
  HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "UNCOMPRESS_SIZE=%d COMPRESS_SIZE=%d", HDRSCMDB_UNCOMPRESSED_MEMSIZE, HDRSCMDB_COMPRESSED_MEMSIZE);

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Session database decompression and EFS reading Start");
  if(efs_stat("/hdr/hdr_protocol.z", &sbuf) == -1) 
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
	    "Compressed scmdb cfg file does not exits in EFS");
    err_no = E_FAILURE;
    return err_no;
  }

  fd = efs_open( "/hdr/hdr_protocol.z", O_RDONLY );

  if (fd == -1)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Cannot open config file for read");
    err_no = E_FAILURE;
    return err_no;
  }

  if ( !(hdr_cfg_data_compressed = 
           (uint8 *)modem_mem_alloc (sbuf.st_size, 
		                             MODEM_MEM_CLIENT_HDR_CRIT)) ) 
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
	    "Cannot Allocate Memory for getting compressed scmdb file");
    err_no = E_FAILURE;
    (void) efs_close(fd);
    return err_no;
  }
  
  /*Start Reading the compressed file from efs*/
  buffer = (byte*) hdr_cfg_data_compressed;
  bytes_remaining = sbuf.st_size;
  offset = 0;
  while (bytes_remaining > 0) 
  {
    bytes_read = efs_read (fd, buffer + offset, bytes_remaining);
    if (bytes_read <= 0) 
    {
      err_no = E_FAILURE;
      break;
    }
    bytes_remaining -= bytes_read;
    offset += bytes_read;
  }

  if (err_no == E_FAILURE)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Cannot read hdr cfg file from efs");
    modem_mem_free((void*)hdr_cfg_data_compressed, MODEM_MEM_CLIENT_HDR_CRIT);
    (void) efs_close(fd);
    return err_no;
  }
  
  if  ( !(hdr_cfg_data_uncompressed = 
          (uint8 *)modem_mem_alloc (HDRSCMDB_UNCOMPRESSED_MEMSIZE, 
		                             MODEM_MEM_CLIENT_HDR_CRIT)) )
  {	
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
	    " Cannot Allocate Memory for uncompressing scmdb file");
    modem_mem_free((void*)hdr_cfg_data_compressed, MODEM_MEM_CLIENT_HDR_CRIT);
    err_no = E_FAILURE;
    (void) efs_close(fd);
    return err_no;
  }

  (void) efs_close(fd);
  /* do while(0) loop is used to use to break easily in case of an error 
     has better redability tan else if*/
  do  
  {
    if(  (uncompress_status = uncompress((uint8 *)hdr_cfg_data_uncompressed, 
                                   &uncompress_size, 
                                   (const uint8 *)hdr_cfg_data_compressed, 
                                   (uint32)compress_size))  != Z_OK )
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Failed to uncompress the hdr cfg file from efs");
      err_no = E_FAILURE;
      break;
    }
    
	/* Config file opened & uncompressed successfully*/
    /*------------------------------------------------------------------------
    Read build info (compile date and time) 
    ------------------------------------------------------------------------*/
	mem_read_ptr = hdr_cfg_data_uncompressed;
	memscpy_size = sizeof(hdrscmdb_session_db_status_enum_type);
	if( (memscpy((void*)&hdrscmdb.session_db_status, memscpy_size, 
		         (void*)mem_read_ptr, memscpy_size)) != memscpy_size ) 
	{
	  HDR_MSG_PROT(MSG_LEGACY_ERROR, "Failed to copy session status from memory");
          err_no = E_FAILURE;
	  break;
	}

	mem_read_ptr += memscpy_size;
	memscpy_size = sizeof(hdrscmdb_build_info_type);
	if( (memscpy((void*)&hdrscmdb.build_info, memscpy_size, 
		         (void*)mem_read_ptr, memscpy_size)) != memscpy_size ) 
	{
	  HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying build info from memory");
          err_no = E_FAILURE;
	  break;
	}
    /*------------------------------------------------------------------------
    Check build info from config file matches with build loaded in the phone
    ------------------------------------------------------------------------*/
    if (strcmp(hdrscmdb.build_info.build_date, ver_date ) ||
             strcmp(hdrscmdb.build_info.build_time, ver_time ))
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Config file is invalid");
      err_no = E_FAILURE;
      break;
    }

    /*------------------------------------------------------------------------
    Read Address Management Protocol's config data
    ------------------------------------------------------------------------*/
    mem_read_ptr += memscpy_size;
    memscpy_size = sizeof(hdramp_session_info_type);
    if( (memscpy((void*)&hdrscmdb.amp_info, memscpy_size, 
	         (void*)mem_read_ptr, memscpy_size)) != memscpy_size ) 
    {
       HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying AMP info from memory");
       err_no = E_FAILURE;
       break;
    }

    /*------------------------------------------------------------------------
    Read Session Configuration Protocol's hardlink config params
    ------------------------------------------------------------------------*/
    mem_read_ptr += memscpy_size;
    memscpy_size = sizeof(hdrscp_hardlink_config_params_type);
    if( (memscpy((void*)&hdrscmdb.scp_info, memscpy_size, 
    	         (void*)mem_read_ptr, memscpy_size)) != memscpy_size ) 
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying SCP info from memory");
          err_no = E_FAILURE;
      break;
    }
    
    /*------------------------------------------------------------------------
    Read Location Update Protocol's config data
    ------------------------------------------------------------------------*/
    mem_read_ptr += memscpy_size;
    memscpy_size = sizeof(hdrlup_session_info_type);
    if( (memscpy((void*)&hdrscmdb.lup_info, memscpy_size, 
	             (void*)mem_read_ptr, memscpy_size)) != memscpy_size ) 
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying LUP info from memory");
             err_no = E_FAILURE;
      break;
    }

    /*------------------------------------------------------------------------
    Read protocol configuration parameters
    ------------------------------------------------------------------------*/
    mem_read_ptr += memscpy_size;
    memscpy_size = sizeof(hdrscmdb.personality);
    if( (memscpy((void*)hdrscmdb.personality, memscpy_size, 
    	         (void*)mem_read_ptr, memscpy_size)) != memscpy_size ) 
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying personality info from memory");
             err_no = E_FAILURE;
      break;
    }

#ifdef FEATURE_SEC

    /*------------------------------------------------------------------------
      Read the keys from the NV, restore it in DB. Hash this DB and compare 
      with the hash read from NV. 
    ------------------------------------------------------------------------*/
    if (hdrutil_read_nv_item (
          NV_HDRSCMDB_SECURE_CONFIG_I,
          (nv_item_type*)&hdrscmdb.nv_sec_config) != NV_DONE_S)
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "HDR Secure Config NV read failed...Discarding session");
      err_no = E_FAILURE;
      break;
    }
    
    /* If NV read succeeded or err_no != E_FAILURE*/
    /* Copy KEP config for all personalities from NV item into DB */
    for (personality = 0; 
          personality < HDRSCMDB_MAX_PERSONALITIES; ++personality)
    {
    
      /* Copy one field at a time since nv_hdrscmdb_secure_config_type is 
         packed */
      hdrscmdb.personality[personality].kep.key_len = 
        hdrscmdb.nv_sec_config.kep_config[personality].key_len;
    
      hdrscmdb.personality[personality].kep.keys_are_valid = 
        hdrscmdb.nv_sec_config.kep_config[personality].keys_are_valid;
    
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy(
        (uint8*) hdrscmdb.personality[personality].kep.rpc_auth_key, 
        HDRKEP_KEY_SIG_LEN, 
        (uint8*) hdrscmdb.nv_sec_config.kep_config[personality].rpc_auth_key,
        HDRKEP_KEY_SIG_LEN );
#else
      memcpy(
        (uint8*) hdrscmdb.personality[personality].kep.rpc_auth_key, 
        (uint8*) hdrscmdb.nv_sec_config.kep_config[personality].rpc_auth_key,
        HDRKEP_KEY_SIG_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
    
    /* Compute SHA1 Hash for DB */
    if ( hdrscmdb_create_sha1_digest (hash) != E_SUCCESS )
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "SHA1 hash computation failed");
      err_no = E_FAILURE;
    }
    /* If success, compare hash; If the hash doesn't match, return failure */
    else if ((memcmp((uint8*) hash, 
                     (uint8*) hdrscmdb.nv_sec_config.hash, 
                     SECAPI_HSH_SHA_DIGEST_SIZE) != 0))
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Config file has invalid hash");
      err_no = E_FAILURE;
    }
#endif /* FEATURE_SEC */

  }while(0); /* do  while(0)*/

   /* If read is failed, remove the EFS file */
  if(err_no == E_FAILURE)
  {
     HDR_MSG_PROT(MSG_LEGACY_MED, " EFS read failure and deleting the existing EFS file stored ");
     (void) efs_unlink("/hdr/hdr_protocol.z");
  }

  modem_mem_free((void*)hdr_cfg_data_compressed, MODEM_MEM_CLIENT_HDR_CRIT);
  modem_mem_free((void*)hdr_cfg_data_uncompressed, MODEM_MEM_CLIENT_HDR_CRIT);
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Session database decompression and EFS reading End");
  return err_no;

}
#else /*end #ifdef FEATURE_HDR_EFS_COMPRESS*/
{
  int fd = -1;
    /* EFS file descriptor */
  hdrerrno_enum_type err_no = E_SUCCESS; 
    /* EFS read status flag */
  uint32 bytes_remaining = 0;
    /* Bytes left to be written to EFS config file */
  uint32 offset = 0;
    /* Byte offset in the file at which to write the data */
  int32 bytes_read = 0;
  byte *buffer = NULL;
    /* Pointer to data buffer */
#ifdef FEATURE_SEC
  uint8 hash[SECAPI_HSH_SHA_DIGEST_SIZE];
#endif /* FEATURE_SEC */
    /* Hash value in bytes */
  uint8 personality = 0;
    /* Personality Index */
/*---------------------------------------------------------------------------*/

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Legacy Session database EFS reading Start");
  fd = efs_open( "/hdr/hdr_protocol.cfq", O_RDONLY );

  if (fd == -1)
  {
    ERR( "Cannot open config file for read", 0, 0, 0);
    err_no = E_FAILURE;
  }
  else /* Config file opened successfully for read */
  {
    /*------------------------------------------------------------------------
    Read build info (compile date and time) 
    ------------------------------------------------------------------------*/
    if (efs_read(fd, &hdrscmdb.session_db_status,
            sizeof(hdrscmdb_session_db_status_enum_type)) == -1)
    {
      ERR( "Cannot read session status", 0, 0, 0);
      err_no = E_FAILURE;
    }
    else if (efs_read(fd, &hdrscmdb.build_info, 
                 sizeof(hdrscmdb_build_info_type)) == -1)
    {
      ERR("Error reading build info from config file", 0, 0, 0);
      err_no = E_FAILURE;
    }
    /*------------------------------------------------------------------------
    Check build info from config file matches with build loaded in the phone
    ------------------------------------------------------------------------*/
    else if (strcmp(hdrscmdb.build_info.build_date, ver_date ) ||
             strcmp(hdrscmdb.build_info.build_time, ver_time ))
    {
      ERR("Config file is invalid", 0, 0, 0);
      err_no = E_FAILURE;
    }
    /*------------------------------------------------------------------------
    Read Address Management Protocol's config data
    ------------------------------------------------------------------------*/
    else if (efs_read(fd, &hdrscmdb.amp_info, 
                      sizeof(hdramp_session_info_type)) == -1)
    {
      ERR("Error reading AMP info from config file", 0, 0, 0);
      err_no = E_FAILURE;
    }
    /*------------------------------------------------------------------------
    Read Session Configuration Protocol's hardlink config params
    ------------------------------------------------------------------------*/
    else if (efs_read(fd, &hdrscmdb.scp_info, 
                      sizeof(hdrscp_hardlink_config_params_type)) == -1)
    {
      ERR("Error reading SCP info from config file", 0, 0, 0);
      err_no = E_FAILURE;
    }
    /*------------------------------------------------------------------------
    Read Location Update Protocol's config data
    ------------------------------------------------------------------------*/
    else if (efs_read(fd, &hdrscmdb.lup_info, 
                      sizeof(hdrlup_session_info_type)) == -1)
    {
      ERR("Error reading LUP info from config file", 0, 0, 0);
      err_no = E_FAILURE;
    }

    /*------------------------------------------------------------------------
    Read protocol configuration parameters
    ------------------------------------------------------------------------*/
    buffer = (byte*) hdrscmdb.personality;
    bytes_remaining = sizeof(hdrscmdb.personality);
    offset = 0;

    while (bytes_remaining > 0) 
    {
      bytes_read = efs_read (fd, buffer + offset, bytes_remaining);
      if (bytes_read <= 0) 
      {
        err_no = E_FAILURE;
        break;
      }
      bytes_remaining -= bytes_read;
      offset += bytes_read;
    }

    (void) efs_close(fd);

#ifdef FEATURE_SEC

    /*------------------------------------------------------------------------
      Read the keys from the NV, restore it in DB. Hash this DB and compare 
      with the hash read from NV. 
    ------------------------------------------------------------------------*/
    if (hdrutil_read_nv_item (
          NV_HDRSCMDB_SECURE_CONFIG_I,
          (nv_item_type*)&hdrscmdb.nv_sec_config) != NV_DONE_S)
    {
      ERR("HDR Secure Config NV read failed...Discarding session", 0, 0, 0);
      err_no = E_FAILURE;
    }

    /* If NV read succeeded */
    if (err_no != E_FAILURE)
    {
      /* Copy KEP config for all personalities from NV item into DB */
      for (personality = 0; 
            personality < HDRSCMDB_MAX_PERSONALITIES; ++personality)
      {

        /* Copy one field at a time since nv_hdrscmdb_secure_config_type is 
           packed */
        hdrscmdb.personality[personality].kep.key_len = 
          hdrscmdb.nv_sec_config.kep_config[personality].key_len;

        hdrscmdb.personality[personality].kep.keys_are_valid = 
          hdrscmdb.nv_sec_config.kep_config[personality].keys_are_valid;

#ifdef FEATURE_MEMCPY_REMOVAL
        memscpy(
          (uint8*) hdrscmdb.personality[personality].kep.rpc_auth_key, 
          HDRKEP_KEY_SIG_LEN,
          (uint8*) hdrscmdb.nv_sec_config.kep_config[personality].rpc_auth_key,
          HDRKEP_KEY_SIG_LEN );   
#else
        memcpy(
          (uint8*) hdrscmdb.personality[personality].kep.rpc_auth_key, 
          (uint8*) hdrscmdb.nv_sec_config.kep_config[personality].rpc_auth_key,
          HDRKEP_KEY_SIG_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */        
      }

      /* Compute SHA1 Hash for DB */
      if ( hdrscmdb_create_sha1_digest (hash) != E_SUCCESS )
      {
        ERR("SHA1 hash computation failed", 0, 0, 0);
        err_no = E_FAILURE;
      }
      /* If success, compare hash; If the hash doesn't match, return failure */
      else if ((memcmp((uint8*) hash, 
                       (uint8*) hdrscmdb.nv_sec_config.hash, 
                       SECAPI_HSH_SHA_DIGEST_SIZE) != 0))
      {
        ERR("Config file has invalid hash", 0, 0, 0);
        err_no = E_FAILURE;
      }
    }
#endif /* FEATURE_SEC */

    /* If read is failed, remove the EFS file */
    if(err_no == E_FAILURE)
    {
       HDR_MSG_PROT(MSG_LEGACY_MED, " EFS read failure and deleting the existing EFS file stored ");
       (void) efs_unlink("/hdr/hdr_protocol.cfq");
    }

  } /* else Config file opened successfully for read */

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Legacy Session database EFS reading End");
  return err_no;

} /* hdrscmdb_read() */
#endif /*#else *FEATURE_HDR_EFS_COMPRESS*/

/*=============================================================================
FUNCTION HDRSCMDB_WRITE_ALL_CONFIG_DATA

DESCRIPTION
  This function writes the database of session information to EFS.

DEPENDENCIES
  None.

PARAMETERS
  None.
                          
RETURN VALUE
  E_SUCCESS if database was successfully written to EFS file. E_FAILURE 
  otherwise.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
hdrerrno_enum_type hdrscmdb_write_all_config_data( void )
{
#ifdef FEATURE_HDR_EFS_COMPRESS
  const char* temp_file = "/hdr/hdr_temp.z";
  const char* final_file = "/hdr/hdr_protocol.z";
#else /*end #ifdef FEATURE_HDR_EFS_COMPRESS*/
  const char* temp_file = "/hdr/hdr_temp.cfg";
  const char* final_file = "/hdr/hdr_protocol.cfg";
#endif /*#else FEATURE_HDR_EFS_COMPRESS*/
  int fd = -1;
    /* EFS file descriptor */
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* File write status */
  uint32 bytes_remaining = 0;
    /* Bytes left to written to EFS config file */
  uint32 offset = 0;
    /* Byte offset in the file at which to write the data */
  int32 bytes_written = 0;
  byte *buffer = NULL;
    /* Pointer to data buffer */
#ifdef FEATURE_SEC
  uint8 hash[SECAPI_HSH_SHA_DIGEST_SIZE] = {0};
#endif /* FEATURE_SEC */
    /* Hash value in bytes */
  uint8 personality = 0; 
    /* Personality index */
  hdrscmdb_protocol_config_data_type *personality_ptr;
    /* points to current or previous session data */
  hdramp_session_info_type *amp_info_ptr;
  hdrscp_hardlink_config_params_type *scp_info_ptr;
  hdrlup_session_info_type *lup_info_ptr;
  uint8 *hdr_cfg_data_uncompressed = NULL;
    /* for storing the compressed and uncompressed hdrscmdb data */
#ifdef FEATURE_HDR_EFS_COMPRESS
  uint8 *hdr_cfg_data_compressed = NULL;
  int8 compress_status = Z_ERRNO;
  uint32 compress_size = HDRSCMDB_COMPRESSED_MEMSIZE;
#endif /*FEATURE_HDR_EFS_COMPRESS*/
  uint32 uncompress_size = HDRSCMDB_UNCOMPRESSED_MEMSIZE;
  uint8 *mem_write_ptr = (void *)NULL;
  size_t memscpy_size = 0;

/*---------------------------------------------------------------------------*/
  if (hdrscmdb.session_db_status == HDRSCMDB_SESSION_CURRENT)
  {
    amp_info_ptr = &hdrscmdb.amp_info;
    scp_info_ptr = &hdrscmdb.scp_info;
    lup_info_ptr = &hdrscmdb.lup_info;
    personality_ptr = hdrscmdb.personality;
    HDR_MSG_PROT(MSG_LEGACY_LOW,
        "Writing current session to EFS");
  }
  else if (hdrscmdb.session_db_status != HDRSCMDB_SESSION_INVALID)
  {
    amp_info_ptr = &hdrscmdb.prev_amp_info;
    scp_info_ptr = &hdrscmdb.prev_scp_info;
    lup_info_ptr = &hdrscmdb.prev_lup_info;
    personality_ptr = hdrscmdb.previous;
    HDR_MSG_PROT(MSG_LEGACY_LOW,
        "Writing previous session to EFS");
  }
  else /* HDRSCMDB_SESSION_INVALID */
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                 "Session database is not valid, so not writing to EFS");
    return E_FAILURE;
  }

  /* Since ConfigLock is active only when connection is open and it has to be
     set to unlock on power re-cycle, data should not be committed to EFS when
     this is set */
  ASSERT (hdrscmdb.scp_info.config_is_locked == FALSE);

  
  /*--------------------------------------------------------------------------
   Before writing the configuration data to EFS invalidate the current file by
   renaming it as temporary file and after successful write rename it back to
   a valid filename. Otherwise if the phone powers down in middle of write and
   comes back up it will use a invalid configuration file. By renaming the file
   as temporary and if the write did not complete fully, on powerup efs_read 
   would fail because a valid file would not exist only the temp file would. 
   In this case AT would just re-negotiate the session.
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_EFS_COMPRESS
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Session database Compression and EFS writing start");
#else
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Session database EFS writing start");
#endif /*#ifdef *FEATURE_HDR_EFS_COMPRESS*/
      
  if ( efs_rename(final_file, temp_file) != E_SUCCESS )
  {
    /* If rename failed a temporary file could exist, delete it. AT could have
       powered down in middle of write to temp file */ 
    (void) efs_unlink(temp_file);
  
    /* Rename could fail for other reasons too such as AT is writing it's config
       data to EFS for the first time i.e a valid file doesn't exist at all then
       both rename and delete would fail. In this case AT would just start with
       a temp file and rename it to valid filename after a successful write */
  }

#ifdef FEATURE_HDR_EFS_COMPRESS
  if ( !(hdr_cfg_data_compressed = 
           (uint8 *)modem_mem_alloc (HDRSCMDB_COMPRESSED_MEMSIZE, MODEM_MEM_CLIENT_HDR_CRIT)) ) 
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
	    "Cannot Allocate Memory for getting compressed scmdb file");
    err_no = E_FAILURE;
    (void) efs_unlink(temp_file);
    return err_no;
  }
#endif /*#ifdef *FEATURE_HDR_EFS_COMPRESS*/
  
  if  ( !(hdr_cfg_data_uncompressed = 
         (uint8 *)modem_mem_alloc (HDRSCMDB_UNCOMPRESSED_MEMSIZE, MODEM_MEM_CLIENT_HDR_CRIT)) )
  {	
    HDR_MSG_PROT(MSG_LEGACY_ERROR, 
	    " Cannot Allocate Memory for uncompressing scmdb file");
#ifdef FEATURE_HDR_EFS_COMPRESS
    modem_mem_free((void*)hdr_cfg_data_compressed, MODEM_MEM_CLIENT_HDR_CRIT);
#endif /*#ifdef *FEATURE_HDR_EFS_COMPRESS*/
    err_no = E_FAILURE;
    (void) efs_unlink(temp_file);
    return err_no;
  }

  /* do while(0) loop is used to use to break easily in case of an error 
     has better redability tan else if*/
  do  
  {
    /*------------------------------------------------------------------------
     Write session status
    -------------------------------------------------------------------------*/
    uncompress_size = 0;
    mem_write_ptr = hdr_cfg_data_uncompressed;
    memscpy_size = sizeof(hdrscmdb_session_db_status_enum_type);
    if( (memscpy((void*)mem_write_ptr, memscpy_size, 
	         (void*)&hdrscmdb.session_db_status, memscpy_size)) != memscpy_size ) 
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Failed to copy session status to memory");
      err_no = E_FAILURE;
      break;
    }
    uncompress_size += memscpy_size;

    /*------------------------------------------------------------------------
     Write build info (compile date and time)
    -------------------------------------------------------------------------*/
    mem_write_ptr += memscpy_size;
    memscpy_size = sizeof(hdrscmdb_build_info_type);
    if( (memscpy((void*)mem_write_ptr, memscpy_size, 
	         (void*)&hdrscmdb.build_info, memscpy_size)) != memscpy_size ) 
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying build info to memory");
      err_no = E_FAILURE;
      break;
    }
    uncompress_size += memscpy_size;
	
    /*------------------------------------------------------------------------
     Write AMP's config data 
     ------------------------------------------------------------------------*/
    mem_write_ptr += memscpy_size;
    memscpy_size = sizeof(hdramp_session_info_type);
    if( (memscpy((void*)mem_write_ptr, memscpy_size, 
	         (void*)amp_info_ptr, memscpy_size)) != memscpy_size ) 
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying AMP info to memory");
      err_no = E_FAILURE;
      break;
    }
    uncompress_size += memscpy_size;

    /*------------------------------------------------------------------------
     Write SCP's config data 
    ------------------------------------------------------------------------*/
    mem_write_ptr += memscpy_size;
    memscpy_size = sizeof(hdrscp_hardlink_config_params_type);
    if( (memscpy((void*)mem_write_ptr, memscpy_size, 
	         (void*)scp_info_ptr, memscpy_size)) != memscpy_size ) 
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying SCP info to memory");
      err_no = E_FAILURE;
      break;
    }
    uncompress_size += memscpy_size;

    /*------------------------------------------------------------------------
    Write LUP's config data 
    ------------------------------------------------------------------------*/
    mem_write_ptr += memscpy_size;
    memscpy_size = sizeof(hdrlup_session_info_type);
    if( (memscpy((void*)mem_write_ptr, memscpy_size, 
	         (void*)lup_info_ptr, memscpy_size)) != memscpy_size ) 
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying LUP info to memory");
      err_no = E_FAILURE;
      break;
    }
    uncompress_size += memscpy_size;


#ifdef FEATURE_SEC
    /*---------------------------------------------------------------------
    Hash DB. Write KEP config and hash into secure file
    ----------------------------------------------------------------------*/
    /* Hash dB */
    if ( ( err_no != E_FAILURE ) && 
         ( hdrscmdb_create_sha1_digest (hash) != E_SUCCESS ) )
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "SHA1 hash computation failed");
      err_no = E_FAILURE;
	  break;
    }

    /* Checking for E_SUCCESS only since the SHA API's could written more
       than one error code on error */
    if (err_no != E_FAILURE)
    {
      /* Copy the computed hash into the NV item */
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy((uint8*) hdrscmdb.nv_sec_config.hash, 
               NV_SECAPI_HSH_SHA_DIGEST_SIZE, 
              (uint8*) hash, 
               SECAPI_HSH_SHA_DIGEST_SIZE);
#else
      memcpy((uint8*) hdrscmdb.nv_sec_config.hash, 
             (uint8*) hash, 
             SECAPI_HSH_SHA_DIGEST_SIZE);
#endif /* FEATURE_MEMCPY_REMOVAL */

      /* Copy KEP config from DB to NV item */
      for (personality = 0; 
           personality < HDRSCMDB_MAX_PERSONALITIES; ++personality)
      {
        /* Copy one field at a time since nv_hdrscmdb_secure_config_type is 
           packed */
        hdrscmdb.nv_sec_config.kep_config[personality].key_len = 
          personality_ptr[personality].kep.key_len;

        hdrscmdb.nv_sec_config.kep_config[personality].keys_are_valid = 
          personality_ptr[personality].kep.keys_are_valid;

#ifdef FEATURE_MEMCPY_REMOVAL
        memscpy(
          (uint8*) hdrscmdb.nv_sec_config.kep_config[personality].rpc_auth_key, 
          NV_HDRKEP_KEY_SIG_LEN, 
          (uint8*) personality_ptr[personality].kep.rpc_auth_key, 
          HDRKEP_KEY_SIG_LEN );
#else
        memcpy(
          (uint8*) hdrscmdb.nv_sec_config.kep_config[personality].rpc_auth_key, 
          (uint8*) personality_ptr[personality].kep.rpc_auth_key, 
          HDRKEP_KEY_SIG_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */

        /* Zero out the secure keys in DB */
        memset(&personality_ptr[personality].kep, 
               0, 
               sizeof(hdrkep_config_params_type));
      }

      hdrscmdb.nv_cmd.item = NV_HDRSCMDB_SECURE_CONFIG_I;
      hdrscmdb.nv_cmd.data_ptr = (nv_item_type*) &hdrscmdb.nv_sec_config;

      hdrutil_write_nv_for_hdrmc(&hdrscmdb.nv_cmd);

    } /* if (err_no != E_FAILURE) */

#endif /* FEATURE_SEC */

    if (err_no != E_FAILURE)
    {

      /*------------------------------------------------------------------------
      Write protocol configuration params to memory
      -------------------------------------------------------------------------*/	  

      mem_write_ptr += memscpy_size;
      memscpy_size = sizeof(hdrscmdb.personality);
      if( (memscpy((void*)mem_write_ptr, memscpy_size, 
                   (void*)hdrscmdb.personality, memscpy_size)) != memscpy_size ) 
      {
	     HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error copying personality info to memory");
         err_no = E_FAILURE;
	     break;
      }
      uncompress_size += memscpy_size;
	
#ifdef FEATURE_HDR_EFS_COMPRESS	
      if(  (compress_status = compress2((uint8 *)hdr_cfg_data_compressed, 
                                 &compress_size, 
                                 (const uint8 *)hdr_cfg_data_uncompressed, 
                              (uint32)uncompress_size, Z_DEFAULT_COMPRESSION))  != Z_OK )
      {
         HDR_MSG_PROT(MSG_LEGACY_ERROR, "Failed to compress the SCMDB data");
         err_no = E_FAILURE;
	 break;  /* break out of the do while(0) loop*/
      }
#endif /*#ifdef *FEATURE_HDR_EFS_COMPRESS*/
      fd = efs_open( temp_file, O_WRONLY | O_CREAT );
      
      if (fd == -1) 
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "Cannot open config file for write" );
        err_no = E_FAILURE;
        break;
      }
      
      hdrscmdb.file_write_counter++;
      HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
        "SCMDB: Saving session data, status: %d, write count= %d",
      hdrscmdb.session_db_status, hdrscmdb.file_write_counter);

      /*------------------------------------------------------------------------
      Write compressed protocol configuration params to EFS. Currently 
	  "efs_write" can write only 8192 bytes at a time, so write it in parts.
      ------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_EFS_COMPRESS
      buffer = (byte*) hdr_cfg_data_compressed;
      bytes_remaining = compress_size;
#else
      buffer = (byte*) hdr_cfg_data_uncompressed;
      bytes_remaining = uncompress_size;
#endif /*#ifdef *FEATURE_HDR_EFS_COMPRESS*/

      offset = 0;

      while (bytes_remaining > 0) 
      {
        bytes_written = efs_write (fd, buffer + offset, bytes_remaining);
        if (bytes_written <= 0) 
        {
          HDR_MSG_PROT( MSG_LEGACY_ERROR, "Cannot write  data to efs config file" );
          err_no = E_FAILURE;
          break;
        }
        bytes_remaining -= bytes_written;
        offset += bytes_written;
      }
    
      (void) efs_close(fd);

      if (bytes_written <= 0) break;
	
	
#ifdef FEATURE_SEC

      /* Restore the KEP keys to DB */
      for (personality = 0; 
            personality < HDRSCMDB_MAX_PERSONALITIES; ++personality)
      {
        personality_ptr[personality].kep.key_len = 
          hdrscmdb.nv_sec_config.kep_config[personality].key_len;

        personality_ptr[personality].kep.keys_are_valid = 
          hdrscmdb.nv_sec_config.kep_config[personality].keys_are_valid;

#ifdef FEATURE_MEMCPY_REMOVAL
        memscpy(
          (uint8*) &personality_ptr[personality].kep.rpc_auth_key[0], 
          HDRKEP_KEY_SIG_LEN,
          (uint8*) &hdrscmdb.nv_sec_config.kep_config[personality].rpc_auth_key[0],
          HDRKEP_KEY_SIG_LEN );
#else
        memcpy(
          (uint8*) &personality_ptr[personality].kep.rpc_auth_key[0], 
          (uint8*) &hdrscmdb.nv_sec_config.kep_config[personality].rpc_auth_key[0],
          HDRKEP_KEY_SIG_LEN );
#endif /* FEATURE_MEMCPY_REMOVAL */
      }

#endif /* FEATURE_SEC */

    } /* if (err_no != E_FAILURE) */

    

    HDR_MSG_PROT (MSG_LEGACY_HIGH, "SCMDB: Config data committed to EFS");

  }while(0); /* do  while(0)*/

  if(err_no != E_FAILURE)

  {
    /* Rename the temp file to valid configuration file name */
    if ( (efs_rename(temp_file, final_file) == -1) )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "config file write failed" );
        err_no = E_FAILURE;
      }

  }

  /* If EFS write is success, the EFS db and SCMDB are now in sync */
  if(err_no == E_SUCCESS)
  {  
    hdrscmdb_set_efs_up_to_date_flag(TRUE);	
  }
#ifdef FEATURE_HDR_EFS_COMPRESS
  modem_mem_free((void*)hdr_cfg_data_compressed, MODEM_MEM_CLIENT_HDR_CRIT);
#endif /*#ifdef *FEATURE_HDR_EFS_COMPRESS*/
  modem_mem_free((void*)hdr_cfg_data_uncompressed, MODEM_MEM_CLIENT_HDR_CRIT);
  (void) efs_unlink(temp_file);

#ifdef FEATURE_HDR_EFS_COMPRESS
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Session database Compression and EFS writing End");
#else
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Session database EFS writing End");
#endif /*#ifdef *FEATURE_HDR_EFS_COMPRESS*/
  return err_no;  

} /* hdrscmdb_write_all_config_data() */

/*=============================================================================
FUNCTION HDRSCMDB_FIND_REG_ENTRY

DESCRIPTION
  This function finds the entry of given protocol in DB reg table
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Name of the protocol
  
RETURN VALUE
  If successful returns pointer to protocol's entry in reg table, otherwise NULL

SIDE EFFECTS
  None
=============================================================================*/
static hdrscmdb_reg_type const *hdrscmdb_find_reg_entry
( 
  hdrhai_protocol_name_enum_type protocol_name
)
{
  uint8 index;
    /* Loop index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop through to find the entry for the protocol in DB reg table */
  for (index = 0; index < HDRSCMDB_NUM_REG_PROTOCOLS; index++)
  {
    if (hdrscmdb_reg[index].protocol_name == protocol_name)
    {               
      return &hdrscmdb_reg[index];
    }
  }

  HDR_MSG_PROT_1(MSG_LEGACY_LOW,"SCMDB: hdrscmdb_reg look up failed p=%d", 
                 protocol_name );

  return NULL;

} /* hdrscmdb_find_reg_entry() */

/*=============================================================================
FUNCTION HDRSCMDB_PROTOCOL_SUBTYPE_IS_HARDLINK

DESCRIPTION
  This function finds if the specified protocol's subtype is hardlink in given
  personality.

DEPENDENCIES
  Caller should interpret the return value appropriately. When FALSE is returned
  it could mean that the protocol subtype is not hardlinked or personality index
  is not valid.
  
  Caller should ensure that personality corresponding to the personality_index
  specified as argument is already committed to database. So the FALSE return 
  value can be unambigiously interpreted as protocol subtype is not hardlinked.
  
PARAMETERS
  pindex        - Personality index
  protocol_name - Name of the protocol 
  
RETURN VALUE
  TRUE if protocol subtype is hardlink FALSE otherwise

SIDE EFFECTS
  None
=============================================================================*/
static boolean hdrscmdb_protocol_subtype_is_hardlink
( 
  uint8 pindex, 
  hdrhai_protocol_name_enum_type protocol_name 
)
{
  boolean st_is_hardlink = FALSE;
    /* Flag to indicate if the given protocol is hardlinked or not */
/*---------------------------------------------------------------------------*/
  if (hdrscmdb.personality[pindex].personality_is_valid)
  {
    if (protocol_name >= HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX)
    {
      ASSERT (protocol_name == HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL ||
              protocol_name == HDRHAI_LOCATION_UPDATE_PROTOCOL ||
              protocol_name == HDRHAI_FLOW_CONTROL_PROTOCOL ||
              protocol_name == HDRHAI_DATA_OVER_SIGNALING_PROTOCOL ||
              protocol_name == HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL);

      /* If it is application layer protocol then look at Stream protocol's 
       * subtype */
      if (hdrscmdb.personality[pindex].scp.subtype[HDRHAI_STREAM_PROTOCOL] ==
          HDRSCMDB_HARDLINK_SUBTYPE)
      {
        st_is_hardlink = TRUE;
      }
    }
    else
    {
      /* Otherwise if this is not application layer protocol look at the given 
       * protocol's subtype itself and for AMP since it is always hardlinked
       * return TRUE */
      if ((hdrscmdb.personality[pindex].scp.subtype[protocol_name] ==
           HDRSCMDB_HARDLINK_SUBTYPE) ||
          (protocol_name == HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL))
      {
        st_is_hardlink = TRUE;
      }
    }
  }

  return st_is_hardlink;
} /* hdrscmdb_protocol_subtype_is_hardlink() */

/*============================================================================n

FUNCTION HDRSCMDB_RESET_ATTRIB_VERNO

DESCRIPTION
  This function sets the version number of all attributes to zero, this is 
  called during powerup and everytime the personality is switched.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_reset_attrib_verno( void )
{
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "SCMDB: Attribute version numbers reset");

  memset(&hdrscmdb.version_num, 0, sizeof(hdrscmdb.version_num));
}/* hdrscmdb_reset_attrib_verno() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_INUSE_ATTRIB

DESCRIPTION
  This function sets the inuse value of the specified attribute to the 
  value given.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name    - The protocol that the attribute belongs to
  attrib_id        - AttributeID
  value_id         - Value for simple attributes, ValueID for complex attributes
  complex_val_ptr  - Pointer to complex data value, or NULL for simple attrib
  subtype          - Negotiated subtype of the protocol
  curr_personality - AT's current personality index
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_set_inuse_attrib
( 
  hdrhai_protocol_name_enum_type  protocol_name,
  uint16                          attrib_id,
  uint16                          value_id,
  void                           *complex_val_ptr,
  uint16                          subtype,
  uint16                          curr_personality
)
{
  uint16 attrib_size = 0;   
    /* Size of attribute value */
  uint16 attrib_offset = 0; 
    /* Offset of attribute in protocol configuration structure */ 
  void *attrib_val_ptr;  
    /* Convenience pointer that points to value of the attribute */
  byte *curr_cfg_ptr;
    /* Pointer to protocol's inuse configuration structure in DB */
  uint16 attrib_verno_index = 0;
    /* Index that points to attributes version number in verno array */ 
  uint8 *attrib_verno_ptr;
    /* Convenience pointer to attribute version number */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR("hdrscmdb_set_inuse_attrib called with invalid protocol name", 
         0, 0, 0 );
    return;
  }

  /*--------------------------------------------------------------------------
   Store inuse configuration attribute's value in SCMDB
  --------------------------------------------------------------------------*/
  /* For simple attribute complex_val_ptr is NULL */
  if (complex_val_ptr == NULL)
  {
    attrib_val_ptr = &value_id;
  }
  else /* Complex attribute */
  {
    attrib_val_ptr = complex_val_ptr;
  }

  /* Get attribute's offset and size */
  (void) pip->get_attrib_offset_and_size(
            attrib_id, &attrib_offset, &attrib_size, subtype); 

  /* Get pointer to protocol's inuse configuration structure */
  curr_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset); 

  /* Commit the changed attribute's value to SCMDB */
  hdrscmdb_store_attrib(curr_cfg_ptr, attrib_offset, attrib_val_ptr, 
                        attrib_size);

  /*-------------------------------------------------------------------------
   Update the version number of attribute 
  --------------------------------------------------------------------------*/
  if (pip->get_verno_index != NULL)
  {
    /* Get index of attribute's version number in version number array */
    (void) pip->get_verno_index(attrib_id, &attrib_verno_index, subtype);

    /* Get pointer to attribute's version number value */
    attrib_verno_ptr = hdrscmdb_get_attrib_verno_handle(pip->verno_offset,
                                                        attrib_verno_index); 
                                                      
    /* Increment the version number */
    (*attrib_verno_ptr)++ ;

  }

} /* hdrscmdb_set_inuse_attrib() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_INUSE_CONFIG

DESCRIPTION
  This function sets the protocol's inuse configuration values to the value 
  given.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name    - The protocol that the attributes belong to
  cfg_data_ptr     - Pointer to the protocol's configuration data
  curr_personality - AT's current personality index
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_set_inuse_config
( 
  hdrhai_protocol_name_enum_type  protocol_name,
  void                           *cfg_data_ptr,
  uint16                          curr_personality
)
{
  byte *curr_cfg_ptr;
    /* Pointer to protocol's inuse configuration structure in DB */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_set_inuse_config called with invalid protocol name", 
          0, 0, 0 );
    return;
  }
    
  /*--------------------------------------------------------------------------
   Store inuse configuration attribute values in SCMDB
  --------------------------------------------------------------------------*/
  /* Get pointer to protocol's inuse configuration structure */
  curr_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset);
    
  /* Commit the changed configuration to SCMDB */
  hdrscmdb_store_config_attribs(curr_cfg_ptr, cfg_data_ptr, pip->cfg_size);
    
} /* hdrscmdb_set_inuse_config() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_NEGOTIATED_ATTRIB

DESCRIPTION
  This function sets the negotiated value of the specified attribute to the 
  value given. (for the current personality)

DEPENDENCIES
  None.

PARAMETERS
  protocol_name   - The protocol that the attribute is for
  attrib_id       - AttributeID
  value_id        - Value for simple attributes, ValueID for complex attributes
  complex_val_ptr - Pointer to complex data value, or NULL for simple attrib
  subtype         - Negotiated subtype of the protocol
  curr_personality - AT's current personality index
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_set_negotiated_attrib
( 
  hdrhai_protocol_name_enum_type  protocol_name,
  uint16                          attrib_id,
  uint16                          value_id,
  void                           *complex_val_ptr,
  uint16                          subtype,
  uint16                          curr_personality
)
{
  uint16 attrib_size = 0;   
    /* Size of attribute value */
  uint16 attrib_offset = 0; 
    /* Offset of attribute in protocol configuration structure */ 
  void *attrib_val_ptr;  
    /* Convenience pointer that points to value of the attribute */
  byte *neg_cfg_ptr;
    /* Pointer to protocol's inuse configuration structure in DB */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_set_negotiated_attrib called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /*--------------------------------------------------------------------------
   Store negotiated configuration attribute's value in SCMDB
  --------------------------------------------------------------------------*/
  /* For simple attribute complex_val_ptr is NULL */
  if (complex_val_ptr == NULL)
  {
    attrib_val_ptr = &value_id;
  }
  else /* Complex attribute */
  {
    attrib_val_ptr = complex_val_ptr;
  }

  /* Get attribute's offset and size */
  (void) pip->get_attrib_offset_and_size(
            attrib_id, &attrib_offset, &attrib_size, subtype); 

  /* Get pointer to protocol's inuse configuration structure */
  neg_cfg_ptr = hdrscmdb_get_negotiated_config_handle(curr_personality,
                                                      pip->cfg_offset);

  /* Commit the changed attribute's value to SCMDB */
  hdrscmdb_store_attrib(neg_cfg_ptr, attrib_offset,  attrib_val_ptr, 
                        attrib_size);

} /* hdrscmdb_set_negotiated_attrib() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_NEGOTIATED_CONFIG

DESCRIPTION
  This function sets the protocol's negotiated configuration values to the 
  value given. (for the current personality)

DEPENDENCIES
  None.

PARAMETERS
  protocol_name - The protocol that the attribute is for
  cfg_data_ptr  - Pointer to the protocol's inuse configuration data
  curr_personality - AT's current personality index
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_set_negotiated_config
( 
  hdrhai_protocol_name_enum_type  protocol_name,
  void                           *cfg_data_ptr,
  uint16                          curr_personality
)
{
  byte *neg_cfg_ptr;
    /* Pointer to protocol's negotiated configuration structure */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/
  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_set_negotiated_config called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /*--------------------------------------------------------------------------
   Store negotiated configuration attribute values in SCMDB
  --------------------------------------------------------------------------*/
  /* Get pointer to protocol's negotiated configuration structure */
  neg_cfg_ptr = hdrscmdb_get_negotiated_config_handle(curr_personality,
                                                      pip->cfg_offset);

  /* Commit the changed configuration to SCMDB */
  hdrscmdb_store_config_attribs(neg_cfg_ptr, cfg_data_ptr, pip->cfg_size);

} /* hdrscmdb_set_negotiated_config() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_NEGOTIATED_TO_INCONFIG

DESCRIPTION
  This function sets the negotiated values of the protocol in given personality
  to the inconfig values. 

DEPENDENCIES
  None.

PARAMETERS
  pindex_store - specifies the personality that was negotiated.
  pip          - Pointer to protocol's registration entry in SCMDB reg table
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_set_negotiated_to_inconfig
( 
  uint8 pindex_store, 
  hdrscmdb_reg_type const *pip
)
{
  uint8 pindex;
  byte  *neg_cfg_ptr;
    /* Pointer to protocol's negotiated config instance in given personality */
  void  *inconfig_ptr; 
    /* Pointer to protocol's inconfig instance */
  hdrscp_config_params_type *scp_in_cfg_ptr;
  hdrscp_per_personality_config_params_type *scp_pp_cfg_ptr;
  void* tmp_ptr;
    /* Temporary pointer */

/*---------------------------------------------------------------------------*/
  /* Get handle to protocol's inconfig instance */
  inconfig_ptr = pip->get_inconfig_handle();

  /* Since SCP is half way hardlink copy only the per personality config 
     data from inconfig instance to the personality_index_store. Hardlink 
     config attributes will be committed to database after the entire 
     session negotiation is complete */
  if (pip->protocol_name == HDRHAI_SESSION_CONFIGURATION_PROTOCOL)
  {
    /* Get handle to protocol's config buffer in "pindex_store" personality */
    neg_cfg_ptr = hdrscmdb_get_negotiated_config_handle( pindex_store,
                                                         pip->cfg_offset );

    scp_in_cfg_ptr = (hdrscp_config_params_type*)inconfig_ptr;

    /* Use void pointer as a bridge to avoid the following compiler warning: 
       "cast increases required alignment of target type". */
    tmp_ptr = (void *)neg_cfg_ptr;
    scp_pp_cfg_ptr = (hdrscp_per_personality_config_params_type*)tmp_ptr;  

    /* Set SCP's per personality attributes */
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy(scp_pp_cfg_ptr->subtype, 
            sizeof(scp_pp_cfg_ptr->subtype), 
            scp_in_cfg_ptr->subtype, 
            sizeof(scp_in_cfg_ptr->subtype));
#else
    memcpy(scp_pp_cfg_ptr->subtype, 
           scp_in_cfg_ptr->subtype, 
           sizeof(scp_in_cfg_ptr->subtype));
#endif /* FEATURE_MEMCPY_REMOVAL */
  }
  else
  {
    if (hdrscmdb_protocol_subtype_is_hardlink(pindex_store, pip->protocol_name) || 
        (pindex_store == HDRSCMDB_MAIN_PERSONALITY))
    {
      /* For protocols with hardlink subtype set the main personality's
         negotiated configuration to newly negotiated configuration 
         parameters. */
      hdrscmdb_set_negotiated_config(pip->protocol_name, 
                                     inconfig_ptr,
                                     HDRSCMDB_MAIN_PERSONALITY);

      /* Also update the protocol's configuration in all other personalities
         in which it is hardlinked */
      for (pindex = 1; pindex < HDRSCMDB_MAX_PERSONALITIES; pindex++)
      {
        if (hdrscmdb_protocol_subtype_is_hardlink(pindex, pip->protocol_name))
        {
          hdrscmdb_set_negotiated_config(pip->protocol_name, 
                                         inconfig_ptr, 
                                         pindex);
        }
      }
    } 
    else
    {
      hdrscmdb_set_negotiated_config(pip->protocol_name, 
                                     inconfig_ptr, 
                                     pindex_store);
    }
  }

} /* hdrscmdb_set_negotiated_to_inconfig() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_NEGOTIATED_TO_PREVIOUS

DESCRIPTION
  This function sets the negotiated values to what is stored as the prior 
  session (for all personalities).

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_set_negotiated_to_previous( void )
{
  uint8 pindex = 0;
    /* Loop index up to maximum number of personalities */
/*---------------------------------------------------------------------------*/
  /* do not copy prev_amp_info to amp_info, since AMP manages its own
     session info and will update hdrscmdb's copy accordingly */
  hdrscmdb.scp_info = hdrscmdb.prev_scp_info;
  hdrscmdb.lup_info = hdrscmdb.prev_lup_info;

  for ( pindex = 0; pindex < HDRSCMDB_MAX_PERSONALITIES; pindex++ )
  {
    hdrscmdb.personality[pindex] = hdrscmdb.previous[pindex];
  }

} /* hdrscmdb_set_negotiated_to_previous() */
        
/*=============================================================================
FUNCTION HDRSCMDB_SET_INUSE_TO_NEGOTIATED

DESCRIPTION
  This function sets the value of the "in use" instances of session information
  to the negotiated values of the specified personality index. This function 
  is called whenever personality changes

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_set_inuse_to_negotiated( void )
{
  uint8 curr_personality = hdrscmdb_get_inuse_personality();
/*---------------------------------------------------------------------------*/
  ASSERT (curr_personality < HDRSCMDB_MAX_PERSONALITIES);

  /* Copy negotiated configuration to INUSE */
  hdrscmdb.inuse = hdrscmdb.personality[curr_personality];
  
} /* hdrscmdb_set_inuse_to_negotiated() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_PREVIOUS_TO_NEGOTIATED

DESCRIPTION
  This function sets the value of the previous session to be the currently
  negotiated session information ( for all personalities ).

DEPENDENCIES
  hdramp_set_previous_to_current() is called before this function is 
  called, and updated the amp_info for current session.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_set_previous_to_negotiated( void )
{
  uint8 pindex = 0;
    /* Loop index up to maximum number of personalities */
/*---------------------------------------------------------------------------*/
  hdrscmdb.prev_scp_info = hdrscmdb.scp_info;
  hdrscmdb.prev_lup_info = hdrscmdb.lup_info;
  hdrscmdb.prev_amp_info = hdrscmdb.amp_info;

  for ( pindex = 0; pindex < HDRSCMDB_MAX_PERSONALITIES; pindex++ )
  {
    hdrscmdb.previous[pindex] = hdrscmdb.personality[pindex];
  }
} /* hdrscmdb_set_previous_to_negotiated() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_INUSE_TO_DEFAULT

DESCRIPTION
  This function sets the protocol's inuse configuration value to default
  values.

DEPENDENCIES
  None.

PARAMETERS
  None.
   
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_inuse_to_default( void )
{
  uint8 cnt; 
    /* Loop Counter */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
  byte *curr_cfg_ptr;
    /* Pointer to protocol's inuse configuration structure in DB */
/*---------------------------------------------------------------------------*/
  /* When AT powers up with no session or session is closed set all 
     personalities as invalid */
  for ( cnt = 0; cnt < HDRSCMDB_MAX_PERSONALITIES; cnt++ )
  {
    hdrscmdb.personality[cnt].personality_is_valid = FALSE;
  }

  /* Set current configuration values to default */
  for ( cnt = 0; cnt < HDRSCMDB_NUM_REG_PROTOCOLS; cnt++ )
  {
    pip = &hdrscmdb_reg[cnt];

    curr_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset); 

#ifdef FEATURE_LTE_TO_HDR_OH
    if ((pip->protocol_name == HDRHAI_RTC_MAC_PROTOCOL) &&
        (hdrsap_tunnel_mode_is_enabled(HDRHAI_SESSION_CONFIG_MANAGER)))
    {
      pip->set_config_to_default(curr_cfg_ptr, HDRSCP_SUB3_RMAC);
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    {
      pip->set_config_to_default(curr_cfg_ptr, HDRSCP_DEFAULT_PROTOCOL);
    }
  }

  /* Reset LUP and SCP session information data to default */
  hdrlup_set_session_info_to_default(&hdrscmdb.lup_info);

  hdrscp_set_hardlink_attribs_to_default(&hdrscmdb.scp_info); 

} /* hdrscmdb_set_inuse_to_default() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_SESSION_CONFIG_TOKEN

DESCRIPTION
  This function sets the inuse SessionConfigToken to the given value
  
DEPENDENCIES
  None.

PARAMETERS
  sess_cfg_token - Value to which inuse session config token have to set to
     
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_session_config_token(uint16 sess_cfg_token)
{
  /*-------------------------------------------------------------------------
  Store Session Config Token in SCMDB
  --------------------------------------------------------------------------*/
  hdrscmdb.scp_info.config_token = sess_cfg_token;

} /* hdrscmdb_set_session_config_token() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_INUSE_PERSONALITY

DESCRIPTION
  This function sets the current personality index to the given value
  
DEPENDENCIES
  None.

PARAMETERS
  pindex - Value to which current personality index have to set to
     
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_inuse_personality(uint16 pindex)
{
  if (hdrscmdb.scp_info.personality_count > 1)
  {
    hdrscmdb.curr_personality = pindex;
  }
  else
  {
    hdrscmdb.curr_personality = HDRSCMDB_MAIN_PERSONALITY;
  }
} /* hdrscmdb_set_inuse_personality() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_INUSE_ATTRIB
  
DESCRIPTION
  This function gets the inuse value of the attribute ID. The value is 
  returned as pointer to the data item.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name  - The protocol that the attribute is for
  attrib_id      - AttributeID
  attrib_val_ptr - Pointer to the attibute value, for complex attribute this 
                  points to struct.
  subtype        - Negotiated subtype of the protocol
  
RETURN VALUE
  Returns the version number of the attribute.

SIDE EFFECTS
  None
=============================================================================*/
uint8 hdrscmdb_get_inuse_attrib
(
  hdrhai_protocol_name_enum_type  protocol_name,
  uint16                          attrib_id,
  void                           *attrib_val_ptr,
  uint16                          subtype
)
{
  uint16 attrib_size = 0;   
    /* Size of attribute value */
  uint16 attrib_offset = 0; 
    /* Offset of attribute in protocol configuration structure */ 
  byte *curr_cfg_ptr;
    /* Pointer to protocol's inuse configuration structure */
  uint16 attrib_verno_index = 0;
    /* Index that points to attributes version number in verno array */ 
  uint8 *attrib_verno_ptr;
    /* Convenience pointer to attribute version number */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
  uint8 attrib_verno = 0;
    /* Attribute version number */
/*---------------------------------------------------------------------------*/
  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_get_inuse_attrib called with invalid protocol name", 
          0, 0, 0 );
    return 0xFF;
  }

  /*--------------------------------------------------------------------------
   Get inuse attribute value from SCMDB
  --------------------------------------------------------------------------*/
  /* Get attribute's offset and size */
  (void) pip->get_attrib_offset_and_size(
            attrib_id, &attrib_offset, &attrib_size, subtype); 

  /* Get pointer to protocol's inuse configuration structure */
  curr_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset);

  /* Get copy of attributes inuse value from SCMDB */
  hdrscmdb_retrieve_attrib(attrib_val_ptr, curr_cfg_ptr, attrib_offset, 
                           attrib_size);

  /*-------------------------------------------------------------------------
   Get the version number of the attribute
  --------------------------------------------------------------------------*/
  if (pip->get_verno_index != NULL)
  {
    /* Get index of attribute's version number in version number array */
    (void) pip->get_verno_index( attrib_id, &attrib_verno_index, subtype );

    attrib_verno_ptr = hdrscmdb_get_attrib_verno_handle(pip->verno_offset, 
                                                        attrib_verno_index);

    attrib_verno = (*attrib_verno_ptr);

  }

  return attrib_verno;

} /* hdrscmdb_get_inuse_attrib() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_INUSE_CONFIG
  
DESCRIPTION
 This function returns the given protocol's inuse configuration parameters.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name - The protocol that the attribute is for
  cfg_data_ptr  - Pointer to the protocol configuration structure where
                  the configuration data need to be copied to
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_inuse_config
(
  hdrhai_protocol_name_enum_type  protocol_name,
  void                           *cfg_data_ptr
)
{
  byte *curr_cfg_ptr;
    /* Pointer to protocol's inuse configuration structure */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/
  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_get_inuse_config called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /*--------------------------------------------------------------------------
   Get inuse configuration value from SCMDB
  --------------------------------------------------------------------------*/
  /* Get pointer to protocol's inuse configuration structure */
  curr_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset);

  /* Get copy of attributes inuse value from SCMDB */
  hdrscmdb_retrieve_config_attribs(cfg_data_ptr, curr_cfg_ptr, pip->cfg_size);

} /* hdrscmdb_get_inuse_config() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_INUSE_CONFIG_AND_VERNO
  
DESCRIPTION
 This function returns the given protocol's inuse configuration and the 
 version numbers for all attribute's of the protocol.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name     - The protocol that the attribute is for
  cfg_data_ptr      - Pointer to the protocol configuration structure where
                      the configuration data need to be copied to
  attrib_verno      - Pointer to attribute version number structure to which
                      version information need to be copied to
  attrib_verno_size - Size of protocol's attrib version number structure in
                      bytes
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_inuse_config_and_verno
(
  hdrhai_protocol_name_enum_type  protocol_name,
  void                           *cfg_data_ptr, 
  void                           *attrib_verno,
  uint16                          attrib_verno_size
)
{
  byte  *curr_cfg_ptr;
    /* Pointer to protocol's inuse configuration structure */
  uint8 *attrib_verno_ptr;
    /* Convenience pointer to attribute version number */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/
  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_get_inuse_config_and_verno called with invalid protocol name",
          0, 0, 0 );
    return;
  }

  /*--------------------------------------------------------------------------
   Get inuse configuration value from SCMDB
  --------------------------------------------------------------------------*/
  /* Get pointer to protocol's inuse configuration structure */
  curr_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset);

  /* Get copy of inuse config data from SCMDB */
  hdrscmdb_retrieve_config_attribs(cfg_data_ptr, curr_cfg_ptr, pip->cfg_size);

  /*--------------------------------------------------------------------------
   Get inuse version numbers from SCMDB
  --------------------------------------------------------------------------*/
  if (attrib_verno != NULL)
  {
    /* Set attrib_verno_index (2nd param) to zero to get the start address of 
       the protocols version number array */
    attrib_verno_ptr = hdrscmdb_get_attrib_verno_handle(pip->verno_offset, 0);

#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy(attrib_verno, attrib_verno_size, attrib_verno_ptr, attrib_verno_size);
#else
    memcpy(attrib_verno, attrib_verno_ptr, attrib_verno_size);
#endif /* FEATURE_MEMCPY_REMOVAL */
  }

} /* hdrscmdb_get_inuse_config_and_verno() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_RMAC_INUSE_CONFIG_AND_VERNO
  
DESCRIPTION
 This function returns the RMAC's inuse configuration and the version numbers
 for all attribute's of the protocol.

DEPENDENCIES
  None.

PARAMETERS
  cfg_data_ptr   - Pointer to the RMAC's configuration structure where
                   the configuration data need to be copied to
  cfg_data_size  - Size of configuration data                 
  attrib_verno   - Pointer to attribute version number structure to which
                   version information need to be copied to
  attrib_verno_size - Size of RMAC's attrib version number structure in
                      bytes
  subtype        - RMAC's subtype                      
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_rmac_inuse_config_and_verno
(
  void                           *cfg_data_ptr, 
  uint32                          cfg_data_size,
  void                           *attrib_verno,
  uint16                          attrib_verno_size,
  uint16                          subtype

)
{
  byte  *curr_cfg_ptr;
    /* Pointer to protocol's inuse configuration structure */
  uint8 *attrib_verno_ptr;
    /* Convenience pointer to attribute version number */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/
  /* Make sure that the calling routine is not asking for more data than
     the buffer size that is passed in */
  if ((subtype == HDRSCP_DEFAULT_RMAC) || (subtype == HDRSCP_SUB1_RMAC))
  {
    ASSERT (cfg_data_size == sizeof(hdrscmrmac0_config_struct_type));
  }
  else if (subtype == HDRSCP_SUB3_RMAC)
  {
    ASSERT (cfg_data_size == sizeof(hdrscmrmac3_config_struct_type));
  }
#ifdef FEATURE_HDR_REVB_SUPPORTED
  else if (subtype == HDRSCP_MC_RMAC)
  {
    ASSERT (cfg_data_size == sizeof(hdrscmrmac4_config_struct_type));
  }
#endif /* FEATURE_HDR_REVB_SUPPORTED */

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(HDRHAI_RTC_MAC_PROTOCOL);

  if (pip == NULL)
  { 
    ERR("hdrscmdb_get_rmac_inuse_config_and_verno called with invalid protocol",
         0, 0, 0 );
    return;
  }

  /*--------------------------------------------------------------------------
   Get inuse configuration value from SCMDB
  --------------------------------------------------------------------------*/
  /* Get pointer to protocol's inuse configuration structure */
  curr_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset);

  /* Get copy of inuse config data from SCMDB */
  hdrscmdb_retrieve_config_attribs(cfg_data_ptr, curr_cfg_ptr, cfg_data_size);

  /*--------------------------------------------------------------------------
   Get inuse version numbers from SCMDB
  --------------------------------------------------------------------------*/
  if (attrib_verno != NULL)
  {
    /* Set attrib_verno_index (2nd param) to zero to get the start address of 
       the protocols version number array */
    attrib_verno_ptr = hdrscmdb_get_attrib_verno_handle(pip->verno_offset, 0);

#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy(attrib_verno, attrib_verno_size, attrib_verno_ptr, attrib_verno_size);
#else
    memcpy(attrib_verno, attrib_verno_ptr, attrib_verno_size);
#endif /* FEATURE_MEMCPY_REMOVAL */
  }

} /* hdrscmdb_get_rmac_inuse_config_and_verno() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_AMP_SESSION_INFO
  
DESCRIPTION
 This function copies AMP's inuse configuration information from SCMDB to the 
 location specified by the pointer passed.
  
DEPENDENCIES
  None.

PARAMETERS
  amp_cfg_ptr - Pointer to AMP's inuse configuration parameters.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_amp_session_info( hdramp_session_info_type *amp_cfg_ptr )
{
  /* Get copy of inuse config data from SCMDB */
  hdrscmdb_retrieve_config_attribs(amp_cfg_ptr,
                                   (byte*)&hdrscmdb.amp_info,
                                   sizeof(hdrscmdb.amp_info));

} /* hdrscmdb_get_amp_session_info() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_AMP_SESSION_INFO
  
DESCRIPTION
 This function copies the AMP's configuration parameters to SCMDB.
 
DEPENDENCIES
  None.

PARAMETERS
 amp_cfg_ptr - Pointer to AMP's inuse configuration parameter strcuture.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_amp_session_info
( 
  hdramp_session_info_type *amp_cfg_ptr
)
{
  /*-------------------------------------------------------------------------
   Store AMP's configuration data in SCMDB
  --------------------------------------------------------------------------*/
  /* Commit the changed attribute's value to SCMDB */
  hdrscmdb_store_config_attribs((byte*)&hdrscmdb.amp_info, 
                                 amp_cfg_ptr, 
                                 sizeof(hdramp_session_info_type));

} /* hdrscmdb_set_amp_session_info() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_SCP_SESSION_INFO
  
DESCRIPTION
 This function copies the SCP's configuration parameters from SCMDB to the 
 location specified by the pointer passed.
 
DEPENDENCIES
  None.

PARAMETERS
 scp_cfg_ptr - Pointer to SCP's inuse configuration parameter strcuture.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_scp_session_info(hdrscp_config_params_type *scp_cfg_ptr)
{
  hdrscp_hardlink_config_params_type  *scp_info = &hdrscmdb.scp_info;
/*---------------------------------------------------------------------------*/
  /* Copy the per personality attributes */
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy(scp_cfg_ptr->subtype, sizeof(scp_cfg_ptr->subtype), 
          hdrscmdb.inuse.scp.subtype, 
          sizeof(hdrscmdb.inuse.scp.subtype));
#else
  memcpy(scp_cfg_ptr->subtype, 
         hdrscmdb.inuse.scp.subtype, 
         sizeof(hdrscmdb.inuse.scp.subtype));
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* Copy the hardlink attributes */
  scp_cfg_ptr->config_token = scp_info->config_token;
  scp_cfg_ptr->personality_count = scp_info->personality_count;
  scp_cfg_ptr->support_config_lock = scp_info->support_config_lock;
  scp_cfg_ptr->support_gaup_session_config_token = 
                          scp_info->support_gaup_session_config_token;
  scp_cfg_ptr->config_is_locked = scp_info->config_is_locked;
#ifdef FEATURE_EHRPD
  scp_cfg_ptr->ehrpd_was_advertised = scp_info->ehrpd_was_advertised;
#endif /* FEATURE_EHRPD */
} /* hdrscmdb_get_scp_session_info() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_SCP_SESSION_INFO
  
DESCRIPTION
  This function is called by SCP when session negotiation is complete to commit
  the hardlink attributes to SCMDB
 
DEPENDENCIES
  None.

PARAMETERS
 scp_cfg_ptr - Pointer to SCP's inconfig instance.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_scp_session_info(hdrscp_config_params_type *scp_cfg_ptr)
{
  hdrscp_hardlink_config_params_type  *scp_info = &hdrscmdb.scp_info;
/*---------------------------------------------------------------------------*/
  /* Store hardlink attributes to SCMDB after the entire session negotiation
     is done, where as per personality attributes are stored after each 
     personality is negotiated */
  scp_info->config_token = scp_cfg_ptr->config_token;
  scp_info->personality_count = scp_cfg_ptr->personality_count;
  scp_info->support_config_lock = scp_cfg_ptr->support_config_lock;
  scp_info->support_gaup_session_config_token = 
                    scp_cfg_ptr->support_gaup_session_config_token;
  scp_info->config_is_locked = scp_cfg_ptr->config_is_locked;
#ifdef FEATURE_EHRPD
  scp_info->ehrpd_was_advertised = scp_cfg_ptr->ehrpd_was_advertised;
#endif /* FEATURE_EHRPD */
} /* hdrscmdb_set_scp_session_info() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_LUP_SESSION_INFO
  
DESCRIPTION
 This function copies LUP's inuse configuration information from SCMDB to the 
 location specified by the pointer passed.
  
DEPENDENCIES
  None.

PARAMETERS
  lup_cfg_ptr - Pointer to LUP's inuse configuration parameters.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_lup_session_info( hdrlup_session_info_type *lup_cfg_ptr )
{
  /* Get copy of inuse config data from SCMDB */
  hdrscmdb_retrieve_config_attribs(lup_cfg_ptr,
                                   (byte*)&hdrscmdb.lup_info,
                                   sizeof(hdrscmdb.lup_info));

} /* hdrscmdb_get_lup_session_info() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_LUP_SESSION_INFO
  
DESCRIPTION
 This function copies the LUP's configuration parameters to SCMDB.
 
DEPENDENCIES
  None.

PARAMETERS
 lup_cfg_ptr - Pointer to LUP's inuse configuration parameter strcuture.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_lup_session_info
( 
  hdrlup_session_info_type *lup_cfg_ptr
)
{
  /*-------------------------------------------------------------------------
   Store LUP's configuration data in SCMDB
  --------------------------------------------------------------------------*/
  /* Commit the changed attribute's value to SCMDB */
  hdrscmdb_store_config_attribs((byte*)&hdrscmdb.lup_info,
                                 lup_cfg_ptr,
                                 sizeof(hdrlup_session_info_type));

} /* hdrscmdb_set_lup_session_info() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_SCP_PREV_SESSION_INFO
  
DESCRIPTION
 This function copies the SCP's configuration parameters from SCMDB to the 
 location specified by the pointer passed.
 
DEPENDENCIES
  None.

PARAMETERS
 scp_cfg_ptr - Pointer to SCP's inuse configuration parameter strcuture.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_scp_prev_session_info
(
  hdrscp_config_params_type *scp_cfg_ptr
)
{
  uint16 prev_pindex;
    /* Session config token of the previous session */
  hdrscp_per_personality_config_params_type  *prev_cfg_ptr;
    /* Pointer to protocol's previous config data */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
  hdrscp_hardlink_config_params_type  *prev_scp_info = &hdrscmdb.prev_scp_info;
  uint8 protocol_type = 0;
    /* Name of the protocols */
  void* tmp_ptr;
    /* Temporary pointer */

/*---------------------------------------------------------------------------*/
  if (!hdrscmdb_prior_session_is_valid())
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "No Previous Session!" );
    return;
  }

  if (hdrscmdb.prev_scp_info.personality_count > 1)
  {
    /* Extract the personality index from Session Configuration Token of
       previous session */
    prev_pindex = (hdrscmdb.prev_scp_info.config_token>>12);
  }
  else
  {
    /* If there is only one personality then it is the personality at
       index zero. */
    prev_pindex = HDRSCMDB_MAIN_PERSONALITY;
  }

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(HDRHAI_SESSION_CONFIGURATION_PROTOCOL);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_get_scp_prev_session_info called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /*--------------------------------------------------------------------------
   Get previous configuration value from SCMDB
  --------------------------------------------------------------------------*/
  /* Get pointer to SCP's previous configuration data */
  tmp_ptr = (void *)hdrscmdb_get_previous_config_handle(prev_pindex, pip->cfg_offset);
  prev_cfg_ptr = (hdrscp_per_personality_config_params_type*)tmp_ptr;

  /* Copy SCP's per personality attributes */
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy(scp_cfg_ptr->subtype, sizeof(scp_cfg_ptr->subtype), 
          prev_cfg_ptr->subtype, sizeof(prev_cfg_ptr->subtype));
#else
  memcpy(scp_cfg_ptr->subtype, prev_cfg_ptr->subtype, 
         sizeof(prev_cfg_ptr->subtype));
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* For protocols whose subtype is hardlink in the given personality set it to 
     the corresponding subtype in main personality */
  for (protocol_type = 0; protocol_type < HDRHAI_PROTOCOL_TYPE_ENUM_MAX; 
       protocol_type++)
  {
    if (prev_cfg_ptr->subtype[protocol_type] == HDRSCMDB_HARDLINK_SUBTYPE)
    {
      scp_cfg_ptr->subtype[protocol_type] = 
          hdrscmdb.previous[HDRSCMDB_MAIN_PERSONALITY].scp.subtype[protocol_type];
    }
  }

  /* Copy SCP's hardlink attributes */
  scp_cfg_ptr->config_token        = prev_scp_info->config_token;
  scp_cfg_ptr->personality_count   = prev_scp_info->personality_count;
  scp_cfg_ptr->support_config_lock = prev_scp_info->support_config_lock;
  scp_cfg_ptr->support_gaup_session_config_token = 
                          prev_scp_info->support_gaup_session_config_token;
  scp_cfg_ptr->config_is_locked    = prev_scp_info->config_is_locked;
#ifdef FEATURE_EHRPD
  scp_cfg_ptr->ehrpd_was_advertised = prev_scp_info->ehrpd_was_advertised;
#endif /* FEATURE_EHRPD */
} /* hdrscmdb_get_scp_prev_session_info() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_PREVIOUS_CONFIG
  
DESCRIPTION
  This function returns the given protocol's previous configuration parameters.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Protocol for which previous config is needed
  cfg_data_ptr  - Pointer to given protocol's configuration structure where
                  the configuration data need to be copied to.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_previous_config
(
  hdrhai_protocol_name_enum_type  protocol_name,
  void                           *cfg_data_ptr
)
{
  uint16 prev_pindex;
    /* Session config token of the previous session */
  byte  *prev_cfg_ptr;
    /* Pointer to protocol's previous config data */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/
  if (!hdrscmdb_prior_session_is_valid())
  {
    ERR("No Previous Session!", 0, 0, 0 );
    return;
  }

  if (hdrscmdb.prev_scp_info.personality_count > 1)
  {
    /* Extract the personality index from Session Configuration Token of
       previous session */
    prev_pindex = (hdrscmdb.prev_scp_info.config_token>>12);
  }
  else
  {
    /* If there is only one personality then it is the personality at
       index zero. */
    prev_pindex = HDRSCMDB_MAIN_PERSONALITY;
  }

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_get_previous_config called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /*--------------------------------------------------------------------------
   Get previous configuration value from SCMDB
  --------------------------------------------------------------------------*/
  /* Get pointer to protocol's previous configuration structure */
  prev_cfg_ptr = hdrscmdb_get_previous_config_handle(prev_pindex,  
                                                     pip->cfg_offset);

  /* Get copy of attributes previous value from SCMDB */
  hdrscmdb_retrieve_config_attribs(cfg_data_ptr, prev_cfg_ptr, pip->cfg_size);

} /* hdrscmdb_get_previous_config() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_IDLE_SLOT_CYCLE_TIMEOUT
  
DESCRIPTION
  This function sets the inuse and negotiated value of IDLE slot cycle 
  timeout to the value given.

DEPENDENCIES
  None.

PARAMETERS
   t1 - System time at which to change from SlotCycle1 to SlotCycle2
   t2 - System time at which to change from SlotCcyle2 to SlotCycle3
  
RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void hdrscmdb_set_idle_slot_cycle_timeout( uint64 t1, uint64 t2 )
{
  uint8 cp_index = hdrscmdb_get_inuse_personality();
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/
  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(HDRHAI_IDLE_STATE_PROTOCOL);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_set_idle_slot_cycle_timeout called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  ASSERT (cp_index < HDRSCMDB_MAX_PERSONALITIES);

  /*-------------------------------------------------------------------------
   Store IDLE slot configuration attribute values in SCMDB
  --------------------------------------------------------------------------*/
  hdrscmdb.inuse.idle.slot_cycle1_timeout = t1;
  hdrscmdb.inuse.idle.slot_cycle2_timeout = t2;

  hdrscmdb.personality[cp_index].idle.slot_cycle1_timeout = t1;
  hdrscmdb.personality[cp_index].idle.slot_cycle2_timeout = t2;

} /* hdrscmdb_set_idle_slot_cycle_timeout() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_INCONFIG_ATTRIB

DESCRIPTION
  This function sets the inconfig instance of the specified attribute to the 
  value given.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name   - The protocol that the attribute belongs to
  attrib_id       - AttributeID
  value_id        - Value for simple attributes, ValueID for complex attributes
  complex_val_ptr - Pointer to complex data value, or NULL for simple attrib
  subtype         - Negotiated subtype of the protocol
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_inconfig_attrib
( 
  hdrhai_protocol_name_enum_type  protocol_name,
  uint16                          attrib_id,
  uint16                          value_id,
  void                           *complex_val_ptr,
  uint16                          subtype
)
{
  uint16 attrib_size = 0;   
    /* Size of attribute value */
  uint16 attrib_offset = 0; 
    /* Offset of attribute in protocol configuration structure */ 
  void *attrib_val_ptr;  
    /* Convenience pointer that points to value of the attribute */
  byte *inconfig_ptr;
    /* Pointer to protocol's inconfig instance */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/
  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_set_inconfig_attrib called with invalid protocol name", 
          0, 0, 0 );
    return;
  }
  
  /*--------------------------------------------------------------------------
   Store negotiated configuration attribute's value in SCMDB
  --------------------------------------------------------------------------*/
  /* For simple attribute complex_val_ptr is NULL */
  if (complex_val_ptr == NULL)
  {
    attrib_val_ptr = &value_id;
  }
  else /* Complex attribute */
  {
    attrib_val_ptr = complex_val_ptr;
  }
  
  /* Get attribute's offset and size */
  (void) pip->get_attrib_offset_and_size(
            attrib_id, &attrib_offset, &attrib_size, subtype); 
  
  /* Get pointer to protocol's inconfig instance */
  inconfig_ptr = pip->get_inconfig_handle();
                                                           
  /* Commit the changed attribute's value to SCMDB */
  hdrscmdb_store_attrib(inconfig_ptr, 
                        attrib_offset,
                        attrib_val_ptr, 
                        attrib_size);
  
} /* hdrscmdb_set_inconfig_attrib() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_GAUPED_ATTRIB

DESCRIPTION
  This function is called to update the database of gauped attribute
   
DEPENDENCIES
  None.

PARAMETERS
  protocol_name   - The protocol that the attribute is for
  attrib_id       - AttributeID
  value_id        - Value for simple attributes, ValueID for complex attributes
  complex_val_ptr - Pointer to complex data value, or NULL for simple attrib
  subtype         - Negotiated subtype of the protocol
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_gauped_attrib
( 
  hdrhai_protocol_name_enum_type protocol_name,
  uint16                         attrib_id,
  uint16                         value_id,
  void                           *complex_val_ptr,
  uint16                         subtype
)
{
  uint8 curr_personality = hdrscmdb_get_inuse_personality();
  uint8 index;
    /* Loop Index */
/*---------------------------------------------------------------------------*/
  HDR_MSG_PROT_4 (MSG_LEGACY_LOW, "SCMDB: P=%d St=%d Attrib= %d Value=%d GAUP'ed ", 
                                    protocol_name, subtype, attrib_id, value_id);

  ASSERT (curr_personality < HDRSCMDB_MAX_PERSONALITIES);

  /* If the protocol subtype is HARDLINK in current personality update the 
     NEGOTIATED instance of protocol configuration in Main personality and 
     also all other personalities in which the protocol is hardlinked */
  if (hdrscmdb_protocol_subtype_is_hardlink(curr_personality, protocol_name) ||
      (curr_personality == HDRSCMDB_MAIN_PERSONALITY))
  {
    hdrscmdb_set_negotiated_attrib( protocol_name, 
                                    attrib_id, 
                                    value_id, 
                                    complex_val_ptr, 
                                    subtype,
                                    HDRSCMDB_MAIN_PERSONALITY );

    for ( index = 1; index < HDRSCMDB_MAX_PERSONALITIES; index++ )
    {
      if (hdrscmdb_protocol_subtype_is_hardlink(index, protocol_name))
      {
        /* update the negotiated instance of the personality */
        hdrscmdb_set_negotiated_attrib( protocol_name, 
                                        attrib_id, 
                                        value_id, 
                                        complex_val_ptr, 
                                        subtype,
                                        index );
      }
    }
  }
  /* Otherwise update only the NEGOTIATED instance of protocol configuration
     in current personality */
  else
  {
    hdrscmdb_set_negotiated_attrib(protocol_name,
                                   attrib_id,
                                   value_id,
                                   complex_val_ptr,
                                   subtype,
                                   curr_personality);
  }

  /* update the INUSE instance of given protocol's attribute and also
     increment the verno */
  hdrscmdb_set_inuse_attrib(protocol_name, 
                            attrib_id, 
                            value_id,
                            complex_val_ptr, 
                            subtype,
                            curr_personality);

} /* hdrscmdb_set_gauped_attrib() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_GAUPED_CONFIG

DESCRIPTION
  This function is called to update the database of gauped config attributes

DEPENDENCIES
  None.

PARAMETERS
  protocol_name    - The protocol that the attribute is for
  config_data_ptr  - Pointer to the protocol's current configuration data
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_gauped_config
( 
  hdrhai_protocol_name_enum_type protocol_name,
  void                           *config_data_ptr
)
{
  uint8 curr_personality = hdrscmdb_get_inuse_personality();
  uint8 index;
    /* Loop Index */
/*---------------------------------------------------------------------------*/
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "SCMDB: P=%d GAUP'ed ", protocol_name);

  ASSERT (curr_personality < HDRSCMDB_MAX_PERSONALITIES);

  /* If the protocol subtype is HARDLINK in current personality update the 
     NEGOTIATED instance of protocol configuration in Main personality and 
     also all other personalities in which the protocol is hardlinked */
  if (hdrscmdb_protocol_subtype_is_hardlink(curr_personality, protocol_name) ||
      (curr_personality == HDRSCMDB_MAIN_PERSONALITY))
  {
    hdrscmdb_set_negotiated_config( protocol_name,
                                    config_data_ptr,
                                    HDRSCMDB_MAIN_PERSONALITY );

    for ( index = 1; index < HDRSCMDB_MAX_PERSONALITIES; index++ )
    {
      if (hdrscmdb_protocol_subtype_is_hardlink(index, protocol_name))
      {
        /* update the negotiated instance of the personality */
        hdrscmdb_set_negotiated_config( protocol_name,
                                        config_data_ptr,
                                        index );
      }
    }
  }
  /* Otherwise update only the NEGOTIATED instance of protocol configuration
     in current personality */
  else
  {
    hdrscmdb_set_negotiated_config( protocol_name,
                                    config_data_ptr,
                                    curr_personality );
  }

  /* and update the INUSE instance of protocol configuration */
  hdrscmdb_set_inuse_config(protocol_name, config_data_ptr, curr_personality);

} /* hdrscmdb_set_gauped_config() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_INCONFIG_ATTRIB_TO_INUSE

DESCRIPTION
  This function sets the inconfig instance of the specified attribute to the
  inuse (fall-back) value.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - The protocol that the attribute is for
  attrib_id           - AttributeID
  subtype             - Negotiated subtype of the protocol
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_inconfig_attrib_to_inuse
( 
  hdrhai_protocol_name_enum_type protocol_name,
  uint16                         attrib_id,
  uint16                         config_subtype
)
{
  uint16 attrib_size = 0;   
    /* Size of attribute value */
  uint16 attrib_offset = 0; 
    /* Offset of attribute in protocol configuration structure */ 
  byte *fallback_cfg_ptr;
    /* Pointer to protocol's inuse configuration structure */
  byte *inconfig_ptr;
    /* Pointer to protocol's inconfig configuration structure */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
  uint16 curr_subtype = hdrscp_get_current_subtype(protocol_name);
    /* Protocol's current subtype */
/*---------------------------------------------------------------------------*/
  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_set_inconfig_attrib_to_inuse called with invalid prot name", 
          0, 0, 0 );
    return;
  }

  /* Get pointer to protocol's inconfig instance */
  inconfig_ptr = pip->get_inconfig_handle();

  /* Get pointer to protocol's fallback config instance */
  if (curr_subtype == config_subtype)
  {
    /* If config subtype is same as current subtype then the fallback value 
       is the inuse value. So set the fallback_cfg_ptr to inuse config */
    fallback_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset);
  }
  else 
  {
    /* If the config and current subtype are different, the fallback value
       will be the default value of the attribute in the config subtype. 
       In order to find the default value, we temporarily initialize
       the given protocol's config struct in hdrscmdb.protocol_default_cfg
       with default attribute values and use this as fallback_cfg_ptr */

    ASSERT (pip->incfg_cfg_offset != HDRSCMDB_DATA_INVALID);

    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
        "Attrib rejected by AT resetting to config subtype defaults");

    fallback_cfg_ptr = (byte*)&hdrscmdb.protocol_default_cfg +
                        pip->incfg_cfg_offset;

    pip->set_config_to_default(fallback_cfg_ptr, config_subtype);
  }

  /* Get attribute's offset and size */
  (void) pip->get_attrib_offset_and_size(
            attrib_id, &attrib_offset, &attrib_size, config_subtype); 

  /* Set the inconfig to the fallback value of the attribute */
  hdrscmdb_store_attrib(inconfig_ptr, 
                        attrib_offset, 
                        (fallback_cfg_ptr + attrib_offset), 
                        attrib_size);

} /* hdrscmdb_set_inconfig_attrib_to_inuse() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_INCONFIG_TO_INUSE

DESCRIPTION
  This function sets the value of the "in config" instances to the inuse 
  values e.g. at the beginning of negotiation.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_inconfig_to_inuse( void )
{
  void *inconfig_ptr;
    /* Pointer to protocol's inconfig instance */
  byte *curr_cfg_ptr;
    /* Pointer to protocol's inuse instance */
  uint8 index;
    /* Loop index */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
  hdrscp_config_params_type *scp_in_cfg_ptr;
/*---------------------------------------------------------------------------*/

  /* when AT starts negotation, the current session data is no longer
     valid, however, if a previous session was valid, it is unaffected */
  if (hdrscmdb.session_db_status == HDRSCMDB_SESSION_CURRENT)
  {
    hdrscmdb.session_db_status = HDRSCMDB_SESSION_INVALID;
  }

  for (index = 0; index < HDRSCMDB_NUM_REG_PROTOCOLS; index++)
  {
    pip = &hdrscmdb_reg[index];

    /* Since AMP is hardlinked in all personalities we do not reset inconfig
       instance to inuse values for each personality negotiation, instead we 
       continue with the values that have been negotiated so far. AT the time 
       of storing a given personality in the database SCMDB updates the AMP's 
       negotiated configuration values in each of the valid personality. This
       way the AMP configuration attributes are same in all personalities */
    if (pip->protocol_name == HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL)
    {
      continue;
    }

    /* Get a pointer to legacy protocol's inconfig instance */
    inconfig_ptr = pip->get_inconfig_handle();

    /* Get a pointer to inuse instance of the protocol in DB */
    curr_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset); 

    /* Since each personality has only the subtype's attribute SCP needs to be 
       handled separately */
    if (pip->protocol_name == HDRHAI_SESSION_CONFIGURATION_PROTOCOL)
    {
      scp_in_cfg_ptr = (hdrscp_config_params_type*)inconfig_ptr;

      /* Set SCP's per personality attributes */
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( (void*) scp_in_cfg_ptr->subtype, sizeof(scp_in_cfg_ptr->subtype),
               curr_cfg_ptr, 
               pip->cfg_size );
#else
      memcpy( (void*) scp_in_cfg_ptr->subtype, 
             curr_cfg_ptr, 
             pip->cfg_size );
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
    else
    {
      /* Copy the inuse instance to inconfig */
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy(inconfig_ptr, pip->cfg_size, curr_cfg_ptr, pip->cfg_size);
#else
      memcpy(inconfig_ptr, curr_cfg_ptr, pip->cfg_size);
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
  }

} /* hdrscmdb_set_inconfig_to_inuse() */

/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  attrib_verno_val    - inuse version number for the attributes.
  attrib_verno_offset - Offset of each attribute in its version number list
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
)
{
  uint8 index = 0; 
    /* Loop index up to max of num_attribs specified by callign function */
  uint8 *attrib_verno_ptr;
    /* Pointer to attributes version number value */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_find_changed_attribs called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /* Initialize the is_changed flags to FALSE */
  memset(is_changed, 0, num_attribs);

  for ( index = 0; index < num_attribs; index++ )
  {
    /* Get pointer to version number of SCMDB's copy of the attribute */
    attrib_verno_ptr = hdrscmdb_get_attrib_verno_handle( 
                         pip->verno_offset, 
                         attrib_verno_offset[index]);

    /* Compare protocol's copy of attrib version number with that in SCMDB */
    if ( (*attrib_verno_ptr) != attrib_verno_val[index] )
    {
      is_changed[index] = TRUE;
    }
  }

} /* hdrscmdb_find_changed_attribs() */

/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_NN_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array. This function is
 invoked by protocols that have per flow attributes.
 (Example: RMAC's AssociatedFlowsNN)
 
DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  num_flownn          - Number of flows
  attrib_verno_val    - Version number of protocol's copy of the attributes.
                        Version number for each attribute for each flow have
                        to be specified.
  attrib_verno_offset - Offset of the attribute in its version number list. 
                        Offset of only the first element of given flowNN 
                        attribute need to be specified.
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
  Length of "attrib_verno_val" and "is_changed" array is 
  "num_attribs" X "num_flownn"
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_nn_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8                          num_flownn,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
)
{
  uint8 index = 0; 
    /* Loop index up to max of num_attribs specified by callign function */
  uint8 flow_nn = 0;
    /* Loop index up to max of num_flownn specified by calling function */
  uint8 *attrib_verno_ptr;
    /* Pointer to attributes version number value */
  uint16 offset;
    /* Offset of particular flow NN attribute */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_find_changed_nn_attribs called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /* Initialize the is_changed flags to FALSE */
  memset(is_changed, 0, (num_attribs * num_flownn));

  for ( index = 0; index < num_attribs; index++ )
  {
    for ( flow_nn = 0; flow_nn < num_flownn; flow_nn++ )
    {
      /* Offset of attribute version number value for a given flow in the
         version num array */
      offset = attrib_verno_offset[index] + flow_nn;

      /* Get pointer to version number of SCMDB's copy of the attribute */
      attrib_verno_ptr = hdrscmdb_get_attrib_verno_handle(
                           pip->verno_offset,
                           offset);

      /* Compare protocol's copy of attrib version number with that in SCMDB */
      if ( (*attrib_verno_ptr) != attrib_verno_val[(index * num_flownn) + flow_nn] )
      {
        is_changed[(index * num_flownn) + flow_nn] = TRUE;
      }

    } /* for each flow(NN) of a given attribute */

  } /* for each attribute */

} /* hdrscmdb_find_changed_nn_attribs() */

#ifdef FEATURE_HDR_REVB_SUPPORTED
/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_CC_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array. This function is
 invoked by protocols that have per Reverse Logical Channel attributes.
 (Example: RMAC4's MaxNumSubPacketsCC)
 
DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  num_channelcc       - Number of Reverse Logical Channels
  attrib_verno_val    - Version number of protocol's copy of the attributes.
                        Version number for each attribute for each flow have
                        to be specified.
  attrib_verno_offset - Offset of the attribute in its version number list. 
                        Offset of only the first element of given CC 
                        attribute need to be specified.
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
  Length of "attrib_verno_val" and "is_changed" array is 
  "num_attribs" X "num_channelcc"
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_cc_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8                          num_channelcc,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
)
{
  uint8 index = 0; 
    /* Loop index up to max of num_attribs specified by callign function */
  uint8 channel_cc = 0;
    /* Loop index up to max of num_channelcc specified by calling function */
  uint8 *attrib_verno_ptr;
    /* Pointer to attributes version number value */
  uint16 offset;
    /* Offset of particular channel CC attribute */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_find_changed_cc_attribs called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /* Initialize the is_changed flags to FALSE */
  memset(is_changed, 0, (num_attribs * num_channelcc));

  for ( index = 0; index < num_attribs; index++ )
  {
    for ( channel_cc = 0; channel_cc < num_channelcc; channel_cc++ )
    {
      /* Offset of attribute version number value for a given channel in the
         version num array */
      offset = attrib_verno_offset[index] + channel_cc;

      /* Get pointer to version number of SCMDB's copy of the attribute */
      attrib_verno_ptr = hdrscmdb_get_attrib_verno_handle(
                           pip->verno_offset,
                           offset);

      /* Compare protocol's copy of attrib version number with that in SCMDB */
      if ( (*attrib_verno_ptr) != attrib_verno_val[(index * num_channelcc) + channel_cc] )
      {
        is_changed[(index * num_channelcc) + channel_cc] = TRUE;
      }

    } /* for each channel(CC) of a given attribute */

  } /* for each attribute */

} /* hdrscmdb_find_changed_cc_attribs() */

/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_XX_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array. This function is
 invoked by protocols that have per Configuration attributes.
 (Example: RMAC4's PilotStrengthConfigXX)
 
DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  num_configxx        - Number of configurations
  attrib_verno_val    - Version number of protocol's copy of the attributes.
                        Version number for each attribute for each flow have
                        to be specified.
  attrib_verno_offset - Offset of the attribute in its version number list. 
                        Offset of only the first element of given configXX
                        attribute need to be specified.
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
  Length of "attrib_verno_val" and "is_changed" array is 
  "num_attribs" X "num_configxx"
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_xx_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8                          num_configxx,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
)
{
  uint8 index = 0; 
    /* Loop index up to max of num_attribs specified by callign function */
  uint8 config_xx = 0;
    /* Loop index up to max of num_configxx specified by calling function */
  uint8 *attrib_verno_ptr;
    /* Pointer to attributes version number value */
  uint16 offset;
    /* Offset of particular config XX attribute */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_find_changed_xx_attribs called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /* Initialize the is_changed flags to FALSE */
  memset(is_changed, 0, (num_attribs * num_configxx));

  for ( index = 0; index < num_attribs; index++ )
  {
    for ( config_xx = 0; config_xx < num_configxx; config_xx++ )
    {
      /* Offset of attribute version number value for a given flow in the
         version num array */
      offset = attrib_verno_offset[index] + config_xx;

      /* Get pointer to version number of SCMDB's copy of the attribute */
      attrib_verno_ptr = hdrscmdb_get_attrib_verno_handle(
                           pip->verno_offset,
                           offset);

      /* Compare protocol's copy of attrib version number with that in SCMDB */
      if ( (*attrib_verno_ptr) != attrib_verno_val[(index * num_configxx) + config_xx] )
      {
        is_changed[(index * num_configxx) + config_xx] = TRUE;
      }

    } /* for each config(XX) of a given attribute */

  } /* for each attribute */

} /* hdrscmdb_find_changed_xx_attribs() */

/*=============================================================================
FUNCTION HDRSCMDB_FIND_CHANGED_NC_ATTRIBS
  
DESCRIPTION
 This function finds if the given set of attributes have changed and sets the
 appropriate flag to TRUE in the "is_changed" boolean array. This function is
 invoked by protocols that have per flow attributes.
 (Example: RMAC's AssociatedFlowsNN)
 
DEPENDENCIES
  None.

PARAMETERS
  protocol_name       - Protocol to which these attributes belong to.
  num_attribs         - Number of attributes for which version number have to 
                        checked
  num_flownn          - Number of flows
  num_channelcc       - Number of Reverse Logical Channels
  attrib_verno_val    - Version number of protocol's copy of the attributes.
                        Version number for each attribute for each flow have
                        to be specified.
  attrib_verno_offset - Offset of the attribute in its version number list. 
                        Offset of only the first element of given NC
                        attribute need to be specified.
  is_changed          - Boolean list that is set by this function to notify
                        that the value has been changed for the attribute.
  
  Length of "attrib_verno_val" and "is_changed" array is 
  "num_attribs" X "num_flownn" X "num_channelcc"
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_find_changed_nc_attribs
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint8                          num_attribs,
  uint8                          num_flownn,
  uint8                          num_channelcc,
  uint8              const       *attrib_verno_val,
  uint16             const       *attrib_verno_offset,
  boolean                        *is_changed
)
{
  uint8 index = 0; 
    /* Loop index up to max of num_attribs specified by callign function */
  uint8 flow_nn = 0;
    /* Loop index up to max of num_flownn specified by calling function */
  uint8 channel_cc = 0;
    /* Loop index up to max of num_channelcc specified by calling function */
  uint8 *attrib_verno_ptr;
    /* Pointer to attributes version number value */
  uint16 offset;
    /* Offset of particular flow NC attribute */
  uint16 verno_offset;
    /* Offset into the verno array of a particular NC attribute */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_find_changed_nn_attribs called with invalid protocol name", 
          0, 0, 0 );
    return;
  }

  /* Initialize the is_changed flags to FALSE */
  memset(is_changed, 0, (num_attribs * num_flownn * num_channelcc));

  for ( index = 0; index < num_attribs; index++ )
  {
    for ( flow_nn = 0; flow_nn < num_flownn; flow_nn++ )
    {
      for ( channel_cc = 0; channel_cc < num_channelcc; channel_cc++)
      {
        /* Calculate the offset of the verno of this attribute */
        verno_offset = (flow_nn * num_channelcc) + channel_cc;

        /* Offset of attribute version number value for a given flow in the
           version num array */
        offset = attrib_verno_offset[index] + verno_offset;

        /* Get pointer to version number of SCMDB's copy of the attribute */
        attrib_verno_ptr = hdrscmdb_get_attrib_verno_handle(
                            pip->verno_offset,
                            offset);

        /* Compare protocol's copy of attrib version number with that in SCMDB */
        if ( (*attrib_verno_ptr) != 
              attrib_verno_val[((index * num_flownn * num_channelcc) + 
                                 verno_offset)] )
        {
          is_changed[((index * num_flownn * num_channelcc) + verno_offset)] = TRUE;
        }

      } /* for each channel(CC) of a given attribute */

    } /* for each flow(NN) of a given attribute */

  } /* for each attribute */

} /* hdrscmdb_find_changed_nc_attribs() */
#endif /* FEATURE_HDR_REVB_SUPPORTED */

/*=============================================================================
FUNCTION HDRSCMDB_SET_INCONFIG_TO_MP_CONFIG
  
DESCRIPTION
  This function sets the inconfig instance values of the given protocol to 
  negotiated instance of the protocol in main personality.

DEPENDENCIES
  None.

PARAMETERS
  pip - Pointer to protocol's registration entry in SCMDB reg table.
                           
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
static void hdrscmdb_set_inconfig_to_mp_config(hdrscmdb_reg_type const *pip)
{
  byte  *neg_cfg_ptr;
    /* Pointer to protocol's negotiated configuration instance */
  void  *inconfig_ptr = NULL;
    /* Pointer to protocol's inConfig instance */
/*---------------------------------------------------------------------------*/

  if (pip == NULL)
  {
    ERR_FATAL("Invalid pip passed to - hdrscmdb_set_inconfig_to_mp_config",
               0,0,0);
  }

  /* Get a pointer to legacy protocol's inconfig instance */
  inconfig_ptr = pip->get_inconfig_handle();

  /* Get a pointer to negotiated instance of the main personality */
  neg_cfg_ptr = hdrscmdb_get_negotiated_config_handle( 
                             HDRSCMDB_MAIN_PERSONALITY, 
                             pip->cfg_offset );

  /* Set the inconfig instance of the protocol to negotiated values
     of the main personality */
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy(inconfig_ptr, pip->cfg_size, neg_cfg_ptr, pip->cfg_size);
#else
  memcpy(inconfig_ptr, neg_cfg_ptr, pip->cfg_size);
#endif /* FEATURE_MEMCPY_REMOVAL */

} /* hdrscmdb_set_inconfig_to_mp_config() */

/*=============================================================================
FUNCTION HDRSCMDB_PROTOCOL_SUBTYPE_CHANGED
  
DESCRIPTION
  This function is called by SCP on subtype change for a given protocol during
  SCP subtype negotiation.

  When subtype changes to hardlink (0xFFFE), the inconfig instance of the
  protocol is set to the negotiated instance of the main personality.

  When subtype changes to other than hardlink, the inconfig instance is
  set to the default values of the new subtype.
  Note: Second case call is needed for protocols that store the config
  attributes as a union of config structs of subtypes in order to save
  space. For most protocols whose config struct includes a separate
  instance of config attributes for each subtype, this call is not needed.

DEPENDENCIES
  None.

PARAMETERS
  protocol - Protocol for which the subtype has changed. 
  subtype  - Negotiated subtype for the protocol.
                         
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_protocol_subtype_changed
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint16                         subtype
)
{
  void  *inconfig_ptr = NULL;
    /* Pointer to protocol's inConfig instance */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  if ((protocol_name == HDRHAI_SESSION_CONFIGURATION_PROTOCOL) ||
      (protocol_name == HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL))
  {
    /* By default SCP is partially hardlinked and AMP is completley hardlinked
       always, so no special processing is needed on subtype change to hardlink
       or default. Even when we explicitly hardlink SCP, subtype's attribute in
       each personality will not be hardlinked. 
     */
    return;
  }

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry((hdrhai_protocol_name_enum_type)protocol_name);

  if (pip == NULL)
  { 
    /* Protocol does not have any configuration attributes */
    return;
  }

  HDR_MSG_PROT_2(MSG_LEGACY_LOW, 
                 "SCMDB: Subtype Changed P %d, St %d", protocol_name, subtype);

  /* The protocol subtype changed to hardlink */
  if (subtype == HDRSCMDB_HARDLINK_SUBTYPE)
  {
    hdrscmdb_set_inconfig_to_mp_config(pip);

    if (protocol_name == HDRHAI_STREAM_PROTOCOL)
    {
      /* When stream in hardlinked also update Application layer protocol's
         inconfig instances */
      pip = hdrscmdb_find_reg_entry(HDRHAI_LOCATION_UPDATE_PROTOCOL);
      hdrscmdb_set_inconfig_to_mp_config(pip);

      pip = hdrscmdb_find_reg_entry(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL);
      hdrscmdb_set_inconfig_to_mp_config(pip);

      pip = hdrscmdb_find_reg_entry(HDRHAI_DATA_OVER_SIGNALING_PROTOCOL);
      hdrscmdb_set_inconfig_to_mp_config(pip);

      pip = hdrscmdb_find_reg_entry(HDRHAI_FLOW_CONTROL_PROTOCOL);
      hdrscmdb_set_inconfig_to_mp_config(pip);

      pip = hdrscmdb_find_reg_entry(HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL);
      hdrscmdb_set_inconfig_to_mp_config(pip);
    }
  }
  else
  /* Subtype changed so set the inconfig instance to default values of the new
     protocol subtype */
  {
    /* Get a pointer to inconfig instance of the protocol */
    inconfig_ptr = pip->get_inconfig_handle();

    /* Set inconfig instance to defaults of the corresponding subtype */
    pip->set_config_to_default(inconfig_ptr, subtype);
  }

} /* hdrscmdb_protocol_subtype_changed() */

/*=============================================================================
FUNCTION HDRSCMDB_SESSION_CONFIG_TOKEN_CHANGED

DESCRIPTION
  This function processes the session configuration token changed event.

DEPENDENCIES
  Before calling this function current personality index in DB should be 
  updated by calling hdrscmdb_set_inuse_personality()

PARAMETERS
  None
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_session_config_token_changed
( 
  hdrhai_session_change_reason_enum_type change_reason 
)
{
  hdrind_ind_data_union_type ind_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "SCMDB: Switching personality new SCT=0x%x", 
                 hdrscmdb.scp_info.config_token );
                          
  /* Store the session change reason */
  ind_data.change_reason = change_reason;

  if ((change_reason == HDRHAI_SCHG_OPENED_NORMAL) ||
      (change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR) ||
      (change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS))
  {
    ASSERT(hdrscmdb.session_db_status != HDRSCMDB_SESSION_CURRENT);
    hdrscmdb.session_db_status = HDRSCMDB_SESSION_CURRENT;
  }
  else
  {
    ASSERT(hdrscmdb.session_db_status == HDRSCMDB_SESSION_CURRENT);
  }

  /* The four most significant bits of SessionConfigToken specifies the
     current personality index */
  hdrscmdb_set_inuse_personality((hdrscmdb.scp_info.config_token >> 12));

  /*-------------------------------------------------------------------------
  Set the inuse values to negotiated values of current personality
  -------------------------------------------------------------------------*/
  hdrscmdb_set_inuse_to_negotiated();

  /* Intialize attribute version number's on power up to zero */
  hdrscmdb_reset_attrib_verno();

  if (change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR)
  {
    /* The HDRPAC does not process the session close indication that is given
       by SCMDB before the prior session restore attempt. So it ignores the 
       session change reason HDRHAI_SCHG_OPENED_RESTORE_PRIOR, since nothing 
       has changed from HDRPAC's perspective. However if AN switched the 
       personality during prior session restore HDRPAC needs to do the necessary
       processing for PS, for this reason SCMDB gives a new session 
       change reason. */
    if ((hdrscmdb.scp_info.config_token & 0xF000) != 
        (hdrscmdb.prev_scp_info.config_token & 0xF000))
    {
      ind_data.change_reason = HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS;
    }
  }

  /*--------------------------------------------------------------------------
  Return SCMSessionChanged Indication. Protocols that register to receive this
  indication updates it's local copy of configuration parameters from SCMDB.
  -------------------------------------------------------------------------*/
  HDRTRACE_SCM_SESSION_CHANGED( ind_data.change_reason );
#ifdef FEATURE_EHRPD
  /* Query Data's eHRPD capability again. Save the value and check eHPRD
   * availability after session changed indication. */
  hdrscp_set_data_ehrpd_capability_internal(
    hdrpacdsif_get_data_ehrpd_capability() );
#endif /* FEATURE_EHRPD */
  hdrind_give_ind( HDRIND_SCM_SESSION_CHANGED, (void *) &ind_data );


  /* Log the protocol subtype info log packet to record the changed current
     personality  */
  hdrscmdb_log_pers_subtype_stream_config();

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsap_set_prereg_status( HDRHAI_PREREG_STATUS_EVENT_EVDO_SESSION_OPENED );
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdrscmdb_session_config_token_changed() */

/*=============================================================================
FUNCTION HDRSCMDB_STORE_PERSONALITY

DESCRIPTION
  This functions stores the last negotiated personality in DB.
  
DEPENDENCIES
  None
  
PARAMETERS
  None
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
static void hdrscmdb_store_personality(uint8 pindex_store)
{
  uint8 index;
    /* Loop index */
  hdrscmdb_reg_type const *pip = NULL;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  ASSERT(pindex_store < HDRSCMDB_MAX_PERSONALITIES);

  /*---------------------------------------------------------------------------
  Store SCP's configuration in DB first so that we can know if a given protocol
  is hardlinked in this personality by querying the DB instead of SCP.
  ---------------------------------------------------------------------------*/
  pip = hdrscmdb_find_reg_entry(HDRHAI_SESSION_CONFIGURATION_PROTOCOL);

  if (pip == NULL)
  {
    ERR_FATAL("Unable to find pip for SCP in hdrscmdb_store_personality()",
               0,0,0);
  }

  hdrscmdb_set_negotiated_to_inconfig(pindex_store, pip);

  /*---------------------------------------------------------------------------
  Store rest of the protocols negotiated configuration in DB
  ---------------------------------------------------------------------------*/
  for (index = 0; index < HDRSCMDB_NUM_REG_PROTOCOLS; index++)
  {
    pip = &hdrscmdb_reg[index];

    hdrscmdb_set_negotiated_to_inconfig(pindex_store, pip);
  }

} /* hdrscmdb_store_personality() */

/*=============================================================================
FUNCTION HDRSCMDB_PROCESS_CONFIG_COMPLETE

DESCRIPTION
  This function is called by SCP on receiving a soft config complete or
  config complete message.

DEPENDENCIES
  None.

PARAMETERS
  pindex_store - Personality index store in Soft CC message or Session config
                 token in Config complete message
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_process_config_complete( uint8 pindex_store )
{
  hdrscmdb.personality[pindex_store].personality_is_valid = TRUE;

  /* Copy the protocol configuration data negotiated in this round of
     negotation in index store specified */
  hdrscmdb_store_personality(pindex_store);

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "SCMDB: Personality %d updated",
                 pindex_store);

} /* hdrscmdb_process_config_complete() */
        
/*=============================================================================
FUNCTION HDRSCMDB_PROCESS_PREVIOUS_SESSION_OPENED

DESCRIPTION
  This function is called by SCP when AT wants to restore its previous session.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_process_previous_session_opened( void )
{

  if (hdrscmdb_prior_session_is_valid())
  {
    /*-------------------------------------------------------------------------
    This is a shorthand to negotiate to the "previous" session all at once.
    -------------------------------------------------------------------------*/
    hdrscmdb_set_negotiated_to_previous();

    HDR_MSG_PROT(MSG_LEGACY_HIGH, "SCMDB: Restored prior session");
  }
  else
  {
    /* We should never get here */
    ERR("No Previous Session!", 0, 0, 0 );
  }

} /* hdrscmdb_process_previous_session_opened() */

/*=============================================================================
FUNCTION HDRSCMDB_PROCESS_SESSION_CLOSED

DESCRIPTION
  This function is called by SMP when current session is closed.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_process_session_closed
(
  hdrhai_session_close_reason_enum_type close_reason
)
{
  hdrind_ind_data_union_type ind_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*--------------------------------------------------------------------------
  Make the negotiated configuration parameters of current session as "previous"
  & set flag to indicate that "previous" parameters are valid.
  ---------------------------------------------------------------------------*/
  if ( close_reason == HDRHAI_CLOSE_NEW_NETWORK )
  {
    ind_data.change_reason = HDRHAI_SCHG_CLOSED_RESTORE_PRIOR;
    if (hdrscmdb.session_db_status == HDRSCMDB_SESSION_CURRENT)
    {
      hdrscmdb_set_previous_to_negotiated();
      hdrscmdb.session_db_status = HDRSCMDB_SESSION_PREVIOUS;
    }
    else
    {
      /* If AT received a valid UATI, but has no open session, loses network,
         and reacquire a new network, it will get here. Nothing to do */
    }
  }
  else
  {
    ind_data.change_reason = HDRHAI_SCHG_CLOSED_NORMAL;
    hdrscmdb.session_db_status = HDRSCMDB_SESSION_INVALID;
  }

  /*-------------------------------------------------------------------------
  Set the inuse configuration paramters to default values
  -------------------------------------------------------------------------*/
  hdrscmdb_set_inuse_to_default();

  /*-------------------------------------------------------------------------
  If negotiation was in progress when the session was closed, whatever was 
  negotiated must be cleared out.
  -------------------------------------------------------------------------*/
  /* Set negotiated as invalid */

  /* Clear all of the ConfigResponse timers set by SCM */
  hdrscm_clear_all_rsp_timers();

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsap_set_prereg_status( HDRHAI_PREREG_STATUS_EVENT_EVDO_SESSION_CLOSED );
#endif /* FEATURE_LTE_TO_HDR_OH */

  /*--------------------------------------------------------------------------
  Return SCMSessionChanged Indication. Protocols that register to receive this
  indication updates it's local copy of configuration parameters from SCMDB.
  -------------------------------------------------------------------------*/
  HDRTRACE_SCM_SESSION_CHANGED( ind_data.change_reason );
  hdrind_give_ind( HDRIND_SCM_SESSION_CHANGED, (void *) &ind_data );

} /* hdrscmdb_process_session_closed() */

/*=============================================================================
FUNCTION HDRSCMDB_POWERUP_INIT

DESCRIPTION
  This function does initialization of SCMDB module during powerup.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  TRUE if session is active and read from EFS succeeded, FALSE otherwise.

SIDE EFFECTS
  Modifies the database of configuration information.
=============================================================================*/
boolean hdrscmdb_powerup_init( void )
{
  hdrscmdb.file_write_counter = 0;
  hdrscmdb.session_db_status = HDRSCMDB_SESSION_INVALID;

  if ( hdrmc_is_first_init() )
  {
    if (hdrscp_restore_from_efs(HDRHAI_SESSION_CONFIG_MANAGER) &&
         (hdrscmdb_read() == E_SUCCESS))
    {
      /* Session information in EFS is valid and read from EFS succeeded. */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "SCMDB: Session info restored from EFS");
    }
    else
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                    "SCMDB: Reading from EFS failure, invalidate session");
      hdrscmdb.session_db_status = HDRSCMDB_SESSION_INVALID;

      /* Create a directory under which the config file will be stored */
      if ( efs_mkdir("/hdr", O_CREAT) == -1 )
      {
        ERR("err creating /hdr directory or it already exists in EFS", 0, 0, 0 );
      }
    }
  }

  /* SCMDB info is either restored succesfully or failed,
   * the EFS DB and hdrscmdb are in sync. */
  hdrscmdb_set_efs_up_to_date_flag(TRUE);

  if (hdrscmdb.session_db_status == HDRSCMDB_SESSION_PREVIOUS)
  {
    /* If previous session is stored in the EFS and it is silently restorable
       then treat it as current session */
    hdrscmdb.session_db_status = HDRSCMDB_SESSION_CURRENT;
  }
  else if (hdrscmdb.session_db_status == HDRSCMDB_SESSION_PREV_NO_SILENT_RESTORE)
  {
    /* If previous session in EFS is not silently restorable create a previous 
       session */
    hdrscmdb_set_previous_to_negotiated();
  }

  if (hdrscmdb.session_db_status == HDRSCMDB_SESSION_CURRENT)
  {
    /* On powerup intialize current personality index based on session config
       token read from EFS */
    hdrscmdb_set_inuse_personality((hdrscmdb.scp_info.config_token>>12));
     /* Initialize in_use instance with negotiated personality, whose index is
       specified by curr_personality */
    hdrscmdb_set_inuse_to_negotiated();
  }
  else
  {
    /* As there is no session, set curr_personality to be the main personality */
    hdrscmdb_set_inuse_personality(HDRSCMDB_MAIN_PERSONALITY);

    /* Also initialize the in_use instance to default values */
    hdrscmdb_set_inuse_to_default();
  }

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "SCMDB: DB powerup in_use personality = %d", 
                 hdrscmdb.curr_personality);

  /* Intialize attribute version number's on power up to zero */
  hdrscmdb_reset_attrib_verno();

  /* Initialize the Build compile data and time, stored along with 
     configuration parameters in EFS file to validate the file  */
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( hdrscmdb.build_info.build_date, 
           sizeof(hdrscmdb.build_info.build_date), 
           ver_date, 
           HDRSCMDB_VERNO_DATE_STRLEN );
    
  memscpy( hdrscmdb.build_info.build_time, 
           sizeof(hdrscmdb.build_info.build_time), 
           ver_time, 
           HDRSCMDB_VERNO_TIME_STRLEN );  
#else
  memcpy( hdrscmdb.build_info.build_date, ver_date, 
          HDRSCMDB_VERNO_DATE_STRLEN );

  memcpy( hdrscmdb.build_info.build_time, ver_time, 
          HDRSCMDB_VERNO_TIME_STRLEN );  
#endif /* FEATURE_MEMCPY_REMOVAL */
     
  return (hdrscmdb.session_db_status == HDRSCMDB_SESSION_CURRENT);

} /* hdrscmdb_powerup_init() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_MAIN_PERSONALITY_PS

DESCRIPTION
  This function returns the subtype of protocols in main personality.
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_subtype - Array in which subtype of protocol's in main personality
                     is returned to the calling function.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
hdrerrno_enum_type hdrscmdb_get_main_personality_ps(uint16 *protocol_subtype)
{
  if (hdrscmdb.personality[HDRSCMDB_MAIN_PERSONALITY].personality_is_valid)
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy(protocol_subtype, 
            sizeof(uint16) * HDRHAI_PROTOCOL_TYPE_ENUM_MAX,
            hdrscmdb.personality[HDRSCMDB_MAIN_PERSONALITY].scp.subtype,
            sizeof(hdrscmdb.personality[HDRSCMDB_MAIN_PERSONALITY].scp.subtype));
#else
    memcpy(protocol_subtype,
           hdrscmdb.personality[HDRSCMDB_MAIN_PERSONALITY].scp.subtype,
           sizeof(hdrscmdb.personality[HDRSCMDB_MAIN_PERSONALITY].scp.subtype));
#endif /* FEATURE_MEMCPY_REMOVAL */

    return E_SUCCESS;
  }

  return E_FAILURE;

} /* hdrscmdb_get_main_personality_ps() */

/*=============================================================================
FUNCTION HDRSCMDB_DELETE_PERSONALITIES

DESCRIPTION
  This function sets the specified personalities as deleted.
    
DEPENDENCIES
  None.

PARAMETERS
  personalities - Array of personality indexes to be deleted.
  personality_count - Number of personalities to delete.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_delete_personalities
( 
  uint8 personalities[], 
  uint8 personality_count 
)
{
  uint8 cnt = 0;
    /* Loop Counter */
/*---------------------------------------------------------------------------*/
  for (cnt = 0; cnt < personality_count; cnt++ )
  {
    hdrscmdb.personality[ personalities[cnt] ].personality_is_valid = FALSE;
  }

} /* hdrscmdb_delete_personalities() */

/*=============================================================================
FUNCTION HDRSCMDB_PERSONALITY_IS_VALID

DESCRIPTION
  This function determines if the personality at given index is valid or not.
    
DEPENDENCIES
  None.

PARAMETERS
  personality_index - Index of the personality that need to checked.
  check_curr_session- If this is set to TRUE by the calling routine personality
          validity check is done for current session otherwise check is done in
          previous session.
  
RETURN VALUE
  TRUE if the personality is valid, FALSE otherwise.

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrscmdb_personality_is_valid
( 
  boolean check_curr_session,
  uint8   personality_index
)
{
  boolean personality_is_valid = FALSE;
/*---------------------------------------------------------------------------*/

  if (personality_index >= HDRSCMDB_MAX_PERSONALITIES)
  {
    return personality_is_valid;
  }

  if (check_curr_session)
  {
    if(hdrscmdb.personality[personality_index].personality_is_valid)
    {
      personality_is_valid = TRUE;
    }
  }
  else  /* Previous session */
  {
    if(hdrscmdb.previous[personality_index].personality_is_valid)
    {
      personality_is_valid = TRUE;
    }
  }

  return personality_is_valid;

} /* hdrscmdb_personality_is_valid() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_NEGOTIATED_CONFIG

DESCRIPTION
  This function returns a pointer to the given protocol's negotiated
  configuration instance in a given personality.
    
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - The protocol whose configuration is requested.
  pindex        - Personality index from which protocol's configuration
                  data is to be retrieved.
  
RETURN VALUE
  Returns a pointer to protocol's negotiated configuration in database.

SIDE EFFECTS
  None
=============================================================================*/
void const* hdrscmdb_get_negotiated_config
(
  hdrhai_protocol_name_enum_type  protocol_name,
  uint16                          pindex
)
{
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
  void const *cfg_data_ptr;
    /* Pointer to protocol's negotiated configuration structure in database */ 
/*---------------------------------------------------------------------------*/

  /* Get a pointer to protocol's entry in reg table */
  pip = hdrscmdb_find_reg_entry(protocol_name);

  if (pip == NULL)
  { 
    ERR(" hdrscmdb_get_negotiated_config called with invalid protocol name", 
          0, 0, 0 );
    cfg_data_ptr = NULL;
    return cfg_data_ptr;
  }

  if (hdrscmdb.personality[pindex].personality_is_valid)
  {
    /* Get pointer to protocol's negotiated configuration structure */
    cfg_data_ptr = hdrscmdb_get_negotiated_config_handle(pindex,
                                                         pip->cfg_offset);
  }
  else
  {
    cfg_data_ptr = NULL;
  }

  return cfg_data_ptr;

} /* hdrscmdb_get_negotiated_config() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_NEGOTIATED_APP_SUBTYPES

DESCRIPTION
  This function returns the list of application subtypes that have been 
  negotiated.
                             
DEPENDENCIES
  None.

PARAMETERS
  app_subtypes      - List of application subtypes that have been negotiated.  
                      This function fills up this array and passes it back to
                      the caller.
  num_app_subtypes  - Number of application subtypes that are included in
                      "app_subtypes" array
    
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_get_negotiated_app_subtypes
(
  uint16 *app_subtypes, 
  uint8  *num_app_subtypes
)
{
  uint8 pindex;
    /* Personality index */
  hdrstream_stream_config_attrib_type const *sc_ptr = NULL; 
    /* Pointer to stream configuration in main personality */
  uint8 stream_id;
    /* Stream identifier */
  uint8 app_count = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (pindex = 0; pindex < HDRSCMDB_MAX_PERSONALITIES; pindex++)
  {
    sc_ptr = hdrscmdb_get_negotiated_config(HDRHAI_STREAM_PROTOCOL, pindex);

    if (sc_ptr == NULL)
    {
      continue;
    }

    for (stream_id = 1; stream_id < HDRSTREAM_NUM_STREAMS; stream_id++)
    {
      if (sc_ptr->stream_application[stream_id] != HDRHAI_APP_STREAM_NOT_USED)
      {
        app_subtypes[app_count++] = sc_ptr->stream_application[stream_id];
      }
    }
  }

  *num_app_subtypes = app_count;

} /* hdrscmdb_get_negotiated_app_subtypes() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_CURRENT_PCCC_TO_DEFAULT

DESCRIPTION
  This function is called by IDLE to update the SCMDB's inuse copy of PCCC
  value. 

DEPENDENCIES
  None.
    
PARAMETERS
  curr_pccc_value - Current PCCC value.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_set_current_pccc_to_default( uint16 curr_pccc_value )
{
  hdrscmdb.inuse.idle.pccc.value = curr_pccc_value;
} /* hdrscmdb_set_current_pccc_to_default() */

/*=============================================================================
FUNCTION HDRSCMDB_UPDATE_CONFIG_LOCK

DESCRIPTION
  This function updates the ConfigLock value in SCMDB.
  
DEPENDENCIES
  None.
    
PARAMETERS
  config_lock_val - Value of config lock
     
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_update_config_lock(boolean config_lock_val)
{
  hdrscmdb.scp_info.config_is_locked = config_lock_val;

  if (hdrscmdb.scp_info.config_is_locked)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Config locked");
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Config unlocked");
  }

} /* hdrscmdb_update_config_lock() */

/*=============================================================================
FUNCTION HDRSCMDB_LOG_PERS_SUBTYPE_STREAM_CONFIG

DESCRIPTION
  This function is used to log the protocol subtype information and stream 
  configuration for each of the negotiated personalities and the personality 
  index of the current personality.

DEPENDENCIES
  None.
    
PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_log_pers_subtype_stream_config( void )
{
  uint8 pers_ind = 0;
    /* Personality index */

  uint8 valid_pers_ind = 0;
    /* Valid personality index */

  LOG_HDR_SUBTYPE_STREAM_CONFIG_C_type *log_ptr = NULL;
    /* pointer to subtype & stream config log packet */

  hdrscmdb_protocol_config_data_type *config = NULL;
    /* Per-personality configuration; convenience pointer */

  uint8 personality_count = 0;
    /* Number of valid personalities. */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Find the number of valid personalities. */
  for (pers_ind=0; pers_ind < HDRSCMDB_MAX_PERSONALITIES; ++pers_ind)
  {
    if (hdrscmdb.personality[pers_ind].personality_is_valid == TRUE)
    {
      ++personality_count;
    }
  }

  /* Allocate memory for log packet */
  log_ptr = (LOG_HDR_SUBTYPE_STREAM_CONFIG_C_type*)
              log_alloc( LOG_HDR_SUBTYPE_STREAM_CONFIG_C,
                         ( sizeof(LOG_HDR_SUBTYPE_STREAM_CONFIG_C_type) +
                           (personality_count-1) * 
                           sizeof(hdrlog_st_stream_cfg_type) ) );

  /* If memory allocation hasn't failed or if log code not enabled.
     Note: Failed log_alloc doesn't always mean failed memory allocation.
     log_alloc could fail if the log code is not enabled too.
     log_alloc calls log_status internally */  
  if ( log_ptr != NULL )
  {
    /* Store the version number of the log packet */
    log_ptr->version = HDRSCMDB_PROT_SUB_N_STREAM_LOG_PKT_REV;

    /* Log the current personality index */
    log_ptr->current_personality = hdrscmdb.curr_personality;

    /* Log the personality count */
    log_ptr->personality_count = personality_count;

    for (pers_ind=0; pers_ind < HDRSCMDB_MAX_PERSONALITIES; ++pers_ind)
    {
      /* Initialize the convenience pointer */
      config = &hdrscmdb.personality[pers_ind];

      if (config->personality_is_valid == TRUE)
      {
        log_ptr->pers[valid_pers_ind].personality_index = pers_ind;

        /* Copy the protocol subtype information of personality 'pers_ind'
           into the log packet */
#ifdef FEATURE_MEMCPY_REMOVAL
        memscpy ( (void*) &(log_ptr->pers[valid_pers_ind].prot_sub[0]), 
                  (sizeof(uint16) * HDRLOG_MAX_PROTOCOL_COUNT),
                  (void*) &(config->scp.subtype[0]),
                  (HDRHAI_PROTOCOL_TYPE_ENUM_MAX * sizeof(uint16)) );
#else
        memcpy ( (void*) &(log_ptr->pers[valid_pers_ind].prot_sub[0]), 
                 (void*) &(config->scp.subtype[0]),
                 (HDRHAI_PROTOCOL_TYPE_ENUM_MAX * sizeof(uint16)) );
#endif /* FEATURE_MEMCPY_REMOVAL */

        log_ptr->pers[valid_pers_ind].prot_sub[HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL] =
            HDRSCMDB_HARDLINK_SUBTYPE; 
        log_ptr->pers[valid_pers_ind].prot_sub[HDRHAI_SESSION_CONFIGURATION_PROTOCOL] =
            HDRSCMDB_HARDLINK_SUBTYPE;

        /* Copy the Stream configuration of personality 'pers_ind' into the 
           log packet */
#ifdef FEATURE_MEMCPY_REMOVAL
        memscpy( (void*) &(log_ptr->pers[valid_pers_ind].app_type[0]),
                 (sizeof(uint16) * HDRLOG_MAX_STREAM_COUNT),
                 (void*) &(config->stream.stream_config.stream_application[0]),
                 (HDRSTREAM_NUM_STREAMS * sizeof(uint16))); 
#else
        memcpy( (void*) &(log_ptr->pers[valid_pers_ind].app_type[0]),
                (void*) &(config->stream.stream_config.stream_application[0]),
                (HDRSTREAM_NUM_STREAMS * sizeof(uint16)));
#endif /* FEATURE_MEMCPY_REMOVAL */

        /* One more valid personality has been logged */
        ++valid_pers_ind;

      }
    }

    /* Ship out the log packet */
    log_commit((log_type*)log_ptr);

    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Logged subtype-stream log pkt");
  }

} /* hdrscmdb_log_pers_subtype_stream_config */

/*=============================================================================
FUNCTION HDRSCMDB_NONDEFAULT_SECURITY_PS_IS_ALLOWED

DESCRIPTION
  This function checks if the AT is allowed to propose/accept non-default
  protocol subtype for a given security layer protocol.
  
DEPENDENCIES
  None.
    
PARAMETERS
  protocol_name - Name of the protocol
       
RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrscmdb_nondefault_security_ps_is_allowed
( 
  hdrhai_protocol_name_enum_type protocol_name 
)
{
  uint8 pindex;
    /* Personality index */
/*---------------------------------------------------------------------------*/
  if ((protocol_name >= HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX))
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
      "Invalid protocol- %d passed to nondefault_security_ps_is_allowed",
       protocol_name); 
    return FALSE;
  }

  for (pindex=0; pindex < HDRSCMDB_MAX_PERSONALITIES; pindex++)
  {
    if (hdrscmdb.personality[pindex].personality_is_valid)
    {
      /* Currently we allow only one personality to have non-default protocol 
         subtype for security layer protocols */
      if ((hdrscmdb.personality[pindex].scp.subtype[protocol_name] != 
           HDRSCP_DEFAULT_PROTOCOL) &&
          (hdrscmdb.personality[pindex].scp.subtype[protocol_name] != 
           HDRSCP_HARDLINK))
      {
        return FALSE;
      }
    }
  }

  return TRUE;

} /* hdrscmdb_nondefault_security_ps_is_allowed() */

/*=============================================================================
FUNCTION HDRSCMDB_GET_SNPKTAPP_SUBTYPE

DESCRIPTION
  This function returns the name of packet application bound to SN if any in the 
  given stream configuration.

DEPENDENCIES
  None.
    
PARAMETERS
  None
       
RETURN VALUE
  Name of packet application bound to SN, otherwise HDRHAI_APP_STREAM_NOT_USED.

SIDE EFFECTS
  None
=============================================================================*/
static uint16 hdrscmdb_get_snpktapp_subtype
(
  hdrstream_config_params_type stream_cfg
)
{
  uint8 i;
    /* Loop index */
  uint16 app_type = HDRHAI_APP_STREAM_NOT_USED;
    /* App subtype bound to SN negotiated in the given stream config */
/*---------------------------------------------------------------------------*/

  for (i = 1; i < HDRSTREAM_NUM_STREAMS; i++)
  {
    app_type = stream_cfg.stream_config.stream_application[i];

    if ((app_type == HDRHAI_DEF_PACKET_APP_SN) ||
        (app_type == HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
        (app_type == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
        (app_type == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
    {
      /* Found the packet application bound to SN */
      break;
    }
  }

  return app_type;
} /* hdrscmdb_get_snpktapp_subtype() */

/*=============================================================================
FUNCTION HDRSCMDB_NEGOTIATED_SESSION_IS_VALID

DESCRIPTION
  This function checks if the negotiated session is valid or not.

  Negotiated session is invalid if there are 
  1) More than one instance of MPA/EMPA.
  2) More than one instance of DPA and they are not hardlinked.
  3) More than one instance of non-default security protocol and they are not 
     hardlinked.

DEPENDENCIES
  None.
    
PARAMETERS
  None
       
RETURN VALUE
  TRUE if the session is valid, FALSE otherwise.

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrscmdb_negotiated_session_is_valid( void )
{
  boolean negotiated_session_is_valid = TRUE;
    /* Whether the negotiated session is valid or not */
  uint8 i, j;
    /* Loop indices */
  hdrscmdb_protocol_config_data_type *p1, *p2;
    /* Pointers to config's of two personalities that are compared */
  uint16 app_type1, app_type2;
    /* AppSubtypes from two different personalities that are compared */
/*---------------------------------------------------------------------------*/
  for (i = 0; ((i < (HDRSCMDB_MAX_PERSONALITIES - 1)) && negotiated_session_is_valid);
       i++)
  {
    p1 = &hdrscmdb.personality[i];

    if (!p1->personality_is_valid)
    {
      continue;
    }

    for (j = i + 1; j < HDRSCMDB_MAX_PERSONALITIES; j++)
    {
      p2 = &hdrscmdb.personality[j];

      if (!p2->personality_is_valid)
      {
        continue;
      }

      /* Get the name of packet application bound to SN in p1 and p2 */
      app_type1 = hdrscmdb_get_snpktapp_subtype(p1->stream);
      app_type2 = hdrscmdb_get_snpktapp_subtype(p2->stream);

      /* Check if the stream config is valid between personalities P1 and P2 */
      if ((app_type1 == app_type2) &&
          (p2->scp.subtype[HDRHAI_STREAM_PROTOCOL] != 
           HDRSCMDB_HARDLINK_SUBTYPE))
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR,
          "Err: Two instances of same SNPktApp: %d, personalities %d, %d",
          app_type2, i, j);

        negotiated_session_is_valid = FALSE;
        break;
      }

      /* Check if the security layer config is valid between personalities
         P1 and P2 */
      if (((p1->scp.subtype[HDRHAI_KEY_EXCHANGE_PROTOCOL] == 
            p2->scp.subtype[HDRHAI_KEY_EXCHANGE_PROTOCOL]) &&
           (p2->scp.subtype[HDRHAI_KEY_EXCHANGE_PROTOCOL] == 1))
            ||
          ((p1->scp.subtype[HDRHAI_AUTHENTICATION_PROTOCOL] == 
            p2->scp.subtype[HDRHAI_AUTHENTICATION_PROTOCOL]) &&
           (p2->scp.subtype[HDRHAI_AUTHENTICATION_PROTOCOL] == 1))
            ||
          ((p1->scp.subtype[HDRHAI_SECURITY_PROTOCOL] == 
            p2->scp.subtype[HDRHAI_SECURITY_PROTOCOL]) &&
           (p2->scp.subtype[HDRHAI_SECURITY_PROTOCOL] == 1)))
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
          "Err: Two instances of non-default SEC protocols, personalities %d, %d",
           i, j);

        negotiated_session_is_valid = FALSE;
        break;
      }
    }
  }

  return negotiated_session_is_valid;
} /* hdrscmdb_negotiated_session_is_valid() */

/*=============================================================================
FUNCTION HDRSCMDB_PRIOR_SESSION_IS_VALID

DESCRIPTION
  This function checks if previous session is valid

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Return if previous session is valid.

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrscmdb_prior_session_is_valid( void )
{
  return (((hdrscmdb.session_db_status == HDRSCMDB_SESSION_PREVIOUS) ||
           (hdrscmdb.session_db_status == HDRSCMDB_SESSION_PREV_NO_SILENT_RESTORE)) &&
          hdramp_is_previous_uati_valid());
} /* hdrscmdb_prior_session_is_valid */

/*=============================================================================
FUNCTION HDRSCMDB_SET_PRIOR_SESSION_VALIDITY

DESCRIPTION
  This function sets the validity of previous session. Simplying setting
  previous session to valid does not gurantee hdrscmdb_previous_session_is_valid()
  will return true. KA time must not have passed.

DEPENDENCIES
  None.
    
PARAMETERS
  prev_session_valid - whether current session is valid
    
RETURN VALUE
  None

SIDE EFFECTS
  Setting Previous session to Valid may invalidate current session.
=============================================================================*/
void hdrscmdb_set_prior_session_validity
(
  boolean prev_session_valid
)
{
  if (prev_session_valid)
  {
    hdrscmdb.session_db_status = HDRSCMDB_SESSION_PREVIOUS;
  }
  else if (hdrscmdb.session_db_status == HDRSCMDB_SESSION_PREVIOUS)
  {
    hdrscmdb.session_db_status = HDRSCMDB_SESSION_INVALID;
  }
} /* hdrscmdb_set_prior_session_validity */

/*=============================================================================
FUNCTION HDRSCMDB_SET_CURRENT_SESSION_VALIDITY

DESCRIPTION
  This function sets the validity of current session

DEPENDENCIES
  None.

PARAMETERS
  curr_session_valid - whether current session is valid
    
RETURN VALUE
  None

SIDE EFFECTS
  Setting Current session to valid may invalidate previous session.
=============================================================================*/
void hdrscmdb_set_current_session_validity
(
  boolean curr_session_valid
)
{
  if (curr_session_valid)
      {
    hdrscmdb.session_db_status = HDRSCMDB_SESSION_CURRENT;
    }
  else if (hdrscmdb.session_db_status == HDRSCMDB_SESSION_CURRENT)
  {
    hdrscmdb.session_db_status = HDRSCMDB_SESSION_INVALID;
  }
} /* hdrscmdb_set_current_session_validity */

/*=============================================================================
FUNCTION HDRSCMDB_IS_SILENT_RESTORE_ENABLED

DESCRIPTION
  This function checks if previous session can be silently restored.

DEPENDENCIES
  None.
    
PARAMETERS
  None.
    
RETURN VALUE
  True - prior session can be silently restored
  False - Otherwise

SIDE EFFECTS
  None
=============================================================================*/
boolean hdrscmdb_is_silent_restore_enabled( void )
{  
  return (hdrscmdb.session_db_status == HDRSCMDB_SESSION_PREVIOUS) &&
      hdramp_is_previous_uati_valid();
} /* hdrscmdb_is_silent_restore_enabled */

/*=============================================================================
FUNCTION HDRSCMDB_DISABLE_SILENT_RESTORE

DESCRIPTION
  This function disables silent restore of prior session.

DEPENDENCIES
  None.
    
PARAMETERS
  None.
    
RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void hdrscmdb_disable_silent_restore( void )
{
  if (hdrscmdb.session_db_status == HDRSCMDB_SESSION_PREVIOUS)
  {
    hdrscmdb.session_db_status = HDRSCMDB_SESSION_PREV_NO_SILENT_RESTORE;
  }
} /* hdrscmdb_disable_silent_restore */
/*===========================================================================

FUNCTION HDRSCMDB_SET_IDLE_SLOTTED_MODE

DESCRIPTION
  This function updated the scmdb with given slottedmode value.

DEPENDENCIES
  None

PARAMETERS
   saved_slotted_mode_config - saved slottedmode attribute value 

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmdb_set_idle_scmidle_info
(
  hdridlec_saved_scmidle_info_type *saved_scmidle_info_ptr
)
{
  uint8 curr_personality = hdrscmdb_get_inuse_personality();
  uint8 index; /* Loop index */
/*---------------------------------------------------------------------------*/
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "SCMDB: Set Idlec scmidle info, Valid: %d",
                  saved_scmidle_info_ptr->saved_slotted_mode_is_valid);

  ASSERT (curr_personality < HDRSCMDB_MAX_PERSONALITIES);

  /* If the protocol subtype is HARDLINK in current personality update the 
   * Main personality and also all other personalities in which the protocol 
   * is hardlinked 
   */
  if (hdrscmdb_protocol_subtype_is_hardlink(curr_personality, HDRHAI_IDLE_STATE_PROTOCOL) ||
      (curr_personality == HDRSCMDB_MAIN_PERSONALITY))
  {

    hdrscmdb.personality[HDRSCMDB_MAIN_PERSONALITY].idle.saved_scmidle_info = *saved_scmidle_info_ptr;

    for ( index = 1; index < HDRSCMDB_MAX_PERSONALITIES; index++ )
    {
      if (hdrscmdb_protocol_subtype_is_hardlink(index, HDRHAI_IDLE_STATE_PROTOCOL))
      {
        /* update the negotiated instance of the personality */
        hdrscmdb.personality[index].idle.saved_scmidle_info = *saved_scmidle_info_ptr;
      }
    }
  }
  /* Otherwise update only the current personality */
  else
  {
    hdrscmdb.personality[curr_personality].idle.saved_scmidle_info = *saved_scmidle_info_ptr;
  }

  /* Update InUse instance*/
  hdrscmdb.inuse.idle.saved_scmidle_info = *saved_scmidle_info_ptr;

} /* hdrscmdb_set_idle_slotted_mode*/

/*=============================================================================
FUNCTION HDRSCMDB_SET_INCONFIG_TO_DEFAULT

DESCRIPTION
  This function sets the inconfig instance of a given protocol to default 
  values.

DEPENDENCIES
  None
 
PARAMETERS
  None
    
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
static void hdrscmdb_set_inconfig_to_default
(
  hdrhai_protocol_name_enum_type protocol_name
)
{
  void  *inconfig_ptr = NULL;
    /* Pointer to protocol's inConfig instance */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
  uint16 subtype = 0;
/*---------------------------------------------------------------------------*/

  pip = hdrscmdb_find_reg_entry(protocol_name);
  if (pip != NULL)
  {
    inconfig_ptr = pip->get_inconfig_handle();
    pip->set_config_to_default(inconfig_ptr, subtype);
  }
  else
  {
    ERR_FATAL("pip not found - hdrscmdb_set_inconfig_to_default",0,0,0);
  }

} /* hdrscmdb_set_inconfig_to_default */

/*=============================================================================
FUNCTION HDRSCMDB_SET_PKTAPP_PROTOCOLS_TO_DEFAULT

DESCRIPTION
  This function sets the inconfig instance of packet application 
  protocols (LUP, DOS, FCP, MRLP) to default values.

DEPENDENCIES
  None
 
PARAMETERS
  None
    
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void hdrscmdb_set_pktapp_protocols_to_default( void )
{
  hdrscmdb_set_inconfig_to_default(HDRHAI_LOCATION_UPDATE_PROTOCOL);

  hdrscmdb_set_inconfig_to_default(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL);

  hdrscmdb_set_inconfig_to_default(HDRHAI_DATA_OVER_SIGNALING_PROTOCOL);

  hdrscmdb_set_inconfig_to_default(HDRHAI_FLOW_CONTROL_PROTOCOL);

} /* hdrscmdb_set_pktapp_protocols_to_default() */

/*=============================================================================
FUNCTION HDRSCMDB_SET_CSNA_PROTOCOL_TO_DEFAULT

DESCRIPTION
  This function sets the inconfig instance of CSNA protocol to default values.

DEPENDENCIES
  None
 
PARAMETERS
  None
    
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void hdrscmdb_set_csna_protocol_to_default( void )
{
  hdrscmdb_set_inconfig_to_default(HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL);
} /* hdrscmdb_set_csna_protocol_to_default() */


/*=============================================================================
FUNCTION HDRSCMDB_SET_AMP_INCONFIG_TO_INUSE

DESCRIPTION
  This function sets the value of the AMP "in config" instances to the inuse 
  values e.g. at the beginning of negotiation.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_set_amp_inconfig_to_inuse( void )
{
  void *inconfig_ptr;
    /* Pointer to protocol's inconfig instance */
  byte *curr_cfg_ptr;
    /* Pointer to protocol's inuse instance */
  hdrscmdb_reg_type const *pip;
    /* Pointer to protocol's registration entry in SCMDB reg table */
/*---------------------------------------------------------------------------*/

  pip = hdrscmdb_find_reg_entry(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL);

  ASSERT (pip != NULL);

  /* Get a pointer to legacy protocol's inconfig instance */
  inconfig_ptr = pip->get_inconfig_handle();

  /* Get a pointer to inuse instance of the protocol in DB */
  curr_cfg_ptr = hdrscmdb_get_inuse_config_handle(pip->cfg_offset); 

  /* Copy the inuse instance to inconfig */
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy(inconfig_ptr, pip->cfg_size, curr_cfg_ptr, pip->cfg_size);
#else
  memcpy(inconfig_ptr, curr_cfg_ptr, pip->cfg_size);
#endif /* FEATURE_MEMCPY_REMOVAL */

} /* hdrscmdb_set_amp_inconfig_to_inuse() */

/*=============================================================================
FUNCTION HDRSCMDB_RESET_DTX_MODE

DESCRIPTION
  This function is called to reset the physical layer attribute dtx_mode. When
  AT goes back to idle from connected state or connection set up state dtx mode
  should be reset implicitly (that is without any AUR message from AN).
  
DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_reset_dtx_mode( void )
{
  hdrscmdb_set_gauped_attrib(HDRHAI_PHYSICAL_LAYER_PROTOCOL,
                             HDRSCMPHY_DTX_MODE,
                             0,
                             NULL,
                             HDRSCP_SUB3_PHYS_LAYER);

} /* hdrscmdb_reset_dtx_mode() */

#ifdef FEATURE_EHRPD
/*=============================================================================
FUNCTION HDRSCMDB_AT_SESSION_HAS_EHRPD_PERSONALITY

DESCRIPTION
  This function checks whether AT has a valid eHRPD personality negotiated in 
  the current session.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  TRUE if AT has a valid eHRPD personality. FALSE othewise.

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrscmdb_at_session_has_ehrpd_personality( void )
{

  uint8 pindex;
    /* Personality index */
  uint16 app_type;
    /* AppSubtype for a given personality */
/*---------------------------------------------------------------------------*/

  for (pindex=0; pindex < HDRSCMDB_MAX_PERSONALITIES; pindex++)
  {
    if (hdrscmdb.personality[pindex].personality_is_valid)
    {
      app_type = hdrscmdb_get_snpktapp_subtype( 
          hdrscmdb.personality[pindex].stream );

      if ((app_type == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) &&
          (hdrscmdb.personality[pindex].mrlp.fwd_flow_prot[0].id == 0x07))
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, "AT's session has a valid eHRPD personality");
        return TRUE;
      }
    }
  }

  HDR_MSG_PROT (MSG_LEGACY_HIGH, "AT's session does not have a valid eHRPD personality");

  return FALSE;

} /* hdrscmdb_at_session_has_ehrpd_personality() */

#endif /* FEATURE_EHRPD */

/*=============================================================================
FUNCTION HDRSCMDB_DELETE

DESCRIPTION
  This function deletes the database information stored in EFS.

DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the database of configuration information
=============================================================================*/
void hdrscmdb_delete( void )
{
  /* Delete the file, as soon as AT enter online. As AT enters online, the 
     SCMDB and EFS db can go out of sync at any point of time. */
#ifdef FEATURE_HDR_EFS_COMPRESS
  (void) efs_unlink("/hdr/hdr_protocol.z");
#else /*#else FEATURE_HDR_EFS_COMPRESS*/
  (void) efs_unlink("/hdr/hdr_protocol.cfq");
#endif /*end #ifdef FEATURE_HDR_EFS_COMPRESS*/
		   
  /* as the file is now removed, the EFS db is no longer in sync with SCMDB */
  hdrscmdb_set_efs_up_to_date_flag(FALSE);  

}

/*=============================================================================
FUNCTION HDRSCMDB_SET_EFS_UP_TO_DATE_FLAG

DESCRIPTION
  This function sets the variable, efs_is_up_to_date to TRUE or FALSE.
  If TRUE, scmdb and the db information stored on EFS is in sync, else it 
  is not in sync.

DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  NONE.

SIDE EFFECTS
  NONE
=============================================================================*/
static void hdrscmdb_set_efs_up_to_date_flag( boolean value )
{ 
  hdrscmdb.efs_is_up_to_date = value;
}

/*=============================================================================
FUNCTION HDRSCMDB_EFS_IS_UP_TO_DATE

DESCRIPTION
  This function gets the variable, efs_is_up_to_date current value.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  TRUE - scmdb and the db information stored on EFS is in sync.
  FALSE - not in sync

SIDE EFFECTS
  NONE
=============================================================================*/
boolean hdrscmdb_efs_is_up_to_date( void )
{ 
  return hdrscmdb.efs_is_up_to_date;
}
