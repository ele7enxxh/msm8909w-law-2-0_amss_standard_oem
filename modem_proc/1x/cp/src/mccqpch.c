/*===========================================================================

               Q U I C K   P A G I N G   C H A N N E L

GENERAL DESCRIPTION
  This module provide call processing related code to support operation of
  the quick paging channel.

  Copyright (c) 2000 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccqpch.c_v   1.6   25 Sep 2002 19:51:50   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccqpch.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/13   ppr     1X_CP related logging changes
10/09/13   pap     Removing warnings for LLVM.
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
05/09/12   ppr     Feature Cleanup
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features, 
                   Remove Always Off Features
12/16/11   ppr     Fixed MS to operate in IS2000 timelines if P-REV in 
                   Sync or ESPM is greater than or equal to P_REV_IS95B= 4
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
10/20/10   azh     Fixed KW warnings.
08/26/09   adw     Removed deprecated ts.h header and unecessary time_cdma.h.
                   Added the necessary qw.h header.
05/03/09   jj      CMI Phase-II update.
04/17/09   jtm     Added includes to fix compiler warnings.
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
05/14/07   pg      Do not let srch do QPCH after a failed reacq.
05/19/06   fc      Mainlined Rel A searcher interface.
04/26/06   an      Fixed Lint Errors
09/15/05   fc      Fixed lint errors.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
05/08/03   fc      Added debug message for QPCH hashing.
04/14/03   va      Corrected references to ovhd_chan to to be consistent
                   with new data struct
03/21/03   fc      Added support for PCH BI.
10/31/02   va      Added support for dynamic p_rev changes.
09/24/02   fc      Added BI walsh code.
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
04/10/02   yll     Added Broadcast SMS support on IS2000 RelA common channel.
02/22/02   fc      Added support for Release A common channels.
06/14/00   ks      Sending the pi_power_level & cci_power_level in the raw
                   format, to facilitate the SRCH with its lookup table.
06/06/00   ks      Fixed the band_class initialization of pc_id data.
06/05/00   ks      Added checking for reception of either Sync Channel Msg or
                   ESPM message to determine the PREV capability in
                   mccqpch_get_pch_framing_type function.
05/25/00   ks      Fixed the pch_id initialization in mccqpch_get_qpch_info()
                   Fixed the Hashing functions for PI1 & PI2 bit computation.
04/11/00   lcc     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "caii_v.h"
#include "mc_v.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccqpch_i.h"
#include "mccbcsms.h"
#include "ran.h"
#include "mccap_v.h"
#include "msg.h"
#include "m1x_diag.h"
#include "qw.h"

/*===========================================================================

                 PROTOTYPES FOR SOME FUNCTIONS USED LOCALLY

===========================================================================*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
// Walsh code used by quick paging channel.  Defined in the Standard
// (IS2000.2 section 3.1.3.6.6)
static const uint8 qpch_walsh[] =
{
  0,                    // Filler, entry not used
  QPCH_CHAN_1_WALSH,
  QPCH_CHAN_2_WALSH,
  QPCH_CHAN_3_WALSH
};

// Value N for both rates used in hash function for computing the PI positions.
static const uint16 qpch_hash_n[] = { 188, 376 };

/*===========================================================================

                DECLARATIONS FOR LOCAL FUNCTIONS

This section contains declarations for functions used locally.

===========================================================================*/
static uint16 mccqpch_hash_2nd_pi
(
  uint16    R1,
  uint16    Nx,
  qword     slot_sys_time
);

/*===========================================================================
FUNCTION mccqpch_get_qpch_info

DESCRIPTION
  Gets the quick paging channel information from the specified pilot.  The
  pilot should be a known pilot to the mobile (i.e. stored in bs_info).  The
  QPCH is assumed to be in the same band class.  sys_time specified when the
  information will be used.  If it is NULL (pointer), the information will not
  be checked for expiration.

DEPENDENCIES
  None.

RETURN VALUE
  UNKNOWN_PILOT       - Pilot not found in bs_info.
  QPCH_NOT_SUPPORTED  - QPCH is not support for the specified pilot
  QPCH_INFO_INVALID   - The stored QPCH information for the pilot is not valid
                        (expired or not yet received)
  QPCH_INFO_VALID     - Requested information is returned in the function
                        payload.

SIDE EFFECTS
  None.
===========================================================================*/

