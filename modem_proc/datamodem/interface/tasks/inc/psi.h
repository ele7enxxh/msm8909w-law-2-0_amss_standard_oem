#ifndef PSI_H
#define PSI_H
/*===========================================================================

                           P S I . H

DESCRIPTION

  Protocol Task local header file. All definitions, structures, and functions
  needed internal to protocol task services.
  
EXTERNALIZED FUNCTIONS    
  
Copyright (c) 1994-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/psi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/11   hm      Use ps_task_crit_section for all PS_task operations. 
01/10/11   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
09/27/10   ss      DYNAMIC WDOG support for DATA tasks.
08/11/10   ss      Rearchitectured PS extension signal mechanism. Any number
                   of signals supported now.
06/15/10   sy      Moved cmd types to psi_cmd.h file
09/03/09   pp      Moved psi_wait APIs to ps_svc.h.
03/26/09   pp      CMI De-featurization.
12/14/08   pp      Common Modem Interface: Public/Private API split.
09/07/07   scb     Added REX signal ext code under FEATURE_REX_SIGS_EXT
04/25/06   mct     Removed tick timer code from old pstimer implementation.
01/23/06   rt      Increased the PSI_CMD_BUF_CNT to 100.
08/05/02   usb     Added new function prototype psi_wait()
07/15/02   aku     Include dssocket.h for sockets build.
10/24/94   jjw     Initial version of file


===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "ps_svc.h"
#include "rex.h"
#include "psi_cmd.h"
#include "ps_crit_sect.h"


/*===========================================================================

                           MACROS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  Dog data structures Extern defintion.
---------------------------------------------------------------------------*/
extern uint32 ps_dog_rpt_id;                         /*! dog report ID     */

/*--------------------------------------------------------------------------
  Defines the PS task critical section.
---------------------------------------------------------------------------*/
extern ps_crit_sect_type ps_task_crit_section;

    /*--------------------------------------------------------------------------
      The SIG_IDX macro provides the index of the array for the signal sig_num.
      The SIG_OFFSET macro provides the offset for the signal sig_num within 
      the signal dword.
    ---------------------------------------------------------------------------*/
#define SIG_IDX(sig_num) ((uint32)(sig_num) / (8*sizeof(rex_sigs_type)))
#define SIG_OFFSET(sig_num) ((uint32)(sig_num) % (8*sizeof(rex_sigs_type)))
    
    /*--------------------------------------------------------------------------
      PS_CLR_SIG clears the signal sig_num within the mask pointed to by val_ptr
    ---------------------------------------------------------------------------*/
#define PS_CLR_SIG(val_ptr, sig_num)\
    { \
      *(((rex_sigs_type *)(val_ptr)) + SIG_IDX(sig_num)) &= \
        ~((rex_sigs_type) 1 << SIG_OFFSET(sig_num)); \
    }
    
    /*--------------------------------------------------------------------------
      PS_SET_SIG sets the signal sig_num in the mask pointed to by val_ptr
    ---------------------------------------------------------------------------*/
#define PS_SET_SIG(val_ptr, sig_num)\
    {\
      *(((rex_sigs_type *)(val_ptr)) + SIG_IDX(sig_num)) |=  \
        ((rex_sigs_type) 1 << SIG_OFFSET(sig_num)); \
    }
    
    /*--------------------------------------------------------------------------
      PS_IS_SET_SIG verifies if signal sig_num has been set in the mask pointed
      to by val_ptr
    ---------------------------------------------------------------------------*/
#define PS_IS_SET_SIG(val_ptr, sig_num)\
       (*(((rex_sigs_type *)(val_ptr)) + SIG_IDX(sig_num)) & \
           ((rex_sigs_type) 1 << SIG_OFFSET(sig_num)))

/*--------------------------------------------------------------------------
  PS_MAX_SIG_ARR_LEN indicates the array length corresponding to the maximum 
  number of PS signals required.
  For eg. if 
  0 <= PS_MAX_SIGNALS < 32, PS_MAX_SIG_ARR_LEN = 1
  32 <= PS_MAX_SIGNALS < 64, PS_MAX_SIG_ARR_LEN = 2
  64 <= PS_MAX_SIGNALS < 96, PS_MAX_SIG_ARR_LEN = 3
  and so on.
  We add 1 so that PS_EXT_SIG_ARR_LEN is always > 0, i.e. even if extension
  signal array is not required as we cant have an array of size 0.
---------------------------------------------------------------------------*/
#define PS_MAX_SIG_ARR_LEN \
((((uint32)PS_MAX_SIGNALS / (sizeof(rex_sigs_type)*8)) + \
(((uint32)PS_MAX_SIGNALS % (sizeof(rex_sigs_type)*8))? 1:0)) + 1)

