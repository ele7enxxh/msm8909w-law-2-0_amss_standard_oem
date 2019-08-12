/*===========================================================================

              S Y S T E M   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions that are shared between Call Manager,
  Call Manager clients and the protocol stacks.


  Organization of the file:

    The file is divided into multiple sections. Each section contains
    validation routines for a particular entity.  The following entities
    are defined in this file:

        Service Indicators
        System Identifiers
        Network Lists
        Mobility Management Information

EXTERNALIZED FUNCTIONS
  All functions are externalized except for sys_plmn_id_digit_is_valid.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None


Copyright (c) 2002 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/sys.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/10/13   aa      Added function to check MNC digits
05/21/13   fj      Buffer the LTE PWR_SAVE status on hybr_2 stack. 
09/12/12   skk     Do not send internal service lost or limited service during uptime.
08/24/11   rj      MNC value incorrectly calculated in sys_plmn_get_mcc_mnc2()
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/17/10   am      Removing header files added for MOB_CUTE
10/16/10   am      Fixing cute compilation
08/30/10   aj      update to MMSS utility APIs
03/28/10   aj      Add sys_plmn_get_mcc_mnc2 
03/22/10   aj      KW fixes
03/21/10   aj      MMSS support
12/03/09   rm      Correct the order of input parameters in 
                   sys_plmn_list_found_plmn
10/29/09   sg      Add non-demand paging macro to function that are not to be 
                   demand-paged.
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
02/11/09   ks      Validate spare_bits field in the mm_information to indicate 
                   no. of spare bits in end octet of n/w names(full/short)
08/07/07   ic      Lint clean up
11/15/06   pk      Added support for wild card MNC for GW systems
03/10/06   pk      Modified sys_sys_id_match to check for MCC and IMSI_11_12
                   for IS 95 systems
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/18/05   jqi     Replaced cmdbg.h with cmidbg.h file to reduce header file 
                   dependency.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
08/26/05   up      Updated sys_plmn_match to compare NA PLMNs of 2 digit MNCs 
                   when 3rd digit MNC is transmitted as '0'/'F'.
05/12/05   ic      Removed bcmcs_srv_status_is_srv()
04/04/05   ic      Removed unnecessary Lint comments 
03/22/05   up      Function sys_mcc_same_country is added to extend MCC
                   matching for NA MCC range 310-316.
03/09/05   kwa     Replaced code within sys_plmn_match with the code in
                   sys_hplmn_match.  Fixed a bug in sys_plmn_match so it
                   properly compares two PLMN IDs with 2 digit MNCs within
                   the North American country codes.
08/10/04   kwa     Added sys_hplmn_match.  Moved the functionality added to
                   sys_plmn_match in the previous update to sys_hplmn_match.
                   Removed the North American HPLMN comparison requirements
                   from sys_plmn_match.
06/24/04   kwa     Updated the function sys_plmn_match to support the PLMN
                   matching requirements for North American GSM networks as
                   defined in Annex A of 3GPP TS 23.122.
03/15/04   ws      Fixed bug with system id comparison.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/06/04   ws      Initial jaguar/mainline merge.
11/06/03   sj      Updated sys_srv_status_is_srv() for limited_regional srv.
10/24/03   ic      Changes to support common sys.c across targets
05/05/03   kwa     Updated the logic in sys_mm_information_is_valid for
                   determining if an sys_mm_information_s_type is valid to
                   account for optional elements within the structure.
                   Modified several functions to pass pointers rather than
                   structures to minimize stack utilization.
05/05/03   vt      Removed #if 0 stuff.
04/28/03   AT      Lint cleanup.
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Modified sys_user_pref_plmn_list_info_is_valid() to only
                   validate plmn.
11/22/02   MKS     Added sys_network_name_coding_scheme_is_valid() to check
                   if a valid coding scheme is used to encode short name or
                   full name of the network.
04/01/02   PRK     Initial version
===========================================================================*/

#include "mmcp_variation.h"
#include "sys.h"
#include "sys_v.h"
#include "string.h"
#include "cmidbg.h" 
#include "cmutil.h"
#include "mmgsdilib_common.h"

#define SYS_MNC_2_DIGITS                    0x02
#define SYS_MNC_3_DIGITS                    0x03
#define SYS_IMSI_DIGIT_1_OFFSET             0x01
#define SYS_IMSI_DIGIT_2_OFFSET             0x02
#define SYS_IMSI_LOW_DIGIT_MASK             0x0F

#define SYS_MNC_LEN_BYTE                    0x03


/*==============================================================================

                     S E R V I C E   I N D I C A T O R S

==============================================================================*/

/*===========================================================================

FUNCTION: sys_get_max_short_name_chars

DESCRIPTION
  This function returns the max value of the sys_max_short_name_chars

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA

RETURN VALUE
 max possible short name character based of definition of feature

SIDE EFFECTS
  None
===========================================================================*/
static uint8 sys_get_max_short_name_chars
(
   void
)

{
  #ifdef CM_GW_SUPPORTED
  return SYS_MAX_SHORT_NAME_CHARS;
  #else
  return SYS_MIN_SHORT_NAME_CHARS;
  #endif
} /* sys_get_max_short_name_chars() */


/*===========================================================================

FUNCTION: sys_get_max_full_name_chars

DESCRIPTION
  This function returns the max value of the sys_max_full_name_chars

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA

RETURN VALUE
 max possible full name character based of definition of feature

SIDE EFFECTS
  None
===========================================================================*/
static uint8 sys_get_max_full_name_chars
(
   void
)
{
  #ifdef CM_GW_SUPPORTED
  return SYS_MAX_FULL_NAME_CHARS;
  #else
  return SYS_MIN_FULL_NAME_CHARS;
  #endif
} /* sys_get_max_full_name_chars */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_oprt_mode_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_oprt_mode_e_type  value          Value that is to be checked
                                       for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_oprt_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_oprt_mode_is_valid
(
  sys_oprt_mode_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_OPRT_MODE_NONE &&
                    value < SYS_OPRT_MODE_MAX    );

  return valid;
} /* sys_oprt_mode_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_sys_mode_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_sys_mode_e_type  value          Value that is to be checked
                                      for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_sys_mode_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_sys_mode_is_valid
(
  sys_sys_mode_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SYS_MODE_NONE &&
                    value < SYS_SYS_MODE_MAX    );

  return valid;
} /* sys_sys_mode_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_band_class_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_band_class_e_type  value        Value that is to be checked
                                      for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_band_class_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_band_class_is_valid
(
  sys_band_class_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_BAND_CLASS_NONE &&
                    value < SYS_BAND_CLASS_MAX    );

  return valid;
} /* sys_band_class_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_block_or_system_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_blksys_e_type  value            Value that is to be checked
                                      for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_blksys_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_block_or_system_is_valid
(
  sys_blksys_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_BLKSYS_NONE &&
                    value < SYS_BLKSYS_MAX    );

  return valid;
} /* sys_blksys_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_status_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_srv_status_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_srv_status_is_valid
(
  sys_srv_status_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SRV_STATUS_NONE &&
                    value < SYS_SRV_STATUS_MAX    );

  return valid;
} /* sys_srv_status_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_status_is_srv

DESCRIPTION

  Function that verifies that the input value is indicates any service.

PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the value indicates service( TRUE)
           or no service (FALSE).

==============================================================================*/
boolean sys_srv_status_is_srv
(
  sys_srv_status_e_type  value
)
{

  if( value == SYS_SRV_STATUS_NO_SRV  ||
      value == SYS_SRV_STATUS_NO_SRV_INTERNAL  ||
      value == SYS_SRV_STATUS_PWR_SAVE ||
      value == SYS_SRV_STATUS_PWR_SAVE_INTERNAL )
  {
    return FALSE;
  }


  return TRUE;

} /* sys_srv_status_is_srv */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_status_is_full_srv

DESCRIPTION

  Function that verifies that the input value is indicates full service.

PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the value indicates full service( TRUE)
           or other service (FALSE).

==============================================================================*/
boolean sys_srv_status_is_full_srv
(
  sys_srv_status_e_type  value
)
{

  if( value == SYS_SRV_STATUS_SRV )
  {
    return TRUE;
  }


  return FALSE;

} /* sys_srv_status_is_full_srv */

