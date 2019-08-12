#ifndef SNM_I_H
#define SNM_I_H
/*===========================================================================

          S E R V I C E    N E G O T I A T I O N   M A N A G E R
         S U P P O R T   F O R   M E D I U M   D A T A   R A T E

GENERAL DESCRIPTION
  This module contains definitions and interfaces for service negotiation
  manager (SNM).  These functions are created for support of MDR Release 1.0 and
  it is taken into consideration that a service options will only be gradually
  transitioned into SNM.  Initially, only MDR test and DS service options will
  be managed by SNM.

DESCRIPTION
  This module contains definitions and interfaces for service configuration
  and negotiation for Data Services service options.  This functions are
  created for support of MDR Release 1.0.  Note that once Call Manager is
  implemented, these function may become obsolete.

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
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
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/snm.h_v   1.21   04 Oct 2002 15:30:08   varavamu  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/snm_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/14   agh     Support for new customer specific voice service option
09/27/13   ppr     Reject ESCAM if Service Connect Msg not received.
08/10/12   ppr     Added snm_update_pending_with_current_config() API
05/24/12   ppr     Feature Cleanup
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/08/11   ag      Support for fpc_mode field in RCPM and RCP record of GEM.
12/10/10   ag      Increased MAX_SUPPORTED_SO for accomodating SO 75.
11/15/10   jtm     Initial SU API split for CP.
12/01/09   ag      Increased MAX_SUPPORTED_SO for accomodating SO 73 and SO 74.
06/03/09   jj      Updaed few CMI Phase2 changes
05/14/09   ag      Fixed a compiler error
04/09/09   jj      CMI Phase2 activity.
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
03/19/07   pg      Lint Cleanup.
06/23/06   pg      Added support for RPC.
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
05/17/06   fh      Merged in new development of AKA/AES features
04/24/06   an      Fixed lint errors
10/10/05   fh      Removed including of dec.h.
08/03/05   fc      Fixed lint errors.
05/03/05   fc      Merged the following from MSMSHARED:
04/27/05   an       Added support for 4GV service option
04/19/05   fc      Merged the following from MSMSHARED:
04/12/05   az       Fix for CR 60463. SDB_OMIT info is being passed through
                    callback to DS for assembling SDB header.
03/28/05   fc      Made FCS also available to Rel D.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
12/22/04   fc      Code review comment changes.
09/28/04   fc      SCR mask updates.
09/16/04   fc      Added support for reverse DV.
08/09/04   fc      Service negotiation changes for Release C and D.
06/30/04   bkm     Added prototype for snm_set_default_fields_in_nnscr().
06/28/04   bkm     Match signature for snm_update_non_neg_configuration() as
                   in implementation.
06/16/04   fc      Support for Release C addendum SYNC ID changes.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  05/26/04   fc      Excluded mccsyncid.h.
  05/21/04   fc      Lint clean up.
04/08/04   bkm     Channel mask updates.
03/26/04   bkm     Merged the following from IS2000_REL_B_C:
  03/12/04   fc      Added support for Release C fast call setup.
01/05/04   bkm     Misc updates to support DV F-PDCH.
12/04/03   bkm     Add snm_so_supports_pdch().
12/02/03   bkm     Update SNM ifaces to use separate fwd/rev channel masks
                   instead of OTA chind. Also remove P2 featurization on
                   internal chind variable.
08/19/03   ph      Increased MAX_SUPPORTED_SO for accomodating SO_8026
08/14/03   va      Removed FEATURE_JCDMA_MAX_RATE feature for snm_set_sch_mux_list
08/01/03   ph      Added chind to snm_eval_config().
06/18/03   fc      Added support for the decoupling of FCH and DCCH MUX.
06/04/03   va      Added interface to get RLP BLOB for CAP IR :
                   snm_get_rlp_cap_blob
04/23/03   bkm     Added declaration of snm_set_sch_mux_list().
03/28/03   va      Added support for Release B features.
11/11/02   ph      Fixed typo.
11/07/02   az      Removed FEATURE_3GDATA_PROTOTYPE
11/07/02   az      Mainlined FEATURE_IS95B
10/03/02   va      Added fn prototype for snm_set_default_fields_in_scr
10/03/02   ph      Lint Cleanup
10/02/02   az      Mainlined FEATURE_IS2000
08/02/02   ph      Fix for rel A turned off.
07/02/02   az      Increased the value of MAX_SUPPORTED_SO by 1
06/28/02   ph      Included ch_ind for snm_update_lpm_table(). Added P2 featurization
                   to snm_is2000_nnscr_lpm_default().
06/18/02   ph      Added new function prototype snm_verify_phy_chan_allowed
05/29/02   ph      Added support for LPM table
05/02/02   ph      Added QOS support and some cleanup.
05/01/02   ph      Some cleanup of featurization.
03/19/02   ph      Changed snm_is_eom_so_compatible_cur_cfg() to allow SO to
                   be changed within the function.
01/23/02   ph      Added support for proposing SVD configurations and handling EOM.
12/11/01   ph      merged in SVD branch
                   11/30/01   va      Added prototype for snm_get_grp_num
                   09/04/01   ph      Included variable to allow sch_cc verification for SVD
                   08/23/01   ph      Updates for SVD.
08/09/01   ph      Merged in from MSM_CP.05.00.87
                   08/06/01   jrp     Added dependence on dec.h to solve nested include problem.
                                      dssocfg.c depended on snm.h including dec.h.
                   08/02/01   jrp     Changed RCLIST_MAX from 3 to 5 to support additional RC
                                      options in SO54, and SO55.  Removed dependence on dec.h by
                                      using cai_rate_set_type.
06/26/01   ph      Merged in from MSM_CP.05.00.81
                   06/01/01   fc      Changed snm_update_non_neg_configuration to accept constant
                                      pointer as argument.
                   05/29/01   fc      Changed snm_update_non_neg_configuration function prototype.
                   05/22/01   lcc     Added prototype for function snm_verify_chind.
05/25/01   va      Changed the prototype of snm_update_non_neg_cfg
05/17/01   ph      Merged from MSM_CP.05.00.76
                   04/10/01   lcc     1. Changed snm_neg_par_type to support DCCH.
                                      2. Changed prototypes of snm_get_current_config and
                                        snm_get_current_nnscr to return the pointers directly.
                                      3. Added function snm_update_fpc_pri_chan.
                   03/31/01   fc      Added prototype for snm_update_non_neg_configuration.
03/14/01   va      Increased MAX_SUPPORTED_SO to 35.
03/02/01   va      Merged the following from common archive:
  01/17/01 lcc     Added function prototypes for snm_is2000_scr_init and
                   snm_is2000_nnscr_init.
27/02/01   kk      Added prototypes for Release A functions
11/02/00   lcc     Added function snm_eval_so for FEATURE_COMPLETE_SNM.
06/22/00   va      Support for caching the pref RC NV item.
06/12/00   va      Support for mux negotiation
06/05/00   lcc     Corrected some featurization.
06/02/00   lcc     Added prototypes for snm_reg_rlp_blob_generator,
                   snm_get_complete_pending_config, and snm_get_current_nnscr.
05/05/00   lcc     Put back the prototype of snm_initialize_connection when FEATURE_IS2000
                   is not defined.  It was removed by accident.
04/24/00   lcc     Added rlp_blob related stuff.  Separated snm_update_configuration
                   into 2 functions: snm_update_configuration and snm_continue_update_configuration
                   Some minor cleanup as well.
04/17/00   va      Made snm_get_pref_RCs return nothing.
04/05/00   va      Support for mux opt list.
03/15/00   ry      Added IS2000 featurization
02/03/00   lcc     Added support for RC list.
11/15/99   va      IS2000 Voice Changes:
                   snm_get_pref_rcs() function.
                   Support for storing non negotiable service configuration and
                   evaluating it snm_eval_non_neg_scr()
07/30/99   lcc     Added typedef for snm_eval_status_type.
07/19/99   nmn     Added declaration of snm_get_alt_so_list, surrounded by
                   #ifdef of FEATURE_IS95B or FEATURE_SPECIAL_MDR.
06/30/99   kmp     Remove #includes of msg.h and err.h since they are not
                   necessary.
04/29/99   lcc     Corrected some comments and some cleanup.
02/16/99   nmn     Cleaned, linted, and tested SOCM support
02/12/99   lcc     Added several function to get information from SNM as well as
                   changed some names to conform to standard.
12/09/98   lcc     Changed snm_initialize_connection to return a status.
12/03/98   lcc     Minor comment related changes.  Code not affected.
11/25/98   lcc     Initial release

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "cai.h"
#include "caii.h"
#include "caii_v.h"
#include "caii_i.h"
#include "mcc.h"
#include "mcc_v.h"
#include "mcc_i.h"
#include "snm.h"
#include "snm_v.h"

#ifdef FEATURE_IS2000_1X_ADV
#define MAX_SUPPORTED_SO  43  /* Reserve enough space for all SOs we support now */
#else
#define MAX_SUPPORTED_SO  40  /* Reserve enough space for all SOs we support now */
#endif

