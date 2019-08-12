
/*
WARNING: This file is auto-generated.

Generated using: asm_autogen.pl
Generated from:  v3.0.63 of RFDevice_ASM.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2009, 2010, 2011, 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1/Main/modem_proc/rfdevice_asm/etc/asm_autogen.pl#1 n

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#ifdef FEATURE_RF_HAS_TP_CARDS
#error code not present
#endif

#include "rfdevice_asm_cxa4422agc_data_ag.h"
#include "rfdevice_asm_acpm7910_data_ag.h"
#include "rfdevice_asm_acpm7915_data_ag.h"
#include "rfdevice_asm_qm77003_data_ag.h"
#include "rfdevice_asm_qm77002_data_ag.h"
#include "rfdevice_asm_qm75001_data_ag.h"
#include "rfdevice_asm_cxa4422gc_0_data_ag.h"
#include "rfdevice_asm_cxm3658k_data_ag.h"
#include "rfdevice_asm_cxm3659k_data_ag.h"
#include "rfdevice_asm_cxm3653er_data_ag.h"
#include "rfdevice_asm_cxm3653er_3_data_ag.h"
#include "rfdevice_asm_cxa4414gc_data_ag.h"
#include "rfdevice_asm_cxa4414gc_es_data_ag.h"
#include "rfdevice_asm_lmspfbqh_h08_data_ag.h"
#include "rfdevice_asm_tqf6297h_data_ag.h"
#include "rfdevice_asm_lmspfbqh_f65_data_ag.h"
#include "rfdevice_asm_lmsp32qq_f31_data_ag.h"
#include "rfdevice_asm_rfmd1658_data_ag.h"
#include "rfdevice_asm_rfmd1496a_0x20_data_ag.h"
#include "rfdevice_asm_rfmd1681_data_ag.h"
#include "rfdevice_asm_rfmd1498a_0x28_data_ag.h"
#include "rfdevice_asm_rfmd1607_data_ag.h"
#include "rfdevice_asm_rfmd1654a_data_ag.h"
#include "rfdevice_asm_rfmd1660p_data_ag.h"
#include "rfdevice_asm_d5295_data_ag.h"
#include "rfdevice_asm_rfmd1656_data_ag.h"
#include "rfdevice_asm_rfmd1655_data_ag.h"
#include "rfdevice_asm_rfmd1680_data_ag.h"
#include "rfdevice_asm_rfmd1890a_data_ag.h"
#include "rfdevice_asm_xmss1t3g0pa_011_data_ag.h"
#include "rfdevice_asm_xmss1t3g0pa_069_data_ag.h"
#include "rfdevice_asm_xmss1t3g0pa_013_data_ag.h"
#include "rfdevice_asm_xmssgd3g0pa_005_data_ag.h"
#include "rfdevice_asm_xmssgf3g0pa_009_data_ag.h"
#include "rfdevice_asm_lmsp2pqd_g22_data_ag.h"
#include "rfdevice_asm_lmsp2pqd_g89_data_ag.h"
#include "rfdevice_asm_lmsp2qqd_h46_data_ag.h"
#include "rfdevice_asm_lmsp32qp_e10_data_ag.h"
#include "rfdevice_asm_lmsp32qp_f03_data_ag.h"
#include "rfdevice_asm_lmsp2pqp_h82_data_ag.h"
#include "rfdevice_asm_lmsp2pqp_h88_data_ag.h"
#include "rfdevice_asm_lmsp2pqp_j08_data_ag.h"
#include "rfdevice_asm_cxm3641er_data_ag.h"
#include "rfdevice_asm_cxa4416gc_data_ag.h"
#include "rfdevice_asm_cxm3655k_data_ag.h"
#include "rfdevice_asm_cxm3656k_data_ag.h"
#include "rfdevice_asm_cxm3654ur_data_ag.h"
#include "rfdevice_asm_cxm3642k_data_ag.h"
#include "rfdevice_asm_cxm3637k_data_ag.h"
#include "rfdevice_asm_cxm3637ak_data_ag.h"
#include "rfdevice_asm_cxm3643k_es03_data_ag.h"
#include "rfdevice_asm_cxm3643k_es02_data_ag.h"
#include "rfdevice_asm_cxm3640k_data_ag.h"
#include "rfdevice_asm_cxm3641k_data_ag.h"
#include "rfdevice_asm_cxm3657k_data_ag.h"
#include "rfdevice_asm_cxm3657k_ts2_data_ag.h"
#include "rfdevice_asm_cxm3632er_data_ag.h"
#include "rfdevice_asm_cxm3617er_data_ag.h"
#include "rfdevice_asm_cxm3807k_data_ag.h"
#include "rfdevice_asm_dgm366_data_ag.h"
#include "rfdevice_asm_hfqswejua224_data_ag.h"
#include "rfdevice_asm_murata_hfqswkcua212_20c_data_ag.h"
#include "rfdevice_asm_murata_hfqswkcua212_23e_data_ag.h"
#include "rfdevice_asm_lmsp2pqp_h23temp_data_ag.h"


rfdevice_asm_data* rfdevice_asm_data_create (uint16 mfg_id, uint8 prd_id, uint8 prd_rev)
{
  rfdevice_asm_data * asm_data = NULL;

  if ( mfg_id ==  0x01B0 && prd_id == 0x35  && prd_rev == 2)
  {
    asm_data = rfdevice_asm_cxa4422agc_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x107 && prd_id == 0x03  && prd_rev == 3)
  {
    asm_data = rfdevice_asm_acpm7910_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x107 && prd_id == 0x83  && prd_rev == 3)
  {
    asm_data = rfdevice_asm_acpm7915_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x2B  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_qm77003_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0xCF  && prd_rev == 1)
  {
    asm_data = rfdevice_asm_qm77002_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x286 && prd_id == 0x1C  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_qm75001_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x23  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxa4422gc_0_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x2D  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3658k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x2E  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3659k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x26  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3653er_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x00  && prd_rev == 3)
  {
    asm_data = rfdevice_asm_cxm3653er_3_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x02  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxa4414gc_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x00  && prd_rev == 1)
  {
    asm_data = rfdevice_asm_cxa4414gc_es_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x11  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_lmspfbqh_h08_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x286 && prd_id == 0x1C  && prd_rev == 1)
  {
    asm_data = rfdevice_asm_tqf6297h_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x06  && prd_rev == 3)
  {
    asm_data = rfdevice_asm_lmspfbqh_f65_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x0E  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_lmsp32qq_f31_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x25  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_rfmd1658_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x20  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_rfmd1496a_0x20_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x22  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_rfmd1681_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x28  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_rfmd1498a_0x28_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x1F  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_rfmd1607_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x2C  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_rfmd1654a_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x1A  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_rfmd1660p_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x248 && prd_id == 0x7E  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_d5295_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x24  && prd_rev == 1)
  {
    asm_data = rfdevice_asm_rfmd1656_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x24  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_rfmd1655_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x21  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_rfmd1680_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x28  && prd_rev == 0x01)
  {
    asm_data = rfdevice_asm_rfmd1890a_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x8B  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_xmss1t3g0pa_011_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x98  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_xmss1t3g0pa_069_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x83  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_xmss1t3g0pa_013_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x88  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_xmssgd3g0pa_005_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x81  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_xmssgf3g0pa_009_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x06  && prd_rev == 2)
  {
    asm_data = rfdevice_asm_lmsp2pqd_g22_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x020  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_lmsp2pqd_g89_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x018  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_lmsp2qqd_h46_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x09  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_lmsp32qp_e10_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x08  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_lmsp32qp_f03_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x25  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_lmsp2pqp_h82_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x22  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_lmsp2pqp_h88_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x26  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_lmsp2pqp_j08_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x13  && prd_rev == 1)
  {
    asm_data = rfdevice_asm_cxm3641er_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x01  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxa4416gc_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x21  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3655k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x22  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3656k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x20  && prd_rev == 1)
  {
    asm_data = rfdevice_asm_cxm3654ur_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x11  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3642k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x0E  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3637k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x1A  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3637ak_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x14  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3643k_es03_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x00  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3643k_es02_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x12  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3640k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x13  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3641k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x20  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3657k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x2C  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3657k_ts2_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x07  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3632er_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x06  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3617er_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01B0 && prd_id == 0x05  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_cxm3807k_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x03E && prd_id == 0x0A  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_dgm366_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x06  && prd_rev == 1)
  {
    asm_data = rfdevice_asm_hfqswejua224_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x0B  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_murata_hfqswkcua212_20c_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x023E && prd_id == 0x0A  && prd_rev == 0)
  {
    asm_data = rfdevice_asm_murata_hfqswkcua212_23e_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x020C && prd_id == 0x08  && prd_rev == 1)
  {
    asm_data = rfdevice_asm_lmsp2pqp_h23temp_data_ag::get_instance();
  }

#ifdef FEATURE_RF_HAS_TP_CARDS
  #error code not present
#endif

  return asm_data;
}

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

