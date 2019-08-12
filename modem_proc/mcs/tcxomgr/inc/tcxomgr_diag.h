#ifndef TCXOMGR_DIAG_H
#define TCXOMGR_DIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    T C X O M G R    D I A G

GENERAL DESCRIPTION
  This module contains functions pertaining to the search diag interface

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/inc/tcxomgr_diag.h#1 $

when         who      what, where, why
--------     ---      ----------------------------------------------------------
06/04/15     tl       Update log packet versions
01/30/15     tl       Remove obsolete featurization
01/08/15     tl       Pack log packet data structures in off-target builds
05/01/14     tl       Update field cal packet version to support smaller FT bins
10/28/13     mn       Support for client and subscription RGS retrieval log packet.
01/29/13     sk       Support for frequency estimate separation
06/12/13     mn       Adding CNo to the GPS XO cal interface.
02/04/13     mn       New log packet for Retrieve RGS.
10/18/12     mn       Added t0 to the field cal log packet.
07/13/12     gs       Support discarded RGS logging
07/09/12     sg       Support for setting uncertainty and age using diag
05/23/12     mn       Support to reset RGS through the SET_UNCERTAINTY diag command.
04/23/12     sg       Support for factory fine cal removal
02/01/12     sr       TCXOMGR Feature Cleanup
01/31/12     ns       Support for IFC history, filtering and voting packets
01/27/12     ns       Support for IFC
07/08/11     ns       Adding RGS updates as log packet and also enchanced
                      Field cal log packets
02/16/10     ns       Revert back TCXOMGR_AFC_LOG_VERSION to 5. It was changed
                      to 6 incorrectly. 6 is for Multi carrier AFC packets.
11/04/10     sm       Added Support For Fast Field Cal
08/12/10     bb       Code changes to update "TCXOMGR_AFC_LOG_VERSION" 6
03/02/10     ns       IQ logging support
10/20/08     adw      Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
08/28/08     adw      Added FEATURE_XO_DIAG to featurize for ULC.
04/07/08     ljl      Supported multi-carrier AFC log.
11/16/07     va       Remove TCXO Diag cal functions.
                      XO Freq Est logging Support
                      Bumped AFC Log version num
09/13/07     sst      Correct featurization
09/04/07     cab      Added support for diag control of slope error
                      Field cal logging support
07/20/07     sst      Update FacCal to increase offset size and enable NV write
06/06/07     sst      Added debug msg levels and Cal diag commands
10/10/06     cab      Initial version for tcxomgr v3
===========================================================================*/

#include "mcs_variation.h"
#include "customer.h"


#include "comdef.h"
#include "msg.h"
#include "log_codes.h"
#include "log.h"

#include "diagpkt.h"
#include "tcxomgr_i.h"

#ifdef T_WINNT
#error code not present
#endif

/*--------------------------------------------------------------------------
                        Message Mask Definations
--------------------------------------------------------------------------*/
/* the following are required for all subsystems and are defined in msg.h
 *
 * MSG_LEGACY_LOW      MSG_MASK_0
 * MSG_LEGACY_MED      MSG_MASK_1
 * MSG_LEGACY_HIGH     MSG_MASK_2
 * MSG_LEGACY_ERROR    MSG_MASK_3
 * MSG_LEGACY_FATAL    MSG_MASK_4
 *
 * The remaining bits are sub subsystem specific. 1X search specific
 * ones are defined below. To enable, ensure the bit mask is set
 * correctly in custsrch.h
 */

// For future use: #define MSG_DBG_INIT          MSG_MASK_5

#define MSG_DBG_XO_TRIM          MSG_MASK_5      /* XO Trim               */
#define MSG_DBG_FAC_CAL          MSG_MASK_6      /* Factory Calibration   */
#define MSG_DBG_FIELD_CAL        MSG_MASK_7      /* Field Calibration     */

/*
 * Enum to denote the version number of coarse cal
 * diag command
 */
typedef enum
{
  TCXOMGR_DIAG_COARSE_DC_CAL_CMD_V1 = 1
} tcxomgr_diag_coarse_dc_cal_cmd_version_enum_type;

