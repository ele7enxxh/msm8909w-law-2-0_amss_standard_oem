/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Time Services Diagnostics Packet Processing
        T I M E   U N I T   T E S T   F R A M E W O R K 

General Description
  Diagnostic packet processing routines for Time Service operations.

Copyright (c) 2011 - 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/15   abh     Added new test cases.
01/07/09   pc      Created.
===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include <stringl/stringl.h>
#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "memheap.h"
#include "time_jul.h"
#include "time_svc.h"
#include "time_diag.h"
#include "ats_v.h"
#include "time_fn_map.h"
#include "ats_rtc.h"
#include "time_genoff_v.h"

#ifdef FEATURE_QMI_TIME_REMOTE
#include "qmi_time_server.h"
#endif /*FEATURE_QMI_TIME_REMOTE*/

/* test purpose */
#include "qw.h"
#include "timer.h"
#include "timetick_sclk64.h"
#include "time_conv.h"
#include "time_efs.h"


/****************************************************************************\
Example for Command compilation :-
===============================

 - Below is the diag packet req/resp structure for reference :-
   ---------------------------------------------------------
typedef struct
{
  diagpkt_subsys_header_type header;        - Sub System header
  uint16 command;                           - Which command
  uint16 command_data[DIAG_TIME_DATA_SIZE]; - Command parameters
                                              -to be used by function
  uint16 return_stat;                       - Return Status

}TIME_SERVICES_TEST_F_(req_type/resp_type);
  ----------------------------------------------------------

 - Sample Command building:-
   -----------------------
diagpkt_subsys_header_type header (4 bytes)
------------------------------------------------
Command Class is default   - 1 byte  : 0x4B
Subsystem id for Time_Diag - 1 byte  : 0x49(73 for Time Services)
??                         - 2 bytes : 0x00 0x00
------------------------------------------------

uint16 command (2 bytes)
-----------------------------------------------
Command ID                 - 2 bytes: 0x00 0x00 (Eg for TIME_SET)
-----------------------------------------------

uint16 command_data[DIAG_TIME_DATA_SIZE] (14 bytes max)
-----------------------------------------------
0xD9 0x07 0x02 0x00 0x05 0x00 0x0A 0x00 0x14 0x00 0x1E 0x00 0x03 00 - 14 bytes
 # Eg for time_set_julian()
   0xD9 0x07 -> 0x07D9 = 2009 (Year)
   0x02 0x00 -> 0x0002 =    2 (Month)
   0x05 0x00 -> 0x0005 =    5 (Day)
   0x0A 0x00 -> 0x000A =   10 (Hour)
   0x14 0x00 -> 0x0014 =   20 (Mins)
   0x1E 0x00 -> 0x001E =   30 (Secs)
   0x03 0x00 -> 0x0003 =    3 (day of week)
-----------------------------------------------

uint16 return_stat (2 bytes)
----------------------------------------------------------------
Return Status used in response packets "0x00 0x00", if succesful
----------------------------------------------------------------
If
Tx: 0x4B 0x49 0x00 0x00 0x00 0x00 0xD9 0x07 0x02 0x00 0x05 0x00 0x0A 0x00
                                  0x14 0x00 0x1E 0x00 0x01 0x00
then expected
Rx: 0x4B 0x49 0x00 0x00 0x00 0x00 0xD9 0x07 0x02 0x00 0x05 0x00 0x0A 0x00
                                  0x14 0x00 0x1E 0x00 0x01 0x00 0x00 0x00

NOTE : For Commands that donot require any payload, 14 byte command data
       need not be sent
\****************************************************************************/

/*=============================================================================

                         DATA DEFINITIONS
=============================================================================*/

#define TIME_SET                  0
#define TIME_GET                  1
#define USER_TIME_GENOFF_GET_MS   2
#define USER_TIME_GENOFF_SET_SECS 3
#define USER_TIME_GENOFF_GET_SECS 4
#define USER_TIME_GENOFF_SET_JULI 5
#define USER_TIME_GENOFF_GET_JULI 6
#define TOD_TIME_GENOFF_GET_MS    7
#define TOD_TIME_GENOFF_SET_SECS  8
#define TOD_TIME_GENOFF_GET_SECS  9
#define TOD_TIME_GENOFF_SET_JULI  10
#define TOD_TIME_GENOFF_GET_JULI  11
#define TIME_SET_REMOTE_JULIAN 14
#define TIME_REGISTER_WITH_SERVER_MODEM 15

#define TIME_ALARM_REGISTER   16
#define TIME_ALARM_SET        17
#define TIME_1X_SET           18
#define TIME_GPS_SET          19
#define TIME_SERVER_TEST      20

#define TIME_GENOFF_GET_JULIAN 22
#define TIME_PRINT_RTC         23
#define TIME_GET_MEMHEAP_DEBUG_INFO 21
#define TIME_GENOFF_GET_STATS  30  /* Command - send_data 0x4B 0x49 0x00 0x00 30 0x0 */
#define TIME_SET_STRESS_TEST   31
#define TIME_AUTO_SET_TEST     32  /* Command - send_data 0x4B 0x49 0x00 0x00 32 0x0 10 0 - Sets time 10times using a timer */
#define TIME_AUTO_SET_WITH_TIMETICK_TEST   33 /* Command - send_data 0x4B 0x49 0x00 0x00 33 0x0 - Enables/Disables flag to set time along with timetick or not */
#define TIME_DIAG_ATS_PRIVATE_BASE_GENERIC_TEST    34 /* Command Reference - see in switch-case below */
#define TIME_DIAG_ATS_PRIVATE_BASE_SET_STRESS_TEST 35 /* Command Reference - see in switch-case below */
#define TIME_DIAG_ATS_PRIVATE_BASE_NEGATIVE_TEST   36 /* Command Reference - see in switch-case below */

