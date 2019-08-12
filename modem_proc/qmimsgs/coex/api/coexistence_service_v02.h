#ifndef COEX_SERVICE_02_H
#define COEX_SERVICE_02_H
/**
  @file coexistence_service_v02.h

  @brief This is the public header file which defines the coex service Data structures.

  This header file defines the types and structures that were defined in
  coex. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set 
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/coex/api/coexistence_service_v02.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.10 
   It was generated on: Tue Aug  5 2014 (Spin 1)
   From IDL File: coexistence_service_v02.idl */

/** @defgroup coex_qmi_consts Constant values defined in the IDL */
/** @defgroup coex_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup coex_qmi_enums Enumerated types used in QMI messages */
/** @defgroup coex_qmi_messages Structures sent as QMI messages */
/** @defgroup coex_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup coex_qmi_accessor Accessor for QMI service object */
/** @defgroup coex_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup coex_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define COEX_V02_IDL_MAJOR_VERS 0x02
/** Revision Number of the IDL used to generate this file */
#define COEX_V02_IDL_MINOR_VERS 0x03
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define COEX_V02_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define COEX_V02_MAX_MESSAGE_ID 0x0034
/**
    @}
  */


/** @addtogroup coex_qmi_consts 
    @{ 
  */

/**  Maximum number of supported unique radio access technologies (RATs).  */
#define COEX_MAX_TECHS_SUPPORTED_V02 24

/**  Maximum number of unique operating bands and channels supported for WWAN GSM RAT.  */
#define COEX_WWAN_GSM_MAX_BANDS_SUPPORTED_V02 16

/**  Maximum number of unique operating bands and channels supported for all other WWAN RATs.  */
#define COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02 8

/**  Maximum number of unique events supported for WLAN Wi-Fi RAT.  */
#define COEX_WLAN_WIFI_MAX_BANDS_SUPPORTED_V02 4

/**  Maximum number of unique WLAN Wi-Fi high-priority events supported.  */
#define COEX_WLAN_WIFI_MAX_EVENTS_SUPPORTED_V02 8

/**  Maximum number of unique WLAN Wi-Fi active connections supported.  */
#define COEX_WLAN_WIFI_MAX_CONNS_SUPPORTED_V02 4

/**  Maximum number of conflict entries in the victim table shared with each
     response.  */
#define COEX_NV_MSG_VICTIM_TABLE_MAX_ENTRIES_V02 2
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_TECH_TYPE_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_TECH_TYPE_UNINIT_V02 = 0, /**<  RAT type is uninitialized  */
  COEX_TECH_TYPE_WWAN_LTE_V02 = 1, /**<  RAT type is WWAN LTE  */
  COEX_TECH_TYPE_WWAN_TDSCDMA_V02 = 2, /**<  RAT type is WWAN TD-SCDMA  */
  COEX_TECH_TYPE_WWAN_GSM_V02 = 3, /**<  RAT type is WWAN GSM [Instance 1]  */
  COEX_TECH_TYPE_WWAN_ONEX_V02 = 4, /**<  RAT type is WWAN CDMA2000\textsuperscript{\textregistered} 1X  */
  COEX_TECH_TYPE_WWAN_HDR_V02 = 5, /**<  RAT type is WWAN HDR  */
  COEX_TECH_TYPE_WWAN_WCDMA_V02 = 6, /**<  RAT type is WWAN WCDMA  */
  COEX_TECH_TYPE_WLAN_WIFI_V02 = 7, /**<  RAT type is WLAN Wi-Fi  */
  COEX_TECH_TYPE_WLAN_BT_V02 = 8, /**<  RAT type is WLAN BlueTooth  */
  COEX_TECH_TYPE_SPECIAL_DIVERSITY_ANTENNA_V02 = 9, /**<  Special type to represent WWAN (MDM) diversity antenna chain  */
  COEX_TECH_TYPE_WWAN_GSM2_V02 = 10, /**<  RAT type is WWAN GSM [Instance 2]  */
  COEX_TECH_TYPE_WWAN_GSM3_V02 = 11, /**<  RAT type is WWAN GSM [Instance 3]  */
  COEX_TECH_TYPE_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_tech_type_v02;
/**
    @}
  */

typedef uint8_t coex_tech_state_v02;
#define COEX_TECH_STATE_OFF_V02 ((coex_tech_state_v02)0x00) /**<  Whether the technology's state is off  */
#define COEX_TECH_STATE_IDLE_V02 ((coex_tech_state_v02)0x01) /**<  Whether the technology's state is idle  */
#define COEX_TECH_STATE_CONN_V02 ((coex_tech_state_v02)0x02) /**<  Whether the technology's state is connected  */
#define COEX_TECH_STATE_ACTIVE_V02 ((coex_tech_state_v02)0x04) /**<  Whether the technology's state is active  */
/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_TECH_OPERATING_DIMENSION_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_TECH_OPERATING_DIMENSION_FDD_V02 = 0x0, /**<  Technology is operating in the Frequency Division Duplex dimension  */
  COEX_TECH_OPERATING_DIMENSION_TDD_V02 = 0x1, /**<  Technology is operating in the Time Division Duplex dimension  */
  COEX_TECH_OPERATING_DIMENSION_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_tech_operating_dimension_v02;
