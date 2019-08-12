#ifndef TM_DIAG_H
#define TM_DIAG_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "diag.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "sm_api.h"
/*===========================================================================

                  TM_DIAG Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for TM Diag
  interface.

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_diag.h#1 $  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/0    jlp     Added diag command to force a dump of various data
                   structures.
06/16/08   jlp     Added handling of SV No Exist list diag command.
02/14/08    jw     Support for Optimistic Punc
06/18/07    jd     Implemented diag support for GPS File System support.
07/12/06    cl     Initial Release
===========================================================================*/

#define TM_DIAG_SESS_REQ_RF_DEV_COMMAND             0
/* TM_DIAG_SESS_REQ_RF_DEV_COMMAND  Sub Commands */
#define TM_DIAG_SESS_CGPS_SUB_COMMAND               10
#define TM_DIAG_SESS_LTE_OTDOA_COMMAND              11

/* DIAG command to enable/disable LTE TTr RF dev test */
#define TM_DIAG_SESS_LTE_RF_DEV_COMMAND             12

/* DIAG command to force hybrid GPS-OTDOA fix. In this mode,
 * only 2 GPS SVs and one OTDOA measurement will be reported */
#define TM_DIAG_SESS_LTE_FORCE_HYBRID_COMMAND       13

/* DIAG command to trigger LOWI scans */
#define TM_DIAG_LOWI_ENABLE_CMD                     14

/* DIAG commands for GTS module */
#define TM_DIAG_SESS_GTS_COMMAND                    15

/* Diag Command for Frequency Estimation Module */
#define TM_DIAG_SESS_FREQEST_COMMAND                16


/* LOWI sub commands */
#define TM_DIAG_LOWI_INIT_SUB_COMMAND  0
#define TM_DIAG_LOWI_DESTROY_SUB_COMMAND 1
#define TM_DIAG_LOWI_DISCOVERY_SCAN_SUB_COMMAND  2
#define TM_DIAG_LOWI_SNOOP_SUBSCRIPTION_SUB_COMMAND 3
#define TM_DIAG_LOWI_SNOOP_SUBSCRIPTION_CANCEL_SUB_COMMAND 4
#define TM_DIAG_LOWI_EVENTS_QUERY_SUB_COMMAND 5
#define TM_DIAG_LOWI_EVENTS_SUBSCRIPTION_SUB_COMMAND 6
#define TM_DIAG_LOWI_EVENTS_SUBSCRIPTION_CANCEL_SUB_COMMAND 7
#define TM_DIAG_LOWI_CAPABILITY_SUB_COMMAND 8

/* Inject position to Position Manager */
#define TM_DIAG_SESS_CGPS_SET_PM_POS_SUB_COMMAND         0
/* Testmode where we force SBAS to start/stop */
#define TM_DIAG_SESS_CGPS_START_SBAS_TEST_SUB_COMMAND    1
#define TM_DIAG_SESS_CGPS_STOP_SBAS_TEST_SUB_COMMAND     2
#define TM_DIAG_SESS_GNSS_NAV_HW_SUB_COMMAND             3

#define TM_DIAG_PORT_CONTROL_COMMAND                102
#define TM_DIAG_SESS_SARF_COMMAND                   0x0065
#define TM_DIAG_SESS_REQ_SW_VER_COMMAND             0x0018
#define TM_DIAG_SV_NO_EXIST_CMD                     0x006A
#define TM_DIAG_FORCE_DPO_FOR_POWER_MEAS_CMD        0x006B
#define TM_DIAG_DATA_DUMP_CMD                       0x006C
#define TM_DIAG_QUERY_BLANKING_CMD                  0x006D
#define TM_DIAG_NAV_CONFIG_CMD                      0x006E
#define TM_DIAG_NAV_SET_TEST_MODE_CMD               0x006F
#define TM_DIAG_NAV_IOCTL_CMD                       0x0070
#define TM_DIAG_QUERY_ADC_IQ_SAT_PER_CMD            0x0071
#define TM_DIAG_FACTORY_IQ_TEST_CMD                 0x0072

#ifdef FEATURE_CGPS_FILE_SYSTEM
#define CGPS_EFS_FILE_HANDLING_F                    0x0069
#endif /* FEATURE_CGPS_FILE_SYSTEM */