/*
 * Enum to denote the version number of coarse cal
 * diag response
 */
typedef enum
{
  TCXOMGR_DIAG_COARSE_DC_CAL_RSP_V1 = 1
} tcxomgr_diag_coarse_dc_cal_r_version_enum_type;

/*--------------------------------------------------------------------------
                    Subsystem Dispatch Request Response
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                    AFC LOG PACKET INFO
--------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_TCXOMGR_AFC_DATA_C)

   uint8                               version;
     /* The version number of the packet. */

   tcxomgr_log_header_struct_type      afc_header_info;

   tcxomgr_log_entries_struct_type     entries[1];

LOG_RECORD_END

/* Macro for variable size log packet allocation. */
#define TCXOMGR_VAR_SIZEOF( main_type, sub_type, sub_cnt ) \
           ( sizeof( main_type ) + ( (sub_cnt-1) * sizeof( sub_type ) ) )

/* To calculate the log buffer size
   with variable carriers and variable entries.
   Will clean up the calculation when version 5 log is phased out. */
#define TCXOMGR_MC_VAR_SIZE( mc_log_header_size,                         \
                             mc_log_carriers_size,                       \
                             mc_log_entries_size )                       \
        (                                                                   \
          (                                                                 \
            (                                                               \
              (                                                             \
                sizeof(LOG_TCXOMGR_AFC_DATA_C_type) -                       \
                sizeof(tcxomgr_log_header_struct_type)                      \
              ) -                                                           \
              sizeof(tcxomgr_log_entries_struct_type)                       \
            ) +                                                             \
            mc_log_header_size + mc_log_carriers_size + mc_log_entries_size \
          )                                                                 \
        )

#define TCXOMGR_AFC_LOG_VERSION            5
#define TCXOMGR_MC_AFC_LOG_VERSION         6

/*--------------------------------------------------------------------------
                    IQ DATA LOG PACKET INFO
--------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_XO_IQ_DATA_C)

   uint8                               version;
     /* The version number of the packet. */

   tcxomgr_log_iq_data_struct_type    iq_data;

LOG_RECORD_END

#define TCXOMGR_IQ_DATA_LOG_VERSION      1

/*--------------------------------------------------------------------------
                    FIELD CAL LOG PACKET INFO
--------------------------------------------------------------------------*/
/* Version 4 introduces the concept of opcodes */
#define TCXOMGR_FIELD_CAL_LOG_VERSION              6
#define TCXOMGR_FIELD_CAL_LOG_CAL_SUBCODE          1
#define TCXOMGR_FIELD_CAL_LOG_RGS_SUBCODE          2

typedef PACK (struct)
{
   log_hdr_type                        hdr;
   uint8                               version;
   uint8                               subcode;
   uint16                              id;
   tcxomgr_log_field_cal_struct_type   field_cal_data;
} tcxomgr_log_field_cal_calibration_data_type;

#define TCXOMGR_RGS_LOG_VERSION 3
#define TCXOMGR_RGS_LOG_SUBCODE 1

typedef PACK (struct)
{
   log_hdr_type                        hdr;
   uint8                               version;
   uint8                               subcode;
   uint16                              id;
   tcxomgr_log_rgs_struct_type         rgs_data;
} tcxomgr_log_rgs_data_type;

#define TCXOMGR_TEMP_SUBSCRIPTION_LOG_VERSION 2
#define TCXOMGR_TEMP_SUBSCRIPTION_LOG_SUBCODE 1

typedef PACK (struct)
{
   log_hdr_type                               hdr;
   uint8                                      version;
   uint8                                      subcode;
   uint16                                     id;
   tcxomgr_log_temp_subscription_struct_type  subs_data;
} tcxomgr_temp_subscription_data_type;
#define TCXOMGR_RGS_RETRIEVAL_LOG_VERSION 3

#define TCXOMGR_RGS_RETRIEVAL_LOG_SUBCODE               1
#define TCXOMGR_RGS_AGED_RETRIEVAL_LOG_SUBCODE          2

