/*===========================================================================

                         D S _ Q M I _ V O I C E _ C M _ SUPS . C

DESCRIPTION

 The Data Services Qualcomm Voice supplementary services util source file.

Copyright (c) 2004-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice_cm_sups.c#1 $ $DateTime:$ $Author:$

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/15/09    nd     Created Module.
===========================================================================*/

/*===========================================================================
 
  INCLUDE FILES FOR MODULE
 
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_QMI_SERVICE_SPLIT

#include <string.h>       
#include <stringl/stringl.h>   
#include "IxErrno.h"
#include "cm.h"
#include "amssassert.h"
#include "qmi_voice_cm_sups.h"
#include "msgcfg.h"
#include "msg_diag_service.h"
#include "qmi_mmode_task_cmd.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
#ifndef FEATURE_NO_QMI_VOICE
byte qmi_voice_cm_sups_ref;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                         GLOBAL VARIABLES

===========================================================================*/
/* mapping table for mapping service class to bearer type and code */
qmi_voice_cm_sups_bs_mapping_s_type qmi_voice_cm_sups_bs_mapping_table[] =
{
  {QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_ASYNC,      (uint8)BS_CODE,   qmi_voice_cm_sups_allAsynchronousServices },
  {QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_SYNC,       (uint8)BS_CODE,   qmi_voice_cm_sups_allSynchronousServices },
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PKT,            (uint8)BS_CODE,   qmi_voice_cm_sups_allDataPDS_Services},
  {QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_SYNC_ASYNC, (uint8)BS_CODE,   qmi_voice_cm_sups_allBearerServices},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_allDataCircuitAsynchronous},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC,           (uint8)BS_CODE,   qmi_voice_cm_sups_allDataCircuitSynchronous},
  {QMI_VOICE_CM_SUPS_ALL_TELE_SERV,             (uint8)TS_CODE,   qmi_voice_cm_sups_allTeleservices},
  {QMI_VOICE_CM_SUPS_CLASS_ALL_TS_DATA,         (uint8)TS_CODE,   qmi_voice_cm_sups_allDataTeleservices},
  {QMI_VOICE_CM_SUPS_CLASS_SMS,                 (uint8)TS_CODE,   qmi_voice_cm_sups_allShortMessageServices},
  {QMI_VOICE_CM_SUPS_ALL_TELE_SERV_EX_SMS,      (uint8)TS_CODE,   qmi_voice_cm_sups_allTeleservices_ExeptSMS},
  {QMI_VOICE_CM_SUPS_CLASS_FAX,                 (uint8)TS_CODE,   qmi_voice_cm_sups_allFacsimileTransmissionServices},
  {QMI_VOICE_CM_SUPS_CLASS_DATA,                (uint8)TS_CODE,   qmi_voice_cm_sups_allBearerServices},
  {QMI_VOICE_CM_SUPS_CLASS_VOICE,               (uint8)TS_CODE,   qmi_voice_cm_sups_allSpeechTransmissionservices},
  {QMI_VOICE_CM_SUPS_CLASS_GROUP_CALL,          (uint8)TS_CODE,   qmi_voice_cm_sups_voiceGroupCall},
  {QMI_VOICE_CM_SUPS_CLASS_BROADCAST_CALL,      (uint8)TS_CODE,   qmi_voice_cm_sups_voiceBroadcastCall},
  {QMI_VOICE_CM_SUPS_CLASS_ALL_GROUP_CALL,      (uint8)TS_CODE,   qmi_voice_cm_sups_allVoiceGroupCallServices},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PAD,            (uint8)BS_CODE,   qmi_voice_cm_sups_allPadAccessCA_Services},
  {QMI_VOICE_PLMN_SPECIFIC_TS_ALL,              (uint8)TS_CODE,   qmi_voice_cm_sups_ts_all},
  {QMI_VOICE_PLMN_SPECIFIC_TS_1,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_1},
  {QMI_VOICE_PLMN_SPECIFIC_TS_2,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_2},
  {QMI_VOICE_PLMN_SPECIFIC_TS_3,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_3},
  {QMI_VOICE_PLMN_SPECIFIC_TS_4,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_4},
  {QMI_VOICE_PLMN_SPECIFIC_TS_5,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_5},
  {QMI_VOICE_PLMN_SPECIFIC_TS_6,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_6},
  {QMI_VOICE_PLMN_SPECIFIC_TS_7,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_7},
  {QMI_VOICE_PLMN_SPECIFIC_TS_8,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_8},
  {QMI_VOICE_PLMN_SPECIFIC_TS_9,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_9},
  {QMI_VOICE_PLMN_SPECIFIC_TS_A,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_a},
  {QMI_VOICE_PLMN_SPECIFIC_TS_B,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_b},
  {QMI_VOICE_PLMN_SPECIFIC_TS_C,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_c},
  {QMI_VOICE_PLMN_SPECIFIC_TS_D,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_d},
  {QMI_VOICE_PLMN_SPECIFIC_TS_E,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_e},
  {QMI_VOICE_PLMN_SPECIFIC_TS_F,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_f},
  {QMI_VOICE_CM_SUPS_CLASS_MAX,                 (uint8)MAX_CODE,  0xFF}
};

/* See Spec 3GPP 29.002,Section 17.7.10. */
qmi_voice_cm_sups_bs_mapping_s_type qmi_voice_cm_sups_extra_bs_mapping_table[] =
{
  {QMI_VOICE_CM_SUPS_CLASS_VOICE,               (uint8)TS_CODE,    qmi_voice_cm_sups_telephony},
  {QMI_VOICE_CM_SUPS_CLASS_VOICE,               (uint8)TS_CODE,   qmi_voice_cm_sups_emergencyCalls},
  {QMI_VOICE_CM_SUPS_CLASS_FAX,                 (uint8)TS_CODE,   qmi_voice_cm_sups_facsimileGroup3AndAlterSpeech},
  {QMI_VOICE_CM_SUPS_CLASS_FAX,                 (uint8)TS_CODE,   qmi_voice_cm_sups_automaticFacsimileGroup3},
  {QMI_VOICE_CM_SUPS_CLASS_FAX,                 (uint8)TS_CODE,   qmi_voice_cm_sups_facsimileGroup4},
  {QMI_VOICE_CM_SUPS_CLASS_SMS,                 (uint8)TS_CODE,   qmi_voice_cm_sups_shortMessageMT_PP},
  {QMI_VOICE_CM_SUPS_CLASS_SMS,                 (uint8)TS_CODE,   qmi_voice_cm_sups_shortMessageMO_PP},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC,           (uint8)BS_CODE,   qmi_voice_cm_sups_allDataCDS_Services},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC,           (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDS_1200bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC,           (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDS_2400bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC,           (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDS_4800bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC,           (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDS_9600bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC,           (uint8)BS_CODE,   qmi_voice_cm_sups_general_dataCDS},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC,           (uint8)BS_CODE,   qmi_voice_cm_sups_allAlternateSpeech_DataCDS},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC,           (uint8)BS_CODE,   qmi_voice_cm_sups_allSpeechFollowedByDataCDS},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_allDataCDA_Services},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDA_300bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDA_1200bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDA_1200_75bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDA_2400bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDA_4800bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_dataCDA_9600bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_general_dataCDA},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_allAlternateSpeech_DataCDA},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC,          (uint8)BS_CODE,   qmi_voice_cm_sups_allSpeechFollowedByDataCDA},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PKT,            (uint8)BS_CODE,   qmi_voice_cm_sups_dataPDS_2400bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PKT,            (uint8)BS_CODE,   qmi_voice_cm_sups_dataPDS_4800bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PKT,            (uint8)BS_CODE,   qmi_voice_cm_sups_dataPDS_9600bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PKT,            (uint8)BS_CODE,   qmi_voice_cm_sups_general_dataPDS},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PAD,            (uint8)BS_CODE,   qmi_voice_cm_sups_padAccessCA_300bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PAD,            (uint8)BS_CODE,   qmi_voice_cm_sups_padAccessCA_1200bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PAD,            (uint8)BS_CODE,   qmi_voice_cm_sups_padAccessCA_1200_75bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PAD,            (uint8)BS_CODE,   qmi_voice_cm_sups_padAccessCA_2400bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PAD,            (uint8)BS_CODE,   qmi_voice_cm_sups_padAccessCA_4800bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PAD,            (uint8)BS_CODE,   qmi_voice_cm_sups_padAccessCA_9600bps},
  {QMI_VOICE_CM_SUPS_CLASS_DATA_PAD,            (uint8)BS_CODE,   qmi_voice_cm_sups_general_padAccessCA},
  {QMI_VOICE_PLMN_SPECIFIC_TS_ALL,              (uint8)TS_CODE,   qmi_voice_cm_sups_ts_all},
  {QMI_VOICE_PLMN_SPECIFIC_TS_1,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_1},
  {QMI_VOICE_PLMN_SPECIFIC_TS_2,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_2},
  {QMI_VOICE_PLMN_SPECIFIC_TS_3,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_3},
  {QMI_VOICE_PLMN_SPECIFIC_TS_4,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_4},
  {QMI_VOICE_PLMN_SPECIFIC_TS_5,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_5},
  {QMI_VOICE_PLMN_SPECIFIC_TS_6,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_6},
  {QMI_VOICE_PLMN_SPECIFIC_TS_7,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_7},
  {QMI_VOICE_PLMN_SPECIFIC_TS_8,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_8},
  {QMI_VOICE_PLMN_SPECIFIC_TS_9,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_9},
  {QMI_VOICE_PLMN_SPECIFIC_TS_A,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_a},
  {QMI_VOICE_PLMN_SPECIFIC_TS_B,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_b},
  {QMI_VOICE_PLMN_SPECIFIC_TS_C,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_c},
  {QMI_VOICE_PLMN_SPECIFIC_TS_D,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_d},
  {QMI_VOICE_PLMN_SPECIFIC_TS_E,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_e},
  {QMI_VOICE_PLMN_SPECIFIC_TS_F,                (uint8)TS_CODE,   qmi_voice_cm_sups_ts_f},
  {QMI_VOICE_CM_SUPS_CLASS_MAX,                 (uint8)MAX_CODE,  0xFF}
};

