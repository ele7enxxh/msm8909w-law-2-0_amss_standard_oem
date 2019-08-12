/*!
  @file
  lte_rrc_loc_servicesi.h

  @brief
  Header file internal to RRC LOC Services Module

  @detail
  This header contains the constants definition and external function prototype 
  declarations used by the CRE module and its UTF test cases.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_loc_servicesi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#ifndef LTE_RRC_LOC_SERVICESI_H
#define LTE_RRC_LOC_SERVICESI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte.h>
#include "lte_rrc_utils.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/ 

#define LTE_RRC_LOC_SERVICES_MIN_LOC_INFO_CONF 63 // In %
#define LTE_RRC_LOC_SERVICES_LA_MAX 50 // In meters
#define LTE_RRC_LOC_SERVICES_LA_MIN 500 // In meters
#define LTE_RRC_LOC_SERVICES_T_RPT_DEFAULT 1 // In Sec
#define LTE_RRC_LOC_SERVICES_T_GNSS_DEFAULT 60 // In Sec
#define LTE_RRC_LOC_SERVICES_I_GPS_DEFAULT 10 // In Min
#define LTE_RRC_LOC_SERVICES_T_GPS_DEFAULT 120 // In Min
#define LTE_RRC_LOC_SERVICES_MIN_BATTERY 20 // In %

#define LTE_RRC_LOC_SERVICES_T_RPT_EFS_FILENAME "/nv/item_files/modem/lte/rrc/loc_services/t_rpt"

#define LTE_RRC_LOC_SERVICES_T_GNSS_EFS_FILENAME "/nv/item_files/modem/lte/rrc/loc_services/t_gnss"

#define LTE_RRC_LOC_SERVICES_I_GPS_EFS_FILENAME "/nv/item_files/modem/lte/rrc/loc_services/i_gps"

#define LTE_RRC_LOC_SERVICES_T_GPS_EFS_FILENAME "/nv/item_files/modem/lte/rrc/loc_services/t_gps"

#define LTE_RRC_LOC_SERVICES_P_MIN_EFS_FILENAME "/nv/item_files/modem/lte/rrc/loc_services/p_min"

#define LTE_RRC_LOC_SERVICES_CONF_EFS_FILENAME "/nv/item_files/modem/lte/rrc/loc_services/conf"

#define LATITUDE_MULTIPLIER 93206.74 /*This is ((2^(23)) - 1) / 90. Multiply the abs(Latitude) received from QMI-LOC with this
                                        value to get the actual encoded value to be populated in Loc info IE*/
#define LONGITUDE_MULTIPLIER 46603.37 /*This is ((2^(23)) - 1) / 180. Multiply the signed Longitude received from QMI-LOC with this
                                        value to get the actual encoded value to be populated in Loc info IE*/
typedef enum
{
  LTE_RRC_LOC_SERVICES_LAT_SIGN_NORTH = 0,
  LTE_RRC_LOC_SERVICES_LAT_SIGN_SOUTH
  
}lte_rrc_loc_services_latitude_sign_e;

typedef enum
{
  LTE_RRC_LOC_SERVICES_ALTITUDE_DIRECTION_HEIGHT = 0,
  LTE_RRC_LOC_SERVICES_ALTITUDE_DIRECTION_DEPTH  
}lte_rrc_loc_services_altitude_direction_e;


typedef struct
{
  /*!< Rel Valid */
  boolean rel_valid;

  /*!< Rel Value */
  uint8 rel_value;
  
  /*!< Latitude Valid */
  boolean lat_valid;

  /*!< Latitude Sign */
  lte_rrc_loc_services_latitude_sign_e lat_sign;

  /*!< Latitude Degrees */
  uint32 lat_degree;

  /*!< Longitude Valid */
  boolean lon_valid;

  /*!< Logitude Degrees */
  int lon_degree;  

  /*!< Altitude Valid */
  boolean altitude_valid;

  /*!< Altitude direction */
  lte_rrc_loc_services_altitude_direction_e altitude_direction;

  /*!< Altitude */
  uint16 altitude;  

  /*!< UncertaintyCircular Valid */
  boolean unc_cir_valid;

  /*!< UncertaintyCircular */
  uint32 unc_cir;

  /*!< UncertaintySemiMajor Valid */
  boolean unc_semi_maj_valid;

  /*!< UncertaintySemiMajor */
  uint32 unc_semi_maj;   

  /*!< UncertaintySemiMinor Valid */
  boolean unc_semi_min_valid;

  /*!< UncertaintySemiMinor */
  uint32 unc_semi_min; 

  /*!< Orientation Major Axis Valid */
  boolean ori_maj_axi_valid;

  /*!< Orientation Major Axis */
  uint8 ori_maj_axi; 

  /*!< Confidence Valid */
  boolean confidence_valid;

  /*!< Confidence */
  uint8 confidence; 

  /*!< Uncertanity Altitude Valid */
  boolean unc_altitude_valid;

  /*!< Uncertanity Altitude */
  uint32 unc_altitude; 

  /*!< Horizontal velocity valid */
  boolean hor_velocity_valid;

  /*!< Heading */
  uint16 heading; 

  /*!< Speed */
  uint16 hor_speed;  

  /*!< Gnss TOD valid */
  boolean gnss_tod_valid;

  /*!< Gnss TOD in ms */
  uint32 tod_ms;   

} lte_rrc_loc_services_loc_info_s;


