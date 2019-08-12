#ifndef CONFIG_H
#define CONFIG_H

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/aconfig.h_v   1.7   21 Oct 2001 12:05:32   skoganti  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/aconfig.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/02   JC      Removed declaration of default_powrer[], not used in code.
04/10/01   jca     Added include of comdef.h.
06/11/01   TS      Added GSM build support.
09/18/01   sbk     GPRS related system constants declared
10/21/01   sbk     WCDMA_GSM_24008 define added as the new revision level
07/20/02   vdr     Added new definitions for access technology list
03/18/09   abhi    Removing GRR only used variable as they are moved to GRR
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/********************************************/
/* Define the SVN (Software Version Number) */
/********************************************/

#define SVN 00

/************************************************/
/* Define the MS capability in term of channels */
/************************************************/

#define DUAL_RATE_SUPPORTED 0
#define FULL_RATE_ONLY      1
#define SDCCH_ONLY          2

/***********************************************/
/* Define the MT SMS point to point capability */
/***********************************************/

#define PP_SMS_SUPPORTED     1
#define PP_SMS_NOT_SUPPORTED 0

/************************************************/
/* Define the Pseudo Synchronisation capability */
/************************************************/

#define PS_CAPABILITY_NOT_PRESENT 0
#define PS_CAPABILITY_PRESENT     1

/******************/
/* Revision Level */
/******************/

#define PHASE_1 0
#define PHASE_2 1
#define WCDMA_GSM_24008 2

/**********************************************/
/* Controlled Early Classmark Sending support */
/**********************************************/

#define CECS_NOT_IMPLEMENTED     0
#define CECS_IMPLEMENTED         1


/**********************************************/
/* Encryption supported                       */
/**********************************************/

#define A5_1_SUPPORTED     TRUE
#define A5_2_SUPPORTED     TRUE
#define A5_3_SUPPORTED     FALSE
#define A5_4_SUPPORTED     FALSE
#define A5_5_SUPPORTED     FALSE
#define A5_6_SUPPORTED     FALSE
#define A5_7_SUPPORTED     FALSE

/**********************************************************/
/* Access Technology Type (see 3GPP 24.008 Table 10.5.146 */
/**********************************************************/

#define ACCESS_TECH_GSM_P  0x00
#define ACCESS_TECH_GSM_E  0x01
#define ACCESS_TECH_GSM_R  0x02
#define ACCESS_TECH_GSM_1800 0x03
#define ACCESS_TECH_GSM_1900 0x04
#define ACCESS_TECH_GSM_450  0x05
#define ACCESS_TECH_GSM_480  0x06
#define ACCESS_TECH_GSM_850  0x07

/*************************************/
/* Define the SS screening indicator */
/*************************************/

extern byte sm_capability;
extern byte ss_screening_indicator;
extern byte ps_capability;
extern byte revision_level;
extern byte ES_IND;
extern boolean SIM_3_volt_supported;
extern boolean SIM_5_volt_supported;

/* GPRS */
extern boolean sm_capability_gprs;
extern boolean ucs2_support_default_preferred;
extern boolean solsa_supported;
extern boolean PFC_feature_mode_supported;

extern boolean VGCS_supported;
extern boolean VBS_supported;

extern boolean compact_interference_caps;
extern boolean UMTS_FDD_RAT_supported;
extern boolean UMTS_TDD_RAT_supported;
extern boolean CDMA2000_RAT_supported;

#endif /* CONFIG_H */