/**
    @}
  */

typedef uint8_t coex_tech_band_direction_v02;
#define COEX_TECH_BAND_DIRECTION_DONT_CARE_V02 ((coex_tech_band_direction_v02)0x00) /**<  Band direction is don't care  */
#define COEX_TECH_BAND_DIRECTION_DOWNLINK_V02 ((coex_tech_band_direction_v02)0x01) /**<  Band information is for a downlink.  */
#define COEX_TECH_BAND_DIRECTION_UPLINK_V02 ((coex_tech_band_direction_v02)0x02) /**<  Band information is for an uplink.  */
/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t frequency;
  /**<   Frequency; radio access technology's operating center frequency in MHz 
       (times 10). This value is represented in x10 to provide the fractional 
       precision of one decimal point. Valid range: 0.0 MHz to 6553.5 MHz.
       Values: 0 to 65535 (2\textsuperscript{16}-1).
  */

  uint16_t bandwidth_and_direction;
  /**<   Bandwidth and direction; radio access technology's operating bandwidth 
       in MHz (times 10) bit-packed with the link direction.\n
       - bit[15:14] -- coex_tech_band_direction: Uplink, Downlink, Both, Don't Care.
                      Note that a band can be specified as both uplink (UL) and 
                      downlink (DL) by combining the mask values of UL and DL.\n
       - bit[13:0]  -- Indicates the bandwidth in x10 to provide fractional precision 
                      of one decimal point. Valid range: 0.0 MHz to 1638.3 MHz.
                      Values: 0 to 16383 (2\textsuperscript{14}-1).
  */
}coex_tech_band_type_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_ANTENNA_CHAIN_STATE_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_ANTENNA_CHAIN_STATE_INACTIVE_V02 = 0, /**<  Antenna chain is inactive (not in use by the modem)  */
  COEX_ANTENNA_CHAIN_STATE_ACTIVE_V02 = 1, /**<  Antenna chain is active (in use by the modem).  */
  COEX_ANTENNA_CHAIN_STATE_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_antenna_chain_state_v02;
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t adv_notice;
  /**<    Advance notice; delta time (in the future), in milliseconds, when 
        the specified coex_antenna_chain_state takes effect. */

  uint16_t duration;
  /**<    Duration; delta time (in the future), in milliseconds, beyond the 
        advance notice that the coex_antenna_chain_state remains in effect. */
}coex_antenna_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t id;
  /**<   ID; bit-packed to represent the following fields:\n
       - bit[7:6] -- Reserved for future use \n
       - bit[5:0] -- coex_tech_type; COEX technology type
  */

  uint8_t sync_state_token;
  /**<   Sync state token; bit-packed to represent the following fields:\n
       - bit[7:5] -- coex_tech_state; the technology's current state\n
       - bit[4:0] -- 5-bit revolving token that indicates if the state changed;
                     valid range: 0 to 31
  */
}coex_tech_sync_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Requests synchronization of COEX states between modules. */
typedef struct {

  /* Optional */
  /*  WWAN/WLAN Technologies Sync State */
  uint8_t tech_valid;  /**< Must be set to true if tech is being passed */
  uint32_t tech_len;  /**< Must be set to # of elements in tech */
  coex_tech_sync_state_v02 tech[COEX_MAX_TECHS_SUPPORTED_V02];
  /**<   \n
     (Shares the current COEX synchronization state for all valid/current 
     technologies.)
   */
}qmi_coex_tech_sync_req_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Responds to synchronize the COEX states between modules. */
typedef struct {

  /* Optional */
  /*  WWAN/WLAN Technologies Sync State */
  uint8_t tech_valid;  /**< Must be set to true if tech is being passed */
  uint32_t tech_len;  /**< Must be set to # of elements in tech */
  coex_tech_sync_state_v02 tech[COEX_MAX_TECHS_SUPPORTED_V02];
  /**<   \n
       (Used to share the current COEX synchronization state for all valid/current 
       technologies.)*/
}qmi_coex_tech_sync_resp_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Updates a specific COEX state between modules. */
typedef struct {

  /* Optional */
  /*  WWAN/WLAN Technologies Sync State */
  uint8_t tech_valid;  /**< Must be set to true if tech is being passed */
  coex_tech_sync_state_v02 tech;
  /**<   \n 
       (Used to share the current COEX synchronization state for the current technology.)*/
}qmi_coex_tech_sync_update_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t id;
  /**<   ID; bit-packed to represent the following fields:\n
       - bit[7:6] -- Reserved for future use\n
       - bit[5:0] -- coex_tech_type; the COEX technology type */
}coex_tech_state_update_req_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Requests an update of the COEX state of various technologies. */
typedef struct {

  /* Optional */
  /*  WWAN/WLAN Technologies Sync State */
  uint8_t tech_ids_valid;  /**< Must be set to true if tech_ids is being passed */
  uint32_t tech_ids_len;  /**< Must be set to # of elements in tech_ids */
  coex_tech_state_update_req_v02 tech_ids[COEX_MAX_TECHS_SUPPORTED_V02];
  /**<   \n
       (Requests the current COEX state for selected technologies.)
  */
}qmi_coex_tech_state_update_req_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_LTE_TDD_CONFIG_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_LTE_TDD_CONFIG_UNDEF_V02 = 0, /**<  Uninitialized LTE TDD configuration  */
  COEX_LTE_TDD_CONFIG_0_V02 = 1, /**<  LTE TDD configuration 0  */
  COEX_LTE_TDD_CONFIG_1_V02 = 2, /**<  LTE TDD configuration 1  */
  COEX_LTE_TDD_CONFIG_2_V02 = 3, /**<  LTE TDD configuration 2  */
  COEX_LTE_TDD_CONFIG_3_V02 = 4, /**<  LTE TDD configuration 3  */
  COEX_LTE_TDD_CONFIG_4_V02 = 5, /**<  LTE TDD configuration 4  */
  COEX_LTE_TDD_CONFIG_5_V02 = 6, /**<  LTE TDD configuration 5  */
  COEX_LTE_TDD_CONFIG_6_V02 = 7, /**<  LTE TDD configuration 6  */
  COEX_LTE_TDD_CONFIG_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_lte_tdd_config_v02;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_LTE_TDD_SUBFRAME_CONFIG_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_LTE_TDD_SUBFRAME_CONFIG_UNDEF_V02 = 0, /**<  Uninitialized LTE TDD configuration  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_0_V02 = 1, /**<  LTE TDD special subframe configuration 0  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_1_V02 = 2, /**<  LTE TDD special subframe configuration 1  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_2_V02 = 3, /**<  LTE TDD special subframe configuration 2  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_3_V02 = 4, /**<  LTE TDD special subframe configuration 3  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_4_V02 = 5, /**<  LTE TDD special subframe configuration 4  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_5_V02 = 6, /**<  LTE TDD special subframe configuration 5  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_6_V02 = 7, /**<  LTE TDD special subframe configuration 6  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_7_V02 = 8, /**<  LTE TDD special subframe configuration 7  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_8_V02 = 9, /**<  LTE TDD special subframe configuration 8  */
  COEX_LTE_TDD_SUBFRAME_CONFIG_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_lte_tdd_subframe_config_v02;
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_LTE_TDD_LINK_CONFIG_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_LTE_TDD_LINK_UNDEF_V02 = 0, /**<  Uninitialized LTE TDD link  */
  COEX_LTE_TDD_LINK_NORMAL_V02 = 1, /**<  Normal cyclic prefix  */
  COEX_LTE_TDD_LINK_EXTENDED_V02 = 2, /**<   Extended cyclic prefix  */
  COEX_LTE_TDD_LINK_CONFIG_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_lte_tdd_link_config_v02;
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t band_info_len;  /**< Must be set to # of elements in band_info */
  coex_tech_band_type_v02 band_info[COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02];
  /**<   Band info; WWAN LTE's current operating frequencies */

  uint8_t sync_state_token;
  /**<   Sync state token; bit-packed to represent the following fields:\n
       - bit[7:5] -- coex_tech_state; technology's current state\n
       - bit[4:0] -- 5-bit revolving token to indicate if the state changed;
                     valid range: 0 to 31
  */

  uint16_t data;
  /**<   Data; bit-packed to represent the following fields:\n
       - bit[15]    -- coex_tech_operating_dimension; LTE's dimension\n
       - bit[14:10] -- Reserved for future use\n
       - bit[9:0]   -- Frame offset (timing advance) in microseconds;
                       valid range: 0 to 1023 microseconds
  */

  uint16_t tdd_info;
  /**<   LTE Time Division Duplex (TDD) information; contains the TDD parameters
       and is bit-packed to represent the following fields:\n
       - bit[15:12] -- Reserved for future use\n
       - bit[11:10] -- Uplink coex_lte_tdd_link_config\n
       - bit[9:8]   -- Downlink coex_lte_tdd_link_config\n
       - bit[7:4]   -- coex_lte_tdd_config\n
       - bit[3:0]   -- coex_lte_tdd_subframe_config
  */
}coex_wwan_lte_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the COEX state for the WWAN LTE technology. */
typedef struct {

  /* Optional */
  /*  WWAN LTE Technology's State Information for COEX */
  uint8_t lte_band_info_valid;  /**< Must be set to true if lte_band_info is being passed */
  coex_wwan_lte_state_v02 lte_band_info;
  /**<   \n
       (Contains the LTE state information needed for COEX.)
      */
}qmi_coex_wwan_lte_state_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t band_info_len;  /**< Must be set to # of elements in band_info */
  coex_tech_band_type_v02 band_info[COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02];
  /**<   Band information; represents the WWAN TD-SCDMA's current operating frequencies. */

  uint8_t sync_state_token;
  /**<   Sync state token; bit-packed to represent the following fields:\n
       - bit[7:5] -- coex_tech_state; technology's current state\n
       - bit[4:0] -- 5-bit revolving token to indicate if the state changed;
                     valid range: 0 to 31
  */
}coex_wwan_tdscdma_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the COEX state for the WWAN TD-SCDMA technology. */
typedef struct {

  /* Optional */
  /*  WWAN TD-SCDMA Technology's State Information for COEX */
  uint8_t tdscdma_band_info_valid;  /**< Must be set to true if tdscdma_band_info is being passed */
  coex_wwan_tdscdma_state_v02 tdscdma_band_info;
  /**<   \n
       (Contains the TD-SCDMA state information needed for COEX.)
       */
}qmi_coex_wwan_tdscdma_state_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t band_info_len;  /**< Must be set to # of elements in band_info */
  coex_tech_band_type_v02 band_info[COEX_WWAN_GSM_MAX_BANDS_SUPPORTED_V02];
  /**<   Band information; the WWAN GSM's current operating frequencies */

  uint8_t sync_state_token;
  /**<   Sync state token; bit-packed to represent the following fields:\n
       - bit[7:5] -- coex_tech_state; technology's current state\n
       - bit[4:0] -- 5-bit revolving token to indicate if the state changed;
                    valid range: 0 to 31
  */
}coex_wwan_gsm_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the COEX state for the WWAN GSM [Instance 1] 
                     technology. */
