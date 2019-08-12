#ifndef RRCASN1DEF_H
#define RRCASN1DEF_H

/*==========================================================================*/

#define          rrc_DL_DCCH_Message_PDU 1
#define          rrc_UL_DCCH_Message_PDU 2
#define          rrc_DL_CCCH_Message_PDU 3
#define          rrc_UL_CCCH_Message_PDU 4
#define          rrc_PCCH_Message_PDU 5
#define          rrc_DL_SHCCH_Message_PDU 6
#define          rrc_UL_SHCCH_Message_PDU 7
#define          rrc_BCCH_FACH_Message_PDU 8
#define          rrc_BCCH_BCH_Message_PDU 9
#define          rrc_MCCH_Message_PDU 10
#define          rrc_MSCH_Message_PDU 11
#define          rrc_HandoverToUTRANCommand_PDU 12
#define          rrc_InterRATHandoverInfo_PDU 13
#define          rrc_InterRATHandoverInfo_r3_add_ext_IEs_PDU 14
#define          rrc_RRCConnectionSetupComplete_r3_add_ext_IEs_PDU 15
#define          rrc_RRC_FailureInfo_PDU 16
#define          rrc_UECapabilityInformation_r3_add_ext_IEs_PDU 17
#define          rrc_UE_CapabilityContainer_IEs_PDU 18
#define          rrc_UE_RadioAccessCapabilityInfo_PDU 19
#define          rrc_UL_PhysChCapabilityFDD_r6_PDU 20
#define          rrc_GSMCellGroupingList_PDU 21
#define          rrc_UE_Positioning_GANSS_Data_PDU 22
#define          rrc_MasterInformationBlock_PDU 23
#define          rrc_SysInfoType1_PDU 24
#define          rrc_SysInfoType2_PDU 25
#define          rrc_SysInfoType3_PDU 26
#define          rrc_SysInfoType4_PDU 27
#define          rrc_SysInfoType5bis_PDU 28
#define          rrc_SysInfoType6_PDU 29
#define          rrc_SysInfoType7_PDU 30
#define          rrc_SysInfoType8_PDU 31
#define          rrc_SysInfoType9_PDU 32
#define          rrc_SysInfoType10_PDU 33
#define          rrc_SysInfoType11_PDU 34
#define          rrc_SysInfoType11bis_PDU 35
#define          rrc_SysInfoType12_PDU 36
#define          rrc_SysInfoType13_PDU 37
#define          rrc_SysInfoType13_1_PDU 38
#define          rrc_SysInfoType13_2_PDU 39
#define          rrc_SysInfoType13_3_PDU 40
#define          rrc_SysInfoType13_4_PDU 41
#define          rrc_SysInfoType14_PDU 42
#define          rrc_SysInfoType15_PDU 43
#define          rrc_SysInfoType15bis_PDU 44
#define          rrc_SysInfoType15_1_PDU 45
#define          rrc_SysInfoType15_1bis_PDU 46
#define          rrc_SysInfoType15_2_PDU 47
#define          rrc_SysInfoType15_2bis_PDU 48
#define          rrc_SysInfoType15_2ter_PDU 49
#define          rrc_SysInfoType15_3_PDU 50
#define          rrc_SysInfoType15_3bis_PDU 51
#define          rrc_SysInfoType15_4_PDU 52
#define          rrc_SysInfoType15_5_PDU 53
#define          rrc_SysInfoType15_6_PDU 54
#define          rrc_SysInfoType15_7_PDU 55
#define          rrc_SysInfoType15_8_PDU 56
#define          rrc_SysInfoType16_PDU 57
#define          rrc_SysInfoType17_PDU 58
#define          rrc_SysInfoType18_PDU 59
#define          rrc_SysInfoType19_PDU 60
#define          rrc_SysInfoType20_PDU 61
#define          rrc_SysInfoTypeSB1_PDU 62
#define          rrc_SysInfoTypeSB2_PDU 63
#define          rrc_UE_HistoryInformation_PDU 64
#define          rrc_System_Information_Container_PDU 65

#define RRC_GET_BITS_STRING_LEN_PTR(msg) \
	(msg->numbits)

#define RRC_GET_BITS_PTR_STRING_PTR(msg) \
	(msg->data)
	