/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_status_is_full_or_lmt_reg_srv

@DESCRIPTION

  Function that verifies that the input value is indicates full service or limited regional service.

@PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the value indicates full or limited regional service( TRUE)
           or no full service (FALSE).

==============================================================================*/
extern boolean sys_srv_status_is_full_or_lmt_reg_srv
(
  sys_srv_status_e_type  value
)
{

  if( value == SYS_SRV_STATUS_SRV  ||
     value == SYS_SRV_STATUS_LIMITED_REGIONAL)
  {
    return TRUE;
  }


  return FALSE;

} /* sys_srv_status_is_full_srv */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_status_is_lmtd_srv

DESCRIPTION

  Function that verifies that the input value is indicates limited service.

PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the value indicates limited  service( TRUE)
           or other service (FALSE).

==============================================================================*/
boolean sys_srv_status_is_lmtd_srv
(
  sys_srv_status_e_type  value
)
{

  if( value == SYS_SRV_STATUS_LIMITED  ||
      value == SYS_SRV_STATUS_LIMITED_INTERNAL )
  {
    return TRUE;
  }


  return FALSE;

} /* sys_srv_status_is_lmtd_srv */



/**==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

@FUNCTION NAME

  sys_srv_status_is_lmtd_or_lmtd_reg_srv

@DESCRIPTION

  Function that verifies that the input value is indicates full service or limited regional service.

@PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

@RETURN VALUE

  boolean  Flag indicating whether the value indicates limited or limited regional service( TRUE)
           or FALSE in all other cases

==============================================================================*/
extern boolean sys_srv_status_is_lmtd_or_lmtd_reg_srv
(
  sys_srv_status_e_type  value
)
{

  if( value == SYS_SRV_STATUS_LIMITED  ||
     value == SYS_SRV_STATUS_LIMITED_REGIONAL)
  {
    return TRUE;
  }


  return FALSE;

} /* sys_srv_status_is_full_srv */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_domain_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_srv_domain_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_service_domain_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_srv_domain_is_valid
(
  sys_srv_domain_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SRV_DOMAIN_NONE &&
                    value < SYS_SRV_DOMAIN_MAX    );

  return valid;
} /* sys_srv_domain_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_capability_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_srv_capability_e_type   value       Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_srv_capability_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_srv_capability_is_valid
(
  sys_srv_capability_e_type value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SRV_CAPABILITY_NONE &&
                    value < SYS_SRV_CAPABILITY_MAX    );

  return valid;
} /* sys_srv_capability_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_roam_status_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_roam_status_e_type value                   Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_roam_status_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_roam_status_is_valid
(
  sys_roam_status_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_ROAM_STATUS_NONE &&
                    value < SYS_ROAM_STATUS_MAX    );

  return valid;
} /* sys_roam_status_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_radio_access_tech_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_radio_access_tech_e_type value             Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_radio_access_tech_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_radio_access_tech_is_valid
(
  sys_radio_access_tech_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_RAT_NONE &&
                    value < SYS_RAT_MAX    );

  return valid;
} /* sys_radio_access_tech_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_sim_state_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_sim_state_e_type value                     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_sim_state_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_sim_state_is_valid
(
  sys_sim_state_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_SIM_STATE_NONE &&
                    value < SYS_SIM_STATE_MAX    );

  return valid;
} /* sys_sim_state_is_valid */


/*==============================================================================

                     S Y S T E M   I D E N T I F I E R

==============================================================================*/


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_id_digit_is_valid

DESCRIPTION

  Function that checks if the input digit is between 0 and 9 or equal to 0xF.

PARAMETERS

  byte       digit                     Digit to be checked for validity.

RETURN VALUE

  boolean  Flag indicating if the digit is valid (TRUE) or not (FALSE).

