/*===========================================================================

                                  R R C L O G

GENERAL DESCRIPTION
 The RRCLOG module consists of functions logging tyhe OTA messages in binary 
 format. These binary files can be used for post analysis.

EXTERNALIZED FUNCTIONS

  rrclog_create_log_file()
    This function creates a new data file for write only.

  rrclog_close_log_file()
    This function closes the file.

  rrclog_log_sig_msg()
    This function writes the SDU header first into binary file and then writes
    the SDU data into binary file.

  
Copyright (c) 2000-2001, 2003-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrclog.c_v   1.7   07 Aug 2001 10:36:36   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrclog.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/16   nr      Adding log packet for standalone HRB db
05/12/16   vs      Adding log packet for measurement related logging
05/09/16   nr      Adding log packet for hanging RB logging
05/25/15   sg      Made changes to add F3 to know if log packet is not enabled
11/25/14   aa      Made changes to print an invalid HFN value if there is no RLC reestablishment
11/19/14   aa      Made Changes to print HFN value only in the case of an RLC reestablisment
09/05/14   as      Made changes to log events for inter freq HHO status
09/02/14   sp      Made changes to use dynamic size for CMAC DL log packet
08/04/14   sp      Made changes to fix compilation errors
07/17/14   sp      Made changes to check for null pointer before accessing hs_rach_config_ptr pointer.
06/09/14   db      RRC Logging Changes for CMAC and CRLC
11/03/10   rl      Fixed compiler warnings
12/21/09   gkg     Corrected misleading F3s.
05/08/09   ss      Updated Copyright Information
04/11/06   vm      RVDS2.1 compilation related changes.
03/31/06   vm      Fixed lint error.
12/18/05   da      Added new function rrc_send_rb_rate_info_packet to log 
                   RB rate information.
08/30/04   sgk     Type cast fclose to void in rrclog_close_log_file to fix 
                   lint error Ignoring return value of function.
01/13/02   ram     Linted file and cleared all existing errors.
08/07/01   rj      Fixed a compilation warning for ARM.
03/03/01   js      Initialization of sdu_hdr in function rrclog_log_sig_msg
                   is modified to fix ARM Compiler warnings.
12/05/00   rj      The formal parameter for function rrclog_log_sig_msg is
                   changed from uint16 to rrclog_log_ch_e_type
11/30/00   rj      Added $Header to the EDIT HISTORY 
11/30/00   rj      Added more comments and editorial corrections
11/17/00   rj      deleted rrclog_read_from_file. 
                   Changed rrclog_write_to_file function name to 
                   rrclog_log_sig_msg.
                   Changed rrclog_close_file to rrclog_close_log_file.
                   Changed rrclog_create_file to rrclog_create_log_file.
                   File Pointer is removed as formal parameter for the above 
                   functions.
11/14/00   rj      Created file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rrclog.h"
#include "stdio.h"
#include "err.h"
#include "rrcllci.h"
#include "rrcscmgr.h"
#include "rrcllc.h"
#include "rrclogging.h"
#include "rrclcm.h"
#include "rrcllcoc.h"
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
boolean rrc_log_inter_freq_hho_started = FALSE;

extern rrc_established_rabs_type rrc_est_rabs;

static FILE *fp;                            /* File pointer for log file */
rrclog_file_status_e_type file_status = RRCLOG_FILE_CLOSED;
                                            /* The file status is 
                                               initialized */


extern uint32 rrcllc_get_dl_ded_rate(rrc_state_e_type  rrc_state, rrc_RB_Identity  rb_id);
extern uint32 rrcllc_get_ul_ded_rate(rrc_state_e_type  rrc_state, rrc_RB_Identity  rb_id);

/*===========================================================================

FUNCTION rrclog_create_log_file

DESCRIPTION
  This function creates and opens a new data file for write only.

DEPENDENCIES
  None

RETURN VALUE
  rrclog_status_e_type

SIDE EFFECTS
  None
===========================================================================*/

rrclog_status_e_type rrclog_create_log_file
(    
  const char *filename  
)
{
  uint32 count;
  rrclog_file_hdr_type file_hdr = 
                   {{'R','R','C',' '},
                    FILE_FORMAT_VER,       /* File Format Version */
                    MAJOR_SPEC_RELEASE,    /* Major Specification release */
                    MINOR_SPEC_RELEASE,    /* Minor Specification release */
                    MINOR_MINOR_SPEC_RELEASE,
                                             /* Minor minor Spec release */
                    ' ',                   /* A byte reserved for future
                                               expansion */
                    {' ',' ',' ',' ',' ',' '}
                   };                        /* Reserved for future expansion */
  
  rrclog_status_e_type status = RRCLOG_FAILURE;
   /* Checks whether already log file is created or not.*/
  if(file_status != RRCLOG_FILE_CLOSED)
  {
    WRRC_MSG0_ERROR("Log File is already opened");
    return (status);                        /* If log file already created
                                               it returns the failure status*/
  }
  /* Creates a log file if not already created */
  if((fp = fopen(filename, "wb")) == NULL)
  {
     WRRC_MSG0_ERROR("Cannot open designated file");
    return (status);                        /* Returns failure status if it 
                                               is unable open a file */
  }
  count = fwrite(&file_hdr, 1, sizeof(rrclog_file_hdr_type), fp);   
                                            /* Writes Header for the file  
                                               into the log file */
  if (count != sizeof(rrclog_file_hdr_type))
  {
    WRRC_MSG0_ERROR("Failed to write into the File");
    return (status);  
  }

  file_status = RRCLOG_FILE_OPENED;         /* File status is updated */
  status = RRCLOG_SUCCESS;
  return (status);
} /* rrclog_create_log_file */

/*===========================================================================
FUNCTION rrclog_close_log_file

DESCRIPTION
  This function closes the file.

DEPENDENCIES
  None

RETURN VALUE
  rrclog_status_e_type

SIDE EFFECTS
  None
===========================================================================*/

rrclog_status_e_type rrclog_close_log_file
(    
  void
)
{ 
  rrclog_status_e_type status = RRCLOG_SUCCESS;
  if(file_status == RRCLOG_FILE_CLOSED)
  {
    WRRC_MSG0_ERROR("The log file is not created. Hence can not be closed");
    status = RRCLOG_FAILURE; 
    return (status);
  }
  if(fp != NULL)
  {
    (void)fclose (fp);
    file_status = RRCLOG_FILE_CLOSED;
  }
  else
  {
    WRRC_MSG0_ERROR("Invalid File Pointer. Can not close");
    status = RRCLOG_FAILURE;
    file_status = RRCLOG_FILE_CLOSED;
  }
  return (status);
} /* rrclog_close_log_file */

/*===========================================================================
FUNCTION rrclog_log_sig_msg

DESCRIPTION
  This function writes the SDU header first into binary file and then writes
  the SDU data into binary file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

rrclog_status_e_type rrclog_log_sig_msg
(    
  void  *sdu,                                /* SDU data */
  uint32 sdu_size,                           /* SDU size in bytes */
  uint16 rb_id,                              /* Radio Bearer Id to which SDU
                                                belongs */
  rrclog_log_ch_e_type log_ch                /* Logical Channel type to which
                                                SDU belongs */           
)
{
  int i;                                     /* Loop counter */
  rrclog_status_e_type status;
  uint32 count;
  rrclog_sdu_hdr_type sdu_hdr;
   
  /* Initialize the SDU header */
  sdu_hdr.log_ch = (uint16)log_ch;         /* Logical Channel type - typecast
                                           to the right size since enum value is
                                           assumed to be int size. Keeps lint happy.
                                           Logical channel is between 0 and 4. */
  sdu_hdr.rb_id = rb_id;                   /* Radio Bearer Id */
  sdu_hdr.num_bytes = sdu_size;            /* Size of SDU in bytes */

  for (i = 0; i < TIME_STAMP_LENGTH; i++)
  {
    sdu_hdr.time_stamp[i] = 0;             /* Time stamp for future expansion */
  }

  for (i = 0; i < SDU_HDR_RES_LENGTH; i++)
  {
    sdu_hdr.reserved_bytes[i] = 0;         /* Reserved for future expansion */
  }

  status = RRCLOG_FAILURE;

  if(file_status == RRCLOG_FILE_CLOSED)     /* Checks whether log is opened */
  {
    WRRC_MSG0_ERROR("Log File is not opened. Hence Sig message can not be logged");
    return(status);
  }  

  count = fwrite(&sdu_hdr, 1, sizeof(rrclog_sdu_hdr_type), fp);   
                                            /* Writes Header for the SDU 
                                               into the file */
  if (count != sizeof(rrclog_sdu_hdr_type))
  {
    WRRC_MSG0_ERROR("Failed to write into the File");
    return (status);
  }
  
  count = fwrite(sdu, 1, sdu_hdr.num_bytes,fp);   
                                           /* Writes the SDU Data into
                                              the file */
  if (count != sdu_hdr.num_bytes)
  {
    WRRC_MSG0_ERROR("Failed to write into the File");
    return (status);
  }
  status = RRCLOG_SUCCESS;
  return (status);
} /* rrclog_log_sig_msg */