typedef PACK (struct)
{
   log_hdr_type                           hdr;
   uint8                                  version;
   uint8                                  subcode;
   uint16                                 id;
   tcxomgr_log_rgs_retrieval_struct_type  rgs_data;
} tcxomgr_log_rgs_retrieval_data_type;

typedef PACK (struct)
{
   log_hdr_type                               hdr;
   uint8                                      version;
   uint8                                      subcode;
   uint16                                     id;
   tcxomgr_log_aged_rgs_retrieval_struct_type rgs_data;
} tcxomgr_log_aged_rgs_retrieval_data_type;

typedef PACK (struct)
{
  log_hdr_type                        hdr;
  uint32                              id;

  tcxomgr_coarse_dc_cal_req_data_type req_data;
  tcxomgr_coarse_dc_cal_rsp_data_type rsp_data;

} tcxomgr_log_coarse_dc_cal_data_type;

/*--------------------------------------------------------------------------
                    IFC LOG PACKET INFO
--------------------------------------------------------------------------*/
#define TCXOMGR_IFC_LOG_SUMMARY_VERSION                  1
#define TCXOMGR_IFC_LOG_SUMMARY_BUF_CREATION_SUBCODE     1
#define TCXOMGR_IFC_LOG_SUMMARY_BUF_STATS_SUBCODE        2
#define TCXOMGR_IFC_LOG_SUMMARY_BUF_QUEUE_SUBCODE        3
#define TCXOMGR_IFC_LOG_SUMMARY_CAL_SUBCODE              4
#define TCXOMGR_IFC_LOG_SUMMARY_CAL_HISTORY_SUBCODE      5
#define TCXOMGR_IFC_LOG_SUMMARY_CAL_FILTERED_SUBCODE     6
#define TCXOMGR_IFC_LOG_SUMMARY_CAL_VOTING_SUBCODE       7

#define TCXOMGR_IFC_LOG_RPUSH_VERSION                    1
#define TCXOMGR_IFC_LOG_RPUSH_INFO_SUBCODE               1

typedef PACK (struct)
{
   log_hdr_type                             hdr;
   uint8                                    version;
   uint8                                    subcode;
}tcxomgr_log_ifc_template;


/* list of search diag request indexes */
typedef enum
{
  TXCOMGR_DIAG_MAX_EXTERNAL_ID,

  TXCOMGR_DIAG_START_INTERNAL_ID = 0xF0,
  TEMP_TABLE_OVERRIDE            = 0xF1,
  TEMP_TABLE_READ                = 0xF2,
  VCO_DFLT_OVERRIDE              = 0xF3,
  VCO_SLOPE_OVERRIDE             = 0xF4,
  SLOPE_RANGE_OVERRIDE           = 0xF5,
  RGS_OVERRIDE                   = 0xF6,
  RUN_TEST_CASE                  = 0xF7,
  RFE_SLOPE_OVERRIDE             = 0xF8,
  SET_UNCERTAINTY                = 0xF9,
  SET_AGE                        = 0xFA,
  COARSE_DC_CAL                  = 0xFB,
  TXCOMGR_DIAG_MAX_ID
} tcxomgr_diag_table_id_type;

/* Typedef for the override command from the QXDM */
typedef enum
{
  NO_OVERRIDE       = 0,
  OVERRIDE,
  OVERRIDE_AND_WRITE
} override_cmd_type;


#ifdef T_WINCE
#pragma pack(1)
#endif /* T_WINCE */
typedef PACK (struct)
{
  uint8                         system;
  int16                         delta_temp;
  int16                         vco;
  int16                         rot;
  uint32                        time;
}
tcxomgr_diag_rgs_struct_type;
#ifdef T_WINCE
#pragma pack()
#endif /* T_WINCE */


/*--------------------------------------------------------------------------
                             TEMP TABLE OVERRIDE

This interface provides a way for a test tool to override the current
temperature vs vco table.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, TEMP_TABLE_OVERRIDE)
  override_cmd_type enable;    /* 0 = disable, 1 = enable */
  tcxomgr_vco_type  temp[ VCO_TEMP_TABLE_SIZE ];
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, TEMP_TABLE_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             TEMP TABLE READ