/* Deprecated Commands */
#define TM_DIAG_SESS_REQ_START_COMMAND              0x0001
#define TM_DIAG_SESS_REQ_STOP_COMMAND               0x0002
#define TM_DIAG_SESS_REQ_CONTINUE_COMMAND           0x0003
#define TM_DIAG_SESS_POST_DATA_COMMAND              0x0004
#define TM_DIAG_SESS_POLLING_PPM_COMMAND            0x0005
#define TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND     0x0006
/* End of deprecated Commands */

/* Any change to this enum should be reflected in lm_api.h (tm_diag_lm_msg_id_type enum )
   as well */
enum
{
  TM_DIAG_GPS_CMD_CODE_START = 20,
  TM_DIAG_GPS_CMD_CODE_SARF_MODE_SWITCH = TM_DIAG_GPS_CMD_CODE_START, /* 20 */
  TM_DIAG_GPS_CMD_CODE_PRESC_DWELL,                                   /* 21 */
  TM_DIAG_GPS_CMD_CODE_IQTEST,                                        /* 22 */
  TM_DIAG_GPS_CMD_CODE_1SV_STANDALONE,                                /* 23 */
  TM_DIAG_GPS_CMD_CODE_MEM_BW_TEST,                                   /* 24 */
  TM_DIAG_GPS_CMD_CODE_TSG_CMD,                                       /* 25 */
  TM_DIAG_GPS_CMD_CODE_RF_CMD,                                        /* 26 */
  TM_DIAG_GPS_CMD_CODE_RF_DEV_TEST,                                   /* 27 */
  TM_DIAG_GPS_CMD_CODE_PRX_RF_CMD,                                    /* 28 */
  TM_DIAG_GPS_CMD_CODE_RF_LINEARITY_CMD,                              /* 29 */
  /* Query for build's RF Dev feature capabilities*/  
  TM_DIAG_GPS_CMD_CODE_RF_DEV_FEATURE_CAPABILITIES,                   /* 30 */
  TM_DIAG_GPS_CMD_CODE_GET_LATEST_WB_FFT_RESULTS,                     /* 31 */  
  /* Following enums are reserved for OEMs usage */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_1,                                /* 32 */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_2,                                /* 33 */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_3,                                /* 34 */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_4,                                /* 35 */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_5,                                /* 36 */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_6,                                /* 37 */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_7,                                /* 38 */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_8,                                /* 39 */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_9,                                /* 40 */
  TM_DIAG_GPS_CMD_CODE_OEM_RESERVED_10,                               /* 41 */
  TM_DIAG_GPS_CMD_CODE_SPECTRUM_ANALYZER_CMD,                         /* 42*/
  TM_DIAG_GPS_CMD_CODE_LAST
};

#define TM_DIAG_SUPPORTING_DATA_LENGTH   36

/* Definition of Session Control Diag Pkt */
/*============================================================================

PACKET   ARIESDIAG_SESS_CTRL_F

PURPOSE  Sent from the DM to the DMSS with GPS session control commands

RESPONSE The DMSS sends back a ariesDiag_SessCtrlRspType packet

============================================================================*/
typedef PACKED struct PACKED_POST
{
  byte cmd_code;             /* Command code */
  byte gps_control;          /* Subcommand code */
  word data_length;          /* Length of data payload */
  byte supporting_data[TM_DIAG_SUPPORTING_DATA_LENGTH];  /* Supporting data buffer */
} aries_diag_sess_ctrl_req_type;

typedef PACKED struct PACKED_POST
{
  byte  cmd_code;            /* Command code  */
  byte  gps_control;         /* Subcommand code */
} aries_diag_sess_ctrl_rsp_type;

/* Definition of Session Start Diag Pkt */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type   header;
  uint8                        operation_mode;  /* MSA, MSB */
  uint32                       req_op;          /* PPM, PRM, LR, Aiding Data Status */
  uint32                       num_fixes;  
  uint32                       time_between_fixes;
  uint8                        ppm_qos;
  uint8                        prm_qos;  
  uint32                       lr_qos;    
  uint8                        accuracy_threshold;    
} tm_diag_sess_start_req_pkt_type;

typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type header;
} tm_diag_sess_start_rsp_pkt_type;

  
/* Definition of Session Stop Diag Pkt */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type   header;
  uint8                        stop_type;    /* Data, fix */
  uint8                        stop_reason;  
  uint32                       op_to_stop;   /* PPM, PRM, LR, Aiding Data Status */  
} tm_diag_sess_stop_req_pkt_type;

typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type header;
} tm_diag_sess_stop_rsp_pkt_type;

/* Definition of Session Continue Diag Pkt */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type   header;
  uint32                       req_op;         /* PPM, PRM, LR, Aiding Data Status */
  uint8                        ppm_qos;
  uint8                        prm_qos;  
  uint32                       lr_qos;    
  uint8                        accuracy_threshold;
} tm_diag_sess_continue_req_pkt_type;

typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type header;
} tm_diag_sess_continue_rsp_pkt_type;

/* Definition of Post Data Diag Pkt */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type   header;
  uint8                        data_type;
  uint8                        data_length;
  byte                         supporting_data[32];   /* Supporting data buffer */  
} tm_diag_sess_post_data_req_pkt_type;

typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type header;
} tm_diag_sess_post_data_rsp_pkt_type;

/* Definition of Polling PPM Diag Pkt */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type   header;
} tm_diag_sess_polling_ppm_req_pkt_type;

typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type header;
} tm_diag_sess_polling_ppm_rsp_pkt_type;

/* Definition of Delete Aiding Data Diag Pkt */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type   header;
  uint32                       del_clock_mask;
  uint32                       del_pos_mask;
  uint32                       del_eph_mask;
  uint32                       del_alm_mask;
  uint32                       del_iono_mask;
  uint32                       del_sv_dir_mask;
  uint32                       del_sv_steer_mask;
  uint32                       del_sv_health_mask;
  uint32                       del_aa_data_mask;
  uint32                       del_rti_mask;  
} tm_diag_delete_aiding_data_req_pkt_type;

typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type header;
} tm_diag_delete_aiding_data_rsp_pkt_type;

/* Definition of Delete Aiding Data Diag Pkt */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type   header;
} tm_diag_sess_req_sw_ver_req_pkt_type;

typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type header;
  uint8  pkt_ver;
  uint8  cgps_major_sw_version;
  uint8  cgps_minor_sw_version;
  uint8  cgps_build_sw_version;
  uint8  cgps_comm_state;
  uint8  cgps_sess_state;
  uint8  cgps_wireless_mode;
  uint8  cgps_pdapi_rev_id;
} tm_diag_sess_req_sw_ver_rsp_pkt_type;

typedef enum
{
  TM_DIAG_CMD_ACCEPTED = 0,
  TM_DIAG_CMD_REJECTED
} tm_diag_rsp_err_status;

typedef PACKED struct PACKED_POST
{
  uint8              u_CmdCode;        /* Command Code */
  uint8              u_SubsysId;       /* Subsystem Id */
  uint16             w_SubsysCmdCode;  /* Subsystem Command Code */
  uint8              u_GpsCmdCode;     /* GPS Command Code */
} tm_diag_sarf_cmd_common_type; 

typedef PACKED struct PACKED_POST
{
  tm_diag_sarf_cmd_common_type z_DiagCmdCommon;     /* diagpkt_subsys_header_type */
  uint8              u_Status;         /* See ENUM gpsdiag_eStatusType */
} tm_diag_sarf_common_rsp_pkt_type;

typedef PACKED struct PACKED_POST
{
  tm_diag_sarf_cmd_common_type z_DiagCmdCommon;     /* diagpkt_subsys_header_type */
  /*RF Dev feature capabilities of current SW - see TM_DIAG_RFDEV_FEATURE_NUM */
  uint32              q_RfDevFeatureEnum;         
} tm_diag_sarf_rfdevfeature_rsp_pkt_type;

