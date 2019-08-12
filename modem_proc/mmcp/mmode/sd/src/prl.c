/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            P R E F E R R E D    R O A M I N G     L I S T

GENERAL DESCRIPTION
  This file makes up the Preferred Roaming List (PRL) component of SD.

EXTERNALIZED FUNCTIONS

  prl_map_tsb58_roam_to_sd_roam
    Map TSB-58 roaming indicator representation to the internal DB roaming
    indicator representation.

  prl_map_acq_rec_to_sd_mode
    Map acquisition record type to SD mode.

  prl_buf_get_ptr
    Return a pointer to the PRL buffer.

  prl_hdr_get_ptr
    Return pointer to a structure containing the header information of the
    currently loaded PRL.

  prl_hdr_nv_get_ptr
    Return pointer to a structure containing the header information of the
    PRL that is stored in NV.

  prl_extract_acq_rec
    Extract a specified acquisition record from the currently loaded PRL.

  prl_extract_sys_rec
    Extract PRL system record from a specified memory location.

  prl_sys_rec_get_sid
    Extract the SID from a PRL system record.

  prl_sys_is_wildcard
    Check whether a system record has a wildcard ID.

  prl_init
    Initialize the PRL component.

  prl_validate
    Validate a specified PRL.

  prl_get_total_size
    Get the total size of the PRL. If a 2nd PRL is detected at the end of the
    1st PRL, return the total size of 1st PRL + 2nd PRL + 3rd CRC. If no
    PRL is detected at the end of the 1st PRL, return the size of 1st PRL.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  The PRL component must be initialized by calling prl_init().

Copyright (c) 1996-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/prl.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------     
03/10/13   vs      Adding LTE band 27
09/30/13   sm      Adding LTE band 31   
07/24/13   vs     Adding LTE band 30
07/10/13   ar      checking index (i - invalid_cnt) should greater than zero 
03/11/13   vs     Adding LTE band 28 and 29.
08/24/12   fj      Adding LTE band 26.
08/10/12   fj      Adding LTE band 23.
11/30/11   jh      Added feature for dynamic rat acq order change
01/20/12   sg      Adding TD-SCDMA RAT into default PRL    
01/02/12   vk      Fixed memory leak issue.
05/17/11   xs      Adding LTE band 24-25
05/13/11   mj      Remove Lint e788 directives, use defaults from Lint cfg
05/10/11   cl      Fix PRL records parsing/appending logic
03/28/11   xs      Added LTE band 41-43
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/11/11   am      Adding QTF support
02/03/11   am      Add UMTS BC19-850 band support in SD.
01/28/11   fj      Remove FEATURE_SD_GWL.
12/23/10   sv      Integrate DSDS changes
09/21/10   aj      Fixes for MCC MNC subnet id record matching
09/15/10   cl      Fix prl_offset variable initialization
08/26/10   aj      Update decoding for MCC MNC subnet id record
08/06/10   aj      Replaced SD_ERR with SD_MSG_ERROR
08/06/10   aj      Reverting BC6 addition to default PRL init.
07/30/10   aj      LTE Rel 9 Band class support
07/14/10   mh      Fixed compile warnings
07/14/10   gm      BC6 is added to default PRL init.
07/08/10   gm      Fix to Klocwork deferred critical errors.
07/01/10   fj      Add support for all the LTE bands.
06/18/10   aj      Add support to pass HDR subnet mask length to CM
06/10/10   ak      Mainlining memory optimization changes.
06/09/10   gm      Fix for Klocwork errors.
06/04/10   aj      Resolve lint warnings
05/07/10   aj      default GWL PRL support
04/26/10   aj      LTE code fixes for BSR operation
04/20/10   aj      updates to fix MCC MNC handling in PRL and MLPL
04/13/10   aj      memory optimization for equivalent PRL sys rec table cache
04/07/10   aj      update prl_plmn_wildcard function
03/26/10   aj      Initialize GWL Acq Record structure.
03/19/10   aj      MMSS support
03/19/10   sg      Add UMTS BC11-1500 band support in SD. Added UMTS BC11-1.5G to
                   default gw single band prl.
02/24/10   mp      Use NV_ROAMING_LIST_MAX_SIZE, if nv_max_size_of_roaming_list returns 0
02/24/10   fj      Added prl_get_nv_prl_size().
02/20/10   fj      Added dynamic memory allocation for MMSS option 1.1.
02/11/10   fj      Added support for MCC-MNC system record.
02/09/10   mp      EF_EPRL Implementation.
02/09/10   pm      Fix KW error - array index may be out of bounds
01/11/10   fj      Added support for GWL-GENERIC acquisition record.
12/14/09   ay      Changed featurization for GWL
12/02/09   sg      Fix KW erros - array may be out of bounds.
11/19/09   sg      Allocate memory buf of size same as used in nvim.c for reding PRL from NV.
10/30/09   pm      Fix KW error - array may be out of bounds
10/16/09   ay      Featurized the creation of default PRL
10/13/09   sg      Allocate dynamic memory for reading prl from NV.
                   Release the memory after reading the rpl from NV.
06/10/09   fj      Added support for MMSS option 1.1.
09/25/09   sv      Lint fix.
09/19/09   sv      KW Fixes
07/10/09   np      Adding LTE Code
03/20/09   rm      Renaming : sdprl_map_chan_to_blksys -> sd_map_chan_to_blksys
                   sdss_map_band_to_band_pref -> sd_map_band_to_band_pref
10/14/08   rm      Printing debug message for Single band group
10/02/08   aj      Bandclass support for BC17, 18, 19
09/25/08   sv      Memory Reduction changes
09/02/08   sv      Count all the channels in blocks for unique band/chan
                   systems
08/25/08   ak      Fixed the general compiler warnings.
08/23/08   ak      Correcting the compilation error caused in 6K targets.
08/12/08   fj      Added new API prl_commit2()
07/24/08   jqi     Do not skip the GSM/UMTS record if there is only GSM or
                   UMTS record with GSM/UMTS
01/28/08   sg      Changes to make Single Band Group PRL the default PRL
12/19/07   ka      Fixing compilation.
12/19/07   ka      Changes to support 16k PRL
12/17/07   ka      Fixing compiler warnings.
11/29/07   ak      Standardized channel validation and updated channel
                   lowest and highest values as per standard.
11/28/07   ak      Updates to enable 8K prl
11/21/07   dm      Updated GSM/WCDMA default PRL with support for RGSM
10/31/07   dm      Explicitly type casted prl_ptr->roaming_list to be of (byte *)
10/16/07   dm      Removed the extra value 0xff that was getting inserted in
                   the prl_ptr->roaming_list buffer while CDMA-800/1900/AMPS-800
                   default prl gets created.
                   Also created a default buffer named prl_default_buffer to
                   populate the roaming list values all at once
09/18/07   pk      Correcting the GW validation checks
08/30/07   cl      Replaced INTLOCK / INTFREE with rex_enter_crit_sect() /
                   rex_leave_crit_sect()
08/25/07   jqi/cl  Removed the cell/pcs band mapping features.
07/24/07   cl      Mainline FEATURE_2ND_PRL and FEATURE_INT_ROAMING
03/30/07   jqi     Lint fix.
02/12/07   jqi     Externalized prl_get_ptr() API;
01/04/07   jqi     Provide the utility APIs for user to change the acquisition
                   table of PRL
11/15/06   pk      Added support for UTF default PRL
09/25/06   jqi     Fixed the compilation warnings.
09/21/06   jqi     Compilation warning fixes.
08/21/06   jqi     Fixed compilation warnings.
06/30/06   jqi     Do not invalid the GW preferred record if only one RAT
                   is present in the record.
05/04/06   jqi     Lint fixes.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/23/06   pk      Corrected the procedure to count unique systems
01/12/06   pk      Modified the PRL validation routine. Skip over invalid
                   acq records and invalid channels and bands
12/02/05   jqi     Lint fixes for lint ver8.
11/02/05   sk      Fixed all Lint and compiler warnings.
06/26/05   ic      Lint and compiler warnings cleanup
11/30/04   jqi     Externalize the API prl_hdr_extract().
09/03/04   RC      Added prl_get_total_size() to return total size of PRL.
08/26/04   jqi     Added WCDMA 1800 band support.
08/26/04   jqi     Lint fix.
08/16/04   dk      Changed prl_validate_proc() to ignore the size of reserved
                   bits before CRC.
06/29/04   RC      Ignore 'valid' field in prl_read(), we validate PRL later.
06/04/04   RC      Added support for FEATURE_2ND_PRL
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/19/04   jqi     Lint fix update.
03/08/04   jqi     Lint fix.
01/14/04   JQ      Updated default GW PRL with two band groups.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
11/25/03   JQ      Replaced with a new default D PRL with all G/W bands.
11/07/03   JQ      Replaced with a new default D PRL with a GW preferred rec.
11/05/03   JQ      Do not error fatal while validating PRL.
                   More MRU3 updates.
10/31/03   JQ      Added support for latest IS-683D format changes.
10/09/03   jqi     Adjusted band preference enum.
08/18/03   RC      Fixed issues with IS-683D implementation.
07/15/03   JQ      Added Extended PRL support
                   - D PRL
                   - PRL auto detection
                   - Skip over undefined ACQ/SYS record.
05/06/03   JQ      Added IS683B PRL support.
03/27/03   RC      Added a check for NID_INC field in prl_validate().
03/27/03   RC      Create a 1 channel default PRL for non 1x targets.
03/07/03   SJ      Changed MS level for few debug messages.
01/16/03   SJ      Removed use of DB if SD20 is defined. Updated
                   prl_create_dflt to create generic PRL and special PRL for
                   JCDMA and cell with AMPS and cell w/o AMPS.
10/30/02   ck      Merged in RTRE changes.
07/18/02   RC      Added prl_map_acq_rec_to_sd_mode and prl_sys_rec_get_sid.
03/14/02   RC      More PRL format changes to support IS-683C/SSPR_P_REV=3.
03/05/02   RC      PRL format changes to support IS-683C/SSPR_P_REV=3.
02/06/02   RC      Eliminated compiler warnings.
02/01/02   RC      Added multi-thread protection in prl_extract_acq_rec().
10/23/01   az      Moved code around in mcsyspr_read_roaming_list() so that
                   when prl_reformat operation fails, default roaming list
                   is used.
09/07/01   RC      Added support for T53 and JCDMA features.
08/09/01   RC      Eliminated a compiler warning in prl_create_dflt().
07/20/01   RC      Fixed IS-856 system table entry extraction.
07/20/01   RC      Added support for IS-683C.
06/26/01   RC      Now prl_validate() declares a PRL as invalid if none of
                   the acquisition records in the PRL's acquisition table is
                   supported by the target.
05/25/01   tc      Changed old RUIM feature switch to new UIM (server) feature
                   switch.
02/09/01   RC      Initial release for SD 2.0.


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"

#include "mmcp_variation.h"
#include "prl.h"      /* Interface to PRL services */
#include "prl_v.h"
#include "sdss.h"     /* External interface to sdss.c */
#include "sdi.h"      /* Internal interface to sd.c */
#include "sddbg.h"    /* SD debug macros */
#include "sdprl.h"    /* External interface to sdprl.c */
#include "sdnv.h"     /* External interface to sdnv.c */

#include "customer.h" /* Customer configuration file */
#include "msg.h"
#include "err.h"

#include "bit.h"
#include "nv.h"
#include "crc.h"
#include "modem_mem.h"
#include "sdtask.h"

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

#include <stringl/stringl.h>

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*---------------------------------------------------------------------------
                              Local Data
---------------------------------------------------------------------------*/

/* PRL RAM image.
**
** The roaming list is read from NV and stored here in its bit-packed format.
** There is one roaming list per NAM, but only one roaming list at a time is
** in RAM because they are large.
*/
static prl_ram_roaming_list_type  prl;
static nv_ram_roaming_list_type   *prl_buffer = NULL;

/* Offset to the beginning of the PRL that is stored in the
** prl.roaming_list[] buffer.
** If a 2nd PRL is concatenated to the end of the 1st PRL (that is stored
** in prl.roaming_list[]), this offset will point at the beginning of 2nd PRL
** (so that the 2nd (not the 1st) PRL is being used). Else, this offset will
** point at the beginning of the 1st PRL (i.e. it will be 0).
*/
static int prl_offset = 0;

/* Item to hold the information about the currently loaded PRL.
*/
static prl_hdr_s_type   prl_hdr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Item to hold the information about the PRL that is stored in NV and
** associated is the currently selected NAM. Note that the PRL that is stored
** in NV might not be the currently loaded PRL (e.g. the PRL is disabled).
** The UI and OTASP need this information.
*/
static prl_hdr_s_type   prl_hdr_nv;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Variables for bookkeeping information so that we can remember where we are
** within the roaming list.
*/
static int    prl_acq_rec_index;
static word   prl_acq_rec_bit_offset;
static boolean prl_updated_from_mmgsdi = FALSE;

typedef struct 
{
  q_link_type               link;
    /* Link element to link clients into a client-list */
    /* this element has to be the first in this structure
           because the address of link is also used as
           the "self pointer" (to save memory space by eliminating
           self_ptr in q_link_type) to access other element in this
           data structure */
           
  sd_sys_s_type tbl_ptr;
} unq_mode_band_chan_type;



/*---------------------------------------------------------------------------
                              Defines
---------------------------------------------------------------------------*/


/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_tsb58_roam_to_sd_roam

DESCRIPTION
  Map TSB-58 roaming indicator representation to the internal DB roaming
  indicator representation.

DEPENDENCIES
  None.

RETURN VALUE
  DB roaming indicator value corresponding to input TSB-58 roaming indicator
  value.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_roam_status_e_type              prl_map_tsb58_roam_to_sd_roam(

        sys_roam_status_e_type              roam_ind
            /* TSB roaming indicator which to convert.
            */

)
{
  switch( roam_ind )
  {
    case SYS_ROAM_STATUS_OFF:
      return SYS_ROAM_STATUS_ON;
    case SYS_ROAM_STATUS_ON:
      return SYS_ROAM_STATUS_OFF;
    case SYS_ROAM_STATUS_BLINK:
      return SYS_ROAM_STATUS_BLINK;
    default:
      return roam_ind;
  }

} /* prl_map_tsb58_roam_to_sd_roam */




/*===========================================================================

FUNCTION prl_map_sd_roam_to_tsb58_roam

DESCRIPTION
  Map the internal DB roaming indicator representation to TSB-58 roaming
  indicator representation.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_roam_status_e_type              prl_map_sd_roam_to_tsb58_roam(

        sys_roam_status_e_type              roam_ind
            /* Internal DB indicator which to convert.
            */

)
{
  switch( roam_ind )
  {
    case SYS_ROAM_STATUS_ON:
      return SYS_ROAM_STATUS_OFF;
    case SYS_ROAM_STATUS_OFF:
      return SYS_ROAM_STATUS_ON;
    case SYS_ROAM_STATUS_BLINK:
      return SYS_ROAM_STATUS_BLINK;
    default:
      return roam_ind;
  }

} /* prl_map_tsb58_roam_to_sd_roam */



/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_gsm_band_to_band

DESCRIPTION
  Map PRL GSM bands to corresponding SD bands.

DEPENDENCIES
  None.

RETURN VALUE
  SD band.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_band_e_type             prl_map_gsm_band_to_band(

        prl_gsm_band_e_type        gsm_band
           /* GSM band to map.
           */
)
{
  /* Map PRL GSM bands to corresponding SD bands.
  */
  switch( gsm_band )
  {
    case PRL_GSM_450:     return SD_BAND_GSM_450;
    case PRL_GSM_480:     return SD_BAND_GSM_480;
    case PRL_GSM_750:     return SD_BAND_GSM_750;
    case PRL_GSM_850:     return SD_BAND_GSM_850;
    case PRL_GSM_PGSM_900:return SD_BAND_GSM_PGSM_900;
    case PRL_GSM_EGSM_900:return SD_BAND_GSM_EGSM_900;
    case PRL_GSM_RGSM_900:return SD_BAND_GSM_RGSM_900;
    case PRL_GSM_DCS_1800:return SD_BAND_GSM_DCS_1800;
    case PRL_GSM_PCS_1900:return SD_BAND_GSM_PCS_1900;

    case PRL_GSM_BAND_MAX:
    default:
      return SD_BAND_MAX;
  } /* switch */

} /* prl_map_gsm_band_to_band */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_wcdma_band_to_band

DESCRIPTION
  Map PRL WCDMA bands to corresponding SD bands.

DEPENDENCIES
  None.

RETURN VALUE
  SD band.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_band_e_type             prl_map_wcdma_band_to_band(

        prl_wcdma_band_e_type      wcdma_band
           /* WCDMA band to map.
           */
)
{
  /* Map PRL WCDMA bands to corresponding SD bands.
  */
  switch (wcdma_band) {

    case PRL_WCDMA_I_IMT_2000:    return SD_BAND_WCDMA_I_IMT_2000;
    case PRL_WCDMA_II_PCS_1900:   return SD_BAND_WCDMA_II_PCS_1900;
    case PRL_WCDMA_III_1700:      return SD_BAND_WCDMA_III_1700;
    case PRL_WCDMA_IV_1700:       return SD_BAND_WCDMA_IV_1700;
    case PRL_WCDMA_V_850:         return SD_BAND_WCDMA_V_850;
    case PRL_WCDMA_VI_800:        return SD_BAND_WCDMA_VI_800;
    case PRL_WCDMA_VII_2600:      return SD_BAND_WCDMA_VII_2600;
    case PRL_WCDMA_VIII_900:      return SD_BAND_WCDMA_VIII_900;
    case PRL_WCDMA_IX_1700:       return SD_BAND_WCDMA_IX_1700;
    case PRL_WCDMA_XI_1500:       return SD_BAND_WCDMA_XI_1500;
    case PRL_WCDMA_XIX_850:       return SD_BAND_WCDMA_XIX_850;

    case PRL_WCDMA_BAND_MAX:
    default:
      return SD_BAND_MAX;

  } /* switch */

} /* prl_map_wcdma_band_to_band */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_lte_band_to_band

DESCRIPTION
  Map PRL LTE bands to corresponding SD bands.

DEPENDENCIES
  None.

RETURN VALUE
  SD band.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_band_e_type             prl_map_lte_band_to_band(

        prl_lte_band_e_type      lte_band
           /* LTE band to map.
           */
)
{
  /* Map PRL LTE bands to corresponding SD bands.
  */
  switch (lte_band)
  {
    case PRL_LTE_EUTRAN_1:        return SD_BAND_LTE_EUTRAN_BAND1;
    case PRL_LTE_EUTRAN_2:        return SD_BAND_LTE_EUTRAN_BAND2;
    case PRL_LTE_EUTRAN_3:        return SD_BAND_LTE_EUTRAN_BAND3;
    case PRL_LTE_EUTRAN_4:        return SD_BAND_LTE_EUTRAN_BAND4;
    case PRL_LTE_EUTRAN_5:        return SD_BAND_LTE_EUTRAN_BAND5;
    case PRL_LTE_EUTRAN_6:        return SD_BAND_LTE_EUTRAN_BAND6;
    case PRL_LTE_EUTRAN_7:        return SD_BAND_LTE_EUTRAN_BAND7;
    case PRL_LTE_EUTRAN_8:        return SD_BAND_LTE_EUTRAN_BAND8;
    case PRL_LTE_EUTRAN_9:        return SD_BAND_LTE_EUTRAN_BAND9;
    case PRL_LTE_EUTRAN_10:       return SD_BAND_LTE_EUTRAN_BAND10;
    case PRL_LTE_EUTRAN_11:       return SD_BAND_LTE_EUTRAN_BAND11;
    case PRL_LTE_EUTRAN_12:       return SD_BAND_LTE_EUTRAN_BAND12;
    case PRL_LTE_EUTRAN_13:       return SD_BAND_LTE_EUTRAN_BAND13;
    case PRL_LTE_EUTRAN_14:       return SD_BAND_LTE_EUTRAN_BAND14;
    case PRL_LTE_EUTRAN_17:       return SD_BAND_LTE_EUTRAN_BAND17;
    case PRL_LTE_EUTRAN_18:       return SD_BAND_LTE_EUTRAN_BAND18;
    case PRL_LTE_EUTRAN_19:       return SD_BAND_LTE_EUTRAN_BAND19;
    case PRL_LTE_EUTRAN_20:       return SD_BAND_LTE_EUTRAN_BAND20;
    case PRL_LTE_EUTRAN_21:       return SD_BAND_LTE_EUTRAN_BAND21;
    case PRL_LTE_EUTRAN_23:       return SD_BAND_LTE_EUTRAN_BAND23;
    case PRL_LTE_EUTRAN_24:       return SD_BAND_LTE_EUTRAN_BAND24;
    case PRL_LTE_EUTRAN_25:       return SD_BAND_LTE_EUTRAN_BAND25;
    case PRL_LTE_EUTRAN_26:       return SD_BAND_LTE_EUTRAN_BAND26;
    case PRL_LTE_EUTRAN_27:       return SD_BAND_LTE_EUTRAN_BAND27;
    case PRL_LTE_EUTRAN_28:       return SD_BAND_LTE_EUTRAN_BAND28;
    case PRL_LTE_EUTRAN_29:       return SD_BAND_LTE_EUTRAN_BAND29;
    case PRL_LTE_EUTRAN_30:       return SD_BAND_LTE_EUTRAN_BAND30;
    case PRL_LTE_EUTRAN_31:       return SD_BAND_LTE_EUTRAN_BAND31;
    case PRL_LTE_EUTRAN_32:       return SD_BAND_LTE_EUTRAN_BAND32;
    case PRL_LTE_EUTRAN_33:       return SD_BAND_LTE_EUTRAN_BAND33;
    case PRL_LTE_EUTRAN_34:       return SD_BAND_LTE_EUTRAN_BAND34;
    case PRL_LTE_EUTRAN_35:       return SD_BAND_LTE_EUTRAN_BAND35;
    case PRL_LTE_EUTRAN_36:       return SD_BAND_LTE_EUTRAN_BAND36;
    case PRL_LTE_EUTRAN_37:       return SD_BAND_LTE_EUTRAN_BAND37;
    case PRL_LTE_EUTRAN_38:       return SD_BAND_LTE_EUTRAN_BAND38;
    case PRL_LTE_EUTRAN_39:       return SD_BAND_LTE_EUTRAN_BAND39;
    case PRL_LTE_EUTRAN_40:       return SD_BAND_LTE_EUTRAN_BAND40;
    case PRL_LTE_EUTRAN_41:       return SD_BAND_LTE_EUTRAN_BAND41;
    case PRL_LTE_EUTRAN_42:       return SD_BAND_LTE_EUTRAN_BAND42;
    case PRL_LTE_EUTRAN_43:       return SD_BAND_LTE_EUTRAN_BAND43;
    case PRL_LTE_EUTRAN_66:       return SD_BAND_LTE_EUTRAN_BAND66;

    case PRL_LTE_BAND_MAX:
    default:
      return SD_BAND_MAX;

  } /* switch */

} /* prl_map_lte_band_to_band */


/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_tds_band_to_band

DESCRIPTION
  Map PRL TD-SCDMA bands to corresponding SD bands.

DEPENDENCIES
  None.

RETURN VALUE
  SD band.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_band_e_type             prl_map_tds_band_to_band(

        prl_tds_band_e_type      tds_band
           /* TD-SCDMA band to map.
           */
)
{
  /* Map PRL TD-SCDMA bands to corresponding SD bands.
  */
  switch (tds_band) {

    case PRL_TDS_BANDA:    return SD_BAND_TDS_BANDA;
    case PRL_TDS_BANDB:    return SD_BAND_TDS_BANDB;
    case PRL_TDS_BANDC:    return SD_BAND_TDS_BANDC;
    case PRL_TDS_BANDD:    return SD_BAND_TDS_BANDD;
    case PRL_TDS_BANDE:    return SD_BAND_TDS_BANDE;
    case PRL_TDS_BANDF:    return SD_BAND_TDS_BANDF;    

    case PRL_TDS_BAND_MAX:
    default:
      return SD_BAND_MAX;

  } /* switch */

} /* prl_map_tds_band_to_band */





/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_band_to_gsm_band

DESCRIPTION
  Map PRL GSM bands to corresponding SD bands.

DEPENDENCIES
  None.

RETURN VALUE
  SD band.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  prl_gsm_band_e_type              prl_map_band_to_gsm_band(

        sd_band_e_type        gsm_band
           /* GSM band to map.
           */
)
{
  /* Map PRL GSM bands to corresponding SD bands.
  */
  switch( gsm_band )
  {
    case SD_BAND_GSM_450 :     return PRL_GSM_450;
    case SD_BAND_GSM_480:     return PRL_GSM_480;
    case SD_BAND_GSM_750:     return PRL_GSM_750;
    case SD_BAND_GSM_850:     return PRL_GSM_850;
	case SD_BAND_GSM_PGSM_900:return PRL_GSM_PGSM_900;
	case SD_BAND_GSM_EGSM_900:return PRL_GSM_EGSM_900;
    case SD_BAND_GSM_RGSM_900:return PRL_GSM_RGSM_900;
	case SD_BAND_GSM_DCS_1800:return PRL_GSM_DCS_1800;
    case SD_BAND_GSM_PCS_1900:return PRL_GSM_PCS_1900;

    default:
      return PRL_GSM_BAND_MAX;
  } /* switch */

} /* prl_map_gsm_band_to_band */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_band_to_wcdma_band

DESCRIPTION
  Map PRL WCDMA bands to corresponding SD bands.

DEPENDENCIES
  None.

RETURN VALUE
  SD band.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  prl_wcdma_band_e_type              prl_map_band_to_wcdma_band(

        sd_band_e_type      wcdma_band
           /* WCDMA band to map.
           */
)
{
  /* Map PRL WCDMA bands to corresponding SD bands.
  */
  switch (wcdma_band) {

    case SD_BAND_WCDMA_I_IMT_2000:    return PRL_WCDMA_I_IMT_2000;
    case SD_BAND_WCDMA_II_PCS_1900:   return PRL_WCDMA_II_PCS_1900;
    case SD_BAND_WCDMA_III_1700:      return PRL_WCDMA_III_1700;
    case SD_BAND_WCDMA_IV_1700:       return PRL_WCDMA_IV_1700;
    case SD_BAND_WCDMA_V_850:         return PRL_WCDMA_V_850;
    case SD_BAND_WCDMA_VI_800:        return PRL_WCDMA_VI_800;
    case SD_BAND_WCDMA_VII_2600:      return PRL_WCDMA_VII_2600;
    case SD_BAND_WCDMA_VIII_900:      return PRL_WCDMA_VIII_900;
    case SD_BAND_WCDMA_IX_1700:       return PRL_WCDMA_IX_1700;
    case SD_BAND_WCDMA_XI_1500:       return PRL_WCDMA_XI_1500;
    case SD_BAND_WCDMA_XIX_850:       return PRL_WCDMA_XIX_850;

    default:
      return PRL_WCDMA_BAND_MAX;

  } /* switch */

} /* prl_map_wcdma_band_to_band */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_band_to_lte_band

DESCRIPTION
  Map PRL LTE bands to corresponding SD bands.

DEPENDENCIES
  None.

RETURN VALUE
  SD band.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  prl_lte_band_e_type              prl_map_band_to_lte_band(

            sd_band_e_type    lte_band
           /* LTE band to map.
           */
)
{
  /* Map PRL LTE bands to corresponding SD bands.
  */
  switch (lte_band)
  {
    case SD_BAND_LTE_EUTRAN_BAND1:        return PRL_LTE_EUTRAN_1;
    case SD_BAND_LTE_EUTRAN_BAND2:        return PRL_LTE_EUTRAN_2;
    case SD_BAND_LTE_EUTRAN_BAND3:        return PRL_LTE_EUTRAN_3;
    case SD_BAND_LTE_EUTRAN_BAND4:        return PRL_LTE_EUTRAN_4;
    case SD_BAND_LTE_EUTRAN_BAND5:        return PRL_LTE_EUTRAN_5;
    case SD_BAND_LTE_EUTRAN_BAND6:        return PRL_LTE_EUTRAN_6;
    case SD_BAND_LTE_EUTRAN_BAND7:        return PRL_LTE_EUTRAN_7;
    case SD_BAND_LTE_EUTRAN_BAND8:        return PRL_LTE_EUTRAN_8;
    case SD_BAND_LTE_EUTRAN_BAND9:        return PRL_LTE_EUTRAN_9;
    case SD_BAND_LTE_EUTRAN_BAND10:       return PRL_LTE_EUTRAN_10;
    case SD_BAND_LTE_EUTRAN_BAND11:       return PRL_LTE_EUTRAN_11;
    case SD_BAND_LTE_EUTRAN_BAND12:       return PRL_LTE_EUTRAN_12;
    case SD_BAND_LTE_EUTRAN_BAND13:       return PRL_LTE_EUTRAN_13;
    case SD_BAND_LTE_EUTRAN_BAND14:       return PRL_LTE_EUTRAN_14;
    case SD_BAND_LTE_EUTRAN_BAND17:       return PRL_LTE_EUTRAN_17;
    case SD_BAND_LTE_EUTRAN_BAND18:       return PRL_LTE_EUTRAN_18;
    case SD_BAND_LTE_EUTRAN_BAND19:       return PRL_LTE_EUTRAN_19;
    case SD_BAND_LTE_EUTRAN_BAND20:       return PRL_LTE_EUTRAN_20;
    case SD_BAND_LTE_EUTRAN_BAND21:       return PRL_LTE_EUTRAN_21;
    case SD_BAND_LTE_EUTRAN_BAND23:       return PRL_LTE_EUTRAN_23;
    case SD_BAND_LTE_EUTRAN_BAND24:       return PRL_LTE_EUTRAN_24;
    case SD_BAND_LTE_EUTRAN_BAND25:       return PRL_LTE_EUTRAN_25;
    case SD_BAND_LTE_EUTRAN_BAND26:       return PRL_LTE_EUTRAN_26;
    case SD_BAND_LTE_EUTRAN_BAND27:       return PRL_LTE_EUTRAN_27;
    case SD_BAND_LTE_EUTRAN_BAND28:       return PRL_LTE_EUTRAN_28;
    case SD_BAND_LTE_EUTRAN_BAND29:       return PRL_LTE_EUTRAN_29;
    case SD_BAND_LTE_EUTRAN_BAND30:       return PRL_LTE_EUTRAN_30;
    case SD_BAND_LTE_EUTRAN_BAND31:       return PRL_LTE_EUTRAN_31;
    case SD_BAND_LTE_EUTRAN_BAND32:       return PRL_LTE_EUTRAN_32;
    case SD_BAND_LTE_EUTRAN_BAND33:       return PRL_LTE_EUTRAN_33;
    case SD_BAND_LTE_EUTRAN_BAND34:       return PRL_LTE_EUTRAN_34;
    case SD_BAND_LTE_EUTRAN_BAND35:       return PRL_LTE_EUTRAN_35;
    case SD_BAND_LTE_EUTRAN_BAND36:       return PRL_LTE_EUTRAN_36;
    case SD_BAND_LTE_EUTRAN_BAND37:       return PRL_LTE_EUTRAN_37;
    case SD_BAND_LTE_EUTRAN_BAND38:       return PRL_LTE_EUTRAN_38;
    case SD_BAND_LTE_EUTRAN_BAND39:       return PRL_LTE_EUTRAN_39;
    case SD_BAND_LTE_EUTRAN_BAND40:       return PRL_LTE_EUTRAN_40;
    case SD_BAND_LTE_EUTRAN_BAND41:       return PRL_LTE_EUTRAN_41;
    case SD_BAND_LTE_EUTRAN_BAND42:       return PRL_LTE_EUTRAN_42;
    case SD_BAND_LTE_EUTRAN_BAND43:       return PRL_LTE_EUTRAN_43;
    case SD_BAND_LTE_EUTRAN_BAND66:       return PRL_LTE_EUTRAN_66;

    default:
      return PRL_LTE_BAND_MAX;

  } /* switch */

} /* prl_map_lte_band_to_band */


/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_band_to_tds_band

DESCRIPTION
  Map PRL TD-SCDMA bands to corresponding SD bands.

DEPENDENCIES
  None.

RETURN VALUE
  SD band.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  prl_tds_band_e_type             prl_map_band_to_tds_band(

        sd_band_e_type      tds_band
           /* TD-SCDMA band to map.
           */
)
{
  /* Map PRL TD-SCDMA bands to corresponding SD bands.
  */
  switch (tds_band) {

    case SD_BAND_TDS_BANDA:    return PRL_TDS_BANDA;
    case SD_BAND_TDS_BANDB:    return PRL_TDS_BANDB;
    case SD_BAND_TDS_BANDC:    return PRL_TDS_BANDC;
    case SD_BAND_TDS_BANDD:    return PRL_TDS_BANDD;
    case SD_BAND_TDS_BANDE:    return PRL_TDS_BANDE;
    case SD_BAND_TDS_BANDF:    return PRL_TDS_BANDF;    

    default:
      return PRL_TDS_BAND_MAX;

  } /* switch */

} /* prl_map_tds_band_to_band */

