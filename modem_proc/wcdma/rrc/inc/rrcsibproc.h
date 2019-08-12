#ifndef RRCSIBPROC_H
#define RRCSIBPROC_H
/*===========================================================================
                        R R C   S I B   P R O C E S S I N G  

DESCRIPTION

  This is the external header file for the SIB processing module.
  This module is to be used by other RRC modules only and not by
  tasks other than RRC.
  
Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcsibproc.h_v   1.8   31 May 2002 18:17:18   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcsibproc.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/15   vi      Made changes to add DMCR IE only when DMCR is supported in SIB3
10/09/14   sn      QTA during SIB read changes
07/21/14   sn      f3 reduction changes
05/12/14   sn      Changes to control SIB sleep before receiving SBs using NV
04/10/14   sg      Made changes to correct the feature flag used for rel 10 spec CRs
02/21/14   bc      Made changes to intiliaze previous scanned freq and psc during BPLMN search 
                   at the end of BPLMN search
01/10/14   gv      Increased the transition buffer gap from 4 frames to 10 frames to account for WL1 overhead
01/03/14   bc      Made Changes to read SIBs across DRX cycles.
12/2/13    bc      Changes to To avoid reading SB1 during Update SIBs event if any of the deferred SIBs are 
                   scheduled in SB1 and if SB1 is already present in SIB DB
09/16/13   sn      G2W TA feature changes
11/18/13   bc      Checked in REL 10 spec crs changes
08/20/13   sr      Made changes to not add DMCR IE in uplink message when only SIB18/19 is deferred
01/03/13   sr      Made changes to remove the ASN functions used for encoding of SIBs and modified the
                   processing of CSFB system information container accordingly as part of memrory optimization
12/07/12   sn      CSG feature changes
10/08/12   sn      Changes to decode SIB7 only once
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/06/12   ad      Changes for HS-RACH feature
05/02/12   sn      Changes to build and send SIB schedule change indication after 
                   receiving SIB7 in FACH state
12/20/11   db      Modified the prototype to support reading SIB7 for every 8s in FACH state
04/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.52
04/01/11   su      Added code to trigger OOS for DL_WK_IND in update_sibs
04/01/11   su      Added prototype for rrcsib_is_event_update_sibs_active
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/13/10   ss      Added code to invalidate value tag of SIBS for CSFB
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/26/10   kp      Modified code for 3GPP CSFB
10/11/10   rm      Added code to support 3GPP CSFB
10/11/10   ss      Upgraded SIB11Bis code to ASN 8.a and added code to init sibs lookup table 
                   after initialization of rrc_nv_rel_indicator.
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
08/17/09   dm      Made changes for initiating cell selection if PLMN id and Cell id
                   of the current camped cell and target cell match.
07/21/09   dm      Added support for updating UL Interference value with the latest
                   received SIB7 content during UPDATE SIBS event.
06/30/09   dm      Added support for updating UL Interference value with the latest
                   received SIB7 content.
05/14/09   kp      Added support to not drop S-BCCH during FACH to PCH transition only if 
                   event active is UPDATE_SIB’s in all other cases if S-BCCH is up during 
                   the transition then Drop.
05/08/09   ss      Updated Copyright Information
04/20/09   dm      Fixed compilation warnings
02/02/09   kp      Added support for optimized SIB heap under feature flag 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP
12/08/08   ns      Added support for FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
03/10/08   kp      Added support for SIB11bis. Code changes are under feature flag 
                   FEATURE.SIB11BIS.SUPPORT 
03/07/08   vr\da   Support for SIB reading in DCH for MBMS
01/28/08   vr      Changed feature definition of Inter SIB Sleep from FEATURE_INTER_SIB_SLEEP
                   to (defined(FEATURE_INTER_SIB_SLEEP) || defined(FEATURE_WCDMA_DRX_MANAGER))
                   since the new DL code in L1 uses FEATURE_WCDMA_DRX_MANAGER instead of
                   FEATURE_INTER_SIB_SLEEP
08/08/07   kp      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
02/20/07   kp      Added MIB_POSITION & MAX_TIME_MIB_WAIT, which control the support
                   to bar the cell if MIB not found at position SFN mod 32 = 0, 
                   but a transport block with correct CRC was found at that position.
01/19/07   kp      Added support for SIB7 update in Cell FACH state.
                   The code is under feature flag FEATURE_UPDATE_SIB7_IN_FACH.
                   With this feature ON SIB-7 which doesn't have value tag will be read     
                   all the time in cell FACH state. The S-BCCH will be kept open for the 
                   duration UE stays in cell FACH state.
09/25/06   kp      Added support for six hours sib re-read & sib_change_ind interface change
02/07/06   vr      Changed feature name from FEATURE_SIB_SLEEP_OPTIMIZATION 
                   to FEATURE_INTER_SIB_SLEEP
12/21/05   vr      Support for FEATURE_SIB_SLEEP_OPTIMIZATION.Added a new function
                   rrcsib_get_sib_sleep_schedule_table_location() to return the 
		   location of the schedule table for CPHY_SETUP_REQ for N_BCCH
11/21/05   vr      Added a command store to buffer Update SIBS command while 
                   BPLMN is in progress
11/08/05   vr      Support for WTOW BPLMN feature.Also added a new function
                   to query is SIB is active or not.
05/27/05   kc      Added interface for to indicate if SIB-7 Update is ongoing or not
12/01/04   kc      Added interface for a utility fn. to check and see if MIB is read for a 
                   particular event.
12/01/04   kc      Added enum for force_event_init for neighbor cell.
08/10/04   kc      Added enum to handle Channel-Config-Req causes in a central function
06/07/04   svk     Added enum rrcsib_event_abort_cause_e_type. Added protoype for
                   fnction RRCSIB_FORCE_SIB_EVENT_INIT which will initialize the 
                   sib variables asscoaietd with the active event when an OOS area
                   is detected or an ABORT_REQ is recvd by RCE when the SIB is int he
                   process of updating SIB7.
05/15/03   ram     Added function prototypes to init sib procedure and events which
                   can be used to initialize the procedure in cases other than
                   powerup init (such as FTM or LPM).
05/09/02   ram     Added function prototype rrcsib_check_and_update_sib7_
                   validity which is used to check if SIB7 is valid before
                   accessing the RACH.
02/05/02   ram     Added function prototype rrcsib_change_serving_cell_status
                   which is used only by cell selection procedure. This is used
                   to indicate to the sib procedure that a new cell has been
                   designated as the serving cell in case of a succesful cell
                   selection or reselection. It also indicates if the current
                   serving cell is acceptable in case of a failed cell
                   reselection.
01/26/01   ram     Updated comments.
01/25/01   ram     Updated copyright. Corrected comment at the end of the file.
12/07/00   ram     Updated module to follow the new rrc command structure. 
                   Moved the internal command definitions to rrcintif.h. Added
                   more comments.
11/29/00   ram     Updated to use command queues instead of events.
10/31/00   ram     Added externalized function rrc_sib_proc_init to initialize
                   the SIB procedure.
10/26/00   ram     Added additional structures required for sending a confirmation
                   event after completing (or failing) reception of SIBs for a
                   registered event.
10/23/00   ram     Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "rrccmd_v.h"
#include "rrcsibproci.h"
#include "seq.h"

extern boolean sib_sleep_after_sb1_sb2;

#include "rrccspdb.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define SIB4_PRESENT_BIT_MASK 0x40                                                          
#define SIB11_PRESENT_BIT_MASK 0x20
#define SIB6_PRESENT_BIT_MASK 0x10

#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
 #define MIB_POSITION 8
 #define MAX_TIME_MIB_WAIT 4
#endif

#ifdef FEATURE_DUAL_SIM
/* Invert mask after RRCSIB_MASK_INVERT_FACTOR 
   times maximum rep rate */
