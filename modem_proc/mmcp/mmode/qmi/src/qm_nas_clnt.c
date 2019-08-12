/*===========================================================================

                        QM_NAS_CLNT.C 

DESCRIPTION

  QMI_MMODE source file for nas client information module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:
  

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/13    sm     Initial version
===========================================================================*/

#include "qmi_mmode_svc.h"
#include "qm_nas_clnt.h"
#include "qm_util.h"
#include "qm_si.h"
#include "msg.h"
#include "qmi_nas_utils.h"

#define NASI_CMD_VAL_SET_E911_STATE                   0x007A
#define NASI_CMD_VAL_E911_STATE_READY_IND             0x007B


//===========================================================================
// GLOBALS
//===========================================================================
#define MAX_NUM_SIG_INFO_THRESHOLDS 16
#define MAX_NUM_SIG_INFO2_THRESHOLDS 32

struct config_sig_s
{
  uint32 threshold_list_len;
  float  threshold_list[MAX_NUM_SIG_INFO_THRESHOLDS];
};

struct config_sig2_s
{
  float  delta;
  uint32 threshold_list_len;
  float  threshold_list[MAX_NUM_SIG_INFO2_THRESHOLDS];
};

struct qm_nas_clnt_global_s
{
  boolean use_config_sig2;

  // config sig info information
  struct config_sig_s config_sig[QM_SIG_TYPE_MAX];
  struct config_sig_s tds_sinr_config;

  // config_sig_info2 information
  struct config_sig2_s config_sig2[QM_SIG_TYPE_MAX][QM_RAT_MAX];
};

static struct qm_nas_clnt_global_s *clnt_config_sig_info[QM_NAS_MAX_NUM_CLIDS];

//===========================================================================
// Function declarations
//===========================================================================
static boolean is_cmd_buf_p_valid( qmi_cmd_buf_type *cmd_buf_p ); 

