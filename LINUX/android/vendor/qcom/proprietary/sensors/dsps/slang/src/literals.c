/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: literals.c
** DESC: sensor language - slang - string constant literals
** Revision History
** ================================================================ */
#include "literals.h"

char *literals[] = {
    /*
    ** error messages
    ** -------------- */
    "die",
    "TODO",
    "out of memory",
    "bad assignment",
    "bad client handle",
    "bad timeout value",
    "bad field name",
    "bad srvc_num",
    "bad msg_id",
    "bad index",
    "bad value",
    "bad argument",
    "unknown sensor_id",
    "uninitialized symbol",
    "unsupported service",
    "not yet supported",
    "missing length",
    "TIMEDOUT",
    "malformed packet",

    "client",
    "srvc_num",
    "msg_id",
    "msg_type",
    "msg_size",
    "txn_id",
    "result",
    "err",

    "report_id",
    "action",
    "report_rate",
    "item",
    "sensor_id",
    "datatype",
    "decimation",
    "calibration",
    "sample_rate",
    "sample_quality",
    "notify_suspend",

    "acknak",
    "reasonpair",
    "reason",

    "status",
    "current_rate",
    "datax",
    "datay",
    "dataz",
    "sscts",
    "flags",
    "quality",
    "indtype",

    "info",
    "short_name",
    "sensor_name",
    "vendor",
    "max_rate",
    "idle_power",
    "max_power",
    "max_range",
    "resolution",
    "max_reports",
    "op_mode",
    "suid",
    "odr_list",
    "odr",

    "add",
    "delete",
    "primary",
    "secondary",
    "sample",
    "filter",
    "raw",
    "factory",
    "full",

    "data",
    "length",

    "testtype",
    "save",
    "apply",
    "testresult",
    "errorcode",

    "instance_id",
    "report_period",

    "revision",
    "proc_type",
    "modes",
    "min_report_rate",
    "max_report_rate",
    "min_sample_rate",
    "max_sample_rate",
    "max_batch_size",
    "power",

    "version",
    "max_msg_id",
    "state",

    "reg_report",
    "ssc_timestamp",
    "apps_timestamp",
    "error_code",
    "ssc_rollover_count",
    "apps_boottime",

    "batch_period",
    "motion_state",

    "gravx",
    "gravy",
    "gravz",
    "linaccx",
    "linaccy",
    "linaccz",
    "accuracy",

    "report",
    "ind_type",
    "coordsys",

    "grv0",
    "grv1",
    "grv2",
    "grv3",

    "rotv0",
    "rotv1",
    "rotv2",
    "rotv3",

    "rotv_accuracy",
    "gravity_accuracy",

    "step_count_threshold",
    "duty_cycle_on_percent",

    "reset",

    "step_event",
    "step_confidence",
    "step_count",
    "step_count_error",
    "step_rate",

    "too far"
};

char *get_literal( literals_e name)
{
    return literals[ name];
}


