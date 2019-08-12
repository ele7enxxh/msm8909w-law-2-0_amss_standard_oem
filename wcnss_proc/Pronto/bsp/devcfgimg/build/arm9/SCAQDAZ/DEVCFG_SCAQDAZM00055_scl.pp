#line 1 "Z:\\b\\wcnss_proc\\build\\ms\\devconfig_data.scl"







 
#line 1 "Z:\\b\\wcnss_proc\\build\\ms\\devcfg_def.h"
#line 10 "Z:\\b\\wcnss_proc\\build\\ms\\devconfig_data.scl"

DEVCFG_CODE_SEG 0x0
{
   CODE_SEC +0x0
   {
      .ANY(+RO-CODE)
   }
   BSS_SEC +0x0
   {
      libspace.o(.bss)
   }
}

DEVCFG_DATA_SEG1 (0x8d50c000)
{
   DEVCFG_DATA_RW +0x0
   {
      devcfg_data.o (+RW-DATA, +FIRST)
      .ANY (+RW-DATA)
   }
}

DEVCFG_DATA_SEG2 (+0x0)
{
   DEVCFG_DATA_RO +0x0
   {
      .ANY (+RO-DATA)
   }
}