typedef struct {

  /* Optional */
  /*  WWAN GSM [Instance 1] Technology's State Information for COEX */
  uint8_t gsm_band_info_valid;  /**< Must be set to true if gsm_band_info is being passed */
  coex_wwan_gsm_state_v02 gsm_band_info;
  /**<   \n
       (Contains the GSM [Instance 1] state information needed for COEX.)
  */
}qmi_coex_wwan_gsm_state_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the COEX state for the WWAN GSM [Instance 2] 
                     technology. */
typedef struct {

  /* Optional */
  /*  WWAN GSM [Instance 2] Technology's State Information for COEX */
  uint8_t gsm2_band_info_valid;  /**< Must be set to true if gsm2_band_info is being passed */
  coex_wwan_gsm_state_v02 gsm2_band_info;
  /**<   \n
        (Contains the GSM [Instance 2] state information needed for COEX.)
   */
}qmi_coex_wwan_gsm2_state_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the COEX state for the WWAN GSM [Instance 3] 
                     technology. */
typedef struct {

  /* Optional */
  /*  WWAN GSM [Instance 3] Technology's State Information for COEX */
  uint8_t gsm3_band_info_valid;  /**< Must be set to true if gsm3_band_info is being passed */
  coex_wwan_gsm_state_v02 gsm3_band_info;
  /**<   \n
          (Contains the GSM [Instance 3] state information needed for COEX.)
     */
}qmi_coex_wwan_gsm3_state_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t band_info_len;  /**< Must be set to # of elements in band_info */
  coex_tech_band_type_v02 band_info[COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02];
  /**<   Band information; represents the WWAN CDMA2000 1X's (ONEX) current 
       operating frequencies 
  */

  uint8_t sync_state_token;
  /**<   Sync state token; bit-packed to represent the following fields:\n
       - bit[7:5] -- coex_tech_state; the technology's current state\n
       - bit[4:0] -- 5-bit revolving token to indicate if the state changed;
                     valid range: 0 to 31
  */
}coex_wwan_onex_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the COEX state for the WWAN ONEX technology. */
typedef struct {

  /* Optional */
  /*  WWAN ONEX Technology's State Information for COEX */
  uint8_t onex_band_info_valid;  /**< Must be set to true if onex_band_info is being passed */
  coex_wwan_onex_state_v02 onex_band_info;
  /**<   \n 
       (Contains the ONEX state information needed for COEX.)
       */
}qmi_coex_wwan_onex_state_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t band_info_len;  /**< Must be set to # of elements in band_info */
  coex_tech_band_type_v02 band_info[COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02];
  /**<   Band information; represents the WWAN HDR's current operating frequencies */

  uint8_t sync_state_token;
  /**<   Sync state token; bit-packed to represent the following fields:\n
       - bit[7:5] -- coex_tech_state; the technology's current state\n
       - bit[4:0] -- 5-bit revolving token to indicate if the state changed;
                     valid range: 0 to 31
  */
}coex_wwan_hdr_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the COEX state for the WWAN HDR technology. */
typedef struct {

  /* Optional */
  /*  WWAN HDR Technology's State Information for COEX */
  uint8_t hdr_band_info_valid;  /**< Must be set to true if hdr_band_info is being passed */
  coex_wwan_hdr_state_v02 hdr_band_info;
  /**<   \n
      (Contains the HDR state information needed for COEX.)
    */
}qmi_coex_wwan_hdr_state_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t band_info_len;  /**< Must be set to # of elements in band_info */
  coex_tech_band_type_v02 band_info[COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02];
  /**<   Band information; represents the WWAN WCDMA's current operating frequencies */

  uint8_t sync_state_token;
  /**<   Sync state token; bit-packed to represent the following fields:\n
       - bit[7:5] -- coex_tech_state; the technology's current state\n
       - bit[4:0] -- 5-bit revolving token to indicate if the state changed;
                     valid range: 0 to 31
  */
}coex_wwan_wcdma_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the COEX state for the WWAN WCDMA technology. */
typedef struct {

  /* Optional */
  /*  WWAN WCDMA Technology's State Information for COEX */
  uint8_t wcdma_band_info_valid;  /**< Must be set to true if wcdma_band_info is being passed */
  coex_wwan_wcdma_state_v02 wcdma_band_info;
  /**<   \n
       (Contains the WCDMA state information needed for COEX.)
       */
}qmi_coex_wwan_wcdma_state_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t band_info_len;  /**< Must be set to # of elements in band_info */
  coex_tech_band_type_v02 band_info[COEX_WLAN_WIFI_MAX_BANDS_SUPPORTED_V02];
  /**<   Band information; represents the current WLAN Wi-Fi's connection 
       operating frequencies */
}coex_wlan_wifi_connection_info_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t high_priority_events_list_len;  /**< Must be set to # of elements in high_priority_events_list */
  coex_tech_band_type_v02 high_priority_events_list[COEX_WLAN_WIFI_MAX_EVENTS_SUPPORTED_V02];
  /**<   High priority events list; list of active WLAN Wi-Fi frequencies with 
    on-going high priority events */

  uint32_t connections_list_len;  /**< Must be set to # of elements in connections_list */
  coex_wlan_wifi_connection_info_v02 connections_list[COEX_WLAN_WIFI_MAX_CONNS_SUPPORTED_V02];
  /**<   Connections list; list of the active WLAN Wi-Fi on-going connections */

  uint8_t sync_state_token;
  /**<   Sync state token; bit-packed to represent the following fields:\n
       - bit[7:5] -- coex_tech_state; the technology's current state\n
       - bit[4:0] -- 5-bit revolving token to indicate if the state changed;
                     valid range: 0 to 31
  */
}coex_wlan_wifi_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the COEX state for the WLAN Wi-Fi\reg technology. */
typedef struct {

  /* Optional */
  /*  WLAN Wi-Fi Technology's State Information for COEX */
  uint8_t wifi_state_info_valid;  /**< Must be set to true if wifi_state_info is being passed */
  coex_wlan_wifi_state_v02 wifi_state_info;
  /**<   \n
       (Contains the WLAN Wi-Fi state information needed for COEX.)
        */
}qmi_coex_wlan_wifi_state_ind_msg_v02;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_coex_wlan_bt_state_ind_msg_v02;

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_antenna_state_v02 data;
  /**<   Data; represents the state of the diversity antenna.
       It comprises the following fields:\n
        - adv_notice -- Time (in the future), in milliseconds, when the
                      coex_antenna_chain_state takes effect\n
        - duration -- Time, in milliseconds, beyond the adv_notice that the 
                    coex_antenna_chain_state remains in effect
  */

  uint8_t sync_state_token;
  /**<   Sync state token; bit-packed to represent the following fields:\n
       - bit[7:5] -- coex_tech_state; the technology's current state; note
                     that only bit[7] applies and is represented by 
                     coex_antenna_chain_state; bit[6:5] is don't care\n
       - bit[4:0] -- 5-bit revolving token to indicate if the state changed;
                     valid range: 0 to 31
  */
}coex_diversity_antenna_state_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Indicates the state of the diversity antenna chain. */
typedef struct {

  /* Optional */
  /*  Antenna Chain State Information for COEX */
  uint8_t state_valid;  /**< Must be set to true if state is being passed */
  coex_diversity_antenna_state_v02 state;
  /**<   \n
        (Contains the current state of the diversity antenna chain needed for 
        COEX.) */
}qmi_coex_diversity_antenna_state_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t filter_alpha;
  /**<   Filter alpha; the "history" parameter for the first-order LTE SINR 
       metrics filter. Valid range: 0 to 1, with 1/100th precision; represented 
       as x100 (times 100), e.g., 0.1 is 10, 0.89 is 89.
  */
}coex_metrics_lte_sinr_params_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Indication Message; Starts collection and collation of the LTE Signal-to-Interface
              Plus Noise Ratio (SINR) metric. */
