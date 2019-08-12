#ifndef DIAGDSMI_H
#define DIAGDSMI_H
/*===========================================================================

                                  D I A G D S M I . H

  GENERAL DESCRIPTION
  Declares sizes for diag dsm items.

Copyright (c) 2009-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/platform/bmp/src/diagdsmi.h#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/01/14    rs     Added support for build mask centralization. 
08/31/13    is     Support for peripheral buffering mode
08/02/13    rh     Added flow control limits for control channel 
07/23/13    rh     Increased extra DSM padding on TX pool to guarantee we 
                   have enough items to handle our larger max packet size 
04/06/13    sg     Reduce the dsm pool count for sio rx,tx item pools
03/19/13    rh     Actually reduce the size of the SIO control DSM pool 
01/03/13    sg     Command Req/Response Channel separation
06/03/12    sa     Added DSM Control Items Size and Number
04/23/10    JV     For the SIO_TX pool, disable flow if we have only 3 items free
                   and enable it back on if we have 10 items free.
10/06/09    JV     Mainlined FEATURE_DSM_DIAG_ITEMS.
04/15/09    mad    Reduced SIO_TX count on slave processors
04/03/09    mad    Created from diagdsm.c
===========================================================================*/

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items SIO RX
  (Receive diag pkts from USB on master. 
  Receive diag pkts forwarded via SMD on slave.)
---------------------------------------------------------------------------*/
#if !defined DSMI_DIAG_SIO_RX_ITEM_SIZ
#define DSMI_DIAG_SIO_RX_ITEM_SIZ                 2048 
#endif

#if !defined DSMI_DIAG_SIO_RX_ITEM_CNT
#define DSMI_DIAG_SIO_RX_ITEM_CNT                 25
#endif

#define DSMI_DIAG_SIO_RX_ITEM_DONT_EXCEED         3 /* we have 3 re-tries for a packet */
#define DSMI_DIAG_SIO_RX_ITEM_FEW_MARK            ((DSMI_DIAG_SIO_RX_ITEM_CNT)/10)
#define DSMI_DIAG_SIO_RX_ITEM_MANY_MARK           ((DSMI_DIAG_SIO_RX_ITEM_CNT) - 10)

/*---------------------------------------------------------------------------
  Size, Count Number for DIAG items CTRL TX/RX
---------------------------------------------------------------------------*/
/* the DSM Items size and number for the control Channel */
//TODO: Analysis to get a optimal size for the items and the count


#if !defined DSMI_DIAG_SIO_CTRL_RX_ITEM_SIZ
#define DSMI_DIAG_SIO_CTRL_RX_ITEM_SIZ                 512
#endif
  
#if !defined DSMI_DIAG_SIO_CTRL_RX_ITEM_CNT
#define DSMI_DIAG_SIO_CTRL_RX_ITEM_CNT                 50
#endif

#if !defined DSMI_DIAG_SIO_CTRL_TX_ITEM_SIZ
#define DSMI_DIAG_SIO_CTRL_TX_ITEM_SIZ                 512
#endif
  
#if !defined DSMI_DIAG_SIO_CTRL_TX_ITEM_CNT
#define DSMI_DIAG_SIO_CTRL_TX_ITEM_CNT                 50
#endif

#define DSMI_DIAG_SIO_CTRL_RX_ITEM_DONT_EXCEED         10
#define DSMI_DIAG_SIO_CTRL_RX_ITEM_FEW_MARK            ((DSMI_DIAG_SIO_CTRL_RX_ITEM_CNT) - 10) /* This many used to start sending again */
#define DSMI_DIAG_SIO_CTRL_RX_ITEM_MANY_MARK           ((DSMI_DIAG_SIO_CTRL_RX_ITEM_CNT) - 5)  /* This many used when we stop sending */
  
#define DSMI_DIAG_SIO_CTRL_TX_ITEM_DONT_EXCEED         10
#define DSMI_DIAG_SIO_CTRL_TX_ITEM_FEW_MARK            ((DSMI_DIAG_SIO_CTRL_TX_ITEM_CNT) - 10) /* This many used to start sending again */
#define DSMI_DIAG_SIO_CTRL_TX_ITEM_MANY_MARK           ((DSMI_DIAG_SIO_CTRL_TX_ITEM_CNT) - 5)  /* This many used when we stop sending */

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items SIO TX
  (Transmit to USB on master
  Transmit via SMD on slave)