#define MAX_CON          CAI_MAX_NUM_CONNECTIONS

/* MAX_SNM_CON is the number of connections that snm can hold information about
   during Service Negotiation until the time we process a SCR in U/GHDM or
   SCM. We need more room than the number of connections we support (MAX_CON) so we
   can evaluate SCRs where the BS is dropping some and proposing new ones */
#define MAX_SNM_CON    (MAX_CON + 2)

/* Define Max value of SR_ID, used in snm.c */
#define MAX_SR_ID_VALUE  6

/* Type to indicate the status of a service option */
typedef enum
{
  SNM_DISABLED,
  SNM_ENABLED
} snm_so_status_type;

/* Type to mux. options to use in a service configuration */
typedef enum
{
  SNM_DEFAULT_MUX,
  SNM_MAXIMUM_MUX
} snm_mux_config_type;

/* Type to capture the characteristics of each service option */
typedef struct
{
  word service_option_num;        /* Service option number */
  snm_so_status_type status;
  word page_response_so;          /* Service option to respond when paged */
  snm_neg_par_type negotiation_pars;
  snm_config_type default_config; /* Parameters for generating default config */
  void (*connection_initializer)(const caii_srv_cfg_type *cfg, word con);
    /* Points to initializer of connection */
  snm_socm_return_type (*socm_handler)(word length, byte* field, word serv_opt);
    /* Points to SOCM handler */
  uint8 (*rlp_blob_generator)(byte* blob);

#ifdef FEATURE_IS2000_REL_A
  byte (*qos_blob_generator) (byte * qos_blob);
  snm_eval_status_type (*qos_blob_eval) (byte    bs_blob_len,
                                         byte  * bs_blob,
                                         boolean can_suggest_alt,
                                         byte  * ret_alt_blob_len,
                                         byte  * ret_alt_blob
                                        );
  void (*sdb_so_omit_status_handler) (boolean sdb_so_omit_status);
#endif /* FEATURE_IS2000_REL_A */

} snm_so_descriptor_type;