typedef struct {

  /* Optional */
  /*  Filter Parameters */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  coex_metrics_lte_sinr_params_v02 data;
  /**<   \n
      (Filter parameters for the LTE SINR metric.)
  */
}qmi_coex_metrics_lte_sinr_start_ind_msg_v02;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_coex_metrics_lte_sinr_read_req_msg_v02;

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  int8_t sinr_system;
  /**<   Filter output for the total (overall system);
       LTE SINR metrics, in dBM. Likely range: -10 to +30 
       (with 1 dBM resolution)
  */

  int8_t sinr_bt_only;
  /**<   Filter output for the LTE SINR metrics, in dBM,
       when Bluetooth is active.
       Likely range: -10 to +30 (with 1 dBM resolution)
  */

  int8_t sinr_wifi_only;
  /**<   Filter output for the LTE SINR metrics, in dBM,
       when Wi-Fi is active.
       Likely range: -10 to +30 (with 1 dBM resolution)
  */

  int8_t sinr_bt_and_wifi;
  /**<   Filter output for the LTE SINR metrics, in dBM,
       when both Bluetooth & Wi-Fi are active.
       Likely range: -10 to +30 (with 1 dBM resolution)
  */

  int8_t sinr_lte_only;
  /**<   Filter output for the LTE SINR metrics, in dBM,
       when only LTE is active (both Bluetooth and Wi-Fi are inactive).
       Likely range: -10 to +30 (with 1 dBM resolution)
  */
}coex_metrics_lte_sinr_stats_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_METRICS_ESTATUS_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_METRICS_E_SUCCESS_V02 = 0x00, /**<  Metrics were collected and reported successfully  */
  COEX_METRICS_E_NOT_STARTED_V02 = 0x01, /**<  Metric collection was not started; the request was never made  */
  COEX_METRICS_E_TECH_NOT_ACTIVE_V02 = 0x02, /**<  Metrics technology is not present or active  */
  COEX_METRICS_ESTATUS_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_metrics_estatus_v02;
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t estatus;
  /**<   Validates the enclosed LTE SINR metrics collected
  */

  uint8_t collection_duration;
  /**<   Collection duration; how long the metric collection 
       has been running, in tens of milliseconds. Duration range: 
       0x00 - 0xFE (0 to 254*10 = 2540 ms). \n
       Special values:\n
       - 0x00 -- Just started or not started (see estatus) \n
       - 0xFF -- More than 2.54 seconds has passed
  */

  coex_metrics_lte_sinr_stats_v02 stats;
  /**<   Summary report of all the LTE SINR metrics collected so far
  */
}coex_metrics_lte_sinr_report_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Provides the requested filter output for the LTE SINR metric. */
typedef struct {

  /* Optional */
  /*  LTE SINR Metrics Report */
  uint8_t report_valid;  /**< Must be set to true if report is being passed */
  coex_metrics_lte_sinr_report_v02 report;
  /**<   \n
      (Current report of LTE SINR statistics, as requested.)
  */
}qmi_coex_metrics_lte_sinr_read_resp_msg_v02;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_coex_metrics_lte_sinr_stop_ind_msg_v02;

