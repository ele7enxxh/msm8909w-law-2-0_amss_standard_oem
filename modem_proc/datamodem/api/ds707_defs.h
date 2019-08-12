#ifndef DS707_DEFS_H
#define DS707_DEFS_H
/*===========================================================================
                         D S 7 0 7 _ D E F S
GENERAL DESCRIPTION
  This file is the external macros exported by ds707 to the other layers
  in the DMSS code than need to access the macros.  Primarily the split was 
  to allow dual processor architectures to use the macros without making RPC
  function calls.

  This is specific to packet data calls. 

EXTERNALIZED FUNCTIONS
   None 

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

EXTERNALIZED MACROS
  DS_ISDATA_RS1_PRE707()
    Is the given Service option rateset 1 and considered part of the pre-707
    service option set?
    
  DS_ISDATA_RS1_IS707()
    Is the given Service option rateset 1 and considered part of the 707
    service option set?
    
  DS_ISDATA_RS2_PRE707()
    Is the given Service option rateset 2 and considered part of the pre-707
    service option set?
    
  DS_ISDATA_RS2_QC_PROPTRY()
    Is the given Service option rateset 2 and considered part of the Qualcomm
    proprietary service option set?
    
  DS_ISDATA_RS1()
    Is the given Service option rateset 1?
    
  DS_ISDATA_RS2()
    Is the given Service option rateset 2?
    
  DS_ISDATA()
    Is the given Service option a data service option?
  
  DS_ISDATA_RLP_TYPE2()
    Is the service option an MDR service option?
    
  DS_ISDATA_RLP_TYPE3()
    Is the service option a 1x service option (SO 33)?
    
EXTERNALIZED MACROS FOR CASE STATEMENTS
  DS_CASE_DATA_RS1_PRE707
    Case statements for rateset 1 service options which are in the pre-707
    service option set.
    
  DS_CASE_DATA_RS1_IS707
    Case statements for rateset 1 service options which are in the 707
    service option set.
    
  DS_CASE_DATA_RS2_PRE707
    Case statements for rateset 2 service options which are in the pre-707
    service option set.
    
  DS_CASE_DATA_RS2_QC_PROPTRY
    Case statements for rateset 2 service options which are in the
    proprietary service option set.

  DS_CASE_DATA_RS1
    Case statements for rateset 1 service options
  
  DS_CASE_DATA_RS2
    Case statements for rateset 1 service options
    
  DS_CASE_DATA
    Case statements for all data service options

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

 Copyright (c) 2003-2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ds707_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
10/28/09    vs        Removed the inclusion of customer.h
09/16/03   as/kvd     First version of file. macros moved from 
                      //depot/asic/msmshared/mmdata/1x/707/ds707_extif.h#5
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "cai.h"

/*===========================================================================
                        EXTERNAL MACROS
===========================================================================*/
/*---------------------------------------------------------------------------
  Is it a rateset 1 (Pre-707) data s.o.? 
---------------------------------------------------------------------------*/
#define DS_ISDATA_RS1_PRE707(so) \
  (((so) == CAI_SO_PPP_PKT_DATA_PRE707) || \
   ((so) == CAI_SO_ASYNC_DATA_PRE707) || \
   ((so) == CAI_SO_G3_FAX_PRE707))

#define DS_CASE_DATA_RS1_PRE707 \
  case CAI_SO_PPP_PKT_DATA_PRE707: \
  case CAI_SO_ASYNC_DATA_PRE707: \
  case CAI_SO_G3_FAX_PRE707:

/*---------------------------------------------------------------------------
  Is it a rateset 1 (IS-707) data s.o.? 
---------------------------------------------------------------------------*/
#define DS_ISDATA_RS1_IS707(so) \
  (((so) == CAI_SO_PPP_PKT_DATA_IS707) || \
   ((so) == CAI_SO_ASYNC_DATA_IS707) || \
   ((so) == CAI_SO_G3_FAX_IS707))