#define RRCSIB_MASK_INVERT_FACTOR 10

/* Transition buffer added to invert mask timer */
#define RRCSIB_TRANS_BUFFER 100

/* Empty mask duration between switching masks*/
#define RRCSIB_EMPTY_FRAMES 100
#endif

#define CSG_IND_IN_MIB(msg_ptr)\
        ((msg_ptr->m.v690NonCriticalExtensionsPresent)&& \
         (msg_ptr->v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent)&& \
         (msg_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent)&&\
         (msg_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.v860NonCriticalExtensions.\
          masterInformationBlock_v860ext.m.csg_IndicatorPresent)\
        )

#define CSG_ID_IN_SIB3(msg_ptr)\
        ((msg_ptr->m.v4b0NonCriticalExtensionsPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent)&& \
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.m.v670NonCriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.m.\
          v770NonCriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.\
          v770NonCriticalExtension.m.v830NonCriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.\
          v770NonCriticalExtension.v830NonCriticalExtension.m.v860NonCriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.\
          v770NonCriticalExtension.v830NonCriticalExtension.v860NonCriticalExtension.sysInfoType3_v860ext.m.csgIdentityPresent))

#define RES_FOR_CSG_IN_SIB3(msg_ptr)\
        ((msg_ptr->m.v4b0NonCriticalExtensionsPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent)&& \
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.m.v670NonCriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.m.\
          v770NonCriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.\
          v770NonCriticalExtension.m.v830NonCriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.\
          v770NonCriticalExtension.v830NonCriticalExtension.m.v860NonCriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.\
          v770NonCriticalExtension.v830NonCriticalExtension.v860NonCriticalExtension.m.v870NonCriticalExtensionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.\
          v770NonCriticalExtension.v830NonCriticalExtension.v860NonCriticalExtension.v870NonCriticalExtension.\
          sysInfoType3_v870ext.m.cellAccessRestrictionPresent)&&\
         (msg_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.\
          v770NonCriticalExtension.v830NonCriticalExtension.v860NonCriticalExtension.v870NonCriticalExtension.\
          sysInfoType3_v870ext.cellAccessRestriction.m.cellReservedForCSGPresent))
#ifdef  __GNUC__