/*MMI to Network value Mapping*/
const qmi_voice_cm_sups_sc_table_s_type qmi_voice_cm_sups_sc_conversion_table [] =
{
  {"",        qmi_voice_cm_sups_all },
  {"002",     qmi_voice_cm_sups_allForwardingSS },
  {"004",     qmi_voice_cm_sups_allCondForwardingSS },
  {"21",      qmi_voice_cm_sups_cfu },
  {"67",      qmi_voice_cm_sups_cfb },
  {"61",      qmi_voice_cm_sups_cfnry },
  {"62",      qmi_voice_cm_sups_cfnrc },
  {"30",      qmi_voice_cm_sups_clip },
  {"31",      qmi_voice_cm_sups_clir },
  {"76",      qmi_voice_cm_sups_colp },
  {"77",      qmi_voice_cm_sups_colr },
  {"43",      qmi_voice_cm_sups_cw },
  {"330",     qmi_voice_cm_sups_allCallRestrictionSS },
  {"333",     qmi_voice_cm_sups_barringOfOutgoingCalls },
  {"353",     qmi_voice_cm_sups_barringOfIncomingCalls },
  {"33",      qmi_voice_cm_sups_baoc },
  {"331",     qmi_voice_cm_sups_boic },
  {"332",     qmi_voice_cm_sups_boicExHC },
  {"35",      qmi_voice_cm_sups_baic },
  {"351",     qmi_voice_cm_sups_bicRoam },
  {"300",     qmi_voice_cm_sups_cnap },
  {"37",      qmi_voice_cm_sups_ccbs }
};

#define QMI_VOICE_CM_SUPS_MAX_SC_ENTRY (sizeof(qmi_voice_cm_sups_sc_conversion_table)/sizeof(qmi_voice_cm_sups_sc_table_s_type))

/* MMI to Service class Mapping*/
qmi_voice_cm_sups_bsg_table_s_type qmi_voice_cm_sups_bsg_conversion_table[] =
{
  {"11",        QMI_VOICE_CM_SUPS_CLASS_VOICE},
  {"16",        QMI_VOICE_CM_SUPS_CLASS_SMS},
  {"13",        QMI_VOICE_CM_SUPS_CLASS_FAX },
  {"10",        QMI_VOICE_CM_SUPS_ALL_TELE_SERV },
  {"10",        QMI_VOICE_CM_SUPS_ALL_TELE_SERV_EXT },
  {"12",        QMI_VOICE_CM_SUPS_CLASS_ALL_TS_DATA},
  {"19",        QMI_VOICE_CM_SUPS_ALL_TELE_SERV_EX_SMS},
  {"17",        QMI_VOICE_CM_SUPS_CLASS_GROUP_CALL},
  {"18",        QMI_VOICE_CM_SUPS_CLASS_BROADCAST_CALL},
  {"20",        QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_SYNC_ASYNC},
  {"21",        QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_ASYNC},
  {"22",        QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_SYNC},
  {"24",        QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC},
  {"25",        QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC},
  {"26",        QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_PDS},
  {"99",        QMI_VOICE_CM_SUPS_CLASS_DATA_PKT},
  {"50",        QMI_VOICE_PLMN_SPECIFIC_TS_ALL},
  {"51",		QMI_VOICE_PLMN_SPECIFIC_TS_1},
  {"52",		QMI_VOICE_PLMN_SPECIFIC_TS_2},
  {"53",		QMI_VOICE_PLMN_SPECIFIC_TS_3},
  {"54",		QMI_VOICE_PLMN_SPECIFIC_TS_4},
  {"55",		QMI_VOICE_PLMN_SPECIFIC_TS_5},
  {"56",		QMI_VOICE_PLMN_SPECIFIC_TS_6},
  {"57",		QMI_VOICE_PLMN_SPECIFIC_TS_7},
  {"58",		QMI_VOICE_PLMN_SPECIFIC_TS_8},
  {"59",		QMI_VOICE_PLMN_SPECIFIC_TS_9},
  {"60",		QMI_VOICE_PLMN_SPECIFIC_TS_A},
  {"61",		QMI_VOICE_PLMN_SPECIFIC_TS_B},
  {"62",		QMI_VOICE_PLMN_SPECIFIC_TS_C},
  {"63",		QMI_VOICE_PLMN_SPECIFIC_TS_D},
  {"64",		QMI_VOICE_PLMN_SPECIFIC_TS_E},
  {"65",		QMI_VOICE_PLMN_SPECIFIC_TS_F}
};

#define QMI_VOICE_CM_SUPS_MAX_BSG_ENTRY (sizeof(qmi_voice_cm_sups_bsg_conversion_table)/sizeof(qmi_voice_cm_sups_bsg_table_s_type))

/* Sups mode type string to Mode mapping */
qmi_voice_cm_sups_mode_table_s_type qmi_voice_cm_sups_mode_input[] =
{
  {"**",       QMI_VOICE_CM_SUPS_MODE_REG},
  {"*#",       QMI_VOICE_CM_SUPS_MODE_QUERY},
  {"*",        QMI_VOICE_CM_SUPS_MODE_ENABLE},
  {"##",       QMI_VOICE_CM_SUPS_MODE_ERASURE},
  {"#",        QMI_VOICE_CM_SUPS_MODE_DISABLE},
  {"**03*",    QMI_VOICE_CM_SUPS_MODE_REG_PASSWD},
  {NULL,       QMI_VOICE_CM_SUPS_MODE_MAX}
};

/* Possible class values for corresponding supplementary services 
Per 22.004 Table A.1 the incoming, waiting call can be of any kind */                                              
uint32 qmi_voice_cm_sups_cfw_allowed_classes = (uint32)(QMI_VOICE_CM_SUPS_CLASS_VOICE |
                                         QMI_VOICE_CM_SUPS_CLASS_DATA | 
                                         QMI_VOICE_CM_SUPS_CLASS_FAX | 
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC | 
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_PAD |
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC |
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_PKT );

/* Possible class values for corresponding supplementary services 
Per 22.004 Table A.1 the incoming, waiting call can be of any kind */                                              
uint32 qmi_voice_cm_sups_clip_allowed_classes = (uint32)(QMI_VOICE_CM_SUPS_CLASS_VOICE |
                                         QMI_VOICE_CM_SUPS_CLASS_DATA | 
                                         QMI_VOICE_CM_SUPS_CLASS_FAX | 
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC | 
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_PAD |
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC |
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_PKT );

/* Possible class values for corresponding supplementary services 
Per 22.004 Table A.1 the incoming, waiting call can be of any kind */                                              
uint32 qmi_voice_cm_sups_colp_allowed_classes = (uint32)(QMI_VOICE_CM_SUPS_CLASS_VOICE |
                                         QMI_VOICE_CM_SUPS_CLASS_DATA | 
                                         QMI_VOICE_CM_SUPS_CLASS_FAX | 
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC | 
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_PAD |
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC |
                                         QMI_VOICE_CM_SUPS_CLASS_DATA_PKT );

uint32 qmi_voice_cm_sups_cb_allowed_classes =  (uint32)QMI_VOICE_CM_SUPS_CLASS_ALL;


uint32 qmi_voice_cm_sups_cw_allowed_classes =  (uint32)QMI_VOICE_CM_SUPS_CLASS_ALL;


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_ss_ref

===========================================================================*/
/*!
    @brief
    Assigning reference to supplimentary services for identifying it uniquely

    @return
    retuns SS reference value
*/
/*=========================================================================*/
uint8 qmi_voice_cm_sups_get_ss_ref(void)
{
  qmi_voice_cm_sups_ref++;
  return(qmi_voice_cm_sups_ref);
}


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_service_class_is_valid

===========================================================================*/
/*!
    @brief
    Checks if the passed service class is valid

    @return
    retuns TRUE if class is valid else returns FALSE
*/
/*=========================================================================*/
boolean qmi_voice_cm_sups_service_class_is_valid(uint32 srv_class)
{
  boolean is_valid = FALSE;
  switch(srv_class)
  {
    case QMI_VOICE_CM_SUPS_CLASS_VOICE: 
    case QMI_VOICE_CM_SUPS_CLASS_DATA: 
    case QMI_VOICE_CM_SUPS_CLASS_FAX: 
    case QMI_VOICE_CM_SUPS_CLASS_SMS:
    case QMI_VOICE_CM_SUPS_ALL_TELE_SERV:
    case QMI_VOICE_CM_SUPS_ALL_TELE_SERV_EXT:
    case QMI_VOICE_CM_SUPS_ALL_TELE_SERV_EX_SMS:
    case QMI_VOICE_CM_SUPS_CLASS_GROUP_CALL:
    case QMI_VOICE_CM_SUPS_CLASS_BROADCAST_CALL:
    case QMI_VOICE_CM_SUPS_CLASS_ALL_GROUP_CALL:
    case QMI_VOICE_CM_SUPS_CLASS_ALL_TS_DATA:
    case QMI_VOICE_CM_SUPS_CLASS_DATA_SYNC:
    case QMI_VOICE_CM_SUPS_CLASS_DATA_ASYNC:
    case QMI_VOICE_CM_SUPS_CLASS_DATA_PKT:
    case QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_SYNC:
    case QMI_VOICE_CM_SUPS_CLASS_DATA_PAD:
    case QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_ASYNC:
    case QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_SYNC_ASYNC:
    case QMI_VOICE_CM_SUPS_CLASS_ALL_DATA_PDS:
      is_valid = TRUE;
      break;
    case QMI_VOICE_PLMN_SPECIFIC_TS_ALL:
    case QMI_VOICE_PLMN_SPECIFIC_TS_1:
    case QMI_VOICE_PLMN_SPECIFIC_TS_2:
    case QMI_VOICE_PLMN_SPECIFIC_TS_3:
    case QMI_VOICE_PLMN_SPECIFIC_TS_4:
    case QMI_VOICE_PLMN_SPECIFIC_TS_5:
    case QMI_VOICE_PLMN_SPECIFIC_TS_6:
    case QMI_VOICE_PLMN_SPECIFIC_TS_7:
    case QMI_VOICE_PLMN_SPECIFIC_TS_8:
    case QMI_VOICE_PLMN_SPECIFIC_TS_9:
    case QMI_VOICE_PLMN_SPECIFIC_TS_A:
    case QMI_VOICE_PLMN_SPECIFIC_TS_B:
    case QMI_VOICE_PLMN_SPECIFIC_TS_C:
    case QMI_VOICE_PLMN_SPECIFIC_TS_D:
    case QMI_VOICE_PLMN_SPECIFIC_TS_E:
    case QMI_VOICE_PLMN_SPECIFIC_TS_F:
      is_valid = TRUE;
      break;		
    default:
      break;
  }
  return is_valid;
} /* qmi_voice_cm_sups_service_class_is_valid() */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_map_service_to_cm_mode