typedef PACKED struct PACKED_POST
{
  tm_diag_sarf_cmd_common_type z_DiagCmdCommon; /* diagpkt_subsys_header_type */
  uint8               u_Ver;                /* Version number */
  uint8               u_Cnt;                /* Counter of how many wbiq info data 
                                               has been updated */
  uint32              q_GpsCnoP1DBHz;       /* C/No reported in units of 0.1 dB-Hz */
  int32               l_GpsFreqHz;          /* freq reported in Hz */
  uint32              q_GloCnoP1DBHz;       /* C/No reported in units of 0.1 dB-Hz */
  int32               l_GloFreqHz;          /* freq reported in Hz */
  uint32              q_BdsCnoP1DBHz;       /* C/No reported in units of 0.1 dB-Hz */
  int32               l_BdsFreqHz;          /* freq reported in Hz */
  uint32              q_GalCnoP1DBHz;       /* C/No reported in units of 0.1 dB-Hz */
  int32               l_GalFreqHz;          /* freq reported in Hz */
  uint8               u_GnssConfig;         /* Flags containing configured 
                                               constellations */
  int32               l_AdcMeanIp1mV;       /* Mean Estimate (DC) of I component in 
                                               ADC processor in 0.1 mV */
  int32               l_AdcMeanQp1mV;       /* Mean Estimate (DC) of Q component in 
                                               ADC processor in 0.1 mV */
  uint32              q_AdcAmpIp1mV;        /* Amplitude estimate of I component in 
                                               ADC processor in 0.1 mV */
  uint32              q_AdcAmpQp1mV;        /* Amplitude estimate of Q component in 
                                               ADC processor in 0.1 mV */
} tm_diag_sarf_wbiq_info_rsp_pkt_type;

/* All SARF cmds have payload starting from the same offset */
typedef PACKED struct PACKED_POST
{
  tm_diag_sarf_cmd_common_type z_DiagCmdCommon;     /* diagpkt_subsys_header_type */
  uint8 u_data[1];
} tm_diag_sarf_cmds_common_req_pkt_type;

/* Definition of NMEA port command */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type   header;
  uint8                        service_type;      /* 0 -> NMEA , 1 -> Serial PDAPI */
  uint8                        operation_type;    /* 0 -> Close Port, 1 -> Open Port */
  uint8                        port;              /* 0 -> UART1, 1 -> UART2, 2 -> USB, 3 -> Shared Memory */
  uint32                       reserved;
} tm_diag_port_control_req_pkt_type;

typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type header;
  uint8                      result;   
} tm_diag_port_control_rsp_pkt_type;

#ifdef FEATURE_CGPS_FILE_SYSTEM
/* (essentially a more detailed definition of 'diagpkt_subsys_header_type' */
typedef PACKED struct PACKED_POST
{
  uint8              u_CmdCode;            /* Command Code */
  uint8              u_SubsysId;           /* Subsystem Id */
  uint16             w_SubsysCmdCode;      /* Subsystem Command Code */
  uint32             q_SubsysStatus;       /* Subsystem Status */
  uint16             w_SubsysDelayedRspId; /* Subsystem Delayed Response Id */
  uint16             w_SubsysRspCnt;       /* Subsystem Response Count */
} gpsdiag_CmdHdrType_v2; 

/* gpsdiag_EFS */
#define C_GPSDIAG_EFS_FILE_HANDLING_RESERVED_BYTES  4
#define C_GPSDIAG_MAX_FULL_FILENAME_LENGTH          255
#define C_GPSDIAG_MAX_ALLOWED_DIAG_PKT_TX_SIZE      0x800
#define C_GPSDIAG_DIAGPKT_OPAQUE_HEADER_SIZE        4

enum
{
  GPSDIAG_EFS_FILE_REQ_SUCCESS,
  GPSDIAG_EFS_FILE_REQ_INVALID_OPERATION,
  GPSDIAG_EFS_FILE_REQ_INVALID_FILE_SIZE,
  GPSDIAG_EFS_FILE_REQ_CMD_BUF_ALLOC_FAIL,
  GPSDIAG_EFS_FILE_REQ_INVALID_DIR,
  GPSDIAG_EFS_FILE_REQ_INVALID_FILENAME_LENGTH,
  GPSDIAG_EFS_FILE_REQ_FAIL_PKT_DATA_SIZE_MISMATCH,
  GPSDIAG_EFS_FILE_REQ_FAIL_PKT_LENGTH
};

