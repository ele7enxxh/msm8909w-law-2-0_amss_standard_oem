/*==============================================================================

                          ds_xml_ANDSF_constants.h

GENERAL DESCRIPTION
  Constants for ANDSF decoder

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_XML_DECODER_ANDSF_CONST_H
#define DS_XML_DECODER_ANDSF_CONST_H

namespace Appsrv
{
namespace XML
{



/* Root elements */
const char ELEM_NAME_ROOT[]                  = "ANDSF";
const char ELEM_NAME_NAME[]                  = "Name";
const char ELEM_NAME_POLICY[]                = "Policy";
const char ELEM_NAME_DISCOVERY_INFORMATION[] = "DiscoveryInformation";
const char ELEM_NAME_UE_LOCATION[]           = "UE_Location";
const char ELEM_NAME_ISRP[]                  = "ISRP";
const char ELEM_NAME_IARP[]                  = "IARP";
const char ELEM_NAME_UE_PROFILE[]            = "UE_Profile";
const char ELEM_NAME_WLANSP[]                = "WLANSP";
const char ELEM_NAME_RULE_SELECTION_INFO[]   = "RuleSelectionInformation";
const char ELEM_NAME_HOME_OPERATOR_PREF[]    = "HomeOperatorPreference";
const char ELEM_NAME_VALIDITY_CRITERIA[]     = "ValidityCriteria";
const char ELEM_NAME_EXT[]                   = "Ext";


/* Common */
const char ELEM_NAME_RULE_PRIORITY[] = "RulePriority";
const char ELEM_NAME_ROAMING[]       = "Roaming";
const char ELEM_NAME_PLMN[]          = "PLMN";
const char ELEM_NAME_UPDATE_POLICY[] = "UpdatePolicy";
const char ELEM_NAME_APN[]           = "APN";


/* Access */
const char ELEM_NAME_PRIORITIZED_ACCESS[]      = "PrioritizedAccess";
const char ELEM_NAME_ROUTING_RULE[]            = "RoutingRule";

const char ELEM_NAME_ACCESS_TECHNOLOGY[]       = "AccessTechnology";
const char ELEM_NAME_ACCESS_ID[]               = "AccessId";
const char ELEM_NAME_SECONDARY_ACCESS_ID[]     = "SecondaryAccessId";
const char ELEM_NAME_ACCESS_NETWORK_PRIORITY[] = "AccessNetworkPriority";
const char ELEM_NAME_APN_PRIORITY[]            = "APNPriority";


/* Validity Area*/
const char ELEM_NAME_VALIDITY_AREA[]       = "ValidityArea";
const char ELEM_NAME_VALIDITY_AREA_REF[]   = "ValidityAreaRef";
const char ELEM_NAME_ACCESS_NETWORK_AREA[] = "AccessNetworkArea";

const char ELEM_NAME_3GPP_LOCATION[]       = "_3GPP_Location";
const char ELEM_NAME_TAC[]                 = "TAC";
const char ELEM_NAME_LAC[]                 = "LAC";
const char ELEM_NAME_GERAN_CI[]            = "GERAN_CI";
const char ELEM_NAME_UTRAN_CI[]            = "UTRAN_CI";
const char ELEM_NAME_EUTRA_CI[]            = "EUTRA_CI";

const char ELEM_NAME_3GPP2_LOCATION[]      = "_3GPP2_Location";
const char ELEM_NAME_1X[]                  = "_1x";
const char ELEM_NAME_SID[]                 = "SID";
const char ELEM_NAME_NID[]                 = "NID";
const char ELEM_NAME_BASE_ID[]             = "Base_ID";
const char ELEM_NAME_HRPD[]                = "HRPD";
const char ELEM_NAME_SECTOR_ID[]           = "Sector_ID";
const char ELEM_NAME_NETMASK[]             = "Netmask";

const char ELEM_NAME_WIMAX_LOCATION[]      = "WiMAX_Location";
const char ELEM_NAME_NAP_ID[]              = "NAP-ID";
const char ELEM_NAME_BS_ID[]               = "BS-ID";

const char ELEM_NAME_WLAN_LOCATION[]       = "WLAN_Location";
const char ELEM_NAME_HESSID[]              = "HESSID";
const char ELEM_NAME_SSID[]                = "SSID";
const char ELEM_NAME_BSSID[]               = "BSSID";

const char ELEM_NAME_GEO_LOCATION[]        = "Geo_Location";
const char ELEM_NAME_CIRCULAR[]            = "Circular";
const char ELEM_NAME_ANCHOR_LATITUDE[]     = "AnchorLatitude";
const char ELEM_NAME_ANCHOR_LONGITUDE[]    = "AnchorLongitude";
const char ELEM_NAME_LATITUDE[]            = "Latitude";
const char ELEM_NAME_LONGITUDE[]           = "Longitude";
const char ELEM_NAME_RADIUS[]              = "Radius";