/*===========================================================================

FUNCTION RRC_SEND_RB_RATE_INFO_PACKET

DESCRIPTION

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.

===========================================================================*/
void rrc_send_rb_rate_info_packet
(
void
)
{
    WCDMA_RB_RATE_INFO_LOG_PACKET_type   *log_rb_rate_ptr;
    uint8 num_rbs = 0;
    uint32 count, count1, count2;
    rrc_state_e_type rrc_state;
    rrc_proc_e_type                 proc_id;
    rrcllc_oc_process_state_e_type  process_state;  
    rrcllc_oc_set_status_e_type     oc_status;

    oc_status = rrcllc_get_ordered_config_state_and_proc_wo_f3(&proc_id, &process_state);

    switch (oc_status) 
    {
      case OC_SET_FOR_CELL_FACH:
      case OC_SET_FOR_DCH_FACH_TRANS:
        rrc_state = RRC_STATE_CELL_FACH;
        break;
      case OC_SET_FOR_CELL_DCH:
        rrc_state = RRC_STATE_CELL_DCH;
        break;
      case OC_SET_FOR_URA_PCH:
      case OC_SET_FOR_DCH_URA_PCH_TRANS:
      case OC_SET_FOR_FACH_URA_PCH_TRANS:
        rrc_state = RRC_STATE_URA_PCH;
        break;
      case OC_SET_FOR_CELL_PCH:
      case OC_SET_FOR_DCH_CELL_PCH_TRANS:
      case OC_SET_FOR_FACH_CELL_PCH_TRANS:
        rrc_state = RRC_STATE_CELL_PCH;
        break;
      default:
        rrc_state = rrc_get_state();
        break;
    }


    /* Get the total number of RBs */
    for (count = 0; count < MAX_RAB_TO_SETUP; count++) 
    {
      if( (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID) &&
          (rrc_est_rabs.rabs[count].num_rbs_for_rab > 0))
      {
        WRRC_MSG2_MED(" RAB_ID %d, num_rbs %d", rrc_est_rabs.rabs[count].rab_id, rrc_est_rabs.rabs[count].num_rbs_for_rab);
        num_rbs = num_rbs + (uint8)rrc_est_rabs.rabs[count].num_rbs_for_rab;
      }
    }
    WRRC_MSG1_MED("Total num rbs %d", num_rbs);
       /* Allocate memory here.*/
    log_rb_rate_ptr = (WCDMA_RB_RATE_INFO_LOG_PACKET_type   *)log_alloc(WCDMA_RB_RATE_INFO_LOG_PACKET,
        WCDMA_RB_RATE_INFO_PACKET_LEN(num_rbs));

    count2 = 0;
    if (log_rb_rate_ptr != NULL)
    {
      log_rb_rate_ptr->num_rbs = num_rbs;
      /* Get the total number of RBs */
      if (num_rbs > 0)
      {
        for (count = 0; count < MAX_RAB_TO_SETUP; count ++) 
        {
          if( (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID) &&
              (rrc_est_rabs.rabs[count].num_rbs_for_rab > 0))
          {
            for (count1 = 0; ((count1 < rrc_est_rabs.rabs[count].num_rbs_for_rab) &&
                              (count1 < MAX_RB_PER_RAB)) ; count1++) 
            {
              log_rb_rate_ptr->rb_rate[count2].rb_id = rrc_est_rabs.rabs[count].rb_for_rab[count1].rb_id;
              log_rb_rate_ptr->rb_rate[count2].dl_rb_rate = rrcllc_get_dl_ded_rate(
                rrc_state, rrc_est_rabs.rabs[count].rb_for_rab[count1].rb_id);
              log_rb_rate_ptr->rb_rate[count2].ul_rb_rate = rrcllc_get_ul_ded_rate(
                rrc_state, rrc_est_rabs.rabs[count].rb_for_rab[count1].rb_id); 
              count2++;
            }
          }
        }
      }
      /* Commit the buffer to log services.*/
      log_commit(log_rb_rate_ptr);
    }
    else
    {
        MSG_LOW("Dropped 0x%x, code enabled: %d\n", WCDMA_RB_RATE_INFO_LOG_PACKET,
            log_status(WCDMA_RB_RATE_INFO_LOG_PACKET), 0);
    }
}

/*===========================================================================

FUNCTION          RRCLOG_CMAC_DL_INFO

DESCRIPTION       Logs CMAC DL information passed to L2

DEPENDENCIES      NONE

RETURN VALUE      NONE
                  
SIDE EFFECTS      NONE

===========================================================================*/
void rrclog_cmac_dl_req(wcdma_l2_dl_cmd_type  *l2_dl_cmd_ptr)
{
  uint32 i,j;
  WCDMA_CMAC_DL_R99_type* log_record_ptr;
  uint8 *dl_log_ptr = NULL;
  uint16 size = 0;
  mac_dl_config_type *mac_config_info = &(l2_dl_cmd_ptr->cmd_data.mac_config_info);

  if(!log_status(WCDMA_CMAC_DL_R99))
  {	
    WRRC_MSG0_MED("WCDMA_CMAC_DL_R99 not enabled");
    return;
  }

  /*Size computation*/
  size += FPOS(WCDMA_CMAC_DL_R99_type,bch_info);
  size += (mac_config_info->num_bchs + mac_config_info->num_pchs)*sizeof(rrclog_mac_dl_bch_pch_type);

  /*FACH trch size*/
  for(i = 0; i < mac_config_info->num_fachs; i++)
  {
    size += SIZE_RRCLOG_MAC_DL_FACH_TYPE(mac_config_info->fach_info[i]->ndlchan);
  }
  /*DCH trch size*/
  for(i = 0; i < mac_config_info->num_dchs; i++)
  {
    size += SIZE_RRCLOG_MAC_DL_DCH_TYPE(mac_config_info->dch_info[i]->ndlchan);
  }
  WRRC_MSG1_MED("LOG_DBG:Size of Log packet is %d",size);

  log_record_ptr = (WCDMA_CMAC_DL_R99_type *)log_alloc( WCDMA_CMAC_DL_R99, size);

  if(log_record_ptr == NULL)
  {
    WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",size);
    return;
  }
  /*R 99 Channel Logging*/

  log_record_ptr->version = 1;
  log_record_ptr->wait_for_l1_sig = mac_config_info->wait_for_l1_sig;
  log_record_ptr->num_bchs = mac_config_info->num_bchs;
  log_record_ptr->num_pchs = mac_config_info->num_pchs;
  log_record_ptr->num_fachs = mac_config_info->num_fachs;
  log_record_ptr->num_dchs = mac_config_info->num_dchs;
  log_record_ptr->rnti_type = (uint8)mac_config_info->rnti_info.rnti_valid;
  log_record_ptr->c_rnti = mac_config_info->rnti_info.crnti;
  log_record_ptr->u_rnti = mac_config_info->rnti_info.urnti;

  dl_log_ptr = (uint8*)log_record_ptr;
  dl_log_ptr += FPOS(WCDMA_CMAC_DL_R99_type,bch_info);

  for(i = 0; i < mac_config_info->num_bchs; i++)
  {
    *(dl_log_ptr++) = mac_config_info->bch_info[i].cctrch_id;
    *(dl_log_ptr++) = mac_config_info->bch_info[i].bcch_id;
  }

  for(i = 0; i < mac_config_info->num_pchs; i++)
  {
    *(dl_log_ptr++) = mac_config_info->pch_info[i].cctrch_id;
    *(dl_log_ptr++) = mac_config_info->pch_info[i].pcch_id;
  }

  for(i = 0; i < mac_config_info->num_fachs; i++)
  {

    *(dl_log_ptr++) = (uint8)(mac_config_info->fach_info[i]->trch_id & 0x00FF);
    *(dl_log_ptr++) = mac_config_info->fach_info[i]->cctrch_id;
    *(dl_log_ptr++) = mac_config_info->fach_info[i]->bcch_enable;
    *(dl_log_ptr++) = mac_config_info->fach_info[i]->bcch_id;
    *(dl_log_ptr++) = mac_config_info->fach_info[i]->ccch_enable;
    *(dl_log_ptr++) = mac_config_info->fach_info[i]->ccch_id;
    *(dl_log_ptr++) = mac_config_info->fach_info[i]->ctch_enable;
    *(dl_log_ptr++) = mac_config_info->fach_info[i]->ctch_id;
    *(dl_log_ptr++) = mac_config_info->fach_info[i]->ndlchan;

    for(j = 0; j < mac_config_info->fach_info[i]->ndlchan; j++)
    {
      *(dl_log_ptr++) = (uint8)(mac_config_info->fach_info[i]->dlchan_info[j].rb_id & 0x00FF);
      *(dl_log_ptr++) = (uint8)((mac_config_info->fach_info[i]->dlchan_info[j].rb_id & 0xFF00) >> 8);
      *(dl_log_ptr++) = mac_config_info->fach_info[i]->dlchan_info[j].rlc_id;
      *(dl_log_ptr++) = mac_config_info->fach_info[i]->dlchan_info[j].mac_id;
      *(dl_log_ptr++) = mac_config_info->fach_info[i]->dlchan_info[j].rlc_mode;
      *(dl_log_ptr++) = mac_config_info->fach_info[i]->dlchan_info[j].chan_type;
    }
  }

  for(i = 0; i < mac_config_info->num_dchs; i++)
  {
    *(dl_log_ptr++) = (uint8)(mac_config_info->dch_info[i]->trch_id & 0x00FF);
    *(dl_log_ptr++) = mac_config_info->dch_info[i]->cctrch_id;
    *(dl_log_ptr++) = mac_config_info->dch_info[i]->ndlchan;

    for(j = 0; j < mac_config_info->dch_info[i]->ndlchan; j++)
    {
      *(dl_log_ptr++) = (uint8)(mac_config_info->dch_info[i]->dlchan_info[j].rb_id & 0x00FF);
      *(dl_log_ptr++) = (uint8)((mac_config_info->dch_info[i]->dlchan_info[j].rb_id & 0xFF00) >> 8);
      *(dl_log_ptr++) = mac_config_info->dch_info[i]->dlchan_info[j].rlc_id;
      *(dl_log_ptr++) = mac_config_info->dch_info[i]->dlchan_info[j].mac_id;
      *(dl_log_ptr++) = mac_config_info->dch_info[i]->dlchan_info[j].rlc_mode;
      *(dl_log_ptr++) = mac_config_info->dch_info[i]->dlchan_info[j].chan_type;
    }
  }
  log_commit(log_record_ptr);
}
/*====================================================================
FUNCTION: rrclog_crlc_am_req

DESCRIPTION:
  This function logs the acknowledged mode configuration passed to RLC.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrclog_crlc_am_req(rlc_am_config_req_type *l2_cmd_ptr)
{

  uint8 count=0;
  WCDMA_RLC_AM_LOGGING_type *log_record_ptr;
  uint8 *log_ptr = NULL;
  uint32 hfn = 0xFFFFFFFF,rlc_size;
  uint16 cfg_mask;

  if(!log_status(WCDMA_RLC_AM_LOGGING))
  {	
    WRRC_MSG0_MED("WCDMA_RLC_AM_LOGGING not enabled");
    return;
  }
	
  log_record_ptr = (WCDMA_RLC_AM_LOGGING_type *)log_alloc( WCDMA_RLC_AM_LOGGING, sizeof(WCDMA_RLC_AM_LOGGING_type));

  if(log_record_ptr == NULL)
  {
    WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_RLC_AM_LOGGING_type));
    return;
  }

  log_record_ptr->version = 1;
  log_record_ptr->nentity = l2_cmd_ptr->nentity;

  log_ptr = (uint8*) log_record_ptr;
  log_ptr += FPOS(WCDMA_RLC_AM_LOGGING_type,rlc_am_channel);
  
  for (count=0; count<l2_cmd_ptr->nentity; count++)
  {
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->common_config.rb_id;
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->common_config.lc_type;
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->reestab_entity;
    *(log_ptr++) = l2_cmd_ptr->am_act[count].action;

    cfg_mask = l2_cmd_ptr->am_act[count].dl_cfg_mask;
    RRCLOG_UINT16(log_ptr,cfg_mask);

    cfg_mask = l2_cmd_ptr->am_act[count].ul_cfg_mask;
    RRCLOG_UINT16(log_ptr,cfg_mask);

    *(log_ptr++) = l2_cmd_ptr->am_info[count]->ul_nchan;
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->dl_nchan;


    /*Uplink Channel*/
    *(log_ptr++) = 0; /*0 - UPLINK: direction*/
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->common_config.ul_control_id;  /*control_data_id*/
    
    /* In case of a RLC re-establishment on a side (UL/DL), the correct hfn value is populated for that side.
       Else it prints an invalid value */

    if (l2_cmd_ptr->am_act[count].ul_cfg_mask & RLC_RE_ESTABLISH_CFG)
    {
      hfn = l2_cmd_ptr->am_info[count]->rlc_ul_am_config.hfn;
    }
    else
    {
      hfn = 0xFFFFFFFF;         /* HFN set to invalid value to avoid the log packet printing the stale value for other RBs in the channel config */
    }  
    RRCLOG_UINT32(log_ptr,hfn);

    rlc_size = l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_rlc_size;
    RRCLOG_UINT32(log_ptr,rlc_size);

    *(log_ptr++) = l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_flex_pdu_size;
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_li_size;
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->rlc_ul_am_config.special_value_he_config;

    RRCLOG_UINT16(log_ptr,l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_flex_min_pdu_size);
    RRCLOG_UINT16(log_ptr,l2_cmd_ptr->am_info[count]->rlc_ul_am_config.ul_flex_max_pdu_size);

    /*Downlink Channel*/
    *(log_ptr++) = 1; /*1 - DOWNLINK: direction*/
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->common_config.dl_control_id; /*control_data_id*/
    
    /* In case of a RLC re-establishment on a side (UL/DL), the correct hfn value is populated for that side.
       Else it prints an invalid value */ 
    if (l2_cmd_ptr->am_act[count].dl_cfg_mask & RLC_RE_ESTABLISH_CFG)
    {
      hfn = l2_cmd_ptr->am_info[count]->rlc_dl_am_config.hfn;
    }
    else
    {
      hfn = 0xFFFFFFFF;         /* HFN set to invalid value to avoid the log packet printing the stale value for other RBs in the channel config */
    }
    RRCLOG_UINT32(log_ptr,hfn);

    rlc_size = l2_cmd_ptr->am_info[count]->rlc_dl_am_config.dl_rlc_size;
    RRCLOG_UINT32(log_ptr,rlc_size);

    *(log_ptr++) = l2_cmd_ptr->am_info[count]->rlc_dl_am_config.am_pdu_size_flexbile;
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->rlc_dl_am_config.am_li_type;
    *(log_ptr++) = l2_cmd_ptr->am_info[count]->rlc_dl_am_config.special_value_he_config;

    RRCLOG_UINT16(log_ptr,0xFFFF);  /*UL Flex Min Size*/
    RRCLOG_UINT16(log_ptr,0xFFFF);  /*UL Flex Max Size*/

  }

  log_commit(log_record_ptr);

}