/** @addtogroup coex_qmi_messages
    @{
  */
/** Request Message; Gets the parameters related to processing conflicting WCN and
              WWAN bands. */
typedef struct {

  /* Optional */
  /*  Victim Table Offset */
  uint8_t victim_tbl_offset_valid;  /**< Must be set to true if victim_tbl_offset is being passed */
  uint8_t victim_tbl_offset;
  /**<   Offset in the complete victim table from which to extract the sub-table
       with a maximum size of COEX_NV_MSG_ VICTIM_TABLE_MAX_ENTRIES in the response. 
       
       \hangindent1.1cm {\sf\small\bf Note: \bf} The offset is zero-based.
   */
}qmi_coex_conflict_params_req_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t tech_type;
  /**<   WWAN technology type; bit-packed to represent the following fields:\n
       - bit[15:10] -- Reserved for future use \n
       - bit[9:8]   -- coex_tech_operating_ dimension; the RAT's dimension \n
       - bit[7:6]  -- coex_tech_band_direction: Uplink, Downlink, Both, or 
                      Don't Care. Note that a band can be specified as both 
                      uplink (UL) and downlink (DL) by combining the mask 
                      values of UL and DL. \n
       - bit[5:0]  -- coex_tech_type; the COEX technology type
  */

  uint16_t frequency_range_start;
  /**<   WWAN lower bound for the RAT's operating frequency range, 
      in MHz times 10; it is done in x10 to provide the fractional precision of
      1 decimal point. Valid range: 0.0 MHz to 65535 MHz. Values: 0 to 65535
	  (2\textsuperscript{16}-1).
   */

  uint16_t frequency_range_stop;
  /**<   WWAN upper bound of RAT's operating frequency range, in MHz times 10; it
       is done in x10 to provide fractional precision of 1 decimal point. 
       Valid range: 0.0 MHz to 65535 MHz. Values: 0 to 65535
	  (2\textsuperscript{16}-1).
   */
}coex_wwan_conflict_band_range_type_v02;  /* Type */
/**
    @}
  */