typedef struct
{
  struct
  {
    word neg_seed;      /* Negotiation seed */
    word beginning_so;  /* First SO used - i.e. SO in orig or page response */
    word next_so_index; /* Points to next SO to propose */
    word* alt_so_list;  /* List of alternate service options */
    snm_so_descriptor_type* connection_sod;
      /* Pointers to service option descriptors of each connection in config. */

    /* Whe we support multiple connections, above members may have to be in
       arrays */

    boolean sr_id_valid;
    byte sr_id;                    /* To remember the service corresponding to SO we're negotiating */
    word prev_so;                  /* Previous SO we were evaluating */
    boolean max_mux_proposed;
      /* Indicates if max fundicated mux. options was proposed */

#ifdef FEATURE_IS2000_REL_A_SVD
    boolean so_can_be_changed; /* If SO can be changed during negotiation */
#endif /* FEATURE_IS2000_REL_A_SVD */

    byte cur_fwd_fch_rc_index;     /* Maintain entry for only fwd FCH RC list */

    byte cur_fwd_dcch_rc_index;    /* Maintain entry for only fwd DCCH RC list */

    snm_chan_mask_type fundicated_rc_proposed_chan_mask;
      /* Indicates the last fundicated channel that alternate RC was proposed for */
    byte cur_fwd_sch_mux_index;
    byte cur_rev_sch_mux_index;
  } con_list[MAX_SNM_CON];

  boolean current_config_valid;               /* Indicates if current_config
                                                 contains a negotiated config. */
  caii_srv_cfg_type* proposed_config;         /* Points to proposed configuration */
  caii_complete_srv_cfg_type* pending_config; /* Points to pending configuration */
  caii_complete_srv_cfg_type* current_config; /* Points to current configuration */

  boolean pref_rc_valid;
  cai_radio_config_type pref_fundicated_rc;

  boolean fch_cc_verified;     /* Indicates if FCH configuration verified */

  boolean dcch_cc_verified;    /* Indicates if DCCH configuration verified */

  boolean fwd_sch_cc_verified; /* Indicates if forward SCH configuration verified */

  boolean rev_sch_cc_verified; /* Indicates if reverse SCH configuration verified */

#ifdef FEATURE_IS2000_REL_A_SVD
  struct
  {
    byte traffic_type_eval_so_far; /* Indicates traffic types (PRI,SEC) evaluated so far */
    byte cfg_pri_con_index;        /* Index to connection in config for pri traffic */
    byte cfg_sec_con_index;        /* Index to connection in config for sec traffic */
  } fwd_traf, rev_traf;
#endif /* FEATURE_IS2000_REL_A_SVD */

} negotiation_info_type;

typedef struct
{
  word total_so; /* Number of SOs in array */
  snm_so_descriptor_type so[MAX_SUPPORTED_SO];
} snp_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION snm_find_so

DESCRIPTION
  This function locates a SO in SNP.  It returns failure is the SO is not
  in SNP.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if SO found.  SNM_FAILURE if SNP is empty or not initialized.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_find_so
(
  word so,    /* Service option number */
  word *index /* Variable to hold index to SO found */
);

/*===========================================================================

FUNCTION snm_initialize_snm

DESCRIPTION
  This function initializes the Service Negotiation Manager (SNM).  For the time
  being, all it does is to initialize SNP to contain no service options and to
  initialize some members in negotiation_info that needs initialized only once.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void snm_initialize_snm
(
  void
);

/*===========================================================================

FUNCTION snm_set_user_pref_fundicated_rc

DESCRIPTION
  This function caches the preferred fundicated RC value that is read from NV
  by MC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void snm_set_user_pref_fundicated_rc
(
  cai_radio_config_type pref_fundicated_rc
);

#ifdef FEATURE_IS2000_REL_A_AES
/*===========================================================================

FUNCTION snm_set_ui_encrypt_mode

DESCRIPTION
  This function sets ui_encrypt_mode field in SCR. For now it is only being
  called by AES

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void snm_set_ui_encrypt_mode
(
  uint8 sr_id,                /* the record with this sr_id will be updated */
  uint8 ui_encrypt_mode       /* the new ui_encrypt_mode */
);

/*===========================================================================

FUNCTION snm_get_voice_so_sr_id

DESCRIPTION
  This function returns the sr_id for the voice service if it exists. For now
  this function is only called by AES.

DEPENDENCIES

RETURN VALUE
  sr_id of the voice service. If no voice service in current configuration, it
  returns MCCAES_INVALID_VOICE_SR_ID

  At the same time, it set the ui_encrypt_mode for the voice service. If no
  voice service, the value set through the pointer does not mean anything.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 snm_get_voice_so_sr_id
(
  uint8 *ui_encrypt_mode
);


#endif /* FEATURE_IS2000_REL_A_AES */
/*===========================================================================

FUNCTION snm_get_page_response_so

DESCRIPTION
  This function returns the page_response_so member of the SO descriptor of
  the service option passed in as parameter.  If the SO is not found in SNP,
  the same service option is returned.

DEPENDENCIES
  None.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.

===========================================================================*/