/*=========================================================================== 
  FUNCTION IS_CMD_BUF_P_VALID() 
 
  DESCRIPTION 
    Validates cmd_buf_p pointer 
 
  PARAMETERS 
    cmd_buf_p: pointer to command buffer for client 
 
  RETURN VALUE 
    TRUE if valid, FALSE otherwise 
 
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
static boolean is_cmd_buf_p_valid( qmi_cmd_buf_type *cmd_buf_p ) 
{ 
  return qmi_nas_validate_cmd_buf_p( cmd_buf_p, &qmi_nas_state );
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_INIT()

  DESCRIPTION
    This function initializes the nas client information

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_init ( void )
{
  memset( (void*)clnt_config_sig_info, 0, sizeof(struct qm_nas_clnt_global_s *)*QM_NAS_MAX_NUM_CLIDS );
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_CONFIG_ALLOC()

  DESCRIPTION
    This function allocates memory for the nas client config information for a single client.

  PARAMETERS
    clid: client to allocate memory for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_config_alloc ( int32 clid )
{
  clnt_config_sig_info[clid-1] = (struct qm_nas_clnt_global_s *)modem_mem_alloc(sizeof(struct qm_nas_clnt_global_s), MODEM_MEM_CLIENT_QMI_MMODE);
  if(clnt_config_sig_info[clid-1])
  {
    memset( (void*)(clnt_config_sig_info[clid-1]), 0, sizeof(struct qm_nas_clnt_global_s) );
  }
  else
  {
    QM_MSG_ERROR_1("Memory was not allocated successfully to clnt_config_sig_info for clid %d", clid);
  }
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_RESET_INFO()

  DESCRIPTION
    This function clears out client specific information for the specified clid

  PARAMETERS
    clid: client to reset information for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_reset_info ( int32 clid )
{
  QM_MSG_HIGH_1("Clearing client config information for clid %d", clid);

  if ( QM_NAS_CLNT_ID_VALID(clid) && clnt_config_sig_info[clid-1] )
  {
    memset( clnt_config_sig_info[clid-1], 0, sizeof( struct qm_nas_clnt_global_s ) );
  }
  else
  {
    QM_MSG_ERROR_1("Memory was not reset successfully to clnt_config_sig_info for clid %d", clid);
  }
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_CONFIG_DEALLOC()

  DESCRIPTION
    This function deallocates memory for the nas client config information for a single client.

  PARAMETERS
    clid: client to deallocate memory for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_config_dealloc ( int32 clid )
{
  if(clnt_config_sig_info[clid-1])
  {
    modem_mem_free(clnt_config_sig_info[clid-1], MODEM_MEM_CLIENT_QMI_MMODE);
  }
  else
  {
    QM_MSG_ERROR_1("Memory was not deallocated successfully to clnt_config_sig_info for clid %d", clid);
  }
  clnt_config_sig_info[clid-1] = NULL;
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_PROCESS_CONFIG_SIG_INFO()

  DESCRIPTION
    This function sets the config_sig_info information for the specific client

  PARAMETERS
    clid: client to store config_sig_info information for
    config: information to store for client 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_process_config_sig_info( int32 clid, nas_config_sig_info_req_msg_v01 *config )
{
  struct qm_nas_clnt_global_s *clnt;
  int i = 0;

  QM_MSG_HIGH_1("Storing config sig info for clid %d", clid);

  if ( config && QM_NAS_CLNT_ID_VALID(clid) )
  {
    clnt = clnt_config_sig_info[clid-1];
    
    if(!clnt)
    {
      QM_MSG_ERROR_1("qm_nas_clnt_process_config_sig_info(): Invalid clnt received for clid %d", clid);
      return;
    }
    
    clnt->use_config_sig2 = FALSE;
    memset( clnt->config_sig2, 0, sizeof(struct config_sig2_s)*QM_SIG_TYPE_MAX*QM_RAT_MAX );

    if ( config->rssi_threshold_list_valid )
    {
      memset( clnt->config_sig[QM_SIG_TYPE_RSSI].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt->config_sig[QM_SIG_TYPE_RSSI].threshold_list_len = config->rssi_threshold_list_len;
      for ( i = 0; i < config->rssi_threshold_list_len; i++ )
      {
        clnt->config_sig[QM_SIG_TYPE_RSSI].threshold_list[i] = (float)config->rssi_threshold_list[i];
      }
    }

    if ( config->ecio_threshold_list_valid )
    {
      memset( clnt->config_sig[QM_SIG_TYPE_ECIO].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt->config_sig[QM_SIG_TYPE_ECIO].threshold_list_len = config->ecio_threshold_list_len;
      for ( i = 0; i < config->ecio_threshold_list_len; i++ )
      {
        clnt->config_sig[QM_SIG_TYPE_ECIO].threshold_list[i] = (float)config->ecio_threshold_list[i]*-0.5;
      }
    }

    if ( config->hdr_sinr_threshold_list_valid )
    {
      memset( clnt->config_sig[QM_SIG_TYPE_SINR].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt->config_sig[QM_SIG_TYPE_SINR].threshold_list_len = config->hdr_sinr_threshold_list_len;
      for ( i = 0; i < config->hdr_sinr_threshold_list_len; i++ )
      {
        clnt->config_sig[QM_SIG_TYPE_SINR].threshold_list[i] = (float)config->hdr_sinr_threshold_list[i];
      }
    }

    if ( config->lte_snr_threshold_list_valid )
    {
      memset( clnt->config_sig[QM_SIG_TYPE_SNR].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt->config_sig[QM_SIG_TYPE_SNR].threshold_list_len = config->lte_snr_threshold_list_len;
      for ( i = 0; i < config->lte_snr_threshold_list_len; i++ )
      {
        clnt->config_sig[QM_SIG_TYPE_SNR].threshold_list[i] = (float)config->lte_snr_threshold_list[i]/10.0;
      }
    }

    if ( config->io_threshold_list_valid )
    {
      memset( clnt->config_sig[QM_SIG_TYPE_IO].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt->config_sig[QM_SIG_TYPE_IO].threshold_list_len = config->io_threshold_list_len;
      for ( i = 0; i < config->io_threshold_list_len; i++ )
      {
        clnt->config_sig[QM_SIG_TYPE_IO].threshold_list[i] = (float)config->io_threshold_list[i];
      }
    }

    if ( config->lte_rsrq_threshold_list_valid )
    {
      memset( clnt->config_sig[QM_SIG_TYPE_RSRQ].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt->config_sig[QM_SIG_TYPE_RSRQ].threshold_list_len = config->lte_rsrq_threshold_list_len;
      for ( i = 0; i < config->lte_rsrq_threshold_list_len; i++ )
      {
        clnt->config_sig[QM_SIG_TYPE_RSRQ].threshold_list[i] = (float)config->lte_rsrq_threshold_list[i];
      }
    }

    if ( config->lte_rsrp_threshold_list_valid )
    {
      memset( clnt->config_sig[QM_SIG_TYPE_RSRP].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt->config_sig[QM_SIG_TYPE_RSRP].threshold_list_len = config->lte_rsrp_threshold_list_len;
      for ( i = 0; i < config->lte_rsrp_threshold_list_len; i++ )
      {
        clnt->config_sig[QM_SIG_TYPE_RSRP].threshold_list[i] = (float)config->lte_rsrp_threshold_list[i];
      }
    }

    if ( config->rscp_threshold_list_valid )
    {
      memset( clnt->config_sig[QM_SIG_TYPE_RSCP].threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt->config_sig[QM_SIG_TYPE_RSCP].threshold_list_len = config->rscp_threshold_list_len;
      for ( i = 0; i < config->rscp_threshold_list_len; i++ )
      {
        clnt->config_sig[QM_SIG_TYPE_RSCP].threshold_list[i] = (float)config->rscp_threshold_list[i];
      }
    }

    if ( config->tds_sinr_threshold_list_valid )
    {
      memset( clnt->tds_sinr_config.threshold_list, 0x00, sizeof(float)*MAX_NUM_SIG_INFO_THRESHOLDS );
      clnt->tds_sinr_config.threshold_list_len = config->tds_sinr_threshold_list_len;
      for ( i = 0; i < config->tds_sinr_threshold_list_len; i++ )
      {
        clnt->tds_sinr_config.threshold_list[i] = (float)config->tds_sinr_threshold_list[i];
      }
    }
  }
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_PROCESS_CONFIG_SIG_INFO2()

  DESCRIPTION
    This function sets the config_sig_info information for the specific client

  PARAMETERS
    clid: client to store config_sig_info information for
    config: information to store for client 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_process_config_sig_info2( int32 clid, nas_config_sig_info2_req_msg_v01 *config )
{
  struct qm_nas_clnt_global_s *clnt;
  int i = 0;

  QM_MSG_HIGH_1("Storing config sig info 2 for clid %d", clid);

  if ( config && QM_NAS_CLNT_ID_VALID(clid) )
  {
    clnt = clnt_config_sig_info[clid-1];
    
    if(!clnt)
    {
      QM_MSG_ERROR_1("qm_nas_clnt_process_config_sig_info2(): Invalid clnt received for clid %d", clid);
      return;
    }
    
    clnt->use_config_sig2 = TRUE;
    memset( clnt->config_sig, 0, sizeof(struct config_sig_s)*QM_SIG_TYPE_MAX );
    memset( &clnt->tds_sinr_config, 0, sizeof(struct config_sig_s) );

    // CDMA RSSI
    if ( config->cdma_rssi_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list_len = config->cdma_rssi_threshold_list_len;
      for ( i = 0; i < config->cdma_rssi_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list[i] = (float)config->cdma_rssi_threshold_list[i]/10;
      }
    }

    if ( config->cdma_rssi_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_1X].delta = (float)config->cdma_rssi_delta/10;
    }

    // CDMA ECIO
    if ( config->cdma_ecio_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list_len = config->cdma_ecio_threshold_list_len;
      for ( i = 0; i < config->cdma_ecio_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list[i] = (float)config->cdma_ecio_threshold_list[i]/10;
      }
    }

    if ( config->cdma_ecio_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_1X].delta = (float)config->cdma_ecio_delta/10;
    }

    // HDR RSSI
    if ( config->hdr_rssi_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list,0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list_len = config->hdr_rssi_threshold_list_len;
      for ( i = 0; i < config->hdr_rssi_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list[i] = (float)config->hdr_rssi_threshold_list[i]/10;
      }
    }

    if ( config->hdr_rssi_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_HDR].delta = (float)config->hdr_rssi_delta/10;
    }

    // HDR ECIO
    if ( config->hdr_ecio_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list_len = config->hdr_ecio_threshold_list_len;
      for ( i = 0; i < config->hdr_ecio_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list[i] = (float)config->hdr_ecio_threshold_list[i]/10;
      }
    }

    if ( config->hdr_ecio_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_HDR].delta = (float)config->hdr_ecio_delta/10;
    }

    // HDR SINR
    if ( config->hdr_sinr_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list_len = config->hdr_sinr_threshold_list_len;
      for ( i = 0; i < config->hdr_sinr_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list[i] = (float)config->hdr_sinr_threshold_list[i];
      }
    }

    if ( config->hdr_sinr_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_HDR].delta = (float)config->hdr_sinr_delta;
    }

    // HDR IO
    if ( config->hdr_io_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list_len = config->hdr_io_threshold_list_len;
      for ( i = 0; i < config->hdr_io_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list[i] = (float)config->hdr_io_threshold_list[i]/10;
      }
    }

    if ( config->hdr_io_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_IO][QM_RAT_HDR].delta = (float)config->hdr_io_delta/10;
    }

    // GSM RSSI
    if ( config->gsm_rssi_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list,0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list_len = config->gsm_rssi_threshold_list_len;
      for ( i = 0; i < config->gsm_rssi_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list[i] = (float)config->gsm_rssi_threshold_list[i]/10;
      }
    }

    if ( config->gsm_rssi_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_GERAN].delta = (float)config->gsm_rssi_delta/10;
    }

    // WCDMA RSSI
    if ( config->wcdma_rssi_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list,0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list_len = config->wcdma_rssi_threshold_list_len;
      for ( i = 0; i < config->wcdma_rssi_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list[i] = (float)config->wcdma_rssi_threshold_list[i]/10;
      }
    }

    if ( config->wcdma_rssi_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_UMTS].delta = (float)config->wcdma_rssi_delta/10;
    }

    // WCDMA ECIO
    if ( config->wcdma_ecio_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list_len = config->wcdma_ecio_threshold_list_len;
      for ( i = 0; i < config->wcdma_ecio_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list[i] = (float)config->wcdma_ecio_threshold_list[i]/10;
      }
    }

    if ( config->wcdma_ecio_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_UMTS].delta = (float)config->wcdma_ecio_delta/10;
    }

    // LTE RSSI
    if ( config->lte_rssi_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list,0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list_len = config->lte_rssi_threshold_list_len;
      for ( i = 0; i < config->lte_rssi_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list[i] = (float)config->lte_rssi_threshold_list[i]/10;
      }
    }

    if ( config->lte_rssi_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_LTE].delta = (float)config->lte_rssi_delta/10;
    }

    // LTE SNR
    if ( config->lte_snr_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list_len = config->lte_snr_threshold_list_len;
      for ( i = 0; i < config->lte_snr_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list[i] = (float)config->lte_snr_threshold_list[i]/10;
      }
    }

    if ( config->lte_snr_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_SNR][QM_RAT_LTE].delta = (float)config->lte_snr_delta/10;
    }

    // LTE RSRQ
    if ( config->lte_rsrq_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list_len = config->lte_rsrq_threshold_list_len;
      for ( i = 0; i < config->lte_rsrq_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list[i] = (float)config->lte_rsrq_threshold_list[i]/10;
      }
    }

    if ( config->lte_rsrq_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSRQ][QM_RAT_LTE].delta = (float)config->lte_rsrq_delta/10;
    }

    // LTE RSRP
    if ( config->lte_rsrp_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list_len = config->lte_rsrp_threshold_list_len;
      for ( i = 0; i < config->lte_rsrp_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list[i] = (float)config->lte_rsrp_threshold_list[i]/10;
      }
    }

    if ( config->lte_rsrp_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSRP][QM_RAT_LTE].delta = (float)config->lte_rsrp_delta/10;
    }

    // TDS RSCP
    if ( config->tdscdma_rscp_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].delta = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list_len = config->tdscdma_rscp_threshold_list_len;
      for ( i = 0; i < config->tdscdma_rscp_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list[i] = (float)config->tdscdma_rscp_threshold_list[i]/10;
      }
    }

    if ( config->tdscdma_rscp_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].threshold_list_len = 0;

      clnt->config_sig2[QM_SIG_TYPE_RSCP][QM_RAT_TDSCDMA].delta = (float)config->tdscdma_rscp_delta/10;
    }

    // TDS RSSI
    if ( config->tds_rssi_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].delta = 0;
  
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list_len = config->tds_rssi_threshold_list_len;
      for ( i = 0; i < config->tds_rssi_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list[i] = (float)config->tds_rssi_threshold_list[i];
      }
    }

    if ( config->tdscdma_rssi_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].threshold_list_len = 0;
  
      clnt->config_sig2[QM_SIG_TYPE_RSSI][QM_RAT_TDSCDMA].delta = (float)config->tdscdma_rssi_delta;
    }

    // TDS ECIO
    if ( config->tds_ecio_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].delta = 0;
  
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list_len = config->tds_ecio_threshold_list_len;
      for ( i = 0; i < config->tds_ecio_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list[i] = (float)config->tds_ecio_threshold_list[i];
      }
    }

    if ( config->tdscdma_ecio_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].threshold_list_len = 0;
  
      clnt->config_sig2[QM_SIG_TYPE_ECIO][QM_RAT_TDSCDMA].delta = (float)config->tdscdma_ecio_delta;
    }

    // TDS SINR
    if ( config->tds_sinr_threshold_list_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].delta = 0;
  
      clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list_len = config->tds_sinr_threshold_list_len;
      for ( i = 0; i < config->tds_sinr_threshold_list_len; i++ )
      {
        clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list[i] = (float)config->tds_sinr_threshold_list[i];
      }
    }

    if ( config->tdscdma_sinr_delta_valid )
    {
      memset(clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list, 0x00, sizeof(float) * MAX_NUM_SIG_INFO2_THRESHOLDS);
      clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].threshold_list_len = 0;
  
      clnt->config_sig2[QM_SIG_TYPE_SINR][QM_RAT_TDSCDMA].delta = (float)config->tdscdma_sinr_delta;
    }
  }
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_GET_NEXT_CLIENT_FOR_SIG_INFO_IND()

  DESCRIPTION
    This function retrieves the next client registered for sig_info_ind, based
    on provided previous.

  PARAMETERS
    cl_id: previous client id that was returned. QM_NAS_INVALID_CLIENT if first call.

  RETURN VALUE
    uint32: next client, QM_NAS_INVALID_CLIENT if no more.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 qm_nas_clnt_get_next_client_for_sig_info_ind( int32 clid, enum qm_subs_e sub_id )
{
  int32 next_clnt = QM_NAS_INVALID_CLIENT;
  int i = ( clid == QM_NAS_INVALID_CLIENT ? 0 : clid );

  for ( ; i < QM_NAS_MAX_NUM_CLIDS; i++ )
  {
    if ( qmi_nas_state.client[i] != NULL && 
         qmi_nas_state.client[i]->common.clid != QMI_SVC_CLID_UNUSED &&
         qmi_nas_state.client[i]->report_status.report_sig_info &&
         qmi_nas_state.client[i]->report_status.bound_subs == (enum qmi_nas_subs_e) sub_id)
    {
      next_clnt = qmi_nas_state.client[i]->common.clid;
      break;
    }
  }

  return next_clnt;
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_GET_NEXT_CLIENT_FOR_ERR_RATE_IND()

  DESCRIPTION
    This function retrieves the next client registered for err_rate_ind, based
    on provided previous.

  PARAMETERS
    cl_id: previous client id that was returned. QM_NAS_INVALID_CLIENT if first call.

  RETURN VALUE
    int32: next client, QM_NAS_INVALID_CLIENT if no more.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 qm_nas_clnt_get_next_client_for_err_rate_ind( int32 clid, enum qm_subs_e sub_id )
{
  int32 next_clnt = QM_NAS_INVALID_CLIENT;
  int i = ( clid == QM_NAS_INVALID_CLIENT ? 0 : clid );

  for ( ; i < QM_NAS_MAX_NUM_CLIDS; i++ )
  {
    if ( qmi_nas_state.client[i] != NULL && 
         qmi_nas_state.client[i]->common.clid != QMI_SVC_CLID_UNUSED &&
         qmi_nas_state.client[i]->report_status.report_err_rate &&
         qmi_nas_state.client[i]->report_status.bound_subs == (enum qmi_nas_subs_e) sub_id)
    {
      next_clnt = qmi_nas_state.client[i]->common.clid;
      break;
    }
  }

  return next_clnt;
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_GET_BINDING()

  DESCRIPTION
    This function retrieves the subscription for the given client.

  PARAMETERS
    cl_id: client to query bound subscription for.

  RETURN VALUE
    enum qm_subs_e: bound subscription for a client, QM_SUBS_MIN if error.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qm_subs_e qm_nas_clnt_get_binding( int32 clid )
{
  enum qm_subs_e subs_id = QM_SUBS_MIN;

  if ( QM_NAS_CLNT_ID_VALID(clid) )
  {
    if ( qmi_nas_state.client[clid-1] != NULL &&
         qmi_nas_state.client[clid-1]->common.clid != QMI_SVC_CLID_UNUSED )
    {
      subs_id = qm_util_map_nas_subs_to_qm_subs(qmi_nas_state.client[clid-1]->report_status.bound_subs);
    }
  }

  return subs_id;
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_SEND_IND()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the NAS service
    indication to client.

  PARAMETERS
    clid: client ID to send ind to
    cmd: Which indication is being sent
    ind: pointer to dsm item to send

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_send_ind( int32 clid, int32 cmd, dsm_item_type* ind )
{
  qmi_framework_common_msg_hdr_type hdr;
  qmi_nasi_client_state_type *      cl_sp = NULL;
  boolean                           status = FALSE;

  if ( QM_NAS_CLNT_ID_VALID( clid ) )
  {
    cl_sp = qmi_nas_state.client[clid-1];
  }

  if ( cl_sp )
  {
    hdr.client_id    = cl_sp->common.clid;
    hdr.qmi_instance = cl_sp->instance;
    hdr.service      = (qmux_service_e_type)cl_sp->service_id;

    if ( hdr.service < QMUX_SERVICE_MAX )
    {
      status = qmi_mmode_svc_send_indication( &hdr, (uint16)cmd, ind );
      QM_MSG_HIGH_3("qmi_nas_clnt_send_ind() sending ind: cmd %d clnt %d status %d", cmd, hdr.client_id, status);
    }
    else
    {
      QM_MSG_ERROR_1("qm_nas_clnt_send_ind(): Invalid service(%d)", hdr.service);
      dsm_free_packet( &ind );
    }
  }
  else
  {
    QM_MSG_ERROR("qmi_nas_clnt_send_indication invalid clid");
    dsm_free_packet( &ind );
  }
}

/*===========================================================================
  FUNCTION CROSSED_THRESHOLD_OR_DELTA()

  DESCRIPTION
    Static function to check if rssi threshold has been crossed.

  PARAMETERS
    clid: client ID to check
    rat: RAT to check thresholds for
    new_value: the new value
    old_value: the previous value

  RETURN VALUE
    boolean: whether new value crossed a threshold from old value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean crossed_threshold_or_delta( int32 clid, enum qm_rat_e rat, enum qm_sig_type_e type, 
                                           float new_value, float old_value )
{
  int i;
  boolean ret_val = FALSE;
  struct qm_nas_clnt_global_s *clnt;

  if ( QM_NAS_CLNT_ID_VALID( clid ) )
  {
    clnt = clnt_config_sig_info[clid-1];
    
    if(!clnt)
    {
      QM_MSG_ERROR_1("crossed_threshold_or_delta(): Invalid clnt received for clid %d", clid);
      return FALSE;
    }
    else
    {
      QM_MSG_LOW_1("clnt->use_config_sig2 = %d", clnt->use_config_sig2);
    }

    if( !clnt->use_config_sig2 )
    {
      struct config_sig_s config_info;

      if ( rat == QM_RAT_TDSCDMA && type == QM_SIG_TYPE_SINR )
      {
        config_info = clnt->tds_sinr_config;
      }
      else
      {
        config_info = clnt->config_sig[type];
      }

      // compare thresholds only if the list exists
      if( config_info.threshold_list_len )
      {
        for(i=0; i<config_info.threshold_list_len; i++)
        {
          // If new_value < threshold <= old_value
          if( ( new_value < config_info.threshold_list[i] ) &&
              ( config_info.threshold_list[i] <= old_value ) )
          {
            ret_val = TRUE;
            break;
          }

          // If old_value < threshold <= new_value
          if( (old_value < config_info.threshold_list[i] ) &&
              (config_info.threshold_list[i]  <= new_value ) )
          {
            ret_val = TRUE;
            break;
          }
        }
      }
    }
    else
    {
      struct config_sig2_s config2_info = clnt->config_sig2[type][rat];

      if( config2_info.threshold_list_len )
      {
        for(i=0; i<config2_info.threshold_list_len; i++)
        {
          // If new_value < threshold <= old_value
          if( ( new_value < (config2_info.threshold_list[i]) ) &&
              ( (config2_info.threshold_list[i]) <= old_value ) )
          {
            ret_val = TRUE;
            break;
          }

          // If old_value < threshold <= new_value
          if( (old_value < (config2_info.threshold_list[i]) ) &&
              ((config2_info.threshold_list[i])  <= new_value ) )
          {
            ret_val = TRUE;
            break;
          }
        }
      }
      else if( config2_info.delta > 0 )
      {
        int16 inner_bound=0, outer_bound=0;
        float delta = config2_info.delta; //shorthand

        inner_bound = ((int16)(old_value/delta) * delta);
        outer_bound = inner_bound + (int16)( (old_value < 0) ? (-1*delta) : delta );

        if( abs(new_value) >= abs(outer_bound) || abs(new_value) < abs(inner_bound) )
        {
          ret_val = TRUE;
        }
      }
    }
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_CROSSED_THRESHOLD()

  DESCRIPTION
    This function determines if provided new value has crossed threshold or delta
    for specified client.

  PARAMETERS
    clid: client ID to check
    subscription: subscription this request is for
    rat: RAT to check thresholds for
    type: the measurement type to check threshold for
    new_value: the new value
    old_value: the previous value

  RETURN VALUE
    boolean: whether new value crossed a threshold from old value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_nas_clnt_crossed_threshold( int32 clid, enum qm_subs_e subscription,
                                       enum qm_rat_e rat, enum qm_sig_type_e type,
                                       float new_value, float old_value )
{
  boolean ret_val = FALSE;
  qmi_nasi_client_state_type *cl_sp;

  if ( QM_NAS_CLNT_ID_VALID(clid) )
  {
    cl_sp = qmi_nas_state.client[clid-1];

    if( cl_sp != NULL && 
        cl_sp->common.clid != QMI_SVC_CLID_UNUSED &&
        qm_util_map_nas_subs_to_qm_subs(cl_sp->report_status.bound_subs) == subscription &&
        cl_sp->report_status.report_sig_info )
    {
      if ( qm_si_is_rat_active( subscription, rat ) )
      {
        switch ( type )
        {
          case QM_SIG_TYPE_RSSI:
            if ( rat > QM_RAT_MIN && rat < QM_RAT_MAX )
            {
              ret_val = crossed_threshold_or_delta( clid, rat, type, new_value, old_value );
            }
            break;

          case QM_SIG_TYPE_ECIO:
            if ( rat == QM_RAT_1X || rat == QM_RAT_HDR || rat == QM_RAT_UMTS || rat == QM_RAT_TDSCDMA )
            {
              ret_val = crossed_threshold_or_delta( clid, rat, type, new_value, old_value );
            }
            break;

          case QM_SIG_TYPE_SINR:
            if ( rat == QM_RAT_HDR || rat == QM_RAT_TDSCDMA )
            {
              ret_val = crossed_threshold_or_delta( clid, rat, type, new_value, old_value );
            }
            break;

          case QM_SIG_TYPE_IO:
            if ( rat == QM_RAT_HDR )
            {
              ret_val = crossed_threshold_or_delta( clid, rat, type, new_value, old_value );
            }
            break;

          case QM_SIG_TYPE_RSRQ:
          case QM_SIG_TYPE_RSRP:
          case QM_SIG_TYPE_SNR:
            if ( rat == QM_RAT_LTE )
            {
              ret_val = crossed_threshold_or_delta( clid, rat, type, new_value, old_value );
            }
            break;

          case QM_SIG_TYPE_RSCP:
            if ( rat == QM_RAT_TDSCDMA )
            {
              ret_val = crossed_threshold_or_delta( clid, rat, type, new_value, old_value );
            }
            break;

          default:
            QM_MSG_HIGH_1("Unsupported type %d", type);
            break;
        }
      }
    }
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION GET_CL_SP_BY_CMD_BUF()

  DESCRIPTION
    Retrieve client sp from the command buffer

  PARAMETERS
    cmd_buf_p_in  :  command buffer

  RETURN VALUE
    cl_sp   :  client sp

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_nasi_client_state_type * get_cl_sp_by_cmd_buf( qmi_cmd_buf_type* cmd_buf_p ) 
{
  qmi_common_client_state_type *cl_sp;
  void                         *retval = NULL;
  int                           i=0;

  if ( is_cmd_buf_p_valid(cmd_buf_p) )
  {
    cl_sp = cmd_buf_p->x_p->cl_sp;

    for ( i=0; i<QM_NAS_MAX_NUM_CLIDS; i++ )
    {
      if ( qmi_nas_state.client[i] )
      {
        if ( qmi_nas_state.client[i]->common.clid == cl_sp->clid )
        {
          retval = qmi_nas_state.client[i]; 
          break;
        }
      }
    }
  }

  return retval;
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_SEND_RESPONSE()

  DESCRIPTION
    Send the response to framework. Validates cmd_buf_p before sending response. 
    Upon failure to send, msg_ptr DSM is freed.

  PARAMETERS
    cmd_buf_p: pointer to command buffer for client
    msg_ptr: pointer to dsm item to send

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_clnt_send_response
(
  qmi_cmd_buf_type *      cmd_buf_p,
  dsm_item_type *         msg_ptr
)
{
  qmi_framework_common_msg_hdr_type hdr;
  qmi_nasi_client_state_type *      cl_sp;
  boolean                           status = FALSE;

  cl_sp = get_cl_sp_by_cmd_buf( cmd_buf_p ); 

  if ( cl_sp )
  {
    memset( &hdr, 0x00, sizeof(hdr) );

    hdr.service      = QMUX_SERVICE_NAS;
    hdr.client_id    = cl_sp->common.clid;
    hdr.qmi_instance = cl_sp->instance;
    /* Transaction id will be filled by svc layer */

    status = qmi_mmode_svc_send_response( &hdr, cmd_buf_p, msg_ptr );

    QM_MSG_HIGH_3("qmi_nas_clnt_send_response() sending resp: clnt %d status %d cmd %d", hdr.client_id, status, cmd_buf_p->cmd_type); 
  }
  else
  {
    dsm_free_packet( &msg_ptr );
    QM_MSG_ERROR("qmi_nas_clnt_send_response() can't find cl_sp");
  }

  return status;
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_VALIDATE_CMD_BUF_P()

  DESCRIPTION
    Validates cmd_buf_p pointer

  PARAMETERS
    cmd_buf_p: pointer to command buffer for client

  RETURN VALUE
    TRUE if valid, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_nas_clnt_validate_cmd_buf_p( qmi_cmd_buf_type *cmd_buf_p )
{
  boolean retval = TRUE;

  if ( !qmi_nas_validate_cmd_buf_p( cmd_buf_p, &qmi_nas_state ) || cmd_buf_p == NULL || cmd_buf_p->x_p == NULL )
  {
    QM_MSG_ERROR_1("qm_nas_clnt_validate_cmd_buf_p: cmd_buf_p is invalid %d", cmd_buf_p);
    retval = FALSE;
  }  

  return retval;
}

