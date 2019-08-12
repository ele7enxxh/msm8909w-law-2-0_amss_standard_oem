/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            IS801 Module

General Description
  This file contains implementations for TM Diag Interface
  
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/src/tm_is801.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/11   rc	   Changed mappings of IS801_RF_BAND_PCS and IS801_RF_BAND_CDMA
10/07/09   atien   Handle out of order eph/alm during MSB download
02/27/08   cl      Ignore unsupported responses to make it IS801-B compliant
06/26/07   cl      Added support for BC15
06/13/07   cl      Changes to use semi-dynamic memory allocation
02/15/07   jw      Added functionality to read the RF Delay NV items.
12/15/06   cl      If first PDDM can accommodate all PPM and PRM part.
                   Pack them all in the PDDM.
12/14/06   cl      Added req_mask and resp_mask in rev link pddm structure
12/11/06   ank     Reset fwd link pddm req_mask and resp_mask.
12/07/06   cl      Modified reverse link pddm creation to allow sending 
                   multiple rejct respon in a single PDDM
08/02/06   cl      Initail Release
===========================================================================*/

#include "gps_variation.h"
#include "tm_is801.h"
#include "tm_is801_i.h"
#include "paixlate.h"
#include "msg.h"
#include "sm_nv.h"
#include "pdapibuf.h"
#include "cai.h"
#include "sys.h" 
#include "sm_oemdata.h"

/* Data Structure Definition Section */
uint32 is801_registred_client_type = 0;
is801_client_data_s_type is801_clnt_arr[TM_IS801_MAX_CLIENT];

typedef enum
{
  IS801_PROC_FWD_LINK_RESP_RESULT_ERROR = -1,
  IS801_PROC_FWD_LINK_RESP_RESULT_NEW,
  IS801_PROC_FWD_LINK_RESP_RESULT_APPENDING,
  IS801_PROC_FWD_LINK_RESP_RESULT_IGNORE
} is801_process_fwd_link_resp_result_e_type;


/* The following are related to RF Delay values */

/* This is the highest band from which GPS can tune. */
#define IS801_MAX_SUPPORTED_BAND  CAI_MAX_BAND_CLASSES

static nv_items_enum_type is801_rf_delay_nv_items[IS801_MAX_SUPPORTED_BAND] =
{
  NV_BC0_GPS1_RF_DELAY_I,
  NV_BC1_GPS1_RF_DELAY_I,
  NV_BC1_GPS1_RF_DELAY_I, /* JTACS actually not supported */
  NV_BC3_GPS1_RF_DELAY_I,
  NV_BC4_GPS1_RF_DELAY_I,
  NV_BC5_GPS1_RF_DELAY_I,
  NV_BC6_GPS1_RF_DELAY_I,
  NV_BC6_GPS1_RF_DELAY_I,
  NV_BC6_GPS1_RF_DELAY_I,
  NV_BC6_GPS1_RF_DELAY_I,
  NV_BC10_GPS1_RF_DELAY_I,
  NV_BC11_GPS1_RF_DELAY_I,
  NV_BC11_GPS1_RF_DELAY_I,
  NV_BC11_GPS1_RF_DELAY_I,
  NV_BC14_GPS1_RF_DELAY_I,
  NV_BC15_GPS1_RF_DELAY_I
};

static int32  is801_rf_delay_calib_gps_cx1024[IS801_MAX_SUPPORTED_BAND];
static uint32 is801_rf_delay_valid_mask=0;

#define IS801_RF_DELAY_FOR_BAND(i) is801_rf_delay_calib_gps_cx1024[i]

/* Following variables holds the value stored in NV for the
   delta between the RF chain delay for CDMA-GPS RF path and
   PCS-GPS RF path. */

static int32  is801_cdma_rf_delay_calib_gpsCx1024;
static int32  is801_pcs_rf_delay_calib_gpsCx1024;

/* Fast TCal info */
static cgps_FtcalTTStruct is801_FTCalTtData;
static cgps_FtcalPpmStruct is801_FTCalPpmData;
static boolean is801_FTCalEnabled = FALSE;

#define IS801_GPS_TRANS_NO_TUNE_BAND   0xFF

#define IS801_RF_BAND_PCS   SYS_BAND_BC1               
#define IS801_RF_BAND_CDMA  SYS_BAND_BC0               




/*===========================================================================

FUNCTION is801_reset_fwd_link_pddm_queue_element

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void is801_reset_fwd_link_pddm_queue_element(is801_fwd_link_pddm_info_queue_element_s_type *fwd_link_pddm_ptr)
{
  if(fwd_link_pddm_ptr)
  {
    fwd_link_pddm_ptr->req_rd_idx  = 0;
    fwd_link_pddm_ptr->resp_rd_idx = 0;
    fwd_link_pddm_ptr->wait_mask   = 0;
    fwd_link_pddm_ptr->pddm_info.req_mask = 0;
    fwd_link_pddm_ptr->pddm_info.resp_mask = 0;

    fwd_link_pddm_ptr->alm_resp_process.curr_dr_index        = 0;
    fwd_link_pddm_ptr->alm_resp_process.part_idx             = 1;
    fwd_link_pddm_ptr->alm_resp_process.part_mask            = 0;
    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr = NULL;

    fwd_link_pddm_ptr->sa_resp_process.curr_dr_index        = 0;
    fwd_link_pddm_ptr->sa_resp_process.part_idx             = 1;
    fwd_link_pddm_ptr->sa_resp_process.part_mask            = 0;
    fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr = NULL;

    fwd_link_pddm_ptr->eph_resp_process.curr_dr_index        = 0;
    fwd_link_pddm_ptr->eph_resp_process.part_idx             = 1;
    fwd_link_pddm_ptr->eph_resp_process.part_mask            = 0;
    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr = NULL;
  }
}

/*===========================================================================

FUNCTION is801_get_rfdelay_nv_items

DESCRIPTION    This function reads the RF delay related NV items such as Delays
   of CDMA & GPS RF Chains, etc out of NV.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

============================================================================*/

