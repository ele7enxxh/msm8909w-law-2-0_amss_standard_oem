/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Diagnostic Packet Definitions for GPS

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes which 
  define the interface between the diagnostic module and the GPS module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/diag/src/aries_gpsdiag.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $ 

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/15    jv     QZSS Support
05/26/15    jv     SBAS user preference changed from boolean to uint8
02/24/15   ssu     Adding GM in memory logging.
10/17/14    jv     Added CGPS_TEST_FORCE_MEM_ALLOC_ERROR
09/10/10    ssk    Make additional GPS XTRA Diag changes from code review feeback
08/20/10    ssk    Support for new GPS diag commands for XTRA
02/04/09    atien  MOLR vs AD call flow support
01/12/09    atien  XTRA-T support
06/12/08    lt     Support for external coarse position injection.
04/16/08    gk     On-Demand Changes
11/13/07    gk     Add SUPL Certificate injection through PDAPI
09/14/07    gk     NMEA CONFIG set parameters support
06/18/07    ah     Moved GPS File System support to TM Diag (modem proc).
06/18/07    ld     Added support for Deleting Cell DB Information.
05/29/07    jd     Implemented GPSDIAG support for GPS File System support.
05/15/07    ah     Implemented SUPL and UMTS CP support for Notify Verify cmd.
05/07/07    ah     Moved Subsystem Cmd Codes for CGPS to start from 100.
                   Renumbered CGPS Cmd Codes for all CGPS DM commands defined.
                   Broke up CGPS_API_CMD into individual CGPS DM commands.
                   Added Version field to all Req/Rsp msgs for each DM cmd.
04/16/07    gk     Renumbered delete params.
04/12/07    ah     Various code cleanup and lint fixes.
04/11/07    gk     Modified Time injection cmd
04/05/07    gk     Added Time injection cmd
04/04/07    ah     Broke up CGPS_LCS_VX_CMD into separate individual cmds.
03/07/07    ah     Added LCS support (Notify_Verify_Send_User_Action and
                   various Vx commands), Get_PDAPI_Version command also.
01/15/07    ah     Added Multiple App support (up to 10 simultaneous clients).
                   Modified some CGPS_SET_PARAM_CMD values to reflect recent
                   PDAPI updates.
01/05/07    ah     Added Application Tracking functionality.
12/07/06    ah     Renamed Aries GPS references to CGPS.
07/20/06    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef CGPS_DIAG_H
#define CGPS_DIAG_H

/*
 * Include files
*/

#include "gps_variation.h"
#include "comdef.h"

#include "rex.h"
#include "task.h"

#include "aries_gpsdiag_defs.h"
#include "aries_os_api.h"
#include "gps_types.h"

#if INCLUDE_PDAPI_TEST
#include "pdapi.h"
/* sets the specified bit in a u32 bit mask. bit counts range: 0-31 */
#define GPSDIAG_SET_U32BITMASK_BIT PDSM_SET_U32BITMASK_BIT

#else
/* sets the specified bit in a u32 bit mask. bit counts range: 0-31 */
#define GPSDIAG_SET_U32BITMASK_BIT(n) (((uint32)0x1) << (n))

#endif /* INCLUDE_PDAPI_TEST */


/*
 * Constant definitions
*/

/*============================================================================
   Command Code = DIAG_SUBSYS_CMD_F (75) (uint8)
============================================================================*/

/*============================================================================
   Subsystem ID = DIAG_SUBSYS_GPS (13) (uint8)
============================================================================*/

/*============================================================================
   Subsystem Command Codes (uint16)
============================================================================*/

#define CGPS_DIAG_START_TASK       0x0008  /* Legacy 1x gpsOne Diag ID */
#define CGPS_DIAG_END_TASK         0x000C  /* Legacy 1x gpsOne Diag ID */

#define CGPS_DIAG_PDAPI_CMD        0x0064  /* Aries CGPS Diag ID - NEW */
#define CGPS_DIAG_RF_VERIF_CMD     0x0065  /* Aries CGPS Diag ID - see tm_diag */
#define CGPS_DIAG_USE_NMEA_PORT    0x0066  /* Aries CGPS Diag ID - see tm_diag */

#define CGPS_PDCOMM_TD_START_TASK  0x0067  /* Aries CGPS PDCOMM  - NEW */
#define CGPS_PDCOMM_TD_END_TASK    0x0068  /* Aries CGPS PDCOMM  - NEW */

/*============================================================================
   CGPS Command Codes (uint8)
============================================================================*/

#if INCLUDE_PDAPI_TEST

#define CGPS_INIT_CMD                          0
#define CGPS_REGISTER_PD_CMD                   1
#define CGPS_REGISTER_PA_CMD                   2
#define CGPS_REGISTER_LCS_CMD                  3
#define CGPS_REGISTER_EXT_STAT_CMD             4

#define CGPS_ACTIVATE_CMD                      7
#define CGPS_DEACTIVATE_CMD                    8
#define CGPS_RELEASE_CMD                       9
#define CGPS_START_CMD                        10
#define CGPS_END_CMD                          11
#define CGPS_SET_PARAM_CMD                    12
#define CGPS_GET_PARAM_CMD                    13
#define CGPS_GET_LAST_KNOWN_POS_CMD           14
#define CGPS_GET_PDAPI_VERS_CMD               15
#define CGPS_NTFY_VRFY_SND_USR_ACTN_CMD       16
#define CGPS_INJECT_TIME_CMD                  17
#define CGPS_EXTERN_COARSE_POS_INJ_CMD        18  
#define CGPS_QUERY_GPS_STATE_CMD              19

#define CGPS_LCS_VX_INIT_CLIENT_CMD           31
#define CGPS_LCS_VX_INIT_CLIENT_APP_INFO_CMD  32
#define CGPS_LCS_VX_RELEASE_CLIENT_CMD        33
#define CGPS_LCS_VX_QUERY_CONFIG_CMD          34
#define CGPS_LCS_VX_GET_CLIENT_TYPE_CMD       35
#define CGPS_LCS_VX_GET_STATUS_CMD            36
#define CGPS_TRIGGER_XTRAT_SESSION            37
#define CGPS_TEST_FORCE_MEM_ALLOC_ERROR       38

#endif /* INCLUDE_PDAPI_TEST */

#define CGPS_INTERNAL_REQ_1XEVDO_RF_INFO_CMD  50

/* new command to query SM version via diag. 
   This is different from PDAPI get version because
   it includes major and minor revisions for SM
   and possibly also for underlying MGP subsystems

Starting at version 200 to keep enough separation for
any Gen7 extension. All Gen8 and higher sub commands
start at 200+ 
*/
#define CGPS_QUERY_VERSION                    200
#define CGPS_INJECT_EXTERNAL_SPI              201
#define CGPS_OEM_CONTROL                      202
#define CGPS_OEM_DRSYNC_SET_CFG               203

/* Diag command for XTRA Data Injection  */
#define CGPS_XTRA_INJECT_DATA_CMD             204
/* Diag command for XTRA Client registration  */
#define CGPS_XTRA_CLIENT_REGISTER_CMD         205

/* Geofence In memory logging command  */
#define CGPS_GEOFENCE_IN_MEM_LOG_CMD          206

/*============================================================================
   GPSDIAG task related definitions for Rex (task.h)
============================================================================*/

#define GPSDIAG_INIT_VALUE      (-1)
#define GPSDIAG_ERROR_VALUE     (-1)

/* REX priority for the GPSDIAG task */
#define GPSDIAG_PRI          rcinit_lookup_prio("diag")

/* Size (in units of rex_stack_word_type) of GPSDIAG stack */
#define GPSDIAG_STACK_SIZ    (8192/sizeof(rex_stack_word_type))

extern rex_stack_word_type   gpsdiag_stack[];
extern rex_tcb_type          gpsdiag_tcb;

#ifdef FEATURE_CGPS_PDCOMM_TEST_DRIVER
/*============================================================================
   PDCOMM TD task related definitions for Rex (task.h)
============================================================================*/

#define PDCOMMTD_PRI          rcinit_lookup_prio("pdcommtcp")
/* Size (in units of rex_stack_word_type) of PDCOMM TD stack */
/*overrun was detected with 4KB during apptrack test. Thus increasing to 8KB.*/
#define PDCOMMTD_STACK_SIZ    (8192/sizeof(rex_stack_word_type))

extern rex_stack_word_type   pdcommtd_stack[];
extern rex_tcb_type          pdcommtd_tcb;
#endif /* FEATURE_CGPS_PDCOMM_TEST_DRIVER */

/*============================================================================
   Miscellaneous definitions
============================================================================*/

#define GPSDIAG_MSG_VERSION_1  1


/*
 * Structure definitions
*/

/* Coding Guideline Prefixes
 * enum: e_
 * pointer: p_
 * struct: z_
 * uint8 : unsigned char u_
 * uint16: unsigned short w_
 * uint32: unsigned long q_
 * int32:  signed long l_
 */

