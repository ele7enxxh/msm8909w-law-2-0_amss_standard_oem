/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   RPM   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager service domain selection Object.

  The  service domain selection Object is responsible for:
  1. Activating/Deactivating 1XCSFB.
  2. Receiving the info related to CSFB from 1XCP and NAs and processing
     the info.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmrpm_init() must be called to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2010 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/04/13   jvo     Mainline FEATURE_MMODE_RPM, add cmrpm_is_rpm_enabled
02/07/13   vm      Reset in PS call idle mode
11/16/12   vm      Start 1hr timer on any reject cause. Reset 1hr timer on
                   CS and PS registration only.
08/03/12   jh      Remove RPM version update removal
11/15/11   jh      Integrated RPM feature

===========================================================================*/


/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines
        enums
        macros

        functions
        - Internal [Common, 1x only, GW only]
        - External [Common, 1x only, GW only]

****************************************************************************/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

  
#include "mmcp_variation.h"
#include "customer.h"

#include "cmrpm.h"
#include "cmph.h"
#include "cmmmgsdi.h"
#include "cmtaski.h"
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmlog.h"     /* Interface for Diagnostic Logging */
#include "cmlog_v.h"
#include "time_svc.h"  /* Interface to clock services */
#include "sys_m_reset.h"
#include "mm.h"
#include "cmutil.h"


#ifdef CM_DEBUG
#error code not present
#endif


/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/


#define CMMMGSDI_RPM_OMC_N1_T1_SIZE    2 /* EF-Operational Management N1 and T1 Counters */

/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/
  /*===========================================================================
  
  FUNCTION cmrpm_gsdi_lpm_cb
  
  DESCRIPTION
    This is callback function for processing the mmgsdi_card_pdown/pup.  
  
    This function should be called after the gsdi card status change.
  
  DEPENDENCIES
    none
  
  RETURN VALUE
    none
  
  SIDE EFFECTS
    none
  
  ===========================================================================*/
  static void cmrpm_gsdi_lpm_cb(
      mmgsdi_return_enum_type       status,
      mmgsdi_cnf_enum_type          cnf,
      const mmgsdi_cnf_type         *cnf_ptr
  )
  {
  
    CM_ASSERT(cnf_ptr != NULL);
  
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
    if (status == MMGSDI_SUCCESS)
    {
      switch(cnf)
      {
        case MMGSDI_CARD_PUP_CNF:
          CM_MSG_HIGH_0("RPM: MMGSDI succesfull power-up SIM RESET finished");
          break;
  
        case MMGSDI_CARD_PDOWN_CNF:
          CM_MSG_HIGH_0(" ATT RPM : MMGSDI succesfull power-down");
          break;
        case MMGSDI_REFRESH_CNF:
          switch (cnf_ptr->refresh_cnf.orig_refresh_req)
          {
            case MMGSDI_REFRESH_ORIG_REG_REQ:
              CM_MSG_HIGH_2("MMGSDI suceed reg CM refsh evnt.Status %d, Confirmation %d",
                     status, cnf);
              break;
  
            case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
              CM_MSG_HIGH_2("Ok to refsh Ack succeed %d %d", cnf, status);
              break;
  
            default:
              CM_ERR_2("MMGSDI ret unexpected cnf.orig_refresh_req %d, Confirmation %d",
                     cnf_ptr->refresh_cnf.orig_refresh_req, cnf);
              break;
          }
          break;
  
        default:
          CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
                 status, cnf);
          break;
      }
    }
    else if(status == MMGSDI_REFRESH_SUCCESS)
    {
      switch(cnf)
      {
        case MMGSDI_CARD_PUP_CNF:
          CM_MSG_HIGH_0("MMGSDI power-up REFRESH CNF");
          
          break;
        case MMGSDI_CARD_PDOWN_CNF:
          CM_MSG_HIGH_0("MMGSDI power-down REFRESH CNF");
          break;
  
        default:
          CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
                 status, cnf);
          break;
      }
  
    }
    else /* status != MMGSDI_SUCCESS */
    {
      switch(cnf)
      {
        case MMGSDI_CARD_PUP_CNF:
          CM_ERR_0("MMGSDI power-up fail");       
          break;
        case MMGSDI_CARD_PDOWN_CNF:
          CM_ERR_0("MMGSDI power-down fail, continue power up");
          break;
        case MMGSDI_REFRESH_CNF:
          switch (cnf_ptr->refresh_cnf.orig_refresh_req)
          {
            case MMGSDI_REFRESH_ORIG_REG_REQ:
              CM_MSG_HIGH_2("MMGSDI failed reg CM refsh evnt.Status %d, Confirmation %d",
                     status, cnf);
              break;
      
            case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
              CM_MSG_HIGH_2("Ok to refresh Ack fail %d %d", cnf, status);
              break;
      
            default:
              CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
                     status, cnf);
              break;
          }
          break;
  
        default:
          CM_ERR_2("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
                 status, cnf);
          break;
      }
    }  /* status != MMGSDI_SUCCESS */
  }  /* cmrpm_gsdi_lpm_cb() */


/*===========================================================================

FUNCTION cmrpm_read_efs_sys_time

DESCRIPTION
 This function reads system time from EFS, that was written at last power-down 
 And updates local variable sys_time_efs with the time read from EFS.
 If time read from EFS is greater than uptime returned by API  
 cm_util_get_curr_systime(), we can consider coin cell battery as absent and
 use EFS itself for maintaining time across resets.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_read_efs_sys_time(boolean is_update)
{
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
  uint32                        sys_time_pwr_up ;
  int32                         size_of_timer   = sizeof(uint32);
  boolean                       status = TRUE;
 

  /* Read RPM SYS TIME from NV-EFS */
  cmnv_efs_read(CMNV_EFS_ID_RPM_SYS_TIME,
                (byte *)&sys_time_pwr_up,
                 &size_of_timer);

  CM_MSG_HIGH_3("system time read at power-up is %d, curr_systime:%d is_update:%d",
                         sys_time_pwr_up, cm_util_get_curr_systime(), is_update);
  
  /* If time read from EFS(time stored at last power-down) is greater than the 
  ** uptime(time since recent power-up), we can consider time is not persistent
  ** across resets and coin cell battery support is not there. 
  ** In this case, update time to local variable and set is_coin_cell_support 
  ** to FALSE
  */
  if((is_update == TRUE) && size_of_timer > 0 && sys_time_pwr_up > cm_util_get_curr_systime())
  {
    rpm_ptr->is_coin_cell_support = FALSE;
    rpm_ptr->sys_time_efs = (uint32)sys_time_pwr_up;
  }
  else if(size_of_timer < 0)
  {
    status = FALSE;
  }
  CM_MSG_HIGH_2("rpm_ptr->sys_time_efs:%d coin_cell_support:%d",
                                           rpm_ptr->sys_time_efs,
                                           rpm_ptr->is_coin_cell_support);
  return status;
}/* cmrpm_read_efs_sys_time() */

/*===========================================================================

FUNCTION cmrpm_write_efs_sys_time

DESCRIPTION
  This function writes system time at power-down in EFS

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_write_efs_sys_time(
  sys_oprt_mode_e_type  prev_oprt_mode,

  sys_oprt_mode_e_type  curr_oprt_mode

)
{
  boolean                  status = TRUE;
  
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
  uint32                        sys_time_pwr_down;
 
  CM_MSG_HIGH_6("CMRPM: oprt_mode_chg, prev %d curr %d rpm_enable %d sys_time_efs %d coin_cell_support %d time %d",
                prev_oprt_mode, curr_oprt_mode, cmrpm_is_rpm_enabled(),
                rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support,
                cm_util_get_curr_systime());
  /* Check if oprt mode switch is valid and CMRPM is enabled. If so and 
  ** no coin cell battery support, write current system time to EFS
  */
  if(
     #ifndef FEATURE_MMODE_RPM_TDSCDMA
      (TRUE == cmrpm_is_rpm_enabled()) &&
     #endif 
      prev_oprt_mode == SYS_OPRT_MODE_ONLINE && 
      ((curr_oprt_mode == SYS_OPRT_MODE_OFFLINE)||
       (curr_oprt_mode == SYS_OPRT_MODE_OFFLINE_CDMA)||
       (curr_oprt_mode == SYS_OPRT_MODE_LPM)||
       (curr_oprt_mode == SYS_OPRT_MODE_FTM)))
  {
    if(!cmrpm_read_efs_sys_time(FALSE))
    {
      CM_ERR_0("Can't read RPM EFS INFO");
    }
    else if(!(rpm_ptr->is_coin_cell_support == TRUE &&
              (rpm_ptr->sys_time_efs == cm_util_get_curr_systime())))
    {
      /* Current system time = time from EFS at power-up + time since power-up
      */
      sys_time_pwr_down = (rpm_ptr->sys_time_efs) + cm_util_get_curr_systime();
      CM_MSG_HIGH_1("Write time %d to EFS", sys_time_pwr_down);
	 
      cmnv_efs_write(CMNV_EFS_ID_RPM_SYS_TIME,
                     (byte *)&sys_time_pwr_down,
                      sizeof(uint32));
      status = TRUE;
 
   }
  }

  return status;

}/* cmrpm_write_efs_sys_time() */