word snm_get_page_response_so
(
  word so_number /* Service option number */
);

/*===========================================================================

FUNCTION snm_eval_paged_so

DESCRIPTION
  This function evaluates the SO paged by the base station (after translating
  with snm_get_page_response_so).  It does it by looking into SNP and see
  if the SO exists.  If it does, negotiation_info is updated with this SO as
  the seed for negotiation.  If this SO is enabled, the same SO is returned;
  otherwise, the list of alternative SOs will be traversed until the first
  enabled SO is found.  If an alternative does not exist or if they are all
  disabled, CAI_SO_REJ will be returned.

  If the SO is not in SNP, the list of compatible SOs (compatible_service_options)
  of each SO will be scanned.  The first enabled SO containing the evaluated SO
  in its compatible SO list will be returned, which will also be used as the seed.
  If this fails also, the old negotiation mechanism will be used (srv_eval_so
  is called to evaluated the SO).  If this fails also, CAI_SO_REJ is returned.

  This function also initializes negotiation_info to prepare for negotiation
  on the traffic channel.

DEPENDENCIES
  None.

RETURN VALUE
  If SO is supported, the same SO is returned; otherwise an alternate SO is
  returned.  If no alternate SO exists, CAI_SO_REJ is returned.

SIDE EFFECTS
  None.
===========================================================================*/

word snm_eval_paged_so
(
  word paged_so /* The service option number to be evaluated */
);

/*===========================================================================

FUNCTION snm_get_pref_fundicated_RCs

DESCRIPTION
  This function returns the preferred RC for the forward and reverse fundicated
  channel. This preference is sent back in the page resp or the origination
  message. The SNM has a RC list stored for each SO and it returns the first
  in the list.  For SOs not managed by SNM, it calls a corresponding function
  in SRV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_get_pref_fundicated_RCs
(
  word                  so,          /* The SO for which pref RCs are needed */
  cai_radio_config_type *fwd_rc_ptr, /* Storage for returned fwd RC */
  cai_radio_config_type *rev_rc_ptr  /* Storage for return rev RC */
);

/*===========================================================================

FUNCTION snm_get_user_pref_fundicated_rc

DESCRIPTION
  This function returns the user perferred fundicated RC as stored in NV
  that is cached by SNM.
  We need an interface for this because this value is needed by SRV also.
  Once it's all SOs are migrated to SNM, this value will be local to SNM.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if there is a user preferred RC, FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean snm_get_user_pref_fundicated_rc
(
  cai_radio_config_type *user_pref_fundicated_rc
);

/*===========================================================================

FUNCTION snm_so_is_enabled

DESCRIPTION
  This function evaluates if a SO can be used under the current operating
  conditions.  For SNM managed SOs, SNP will be consulted to see if
  the SO is enabled.  For other SOs, srv_eval_so is used, which support
  smooth transition of SOs to SNM.  Eventually, when all SOs are captured
  in SNP, the call to srv_eval_so will be obsolete.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SO is supported, FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean snm_so_is_enabled
(
  word so /* The service option number to be evaluated */
);

/*===========================================================================

FUNCTION snm_gen_config

DESCRIPTION
  This function builds a service configuration with default or max. mux.
  for a specified service option based on information stored in SNP.  When
  set_connection is FALSE, fch_cc_incl, dcch_cc_incl, for_sch_cc_incl,
  rev_sch_cc_incl are all set to false (used only when first get to TC before
  service negotiation is finished).

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS - Service configuration successfully built.
  SNM_FAILURE - service option not in SNP, default configuration generated.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_gen_config
(
  word so,
    /* The service option for which a default service configuration
       is to be constructed */
  caii_srv_cfg_type *cfg_ptr,
    /* Pointer to place to construct the default configuration. */
  boolean set_connection,
    /* Specifies whether or not to include a service option connection
       in the default service configuration. */
  snm_mux_config_type mux,
    /* Specifies whether default or maximum mux. options to use */
  byte con_index,
    /* Specifies index in to array of connections in the negotiation_info
       struct. */
  mcc_fwd_chan_mask_type  fwd_chan_mask,
    /* The fwd channel mask to use for evaluation when P_REV is >=6 */
  mcc_rev_chan_mask_type  rev_chan_mask
    /* The rev channel mask to use for evaluation when P_REV is >=6 */
);

/*===========================================================================

FUNCTION snm_compose_config

DESCRIPTION
  This function performs incremental update of new service configuration
  with old service configuration. It also updates the forward and reverse
  SCR channel mask.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void snm_compose_config
(
  caii_srv_cfg_type *new_cfg_ptr,
    /* New service configuration */
  caii_srv_cfg_type *old_cfg_ptr,
    /* Old service configuration */
  mcc_fwd_chan_mask_type fwd_chan_mask,
    /* Forward channel mask */
  mcc_rev_chan_mask_type rev_chan_mask,
    /* Reverse channel mask */
  snm_chan_mask_type *fwd_scr_chan_mask_ptr,
    /* Pointer to SCR forward channel mask */
  snm_chan_mask_type *rev_scr_chan_mask_ptr
    /* Pointer to SCR reverse channel mask */
);

