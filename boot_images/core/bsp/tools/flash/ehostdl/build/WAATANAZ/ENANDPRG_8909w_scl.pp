#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/storage/flash/tools/src/hostdl/hostdl_in.scl"































 

RAM 0x08006050
{
   CODE 0x08006050 
   {
      "hostdl_startup.o" (BootRom, +FIRST)
   }

   CODE_ALL +0
   {
      * (+RO)
   }

   APP_RAM +0x0
   {

      * (.constdata)   ; Relocate all constdata to RAM since accessing
                       ; string literals and other unaligned
                       ; data in ADSP IRAM could cause issues
      * (+RW, +ZI)
   }

   ZI_RAM  0x00210000
   {
      qhsusb_*.o (+ZI)
      * (FLASH_TOOLS_HEAP_ZONE)
   }
}
