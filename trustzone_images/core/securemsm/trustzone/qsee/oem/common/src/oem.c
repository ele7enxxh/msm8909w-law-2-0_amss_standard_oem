#include "comdef.h"

extern void qsee_wdog_start(unsigned long long bark_timeout, unsigned long long bite_timeout);
extern void qsee_wdog_stop(void);
extern qsee_enable_wdog_interrupt(void);

void oem_init(void)
{

#if 0
  /*OEMs need to modify this function.*/
  unsigned long long bark_timeout = 3000;  /*ms*/
  unsigned long long bite_timeout = 120000;  /*ms*/

  qsee_enable_wdog_interrupt();

  qsee_wdog_start(bark_timeout, bite_timeout);

  qsee_wdog_stop();

#endif
}

unsigned int qsee_oem_process_cmd(unsigned char *input, unsigned int input_len, unsigned char *output, unsigned int output_len)
{
  //To be filled out by OEMs
  return 0;
}

/* For OEM to set. If it returns TRUE, then the Secure File System will enable its version 
 * anti-rollback feature which uses RPMB. Else, if FALSE is returned this feature will be 
 * disabled and there will be no dependency on RPMB be SFS, and consequently there will be
 * no version anti-rollback protection on the Secure File System Counter File. 
 * WARNING: This opens up SFS to rollack attacks causing security degration. This feature
 * was implemented to stop rollback attacks and by disabling you are making the decision to
 * decrease security and open vulnerabilities.*/
boolean tzbsp_counter_enable_rpmb(void)
{
  return TRUE;
}

/* Note !!! This funciton is used for the below condition only: !!!
  A customer's refurbish device's eMMC is replaced, since RPMB key provision fuse is blown,
  RPMB auto production key provision won't happen for new eMMC.
  The return TRUE value can override RPMB key provision fuse and let RPMB be provsioned with
  new eMMC.
  This functions MUST always return FALSE for production devices. 
  Otherwise the security of the RPMB will be compromised */
boolean oem_allow_rpmb_key_provision(void)
{
  return FALSE;
}