/*===========================================================================

FUNCTION snm_eval_config

DESCRIPTION
  This function evaluates a specified service configuration to see if it is
  supported by the mobile station.

  It the negotiation is started by a SNM managed SO, then SNM style
  negotiation is used.  It returns SNM_ACCEPT_CFG if the configuration is
  considered optimal, or if suggest_alt is FALSE, as long as the
  configuration can be supported by the mobile.

  There are several scenarios that the proposed configuration is not accepted:
  1. Config. contains more than 1 connection
  2. Service option not supported or enabled
  3. Mux. option (and other configuration variables) not acceptable

  For (1), the list of SO connections is scanned through to see if one of them
  can be supported.  If so, a configuration will be generated as a counter-
  proposal; otherwise, it will be handled like (2).

  For (2), starting from the first SO in the alt_so_list, an alternative for
  that SO will be sought.  If
  such an SO is located, a configuration based on that will be proposed (which
  results in SNM_ALT_CFG being returned).  If not found, or if the list of
  alternate SOs is exhausted, SNM_REJECT_CFG is returned.

  For (3), a configuration will be generated by taking the min. of the base
  station's mux.  options and the mobile's max. mux. options (unless max. mux.
  options have not been proposed before and if so, max. mux. will be proposed).  The other
  variables in the configuration (e.g. traffic type) will be corrected if
  necessary.

  To support the old form of negotiation (SOs not managed by SNM yet), if the
  evaluated SO is not in SNM, this function will return failure with the first
  connection set to CAI_SO_NULL.  This information can be used by the caller to
  determine if it should proceed with the old negotiation mechanism should this
  function returns failure.

  Note that suggest_alt controls whether an alternative configuration is to be
  generated.  When suggest_alt is false, alt_cfg_ptr is not used.

DEPENDENCIES
  negotiation_info must have been initialized.

RETURN VALUE
  SNM_ACCEPT_CFG if configuration is accepted as is,
  SNM_ALT_CFG if an alternative configuration is to be used (in alt_cfg_ptr),
  SNM_REJECT_CFG if configuration is not supported and there is no alternative
  to propose.

SIDE EFFECTS
  negotiation_info is updated if necessary.
  alt_cfg_ptr normally points to srv_proposed_cfg which is also the
  proposed_config member of negotiation_info.

===========================================================================*/

snm_eval_status_type snm_eval_config
(
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated */
  caii_srv_cfg_type *alt_cfg_ptr,
    /* Location to put alternate service configuration if desired */
  boolean restore_ind,
    /* Indicates if config is to be restored */
  boolean suggest_alt,
    /* Indicates if a alternative configuration is to be generated */
  mcc_fwd_chan_mask_type  fwd_chan_mask,
    /* The fwd channel mask to use for evaluation when P_REV is >=6 */
  mcc_rev_chan_mask_type  rev_chan_mask
    /* The rev channel mask to use for evaluation when P_REV is >=6 */
);

/*===========================================================================

FUNCTION snm_eval_non_neg_scr

DESCRIPTION
  This function evaluates the non negotiable service configuration that
  is passed into it.  This makes sure we can accept all the parameters that
  the BS is giving us. This record needs to be evaluated along with the SCR
  to make sure if we have valid FPC parameters if we operating in new RCs etc.
  Non negotiable SCRs unlike SCRs are cumulative i.e new record does not completely
  wipe out the old record. We modify what is specified in this record and leave
  the rest as is. It is important to reset this record in current config properly.

DEPENDENCIES

RETURN VALUE
  SNM_SUCCESS if configuration is acceptable, SNM_FAILURE otherwise.

SIDE EFFECTS
===========================================================================*/

snm_status_type snm_eval_non_neg_scr
(
  caii_non_neg_srv_cfg_type *nn_cfg_ptr,
    /* The Non-Neg service configuration record to be evaluated */
  caii_srv_cfg_type *neg_cfg_ptr,
    /*  The SCR that goes with the Non-neg part */
  mcc_fwd_chan_mask_type  fwd_chan_mask,
    /* The fwd channel mask to use for evaluation */
  mcc_rev_chan_mask_type  rev_chan_mask
    /* The rev channel mask to use for evaluation */
);

/*===========================================================================

FUNCTION snm_get_pref_chan_ind_for_so

DESCRIPTION
  This function gets the preferred CH_IND and EXT_CH_IND for the specified SO.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_get_pref_chan_ind_for_so
(
  uint16 so,                      /* Service option for which CH_IND and
                                     EXT_CH_IND to be set */
  uint8 *ch_ind_ptr               /* Pointer to CH_IND */
);

/*===========================================================================

FUNCTION snm_verify_chan_mask_for_so

DESCRIPTION
  This function verifies if the channel mask specified can be used for the
  specified service option.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if supported; FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean snm_verify_chan_mask_for_so
(
  mcc_fwd_chan_mask_type fwd_chan_mask,
  mcc_rev_chan_mask_type rev_chan_mask,
  uint16                 so
);

/*===========================================================================

FUNCTION snm_gen_so_chan_mask

DESCRIPTION
  This function generates the channel mask that indicates physical channel
  allowed for the specified SO.

DEPENDENCIES
  None

RETURN VALUE
  Channel mask indicates physical channel supported by SO.

SIDE EFFECTS
  None.
===========================================================================*/

