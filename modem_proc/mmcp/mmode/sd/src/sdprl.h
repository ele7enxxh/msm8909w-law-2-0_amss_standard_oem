#ifndef SDPRL_H
#define SDPRL_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

    P R E F E R R E D   R O A M I N G    L I S T   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with SDPRL.C (i.e. the PRL interface components of the SD).

Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdprl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/13   gm      Selectively clear SDSR lists/systems and engine stack
03/23/12   gm      DIME integration fix: TMC deprecation and others.
11/30/11   jh      Added feature for dynamic rat acq order change
01/27/12   aj      set PRL status to inactive when sd_init() is invoked
10/18/11   rj      Feature to clear the Avoid System list 
10/03/11   aj      delete eqprl in EFS if generation fails
07/24/11   aj      SC SVLTE 1.0 merge to mainline
04/28/11   sv      Return geo system index in get_info_base
12/10/10   rk      Changes for e1XCSFB call
11/03/10   aj      DO -> LTE reselection code 
10/05/10   rk      Merging 1XCSFB pre-registration code 
09/03/10   aj      support to stay on any LTE system if UE is in conn state
08/15/10   aj      update encoding of large (>10k) prl
08/08/10   ay      Changes related to MRU format for LTE band
08/03/10   aj      Add LTE support in sdprl_is_supp_mode_band_pref
04/13/10   aj      memory optimization for equivalent PRL sys rec table cache 
03/19/10   aj      MMSS support
02/11/10   fj      Expose sdprl_encode_cdma_sys_rec().
07/10/09   np      Adding LTE Code 
03/12/09   rm      Removed sdprl_map_chan_to_blksys
02/10/09   sn      Constant GW UOOS scan time across band groups.
12/30/08   aj      WLAN subscription decoupling 
09/04/08   sv      Expose sdprl_update_wlan_mru_list
09/02/08   sv      Include Block-channel definition from prl.h
03/17/08   vs      Added function sdprl_get_addn_sys_record_info to get the
                   association tag and geo info for the system record
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
01/04/07   jqi     Provide the utility APIs for user to change the acquisition 
                   table of PRL
05/22/06   jqi     Made use of the chan list for HDR oh info.
03/20/06   sk      Added support for PRL extraction level SDPRL_EXTRACT_HOME.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
02/24/06   sk      Added a new enum to sdprl_comp_depth_e_type to support the
                   change of order of checks in sdprl_is_more_pref_upto().
                   Added comments to sdprl_comp_depth_e_type.
01/09/06   sk      Added support for custom homes bitmap in NV.
                   Made changes to treat custom home system as home system.
                   Added sdprl_is_more_pref_upto().
                   Added sdprl_roam_ind_map_custom_home_to_home().
                   Added sdprl_comp_depth_e_type.
12/02/05   jqi     LInt 8 fixes.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
11/16/04   sj      Added prototype for adding sid/nid lock.
10/04/04   jqi     Valid band and mode againt both target and PRL for task
                   initialization and system pref change command.
08/09/04   sj      Added hybr_pref param to sdprl_nam_sel().
06/21/04   RC      Improve association (collocation) determination algorithm
                   for HDR systems while in hybrid operation - CR 44638.
01/15/04   RC      Added support for FEATURE_SILENT_REDIAL_20
10/31/03   JQ      Added support for latest IS-683D format changes.
07/15/03   JQ      Added IS683 Support
                   - D PRL support.
                   - Propagate the LAC, band and chan from stack.
12/12/01   RC      Added support CDMA-HDR hybrid operation.
10/30/01   RC      Added support for IS-683C collcated systems.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
01/03/01   RC      Changes per 4th week of I&T.
11/17/00   RC      Initial release.

===========================================================================*/


#include "mmcp_variation.h"
#include "sd.h"        /* External interface to SD */
#include "sd_v.h"
#include "sdsr.h"      /* External interface to SDSR.C (System Record)*/
#include "prl.h"       /* Interface to PRL */
#include "prl_v.h"
#include "comdef.h"    /* Definition for basic types and macros */

/*---------------------------------------------------------------------------
                          Type for PRL parser
---------------------------------------------------------------------------*/

typedef struct {

  /* Associated dystem selection - MAIN or HDR.
  */
  sd_ss_e_type                    ss;

  /* Pointer to a RAM image of the PRL.
  */
  const byte                      *prl_ptr;

  /* Bit offsets to the beginning of the system table, as well as, the
  ** number of system table entries.
  */
  dword                           sys_tbl_offset;
  word                            sys_tbl_num;

  /* Index and offset (in bits) of the last system record table entry to be
  ** parsed, as well as, the record itself and its packed length in bits.
  */
  word                            sys_idx;
  dword                           sys_offset;
  prli_sys_rec_type               sys_rec;
  dword                           sys_rec_len;


  /* Index and offset (in bits) of the first system in the GEO of the last
  ** system table entry to be parsed.
  */
  word                            geo_idx;
  dword                           geo_offset;

  /* Index and offset of the first more preferred system relative to the last
  ** system table entry to be parsed. Note that the index and offset only
  ** have meaning when pri_is_higher = TRUE;
  */
  word                            pri_idx;
  dword                           pri_offset;
  boolean                         pri_is_higher;
  prl_sspr_p_rev_e_type           sspr_p_rev;
  word                            prl_insert_idx;
} sdprl_parser_s_type;