/*====================================================================
FUNCTION: rrclog_cmac_ul_req

DESCRIPTION:
  Logs MAC uplink configuratio for R99 Channels(RACH and DCH)

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrclog_cmac_ul_req(wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr)
{
  mac_ul_config_type *mac_ul_parms = &(l2_ul_cmd_ptr->cmd_data.mac_config_info) ;
  uint8 index = 0, ix = 0;
  uint8 ndlchan = 0;

  WCDMA_CMAC_UL_R99_LOGGING_type *log_record_ptr;
  uint8 *log_ptr = NULL;

  if(!log_status(WCDMA_CMAC_UL_R99_LOGGING))
  {
    WRRC_MSG0_MED("WCDMA_CMAC_UL_R99_LOGGING not enabled");
    return;
  }
  log_record_ptr = (WCDMA_CMAC_UL_R99_LOGGING_type *)log_alloc( WCDMA_CMAC_UL_R99_LOGGING, sizeof(WCDMA_CMAC_UL_R99_LOGGING_type));

  if(log_record_ptr == NULL)
  {
    WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_CMAC_UL_R99_LOGGING_type));
    return;
  }

  log_record_ptr->version =                                  1;
  log_record_ptr->mac_config_changed =                       mac_ul_parms->mac_config_changed;
  log_record_ptr->wait_for_l1_sig =                          mac_ul_parms->wait_for_l1_sig;
  log_record_ptr->new_tfcs_list_present =                    mac_ul_parms->new_tfcs_list_present;
  log_record_ptr->rnti_type =                                mac_ul_parms->rnti_info.rnti_valid;
  log_record_ptr->c_rnti =                                   mac_ul_parms->rnti_info.crnti;
  log_record_ptr->u_rnti =                                   mac_ul_parms->rnti_info.urnti;
  log_record_ptr->cctrch_type =                              mac_ul_parms->cctrch_ptr->cctrch_type;

  log_ptr = (uint8*) log_record_ptr;
  log_ptr += FPOS(WCDMA_CMAC_UL_R99_LOGGING_type,trch_info);

  switch(mac_ul_parms->cctrch_ptr->cctrch_type)
  {
    case RACH:

#ifdef FEATURE_WCDMA_HS_RACH
     if(mac_ul_parms->hs_rach_action == MAC_HS_RACH_START ||
        mac_ul_parms->hs_rach_action == MAC_HS_RACH_CONFIG)
     {
       /*Skip logging legacy parameters if HSRACH is active*/
       log_record_ptr->cctrch_type = EDCH;
       log_record_ptr->num_trch = 0;
       *(log_ptr++) = 0;
       break;
     }
#endif

     log_record_ptr->num_trch = 1;
     *(log_ptr++) = mac_ul_parms->cctrch_ptr->rach_info.trch_id;
     *(log_ptr++) = mac_ul_parms->cctrch_ptr->rach_info.ndlchan;
     *(log_ptr++) = 0xFF;  /*CCTrCH ID*/
     *(log_ptr++) = mac_ul_parms->cctrch_ptr->rach_info.ccch_enable;
     *(log_ptr++) = mac_ul_parms->cctrch_ptr->rach_info.ccch_id;

     *(log_ptr++) = mac_ul_parms->cctrch_ptr->rach_info.ndlchan;
     for(index = 0;index < mac_ul_parms->cctrch_ptr->rach_info.ndlchan;index++)
     {
       *(log_ptr++) =  mac_ul_parms->cctrch_ptr->rach_info.dlchan_info[index].rb_id;
       *(log_ptr++) =  mac_ul_parms->cctrch_ptr->rach_info.dlchan_info[index].rlc_id;
       *(log_ptr++) =  mac_ul_parms->cctrch_ptr->rach_info.dlchan_info[index].mac_id;
       *(log_ptr++) =  mac_ul_parms->cctrch_ptr->rach_info.trch_id;
       *(log_ptr++) =  mac_ul_parms->cctrch_ptr->rach_info.dlchan_info[index].rlc_mode;
       *(log_ptr++) =  mac_ul_parms->cctrch_ptr->rach_info.dlchan_info[index].chan_type;
       *(log_ptr++) =  mac_ul_parms->cctrch_ptr->rach_info.dlchan_info[index].priority;
       *(log_ptr++) =  mac_ul_parms->cctrch_ptr->rach_info.dlchan_info[index].rlc_size_info.all_tfi_valid;
       *(log_ptr++) =  mac_ul_parms->cctrch_ptr->rach_info.dlchan_info[index].rlc_size_info.ntfi_per_lc;
     }
     break;

     case DCH:
       log_record_ptr->num_trch = mac_ul_parms->cctrch_ptr->dch_info.ndchs;
       for(index = 0;index < mac_ul_parms->cctrch_ptr->dch_info.ndchs;index++)
       {
         ndlchan += mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].ndlchan;

         *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].trch_id;
         *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].ndlchan;
         *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].cctrch_id;
         *(log_ptr++) =  0;  /*CCCH Enabled*/
         *(log_ptr++) =  0xFF; /*CCCH ID*/
       }

       *(log_ptr++) = ndlchan;
       for(index = 0;index < mac_ul_parms->cctrch_ptr->dch_info.ndchs;index++)
       {
         for(ix = 0;ix < mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].ndlchan; ix++)
         {
           *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].dlchan_info[ix].rb_id;
           *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].dlchan_info[ix].rlc_id;
           *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].dlchan_info[ix].mac_id;
           *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].trch_id;
           *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].dlchan_info[ix].rlc_mode;
           *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].dlchan_info[ix].chan_type;
           *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].dlchan_info[ix].priority;
           *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].dlchan_info[ix].rlc_size_info.all_tfi_valid;
           *(log_ptr++) =  mac_ul_parms->cctrch_ptr->dch_info.dch_info[index].dlchan_info[ix].rlc_size_info.ntfi_per_lc;
         }
       }
       break;
       default:
         log_record_ptr->num_trch = 0;
         *(log_ptr++) = 0; /*Number of Logical Channels*/
         break;
  }
  log_commit(log_record_ptr);

}