===========================================================================*/
/*!
    @brief
    Map the Cotrol point sent status/service to a equivalent CM mode

    @return
    returns CM mode.
*/
/*=========================================================================*/
uint8 qmi_voice_cm_sups_map_service_to_cm_mode(uint8 service)
{

  switch(service)
  {
    case  QMI_VOICE_CM_SUPS_SERVICE_ACTIVATE:
      return QMI_VOICE_CM_SUPS_MODE_ENABLE;
    case  QMI_VOICE_CM_SUPS_SERVICE_DEACTIVATE:
      return QMI_VOICE_CM_SUPS_MODE_DISABLE;
    case  QMI_VOICE_CM_SUPS_SERVICE_REGISTER:
      return QMI_VOICE_CM_SUPS_MODE_REG;
    case  QMI_VOICE_CM_SUPS_SERVICE_ERASE:
      return QMI_VOICE_CM_SUPS_MODE_ERASURE;
    default :
      QM_MSG_ERROR_1("Invalid service/status recieved :  %d ", service);
      return QMI_VOICE_CM_SUPS_MODE_MAX;
  }
}
/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_cmd_set_call_wait_is_valid

===========================================================================*/
/*!
    @brief
    To check whether RIL_REQUEST_SET_CALL_WAITING is received with valid 
    parameters

    @return
    returns TRUE is parameters are valid else FALSE
*/
/*=========================================================================*/
boolean qmi_voice_cm_sups_cmd_set_call_wait_is_valid( int status, 
                                                      uint32 service_class)
{
  if(!((status == (int)QMI_VOICE_CM_SUPS_MODE_DISABLE) || 
     (status == (int)QMI_VOICE_CM_SUPS_MODE_ENABLE)) )
  {
    return (FALSE);
  }

  if ((qmi_voice_cm_sups_cfw_allowed_classes != ( (uint32)service_class | qmi_voice_cm_sups_cfw_allowed_classes )) )
  {
    /* since service class is optional for all call forwarding ignoring it if incorrect value is received */
    QM_MSG_HIGH("invalid service class received in set sups service call wait");
  }

  return (TRUE);
}

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_cmd_set_clip_is_valid

===========================================================================*/
/*!
    @brief
    To check whether SET_CLIP is received with valid 
    parameters

    @return
    returns TRUE is parameters are valid else FALSE
*/
/*=========================================================================*/
boolean qmi_voice_cm_sups_cmd_set_clip_is_valid( int status, 
                                                      uint32 service_class)
{
  if(!((status == (int)QMI_VOICE_CM_SUPS_MODE_DISABLE) || 
     (status == (int)QMI_VOICE_CM_SUPS_MODE_ENABLE)) )
  {
    return (FALSE);
  }

  if ((qmi_voice_cm_sups_clip_allowed_classes != ( (uint32)service_class | qmi_voice_cm_sups_clip_allowed_classes )) )
  {
    /* since service class is optional for clip ignoring it if incorrect value is received */
    QM_MSG_HIGH("invalid service class received in set sups service clip");
  }

  return (TRUE);
}

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_cmd_set_colp_is_valid

===========================================================================*/
/*!
    @brief
    To check whether SET_COLP is received with valid 
    parameters

    @return
    returns TRUE is parameters are valid else FALSE
*/
/*=========================================================================*/
boolean qmi_voice_cm_sups_cmd_set_colp_is_valid( int status, 
                                                      uint32 service_class)
{
  if(!((status == (int)QMI_VOICE_CM_SUPS_MODE_DISABLE) || 
     (status == (int)QMI_VOICE_CM_SUPS_MODE_ENABLE)) )
  {
    return (FALSE);
  }

  if ((qmi_voice_cm_sups_colp_allowed_classes != ( (uint32)service_class | qmi_voice_cm_sups_colp_allowed_classes )) )
  {
    /* since service class is optional for colp ignoring it if incorrect value is received */
    QM_MSG_HIGH("invalid service class received in set sups service colp");
  }

  return (TRUE);
}

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_cmd_set_call_fwd_is_valid

===========================================================================*/
/*!
    @brief
    To check whether RIL_REQUEST_SET_CALL_FORWARD is received with valid 
    parameters

    @return
    Returns TRUE or FALSE based on whether received parameters 
    are valid or not
*/
/*=========================================================================*/
boolean qmi_voice_cm_sups_cmd_set_call_fwd_is_valid(qmi_voice_cm_if_info_item_s *const params_ptr,
                                                                                                          qmi_voice_cm_if_set_sups_service_s *in_data_ptr)
{
  char *digit;
  uint8   forward_num[QMI_VOICE_CM_IF_MAX_CALLING_NUM_LEN];
  /*-----------------------------------------------------------------------*/
  ASSERT( in_data_ptr != NULL );
  ASSERT( params_ptr != NULL );
  /*-----------------------------------------------------------------------*/
  
  if((in_data_ptr->sups_service_info.service < (uint8)QMI_VOICE_CM_SUPS_SERVICE_ACTIVATE) || 
       (in_data_ptr->sups_service_info.service >= (uint8)QMI_VOICE_CM_SUPS_SERVICE_MAX))
    return(FALSE);

  if((in_data_ptr->sups_service_info.reason < (uint8)QMI_VOICE_CM_SUPS_CCFC_REASON_UNCOND) || 
       (in_data_ptr->sups_service_info.reason >= (uint8)QMI_VOICE_CM_SUPS_CCFC_REASON_MAX))
    return(FALSE);

  if ((in_data_ptr->service_class_valid == TRUE) && (qmi_voice_cm_sups_cfw_allowed_classes != ( (uint32)in_data_ptr->service_class | qmi_voice_cm_sups_cfw_allowed_classes )) )
  {
    /* since service class is optional for all call forwarding ignoring it if incorrect value is received */
    QM_MSG_HIGH("invalid service class received");
  } 

  memset(forward_num,0,QMI_VOICE_CM_IF_MAX_CALLING_NUM_LEN);

  if(in_data_ptr->sups_service_info.service == (uint8)QMI_VOICE_CM_SUPS_SERVICE_REGISTER)
  {
    if(!(in_data_ptr->fwd_number_valid))
    {
      QM_MSG_HIGH("Call FWD number should be present for CFW REGISTER");
      return(FALSE);
    }
  }
  if(in_data_ptr->fwd_number_valid)
  {
    if(in_data_ptr->fwd_number_len >0)
    {
      /* max number of charecters is 80, 24.008 10.5.4.7 */
      if( QMI_VOICE_CM_SUPS_CCFC_NUM_MAX_LEN < in_data_ptr->fwd_number_len)
        return(FALSE);
    
      /* verifying whether digits are valid or not */
      memscpy(forward_num, sizeof(forward_num), in_data_ptr->fwd_number, in_data_ptr->fwd_number_len);
      digit = (char *) forward_num;
      /* cm_num_fill does nothing to verify number */
      if ('+' == digit[0])
      {
        digit++;
      }
      /* per GSM 04.08, table 10.81 */
      while (*digit)
      {
        if ( ((*digit < '0') || (*digit > '9')) &&
             (*digit != '*') && (*digit != '#') && 
             (*digit != 'a') && (*digit != 'b') && (*digit != 'c') )
        {
          return FALSE;
        }       
        digit++;
      }
    }
    else
    {
      QM_MSG_HIGH("Invalid CF Number length");
    }
  }
  else
  {
    QM_MSG_MED("Received CF Number as NULL");
  }
    
/*    
  if( (in_data_ptr->sups_service_info.reason ==  (int)QMI_VOICE_CM_SUPS_CCFC_REASON_NOREPLY) || 
      (in_data_ptr->sups_service_info.reason ==  (int)QMI_VOICE_CM_SUPS_CCFC_REASON_ALLCALL) || 
      (in_data_ptr->sups_service_info.reason ==  (int)QMI_VOICE_CM_SUPS_CCFC_REASON_ALLCOND) )
  {
    if( in_data_ptr->no_reply_timer_valid )
    {
      if(!((in_data_ptr->no_reply_timer >= 5) && (in_data_ptr->no_reply_timer <= 30)))
      {
        QM_MSG_HIGH_1("invalid no_reply_timer value received = %d ", in_data_ptr->no_reply_timer);
        return(FALSE);
      }
    }
    else
    {
      QM_MSG_HIGH("no_reply_timer not received for set CFW on  - no reply");
    }
  }*/

  return(TRUE);
  
}


/*=========================================================================
  FUNCTION:  qcril_cmd_change_cb_pwd_is_valid

===========================================================================*/
/*!
    @brief
    To check whether qmi_voice_cm_if_sups_change_call_barring_pwd is received with 
    valid parameters

    @return
    Returns TRUE or FALSE based on whether received parameters are 
    valid or not
*/
/*=========================================================================*/
boolean qmi_voice_cm_sups_cmd_change_cb_pwd_is_valid(int facility , char *old_password, char *new_password)
{

  if( (facility < (int)QMI_VOICE_CM_SUPS_BARR_ALLOUTGOING) || (facility > (int)QMI_VOICE_CM_SUPS_BARR_ALLINCOMINGBARRING))
    return(FALSE);

 if(old_password == NULL)
   return(FALSE);

 if(new_password == NULL)
   return(FALSE);

 return(TRUE);
 
}


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_query_facility_lock_is_valid

===========================================================================*/
/*!
    @brief
    To check whether Get Call Barring is received with
    valid parameters

    @return
    Returns TRUE or FALSE based on whether received parameters are 
    valid or not
*/
/*=========================================================================*/

boolean qmi_voice_cm_sups_query_facility_lock_is_valid( int facility, uint32 service_class)
{

  if(
     (facility < (int)QMI_VOICE_CM_SUPS_BARR_ALLOUTGOING) || 
     (
      (facility > (int)QMI_VOICE_CM_SUPS_BARR_ALLINCOMINGBARRING) && 
      (facility != (int)QMI_VOICE_CM_SUPS_BARR_INCOMINGNUMBER) && 
      (facility != (int)QMI_VOICE_CM_SUPS_BARR_INCOMINGANONYMOUS)
     )
    )
  {
    QM_MSG_MED("Invalid Facility class received");
    return(FALSE);
  }
 /* ignoring the service class with invalid value as it is optional */
  if ( qmi_voice_cm_sups_cb_allowed_classes != ( service_class | qmi_voice_cm_sups_cb_allowed_classes ) )
    QM_MSG_MED("Invalid service class received");
  
  return(TRUE);
}

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_set_facility_lock_is_valid

===========================================================================*/
/*!
    @brief
    To check whether set sups service for Call Barring is received with
    valid parameters

    @return
    Returns TRUE or FALSE based on whether received parameters are 
    valid or not
*/
/*=========================================================================*/

