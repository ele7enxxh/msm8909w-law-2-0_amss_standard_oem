#ifndef GMM_INT_H
#define GMM_INT_H
/*===========================================================================

              GPRS MOBILITY MANAGEMENT (GMM) INTERNAL HEADER FILE

DESCRIPTION
  This file data and function declarations necessary for the GMM sub-system
  of the UMTS/GSM MM task.

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmm_int.h_v   1.13   09 Jul 2002 13:21:12   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/gmm_int.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/01   jca     Created file
05/11/01   jca     Added new data types and functions.
05/23/01   jca     Changed some types to be compatible with OTA message format.
06/08/01   jca     Added gmm_power_down_active regional variable.
08/03/01   jca     Added new data types.
09/07/01   jca     Corrected numeric values for GPRS Detach Type definitions.
11/06/01   jca     Added new variable for PS signalling connection.
06/05/02   ks      Added support for Dual mode,Consolidated gmm PLMN,LAC & RAC
                   into Serving_plmn type. Removed gmm_handle_activation.
07/06/02   vdr     Added new external definition in order to support GMM-LLC interface.
07/09/02   vdr     Fixed LINT error.
10/09/02   jca     Fixed some bugs related to RR(C)_SERVICE_IND processing.
11/06/02   jca     Added function gmm_update_mm_serving_plmn_info().
02/27/03   vdr     Added prototypes in order to support UMTS to GSM RAT changes
02/27/03   mks     Added function prototypes gmm_sm_send_rat_change_req() &
                   gmm_sm_send_rat_resume_req for supporting PS data continuity
                   feature.
03/07/03   vdr     Added function gmm_process_cm_req() and some external definitions
                   required by PS data continuity feature
03/31/03   jca     Externalized function gmm_initiate_gprs_detach() and
                   added boolean variable gmm_gprs_detach_pending.
04/14/03   jca     Added boolean variable gmm_ra_update_pending.
04/29/03   jca     Added gmm_previous_network_op_mode variable.
05/29/03   jca     Added boolean variable gmm_follow_on_request.
06/10/03   ks      Added support for default values for GPRS NV Items.
06/19/03   ks      Passing the RAT info in the gmm_sm_send_rat_change_req ().
                   Made the GPRS NV items as external.
07/09/03   vdr     Removed prototype of the gmm_ptmsi_allocated_rai variable
07/16/03   jca     Added boolean variable gmm_gprs_attach_pending.
08/13/03   jca     Added boolean gmm_detach_after_rau_pending.
08/28/03   ks      Modified the GPRS_NON_DRX_TIMER value from 0x3 to 0x0
10/22/03   jca     Added function prototype gmm_change_of_network_mode_of_operation().
12/03/03   vdr     Added prototype for gmm_ptmsi_sig_tobe_deleted
12/11/03   jca     Added regional variables gmm_previous_serving_rai
                   and gmm_previous_gmm_substate.
02/20/04   vdr     Added processing required by READY timer negotiation procedure
04/08/04   vdr     Added prototype for gmm_inter_rat_follow_on_ind
05/12/04   jca     Added boolean variable gmm_directed_signalling_reestablishment.
06/18/04   jca     Added variable gmm_pending_ra_update_type.
11/02/04   sv      Added default values for EDGE features supported.
01/05/05   ks      Added new definition for GPRS_EDGE_FEATURE_NOT_SUPPORTED
01/07/05   sv      Changed the default value of EDGE_MULTISLOT_CLASS from '1' to '8'.
02/15/05   sv      Enabled 8PSK uplink modulation - changed the default value.
02/17/05   sv      Added support for Rel-4.
03/21/05   sv      Modified the EDGE Multislot class of the phone to '10'.
06/17/05   jca     Added variable gmm_previous_gprs_service_state.
09/27/05   abe     Modified GPRS_EDGE_NW_ASS_CELL_CHANGE to 0x01
11/11/05   ss      Added gmm_service_request_data_accepted for Rel5.
10/06/05   ajt     Declaration for mm_lcs_domain and its type
12/15/05   abe     Supported GSM DTM under FEATURE_GSM_DTM
02/02/06   up      Adding GPRS_INTERRAT_NACC_SUPPORT NV item for RR.
02/24/06   abe     Adding support for FEATURE_REL_6_SUPPORT
03/02/06   up      Default value of INTERRAT NACC is set to 0x03.
05/19/06   npr     Default value of GPRS_EDGE_DTM_MULTISLOT_SUBCLASS set to 0x3 (Class 11)
06/14/06   up      Modified prototype gmm_update_mm_serving_plmn_info.
07/14/06   pk     Updated for Feature DSAC
11/27/06   vsm   added the prototype for the function gmm_update_ra_type.
02/26/08   rk     removed gmm_inter_rat_follow_on_ind as its not used.
10/10/11   HC      Moved GPRS_MULTISLOT_CLASS and GPRS_EDGE_MULTISLOT_CLASS from apis(gmm_int.h) 
                   to inc(gmm_int_v.h) folder 
10/31/11   mrk     reverted above "GPRS_MULTISLOT_CLASS and GPRS_EDGE_MULTISLOT_CLASS" changes 
03/13/12   hc     reverted CL 2011080, moved GPRS_MULTISLOT_CLASS and GPRS_EDGE_MULTISLOT_CLASS 
                    definitions to gmm_int_v.h 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "gmm.h"
#include "mmtask.h"
#include "mmtask_v.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* -------------------------------------
** GPRS NV PARAMETER DEFAULT VALUES
** ------------------------------------- */

/* Default values for EDGE NV items */
#define GPRS_EDGE_FEATURE_NOT_SUPPORTED   0x0  /* EDGE support is OFF, irrespective of NV */
#define GPRS_EDGE_FEATURE_SUPPORT_DEFAULT 0x1  /* EDGE is defaulted to OFF, but NV would turn it ON */

#define GPRS_EDGE_8PSK_POWER_CLASS        0x1  /* 8PSK uplink modulation is turned ON by default */
#define GPRS_EDGE_8PSK_POWER_CAPABILITY   0x2  /* If 8PSK uplink modulation is supported, power class is '2' */


#define GPRS_EDGE_DTM_SUPPORTED           0x1  /* DTM Feature support is enabled by default */
#define GPRS_EDGE_DTM_MULTISLOT_SUBCLASS  0x3  /* Code for GPRS/EDGE DTM multislot class of 11 */



#define GPRS_EDGE_EDTM_SUPPORTED           0x1  /* EDTM Feature support is enabled by default */



#define GPRS_EDGE_EDA_SUPPORTED           0x1  /* MSC12/EDA Feature support is enabled by default */



#define GPRS_EDGE_NW_ASS_CELL_CHANGE      0x1  /* N/W ass cell change is ON */
#define GPRS_INTERRAT_NACC_SUPPORT        0x03  /* Inter RAT NACC default value */


/* TS 24.008 10.5.5.6 */
#define GPRS_NON_DRX_TIMER            0x0  /* Code for 0 seconds non-DRX mode after xfer state */
#define GPRS_SPLIT_PAGING_CYCLE       0xA  /* Code for Split Paging cycle of 10 */
#define GPRS_GEA_SUPPORT              0x7  /* By default GEA1 & GEA2 support is enabled */

#endif