static void is801_get_rfdelay_nv_items( void )
{
  int16 is801_rf_delay_gps_chain;
  int16 is801_rf_delay_cdma_chain;
  int16 is801_rf_delay_pcs_chain;

  boolean status;

  union  {
    int16 cdma_rf_delay;
    int16 pcs_rf_delay;
    int16 gps_rf_delay;
  } is801_nv_item ;

  int16 rf_delay;
  uint8 i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  is801_nv_item.gps_rf_delay = 0;

  status = sm_nv_read ( NV_GPS1_GPS_RF_DELAY_I, (void *)&is801_nv_item );

  if( !status )   /* probably not active. */
  {
    /* Set this value to 0 */
    is801_rf_delay_gps_chain = 0;
  }
  else
  {
    is801_rf_delay_gps_chain = (int)(is801_nv_item.gps_rf_delay);
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------
    Each band has an associated RF delay.
   *-------------------------------*/

  is801_rf_delay_valid_mask = 0;
  for(i=0; i<IS801_MAX_SUPPORTED_BAND; ++i)
  {
    status = sm_nv_read( is801_rf_delay_nv_items[i], &rf_delay );

    if(status)
    {
      /* The NV item is valid */
      is801_rf_delay_valid_mask |= (1<<i); /* Set Corresponding bit */

      IS801_RF_DELAY_FOR_BAND(i) = (int32) rf_delay - is801_rf_delay_gps_chain;
    }
    else
    {
      /* The NV item is not valid--probably never set */
      IS801_RF_DELAY_FOR_BAND(i) = -is801_rf_delay_gps_chain;
    }
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  is801_nv_item.cdma_rf_delay = 0;

  status = sm_nv_read ( NV_GPS1_CDMA_RF_DELAY_I, &is801_nv_item );

  if( !status )   /* probably not active. */
  {
    /* Set this value to 0 */
    is801_rf_delay_cdma_chain = 0;
  }
  else
  {
    is801_rf_delay_cdma_chain = is801_nv_item.cdma_rf_delay;
  }

  is801_cdma_rf_delay_calib_gpsCx1024 = is801_rf_delay_cdma_chain -
                                          is801_rf_delay_gps_chain;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  is801_nv_item.pcs_rf_delay = 0;

  status = sm_nv_read ( NV_GPS1_PCS_RF_DELAY_I, &is801_nv_item );

  if( !status )   /* probably not active. */
  {
    /* Set this value to 0 */
    is801_rf_delay_pcs_chain = 0;
  }
  else
  {
    is801_rf_delay_pcs_chain = is801_nv_item.pcs_rf_delay;
  }

  is801_pcs_rf_delay_calib_gpsCx1024 = is801_rf_delay_pcs_chain -
                                         is801_rf_delay_gps_chain;

} /* is801_get_rfdelay_nv_items */


/*===========================================================================

FUNCTION is801_get_rf_delay

DESCRIPTION    This function updates the band class and associated RF delay.

DEPENDENCIES   None.

RETURN VALUE   RF Delay for the band class in GPS Cx1024

SIDE EFFECTS   None.

===========================================================================*/

int32 is801_get_rf_delay
(
  uint8 band_class
)
{
  int32 rf_calib_gpsCx1024 = 0;
  boolean found_rf_delay=FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(band_class < IS801_MAX_SUPPORTED_BAND)
  {
    if( (is801_rf_delay_valid_mask & (1 << band_class)) != 0 )
    {
      /* Bit for band class is set */
      rf_calib_gpsCx1024 = IS801_RF_DELAY_FOR_BAND(band_class);

      found_rf_delay = TRUE;
    }
  }
  else if (band_class != IS801_GPS_TRANS_NO_TUNE_BAND)
  {
    MSG_ERROR("Unsupported band class %d", band_class, 0, 0);
  }

  if(found_rf_delay == FALSE)
  {
    /* Set the RF chain delay to be either CDMA or PCS */
    switch ( band_class )
    {
      case IS801_RF_BAND_PCS:
      {
        rf_calib_gpsCx1024 = is801_pcs_rf_delay_calib_gpsCx1024;
        break;
      }
      case IS801_RF_BAND_CDMA:
      default: /* Use default RF CAL as CDMA */
      {
        rf_calib_gpsCx1024 = is801_cdma_rf_delay_calib_gpsCx1024;
        break;
      }
    }
  } /* if !found_rf_delay */

  return rf_calib_gpsCx1024;

} /* is801_get_rf_delay */


/*===========================================================================

FUNCTION is801_init

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void is801_init(void)
{
  int i;
  
  for(i = 0; i < TM_IS801_MAX_CLIENT; i++)
  {
    is801_clnt_arr[i].active             = FALSE;
    is801_clnt_arr[i].prtl_type          = TM_PRTL_TYPE_NONE;    
    is801_clnt_arr[i].client_msg_cb_fp   = NULL;

    /* Initializing Forward Link Queues */  
    (void)q_init(&(is801_clnt_arr[i].fwd_link_pddm_q));

    /* Initializing Reverse Link Req/Resp Queues */
    (void)q_init(&(is801_clnt_arr[i].rev_link_req_q));

    (void)q_init(&(is801_clnt_arr[i].rev_link_resp_q));

    /* Initializing Reverse Link PDDM Queues */
    (void)q_init(&(is801_clnt_arr[i].rev_link_pddm_q));
  }  

  /* Get the various rf delay NV values */
  is801_get_rfdelay_nv_items();


#if ((defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
     (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
  #error code not present
#endif
}

/*===========================================================================

FUNCTION is801_get_clnt_arr_idx

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
int is801_get_clnt_arr_idx(tm_prtl_type prtl_type)
{
  int i;
  for(i = 0; i < TM_IS801_MAX_CLIENT; i++)
  {
    if( (is801_clnt_arr[i].active == TRUE)
      &&(is801_clnt_arr[i].prtl_type == prtl_type))
    {
      return i;
    }
  }
  return -1;
}

/*===========================================================================

FUNCTION is801_prtl_reg

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_prtl_reg(
                       tm_prtl_type              prtl_type,
                       prtl_is801_msg_cb_f_type  *msg_cb_fp
                     )
{
  int i;

  if(msg_cb_fp == NULL)
  {
    MSG_ERROR("Registration failed because of null msg_cb_fp", 0, 0,0);  
    return FALSE;
  }

  if(is801_registred_client_type & prtl_type)
  {
    MSG_HIGH("Client Already Registered", 0, 0,0);  
    return TRUE;
  }  
  
  for(i = 0; i < TM_IS801_MAX_CLIENT; i++)
  {
    if(is801_clnt_arr[i].active == FALSE)
    {
      is801_registred_client_type        |= prtl_type;
      is801_clnt_arr[i].active           = TRUE;
      is801_clnt_arr[i].prtl_type        = prtl_type;
      is801_clnt_arr[i].client_msg_cb_fp = msg_cb_fp;
      return TRUE;
    }
  }

  MSG_ERROR("Registration failed because of max # of client reached", 0, 0, 0);
  return FALSE;
}

/*===========================================================================

FUNCTION is801_xlate_fwd_req_type

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
is801_fwd_link_req_mask_type is801_xlate_fwd_req_type(pde_to_ms_req_type ext_req_type)
{
  switch(ext_req_type)
  {
    case MS_INFO_REQ_CODE:
    return IS801_FWD_LINK_REQ_MASK_MSI;
  
    case AUTO_MEAS_WGHT_FACTR_REQ_CODE:
    return IS801_FWD_LINK_REQ_MASK_AMWF;

    case PR_MEAS_REQ_CODE:
    return IS801_FWD_LINK_REQ_MASK_PRM;

    case PPM_MEAS_REQ_CODE:
    return IS801_FWD_LINK_REQ_MASK_PPM;

    case LOC_RESP_PDE_REQ_CODE:
    return IS801_FWD_LINK_REQ_MASK_LR;

    case TIME_OFFSET_MEAS_REQ_CODE:
    return IS801_FWD_LINK_REQ_MASK_TOM;

    case CANCEL_REQ_CODE:
    return IS801_FWD_LINK_REQ_MASK_CAN;

    default:
    return IS801_FWD_LINK_REQ_MASK_NONE;
  }
}

/*===========================================================================

FUNCTION is801_xlate_fwd_resp_type

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
is801_fwd_link_resp_mask_type is801_xlate_fwd_resp_type(pde_to_ms_resp_type ext_resp_type)
{
  if(ext_resp_type == (pde_to_ms_resp_type)REJ_RESP_CODE)
    return IS801_FWD_LINK_RESP_MASK_REJ;
  
  switch(ext_resp_type)
  {
    case BS_CAP_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_BSC;

    case GPS_AA_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_AA;

    case GPS_LA_S_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_LASC;

    case GPS_LA_C_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_LACC;

    case GPS_SA_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_SA;

    case BS_ALM_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_BSA;

    case GPS_ALM_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_ALM;

    case GPS_EPH_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_EPH;

    case GPS_NAV_MSG_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_NMB;

    case LOC_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_LR;

    case GPS_ALM_COR_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_AC;

    case GPS_SAT_HLTH_RESP_CODE:
    return IS801_FWD_LINK_RESP_MASK_SHI;

    default:
    return IS801_FWD_LINK_RESP_MASK_NONE;
  
  }
}


/*===========================================================================

FUNCTION is801_process_eph_resp

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
is801_process_fwd_link_resp_result_e_type is801_process_eph_resp
(
  is801_fwd_link_pddm_info_queue_element_s_type  *fwd_link_pddm_ptr,
  is801_fwd_link_resp_part_s_type                *resp_part_ptr
)
{
  int i;
  byte resp_rd_indx;
  byte curr_dr_index = 0;
  is801_eph_part_type *eph_part_ptr;
  is801_process_fwd_link_resp_result_e_type retVal = IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;

  if((fwd_link_pddm_ptr == NULL) || (resp_part_ptr == NULL))
  {
    MSG_ERROR( "null resp_part_ptr", 0, 0, 0);
    return IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;
  }
   
  eph_part_ptr = (is801_eph_part_type *)&(resp_part_ptr->resp_msg_payload.eph_part);


  /* Obtain a buffer to construct this response in */

  if (NULL == fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr) 
  {
    resp_rd_indx = fwd_link_pddm_ptr->resp_rd_idx;

    if(resp_rd_indx < MAX_RESP_PER_PDDM)
    {
      fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr = &(fwd_link_pddm_ptr->pddm_info.resp_rd[resp_rd_indx]);    
      /* Initialize EPH parts info.
      */
      fwd_link_pddm_ptr->eph_resp_process.curr_dr_index = 0;
      /* Set EPH bit mask based on its total part value. E.g., if
      ** EPH has 6 parts, its bit mask is set to have 0x0011 1111.
      */
      fwd_link_pddm_ptr->eph_resp_process.part_mask = 0xffffffff >> (MAX_IS801_EPH_PARTS - eph_part_ptr->fix.u_Total_Parts);

      retVal = IS801_PROC_FWD_LINK_RESP_RESULT_NEW;
    }
    else
    {
      MSG_ERROR( "resp_rd_indx out of bound", 0, 0, 0);
      return IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;      
    }
  }
  else
  {
    retVal = IS801_PROC_FWD_LINK_RESP_RESULT_APPENDING;  
  }

  /* The current algorithm is to drop out-of-sequence EPH data and wait
  ** until the correct part arrives (regardless whether it is from the
  ** same message or not */
  if(eph_part_ptr->fix.u_Part_Num != fwd_link_pddm_ptr->eph_resp_process.part_idx)
  {
    MSG_ERROR("Out-of-sequence EPH data recvd! %d", eph_part_ptr->fix.u_Part_Num, 0, 0);
    // return retVal;
  }
  
  if ( !(fwd_link_pddm_ptr->eph_resp_process.part_mask & (0x1 << (eph_part_ptr->fix.u_Part_Num - 1))) )
  {
    /* Don't process duplicate EPF data.
    */
    MSG_ERROR("Recvd duplicate EPH: %d", eph_part_ptr->fix.u_Part_Num, 0, 0);
    return retVal;
  }

  /* May need to initialize the resp_construct element timestamp */
  if (1 == eph_part_ptr->fix.u_Part_Num) 
  {
    qw_equ( fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.timestamp, eph_part_ptr->timestamp);
    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_type   = resp_part_ptr->resp_type;
    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->unsolicited = resp_part_ptr->unsolicited;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_SatIndex = 0;
    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_AB_Par_Incl = eph_part_ptr->fix.u_AB_Par_Incl;

    if (TRUE == eph_part_ptr->fix.u_AB_Par_Incl)
    {
      fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_Alpha_0 = eph_part_ptr->fix.u_Alpha_0;
      fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_Alpha_1 = eph_part_ptr->fix.u_Alpha_1;
      fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_Alpha_2 = eph_part_ptr->fix.u_Alpha_2;
      fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_Alpha_3 = eph_part_ptr->fix.u_Alpha_3;

      fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_Beta_0 = eph_part_ptr->fix.u_Beta_0;
      fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_Beta_1 = eph_part_ptr->fix.u_Beta_1;
      fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_Beta_2 = eph_part_ptr->fix.u_Beta_2;
      fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_Beta_3 = eph_part_ptr->fix.u_Beta_3;
    }
  }

  curr_dr_index = fwd_link_pddm_ptr->eph_resp_process.curr_dr_index;

  /* Copy the data records out */
  for (i=0; i < eph_part_ptr->fix.u_TotalSats; i++) 
  {
    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].u_Sv_Prn_Num = 
       eph_part_ptr->var[i].u_Sv_Prn_Num;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].u_Iode = 
       eph_part_ptr->var[i].u_Iode;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_C_Rs = 
       eph_part_ptr->var[i].x_C_Rs;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_Delta_N = 
      eph_part_ptr->var[i].x_Delta_N;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].l_M0 = 
       eph_part_ptr->var[i].l_M0;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_C_Uc = 
       eph_part_ptr->var[i].x_C_Uc;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].l_Eccentricity = 
       eph_part_ptr->var[i].l_Eccentricity;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_C_Us = 
       eph_part_ptr->var[i].x_C_Us;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].l_A_Sqrt = 
       eph_part_ptr->var[i].l_A_Sqrt;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_Toe = 
       eph_part_ptr->var[i].x_Toe;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_C_Ic = 
       eph_part_ptr->var[i].x_C_Ic;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].l_Omega_0 = 
       eph_part_ptr->var[i].l_Omega_0;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_C_Is = 
       eph_part_ptr->var[i].x_C_Is;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].l_Angle_Inclination = 
       eph_part_ptr->var[i].l_Angle_Inclination;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_C_Rc = 
       eph_part_ptr->var[i].x_C_Rc;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].l_Omega = 
       eph_part_ptr->var[i].l_Omega;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].l_Omegadot = 
       eph_part_ptr->var[i].l_Omegadot;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_Idot = 
       eph_part_ptr->var[i].x_Idot;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].x_Toc = 
       eph_part_ptr->var[i].x_Toc;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].b_Af2 = 
       eph_part_ptr->var[i].b_Af2;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].b_Af1 = 
       eph_part_ptr->var[i].b_Af1;

    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.var[curr_dr_index].l_Af0 = 
       eph_part_ptr->var[i].l_Af0;

    curr_dr_index++;
  }

  fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr->resp_msg_payload.eph_resp.fix.u_TotalSats = curr_dr_index;
  fwd_link_pddm_ptr->eph_resp_process.curr_dr_index = curr_dr_index;  
  fwd_link_pddm_ptr->eph_resp_process.part_idx++;  

  /* Modify EPH bit mask, chaning the bit corresponding to the part_num
  ** in the bit mask from '1' to '0'. E.g., if we receive the second part first
  ** of the total 6 parts, we change the mask from '0x00111111' to '0x00111101'.
  */
  fwd_link_pddm_ptr->eph_resp_process.part_mask &= ~(0x1 << (eph_part_ptr->fix.u_Part_Num - 1)); /*lint !e502 */

  if (!fwd_link_pddm_ptr->eph_resp_process.part_mask) 
  {
    fwd_link_pddm_ptr->eph_resp_process.part_resp_holder_ptr = NULL;
    fwd_link_pddm_ptr->eph_resp_process.part_idx = 1;
    fwd_link_pddm_ptr->eph_resp_process.part_mask = 0;
    fwd_link_pddm_ptr->eph_resp_process.curr_dr_index = 0;

    fwd_link_pddm_ptr->wait_mask &= ~IS801_FWD_LINK_WAIT_MASK_EPH;
  }
  else
  {
    fwd_link_pddm_ptr->wait_mask |= IS801_FWD_LINK_WAIT_MASK_EPH;   
  }

  return retVal;  
}

/*===========================================================================

FUNCTION is801_process_alm_resp

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
is801_process_fwd_link_resp_result_e_type is801_process_alm_resp
(
  is801_fwd_link_pddm_info_queue_element_s_type  *fwd_link_pddm_ptr,
  is801_fwd_link_resp_part_s_type                *resp_part_ptr
)
{
  int i;
  byte resp_rd_indx;
  byte curr_dr_index = 0;
  is801_alm_part_type *alm_part_ptr;
  is801_process_fwd_link_resp_result_e_type retVal = IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;

  if((fwd_link_pddm_ptr == NULL) || (resp_part_ptr == NULL))
  {
    MSG_ERROR( "null resp_part_ptr", 0, 0, 0);
    return IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;
  }
   
  alm_part_ptr = (is801_alm_part_type *)&(resp_part_ptr->resp_msg_payload.alm_part);
  

  /* Obtain a buffer to construct this response in */

  if (NULL == fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr) 
  {
    resp_rd_indx = fwd_link_pddm_ptr->resp_rd_idx;

    if(resp_rd_indx < MAX_RESP_PER_PDDM)
    {
      fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr = &(fwd_link_pddm_ptr->pddm_info.resp_rd[resp_rd_indx]);    
      /* Initialize ALM parts info.
      */
      fwd_link_pddm_ptr->alm_resp_process.curr_dr_index = 0;
      /* Set ALM bit mask based on its total part value. E.g., if
      ** ALM has 6 parts, its bit mask is set to have 0x0011 1111.
      */
      fwd_link_pddm_ptr->alm_resp_process.part_mask = 0xff >> (MAX_IS801_ALM_PARTS - alm_part_ptr->fix.u_Total_Parts);

      retVal = IS801_PROC_FWD_LINK_RESP_RESULT_NEW;
    }
    else
    {
      MSG_ERROR( "resp_rd_indx out of bound", 0, 0, 0);
      return IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;      
    }
  }
  else
  {
    retVal = IS801_PROC_FWD_LINK_RESP_RESULT_APPENDING;  
  }
  

  /* The current algorithm is to drop out-of-sequence ALM data and wait
  ** until the correct part arrives (regardless whether it is from the
  ** same message or not */
  if(alm_part_ptr->fix.u_Part_Num != fwd_link_pddm_ptr->alm_resp_process.part_idx)
  {
    MSG_ERROR("Out-of-sequence ALM data recvd! %d", alm_part_ptr->fix.u_Part_Num, 0, 0);
    // return retVal;
  }  

  if ( !(fwd_link_pddm_ptr->alm_resp_process.part_mask & (0x1 << (alm_part_ptr->fix.u_Part_Num - 1))) )
  {
    /* Don't process duplicate ALM data.
    */
    MSG_ERROR("Recvd duplicate ALM: %d", alm_part_ptr->fix.u_Part_Num, 0, 0);
    return retVal;
  }

  /* May need to initialize the resp_construct element timestamp */
  if (1 == alm_part_ptr->fix.u_Part_Num) 
  {
    qw_equ( fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.timestamp, alm_part_ptr->timestamp);
    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_type   = resp_part_ptr->resp_type;
    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->unsolicited = resp_part_ptr->unsolicited;
  }

  curr_dr_index = fwd_link_pddm_ptr->alm_resp_process.curr_dr_index;
  
  /* Copy the data records out */
  for (i=0; i < alm_part_ptr->fix.u_TotalSats; i++) 
  {
    /* The Toa and Weeknum might be different for each part, so
    ** let's save it also.
    */
    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].u_Toa 
      = alm_part_ptr->fix.u_Toa;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].u_WeekNum
      = alm_part_ptr->fix.u_WeekNum;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].u_Sv_Prn_Num 
      = alm_part_ptr->var[i].u_Sv_Prn_Num;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].w_DeltaI
      = alm_part_ptr->var[i].w_DeltaI;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].q_MZero
      = alm_part_ptr->var[i].q_MZero;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].w_E
      = alm_part_ptr->var[i].w_E;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].q_SqrtA
      = alm_part_ptr->var[i].q_SqrtA;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].q_OmegaZero
      = alm_part_ptr->var[i].q_OmegaZero;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].q_Omega
      = alm_part_ptr->var[i].q_Omega;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].w_OmegaDot
      = alm_part_ptr->var[i].w_OmegaDot;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].w_Af1
      = alm_part_ptr->var[i].w_Af1;

    fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.var[curr_dr_index].w_Af0
      = alm_part_ptr->var[i].w_Af0;

    curr_dr_index++;
  }

  fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr->resp_msg_payload.alm_resp.fix.u_TotalSats = curr_dr_index;
  fwd_link_pddm_ptr->alm_resp_process.curr_dr_index = curr_dr_index;  
  fwd_link_pddm_ptr->alm_resp_process.part_idx++;

  /* Modify ALM bit mask, chaning the bit corresponding to the part_num
  ** in the bit mask from '1' to '0'. E.g., if we receive the second part first
  ** of the total 6 parts, we change the mask from '0x00111111' to '0x00111101'.
  */
  fwd_link_pddm_ptr->alm_resp_process.part_mask &= ~(0x1 << (alm_part_ptr->fix.u_Part_Num - 1)); /*lint !e502 */

   if (!fwd_link_pddm_ptr->alm_resp_process.part_mask) 
   {
     fwd_link_pddm_ptr->alm_resp_process.part_resp_holder_ptr = NULL;
     fwd_link_pddm_ptr->alm_resp_process.part_idx = 1;
     fwd_link_pddm_ptr->alm_resp_process.part_mask = 0;
     fwd_link_pddm_ptr->alm_resp_process.curr_dr_index = 0;

     fwd_link_pddm_ptr->wait_mask &= ~IS801_FWD_LINK_WAIT_MASK_ALM;
   }
   else
   {
     fwd_link_pddm_ptr->wait_mask |= IS801_FWD_LINK_WAIT_MASK_ALM;   
   }

   return retVal;  
}