/*===========================================================================
  FUNCTION QM_NAS_CLNT_SEND_IND_ALL()

  DESCRIPTION
    This function calls the QMI Framework API to send out the NAS service
    indication to all client that are registered for the IND.

  PARAMETERS
    clid: client ID to send ind to
    cmd: Which indication is being sent
    ind: pointer to dsm item to send

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_send_ind_all( uint16 cmd, void *cstruct, enum qm_subs_e subs )
{
  qmi_nasi_client_state_type *      cl_sp = NULL;
  dsm_item_type                    *ind = NULL;
  int32                             client;

  for ( client=0; client<QM_NAS_MAX_NUM_CLIDS; client++ )
  {
    cl_sp = qmi_nas_state.client[client];

    if ( cl_sp && cl_sp->common.clid != QMI_SVC_CLID_UNUSED )
    {
      if ( cl_sp->report_status.bound_subs == (enum qmi_nas_subs_e)subs ) // TODO: move bound_subs into qm_nas_clnt
      {
        switch ( cmd )
        {
          case NASI_CMD_VAL_E911_STATE_READY_IND:
            if ( cl_sp->report_status.report_e911_state_ready )
            {
              ind = NULL;
              // no encoding procedure needed as payload is empty
              qm_nas_clnt_send_ind( cl_sp->common.clid, NASI_CMD_VAL_E911_STATE_READY_IND, ind );
            }
            break;

          default:
            QM_MSG_ERROR_1("Not handled cmd %d to qm_nas_clnt_send_ind_all", cmd);
            break;
        }
      }
    }
  }
}