/*===========================================================================

FUNCTION cmrpm_reset_sim

DESCRIPTION
  This function is called to do sim power down and power up.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static boolean cmrpm_reset_sim(void)
{
  /*reuse the CM's client_id*/
  if(mmgsdi_card_pdown(cm_mmgsdi_get_client_id(),
                       MMGSDI_SLOT_1,
                       cmrpm_gsdi_lpm_cb,
                       MMGSDI_CARD_POWER_DOWN_NOTIFY_GSDI,
                       0 ) != MMGSDI_SUCCESS
    )
  {
    CM_ERR_0("Unable to power-down card");
  }
  /* Send card power-up cmd to MMGSDI.
  */
  if(mmgsdi_card_pup(cm_mmgsdi_get_client_id(),
                     MMGSDI_SLOT_1,
                     cmrpm_gsdi_lpm_cb,
                     MMGSDI_CARD_POWER_UP_INITIAL_PUP,
                     0 ) != MMGSDI_SUCCESS
    )
  {
    CM_ERR_0("Unable to power-up card");
  }
  return TRUE;
}

/*===========================================================================

FUNCTION cmrpm_read_efs_info

DESCRIPTION
  This function reads RPM parameter from EFS

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_read_efs_info(cmrpm_efs_info_s_type *rpm_info_ptr)
{
  boolean                  status = TRUE;
  int32                    size_of_efs_info = sizeof(cmrpm_efs_info_s_type);  

  /* Read RPM INFO from NV-EFS */
  cmnv_efs_read(CMNV_EFS_ID_RPM_INFO,
                (byte *)rpm_info_ptr,
                &size_of_efs_info);

  if(size_of_efs_info < 0)
  {
    status = FALSE;
  }  

  return status;
}

/*===========================================================================

FUNCTION cmrpm_write_efs_info

DESCRIPTION
  This function writes RPM parameter into EFS

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_write_efs_info(const cmrpm_s_type   *rpm_ptr )
{
  boolean                            status = FALSE;
  cmrpm_efs_info_s_type              rpm_efs_info;

  if(!cmrpm_read_efs_info(&rpm_efs_info))
  {
    CM_ERR_0("Can't read RPM EFS INFO");
  }
  else
  {
    /* Update writable RPM parameter 
    ** Do not update other fixed parameter such as below
    ** is_rpm_enabled, app_max_num_reset, average_reset_time
    */
    rpm_efs_info.app_reset_counter = rpm_ptr->app_reset.counter;
    rpm_efs_info.app_reset_timer = rpm_ptr->app_reset.timer;
    rpm_efs_info.app_reset_block_leak_rate_timer = 
      rpm_ptr->app_reset.block_leak_rate_timer;
    rpm_efs_info.leak_rate_timer = 
      rpm_ptr->mmreject_reset.leak_rate_timer;
    
    
    cmnv_efs_write(CMNV_EFS_ID_RPM_INFO,
                  (byte *)&rpm_efs_info,
                  sizeof(cmrpm_efs_info_s_type));

    status = TRUE;

  }
  return status;

}

/*===========================================================================

FUNCTION cmrpm_check_sim_changed

DESCRIPTION
  Check if SIM has changed
  
DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  boolean 

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_check_sim_changed( void )
{ 
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
  boolean                       is_sim_changed = FALSE;

  #ifdef CM_GW_SUPPORTED
  if ((mmgsdi_rpm_has_iccid_changed(MMGSDI_SLOT_1, &is_sim_changed) 
      == MMGSDI_SUCCESS) && is_sim_changed)
  {
    CM_MSG_HIGH_0("SIM Changed!! Reset RPM counter/timer");
    
    rpm_ptr->app_reset.timer = 0;
    rpm_ptr->app_reset.counter = 0;
    rpm_ptr->app_reset.block_leak_rate_timer = 0;
    rpm_ptr->mmreject_reset.leak_rate_timer = 0;
  
    if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
    {
      CM_ERR_0("Can't write RPM EFS INFO");
      
    }

    return TRUE;
    
  }  
  else
  {
    return FALSE;
  }
  #else
  return FALSE;
  #endif
}

/*==========================================================================

FUNCTION cmrpm_read_imsi_hplmn

DESCRIPTION
  Function that reads EF-IMSI and return HPLMN(MCC-MNC)

RETURN VALUE
  boolean  Flag indicating whether EF-IMSI read
           from the SIM successfully (TRUE) or unsuccessfully(FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmrpm_read_imsi_hplmn
(
  sys_plmn_id_s_type *imsi_ptr
)
{
  #ifdef FEATURE_MMGSDI_SESSION_LIB  
  boolean                       status    = FALSE;
  mmgsdi_app_enum_type          app_type;
  byte                          imsi[MMGSDI_IMSI_LEN] = {0};
  byte                          admin_data[CMMMGSDI_RPM_AD_SIZE] = {0};


  app_type = cm_mmgsdi_ssn_tbl_get_app_type_for_type(
             MMGSDI_GW_PROV_PRI_SESSION);
 
  /* Read EF-IMSI */
  status = cmmmgsdi_read(MMGSDI_GW_PROV_PRI_SESSION,
             (app_type == MMGSDI_APP_SIM ? MMGSDI_GSM_IMSI : MMGSDI_USIM_IMSI),
             0,
             MMGSDI_IMSI_LEN,
             (uint8*)imsi,
             sizeof(imsi),
             transparent);
  
  /* Read EF-AD to get MNC length */
  if(status)
  {
    status = cmmmgsdi_read(MMGSDI_GW_PROV_PRI_SESSION,
              (app_type == MMGSDI_APP_SIM ? MMGSDI_GSM_AD : MMGSDI_USIM_AD),
              0,
              CMMMGSDI_RPM_AD_SIZE,
              (uint8*)admin_data, 
              sizeof(admin_data),
              transparent);      
  }

  /* Extract MCC and MNC */
  if(status)
  {
    /*
    ** Fill MCC Digit 1 and MCC Digit 2 
    */
    imsi_ptr->identity[0] = (byte)(( ( imsi[1] & 0xF0 ) / 0x10 ) +
                                   ( ( imsi[2] & 0x0F ) * 0x10 ));

    /*
    ** Fill MCC Digit 3 and MNC Digit 3 
    */
    imsi_ptr->identity[1] = (byte)(( ( imsi[2] & 0xF0 ) / 0x10 ) +
                                   ( ( imsi[4] & 0x0F ) * 0x10 ));

    /*
    ** If 2 digits MNC is indicated in EF-AD then set MNC digit 3 to 'F'. 
    */
    if ((admin_data[3] & 0x0F) == 0x02)
    {
        CM_MSG_MED_0("2 digit MNC");

      imsi_ptr->identity[1] |= 0xF0;
    }

    /*
    ** Fill MNC Digit 1 and MNC Digit 2 
    */
    imsi_ptr->identity[2] = imsi[3];

    CM_MSG_HIGH_3("Read IMSI - HPLMN : 0x%02x, 0x%02x, 0x%02x",
                  imsi_ptr->identity[0],imsi_ptr->identity[1],imsi_ptr->identity[2]);
  }

  return status;
  #else 

  return FALSE;
  #endif

} /* cmrpm_read_imsi_hplmn() */

/*==========================================================================

FUNCTION cmrpm_read_rpm_enabled_flag

DESCRIPTION
  Function that reads EF-RPM Enabled Flag

RETURN VALUE
  boolean  Flag indicating whether EF-RPM Enabled Flag read
           from the SIM successfully (TRUE) or unsuccessfully(FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmrpm_read_rpm_enabled_flag
(
  boolean                        *enabled_flag_ptr
)
{
  #ifdef FEATURE_MMGSDI_SESSION_LIB  
  boolean                       status    = FALSE;
   
  /*
  ** Read the RPM Enabled Flag
  */
  status = cmmmgsdi_read(MMGSDI_GW_PROV_PRI_SESSION,
                         MMGSDI_USIM_7F66_PROP1_RPM_EF,
                         0,
                         CMMMGSDI_RPM_ENABLED_FLAG_SIZE,
                         (uint8*)enabled_flag_ptr,
                         sizeof(boolean),
                         transparent);
  

  return status;
  #else

  return FALSE;
  #endif

} /* cmrpm_read_rpm_enabled_flag() */