/*===========================================================================

FUNCTION is801_process_sa_resp

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
is801_process_fwd_link_resp_result_e_type is801_process_sa_resp
(
  is801_fwd_link_pddm_info_queue_element_s_type  *fwd_link_pddm_ptr, 
  is801_fwd_link_resp_part_s_type                *resp_part_ptr
)
{
  int i;
  int j;
  byte resp_rd_indx;
  byte curr_dr_index = 0;
  is801_sa_part_type *sa_part_ptr;
  is801_process_fwd_link_resp_result_e_type retVal = IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;

  if((fwd_link_pddm_ptr == NULL) || (resp_part_ptr == NULL))
  {
    MSG_ERROR( "null resp_part_ptr", 0, 0, 0);
    return IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;  
  }

  sa_part_ptr = (is801_sa_part_type *)&(resp_part_ptr->resp_msg_payload.sa_part);
  
  /* Obtain a buffer to construct this response in */
  if (NULL == fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr)
  {
    resp_rd_indx = fwd_link_pddm_ptr->resp_rd_idx;

    if(resp_rd_indx < MAX_RESP_PER_PDDM)
    {
      fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr = &(fwd_link_pddm_ptr->pddm_info.resp_rd[resp_rd_indx]);    
      /* Initialize SA parts info.
      */
      fwd_link_pddm_ptr->sa_resp_process.curr_dr_index = 0;
      /* Set SA bit mask based on its total part value. E.g., if
      ** SA has 6 parts, its bit mask is set to have 0x0011 1111.
      */
      fwd_link_pddm_ptr->sa_resp_process.part_mask = 0xff >> (MAX_IS801_SA_PARTS - sa_part_ptr->fix.total_parts);

      retVal = IS801_PROC_FWD_LINK_RESP_RESULT_NEW;
    }
    else
    {
      MSG_ERROR( "resp_rd_indx out of bound", 0, 0, 0);
      return IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;      
    }   
  }
  else
  {
    retVal = IS801_PROC_FWD_LINK_RESP_RESULT_APPENDING;  
  }

  /* The current algorithm is to drop out-of-sequence SA data and wait
  ** until the right SA part arrives (regardless if it is from the same
  ** message or not. It also assumes that this function is called under
  ** the context of the same task. */
  if (sa_part_ptr->fix.part_num != fwd_link_pddm_ptr->sa_resp_process.part_idx)
  {
    MSG_ERROR("OoO SA data recvd! %d instead of %d", sa_part_ptr->fix.part_num, fwd_link_pddm_ptr->sa_resp_process.part_idx, 0);
    return retVal;  
  }  

  if ( !(fwd_link_pddm_ptr->sa_resp_process.part_mask & (0x1 << (sa_part_ptr->fix.part_num - 1))) )
  {
    /* Don't process duplicate SA data.
    */
    MSG_ERROR("Recvd duplicate SA: %d", sa_part_ptr->fix.part_num, 0, 0);
    return retVal;
  }  

  /* May need to initialize the resp_construct element timestamp */
  if (1 == sa_part_ptr->fix.part_num) 
  {
    qw_equ( fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_msg_payload.sa_resp.timestamp, sa_part_ptr->timestamp);

    fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_type   = resp_part_ptr->resp_type;
    fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->unsolicited = resp_part_ptr->unsolicited;
  }

  curr_dr_index = fwd_link_pddm_ptr->sa_resp_process.curr_dr_index;

  /* Copy the data records out */
  for (i=0; i < sa_part_ptr->fix.num_dr_p; i++) 
  {
    fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_msg_payload.sa_resp.data[curr_dr_index].dr_size
    = sa_part_ptr->fix.dr_size;

    fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_msg_payload.sa_resp.data[curr_dr_index].ref_bit_num
    = sa_part_ptr->fix.ref_bit_num;

    memscpy((void*)(fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_msg_payload.sa_resp.data[curr_dr_index].nav_msg_bits), 
		   sizeof(fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_msg_payload.sa_resp.data[curr_dr_index].nav_msg_bits),
           (const void*)(sa_part_ptr->var[i].nav_msg_bits), 
           DR_SIZE_IN_BYTES(sa_part_ptr->fix.dr_size));
          
    fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_msg_payload.sa_resp.data[curr_dr_index].num_sv_dr 
    = sa_part_ptr->var[i].num_sv_dr;

    for (j=0; j < sa_part_ptr->var[i].num_sv_dr; j++) 
    {
      fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_msg_payload.sa_resp.data[curr_dr_index].sv_prn_num[j] 
      = sa_part_ptr->var[i].sv_prn_num[j];
    }
    curr_dr_index++;
  }

  fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_msg_payload.sa_resp.num_data_records
  = curr_dr_index;

  fwd_link_pddm_ptr->sa_resp_process.curr_dr_index = curr_dr_index;
   
  fwd_link_pddm_ptr->sa_resp_process.part_idx++;

  /* Modify SA bit mask, chaning the bit corresponding to the part_num
  ** in the bit mask from '1' to '0'. E.g., if we receive the second part first
  ** of the total 6 parts, we change the mask from '0x00111111' to '0x00111101'.
  */
  fwd_link_pddm_ptr->sa_resp_process.part_mask &= ~(0x1 << (sa_part_ptr->fix.part_num - 1)); /*lint !e502 */
   
  if (!fwd_link_pddm_ptr->sa_resp_process.part_mask) 
  {
    if (oemData_IsDataTypeOn(OEMDATA_SENSTIVITY_ASSIST_DATA))
    {
      /* SA records complete, dump to OEM layer */
      (void)oemData_SendDataType(OEMDATA_SENSTIVITY_ASSIST_DATA,
                         (void *)&(fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr->resp_msg_payload.sa_resp));
    }

    fwd_link_pddm_ptr->sa_resp_process.part_resp_holder_ptr = NULL;
    fwd_link_pddm_ptr->sa_resp_process.part_idx = 1;
    fwd_link_pddm_ptr->sa_resp_process.part_mask = 0;
    fwd_link_pddm_ptr->sa_resp_process.curr_dr_index = 0;

    fwd_link_pddm_ptr->wait_mask &= ~IS801_FWD_LINK_WAIT_MASK_SA;
  }
  else
  {
    fwd_link_pddm_ptr->wait_mask |= IS801_FWD_LINK_WAIT_MASK_SA;   
  }

  return retVal;  
}

/*===========================================================================

FUNCTION is801_process_fwd_link_resp

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
is801_process_fwd_link_resp_result_e_type is801_process_fwd_link_resp
(
  is801_fwd_link_pddm_info_queue_element_s_type  *fwd_link_pddm_ptr,
  is801_fwd_link_resp_part_s_type                *resp_part_ptr
)
{
  byte*  resp_rd_indx;
  is801_process_fwd_link_resp_result_e_type retVal = IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;
  
  if((resp_part_ptr == NULL) || (fwd_link_pddm_ptr == NULL))
  {
    MSG_ERROR( "NULL resp_part_ptr", 0, 0, 0);
    return IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;  
  }

  resp_rd_indx = &(fwd_link_pddm_ptr->resp_rd_idx);

  switch(resp_part_ptr->resp_type)
  { 
    /* REJ/AA/LR will come with complete message */
    case IS801_FWD_LINK_RESP_MASK_REJ:  
    case IS801_FWD_LINK_RESP_MASK_AA:
    case IS801_FWD_LINK_RESP_MASK_LR:
	  if (*resp_rd_indx >= MAX_RESP_PER_PDDM)
	  {
	    MSG_ERROR( "resp_rd_indx overflows array pddm_info.resp_rd", 0, 0, 0);
		return IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;
	  }	
    
      memscpy((void *)&(fwd_link_pddm_ptr->pddm_info.resp_rd[*resp_rd_indx]), sizeof(fwd_link_pddm_ptr->pddm_info.resp_rd[*resp_rd_indx]),
	  	      (void *)resp_part_ptr, sizeof(*resp_part_ptr));
      retVal = IS801_PROC_FWD_LINK_RESP_RESULT_NEW;
      break;

    case IS801_FWD_LINK_RESP_MASK_SA:
      MSG_MED("Processing SA Resp", 0, 0, 0);
      retVal = is801_process_sa_resp(fwd_link_pddm_ptr, resp_part_ptr);
    break;

    case IS801_FWD_LINK_RESP_MASK_ALM:
      MSG_MED("Processing ALM Resp", 0, 0, 0);
      retVal = is801_process_alm_resp(fwd_link_pddm_ptr, resp_part_ptr);
    break;

    case IS801_FWD_LINK_RESP_MASK_EPH:
      MSG_MED("Processing EPH Resp", 0, 0, 0);
      retVal = is801_process_eph_resp(fwd_link_pddm_ptr, resp_part_ptr);
    break; 

    /* IS801-B handshake compliant */
    case  IS801_FWD_LINK_RESP_MASK_BSC:
    case  IS801_FWD_LINK_RESP_MASK_LASC:
    case  IS801_FWD_LINK_RESP_MASK_LACC:
    case  IS801_FWD_LINK_RESP_MASK_BSA:
    case  IS801_FWD_LINK_RESP_MASK_NMB:
    case  IS801_FWD_LINK_RESP_MASK_AC:
    case  IS801_FWD_LINK_RESP_MASK_SHI:
      MSG_MED("Response %d not supported, ignore it", resp_part_ptr->resp_type, 0, 0);
      retVal = IS801_PROC_FWD_LINK_RESP_RESULT_IGNORE;
     break;
  }    

  if (resp_part_ptr->resp_type == IS801_FWD_LINK_RESP_MASK_AA)
  {
    if (oemData_IsDataTypeOn(OEMDATA_ACQUISITION_ASSIST_DATA))
    {
      (void)oemData_SendDataType(OEMDATA_ACQUISITION_ASSIST_DATA,
                               (void *)&(resp_part_ptr->resp_msg_payload.aa_resp));
    }
  }

  return retVal;
}


/*===========================================================================

FUNCTION      is801_queue_check_sess_tag

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static int32 is801_queue_check_sess_tag( void *p_Item, void *p_CompareValue )
{
  is801_fwd_link_pddm_info_queue_element_s_type *fwd_link_pddm_ptr;
  byte  sess_tag;
  
  if((p_Item == NULL) || (p_CompareValue == NULL))
  {
    return -1;
  }
  
  fwd_link_pddm_ptr = (is801_fwd_link_pddm_info_queue_element_s_type *)p_Item;
  sess_tag = *(( byte *)p_CompareValue);

  if(fwd_link_pddm_ptr->pddm_info.hdr.sess_tag == sess_tag)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}



/*===========================================================================

FUNCTION is801_get_fwd_link_pddm_elem

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
is801_fwd_link_pddm_info_queue_element_s_type* is801_get_fwd_link_pddm_elem
(
  int                   clnt_arr_idx,
  is801_pddm_hdr_type   pddm_rec_hdr
)
{
  is801_fwd_link_pddm_info_queue_element_s_type *fwd_link_pddm_ptr = NULL;
  
  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((clnt_arr_idx < 0) || (clnt_arr_idx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "Invalid client array index", 0, 0, 0);
    return NULL;  
  }

  /* Check if this is a pddm for unfinished responses
     If it is, we should already have pddm in the queue which has the same see tag */
  fwd_link_pddm_ptr = (is801_fwd_link_pddm_info_queue_element_s_type *)q_linear_search(&(is801_clnt_arr[clnt_arr_idx].fwd_link_pddm_q), 
                                                                                       (q_compare_func_type)is801_queue_check_sess_tag,
                                                                                       (void *)&(pddm_rec_hdr.sess_tag));
  if(fwd_link_pddm_ptr != NULL)
  {
     /* Take it out from the queue for further processing */
    q_delete(&(is801_clnt_arr[clnt_arr_idx].fwd_link_pddm_q), &(fwd_link_pddm_ptr->link));
  }
  else
  {
    /* This is a new PDDM, so allocate for it */
    fwd_link_pddm_ptr = (is801_fwd_link_pddm_info_queue_element_s_type *)pdsm_getbuf(sizeof(is801_fwd_link_pddm_info_queue_element_s_type));

    if(fwd_link_pddm_ptr)
    {
      is801_reset_fwd_link_pddm_queue_element(fwd_link_pddm_ptr);
      
      /* Populate header information to fwd_link_pddm_ptr */
      fwd_link_pddm_ptr->pddm_info.hdr = pddm_rec_hdr;
    }
    else
    {
      MSG_ERROR("Unable to get buffer for fwd_link_pddm of size (%d)", 
                 sizeof(is801_fwd_link_pddm_info_queue_element_s_type), 0, 0);
    }
  }
  
  return fwd_link_pddm_ptr;
}

/*===========================================================================

FUNCTION is801_decode_fwd_link_req

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static int is801_decode_fwd_link_req
(
  byte          *start,                                                /* pointer to the start of packed PDDM buffer */
  byte          starting_offset,                                       /* Where we should be start in the PDDM buffer */
  is801_pddm_hdr_type                             pddm_rec_hdr,        /* PDDM header info */
  is801_fwd_link_pddm_info_queue_element_s_type  *fwd_link_pddm_ptr,   /* Pointer for storing unpacked PDDM data */
  uint16        length
)
{
  paii_gen_req_hdr     pddm_req_hdr;    /* Holds a received request header */
  int                  offset = starting_offset;
  int                  i;
  byte                 *req_rd_indx;

  if((start == NULL) || (fwd_link_pddm_ptr == NULL))
  {
    MSG_ERROR( "null input pointer", 0, 0, 0);
    return -1;
  }

  req_rd_indx = &(fwd_link_pddm_ptr->req_rd_idx);

  MSG_MED( "Processing %d reqs.", pddm_rec_hdr.num_requests, 0, 0);

  for (i = 0; i < pddm_rec_hdr.num_requests; i++) 
  {
    /* Decode Request Header */
    if(offset < length &&  
       paix_xlate_ext_req_header(&(start[offset]), &pddm_req_hdr, 
                                  (length - offset)))
    {
      offset += REQ_HDR_LEN;

      if(*req_rd_indx < MAX_REQ_PER_PDDM)
      {
        fwd_link_pddm_ptr->pddm_info.req_rd[*req_rd_indx].request_type = is801_xlate_fwd_req_type((pde_to_ms_req_type)pddm_req_hdr.req_type);
        fwd_link_pddm_ptr->pddm_info.req_mask |= fwd_link_pddm_ptr->pddm_info.req_rd[*req_rd_indx].request_type;

        if(offset <= length && 
           paix_xlate_ext_req_payload(&(start[offset]), &(fwd_link_pddm_ptr->pddm_info.req_rd[*req_rd_indx]), 
                                       (length - offset)))
        {
          offset += pddm_req_hdr.req_par_len;         
          (*req_rd_indx)++;
        }
        else
        {
          MSG_ERROR( "Error Decoding Request Payload", 0, 0, 0);
          return -1;
        }
      }
      else
      {
        MSG_ERROR( "Exceeded max number of request", 0, 0, 0);
        return -1;
      }
    }
    else
    {
      MSG_ERROR( "Error Decoding Request Header", 0, 0, 0);
      return -1;
    }
  }

  return (offset - starting_offset);
}