mccqpch_status_type mccqpch_get_qpch_info
(
  uint16                  cdma_channel,
  uint16                  pilot_pn,
  qword                   sys_time,     // The time the bs info will be used
  mccqpch_qpch_info_type  *qpch_info
)
{

  cdma_bs_type* bs;
  bs_info_id_type bs_info_id;

  // If we're in the process of retrying sleep ala OOS phase 1, don't try
  // QPCH.
  if (mccidl_retrying_sleep())
  {
    return QPCH_INFO_INVALID;
  }

  // Prepare bcch or pch ID
  bs_info_id.ovhd_chan = cur_bs_ptr->ovhd_chan_info.ovhd_chan;
  bs_info_id.cdma_ch = cdma_channel;
  bs_info_id.pilot_pn = pilot_pn;

  // We don't expect different broadcast control channel or paging channel to
  // contain different QPCH info, so use current broadcast control channel or
  // paging channel.
  bs_info_id.ovhd_walsh_ch = cdma.pagech;

  // It's unthinkable to get QPCH info for a different band class.  At lease
  // this is what this function is designed to do.
  bs_info_id.band_class = cdma.band_class;

  if (mcc_get_bs_info(&bs_info_id, sys_time, &bs))
  {
    if (((bs->ovhd_chan_info.ovhd_chan == CAIX_PC) && (!bs->rx.esp_rx ||
         (bs->csp.esp.config_msg_seq != bs->rx.config_msg_seq)))
       )
    {
      // ESPM or MCRRM has not been received yet
      return QPCH_INFO_INVALID;
    }

    // If message belongs to P_REV older than 6 or QPCH is not supported by BS
    else if ((OVHD_CHAN_IS_PCH && (bs->csp.esp.p_rev < P_REV_IS2000)) ||
             !bs->csp.esp.qpch_supported ||
             !mccap_item_is_supported(MCCAP_DF_QPCH) /*DYNAMIC_P_REV */
            )
    {
      return QPCH_NOT_SUPPORTED;
    }

    else
    {
      // If we are here, QPCH is supported so generate the return information.
      qpch_info->qpch_rate = (mccqpch_rate_type)bs->csp.esp.qpch_rate;

      qpch_info->hash_basis = (mccqpch_hash_basis_type)( ( bs->csp.esp.imsi_t_supported != FALSE ) ?
                                                           IMSI_T_HASHED : IMSI_M_HASHED );
      // According to the Standard (table 3.7.2.3.2.13-3), value from message minus
      // is dB value.
      qpch_info->pi_power_level = (int8)bs->csp.esp.qpch_power_level_page;

      if ( (qpch_info->cci_supported = bs->csp.esp.qpch_cci_supported) != FALSE )
      {
        // According to the Standard (table 3.7.2.3.2.13-3), value from message minus
        // is dB value.
        qpch_info->cci.power_level = (int8)bs->csp.esp.qpch_power_level_config;
        qpch_info->cci.walsh_code = QPCH_CCI_WALSH;
      }

      if ( bs->qpch_num == QPCH_INVALID_CH )
      {
        // QPCH channel number has not been hashed yet, do it now
        // Note: we need to generate our own hash key (not the one in cdma structure)
        // because it may be different for the other base station.
        bs->qpch_num = (mccqpch_channel_type)( hash( bs->imsi_hash_key,
                       bs->csp.esp.num_qpch,
                       (word)(2*(bs->imsi_hash_key & 0xfff)) ) + 1 );
      }

      qpch_info->pi_walsh_code = qpch_walsh[(uint8)bs->qpch_num];

      M1X_MSG( DCP, LEGACY_LOW,
        "QPCH: Tot %d, Hash %d PI walsh %d",
        bs->csp.esp.num_qpch,
        bs->qpch_num,
        qpch_info->pi_walsh_code);

      qpch_info->bi_supported = FALSE;

      return QPCH_INFO_VALID;
    }
  }
  else
  {
    // Requested message does not exist or is too old.  Return proper status.
    return QPCH_INFO_INVALID;
  }

} // mccqpch_get_qpch_info