typedef struct  sdprl_tbl_elem_s_type{
 
  /* The system (i.e., mode, band and channel).
  */
  sd_sys_s_type       sys;

  struct  sdprl_tbl_elem_s_type *next;

}sdprl_tbl_elem_s_type; 

/* Type for a system record list.
*/
typedef struct {

  /* List for holding the system record references.
  */
  sdprl_tbl_elem_s_type     * sdprl_tbl_list;

  /* Number of system record references that are stored in list.
  */
  int               num;

} sdprl_tbl_list_s_type;

/*---------------------------------------------------------------------------
                           PRL Matching Levels
---------------------------------------------------------------------------*/

/* Enumeration of PRL matching levels.
*/
typedef enum {

  SDPRL_MATCH_NONE,               /* No match */

  SDPRL_MATCH_SID,                /* SID/NID only match */
  SDPRL_MATCH_SID_MODE,           /* SID/NID and mode only match */
  SDPRL_MATCH_SID_MODE_BAND,      /* SID/NID, mode and band only match */
  SDPRL_MATCH_SID_MODE_BAND_CHAN, /* SID/NID, mode, band and channel match */

  SDPRL_MATCH_MAX
} sdprl_match_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                           PRL Matching Levels
---------------------------------------------------------------------------*/

/* Enumeration of PRL sid/nid matching levels.
*/
typedef enum {

  SDPRL_MATCH_SID_NID_NONE,               /* No match */

  SDPRL_MATCH_WILDCARD_SID,       /* WILDCARD SID(PLMN) only match */
  SDPRL_MATCH_WILDCARD_NID,       /* SID(PLMN), WILDCARD NID(LAC) match */
  SDPRL_MATCH_SID_NID,            /* SID(PLMN) NID(LAC) match */

  SDPRL_MATCH_SID_NID_MAX
} sdprl_match_sid_nid_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                           PRL Extraction Levels
---------------------------------------------------------------------------*/

/* Enumeration of PRL information extraction levels.
*/
typedef enum {

  SDPRL_EXTRACT_ALL,            /* Indicate that all information should be
                                ** extracted */

  SDPRL_EXTRACT_MATCH_ONLY,     /* Indicate that only the information of a
                                ** system that has a PRL match (as apposed
                                ** to a system that does not have a PRL
                                ** match) should be extracted */

  SDPRL_EXTRACT_ROAM_COMPLY,    /* Indicate that only systems that comply
                                ** with the current roaming preferences
                                ** should be extracted */

  SDPRL_EXTRACT_SAME_BETTER,    /* Indicate that only systems that are of
                                ** same or better preference of the serving
                                ** system should be extracted */

  SDPRL_EXTRACT_HOME,           /* Indicate that only systems that are home
                                ** systems should be extracted */

  SDPRL_EXTRACT_MAX
} sdprl_extract_e_type;


typedef enum {

  SDPRL_APPEND_ALL,       /* Refer to Celluar acq rec for CDMA preferred rec
                          ** OR non-preferred GSM/WCDMA acq rec
                          ** for GW preferred record.
                          */
  SDPRL_APPEND_PREF_ONLY, /* Append only preferred acq record.
                          */
  SDPRL_APPEND_MAX

} sdprl_append_e_type;
/*---------------------------------------------------------------------------
                           PRL Matching Depth
---------------------------------------------------------------------------*/

/* Enumeration of more pref comparison depth.
**
** NOTE: Maintain the order of the enum to match the comparison order in
**       sdprl_is_more_pref_upto.
*/

