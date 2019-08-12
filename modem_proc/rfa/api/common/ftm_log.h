#ifndef FTM_LOG_H
#define FTM_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM Log

GENERAL DESCRIPTION
  This is the header file for FTM Logging.

Copyright (c) 2003-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/ftm_log.h#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/10/13   aro     Migrated FTM Log ID
07/09/13   aca     ATUNER logging
04/05/13   gvn     Add new log for IRAT Timing info 
02/26/13   gvn     Add new LTE FTM log for CA enabled RxAGC 
07/16/12   hy      Add TD-SCDMA FTM Rx AGC and Tx AGC logs. 
02/10/12   sb      Changes to support FTM logging for GSM + removed log_dmss.h 
12/05/11   whc     LTE SMPS logging / Enhanced TXAGC log support
07/11/11   hy      Including TD-SCDMA support.
05/09/11   sar     Removed ftmicap.h for cmi-4 cleanup.
03/31/11   sar     Removed conditional definitions for CMI-4.
01/25/11   sr      removed the clk.h dependency
09/10/10   ems     Added support for FTM sequencer.
07/28/10   mkv     Intial version of LTE Tx and Rx AGC log packets.  
12/10/09   bn      FTM WCDMA ENH log packet 
10/25/09   dyc     Updates for SCMM mode specific builds.
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
09/17/09   sar     Updated clk interface for CMI conformance.
09/13/09   vb      ICI calibration support
02/26/08   dw      Fixed high Lint warning (546)
12/14/07   ad      Added ftm_log_gsm_therm_vbatt_type as fix for uninitialized 
                   pointer issue in log packet in ftm_gsm_ctl.c/ftm_tdma_isr()
06/25/07   ad      Added support for single ended BER
02/05/07   ycl     Added FTM_LOG_WCDMA_TXPLIM log ID.
12/06/06   jfc     Added definition of ftm_log_gen_type
11/20/06   jfc     Added definition of log_ftm_gen_type
11/16/06   vm/jfc  Allow FTM logging to be mode independent     
10/17/06   ycl     Modifications for initial MSM7600 build.
06/26/06   et      GSM QUADBAND support
02/01/06   rsr     Lint fixes.
09/12/05   hkk     Changes for TX/RX sweep cal 
06/19/05   jac     Again, made log ID param of ftm_log_malloc() uint16 so log codes
                   larger than 0xFF can always be used.
06/14/05   rsr     Added log support for therm and vbatt
05/09/05   bmg     Added IntelliCeiver log
03/07/05   bmg     Made log ID param of ftm_log_malloc() uint16 so log codes
                   larger than 0xFF can always be used.
12/09/04   rsr     Added support for autocalibration
11/12/04   ra      Add FTM_LOG_HDET_TRACKING
10/12/04   jac     Mainlined ASYNC packet code.
09/16/04   dp      Add WCDMA AGC and FTM GSM BER log codes
09/08/04   ra      1X merge with UMTS
10/17/03   ka      Removed expiration parameter from log enable API.
10/15/03   ka      Added data structures for flexible callback-based logging.
10/08/03   ka      Added new ftm log types for DC loop monitoring.
09/16/03   wd      Added define for FTM_LOG_HEADER_SIZE wich returns the size
                   of the FTM LOG Header with out the data.
09/12/03   wd      Initial revision.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/


#include "customer.h"
#include "target.h"

#include "ftm.h"
#include "ftmdiag.h"


#include "log_codes.h"
#include "time_svc.h"   /* Replaces clk.h*/
#include "time_tod.h"   /* Replaces clk.h*/
#include "DALSys.h"
#include "log.h"
#include "ftm_log_id.h"


/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                              DEFINITIONS

===========================================================================*/
#define FTM_LOG_ELEMENT_COUNT(x) ( sizeof(x) / sizeof(x[0]))

/* FTM LOG Header size with out data, since the data size depends on the 
   FTM log id 
*/