/*===========================================================================
FUNCTION mccqpch_get_qpch_pi_pos

DESCRIPTION
  Gets the paging indicator positions for the specified slot of the active
  pilot.  Note that the PI positions for both QPCH rates are returned just in
  case SRCH performs an idle handoff to a known pilot which uses a different
  QPCH rate after calling this function (saves one call to this function).

DEPENDENCIES
  Input parameter pi_pos must be array larger enough (2 members) to hold the
  PI positions.

RETURN VALUE
  QPCH_NOT_SUPPORTED  - QPCH is not support for the specified pilot
  QPCH_INFO_INVALID   - The stored QPCH information for the pilot is not valid
                        and no PI positions are generated.
  QPCH_INFO_VALID     - PI positions are returned in the function payload.

SIDE EFFECTS
  None.
===========================================================================*/

mccqpch_status_type mccqpch_get_qpch_pi_pos
(
  qword               slot_sys_time,
  mccqpch_pi_pos_type pi_pos[]
)
{

  qword msg_age = { 0, 0};  // Temporary storage for age of message in current bs_info

  if ((OVHD_CHAN_IS_PCH && ESP_MSG_NOT_RECVD)
     )
  {
    // ESPM or MCRRM has not been received yet, don't know how to compute the
    // PI positions nor do we know if QPCH is supported
    return QPCH_INFO_INVALID;
  }
  else
  {
    // Make sure that we are not getting a bogus time such that slot_sys_time is
    // even before the message time
    if ( qw_cmp( slot_sys_time, cur_bs_ptr->rx.msg_time ) == -1 )
    {
      return QPCH_INFO_INVALID;
    }

    // Now make sure that the stored message has not expired
    // Note: CAI_T31M is in milliseconds
    qw_sub( msg_age, slot_sys_time, cur_bs_ptr->rx.msg_time );
    if ((qw_hi( msg_age ) > 0) ||
        (qw_lo( msg_age ) > CAI_T31M/20))
    {
      // Overhead msg older than 10 minutes
      return QPCH_INFO_INVALID;
    }

    // If message belongs to P_REV older than 6, QPCH is not supported
    else if (OVHD_CHAN_IS_PCH &&
             ((cur_bs_ptr->csp.esp.p_rev < P_REV_IS2000) ||
              !cur_bs_ptr->csp.esp.qpch_supported))
    {
      return QPCH_NOT_SUPPORTED;
    }
  }

  // If we ever get here, the we have valid information to compute the PI bit positions.
  // Do it now.

  // Hash the PI bit position for 9600.
  pi_pos[QPCH_9600].pi1_pos =  hash( cdma.imsi_hash_key, qpch_hash_n[QPCH_9600],
                               (word)((slot_sys_time[0] >> 6) & 0xffff) );

  pi_pos[QPCH_4800].pi1_pos =  pi_pos[QPCH_9600].pi1_pos/2;

  // Now for the second PI positions
  pi_pos[QPCH_9600].pi2_pos = mccqpch_hash_2nd_pi( pi_pos[QPCH_9600].pi1_pos,
                                                   (qpch_hash_n[QPCH_9600]+8)/2,
                                                   slot_sys_time );

  pi_pos[QPCH_4800].pi2_pos = pi_pos[QPCH_9600].pi2_pos/2;

  return QPCH_INFO_VALID;

} // mccqpch_get_qpch_pi_pos