/* Time of Day */
const char ELEM_NAME_TIME_OF_DAY[]     = "TimeOfDay";
const char ELEM_NAME_TIME_OF_DAY_REF[] = "TimeOfDayRef";
const char ELEM_NAME_TIME_START[]      = "TimeStart";
const char ELEM_NAME_TIME_STOP[]       = "TimeStop";
const char ELEM_NAME_DATE_START[]      = "DateStart";
const char ELEM_NAME_DATE_STOP[]       = "DateStop";
const char ELEM_NAME_DAY_OF_WEEK[]     = "DayOfWeek";


/* Rule types */
const char ELEM_NAME_FLOW_BASED[]           = "ForFlowBased";
const char ELEM_NAME_SERVICE_BASED[]        = "ForServiceBased";
const char ELEM_NAME_NON_SEAMLESS_OFFLOAD[] = "ForNonSeamlessOffload";
const char ELEM_NAME_INTER_APN_ROUTING[]    = "ForInterAPNRouting";
const char ELEM_NAME_FOR_CS_BASED[]         = "ForCSBased";


/* Flow Distribution */
const char ELEM_NAME_IP_FLOW[]                  = "IPFlow";
const char ELEM_NAME_ROUTING_CRITERIA[]         = "RoutingCriteria";
const char ELEM_NAME_API_OVERRIDE[]             = "IsAPIOverride";

const char ELEM_NAME_APP_ID[]                   = "App-ID";
const char ELEM_NAME_OS_APPS[]                  = "OSApps";
const char ELEM_NAME_OS_APP_ID[]                = "OSAppId";

const char ELEM_NAME_ADDRESS_TYPE[]             = "AddressType";
const char ELEM_NAME_START_SOURCE_IP_ADDRESS[]  = "StartSourceIPaddress";
const char ELEM_NAME_END_SOURCE_IP_ADDRESS[]    = "EndSourceIPaddress";
const char ELEM_NAME_START_DEST_IP_ADDRESS[]    = "StartDestIPaddress";
const char ELEM_NAME_END_DEST_IP_ADDRESS[]      = "EndDestIPaddress";
const char ELEM_NAME_PROTOCOL_TYPE[]            = "ProtocolType";
const char ELEM_NAME_START_SOURCE_PORT_NUMBER[] = "StartSourcePortNumber";
const char ELEM_NAME_END_SOURCE_PORT_NUMBER[]   = "EndSourcePortNumber";
const char ELEM_NAME_START_DEST_PORT_NUMBER[]   = "StartDestPortNumber";
const char ELEM_NAME_END_DEST_PORT_NUMBER[]     = "EndDestPortNumber";
const char ELEM_NAME_QOS[]                      = "QoS";
const char ELEM_NAME_DOMAIN_NAME[]              = "DomainName";


/* RAN Validity Condition */
const char ELEM_NAME_RAN_VALIDITY[]          = "RANValidityCondition";
const char ELEM_NAME_RAN_VALIDITY_REF[]      = "RANValidityConditionRef";

const char ELEM_NAME_THRESHOLD_COND[]        = "ThresholdConditions";
const char ELEM_NAME_OPI[]                   = "OPI";

const char ELEM_NAME_THRESHOLD[]             = "Thresholds";
const char ELEM_NAME_THRESHOLD_COND_EVAL[]   = "ThresholdConditionEvaluationPolicy";

const char ELEM_NAME_THRESH_WLAN_LOW_P[]     = "ThreshServingOffloadWLANLowP";
const char ELEM_NAME_THRESH_WLAN_HIGH_P[]    = "ThreshServingOffloadWLANHighP";
const char ELEM_NAME_THRESH_WLAN_LOW_Q[]     = "ThreshServingOffloadWLANHighQ";
const char ELEM_NAME_THRESH_WLAN_HIGH_Q[]    = "ThreshServingOffloadWLANLowQ";

const char ELEM_NAME_THRESH_CH_WLAN_LOW[]    = "ThreshChUtilWLANLow";
const char ELEM_NAME_THRESH_CH_WLAN_HIGH[]   = "ThreshChUtilWLANHigh";
const char ELEM_NAME_THRESH_DL_WLAN_LOW[]    = "ThreshBackhRateDLWLANLow";
const char ELEM_NAME_THRESH_DL_WLAN_HIGH[]   = "ThreshBackhRateDLWLANHigh";
const char ELEM_NAME_THRESH_UL_WLAN_LOW[]    = "ThreshBackhRateULWLANLow";
const char ELEM_NAME_THRESH_UL_WLAN_HIGH[]   = "ThreshBackhRateULWLANHigh";
const char ELEM_NAME_THRESH_RSSI_WLAN_LOW[]  = "ThreshBeaconRSSIWLANLow";
const char ELEM_NAME_THRESH_RSSI_WLAN_HIGH[] = "ThreshBeaconRSSIWLANHigh";