==============================================================================*/
static boolean sys_plmn_id_digit_is_valid
(
  byte digit
)
{
  return (boolean)( (digit <= 0x9) || (digit == 0xF) );
} /* sys_plmn_id_digit_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_get_digits

DESCRIPTION

  Function that returns the each of the digits in the MCC and MNC contained in
  a PLMN ID as integer values.

PARAMETERS

  sys_plmn_id_s_type                   plmn provided as input from which
            plmn                       to read the MCC and MNC.
  uint32*   mcc_digit_1_ptr            First digit of the MCC.
  uint32*   mcc_digit_2_ptr            Second digit of the MCC.
  uint32*   mcc_digit_3_ptr            Third digit of the MCC.
  uint32*   mnc_digit_1_ptr            First digit of the MNC.
  uint32*   mnc_digit_2_ptr            Second digit of the MNC.
  uint32*   mnc_digit_3_ptr            Third digit of the MNC.

RETURN VALUE

  void

==============================================================================*/
void sys_plmn_get_digits
(
  sys_plmn_id_s_type plmn_id,
  uint32*            mcc_digit_1_ptr,
  uint32*            mcc_digit_2_ptr,
  uint32*            mcc_digit_3_ptr,
  uint32*            mnc_digit_1_ptr,
  uint32*            mnc_digit_2_ptr,
  uint32*            mnc_digit_3_ptr
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(mcc_digit_1_ptr  != NULL); 
  CM_ASSERT(mcc_digit_2_ptr  != NULL); 
  CM_ASSERT(mcc_digit_3_ptr  != NULL); 
  CM_ASSERT(mnc_digit_1_ptr  != NULL); 
  CM_ASSERT(mnc_digit_2_ptr  != NULL); 
  CM_ASSERT(mnc_digit_3_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *mcc_digit_1_ptr = plmn_id.identity[0] & 0x0F;
  *mcc_digit_2_ptr = plmn_id.identity[0] / 0x10;
  *mcc_digit_3_ptr = plmn_id.identity[1] & 0x0F;

  *mnc_digit_1_ptr = plmn_id.identity[2] & 0x0F;
  *mnc_digit_2_ptr = plmn_id.identity[2] / 0x10;
  *mnc_digit_3_ptr = plmn_id.identity[1] / 0x10;

}



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_get_mcc_mnc

DESCRIPTION

  Function that returns the MCC and MNC contained in a PLMN ID as integer
  values.

PARAMETERS

  sys_plmn_id_s_type                         plmn provided as input from which
                 plmn                        to read the MCC and MNC.
  boolean*       plmn_is_undefined_ptr       Flag indicating that the input plmn
                                             contains a defined MCC and MNC
                                             (FALSE) or was set to 0xFFFFFF
                                             (TRUE).
  boolean*       mnc_includes_PCS_digit_ptr  Flag indicating whether the MNC
                                             included the third PCS digit (TRUE)
                                             or not (FALSE)
  sys_mcc_type*  mcc_ptr                     MCC converted from Binary Coded
                                             Decimal Digits in the plmn to an
                                             integer.
  sys_mnc_ptr*   mnc_ptr                     MNC converted from Binary Coded
                                             Decimal Digits in the plmn to an
                                             integer.

RETURN VALUE

  void

==============================================================================*/
void sys_plmn_get_mcc_mnc
(
  sys_plmn_id_s_type  plmn,
  boolean*            plmn_is_undefined_ptr,
  boolean*            mnc_includes_pcs_digit_ptr,
  sys_mcc_type*       mcc_ptr,
  sys_mnc_type*       mnc_ptr
)
{

  sys_mcc_type        mcc_digit_1;
  sys_mcc_type        mcc_digit_2;
  sys_mcc_type        mcc_digit_3;

  sys_mnc_type        mnc_digit_1;
  sys_mnc_type        mnc_digit_2;
  sys_mnc_type        mnc_digit_3;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(plmn_is_undefined_ptr       != NULL); 
  CM_ASSERT(mnc_includes_pcs_digit_ptr  != NULL); 
  CM_ASSERT(mcc_ptr  != NULL); 
  CM_ASSERT(mnc_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (sys_plmn_id_is_undefined (plmn))
  {
    *plmn_is_undefined_ptr = TRUE;
    *mcc_ptr               = 0;
    *mnc_ptr               = 0;
  }
  else
  {
    *plmn_is_undefined_ptr = FALSE;

    mcc_digit_1 = plmn.identity[0] & 0x0F;
    mcc_digit_2 = plmn.identity[0] / 0x10;
    mcc_digit_3 = plmn.identity[1] & 0x0F;

    mnc_digit_1 = plmn.identity[2] & 0x0F;
    mnc_digit_2 = plmn.identity[2] / 0x10;
    mnc_digit_3 = plmn.identity[1] / 0x10;

    *mcc_ptr = 100 * mcc_digit_1 + 10 * mcc_digit_2 + mcc_digit_3;

    if (mnc_digit_3 == 0xF)
    {
      *mnc_includes_pcs_digit_ptr = FALSE;
      *mnc_ptr = 10 * mnc_digit_1 + mnc_digit_2;
    }
    else
    {
      *mnc_includes_pcs_digit_ptr = TRUE;
      *mnc_ptr = 100 * mnc_digit_1 + 10 * mnc_digit_2 + mnc_digit_3;
    }
  }

} /* sys_plmn_get_mcc_mnc */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_get_mcc_mnc2

DESCRIPTION

  Function that returns the MCC and MNC contined in a PLMN ID as integer
  values. Same as sys_plmn_get_mcc_mnc() but with support for wildcard MNC

PARAMETERS

  sys_plmn_id_s_type                         plmn provided as input from which
                 plmn                        to read the MCC and MNC.
  boolean*       plmn_is_undefined_ptr       Flag indicating that the input plmn
                                             contains a defined MCC and MNC
                                             (FALSE) or was set to 0xFFFFFF
                                             (TRUE).
  boolean*       mnc_includes_PCS_digit_ptr  Flag indicating whether the MNC
                                             included the third PCS digit (TRUE)
                                             or not (FALSE)
  sys_mcc_type*  mcc_ptr                     MCC converted from Binary Coded
                                             Decimal Digits in the plmn to an
                                             integer.
  sys_mnc_ptr*   mnc_ptr                     MNC converted from Binary Coded
                                             Decimal Digits in the plmn to an
                                             integer.

RETURN VALUE

  void

==============================================================================*/
void sys_plmn_get_mcc_mnc2
(
  sys_plmn_id_s_type  plmn,
  boolean*            plmn_is_undefined_ptr,
  boolean*            mnc_includes_pcs_digit_ptr,
  sys_mcc_type*       mcc_ptr,
  sys_mnc_type*       mnc_ptr
)
{

  sys_mcc_type        mcc_digit_1;
  sys_mcc_type        mcc_digit_2;
  sys_mcc_type        mcc_digit_3;

  sys_mnc_type        mnc_digit_1;
  sys_mnc_type        mnc_digit_2;
  sys_mnc_type        mnc_digit_3;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(plmn_is_undefined_ptr       != NULL); 
  CM_ASSERT(mnc_includes_pcs_digit_ptr  != NULL); 
  CM_ASSERT(mcc_ptr  != NULL); 
  CM_ASSERT(mnc_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (sys_plmn_id_is_undefined (plmn))
  {
    *plmn_is_undefined_ptr = TRUE;
    *mcc_ptr               = 0;
    *mnc_ptr               = 0;
  }
  else
  {
    *plmn_is_undefined_ptr = FALSE;

    mcc_digit_1 = plmn.identity[0] & 0x0F;
    mcc_digit_2 = plmn.identity[0] / 0x10;
    mcc_digit_3 = plmn.identity[1] & 0x0F;

    mnc_digit_1 = plmn.identity[2] & 0x0F;
    mnc_digit_2 = plmn.identity[2] / 0x10;
    mnc_digit_3 = plmn.identity[1] / 0x10;

    *mcc_ptr = 100 * mcc_digit_1 + 10 * mcc_digit_2 + mcc_digit_3;
    
    if(plmn.identity[2] == 0xFF)
    {
      *mnc_ptr = SYS_WILDCARD_MNC;
      return;
    }

    if (mnc_digit_3 == 0xF)
    {
      *mnc_includes_pcs_digit_ptr = FALSE;
      *mnc_ptr = 10 * mnc_digit_1 + mnc_digit_2;
    }
    else
    {
      *mnc_includes_pcs_digit_ptr = TRUE;
      *mnc_ptr = 100 * mnc_digit_1 + 10 * mnc_digit_2 + mnc_digit_3;
    }
  }

} /* sys_plmn_get_mcc_mnc2 */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_set_mcc_mnc

DESCRIPTION

  Function that fills a PLMN ID with the MCC and MNC provided as integer values.

PARAMETERS

  boolean           mnc_includes_pcs_digit  Flag indicating whether the MNC
                                            includes the third PCS digit (TRUE)
                                            or not (FALSE)
  sys_mcc_type      mcc                     MCC as an integer value (0 - 999).
  sys_mnc_type      mnc                     MCC as an integer value (0 - 99 or
                                            0 - 999 if the MCS digit is
                                            included).
  sys_plmn_id_type* plmn_ptr                Pointer to the plmn to populate
                                            input from which to read the MCC and
                                            MNC.

RETURN VALUE

  boolean  Flag indicating whether or not the plmn_ptr was successfully filled
           with values provided for the MCC and MNC.  If TRUE then *plmn_ptr
           was successfully filled.  If FALSE then the input parameters were
           determined to be out of range and *plmn_ptr was set to 0xFFFFFF.

==============================================================================*/
boolean sys_plmn_set_mcc_mnc
(
  boolean               mnc_includes_pcs_digit,
  sys_mcc_type          mcc,
  sys_mnc_type          mnc,
  sys_plmn_id_s_type*   plmn_ptr
)

{
  sys_mcc_type          mcc_digit_1;
  sys_mcc_type          mcc_digit_2;
  sys_mcc_type          mcc_digit_3;

  sys_mnc_type          mnc_digit_1;
  sys_mnc_type          mnc_digit_2;
  sys_mnc_type          mnc_digit_3;

  if ( (mcc > 999) ||
       (mnc_includes_pcs_digit && mnc > 999) ||
       (!mnc_includes_pcs_digit && mnc > 99) )
  {
    sys_plmn_undefine_plmn_id (plmn_ptr);
    return FALSE;
  }

  plmn_ptr->identity[0] = 0x00;
  plmn_ptr->identity[1] = 0x00;
  plmn_ptr->identity[2] = 0x00;

  mcc_digit_1 = mcc/100;
  mcc_digit_2 = (mcc - 100 * mcc_digit_1) / 10;
  mcc_digit_3 = mcc - 100 * mcc_digit_1 - 10 * mcc_digit_2; /*lint !e834 */

  if (mnc_includes_pcs_digit)
  {
    mnc_digit_1 = mnc/100;
    mnc_digit_2 = (mnc - 100 * mnc_digit_1) / 10;
    mnc_digit_3 = mnc - 100 * mnc_digit_1 - 10 * mnc_digit_2; /*lint !e834 */
  }
  else
  {
    mnc_digit_1 = mnc/10;
    mnc_digit_2 = mnc - 10 * mnc_digit_1;
    mnc_digit_3 = 0xF;
  }

  plmn_ptr->identity[0] = (byte)((mcc_digit_2 << 4) + mcc_digit_1);
  plmn_ptr->identity[1] = (byte)((mnc_digit_3 << 4) + mcc_digit_3);
  plmn_ptr->identity[2] = (byte)((mnc_digit_2 << 4) + mnc_digit_1);

  return TRUE;
} /* sys_plmn_set_mcc_mnc */


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_set_mcc_mnc2

DESCRIPTION

  Function that fills a PLMN ID with the MCC and MNC provided as integer values
  or wildcard values . MCC wildcard = MNC wildcard = 0xFFF

PARAMETERS

  boolean           mnc_includes_pcs_digit  Flag indicating whether the MNC
                                            includes the third PCS digit (TRUE)
                                            or not (FALSE)
  sys_mcc_type      mcc                     MCC as an integer value (0 - 999).
  sys_mnc_type      mnc                     MCC as an integer value (0 - 99 or
                                            0 - 999 if the MCS digit is
                                            included).or wildcard 0xFF or 0xFFF
  sys_plmn_id_type* plmn_ptr                Pointer to the plmn to populate
                                            input from which to read the MCC and
                                            MNC.

RETURN VALUE

  boolean  Flag indicating whether or not the plmn_ptr was successfully filled
           with values provided for the MCC and MNC.  If TRUE then *plmn_ptr
           was successfully filled.  If FALSE then the input parameters were
           determined to be out of range and *plmn_ptr was set to 0xFFFFFF.

==============================================================================*/
boolean sys_plmn_set_mcc_mnc2
(
  boolean               mnc_includes_pcs_digit,
  sys_mcc_type          mcc,
  sys_mnc_type          mnc,
  sys_plmn_id_s_type*   plmn_ptr
)

{
  sys_mcc_type          mcc_digit_1;
  sys_mcc_type          mcc_digit_2;
  sys_mcc_type          mcc_digit_3;

  sys_mnc_type          mnc_digit_1;
  sys_mnc_type          mnc_digit_2;
  sys_mnc_type          mnc_digit_3;


  if ( (mcc > 999) ||
       (mnc_includes_pcs_digit && mnc > 999 && mnc != 0xFFF) ||
       (!mnc_includes_pcs_digit && mnc > 99 && mnc != 0xFF) )
  {
    sys_plmn_undefine_plmn_id (plmn_ptr);
    return FALSE;
  }

  plmn_ptr->identity[0] = 0x00;
  plmn_ptr->identity[1] = 0x00;
  plmn_ptr->identity[2] = 0x00;

  mcc_digit_1 = mcc/100;
  mcc_digit_2 = (mcc - 100 * mcc_digit_1) / 10;
  mcc_digit_3 = mcc - 100 * mcc_digit_1 - 10 * mcc_digit_2; /*lint !e834 */

  if(mnc == 0xFFF || mnc == 0xFF)
  {
    mnc_digit_1 = 0xF;
    mnc_digit_2 = 0xF;
    mnc_digit_3 = 0xF;
  }
  else if (mnc_includes_pcs_digit)
  {
    mnc_digit_1 = mnc/100;
    mnc_digit_2 = (mnc - 100 * mnc_digit_1) / 10;
    mnc_digit_3 = mnc - 100 * mnc_digit_1 - 10 * mnc_digit_2; /*lint !e834 */
  }
  else
  {
    mnc_digit_1 = mnc/10;
    mnc_digit_2 = mnc - 10 * mnc_digit_1;
    mnc_digit_3 = 0xF;
  }

  plmn_ptr->identity[0] = (byte)((mcc_digit_2 << 4) + mcc_digit_1);
  plmn_ptr->identity[1] = (byte)((mnc_digit_3 << 4) + mcc_digit_3);
  plmn_ptr->identity[2] = (byte)((mnc_digit_2 << 4) + mnc_digit_1);

  return TRUE;
} /* sys_plmn_set_mcc_mnc2 */




/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_get_mcc

DESCRIPTION

  Function that returns the MCC contained in a PLMN ID as integer
  value.
  If plmn is undefined 0 will be put in mcc ptr

PARAMETERS

  sys_plmn_id_s_type                         plmn provided as input from which
                 plmn                        to read the MCC and MNC.

  sys_mcc_type*  mcc_ptr                     MCC converted from Binary Coded
                                             Decimal Digits in the plmn to an
                                             integer.

RETURN VALUE

  void

==============================================================================*/
void sys_plmn_get_mcc
(
  sys_plmn_id_s_type  plmn,
  sys_mcc_type*       mcc_ptr
)
{

  sys_mcc_type        mcc_digit_1;
  sys_mcc_type        mcc_digit_2;
  sys_mcc_type        mcc_digit_3;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(mcc_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (sys_plmn_id_is_undefined (plmn))
  {
    *mcc_ptr               = 0;
  }
  else
  {

    mcc_digit_1 = plmn.identity[0] & 0x0F;
    mcc_digit_2 = plmn.identity[0] / 0x10;
    mcc_digit_3 = plmn.identity[1] & 0x0F;


    *mcc_ptr = 100 * mcc_digit_1 + 10 * mcc_digit_2 + mcc_digit_3;
  }

} /* sys_plmn_get_mcc_mnc */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_add_plmn_to_list

DESCRIPTION


PARAMETERS



RETURN VALUE

 None

==============================================================================*/
void sys_add_plmn_to_list
(
  sys_plmn_rat_category_list_s_type * plmn_list, 
  const sys_plmn_id_s_type          * plmn_ptr
)

{
  if(plmn_list == NULL          ||
     plmn_ptr  == NULL          ||
     plmn_list->length   >= SYS_MMSS_PLMN_LIST_MAX_LENGTH
    )
  {
    return;
  }
  plmn_list->info[plmn_list->length].plmn = *plmn_ptr;
  plmn_list->length++;

} /* sys_add_plmn_to_list */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_id_is_valid

DESCRIPTION

  Function that checks if each of the digits in the input plmn are between 0
  and 9 or equal to 0xF.

PARAMETERS

  sys_plmn_id_s_type  plmn                PLMN ID to validate.

RETURN VALUE

  boolean  Flag indicating if the plmn is valid (TRUE) or not (FALSE).

==============================================================================*/
boolean sys_plmn_id_is_valid
(
  sys_plmn_id_s_type  plmn
)
{
  return (boolean)( sys_plmn_id_digit_is_valid( (byte)(plmn.identity[0] & 0x0F) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[0] / 0x10) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[1] & 0x0F) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[2] & 0x0F) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[2] / 0x10) ) &&
                    sys_plmn_id_digit_is_valid( (byte)(plmn.identity[1] / 0x10) ) );
} /* sys_plmn_id_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_id_is_undefined

DESCRIPTION

  Function that checks if each of the digits in the input plmn is equal to
  0xF.  In other words, an undefined plmn contains 0xFFFFFF.

PARAMETERS

  sys_plmn_id_s_type  plmn                PLMN ID to check.

RETURN VALUE

  boolean  Flag indicating if the plmn is undefined (TRUE) or not (FALSE).

==============================================================================*/
boolean sys_plmn_id_is_undefined
(
  sys_plmn_id_s_type  plmn
)
{
  return (boolean)( (plmn.identity[0] == 0xFF) &&
                    (plmn.identity[1] == 0xFF) &&
                    (plmn.identity[2] == 0xFF));

} /* sys_plmn_id_is_undefined */
/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_id_is_null