typedef enum
{
  /* RESERVED: All OS-related Msg Ids (prefix C_OS_MSG_ID_ values 0-9 inclusive)
   * defined in aries_os_globals.h, including C_OS_MSG_ID_TIMER_EXPIRY
   */

  /* GPSDIAG APP TRACKING TIMER IDs 
   *
   * The following ten Clients' Timer Ids = their respective Client Types + 10
   * Therefore, Client Type = Client's Timer Id - 10
   * Use macro GET_CLIENT_TYPE(timer_id) to perform translation to Client Type
   */
  C_GPSDIAG_TIMER_APP_TRACKING_START   = C_USR_MSG_ID_FIRST,
  C_GPSDIAG_TIMER_APP_TRACKING_BROWSER = C_GPSDIAG_TIMER_APP_TRACKING_START,
  C_GPSDIAG_TIMER_APP_TRACKING_UI,
  C_GPSDIAG_TIMER_APP_TRACKING_PDA,
  C_GPSDIAG_TIMER_APP_TRACKING_DM,
  C_GPSDIAG_TIMER_APP_TRACKING_NI,
  C_GPSDIAG_TIMER_APP_TRACKING_BREW,
  C_GPSDIAG_TIMER_APP_TRACKING_JAVA,
  C_GPSDIAG_TIMER_APP_TRACKING_TEST1,
  C_GPSDIAG_TIMER_APP_TRACKING_TEST2,
  C_GPSDIAG_TIMER_APP_TRACKING_OEM,

  /* GPSDIAG MSG IDs 
   *
   * The following Ids are associated to IPC msgs originating from GPSDIAG task
   */
  C_GPSDIAG_TEST_PDAPI,
  C_GPSDIAG_TEST_APP_TRACKING,
  C_GPSDIAG_TEST_LCS,
  C_GPSDIAG_END_TASK
} gpsdiag_MsgEnumType;

/* Macro definition for translating from from Timer Id to Client Type */
#define GET_CLIENT_TYPE(timer_id) ((timer_id)-10)


/*============================================================================
   GPSDIAG Request and Response Packets
============================================================================*/

/* Typical command header for all Req/Rsp packets */
/* (essentially a more detailed definition of 'diagpkt_subsys_header_type' */
typedef PACKED struct PACKED_POST gpsdiag_CmdHdrType
{
  uint8              u_CmdCode;        /* Command Code */
  uint8              u_SubsysId;       /* Subsystem Id */
  uint16             w_SubsysCmdCode;  /* Subsystem Command Code */
  uint8              u_CgpsCmdCode;    /* CGPS Command Code */
  uint8              u_Version;        /* Request Message Version Number */
} gpsdiag_CmdHdrType; 

typedef PACKED struct PACKED_POST gpsdiag_OldCmdHdrType
{
  uint8              u_CmdCode;        /* Command Code */
  uint8              u_SubsysId;       /* Subsystem Id */
  uint16             w_SubsysCmdCode;  /* Subsystem Command Code */
} gpsdiag_OldCmdHdrType; 
/* Used for Start/End Task Req/Rsp packets */
typedef PACKED struct PACKED_POST gpsdiag_OldCmdHdrType gpsdiag_StartTaskReqType;
typedef PACKED struct PACKED_POST gpsdiag_OldCmdHdrType gpsdiag_StartTaskRspType;
typedef PACKED struct PACKED_POST gpsdiag_OldCmdHdrType gpsdiag_EndTaskReqType;
typedef PACKED struct PACKED_POST gpsdiag_OldCmdHdrType gpsdiag_EndTaskRspType;

#if INCLUDE_PDAPI_TEST

/* CGPS_API_CMD Req/Rsp packets
 *
 * This msg is used to initialize, activate, deactivate, and release a
 * PDAPI client. This cmd should be sent to initialize the PDAPI clients
 * before sending any other request commands. This cmd is sent after the
 * CGPS_DIAG_START_TASK command.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
} gpsdiag_ApiCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_ApiCmdRspType;

/* CGPS_REGISTER_XX_CMD Req/Rsp packets
 *
 * This msg is used to register and deregister a PDAPI client. This cmd
 * should be sent to register the PDAPI clients for events before sending
 * any other request commands. This cmd is sent after the
 * CGPS_DIAG_START_TASK command. 
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint8              u_RegisterType;   /* See ENUM gpsdiag_eRegisterType */
} gpsdiag_RegCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_RegCmdRspType;

/* The following are elements of the union defined in gpsdiag_StartCmdReqType
 * if u_ServerAddrType = ...
 */
typedef PACKED struct PACKED_POST
{
  uint32             q_ServerAdrs;     /* Server address (IPV4 lower 4 bytes) */
  uint32             q_ServerPort;     /* Server IP port */
} gpsdiag_IpV4Type;

typedef PACKED struct PACKED_POST
{
  uint8              u_UrlLength;      /* URL length */
  uint8              u_Url[255];       /* URL value (between 1-255 bytes) */
} gpsdiag_UrlType;

typedef PACKED struct PACKED_POST
{
  uint16             w_ServerAdrs[8];  /* Server address (IPV6 all 16 bytes) */
  uint32             q_ServerPort;     /* Server IP port */
} gpsdiag_IpV6Type;

typedef PACKED union PACKED_POST
{
  gpsdiag_IpV4Type  z_IpV4Data;        /* Used if Server Type is IP_V4 */
  gpsdiag_IpV6Type  z_IpV6Data;        /* Used if Server Type is IP_V6 */
  gpsdiag_UrlType   z_UrlData;         /* Used if Server Type is URL */
} gpsdiag_SrvrAddrType;

/* CGPS_START_CMD Req/Rsp packets
 *
 * This msg is used to start a GPS position session from the PDAPI.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint8              u_SessionType;    /* See ENUM gpsdiag_eSessionType */
  uint8              u_OperatingType;  /* See ENUM gpsdiag_eOperatingType */
  uint8              u_ServerOption;   /* See ENUM gpsdiag_eServerOptionType */
  uint8              u_ServerAddrType; /* See ENUM pdsm_server_address_e_type */
  
  uint32             q_ClassId;        /* Class Id */
  uint8              u_QosGpsSessTout; /* Qos Performance value between 0-255 */
                                       /* Specifies how long the GPS engine is
                                          permitted to process a position req */
  uint32             q_QosAccThrshld;  /* Accuracy Threshold in meters */
  uint32             q_TimeBwFixes;    /* Time between fixes in seconds */
  uint32             q_NumFixes;       /* Number of fixes */
  int32              l_ClientId;       /* Test Client ID */
  uint8              u_BypassMask;     /* Internal Bypass Mask used for testing */
  uint8              u_Reserved[7];    /* Reserved bytes */ 

  gpsdiag_SrvrAddrType u; /* See UNION (IPV4, URL, IPV6 Type) */

} gpsdiag_StartCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_StartCmdRspType;

/* CGPS_END_CMD Req/Rsp packets
 *
 * This msg is used to invoke end of an ongoing position session or data
 * download request.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_EndType;        /* See ENUM gpsdiag_eEndType */
  int32              l_ClientId;       /* Test Client ID */
} gpsdiag_EndCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_EndCmdRspType;


/* The following are elements of the union defined in gpsdiag_SetParamCmdReqType
 * if u_Param = ...
 */
typedef PACKED struct PACKED_POST
{
  uint8                u_ServerAddrType; /* See ENUM pdsm_server_address_e_type */
  gpsdiag_SrvrAddrType z_ServerAddr;     /* See UNION (IPV4, URL, IPV6 Type) */
} gpsdiag_PaNtwrkAdrsType;

typedef PACKED struct PACKED_POST
{
  uint32             q_DelParamsFlags; /* See ENUM gpsdiag_eDelParamsType */
  uint32             q_GnssDelParamsFlags; /* extended flag for gnss systems */
  uint32             q_GnssDelParamsFlagsReserved; /* reserved for future use */
  uint32             q_TimeUnc;        /* Time Uncertainty */
  uint32             q_PosUnc;         /* Position Uncertainty */
  uint32             q_TimeOffset;     /* Time Offset */
  uint32             q_PosOffset;      /* Position Offset */
  uint32             q_EphSvMask;      /* Ephemeris SV Mask */
  uint32             q_AlmSvMask;      /* Almanac SV Mask */
  uint32             q_GloEphSvMask;   /* Ephemeris SV Mask */
  uint32             q_GloAlmSvMask;   /* Almanac SV Mask */
  uint32             q_SBASEphSvMask;  /* Ephemeris SV Mask */
  uint32             q_SBASAlmSvMask;  /* Almanac SV Mask */
  uint32             q_DelCellDBMask;  /* CellDB Delete Mask */
  uint32             q_DeleteClockInfoMask; /* mask for finer control of 
                                               time deletion in MGP.
                                               See C_DELETE_INFO_CLOCK_xxx  */
  uint32             q_Reserved;       
} gpsdiag_PaDelParamsType;


typedef PACKED struct PACKED_POST
{
  gpsdiag_PaDelParamsType z_DelParams;
  float              f_TimeUnc;
  float              f_TimeOffset;
  uint32             q_Reserved;       
} gpsdiag_PaDelParamsTypeEx;

typedef PACKED struct PACKED_POST
{
  uint32 q_NmeaPortType;
  uint32 q_NmeaReportingType;
} gpsdiag_PaNmeaConfigInfoType;

/* This is the Max allowable data payload for LCS Vx Client App Info */
#define C_GPSDIAG_LCS_VX_APP_INFO_MAX                 256

/* This is the Max allowable XTRA data payload calculated by subtracting all the 
   header fields from the Max Diag Pkt size.

  2000 bytes is the maximum size of any DIAG packet.
  10 bytes is the size of the Common Diag Header + client id
  20 bytes is the size of the XTRA header(fragment cnt, size, tot size etc.)
  10 bytes reserved for any further usage.
*/
#define C_GPSDIAG_XTRA_DATA_MAX_PAYLOAD               1960 /* 2000 - 10 -20 -10 */