enum
{
  GPS_FS_NVS_FILE_RD_STAT_NONE,
  GPS_FS_NVS_FILE_RD_STAT_UNKNOWN_FILE_LEN,
  GPS_FS_NVS_FILE_RD_STAT_FILE_OPEN_ERR,
  GPS_FS_NVS_FILE_RD_STAT_FILE_RD_ERR
};

/* Fixed field size = 14 (4 byte header; 4 reserved bytes; 4 byte filesize;
   1 byte file operation; 1 byte file length */
#define C_GPSDIAG_EFS_FILE_HANDLING_FIXED_FIELD_SIZE  14

/* Max allowed diag pkt size - fixed field size */
#define C_GPSDIAG_EFS_MAX_PAYLOAD                     2034

/* Typical command header for all Req/Rsp packets */
/* (essentially a more detailed definition of 'diagpkt_subsys_header_type' */
typedef PACKED struct PACKED_POST
{
  uint8              u_CmdCode;       /* Command Code */
  uint8              u_SubsysId;      /* Subsystem Id */
  uint16             w_SubsysCmdCode; /* Subsystem Command Code */
} gpsdiag_CmdHdrType;

/* Request packet */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint8              u_FileOperation;
  uint8              u_FullPathAndFileNameLength;
  uint8              u_Reserved[C_GPSDIAG_EFS_FILE_HANDLING_RESERVED_BYTES];
  uint32             q_FileSize;
  char               FilePayload[C_GPSDIAG_EFS_MAX_PAYLOAD];
} gpsdiag_EfsFileSysReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;
} gpsdiag_EfsFileSysRspType;
#endif /* FEATURE_CGPS_FILE_SYSTEM */

enum
{
  GPSDIAG_NOEXIST_ACTION_SET_TO_MASK = 0  /* Command to set No Exist List to mask */
};

enum
{
  GPSDIAG_NOEXIST_STATUS_SUCCESS = 0,     /* Action successful response */
  GPSDIAG_NOEXIST_STATUS_FAIL,            /* Action failed */
  GPSDIAG_NOEXIST_STATUS_TOO_MANY_SVS,    /* Action failed due to too many Sv's in the list */
  GPSDIAG_NOEXIST_STATUS_INVALID_ACTION   /* Action was invalid */
};

/* Definition of No Exist List command */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type z_header;
  uint32 q_SvNoExistMask;             /* Mask of Svs to change in No Exist list */
  uint8 u_Action;                     /* Action to perform */
} gpsdiag_SvNoExistReqPktType;

/* Definition of No Exist List response */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;
} gpsdiag_SvNoExistRspPktType;

/* Definition of Force DPO for Power Measurements request command */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type z_header;
  uint8 u_ForceDpoForPowerMeas;     /* TRUE when DPO is forced for Power measurements */
} gpsdiag_ForceDpoForPowerMeasReqPktType;

/* Definition of Power Measurement Response type */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;
} gpsdiag_ForceDpoForPowerMeasRspPktType;

/* Definition of request Data Dump Diag command */
typedef PACKED struct PACKED_POST
{
  diagpkt_subsys_header_type z_DiagCmdHdr;
  uint8 u_Version;
  uint32 q_ReqDataBitMask;
} gpsdiag_DataDumpReqType;

/* Definition of response to Data Dump diag command */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;    /* diagpkt_subsys_header_type */
} gpsdiag_DataDumpRspType;

/* Definition of blanking query command */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;         /* diagpkt_subsys_header_type */  
} gpsdiag_QueryBlankingReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;        /* diagpkt_subsys_header_type */
  boolean            b_Enabled;           /* Blanking config */
  uint32             q_Rx1Counter;        /* Blanking counter for Rx1 */
  uint32             q_Rx2Counter;        /* Blanking counter for Rx2 */
} gpsdiag_QueryBlankingRspType;