/*====================================================================
FUNCTION: rrclog_cmac_dl_hs_setup

DESCRIPTION:
  This function logs the HS configuration in downlink passed to MAC.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrclog_cmac_dl_hs_setup(wcdma_l2_dl_cmd_type  *l2_dl_cmd_ptr)
{
  uint32 i,ix,iy;
  mac_dl_config_type *mac_cmd_ptr = &(l2_dl_cmd_ptr->cmd_data.mac_config_info);
  uint8 num_queue =0;
  uint8 ndlchan = 0;

  WCDMA_CMAC_DL_HS_LOGGING_type *log_record_ptr;
  uint8 *log_ptr = NULL;

  if(!log_status(WCDMA_CMAC_DL_HS_LOGGING))
  {
    WRRC_MSG0_MED("WCDMA_CMAC_DL_HS_LOGGING not enabled");
    return;
  }

  log_record_ptr = (WCDMA_CMAC_DL_HS_LOGGING_type *)log_alloc( WCDMA_CMAC_DL_HS_LOGGING, sizeof(WCDMA_CMAC_DL_HS_LOGGING_type));

  if(log_record_ptr == NULL)
  {
    WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_CMAC_DL_HS_LOGGING_type));
    return;
  }

  log_record_ptr->version = 1;
  log_record_ptr->mac_dl_action =                       mac_cmd_ptr->mac_dl_action;
  log_record_ptr->wait_for_l1_sig =                     mac_cmd_ptr->wait_for_l1_sig;
  log_record_ptr->hs_status_in_e_fach =                 mac_cmd_ptr->hs_status_in_e_fach;
  log_record_ptr->mac_hsdpa_action =                    mac_cmd_ptr->mac_hsdpa_action;
  log_record_ptr->mac_hs_reset_indicator =              mac_cmd_ptr->mac_hs_reset_indicator;
  log_record_ptr->mac_hs_restore_tsns =                 mac_cmd_ptr->mac_hs_restore_tsns;
  log_record_ptr->mac_hs_or_ehs =                       mac_cmd_ptr->mac_hs_or_ehs;
  log_record_ptr->mac_hs_transition_type =              mac_cmd_ptr->mac_hs_transition_type;
  log_record_ptr->reset_timer_valid =                   mac_cmd_ptr->reset_timer_valid;

  log_ptr = (uint8*) log_record_ptr;
  log_ptr += FPOS(WCDMA_CMAC_DL_HS_LOGGING_type,hs_chanel_info);

#ifdef FEATURE_WCDMA_HS_FACH
  if(mac_cmd_ptr->hs_status_in_e_fach == TRUE)
  {
    log_record_ptr->ndlchan_mac_ehs_hs =               mac_cmd_ptr->efach_ndlchans;
    for (i=0; i< mac_cmd_ptr->efach_ndlchans;i++)
    {
      *(log_ptr++) = mac_cmd_ptr->efach_dl_logchan_info[i]->rb_id;
      *(log_ptr++) = mac_cmd_ptr->efach_dl_logchan_info[i]->rlc_id;
      *(log_ptr++) = mac_cmd_ptr->efach_dl_logchan_info[i]->mac_id;
      *(log_ptr++) = mac_cmd_ptr->efach_dl_logchan_info[i]->rlc_mode;
      *(log_ptr++) = mac_cmd_ptr->efach_dl_logchan_info[i]->chan_type;
      *(log_ptr++) = mac_cmd_ptr->efach_dl_logchan_info[i]->queue_id;
    }

    *(log_ptr++) =   mac_cmd_ptr->efach_num_queue;
    for (i=0; i< mac_cmd_ptr->efach_num_queue;i++)
    {
      *(log_ptr++) = mac_cmd_ptr->efach_queue_info[i]->queue_id;
      *(log_ptr++) = mac_cmd_ptr->efach_queue_info[i]->cctrch_id;
      *(log_ptr++) = mac_cmd_ptr->efach_queue_info[i]->window_size;

      RRCLOG_UINT16(log_ptr,mac_cmd_ptr->efach_queue_info[i]->release_timer);

      *(log_ptr++) = mac_cmd_ptr->efach_queue_info[i]->reset_timer;
      *(log_ptr++) = 0xFF ;/*MAC_D Flow ID associated*/
      *(log_ptr++) = 0xFF ;/*No of PDU sizes*/
    }
    *(log_ptr++) = 0;  /*Number of Mac-D flows*/

  }
  else
#endif  /*FEATURE_WCDMA_HS_FACH*/
  if(mac_cmd_ptr->mac_hs_or_ehs == MAC_EHS)
  {
    log_record_ptr->ndlchan_mac_ehs_hs =               mac_cmd_ptr->ndlchan_macehs;
    for (i=0; i< mac_cmd_ptr->ndlchan_macehs;i++)
    {
      *(log_ptr++) = mac_cmd_ptr->dl_macehs_logchan_info[i]->rb_id;
      *(log_ptr++) = mac_cmd_ptr->dl_macehs_logchan_info[i]->rlc_id; 
      *(log_ptr++) = mac_cmd_ptr->dl_macehs_logchan_info[i]->mac_id;
      *(log_ptr++) = mac_cmd_ptr->dl_macehs_logchan_info[i]->rlc_mode;
      *(log_ptr++) = mac_cmd_ptr->dl_macehs_logchan_info[i]->chan_type;
      *(log_ptr++) = mac_cmd_ptr->dl_macehs_logchan_info[i]->queue_id;
    }

    *(log_ptr++) = mac_cmd_ptr->num_mac_ehs_queue;
    for (i=0; i< mac_cmd_ptr->num_mac_ehs_queue;i++)
    {
      *(log_ptr++) = mac_cmd_ptr->hsdpa_ehs_queue_info[i]->queue_id;
      *(log_ptr++) = mac_cmd_ptr->hsdpa_ehs_queue_info[i]->cctrch_id;
      *(log_ptr++) = mac_cmd_ptr->hsdpa_ehs_queue_info[i]->window_size;

      RRCLOG_UINT16(log_ptr,mac_cmd_ptr->hsdpa_ehs_queue_info[i]->release_timer);

      *(log_ptr++) = mac_cmd_ptr->hsdpa_ehs_queue_info[i]->reset_timer;
      *(log_ptr++) = 0xFF ;/*MAC_D Flow ID associated*/
      *(log_ptr++) = 0xFF ;/*No of PDU sizes*/
    }
    *(log_ptr++) = 0;  /*Number of Mac-D flows*/

  }
  else if(mac_cmd_ptr->mac_hs_or_ehs == MAC_HS)
  {
    for(ix = 0; ix < mac_cmd_ptr->num_dflow; ix++)
    {
      num_queue += mac_cmd_ptr->hsdpa_info[ix]->no_of_queues;
      ndlchan += mac_cmd_ptr->hsdpa_info[ix]->ndlchan;

      for (iy=0; iy<mac_cmd_ptr->hsdpa_info[ix]->ndlchan; iy++)
      {
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->dlchan_info[iy].rb_id;
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->dlchan_info[iy].rlc_id;
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->dlchan_info[iy].mac_id;
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->dlchan_info[iy].rlc_mode;
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->dlchan_info[iy].chan_type;
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->mac_dflow_id;
      }
    }
    log_record_ptr->ndlchan_mac_ehs_hs =              ndlchan;

    *(log_ptr++) = num_queue;
    for(ix = 0; ix < mac_cmd_ptr->num_dflow; ix++)
    {
      for (iy=0; iy<mac_cmd_ptr->hsdpa_info[ix]->no_of_queues; iy++)
      {
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->queue_info[iy].queue_id;
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->cctrch_id;
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->queue_info[iy].window_size;

        RRCLOG_UINT16(log_ptr,mac_cmd_ptr->hsdpa_info[ix]->queue_info[iy].release_timer);

        *(log_ptr++) = 0xFF; /*Reset Timer*/
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->mac_dflow_id;
        *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->queue_info[iy].no_of_pdu_sizes;
      }
    }

    *(log_ptr++) = mac_cmd_ptr->num_dflow;
    for (ix=0; ix< mac_cmd_ptr->num_dflow;ix++)
    {
      *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->mac_dflow_id;
      *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->cctrch_id;
      *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->no_of_queues;
      *(log_ptr++) = mac_cmd_ptr->hsdpa_info[ix]->ndlchan;
    }

  }
  else /*MAC_HS_TYPE_INVALID*/
  {
    log_record_ptr->ndlchan_mac_ehs_hs = 0; /*Number of logical Channels*/
    *(log_ptr++) =   0; /* Number of Queues*/
    *(log_ptr++) =   0; /*Number of Flows*/
  }
  log_commit(log_record_ptr);

}

/*====================================================================
FUNCTION: rrclog_cmac_ul_hs_setup

DESCRIPTION:
  This function logs the HS configuration in uplink passed to MAC.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrclog_cmac_ul_hs_setup(wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr)
{
  mac_ul_config_type *mac_cmd_ptr = &(l2_ul_cmd_ptr->cmd_data.mac_config_info);
  mac_e_dch_info_s_type *e_dch_info = NULL;
  mac_e_mac_d_flow_info_s_type *e_mac_d_flow_info = NULL;

  uint8 num_e_mac_d_flow = 0;
  uint8 ix = 0,iy = 0;
  uint8 ndlchan = 0;

  WCDMA_CMAC_UL_HS_LOGGING_type *log_record_ptr;
  uint8 *log_ptr = NULL;

  if(!log_status(WCDMA_CMAC_UL_HS_LOGGING))
  {
    WRRC_MSG0_MED("WCDMA_CMAC_UL_HS_LOGGING not enabled");
    return;
  }

  log_record_ptr = (WCDMA_CMAC_UL_HS_LOGGING_type *)log_alloc( WCDMA_CMAC_UL_HS_LOGGING, sizeof(WCDMA_CMAC_UL_HS_LOGGING_type));

  if(log_record_ptr == NULL)
  {
    WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_CMAC_UL_HS_LOGGING_type));
    return;
  }

  log_record_ptr->version = 1;
  log_record_ptr->mac_config_changed =                  mac_cmd_ptr->mac_config_changed;
  log_record_ptr->wait_for_l1_sig =                     mac_cmd_ptr->wait_for_l1_sig;
  log_record_ptr->ul_pdu_type =                         mac_cmd_ptr->ul_pdu_type;
  log_record_ptr->ul_rlc_action =                       mac_cmd_ptr->ul_rlc_action;
  log_record_ptr->mac_eul_action =                      mac_cmd_ptr->mac_eul_action;
  log_record_ptr->mac_e_tsn_action  =                   mac_cmd_ptr->mac_e_tsn_action;
  log_record_ptr->mac_e_reset_indicator =               mac_cmd_ptr->mac_e_reset_indicator;

  log_ptr = (uint8*) log_record_ptr;
  log_ptr += FPOS(WCDMA_CMAC_UL_HS_LOGGING_type,edch_params);

  /*HSRACH Parameters*/
#ifdef FEATURE_WCDMA_HS_RACH
  *(log_ptr++) = mac_cmd_ptr->hs_rach_action;
  if(((mac_cmd_ptr->hs_rach_action == MAC_HS_RACH_START) ||
      (mac_cmd_ptr->hs_rach_action == MAC_HS_RACH_CONFIG)) &&
     (mac_cmd_ptr->hs_rach_config_ptr != NULL))
  {
    *(log_ptr++) = mac_cmd_ptr->hs_rach_to_eul_trans;
    *(log_ptr++) = mac_cmd_ptr->mac_hsrach_e_reset_indicator;
    *(log_ptr++) = mac_cmd_ptr->hs_rach_config_ptr->primary_e_rnti_present;
    RRCLOG_UINT16(log_ptr,mac_cmd_ptr->hs_rach_config_ptr->primary_e_rnti);
    *(log_ptr++) = mac_cmd_ptr->hs_rach_config_ptr->l1_e_dch_tx_cont_backoff;
    *(log_ptr++) = mac_cmd_ptr->hs_rach_config_ptr->add_e_dch_tx_backoff;
    *(log_ptr++) = mac_cmd_ptr->hs_rach_config_ptr->max_e_dch_res_alloc_ccch;
    *(log_ptr++) = mac_cmd_ptr->hs_rach_config_ptr->ccch_asc;
    *(log_ptr++) = mac_cmd_ptr->hs_rach_config_ptr->max_collision_resol;
  }
