#ifndef MS_H
#define MS_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/ms.h_v   1.17   24 Jun 2002 19:39:12   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/ms.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/02/07   kgj     Added stuff for FEATURE_CGPS
20/03/07   cs      Add new definition for TCXO_MGR_3->GL1 restriction request 
07/03/07   cs      Add new definition for CGPS->GL1 timetag request 
04/06/05   kvk     New feature message sets defined for UMTS BMC
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
06/08/04   cd      LCS support.  Added MS_MM_LCS message set definition
02/12/04   vdr     Added new MS_WMS_CB message set definition under FEATURE_GSM_CB
02/02/04   vk      Added new MS_LSM_RRC message set definition under FEATURE_MMGPS
04/15/03   sbs     Added new MS_LSM_RR message set definition
04/14/03   lalit   Added new MS_LSM_L1 message set definition
02/14/03   ab      Added new MS_CC_MM_PS message set definition
02/12/02   vdr     Added new MS_CM_MM message set definition
02/12/02   vdr     Added new MS_WMS_MM message set definition
08/01/02   ATM     Added InterRAT support (RRC <-> RR)
06/18/02   ks      Added GPRS support
05/13/02   ATM     Split RRC enum into it's parts (internal/L1/MAC/RLC) 
                   NOTE: MM and TC were already defined
05/06/02   ATM     Added new MSGSETs for WCDMA
05/06/02   ATM     Removed redefinition of MS_SNDCP_SM
05/06/02   ATM     Added new MSGSETs for GPRS
04/22/02   ks      Added MS_CM_RABM messege set definition for CM/RABM
01/30/02   mks     Added new message set definitions for TC
05/30/01   ks      Added new message set definitions for SM/RABM.
05/03/01   jca     Added new message set definitions.

7/13/01    cd      Replaced MS_MN_CM by MS_MN_CM
                   Added MS_MN_UASMS

08/14/01   ab      Renamed connection manager reference to CNM from CM.

08/24/01   ab      Restored MS_CM_SM name.

===========================================================================*/

/*
 * Definition of message sets
 * --------------------------
 */
 
#define  MS_CC_MM          1   /* messages between CC and MM          */
#define  MS_MN_CM          2   /* messages between MN and Call Manager*/
#define  MS_MN_CC          3   /* messages between MN and CC          */
#define  MS_MN_SS          4   /* messages between MN and SS          */
#define  MS_MN_SMS         5   /* messages between MN and SMS         */

#define  MS_TIMER          6   /* Messages indicating timeouts        */

#define  MS_CM_REG         7   /* messages between CM and REG         */
#define  MS_MN_REG         8   /* messages between MN and REG         */
#define  MS_MM_RR          9   /* messages between MM and RR          */
#define  MS_MM_REG         10  /* messages between MM and REG         */
#define  MS_TEST_HARNESS   11  /* messages between test harness tasks */
#define  MS_GHDI_EXPORT    12  /* messages from/to GHDI               */

#define  MS_GSDI           13  /* messages from/to GSDI               */

#define  MS_GHDI_LOCAL     14  /* Private GHDI messages.              */
#define  MS_MN_TEST        15  /* special MN test messages            */
#define  MS_CM_DS          16  /* messages between CM and DS          */
#define  MS_RR_L2          17  /* messages between RR and layer 2     */
#define  MS_RR_L1          18  /* messages between RR and layer 1     */
#define  MS_CM_RR          19  /* messages between CM and RR          */
#define  MS_CM_CB          20  /* messages between CM and CB          */
#define  MS_CB_RR          21  /* messages between CB and RR          */
#define  MS_CB_L2          22  /* messages between CB and L2          */
#define  MS_CB_L1          23  /* messages between CB and L1          */
#define  MS_DS_LOCAL       24  /* messages between DS task and ISR    */
#define  MS_L2_L1          25  /* messages between L2 and L1          */
#define  MS_GS_DIAG        26  /* messages between task and DIAG task */
#define  MS_L1_L1          27  /* messages between L1 task and L1 ISR */
#define  MS_DS_L1          28  /* messages between DS ISR and L1 Task */
#define  MS_RABM_SM        30  /* messages between SM and SNDCP tasks */
#define  MS_SNDCP_SM       31  /* messages between SM and SNDCP tasks */
#define  MS_CM_SM          32  /* messages between CNM and SM tasks   */
#define  MS_CNM_RABM       33  /* messages between CNM and RABM tasks */
#define  MS_RABM_RRC       34  /* messages between RABM and RRC tasks */
#define  MS_MN_UASMS       35  /* messages between MN and UASMS       */
#define  MS_WMS_MM         36  /* messages between WMS and MM         */
#define  MS_CM_MM          37  /* messages between CM and MM tasks    */
#define  MS_CC_MM_PS       38  /* messages between CC&MM in PS domain */

#define  MS_CGPS_GL1       39  /* messages between CGPS and GSM L1    */

#define  MS_LSM_L1         39  /* messages between LSM and GSM L1     */



/* NOTE - The aliasing is intentional for backward compatibility */
#define  MS_CGPS_RR        40  /* messages between CGPS and RR        */
#define  MS_LSM_RR         40  /* messages between CGPS and RR        */



#define  MS_WMS_CB         41  /* messages between WMS and CB         */



