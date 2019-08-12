#ifndef CUSTIMS_H
#define CUSTIMS_H
/*===========================================================================

                           C U S T   I M S

DESCRIPTION
  Customer file for IMS features

  Copyright (c) 2006 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/
/*
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:34 $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/ims/cust/inc/custims.h#1 $
*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/07   zxm     Enable FEATURE_IMS_ENABLERXDM and FEATURE_IMS_ENABLERPRESENCE
05/10/06   RI      Initial revision.
09/05/08   Ravi    Removed QDJ section as per Sanjini's request.
19/05/08   Ravi    Removed QIPCALL_FEATURE_SIMPLE_IP  as per Sanjini's request.
09/06/08   Ravi    Removed FEATURE_QIMF_03_04 as per Sanjini's request
18/08/08   Ravi    Added PSVT and QVIEDOPHONE flags as per Sanjini's request.
10/02/12   sameer  Added FEATURE_IMS_QMI_SETTINGS_SERVICE
28/03/12   gmishra Added FEATURE_IMS_QMI_PRESENCE_SERVICE
20/04/12   amurali Added FEATURE_IMS_QMI_VIDEO_SERVICE
01/04/12   snamdev Added FEATURE_IMS_QMI_REGISTRATION_APPS_SERVICE
28/11/14   Dipak   Added strcmp_c CR:756274
===========================================================================*/

#ifdef FEATURE_QDSP6

//#ifndef FEATURE_IMS_PROCESS
//top level feature removed: #define FEATURE_IMS_PROCESS
//#endif

/* IMS feature flag */
#define FEATURE_IMS

//top level feature removed: #define FEATURE_IMS_FW_API
#define FEATURE_IMS_FW_API_NOMEDIA

//////////////////////////////////////////////////////////////////////////////
/// QIPCALL Section : Begin
//////////////////////////////////////////////////////////////////////////////


//top level feature removed: #define FEATURE_VOIP
//top level feature removed: #define FEATURE_IP_CALL
//top level feature removed: #define FEATURE_3GPP_CSFB

//top level feature removed: #define MSMAUD_VOC_AMR

//top level feature removed: #define FEATURE_SMS_OVER_IP

#define FEATURE_IMS_QMI_SETTINGS_SERVICE

//#define FEATURE_IMS_QMI_PRESENCE_SERVICE Moved to IMS Scons

//#define FEATURE_IMS_QMI_VIDEO_SERVICE moved to ims .builds

#define FEATURE_IMS_QMI_REGISTRATION_APPS_SERVICE

#define FEATURE_IMS_QMI_DCM_QCCI_CLIENT

//#define FEATURE_IMS_QMI_LOC_QCCI_CLIENT

//#define FEATURE_IMS_HYBRID

#define FEATURE_IMS_QMI_IMSRTP

#define FEATURE_IMS_QMI_IMSRTP_CLIENT

#ifdef FEATURE_STRCMP_WRAP
#define strcmp strcmp_c
#endif  //FEATURE_STRCMP_WRAP

#endif //FEATURE_QDSP6


#endif /* CUSTIMS_H */