/* Max allowed diag pkt size - fixed field size */
#define C_GPSDIAG_EFS_MAX_PAYLOAD                     2034
#define C_GPSDIAG_EFS_MAX_FILESIZE                    255
/*  GPS FS task operations */
enum
{
  GPS_FS_CREATE_WRITE_FILE,
  GPS_FS_APPEND_FILE,
  GPS_FS_DELETE_FILE,
  GPS_FS_READ_FILE
};

/* Request packet */
typedef PACKED struct PACKED_POST
{
  uint8              u_FullPathAndFileNameLength;
  char               u_FilePathAndFileName[C_GPSDIAG_EFS_MAX_FILESIZE];
  uint8              u_FileOperation;
  uint32             q_Reserved; 
  uint32             q_FileSize;
  uint8              u_Data_Part_Number;    /* data part number */
  uint8              u_Data_Total_Parts;    /* total data parts */
  char               u_FilePayload[C_GPSDIAG_EFS_MAX_PAYLOAD];
} gpsdiag_PaEfsDataType;

typedef PACKED struct PACKED_POST {
   uint8   u_Enabled; /* 0 = disabled, 1 = enabled */
   uint32  q_Reserved[2];
} gpsdiag_XspiConfigType;

#define C_GPSDIAG_MAX_APN_NAME_LENGTH 100
#define C_GPSDIAG_MAX_APN_PROFILES 6


typedef PACKED struct PACKED_POST{  
  uint32  srv_system_type;
  uint32  pdp_type;
  uint32  reserved;
  uint8   apn_name[C_GPSDIAG_MAX_APN_NAME_LENGTH];
} gpsdiag_apn_profiles_type;

/* CGPS_SET_PARAM_CMD Req/Rsp packets
 *
 * This msg is used to set the NV parameters.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint8              u_Param;          /* See ENUM gpsdiag_eParamType */
  uint16             w_Length;         /* Length of the parameter data */

  PACKED union PACKED_POST {
    gpsdiag_PaNtwrkAdrsType  z_NtwrkAdrs;    /* Network (Server) Address */
    uint8                    u_GpsLock;      /* See ENUM gpsdiag_eGpsLockType */
    boolean                  u_KeepWarm;     /* TRUE=Enabled, FALSE=Disabled */
    gpsdiag_PaDelParamsType  z_DelParams;    /* Delete Parameters */
    uint32                   q_PosMode;      /* See ENUM gpsdiag_ePosModeType */
    uint8                    u_MtLrSpprt;    /* 0=Disabled, 1=Enabled */
    uint8                    u_MoMethod;     /* 0=CP, 1=UP */
    uint16                   w_NmeaSentence; /* See ENUM gpsdiag_eNmeaSentenceType */
    gpsdiag_PaNmeaConfigInfoType z_NmeaConfigInfo; /* NMEA configuration info */
    gpsdiag_PaEfsDataType    z_EfsData;      /* EFS Data to be written. Only used to write SUPL certificate */
    uint8                    u_SbasUserPreference;  /* SBAS Enable mask */
    uint8                    u_DpoControl;   /* DPO Control */
    boolean                  u_OnDemandLpm;
    boolean                  u_OnDemandReady;
    uint8                    u_CallFlowSelectMask;  /* MO-LR vs AD Call Flow Select Mask */
    gpsdiag_XspiConfigType   z_XspiConfig;   /* 0 = disable xspi, 1 = enable */
    uint32                   q_ClientCookie;  /*XTRA-T client cookie for data transfer with XTRA-T server*/
    uint8                    u_SessControl; /* User Control: (0 - disable)/(1 - Enable) XTRA-T session*/    
    boolean                  u_DataEnable; /* enable/disable data*/
    gpsdiag_apn_profiles_type z_ApnProfiles_List[C_GPSDIAG_MAX_APN_PROFILES]; /*APN profiles*/
    uint32                   u_SuplVersion;
    boolean                  u_SuplSecurity; /* TRUE: SUPL FALSE: UPL */
    gpsdiag_PaDelParamsTypeEx  z_DelParams_ex;    /* Delete Parameters extended */
    uint8                    u_lpp_enable; /* bit 0 for UP, bit 1 for CP */                  
  } z_ParamVal;

} gpsdiag_SetParamCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_SetParamCmdRspType;

/* CGPS_GET_PARAM_CMD Req/Rsp packets
 *
 * This msg is used to get the PDSM parameters from the NV.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint8              u_Param;          /* See ENUM gpsdiag_eParamType */
  int32              l_ClientId;       /* Test Client ID */
} gpsdiag_GetParamCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_GetParamCmdRspType;

/* CGPS_GET_LAST_KNOWN_POS_CMD Req/Rsp packets
 *
 * This msg is used to get the last known position.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
} gpsdiag_GetLastKnownPosCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
  int32              l_Latitude;       /* Latitude of the fix */
  int32              l_Longitude;      /* Longitude of the fix */
  uint32             q_TimeStamp;      /* Time stamp in secs since 1/6/80 12am */
  uint8              u_LocUncAng;      /* Location uncertainty angle */
  uint8              u_LocUncA;        /* Stdev of axis along angle */
  uint8              u_LocUncP;        /* Stdev of axis perp to angle */
  uint8              u_OptFieldMask;   /* Optional fields valid flag */
  int16              x_Altitude;       /* Altitude in meters */
  uint16             w_Heading;        /* Heading [360/210 deg/bit]  */
  uint16             w_VelocityHor;    /* Horizontal velocity 0.25 m/s */
  uint8              u_FixType;        /* Fix type (1 if 3D fix, else 0) */
  int8               b_VelocityVer;    /* Vertical velocity 0.5 m/s */
  uint8              u_LocUncV;        /* Stdev of vertical error  */
  uint8              e_GpsPosMethod;   /* See ENUM gpsdiag_ePositioningType */
  uint8              u_PosSource;      /* AHTODO: Need to revisit this */
  uint8              u_UncEllipseConf; /* Confidence [0-100%] for unc ellipse */
  int8               b_UtcOffset;      /* UTC offset if valid flag is set */
  int32              l_ExtLocLat;      /* Latitude in WGS84 (more resolution) */
  int32              l_ExtLocLong;     /* Longitude in WGS84 (more resolution) */
  uint16             w_ExtLocUncA;     /* Extended value of unc in meters */
  uint16             w_ExtLocUncP;     /* Extended value of unc in meters */
  int16              x_VelHoriz;       /* Horizontal velocity in cm/s */
  int16              x_VelVert;        /* Vertical velocity in cm/s */
  uint32             q_TStampTowMs;    /* Time stamp in GPS TOW MilliSeconds */
  uint16             w_TStampGpsWeek;  /* Time stamp GPS week number */
  uint8              u_PosReportToNw;  /* True if pos was reported to network */
  uint16             w_VelUncHoriz;    /* Horizontal velocity uncertainty */
  uint16             w_VelUncVert;     /* Vertical velocity uncertainty */
} gpsdiag_GetLastKnownPosCmdRspType;

#if INCLUDE_LCS_CMDS

/* The following are elements of the SUPL struct defined in gpsdiag_NtfyVrfySndUsrActnCmdReqType
 */
typedef PACKED struct PACKED_POST
{
  uint8                   u_SlpAddrType;      /* See ENUM pdsm_server_address_e_type */
  gpsdiag_SrvrAddrType    z_SlpAddr;          /* See UNION (IPV4, URL, IPV6 Type) */
} gpsdiag_SuplSlpAddrType;

typedef PACKED struct PACKED_POST
{
  boolean                 u_Presence;         /* Not used in PDAPI, leave for SUPL use */
  uint8                   u_SessionId[PDSM_SUPL_SLP_SESSION_ID_BYTE_LENGTH];
  gpsdiag_SuplSlpAddrType z_SlpAddr;          /* SUPL SLP Address */
} gpsdiag_SuplSlpSessIdType;

typedef PACKED struct PACKED_POST
{
  uint8                   u_DatacodingScheme; /* Requestor ID Datacoding Scheme */
  uint8                   u_ReqIdStr[PDSM_LCS_REQUESTOR_ID_LEN]; /* Requestor ID String */
  uint8                   u_ReqIdStrLen;      /* Requestor ID String Length */
} gpsdiag_LcsReqIdType;

typedef PACKED struct PACKED_POST
{
  uint8                   u_DatacodingScheme; /* Client Name Datacoding Scheme */
  uint8                   u_ClientNameStr[PDSM_LCS_CLIENT_NAME_LEN]; /* Client Name String */
  uint8                   u_ClientNameStrLen; /* Client Name String Length */
} gpsdiag_LcsClientNameType;

typedef PACKED struct PACKED_POST
{
  uint8                   u_BitMask;   /* Indicates which fields are valid in this struct */
  uint8                   u_HorAcc;
  uint8                   u_VerAcc;    /* Optional: set in bit_mask supl_veracc_present if present */
  uint16                  w_MaxLocAge; /* Optional: set in bit_mask supl_maxLocAge_present if present */
  uint8                   u_Delay;     /* Optional: set in bit_mask supl_delay_present if present */
} gpsdiag_LcsSuplQopType;