/*==========================================================================

FUNCTION cmrpm_read_rpm_parameters

DESCRIPTION
  Function that reads EF-RPM Parameters, and parse N1, and T1. 
  N1 - Max number of application triggered resets allowed per hour. 
  T1 - Average time before RPM resets modem following MM/GMM reject. 

RETURN VALUE
  boolean  Flag indicating whether EF-RPM Parameters read
           from the SIM successfully (TRUE) or unsuccessfully(FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmrpm_read_rpm_parameters
(
  uint8                           *max_num_app_resets_ptr,
  uint8                           *avg_mmreject_reset_time_ptr
)
{
   #ifdef FEATURE_MMGSDI_SESSION_LIB  
  boolean                       status    = FALSE;
  uint8                         *read_data_ptr;


  /* Allocate memory for rpm_parameters */
  read_data_ptr = (uint8*)cm_mem_malloc(
                     sizeof(uint8) * CMMMGSDI_RPM_PARAMETERS_SIZE);
   
  /* Read the RPM Parameters */
  status = cmmmgsdi_read(MMGSDI_GW_PROV_PRI_SESSION,
                         MMGSDI_USIM_7F66_PROP1_RPM_PARAM,
                         0,
                         CMMMGSDI_RPM_PARAMETERS_SIZE,
                         (uint8*)read_data_ptr,
                         (sizeof(uint8) * CMMMGSDI_RPM_PARAMETERS_SIZE),
                         transparent);

  if(status == TRUE)
  {
    /* Parse N1 and T1 from RPM parameters */
    *max_num_app_resets_ptr = read_data_ptr[0] & 0xFF; 
    *avg_mmreject_reset_time_ptr = read_data_ptr[1] & 0xFF;

    CM_MSG_HIGH_2("RPM - N1: %d, T1: %d", 
      *max_num_app_resets_ptr, *avg_mmreject_reset_time_ptr );
  }

  /* Free rpm_param_ptr */
  cm_mem_free(read_data_ptr);
  
  return status;
  #else
  return FALSE;
  #endif

} /* cmrpm_read_rpm_parameters */

/*==========================================================================

FUNCTION cmrpm_read_rpm_omclr

DESCRIPTION
  Function that reads EF-RPM OMCLR(Operational Management Counters Leak Rate)
  and parse LR-1 and LR-2
  LR-1 - Leak rate for C-BR-1
  LR-2 - Leak rate for C-R-1

RETURN VALUE
  boolean  Flag indicating whether EF-RPM OMCLR read
           from the SIM successfully (TRUE) or unsuccessfully(FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmrpm_read_rpm_omclr
(
  uint8                           *leak_rate_1_ptr,
  uint8                           *leak_rate_2_ptr
)
{
   #ifdef FEATURE_MMGSDI_SESSION_LIB  
  boolean                       status    = FALSE;
  uint8                         *read_data_ptr;


  /* Allocate memory for RPM OMCLR */
  read_data_ptr = (uint8*)cm_mem_malloc(
                     sizeof(uint8) * CMMMGSDI_RPM_OMCLR_SIZE);
   
  /* Read the RPM OMCLR */
  status = cmmmgsdi_read(MMGSDI_GW_PROV_PRI_SESSION,
                         MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR,
                         0,
                         CMMMGSDI_RPM_OMCLR_SIZE,
                         (uint8*)read_data_ptr,
                         (sizeof(uint8) * CMMMGSDI_RPM_OMCLR_SIZE),
                         transparent);

  if(status == TRUE)
  {
    /* Parse LR1 and LR2 from RPM parameters */
    *leak_rate_1_ptr = read_data_ptr[0] & 0xFF; 
    *leak_rate_2_ptr = read_data_ptr[1] & 0xFF;
  }    

  CM_MSG_HIGH_2("RPM - LR1: %d, LR2: %d", 
    *leak_rate_1_ptr, *leak_rate_2_ptr );

  /* Free rpm_param_ptr */
  cm_mem_free(read_data_ptr);
  
  return status;
  #else
  return FALSE;
  #endif

} /* cmrpm_read_rpm_omclr */

/*==========================================================================

FUNCTION cmrpm_read_rpm_omc

DESCRIPTION
  Function that reads EF-RPM OMC(Operational Management Counters)
  and parse C-BR-1 and C-R-1
  C-BR-1: Counter related to N1. # of blocked app-initiated resets
  C-R-1: Counter related to T1. # of mmreject-intiated resets

RETURN VALUE
  boolean  Flag indicating whether EF-RPM OMC read
           from the SIM successfully (TRUE) or unsuccessfully(FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmrpm_read_rpm_omc
(
  uint8                           *block_reset_counter_ptr,
  uint8                           *reset_counter_ptr
)
{
  #ifdef FEATURE_MMGSDI_SESSION_LIB  
  boolean                       status    = FALSE;
  uint8                         *read_data_ptr;

  /* Allocate memory for RPM OMC */
  read_data_ptr = (uint8*)cm_mem_malloc(
                     sizeof(uint8) * CMMMGSDI_RPM_OMC_SIZE);
   
  /* Read the RPM OMC */
  status = cmmmgsdi_read(MMGSDI_GW_PROV_PRI_SESSION,
                         MMGSDI_USIM_7F66_PROP1_RPM_CNTRS,
                         0,
                         CMMMGSDI_RPM_OMC_SIZE,
                         (uint8*)read_data_ptr,
                         (sizeof(uint8) * CMMMGSDI_RPM_OMC_SIZE),
                         transparent);

  if(status == TRUE)
  {
    /* Parse C-BR-1 and C-R-1 from RPM parameters */
    *block_reset_counter_ptr = read_data_ptr[0] & 0xFF; 
    *reset_counter_ptr = read_data_ptr [1] & 0xFF;
  }  

  CM_MSG_HIGH_2("RPM - C-BR-1: %d, C-R-1: %d", 
     *block_reset_counter_ptr, *reset_counter_ptr );

  /* Free rpm_param_ptr */
  cm_mem_free(read_data_ptr);
  
  return status;

#else
  return FALSE;
#endif
} /* cmrpm_read_rpm_omc */

/*==========================================================================

FUNCTION cmrpm_write_rpm_omc

DESCRIPTION
  Function that writes EF-RPM OMC(Operational Management Counters)  
  C-BR-1: Counter related to N1. # of blocked app-initiated resets
  C-R-1: Counter related to T1. # of mmreject-intiated resets

RETURN VALUE
  boolean  Flag indicating whether EF-RPM OMC write
           from the SIM successfully (TRUE) or unsuccessfully(FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmrpm_write_rpm_omc
(
  const uint8                      block_reset_counter,
  const uint8                      reset_counter
)
{
  #ifdef CM_GW_SUPPORTED
  #ifdef FEATURE_MMGSDI_SESSION_LIB  
  boolean                       status    = FALSE;
  uint8*                        write_data_ptr;
  mmgsdi_write_data_type        write_data;


  /* Allocate memory for RPM OMC */
  write_data_ptr = (uint8*)cm_mem_malloc(
                     sizeof(uint8) * CMMMGSDI_RPM_OMC_N1_T1_SIZE);
   
  /* Initialize the buffer */
  (void)memset(write_data_ptr, 0, 
               (sizeof(uint8)) * CMMMGSDI_RPM_OMC_N1_T1_SIZE);

    /* Update only N1 and T1 */
    write_data_ptr[0] = block_reset_counter;
    write_data_ptr[1] = reset_counter; 

    CM_MSG_HIGH_2("RPM - Update C-BR-1: %d, C-R-1: %d", 
      block_reset_counter, reset_counter );

    /* Write EF-RPM OMC */
    write_data.data_len = CMMMGSDI_RPM_OMC_N1_T1_SIZE;
    write_data.data_ptr = (uint8*) write_data_ptr;
    status = cmmmgsdi_write(cmph_map_cm_ss_to_subs(CM_SS_MAIN), 
                            MMGSDI_USIM_7F66_PROP1_RPM_CNTRS,                             
                            CM_MMGSDI_SUBS_TYPE_GW,
                            write_data);    


  /* Free rpm_param_ptr */
  cm_mem_free(write_data_ptr);
  
  return status;
  #else
  return FALSE;
  #endif

  #else
  return FALSE;
  #endif /* CM_GW_SUPPORTED */

} /* cmrpm_write_rpm_omc */