This interface provides a way for a test tool to read the temperature vs
vco table.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, TEMP_TABLE_READ)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, TEMP_TABLE_READ)
  tcxomgr_vco_type  temp[ VCO_TEMP_TABLE_SIZE ];
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             DEFAULT VCO OVERRIDE

This interface provides a way for a test tool to override the current
 vco default value.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, VCO_DFLT_OVERRIDE)
  override_cmd_type enable;    /* 0 = disable, 1 = enable */
  tcxomgr_vco_type  dflt;      /* Default VCO value */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, VCO_DFLT_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                            VCO SLOPE OVERRIDE

This interface provides a way for a test tool to override the current
 vco slope.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, VCO_SLOPE_OVERRIDE)
  override_cmd_type enable;    /* 0 = disable, 1 = enable */
  uint16            slope;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, VCO_SLOPE_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             SLOPE RANGE OVERRIDE

This interface provides a way for a test tool to override the current
 slope range.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, SLOPE_RANGE_OVERRIDE)
  override_cmd_type enable;    /* 0 = disable, 1 = enable */
  uint8             slope_range;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, SLOPE_RANGE_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             RGS OVERRIDE

This interface provides a way for a test tool to override the current
 recent good system values.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, RGS_OVERRIDE)
  override_cmd_type             enable;    /* 0 = disable, 1 = enable */
  tcxomgr_diag_rgs_struct_type  rgs;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, RGS_OVERRIDE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             RUN TEST CASE

This interface provides a way for a test tool to override the current
 recent good system values.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, RUN_TEST_CASE)
  uint8             test_case;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, RUN_TEST_CASE)
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                             RFE SLOPE OVERRIDE

This interface provides a way for a test tool to override the AFC slope
 component of frequency estimation.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, RFE_SLOPE_OVERRIDE)
  boolean             enable;    /* 0 = disable, 1 = enable */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, RFE_SLOPE_OVERRIDE)
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                             SET UNCERTAINTY

This interface provides a way for a test tool to set the uncertainty that
tcxomgr passes to RATs.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, SET_UNCERTAINTY)
  uint16    uncertainty; /* Uncertainty returned to RATs should be in ppb */
  int8      error;       /* Error from FT curve returned to RATs */
  uint8     rgs_status;   /* Flag to reset the RGS. A non-zero value will reset the RGS */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, SET_UNCERTAINTY)
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                             SET AGE

This interface provides a way for a test tool to set the age of
coeffecients which allows for testing aging related changes.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, SET_AGE)
  uint8     age;       /* Age in months */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, SET_AGE)
DIAGPKT_RSP_END

/*test purpose*/
/*--------------------------------------------------------------------------
                             TEMP_SUBSCRIBE

This interface provides a way for a test tool to subscribe
unsubscribe/modify parameters for temp read subscription
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(TCXOMGR, TEMP_SUBSCRIBE)
  uint8     id;
  uint8     option;
  uint16    time_period;
  uint32    temp_channel;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(TCXOMGR, TEMP_SUBSCRIBE)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             COARSE DC CAL

This interface provides a way to invoke Coarse Calibration through a diag
command.
--------------------------------------------------------------------------*/

typedef PACK(struct)
{
  diagpkt_subsys_header_type header;

  /* Unique Identifier for the command. The response for this
     command will have the same identifier. */
  uint32 id;

  /* Payload of the request */
  tcxomgr_coarse_dc_cal_req_data_type data;
} trxomgr_diag_dc_coarse_cal_req_struct_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_v2_type header;

  /* A unique identifier for a command response pair. The
     response will have the identifier same as the corresponding
     command. */
  uint32 id;

  /* Payload of the response */
  tcxomgr_coarse_dc_cal_rsp_data_type data;
} trxomgr_diag_dc_coarse_cal_delayed_rsp_struct_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_v2_type header;
} trxomgr_diag_dc_coarse_cal_imm_rsp_struct_type;