typedef uint8_t coex_wlan_tech_operating_mode_v02;
#define COEX_WLAN_UNDEF_MODE_V02 ((coex_wlan_tech_operating_mode_v02)0x00) /**<  WLAN technology mode is undefined or unintialized  */
#define COEX_WLAN_CONN_MODE_V02 ((coex_wlan_tech_operating_mode_v02)0x01) /**<  WLAN technology is connected  */
#define COEX_WLAN_HIGH_PRIO_MODE_V02 ((coex_wlan_tech_operating_mode_v02)0x02) /**<  WLAN technology is in high priority mode  */
/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t tech_type;
  /**<   WLAN technology type; bit-packed to represent the following fields:\n
       - bit[15:8] -- Reserved for future use \n
       - bit[7:6]  -- coex_wlan_tech_operating_ mode: connected or high_priority.
                      Note that the mode can be specified as both connected and 
                      high_priority by combining the mask values of each in 
                      these two bits.\n
       - bit[5:0]  -- coex_tech_type; the COEX technology type
  */

  uint16_t frequency_range_start;
  /**<   WLAN lower bound of RAT's operating frequency range, in MHz times 10;
       this is represented in x10 to provide fractional precision of 1 decimal
       point. Valid range: 0.0 MHz to 65535 MHz. Values: 0 to 65535
	  (2\textsuperscript{16}-1).
   */

  uint16_t frequency_range_stop;
  /**<   WLAN upper bound of RAT's operating frequency range, in MHz times 10;
       this is represented in x10 to provide fractional precision of 1 decimal
       point. Valid range: 0.0 MHz to 65535 MHz. Values: 0 to 65535
	  (2\textsuperscript{16}-1).
   */
}coex_wlan_conflict_band_range_type_v02;  /* Type */
/**
    @}
  */