#if 0
/*==========================================================================

FUNCTION cmrpm_write_rpm_version

DESCRIPTION
  Function that writes EF-RPM Version Implemented during power up.

RETURN VALUE
  boolean  Flag indicating whether EF-RPM RPM Version write
           from the SIM successfully (TRUE) or unsuccessfully(FALSE).

DEPENDENCIES
  None
===========================================================================*/
static boolean cmrpm_write_rpm_version
(
  const uint8                      rpm_version
)
{

#ifdef FEATURE_MMGSDI_SESSION_LIB  
  boolean                       status    = FALSE;
  mmgsdi_write_data_type        write_data;

  /* Write EF-RPM VERSION Implemented */
  write_data.data_len = CMMMGSDI_RPM_VERSION_SIZE;
  write_data.data_ptr = (uint8*)&rpm_version;

  CM_MSG_HIGH_1("RPM - Update version : %d", rpm_version);
  
  status = cmmmgsdi_write(CM_SS_MAIN, 
                          MMGSDI_USIM_7F66_PROP1_RPM_VI, 
                          CM_MMGSDI_SUBS_TYPE_GW,
                          write_data);


  return status;
  #else
  return FALSE;
  #endif
  
}/* cmrpm_write_rpm_version */
#endif
/*===========================================================================

FUNCTION cmrpm_is_rpm_sim

DESCRIPTION
  This function checks if SIM is rpm sim or not

DEPENDENCIES
  MMGSDI should be initialized prior to this function

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmrpm_is_rpm_sim(void)
{

  boolean                       status = FALSE;
  sys_plmn_id_s_type            hplmn;
  int32                         i;
  sys_plmn_id_s_type            rpm_plmns;
  int32                         mnc_mcc_list_size;
  cmrpm_supported_mcc_mnc_s_type* mcc_mnc_list_ptr;

  /* Read HPLMN from IMSI */
  if(cmrpm_read_imsi_hplmn(&hplmn) == FALSE)
  {    
    CM_ERR_0("Can't read IMSI");    
  }
  else
  { 
  
    /* Allocate memory for mcc/mnc list for RPM supported SIM
    ** and initialize it 
    */  
    mnc_mcc_list_size = CMRPM_MAX_SIM_SUPPORTED
                          * sizeof(cmrpm_supported_mcc_mnc_s_type);
    
    mcc_mnc_list_ptr = \
       (cmrpm_supported_mcc_mnc_s_type*)cm_mem_malloc(
        sizeof(cmrpm_supported_mcc_mnc_s_type) * CMRPM_MAX_SIM_SUPPORTED);
  
    /* Read RPM supported IMSI List from EFS_NV
    ** and compare if Current IMSI is match with any of Supported IMSI list
    */
    cmnv_efs_read(CMNV_EFS_ID_RPM_MCC_MNC_LIST,
                 (byte*)mcc_mnc_list_ptr,
                 &mnc_mcc_list_size);

    if(mnc_mcc_list_size <= 0)
    {
       CM_ERR_0("Can't read RPM MCC_MNC List");
    }
    else
    {
  
      for(i=0; i < CMRPM_MAX_SIM_SUPPORTED; i++)
      {
        (void)memset(&rpm_plmns, 0, sizeof(sys_plmn_id_s_type));
  
        /* Covert mcc/mnc to sys_plmn format */
        rpm_plmns.identity[0] = (byte)(mcc_mnc_list_ptr[i].mcc[0] + 
                                       mcc_mnc_list_ptr[i].mcc[1] * 0x10);
        rpm_plmns.identity[1] = (byte)(mcc_mnc_list_ptr[i].mcc[2] + 
                                       mcc_mnc_list_ptr[i].mnc[2] * 0x10);
        rpm_plmns.identity[2] = (byte)(mcc_mnc_list_ptr[i].mnc[0] + 
                                       mcc_mnc_list_ptr[i].mnc[1] * 0x10);

        CM_MSG_LOW_3("RPM MCC-MNC : 0x%02x, 0x%02x, 0x%02x", 
          rpm_plmns.identity[0], rpm_plmns.identity[1], rpm_plmns.identity[2]);
      
        if(mcc_mnc_list_ptr[i].mnc[2] == 0x0F)
        {
          rpm_plmns.identity[1] |= 0xF0;
        }
      
        /* Check if IMSI is match with any of RPM MCC_MNC list */
        if(sys_plmn_match(rpm_plmns, hplmn))
        {
          /* PLMN matched, return true */
          status = TRUE;
          break;
        } 
      }    
    }
  
    /* Free rpm_param_ptr */
    cm_mem_free(mcc_mnc_list_ptr); 
  }


  return status;

}

/*===========================================================================

FUNCTION cmrpm_check_timer_expired

DESCRIPTION
  Check if RPM timers are expired. 
  
DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmrpm_check_timer_expired( void )
{
  #ifdef CM_GW_SUPPORTED
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
  uint32                        curr_system_time   = cm_util_get_curr_systime();

  CM_MSG_MED_3("check_timer: rpm_ptr->sys_time_efs:%d, coin_cell_support:%d, curr_systime:%d",
               rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support, curr_system_time);
  
  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_system_time = (rpm_ptr->sys_time_efs) + curr_system_time;
  }

  /* 
  ** Check APP_RESET_TIMER if it's already expired or not 
  */
  if((rpm_ptr->app_reset.timer != 0) && 
     (rpm_ptr->app_reset.timer <= curr_system_time))
  {
    CM_MSG_HIGH_2("App Reset Timer expired - curr_systime:%d, timer:%d",
      curr_system_time, rpm_ptr->app_reset.timer);
    
    /* Update APP_RESET_TIMER to 0 */
    rpm_ptr->app_reset.timer = 0;
    if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
    {
      CM_ERR_0("Fail to write APP_RESET_TIMER ");
    }    
  } 

  /* 
  ** Check APP_RESET_BLOCK_LR_TIMER if it's already expired or not 
  */
  if(rpm_ptr->app_reset.block_leak_rate_hour!=0)
  {
    if((rpm_ptr->app_reset.block_leak_rate_timer != 0) &&
       (rpm_ptr->app_reset.block_leak_rate_timer <= curr_system_time))
    {        
      /* Calculate expired count and remainig time */
      uint8   block_lr_timer_expired_count;
      uint32  block_lr_timer_remaining_time;
      uint32  elapsed_block_lr_time;
  
      elapsed_block_lr_time = curr_system_time - 
        rpm_ptr->app_reset.block_leak_rate_timer;
  
      block_lr_timer_expired_count = (uint8)(elapsed_block_lr_time /
        (rpm_ptr->app_reset.block_leak_rate_hour * CMRPM_SEC_PER_HOUR)) + 1;
  
      block_lr_timer_remaining_time = 
        (rpm_ptr->app_reset.block_leak_rate_hour * CMRPM_SEC_PER_HOUR) - 
        (elapsed_block_lr_time %
        (rpm_ptr->app_reset.block_leak_rate_hour * CMRPM_SEC_PER_HOUR));
      
      CM_MSG_HIGH_4("Block LR Timer expired-curr_systime:%d, timer:%d, expired_count:%d, remaining_time:%d",
        curr_system_time, rpm_ptr->app_reset.block_leak_rate_timer,block_lr_timer_expired_count, block_lr_timer_remaining_time);
  
      /* Update C-BR-1 */
      rpm_ptr->app_reset.block_counter -= block_lr_timer_expired_count;
      if(!cmrpm_write_rpm_omc(rpm_ptr->app_reset.block_counter,
                             rpm_ptr->mmreject_reset.counter))
      {
        CM_ERR_0("Fail to write EF-RPM OMC ");
      }
  
      /* Update APP_RESET_BLOCK_LR_TIMER*/
      if(rpm_ptr->app_reset.block_counter > 0) 
      {
        /* If the block_counter is greater than 0, 
        ** keep running the timer for the duration of the remaining time */
        rpm_ptr->app_reset.block_leak_rate_timer = 
          curr_system_time + block_lr_timer_remaining_time;
      }
      else
      {
        /* If the block counter is 0, do not run timer */
        rpm_ptr->app_reset.block_leak_rate_timer = 0;
      }
  
      if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
      {
        CM_ERR_0("Fail to write APP_RESET_BLOCK_LR_TIMER ");
      }
      
    }    
    else
    {
      /* When phone powers up, we have valid LR-1, but the timer is not running,
      ** then we start the LR-1 timer. In real case, the timer should have been 
      ** started whenever C-BR-1 is increased, so this case shouldn't happen.
      ** However, in the test case, it writes both C-BR-1 and LR-1 in the SIM and
      ** power up the phone and expect LR-1 timer is running. We are adding this 
      ** to cover the test case. 
      */
      if((rpm_ptr->app_reset.block_leak_rate_timer == 0) &&
         (rpm_ptr->app_reset.block_counter > 0))
      {
        rpm_ptr->app_reset.block_leak_rate_timer = curr_system_time +
              (rpm_ptr->app_reset.block_leak_rate_hour * CMRPM_SEC_PER_HOUR);
  
        if(cmrpm_write_efs_info(rpm_ptr))
        {
          CM_MSG_MED_1("Update block_leak_rate_timer : %d",
            rpm_ptr->app_reset.block_leak_rate_timer);
        }
        else
        {
          CM_ERR_0("Can't write APP_RESET_BLOCK_LR_TIMER");   
        }        
    
      }
    }
  }


  /* 
  ** Check RESET_LR_TIMER if it's already expired or not 
  */
  if(rpm_ptr->mmreject_reset.leak_rate_hour != 0)
  {
    if((rpm_ptr->mmreject_reset.leak_rate_timer!= 0) &&
       (rpm_ptr->mmreject_reset.leak_rate_timer <= curr_system_time))
    {     
      /* Calculate expired count and remainig time */
      uint8   lr_timer_expired_count;
      uint32  lr_timer_remaining_time;
      uint32  elapsed_lr_time;
  
      elapsed_lr_time = curr_system_time - 
           rpm_ptr->mmreject_reset.leak_rate_timer;
  
      lr_timer_expired_count = (uint8)(elapsed_lr_time /
        (rpm_ptr->mmreject_reset.leak_rate_hour * CMRPM_SEC_PER_HOUR)) + 1;
  
      lr_timer_remaining_time = 
        (rpm_ptr->mmreject_reset.leak_rate_hour * CMRPM_SEC_PER_HOUR) - 
        (elapsed_lr_time %
        (rpm_ptr->mmreject_reset.leak_rate_hour * CMRPM_SEC_PER_HOUR));
  
      CM_MSG_HIGH_4("L R Timer expired-curr_systime:%d, timer:%, expired_count:%d, remaining_time:%d",
        curr_system_time, rpm_ptr->mmreject_reset.leak_rate_timer,lr_timer_expired_count, lr_timer_remaining_time);
      
  
      /* Update C-R-1 */
      rpm_ptr->mmreject_reset.counter -= lr_timer_expired_count;
      if(!cmrpm_write_rpm_omc(rpm_ptr->app_reset.block_counter,
                             rpm_ptr->mmreject_reset.counter))
      {
        CM_ERR_0("Fail to write EF-RPM OMC ");
      }
  
  
      /* Update RESET_LR_TIMER*/
      if(rpm_ptr->mmreject_reset.counter > 0) 
      {
        /* If the reset counter is greater than 0, 
        ** keep running the timer for the duration of the remaining time */
        rpm_ptr->mmreject_reset.leak_rate_timer = 
          curr_system_time + lr_timer_remaining_time;
      }
      else
      {
        /* If the block counter is 0, do not run timer */
        rpm_ptr->mmreject_reset.leak_rate_timer = 0;
      }
  
      if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
      {
        CM_ERR_0("Fail to write RPM_RESET_LR_TIMER ");
      }
      
    }    
    else
    {
      /* When phone powers up, we have valid LR-2, but the timer is not running,
      ** then we start the LR-2 timer. In real case, the timer should have been 
      ** started whenever C-R-1 is increased, so this case shouldn't happen.
      ** However, in the test case, it writes both C-R-1 and LR-2 in the SIM and
      ** power up the phone and expect LR-2 timer is running. We are adding this 
      ** to cover the test case. 
      */
      if((rpm_ptr->mmreject_reset.leak_rate_timer == 0) &&
         (rpm_ptr->mmreject_reset.counter > 0))
      {
        rpm_ptr->mmreject_reset.leak_rate_timer = curr_system_time + 
          (rpm_ptr->mmreject_reset.leak_rate_hour * CMRPM_SEC_PER_HOUR);
  
        if(cmrpm_write_efs_info(rpm_ptr))
        {
          CM_MSG_MED_1("Update leak_rate_timer(LR2) : %d",
            rpm_ptr->mmreject_reset.leak_rate_timer);
        }
        else
        {
          CM_ERR_0("Can't write LR2_TIMER");           
        }       
      }
    }
  }  

  #endif /* CM_GW_SUPPORTED */
}
/*===========================================================================

FUNCTION cmrpm_process_app_reset

DESCRIPTION
  Process oprt mode change 

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  TRUE if reset counter is incremented

SIDE EFFECTS
  None
===========================================================================*/
  boolean  cmrpm_process_app_reset(
  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_ph_cmd_info_s_type          *cmd_info_ptr = NULL;
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
  int i;

  static const struct {
    
          sys_oprt_mode_e_type    from_oprt_mode;
              /* from operating mode */
    
          sys_oprt_mode_e_type    to_oprt_mode;
              /* to operating mode */
    
      } cmrpm_oprt_mode_tbl[] = {
    
            /* from oprt mode */         /* to oprt mode */
            {SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_OFFLINE},
            {SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_OFFLINE_CDMA},
            {SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_LPM},
            {SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_FTM}
           
                };
  
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /* App inititated resets for oprt mode OFFLINE/LPM/OFFLINE_CDMA are 
  ** counted for rpm resets
  */

  if(rpm_ptr->is_rpm_enabled &&
      (
     (cmd_info_ptr->oprt_mode == SYS_OPRT_MODE_OFFLINE) ||
       (cmd_info_ptr->oprt_mode == SYS_OPRT_MODE_LPM) ||
       (cmd_info_ptr->oprt_mode == SYS_OPRT_MODE_OFFLINE_CDMA) || 
       (cmd_info_ptr->oprt_mode == SYS_OPRT_MODE_FTM)))
    {
      boolean valid_oprt_change = FALSE;
      for( i=0; i < ARR_SIZE(cmrpm_oprt_mode_tbl); i++ )
      {
        if( cmrpm_oprt_mode_tbl[i].from_oprt_mode == cmph_ptr()->oprt_mode &&
            cmrpm_oprt_mode_tbl[i].to_oprt_mode   == cmd_info_ptr->oprt_mode )
        {
          valid_oprt_change =  TRUE;
        }
      }
      
        /* If RPM APP RESET TIMER is not running, we don't need to track the 
        ** app reset counter nor checking reset allow or not 
        */
        if((rpm_ptr->app_reset.timer != 0) && (valid_oprt_change == TRUE))
        {
          if(rpm_ptr->app_reset.counter 
              < rpm_ptr->app_reset.max_num_reset)
          {
              /* app reset counter has not been reached the max reset allowed 
              ** - increament app_reset_counter
              ** - allow reset 
              */
            rpm_ptr->app_reset.counter += 1;
            CM_MSG_HIGH_1("RESET Allowed, app_reset_counter:%d", 
                rpm_ptr->app_reset.counter);

            if(cmrpm_write_efs_info(rpm_ptr) ==FALSE )
            {
              CM_ERR_0("Can't write APP_RESET_COUNTER");     
            }
            return TRUE;
          }
            
          
        }
        
    }
  
    return FALSE;

}