/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_acq_rec_to_sd_mode

DESCRIPTION
  Map acquisition record type to SD mode.

DEPENDENCIES
  None.

RETURN VALUE
  SD mode value corresponding to input PRL system record type.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_mode_e_type            prl_map_acq_rec_to_sd_mode(

        prl_acq_rec_type          acq_rec
            /* PRL acquisition record type which to convert.
            */
)
{
  switch( acq_rec )
  {
    case PRL_ACQ_REC_CELLULAR_ANALOG:       return SD_MODE_AMPS;
    case PRL_ACQ_REC_CELLULAR_CDMA_STD:     return SD_MODE_CDMA;
    case PRL_ACQ_REC_CELLULAR_CDMA_CUST:    return SD_MODE_CDMA;
    case PRL_ACQ_REC_CELLULAR_GENERIC:      return SD_MODE_CDMA;
    case PRL_ACQ_REC_PCS_CDMA_BLOCKS:       return SD_MODE_CDMA;
    case PRL_ACQ_REC_PCS_CDMA_CHANNELS:     return SD_MODE_CDMA;

    /* The following three acq record types apply for IS683B PRL or above.*/
    case PRL_ACQ_REC_JTACS_CDMA_STD:        return SD_MODE_CDMA;
    case PRL_ACQ_REC_JTACS_CDMA_CUST:       return SD_MODE_CDMA;
    case PRL_ACQ_REC_CDMA_2GHZ_CHANS:       return SD_MODE_CDMA;

    case PRL_ACQ_REC_CDMA_GENERIC:          return SD_MODE_CDMA;
    case PRL_ACQ_REC_HDR_GENERIC:           return SD_MODE_HDR;

    case PRL_ACQ_REC_GSM_GENERIC:           return SD_MODE_GSM;
    case PRL_ACQ_REC_WCDMA_GENERIC:         return SD_MODE_WCDMA;

    case PRL_ACQ_REC_GW_PREFERRED:          return SD_MODE_GSM; /* or WCDMA*/

    case PRL_ACQ_REC_GWL_GENERIC:           return SD_MODE_GWL;
    case PRL_ACQ_REC_RESERVED:
    case PRL_ACQ_REC_NONE:
    default:
      return SD_MODE_MAX;
  }

} /* prl_map_acq_rec_to_sd_mode */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_get_acq_tbl_ptr

DESCRIPTION
  Return a pointer to the PRL's acq table cache if is_reset = false.
  If is_reset = TRUE, this function will deallocate the dyn memory
  allocated for the acq table cache.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the to the PRL's acq table cache

SIDE EFFECTS
  None.

===========================================================================*/
prli_acq_rec_tbl_s_type   *prl_get_acq_tbl_ptr(

       boolean  is_reset
        /* Reset the buffer Pointer
        */
)
{

  static prli_acq_rec_tbl_s_type * tbl_ptr = NULL;

  /* Clear the buffer PTR after clearing the buffer
  */
  if (is_reset)
  {
    if(tbl_ptr)
    {
      modem_mem_free((void *) tbl_ptr, MODEM_MEM_CLIENT_MMODE);
      tbl_ptr = NULL;
    }
    return NULL;
  }

  if(!tbl_ptr)
  {
    tbl_ptr = (prli_acq_rec_tbl_s_type *)\
                            modem_mem_alloc( sizeof(prli_acq_rec_tbl_s_type),
                                                     MODEM_MEM_CLIENT_MMODE);
    if(tbl_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }


    /* Initialize acq table
    */
    memset(tbl_ptr,0,sizeof(prli_acq_rec_tbl_s_type));
    tbl_ptr->num_entries = 0;
  }
  return tbl_ptr;

}

/* <EJECT> */
/*===========================================================================

FUNCTION prl_get_enc_sys_tbl_ptr

DESCRIPTION
  Return a pointer to the PRL's encoded sys table cache if is_reset = false.
  If is_reset = TRUE, this function will deallocate the dyn memory
  allocated for the encoded sys table cache.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the to the PRL's encoded sys table cache

SIDE EFFECTS
  None.

===========================================================================*/
prli_sys_rec_enc_tbl_s_type   *prl_get_enc_sys_tbl_ptr(

       boolean  is_reset
        /* Reset the buffer Pointer
        */
)
{
  static prli_sys_rec_enc_tbl_s_type  *enc_tbl_ptr = NULL;

  /* Clear the buffer PTR after clearing the buffer
  */
  if (is_reset)
  {
    if(enc_tbl_ptr != NULL)
    {
      SD_MSG_MED_1("Free mem for enc_tbl, sys_enc_tbl:%x",enc_tbl_ptr->sys_rec_tbl);
      if(enc_tbl_ptr->sys_rec_tbl != NULL)
      {
        modem_mem_free((void *)enc_tbl_ptr->sys_rec_tbl,
                       MODEM_MEM_CLIENT_MMODE);
      }
      modem_mem_free((void *)enc_tbl_ptr,
                     MODEM_MEM_CLIENT_MMODE);
      enc_tbl_ptr = NULL;
    }
    return NULL;
  }

  if(enc_tbl_ptr == NULL)
  {
    /* allocate memory to enc table structure
    */
    enc_tbl_ptr  = (prli_sys_rec_enc_tbl_s_type*)
                    modem_mem_alloc( sizeof(prli_sys_rec_enc_tbl_s_type),
                                     MODEM_MEM_CLIENT_MMODE);
    if(enc_tbl_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }


    /* initialize the fields
    */
    enc_tbl_ptr->offset      = 0;
    enc_tbl_ptr->num_entries = 0;

    /* allocate memory for the actual encoded data
    */
    enc_tbl_ptr->sys_rec_tbl  = (byte*)modem_mem_alloc(PRL_ENC_SYS_TBL_SIZE,
                                                    MODEM_MEM_CLIENT_MMODE);
    if(enc_tbl_ptr->sys_rec_tbl == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }
    /* Initialize sys table
    */


    memset(enc_tbl_ptr->sys_rec_tbl,
           0,
           PRL_ENC_SYS_TBL_SIZE);
  }
  return enc_tbl_ptr;
}

/* <EJECT> */
/*===========================================================================

FUNCTION prl_get_sys_tbl_ptr

DESCRIPTION
  Return a pointer to the PRL's sys table cache if is_reset = false.
  If is_reset = TRUE, this function will deallocate the dyn memory
  allocated for the sys table cache.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the to the PRL's sys table cache

SIDE EFFECTS
  None.

===========================================================================*/
prli_sys_rec_tbl_s_type   *prl_get_sys_tbl_ptr(

       boolean  is_reset
        /* Reset the buffer Pointer
        */
)
{

  static prli_sys_rec_tbl_s_type * tbl_ptr = NULL;

  /* Clear the buffer PTR after clearing the buffer
  */
  if (is_reset)
  {
    if(tbl_ptr)
    {
      modem_mem_free((void *) tbl_ptr, MODEM_MEM_CLIENT_MMODE);
      tbl_ptr = NULL;
    }
    return NULL;
  }

  if(!tbl_ptr)
  {
    tbl_ptr = (prli_sys_rec_tbl_s_type *)\
                            modem_mem_alloc( sizeof(prli_sys_rec_tbl_s_type),
                                                     MODEM_MEM_CLIENT_MMODE);
    if(tbl_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }

    /* Initialize sys table
    */
    memset(tbl_ptr,0,sizeof(prli_sys_rec_tbl_s_type));
    tbl_ptr->num_entries = 0;
  }
  return tbl_ptr;

}




/* <EJECT> */
/*===========================================================================

FUNCTION prl_get_ptr

DESCRIPTION
  Return a pointer to the PRL.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the to the PRL.

SIDE EFFECTS
  None.

===========================================================================*/
prl_ram_roaming_list_type   *prl_get_ptr( void )
{
  return &prl;
}




/* <EJECT> */
/*===========================================================================

FUNCTION prl_buf_get_ptr

DESCRIPTION
  Return a pointer to the PRL buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the to the PRL buffer.

SIDE EFFECTS
  None.

===========================================================================*/
byte          *prl_buf_get_ptr( void )
{
  return (byte*) ( prl_get_ptr()->roaming_list + prl_offset );
}