/*=============================================================================
                         EXTERN & GLOBAL FILE VARIABLES
=============================================================================*/
extern time_genoff_ptr ats_private_bases[ATS_PRIVATE_MAX];

/* Array of the test bases for time diag, along with the file name */
typedef struct
{
  uint32 base_num;
  char f_name[MAX_EFS_FILE_NAME_SIZE];
}time_diag_private_test_bases_type;
static time_diag_private_test_bases_type time_diag_private_test_bases[ATS_PRIVATE_MAX];

/*=============================================================================

                         FUNCTION DEFINITIONS
=============================================================================*/

/*extern functions to be called*/
#ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT
extern void time_remote_mm_set_julian(time_julian_type *jul_time);
extern void time_start_mm_registration(void);
#endif

static uint32 alarm_in_sec = 10;
extern int32 time_alarm_set_secs(uint32 expiration);

#ifdef FEATURE_QMI_TIME_REMOTE
static uint32 time_diag_fps_initialized = 0;
static time_fn_map_fp_type * time_diag_fps;
#endif /*FEATURE_QMI_TIME_REMOTE*/

timer_type time_test_timer;
timer_group_type time_test_group;
static uint32 send_with_timetick = 0;
static uint32 time_setting_num = 0;
static uint32 time_setting_curr = 0;

void test_time_setting(timer_cb_data_type y)
{
    /* Arguments to be sent to set time_rtc */
   time_genoff_args_struct_type    temp_genoff_args;
   time_type test_time;
   uint64 timetick, uptime;
   
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   test_time[1] = 0x00c9cbf2;
   test_time[0] = 0x4a000000;
   
   time_setting_curr++;
   
   timetick = timetick_sclk64_get();                    /* timetick */
   uptime = timetick - time_sclk64_get_base_timetick(); /* uptime */
   timetick_sclk64_add_xo(test_time, timetick);
   
   ATS_ERR_4("time_diag: timetick: %.8x %.8x, uptime: %.8x %.8x", timetick>>32, timetick, uptime>>32, uptime);

   temp_genoff_args.base        = ATS_HDR; 
   temp_genoff_args.base_source = TIME_SCLK;
   temp_genoff_args.ts_val      = test_time;
   temp_genoff_args.unit        = TIME_STAMP;
   temp_genoff_args.operation   = T_SET;
   
   if(send_with_timetick == 0)
   {
      temp_genoff_args.dal_timetick_val_valid = 0;
      temp_genoff_args.dal_timetick_val       = 0;
   }
   else
   {
      temp_genoff_args.dal_timetick_val_valid = TIME_GENOFF_TIMETICK_VALID;
      temp_genoff_args.dal_timetick_val       = timetick;
   }
   
   /* Set the time */
   time_genoff_opr( &temp_genoff_args );
   
   if(time_setting_curr < time_setting_num)
      timer_set(&time_test_timer, (timetick % 1000), 0, T_MSEC);
   
   return;
} /* test_time_setting */

