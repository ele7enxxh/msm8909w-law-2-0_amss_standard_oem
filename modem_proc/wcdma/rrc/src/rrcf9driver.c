/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               M S M 5 2 0 0  F 9  ( I N T E G R I T Y   E N G I N E ) 
                              D R I V E R


GENERAL DESCRIPTION

  This is a driver for the F9 Engine that implements the Integrity
  Protection algorithm for WCDMA.

EXTERNALIZED FUNCTIONS

  
 
                    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  


  Copyright (c) 2002-2009 by Qualcomm Technologies, Inc.  
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcf9driver.c_v   1.9   18 Jun 2002 18:14:02   kiranc  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcf9driver.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/14   sp      Made changes to fix compilation errors
01/23/14   sr      Made changes to use msg_length in bits rather than bytes for security Kasumi api
01/06/14   sr      Made changes to extend Security API to Support Kasumi algorithm
07/08/13   ad      Made changes to remove redundant F3s
03/22/13   mp      Removed tmc.h include file
03/08/13   sg      Made changes to print the Keys properly and removed the code
                   to get F9 interrupt from HW.
12/12/12   mp      Fixing KW errors
11/2/12    sg      Made changes to remove the nv "rrc_nv_a2_power_opt" and remove
                   remove the code related to HW integrity protection.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12   zr      Adding feature definitions for Dime
07/04/12   ad      Made changes to use kasumi algo when integrity algo passed is invalid
05/30/12   rl      Made changes to support the usage of snow 3g algo for SW integrity.
09/05/12   md      Mainlined feature flags FEATURE_RRC_INTEGRITY_SW_ENGINE,
                   FEATURE_WCDMA_A2_POWER_COLLAPSE and FEATURE_WCDMA_NIKEL_SW
03/19/12   md      Featurized the code for enabling and disabling
                   F9 clocks for 8960
03/15/12   rl      Fixed the KW error
12/14/11   rl      Made changes to use the SW integrity for integrity opt NV value as 0 or 1
12/05/11   rl      Removed the A2 API calls
11/24/11   rl      Removed the unused A2 powerup/powerdown function definition
11/17/11   ad      Added featurization for A2 power optimization
11/15/11   rl      Made the changes for A2 power optimization.
10/28/11   md      Merged changes of Featurization of code for enabling and disabling
                   F9 clocks for 8960
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/09/10   rm      Fixed compiler warnings
07/05/10   ss      Fixed compiler warnnigs
06/24/10   ss      Cleanup of INTLOCK with WCDMA_INTLOCK
05/06/10   ss      Fixed Compilation error under feature FEATURE_WCDMA_SNOW3G_SECURITY
12/10/09   ss      Bug fix in Snow3G driver for Continuous mode operation
05/29/09   kp      Added support for new SNOW3g security Algo & single cipher algo usage across 
                   access stratum layers.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
01/28/09   dm      Fixed lint errors
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
09/02/08   cnp     Replaced intlock/free with wcdma_intlock/free.
           cnp     Replaced tasklock/free with wcdma_tasklock/free.
           cnp     Replaced intlock/free_sav with wcdma_intlock/free_sav.
06/25/08   kp      Fixed compiler warnings for MSM6240/6270.
02/04/08   kp      Enabled code for interrupt based polling of F9 engine for 
                   MSM6240/M6270 which is currently present only for MSM7600.
                   These interrupts are specific to 6240/6270 so code is under 
                   feature flag T_QSC6270
02/06/07   kp      Fixed compiler error for 7200.
01/31/07   kp      Added code for interrupt based polling of F9 engine for 
                   MSM7600.These interrupts are specific to 7600 so code is 
                   under feature flag T_MSM7600.
09/13/06   gfr     Use tramp driver to access IRQ registers.
04/26/06   vr      Changes for RVDS compiler.
08/02/05   vr      Lock Interrupts while writing data into the registers
                   for MAC-I calculation.Interleaving may result in inconsistent
                   results.
06/09/05   vr      Changed from software based polling to harware interrupt based
                   polling of F9 engine for Raven.So at this point both Saber
                   and Raven have hardware interrupt based polling.
                   For Saber:Interrupt register 1,interrupt bit 3
                   For Raven:Interrupt register 0,interrupt bit 7
05/13/05   vr      Separated rrcf9_generate_mac_for_message functionality for 
                   saber and raven  
04/26/05   vr      Fix for continous mode operation.In the last write 
                   wait till the write is "complete" before reading the MACI
                   from the register.Earlier we were waiting till "Not Busy"
                   to break out of the loop which is also logically right
                   but the new method would guarantee that the register is updated
                   before we read the value from it.
