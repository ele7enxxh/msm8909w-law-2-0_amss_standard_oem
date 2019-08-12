#ifndef HDRRUPC_H
#define HDRRUPC_H

/*===========================================================================

                 R O U T E   U P D A T E   P R O T O C O L
             C O N F I G U R A T I O N  D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for hdrrups.c.

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrrupc.h_v   1.3   17 Oct 2002 15:16:20   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrrupc.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/21/14   cnx     Format LTE 1x capability req with the band provided by LTE
04/23/14   vko     Support for Single carrier RevB
04/17/12   cnx     Support rfm_is_band_chan_supported_v2().
04/15/11   cnx     Removed hdrrupc_band_is_supported() if RF_HAS_RFM_BAND_CHAN_V2_API defined. 
03/25/10   pba     Populate the 1X-HRPD capabilities container with CDMA band 
                   information.
06/23/09   sju     Changed maximum bandwidth support in revB
11/05/08   wsh     Fixed hashing when SP includes BC15
12/14/07   sju     Added hdrrupc_get_fwd_fb_mux_supp_attrib()
07/31/07   wsh     Fixed RuR report timer such that search will not ignore an
                   RuR while processing the previous one.
07/19/07   sju     Added hdrrupc_config_is_valid()
07/13/07   sju     Added support for revB Multicarrier RUP
06/18/07   hal     Added hdrutil_chan_is_in_band_with_all_subclasses()
                   Increased SupportedCDMAChannels Max Subclasses count to 11
01/25/07   hal     Increased SupportedCDMAChannels Max Bands count to 10
11/20/06   hal     Updated feature for SupportedCDMAChannels to silently close
                   session if band preferences are changed
05/31/06   hal     Modified hdrrupc_band_is_supported() to calculate the
                   band subclass support in a more robust manner
01/04/06   hal     Added #defines for several default attrib values
12/13/05   pba     Changes to support multiple personality negotiation
12/06/05   hal     Added support for AT-init cfg of SupportedCDMAChannels attrib
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/12/05   hal     Added accessors for simple attribs, headers for 
                   hdrrupc_band_is_supported() and hdrrupc_config_is_needed()
                   Added support for AN-init GAUP
10/09/02   mpa     Added Attribute Override Message IDs and
                   restore_current_srch_params
09/11/01   vas     Changed RUP Config Request/Response Message Id macro name
06/04/00   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"

#include "hdrsrchtypes.h"
#include "hdrind.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#include "sd_v.h" /* Needed for SupportedCDMAChannels attribute */
#else
#include "sdss.h" /* Needed for SupportedCDMAChannels attribute */
#endif

/*===========================================================================

                    DATA DECLARATIONS

===========================================================================*/
#define HDRRUPC_CON_REQ_MSG_ID 0x50  /* Config Request Message ID */
#define HDRRUPC_CON_RSP_MSG_ID 0x51  /* Config Response Message ID */

#define HDRRUPC_AO_MSG_ID   0x05 /* Attribute Override Message ID */
#define HDRRUPC_AOR_MSG_ID  0x06 /* Attribute Override Response Message ID */

#define HDRRUPC_ATTRIB_UPDATE_REQ_MSG_ID    0x52 /* AttributeUpdateRequest */
#define HDRRUPC_ATTRIB_UPDATE_ACCEPT_MSG_ID 0x53 /* AttributeUpdateAccept */

#define HDRRUPC_MAX_SUPPORTED_BANDS 10
#define HDRRUPC_MAX_SUPPORTED_SUB_BANDS 11
#define HDRRUPC_MAX_BANDS SD_BAND_BC16 /* Weakly linked to BC 16 as the max */

#define HDRRUPC_DEFAULT_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_VAL 0x0a
#define HDRRUPC_DEFAULT_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_VAL  0x0000
#define HDRRUPC_DEFAULT_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_VAL 0x00