#ifdef T_WINNT
#error code not present
#endif

/*===========================================================================

FUNCTION TCXOMGR_DIAG_INIT

DESCRIPTION    This function is called at startup to load Diag with the
      TCXOMGR logging dispatch table.

DEPENDENCIES   tcxomgr_diag_tbl.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void tcxomgr_diag_init (void);


/*===========================================================================

FUNCTION SRCH_DIAG_GET_TEMP_TABLE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the temp table override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_temp_table_override_enable(void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_VCO_DFLT_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the vco_dflt override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_vco_dflt_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_VCO_SLOPE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the vco_slope override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_vco_slope_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_SLOPE_RANGE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the SLOPE_RANGE override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_slope_range_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the RGS override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_rgs_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RFE_SLOPE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the rfe_slope override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean tcxomgr_diag_get_rfe_slope_override_enable( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_TIME

DESCRIPTION    This function returns the delta time for the overriden RGS

DEPENDENCIES   None.

RETURN VALUE   uint32 delta time.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 tcxomgr_diag_get_rgs_time( void );


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_TEMP_DIFF

DESCRIPTION    This function returns the temperature difference for the overriden RGS

DEPENDENCIES   None.

RETURN VALUE   int16 delta temp.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 tcxomgr_diag_get_rgs_temp_diff( void );

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_FIELD_CAL_LOG

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tcxomgr_diag_send_iq_data_log
(
  tcxomgr_log_iq_data_struct_type *raw_data
);


/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_FIELD_CAL_LOG

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tcxomgr_diag_send_field_cal_log
(
  tcxomgr_log_field_cal_struct_type *field_cal_data,
  uint16 id
);

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_RGS_LOG

DESCRIPTION: Logs new RGS log packet to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_rgs_log
(
  tcxomgr_log_rgs_struct_type *rgs_data,
  uint16 id
);

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_RGS_RETRIEVAL_LOG

DESCRIPTION: Logs new RGS retrieval log packet to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_rgs_retrieval_log
(
  tcxomgr_log_rgs_retrieval_struct_type *rgs_data,
  uint16 id
);

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_TEMP_SUBS_LOG

DESCRIPTION: Logs new TEMP SUBSCRIPTION log packet to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_temp_subs_log
(
  tcxomgr_log_temp_subscription_struct_type *temp_subscribe,
  uint16 id
);

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_AGED_RGS_RETRIEVAL_LOG

DESCRIPTION: Logs new RGS retrieval log packet to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_aged_rgs_retrieval_log
(
  tcxomgr_log_aged_rgs_retrieval_struct_type *rgs_data,
  uint16 id
);

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_LOG_IFC

DESCRIPTION: Sends IFC log packets to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_log_ifc
(
  int32 log_id,
  int32 log_version,
  int32 log_sub_code,
  size_t payload_size,
  void *payload
);

/*===========================================================================

FUNCTION TCXOMGR_DIAG_COARSE_DC_CAL_RSP

DESCRIPTION    This function is called to respond to the
               XO Coarse DC calibration command.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void tcxomgr_diag_coarse_dc_cal_rsp
(
  uint32                               id,            /* Identifier from req       */
  tcxomgr_coarse_dc_cal_req_data_type* req_data,      /* Request data              */
  diagpkt_subsys_status_type status,                  /* Status indicating success
                                                          or failure               */
  tcxomgr_cal_fac_param_type      *fac_cal_param,     /* factory cal parameters    */
  tcxomgr_cal_info_type           *info_pkt,          /* cal info pkt              */
  tcxomgr_cal_results_info_struct *cal_results        /* Cal results               */
);

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_COARSE_DC_CAL_LOG

DESCRIPTION: Logs new coarse cal log packet to DIAG.

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_coarse_dc_cal_log
(
  uint32                               id,
  tcxomgr_cal_failure_enum_type        fail_reason,
  tcxomgr_coarse_dc_cal_req_data_type* req_data,
  tcxomgr_coarse_dc_cal_rsp_data_type* rsp_data
);

#endif /* TCXOMGR_DIAG_H */