boolean qmi_voice_cm_sups_set_facility_lock_is_valid( int facility, int status, uint32 service_class)
{

  if( (facility < (int)QMI_VOICE_CM_SUPS_BARR_ALLOUTGOING) || ((facility > (int)QMI_VOICE_CM_SUPS_BARR_ALLINCOMINGBARRING) && 
                                                                (facility != (int)QMI_VOICE_CM_SUPS_BARR_INCOMINGNUMBER) && 
                                                                (facility != (int)QMI_VOICE_CM_SUPS_BARR_INCOMINGANONYMOUS)))
  {
    return(FALSE);
  }

  if(
     ((facility >= (int)QMI_VOICE_CM_SUPS_BARR_ALLOUTGOING) && 
     ((facility <= (int)QMI_VOICE_CM_SUPS_BARR_ALLINCOMINGBARRING) || (facility == (int)QMI_VOICE_CM_SUPS_BARR_INCOMINGANONYMOUS))) &&
     !((status == QMI_VOICE_CM_SUPS_MODE_ENABLE) || (status == QMI_VOICE_CM_SUPS_MODE_DISABLE))
    )
  {
    return(FALSE);
  }

  if(
     (facility == (int)QMI_VOICE_CM_SUPS_BARR_INCOMINGNUMBER) &&
     !((status >= QMI_VOICE_CM_SUPS_MODE_DISABLE) || (status <= QMI_VOICE_CM_SUPS_MODE_ERASURE))
    )
  {
    return(FALSE);
  }

  /* ignoring the service class with invalid value as it is optional */
  if ( qmi_voice_cm_sups_cb_allowed_classes != ( service_class | qmi_voice_cm_sups_cb_allowed_classes ) )
  {
    QM_MSG_MED("Invalid service class received");
    return FALSE;
  }

  return(TRUE);
}


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_cfw_ss_code

===========================================================================*/
/*!
    @brief
    Returns the appropriate supplimentary services code for call forwarding
    based on the reason value received from ANDROID RIL which is as per
    spec 27.007 section 7.11

    @return
    Returns SS code for call forwarding
*/
/*=========================================================================*/
uint8  qmi_voice_cm_sups_get_cfw_ss_code(int reason, char **cf_reason_name)
{
  qmi_voice_cm_sups_operation_code_e_type cfw_ss_code = qmi_voice_cm_sups_all;
  
  switch(reason)
  {
    case QMI_VOICE_CM_SUPS_CCFC_REASON_UNCOND:
      cfw_ss_code = qmi_voice_cm_sups_cfu;
      *cf_reason_name = "CFU";
      break;
 
    case QMI_VOICE_CM_SUPS_CCFC_REASON_BUSY:
      cfw_ss_code = qmi_voice_cm_sups_cfb;
      *cf_reason_name = "CFB";
      break;
 
    case QMI_VOICE_CM_SUPS_CCFC_REASON_NOREPLY:
      cfw_ss_code = qmi_voice_cm_sups_cfnry;
      *cf_reason_name = "CFNRY";
      break;
     
    case QMI_VOICE_CM_SUPS_CCFC_REASON_NOTREACH:
      cfw_ss_code = qmi_voice_cm_sups_cfnrc;
      *cf_reason_name = "CFNRC";
      break;
     
    case QMI_VOICE_CM_SUPS_CCFC_REASON_ALLCALL:
      cfw_ss_code = qmi_voice_cm_sups_allForwardingSS;
      *cf_reason_name = "ALLCALL";
      break;
     
    case QMI_VOICE_CM_SUPS_CCFC_REASON_ALLCOND:
      cfw_ss_code = qmi_voice_cm_sups_allCondForwardingSS;    
      *cf_reason_name = "ALLCOND";
      break;
 
    default:
      *cf_reason_name = "ALLCF";
      break;
        /*none as of now as the error values are already checked before */
     
  }

  return((uint8)cfw_ss_code);
}


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_cfw_reason

===========================================================================*/
/*!
    @brief
    Returns the appropriate call forwarding reason(27.007 7.11) based
    on the supplimentary services code

    @return
    Returns call forwarding reason 
*/
/*=========================================================================*/
int  qmi_voice_cm_sups_get_cfw_reason(int ss_code)
{
  qmi_voice_cm_sups_ccfc_reason_e_type cfw_reason = QMI_VOICE_CM_SUPS_CCFC_REASON_UNCOND;
  
  switch(ss_code)
  {
    case qmi_voice_cm_sups_cfu:
      cfw_reason = QMI_VOICE_CM_SUPS_CCFC_REASON_UNCOND;
        break;
 
    case qmi_voice_cm_sups_cfb:
      cfw_reason = QMI_VOICE_CM_SUPS_CCFC_REASON_BUSY;
        break;
 
    case qmi_voice_cm_sups_cfnry:
        cfw_reason = QMI_VOICE_CM_SUPS_CCFC_REASON_NOREPLY;
        break;
     
    case qmi_voice_cm_sups_cfnrc:
        cfw_reason = QMI_VOICE_CM_SUPS_CCFC_REASON_NOTREACH;
        break;
     
    case qmi_voice_cm_sups_allForwardingSS:
        cfw_reason = QMI_VOICE_CM_SUPS_CCFC_REASON_ALLCALL;
        break;
     
    case qmi_voice_cm_sups_allCondForwardingSS :
        cfw_reason = QMI_VOICE_CM_SUPS_CCFC_REASON_ALLCOND;    
        break;
 
    default:
        break;
        /*none as of now as the error values are already checked before */
     
  }

  return((int)cfw_reason);
}


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_facility_value

===========================================================================*/
/*!
    @brief
    Converts the facility string to a numberical value

    @return
    Returns numerical value corresponding to facility string
*/
/*=========================================================================*/
int  qmi_voice_cm_sups_get_facility_value(const char * facility, char* facility_name)
{
  char temp[3];

  /*-----------------------------------------------------------------------*/
  ASSERT( facility != NULL );
  /*-----------------------------------------------------------------------*/
   
  if(strlen(facility) != sizeof(char)*2)
    return((int)QMI_VOICE_CM_SUPS_LOCK_MAX);

  memset(temp,0,sizeof(temp));
  memscpy(&temp[0], sizeof(temp), facility, sizeof(char)*2); 
  temp[2] = '\0';

  temp[0] = QMI_VOICE_CM_SUPS_UPCASE(temp[0]);
  temp[1] = QMI_VOICE_CM_SUPS_UPCASE(temp[1]);

  memscpy(facility_name, sizeof(facility_name), temp, 3); 

  if(strcmp(temp, "SC") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_SC);
  
  if(strcmp(temp, "AO") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_AO);
   
  if(strcmp(temp, "OI") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_OI);
   
  if(strcmp(temp, "OX") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_OX);

  if(strcmp(temp, "AI") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_AI);

  if(strcmp(temp, "IR") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_IR);

  if(strcmp(temp, "AB") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_AB);
  
  if(strcmp(temp, "AG") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_AG);

  if(strcmp(temp, "AC") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_AC);

  if(strcmp(temp, "FD") == FALSE)
   return((int)QMI_VOICE_CM_SUPS_LOCK_FD);  

  return((int)QMI_VOICE_CM_SUPS_LOCK_MAX);
}


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_facility_value_is_valid

===========================================================================*/
/*!
    @brief
    Checks whether the facility value is valid as per mode or not

    @return
    TRUE if the facility value is valid
*/
/*=========================================================================*/
boolean qmi_voice_cm_sups_facility_value_is_valid(int facility, int status)
{

 if( (status == FALSE) && (((int)QMI_VOICE_CM_SUPS_LOCK_AB <= facility && (int)QMI_VOICE_CM_SUPS_LOCK_AC>= facility)||
                            ((int)QMI_VOICE_CM_SUPS_LOCK_SC <= facility && (int)QMI_VOICE_CM_SUPS_LOCK_IR >= facility) ||
                            ((int)QMI_VOICE_CM_SUPS_LOCK_FD == facility)))
 {
   return(TRUE);
 }
 else if( (status == TRUE) && (((int)QMI_VOICE_CM_SUPS_LOCK_SC <= facility && (int)QMI_VOICE_CM_SUPS_LOCK_IR >= facility) ||
                                ((int)QMI_VOICE_CM_SUPS_LOCK_FD == facility)))  
 {
   return(TRUE);
 }
 else
 {
  return(FALSE);
 }  
}

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_cb_ss_code

===========================================================================*/
/*!
    @brief
    Returns the appropriate supplimentary services code for call barring 
    supplimentary services based on the facility value received from Control Point 
    which is as per QMI spec

    @return
    Returns SS code for call barring
*/
/*=========================================================================*/
int  qmi_voice_cm_sups_get_cb_ss_code(int facility)
{
  qmi_voice_cm_sups_operation_code_e_type cb_ss_code = qmi_voice_cm_sups_all;
  
  switch(facility)
  {
    case QMI_VOICE_CM_SUPS_BARR_ALLOUTGOING:
      cb_ss_code = qmi_voice_cm_sups_baoc;
      break;
 
    case QMI_VOICE_CM_SUPS_BARR_OUTGOINGINT:
      cb_ss_code = qmi_voice_cm_sups_boic;
      break;
 
    case QMI_VOICE_CM_SUPS_BARR_OUTGOINGINTEXTOHOME:
      cb_ss_code = qmi_voice_cm_sups_boicExHC;
      break;
   
    case QMI_VOICE_CM_SUPS_BARR_ALLINCOMING:
      cb_ss_code = qmi_voice_cm_sups_baic;
      break;
   
    case QMI_VOICE_CM_SUPS_BARR_INCOMINGROAMING:
      cb_ss_code = qmi_voice_cm_sups_bicRoam;
      break;
   
    case QMI_VOICE_CM_SUPS_BARR_ALLBARRING:
      cb_ss_code = qmi_voice_cm_sups_allCallRestrictionSS;  
      break;

     case QMI_VOICE_CM_SUPS_BARR_ALLOUTGOINGBARRING:
      cb_ss_code = qmi_voice_cm_sups_barringOfOutgoingCalls;
      break;
   
    case QMI_VOICE_CM_SUPS_BARR_ALLINCOMINGBARRING:
      cb_ss_code = qmi_voice_cm_sups_barringOfIncomingCalls;
      break;
      
    case QMI_VOICE_CM_SUPS_BARR_INCOMINGNUMBER:
      cb_ss_code = qmi_voice_cm_sups_bil;
      break;

    case QMI_VOICE_CM_SUPS_BARR_INCOMINGANONYMOUS:
      cb_ss_code = qmi_voice_cm_sups_bin;
      break;
      
    default:
      QM_MSG_MED_1("received invalid facility in get_call_barring :  %d", facility);
      break;
      /*none as of now as the error values are already checked before */
   
  }

  return((int)cb_ss_code);
}


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_service_class