/* Configuration Request/ Response Message Attribute Identifiers */
#define HDRRUPC_SRCH_PARAMS_ATTRIB_ID                0x00
#define HDRRUPC_SET_MNGMT_SAME_CHAN_PARAMS_ATTRIB_ID 0x01
#define HDRRUPC_SET_MNGMT_DIFF_CHAN_PARAMS_ATTRIB_ID 0x02
#define HDRRUPC_SET_MNGMT_OVERRIDE_ALLOWED_ATTRIB_ID 0x03
#define HDRRUPC_ROUTE_UPDATE_RADIUS_MULT_ATTRIB_ID   0xff
#define HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_ID    0xfe
#define HDRRUPC_SUPPORT_ROUTE_UPDATE_ENH_ATTRIB_ID   0xfd

/* Following Attributes will be proposed by the AT using HDRSCMRUP */
#define HDRRUPC_SUPPORTED_CDMA_CHANNELS_ATTRIB_ID    0x04
#define HDRRUPC_MAX_NUMBER_FL_SUPPORTED_ATTRIB_ID    0xfc
#define HDRRUPC_MAX_NUMBER_RL_SUPPORTED_ATTRIB_ID    0xfb
#define HDRRUPC_MAX_FL_BANDWIDTH_NO_JAMMER_ATTRIB_ID 0xfa
#define HDRRUPC_MAX_FL_BANDWIDTH_W_JAMMER_ATTRIB_ID  0xf9
#define HDRRUPC_MAX_RL_BANDWIDTH_ATTRIB_ID           0xf8
#define HDRRUPC_MAX_NUMBER_SUB_ASETS_ATTRIB_ID       0xf7
#define HDRRUPC_FWD_FEEDBACK_MUX_SUPPORTED_ATTRIB_ID 0xf6

/* Default value for AT initiated configuration attributes*/
#define HDRRUPC_DEFAULT_BAND_CLASS_COUNT               0x00
#define HDRRUPC_DEFAULT_MAX_NUM_FL_SUPPORTED           0x01
#define HDRRUPC_DEFAULT_MAX_NUM_RL_SUPPORTED           0x01
#define HDRRUPC_DEFAULT_MAX_FL_BANDWIDTH_NO_JAMMER     0x0000
#define HDRRUPC_DEFAULT_MAX_FL_BANDWIDTH_W_JAMMER      0x0000
#define HDRRUPC_DEFAULT_MAX_RL_BANDWIDTH               0x0000
#define HDRRUPC_DEFAULT_MAX_NUM_SUB_ASETS              0x01
#define HDRRUPC_DEFAULT_FWD_FEEDBACK_MUX_SUPPORTED     0x00

/* Desired value for AT initiated configuration attributes*/
#ifdef FEATURE_HDR_SC_REVB /* Single Carrier RevB */
#define HDRRUPC_DESIRED_MAX_NUM_FL_SUPPORTED           0x01
#define HDRRUPC_DESIRED_MAX_NUM_RL_SUPPORTED           0x01
#define HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_NO_JAMMER     0x0000
#define HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_W_JAMMER      0x0000
#define HDRRUPC_DESIRED_MAX_RL_BANDWIDTH               0x0000
#define HDRRUPC_DESIRED_MAX_NUM_SUB_ASETS              0x01
#define HDRRUPC_DESIRED_FWD_FEEDBACK_MUX_SUPPORTED     0x00
#else
#define HDRRUPC_DESIRED_MAX_NUM_FL_SUPPORTED           0x03
#define HDRRUPC_DESIRED_MAX_NUM_RL_SUPPORTED           0x03
#define HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_NO_JAMMER     0x0810
#define HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_W_JAMMER      0x0810
#define HDRRUPC_DESIRED_MAX_RL_BANDWIDTH               0x0810
#define HDRRUPC_DESIRED_MAX_NUM_SUB_ASETS              0x03
#define HDRRUPC_DESIRED_FWD_FEEDBACK_MUX_SUPPORTED     0x00
#endif

typedef struct
{
  uint8 band_class;
  uint8 band_sub_class_cnt;
  uint8 band_sub_class[HDRRUPC_MAX_SUPPORTED_SUB_BANDS];

} hdrrupc_band_class_rec_type;

typedef struct
{
  uint8 value_id;
  uint8 band_class_cnt;
  hdrrupc_band_class_rec_type band_class_list[HDRRUPC_MAX_SUPPORTED_BANDS];

} hdrrupc_supp_cdma_chan_attrib_type;

