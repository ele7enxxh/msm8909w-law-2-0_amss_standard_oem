#ifndef DLBTFD_H
#define DLBTFD_H

/*============================================================================
                                  D E C . H
DESCRIPTION

Copyright (c) 2001 - 2003 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dlbtfd.h_v   1.5   28 Feb 2002 17:36:32   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlbtfd.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
01/28/12    vs      Nikel feature cleanup.
11/12/03    m       Power control improvements for BTFD calls (Jun+Subra)
02/24/03    gs      Changed the prototype of function
                    dl_dob_status_fifo_info_btfd_update to return boolean.
02/28/02    gs      1. Externalized variable dl_btfd_failed_decision
11/15/01    gs      1. Externalized the function dl_btfd_abort_cur_btfd_accum
09/17/01    gs      1. Externalized the function dl_query_guided_trch.
08/14/01    gs      1. Corrected some function names.
07/17/01    gs      1. Added the declaration for the function
                       dl_query_btfd_all_tf_non_zero_crc_type.
04/26/01    gs      1. Created this file
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "l1rrcif.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* This variable maintains the failed BTFD decision for
   in-sync and out-of sync update. These variables should be accessed
   in INTLOCKS */
extern uint16 dl_btfd_failed_decision;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION dl_btfd_init

DESCRIPTION
  This function initializes the BTFD related variables. This should be
  called along with other DL module initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_btfd_init(void);

/*===========================================================================
FUNCTION dl_init_cctrch_setup_btfd

DESCRIPTION
  This function reseve the BTFD info and initialize the info stat for this
  CCTrCh table index.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE as per the CCTrCh init for BTFD.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_init_cctrch_setup_btfd(
  /* CCTrCh table index of the CcTrCh */
  uint8 cctrch_table_index,
  /* CCTrCh info structure pointer */
  l1_dl_cctrch_info_struct_type *cctrch_info);

/*===========================================================================
FUNCTION dl_reset_btfd_stat

DESCRIPTION
  This function reset the BTFD TB decopde done stats for the given CCTrCh
  table index.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE as per the CCTrCh init for BTFD.

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_reset_btfd_stat(
  /* CCTrCh table index of the CcTrCh */
  uint8 cctrch_table_index);

/*===========================================================================
FUNCTION dl_find_guiding_trch

DESCRIPTION
  This function finds the guiding TrCh for the TrCh in which CRC information
  is not present to assist the BTFD

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE as per the guiding TrCh search results for all the TrCh that
  need guided detection.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_find_guiding_trch(
  /* CCTrCh table index of the CCTrCh */
  uint8 cctrch_table_index,
  /* CCTrCh setup information from RRC. */
  l1_dl_cctrch_info_struct_type   *cctrch_info,
  /* CTFC information from RRC */
  l1_dl_ctfc_info_struct_type     *ctfc_info);

/*===========================================================================
FUNCTION dl_query_cctrcht_btfd_type

DESCRIPTION
  This function returns the state of the CCTrCh for its usage of BTFD.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on CCTrCh BTFD usage.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_query_cctrcht_btfd_type(
  /* CcTrCh table index of the CCTrCh */
  uint8 cctrch_table_index);

/*===========================================================================
FUNCTION dl_query_btfd_all_tf_non_zero_crc_type

DESCRIPTION
  This function returns the state of the CCTrCh for its usage of BTFD and
  having all the TB in all TF to have non zero CRC.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on CCTrCh BTFD usage.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_query_btfd_all_tf_non_zero_crc_type(
  /* CcTrCh table index of the CCTrCh */
  uint8 cctrch_table_index,
  /* TrCh index of the CCTrCh */
  uint8 trch_index);

/*===========================================================================
FUNCTION dl_query_guided_trch

DESCRIPTION
  This function returns if the TrCh index in the CCTrCh need guiding or
	not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on CCTrCh BTFD usage.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_query_guided_trch(
  /* CcTrCh table index of the CCTrCh */
  uint8 cctrch_table_index,
  /* TrCh index of the CCTrCh */
  uint8 trch_index);


/*===========================================================================
FUNCTION dl_query_btfd_trch_only_tb_w_crc_pc

DESCRIPTION
  This function returns if the TrCh index needs special power control
  if it has only one TB with CRC attached out of total 2 TBs
  not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_query_btfd_trch_only_1_tb_w_crc_pc(
  /* CcTrCh table index of the CCTrCh */
  uint8 cctrch_table_index,
  /* TrCh index of the CCTrCh */
  uint8 trch_index);


/*===========================================================================
FUNCTION dl_reset_cctrcht_btfd_info_idx

DESCRIPTION
  This function reset the BTFD info index. This is used as indicator of
  wheather this cctrch_table_index need BTFD or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_reset_cctrcht_btfd_info_idx(
  /* CcTrCh table index of the CCTrCh */
  uint8 cctrch_table_index);

/*===========================================================================
FUNCTION dl_dob_status_fifo_info_btfd_update

DESCRIPTION
  This function updates the decoded TrCh result in case the CCTrCh need the
  BTFD. It saves the pointer to of mDSP status FIFO for this CCTrCh.
  It then check if all the decoding result available have been made or not.
  If all the TBsets have been decoded. It initiates the BTFD decision
  algorithm.
  In case all the decoding results have not been made available or
  The BTFD decision and subsequent processing of DOB has not yet been done
  and another decoding result has arrived that has been previously updated
  it discards the whole previous decoding results and frees the DOB status FIFOs.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on add to BTFD info success.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dl_dob_status_fifo_info_btfd_update(
  /* DOB status FIFO index */
  uint8 dob_status_fifo_idx);

/*===========================================================================
FUNCTION dl_btfd_abort_cur_btfd_accum

DESCRIPTION
  This function aborts the current accumulation of BTFD info. This function
  can be called at the cleanup or when any error has occured while collecting
  the BTFD decisions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All the BTFD decision collected for all TrCh and TF are thrown away and
  mDSP status FIFO are freed.
===========================================================================*/

extern void dl_btfd_abort_cur_btfd_accum(
  /* CcTrCh table index of the CCTrCh */
  uint8 cctrch_table_index);

/*===========================================================================
FUNCTION dl_btfd_process

DESCRIPTION
  This function performs the BTFD decision for a CCTrCh. Once all the TF for
  all TrChs of a CCTrCh have been decoded, this function should be called.
  It follows the following algorithm.
  
  1. If only 1 TF in a TrCh is with CRC passed on ALL the TB, declare that as
     valid TF.
  2. If more than 1 TF in a TrCh is with CRC passed on ALL the TB, declare 
     erasure on that TrCh. Nothing is sent to MAC in this case.
  3. If the guiding TrCh for a guided TrCh can't decode a valid TF then declare
     erasure for the guided TrCh too.
     
  Once the valid TF for all TrCh have been detected then the mDSP DOB status
  FIFO state is set to deliverable. The next TB HDR decode/Data read event will
  get the data out of DOB and send then TB to MAC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_btfd_process(
  /* CCTrCh table index of the CCTrCh */
  uint8 cctrch_table_index);

#endif /* DLBTFD_H */
