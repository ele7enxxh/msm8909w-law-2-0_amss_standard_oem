#ifndef _GNSS_LPP_ECID_H_
#define _GNSS_LPP_ECID_H_


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Session Manager GPS->ECID API

GENERAL DESCRIPTION
  This module makes up the Position Determination API between GPS
  and ECID ML1 software.

  The Session Manager API file is used by SM to interface with
  ECID world for LPP


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/api/gnss_lpp_ecid.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/10/13   sk      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include "gnss_common.h"
#include "aries_os_api.h"

#define GNSS_LPP_ECID_REQ_LOC_MEAS_RSRP_REQ 0
#define GNSS_LPP_ECID_REQ_LOC_MEAS_RSRQ_REQ 1
#define GNSS_LPP_ECID_REQ_LOC_MEAS_UERXTX_REQ 2


#define GNSS_LPP_ECID_PRTL_TYPE_UMTS_UP 8 // this value is used as u_sess_type
#define GNSS_LPP_ECID_PRTL_TYPE_UMTS_CP 0x200 // this value is used as u_sess_type

#define GNSS_LPP_ECID_MODE_UE_ASSISTED  2 // this value is used for q_operation_mode if network asked for multiple technology
#define SM_LOC_ECID_MODE 8 // this value is used for q_operation_mode for ECID only request


#define GNSS_LPP_ECID_CELLID_EUTRA 1
#define GNSS_LPP_ECID_CELLID_UTRA  2

typedef struct gnss_lpp_ECID_CellIdentity {
   int t; /* can be either EUTRA or UTRA */
   uint32 cell_id_num_bits; /* number of bits in the cell id */
   union {
      /* t = 1 */
      uint32 cellIdEutra;
      /* t = 2 */
      uint32 cellIdUtra; 
   } u;
} gnss_lpp_ECID_CellIdentity;


/* PLMN Identity */
typedef struct gnss_lpp_ECID_Plmn_Identity {
   uint8 mcc[3]; /* MCC always 3 digits */
   uint8 mnc[3]; /* MNC  can be less than 3 digits */
   uint8 num_mnc_digits; /* number of MNC digits */
} gnss_lpp_ECID_Plmn_Identity;

/* Cell Global ID either EUTRA or UTRA. Currently only EUTRA supported */
typedef struct gnss_lpp_ECID_CellGlobalIdEUTRA_AndUTRA {
   gnss_lpp_ECID_Plmn_Identity plmn_Identity; /* MCC MNC */
   gnss_lpp_ECID_CellIdentity cellIdentity; /* cell id */
} gnss_lpp_ECID_CellGlobalIdEUTRA_AndUTRA;

/**************************************************************/
/*                                                            */
/*  MeasuredResultsElement                                    */
/*                                                            */
/**************************************************************/

typedef struct GNSS_LPP_ECID_MeasuredResultsElement {
   struct {
      unsigned cellGlobalIdPresent : 1;
      unsigned systemFrameNumberPresent : 1;
      unsigned rsrp_ResultPresent : 1;
      unsigned rsrq_ResultPresent : 1;
      unsigned ue_RxTxTimeDiffPresent : 1;
   } m;
   uint16 w_physCellId; /* phys cell ID */
   gnss_lpp_ECID_CellGlobalIdEUTRA_AndUTRA cellGlobalId; /* global cellID */
   uint16 arfcnEUTRA; /* EUTRA arfcn */
   uint32 systemFrameNumber; /* frame number */
   uint8 rsrp_Result; /* rsrp result */
   uint8 rsrq_Result; /* rsrq result */
   uint16 ue_RxTxTimeDiff; /* rxtx time diff */
} gnss_lpp_ECID_MeasuredResultsElement;

/* can report upto 32 elements to the network */
#define GNSS_LPP_ECID_MAX_RESULTS 32

typedef struct gnss_lpp_ECID_MeasuredResultsList {
   uint32 n; /* number of elements */
   gnss_lpp_ECID_MeasuredResultsElement elem[GNSS_LPP_ECID_MAX_RESULTS];
} gnss_lpp_ECID_MeasuredResultsList;