/*===========================================================================

FUNCTION is801_decode_fwd_link_resp

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
int is801_decode_fwd_link_resp
(
  byte          *start,                                                /* pointer to the start of packed PDDM buffer */
  byte          starting_offset,                                       /* Where we should be start in the PDDM buffer */
  is801_pddm_hdr_type                             pddm_rec_hdr,        /* PDDM header info */
  is801_fwd_link_pddm_info_queue_element_s_type  *fwd_link_pddm_ptr,   /* Pointer for storing unpacked PDDM data */
  uint16        length                                                 /* length of packed PDDM data */
)
{
  paii_gen_resp_hdr    pddm_resp_hdr;    /* Holds a received response header */
  int                  offset = starting_offset;
  int                  i;
  byte                 *resp_rd_indx;
  is801_fwd_link_resp_part_s_type fwd_link_resp_part;  
  is801_process_fwd_link_resp_result_e_type proc_result = IS801_PROC_FWD_LINK_RESP_RESULT_ERROR;

  if((start == NULL) || (fwd_link_pddm_ptr == NULL))
  {
    MSG_ERROR( "null input pointer", 0, 0, 0);
    return -1;
  }

  resp_rd_indx = &(fwd_link_pddm_ptr->resp_rd_idx);

  MSG_MED( "Processing %d resp.", pddm_rec_hdr.num_responses, 0, 0);  

  for (i = 0; i < pddm_rec_hdr.num_responses; i++) 
  {
    /* Decode Resp Header */
    if(offset < length &&  
       paix_xlate_ext_resp_header(&(start[offset]), &pddm_resp_hdr, 
                                   (length - offset)))
    {
      offset += RESP_HDR_LEN;

      if(*resp_rd_indx < MAX_RESP_PER_PDDM)
      {
        /* Fill up response info */
        fwd_link_resp_part.resp_type   = is801_xlate_fwd_resp_type((pde_to_ms_resp_type)pddm_resp_hdr.resp_type);
        fwd_link_resp_part.unsolicited = pddm_resp_hdr.unsol_resp;
        fwd_link_pddm_ptr->pddm_info.resp_mask |= fwd_link_resp_part.resp_type;

        if(offset < length &&
           paix_xlate_ext_resp_payload(&(start[offset]), &fwd_link_resp_part, pddm_resp_hdr.resp_par_len, (length - offset)))
        {
          proc_result = is801_process_fwd_link_resp(fwd_link_pddm_ptr, &fwd_link_resp_part);
          
          if(proc_result == IS801_PROC_FWD_LINK_RESP_RESULT_NEW)
          {
            offset += pddm_resp_hdr.resp_par_len;      
            (*resp_rd_indx)++;
          }
          else if(proc_result == IS801_PROC_FWD_LINK_RESP_RESULT_APPENDING)
          { 
            /* This is continuation of SA/ALM/EPH which we recived earlier.
               Therefore, don't increse resp_rd_indx.
            */
            offset += pddm_resp_hdr.resp_par_len;
          }
          /* IS801-B handshake compliant */
          else if(proc_result == IS801_PROC_FWD_LINK_RESP_RESULT_IGNORE)
          {
            fwd_link_pddm_ptr->pddm_info.resp_mask &= ~fwd_link_resp_part.resp_type;
            offset += pddm_resp_hdr.resp_par_len;
          }
          else
          {
            MSG_ERROR( "Error When Assembling Fwd Link Response Part", 0, 0, 0);
            return -1;
          }          
        }
        else
        {
          MSG_ERROR( "Error Decoding Response Payload", 0, 0, 0);
          return -1;
        }
      }
      else
      {
        MSG_ERROR( "Exceeded Max Number of Response", 0, 0, 0);
        return -1;
      }
    }
    else
    {
      MSG_ERROR( "Error Decoding Response Header", 0, 0, 0);
      return -1;
    }
  }

  return (offset - starting_offset);
}

/*===========================================================================

FUNCTION is801_notify_clnt_fwd_link_pddm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void is801_notify_clnt_fwd_link_pddm(byte clnt_arr_idx)
{
  byte num_pddm = 0;
  is801_fwd_pddm_info_s_type pddm_info;
  is801_fwd_link_pddm_info_queue_element_s_type *item_ptr = NULL;

  item_ptr = (is801_fwd_link_pddm_info_queue_element_s_type *)q_check(&(is801_clnt_arr[clnt_arr_idx].fwd_link_pddm_q));

  while( item_ptr != NULL )
  {
    if(item_ptr->wait_mask == IS801_FWD_LINK_WAIT_MASK_NONE)
    {
      num_pddm++;
    }
    item_ptr = (is801_fwd_link_pddm_info_queue_element_s_type *)q_next(&(is801_clnt_arr[clnt_arr_idx].fwd_link_pddm_q), &item_ptr->link );
  } /* END while traversing the queue */

  if(num_pddm > 0)
  {
    pddm_info.num_pddm = num_pddm;
    (void)is801_clnt_arr[clnt_arr_idx].client_msg_cb_fp(pddm_info);
  }
}

/*===========================================================================

FUNCTION is801_decode_fwd_link_pddm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_decode_fwd_link_pddm
(
                                 tm_prtl_type  prtl_type,
                                 byte          *start,
                                 uint16        length,
                                 byte          *offset
                               )                                   
{
  int                                            clnt_arr_idx = -1;
  is801_pddm_hdr_type                            pddm_rec_hdr;                 /* Holds received PDDM header info */
  is801_fwd_link_pddm_info_queue_element_s_type  *fwd_link_pddm_ptr = NULL;
  int                                            processed_length = -1;

  boolean retVal = TRUE;

  *offset       = 0;
  clnt_arr_idx = is801_get_clnt_arr_idx(prtl_type);

  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((clnt_arr_idx < 0) || (clnt_arr_idx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "No protocol match found", 0, 0, 0);
    return FALSE;  
  }
  
  if (NULL == start || length < MIN_PDDM_LENGTH) 
  {
    MSG_ERROR( "IS801_PROCESS_PKT:  Invalid input params.", 0, 0, 0);
    return FALSE;
  }

  /* Decode PDDM header first */
  if(paix_xlate_ext_pddm_hdr(start, &pddm_rec_hdr))
  {  
    *offset += PDDM_HDR_LEN;
   
    /* Get a Foward Link PDDM queue element to hold unpacked PDDM */
    fwd_link_pddm_ptr = is801_get_fwd_link_pddm_elem(clnt_arr_idx, pddm_rec_hdr);

    if(fwd_link_pddm_ptr)
    {
      /* Now decode Request, if any */
      if(pddm_rec_hdr.num_requests)
      {
        processed_length = is801_decode_fwd_link_req(start, *offset, pddm_rec_hdr, fwd_link_pddm_ptr, length);

        if(processed_length > 0) 
        {
          *offset += processed_length;
          retVal = TRUE;
        }
        else
        {
          MSG_ERROR( "Error When decoding Fwd Link Req", 0, 0, 0);  
          retVal = FALSE;
        }
      }
      
      /* Process Response only when requests are decoded correctly */
      if((retVal == TRUE) && (pddm_rec_hdr.num_responses))
      {
        processed_length = is801_decode_fwd_link_resp(start, *offset, pddm_rec_hdr, fwd_link_pddm_ptr, length);

        if(processed_length > 0)
        {
          *offset += processed_length;
          retVal = TRUE;
        }
        else
        {
          MSG_ERROR( "Error When decoding Fwd Link Resp", 0, 0, 0);  
          retVal = FALSE;
        }
      } 
    }
    else
    {
      MSG_ERROR( "PDDM Queue Full", 0, 0, 0);  
      retVal = FALSE;
    }
  }
  else
  {
    MSG_ERROR( "Error when decoding PDDM header", 0, 0, 0);  
    retVal = FALSE;
  }

  if(retVal == TRUE)
  {
    /* Now put fwd_link_pddm_ptr back to fwd link pddm queue
    If there is any complete PDDM, notify clients */  
    q_put(&(is801_clnt_arr[clnt_arr_idx].fwd_link_pddm_q), &(fwd_link_pddm_ptr->link));

    /* Notify Client, if needed */
    is801_notify_clnt_fwd_link_pddm(clnt_arr_idx);
  }
  else
  {
    if(fwd_link_pddm_ptr)
    {
      /* Reset all the fields and put it back on fwd link req free queue */
      is801_reset_fwd_link_pddm_queue_element(fwd_link_pddm_ptr);
      (void)pdsm_freebuf((char *)fwd_link_pddm_ptr);
    }
  }
  return retVal;
}


/*===========================================================================

FUNCTION is801_get_fwd_link_pddm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_get_fwd_link_pddm
(
  tm_prtl_type                     prtl_type,
  is801_fwd_link_pddm_info_s_type  *clnt_fwd_link_pddm_buf_ptr
)
{
  int                                            clnt_arr_idx = -1;
  is801_fwd_link_pddm_info_queue_element_s_type  *fwd_link_pddm_ptr = NULL;
  
  clnt_arr_idx = is801_get_clnt_arr_idx(prtl_type);

  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((clnt_arr_idx < 0) || (clnt_arr_idx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "No protocol match found", 0, 0, 0);
    return FALSE;  
  }

  if(clnt_fwd_link_pddm_buf_ptr == NULL)
  {
    MSG_ERROR( "Null clnt_fwd_link_pddm_buf_ptr", 0, 0, 0);
    return FALSE;    
  }

  fwd_link_pddm_ptr = (is801_fwd_link_pddm_info_queue_element_s_type *)q_check(&(is801_clnt_arr[clnt_arr_idx].fwd_link_pddm_q));

  while( fwd_link_pddm_ptr != NULL )
  {
    if(fwd_link_pddm_ptr->wait_mask == IS801_FWD_LINK_WAIT_MASK_NONE)
    {
      /* Remove it from pddm_q */
      q_delete(&(is801_clnt_arr[clnt_arr_idx].fwd_link_pddm_q), &(fwd_link_pddm_ptr->link));

      /* Copy it to client */
      memscpy((void *)clnt_fwd_link_pddm_buf_ptr, sizeof(*clnt_fwd_link_pddm_buf_ptr), (void *)&(fwd_link_pddm_ptr->pddm_info), sizeof(fwd_link_pddm_ptr->pddm_info));
      
      /* Put it back to pddm free queue */ 
      is801_reset_fwd_link_pddm_queue_element(fwd_link_pddm_ptr);
      (void)pdsm_freebuf((char *)fwd_link_pddm_ptr);

      return TRUE;
    }
    else
    {
      fwd_link_pddm_ptr = (is801_fwd_link_pddm_info_queue_element_s_type *)q_next(&(is801_clnt_arr[clnt_arr_idx].fwd_link_pddm_q), &fwd_link_pddm_ptr->link);
    }
  } /* END while traversing the queue */  

  MSG_HIGH( "No more PDDM", 0, 0, 0);
  return FALSE;
}

/*===========================================================================

FUNCTION is801_create_rev_link_req_aa

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_create_rev_link_req_aa
(
  is801_rev_link_req_queue_element_s_type  *rev_link_req_q_elem_ptr
)
{
   
  if(rev_link_req_q_elem_ptr == NULL)
  {
    MSG_ERROR( "Null rev_link_req_q_elem_ptr", 0, 0, 0);    
    return FALSE;
  }

  rev_link_req_q_elem_ptr->req_msg.request_type = IS801_REV_LINK_REQ_MASK_AA;

  /* Always request for Additional Doppler */
  rev_link_req_q_elem_ptr->req_msg.req_msg_payload.aa_req.add_dopp_req    = TRUE;

  /* Always request for Azimuth and elevation */
  rev_link_req_q_elem_ptr->req_msg.req_msg_payload.aa_req.az_el_req       = TRUE;

  /* Always request for Code Phase measurements */
  rev_link_req_q_elem_ptr->req_msg.req_msg_payload.aa_req.code_ph_par_req = TRUE;

  /* Always request for 0th order Doppler */
  rev_link_req_q_elem_ptr->req_msg.req_msg_payload.aa_req.dopp_req        = TRUE;

  return TRUE;  
}