/* The following are elements of the UMTS CP struct defined in gpsdiag_NtfyVrfySndUsrActnCmdReqType
 */
typedef PACKED struct PACKED_POST
{
  uint8                   u_ExtClientAddrLen; /* External Client Address Length */
  uint8                   u_ExtClientAddr[PDSM_LCS_MAX_EXT_CLIENT_ADDRESS]; /* External Client Address valid if present bit set */
} gpsdiag_LcsExtClntAdrType;

typedef PACKED struct PACKED_POST
{
  uint8                   u_DefLocUnusedBits; /* Deferred Location Unused bits */
  uint8                   u_DefLocMsAvail;    /* Deferred Location MS available */
} gpsdiag_LcsDeferLocType;

typedef PACKED struct PACKED_POST
{
  uint8                   u_DatacodingScheme; /* Codeword Datacoding Scheme */
  uint8                   u_CodewordStr[PDSM_LCS_CODEWORD_LEN]; /* Codeword String */
  uint8                   u_CodewordStrLen;   /* Codeword String Length */
} gpsdiag_LcsCodewordType;


/* The following are elements of the union defined in gpsdiag_NtfyVrfySndUsrActnCmdReqType
 * if u_CmdType = ...
 */
typedef PACKED struct PACKED_POST
{
  uint8                     u_NotifPrivType;            /* See ENUM gpsdiag_eNotifPrivType */
  uint8                     u_PosTechIndicator;         /* See ENUM gpsdiag_ePosTechIndctr */
  uint8                     u_PosQosPresent;            /* Position QoS available? 0=NO, 1=YES */
  uint8                     u_PosQos;                   /* QoS of the position */
  uint32                    q_NumFixes;                 /* Number of fixes */
  uint32                    q_TimeBwFixes;              /* Time between fixes in seconds */
  uint8                     u_PositionMode;             /* See ENUM gpsdiag_ePositionMode */
  uint8                     u_DataCodingScheme;         /* ENUM Type of data encoding scheme for the text */
  uint8                     u_ReqIdLength;              /* Requestor ID Length */
  uint8                     u_ReqId[200];               /* Requestor ID */
  uint16                    w_UserRspTime;              /* Time before which a user needs to respond */
} gpsdiag_RespVxType;

typedef PACKED struct PACKED_POST
{
  uint8                     u_NotifPrivType;            /* See ENUM gpsdiag_eNotifPrivType */
  uint16                    w_LcsFlags;                 /* See ENUM gpsdiag_ePdsmLcsFlags */
  gpsdiag_SuplSlpSessIdType z_SuplSlpSessId;            /* SLP Session ID */
#if 0
  uint8                     u_SuplHash[PDSM_SUPL_HASH_LEN]; /* HASH for SUPL_INIT */
  uint8                     u_DatacodingScheme;         /* See ENUM gpsdiag_eDatacodingScheme */
  uint8                     u_SuplPosMethod;            /* See ENUM gpsdiag_eSuplPosMethodType */
  gpsdiag_LcsReqIdType      z_RequestorId;              /* Requestor ID */
  gpsdiag_LcsClientNameType z_ClientName;               /* Client Name */
  gpsdiag_LcsSuplQopType    z_SuplQop;                  /* SUPL QOP */
  uint16                    w_UserRspTime;              /* Time before which a user has to respond (Not used) */
#endif /* 0 */
} gpsdiag_RespSuplType;

typedef PACKED struct PACKED_POST
{
  uint8                     u_NotifPrivType;            /* See ENUM gpsdiag_eNotifPrivType */
  uint16                    w_LcsFlags;                 /* See ENUM gpsdiag_ePdsmLcsFlags */
  uint8                     u_SessInvokeId;             /* Session Invoke Id */
#if 0
  uint8                     u_NotifTextLength;          /* Notification Text Length */
  uint8                     u_NotifText[255];           /* Notification Text valid only if flags indicate present */
  uint8                     u_DatacodingScheme;         /* See ENUM gpsdiag_eDatacodingScheme */
  gpsdiag_LcsExtClntAdrType z_ExtClientAddrData;        /* External Client Address */
  uint8                     u_LocationType;             /* See ENUM gpsdiag_eLocationType */
  gpsdiag_LcsDeferLocType   z_DeferredLocation;         /* Deferred Location - CP Only */
  gpsdiag_LcsReqIdType      z_RequestorId;              /* Requestor ID */
  gpsdiag_LcsCodewordType   z_CodewordString;           /* Codeword String - CP Only */
  uint8                     u_ServiceTypeId;        /* Service Type ID - CP Only */
  uint16                    w_UserRspTime;              /* Time before which a user has to respond (Not used) */
#endif /* 0 */
} gpsdiag_RespUmtsCPType;

/* CGPS_NTFY_VRFY_SND_USR_ACTN_CMD Req/Rsp packets
 *
 * This msg is used to inform the PDAPI from the UI, of the response to the
 * incoming request for positioning request msg.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint8              u_AcceptDeny;     /* See ENUM gpsdiag_eAcceptDenyType */
  int32              l_ClientId;       /* Client ID received in notif req msg */
  uint8              u_CmdType;        /* See ENUM gpsdiag_eNvsuaCmdType */
  uint16             w_RespDataLength; /* Length of the response data */

  PACKED union PACKED_POST {
    gpsdiag_RespSuplType   z_SuplData;    /* SUPL Response Data */
    gpsdiag_RespVxType     z_VxData;      /* V1/V2 Response Data */
    gpsdiag_RespUmtsCPType z_UmtsCPData;  /* UMTS Control Plane Response Data */
  } z_RespVal;

} gpsdiag_NtfyVrfySndUsrActnCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_NtfyVrfySndUsrActnCmdRspType;

/* CGPS_LCS_VX_INIT_CLIENT_CMD Req/Rsp packets
 *
 * This msg is used to execute the LCS commands for V1/V2.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_ClientType;     /* Client Type */
  uint32             q_ClassId;        /* Class ID */
  uint8              u_IsBrewApp;      /* TRUE if BREW App, FALSE otherwise */
} gpsdiag_LcsVxInitClientCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_LcsVxInitClientCmdRspType;

/* CGPS_LCS_VX_INIT_CLIENT_APP_INFO_CMD Req/Rsp packets
 *
 * This msg is used to execute the LCS commands for V1/V2.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_ClientType;     /* Client Type */
  uint32             q_ClassId;        /* Class ID */
  uint8              u_IsBrewApp;      /* TRUE if BREW App, FALSE otherwise */
  uint8              u_AppInfo[C_GPSDIAG_LCS_VX_APP_INFO_MAX]; /* App Info data */
} gpsdiag_LcsVxInitClientAppInfoCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_LcsVxInitClientAppInfoCmdRspType;

/* CGPS_LCS_VX_RELEASE_CLIENT_CMD Req/Rsp packets
 *
 * This msg is used to execute the LCS commands for V1/V2.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_ClientType;     /* Client Type */
  uint32             q_ClassId;        /* Class ID */
} gpsdiag_LcsVxReleaseClientCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_LcsVxReleaseClientCmdRspType;

/* CGPS_LCS_VX_QUERY_CONFIG_CMD Req/Rsp packets
 *
 * This msg is used to execute the LCS commands for V1/V2.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_ClientType;     /* Client Type */
} gpsdiag_LcsVxQueryConfigCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_LcsVxQueryConfigCmdRspType;

/* CGPS_LCS_VX_GET_CLIENT_TYPE_CMD Req/Rsp packets
 *
 * This msg is used to execute the LCS commands for V1/V2.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_ClassId;        /* Class ID */
} gpsdiag_LcsVxGetClientTypeCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_ClientType;     /* Client Type Value Returned */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_LcsVxGetClientTypeCmdRspType;

/* CGPS_LCS_VX_GET_STATUS_CMD Req/Rsp packets
 *
 * This msg is used to execute the LCS commands for V1/V2.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_ClientType;     /* Client Type */
} gpsdiag_LcsVxGetStatusCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_LcsVxGetStatusCmdRspType;

#endif /* INCLUDE_LCS_CMDS */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32              session_control;  /* 0 stops an xtrat session - 1 - triggers an xtrat session*/
}gpsdiag_Trigger_Xtrat_Session_req_type;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
}gpsdiag_Trigger_Xtrat_Session_rsp_type;

/* CGPS_GET_PDAPI_VERS_CMD Req/Rsp packets
 *
 * This msg is used to get the version of the PDAPI that is used for the 
 * diagnostic msgs. This version number is used to send the appropriate GPS
 * diagnostic msgs.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
} gpsdiag_GetPdapiVersReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint8              u_MajorRev;       /* >= 6 for using these diag cmds */
  uint8              u_MinorRev;       /* >= 0 for using these diag cmds */
} gpsdiag_GetPdapiVersRspType;

