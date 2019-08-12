/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC definitions
***
***
*** DESCRIPTION
***
***  Contains definitions used by MAC that may be references by external
***  sources (i.e. RLC, GRR etc.).
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmac.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 06/27/01   SB      Created
***
*****************************************************************************/

#ifndef INC_GMAC_H
#define INC_GMAC_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif


/************************************************************
* PACKET ACCESS MODES used for PCCCH and CCCH packet access *
************************************************************/
/* Need to be higher in value than those defined for GSM in establishment_cause_T */
#define GMAC_ONE_PHASE_ACCESS     100
#define GMAC_TWO_PHASE_ACCESS     101
#define GMAC_SHORT_ACCESS         102
#define GMAC_SINGLE_BLOCK_ACCESS  103
#define GMAC_NO_PHASE_ACCESS      104

/* Access types used for EGPRS Packet Channel Request */
#define GMAC_EGPRS_ONE_PHASE_ACCESS  105
#define GMAC_EGPRS_TWO_PHASE_ACCESS  106
#define GMAC_EGPRS_SIGNALLING_ACCESS 107

/******************************************************************
* Retry Indicator.                                                *
* Indicates whether or not the MS has sent the Channel Request or *
* Packet Channel Request once or twice or more.                   *
******************************************************************/
#define GMAC_RETRY_SENT_REQ_ONCE            0
#define GMAC_RETRY_SENT_REQ_TWICE_OR_MORE   1

/********************************************************
* MULTISLOT CLASS SETTINGS (ONLY COVERS CLASSES 1 - 12) *
*-------------------------------------------------------*
* Taken from GSM 05.02                                  *
*********************************************************/
#define MULTISLOT_CLASS_1       0x00
#define MULTISLOT_CLASS_SUM_1   2
#define MS1_MAX_TS_RX           1
#define MS1_MAX_TS_TX           1
#define MS1_MIN_TS_TTA          3
#define MS1_MIN_TS_TTB          2
#define MS1_MIN_TS_TRA          4
#define MS1_MIN_TS_TRB          2

#define MULTISLOT_CLASS_2       0x01
#define MULTISLOT_CLASS_SUM_2   3
#define MS2_MAX_TS_RX           2
#define MS2_MAX_TS_TX           1
#define MS2_MIN_TS_TTA          3
#define MS2_MIN_TS_TTB          2
#define MS2_MIN_TS_TRA          3
#define MS2_MIN_TS_TRB          1

#define MULTISLOT_CLASS_3       0x02
#define MULTISLOT_CLASS_SUM_3   3
#define MS3_MAX_TS_RX           2
#define MS3_MAX_TS_TX           2
#define MS3_MIN_TS_TTA          3
#define MS3_MIN_TS_TTB          2
#define MS3_MIN_TS_TRA          3
#define MS3_MIN_TS_TRB          1

#define MULTISLOT_CLASS_4       0x03
#define MULTISLOT_CLASS_SUM_4   4
#define MS4_MAX_TS_RX           3
#define MS4_MAX_TS_TX           1
#define MS4_MIN_TS_TTA          3
#define MS4_MIN_TS_TTB          1
#define MS4_MIN_TS_TRA          3
#define MS4_MIN_TS_TRB          1

#define MULTISLOT_CLASS_5       0x04
#define MULTISLOT_CLASS_SUM_5   4
#define MS5_MAX_TS_RX           2
#define MS5_MAX_TS_TX           2
#define MS5_MIN_TS_TTA          3
#define MS5_MIN_TS_TTB          1
#define MS5_MIN_TS_TRA          3
#define MS5_MIN_TS_TRB          1

#define MULTISLOT_CLASS_6       0x05
#define MULTISLOT_CLASS_SUM_6   4
#define MS6_MAX_TS_RX           3
#define MS6_MAX_TS_TX           2
#define MS6_MIN_TS_TTA          3
#define MS6_MIN_TS_TTB          1
#define MS6_MIN_TS_TRA          3
#define MS6_MIN_TS_TRB          1

