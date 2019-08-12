#ifndef MACULDATA_H
#define MACULDATA_H
/*========================================================================

          U P L I N K   M A C  D A T A   H E A D E R   F I L E 
                
GENERAL DESCRIPTION
  
  This module contains the declaration of the functions and data types
  defined in "maculdata.c"
  
Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*========*/

/*=========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/maculdata.h_v   1.6   09 May 2001 18:17:18   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/maculdata.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/09/13   as      Code change to EDRX full sleep if there pending partials at MAC. 
11/08/13   as      Added code change not to disable RACH if there is any pending data in DL 
                   that needs to be received which in turn would not trigger EDRX full sleep.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/05/12   mrg     HS-RACH & MAC-I Feature code integrated
01/02/12   grk     Feature cleanup
01/09/12   sk      MAC changes to support RxD in FACH
12/21/11   mrg     R99 profiling support added in MAC
10/22/11   sk      NikeL UL CM workaround under feature FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
05/07/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
10/17/08   grk     Merged taxis changes to mainline.
==============================================================================================*/

/* MACRO for UL LC Buffer Status Pointer */
extern		rlc_ul_buf_status_type		ul_buf_status;   
#ifdef __GNUC__
#define		GET_UL_LC_BUF_STATUS_PTR(rlc_index)		&ul_buf_status.chan_info[rlc_index]
#else /* __GNUC__*/
#define		GET_UL_LC_BUF_STATUS_PTR(rlc_index)		&ul_buf_status.chan_info[##rlc_index##]
#endif /* __GNUC__ */

extern boolean mac_rach_rxd_enable;


/* Profiling samples structure to capture the snapshot of the times stamps*/

#define MAC_NUM_PROFILE_SAMPLES 60

typedef struct
{
int32 frame_qty_start;
int32 cx8_qty_start;
int32 frame_qty_end;
int32 cx8_qty_end;
}l2_profile_data_struct_type;


/*==============================================================================================
 FUNCTION NAME	:	rlc_buffers_are_not_empty

 DESCRIPTION	:	This function checks whether RLC Logical Channel Buffers are empty or not.
					Returns TRUE if the buffers are not empty. FALSE otherwise
===============================================================================================*/
boolean   rlc_buffers_are_not_empty
(
  rlc_ul_lc_info_type       *mac_ul_lc_info_ptr,
  boolean                   *rlc_wait_for_ack   
);


/*==============================================================================================
 FUNCTION NAME		build_ul_frame

 DESCRIPTION		This function builds an UL frame

===============================================================================================*/
extern  void    build_ul_frame 
(
	/* Pointer to Build Frame Indication Parameters */
	mac_ul_build_frame_ind_type		*bld_frame_ind_ptr
);

/*==============================================================================================
 FUNCTION NAME	build_ul_tblks

 DESCRIPTION	This function builds UL Transport Blocks
===============================================================================================*/
extern	boolean		build_ul_tblks 
(
 	/* Pointer to Build Frame Indication Parameters */
	mac_ul_build_frame_ind_type		*bld_frame_ind_ptr
);

/*===============================================================================================
 FUNCTION NAME  : mac_ignore_ul_srbs

 DESCRIPTION  : This function clears the buffer status of UL Signal Radio Bearers
===============================================================================================*/
void    mac_ignore_ul_srbs
(
/* Uplink Buffer Status Pointer */
rlc_ul_buf_status_type        *rlc_ul_buf_status
);

/*===============================================================================================
 FUNCTION NAME  attach_mac_parameters

 DESCRIPTION  Attaches MAC parameters to each UL RLC PDU
================================================================================================*/
void  attach_mac_parameters
(
/* RLC Frame Data Pointer */
rlc_ul_frame_data_type  *rlc_frame_data_ptr,

/* Shared Transmit Data Buffer Pointer */
l1_ul_tx_data_type    *txbufptr
);

/*===============================================================================================
 FUNCTION NAME  mac_compute_pdus_from_rlc_status

 DESCRIPTION  Compute the nbytes and npdus needed per LC based on RLC BO calculations
================================================================================================*/
void mac_compute_pdus_from_rlc_status(uint8  nlchan, rlc_lc_id_type   *rlc_id_ptr,
                                       rlc_ul_buf_status_type    *ul_buf_status_ptr, boolean *data_present_on_srb_ptr);

#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*==============================================================================================
 FUNCTION NAME  mac_rlc_check_pending_dl_data

 FUNCTION   This function if there is any data pending to be received or processed in DL.
===============================================================================================*/
extern boolean mac_rlc_check_pending_dl_data(void);
#endif

#endif