DESCRIPTION

  Function that checks if each of the digits in the input plmn is equal to
  0x0.  In other words, a nulll plmn contains 0x00000.

PARAMETERS

  sys_plmn_id_s_type  plmn                PLMN ID to check.

RETURN VALUE

  boolean  Flag indicating if the plmn is undefined (TRUE) or not (FALSE).

==============================================================================*/
boolean sys_plmn_id_is_null
(
  sys_plmn_id_s_type  plmn
)
{
  return (boolean)( (plmn.identity[0] == 0x00) &&
                    (plmn.identity[1] == 0x00) &&
                    (plmn.identity[2] == 0x00));

} /* sys_plmn_id_is_undefined */


/*==============================================================================

FUNCTION NAME
  sys_intl_srv_status_to_actl_srv_status

DESCRIPTION

  convert internal srv status to actual srv status.

PARAMETERS

  sys_srv_status_e_type  value   Value that needs to be converted.

RETURN VALUE

  sys_srv_status_e_type  converted srv status.

==============================================================================*/
sys_srv_status_e_type sys_intl_srv_status_to_actl_srv_status
(
  sys_srv_status_e_type  value
)
{
  switch(value)
  {
  case SYS_SRV_STATUS_NO_SRV_INTERNAL:
    return SYS_SRV_STATUS_NO_SRV;

  case SYS_SRV_STATUS_LIMITED_INTERNAL:
    return SYS_SRV_STATUS_LIMITED;

  case SYS_SRV_STATUS_LIMITED_REGIONAL_INTERNAL:
    return SYS_SRV_STATUS_LIMITED_REGIONAL;

  case SYS_SRV_STATUS_PWR_SAVE_INTERNAL:
    return SYS_SRV_STATUS_PWR_SAVE;
  default:
    return value;
  }
} /* sys_intl_srv_status_to_actl_srv_status */


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_undefine_plmn_id

DESCRIPTION

  Function that sets each of the digits in the plmn_id to 0xF.  The resulting
  undefined *plmn_ptr contains 0xFFFFFF.

PARAMETERS

  sys_plmn_id_s_type*  plmn_ptr           Pointer to the PLMN ID to undefine.

RETURN VALUE

  void

