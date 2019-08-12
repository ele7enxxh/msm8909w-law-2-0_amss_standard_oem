
/*
WARNING: This file is auto-generated.

Generated using: pa_autogen.pl
Generated from:  4.0.60 of RFDevice_PA.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2009 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/src/rfdevice_pa_factory_ag.cpp#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#ifdef FEATURE_RF_HAS_TP_CARDS
#error code not present
#endif

#include "rfdevice_pa_qorvo_rf5425_lb_data_ag.h"
#include "rfdevice_pa_qorvo_rf5425_mb_data_ag.h"
#include "rfdevice_pa_sky_77772_data_ag.h"
#include "rfdevice_pa_sky_77773_data_ag.h"
#include "rfdevice_pa_sky_77358_11_data_ag.h"
#include "rfdevice_pa_sky_77358_11_es01_data_ag.h"
#include "rfdevice_pa_sky_77358_11_0000_data_ag.h"
#include "rfdevice_pa_tqp_9051_data_ag.h"
#include "rfdevice_pa_2g_rf8108_0x04_20_data_ag.h"
#include "rfdevice_pa_acpm_9420_data_ag.h"
#include "rfdevice_pa_acpm_9417_data_ag.h"
#include "rfdevice_pa_acpm_9512_data_ag.h"
#include "rfdevice_pa_acpm_9517_data_ag.h"
#include "rfdevice_pa_acpm_7620_data_ag.h"
#include "rfdevice_pa_acpm_7910_data_ag.h"
#include "rfdevice_pa_acpm_7915_data_ag.h"
#include "rfdevice_pa_acpm_9428_data_ag.h"
#include "rfdevice_pa_acpm_9301_data_ag.h"
#include "rfdevice_pa_acpm_9413_data_ag.h"
#include "rfdevice_pa_acpm_9513_data_ag.h"
#include "rfdevice_pa_acpm_9411_data_ag.h"
#include "rfdevice_pa_acpm_9407_data_ag.h"
#include "rfdevice_pa_acpm_9341_data_ag.h"
#include "rfdevice_pa_acpm_9340_data_ag.h"
#include "rfdevice_pa_sky_77629_data_ag.h"
#include "rfdevice_pa_sky_77629_51_data_ag.h"
#include "rfdevice_pa_acpm_7600_data_ag.h"
#include "rfdevice_pa_qorvo_qm77003_data_ag.h"
#include "rfdevice_pa_qorvo_qm77002_data_ag.h"
#include "rfdevice_pa_qorvo_tqf6297h_hb_data_ag.h"
#include "rfdevice_pa_qorvo_rf6120_data_ag.h"
#include "rfdevice_pa_qorvo_qm52042_e3p1_data_ag.h"
#include "rfdevice_pa_qorvo_qm52042_e2p5_data_ag.h"
#include "rfdevice_pa_qorvo_qm52042_data_ag.h"
#include "rfdevice_pa_acpm_7650_data_ag.h"
#include "rfdevice_pa_rfmd_8013_data_ag.h"
#include "rfdevice_pa_rfmd_8013a_data_ag.h"
#include "rfdevice_pa_rfmd_8012a_data_ag.h"
#include "rfdevice_pa_rfmd_8017a_data_ag.h"
#include "rfdevice_pa_rfmd_8014_data_ag.h"
#include "rfdevice_pa_rfmd_8020_data_ag.h"
#include "rfdevice_pa_rfmd_8017_data_ag.h"
#include "rfdevice_pa_rfmd_8021_data_ag.h"
#include "rfdevice_pa_rfmd_8028_data_ag.h"
#include "rfdevice_pa_rfmd_8007_data_ag.h"
#include "rfdevice_pa_rfmd_8007a_data_ag.h"
#include "rfdevice_pa_rfmd_7907_data_ag.h"
#include "rfdevice_pa_rfmd_7913_data_ag.h"
#include "rfdevice_pa_rfmd_7917_data_ag.h"
#include "rfdevice_pa_rfmd_7920_data_ag.h"
#include "rfdevice_pa_rfmd_7941_data_ag.h"
#include "rfdevice_pa_rfmd_7941_es02_data_ag.h"
#include "rfdevice_pa_rfmd_7940_data_ag.h"
#include "rfdevice_pa_rfmd_7389_data_ag.h"
#include "rfdevice_pa_rfmd_7460_data_ag.h"
#include "rfdevice_pa_rfmd_7921_data_ag.h"
#include "rfdevice_pa_rfmd_qm58101_data_ag.h"
#include "rfdevice_pa_hb_qm75001_data_ag.h"
#include "rfdevice_pa_mlb_qpa4351_batt_data_ag.h"


rfdevice_pa_data* rfdevice_pa_data_create (uint16 mfg_id, uint8 prd_id, uint8 prd_rev)
{
  rfdevice_pa_data * pa_data = NULL;

  if ( mfg_id ==  0x0134 && prd_id == 0xFE  && prd_rev == 1)
  {
    pa_data = rfdevice_pa_qorvo_rf5425_lb_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0134 && prd_id == 0xFD  && prd_rev == 1)
  {
    pa_data = rfdevice_pa_qorvo_rf5425_mb_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01A5 && prd_id == 0x4E  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_sky_77772_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01A5 && prd_id == 0x4A  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_sky_77773_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01A5 && prd_id == 0x04  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_sky_77358_11_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01A5 && prd_id == 0x00  && prd_rev == 1)
  {
    pa_data = rfdevice_pa_sky_77358_11_es01_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01A5 && prd_id == 0x57  && prd_rev == 2)
  {
    pa_data = rfdevice_pa_sky_77358_11_0000_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0286 && prd_id == 0x0B  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_tqp_9051_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0134 && prd_id == 0x04  && prd_rev == 20)
  {
    pa_data = rfdevice_pa_2g_rf8108_0x04_20_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x04  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_acpm_9420_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x02  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_acpm_9417_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x0C  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_acpm_9512_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x02  && prd_rev == 2)
  {
    pa_data = rfdevice_pa_acpm_9517_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x01  && prd_rev == 2)
  {
    pa_data = rfdevice_pa_acpm_7620_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x03  && prd_rev == 3)
  {
    pa_data = rfdevice_pa_acpm_7910_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x83  && prd_rev == 3)
  {
    pa_data = rfdevice_pa_acpm_7915_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x08  && prd_rev == 2)
  {
    pa_data = rfdevice_pa_acpm_9428_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x07  && prd_rev == 2)
  {
    pa_data = rfdevice_pa_acpm_9301_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x03  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_acpm_9413_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x03  && prd_rev == 2)
  {
    pa_data = rfdevice_pa_acpm_9513_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x06  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_acpm_9411_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x05  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_acpm_9407_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x0A  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_acpm_9341_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x09  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_acpm_9340_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01A5 && prd_id == 0x09  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_sky_77629_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x01A5 && prd_id == 0x12  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_sky_77629_51_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x01  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_acpm_7600_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0xFF  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_qorvo_qm77003_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0xFF  && prd_rev == 1)
  {
    pa_data = rfdevice_pa_qorvo_qm77002_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x286 && prd_id == 0x1C  && prd_rev == 1)
  {
    pa_data = rfdevice_pa_qorvo_tqf6297h_hb_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x29  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_qorvo_rf6120_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x286 && prd_id == 0x10  && prd_rev == 3)
  {
    pa_data = rfdevice_pa_qorvo_qm52042_e3p1_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x286 && prd_id == 0x2C  && prd_rev == 2)
  {
    pa_data = rfdevice_pa_qorvo_qm52042_e2p5_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x286 && prd_id == 0x2C  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_qorvo_qm52042_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x0107 && prd_id == 0x01  && prd_rev == 1)
  {
    pa_data = rfdevice_pa_acpm_7650_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x11  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8013_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x16  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8013a_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x53  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8012a_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x1E  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8017a_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x12  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8014_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x14  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8020_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x13  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8017_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x15  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8021_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x52  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8028_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x18  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8007_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x38  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_8007a_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x17  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_7907_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x19  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_7913_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x1B  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_7917_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x1C  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_7920_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0xA8  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_7941_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x2B  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_7941_es02_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x2A  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_7940_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x30  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_7389_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x00  && prd_rev == 1)
  {
    pa_data = rfdevice_pa_rfmd_7460_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x1D  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_7921_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x134 && prd_id == 0x2F  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_rfmd_qm58101_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x286 && prd_id == 0x1C  && prd_rev == 0)
  {
    pa_data = rfdevice_pa_hb_qm75001_data_ag::get_instance();
  }

  else if ( mfg_id ==  0x217 && prd_id == 0x49  && prd_rev == 1)
  {
    pa_data = rfdevice_pa_mlb_qpa4351_batt_data_ag::get_instance();
  }

#ifdef FEATURE_RF_HAS_TP_CARDS
  #error code not present
#endif

  return pa_data;
}

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