/* CGPS_INJECT_TIME_CMD Req/Rsp packets
 *
 * This msg is used to inject external time into the GPS system.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;         /* diagpkt_subsys_header_type */
  uint16             w_Unused;             /* Unused bytes */
  int32              l_ClientId;           /* Test Client ID */

  /* If src is Host, then this field is "relative" not abs time.
   * If src is Network, then this is time in msec since 00:00:00 Jan 6, 1980 */
  uint32             q_LowerTimeMsec;      /* This is the lower 32 bytes */
  uint32             q_UpperTimeMsec;      /* This is the upper 32 bytes */
  uint32       q_TimeUncMsec;        /* Time uncertainty */
  uint8        u_TimeSource;         /* Time source of the PC clock */
  boolean      u_TimeIsGps;          /* TRUE if time base is GPS
                                            * FALSE if time base is UTC
                                            * Valid only if Time Source is Network */
  uint8        u_DiscontinuityCount; /* If Time Source is Host, then signal any
                                            * time discontinuities with this flag */
  boolean      u_ForceFlag;          /* Force acceptance of Time Injection */
} gpsdiag_InjectExtTimeReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;         /* diagpkt_subsys_header_type */
  int32              l_ClientId;           /* Test Client ID */
  uint32             q_Status;             /* See ENUM gpsdiag_eStatusType */
} gpsdiag_InjectExtTimeRspType;


/* CGPS_QUERY_GPS_STATE_CMD Req/Rsp packets
 *
 * This msg is used to Query GPS state.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;         /* diagpkt_subsys_header_type */
  uint16             w_Unused;             /* Unused bytes */
  int32              l_ClientId;           /* Test Client ID */
  
} gpsdiag_QueryGpsStateReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;         /* diagpkt_subsys_header_type */
  int32              l_ClientId;           /* Test Client ID */
  uint32             q_Status;             /* See ENUM gpsdiag_eStatusType */
} gpsdiag_QueryGpsStateRspType;


/* CGPS_EXTERN_COARSE_POS_INJ_CMD Req/Rsp packets
 *
 * This msg is used to inject external coase position into the GPS system.
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;         /* diagpkt_subsys_header_type */
  uint16             w_Unused;             /* Unused bytes */
  int32              l_ClientId;           /* Test Client ID */

  /* indicating the presence/absence of the optional fields */
  uint32  opt_field_mask; 

  /* info. of the time associated with the position. */
  uint32  time_info_type;
  uint32  timestamp_sec; /* acc. sec */

  uint32  alt_def; /* def. of altitude */

  uint32  source; /* position source */

  int32   latitude;  /* Lat.: rad. LSB = pi/(2^32), 2's comp. */
  int32   longitude; /* Long.: rad, LSB = pi /(2^31), 2's comp. */ 

  /* horizontal unc: circular shape, radial */
  uint32  loc_unc_hor;   /* horizontal location unc.: meters */

  int16   altitude; /* alt: meters, positive height, negative depth  */ 

  /* vertical unc */
  uint16  loc_unc_vert;   /* loc unc vertical: meters */

  /* confidence horizontal: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8   confidence_hor; 

  /* confidence vertical: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8   confidence_vert; 

} gpsdiag_InjectExternCoasePosReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;         /* diagpkt_subsys_header_type */
  int32              l_ClientId;           /* Test Client ID */
  uint32             q_Status;             /* See ENUM gpsdiag_eStatusType */
} gpsdiag_InjectExternCoasePosRspType;

/* CGPS_INTERNAL_REQ_1XEVDO_RF_INFO_CMD Req/Rsp packets */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
} gpsdiag_1xEvdoRfInfoReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_1xEvdoRfInfoRspType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType   z_DiagCmdHdr;           /* diagpkt_subsys_header_type */
  int32                l_ClientId;             /* Test Client ID */

  uint32               q_FileSize;
  uint32               q_Data_Part_Number;    /* data part number */
  uint32               q_Data_Total_Parts;    /* total data parts */
  uint32               q_Xtra_Data_Len;       /* Length of XTRA data */
  unsigned char        u_Xtra_Data_Load[C_GPSDIAG_XTRA_DATA_MAX_PAYLOAD];
  uint32               p_Client_Data_Ptr;     /* user data pointer */
} gpsdiag_XtraDataInjectReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType      z_DiagCmdHdr;        /* diagpkt_subsys_header_type */
  int32                   l_ClientId;          /* Client id of user */
  uint32                  q_Status;            /* See ENUM gpsdiag_eStatusType */
  uint32                  q_Xtra_Data_Len;     /* Length of XTRA data */
  uint32                  q_Data_Part_Number;  /* data part number */
  uint32                  q_Data_Total_Parts;  /* total data parts */
  uint32                  p_Client_Data_Ptr;   /* user data pointer */
} gpsdiag_XtraDataInjectRspType;


typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType   z_DiagCmdHdr;       /* diagpkt_subsys_header_type */
  int32                l_ClientId;         /* Test Client ID */
  uint8                u_Param;            /* See ENUM gpsdiag_eParamType */
  uint16               w_Length;           /* Length of the parameter data */
  uint32               p_Client_Data_Ptr;  /* user data pointer */

} gpsdiag_XtraRegisterReqType;


typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType      z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32                   l_ClientId;       /* Client id of user */
  uint8                   u_Param;          /* See ENUM gpsdiag_eParamType */
  uint16                  w_Length;         /* Length of the parameter data */
  uint32                  p_Client_Data_Ptr;/* Pointer to client provided data block. */
  /* NOte: The data block pointed by this pointer might be accessed from  client, as well as, PDSM context simultaneously */
  uint32                  q_Status;         /* See ENUM gpsdiag_eStatusType */
} gpsdiag_XtraRegisterResType;


/* Enumeration Types */

/* CGPS_REGISTER_XX_CMD request msg REGISTER_TYPE enumeration */
typedef enum
{
  GPSDIAG_REG_TYPE_REGISTER    = 0,  /* Event Register */
  GPSDIAG_REG_TYPE_DEREGISTER  = 1,  /* Event Deregister */
  GPSDIAG_REG_TYPE_MAX
} gpsdiag_eRegisterType;

/* CGPS_START_CMD request msg SESSION_TYPE enumeration */
typedef enum
{
  GPSDIAG_SESS_TYPE_LATEST     = 0,  /* Get latest PD info */
  GPSDIAG_SESS_TYPE_NEW        = 1,  /* Get new position */
  GPSDIAG_SESS_TYPE_TRACK_IND  = 2,  /* Tracking mode independent fixes */
  GPSDIAG_SESS_TYPE_DATA_DLOAD = 3,  /* Data download option */
  GPSDIAG_SESS_TYPE_MAX
} gpsdiag_eSessionType;

/* CGPS_START_CMD request msg OPERATING_TYPE enumeration */
typedef enum
{
  GPSDIAG_SESS_OP_STANDALONE_ONLY  = 1,
  GPSDIAG_SESS_OP_MSBASED          = 2,
  GPSDIAG_SESS_OP_MSASSISTED       = 3,
  GPSDIAG_SESS_OP_OPTIMAL_SPEED    = 4,
  GPSDIAG_SESS_OP_OPTIMAL_ACCURACY = 5,
  GPSDIAG_SESS_OP_OPTIMAL_DATA     = 6,
  GPSDIAG_SESS_OP_REFERENCE_POS    = 7,
  GPSDIAG_SESS_OP_CELLID           = 8,
  GPSDIAG_SESS_OP_MAX
} gpsdiag_eOperatingType; /* Operating mode */

/* CGPS_START_CMD request msg SERVER_OPTION enumeration */
typedef enum
{
  GPSDIAG_SERVER_DEFAULT = 0,
  GPSDIAG_SERVER_LOCAL   = 1,
  GPSDIAG_SERVER_OP_MAX
} gpsdiag_eServerOptionType; /* PDE server option */

/* CGPS_END_CMD request msg END_TYPE enumeration */
typedef enum
{
  GPSDIAG_END_NON_DLOAD = 0,  /* End position session */
  GPSDIAG_END_DLOAD     = 1,  /* End data download */
  GPSDIAG_END_TYPE_MAX
} gpsdiag_eEndType;

/* CGPS_SET_PARAM_CMD request msg PARAMETER enumeration */
/* CGPS_GET_PARAM_CMD request msg PARAMETER enumeration */
typedef enum
{
  GPSDIAG_PA_APP_INFO              = 1,
  GPSDIAG_PA_GPS_LOCK              = 2,
  GPSDIAG_PA_KEEP_WARM             = 3,
  GPSDIAG_PA_DELETE_PARAMS         = 4,
  GPSDIAG_PA_POSITION_MODE         = 5,
  GPSDIAG_PA_MT_LR_SUPPORT         = 6,
  GPSDIAG_PA_MO_METHOD             = 7,
  GPSDIAG_PA_NMEA_SENTENCE_TYPE    = 8,
  GPSDIAG_PA_1X_PDE_NETWORK_ADRS   = 9,
  GPSDIAG_PA_1X_MPC_NETWORK_ADRS   = 10,
  GPSDIAG_PA_UMTS_PDE_NETWORK_ADRS = 11,
  GPSDIAG_PA_SSD_DATA              = 12,
  GPSDIAG_PA_SEC_UPDATE_RATE       = 13,
  GPSDIAG_PA_PDAPI_ENABLE          = 14,
  GPSDIAG_PA_NMEA_CONFIG_INFO      = 15,
  GPSDIAG_PA_EFS_DATA              = 16,
  GPSDIAG_PA_SBAS_USER_PREFERENCE  = 17,
  GPSDIAG_PA_DPO_CONTROL           = 18,
  GPSDIAG_PA_ON_DEMAND_ENABLE      = 19,
  GPSDIAG_PA_ON_DEMAND_READY       = 20,
  GPSDIAG_PA_MOLR_AD_CALLFLOW_SELECT = 21,
  GPSDIAG_PA_SET_EXTERNAL_SPI      = 22,
  GPSDIAG_PA_XTRAT_CLIENT_TOKEN    = 23,
  GPSDIAG_PA_XTRAT_USER_SESSION_CONTROL = 24,
  GPSDIAG_PA_DATA_ENABLE = 25,
  GPSDIAG_PA_LBS_APN_PROFILE_LIST  = 26,
  GPSDIAG_PA_XTRA_APN_PROFILE_LIST  = 27,
  GPSDIAG_PA_SUPL_VERSION          = 28,
  GPSDIAG_PA_SUPL_SECURITY         = 29,
  GPSDIAG_PA_DELETE_PARAMS_EX      = 33, /* match with PDAPI ENUM */
  GPSDIAG_PA_LPP_ENABLE            = 34, /* match with PDAPI ENUM */
  GPSDIAG_PA_MAX
} gpsdiag_eParamType;

