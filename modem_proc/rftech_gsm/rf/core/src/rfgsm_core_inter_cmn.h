#ifndef RF_INTERRAT_H
#define RF_INTERRAT_H

/*!
   @file
   rfgsm_core_inter_cmm.h

   @brief
   This file contains declarations and definitions to support WCDMA Compressed
  Mode and WCDMA/GSM interRAT.

   @details
                 R F  I N T E R R A T,  I N T E R F R E Q,  I N T E R B A N D
                              H E A D E R   F I L E

*/
/*===========================================================================
Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_inter_cmn.h#1 $ $DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/09   sr      nv include file change
09/03/08   dw      Removed dm support.
10/16/06   lcl     Increased W2G start script from 12 to 14 and decreased
                   W2G stop script from 17 to 15--no change in FW memory.
04/18/06   lcl     Increased WTW SBI transactions to 23 and changed DM
                   definition name.
02/08/06   lcl     Added 4 more DM transactions during interBand CM.
01/30/06   jfc     Split rfwcdmacm_mdsp_dm_type into _list_ and _data_ type
11/29/05   jfc     Changed filename to rf_inter_cmn.h
11/15/05   ka       Added prototypes.
11/11/05   rsr/jfc  Added ctrl_fifo field to rfwcdmacm_sbi_trans_type.
10/25/05   jfc     Updates to cohere to coding standard.
09/06/05   ka      Added RFWCDMACM_MDSP_IFREQ_BUF_SIZE_MAX.
08/27/05   xw      Created.

=============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "rfcommon_nv_mm.h"

#define SET_AT_RUNTIME 0

#ifdef FEATURE_RF_ENABLE_CM                                            

/* Change the three #define below to calculate based on RFWCDMACM_MDSP_BUFFER* in rfllumts.h ????*/
#define RFWCDMACM_MAX_NUM_WTOW_SBI_SCRIPTS            23
#define RFWCDMACM_MAX_NUM_WTOG_START_SBI_SCRIPTS      14
#define RFWCDMACM_MAX_NUM_WTOG_STOP_SBI_SCRIPTS       15

#define RFWCDMACM_MDSP_WTOG_START_RF_BUF_IDX          RFWCDMACM_MDSP_BUFFER_CM_START_INDEX
#define RFWCDMACM_MDSP_WTOG_STOP_RF_BUF_IDX           RFWCDMACM_MDSP_BUFFER_CM_STOP_INDEX

#endif /*FEATURE_RF_ENABLE_CM*/



#endif  /* RF_INTERRAT_H */


