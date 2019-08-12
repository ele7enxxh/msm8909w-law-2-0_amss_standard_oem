#include "time_dal_v.h"
#include "DALSys.h"
#include "DALDeviceId.h"

const DALREG_DriverInfo 
DALTime_Config_DriverInfo = { NULL, //Time_Config_DalTime_Config_Attach,
                              0,    //DalTime_Config_DeviceId
                             };


/*=============================================================================

FUNCTION time_dal_config_init

DESCRIPTION
  Initializes the data members of time_defines structure.
  This information will be used in lieu of features for making
  OS dependent calls

DEPENDENCIES
  XML configuration file for the target should be configured and thoroughly 
reviewed before calling this function.
  

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_dal_config_init
(
  time_defines_type* time_define_ptr
)
{
  DALSYSPropertyVar PropVar;
  uint32 DALProps[2];
  DALSYSPropertyHandle hDALProps=DALProps;

   
   /*First get the property handle*/
  /*TODO: Need to define the DALDEVICEID for getting the handle*/
  DALSYS_GetDALPropertyHandle(DALDEVICEID_TIME_CFG,hDALProps);

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "os",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->os = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "processor",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->processor = (time_processor_enum_type)PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "image",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->image = (time_image_enum_type)PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "cdma_1x",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->cdma_1x = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "hdr",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->hdr = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "gsm",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->gsm = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "gps_one",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->gps_one = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "mflo",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->mflo = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "multiproc_sync",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->multiproc_sync = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "tod_multiproc_sync",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->tod_multiproc_sync = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "wince_oal",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->wince_oal = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "wince_bootloader",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->wince_bootloader = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "htorpc",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->htorpc = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "oncrpc",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->oncrpc = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "export_time_remote_atom",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->export_time_remote_atom = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "oncrpc_server_cleanup_support",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->oncrpc_server_cleanup_support = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "sec_time",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->sec_time = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "sec_time_v2",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->sec_time_v2 = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "pm1000_rtc",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->pm1000_rtc = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "tod_sync_pmic_rtc",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->tod_sync_pmic_rtc = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "time_tod_as_rtc_offset",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->time_tod_as_rtc_offset = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "use_pmic_rtc",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->use_pmic_rtc = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "time_dem_sync",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->time_dem_sync = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "use_timer_task",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->use_timer_task = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "use_os_memory_protection",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->use_os_memory_protection = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "timer_trace_enabled",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->timer_trace_enabled = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "timer_requires_odd_write",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->timer_requires_odd_write = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "rex_apc",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->rex_apc = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "timetick_int_edge_triggerred",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->timetick_int_edge_triggerred = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "define_active_timer",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->define_active_timer = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "build_boot_chain",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->build_boot_chain = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "sec_time_mu",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->sec_time_mu = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "no_tai_wrapper",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->no_tai_wrapper = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "linux",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->linux = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "sectime_mu",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->sectime_mu = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "smartfon",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->smartfon = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "use_time_vu",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->use_time_vu = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "time_umts_target",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->time_umts_target = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "power_collapse",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->power_collapse = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "t_quasar",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->t_quasar = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "t_qsc60X0",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->t_qsc60X0 = PropVar.Val.dwVal;
  }
  
  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hDALProps, //Prop handle
                                   "t_quasarX",     //Prop Name String
                                   0,                  //Not interested in Prop Id
                                   &PropVar))          //Output
  {
    time_define_ptr->t_quasarX = PropVar.Val.dwVal;
  }

}