typedef enum {

  SDPRL_COMP_UPTO_GEO_POS,
    /* Check if one system is more preferred than the other system in the
    ** first GEO that lists both systems.
    */

  SDPRL_COMP_UPTO_PRL_DESIG,
    /* Check if only one of the systems is listed as the preferred or most
    ** preference system in the PRL.
    */

  SDPRL_COMP_UPTO_ROAM,
    /* Check if the roaming indicator of one system is more favorable than
    ** the other.
    */

  SDPRL_COMP_UPTO_ONLY_ONE_MOST_PREF,
    /* Check if only one system is listed as most preferred system in its
    ** GEO.
    */

  SDPRL_COMP_UPTO_CELL_HOME,
    /* Check if both systems are available and only one of them is a cellular
    ** home side system.
    */

  SDPRL_COMP_UPTO_ACQ_TBL_ORDER,
    /* Check if one system is higher than the other in the PRL acquisition
    ** table. 
    */

  SDPRL_COMP_UPTO_DIGITAL,
    /* Check if only one of the systems is DIGITAL.
    */

  SDPRL_COMP_UPTO_LAST
    /* Check all the above conditions.
    */

} sdprl_comp_depth_e_type;
/* Type for holding the information that is associated with the SD-PRL
** component.
*/
typedef struct {

  /* The sdsr ref for accessing sdsr tbl.
  */
  sdsr_ref_type                   sr_ref;

  /* System index for corresonding record in sdsr tbl
  */
  word                             sys_ix;

} sdprl_map_elem_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for holding the information that is associated with the SD-PRL
** component.
*/
typedef struct {

  /* list to store sdsr ref per acq record in PRL
  */
  sdprl_map_elem_type *sdprl_acqix_list;
        
  /* number of record in list
  */
  byte  num;

  /* size of list
  */
  byte size;


} sdprl_map_tbl_type;

#define MAX_ACQ_IX_IN_PRL          512

/* WILD-CARD acq record value*/
#define  ACQ_IDX_WILDCARD      0xFFFF

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#define SD_CT_CDMA_IMSI_LEN 10

#endif

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_amps_bc0_sys_to_blksys

DESCRIPTION
  Given an AMPS band-class 0 system, this function returns the cellular
  system (in blksys representation) that is associated with this AMPS system.

DEPENDENCIES
  None.

RETURN VALUE
  The cellular system (in blksys representation) that is associated with the
  input AMPS band-class 0 system.

  SD_BLKSYS_MAX if the input system is bad/unsupported.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_blksys_e_type           sdprl_amps_bc0_sys_to_blksys(

        sd_cell_sys_e_type        cell_sys
            /* The band-class 0 CDMA channel for which to get the sub-band.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_map_amps_vc_to_sys

DESCRIPTION
  Map AMPS (band-class 0) voice channel to cellular system (A/B).

DEPENDENCIES
  None.

RETURN VALUE
  SD_CELL_SYS_A if AMPS voice channel belongs to A-Side.
  SD_CELL_SYS_B if AMPS voice channel belongs to B-Side.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_cell_sys_e_type        sdprl_map_amps_vc_to_sys(

        word                      voice_chan
            /* AMPS voice channel number to convert.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_supp_mode_band_pref

DESCRIPTION
  Inform the caller to whether a specified combination of mode and band
  preference is supported by the PRL that is associated with the currently
  selected NAM.

  For example, analog-only mode preference is only supported by a PRL that
  contains analog acquisition records.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the PRL that is associated with the currently selected NAM. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band preference to be validated.
            */

        sys_lte_band_mask_e_type  lte_band_pref,
            /* LTE Band preference to be validated.
            */

        sd_ss_band_pref_e_type    tds_band_pref
            /* TD-SCDMA Band preference to be validated.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_amps_backstop

DESCRIPTION
  Inform the caller to whether AMPS backstop is turned ON for the current
  NAM selection.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if AMPS backstop is turned ON for the current NAM selection. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_amps_backstop( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_exp_sys_idx

DESCRIPTION
  Check whether a system record matches its expected PRL's system table
  index.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID fields of the system record that is being checked is set to
  appropriate value.

RETURN VALUE
  TRUE if system record matchs its expected PRL's system table index. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_exp_sys_idx(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record to be checked for a match.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_associated

DESCRIPTION
  Check whether two system records are associated (collocated). Two systems
  are said to be associated if they are in the same GEO and their association
  tag is equal.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID fields of the system record that is being checked is set to
  appropriate value.

RETURN VALUE
  TRUE if system record 1 and 2 are associated. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_associated(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to system record 2.
            */
);

/*===========================================================================

FUNCTION sdprl_get_addn_sys_record_info

DESCRIPTION
  Get the associated tag of the system record.
  - find the system ercord in the PRL.
  - If found, return the association tag.
  - return null is not found

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID fields of the system record that is being checked is set to
  appropriate value.

RETURN VALUE
  association_tag if system record is found. SDSS_ASSN_TAG_NONE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                   sdprl_get_addn_sys_record_info(

        const sdsr_s_type      *sr_ptr,
            /* Pointer to system record.
            */
        byte                   *assn_tag,
            /* Pointer to association tag.
            */
        int                    *geo_offset,
            /* Pointer to system record's geo.
            */
        int                    *multi_geo_count
            /* Pointer to the counter, for counting geo's
            ** in a multi-geo scenario.
            */
);

/*===========================================================================

FUNCTION sdprl_is_more_pref_upto_2

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2 upto
  the requested depth.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

RETURN VALUE
  TRUE if system record 1 is more preferred than system record 2. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_more_pref_upto_2(

        sd_ss_e_type               sr1_ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_e_type               sr2_ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to system record 2.
            */

        sdprl_comp_depth_e_type   depth
            /* Depth of the comparison.
            */
);