/* NOTE - The aliasing is intentional for backward compatibility */
#define  MS_MM_CGPS         42  /* messages between MM and CGPS       */
#define  MS_MM_LSM          42  /* messages between MM and CGPS       */



#define  MS_TCXO_GL1       43  /* Messages from Tcxo Mgr 3 and GL1    */
/* Used only for FEATURE_TRIPLE_SIM */
#define  MS_GRM_L1       44    /* Messages from GL1 ISR to GRM task   */

#define  MS_REG_CB         48  /* Messages from REG to CB*/


/* WCDMA Lower layer pseudo-message sets                      */
#define  MS_WCDMA_L1_LOCAL 90 /* msgs in wcdma/l1/l1def.h     */
#define  MS_WCDMA_L1_EXT   91 /* msgs in wcdma/l1/l1taskc.h   */
#define  MS_WCDMA_L2DL     92 /* msgs in wcdma/l2/l2dltask.h  */
#define  MS_WCDMA_L2UL     93 /* msgs in wcdma/l2/l2ultask.h  */
#define  MS_WCDMA_RRC_INT  94 /* RRC<->RRC msgs in wcdma/rrc/rrccmd.h */
#define  MS_WCDMA_RRC_L1   95 /*  L1<->RRC msgs in wcdma/rrc/rrccmd.h */
#define  MS_WCDMA_RRC_MAC  96 /* MAC<->RRC msgs in wcdma/rrc/rrccmd.h */
#define  MS_WCDMA_RRC_RLC  97 /* RLC<->RRC msgs in wcdma/rrc/rrccmd.h */
#define  MS_RRC_RR         98 /*  RR<->RRC msgs for InterRAT activity */

#define  MS_MM_RRC         100 /* messages between MM and RRC         */
#define  MS_GMM_SMS        101 /* messages between GMM and SMS        */
#define  MS_GMM_SM         102 /* messages between GMM and SM         */
#define  MS_GMM_RABM       103 /* messages between GMM and RABM       */
#define  MS_GMM_TC         104 /* messages between GMM and TC         */
#define  MS_MM_TC          105 /* messages between MM and TC          */
#define  MS_RABM_TC        106 /* messages between RABM and TC        */
#define  MS_RRC_TC         107 /* messages between RRC and TC         */
#define  MS_CM_TC          108 /* messages between CM and TC          */
#define  MS_CM_RABM        109 /* messages between CM and RABM        */


/* NOTE - The aliasing is intentional for backward compatibility */
#define  MS_CGPS_RRC     110 /* Messages between CGPS and RRC       */
#define  MS_LSM_RRC        110 /* Messages between CGPS and RRC       */


/*
** BMC Feature's message set definitions
*/


#define  MS_RRC_BMC         111 /* messages between RRC and BMC        */
#define  MS_RLC_BMC         112 /* messages between RLC and BMC        */
#define  MS_BMC_L1          113 /* messages between WCDMA L1 and BMC   */





/* GPRS message sets */
#define  MS_MAC_L1         200 /* messages between MAC and L1         */
#define  MS_MAC_RR         201 /* messages between MAC and RR         */
#define  MS_RLC_L1         202 /* messages between RLC and L1         */
#define  MS_MAC_RLC        203 /* messages between MAC and RLC        */     
#define  MS_MAC_MAC        204 /* messages between MAC and MAC        */
#define  MS_RLC_LLC        205 /* messages between RLC and LLC        */
#define  MS_LLC_GMM        206 /* messages between LLC and GMM        */
#define  MS_LLC_SNDCP      207 /* messages between LLC and SNDCP      */
#define  MS_SNDCP_PS       208 /* messages between SNDCP and PS       */

#define  MS_RLC_RR         209 /* messages between RLC and RR         */



#define  MS_MSM_SM         210 /* messages between MBMS-SM and SM     */
#define  MS_MM_MSM         211 /* messages between MBMS-SM and MM     */



#define MS_RR_RR           212 /* messages between RR and RR          */



#define MS_RR_GAN          213 /* messages between RR and GAN client  */



#define MS_MSGR_RR         214 /* Messages sent to RR via Message Router */



#define MS_ECALL_REG       215 /* messages between ECALL and NAS client  */

#define MS_RR_RR_EXT       216 /* messages between RR-RR (external trigger) */

#define MS_TDS_MACHS  239
#define MS_TDS_MACUL  240
#define MS_TDS_MACDL  241

#define MS_TDS_RLCDL  242
#define MS_TDS_RLCUL  243
#define MS_TDS_RRC  244
#define MS_TDS_L1  245

#define MS_MM_MM 247     /* Messages MM to MM internal */
#define MS_SM_SM 248     /* Messages SM to SM internal */
#define MS_REG_REG 249   /* Messages REG to REG internal */

#define  MS_RABM_TDSRRC    250  /* Messages between RABM and TD-SCDMA RRC */
#define  MS_MM_TDSRRC      251  /* Messages between MM and TD-SCDMA RRC   */
#define  MS_TDSRRC_RR      252  /* RR<->TD-SCDMA RRC msgs for InterRAT activity */
#define  MS_TDSRRC_TC      253  /* Messages between TD-SCDMA RRC and TC         */

#endif /* MS_H */
