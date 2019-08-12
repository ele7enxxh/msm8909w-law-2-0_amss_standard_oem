/*=============================================================================

FILE:         QDSS.c

DESCRIPTION:  This file has the implementation of QDSS lite for RPM.

================================================================================
              Copyright (c) 2012 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/lite/src/QDSS.c#2 $
==============================================================================*/

#include "qdss_fuse_check.h"
#include "qdss_imem.h"
#include "npa.h"
#include "ClockDefs.h"
#include "time_service.h"
#include "QDSSLite.h"
#include "tracer_event_ids.h"
#include "etm.h"


/* If the build supplies these values we initialize the 
   trace sink to system memory, otherwise the default trace sink is ETB */
#if (QDSS_ETR_RAM_BASE > 0) &&  (QDSS_ETR_RAM_SIZE > 0)
#define QDSS_INIT_ETR 1
#endif


int qdss_trace_enable=0; //Turn this flag on to enable QDSS trace on boot up.

//Turn this flag on to enable processor halt on watch-dog and bus hang HW events.
//#define QDSS_CTI_ENABLE 1

//inline to reduce space
#include "STMTrace.c"    
#include "ETFETB.c"        
#include "ETR.c"         
#include "TFunnel.c"     
#include "CSR.c"
#include "CTI.c"

enum {
   QDSS_UNINITIALIZED,
   QDSS_BASE_INIT,
   QDSS_TRACE_ENABLED,
};

struct qdss_context {
   uint8 init_level;
   npa_client_handle npa_client;
   uint8 num_ports_in_use;
   uint64 mpm_ts_counter_premin;
   uint64 mpm_ts_counter_postmin;
   uint64 qdss_ts_counter_premin;
   uint32 qdss_to_mpm_ratio;
   uint32 ts_freq;
   uint32 port;
   boolean  etr_trace_disabled; 
   uint32 systest_ctl_addr;
   boolean etm_trace_on;
};

static struct qdss_context qdss; //zero initialized

//MPM counter is incrementing at 19.2 MHz.
// TBD: Table 4-6 in QDSS HPG mentions it is 32.768 kHz
#define QDSS_TO_MPM_RATIO_150  7.8125   //QDSS TS counter at 150 MHz
#define QDSS_TO_MPM_RATIO_300  15.625   //QDSS TS counter at 300 MHz
#define QDSS_TO_MPM_RATIO_600  31.25    //QDSS TS counter at 600 MHz
#define QDSS_TO_MPM_RATIO_19   1        //QDSS TS counter at 19.2 MHz


#define QDSS_TS_FREQ_150  150000000
#define QDSS_TS_FREQ_300  300000000
#define QDSS_TS_FREQ_600  600000000
#define QDSS_TS_FREQ_19    19200000


static void QDSSSaveTimestamp(void)
{
   HAL_qdss_csr_AccessUnLock();
   HAL_qdss_csr_DisableTimestampGenerator();
   HAL_qdss_csr_TriggerTimestampCapture();
   qdss.qdss_ts_counter_premin = HAL_qdss_csr_GetTimestampCounter();
   qdss.mpm_ts_counter_premin = HAL_qdss_csr_GetMPMCounter();
   HAL_qdss_csr_EnableTimestampGenerator();
   HAL_qdss_csr_AccessLock();
}


static void QDSSRestoreTimestamp(void)
{
   uint64 minimization_period;
   uint64 qdss_ts_counter_postmin;

   HAL_qdss_csr_AccessUnLock();
   HAL_qdss_csr_DisableTimestampGenerator();

   HAL_qdss_csr_TriggerTimestampCapture();

   qdss.mpm_ts_counter_postmin = HAL_qdss_csr_GetMPMCounter();
   minimization_period = qdss.mpm_ts_counter_postmin - qdss.mpm_ts_counter_premin;

   qdss_ts_counter_postmin =qdss.qdss_ts_counter_premin +
      (minimization_period * qdss.qdss_to_mpm_ratio);

   
   HAL_qdss_csr_SetTimestampCounter(qdss_ts_counter_postmin);
   HAL_qdss_csr_EnableTimestampGenerator();
   HAL_qdss_csr_AccessLock();
}

static void QDSSSyncWithMPM(void)
{
   //This will initialize the QDSS timestamp value
   //to the MPM counter value scaled to the current QDSS frequency
   QDSSRestoreTimestamp(); 
}


/*============================================================================
 * @brief Iniitialize system initialization time measurement
 *==========================================================================*/