#define DS_CASE_DATA_RS1_IS707 \
  case CAI_SO_PPP_PKT_DATA_IS707: \
  case CAI_SO_ASYNC_DATA_IS707: \
  case CAI_SO_G3_FAX_IS707:

/*---------------------------------------------------------------------------
  Is it a rateset 2 (Pre-707) data s.o.? 
---------------------------------------------------------------------------*/
#define DS_ISDATA_RS2_PRE707(so) \
  (((so) == CAI_SO_PPP_PKT_DATA_13K_PRE707) || \
   ((so) == CAI_SO_ASYNC_DATA_13K_PRE707) || \
   ((so) == CAI_SO_G3_FAX_13K_PRE707))

#define DS_CASE_DATA_RS2_PRE707 \
  case CAI_SO_PPP_PKT_DATA_13K_PRE707: \
  case CAI_SO_ASYNC_DATA_13K_PRE707: \
  case CAI_SO_G3_FAX_13K_PRE707:

/*---------------------------------------------------------------------------
  Is it a rateset 2 (QC proprietary) data s.o.?
---------------------------------------------------------------------------*/
#define DS_ISDATA_RS2_QC_PROPTRY(so) \
  (((so) == CAI_SO_PPP_PKT_DATA_13K_PROPTRY) || \
   ((so) == CAI_SO_ASYNC_DATA_13K_PROPTRY) || \
   ((so) == CAI_SO_G3_FAX_13K_PROPTRY))

#define DS_CASE_DATA_RS2_QC_PROPTRY \
  case CAI_SO_PPP_PKT_DATA_13K_PROPTRY: \
  case CAI_SO_ASYNC_DATA_13K_PROPTRY: \
  case CAI_SO_G3_FAX_13K_PROPTRY:

/*---------------------------------------------------------------------------
  Is it a rateset 1 data s.o.?
---------------------------------------------------------------------------*/
#define DS_ISDATA_RS1(so) \
  ( \
   (CAI_SO_PPP_PKT_DATA_3G == (so)) || \
    (CAI_SO_DATA == (so)) || \
    DS_ISDATA_RS1_PRE707(so) || \
    DS_ISDATA_RS1_IS707(so) || \
    (CAI_SO_MDR_PKT_DATA_FRS1_RRS1 == (so)) \
  )

#define DS_CASE_DATA_RS1 \
     case CAI_SO_PPP_PKT_DATA_3G: \
     case CAI_SO_DATA: \
     DS_CASE_DATA_RS1_PRE707 \
     DS_CASE_DATA_RS1_IS707 \
     case CAI_SO_MDR_PKT_DATA_FRS1_RRS1:

/*---------------------------------------------------------------------------
  Is it a rateset 2 data s.o.?
---------------------------------------------------------------------------*/
#define DS_ISDATA_RS2(so) \
  ( \
    DS_ISDATA_RS2_PRE707(so) || \
    DS_ISDATA_RS2_QC_PROPTRY(so) || \
    (CAI_SO_MDR_PKT_DATA_FRS2_RRS2 == (so)) \
  )

#define DS_CASE_DATA_RS2 \
     DS_CASE_DATA_RS2_PRE707 \
     DS_CASE_DATA_RS2_QC_PROPTRY \
     case CAI_SO_MDR_PKT_DATA_FRS2_RRS2:

/*---------------------------------------------------------------------------
  Is it a data s.o.?
---------------------------------------------------------------------------*/
#define DS_ISDATA(so) \
  (DS_ISDATA_RS1(so) || DS_ISDATA_RS2(so))

#define DS_CASE_DATA \
  DS_CASE_DATA_RS1 \
  DS_CASE_DATA_RS2

#define DS_ISDATA_RLP_TYPE2(so) \
     ( (CAI_SO_MDR_PKT_DATA_FRS1_RRS1 == (so)) || \
       (CAI_SO_MDR_PKT_DATA_FRS2_RRS2 == (so)) )

#define DS_ISDATA_RLP_TYPE3(so) \
     ( (CAI_SO_PPP_PKT_DATA_3G == (so)) )

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

#endif /* DS707_DEFS_H        */