/*===========================================================================

FUNCTION sdprl_is_more_pref_upto_3

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2 upto
  the requested depth.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

RETURN VALUE
  TRUE if system record 1 is more preferred than system record 2. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_more_pref_upto_3(

        sd_ss_e_type               sr1_ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_e_type               sr2_ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to system record 2.
            */


        const sdsr_s_type         *sr2_ptr_local,
            /* Pointer to system record 2.
            */


        sdprl_comp_depth_e_type   depth
            /* Depth of the comparison.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_more_pref_upto

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2 upto
  the requested depth.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

RETURN VALUE
  TRUE if system record 1 is more preferred than system record 2. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_more_pref_upto(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type               *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type               *sr2_ptr,
            /* Pointer to system record 2.
            */

        sdprl_comp_depth_e_type   depth
            /* Depth of the comparison.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_more_pref

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_more_pref(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to system record 2.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_autonam_proc

DESCRIPTION
  Does auto-NAM change related processing.

  Specifically, check whether a specified system record has a SID/NID that is
  appropriate for auto-NAM change and if so notify the registered client by
  calling on call on sd_si_autonam_change.

  And auto-NAM change is appropriate whenever the following conditions
  are met:

    1. The SID/NID of the specified system does not match any of the home
       SID/NIDs of the current NAM.
    2. The SID/NID of the specified system does match one of the home
       SID/NIDs of other than the current NAM.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_autonam_proc(

        const sdsr_s_type         *sr_ptr
            /* Pointer to system record to be checked against the home
            ** SID/NID list.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_blksys_app

DESCRIPTION
  Append the CDMA systems that are associated with a specified band class and
  a PCS frequency block/Cellular system to a specified system record list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if band-class and/or PCS frequency block/Cellular system are
  supported. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_blksys_app(

        sdsr_e_type               list,
            /* System record list to which to append the system records.
            */

        sd_band_e_type            band,
            /* The band-class from which to append the system records.
            */

        sd_blksys_e_type          blksys
            /* The PCS frequency block/Cellular system from which to append
            ** the system records.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_all_pref_sys_app

DESCRIPTION
  Append all the CDMA preferred systems that are supported by the MS to the
  specified list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_all_pref_sys_app(

        sdsr_e_type               list
            /* System record list to which to append the system records.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_home_side

DESCRIPTION
  Return the home side cellular system (i.e., the cellular system that is
  associated with the first home SID/NID) of the specified NAM and mode.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  The home side cellular system of the specified NAM and mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_cell_sys_e_type        sdprl_get_home_side(

        sd_nam_e_type             nam,
            /* NAM for which to get the cellular side.
            */

        sd_mode_e_type            mode
            /* Mode for which to get the cellular side.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info_base

DESCRIPTION
  Given a system record, geo_list and a pref_list, this function does
  the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  word                   sdprl_get_info_base(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               srv_list,
            /* The system record list for which to build the serving list.
            */

        int                       srv_list_pos,
            /* The system record list position for which to build the serving
            ** list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        sys_plmn_mcc_list         *hdr_mcc_list_ptr
            /* pointer to sys_plmn_mcc_list, to hold the PLMN sys MCCs
            */

);


#if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X) 
/*===========================================================================

FUNCTION sdprl_get_info_base_basic

DESCRIPTION
  Given a system record, srv_sys, geo_list and a pref_list, this function
  does the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.
    4. Update the srv_sys wiht PRL designation if required.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_get_info_base_basic(

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

);

#endif /* (FEATURE_LTE) && (FEATURE_LTE_TO_1X) */

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info2

