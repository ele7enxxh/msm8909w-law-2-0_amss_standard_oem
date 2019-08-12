#include "rpm_hwio.h"
#include "HALhwio.h"

void time_service_init(void)
{
  //allow read/write access to timer registers
  //TODO: make this write more obvious/indicative of whats going on
  HWIO_OUTI(RPM_QTMR_AC_CNTACR_n, 0, 33);

  //enable RPM QTMR
  //HWIO_OUTF(RPM_F0_QTMR_V1_CNTP_CTL, EN, 1);
}