===========================================================================*/
/*!
    @brief
    finds the appropriate service class that matches the input input 
    basic service group and type

    INPUT:        basic service type and basic service code   
    
    @return
    Returns service class
*/
/*=========================================================================*/
uint32 qmi_voice_cm_sups_get_service_class(bsg_code_type bsg_type, int bsg_code)
{
  qmi_voice_cm_sups_class_e_type bs_class = QMI_VOICE_CM_SUPS_CLASS_MAX;
  uint8 index = 0;
  uint8 total_sups_bs_table_size=0;
  uint8 total_sups_extra_bs_table_size=0;

  total_sups_bs_table_size = sizeof(qmi_voice_cm_sups_bs_mapping_table)/sizeof(qmi_voice_cm_sups_bs_mapping_s_type);
  total_sups_extra_bs_table_size = sizeof(qmi_voice_cm_sups_extra_bs_mapping_table)/sizeof(qmi_voice_cm_sups_bs_mapping_s_type);
  /* Lookup for Class parameter */
  while ((index < total_sups_bs_table_size) && (QMI_VOICE_CM_SUPS_CLASS_MAX != qmi_voice_cm_sups_bs_mapping_table[index].bs_class) )
  {
    if ((qmi_voice_cm_sups_bs_mapping_table[index].bs_code == (uint8)bsg_code) &&
        (qmi_voice_cm_sups_bs_mapping_table[index].bs_type == (uint8)bsg_type))
    {
      bs_class = qmi_voice_cm_sups_bs_mapping_table[index].bs_class;
      break;
    }
    index++;
  }

  if ( QMI_VOICE_CM_SUPS_CLASS_MAX == bs_class )
  {
    index = 0;
    /* Lookup BSG extra mapping table for Class parameter */
    while ( (index < total_sups_extra_bs_table_size) && (QMI_VOICE_CM_SUPS_CLASS_MAX != qmi_voice_cm_sups_extra_bs_mapping_table[index].bs_class))
    {
      if ( ( qmi_voice_cm_sups_extra_bs_mapping_table[index].bs_code == (uint8)bsg_code ) &&
           ( qmi_voice_cm_sups_extra_bs_mapping_table[index].bs_type == (uint8)bsg_type ) )
      {
        bs_class = qmi_voice_cm_sups_extra_bs_mapping_table[index].bs_class;
        break;
      }
      index++;
    }
  }

  if (QMI_VOICE_CM_SUPS_CLASS_MAX == bs_class)
  {
    QM_MSG_ERROR_2("Could not decode BSG type,code:  %d,%d ", bsg_type, bsg_code);
    bs_class = QMI_VOICE_CM_SUPS_CLASS_MIN;
  }
  else
  {
    QM_MSG_ERROR_3("BSG type=%d, code=%d and class=%d", bsg_type, bsg_code, bs_class);
  }

  return (uint32)bs_class;
}

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_set_time_info

===========================================================================*/
/*!
    @brief
    sets appropriate time info

    INPUT:   source time info, destination time info

    @return
    IxErrnoType  Result of setting the time info.
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cm_sups_set_time_info
(
  cm_ip_sups_call_fwd_time_s_type source_time_info,
  cm_ip_sups_call_fwd_time_s_type *dest_time_info_ptr
)
{
  IxErrnoType status = E_SUCCESS;

  /* Validate Minute info*/
  if (source_time_info.minute > QMI_VOICE_CM_SUPS_TIME_MINUTE_MAX)
  {
    QM_MSG_ERROR_1("Invalid QMI_Voice Minute info: %d", source_time_info.minute);
    status = E_FAILURE;
    goto Done;
  }
  /* Validate Second info*/
  if (source_time_info.second > QMI_VOICE_CM_SUPS_TIME_SECOND_MAX)
  {
    QM_MSG_ERROR_1("Invalid QMI_Voice Second info: %d", source_time_info.second);
    status = E_FAILURE;
    goto Done;
  }

  /* All Time data are valid, fill the destination time info */
    QM_MSG_HIGH_7("set_time_info: Year %d, Month %d, Day %d, Hour %d, Minute %d, Second %d, Time Zone %d",
                  source_time_info.year, source_time_info.month,
                  source_time_info.day, source_time_info.hour,
                  source_time_info.minute, source_time_info.second,
                 source_time_info.time_zone);

  dest_time_info_ptr->year = source_time_info.year; /* We do not validate year info */
  dest_time_info_ptr->month = source_time_info.month;
  dest_time_info_ptr->day = source_time_info.day;
  dest_time_info_ptr->hour = source_time_info.hour;
  dest_time_info_ptr->minute = source_time_info.minute;
  dest_time_info_ptr->second = source_time_info.second;
  dest_time_info_ptr->time_zone = source_time_info.time_zone; /* We do not validate time_zone info */

Done:
  return status;
 
} /* end of qmi_voice_cm_sups_set_time_info */

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_set_bsg_info

===========================================================================*/
/*!
    @brief
    finds and sets appropriate bsg type and bsg code that matches the received service class

    INPUT:   service class

    @return
    Returns none
*/
/*=========================================================================*/
void qmi_voice_cm_sups_set_bsg_info(uint32 service_class, basic_service_s_type *bsg_info)
{

  uint8 index = 0;

 if (service_class < QMI_VOICE_PLMN_SPECIFIC_TS_ALL)
 {
  while ( QMI_VOICE_CM_SUPS_CLASS_MAX != qmi_voice_cm_sups_bs_mapping_table[index].bs_class)
  {
    /* This same comparison will handle aggregate or single class */
    if ( ( service_class & (uint32) qmi_voice_cm_sups_bs_mapping_table[index].bs_class ) ==
         (uint32) qmi_voice_cm_sups_bs_mapping_table[index].bs_class )
    {
      bsg_info->code_type = (bsg_code_type) qmi_voice_cm_sups_bs_mapping_table[index].bs_type;
      bsg_info->bs_code = qmi_voice_cm_sups_bs_mapping_table[index].bs_code;
      bsg_info->present = TRUE;
      QM_MSG_MED_5("SC Mapping: code_type=%d, bs_code=%d (hex is %#x),for srv_class=%d (hex is %#x)",
                   (uint8)bsg_info->code_type, bsg_info->bs_code,
                   bsg_info->bs_code, service_class, service_class);
      break;
    }
    index++;
  }
 }
 else
 {
  while ( QMI_VOICE_CM_SUPS_CLASS_MAX != qmi_voice_cm_sups_bs_mapping_table[index].bs_class)
  {
    /* This same comparison will handle aggregate or single class */
    if ( service_class == (uint32) qmi_voice_cm_sups_bs_mapping_table[index].bs_class )
    {
      bsg_info->code_type = (bsg_code_type) qmi_voice_cm_sups_bs_mapping_table[index].bs_type;
      bsg_info->bs_code = qmi_voice_cm_sups_bs_mapping_table[index].bs_code;
      bsg_info->present = TRUE;
      QM_MSG_MED_5("SC Mapping: code_type=%d, bs_code=%d (hex is %#x),for srv_class=%d (hex is %#x)",
                   (uint8)bsg_info->code_type, bsg_info->bs_code,
                   bsg_info->bs_code, service_class, service_class);
      break;
    }
    index++;
  }
 }
 
} /* end of qmi_voice_cm_sups_set_bsg_info */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_write_token

===========================================================================*/
/*!
    @brief
    Writes the Sups token to the dest character buffer.

    INPUT:    Sups string tokens one at a time
    
    @return
    IxErrnoType Result of Sups construction
    (Also stores the Sups MMI characters in dest buffer)
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cm_sups_write_token
(
  char *dest,
  char *src, 
  int *idx, 
  int max_dest_len
)
{
  int src_len = 0;

  /* check if we have valid input */
  if((NULL == src) || ('\0' == *src) || (NULL == dest) || (NULL == idx) )
  {
    return E_NO_DATA;
  }

  src_len = strlen(src);
  /* dest buffer bounds check */
  if((*idx + src_len) >= max_dest_len )
  {
    QM_MSG_ERROR("dest buffer full!");
    return E_OUT_OF_RANGE;
  }

  /* Copy the Sups token to the destination buffer */
  if(strlcpy(&dest[*idx], src, src_len+1) >= (src_len+1))
  {
    QM_MSG_ERROR("std_strlcpy failed");
    return E_FAILURE;
  }
  *idx += src_len;
  return E_SUCCESS;
}/* qmi_voice_cm_sups_write_token */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_write_sups_tokens

===========================================================================*/
/*!
    @brief
    Build the Sups string from the sups tokens.
    One of **, ##, *#, *, #, **03* is say Sups Type (ST)
    Service Code is (SC).
    Supplementary information (SI) is made of SIA, SIB, SIC
    Sups String constructed will be one among the many forms given below
    STSC*SIA*SIB*SIC#
    STSC*SIA*SIB#
    STSC*SIA**SIC#
    STSC*SIA#
    STSC**SIB*SIC#
    STSC**SIB#
    STSC***SIC#
    STSC#

    INPUT:    Sups string in tokenized form
              ss_tokens[0]  is  ST
              ss_tokens[1]  is  SC
              ss_tokens[2]  is  SIA
              ss_tokens[3]  is  SIB
              ss_tokens[4]  is  SIC

    @return
    IxErrnoType  Result of Sups construction
    (Also buf will have the final constructed sups string)
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cm_sups_write_sups_tokens
(
  char *ss_tokens[], 
  char *buffer, 
  int max_buf_len
)
{
  int i = 0;
  int max_len = max_buf_len - 2;
  boolean is_sia_present = FALSE, is_sib_present = FALSE, is_sic_present = FALSE;

  if( NULL == buffer )
  {
    QM_MSG_ERROR("Input buffer is NULL");
    return E_FAILURE;
  }

  /* Write the mode type string. This is mandatory */
  if(E_SUCCESS != qmi_voice_cm_sups_write_token(buffer, ss_tokens[0], &i, max_len) )
  {
    return E_FAILURE;
  }

  /* Write the Service Code(SC). SC is not mandatory in all cases. */
  if(E_OUT_OF_RANGE == qmi_voice_cm_sups_write_token( buffer, ss_tokens[1], &i, max_len ))
  {
    return E_FAILURE;
  }

  is_sia_present = ( (ss_tokens[2] != NULL) && (*ss_tokens[2] != '\0') );
  is_sib_present = ( (ss_tokens[3] != NULL) && (*ss_tokens[3] != '\0') );
  is_sic_present = ( (ss_tokens[4] != NULL) && (*ss_tokens[4] != '\0') );

  if( is_sia_present )
  {
    buffer[i++] = '*';
    /* Write SIA */
    if(E_SUCCESS != qmi_voice_cm_sups_write_token( buffer, ss_tokens[2], &i, max_len ))
    {
      return E_FAILURE;
    }
  }
  else
  {
    if( is_sib_present || is_sic_present )
    {
      /* Extra * for not having SIA. Check function description 
      for more information */
      buffer[i++] = '*';
    }
  }

  if( is_sib_present )
  {
    buffer[i++] = '*';
    /* Write SIB */
    if(E_SUCCESS != qmi_voice_cm_sups_write_token( buffer, ss_tokens[3], &i, max_len ))
    {
      return E_FAILURE;
    }
  }
  else
  {
    if(is_sic_present)
    {
      /* Extra * for not having SIB */
      buffer[i++] = '*';
    }
  }

  if(is_sic_present)
  {
    buffer[i++] = '*';
    /* Write SIC */
    if(E_SUCCESS != qmi_voice_cm_sups_write_token( buffer, ss_tokens[4], &i, max_len ))
    {
      return E_FAILURE;
    }
  }

  buffer[i++] = '#';
  buffer[i] = '\0';
  return E_SUCCESS;
}/* qmi_voice_cm_sups_write_sups_tokens */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_build_sups_string

