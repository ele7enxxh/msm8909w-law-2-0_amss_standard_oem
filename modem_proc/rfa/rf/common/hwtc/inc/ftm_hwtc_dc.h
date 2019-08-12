/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM HWTC DC 

GENERAL DESCRIPTION
  This is the header file for the embedded FTM DC Commands 

Copyright (c) 2003, 2007-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2010            by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/hwtc/inc/ftm_hwtc_dc.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/12   zhw     Remove hw.h. Compiler error fix
03/17/12   jfc     Remove clkrgm* header files
07/07/11   sar     Removed deprecated file.
01/11/10   sr      Deleted rficap.h
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
08/27/09   bmg     Removed unnecessary vocstate.h header
07/29/08   dw      Fixed nightly build compiling error 
07/25/08   ap      Featuring #include vocstate.h as requested by audio team 
03/17/07   jfc     Removing #include "mc.h". File no longer exists.
12-17-03   dp      Port to MSM6250
10-15-03   ka      Added prototype for ftm_hwtc_dc_read_regs.
09-30-03   ka      Created by katkinso.
===========================================================================*/

#ifndef FTM_HWTC_DC_H
#define FTM_HWTC_DC_H

#include "rfa_variation.h"
#if defined(FEATURE_FACTORY_TESTMODE)
#if defined(FEATURE_FTM_HWTC)

#include "comdef.h"
#include "msm.h"
#include "rfnv.h"
#include "diagpkt.h"
#include "rf.h"
#include "msg.h"
#include "err.h"
#include "ftm_hwtc_cmd.h"
#include "ftm_hwtc_iqcapture.h"
#include <string.h>

/* HWTC DC sub commands */
typedef enum{
  HWTC_DC_ACCUM_RD_ENABLE,
  HWTC_DC_ACCUM_RD_DISABLE,
  HWTC_DC_RD_THRESH,
  HWTC_DC_DRIFT_UPDATE_EN,
  HWTC_DC_SET_THRESH,
  HWTC_DC_SET_CONFIG,
  HWTC_MAX_DC_SUB_CMD
} ftm_hwtc_dc_sub_cmd_type;

typedef struct {
  ftm_hwtc_iq_config_type         iq_cfg;
  uint32                          dacc_est_offset;
} ftm_hwtc_config_type;

typedef PACK(struct) {
  byte                     interval;
  byte                     print_level;
  boolean                  log;
} ftm_hwtc_dc_param_type;

typedef PACK(union) {
  ftm_hwtc_dc_param_type   cfg;
  boolean                  enable;
  uint32                   thresh;
} ftm_hwtc_dc_ctl_params_type;

typedef PACK(struct){
  diagpkt_subsys_header_type       header;
  ftm_cmd_header_type              ftm_cmd_header;
  ftm_hwtc_dc_ctl_params_type      parms;
} ftm_hwtc_dc_ctl_pkt_type;



/*===========================================================================

FUNCTION FTM_HWTC_DACC_ACCUM_READ_ENABLE

DESCRIPTION
   This function will enable I and Q DACC accumulator value logging.
===========================================================================*/   
void ftm_hwtc_dc_read_enable(void);

/*===========================================================================

FUNCTION FTM_HWTC_DACC_ACCUM_READ_DISABLE

DESCRIPTION
   This function will disable I and Q DACC accumulator value logging.
===========================================================================*/  
void ftm_hwtc_dc_read_disable(void);

/*===========================================================================

FUNCTION FTM_HWTC_DC_DISPATCH

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions.
===========================================================================*/    
void ftm_hwtc_dc_dispatch(ftm_hwtc_dc_ctl_pkt_type *hwtc_pkt);

/*===========================================================================

FUNCTION ftm_hwtc_dc_read_regs

DESCRIPTION
   This function will read the appropriate I and Q DACC accum values.
===========================================================================*/
void ftm_hwtc_dc_read_regs(boolean log_en);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_DC_INIT

DESCRIPTION
  This function initializes the master DVT string table. 
               
DEPENDENCIES
  none

===========================================================================*/
void ftm_hwtc_dvt_dc_init (void);

#endif /* FEATURE_FTM_HWTC   */
#endif /* FEATURE_FACTORY_TESTMODE) */
#endif /* FTM_HWTC_DC_H */