#else
  *(log_ptr++) = 0; /*HSRACH Action is NOOP*/
#endif

  if(((mac_cmd_ptr->mac_eul_action == MAC_EUL_START ||
     mac_cmd_ptr->mac_eul_action == MAC_EUL_CONFIG )) &&
     (mac_cmd_ptr->mac_e_config_ptr != NULL))
  {
    e_dch_info = &(mac_cmd_ptr->mac_e_config_ptr->e_dch_info);
    e_mac_d_flow_info = &(mac_cmd_ptr->mac_e_config_ptr->e_mac_d_flow_info[0]);
    num_e_mac_d_flow = mac_cmd_ptr->mac_e_config_ptr->num_e_mac_d_flow;

    *(log_ptr++) = mac_cmd_ptr->mac_e_config_ptr->e_tti;
  }
  else if(((mac_cmd_ptr->hs_rach_action == MAC_HS_RACH_START) ||
      (mac_cmd_ptr->hs_rach_action == MAC_HS_RACH_CONFIG)) &&
     (mac_cmd_ptr->hs_rach_config_ptr != NULL))
  {
    e_dch_info = &(mac_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_dch_info);
    e_mac_d_flow_info = &(mac_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_mac_d_flow_info[0]);
    num_e_mac_d_flow = mac_cmd_ptr->hs_rach_config_ptr->hs_rach_config.num_e_mac_d_flow;

    *(log_ptr++) = mac_cmd_ptr->hs_rach_config_ptr->hs_rach_config.e_tti;
  }

  if(e_dch_info != NULL)
  {
    RRCLOG_UINT16(log_ptr,e_dch_info->e_dpcch_info.happy_bit_delay_in_ms);
    *(log_ptr++) = e_dch_info->e_dpdch_info.e_tfci_table_index;
    *(log_ptr++) = e_dch_info->e_dpdch_info.e_tfci_min_set;
    RRCLOG_UINT16(log_ptr,e_dch_info->e_dpdch_info.sched_info_config.no_grant_periodicity_of_si);
    RRCLOG_UINT16(log_ptr,e_dch_info->e_dpdch_info.sched_info_config.grant_periodicity_of_si);
    *(log_ptr++) = e_dch_info->e_dpdch_info.sched_info_config.power_offset_of_si;
    *(log_ptr++) = e_dch_info->e_dpdch_info.sched_tx_config.sched_tx_grant_harq_allocation_2ms;
  }

  /*MAC-D Flow Parameters */
  *(log_ptr++) = num_e_mac_d_flow;
  for(ix = 0;ix < num_e_mac_d_flow;ix++)
  {
    ndlchan += e_mac_d_flow_info[ix].num_logch;

    *(log_ptr++) = e_mac_d_flow_info[ix].e_mac_d_flow_id;
    *(log_ptr++) = e_mac_d_flow_info[ix].num_logch;
    *(log_ptr++) = e_mac_d_flow_info[ix].e_mac_d_flow_power_offset;
    *(log_ptr++) = e_mac_d_flow_info[ix].e_mac_d_flow_max_num_of_retx;
    *(log_ptr++) = e_mac_d_flow_info[ix].e_mac_d_flow_mux_list;
    *(log_ptr++) = e_mac_d_flow_info[ix].e_tx_grant_info;
    RRCLOG_UINT16(log_ptr,e_mac_d_flow_info[ix].non_sched_tx_grant.max_mac_e_pdu_size_in_bits);
    *(log_ptr++) = e_mac_d_flow_info[ix].non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms;
  }

  /*Logical Channel parameters*/
  *(log_ptr++) = ndlchan;
  for(ix = 0;ix < num_e_mac_d_flow;ix++)
  {
    for(iy = 0;iy < e_mac_d_flow_info[ix].num_logch; iy++)
    {

      *(log_ptr++) = (uint8)(e_mac_d_flow_info[ix].e_ul_logch_info[iy].rb_id & 0x00FF);
      *(log_ptr++) = e_mac_d_flow_info[ix].e_ul_logch_info[iy].rlc_id;
      *(log_ptr++) = e_mac_d_flow_info[ix].e_mac_d_flow_id;
      *(log_ptr++) = e_mac_d_flow_info[ix].e_ul_logch_info[iy].rlc_mode;
      *(log_ptr++) = e_mac_d_flow_info[ix].e_ul_logch_info[iy].rlc_pdu_type;
      *(log_ptr++) = e_mac_d_flow_info[ix].e_ul_logch_info[iy].ul_li_size;
      *(log_ptr++) = e_mac_d_flow_info[ix].e_ul_logch_info[iy].chan_type;
      *(log_ptr++) = e_mac_d_flow_info[ix].e_ul_logch_info[iy].priority;
      RRCLOG_UINT16(log_ptr,e_mac_d_flow_info[ix].e_ul_logch_info[iy].ul_flex_min_pdu_size);
      RRCLOG_UINT16(log_ptr,e_mac_d_flow_info[ix].e_ul_logch_info[iy].ul_flex_max_pdu_size);
    }
  }

  log_commit(log_record_ptr);

}

/*====================================================================
FUNCTION: rrc_log_event_inter_freq_hho_update_status

DESCRIPTION:
  This function logs the status of inter frequency hard hand off.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_log_event_inter_freq_hho_update_status(rrc_inter_freq_hho_status_e_type status)
{
  rrc_log_inter_freq_hho_status_event_type log_status;
	WRRC_MSG3_MED("rrc_log_inter_freq_hho_started : %d",rrc_log_inter_freq_hho_started,0,0);

  log_status.status = status;
  if(rrc_log_inter_freq_hho_started == TRUE)
  {
    MSG_HIGH("LOG INTER FREQ HHO event: status %d rrc_log_inter_freq_hho_started: %d",status,rrc_log_inter_freq_hho_started,0);
    event_report_payload(EVENT_WCDMA_RRC_INTER_FREQ_HHO_STATUS, sizeof(log_status),(void *)&log_status);
  }
  if(status != RRC_LOG_INTER_FREQ_HHO_STARTED )
  {
    rrc_log_inter_freq_hho_started = FALSE;
  }
}

/*====================================================================
FUNCTION: rrc_hanging_rb_info

DESCRIPTION:
  This function logs the hanging rb information.
  Log code 0x4804

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_log_hanging_rb_info( void )
{
  uint8 *log_ptr = NULL;
  WCDMA_HANGING_RB_INFO_type* log_record_ptr = NULL;

  uint8 rb_index, combo_index, combo_mask;
  uint8 count=0, num_mapping_combo=0, num_rb_in_use=0;
  uint16 size=0;

  uint8 valid_rb_index[MAX_HANGING_RB_MAPPING_INFO] = {0};          /*To store the indices of the valid RB's*/
  uint8 num_mapping_combo_per_rb[MAX_HANGING_RB_MAPPING_INFO] = {0};

  if(!log_status(WCDMA_HANGING_RB_INFO))
  {	
    WRRC_MSG0_MED("WCDMA_HANGING_RB_INFO logging is not enabled");
    return;
  }

  /* Finding the rb's in use and the mapping combo per rb*/
  for(rb_index=0; rb_index<MAX_HANGING_RB_MAPPING_INFO; rb_index++)
  {
    /* Check if rb is in use*/
    if(rrc_ordered_hanging_rb[rb_index].rb_id != MAX_RB)
    {
      num_mapping_combo = 0; /* Initialized to zero*/
      for(combo_index=0; combo_index<MAX_MAPPING_COMBO; combo_index++)
      {
        /*Check which rb has a mapping combination*/
        if(rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].combo_type != 0)
        {
          num_mapping_combo +=1;
        }
      }
      
      /* If the rb has atleast one mapping combo then store it for logging*/
      if(num_mapping_combo !=0)
      {
        valid_rb_index[count] = rb_index;
        num_mapping_combo_per_rb[count] = num_mapping_combo; 
        count +=1; /*Increment to store the info about the next rb in use */
      }
    }
  }
  num_rb_in_use = count;

  /* Return if none of the RB has any mapping combo*/
  if(num_rb_in_use == 0)
  {
     return;
  }

  /*Size computation*/
  size += FPOS(WCDMA_HANGING_RB_INFO_type, rrclog_hrb_info);
  for(count=0; count<num_rb_in_use; count++)
  {
    size += SIZE_RRCLOG_HRB_INFO_TYPE(num_mapping_combo_per_rb[count]);
  }

  /*Allocating memory for the log record pointer*/
  log_record_ptr = (WCDMA_HANGING_RB_INFO_type *)log_alloc( WCDMA_HANGING_RB_INFO, size);
  if(log_record_ptr == NULL)
  {
    WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d for WCDMA_HANGING_RB_INFO logging",size);
    return;
  }
  
  /*Storing in the log_record_ptr*/
  log_record_ptr->version = 1;
  log_record_ptr->num_rb_in_use = num_rb_in_use;
  log_ptr = (uint8*) log_record_ptr;
  log_ptr += FPOS(WCDMA_HANGING_RB_INFO_type,rrclog_hrb_info);

  /* Find the rb in use*/
  for(count=0; count<num_rb_in_use; count++)
  {
    /*Get the index of the valid rb*/
    rb_index = valid_rb_index[count];
     
     /* For the rb in use*/
    *(log_ptr++) = rrc_ordered_hanging_rb[rb_index].rb_id;
    *(log_ptr++) = rrclcm_get_ul_rlc_lc_id(rrc_ordered_hanging_rb[rb_index].rb_id);
    *(log_ptr++) = num_mapping_combo_per_rb[count];

    for(combo_index=0; combo_index<MAX_MAPPING_COMBO && rb_index<MAX_HANGING_RB_MAPPING_INFO ; combo_index++)
    {
      if(rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].combo_type != 0)
      {
        combo_mask = rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].combo_type;

       /*Log the Uplink Mapping Type*/
       *(log_ptr++)= (combo_mask & 0xF0);

        switch(combo_mask & 0xF0)
        {
          case UL_DCH_MAPPING:
            *(log_ptr++) = rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].ul_map.dch_map_info.tr_id;
            break;
          case UL_EDCH_MAPPING:
            *(log_ptr++) = rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].ul_map.edch_map_info.mac_e_flow_id;
            break;
          case UL_RACH_MAPPING:
            *(log_ptr++) = rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].ul_map.rach_map_info.tr_id;
            break;
          default:
            *(log_ptr++) = 0xFF;
            break;
        }
         
       /*Log the Downlink Mapping Type*/
       *(log_ptr++)= (combo_mask & 0x0F);
        switch(combo_mask & 0x0F)
        {
          case DL_DCH_MAPPING:
            *(log_ptr++) = rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].dl_map.dch_map_info.tr_id;
            break;
          case DL_MAC_D_MAPPING:
            *(log_ptr++) = rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].dl_map.macd_map_info.mac_d_flow_id;
            break;
          case DL_MAC_EHS_MAPPING:
            *(log_ptr++) = rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].dl_map.macehs_map_info.queue_id;
            break;
          case DL_FACH_MAPPING:
            *(log_ptr++) = rrc_ordered_hanging_rb[rb_index].mapping_info[combo_index].dl_map.fach_map_info.fach_id;
            break;
          default:
            *(log_ptr++) = 0xFF;
            break;
        }
      }
    }
  }

  log_commit(log_record_ptr);

}/* rrc_hanging_rb_info */