===========================================================================*/
/*!
    @brief
    Prepares the Sups tokens and constructs the Sups string.

    INPUT:    Sups information required to construct the string
    
    @return
    IxErrnoType  Result of the sups construction.
    (Also stores the constructed Sups string in buf)
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cm_sups_build_sups_string
(
  qmi_voice_cm_sups_params_s_type *ss_params,
  char *buffer,
  int max_len
)
{
  char *ss_tokens[5];
  char *ss_bsg = NULL;
  char ss_nr_timer[QMI_VOICE_CM_SUPS_MAX_INT_DIGITS+1];  /* include nul terminating char */
  uint8 i = 0;
  qmi_voice_cm_sups_mode_table_s_type *curr_ss_mode = qmi_voice_cm_sups_mode_input;

  if( (NULL == buffer) || (NULL == ss_params) )
  {
    QM_MSG_ERROR_1( "%s ", "Invalid Input parameters");
    return E_FAILURE;
  }

  if( qmi_voice_cm_sups_bil == ss_params->code || qmi_voice_cm_sups_bin == ss_params->code)
  {
    buffer[0] = '\0';
    return E_SUCCESS;
  }

  /* Derive the sups type string */
  while( curr_ss_mode->sups_mode_str != NULL )
  {
    if( curr_ss_mode->sups_mode == ss_params->mode )
    {
      /* Store mode type */
      ss_tokens[0] = curr_ss_mode->sups_mode_str;
      break;
    }
    curr_ss_mode++;
  }

  /* Derive the MMI Service Code */
  ss_tokens[1] = NULL;
  for (i = 0; i < QMI_VOICE_CM_SUPS_MAX_SC_ENTRY; i++)
  {
    if (ss_params->code == qmi_voice_cm_sups_sc_conversion_table[i].net_sc) 
    {
      /* Store service code in [1] */
      ss_tokens[1] = qmi_voice_cm_sups_sc_conversion_table[i].mmi_sc;
      break;
    }
  }

  /* check if sups request is for change password */
  if(QMI_VOICE_CM_SUPS_MODE_REG_PASSWD == ss_params->mode)
  {
    /* Copy the password in the tokens */
    ss_tokens[2] = ss_params->cmd.passwd.old_passwd;
    ss_tokens[3] = ss_params->cmd.passwd.new_passwd;
    ss_tokens[4] = ss_params->cmd.passwd.new_passwd_again;
    return qmi_voice_cm_sups_write_sups_tokens( ss_tokens, buffer, max_len );
  }

  /* Derive the Basic Service Group MMI from the Service class */
  QM_MSG_MED_1("Deriving Basic Service Group MMI from Service class, MAX-BSG-ENTRY=%d", QMI_VOICE_CM_SUPS_MAX_BSG_ENTRY);
  for (i = 0; i < QMI_VOICE_CM_SUPS_MAX_BSG_ENTRY; i++) 
  {
    if (ss_params->service_class == qmi_voice_cm_sups_bsg_conversion_table[i].service_class) 
    {
      ss_bsg = qmi_voice_cm_sups_bsg_conversion_table[i].mmi_bsg;
      break;
    }
  }

  if( (ss_params->code & 0xF0) == qmi_voice_cm_sups_allForwardingSS )
  {
    /* Here SIA will be Directory number */
    /* Store SIA */
    ss_tokens[2] = ss_params->cmd.reg.number;
    /* In case of Call forwarding, BSG will be in SIB */
    /* Store SIB */
    ss_tokens[3] = ss_bsg;
    ss_tokens[4] = NULL;
  }
  else if((ss_params->code & 0xF0) == qmi_voice_cm_sups_allCallRestrictionSS)
  {
    ss_tokens[2] = ss_params->cmd.passwd.old_passwd;
    /* In case of Call Barring, BSG will be in SIB */
    /* Store SIB in [3]. SIC will not present */
    ss_tokens[3] = ss_bsg;
    ss_tokens[4] = NULL;
  }
  else if( (qmi_voice_cm_sups_cw == ss_params->code)|| 
           (qmi_voice_cm_sups_clip == ss_params->code) || 
           (qmi_voice_cm_sups_colp == ss_params->code))
  {
    /* In case of Call Waiting, BSG will be in SIA */
    /* Store SIA.  SIB, SIC are not applicable */
    ss_tokens[2] = ss_bsg;
    ss_tokens[3] = NULL;
    ss_tokens[4] = NULL;
  }
  else
  {
    /* SIA, SIB, SIC will not be present*/
    ss_tokens[2] = NULL;
    ss_tokens[3] = NULL;
    ss_tokens[4] = NULL;
  }

  if ( QMI_VOICE_CM_SUPS_MODE_REG == ss_params->mode && 
      ((ss_params->code & 0xF0) == qmi_voice_cm_sups_allForwardingSS) ) 
  {
    if (0 != ss_params->cmd.reg.nr_timer) 
    {
      (void)QMI_VOICE_CM_IF_STR_PRINTF( ss_nr_timer, QMI_VOICE_CM_SUPS_MAX_INT_DIGITS+1, "%d", ss_params->cmd.reg.nr_timer );
      ss_tokens[4] = ss_nr_timer;
    }
  }
  return qmi_voice_cm_sups_write_sups_tokens( ss_tokens, buffer, max_len );
}/* qmi_voice_cm_sups_build_sups_string */

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_map_cc_cause_to_end_reason

===========================================================================*/
/*!
    @brief
    Maps the Network call control causes to corresponding QMI Failure causes

    INPUT:    cc cause

    @return
    QMI Failure causes  
    Reference: QMI spec Table A.1
*/
/*=========================================================================*/
qmi_voice_cm_if_failure_cause_e_type qmi_voice_cm_sups_map_cc_cause_to_failure_cause
(
  int  cc_cause
)
{
  qmi_voice_cm_if_failure_cause_e_type  failure_cause = QMI_FAILURE_CAUSE_REL_NORMAL;
  uint16 i=0;
  uint16 total_size_of_cause_table =0;
  /* Cause Value to Failure causes Mapping*/

  qmi_voice_cm_sups_cc_cause_to_failure_cause_table_s_type qmi_voice_cm_sups_failure_cause_conv_table[] =
  {
    {1,          QMI_FAILURE_CAUSE_UNASSIGNED_NUMBER},
    {3,          QMI_FAILURE_CAUSE_NO_ROUTE_TO_DESTINATION},
    {6,          QMI_FAILURE_CAUSE_CHANNEL_UNACCEPTABLE},
    {8,          QMI_FAILURE_CAUSE_OPERATOR_DETERMINED_BARRING },
    {16,         QMI_FAILURE_CAUSE_NORMAL_CALL_CLEARING},
    {17,         QMI_FAILURE_CAUSE_USER_BUSY},
    {18,         QMI_FAILURE_CAUSE_NO_USER_RESPONDING},
    {19,         QMI_FAILURE_CAUSE_USER_ALERTING_NO_ANSWER},
    {21,         QMI_FAILURE_CAUSE_CALL_REJECTED},
    {22,         QMI_FAILURE_CAUSE_NUMBER_CHANGED},
    {25,         QMI_FAILURE_CAUSE_PREEMPTION},
    {27,         QMI_FAILURE_CAUSE_DESTINATION_OUT_OF_ORDER},
    {28,         QMI_FAILURE_CAUSE_INVALID_NUMBER_FORMAT},
    {29,         QMI_FAILURE_CAUSE_FACILITY_REJECTED},
    {30,         QMI_FAILURE_CAUSE_RESP_TO_STATUS_ENQUIRY},
    {31,         QMI_FAILURE_CAUSE_NORMAL_UNSPECIFIED},
    {34,         QMI_FAILURE_CAUSE_NO_CIRCUIT_OR_CHANNEL_AVAILABLE},
    {38,         QMI_FAILURE_CAUSE_NETWORK_OUT_OF_ORDER},
    {41,         QMI_FAILURE_CAUSE_TEMPORARY_FAILURE},
    {42,         QMI_FAILURE_CAUSE_SWITCHING_EQUIPMENT_CONGESTION},
    {43,         QMI_FAILURE_CAUSE_ACCESS_INFORMATION_DISCARDED},
    {44,         QMI_FAILURE_CAUSE_REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE},
    {47,         QMI_FAILURE_CAUSE_RESOURCES_UNAVAILABLE_OR_UNSPECIFIED},
    {49,         QMI_FAILURE_CAUSE_QOS_UNAVAILABLE},
    {50,         QMI_FAILURE_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED},
    {55,         QMI_FAILURE_CAUSE_INCOMING_CALLS_BARRED_WITHIN_CUG},
    {57,         QMI_FAILURE_CAUSE_BEARER_CAPABILITY_NOT_AUTH},
    {58,         QMI_FAILURE_CAUSE_BEARER_CAPABILITY_UNAVAILABLE},
    {63,         QMI_FAILURE_CAUSE_SERVICE_OPTION_NOT_AVAILABLE},
    {65,         QMI_FAILURE_CAUSE_BEARER_SERVICE_NOT_IMPLEMENTED},
    {68,         QMI_FAILURE_CAUSE_ACM_LIMIT_EXCEEDED},
    {69,         QMI_FAILURE_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED },
    {70,         QMI_FAILURE_CAUSE_ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE},
    {79,         QMI_FAILURE_CAUSE_SERVICE_OR_OPTION_NOT_IMPLEMENTED },
    {81,         QMI_FAILURE_CAUSE_INVALID_TRANSACTION_IDENTIFIER},
    {87,         QMI_FAILURE_CAUSE_USER_NOT_MEMBER_OF_CUG},
    {88,         QMI_FAILURE_CAUSE_INCOMPATIBLE_DESTINATION},
    {91,         QMI_FAILURE_CAUSE_INVALID_TRANSIT_NW_SELECTION},
    {95,         QMI_FAILURE_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE},
    {96,         QMI_FAILURE_CAUSE_INVALID_MANDATORY_INFORMATION},
    {97,         QMI_FAILURE_CAUSE_MESSAGE_TYPE_NON_IMPLEMENTED},
    {98,         QMI_FAILURE_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE},
    {99,         QMI_FAILURE_CAUSE_INFORMATION_ELEMENT_NON_EXISTENT},
    {100,       QMI_FAILURE_CAUSE_CONDITONAL_IE_ERROR},
    {101,       QMI_FAILURE_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE},
    {102,       QMI_FAILURE_CAUSE_RECOVERY_ON_TIMER_EXPIRED},
    {111,       QMI_FAILURE_CAUSE_PROTOCOL_ERROR_UNSPECIFIED},
    {127,       QMI_FAILURE_CAUSE_INTERWORKING_UNSPECIFIED},
    {129,       QMI_FAILURE_CAUSE_OUTGOING_CALLS_BARRED_WITHIN_CUG},
    {130,       QMI_FAILURE_CAUSE_NO_CUG_SELECTION},
    {131,       QMI_FAILURE_CAUSE_UNKNOWN_CUG_INDEX},
    {132,       QMI_FAILURE_CAUSE_CUG_INDEX_INCOMPATIBLE},
    {133,       QMI_FAILURE_CAUSE_CUG_CALL_FAILURE_UNSPECIFIED},
    {134,       QMI_FAILURE_CAUSE_CLIR_NOT_SUBSCRIBED},
    {135,       QMI_FAILURE_CAUSE_CCBS_POSSIBLE},
    {136,       QMI_FAILURE_CAUSE_CCBS_NOT_POSSIBLE},
    {161,       QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJ_RR_REL_IND},
    {162,       QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJ_RR_RANDOM_ACCESS_FAILURE},
    {163,       QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJ_RRC_REL_IND},
    {164,       QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJ_RRC_CLOSE_SESSION_IND},
    {165,       QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJ_RRC_OPEN_SESSION_FAILURE},
    {166,       QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJ_LOW_LEVEL_FAIL},
    {167,       QMI_FAILURE_CAUSE_ACCESS_STRATUM_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED},
    {168,       QMI_FAILURE_CAUSE_INVALID_SIM},
    {169,       QMI_FAILURE_CAUSE_NO_SRV},
    {170,       QMI_FAILURE_CAUSE_TIMER_T3230_EXPIRED},
    {171,       QMI_FAILURE_CAUSE_NO_CELL_AVAILABLE},
    {172,       QMI_FAILURE_CAUSE_WRONG_STATE},
    {173,       QMI_FAILURE_CAUSE_ACCESS_CLASS_BLOCKED},
    {174,       QMI_FAILURE_CAUSE_ABORT_MSG_RECEIVED},
    {175,       QMI_FAILURE_CAUSE_RADIO_LINK_LOST},
    {176,       QMI_FAILURE_CAUSE_TIMER_T303_EXPIRED},
    {177,       QMI_FAILURE_CAUSE_NO_RESOURCES},
    {178,       QMI_FAILURE_CAUSE_CNM_MM_REL_PENDING},
    {179,       QMI_FAILURE_CAUSE_INVALID_USER_DATA},
    {213,       QMI_FAILURE_CAUSE_FADE}
  };

  total_size_of_cause_table = sizeof(qmi_voice_cm_sups_failure_cause_conv_table)/sizeof(qmi_voice_cm_sups_cc_cause_to_failure_cause_table_s_type);

  while(i < total_size_of_cause_table)
  {
    if(cc_cause == qmi_voice_cm_sups_failure_cause_conv_table[i].cause)
    {
      failure_cause = qmi_voice_cm_sups_failure_cause_conv_table[i].reason;
      break;
    }
    i++;
  }
  QM_MSG_HIGH_2("cc_cause = %d mapped to failure_cause %d", cc_cause, failure_cause);
  return failure_cause;
}/* qmi_voice_cm_sups_map_cc_cause_to_failure_cause() */

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_map_ss_errors_to_failure_cause

