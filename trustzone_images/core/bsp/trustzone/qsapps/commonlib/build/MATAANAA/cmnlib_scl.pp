#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/securemsm/trustzone/qsapps/libs/applib/common/build/commonlib.scl"
CMN_TZLIB  0 PI
{  
  TZ_APP_CODE +0  
  {
    cmnlib_entry.o (CMNLIBENTRYCODE, +FIRST)
    *(+RO)
   }
}
CMN_TZLIB_DATA +0 ALIGN 4096
{
  TZ_APP_RW +0
  {
    * (+RW, +ZI)
  }
}

