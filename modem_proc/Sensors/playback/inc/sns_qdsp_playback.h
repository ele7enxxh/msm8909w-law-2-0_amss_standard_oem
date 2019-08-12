/******************************************************************************
  @file    sns_qdsp_playback.h
  

  ---------------------------------------------------------------------------
  Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------*/


#define PLAYBACK_CONFIG_FILE "qdsp_playback/qdsp_playback.cfg"
#define PLAYBACK_LOG_FILE "qdsp_playback/qdsp_playback_log.dlf"
#define PLAYBACK_STATUS_FILE "qdsp_playback/qdsp_playback_sim_status"
#define PLAYBACK_SIM_DONE 1
#define MAX_ALGO 1

#define strtok_r strtok_s

extern FILE *qdsp_sim_log;

typedef struct plbk_msg_req_s
{
  unsigned int    plbk_cient;
  unsigned int    report_req;
  void           *rep_req_data;
  unsigned int    rep_req_size;
  void           *rep_resp_data;
  unsigned int    rep_resp_size;
  void           *resp_clbk_data;
} plbk_msg_req_s;

typedef struct
{
   int svc_id;
   int num_config_elem; 
   int algo_config[8];   /* should be enough to hold config for one algo */
} pb_algo_attrib_s;