==============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void sys_plmn_undefine_plmn_id
(
  sys_plmn_id_s_type*  plmn_ptr
)
{
  plmn_ptr->identity[0] = 0xFF;
  plmn_ptr->identity[1] = 0xFF;
  plmn_ptr->identity[2] = 0xFF;
} /* sys_plmn_undefine_plmn_id */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_match

DESCRIPTION

  Function that compares two PLMN IDs and determines if they are equal.
  plmn_2 is the ID of the PLMN broadcast by the network and plmn_1 is the
  ID of the PLMN that it is being compared against (e.g., the HPLMN stored
  on the SIM).  If the third MNC digit in the plmn_2 is 0xF, then the plmn_2
  is assumed to contain a two digit MNC so the third MNC digit is not compared.
  This function differs from sys_plmn_match in that it implements a further
  requirement defined in 3GPP TS23.122 Annex A for comparing the HPLMN in
  North American PCS networks.

PARAMETERS

  sys_plmn_id_s_type  plmn_1                  The first PLMN ID.
  sys_plmn_id_s_type  plmn_2                  The second PLMN ID.

RETURN VALUE

  boolean  Flag indicating whether the two PLMN IDs are equal (TRUE) or
  not (FALSE).

==============================================================================*/
boolean sys_plmn_match
(
  sys_plmn_id_s_type  plmn_1,
  sys_plmn_id_s_type  plmn_2
)
{
  uint32 sim_mcc_digit_1  = plmn_1.identity[0] & 0x0F;
  uint32 sim_mcc_digit_2  = plmn_1.identity[0] / 0x10;
  uint32 sim_mcc_digit_3  = plmn_1.identity[1] & 0x0F;

  uint32 sim_mnc_digit_1  = plmn_1.identity[2] & 0x0F;
  uint32 sim_mnc_digit_2  = plmn_1.identity[2] / 0x10;
  uint32 sim_mnc_digit_3  = plmn_1.identity[1] / 0x10;

  uint32 bcch_mcc_digit_1 = plmn_2.identity[0] & 0x0F;
  uint32 bcch_mcc_digit_2 = plmn_2.identity[0] / 0x10;
  uint32 bcch_mcc_digit_3 = plmn_2.identity[1] & 0x0F;

  uint32 bcch_mnc_digit_1 = plmn_2.identity[2] & 0x0F;
  uint32 bcch_mnc_digit_2 = plmn_2.identity[2] / 0x10;
  uint32 bcch_mnc_digit_3 = plmn_2.identity[1] / 0x10;

  boolean value = FALSE;

  if ( (sim_mcc_digit_1 == bcch_mcc_digit_1) &&
       (sim_mcc_digit_2 == bcch_mcc_digit_2) &&
       (sim_mcc_digit_3 == bcch_mcc_digit_3) )
  {
    /* Check for wildcard MNC, if MNC if FFF, ignore it
    */
    if( sim_mnc_digit_1 == 0xF && sim_mnc_digit_2 == 0xF && sim_mnc_digit_3 == 0xF )
    {
      return TRUE;
    }
    if( bcch_mnc_digit_1 == 0xF && bcch_mnc_digit_2 == 0xF && bcch_mnc_digit_3 == 0xF )
    {
      return TRUE;
    }
    if (bcch_mnc_digit_3 == 0xF)
    {
      if ( (bcch_mcc_digit_1 == 3) &&
           (bcch_mcc_digit_2 == 1) &&
           (bcch_mcc_digit_3 <= 6) )
      {
        if ((sim_mnc_digit_3 == 0) || (sim_mnc_digit_3 == 0xF))
        {
          value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                             (sim_mnc_digit_2 == bcch_mnc_digit_2) );
        }
      }
      else
      {
        value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                           (sim_mnc_digit_2 == bcch_mnc_digit_2) );
      }
    }
    else
    {
      if ( (bcch_mcc_digit_1 == 3) &&
           (bcch_mcc_digit_2 == 1) &&
           (bcch_mcc_digit_3 <= 6) &&
           (bcch_mnc_digit_3 == 0) &&
           (sim_mnc_digit_3 == 0xF) )
      {
        value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                           (sim_mnc_digit_2 == bcch_mnc_digit_2) );        
      }
      else
      {
        value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                           (sim_mnc_digit_2 == bcch_mnc_digit_2) &&
                           (sim_mnc_digit_3 == bcch_mnc_digit_3) );
      }
    }
  }

  return value;
} /* sys_plmn_match */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_mcc_match

DESCRIPTION

  Function that compares two PLMN IDs and determines if their MCCs are equal

PARAMETERS

  sys_plmn_id_s_type  plmn_1                  The first PLMN ID.
  sys_plmn_id_s_type  plmn_2                  The second PLMN ID.

RETURN VALUE

  boolean  Flag indicating whether the two MCCs are equal (TRUE) or
  not (FALSE).

==============================================================================*/
boolean sys_mcc_match
(
  sys_plmn_id_s_type  plmn_1,
  sys_plmn_id_s_type  plmn_2
)
{
  uint32 sim_mcc_digit_1  = plmn_1.identity[0] & 0x0F;
  uint32 sim_mcc_digit_2  = plmn_1.identity[0] / 0x10;
  uint32 sim_mcc_digit_3  = plmn_1.identity[1] & 0x0F;

  uint32 bcch_mcc_digit_1 = plmn_2.identity[0] & 0x0F;
  uint32 bcch_mcc_digit_2 = plmn_2.identity[0] / 0x10;
  uint32 bcch_mcc_digit_3 = plmn_2.identity[1] & 0x0F;

  if ( (sim_mcc_digit_1 == bcch_mcc_digit_1) &&
       (sim_mcc_digit_2 == bcch_mcc_digit_2) &&
       (sim_mcc_digit_3 == bcch_mcc_digit_3) )
  {
    return TRUE;
  }

  return FALSE;

} /* sys_mcc_match */

/*==============================================================================

FUNCTION NAME

  sys_mcc_same_country

DESCRIPTION

  Function to check if the MCCs are of same country. MCCs from NA with
  different values (in range 310-316) are considered as MCCs of same country.

RETURN VALUE

  TRUE if MCCs are of same country, FALSE otherwise

==============================================================================*/

boolean sys_mcc_same_country
(
  uint32  mcc1,
  uint32  mcc2
)
{
  /*
  ** MCC in NA (i.e. in range 310-316) or India (with range 404 and 406) OR Japan (440 and 441)with different mcc values 
  ** are also considered as mcc of same country (NA).
  */
  return ((mcc1 == mcc2) ? TRUE : 
                          (((mcc1 >= 310) && (mcc1 <= 316) && (mcc2 >= 310) && (mcc2 <= 316)) || 
                            ((mcc1 >= 404) && (mcc1 <= 406) && (mcc2>=404) && (mcc2<=406)) || 
                            ((mcc1 >= 440) && (mcc1 <= 441) && (mcc2>=440) && (mcc2<=441))));
}

/*===========================================================================

FUNCTION sys_undefine_sys_id

DESCRIPTION
  This function undefines or initializes a system identifier.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sys_undefine_sys_id(

  sys_sys_id_s_type     *sys_id_ptr
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(sys_id_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) memset( sys_id_ptr, 0, sizeof( sys_sys_id_s_type ) );
  sys_id_ptr->id_type = SYS_SYS_ID_TYPE_UNDEFINED;

} /* sys_undefine_sys_id */



/*===========================================================================

FUNCTION sys_sys_id_match

DESCRIPTION
  This function compares two system identifiers.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = if the two PLMN identifiers are equal
  FALSE = if the two PLMN identifiers are not equal

SIDE EFFECTS
  None

===========================================================================*/
boolean sys_sys_id_match(

  sys_sys_id_s_type     sys_id_1,
  sys_sys_id_s_type     sys_id_2
)
{

  boolean   is_equal = FALSE;
  unsigned int       i = 0;

  if ( sys_id_1.id_type == sys_id_2.id_type )
  {
     switch (sys_id_1.id_type)
     {
       case SYS_SYS_ID_TYPE_UMTS      :
         is_equal = sys_plmn_match( sys_id_1.id.plmn, sys_id_2.id.plmn );
         break;

       case SYS_SYS_ID_TYPE_IS95      :
         is_equal = (boolean)( ( sys_id_1.id.is95.nid ==
                                 sys_id_2.id.is95.nid    ) &&
                               ( sys_id_1.id.is95.sid ==
                                 sys_id_2.id.is95.sid    ) &&
                               ( sys_id_1.id.is95.mcc ==
                                 sys_id_2.id.is95.mcc    ) &&
                               ( sys_id_1.id.is95.imsi_11_12 ==
                                 sys_id_2.id.is95.imsi_11_12 ) );
         break;

       case SYS_SYS_ID_TYPE_IS856     :
         is_equal = TRUE;
         for (i = 0; i < ARR_SIZE( sys_id_1.id.is856 ); i++)
         {
           if (sys_id_1.id.is856[i] != sys_id_2.id.is856[i])
           {
             is_equal = FALSE;
             break;
           }
         }
         break;


       case SYS_SYS_ID_TYPE_UNDEFINED :
         is_equal = TRUE;
         break;

       default                        :
         break;
     } /* switch id_type */
  } /* if id_type */

  return is_equal;

} /* cm_util_sys_id_match */