---------------------------------------------------------------------------*/
#if !defined DSMI_DIAG_SIO_TX_ITEM_SIZ
#define DSMI_DIAG_SIO_TX_ITEM_SIZ                 4096
#endif

#if !defined DSMI_DIAG_SIO_TX_ITEM_CNT
#if defined DIAG_MP_MASTER
#define DSMI_DIAG_SIO_TX_ITEM_CNT                 50
#else
#define DSMI_DIAG_SIO_TX_ITEM_CNT                 25 /* Making the count less on slave processor, since master should have enough DSM items to handle sum of all slave traffic */
#endif /* defined DIAG_MP_MASTER */
#endif
  
#define DSMI_DIAG_SIO_TX_ITEM_DONT_EXCEED                  5
#define DSMI_DIAG_SIO_TX_ITEM_FEW_MARK                     ((DSMI_DIAG_SIO_TX_ITEM_CNT) - 10)
#define DSMI_DIAG_SIO_TX_ITEM_MANY_MARK                    ((DSMI_DIAG_SIO_TX_ITEM_CNT) - 3) 

  /* Number of items we want consumed before triggering flow control */
  #define DSMI_DIAG_SIO_TX_ITEM_BUFFERED_MANY_MARK           ((DSMI_DIAG_SIO_TX_ITEM_CNT) - 8)
  /* Number of items we want outstanding before restoring flow */
  #define DSMI_DIAG_SIO_TX_ITEM_BUFFERED_FEW_MARK            ((DSMI_DIAG_SIO_TX_ITEM_BUFFERED_MANY_MARK) - 7)
  
#if !defined DSMI_DIAG_SIO_TX_ITEM_BUFFERED_MANY_DRAIN_MARK
  #define DSMI_DIAG_SIO_TX_ITEM_BUFFERED_MANY_DRAIN_MARK     ((DSMI_DIAG_SIO_TX_ITEM_CNT) - 15)
#endif

#if !defined DSMI_DIAG_SIO_TX_ITEM_BUFFERED_FEW_DRAIN_MARK
  #define DSMI_DIAG_SIO_TX_ITEM_BUFFERED_FEW_DRAIN_MARK      (5)
#endif

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items SMD RX
  (Forward incoming diag packet to slave, on master)
  (Not used, in slave)
---------------------------------------------------------------------------*/
#if !defined DSMI_DIAG_SMD_RX_ITEM_SIZ
#define DSMI_DIAG_SMD_RX_ITEM_SIZ                 (DSMI_DIAG_SIO_RX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_SMD_RX_ITEM_CNT
#define DSMI_DIAG_SMD_RX_ITEM_CNT                 (DSMI_DIAG_SIO_RX_ITEM_CNT)
#endif
  
#define DSMI_DIAG_SMD_RX_ITEM_DONT_EXCEED         (DSMI_DIAG_SIO_RX_ITEM_DONT_EXCEED)
#define DSMI_DIAG_SMD_RX_ITEM_FEW_MARK            (DSMI_DIAG_SIO_RX_ITEM_FEW_MARK)
#define DSMI_DIAG_SMD_RX_ITEM_MANY_MARK           (DSMI_DIAG_SIO_RX_ITEM_MANY_MARK) 

/*---------------------------------------------------------------------------
  Size, Number counts for DIAG items CTRL TX/RX
  (control diag packet to/from slaves)
---------------------------------------------------------------------------*/


#if !defined DSMI_DIAG_SMD_CTRL_RX_ITEM_SIZ
#define DSMI_DIAG_SMD_CTRL_RX_ITEM_SIZ                 512
#endif
	
#if !defined DSMI_DIAG_SMD_CTRL_RX_ITEM_CNT
#define DSMI_DIAG_SMD_CTRL_RX_ITEM_CNT                 100
#endif

#if !defined DSMI_DIAG_SMD_CTRL_TX_ITEM_SIZ
#define DSMI_DIAG_SMD_CTRL_TX_ITEM_SIZ                 512
#endif
	
#if !defined DSMI_DIAG_SMD_CTRL_TX_ITEM_CNT
#define DSMI_DIAG_SMD_CTRL_TX_ITEM_CNT                 100
#endif

		

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items SMD TX
  (Receive diag pkts from slave and Transmit to USB, in master
  Not used, in slave)
