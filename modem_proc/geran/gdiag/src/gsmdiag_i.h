#ifndef GSMDIAG_I_H
#define GSMDIAG_I_H
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdiag/src/gsmdiag_i.h#1 $ */
#include "customer.h"
#include "sys.h"
#ifdef FEATURE_DUAL_SIM
/* MMCP doesn't extern these correctly, so they are defined here.
   Moved out the .c files to avoid "extern in .c file" internal SA warning */

extern byte cmlog_get_system_mode_per_subs(sys_modem_as_id_e_type);
extern byte cmlog_get_call_state_per_subs(sys_modem_as_id_e_type);
#endif /* FEATURE_DUAL_SIM */

#endif