/* CGPS_SET_PARAM_CMD request msg GPS_LOCK enumeration */
typedef enum
{
  GPSDIAG_PA_LOCK_NONE = 0,
  GPSDIAG_PA_LOCK_MI   = 1,
  GPSDIAG_PA_LOCK_MT   = 2,
  GPSDIAG_PA_LOCK_ALL  = 3,
  GPSDIAG_PA_LOCK_MAX
} gpsdiag_eGpsLockType;

/* CGPS_SET_PARAM_CMD request msg DELETE_PARAMS enumeration */
typedef enum
{
  /* Bit map of 4 bytes */
  GPSDIAG_PA_DELETE_EPH     = 0x0001,
  GPSDIAG_PA_DELETE_ALM     = 0x0002,
  GPSDIAG_PA_DELETE_POS     = 0x0004,
  GPSDIAG_PA_DELETE_TIME    = 0x0008,
  GPSDIAG_PA_DELETE_IONO    = 0x0010,
  GPSDIAG_PA_DELETE_UTC     = 0x0020,
  GPSDIAG_PA_DELETE_HEALTH  = 0x0040,
  GPSDIAG_PA_DELETE_SVDIR   = 0x0080,
  GPSDIAG_PA_DELETE_SVSTEER = 0x0100,
  GPSDIAG_PA_DELETE_SADATA  = 0x0200,
  GPSDIAG_PA_DELETE_RTI     = 0x0400,
  GPSDIAG_PA_TIME_UNC       = 0x0800,
  GPSDIAG_PA_POS_UNC        = 0x1000,
  GPSDIAG_PA_TIME_OFFSET    = 0x2000,
  GPSDIAG_PA_POS_OFFSET     = 0x4000,
  GPSDIAG_PA_DELETE_CELLDB_INFO = 0x8000,
  GPSDIAG_PA_DELETE_ALM_CORR = 0x10000,
  GPSDIAG_PA_FREQ_BIAS_EST  =  0x20000,
  GPSDIAG_PA_DELETE_SV_NO_EXIST = 0x40000
} gpsdiag_eDelParamsType;

typedef enum
{
   /* bitmap of 4 bytes */
   GNSSDIAG_PA_DELETE_EPH_GLO       = GPSDIAG_SET_U32BITMASK_BIT(0),
   GNSSDIAG_PA_DELETE_EPH_SBAS      = GPSDIAG_SET_U32BITMASK_BIT(1),
   GNSSDIAG_PA_DELETE_ALM_GLO       = GPSDIAG_SET_U32BITMASK_BIT(2),
   GNSSDIAG_PA_DELETE_ALM_SBAS      = GPSDIAG_SET_U32BITMASK_BIT(3),
   GNSSDIAG_PA_DELETE_SVDIR_GLO     = GPSDIAG_SET_U32BITMASK_BIT(4),
   GNSSDIAG_PA_DELETE_SVDIR_SBAS    = GPSDIAG_SET_U32BITMASK_BIT(5),
   GNSSDIAG_PA_DELETE_SVSTEER_GLO   = GPSDIAG_SET_U32BITMASK_BIT(6),
   GNSSDIAG_PA_DELETE_SVSTEER_SBAS  = GPSDIAG_SET_U32BITMASK_BIT(7),
   GNSSDIAG_PA_DELETE_ALM_CORR_GLO  = GPSDIAG_SET_U32BITMASK_BIT(8),
   GNSSDIAG_PA_DELETE_CLOCK_INFO    = GPSDIAG_SET_U32BITMASK_BIT(9),
   GNSSDIAG_PA_DELETE_EPH_BDS       = GPSDIAG_SET_U32BITMASK_BIT(10),
   GNSSDIAG_PA_DELETE_ALM_BDS       = GPSDIAG_SET_U32BITMASK_BIT(11),
   GNSSDIAG_PA_DELETE_SVDIR_BDS     = GPSDIAG_SET_U32BITMASK_BIT(12),
   GNSSDIAG_PA_DELETE_SVSTEER_BDS   = GPSDIAG_SET_U32BITMASK_BIT(13),
   GNSSDIAG_PA_DELETE_ALM_CORR_BDS  = GPSDIAG_SET_U32BITMASK_BIT(14),
   GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_GPS  = GPSDIAG_SET_U32BITMASK_BIT(15),
   GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_GLO  = GPSDIAG_SET_U32BITMASK_BIT(16),
   GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_BDS  = GPSDIAG_SET_U32BITMASK_BIT(17),
   GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_GAL  = GPSDIAG_SET_U32BITMASK_BIT(18),
   GNSSDIAG_PA_DELETE_EPH_GAL       = GPSDIAG_SET_U32BITMASK_BIT(19),
   GNSSDIAG_PA_DELETE_ALM_GAL       = GPSDIAG_SET_U32BITMASK_BIT(20),
   GNSSDIAG_PA_DELETE_SVDIR_GAL     = GPSDIAG_SET_U32BITMASK_BIT(21),
   GNSSDIAG_PA_DELETE_SVSTEER_GAL   = GPSDIAG_SET_U32BITMASK_BIT(22),
   GNSSDIAG_PA_DELETE_ALM_CORR_GAL  = GPSDIAG_SET_U32BITMASK_BIT(23),
   GNSSDIAG_PA_DELETE_GNSS_SV_BLACKLIST_QZSS  = GPSDIAG_SET_U32BITMASK_BIT(24),
   GNSSDIAG_PA_DELETE_EPH_QZSS       = GPSDIAG_SET_U32BITMASK_BIT(25),
   GNSSDIAG_PA_DELETE_ALM_QZSS       = GPSDIAG_SET_U32BITMASK_BIT(26),
   GNSSDIAG_PA_DELETE_SVDIR_QZSS     = GPSDIAG_SET_U32BITMASK_BIT(27),
   GNSSDIAG_PA_DELETE_SVSTEER_QZSS   = GPSDIAG_SET_U32BITMASK_BIT(28),
   GNSSDIAG_PA_DELETE_ALM_CORR_QZSS  = GPSDIAG_SET_U32BITMASK_BIT(29),
   GNSSDIAG_PA_DELETE_IONO_QZSS      = GPSDIAG_SET_U32BITMASK_BIT(30)
} gnssdiag_eDelParamsType;

/* CGPS_SET_PARAM_CMD request msg POSITION_MODE enumeration */
typedef enum
{
  /* Bit map of 4 bytes */
  GPSDIAG_PA_POS_MODE_2G_CP_MS_BASED    = 0x0008,
  GPSDIAG_PA_POS_MODE_2G_CP_MS_ASSISTED = 0x0010,
  GPSDIAG_PA_POS_MODE_3G_CP_UE_BASED    = 0x0020,
  GPSDIAG_PA_POS_MODE_3G_CP_UE_ASSISTED = 0x0040
} gpsdiag_ePosModeType;

/* CGPS_SET_PARAM_CMD request msg NMEA_SENTENCE_TYPE enumeration */
typedef enum
{
  /* Bit map of 2 bytes */
  GPSDIAG_PA_NMEA_GGA = (uint16) (GPSDIAG_SET_U32BITMASK_BIT(0) & 0xffff),
  GPSDIAG_PA_NMEA_RMC = (uint16) (GPSDIAG_SET_U32BITMASK_BIT(1) & 0xffff),
  GPSDIAG_PA_NMEA_GSV = (uint16) (GPSDIAG_SET_U32BITMASK_BIT(2) & 0xffff),
  GPSDIAG_PA_NMEA_GSA = (uint16) (GPSDIAG_SET_U32BITMASK_BIT(3) & 0xffff),
  GPSDIAG_PA_NMEA_VTG = (uint16) (GPSDIAG_SET_U32BITMASK_BIT(4) & 0xffff),
  GNSSDIAG_PA_NMEA_GLGSV = (uint16) (GPSDIAG_SET_U32BITMASK_BIT(5) & 0xffff),
  GNSSDIAG_PA_NMEA_GNGSA = (uint16) (GPSDIAG_SET_U32BITMASK_BIT(6) & 0xffff),
  GNSSDIAG_PA_NMEA_GNGNS = (uint16) (GPSDIAG_SET_U32BITMASK_BIT(7) & 0xffff),
  GNSSDIAG_PA_NMEA_PSTIS =  (uint16) (GPSDIAG_SET_U32BITMASK_BIT(8) & 0xffff),
  GNSSDIAG_PA_NMEA_GARMC =  (uint16) (GPSDIAG_SET_U32BITMASK_BIT(9) & 0xffff),
  GNSSDIAG_PA_NMEA_GAGSV =  (uint16) (GPSDIAG_SET_U32BITMASK_BIT(10) & 0xffff),
  GNSSDIAG_PA_NMEA_GAGSA =  (uint16) (GPSDIAG_SET_U32BITMASK_BIT(11) & 0xffff),
  GNSSDIAG_PA_NMEA_GAVTG =  (uint16) (GPSDIAG_SET_U32BITMASK_BIT(12) & 0xffff),
  GNSSDIAG_PA_NMEA_GAGGA =  (uint16) (GPSDIAG_SET_U32BITMASK_BIT(13) & 0xffff),
  GNSSDIAG_PA_NMEA_PQGSX =  (uint16) (GPSDIAG_SET_U32BITMASK_BIT(14) & 0xffff)
} gpsdiag_eNmeaSentenceType;

