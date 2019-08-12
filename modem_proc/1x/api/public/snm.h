#ifndef SNM_H
#define SNM_H
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/snm.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/10   ag      Removed featurization for RCLIST_MAX.
01/11/10   ag      Increased RCLIST_MAX to 6 for 1x Advanced.
10/05/09   jj      Meta-Comments cleanup for API's exported to DS.
08/31/09   adw     Removed unnecessary customer.h and target.h includes.
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
06/03/09   jj      Updaed few CMI Phase2 change
04/01/09   adw     Added temporary CMI-lite featurization.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_CP.07.02.09
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
#include "comdef.h"
#include "cai.h"
#include "caii.h"
#include "mcc.h"

/*===========================================================================

                      SOME CONSTANTS USED IN THIS FILE

===========================================================================*/
#define MAX_ALT_SO        CAI_AC_ALT_SO_MAX   /* Maximum number of
                                                 alternate service options */
#define MAX_COMP_SO       2   /* Maximum number of compatible service options */

/* Maximum number of RCs in negotiation parameters */
#define RCLIST_MAX       6

/* Maximum number of supported channels in channel list */
#define MAX_CHN          3

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/
typedef enum
{
  SNM_VALID,                 /* The fields were completely valid */
  SNM_INVALID_MSG_TYP,       /* Reject: Bad Message Type */
  SNM_INVALID_FIELD_VALUE,   /* Reject: Invalid Field Value */
  SNM_MESSAGE_NOT_SUPPORTED, /* Reject: Message not Supported */
  SNM_UNSPEC                 /* Reject: Unspecified reason */
} snm_socm_return_type;

typedef enum
{
  SNM_FAILURE = FALSE,
  SNM_SUCCESS
} snm_status_type;

typedef uint8   snm_fwd_rev_type;

/* Type to indicate active channel configurations */
typedef uint8   snm_chan_mask_type;

/* Type to define the default configuration for a service option */
typedef struct
{
  struct
  {
    word mux;                   /* Multiplex option */
    cai_rate_set_type rate_set;
    byte tx_rates;              /* Transmission rates used */
    cai_traffic_type_type allowed_traffic_type;
  } forward, reverse;
} snm_config_type;

/* Type to indicate forward and reverse multiplex options */
typedef struct
{
  word forward; /* Forward link multiplex option */
  word reverse; /* Reverse link multiplex option */
} snm_mux_pair_type;

/* Type to indicate forward and reverse rate set */
typedef struct
{
  cai_rate_set_type forward; /* Forward link rate set */
  cai_rate_set_type reverse; /* Reverse link rate set */
} snm_rate_set_pair_type;


/* RC list type */
typedef struct
{
  byte num_of_rc;
  cai_radio_config_type list[RCLIST_MAX];
} snm_rc_list_type;

/* SCH mux Option list type */
typedef struct
{
  byte num_mos;
  word list[CAI_MAX_MULT_OPT_SCH];
} snm_sch_mo_list_type;

/* Type to define the negotiation parameters of a service option */
typedef struct
{
  byte min_p_rev;                       /* Minimum P_REV to accept SO  */
  word alternate_so[MAX_ALT_SO];        /* Alternate to suggest if SO not
                                           supported. Terminated by CAI_SO_NULL. */
  word compatible_so[MAX_COMP_SO];
  snm_mux_pair_type max_mux_to_use;
  snm_rc_list_type fwd_rc;              /* List of forward RCs supported */
  snm_rc_list_type rev_rc;              /* List of reverse RCs supported */
  snm_sch_mo_list_type fwd_sch_mo_list; /* List of F_SCH multiplex options supported */
  snm_sch_mo_list_type rev_sch_mo_list; /* List of R_SCH multiplex options supported */
  snm_chan_mask_type chan_list[MAX_CHN];/* List of channels supported */

  byte so_group;                        /* Indicates the Service Option Group this particular
                                           SO belongs to. */
  byte bitmap_pos;                      /* SO bitmask in the group */

} snm_neg_par_type;

/*===========================================================================

FUNCTION snm_add_so

DESCRIPTION
  This function adds a service option descriptor to SNP.

DEPENDENCIES
  SNP must be initialized (snm_initialize_snm does it).

RETURN VALUE
  SNM_SUCCESS if the SO is added successfully;otherwise if there is no more
  room in SNP, or if a same SO already exists, it returns SNM_FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_add_so
(
  word                   so_number,
  snm_neg_par_type       *neg_par,
  const snm_config_type  *default_config
);

/*===========================================================================

FUNCTION snm_set_sch_mux_list

DESCRIPTION
  This function sets the SCH mux list for the given service option number.

DEPENDENCIES
  SNP must be initialized (snm_initialize_snm does it).

RETURN VALUE
  SUCCESS if the mux list was added successfully; otherwise if
  the SO doesn't exists, it returns FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_set_sch_mux_list
(
  word                 so_number, /* Service option number */
  snm_fwd_rev_type     fwd_rev,   /* Direction of SCH */
  snm_sch_mo_list_type *mo_list   /* Mux options to set */
);


/*===========================================================================

FUNCTION snm_set_page_response_so

DESCRIPTION
  This function changes the page_response_so member of a SO descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if page response changed successfully; otherwise if the SO is not
  found in SNP, it returns SNM_FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_set_page_response_so
(
  word so_number,  /* Service option number */
  word response_so
);

/*===========================================================================

FUNCTION snm_enable_so

DESCRIPTION
  This function enables a service option.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if SO successfully enabled; otherwise if the SO is not found in SNP,
  SNM_FAILURE is returned.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_enable_so
(
  word so_number
);

/*===========================================================================

FUNCTION snm_disable_so

DESCRIPTION
  This function disables a service option.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if SO successfully disabled; otherwise if the SO is not found in SNP,
  SNM_FAILURE is returned.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_disable_so
(
  word so_number
);

/*===========================================================================

FUNCTION snm_get_current_config

DESCRIPTION
  This function returns the current negotiable part of service configuration.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if valid configuration has been negotiated.  SNM_FAILURE otherwise.
  If successful, pointer to current config is returned in function parameter.

SIDE EFFECTS
  None
===========================================================================*/

const caii_srv_cfg_type* snm_get_current_config
(
  void
);

/*===========================================================================

FUNCTION snm_set_max_for_sups

DESCRIPTION
  This function changes the maximum number of forward supplementals to be used
  with a service option.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if serivce option exists and number of supplementals are valid;
  SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_set_max_for_sups
(
  word  service_option,
  byte  num_sup
);

/*===========================================================================

FUNCTION snm_set_cont_msg_handler

DESCRIPTION
  This function sets the Service Option Control Message handler function
  pointer.

DEPENDENCIES
  snp.total_so must be less than MAX_WORD value.

RETURN VALUE
  SNM_SUCCESS if serivce option exists,
  SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

typedef snm_socm_return_type (*cont_msg_handler_type) (word length,
                                                       byte* field,
                                                       word serv_opt);
/*~ PARAM IN field VARRAY LENGTH length */

snm_status_type snm_set_cont_msg_handler
(
  word so,
  cont_msg_handler_type handler_ptr
);


#endif /* SNM_H */