/* Definition of Nav config command */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;         /* diagpkt_subsys_header_type */ 
  uint8                 u_GnssDesiredConfig;    /* flags containing constellations 
                                                   desired, uses the 
                                                   C_RCVR_GNSS_CONFIG_XXX defines */
} gpsdiag_NavConfigReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;        /* diagpkt_subsys_header_type */
  uint8                 u_GnssSupportedConfig; /* flags containing constellations 
                                                   supported by HW, uses the 
                                                   C_RCVR_GNSS_CONFIG_XXX defines */
  uint8                 u_GnssActualConfig;    /* flags containing constellations 
                                                   actually configured, uses the 
                                                   C_RCVR_GNSS_CONFIG_XXX defines */
} gpsdiag_NavConfigRspType;

/* Definition of Nav set PGA backoff command */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;         /* diagpkt_subsys_header_type */ 
  uint8               u_Cmd;                   /*  Test mode config
                                                   0 - exit test mode    
                                                   1 - backoff PGA
                                                   2 - restore PGA   */
} gpsdiag_NavSetTestModeReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;        /* diagpkt_subsys_header_type */
} gpsdiag_NavSetTestModeRspType;


/* Definition of Nav Ioctl command */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;         /* diagpkt_subsys_header_type */ 
  uint8 u_RequestCode;                          /*Ioctl request code*/
  int32 l_RequestParam1;
  int32 l_RequestParam2; 
} gpsdiag_NavIoctlReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;        /* diagpkt_subsys_header_type */
  uint8 u_RequestCode;                         /*Ioctl request code*/
  int32 l_ResponseParam1;
  int32 l_ResponseParam2;
} gpsdiag_NavIoctlRspType;


/* Definition of ADC IQ saturation percentage command  */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;         /* diagpkt_subsys_header_type */
} gpsdiag_QueryAdcIqSatPerReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;        /* diagpkt_subsys_header_type */
  uint8                 u_AdcIqSatPerStatus;   /* Indicates validity/status of data */
  uint32                q_ISatPer;             /* Percentage of I samples saturated */
  uint32                q_QSatPer;             /* Percentage of Q samples saturated */
} gpsdiag_QueryAdcIqSatPerRspType;

/* Definition for Factory I/Q Test request and responses (Immediate and Delayed) */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2; /* diagpkt_subsys_header_type */
  uint8  u_Version; /* Min value of 54 */
  uint16 w_Flags; /* Flags:
                     Bit 1: Enable on-board FFT logging (generates 0x138A log packet)
                     Bit 2: Enable Hanning window for on-board FFT
                     Bit 4: Enable CN0 measurement for a single constellation
                     Bit 5: Enables CN0 measurement for all enabled constellations with a single command
                     Bit 6: Automatically exit Standalone RF (SARF) mode upon test completion
                     Bit 7: Enable wideband jammer power estimation for ADC IQ */
  uint8  u_CollectMode; /* I/Q Test Collect Mode:
                           1- WBIQ BP1 (2.046 MHz sample rate GPS)
                           3 - WBIQ BP3 (1.022 MHz sample rate GLO)
                           16 - ADC IQ (Full ADC sample rate ~82 MHz)
                           17 - WBIQ BP2 (4 MHz sample rate BDS)
                           18 - WBIQ BP1 (4 MHz sample rate GAL) */
  int8   b_GloR1Chan; /* Applicable when CollectMode equals 3.
                         Defines one of 14 possible GLO R1 frequency bands to use.
                         -7 through 6 – 14 possible GLONASS R1 frequencies starting from 1598.0625 
                         and increasing in steps of 562.5 KHz*/
  uint8  u_GloHWChan; /* Applicable when CollectMode equals 3. 
                         Defines one of 12 BP3 hardware channels to be used in the IQ Test.
                         1 through 12 – 12 possible GLO hardware channels */
  uint16 w_SampleCnt1k; /* Number of samples (in multiples of 1024) collected per incoherent integration.
                           1,2,4,8,16, or 32 – power of 2 with a max of 32 */
  uint8  u_Integrations; /* Number of incoherent integrations, min value of 1 */
  uint32 q_ADCIqSatThresh; /* Applicable when CollectMode equals 16. ADC IQ saturation threshold 
                              specified in units of 0.01%.
                              Range 0 to 10000, e.g. 9950 for a saturation threshold of 99.5% */
  uint32 q_AdcIQWidebandJammerStartFreqHz;  /* Applicable when CollectMode equals 16.
                                               ADC IQ wideband jammer estimation start frequency in Hz.
                                               Absolute frequency, e.g. 1555000000 for LTE B13 start frequency of 1555 MHz. */
  uint32 q_AdcIQWidebandJammerStopFreqHz; /* Applicable when CollectMode equals 16.
                                             ADC IQ wideband jammer estimation stop frequency in Hz.
                                             Absolute frequency, e.g. 1573000000 for LTE B13 stop frequency of 1573 MHz.  */
  int32  l_AdcIQRefTonePwrDBm; /* Applicable when CollectMode equals 16.
                                  ADC IQ reference tone power in units of dBm at +100 kHz offset relative to GPS L1.
                                  Typically -120 dBm. */
} gpsdiag_QueryFactoryIqTestReqType;