/*===========================================================================
FUNCTION mccqpch_hash_2nd_pi

DESCRIPTION
  Computes the position of the 2nd PI based on the hash function specified in
  the Standard (IS2000.5 2.6.7.1).  After simplication, the formula looks like:

    hash(hash_key, Nx - 4K, DECORR2) + 2*Nx + K*Nx

    where
    Nx = 96 for QPCH rate = 4800    (N+4 on Standard)
    Nx = 192 for QPCH rate = 9600   (N+8 on Standard)
    K = floor(R1/Nx), either 1 or 0
    R1 = bit position of 1st PI (as in Standard)
    hash is the hash function defined in ran.c

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/

static uint16 mccqpch_hash_2nd_pi
(
  uint16    R1,
  uint16    Nx,
  qword     slot_sys_time
)
{

  // Same as K in description
  uint8 which_half = R1/Nx;   //lint !e734  Loss of precision okay.
  uint16 Kx;

  MCC_CHK_NULL_PTR(slot_sys_time);

  Kx = (Nx - 96) ? Nx - 8 : Nx - 4;

  return hash( cdma.imsi_hash_key, which_half ? Kx : Nx,
               (word)(((slot_sys_time[0] >> 6) + 1) & 0xffff) ) +
               2*Nx + (which_half ? Nx : 0);  //lint !e734

} /*lint !e818 */

/*===========================================================================
FUNCTION mccqpch_get_pch_framing_type

DESCRIPTION
  Gets the forward common control channel or paging channel framing type of
  the specified pilot.  This helps searcher to determine where to wake up
  before the assigned slot.

DEPENDENCIES
  None.

RETURN VALUE
  UNKNOWN_FRAMING     - Paging channel framing of specified pilot is unknown.
                        (because pilot is unknown).
  PRE_IS95B_FRAMING   - IS95A or JSTD008 framing.
  IS95B_FRAMING       - For IS95B and beyond, the frame right before the slot
                        that begins a synchronized capsule is guaranteed to
                        tail off with 0's.  Searcher can take advantage of
                        this to reduce wakeup time.

SIDE EFFECTS
  None.
===========================================================================*/

mccqpch_pch_framing_type mccqpch_get_pch_framing_type
(
  uint16  cdma_channel,
  uint16  pilot_pn
)
{
  bs_info_id_type  bs_info_id;  // Used to specify bcch or pch of bs info
  cdma_bs_type*    bs;          // Pointer to BS info.

  // Prepare bcch or pch ID
  bs_info_id.ovhd_chan = cur_bs_ptr->ovhd_chan_info.ovhd_chan;
  bs_info_id.cdma_ch = cdma_channel;
  bs_info_id.pilot_pn = pilot_pn;

  // We don't expect different broadcast control channel or paging channel to
  // contain different QPCH info, so use current overhead channel.
  bs_info_id.ovhd_walsh_ch = cdma.pagech;

  // This function only gets the info for the current band class.
  bs_info_id.band_class = cdma.band_class;

  // Obtain a pointer to the bs_info that contains the info for this pilot
  if (mcc_get_bs_info(&bs_info_id, 0, &bs))
  {
    if (!bs->rx.sy_rx && (bs->ovhd_chan_info.ovhd_chan == CAIX_PC) && ESP_MSG_NOT_RECVD)
    {
      // This shouldn't happen, but if MS is currently monitoring Paging Channel,
      // sync channel message & ESPM message are not received for this pilot
      // yet, then just declare unknown framing.

      return PCH_UNKNOWN_FRAMING;
    }

    // (1) If MS is currently monitoring BCCH/FCCCH,
    //                   OR
    // (2) If MS is currently monitoring Paging Channel and received Sync
    //     channel message or ESP message,
    //
    // return proper Framing Information.

    if ( ( (bs->rx.sy_rx) && (bs->sy.p_rev >= P_REV_IS95B ) ) ||
       ( (bs->ovhd_chan_info.ovhd_chan == CAIX_PC) && ESP_MSG_IS_RECVD &&
           (bs->csp.esp.p_rev >= P_REV_IS95B) ) )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Debug IS95B sy PREV %d, esp PREV %d",
        bs->sy.p_rev,
        bs->csp.esp.p_rev);
      return PCH_IS95B_FRAMING;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Debug PRE IS95B sy PREV %d, esp PREV %d",
        bs->sy.p_rev,
        bs->csp.esp.p_rev);
      return PCH_PRE_IS95B_FRAMING;
    }
  }
  else
  {
    // BS info does not exist, we don't know the framing type
    return PCH_UNKNOWN_FRAMING;
  }
} // mccqpch_get_pch_framing_type


