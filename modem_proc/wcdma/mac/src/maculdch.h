#ifndef MACULDCH_H
#define MACULDCH_H
/*===============================================================================================

							U L    D C H    H A N D L E R
                
GENERAL DESCRIPTION
  
  This module contains the definition of the data types and functions 
  declared in "maculdchcfg.c".
  
Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

===============================================================================================*/

/*==============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/maculdch.h_v   1.1   09 May 2001 18:21:20   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/maculdch.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------
06/02/11   kc      Added support for logical channel to transport channel mapping info
05/07/09   ssg     Updated Copyright Information
04/04/01   ps	   Initial release after changing "maculdchcfg.h" to "maculdch.h"

===============================================================================================*/

/*==============================================================================================

FUNCTION NAME	proc_ul_dch_cctrch_cfg 

DESCRIPTION		This function processes UL DCH CCTrCh Configuration Data

=================================================================================================*/
extern		void	proc_ul_dch_cctrch_cfg 
(
	/* Pointer to ordered config data of UL DCH type CCTrCh */ 
	mac_ul_dch_cctrch_config_type	*ded_cctrch_ptr
);

/*==============================================================================================
FUNCTION NAME	build_rach_tblks

DESCRIPTION		This function builds RACH Transport Blocks
===============================================================================================*/
extern	void	build_dch_tblks 
(
 	/* Pointer to Build Frame Indication Parameters */
	mac_ul_build_frame_ind_type		*bld_frame_ind_ptr
);

/*===============================================================================================
FUNCTION NAME mac_ul_check_dch_trch

DESCRIPTION   This function checks if the input rlc id and rb id pair map to a UL-DCH transport channel
===============================================================================================*/

log_wcdma_trch_type mac_ul_check_dch_trch
(
   /* RLC logical channel buffer identifier (0-20) */
   rlc_lc_id_type rlc_id, 

   /* Radio Bearer Identifier (0-31) */
   rb_id_type rb_id
);

#endif