#define MULTISLOT_CLASS_7       0x06
#define MULTISLOT_CLASS_SUM_7   4
#define MS7_MAX_TS_RX           3
#define MS7_MAX_TS_TX           3
#define MS7_MIN_TS_TTA          3
#define MS7_MIN_TS_TTB          1
#define MS7_MIN_TS_TRA          3
#define MS7_MIN_TS_TRB          1

#define MULTISLOT_CLASS_8       0x07
#define MULTISLOT_CLASS_SUM_8   5
#define MS8_MAX_TS_RX           4
#define MS8_MAX_TS_TX           1
#define MS8_MIN_TS_TTA          3
#define MS8_MIN_TS_TTB          1
#define MS8_MIN_TS_TRA          2
#define MS8_MIN_TS_TRB          1

#define MULTISLOT_CLASS_9       0x08
#define MULTISLOT_CLASS_SUM_9   5
#define MS9_MAX_TS_RX           3
#define MS9_MAX_TS_TX           2
#define MS9_MIN_TS_TTA          3
#define MS9_MIN_TS_TTB          1
#define MS9_MIN_TS_TRA          2
#define MS9_MIN_TS_TRB          1

#define MULTISLOT_CLASS_10      0x09
#define MULTISLOT_CLASS_SUM_10  5
#define MS10_MAX_TS_RX          4
#define MS10_MAX_TS_TX          2
#define MS10_MIN_TS_TTA         3
#define MS10_MIN_TS_TTB         1
#define MS10_MIN_TS_TRA         2
#define MS10_MIN_TS_TRB         1

#define MULTISLOT_CLASS_11      0x0a
#define MULTISLOT_CLASS_SUM_11  5
#define MS11_MAX_TS_RX          4
#define MS11_MAX_TS_TX          3
#define MS11_MIN_TS_TTA         3
#define MS11_MIN_TS_TTB         1
#define MS11_MIN_TS_TRA         2
#define MS11_MIN_TS_TRB         1

#define MULTISLOT_CLASS_12      0x0b
#define MULTISLOT_CLASS_SUM_12  5
#define MS12_MAX_TS_RX          4
#define MS12_MAX_TS_TX          4

#define MS12_MIN_TS_TTA         2
#define MS12_MIN_TS_TTB         1
#define MS12_MIN_TS_TRA         2
#define MS12_MIN_TS_TRB         1

#ifdef FEATURE_GSM_GPRS_MSC33
#define MULTISLOT_CLASS_30      0x1d
#define MULTISLOT_CLASS_SUM_30  6
#define MS30_MAX_TS_RX          5
#define MS30_MAX_TS_TX          1
#define MS30_MIN_TS_TTA         2
#define MS30_MIN_TS_TTB         1
#define MS30_MIN_TS_TRA         1
#define MS30_MIN_TS_TRB         1

#define MULTISLOT_CLASS_31      0x1e
#define MULTISLOT_CLASS_SUM_31  6
#define MS31_MAX_TS_RX          5
#define MS31_MAX_TS_TX          2
#define MS31_MIN_TS_TTA         2
#define MS31_MIN_TS_TTB         1
#define MS31_MIN_TS_TRA         1
#define MS31_MIN_TS_TRB         1

#define MULTISLOT_CLASS_32      0x1f
#define MULTISLOT_CLASS_SUM_32  6
#define MS32_MAX_TS_RX          5
#define MS32_MAX_TS_TX          3
#define MS32_MIN_TS_TTA         2
#define MS32_MIN_TS_TTB         1
#define MS32_MIN_TS_TRA         1
#define MS32_MIN_TS_TRB         1

#define MULTISLOT_CLASS_33      0x20
#define MULTISLOT_CLASS_SUM_33  6
#define MS33_MAX_TS_RX          5
#define MS33_MAX_TS_TX          4
#define MS33_MIN_TS_TTA         2
#define MS33_MIN_TS_TTB         1
#define MS33_MIN_TS_TRA         1
#define MS33_MIN_TS_TRB         1
#endif /*FEATURE_GSM_GPRS_MCS33*/

#define GMAC_MS_DEFAULT_RX_BITMAP   0x0080
#define GMAC_MS_TS_MASK             0x0080
#define GMAC_MS_BOUNDARY_CHECK      0x81

#endif /* INC_GMAC_H */
/*** EOF: don't remove! ***/
