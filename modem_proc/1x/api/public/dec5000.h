#ifndef DEC5000_H
#define DEC5000_H
/*===========================================================================

           D E C O D E R   ( S V D )    H E A D E R    F I L E

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA Serial Viterbi Decoder chip

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

  Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/MSM5100/vcs/dec5000.h_v   1.23   23 Oct 2002 14:25:42   vlechan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/dec5000.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/06/09   jtm     Removed featurization for CMI.
06/06/09   vlc     Added support for 1x Advanced (RC11.)
04/02/09   jtm     Removed featurization.
03/20/09   jtm     Cleaned up header file inclusions.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_DRIVERS.00.00.06
07/24/08   jtm     Modified DEC_DCCH_RC_INDEX to perform bounds checking.
07/16/08   jtm     Removed FEATURE_RXCCHAIN_DEBUG. Modified DEC_FCH_RC_INDEX
                   and DEC_SCH_RC_INDEX to perform bounds checking.
07/01/08   vlc     Merged code changes from old tip archive.
04/10/08   trc     Switch clock mgmt strategy via FEATURE_SRCH_CLKREGIM_RM
03/05/08   vlc     User clock resource manager to manage RX clocks
                   (FEATURE_CLKREGIM_RM.)
01/03/05   rkc     Added DEC_FCCCH_RAM_BASE.  Decoder Output Buffer offset for FCCCH
                   channel data. Value is offset in the Short RAM in Decoder Output
                   Buffer.
04/21/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
05/06/04   sr      Moved out DV driver code to separate file
09/03/03   dna     Ported for MSM6500
08/08/03   vlc     Added changes to move output buffer address of DCCH task to
                   decouple FCH/DCCH rate sets.
07/08/03   vlc     Added DEC_SCH_DUMMY_FULL_OB_ADDR.  This address is used
                   for Turbo SCH dummy task and is beyong any critical
                   decoder output buffer adresses.
01/21/03   vlc     Put DEC_TURBO_SCH_TASK define under release A feature for
                   clarity sake.
11/26/02   sr/vlc  Defined DEC_TURBO_SCH_TASK for MSM6100.
10/23/02   vlc     Increased NUM_TD_SCH_RATES to account for Turbo 32X rate
                   (applies to MSM6100 only.)
07/23/02   dlb2    Add EXTENDED_ENERGY_MATRIX support to CDMA2000 Rel. A code.
06/07/02   dlb2    Always use mask macros to write to VD_MODE_WB.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/21/02   vlc     Remove rate from parameter list to dec_read_dcch_data ().
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up
                   (feature FEATURE_IS2000_REL_A_VP2.)
03/27/02   ems     Featurized dec_read_scch_data for MSMHW_EXTENDED_ENERGY_MATRIX
02/25/02   sr      Merged in Release A common channels from the branch
01/23/02   dlb     Added extended energy matrix feature for MSM6050.
01/22/02   vlc     Added rate_set field to dec_sch_cfg_type, consolidated
                   dec_sch_cfg_type.
11/16/01   vlc     Added dec_clk_select_type and prototype for function
                   dec_clk_select ().
10/05/01   sr      Changed dec_fch_status_type's ser field from byte to word
08/08/01   vlc     Changed #include msmhw.h to #include msm.h to stay forward
                   compatible with future MSMs.
08/07/01   hrk     Modified prototype of dec_dtx_thresh_init().
07/03/01   baon    Added and cleaned up support for REALEASE_A / 32X
02/20/01   sr      Added support for 32X
01/10/01   tc      Added decoder CRC and quality bits.
12/13/00   vlc     Added #include msmhw.h to enable MSMHW_TURBO_CODING.
10/20/00   rmd     Merged with MSM5000 code. Moved bits to msm5XXXbits.h
09/06/00   rm      Added new function prototype of dec_read_sch_data
                   for Factory Test Mode.
09/06/00   djd/et  Added ifdef MSMHW_TURBO_CODING for turbo coding software.
07/12/00   rm      Added suuport for for 5ms frames and MULTIFRAME
                   INTERLEAVING.
05/18/00   et      added prototype for dec_dtx_thresh_init
03/24/00   bgc     Changes energy's to signed values.
02/16/00   eh      Changed DEC_MAX_SCC_NUM to 0x07
01/28/00   rm      Added support for 1X SCH.
01/26/00   rm      Modified the dec_read_sch_data fumction to read
                   each mux pdu into the buffer pointed by the dsm_item.
11/09/99   rm      Added in support for Turbo Codes.
10/20/99   rm      Added in support for IS95B SCCH.
10/01/99   rm      Externalized dec_sch_ob_params parameter.
09/24/99   rm      Added in support for SCH (RC3 & RC4).
07/31/99   rm      Created file. Interfaces to support Serial Viterbi Decoder

============================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* HACK FOR SNM */
typedef enum {
  DEC_RATE_9600    = 0x00, /* Select 9600 bps rate set   */
  DEC_RATE_14400   = 0x01, /* Select 14400 bps rate set  */
  DEC_RATE_INVALID = 0x02  /* Invalid rate set           */
} dec_rate_set_type;

typedef enum {
   DEC_RC1=0x01,
   DEC_RC2,
   DEC_RC3,
   DEC_RC4,
   DEC_RC5,
   DEC_RC11 = 11
} dec_rc_type;

/*  Decoder Supplemental Channel Rate */
typedef enum {
  DEC_SCH_RATE_1X,
  DEC_SCH_RATE_2X,
  DEC_SCH_RATE_4X,
  DEC_SCH_RATE_8X,
  DEC_SCH_RATE_16X,
  DEC_SCH_RATE_32X
} dec_sch_rate_type;


#endif /* DEC_5000_H */
