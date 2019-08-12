1. build_ro_decompressor.sh builds the ro decompressor in either Cygwin or Linux (tested on "CYGWIN_NT-6.1-WOW64 1.7.5 i686 Cygwin" and "GNU bash, version 4.2.25(1)-release x86_64-pc-linux-gnu")
2. q6zip_ro_decompressor.exe is the Windows executable version of the ro decompressor (tested on Windows 7 64 bits)
3. q6zip_ro_decompressor is the Linux executable version of the ro decompressor (tested on Linux 3.2.0-40-generic #64-Ubuntu SMP x86_64 GNU/Linux)
4. If the algorithms are changed for either RO or RW compression/decompression the version needs to be updated:
   - For RO, change the value of Q6ZIP_RO_ALGORITHM_VERSION q6zip_constants.py
   - For RW, change the variable Q6ZIP_RW_ALGORITHM_VERSION q6zip_constants.py
 