---------------------------------------------------------------------------*/
#if !defined DSMI_DIAG_SMD_TX_ITEM_SIZ
#define DSMI_DIAG_SMD_TX_ITEM_SIZ                 (DSMI_DIAG_SIO_TX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_SMD_TX_ITEM_CNT
#define DSMI_DIAG_SMD_TX_ITEM_CNT                 (DSMI_DIAG_SIO_TX_ITEM_CNT)
#endif
  
  #define DSMI_DIAG_SMD_TX_ITEM_DONT_EXCEED         (DSMI_DIAG_SIO_TX_ITEM_DONT_EXCEED)
  #define DSMI_DIAG_SMD_TX_ITEM_FEW_MARK            (DSMI_DIAG_SIO_TX_ITEM_FEW_MARK)
  #define DSMI_DIAG_SMD_TX_ITEM_MANY_MARK           (DSMI_DIAG_SIO_TX_ITEM_MANY_MARK)


/* Command/Response Path */
#ifdef DIAG_MP_MASTER
#if !defined DSMI_DIAG_CMD_SMD_RX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SMD_RX_ITEM_SIZ               (DSMI_DIAG_SIO_RX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_CMD_SMD_RX_ITEM_CNT
  #define DSMI_DIAG_CMD_SMD_RX_ITEM_CNT               (DSMI_DIAG_SIO_RX_ITEM_CNT)
#endif

  #define DSMI_DIAG_CMD_SMD_RX_ITEM_DONT_EXCEED       (DSMI_DIAG_SIO_RX_ITEM_CNT)
  #define DSMI_DIAG_CMD_SMD_RX_ITEM_FEW_MARK          (DSMI_DIAG_SIO_RX_ITEM_FEW_MARK)
  #define DSMI_DIAG_CMD_SMD_RX_ITEM_MANY_MARK         (DSMI_DIAG_SIO_RX_ITEM_MANY_MARK)
  
#if !defined DSMI_DIAG_CMD_SMD_TX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SMD_TX_ITEM_SIZ               2048 
#endif

#if !defined DSMI_DIAG_CMD_SMD_TX_ITEM_CNT
  #define DSMI_DIAG_CMD_SMD_TX_ITEM_CNT               25
#endif

  #define DSMI_DIAG_CMD_SMD_TX_ITEM_DONT_EXCEED       5
  #define DSMI_DIAG_CMD_SMD_TX_ITEM_FEW_MARK          ((DSMI_DIAG_SIO_CMD_TX_ITEM_CNT)/10)
  #define DSMI_DIAG_CMD_SMD_TX_ITEM_MANY_MARK         ((DSMI_DIAG_SIO_CMD_TX_ITEM_CNT) - 10)

#if !defined DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ               (DSMI_DIAG_SIO_RX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_CNT
  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_CNT               (DSMI_DIAG_SIO_RX_ITEM_CNT)
#endif

  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_DONT_EXCEED       (DSMI_DIAG_SIO_RX_ITEM_CNT)
  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_FEW_MARK          (DSMI_DIAG_SIO_RX_ITEM_FEW_MARK)
  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_MANY_MARK         (DSMI_DIAG_SIO_RX_ITEM_MANY_MARK)
  
#if !defined DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ               2048 
#endif

#if !defined DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_CNT
  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_CNT               25
#endif

  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_DONT_EXCEED       5
  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_FEW_MARK          ((DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_CNT)/10)
  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_MANY_MARK         ((DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_CNT) - 10)
  
#else
#if !defined DSMI_DIAG_CMD_SIO_RX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SIO_RX_ITEM_SIZ               (DSMI_DIAG_SIO_RX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_CMD_SIO_RX_ITEM_CNT
  #define DSMI_DIAG_CMD_SIO_RX_ITEM_CNT               10
#endif

  #define DSMI_DIAG_CMD_SIO_RX_ITEM_DONT_EXCEED       (DSMI_DIAG_SIO_RX_ITEM_CNT)
  #define DSMI_DIAG_CMD_SIO_RX_ITEM_FEW_MARK          (DSMI_DIAG_SIO_RX_ITEM_FEW_MARK)
  #define DSMI_DIAG_CMD_SIO_RX_ITEM_MANY_MARK         (DSMI_DIAG_SIO_RX_ITEM_MANY_MARK)
  
#if !defined DSMI_DIAG_CMD_SIO_TX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SIO_TX_ITEM_SIZ               2048 
#endif

#if !defined DSMI_DIAG_CMD_SIO_TX_ITEM_CNT
  #define DSMI_DIAG_CMD_SIO_TX_ITEM_CNT               10