/*===========================================================================

FUNCTION is801_create_rev_link_req_sa

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_create_rev_link_req_sa
(
  is801_rev_link_req_queue_element_s_type  *rev_link_req_q_elem_ptr
)
{
   
  if(rev_link_req_q_elem_ptr == NULL)
  {
    MSG_ERROR( "Null rev_link_req_q_elem_ptr", 0, 0, 0);    
    return FALSE;
  }

  rev_link_req_q_elem_ptr->req_msg.request_type = IS801_REV_LINK_REQ_MASK_SA;

  return TRUE;  
}

/*===========================================================================

FUNCTION is801_create_rev_link_req_eph

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_create_rev_link_req_eph
(
  is801_rev_link_req_queue_element_s_type  *rev_link_req_q_elem_ptr
)
{
   
  if(rev_link_req_q_elem_ptr == NULL)
  {
    MSG_ERROR( "Null rev_link_req_q_elem_ptr", 0, 0, 0);    
    return FALSE;
  }

  rev_link_req_q_elem_ptr->req_msg.request_type = IS801_REV_LINK_REQ_MASK_EPH;
  rev_link_req_q_elem_ptr->req_msg.req_msg_payload.eph_req.ab_par_req = TRUE;

  return TRUE;  
}

/*===========================================================================

FUNCTION is801_create_rev_link_req_alm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_create_rev_link_req_alm
(
  is801_rev_link_req_queue_element_s_type  *rev_link_req_q_elem_ptr
)
{
   
  if(rev_link_req_q_elem_ptr == NULL)
  {
    MSG_ERROR( "Null rev_link_req_q_elem_ptr", 0, 0, 0);    
    return FALSE;
  }

  rev_link_req_q_elem_ptr->req_msg.request_type = IS801_REV_LINK_REQ_MASK_ALM;

  return TRUE;  
}

/*===========================================================================

FUNCTION is801_create_rev_link_req_lr

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_create_rev_link_req_lr
(
  is801_rev_link_req_queue_element_s_type  *rev_link_req_q_elem_ptr
)
{
   
  if(rev_link_req_q_elem_ptr == NULL)
  {
    MSG_ERROR( "Null rev_link_req_q_elem_ptr", 0, 0, 0);    
    return FALSE;
  }

  rev_link_req_q_elem_ptr->req_msg.request_type = IS801_REV_LINK_REQ_MASK_LR;

  return TRUE;  
}

/*===========================================================================

FUNCTION is801_new_rev_link_req

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_new_rev_link_req(
                             tm_prtl_type                  prtl_type,                             
                             is801_rev_link_req_mask_type  req_type
                           )
{
  int     prtl_arr_indx  = -1;
  boolean retVal = FALSE;
  is801_rev_link_req_queue_element_s_type  *rev_link_req_q_elem_ptr;  
  
  prtl_arr_indx = is801_get_clnt_arr_idx(prtl_type);

  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((prtl_arr_indx < 0) || (prtl_arr_indx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "No protocol match found", 0, 0, 0);
    return FALSE;  
  }
  
  rev_link_req_q_elem_ptr = (is801_rev_link_req_queue_element_s_type *)pdsm_getbuf(sizeof(is801_rev_link_req_queue_element_s_type));

  if(rev_link_req_q_elem_ptr)
  {
    switch(req_type)
    {
      case IS801_REV_LINK_REQ_MASK_AA:
        retVal = is801_create_rev_link_req_aa(rev_link_req_q_elem_ptr);
        break;

      case IS801_REV_LINK_REQ_MASK_SA:
        retVal = is801_create_rev_link_req_sa(rev_link_req_q_elem_ptr);
        break;

      case IS801_REV_LINK_REQ_MASK_LR:
        retVal = is801_create_rev_link_req_lr(rev_link_req_q_elem_ptr);
        break;

      case IS801_REV_LINK_REQ_MASK_ALM:
        retVal = is801_create_rev_link_req_alm(rev_link_req_q_elem_ptr);
        break;

      case IS801_REV_LINK_REQ_MASK_EPH:
        retVal = is801_create_rev_link_req_eph(rev_link_req_q_elem_ptr);
        break;

      default:
        MSG_ERROR("Request type %x not supported", req_type, 0, 0);
        retVal = FALSE;
        break;
    }

    if(retVal)
    {
      /* Req successfully created, put it in rev link req queue */
      MSG_MED("Request type %x queued", req_type, 0, 0);
      q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_req_q), &(rev_link_req_q_elem_ptr->link));
      return retVal;    
    }
    else
    {
      /* Failed to create req, put it back to rev link req free queue */    
      MSG_ERROR("Failed to queue request type %x", req_type, 0, 0);
      (void)pdsm_freebuf((char *)rev_link_req_q_elem_ptr);
      return retVal;
    }
  }
  else
  {
    MSG_ERROR("Unable to get buffer for rev-link request", 0, 0, 0);
    return FALSE;
  }  
}

/*===========================================================================

FUNCTION is801_new_rev_link_resp

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_new_rev_link_resp(
                              tm_prtl_type                    prtl_type,
                              is801_rev_link_resp_s_type      *resp_msg
                            )
{
  int     prtl_arr_indx = -1;
  boolean retVal = FALSE;
  is801_rev_link_resp_queue_element_s_type  *rev_link_resp_q_elem_ptr;  

  if(resp_msg == NULL)
  {
    MSG_ERROR( "Null resp_msg", 0, 0, 0);
    return FALSE;    
  }
  
  prtl_arr_indx = is801_get_clnt_arr_idx(prtl_type);

  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((prtl_arr_indx < 0) || (prtl_arr_indx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "No protocol match found", 0, 0, 0);
    return FALSE;  
  }

  rev_link_resp_q_elem_ptr = (is801_rev_link_resp_queue_element_s_type *)pdsm_getbuf(sizeof(is801_rev_link_resp_queue_element_s_type));

  if(rev_link_resp_q_elem_ptr)
  {
    switch(resp_msg->response_type)
    {
      case IS801_REV_LINK_RESP_MASK_MSI:
      case IS801_REV_LINK_RESP_MASK_PRM:
      case IS801_REV_LINK_RESP_MASK_PPM:
      case IS801_REV_LINK_RESP_MASK_TOM:
      case IS801_REV_LINK_RESP_MASK_LR:
      case IS801_REV_LINK_RESP_MASK_CACK:
      case IS801_REV_LINK_RESP_MASK_REJ:
      case IS801_REV_LINK_RESP_MASK_PRDM_BSP:
        MSG_MED("Creating response for type %x", resp_msg->response_type, 0, 0);
        memscpy((void *)&(rev_link_resp_q_elem_ptr->resp_msg), sizeof(rev_link_resp_q_elem_ptr->resp_msg),
                (void *)resp_msg, sizeof(*resp_msg));
        retVal = TRUE;
        break;       

      default:
        MSG_ERROR("Response type %x not supported", resp_msg->response_type, 0, 0);
        retVal = FALSE;
        break;
    }

    if(retVal)
    {
      /* Resp successfully created, put it in rev link resp queue */
      MSG_MED("Response type %x queued", resp_msg->response_type, 0, 0);
      q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_elem_ptr->link));
      return retVal;    
    }
    else
    {
      /* Failed to create resp, put it back to rev link resp free queue */ 
      MSG_ERROR("Failed to queue response type %x", resp_msg->response_type, 0, 0);
      (void)pdsm_freebuf((char *)rev_link_resp_q_elem_ptr);
      return retVal;
    }
  }
  else
  {
    MSG_ERROR("Unable to get buffer for rev-link resp", 0, 0, 0);
    return FALSE;
  }  
}

/*===========================================================================

FUNCTION is801_xlate_rev_link_req

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  If packing is done successfully, return number of bytes of the reverse link request,
  otherwise, return -1;

SIDE EFFECTS

===========================================================================*/
int is801_xlate_rev_link_req
(
  byte                                      *pddm_buff_ptr,
  is801_rev_link_req_queue_element_s_type   *req_ptr
)
{
  int retVal = -1;
  word  req_msg_size = 0;  
  
  if((pddm_buff_ptr == NULL) || (req_ptr == NULL))
  {
    MSG_ERROR("Null input pointer", 0, 0, 0);
    return -1;      
  }

  switch(req_ptr->req_msg.request_type)
  {
    case IS801_REV_LINK_REQ_MASK_AA:
      (void)paix_xlate_int_aa_req((byte *)pddm_buff_ptr, &(req_ptr->req_msg.req_msg_payload.aa_req), &req_msg_size);
      retVal = req_msg_size;
      break;

    case IS801_REV_LINK_REQ_MASK_SA:
      (void)paix_xlate_int_sa_req((byte *)pddm_buff_ptr, &req_msg_size);
      retVal = req_msg_size;
      break;

    case IS801_REV_LINK_REQ_MASK_LR:
      (void)paix_xlate_int_lr_req((byte *)pddm_buff_ptr, &(req_ptr->req_msg.req_msg_payload.lr_req), &req_msg_size);
      retVal = req_msg_size;
      break;

    case IS801_REV_LINK_REQ_MASK_ALM:
      (void)paix_xlate_int_alm_req((byte *)pddm_buff_ptr, &req_msg_size);
      retVal = req_msg_size;
      break;

    case IS801_REV_LINK_REQ_MASK_EPH:
      (void)paix_xlate_int_eph_req((byte *)pddm_buff_ptr, &(req_ptr->req_msg.req_msg_payload.eph_req), &req_msg_size);
      retVal = req_msg_size;
      break;

    default:
      MSG_ERROR("Unknown Request Type %d", req_ptr->req_msg.request_type, 0, 0);
      retVal = -1;
      break;
  }

  return retVal;
}