/*====================================================================
FUNCTION: rrc_log_standalone_hanging_rb_info

DESCRIPTION:
  This function logs the hanging rb information.
  Log code 0x4804

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_log_standalone_hanging_rb_info(void)
{
  uint8 *log_ptr = NULL;
  WCDMA_HANGING_RB_MAPPING_INFO_type* log_record_ptr = NULL;

  uint8 rb_index=0, count=0, num_valid_mappings=0, num_rb_in_use=0;
  uint16 size=0;

  uint8 valid_rb_index[MAX_HANGING_RB_MAPPING_INFO] = {0};          /*To store the indices of the valid RB's*/
  uint8 num_valid_mappings_per_rb[MAX_HANGING_RB_MAPPING_INFO] = {0};

  if(!log_status(WCDMA_HANGING_RB_MAPPING_INFO))
  {	
    WRRC_MSG0_MED("WCDMA_STANDALONE_HANGING_RB_MAPPING_INFO logging is not enabled");
    return;
  }

  /* Finding the num of rb's in use and the mappings per rb*/
  for(rb_index=0; rb_index<MAX_HANGING_RB_MAPPING_INFO; rb_index++)
  {
    /* Check if rb is in use*/
    if(rrc_ordered_hanging_rb_mapping[rb_index].in_use == TRUE)
    {
      num_valid_mappings= 0; /* Initialized to zero*/

      if(rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.ul_dch_info.valid)
      {
        num_valid_mappings++;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.rach_info.valid)
      {
        num_valid_mappings++;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.ul_mace.valid)
      {
        num_valid_mappings++;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.dch_map_info.valid)
      {
        num_valid_mappings++;

      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.fach_map_info.valid)
      {
        num_valid_mappings++;

      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.macd_map_info.valid)
      {
        num_valid_mappings++;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.macehs_map_info.valid)
      {
        num_valid_mappings++;
      }
      
      /* If the rb has atleast one mapping combo then store it for logging*/
      if(num_valid_mappings !=0)
      {
        valid_rb_index[count] = rb_index;
        num_valid_mappings_per_rb[count] = num_valid_mappings; 
        count +=1; /*Increment to store the info about the next rb in use */
      }
    }
  }
  num_rb_in_use = count; /*Store the num of rb's in use */

  /* Return if none of the RBs has any mapping*/
  if(num_rb_in_use == 0)
  {
     return;
  }

  /*Size computation*/
  size += FPOS(WCDMA_HANGING_RB_MAPPING_INFO_type, rrclog_hrb2_info);
  for(count=0; count<num_rb_in_use; count++)
  {
    size += SIZE_RRCLOG_HRB2_INFO_TYPE(num_valid_mappings_per_rb[count]);
  }

  /*Allocating memory for the log record pointer*/
  log_record_ptr = (WCDMA_HANGING_RB_MAPPING_INFO_type *)log_alloc( WCDMA_HANGING_RB_MAPPING_INFO, size);
  if(log_record_ptr == NULL)
  {
    WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d for WCDMA_STANDALONE_HANGING_RB_INFO logging",size);
    return;
  }
  
  /*Storing in the log_record_ptr*/
  log_record_ptr->version = 1;
  log_record_ptr->num_rb_in_use = num_rb_in_use;
  log_ptr = (uint8*) log_record_ptr;
  log_ptr += FPOS(WCDMA_HANGING_RB_MAPPING_INFO_type,rrclog_hrb2_info);

  /* Find the rb in use*/
  for(count=0; count<num_rb_in_use; count++)
  {
    /*Get the index of the valid rb*/
    rb_index = valid_rb_index[count];
     
     /* For the rb in use*/
    *(log_ptr++) = rb_index;
    *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].rb_id;
    *(log_ptr++) = rrclcm_get_ul_rlc_lc_id(rrc_ordered_hanging_rb_mapping[rb_index].rb_id);
    *(log_ptr++) = num_valid_mappings_per_rb[count];

      if(rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.ul_dch_info.valid)
      {
        *(log_ptr++) = UL_DCH_RB_MAPPING;
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.ul_dch_info.tr_id,
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.ul_dch_info.log_channel_id;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.rach_info.valid)
      {
        *(log_ptr++) = RACH_RB_MAPPING;
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.rach_info.tr_id,
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.rach_info.log_channel_id;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.ul_mace.valid)
      {
        *(log_ptr++) = E_DCH_RB_MAPPING;
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.ul_mace.mac_e_flow_id,
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].ul_mapping_info.ul_mace.log_channel_id;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.dch_map_info.valid)
      {
        *(log_ptr++) = DL_DCH_RB_MAPPING;
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.dch_map_info.tr_id,
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.dch_map_info.log_channel_id;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.fach_map_info.valid)
      {
        *(log_ptr++) = FACH_RB_MAPPING;
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.fach_map_info.fach_id,
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.fach_map_info.log_channel_id;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.macd_map_info.valid)
      {
        *(log_ptr++) = MAC_D_RB_MAPPING;
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.macd_map_info.mac_d_flow_id,
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.macd_map_info.log_channel_id;
      }
      if(rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.macehs_map_info.valid)
      {
        *(log_ptr++) = MAC_EHS_RB_MAPPING;
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.macehs_map_info.queue_id,
        *(log_ptr++) = rrc_ordered_hanging_rb_mapping[rb_index].dl_mapping_info.macehs_map_info.log_channel_id;
      }
    }

  log_commit(log_record_ptr);

} /*rrc_log_standalone_hanging_rb_info*/

