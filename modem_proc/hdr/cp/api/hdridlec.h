#ifndef HDRIDLEC_H
#define HDRIDLEC_H

/*===========================================================================

          I N I T I A L I Z A T I O N S T A T E  P R O T O C O L
             C O N F I G U R A T I O N  D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for hdridles.c.

Copyright (c) 2000-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdridlec.h_v   1.7   17 Apr 2002 11:00:00   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdridlec.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/13   ukl     Set PCCC on entering idle state.
09/23/11   smd     SU API cleanup.
02/17/09   pba     Do not accpet or send GAUP of slot cycle < 6 when 
                   SmallSlotCycleAllowed attribute is set to 0.
12/23/08   pba     Support for Quick Idle Protocol
05/22/07   sju     Added support for phase2 of AT-init GAUP of SlottedMode attribute.    
09/29/06   yll     Treat SCI < 5 as non-slotted mode
06/14/06   etv     Added support for AT init GAUP of SlottedMode
02/15/06   hal     Changed default AccessHashingClassMask from 0xFFFF to 
                   0x0000 to be standards compliant
12/13/05   pba     Changes to support multiple personality negotiation
11/21/05   hal     Added support for the AccessHashingChannelMask attribute
11/14/05   dna     Phase 0 support for multiple personalities
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
07/07/05   dna     Added support for Enhanced Idle State protocol
03/29/02   dna     Changed timers to use REX callback instead of indications
05/31/01   SH      Added hdridlec_init_config_if_empty.
04/17/01   SH      Removed negotiation state
03/14/01   SH      Added Sending Config request and Process Config response
02/23/00   SH      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrhai.h"
#include "hdrcp_api.h"
/*===========================================================================

                    DATA DECLARATIONS

===========================================================================*/

#define HDRIDLEC_CONFIG_REQ_ID 0x50  /* message id for the config 
                                        request msg */
#define HDRIDLEC_CONFIG_RES_ID 0x51  /* message id for the config 
                                        response msg */
#define HDRIDLEC_ATTRIB_UPDATE_REQ_ID 0x52  /* AttributeUpdateRequest */
#define HDRIDLEC_ATTRIB_UPDATE_ACCEPT_ID 0x53  /* AttributeUpdateAccept */
#define HDRIDLEC_ATTRIB_UPDATE_REJECT_ID 0x54  /* AttributeUpdateReject */

#define HDRIDLEC_ACCESS_HASHING_CHANNEL_MASK_ATTRIB_DEFAULT 0x0000

typedef struct
{
  /* Whether the save slotted mode config is valid, it can also be used to determine
   * if SlottedMode has been overridden or not (e.g. TRUE: saved info is valid (sm override))
   */
  boolean saved_slotted_mode_is_valid;

  /* If app overrides, save the original slotted mode config here */
  hdridlec_slotted_mode_type saved_slotted_mode;

} hdridlec_saved_scmidle_info_type;

/* Idle PCCC configuration control block structs */
typedef struct
{
  boolean    is_custom;
  uint16     value;       /* value for the preferred
                              control channel cycle */
}hdridlec_pccc_config_attrib_type;

typedef struct
{
  boolean quick_paging_enabled;
  boolean subsync_quick_paging;

}hdridlec_quick_paging_attrib_type;

/* Idle configuration parameter */
typedef struct
{
  hdridlec_pccc_config_attrib_type  pccc;
  hdridlec_slotted_mode_type        slotted_mode;
  boolean                           small_slot_cycle_is_allowed;
  uint64                            slot_cycle1_timeout;
    /* System time at which to change from SlotCcyle1 to SlotCycle2 */
  uint64                            slot_cycle2_timeout;
    /* System time at which to change from SlotCcyle2 to SlotCycle3 */
  uint16                            access_hashing_channel_mask;
   /* AccessHashingChannelMask modifies the set of available channels
      over which the AT hashes; default is 0xffff */
  hdridlec_saved_scmidle_info_type  saved_scmidle_info;

  uint16                            connectiondeny_backoff;
  /* AT has to observe a backoff period specified by this attribute (in units
     of control channel cycles) after receiving a ConnectionDeny message with 
     DenyReason field set to 0x01 before sending an access terminal-initiated 
     ConnectionRequest message */
  uint16                            pageresponse_apersistence_supported;

  hdridlec_quick_paging_attrib_type quick_paging; 
    /* Quick Paging attribute */

#ifdef FEATURE_HDR_REVC
  boolean                           multi_page_supported;
#endif /* FEATURE_HDR_REVC */
}hdridlec_config_params_type;