#define RRC_GET_OCTET_STRING_LEN(msg) \
	(msg.numocts)

#define RRC_GET_OCTET_STRING_PTR(msg) \
	(msg.data)

#define RRC_MSG_LIST_BITMASK_IE(msg,struct_name,tag) \
		((msg)->elem[idx].m.tag##Present)
	
#define RRC_MSG_LIST_BITMASK_IE_TYPE2(msg,tag) \
		(msg->elem[idx].m.tag##Present)

#define RRC_CHECK_LIST_MSG_TYPE(msg,OB_type) \
    (msg->elem[idx].t == T_##OB_type)

#define RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(msg,struct_name,tag) \
	(msg->m.tag##Present)

#define RRC_SIB_TYPE(sib_type)\
(rrc_SIB_Type_##sib_type)

#define RRC_MSG_COMMON_SET_BITMASK_IE(msg,struct_name,tag) \
    (msg.m.tag##Present = 1)

#define RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg,tag) \
		(msg.m.tag##Present = 1)

#define RRC_MSG_COMMON_SET_BITMASK_IE_PTR(msg,struct_name,tag) \
            (msg->m.tag##Present = 1)
    
#define RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2_PTR(msg,tag) \
            (msg->m.tag##Present = 1)

#define RRC_CHECK_COMMON_MSG_TYPE(msg,OB_type) \
    (msg.t == T_##OB_type)

#define RRC_CHECK_COMMON_MSG_TYPE_PTR(msg,OB_type) \
    (msg->t == T_##OB_type)

#define RRC_CHECK_COMMON_MSG_TYPE2(msg,struct_name,OB_type) \
    (msg.t == T_##struct_name##_##OB_type)

#define RRC_CHECK_COMMON_MSG_TYPE2_PTR(msg,struct_name,OB_type) \
    (msg->t == T_##struct_name##_##OB_type)

#define RRC_SET_COMMON_MSG_TYPE(msg,OB_type) \
    (msg.t = T_##OB_type)

#define RRC_SET_COMMON_MSG_TYPE_PTR(msg,OB_type) \
    (msg->t = T_##OB_type)

#define RRC_SET_COMMON_MSG_TYPE2(msg,struct_name,OB_type) \
    (msg.t = T_##struct_name##_##OB_type)

#define RRC_SET_COMMON_MSG_TYPE2_PTR(msg,struct_name,OB_type) \
    (msg->t = T_##struct_name##_##OB_type)


#define RRC_MSG_COMMON_BITMASK_IE_PTR(msg,struct_name,tag) \
	(msg->m.tag##Present)

#define RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(msg,tag) \
	(msg->m.tag##Present)
#define RRC_MSG_COMMON_BITMASK_IE(msg,struct_name,tag) \
		(msg.m.tag##Present)
	
#define RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg,tag) \
		(msg.m.tag##Present)

/* Macros to check presence of I.E's*/
#define RRC_RESET_MSG_IE_PRESENT(msg) \
    (memset(&msg.m, 0, sizeof(msg.m)))

#define RRC_RESET_MSG_IE_PRESENT_PTR(msg) \
    (memset(&msg->m, 0, sizeof(msg->m)))

#define RRC_GET_MSG_TYPE(msg) \
      (msg.t)
      
#define RRC_GET_MSG_TYPE_PTR(msg) \
        (msg->t)

#define RRC_CHECK_MSG_TYPE(msg,OB_type) \
    (msg.t == OB_type)


#define RRC_CHECK_MSG_TYPE_IE(msg, OB_type) \
    (msg.m.OB_type)

#define RRC_CHECK_MSG_TYPE_IE_PTR(msg, OB_type) \
    (msg->m.OB_type)

#define RRCSET_MSG_TYPE(msg, OB_type) \
    (msg.t = OB_type)
    
#define RRCSET_MSG_TYPE_IE_PTR(msg, OB_type) \
    (msg->m.OB_type = 1)


#define RRCSET_R3_MSG_IE_PRESENT(msg,msg_struct_name,msg_bitfield) \
	(msg.m.msg_bitfield##Present = 1)
	
#define RRCSET_R7_MSG_IE_PRESENT(msg,msg_struct_name,msg_bitfield) \
	(msg.m.msg_bitfield##Present = 1)
	
#define RRCGET_PLMN_TYPE_TAG(msg) \
  (T_rrc_PLMN_Type_##msg)

#define RRCGET_PLMN_TYPE(msg) \
    (msg->t)

#define RRCGET_SystemInformation_BCH_payload_TYPE(msg) \
  (T_rrc_SystemInformation_BCH_payload_##msg)

#define RRCGET_SystemInformation_FACH_payload_TYPE(msg) \
  (T_rrc_SystemInformation_FACH_payload_##msg)

#define RRCGET_SIB_OFF_VALUE(msg) \
  (rrc_SibOFF_##msg)

#define RRCGET_SIB_TYPE_AND_TAG_VALUE_SYS_INFO_TYPE(msg) \
  (T_rrc_SIBSb_TypeAndTag_##msg)

#define RRCGET_COMMON_CHOICE_TYPE(msg) \
  (T_##msg)

#define RRCGET_SIB_TYPE_AND_TAG(msg) \
	(msg->t)

#define RRCGET_SCHD_INFO_TAG(msg) \
  (msg->t)

#define RRCGET_SCHD_INFO_SIB_POS(msg) \
  (T_rrc_SchedulingInformation_sib_Pos_##msg)

#define RRCCHECK_MSG_PRESENT(msg,msg_struct_name,msg_bitfield) \
	(msg->t == msg_bitfield##Present)
	
#define RRCCHECK_SIB_TypeExt(msg,value)\
  (msg->t == T_rrc_SIB_TypeExt_##value)

#define RRCCHECK_SIBSb_TypeAndTag(msg,value)\
  (msg->t == T_rrc_SIBSb_TypeAndTag_##value)

#define RRCCHECK_SIB_MSG_IE_PRESENT(msg,msg_struct_name,msg_bitfield) \
	(msg->m.msg_bitfield##Present)

#define RRCCHECK_TIMER_CONST_PRESENT_R5(msg,msg_struct_name,msg_bitfield) 1

#include "wcdma_variation.h"
#ifdef T_WINNT
#error code not present
#else
/* Macros to extract pointers for message I.E's from rrc_DL_DCCH_Message*/
#define RRC_EXTRACT_R3_MSG_POINTER(dcch_msg_ptr, msg_name, msg_ie_struct) \
   (&(dcch_msg_ptr->message.u.msg_name.u.r3.msg_ie_struct))

#define RRC_EXTRACT_R5_MSG_POINTER(dcch_msg_ptr, msg_name, msg_ie_struct) \
   (&(dcch_msg_ptr->message.u.msg_name.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.\
      r5.msg_ie_struct))
#define RRC_EXTRACT_R5_MSG_POINTER_UMI(dcch_msg_ptr, msg_name, msg_ie_struct) \
     (&(dcch_msg_ptr->message.u.msg_name.u.later_than_r3.criticalExtensions.u.\
        r5.msg_ie_struct))

#define RRC_EXTRACT_R6_MSG_POINTER(dcch_msg_ptr, msg_name, msg_ie_struct) \
   (&(dcch_msg_ptr->message.u.msg_name.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.\
      criticalExtensions.u.r6.msg_ie_struct))
      
#define RRC_EXTRACT_R7_MSG_POINTER(dcch_msg_ptr, msg_name, msg_ie_struct) \
   (&(dcch_msg_ptr->message.u.msg_name.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.\
      criticalExtensions.u.criticalExtensions.u.r7.msg_ie_struct))

#define RRC_EXTRACT_R7_MSG_POINTER_UMI_URA(dcch_msg_ptr, msg_name, msg_ie_struct) \
   (&(dcch_msg_ptr->message.u.msg_name.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.\
      r7.msg_ie_struct))
      
#define RRC_EXTRACT_R8_MSG_POINTER(dcch_msg_ptr, msg_name, msg_ie_struct) \
   (&(dcch_msg_ptr->message.u.msg_name.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.\
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.msg_ie_struct))
#ifdef FEATURE_WCDMA_REL9
#define RRC_EXTRACT_R9_MSG_POINTER(dcch_msg_ptr, msg_name, msg_ie_struct) \
	   (&(dcch_msg_ptr->message.u.msg_name.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.\
		  criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.msg_ie_struct))

#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */


#endif
    

      
typedef  OSUINT32 OSS_UINT32;

#endif