/*=========================================================================*/
/*=========================================================================
FUNCTION: rrc_log_meas_info
  
DESCRIPTION:
  This function logs the measurments read from SIB11/12
  Log code 0x4805
DEPENDENCIES:
  None.

RETURN VALUE:
  None
  
SIDE EFFECTS:
  None.
====================================================================*/
void rrc_log_meas_info
(
  l1_meas_sib_parm_struct_type* rrcmeas_sib_ptr,
  l1_meas_ctrl_parm_struct_type* rrcmeas_mcm_ptr,
  l1_meas_trans_type* rrcmeas_trans_ptr,
  l1_meas_ctrl_enum_type  meas_choice,
  l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib_ptr
)
{
  WCDMA_MEAS_LOGGING_INTRAFREQ_type* log_record_ptr_intraf = NULL;
  WCDMA_MEAS_LOGGING_INTERFREQ_type* log_record_ptr_interf = NULL;
  WCDMA_MEAS_LOGGING_IRAT_type* log_record_ptr_inters = NULL;
  uint8 *log_ptr = NULL;
  uint8 count, i = 0;
  uint16 psc_mask = 0, uarfcn_dl_mask = 0;
  if(meas_choice == L1_MEAS_CTRL_PARMS)
  {
    if(rrcmeas_mcm_ptr->meas_object.meas_type == L1_INTRA_FREQ)
    {
      log_record_ptr_intraf = (WCDMA_MEAS_LOGGING_INTRAFREQ_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_INTRAFREQ,sizeof(WCDMA_MEAS_LOGGING_INTRAFREQ_type));
      if(log_record_ptr_intraf == NULL)
      {
        WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_INTRAFREQ_type));
        return;
      }
      log_record_ptr_intraf->version = 1;
      log_record_ptr_intraf->rpt_cell_stat = rrcmeas_mcm_ptr->meas_object.u.intra_f.rpt_cell_status[PRI_FREQ].rpt_cell_stat;
      log_record_ptr_intraf->rpt_cell_type = rrcmeas_mcm_ptr->meas_object.u.intra_f.rpt_cell_status[PRI_FREQ].rpt_cell_type;
      log_record_ptr_intraf->maxrptcells = rrcmeas_mcm_ptr->meas_object.u.intra_f.rpt_cell_status[PRI_FREQ].max_num_rpt_cells;
      log_record_ptr_intraf->num_events = rrcmeas_mcm_ptr->meas_object.u.intra_f.rpt_crit[PRI_FREQ].num_event;
      log_record_ptr_intraf->num_cells_added = rrcmeas_mcm_ptr->meas_object.u.intra_f.cell_list[PRI_FREQ].num_cell_add;
      log_ptr = (uint8*) log_record_ptr_intraf;
      log_ptr += FPOS(WCDMA_MEAS_LOGGING_INTRAFREQ_type,intraf_event_info);
      for(count=0;count<rrcmeas_mcm_ptr->meas_object.u.intra_f.rpt_crit[PRI_FREQ].num_event;count++)
      {
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.intra_f.rpt_crit[PRI_FREQ].evt[count].evt_id;
      }
      for(i=0;i<log_record_ptr_intraf->num_cells_added;i++)
      {
        psc_mask  = rrcmeas_mcm_ptr->meas_object.u.intra_f.cell_list[PRI_FREQ].add_cell[i].psc;
        RRCLOG_UINT16(log_ptr,psc_mask);
        *(log_ptr++) = i;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.intra_f.cell_list[PRI_FREQ].add_cell[i].measure_in_idle_pch_fach;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.intra_f.cell_list[PRI_FREQ].add_cell[i].cell_info.cell_offset;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.intra_f.cell_list[PRI_FREQ].add_cell[i].cell_info.rd_sfn_ind;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.intra_f.cell_list[PRI_FREQ].add_cell[i].cell_info.pri_cpich_txpwr;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.intra_f.cell_list[PRI_FREQ].add_cell[i].cell_info.tx_diver_ind;
      }
      log_commit(log_record_ptr_intraf);
    }
    else if(rrcmeas_mcm_ptr->meas_object.meas_type == L1_INTER_FREQ)
    {
      if(!log_status(WCDMA_MEAS_LOGGING_INTERFREQ))
      {
        WRRC_MSG0_HIGH("WCDMA_MEAS_LOGGING_INTERFREQ not enabled");
        return;
      }
      log_record_ptr_interf = (WCDMA_MEAS_LOGGING_INTERFREQ_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_INTERFREQ,sizeof(WCDMA_MEAS_LOGGING_INTERFREQ_type));
      if(log_record_ptr_interf == NULL)
      {
        WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_INTERFREQ_type));
        return;
      }
      log_record_ptr_interf->version = 1;
      log_record_ptr_interf->rpt_cell_stat = rrcmeas_mcm_ptr->meas_object.u.inter_f.rpt_cell_status.rpt_cell_stat;
      log_record_ptr_interf->rpt_cell_type = rrcmeas_mcm_ptr->meas_object.u.inter_f.rpt_cell_status.rpt_cell_type;
      log_record_ptr_interf->maxrptcells = rrcmeas_mcm_ptr->meas_object.u.inter_f.rpt_cell_status.max_num_rpt_cells;
      log_record_ptr_interf->num_events = rrcmeas_mcm_ptr->meas_object.u.inter_f.u.inter_rpt_crit.num_event;
      log_record_ptr_interf->num_cells_added = rrcmeas_mcm_ptr->meas_object.u.inter_f.cell_list.num_cell_add;
      log_ptr= (uint8*)log_record_ptr_interf;
      log_ptr += FPOS(WCDMA_MEAS_LOGGING_INTERFREQ_type,interf_event_info);
      for(count=0;count<rrcmeas_mcm_ptr->meas_object.u.inter_f.u.inter_rpt_crit.num_event;count++)
      {
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.inter_f.u.inter_rpt_crit.evt[count].evt_id;
      }
      for(i=0;i<log_record_ptr_interf->num_cells_added;i++)
      {
        psc_mask   = rrcmeas_mcm_ptr->meas_object.u.inter_f.cell_list.add_cell[i].psc;
        RRCLOG_UINT16(log_ptr,psc_mask);
        *(log_ptr++) = i;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.inter_f.cell_list.add_cell[i].measure_in_idle_pch_fach;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.inter_f.cell_list.add_cell[i].cell_info.cell_offset;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.inter_f.cell_list.add_cell[i].cell_info.rd_sfn_ind;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.inter_f.cell_list.add_cell[i].cell_info.pri_cpich_txpwr;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.inter_f.cell_list.add_cell[i].cell_info.tx_diver_ind;
        uarfcn_dl_mask = rrcmeas_mcm_ptr->meas_object.u.inter_f.cell_list.add_cell[i].freq_info.uarfcn_dl;
        RRCLOG_UINT16(log_ptr,uarfcn_dl_mask);
      }
      log_commit(log_record_ptr_interf);
    }
    else if(rrcmeas_mcm_ptr->meas_object.meas_type == L1_INTER_SYS)
    {
      if(!log_status(WCDMA_MEAS_LOGGING_IRAT))
      {
        WRRC_MSG0_HIGH("WCDMA_MEAS_LOGGING_IRAT not enabled");
        return;
      }
      log_record_ptr_inters = (WCDMA_MEAS_LOGGING_IRAT_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_IRAT,sizeof(WCDMA_MEAS_LOGGING_IRAT_type));
      if(log_record_ptr_inters == NULL)
      {
        WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_IRAT_type));
        return;
      }
      log_record_ptr_inters->version = 1;
      log_record_ptr_inters->rpt_cell_stat = rrcmeas_mcm_ptr->meas_object.u.inter_s.rpt_cell_status.rpt_cell_stat;
      log_record_ptr_inters->rpt_cell_type = rrcmeas_mcm_ptr->meas_object.u.inter_s.rpt_cell_status.rpt_cell_type;
      log_record_ptr_inters->maxrptcells = rrcmeas_mcm_ptr->meas_object.u.inter_s.rpt_cell_status.max_num_rpt_cells;
      log_record_ptr_inters->num_events = rrcmeas_mcm_ptr->meas_object.u.inter_s.rpt_criteria.u.evt_rpt_crit.num_event;
      log_record_ptr_inters->num_cells_added = rrcmeas_mcm_ptr->meas_object.u.inter_s.cell_list.num_cell_add;
      log_ptr = (uint8*)log_record_ptr_inters;
      log_ptr += FPOS(WCDMA_MEAS_LOGGING_IRAT_type,inters_event_info);
      for(count=0;count<rrcmeas_mcm_ptr->meas_object.u.inter_s.rpt_criteria.u.evt_rpt_crit.num_event;count++)
      {
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.inter_s.rpt_criteria.u.evt_rpt_crit.evt[count].evt_id;
      }
      for(i=0;i<log_record_ptr_inters->num_cells_added;i++)
      {
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.inter_s.cell_list.add_cell[i].intersys_cell_id;
        *(log_ptr++) = rrcmeas_mcm_ptr->meas_object.u.inter_s.cell_list.add_cell[i].measure_in_idle_pch_fach;
      }
      log_commit(log_record_ptr_inters);
    }
  }
  else if(meas_choice == L1_SIB_PARMS)
  {
    if(rrcmeas_sib_ptr->intra_freq_meas_info_incl)
    {
      if(!log_status(WCDMA_MEAS_LOGGING_INTRAFREQ))
      {
        WRRC_MSG0_HIGH("WCDMA_MEAS_LOGGING_INTRAFREQ not enabled");
        return;
      }
      log_record_ptr_intraf = (WCDMA_MEAS_LOGGING_INTRAFREQ_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_INTRAFREQ,sizeof(WCDMA_MEAS_LOGGING_INTRAFREQ_type));
      if(log_record_ptr_intraf == NULL)
      {
        WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_INTRAFREQ_type));
        return;
      }
      log_record_ptr_intraf->version = 1;
      log_record_ptr_intraf->rpt_cell_stat = 255;
      log_record_ptr_intraf->rpt_cell_type = 255;
      log_record_ptr_intraf->maxrptcells = 255;
      log_record_ptr_intraf->num_events = rrcmeas_sib_ptr->intra_f.dch_rpt_info.rpt_crit.num_event;
      log_record_ptr_intraf->num_cells_added = rrcmeas_sib_ptr->intra_f.cell_list.num_cell_add;
      log_ptr = (uint8*)log_record_ptr_intraf;
      log_ptr += FPOS(WCDMA_MEAS_LOGGING_INTRAFREQ_type,intraf_event_info);
      for(count=0;count<rrcmeas_sib_ptr->intra_f.dch_rpt_info.rpt_crit.num_event;count++)
      {
        *(log_ptr++) = rrcmeas_sib_ptr->intra_f.dch_rpt_info.rpt_crit.evt[count].evt_id;
      }
      for(i=0;i<log_record_ptr_intraf->num_cells_added;i++)
      {
        psc_mask = rrcmeas_sib_ptr->intra_f.cell_list.add_cell[i].psc;
        RRCLOG_UINT16(log_ptr,psc_mask);
        *(log_ptr++) = i;
        *(log_ptr++) = rrcmeas_sib_ptr->intra_f.cell_list.add_cell[i].measure_in_idle_pch_fach;
        *(log_ptr++) = rrcmeas_sib_ptr->intra_f.cell_list.add_cell[i].cell_info.cell_offset;
        *(log_ptr++) = rrcmeas_sib_ptr->intra_f.cell_list.add_cell[i].cell_info.rd_sfn_ind;
        *(log_ptr++) = rrcmeas_sib_ptr->intra_f.cell_list.add_cell[i].cell_info.pri_cpich_txpwr;
        *(log_ptr++) = rrcmeas_sib_ptr->intra_f.cell_list.add_cell[i].cell_info.tx_diver_ind;
      }
      log_commit(log_record_ptr_intraf);
    }
    if(rrcmeas_sib_ptr->inter_sys_meas_info_incl)
    {
      if(!log_status(WCDMA_MEAS_LOGGING_IRAT))
      {
        WRRC_MSG0_HIGH("WCDMA_MEAS_LOGGING_IRAT not enabled");
        return;
      }
      log_record_ptr_inters = (WCDMA_MEAS_LOGGING_IRAT_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_IRAT,sizeof(WCDMA_MEAS_LOGGING_IRAT_type));
      if(log_record_ptr_inters == NULL)
      {
        WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_IRAT_type));
        return;
      }
      log_record_ptr_inters->version = 1;
      log_record_ptr_inters->rpt_cell_stat = 255;
      log_record_ptr_inters->rpt_cell_type = 255;
      log_record_ptr_inters->maxrptcells = 255;
      log_record_ptr_inters->num_events  = 0;
      log_record_ptr_inters->num_cells_added = rrcmeas_sib_ptr->inter_s.num_cell_add;
      log_ptr = (uint8*)log_record_ptr_inters;
      log_ptr += FPOS(WCDMA_MEAS_LOGGING_IRAT_type,inters_event_info);
      for(i=0;i<log_record_ptr_inters->num_cells_added;i++)
      {
        *(log_ptr++) = rrcmeas_sib_ptr->inter_s.add_cell[i].intersys_cell_id;
        *(log_ptr++) = rrcmeas_sib_ptr->inter_s.add_cell[i].measure_in_idle_pch_fach;
      }
      log_commit(log_record_ptr_inters);
    }
  }
  else if(meas_choice == L1_INTER_FREQ_SIB_PARMS)
  {
    if(!log_status(WCDMA_MEAS_LOGGING_INTERFREQ))
    {
      WRRC_MSG0_HIGH("WCDMA_MEAS_LOGGING_INTERFREQ not enabled");
      return;
    }
    log_record_ptr_interf = (WCDMA_MEAS_LOGGING_INTERFREQ_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_INTERFREQ,sizeof(WCDMA_MEAS_LOGGING_INTERFREQ_type));
    if(log_record_ptr_interf == NULL)
    {
      WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_INTERFREQ_type));
      return;
    }
    log_record_ptr_interf->version = 1;
    log_record_ptr_interf->rpt_cell_stat = 255;
    log_record_ptr_interf->rpt_cell_type = 255;
    log_record_ptr_interf->maxrptcells = 255;
    log_record_ptr_interf->num_events = 0;
    log_record_ptr_interf->num_cells_added = l1_inter_freq_sib_ptr->num_cell_add;
    log_ptr = (uint8*)log_record_ptr_interf;
    log_ptr += FPOS(WCDMA_MEAS_LOGGING_INTERFREQ_type,interf_event_info);
    for(i=0;i<log_record_ptr_interf->num_cells_added;i++)
    {
      psc_mask = l1_inter_freq_sib_ptr->add_cell[i].psc;
      RRCLOG_UINT16(log_ptr,psc_mask);
      *(log_ptr++) = i;
      *(log_ptr++) = l1_inter_freq_sib_ptr->add_cell[i].measure_in_idle_pch_fach;
      *(log_ptr++) = l1_inter_freq_sib_ptr->add_cell[i].cell_info.cell_offset;
      *(log_ptr++) = l1_inter_freq_sib_ptr->add_cell[i].cell_info.rd_sfn_ind;
      *(log_ptr++) = l1_inter_freq_sib_ptr->add_cell[i].cell_info.pri_cpich_txpwr;
      *(log_ptr++) = l1_inter_freq_sib_ptr->add_cell[i].cell_info.tx_diver_ind;
      uarfcn_dl_mask = l1_inter_freq_sib_ptr->add_cell[i].freq_info.uarfcn_dl;
      RRCLOG_UINT16(log_ptr,uarfcn_dl_mask);
    }
    log_commit(log_record_ptr_interf);
  }
  else
  {
    if(rrcmeas_trans_ptr->meas_type == L1_INTRA_FREQ)
    {
      if(!log_status(WCDMA_MEAS_LOGGING_INTRAFREQ))
      {
        WRRC_MSG0_HIGH("WCDMA_MEAS_LOGGING_INTRAFREQ not enabled");
        return;
      }
      if((rrcmeas_trans_ptr->meas_action == L1_MEAS_STOP_AND_CONFIG_CELL_LIST) && (rrcmeas_trans_ptr->u.l1_new_intra_freq_cell_list.cell_list_incl))
      {
        log_record_ptr_intraf = (WCDMA_MEAS_LOGGING_INTRAFREQ_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_INTRAFREQ,sizeof(WCDMA_MEAS_LOGGING_INTRAFREQ_type));
        if(log_record_ptr_intraf == NULL)
        {
          WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_INTRAFREQ_type));
          return;
        }
        log_record_ptr_intraf->version = 1;
        log_record_ptr_intraf->rpt_cell_stat = 255;
        log_record_ptr_intraf->rpt_cell_type = 255;
        log_record_ptr_intraf->maxrptcells = 255;
        log_record_ptr_intraf->num_events = 0;
        log_record_ptr_intraf->num_cells_added = rrcmeas_trans_ptr->u.l1_new_intra_freq_cell_list.cell_list.num_cell_add;
        log_ptr = (uint8*)log_record_ptr_intraf;
        log_ptr += FPOS(WCDMA_MEAS_LOGGING_INTRAFREQ_type,intraf_event_info);
        for(i=0;i<log_record_ptr_intraf->num_cells_added;i++)
        {
          psc_mask = rrcmeas_trans_ptr->u.l1_new_intra_freq_cell_list.cell_list.add_cell[i].psc;
          RRCLOG_UINT16(log_ptr,psc_mask);
          *(log_ptr++) = i;
          *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_cell_list.cell_list.add_cell[i].measure_in_idle_pch_fach;
          *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_cell_list.cell_list.add_cell[i].cell_info.cell_offset;
          *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_cell_list.cell_list.add_cell[i].cell_info.rd_sfn_ind;
          *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_cell_list.cell_list.add_cell[i].cell_info.pri_cpich_txpwr;
          *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_cell_list.cell_list.add_cell[i].cell_info.tx_diver_ind;
        }
        log_commit(log_record_ptr_intraf);
      }
      else if(rrcmeas_trans_ptr->meas_action == L1_MEAS_STOP_AND_OVERWRITE)
      {
        if(rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_freq_meas_info_incl)
        {
          log_record_ptr_intraf = (WCDMA_MEAS_LOGGING_INTRAFREQ_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_INTRAFREQ,sizeof(WCDMA_MEAS_LOGGING_INTRAFREQ_type));
          if(log_record_ptr_intraf == NULL)
          {
            WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_INTRAFREQ_type));
            return;
          }
          log_record_ptr_intraf->version= 1;
          log_record_ptr_intraf->rpt_cell_stat = 255;
          log_record_ptr_intraf->rpt_cell_type = 255;
          log_record_ptr_intraf->maxrptcells = 255;
          log_record_ptr_intraf->num_events = rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.dch_rpt_info.rpt_crit.num_event;
          log_record_ptr_intraf->num_cells_added = rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.cell_list.num_cell_add;
          log_ptr = (uint8*)log_record_ptr_intraf;
          log_ptr += FPOS(WCDMA_MEAS_LOGGING_INTRAFREQ_type,intraf_event_info);
          for(count=0;count<rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.dch_rpt_info.rpt_crit.num_event;count++)
          {
            *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.dch_rpt_info.rpt_crit.evt[count].evt_id;
          }
          for(i=0;i<log_record_ptr_intraf->num_cells_added;i++)
          {
            psc_mask = rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.cell_list.add_cell[i].psc;
            RRCLOG_UINT16(log_ptr,psc_mask);
            *(log_ptr++) = i;
            *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.cell_list.add_cell[i].measure_in_idle_pch_fach;
            *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.cell_list.add_cell[i].cell_info.cell_offset;
            *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.cell_list.add_cell[i].cell_info.rd_sfn_ind;
            *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.cell_list.add_cell[i].cell_info.pri_cpich_txpwr;
            *(log_ptr++) = rrcmeas_trans_ptr->u.l1_new_intra_freq_sib_meas_params.intra_f.cell_list.add_cell[i].cell_info.tx_diver_ind;
          }
          log_commit(log_record_ptr_intraf);
        }
      }
    }
    if((rrcmeas_trans_ptr->meas_type == L1_INTER_FREQ) && (rrcmeas_trans_ptr->meas_action == L1_MEAS_STOP_AND_CONFIG_CELL_LIST))
    {
      if(!log_status(WCDMA_MEAS_LOGGING_INTERFREQ))
      {
        WRRC_MSG0_HIGH("WCDMA_MEAS_LOGGING_INTERFREQ not enabled");
        return;
      }
      log_record_ptr_interf = (WCDMA_MEAS_LOGGING_INTERFREQ_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_INTERFREQ,sizeof(WCDMA_MEAS_LOGGING_INTERFREQ_type));
      if(log_record_ptr_interf == NULL)
      {
        WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_INTERFREQ_type));
        return;
      }
      log_record_ptr_interf->version = 1;
      log_record_ptr_interf->rpt_cell_stat = 255;
      log_record_ptr_interf->rpt_cell_type = 255;
      log_record_ptr_interf->maxrptcells = 255;
      log_record_ptr_interf->num_events = 0;
      log_record_ptr_interf->num_cells_added = rrcmeas_trans_ptr->u.l1_inter_freq_cell_list.num_cell_add;
      log_ptr = (uint8*)log_record_ptr_interf;
      log_ptr += FPOS(WCDMA_MEAS_LOGGING_INTERFREQ_type,interf_event_info);
      for(i=0;i<log_record_ptr_interf->num_cells_added;i++)
      {
        psc_mask                                   = rrcmeas_trans_ptr->u.l1_inter_freq_cell_list.add_cell[i].psc;
        RRCLOG_UINT16(log_ptr,psc_mask);
        *(log_ptr++) = i;
        *(log_ptr++) = rrcmeas_trans_ptr->u.l1_inter_freq_cell_list.add_cell[i].measure_in_idle_pch_fach;
        *(log_ptr++) = rrcmeas_trans_ptr->u.l1_inter_freq_cell_list.add_cell[i].cell_info.cell_offset;
        *(log_ptr++) = rrcmeas_trans_ptr->u.l1_inter_freq_cell_list.add_cell[i].cell_info.rd_sfn_ind;
        *(log_ptr++) = rrcmeas_trans_ptr->u.l1_inter_freq_cell_list.add_cell[i].cell_info.pri_cpich_txpwr;
        *(log_ptr++) = rrcmeas_trans_ptr->u.l1_inter_freq_cell_list.add_cell[i].cell_info.tx_diver_ind;
        uarfcn_dl_mask = rrcmeas_trans_ptr->u.l1_inter_freq_cell_list.add_cell[i].freq_info.uarfcn_dl;
        RRCLOG_UINT16(log_ptr,uarfcn_dl_mask);
      }
      log_commit(log_record_ptr_interf);
    }
    if(rrcmeas_trans_ptr->meas_type == L1_INTER_SYS)
    {
      if(!log_status(WCDMA_MEAS_LOGGING_IRAT))
      {
        WRRC_MSG0_HIGH("WCDMA_MEAS_LOGGING_IRAT not enabled");
        return;
      }
      log_record_ptr_inters = (WCDMA_MEAS_LOGGING_IRAT_type*)WRRC_LOG_ALLOC(WCDMA_MEAS_LOGGING_IRAT,sizeof(WCDMA_MEAS_LOGGING_IRAT_type));
      if(log_record_ptr_inters == NULL)
      {
        WRRC_MSG1_ERROR("RRCLOG: Unable to allocate memory of size %d",sizeof(WCDMA_MEAS_LOGGING_IRAT_type));
        return;
      }
      log_record_ptr_inters->version = 1;
      log_record_ptr_inters->rpt_cell_stat = 255;
      log_record_ptr_inters->rpt_cell_type = 255;
      log_record_ptr_inters->maxrptcells = 255;
      log_record_ptr_inters->num_events = 0;
      log_record_ptr_inters->num_cells_added= rrcmeas_trans_ptr->u.inter_s.num_cell_add;
      log_ptr = (uint8*)log_record_ptr_inters;
      log_ptr += FPOS(WCDMA_MEAS_LOGGING_IRAT_type,inters_event_info);
      for(i=0;i<log_record_ptr_inters->num_cells_added;i++)
      {
        *(log_ptr++) = rrcmeas_trans_ptr->u.inter_s.add_cell[i].intersys_cell_id;
        *(log_ptr++) = rrcmeas_trans_ptr->u.inter_s.add_cell[i].measure_in_idle_pch_fach;
      }
      log_commit(log_record_ptr_inters);
    }
  }
}