DESCRIPTION
  Given a system record, srv_sys, geo_list and a pref_list, this function 
  does the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.
    4. Update the srv_sys wiht PRL designation.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  word                   sdprl_get_info2(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               srv_list,
            /* The system record list for which to subtract the GEO list and
            ** PREF list.
            */

        int                       srv_list_pos,
            /* The system record list position for which to subtract the GEO
            ** list and PREF list. 
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        boolean                   is_update_srv_sys
            /* Indicate if the serving system need to be updated.
            */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info3

DESCRIPTION
  Given a system record, srv_sys, geo_list and a pref_list, this function 
  does the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.
    4. Update the srv_sys wiht PRL designation.
    5. Update the HDR mcc list drived from PLMN in GEO

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  word                   sdprl_get_info3(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               srv_list,
            /* The system record list for which to build the serving list.
            */

        int                       srv_list_pos,
            /* The system record list position for which to build the serving
            ** list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        boolean                   is_update_srv_sys,
            /* Indicate if the serving system need to be updated.
            */

        sys_plmn_mcc_list       *hdr_mcc_list_ptr
            /* pointer to sys_plmn_mcc_list, to hold the PLMN sys MCCs for HDR
            */

            
);

#ifdef FEATURE_MMSS_3_1
/*===========================================================================

FUNCTION
 sdprl_update_tot_list

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void  sdprl_update_tot_list( sd_ss_e_type ss );
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_info

DESCRIPTION
  Given a system record, geo_list and a pref_list, this function does the 
  following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  word                   sdprl_get_info(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_gw_get_info

DESCRIPTION
  Given a GSM/WCDMA system record, geo_list and a pref_list, this function
  does the following:

    1. Set the PRL designation of the input system record as per the
       plmn_forbidden flag
    2. Clears the GEO_LST/PREF_LST/COLLOC_LST.
    3. Updates the system record as per the values passed.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE always.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_gw_get_info(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        sys_plmn_id_s_type        plmn,
            /* PLMN.
            */

        sys_lac_type              lac,
            /* LAC.
            */

        sys_srv_capability_e_type plmn_srv_cap,
           /* PLMN service capability.
           */

        sys_srv_status_e_type     service_status,
           /* Service status.
           */

        boolean                   plmn_forbidden,
          /* Is PLMN forbidden?
           */


        sys_roam_status_e_type    roaming_ind
           /* Roaming status.
           */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_component_init

DESCRIPTION
  Initialize the SD PRL services before task start. Now indicate that PRL
  is inactive at this time.


DEPENDENCIES
  This function must be called before any other sdprl_xxx() function is
  ever called.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_component_init( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_init

DESCRIPTION
  Initialize the SD PRL services.

  NOTE! This function only returns after the PRL related items (but the PRL
  itself that is being loaded when sdprl_nam_sel() is called) are read from
  NV.

DEPENDENCIES
  This function must be called before any other sdprl_xxx() function is
  ever called.

RETURN VALUE
  TRUE if SD PRL initialization is successful. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_init( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_save

DESCRIPTION
  Save PRL-Component related information to NV.

  NOTE! This function only returns after the SD related items (i.e., the
  MRU list) are written to NV.

DEPENDENCIES
  This function must be called when powering off the phone or going offline.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_save( void );

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_update

DESCRIPTION
  Clear the MRU if e_type is SD_MRU_CLEAN other wise update the index  record of MRU buffer. 

  NOTE! This function only returns after the SD related items (i.e., the
  MRU list) are written to MRU buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern  boolean                      sdprl_update( 
        sd_mmode_mru_table_entry_u_type *mru_table,
        /* MRU object whihc has the system record of any system mode needs to be write in to MRU buffer .
            */
        
        uint8 index ,
        /* Need to be update the indexth record of MRU buffer with system record  .
            */

        sys_sys_mode_e_type       mode,
        /*System Mode 
            */      

        sd_mru_update_e_type  mru_update_e_type
        /*Type of enum which tells either update the MRU buffer or clear the MRU buffer
             */		
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_read_mru

DESCRIPTION
  Read the indexth record from the MRU buffer and copied in to buffer.

  NOTE! This function only returns after the SD related items (i.e., the
  MRU list) are written to NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern  void 					sdprl_read_mru(
        sd_mmode_mru_table_entry_u_type *mru_table,
            /* MRU object whihc needs to fill with appropriate indexth record .
            */

        uint8 				   index
            /* index will give that  which record needs to be read from the MRU buffer .
            */
);
/*===========================================================================

FUNCTION sdprl_nam_sel

DESCRIPTION
  Inform the SD-PRL component of a new NAM selection.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize SDPRL.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully read from NV). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_nam_sel(

        sd_nam_e_type             curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_hybr_pref_e_type    hybr_pref
            /* Current hybrid preference.
            */
);

/*===========================================================================

FUNCTION sdprl_is_mru_updated_in_offline

DESCRIPTION
  Checks if MRU buffer was updated by other than SD. Sometimes UI components
  could be updating MRU buffer directly in LPM in order to enhance system
  selection behavior from LPM to ONLINE.

RETURNS
  TRUE - If MRU was updated in LPM/PWROFF
  FALSE - Otherwise
===========================================================================*/

boolean sdprl_is_mru_updated_in_offline( void );

/*===========================================================================

FUNCTION sdprl_nam_sel_hybr_ss

DESCRIPTION
  Inform the SD-PRL component of a new NAM selection for hybrid2 or hybrid3.

DEPENDENCIES
  
RETURN VALUE
  TRUE if updation is successful. FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern  boolean                   sdprl_nam_sel_hybr_ss(

        sd_ss_e_type   ss
            
);

/*===========================================================================

FUNCTION sdprl_copy_active_nam_mru_to_lists

DESCRIPTION
  Copy the MRU table that corresponds to the new NAM selection from its
  local RAM buffer into the SDSR_MRU_LST & SDSR_HDR_MRU_LST.

  NOTE: This memory is read from EFS at pwrup.
        This function doesn't re-read from EFS.

===========================================================================*/
void sdprl_copy_active_nam_mru_to_lists( void );


/*===========================================================================

FUNCTION sdprl_plmn_set_plmn_sys

DESCRIPTION
  Set plmn information: plmn lac include, plmn select mode, plmn id and lac.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                       sdprl_plmn_set_plmn_sys(

        sd_plmn_s_type             *plmn_ptr,
        /* Pointer to sid union type (PLMN)
        */

        const sys_plmn_id_s_type   *plmn_id_ptr,
        /* plmn id
        */

        word                       lac,
        /* lac id
        */

        sd_net_select_mode_e_type  net_select_mode
        /* network selection mode
        */
);




/*===========================================================================

FUNCTION sdprl_is_prl_active

DESCRIPTION
  Indicate if PRL is active or not.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  Reutn TRUE if PRL is active otherwise return FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_prl_active( void );


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_add_lock_sid_nid

DESCRIPTION
  Adds the given SID/NID as locked.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the specified SID/NID was added.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_add_lock_sid_nid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to system record to be added to the locked
            ** SID/NID list.
            */

        boolean                   ignore_nid
            /* Set nid as WILDCARD
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_clr_avoid_system_list

DESCRIPTION
  Cleasr the Avoid System list i.e unlock SID/NIDs present in the list.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                   sdprl_clr_avoid_system_list( void );


/*===========================================================================

FUNCTION sdprl_roam_ind_map_custom_home_to_home

DESCRIPTION
  Maps the custom homes to home and leaves the rest untouched.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  SYS_ROAM_STATUS_OFF if input is a custom home. Input value otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_roam_status_e_type    sdprl_roam_ind_map_custom_home_to_home(
    
        sys_roam_status_e_type    roam_ind
            /* Roaming indicator from system record.
            */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref

DESCRIPTION
  Given a band_pref in sd band_pref format, maps it to MRU format

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_map_sd_band_pref_to_mru_band_pref(

        sd_ss_band_pref_e_type    sd_band_pref,
            /* system selection band and sub-band preferences
            */
        sd_band_pref_s_type       *band_pref
           /* MRU band and sub-band preference output
           */
);


/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref2

DESCRIPTION
  Given band preferences for 2G/3G and 4G, maps it to MRU format for both
  2G3G and 4G

DEPENDENCIES
  None.

RETURN VALUE
  The band_pref, lte_band_pref

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_map_sd_band_pref_to_mru_band_pref2(

        sd_ss_band_pref_e_type    sd_band_pref,
            /* system selection band and sub-band preferences 2G3G
            */
        sd_band_pref_s_type       *band_pref,
           /* MRU band and sub-band preference output 2G3G
           */

        sd_sys_rec_lte_band_pref_e_type  sd_lte_band_pref,
            /* system selection band and sub-band preferences 4G
            */
        sd_lte_band_pref_s_type       *lte_band_pref
           /* MRU band and sub-band preference output 4G
           */
);
/*===========================================================================

FUNCTION sdprl_map_sd_band_pref_to_mru_band_pref3

DESCRIPTION
  Given band preferences for 2G3G and 4G, maps it to MRU formats

DEPENDENCIES
  None.

RETURN VALUE
  band_pref, lte_band_pref, band_pref_tds

SIDE EFFECTS
  None.

===========================================================================*/
void                      sdprl_map_sd_band_pref_to_mru_band_pref3(

        sd_ss_band_pref_e_type    sd_band_pref,
            /* system selection band and sub-band preferences 2G3G
            */
        sd_band_pref_s_type       *band_pref,
           /* MRU band and sub-band preference output 2G3G
           */

        sd_ss_band_pref_e_type    sd_band_pref_tds,
            /* system selection band and sub-band preferences 2G3G
            */
        sd_band_pref_s_type       *band_pref_tds,
           /* MRU band and sub-band preference output 2G3G
           */

        sd_sys_rec_lte_band_pref_e_type  sd_lte_band_pref,
            /* system selection band and sub-band preferences 4G
            */
        sd_lte_band_pref_s_type       *lte_band_pref
           /* MRU band and sub-band preference output 4G
           */
);
/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref3

DESCRIPTION
  Given 2G3G and 4G band preferences in MRU formats, maps them to
  regular format of type sd_ss_band_pref_e_type

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  void                  sdprl_map_mru_band_pref_to_sd_band_pref3(
                            


        sd_band_pref_s_type       band_pref,
           /* MRU band and sub-band preference output
           */

        sd_ss_band_pref_e_type    *sd_band_pref,
            /* system selection band and sub-band preferences
            */        

        sd_band_pref_s_type       band_pref_tds,
           /* MRU band and sub-band preference output 2G3G
           */

        sd_ss_band_pref_e_type    *sd_band_pref_tds,
            /* system selection band and sub-band preferences TDS
            */

        sd_lte_band_pref_s_type       lte_band_pref,
           /* MRU band and sub-band preference output
           */

        sys_lte_band_mask_e_type    *sd_lte_band_pref
            /* system selection band and sub-band preferences
            */
);
/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref2

DESCRIPTION
  Given a band preferences for 2G/3G and 4G in MRU format, maps it to the regular 
  sd_ss_band_pref_e_type formats.
DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/

extern  void                    sdprl_map_mru_band_pref_to_sd_band_pref2(

        
        sd_band_pref_s_type       band_pref,
           /* MRU band and sub-band preference output 2G/3G
           */

        sd_ss_band_pref_e_type    *sd_band_pref,
            /* system selection band and sub-band preferences 2G/3G
            */

        sd_lte_band_pref_s_type       lte_band_pref,
           /* MRU band and sub-band preference output 4G
           */

        sys_lte_band_mask_e_type    *sd_lte_band_pref
            /* system selection band and sub-band preferences 4G
            */
);


/*===========================================================================

FUNCTION sdprl_is_sys_match

DESCRIPTION
  indicate if there is a match of input system in the PRL
  
DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  True if there is a match.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdprl_is_sys_match(

       sd_ss_e_type               ss,
           /* System selection enum type
           */

       sdsr_ref_type              sr_ref
            /* System reference
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_mru_clr

DESCRIPTION
  Clear the MRU of the current NAM.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void                      sdprl_clr_mru( void );

/*===========================================================================

FUNCTION sdprl_is_prl_gw_mode_gw

DESCRIPTION
  Parses through GW systems in PRL / GW_LST and finds whether both GSM and 
  WCDMA systems are present or not

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  Boolean - Whether both GSM and WCDMA records present or not.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                   sdprl_is_prl_gw_mode_gw(
     sd_ss_e_type               ss
);


#ifdef FEATURE_MMSS_3_1
/*===========================================================================

FUNCTION 
 sdmmss_write_to_prl_buffer

DESCRIPTION
  This function encodes the sys rec tbl and acq_rec tbl into equivalent PRL.
  
DEPENDENCIES
  None
    
RETURN VALUE
  TRUE - eq PRL was created/ validated successfully
  FALSE - otherwise
  
SIDE EFFECTS
  None

===========================================================================*/
extern boolean sdprl_write_to_prl_buffer(void);

/*===========================================================================

FUNCTION 
 sdprl_encode_sys_record

DESCRIPTION
  This function encodes the system record.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void sdprl_encode_sys_record(
  dword                       *offset,
  prli_sys_rec_type           *rec_ptr,
  byte                        *new_prl_ptr
);

#endif // FEATURE_MMSS_3_1

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_init

DESCRIPTION
  Initialize a PRL-parser with a new PRL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                            sdprl_parser_init(

        sdprl_parser_s_type             *parser_ptr,
            /* Pointer to a PRL-parser item to be initialized with the
            ** PRL that is pointed to by the prl_ptr parameter.
            */

        sd_ss_e_type                    ss,
            /* System selection - MAIN or HDR.
            */

        const byte                      *prl_ptr,
            /* Pointer to a PRL RAM image with which to initialize the
            ** PRL-parser that is pointed to by the parser_ptr parameter.
            */

        const prl_hdr_s_type            *prl_hdr_ptr
            /* Pointer to buffer that holds information about the PRL that is
            ** pointed to by prl_ptr.
            */
);

/*===========================================================================

FUNCTION sdprl_parser_get

DESCRIPTION
  Extract the system table entry that is currently pointed to by the parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  Length of system table entry if entry is successfully extracted.
  0 otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  dword                      sdprl_parser_get(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

        prli_sys_rec_type         *sys_rec
            /* Pointer to a buffer where to store the system record that
            ** is extracted from the PRL.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_parser_next

DESCRIPTION
  Increment the PRL parser to point at the next system record (of the PRL
  with which the parser is currently associated). Return the content of
  newly pointed record via the rtrn_sys_rec_ptr parameter.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if next system record is successfully extracted. FALSE otherwise.

SIDE EFFECTS
  Increment the system table index and offset to point at the next system
  table entry.

  Increment the GEO index and offset as necessary.

  Increment the PREF index and offset as necessary.

===========================================================================*/
extern  boolean                   sdprl_parser_next(

        sdprl_parser_s_type       *parser_ptr,
            /* Pointer to a PRL-parser item to be initialized with the
            ** PRL that is pointed to by the prl_ptr parameter.
            */

        prli_sys_rec_type         *rtrn_sys_rec_ptr
            /* Pointer to a buffer where to store the system record that
            ** is extracted from the PRL.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_acq_rec_app

DESCRIPTION
  Append the acquisition record that is pointer to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_acq_rec_app(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        sdprl_append_e_type       append_type,
            /* Indicates whether the AMPS portion of an acquisition record of
            ** type PRL_ACQ_REC_CELLULAR_GENERIC should be appended.
            */
            
        word                      acq_ix
        
            /* System record lists to which to append the acquisition record.
            */
            
);

/*===========================================================================

FUNCTION 
 sdprl_check_roam

DESCRIPTION
  This function provides a wrapper for sdprl_is_home_sid_nid. used to check if
  current system record has sid/nid in the home_sid_nid list.
  
DEPENDENCIES
  None
    
RETURN VALUE
  TRUE: if the system is a home system.
  FALSE: if system is not in the home_sid_nid list.
  
SIDE EFFECTS
  None

===========================================================================*/
extern boolean sdprl_check_roam(

              const sdsr_s_type         *sr_ptr
                  /* Pointer to system record to be checked against the home
                  ** SID/NID list.
                  */
  );


/*===========================================================================

FUNCTION sdprl_add_gwl_sys

DESCRIPTION
  Add GSM/WCDMA/LTE systems to the specified list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize SDPRL.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.
===========================================================================*/
extern void                      sdprl_add_gwl_sys(

        sdsr_e_type               list            

);



/*===========================================================================

FUNCTION sdprl_init_acq_map_tbl

DESCRIPTION
  It initialize acq map table .

DEPENDENCIES
  The PRL must have already been read from NV into RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

 EXTERN  void sdprl_init_acq_map_tbl(void);
 

/*===========================================================================

FUNCTION sdprl_make_hdr_colloc_list

DESCRIPTION
  Make HDR colloc list at fresh acquisition. This is based on PRL only.
  As and when 1x systems are acquired, these collocated lists are updated.
  This function needs to be called at nam sel and LPM.

===========================================================================*/
void sdprl_make_hdr_colloc_list( void );


/*===========================================================================

FUNCTION sdprl_acq_rec_app_from_map

DESCRIPTION
  Get  sdsr ref from map table using input paramters. 
  

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN void sdprl_acq_rec_app_from_map(
      sdsr_e_type               list1,
      /* System record lists to which to append the acquisition record.
                     */
                     
      word                            sys_idx,
      /* system index
      */

      byte                  acq_index,      
      /* Acquisition record index */
      prl_acq_rec_type      acq_rec_type
      /* Acquistion record type*/

);
/*===========================================================================

FUNCTION sdprl_acq_rec_app_from_map

DESCRIPTION
  Get  sdsr ref from map table using input paramters. 
  

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

sdprl_map_tbl_type* sdprl_get_map_tbl(void);



/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_tbl_temp_list_reset

DESCRIPTION
  It resets temp scratch pad list
  
DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdprl_tbl_temp_list_reset(void);
/*===========================================================================

FUNCTION NAME cmmmgsdi_read_cdma_imsi

DESCRIPTION

  Function that reads the IMSI from the SIM.

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
extern boolean cmmmgsdi_read_cdma_imsi
(
  void
);
/*===========================================================================

FUNCTION NAME cmmmgsdi_get_imsi_mcc

DESCRIPTION

  Function that reads the sends the mcc value read from the SIM

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
extern word cmmmgsdi_get_imsi_mcc
(
  void
);
/*===========================================================================

FUNCTION NAME cmmmgsdi_get_imsi_mnc

DESCRIPTION

  Function that reads the sends the mnc value read from the SIM

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
extern byte cmmmgsdi_get_imsi_mnc
(
  void
);
/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_get_more_pref_ref

DESCRIPTION
  Given a system record, geo_list and a pref_list, this function does the 
  following:


DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if a minimum sr ref was successfully found in the geo matching the splmns.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdprl_get_more_pref_ref
(
    	sd_ss_e_type			   ss,
        /* System selection - MAIN or HDR.
        */

        sd_splmn_info_list_s	   *splmn_info_ptr
);
/*===========================================================================

FUNCTION sdprl_free_gwl_more_pref_list

DESCRIPTION
  frees the gwl more pref list composed while computing 
  more pref systems than the current srv sys of mode 'sys_mode'


DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if a minimum sr ref was successfully found in the geo matching the splmns.

SIDE EFFECTS
  None.

===========================================================================*/

extern void sdprl_free_gwl_more_pref_list(sys_sys_mode_e_type sys_mode);	
/*===========================================================================

FUNCTION sdprl_plmn_exact_match

DESCRIPTION
  compares two plmn ids and evaluates if they are identical


DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE for exact match.
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern   boolean sdprl_plmn_exact_match
(
    sys_plmn_id_s_type    plmn_1,
    sys_plmn_id_s_type    plmn_2
);


/*===========================================================================

FUNCTION sdprl_ral_cpy

DESCRIPTION
  Copy the RAL table that corresponds to a from its
  local RAM buffer into a specified system record list.


DEPENDENCIES
  The RAL must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdprl_ral_cpy(

        sdsr_e_type               list,
            /* System record list into which to copy the RAL entries.
            */
        sys_modem_as_id_e_type asubs_id
);

#endif /* SDPRL_H */