typedef struct
{
  /* Search Parameters */
  hdrsrch_search_params_struct_type srch;  

  /* Set Management Same Channel Paramters 
   * revB : renamed - SetManagementParameters*/
  hdrsrch_chan_mgmt_params_struct_type smsc;    

  /* Set Management Diff. Channel Paramters. revA only */
  hdrsrch_chan_mgmt_params_struct_type smdc;    

  /* Set Management Override Allowed */
  uint8 smoa;

  /* RouteUpdateRadiusMultiply */
  uint8 rup_rad_mult_attrib;

  /* RouteUpdateRadiusAdd */
  int16 rup_rad_add_attrib;

  /* SupportRouteUpdateEnhancements */
  uint8 supp_rup_enh_attrib;

  /* SupportedCDMAChannels */
  /* revB: new fields: InterFlexDuplexTag, IntraFlexDuplexFlag */
  hdrrupc_supp_cdma_chan_attrib_type succ;

  /* revB - simple attribute*/
  /* MaxNumberofFLSupported*/
  uint8 max_num_fl_supported_attrib;

  /* MaxNumberofRLSupported*/
  uint8 max_num_rl_supported_attrib;

  /* MaxForwardLinkBandwidthnoJammer*/
  uint16 max_fl_bdwidth_no_jammer_attrib;

  /* MaxForwardLinkBandwidthWithJammer*/
  uint16 max_fl_bdwidth_w_jammer_attrib;

  /* MaxReverseLinkBandwith*/
  uint16 max_rl_bdwidth_attrib;

  /* MaxNumberOfSub-ActiveSets*/
  uint8 max_num_sub_asets_attrib;

  /* ForwardFeedbackMultiplexingSupported*/
  uint8 fwd_fb_mux_supp_attrib;

  /* revB - complex attribute*/
  /* SupportedDRXPatterns - Not Planned*/

} hdrrupc_config_params_type;

typedef struct
{
  /* SupportedCDMAChannels */
  /* revB: new fields: InterFlexDuplexTag, IntraFlexDuplexFlag */
  hdrrupc_supp_cdma_chan_attrib_type succ;

  /* revB - simple attribute*/
  /* MaxNumberofFLSupported*/
  uint8 max_num_fl_supported_attrib;

  /* MaxNumberofRLSupported*/
  uint8 max_num_rl_supported_attrib;

  /* MaxForwardLinkBandwidthnoJammer*/
  uint16 max_fl_bdwidth_no_jammer_attrib;

  /* MaxForwardLinkBandwidthWithJammer*/
  uint16 max_fl_bdwidth_w_jammer_attrib;

  /* MaxReverseLinkBandwith*/
  uint16 max_rl_bdwidth_attrib;

  /* MaxNumberOfSub-ActiveSets*/
  uint8 max_num_sub_asets_attrib;

  /* ForwardFeedbackMultiplexingSupported*/
  uint8 fwd_fb_mux_supp_attrib;

  /* revB - complex attribute*/
  /* SupportedDRXPatterns - Not Planned*/

} hdrrupc_config_at_init_params_type;

/*===========================================================================

FUNCTION HDRRUPC_POWERUP_INIT

DESCRIPTION
  This function performs the powerup initialization of the rup state
  configuration negotiation module.

DEPENDENCIES
  This function must called only once after powerup

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrupc_powerup_init ( void );

/*===========================================================================

FUNCTION HDRRUPC_RESTORE_CURRENT_SRCH_PARAMS

DESCRIPTION
  This function restores config params if they were overwritten in connected
  state.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrrupc_restore_current_srch_params ( void );

/*===========================================================================

FUNCTION HDRRUPC_PROCESS_MSG

DESCRIPTION
  This function processes messages for RUPC.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message
  msg_id - The message ID already extracted from the DSM item

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrupc_process_msg 
(
  dsm_item_type   *msg_ptr, 
  uint8           msg_id
);

/*===========================================================================

FUNCTION HDRRUPC_PROCESS_IND

DESCRIPTION
  This function processes indications given to RUPC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for LUP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrupc_process_ind 
(
  hdrind_ind_name_enum_type    ind_name,
  hdrind_ind_data_union_type   *ind_data_ptr
);

/*===========================================================================

FUNCTION HDRRUP_GET_RUP_RAD_MULT_ATTRIB

DESCRIPTION
  This function is an accessor for the RouteUpdateRadiusMultiply simple 
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the RouteUpdateRadiusMultiply attribute.
  Valid return values are:
    0x00 - Indicates distance based registration is disabled
    0x0a - Default multiplier for the Route Update radius (x1)
    0x01 to 0x64 - Multiplier for the Route Update radius in units of 0.1
  

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrrupc_get_rup_rad_mult_attrib( void );

/*===========================================================================

FUNCTION HDRRUP_GET_RUP_RAD_ADD_ATTRIB

DESCRIPTION
  This function is an accessor for the RouteUpdateRadiusAdd simple attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the RouteUpdateRadiusAdd attribute.
  Valid return values are:
    0x00 - Default; addition to the Route Update radius of 0
    0x0001 to 0x0fff - Addition to the Route Update radius expressed as a
      12 bit two's complement number in 16 bit number space

SIDE EFFECTS
  None.

===========================================================================*/
extern int16 hdrrupc_get_rup_rad_add_attrib( void );