/*===========================================================================

FUNCTION      is801_queue_check_resp_type

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static int32 is801_queue_check_resp_type( void *p_Item, void *p_CompareValue )
{
  is801_rev_link_resp_queue_element_s_type *rev_link_resp_ptr;
  is801_rev_link_resp_mask_type             rev_link_resp_mask;
  
  if((p_Item == NULL) || (p_CompareValue == NULL))
  {
    return -1;
  }
  
  rev_link_resp_ptr = (is801_rev_link_resp_queue_element_s_type *)p_Item;
  rev_link_resp_mask = *(( is801_rev_link_resp_mask_type *)p_CompareValue);

  if(rev_link_resp_ptr->resp_msg.response_type & rev_link_resp_mask)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*===========================================================================

FUNCTION is801_check_rev_link_resp

DESCRIPTION
  This function checks if rev-link response queue contains a specific resp message
 
DEPENDENCIES

RETURN VALUE
  True, it response message is found in the rev-lin resp queue
  False, otherwise.

SIDE EFFECTS

===========================================================================*/
boolean is801_check_rev_link_resp
(
  int                           clnt_arr_idx,
  is801_rev_link_resp_mask_type rev_link_resp_mask
)
{
  is801_rev_link_resp_queue_element_s_type *rev_link_resp_ptr = NULL;
  
  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((clnt_arr_idx < 0) || (clnt_arr_idx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "Invalid client array index", 0, 0, 0);
    return FALSE;  
  }

  rev_link_resp_ptr = (is801_rev_link_resp_queue_element_s_type *)q_linear_search(&(is801_clnt_arr[clnt_arr_idx].rev_link_resp_q), 
                                                                                   (q_compare_func_type)is801_queue_check_resp_type,
                                                                                   (void *)&(rev_link_resp_mask));

  if(rev_link_resp_ptr != NULL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION is801_get_rev_link_resp_elem

DESCRIPTION

 
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
is801_rev_link_resp_queue_element_s_type* is801_get_rev_link_resp_elem
(
  int                           clnt_arr_idx,
  is801_rev_link_resp_mask_type rev_link_resp_mask
)
{
  is801_rev_link_resp_queue_element_s_type *rev_link_resp_ptr = NULL;
  
  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((clnt_arr_idx < 0) || (clnt_arr_idx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "Invalid client array index", 0, 0, 0);
    return NULL;  
  }

  rev_link_resp_ptr = (is801_rev_link_resp_queue_element_s_type *)q_linear_search(&(is801_clnt_arr[clnt_arr_idx].rev_link_resp_q), 
                                                                                   (q_compare_func_type)is801_queue_check_resp_type,
                                                                                   (void *)&(rev_link_resp_mask));

  if(rev_link_resp_ptr != NULL)
  {
     /* Take it out from the queue for further processing */
    q_delete(&(is801_clnt_arr[clnt_arr_idx].rev_link_resp_q), &(rev_link_resp_ptr->link));
  }

  return rev_link_resp_ptr;
}

/*===========================================================================

FUNCTION is801_fit_rev_link_resp

DESCRIPTION

 
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_fit_rev_link_resp
(
  is801_rev_link_resp_queue_element_s_type *rev_link_resp_q_elem_ptr,
  byte                                     offset,
  byte                                     max_pddm_size
)
{
  boolean retVal = FALSE;
  byte    temp;
  
  if(rev_link_resp_q_elem_ptr == NULL)
  {
    MSG_ERROR("null rev_link_resp_q_elem_ptr", 0, 0, 0);
    return FALSE;
  }

  switch (rev_link_resp_q_elem_ptr->resp_msg.response_type) 
  {
    case IS801_REV_LINK_RESP_MASK_REJ:
      if ((offset + NUM_BYTES_IN_BIT(REJ_RESP_LEN) + RESP_HDR_LEN) <= max_pddm_size) 
      {
        retVal = TRUE;
      }
      break;

    case IS801_REV_LINK_RESP_MASK_CACK:
      if ((offset + NUM_BYTES_IN_BIT(CACK_RESP_LEN) + RESP_HDR_LEN) <= max_pddm_size)
      {
        retVal = TRUE;
      }
      break;

    case IS801_REV_LINK_RESP_MASK_MSI:
      if ((offset + NUM_BYTES_IN_BIT(MSI_RESP_LEN) + RESP_HDR_LEN) <= max_pddm_size)
      {
        retVal = TRUE;
      }
      break;

    case IS801_REV_LINK_RESP_MASK_TOM:
      if ((offset + NUM_BYTES_IN_BIT(MSTO_RESP_LEN) + RESP_HDR_LEN) <= max_pddm_size)
      {
        retVal = TRUE;
      }
      break;
      
    case IS801_REV_LINK_RESP_MASK_LR:
      temp = (rev_link_resp_q_elem_ptr->resp_msg.resp_msg_payload.lr_resp.velocity_incl == 1) ? LOC_RESP_FIX1_LEN + LOC_RESP_FIX2_LEN : LOC_RESP_FIX1_LEN;
      temp += (rev_link_resp_q_elem_ptr->resp_msg.resp_msg_payload.lr_resp.clock_incl == 1) ? LOC_RESP_FIX3_LEN : 0;
      temp += (rev_link_resp_q_elem_ptr->resp_msg.resp_msg_payload.lr_resp.height_incl == 1) ? LOC_RESP_FIX4_LEN : 0;

      if ((offset + RESP_HDR_LEN + NUM_BYTES_IN_BIT(temp)) <= max_pddm_size)
      {
        retVal = TRUE;
      }
      break;

    case IS801_REV_LINK_RESP_MASK_PRDM_BSP:
      if ((offset + NUM_BYTES_IN_BIT(PRDM_BSP_RESP_LEN) + RESP_HDR_LEN) <= max_pddm_size)
      {
        retVal = TRUE;
      }
      break;

    default:
      MSG_ERROR("Unknown Reverse Link Response Type", 0, 0, 0);
      retVal = FALSE;
      break;
  }

  return retVal;
}

/*===========================================================================

FUNCTION is801_fit_rev_link_resp_ppm_prm

DESCRIPTION
  This function checks if all PPM and PRM parts can be fit into the remaining PDDM
 
DEPENDENCIES

RETURN VALUE
  TRUE, if all PPM and PRM parts can be fit into the remaining PDDM
  FALSE, otherwise.

SIDE EFFECTS

===========================================================================*/
boolean is801_fit_rev_link_resp_ppm_prm
(
  is801_ppm_resp_type  *ppm_ptr,
  is801_prm_resp_type  *prm_ptr,
  byte                 offset,
  byte                 max_pddm_size  
)
{
  word ppm_total_size_in_bits;
  byte ppm_total_size_in_bytes;
  word prm_total_size_in_bits;
  byte prm_total_size_in_bytes;  

  if((ppm_ptr == NULL) || (prm_ptr == NULL))
  {
    MSG_ERROR("null pointer", 0, 0, 0);
    return FALSE;
  }
  
  /* Calculate total PPM size (in bits) */
  ppm_total_size_in_bits = RESP_HDR_LEN * 8 + PPM_RESP_FIX1_LEN; 
  
  /* Add Offset if included. */
  ppm_total_size_in_bits += (ppm_ptr->fix.offset_incl == 1) ? PPM_RESP_FIX2_LEN : 0;  

  /* Add variable size */
  ppm_total_size_in_bits += ppm_ptr->fix.num_pilots_p * PPM_RESP_VAR_LEN;

  ppm_total_size_in_bytes = (ppm_total_size_in_bits + 7) / 8;
  
  /* Calculate total PRM size (in bits) */
  prm_total_size_in_bits = RESP_HDR_LEN * 8 + PR_RESP_FIX1_LEN; 
  
  /* Add Offset if included. */
  prm_total_size_in_bits += (prm_ptr->fix.offset_incl == 1) ? PR_RESP_FIX2_LEN : 0;

  /* Add variable par lenghth */
  prm_total_size_in_bits += prm_ptr->fix.num_ps_ranges_p * PR_RESP_VAR_LEN;

  prm_total_size_in_bytes = (prm_total_size_in_bits + 7) / 8;
  
  if((max_pddm_size - offset) >= (ppm_total_size_in_bytes + prm_total_size_in_bytes))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
  
/*===========================================================================

FUNCTION is801_smallest_ppm_part_size

DESCRIPTION
  This function is used to calculate the smallest possible size of a ppm part
  
DEPENDENCIES
  None

RETURN VALUE
  -1 for any error
  otherwise, size of smallest ppm part size in byte.

SIDE EFFECTS
  Note: We assume additional pilot incl flag is set to FALSE.
        If in the future, we want to support additional pilot incl, this 
        function has to be modified

===========================================================================*/
int is801_smallest_ppm_part_size
(
  is801_ppm_resp_type *ppm_ptr
    /* Pointer to the response element */
)
{
  int length = 0;

  if(ppm_ptr == NULL)
  {
    MSG_ERROR("Null ppm_ptr", 0, 0, 0);
    return -1;
  }
  
  /* Calculate the smallest PPM fragment.*/
  length = RESP_HDR_LEN * 8 + PPM_RESP_FIX1_LEN;

  /* Add MOB_SYS_T_OFFSET if included. */
  length += (ppm_ptr->fix.offset_incl == 1) ? PPM_RESP_FIX2_LEN : 0;

  MSG_MED("Smallest Length: %d", length, 0, 0);
  return ((length + 7) / 8);
}


/*===========================================================================

FUNCTION is801_calc_ppm_fragmentation_info

DESCRIPTION
  This function gives the information of how PPM should be fragmented based on
  the available pddm size.
 
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  Note: We assume additional pilot incl flag is set to FALSE.
        If in the future, we want to support additional pilot incl, this 
        function has to be modified

===========================================================================*/
boolean is801_calc_ppm_fragmentation_info
(
  is801_ppm_resp_type             *ppm_ptr,  
  uint8                           available_pddm_size,    
  uint8                           empty_pddm_payload_size,
  is801_ppm_fragment_info_s_type  *ppm_fragmentation_info_ptr
)
{
  word fixed_bits;
  int  num_pilots_fit_this_pddm;
  int  num_pilots_fit_full_pddm;  

  if((ppm_ptr == NULL) || (ppm_fragmentation_info_ptr == NULL))
  {
    MSG_ERROR("Null ppm_ptr or ppm_fragmentation_info_ptr", 0, 0, 0);
    return FALSE;
  }
  
  fixed_bits = RESP_HDR_LEN * 8 + PPM_RESP_FIX1_LEN; 
  
  /* Add Offset if included. */
  fixed_bits += (ppm_ptr->fix.offset_incl == 1) ? PPM_RESP_FIX2_LEN : 0;  

  /* First calculate how many pilots can be squeezed in the first pddm */
  num_pilots_fit_this_pddm = (available_pddm_size * 8 - fixed_bits) / PPM_RESP_VAR_LEN;

  /* Then calculate how many pilots can be squeezed in a full pddm */
  num_pilots_fit_full_pddm = (empty_pddm_payload_size * 8 - fixed_bits) / PPM_RESP_VAR_LEN;  

  /* If this PDDM is able to contain the rest of pilots */
  if((ppm_fragmentation_info_ptr->pilot_arr_idx_to_resume + num_pilots_fit_this_pddm) > ppm_ptr->fix.num_pilots_p)
  {
    ppm_fragmentation_info_ptr->num_pilot_in_this_pddm = ppm_ptr->fix.num_pilots_p - ppm_fragmentation_info_ptr->pilot_arr_idx_to_resume;
  }
  else
  {
    ppm_fragmentation_info_ptr->num_pilot_in_this_pddm = num_pilots_fit_this_pddm;
  }

  if(num_pilots_fit_this_pddm < 0)
  {
    MSG_ERROR("Can not fit any pilot in the first PDDM", 0, 0, 0);
    ppm_fragmentation_info_ptr->total_parts = 0;
    return FALSE;
  }
  else if(num_pilots_fit_this_pddm >= ppm_ptr->fix.num_pilots_p)
  {
    if(ppm_fragmentation_info_ptr->parsed == FALSE)
    {
      ppm_fragmentation_info_ptr->total_parts = 1;  
      ppm_fragmentation_info_ptr->part_num    = 1;
      ppm_fragmentation_info_ptr->parsed = TRUE;
    }
    return TRUE;
  }
  else
  {
    if(ppm_fragmentation_info_ptr->parsed == FALSE)
    {
      ppm_fragmentation_info_ptr->total_parts = 1; /* First PDDM */
      ppm_fragmentation_info_ptr->part_num    = 1;
      ppm_fragmentation_info_ptr->total_parts += ((ppm_ptr->fix.num_pilots_p - num_pilots_fit_this_pddm) + (num_pilots_fit_full_pddm - 1)) / num_pilots_fit_full_pddm;
      ppm_fragmentation_info_ptr->parsed = TRUE;
    }
    return TRUE;
  }
}

/*===========================================================================

FUNCTION is801_calc_prm_fragmentation_info

DESCRIPTION
  This function gives the information of how PRM should be fragmented based on
  the available pddm size.. 
 
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_calc_prm_fragmentation_info
(
  is801_prm_resp_type             *prm_ptr,  
  uint8                           available_pddm_size,  
  uint8                           empty_pddm_payload_size,
  is801_prm_fragment_info_s_type  *prm_fragmentation_info_ptr  
)
{
  word fixed_bits;
  int  num_meas_fit_this_pddm;
  int  num_meas_fit_full_pddm;  

  if((prm_ptr == NULL) || (prm_fragmentation_info_ptr == NULL))
  {
    MSG_ERROR("Null prm_ptr or prm_fragmentation_info_ptr", 0, 0, 0);
    return FALSE;
  }
  
  fixed_bits = RESP_HDR_LEN * 8 + PR_RESP_FIX1_LEN; 
  
  /* Add Offset if included. */
  fixed_bits += (prm_ptr->fix.offset_incl == 1) ? PR_RESP_FIX2_LEN : 0;  

  /* First calculate how many measurement can be squeezed in the first pddm */
  num_meas_fit_this_pddm = (available_pddm_size * 8 - fixed_bits) / PR_RESP_VAR_LEN;

  /* Then calculate how many measurement can be squeezed in a full pddm */
  num_meas_fit_full_pddm = (empty_pddm_payload_size * 8 - fixed_bits) / PR_RESP_VAR_LEN;  

  if((prm_fragmentation_info_ptr->sv_arr_idx_to_resume + num_meas_fit_this_pddm) > prm_ptr->fix.num_ps_ranges_p)
  {
    prm_fragmentation_info_ptr->num_meas_in_this_pddm = prm_ptr->fix.num_ps_ranges_p - prm_fragmentation_info_ptr->sv_arr_idx_to_resume;
  }
  else
  {
    prm_fragmentation_info_ptr->num_meas_in_this_pddm = num_meas_fit_this_pddm;
  }
  
  /* All pilots can be squeezed in the first pddm */
  if(num_meas_fit_this_pddm < 0)
  {
    MSG_ERROR("Can not fit any measurement in the first PDDM", 0, 0, 0);
    prm_fragmentation_info_ptr->total_parts = 0;
    return FALSE;
  }
  else if(num_meas_fit_this_pddm >= prm_ptr->fix.num_ps_ranges_p)
  {
    if(prm_fragmentation_info_ptr->parsed == FALSE)
    {
      prm_fragmentation_info_ptr->total_parts = 1;  
      prm_fragmentation_info_ptr->part_num    = 1;
      prm_fragmentation_info_ptr->parsed = TRUE;
      return TRUE;
    }
  }
  else
  {
    if(prm_fragmentation_info_ptr->parsed == FALSE)
    {  
      prm_fragmentation_info_ptr->total_parts = 1; /* First PDDM */
      prm_fragmentation_info_ptr->part_num    = 1;
      prm_fragmentation_info_ptr->total_parts += ((prm_ptr->fix.num_ps_ranges_p - num_meas_fit_this_pddm) + (num_meas_fit_full_pddm - 1)) / num_meas_fit_full_pddm;
      prm_fragmentation_info_ptr->parsed = TRUE;
      return TRUE;
    }
  }  
  return TRUE;  
}


/*===========================================================================

FUNCTION is801_create_rev_link_pddm

DESCRIPTION
 When protocol sub-modules creates rev-link PDDM, it also needs to tell IS801 
 module the maximum size of PDDM, which is limited by each protocol sub-modules'
 transport layer. 

 For example, 1x cp uses DBM as its transport layer, and the maximum dbm size 
 can differ from network to network.
 
DEPENDENCIES

RETURN VALUE
  Number of PDDM created.
  -1 indicates there is error when creating PDDM.

  Client should then call is801_GetRevLinkPDDM to get packed PDDM data to send it
  over to PDE.

SIDE EFFECTS

===========================================================================*/
int is801_create_rev_link_pddm
(
  tm_prtl_type                 prtl_type,
  is801_pddm_sess_info_s_type  msg_param,
  uint8                        max_pddm_size
)
{
  int                  prtl_arr_indx  = -1;
  int                  xlateVal = -1;
  int                  num_pddm = 0;  
  is801_pddm_hdr_type  pddm_hdr;  
  byte                 offset = 0;
  byte                 reserved_first_ppm_room_in_byte = 0;
  boolean              result = TRUE;
  word                 resp_msg_size = 0;
  boolean              prm_ppm_together = TRUE;
  is801_ppm_fragment_info_s_type            ppm_fragmentation_info;
  is801_prm_fragment_info_s_type            prm_fragmentation_info;  
  is801_rev_link_pddm_queue_element_s_type  *rev_link_pddm_q_elem_ptr = NULL;  
  is801_rev_link_req_queue_element_s_type   *rev_link_req_q_elem_ptr  = NULL;  
  is801_rev_link_resp_queue_element_s_type  *rev_link_resp_q_elem_ptr = NULL; 
  is801_rev_link_resp_queue_element_s_type  *rev_link_resp_q_ppm_ptr = NULL; 
  is801_rev_link_resp_queue_element_s_type  *rev_link_resp_q_prm_ptr = NULL;   

  ppm_fragmentation_info.parsed                  = FALSE;
  ppm_fragmentation_info.total_parts            = 0;
  ppm_fragmentation_info.pilot_arr_idx_to_resume = 0;
  

  prm_fragmentation_info.parsed               = FALSE;
  prm_fragmentation_info.total_parts          = 0;
  prm_fragmentation_info.sv_arr_idx_to_resume = 0;
 
  prtl_arr_indx = is801_get_clnt_arr_idx(prtl_type);

  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((prtl_arr_indx < 0) || (prtl_arr_indx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "No protocol match found", 0, 0, 0);
    return -1;
  }
  
  /* Check if size is valid */
  if((max_pddm_size < MIN_IS801_PDDM_SIZE) || (max_pddm_size > MAX_IS801_PDDM_SIZE))
  {
    MSG_ERROR("Invalid max pddm size %d", max_pddm_size, 0 ,0);
    return -1;
  }


  /* Looping until we finish all req/resp */
  while( (q_cnt(&(is801_clnt_arr[prtl_arr_indx].rev_link_req_q)))
       ||(q_cnt(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q))))
  {

    /* Get a buffer to hold packed PDDM */
    if(rev_link_pddm_q_elem_ptr == NULL)
    {
      /* Get a PDDM buffer and see if it can hold all req/resp */
      rev_link_pddm_q_elem_ptr = (is801_rev_link_pddm_queue_element_s_type *)pdsm_getbuf(sizeof(is801_rev_link_pddm_queue_element_s_type));
        
      if(rev_link_pddm_q_elem_ptr == NULL)
      {
        MSG_ERROR("Unable to get buffer for rev_link_pddm", 0, 0, 0);        
        result = FALSE;
        break;
      }

      rev_link_pddm_q_elem_ptr->pddm_info.req_mask  = IS801_REV_LINK_REQ_MASK_NONE;
      rev_link_pddm_q_elem_ptr->pddm_info.resp_mask = IS801_REV_LINK_RESP_MASK_NONE;
      
      num_pddm++;

      /* Copy whatever is set from prtocol sub-modules for the first pddm
      # of req/resp can not be determined right now, since we're not sure how much can be fit in one PDDM.
      */
      if(num_pddm == 1)
      {
        pddm_hdr.sess_start    = msg_param.sess_start;
        pddm_hdr.sess_end      = msg_param.sess_end;  
        pddm_hdr.sess_source   = msg_param.sess_source;
        pddm_hdr.sess_tag      = msg_param.sess_tag;  
        pddm_hdr.pd_msg_type   = msg_param.pd_msg_type;
        pddm_hdr.num_requests  = 0;
        pddm_hdr.num_responses = 0;  
      }
      else
      {
        /* Subsequent PDDMs */
        pddm_hdr.sess_start    = 0;
        pddm_hdr.sess_end      = 0;  
        pddm_hdr.sess_source   = msg_param.sess_source;
        pddm_hdr.sess_tag      = msg_param.sess_tag;  
        pddm_hdr.pd_msg_type   = msg_param.pd_msg_type;
        pddm_hdr.num_requests  = 0;
        pddm_hdr.num_responses = 0;        
      }

      /* Save room for pddm_hdr */
      offset = IS801_PDDM_HDR_LEN;      

      resp_msg_size = 0;
    }

    /* Process the request queue to pack all the requests into the capsule */
    /* Assume all the request can be fit in one pddm */
    while(NULL != (rev_link_req_q_elem_ptr = (is801_rev_link_req_queue_element_s_type*)q_get(&(is801_clnt_arr[prtl_arr_indx].rev_link_req_q))))
    {   
      rev_link_pddm_q_elem_ptr->pddm_info.req_mask |= rev_link_req_q_elem_ptr->req_msg.request_type;
      
      xlateVal = is801_xlate_rev_link_req((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), rev_link_req_q_elem_ptr);

      /* Free req elem, no matter what */
      (void)pdsm_freebuf((char *)rev_link_req_q_elem_ptr);

      if(xlateVal)
      { 
        pddm_hdr.num_requests++; /*lint !e644 */
        offset += (xlateVal / 8);
      }
      else
      {
        MSG_ERROR("Error when packing rev link request", 0, 0 ,0);
        result = FALSE;
        break;
      }
    }

    if(result == TRUE)
    {
      /* Packing responses, using the rules below.
       ** 1. Pack REJ, CACK, MSI, TOM, LR response first
       ** 2. Then pack PPM, and PRM
      */

      /* There might be multiple REJ responses in the queue, so use while to find them all */
      while((rev_link_resp_q_elem_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_REJ)) != NULL)
      {        
        /* Try to fit in Rej resp */

        if(is801_fit_rev_link_resp(rev_link_resp_q_elem_ptr, offset, max_pddm_size))
        {
          rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_REJ;
            
          (void)paix_xlate_int_rej_resp((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), &(rev_link_resp_q_elem_ptr->resp_msg), &resp_msg_size);

          pddm_hdr.num_responses++;
  
          offset += (resp_msg_size / 8);
  
          /* if it can fit in, release resp elem */
          (void)pdsm_freebuf((char *)rev_link_resp_q_elem_ptr);
        }
        else
        {
          /* if it can NOT fit in,
          ** 1. Put pddm_ptr to rev_link_pddm queue
          ** put resp elem back to rev_link_resp_q for next PDDM 
          ** do the while loop again
          */
          (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

          if(rev_link_pddm_q_elem_ptr != NULL)
          {
            rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;
          }
          
          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_elem_ptr->link));

          rev_link_pddm_q_elem_ptr = NULL;

          break;
          /* break here (to avoid infinite loop).
             Go ahead and let subsequent code to take care of getting another PDDM for the rest of reject responses */
        }
      }

      if(rev_link_pddm_q_elem_ptr == NULL)
      {
        /* Go ahead and let subsequent code to take care of getting another PDDM for the rest of reject responses */
        continue;
      }

      if((rev_link_resp_q_elem_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_CACK)) != NULL)
      {        
        /* Try to fit in CACK resp */

        if(is801_fit_rev_link_resp(rev_link_resp_q_elem_ptr, offset, max_pddm_size))
        {
          rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_CACK;
          
          (void)paix_xlate_int_cack_resp((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), &(rev_link_resp_q_elem_ptr->resp_msg), &resp_msg_size);

          pddm_hdr.num_responses++;

          offset += (resp_msg_size / 8);  

          /* if it can fit in, release resp elem */
          (void)pdsm_freebuf((char *)rev_link_resp_q_elem_ptr);
        }
        else
        {
          /* if it can NOT fit in,
          ** 1. Put pddm_ptr to rev_link_pddm queue
          ** put resp elem back to rev_link_resp_q for next PDDM 
          ** do the while loop again
          */
          (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

          rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;

          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_elem_ptr->link));

          rev_link_pddm_q_elem_ptr = NULL;

          continue;
        }
      }

      if((rev_link_resp_q_elem_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_MSI)) != NULL)
      {        
        /* Try to fit in MS Info resp */

        if(is801_fit_rev_link_resp(rev_link_resp_q_elem_ptr, offset, max_pddm_size))
        {
          rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_MSI;
          
          (void)paix_xlate_int_msi_resp((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), &(rev_link_resp_q_elem_ptr->resp_msg), &resp_msg_size);

          pddm_hdr.num_responses++;
  
          offset += (resp_msg_size / 8);

          /* if it can fit in, release resp elem */
          (void)pdsm_freebuf((char *)rev_link_resp_q_elem_ptr);
        }
        else
        {
          /* if it can NOT fit in,
          ** 1. Put pddm_ptr to rev_link_pddm queue
          ** put resp elem back to rev_link_resp_q for next PDDM 
          ** do the while loop again
          */
          (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

          rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;

          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_elem_ptr->link));

          rev_link_pddm_q_elem_ptr = NULL;

          continue;
        }
      }

      if((rev_link_resp_q_elem_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_TOM)) != NULL)
      {        
        /* Try to fit in Time offset resp */

        if(is801_fit_rev_link_resp(rev_link_resp_q_elem_ptr, offset, max_pddm_size))
        {
          rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_TOM;
        
          (void)paix_xlate_int_tom_resp((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), &(rev_link_resp_q_elem_ptr->resp_msg), &resp_msg_size);

          pddm_hdr.num_responses++;

          offset += (resp_msg_size / 8);

          /* if it can fit in, release resp elem */
          (void)pdsm_freebuf((char *)rev_link_resp_q_elem_ptr);
        }
        else
        {
          /* if it can NOT fit in,
          ** 1. Put pddm_ptr to rev_link_pddm queue
          ** put resp elem back to rev_link_resp_q for next PDDM 
          ** do the while loop again
          */
          (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

          rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;

          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_elem_ptr->link));
  
          rev_link_pddm_q_elem_ptr = NULL;

          continue;
        }
      }

      if((rev_link_resp_q_elem_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_LR)) != NULL)
      {        
        /* Try to fit in LR resp */

        if(is801_fit_rev_link_resp(rev_link_resp_q_elem_ptr, offset, max_pddm_size))
        {
          rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_LR;
          
          (void)paix_xlate_int_lr_resp((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), &(rev_link_resp_q_elem_ptr->resp_msg), &resp_msg_size);

          pddm_hdr.num_responses++;

          offset += (resp_msg_size / 8);

          /* if it can fit in, release resp elem */
          (void)pdsm_freebuf((char *)rev_link_resp_q_elem_ptr);
        }
        else
        {
          /* if it can NOT fit in,
          ** 1. Put pddm_ptr to rev_link_pddm queue
          ** put resp elem back to rev_link_resp_q for next PDDM 
          ** do the while loop again
          */
          (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

          rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;

          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_elem_ptr->link));

          rev_link_pddm_q_elem_ptr = NULL;

          continue;
        }
      }

      if((rev_link_resp_q_elem_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_PRDM_BSP)) != NULL)
      {   
         if(offset == IS801_PDDM_HDR_LEN)
         {
           MSG_MED("Translate PRDM resp",0,0,0);

           rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_PRDM_BSP;
          
           (void)paix_xlate_int_prdm_bsp_resp((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), &(rev_link_resp_q_elem_ptr->resp_msg), &resp_msg_size);

           pddm_hdr.num_responses++;
  
           offset += (resp_msg_size / 8);

          /* if it can fit in, release resp elem */
          (void)pdsm_freebuf((char *)rev_link_resp_q_elem_ptr);

           pddm_hdr.pd_msg_type = PRDM_MSG_TYPE;
           pddm_hdr.sess_start = 0;

           (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

           rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;
  
           q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

           rev_link_pddm_q_elem_ptr = NULL;

           continue;
         }
         else
         {
           MSG_MED("Create new PDDM for PRDM resp",0,0,0);
           (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

           rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;

           q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

           q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_elem_ptr->link));
 
           rev_link_pddm_q_elem_ptr = NULL;

           continue;
         }       
      }
      /* 
      ** When we reach here, there should be only PPM and PRM left 
      ** If they both exist, part of them should be included in the first PDDM
      */
      