typedef uint16_t coex_wlan_policy_config_mask_v02;
#define COEX_WLAN_PCM_UNDEF_V02 ((coex_wlan_policy_config_mask_v02)0x0000) /**<  Undefined policy configuration mask for WLAN
   */
#define COEX_WLAN_PCM_ENFORCE_WWAN_RX_IMD_PROTECTION_V02 ((coex_wlan_policy_config_mask_v02)0x0001) /**<  Protect the WWAN receive; apply the static power back-off or block 
       the WLAN transmit based on WLAN RSSI
   */
#define COEX_WLAN_PCM_ENFORCE_DYNAMIC_WLAN_POWER_BACKOFF_V02 ((coex_wlan_policy_config_mask_v02)0x0002) /**<  Dynamically adjust the WLAN transmit power based on the modem Rx 
       de-sensing due to the WLAN transmit
   */
#define COEX_WLAN_PCM_ENFORCE_BT_AFH_UPDATE_V02 ((coex_wlan_policy_config_mask_v02)0x0004) /**<  Update the Bluetooth AFH channels range
   */
#define COEX_WLAN_PCM_ENFORCE_WLAN_CHANNEL_AVOIDANCE_V02 ((coex_wlan_policy_config_mask_v02)0x0008) /**<  WLAN is to carry out channel avoidance
   */
#define COEX_WLAN_PCM_ENFORCE_TDM_V02 ((coex_wlan_policy_config_mask_v02)0x0010) /**<  Run TDM, which is a mechanism to avoid WLAN transmit and receive 
       when the WWAN transmit is active
   */
/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  coex_wwan_conflict_band_range_type_v02 wwan;
  /**<   WWAN band information for this conflict
      */

  coex_wlan_conflict_band_range_type_v02 wlan;
  /**<   WLAN band information for this conflict
  */

  coex_wlan_policy_config_mask_v02 wlan_coex_policy;
  /**<   Action to be taken by WLAN when this conflict is found. Values: \n
      - COEX_WLAN_PCM_UNDEF (0x0000) --  Undefined policy configuration mask for WLAN
  
      - COEX_WLAN_PCM_ENFORCE_WWAN_RX_IMD_PROTECTION (0x0001) --  Protect the WWAN receive; apply the static power back-off or block 
       the WLAN transmit based on WLAN RSSI
  
      - COEX_WLAN_PCM_ENFORCE_DYNAMIC_WLAN_POWER_BACKOFF (0x0002) --  Dynamically adjust the WLAN transmit power based on the modem Rx 
       de-sensing due to the WLAN transmit
  
      - COEX_WLAN_PCM_ENFORCE_BT_AFH_UPDATE (0x0004) --  Update the Bluetooth AFH channels range
  
      - COEX_WLAN_PCM_ENFORCE_WLAN_CHANNEL_AVOIDANCE (0x0008) --  WLAN is to carry out channel avoidance
  
      - COEX_WLAN_PCM_ENFORCE_TDM (0x0010) --  Run TDM, which is a mechanism to avoid WLAN transmit and receive 
       when the WWAN transmit is active
  
 */

  uint8_t wlan_policy_coex_wwan_rx_imd_protection_params;
  /**<   WLAN's WWAN receive IMD protection policy parameters.
       Parameters applying to COEX_WLAN_PCM_ ENFORCE_WWAN_RX_IMD_ PROTECTION policy;
       bit-packed to represent the following fields: \n
         - bit[7]   -- Reserved for future use \n
         - bit[6]   -- Boolean value to enable or disable blocking the WLAN transmit
                       when RSSI is too low \n
         - bit[5:0] -- Static WLAN transmit power limit, in dBm;
                       valid range: 0 dBm to approximately 25 dBm
  */
}coex_conflict_definition_type_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_enums
    @{
  */
typedef enum {
  COEX_CONFLICT_PARAMS_ESTATUS_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  COEX_CONFLICT_PARAMS_REQ_E_SUCCESS_V02 = 0x00, /**<   Conflict parameters request was successful  */
  COEX_CONFLICT_PARAMS_REQ_E_INVALID_ARG_V02 = 0x01, /**<   Victim table offset in the request is out of bounds  */
  COEX_CONFLICT_PARAMS_ESTATUS_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}coex_conflict_params_estatus_v02;
/**
    @}
  */

/** @addtogroup coex_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t request_estatus;
  /**<    Whether the request made was successful; coex_conflict_params_estatus
        gives the error status. Error status values: \n
        - COEX_CONFLICT_PARAMS_REQ_ E_SUCCESS (0x00) --
		   Conflict parameters request was successful\n
		- COEX_CONFLICT_PARAMS_REQ_ E_INVALID_ARG (0x01) --
	       Victim table offset in the request is out of bounds
  */

  uint8_t victim_tbl_offset;
  /**<   Offset in the complete victim table from which the provided table in this 
        message was extracted; this number is equal to the offset in the
        request message. Offset is zero-based.
  */

  uint8_t victim_tbl_complete_size;
  /**<   Size of the complete victim table; used to determine when the
        complete victim table has been sent. If this value is greater than the size of the
        provided victim table upon request, the client can make more requests with a
        larger offset to get more pieces of the complete victim table.
  */

  uint32_t partial_victim_table_len;  /**< Must be set to # of elements in partial_victim_table */
  coex_conflict_definition_type_v02 partial_victim_table[COEX_NV_MSG_VICTIM_TABLE_MAX_ENTRIES_V02];
  /**<    An array of conflicts where each row contains:\n
        - WWAN band information \n
        - WLAN band information \n
        - Action (policy) to be taken by the WLAN when this conflict is found \n
        - WLAN's WWAN receive IMD protection policy parameters
  */
}coex_conflict_params_report_type_v02;  /* Type */
/**
    @}
  */