snm_chan_mask_type snm_gen_so_chan_mask
(
  uint16 so
);

/*===========================================================================

FUNCTION snm_update_pilot_gating_rate

DESCRIPTION
  Updates the value of pilot_GATING_RATE in non-negotiable SCR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_update_pilot_gating_rate
(
  cai_pilot_gating_rate_type gating_rate /* The pilot gating rate to be set to */
);

/*===========================================================================

FUNCTION snm_update_for_deleted_connection

DESCRIPTION
  Clean up negotiation_info.con_list[] for the entry corresponding to
  "deleted_sr_id". This function should be called when a call control
  instance is deleted or when an EOM fails.

DEPENDENCIES
  none.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_update_for_deleted_connection
(
  byte deleted_sr_id
);

/*===========================================================================

FUNCTION snm_update_with_new_config

DESCRIPTION
  Clean up negotiation_info.con_list[] for entries not in the current cfg;
  This is called by CCIM upon receiving a SCM or U/GHDM.

DEPENDENCIES
  An assumption being made here is that sr_id only has values ranging
  from 0 to 6 (MAX_SR_ID_VALUE). This is currently being enforced in SNM.
  If MAX_SR_ID_VALUE is changed in future, changes need to be made to this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_update_with_new_config
(
  caii_srv_cfg_type *new_cfg_ptr
);

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION snm_is_eom_so_compatible_cur_cfg

DESCRIPTION
  see if so is compatible with existing configuration when sending an EOM.
  Right now, we check to see if the existing rate set is compatible
  with what the SO can support.
  Also, we check if the pri/sec traffic types are compatible.
  If it is not, this function will try and find an alternate that is
  compatible and set it in 'so'

DEPENDENCIES
  none.

RETURN VALUE
  TRUE, if compatible or if alternate SO being used, FALSE otherwise.

SIDE EFFECTS
  The so can be potentially changed by this function to an alternate
  that is compatible with the current configuration.
===========================================================================*/

boolean snm_is_eom_so_compatible_cur_cfg
(
  word *so
);

/*===========================================================================

FUNCTION snm_eom_init

DESCRIPTION
  init snm's negotiation_info database with the eom SO and sr_id

DEPENDENCIES
  none.

RETURN VALUE
  TRUE if initialized, FALSE otherwise

SIDE EFFECTS
  none
===========================================================================*/

boolean snm_eom_init
(
  word so,
  byte sr_id
);
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION snm_validate_for_sch_cfg

DESCRIPTION
  Performs validation of For-SCH config received in ESCAM

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if successfully validated; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
snm_status_type snm_validate_for_sch_cfg
(
  int num_for_sch,
  caii_for_sch_type *for_sch_cfg
);

/*===========================================================================

FUNCTION snm_validate_rev_sch_cfg

DESCRIPTION
  Performs validation of rev-SCH config received in ESCAM

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if successfully validated; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_validate_rev_sch_cfg
(
  int num_rev_sch,
  caii_rev_sch_type *rev_sch_cfg
);

/*===========================================================================

FUNCTION snm_update_for_sch_cfg

DESCRIPTION
  Updates for-SCH config received in ESCAM

DEPENDENCIES
  validate_for_sch_cfg shoudl be called before this to validate the config

RETURN VALUE
  None

SIDE EFFECTS
  SCR is updated
===========================================================================*/

void snm_update_for_sch_cfg
(
  uint8 num_for_sch,
  caii_for_sch_type *for_sch_cfg
);

/*===========================================================================

FUNCTION snm_update_rev_sch_cfg

DESCRIPTION
  Updates rev-SCH config received in ESCAM

DEPENDENCIES
  validate_rev_sch_cfg shoudl be called before this to validate the config

RETURN VALUE
  None

SIDE EFFECTS
  SCR is updated
===========================================================================*/

void snm_update_rev_sch_cfg
(
  uint8 num_rev_sch,
  caii_rev_sch_type *rev_sch_cfg
);
#endif /*FEATURE_IS2000_REL_B */