/* CGPS_GET_LAST_KNOWN_POS_CMD response msg GPS_POSITIONING_METHOD enumeration */
typedef enum
{
  GPSDIAG_MS_BASED    = 0,
  GPSDIAG_MS_ASSISTED = 1,
  GPSDIAG_STANDALONE  = 2,
  GPSDIAG_UNKNOWN     = 3
} gpsdiag_ePositioningType;

typedef enum
{
  GPSDIAG_PA_NMEA_PORT_TYPE_MIN = -1, /* Disable NMEA port */
  GPSDIAG_PA_NMEA_PORT_TYPE_UART1,  /* COM PORT */
  GPSDIAG_PA_NMEA_PORT_TYPE_UART2,  /* COM PORT */
  GPSDIAG_PA_NMEA_PORT_TYPE_USB,   /* USB */
  GPSDIAG_PA_NMEA_PORT_TYPE_SHARED_MEM, /* Shared Memory port */
  GPSDIAG_PA_NMEA_PORT_TYPE_NONE, /* Disable NMEA port */
  GPSDIAG_PA_NMEA_PORT_TYPE_MAX
} gpsdiag_eNmeaPortType;

typedef enum
{
  GPSDIAG_PA_NMEA_REPORT_NONE = -1, 
  GPSDIAG_PA_NMEA_REPORT_AT_1HZ_INTERVAL,
  GPSDIAG_PA_NMEA_REPORT_AT_FINAL_POSITION,
  GPSDIAG_PA_NMEA_REPORT_MAX,
} gpsdiag_eNmeaReportType;

#if INCLUDE_LCS_CMDS

/* CGPS_NTFY_VRFY_SND_USR_ACTN_CMD request msg ACCEPT_DENY enumeration */
typedef enum
{
  GPSDIAG_NVSUA_ACCEPT      = 0,
  GPSDIAG_NVSUA_DENY        = 1,
  GPSDIAG_NVSUA_NO_RESPONSE = 2,
  GPSDIAG_NVSUA_MAX
} gpsdiag_eAcceptDenyType;

/* CGPS_NTFY_VRFY_SND_USR_ACTN_CMD request msg CMD_TYPE enumeration */
typedef enum
{
  GPSDIAG_NVSUA_CMD_VX_REQ                 = 0,
  GPSDIAG_NVSUA_CMD_SUPL_REQ               = 1,
  GPSDIAG_NVSUA_CMD_UMTS_CP_REQ            = 2,
  GPSDIAG_NVSUA_CMD_VX_SERVICE_INTERACTION = 3,
  GPSDIAG_NVSUA_CMD_CLIENT_TYPE            = 4,
  GPSDIAG_NVSUA_CMD_VX_CONFIG              = 5,
  GPSDIAG_NVSUA_CMD_MAX
} gpsdiag_eNvsuaCmdType;

/* CGPS_NTFY_VRFY_SND_USR_ACTN_CMD request msg NTFY_PRIV_TYPE enumeration */
typedef enum
{
  GPSDIAG_LCS_USR_NO_NTFY_NO_VRFY            = 0,
  GPSDIAG_LCS_USR_NTFY_ONLY                  = 1,
  GPSDIAG_LCS_USR_NTFY_VRFY_ALLOW_NO_RSP     = 2,
  GPSDIAG_LCS_USR_NTFY_VRFY_NOT_ALLOW_NO_RSP = 3,
  GPSDIAG_LCS_USR_PRIVACY_NEEDED             = 4,
  GPSDIAG_LCS_USR_PRIVACY_OVERRIDE           = 5,
  GPSDIAG_LCS_USR_NTFY_PRIV_MAX
} gpsdiag_eNotifPrivType;

/* CGPS_NTFY_VRFY_SND_USR_ACTN_CMD request msg LCS_FLAGS enumeration */
typedef enum
{
  GPSDIAG_LCS_CLIENT_NAME_PRESENT            = 0x0001,
  GPSDIAG_LCS_CLIENT_EXTADDR_PRESENT         = 0x0002,
  GPSDIAG_LCS_SS_INVOKE_ID_PRESENT           = 0x0004,
  GPSDIAG_LCS_URL_PRESENT                    = 0x0008,
  GPSDIAG_LCS_DEF_LOC_TYPE_PRESENT           = 0x0010,
  GPSDIAG_LCS_REQ_ID_PRESENT                 = 0x0020,
  GPSDIAG_LCS_CODEWORD_PRESENT               = 0x0040,
  GPSDIAG_LCS_SERVICE_TYPE_ID_PRESENT        = 0x0080,
  GPSDIAG_LCS_ENCODING_TYPE_PRESENT          = 0x0100,
} gpsdiag_ePdsmLcsFlags;

/* CGPS_NTFY_VRFY_SND_USR_ACTN_CMD request msg POS_TECH_INDICATOR enumeration */
typedef enum
{
  GPSDIAG_LCS_POS_TECH_S_SHOT      = 0,
  GPSDIAG_LCS_POS_TECH_TRACK       = 1,
  GPSDIAG_LCS_POS_TECH_CELL_S_SHOT = 2,
  GPSDIAG_LCS_POS_TECH_CELL_TRACK  = 3,
  GPSDIAG_LCS_POS_TECH_CACHED      = 4,
} gpsdiag_ePosTechIndctr;

/* CGPS_NTFY_VRFY_SND_USR_ACTN_CMD request msg POS_MODE enumeration */
typedef enum
{
  GPSDIAG_LCS_MSA_ONLY             = 0,
  GPSDIAG_LCS_MSB_ONLY             = 1,
  GPSDIAG_LCS_MSA_PREF_MSB_ALLOWED = 2,
  GPSDIAG_LCS_MSB_PREF_MSA_ALLOWED = 3,
} gpsdiag_ePositionMode;

#endif /* INCLUDE_LCS_CMDS */

#endif /* INCLUDE_PDAPI_TEST */

/* GPS_xxx_CMD response msg STATUS enumeration */
typedef enum
{
  GPSDIAG_SUCCESS = 0,
  GPSDIAG_ERROR
} gpsdiag_eStatusType;

/******************************************************************************/
/*************************** GPS DIAG QUERY ***********************************/
/******************************************************************************/
/*
What version of Query command packet are we using? 
*/
#define GPSDIAG_QUERY_CMD_VERSION_1             1

#define GPSDIAG_QUERY_SM_MAJOR_VERSION       1 /* 1 = Gen8 */
#define GPSDIAG_QUERY_SM_MINOR_VERSION       0

#define GPSDIAG_QUERY_DIAG_DESCRIPTION_LENGTH_MAX  (255)

typedef enum {
   GPSDIAG_QUERY_SUBSYSTEM_TYPE_SM = 0,
   GPSDIAG_QUERY_SUBSYSTEM_DIAG,
   GPSDIAG_QUERY_SUBSYSTEM_PDAPI,
   GPSDIAG_QUERY_SUBSYSTEM_PE,
   GPSDIAG_QUERY_SUBUSYSTE_ME,

   /* add newer sub-systems above this line */
   GPSDIAG_QUERY_SUBSYSTEM_MAX = 0xffff

} gpsdiag_QuerySubSystemType;

typedef enum {
   GPSDIAG_QUERY_SUBCOMMAND_GET_SM_VERSION = 0,

   /* add newer sub-commands above this line */
   GPSDDIAG_QUERY_SUBCOMMAND_MAX = 0xffff
} gpsdiag_QuerySubCommandType;

typedef enum {
   GPSDIAG_QUERY_STATUS_SUCCESS = 0,
   GPSDIAG_QUERY_STATUS_ERROR_BAD_PARAMS,
   GPSDIAG_QUERY_STATUS_ERROR_BAD_PKT_LENGTH,
   GPSDIAG_QUERY_STATUS_ERROR_VERSION,
   GPSDIAG_QUERY_STATUS_ERROR_SUBSYSTEM,
   GPSDIAG_QUERY_STATUS_ERROR_SUBCOMMAND,
   GPSDIAG_QUERY_STATUS_ERROR_NOT_IMPLEMENTED,


   /* add newer enumerations above this line */
   GPSDIAG_QUERY_STATUS_ERROR_UNKNOWN = 0xfffe,
   GPSDIAG_QUERY_STATUS_LAST_RESERVED = 0xffff

} gpsdiag_QueryStatusType;


typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_SubsystemType;  /* denotes which subsystem to query See gpsdiag_QuerySubSystemType */
  uint32             q_Subcommand;     /* to query version of a particular subcomamnd supported by the given
                                          subsystem
                                       */
  uint32             q_Reserved[2];
} gpsdiag_QueryVersionReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_SubsystemType;
  uint32             q_Subcommand;
  uint32             q_Status;         /* what was the status of query command? See gpsdiag_QueryStatusType */
  uint32             q_SubCommandVersion; /* Which version of the subcommand is supported? Depends on sub-system */
  uint32             q_MajorRev;       /* sub-system Major Rev */
  uint32             q_MinorRev;       /* sub-system Minor Rev */
  uint32             q_Reserved[4];    
  uint8              u_DescriptionLength; /* length of u_Description, including terminatino null. 
                                             See GPSDIAG_QUERY_DIAG_DESCRIPTION_LENGTH_MAX */
  uint8              u_Description[1];  /* 0-255 bytes as specified by u_DescriptionLength. Contains
                                         file id for diag pkt xor-ed with FF. See gpsdiag_GetDescriptionString */
} gpsdiag_QueryVersionRespType;

/******************************************************************************/
/********************************* XSPI ***************************************/
/******************************************************************************/
typedef enum {
   GPSDIAG_XSPI_UNKNOWN = 0,
   GPSDIAG_XSPI_STATIONARY = 1,
   GPSDIAG_XSPI_NON_STATIONARY = 2,

   GPS_DIAG_XSPI_MAX
} gpsdiag_XSpiType;

typedef enum {
   GPSDIAG_XSPI_CMD_STATUS_ERROR_NONE = 0,
   GPSDIAG_XSPI_CMD_STATUS_ERROR_UNKNOWN,
   GPSDIAG_XSPI_CMD_STATUS_ERROR_INVALID_PARAMS,
   GPSDIAG_XSPI_CMD_STATUS_BAD_PKT_LENGTH,
   GPSDIAG_XSPI_CMD_STATUS_PDAPI_ERROR,

   GPSDIAG_XSPI_CMD_STATUS_MAX
} gpsdiag_XSpiInjectionCmdStatusType;

typedef PACKED struct PACKED_POST
{
   gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
   int32              l_ClientId;       /* client id obtained by CGPS_START_DIAG */
   uint8              u_Confidence;     /* expressed as percentage. range 0 - 100*/
   uint32             q_Mode;           /* see gpsdiag_XSpiInjectionCmdStatusType */
   uint32             q_Reserved[2];
} gpsdiag_XSpiInjectionReqType;

typedef PACKED struct PACKED_POST
{
   gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
   int32              l_ClientId;       /* client id obtained by CGPS_START_DIAG */
   uint32             q_Status;         /* see gpsdiag_XSpiInjectionCmdStatusType */
   uint32             q_Reserved[2];
} gpsdiag_XSpiInjectionRespType;

/******************************************************************************/
/***************** OEM Feature Related data structures ************************/
/******************************************************************************/
typedef uint32 gpsdiag_OemFeatureType;
#define GPSDIAG_OEMFEATURE_NONE     0
#define GPSDIAG_OEMFEATURE_DRE      1
#define GPSDIAG_OEMFEATURE_DRSYNC   2
#define GPSDIAG_OEMFEATURE_MAX      0xFFFFFFFF

/*****************************/
/*         OEM CONTROL       */
/*****************************/
#define GPSDIAG_OEM_DRE_ON         1
#define GPSDIAG_OEM_DRE_OFF        2 
#define GPSDIAG_OEM_DRSYNC_ARM     3
#define GPSDIAG_OEM_DRSYNC_GET_CFG 4
#define GPSDIAG_OEM_DRSYNC_SET_CFG 5 /*This is only used in control response */


typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType     z_DiagCmdHdr;  /* diagpkt_subsys_header_type */
  gpsdiag_OemFeatureType q_OemFeature;  /* which oem feature, e.g. oemdre etc */
  uint32                 q_OemCommand;  /* oem command. e.g. on/off/etc */
  uint32                 q_Reserved[2];
} gpsdiag_OemControlReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType     z_DiagCmdHdr;  /* diagpkt_subsys_header_type */
  gpsdiag_OemFeatureType q_OemFeature;  /* which oem feature, e.g. oemdre etc */
  uint32                 q_OemCommand;  /* oem command. e.g. on/off/etc */
  uint32                 q_RespResult;
  uint32                 q_Reserved[2];
} gpsdiag_OemControlRespType;


/* Memory Allocation Failure Test */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType     z_DiagCmdHdr;  /* diagpkt_subsys_header_type */
  uint32                 q_Mode;        /* Bit 0 set: Simulate failure */
  uint32                 q_Reserved[3];
} gpsdiag_TestForceMemAllocErrorReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType     z_DiagCmdHdr;  /* diagpkt_subsys_header_type */
  uint32                 q_RespResult;
  uint32                 q_Reserved[3];
} gpsdiag_TestForceMemAllocErrorRespType;




/*****************************/
/*       OEM DRSync          */
/*****************************/
typedef uint32 gpsdiag_DrSyncMode;
/* No timed pulse output */
#define  GPSDIAG_DRSYNC_TIMED_OUTPUT_OFF      0
/* Timed pulses will be generated without re-arm messages.*/
#define  GPSDIAG_DRSYNC_TIMED_OUTPUT_WO_REARM 1

/* After the initial timed pulse, every generation of a timed 
   pulse is conditioned on receiving of a "Re-Arm" message 
   from the client application. */
#define  GPSDIAG_DRSYNC_TIMED_OUTPUT_W_REARM  2

/* Timed pulses will be generated at the fastest rate permitted by software and 
   a re-arm message is required for each pulse. Currently, the fastest pulse rate 
   in this mode shall be 1 HZ*/
#define GPSDIAG_DRSYNC_TIMED_OUTPUT_ON_DEMAND  3

typedef uint32 gpsdiag_DrSyncPolarity;
#define GPSDIAG_DRSYNC_POLARITY_POSITIVE  0
#define GPSDIAG_DRSYNC_POLARITY_NEGATIVE  1

typedef PACKED struct PACKED_POST {
  boolean                b_TuncIgnore;            /* Flag to ignore TUNC */
  boolean                b_Armed;                 /* Flag to arm the next pulse */
  int32                  l_PhaseSec;              /* The phase in seconds of the pulse 
                                                     relative to GPS week */
  int32                  l_PeriodSec;             /* The period of the pulse in seconds */
  gpsdiag_DrSyncMode     q_Mode;                  /* DR Sync mode */
  gpsdiag_DrSyncPolarity q_Polarity;              /* Polarity of the sync pulse */
  int32                  f_TuncStartThresholdMs;  /* The time uncertainty below which 
                                                     the pulse generation will start */
  int32                  f_TuncStopThresholdMs;   /* The time uncertainty above which 
                                                     the pulse generation will stop */
  int32                  f_BiasNs;                /* Bias of the pulse in nsec */
} gpsdiag_DRSyncConfig;

typedef PACKED struct PACKED_POST {
  uint16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  uint32 q_Count;       /* Number of pulses generated */
  FLT    f_GpsMsec;     /* GPS msec at reference tick [msecs] */
  FLT    f_ClkTimeUncMs;/* 1 sided max time bias uncertainty [msecs] */
} gpsdiag_DRSyncPulseDesc;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType     z_DiagCmdHdr;   /* diagpkt_subsys_header_type */
  gpsdiag_DRSyncConfig   z_DrSyncConfig;
} gpsdiag_OemDrSyncSetConfig;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType      z_DiagCmdHdr;  /* diagpkt_subsys_header_type */
  gpsdiag_DRSyncPulseDesc z_DrSyncPulseDesc;
} gpsdiag_OemDrSyncRespType;

/* gpsOneXTRA DM event (EVENT_GPSONEXTRA_DATA_ACK) payload */
typedef PACKED struct PACKED_POST
{
  int32                   l_ClientId;          /* Client id of user */
  uint32                  q_Status;            /* Status of the XTRA data inject request
                                                  Return value is of type pdsm_xtra_status_e_type. */
  uint32                  q_Data_Part_Number;    /* data part number */
  uint32                  q_Data_Total_Parts;    /* total data parts */
}gpsdiag_XtraDataAckEvtType;


/*****************************/
/* GM In memory logging */
/*****************************/
#define GPSDIAG_GEOFENCE_IN_MEM_LOG_START  1
#define GPSDIAG_GEOFENCE_IN_MEM_LOG_STOP   2


typedef PACKED struct PACKED_POST
{
  uint32 q_log_size;
  uint32 q_period_s;
} gpsdiag_GeofenceLogConfig;

/* Set geofence In memory logging  Req/Rsp packets
 *
 * This msg is used to set the geofence in memory logging logging 
 */
typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  int32              l_ClientId;       /* Test Client ID */
  uint32             q_GeofenceCommand;
  gpsdiag_GeofenceLogConfig z_LogConfig;
} gpsdiag_GeofenceInMemLogCmdReqType;

typedef PACKED struct PACKED_POST
{
  gpsdiag_CmdHdrType z_DiagCmdHdr;     /* diagpkt_subsys_header_type */
  uint32             q_Status;         /* See ENUM gpsdiag_eStatusType */
  int32              l_ClientId;       /* Test Client ID */
} gpsdiag_GeofenceInMemLogCmdRspType;


#endif /* CGPS_DIAG_H */
