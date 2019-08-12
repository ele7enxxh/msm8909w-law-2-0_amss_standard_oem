#ifndef DIAGDSM_V_H
#define DIAGDSM_V_H
/*===========================================================================

                                  D I A G D S M _ V . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2005-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagdsm_v.h#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/30/13    sr     Removed SIO_RX pool. Cmd req/rsp will now use CMD_SIO_TX 
                   and CMD_SIO_RX pool even in backwards compatibility case  
01/17/13    rh     Support for buffered threshold mode
01/03/13    sg      Command Response Channel separation
06/03/12    sa     Added Control Channel Size and Counts
11/27/12    rh     Removed DIAG_MP_SLAVE_LAST
01/18/10    JV     Do not initilaize the 2 SMD pools for single processor targets.
09/23/09    JV     Do not initialize the SMD_RX and SMD_TX pools on the slave
                   last.
07/06/09    JV     Created private header file.
===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#include "diagdsm.h"
#include "dsm_pool.h"
#include "diag_v.h"

/*---------------------------------------------------------------------------
  Definitions for few, many and do not exceed counts used in buffer based
  flow control.  Dont_Exceed is based on the minimum number of buffers
  required by the system. All incoming data from Rm or Um is dropped at this
  point.  Few Mark is when the system starts flow controlling the Um and Rm
  interfaces to stop new data from coming into the system.  Many Mark is when
  the system reallows new data to come into the system from the Um or the Rm
  interface.  These are empirical numbers based on various test scenarios.
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
  Size, Count and count for different memory marks/levels for LARGE items. 
  The significants of the counts DONT_EXCEED, TRANSPORT_FEW, FEW and MANY
  is same as described above for small items.

  Sizing for Large Memory Pool items. Header size defines enough space for
  worst case TCP/IP/PPP header.  This should be:

    Default Max MSS + TCP Header Size + MAX TCP Options Size + IP Header Size
    + MAX IP Options Size + MAX PPP Header Size + PPP Checksum Size.

  The large item count needs to be increased if maximum-size SSL records
  are processed.
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items 
---------------------------------------------------------------------------*/

#define DSM_DIAG_SIO_TX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_SIO_TX_ITEM_POOL)) 
#define DSM_DIAG_SIO_TX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_SIO_TX_ITEM_POOL))

#if !defined(DIAG_MP_MASTER)
//SIO RX Control Channel Size and Count
#define DSM_DIAG_SIO_CTRL_RX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_SIO_CTRL_RX_ITEM_POOL)) 
#define DSM_DIAG_SIO_CTRL_RX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_SIO_CTRL_RX_ITEM_POOL))

//SIO TX Control Channel Size and Count
#define DSM_DIAG_SIO_CTRL_TX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_SIO_CTRL_TX_ITEM_POOL)) 
#define DSM_DIAG_SIO_CTRL_TX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_SIO_CTRL_TX_ITEM_POOL))

#endif


#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items 
---------------------------------------------------------------------------*/

#define DSM_DIAG_SMD_RX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_SMD_RX_ITEM_POOL)) 
#define DSM_DIAG_SMD_RX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_SMD_RX_ITEM_POOL))

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items 
---------------------------------------------------------------------------*/

#define DSM_DIAG_SMD_TX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_SMD_TX_ITEM_POOL)) 
#define DSM_DIAG_SMD_TX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_SMD_TX_ITEM_POOL))

#define DSM_DIAG_SMD_TX_USED_CNT()                                                 \
    (DSM_DIAG_SMD_TX_ITEM_CNT - DSM_POOL_FREE_CNT(DSM_DIAG_SMD_TX_ITEM_POOL))	

#define DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_SMD_CTRL_RX_ITEM_POOL)) 
#define DSM_DIAG_SMD_CTRL_RX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_SMD_CTRL_RX_ITEM_POOL))

#define DSM_DIAG_SMD_CTRL_TX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_SMD_CTRL_TX_ITEM_POOL)) 
#define DSM_DIAG_SMD_CTRL_TX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_SMD_CTRL_TX_ITEM_POOL))
#endif

#define DSM_DIAG_CMD_SIO_RX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_CMD_SIO_RX_ITEM_POOL)) 
#define DSM_DIAG_CMD_SIO_RX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_CMD_SIO_RX_ITEM_POOL))
	
#define DSM_DIAG_CMD_SIO_TX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_CMD_SIO_TX_ITEM_POOL)) 
#define DSM_DIAG_CMD_SIO_TX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_CMD_SIO_TX_ITEM_POOL))

#define DSM_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_CMD_SIO_DCI_RX_ITEM_POOL)) 
#define DSM_DIAG_CMD_SIO_DCI_RX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_CMD_SIO_DCI_RX_ITEM_POOL))
	
#define DSM_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_CMD_SIO_DCI_TX_ITEM_POOL)) 
#define DSM_DIAG_CMD_SIO_DCI_TX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_CMD_SIO_DCI_TX_ITEM_POOL))
#define DSM_DIAG_CMD_SMD_RX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_CMD_SMD_RX_ITEM_POOL)) 
#define DSM_DIAG_CMD_SMD_RX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_CMD_SMD_RX_ITEM_POOL))
	
#define DSM_DIAG_CMD_SMD_TX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_CMD_SMD_TX_ITEM_POOL)) 
#define DSM_DIAG_CMD_SMD_TX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_CMD_SMD_TX_ITEM_POOL))

#define DSM_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_CMD_SMD_DCI_RX_ITEM_POOL)) 
#define DSM_DIAG_CMD_SMD_DCI_RX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_CMD_SMD_DCI_RX_ITEM_POOL))
	
#define DSM_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ                                                   \
    (DSM_POOL_ITEM_SIZE(DSM_DIAG_CMD_SMD_DCI_TX_ITEM_POOL)) 
#define DSM_DIAG_CMD_SMD_DCI_TX_ITEM_CNT                                                   \
    (DSM_POOL_ITEM_CNT(DSM_DIAG_CMD_SMD_DCI_TX_ITEM_POOL))


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/
/*================ MEMORY MANAGMENT FUNCTION ==============================*/
/*===========================================================================
FUNCTION DIAGDSM_INIT()

DESCRIPTION
  This function will initialize the Data Service Memory Pool. It should be
  called once upon system startup. All the memory items are initialized and
  put onto their respective free queues.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Whether memory allocation was successful

SIDE EFFECTS
  None
===========================================================================*/
extern boolean diagdsm_init
( 
  void 
);

#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */

#endif /* DIAGDSM_V_H */