typedef enum {
  FTM_LOG_ENABLE,
  FTM_LOG_DISABLE,
  FTM_LOG_CMD_MAX,
} ftm_log_cmd_type;

typedef PACK(struct)
{
  log_hdr_type hdr;
  word         ftm_log_id;      /* FTM log id */
} ftm_log_gen_type;

//typedef log_ftm_gen_type ftm_log_gen_type ;

#define FTM_LOG_HEADER_SIZE (sizeof(ftm_log_gen_type))

typedef PACK(union)
{
  log_hdr_type hdr;       /* Generic header.                     */
} ftm_log_type;
  

typedef PACK(struct) {
  ftm_log_gen_type  header;
  byte              lna_state;
  uint16            dacc_iaccum;
  uint16            dacc_qaccum;
  uint32            dacc_est_ioff;
  uint32            dacc_est_qoff;
  uint32            fg_ioffset;
  uint32            fg_qoffset;
} ftm_log_dacc_type;

typedef PACK(struct)
{
  ftm_log_gen_type   header;
  int16              therm_recorded;        
  int16              temp_comp_bin_index;   
  int16              temp_comp_rem;         
  int16              temp_comp_val;         
  int16              vbatt_recorded;       
  int16              vbatt_backoff;        
} ftm_log_gsm_therm_vbatt_type;


typedef PACK(struct)
{
  uint16          log_id;
  int4            interval;
  int             num_buffer;
} ftm_log_packed_param_type;

/* This type specifies the full request packet (including diag subsystem header) for configuration of
 * a particular FTM log id 
 *
 */
typedef PACK(struct)
{
  diagpkt_subsys_header_type  diag_hdr;
  ftm_cmd_header_type         ftm_hdr;
  ftm_log_packed_param_type   params;
} ftm_log_req_pkt_type;

typedef void (*ftm_log_cb_func_ptr_type)(int4);

typedef struct
{
  timer_type              *cb_struct;
  ftm_log_cb_func_ptr_type cb_func;
  boolean                  active;
  int                      num_buffer;
} ftm_log_config_type;

typedef void (*ftm_log_init_fn_type)( void);
typedef void (*ftm_log_exit_fn_type)( void);


typedef struct
{
  ftm_log_id_type            log_id;
  ftm_log_config_type        log_config;
  ftm_log_init_fn_type       log_init_func;
  ftm_log_exit_fn_type       log_exit_func;

} ftm_log_config_table_entry;

/*===========================================================================

                          FUNCTION PROTOTYPES

===========================================================================*/
ftm_log_config_table_entry*   ftm_log_find_log_id( ftm_log_id_type   ftm_log_id);

/*===========================================================================

FUNCTION FTM_LOG_MALLOC

DESCRIPTION
   This function allocates memory for an ftm lok pkt.

DEPENDENCIES
   None.

===========================================================================*/
PACKED void * ftm_log_malloc( uint16 ftm_log_id, unsigned int length);

/*===========================================================================

FUNCTION FTM_LOG_GET_DATA_PTR

DESCRIPTION
   This function returns a ptr to the data portion of a given ftm log pkt.

DEPENDENCIES
   None.

===========================================================================*/
PACKED void * ftm_log_get_data_ptr (void *log_pkt);

/*===========================================================================

FUNCTION FTM_LOG_PRINTF

DESCRIPTION
  This function is a printf clone, it will work just as printf does, but 
  will output logged to the dm monitor. 


DEPENDENCIES
   None.

===========================================================================*/
void ftm_log_printf(const char *fmt, ... );

/*===========================================================================

FUNCTION FTM_LOG_DISPATCH

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None. 

===========================================================================*/
ftm_rsp_pkt_type ftm_log_dispatch(ftm_log_req_pkt_type  *cmd_ptr);

void ftm_log_enable(ftm_log_id_type log_id, int4 interval, int num_buffer);
void ftm_log_disable(ftm_log_id_type log_id);

#endif /* FTM_LOG_H */