/*===========================================================================

FUNCTION cmrpm_app_reset_rejected_proc

DESCRIPTION
  Processes PH cmd from CM when app reset was rejected 

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/

void cmrpm_app_reset_rejected_proc( void )

{
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
  uint32                        curr_sys_time = cm_util_get_curr_systime();

  CM_MSG_MED_3("app_reset_reject: rpm_ptr->sys_time_efs:%d, coin_cell_support:%d, curr_systime:%d",
               rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support, curr_sys_time );
  
  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */	      
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_sys_time = (rpm_ptr->sys_time_efs) + curr_sys_time;
  }


 if(rpm_ptr->is_rpm_enabled )
  {
    
    /* If RPM APP RESET TIMER is not running, we don't need to track the 
    ** app reset counter nor checking reset allow or not 
    */
    if(rpm_ptr->app_reset.timer != 0)
    {
      if(rpm_ptr->app_reset.counter 
          >= rpm_ptr->app_reset.max_num_reset)
      {

        /* If the app reset counter is reached the max reset allowed,
        **  - increment C-BR-1(app reset block counter) 
        **  - start LR-1 timer 
        **  - do not allow reset(return error)
        */ 
        rpm_ptr->app_reset.block_counter += 1;        
        if(cmrpm_write_rpm_omc(rpm_ptr->app_reset.block_counter,
                                   rpm_ptr->mmreject_reset.counter))
        {
           CM_MSG_MED_4("Update RPM OMC - block_counter:%d, reset_counter:%d,max_num_reset :%d timer:%d",
                        rpm_ptr->app_reset.block_counter,
                        rpm_ptr->mmreject_reset.counter,rpm_ptr->app_reset.max_num_reset,rpm_ptr->app_reset.timer);                   
        }
        else
        {
           CM_ERR_2("Fail to write EF-RPM OMC max_num_reset :%d timer:%d",rpm_ptr->app_reset.max_num_reset,
           rpm_ptr->app_reset.timer);
        }
      

      
        if((rpm_ptr->app_reset.block_leak_rate_hour != 0) &&
           (rpm_ptr->app_reset.block_leak_rate_timer == 0 ))
        {
          rpm_ptr->app_reset.block_leak_rate_timer = curr_sys_time +
            (rpm_ptr->app_reset.block_leak_rate_hour * CMRPM_SEC_PER_HOUR);

          if(cmrpm_write_efs_info(rpm_ptr))
          {
            CM_MSG_MED_1("Update block_leak_rate_timer : %d",
              rpm_ptr->app_reset.block_leak_rate_timer);
          }
          else
          {
            CM_ERR_0("Can't write APP_RESET_BLOCK_LR_TIMER");   
          }        
        }

        /* If T1 timer was running 
        ** reset T1 timer as well when app reset is blocked 
        */
        if((rpm_ptr->mmreject_reset.timer != 0) &&
          (rpm_ptr->mmreject_reset.average_reset_time != 0))
          {
            rpm_ptr->mmreject_reset.timer = curr_sys_time +
              (rpm_ptr->mmreject_reset.average_reset_time 
               * CMRPM_MIN_PER_T1 * CMRPM_SEC_PER_MIN);
    
            CM_MSG_HIGH_2("MM Reject reset timer is running: %d, curr_systime:%d",
              rpm_ptr->mmreject_reset.timer, curr_sys_time);
          }      

        
      }
       
     }
   }

}
/*===========================================================================

FUNCTION cmrpm_check_reset_allowed

DESCRIPTION
  Check whether reset is allowed or not when RPM is on

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
TRUE if reset is allowed

SIDE EFFECTS
  None
===========================================================================*/
boolean  cmrpm_check_reset_allowed(
  void
)
 {
  boolean            ret_val      = TRUE;
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
  

  /* Internal CM reset(CLIENT_ID_ANONYMOUS) should be always allowed 
  ** All other reset should be checked before proceeing 
        */

  if(rpm_ptr->is_rpm_enabled )
  {
    
      /* If RPM APP RESET TIMER is not running, we don't need to track the 
      ** app reset counter nor checking reset allow or not 
          */
      if(rpm_ptr->app_reset.timer != 0)
      {
        if(rpm_ptr->app_reset.counter 
            >= rpm_ptr->app_reset.max_num_reset)
        {
          CM_MSG_HIGH_2("App Reset Not Allowed - max_num_reset :%d timer:%d",
            rpm_ptr->app_reset.max_num_reset,
            rpm_ptr->app_reset.timer);
  
           ret_val = FALSE; 
          
        }
        else
        {
          CM_MSG_HIGH_0("App Reset Allowed");
        }
      }
      
    }

  return ret_val;
}