/* Immediate Response */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2;        /* diagpkt_subsys_header_type */
} gpsdiag_QueryFactoryIqTestImmedRspType;

/* Delayed Response */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType_v2 z_DiagCmdHdrV2; /* diagpkt_subsys_header_type */
  uint8 u_Version; /* Min value of 54 */
  uint8 u_GnssConfig; /* Flags containing configured constellations.
                         C/N0 results are valid for the configured constellations.  
                         Bit 0 – GPS
                         Bit 1 – Glonass
                         Bit 4 – Beidou
                         Bit 5 – Galileo */
  uint32 q_GpsCnoP1DBHz; /* C/N0 reported in units of 0.1 dB-Hz */
  int32  l_GpsFreqHz;    /* freq reported in Hz */
  uint32 q_GloCnoP1DBHz; /* C/N0 reported in units of 0.1 dB-Hz */
  int32  l_GloFreqHz;    /* freq reported in Hz */
  uint32 q_BdsCnoP1DBHz; /* C/N0 reported in units of 0.1 dB-Hz */
  int32  l_BdsFreqHz;    /* freq reported in Hz */
  uint32 q_GalCnoP1DBHz; /* C/N0 reported in units of 0.1 dB-Hz */
  int32  l_GalFreqHz;    /* freq reported in Hz */
  int32  l_AdcMeanIp1mV; /* Mean Estimate (DC) of I component in 
                            ADC processor in 0.1 mV */
  int32  l_AdcMeanQp1mV; /* Mean Estimate (DC) of Q component in 
                            ADC processor in 0.1 mV */
  uint32 q_AdcAmpIp1mV;  /* Amplitude estimate of I component in 
                            ADC processor in 0.1 mV */
  uint32 q_AdcAmpQp1mV;  /* Amplitude estimate of Q component in 
                            ADC processor in 0.1 mV */
  uint8  u_AdcIqSatPerStatus;  /* Indicates validity/status of data.
                                  0 - ADC IQ saturation percentage is not valid
                                  3 – Number of samples in ADC IQ test was zero
                                  4 – ADC IQ saturation percentage is valid */
  uint32 q_ISatPer; /* Percentage of I samples saturated, in units of 0.001 */
  uint32 q_QSatPer; /* Percentage of Q samples saturated, in units of 0.001 */
  int32  l_AdcIqWbJammerPwrEstDb; /* Estimated wideband jammer power in units of 0.01 dB.
                                     Only valid if wideband jammer power estimation was enabled in the 
                                     ADC IQ test request.*/
} gpsdiag_QueryFactoryIqTestDelayedRspType;



/*===========================================================================

FUNCTION tm_diag_msg_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_diag_msg_handler(const void *data);

/*===========================================================================

FUNCTION: tm_diag_init

DESCRIPTION:
  
PARAMETERS:
  
===========================================================================*/
extern void tm_diag_init(void);

/*===========================================================================

FUNCTION tm_diag_prtl_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_diag_prtl_init(void);



/*===========================================================================

FUNCTION tm_diag_wbiq_info_handler

DESCRIPTION 
  handles new info received for the WBIQ report 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_diag_wbiq_info_handler(const sm_ReportWBIQInfoStructType * p_ReportWBIQInfo);


#ifdef __cplusplus
}
#endif

#endif /* TM_DIAG_H */