/*===========================================================================

FUNCTION sys_lac_match

DESCRIPTION
  This function compares two LAC system identifiers of a UMTS SYS ID.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = if the two LAC are equal
  FALSE = if the two LAC are not equal

  Note: TRUE is returned if both system identifiers are not UMTS type.

SIDE EFFECTS
  None

===========================================================================*/
boolean sys_lac_match(

  sys_sys_id_s_type     sys_id_1,
  sys_sys_id_s_type     sys_id_2
)
{
  if( sys_id_1.id_type == sys_id_1.id_type )
  {
    if( sys_id_1.id_type == SYS_SYS_ID_TYPE_UMTS)
    {
      if( sys_id_1.id.plmn_lac.lac != sys_id_2.id.plmn_lac.lac)
      {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/*==============================================================================

                            N E T W O R K   L I S T S

==============================================================================*/


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_signal_quality_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_signal_quality_e_type value                Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_signal_quality_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_signal_quality_is_valid
(
  sys_signal_quality_e_type value
)
{
  boolean valid = FALSE;

  valid = (boolean)(value > SYS_SIGNAL_QUALITY_NONE &&
                    value < SYS_SIGNAL_QUALITY_MAX    );

  return valid;
} /* sys_signal_quality_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_detailed_plmn_list_category_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_detailed_plmn_list_category_e_type value   Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_plmn_list_category_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_detailed_plmn_list_category_is_valid
(
  sys_detailed_plmn_list_category_e_type  value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_DETAILED_PLMN_LIST_CATEGORY_NONE &&
                    value < SYS_DETAILED_PLMN_LIST_CATEGORY_MAX    );

  return valid;
} /* sys_detailed_plmn_list_category_is_valid */


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_detailed_plmn_list_find_plmn

DESCRIPTION

  Function that verifies that the given PLMN  is in the PLMN list.

PARAMETERS

  sys_detailed_plmn_list_s_type  plmn_list_ptr    The PLMN list to be searched
  sys_plmn_id_s_type             plmn             The PLMN being searched


RETURN VALUE

 uint32  indicating position of match in PLMN list.

==============================================================================*/
boolean sys_detailed_plmn_list_find_plmn
(
  const sys_plmn_rat_category_list_s_type    * plmn_list_ptr,
  sys_plmn_id_s_type                         plmn
)
{
  uint32 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(plmn_list_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (i = 0; i < plmn_list_ptr->length; i++)
  {
    if ( sys_plmn_match(plmn,plmn_list_ptr->info[i].plmn))
    {
      return TRUE;
    }
  }

  return FALSE;

} /* sys_detailed_plmn_list_find_plmn */

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_detailed_plmn_list_found_plmn_rat

DESCRIPTION

  Function that verifies that the given PLMN and RAT is in the PLMN list.

PARAMETERS

  sys_detailed_plmn_list_s_type  plmn_list_ptr    The PLMN list to be searched
  sys_plmn_id_s_type             plmn             The PLMN being searched
  sys_radio_access_tech_e_type   rat              The RAT being searched

RETURN VALUE

  boolean  Flag indicating whether the PLMN and RAT was found in the PLMN list.

==============================================================================*/
boolean sys_detailed_plmn_list_found_plmn_rat
(
  const sys_detailed_plmn_list_s_type*      plmn_list_ptr,
  sys_plmn_id_s_type                        plmn,
  sys_radio_access_tech_e_type              rat
)
{
  uint32 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(plmn_list_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (i = 0; i < plmn_list_ptr->length; i++)
  {
    if ( (sys_plmn_match(plmn_list_ptr->info[i].plmn, plmn) ) &&
         (plmn_list_ptr->info[i].rat == rat) )
    {
      return TRUE;
    }
  }

  return FALSE;

} /* sys_detailed_plmn_list_found_plmn_rat */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_list_found_plmn

DESCRIPTION

  Function that verifies that the given PLMN is in the PLMN list.

PARAMETERS

  sys_plmn_list_s_type           plmn_list_ptr    The PLMN list to be searched
  sys_plmn_id_s_type             plmn             The PLMN being searched

RETURN VALUE

  boolean  Flag indicating whether the PLMN and RAT was found in the PLMN list.

==============================================================================*/
boolean sys_plmn_list_found_plmn
(
  const sys_plmn_list_s_type*               plmn_list_ptr,
  sys_plmn_id_s_type                        plmn
)
{
  uint32 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(plmn_list_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (i = 0; i < plmn_list_ptr->length; i++)
  {
    if (sys_plmn_match(plmn, plmn_list_ptr->plmn[i]))
    {
      return TRUE;
    }
  }

  return FALSE;

} /* sys_plmn_list_found_plmn */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_detailed_plmn_list_info_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_detailed_plmn_list_info_s_type value       Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_PLMN_list_info_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_detailed_plmn_list_info_is_valid
(
  sys_detailed_plmn_list_info_s_type value
)
{
  boolean valid;

  valid = (boolean)( sys_plmn_id_is_valid                     ( value.plmn ) &&
                     sys_radio_access_tech_is_valid           ( value.rat ) &&
                     sys_srv_capability_is_valid              ( value.plmn_service_capability ) &&
                     sys_signal_quality_is_valid              ( value.signal_quality ) &&
                     sys_detailed_plmn_list_category_is_valid ( value.list_category ) );

  return valid;
}



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_detailed_plmn_list_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_detailed_plmn_list_s_type value            Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_detailed_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_detailed_plmn_list_is_valid
(
  const sys_detailed_plmn_list_s_type* value_ptr
)
{
 
  uint32   i;
  boolean  valid = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (value_ptr->length <= SYS_PLMN_LIST_MAX_LENGTH)
  {
    for (i = 0; i < value_ptr->length; i++)
    {
      if (!sys_detailed_plmn_list_info_is_valid (value_ptr->info[i]))
      {
        valid = FALSE;
        break;
      }
    } /* for */
  }
  else
  {
    valid = FALSE;
  }

  return valid;

} /* sys_detailed_plmn_list_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_plmn_list_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_PLMN_list_s_type value                     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_plmn_list_is_valid
(
  const sys_plmn_list_s_type* value_ptr
)
{
  uint32   i;
  boolean  valid = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (value_ptr->length <= SYS_PLMN_LIST_MAX_LENGTH)
  {
    for (i = 0; i < value_ptr->length; i++)
    {
      if (!sys_plmn_id_is_valid (value_ptr->plmn[i]))
      {
        valid = FALSE;
        break;
      }
    } /* for */
  }
  else
  {
    valid = FALSE;
  }

  return valid;

} /* sys_plmn_list_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_user_pref_plmn_list_info_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_user_pref_plmn_list_info_s_type  value     Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_user_pref_plmn_list_info_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_user_pref_plmn_list_info_is_valid
(
  sys_user_pref_plmn_list_info_s_type  value
)
{
  boolean valid;

  valid = (boolean)( sys_plmn_id_is_valid( value.plmn ) );

  return valid;
} /* sys_user_pref_plmn_list_info_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_user_pref_plmn_list_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_user_pref_plmn_list_s_type  value          Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_user_pref_plmn_list_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_user_pref_plmn_list_is_valid
(
  const sys_user_pref_plmn_list_s_type*  value_ptr
)
{
  uint32   i;
  boolean  valid = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (value_ptr->length <= SYS_USER_PPLMN_LIST_MAX_LENGTH)
  {
    for (i = 0; i < value_ptr->length; i++)
    {
      if (!sys_user_pref_plmn_list_info_is_valid (value_ptr->info[i]))
      {
        valid = FALSE;
        break;
      }
    } /* for */
  }
  else
  {
    valid = FALSE;
  }

  return valid;

} /* sys_user_pref_plmn_list_is_valid */


/*------------------------------------------------------------------------------
   Mobility Management Information Functions
------------------------------------------------------------------------------*/


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_network_name_coding_scheme_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

   sys_network_name_coding_scheme_type_e_type value
                             Value that is to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_network_name_coding_scheme_type_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_network_name_coding_scheme_is_valid
(
  sys_network_name_coding_scheme_type_e_type value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_NW_NAME_CODING_SCHEME_NONE &&
                    value < SYS_NW_NAME_CODING_SCHEME_MAX);

  return valid;
} /* sys_network_name_coding_scheme_is_valid */


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_network_short_name_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  const sys_network_short_name_s_type* value_ptr Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_network_short_name_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_network_short_name_is_valid
(
  const sys_network_short_name_s_type* value_ptr
)
{

  boolean valid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e685 */
  valid = (boolean)((value_ptr->length <= sys_get_max_short_name_chars()) &&
                     sys_network_name_coding_scheme_is_valid(value_ptr->coding_scheme)&&
                    (value_ptr->spare_bits <= SYS_MAX_NW_NAME_SPARE_BITS));
  /*lint -restore */

  return valid;

} /* sys_network_short_name_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_network_full_name_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  const sys_network_full_name_s_type* value_ptr  Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_network_full_name_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_network_full_name_is_valid
(
  const sys_network_full_name_s_type* value_ptr
)
{
  boolean valid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e685 */
  valid = (boolean)((value_ptr->length <= sys_get_max_full_name_chars()) &&
                     sys_network_name_coding_scheme_is_valid(value_ptr->coding_scheme)&&
                    (value_ptr->spare_bits <= SYS_MAX_NW_NAME_SPARE_BITS));
  /*lint -restore */ 

  return valid;

} /* sys_network_full_name_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_time_zone_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_time_zone_type value                       Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_time_zone_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_time_zone_is_valid
(
  sys_time_zone_type value
)
{
  boolean valid;

  valid = (boolean)(value >= -96 && value <= 96);

  return valid;
} /* sys_time_zone_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_time_and_time_zone_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  const sys_time_and_time_zone_s_type* value_ptr Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_time_and_time_zone_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_time_and_time_zone_is_valid
(
  const sys_time_and_time_zone_s_type* value_ptr
)
{

  boolean valid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  valid = (boolean)( ( value_ptr->year   <= 99) &&
                     ( value_ptr->month  >= 1 && value_ptr->month <= 12) &&
                     ( value_ptr->day    >= 1 && value_ptr->day   <= 31) &&
                     ( value_ptr->hour   <= 23) &&
                     ( value_ptr->minute <= 59) &&
                     ( value_ptr->second <= 59) &&
                     sys_time_zone_is_valid ( value_ptr->time_zone ) );
  return valid;

} /* sys_time_and_time_zone_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_lsa_id_type_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_LSA_identity_type_e_type value             Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_LSA_identity_type_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_lsa_id_type_is_valid
(
  sys_lsa_identity_type_e_type value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_LSA_ID_TYPE_NONE &&
                    value < SYS_LSA_ID_TYPE_MAX);

  return valid;
} /* sys_lsa_id_type_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_lsa_id_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_LSA_identity_s_type value                  Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_LSA_identity_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_lsa_id_is_valid
(
  sys_lsa_identity_s_type value
)
{
  boolean valid;

  valid = sys_lsa_id_type_is_valid ( value.type );

  return valid;
} /* sys_lsa_id_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_day_sav_adj_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  sys_daylight_saving_adj_e_type value           Value that is to be checked
                                                 for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_daylight_saving_adj_e_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_day_sav_adj_is_valid
(
  sys_daylight_saving_adj_e_type value
)
{
  boolean valid;

  valid = (boolean)(value > SYS_DAY_SAV_ADJ_NONE &&
                    value < SYS_DAY_SAV_ADJ_MAX);

  return valid;
} /* sys_day_sav_adj_is_valid */



/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_mm_information_is_valid

DESCRIPTION

  Function that verifies that the input value is within its valid range.

PARAMETERS

  const sys_mm_information_s_type* value_ptr     Pointer to the value that is
                                                 to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the sys_mm_information_s_type
           is valid (TRUE) or invalid (FALSE).

==============================================================================*/
boolean sys_mm_information_is_valid
(
  const sys_mm_information_s_type* value_ptr
)
{

  boolean valid = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(value_ptr  != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (value_ptr->plmn_avail)
  {
    valid = (boolean)( valid && sys_plmn_id_is_valid( value_ptr->plmn ) );
  }

  if (value_ptr->full_name_avail)
  {
    valid = (boolean)( valid && sys_network_full_name_is_valid( &(value_ptr->full_name) ) );
  }

  if (value_ptr->short_name_avail)
  {
    valid = (boolean)( valid && sys_network_short_name_is_valid ( &(value_ptr->short_name) ) );
  }

  if (value_ptr->univ_time_and_time_zone_avail)
  {
    valid = (boolean)( valid && sys_time_and_time_zone_is_valid ( &(value_ptr->univ_time_and_time_zone) ) );
  }

  if (value_ptr->time_zone_avail)
  {
    valid = (boolean)( valid && sys_time_zone_is_valid          ( value_ptr->time_zone ) );
  }

  if (value_ptr->lsa_identity_avail)
  {
    valid = (boolean)( valid && sys_lsa_id_is_valid             ( value_ptr->lsa_identity ) );
  }

  if (value_ptr->daylight_saving_adj_avail)
  {
    valid = (boolean)( valid && sys_day_sav_adj_is_valid        ( value_ptr->daylight_saving_adj ) );
  }

  return valid;

} /* sys_mm_information_is_valid */


/* ==========================================================================
   FUNCTION:      SYS_GET_NUM_OF_MNC_DIGITS

   DESCRIPTION:
     This function returns length of MNC in the IMSI which is store in 4th
     byte of EF-AD.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:     
     TRUE:  Successful retrieval of number of mnc digits
     FALSE: Null input pointer

   SIDE EFFECTS:
     None
==========================================================================*/
boolean sys_get_num_of_mnc_digits(
  mmgsdi_data_type * imsi_ptr,
  mmgsdi_data_type * ad_ptr,
  uint8            * num_mnc_digits_ptr
)
{
  size_t i                     = 0;
  size_t mcc_value             = 0;
  uint16 mcc_pcs1900_na_list[] = {302, 310, 311, 312, 313,
                                  314, 315, 316, 334, 348 };

  if(imsi_ptr  == NULL || imsi_ptr->data_ptr == NULL || num_mnc_digits_ptr == NULL) 
  { 
    CM_ERR_2("imsi_ptr 0x%x, imsi_ptr->data_ptr or num_mnc_digits_ptr 0x%x is NULL", imsi_ptr, num_mnc_digits_ptr);
    return FALSE;
  }
  
  /* Old SIM cards had only 2 digits MNC and EF-AD with 3 bytes only.
     Default to 2 digits to keep backward compatibility */
  *num_mnc_digits_ptr = SYS_MNC_2_DIGITS;

  if(ad_ptr == NULL)
  {
    CM_MSG_HIGH_0("EF-AD ptr is NULL, use default 2 digit MNC");
    return TRUE;
  }

  /* 3GPP 23.122 Annex 'A'
     For PCS1900 for North America, regulations mandate that a 3-digit MNC
     shall be used; however during a transition period, a 2 digit MNC may
     be broadcast by the Network and, in this case, the 3rd digit of the
     SIM is stored as 0 (this is the 0 suffix rule).
     For these specific country codes, MNC length shall default to 3. */
  mcc_value  = (size_t)((imsi_ptr->data_ptr[SYS_IMSI_DIGIT_1_OFFSET] >> 4) &
                SYS_IMSI_LOW_DIGIT_MASK) * 100;
  mcc_value += (size_t)((imsi_ptr->data_ptr[SYS_IMSI_DIGIT_2_OFFSET]) &
                SYS_IMSI_LOW_DIGIT_MASK) * 10;
  mcc_value += (size_t)((imsi_ptr->data_ptr[SYS_IMSI_DIGIT_2_OFFSET] >> 4) &
                SYS_IMSI_LOW_DIGIT_MASK);
  for (i = 0; i < sizeof(mcc_pcs1900_na_list) / sizeof(uint16); i++)
  {
    if (mcc_value == (size_t)mcc_pcs1900_na_list[i])
    {
      *num_mnc_digits_ptr = SYS_MNC_3_DIGITS;
      break;
    }
  }

  if (ad_ptr->data_len > SYS_MNC_LEN_BYTE && ad_ptr->data_ptr != NULL)
  {
    if ( ((ad_ptr->data_ptr[SYS_MNC_LEN_BYTE] & 0x0F) == SYS_MNC_2_DIGITS ) ||
         ((ad_ptr->data_ptr[SYS_MNC_LEN_BYTE] & 0x0F) == SYS_MNC_3_DIGITS ) )
    {
      *num_mnc_digits_ptr = ad_ptr->data_ptr[SYS_MNC_LEN_BYTE] & 0x0F;
    }
    else
    {
      CM_ERR_1("EF-AD has invalid MNC length: 0x%x", ad_ptr->data_ptr[SYS_MNC_LEN_BYTE]);
    }
  }

  CM_MSG_HIGH_1("Number of MNC digits: 0x%x", *num_mnc_digits_ptr );
  return TRUE;
}/* sys_get_num_of_mnc_digits */

/*===========================================================================

FUNCTION sys_err_fatal_null_ptr_exception

DESCRIPTION
  Common error fatal function for all NULL ptr access
  
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  sys_err_fatal_null_ptr_exception_debug()
{
  CM_ERR_FATAL_0("NULL ptr access exception thrown!!");
}

/*===========================================================================

FUNCTION sys_err_fatal_null_ptr_exception

DESCRIPTION
  Common error fatal function for all NULL ptr access
  
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  sys_err_fatal_invalid_value_exception_debug(int value)
{
  CM_ERR_FATAL_1("Invalid value:%d passed!!",value);
}

/*===========================================================================

FUNCTION sys_lte_band_pref_lte_const_none

DESCRIPTION
  Provide the value for the constant NONE of the LTE band mask.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
sys_lte_band_mask_e_type sys_lte_band_pref_lte_const_none()
{
  sys_lte_band_mask_e_type lte_band_pref_none;

  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_pref_none);
  return lte_band_pref_none;
}

/*===========================================================================

FUNCTION sys_lte_band_pref_lte_const_any

DESCRIPTION
  Provide the value for the constant ANY of the LTE band mask. Enable all the
  defined LTE bands.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
sys_lte_band_mask_e_type sys_lte_band_pref_lte_const_any()
{
  sys_lte_band_mask_e_type lte_band_pref_any;
  int i;

  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_pref_any);

  for ( i=SYS_SBAND_LTE_EUTRAN_BAND1;
        i<= SYS_SBAND_LTE_EUTRAN_BAND14;i++)
  {
    SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_pref_any,(sys_sband_lte_e_type)i );
  }

  for ( i=SYS_SBAND_LTE_EUTRAN_BAND17;
        i<= SYS_SBAND_LTE_EUTRAN_BAND21;i++)
  {
    SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_pref_any,(sys_sband_lte_e_type)i );
  }

  for ( i=SYS_SBAND_LTE_EUTRAN_BAND23;
        i<= SYS_SBAND_LTE_EUTRAN_BAND43;i++)
  {
    SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_pref_any,(sys_sband_lte_e_type)i );
  }

  /* LTE-U bands. */
  #if(LTE_BAND_NUM == 256)
  SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_pref_any,SYS_SBAND_LTE_EUTRAN_BAND66 );
  SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_pref_any,SYS_SBAND_LTE_EUTRAN_BAND252 );
  SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_pref_any,SYS_SBAND_LTE_EUTRAN_BAND255 );
  #endif
  return lte_band_pref_any;
}