/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/  
       
/*===========================================================================

FUNCTION cmrpm_ptr

DESCRIPTION
  Return a pointer to the one and only RPM object.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmrpm_s_type *cmrpm_ptr( void )
{
  static cmrpm_s_type    cmrpm_local;       /* The one and only rpm object */

  return &cmrpm_local;

} /* *cmrpm_ptr() */

/*===========================================================================

FUNCTION cmrpm_is_rpm_enabled

DESCRIPTION
  Returns whether rpm is enabled, based on the rpm pointer.
  Meant for use by external functions that don't have much to do with RPM
  but need to know if it is enabled.

DEPENDENCIES
  cmrpm_init should be called before this function.

RETURN VALUE
  TRUE if the rpm pointer indicates that rpm is enabled
  FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_is_rpm_enabled( void )
{
  return (cmrpm_ptr()->is_rpm_enabled);
}


/*===========================================================================

FUNCTION cmrpm_init

DESCRIPTION
  Initializes the RPM object.

  This function must be called before the before RPM object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_init( void )
{
  boolean                       is_rpm_enabled = FALSE; 
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();

  /* Initialize all RPM parameters in ph_ptr */
  rpm_ptr->is_rpm_enabled = FALSE;
  rpm_ptr->app_reset.max_num_reset = 0;
  rpm_ptr->app_reset.counter = 0;
  rpm_ptr->app_reset.block_counter = 0;
  rpm_ptr->app_reset.block_leak_rate_hour = 0;
  rpm_ptr->app_reset.timer = 0; 
  rpm_ptr->app_reset.block_leak_rate_timer = 0;
  rpm_ptr->mmreject_reset.average_reset_time = 0;
  rpm_ptr->mmreject_reset.counter = 0;
  rpm_ptr->mmreject_reset.leak_rate_hour = 0;
  rpm_ptr->mmreject_reset.timer = 0;
  rpm_ptr->mmreject_reset.leak_rate_timer = 0;
  rpm_ptr->is_coin_cell_support = TRUE;
  rpm_ptr->sys_time_efs = 0;

  if(FALSE == (*(cmph_get_gprs_anite_gcf_ptr())))
  {
    /* RPM functionality is only enabled when GCF flag is OFF/FALSE */
    if(cmrpm_is_rpm_sim())
    {
      /* RPM functionality is only enabled when RPM SIM exist */
      CM_MSG_MED_0("RPM Initializing...");
      
      /* Read default system time from EFS at init*/
      cmrpm_read_efs_sys_time(TRUE);
      
      /* Read RPM parameter from SIM */
      if(cmrpm_read_rpm_parameters(
          &(rpm_ptr->app_reset.max_num_reset),
          &(rpm_ptr->mmreject_reset.average_reset_time)) != FALSE)         
      {
        /* If RPM parameters exist in SIM, enable the RPM functionality 
        ** based on RPM Enabled Flag in SIM 
        */
        if(cmrpm_read_rpm_enabled_flag(&is_rpm_enabled) && is_rpm_enabled)
        {
          rpm_ptr->is_rpm_enabled = TRUE;        
          
          /* Read RPM OMCLR(Operational Management Counters Leak Rate)M */
          if(cmrpm_read_rpm_omclr(
            (uint8*)&(rpm_ptr->app_reset.block_leak_rate_hour),
            (uint8*)&(rpm_ptr->mmreject_reset.leak_rate_hour))== FALSE)
          {
            CM_ERR_0("Can't read RPM OMCLR in SIM");
          }

          if(cmrpm_read_rpm_omc(
              (uint8*)&(rpm_ptr->app_reset.block_counter),
              (uint8*)&(rpm_ptr->mmreject_reset.counter)) == FALSE)
          {
            CM_ERR_0("Can't read RPM OMC in SIM");
          }
        }
        else
        {
          CM_MSG_HIGH_0("RPM Enabled Flag is OFF in SIM ");
        }             
      }
      else
      {         
        /* If RPM parameter doesn't exist enable RPM functionality based on 
        ** RPM Enabled Flag saved on device
        */       
        cmrpm_efs_info_s_type  rpm_efs_info;

        CM_MSG_MED_0("RPM Param not in SIM, read from EFS");
        
        if(cmrpm_read_efs_info(&rpm_efs_info))
        {
          if(rpm_efs_info.is_rpm_enabled == TRUE)
          {
            rpm_ptr->is_rpm_enabled = TRUE;
            rpm_ptr->app_reset.max_num_reset = rpm_efs_info.app_max_num_reset;
            rpm_ptr->mmreject_reset.average_reset_time = rpm_efs_info.average_reset_time;
          }
        }
        else
        {
          CM_MSG_HIGH_0("RPM Enabled Flag is OFF in EFS");
        }
      }
    }
    else
    {
      CM_MSG_HIGH_0("RPM is OFF - NO RPM SIM");
    }
  }
  else
  {
    /* RPM must be disabled if this GCF flag is ON/TRUE */
    CM_MSG_HIGH_0("RPM is OFF - GCF flag is ON");
  }

  /* If RPM is enabled, read all other permanent parameters from EFS */
  if(rpm_ptr->is_rpm_enabled)
  {
    
    /* Check if sim is changed then reset all permenent RPM parameters  
    ** If not, read all parameters from EFS and store them in local variables
    */
    if(!cmrpm_check_sim_changed())
    {
      cmrpm_efs_info_s_type  rpm_efs_info;
      
      if(cmrpm_read_efs_info(&rpm_efs_info))
      {
        rpm_ptr->app_reset.timer = rpm_efs_info.app_reset_timer;
        rpm_ptr->app_reset.counter = rpm_efs_info.app_reset_counter;
        rpm_ptr->app_reset.block_leak_rate_timer = 
          rpm_efs_info.app_reset_block_leak_rate_timer;
        rpm_ptr->mmreject_reset.leak_rate_timer = rpm_efs_info.leak_rate_timer;

      }
      else
      {
        CM_ERR_0("Can't read RPM info from EFS");           
      }          
    }

    CM_MSG_HIGH_6("RPM Parameter app_reset: N1:%d, timer:%d, counter:%d, C-BR-1:%d, LR-1:%d, LR-1 timer:%d",
      rpm_ptr->app_reset.max_num_reset,
      rpm_ptr->app_reset.timer,
      rpm_ptr->app_reset.counter,
      rpm_ptr->app_reset.block_counter,
      rpm_ptr->app_reset.block_leak_rate_hour,
      rpm_ptr->app_reset.block_leak_rate_timer);
	
    CM_MSG_HIGH_5("RPM Parameter mmreject_reset: T1:%d, timer:%d, C-R-1:%d, LR-2:%d, LR-2 timer:%d",
      rpm_ptr->mmreject_reset.average_reset_time,
      rpm_ptr->mmreject_reset.timer,
      rpm_ptr->mmreject_reset.counter,
      rpm_ptr->mmreject_reset.leak_rate_hour,
      rpm_ptr->mmreject_reset.leak_rate_timer);
    
    /* Check if there is any expired timer, then do processing accordingly */
    cmrpm_check_timer_expired();
  }  

  return rpm_ptr->is_rpm_enabled;

} /* cmrpm_init */


