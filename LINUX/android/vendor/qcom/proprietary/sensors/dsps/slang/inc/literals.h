/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: literals.h
** DESC: sensor language - slang - literal string constants
** Revision History
** ================================================================ */
#ifndef LITERALS_H
#define LITERALS_H
/*
** field names
** -------- */
typedef enum {
    DIE = 0,
    TODO,
    OOM,
    BAD_ASSIGNMENT,
    BAD_CLIENT,
    BAD_TIMEOUT,
    BAD_FIELD,
    BAD_SRVC_NUM,
    BAD_MSG_ID,
    BAD_INDEX,
    BAD_VALUE,
    BAD_ARGUMENT,
    UNKNOWN_SENSOR,
    UNINITIALIZED_SYMBOL,
    UNSUPPORTED_SERVICE,
    NOT_YET_SUPPORTED,
    MISSING_LENGTH,
    TIMEDOUT,
    MALFORMED_PACKET,

    CLIENT,                        // respind
    SRVC_NUM,
    MSG_ID,
    MSG_TYPE,
    MSG_SIZE,
    TXN_ID,
    RESULT,                        // sns_common_resp_s_v01
    ERR,                           // +

    REPORTID,                      // sns_smgr_periodic_report_req_msg_v01
    ACTION,
    REPORTRATE,
    ITEM,
    SENSORID,
    DATATYPE,
    DECIMATION,
    CALIBRATION,
    SAMPLERATE,
    SAMPLEQUALITY,
    NOTIFY_SUSPEND,

    ACKNAK,                         // sns_smgr_periodic_report_resp_msg_v01
    REASONPAIR,
    REASON,

    STATUS,                         // sns_smgr_periodic_report_ind_msg_v01
    CURRENTRATE,
    DATAX,
    DATAY,
    DATAZ,
    SSCTS,                          // measurement timestamp aka dsps_ts aka adsp_ts
    FLAGS,
    QUALITY,
    INDTYPE,

    INFO,
    SHORTNAME,
    SENSOR_NAME,
    VENDOR,
    MAX_RATE,
    IDLE_POWER,
    MAX_POWER,
    MAX_RANGE,
    RESOLUTION,
    MAX_REPORTS,
    OP_MODE,
    SUID,
    ODR_LIST,
    ODR,

    ADD,
    DELETE,
    PRIMARY,
    SECONDARY,
    SAMPLE,
    FILTER,
    RAW,
    FACTORY,
    FULL,

    DATA,
    LENGTH,

    TESTTYPE,
    SAVE,
    APPLY,
    TESTRESULT,
    ERRORCODE,

    INSTANCE_ID,
    REPORT_PERIOD,

    ALGORITHM_REVISION,
    PROC_TYPE,
    REPORTING_MODES,
    MIN_REPORT_RATE,
    MAX_REPORT_RATE,
    MIN_SAMPLE_RATE,
    MAX_SAMPLE_RATE,
    MAX_BATCH_SIZE,
    POWER,

    VERSION,
    MAX_MSG_ID,
    STATE,

    REG_REPORT,                             // sns_time_timestamp_req_msg_v02
    SSC_TIMESTAMP,
    APPS_TIMESTAMP,
    ERROR_CODE,
    SSC_ROLLOVER_COUNT,
    APPS_BOOTTIME,

    BATCH_PERIOD,
    MOTION_STATE,

    GRAVX,
    GRAVY,
    GRAVZ,
    LINACCX,
    LINACCY,
    LINACCZ,
    ACCURACY,

    REPORT,
    IND_TYPE,
    COORDSYS,

    GRV0,
    GRV1,
    GRV2,
    GRV3,

    ROTV0,
    ROTV1,
    ROTV2,
    ROTV3,

    ROTV_ACCURACY,
    GRAVITY_ACCURACY,

    STEP_COUNT_THRESHOLD,
    DUTY_CYCLE_ON_PERCENT,

    RESET,

    STEP_EVENT,
    STEP_CONFIDENCE,
    STEP_COUNT,
    STEP_COUNT_ERROR,
    STEP_RATE,

} literals_e;

char *get_literal( literals_e name);

#endif //LITERALS_H