/*===========================================================================

FUNCTION sys_lte_band_pref_lte_const_no_chg

DESCRIPTION
  Provide the value for the constant NO_CHANGE of the LTE band mask.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
sys_lte_band_mask_e_type sys_lte_band_pref_lte_const_no_chg()
{
  sys_lte_band_mask_e_type lte_band_pref_no_chg;

  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_pref_no_chg);
  SYS_LTE_BAND_MASK_ADD_BAND(&lte_band_pref_no_chg, SYS_SBAND_LTE_EUTRAN_BAND60);

  return lte_band_pref_no_chg;
}

/*===========================================================================

FUNCTION sys_map_lte_band_to_sband

DESCRIPTION
  Mapping function from sys_band_class_e_type ==> sys_sband_lte_e_type

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
sys_sband_lte_e_type sys_map_lte_band_to_sband(

   sys_band_class_e_type lte_band
)
{
  sys_sband_lte_e_type lte_sband;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Map the specified band to its corresponding band preferences.
  */
  switch( lte_band )
  {
    case SYS_BAND_LTE_EUTRAN_BAND1:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND1;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND2:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND2;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND3:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND3;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND4:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND4;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND5:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND5;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND6:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND6;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND7:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND7;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND8:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND8;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND9:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND9;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND10:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND10;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND11:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND11;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND12:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND12;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND13:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND13;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND14:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND14;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND17:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND17;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND18:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND18;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND19:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND19;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND20:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND20;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND21:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND21;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND23:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND23;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND24:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND24;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND25:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND25;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND26:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND26;
      break;

   case SYS_BAND_LTE_EUTRAN_BAND27:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND27;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND28:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND28;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND29:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND29;
      break;

  case SYS_BAND_LTE_EUTRAN_BAND30:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND30;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND31:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND31;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND32:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND32;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND33:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND33;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND34:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND34;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND35:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND35;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND36:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND36;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND37:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND37;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND38:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND38;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND39:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND39;
    break;

    case SYS_BAND_LTE_EUTRAN_BAND40:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND40;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND41:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND41;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND42:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND42;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND43:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND43;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND66:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND66;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND125:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND125;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND126:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND126;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND127:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND127;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND252:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND252;
      break;

    case SYS_BAND_LTE_EUTRAN_BAND255:
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND255;
      break;

    default:
      CM_ERR_0("Incorrect LTE band");
      lte_sband = SYS_SBAND_LTE_EUTRAN_BAND_MAX;
      break;

  } /* switch */

  return lte_sband;
}