/*===========================================================================

FUNCTION cmrpm_timer_proc

DESCRIPTION
  Process timer for RPM.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_timer_proc(
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
)
{
  #ifdef CM_GW_SUPPORTED
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
  uint32                        curr_sys_time  = cm_util_get_curr_systime();

  CM_MSG_LOW_3("rpm_timer_proc: rpm_ptr->sys_time_efs:%d, coin_cell_support:%d, curr_systime:%d",
               rpm_ptr->sys_time_efs, rpm_ptr->is_coin_cell_support, curr_sys_time);
  
  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_sys_time = (rpm_ptr->sys_time_efs) + curr_sys_time;
  }

  SYS_ARG_NOT_USED( timer_event );

  /* Check if APP reset timer is expired */
  if((rpm_ptr->app_reset.timer !=0) &&
     (curr_sys_time >= rpm_ptr->app_reset.timer))
  {
    CM_MSG_HIGH_2("App Reset Timer expired - curr_systime:%d, timer:%d",
      curr_sys_time, rpm_ptr->app_reset.timer);

    
    /* reset the counter */
    rpm_ptr->app_reset.counter = 0;
    
    /* restart the timer */
    rpm_ptr->app_reset.timer = curr_sys_time + CMRPM_SEC_PER_HOUR;
    if(cmrpm_write_efs_info(rpm_ptr)  == FALSE)
    {
      CM_ERR_0("Can't write APP_RESET_TIMER");
      
    }    
  }  
 
  /* Check if LR-1 timer is expired */ 
  if((rpm_ptr->app_reset.block_leak_rate_timer !=0) &&
    (curr_sys_time >= rpm_ptr->app_reset.block_leak_rate_timer))
  {
    CM_MSG_HIGH_2("Block Leak Rate Timer expired - curr_systime:%d, timer:%d",
      curr_sys_time, rpm_ptr->app_reset.block_leak_rate_timer);
        
    if(rpm_ptr->app_reset.block_counter > 0)
    {
      /* decrement C-BR-1 */
      rpm_ptr->app_reset.block_counter -= 1; 
      if(cmrpm_write_rpm_omc(rpm_ptr->app_reset.block_counter,
                             rpm_ptr->mmreject_reset.counter))
      {
        CM_MSG_HIGH_2( "Update RPM OMC - block_counter:%d, reset_counter:%d",
                       rpm_ptr->app_reset.block_counter,
                       rpm_ptr->mmreject_reset.counter );

      }

      if(rpm_ptr->app_reset.block_counter > 0)
      {
        /* if C-BR-1 is still greater than 0, restart LR-1 timer */
        rpm_ptr->app_reset.block_leak_rate_timer = curr_sys_time + 
          (rpm_ptr->app_reset.block_leak_rate_hour * CMRPM_SEC_PER_HOUR);  
      }     
      else
      {
        /* counter becomes 0, no need to run timer, 
        ** reset the LR-1 timer to 0 
        */
        rpm_ptr->app_reset.block_leak_rate_timer = 0;
      }
    
      if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
      {
        CM_ERR_1("Can't write APP_RESET_BLOCK_LR_TIMER: %d",
          rpm_ptr->app_reset.block_leak_rate_timer);   
      }              
      
    }
  }

  /* Check LR-2 timer */
  if((rpm_ptr->mmreject_reset.leak_rate_timer !=0)&&
    (curr_sys_time >= rpm_ptr->mmreject_reset.leak_rate_timer))
  {    
    CM_MSG_HIGH_2("Leak Rate Timer expired - curr_systime:%d, timer:%d",
      curr_sys_time, rpm_ptr->mmreject_reset.leak_rate_timer);
    
    if(rpm_ptr->mmreject_reset.counter > 0)
    {
      /* decrement C-R-1 */
      rpm_ptr->mmreject_reset.counter -= 1; 
      if(cmrpm_write_rpm_omc(rpm_ptr->app_reset.block_counter,
                            rpm_ptr->mmreject_reset.counter))
      {
        CM_MSG_HIGH_2("Update RPM OMC - block_counter:%d, reset_counter:%d",
                      rpm_ptr->app_reset.block_counter,
                      rpm_ptr->mmreject_reset.counter);

      }
      
      if(rpm_ptr->mmreject_reset.counter > 0)
      {
       /* if C-R-1 is still greater than 0, restart LR-2 timer */
       rpm_ptr->mmreject_reset.leak_rate_timer = curr_sys_time + 
         (rpm_ptr->mmreject_reset.leak_rate_hour * CMRPM_SEC_PER_HOUR);    
      }
      else
      {
        /* counter becomes 0, no need to run timer, 
        ** reset the LR-2 timer to 0 
        */
        rpm_ptr->mmreject_reset.leak_rate_timer = 0;
      }

      if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
      {
         CM_ERR_1("Can't write RESET_LR_TIMER:%d",
          rpm_ptr->mmreject_reset.leak_rate_timer);   
      }              
    }
  }

  /* Check if MM/GMM reject reset timer(T1) is expired 
  ** This check should be at the last since we should process all other timer 
  ** prior to reset the modem.
  */
  if((rpm_ptr->mmreject_reset.timer !=0) && 
     (curr_sys_time >= rpm_ptr->mmreject_reset.timer))
  {
    CM_MSG_HIGH_2("MMReject Reset Timer expired - curr_systime:%d, timer:%d",
      curr_sys_time, rpm_ptr->mmreject_reset.timer);
        
    /* Check if there is any on-going call, if then, we need to delay 
    ** the reset until the ME is back to IDLE 
    */    
	if ((mm_per_stacks_get_connection_status(SYS_MODEM_AS_ID_1,SYS_MODEM_STACK_ID_1) 
		!= SYS_GW_CONN_STATUS_CONN)||(cmph_is_subs_feature_mode_sglte(SYS_MODEM_AS_ID_1) 
		&& (mm_per_stacks_get_connection_status(SYS_MODEM_AS_ID_1,SYS_MODEM_STACK_ID_2)
		!= SYS_GW_CONN_STATUS_CONN)))
    {
      /* stop the timer */
      rpm_ptr->mmreject_reset.timer = 0;
      
      /* increment C-R-1 */
      if(rpm_ptr->mmreject_reset.counter != 0xFF)
      {
        rpm_ptr->mmreject_reset.counter += 1;

        if(cmrpm_write_rpm_omc(rpm_ptr->app_reset.block_counter,
                           rpm_ptr->mmreject_reset.counter))
        {
          CM_MSG_HIGH_2("Update RPM OMC - block_counter:%d, reset_counter:%d",
                        rpm_ptr->app_reset.block_counter,
                        rpm_ptr->mmreject_reset.counter);

        }
      }

      /* start LR-2 timer if it's not already running */
      if((rpm_ptr->mmreject_reset.leak_rate_timer == 0) &&
         (rpm_ptr->mmreject_reset.leak_rate_hour != 0))
      {
        rpm_ptr->mmreject_reset.leak_rate_timer = curr_sys_time + 
          (rpm_ptr->mmreject_reset.leak_rate_hour * CMRPM_SEC_PER_HOUR);

        if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
        {
          CM_ERR_0("Can't write RESET_LR_TIMER");           
        }           
      }      

      cmrpm_reset_sim();
      
    }
    else
    {
      CM_MSG_HIGH_0("MM/GMM Reject reset pending - call in progress");
    }
    
  }
  #else
  SYS_ARG_NOT_USED( timer_event );
  #endif /* CM_GW_SUPPORTED */
}


/*===========================================================================

  FUNCTION cmrpm_sd_rpt_proc

  DESCRIPTION
    Process reports from System Determination.

  DEPENDENCIES
    RPM object must have already been initialized with cmrpm_init().

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/
void cmrpm_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{

  /*lint -e{826} */
  const cm_sd_rpt_u_type         *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD reports */

  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
    /* Point at rpm  object */


 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( rpm_ptr != NULL );
  CM_ASSERT( sd_rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is serving system change notifiction
  ** 1. Updating appropriate domain selection object fields.
  */
  switch( sd_rpt_ptr->hdr.cmd )
  {
    /* Service Indicators information is changed.
    */
    case CM_SRV_IND_INFO_F:
      if(rpm_ptr->is_rpm_enabled)
      {
        /* Reset timer 1hr timer when UE is registered in both CS and PS domain */
        if((sd_rpt_ptr->srv_ind_info.si_info.srv_status == SYS_SRV_STATUS_SRV) && 
           (sd_rpt_ptr->srv_ind_info.si_info.srv_domain == SYS_SRV_DOMAIN_CS_PS))
        {
          /* Reset the app reset counter */
          rpm_ptr->app_reset.counter = 0;
  
          /* Stop the app reset timer */
          rpm_ptr->app_reset.timer = 0;
  
          /* Update EFS param */
          if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
          {
            CM_ERR_0("Can't write EFS_INFO");
          }    
  
          CM_MSG_HIGH_0("CS/PS registered : Stop APP reset timer");
        }
      }
      break;
         
          
    default:
      break;
  } /* switch( sd_rpt_ptr->hdr.cmd ) */

  return;
} /* cmsds_sd_rpt_proc() */

/*===========================================================================

FUNCTION cmrpm_rpt_proc

DESCRIPTION

  Process LL reports related to RPM


DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmrpm_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to a LL report */
)

