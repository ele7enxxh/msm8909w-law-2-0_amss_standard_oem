/*============================================================================
  FILE:         GRLCGAN.C

  OVERVIEW:     GRLC Generic Access

                Provides support for Generic Access

  FUNCTION SUMMARY

                grlc_send_grr_gan_ps_access_cnf
                grlc_send_grr_no_gan_ps_access_cnf
                grlc_gan_handle_gu_ps_connect_req
                grlc_gan_process_ack
                grlc_gan_xfer
                grlc_gan_rove_in
                grlc_gan_status_ind

  DEPENDENCIES: Requires RLC uplink data structures

                Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcgan.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
20/08/13   hv      CR531564
02/19/08   hv      Created

===========================================================================*/


#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */


  /* If FEATURE_GAN is undefined, a compiler warning will result because this module
  ** will contain no code - define a dummy function here
  */
  void grlcgan_dummy(void) { }

