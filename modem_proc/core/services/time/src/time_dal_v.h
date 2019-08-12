#ifndef TIME_DAL_V_H
#define TIME_DAL_V_H


#include "comdef.h"


typedef struct
{
  /*processor and os info*/
  uint8 os  ;   /*  FEATURE_WINCE   */
  uint32 processor  ;   /*  T_MSM7500   */

  /*image info*/
  uint8 image   ;   /*  IMAGE_APPS_PROC */

  /*technologies*/
  uint8 cdma_1x ;   /*  FEATURE_CDMA1X  */
    uint8   hdr ;   /*  FEATURE_HDR */
    uint8   gsm ;   /*  FEATURE_GSM */
    uint8   gps_one ;   /*  FEATURE_GPSONE_PE   */
    uint8   mflo    ;   /*  FEATURE_MFLO    */
  
  /*multi proc sync*/
  uint8 multiproc_sync  ;   /*  FEATURE_SLOW_CLOCK_MULTIPROC_SYNC   */
  uint8 tod_multiproc_sync  ;   /*  FEATURE_TOD_MULTIPROC_SYNC  */

  /*wince features*/
  uint8 wince_oal;   /*FEATURE_WINCE_OAL*/
  uint8 wince_bootloader; /*FEATURE_WINCE_BOOTLOADER*/

  /*remoting related*/
  uint8 htorpc  ;   /*  FEATURE_HTORPC_METACOMMENTS */
    uint8   oncrpc  ;   /*  FEATURE_ONCRPC  */
    uint8   export_time_remote_atom ;   /*  FEATURE_EXPORT_TIME_REMOTE_ATOM */
    uint8   oncrpc_server_cleanup_support   ;   /*  FEATURE_ONCRPC_SERVER_CLEANUP_SUPPORT   */

  /*sec time*/
  uint8 sec_time    ;   /*  FEATURE_SEC_TIME    */
  uint8 sec_time_v2 ;   /*  FEATURE_SEC_TIME_V2 */

  /*rtc related features*/
  uint8 pm1000_rtc  ;   /*  FEATURE_PM1000_RTC  */
    uint8   tod_sync_pmic_rtc   ;   /*  TIME_TOD_SYNC_PMIC_RTC  */
    uint8   time_tod_as_rtc_offset  ;   /*  FEATURE_TIME_TOD_AS_RTC_OFFSET  */
  uint8 use_pmic_rtc    ;   /*  FEATURE_PMIC_RTC    */
  uint8 time_dem_sync   ;   /*  FEATURE_TIME_DEM_SYNCHRONIZE    */

  /*miscellaneous*/
    uint8   use_timer_task  ;   /*  FEATURE_TIMER_TASK  */
    uint8   use_os_memory_protection    ;   /*  FEATURE_OS_MEMORY_PROTECTION    */
    uint8   timer_trace_enabled ;   /*  FEATURE_TIMER_TRACE */
    uint8   timer_requires_odd_write    ;   /*  FEATURE_TIME_REQUIRES_ODD_WRITE */
    uint8   rex_apc ;   /*  FEATURE_REX_APC */
    uint8   timetick_int_edge_triggerred    ;   /*  TIMETICK_INT_EDGE_TRIGGERED */
    uint8   define_active_timer ;   /*  TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER  */
    uint8   build_boot_chain    ;   /*  BUILD_BOOT_CHAIN    */
    uint8   sec_time_mu ;   /*  FEATURE_SEC_TIME_MU */
    uint8   no_tai_wrapper  ;   /*  FEATURE_NO_TAI_WRAPPER  */
    uint8   linux   ;   /*  FEATURE_NATIVELINUX */
    uint8   sectime_mu  ;   /*  FEATURE_SECTIME_MU  */
    uint8   smartfon    ;   /*  smartfon    */
    uint8   use_time_vu ;   /*  FEATURE_USE_TIME_VU */
    uint8   time_umts_target    ;   /*  FEATURE_TIME_UMTS_TARGETS   */
    uint8   power_collapse  ;   /*  FEATURE_TIME_POWER_COLLAPSE */
    uint8   t_quasar    ;   /*  T_QUASAR    */
    uint8   t_qsc60X0   ;   /*  T_QSC60X0   */
    uint8   t_quasarX   ;   /*  T_QUASAR_X  */
}time_defines_type;

/* External declaration of the time_defines global structure */
extern time_defines_type time_defines;


typedef enum 
{
  OS_REX =0,
  OS_BMP,
  OS_WM7,
  OS_QUBE_L4,
  OS_BLAST
}time_os_enum_type;

typedef enum 
{
  MSM_6695=6695,
  MSM_7630=7630,
  MSM_7500=7500,
  MSM_8650=8650,
  MDM_9600=9600
  
}time_processor_enum_type;

typedef enum
{
  MODEM_IMAGE = 0,
  APPS_IMAGE,
  Q6_IMAGE
}time_image_enum_type;

typedef struct
{
  void (*mpm_sync_fp)(void);
  void (*time_call_init_functions_fp)(void);
}time_fn_ptr_type;

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
);


#endif /*TIME_DAL_V_H*/
