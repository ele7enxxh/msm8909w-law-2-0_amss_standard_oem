#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/build/sampleapp.scl"
TZ_TEST_APP  0 PI
{  
  TZ_APP_CODE +0  
  {
    tzapp_entry.o (TZAPPENTRYCODE, +FIRST)
    *(+RO)
   }
}
TZ_TEST_APP_DATA +0 ALIGN 4096
{
  TZ_APP_RW +0
  {
    * (+RW, +ZI)
  }
  TZ_APP_STACK +0 EMPTY (0x8000)
  {
  }
  TZ_APP_HEAP +0 EMPTY (0x8000)
  {
  }
}