typedef struct
{
  /*!< Dynamically allocated part of private data */

  /*!< Logging interval */
  uint32 log_interval;

   /*!< Requesting module */
  lte_rrc_loc_services_client_type_e client;

  /*!< Decoded location info */
  lte_rrc_loc_services_loc_info_s decoded_loc_info;

  /*!< location info */
  boolean loc_info_present;
  
  /*!< location info from TLB */
  boolean is_update_loc_info;  
  
} lte_rrc_loc_services_dd_s;

/* horizontal reliability of the received location reports */
typedef enum {
  LTE_RRC_LOC_SERVICES_RELIABILITY_NOT_SET_V02 = 0, /**<  Location reliability is not set  */
  LTE_RRC_LOC_SERVICES_RELIABILITY_VERY_LOW_V02 = 1, /**<  Location reliability is very low; use it at your own risk  */
  LTE_RRC_LOC_SERVICES_RELIABILITY_LOW_V02 = 2, /**<  Location reliability is low; little or no cross-checking is possible  */
  LTE_RRC_LOC_SERVICES_RELIABILITY_MEDIUM_V02 = 3, /**<  Location reliability is medium; limited cross-check passed   */
  LTE_RRC_LOC_SERVICES_RELIABILITY_HIGH_V02 = 4, /**<  Location reliability is high; strong cross-check passed  */
  LTE_RRC_LOC_SERVICES_RELIABILITY_MAX
}lte_rrc_loc_services_rel_e;

typedef struct
{
  /*!<-Minimum horizontal reliability of the received location reports */
  uint8 rel_min; /* Corresponds to lte_rrc_loc_services_rel_e */

  /*!<-Minimum horizontal confidence of the received location reports */
  uint8 conf_min; /* 0-99 */

  /*!< Satisfactory level of Location Accuracy in meters */
  uint16 la_max;

  /*!< Minimum location accuracy acceptable for reporting in meters */
  uint16 la_min;

  /*!< Seconds. Time allowed for the modem to wait for location information to be available */
  uint8 t_rpt_value;   
    
  /*!< Seconds. Time UE allowed to keep GNSS running for location before it gets to acceptable accuracy level */
  uint8 t_gnss_value; 

  /*!< Minutes. Time interval between two consecutive GNSS read request */
  uint8 i_gps_value;  

  /*!< Minutes. Time for which MDT measurement may request for the GNSS location information */
  uint8 t_gps_value;      

  /* Minimum battery percentage where GNSS read for location information is allowed */
  uint8 pmin;

  /*! Seconds. T_RPT value read via EFS*/
  uint8 t_rpt_value_efs;
} lte_rrc_loc_services_acq_param_s;

typedef struct
{
  /*!< Time for which MDT measurement may request for the GNSS location information */
  lte_rrc_tmr_s t_gps;
  
  /*!< Time UE allowed to keep GNSS running for location before it gets to acceptable accuracy level */
  lte_rrc_tmr_s t_gnss;

  /*!< Time allowed for the modem to wait for location information to be available */
  lte_rrc_tmr_s t_rpt;

  /*!< Time interval between two consecutive GNSS read request */
  lte_rrc_tmr_s i_gps;

  /*!< Is location service initialized */
  boolean service_notify_int_done;

  /*!< Is location service initialized */
  boolean loc_ser_client_int_done;
  
  /*!< Number of available services */
  uint32 num_services;
  
  /*!< Loc Info thresholds */
  lte_rrc_loc_services_acq_param_s acq_param;
  
  /*! Callback data stored in global space*/
  int client_cb_data;
} lte_rrc_loc_services_sd_s;