/* <EJECT> */
/*===========================================================================

FUNCTION prl_hdr_get_ptr

DESCRIPTION
  Return pointer to a structure containing the header information of the
  currently loaded PRL.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to a structure containing the header information of the currently
  loaded PRL.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  prl_hdr_s_type      *prl_hdr_get_ptr( void )
{
  return &prl_hdr;
}




/* <EJECT> */
/*===========================================================================

FUNCTION prl_hdr_nv_get_ptr

DESCRIPTION
  Return pointer to a structure containing the header information of the
  PRL that is stored in NV.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to a structure containing the header information of the PRL that is
  stored in NV.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  const prl_hdr_s_type      *prl_hdr_nv_get_ptr( void )
{
  return &prl_hdr_nv;
}

/* <EJECT> */
/*===========================================================================

FUNCTION prl_get_nv_prl_size

DESCRIPTION
  Return the max size of the PRL that is stored in NV.

DEPENDENCIES
  None.

RETURN VALUE
  The max size of the PRL that is stored in NV.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  unsigned int              prl_get_nv_prl_size( void )
{
  unsigned int nv_prl_size = nv_max_size_of_roaming_list();

  if (!nv_prl_size)
  {
    nv_prl_size = NV_ROAMING_LIST_MAX_SIZE;
  }
  return(sizeof(nv_roaming_list_header_type) + \
         nv_prl_size + 8);
}

/* <EJECT> */
/*===========================================================================

FUNCTION prl_extract_acq_rec

DESCRIPTION
  Extract a specified acquisition record from the currently loaded PRL.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if record is successfully extracted. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   prl_extract_acq_rec(

        prli_acq_rec_type         *acq_rec,
            /* Pointer to where to build the acq record.
            */

        int                       index
            /* Index of the acquisition entry to extract.
            */
)
{
  byte              num_chans;
  byte              num_blocks;
  int               i;
  int               acq_rec_index;
  int               invalid_cnt = 0;
  word              offset;
  byte              *buf_ptr;

  byte              rec_len;

  prl_hdr_s_type            *prl_hdr_ptr            = &prl_hdr;

  word chan;                  /* The chan field from an acq_rec */
  prl_ab_type a_b;            /* The a_b field from an sys_rec */
  byte block;                 /* The block field from an acq_rec */
  sd_blksys_e_type  blksys;
  word band_chan;
  sd_mode_e_type mode;
  sd_band_e_type band;
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || \
      defined(FEATURE_SD_LTE)
  prl_wcdma_band_e_type prl_wcdma_band;
  word uarfcn;
  prl_uarfcn_incl_e_type uarfcn_incl;
  #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  ||
            (FEATURE_SD_LTE)*/

  #if defined(FEATURE_SD_LTE) || defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  int j = 0;
  int                   pri_index = 0;
  sys_sys_mode_e_type   sys_mode = SYS_SYS_MODE_NONE;
  prl_gsm_band_e_type   prl_gsm_band;
  prl_lte_band_e_type   prl_lte_band;
  prl_tds_band_e_type   prl_tds_band;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_rec != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize local acquisition record table index and offset per our
  ** current acquisition record table index and offset position while
  ** interrupts are disabled.
  */
  sd_rex_enter_crit_sect();
  acq_rec_index   = prl_acq_rec_index;
  offset          = prl_acq_rec_bit_offset;
  sd_rex_leave_crit_sect();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize buf_ptr to the start of the roaming list
  */
  buf_ptr = (byte*) ( prl.roaming_list + prl_offset );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the requested acquisition record is less than where we are pointing
  ** at the moment, start traversing the acquisition table from the beginning.
  */
  if( acq_rec_index > index)
  {
    acq_rec_index   = 0;
    offset          = prl_hdr_ptr->acq_tbl_bit_offset;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Search forward until we find the desired record.
  */
  while( acq_rec_index != index )
  {
    if (prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3)
    {
      /* Jump over the acquisition record type field.
      */
      offset += ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev);

      /* Unpack the acquisition record length field.
      */
      rec_len = SD_B_UNPACKB( buf_ptr,
                     offset,
                     (word)ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) ) ;
      SD_ASSERT( rec_len > 0 );

      /* Jump over the acquisition record length field + the length of the
      ** acquisition record (in bits).
      */
      offset += (ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) + rec_len*8);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    }
    else
    {

      /* Unpack the acquisition record type.
      */
      acq_rec->acq_type = (prl_acq_rec_type) SD_B_UNPACKB( buf_ptr,
                        offset,
                        (word)ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev) );
      offset += (word)ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      switch( acq_rec->acq_type )
      {
        case PRL_ACQ_REC_RESERVED :         /* Reserved */
          SD_MSG_LOW_0("Reserved acq_type");
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CELLULAR_ANALOG :  /* Cellular analog */
          offset += sizeof(prl_cellular_analog_type);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CELLULAR_CDMA_STD:
          offset += sizeof(prl_cellular_cdma_type);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CELLULAR_CDMA_CUST:
          num_chans = SD_B_UNPACKB(
                          buf_ptr,
                          offset,
                          FSIZ(prl_cellular_cdma_custom_type,num_chans) );

          offset += FSIZ(prl_cellular_cdma_custom_type, num_chans);
          offset += num_chans * FSIZ(prl_cellular_cdma_custom_type, chan);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CELLULAR_GENERIC : /* Cellular generic */
          offset += sizeof(prl_cellular_generic_type);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_PCS_CDMA_BLOCKS :  /* PCS CDMA using blocks */
          num_blocks = SD_B_UNPACKB(
                          buf_ptr,
                          offset,
                          FSIZ(prl_pcs_cdma_using_blocks_type, num_blocks) );

          offset += FSIZ(prl_pcs_cdma_using_blocks_type, num_blocks);
          offset += num_blocks * FSIZ(prl_pcs_cdma_using_blocks_type, block);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_PCS_CDMA_CHANNELS :/* PCS CDMA using channels */
          num_chans = SD_B_UNPACKB(
                          buf_ptr,
                          offset,
                          FSIZ(prl_pcs_cdma_using_chans_type, num_chans) );

          offset += FSIZ(prl_pcs_cdma_using_chans_type, num_chans);
          offset += num_chans * FSIZ(prl_pcs_cdma_using_chans_type, chan);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* The following three acq record types apply for IS683B PRL or above
        */

        case PRL_ACQ_REC_JTACS_CDMA_STD:
          offset +=sizeof(prl_jtacs_cdma_std_type);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_JTACS_CDMA_CUST:
          num_chans = SD_B_UNPACKB(
                          buf_ptr,
                          offset,
                          FSIZ(prl_jtacs_cdma_cust_type, num_chans) );

          offset += FSIZ(prl_jtacs_cdma_cust_type, num_chans);
          offset += num_chans * FSIZ(prl_jtacs_cdma_cust_type, chan);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CDMA_2GHZ_CHANS:
          num_chans = SD_B_UNPACKB(
                          buf_ptr,
                          offset,
                          FSIZ(prl_cdma_2ghz_chans_type, num_chans) );

         offset += FSIZ(prl_cdma_2ghz_chans_type, num_chans);
         offset += num_chans * FSIZ(prl_cdma_2ghz_chans_type, chan);
         break;

        case PRL_ACQ_REC_CDMA_GENERIC:
        case PRL_ACQ_REC_HDR_GENERIC:
        case PRL_ACQ_REC_GSM_GENERIC:
        case PRL_ACQ_REC_WCDMA_GENERIC:
        case PRL_ACQ_REC_GW_PREFERRED:
        case PRL_ACQ_REC_GWL_GENERIC:
        case PRL_ACQ_REC_NONE:
        default :
          SD_MSG_LOW_1("Unrecognized acq_type %d", acq_rec->acq_type);
          return FALSE;

      } /* switch */

    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If we reached the end of the acquisition table, return FALSE.
    */
    if( ++acq_rec_index >= prl_hdr_ptr->num_acq_recs )
    {
      return FALSE;
    }

  } /* while( acq_rec_index != index ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Record our current acquisition record table index and offset position
  ** per the local variables while interrupts are disabled.
  */
  sd_rex_enter_crit_sect();
  prl_acq_rec_index       = acq_rec_index;
  prl_acq_rec_bit_offset  = offset;
  sd_rex_leave_crit_sect();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the acquisition record fields.
  */

  /* Extract the ACQ_TYPE field.
  */
  acq_rec->acq_type = (prl_acq_rec_type) SD_B_UNPACKB( buf_ptr,
                        offset,
                        (word)ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev) );
  offset += ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the acquisition record length field.
  */
  rec_len = SD_B_UNPACKB( buf_ptr,
                   offset,
                   (word)ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) ) ;
  SD_ASSERT( (prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3 ) ? rec_len > 0 : rec_len == 0);
  offset += ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the acquisition record type extract the remaining acquisition
  ** record field.
  */
  switch( acq_rec->acq_type )
  {
    case PRL_ACQ_REC_RESERVED:            /* Reserved */
      SD_MSG_LOW_0("Reserved acq record type");
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_ANALOG:     /* Cellular analog */

      a_b = (prl_ab_type)SD_B_UNPACKB( buf_ptr,
                                      offset,
                                      FSIZ(prl_cellular_analog_type, a_b));

      acq_rec->is_valid = TRUE;

      /* Validate the a_b field.
      */
      if (a_b == PRL_AB_RESERVED)
      {
        SD_ERR_0("FAILED - Reserved a_b");
        acq_rec->is_valid = FALSE;
      }

      acq_rec->type.cellular_analog.a_b = a_b;
      offset += FSIZ(prl_cellular_analog_type, a_b);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_STD:   /* Cellular CDMA std */

      a_b = (prl_ab_type)SD_B_UNPACKB( buf_ptr,
                                      offset,
                                      FSIZ(prl_cellular_cdma_type, a_b));

      acq_rec->is_valid = TRUE;

      /* Validate the a_b field.
      */
      if (a_b == PRL_AB_RESERVED)
      {
        SD_ERR_0("FAILED - Reserved a_b");
        acq_rec->is_valid = FALSE;
      }
      acq_rec->type.cellular_cdma.a_b = a_b;
      offset += FSIZ(prl_cellular_cdma_type, a_b);
      acq_rec->type.cellular_cdma.pri_sec = (prl_pri_sec_type)
         SD_B_UNPACKB(buf_ptr, offset, FSIZ(prl_cellular_cdma_type, pri_sec));
      offset += FSIZ(prl_cellular_cdma_type, pri_sec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_CUST:  /* Cellular CDMA custom */
      acq_rec->type.cellular_cdma_cust.num_chans = SD_B_UNPACKB(buf_ptr, offset,
        FSIZ(prl_cellular_cdma_custom_type, num_chans));
      offset += FSIZ(prl_cellular_cdma_custom_type, num_chans);
      acq_rec->is_valid = TRUE;

      invalid_cnt = 0;

      /* Check if the # of channels is valid */
      if(acq_rec->type.cellular_cdma_cust.num_chans >=
         PRL_MAX_CDMA_CUSTOM_CELLULAR_CHAN)
      {
        SD_ERR_2( "# chan's (%d) is >= %d", \
                acq_rec->type.cellular_cdma_cust.num_chans,
                PRL_MAX_CDMA_CUSTOM_CELLULAR_CHAN);
        acq_rec->is_valid = FALSE;
        break;
      }

      for( i = 0; i < acq_rec->type.cellular_cdma_cust.num_chans; i++ )
      {
        chan = SD_B_UNPACKW( buf_ptr,
                          offset,
                          FSIZ(prl_cellular_cdma_custom_type, chan));
        offset += FSIZ(prl_cellular_cdma_custom_type, chan);

        blksys = sd_map_chan_to_blksys( SD_MODE_CDMA,
                                           SD_BAND_CELL,
                                           chan );
        if( blksys == SD_BLKSYS_MAX )
        {
          SD_ERR_1("FAILED - Invalid CDMA channel %d in Cellular Band",
                  chan);
          invalid_cnt++;
          continue;
        }
        if((i - invalid_cnt) >= 0)
        {
          acq_rec->type.cellular_cdma_cust.chan[i - invalid_cnt] = chan;
        }
      }
      acq_rec->type.cellular_cdma_cust.num_chans -=invalid_cnt;

      if( acq_rec->type.cellular_cdma_cust.num_chans == 0 )
      {
        acq_rec->is_valid = FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_GENERIC:    /* Cellular generic */

      a_b = (prl_ab_type)SD_B_UNPACKB( buf_ptr,
                                      offset,
                                      FSIZ(prl_cellular_generic_type, a_b));
      acq_rec->is_valid = TRUE;

      /* Validate the a_b field
      */
      if (a_b == PRL_AB_RESERVED)
      {
        SD_ERR_0("FAILED - Reserved a_b");
        acq_rec->is_valid = FALSE;
      }
      offset += sizeof(prl_cellular_generic_type);

      acq_rec->type.cellular_generic.a_b = a_b;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_BLOCKS:     /* PCS CDMA using blocks */
      num_blocks = SD_B_UNPACKB(buf_ptr, offset,
        FSIZ(prl_pcs_cdma_using_blocks_type, num_blocks));
      offset += FSIZ(prl_pcs_cdma_using_blocks_type, num_blocks);
      acq_rec->type.pcs_cdma_block.num_blocks = num_blocks;

      acq_rec->is_valid = TRUE;
      invalid_cnt = 0;

      /* validate # of blocks */
      if( num_blocks >= PRL_MAX_CDMA_PCS_BLOCKS )
      {
        SD_ERR_2( "# bands (%d) > %d", num_blocks, PRL_MAX_CDMA_PCS_BLOCKS);
        acq_rec->is_valid = FALSE;
        break;
      }

      /* Validate each block.
      */
      for( i = 0; i < num_blocks; i++ )
      {
        block = SD_B_UNPACKB( buf_ptr,
                           offset,
                           FSIZ(prl_pcs_cdma_using_blocks_type, block));
        offset += FSIZ(prl_pcs_cdma_using_blocks_type, block);

        if (block == PRL_BLOCK_RESERVED)
        {
          SD_ERR_0("FAILED - Reserved block");
          invalid_cnt++;
          continue;
        }
        if((i - invalid_cnt) >= 0)
        {
          acq_rec->type.pcs_cdma_block.block[i - invalid_cnt] = block;
        }
      }
      acq_rec->type.pcs_cdma_block.num_blocks -=invalid_cnt;
      if( acq_rec->type.pcs_cdma_block.num_blocks == 0 )
      {
        acq_rec->is_valid = FALSE;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_CHANNELS:   /* PCS CDMA using channels */
      num_chans = SD_B_UNPACKB(buf_ptr, offset,
        FSIZ(prl_pcs_cdma_using_chans_type, num_chans));
      offset += FSIZ(prl_pcs_cdma_using_chans_type, num_chans);
      acq_rec->type.pcs_cdma_chan.num_chans = num_chans;

      acq_rec->is_valid = TRUE;
      invalid_cnt = 0;

      /* validate # of chan */
      if( num_chans >= PRL_MAX_CDMA_PCS_CHAN )
      {
        SD_ERR_2( "# chans (%d) > %d", num_chans, PRL_MAX_CDMA_PCS_CHAN);
        acq_rec->is_valid = FALSE;
        break;
      }

      for (i = 0; i < num_chans; i++)
      {
        chan = SD_B_UNPACKW( buf_ptr,
                            offset,
                            FSIZ(prl_pcs_cdma_using_chans_type, chan));
        offset += FSIZ(prl_pcs_cdma_using_chans_type, chan);
        blksys = sd_map_chan_to_blksys( SD_MODE_CDMA,
                                           SD_BAND_PCS,
                                           chan );
        if( blksys == SD_BLKSYS_MAX )
        {
          SD_ERR_1("FAILED - Invalid CDMA channel %d in PCS band", chan);
          invalid_cnt++;
          continue;
        }
        if((i - invalid_cnt) >= 0)
        {
          acq_rec->type.pcs_cdma_chan.chan[i - invalid_cnt] = chan;
        }
      }

      acq_rec->type.pcs_cdma_chan.num_chans -=invalid_cnt;
      if( acq_rec->type.pcs_cdma_chan.num_chans == 0 )
      {
        acq_rec->is_valid = FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following three acq record types apply for IS683B PRL or above.*/

    case PRL_ACQ_REC_JTACS_CDMA_STD:

      a_b = (prl_ab_type)SD_B_UNPACKB( buf_ptr,
                                      offset,
                                      FSIZ(prl_jtacs_cdma_std_type, a_b));

      acq_rec->is_valid = TRUE;
      /* Validate the a_b field.
      */
      if (a_b == PRL_AB_RESERVED)
      {
        SD_ERR_0("FAILED - Reserved a_b");
        acq_rec->is_valid = FALSE;
      }

      acq_rec->type.jtacs_cdma_std.a_b = a_b;
      offset += FSIZ(prl_jtacs_cdma_std_type, a_b);
      acq_rec->type.jtacs_cdma_std.pri_sec = (prl_pri_sec_type)
         SD_B_UNPACKB(buf_ptr, offset, FSIZ(prl_jtacs_cdma_std_type, pri_sec));
      offset += FSIZ(prl_jtacs_cdma_std_type, pri_sec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_JTACS_CDMA_CUST:
      num_chans = SD_B_UNPACKB(buf_ptr, offset,
        FSIZ(prl_jtacs_cdma_cust_type, num_chans));
      offset += FSIZ(prl_jtacs_cdma_cust_type, num_chans);
      acq_rec->type.jtacs_cdma_cust.num_chans = num_chans;

      acq_rec->is_valid = TRUE;
      invalid_cnt  = 0;

      /* validate # of chan */
      if( num_chans >= PRL_MAX_CDMA_JTACS_CHAN )
      {
        SD_ERR_2( "# chans (%d) > %d", num_chans, PRL_MAX_CDMA_JTACS_CHAN);
        acq_rec->is_valid = FALSE;
        break;
      }

      for( i = 0; i < num_chans; i++ )
      {

        chan = SD_B_UNPACKW( buf_ptr,
                            offset,
                            FSIZ(prl_jtacs_cdma_cust_type, chan));
        offset += FSIZ(prl_jtacs_cdma_cust_type, chan);

        blksys = sd_map_chan_to_blksys( SD_MODE_CDMA,
                                           SD_BAND_BC3,
                                           chan );

        if( blksys == SD_BLKSYS_MAX )
        {
            SD_ERR_1("FAILED - Invalid CDMA channel %d in Band Class 3",
                    chan);
            invalid_cnt++;
            continue;
        }
        if((i - invalid_cnt)>=0)
        {
          acq_rec->type.jtacs_cdma_cust.chan[i - invalid_cnt] = chan;
        }
      }

      acq_rec->type.jtacs_cdma_cust.num_chans -=invalid_cnt;
      if( acq_rec->type.jtacs_cdma_cust.num_chans == 0 )
      {
        acq_rec->is_valid = FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CDMA_2GHZ_CHANS:
      num_chans = SD_B_UNPACKB(buf_ptr, offset,
        FSIZ(prl_cdma_2ghz_chans_type, num_chans));
      offset += FSIZ(prl_cdma_2ghz_chans_type, num_chans);
      acq_rec->type.cdma_2ghz_chans.num_chans = num_chans;

      acq_rec->is_valid = TRUE;
      invalid_cnt = 0;

      /* validate # of chan */
      if( num_chans >= PRL_MAX_CDMA_2GHZ_CHAN )
      {
        SD_ERR_2( "# chans (%d) > %d", num_chans, PRL_MAX_CDMA_2GHZ_CHAN);
        acq_rec->is_valid = FALSE;
        break;
      }

      for (i = 0; i < num_chans; i++)
      {

        chan = SD_B_UNPACKW( buf_ptr,
                            offset,
                            FSIZ(prl_cdma_2ghz_chans_type, chan));
        offset += FSIZ(prl_cdma_2ghz_chans_type, chan);

        blksys = sd_map_chan_to_blksys( SD_MODE_CDMA,
                                           SD_BAND_BC6,
                                           chan );
        if( blksys == SD_BLKSYS_MAX )
        {
          SD_ERR_1("FAILED - Invalid CDMA channel %d in Band Class 6",
                  chan);
          invalid_cnt++;
          continue;
        }
        if((i - invalid_cnt) >= 0)
        {
          acq_rec->type.cdma_2ghz_chans.chan[i - invalid_cnt] = chan;
        }
      }
      acq_rec->type.cdma_2ghz_chans.num_chans -=invalid_cnt;
      if( acq_rec->type.cdma_2ghz_chans.num_chans == 0 )
      {
        acq_rec->is_valid = FALSE;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following AQC REC types only apply to IS683C PRL and above.
    */

    case PRL_ACQ_REC_CDMA_GENERIC:        /* CDMA Generic */
    case PRL_ACQ_REC_HDR_GENERIC:         /* HDR Generic */

      /* Set the number of records field
      */
      acq_rec->type.cdma_generic.num_recs = rec_len / 2;
      acq_rec->is_valid = TRUE;
      invalid_cnt = 0;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* validate # of recs */
      if( acq_rec->type.cdma_generic.num_recs >= PRL_MAX_CDMA_GENERIC_REC )
      {
        SD_ERR_2( "# of recs (%d) is > %d", \
                acq_rec->type.cdma_generic.num_recs,
                PRL_MAX_CDMA_GENERIC_REC);
        acq_rec->is_valid = FALSE;
        break;
      }

      /* Extract the records.
      */
      for( i=0; i < acq_rec->type.cdma_generic.num_recs; i++ )
      {
        band_chan = SD_B_UNPACKW( buf_ptr,
                                 offset,
                                 FSIZ(prl_cdma_generic_type, rec) );


        band    = PRLI_CDMA_GENERIC_BAND( band_chan );
        chan    = PRLI_CDMA_GENERIC_CHAN( band_chan );

        if( acq_rec->acq_type == PRL_ACQ_REC_CDMA_GENERIC )
        {
          mode = SD_MODE_CDMA;
        }
        else
        {
          mode = SD_MODE_HDR;
        }

        if( sd_map_chan_to_blksys(mode, band, chan) == SD_BLKSYS_MAX )
        {
        
          invalid_cnt++;
           offset += sizeof(prl_cdma_generic_type);
          continue;
        }
        if((i - invalid_cnt)>=0)
        {
          acq_rec->type.cdma_generic.rec[i - invalid_cnt] =
                                    SD_B_UNPACKW(buf_ptr,
                                    offset,
                                    sizeof(prl_cdma_generic_type));
        }
        offset += sizeof(prl_cdma_generic_type);
      }

      acq_rec->type.cdma_generic.num_recs -= invalid_cnt;
      if( acq_rec->type.cdma_generic.num_recs == 0 )
      {
        acq_rec->is_valid = FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following AQC REC types only apply to IS683D PRL and above.
    */

    case PRL_ACQ_REC_GSM_GENERIC:

      #if defined(SD_GSM_SUPPORTED)
      /* Get the number of bands field
      */
      acq_rec->type.gsm_generic.num_bands =
                          SD_B_UNPACKB(buf_ptr,
                                    offset,
                                    FSIZ(prl_gsm_generic_type, num_bands) );
      offset += FSIZ(prl_gsm_generic_type, num_bands);

      acq_rec->is_valid = TRUE;

      if( acq_rec->type.gsm_generic.num_bands == 0 ||
          acq_rec->type.gsm_generic.num_bands >= PRL_MAX_GSM_GENERIC_BAND )
      {
        SD_ERR_1("FAILED - Invalid number of GSM bands: %d",
                     acq_rec->type.gsm_generic.num_bands);
        acq_rec->is_valid = FALSE;
        break;
      }

      /* Get band class field
      */

      for( i=0; i< acq_rec->type.gsm_generic.num_bands; i++)
      {
        acq_rec->type.gsm_generic.band[i] =
                          SD_B_UNPACKB(buf_ptr,
                                    offset,
                                    FSIZ(prl_gsm_generic_type, band) );
        offset += FSIZ(prl_gsm_generic_type, band);
      }
      #endif /* (SD_GSM_SUPPORTED) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_WCDMA_GENERIC:

      #if defined(SD_WCDMA_SUPPORTED) 
      /* Get the number of bands field
      */
      acq_rec->type.wcdma_generic.num_bands =
                            SD_B_UNPACKB(buf_ptr,
                                  offset,
                                  FSIZ(prl_wcdma_generic_type, num_bands) );
      offset += FSIZ(prl_wcdma_generic_type, num_bands);

      acq_rec->is_valid = TRUE;
      invalid_cnt = 0;

      if (acq_rec->type.wcdma_generic.num_bands == 0)
      {
        SD_ERR_0("FAILED - Invalid number of WCDMA bands:0");
        acq_rec->is_valid = FALSE;
      }

      /* Get band class, and channel field
      */
      for( i=0; i< acq_rec->type.wcdma_generic.num_bands; i++)
      {
        prl_wcdma_band =  (prl_wcdma_band_e_type)SD_B_UNPACKB(
                                  buf_ptr,
                                  offset,
                                  (word)FSIZ(prl_wcdma_generic_type, band) );

        band = prl_map_wcdma_band_to_band(prl_wcdma_band);

        offset += FSIZ(prl_wcdma_generic_type, band);

        uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                               buf_ptr,
                               offset,
                               (word)FSIZ(prl_wcdma_generic_type, uarfcn_incl) );

        offset += FSIZ(prl_wcdma_generic_type, uarfcn_incl);

        if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
        {
          SD_ERR_1("FAILED - Invalid UARFCN_INCL of WCDMA bands %d",
                  uarfcn_incl);
          invalid_cnt++;
          continue;

        }

        if (uarfcn_incl == PRL_UARFCN_INCL)
        {
          uarfcn = SD_B_UNPACKW( buf_ptr,
                               offset,
                              FSIZ(prl_wcdma_generic_type, uarfcn) );

          offset += FSIZ(prl_wcdma_generic_type, uarfcn);

          if( sd_map_chan_to_blksys(SD_MODE_WCDMA,band,uarfcn) ==
              SD_BLKSYS_MAX )
          {
            SD_ERR_2("FAILED - Invalid WCDMA band %d and chan %d ",
                    band, uarfcn);
            invalid_cnt++;
            continue;
          }

         acq_rec->type.wcdma_generic.rec[i - invalid_cnt].uarfcn = uarfcn;

        }
        else
        {
          acq_rec->type.wcdma_generic.rec[i - invalid_cnt].uarfcn = PRL_WILDCARD_UARFCN;
        }

        acq_rec->type.wcdma_generic.rec[i - invalid_cnt].band = (int)prl_wcdma_band;
        acq_rec->type.wcdma_generic.rec[i - invalid_cnt].uarfcn_incl = uarfcn_incl;

      } /* for */

      acq_rec->type.wcdma_generic.num_bands -= invalid_cnt;
      if( acq_rec->type.wcdma_generic.num_bands == 0 )
      {
        acq_rec->is_valid = FALSE;
      }

      #endif /* (SD_WCDMA_SUPPORTED) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GW_PREFERRED:


      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
      acq_rec->is_valid = TRUE;

      /* Get preferred system indicator and number of gsm/wcdma band.
      */
      acq_rec->type.gw_pref.pref_sys_ind =
         (prl_gw_pref_sys_ind_e_type)SD_B_UNPACKB(
                                     buf_ptr,
                                     offset,
                                     FSIZ(prl_gw_pref_type, pref_sys_ind) );
      offset += FSIZ(prl_gw_pref_type, pref_sys_ind);

      if (acq_rec->type.gw_pref.pref_sys_ind >= PRL_GW_PREF_SYS_IND_RESERVED)
      {
        SD_ERR_1( "FAILED - Invalid GW preferred sys indicator %d",
                   acq_rec->type.gw_pref.pref_sys_ind);
        acq_rec->is_valid = FALSE;
      }

      acq_rec->type.gw_pref.num_gsm_bands = SD_B_UNPACKB(
                                    buf_ptr,
                                    offset,
                                    FSIZ(prl_gw_pref_type, num_gsm_bands) );
      offset += FSIZ(prl_gw_pref_type, num_gsm_bands);

      acq_rec->type.gw_pref.num_wcdma_bands = SD_B_UNPACKB(
                                  buf_ptr,
                                  offset,
                                  FSIZ(prl_gw_pref_type, num_wcdma_bands) );
      offset += FSIZ(prl_gw_pref_type, num_wcdma_bands);

      if ( acq_rec->type.gw_pref.num_gsm_bands == 0 &&
           acq_rec->type.gw_pref.num_wcdma_bands == 0 )
      {
        SD_ERR_1("FAILED - Invalid number of GSM/WCDMA bands %d", acq_rec->type.gw_pref.num_gsm_bands);
        acq_rec->is_valid = FALSE;
      }

      /* Get GSM records: band classes
      */
      for( i=0;
           acq_rec->type.gw_pref.num_gsm_bands < PRL_MAX_GSM_GENERIC_BAND &&
           i<acq_rec->type.gw_pref.num_gsm_bands;
           i++
         )
      {
        acq_rec->type.gw_pref.gsm_band[i] = SD_B_UNPACKB(
                                           buf_ptr,
                                           offset,
                                           FSIZ(prl_gw_pref_type,gsm_band) );
        offset += FSIZ(prl_gw_pref_type,gsm_band);
      }

      /* If coming here, there is GSM bands in the rec. Reset the counter.
      */
      invalid_cnt = 0;

      /* Get WCDMA records: band classes and channels
      */
      for( i=0;
           acq_rec->type.gw_pref.num_wcdma_bands < PRL_MAX_GWL_GENERIC_BAND &&
           i<acq_rec->type.gw_pref.num_wcdma_bands;
           i++
         )
      {

        prl_wcdma_band = (prl_wcdma_band_e_type)SD_B_UNPACKB(
                                        buf_ptr,
                                        offset,
                                        FSIZ(prl_gw_pref_type, wcdma_band) );

        offset += FSIZ(prl_gw_pref_type, wcdma_band);

        band = prl_map_wcdma_band_to_band(prl_wcdma_band);

        uarfcn_incl =
        (prl_uarfcn_incl_e_type)SD_B_UNPACKB( buf_ptr,
                                       offset,
                                       FSIZ(prl_gw_pref_type, uarfcn_incl) );

        offset += FSIZ(prl_gw_pref_type, uarfcn_incl);


        if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
        {
          SD_ERR_1("FAILED - Invalid UARFCN_INCL of WCDMA bands %d",
                  uarfcn_incl);
          invalid_cnt++;
          continue;

        }

        /* If channel is included, get channel field
        ** O/W set channel to wildcard.
        */

        if (uarfcn_incl == PRL_UARFCN_INCL)
        {
          uarfcn = SD_B_UNPACKW( buf_ptr,
                               offset,
                              FSIZ(prl_wcdma_generic_type, uarfcn) );

          offset += FSIZ(prl_gw_pref_type, uarfcn);

          if( sd_map_chan_to_blksys(SD_MODE_WCDMA,band,uarfcn) ==
              SD_BLKSYS_MAX )
          {
            SD_ERR_2("FAILED - Invalid WCDMA band %d and chan %d ",
                    band, uarfcn);
            invalid_cnt++;
            continue;
          }

         acq_rec->type.gw_pref.wcdma_rec[i - invalid_cnt].uarfcn = uarfcn;

        }
        else
        {
          acq_rec->type.gw_pref.wcdma_rec[i - invalid_cnt].uarfcn = PRL_WILDCARD_UARFCN;
        }


        acq_rec->type.gw_pref.wcdma_rec[i - invalid_cnt].uarfcn_incl = uarfcn_incl;
        acq_rec->type.gw_pref.wcdma_rec[i - invalid_cnt].band = (int)prl_wcdma_band;

      }  /* for */

      /* Set record to invalid if there is invalid WCDMA channel.
      */
      if( invalid_cnt > 0)
      {
        acq_rec->is_valid = FALSE;
      }

      #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GWL_GENERIC:


      #if defined(FEATURE_SD_LTE) || defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
      acq_rec->is_valid = TRUE;
      pri_index         = 0;  /* indexes the priority list */

      /*Initialize the record
      */
      memset(&(acq_rec->type.gwl_generic),0,sizeof(prli_gwl_generic_type));

      /* Gets the number of gsm/wcdma/lte band.
      */
      acq_rec->type.gwl_generic.num_of_rats = SD_B_UNPACKB(buf_ptr,
                          offset,
                          FSIZ(prl_gwl_generic_type, num_of_rats) );
      offset += FSIZ(prl_gwl_generic_type, num_of_rats);

      if (acq_rec->type.gwl_generic.num_of_rats > SYS_MAX_NUM_3GPP_RATS ||
          acq_rec->type.gwl_generic.num_of_rats == 0 )
      {
        SD_MSG_HIGH_1( "FAILED - Invalid GWL num_of_rats %d",
                   acq_rec->type.gwl_generic.num_of_rats);
        acq_rec->is_valid = FALSE;
        break;
      }

      for (i = 0; i < acq_rec->type.gwl_generic.num_of_rats; i++)
      {
        sys_mode = (sys_sys_mode_e_type)SD_B_UNPACKB(buf_ptr,
                    offset,
                    FSIZ(prl_gwl_generic_type, sys_mode));
        offset += FSIZ(prl_gwl_generic_type, sys_mode);

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        switch(sys_mode)
        {

          case SYS_SYS_MODE_GSM:
          {
            /* Get the number of bands field
            */
            acq_rec->type.gwl_generic.gsm_rec.num_bands =
              SD_B_UNPACKB(buf_ptr,
              offset,
              FSIZ(prl_gwl_generic_type, num_bands) );
            offset += FSIZ(prl_gwl_generic_type, num_bands);

            acq_rec->is_valid = TRUE;
            invalid_cnt = 0;

            if ((acq_rec->type.gwl_generic.gsm_rec.num_bands == 0) ||
                (acq_rec->type.gwl_generic.gsm_rec.num_bands >
                PRL_MAX_GWL_GENERIC_BAND))
            {
              SD_ERR_1("FAILED - Invalid number of GSM bands:%d",
                acq_rec->type.gwl_generic.gsm_rec.num_bands);
              acq_rec->is_valid = FALSE;

              break;
            }

            /* Get band class, and channel field
            */
            for( j=0; j< acq_rec->type.gwl_generic.gsm_rec.num_bands; j++)
            {
              prl_gsm_band = (prl_gsm_band_e_type)SD_B_UNPACKB(
                         buf_ptr,
                         offset,
                         (word)FSIZ(prl_gwl_generic_type, band) );

              band = prl_map_gsm_band_to_band(prl_gsm_band);

              offset += FSIZ(prl_gwl_generic_type, band);

              uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                            buf_ptr,
                            offset,
                            (word)FSIZ(prl_gwl_generic_type, uarfcn_incl) );

              offset += FSIZ(prl_gwl_generic_type, uarfcn_incl);

              if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
              {
                SD_ERR_1("FAILED - Invalid ARFCN_INCL of GSM bands %d",
                             uarfcn_incl);
                invalid_cnt++;
                continue;

              }

              if (uarfcn_incl == PRL_UARFCN_INCL)
              {
                uarfcn = SD_B_UNPACKW( buf_ptr, offset,
                         FSIZ(prl_gwl_generic_type, uarfcn) );

                offset += FSIZ(prl_gwl_generic_type, uarfcn);

                if( sd_map_chan_to_blksys(SD_MODE_GSM,band,uarfcn) ==
                    SD_BLKSYS_MAX )
                {
                  SD_ERR_2("FAILED - Invalid GSM band %d and chan %d ",
                                band, uarfcn);
                  invalid_cnt++;
                  continue;
                }

                acq_rec->type.gwl_generic.gsm_rec.rec[j - invalid_cnt].uarfcn = uarfcn;

              }
              else
              {
                acq_rec->type.gwl_generic.gsm_rec.rec[j - invalid_cnt].uarfcn = PRL_WILDCARD_UARFCN;
              }

              acq_rec->type.gwl_generic.gsm_rec.rec[j - invalid_cnt].band = (int)prl_gsm_band;
              acq_rec->type.gwl_generic.gsm_rec.rec[j - invalid_cnt].uarfcn_incl = (prl_uarfcn_incl_e_type)uarfcn_incl;

            } /* for */

            acq_rec->type.gwl_generic.gsm_rec.num_bands -= invalid_cnt;
            if( acq_rec->type.gwl_generic.gsm_rec.num_bands == 0 )
            {
              acq_rec->is_valid = FALSE;
            }
            else
            {
              /* Update the priority list
              */
              acq_rec->type.gwl_generic.pri_lst[pri_index] = SYS_SYS_MODE_GSM;
              pri_index++;
            }

            break;
          }

          case SYS_SYS_MODE_WCDMA:
          {
            /* Get the number of bands field
            */
            acq_rec->type.gwl_generic.wcdma_rec.num_bands =
              SD_B_UNPACKB(buf_ptr,
                           offset,
                           FSIZ(prl_gwl_generic_type, num_bands) );
            offset += FSIZ(prl_gwl_generic_type, num_bands);

            acq_rec->is_valid = TRUE;
            invalid_cnt = 0;

            if ((acq_rec->type.gwl_generic.wcdma_rec.num_bands == 0) ||
                (acq_rec->type.gwl_generic.wcdma_rec.num_bands >
                PRL_MAX_GWL_GENERIC_BAND))
            {
              SD_ERR_1("FAILED - Invalid number of WCDMA bands:%d",
                acq_rec->type.gwl_generic.wcdma_rec.num_bands);
              acq_rec->is_valid = FALSE;

              break;
            }

            /* Get band class, and channel field
            */
            for( j=0; j< acq_rec->type.gwl_generic.wcdma_rec.num_bands; j++)
            {
              prl_wcdma_band = (prl_wcdma_band_e_type)SD_B_UNPACKB(
                         buf_ptr,
                         offset,
                         (word)FSIZ(prl_gwl_generic_type, band) );

              band = prl_map_wcdma_band_to_band(prl_wcdma_band);

              offset += FSIZ(prl_gwl_generic_type, band);

              uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                            buf_ptr,
                            offset,
                            (word)FSIZ(prl_gwl_generic_type, uarfcn_incl) );

              offset += FSIZ(prl_gwl_generic_type, uarfcn_incl);

              if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
              {
                SD_ERR_1("FAILED - Invalid UARFCN_INCL of WCDMA bands %d",
              uarfcn_incl);
                invalid_cnt++;
                continue;

              }

              if (uarfcn_incl == PRL_UARFCN_INCL)
              {
                uarfcn = SD_B_UNPACKW( buf_ptr,
                                       offset,
                                       FSIZ(prl_gwl_generic_type, uarfcn) );

                offset += FSIZ(prl_gwl_generic_type, uarfcn);

                if( sd_map_chan_to_blksys(SD_MODE_WCDMA,band,uarfcn) ==
                                                              SD_BLKSYS_MAX )
                {
                  SD_ERR_2("FAILED - Invalid WCDMA band %d and chan %d ",
                                                           band, uarfcn);
                  invalid_cnt++;
                  continue;
                }

                acq_rec->type.gwl_generic.wcdma_rec.rec[j - invalid_cnt].uarfcn = uarfcn;

              }
              else
              {
                acq_rec->type.gwl_generic.wcdma_rec.rec[j - invalid_cnt].uarfcn = PRL_WILDCARD_UARFCN;
              }

              acq_rec->type.gwl_generic.wcdma_rec.rec[j - invalid_cnt].band = (int)prl_wcdma_band;
              acq_rec->type.gwl_generic.wcdma_rec.rec[j - invalid_cnt].uarfcn_incl = uarfcn_incl;

            } /* for */

            acq_rec->type.gwl_generic.wcdma_rec.num_bands -= invalid_cnt;
            if( acq_rec->type.gwl_generic.wcdma_rec.num_bands == 0 )
            {
              acq_rec->is_valid = FALSE;
            }
            else
            {
              /* Update the priority list
              */
              acq_rec->type.gwl_generic.pri_lst[pri_index] = SYS_SYS_MODE_WCDMA;
              pri_index++;
            }

            break;
          }

          case SYS_SYS_MODE_LTE:
          {
            /* Get the number of bands field
            */
            acq_rec->type.gwl_generic.lte_rec.num_bands =
              SD_B_UNPACKB(buf_ptr,
                           offset,
                           FSIZ(prl_gwl_generic_type, num_bands) );
            offset += FSIZ(prl_gwl_generic_type, num_bands);

            acq_rec->is_valid = TRUE;
            invalid_cnt = 0;

            if ((acq_rec->type.gwl_generic.lte_rec.num_bands == 0) ||
                (acq_rec->type.gwl_generic.lte_rec.num_bands >
                PRL_MAX_GWL_GENERIC_BAND))
            {
              SD_ERR_1("FAILED - Invalid number of LTE bands:%d",
                acq_rec->type.gwl_generic.lte_rec.num_bands);
              acq_rec->is_valid = FALSE;

              break;
            }

            /* Get band class, and channel field
            */
            for( j=0; j< acq_rec->type.gwl_generic.lte_rec.num_bands; j++)
            {
              prl_lte_band = (prl_lte_band_e_type)SD_B_UNPACKB(
                         buf_ptr,
                         offset,
                         (word)FSIZ(prl_gwl_generic_type, band) );

              band = prl_map_lte_band_to_band(prl_lte_band);

              offset += FSIZ(prl_gwl_generic_type, band);

              uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                            buf_ptr,
                            offset,
                            (word)FSIZ(prl_gwl_generic_type, uarfcn_incl) );

              offset += FSIZ(prl_gwl_generic_type, uarfcn_incl);

              if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
              {
                SD_ERR_1("FAILED - Invalid UARFCN_INCL of LTE bands %d",
                              uarfcn_incl);
                invalid_cnt++;
                continue;

              }

              if (uarfcn_incl == PRL_UARFCN_INCL)
              {
                uarfcn = SD_B_UNPACKW( buf_ptr,
                                       offset,
                                       FSIZ(prl_gwl_generic_type, uarfcn) );

                offset += FSIZ(prl_gwl_generic_type, uarfcn);

                acq_rec->type.gwl_generic.lte_rec.rec[j - invalid_cnt].uarfcn = uarfcn;

              }
              else
              {
                acq_rec->type.gwl_generic.lte_rec.rec[j - invalid_cnt].uarfcn = PRL_WILDCARD_UARFCN;
              }

              acq_rec->type.gwl_generic.lte_rec.rec[j - invalid_cnt].band = (int)prl_lte_band;
              acq_rec->type.gwl_generic.lte_rec.rec[j - invalid_cnt].uarfcn_incl = (prl_uarfcn_incl_e_type)uarfcn_incl;

            } /* for */

            acq_rec->type.gwl_generic.lte_rec.num_bands -= invalid_cnt;
            if( acq_rec->type.gwl_generic.lte_rec.num_bands == 0 )
            {
              acq_rec->is_valid = FALSE;
            }
            else
            {
              /* Update the priority list
              */
              acq_rec->type.gwl_generic.pri_lst[pri_index] = SYS_SYS_MODE_LTE;
              pri_index++;
            }

            break;
          }

          case SYS_SYS_MODE_TDS:
          {
            /* Get the number of bands field
            */
            acq_rec->type.gwl_generic.tds_rec.num_bands =
              SD_B_UNPACKB(buf_ptr,
                           offset,
                           FSIZ(prl_gwl_generic_type, num_bands) );
            offset += FSIZ(prl_gwl_generic_type, num_bands);

            acq_rec->is_valid = TRUE;
            invalid_cnt = 0;

            if ((acq_rec->type.gwl_generic.tds_rec.num_bands == 0) ||
                (acq_rec->type.gwl_generic.tds_rec.num_bands >
                PRL_MAX_GWL_GENERIC_BAND))
            {
              SD_ERR_1("FAILED - Invalid number of TD-SCDMA bands:%d",
                acq_rec->type.gwl_generic.tds_rec.num_bands);
              acq_rec->is_valid = FALSE;

              break;
            }

            /* Get band class, and channel field
            */
            for( j=0; j< acq_rec->type.gwl_generic.tds_rec.num_bands; j++)
            {
              prl_tds_band = (prl_tds_band_e_type)SD_B_UNPACKB(
                         buf_ptr,
                         offset,
                         (word)FSIZ(prl_gwl_generic_type, band) );

              band = prl_map_tds_band_to_band(prl_tds_band);

              offset += FSIZ(prl_gwl_generic_type, band);

              uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                            buf_ptr,
                            offset,
                            (word)FSIZ(prl_gwl_generic_type, uarfcn_incl) );

              offset += FSIZ(prl_gwl_generic_type, uarfcn_incl);

              if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
              {
                SD_ERR_1("FAILED - Invalid UARFCN_INCL of TD-SCDMA bands %d",
                uarfcn_incl);
                invalid_cnt++;
                continue;

              }

              if (uarfcn_incl == PRL_UARFCN_INCL)
              {
                uarfcn = SD_B_UNPACKW( buf_ptr,
                                       offset,
                                       FSIZ(prl_gwl_generic_type, uarfcn) );

                offset += FSIZ(prl_gwl_generic_type, uarfcn);

                acq_rec->type.gwl_generic.tds_rec.rec[j - invalid_cnt].uarfcn = uarfcn;

              }
              else
              {
                acq_rec->type.gwl_generic.tds_rec.rec[j - invalid_cnt].uarfcn = PRL_WILDCARD_UARFCN;
              }

              acq_rec->type.gwl_generic.tds_rec.rec[j - invalid_cnt].band = (int)prl_tds_band;
              acq_rec->type.gwl_generic.tds_rec.rec[j - invalid_cnt].uarfcn_incl = (prl_uarfcn_incl_e_type)uarfcn_incl;

            } /* for */

            acq_rec->type.gwl_generic.tds_rec.num_bands -= invalid_cnt;
            
            if( acq_rec->type.gwl_generic.tds_rec.num_bands == 0 )
            {
              acq_rec->is_valid = FALSE;
            }
            else
            {
              /* Update the priority list
              */
              acq_rec->type.gwl_generic.pri_lst[pri_index] = SYS_SYS_MODE_TDS;
              pri_index++;
            }

            break;
          }

          case SYS_SYS_MODE_NONE:
          case SYS_SYS_MODE_NO_SRV:
          case SYS_SYS_MODE_AMPS:
          case SYS_SYS_MODE_CDMA:
          case SYS_SYS_MODE_HDR:
          case SYS_SYS_MODE_GPS:
          case SYS_SYS_MODE_GW:
          case SYS_SYS_MODE_GWL:
          case SYS_SYS_MODE_MAX:
          default:
          {
            SD_ERR_1( "FAILED - Invalid sys_mode of GWL record %d",
             (int)sys_mode);
            acq_rec->is_valid = FALSE;

          }
        } // end of switch
        if(pri_index == SYS_MAX_NUM_3GPP_RATS)
        {
          break;
        }
      } // end of for loop

      #endif /* (FEATURE_SD_LTE) || (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)*/
      break;

      case PRL_ACQ_REC_NONE:
      default:
        break;

  } // end of switch

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* prl_extract_acq_rec */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_extract_comm_subnet_rec

DESCRIPTION
    Extract a specified common subnet record from the currently loaded PRL
    (IS-683C and above).

DEPENDENCIES
  None.

RETURN VALUE
  The subnet prefix length (in bytes).

SIDE EFFECTS
  None.

===========================================================================*/
static  byte                      prl_extract_comm_subnet_rec(

        word                      entry_offset,
            /* Offset (in bytes) from the beginning of the common subnet
            ** table to beginning of the entry to be extracted.
            */

        byte                      *buf_ptr,
            /* Pointer to a destination buffer into which to copy the common
            ** subnet prefix.
            */

        size_t                    buf_size
           /* Size in bytes of the buffer pointed in buf_ptr
           */

)
{
  prl_hdr_s_type            *prl_hdr_ptr    = &prl_hdr;
  byte                      *entry_ptr;
  byte                      length;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( buf_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point to beginning of requested entry.
  */
  entry_ptr = prl_hdr_ptr->subnet_tbl_ptr + entry_offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the length of the subnet prefix.
  */
  length = entry_ptr[0] & 0x0F;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the common subnet prefix into the specified buffer.
  */
  memscpy( buf_ptr,
           buf_size,
           &entry_ptr[1],
           length );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the subnet prefix length.
  */
  return length;

} /* prl_extract_comm_subnet_rec */

/* <EJECT> */
/*===========================================================================

FUNCTION prl_set_net_sel_mode

DESCRIPTION
 Populate plmn network selection mode from plmn lac include, plmn
 select mode and auto network selection mode.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void                        prl_set_plmn_net_sel_mode(

      prl_plmn_lac_incl_e_type    plmn_lac_incl,
      /* plmn lac include in sr from PRL
      */

      prl_plmn_sel_mode_e_type    plmn_sel_mode,
      /* plmn select mode in sr from PRL
      */

      boolean                     is_auto_net_sel_mode,
      /* auto network selection mode in sr from PRL
      */

      prli_plmn_sys_type          *plmn_ptr
      /* Save the plmn network selection mode into this buffer.
      */
)
{

    SD_ASSERT( plmn_ptr != NULL );

    switch( plmn_lac_incl ) {

      case PRL_PLMN_LAC_NOT_INCL:

        switch( plmn_sel_mode ) {

          case PRL_PLMN_SEL_MODE_ANY_PLMN:

            plmn_ptr->net_select_mode = SD_NET_SELECT_MODE_LIMITED;

            return;

          case PRL_PLMN_SEL_MODE_ALLOWED_PLMN:

            plmn_ptr->net_select_mode = SD_NET_SELECT_MODE_AUTO;

            return;

          case PRL_PLMN_SEL_MODE_HOME_PLMN:

            plmn_ptr->net_select_mode = SD_NET_SELECT_MODE_HPLMN;

            return;

          case PRL_PLMN_SEL_MODE_RESERVED:
          case PRL_PLMN_SEL_MODE_MAX:
          default:
            plmn_ptr->net_select_mode = SD_NET_SELECT_MODE_MAX;
            return;
        }

      case PRL_PLMN_INCL:
      case PRL_PLMN_LAC_INCL:
      case PRL_PLMN_MULTI_LAC_INCL:

         if( plmn_lac_incl == PRL_PLMN_INCL )
         {
           plmn_ptr->lac = PRL_WILDCARD_LAC;
           plmn_ptr->num_lac = 0;
         }

         if( is_auto_net_sel_mode )
         {
           plmn_ptr->net_select_mode = SD_NET_SELECT_MODE_AUTO;
         }
         else
         {
           plmn_ptr->net_select_mode = SD_NET_SELECT_MODE_PRL_SYS;
         }
        return;

      case PRL_PLMN_LAC_INCL_RESERVED:
      default:
        plmn_ptr->net_select_mode = SD_NET_SELECT_MODE_MAX;
        break;
    }

}


/* <EJECT> */
/*===========================================================================

FUNCTION prl_extract_sys_rec

DESCRIPTION
  Extract PRL system record from a specified memory location.

DEPENDENCIES
  None.

RETURN VALUE
  Offset (in bits) to next system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  dword                      prl_extract_sys_rec(

        byte                      *buf_ptr,
            /* Pointer to buffer containing PRL from which to extract the
            ** system record.
            */

        dword                      offset,
            /* Offset (in bits) to the beginning of system record which to
            ** extract.
            */

        prli_sys_rec_type         *sys_rec,
            /* Pointer to buffer where to deposit the extracted system
            ** record.
            */
        prl_sspr_p_rev_e_type     sspr_p_rev
)
{
  dword                    input_offset        = offset;
  byte                     i;                     /* loope counter */
  prl_plmn_lac_incl_e_type plmn_lac_incl;
  prl_plmn_sel_mode_e_type plmn_sel_mode;
  boolean                  is_auto_net_sel_mode;
  prli_sid_nid_type        *sid_nid_ptr        = NULL;
  prli_subnet_id_type      *subnet_id_ptr      = NULL;
  uint8                    subnet_len          = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( buf_ptr != NULL );
  SD_ASSERT( offset != 0 );
  SD_ASSERT( sys_rec != NULL );
  SD_ASSERT( (sspr_p_rev == PRL_SSPR_P_REV_1) || (sspr_p_rev == PRL_SSPR_P_REV_3) )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract system record based on PRL or Extended PRL catagory
  */
  switch (sspr_p_rev) {

    case PRL_SSPR_P_REV_1:

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the fields of the system record one by one while
      ** incrementing the bit offset per each extracted field.
      */

      /* Mapping SSPR_P_REV_1 system record to SSPR_P_REV_3 recrod
      ** Set SYSTEM REC TYPE to IS2000, ASSOCIATION_INC to 0
      */
      sys_rec->rec_type = PRL_SYS_REC_IS2000;
      sys_rec->association_inc = FALSE;

      /* Extract the SID field.
      */
      sys_rec->rec.is2000.sid = SD_B_UNPACKW(buf_ptr,
                                          offset,
                                          FSIZ(prl_sys_record_type, sid));
      offset += FSIZ(prl_sys_record_type, sid);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the NID_INCL field
      */
      sys_rec->rec.is2000.nid_incl = (prl_nid_incl_type)
                             SD_B_UNPACKB(buf_ptr,
                                       offset,
                                       FSIZ(prl_sys_record_type, nid_incl));
      offset += FSIZ(prl_sys_record_type, nid_incl);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If a NID is included, extract the NID field; else set the NID to
      ** wildcard NID.
      */
      if (sys_rec->rec.is2000.nid_incl == PRL_NID_INCL)
      {
        sys_rec->rec.is2000.nid = SD_B_UNPACKW(buf_ptr, offset,
        FSIZ(prl_sys_record_type, nid));
        offset += FSIZ(prl_sys_record_type, nid);
      }
      else
      {
        if (sys_rec->rec.is2000.nid_incl == PRL_NID_NOT_INCL_WILDCARD_NID)
        {
          sys_rec->rec.is2000.nid = PRL_WILDCARD_NID;
        }
        else if (sys_rec->rec.is2000.nid_incl == PRL_NID_NOT_INCL_PUBLIC_NID)
        {
          sys_rec->rec.is2000.nid = PRL_PUBLIC_NETWORK_NID;
        }
        else
        {
          sys_rec->rec.is2000.nid = PRL_WILDCARD_NID;
          SD_ERR_0("NID_INCL");
        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the NEG_PREF field
      */
      sys_rec->neg_pref = (prl_neg_pref_type)SD_B_UNPACKB(buf_ptr,
                                        offset,
                                        FSIZ(prl_sys_record_type, neg_pref));
      offset += FSIZ(prl_sys_record_type, neg_pref);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the GEO field
      */
      sys_rec->geo = (prl_geo_region_type)SD_B_UNPACKB(buf_ptr,
                                          offset,
                                          FSIZ(prl_sys_record_type, geo));
      offset += FSIZ(prl_sys_record_type, geo);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* In IS-683A the priority field is defined only if the system is a
      ** preferred system
      */
      sys_rec->pri = PRL_PRI_SAME;

      if (sys_rec->neg_pref == PRL_PREFERRED)
      {
        /* Extract the PRI field
        */
        sys_rec->pri = (prl_pri_type) SD_B_UNPACKB( buf_ptr,
                                             offset,
                                             FSIZ(prl_sys_record_type, pri));
        offset += FSIZ(prl_sys_record_type, pri);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the ACQ_INDEX field
      */
      sys_rec->acq_index = SD_B_UNPACKB(buf_ptr, offset,
      FSIZ(prl_sys_record_type, acq_index));
      offset += FSIZ(prl_sys_record_type, acq_index);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the ROAM_IND field if this is a preferred entry
      */
      if( sys_rec->neg_pref == PRL_PREFERRED )
      {
        sys_rec->roam_ind = (sys_roam_status_e_type)SD_B_UNPACKB(buf_ptr,
                                        offset,
                                        FSIZ(prl_sys_record_type, roam_ind));
        offset += FSIZ(prl_sys_record_type, roam_ind);

        /* IS-683A PRLs have TSB58 Roam Indicators - convert it to the
        ** internal SD roaming indicator representation.
        */
        sys_rec->roam_ind = prl_map_tsb58_roam_to_sd_roam(sys_rec->roam_ind);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Return offset to next system record.
      */
      return offset;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SSPR_P_REV_3:

      /* Extract the fields of the system record one by one while incrementing
      ** the bit offset per each extracted field.
      */

      /* Extract the SYS_REC_LENGTH field
      */
      sys_rec->sys_rec_length = SD_B_UNPACKB( buf_ptr,
                                  offset,
                                  (word)SYS_REC_LENGTH_LENGTH );

      offset += (word)SYS_REC_LENGTH_LENGTH;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the SYS_REC_TYPE field
      */
      sys_rec->rec_type = (prl_sys_rec_type) SD_B_UNPACKB( buf_ptr,
                                                        offset,
                                                        SYS_REC_TYPE_LENGTH);
      offset += SYS_REC_TYPE_LENGTH;

      /* If record is not supported, jump to next record.
      */
      if( sys_rec->rec_type >= PRL_SYS_REC_MAX )
      {
        SD_MSG_LOW_1("Unrecognized system rec type %d",sys_rec->rec_type);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the NEG_PREF field
      */
      sys_rec->neg_pref = (prl_neg_pref_type)SD_B_UNPACKB(buf_ptr, offset,
                                        FSIZ(prl_sys_record_type, neg_pref));
      offset += FSIZ(prl_sys_record_type, neg_pref);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the GEO field
      */
      sys_rec->geo = (prl_geo_region_type)SD_B_UNPACKB(buf_ptr, offset,
                                             FSIZ(prl_sys_record_type, geo));
      offset += FSIZ(prl_sys_record_type, geo);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the PRI field (PRI field has meaning only for preferred
      ** systems).
      */
      sys_rec->pri = PRL_PRI_SAME;

      if( sys_rec->neg_pref == PRL_PREFERRED )
      {
        sys_rec->pri = (prl_pri_type) SD_B_UNPACKB( buf_ptr,offset,
                                             FSIZ(prl_sys_record_type, pri));
      }

      offset += FSIZ(prl_sys_record_type, pri);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the ACQ_INDEX field
      */
      sys_rec->acq_index = SD_B_UNPACKB( buf_ptr,
                                      offset,
                                      FSIZ(prl_sys_record_type, acq_index) );

      offset += FSIZ(prl_sys_record_type, acq_index);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the type specific fields based on the system record type.
      */
      switch( sys_rec->rec_type )
      {
        case PRL_SYS_REC_IS2000:

          /* Jump over reserved bits.
          */
          offset += SYS_REC_IS2000_RESERVED_LENGTH;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Extract the NID_INCL field
          */
          sys_rec->rec.is2000.nid_incl =
            (prl_nid_incl_type) SD_B_UNPACKB( buf_ptr,
                                        offset,
                                        FSIZ(prl_sys_record_type, nid_incl));

          offset += FSIZ(prl_sys_record_type, nid_incl);

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Extract the SID field
          */
          sys_rec->rec.is2000.sid = SD_B_UNPACKW( buf_ptr, offset,
                                            FSIZ(prl_sys_record_type, sid) );

          offset += FSIZ(prl_sys_record_type, sid);

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* If a NID is included, extract the NID field; else set the NID to
          ** wildcard/public NID.
          */
          switch( sys_rec->rec.is2000.nid_incl )
          {
            case PRL_NID_INCL:
              sys_rec->rec.is2000.nid = SD_B_UNPACKW( buf_ptr, offset,
                                            FSIZ(prl_sys_record_type, nid) );
              offset += FSIZ(prl_sys_record_type, nid);
              break;

            case PRL_NID_NOT_INCL_WILDCARD_NID:
              sys_rec->rec.is2000.nid = PRL_WILDCARD_NID;
              break;

            case PRL_NID_NOT_INCL_PUBLIC_NID:
              sys_rec->rec.is2000.nid = PRL_PUBLIC_NETWORK_NID;
              break;

            case PRL_NID_INCL_RESERVED:
            default:
              sys_rec->rec.is2000.nid = PRL_WILDCARD_NID;
              SD_ERR_0("NID_INCL");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_SYS_REC_IS856:

          /* Jump over reserved bits.
          */
          offset += SYS_REC_IS856_RESERVED_LENGTH;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Extract the subnet common included field.
          */
          sys_rec->rec.is856.common_incl = SD_B_UNPACKB( buf_ptr,
                                   offset,
                                   SYS_REC_SUBNET_COM_INC_LENGTH );

          offset += SYS_REC_SUBNET_COM_INC_LENGTH;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Extract the subnet LSB length field.
          */
          sys_rec->rec.is856.lsb_len = SD_B_UNPACKB( buf_ptr,
                               offset,
                               SYS_REC_SUBNET_LSB_LENGTH );

          offset += SYS_REC_SUBNET_LSB_LENGTH;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* If included, get the common subnet prefix.
          */
          if( sys_rec->rec.is856.common_incl )
          {
            /* Extract the common subnet offset.
            */
            sys_rec->rec.is856.common_offset = SD_B_UNPACKW( buf_ptr,
                                       (sys_rec->rec.is856.lsb_len + offset),
                                       SYS_REC_SUBNET_COM_OFFSET_LENGTH );

            /* Extract the common subnet prefix.
            */
            sys_rec->rec.is856.common_len = prl_extract_comm_subnet_rec( sys_rec->rec.is856.common_offset,
                                                                         sys_rec->rec.is856.subnet,
                                                                         sizeof(sys_rec->rec.is856.subnet) );
          }
          else
          {
            sys_rec->rec.is856.common_offset = 0;
            sys_rec->rec.is856.common_len = 0;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Extract the subnet LSB field.
          */
          memscpy( &sys_rec->rec.is856.subnet[sys_rec->rec.is856.common_len],
                   (byte)(sizeof(sys_rec->rec.is856.subnet) - sys_rec->rec.is856.common_len),
                   &buf_ptr[offset/8],
                   ((sys_rec->rec.is856.lsb_len + 7) / 8)
                 );

          /* Set the subnet length to common length + LSB length.
          */
          sys_rec->rec.is856.subnet_length = \
            sys_rec->rec.is856.common_len + sys_rec->rec.is856.lsb_len;

          offset += sys_rec->rec.is856.lsb_len;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* If included, jump over the subnet common offset.
          */
          if( sys_rec->rec.is856.common_incl )
          {
            offset += SYS_REC_SUBNET_COM_OFFSET_LENGTH;
          }

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_SYS_REC_PLMN:

          plmn_lac_incl = PRL_PLMN_LAC_INCL_RESERVED;
          plmn_sel_mode = PRL_PLMN_SEL_MODE_RESERVED;
          is_auto_net_sel_mode = FALSE;

          /* Initial PLMN related fields with default value. */
          sys_plmn_undefine_plmn_id(&sys_rec->rec.plmn.plmn_id);
          sys_rec->rec.plmn.net_select_mode = SD_NET_SELECT_MODE_MAX;
          sys_rec->rec.plmn.lac = PRL_WILDCARD_LAC;
          sys_rec->rec.plmn.num_lac = 0;

          /* Extract plmn lac include field. */
          plmn_lac_incl = (prl_plmn_lac_incl_e_type)SD_B_UNPACKB(
                                     buf_ptr,
                                     offset,
                                     (word)SYS_REC_PLMN_LAC_INCL_LENGTH);

          offset += SYS_REC_PLMN_LAC_INCL_LENGTH;

          /* Extract plmn select mode, network selection mode,plmn id,
          ** and local area code fields.
          */
          if( plmn_lac_incl == PRL_PLMN_LAC_NOT_INCL )
          {
            plmn_sel_mode = (prl_plmn_sel_mode_e_type)SD_B_UNPACKB(
                                       buf_ptr,
                                       offset,
                                       (word)SYS_REC_PLMN_SELECT_MODE_LENGTH);

            offset += SYS_REC_PLMN_SELECT_MODE_LENGTH;

          }

          if( plmn_lac_incl == PRL_PLMN_INCL ||
              plmn_lac_incl == PRL_PLMN_LAC_INCL ||
              plmn_lac_incl == PRL_PLMN_MULTI_LAC_INCL )
          {
            sys_rec->rec.plmn.plmn_id.identity[0] = SD_B_UNPACKB( buf_ptr,
                                                       offset,
                                                       8 );
            offset += 8;

            sys_rec->rec.plmn.plmn_id.identity[1] = SD_B_UNPACKB( buf_ptr,
                                                       offset,
                                                       8 );
            offset += 8;

            sys_rec->rec.plmn.plmn_id.identity[2] = SD_B_UNPACKB( buf_ptr,
                                                       offset,
                                                       8 );
            offset += 8;

            if( plmn_lac_incl == PRL_PLMN_LAC_INCL )
            {
              sys_rec->rec.plmn.lac = SD_B_UNPACKW( buf_ptr,
                                                 offset,
                                                 SYS_REC_LAC_LENGTH );
              offset += SYS_REC_LAC_LENGTH;
            }

            is_auto_net_sel_mode = SD_B_UNPACKB(
                                            buf_ptr,
                                            offset,
                                            SYS_REC_NET_SELECT_MODE_LENGTH );
            offset += SYS_REC_NET_SELECT_MODE_LENGTH;

            if( plmn_lac_incl == PRL_PLMN_MULTI_LAC_INCL )
            {
              offset += SYS_REC_MULTI_LAC_RESERVED_LENGTH;

              sys_rec->rec.plmn.num_lac = SD_B_UNPACKB( buf_ptr,
                                                     offset,
                                                     SYS_REC_NUM_LAC_LENGTH);
              offset += SYS_REC_NUM_LAC_LENGTH;

              if( sys_rec->rec.plmn.num_lac > PRL_MAX_PLMN_LAC_ARRAY )
              {
                sys_rec->rec.plmn.num_lac = PRL_MAX_PLMN_LAC_ARRAY;
              }

              for( i = 0; i < sys_rec->rec.plmn.num_lac; i++ )
              {
               sys_rec->rec.plmn.lac_a[i] = SD_B_UNPACKW( buf_ptr,
                                                       offset,
                                                       SYS_REC_LAC_LENGTH );
               offset += SYS_REC_LAC_LENGTH;
              }
            }

          }
          /* Populate plmn network selection mode. */
          prl_set_plmn_net_sel_mode( plmn_lac_incl,
                                     plmn_sel_mode,
                                     is_auto_net_sel_mode,
                                     &sys_rec->rec.plmn );

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_SYS_REC_MCC_MNC:

          /* Clear MCC-MNC sys rec fields
          */
          sys_rec->rec.mcc_mnc.mcc = SD_MCC_NONE;
          sys_rec->rec.mcc_mnc.mnc = SD_MNC_NONE;
          sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len = 0;
          sys_rec->rec.mcc_mnc.info.subnet_id.num_subnet_id = 0;
          sys_rec->rec.mcc_mnc.mcc_mnc_subtype = PRL_MCC_MNC_SUBTYPE_NONE;


          /* Offset already points to the field after ACQ_INDEX */
          /* Extract MCC-MNC sys rec subtype */
          sys_rec->rec.mcc_mnc.mcc_mnc_subtype = \
            (prl_mcc_mnc_subtype_e_type)SD_B_UNPACKB(buf_ptr,
                                                      offset,
                       (word)SYS_REC_MCC_MNC_SUBTYPE_LENGTH);

          offset += SYS_REC_MCC_MNC_SUBTYPE_LENGTH;

          /* mcc, mnc subtype should be in right range */
          if (! BETWEEN(sys_rec->rec.mcc_mnc.mcc_mnc_subtype,
                        PRL_MCC_MNC_SUBTYPE_NONE,
                        PRL_MCC_MNC_SUBTYPE_MAX)
             )
          {
            SD_ERR_1 ("Error mcc_mnc subtype %d, Rej sys rec",
                           sys_rec->rec.mcc_mnc.mcc_mnc_subtype);
            break;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* MCC and MNC do not depend on subtype, extract them */
          /* Mobile Country Code */
          sys_rec->rec.mcc_mnc.mcc = (word)SD_B_UNPACKW(
                                               buf_ptr,
                                               offset,
                                               (word)SYS_REC_MCC_LEN);

          offset += SYS_REC_MCC_LEN;

          /* Mobile Network Code */
          sys_rec->rec.mcc_mnc.mnc = (word)SD_B_UNPACKW(
                                               buf_ptr,
                                               offset,
                                               (word)SYS_REC_MNC_LEN);

          offset += SYS_REC_MNC_LEN;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


          /* Extract other fields based on MCC MNC subtypes */
          switch (sys_rec->rec.mcc_mnc.mcc_mnc_subtype)
          {

            /* MCC MNC only */
            case PRL_MCC_MNC_SUBTYPE_ONLY:
              break;

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            /* MCC, MNC and multiple SIDs */
            case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID:
            case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID_NID:

              /* Move the offset to point to next field */
              offset += SYS_REC_MCC_MNC_RESERVED_LENGTH;

              sid_nid_ptr = &(sys_rec->rec.mcc_mnc.info.sid_nid);

              /* number of SIDs */
              sid_nid_ptr->sid_nid_pair_len = (uint8)SD_B_UNPACKB(buf_ptr,
                           offset,
                           (word)SYS_REC_SID_NID_NUM_LENGTH);

              offset += SYS_REC_SID_NID_NUM_LENGTH;

              if (sid_nid_ptr->sid_nid_pair_len > SYS_REC_MAX_SID_NID)
              {
                SD_MSG_HIGH_0 ("Truncating SIDs to max allowed");

                sid_nid_ptr->sid_nid_pair_len = SYS_REC_MAX_SID_NID;
              }

              /* Extract SIDs and populate sys rec*/
              for (i=0;i < sid_nid_ptr->sid_nid_pair_len;i++)
              {

                sid_nid_ptr->sid[i] = (word)SD_B_UNPACKW(buf_ptr,
                             offset,
                             (word)SYS_REC_SID_LEN);

                offset += SYS_REC_SID_LEN;

                if (sys_rec->rec.mcc_mnc.mcc_mnc_subtype ==
                    PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID)
                {
                  /* Default NIDs to WILDCARD */
                  sid_nid_ptr->nid[i] = SD_WILDCARD_NID;
                }
                else
                {
                  /* Extract NID */
                  sid_nid_ptr->nid[i] = (word)SD_B_UNPACKW(buf_ptr,
                               offset,
                               (word)SYS_REC_NID_LEN);

                  offset += SYS_REC_NID_LEN;

                }

              }

              break;

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* MCC, MNC and multiple Subnet IDs included */
            case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID:

              /* Move the offset to point to next field */
              offset += SYS_REC_MCC_MNC_RESERVED_LENGTH;

              subnet_id_ptr = &(sys_rec->rec.mcc_mnc.info.subnet_id);

              /* Number of Subnet IDs */
              subnet_id_ptr->num_subnet_id = (uint8)SD_B_UNPACKB(buf_ptr,
                             offset,
                             (word)SYS_REC_SUBNET_ID_NUM_LENGTH);

              offset += SYS_REC_SUBNET_ID_NUM_LENGTH;

              if (subnet_id_ptr->num_subnet_id > SYS_REC_MAX_SUBNET_ID)
              {
                SD_MSG_HIGH_0 ("Truncating subnet IDs to max allowed");

                subnet_id_ptr->num_subnet_id = SYS_REC_MAX_SUBNET_ID;
              }

              for (i=0; i < subnet_id_ptr->num_subnet_id; i++)
              {
                subnet_id_ptr->subnet_length[i] =(uint8)SD_B_UNPACKB(
                               buf_ptr,
                               offset,
                               (word)SYS_REC_SUBNET_LENGTH_LEN);

                offset += SYS_REC_SUBNET_LENGTH_LEN;

                /* clear the subnet ID */
                memset( (subnet_id_ptr->subnet_id[i]),
                        0,
                        SYS_REC_MAX_SUBNET_ID_LENGTH );

                subnet_len = MIN(SYS_REC_MAX_SUBNET_ID_LENGTH,
                                 ((subnet_id_ptr->subnet_length[i]+7)/8));

                /* Extract the subnet ID field.
                */
                memscpy( (subnet_id_ptr->subnet_id[i]),
                         sizeof(subnet_id_ptr->subnet_id[i]),
                         &(buf_ptr[offset/8]),
                         subnet_len);

                offset += subnet_id_ptr->subnet_length[i];
              }
              /* add padding
              */
              offset = ((offset + 7)/8)*8;
              break;

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* Default cases */
            case PRL_MCC_MNC_SUBTYPE_NONE:
            case PRL_MCC_MNC_SUBTYPE_MAX:
            default:
              SD_ERR_1 (" invalid mcc mnc subtype  %d",
                            sys_rec->rec.mcc_mnc.mcc_mnc_subtype);
              break;
          }

          /* PRL_SYS_REC_MCC_MNC */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_SYS_REC_MAX:
        default:

          SD_ERR_0( "Unrecognized sys rec type");
          break;

      } /* switch( rec_type ) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the ROAM_IND field if this is a preferred entry
      */
      if( sys_rec->neg_pref == PRL_PREFERRED )
      {
        sys_rec->roam_ind = (sys_roam_status_e_type)SD_B_UNPACKB(buf_ptr,
                            offset,FSIZ(prl_sys_record_type, roam_ind));

        offset += FSIZ(prl_sys_record_type, roam_ind);

        /* IS-683A PRLs have TSB58 Roam Indicators - convert it to the internal
        ** SD roaming indicator representation.
        */
        sys_rec->roam_ind = prl_map_tsb58_roam_to_sd_roam(sys_rec->roam_ind);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the association included field.
      */
      sys_rec->association_inc = SD_B_UNPACKB( buf_ptr,
                                            offset,
                                            SYS_REC_ASSOCIATION_INC_LENGTH );

      offset += SYS_REC_ASSOCIATION_INC_LENGTH;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the association related fields, as needed.
      */
      if( sys_rec->association_inc )
      {
        sys_rec->association_tag = SD_B_UNPACKB( buf_ptr,offset,
                                             SYS_REC_ASSOCIATION_TAG_LENGTH);
        offset += SYS_REC_ASSOCIATION_TAG_LENGTH;


        sys_rec->pn_association = SD_B_UNPACKB( buf_ptr,
                                             offset,
                                             SYS_REC_PN_ASSOCIATION_LENGTH );
        offset += SYS_REC_PN_ASSOCIATION_LENGTH;


        sys_rec->data_association = SD_B_UNPACKB( buf_ptr,offset,
                                            SYS_REC_DATA_ASSOCIATION_LENGTH);
        offset += SYS_REC_DATA_ASSOCIATION_LENGTH;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Return offset to next system record.
      */
      return (input_offset + ((sys_rec->sys_rec_length)*8));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case PRL_SSPR_P_REV_RESERVED:
    case PRL_SSPR_P_REV_INVALID:
    default:
      SD_ERR_1("Bad PRL SSPR_P_REV : %d", sspr_p_rev);
      return 0;
  }/* switch */

} /* prl_extract_sys_rec */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_sys_rec_get_sid

DESCRIPTION
  Extract the SID from a PRL system record.

DEPENDENCIES
  None.

RETURN VALUE
  Void.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      prl_sys_rec_get_sid(

        const prli_sys_rec_type   *sys_rec_ptr,
            /* Pointer to a system record from which to get the SID.
            */

        sd_sid2_u_type            *sid_ptr
            /* Pointer to buffer where to extract the SID.
            */
)
{
  SD_ASSERT( sys_rec_ptr != NULL );
  SD_ASSERT( sid_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( sys_rec_ptr->rec_type )
  {
    case PRL_SYS_REC_IS2000:

      sid_ptr->is95.sid = sys_rec_ptr->rec.is2000.sid;
      sid_ptr->is95.nid = sys_rec_ptr->rec.is2000.nid;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SYS_REC_IS856:

      memscpy( sid_ptr->is856.sector_id,
               sizeof(sid_ptr->is856.sector_id),
               sys_rec_ptr->rec.is856.subnet,
               sizeof(sid_ptr->is856.sector_id) );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SYS_REC_PLMN:

     sid_ptr->plmn.plmn_id = sys_rec_ptr->rec.plmn.plmn_id;
     sid_ptr->plmn.lac     = sys_rec_ptr->rec.plmn.lac;

     break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SYS_REC_MCC_MNC:

    {
      switch( sys_rec_ptr->rec.mcc_mnc.mcc_mnc_subtype )
      {
        case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID:

          sid_ptr->is95.sid = sys_rec_ptr->rec.mcc_mnc.info.sid_nid.sid[0];
          sid_ptr->is95.nid = PRL_WILDCARD_NID;

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

        case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID_NID:

          sid_ptr->is95.sid = sys_rec_ptr->rec.mcc_mnc.info.sid_nid.sid[0];
          sid_ptr->is95.nid = sys_rec_ptr->rec.mcc_mnc.info.sid_nid.nid[0];

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

        case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID:

          memscpy( sid_ptr->is856.sector_id,
                   sizeof(sid_ptr->is856.sector_id),
                   sys_rec_ptr->rec.mcc_mnc.info.subnet_id.subnet_id[0],
                   sizeof(sid_ptr->is856.sector_id) );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

        case PRL_MCC_MNC_SUBTYPE_NONE:
        case PRL_MCC_MNC_SUBTYPE_ONLY:
        case PRL_MCC_MNC_SUBTYPE_MAX:
        default:
          SD_MSG_LOW_1( "Unrecognized mcc_mnc_subtype %d",
            sys_rec_ptr->rec.mcc_mnc.mcc_mnc_subtype);
          break;

      }

    break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SYS_REC_MAX:
    default:
      SD_MSG_LOW_1( "Unrecognized sys_rec_type %d",sys_rec_ptr->rec_type);
      break;

  } /* switch */

} /* prl_sys_rec_get_sid */



/* <EJECT> */
/*===========================================================================

FUNCTION prl_plmn_wildcard

DESCRIPTION
  Get PRL wildcard enum type of a plmn system record.

DEPENDENCIES

RETURN VALUE
  enumeration of prl wcard type

SIDE EFFECTS
  None.

===========================================================================*/
static  prl_wcard_e_type          prl_plmn_wildcard(

        const prli_plmn_sys_type  *plmn_ptr
        /* Pointer to a plmn system record which to check for wildcard ID.
        */
)
{

  sys_mcc_type                            mcc;
  sys_mnc_type                            mnc;
  boolean                                 plmn_undefined;
  boolean                                 includes_pcs = TRUE;

  SD_ASSERT( plmn_ptr != NULL );

  switch( plmn_ptr->net_select_mode ) {

    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:

      return PRL_WCARD_SID;

    case SD_NET_SELECT_MODE_AUTO:

      if(sys_plmn_id_is_undefined(plmn_ptr->plmn_id ) )
      {
        return PRL_WCARD_SID;
      }

      /* if MMSS is not enabled, the PRL will not have any wildcard MNC.
      ** So a SID NID match = non wildcard mcc and mnc + non wildcard lac
      ** SID wilcard NID match = non wildcard mcc and mnc + wildcard lac
      ** wilcard SID match = mcc and mnc set to wildcard
      */
      if(!sdss_is_mmss_operation(SD_SS_MAIN))
      {
        if( plmn_ptr->lac == PRL_WILDCARD_LAC && plmn_ptr->num_lac == 0 )
        {
          return PRL_WCARD_NID;
        }
        else
        {
          return PRL_WCARD_NOT;
        }
      }
      else
      /* if MMSS is defined, the equivalent PRL will have wildcard MNC and
      ** the LAC is always set to wildcard.
      ** So a SID NID match = non wildcard mcc + non wildcard mnc
      ** SID wilcard NID match = non wildcard mcc  + wildcard mnc
      ** wilcard SID match = mcc and mnc set to wildcard .
      */
      {
        sys_plmn_get_mcc_mnc2(plmn_ptr->plmn_id,
                             &plmn_undefined,
                             &includes_pcs,
                             &mcc,
                             &mnc);
        if(mnc == SYS_WILDCARD_MNC)
        {
          return PRL_WCARD_NID;
        }
        else
        {
          return PRL_WCARD_NOT;
        }
      }

    case SD_NET_SELECT_MODE_PRL_SYS:

      if( plmn_ptr->lac == PRL_WILDCARD_LAC && plmn_ptr->num_lac == 0 )
      {
        return PRL_WCARD_NID;
      }
      else
      {
        return PRL_WCARD_NOT;
      }


    case SD_NET_SELECT_MODE_MANUAL_SYS:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    case SD_NET_SELECT_MODE_MAX:
    default:

      SD_ERR_1("sd network selection mode error code: %d",
             plmn_ptr->net_select_mode);
      return PRL_WCARD_NOT;
  }

}



/* <EJECT> */
/*===========================================================================

FUNCTION prl_sys_wildcard

DESCRIPTION
  Get PRL wildcard enum type of a system record.

DEPENDENCIES
  None.

RETURN VALUE
  PRL wcard enum type

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  prl_wcard_e_type          prl_sys_wildcard(

        const prli_sys_rec_type   *sys_rec_ptr
            /* Pointer to a system record which to check for wildcard ID.
            */
)
{
  prl_wcard_e_type           wcard    = PRL_WCARD_NOT;
  int                        i        = 0;
  const prli_sid_nid_type    *sid_nid_ptr;
  const prli_subnet_id_type  *subnet_id_ptr;

  SD_ASSERT( sys_rec_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( sys_rec_ptr->rec_type )
  {
    case PRL_SYS_REC_IS2000:

      if( sys_rec_ptr->rec.is2000.sid == PRL_WILDCARD_SID )
      {
        wcard = PRL_WCARD_SID;
      }
      else if( sys_rec_ptr->rec.is2000.nid == PRL_WILDCARD_NID )
      {
        wcard = PRL_WCARD_NID;
      }
      else
      {
        wcard = PRL_WCARD_NOT;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SYS_REC_IS856:

      if( sys_rec_ptr->rec.is856.subnet_length == 0 )
      {
        wcard = PRL_WCARD_SID;
      }
      else
      {
        wcard = PRL_WCARD_NOT;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SYS_REC_PLMN:

      wcard = prl_plmn_wildcard(&sys_rec_ptr->rec.plmn);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SYS_REC_MCC_MNC:
      switch(sys_rec_ptr->rec.mcc_mnc.mcc_mnc_subtype)
      {
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        case  PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID_NID:
          sid_nid_ptr = &(sys_rec_ptr->rec.mcc_mnc.info.sid_nid);
          for( i = 0;i < sid_nid_ptr->sid_nid_pair_len;i++ )
          {
            if( sid_nid_ptr->sid[i] == PRL_WILDCARD_SID )
            {
              wcard = PRL_WCARD_SID;
              break;
            }
            else if( sid_nid_ptr->nid[i] == PRL_WILDCARD_NID )
            {
              wcard = PRL_WCARD_NID;
              break;
            }
            else
            {
              wcard = PRL_WCARD_NOT;
            }
          }
          break;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID:
          subnet_id_ptr = &(sys_rec_ptr->rec.mcc_mnc.info.subnet_id);
          for( i = 0; i < subnet_id_ptr->num_subnet_id; i++ )
          {
            if( subnet_id_ptr->subnet_length[i] == 0 )
            {
              wcard = PRL_WCARD_SID;
            }
            else
            {
              wcard = PRL_WCARD_NOT;
            }
          }
          break;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        case PRL_MCC_MNC_SUBTYPE_NONE:
        case PRL_MCC_MNC_SUBTYPE_ONLY:
        case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID:
        case PRL_MCC_MNC_SUBTYPE_MAX:
        default:
          SD_MSG_HIGH_1("Unsupp mcc mnc type %d",
                       sys_rec_ptr->rec.mcc_mnc.mcc_mnc_subtype);
          wcard = PRL_WCARD_SID;
          break;

      }//switch( mcc mnc subtype)

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SYS_REC_MAX:
    default:
      SD_MSG_LOW_1( "Unknown sys rec type d", sys_rec_ptr->rec_type);
      wcard = PRL_WCARD_UNKNOWN;
      break;

  } /* switch */

  return wcard;

} /* prl_sys_wildcard */





/* <EJECT> */
/*===========================================================================

FUNCTION prl_hdr_init

DESCRIPTION
  Initialize a prl_hdr variable.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      prl_hdr_init(

        prl_hdr_s_type            *prl_hdr_ptr
            /* Pointer to PRL header which to initialize.
            */
)
{
  SD_ASSERT( prl_hdr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  prl_hdr_ptr->is_valid           = FALSE;
  prl_hdr_ptr->is_disabled        = FALSE;

  prl_hdr_ptr->is_mcc_sys_rec     = FALSE;

  prl_hdr_ptr->pr_list_size       = 0;
  prl_hdr_ptr->pr_list_id         = PRL_DEFAULT_VER;

  prl_hdr_ptr->sspr_p_rev         = PRL_SSPR_P_REV_INVALID;
  prl_hdr_ptr->num_subnet_recs    = 0;

  prl_hdr_ptr->pref_only          = FALSE;
  prl_hdr_ptr->def_roam_ind       = SYS_ROAM_STATUS_OFF;
  prl_hdr_ptr->num_acq_recs       = 0;
  prl_hdr_ptr->num_sys_recs       = 0;
  prl_hdr_ptr->acq_tbl_bit_offset = 0;
  prl_hdr_ptr->sys_tbl_bit_offset = 0;

} /* prl_hdr_init */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_hdr_extract

DESCRIPTION
  Extract the header of a PRL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      prl_hdr_extract(

        prl_hdr_s_type            *prl_hdr_ptr,
            /* Pointer to PRL header where to copy extracted header
            ** information.
            */

        byte                      *prl_ptr,
            /* Pointer to PRL from which to extract the header information.
            */

        prl_sspr_p_rev_e_type     sspr_p_rev
            /* SSPR P REV information
  */
)
{

  /* Pointer to roaming list.
  */
  byte *buf_ptr;
  word offset = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( prl_hdr_ptr != NULL );
  SD_ASSERT( prl_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize buf_ptr to the start of the roaming list
  */
  buf_ptr = prl_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the PRL size field.
  */
  prl_hdr_ptr->pr_list_size = SD_B_UNPACKW(buf_ptr,
                                        offset,
                                        PR_LIST_SIZE_LENGTH);
  offset += PR_LIST_SIZE_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the PRL ID.
  */
  prl_hdr_ptr->pr_list_id = SD_B_UNPACKW(buf_ptr,
                                      offset,
                                      PR_LIST_ID_LENGTH);
  offset += PR_LIST_ID_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  prl_hdr_ptr->sspr_p_rev = sspr_p_rev;

  if (prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3) {
    prl_hdr_ptr->sspr_p_rev = (prl_sspr_p_rev_e_type)SD_B_UNPACKB(
                                        buf_ptr,
                                        offset,
                                        (word)SSPR_P_REV_LENGTH);
    offset += SSPR_P_REV_LENGTH;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the preferred only field
  */
  prl_hdr_ptr->pref_only = SD_B_UNPACKB(buf_ptr,
                                     offset,
                                     (word)PREF_ONLY_LENGTH);
  offset += PREF_ONLY_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the default roaming indicator field
  */
  prl_hdr_ptr->def_roam_ind = (sys_roam_status_e_type)SD_B_UNPACKB(buf_ptr,
                                                                offset,
                                                                ROAM_IND_LENGTH);
  offset += ROAM_IND_LENGTH;
  SD_MSG_HIGH_5( "PRL HDR EXTRACT size=%d PRL ID=%d SSPR_P_REV=%d pref_only=%d def_roam_ind = %d ", 
                                                                           prl_hdr_ptr->pr_list_size,
                                                                           prl_hdr_ptr->pr_list_id,
                                                                           prl_hdr_ptr->sspr_p_rev,
                                                                           prl_hdr_ptr->pref_only,
                                                                           prl_hdr_ptr->def_roam_ind);


  /* Map the TSB-58 roaming indicator representation to the internal DB
  ** roaming indicator representation.
  */
  prl_hdr_ptr->def_roam_ind = prl_map_tsb58_roam_to_sd_roam(
                                                 prl_hdr_ptr->def_roam_ind );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the number of acquisition records
  */
  prl_hdr_ptr->num_acq_recs = (int)SD_B_UNPACKW(buf_ptr,
                                             offset,
                                             (word)ACQ_REC_INDEX_LENGTH);
  offset += ACQ_REC_INDEX_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3)
  {
   /* Extract the number the number of records in the common subnet-ID prefix
    ** table.
    */
    prl_hdr_ptr->num_subnet_recs = (int)SD_B_UNPACKW(
                                               buf_ptr,
                                               offset,
                                               (word)NUM_SUBNET_RECS_LENGTH);
    offset += (word)NUM_SUBNET_RECS_LENGTH;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the number of system records
  */
  prl_hdr_ptr->num_sys_recs = (int)SD_B_UNPACKW(buf_ptr,
                                             offset,
                                             (word)SYS_REC_INDEX_LENGTH);
  offset += SYS_REC_INDEX_LENGTH;
  SD_MSG_MED_3("num acq recs=%d  num subnet recs=%d   num sys recs=%d", prl_hdr_ptr->num_acq_recs, 
                                                                        prl_hdr_ptr->num_subnet_recs, 
                                                                        prl_hdr_ptr->num_sys_recs);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the bit offset from the beginning of the PRL to the beginning
  ** of the acquisition table.
  */
  prl_hdr_ptr->acq_tbl_bit_offset =
    PRL_FIXED_ROAMING_LIST_LENGTH(prl_hdr_ptr->sspr_p_rev);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* prl_hdr_extract */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_read

DESCRIPTION
  Read a PRL from NV.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if PRL read is successful. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   prl_read(

        sd_nam_e_type             nam,
            /* NAM for which to read PRL.
            */

        nv_ram_roaming_list_type *prl_ptr
            /* PRL buffer to read prl data
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read into RAM the PRL that is associated with the specified NAM. If read
  ** operation fails, return FALSE.
  */
  prl_ptr->nam = (byte) nam;
  if (prl_updated_from_mmgsdi == FALSE)
  {
     /* If there is no PRL loaded from the GSDI card, read PRL from the NV.
     */
     if( !sdnv_read(NV_ROAMING_LIST_683_I, (nv_item_type*) prl_ptr) )
     {
        return FALSE;
     }
  }
  else
  {
    prl_updated_from_mmgsdi = FALSE;

  }

  SD_MSG_HIGH_3("Using the PRL read from NV!!!, NAM=%d,prl_size=%d,prl_valid=%d",\
               prl_ptr->nam,prl_ptr->size,prl_ptr->valid);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* prl_read */



/* <EJECT> */
/*===========================================================================

FUNCTION prl_create_dflt

DESCRIPTION
  Load a default PRL.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if default PRL is successfully loaded. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   prl_create_dflt(

        word                      cdma_home_sid,
            /* CDMA home SID.
            */
                                  /*lint -esym(715,cdma_home_nid) */
        word                      cdma_home_nid,
            /* CDMA home NID.
            */

        word                      amps_home_sid,
            /* AMPS home SID.
            */

        prl_sspr_p_rev_e_type     *sspr_p_rev_ptr,
            /* PRL protocol revision.
            */

        nv_ram_roaming_list_type  *prl_ptr
            /* buffer to store the prl info
            */
)
{


  sd_cell_sys_e_type        cdma_home_sys;

  sd_cell_sys_e_type        amps_home_sys   = SD_CELL_SYS_MAX;

  word                      size            = 0;

  nv_item_type              nv_data;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sspr_p_rev_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(amps_home_sys);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the PRL size to 0.
  */
  prl_ptr->size = 0;

  /* Figure out the Cellular CDMA and AMPS home-sys.
  */
  if( cdma_home_sid & 0x0001 ) {
    cdma_home_sys = SD_CELL_SYS_A;
  }
  else {
    cdma_home_sys = SD_CELL_SYS_B;
  }

  if( amps_home_sid & 0x0001 ) {
    amps_home_sys = SD_CELL_SYS_A;
  }
  else {
    amps_home_sys = SD_CELL_SYS_B;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(cdma_home_sys);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return FALSE for cases where default PRL is not supposed to be created.
  */

  (void) sdnv_read(NV_SSPR_P_REV_I,&nv_data);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Create special PRLs
  */

  /* For LTE targets which dont have 3GPP2 enabled
  **  - create single bandgroup PRL with all default G,W,L bands
  */
  #if   !defined( FEATURE_MMODE_CDMA_800 ) && \
        !defined( FEATURE_MMODE_CDMA_1900 ) && \
        !defined( FEATURE_ACP ) && \
        !defined( FEATURE_HDR ) &&\
         defined FEATURE_SD_LTE

  if(nv_data.sspr_p_rev != (byte)PRL_SSPR_P_REV_3)
  {
    nv_data.sspr_p_rev = (byte)PRL_SSPR_P_REV_3;
    #ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE
    (void)sdnv_write(NV_SSPR_P_REV_I, &nv_data);
    #endif
  }

  *sspr_p_rev_ptr = (prl_sspr_p_rev_e_type)nv_data.sspr_p_rev;

  SD_MSG_HIGH_0( "Creating GWL default PRL");

 /* SINGLE BAND PRL:
    =============== ROAMING LIST ===============

  Roaming List Type:    IS-683E
  Preferred Only:   FALSE
  Default Roaming Indicator:  0
  Preferred List ID:    8888

    *********** Acquisition Table ***********

  INDEX ACQ TYPE  CH1 CH2 CH3 CH4 CH5 CH6 CH7 - - CH31
  0 15  3 GSM 9 7:DCS-1800  UARFCN_NOT_PRESENT  4:P-GSM-900 UARFCN_NOT_PRESENT
                5:E-GSM-900 UARFCN_NOT_PRESENT  6:R-GSM-900 UARFCN_NOT_PRESENT
                3:GSM-850 UARFCN_NOT_PRESENT    8:PCS-1900  UARFCN_NOT_PRESENT
                0:GSM-450 UARFCN_NOT_PRESENT    1:GSM-480 UARFCN_NOT_PRESENT
                2:GSM-750 UARFCN_NOT_PRESENT
        WCDMA 10  BC1-IMT2G UARFCN_NOT_PRESENT  BC3-1.8G  UARFCN_NOT_PRESENT
                  BC6-800M  UARFCN_NOT_PRESENT  BC8-900M  UARFCN_NOT_PRESENT
                  BC9-1.7G  UARFCN_NOT_PRESENT  BC2-PCS1.9G UARFCN_NOT_PRESENT
                  BC4-1.7G  UARFCN_NOT_PRESENT  BC5-850M  UARFCN_NOT_PRESENT
                  BC7-2.6G  UARFCN_NOT_PRESENT  BC11-1.5G UARFCN_NOT_PRESENT
                  BC19-850M UARFCN_NOT_PRESENT
        LTE 27     EUTRAN-1  UARFCN_NOT_PRESENT  EUTRAN-2  UARFCN_NOT_PRESENT
                  EUTRAN-3 UARFCN_NOT_PRESENT   EUTRAN-4 UARFCN_NOT_PRESENT
                  EUTRAN 5 UARFCN_NOT_PRESENT  LTE 6 - LTE 14 (both inclusive)
                  LTE 17 - LTE 21 (Both inclusive)
                  LTE 23 - LTE 30 (Both inclusive)
                  LTE 31 - LTE 43 (Both inclusive)
                  LTE 66

    *********** System Table ***********

INDEX SYS TYPE  NEG/PREF  GEO PRI ACQ IND ROAM IND  ASSN INC  ASSN TAG  PN ASSN DATA ASSN SID, NID / MCC, IMSI_11_12 / SUBNETID / PLMN TYPE, PLMN, LAC
  0 2 Pref  New Same  0 0 No  0 1

0x00, 0x47, 0x22, 0xB8, 0x03, 0x00, 0x00, 0x40, 
0x00, 0x00, 0x80, 0x0F, 0x33, 0xCC, 0x90, 0x00, 
0x82, 0x06, 0x10, 0x28, 0x60, 0xE2, 0x0A, 0x50,
0x00, 0x41, 0x03, 0x08, 0x14, 0x30, 0x71, 0x02,
0x93, 0x10, 0x00, 0x41, 0x03, 0x08, 0x14, 0x30, 
0x71, 0x02, 0x45, 0x0B, 0x18, 0x34, 0x81, 0x12,
0x44, 0xCA, 0x16, 0x2E, 0x60, 0xCA, 0x04, 0x28, 
0x91, 0xA4, 0x4A, 0x99, 0x3A, 0x85, 0x2A, 0x80, 
0x29, 0x40, 0x00, 0x40, 0x00, 0xC8, 0x6D


  */
  /* Added new temporary buffer named prl_default_buffer to populate the
  ** roaming list values all at once instead of filling up
  ** prl_ptr->roaming_list buffer with values one at a time.
  */
  {

        char prl_default_buffer[] = {0x00, 0x4B, 0x22, 0xB8, 0x03, 0x00, 0x00, 0x40, 
0x00, 0x00, 0x80, 0x0F, 0x37, 0xCC, 0x90, 0x00, 
0x82, 0x06, 0x10, 0x28, 0x60, 0xE2, 0x0A, 0x58, 
0x00, 0x41, 0x03, 0x08, 0x14, 0x30, 0x71, 0x02, 
0x85, 0xA6, 0x60, 0x00, 0x82, 0x06, 0x10, 0x28, 
0x60, 0xE2, 0x04, 0x8A, 0x16, 0x30, 0x69, 0x02, 
0x24, 0x89, 0x94, 0x2C, 0x5C, 0xC1, 0x93, 0x67, 
0x0E, 0xA0, 0x42, 0x89, 0x1A, 0x44, 0xA9, 0x93, 
0xA8, 0x52, 0xA9, 0x90, 0x29, 0x40, 0x00, 0x40, 
0x00, 0x14, 0x44
    };

    /* Calculate size to be minimum of prl_ptr->roaming_list and
    ** prl_default_buffer and then copy over the values from
    ** prl_default_buffer to prl_ptr->roaming_list
    */

    /*lint -e{506} Constant value boolean */
    size = MIN(ARR_SIZE(prl_ptr->roaming_list),ARR_SIZE(prl_default_buffer));
    memscpy( (byte *)prl_ptr->roaming_list, ARR_SIZE(prl_ptr->roaming_list), prl_default_buffer, size );

  }

  /* Fill in the roaming list size.
  */
  prl_ptr->size = 8*size;

  /*lint -e774*/
  if ( size == 0 ) /*lint +e744*/
  {
    SD_MSG_LOW_0("No default roaming list created");
    return FALSE;
  }

  return TRUE; /*lint +e527 */

  /* For GSM/WCDMA only targets create a 1 channel PRL.
  */

  #elif( !defined( FEATURE_MMODE_CDMA_800 ) && \
     !defined( FEATURE_MMODE_CDMA_1900 ) && \
     !defined( FEATURE_ACP ) && \
     !defined( FEATURE_HDR ) )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(nv_data.sspr_p_rev != (byte)PRL_SSPR_P_REV_3)
  {
    nv_data.sspr_p_rev = (byte)PRL_SSPR_P_REV_3;
    #ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE
    (void)sdnv_write(NV_SSPR_P_REV_I, &nv_data);
    #endif
  }

  *sspr_p_rev_ptr = (prl_sspr_p_rev_e_type)nv_data.sspr_p_rev;

  SD_MSG_LOW_0( "Creating GSM/WCDMA default PRL");

  /*
  create default PRL:

  SINGLE BAND PRL: GSM(EGSM900/PGSM900/DCS1800/RGSM900) WCDMA(IMT2100/JAPANVI800/VIII900) GSM(850/1900) WCDMA(PCS1900/US850/BC11-1.5G/BC19-850)

    =============== ROAMING LIST ===============

    Roaming List Type:      IS-683D-Proposed
    Preferred Only:     FALSE
    Default Roaming Indicator:  0
    Preferred List ID:      8888

        *********** Acquisition Table ***********

    INDEX   ACQ TYPE    CH1 CH2 CH3 CH4 CH5 CH6 CH7 - - CH31
    0   14  0   0   7   0   4   0   5   0   6   0   3   0   8   1   0   UARFCN_INCL_NO  1   2   UARFCN_INCL_NO  1   5   UARFCN_INCL_NO  1   7   UARFCN_INCL_NO  1   8   UARFCN_INCL_NO  1   1   UARFCN_INCL_NO  1   3   UARFCN_INCL_NO  1   4   UARFCN_INCL_NO

        *********** System Table ***********

INDEX   SYS TYPE    NEG/PREF    GEO PRI ACQ IND ROAM IND    ASSN INC    ASSN TAG    PN ASSN DATA ASSN   SID, NID / MCC, IMSI_11_12 / SUBNETID / PLMN TYPE, PLMN, LAC
    0   2   Pref    New Same    0   0   No  0   1


0x00, 0x21, 0x22, 0xB8, 0x03, 0x00, 0x00, 0x40,
0x00, 0x00, 0x80, 0x0E, 0x0D, 0x19, 0x47, 0x21,
0x4C, 0x34, 0x00, 0x21, 0x47, 0x20, 0x10, 0xC4,
0x28, 0xB0, 0x29, 0x40, 0x00, 0x40, 0x00, 0x7E,
0x6C,


  */
  /* Added new temporary buffer named prl_default_buffer to populate the
  ** roaming list values all at once instead of filling up
  ** prl_ptr->roaming_list buffer with values one at a time.
  */
  {

    char prl_default_buffer[] = { 0x00, 0x21, 0x22, 0xB8, 0x03, 0x00, 0x00, 0x40,
0x00, 0x00, 0x80, 0x0E, 0x0D, 0x19, 0x47, 0x21,
0x4C, 0x34, 0x00, 0x21, 0x47, 0x20, 0x10, 0xC4,
0x28, 0xB0, 0x29, 0x40, 0x00, 0x40, 0x00, 0x7E,
0x6C};

  SD_MSG_HIGH_0( "Using Single band group PRL");

    /* Calculate size to be minimum of prl_ptr->roaming_list and
    ** prl_default_buffer and then copy over the values from
    ** prl_default_buffer to prl_ptr->roaming_list
    */

    /*lint -e{506} Constant value boolean */
    size = MIN(ARR_SIZE(prl_ptr->roaming_list),ARR_SIZE(prl_default_buffer));
    memscpy( (byte *)prl_ptr->roaming_list, ARR_SIZE(prl_ptr->roaming_list), prl_default_buffer, size );
  }

  /* Fill in the roaming list size.
  */
  prl_ptr->size = 8*size;

  return TRUE;
  /*lint complain about the code after endif,suppress it*/     /*lint -e527*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#else

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update protocol revision to 1.*/
  if (nv_data.sspr_p_rev != (byte)PRL_SSPR_P_REV_1) {
    nv_data.sspr_p_rev = (byte)PRL_SSPR_P_REV_1;
    #ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE
    (void)sdnv_write(NV_SSPR_P_REV_I, &nv_data);
    #endif
  }
  *sspr_p_rev_ptr = (prl_sspr_p_rev_e_type)nv_data.sspr_p_rev;

  #if (defined( FEATURE_MMODE_SSPR_800 ) && !defined( FEATURE_MMODE_CDMA_1900 ))

    #ifdef FEATURE_ACP

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      SD_MSG_LOW_0( "Creating CDMA-800/AMPS-800 default PRL");

      /* Roaming list file format version: 2 (IS-683-A)
      ** Number of bits:                   256
      ** List size in bytes:               32
      ** List identifier:                  0
      ** Preferred only:                   False
      ** Default roam indicator:           FLASH
      ** Number of acquisition records:    4
      ** Number of system records          4
      **
      ** Acquisition Table
      **
      **                              system/
      **         acq_   num blocks/   block/
      ** index   type   channels      chan
      ** ----------------------------------
      ** 0       2                    A  (Pri/Sec) (offset 68)
      ** 1       1                    A            (offset 76)
      ** 2       2                    B  (Pri/Sec) (offset 82)
      ** 3       1                    B            (offset 90)
      **
      ** System Table
      **
      **         nid_
      ** sid     incl  nid     neg_pref   geo   pri   acq_index   roam_ind
      ** -----------------------------------------------------------------
      ** 0       WILD  65535   Preferred  New   More  0           ON
      ** 0       WILD  65535   Preferred  Same  More  1           ON
      ** 0       WILD  65535   Preferred  Same  More  2           FLASH
      ** 0       WILD  65535   Preferred  Same  Same  3           FLASH
      **
      ** There are 0 pad bits
      ** The CRC-16 inside the list is 31543 (0x7B37)
      */
      /* Added new temporary buffer named prl_default_buffer to populate the
      ** roaming list values all at once instead of filling up
      ** prl_ptr->roaming_list buffer with values one at a time.
      */
      {
        char prl_default_buffer[] = { 0x00, 0x20, 0x00, 0x00, 0x01, 0x01, 0x00, 0x04,
        0x23, 0x10, 0x9c, 0x50, 0x00, 0x05, 0x00, 0x00,
        0x00, 0x00, 0x38, 0x04, 0x00, 0x00, 0x01, 0xc0,
        0x40, 0x40, 0x00, 0x0c, 0x03, 0x02, 0x94, 0x1A };

        /* Calculate size to be minimum of prl_ptr->roaming_list and
        ** prl_default_buffer and then copy over the values from
        ** prl_default_buffer to prl_ptr->roaming_list
        */

        /*lint -e{506} Constant value boolean */
        size = MIN(ARR_SIZE(prl_ptr->roaming_list),ARR_SIZE(prl_default_buffer));
        memscpy( (byte *)prl_ptr->roaming_list, ARR_SIZE(prl_ptr->roaming_list), prl_default_buffer, size );
      }

      /* Fill in the roaming list size.
      */
      prl_ptr->size = 8*size;

      /* Change the Acquisition Table A_B fields as is necessary to make the
      ** power-up scan order and the priorities of the System Table consistent
      ** with the home sides of CDMA and analog.
      */
      if( (cdma_home_sys == NV_SYSTEM_B) &&
          (amps_home_sys == NV_SYSTEM_B) )
      {
        b_packb( PRL_AB_SYSTEM_B, (byte*) prl_ptr->roaming_list, 68, 2 );
        b_packb( PRL_AB_SYSTEM_B, (byte*) prl_ptr->roaming_list, 76, 2 );
        b_packb( PRL_AB_SYSTEM_A, (byte*) prl_ptr->roaming_list, 82, 2 );
        b_packb( PRL_AB_SYSTEM_A, (byte*) prl_ptr->roaming_list, 90, 2 );
      }
      else if( (cdma_home_sys == NV_SYSTEM_A) &&
               (amps_home_sys == NV_SYSTEM_B) )
      {
        b_packb( PRL_AB_SYSTEM_B, (byte*) prl_ptr->roaming_list, 76, 2 );
        b_packb( PRL_AB_SYSTEM_A, (byte*) prl_ptr->roaming_list, 90, 2 );
      }
      else if( (cdma_home_sys == NV_SYSTEM_B) &&
               (amps_home_sys == NV_SYSTEM_A) )
      {
        b_packb( PRL_AB_SYSTEM_B, (byte*) prl_ptr->roaming_list, 68, 2 );
        b_packb( PRL_AB_SYSTEM_A, (byte*) prl_ptr->roaming_list, 82, 2 );
      }

       /* PRL created, return sucess.
      */
      return TRUE;


    #else /* FEATURE_ACP */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      SD_MSG_LOW_0( "Creating CDMA-800 default PRL");

/* Default roaming list depends on the CDMA home side */

      /* Roaming list file format version: 2 (IS-683-A)
      ** Number of bits:                   176
      ** List size in bytes:               22
      ** List identifier:                  0
      ** Preferred only:                   False
      ** Default roam indicator:           FLASH
      ** Number of acquisition records:    2
      ** Number of system records          2
      **
      ** Acquisition Table
      **
      **                              system/
      **         acq_   num blocks/   block/
      ** index   type   channels      chan
      ** ----------------------------------
      ** 0       2                    A  (Pri/Sec) (offset 68)
      ** 2       2                    B  (Pri/Sec) (offset 82)
      **
      ** System Table
      **
      **         nid_
      ** sid     incl  nid     neg_pref   geo   pri   acq_index   roam_ind
      ** -----------------------------------------------------------------
      ** 0       WILD  65535   Preferred  New   More  0           ON
      ** 0       WILD  65535   Preferred  Same  Same  1           FLASH
      **
      ** There are 0 pad bits
      ** The CRC-16 inside the list is 0x9c60
      */
      /* Added new temporary buffer named prl_default_buffer to populate the
      ** roaming list values all at once instead of filling up
      ** prl_ptr->roaming_list buffer with values one at a time.
      */
      {
        char prl_default_buffer[] = { 0x00, 0x16, 0x00, 0x01, 0x01, 0x00, 0x80, 0x02,
        0x23, 0x27, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x40, 0x80, 0x9c, 0x60 };

        /* Calculate size to be minimum of prl_ptr->roaming_list and
        ** prl_default_buffer and then copy over the values from
        ** prl_default_buffer to prl_ptr->roaming_list
        */

        /*lint -e{506} Constant value boolean */
        size = MIN(ARR_SIZE(prl_ptr->roaming_list),ARR_SIZE(prl_default_buffer));
        memscpy( (byte *)prl_ptr->roaming_list, ARR_SIZE(prl_ptr->roaming_list), prl_default_buffer, size );
      }

      /* Fill in the roaming list size.
      */
     prl_ptr->size = 8*size;

      /* Change the Acquisition Table A_B fields as is necessary to make the
      ** power-up scan order and the priorities of the System Table consistent
      ** with the home side of CDMA.
      */
      if( cdma_home_sys == (sd_cell_sys_e_type) NV_SYSTEM_B )
      {
        b_packb( (byte)PRL_AB_SYSTEM_B, (byte*) prl_ptr->roaming_list, 68, 2 );
        b_packb( (byte)PRL_AB_SYSTEM_A, (byte*) prl_ptr->roaming_list, 76, 2 );
      }

      /* PRL created, return sucess.
      */
      return TRUE;

    #endif /* ! FEATURE_ACP */ /*lint -e527*/

  #else

    /*************************************************************************
    ** Section: Create Generic PRL for all other cases.
    *************************************************************************/
    /*lint -e527 */  /* Unreachable */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    SD_MSG_LOW_0( "Creating CDMA-800/1900/AMPS-800 default PRL");

    /* Create the following roaming list:
    =============== ROAMING LIST ===============

  Roaming List Type:    IS-683C
  Preferred Only:   FALSE
  Default Roaming Indicator:  2
  Preferred List ID:    1

    *********** Acquisition Table ***********

  INDEX ACQ TYPE  CH1 CH2 CH3 CH4 CH5 CH6 CH7 - - CH31
  0 1 B
  1 1 A
  2 2 B Both
  3 2 A Both
  4 10  0 283 0 691 0 384 0 777 0 779 0 738 0 486 0 568 0 40  0 1022
  5 10  1 25  1 50  1 75  1 100 1 125 1 150 1 175 1 200 1 225 1 250 1 275 1 325 1 350 1 375 1 425 1 450 1 475 1 500 1 525 1 550 1 575 1 600 1 625 1 650 1 675 1 725 1 750 1 775 1 825 1 850 1 875 1 925
  6 10  1 950 1 975 1 1000  1 1025  1 1050  1 1075  1 1100  1 1125  1 1150  1 1175
  7 10  15  25  15  50  15  75  15  100 15  125 15  150 15  175 15  225 15  250 15  275 15  300 15  325 15  350 15  375 15  425 15  450 15  475 15  525 15  550 15  575 15  625 15  650 15  675 15  725 15  750 15  775 15  800 15  825 15  850 15  875
  8 10  18  45  18  70  18  95  18  145 18  170 18  195
  9 10  19  23  19  48  19  73  19  98  19  143 19  168 19  193 19  218 19  263 19  288 19  313 19  338
  10  5 Any

    *********** System Table ***********

INDEX SYS TYPE  NEG/PREF  GEO PRI ACQ IND ROAM IND  ASSN INC  ASSN TAG  PN ASSN DATA ASSN SID, NID / MCC, IMSI_11_12 / SUBNETID / PLMN TYPE, PLMN, LAC
  0 0 Pref  New Same  4 0 No  0 65535
  1 0 Pref  Same  Same  5 0 No  0 65535
  2 0 Pref  Same  Same  6 0 No  0 65535
  3 0 Pref  Same  Same  7 0 No  0 65535
  4 0 Pref  Same  Same  8 0 No  0 65535
  5 0 Pref  Same  Same  9 0 No  0 65535
  6 0 Pref  Same  Same  10  2 No  0 65535
  7 0 Pref  Same  Same  2 0 No  0 65535
  8 0 Pref  Same  More  3 2 No  0 65535
  9 0 Pref  Same  Same  0 0 No  0 65535
  10  0 Pref  Same  Same  1 2 No  0 65535

  */
 /* Added new temporary buffer named prl_default_buffer to populate the
    ** roaming list values all at once instead of filling up
    ** prl_ptr->roaming_list buffer with values one at a time.
    */
    {
      char prl_default_buffer[] = { 0x01, 0x32, 0x00, 0x01, 0x03, 0x01, 0x02, 0xC0,
      0x00, 0x05, 0x80, 0x01, 0x01, 0x40, 0x01, 0x01,
      0x00, 0x02, 0x01, 0x70, 0x02, 0x01, 0x30, 0x0A,
      0x14, 0x01, 0x1B, 0x02, 0xB3, 0x01, 0x80, 0x03,
      0x09, 0x03, 0x0B, 0x02, 0xE2, 0x01, 0xE6, 0x02,
      0x38, 0x00, 0x28, 0x03, 0xFE, 0x0A, 0x40, 0x08,
      0x19, 0x08, 0x32, 0x08, 0x4B, 0x08, 0x64, 0x08,
      0x7D, 0x08, 0x96, 0x08, 0xAF, 0x08, 0xC8, 0x08,
      0xE1, 0x08, 0xFA, 0x09, 0x13, 0x09, 0x45, 0x09,
      0x5E, 0x09, 0x77, 0x09, 0xA9, 0x09, 0xC2, 0x09,
      0xDB, 0x09, 0xF4, 0x0A, 0x0D, 0x0A, 0x26, 0x0A,
      0x3F, 0x0A, 0x58, 0x0A, 0x71, 0x0A, 0x8A, 0x0A,
      0xA3, 0x0A, 0xD5, 0x0A, 0xEE, 0x0B, 0x07, 0x0B,
      0x39, 0x0B, 0x52, 0x0B, 0x6B, 0x0B, 0x9D, 0x0A,
      0x14, 0x0B, 0xB6, 0x0B, 0xCF, 0x0B, 0xE8, 0x0C,
      0x01, 0x0C, 0x1A, 0x0C, 0x33, 0x0C, 0x4C, 0x0C,
      0x65, 0x0C, 0x7E, 0x0C, 0x97, 0x0A, 0x3C, 0x78,
      0x19, 0x78, 0x32, 0x78, 0x4B, 0x78, 0x64, 0x78,
      0x7D, 0x78, 0x96, 0x78, 0xAF, 0x78, 0xE1, 0x78,
      0xFA, 0x79, 0x13, 0x79, 0x2C, 0x79, 0x45, 0x79,
      0x5E, 0x79, 0x77, 0x79, 0xA9, 0x79, 0xC2, 0x79,
      0xDB, 0x7A, 0x0D, 0x7A, 0x26, 0x7A, 0x3F, 0x7A,
      0x71, 0x7A, 0x8A, 0x7A, 0xA3, 0x7A, 0xD5, 0x7A,
      0xEE, 0x7B, 0x07, 0x7B, 0x20, 0x7B, 0x39, 0x7B,
      0x52, 0x7B, 0x6B, 0x0A, 0x0C, 0x90, 0x2D, 0x90,
      0x46, 0x90, 0x5F, 0x90, 0x91, 0x90, 0xAA, 0x90,
      0xC3, 0x0A, 0x18, 0x98, 0x17, 0x98, 0x30, 0x98,
      0x49, 0x98, 0x62, 0x98, 0x8F, 0x98, 0xA8, 0x98,
      0xC1, 0x98, 0xDA, 0x99, 0x07, 0x99, 0x20, 0x99,
      0x39, 0x99, 0x52, 0x05, 0x01, 0x3C, 0x30, 0x40,
      0x20, 0x00, 0x00, 0x00, 0x30, 0x60, 0x28, 0x00,
      0x00, 0x00, 0x30, 0x60, 0x30, 0x00, 0x00, 0x00,
      0x30, 0x60, 0x38, 0x00, 0x00, 0x00, 0x30, 0x60,
      0x40, 0x00, 0x00, 0x00, 0x30, 0x60, 0x48, 0x00,
      0x00, 0x00, 0x30, 0x60, 0x50, 0x00, 0x00, 0x04,
      0x30, 0x60, 0x10, 0x00, 0x00, 0x00, 0x30, 0x70,
      0x18, 0x00, 0x00, 0x04, 0x30, 0x60, 0x00, 0x00,
      0x00, 0x00, 0x30, 0x60, 0x08, 0x00, 0x00, 0x04,
      0xF2, 0x02};
      
      /* Calculate size to be minimum of prl_ptr->roaming_list and
      ** prl_default_buffer and then copy over the values from
      ** prl_default_buffer to prl_ptr->roaming_list
      */

      /*lint -e{506} Constant value boolean */
    size = MIN(ARR_SIZE(prl_ptr->roaming_list),ARR_SIZE(prl_default_buffer));
      memscpy( (byte *)prl_ptr->roaming_list, ARR_SIZE(prl_ptr->roaming_list), prl_default_buffer, size );
    }

    /* Update the sspr_p_rev to PRL_SSPR_P_REV_3
    */
    if(nv_data.sspr_p_rev != (byte)PRL_SSPR_P_REV_3)
    {
      nv_data.sspr_p_rev = (byte)PRL_SSPR_P_REV_3;
      #ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE
      (void)sdnv_write(NV_SSPR_P_REV_I, &nv_data);
      #endif
    }
    *sspr_p_rev_ptr = (prl_sspr_p_rev_e_type)nv_data.sspr_p_rev;

    /* Fill in the roaming list size.
    */
    prl_ptr->size = 8*size;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*lint -e774*/
    if ( size == 0 ) /*lint +e744*/
    {
      SD_MSG_LOW_0("No default roaming list created");
      return FALSE;
    }

    return TRUE; /*lint +e527 */

  #endif /* (defined( FEATURE_MMODE_SSPR_800 ) && !defined( FEATURE_MMODE_CDMA_1900 )) */

#endif /*!defined( FEATURE_MMODE_CDMA_800 ) && !defined( FEATURE_MMODE_CDMA_1900 ) && \
       ** !defined( FEATURE_ACP ) &&  !defined( FEATURE_HDR )*/
                           /*lint +e527*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* prl_create_dflt */ /*lint +esym(715,cdma_home_nid)*/ /*lint +e527*/

/*===========================================================================

FUNCTION prl_unq_sys_q_dealloc

DESCRIPTION
  Deallocate memory assigned for unq_sys_q.

DEPENDENCIES
  None
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void prl_unq_sys_q_dealloc(q_type * unq_sys_q_ptr)
{
  unq_mode_band_chan_type *cur_sys_ptr;

    // delete all the items from the queue
  while((cur_sys_ptr = q_get(unq_sys_q_ptr)) != NULL)
    {
      modem_mem_free((void *) cur_sys_ptr, MODEM_MEM_CLIENT_MMODE);
    } /* end while */
  q_destroy(unq_sys_q_ptr);
  }


/*===========================================================================

FUNCTION prl_is_sys_present

DESCRIPTION

Checks if sys is already parsed earlier

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if sys was already parsed earlier
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

static boolean prl_is_sys_present(

  sd_mode_e_type        mode,
     /* Mode - AMPS or CDMA
     */

  sd_band_e_type        band,
     /* Band class preference.
     */

  sd_chan_type          chan,
     /* CDMA Channel/AMPS A/B system,
     */

  q_type   *            unq_sys_q_ptr
  /* pointer to the unique sys record table
     */

)
{
  int sys_q_counter = 0;
  unq_mode_band_chan_type *temp_sys_ptr = NULL;
  sd_sys_s_type prl_sys;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sys_q_counter = q_cnt(unq_sys_q_ptr);
  SD_ASSERT( INRANGE(sys_q_counter, -1, ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) ));
  
  
  if( sys_q_counter <= 0 )
  {
    return FALSE;
  }

  prl_sys.mode = mode;
  prl_sys.chan = chan;
  prl_sys.band = sd_map_band_to_band_pref( band );

  temp_sys_ptr = q_check(unq_sys_q_ptr);

  while((sys_q_counter > 0) && (temp_sys_ptr != NULL))
    {
      if(sd_misc_is_sys_same(temp_sys_ptr->tbl_ptr, prl_sys) )
      {
        return TRUE;
      }
      sys_q_counter--;
      temp_sys_ptr = q_next(unq_sys_q_ptr, &(temp_sys_ptr->link));
    } /* end while */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return FALSE;

} /* prl_is_sys_present */


/*===========================================================================

FUNCTION prl_insert_sys

DESCRIPTION

Inserts the sys in the temp dB

DEPENDENCIES
  None.

RETURN VALUE
  The count of the number of entries in the dB


SIDE EFFECTS
  None.

===========================================================================*/

static int prl_insert_sys(

  sd_mode_e_type        mode,
     /* Mode - AMPS or CDMA
     */

  sd_band_e_type        band,
     /* Band class preference.
     */

  sd_chan_type          chan,
     /* CDMA Channel/AMPS A/B system,
     */


  q_type   *            unq_sys_q_ptr
  /* pointer to the unique sys record table
     */
)
{
  int sys_q_counter = 0;
  unq_mode_band_chan_type *new_sys_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_q_counter = q_cnt( unq_sys_q_ptr);
  SD_ASSERT( INRANGE( sys_q_counter, -1, ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (sys_q_counter + 1) >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
  {
    return (sys_q_counter + 1);
  }

  new_sys_ptr = (unq_mode_band_chan_type *)\
                             modem_mem_alloc( sizeof(unq_mode_band_chan_type),
                                                      MODEM_MEM_CLIENT_MMODE);
  if(new_sys_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }
  
  (void) q_link( new_sys_ptr, &new_sys_ptr->link );  /* init link */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  new_sys_ptr->tbl_ptr.band = sd_map_band_to_band_pref( band );
  new_sys_ptr->tbl_ptr.chan = chan;
  new_sys_ptr->tbl_ptr.mode = mode;

 // check if this is the first entry into queue
  if ( sys_q_counter == 0 )
  {
    q_put( unq_sys_q_ptr, &new_sys_ptr->link);
  }
  else
  {
    /* Insert the new system at the beginning of the queue*/
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_insert( unq_sys_q_ptr,
           & new_sys_ptr->link,
           & ((unq_mode_band_chan_type*) q_check( unq_sys_q_ptr ))->link );
#else
    q_insert( & new_sys_ptr->link,
           & ((unq_mode_band_chan_type*) q_check( unq_sys_q_ptr ))->link );

#endif /* FEATURE_Q_NO_SELF_QPTR */
  }

  return (sys_q_counter + 1);

} /* prl_insert_sys */


/* <EJECT> */
/*===========================================================================

FUNCTION prl_validate_proc

DESCRIPTION
  Validate a specified PRL.

  The following validations are made:

    - The number of acquisition records must be greater than 0.
    - The size of the PRL should actually match with PR_LIST_SIZE field
    - The number of system records must be greater than 0.
    - The type field of each acquisition record must be valid.
    - The a_b field of each cellular acquisition record must not equal the
      reserved value.
    - The block field of each PCS CDMA Using Blocks record must not equal the
      reserved value.
    - The chan field of each PCS CDMA Using Channels record must be between
      25 and 1175 (inclusive).
    - The GEO field of the first system record must indicate that this is the
      start of a new geographical region.
    - The acq_index field of each system record must be less than the number
      of acquisition records.

DEPENDENCIES
  None.

RETURN VALUE
  PRL_VALID if the roaming list is valid. Specific PRL_INVALID_XXX code
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static   prl_validation_enum_type    prl_validate_proc(

        byte                        *prl_ptr,
            /* Pointer to the PRL to validate.
            */

        word                        *prl_ver_ptr,
            /* Pointer where to save the PRL version.
            */

        prl_sspr_p_rev_e_type       sspr_p_rev,
            /* PRL protocol revision.
            */

        boolean                     *is_mcc_sys_rec_ptr
            /* Pointer where to return whether system table contains any
            ** MCC/IMIS_11_12 system record.
            */
)
{
  byte *buf_ptr;              /* Pointer to roaming list */
  int i;                      /* Loop counter */
  int j;                      /* Loop counter */
  int k;                      /* Loop counter */
  int num_chans;              /* num_chans field from acquisition record */
  int num_blocks;             /* num_blocks field from acquisition record */

  byte acq_rec_len;
  word band_chan;
  sd_mode_e_type mode;
  sd_band_e_type band;
  sd_ss_band_pref_e_type band_pref;
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || \
      defined(FEATURE_SD_LTE)
  byte num_bands = 0;         /* to remove the compile warning */
  prl_gsm_band_e_type gsm_band;
  prl_wcdma_band_e_type wcdma_band;
  prl_uarfcn_incl_e_type uarfcn_incl;
  word uarfcn;
  prl_gw_pref_sys_ind_e_type pref_sys_ind;
  word num_gsm_bands;
  word num_wcdma_bands;
  #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED) */

  #if defined(FEATURE_SD_LTE) || defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  byte                    num_of_rats = 0;
  sys_sys_mode_e_type     sys_mode = SYS_SYS_MODE_NONE;
  prl_gsm_band_e_type     prl_gsm_band;
  prl_wcdma_band_e_type   prl_wcdma_band;
  prl_lte_band_e_type     prl_lte_band;
  prl_tds_band_e_type     prl_tds_band;
  #endif

  dword next_acq_rec_offset;

  prl_geo_region_type geo;    /* The geo field from a sys_rec */
  byte acq_index;             /* The acq_index field from a sys_rec */
  prl_nid_incl_type nid_incl; /* The nid_incl field from a sys_rec */
  prl_neg_pref_type neg_pref; /* The neg_pref field from a sys_rec */

  dword offset;               /* Bit offset into the roaming list */
  word chan;                  /* The chan field from an acq_rec */
  prl_ab_type a_b;            /* The a_b field from an sys_rec */
  byte block;                 /* The block field from an acq_rec */
  word sid;

  prl_acq_rec_type acquisition_type;  /* Acquisition record type */

  word pr_list_crc;
  word crc_calc;
  boolean is_supp_acq_rec = FALSE;

  sd_blksys_e_type  blksys;

  prl_hdr_s_type    prlhdr;
  prl_hdr_s_type    *prl_hdr_ptr = &prlhdr;

  int unq_sys_cnt = -1;

  /* Actual size over which CRC is calculated */
  word byte_cnt_for_crc = 0;

  /* Queue for maintining unique sd_sys_s info - mode, band and channel.
  */
  q_type     unq_sys_q;
  q_type *   unq_sys_q_ptr = &unq_sys_q;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*  ensure the unique system queue is initialized 
  */
  memset(unq_sys_q_ptr, 0, sizeof(q_type));
  (void) q_init( unq_sys_q_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( prl_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if SD task is ready or not */
  if( !sdtask_is_init_complete() )
  {
    SD_MSG_HIGH_0("FAILED - SD Task not Ready");
    prl_unq_sys_q_dealloc(unq_sys_q_ptr);
    return PRL_INVALID_SD_NOT_READY;
  }

  /* Validate the PRL header.
  */
  SD_MSG_LOW_1( "Validate for SSPR_P_REV=%d PRL", sspr_p_rev);
  /* Extract the PRL header information.
  */
  prl_hdr_extract( prl_hdr_ptr, prl_ptr, sspr_p_rev );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate the size of the PRL.
  */
  if( prl_hdr_ptr->pr_list_size < MINIMUM_PRL_SIZE(prl_hdr_ptr->sspr_p_rev))
  {
    SD_MSG_MED_2( "FAILED - PRL Size=%d < %d bytes",
         prl_hdr_ptr->pr_list_size,
         MINIMUM_PRL_SIZE(prl_hdr_ptr->sspr_p_rev));

    prl_unq_sys_q_dealloc(unq_sys_q_ptr);
    return PRL_INVALID_SIZE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the ID of the PRL.
  */
  if( prl_ver_ptr != NULL )
  {
    *prl_ver_ptr = prl_hdr_ptr->pr_list_id;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the beginning of the PRL and the acquisition table within the
  ** PRL.
  */
  buf_ptr = prl_ptr;
  offset  = prl_hdr_ptr->acq_tbl_bit_offset;

  /*
  ** Compute and check the CRC for the table.
  */

  /*
  ** Extract PRL CRC.
  ** CRC in the PRL points at the bottom of the PRL and it takes 2 bytes.
  ** PRL List Size in the header includes 2 bytes of the CRC.
  ** CRC bytes are not included in CRC calculation.
  */

  byte_cnt_for_crc = prl_hdr_ptr->pr_list_size - (CRC_LENGTH/8);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* pos is of size word. Its easier to offset buf_ptr when pos
  ** can be more than 65535 for a 16k PRL
  ** Extract the CRC value from PRL
  */
  pr_list_crc = SD_B_UNPACKW (buf_ptr,
                               (byte_cnt_for_crc * 8),
                               CRC_LENGTH);

  crc_calc = (word) CRC_16_STEP_SEED;

  /* CRC is calculated in chunks of 65536 bits of data since
  ** crc_16_xx api allows only a word sized length field.
  */
  crc_calc = crc_16_step( crc_calc, buf_ptr, byte_cnt_for_crc);

  if (crc_calc != pr_list_crc)
  {
    SD_MSG_MED_2("FAILED - Invalid CRC calc %0x prl %0x", crc_calc, pr_list_crc);
    prl_unq_sys_q_dealloc(unq_sys_q_ptr);
    return PRL_INVALID_CRC;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate the format revision (SSPR_P_REV) of the PRL.
  */
  if( prl_hdr_ptr->sspr_p_rev != PRL_SSPR_P_REV_3 &&
      prl_hdr_ptr->sspr_p_rev != PRL_SSPR_P_REV_1 )
  {
    SD_MSG_MED_1("FAILED - Invalid SSPR_P_REV=%d", prl_hdr_ptr->sspr_p_rev);
    prl_unq_sys_q_dealloc(unq_sys_q_ptr);
    return PRL_INVALID_SSPR_P_REV;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate the number of acquisition records
  */
  if( prl_hdr_ptr->num_acq_recs == 0 ||
       prl_hdr_ptr->num_sys_recs == 0)
  {
    SD_MSG_MED_2("FAILED - acquisition records: %d,system records:%d",
                       prl_hdr_ptr->num_acq_recs,prl_hdr_ptr->num_sys_recs);
    prl_unq_sys_q_dealloc(unq_sys_q_ptr);
    return PRL_INVALID_NUM_OF_RECORDS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate the acq_table
  */

  /* Point at the beginning of the PRL and the acquisition table within the
  ** PRL.
  */
  buf_ptr = prl_ptr;
  offset  = prl_hdr_ptr->acq_tbl_bit_offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3 )
  {
    /* Acquisition table must start on a byte boundery.
    */
    if( offset & 0x0007 )
    {
      SD_ERR_0( "FAILED - Invalid table offset");
      prl_unq_sys_q_dealloc(unq_sys_q_ptr);
      return PRL_INVALID_RECORD;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; i < (int) prl_hdr_ptr->num_acq_recs; i++)
  {

    /* Unpack the acquisition type
    */
    acquisition_type = (prl_acq_rec_type)SD_B_UNPACKB(
                        buf_ptr,
                        offset,
                        (word)ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev) );
    offset += ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev);

    if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_EQPRL)
    {
      SD_MSG_LOW_4 ("offset %d acq type %d bytepos %d bitpos %d",
                  offset, acquisition_type, offset/8, offset%8);
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Unpack the acquisition record length field.
    */
    acq_rec_len = SD_B_UNPACKB(
                     buf_ptr,
                        offset,
                     (word)ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) ) ;

    if( (acq_rec_len == 0) && (prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3) )
    {
      SD_ERR_1("FAILED - Invalid record length %d", acq_rec_len);
    }

    offset += ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev);

    /* Record the offset of the next record.
    */
    next_acq_rec_offset = offset + acq_rec_len*8;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    switch( acquisition_type )
    {
      case PRL_ACQ_REC_RESERVED :         /* Reserved */
        SD_MSG_MED_0("FAILED - Reserved acq_type");
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case PRL_ACQ_REC_CELLULAR_ANALOG :  /* Cellular analog */
        a_b = (prl_ab_type)SD_B_UNPACKB( buf_ptr,
                                      offset,
                                      FSIZ(prl_cellular_analog_type, a_b));

        /* Validate the a_b field.
        */
        if (a_b == PRL_AB_RESERVED)
        {
          SD_MSG_MED_0("FAILED - Reserved a_b");
        }
        offset += sizeof(prl_cellular_analog_type);

        #ifdef FEATURE_ACP
        is_supp_acq_rec = TRUE;
        #endif

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case PRL_ACQ_REC_CELLULAR_CDMA_STD :
        a_b = (prl_ab_type)SD_B_UNPACKB( buf_ptr,
                                      offset,
                                      FSIZ(prl_cellular_cdma_type, a_b));

        /* Validate the a_b field.
        */
        if (a_b == PRL_AB_RESERVED)
        {
          SD_MSG_MED_0("FAILED - Reserved a_b");
        }
        offset += sizeof(prl_cellular_cdma_type);

        #ifdef FEATURE_MMODE_CDMA_800
        is_supp_acq_rec = TRUE;
        #endif

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case PRL_ACQ_REC_CELLULAR_CDMA_CUST :
        num_chans =SD_B_UNPACKB(buf_ptr,
                             offset,
                             FSIZ(prl_cellular_cdma_custom_type, num_chans));
        offset += FSIZ(prl_cellular_cdma_custom_type, num_chans);

        /* Validate each CDMA channel.
        */
        for (j = 0; j < num_chans; j++)
        {
          chan = SD_B_UNPACKW( buf_ptr,
                            offset,
                            FSIZ(prl_cellular_cdma_custom_type, chan));
          offset += FSIZ(prl_cellular_cdma_custom_type, chan);

          blksys = sd_map_chan_to_blksys( SD_MODE_CDMA,
                                             SD_BAND_CELL,
                                             chan );

          if(blksys == SD_BLKSYS_MAX && 
             (sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID))
          {
            SD_MSG_MED_1("FAILED - Invalid CDMA channel %d in Cellular Band",
                    chan);
          }
          else if( blksys != SD_BLKSYS_MAX )
          {
            /* check if we have this entry in the table, insert this, if
            ** we don't have the entry
            */
            if( !prl_is_sys_present( SD_MODE_CDMA,
                                     SD_BAND_CELL,
                                     chan,
                                     unq_sys_q_ptr ) )
            {
              unq_sys_cnt = prl_insert_sys( SD_MODE_CDMA,
                                            SD_BAND_CELL,
                                            chan,
                                            unq_sys_q_ptr );

              if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
              {                
                prl_unq_sys_q_dealloc(unq_sys_q_ptr );      
                return PRL_INVALID_UNIQUE_SYS;
              }
            }
          }
        }

        #ifdef FEATURE_MMODE_CDMA_800
        is_supp_acq_rec = TRUE;
        #endif

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case PRL_ACQ_REC_CELLULAR_GENERIC : /* Cellular generic */
        a_b = (prl_ab_type)SD_B_UNPACKB( buf_ptr,
                                      offset,
                                      FSIZ(prl_cellular_generic_type, a_b));

        /* Validate the a_b field
        */
        if (a_b == PRL_AB_RESERVED)
        {
          SD_MSG_MED_0("FAILED - Reserved a_b");
        }
        offset += sizeof(prl_cellular_generic_type);

        #if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_ACP) )
        is_supp_acq_rec = TRUE;
        #endif

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case PRL_ACQ_REC_PCS_CDMA_BLOCKS :  /* PCS CDMA using blocks */
        num_blocks = SD_B_UNPACKB( buf_ptr,
                                offset,
          FSIZ(prl_pcs_cdma_using_blocks_type, num_blocks));
        offset += FSIZ(prl_pcs_cdma_using_blocks_type, num_blocks);

        /* Validate each block.
        */
        for (j = 0; j < num_blocks; j++)
        {
          block = SD_B_UNPACKB( buf_ptr,
                             offset,
                             FSIZ(prl_pcs_cdma_using_blocks_type, block));
          offset += FSIZ(prl_pcs_cdma_using_blocks_type, block);
          if (block == PRL_BLOCK_RESERVED)
          {
            SD_MSG_MED_0("FAILED - Reserved block");
            continue;
          }

          /* Add all channels in the block to the unq_mode_band_chan_tbl
          */
          for( k=0; k < (int) ARR_SIZE(prl_pcs_block_chan_tbl); k++ )
          {
            /* check if we have this entry in the table, insert this, if
            ** we don't have the entry
            */
            if( !prl_is_sys_present( SD_MODE_CDMA,
                                     SD_BAND_PCS,
                                     prl_pcs_block_chan_tbl[k].chan,
                                     unq_sys_q_ptr ) )
            {
              unq_sys_cnt = prl_insert_sys( SD_MODE_CDMA,
                                            SD_BAND_PCS,
                                            prl_pcs_block_chan_tbl[k].chan,
                                            unq_sys_q_ptr );

              if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
              {
                prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                return PRL_INVALID_UNIQUE_SYS;
              }
            } /* end of table check */
          } /* end of block loop */
        } /* end of all blocks loop */

        #ifdef FEATURE_MMODE_CDMA_1900
        is_supp_acq_rec = TRUE;
        #endif

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case PRL_ACQ_REC_PCS_CDMA_CHANNELS :/* PCS CDMA using channels */
        num_chans = SD_B_UNPACKB(buf_ptr,
                              offset,
          FSIZ(prl_pcs_cdma_using_chans_type, num_chans));
        offset += FSIZ(prl_pcs_cdma_using_chans_type, num_chans);

        /* Validate each CDMA channel.
        */
        for (j = 0; j < num_chans; j++)
        {
          chan = SD_B_UNPACKW( buf_ptr,
                            offset,
                            FSIZ(prl_pcs_cdma_using_chans_type, chan));
          offset += FSIZ(prl_pcs_cdma_using_chans_type, chan);

          blksys = sd_map_chan_to_blksys( SD_MODE_CDMA,
                                             SD_BAND_PCS,
                                             chan );
          if(blksys == SD_BLKSYS_MAX && 
             (sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID))
          {
            SD_MSG_MED_1("FAILED - Invalid CDMA channel %d in PCS band", chan);
          }
          else if( blksys != SD_BLKSYS_MAX )
          {
            /* check if we have this entry in the table, insert this, if
            ** we don't have the entry
            */
            if( !prl_is_sys_present( SD_MODE_CDMA,
                                     SD_BAND_PCS,
                                     chan,
                                     unq_sys_q_ptr ) )
            {
              unq_sys_cnt = prl_insert_sys( SD_MODE_CDMA,
                                            SD_BAND_PCS,
                                            chan,
                                            unq_sys_q_ptr );

              if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
              {
                prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                return PRL_INVALID_UNIQUE_SYS;
              }
            }
          }
        }

        #ifdef FEATURE_MMODE_CDMA_1900
        is_supp_acq_rec = TRUE;
        #endif

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* The following three acq record types apply for IS683B PRL or above*/

      case PRL_ACQ_REC_JTACS_CDMA_STD:
        a_b = (prl_ab_type)SD_B_UNPACKB( buf_ptr,
                                      offset,
                                      FSIZ(prl_jtacs_cdma_std_type, a_b));
        /* Validate the a_b field.
        */
        if (a_b == PRL_AB_RESERVED)
        {
          SD_MSG_MED_0("FAILED - Reserved a_b");
        }
        offset += sizeof(prl_jtacs_cdma_std_type);

        is_supp_acq_rec = TRUE;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case PRL_ACQ_REC_JTACS_CDMA_CUST:
        num_chans = SD_B_UNPACKB( buf_ptr,
                               offset,
                               FSIZ(prl_jtacs_cdma_cust_type, num_chans));
        offset += FSIZ(prl_cellular_cdma_custom_type, num_chans);

        /* Validate each CDMA channel.
        */
        for (j = 0; j < num_chans; j++)
        {
          chan = SD_B_UNPACKW( buf_ptr,
                            offset,
                            FSIZ(prl_jtacs_cdma_cust_type, chan));
          offset += FSIZ(prl_jtacs_cdma_cust_type, chan);

          blksys = sd_map_chan_to_blksys( SD_MODE_CDMA,
                                             SD_BAND_BC3,
                                             chan );

          if(blksys == SD_BLKSYS_MAX && 
             (sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID))
          {
            SD_MSG_MED_1("FAILED - Invalid CDMA channel %d in Band Class 3",
                    chan);
          }
          else if( blksys != SD_BLKSYS_MAX )
          {
            /* check if we have this entry in the table, insert this, if
            ** we don't have the entry
            */
            if( !prl_is_sys_present( SD_MODE_CDMA,
                                     SD_BAND_BC3,
                                     chan,
                                     unq_sys_q_ptr ) )
            {
              unq_sys_cnt = prl_insert_sys( SD_MODE_CDMA,
                                            SD_BAND_BC3,
                                            chan,
                                            unq_sys_q_ptr );

              if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
              {
                prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                return PRL_INVALID_UNIQUE_SYS;
              }
            }
          }
        }

        is_supp_acq_rec = TRUE;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case PRL_ACQ_REC_CDMA_2GHZ_CHANS:
        num_chans = SD_B_UNPACKB( buf_ptr,
                               offset,
                               FSIZ(prl_cdma_2ghz_chans_type, num_chans));
        offset += FSIZ(prl_cdma_2ghz_chans_type, num_chans);

        /* Validate each CDMA channel.
        */
        for (j = 0; j < num_chans; j++)
        {
          chan = SD_B_UNPACKW( buf_ptr,
                            offset,
                            FSIZ(prl_cdma_2ghz_chans_type, chan));
          offset += FSIZ(prl_cdma_2ghz_chans_type, chan);

          blksys = sd_map_chan_to_blksys( SD_MODE_CDMA,
                                             SD_BAND_BC6,
                                             chan );

          if(blksys == SD_BLKSYS_MAX && 
             (sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID))
          {
            SD_MSG_MED_1("FAILED - Invalid CDMA channel %d in Band Class 6",
                    chan);
          }
          else if( blksys != SD_BLKSYS_MAX )
          {
            /* check if we have this entry in the table, insert this, if
            ** we don't have the entry
            */
            if( !prl_is_sys_present( SD_MODE_CDMA,
                                     SD_BAND_BC6,
                                     chan,
                                     unq_sys_q_ptr ) )
            {
              unq_sys_cnt = prl_insert_sys( SD_MODE_CDMA,
                                            SD_BAND_BC6,
                                            chan,
                                            unq_sys_q_ptr );

              if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
              {
                prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                return PRL_INVALID_UNIQUE_SYS;
              }
            }
          }
        }

        is_supp_acq_rec = TRUE;

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* The following two acq record types apply for IS683C PRL or above.*/

      case PRL_ACQ_REC_CDMA_GENERIC:  /* CDMA generic */
      case PRL_ACQ_REC_HDR_GENERIC:   /* HDR generic */

        /* Compute the number of records.
        */
        num_chans = acq_rec_len / 2;

        if( num_chans == 0 )
        {
          SD_MSG_MED_1("FAILED - Invalid record length %d", acq_rec_len);
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Validate each channel record.
        */
        for( j=0; j < num_chans; j++ )
        {
          band_chan = SD_B_UNPACKW( buf_ptr,
                                 offset,
                                 FSIZ(prl_cdma_generic_type, rec) );

          offset  += FSIZ(prl_cdma_generic_type, rec);

          band    = PRLI_CDMA_GENERIC_BAND( band_chan );
          chan    = PRLI_CDMA_GENERIC_CHAN( band_chan );
          band_pref = sd_map_band_to_band_pref(band);

          if( acquisition_type == PRL_ACQ_REC_CDMA_GENERIC ) {
            mode = SD_MODE_CDMA;
          }
          else {
            mode = SD_MODE_HDR;
          }

          if( sd_map_chan_to_blksys(mode, band, chan) != SD_BLKSYS_MAX )
          {
            /* check if we have this entry in the table, insert this, if
            ** we don't have the entry
            */
            if( !prl_is_sys_present( mode, band, chan, unq_sys_q_ptr ))
            {
              unq_sys_cnt = prl_insert_sys( mode, band, chan, unq_sys_q_ptr );

              if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
              {
                prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                return PRL_INVALID_UNIQUE_SYS;
              }
            }
          }

          if( sdss_is_supp_mode_band(mode,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE) )
          {
            is_supp_acq_rec = TRUE;
          }
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* The following two acq record types apply for IS683D PRL or above.*/

      case PRL_ACQ_REC_GSM_GENERIC:

        #if defined(SD_GSM_SUPPORTED) 
        /* Compute the number of records
        */
        num_bands = SD_B_UNPACKB( buf_ptr,
                               offset,
                               FSIZ(prl_gsm_generic_type, num_bands) );

        offset += FSIZ(prl_gsm_generic_type, num_bands);

        if( num_bands == 0 )
        {
          SD_MSG_MED_0("FAILED - Invalid number of GSM bands: 0");
        }
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Validate each GSM band.
        */
        for (j=0; j<num_bands; j++ )
        {

          gsm_band = (prl_gsm_band_e_type)SD_B_UNPACKB(
                                buf_ptr,
                                offset,
                                (word)FSIZ(prl_gsm_generic_type, band) );

          offset += FSIZ(prl_gsm_generic_type, band);


          band = prl_map_gsm_band_to_band(gsm_band);
          band_pref = sd_map_band_to_band_pref(band);

          if( sdss_is_supp_mode_band(SD_MODE_GSM,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE) )
          {
            is_supp_acq_rec = TRUE;
          }

          /* PRL_WILDCARD_UARFCN is used as a place holder for the channel
          */

          if( !prl_is_sys_present( SD_MODE_GSM, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr))
          {
            unq_sys_cnt = prl_insert_sys( SD_MODE_GSM, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr );

            if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
            {
              prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
              return PRL_INVALID_UNIQUE_SYS;
            }
          }/* if sys not present */

        }

        #endif /* (SD_GSM_SUPPORTED) */
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case PRL_ACQ_REC_WCDMA_GENERIC:

        #if defined(SD_WCDMA_SUPPORTED) 
        /* Compute the number of records
        */
        num_bands = SD_B_UNPACKB( buf_ptr,
                               offset,
                               FSIZ(prl_wcdma_generic_type, num_bands) );

        offset += FSIZ(prl_wcdma_generic_type, num_bands);

        if (num_bands == 0)
        {
          SD_MSG_MED_0("FAILED - Invalid number of WCDMA bands: 0");
        }
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Validate each WCDMA band and UTRA Absolute Radio Freq Chan Num.
        */
        for (j=0; j<num_bands; j++)
        {

          wcdma_band = (prl_wcdma_band_e_type)SD_B_UNPACKB(
                                  buf_ptr,
                                  offset,
                                  (word)FSIZ(prl_wcdma_generic_type, band) );

          offset += FSIZ(prl_wcdma_generic_type, band);

          band = prl_map_wcdma_band_to_band(wcdma_band);
          band_pref = sd_map_band_to_band_pref(band);

          uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                                 buf_ptr,
                                 offset,
                                 (word)FSIZ(prl_wcdma_generic_type, uarfcn_incl) );

          offset += FSIZ(prl_wcdma_generic_type, uarfcn_incl);

          if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
          {
            SD_ERR_1("FAILED - Invalid UARFCN_INCL of WCDMA bands %d",
                    uarfcn_incl);
          }

          if (uarfcn_incl == PRL_UARFCN_INCL)
          {
            uarfcn = SD_B_UNPACKW( buf_ptr,
                                offset,
                                FSIZ(prl_wcdma_generic_type, uarfcn) );

            offset += FSIZ(prl_wcdma_generic_type, uarfcn);

            if( (sd_map_chan_to_blksys(SD_MODE_WCDMA,band,uarfcn) == SD_BLKSYS_MAX)&&
              (sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID) )
            {
              SD_MSG_MED_2("FAILED - Invalid WCDMA band %d and chan %d ",
                      band, uarfcn);
            }
            else if(sd_map_chan_to_blksys(SD_MODE_WCDMA,band,uarfcn) != SD_BLKSYS_MAX)
            {
              /* check if we have this entry in the table, insert this, if
              ** we don't have the entry
              */
              if( !prl_is_sys_present( SD_MODE_WCDMA, band, uarfcn, unq_sys_q_ptr))
              {
                unq_sys_cnt = prl_insert_sys( SD_MODE_WCDMA, band, uarfcn, unq_sys_q_ptr );

                if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
                {
                  prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                  return PRL_INVALID_UNIQUE_SYS;
                }
              }/* if sys not present */
            } /* else */
          } /* if uarfcn included */
          else if( !prl_is_sys_present( SD_MODE_WCDMA, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr ))
          {
            unq_sys_cnt = prl_insert_sys( SD_MODE_WCDMA, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr );

            if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
            {
              prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
              return PRL_INVALID_UNIQUE_SYS;
            }
          }/* if sys not present */

          if( sdss_is_supp_mode_band(SD_MODE_WCDMA,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE) )
          {
            is_supp_acq_rec = TRUE;
          }

        } /* for */

        #endif /* (SD_WCDMA_SUPPORTED) */
        break;

      case PRL_ACQ_REC_GW_PREFERRED:

        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) 
        /* Check the GSM/WCDMA preferred system indicator, number of GW bands
        */
        pref_sys_ind = (prl_gw_pref_sys_ind_e_type)SD_B_UNPACKB(
                                  buf_ptr,
                                  offset,
                                  (word)FSIZ(prl_gw_pref_type, pref_sys_ind) );

        offset += FSIZ(prl_gw_pref_type, pref_sys_ind);

        if (pref_sys_ind >= PRL_GW_PREF_SYS_IND_RESERVED)
        {
          SD_ERR_1( "FAILED - Invalid GW preferred sys indicator %d",
                   pref_sys_ind);
        }

        num_gsm_bands = SD_B_UNPACKB( buf_ptr,
                                   offset,
                                   FSIZ(prl_gw_pref_type, num_gsm_bands));

        offset += FSIZ(prl_gw_pref_type, num_gsm_bands);

        num_wcdma_bands = SD_B_UNPACKB( buf_ptr,
                                     offset,
                                   FSIZ(prl_gw_pref_type, num_wcdma_bands));

        offset += FSIZ(prl_gw_pref_type, num_wcdma_bands);

        if ( (num_wcdma_bands == 0) &&
             (num_gsm_bands == 0))
        {
          SD_ERR_1("FAILED - Invalid number of GW bands %d",
                 (num_wcdma_bands+num_gsm_bands));
        }

        /* Check GSM band classes
        */
        for( j=0; j<num_gsm_bands; j++)
        {
          gsm_band = (prl_gsm_band_e_type)SD_B_UNPACKB(
                                buf_ptr,
                                offset,
                                (word)FSIZ(prl_gw_pref_type, gsm_band) );

          offset += FSIZ(prl_gw_pref_type, gsm_band);

          band = prl_map_gsm_band_to_band(gsm_band);
          band_pref = sd_map_band_to_band_pref(band);

          if( sdss_is_supp_mode_band(SD_MODE_GSM,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE) )
          {
            is_supp_acq_rec = TRUE;
          }

          if( !prl_is_sys_present( SD_MODE_GSM, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr ))
          {
            unq_sys_cnt = prl_insert_sys( SD_MODE_GSM, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr );

            if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
            {
              prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
              return PRL_INVALID_UNIQUE_SYS;
            }
          }/* if sys not present */

        }

        /* Check WCDMA band classes and channels.
        */
        for( j=0; j<num_wcdma_bands; j++)
        {
          wcdma_band = (prl_wcdma_band_e_type)SD_B_UNPACKB(
                                  buf_ptr,
                                  offset,
                                  (word)FSIZ(prl_gw_pref_type, wcdma_band) );

          offset += FSIZ(prl_gw_pref_type, wcdma_band);

          band = prl_map_wcdma_band_to_band(wcdma_band);
          band_pref = sd_map_band_to_band_pref(band);

          uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                                  buf_ptr,
                                  offset,
                                  (word)FSIZ(prl_gw_pref_type,uarfcn_incl) );

          offset += FSIZ(prl_gw_pref_type,uarfcn_incl);

          if ( uarfcn_incl >= PRL_UARFCN_INCL_RESERVED )
          {
            SD_ERR_1("FAILED - Invalid UARFCN_INCL %d", uarfcn_incl);
          }

          if ( uarfcn_incl == PRL_UARFCN_INCL )
          {
            uarfcn = SD_B_UNPACKW( buf_ptr,
                                offset,
                                FSIZ(prl_gw_pref_type, uarfcn) );

            offset += FSIZ(prl_gw_pref_type, uarfcn);

            if( (sd_map_chan_to_blksys(SD_MODE_WCDMA,band,uarfcn) == SD_BLKSYS_MAX)&&
              (sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID) )
            {
              SD_MSG_MED_2("FAILED - Invalid wcdma band %d and chan %d",
                      band, uarfcn);
            }
            else if(sd_map_chan_to_blksys(SD_MODE_WCDMA,band,uarfcn) == SD_BLKSYS_MAX)
            {
              /* check if we have this entry in the table, insert this, if
              ** we don't have the entry
              */
              if( !prl_is_sys_present( SD_MODE_WCDMA, band, uarfcn, unq_sys_q_ptr ))
              {
                unq_sys_cnt = prl_insert_sys( SD_MODE_WCDMA, band, uarfcn, unq_sys_q_ptr);

                if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
                {
                  prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                  return PRL_INVALID_UNIQUE_SYS;
                }
              }
            } /* else */

          } /* If uarfcn not included */
          else if( !prl_is_sys_present( SD_MODE_WCDMA, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr))
          {
            unq_sys_cnt = prl_insert_sys( SD_MODE_WCDMA, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr );

            if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
            {
              prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
              return PRL_INVALID_UNIQUE_SYS;
            }
          }/* if sys not present */

          if( sdss_is_supp_mode_band(SD_MODE_WCDMA,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE) )
          {
            is_supp_acq_rec = TRUE;
          }

        } /* for */

        #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED) */
        break;

      case PRL_ACQ_REC_GWL_GENERIC:
        #if defined(FEATURE_SD_LTE) || defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
        num_of_rats = SD_B_UNPACKB(buf_ptr,
                      offset,
                      FSIZ(prl_gwl_generic_type, num_of_rats) );
        offset += FSIZ(prl_gwl_generic_type, num_of_rats);

        if (num_of_rats > SYS_MAX_NUM_3GPP_RATS)
        {
          SD_ERR_1( "FAILED - Invalid GWL num_of_rats %d",
                        num_of_rats);

          break;
        }

        for (j = 0; j < num_of_rats; j++)
        {
          sys_mode = (sys_sys_mode_e_type)SD_B_UNPACKB(buf_ptr,
                     offset,
                     FSIZ(prl_gwl_generic_type, sys_mode));
          offset += FSIZ(prl_gwl_generic_type, sys_mode);

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          switch(sys_mode)
          {
            case SYS_SYS_MODE_GSM:
            {
              /* Get the number of bands field
              */
              num_bands = SD_B_UNPACKB(buf_ptr,
                          offset,
                          FSIZ(prl_gwl_generic_type, num_bands) );
              offset += FSIZ(prl_gwl_generic_type, num_bands);

              if ((num_bands == 0) ||
                  (num_bands > PRL_MAX_GWL_GENERIC_BAND))
              {
                SD_ERR_0("FAILED - Invalid number of GSM bands:0");

                break;
              }

              /* Get band class, and channel field
              */
              for( k=0; k< num_bands; k++)
              {
                prl_gsm_band = (prl_gsm_band_e_type)SD_B_UNPACKB(
                               buf_ptr,
                               offset,
                               (word)FSIZ(prl_gwl_generic_type, band) );

                band = prl_map_gsm_band_to_band(prl_gsm_band);
                band_pref = sd_map_band_to_band_pref(band);

                offset += FSIZ(prl_gwl_generic_type, band);

                uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                              buf_ptr,
                              offset,
                              (word)FSIZ(prl_gwl_generic_type, uarfcn_incl) );

                offset += FSIZ(prl_gwl_generic_type, uarfcn_incl);

                if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
                {
                  SD_ERR_1("FAILED - Invalid ARFCN_INCL of GSM bands %d",
                               uarfcn_incl);
                }

                if (uarfcn_incl == PRL_UARFCN_INCL)
                {
                  uarfcn = SD_B_UNPACKW( buf_ptr,
                                         offset,
                                         FSIZ(prl_gwl_generic_type, uarfcn) );

                  offset += FSIZ(prl_gwl_generic_type, uarfcn);

                  if( (sd_map_chan_to_blksys(SD_MODE_GSM,band,uarfcn) == SD_BLKSYS_MAX)&&
                    (sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID) )
                  {
                    SD_ERR_2("FAILED - Invalid GSM band %d and chan %d ",
                                 band, uarfcn);
                  }
                  else if(sd_map_chan_to_blksys(SD_MODE_GSM,band,uarfcn) == SD_BLKSYS_MAX)
                  {
                    /* check if we have this entry in the table, insert this, if
                    ** we don't have the entry
                    */
                    if( !prl_is_sys_present( SD_MODE_GSM, band, uarfcn, unq_sys_q_ptr ))
                    {
                      unq_sys_cnt = prl_insert_sys( SD_MODE_GSM, band, uarfcn, unq_sys_q_ptr );

                      if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
                      {
                        prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                        return PRL_INVALID_UNIQUE_SYS;
                      }
                    }
                  } /* else */

                } /* If uarfcn not included */
                else if( !prl_is_sys_present( SD_MODE_GSM, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr ))
                {
                  unq_sys_cnt = prl_insert_sys( SD_MODE_GSM, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr );

                  if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
                  {
                    prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                    return PRL_INVALID_UNIQUE_SYS;
                  }
                }/* if sys not present */

                if( sdss_is_supp_mode_band(SD_MODE_GSM,
                                           band_pref,
                                           SYS_LTE_BAND_MASK_CONST_NONE,
                                           SD_SS_BAND_PREF_NONE) )
                {
                  is_supp_acq_rec = TRUE;
                }
              } /* for */

              break;
            }

            case SYS_SYS_MODE_WCDMA:
            {
              /* Get the number of bands field
              */
              num_bands = SD_B_UNPACKB(buf_ptr,
                          offset,
                          FSIZ(prl_gwl_generic_type, num_bands) );
              offset += FSIZ(prl_gwl_generic_type, num_bands);

              if ((num_bands == 0) ||
                  (num_bands > PRL_MAX_GWL_GENERIC_BAND))
              {
                SD_ERR_0("FAILED - Invalid number of WCDMA bands:0");

                break;
              }

              /* Get band class, and channel field
              */
              for( k=0; k< num_bands; k++)
              {
                prl_wcdma_band = (prl_wcdma_band_e_type)SD_B_UNPACKB(
                                 buf_ptr,
                                 offset,
                                 (word)FSIZ(prl_gwl_generic_type, band) );

                band = prl_map_wcdma_band_to_band(prl_wcdma_band);
                band_pref = sd_map_band_to_band_pref(band);

                offset += FSIZ(prl_gwl_generic_type, band);

                uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                              buf_ptr,
                              offset,
                              (word)FSIZ(prl_gwl_generic_type, uarfcn_incl) );

                offset += FSIZ(prl_gwl_generic_type, uarfcn_incl);

                if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
                {
                  SD_ERR_1("FAILED - Invalid UARFCN_INCL of WCDMA bands %d",
                               uarfcn_incl);
                }

                if (uarfcn_incl == PRL_UARFCN_INCL)
                {
                  uarfcn = SD_B_UNPACKW( buf_ptr,
                                         offset,
                                         FSIZ(prl_gwl_generic_type, uarfcn) );

                  offset += FSIZ(prl_gwl_generic_type, uarfcn);

                  if( (sd_map_chan_to_blksys(SD_MODE_WCDMA,band,uarfcn) == SD_BLKSYS_MAX)&&
                    (sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID) )
                  {
                    SD_ERR_2("FAILED - Invalid WCDMA band %d and chan %d ",
                                  band, uarfcn);
                  }
                  else if(sd_map_chan_to_blksys(SD_MODE_WCDMA,band,uarfcn) == SD_BLKSYS_MAX)
                  {
                    /* check if we have this entry in the table, insert this, if
                    ** we don't have the entry
                    */
                    if( !prl_is_sys_present( SD_MODE_WCDMA, band, uarfcn, unq_sys_q_ptr ))
                    {
                      unq_sys_cnt = prl_insert_sys( SD_MODE_WCDMA, band, uarfcn, unq_sys_q_ptr );

                      if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
                      {
                        prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                        return PRL_INVALID_UNIQUE_SYS;
                      }
                    }
                  } /* else */

                } /* If uarfcn not included */
                else if( !prl_is_sys_present( SD_MODE_WCDMA, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr ))
                {
                  unq_sys_cnt = prl_insert_sys( SD_MODE_WCDMA, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr);

                  if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
                  {
                    prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                    return PRL_INVALID_UNIQUE_SYS;
                  }
                }/* if sys not present */

                if( sdss_is_supp_mode_band(SD_MODE_WCDMA,
                                           band_pref,
                                           SYS_LTE_BAND_MASK_CONST_NONE,
                                           SD_SS_BAND_PREF_NONE) )
                {
                  is_supp_acq_rec = TRUE;
                }

              } /* for */

              break;
            }

            case SYS_SYS_MODE_LTE:
            {
              /* Get the number of bands field
              */
              num_bands = SD_B_UNPACKB(buf_ptr,
                          offset,
                          FSIZ(prl_gwl_generic_type, num_bands) );
              offset += FSIZ(prl_gwl_generic_type, num_bands);

              if ((num_bands == 0) ||
                  (num_bands > PRL_MAX_GWL_GENERIC_BAND))
              {
                SD_ERR_0("FAILED - Invalid number of LTE bands:0");

                break;
              }

              /* Get band class, and channel field
              */
              for( k=0; k< num_bands; k++)
              {
                prl_lte_band = (prl_lte_band_e_type)SD_B_UNPACKB(
                               buf_ptr,
                               offset,
                               (word)FSIZ(prl_gwl_generic_type, band) );

                band = prl_map_lte_band_to_band(prl_lte_band);
                band_pref = sys_map_lte_band_mask_to_band_mask(sd_map_band_to_band_pref_lte(band));

                offset += FSIZ(prl_gwl_generic_type, band);

                uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                              buf_ptr,
                              offset,
                              (word)FSIZ(prl_gwl_generic_type, uarfcn_incl) );

                offset += FSIZ(prl_gwl_generic_type, uarfcn_incl);

                if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
                {
                  SD_ERR_1("FAILED - Invalid UARFCN_INCL of LTE bands %d",
                                uarfcn_incl);
                }

                if (uarfcn_incl == PRL_UARFCN_INCL)
                {
                  uarfcn = SD_B_UNPACKW( buf_ptr,
                           offset,
                           FSIZ(prl_gwl_generic_type, uarfcn) );

                  offset += FSIZ(prl_gwl_generic_type, uarfcn);


                }
                else if( !prl_is_sys_present( SD_MODE_LTE, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr ))
                {
                  unq_sys_cnt = prl_insert_sys( SD_MODE_LTE, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr );

                  if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
                  {
                    prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                    return PRL_INVALID_UNIQUE_SYS;
                  }
                }/* if sys not present */

                if( sdss_is_supp_mode_band(SD_MODE_LTE,
                                           SD_SS_BAND_PREF_NONE,
                                           sys_map_band_mask_to_lte_band_mask(band_pref),
                                           SD_SS_BAND_PREF_NONE) )
                {
                  is_supp_acq_rec = TRUE;
                }

              } // end of for loop
              break;
            }

            case SYS_SYS_MODE_TDS:
            {
              /* Get the number of bands field
              */
              num_bands = SD_B_UNPACKB(buf_ptr,
                          offset,
                          FSIZ(prl_gwl_generic_type, num_bands) );
              offset += FSIZ(prl_gwl_generic_type, num_bands);

              if ((num_bands == 0) ||
                  (num_bands > PRL_MAX_GWL_GENERIC_BAND))
              {
                SD_ERR_0("FAILED - Invalid number of TDS bands:0");

                break;
              }

              /* Get band class, and channel field
              */
              for( k=0; k< num_bands; k++)
              {
                prl_tds_band = (prl_tds_band_e_type)SD_B_UNPACKB(
                               buf_ptr,
                               offset,
                               (word)FSIZ(prl_gwl_generic_type, band) );

                band = prl_map_tds_band_to_band(prl_tds_band);
                band_pref = sd_map_band_to_band_pref_tds(band);

                offset += FSIZ(prl_gwl_generic_type, band);

                uarfcn_incl = (prl_uarfcn_incl_e_type)SD_B_UNPACKB(
                              buf_ptr,
                              offset,
                              (word)FSIZ(prl_gwl_generic_type, uarfcn_incl) );

                offset += FSIZ(prl_gwl_generic_type, uarfcn_incl);

                if (uarfcn_incl >= PRL_UARFCN_INCL_RESERVED)
                {
                  SD_ERR_1("FAILED - Invalid UARFCN_INCL of TDS bands %d",
                                uarfcn_incl);
                }

                if (uarfcn_incl == PRL_UARFCN_INCL)
                {
                  uarfcn = SD_B_UNPACKW( buf_ptr,
                           offset,
                           FSIZ(prl_gwl_generic_type, uarfcn) );

                  offset += FSIZ(prl_gwl_generic_type, uarfcn);


                }
                else if( !prl_is_sys_present( SD_MODE_TDS, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr  ))
                {
                  unq_sys_cnt = prl_insert_sys( SD_MODE_TDS, band, PRL_WILDCARD_UARFCN, unq_sys_q_ptr  );

                  if( unq_sys_cnt >= ( PRL_TBL_MAX - PRL_DEFAULT_ENTRIES ) )
                  {
                    prl_unq_sys_q_dealloc(unq_sys_q_ptr );   
                    return PRL_INVALID_UNIQUE_SYS;
                  }
                }/* if sys not present */

                if( sdss_is_supp_mode_band(SD_MODE_TDS,
                                           SD_SS_BAND_PREF_NONE,
                                           SYS_LTE_BAND_MASK_CONST_NONE,
                                           band_pref) )
                {
                  is_supp_acq_rec = TRUE;
                }

              } // end of for loop
              break;
            }

            case SYS_SYS_MODE_NONE:
            case SYS_SYS_MODE_NO_SRV:
            case SYS_SYS_MODE_AMPS:
            case SYS_SYS_MODE_CDMA:
            case SYS_SYS_MODE_HDR:
            case SYS_SYS_MODE_GPS:
            case SYS_SYS_MODE_GW:
            case SYS_SYS_MODE_GWL:
            case SYS_SYS_MODE_MAX:
            default:
            {
              SD_MSG_MED_1( "FAILED - Invalid sys_mode of GWL record %d",
                            (int)sys_mode);
            }
          } // end of switch
        }

        #endif /* (FEATURE_SD_LTE) || (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED) */
        break;

      case PRL_ACQ_REC_NONE:
      default:
        SD_ERR_1("FAILED - Unrecognized acq_type %d", acquisition_type);

        /* For PRL SSPR P REV 1, return
        ** OW skip over undefined acq type.
        */
       if( prl_hdr_ptr->sspr_p_rev != PRL_SSPR_P_REV_1 )
       {
         is_supp_acq_rec = TRUE;
       }
       break;

    } /* switch */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3)
    {
      /* Validate the acquisition record length.
      */
      if( offset > next_acq_rec_offset )
      {
        SD_ERR_1("FAILED - Invalid record length %d", acq_rec_len);
      }

      offset = next_acq_rec_offset;
    }

  } /* for */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Deallocate memory for the unique system record table now that the acquistion
  ** record table has been validated.
  */
  prl_unq_sys_q_dealloc(unq_sys_q_ptr );
  unq_sys_q_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If none of the acquisition records is supported by this target, indicate
  ** that the PRL is invalid.
  */
  if( ! is_supp_acq_rec )
  {
    return PRL_INVALID_RECORD;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3 )
  {
    /* Subnet table must start on a byte boundery.
    */
    if( offset & 0x0007 )
    {
      SD_ERR_0( "FAILED - Invalid table offset");
      return PRL_INVALID_RECORD;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Jump over the subnet table.
    */
    for( i=0; i < prl_hdr_ptr->num_subnet_recs; i++ )
    {
      byte common_length;

      offset += COM_SUBNET_REC_RESERVED_LENGTH;

      common_length = SD_B_UNPACKB( buf_ptr,
                                 offset,
                                 COM_SUBNET_REC_LENGTH_LENGTH );

      offset += COM_SUBNET_REC_LENGTH_LENGTH;

      offset += common_length*8;
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate the system table
  */
  if( prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3 )
  {
    /* System table must start on a byte boundery.
    */
    if( offset & 0x0007 )
    {
      SD_ERR_0( "FAILED - Invalid table offset");
      return PRL_INVALID_RECORD;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially assume there is no MCC/IMSI_11_12 record in the system table.
  */
  if( is_mcc_sys_rec_ptr != NULL ) {
    *is_mcc_sys_rec_ptr = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; i < prl_hdr_ptr->num_sys_recs; i++ )
  {

    if( prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3 )
    {
      prli_sys_rec_type         sys_rec;

      offset = prl_extract_sys_rec( buf_ptr, offset, &sys_rec,
                                    prl_hdr_ptr->sspr_p_rev );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      /* For IS-2000 record if SID = PRL_MCC_SID_0 or PRL_MCC_SID_1, indicate
      ** that the system table contains MCC/IMIS_11_12 based records.
      */
      if( is_mcc_sys_rec_ptr != NULL &&
          sys_rec.rec_type == PRL_SYS_REC_IS2000 )
      {
        if( sys_rec.rec.is2000.sid == PRL_MCC_SID_0 ||
            sys_rec.rec.is2000.sid == PRL_MCC_SID_1 )
        {
          *is_mcc_sys_rec_ptr = TRUE;
        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Validate the system record type.
      */
      if( sys_rec.rec_type >= PRL_SYS_REC_MAX )
      {
        SD_ERR_1("FAILED - Invalid sys_type %d", sys_rec.rec_type);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If this is the first system record, the GEO bit must indicate that
      ** this is the start of a new geographical region
      */
      if( i == 0 )
      {
        if( sys_rec.geo != PRL_NEW_GEO_REGION )
        {
          SD_ERR_0("FAILED - Invalid GEO");
          return PRL_INVALID_RECORD;
        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Validate the acq_index
      */
      if( sys_rec.acq_index >= prl_hdr_ptr->num_acq_recs)
      {
        SD_ERR_0("FAILED - Invalid acq_index");
        return PRL_INVALID_RECORD;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Validate the NID_INC field.
      */
      if( sys_rec.rec_type == PRL_SYS_REC_IS2000 &&
          sys_rec.rec.is2000.nid_incl >= PRL_NID_INCL_RESERVED )
      {
        SD_ERR_0("FAILED - Invalid NID_INC");
        return PRL_INVALID_RECORD;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Validate the subnet length.
      */
      if( sys_rec.rec_type == PRL_SYS_REC_IS856 &&
          sys_rec.rec.is856.subnet_length > SYS_REC_MAX_SUBNET_ID_LENGTH *8 )
      {
        SD_ERR_0("FAILED - Invalid subnet length");
        return PRL_INVALID_RECORD;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Validate the subnet length for MCC_MNC based system record.
      */
      if( sys_rec.rec_type == PRL_SYS_REC_MCC_MNC &&
          sys_rec.rec.mcc_mnc.mcc_mnc_subtype == \
          PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID)
      {
        for( j=0; j<sys_rec.rec.mcc_mnc.info.subnet_id.num_subnet_id; j++ )
        {
          if( sys_rec.rec.mcc_mnc.info.subnet_id.subnet_length[j] >=
                                           SYS_REC_MAX_SUBNET_ID_LENGTH *8 )
          {
            SD_ERR_0("FAILED - Invalid subnet length");
            return PRL_INVALID_RECORD;
          }
        } // end of for loop

      }


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Validate the PLMN_LAC_INCL, PLMN select mode, PLMN ID, and LAC ID.
      */
      if( sys_rec.rec_type == PRL_SYS_REC_PLMN &&
          ((sys_rec.rec.plmn.net_select_mode >= SD_NET_SELECT_MODE_MAX) ||
           !sys_plmn_id_is_valid(sys_rec.rec.plmn.plmn_id))
        )
      {
        SD_ERR_0( "FAILED - Invalid plmn id or network select mode");
        return PRL_INVALID_RECORD;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Skip over the system record with any other cases.
      */
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, this is an PRL_SSPR_P_REV_1 PRL.
    */
    else
    {
      /* Extract the NID field.
      ** SD_B_UNPACKW cannot take a dword for arg
      ** pos.
      */
      sid = SD_B_UNPACKW( buf_ptr,
                       offset,
                       FSIZ(prl_sys_record_type, sid));
      offset += FSIZ(prl_sys_record_type, sid);

      /* If SID = PRL_MCC_SID_0 or PRL_MCC_SID_1, indicate that the system
      ** table contains MCC/IMIS_11_12 based records.
      */
      if( (sid == PRL_MCC_SID_0 ||
           sid == PRL_MCC_SID_1 ) &&
           is_mcc_sys_rec_ptr != NULL
         )
      {
        *is_mcc_sys_rec_ptr = TRUE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the NID_INCL field
      */
      nid_incl = (prl_nid_incl_type) SD_B_UNPACKB(buf_ptr, offset,
        FSIZ(prl_sys_record_type, nid_incl));
      offset += FSIZ(prl_sys_record_type, nid_incl);

      /* If a NID is included, skip the NID field
      */
      if (nid_incl == PRL_NID_INCL)
      {
        offset += FSIZ(prl_sys_record_type, nid);
      }

      /* Else if NID_INC is not WILDCARD or PUBLIC, this is an invalid
      ** record.
      */
      else if ( nid_incl != PRL_NID_NOT_INCL_WILDCARD_NID &&
                nid_incl != PRL_NID_NOT_INCL_PUBLIC_NID )
      {
        SD_MSG_MED_0("FAILED - Invalid NID_INC");
        return PRL_INVALID_RECORD;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the NEG_PREF field
      */
      neg_pref = (prl_neg_pref_type)SD_B_UNPACKB(buf_ptr, offset,
        FSIZ(prl_sys_record_type, neg_pref));
      offset += FSIZ(prl_sys_record_type, neg_pref);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the GEO field
      */
      geo = (prl_geo_region_type)SD_B_UNPACKB(buf_ptr, offset,
        FSIZ(prl_sys_record_type, geo));
      offset += FSIZ(prl_sys_record_type, geo);

      /* If this is the first system record, the GEO bit must indicate that
      ** this is the start of a new geographical region
      */
      if (i == 0)
      {
        if (geo != PRL_NEW_GEO_REGION)
        {
          SD_MSG_MED_0("FAILED - Invalid GEO");
          return PRL_INVALID_RECORD;
        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Skip the PRI Field, Only If it is a Preferred Entry.
      */
      if (neg_pref == PRL_PREFERRED)
      {
        offset += FSIZ(prl_sys_record_type, pri);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the ACQ_INDEX field
      */
      acq_index = SD_B_UNPACKB(buf_ptr, offset,
        FSIZ(prl_sys_record_type, acq_index));
      offset += FSIZ(prl_sys_record_type, acq_index);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Validate the acq_index
      */
      if (acq_index >= prl_hdr_ptr->num_acq_recs)
      {
        SD_MSG_MED_0("FAILED - Invalid acq_index");
        return PRL_INVALID_RECORD;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Skip the ROAM_IND field if this is a preferred entry
      */
      if (neg_pref == PRL_PREFERRED)
      {
        offset += FSIZ(prl_sys_record_type, roam_ind);
      }

    } /* if */

  } /* for */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Skip the Reserved Bits.
  ** If there is nothing more, then may be these are reserved bits for this
  ** PRL version. Lets just move over to the CRC.
  */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Check that offset is in the valid boundry and hasn't stepped over CRC.
  ** CRC occupies 2 bytes at the end of the PRL.
  */
  if((offset/8) > (dword)(prl_hdr_ptr->pr_list_size - 2) )
  {
    SD_ERR_0("FAILED - Invalid prl_hdr_ptr->pr_list_size");
    return PRL_INVALID_SIZE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the PRL is valid - return TRUE.
  */
  return PRL_VALID;

} /* prl_validate_proc */




/*===========================================================================

FUNCTION prl_validate_base

DESCRIPTION
  Validate a specified PRL.

  This function is expected to be called by clients such as OTASP or DIAG
  upon writing a new PRL into NV. The returned validation indication should
  be saved along with the PRL.

  System determination consults the validation indication when reading the
  PRL from NV at power-up or NAM change.

  The function return the PRL version number through the prl_ver_ptr
  parameter.

  The function update the PRL protocol revision through the sspr_p_rev_ptr
  if is_full_validation is set to true.

  The following validations are made:

    - If is_full_validation is true, validate with the specific PRL
      protocol revision first. If fail, try other protocl revisions and
      update protocol revision parameter.
    - If is_full_validation is false, validate only with specified
      PRL protocol revision.

     It is api caller's responsibility to validate that the size bytes 
     present in prl buffer are correct and same as the actual prl size. 
     The first two bytes of the prl indicate the prl size

DEPENDENCIES
  None.

RETURN VALUE
  PRL_VALID if the roaming list is valid. Specific PRL_INVALID_XXX code
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  prl_validation_enum_type  prl_validate_base(

        byte                      *prl_ptr,
        /* Pointer to the PRL to validate */

        word                      *prl_ver_ptr,
            /* Pointer where to save the PRL version.
            */

        prl_sspr_p_rev_e_type     *sspr_p_rev_ptr,
            /* PRL SSPR_P_REV information
            */

        boolean                   is_full_validation,
            /* Indicate whether to do full validation.
            */

        boolean                   *is_mcc_sys_rec_ptr
            /* Pointer where to return whether system table contains any
            ** MCC/IMIS_11_12 system record.
            */
)
{
  prl_validation_enum_type status;
  prl_sspr_p_rev_e_type  sspr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( prl_ptr != NULL );
  SD_ASSERT( sspr_p_rev_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate based on protocol revision.
         */
  sspr = *sspr_p_rev_ptr;

  status = prl_validate_proc( prl_ptr,
                              prl_ver_ptr,
                              sspr,
                              is_mcc_sys_rec_ptr );

  SD_MSG_HIGH_2("PRL validate status:%d, p_rev:%d", status,sspr);

  if( ( status != PRL_VALID ) && is_full_validation )
  {
    /* Validate with other protocol revision
    */
    if ( sspr == PRL_SSPR_P_REV_1 )
    {
      sspr = PRL_SSPR_P_REV_3;
    }
    else
    {
      sspr = PRL_SSPR_P_REV_1;
    }

    status = prl_validate_proc( prl_ptr,
                                prl_ver_ptr,
                                sspr,
                                is_mcc_sys_rec_ptr );
    
    SD_MSG_HIGH_2("PRL validate status:%d, p_rev:%d", status,sspr);
  }

  if( (status == PRL_VALID) && (sspr != *sspr_p_rev_ptr) )
  {
    /* Update protocol revision.
    */
    *sspr_p_rev_ptr = sspr;
  }

  return status;

} /* prl_validate_base */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_validate

DESCRIPTION
  Validate a specified PRL.

  This function is expected to be called by clients such as OTASP or DIAG
  upon writing a new PRL into NV. The returned validation indication should
  be saved along with the PRL.

  System determination consults the validation indication when reading the
  PRL from NV at power-up or NAM change.

  The function return the PRL version number through the prl_ver_ptr
  parameter.

  The function update the PRL protocol revision through the sspr_p_rev_ptr
  if is_full_validation is set to true.

  The following validations are made:

    - If is_full_validation is true, validate with the specific PRL
      protocol revision first. If fail, try other protocl revisions and
      update protocol revision parameter.
    - If is_full_validation is false, validate only with specified
      PRL protocol revision.

     It is api caller's responsibility to validate that the size bytes 
     present in prl buffer are correct and same as the actual prl size. 
     The first two bytes of the prl indicate the prl size

DEPENDENCIES
  None.

RETURN VALUE
  PRL_VALID if the roaming list is valid. Specific PRL_INVALID_XXX code
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  prl_validation_enum_type  prl_validate(

        byte                      *prl_ptr,
        /* Pointer to the PRL to validate */

        word                      *prl_ver_ptr,
        /* Pointer where to save the PRL version.
        */
        prl_sspr_p_rev_e_type     *sspr_p_rev_ptr,
        /* PRL SSPR_P_REV information
        */
        boolean                   is_full_validation
)
{
  return prl_validate_base( prl_ptr,
                            prl_ver_ptr,
                            sspr_p_rev_ptr,
                            is_full_validation,
                            NULL );
} /* prl_validate */



/* <EJECT> */
/*===========================================================================

FUNCTION prl_commit2

DESCRIPTION
  Validate a specified PRL and commits it to NV if validation is successful.

  Wrapper function around prl_validate() that transfers the PRL locally then
  calls the validation function.

DEPENDENCIES
  None.

RETURN VALUE
  SD error code corresponding to the error state:
  TRUE        - success
  FALSE       - PRL invalid

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  prl_commit_status_e_type prl_commit2 (
        byte                       *prl_to_validate,
            /* PRL to validate */

        dword                       size,
           /*  Size of PRL array in BYTES */

        sd_nam_e_type               curr_nam,
           /* The current NAM to write for */

        word                        *prl_ver_ptr,
            /* Where to save the PRL version.*/

        prl_sspr_p_rev_e_type       *sspr_p_rev_ptr,
            /* PRL protocol revision information. */

        boolean                     is_full_validation

)
/*lint +e715*/
{
  prl_commit_status_e_type    commit_status = PRL_COMMIT_STATUS_DONE;
  prl_validation_enum_type    prl_status;
  boolean                     is_nv_done   = TRUE;

  sdsr_e_type                 hdr_mru_lst  = SDSR_MAX_LST;
  sdsr_e_type                 user_mru_lst = SDSR_MAX_LST;

  dword                       prl_buf_size;
  nv_ram_roaming_list_type    *prl_to_write;
  unsigned int                nv_prl_size  = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Return if the input parameter is invalid.
  */
  if( ( prl_to_validate == NULL) ||
      ( size > NV_ROAMING_LIST_MAX_SIZE ) ||
      ( curr_nam >= SD_NAM_MAX) ||
      ( prl_ver_ptr == NULL ) ||
      ( sspr_p_rev_ptr == NULL ) ||
      !( *sspr_p_rev_ptr > PRL_SSPR_P_REV_INVALID &&
         *sspr_p_rev_ptr < PRL_SSPR_P_REV_RESERVED ) )
  {
    return PRL_COMMIT_STATUS_PRL_INVALID;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Validate the PRL
  */
  prl_status= prl_validate( prl_to_validate,
                            prl_ver_ptr,
                            sspr_p_rev_ptr,
                            is_full_validation);

  SD_MSG_HIGH_1( " Validation status %d", prl_status);

  if(prl_status != PRL_VALID)
  {
    return PRL_COMMIT_STATUS_PRL_INVALID;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* PRL is valid, update the prl variable.
  */

  /* Allocate memory buf of size same as used in nvim.c for writing/reading PRL from NV. */
  nv_prl_size = prl_get_nv_prl_size();

  /* Allocate the memory to write prl to NV. Release the memory after NV write. */
  if((prl_to_write = (nv_ram_roaming_list_type *)modem_mem_alloc(nv_prl_size,
                                                 MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* First 16 bits holds the prl size */
  prl_buf_size = SD_B_UNPACKW( prl_to_validate,
                               0,
                               PR_LIST_SIZE_LENGTH );

  prl_to_write->nam         = (byte) curr_nam;
  prl_to_write->size        = (word)(size * 8);
  prl_to_write->prl_version = *prl_ver_ptr;
  prl_to_write->valid       = TRUE;

  /* Ensure that size of the prl buf is less than the size of the buf allocated for writing to NV */
  if ((prl_buf_size < nv_prl_size) && (prl_buf_size == MIN( prl_buf_size, NV_SIZE_OF_RAM_ROAMING_LIST)))
  {
    memscpy( (byte*)prl_to_write->roaming_list, prl_buf_size, prl_to_validate, prl_buf_size );
  }
  else
  {
    SD_ERR_FATAL_1("Size of the prl buf is more than the size of the buf allocated for writing to NV!, prl buf size = %d", prl_buf_size);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the PRL in NV.
  */
  is_nv_done = sdnv_write(NV_ROAMING_LIST_683_I, (nv_item_type *) prl_to_write);

  modem_mem_free((void *)prl_to_write, MODEM_MEM_CLIENT_MMODE);
  prl_to_write = NULL;

  if (!is_nv_done)
  {
    commit_status = PRL_COMMIT_STATUS_NV_FAIL;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined FEATURE_HDR
  hdr_mru_lst = SDSR_MRU_LST_;
  #endif

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  user_mru_lst = SDSR_GWL_LAST_USER_SYS ;
  #endif

  /* Clear the MRU
  */
  sdsr_list_clr( SDSR_MRU_LST );
  sdsr_list_clr( hdr_mru_lst);
  sdsr_list_clr( user_mru_lst );

  /* Update the MRU of the current NAM
  */
  sdprl_clr_mru();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return commit_status;

} /* prl_commit2 */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_get_total_size

DESCRIPTION
  Get the total size of the PRL in octets.

  If a 2nd PRL is detected at the end of the 1st PRL, the total size is size
  of 1st PRL + size 2nd PRL + 2 for the 3rd CRC.

  If no PRL is detected at the end of the 1st PRL, the total size is the size
  of 1st PRL.

DEPENDENCIES
  None.

RETURN VALUE
  Total size of PRL.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  unsigned int              prl_get_total_size(

        byte                      *prl_ptr
            /* Pointer to PRL for which to get the total size.
            */
)
{
  unsigned int              prl1_size;
  unsigned int              prl2_size         = 0;
  unsigned int              total_size;
  byte                      *prl_ptr_temp;
  prl_validation_enum_type  prl_validation;
  prl_sspr_p_rev_e_type     sspr_p_rev        = PRL_SSPR_P_REV_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( prl_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the PRL size field out of the 1st PRL.
  */
  prl1_size = SD_B_UNPACKW( prl_ptr,
                         0,
                         PR_LIST_SIZE_LENGTH );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Try validating a 2nd PRL that may be stored at the end of the 1st PRL.
  */
  prl_validation = prl_validate_base( (byte*) (prl_ptr + prl1_size),
                                      NULL,
                                      &sspr_p_rev,
                                      TRUE,
                                      NULL );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If validation of the 2nd PRL was successful, extract the size of the 2nd
  ** PRL.
  */
  if( prl_validation == PRL_VALID )
  {
    /* Extract the PRL size field out of the 1st PRL.
    */
    prl_ptr_temp = prl_ptr + prl1_size;
    prl2_size = SD_B_UNPACKW( prl_ptr_temp,
                               0,
                               PR_LIST_SIZE_LENGTH );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compute the total size of PRL. Note that if a 2nd PRL was detected at
  ** the end of the 1st PRL, the total size = size of 1st PRL + size of 2nd
  ** PRL + 2 for the 3rd CRC.
  */
  if( prl2_size > 0 )
  {
    total_size = ( prl1_size + prl2_size + 2 );
  }
  else
  {
    total_size = ( prl1_size );
  }

  SD_MSG_HIGH_3( "PRL size 1st=%d, 2nd=%d, Tot=%d",
                prl1_size, prl2_size, total_size );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the total size of PRL.
  */
  return ( total_size );

} /* prl_get_total_size */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_init

DESCRIPTION
  Initialize the PRL component with a specified NAM.

DEPENDENCIES
  The PRL must have already been loaded into the "prl" buffer.

RETURN VALUE
  TRUE if PRL initialization is successful. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   prl_init(

        byte                      nam,
            /* NAM with which to initialize the PRL component.
            */

        word                      cdma_home_sid,
            /* CDMA home SID.
            */

        word                      cdma_home_nid,
            /* CDMA home NID.
            */

        word                      amps_home_sid
            /* AMPS home SID.
            */
)
{
  int                       i;
  word                      offset;
  byte                      *buf_ptr;

  prl_acq_rec_type          acquisition_type;
  uint8                     num_chans;
  uint8                     num_blocks;

  boolean                   is_prl_read_uccessful;
  boolean                   is_prl_disabled         = FALSE;
  prl_validation_enum_type  prl_validation          = PRL_VALID;
  nv_ram_roaming_list_type  *prl_ptr;
  prl_ram_roaming_list_type *sd_prl_ptr;
  unsigned int              prl_buf_size = 0;
  boolean                   working_on_2nd_prl = FALSE;
  unsigned int              size_of_1st_prl = 0,  size_of_2nd_prl = 0;
  unsigned int              nv_prl_size = 0;

  prl_hdr_s_type            *prl_hdr_ptr            = &prl_hdr;
  prl_hdr_s_type            *prl_hdr_nv_ptr         = &prl_hdr_nv;

  /*lint -save -e826 */ /*Suspicious pointer-to-pointer conversion*/
  sdnv_data_s_type          buf;
  nv_item_type              *item_ptr               = (nv_item_type*) &buf;

  prl_sspr_p_rev_e_type     sspr_p_rev;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( (sd_nam_e_type) nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate memory buf of size same as used in nvim.c for reading PRL from NV. */
  nv_prl_size = prl_get_nv_prl_size();

  if (prl_updated_from_mmgsdi == FALSE)
  {
     /* Allocate the memory to read prl info from NV. Release the memory after NV read.
      */
     if((prl_ptr = (nv_ram_roaming_list_type *)modem_mem_alloc(nv_prl_size,
                                                    MODEM_MEM_CLIENT_MMODE)) == NULL)
     {
       sys_err_fatal_null_ptr_exception();
     }
     else
     {
       SD_MSG_HIGH_2( "Allocated memory for prl object = %x of size = %d", prl_ptr, nv_prl_size);
       memset(prl_ptr, 0, nv_prl_size);
     }
  }
  else
  {
     prl_ptr = (nv_ram_roaming_list_type *) prl_buffer;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the variables that keeps track of the currently loaded PRL
  ** and the PRL that is stored in NV.
  */
  prl_hdr_init( prl_hdr_ptr );
  prl_hdr_init( prl_hdr_nv_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read SSPR_P_REV from nv.
  **
  */
  (void) sdnv_read(NV_SSPR_P_REV_I,item_ptr);
  sspr_p_rev = (prl_sspr_p_rev_e_type)item_ptr->sspr_p_rev;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Try reading the PRL from NV. If read is successful, extract the PRL
  ** header information and check whether PRL is valid.
  */
  is_prl_read_uccessful = prl_read( (sd_nam_e_type) nam, prl_ptr  );

  if( is_prl_read_uccessful )
  {
    /* Check whether a 2nd PRL is concatenated to the end of the 1st PRL
    ** that is stored in prl.roaming_list[].
    */
    SD_MSG_HIGH_0( "=== Check for 2nd PRL ===");

    /* Set the prl_offset to the end of the 1st PRL.
    */
    prl_offset = SD_B_UNPACKW( ((byte *)prl_ptr->roaming_list),
                            0,
                            PR_LIST_SIZE_LENGTH );

    /* make sure that prl_offset is valid */
    if( prl_offset < NV_SIZE_OF_RAM_ROAMING_LIST )
    {
      /* Try validating a 2nd PRL that may be stored at the end of the 1st PRL.
      */
      prl_validation = prl_validate_base(
                                  (byte*) (prl_ptr->roaming_list + prl_offset),
                                  NULL,
                                  &sspr_p_rev,
                                  TRUE,
                                  &prl_hdr_ptr->is_mcc_sys_rec );
    }
    else
    {
      prl_validation = PRL_INVALID_SIZE;
    }

    /* If validation of the 2nd PRL was successful, indicate the MS is
    ** working out of the 2nd PRL.
    */
    if( prl_validation == PRL_VALID )
    {
      working_on_2nd_prl = TRUE;
      SD_MSG_HIGH_0( "=== Working of 2nd PRL ===");
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    else
    {
      /* Indicate the MS is working out of the 1st PRL.
      */
      SD_MSG_HIGH_0( "=== Working of 1st PRL ===");

      prl_offset = 0;
      prl_validation = prl_validate_base( (byte*) prl_ptr->roaming_list,
                                          NULL,
                                          &sspr_p_rev,
                                          TRUE,
                                          &prl_hdr_ptr->is_mcc_sys_rec );

    }

    if( prl_validation == PRL_VALID )
    {
      prl_hdr_extract( prl_hdr_ptr,
                       (byte*) (prl_ptr->roaming_list + prl_offset),
                       sspr_p_rev );

      prl_hdr_ptr->is_valid = TRUE;

      if( (prl_sspr_p_rev_e_type)item_ptr->sspr_p_rev != sspr_p_rev )
      {
        /* Overwrite NV flag*/
        item_ptr->sspr_p_rev = (byte)sspr_p_rev;
        #ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE
        (void)sdnv_write(NV_SSPR_P_REV_I, item_ptr);
        #endif
      }
    }
    else
    {
      SD_MSG_MED_1( "FAILED - Invalid PRL - code=%d", prl_validation);
    }/* if */

  }/* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether PRL is disabled.
  */
  item_ptr->prl_enabled.nam = nam;

  (void) sdnv_read(NV_PRL_ENABLED_I, item_ptr); 
  if(!item_ptr->prl_enabled.enabled )
  {
    is_prl_disabled           = TRUE;
    prl_hdr_ptr->is_disabled  = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Record the information about the PRL that is stored in NV.
  */
  *prl_hdr_nv_ptr = *prl_hdr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If PRL read is not successful, PRL is disabled or PRL is invalid, try
  ** creating a default PRL. If default PRL is not available for this target,
  ** return FALSE.
  */

  if( ! is_prl_read_uccessful ||
      is_prl_disabled ||
      prl_validation != PRL_VALID )
  {
    /*If we did not read the PRL successful, the prl_offset values is
    **no longer valid. So reset back to zero.
    */
    SD_MSG_HIGH_1("Reset prl_offset before create default PRL, offset=%d -> 0",
                 prl_offset);
    prl_offset = 0;

    if( ! prl_create_dflt(cdma_home_sid, cdma_home_nid, amps_home_sid,
                          &sspr_p_rev, prl_ptr) )
    {
      prl_ptr->size             = 0;
      prl_ptr->valid            = FALSE;
      prl_ptr->prl_version      = PRL_DEFAULT_VER;

      /* If no prl found, release the memory allocated dynamically
      */
      SD_MSG_HIGH_1( "No stored or default PRL found,Free memory for prl object = %x", prl_ptr);
      modem_mem_free((void *)prl_ptr, MODEM_MEM_CLIENT_MMODE);
      prl_ptr = NULL;

      return FALSE;
    }
    else
    {
      /* Update protocol revision based on default PRL
      */
      SD_MSG_HIGH_0( "=== Working of default PRL ===");
      prl_hdr_ptr->sspr_p_rev = sspr_p_rev;
      prl_hdr_nv_ptr->sspr_p_rev = sspr_p_rev;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, we have a loaded PRL (from NV or a default) - extract
  ** the header of the loaded a PRL and mark the PRL as valid.
  */
  prl_hdr_extract( prl_hdr_ptr,
                   (byte*) (prl_ptr->roaming_list + prl_offset),
                   sspr_p_rev );

  prl_hdr_ptr->is_valid = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a bit pointer to the first acquisition record
  */
  offset = prl_hdr_ptr->acq_tbl_bit_offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize buf_ptr to the start of the roaming list
  */
  sd_prl_ptr = prl_get_ptr();

  /* save the prl info retrieved from NV
  */
  sd_prl_ptr->nam            = prl_ptr->nam;
  sd_prl_ptr->prl_version    = prl_ptr->prl_version;
  sd_prl_ptr->size           = prl_ptr->size;

  /* get the size of the roaming list retrieved from NV */
  if(working_on_2nd_prl )
  {
    /* First 16 bits holds the 1st prl size */
     size_of_1st_prl = SD_B_UNPACKW( ((byte *)prl_ptr->roaming_list),
                            0,
                            PR_LIST_SIZE_LENGTH );

    /* Ensure that the size of 1st PRL is less than the size of the prl buf read from NV*/
    if (size_of_1st_prl <= NV_ROAMING_LIST_MAX_SIZE)
    {
      /* First 16 bits of the 2nd prl holds the 2nd prl size */
      size_of_2nd_prl = SD_B_UNPACKW( ((byte *)(prl_ptr->roaming_list + size_of_1st_prl)),
                              0,
                              PR_LIST_SIZE_LENGTH );
    }
    else
    {
      SD_ERR_FATAL_1("Size of the first prl is more than the size of the prl buf read from NV!, 1st prl size = %d", size_of_1st_prl);
    }

    prl_buf_size = size_of_1st_prl + size_of_2nd_prl + 1;
  }
  else
  {
    /* First 16 bits holds the prl size */
    prl_buf_size = SD_B_UNPACKW( ((byte *)prl_ptr->roaming_list),
                            0,
                            PR_LIST_SIZE_LENGTH );
  }

  /* Free the old buffer and alloc the buffer again as per the new prl size.
  */
  if(sd_prl_ptr->roaming_list != NULL)
  {
    SD_MSG_HIGH_1( "Free memory for roaming_list object = %x", sd_prl_ptr->roaming_list);
    modem_mem_free((void *)sd_prl_ptr->roaming_list, MODEM_MEM_CLIENT_MMODE);
    sd_prl_ptr->roaming_list = NULL;
  }

  if((sd_prl_ptr->roaming_list = (byte *)modem_mem_alloc(prl_buf_size, MODEM_MEM_CLIENT_MMODE))
                                                                                == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    SD_MSG_HIGH_2( "Allocated memory for new prl's roaming_list object=%x, size = %d",
                                                  sd_prl_ptr->roaming_list, prl_buf_size);
    memset(sd_prl_ptr->roaming_list, 0, prl_buf_size);
  }

  /* Ensure that size of the prl is less than the size of the prl buf read from NV */
  if ((prl_buf_size <= nv_prl_size) && (prl_buf_size == MIN( prl_buf_size, NV_SIZE_OF_RAM_ROAMING_LIST)))
  {
    /* Save the prl info retrieved from NV */
    memscpy( sd_prl_ptr->roaming_list, prl_buf_size, (byte*)prl_ptr->roaming_list, prl_buf_size );
  }
  else
  {
    SD_ERR_FATAL_1("Size of the prl is more than the size of the prl buf read from NV!, prl size = %d", prl_buf_size);
  }

  buf_ptr = (byte*) ( sd_prl_ptr->roaming_list + prl_offset );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Skip over the acq_table.
  */
  if (prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3)
  {
    for( i=0; i < (int) prl_hdr_ptr->num_acq_recs; i++ )
    {
       uint8 rec_len;

      /* Jump over the acquisition record type field.
      */
      offset += ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev);

      /* Extract the acquisition record length field and jump to next record.
      */
      rec_len = SD_B_UNPACKB( buf_ptr,
                           offset,
                           (word)ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) ) ;
      SD_ASSERT( rec_len > 0 );

      offset += (ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) + rec_len*8);
    } /* for */
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  else
  {
    for( i=0; i < (int) prl_hdr_ptr->num_acq_recs; i++ )
    {
      /* Extract the ACQ_TYPE field.
      */
        acquisition_type =
        (prl_acq_rec_type) SD_B_UNPACKB(
                       buf_ptr,
                           offset,
                           (word)ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev));

        offset += ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      switch (acquisition_type)
      {
        case PRL_ACQ_REC_RESERVED :         /* Reserved */
            SD_MSG_LOW_0("Reserved acq_type");
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CELLULAR_ANALOG :  /* Cellular analog */
          offset += sizeof(prl_cellular_analog_type);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CELLULAR_CDMA_STD : /* Cellular CDMA */
          offset += sizeof(prl_cellular_cdma_type);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CELLULAR_CDMA_CUST:/* Cellular CDMA */
          num_chans = SD_B_UNPACKB(buf_ptr, offset,
            FSIZ(prl_pcs_cdma_using_chans_type, num_chans));
          offset += FSIZ(prl_cellular_cdma_custom_type, num_chans);
          offset += num_chans * FSIZ(prl_cellular_cdma_custom_type, chan);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CELLULAR_GENERIC : /* Cellular generic */
          offset += sizeof(prl_cellular_generic_type);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_PCS_CDMA_BLOCKS :  /* PCS CDMA using blocks */
          num_blocks = SD_B_UNPACKB(buf_ptr, offset,
            FSIZ(prl_pcs_cdma_using_blocks_type, num_blocks));
          offset += FSIZ(prl_pcs_cdma_using_blocks_type, num_blocks);
          offset += num_blocks * FSIZ(prl_pcs_cdma_using_blocks_type, block);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_PCS_CDMA_CHANNELS :/* PCS CDMA using channels */
          num_chans = SD_B_UNPACKB(buf_ptr, offset,
            FSIZ(prl_pcs_cdma_using_chans_type, num_chans));
          offset += FSIZ(prl_pcs_cdma_using_chans_type, num_chans);
          offset += num_chans * FSIZ(prl_pcs_cdma_using_chans_type, chan);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* The following three acq record types apply for IS683B PRL or above*/

        case PRL_ACQ_REC_JTACS_CDMA_STD : /* JTACS CDMA standard channels*/
          offset += sizeof(prl_jtacs_cdma_std_type);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_JTACS_CDMA_CUST:/* JTACS CDMA customer channels*/
          num_chans = SD_B_UNPACKB(buf_ptr, offset,
            FSIZ(prl_jtacs_cdma_cust_type, num_chans));
          offset += FSIZ(prl_jtacs_cdma_cust_type, num_chans);
          offset += num_chans * FSIZ(prl_jtacs_cdma_cust_type, chan);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CDMA_2GHZ_CHANS:/* 2G BAND CDMA using channels*/
          num_chans = SD_B_UNPACKB(buf_ptr, offset,
            FSIZ(prl_cdma_2ghz_chans_type, num_chans));
          offset += FSIZ(prl_cdma_2ghz_chans_type, num_chans);
          offset += num_chans * FSIZ(prl_cdma_2ghz_chans_type, chan);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_ACQ_REC_CDMA_GENERIC:
        case PRL_ACQ_REC_HDR_GENERIC:
        case PRL_ACQ_REC_GSM_GENERIC:
        case PRL_ACQ_REC_WCDMA_GENERIC:
        case PRL_ACQ_REC_GW_PREFERRED:
        case PRL_ACQ_REC_GWL_GENERIC:
        case PRL_ACQ_REC_NONE:
        default :
          SD_MSG_LOW_1("Illegal acq_type %d", acquisition_type);
          break;

      } /* switch */

    } /* for */

  } /* if*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3)
  {

    /* Set the common subnet bit offset to point at the beginning of the common
    ** subnet table.
    */
    SD_ASSERT( (offset - (offset/8)*8) == 0 );
    prl_hdr_ptr->subnet_tbl_ptr = buf_ptr + offset/8;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Skip over the common_subnet_table.
    */
    for( i=0; i < prl_hdr_ptr->num_subnet_recs; i++ )
    {
      byte common_length;

      offset += COM_SUBNET_REC_RESERVED_LENGTH;

      common_length = SD_B_UNPACKB( buf_ptr,
                                 offset,
                                 COM_SUBNET_REC_LENGTH_LENGTH );

      offset += COM_SUBNET_REC_LENGTH_LENGTH;

      offset += common_length * 8;
    } /* for */

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the system record bit offset to point at the beginning of the system
  ** record table.
  */
  prl_hdr_ptr->sys_tbl_bit_offset = offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the acquisition record index to zero and the acquisition record
  ** bit index to point to the first acquisition record
  */
  prl_acq_rec_index = 0;
  prl_acq_rec_bit_offset = prl_hdr_ptr->acq_tbl_bit_offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Release the memory allocated dynamically to read the prl info from NV */
  modem_mem_free((void *)prl_ptr, MODEM_MEM_CLIENT_MMODE);
  SD_MSG_HIGH_1( "Free memory for prl object = %x", prl_ptr);
  prl_ptr = NULL;
  prl_buffer = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* prl_init */
/*lint -restore */




/*===========================================================================

FUNCTION PRL_IS_EXTENDED_PRL

DESCRIPTION
  Check if current used PRL is extended PRL.

DEPENDENCIES
  None.

RETURN VALUE
  True if PRL is extended PRL.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   prl_is_extended_prl( void )
{
  return (prl_hdr.sspr_p_rev == PRL_SSPR_P_REV_3);
}





/*===========================================================================

FUNCTION prl_update_crc

DESCRIPTION
  Update the PRL crc.

DEPENDENCIES
  None.

RETURN VALUE
  True if PRL is extended PRL.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static void                       prl_update_crc(

       byte                       *prl_ptr,
         /* Pointer to the beginning of the PRL.
         */
       const prl_hdr_s_type       *prl_hdr_ptr
)
{

  word                            crc_calc = (word) CRC_16_STEP_SEED;

  /*
  ** Compute and check the CRC for the table.
  */
  crc_calc = crc_16_step( crc_calc, (byte *)prl_ptr, prl_hdr_ptr->pr_list_size-(CRC_LENGTH/8) );

  SD_MSG_HIGH_1( "PRL CRC %d", crc_calc);
  /*
  ** Update PRL CRC.
  ** CRC in the PRL points at the bottom of the PRL and it takes 2 bytes.
  ** PRL List Size in the header includes 2 bytes of the CRC.
  ** CRC bytes are not included in CRC calculation.
  */
  b_packw( crc_calc,
           prl_ptr,
           (word)((8*(prl_hdr_ptr->pr_list_size)) - CRC_LENGTH),
           CRC_LENGTH);

  return;
}


/*===========================================================================

FUNCTION prl_update_acq_rec

DESCRIPTION
  Update the PRL acquisition table entry with the specified acqusition record.

DEPENDENCIES
  None.

RETURN VALUE
  True if PRL is extended PRL.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean                    prl_update_acq_rec(

       const prli_acq_rec_type    *acq_rec_ptr,
            /* The input of the acq record pointer to be updated.
            */
       int                        index
            /* The index of the acq table to be updated.
            */
)
{

  /* Initialize prl_hdr_ptr to prl hdr
  */
  prl_hdr_s_type                  *prl_hdr_ptr           = &prl_hdr;

  /* Initialize buf_ptr to the start of the roaming list
  */
  byte           *buf_ptr = (byte*) ( prl.roaming_list + prl_offset );



  int                             i,j;
  int                             acq_rec_index = 0;
  word                            offset = 0;

  prl_acq_rec_type                acq_type;
  byte                            rec_len;
  word                            band_chan;

  sd_mode_e_type                  mode;
  word                            chan;  /* The chan field from an acq_rec */
  sd_band_e_type                  band;  /* The band field from an acq_rec */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The input acq record pointer is NULL.
  */
  if( acq_rec_ptr == NULL )
  {
    SD_ERR_0( " acq_rec_ptr NULL pointer! ");
    return FALSE;
  }

  /* If the index of the acquisition entry is out side the range of PRL,
  ** return false now.
  */
  if( index >= prl_hdr.num_acq_recs ||
      index < 0)
  {
    SD_ERR_2( " Invalid the index of acq table index%d, num_acq_rec %d",
                  index,
                  prl_hdr.num_acq_recs);

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return false, if SSPR P REV is less than 3.
  */
  if( prl_hdr.sspr_p_rev < PRL_SSPR_P_REV_3 )
  {
    SD_ERR_1( " Unable to update SSPR_P_REV %d",
                  prl_hdr.sspr_p_rev);

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize buf_ptr to the start of the roaming list
  */
  offset = prl_hdr_ptr->acq_tbl_bit_offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Search forward until we find the desired record.
  */
  while( acq_rec_index != index )
  {
    /* Jump over the acquisition record type field.
    */
    offset += ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev);

    /* Unpack the acquisition record length field.
    */
    rec_len = SD_B_UNPACKB( buf_ptr, offset,
                     (word)ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) ) ;

    SD_ASSERT( rec_len > 0 );

    /* Jump over the acquisition record length field + the length of the
    ** acquisition record (in bits).
    */
    offset += (ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) + rec_len*8);

    /* Move to the next idex
    */
    acq_rec_index++;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } /* while( acq_rec_index != index ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the acquisition record fields.
  */

  /* Extract the ACQ_TYPE field.
  */
  acq_type = (prl_acq_rec_type) SD_B_UNPACKB( buf_ptr,
                        offset,
                        (word)ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev) );

  offset += ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the acquisition record length field.
  */
  rec_len = SD_B_UNPACKB( buf_ptr,
                   offset,
                   (word)ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) ) ;
  SD_ASSERT( (prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3 ) ? rec_len > 0 : rec_len == 0);
  offset += ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( acq_type != acq_rec_ptr->acq_type )
  {

    SD_ERR_2(" Acq type does not match %d, %d",
                  acq_type,
                  acq_rec_ptr->acq_type);

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the acquisition record type extract the remaining acquisition
  ** record field.
  */
  switch( acq_type )
  {
    case PRL_ACQ_REC_CDMA_GENERIC:
    case PRL_ACQ_REC_HDR_GENERIC:

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( acq_rec_ptr->type.cdma_generic.num_recs == 0 )
      {
        SD_ERR_0(" Num rec is 0");

        return FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Defaul mode to CDMA
      */
      mode = (( acq_type == PRL_ACQ_REC_CDMA_GENERIC ) ? \
             SD_MODE_CDMA : SD_MODE_HDR );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Step through the PRL rec and update the rec with input value.
      ** Bail out if reaching the end of PRL rec or the input rec.
      */
      for( i=0,j=0; (j < acq_rec_ptr->type.cdma_generic.num_recs) && (i < rec_len/2); i++ )
      {
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Get the band/channel in the PRL.
        */
        band_chan = SD_B_UNPACKW( buf_ptr,
                               offset,
                               FSIZ(prl_cdma_generic_type, rec) );

        band = PRLI_CDMA_GENERIC_BAND( band_chan );

        chan = PRLI_CDMA_GENERIC_CHAN( band_chan );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Skip the position of invalid band/channel
        */
        if( sd_map_chan_to_blksys(mode, band, chan) == SD_BLKSYS_MAX )
        {
          SD_MSG_MED_3("Skip the pos of Invalid mode %d band %d chan %d",
                       mode, band, chan);

          offset += sizeof(prl_cdma_generic_type);

          continue;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Pack the record and store to the PRL.
        */
        b_packw( acq_rec_ptr->type.cdma_generic.rec[j],
                 buf_ptr,
                 offset,
                 sizeof(prl_cdma_generic_type) );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Move to the next entry of the input rec.
        */
        j++;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /* Update PRL offset to the next rec.
        */
        offset += sizeof(prl_cdma_generic_type);

      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Not support to change the contents of hte following acq type.
    */
    case PRL_ACQ_REC_RESERVED:
    case PRL_ACQ_REC_CELLULAR_ANALOG:
    case PRL_ACQ_REC_CELLULAR_CDMA_STD:
    case PRL_ACQ_REC_CELLULAR_CDMA_CUST:
    case PRL_ACQ_REC_CELLULAR_GENERIC:
    case PRL_ACQ_REC_PCS_CDMA_BLOCKS:
    case PRL_ACQ_REC_PCS_CDMA_CHANNELS:
    case PRL_ACQ_REC_JTACS_CDMA_STD:
    case PRL_ACQ_REC_JTACS_CDMA_CUST:
    case PRL_ACQ_REC_CDMA_2GHZ_CHANS:
    case PRL_ACQ_REC_GSM_GENERIC:
    case PRL_ACQ_REC_WCDMA_GENERIC:
    case PRL_ACQ_REC_GW_PREFERRED:
    case PRL_ACQ_REC_GWL_GENERIC:
    case PRL_ACQ_REC_NONE:

    default:
      SD_ERR_1( " Unable to update acq type %d",
                  acq_rec_ptr->acq_type);
      return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the CRC
  */
  prl_update_crc( buf_ptr, prl_hdr_ptr );

  return TRUE;
}


/*===========================================================================

FUNCTION prl_commit

DESCRIPTION
  Commit PRL in NV and clear MRU.

DEPENDENCIES
  None.

RETURN VALUE
  True if PRL is extended PRL.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean                    prl_commit(void)
{

  /* Initialize prl_hdr_ptr to prl hdr
  */
  prl_hdr_s_type                  *prl_hdr_ptr = &prl_hdr;

  /* Initialize buf_ptr to the start of the roaming list
  */
  byte           *buf_ptr = (byte*) ( prl.roaming_list + prl_offset );

  prl_commit_status_e_type      commit_status;

  commit_status = prl_commit2 (buf_ptr,
                               prl_hdr_ptr->pr_list_size/8,
                               (sd_nam_e_type)(prl.nam),
                               &(prl_hdr_ptr->pr_list_id),
                               &(prl_hdr_ptr->sspr_p_rev),
                               FALSE
                               );

  if (commit_status != PRL_COMMIT_STATUS_DONE)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }

}

/**===========================================================================

FUNCTION prl_update_from_mmgsdi

DESCRIPTION
  This function is used to copy the received PRL parameters and PRL data
  to the SD prl buffer.


DEPENDENCIES
  sd_init has to be called before this function call

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void   prl_update_from_mmgsdi
(
       const dword prl_size,
       /**< Size of the PRL in bytes
       */
       const word  prl_version,
       /**< version of the PRL
       */
       const byte  *session_read_prl
       /**< Pointer to the PRL buffer read through MMGSDI card
       */
)
{

  nv_ram_roaming_list_type  *prl_ptr = NULL;
  unsigned int nv_prl_size = 0;

  prl_updated_from_mmgsdi = FALSE;

  if (session_read_prl == NULL)
  {
     SD_MSG_HIGH_0( "PRL NOT Loaded from MMGSDI card; session_read_prl is NULL" );
     return;
  }

  if (prl_size)
  {
     /* Allocate memory buf of size same as used in nvim.c for reading PRL.
     */
     nv_prl_size = prl_get_nv_prl_size();

     /* Allocate the memory to read prl info. Release the memory after read. */
     if((prl_buffer = (nv_ram_roaming_list_type *)modem_mem_alloc(nv_prl_size,
                                                 MODEM_MEM_CLIENT_MMODE)) == NULL)
     {
       sys_err_fatal_null_ptr_exception();
     }
     else
     {
       SD_MSG_HIGH_2( "Allocated memory for prl object = %x of size = %d", prl_ptr, nv_prl_size);
       prl_ptr = prl_buffer;
       memset(prl_ptr, 0, nv_prl_size);
     }

     /* If the PRL is valid and size is non zero and
     ** prl buffer pointer is not NULL, copy the prl information and
     ** the received prl buffer to the SD.
     */
     prl_ptr->prl_version = prl_version;
     prl_ptr->size = prl_size;
     prl_ptr->valid = TRUE;

     memscpy( (byte *)(prl_ptr->roaming_list), prl_size, (byte *)session_read_prl, prl_size );

     /* Set the flag to TRUE so that in prl_read, NV read has to be skipped
     */
     prl_updated_from_mmgsdi = TRUE;

  }

  SD_MSG_HIGH_1( "PRL Loaded from MMGSDI card;session_read_prl =%d",session_read_prl );

  return;
} /* prl_update_from_mmgsdi () */



/* <EJECT> */
/*===========================================================================

FUNCTION prl_buf_set_ptr

DESCRIPTION
   Sets the prl buffer pointer to a valid value

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void         prl_buf_set_ptr(
    const byte       * new_prl_buf,
    dword            new_prl_size
)
{

  prl_ram_roaming_list_type  *sd_prl_ptr      = prl_get_ptr();

  /* update size info
  */
  sd_prl_ptr->size           = new_prl_size;

  /* Free the old PRL buffer
  */
  if(sd_prl_ptr->roaming_list)
  {
    modem_mem_free((void *)sd_prl_ptr->roaming_list, MODEM_MEM_CLIENT_MMODE);
    SD_MSG_MED_1( "EqPRL:Free memory for old PRL = %x",sd_prl_ptr->roaming_list);
  }
  /* Allocate memory for the new  PRL
  */
  sd_prl_ptr->roaming_list = (byte *)modem_mem_alloc(new_prl_size,
                                                     MODEM_MEM_CLIENT_MMODE);
  if(sd_prl_ptr->roaming_list == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
  
    SD_MSG_HIGH_2( "EqPRL:Alloc mem for new PRL =%x, size = %d",
                                    sd_prl_ptr->roaming_list,
                                           new_prl_size);
  }

  /* Save the new prl info
  */
  memset(sd_prl_ptr->roaming_list, 0, new_prl_size);
  memscpy(sd_prl_ptr->roaming_list, new_prl_size, new_prl_buf, new_prl_size );

  /* Set the offset to 0
  */
  prl_offset  = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the acquisition record index to zero and the acquisition record
  ** bit index to point to the first acquisition record
  */
  prl_acq_rec_index = 0;
  prl_acq_rec_bit_offset = prl_hdr_get_ptr()->acq_tbl_bit_offset;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}//prl_buf_set_ptr

/*===========================================================================

FUNCTION prl_decode_tot_tbl()

DESCRIPTION
  Decode the TOT table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the decoding is successful
  or
  FALSE if the decoding is failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean prl_decode_tot_tbl(

  byte                      *buf_ptr,
    /* byte array that has to be decoded
    */
  sd_rat_acq_order_s_type   *tot
    /* TOT table
    */

)
{
  uint16                       offset         = 0;
  uint16                       size           = 0;
  uint16                       i              = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT( buf_ptr != NULL);
  SD_ASSERT( tot     != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract Version field
  */
  size = sizeof(tot->version)*8;
  tot->version  = SD_B_UNPACKW(buf_ptr,offset,size);
  offset += size;

  /* Extract num rats
  */
  size = sizeof(tot->num_rat)*8;
  tot->num_rat  = SD_B_UNPACKW(buf_ptr,offset,size);
  offset += size;

  if(tot->num_rat > SYS_MAX_ACQ_PREF_RAT_LIST_NUM ||
   tot->num_rat == 0)
  {
  
    SD_MSG_HIGH_1("TOT: invld num_rat %d", tot->num_rat);
    return FALSE;
  }



  /* Extract acq sys mode
  */
  size = 8;
  for(i = 0; i < tot->num_rat; i++)
  {
    /* KW: Preceeding check tot->num_rat > SYS_MAX_ACQ_PREF_RAT_LIST_NUM, keeps i
    ** within acq_sys_mode bounds
    */
    tot->acq_sys_mode[i] = (sys_sys_mode_e_type)SD_B_UNPACKW(buf_ptr,
                                                             offset,
                                                             size);

    /* If sys mode is not a valid cellular system sys mode (C, H, G, W or L)
    ** return false
    */
    if( tot->acq_sys_mode[i] != SYS_SYS_MODE_CDMA  &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_HDR   &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_GSM   &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_WCDMA &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_LTE   &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_TDS
      )
    {
      return FALSE;
    }
    offset += size;
  }
  return TRUE;

}//prl_decode_tot_tbl

/*===========================================================================

FUNCTION prl_encode_tot_tbl()

DESCRIPTION
  Encode the TOT table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the encoding is successful
  or
  FALSE if the encoding is failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean prl_encode_tot_tbl(

  const sd_rat_acq_order_s_type   *tot,
    /* TOT table to be encoded.
    */

  byte                      *buf_ptr
    /* byte array that stores the encoded TOT list.
    */
)
{
  uint16                       offset         = 0;
  uint16                       size           = 0;
  uint16                       i              = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* verify against null value of tot pointer and buffer pointer.
  */
  if(tot == NULL)
  {
    SD_ERR_0("tot is NULL");
    return FALSE;
  }

  if(buf_ptr == NULL)
  {
    SD_ERR_0("buf_ptr is NULL");
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Encode Version field
  */
  size = sizeof(tot->version)*8;
  SD_B_PACKW(tot->version,buf_ptr,offset,size);
  offset += size;

  /* Validate number of RATs
  */
  if(tot->num_rat > SYS_MAX_ACQ_PREF_RAT_LIST_NUM ||
   tot->num_rat == 0)
  {
    SD_ERR_1("TOT encoding: invld num_rat %d", tot->num_rat);
    return FALSE;
  }

  /* Encode num rats
  */
  size = sizeof(tot->num_rat)*8;
  SD_B_PACKW(tot->num_rat,buf_ptr,offset,size);
  offset += size;


  SD_MSG_HIGH_2("TOT encoding: ver %d num_rat %d", tot->version, tot->num_rat );

  /* Encode acq sys mode
  */
  size = 8;
  for(i = 0; i < tot->num_rat; i++)
  {
    /* KW: Preceeding check tot->num_rat > SYS_MAX_ACQ_PREF_RAT_LIST_NUM, keeps i
    ** within acq_sys_mode bounds
    */
    /* If sys mode is not a valid cellular system sys mode (C, H, G, W or L)
    ** return false
    */
    if( tot->acq_sys_mode[i] != SYS_SYS_MODE_CDMA  &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_HDR   &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_GSM   &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_WCDMA &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_LTE   &&
        tot->acq_sys_mode[i] != SYS_SYS_MODE_TDS   
      )
    {
      return FALSE;
    }

    SD_B_PACKB((uint8)(tot->acq_sys_mode[i]),buf_ptr,offset,size);
    SD_MSG_HIGH_1("TOT encoding: rat %d", tot->acq_sys_mode[i]);

    offset += size;
  }

  return TRUE;

}//prl_encode_tot_tbl