#define EXTENSION_SIB_SCHED_PRESENT_IN_MIB(msg_ptr)\
		((msg_ptr->m.v690NonCriticalExtensionsPresent)&& \
		 (msg_ptr->v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent)&& \
		 (msg_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.\
			masterInformationBlock_v6b0ext.m.extSIBTypeInfoSchedulingInfo_ListPresent)\
		)


#define EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(msg_ptr, msg_type_prefix1,msg_type_prefix2)\
        ((msg_ptr->m.v6b0NonCriticalExtensionsPresent)&&\
         (msg_ptr->v6b0NonCriticalExtensions.msg_type_prefix2##_v6b0ext.m.extSIBTypeInfoSchedulingInfo_ListPresent)\
        )


#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING

#define DEFERRED_MEAS_SUPPORT(sib3_ptr)\
   ((sib3_ptr->m.v4b0NonCriticalExtensionsPresent)&&\
      (sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent)&&\
       (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)&&\
          (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.m.v670NonCriticalExtensionPresent)&&\
            (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.m.v770NonCriticalExtensionPresent)&&\
              (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.m.deferredMeasurementControlReadingSupportPresent))


#define INTRA_FREQ_MEAS_QUAN_FOR_DEFERRED_SIB(sib3_ptr)\
   ((sib3_ptr->m.v4b0NonCriticalExtensionsPresent)&&\
      (sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent)&&\
       (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)&&\
          (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.m.v670NonCriticalExtensionPresent)&&\
            (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.m.v770NonCriticalExtensionPresent)&&\
              (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.m.deferredMeasurementControlReadingSupportPresent)&&\
                (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.deferredMeasurementControlReadingSupport.m.modeSpecificInfoPresent)&&\
                  (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.deferredMeasurementControlReadingSupport.modeSpecificInfo.t == T_rrc_DeferredMeasurementControlReadingSupport_modeSpecificInfo_fdd))

#endif

#ifdef FEATURE_WCDMA_TO_LTE
#define EXTENSION_SIB_SCHED_2_PRESENT_IN_MIB(msg_ptr)\
  ((msg_ptr->m.v690NonCriticalExtensionsPresent)&&\
    (msg_ptr->v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent)&&\
      (msg_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent)&&\
        (msg_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.v860NonCriticalExtensions.masterInformationBlock_v860ext.m.extSIBTypeInfoSchedulingInfo_ListPresent)\
  )


#define EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(msg_ptr, msg_type_prefix1,msg_type_prefix2)\
  ((msg_ptr->m.v6b0NonCriticalExtensionsPresent)&&\
    (msg_ptr->v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent)&&\
      (msg_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.msg_type_prefix2##_v860ext.m.extSIBTypeInfoSchedulingInfo_ListPresent)\
  )

#endif

#else /* __GNUC__*/


#define EXTENSION_SIB_SCHED_PRESENT_IN_MIB(msg_ptr)\
		((msg_ptr->m.v690NonCriticalExtensionsPresent)&& \
		 (msg_ptr->v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent)&& \
		 (msg_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.\
			masterInformationBlock_v6b0ext.m.extSIBTypeInfoSchedulingInfo_ListPresent)\
		)


#define EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(msg_ptr, msg_type_prefix1,msg_type_prefix2)\
        ((msg_ptr->m.v6b0NonCriticalExtensionsPresent)&&\
         (msg_ptr->v6b0NonCriticalExtensions.##msg_type_prefix2##_v6b0ext.m.extSIBTypeInfoSchedulingInfo_ListPresent)\
        )
        

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING

#define DEFERRED_MEAS_SUPPORT(sib3_ptr)\
   ((sib3_ptr->m.v4b0NonCriticalExtensionsPresent)&&\
      (sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent)&&\
       (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)&&\
          (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.m.v670NonCriticalExtensionPresent)&&\
            (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.m.v770NonCriticalExtensionPresent)&&\
              (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.m.deferredMeasurementControlReadingSupportPresent))


#define INTRA_FREQ_MEAS_QUAN_FOR_DEFERRED_SIB(sib3_ptr)\
   ((sib3_ptr->m.v4b0NonCriticalExtensionsPresent)&&\
      (sib3_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionPresent)&&\
       (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.m.v5c0NoncriticalExtensionPresent)&&\
          (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.m.v670NonCriticalExtensionPresent)&&\
            (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.m.v770NonCriticalExtensionPresent)&&\
              (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.m.deferredMeasurementControlReadingSupportPresent)&&\
                (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.deferredMeasurementControlReadingSupport.m.modeSpecificInfoPresent)&&\
                  (sib3_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtension.v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.sysInfoType3_v770ext.deferredMeasurementControlReadingSupport.modeSpecificInfo.t == T_rrc_DeferredMeasurementControlReadingSupport_modeSpecificInfo_fdd))

#endif

#ifdef FEATURE_WCDMA_TO_LTE
#define EXTENSION_SIB_SCHED_2_PRESENT_IN_MIB(msg_ptr)\
  ((msg_ptr->m.v690NonCriticalExtensionsPresent)&&\
    (msg_ptr->v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent)&&\
      (msg_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent)&&\
        (msg_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.v860NonCriticalExtensions.masterInformationBlock_v860ext.m.extSIBTypeInfoSchedulingInfo_ListPresent)\
  )


#define EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(msg_ptr, msg_type_prefix1,msg_type_prefix2)\
  ((msg_ptr->m.v6b0NonCriticalExtensionsPresent)&&\
    (msg_ptr->v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent)&&\
      (msg_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.msg_type_prefix2##_v860ext.m.extSIBTypeInfoSchedulingInfo_ListPresent)\
  )

#endif


#endif /* __GNUC__*/
#ifdef FEATURE_3GPP_CSFB
#define DEFAULT_SIB_MASK  (MIB_BIT|SB_1_BIT|SB_2_BIT|SIB_1_BIT|SIB_2_BIT|SIB_3_BIT|SIB_4_BIT|SIB_5_BIT|SIB_6_BIT|SIB_7_BIT|SIB_11_BIT|SIB_12_BIT|SIB_18_BIT)
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef enum
{
  RRCSIB_ABORT_SIB7_UPDATE,
  RRCSIB_ABORT_DUE_TO_OOS,
  RRCSIB_ABORT_DUE_TO_NGH_OOS,
  RRCSIB_ABORT_DUE_TO_WTOW_BPLMN,
  RRCSIB_ABORT_DUE_SIB_READ_IN_DCH,                  /*This will abort sib reading & will teardown S-PCCPCH*/
  RRCSIB_ABORT_DUE_SIB_READ_IN_DCH_NO_PCCPCH_DROP, /*This will abort sib reading & will not teardown S-PCCPCH*/
  RRCSIB_ABORT_DUE_TO_DL_WK_IND
}rrcsib_event_abort_cause_e_type;

//setup cause for channel_config_req
typedef enum
{
  SBCCH_SETUP,
  SBCCH_TEARDOWN,
  SBCCH_NOOP,
  NBCCH_SETUP,
  NBCCH_TEARDOWN,
  NBCCH_NOOP
} rrcsib_bcch_action_type;

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
typedef enum
{
  DEFERRED_SIB11,
  DEFERRED_SIB11_BIS,
  DEFERRED_SIB12,
  DEFERRED_ANY_ALLOWED_SIB,
  DEFERRED_UPDATE_SIB
}rrcsib_deferred_sib_type;
#endif

typedef struct
{
  rrc_MasterInformationBlock *mib;
  rrc_SysInfoType1           *sib1;
  rrc_SysInfoType3           *sib3;
  rrc_freq_type              prev_freq;
  rrc_scr_code_type          prev_scr;
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
  rrc_SysInfoTypeSB1        *sb1;
  rrc_SysInfoTypeSB2        *sb2;
#endif
}rrcsib_bplmn_sibs_type;

/* Command store for buffered processing */
typedef struct {
  boolean pending_flag;
  rrc_cmd_type saved_cmd;
} rrcsib_cmd_store_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* Number of SFNs to skip between SIB7 reading in FACH state. 800SFNs in 8s*/
#define RRC_SKIP_SFN 800
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

extern boolean rrcrce_is_rce_procedure_active(void);

/*===========================================================================

FUNCTION RRC_SIB_EVENT_CONTROLLER

DESCRIPTION

  This is the main function that controls the SIB processing module.
  The SIB processing module is event based, i.e it waits for events
  to be received from other RRC modules and does the required processing
  for each event. This function receives events from the RRC dispatcher
  and processes the events as needed. 

DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  BCCH logical channels may be set up and teared down. 
===========================================================================*/
void rrc_sib_event_controller
(
  rrc_cmd_type *cmd_ptr  /* Pointer to command */
);

/*===========================================================================

FUNCTION rrcsib_update_sched_info_from_mib_sb

DESCRIPTION
  Updates scheduling information based on MIB/SB1/SB2 received OTA

DEPENDENCIES
 
  None.

RETURN VALUE
 
  None.

SIDE EFFECTS
  Updates the function to update scheduling information, which updates the
  sib_events database based on scheduling information received.        
===========================================================================*/
void rrcsib_update_sched_info_from_mib_sb
(
  uint32 pdu_num,    /* PDU number defined by ASN.1 for this SIB */
  void *sib_ptr,    /* Pointer to the SIB typecast to void.     */
  sib_events_index_e_type  event_index
);


/*===========================================================================

FUNCTION RRC_SIB_PROC_INIT

DESCRIPTION

  This function initializes the SIB processing procedure.
  This function needs to be called once only, at powerup.
  The SIB database and the SIB events database are cleared
  by this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.
  
SIDE EFFECTS

  None.

===========================================================================*/
extern void rrc_sib_proc_init( void );




/*===========================================================================

FUNCTION RRCSIB_CHANGE_SERVING_CELL_STATUS

DESCRIPTION

  This function is used to give information about any change
  in the serving cell due to cell selection or cell reselection.
  The calling procedure needs to indicate whether a new cell
  has been successfully selected. If so, it should give the PLMN
  and cell ids for the cell. Only after all the required
  sibs for that cell have been read, this function should be
  called.
  
  This function must be used ONLY by the cell selection and
  reselection procedure.
  
DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS if the serving cell status was changed correctly.
  FAILURE if the PLMN and cell id can't be found in the database.
  
SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrcsib_change_serving_cell_status
(
  boolean new_cell_selection_succeeded,
            /* set to TRUE if a new cell has been selected through
            cell selection or reselection. Set to FALSE if a new
            cell reselection failed. */
  boolean existing_serving_cell_valid,
            /* Set to TRUE if we can go back to the existing serving
            cell. This is only used if new_cell_selection_succeeded
            is set to FALSE. Set to FALSE if the existing cell is
            no longer valid and the new cell selection failed. */
  rrc_plmn_identity_type plmn,    
           /* The PLMN Id for the cell */ 
  rrc_int_cell_id_type cell       
           /* The cell id for the cell */
);



/*===========================================================================

FUNCTION RRCSIB_CHECK_AND_UPDATE_SIB7_VALIDITY

DESCRIPTION

  This function checks if the SIB7 for the current active
  cell is valid or not. If it's not valid it will generate 
  a command to read SIB7 again. Then it will send a 
  command RRC_SIB7_UPDATE_STATUS_IND to indicate to the
  calling procedure if the SIB7 read succeeded.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE if SIB7 is valid. FALSE if SIB7 is not valid. If
  FALSE is returned a RRC_SIB7_UPDATE_STATUS_IND will be sent
  to the calling procedure. 
  
SIDE EFFECTS

  BCCH logical channel may be enabled to read SIB7.

===========================================================================*/
boolean rrcsib_check_and_update_sib7_validity
(
  rrc_proc_e_type  procedure  /* The procedure that needs the SIB7 */
);
/*===========================================================================

FUNCTION rrcsib_check_if_band_supported

DESCRIPTION

  This function checks if the band is supported by the UE 
  or not.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE if band is supported
  FALSE if band is not supported.
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrcsib_check_if_band_supported
(
  rrc_csp_band_class_type band_class,
  boolean for_serving_cell, 
  rrc_plmn_identity_type plmn
);
#ifdef FEATURE_WCDMA_REL10 
#error code not present
#endif

#ifdef FEATURE_WCDMA_HS_RACH

/*===========================================================================

FUNCTION RRCSIB_UPDATE_SIB7_HS_RACH

DESCRIPTION

  This function checks if the ul_Interference parameter is present in SIB5 or SIB7.
  If not then a SIB7 update is required.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE if SIB7 update is required 
  
SIDE EFFECTS

  BCCH logical channel may be enabled to read SIB7.

===========================================================================*/
boolean rrcsib_update_sib7_hs_rach
(
  void  /* The procedure that needs the SIB7 */
);

#endif


/*===========================================================================

FUNCTION RRC_SIB_PROC_VARIABLES_INIT

DESCRIPTION

  This function initializes the SIB variables to their
  initial state. This may be called at powerup as well as
  when the procedure needs to be re-initialized such as
  due to Factory Test Mode or Low Power Mode.

DEPENDENCIES

  None.

RETURN VALUE

  None.
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sib_proc_variables_init( void );

/*===========================================================================

FUNCTION RRC_SIB_PROC_EVENTS_INIT

DESCRIPTION

  This function initializes the SIB events to their
  initial state. This may be called at powerup as well as
  when the procedure needs to be re-initialized such as
  due to Factory Test Mode or Low Power Mode.

DEPENDENCIES

  None.

RETURN VALUE

  None.
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sib_proc_events_init( void );

/*===========================================================================

FUNCTION RRCSIB_FORCE_SIB_EVENT_INIT

DESCRIPTION

  This function will be used in the following 2 scenarios
  
  1. When RCE needs to abort SIB procedure from reading the SIBs when it had earlier
     requested to update SIB7
     
     Action : SIB procedure will clean up the event specific variables associated
     It will delete all the SIB's that it had partially read for that event. It will
     send CNF with failure to RCE. It will send a CHANNEL_CFG_REQ to LLC procedure 
     to tear down the S-BCCH.
     
  2. CSP can call this function when it detects Out of service area
  
     Action : SIB will clean up all variables associated with the active event. It will
     delete all the sibs that it had partially read. Send FAILURE to the procedure which
     requested it. No need to send a tear down to LLC as by this time all the channels
     would already have been torn down.
  
DEPENDENCIES

  None.

RETURN VALUE

  
  
SIDE EFFECTS

  

===========================================================================*/
void rrcsib_force_sib_event_init(rrcsib_event_abort_cause_e_type abort_cause) ;

/*===========================================================================

FUNCTION RRCSIB_CHECK_MIB_READ_STATUS

DESCRIPTION

  This is the function get's the current active index of SIB_EVENTS.
  Takes care of pending configuration for a particular event.
  
DEPENDENCIES
 
  None.

RETURN VALUE
  TRUE:  If MIB is already read
  FALSE: If MIB is yet to be read
  
SIDE EFFECTS

  None.
 
===========================================================================*/
boolean rrcsib_check_mib_read_status(boolean for_serv_cell);
/*===========================================================================

FUNCTION RRCSIB_GET_SIB_7_UPDATE_STATUS

DESCRIPTION

  API to provide info whether SIB_7 update is going on or not.
    
DEPENDENCIES

  None.

RETURN VALUE

SUCCESS:  SIB_7 update is on-going
FAILURE:  SIB_7 update is not-on-going  
  
SIDE EFFECTS

  

===========================================================================*/
uecomdef_status_e_type rrcsib_get_sib_7_update_status(void);

/*===========================================================================

FUNCTION RRC_BPLMN_RETURN_SIB_FOR_CELL

DESCRIPTION

  This function returns a pointer to the requested SIB.
  if this particular SIB has not been read completely for that cell, 
  this function returns NULL.
  
  The pointer is typecast to void since different sibs have
  different type definitions. The calling function is responsible
  for typecasting the pointer to the required SIB type.
   
DEPENDENCIES

  NONE

RETURN VALUE

  Pointer to the requested SIB. 
  "NULL" if the requested SIB has not been read.
  

SIDE EFFECTS

  None.

===========================================================================*/
void * rrc_bplmn_return_sib_for_cell
(
  rrc_SIB_type  req_sib          /* The SIB that is required */
);
/*===========================================================================

FUNCTION RRCSIB_BPLMN_INIT_PREV_FREQ_PSC

DESCRIPTION

  This function intializes previously scanned Freq and PSC during BPLMN search to 0.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcsib_bplmn_init_prev_freq_psc
(
  void
);

/*===========================================================================

FUNCTION RRCSIB_FREE_BPLMN_SIBS

DESCRIPTION

  This function frees the memory used by a System Information
  Block stored in the BPLMN SIB database using the ASN.1 free pdu function.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcsib_free_bplmn_sibs
(
  void
);

/*===========================================================================

FUNCTION RRCSIB_IS_SIB_ACTIVE

DESCRIPTION

  Determines whether any event is active in SIB or not.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcsib_is_sib_active
(
  void
);


/*===========================================================================

FUNCTION RRCSIB_GET_SIB_SLEEP_SCHEDULE_TABLE_LOCATION

DESCRIPTION

  This function returns the location to the sib sleep table.This 
  would be passed to L1 in the CPHY_SETUP_REQ for N-BCCH 
  
DEPENDENCIES
 
  None.

RETURN VALUE

  Pointer to the table

SIDE EFFECTS

  None.
     
===========================================================================*/
uint8* rrcsib_get_sib_sleep_schedule_table_location
(
  void
);


/*===========================================================================

FUNCTION  rrcsib_update_sixhr_sib_timer

DESCRIPTION

  This function will Implement the required six hours 
  Timer for each of the valid sib index.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_update_sixhr_sib_timer(void);

/*===========================================================================

FUNCTION rrcsib_is_need_to_cleanup_cell

DESCRIPTION

  This function will check whether the six hours Criteria needs 
  to be checked for this sib event.
  
DEPENDENCIES
  None.

RETURN VALUE

  TRUE: If six hours Criteria needs to be checked 
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_need_to_cleanup_cell(sib_events_index_e_type event_index);


/*===========================================================================

FUNCTION rrcsib_is_event_sixhrs_sib_update

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  due to six hrs sib timer expiry
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If this event is UPDATE_SIBs due to six hrs sib timer expiry
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_event_sixhrs_sib_update
(
sib_events_index_e_type  event_index
);
/*===========================================================================

FUNCTION rrcsib_is_sixhrs_sib_timer_expired

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  return TRUE if six hrs sib timer has expired for serving cell
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If six hrs sib timer has expired for serving cell
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_sixhrs_sib_timer_expired
(
sib_events_index_e_type  event_index
);
/*===========================================================================

FUNCTION rrcsib_is_current_event_due_to_sfn_drift

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  due to SFN drift
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: UPDATE_SIBs is due to SFN drift
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_current_event_due_to_sfn_drift
(
void
);
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
/*===========================================================================

FUNCTION rrcsib_can_sib7_event_start

DESCRIPTION

  This function will check whether the sib-7 event be started.
  condition for it is UE must be in FACH state & none of other 
  sib events are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If sib-7 event can be started.
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_can_sib7_event_start
(
void
);
/*===========================================================================

FUNCTION rrc_sib_is_current_event_sib7_in_fach

DESCRIPTION

  This function will check whether the current event is sib-7 event 
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If event is sib-7 event in FACH.
  FALSE: If event is other than sib-7 event.

SIDE EFFECTS
  None.
     
===========================================================================*/
 boolean rrc_sib_is_current_event_sib7_in_fach
 (
 void
 );
/*===========================================================================

FUNCTION rrcsib_build_sib7_schedule_table

DESCRIPTION

  This function updates the SIB schedule table
  for SIB-7 only.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_build_sib7_schedule_table
(
  rrc_SIB_type sib_type,                /* Indicates whether this update is from MIB or SBs */
  sib_events_index_e_type event_index   /* Index of the current event */
#ifdef FEATURE_WCDMA_HS_FACH_DRX
  ,uint32 skip_sfn                    /*Number of sfns to skip */
#endif
);
/*===========================================================================
FUNCTION rrc_sib_is_sib_segment_needed_by_rrc_fach
DESCRIPTION
  This function will check whether the currently received 
  SIB segment by L1 is needed by RRC or not.If its not 
  needed then L1 will not forward this sib segment to RRC.

  The input to this function is SFN, of currently received 
  SIB segment.
DEPENDENCIES
  None.
RETURN VALUE
  TRUE: If this sib segment is needed by RRC.
  FALSE: other wise.
SIDE EFFECTS
  None.
===========================================================================*/
 boolean rrc_sib_is_sib_segment_needed_by_rrc_fach
 (
 uint16 sfn
 );
#endif

/*===========================================================================

FUNCTION rrcsib_gen_bit_mask_for_schedule_table

DESCRIPTION

  This function will generate Bit mask needed for filling 
  SIB schedule table
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
uint8 rrcsib_gen_bit_mask_for_schedule_table
(
  uint8 byte_offset
);
/*===========================================================================

FUNCTION check_event_completion_status_for_cell_id_read

DESCRIPTION

  This function checks the status of the cell id read sib event
  in cell dch state of RRC, the sib event index is passed to this
  function. If all SIBs(i.e.SIB3 only) for the event have been received, 
  it will return TRUE. Else it will return FALSE.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is event has been completed, else FALSE.

SIDE EFFECTS

  None.
  
===========================================================================*/
boolean check_event_completion_status_for_cell_id_read
(
  uint32 index
);
/*===========================================================================
FUNCTION rrc_sib_is_current_event_sib_read_in_dch
DESCRIPTION
  This function will check whether the current event is sib reading in dch event 
  DEPENDENCIES
  None.
RETURN VALUE
  TRUE: If event is sib reading in DCH.
  FALSE: otherwise.
SIDE EFFECTS
  None.
===========================================================================*/
boolean rrc_sib_is_current_event_sib_read_in_dch
(
void
);

/*===========================================================================
FUNCTION RRC_SIB_GET_PROC_ID_FOR_SIB_READ_IN_DCH
DESCRIPTION
  This function will return the proc id which has initiated sib reading in dch
  DEPENDENCIES
  None.
RETURN VALUE
  Proc id  If event is sib reading in DCH.
  PROC_NONE: otherwise.
SIDE EFFECTS
  None.
===========================================================================*/
rrc_proc_e_type rrc_sib_get_proc_id_for_sib_read_in_dch
(
  void
);

/*===========================================================================

FUNCTION rrcsib_check_if_extension_sib_present

DESCRIPTION

  This function checks the status of the sib event, the index of
  which is passed to this function. If all SIBs for the event
  have been received, it will return TRUE. Else it will return FALSE.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is event has been completed, else FALSE.

SIDE EFFECTS

  None.
  
===========================================================================*/
boolean rrcsib_check_if_extension_sib_present
(
  uint32 dbindex    /* Database index    */
);
#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION rrcsib_check_if_sib19_present

DESCRIPTION

  This function checks whether scheduling information for SIB19 is present in
  MIB/SB1/SB2 of the cell stored in the dbindex passed as argument.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is SIB19 is scheduled, else FALSE.

SIDE EFFECTS

  None.
  
===========================================================================*/
boolean rrcsib_check_if_sib19_present
(
  uint32 dbindex    /* Database index    */
);
#endif

/*===========================================================================

FUNCTION rrcsib_check_if_connected_mode_sibs_present

DESCRIPTION

  This function will be used to find if SIB4/6/12 Ind present in 
  encoded format of SIB3/5/11
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If SIB4/6/12 Ind present in SIB3/5/11
  FALSE - Otherwise
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrcsib_check_if_connected_mode_sibs_present
(
  byte val,
  rrc_SIB_type sib_type
);

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
/*===========================================================================
FUNCTION rrc_sib_can_s_bcch_torndown_in_fach_to_pch_transition
DESCRIPTION
  This function will return status on whether S-BCCH needs to be dropped at FACH to PCH transition
  DEPENDENCIES
  None.
RETURN VALUE
TRUE - Drop S-BCCH
FALSE - otherwise
SIDE EFFECTS
  None.
===========================================================================*/
boolean rrc_sib_can_s_bcch_torndown_in_fach_to_pch_transition
(
  void
);
#endif

/*===========================================================================
FUNCTION rrcsib_decode_sib7_in_update_sibs

DESCRIPTION

  This function decodes and saves the sib7 
  received for UPDATE SIBS event.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void rrcsib_decode_sib7_in_update_sibs
(
  sib_events_index_e_type index   /* Index to the SIB events database
                                  in which SIBs have been completed. */
);

/*===========================================================================
FUNCTION rrcsib_initiate_cell_selection

DESCRIPTION

  This function sends initiate cell selection indication with cause as OOS, if current state
  is Disconnected/Connecting. If the current state is PCH/FACH, it sends CU initiate req with cause 
  as Reentered Service Area.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void rrcsib_initiate_cell_selection
(
  void
);

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
/*===========================================================================
FUNCTION rrcsib_can_nw_support_deferred_sib

DESCRIPTION

  This function checks whether Deferred SIB reading is supported by the UTRAN or not. This is done 
  by looking at the IE 'Deferred measurement reading support' present in SIB 3.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean: True, if Deferred SIB reading is supported, else false

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcsib_can_nw_support_deferred_sib
(
  int32 dbindex
);


/*===========================================================================
FUNCTION rrcsib_is_dmcr_ie_needed

DESCRIPTION

  This function checks whether DMCR IE is needed for the OTA.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean: 

SIDE EFFECTS
  None.

===========================================================================*/

boolean rrcsib_is_dmcr_ie_needed(void);


/*===========================================================================
FUNCTION rrcsib_is_sib11_11bis_12_read_deferred

DESCRIPTION

  This function checks whether reading of SIB11/11bis/12 is deferred for the serving cell.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean: True, if reading of SIB11/11bis/12 is deferred, else false

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcsib_is_sib11_11bis_12_read_deferred
(
  rrcsib_deferred_sib_type deferred_sib_type
);
#endif

/*===========================================================================

FUNCTION rrc_sib_initialize_supported_sibs_lookup_table

DESCRIPTION

  Initializes lookup table of sibs that are supported.

DEPENDENCIES
 
  None.

RETURN VALUE
 
  None.

SIDE EFFECTS

  None.
        
===========================================================================*/
void rrc_sib_initialize_supported_sibs_lookup_table
(
  void
);

/*===========================================================================

FUNCTION RRCSIB_CHECK_IF_SIBS_SCHEDULED_DURING_PICH

DESCRIPTION

  This function checks if sib1 or sib3 is scheduled during the 
  PICH of next DRX cycle. 

DEPENDENCIES

  None.
  
RETURN VALUE
  
  TRUE if they are scheduled.
  FALSE if they are not scheduled.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcsib_check_if_sibs_scheduled_during_pich(void);

#ifdef FEATURE_3GPP_CSFB

/*===========================================================================

FUNCTION  rrc_sibdb_invalidate_sib_v_tag_for_csfb

DESCRIPTION

  This function will invalidate the sibdb for requested sib mask

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrc_sibdb_invalidate_sib_v_tag_for_csfb
(
  uint32  req_sib_bit_mask,          /* SIB's  that are required to be invalidated*/
  rrc_plmn_identity_type plmn,       /* The PLMN Id for the cell */ 
  rrc_int_cell_id_type cell          /* The cell id for the cell */
);