/*===========================================================================

FUNCTION sys_map_band_mask_to_lte_band_mask

DESCRIPTION
  Mapping function from sys_band_mask_e_type ==> sys_lte_band_mask_e_type.
  Useful when LTE band mask is defined as 256 bit data item.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
sys_lte_band_mask_e_type sys_map_band_mask_to_lte_band_mask(

   sys_band_mask_e_type band_pref
)
{
  sys_lte_band_mask_e_type lte_band_pref;
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS( &lte_band_pref );

  switch (band_pref)
  {
     case CM_BAND_PREF_NO_CHANGE:
       lte_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
       break;

     case CM_BAND_PREF_LTE_ANY:
     case SYS_BAND_MASK_ANY:
       lte_band_pref = SYS_LTE_BAND_MASK_CONST_ANY;
       break;

     default:
       #if(LTE_BAND_NUM == 256)
       lte_band_pref.bits_1_64 = band_pref;
       #else
       lte_band_pref = band_pref;
       #endif

  }

  return lte_band_pref;
}

/*===========================================================================

FUNCTION sys_map_lte_band_mask_to_band_mask

DESCRIPTION
  Mapping function from sys_lte_band_mask_e_type ==> sys_band_mask_e_type.
  Useful when LTE band mask is defined as 256 bit data item.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
sys_band_mask_e_type sys_map_lte_band_mask_to_band_mask(

   sys_lte_band_mask_e_type lte_band_pref
)
{
  #if(LTE_BAND_NUM == 256)
  return (lte_band_pref.bits_1_64);
  #else
  return (lte_band_pref);
  #endif
}