04/06/05   vr      Replaced WORD by SW_WORD and DWORD by SW_DWORD
03/24/05   kc      Merged changes to poll status register rather than Interrupt to 
                   figure out end of MAC-I computation.
01/21/05   sm      Changed interrupt based polling to STATUS register based 
                   polling for MAC-I calculation. Also did some clean-up in 
                   F9 driver's normal mode of operation.
11/12/04   kc      Fixed lint errors
08/19/04   sgk     Changed MSG_ERROR to ERR_FATAL in check for 
                   local_sdu_uint32_ptr NULL pointer to fix lint warning 
                   Possible use of null pointer 'local_sdu_uint32_ptr' in 
                   argument to operator 'unary *' and Possible use of null 
                   pointer 'local_sdu_uint32_ptr' in argument to operator '++'
03/05/04   kc      Added code to implement continuous mode 1 and 2 to handle
                   RRC SDU's greater than 2624 bits.
02/05/04   ih      Modified to use clk_regime_enable/disable(CLK_RGM_F9_M) 
                   to manage the F9 clock
09/17/03   kc      Removed FEATURE 6250 COMPILE.  Added Macro HWIO_ADDR to
                   convert register addresses compatible with Cheetah/Saber 
                   naming conventions.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
10/08/02   kc      Added code to disable F9 engine's clock when the F9 engine
                   is not being used.
08/27/02   kc      Fix for CR24192. Removed the FEATURE defn. for Integrity 
                   Disabling as it is now controlled through NV RAM
06/17/02   kc      Fixed Memory leak in S/W Integrity. Added code to support
                   Integrity Modify in Security Mode Command.
05/22/05   kc      Removed msm5200creg.h from incl files. it's no longer used 
05/17/05   kc      Added more MSG_HIGH's to get addnl. info from Interop test
05/15/05   rj      Modified coding style and naming conventions for SW
                   integrity engine
05/07/02   rj      Added code to implement F9 Algorithm in software.
04/24/02   kc      Included msm5200creg.h instead of msm6200reg.h file
04/23/02   kc      Fixed compilation problems when Integrity feature is not
                   enabled.
04/22/02   kc      Fixed bugs discovered during integration testing. 
04/10/02   kc      Created Module from RAM's initial file.Base build for 
                   Integrity Support. 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "rrcmmif.h"
#include "msm.h"
#include "msg.h"
#include "err.h"
#include "string.h"
#include "rrcf9driver.h"
#include "rrcdata_v.h"
#include "bit.h"
#include "stdio.h"
#include "math.h"
#include "rrcsmc.h"
#include "mutils_security.h"

//number of bits that can be written to R9 RAM at one shot.
#define RRC_SDU_WRITE_LEN_BITS         2624

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

uint8 f9_cs_key[16];
uint8 f9_ps_key[16];

uint32 f9_fresh;


/*===========================================================================

  FUNCTION rrcf9_init_fresh_per_connection

  DESCRIPTION

    This function performs initialization for the F9 device
    for each RRC connection. Each time an RRC connection
    is eastablished, once the input parameters are availabe
    this function should be called.

    The following are initialized by this function -
  
    FRESH - FRESH is a 32 bit value unique to a UE. It's given
    by the UTRAN in the security mode command.
   
   
  DEPENDENCIES

    None.

  RETURN VALUE

    None

===========================================================================*/
void rrcf9_init_fresh_per_connection
( 
  uint32 fresh_value      /* Value of Fresh */
)
{
  f9_fresh = fresh_value;
  WRRC_MSG1_HIGH("Fresh Value: 0x%x",  f9_fresh);
}/*rrcf9_init_fresh_per_connection*/
/*===========================================================================

  FUNCTION rrcf9_init_key_values 

  DESCRIPTION

    This function performs initialization for the F9 device
    for each RRC connection. Each time an RRC connection
    is eastablished, once the input parameters are availabe
    this function should be called.

    The following are initialized by this function -
  
    Integrity Key - 128 bit key used by the F9 engine. The
    key is different for CS connections and PS connections.
  
   
  DEPENDENCIES

    None.

  RETURN VALUE

    None

===========================================================================*/
void rrcf9_init_key_values
( 
  byte *cs_key,         /* Integrity Key value for PS domain */
  byte *ps_key          /* Integrity Key value for CS domain */
)
{
  uint32 count;

  for(count=0; count < (RRC_MAX_INTEGRITY_KEY_LENGTH); count++)
  {
    f9_cs_key[count] = cs_key[count];
  }

  for(count=0; count < (RRC_MAX_INTEGRITY_KEY_LENGTH); count++)
  {
    f9_ps_key[count] = ps_key[count];
  }
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"CS keys 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", 
  f9_cs_key[0], f9_cs_key[1], f9_cs_key[2], f9_cs_key[3], f9_cs_key[4], f9_cs_key[5], f9_cs_key[6], f9_cs_key[7]);
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"CS keys 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", 
  f9_cs_key[8], f9_cs_key[9], f9_cs_key[10], f9_cs_key[11], f9_cs_key[12], f9_cs_key[13], f9_cs_key[14], f9_cs_key[15]);
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"PS keys 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x ", 
  f9_ps_key[0], f9_ps_key[1], f9_ps_key[2], f9_ps_key[3], f9_ps_key[4], f9_ps_key[5], f9_ps_key[6], f9_ps_key[7]);
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"PS keys 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x ", 
  f9_ps_key[8], f9_ps_key[9], f9_ps_key[10], f9_ps_key[11], f9_ps_key[12], f9_ps_key[13], f9_ps_key[14], f9_ps_key[15]);

}/*rrcf9_init_key_values*/

