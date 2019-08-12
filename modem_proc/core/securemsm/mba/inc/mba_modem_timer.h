#ifndef MBA_MODEM_TIMER_H
#define MBA_MODEM_TIMER_H
 /**===========================================================================
 ** FILE 
 **     mba_modem_timer.h
 ** 
 ** GENERAL DESCRIPTION 
 **     This file provides the APIs to use QTimers within
 **     the Modem subsystem. All QTimer functionality is based
 **     on a global system counter which runs at 19.2 Mhz.                                        
 ** 
 **==========================================================================*/
#include "comdef.h"

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/inc/mba_modem_timer.h#1 $

  when       who     what, where, why
  --------   ---     ------------------------------------------------------------
  04/11/12   mm      Adapted to Aragorn v1
  03/01/12   as      Initial Revision for MSM8974
 
=============================================================================*/


/*=============================================================================
                                MODULE INCLUDES
                          ADD NEW ONES UNDER THIS LINE     
=============================================================================*/


/*=============================================================================
                                PUBLIC DEFINES
                          ADD NEW ONES UNDER THIS LINE     
=============================================================================*/
#define MBA_WAIT_1_MICROSECOND                                    \
    do {                                                          \
	  /* Set QTimer match value in cycles */                      \
	  HWIO_OUT(QDSP6SS_QTMR_V1_CNTP_TVAL_0, 19);                  \
	  /* Start QTimer */                                          \
	  HWIO_OUTF(QDSP6SS_QTMR_V1_CNTP_CTL_0, EN, 0x1);             \
	  /* Loop until the interrupt status bit goes high */         \
	  while(HWIO_INF(QDSP6SS_QTMR_V1_CNTP_CTL_0,ISTAT) == 0x0);   \
	  /* Disable QTimer upon finishing. */                        \
	  HWIO_OUTF(QDSP6SS_QTMR_V1_CNTP_CTL_0,EN, 0x0);              \
      } while(0)

#define MBA_WAIT_2_MICROSECONDS                                    \
	do {															\
	  /* Set QTimer match value in cycles */						\
	  HWIO_OUT(QDSP6SS_QTMR_V1_CNTP_TVAL_0, 38);					\
	  /* Start QTimer */											\
	  HWIO_OUTF(QDSP6SS_QTMR_V1_CNTP_CTL_0, EN, 0x1); 			\
	  /* Loop until the interrupt status bit goes high */ 		\
	  while(HWIO_INF(QDSP6SS_QTMR_V1_CNTP_CTL_0,ISTAT) == 0x0);	\
	  /* Disable QTimer upon finishing. */						\
	  HWIO_OUTF(QDSP6SS_QTMR_V1_CNTP_CTL_0,EN, 0x0);				\
	  } while(0)

#define MBA_WAIT_5_MICROSECONDS                                    \
	do {														  \
	  /* Set QTimer match value in cycles */					  \
	  HWIO_OUT(QDSP6SS_QTMR_V1_CNTP_TVAL_0, 96);				  \
	  /* Start QTimer */										  \
	  HWIO_OUTF(QDSP6SS_QTMR_V1_CNTP_CTL_0, EN, 0x1);			  \
	  /* Loop until the interrupt status bit goes high */		  \
	  while(HWIO_INF(QDSP6SS_QTMR_V1_CNTP_CTL_0,ISTAT) == 0x0);   \
	  /* Disable QTimer upon finishing. */						  \
	  HWIO_OUTF(QDSP6SS_QTMR_V1_CNTP_CTL_0,EN, 0x0);			  \
	  } while(0)

/*=============================================================================
                            PUBLIC DATA DECLARATIONS
                          ADD NEW ONES UNDER THIS LINE     
=============================================================================*/


/*=============================================================================
                          PUBLIC FUNCTION DECLARATIONS
                          ADD NEW ONES UNDER THIS LINE    
=============================================================================*/
 /**===========================================================================
 **
 ** FUNCTION  mba_MODEM_WAIT_MICROSECONDS
 **
 ** DESCRIPTION
 **     This function waits for microseconds. This timer
 **     driver is based off of the Modem QTimer which
 **     references a global system timer which runs at 19.2 MHz. 
 ** 
 ** DEPENDENCIES
 **     mba_init_tmr() must be called before calling this function.
 ** 
 ** PARAMETERS
 **     Type : uint32
 **     Param: microseconds
 **            This parameter specifies the number of
 **            microseconds we want to enable the wait timer for.
 ** 
 ** RETURN VALUE
 **     Type : void
 **     Value: None
 **
 ** SIDE EFFECTS
 **     None.
 **
 **==========================================================================*/
void mba_modem_wait_microseconds(uint32 microseconds);


 /**===========================================================================
 **
 ** FUNCTION  mba_MODEM_INIT_TMR
 **
 ** DESCRIPTION
 **     This function preforms any initializations required
 **     to use the QTimer. 
 ** 
 ** DEPENDENCIES
 **     None.
 ** 
 ** PARAMETERS
 **     Type : void
 **     Param: None
 ** 
 ** RETURN VALUE
 **     Type : void
 **     Value: None
 **
 ** SIDE EFFECTS
 **     None.
 **
 **==========================================================================*/
void mba_modem_init_tmr(void);


 /**===========================================================================
 **
 ** FUNCTION  mba_MODEM_TMR_START_TIMEOUT
 **
 ** DESCRIPTION
 **     This function initializes and starts the mba timeout   
 **         1. The QTimer is forced off                        
 **         2. Timer expiration value is set in TVAL           
 **         3. The QTimer is turned on                         
 ** 
 **         Maximum allowed expiration time is ~ 223 seconds
 ** 
 ** DEPENDENCIES
 **     mba_init_tmr() must be called before calling this function.
 ** 
 ** PARAMETERS
 **     Type : uint32
 **     Param: milliseconds
 **            This parameter specifies the time in milliseconds
 **            for which the timeout is desired.
 ** 
 ** RETURN VALUE
 **     Type : void
 **     Value: None
 **
 ** SIDE EFFECTS
 **     None.
 **
 **==========================================================================*/
void mba_modem_tmr_start_timeout(uint32 milliseconds);


 /**===========================================================================
 **
 ** FUNCTION  mba_MODEM_TMR_IS_EXPIRED
 **
 ** DESCRIPTION
 **     This function checks if the mba QTimer has expired
 **                                                                                      
 **     This function will poll the ISTAT bit of control
 **     register to determine if the timer has expired.
 ** 
 ** DEPENDENCIES
 **     Must be called after mba_tmr_start_timeout().
 ** 
 ** PARAMETERS
 **     Type : void
 **     Param: None.
 ** 
 ** RETURN VALUE
 **     Type : boolean
 **     Value: Timer expire status.
 **            TRUE if the timer has expired, FALSE otherwise.
 **
 ** SIDE EFFECTS
 **     None.
 **
 **==========================================================================*/
boolean mba_modem_tmr_is_expired(void);

#endif /* mba_MODEM_TIMER_H */
/*=============================================================================
                                  END OF FILE
=============================================================================*/

