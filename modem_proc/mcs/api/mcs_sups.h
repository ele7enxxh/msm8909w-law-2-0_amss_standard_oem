#ifndef MCS_SUPS_H
#define MCS_SUPS_H
/**===========================================================================
@FILE_DOC

                                 MCS _ SUPS

                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for building
  supplementary string necessary for call

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

/* <EJECT> */
/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: 
  $Header: 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/22/12   ejv     Moved to MCS.
11/11/11    ar	   Initial revision.

===========================================================================*/

/*===========================================================================
	
							   INCLUDE FILES
	
===========================================================================*/

#include <IxErrno.h>
#include <comdef.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define MCS_SUPS_MAX_INT_DIGITS        10
#define MCS_SUPS_MAX_SUPS_LENGTH       81
typedef uint16 mcs_sups_class_mask_type;

/**==============================================================================

            Bitmasks for the class values for supplimentary services
            Taken from 3GPP spec 27.007
==============================================================================*/
 
#define  MCS_SUPS_CLASS_MIN_MASK        ((mcs_sups_class_mask_type) 0)
                 
#define  MCS_SUPS_CLASS_VOICE_MASK      ( (mcs_sups_class_mask_type) (1<<0) )
                 
#define  MCS_SUPS_CLASS_DATA_MASK       ( (mcs_sups_class_mask_type) (1<<1) )
                 
#define  MCS_SUPS_CLASS_FAX_MASK        ( (mcs_sups_class_mask_type) (1<<2) )
                 
#define  MCS_SUPS_CLASS_SMS_MASK        ( (mcs_sups_class_mask_type) (1<<3) )
                 
#define  MCS_SUPS_CLASS_DATA_SYNC_MASK  ( (mcs_sups_class_mask_type) (1<<4) )
                 
#define  MCS_SUPS_CLASS_DATA_ASYNC_MASK ( (mcs_sups_class_mask_type) (1<<5) )
                 
#define  MCS_SUPS_CLASS_DATA_PKT_MASK   ( (mcs_sups_class_mask_type) (1<<6) )
                 
#define  MCS_SUPS_CLASS_DATA_PAD_MASK   ( (mcs_sups_class_mask_type) (1<<7) )
                 
#define  MCS_SUPS_ALL_TELE_SERV_MASK    ( (mcs_sups_class_mask_type)  MCS_SUPS_CLASS_VOICE_MASK | \
                                                                      MCS_SUPS_CLASS_FAX_MASK | \
                                                                      MCS_SUPS_CLASS_SMS_MASK )
                                                                   
#define  MCS_SUPS_ALL_TELE_SERV_EX_SMS_MASK  ( (mcs_sups_class_mask_type) MCS_SUPS_CLASS_VOICE_MASK | \
                                                                          MCS_SUPS_CLASS_FAX_MASK)
                                                     
#define  MCS_SUPS_CLASS_ALL_TS_DATA_MASK  ( (mcs_sups_class_mask_type)  MCS_SUPS_CLASS_FAX_MASK | \
                                                                        MCS_SUPS_CLASS_SMS_MASK)
                                             
#define  MCS_SUPS_CLASS_ALL_DATA_SYNC_MASK ( (mcs_sups_class_mask_type)  MCS_SUPS_CLASS_DATA_SYNC_MASK | \
                                                                         MCS_SUPS_CLASS_DATA_PKT_MASK)

#define  MCS_SUPS_CLASS_ALL_DATA_ASYNC_MASK ( (mcs_sups_class_mask_type)  MCS_SUPS_CLASS_DATA_ASYNC_MASK | \
                                                                          MCS_SUPS_CLASS_DATA_PAD_MASK)

#define  MCS_SUPS_CLASS_ALL_DATA_SYNC_ASYNC_MASK ( (mcs_sups_class_mask_type)  MCS_SUPS_CLASS_ALL_DATA_SYNC_MASK | \
                                                                               MCS_SUPS_CLASS_ALL_DATA_ASYNC_MASK)

#define  MCS_SUPS_CLASS_ALL_DATA_PDS_MASK  ( (mcs_sups_class_mask_type)  MCS_SUPS_CLASS_ALL_DATA_SYNC_MASK | \
                                                                         MCS_SUPS_CLASS_VOICE_MASK)

#define  MCS_SUPS_CLASS_DATA_ALL_MASK  ( (mcs_sups_class_mask_type)  MCS_SUPS_CLASS_ALL_DATA_SYNC_MASK | \
                                                                     MCS_SUPS_CLASS_ALL_DATA_ASYNC_MASK)

#define  MCS_SUPS_CLASS_ALL_MASK  ( (mcs_sups_class_mask_type)  MCS_SUPS_CLASS_DATA_ALL_MASK | \
                                                                MCS_SUPS_ALL_TELE_SERV_MASK | \
                                                                MCS_SUPS_CLASS_DATA_MASK )


typedef enum 
{
  MCS_SUPS_MODE_DISABLE    = 0,
  MCS_SUPS_MODE_ENABLE     = 1,
  MCS_SUPS_MODE_QUERY      = 2,
  MCS_SUPS_MODE_REG        = 3,
  MCS_SUPS_MODE_ERASURE    = 4,
  MCS_SUPS_MODE_REG_PASSWD = 5,
  MCS_SUPS_MODE_MAX
} mcs_sups_mode_e_type;