typedef struct
{
  /*!< Dynamically allocated part of private data */
  lte_rrc_loc_services_dd_s *dd_ptr;

  /*!< Staic private data */
  lte_rrc_loc_services_sd_s *sd_ptr;

} lte_rrc_loc_services_s;


/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_get_loc_info

===========================================================================*/
/*!
  @brief
  Save received position report into local DB

  @detail
  
  @return  

*/
/*=========================================================================*/  
extern void  lte_rrc_loc_services_loc_info_Indi
(
  lte_rrc_loc_services_info_indi_s *msg_ptr  /* Ptr to received rpt */
);


/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_update_loc_info_Ind

===========================================================================*/
/*!
  @brief
  Save received position report into local DB

  @detail
  
  @return  

*/
/*=========================================================================*/  
extern void  lte_rrc_loc_services_update_loc_info_Ind
(
  lte_tlb_ctrl_update_loc_info_s *msg_ptr  /* Ptr to received rpt */
);

/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_get_loc_info

===========================================================================*/
/*!
  @brief
  Fill location info to caller passes pointer. NULL will be returned if
  loc info is not available

  @detail
  
  @return  

*/
/*=========================================================================*/  
extern void lte_rrc_loc_services_get_loc_info
(
  lte_rrc_osys_LocationInfo_r10 *rcv_asn1_loc_info  
);

/* Timer call back */
extern void lte_rrc_loc_services_tmr_cb
(
  lte_rrc_timer_cb_data_type data
);

/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_start_fix_collection

===========================================================================*/
/*!
  @brief
    This function will start fix collection from GPS.

  @detail
  
  @return  

*/
/*=========================================================================*/ 
extern void lte_rrc_loc_services_start_fix_collection
(
  uint32 log_interval, /* Logging interval in ms, this needs to be 0 if requesting for single fix */
  lte_rrc_loc_services_client_type_e client, /* Module requesting for loc services */
  boolean is_new_session  
);

/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_stop_fix_collection

===========================================================================*/
/*!
  @brief
    This function will stop fix collection from GPS.

  @detail
  
  @return  

*/
/*=========================================================================*/ 
extern void lte_rrc_loc_services_stop_fix_collection
(  
  lte_rrc_loc_services_client_type_e client /* Module requesting for loc services */ 
);

/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_dd_init

===========================================================================*/
/*!
  @brief
  Function to initialize dynamic data required for the state machine

  @detail
  Called upon activation of RRC task.

  @return
  None

*/
/*=========================================================================*/
extern void lte_rrc_loc_services_dd_init( void );

/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_dd_deinit

===========================================================================*/
/*!
  @brief
  Function to deinitialize dynamic data required for the state machine

  @detail
  Called upon deactivation of RRC task.

  @return
  None

*/
/*=========================================================================*/
extern void lte_rrc_loc_services_dd_deinit( void );


/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_get_decoded_loc_info

===========================================================================*/
/*!
  @brief
  Fill location info to caller passes pointer. NULL will be returned if
  loc info is not available

  @detail
  
  @return  

*/
/*=========================================================================*/  
extern void lte_rrc_loc_services_get_decoded_loc_info
(
  lte_rrc_loc_services_loc_info_s *rcv_loc_info  
);

/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_get_decoded_loc_info_for_logging

===========================================================================*/
/*!
  @brief
  Fill location info to caller passes pointer. NULL will be returned if
  loc info is not available. DO NOT clear the decoded loc info

  @detail
  
  @return  

*/
/*=========================================================================*/

extern void lte_rrc_loc_services_get_decoded_loc_info_for_logging
(
  lte_rrc_loc_services_loc_info_s *rcv_loc_info  
);

extern void lte_rrc_loc_services_init_data( void );

extern void lte_rrc_loc_services_deinit_data( void );
/*===========================================================================

  FUNCTION:  lte_rrc_loc_services_encode_loc_info

===========================================================================*/
/*!
  @brief
  Send location to MDT

  @detail
  
  @return  

*/
/*=========================================================================*/  

extern void  lte_rrc_loc_services_encode_loc_info
(
  lte_rrc_loc_services_loc_info_s *loc_info, /* Pointer to local DB */ 
  lte_rrc_osys_LocationInfo_r10 *asn1_loc_info /* POinter to target */
);

extern OSINT8 lte_rrc_loc_services_get_unc
(
  uint32 unc
);
#endif