{
  
  cm_rpt_type             *cm_rpt_ptr = (cm_rpt_type *) rpt_ptr;
     /* Pointer to a LL report */
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
     /* Point at RPM object */
  uint32 					   curr_system_time   = cm_util_get_curr_systime();

  
  /* If no coin cell battery, add EFS time to current uptime to get total
  ** system time since power-up
  */
  if(rpm_ptr->is_coin_cell_support == FALSE )
  {
    curr_system_time = (rpm_ptr->sys_time_efs) + curr_system_time;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(cm_rpt_ptr != NULL);
  CM_ASSERT(rpm_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the report
  */
  switch( cm_rpt_ptr->hdr.cmd )
  {
    case CM_REG_REJECT_IND:
      if(rpm_ptr->is_rpm_enabled)
      {
        cm_reg_reject_ind_s_type *reg_reject_ind_ptr = 
          &cm_rpt_ptr->cmd.reg_reject_ind;
        
        CM_MSG_MED_4("cmrpm_rpt_proc: cmd=%d, rpm_ptr->sys_time_efs:%d, coin_cell_support:%d, curr_systime:%d",
                     cm_rpt_ptr->hdr.cmd, rpm_ptr->sys_time_efs,
                                   rpm_ptr->is_coin_cell_support, curr_system_time);

        if(reg_reject_ind_ptr->reject_domain == SYS_SRV_DOMAIN_CS_ONLY || 
          reg_reject_ind_ptr->reject_domain == SYS_SRV_DOMAIN_PS_ONLY || 
          reg_reject_ind_ptr->reject_domain == SYS_SRV_DOMAIN_CS_PS)
        {
        
           /* Start 1hr timer upon any MM/GMM/EMM reject cause */

           /* Only when N1(max_num_reset) is not 0,
            ** And the app_reset_timer is not already running, 
            ** then start the app_reset_timer
            */
          if((rpm_ptr->app_reset.max_num_reset != 0) &&
             (rpm_ptr->app_reset.timer == 0))
          {
            rpm_ptr->app_reset.timer = curr_system_time + CMRPM_SEC_PER_HOUR;          
    
            CM_MSG_HIGH_1("App reset timer is running: %d",
              rpm_ptr->app_reset.timer);
                      
            if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
            {
              CM_ERR_0("Can't write APP_RESET_TIMER");
              
            }
              
          }
          
          /* If the MM/GMM reject cause of below received, T1 timer should start */
          if(reg_reject_ind_ptr->reject_cause == IMSI_UNKNOWN_IN_HLR || 
            reg_reject_ind_ptr->reject_cause == ILLEGAL_MS || 
            reg_reject_ind_ptr->reject_cause == ILLEGAL_ME || 
            reg_reject_ind_ptr->reject_cause == GPRS_SERVICES_NOT_ALLOWED || 
            reg_reject_ind_ptr->reject_cause == GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED)
          {
            CM_MSG_HIGH_2("MM/GMM Reject : domain: %d, cause: %d ", 
                 reg_reject_ind_ptr->reject_domain,
                 reg_reject_ind_ptr->reject_cause );
      
            /* Only when the T1(average_reset_time) is not 0, 
            ** And when the reset_timer is not already running, 
            ** then start the mmreject reset_timer 
            */
            if((rpm_ptr->mmreject_reset.timer == 0) &&
               (rpm_ptr->mmreject_reset.average_reset_time != 0))
            {
              rpm_ptr->mmreject_reset.timer = curr_system_time +
                (rpm_ptr->mmreject_reset.average_reset_time 
                 * CMRPM_MIN_PER_T1 * CMRPM_SEC_PER_MIN);
      
              CM_MSG_HIGH_2("MM Reject reset timer is running: %d, curr_systime:%d",
                rpm_ptr->mmreject_reset.timer, curr_system_time);
            }      
      
          }
        }
      }
      break;

    default:
      break;
  }
}

/*===========================================================================

FUNCTION cmrpm_process_call_end

DESCRIPTION
  This function process CALL_END when RPM Is running
  If there is mm reject reset timer(T1) is running, we need to reset the modem

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_process_call_end(void)
{
  cmrpm_s_type                  *rpm_ptr        = cmrpm_ptr();
  uint32                        curr_sys_time  = cm_util_get_curr_systime();

  if(rpm_ptr->is_rpm_enabled)
  {    
    
    /* Check if MM/GMM reject reset timer(T1) is expired */ 
    if((rpm_ptr->mmreject_reset.timer != 0 )&&
      (curr_sys_time >= rpm_ptr->mmreject_reset.timer))
    {
      CM_MSG_HIGH_0("Call Ended and Reset timer expired: Reset the phone..");
      
      /* stop the timer */
      rpm_ptr->mmreject_reset.timer = 0;
      
      /* increment C-R-1 */
      if(rpm_ptr->mmreject_reset.counter != 0xFF)
      {
        rpm_ptr->mmreject_reset.counter += 1;
  
        if(cmrpm_write_rpm_omc(rpm_ptr->app_reset.block_counter,
                           rpm_ptr->mmreject_reset.counter))
        {
          CM_MSG_HIGH_2("Update RPM OMC - block_counter:%d, reset_counter:%d",
                       rpm_ptr->app_reset.block_counter,
                       rpm_ptr->mmreject_reset.counter);
        }
      }
  
      /* start LR-2 timer if it's not already running */
      if((rpm_ptr->mmreject_reset.leak_rate_timer == 0) &&
         (rpm_ptr->mmreject_reset.leak_rate_hour != 0))
      {
        rpm_ptr->mmreject_reset.leak_rate_timer = curr_sys_time + 
          (rpm_ptr->mmreject_reset.leak_rate_hour * CMRPM_SEC_PER_HOUR);
  
        if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
        {
          CM_ERR_0("Can't write RESET_LR_TIMER");           
        }           
      }      
  
      cmrpm_reset_sim();
  
      
    }
  }  
}

/*===========================================================================

FUNCTION cmrpm_process_gsdi_refresh

DESCRIPTION
  This function process GSDI REFRESH_EVT when one of RPM files are updated
  This function will be called from CMMMGSDI.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_process_gsdi_refresh(
  const mmgsdi_refresh_file_list_type    *refresh_file_list
)
{
  uint32                         file_num_count = 0; 
  cmrpm_s_type                   *rpm_ptr        = cmrpm_ptr();
  boolean                        is_rpm_param_updated = FALSE;
  boolean                        is_rpm_cntr_lr_updated = FALSE;

  
  for(file_num_count=0; file_num_count < refresh_file_list->num_files;
      file_num_count++)
  {
    switch (refresh_file_list->file_list_ptr[file_num_count])
    {
      case MMGSDI_USIM_7F66_PROP1_RPM_PARAM:         
        if(cmrpm_read_rpm_parameters(
          &(rpm_ptr->app_reset.max_num_reset),
          &(rpm_ptr->mmreject_reset.average_reset_time)))
        {
          is_rpm_param_updated = TRUE;
        }
        else
        {
   
          CM_ERR_0("Fail to Read RPM Parameter");
        }
        break;

      case MMGSDI_USIM_7F66_PROP1_RPM_EF:     
        if(!cmrpm_read_rpm_enabled_flag(
          &(rpm_ptr->is_rpm_enabled)))  
        {   
          CM_ERR_0("Fail to Read RPM Enabled Flag");
        }
        break;      

      
      case MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR:     
        if(cmrpm_read_rpm_omclr(
          (uint8*)&(rpm_ptr->app_reset.block_leak_rate_hour),
          (uint8*)&(rpm_ptr->mmreject_reset.leak_rate_hour)))
        {
          is_rpm_cntr_lr_updated = TRUE;
        }
        else
        {   
          CM_ERR_0("Fail to Read RPM CNTR_LR");
        }
        break;       

      case MMGSDI_USIM_7F66_PROP1_RPM_CNTRS:              
        if(!cmrpm_read_rpm_omc(
            (uint8*)&(rpm_ptr->app_reset.block_counter),
            (uint8*)&(rpm_ptr->mmreject_reset.counter))) 
        {   
          CM_ERR_0("Fail to Read RPM Enabled Flag");
        }
        break;    

      default:
        break;        
    }
  }
      
  if(is_rpm_param_updated || is_rpm_cntr_lr_updated)
  {
    /* If EF-RPM Parameter or EF-RPM Counter Leak Rate is updated by OTA
    ** we need to reset all RPM Operation Management Counters
    */
    rpm_ptr->app_reset.block_counter = 0;
    rpm_ptr->mmreject_reset.counter = 0;

    if(cmrpm_write_rpm_omc(rpm_ptr->app_reset.block_counter,
      rpm_ptr->mmreject_reset.counter))
    {
       CM_MSG_MED_2("Update RPM OMC - block_counter:%d, reset_counter:%d",
                    rpm_ptr->app_reset.block_counter,
                    rpm_ptr->mmreject_reset.counter);
    } 
  }  

  if(is_rpm_param_updated)
  {
    /* If EF-RPM parameter is updated, we need to reset 
    ** all RPM related counters/timer 
    */

    CM_MSG_HIGH_0("EF-RPM PARAM updated, reset all counters/timers");
    
    rpm_ptr->app_reset.timer = 0;
    rpm_ptr->app_reset.counter = 0;
    rpm_ptr->app_reset.block_leak_rate_timer = 0;
    rpm_ptr->mmreject_reset.leak_rate_timer = 0;
  
    if(cmrpm_write_efs_info(rpm_ptr) == FALSE)
    {
      CM_ERR_0("Can't write RPM EFS INFO");
      
    }    
    
  }

  CM_MSG_HIGH_6("RPM parm reload: is_rpm %d, mmrej_reset: avg_reset_tmr %d, tmr %d, counter %d, LR-2 %d, LR-2 tmr %d",
                rpm_ptr->is_rpm_enabled,
                rpm_ptr->mmreject_reset.average_reset_time,
                rpm_ptr->mmreject_reset.timer,
                rpm_ptr->mmreject_reset.counter,
                rpm_ptr->mmreject_reset.leak_rate_hour,
                rpm_ptr->mmreject_reset.leak_rate_timer);
  CM_MSG_HIGH_6("app_reset: max_num(N1) %d, timer %d, counter %d, block_counter(C-BR-1) %d, LR-1 %d, LR-1 timer %d",
    rpm_ptr->app_reset.max_num_reset,
    rpm_ptr->app_reset.timer,
    rpm_ptr->app_reset.counter,
    rpm_ptr->app_reset.block_counter,
    rpm_ptr->app_reset.block_leak_rate_hour,
    rpm_ptr->app_reset.block_leak_rate_timer);
  
  
}