/*===========================================================================

FUNCTION snm_orig_init

DESCRIPTION
  Performs some SNM initialization required for originations.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if successfully initialized; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_orig_init
(
  word originating_so
);

/*===========================================================================

FUNCTION snm_gen_son_config

DESCRIPTION
  Generates the configuration established with Service Option Negotiation.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if successfully initialized; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_gen_son_config
(
  word  so
);

/*===========================================================================

FUNCTION snm_update_non_neg_configuration

DESCRIPTION
  This function is called precisely at the action time of Service Connect
  Message or General Handoff Direction Message or Universal Handoff Direction
  Message to update the non-negotiatiable current configuration to what was
  given in the message.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_update_non_neg_configuration
(
  void
);

/*===========================================================================

FUNCTION snm_update_lpm_table

DESCRIPTION
  This function is called to update the lpm table parameters from a given
  non_neg_config to a target non_neg_cfg.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_update_lpm_table
(
  caii_non_neg_srv_cfg_type       *target_nnscr,
  const caii_non_neg_srv_cfg_type *new_lpm_nnscr,
  const caii_srv_cfg_type         *new_scr,
  mcc_fwd_chan_mask_type          fwd_chan_mask,
  mcc_rev_chan_mask_type          rev_chan_mask
);

/*===========================================================================

FUNCTION snm_set_default_fields_in_scr

DESCRIPTION
 This function defaults certain fields of the neg. service configuration from the
 current configuration. Need to do this since only pending configuration is used
 for HH-RIF cases.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_set_default_fields_in_scr
(
  caii_srv_cfg_type *neg_scr_ptr
);

/*===========================================================================

FUNCTION snm_set_default_fields_in_nnscr

DESCRIPTION
 This function defaults certain fields of the non-neg service configuration from the
 current configuration. Need to do this since only pending configuration is used
 for action packet generation

DEPENDENCIES
  Does not populate LPM table in NNSCR; That should be called separately

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void snm_set_default_fields_in_nnscr
(
  caii_non_neg_srv_cfg_type * nnscr
);

/*===========================================================================

FUNCTION snm_update_configuration

DESCRIPTION
  This function is called precisely at the action time of Service Connect Message
  to update the current configuration to what was given in the message.  Note that
  this function can be called from device driver's context (e.g. RXC) so it must
  only perform the absolute minimum.  This function perform the portion of SCR
  processing that's time critical (which includes calling the connection initializer).
  The function snm_activate_config does the other part of SCR processing that's not
  time critical.

DEPENDENCIES
  negotiation_info.pending_cfg must point to the pending configuration.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_update_configuration
(
  void
);

/*===========================================================================

FUNCTION snm_continue_update_config

DESCRIPTION
  This function is called at the action time of Service Connect Message to
  perform any processing related to the SCR that's not time critical and is
  called from MC's context.

DEPENDENCIES
  snm_update_configuration must have be called prior to this so that the
  current configuration contains what to be used.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_continue_update_config
(
  void
);

/*===========================================================================

FUNCTION snm_first_orig_config

DESCRIPTION
  Generates the first configuration to be proposed for an origination.  Max.
  mux. options will be used.  The generated configuration will be placed in
  *negotiation_info.proposed_cfg (pointing to srv_proposed_cfg).
  negotiation_info.max_mux_proposed will be set to TRUE.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if configuration successfully generated; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_first_orig_config
(
  word so
);

/*===========================================================================

FUNCTION snm_get_total_SOs

DESCRIPTION
  This function returns the total number of service options maintained by SNM.

DEPENDENCIES
  SNM must be initialized.

RETURN VALUE
  Number of SOs in SNM.

SIDE EFFECTS
  None
===========================================================================*/

word snm_get_total_SOs
(
  void
);

/*===========================================================================

FUNCTION snm_get_SO_number

DESCRIPTION
  This function returns service option number of the nth SO in SNM.

DEPENDENCIES
  SNM must be initialized.

RETURN VALUE
  Service option number of quried SO.  If the entry does not exist, CAI_SO_REJ
  is returned.

SIDE EFFECTS
  None
===========================================================================*/

word snm_get_SO_number
(
  word index
);


/*===========================================================================

FUNCTION snm_get_current_nnscr

DESCRIPTION
  This function returns the current non-negotiable part of service configuration.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if valid configuration has been negotiated.  SNM_FAILURE otherwise.
  If successful, pointer to current config is returned in function parameter.

SIDE EFFECTS
  None
===========================================================================*/

const caii_non_neg_srv_cfg_type*  snm_get_current_nnscr
(
  void
);

/*===========================================================================

FUNCTION snm_get_complete_pending_config

DESCRIPTION
  This function returns a pointer to the complete pending service configuration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

const caii_complete_srv_cfg_type* snm_get_complete_pending_config
(
   void
);


/*===========================================================================

FUNCTION snm_process_srv_opt_ctl_msg

DESCRIPTION
  This function returns the Service Option Control Message handler.

DEPENDENCIES
  None.

RETURN VALUE
  Return value from the call to the socm_handler, VALID if no handler
  exists, UNSPEC if the SO does not exist.

SIDE EFFECTS
  None
===========================================================================*/

snm_socm_return_type snm_process_srv_opt_ctl_msg
(
  caii_rx_msg_type* msg_ptr
);

/*===========================================================================

FUNCTION snm_get_alt_so_list

DESCRIPTION
  This function evaluates a given service option, returning the appropriate
  list of alternative service options.

DEPENDENCIES
  alt_so points to an array of bytes large enough to handle the maximum
  number of alternative service options allowed (max_alt_so).  This
  maximum is the lesser of the two values: max_num_alt_so received from
  ESPM, or the internal array size (defined in cai.h as CAI_AC_ALT_SO_MAX).

  The so_num must have already been checked to be a valid service option.

RETURN VALUE
  Returns the number of alternative service options in the alt_so list.

SIDE EFFECTS
  The values that alt_so point to will be changed to the alternative
  service options.

===========================================================================*/

byte snm_get_alt_so_list
(
  word so_num,
  byte max_alt_so,
  word alt_so[]
);

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION snm_convert_alt_so_bitmap

DESCRIPTION
  This function fills the Service Option Bitmap information based on the
  Alternative Service Options.
  Inputs:
    so_num          Primary Service Option number
    num_alt_so      Number of alternative SO's to be translated into bitmap
    alt_so[]        Alternative SO's to be translated into bitmap

  Outputs:
    so_bitmap_ind   Indicates if a bit_map is included, if so the length
    so_group_num    Group number for the primary SO, or if the primary SO
                    does not have a group number (ex, 0x8000), then the SO
                    group of the alternate Service Option is matched.
    so_bitmap       Bitmask of alternative Service Options that are in the
                    same group as so_group_num. See above.
    num_alt_so      Total number of alternative SO's that can not be placed
                    in the SO bitmap
    alt_so[]        Alternative SO's that can not be placed in the SO bitmap