/* Operator Specific RAN Validity Condition */
const char ELEM_NAME_OP_SPECIFIC_RAN_VALIDITY[] = "OPSpecificRANValidityCondition";
const char ELEM_NAME_CALL_CONDITION[]           = "CallConditions";
const char ELEM_NAME_CALLTYPE[]                 = "CallType";
const char ELEM_NAME_CONDITION_EVAL_POLICY[]    = "ConditionEvaluationPolicy";

const char ELEM_NAME_LTE_THRESHOLD[]            = "LTEThreshold";
const char ELEM_NAME_LTE_RSRP_LOW[]             = "LTERSRPLow";
const char ELEM_NAME_LTE_RSRP_HIGH[]            = "LTERSRPHigh";
const char ELEM_NAME_LTE_RSRQ_LOW[]             = "LTERSRQLow";
const char ELEM_NAME_LTE_RSRQ_HIGH[]            = "LTERSRQHigh";
const char ELEM_NAME_LTE_AVAILABLE[]            = "LTEAvailable";

const char ELEM_NAME_WIFI_THRESHOLD[]           = "WiFiThreshold";
const char ELEM_NAME_WIFI_RSSI_LOW[]            = "WiFiRSSILow";
const char ELEM_NAME_WIFI_RSSI_HIGH[]           = "WiFiRSSIHigh";
const char ELEM_NAME_WIFI_SINR_LOW[]            = "WiFiSINRLow";
const char ELEM_NAME_WIFI_SINR_HIGH[]           = "WiFiSINRHigh";
const char ELEM_NAME_WIFI_BSS_LOW[]             = "WiFiBSSLoadLow";
const char ELEM_NAME_WIFI_BSS_HIGH[]            = "WiFiBSSLoadHigh";
const char ELEM_NAME_WIFI_AVAILABLE[]           = "WiFiAvailable";

const char ELEM_NAME_EHRPD_THRESHOLD[]          = "eHRPDThreshold";
const char ELEM_NAME_EHRPD_AVAILABLE[]          = "eHRPDAvailable";


/* ForCSBased */
const char ELEM_NAME_CDMA_1X_CONDITION[]  = "CDMA1xCondition";
const char ELEM_NAME_IS_1X_CS_AVAILABLE[] = "Is1xCSAvailable";
const char ELEM_NAME_CDMA_1X_ECIO_LOW[]   = "CDMA1xECIOLow";
const char ELEM_NAME_CDMA_1X_ECIO_HIGH[]  = "CDMA1xECIOHigh";
const char ELEM_NAME_CDMA_1X_AVAILABLE[]  = "CDMA1xAvailable";


/* Selection Criteria */
const char ELEM_NAME_CRITERIA_PRIORITY[]              = "CriteriaPriority";
const char ELEM_NAME_HOME_NETWORK_INDICATION[]        = "HomeNetworkIndication";
const char ELEM_NAME_PREFERRED_ROAMING_PARTNER_LIST[] = "PreferredRoamingPartnerList";
const char ELEM_NAME_MIN_BACKHAUL_THRESHOLD[]         = "MinBackhaulThreshold";
const char ELEM_NAME_MAXIMUM_BSS_LOAD_VALUE[]         = "MaximumBSSLoadValue";
const char ELEM_NAME_REQUIRED_PROTO_PORT_TUPLE[]      = "RequiredProtoPortTuple";
const char ELEM_NAME_SP_EXCLUSION_LIST[]              = "SPExclusionList";
const char ELEM_NAME_PREFERRED_SSID_LIST[]            = "PreferredSSIDList";


/* Home Operator Preference */
const char ELEM_NAME_PSPL[]                        = "PSPL";
const char ELEM_NAME_S2A_CONNECTIVITY_PREFERENCE[] = "S2aConnectivityPreference";

const char ELEM_NAME_3GPP_RPLMN_PREF[]             = "3GPP_RPLMN_Preferred";
const char ELEM_NAME_EQUIVALENT_HOME_SPS[]         = "EquivalentHomeSPs";
const char ELEM_NAME_PREFERRED_SPS[]               = "PreferredSPs";
const char ELEM_NAME_SERVICE_PROVIDER[]            = "ServiceProvider";
const char ELEM_NAME_PRIORITY[]                    = "Priority";


/* Other */
const char ELEM_NAME_ACCESS_NETWORK_TYPE[] = "AccessNetworkType";
const char ELEM_NAME_ACCESS_NETWORK_REF[]  = "AccessNetworkInformationRef";
const char ELEM_NAME_RPLMN[]               = "RPLMN";
const char ELEM_NAME_SELECTION_CRITERIA[]  = "SelectionCriteria";
const char ELEM_NAME_OSID[]                = "OSId";
const char ELEM_NAME_DEV_CAPABILITY[]      = "DevCapability";
const char ELEM_NAME_VPLMNS_PREF_RULES[]   = "VPLMNswithPreferredRules";
const char ELEM_NAME_VPLMN[]               = "VPLMN";


/* Error messages */
const char ERR_MSG_ROUTING_RULE[] = "Routing rule error";
const char ERR_MSG_UNEXPECTED[]   = "Unexpected Error";


} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_DECODER_ANDSF_CONST_H */
