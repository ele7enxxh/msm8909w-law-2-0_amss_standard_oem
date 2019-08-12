#ifndef MACDLDCH_H
#define MACDLDCH_H
    
/*=====================================================================

    D L   D C H    C O N F I G U R A T I O N    H E A D E R   F I L E
                
GENERAL DESCRIPTION
  
  This module contains the declaration of the functions and data types
  defined in "macdldchcfg.c"
  
Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*===*====*====*====*====*=*/

/*===========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macdldch.h_v   1.1   09 May 2001 18:13:40   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macdldch.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------
06/02/11   kc      Added support for logical channel to transport channel mapping info
05/07/09   ssg     Updated Copyright Information
04/29/09    av     Fixed compiler warnings in L2 
04/04/01	ps	   Initial release after merging "macdldchcfg.c" and "macdldchhdr.c"

===========================================================================================*/
                                                     
/*============================================================================================
FUNCTION NAME   process_dl_dch_config

DESCRIPTION		This function processes DL DCH configuration data
  
============================================================================================*/
extern		void	process_dl_dch_config 
(
	/* Number of DCHs */
	uint8  num_dchs,

	/* Pointer to ordered DCH configuration data list */
	mac_dl_dch_trch_config_type	*oc_dch_ptr_list[]
);

/*==============================================================================================
FUNCTION		process_dl_dch_tbs_hdr

DESCRIPTION		This function processes the header information of DCH transport block set
  
==============================================================================================*/
extern	void	process_dl_dch_tbs_hdr 
(
	/* transport Block ID */
	uint8	tb_id,

	/* Pointer to internal DCH config info */
	dl_dch_config_info_type		*int_dch_ptr,

	/* Pointer to the header info of Transport Block Set */
	l1_dl_tb_set_hdr_struct_type	*tbs_hdr_ptr
);

/*==============================================================================================
FUNCTION NAME   process_dl_dcch_dtch_dch_config

DESCRIPTION   This function processes config info of DCCH/DTCH that is maaped to DL DCH
==============================================================================================*/
void  process_dl_dcch_dtch_dch_config
(
/* Ordered DCH config data pointer */
mac_dl_dch_trch_config_type   *oc_dch_ptr,

/* Internal Dl DCH Config data pointer */
dl_dch_config_info_type     *int_dch_ptr
);

/*===============================================================================================
FUNCTION NAME mac_dl_check_dch_trch

DESCRIPTION   This function checks if given rlc-id and rb-id pair map to a DL-DCH transport channel
===============================================================================================*/
log_wcdma_trch_type mac_dl_check_dch_trch
(
	/* RLC logical channel buffer identifier (0-20) */
	rlc_lc_id_type rlc_id, 

	/* Radio Bearer Identifier (0-31) */
	rb_id_type rb_id
);

#endif