static void QDSSSysTestInit(void) 
{
   uint32 systest_ctl;
   qdss.systest_ctl_addr = qdss_imem_base();

   systest_ctl = qdss_get_systest_ctl(qdss.systest_ctl_addr);
   if (0 == systest_ctl) {
      return;  //system init test not requested
   }
   
   if (systest_ctl & QDSS_SYSTEST_RPM_INIT) {
      qdss_trace_enable = 1;
      systest_ctl |= QDSS_SYSTEST_EVENTS_ENABLE;
      systest_ctl &= ~QDSS_SYSTEST_RPM_INIT; //reset for next reboot
   }
   else {
      systest_ctl &= ~QDSS_SYSTEST_EVENTS_ENABLE;
   }
   __outpdw(qdss.systest_ctl_addr,systest_ctl);
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
void QDSSInit()
{
   qdss.init_level=QDSS_UNINITIALIZED;
   qdss.etr_trace_disabled = FALSE;


#ifdef QDSS_CTI_ENABLE
   qdss_trace_enable = 1;
#endif

   QDSSSysTestInit();
 
   qdss.qdss_to_mpm_ratio = 1; //default

   qdss.npa_client = npa_create_sync_client("/clk/qdss", "qdssrpm", NPA_CLIENT_REQUIRED);
   if (NULL== qdss.npa_client) {
      return;
   }
   npa_issue_required_request(qdss.npa_client,CLOCK_QDSS_LEVEL_DEBUG);

   STMCfgPreInit(qdss.ts_freq);
   
   /*Abort initialization if Non-Secure non-invasive debug is disabled*/
    if (FALSE==STMAccessStatusNSNID())
	{
		npa_issue_required_request(qdss.npa_client,CLOCK_QDSS_LEVEL_OFF);
		return;
	}
   
   CSRInit();
   QDSSSyncWithMPM();
   ETRPreInit();
   ETFETBPreInit();
   TFunnelPreInit();
   qdss.init_level=QDSS_BASE_INIT;
   
   if (qdss_trace_enable) {
      ETRInit();
      ETFETBInit();
      TFunnelInit();
      STMCfgInit();
#ifdef QDSS_CTI_ENABLE
      CTIInit();
#endif 
      qdss.init_level = QDSS_TRACE_ENABLED;
   }
   else {
      npa_issue_required_request(qdss.npa_client,CLOCK_QDSS_LEVEL_OFF);
   }

   if (DAL_SUCCESS != QDSSAllocPort(&qdss.port)) {
      return;
   }

   if (QDSS_SYSTEST_EVENTS_ENABLE &
       qdss_get_systest_ctl(qdss.systest_ctl_addr)) {
      QDSSEvent(qdss.port,QDSS_INIT_COMPLETE);
   }
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QDSSAllocPort(uint32 *pPort)
{
   int retval;

   if (qdss.init_level < QDSS_BASE_INIT) {
      return DAL_ERROR;
   }
   
   retval = STMTrace_NewPort(pPort);
   if (DAL_SUCCESS == retval) {
      qdss.num_ports_in_use++;
   }
   return retval;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QDSSFreePort(uint32 port)
{

   if (qdss.init_level < QDSS_BASE_INIT) {
      return DAL_ERROR;
   }

   STMTrace_FreePort(port);
   qdss.num_ports_in_use--;

   return DAL_SUCCESS;
}


static __inline uint32 endian_swap32(uint32 x)
{
    return (x>>24) | 
       ((x<<8) & 0x00FF0000) |
       ((x>>8) & 0x0000FF00) |
       (x<<24);
}


static __inline int ost_start_packet(uint32 port,uint32 ost_header)
{ 
   STMTrace_Data32(port,                  
                   TRACE_DATA_MARKED,
                   endian_swap32(ost_header));
   return DAL_SUCCESS;
}

static __inline int ost_end_packet(uint32 port)
{ 

   STMTrace_Flag(port,                  
                 TRACE_DATA_TIMESTAMPED);
     
   return DAL_SUCCESS;
}


static __inline int ost_write_data(uint32 port, byte *pCurrent, int nRemaining)
{
   //Make pCurrent 32 bit aligned first
   while (( (uint32)pCurrent & 0x3) && (nRemaining > 0 )) {                                      
      STMTrace_Data8(port,                              
                     TRACE_DATA_NONETYPE,
                     *pCurrent);
      pCurrent++;
      nRemaining--;
   }

   
#define OST_WRITE_DATA_WORDS(_nBytes,_nBits)                            \
   while (nRemaining >= _nBytes) {                                      \
      STMTrace_Data##_nBits(port,                                       \
                            TRACE_DATA_NONETYPE,                        \
                            (*(uint##_nBits *)pCurrent)); \
      pCurrent += _nBytes;                                              \
      nRemaining -=_nBytes;                                             \
   }

   OST_WRITE_DATA_WORDS(4,32);
   OST_WRITE_DATA_WORDS(2,16);
   OST_WRITE_DATA_WORDS(1,8);

   return DAL_SUCCESS;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/

#define OST_TOKEN_SWEVENT      0x80
#define PACK_EVENTID(_id) ((((uint32)(_id)) << 8 ) | OST_TOKEN_SWEVENT)

int QDSSEvent(uint32 port, int event_id)
{
   if (qdss.init_level < QDSS_TRACE_ENABLED) {
      return DAL_ERROR;
   }

   STMTrace_Data32(port,                                     
                   (TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED),
                   PACK_EVENTID(event_id));

   return DAL_SUCCESS;
}


static int WriteOSTPacket(uint32 port, uint32 ost_header, byte *packet, int nLen)
{
   if (nLen < 1) {
      return -1;
   }

   ost_start_packet(port,ost_header);
   ost_write_data(port,packet,nLen);
   ost_end_packet(port);
   return DAL_SUCCESS;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QDSSWriteOSTPacket(uint32 port, uint32 ost_header, byte *packet, int nLen)
{
   if (qdss.init_level < QDSS_TRACE_ENABLED) {
      return DAL_ERROR;
   }

   return WriteOSTPacket(port,ost_header,packet,nLen);
}

#define  QDSS_TRACESINK_UNKNOWN -1
#define  QDSS_TRACESINK_ETB      0
#define  QDSS_TRACESINK_RAM      1
#define  QDSS_TRACESINK_USB      2


static __inline int QDSSGetTraceSink(void) 
{
   int mode;
   HAL_qdss_etfetb_AccessUnLock();
   HAL_qdss_csr_AccessUnLock();
   switch (HAL_qdss_etfetb_GetMode()) {

   case  QDSS_TMC_MODE_CBUF:
      mode = QDSS_TRACESINK_ETB;
      break;
   case QDSS_TMC_MODE_HWFIFO:
      if (HAL_qdss_csr_IsUSBEnabled()) {
         mode = QDSS_TRACESINK_USB;
      }
      else {
         mode = QDSS_TRACESINK_RAM;
      }
      break;
   default:
      mode = QDSS_TRACESINK_UNKNOWN;
   }
   HAL_qdss_csr_AccessLock();
   HAL_qdss_etfetb_AccessLock();
   return mode;
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QDSSFlush(void) 
{
   if (qdss.init_level < QDSS_TRACE_ENABLED) {
      return DAL_SUCCESS;
   }

   switch(QDSSGetTraceSink()) {
   case QDSS_TRACESINK_ETB:
      ETFETBFlush();
      break;
   case QDSS_TRACESINK_RAM:
      ETRFlush();
      break;
   case QDSS_TRACESINK_USB:
      ETRFlush();
      USBFlush();
      break;
   default:
      break; //do nothing
   }
   return DAL_SUCCESS;
}

extern boolean qdss_is_swevent_enabled(tracer_event_id_t id);

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
void QDSSDDRTraceDisable(void) 
{
   if (!qdss.etr_trace_disabled) {
      if ( HAL_qdss_etr_IsEnabled() ) {
         HAL_qdss_etfetb_AccessUnLock();
         if (HAL_qdss_etfetb_IsEnabled()) {
            uint32 ffcr_saved;
            qdss.etr_trace_disabled = TRUE;
            ffcr_saved = HAL_qdss_etfetb_GetFFCR();
            HAL_qdss_etfetb_StopOnFlush(1);
            HAL_qdss_etfetb_Flush();
            HAL_qdss_etfetb_FlushWait();
            HAL_qdss_etfetb_WaitForTMCReadyVal(1);
            HAL_qdss_etfetb_DisableTraceCapture();
            HAL_qdss_etfetb_SetFFCR(ffcr_saved);
            HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_CBUF);
            HAL_qdss_etfetb_EnableTraceCapture();
            HAL_qdss_etfetb_WaitForTMCReadyVal(0);
         }
         HAL_qdss_etfetb_AccessLock();
      }
   }
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
void QDSSDDRTraceEnable(void) 
{
   boolean drain_etb = FALSE;
   uint32 ffcr_saved;

   if (qdss.etr_trace_disabled) {
      qdss.etr_trace_disabled = FALSE;
      HAL_qdss_etfetb_AccessUnLock();

      if (HAL_qdss_stm_isEnabled()) {
         drain_etb = TRUE;
      }

      ffcr_saved = HAL_qdss_etfetb_GetFFCR();
      HAL_qdss_etfetb_StopOnFlush(1);
      HAL_qdss_etfetb_Flush();
      HAL_qdss_etfetb_FlushWait();
      HAL_qdss_etfetb_WaitForTMCReadyVal(1);
      if (drain_etb) {
         HAL_qdss_etfetb_Drain();
         HAL_qdss_etfetb_WaitForTMCReadyVal(1);
      }
      HAL_qdss_etfetb_DisableTraceCapture();
      HAL_qdss_etfetb_SetFFCR(ffcr_saved);
      HAL_qdss_etfetb_SetMode(QDSS_TMC_MODE_HWFIFO);

      HAL_qdss_etfetb_EnableTraceCapture(); 
      HAL_qdss_etfetb_WaitForTMCReadyVal(0);
      HAL_qdss_etfetb_AccessLock();

   }
}

#define QDSS_SWEVT_ENTITY    2
#define QDSS_SWEVT_PROTOCOL  1  


/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
void QDSSSTMSetTimestampFreq(uint32 dwFreq)
{
   qdss.ts_freq=dwFreq; //set it pending

   //Set the ratio to use for qdss timestamp restoration 
   //after XO shutdown
   switch(dwFreq) {
   case QDSS_TS_FREQ_150:
      qdss.qdss_to_mpm_ratio = QDSS_TO_MPM_RATIO_150;
      break;
   case QDSS_TS_FREQ_300:
      qdss.qdss_to_mpm_ratio = QDSS_TO_MPM_RATIO_300;
      break;
   case QDSS_TS_FREQ_600:
      qdss.qdss_to_mpm_ratio = QDSS_TO_MPM_RATIO_600;
      break;
   case QDSS_TS_FREQ_19:
   default:
      qdss.qdss_to_mpm_ratio = QDSS_TO_MPM_RATIO_19;
   }

   if (qdss.init_level < QDSS_BASE_INIT) {
      return;
   }

   //This event is sent so that the host tool has a timestamp 
   //close to the FREQ change to keep track of the elapsed time
   if (0!=qdss.port) {
      uint32 message[4]; 
      uint64 timeval;

      timeval=time_service_now();
   
      message[0]=QDSS_TS_FREQ_CHANGE;
      message[1]=dwFreq;
      message[2]=(uint32) timeval ;  //lower 32 bit
      message[3]=(uint32) (timeval >> 32) ; //upper 32 bit
      

      WriteOSTPacket(qdss.port,
                     QDSSOSTHeader(QDSS_SWEVT_ENTITY,QDSS_SWEVT_PROTOCOL),
                     (byte *)message,
                     sizeof(message));
   }

   STMSetTimestampFreq(dwFreq);
}


uint8 qdss_ts_wa_delay_us = 0; //QDSS timestamp workaround delay in microseconds
void busywait (uint32 pause_time_us);
/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
void QDSSPreXOShutdown(void) 
{
   if (qdss.init_level < QDSS_BASE_INIT) {
      return;
   }

   QDSSSaveTimestamp();

   if (qdss_ts_wa_delay_us) {
      busywait(qdss_ts_wa_delay_us);
   }
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
void QDSSPostXOShutdown(void) 
{
   if (qdss.init_level < QDSS_BASE_INIT) {
      return;
   }

   QDSSRestoreTimestamp();

   if (qdss_ts_wa_delay_us) {
      busywait(qdss_ts_wa_delay_us);
   }
}

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QDSSEnableTrace(void) 
{
   if (qdss.init_level < QDSS_BASE_INIT) {
      return DAL_ERROR;
   }

   npa_issue_required_request(qdss.npa_client,CLOCK_QDSS_LEVEL_DEBUG);
   qdss.init_level = QDSS_TRACE_ENABLED;
   return DAL_SUCCESS;
}

extern boolean Clock_IsQDSSOn(void);

/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QDSSDisableTrace(void) 
{
   if (qdss.init_level < QDSS_BASE_INIT) {
      return DAL_ERROR;
   }

   npa_issue_required_request(qdss.npa_client,CLOCK_QDSS_LEVEL_OFF);

   if (!Clock_IsQDSSOn()) {
      qdss.init_level = QDSS_BASE_INIT;
   }
      return DAL_SUCCESS;
   }

#define RPM_ETM_TOGGLE               1
/*============================================================================
 * @brief QDSS Lite API function. See QDSSLite.h for details
 *==========================================================================*/
int QDSSETMControl(uint32 option, uint64 value) 
{
   int nErr = DAL_ERROR;

   if (qdss.init_level < QDSS_BASE_INIT) {
      return nErr;
   }

   switch (option) {
   case RPM_ETM_TOGGLE:
      if (value) {
         ETM_trace_enable();
         qdss.etm_trace_on = TRUE;
         nErr = DAL_SUCCESS;
      }
      else {
         if (qdss.etm_trace_on) {
            qdss.etm_trace_on = FALSE;
            ETM_trace_disable();
            nErr = DAL_SUCCESS;
         }
      }
      break;
   default:
      nErr = DAL_ERROR;
   }
   return nErr;
}