===========================================================================*/
/*!
    @brief
    Maps the Network call control causes to corresponding QMI Failure causes

    INPUT:    cc cause

    @return
    QMI Failure causes  
    Reference: QMI spec Table A.1
*/
/*=========================================================================*/
qmi_voice_cm_if_failure_cause_e_type qmi_voice_cm_sups_map_ss_errors_to_failure_cause
(
  int  ss_error
)
{
  qmi_voice_cm_if_failure_cause_e_type  failure_cause = QMI_FAILURE_CAUSE_REL_NORMAL;
  uint16 i=0;
  uint16 total_size_of_cause_table=0;
  /* Cause Value to Failure causes Mapping*/
  qmi_voice_cm_sups_ss_error_to_failure_cause_table_s_type qmi_voice_cm_sups_failure_cause_conv_table[] =
  {
    {1,          QMI_FAILURE_CAUSE_UNKNOWN_SUBSCRIBER},
    {9,          QMI_FAILURE_CAUSE_ILLEGAL_SUBSCRIBER},
    {10,          QMI_FAILURE_CAUSE_BEARER_SERVICE_NOT_PROVISIONED},
    {11,          QMI_FAILURE_CAUSE_TELE_SERVICE_NOT_PROVISIONED},
    {12,          QMI_FAILURE_CAUSE_ILLEGAL_EQUIPMENT},
    {13,          QMI_FAILURE_CAUSE_CALL_BARRED},
    {16,          QMI_FAILURE_CAUSE_ILLEGAL_SS_OPERATION},
    {17,          QMI_FAILURE_CAUSE_SS_ERROR_STATUS},
    {18,          QMI_FAILURE_CAUSE_SS_NOT_AVAILABLE},
    {19,          QMI_FAILURE_CAUSE_SS_SUBSCRIPTION_VIOLATION},
    {20,          QMI_FAILURE_CAUSE_SS_INCOMPATIBILITY},
    {21,          QMI_FAILURE_CAUSE_FACILITY_NOT_SUPPORTED},
    {27,          QMI_FAILURE_CAUSE_ABSENT_SUBSCRIBER},
    {29,          QMI_FAILURE_CAUSE_SHORT_TERM_DENIAL},
    {30,          QMI_FAILURE_CAUSE_LONG_TERM_DENIAL},
    {34,          QMI_FAILURE_CAUSE_SYSTEM_FAILURE},
    {35,          QMI_FAILURE_CAUSE_DATA_MISSING},
    {36,          QMI_FAILURE_CAUSE_UNEXPECTED_DATA_VALUE},
    {37,          QMI_FAILURE_CAUSE_PWD_REGISTRATION_FAILURE},
    {38,          QMI_FAILURE_CAUSE_NEGATIVE_PWD_CHECK},
    {43,          QMI_FAILURE_CAUSE_NUM_OF_PWD_ATTEMPTS_VIOLATION},
    {54,          QMI_FAILURE_CAUSE_POSITION_METHOD_FAILURE},
    {71,          QMI_FAILURE_CAUSE_UNKNOWN_ALPHABET},
    {72,          QMI_FAILURE_CAUSE_USSD_BUSY},
    {121,         QMI_FAILURE_CAUSE_REJECTED_BY_USER},
    {122,         QMI_FAILURE_CAUSE_REJECTED_BY_NETWORK},
    {123,         QMI_FAILURE_CAUSE_DEFLECTION_TO_SERVED_SUBSCRIBER},
    {124,         QMI_FAILURE_CAUSE_SPECIAL_SERVICE_CODE},
    {125,         QMI_FAILURE_CAUSE_INVALID_DEFLECTED_TO_NUMBER},
    {126,         QMI_FAILURE_CAUSE_MPTY_PARTICIPANTS_EXCEEDED},
    {127,         QMI_FAILURE_CAUSE_RESOURCES_NOT_AVAILABLE}
  };

  total_size_of_cause_table = sizeof(qmi_voice_cm_sups_failure_cause_conv_table)/sizeof(qmi_voice_cm_sups_ss_error_to_failure_cause_table_s_type);

  while(i < total_size_of_cause_table)
  {
    if(ss_error == qmi_voice_cm_sups_failure_cause_conv_table[i].ss_error)
    {
      failure_cause = qmi_voice_cm_sups_failure_cause_conv_table[i].reason;
      break;
    }
    i++;
  }

  QM_MSG_HIGH_2("ss_error = %d mapped to QMI failure cause = %d", ss_error, failure_cause);
  return failure_cause;
}/* qmi_voice_cm_sups_map_ss_errors_to_failure_cause() */

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_reason_from_ss_code

===========================================================================*/
/*!
    @brief
    

    @return
    Returns Reason code for sups ind
*/
/*=========================================================================*/
qmi_voice_cm_sups_reason_code_e_type  qmi_voice_cm_sups_get_reason_from_ss_code(uint8 ss_code)
{
  qmi_voice_cm_sups_reason_code_e_type reason = QMI_VOICE_CM_SUPS_REASON_NONE;
  
  switch((qmi_voice_cm_sups_operation_code_e_type)ss_code)
  {
    case qmi_voice_cm_sups_cfu:
      reason = QMI_VOICE_CM_SUPS_FWD_UNCONDITIONAL;
      break;
 
    case qmi_voice_cm_sups_cfb:
      reason = QMI_VOICE_CM_SUPS_FWD_MOBILEBUSY;
      break;
 
    case qmi_voice_cm_sups_cfnry:
      reason = QMI_VOICE_CM_SUPS_FWD_NOREPLY;
      break;
     
    case qmi_voice_cm_sups_cfnrc:
      reason = QMI_VOICE_CM_SUPS_FWD_UNREACHABLE;
      break;
     
    case qmi_voice_cm_sups_allForwardingSS:
      reason = QMI_VOICE_CM_SUPS_FWD_ALLFORWARDING;
      break;
     
    case qmi_voice_cm_sups_allCondForwardingSS:
      reason = QMI_VOICE_CM_SUPS_FWD_ALLCONDITIONAL;
      break;
  
    case qmi_voice_cm_sups_baoc:
      reason = QMI_VOICE_CM_SUPS_BARR_ALLOUTGOING;
      break;
 
    case qmi_voice_cm_sups_boic:
      reason = QMI_VOICE_CM_SUPS_BARR_OUTGOINGINT;
      break;
 
    case qmi_voice_cm_sups_boicExHC:
      reason = QMI_VOICE_CM_SUPS_BARR_OUTGOINGINTEXTOHOME;
      break;
   
    case qmi_voice_cm_sups_baic:
      reason = QMI_VOICE_CM_SUPS_BARR_ALLINCOMING;
      break;
   
    case qmi_voice_cm_sups_bicRoam:
      reason = QMI_VOICE_CM_SUPS_BARR_INCOMINGROAMING;
      break;
   
    case qmi_voice_cm_sups_bil:
      reason = QMI_VOICE_CM_SUPS_BARR_INCOMINGNUMBER;
      break;
   
    case qmi_voice_cm_sups_bin:
      reason = QMI_VOICE_CM_SUPS_BARR_INCOMINGANONYMOUS;
      break;
   
    case qmi_voice_cm_sups_allCallRestrictionSS:
      reason = QMI_VOICE_CM_SUPS_BARR_ALLBARRING;  
      break;

     case qmi_voice_cm_sups_barringOfOutgoingCalls:
      reason = QMI_VOICE_CM_SUPS_BARR_ALLOUTGOINGBARRING;
      break;
   
    case qmi_voice_cm_sups_barringOfIncomingCalls:
      reason = QMI_VOICE_CM_SUPS_BARR_ALLINCOMINGBARRING;
      break;

    case qmi_voice_cm_sups_cw:
      reason = QMI_VOICE_CM_SUPS_CALLWAITING;
      break;

     case qmi_voice_cm_sups_clip:
      reason = QMI_VOICE_CM_SUPS_CLIP;
      break;
   
    case qmi_voice_cm_sups_clir:
      reason = QMI_VOICE_CM_SUPS_CLIR;
      break;
      
    case qmi_voice_cm_sups_colp:
      reason = QMI_VOICE_CM_SUPS_COLP;
      break;

    case qmi_voice_cm_sups_colr:
      reason = QMI_VOICE_CM_SUPS_COLR;
      break;

    case qmi_voice_cm_sups_cnap:
	  reason = QMI_VOICE_CM_SUPS_CNAP;
	  break;
	  
    default:
      QM_MSG_HIGH_1("received invalid ss_code in qmi_voice_cm_sups_get_reason_from_ss_code :  %d", ss_code);
      break;
      /*none as of now as the error values are already checked before */
   
  }

  return(reason);
}

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_service_type_from_event