DEPENDENCIES
  This function assumes that number of alternative SO's and the alt_so array
  is properly populated.

RETURN VALUE
  None

SIDE EFFECTS
  Any Alternative Service Option in alt_so_list which either have no Service
  Option group assigned or do not belong to the same Service Option group
  whose bitmap is being included, will be returned in alt_so_list. Total
  number of such alternative SO's will be returned as num_alt_so.

===========================================================================*/

void snm_convert_alt_so_bitmap
(
  word so_num,
  byte *num_alt_so,
  word alt_so[],
  byte *so_bitmap_ind,
  byte *so_group_num,
  uint16 *so_bitmap
);

/*===========================================================================

FUNCTION snm_get_so_grp_num

DESCRIPTION
  This function returns the group number of a given SO.

DEPENDENCIES
  SO must be registered with SNM.

RETURN VALUE
  Group number for the SO if SO is found, else 0.

SIDE EFFECTS
  None
===========================================================================*/

uint8 snm_get_so_grp_num
(
  word so
);

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION snm_eval_so

DESCRIPTION
  This function evaluates the SO specified in the function parameter and returns
  the same SO if it is supported, otherwise a alternative SO is returned.  If there
  is no alternative, CAI_SO_REJ is returned.  Unlike snm_eval_paged_so, the function
  does not affect the internal state of SNM and thus can be called at any time.
  Note 1: If the SO is not defined in SNM, no attempt is made to look for its
  occurrence in the compatible SO lists.
  Note 2: If SO is proposed from alternate SO list and if valid pointer to the index
  of alternate SO list is passed as input pointer, the function will update the content
  of pointer.

DEPENDENCIES
  None.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.
===========================================================================*/

uint16 snm_eval_so
(
  uint16 so,
    /* The service option number to be evaluated */
  uint16 *alt_so_index
    /* Index of the alternate SO */
);

/*===========================================================================

FUNCTION snm_is2000_scr_init

DESCRIPTION
  Initialize the IS2000 fields in a SCR.  Useful when dealing with P_REV lower
  than 6 service configurations.

DEPENDENCIES
  Input configuration must have valid non-IS2000 fields filled.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_is2000_scr_init
(
  caii_srv_cfg_type  *cfg_ptr
    /* Pointer to configuration to be initialized */
);

/*===========================================================================

FUNCTION snm_is2000_nnscr_init

DESCRIPTION
  Initialize the fields in a NNSCR.  Useful when dealing with P_REV lower
  than 6 service configurations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_is2000_nnscr_init
(
  caii_non_neg_srv_cfg_type *nnscr /* Pointer to nnscr to be initialized */
);

/*===========================================================================

FUNCTION snm_is2000_nnscr_lpm_default

DESCRIPTION
  Initialize the LPM fields in a NNSCR to default values.
  sr_id_list[] specifies the sr_id values to use in the default table,
  fwd_chan_mask and rev_chan_mask specifies the channel resources in effect
  and rc_class_2 specifies the current RC class.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_is2000_nnscr_lpm_default
(
  caii_non_neg_srv_cfg_type *nnscr,
    /* Pointer to nnscr to be initialized */
  byte sr_id_count,
  byte sr_id_list[],
  mcc_fwd_chan_mask_type fwd_chan_mask,
  mcc_rev_chan_mask_type rev_chan_mask,
  boolean rc_class_2
);

/*===========================================================================

FUNCTION snm_update_fpc_pri_chan

DESCRIPTION
  Updates the value of FPC_PRI_CHAN in non-negotiable SCR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_update_fpc_pri_chan
(
  uint8 fpc_pri_chan /* New value of FPC_PRI_CHAN */
);

/*===========================================================================

FUNCTION snm_get_rlp_cap_blob

DESCRIPTION
  Retrieve RLP_BLOB to be used for Capability Info Record.

DEPENDENCIES
  SNP has to be initialized.

RETURN VALUE                      m  Length of the RLP_CAP_BLOB

SIDE EFFECTS
  none
===========================================================================*/

uint8 snm_get_rlp_cap_blob
(
  uint8 *rlp_blob
);

#if defined(FEATURE_IS2000_REL_C)
/*===========================================================================

FUNCTION snm_populate_restore_config

DESCRIPTION
  This function populates the service configuration to be restored to
  pending_cfg.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_populate_restore_config
(
  void
);
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

FUNCTION SNM_UPDATE_NN_SCR_FROM_PCNM

DESCRIPTION
  This function updates the Non neg SCR based on the info from the
  Power Control Message.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Also sets the status field in the event report (e_ptr->status).
===========================================================================*/
void snm_update_nn_scr_from_pcnm
(
  const caii_pwr_ctrl_type* pcnm
);

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION SNM_UPDATE_NN_SCR_FROM_RCP_INFO

DESCRIPTION
  This function updates the FPC_MODE in Non neg SCR based on the info from
  the Radio Configuration Parameters Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_update_nn_scr_from_rcp_info
(
  void
);
#endif /* FEATURE_IS2000_1X_ADV */

#endif /*SNM_I_H*/