//<<< ************************ PRM & PPM fragmentation logic ****************************************

      /* If this is the first PDDM and there are PRM and PPM, (partial or all PRM) and (all PPM or smallest possible PPM) need to go together */
      /* Subsequent PDDMs should contain either PRM or PPM */
      if((prm_ppm_together == TRUE) 
        &&(is801_check_rev_link_resp(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_PRM) == TRUE) 
        &&(is801_check_rev_link_resp(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_PPM) == TRUE))
      {
        rev_link_resp_q_prm_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_PRM);
        rev_link_resp_q_ppm_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_PPM);

        if( (rev_link_resp_q_prm_ptr == NULL)
          ||(rev_link_resp_q_ppm_ptr == NULL))
        {
          MSG_ERROR( "Null pointers", 0, 0, 0);
          
          (void)pdsm_freebuf((char *)rev_link_resp_q_prm_ptr);
          rev_link_resp_q_prm_ptr = NULL;
          
          (void)pdsm_freebuf((char *)rev_link_resp_q_ppm_ptr);
          rev_link_resp_q_ppm_ptr = NULL;
          
          result = FALSE;
          break;
        }
      
        /* 
           First check if all PPM and PRM parts can be squeezed in a single PDDM.
           If they can, first PDDM should contains all PPM parts
           Otherwise, just put smallest possible PPM part in the first PDDM.
        */
        if(is801_fit_rev_link_resp_ppm_prm(&(rev_link_resp_q_ppm_ptr->resp_msg.resp_msg_payload.ppm_resp),
                                           &(rev_link_resp_q_prm_ptr->resp_msg.resp_msg_payload.prm_resp),
                                           offset,
                                           max_pddm_size) == TRUE)
        {
          reserved_first_ppm_room_in_byte = 0;
        }
        else
        {
          reserved_first_ppm_room_in_byte = is801_smallest_ppm_part_size(&(rev_link_resp_q_ppm_ptr->resp_msg.resp_msg_payload.ppm_resp));
        }

        /* 
          1. Calculate number of total parts for PRM
          2. Try to fit in as much PRM as possible, 
          3. First calculate how many pseudoranges can be fit in current PDDM
          4. if it can not fit all, put rev_link_resp_q_ppm_ptr back in the rev_link_resp_q
          5. mark (prm_fragmentation_info -> part number, etc) where we should resume next time
        */

        (void)is801_calc_prm_fragmentation_info(&(rev_link_resp_q_prm_ptr->resp_msg.resp_msg_payload.prm_resp),
                                                (max_pddm_size - offset - reserved_first_ppm_room_in_byte),
                                                max_pddm_size,
                                                &prm_fragmentation_info);

        /*Second part of the check if to make Lint happy, as if  there is no space in PDDM,
        above function will set total parts to 0
        */
        if( (prm_fragmentation_info.total_parts > 0) && (offset < MAX_IS801_PDDM_SIZE))
        {      
          rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_PRM;

          /* Try to fit in as much PRM as possible  */
          (void)paix_xlate_int_prm_part((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), 
                                        &(rev_link_resp_q_prm_ptr->resp_msg), 
                                        &prm_fragmentation_info,
                                        &resp_msg_size);

          pddm_hdr.num_responses++;                             
          offset += (resp_msg_size / 8);

          /* Mark where to resume next time */
          prm_fragmentation_info.sv_arr_idx_to_resume += prm_fragmentation_info.num_meas_in_this_pddm;
          prm_fragmentation_info.part_num++;
          
        }


        /* Put rev_link_resp_q_prm_ptr back to proper queue */
        if(rev_link_resp_q_prm_ptr->resp_msg.resp_msg_payload.prm_resp.fix.num_ps_ranges_p > prm_fragmentation_info.sv_arr_idx_to_resume)
        {
          /* If all measurement can not be squeezed in, put rev_link_resp_q_prm_ptr back in the rev_link_resp_q */        
          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_prm_ptr->link));

           rev_link_resp_q_prm_ptr = NULL;
        }
        else
        {
          /* if all prm part can fit in, release it */
          (void)pdsm_freebuf((char *)rev_link_resp_q_prm_ptr);
          rev_link_resp_q_prm_ptr = NULL;
        }
        
        /*
          1. Calculate number of total parts for PPM
          2. Fit in proper PPM parts first (either smallest possible PPM or the entire PPM)
          3. Put rev_link_resp_q_ppm_ptr back in the rev_link_resp_q, 
          4. mark (ppm_fragmentation_info -> part number, etc) where we should resume next time 
        */
        
        (void)is801_calc_ppm_fragmentation_info(&(rev_link_resp_q_ppm_ptr->resp_msg.resp_msg_payload.ppm_resp),
                                                (max_pddm_size - offset),
                                                max_pddm_size,
                                                &ppm_fragmentation_info);

        /*Second part of the check if to make Lint happy, as if  there is no space in PDDM,
        above function will set total parts to 0
        */
        if( (ppm_fragmentation_info.total_parts > 0) && (offset < MAX_IS801_PDDM_SIZE))
        {        
          rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_PPM;

          /* Squeeze in all ppm part or smallest possible PPM part in the first PDDM */
          (void)paix_xlate_int_ppm_part((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]),
                                        &(rev_link_resp_q_ppm_ptr->resp_msg),
                                        &ppm_fragmentation_info,
                                        &resp_msg_size);

          pddm_hdr.num_responses++;                                
          offset += (resp_msg_size / 8);

          /* Mark where to resume next time */
          ppm_fragmentation_info.pilot_arr_idx_to_resume += ppm_fragmentation_info.num_pilot_in_this_pddm;
          ppm_fragmentation_info.part_num++;
        }

        /* Put rev_link_resp_q_ppm_ptr back to proper queue */        
        if(rev_link_resp_q_ppm_ptr->resp_msg.resp_msg_payload.ppm_resp.fix.num_pilots_p > ppm_fragmentation_info.pilot_arr_idx_to_resume)
        {
          /* If all pilots can not be squeezed in, put rev_link_resp_q_ppm_ptr back in the rev_link_resp_q */        
          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_ppm_ptr->link));

          rev_link_resp_q_ppm_ptr = NULL; 
        }
        else
        {
          /* if all ppm part can fit in, release it */
          (void)pdsm_freebuf((char *)rev_link_resp_q_ppm_ptr);
          rev_link_resp_q_ppm_ptr = NULL;
        }
            
        /* Send PRM/PPM together only for the first PDDM */
        prm_ppm_together = FALSE;

        /* We've already tried to squeeze as much data as possible for this PDDM, 
           now, put it in the pddm queue, and loop to see if there is more PDDM to be created
        */
        (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

        rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;

        q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

        rev_link_pddm_q_elem_ptr = NULL;
        
        continue;
      }

      /* This PDDM should only contain PRM (No PPM) */
      if(is801_check_rev_link_resp(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_PRM) == TRUE)
      {      
        rev_link_resp_q_prm_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_PRM);

        if(rev_link_resp_q_prm_ptr == NULL)
        {
          MSG_ERROR( "NUll rev_link_resp_q_prm_ptr", 0, 0, 0);
          result = FALSE;
          break;    
        }
        
        /* 1. Try to fit in as much PRM as possible, 
           2. First calculate how many pseudoranges can be fit in current PDDM
           3. if can not fit all, put rev_link_resp_q_ppm_ptr back in the rev_link_resp_q
           4. mark (prm_fragmentation_info -> part number, etc) where we should resume next time
        */
        
        (void)is801_calc_prm_fragmentation_info(&(rev_link_resp_q_prm_ptr->resp_msg.resp_msg_payload.prm_resp),
                                                (max_pddm_size - offset),
                                                max_pddm_size,
                                                &prm_fragmentation_info);

        /*Second part of the check if to make Lint happy, as if  there is no space in PDDM,
        above function will set total parts to 0
        */
        if( (prm_fragmentation_info.total_parts > 0) && (offset < MAX_IS801_PDDM_SIZE) )
        {      
          rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_PRM;

          /* Try to fit in as much PRM as possible  */
          (void)paix_xlate_int_prm_part((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), 
                                        &(rev_link_resp_q_prm_ptr->resp_msg), 
                                        &prm_fragmentation_info,
                                        &resp_msg_size);

          pddm_hdr.num_responses++;
          offset += (resp_msg_size / 8);

          /* Mark where to resume next time */
          prm_fragmentation_info.sv_arr_idx_to_resume += prm_fragmentation_info.num_meas_in_this_pddm;
          prm_fragmentation_info.part_num++;
        }

        /* Put rev_link_resp_q_prm_ptr back to proper queue */
        if(rev_link_resp_q_prm_ptr->resp_msg.resp_msg_payload.prm_resp.fix.num_ps_ranges_p > prm_fragmentation_info.sv_arr_idx_to_resume)
        {
          /* If all measurement can not be squeezed in, put rev_link_resp_q_prm_ptr back in the rev_link_resp_q */
          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_prm_ptr->link));
  
          rev_link_resp_q_prm_ptr = NULL;
        }
        else
        {
          /* if all prm parts are processed, release it */
          (void)pdsm_freebuf((char *)rev_link_resp_q_prm_ptr);
          rev_link_resp_q_prm_ptr = NULL;
        }
      
        /* We've already tried to squeeze as much data as possible for this PDDM, 
           now, put it in the pddm queue, and loop to see if there is more PDDM to be created
        */
        (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

        rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;

        q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

        rev_link_pddm_q_elem_ptr = NULL;

        continue;
      }

      /* PDDM should only contain PPM (No PRM) */
      if(is801_check_rev_link_resp(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_PPM) == TRUE)
      {      
        rev_link_resp_q_ppm_ptr = is801_get_rev_link_resp_elem(prtl_arr_indx, IS801_REV_LINK_RESP_MASK_PPM);

        if(rev_link_resp_q_ppm_ptr == NULL)
        {
          MSG_ERROR( "NUll rev_link_resp_q_ppm_ptr", 0, 0, 0);
          result = FALSE;
          break;   
        }
        
        /* 1. Try to fit in as much PPM as possible, 
           2. First calculate how many pilots can be fit in current PDDM
           3. if can not fit all, put rev_link_resp_q_ppm_ptr back in the rev_link_resp_q
           4. mark (ppm_fragmentation_info -> part number, etc) where we should resume next time
        */            
        (void)is801_calc_ppm_fragmentation_info(&(rev_link_resp_q_ppm_ptr->resp_msg.resp_msg_payload.ppm_resp),
                                                (max_pddm_size - offset),
                                                max_pddm_size,
                                                &ppm_fragmentation_info);

        /*Second part of the check if to make Lint happy, as if  there is no space in PDDM,
        above function will set total parts to 0
        */
        if( (ppm_fragmentation_info.total_parts > 0) && (offset < MAX_IS801_PDDM_SIZE) )
        {
        
          rev_link_pddm_q_elem_ptr->pddm_info.resp_mask |= IS801_REV_LINK_RESP_MASK_PPM;
           
          /* Squeeze in as much as possible PPM part in this PDDM */
          (void)paix_xlate_int_ppm_part((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[offset]), 
                                        &(rev_link_resp_q_ppm_ptr->resp_msg), 
                                        &ppm_fragmentation_info,
                                        &resp_msg_size);

          pddm_hdr.num_responses++;
          offset += (resp_msg_size / 8);

          /* Mark where to resume next time */
          ppm_fragmentation_info.pilot_arr_idx_to_resume += ppm_fragmentation_info.num_pilot_in_this_pddm;
          ppm_fragmentation_info.part_num++;
        }

        /* Put rev_link_resp_q_ppm_ptr back to proper queue */   
        if(rev_link_resp_q_ppm_ptr->resp_msg.resp_msg_payload.ppm_resp.fix.num_pilots_p > ppm_fragmentation_info.pilot_arr_idx_to_resume)
        {
          /* If all pilots can not be squeezed in, put rev_link_resp_q_ppm_ptr back in the rev_link_resp_q */        
          q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_resp_q), &(rev_link_resp_q_ppm_ptr->link));

          rev_link_resp_q_ppm_ptr = NULL;
        }
        else
        {
          /* if all ppm parts are processed, release it */
          (void)pdsm_freebuf((char *)rev_link_resp_q_ppm_ptr);
          rev_link_resp_q_ppm_ptr = NULL;
        }
      
        /* We've already tried to squeeze as much data as possible for this PDDM, 
           now, put it in the pddm queue, and loop to see if there is more PDDM to be created
        */
        (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

        rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;

        q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

        rev_link_pddm_q_elem_ptr = NULL;

        continue;
      }