/*===========================================================================

  FUNCTION RRCF9_GENERATE_MAC_FOR_MESSAGE

  DESCRIPTION

    This function passes the encoded RRC message and the
  additional parameters that are unique to the message 
  to the integrity engine. It returns the generated
  MAC-I value.
  
  This function is to be used for both downlink and 
  uplink messages.
  

  DEPENDENCIES

    None.

  RETURN VALUE

    A uint32 value that represents the MAC-I (or XMAC-I) for
  this message.

===========================================================================*/
uint32 rrcf9_generate_mac_for_message
( 
  uint32 count_i,                           /* COUNT-I value for the RB */
  rrc_integrity_direction_type dir,         /* Direction - uplink or downlink message */
  uint8 *msg,                               /* Pointer to the encoded message.    */
  uint32 msg_length,                        /* Length of the message in bits */
  rrc_cn_domain_identity_e_type  cn_domain,  /* Key for which domain_id to be used */
  uecomdef_wcdma_integrity_protection_algorithm_e_type integrity_algo /*integrity protection algorithm*/
)
{
  uint32 maci = 0 ;                        /* Local variable for MACI */
  uint8 *key_ptr=NULL,bearer=0;  

    if( cn_domain == RRC_CS_DOMAIN_CN_ID )
    {
    key_ptr = f9_cs_key;
    }
    else
    {
    key_ptr = f9_ps_key;
    }
    
  if(integrity_algo != UE_WCDMA_INTEGRITY_PROT_ALGO_UIA2)
  {
  mutils_security_stream_compute_integrity_maci(
  MUTILS_TECHNOLOGY_WCDMA,	  /*!< Technology */
     MUTILS_INTEGRITY_ALGO_KASUMI,                /*! <Integrity Algorithm  */  
  key_ptr,					  /*! Input key*/
  msg,						  /*! < Input Msg ptr  */
  msg_length, 				  /*! < Input msg len  */
  &maci,						  /*!<Output MAC-I ptr */
  bearer, 					  /*!< Bearer ID ranges from 0 to 31*///TBD as this is not used in case of snow-3G
  f9_fresh,					  /*!< Fresh value */
  count_i,					  /*!< Count */
 (mutils_integrity_direction_t) dir						  /*!< UL = 0, DL = 1*/
  );
  }
#ifdef FEATURE_WCDMA_SNOW3G_SECURITY
  else if(integrity_algo == UE_WCDMA_INTEGRITY_PROT_ALGO_UIA2)
  {
    mutils_security_stream_compute_integrity_maci(
     MUTILS_TECHNOLOGY_WCDMA,                     /*! < Technology */
     MUTILS_INTEGRITY_ALGO_SNOW3G,                /*! <Integrity Algorithm  */  
     key_ptr,                                     /*! Input key*/
     msg,                                         /*! < Input Msg ptr  */
     msg_length,                                  /*! < Input msg len  */
     &maci,                                       /*! <Output MAC-I ptr */
     bearer,                                      /*! < Bearer ID ranges from 0 to 31*///TBD as this is not used in case of snow-3G
     f9_fresh,                                    /*! < Fresh value */
     count_i,                                     /*! < Count */
    (mutils_integrity_direction_t) dir                                         /*! < UL = 0, DL = 1*/
      );
  }
#endif
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Fresh = 0x%x, DIR %d, Count_I 0x%x, maci = 0x%x, CN Doamin = %d,Integrity Algo = %x[0=Invalid,1=UIA1,2=UIA2] in rrcsmc_int_ciph_info, msg_length_bits:%d",
  f9_fresh, dir, count_i, maci,cn_domain,integrity_algo,msg_length,0,0); 

  return(maci);
}


