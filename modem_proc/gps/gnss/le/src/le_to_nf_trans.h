/*============================================================================
  FILE:         le_to_nf_trans.h

  OVERVIEW:     Temporary file to allow LE to call NF-internal fuctions, used
                during the transition of most NF functionality to LE module
                outside MGP.
                To be obsolete once NF module fully integrated into LE module.

                Transition phase: 
                  GNSS/LE -> GNSS/MGP -> GNSS/MGP/PE -> GNSS/MGP/PE/NF 
                  for access to NF/src (& NF/inc)
                Future: 
                  GNSS/MGP/PE/NF files will be moved to GNSS/LE 
                  so all the legacy NF files are directly in LE/src & LE/inc
                  along with new LE files, so there's normal direct access
                  to LE/src/*.h 
                  then all of these transitional pipes can be removed

  DEPENDENCIES: NA
 
                Copyright (c) 2015 QUALCOMM Atheros, Inc.
                All Rights Reserved.
                Qualcomm Atheros Confidential and Proprietary
                                 
                Copyright (c) 2015 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/


/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/le/src/le_to_nf_trans.h#1 $ 
   $DateTime: 2016/12/13 07:59:45 $ 
   $Author: mplcsds1 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-24  wtr  new file for LE module (transition from nf to le)

============================================================================*/

#ifndef _LE_TO_NF_TRANS_H
#define _LE_TO_NF_TRANS_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "com_dtypes.h" /* for boolean */
#include "le_types.h" /* for le_WifiCovFixRequestType */
#include "gnss_common.h" /* for gnss_ExternCoarsePosInjectionType */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*======================================================================

  FUNCTION 		  le_trans_WifiCovMeasAdPut()

  DESCRIPTION 	 Pass message through from LE to MGP (toward, PE & NF)

  DEPENDENCIES  No use of file or regional statics.
                No sequencing requirements.

  PARAMETERS      Description of the parameters to the functions

    pz_WifiCovFix - data to pack into IPC to send

  RETURN VALUE   True if input message was successfully enqueued

  SIDE EFFECTS   None

======================================================================*/
boolean le_trans_WifiCovMeasAdPut(
    const le_WifiCovFixRequestType *pz_WifiCovFix);

/*======================================================================

  FUNCTION       ale_trans_WifiCovLocProcess()

  DESCRIPTION    Pass message through from LE to MGP (toward, PE & NF)

  DEPENDENCIES   No use of file or regional statics.
                 No sequencing requirements.

  PARAMETERS     Description of the parameters to the functions

    pz_WifiCovLoc - data to inject to ALE 

  RETURN VALUE   Result type for logging/stats

  SIDE EFFECTS  None

=============================================================================*/
ale_WclResultEnumType ale_trans_WifiCovLocProcess( 
  le_WifiCovLocType *pz_WifiCovLoc );

/*=============================================================================

  FUNCTION      ale_trans_ProcessBestPosReq

  DESCRIPTION   Pass message through from LE to MGP (toward, PE & NF)

  DEPENDENCIES  None

  PARAMETERS    u_Request: input, request type
                v_IsWifiFixResponse: TRUE iff this is for a Wifi fix response
                pz_WifiCtrlResult: input, often NULL, to sometimes pass through
                Wifi fix information

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void ale_trans_ProcessBestPosReq( 
    U8 u_Request,  
    boolean v_IsWifiFixResponse,
    const le_WifiFixCtrlResultType *pz_WifiCtrlResult);

/*=============================================================================

  FUNCTION      ale_trans_GetBestNavSolution

  DESCRIPTION   Pass message through from LE to MGP (toward, PE & NF)

  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  Pointer to the best position

  SIDE EFFECTS  None

=============================================================================*/
gnss_NavSolutionStructType * ale_trans_GetBestNavSolution(void);

/*==============================================================================

  FUNCTION      ale_trans_PositionsAreConsistent

  DESCRIPTION   Pass message through from LE to MGP (toward, PE & NF)

  DEPENDENCIES  None

  PARAMETERS    pz_Ns1: input, one nav. sol. to compare
                pz_Ns2: input, the other one
                pf_*  : output, optional, values for logging externally

  RETURN VALUE  Enumeration of consistency check result - 
                  unchecked, consistent, or inconsistent

  SIDE EFFECTS  None

=============================================================================*/
le_ConsistencyCheckResultType ale_trans_PositionsAreConsistent(
                                   const gnss_NavSolutionStructType *pz_Ns1,
                                   const gnss_NavSolutionStructType *pz_Ns2,
                                   FLT * pf_Ns1Cep99_m, 
                                   FLT * pf_Ns2Cep99_m,
                                   FLT * pf_SurfaceDist_m);


/*==============================================================================

  FUNCTION      ale_trans_InjectTlePosition

  DESCRIPTION 	Inject TLE position to ALE (via MGP, & PE to NF for now)

  DEPENDENCIES  None

  PARAMETERS    pz_TlePos: input, pointer to a TLE position

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void ale_trans_InjectTlePosition( 
                               const ale_CellDBInputPosStructType *pz_TlePos );

/*==============================================================================

  FUNCTION      le_trans_InjectPdrControl

  DESCRIPTION 	Inject PDR control to NF (via MGP & PE per modularity req's)

  DEPENDENCIES  None

  PARAMETERS    
    b_PdrDisableFlag: PDR control config, TRUE if feature is 
                          disabled, FALSE otherwise
    e_PdrCtrlSrc: Source of the config, build-config, QMI or NV

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
boolean le_trans_InjectPdrControl
(
  boolean b_PdrDisableFlag,
  gnss_PdrCtrlSrcEnumType e_PdrCtrlSrc
);

/*==============================================================================

  FUNCTION      le_trans_GetInitDeltaLeapSec

  DESCRIPTION   Pass message through from LE to MGP (toward, PE & NF)

  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  nfz_NfParams.u_InitDeltaLeapSec

  SIDE EFFECTS  None

=============================================================================*/
uint8 le_trans_GetInitDeltaLeapSec();

#endif  /* _LE_TO_NF_TRANS_H */
