#ifndef MACPCH_H
#define MACPCH_H
/*=====================================================================

     P C H    C O N F I G U R A T I O N    H E A D E R   F I L E
                
GENERAL DESCRIPTION
  
  This module contains the declaration of the functions and data types
  defined in "macpchcfg.c"
  
Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=====================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macpch.h_v   1.1   09 May 2001 18:15:44   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macpch.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------
05/07/09   ssg     Updated Copyright Information
05/02/01	ps	   Changed few functions to MACROs
04/04/01	ps	   Initial release after merging "macpchcfg.c" and "macpchhdr.c"

=====================================================================*/

/*==============================================================================================

FUNCTION NAME   process_pch_config

DESCRIPTION		This function processes PCH Configuration Data received from RRC

==============================================================================================*/
extern	void	process_pch_config 
(
	/* Number of PCHs */
	uint8   num_pchs,
	
	/* Pointer to PCH ordered config data */
	mac_dl_pch_cctrch_config_type	*pch_info
);

/*==============================================================================================

FUNCTION		process_pch_tbs_hdr

DESCRIPTION		This function processes PCH TBS Header information

CONTEXT			Layer 1 Interrupt context 

==============================================================================================*/
void	process_pch_tbs_hdr 
(
	/* First PCH TB ID */
	uint8	tb_id,

	/* Pointer to internal PCH info */
	pch_config_info_type			*int_pch_ptr,
	
	/* Pointer to PCH TBS Header Info */
	l1_dl_tb_set_hdr_struct_type	*tbs_hdr_ptr
);          

#endif