//>>> ***********************************************************************************************

      /* When we reach here, we must have finished packing all the req/resp */
      (void)paix_xlate_int_pddm_hdr((byte *)&(rev_link_pddm_q_elem_ptr->pddm_info.msg_payload[0]), &pddm_hdr);

      rev_link_pddm_q_elem_ptr->pddm_info.msg_size = offset;

      q_put(&(is801_clnt_arr[prtl_arr_indx].rev_link_pddm_q), &(rev_link_pddm_q_elem_ptr->link));

      rev_link_pddm_q_elem_ptr = NULL;
    }
    else
    {
      /* Unexpected error, put pddm ptr back to rev_link_pddm_free_q */
      break;
    }  
  }

  if(result == FALSE)
  {
    if(rev_link_pddm_q_elem_ptr)
    {
      (void)pdsm_freebuf((char *)rev_link_pddm_q_elem_ptr);
    }

    num_pddm = -1;
  }
  
  return num_pddm;
}

/*===========================================================================

FUNCTION is801_get_rev_link_pddm

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is801_get_rev_link_pddm
(
  tm_prtl_type                     prtl_type,
  is801_rev_link_pddm_info_s_type  *pddm_info_ptr
)
{
  int                                        clnt_arr_idx = -1;
  is801_rev_link_pddm_queue_element_s_type   *rev_link_pddm_ptr = NULL;
  
  clnt_arr_idx = is801_get_clnt_arr_idx(prtl_type);

  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((clnt_arr_idx < 0) || (clnt_arr_idx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "No protocol match found", 0, 0, 0);
    return FALSE;  
  }

  if(pddm_info_ptr == NULL)
  {
    MSG_ERROR( "Null pddm_info_ptr", 0, 0, 0);
    return FALSE;    
  }

  rev_link_pddm_ptr = (is801_rev_link_pddm_queue_element_s_type*)q_get(&(is801_clnt_arr[clnt_arr_idx].rev_link_pddm_q));

  if(rev_link_pddm_ptr)
  {
    /* Copy packed PDDM to client memory buffer */
    memscpy((void *)pddm_info_ptr, sizeof(*pddm_info_ptr), (void *)&(rev_link_pddm_ptr->pddm_info), sizeof(rev_link_pddm_ptr->pddm_info));

    /* Put it back to rev_link_pddm_free_q */
    (void)pdsm_freebuf((char *)rev_link_pddm_ptr);
    return TRUE;
  }  
  else
  {
    MSG_HIGH( "No reverse link pddm available", 0, 0, 0);
    return FALSE;      
  }
}

/*===========================================================================

FUNCTION is801_reset_data

DESCRIPTION
  This function needs to be called when client finishes a session
  for any reason.

  It ensures that all the data structures are re-initiailzed for future IS801 sessions.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void is801_reset_data(
                      tm_prtl_type prtl_type
                    )
{
  q_link_type*  link_ptr;
  int           clnt_arr_idx = -1;
  is801_fwd_link_pddm_info_queue_element_s_type  *fwd_link_pddm_ptr = NULL;  
  
  clnt_arr_idx = is801_get_clnt_arr_idx(prtl_type);

  /*-------------------------------------------------------------------------
    Check to ensure valid input parameters.
  -------------------------------------------------------------------------*/
  if((clnt_arr_idx < 0) || (clnt_arr_idx >= TM_IS801_MAX_CLIENT))
  {
    MSG_ERROR( "No protocol match found", 0, 0, 0);
    return;  
  }

  /* Put rev_link_req_q back to rev_link_req_free_q*/
  while ( (link_ptr = (q_link_type *)q_get(&(is801_clnt_arr[clnt_arr_idx].rev_link_req_q))) != NULL)
  {
    (void)pdsm_freebuf((char *)link_ptr);
  }

  /* Put rev_link_resp_q back to  rev_link_resp_free_q*/
  while ( (link_ptr = (q_link_type *)q_get(&(is801_clnt_arr[clnt_arr_idx].rev_link_resp_q))) != NULL)
  {
    (void)pdsm_freebuf((char *)link_ptr);
  }

  /* Put rev_link_pddm_q back to  rev_link_pddm_free_q*/
  while ( (link_ptr = (q_link_type *)q_get(&(is801_clnt_arr[clnt_arr_idx].rev_link_pddm_q))) != NULL)
  {
    (void)pdsm_freebuf((char *)link_ptr);
  }

  /* Put fwd_link_pddm_q back to  fwd_link_pddm_free_q*/
  while ( (fwd_link_pddm_ptr = (is801_fwd_link_pddm_info_queue_element_s_type *)q_get(&(is801_clnt_arr[clnt_arr_idx].fwd_link_pddm_q))) != NULL)
  {
    is801_reset_fwd_link_pddm_queue_element(fwd_link_pddm_ptr);
    (void)pdsm_freebuf((char *)fwd_link_pddm_ptr);
  }

  return;
}


/*===========================================================================

FUNCTION tm_is801_ftcal_enabled

DESCRIPTION    

DEPENDENCIES   None.

RETURN VALUE   

SIDE EFFECTS   None.

===========================================================================*/

void tm_is801_ftcal_enabled( boolean b_Enable )
{
  is801_FTCalEnabled = b_Enable;
}

/*===========================================================================

FUNCTION tm_is801_ftcal_tt_data

DESCRIPTION    

DEPENDENCIES   None.

RETURN VALUE   

SIDE EFFECTS   None.

===========================================================================*/

void tm_is801_ftcal_tt_data(cgps_FtcalTTStruct* p_TTData)
{
  is801_FTCalTtData = *p_TTData;
}

/*===========================================================================

FUNCTION tm_is801_ftcal_ppm_data

DESCRIPTION    

DEPENDENCIES   None.

RETURN VALUE   

SIDE EFFECTS   None.

===========================================================================*/

void tm_is801_ftcal_ppm_data(cgps_FtcalPpmStruct* p_PpmData)
{
  is801_FTCalPpmData = *p_PpmData;
}

/*===========================================================================

FUNCTION tm_is801_get_ftcal_data

DESCRIPTION    

DEPENDENCIES   None.

RETURN VALUE   

SIDE EFFECTS   None.

===========================================================================*/

void tm_is801_get_ftcal_data ( is801_FtcalDataStruct * p_FtcalData )
{
  (*p_FtcalData).p_FTCalEnabled = &is801_FTCalEnabled;
  (*p_FtcalData).p_Ppm          = &is801_FTCalPpmData;
  (*p_FtcalData).p_TT           = &is801_FTCalTtData;
}