/** @addtogroup coex_qmi_messages
    @{
  */
/** Response Message; Provides a set of parameters related to processing 
              conflicting WCN and WWAN bands. */
typedef struct {

  /* Optional */
  /*  Conflict Parameters Report */
  uint8_t report_valid;  /**< Must be set to true if report is being passed */
  coex_conflict_params_report_type_v02 report;
  /**<   \n
       (Current conflict parameter(s) report.) 
  */
}qmi_coex_conflict_params_resp_msg_v02;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_COEX_CONFLICT_PARAMS_REQ_V02 
//#define REMOVE_QMI_COEX_CONFLICT_PARAMS_RESP_V02 
//#define REMOVE_QMI_COEX_DIVERSITY_ANTENNA_STATE_IND_V02 
//#define REMOVE_QMI_COEX_METRICS_LTE_SINR_READ_REQ_V02 
//#define REMOVE_QMI_COEX_METRICS_LTE_SINR_READ_RESP_V02 
//#define REMOVE_QMI_COEX_METRICS_LTE_SINR_START_IND_V02 
//#define REMOVE_QMI_COEX_METRICS_LTE_SINR_STOP_IND_V02 
//#define REMOVE_QMI_COEX_TECH_STATE_UPDATE_REQ_V02 
//#define REMOVE_QMI_COEX_TECH_SYNC_REQ_V02 
//#define REMOVE_QMI_COEX_TECH_SYNC_RESP_V02 
//#define REMOVE_QMI_COEX_TECH_SYNC_UPDATE_IND_V02 
//#define REMOVE_QMI_COEX_WLAN_BT_STATE_IND_V02 
//#define REMOVE_QMI_COEX_WLAN_WIFI_STATE_IND_V02 
//#define REMOVE_QMI_COEX_WWAN_GSM2_STATE_IND_V02 
//#define REMOVE_QMI_COEX_WWAN_GSM3_STATE_IND_V02 
//#define REMOVE_QMI_COEX_WWAN_GSM_STATE_IND_V02 
//#define REMOVE_QMI_COEX_WWAN_HDR_STATE_IND_V02 
//#define REMOVE_QMI_COEX_WWAN_LTE_STATE_IND_V02 
//#define REMOVE_QMI_COEX_WWAN_ONEX_STATE_IND_V02 
//#define REMOVE_QMI_COEX_WWAN_TDSCDMA_STATE_IND_V02 
//#define REMOVE_QMI_COEX_WWAN_WCDMA_STATE_IND_V02 

/*Service Message Definition*/
/** @addtogroup coex_qmi_msg_ids
    @{
  */
#define QMI_COEX_TECH_SYNC_REQ_V02 0x0020
#define QMI_COEX_TECH_SYNC_RESP_V02 0x0021
#define QMI_COEX_TECH_SYNC_UPDATE_IND_V02 0x0022
#define QMI_COEX_TECH_STATE_UPDATE_REQ_V02 0x0023
#define QMI_COEX_WWAN_LTE_STATE_IND_V02 0x0024
#define QMI_COEX_WWAN_TDSCDMA_STATE_IND_V02 0x0025
#define QMI_COEX_WWAN_GSM_STATE_IND_V02 0x0026
#define QMI_COEX_WWAN_ONEX_STATE_IND_V02 0x0027
#define QMI_COEX_WWAN_HDR_STATE_IND_V02 0x0028
#define QMI_COEX_WWAN_WCDMA_STATE_IND_V02 0x0029
#define QMI_COEX_WLAN_WIFI_STATE_IND_V02 0x002A
#define QMI_COEX_WLAN_BT_STATE_IND_V02 0x002B
#define QMI_COEX_DIVERSITY_ANTENNA_STATE_IND_V02 0x002C
#define QMI_COEX_METRICS_LTE_SINR_START_IND_V02 0x002D
#define QMI_COEX_METRICS_LTE_SINR_READ_REQ_V02 0x002E
#define QMI_COEX_METRICS_LTE_SINR_READ_RESP_V02 0x002F
#define QMI_COEX_METRICS_LTE_SINR_STOP_IND_V02 0x0030
#define QMI_COEX_WWAN_GSM2_STATE_IND_V02 0x0031
#define QMI_COEX_WWAN_GSM3_STATE_IND_V02 0x0032
#define QMI_COEX_CONFLICT_PARAMS_REQ_V02 0x0033
#define QMI_COEX_CONFLICT_PARAMS_RESP_V02 0x0034
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro coex_get_service_object_v02( ) that takes in no arguments. */
qmi_idl_service_object_type coex_get_service_object_internal_v02
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define coex_get_service_object_v02( ) \
          coex_get_service_object_internal_v02( \
            COEX_V02_IDL_MAJOR_VERS, COEX_V02_IDL_MINOR_VERS, \
            COEX_V02_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

