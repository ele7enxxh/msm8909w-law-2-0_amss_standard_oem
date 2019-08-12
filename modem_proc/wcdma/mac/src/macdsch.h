#ifndef MACDSCH_H
#define MACDSCH_H
/*=====================================================================

     D S C H    C O N F I G U R A T I O N    H E A D E R   F I L E
                
GENERAL DESCRIPTION
  
  This module contains the declaration of the functions and data types
  defined in "macdschcfg.c"
  
Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=====================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macdsch.h_v   1.2   09 May 2001 18:15:02   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macdsch.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------
06/02/11   kc      Added support for logical channel to transport channel mapping info
05/07/09   ssg     Updated Copyright Information
04/29/09    av     Fixed compiler warnings in L2 
04/04/01	ps	   Initial release after merging "macdschcfg.c" and "macdschhdr.c"

=====================================================================*/

/*===========================================================================================
FUNCTION NAME   process_dsch_config

DESCRIPTION
  This function processes the downlink dedicated transport channel 
  configuration received from RRC. 
  
CONTEXT:
  Downlink Layer 2 Task Context                                    
===========================================================================================*/
extern	void	process_dsch_config 
(
	/* Number of DSCHs */
	uint8	num_dschs,

	/* Pointer to DSCH Config Data received from RRC */
	mac_dl_dsch_trch_config_type   **dsch_trch_list
	
);

/*==========================================================================================

FUNCTION NAME	process_dsch_tbs_hdr

DESCRIPTION		This function processes header information of DSCH transport block set
                                                                  
==========================================================================================*/
extern	void	process_dsch_tbs_hdr 
(
	/* First DSCH TB ID */
	uint8	tb_id,

	/* Pointer to internal DSCH config info table */
	dsch_config_info_type			*int_dsch_ptr,

	/* Pointer to DSCH TBS Header Information */	
	l1_dl_tb_set_hdr_struct_type	*tbs_hdr_ptr
);

/*==================================================================================================
FUNCTION NAME   process_dcch_dtch_dsch_config

DESCRIPTION   This function processes config info of DCCH/DTCH maaped to a DSCH
==================================================================================================*/
void  process_dcch_dtch_dsch_config
  (
/* Ordered DSCH config data pointer */
  mac_dl_dsch_trch_config_type  *oc_dsch_ptr,

/* Internal DSCH Config data pointer */
  dsch_config_info_type     *int_dsch_ptr
  );

/*===============================================================================================
FUNCTION NAME mac_dl_check_dsch_trch

DESCRIPTION   This function checks if given rlc-id and rb-id pair map to a DL-DCH transport channel
===============================================================================================*/
log_wcdma_trch_type mac_dl_check_dsch_trch
(
	/* RLC logical channel buffer identifier (0-20) */
	rlc_lc_id_type rlc_id,

	/* Radio Bearer Identifier (0-31) */
	rb_id_type rb_id
);

#endif