#endif

  #define DSMI_DIAG_CMD_SIO_TX_ITEM_DONT_EXCEED       5
  #define DSMI_DIAG_CMD_SIO_TX_ITEM_FEW_MARK          ((DSMI_DIAG_SIO_CMD_TX_ITEM_CNT)/10)
  #define DSMI_DIAG_CMD_SIO_TX_ITEM_MANY_MARK         ((DSMI_DIAG_SIO_CMD_TX_ITEM_CNT) - 10)

#if !defined DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ               (DSMI_DIAG_SIO_RX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_CNT
  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_CNT               10
#endif

  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_DONT_EXCEED       (DSMI_DIAG_SIO_RX_ITEM_CNT)
  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_FEW_MARK          (DSMI_DIAG_SIO_RX_ITEM_FEW_MARK)
  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_MANY_MARK         (DSMI_DIAG_SIO_RX_ITEM_MANY_MARK)
  
#if !defined DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ               2048 
#endif

#if !defined DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT
  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT               10
#endif

  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_DONT_EXCEED       5
  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_FEW_MARK          ((DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT)/10)
  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_MANY_MARK         ((DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT) - 10)
#endif

/* Buffering Mode parameters */
#if defined(DIAG_MP_MASTER)
  #define DSMI_DIAG_BUFFERED_MANY_FLOW_MARK               DSMI_DIAG_SMD_TX_ITEM_BUFFERED_MANY_MARK
  #define DSMI_DIAG_BUFFERED_FEW_FLOW_MARK                DSMI_DIAG_SMD_TX_ITEM_BUFFERED_FEW_MARK
  #define DSMI_DIAG_BUFFERED_MANY_DRAIN_MARK              DSMI_DIAG_SMD_TX_ITEM_BUFFERED_MANY_DRAIN_MARK
  #define DSMI_DIAG_BUFFERED_FEW_DRAIN_MARK               DSMI_DIAG_SMD_TX_ITEM_BUFFERED_FEW_DRAIN_MARK
  
  #define DIAG_BUFFERED_DSM_CNT_TO_BYTES( cnt )			  (cnt * DSMI_DIAG_SMD_TX_ITEM_SIZ)
  #define DIAG_DEFAULT_BUFFERED_DSM_TOT_COUNT             (DSMI_DIAG_SMD_TX_ITEM_CNT)
  #define DIAG_DEFAULT_BUFFERED_DSM_ITEM_SIZE              DSMI_DIAG_SMD_TX_ITEM_SIZ
#else
  /* For flow control (Count of DSMs used) */
  #define DSMI_DIAG_BUFFERED_MANY_FLOW_MARK               DSMI_DIAG_SIO_TX_ITEM_BUFFERED_MANY_MARK
  #define DSMI_DIAG_BUFFERED_FEW_FLOW_MARK                DSMI_DIAG_SIO_TX_ITEM_BUFFERED_FEW_MARK
  /* For buffering mode draining (Count of DSMs used) */
  #define DSMI_DIAG_BUFFERED_MANY_DRAIN_MARK              DSMI_DIAG_SIO_TX_ITEM_BUFFERED_MANY_DRAIN_MARK
  #define DSMI_DIAG_BUFFERED_FEW_DRAIN_MARK               DSMI_DIAG_SIO_TX_ITEM_BUFFERED_FEW_DRAIN_MARK
  
  #define DIAG_BUFFERED_DSM_CNT_TO_BYTES( cnt )			  (cnt * DSMI_DIAG_SIO_TX_ITEM_SIZ)
  #define DIAG_DEFAULT_BUFFERED_DSM_TOT_COUNT             (DSMI_DIAG_SIO_TX_ITEM_CNT)
  #define DIAG_DEFAULT_BUFFERED_DSM_ITEM_SIZE              DSMI_DIAG_SIO_TX_ITEM_SIZ
  
  #define DIAG_BUFFERED_MANY_FLOW_PER_MARK       (90) /* % of buffer full when we disable flow */
  #define DIAG_BUFFERED_FEW_FLOW_PER_MARK        (70) /* % of buffer full when we resume flow */
  #define DIAG_BUFFERED_MANY_DRAIN_PER_MARK      (80) /* % of buffer full when we start to drain or wrap */
  #define DIAG_BUFFERED_FEW_DRAIN_PER_MARK       (15) /* % of buffer full when we stop draining */
#endif

#endif /* DIAGDSMI_H */