===========================================================================*/
/*!
    @brief
    

    @return
    Returns Service type for sups indication.
*/
/*=========================================================================*/
qmi_voice_cm_sups_service_type_e_type  qmi_voice_cm_sups_get_service_type_from_event(qmi_voice_cm_if_evt_e_type event)
{
  qmi_voice_cm_sups_service_type_e_type service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_INVALID;

  switch(event)
  {
    case QMI_VOICE_CM_IF_EVT_SUPS_REGISTER:
    case QMI_VOICE_CM_IF_EVT_SUPS_REGISTER_CONF:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_REGISTER;
      break;

    case QMI_VOICE_CM_IF_EVT_SUPS_ERASE:
    case QMI_VOICE_CM_IF_EVT_SUPS_ERASE_CONF:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_ERASE;
      break;

    case QMI_VOICE_CM_IF_EVT_SUPS_ACTIVATE:
    case QMI_VOICE_CM_IF_EVT_SUPS_ACTIVATE_CONF:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_ACTIVATE;
      break;

    case QMI_VOICE_CM_IF_EVT_SUPS_DEACTIVATE:
    case QMI_VOICE_CM_IF_EVT_SUPS_DEACTIVATE_CONF:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_DEACTIVATE;
      break;

    case QMI_VOICE_CM_IF_EVT_SUPS_INTERROGATE:
    case QMI_VOICE_CM_IF_EVT_SUPS_INTERROGATE_CONF:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_INTERROGATE;
      break;

    case QMI_VOICE_CM_IF_EVT_SUPS_REG_PASSWORD:
    case QMI_VOICE_CM_IF_EVT_SUPS_REG_PASSWORD_CONF:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_REGISTER_PASSWORD;
      break;

    case QMI_VOICE_CM_IF_EVT_SUPS_PROCESS_USS:
    case QMI_VOICE_CM_IF_EVT_SUPS_PROCESS_USS_CONF:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_USSD;
      break;
 
    default:
      QM_MSG_HIGH_1( "unsupported ss_oper in qmi_voice_cm_sups_get_service_type_from_event : %ld",(uint32)event);
      break;
  }

  return(service_type);
}

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_service_type_from_operation

===========================================================================*/
/*!
    @brief
    

    @return
    Returns Service type from CM ss_operation.
*/
/*=========================================================================*/
qmi_voice_cm_sups_service_type_e_type  qmi_voice_cm_sups_get_service_type_from_operation
(
  uint8 ss_operation
)
{
  qmi_voice_cm_sups_service_type_e_type service_type;
  switch(ss_operation)
  {
    case registerSS:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_REGISTER;
      break;
    case eraseSS:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_ERASE;
      break;
    case activateSS:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_ACTIVATE;
      break;
    case deactivateSS:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_DEACTIVATE;
      break;
    case interrogateSS:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_INTERROGATE;
      break;
    case registerPassword:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_REGISTER_PASSWORD;
      break;
    case processUnstructuredSS_Request:
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_USSD;
      break;
    default:
      QM_MSG_HIGH_1("Invalid ss_operation %d from CM", ss_operation);
      service_type = QMI_VOICE_CM_SUPS_SERVICE_TYPE_INVALID;
      break;
  }
  return service_type;
}/*  */

/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_event_from_service_type

===========================================================================*/
/*!
    @brief
    

    @return
    Returns event for the sups type.
*/
/*=========================================================================*/
qmi_voice_cm_if_evt_e_type  qmi_voice_cm_sups_get_event_from_service_type
(
  qmi_voice_cm_sups_service_type_e_type srv_type
)
{
  qmi_voice_cm_if_evt_e_type evt = QMI_VOICE_CM_IF_EVT_NONE;

  switch(srv_type)
  {
    case QMI_VOICE_CM_SUPS_SERVICE_TYPE_REGISTER:
      evt = QMI_VOICE_CM_IF_EVT_SUPS_REGISTER;
      break;

    case QMI_VOICE_CM_SUPS_SERVICE_TYPE_ERASE:
      evt = QMI_VOICE_CM_IF_EVT_SUPS_ERASE;
      break;

    case QMI_VOICE_CM_SUPS_SERVICE_TYPE_ACTIVATE:
      evt = QMI_VOICE_CM_IF_EVT_SUPS_ACTIVATE;
      break;

    case QMI_VOICE_CM_SUPS_SERVICE_TYPE_DEACTIVATE:
      evt = QMI_VOICE_CM_IF_EVT_SUPS_DEACTIVATE;
      break;

    case QMI_VOICE_CM_SUPS_SERVICE_TYPE_INTERROGATE:
      evt = QMI_VOICE_CM_IF_EVT_SUPS_INTERROGATE;
      break;

    case QMI_VOICE_CM_SUPS_SERVICE_TYPE_REGISTER_PASSWORD:
      evt = QMI_VOICE_CM_IF_EVT_SUPS_REG_PASSWORD;
      break;

    case QMI_VOICE_CM_SUPS_SERVICE_TYPE_USSD:
      evt = QMI_VOICE_CM_IF_EVT_SUPS_PROCESS_USS;
      break;
 
    default:
      QM_MSG_HIGH_1( "unsupported service type : %ld",srv_type);
      break;
  }
  return(evt);
} /* qmi_voice_cm_sups_get_event_from_service_type */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_get_network_resp_event

===========================================================================*/
/*!
    @brief
    

    @return
    Returns event name which corresponds to the network response.
*/
/*=========================================================================*/
qmi_voice_cm_if_evt_e_type  qmi_voice_cm_sups_get_network_resp_event
(
  qmi_voice_cm_if_evt_e_type ms_orig_evt
)
{
  qmi_voice_cm_if_evt_e_type ntwk_evt = QMI_VOICE_CM_IF_EVT_NONE;

  switch(ms_orig_evt)
  {
    case QMI_VOICE_CM_IF_EVT_SUPS_ACTIVATE:
      ntwk_evt = QMI_VOICE_CM_IF_EVT_SUPS_ACTIVATE_CONF;
      break;
    case QMI_VOICE_CM_IF_EVT_SUPS_REGISTER:
      ntwk_evt = QMI_VOICE_CM_IF_EVT_SUPS_REGISTER_CONF;
      break;
    case QMI_VOICE_CM_IF_EVT_SUPS_ERASE:
      ntwk_evt = QMI_VOICE_CM_IF_EVT_SUPS_ERASE_CONF;
      break;
    case QMI_VOICE_CM_IF_EVT_SUPS_DEACTIVATE:
      ntwk_evt = QMI_VOICE_CM_IF_EVT_SUPS_DEACTIVATE_CONF;
      break;
    case QMI_VOICE_CM_IF_EVT_SUPS_INTERROGATE:
      ntwk_evt = QMI_VOICE_CM_IF_EVT_SUPS_INTERROGATE_CONF;
      break;
    case QMI_VOICE_CM_IF_EVT_SUPS_REG_PASSWORD:
      ntwk_evt = QMI_VOICE_CM_IF_EVT_SUPS_REG_PASSWORD_CONF;
      break;
    case QMI_VOICE_CM_IF_EVT_SUPS_PROCESS_USS:
      ntwk_evt = QMI_VOICE_CM_IF_EVT_SUPS_PROCESS_USS_CONF;
      break;
    default:
      QM_MSG_MED_1("Invalid event %d", ms_orig_evt);
      break;
  }
  return ntwk_evt;

}/* qmi_voice_cm_sups_get_network_resp_event */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_sups_is_network_resp_event

===========================================================================*/
/*!
    @brief
    

    @return
    Returns TRUE if the event corresponds to sups response from network
*/
/*=========================================================================*/
boolean  qmi_voice_cm_sups_is_network_resp_event
(
  qmi_voice_cm_if_evt_e_type evt
)
{
  boolean is_ntwk_rsp_evt = FALSE;
  switch(evt)
  {
    case QMI_VOICE_CM_IF_EVT_SUPS_ACTIVATE_CONF:
    case QMI_VOICE_CM_IF_EVT_SUPS_REGISTER_CONF:
    case QMI_VOICE_CM_IF_EVT_SUPS_DEACTIVATE_CONF:
    case QMI_VOICE_CM_IF_EVT_SUPS_ERASE_CONF:
    case QMI_VOICE_CM_IF_EVT_SUPS_INTERROGATE_CONF:
    case QMI_VOICE_CM_IF_EVT_SUPS_REG_PASSWORD_CONF:
    case QMI_VOICE_CM_IF_EVT_SUPS_PROCESS_USS_CONF:
     is_ntwk_rsp_evt = TRUE;
     break;
    default:
      break;
  }

  return is_ntwk_rsp_evt;
}/* qmi_voice_cm_sups_is_network_resp_event */

#endif /*!FEATURE_NO_QMI_VOICE*/

#endif // FEATURE_QMI_SERVICE_SPLIT