/*--------------------------------------------------------------------------
  PS_SIG_ARR_LEN specifies the array length required for normal signals (not 
  extension signals).
---------------------------------------------------------------------------*/
#ifdef FEATURE_REX_SIGS_EXT
#define PS_SIG_ARR_LEN REX_SIGS_ARRAY_LEN
#else
#define PS_SIG_ARR_LEN 1
#endif

/*--------------------------------------------------------------------------
  PS_EXT_SIG_ARR_LEN specifies the length of the PS extension signal mask 
  array. If REX extension signals are supported, we need PS extension 
  mechanism only if the maximum number of PS signals exceeds that supported 
  by REX extension mechanism. Otherwise we set PS extension signal array 
  length to be zero.
  The added 1 in PS_MAX_SIG_ARR_LEN ensures that this array size would always
  be > 0.
---------------------------------------------------------------------------*/
#define PS_EXT_SIG_ARR_LEN (PS_MAX_SIG_ARR_LEN - PS_SIG_ARR_LEN)

/*--------------------------------------------------------------------------
  Minimum signal value which lies in the extension signal range.
---------------------------------------------------------------------------*/
#define PS_MIN_EXT_SIGNAL (sizeof(rex_sigs_type)*PS_SIG_ARR_LEN*8)

/*--------------------------------------------------------------------------
  EXT_SIG_IDX maps the signal number to the index in the ps_ext_sig_mask 
  array.
---------------------------------------------------------------------------*/
#define EXT_SIG_IDX(sig_num) SIG_IDX(sig_num) - PS_SIG_ARR_LEN

/*--------------------------------------------------------------------------
  PS_SET_EXT_SIG sets the bit in the ps_ext_sig_mask array corresponding to 
  the signal number.
---------------------------------------------------------------------------*/
#define PS_SET_EXT_SIG(sig_num) \
{\
  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);\
  *(((rex_sigs_type *)(ps_ext_sig_mask)) + EXT_SIG_IDX(sig_num)) |= \
    ((rex_sigs_type) 1 << SIG_OFFSET(sig_num));\
  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);\
}

/*--------------------------------------------------------------------------  
  PS_CLR_EXT_SIG clears the bit corresponding to the specified signal in the
  ps_ext_sig_mask array.
---------------------------------------------------------------------------*/
#define PS_CLR_EXT_SIG(sig_num) \
{ \
  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);\
  *(((rex_sigs_type *)(ps_ext_sig_mask)) + EXT_SIG_IDX(sig_num)) &= \
    ~ ((rex_sigs_type) 1 << SIG_OFFSET(sig_num));\
  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);\
}

/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/
typedef struct
{
  ps_sig_handler_type sig_handler;
  void *user_data_ptr;
} psi_sig_handler_info_type;

extern psi_sig_handler_info_type psi_sig_handler[PS_MAX_SIGNALS];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
extern rex_sigs_type psi_enabled_sig_mask[PS_MAX_SIG_ARR_LEN];
                                         
/*--------------------------------------------------------------------------  
  This array stores the extension signal. It extends the signal limit upto 
  the maximum specified via PS_MAX_SIGNALS.  
  This definition is common irrespective of whether FEATURE_REX_SIGS_EXT is
  turned on or off. The array length (PS_EXT_SIG_ARR_LEN) is calculated such
  as to account for the difference.
---------------------------------------------------------------------------*/
extern rex_sigs_type ps_ext_sig_mask[PS_EXT_SIG_ARR_LEN];
                                         

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the 
  signals which do not have a registered handler. It merely prints a warning 
  message.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean default_sig_handler
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */ 
);


/*===========================================================================

FUNCTION PS_START_TIMER

DESCRIPTION
  This function will set the specified timer to the specified duration. The
  time units are in milliseconds.

DEPENDENCIES
  The timer should already be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void ps_start_timer
(
  rex_timer_type  *timer_ptr,            /* ptr to timer to be started     */
  dword duration                         /* time interval, in msecs        */
);


/*===========================================================================

FUNCTION PS_CANCEL_TIMER

DESCRIPTION
  This function will cancel the specified timer and the associated signal
  mask will also be cleared. NOTE that the signals are associated with
  timers during initialization by the REX_DEF_TIMER function.

DEPENDENCIES
  The timer should already be initialized. The signal associated with the
  timer should be a PS task signal.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void ps_cancel_timer
(
  rex_timer_type  *timer_ptr,            /* ptr to timer to be cancelled   */
  ps_sig_enum_type sig                   /* signal associated with a timer */
);


#endif /* PSI_H */