/*===========================================================================

FUNCTION HDRIDLEC_CONFIG_IS_NEEDED

DESCRIPTION
  This function check whether the configuration negotiation is needed
  
DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - values match

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdridlec_config_is_needed(void);

/*===========================================================================

FUNCTION HDRIDLEC_PCCC_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function check whether the PreferredControlChannelCycle attribute
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
extern boolean hdridlec_pccc_attrib_config_is_needed(void);

/*===========================================================================

FUNCTION HDRIDLE_CONFIGURE_PCC_ONLY

DESCRIPTION
  Have idle state protocol propose the PreferredControlChannelCycle
  attribute as part of AT initiated session negotiation.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdridlec_configure_pccc_only
( 
  hdrhai_protocol_name_enum_type sender 
);

/*===========================================================================

FUNCTION HDRIDLEC_POWERUP_INIT

DESCRIPTION
  This function performs the powerup initialization of the idle state configuration
  negotiation module.

DEPENDENCIES
  This function must called only once after powerup

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridlec_powerup_init ( void );

/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_IND

DESCRIPTION
  This function processes indications given to IDLEC.

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
extern void hdridlec_process_ind 
(
  hdrind_ind_name_enum_type    ind_name,
  hdrind_ind_data_union_type   *ind_data_ptr
);


/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_MSG

DESCRIPTION
  This function processes messages for IDLEC.

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
extern void hdridlec_process_msg 
(
  dsm_item_type   *msg_ptr, 
  uint8           msg_id
);

/*===========================================================================

FUNCTION HDRIDLEC_START_CONFIG

DESCRIPTION
  This function start idle state pccc configuration

DEPENDENCIES
  hdridlec_init should have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridlec_start_config ( void );


/*===========================================================================

FUNCTION HDRIDLEC_UPDATE_CONFIG

DESCRIPTION
  This function update the configuration if needed
  
DEPENDENCIES
  hdridlec_init should have been called before calling this function.

PARAMETERS
  reason - reason for the reconfig request

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridlec_update_config(void);

/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_CONFIG_RES_TIMEOUT

DESCRIPTION
  This function processes the config response timer timeout.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridlec_process_config_res_timeout ( void );

/*===========================================================================

FUNCTION HDRIDLEC_GET_CURRENT_PCCC

DESCRIPTION
  This function returns the current Preferred Control Channel Cycle
  negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdridlec_get_current_pccc( void );

/*===========================================================================

FUNCTION HDRIDLEC_SET_CCC

DESCRIPTION
  This function determines the Control Channel Cycle to be used and gives 
  it to HDRSRCH.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_set_ccc (void);

/*===========================================================================

FUNCTION HDRIDLEC_GET_SLOTTED_MODE_ATTRIB

DESCRIPTION
  This function returns the current SlottedMode attribute
  negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridlec_get_slotted_mode_attrib
( 
  hdridlec_slotted_mode_type *slotted_mode_attrib_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_SET_CURRENT_PCCC_TO_DEFAULT

DESCRIPTION
  This function sets the current PCCC value based on the hash algorithm.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridlec_set_current_pccc_to_default(void);

/*===========================================================================

FUNCTION HDRIDLEC_SET_CONFIG_PARAMS_DEFAULT

DESCRIPTION
  This function initializes the current configuration values to defaults.

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to IDLE protocol's current configuration structure
                   in SCMDB.

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridlec_set_config_params_default
( 
  hdridlec_config_params_type *current_config 
);

/*===========================================================================

FUNCTION HDRIDLEC_SET_IDLE_SLOT_CYCLE_TIMEOUT

DESCRIPTION
  This function sets the current and negotiated value of IDLE slot cycle
  timeout to the given value.

DEPENDENCIES
  None

PARAMETERS
  t1 - System time at which to change from SlotCycle1 to SlotCycle2
  t2 - System time at which to change from SlotCcyle2 to SlotCycle3

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridlec_set_idle_slot_cycle_timeout
(
  uint64 t1,
  uint64 t2
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_GET_IDLE_SLOT_CYCLE_TIMEOUT

DESCRIPTION
  This function returns the current IDLE slot cycle timeout values.

DEPENDENCIES
  None

PARAMETERS
  t1 - System time at which to change from SlotCycle1 to SlotCycle2
  t2 - System time at which to change from SlotCcyle2 to SlotCycle3

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridlec_get_idle_slot_cycle_timeout
(
  uint64 *t1,
  uint64 *t2
);

/*===========================================================================

FUNCTION HDRIDLEC_GET_ACCESS_CHANNEL_HASHING_MASK

DESCRIPTION
  This accessor function returns the current value of the access channel
  hashing mask attribute.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The current AccessChannelHashingMask attribute.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 hdridlec_get_access_hashing_channel_mask( void );

/*===========================================================================

FUNCTION HDRIDLEC_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of IDLE State Protocol

DEPENDENCIES
  None

PARAMETERS
  None                   
  
RETURN VALUE
  Pointer to inconfig instance IDLE State Protocol

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdridlec_get_inconfig_handle(void);

/*===========================================================================

FUNCTION HDRIDLEC_SLOTTED_MODE_IS_OK_TO_SLEEP

DESCRIPTION
  AST doesn't want to support the configuration and testing of SCI < 5.
  We treat all SCI < 5 as non-slotted mode.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE: if it's OK to go to sleep
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdridlec_slotted_mode_is_ok_to_sleep(void);

/*===========================================================================

FUNCTION hdridlec_qpch_is_enabled

DESCRIPTION
  This function returns whether QPCH is enabled or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE: if AT is allowed to do QPCH
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdridlec_qpch_is_enabled(void);

/*===========================================================================

FUNCTION HDRIDLEC_IS_SMALL_SLOTCYCLE_GAUP_ALLOWED

DESCRIPTION
  This function returns whether AUR of small slot cycle is allowed or not.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  TRUE - If GAUP of small slot cycle is allowed, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdridlec_is_small_slotcycle_gaup_allowed ( void );

#endif /* HDRIDLEC_H */