/*===========================================================================

FUNCTION HDRRUPC_GET_SUPP_RUP_ENH_ATTRIB

DESCRIPTION
  This function is an accessor for the SupportRouteUpdateEnhancements simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the SupportRouteUpdateEnhancements attribute.
  Valid return values are:
    0x00 - Indicates no support for RouteUpdateRequest messages or GAUPing
      of attributes RouteUpdateRadiusMultiply and RouteUpdateRadiusAdd
    0x01 - Indicates support for RouteUpdateRequest messages with no 
      Channel Records and GAUPing of attributes RouteUpdateRadiusMultiply and
      RouteUpdateRadiusAdd

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrrupc_get_supp_rup_enh_attrib( void );

/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_NUM_SUB_ASETS_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxNumberOfSub-ActiveSets simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxNumberOfSub-ActiveSets attribute.
  Valid return values are:
    0x01 - Indicates The access terminal supports a single sub-Active Set only. 
    0x02 to 0x10 - Indicates the maximum number of sub-Active Sets that the 
                   access terminal can support. 
      

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrrupc_get_max_num_sub_asets_attrib( void );


/*===========================================================================

FUNCTION HDRRUPC_CONFIG_IS_NEEDED

DESCRIPTION
  This function determines whether RUP requires any configuration.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if RUP needs configuring; FALSE if RUP does not need any configuration.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrrupc_config_is_needed( void );

/*===========================================================================

FUNCTION HDRRUPC_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes Route Update Protocol's configuration parameters
  to default. 

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Route Update Protocol's current configuration 
                   structure in SCMDB.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrupc_set_config_to_default
( 
  hdrrupc_config_params_type *current_config 
);

/*===========================================================================

FUNCTION HDRRUPC_CONFIG_IS_VALID

DESCRIPTION
  This function checks if negotiated values are valid.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrrupc_config_is_valid( void );

/*===========================================================================

FUNCTION HDRRUPC_START_CONFIG

DESCRIPTION
  This function begins the AT initialized configuration process.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrrupc_start_config( void );

/*===========================================================================

FUNCTION HDRRUPC_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of RUP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of RUP

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdrrupc_get_inconfig_handle(void);

/*===========================================================================

FUNCTION HDRRUPC_BAND_PREF_IS_CHANGED

DESCRIPTION
  Determines if the band preference was changed.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrrupc_band_pref_is_changed( void );

/*===========================================================================

FUNCTION HDRRUPC_SUCC_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether SupportedCDMAChannels  attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrrupc_succ_attrib_config_is_needed(void);

/*===========================================================================

FUNCTION HDRRUPC_MAX_NUM_FL_SUPPORTED_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxNumberofFLSupported attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrrupc_max_num_fl_supported_attrib_config_is_needed(void);

/*===========================================================================

FUNCTION HDRRUPC_MAX_NUM_RL_SUPPORTED_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxNumberofRLSupported attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrrupc_max_num_rl_supported_attrib_config_is_needed(void);

/*===========================================================================

FUNCTION HDRRUPC_MAX_FL_BDWIDTH_NO_JAMMER_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxForwardLnikBandwidthNoJammer attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrrupc_max_fl_bdwidth_no_jammer_attrib_config_is_needed(void);

/*===========================================================================

FUNCTION HDRRUPC_MAX_FL_BDWIDTH_W_JAMMER_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxForwardLnikBandwidthWithJammer attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrrupc_max_fl_bdwidth_w_jammer_attrib_config_is_needed(void);

/*===========================================================================

FUNCTION HDRRUPC_MAX_RL_BDWIDTH_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxReverseLinkBandwidth attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrrupc_max_rl_bdwidth_attrib_config_is_needed(void);

/*===========================================================================

FUNCTION HDRRUPC_MAX_NUM_SUB_ASETS_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether MaxNumberOfSub-ActiveSets attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrrupc_max_num_sub_asets_attrib_config_is_needed(void);

/*===========================================================================

FUNCTION HDRRUPC_FWD_FEEDBACK_MUX_SUPPORTED_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether ForwardFeedbackMultilexingSupported attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrrupc_fwd_feedback_mux_supported_attrib_config_is_needed(void);

/*===========================================================================

FUNCTION HDRRUPC_BUILD_SUPP_CDMA_CHAN_ATTRIB

DESCRIPTION
  This function queries the RF and SD modules to determine which band
  classes and band subclasses are supported by the AT.

DEPENDENCIES
  None.

PARAMETERS
  succ_attrib_ptr - A pointer to a SupportedCDMAChannels attribute
    structure.

RETURN VALUE
  The final length of the attribute and the succ_attrib_ptr is updated to 
  reflect the supported band classes.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrrupc_build_supp_cdma_chan_attrib
(
  hdrrupc_supp_cdma_chan_attrib_type *succ_attrib_ptr
);

/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_FL_BDWIDTH_NO_JAMMER_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxForwardLinkBandwidthNoJammer simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxForwardLinkBandwidthNoJammer attribute.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 hdrrupc_get_max_fl_bdwidth_no_jammer_attrib( void );

/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_FL_BDWIDTH_With_JAMMER_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxForwardLinkBandwidthWithJammer simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxForwardLinkBandwidthWithJammer attribute.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 hdrrupc_get_max_fl_bdwidth_w_jammer_attrib( void );

/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_RL_BDWIDTH_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxReverseLinkBandwidth simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxReverseLinkBandwidth attribute.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 hdrrupc_get_max_rl_bdwidth_attrib( void );

/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_NUM_FL_SUPPORTED_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxNumberofFLSupported simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxNumberofFLSupported attribute.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrrupc_get_max_num_fl_supported_attrib( void );

/*===========================================================================

FUNCTION HDRRUPC_GET_MAX_NUM_RL_SUPPORTED_ATTRIB

DESCRIPTION
  This function is an accessor for the MaxNumberofRLSupported simple
  attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxNumberofRLSupported attribute.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrrupc_get_max_num_rl_supported_attrib( void );

/*===========================================================================

FUNCTION HDRRUPC_GET_FWD_FB_MUX_SUPP_ATTRIB

DESCRIPTION
  This function is an accessor for the ForwardFeedbackMultiplexingSupported 
  simple attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the MaxForwardLinkBandwidthNoJammer attribute.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrrupc_get_fwd_fb_mux_supp_attrib( void );

/*===========================================================================

FUNCTION HDRRUPC_GET_PILOT_INC

DESCRIPTION
  This function is an accessor for the PilotIncrement field in 
  SearchParamters attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The negotiated value of the the PilotIncrement field in SearchParamters
  attribute.
      

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrrupc_get_pilot_inc( void );

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRRUPC_GET_PACKED_CDMA_BANDCLASS_INFO

DESCRIPTION
  This function is called to pack the A21 mobile subscription information 
  requested by LTE.
  
DEPENDENCIES
  None.

PARAMETERS
  band_pref - Band preference
  capabilities_container_ptr - Location where to store the packed values.

RETURN VALUE
  Size of the packed data.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint32 hdrrupc_get_packed_cdma_bandclass_info
( 
  sd_ss_band_pref_e_type band_pref,
  uint8 *capabilities_container_ptr 
);
#endif /* FEATURE_LTE_TO_HDR */
#endif /* HDRRUPC_H */