/*===========================================================================

 FUNCTION TIME_SERVICES_TEST

DESCRIPTION
 This function would test the Time services.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
PACK(void *) time_services_test 
(
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  time_julian_type julian;
  TIME_SERVICES_TEST_F_req_type *req =(TIME_SERVICES_TEST_F_req_type *) req_pkt;
  // uint8 shift=0;
   #ifdef FEATURE_QMI_TIME_REMOTE
   uint64 offset;
   uint16* offset_ptr = (uint16*)&offset;
   uint16 base;
   #endif /*FEATURE_QMI_TIME_REMOTE*/

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* TIME_SERVICES_TEST_F_rsp_type *rsp; */
  const int rsp_len = sizeof( TIME_SERVICES_TEST_F_rsp_type );

  PACK(void *) rsp_ptr = NULL;

  /* Allocate the same length as the request. */
  rsp_ptr = (TIME_SERVICES_TEST_F_rsp_type *)diagpkt_subsys_alloc
                                         ( DIAG_SUBSYS_TIME,
                                           TIME_SERVICES_TEST_F,
                                           rsp_len );

  if (rsp_ptr != NULL)
  {
    memscpy ((void *) rsp_ptr, pkt_len, (void *) req_pkt, pkt_len);
  }

  switch ( req->command )
  {
     case TIME_SET :
     /* This case would be exercising the TOD setting operation */
       julian.year        = req->command_data[0];
       julian.month       = req->command_data[1];
       julian.day         = req->command_data[2];
       julian.hour        = req->command_data[3];
       julian.minute      = req->command_data[4];
       julian.second      = req->command_data[5];
       julian.day_of_week = req->command_data[6];

       time_set_julian (&julian);

       ATS_MSG_ERROR("TIME_DIAG: Set: Year=%d : Month=%d : Day=%d",julian.year,
                                            julian.month,julian.day);
       ATS_MSG_ERROR("TIME_DIAG: Set: Hour=%d : Minute=%d : Second=%d",julian.hour,
                                            julian.minute,julian.second);
       ATS_ERR_1("TIME_DIAG: Set: Day_of_week=%d",julian.day_of_week);

       break;

     case TIME_GET :
       
       time_get_julian (&julian);

       ATS_MSG_ERROR("TIME_DIAG: Get: Year=%d : Month=%d : Day=%d",julian.year,
                                            julian.month,julian.day);
       ATS_MSG_ERROR("TIME_DIAG: Get: Hour=%d : Minute=%d : Second=%d",julian.hour,
                                            julian.minute,julian.second);
       ATS_ERR_1("TIME_DIAG: Get: Day_of_week=%d",julian.day_of_week);

       break;
    
     case USER_TIME_GENOFF_GET_SECS:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;

         /* Pointer to Time_genoff instance in consideration */
          time_type                       ts_val_secs;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_USER; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = ts_val_secs;
         temp_genoff_args.unit        = TIME_SECS;
         temp_genoff_args.operation   = T_GET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );
     
         ATS_ERR_2("TIME_DIAG: Get_USER: ts_val[0]=%d : ts_val[1]=%d",ts_val_secs[0],
                                            ts_val_secs[1]);
       }
       break;

     case USER_TIME_GENOFF_SET_SECS:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;

         /* Pointer to Time_genoff instance in consideration */
         time_type                       ts_val_secs = {0x20000, 0};

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_USER; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = ts_val_secs;
         temp_genoff_args.unit        = TIME_SECS;
         temp_genoff_args.operation   = T_SET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );
     
       }
       break;

     case USER_TIME_GENOFF_SET_JULI:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;
     
         /* Time julian Type */
         time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         jul_val.year        = req->command_data[0];
         jul_val.month       = req->command_data[1];
         jul_val.day         = req->command_data[2];
         jul_val.hour        = req->command_data[3];
         jul_val.minute      = req->command_data[4];
         jul_val.second      = req->command_data[5];
         jul_val.day_of_week = req->command_data[6];

         temp_genoff_args.base        = ATS_USER; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_SET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );
         ATS_MSG_ERROR("TIME_DIAG: Set_TOD: date=%d-%d-%d", jul_val.month, jul_val.day, jul_val.year);  
       }
       break;

     case USER_TIME_GENOFF_GET_JULI:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args ;
     
         /* Time julian Type */
         time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_USER; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_GET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );

         ATS_MSG_ERROR("TIME_DIAG: Get: Year=%d : Month=%d : Day=%d",jul_val.year,
                                            jul_val.month,jul_val.day);
         ATS_MSG_ERROR("TIME_DIAG: Get: Hour=%d : Minute=%d : Second=%d",jul_val.hour,
                                            jul_val.minute,jul_val.second);
         ATS_ERR_1("TIME_DIAG: Get: Day_of_week=%d",jul_val.day_of_week);
       }
       break;
       
	   /*set 1x time*/
	   case TIME_1X_SET:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;
     
         /* Time julian Type */
         time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         jul_val.year        = req->command_data[0];
         jul_val.month       = req->command_data[1];
         jul_val.day         = req->command_data[2];
         jul_val.hour        = req->command_data[3];
         jul_val.minute      = req->command_data[4];
         jul_val.second      = req->command_data[5];
         jul_val.day_of_week = req->command_data[6];

         ATS_MSG_ERROR("TIME_DIAG: Set_1X: date=%d-%d-%d", jul_val.month, jul_val.day, jul_val.year);
         temp_genoff_args.base        = ATS_1X; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_SET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );
       }
       break;

       /*TIME_GPS_SET*/
	    /*set 1x time*/
	   case TIME_GPS_SET:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;
     
         /* Time julian Type */
         time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         jul_val.year        = req->command_data[0];
         jul_val.month       = req->command_data[1];
         jul_val.day         = req->command_data[2];
         jul_val.hour        = req->command_data[3];
         jul_val.minute      = req->command_data[4];
         jul_val.second      = req->command_data[5];
         jul_val.day_of_week = req->command_data[6];

         temp_genoff_args.base        = ATS_GPS; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_SET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );
         ATS_MSG_ERROR("TIME_DIAG: Set_GPS: date=%d-%d-%d", jul_val.month, jul_val.day, jul_val.year);
       }
       break;

     case TOD_TIME_GENOFF_GET_SECS:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;

         /* Pointer to Time_genoff instance in consideration */
     time_type                       ts_val_secs;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = ts_val_secs;
         temp_genoff_args.unit        = TIME_SECS;
         temp_genoff_args.operation   = T_GET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );
     
         ATS_ERR_2("TIME_DIAG: Get_TOD: ts_val[0]=%d : ts_val[1]=%d",ts_val_secs[0],
                                            ts_val_secs[1]);
       }
       break;

     case TOD_TIME_GENOFF_SET_SECS:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;

         /* Pointer to Time_genoff instance in consideration */
         time_type                       ts_val_secs = {0x20000, 0};

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = ts_val_secs;
         temp_genoff_args.unit        = TIME_SECS;
         temp_genoff_args.operation   = T_SET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );
       }
       break;

     case TOD_TIME_GENOFF_SET_JULI:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args;
     
         /* Time julian Type */
         time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         jul_val.year        = req->command_data[0];
         jul_val.month       = req->command_data[1];
         jul_val.day         = req->command_data[2];
         jul_val.hour        = req->command_data[3];
         jul_val.minute      = req->command_data[4];
         jul_val.second      = req->command_data[5];
         jul_val.day_of_week = req->command_data[6];

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_SET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );
       }
       break;

     case TOD_TIME_GENOFF_GET_JULI:
       {
         /* Arguments to be sent to set time_rtc */
         time_genoff_args_struct_type    temp_genoff_args ;
     
         /* Time julian Type */
         time_julian_type                jul_val;

         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_GET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );

         ATS_MSG_ERROR("TIME_DIAG: Get: Year=%d : Month=%d : Day=%d",jul_val.year,
                                            jul_val.month,jul_val.day);
         ATS_MSG_ERROR("TIME_DIAG: Get: Hour=%d : Minute=%d : Second=%d",jul_val.hour,
                                            jul_val.minute,jul_val.second);
         ATS_MSG_ERROR("TIME_DIAG: Get: Day_of_week=%d",jul_val.day_of_week,0,0);
     
       }
       break;
     
    case TIME_SET_REMOTE_JULIAN:
      julian.year        = 2030;
      julian.month       = 9;
      julian.day         = 9;
      julian.hour        = 18;
      julian.minute      = 55;
      julian.second      = 0;
      julian.day_of_week = 3; 
      #ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT
      time_remote_mm_set_julian(&julian);
      ATS_ERR_1("TIME_DIAG: mm_fusion_debug:7K:Set julian value on Q6, jul_yr=%d",julian.year);
      #endif
    break;

    case TIME_REGISTER_WITH_SERVER_MODEM:
      #ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT
      time_start_mm_registration();
      #endif
    break;

    case TIME_ALARM_REGISTER:
    break;

    case TIME_ALARM_SET:
      time_alarm_set_secs(alarm_in_sec);
    break;

    case TIME_SERVER_TEST:
      #ifdef FEATURE_QMI_TIME_REMOTE
      /*first 16 bits can be treated as a base*/
      base =  req->command_data[0];
      offset = 0;
      *offset_ptr++ = req->command_data[1];
      *offset_ptr++ = req->command_data[2];
      *offset_ptr++ = req->command_data[3];
      *offset_ptr++ = req->command_data[4];

      ATS_MSG_ERROR("TIME_DIAG: time_server_test:indication message to be sent base=%d,offset[0]=%d, offset[1]=%d",
	            base,
				(uint32)offset&0xFFFF,
				(uint32)((offset&0xFFFF0000uLL)>>16));
      ATS_MSG_ERROR("TIME_DIAG: time_server_test:offset[2]=%d,offset[3]=%d",
	            (uint32)((offset&0xFFFF00000000uLL)>>32),
				(uint32)((offset&0xFFFF000000000000uLL)>>48), 0);
      time_server_call_ind(base, offset);
      /* Initialize Function Pointers to eliminate dependencies */
      if ( !time_diag_fps_initialized )
      {
        time_diag_fps = time_fn_map_get_fp();
        time_diag_fps_initialized++;
      }
      
      /* Send QMI notification if QMI is available */
      if ( time_diag_fps->time_server_call_ind_fp != NULL )
      {
        ATS_MSG_ERROR("TIME_DIAG: time_server_test:indication message to be sent base=%d,offset[0]=%d, offset[1]=%d",
		          base,
				  (uint32)offset&0xFFFF,
				  (uint32)((offset&0xFFFF0000uLL)>>16));
        ATS_MSG_ERROR("TIME_DIAG: time_server_test:offset[2]=%d,offset[3]=%d",
		         (uint32)((offset&0xFFFF00000000uLL)>>32),
				 (uint32)((offset&0xFFFF000000000000uLL)>>48), 0);
        time_diag_fps->time_server_call_ind_fp(base, offset);
      }
      #else /*FEATURE_QMI_TIME_REMOTE*/
      ATS_MSG("TIME_DIAG: FEATURE_QMI_TIME_REMOTE is not defined");
      #endif /*FEATURE_QMI_TIME_REMOTE*/
    break;

	case TIME_GENOFF_GET_JULIAN:
       {
         /* Arguments to be sent to get Julian time */
         time_genoff_args_struct_type    temp_genoff_args ;
     
         /* Time julian Type */
         time_julian_type                jul_val;
         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
         /* base input from user */
         temp_genoff_args.base        = req->command_data[0]; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_GET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );

         ATS_MSG_ERROR("TIME_DIAG: TIME_GENOFF_GET_JULIAN: Date = %d-%d-%d",jul_val.year,
                                            jul_val.month,jul_val.day);
         ATS_MSG_ERROR("TIME_DIAG: TIME_GENOFF_GET_JULIAN: Time = %d:%d:%d",jul_val.hour,
                                            jul_val.minute,jul_val.second);
         ATS_ERR_2("TIME_DIAG: TIME_GENOFF_GET_JULIAN: Day_of_week=%d : Base = %d",
		                                    jul_val.day_of_week, temp_genoff_args.base);
       }
    break;
	   
	case TIME_PRINT_RTC:
       {
	            /* Arguments to be sent to get RTC time */
         time_genoff_args_struct_type    temp_genoff_args ;
     
         /* Time julian Type */
         time_julian_type                jul_val;
         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
         ats_print_rtc_status();
		 
		 ATS_MSG_ERROR("TIME_DIAG: Time_debug: Calculated RTC for now: ", 0, 0, 0);
		 /* Give the calculated RTC time */
         temp_genoff_args.base        = ATS_RTC; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = (void *)(&jul_val);
         temp_genoff_args.unit        = TIME_JULIAN;
         temp_genoff_args.operation   = T_GET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
         time_genoff_opr( &temp_genoff_args );

         ATS_MSG_ERROR("TIME_DIAG: TIME_PRINT_RTC: Date = %d-%d-%d",jul_val.year,
                                            jul_val.month,jul_val.day);
         ATS_MSG_ERROR("TIME_DIAG: TIME_PRINT_RTC: Time = %d:%d:%d",jul_val.hour,
                                            jul_val.minute,jul_val.second);
         ATS_ERR_1("TIME_DIAG: TIME_PRINT_RTC: Day_of_week=%d",jul_val.day_of_week);

       }
    break;
	   
    case TIME_GET_MEMHEAP_DEBUG_INFO:
       #ifdef FEATURE_ENABLE_HEAP_PROFILING
         print_memheap_profile_table();
         break;
       #endif

     case TIME_GENOFF_GET_STATS:
  	  {
		/* Print the time for all the bases */

        /* base iterator */
		uint32               index;
		uint32               encoded_index;
		uint64               timetick;

		/* Time julian Type */
		time_julian_type				jul_val;
		time_genoff_args_struct_type    temp_genoff_args ;
	
		temp_genoff_args.base_source = TIME_SCLK;
		temp_genoff_args.ts_val 	 = (void *)(&jul_val);
		temp_genoff_args.unit		 = TIME_JULIAN;
		temp_genoff_args.operation	 = T_GET;
		temp_genoff_args.dal_timetick_val_valid = 0;
		temp_genoff_args.dal_timetick_val		= 0;
		
		ATS_ERR("TIME_DIAG : Time on INTENAL ATS bases :");
		
		for (index = 0 ; index < ATS_MAX  ; index++)
		 {
            temp_genoff_args.base = index; 
            time_genoff_opr( &temp_genoff_args );

            ATS_ERR_7("Time on BASE=%.2d %.2d/%.2d/%d %.2d:%.2d:%.2d",
				      index,
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second );
         }

		ATS_ERR(" ");
		ATS_ERR("TIME_DIAG : Time on EXTERNAL ATS bases :");
		for (index = 0 ; index < ATS_PRIVATE_MAX  ; index++)
		 {
            if(ats_private_bases[index]==NULL)
            	{
				ATS_ERR_1("Private base is NULL at index = %d",index);
				continue;
            	}
		 
			/* Encode this before the external time base before the genoff operation */
			encoded_index = ( (index ^ TIME_GENOFF_ENCODE_CONST) << 8 ) | (index);
			encoded_index += ATS_PRIVATE;

		 
            temp_genoff_args.base = encoded_index; 
            time_genoff_opr( &temp_genoff_args );

            ATS_ERR_7("Time on PRIVATE BASE=%.2d %.2d/%.2d/%d %.2d:%.2d:%.2d",
				      encoded_index,
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second );
         }		
		
	  	/* -------------------------------------------------------------------------- */
		/* Print debug stats */
        time_genoff_print_debug_stats();
        
        time_sclk64_debug_stats();
        
        timetick = timetick_sclk64_get();
        ATS_ERR_2("Current timetick = 0x%x%x", timetick>>32, timetick);
  	  }
     break;

	 /* Time set stress test */
	 /* Example Command : 
	   * send_data 0x4B 0x49 0x00 0x00 31 0x0 0xD0 0x07 0x03 0x00 0x00A 0x00 0x05 0x00 0x0A 0x00 0x0F 0x00 0x03 00
	   * This command will start the time set stress test from 10th Feb 2000, 5:10:15, 3rd day of week */
	 case TIME_SET_STRESS_TEST:
	 {
	   /* Arguments to be sent to set time_rtc */
	   time_genoff_args_struct_type    temp_genoff_args;
	 
	   /* Time julian Type */
	   time_julian_type 			   jul_val;
	   uint32 iter1 = 0, iter2 = 0;
	   time_bases_type bases[6] = { ATS_3GPP,
	   	                     ATS_1X,
	   	                     ATS_HDR,
	   	                     ATS_3GPP,
	   	                     ATS_LTE_HR,
	   	                     ATS_LTE_HR_GPS };
	 
	   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
	 
	   jul_val.year 	   = req->command_data[0];
	   jul_val.month	   = req->command_data[1];
	   jul_val.day		   = req->command_data[2];
	   jul_val.hour 	   = req->command_data[3];
	   jul_val.minute	   = req->command_data[4];
	   jul_val.second	   = req->command_data[5];
	   jul_val.day_of_week = req->command_data[6];

	   temp_genoff_args.base_source = TIME_SCLK;
	   temp_genoff_args.ts_val		= (void *)(&jul_val);
	   temp_genoff_args.unit		= TIME_JULIAN;
	   temp_genoff_args.operation	= T_SET;
	   temp_genoff_args.dal_timetick_val_valid = 0;
	   temp_genoff_args.dal_timetick_val	   = 0;

       for ( iter1 = 0 ; iter1 < 20 ; iter1++ )
       	{
       	   for ( iter2 = 0 ; iter2 < 6 ; iter2++ )
       	   	{
               ATS_ERR_7("TIME_SET_STRESS_TEST : BASE=%.2d %.2d/%.2d/%d %.2d:%.2d:%.2d",
				      bases[iter2],
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second );       	   	  
       	   	
       	   	   /* set the time */
	           temp_genoff_args.base = bases[iter2]; 	   
	           time_genoff_opr( &temp_genoff_args );
			   
			   jul_val.year += iter2;
			   if (jul_val.year >= 2020)
			   	jul_val.year = 1990;
			   jul_val.day++;
			   if (jul_val.day >= 25)
			   	 jul_val.day = 1;
       	   	}
		   jul_val.month++;
		   if (jul_val.month >= 12)
		   	 jul_val.month = 1;
		   jul_val.minute+=iter1;
		   if (jul_val.minute >= 60)
		   	jul_val.minute = 0;
       	}

        /* Print debug stats */
        time_genoff_print_debug_stats();		   

	     /* Set time in different units (01 Jan 1990 00:00:00) */

         time_type                       ts_val = {0,0};

         temp_genoff_args.base        = ATS_TOD; 
         temp_genoff_args.base_source = TIME_SCLK;
         temp_genoff_args.ts_val      = ts_val;

         temp_genoff_args.operation   = T_SET;
         temp_genoff_args.dal_timetick_val_valid = 0;
         temp_genoff_args.dal_timetick_val       = 0;
		 
         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
         ts_val[0]=0x12C96000;
		 ts_val[1]=0;
         temp_genoff_args.unit        = TIME_SECS;

         time_genoff_opr( &temp_genoff_args );
		 
         /* Print debug stats */
         time_genoff_print_debug_stats();		 
         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
         ts_val[0]=0x629F0000;
		 ts_val[1]=0x49;
         temp_genoff_args.unit        = TIME_MSEC;
         time_genoff_opr( &temp_genoff_args );

         /* Print debug stats */
         time_genoff_print_debug_stats();		 
         /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
         ts_val[0]=0;
		 ts_val[1]=0x003ab54c;
         temp_genoff_args.unit        = TIME_STAMP;		 
         time_genoff_opr( &temp_genoff_args );

    	 /* Print debug stats */
	     time_genoff_print_debug_stats();		 
	 }
	 break;
     
     case TIME_AUTO_SET_TEST:
     {
        time_setting_num = (uint32)req->command_data[0];
        time_setting_curr = 0;
        
        ATS_ERR_1("TIME_DIAG: TIME_AUTO_SET_TEST called for %d sets ", time_setting_num);
        
        timer_undef(&time_test_timer);
        timer_def_osal(&time_test_timer, &time_test_group, TIMER_FUNC1_CB_TYPE, test_time_setting, 0);
        timer_set_64(&time_test_timer, 1, 0, T_SEC);
     }
     break;
     
     case TIME_AUTO_SET_WITH_TIMETICK_TEST:
     {
        send_with_timetick = (uint32)req->command_data[0];
        ATS_ERR_1("TIME_DIAG: Changed send_with_timetick to %d ", send_with_timetick);
     }
     break;

     case TIME_DIAG_ATS_PRIVATE_BASE_GENERIC_TEST:
  	  {
	  	/* Holder for the filename for a particular base */
	  	char f_name[120];
		ats_error_type ret_error = ATS_FAILURE;
		uint32 base_num = 0;
		uint32 time_diag_index_for_operation = 0;
		uint32 index = 0;
		time_julian_type jul_val;
		time_genoff_args_struct_type temp_genoff_args;

		/* EXAMPLE COMMANDS : 
		 *
		 * PRINT DIAG PRIVATE TEST BASE DETAILS 
		 * send_data 0x4B 0x49 0x00 0x00 34 0x0 0x00 0x00
		 * 
		 * INIT a PRIVATE Base
		 * send_data 0x4B 0x49 0x00 0x00 34 0x0 0x01 0x00 <INDEX_NUM> 0x0
		 *
		 * DE-INIT
		 * send_data 0x4B 0x49 0x00 0x00 34 0x0 0x02 0x00 <INDEX_NUM> 0x0
		 * 
		 * GET TIME
		 * send_data 0x4B 0x49 0x00 0x00 34 0x0 0x03 0x00 <INDEX_NUM> 0x0
		 *
		 * SET TIME
		 * send_data 0x4B 0x49 0x00 0x00 34 0x0 0x04 0x00 <INDEX_NUM> 0x0 <TIME>
		 */

		if(req->command_data[0] == 0)  /* <CMD> == 0 : for Printing all test bases detail */
			{
			for(index = 0; index<ATS_PRIVATE_MAX; index++)
				{
				ATS_MSG_SPRINTF("INDEX=%d; Base_num = 0x%.8x; filename = %s",
					               index,
					               time_diag_private_test_bases[index].base_num,
					               time_diag_private_test_bases[index].f_name);
				}
			}
	  	else if(req->command_data[0] == 1)  /* <CMD> == 1 : INIT operation */
	  		{
	  		time_diag_index_for_operation = req->command_data[1];

	  		(void) snprintf( f_name,sizeof(f_name),"diag_priv_%d",(int)time_diag_index_for_operation);
			
	  		ret_error = time_genoff_private_base_init(f_name,&base_num);
			ATS_MSG_SPRINTF("ATS_PRIVATE INIT f_name=%s;base_num=0x%.8x,error=%d",f_name,base_num,ret_error);

			if(ret_error==ATS_SUCCESS)
				{
				time_diag_private_test_bases[time_diag_index_for_operation].base_num = base_num;
				strlcpy(time_diag_private_test_bases[time_diag_index_for_operation].f_name,
					     f_name,MAX_EFS_FILE_NAME_SIZE);
				}
	  		}
		else if(req->command_data[0] == 2)  /* <CMD> == 2 : DE-INIT operation */
			{
			time_diag_index_for_operation = req->command_data[1];
			base_num = time_diag_private_test_bases[time_diag_index_for_operation].base_num;

			strlcpy(f_name,time_diag_private_test_bases[time_diag_index_for_operation].f_name,MAX_EFS_FILE_NAME_SIZE);

			ret_error = time_genoff_private_base_deinit(base_num,f_name);
			ATS_MSG_SPRINTF("ATS_PRIVATE DEINIT f_name=%s;base_num=0x%.8x,error=%d",f_name,base_num,ret_error);
			if(ret_error==ATS_SUCCESS)
				{
				time_diag_private_test_bases[time_diag_index_for_operation].base_num = 0;
				memset(time_diag_private_test_bases[time_diag_index_for_operation].f_name,0,MAX_EFS_FILE_NAME_SIZE);
				}
			}
		else if(req->command_data[0] == 3)  /* <CMD> == 3 : GET TIME operation */
			{
			time_diag_index_for_operation = req->command_data[1];
			base_num = time_diag_private_test_bases[time_diag_index_for_operation].base_num;
			
			temp_genoff_args.base		 = base_num; 
			temp_genoff_args.base_source = TIME_SCLK;
			temp_genoff_args.ts_val 	 = (void *)(&jul_val);
			temp_genoff_args.unit		 = TIME_JULIAN;
			temp_genoff_args.operation	 = T_GET;
			temp_genoff_args.dal_timetick_val_valid = 0;
			temp_genoff_args.dal_timetick_val		= 0;
			time_genoff_opr( &temp_genoff_args );
			
			ATS_MSG_SPRINTF_2("TIME_GET on private base=0x%.8x; f_name=%s",
				              base_num,
				              time_diag_private_test_bases[time_diag_index_for_operation].f_name);
			ATS_ERR_6("TIME = %.2d/%.2d/%.4d %.2d:%.2d:%.2d",
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second);
			}
		else if(req->command_data[0] == 4)  /* <CMD> == 4 : SET TIME operation */
			{
			time_diag_index_for_operation = req->command_data[1];
			base_num = time_diag_private_test_bases[time_diag_index_for_operation].base_num;

			jul_val.year 	   = req->command_data[2];
			jul_val.month	   = req->command_data[3];
			jul_val.day		   = req->command_data[4];
			jul_val.hour 	   = req->command_data[5];
			jul_val.minute	   = req->command_data[6];
			jul_val.second	   = 0;
			jul_val.day_of_week = 1;			
			ATS_MSG_SPRINTF_2("TIME_SET on private base=0x%.8x; f_name=%s",
				              base_num,
				              time_diag_private_test_bases[time_diag_index_for_operation].f_name);
			ATS_ERR_6("TIME = %.2d/%.2d/%.4d %.2d:%.2d:%.2d",
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second);

			temp_genoff_args.base		 = base_num; 
			temp_genoff_args.base_source = TIME_SCLK;
			temp_genoff_args.ts_val 	= (void *)(&jul_val);
			temp_genoff_args.unit		= TIME_JULIAN;
			temp_genoff_args.operation	= T_SET;
			temp_genoff_args.dal_timetick_val_valid = 0;
			temp_genoff_args.dal_timetick_val	   = 0; 	
			time_genoff_opr( &temp_genoff_args );

			}	
  	  }
     break;

	 /* Example Command : 
	   * send_data 0x4B 0x49 0x00 0x00 35 0x0 0xC1 0x07 0x03 0x00 0x0A 0x00 0x05 0x00 0x0A 0x00 0x0F 0x00 0x03 00
	   * This command will start the time set stress test from 10th Mar 1985, 5:10:15, 3rd day of week */
     case TIME_DIAG_ATS_PRIVATE_BASE_SET_STRESS_TEST:
  	  {
	   /* Arguments to be sent to set Private bases */
	   time_genoff_args_struct_type    temp_genoff_args;
	 
	   /* Time julian Type */
	   time_julian_type 			   jul_val;
	   uint32 iter = 0, iter2 = 0;
	 
	   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
	 
	   jul_val.year 	   = req->command_data[0];
	   jul_val.month	   = req->command_data[1];
	   jul_val.day		   = req->command_data[2];
	   jul_val.hour 	   = req->command_data[3];
	   jul_val.minute	   = req->command_data[4];
	   jul_val.second	   = req->command_data[5];
	   jul_val.day_of_week = req->command_data[6];

	   temp_genoff_args.base_source = TIME_SCLK;
	   temp_genoff_args.ts_val		= (void *)(&jul_val);
	   temp_genoff_args.unit		= TIME_JULIAN;
	   temp_genoff_args.operation	= T_SET;
	   temp_genoff_args.dal_timetick_val_valid = 0;
	   temp_genoff_args.dal_timetick_val	   = 0;

       for ( iter = 0 ; iter < ATS_PRIVATE_MAX ; iter++ )
       	{
       	   if(time_diag_private_test_bases[iter].base_num == 0)
       	   	{
       	   	ATS_ERR_1("No diag test private base @ index:%d",iter);
       	   	continue;
       	   	}

			  for ( iter2 = 0 ; iter2 < 6 ; iter2++ )
			   {
				  ATS_ERR_7("ATS_PRIVATE_SET_STRESS_TEST : BASE=%.2d %.2d/%.2d/%d %.2d:%.2d:%.2d",
						 time_diag_private_test_bases[iter].base_num,
						 jul_val.month,
						 jul_val.day,
						 jul_val.year,
						 jul_val.hour,
						 jul_val.minute,
						 jul_val.second );				 
			   
				  /* set the time */
				  temp_genoff_args.base = time_diag_private_test_bases[iter].base_num; 	  
				  time_genoff_opr( &temp_genoff_args );
				  
				  jul_val.year += iter2;
				  if (jul_val.year >= 2020)
				   jul_val.year = 1990;
				  jul_val.day++;
				  if (jul_val.day >= 25)
					jul_val.day = 1;
			   }
			  jul_val.month++;
			  if (jul_val.month >= 12)
				jul_val.month = 1;
			  jul_val.minute+=iter;
			  if (jul_val.minute >= 60)
			   jul_val.minute = 0;
      	}

        /* Print debug stats */
        time_genoff_print_debug_stats();	 
	 }
     break;

     case TIME_DIAG_ATS_PRIVATE_BASE_NEGATIVE_TEST:
  	  {
	  	/* Holder for the filename for a particular base */
	  	char f_name[120];
		ats_error_type ret_error = ATS_FAILURE;
		uint32 base_num = 0;
		uint32 index = 0;
		time_julian_type jul_val;
		time_genoff_args_struct_type temp_genoff_args;

		/* SAMPLE COMMANDS : 
		 *  N.B. : This needs that some bases be created using TIME_DIAG_ATS_PRIVATE_BASE_GENERIC_TEST
		 * 
		 * DE-INIT with incorrect filename
		 * send_data 0x4B 0x49 0x00 0x00 36 0x0 0 0x00
		 *
		 * INIT with large filename
		 * send_data 0x4B 0x49 0x00 0x00 36 0x0 1 0x00
		 *
		 * GET operation with invalid base number
		 * send_data 0x4B 0x49 0x00 0x00 36 0x0 2 0x00
		 *
		 * GET operation with valid but de-inited base number
		 * send_data 0x4B 0x49 0x00 0x00 36 0x0 3 0x00
		 *
		 * SET operation with invalid base
		 * send_data 0x4B 0x49 0x00 0x00 36 0x0 4 0x00
		 *
		 * SET operation with valid but de-inited base
		 * send_data 0x4B 0x49 0x00 0x00 36 0x0 5 0x00
		 * 
		 */

		if(req->command_data[0] == 0)  /* <CMD> == 0 : DE-INIT with incorrect filename */
			{
			/* Make sure that at least one Private base in inited for this test */
			ATS_ERR("ATS_PRIVATE -ve test : DE-INIT with incorrect filename");
			
			for(index = 0; index<ATS_PRIVATE_MAX; index++)
				{
				if (time_diag_private_test_bases[index].base_num!=0)
					{
				
					/* Do a de-init with incorrect filename */
					ret_error = time_genoff_private_base_deinit(time_diag_private_test_bases[index].base_num,"fname_diag");
					ATS_MSG_SPRINTF("ATS_PRIVATE DEINIT f_name=%s;base_num=0x%.8x,error=%d","fname_diag",base_num,ret_error);
					}
				}
			}
	  	else if(req->command_data[0] == 1)  /* <CMD> == 1 : INIT with large filename */
	  		{
	  		ATS_ERR("ATS_PRIVATE -ve test : INIT with large filename");
	  		(void) snprintf( f_name,sizeof(f_name),"diag_private_test_base");
			
	  		ret_error = time_genoff_private_base_init(f_name,&base_num);
			
			ATS_MSG_SPRINTF("ATS_PRIVATE INIT f_name=%s;base_num=0x%.8x,error=%d",f_name,base_num,ret_error);
	  		}
		else if(req->command_data[0] == 2)  /* <CMD> == 2 : GET operation with invalid base number */
			{
			ATS_ERR("ATS_PRIVATE -ve test : GET operation with invalid base number");
			base_num = ATS_PRIVATE + 2;
			
			temp_genoff_args.base		 = base_num; 
			temp_genoff_args.base_source = TIME_SCLK;
			temp_genoff_args.ts_val 	 = (void *)(&jul_val);
			temp_genoff_args.unit		 = TIME_JULIAN;
			temp_genoff_args.operation	 = T_GET;
			temp_genoff_args.dal_timetick_val_valid = 0;
			temp_genoff_args.dal_timetick_val		= 0;
			time_genoff_opr( &temp_genoff_args );
			
			ATS_ERR_6("TIME = %.2d/%.2d/%.4d %.2d:%.2d:%.2d",
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second);
			}
		else if(req->command_data[0] == 3)  /* <CMD> == 3 : GET operation with valid but de-inited base number */
			{
			ATS_ERR("ATS_PRIVATE -ve test : GET operation with valid but de-inited base number");
			/* Make sure that ATS_PRIVATE base number 0x01C3CA09, i.e. ats_private_bases[0x9] is deinited */
			base_num = 0x01C3CA09;
			
			temp_genoff_args.base		 = base_num; 
			temp_genoff_args.base_source = TIME_SCLK;
			temp_genoff_args.ts_val 	 = (void *)(&jul_val);
			temp_genoff_args.unit		 = TIME_JULIAN;
			temp_genoff_args.operation	 = T_GET;
			temp_genoff_args.dal_timetick_val_valid = 0;
			temp_genoff_args.dal_timetick_val		= 0;
			time_genoff_opr( &temp_genoff_args );
			
			ATS_ERR_6("TIME = %.2d/%.2d/%.4d %.2d:%.2d:%.2d",
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second);
			}
		else if(req->command_data[0] == 4)  /* <CMD> == 4 : SET operation with invalid base */
			{
			ATS_ERR("ATS_PRIVATE -ve test : SET operation with invalid base");
			base_num = ATS_PRIVATE + 2;;

			jul_val.year 	   = 2012;
			jul_val.month	   = 02;
			jul_val.day		   = 22;
			jul_val.hour 	   = 11;
			jul_val.minute	   = 44;
			jul_val.second	   = 37;
			jul_val.day_of_week = 2;			

			ATS_ERR_6("TIME = %.2d/%.2d/%.4d %.2d:%.2d:%.2d",
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second);

			temp_genoff_args.base		 = base_num; 
			temp_genoff_args.base_source = TIME_SCLK;
			temp_genoff_args.ts_val 	= (void *)(&jul_val);
			temp_genoff_args.unit		= TIME_JULIAN;
			temp_genoff_args.operation	= T_SET;
			temp_genoff_args.dal_timetick_val_valid = 0;
			temp_genoff_args.dal_timetick_val	   = 0; 	
			time_genoff_opr( &temp_genoff_args );

			}		
		else if(req->command_data[0] == 5)  /* <CMD> == 5 : SET operation with valid but de-inited base number */
			{
			ATS_ERR("ATS_PRIVATE -ve test : SET operation with valid but de-inited base number");
			base_num = 0x01C3CA09;

			jul_val.year 	   = 2012;
			jul_val.month	   = 02;
			jul_val.day		   = 22;
			jul_val.hour 	   = 11;
			jul_val.minute	   = 44;
			jul_val.second	   = 37;
			jul_val.day_of_week = 2;				

			ATS_ERR_6("TIME = %.2d/%.2d/%.4d %.2d:%.2d:%.2d",
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second);

			temp_genoff_args.base		 = base_num; 
			temp_genoff_args.base_source = TIME_SCLK;
			temp_genoff_args.ts_val 	= (void *)(&jul_val);
			temp_genoff_args.unit		= TIME_JULIAN;
			temp_genoff_args.operation	= T_SET;
			temp_genoff_args.dal_timetick_val_valid = 0;
			temp_genoff_args.dal_timetick_val	   = 0; 	
			time_genoff_opr( &temp_genoff_args );

			}
		else
			{
			ATS_ERR_1("Incorrect command %d",req->command_data[0]);
			}
				
  	  }
     break;
   
     default :
     /* for un-implemented and wrong commands */
       ATS_ERR_1("time_services_test : invalid command - %d",req->command);
       break;
  }

  (void)pkt_len;

  return (rsp_ptr);

} /* time_services_test */


/*===========================================================================

              FUNCTION DISPATCH TABLE FOR DIAG COMMANDS

DESCPRITIION  
  TIME_SERVICES_TEST_F needs to be defined to a value in diagcmd.h if
no subsys ID is used to register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static const diagpkt_user_table_entry_type time_services_test_tbl[] =
{
  {TIME_SERVICES_TEST_F,TIME_SERVICES_TEST_F,time_services_test}
};


/*===========================================================================

FUNCTION TIME_SERVICES_TEST_INIT

DESCRIPTION
  Registers the diag packet function dispatch table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void time_services_test_init (void)
{
 /* Could use the following registration without specifying the processor */
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_TIME, time_services_test_tbl);

}