/*===========================================================================

FUNCTION  rrcsib_read_system_info_container

DESCRIPTION

  This function will perform all the required processing needed once SIBs have been read and the SIB event is to be 
  completed. 

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_read_sibs_from_sys_info_container
(
  rrc_cmd_type *cmd_ptr   /* Pointer to received SIB event */
);

/*===========================================================================

FUNCTION rrc_sib_post_complete_for_csfb_sib_read

DESCRIPTION

  This function posts complete to CSP on CSFB SIB read completed.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/

void rrc_sib_post_complete_for_csfb_sib_read
(
   rrc_sib_status_e_type status /* Status of the SIB event-success or failure*/
);

/*===========================================================================

FUNCTION  rrcsib_update_sibdb_csfb_sib_cont

DESCRIPTION

  This function will update SIBDB related info necessary for SIB container durin CSFB
  for for all SIBs. This function frees the decoded and encoded SIB DB contents and also 
  invalidate the necessary parameters so that info from SIB container can be used


DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_update_sibdb_csfb_sib_cont
(
  uint32 sib_db_index,
  sib_events_index_e_type index,
  rrc_System_Information_Container  *rrc_dec_sys_info_container
);
#endif

/*===========================================================================

FUNCTION rrcsib_is_event_update_sibs_active

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If this event is UPDATE_SIBs due to deferred SIB reading.
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_event_update_sibs_active
(
  void
);
#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*===========================================================================

FUNCTION rrcsib_sib7_schedule_ind

DESCRIPTION

  This function sends SIB7 schedule indication to l1 via LLC
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_sib7_schedule_ind
(
  void
);
#endif

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION rrcsib_send_empty_table

DESCRIPTION

  Send empty table to wl1 if boolean is set else masks 1s with generated table
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_send_empty_table
(
  sib_events_index_e_type event_index,
  boolean is_empty
);

/*===========================================================================
FUNCTION    RRCSIB_CHECK_IF_SIB_G_PAGE_COLLIDE 

DESCRIPTION
  This function will check if G paging occasion collides with SIB of max rep rate.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if collides, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsib_check_if_sib_g_page_collide
(
  uint8 gap_duration
);

/*===========================================================================
FUNCTION    RRCSIB_INTRODUCE_HOLES_FOR_G_PAGE 

DESCRIPTION
  This function will check if G page collides with SIB of maximum rep rate,
  if not sends table with holes introduced
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcsib_introduce_holes_for_g_page
(
  uint8 duration
);
#endif

/*===========================================================================

FUNCTION rrcsib_send_sib7_ind

DESCRIPTION

  Sends SIB7 update status indication
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_send_sib7_ind(void);

#endif /* RRCSIBPROC_H */