/* ECID signal measurement information */
typedef struct gnss_lpp_ECID_SignalMeasurementInformation {
   struct {
      unsigned servingCellMeasuredResultsPresent : 1; /* if serving cell info is included */
   } m;
   gnss_lpp_ECID_MeasuredResultsElement servingCellMeasuredResults; /* serving cell results */
   gnss_lpp_ECID_MeasuredResultsList measuredResultsList; /* neighboring cell list */
} gnss_lpp_ECID_SignalMeasurementInformation;

/* if ECID error is included, error causes */
typedef enum {
   gnss_lpp_ECID_TargetDeviceErrorCauses_cause_undefined = 0,
   gnss_lpp_ECID_TargetDeviceErrorCauses_cause_requestedMeasurementNotAvailable = 1,
   gnss_lpp_ECID_TargetDeviceErrorCauses_cause_notAllrequestedMeasurementsPossible = 2
} gnss_lpp_ECID_TargetDeviceErrorCauses_cause;

typedef struct gnss_lpp_ECID_TargetDeviceErrorCauses {
   struct {
      unsigned rsrpMeasurementNotPossiblePresent : 1;
      unsigned rsrqMeasurementNotPossiblePresent : 1;
      unsigned ueRxTxMeasurementNotPossiblePresent : 1;
   } m;
   gnss_lpp_ECID_TargetDeviceErrorCauses_cause cause;
} gnss_lpp_ECID_TargetDeviceErrorCauses;

/* ECID error */
typedef struct gnss_lpp_ECID_Error {
  gnss_lpp_ECID_TargetDeviceErrorCauses targetDeviceErrorCauses;
} gnss_lpp_ECID_Error;

/* This structure needs to be filled out by the lower layer to report the measurements back to server */
typedef struct gnss_lpp_ECID_ProvideLocationInformation {
   struct {
      unsigned gnss_lpp_ecid_SignalMeasurementInformationPresent : 1; /* if measurement info is present */
      unsigned gnss_lpp_ecid_ErrorPresent : 1; /* if error is present. Can both be reported by the lower layer? */
   } m;
   gnss_lpp_ECID_SignalMeasurementInformation ecid_SignalMeasurementInformation;
   gnss_lpp_ECID_Error ecid_Error;
} gnss_lpp_ECID_ProvideLocationInformation;



typedef enum
{
  GNSS_LPP_ECID_MSG_ID_FIRST = C_USR_MSG_ID_FIRST,
  GNSS_LPP_ECID_MSG_ID_GET_MEAS_CMD,
} gnss_lpp_ecid_msg_id_type;


/* if the network asked for GPS/OTDOA/ECID in the same message, SM will send this request to RRC/L1 to get the measurements.
   Expectaiton is that these measurements will be reported within 100-200ms by RRC/L1. */
typedef struct
{
   uint32 q_meas_type; /* type of measurements being requested. should match what was in the start command */
   uint32 q_sess_type; /* session type requesting the measurements. 
                          THis field needs to be copied when measuremetns are reported to SM */
} gnss_lpp_ecid_get_measurements_type; 


/* this structure contains the measurements reported by the RRC/L1 layer to SM */
typedef struct
{
   uint32 q_sess_type; /* session type that was passed in the start cmd*/
  gnss_lpp_ECID_ProvideLocationInformation  z_meas_info; /* list of measurements and or Error */
} gnss_lpp_ecid_meas_info_type;





/*===========================================================================
  gnss_lpp_ecid_get_measurements

  Description:

     This function is used by SM to request ECID measurements from LOC_LTE_ECID module.

  Parameters: 
   q_meas_type - type of measurements requested by network
   sess_type - CP/UP. needs to be passed back when measurementes are returned to SM
 

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean gnss_lpp_ecid_get_measurements(uint32 q_meas_type, uint32 u_sess_type);


/*===========================================================================
  gnss_lpp_ecid_sm_recv_measurements

  Description:

     This function is used by LOC_LTE_ECID module to send the ECID measurements

  Parameters:
    measurements information

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean gnss_lpp_ecid_recv_measurements(gnss_lpp_ecid_meas_info_type *p_ecid_measurement_info);

#endif