typedef enum {
       MCS_SUPS_ALL                     =   0x00,
       MCS_SUPS_ALLLINEIDENTIFICATIONSS =   0x10, /* all_line_identification */
       MCS_SUPS_CLIP                    =   0x11, /* calling_line_identification_
                                                     presentation_service */
       MCS_SUPS_CLIR                    =   0x12, /* calling_line_identification_
                                                     restriction_service
                                                     presentation_service */
       MCS_SUPS_COLP                    =   0x13, /* connected_line_identification_
                                                     presentation_service */
       MCS_SUPS_COLR                    =   0x14, /* connected_line identification_
                                                     restriction_service */
       MCS_SUPS_CNAP                    =   0x19, /* call name identification presentation */
       MCS_SUPS_MCI                     =   0x1a, /* malicious call identify */
       MCS_SUPS_ALLFORWARDINGSS         =   0x20, /* all_call_forwarding */
       MCS_SUPS_CFU                     =   0x21, /* call_forwarding_unconditional */
       MCS_SUPS_CD                      =   0x24, /* call deflection */
       MCS_SUPS_ALLCONDFORWARDINGSS     =   0x28, /* all conditional call forwarding  */
       MCS_SUPS_CFB                     =   0x29, /* call_forwarding_on_mobile_sub-
                                                     scriber_busy */
       MCS_SUPS_CFNRY                   =   0x2a, /* call_forwarding_on_no_reply */
       MCS_SUPS_CFNRC                   =   0x2b, /* call_forwarding_on_mobile_subsc-
                                                     riber_unreachable */
       MCS_SUPS_ALLCALLOFFERINGSS       =   0x30, /* all call offering sub-services*/
       MCS_SUPS_ECT                     =   0x31, /* call transfer */
       MCS_SUPS_MAH                     =   0x32, /* mobile access hunting */
       MCS_SUPS_ALLCALLCOMPLETIONSS     =   0x40, /* all call completion */
       MCS_SUPS_CW                      =   0x41, /* call waiting */
       MCS_SUPS_HOLD                    =   0x42,
       MCS_SUPS_CCBS                    =   0x43, /* completion of call to busy 
                                                     subscribers */
       MCS_SUPS_ALLMULTIPARTYSS         =   0x50, /* all multi-party services */
       MCS_SUPS_MULTIPTY                =   0x51, /* multi_party_service */
       MCS_SUPS_ALLCOMMUNITYOFINTEREST_SS = 0x60, /* *** NB name deviates from 
                                                     09.02 ***/
       MCS_SUPS_CUG                     =   0x61, /* closed_user_group */
       MCS_SUPS_ALLCHARGINGSS           =   0x70,
       MCS_SUPS_AOCI                    =   0x71, /* advice_of_charge_information */
       MCS_SUPS_AOCC                    =   0x72, /* advice_of_charge_charge */
       MCS_SUPS_ALLADDITIONALINFOTRANSFERSS = 0x80,
       MCS_SUPS_UUS                     =   0x81, /* user to user signalling */
       MCS_SUPS_ALLCALLRESTRICTIONSS    =   0x90,
       MCS_SUPS_BARRINGOFOUTGOINGCALLS  =   0x91,
       MCS_SUPS_BAOC                    =   0x92, /* barring_of_all_outgoing_calls */
       MCS_SUPS_BOIC                    =   0x93, /* barring_of_outgoing_inter-
                                                     national_calls */
       MCS_SUPS_BOICEXHC                =   0x94, /* barring_of_outgoing_inter-
                                                     national_calls_except_those_
                                                     to_home_plmn */
       MCS_SUPS_BARRINGOFINCOMINGCALLS  =   0x99,
       MCS_SUPS_BAIC                    =   0x9a, /* barring of all incoming calls */
       MCS_SUPS_BICROAM                 =   0x9b, /* barring of incomming calls when
                                                     roaming outside home PLMN 
                                                     Country */
       MCS_SUPS_ALLPLMN_SPECIFICSS      =   0xf0, /* all PLMN specific Supplementary
                                                     services *** NB name deviates
                                                     from 09.02 ***/
       MCS_SUPS_CHARGINGINFOID          =   0xa1
} mcs_sups_opcode_e_type;

typedef struct 
{
  char *number;       /* call forwarding number */
  int   nr_timer;    /*no reply timer*/
} mcs_sups_reg_data_s_type;

/* Password information during password change or barring related sups */
typedef struct 
{
  char *old_passwd;
  char *new_passwd;
  char *new_passwd_again;
} mcs_sups_change_passwd_s_type;

typedef struct 
{
  mcs_sups_mode_e_type             mode;            /* Indicates enable/disable etc */
  mcs_sups_opcode_e_type           code;            /* Sups network service code */
  mcs_sups_class_mask_type         service_class;   /* Service class as per 27.007 */
  union
  {
    mcs_sups_reg_data_s_type         reg;             /* Registration data */
    mcs_sups_change_passwd_s_type    passwd;          /* Password information */
  } cmd;
} mcs_sups_params_s_type;

/* Holds MMI Service code and corresponding Network service code */
typedef struct 
{
  char                    *mmi_sc;      /* MMI value of Service Code */
  mcs_sups_opcode_e_type   net_sc;      /* Network value of Service Code */
} mcs_sups_sc_tbl_s_type;


/* Holds MMI BSG code and corresponding Service class */
typedef struct 
{
  char                     *mmi_bsg;         /* MMI value of Basic Service Group */
  mcs_sups_class_mask_type  service_class;   /* Service Class as per 27.007 */
} mcs_sups_bsg_tbl_s_type;


typedef struct 
{
  char                 *sups_mode_str;   /* Sups operation type string */
  mcs_sups_mode_e_type  sups_mode;       /* Indicates enable/disable etc */
} mcs_sups_mode_tbl_s_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern errno_enum_type mcs_sups_build_string
(
  const mcs_sups_params_s_type *ss_params,
  char                         *buffer,
  uint8                         max_len
);

#endif /* MCS_SUPS_H */
